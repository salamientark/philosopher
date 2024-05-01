/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 11:36:38 by dbaladro          #+#    #+#             */
/*   Updated: 2024/05/01 12:25:21 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo_bonus.h"

/*
	Print action + block and exit if philo died
*/
int	log_philo(t_data *data, char *msg)
{
	struct timeval	now;
	int				timestamp;

	sem_wait(data->stdout_sem);
	sem_wait(data->dead_sem);
	if (data->philo_live == 0)
		return (sem_post(data->simulation_stop), sem_post(data->dead_sem),
			sem_post(data->stdout_sem), 1);
	sem_post(data->dead_sem);
	if (gettimeofday(&now, NULL) != 0)
		exit_child(data, "check_death", "gettimeofday error");
	timestamp = (now.tv_sec - data->simulation_start_time.tv_sec) * 1000
		+ (now.tv_usec - data->simulation_start_time.tv_usec) / 1000;
	printf("%d %d %s\n", timestamp, data->philo_id + 1, msg);
	if (*msg == 'd')
	{
		sem_post(data->simulation_stop);
		sem_wait(data->dead_sem);
		data->philo_live = 0;
		sem_post(data->dead_sem);
		usleep(50000);
		return (sem_post(data->stdout_sem), 1);
	}
	sem_post(data->stdout_sem);
	return (0);
}

/* Check of time_to_eat > time_to_die*/
int	philo_eat(t_data *data)
{
	if (log_philo(data, EAT))
		return (sem_post(data->fork), sem_post(data->fork), 1);
	if (data->time_to_eat > data->time_to_die)
	{
		ft_msleep(data->time_to_die);
		usleep(500);
		log_philo(data, DIED);
		return (sem_post(data->fork), sem_post(data->fork), 1);
	}
	sem_wait(data->eat_sem);
	if (gettimeofday(&data->last_meal, NULL) != 0)
		exit_child(data, "check_death", "gettimeofday error");
	sem_post(data->eat_sem);
	ft_msleep(data->time_to_eat);
	sem_post(data->fork);
	sem_post(data->fork);
	return (0);
}

/* Check of time_to_sleep > time_to_die */
int	philo_sleep(t_data *data)
{
	if (log_philo(data, SLEEP))
		return (1);
	if (data->time_to_sleep > data->time_to_die)
	{
		ft_msleep(data->time_to_die);
		log_philo(data, DIED);
		return (1);
	}
	ft_msleep(data->time_to_sleep);
	return (0);
}
