/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 15:50:42 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/20 17:09:38 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

/*
	Set starting time and allow philo thread to start
*/
void	start_simulation(t_data *data)
{
	int	index;

	if (gettimeofday(&data->simulation_start_time, NULL) != 0)
	{
		write(2, "start_simulation: gettimeofday error\n", 38);
		pthread_mutex_lock(&data->dead_lock);
		data->simulation_end = 1;
		pthread_mutex_unlock(&data->dead_lock);
		exit_simulation(data);
	}
	index = 0;
	while (index < data->philo_nbr)
	{
		data->philosopher[index].last_meal = data->simulation_start_time;
		index++;
	}
	pthread_mutex_lock(&data->dead_lock);
	data->simulation_end = 0;
	pthread_mutex_unlock(&data->dead_lock);
}

/*
	Return:
	1 : Everybody survived
	0 : Someone DIED
*/
void	monitor(t_data *data_p)
{
	int				index;
	struct timeval	now;
	struct timeval	last_meal_cp;

	index = 0;
	while (!simulation_stopped(data_p))
	{
		pthread_mutex_lock(&data_p->meal_lock);
		last_meal_cp = data_p->philosopher[index].last_meal;
		if (data_p->meal_to_take == 0)
		{
			pthread_mutex_unlock(&data_p->meal_lock);
			pthread_mutex_lock(&data_p->dead_lock);
			data_p->simulation_end = 1;
			pthread_mutex_unlock(&data_p->dead_lock);
			return ;
		}
		pthread_mutex_unlock(&data_p->meal_lock);
		gettimeofday(&now, NULL);
		if (1000 * (now.tv_sec - last_meal_cp.tv_sec) + (now.tv_usec
				- last_meal_cp.tv_usec) / 1000 > data_p->time_to_die)
			log_philo(&data_p->philosopher[index], DIED);
		index++;
		index %= data_p->philo_nbr;
	}
}

/*
	Init data
	Start threads.
	other thread philo
*/
// MUST REPLACE WRITE
int	main(int ac, char **av)
{
	t_data	*data;

	if (ac != 5 && ac != 6)
		exit_error("philo", BAD_ARG_NBR);
	data = init_simulation(ac, av);
	start_simulation(data);
	monitor(data);
	exit_simulation(data);
	return (0);
}
