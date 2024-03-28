/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 12:21:35 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/28 08:56:55 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

// Return -2 in case of invalid arg
int	ft_atoi(char *s)
{
	long	nbr;
	int		sign;
	int		index;

	sign = 1;
	nbr = 0;
	index = 0;
	while (s[index] && ((9 <= s[index] && s[index] <= 13) || s[index] == ' '))
		index++;
	while (s[index] && (s[index] == '-' || s[index] == '+'))
	{
		if (s[index] == '-')
			sign *= -1;
		index++;
	}
	while (s[index] && ('0' <= s[index] && s[index] <= '9'))
	{
		nbr = 10 * nbr + (s[index] - '0');
		if ((sign && nbr > INT_MAX) || (-nbr) < INT_MIN)
			return (-2);
		index++;
	}
	if (s[index] || sign == -1)
		return (-2);
	return (sign * nbr);
}

void	print_error(char *func_name, char *msg)
{
	int		i;
	char	buffer[500];

	i = 0;
	while (*func_name)
	{
		buffer[i] = *func_name;
		i++;
		func_name++;
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
	msleep = millisecond sleep
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
	while (m_now - m_start < wait_time)
	{
		usleep(500);
		if (gettimeofday(&now, NULL) != 0)
		{
			write(2, "ft_msleep() : gettimeofday error\n", 34);
			return ;
		}
		m_now = 1000 * now.tv_sec + now.tv_usec / 1000;
	}
}

/*
	Wait for thrad to finish then properly exit simulation
*/
void	exit_simulation(t_data *data_p)
{
	int	index;

	index = 0;
	while (index < data_p->philo_nbr)
	{
		pthread_join(data_p->philosopher[index].tid, NULL);
		index++;
	}
	index = 0;
	while (index < data_p->philo_nbr)
	{
		pthread_mutex_destroy(&(data_p->fork[index]));
		index++;
	}
	pthread_mutex_destroy(&(data_p->stdout_lock));
	pthread_mutex_destroy(&(data_p->dead_lock));
	pthread_mutex_destroy(&(data_p->meal_lock));
	free(data_p->fork);
	free(data_p->philosopher);
	free(data_p);
}

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
