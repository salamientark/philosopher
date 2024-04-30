/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 15:50:42 by dbaladro          #+#    #+#             */
/*   Updated: 2024/04/30 13:14:03 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

/*
	Log philosopher action in form:
		[timestamp] [philo.id] ms [msg]
	If the philo DIED (msg == DIED)
	send simulation_end 'signal'
*/
int	log_philo(t_philo *philo, char *msg)
{
	struct timeval	right_now;
	int				timestamp;

	if (simulation_stopped(philo->data))
		return (1);
	pthread_mutex_lock(&philo->data->stdout_lock);
	if (simulation_stopped(philo->data))
		return (pthread_mutex_unlock(&philo->data->stdout_lock), 1);
	timestamp = -1;
	if (gettimeofday(&right_now, NULL) == 0)
		timestamp = 1000 * (right_now.tv_sec
				- philo->data->simulation_start_time.tv_sec)
			+ (right_now.tv_usec
				- philo->data->simulation_start_time.tv_usec) / 1000;
	printf("%d %d %s\n", timestamp, philo->id + 1, msg);
	if (*msg == 'd')
	{
		pthread_mutex_lock(&philo->data->dead_lock);
		philo->alive = 0;
		philo->data->simulation_end = 1;
		pthread_mutex_unlock(&philo->data->dead_lock);
		return (1);
	}
	pthread_mutex_unlock(&philo->data->stdout_lock);
	return (0);
}

/*
	Set starting time and allow philo thread to start
*/
int	start_simulation(t_data *data)
{
	int	index;

	if (gettimeofday(&data->simulation_start_time, NULL) != 0)
	{
		write(2, "start_simulation: gettimeofday error\n", 38);
		pthread_mutex_lock(&data->dead_lock);
		data->simulation_end = 1;
		pthread_mutex_unlock(&data->dead_lock);
		return (exit_simulation(data), ERROR);
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
	return (SUCCESS);
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
			return ((void) pthread_mutex_unlock(&data_p->dead_lock));
		}
		pthread_mutex_unlock(&data_p->meal_lock);
		gettimeofday(&now, NULL);
		if (1000 * (now.tv_sec - last_meal_cp.tv_sec) + (now.tv_usec
				- last_meal_cp.tv_usec) / 1000 > data_p->time_to_die)
			log_philo(&data_p->philosopher[index], DIED);
		index++;
		index %= data_p->philo_nbr;
		usleep(500);
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
		return (print_error("philo", BAD_ARG_NBR), 0);
	data = init_simulation(ac, av);
	if (!data)
		return (0);
	if (start_simulation(data) == ERROR)
		return (0);
	monitor(data);
	exit_simulation(data);
	return (0);
}
