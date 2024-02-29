/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 09:58:44 by dbaladro          #+#    #+#             */
/*   Updated: 2024/02/29 00:37:59 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void    print_error(char *msg, pthread_mutex_t *std_err_lock)
{
    int len;

    len = 0;
    while (msg[len])
        len++;
    pthread_mutex_lock(std_err_lock);
    write(2, msg, len);
    pthread_mutex_unlock(std_err_lock);
}

void    exit_error(char *prog, char *msg)
{
    printf("%s: %s\n", prog, msg);
    exit(EXIT_FAILURE);
}

void    exit_simulation(t_data *data_p)
{
    int index;

    index = 0;
    while (index < data_p->philo_nbr)
    {
        pthread_mutex_destroy(&(data_p->fork[index]));
        index++;
    }
    pthread_mutex_destroy(&(data_p->stdout_lock));
    pthread_mutex_destroy(&(data_p->dead_lock));
    free(data_p->fork);
    free(data_p->philosopher);
    free(data_p);
}
