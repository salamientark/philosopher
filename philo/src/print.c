/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 14:24:57 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/05 20:06:37 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

/*
	Log philosopher action in form:
		[timestamp] [philo.id] ms [msg]
	If the philo died (msg == died)
	send simulation_end 'signal'
*/
void	log_philo(t_philo *philo, char *msg)
{
	struct timeval	right_now;
	int				timestamp;

	if (simulation_stopped(philo->data))
		return ;
	pthread_mutex_lock(&philo->data->stdout_lock);
	if (gettimeofday(&right_now, NULL) == 0)
		timestamp = 1000 * (right_now.tv_sec
				- philo->data->simulation_start_time.tv_sec)
			+ (right_now.tv_usec
				- philo->data->simulation_start_time.tv_usec) / 1000;
	else
		timestamp = -1;
	// if (simulation_stopped(philo->data))
	// 	return ((void) pthread_mutex_unlock(&philo->data->stdout_lock));
	printf("%d %d %s\n", timestamp, philo->id + 1, msg);
	if (*msg == 'd')
	{
		pthread_mutex_lock(&philo->data->dead_lock);
		philo->alive = 0;
		philo->data->simulation_end = 1;
		pthread_mutex_unlock(&philo->data->dead_lock);
	}
	pthread_mutex_unlock(&philo->data->stdout_lock);
}
