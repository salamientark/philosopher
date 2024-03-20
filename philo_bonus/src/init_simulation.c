/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 09:15:44 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/20 17:19:41 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo_bonus.h"

/*
	Should create structure, open semp stdout & fork in main process.
	Then create process for every philo sending SIGSTOP signal right after
*/

void	exit_error(char *func, char *msg)
{
	print_error(func, msg);
	exit(EXIT_FAILURE);
}

/*
	DESTROY SEMAPHORE, free(data) EXIT_ERROR
*/
void	exit_simulation(t_data *data, char *func, char *err_msg)
{
	sem_close(data->stdout_sem);
	sem_close(data->fork);
	sem_close(data->philo_alive);
	sem_unlink(data->stdout_sem);
	sem_unlink(data->fork);
	sem_unlink(data->philo_alive);
	free(data);
	data = NULL;
	if (!func || !err_msg)
		exit(EXIT_SUCCESS);
	exit_error(func, err_msg);
}

/*
	Init data
	Simulation_end :
	-1 : thread are created -> every philo wait
	0  : Simulation start / Run
	1  : Simulation stop
*/
static void	init_data(t_data *data_p, int ac, char **av)
{
	data_p->philo_nbr = ft_atoi(av[1]);
	data_p->time_to_die = ft_atoi(av[2]);
	data_p->time_to_eat = ft_atoi(av[3]);
	data_p->time_to_sleep = ft_atoi(av[4]);
	data_p->meal_to_take = -1;
	if (ac == 6)
		data_p->meal_to_take = ft_atoi(av[5]);
	if (data_p->philo_nbr <= 0 || data_p->time_to_die <= 0
		|| data_p->time_to_eat <= 0 || data_p->time_to_sleep <= 0
		|| data_p->meal_to_take == -2)
		return (free(data_p), exit_error("philo", INVALID_ARG));
	data_p->philo_pid = (pid_t *)malloc(sizeof(pid_t) * data_p->philo_nbr);
	if (!data_p->philo_pid)
	{
		free(data_p);
		printf("%s : %s\n", "philo", INVALID_ARG);
		exit(EXIT_FAILURE);
	}
}

/*
	False: need to free everything when error in mutex_init
*/
static void	init_semaphore(t_data *data)
{
	// clean semaphores ?
	sem_unlink(SEM_FORK);
	sem_unlink(SEM_PHILO_ALIVE);
	sem_unlink(SEM_STDOUT);
	// Create stdout_semaphore
	data->stdout_sem = sem_open(SEM_STDOUT, O_CREAT | O_EXCL, 0660, 1);
	if (data->stdout_sem = SEM_FAILED)
		return (exit_simulation(data, "init_semaphore", "sem_open failed"));
	// Create fork_sem
	data->fork = sem_open(SEM_FORK, O_CREAT | O_EXCL, 0660, data->philo_nbr);
	if (data->fork = SEM_FAILED)
		return (exit_simulation(data, "init_semaphore", "sem_open failed"));
	// CREATE Philo_alive sem
	data->stdout_sem = sem_open(SEM_PHILO_ALIVE, O_CREAT | O_EXCL, 0660, data->philo_nbr);
	if (data->philo_alive = SEM_FAILED)
		return (exit_simulation(data, "init_semaphore", "sem_open failed"));
	
	int index = 0;
	while (index < data->philo_nbr);
	{
		sem_wait(data->philo_alive);
		index++;
	}
}

/*
	ERROR: Need to wait and free correctly when error
*/
static void	init_philo(t_data *data)
{
	int	index;

	index = 0;
	while (index < data->philo_nbr)
	{
		data->philo_pid[index] = fork();
		if (data->philo_pid[index] == -1)
		{
			while (index-- > 0)
				kill(data->philo_pid[index], SIGKILL);
			exit_simulation(data, "init_philo", "fork_error");
		}
		if (data->philo_pid[index] == 0)
		{
			data->philo_id = index;
			philo_routine(data);
			exit(EXIT_SUCCESS);
		}
		// kill(data->philo_pid[index], SIGSTOP);
		index++;
	}
}

/*
	Create data structure -> create mutex -> launch philo_routine
*/
t_data	*init_simulation(int ac, char **av)
{
	t_data	*data;
	int		index;

	data = (t_data *)malloc(sizeof(t_data));
	if (!data)
	// {
		// print_error("init_simulation", "malloc error");
		// exit(EXIT_FAILURE)
	// }
		exit_error("init_simulation", "malloc error");
	init_data(data, ac, av);
	if (data->meal_to_take == 0)
	{
		free_all(data);
		exit(EXIT_SUCCESS);
	}
	init_semaphore(data);
	index = 0;
	init_philo(data);
	return (data);
}
