/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 20:08:09 by Axel              #+#    #+#             */
/*   Updated: 2024/07/19 09:21:01 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Log.hpp"
#include "../includes/Server.hpp"
#include "../includes/utils.hpp"
#include "../includes/Parser.hpp"
#include <cstdlib>
#include <cstring>
#include <exception>
#include <csignal>

int main(int argc, char** argv)
{
	std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGINT, sigHandler);

	std::string config_file;
	argc == 2 ? config_file = argv[1] : config_file = "resources/config/config1.conf";

    try
    {
		Parser parser;
        Server server(argv[1]);
		
		parser.parse(config_file);
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
