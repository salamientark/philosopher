/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 14:24:57 by dbaladro          #+#    #+#             */
/*   Updated: 2024/02/21 12:28:02 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void    log_philo(t_philo *philo, char *msg)
{
    struct timeval  right_now;
    int             timestamp;

    if(gettimeofday(&right_now, NULL) == 0)
        timestamp = 1000 * (right_now.tv_sec - philo->last_meal.tv_sec)
            + (right_now.tv_usec - philo->last_meal.tv_usec) / 1000;
    else
        timestamp = -1;
    pthread_mutex_lock(philo->std_lock);
    printf("%d ms %d %s\n", timestamp, philo->pos, msg);
    pthread_mutex_unlock(philo->std_lock);
}