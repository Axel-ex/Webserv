/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/26 10:07:02 by Axel              #+#    #+#             */
/*   Updated: 2024/05/26 10:38:09 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <atomic>
#include <csignal>

std::atomic<bool> stopFlag(false);

void sigHandler(int signum)
{
    if (signum == SIGINT)
		stopFlag = true;
}
