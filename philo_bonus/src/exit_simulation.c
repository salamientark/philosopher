/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/13 23:40:14 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/20 17:17:09 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void	print_error(char *func, char *msg)
{
	int		i;
	char	buffer[500];

	i = 0;
	while (*func)
	{
		buffer[i] = *func;
		i++;
		func++;
	}
	buffer[i++] = ' ';
	buffer[i++] = ':';
	buffer[i++] = ' ';
	while (*msg)
	{
		buffer[i] = *msg;
		msg++;
		i++;
	}
	buffer[i] = '\n';
	write(2, buffer, i);
}

void	exit_error(char *func, char *msg)
{
	print_error(func, msg);
	exit(EXIT_FAILURE);
}

/*
	Wait for thrad to finish then properly exit simulation
*/
void	exit_simulation(t_data *data_p)
{
	int	index;

	index = 0;
	while (index < data_p->philo_nbr)
	{
		pthread_join(data_p->philosopher[index].tid, NULL);
		index++;
	}
	index = 0;
	while (index < data_p->philo_nbr)
	{
		pthread_mutex_destroy(&(data_p->fork[index]));
		index++;
	}
	pthread_mutex_destroy(&(data_p->stdout_lock));
	pthread_mutex_destroy(&(data_p->dead_lock));
	pthread_mutex_destroy(&(data_p->meal_lock));
	free(data_p->fork);
	free(data_p->philosopher);
	free(data_p);
}
