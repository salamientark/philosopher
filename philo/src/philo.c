/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 15:50:42 by dbaladro          #+#    #+#             */
/*   Updated: 2024/02/21 12:25:54 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"



void    philo_think(int time)
{
    usleep(time * 1000);
}

void    philo_sleep(t_philo *philosopher)
{
    log_philo(philosopher, "is sleeping");
    usleep(*philosopher->time_to_sleep * 1000);
}

int     philo_died(t_philo *philosopher)
{
    struct timeval  right_now;

    if (gettimeofday(&right_now, NULL) != 0)
    {
        write(1, "philosopher_died: gettimeofday error\n", 31);
        return 1;
    }
    if ((1000 * (right_now.tv_sec - philosopher->last_meal.tv_sec)
            + (right_now.tv_usec - philosopher->last_meal.tv_usec) / 1000)
                < *philosopher->time_to_die)
        return (1);
    philosopher->alive = 0;
    return (0);
}

/*
    Fix bug si philo va mourir pendant le repas ou le sleep
*/
void    philo_eat(t_philo *philo)
{
    int first_fork;
    int second_fork;

    first_fork = (philo->pos + 1) % 2;
    second_fork = philo->pos % 2;
    pthread_mutex_lock(philo->fork[first_fork]);
    if (philo_died(philo))
    {
        // pthread_mutex_unlock(philo->left_fork);
        philo->alive = 0;
        return (log_philo(philo, "died"));
    }
    pthread_mutex_lock(philo->fork[second_fork]);
    if (philo_died(philo))
    {
        // pthread_mutex_unlock(philo->left_fork);
        // pthread_mutex_unlock(philo->right_fork);
        philo->alive = 0;
        return (log_philo(philo, "died"));
    }
    log_philo(philo, "is eating");
    gettimeofday(&philo->last_meal, NULL);
    pthread_mutex_unlock(philo->fork[second_fork]);
    pthread_mutex_unlock(philo->fork[first_fork]);
}

/*
    What a philosopher do
    while (philosopher is alive)
    {
        1 : He try to eat
        if he can, he locks both mutex
            he eat
            he unlocks mutex
            he sleep
            he think
            he starve and want to eat again
        else
            he wait a bit an retry
    }
*/
void    *philo(void *param)
{
    t_philo         *philosopher;

    philosopher = (t_philo *)param;
    while(philosopher->alive)
    {
        philo_eat(philosopher);
        philo_sleep(philosopher);
        log_philo(philosopher, "is thinking");
        usleep(10);
    }
    *philosopher->simulation_end = 1;
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

// void    monitor(t_data *data)
// {
    
// }

/*
    Init data
    Start threads.
    Main thread monitor
    other thread philo
*/
int main(int ac, char **av)
{
    t_data  data;

    if (!(ac == 5 || ac == 6))
        exit_error("philo", BAD_ARG_NBR);
    data = init_simulation(ac, av);
    printf("init success\n");
    while (data.simulation_end == 0)
    {
        usleep(10);
    }
    exit_simulation(&data);
}