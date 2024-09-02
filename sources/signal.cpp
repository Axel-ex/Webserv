/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/26 10:07:02 by Axel              #+#    #+#             */
/*   Updated: 2024/09/02 18:08:47 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <atomic>
#include <csignal>

// std::atomic<bool> stopFlag(false);
bool stopFlag = false;

void sigHandler(int signum)
{
    if (signum == SIGINT)
		stopFlag = true;
}

void sigHandler2(int signum)
{
    if (signum == SIGPIPE)
		return;
}
