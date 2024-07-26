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
#include "../includes/Response.hpp"
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

void Config ::parseFile(std::string file)
{
    (void)file;
    /* load errors provided into the conf file*/
    /* if not provided load the rest or the errors supported by our server*/
    getInstance()._errors[NOT_FOUND] =
        (t_error){"Not Found", "<h1>404 Not Found</h1>"};
    getInstance()._errors[500] = (t_error){
        "Internal Server Error", "<h1>500 Internal Server Error</h1>"};
    getInstance()._errors[BAD_REQUEST] =
        (t_error){"Bad Request",
                  "<h1>400 Bad Request</h1><p>The server could not understand "
                  "the request due to invalid syntax.</p>"};

    getInstance()._resources.insert(std::make_pair("/", DUMMY_RESPONSE));

    /* We basically need to do the following with all our resources*/
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
    Config &instance = getInstance();

    instance._ports.clear();
    instance._server_name.clear();
    instance._resources.clear();
    instance._errors.clear();
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

std::map<int, t_error>& Config::getErrors(void)
{
    return (getInstance()._errors);
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
