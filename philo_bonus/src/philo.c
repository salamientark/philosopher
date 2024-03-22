/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 14:52:59 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/22 07:56:23 by dbaladro         ###   ########.fr       */
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
        // kill(0, SIGKILL);
        // kill(data->philo_pid[data->philo_id], SIGKILL);
    }
    sem_post(data->stdout_sem);
}

static void    *check_death(void *param)
{
    long int        ms_since_last_meal;
    struct timeval  now;
    struct timeval  last_meal_cp;
    t_data          *data;

    data = (t_data *)param;
    sem_wait(data->eat_sem);
    last_meal_cp = data->last_meal;
    sem_post(data->eat_sem);
    while (1)
    {
        if (gettimeofday(&now, NULL) != 0)
        {
            sem_post(data->simulation_stop);
            print_error("check_death", "gettimeofday error");
            exit(EXIT_FAILURE);
        }
        ms_since_last_meal = 1000 * (now.tv_sec - last_meal_cp.tv_sec)
            + (now.tv_usec - last_meal_cp.tv_usec) / 1000;
        if (ms_since_last_meal > data->time_to_die)
        {
            log_philo(data, DIED);
            exit(EXIT_SUCCESS);
        }
        ft_msleep(data->time_to_die - ms_since_last_meal - 2);
        while (ms_since_last_meal <= data->time_to_die)
        {
            if (gettimeofday(&now, NULL) != 0)
            {
                sem_post(data->simulation_stop);
                print_error("check_death", "gettimeofday error");
                exit(EXIT_FAILURE);
            }
            ms_since_last_meal = 1000 * (now.tv_sec - last_meal_cp.tv_sec)
                + (now.tv_usec - last_meal_cp.tv_usec) / 1000;
            usleep(400);
        }
        sem_wait(data->eat_sem);
        if (last_meal_cp.tv_usec == data->last_meal.tv_usec && last_meal_cp.tv_sec == data->last_meal.tv_sec)
        {
            log_philo(data, DIED);
            exit(EXIT_SUCCESS);
        }
        last_meal_cp = data->last_meal;
        sem_post(data->eat_sem);
        // sem_wait(data->eat_sem);
        // if (data->meal_to_take == 0)
        // {
        //     sem_post(data->eat_sem);
        //     return ((void *)NULL);
        // }
        // sem_post(data->eat_sem);
        // usleep((data->time_to_die - ms_since_last_meal) * 1000 - 100);
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
        sem_wait(data->eat_sem);
        if (gettimeofday(&data->last_meal, NULL) != 0)
        {
            sem_post(data->simulation_stop);
            print_error("check_death", "gettimeofday error");
            exit(EXIT_FAILURE);
        }
        if (data->meal_to_take > 0)
            data->meal_to_take--;
        if (data->meal_to_take == 0)
            break ;
        sem_post(data->eat_sem);
        ft_msleep(data->time_to_eat);
        sem_post(data->fork);
        sem_post(data->fork);
        log_philo(data, SLEEP);
        ft_msleep(data->time_to_sleep);
        log_philo(data, THINK);
        usleep(100);
    }
    sem_post(data->eat_sem);
    sem_post(data->fork);
    sem_post(data->fork);
}

static void    wait_simulation_start(t_data *data)
{
    struct timeval  now;
    long int        simu_start_ms;
    long int        now_ms;

    if (gettimeofday(&now, NULL) != 0)
    {
        sem_post(data->simulation_stop);
        print_error("philo_routine", "gettimeofday error");
        exit(EXIT_FAILURE);
    }
    now_ms = 1000 * now.tv_sec + now.tv_usec / 1000;
    simu_start_ms = 1000 * data->simulation_start_time.tv_sec
        + data->simulation_start_time.tv_usec / 1000;
    while (now_ms < simu_start_ms)
    {
        if (gettimeofday(&now, NULL) != 0)
        {
            sem_post(data->simulation_stop);
            print_error("philo_routine", "gettimeofday error");
            exit(EXIT_FAILURE);
        }
        now_ms = 1000 * now.tv_sec + now.tv_usec / 1000;
        usleep(200);
    }
}

void    philo_routine(t_data *data)
{
    pthread_t   death_checker;

    // usleep(10000);
    sem_wait(data->simulation_stop);
    sem_wait(data->meal_sem);
    data->last_meal = data->simulation_start_time;
    if (pthread_create(&death_checker, NULL, check_death, (void *)data) != 0)
    {
        sem_post(data->simulation_stop);
        print_error("philo_routine", "pthread_create error");
        exit(EXIT_FAILURE);
        // exit_error("philo_routine", "pthread_create error");
    }
    pthread_detach(death_checker);
    wait_simulation_start(data);
    // if (data->philo_id % 2)
    //     ft_msleep(data->time_to_eat / 2);
    usleep(200 * (data->philo_id + 1));
    philo_live(data);
    sem_post(data->meal_sem);
    // ft_msleep(data->time_to_eat * 2);
    exit(EXIT_SUCCESS);
}