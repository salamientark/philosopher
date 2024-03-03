/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 00:21:49 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/04 00:43:40 by madlab           ###   ########.fr       */
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
			usleep((philo->data->time_to_die - ms_since_last_meal) * 1000);
		log_philo(philo, "died");
		return ;
	}
	usleep(1000 * (philo->data->time_to_sleep));
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
	if (philo->meal_left > 0)
		philo->data->meal_to_take--;
	pthread_mutex_unlock(&philo->data->meal_lock);
	if (philo->meal_left > 0)
		philo->meal_left--;
	// if (philo->data->time_to_eat > philo->data->time_to_die)
	// {
	//	 usleep(1000 * philo->data->time_to_die);
	//	 log_philo(philo, "died");
	// }
	// else
	usleep(1000 * philo->data->time_to_eat);
	pthread_mutex_unlock(&(philo->data->fork[first_fork]));
	pthread_mutex_unlock(&(philo->data->fork[second_fork]));
}

void	*philo_routine(void *param)
{
	t_philo	*philosopher;

	philosopher = (t_philo *)param;
	if (philosopher->id % 2)
		usleep(5000);
	while (1)
	{
		philo_eat(philosopher);
		if (simulation_stopped(philosopher->data))
			break ;
		philo_sleep(philosopher);
		if (simulation_stopped(philosopher->data))
			break ;
		log_philo(philosopher, "is thinking");
		usleep(10);
	}
	return (NULL);
}
