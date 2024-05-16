/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/12 20:08:09 by Axel              #+#    #+#             */
/*   Updated: 2024/05/14 11:41:30 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>

void printUsage(void)
{
	std::cout << "Error: Usage: ./webserv <config_file>";
	exit (EXIT_FAILURE);
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
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}
