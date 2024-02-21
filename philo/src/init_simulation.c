/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 09:15:44 by dbaladro          #+#    #+#             */
/*   Updated: 2024/02/21 12:27:24 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

int ft_atoi(char *s)
{
    long    nbr;
    int     sign;
    int     index;

    sign = 1;
    nbr = 0;
    index = 0;
    while (s[index] && ((9 <= s[index] && s[index] <= 13) || s[index] == ' '))
        index++;
    while(s[index] && (s[index] == '-' || s[index] == '+'))
    {
        if (s[index] == '-')
            sign *= -1;
        index++;
    }
    while (s[index] && ('0' <= s[index] && s[index] <= '9'))
    {
        nbr = 10 * nbr + (s[index] - '0');
        if ((sign && nbr > INT_MAX) || -nbr < INT_MIN)
            return (0);
        index++;
    }
    if (s[index])
        return (0);
    return (sign * nbr);
}

static void    init_data(t_data *data_p, int ac, char **av)
{
    if (gettimeofday(&data_p->simulation_start_time, NULL) != 0)
        exit_error("init_data", "gettimofday failed");
    data_p->simulation_end = 0;
    data_p->philo_nbr = ft_atoi(av[1]);
    data_p->time_to_die = ft_atoi(av[2]);
    data_p->time_to_eat = ft_atoi(av[3]);
    data_p->time_to_sleep = ft_atoi(av[4]);
    if (ac == 6)
        data_p->max_nbr_of_eat = ft_atoi(av[5]);
    if (data_p->philo_nbr <= 0 || data_p->time_to_die <= 0
            || data_p->time_to_eat <= 0 || data_p->time_to_sleep <= 0
            || (ac == 6 && data_p->max_nbr_of_eat == 0))
        exit_error("philo", BAD_ARG_NBR);
    if (pthread_mutex_init(&(data_p->std_lock), NULL) != 0)
        exit_error("init_data", "std_lock mutex failed");
}

static void init_philo(t_data *data)
{
    int index;

    index = 0;
    while (index < data->philo_nbr)
    {
        data->philosopher[index].pos = index;
        data->philosopher[index].alive = 1;
        data->philosopher[index].simulation_end = &data->simulation_end;
        data->philosopher[index].time_to_die = &data->time_to_die;
        data->philosopher[index].time_to_sleep = &data->time_to_sleep;
        data->philosopher[index].time_to_eat = &data->time_to_eat;
        data->philosopher[index].max_nbr_of_eat = data->max_nbr_of_eat;
        data->philosopher[index].last_meal = data->simulation_start_time;
        data->philosopher[index].std_lock = &data->std_lock;
        data->philosopher[index].fork[0] = &data->fork[index];
        data->philosopher[index].fork[1] = &data->fork[(index + 1)
            % data->philo_nbr];
        if (pthread_create(&data->philosopher[index].tid, NULL,
                philo, &data->philosopher[index]) != 0)
        {
            printf("init_philo; pthread_create error\n");
            exit_simulation(data);
        }
        if (pthread_detach(data->philosopher[index].tid) != 0)
        {
            printf("init_philo; pthread_create error\n");
            exit_simulation(data);
        }
        index++;
    }
}

static void init_mutex(t_data *data)
{
    int index = 0;

    while (index < data->philo_nbr)
    {
        if (pthread_mutex_init(&(data->fork[index]), NULL) != 0)
        {
            while (index-- > 0)
                pthread_mutex_destroy(&(data->fork[index]));
            free(data->fork);
            free(data->philosopher);
            pthread_mutex_destroy(&data->std_lock);
            exit_error("mutex_init", "pthread_mutex_init error");
        }
        index++;
    }
}

t_data init_simulation(int ac, char **av)
{
    t_data  data;

    init_data(&data, ac, av);
    data.philosopher = (t_philo *)malloc(sizeof(t_philo) * data.philo_nbr);
    if (!data.philosopher)
        pthread_mutex_destroy(&data.std_lock);
    if (!data.philosopher)
        exit_error("init_simulation", "malloc error");
    data.fork = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)
        * data.philo_nbr);
    if (!data.fork)
    {
        pthread_mutex_destroy(&data.std_lock);
        free(data.philosopher);
        exit_error("init_simulation", "malloc error");
    }
    init_mutex(&data);
    init_philo(&data);
    return (data);
}