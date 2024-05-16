/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:17:43 by Axel              #+#    #+#             */
/*   Updated: 2024/05/16 13:35:45 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Config.hpp"
#include <vector>

Config ::Config(void) {}

Config& Config::getInstance(void)
{
    static Config instance;

    return (instance);
}

Config ::~Config(void) {}

void Config ::parseFile(std::string file)
{
	(void)file;

    /* TODO: PARSE LOGIC */
    getInstance()._ports.push_back(8080);
    getInstance()._ports.push_back(9000);
}

std::vector<int> Config::getPorts(void) { return (getInstance()._ports); }

std::map<std::string, std::string> Config::getResources(void) { return (getInstance()._resources); }
