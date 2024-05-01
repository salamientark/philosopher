/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 08:40:30 by dbaladro          #+#    #+#             */
/*   Updated: 2024/05/01 10:56:28 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <fcntl.h>
# include <sys/time.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <pthread.h>
# include <semaphore.h>

# define INT_MAX 2147483647
# define INT_MIN -2147483648
# define BAD_ARG_NBR "./philo_bonus [ philo_nbr ] [ time_to_die ] \
[ time_to_eat ] [ time_to_sleep ] \
*[ number_of_meal_each_philo_must_have ]*\n\
number_of_meal_each_philo_must_have : Optionnal argument"
# define INVALID_ARG "Argument need to be strictly positive integer\n\
Except [ philo_nbr ], which need to be superior to 1"
# define TAKE_FORK "has taken a fork"
# define DIED "died"
# define EAT "is eating"
# define SLEEP "is sleeping"
# define THINK "is thinking"
# define SEM_STDOUT "/philo_stdout\0"
# define SEM_FORK "/philo_fork\0"
# define SEM_MEAL "/philo_meal\0"
# define SEM_SIMULAION_STOP "/philo_simulation_stop\0"
# define SEM_END_SIMU "/philo_end_simu\0"

// Shared data structure
typedef struct s_data
{
	int				philo_nbr;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				meal_to_take;
	struct timeval	last_meal;
	struct timeval	simulation_start_time;
	int				philo_id;
	int				philo_live;
	sem_t			*stdout_sem;
	sem_t			*fork;
	sem_t			*dead_sem;
	sem_t			*meal_sem;
	sem_t			*eat_sem;
	sem_t			*simulation_stop;
	sem_t			*end_simu;
	pid_t			*philo_pid;
}				t_data;

// utils.c
int			ft_atoi(char *s);
void		print_error(char *func, char *msg);
void		ft_msleep(long int wait_time);
void		exit_child(t_data *data, char *err_func, char *err_msg);
const char	*sem_name(int id, char type, char name[]);

// init_simulation.c;
t_data		*init_simulation(int ac, char **av);

// philo_thread.c
void		*check_simu_end(void *param);
void		*death_checker(void *param);

// philo.c
int			log_philo(t_data *data, char *msg);
void		philo_routine(t_data *data);

void		exit_simulation(t_data *data, char *func, char *err_msg);

#endif
