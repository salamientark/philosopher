/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_error.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 09:58:44 by dbaladro          #+#    #+#             */
/*   Updated: 2024/03/14 00:05:11 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

/*
	OBVIOUS
*/
void	exit_error(char *prog, char *msg)
{
	printf("%s: %s\n", prog, msg);
	exit(EXIT_FAILURE);
}
