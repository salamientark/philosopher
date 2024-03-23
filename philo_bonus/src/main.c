/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 08:39:47 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/23 15:37:39 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo_bonus.h"

static void	*meal_check(void *param)
{
	t_data	*data;
	int		index;

	data = (t_data *)param;
	if (data->meal_to_take == 0)
		return ((void *) NULL);
	index = 0;
	ft_msleep(998);
	while (index < data->philo_nbr)
	{
		sem_wait(data->meal_sem);
		index++;
	}
	sem_post(data->simulation_stop);
	return ((void *) NULL);
}

static void	kill_child(t_data *data)
{
	int	index;

	index = 0;
	while (index < data->philo_nbr)
	{
		kill(data->philo_pid[index], SIGKILL);
		index++;
	}
}

/*
	Init data
	Start threads.
	other thread philo
*/
int	main(int ac, char **av)
{
	t_data		*data;
	pthread_t	meal_checker;
	int			index;

	if (ac != 5 && ac != 6)
	{
		printf("philo : %s\n", BAD_ARG_NBR);
		exit(EXIT_SUCCESS);
	}
	data = init_simulation(ac, av);
	if (pthread_create(&meal_checker, NULL, meal_check, (void *)data) != 0)
	{
		kill_child(data);
		exit_simulation(data, "main", "pthread_create failed");
	}
	ft_msleep(999);
	sem_wait(data->simulation_stop);
	kill_child(data);
	index = 0;
	while (index++ < data->philo_nbr)
		sem_post(data->meal_sem);
	pthread_join(meal_checker, NULL);
	exit_simulation(data, NULL, NULL);
	return (0);
}
