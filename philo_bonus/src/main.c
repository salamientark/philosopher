/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 08:39:47 by dbaladro          #+#    #+#             */
/*   Updated: 2024/05/01 11:47:37 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo_bonus.h"

/*
	DESTROY SEMAPHORE, free(data) EXIT_ERROR
*/
void	exit_simulation(t_data *data, char *func, char *err_msg)
{
	sem_close(data->stdout_sem);
	sem_close(data->fork);
	sem_close(data->meal_sem);
	sem_close(data->simulation_stop);
	sem_close(data->end_simu);
	sem_unlink(SEM_STDOUT);
	sem_unlink(SEM_FORK);
	sem_unlink(SEM_MEAL);
	sem_unlink(SEM_SIMULAION_STOP);
	sem_unlink(SEM_END_SIMU);
	free(data->philo_pid);
	free(data);
	data = NULL;
	if (!func || !err_msg)
		exit(EXIT_SUCCESS);
	print_error(func, err_msg);
	exit(EXIT_FAILURE);
}

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

// static void	kill_child(t_data *data)
// {
// 	int	index;

// 	index = 0;
// 	usleep(500);
// 	while (index < data->philo_nbr)
// 	{
// 		kill(data->philo_pid[index], SIGKILL);
// 		index++;
// 	}
// }

static void	end_simulation(t_data *data, pthread_t meal_checker)
{
	int	index;

	index = 0;
	sem_post(data->end_simu);
	ft_msleep(100);
	index = 0;
	while (index++ < data->philo_nbr)
		sem_post(data->meal_sem);
	pthread_join(meal_checker, NULL);
	index = 0;
	while (index++ < data->philo_nbr)
		waitpid(-1, NULL, 0);
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

	if (ac != 5 && ac != 6)
	{
		printf("philo : %s\n", BAD_ARG_NBR);
		exit(EXIT_SUCCESS);
	}
	sem_unlink(SEM_FORK);
	sem_unlink(SEM_MEAL);
	sem_unlink(SEM_STDOUT);
	sem_unlink(SEM_SIMULAION_STOP);
	sem_unlink(SEM_END_SIMU);
	data = init_simulation(ac, av);
	if (pthread_create(&meal_checker, NULL, meal_check, (void *)data) != 0)
	{
		sem_post(data->end_simu);
		ft_msleep(100);
		exit_simulation(data, "main", "pthread_create failed");
	}
	ft_msleep(999);
	sem_wait(data->simulation_stop);
	end_simulation(data, meal_checker);
	exit_simulation(data, NULL, NULL);
	return (0);
}
