/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 20:08:09 by Axel              #+#    #+#             */
/*   Updated: 2024/05/26 11:20:06 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Log.hpp"
#include "../includes/Server.hpp"
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>

/*TODO: The program should be able to run without argument on a default config
 * file*/

void printUsage(void)
{
    std::cout << "Error: Usage: ./webserv <config_file>";
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{

    if (argc < 2)
        printUsage();
    try
    {
        Server server(argv[1]);

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
