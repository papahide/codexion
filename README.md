*This project has been created as part of the 42 curriculum by paapahid.*

# Codexion

## Description

Codexion is a multithreaded simulation in C of the classic dining philosophers problem, reimagined with programmers, USB dongles, and a quantum compiler. Each programmer runs as an independent POSIX thread and cycles through three states: compiling, debugging, and refactoring.

To compile, a programmer needs two USB dongles simultaneously — one in each hand. Dongles are shared resources arranged in a circle, one between each pair of programmers. After being released, each dongle enters a cooldown period during which it cannot be taken again.

The simulation supports two scheduling policies for dongle arbitration:
- **FIFO** (First In, First Out): the programmer who requested the dongle first receives it first.
- **EDF** (Earliest Deadline First): the programmer whose burnout deadline is closest receives priority.

The simulation stops when all programmers have compiled the required number of times, or when one programmer burns out.

## Instructions

### Compilation

```bash
make
```

This produces a binary named `codexion` compiled with `-Wall -Wextra -Werror -pthread`.

### Execution

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All arguments are mandatory and must be positive integers, except `scheduler` which must be exactly `fifo` or `edf`.

### Arguments

| Argument | Description |
|---|---|
| `number_of_coders` | Number of programmers (and dongles) |
| `time_to_burnout` | Time in ms before a programmer burns out without compiling |
| `time_to_compile` | Time in ms to compile (requires 2 dongles) |
| `time_to_debug` | Time in ms to debug |
| `time_to_refactor` | Time in ms to refactor |
| `number_of_compiles_required` | Compilations needed per programmer to end the simulation |
| `dongle_cooldown` | Time in ms a dongle is unavailable after being released |
| `scheduler` | Arbitration policy: `fifo` or `edf` |

### Example

```bash
./codexion 4 800 200 200 200 3 50 fifo
```

Expected output format:
```
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
201 1 is debugging
401 1 is refactoring
...
```

### Makefile rules

```bash
make        # Build the binary
make clean  # Remove object files
make fclean # Remove object files and binary
make re     # Full rebuild
```

## Blocking Cases Handled

### Deadlock prevention
Programmers sit in a circle and each needs two adjacent dongles. If all programmers simultaneously pick up their left dongle and wait for the right one, a circular deadlock occurs. This is prevented by alternating the pickup order based on the programmer's ID: odd-ID programmers take the left dongle first, even-ID programmers take the right dongle first. This breaks the circular wait condition.

### Starvation prevention
The scheduler guarantees fair access to dongles. With FIFO, requests are served strictly in arrival order. With EDF, the programmer closest to burning out is always served first, ensuring no programmer is indefinitely blocked while others compile repeatedly.

### Cooldown management
After a dongle is released, it enters a cooldown period (`dongle_cooldown` ms) during which it cannot be taken. Each dongle stores the timestamp at which it becomes available again (`available_at`). Programmers polling for a dongle check both the `available` flag and the current time against `available_at` before acquiring it.

### Precise burnout detection
A dedicated monitor thread runs independently and checks each programmer's deadline (`last_compile_start + time_to_burnout`) every millisecond. When a deadline is exceeded, the monitor immediately logs the burnout and sets the stop flag, guaranteeing the log appears within 10ms of the actual burnout moment.

### Log serialization
All log output is protected by a dedicated `log_mutex`. Every state change — dongle taken, compiling, debugging, refactoring, burned out — is printed inside a critical section, ensuring no two log lines are ever interleaved.

### Graceful shutdown
When the stop flag is set, all threads check it at every meaningful point: before and after taking dongles, after each `usleep`. Threads that are blocked waiting for a dongle will detect the stop flag within 500 microseconds (the polling interval) and exit cleanly.

## Thread Synchronization Mechanisms

### `pthread_mutex_t` — mutual exclusion

Three categories of mutexes are used:

**Dongle mutex** (`t_dongle.mutex`): protects the state of each individual dongle — its `available` flag, `available_at` timestamp, and the waiting queue. Any thread reading or modifying dongle state must hold this mutex.

**Log mutex** (`t_simulation.log_mutex`): serializes all output to stdout. Every `printf` in the program is wrapped between a lock and unlock of this mutex.

**Stop mutex** (`t_simulation.stop_mutex`): protects the global `stop` flag shared between the monitor thread and all coder threads. Both reads and writes go through this mutex to prevent data races.

### Priority queue (min-heap) for scheduling

Each dongle maintains its own waiting queue implemented as a min-heap of `t_request` entries. Each request stores the coder's ID and a priority value:
- FIFO: `priority = timestamp of request`
- EDF: `priority = last_compile_start + time_to_burnout`

`heap_push` inserts in O(1) (unordered array); `heap_pop` finds the minimum in O(n) via linear scan, which is efficient enough given the small number of coders per dongle. On equal priorities (EDF tie), the higher `coder_id` wins.

### Polling loop instead of `pthread_cond_wait`

Rather than sleeping on a condition variable (which would require careful signaling after cooldown expiry), coder threads use a polling loop with `usleep(500)`:

```c
while (!sim->stop && (queue[0].coder_id != my_id
        || !dongle->available
        || get_time_ms() < dongle->available_at))
{
    pthread_mutex_unlock(&dongle->mutex);
    usleep(500);
    pthread_mutex_lock(&dongle->mutex);
}
```

The mutex is released during the sleep to allow other threads to modify the dongle state. This naturally handles cooldown expiry without needing a timed condition wait.

### Race-condition-free state transitions

`compiles_done` is only written by the coder thread that owns it and only read by the monitor thread — no mutex is needed because the monitor only uses it to check termination, and a stale read of a single integer is safe on all target architectures. All other shared state (stop flag, dongle fields, log output) is always accessed under the appropriate mutex.

## Resources

### References
- [Unix Threads in C — CodeVault](https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2)
- [POSIX Threads Programming — Lawrence Livermore National Laboratory](https://hpc-tutorials.llnl.gov/posix/)
- [The Little Book of Semaphores — Allen B. Downey](https://greenteapress.com/wp/semaphores/)
- [Dining Philosophers Problem — Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Earliest Deadline First Scheduling — Wikipedia](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
- [pthread_mutex_t — Linux man pages](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- [gettimeofday — Linux man pages](https://man7.org/linux/man-pages/man2/gettimeofday.2.html)

### AI usage

Claude (Anthropic) was used as a support tool throughout the project for the following tasks:

- **Architecture guidance**: explaining threading concepts (mutexes, condition variables, deadlock prevention strategies) before writing any code.

All code was written, reviewed, and fully understood by the team members.