/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 14:52:59 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/20 17:07:10 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo_bonus.h"

static void    *check_death(t_data *data)
{
    //NEED A SEMAPHORE TO READ last_meal
    long int        ms_since_last_meal;
    struct timeval  now;

    while (1)
    {
        if (gettimeofday(&now, NULL) != 0)
        {
            sem_post(data->philo_alive);
            print_error("check_death", "gettimeofday error");
            kill(data->philo_pid[data->philo_id], SIGKILL);
        }
        // >>> Critical code section
        ms_since_last_meal = 1000 * (data->last_meal.tv_sec - now.tv_sec)
            + (data->last_meal.tv_usec - now.tv_usec);
        // <<< Critical code section
        if (ms_since_last_meal > data->time_to_die)
        {
            sem_wait(data->stdout_sem);
            printf("%d %d %s\n", ms_since_last_meal, data->philo_id + 1, DIED);
            sem_post(data->philo_alive);
            kill(data->philo_pid[data->philo_id], SIGKILL);
        }
        usleep(500);
    }
    
}

static void    log_philo(t_data *data, char *msg)
{
    struct timeval	now;
	int				timestamp;

	// if (simulation_stopped(philo->data))
	// 	return ;
    sem_wait(data->stdout_sem);
	// if (simulation_stopped(philo->data))
	// 	return ((void) sem_post(data->stdout_sem));
	if (gettimeofday(&now, NULL) != 0)
    {
        sem_post(data->philo_alive);
        print_error("check_death", "gettimeofday error");
        kill(data->philo_pid[data->philo_id], SIGKILL);
    }
	timestamp = (now.tv_sec - data->simulation_start_time.tv_sec) * 1000
        + (now.tv_usec - data->simulation_start_time.tv_usec) / 1000;
	printf("%d %d %s\n", timestamp, data->philo_id + 1, msg);
    sem_post(data->stdout_sem);
}

static void    philo_live(t_data *data)
{
    while (1)
    {
        sem_wait(data->fork);
        log_philo(data, TAKE_FORK);
        sem_wait(data->fork);
        log_philo(data, TAKE_FORK);
        log_philo(data, EAT);
        if (gettimeofday(&data->last_meal, NULL) != 0)
        {
            sem_post(data->philo_alive);
            print_error("check_death", "gettimeofday error");
            kill(data->philo_pid[data->philo_id], SIGKILL);
        }
        ft_msleep(data->time_to_eat);
        sem_post(data->fork);
        sem_post(data->fork);
        log_philo(data, SLEEP);
        ft_msleep(data->time_to_sleep);
        log_philo(data, THINK);
        usleep(10);
    }
}

void    philo_routine(t_data *data)
{
    pthread_t   *death_checker;

    sem_wait(data->philo_alive);
    if (gettimeofday(&data->simulation_start_time, NULL) != 0)
    {
        sem_post(data->philo_alive);
        exit_error("philo_routine", "gettimeofday error");
    }
    data->last_meal = data->simulation_start_time;
    if (pthread_create(death_checker, NULL, check_death, (void *)data) != 0)
    {
        sem_post(data->philo_alive);
        exit_error("philo_routine", "pthread_create error");
    }
    philo_live(data);
    sem_post(data->philo_alive);
    sem_close(data->philo_alive);
    exit(EXIT_SUCCESS);
}