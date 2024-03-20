/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 08:39:47 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/20 23:45:54 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo_bonus.h"

void	launch_simulation(t_data *data)
{
	int	index;

	index = 0;
	while (index < data->philo_nbr)
	{
		sem_post(data->philo_alive);
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
	t_data	*data;

	if (ac != 5 && ac != 6)
		printf("philo : %s\n", BAD_ARG_NBR);
	data = init_simulation(ac, av);
	launch_simulation(data);
	// Une fois qu'un philo meurt et libere un philo_alive
	// kill tout les philos
	ft_msleep(data->time_to_die / 10);
	sem_wait(data->philo_alive);
	int index = 0;
	while (index < data->philo_nbr)
	{
		kill(data->philo_pid[index], SIGKILL);
		waitpid(data->philo_pid[index], NULL, 0);
		index++;
	}
	//
	exit_simulation(data, NULL, NULL);
	return (0);
}