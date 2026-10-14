#!/usr/bin/env python3
"""
Pruebas en caja negra para Codexion.

Uso:
    python3 test_codexion.py [ruta_al_binario]

Por defecto usa ./codexion. Solo lee el stdout de tu programa y comprueba
formato de logs, ausencia de agotamientos indebidos, no-duplicacion de
dongles compartidos entre vecinos, respeto del cooldown, y precision del
agotamiento. No sustituye a valgrind/helgrind ni a la revision de codigo
(ver notas al final).
"""
import subprocess
import re
import sys

BIN = sys.argv[1] if len(sys.argv) > 1 else "./codexion"

LOG_RE = re.compile(
    r'^(\d+) (\d+) (has taken a dongle|is compiling|is debugging|is refactoring|burned out)$'
)

GREEN, RED, YELLOW, RESET = "\033[92m", "\033[91m", "\033[93m", "\033[0m"
results = []


def report(name, ok, detail=""):
    status = f"{GREEN}PASS{RESET}" if ok else f"{RED}FAIL{RESET}"
    print(f"[{status}] {name}" + (f" -- {detail}" if detail else ""))
    results.append(ok)


def info(msg):
    print(f"[{YELLOW}INFO{RESET}] {msg}")


def run(args, timeout=10):
    try:
        return subprocess.run([BIN, *map(str, args)], capture_output=True,
                               text=True, timeout=timeout)
    except subprocess.TimeoutExpired as e:
        return e


def parse_log(stdout):
    lines = stdout.strip("\n").split("\n") if stdout.strip() else []
    events, malformed = [], []
    for line in lines:
        m = LOG_RE.match(line)
        if m:
            events.append((int(m.group(1)), int(m.group(2)), m.group(3)))
        else:
            malformed.append(line)
    return events, malformed


def per_coder_events(events, n):
    pc = {i: [] for i in range(1, n + 1)}
    for ts, coder, state in events:
        if coder in pc:
            pc[coder].append((ts, state))
    return pc


# ---------- T1: validacion de argumentos ----------
def test_invalid_args():
    print("\n== T1: Validacion de argumentos ==")
    cases = {
        "sin argumentos": [],
        "argumentos insuficientes": [4, 800, 200, 200, 200],
        "numero de coders negativo": [-1, 800, 200, 200, 200, 3, 100, "fifo"],
        "numero de coders no entero": ["abc", 800, 200, 200, 200, 3, 100, "fifo"],
        "time_to_burnout negativo": [4, -800, 200, 200, 200, 3, 100, "fifo"],
        "scheduler invalido": [4, 800, 200, 200, 200, 3, 100, "roundrobin"],
        "number_of_coders cero": [0, 800, 200, 200, 200, 3, 100, "fifo"],
        "valor no entero (float)": [4, "800.5", 200, 200, 200, 3, 100, "fifo"],
        "un coder solo": [1, 800, 200, 200, 200, 2, 100, "fifo"],
    }
    for name, args in cases.items():
        if name == "un coder solo":
            continue  # se prueba aparte en T2b
        p = run(args, timeout=3)
        if isinstance(p, subprocess.TimeoutExpired):
            report(f"rechaza: {name}", False, "el programa se quedo colgado en vez de rechazar la entrada")
            continue
        crashed = p.returncode < 0
        rejected = p.returncode != 0
        report(f"rechaza: {name}", rejected and not crashed,
               f"returncode={p.returncode}" + (" (crash/senal!)" if crashed else ""))


# ---------- T2: simulacion basica ----------
def test_basic_run(n=4, burnout=2000, compile_t=200, debug_t=150,
                    refactor_t=150, required=3, cooldown=50, sched="fifo"):
    print(f"\n== T2: Simulacion basica sin contencion critica (n={n}, sched={sched}) ==")
    args = [n, burnout, compile_t, debug_t, refactor_t, required, cooldown, sched]
    est = required * (compile_t + debug_t + refactor_t) * 3 / 1000 + 3
    p = run(args, timeout=est)
    if isinstance(p, subprocess.TimeoutExpired):
        report("termina en tiempo razonable", False, f"colgado tras {est:.1f}s")
        return None

    report("codigo de salida 0", p.returncode == 0, f"returncode={p.returncode}")
    if p.stderr.strip():
        info(f"stderr no vacio: {p.stderr.strip()[:200]}")

    events, malformed = parse_log(p.stdout)
    report("todas las lineas de log tienen formato valido (sin mezclarse)",
           len(malformed) == 0, f"{len(malformed)} lineas malformadas: {malformed[:3]}" if malformed else "")

    burned = [e for e in events if e[2] == "burned out"]
    report("nadie se agota con parametros viables", len(burned) == 0,
           f"{len(burned)} agotamiento(s): {burned}")

    pc = per_coder_events(events, n)
    compiles_ok, seq_ok = True, True
    bad_seq_detail = ""
    for c, evs in pc.items():
        states = [s for _, s in evs]
        if states.count("is compiling") < required:
            compiles_ok = False
        i = 0
        expected = ["has taken a dongle", "has taken a dongle", "is compiling",
                    "is debugging", "is refactoring"]
        while i < len(states) and states[i] != "burned out":
            chunk = states[i:i + 5]
            if chunk != expected[:len(chunk)]:
                seq_ok = False
                bad_seq_detail = f"coder {c}: {states[max(0,i-2):i+5]}"
                break
            i += 5
    report("cada coder alcanza >= number_of_compiles_required compilaciones", compiles_ok)
    report("secuencia de estados correcta por coder (taken,taken,compiling,debugging,refactoring)",
           seq_ok, bad_seq_detail)

    return events, args


# ---------- T2b: caso de un solo coder ----------
def test_single_coder(burnout=2000, compile_t=200, debug_t=150, refactor_t=150,
                       required=2, cooldown=50, sched="fifo"):
    print("\n== T2b: Caso limite n=1 (un solo dongle sobre la mesa) ==")
    args = [1, burnout, compile_t, debug_t, refactor_t, required, cooldown, sched]
    est = required * (compile_t + debug_t + refactor_t) * 3 / 1000 + 3
    p = run(args, timeout=est)
    if isinstance(p, subprocess.TimeoutExpired):
        report("termina con n=1", False, "colgado")
        return
    report("codigo de salida 0 con n=1", p.returncode == 0, f"returncode={p.returncode}")
    events, malformed = parse_log(p.stdout)
    report("logs validos con n=1", len(malformed) == 0)
    burned = [e for e in events if e[2] == "burned out"]
    report("nadie se agota con n=1 y parametros viables", len(burned) == 0)


# ---------- T3: no duplicacion de dongles (vecinos no compilan a la vez) ----------
def test_adjacency(events, n):
    print("\n== T3: No hay duplicacion de dongles (vecinos no compilan simultaneamente) ==")
    if events is None:
        report("no aplica (T2 fallo)", False)
        return
    pc = per_coder_events(events, n)
    compiling_intervals = {i: [] for i in range(1, n + 1)}
    for c, evs in pc.items():
        for i, (ts, state) in enumerate(evs):
            if state == "is compiling":
                for ts2, st2 in evs[i + 1:]:
                    if st2 == "is debugging":
                        compiling_intervals[c].append((ts, ts2))
                        break

    violations = []
    for c in range(1, n + 1):
        neighbor = c % n + 1
        if neighbor == c:
            continue
        for (s1, e1) in compiling_intervals[c]:
            for (s2, e2) in compiling_intervals[neighbor]:
                if s1 < e2 and s2 < e1:
                    violations.append((c, neighbor, (s1, e1), (s2, e2)))
    report("ningun par de vecinos (que comparten dongle) compila a la vez",
           len(violations) == 0, f"{len(violations)} solapamiento(s), ej: {violations[:2]}")


# ---------- T4: cooldown ----------
def test_cooldown(events, n, cooldown):
    print("\n== T4: Respeto del dongle_cooldown ==")
    if events is None:
        report("no aplica (T2 fallo)", False)
        return
    pc = per_coder_events(events, n)
    releases = {i: [] for i in range(1, n + 1)}
    for c, evs in pc.items():
        for i, (ts, state) in enumerate(evs):
            if state == "is compiling":
                for ts2, st2 in evs[i + 1:]:
                    if st2 == "is debugging":
                        releases[c].append(ts2)
                        break

    violations = []
    for c in range(1, n + 1):
        neighbor = c % n + 1
        if neighbor == c:
            continue
        rel_times = releases[c]
        acquire_times = [ts for ts, st in pc[neighbor] if st == "is compiling"]
        for at in acquire_times:
            prior = [r for r in rel_times if r <= at]
            if prior:
                last_release = max(prior)
                gap = at - last_release
                if gap < cooldown:
                    violations.append((neighbor, c, at, last_release, gap))
    report(f"el dongle compartido respeta el cooldown ({cooldown}ms) antes de reusarse",
           len(violations) == 0, f"{len(violations)} violacion(es), ej: {violations[:2]}")


# ---------- T5: precision del agotamiento ----------
def test_burnout(n=2, burnout=300, compile_t=200, debug_t=200, refactor_t=100,
                  required=100, cooldown=50, sched="fifo"):
    print("\n== T5: Se produce y se loguea el agotamiento correctamente ==")
    # compile_t + debug_t (400) > burnout (300) => el agotamiento es inevitable
    args = [n, burnout, compile_t, debug_t, refactor_t, required, cooldown, sched]
    p = run(args, timeout=6)
    if isinstance(p, subprocess.TimeoutExpired):
        report("el programa se detiene tras el agotamiento", False, "colgado, no se detuvo")
        return
    events, malformed = parse_log(p.stdout)
    burned = [e for e in events if e[2] == "burned out"]
    report("se produce al menos un agotamiento (esperado con estos parametros)", len(burned) > 0)
    report("no hay lineas malformadas / logs mezclados durante el agotamiento",
           len(malformed) == 0, f"{malformed[:3]}" if malformed else "")
    report("codigo de salida 0 tras detectar el agotamiento", p.returncode == 0,
           f"returncode={p.returncode}")
    if burned:
        info(f"timestamp de agotamiento logueado: {burned[0][0]}ms (revisa a mano que tu "
             f"reloj real coincide con esto en +/-10ms; el script no puede medirlo desde fuera)")


# ---------- T6: informativo, diferencia fifo vs edf bajo contencion ----------
def test_scheduler_diff(n=6, burnout=5000, compile_t=150, debug_t=100,
                         refactor_t=100, required=4, cooldown=50):
    print("\n== T6 (informativo): fifo vs edf bajo contencion ==")
    for sched in ("fifo", "edf"):
        args = [n, burnout, compile_t, debug_t, refactor_t, required, cooldown, sched]
        est = required * (compile_t + debug_t + refactor_t) * 4 / 1000 + 3
        p = run(args, timeout=est)
        if isinstance(p, subprocess.TimeoutExpired):
            info(f"{sched}: colgado, revisar a mano")
            continue
        events, _ = parse_log(p.stdout)
        burned = [e for e in events if e[2] == "burned out"]
        info(f"{sched}: returncode={p.returncode}, agotamientos={len(burned)}, "
             f"lineas de log={len(events)}")
    info("este test es orientativo: confirmar a mano que, con deadlines distintos, "
         "edf prioriza al coder con deadline mas cercano y fifo al que pidio antes.")


def main():
    print(f"Probando binario: {BIN}\n")
    test_invalid_args()

    res = test_basic_run()
    events, used_args = (None, None)
    if res:
        events, used_args = res

    test_single_coder()

    if used_args:
        n, cooldown = used_args[0], used_args[6]
        test_adjacency(events, n)
        test_cooldown(events, n, cooldown)

    test_burnout()
    test_scheduler_diff()

    total, passed = len(results), sum(results)
    print(f"\n===== RESUMEN: {passed}/{total} checks superados =====")
    if passed < total:
        sys.exit(1)


if __name__ == "__main__":
    main()
