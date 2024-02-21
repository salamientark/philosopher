/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 09:58:44 by dbaladro          #+#    #+#             */
/*   Updated: 2024/02/20 17:06:10 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

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
    pthread_mutex_destroy(&(data_p->std_lock));
    free(data_p->fork);
    free(data_p->philosopher);
    exit(EXIT_FAILURE);
}
