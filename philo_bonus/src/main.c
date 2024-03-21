/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 08:39:47 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/21 12:39:05 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo_bonus.h"

// void	launch_simulation(t_data *data)
// {
// 	int	index;

// 	index = 0;
// 	while (index < data->philo_nbr)
// 	{
// 		sem_post(data->simulation_stop);
// 		index++;
// 	}
// }

/*
	Init data
	Start threads.
	other thread philo
*/
int	main(int ac, char **av)
{
	t_data	*data;

	if (ac != 5 && ac != 6)
	{
		printf("philo : %s\n", BAD_ARG_NBR);
		exit(EXIT_SUCCESS);
	}
	data = init_simulation(ac, av);
	ft_msleep(999);
	sem_wait(data->simulation_stop);
	int index = 0;
	while (index < data->philo_nbr)
	{
		kill(data->philo_pid[index], SIGKILL);
		index++;
	}
	exit_simulation(data, NULL, NULL);
	return (0);
}