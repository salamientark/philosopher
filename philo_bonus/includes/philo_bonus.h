/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 08:40:30 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/20 17:09:29 by dbaladro         ###   ########.fr       */
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
# define SEM_PHILO_ALIVE "/philot_alive\0"
# define SEM_FORK "/philo_fork\0"

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
	sem_t           *stdout_sem;
    sem_t           *fork;
	sem_t			*philo_alive;
	pid_t			*philo_pid;
}				t_data;

// init_simulation.c;
void	exit_simulation(t_data *data, char *func, char *err_msg);
t_data	*init_simulation(int ac, char **av);

// ft_msleep.c
void	ft_msleep(long int wait_time);

// philo.c
void    philo_routine(t_data *data);

#endif