/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 09:15:44 by dbaladro          #+#    #+#             */
/*   Updated: 2024/02/29 16:37:05 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

// Return -2 in case of invalid arg
static int	ft_atoi(char *s)
{
	long	nbr;
	int		sign;
	int		index;

	sign = 1;
	nbr = 0;
	index = 0;
	while (s[index] && ((9 <= s[index] && s[index] <= 13) || s[index] == ' '))
		index++;
	while (s[index] && (s[index] == '-' || s[index] == '+'))
	{
		if (s[index] == '-')
			sign *= -1;
		index++;
	}
	while (s[index] && ('0' <= s[index] && s[index] <= '9'))
	{
		nbr = 10 * nbr + (s[index] - '0');
		if ((sign && nbr > INT_MAX) || (-nbr) < INT_MIN)
			return (-2);
		index++;
	}
	if (s[index])
		return (-2);
	return (sign * nbr);
}

static void	init_data(t_data *data_p, int ac, char **av)
{
	data_p->simulation_end = 0;
	data_p->philo_nbr = ft_atoi(av[1]);
	data_p->time_to_die = ft_atoi(av[2]);
	data_p->time_to_eat = ft_atoi(av[3]);
	data_p->time_to_sleep = ft_atoi(av[4]);
	data_p->meal_per_philo = -1;
	data_p->meal_to_take = -1;
	if (ac == 6)
		data_p->meal_per_philo = ft_atoi(av[5]);
	if (ac == 6)
		data_p->meal_to_take = (data_p->meal_per_philo * data_p->philo_nbr);
	if (data_p->philo_nbr <= 0 || data_p->philo_nbr > 200 
		|| data_p->time_to_die <= 0 || data_p->time_to_eat <= 0
		|| data_p->time_to_sleep <= 0 || data_p->meal_per_philo == -2)
		return (free(data_p), exit_error("philo", BAD_ARG_NBR));
	if (gettimeofday(&data_p->simulation_start_time, NULL) != 0)
		return (free(data_p), exit_error("init_data", "gettimofday failed"));
	data_p->fork = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)
			* data_p->philo_nbr);
	if (!data_p->fork)
		return (free(data_p), exit_error("init_data", "malloc failed"));
	data_p->philosopher = (t_philo *)malloc(sizeof(t_philo)
			* data_p->philo_nbr);
	if (!data_p->philosopher)
		return (free(data_p->fork), free(data_p), exit_error("init_data",
				"malloc failed"));
}

// NEW INIT PHILO
static void	init_philo(t_data *data)
{
	int	index;

	index = 0;
	while (index < data->philo_nbr)
	{
		data->philosopher[index].id = index;
		data->philosopher[index].alive = 1;
		data->philosopher[index].meal_left = data->meal_per_philo;
		data->philosopher[index].last_meal = data->simulation_start_time;
		data->philosopher[index].data = data;
		if (pthread_create(&data->philosopher[index].tid, NULL,
				better_philo, &data->philosopher[index]) != 0)
		{
			printf("init_philo; pthread_create error\n");
			exit_simulation(data);
		}
		index++;
	}
}

static void	init_mutex(t_data *data)
{
	int	index;

	if (pthread_mutex_init(&data->stdout_lock, NULL) != 0)
		return (free(data), exit_error("init_mutex: ", "mutex_init error"));
	if (pthread_mutex_init(&data->dead_lock, NULL) != 0)
		return (free(data), exit_error("init_mutex: ", "mutex_init error"));
	if (pthread_mutex_init(&data->meal_lock, NULL) != 0)
		return (free(data), exit_error("init_mutex: ", "mutex_init error"));
	index = 0;
	while (index < data->philo_nbr)
	{
		if (pthread_mutex_init(&(data->fork[index]), NULL) != 0)
		{
			while (index-- > 0)
				pthread_mutex_destroy(&(data->fork[index]));
			free(data->fork);
			free(data->philosopher);
			pthread_mutex_destroy(&data->stdout_lock);
			pthread_mutex_destroy(&data->dead_lock);
			pthread_mutex_destroy(&data->meal_lock);
			free(data);
			exit_error("mutex_init", "pthread_mutex_init error");
		}
		index++;
	}
}

/*
	Create data structure -> create mutex -> launch philo_routine
*/
t_data	*init_simulation(int ac, char **av)
{
	t_data	*data;

	data = (t_data *)malloc(sizeof(t_data));
	if (!data)
		exit_error("init_simulation", "malloc error");
	init_data(data, ac, av);
	if (data->philo_nbr == 1)
	{
		printf("0 ms 1 has taken a fork\n");
		usleep(1000 * data->time_to_die);
		printf("%ld ms 1 died\n", data->time_to_die);
	}
	if (data->philo_nbr == 1 || data->meal_to_take == 0)
	{
		free(data->fork);
		free(data->philosopher);
		free(data);
		exit(EXIT_SUCCESS);
	}
	init_mutex(data);
	init_philo(data);
	return (data);
}
