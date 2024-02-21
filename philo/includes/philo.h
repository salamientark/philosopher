/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 15:50:55 by dbaladro          #+#    #+#             */
/*   Updated: 2024/02/21 11:59:07 by dbaladro         ###   ########.fr       */
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
# define BAD_ARG_NBR "zbeub"

typedef struct s_philo
{
    pthread_t       tid;
    int             alive;
    int             pos;
    int             *time_to_die;
    int             *time_to_eat;
    int             *time_to_sleep;
    int             max_nbr_of_eat;
    int             *simulation_end;
    pthread_mutex_t *std_lock;
    pthread_mutex_t *fork[2];
    struct timeval  last_meal;
}               t_philo;

/*
    DATA STRUCTURE
*/
typedef struct s_data
{
    int             philo_nbr;
    int             time_to_die;
    int             time_to_eat;
    int             time_to_sleep;
    int             max_nbr_of_eat;
    int             simulation_end;
    struct timeval  simulation_start_time;
    pthread_mutex_t std_lock;
    pthread_mutex_t *fork;
    t_philo         *philosopher;
}               t_data;

// ERROR.c
void    exit_error(char *prog, char *msg);
void    exit_simulation(t_data *data_p);

// ARG_PARSER.c
t_data  init_simulation(int ac, char **av);

// PHILO.c
void    *philo(void *param);

// PRINT.c
void    log_philo(t_philo *philo, char *msg);

#endif