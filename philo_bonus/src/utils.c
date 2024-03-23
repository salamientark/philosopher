/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 18:01:00 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/23 15:43:18 by madlab           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo_bonus.h"

void	print_error(char *func, char *msg)
{
	int		i;
	char	buffer[500];

	i = 0;
	while (*func)
	{
		buffer[i] = *func;
		i++;
		func++;
	}
	buffer[i++] = ' ';
	buffer[i++] = ':';
	buffer[i++] = ' ';
	while (*msg)
	{
		buffer[i] = *msg;
		msg++;
		i++;
	}
	buffer[i] = '\n';
	write(2, buffer, i);
}

/*
	Exit child ps (philo) properly + signal end to main process via sem_post
*/
void	exit_child(t_data *data, char *err_func, char *err_msg)
{
	sem_post(data->simulation_stop);
	if (err_func || err_msg)
	{
		print_error(err_func, err_msg);
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

/*
	DESTROY SEMAPHORE, free(data) EXIT_ERROR
*/
void	exit_simulation(t_data *data, char *func, char *err_msg)
{
	sem_close(data->stdout_sem);
	sem_close(data->fork);
	sem_close(data->dead_sem);
	sem_close(data->eat_sem);
	sem_close(data->meal_sem);
	sem_close(data->simulation_stop);
	sem_unlink(SEM_STDOUT);
	sem_unlink(SEM_FORK);
	sem_unlink(SEM_DEAD);
	sem_unlink(SEM_MEAL);
	sem_unlink(SEM_EAT);
	sem_unlink(SEM_SIMULAION_STOP);
	free(data->philo_pid);
	free(data);
	data = NULL;
	if (!func || !err_msg)
		exit(EXIT_SUCCESS);
	print_error(func, err_msg);
	exit(EXIT_FAILURE);
}

/*
	Improved sleep
	ARG:
	wait_time : in millisecond
*/
void	ft_msleep(long int wait_time)
{
	struct timeval	start;
	struct timeval	now;
	long int		m_start;
	long int		m_now;

	if (gettimeofday(&start, NULL) != 0)
	{
		write(2, "ft_msleep() : gettimeofday error\n", 34);
		return ;
	}
	m_start = 1000 * start.tv_sec + start.tv_usec / 1000;
	m_now = m_start;
	while (m_now - m_start <= wait_time)
	{
		usleep(400);
		if (gettimeofday(&now, NULL) != 0)
		{
			write(2, "ft_msleep() : gettimeofday error\n", 34);
			return ;
		}
		m_now = 1000 * now.tv_sec + now.tv_usec / 1000;
	}
}
