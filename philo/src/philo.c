/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 15:50:42 by dbaladro          #+#    #+#             */
/*   Updated: 2024/02/28 11:06:01 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void    philo_sleep(t_philo *philo)
{
    unsigned int    ms_since_last_meal;
    struct timeval  now;

    gettimeofday(&now, NULL);
    if (!philo->alive)
        return ;
    ms_since_last_meal = 1000 * (now.tv_sec - philo->last_meal.tv_sec)
        + (now.tv_usec - philo->last_meal.tv_usec) / 1000;
    if (ms_since_last_meal + philo->data->time_to_eat > philo->data->time_to_die)
    {
        usleep(philo->data->time_to_die - ms_since_last_meal);
        log_philo(philo, "died");
        philo->alive = 0;
        return ;
    }
    log_philo(philo, "is sleeping");
    usleep(1000 * (philo->data->time_to_sleep));
}

int     philo_died(t_philo *philosopher)
{
    struct timeval  right_now;

    if (gettimeofday(&right_now, NULL) != 0)
    {
        write(1, "philo_died: gettimeofday error\n", 31);
        return 1;
    }
    if ((1000 * (right_now.tv_sec - philosopher->last_meal.tv_sec)
            + (right_now.tv_usec - philosopher->last_meal.tv_usec) / 1000)
                < philosopher->data->time_to_die)
        return (0);
    philosopher->alive = 0;
    log_philo(philosopher, "died");
    // *philosopher->data->simulation_end = 1;
    return (1);
}

/*
    Fix bug si philo va mourir pendant le repas ou le sleep
*/
void    philo_eat(t_philo *philo)
{
    // BEST to check if philo is going to die before eating
    struct timeval  now;
    int             ms_since_last_meal;

    gettimeofday(&now, NULL);
    ms_since_last_meal = 1000 * (now.tv_sec - philo->last_meal.tv_sec)
        + (now.tv_usec - philo->last_meal.tv_usec) / 1000;
    if (ms_since_last_meal + philo->data->time_to_eat > philo->data->time_to_die)
    {
        usleep(philo->data->time_to_die - ms_since_last_meal);
        log_philo(philo, "died");
        philo->alive = 0;
        return ;
    }
    pthread_mutex_lock(&philo->data->fork[philo->id]);
    log_philo(philo, "has taken a fork");
    pthread_mutex_lock(&philo->data->fork[(philo->id + 1)
        % philo->data->philo_nbr]);
    log_philo(philo, "has taken a fork");
    log_philo(philo, "is eating");
    gettimeofday(&philo->last_meal, NULL);
    usleep(philo->data->time_to_eat * 1000);
    usleep(1000 * philo->data->time_to_eat);
    pthread_mutex_unlock(&philo->data->fork[(philo->id + 1)
        % philo->data->philo_nbr]);
    pthread_mutex_unlock(&philo->data->fork[philo->id]);
}

/*
    FIX:
        - Add max_eat_nbr in the while condition
        - Wait for every thread to create before first eating
        - Add algorithm (first % 2 philo eat then  + 1 % 2)
        - Improve execution speed
*/
// void    *philosoph(void *param)
// {
//     t_philo         *philo;
//     int             first_fork;
//     int             second_fork;
//     unsigned int    ms_since_last_meal;

//     philo = (t_philo *)param;
//     struct timeval  right_now;
//     first_fork = philo->pos % 2;
//     second_fork = (philo->pos + 1) % 2;
//     if (philo->pos % 2)
//         usleep(10000);
//     while(*(philo->simulation_end) == 0)
//     {
//         // PHILO EAT
//         // philo_eat
//         pthread_mutex_lock(philo->fork[first_fork]);
//         if (philo_died(philo))
//         {
//             // printf("philo %d died after first_lock\n", philo->pos + 1);
//             pthread_mutex_unlock(philo->fork[first_fork]);
//             return ((void *) NULL);
//         }
//         pthread_mutex_lock(philo->fork[second_fork]);
//         if (philo_died(philo))
//         {
//             // printf("philo %d died after second lock\n", philo->pos + 1);
//             pthread_mutex_unlock(philo->fork[second_fork]);
//             pthread_mutex_unlock(philo->fork[first_fork]);
//             return ((void *) NULL);
//         }
//         log_philo(philo, "is eating");
//         if (gettimeofday(&(philo->last_meal), NULL) != 0)
//         {
//             *philo->simulation_end = 1;
//             philo->alive = 0;
//             write(2, "philo: gettimeofday error\n", 26);
//             return ((void *) NULL);
//         }
//         usleep(1000 * *philo->time_to_eat);
//         pthread_mutex_unlock(philo->fork[second_fork]);
//         pthread_mutex_unlock(philo->fork[first_fork]);
//         // END philo eat

//         // PHILO SLEEP
//         // Check if philo will die
//         if (gettimeofday(&right_now, NULL) != 0)
//         {
//             *philo->simulation_end = 1;
//             philo->alive = 0;
//             write(2, "philo: gettimeofday error\n", 26);
//             return ((void *) NULL);
//         }
//         ms_since_last_meal = (right_now.tv_sec - philo->last_meal.tv_sec)
//             * 1000 + (right_now.tv_usec - philo->last_meal.tv_usec) / 1000;
//         if (ms_since_last_meal + *philo->time_to_sleep >= *philo->time_to_die)
//         {
//             if (ms_since_last_meal <= *philo->time_to_die)
//                 usleep(1000 * (*philo->time_to_die - ms_since_last_meal));
//             log_philo(philo, "died");
//             *philo->simulation_end = 1;
//             return ((void *) NULL);
//         }
//         log_philo(philo, "is sleeping");
//         usleep(1000 * *philo->time_to_sleep);
//         // END Philo sleep

//         // Philo think, waiting for a fork
//         if (philo->alive)
//             log_philo(philo, "is thinking");
//         // usleep(10);
//     }
//     printf("philo %d end\n", philo->pos);
//     return ((void *) NULL);
// }

void    *better_philo(void *param)
{
    t_philo         *philosopher;

    philosopher = (t_philo *)param;
    t_data *data = (t_data *)philosopher->data;
    (void) data;
    if (philosopher->id % 2)
        usleep(10000);
    while(philosopher->alive)
    {
        philo_eat(philosopher);
        philo_sleep(philosopher);
        if (philosopher->alive)
            log_philo(philosopher, "is thinking");
        // usleep(10);
    }
    *philosopher->data->simulation_end = 1;
    return (NULL);
}

// void    start_simulation(t_data *data)
// {
//     int index;

//     if (gettimeofday(&(data->simulation_start_time), NULL) != 0)
//     {
//         printf("Error gettimeofday\n");
//         exit_simulation(data);
//     }
//     while (index < data->philo_nbr)
//     {
//         data->philosopher[index].last_meal = data->simulation_start_time;
//         index++;
//     }
// }

/*
    Return:
    1 : Everybody survived
    0 : Someone died
*/
int    monitor(t_data *data_p)
{
    int index;
    int simulation_finished;

    index = 0;
    simulation_finished = 0;
    while (!simulation_finished && index < data_p->philo_nbr)
    {
        if (index == data_p->philo_nbr)
        {
            index = 0;
            simulation_finished = 1;
        }
        if (philo_died(&(data_p->philosopher[index]))
            && data_p->philosopher[index].alive)
        {
            data_p->philosopher[index].alive = 0;
            *data_p->simulation_end = 1;
        }
        if (!data_p->philosopher[index].alive)
            break ;
        simulation_finished *= data_p->philosopher[index].meal_left == 0;
        index++;
    }
    return (index == data_p->philo_nbr);
}

/*
    Init data
    Start threads.
    Main thread monitor
    other thread philo
*/
int main(int ac, char **av)
{
    t_data  data;
    int     index;

    if (!(ac == 5 || ac == 6))
        exit_error("philo", BAD_ARG_NBR);
    data = init_simulation(ac, av);
    index = 0;
    while (index < data.philo_nbr)
    {
        if (pthread_create(&data.philosopher[index].tid, NULL,
				better_philo, &(data.philosopher[index])) != 0)
		{
			write(2, "main: pthread_create error\n", 27);
			exit_simulation(&data);
		}
		if (pthread_detach(data.philosopher[index].tid) != 0)
		{
			write(2, "main: pthread_detach error\n", 27);
			exit_simulation(&data);
		}
        index++;
    }
    monitor(&data);
    // return (0);
    exit_simulation(&data);
}