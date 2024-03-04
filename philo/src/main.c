/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 15:50:42 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/04 01:05:59 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

/*
	Return:
	1 : Everybody survived
	0 : Someone died
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
			log_philo(&data_p->philosopher[index], "died");
		index++;
		index %= data_p->philo_nbr;
	}
}

/*
	Init data
	Start threads.
	other thread philo
*/
int	main(int ac, char **av)
{
	t_data	*data;
	int		index;

	if (ac != 5 && ac != 6)
		exit_error("philo", BAD_ARG_NBR);
	data = init_simulation(ac, av);
	monitor(data);
	index = 0;
	while (index < data->philo_nbr)
	{
		pthread_join(data->philosopher[index].tid, NULL);
		index++;
	}
	exit_simulation(data);
	return (0);
}
