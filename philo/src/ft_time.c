/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_time.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 13:51:06 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/12 14:05:47 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void    ft_msleep(long int wait_time)
{
    struct timeval  start;
    struct timeval  now;
    long int        m_start;
    long int        m_now;

    if (gettimeofday(&start, NULL) != 0)
    {
        write(2, "ft_msleep() : gettimeofday error\n", 34);
        //Wait for all
        //Exit simulation
    }
    m_start = 1000 * start.tv_sec + start.tv_usec / 1000;
    m_now = m_start;
    while (m_now - m_start < wait_time)
    {
        usleep(500);
        if (gettimeofday(&now, NULL) != 0)
        {
            write(2, "ft_msleep() : gettimeofday error\n", 34);
            //Wait for all
            //Exit simulation
            return ;
        }
        m_now = 1000 * now.tv_sec + now.tv_usec / 1000;
    }
}