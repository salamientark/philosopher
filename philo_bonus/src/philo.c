/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 14:52:59 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/21 08:08:02 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo_bonus.h"

// static int      philo_died(t_data *data)
// {
//     int philo_died;

//     sem_wait(data->dead_sem);
//     philo_died = 0;
//     if (data->philo_live == 0)
//         philo_died = 1;
//     sem_post(data->dead_sem);
//     return (philo_died);
// }

static void    log_philo(t_data *data, char *msg)
{
    struct timeval	now;
	int				timestamp;

    sem_wait(data->stdout_sem);
    sem_wait(data->dead_sem);
    if (data->philo_live == 0)
    {
        sem_post(data->dead_sem);
        sem_post(data->stdout_sem);
        exit(EXIT_SUCCESS);
    }
    sem_post(data->dead_sem);
	// if (simulation_stopped(philo->data))
	// 	return ((void) sem_post(data->stdout_sem));
	if (gettimeofday(&now, NULL) != 0)
    {
        sem_post(data->simulation_stop);
        print_error("check_death", "gettimeofday error");
        kill(0, SIGKILL);
        // kill(data->philo_pid[data->philo_id], SIGKILL);
    }
	timestamp = (now.tv_sec - data->simulation_start_time.tv_sec) * 1000
        + (now.tv_usec - data->simulation_start_time.tv_usec) / 1000;
	printf("%d %d %s\n", timestamp, data->philo_id + 1, msg);
    if (*msg == 'd')
    {
        sem_wait(data->dead_sem);
        data->philo_live = 0;
        sem_post(data->dead_sem);
        sem_post(data->simulation_stop);
        exit(EXIT_SUCCESS);
        // kill(0, SIGTERM);
        // kill(data->philo_pid[data->philo_id], SIGKILL);
    }
    sem_post(data->stdout_sem);
}

static void    *check_death(void *param)
{
    long int        ms_since_last_meal;
    struct timeval  now;
    t_data          *data;

    data = (t_data *)param;
    while (1)
    {
        if (gettimeofday(&now, NULL) != 0)
        {
            sem_post(data->simulation_stop);
            print_error("check_death", "gettimeofday error");
            exit(EXIT_FAILURE);
            // kill(0, SIGKILL);
            // kill(data->philo_pid[data->philo_id], SIGKILL);
        }

        // >>> Critical code section
        sem_wait(data->meal_sem);
        ms_since_last_meal = 1000 * (now.tv_sec - data->last_meal.tv_sec)
            + (now.tv_usec - data->last_meal.tv_usec) / 1000;
        sem_post(data->meal_sem);
        // <<< Critical code section
        if (ms_since_last_meal > data->time_to_die)
        {
            log_philo(data, DIED);
            exit(EXIT_SUCCESS);
        }
        ft_msleep((data->time_to_die - ms_since_last_meal) / 4);
    }
    
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
        sem_wait(data->meal_sem);
        if (gettimeofday(&data->last_meal, NULL) != 0)
        {
            sem_post(data->simulation_stop);
            print_error("check_death", "gettimeofday error");
            kill(data->philo_pid[data->philo_id], SIGKILL);
        }
        sem_post(data->meal_sem);
        ft_msleep(data->time_to_eat);
        sem_post(data->fork);
        sem_post(data->fork);
        if (data->meal_to_take > 0)
            data->meal_to_take--;
        if (data->meal_to_take == 0)
            exit(EXIT_SUCCESS);
        log_philo(data, SLEEP);
        ft_msleep(data->time_to_sleep);
        log_philo(data, THINK);
        usleep(10);
    }
}

void    philo_routine(t_data *data)
{
    pthread_t   death_checker;

    sem_wait(data->simulation_stop);
    if (gettimeofday(&data->simulation_start_time, NULL) != 0)
    {
        sem_post(data->simulation_stop);
        print_error("philo_routine", "gettimeofday error");
        exit(EXIT_FAILURE);
        // exit_error("philo_routine", "gettimeofday error");
    }
    data->last_meal = data->simulation_start_time;
    if (pthread_create(&death_checker, NULL, check_death, (void *)data) != 0)
    {
        sem_post(data->simulation_stop);
        print_error("philo_routine", "pthread_create error");
        exit(EXIT_FAILURE);
        // exit_error("philo_routine", "pthread_create error");
    }
    pthread_detach(death_checker);
    if (data->philo_id % 2)
        ft_msleep(5);
    philo_live(data);
    sem_post(data->simulation_stop);
    sem_close(data->simulation_stop);
    exit(EXIT_SUCCESS);
}