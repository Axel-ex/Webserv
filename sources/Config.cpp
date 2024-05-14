/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:17:43 by Axel              #+#    #+#             */
/*   Updated: 2024/05/14 08:57:40 by Axel             ###   ########.fr       */
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
    /* TODO: PARSE LOGIC */
    _ports.push_back(8080);
    _ports.push_back(9000);
}

std::vector<int> Config::getPorts(void) const { return (_ports); }

std::map<std::string, std::string> Config::getResources(void) const { return (_resources); }
