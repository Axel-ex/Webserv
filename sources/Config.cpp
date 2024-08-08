/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmoutinh <tmoutinh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:17:43 by Axel              #+#    #+#             */
/*   Updated: 2024/07/25 20:35:32 by tmoutinh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

Config ::Config(void) {}

Config& Config::getInstance(void)
{
    static Config instance;

    return (instance);
}

Config ::~Config(void) {}

//Dummy implementation, should be removed as soon as possible
void Config ::parseFile(std::string file)
{
    (void)file;

    std::ifstream ifs("resources/form.html");
    if (!ifs)
        throw std::runtime_error("Couldn't open the file");
    std::stringstream buff;
    buff << ifs.rdbuf();
    getInstance()._resources.insert(std::make_pair("/form", buff.str()));

	ifs.close();
	ifs.clear();
	ifs.open("resources/okresponse.html");
	if (!ifs)
		throw std::runtime_error("Couldn't open the file");
	buff.clear();
	buff.str("");
	buff << ifs.rdbuf();
	getInstance()._resources.insert(std::make_pair("posted", buff.str()));
    ifs.close();
	ifs.clear();
	ifs.open("resources/deleteresponse.html");
	if (!ifs)
		throw std::runtime_error("Couldn't open the file");
    buff.clear();
	buff.str("");
	buff << ifs.rdbuf();
    getInstance()._resources.insert(std::make_pair("deleted", buff.str()));
}

void Config ::clear(void)
{
    Config& instance = getInstance();

    instance._ports.clear();
    instance._server_name.clear();
    instance._resources.clear();
    instance._default_errors.clear();
    instance._routes.clear();
    instance._max_body_size = 100;
}

// _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/ GETTERS \_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
std::string Config::getServerName(void) { return (getInstance()._server_name); }

std::vector<int>& Config::getPorts(void) { return (getInstance()._ports); }

std::map<std::string, std::string>& Config::getResources(void)
{
    return (getInstance()._resources);
}

std::map<int, std::string>& Config::getDefaultErrors(void)
{
    return (getInstance()._default_errors);
}

std::map<int, std::string>& Config::getErrorPath(void)
{
    return (getInstance()._error_path);
}

std::vector<Route> Config::getRoutes(void) { return (getInstance()._routes); }

int Config ::getMaxBodySize(void) { return (getInstance()._max_body_size); }

// _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/ SETTERS \_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
void Config::setServerName(const std::string& server_name)
{
    getInstance()._server_name = server_name;
}

void Config::setPort(int port_nb) { getInstance()._ports.push_back(port_nb); }

void Config ::setMaxBodySize(int max_body_size)
{
    getInstance()._max_body_size = max_body_size;
}

void Config ::setRoutes(const Route& route)
{
    getInstance()._routes.push_back(route);
}

void Config::setDefaultErrors(int error_code, const std::string& content)
{
    getInstance()._default_errors.insert(std::make_pair(error_code, content));
}

void Config::setErrorPath(int error_code, const std::string& path)
{
    getInstance()._error_path.insert(std::make_pair(error_code, path));
}
