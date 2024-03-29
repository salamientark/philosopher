/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 14:52:59 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/29 11:06:55 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
    Philo_child process work as follow:
	Create the thread to check fo it's death. <- No error when sem_waiting
		MAYBE IT CAN BE SKIPPED BASED ON HOW THE delay FUNCTION IS IMPLEMENTED
		THE PHILO SHOULDN'T BE BLOCKED WHEN WAITING FOR A SEMAPHORE
		-> stdout_sem ? death_sem ? meal_sem
	wait_until the simulation start
	launch infinite loop of philo action (EAT / SLEEP / THINK)
*/

#include "../includes/philo_bonus.h"

/*
	Print action + block and exit if philo died
*/
void	log_philo(t_data *data, char *msg)
{
	struct timeval	now;
	int				timestamp;

	sem_wait(data->stdout_sem);
	sem_wait(data->dead_sem);
	if (data->philo_live == 0)
	{
		sem_post(data->simulation_stop);
		exit(EXIT_SUCCESS);
	}
	sem_post(data->dead_sem);
	if (gettimeofday(&now, NULL) != 0)
		exit_child(data, "check_death", "gettimeofday error");
	timestamp = (now.tv_sec - data->simulation_start_time.tv_sec) * 1000
		+ (now.tv_usec - data->simulation_start_time.tv_usec) / 1000;
	printf("%d %d %s\n", timestamp, data->philo_id + 1, msg);
	if (*msg == 'd')
	{
		sem_wait(data->dead_sem);
		data->philo_live = 0;
		sem_post(data->dead_sem);
		sem_post(data->simulation_stop);
		exit(EXIT_SUCCESS);
	}
	sem_post(data->stdout_sem);
}

/*
	Avoid data rac
*/
static void	delay(t_data *data)
{
	struct timeval	now;
	long int		ms_since_last_meal;

	gettimeofday(&now, NULL);
	sem_wait(data->eat_sem);
	ms_since_last_meal = 1000 * (now.tv_sec - data->last_meal.tv_sec)
		+ (now.tv_usec - data->last_meal.tv_usec) / 1000;
	sem_post(data->eat_sem);
	if (data->philo_nbr % 2 == 1)
	{
		if (ms_since_last_meal < 3 * data->time_to_eat)
			ft_msleep(3 * data->time_to_eat - ms_since_last_meal);
	}
	else
	{
		if (ms_since_last_meal < 2 * data->time_to_eat)
			ft_msleep(2 * data->time_to_eat - ms_since_last_meal);
	}
}

/*
	The process exit if the philosopher die BUT it only break if the meal
	are taken in order to signal a different end to main process
		every meal are taken VS Phil died
*/
static void	philo_live(t_data *data)
{
	while (1)
	{
		sem_wait(data->fork);
		log_philo(data, TAKE_FORK);
		sem_wait(data->fork);
		log_philo(data, TAKE_FORK);
		log_philo(data, EAT);
		sem_wait(data->eat_sem);
		if (gettimeofday(&data->last_meal, NULL) != 0)
			exit_child(data, "check_death", "gettimeofday error");
		sem_post(data->eat_sem);
		if (data->meal_to_take > 0)
			data->meal_to_take--;
		if (data->meal_to_take == 0)
			break ;
		ft_msleep(data->time_to_eat);
		sem_post(data->fork);
		sem_post(data->fork);
		log_philo(data, SLEEP);
		ft_msleep(data->time_to_sleep);
		log_philo(data, THINK);
		delay(data);
	}
	sem_post(data->fork);
	sem_post(data->fork);
}

/*
	Simulation start time is mostly in the future in order to sync the philo
	together. So it just wait until it's time to eat !
*/
static void	wait_simulation_start(t_data *data)
{
	struct timeval	now;
	long int		simu_start_ms;
	long int		now_ms;
	int				philo_sync;

	if (gettimeofday(&now, NULL) != 0)
		exit_child(data, "philo_routine", "gettimeofday error");
	now_ms = 1000 * now.tv_sec + now.tv_usec / 1000;
	simu_start_ms = 1000 * data->simulation_start_time.tv_sec
		+ data->simulation_start_time.tv_usec / 1000;
	while (now_ms < simu_start_ms)
	{
		usleep(400);
		if (gettimeofday(&now, NULL) != 0)
			exit_child(data, "philo_routine", "gettimeofday error");
		now_ms = 1000 * now.tv_sec + now.tv_usec / 1000;
	}
	if (data->philo_nbr % 2 == 0)
		return (ft_msleep(data->time_to_eat * (data->philo_id % 2 == 1)));
	philo_sync = data->philo_nbr / 2;
	if (data->philo_id >= philo_sync)
		ft_msleep(data->time_to_eat - 1);
	if (data->philo_id == data->philo_nbr - 1)
		ft_msleep(data->time_to_eat - 1);
}

/*
	The sem_post( meal_sem ) only happen if the philo didn't die to signal
	to the main process it should not kill everybody for the moment.
	If the philo died the process exit directy without sem_post( meal_sem )
*/
void	philo_routine(t_data *data)
{
	pthread_t	death_thread;

	sem_wait(data->simulation_stop);
	sem_wait(data->meal_sem);
	data->last_meal = data->simulation_start_time;
	if (pthread_create(&death_thread, NULL, death_checker, (void *)data) != 0)
	{
		sem_post(data->simulation_stop);
		print_error("philo_routine", "pthread_create error");
		exit(EXIT_FAILURE);
	}
	pthread_detach(death_thread);
	wait_simulation_start(data);
	philo_live(data);
	sem_post(data->meal_sem);
	exit(EXIT_SUCCESS);
}
