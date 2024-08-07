/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 20:08:09 by Axel              #+#    #+#             */
/*   Updated: 2024/07/19 15:54:50 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Log.hpp"
#include "../includes/Server.hpp"
#include "../includes/utils.hpp"
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <exception>

int main(int argc, char** argv)
{
    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGINT, sigHandler);

    std::string config_file;
    argc == 2 ? config_file = argv[1]
              : config_file = "resources/config/default.conf";

    try
    {
        Server server(config_file);

        server.init();
        server.start();
    }
    catch (const std::exception& e)
    {
        std::string err("exception: ");
        err.append(e.what(), std::strlen(e.what()));
        Log::log(ERROR, err);
    }
    return (EXIT_SUCCESS);
}
