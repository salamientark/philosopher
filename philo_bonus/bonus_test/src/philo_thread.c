/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_thread.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/23 15:14:49 by dbaladro          #+#    #+#             */
/*   Updated: 2024/05/01 10:48:31 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo_bonus.h"

/*
	Check if the simulation end : if another philo die
	it will 'kill' every philo properly leaving time to exit properly
*/
void	*check_simulation_end(void *param)
{
	t_data	*data;

	data = (t_data *)param;
	sem_wait(data->end_simu);
	sem_wait(data->dead_sem);
	data->philo_live = 0;
	sem_post(data->dead_sem);
	sem_post(data->end_simu);
	return ((void *) NULL);
}

/*
	death_checker is the routine associated to a philo thread.
	It check the -last_meal- then wait until this moment to check
	if the -last_meal- timestamp it saved and the one it read again
	are the same.
	If YES it means that the philo have not eaten and it should die.
	If NO it means it have eaten and so it still live.
	This check will be performed until the philo die
	OR the simulation stop.
*/

/*
	wait until estimated death time
*/
static void	prepare_to_die(t_data *data, struct timeval last_meal_cp)
{
	struct timeval	now;
	int				ms_since_last_meal;

	if (gettimeofday(&now, NULL) != 0)
		exit_child(data, "check_death", "gettimeofday error");
	ms_since_last_meal = 1000 * (now.tv_sec - last_meal_cp.tv_sec)
		+ (now.tv_usec - last_meal_cp.tv_usec) / 1000;
	if (ms_since_last_meal > data->time_to_die)
	{
		log_philo(data, DIED);
		return ;
	}
	ft_msleep(data->time_to_die - ms_since_last_meal - 2);
	while (ms_since_last_meal < data->time_to_die)
	{
		if (gettimeofday(&now, NULL) != 0)
			exit_child(data, "check_death", "gettimeofday error");
		ms_since_last_meal = 1000 * (now.tv_sec - last_meal_cp.tv_sec)
			+ (now.tv_usec - last_meal_cp.tv_usec) / 1000;
		usleep(500);
	}
}

static int	philo_died(t_data *data)
{
	sem_wait(data->dead_sem);
	if (data->philo_live == 0)
	{
		sem_post(data->dead_sem);
		return (1);
	}
	sem_wait(data->dead_sem);
	return (0);
}

/*
	This have been splitted because of norminette
*/
void	*death_checker(void *param)
{
	struct timeval	last_meal_cp;
	t_data			*data;

	data = (t_data *)param;
	sem_wait(data->eat_sem);
	last_meal_cp = data->last_meal;
	sem_post(data->eat_sem);
	while (1)
	{
		prepare_to_die(data, last_meal_cp);
		sem_wait(data->eat_sem);
		if (last_meal_cp.tv_usec == data->last_meal.tv_usec
			&& last_meal_cp.tv_sec == data->last_meal.tv_sec)
		{
			log_philo(data, DIED);
			sem_post(data->eat_sem);
			return ((void *) NULL);
		}
		last_meal_cp = data->last_meal;
		sem_post(data->eat_sem);
		if (philo_died(data))
			return ((void *) NULL);
	}
}
