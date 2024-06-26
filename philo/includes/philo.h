/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 15:50:55 by dbaladro          #+#    #+#             */
/*   Updated: 2024/05/01 17:55:19 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <pthread.h>

# define INT_MAX 2147483647
# define INT_MIN -2147483648
# define MAX_PHILO 200
# define BAD_ARG_NBR "./philo [ philo_nbr ] [ time_to_die ] [ time_to_eat ] \
[ time_to_sleep ] *[ number_of_meal_each_philo_must_have ]*\n\
number_of_meal_each_philo_must_have : Optionnal argument"
# define INVALID_ARG "Argument need to be strictly positive integer\n\
Except [ philo_nbr ], which need to be superior to 1"
# define TAKE_FORK "has taken a fork"
# define DIED "died"
# define EAT "is eating"
# define SLEEP "is sleeping"
# define THINK "is thinking"
# define ERROR 1
# define SUCCESS 0

// Philosoph structure
typedef struct s_philo
{
	pthread_t		tid;
	int				id;
	int				alive;
	int				meal_left;
	struct timeval	last_meal;
	struct s_data	*data;
}				t_philo;

// Shared data structure
typedef struct s_data
{
	int				philo_nbr;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				meal_per_philo;
	long			meal_to_take;
	int				simulation_end;
	struct timeval	simulation_start_time;
	pthread_mutex_t	stdout_lock;
	pthread_mutex_t	dead_lock;
	pthread_mutex_t	meal_lock;
	pthread_mutex_t	*fork;
	t_philo			*philosopher;
}				t_data;

// UTILS.C
int		simulation_stopped(t_data *data);
int		ft_atoi(char *s);
void	ft_msleep(long int wait_time);
void	print_error(char *func_name, char *msg);
void	exit_simulation(t_data *data_p);

// INIT_SIMULATION.C
t_data	*init_simulation(int ac, char **av);

// philo.c
int		log_philo(t_philo *philo, char *msg);
void	*philo_routine(void *param);

#endif
