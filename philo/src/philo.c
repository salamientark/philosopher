/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 15:50:42 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/03 22:40:54 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int simulation_stopped(t_data *data)
{
    int simulation_stoped;

    simulation_stoped = 0;
    pthread_mutex_lock(&data->dead_lock);
    simulation_stoped = data->simulation_end;
    pthread_mutex_unlock(&data->dead_lock);
    return (simulation_stoped);
}

void    philo_sleep(t_philo *philo)
{
    unsigned int    ms_since_last_meal;
    struct timeval  now;

    if (simulation_stopped(philo->data))
        return ;
    gettimeofday(&now, NULL);
    // pthread_mutex_lock(&philo->data->meal_lock);
    ms_since_last_meal = 1000 * (now.tv_sec - philo->last_meal.tv_sec)
        + (now.tv_usec - philo->last_meal.tv_usec) / 1000;
    // pthread_mutex_unlock(&philo->data->meal_lock);
    if (simulation_stopped(philo->data))
        return ;
    log_philo(philo, "is sleeping");
    if (ms_since_last_meal + philo->data->time_to_sleep > philo->data->time_to_die)
    {
        if (philo->data->time_to_die > ms_since_last_meal)
            usleep((philo->data->time_to_die - ms_since_last_meal) * 1000);
        log_philo(philo, "died");
        // philo->alive = 0;
        // pthread_mutex_lock(&philo->data->dead_lock);
        // philo->data->simulation_end = 1;
        // pthread_mutex_unlock(&philo->data->dead_lock);
        return ;
    }
    usleep(1000 * (philo->data->time_to_sleep));
}

// OK
int     philo_died(t_philo *philosopher)
{
    struct timeval  now;
    unsigned int    ms_since_last_meal;

    if (gettimeofday(&now, NULL) != 0)
    {
        write(1, "philo_died: gettimeofday error\n", 31);
        return (1);
    }
    pthread_mutex_lock(&philosopher->data->meal_lock);
    ms_since_last_meal = 1000 * (now.tv_sec - philosopher->last_meal.tv_sec)
        + (now.tv_usec - philosopher->last_meal.tv_usec) / 1000;
    pthread_mutex_unlock(&philosopher->data->meal_lock);
    if (ms_since_last_meal <= philosopher->data->time_to_die)
        return (0);
    pthread_mutex_lock(&philosopher->data->dead_lock);
    log_philo(philosopher, "died");
    philosopher->alive = 0;
    philosopher->data->simulation_end = 1;
    pthread_mutex_unlock(&philosopher->data->dead_lock);
    return (1);
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
    // CHECK if alive
    if (simulation_stopped(philo->data))
        return ((void) pthread_mutex_unlock(&philo->data->fork[first_fork]));
    log_philo(philo, "has taken a fork");
    pthread_mutex_lock(&(philo->data->fork[second_fork]));
    // Check if alive
    if (simulation_stopped(philo->data))
    {
        pthread_mutex_unlock(&(philo->data->fork[first_fork]));
        pthread_mutex_unlock(&(philo->data->fork[second_fork]));
        return ;
    }
    log_philo(philo, "has taken a fork");
    log_philo(philo, "is eating");
    pthread_mutex_lock(&philo->data->meal_lock);
    gettimeofday(&philo->last_meal, NULL);
    if (philo->meal_left > 0)
        philo->data->meal_to_take--;
    pthread_mutex_unlock(&philo->data->meal_lock);
    if (philo->meal_left > 0)
        philo->meal_left--;
    /*
    if(philo->data->time_to_eat > philo->data->time_to_die)
        wait(time_to_die - now)
        die
    eat
    */
    if (philo->data->time_to_eat > philo->data->time_to_die)
    {
        usleep(1000 * philo->data->time_to_die);
        log_philo(philo, "died");
    }
    else
        usleep(1000 * philo->data->time_to_eat);
    pthread_mutex_unlock(&(philo->data->fork[first_fork]));
    pthread_mutex_unlock(&(philo->data->fork[second_fork]));
}

void    *better_philo(void *param)
{
    t_philo         *philosopher;

    philosopher = (t_philo *)param;
    if (philosopher->id % 2)
        usleep(5000);
    while(1)
    {
        philo_eat(philosopher);
        if (simulation_stopped(philosopher->data))
            break ;
        philo_sleep(philosopher);
        if (simulation_stopped(philosopher->data))
            break;
        log_philo(philosopher, "is thinking");
        usleep(10);
    }
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
    struct timeval  now;
    struct timeval  last_meal_cp;
    

    index = 0;
    while (1)
    {
        if (index == data_p->philo_nbr)
            index = 0;
        // Meal check
        pthread_mutex_lock(&data_p->meal_lock);
        // Get philo last_meal -> So meal mutex isn't used more than once
        last_meal_cp = data_p->philosopher[index].last_meal;
        if (data_p->meal_to_take == 0)
        {
            pthread_mutex_lock(&data_p->dead_lock);
            data_p->simulation_end = 1;
            pthread_mutex_unlock(&data_p->dead_lock);
            pthread_mutex_unlock(&data_p->meal_lock);
            return ;
        }
        pthread_mutex_unlock(&data_p->meal_lock);
        gettimeofday(&now, NULL);
        if (1000 * (now.tv_sec - last_meal_cp.tv_sec) + (now.tv_usec
            - last_meal_cp.tv_usec) / 1000 > data_p->time_to_die)
            log_philo(&data_p->philosopher[index], "died");
        if (simulation_stopped(data_p))
            return ;
        index++;
        usleep(10);
    }
}

/*
    Init data
    Start threads.
    other thread philo
*/
int main(int ac, char **av)
{
    t_data  *data;

    if (ac != 5 && ac != 6)
        exit_error("philo", BAD_ARG_NBR);
    data = init_simulation(ac, av);
    monitor(data);
    int index = 0;
    while (index < data->philo_nbr)
    {
        pthread_join(data->philosopher[index].tid, NULL);
        index++;
    }
    exit_simulation(data);
    return (0);
}