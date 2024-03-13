/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 00:21:49 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/13 17:39:26 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

static void	philo_sleep(t_philo *philo)
{
	unsigned int	ms_since_last_meal;
	struct timeval	now;

	gettimeofday(&now, NULL);
	ms_since_last_meal = 1000 * (now.tv_sec - philo->last_meal.tv_sec)
		+ (now.tv_usec - philo->last_meal.tv_usec) / 1000;
	log_philo(philo, "is sleeping");
	if (ms_since_last_meal + philo->data->time_to_sleep
		> philo->data->time_to_die)
	{
		if (philo->data->time_to_die > ms_since_last_meal)
			ft_msleep(philo->data->time_to_die - ms_since_last_meal + 1);
		log_philo(philo, "died");
		return ;
	}
	ft_msleep(philo->data->time_to_sleep);
}

/*
	Fix bug si philo va mourir pendant le repas ou le sleep
*/
static void	philo_eat(t_philo *philo)
{
	int	first_fork;
	int	second_fork;

	first_fork = (philo->id + (philo->id % 2 == 1)) % philo->data->philo_nbr;
	second_fork = (philo->id + (philo->id % 2 == 0)) % philo->data->philo_nbr;
	pthread_mutex_lock(&(philo->data->fork[first_fork]));
	log_philo(philo, "has taken a fork");
	pthread_mutex_lock(&(philo->data->fork[second_fork]));
	log_philo(philo, "has taken a fork");
	log_philo(philo, "is eating");
	pthread_mutex_lock(&philo->data->meal_lock);
	gettimeofday(&philo->last_meal, NULL);
	philo->data->meal_to_take -= (philo->meal_left > 0);
	pthread_mutex_unlock(&philo->data->meal_lock);
	philo->meal_left -= (philo->meal_left > 0);
	if (philo->data->time_to_eat > philo->data->time_to_die)
	{
		ft_msleep(philo->data->time_to_die);
		log_philo(philo, "died");
	}
	else
		ft_msleep(philo->data->time_to_eat);
	pthread_mutex_unlock(&(philo->data->fork[first_fork]));
	usleep(700);
	pthread_mutex_unlock(&(philo->data->fork[second_fork]));
}

void	*philo_routine(void *param)
{
	t_philo	*philosopher;

	philosopher = (t_philo *)param;
	while (simulation_stopped(philosopher->data) == -1)
		usleep(10);
	if (philosopher->id % 2)
		usleep(15000);
	while (!simulation_stopped(philosopher->data))
	{
		philo_eat(philosopher);
		// if (simulation_stopped(philosopher->data))
		// 	break ;
		philo_sleep(philosopher);
		// if (simulation_stopped(philosopher->data))
		// 	break ;
		// usleep(100);
		log_philo(philosopher, "is thinking");
		// usleep(100);
	}
	return (NULL);
}
