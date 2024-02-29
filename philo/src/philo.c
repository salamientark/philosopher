/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 15:50:42 by dbaladro          #+#    #+#             */
/*   Updated: 2024/02/29 09:55:36 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void    philo_sleep(t_philo *philo)
{
    unsigned int    ms_since_last_meal;
    struct timeval  now;

    if (!philo->alive)
        return ;
    gettimeofday(&now, NULL);
    ms_since_last_meal = 1000 * (now.tv_sec - philo->last_meal.tv_sec)
        + (now.tv_usec - philo->last_meal.tv_usec) / 1000;
    log_philo(philo, "is sleeping");
    if (ms_since_last_meal + philo->data->time_to_sleep > philo->data->time_to_die)
    {
        if (philo->data->time_to_die > ms_since_last_meal)
            usleep((philo->data->time_to_die - ms_since_last_meal) * 1000);
        log_philo(philo, "died");
        philo->alive = 0;
        pthread_mutex_lock(&philo->data->dead_lock);
        philo->data->simulation_end = 1;
        pthread_mutex_unlock(&philo->data->dead_lock);
        return ;
    }
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
    pthread_mutex_lock(&philosopher->data->dead_lock);
    philosopher->data->simulation_end = 1;
    pthread_mutex_unlock(&philosopher->data->dead_lock);
    return (1);
}

int simulation_stopped(t_philo *philo)
{
    int simulation_stoped;

    simulation_stoped = 0;
    pthread_mutex_lock(&(philo->data->dead_lock));
    simulation_stoped = philo->data->simulation_end;
    pthread_mutex_unlock(&(philo->data->dead_lock));
    return (simulation_stoped);
}

/*
    Fix bug si philo va mourir pendant le repas ou le sleep
*/
void    philo_eat(t_philo *philo)
{
    int first_fork;
    int second_fork;

    first_fork = (philo->id + (philo->id % 2 == 1)) % philo->data->philo_nbr;
    second_fork = (philo->id + (philo->id % 2 == 0)) % philo->data->philo_nbr;
    pthread_mutex_lock(&(philo->data->fork[first_fork]));
    if (philo_died(philo) == 1)
        return ((void) pthread_mutex_unlock(&philo->data->fork[first_fork]));
    log_philo(philo, "has taken a fork");
    pthread_mutex_lock(&(philo->data->fork[second_fork]));
    if (philo_died(philo))
    {
        pthread_mutex_unlock(&(philo->data->fork[first_fork]));
        pthread_mutex_unlock(&(philo->data->fork[second_fork]));
        return ;
    }
    log_philo(philo, "has taken a fork");
    log_philo(philo, "is eating");
    gettimeofday(&philo->last_meal, NULL);
    usleep(1000 * philo->data->time_to_eat);
    pthread_mutex_unlock(&(philo->data->fork[first_fork]));
    pthread_mutex_unlock(&(philo->data->fork[second_fork]));
}

void    *better_philo(void *param)
{
    t_philo         *philosopher;

    philosopher = (t_philo *)param;
    // t_data *data = (t_data *)philosopher->data;
    // (void) data;
    if (philosopher->id % 2)
        usleep(10000);
    // while(philosopher->alive)
    // philosopher->data->simulation_end = 1;
    while(philosopher->alive)
    {
        philo_eat(philosopher);
        philo_sleep(philosopher);
        if (philosopher->alive)
            log_philo(philosopher, "is thinking");
        pthread_mutex_lock(&philosopher->data->dead_lock);
        if (philosopher->data->simulation_end == 1 || !philosopher->alive)
            return ((void) pthread_mutex_unlock(&philosopher->data->dead_lock),
                NULL);
        pthread_mutex_unlock(&philosopher->data->dead_lock);
        usleep(100);
    }
    // *philosopher->data->simulation_end = 1;
    return (NULL);
}

/*
    Return:
    1 : Everybody survived
    0 : Someone died
*/
void    monitor(t_data *data_p)
{
    int index;
    int finished_eating;

    index = 0;
    finished_eating = 0;
    while (!finished_eating && index < data_p->philo_nbr)
    {
        if (index == data_p->philo_nbr)
        {
            index = 0;
            finished_eating = 1;
        }
        pthread_mutex_lock(&data_p->dead_lock);
        if (data_p->simulation_end == 1)
        {
            pthread_mutex_unlock(&data_p->dead_lock);
            return ;
        }
        pthread_mutex_unlock(&data_p->dead_lock);
        finished_eating *= (data_p->philosopher[index].meal_left == 0);
        index++;
        usleep(100);
    }
}

/*
    Init data
    Start threads.
    Main thread monitor
    other thread philo
*/
int main(int ac, char **av)
{
    t_data  *data;
    // int     index;

    data = init_simulation(ac, av);
    // index = 0;
    // monitor(data);
    // while (index < data->philo_nbr)
    // {
    //     pthread_join(data->philosopher[index].tid, NULL);
    //     index++;
    // }
    monitor(data);
    int index = 0;
    while (index < data->philo_nbr)
    {
        printf("waiting philo %d\n", index);
        pthread_join(data->philosopher[index].tid, NULL);
        printf("philo %d : Finish waiting", index);
        index++;
    }
    // usleep(3000000);
    // printf("en simulation\n go exit_simulation\n");
    exit_simulation(data);
    return (0);
}