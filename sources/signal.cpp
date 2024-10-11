/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/26 10:07:02 by Axel              #+#    #+#             */
/*   Updated: 2024/10/10 15:08:34 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <atomic>
#include <csignal>
#include <deque>
#include "../includes/utils.hpp"

bool stopFlag = false;
std::deque<t_chldProcess> finished_pids;

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

void sigchldHandler(int signum)
{
    (void)signum;
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        t_chldProcess child;
        child.pid = pid;
        if (WIFEXITED(status))
            child.type = EXITED;
        else if (WIFSIGNALED(status))
            child.type = SIGNALED;
        child.status = status;
        finished_pids.push_back(child);
    }
}
