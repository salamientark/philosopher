/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 09:15:44 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/27 12:28:15 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

static void	free_all(t_data *data)
{
	free(data->fork);
	free(data->philosopher);
	free(data);
	data = NULL;
}

/*
	Init data
	Simulation_end :
	-1 : thread are created -> every philo wait
	0  : Simulation start / Run
	1  : Simulation stop
*/
static t_data	*init_data(t_data *data_p, int ac, char **av)
{
	data_p->simulation_end = -1;
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
	if (data_p->philo_nbr <= 0
		|| data_p->time_to_die <= 0 || data_p->time_to_eat <= 0
		|| data_p->time_to_sleep <= 0 || data_p->meal_per_philo == -2)
		return (free(data_p), print_error("philo", INVALID_ARG), NULL);
	data_p->fork = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)
			* data_p->philo_nbr);
	if (!data_p->fork)
		return (free(data_p), print_error("init_data", "malloc failed"), NULL);
	data_p->philosopher = (t_philo *)malloc(sizeof(t_philo)
			* data_p->philo_nbr);
	if (!data_p->philosopher)
		return (free(data_p->fork), free(data_p), print_error("init_data",
				"malloc failed"), NULL);
	return (data_p);
}

/*
	False: need to free everything when error in mutex_init
*/
static t_data	*init_mutex(t_data *data)
{
	int	index;

	if (pthread_mutex_init(&data->stdout_lock, NULL) != 0)
		return (free_all(data), print_error("init_mutex", "error"), NULL);
	if (pthread_mutex_init(&data->dead_lock, NULL) != 0)
		return (pthread_mutex_destroy(&data->stdout_lock), free_all(data),
			print_error("init_mutex", "mutex_init error"), NULL);
	if (pthread_mutex_init(&data->meal_lock, NULL) != 0)
		return (pthread_mutex_destroy(&data->stdout_lock),
			pthread_mutex_destroy(&data->dead_lock), free_all(data),
			print_error("init_mutex", "mutex_init error"), NULL);
	index = -1;
	while (++index < data->philo_nbr)
	{
		if (pthread_mutex_init(&(data->fork[index]), NULL) != 0)
		{
			while (index-- > 0)
				pthread_mutex_destroy(&(data->fork[index]));
			return (pthread_mutex_destroy(&data->stdout_lock),
				pthread_mutex_destroy(&data->dead_lock),
				pthread_mutex_destroy(&data->meal_lock), free_all(data),
				print_error("mutex_init", "pthread_mutex_init error"), NULL);
		}
	}
	return (data);
}

/*
	ERROR: Need to wait and free correctly when error
*/
static t_data	*init_philo(t_data *data)
{
	int	index;

	index = 0;
	while (index < data->philo_nbr)
	{
		data->philosopher[index].id = index;
		data->philosopher[index].alive = 1;
		data->philosopher[index].meal_left = data->meal_per_philo;
		data->philosopher[index].data = data;
		if (pthread_create(&data->philosopher[index].tid, NULL,
				philo_routine, &data->philosopher[index]) != 0)
		{
			pthread_mutex_lock(&data->dead_lock);
			data->simulation_end = 1;
			pthread_mutex_unlock(&data->dead_lock);
			while (index-- > 0)
				pthread_join(data->philosopher[index].id, NULL);
			write(2, "init_philo: pthread_create error\n", 33);
			return (exit_simulation(data), NULL);
		}
		index++;
	}
	return (data);
}

/*
	Create data structure -> create mutex -> launch philo_routine
*/
t_data	*init_simulation(int ac, char **av)
{
	t_data	*data;

	data = (t_data *)malloc(sizeof(t_data));
	if (!data)
		return (print_error("init_simulation", "malloc error"), NULL);
	data = init_data(data, ac, av);
	if (!data)
		return (NULL);
	if (data->meal_to_take == 0)
	{
		free_all(data);
		return (NULL);
	}
	data = init_mutex(data);
	if (!data)
		return (NULL);
	data = init_philo(data);
	if (!data)
		return (NULL);
	return (data);
}
