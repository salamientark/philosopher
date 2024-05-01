/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_simulation.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 09:15:44 by dbaladro          #+#    #+#             */
/*   Updated: 2024/05/01 10:40:21 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo_bonus.h"

/*
	Init data
	Simulation_end :
	-1 : thread are created -> every philo wait
	0  : Simulation start / Run
	1  : Simulation stop
*/
static void	init_data(t_data *data_p, int ac, char **av)
{
	data_p->philo_live = 1;
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
	{
		free(data_p);
		print_error("philo", INVALID_ARG);
		exit(EXIT_FAILURE);
	}
	data_p->philo_pid = (pid_t *)malloc(sizeof(pid_t) * data_p->philo_nbr);
	if (!data_p->philo_pid)
	{
		free(data_p);
		print_error("init_data", "malloc error");
		exit(EXIT_FAILURE);
	}
}

/*
	False: need to free everything when error in mutex_init
*/
static void	init_semaphore(t_data *data)
{
	data->stdout_sem = sem_open(SEM_STDOUT, O_CREAT | O_EXCL, 0660, 1);
	if (data->stdout_sem == SEM_FAILED)
		exit_simulation(data, "init_semaphore", "sem_open failed");
	data->fork = sem_open(SEM_FORK, O_CREAT | O_EXCL, 0660, data->philo_nbr);
	if (data->fork == SEM_FAILED)
		exit_simulation(data, "init_semaphore", "sem_open failed");
	data->simulation_stop = sem_open(SEM_SIMULAION_STOP, O_CREAT | O_EXCL,
			0660, data->philo_nbr);
	if (data->simulation_stop == SEM_FAILED)
		exit_simulation(data, "init_semaphore", "sem_open failed");
	data->meal_sem = sem_open(SEM_MEAL, O_CREAT | O_EXCL, 0660,
			data->philo_nbr);
	if (data->meal_sem == SEM_FAILED)
		exit_simulation(data, "init_semaphore", "sem_open failed");
	data->end_simu = sem_open(SEM_END_SIMU, O_CREAT | O_EXCL, 0660, 1);
	if (data->end_simu == SEM_FAILED)
		exit_simulation(data, "init_semaphore", "sem_open failed");
}

static void	add_philo_sem(t_data *data)
{
	char	name[7];

	sem_unlink(sem_name(data->philo_id, 'd', name));
	data->dead_sem = sem_open(sem_name(data->philo_id, 'd', name), O_CREAT
			| O_EXCL, 0660, 1);
	if (data->dead_sem == SEM_FAILED)
	{
		sem_post(data->end_simu);
		exit_simulation(data, "init_semaphore", "sem_open failed");
	}
	sem_unlink(sem_name(data->philo_id, 'e', name));
	data->eat_sem = sem_open(sem_name(data->philo_id, 'e', name), O_CREAT
			| O_EXCL, 0660, 1);
	if (data->eat_sem == SEM_FAILED)
	{
		sem_close(data->dead_sem);
		sem_unlink(sem_name(data->philo_id, 'd', name));
		sem_post(data->end_simu);
		exit_simulation(data, "init_semaphore", "sem_open failed");
	}
}

/*
	ERROR: Need to wait and free correctly when error
*/
static void	init_philo(t_data *data)
{
	int	index;

	index = 0;
	if (gettimeofday(&data->simulation_start_time, NULL) != 0)
		exit_simulation(data, "init_philo", "gettimofday error");
	data->simulation_start_time.tv_sec += 1;
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
			add_philo_sem(data);
			philo_routine(data);
			exit(EXIT_SUCCESS);
		}
		index++;
	}
}

/*
	Create data structure -> create mutex -> launch philo_routine
*/
t_data	*init_simulation(int ac, char **av)
{
	t_data	*data;

	sem_unlink(SEM_FORK);
	sem_unlink(SEM_MEAL);
	sem_unlink(SEM_STDOUT);
	sem_unlink(SEM_SIMULAION_STOP);
	sem_unlink(SEM_END_SIMU);
	data = (t_data *)malloc(sizeof(t_data));
	if (!data)
	{
		print_error("init_simulation", "malloc error");
		exit(EXIT_FAILURE);
	}
	init_data(data, ac, av);
	init_semaphore(data);
	sem_wait(data->end_simu);
	if (data->meal_to_take == 0)
		exit_simulation(data, NULL, NULL);
	if (data->philo_nbr > 200)
		exit_simulation(data, "too much philosopher", "(max_limit = 200)");
	init_philo(data);
	return (data);
}
