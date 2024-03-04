/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 01:05:44 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/04 01:12:25 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

/*
    Return data->simulation_end value
    Mutex protected
*/
int	simulation_stopped(t_data *data)
{
	int	simulation_stoped;

	simulation_stoped = 0;
	pthread_mutex_lock(&data->dead_lock);
	simulation_stoped = data->simulation_end;
	pthread_mutex_unlock(&data->dead_lock);
	return (simulation_stoped);
}
