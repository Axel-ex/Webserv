/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 20:08:09 by Axel              #+#    #+#             */
/*   Updated: 2024/10/03 13:14:14 by Axel             ###   ########.fr       */
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

int main(int argc, char **argv)
{
    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGINT, sigHandler);

	//TODO: make it work with the cluster
    std::signal(SIGCHLD, Server::_sigchldHandler);

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
