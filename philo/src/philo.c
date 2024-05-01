/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 00:21:49 by dbaladro          #+#    #+#             */
/*   Updated: 2024/05/01 18:13:50 by dbaladro         ###   ########.fr       */
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
	if (log_philo(philo, SLEEP) == 1)
		return ;
	if (ms_since_last_meal + philo->data->time_to_sleep
		> philo->data->time_to_die)
	{
		if (philo->data->time_to_die > ms_since_last_meal)
			ft_msleep(philo->data->time_to_die - ms_since_last_meal + 1);
		log_philo(philo, DIED);
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
	log_philo(philo, TAKE_FORK);
	pthread_mutex_lock(&(philo->data->fork[second_fork]));
	log_philo(philo, TAKE_FORK);
	if (log_philo(philo, EAT))
		return ((void) pthread_mutex_unlock(&(philo->data->fork[first_fork])),
			(void) pthread_mutex_unlock(&(philo->data->fork[second_fork])));
	pthread_mutex_lock(&philo->data->meal_lock);
	gettimeofday(&philo->last_meal, NULL);
	philo->data->meal_to_take -= (philo->meal_left > 0);
	pthread_mutex_unlock(&philo->data->meal_lock);
	philo->meal_left -= (philo->meal_left > 0);
	if (philo->data->time_to_eat > philo->data->time_to_die)
		ft_msleep(philo->data->time_to_die + 1);
	if (philo->data->time_to_eat > philo->data->time_to_die)
		log_philo(philo, DIED);
	else
		ft_msleep(philo->data->time_to_eat);
	pthread_mutex_unlock(&(philo->data->fork[first_fork]));
	pthread_mutex_unlock(&(philo->data->fork[second_fork]));
}

static void	delay(t_philo *philo)
{
	struct timeval	now;
	struct timeval	last_meal_cp;
	long			ms_since_last_meal;

	if (simulation_stopped(philo->data))
		return ;
	pthread_mutex_lock(&philo->data->meal_lock);
	last_meal_cp = philo->last_meal;
	pthread_mutex_unlock(&philo->data->meal_lock);
	gettimeofday(&now, NULL);
	ms_since_last_meal = 1000 * (now.tv_sec - last_meal_cp.tv_sec)
		+ (now.tv_usec - last_meal_cp.tv_usec) / 1000;
	if (philo->data->philo_nbr % 2 == 1)
	{
		if (ms_since_last_meal < philo->data->time_to_eat * 3)
			ft_msleep(3 * philo->data->time_to_eat - ms_since_last_meal - 1);
		return ;
	}
	if (ms_since_last_meal < philo->data->time_to_eat * 2)
		ft_msleep(2 * philo->data->time_to_eat - ms_since_last_meal - 1);
}

static int	wait_to_start(t_philo *philo)
{
	if (philo->data->time_to_eat > philo->data->time_to_die)
	{
		ft_msleep(philo->data->time_to_die + 1);
		log_philo(philo, DIED);
		return (1);
	}
	ft_msleep(philo->data->time_to_eat * (philo->id % 2 == 1));
	ft_msleep(philo->data->time_to_eat * (philo->data->philo_nbr % 2
			&& philo->id == philo->data->philo_nbr - 1));
	return (0);
}

void	*philo_routine(void *param)
{
	t_philo	*philosoph;

	philosoph = (t_philo *)param;
	if (philosoph->data->philo_nbr == 1)
	{
		pthread_mutex_lock(&philosoph->data->fork[0]);
		log_philo(philosoph, TAKE_FORK);
		ft_msleep(philosoph->data->time_to_die + 1);
		log_philo(philosoph, DIED);
		pthread_mutex_unlock(&philosoph->data->fork[0]);
		return (NULL);
	}
	while (simulation_stopped(philosoph->data) == -1)
		usleep(10);
	if (wait_to_start(philosoph) == 1)
		return (NULL);
	while (!simulation_stopped(philosoph->data))
	{
		philo_eat(philosoph);
		philo_sleep(philosoph);
		log_philo(philosoph, THINK);
		delay(philosoph);
	}
	return (NULL);
}
