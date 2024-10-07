/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 20:08:09 by Axel              #+#    #+#             */
/*   Updated: 2024/10/07 13:42:36 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Log.hpp"
#include "../includes/utils.hpp"
#include "../includes/Cluster.hpp"
#include "../includes/Server.hpp"
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <exception>

std::deque<t_chldProcess> finished_pids;

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

int main(int argc, char **argv)
{
    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGINT, sigHandler);
    std::signal(SIGCHLD, sigchldHandler);

	//TODO: make it work with the cluster

    std::string config_file;
    argc == 2 ? config_file = argv[1]
              : config_file = "resources/config/default.conf";

    try
    {
		Cluster cluster(config_file);

        cluster.init();
        cluster.start();
    }
    catch (const std::exception &e)
    {
        std::string err("exception: ");
        err.append(e.what(), std::strlen(e.what()));
        Log::log(ERROR, err);
    }
    return (EXIT_SUCCESS);
}
