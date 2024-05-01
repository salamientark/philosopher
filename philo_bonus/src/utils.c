/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 18:01:00 by dbaladro          #+#    #+#             */
/*   Updated: 2024/04/29 22:14:00 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo_bonus.h"

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

const char	*sem_name(int id, char type, char name[])
{
	name[0] = '/';
	name[1] = id / 100 + '0';
	name[2] = (id / 10) % 10 + '0';
	name[3] = id % 10 + '0';
	name[4] = '_';
	name[5] = type;
	name[6] = '\0';
	return ((const char *) name);
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
	usleep(500);
}
