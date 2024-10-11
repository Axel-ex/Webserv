/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 20:08:09 by Axel              #+#    #+#             */
/*   Updated: 2024/10/10 15:09:44 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Log.hpp"
#include "../includes/utils.hpp"
#include "../includes/Cluster.hpp"
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <exception>

int main(int argc, char **argv)
{
    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGINT, sigHandler);
    std::signal(SIGCHLD, sigchldHandler);

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
        Log::log(ERROR, e.what());
    }
    return (EXIT_SUCCESS);
}
