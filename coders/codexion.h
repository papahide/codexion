/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paapahid <paapahid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 22:43:51 by paapahid          #+#    #+#             */
/*   Updated: 2026/08/26 20:54:02 by paapahid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>

typedef struct s_parameters
{
	int		number_of_coders;
	long	time_to_burnout;
	long	time_to_compile;
	long	time_to_debug;
	long	time_to_refactor;
	int		comp_required;
	long	dongle_cooldown;
	int		scheduler;
}	t_parameters;

typedef struct s_simulation	t_simulation;
typedef struct s_coder		t_coder;

typedef struct s_request
{
	int				coder_id;
	long			priority;
}	t_request;

typedef struct s_heap
{
	t_request	*requests;
	int			size;
}	t_heap;

typedef struct s_dongle
{
	int				available;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	long			available_at;
	t_heap			queue;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	int				compiles_done;
	long			last_compile_start;
	t_simulation	*simulation;
}	t_coder;

typedef struct s_simulation
{
	t_parameters	parameters;
	t_coder			*coders;
	t_dongle		*dongles;
	int				stop;
	pthread_mutex_t	log_mutex;
	long			start_time;
	pthread_t		monitor;
	pthread_mutex_t	stop_mutex;
}	t_simulation;

// Parsing
bool			ft_strcmp(const char *str, const char *cmp);
bool			validator(char **str);
void			parse(t_parameters *valid, char **args);

// Initialization
void			init_coders(int coders_n, t_simulation *simulation);
void			init_dongles(int dongle_n, t_simulation *simulation);

// Simulation
t_simulation	*init_simulation(t_parameters *params);
void			start_simulation(t_simulation *sim);
void			stop_simulation(t_simulation *sim, int coder_id, int reason);

// Routines
void			*coder_routine(void *arg);
void			*monitor_routine(void *arg);

// Coder routine utils
int				coder_cycle(t_coder *coder, int coders,
					int coder_id, t_parameters param);

// Routine utils
void			take_dongle(t_coder *coder, t_dongle *dongle);
void			take_both_dongles(t_coder *coder,
					t_dongle *left, t_dongle *right);
void			leave_dongle(t_coder *coder, t_dongle *dongle);
int				check_compiles_done(t_simulation *sim);
int				set_sim_stop(t_simulation *sim);
long			get_time_ms(void);

// Logs
void			print_log(t_simulation *simulation, int coder_id, char *action);

// Heap
t_heap			*init_heap(t_heap *heap, int max_size);
void			heap_push(t_heap *heap, t_request request);
t_request		heap_pop(t_heap *heap);

// Requests
t_request		create_request(int coder_id, long priority);

#endif
