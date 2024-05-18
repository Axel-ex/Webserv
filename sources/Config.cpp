/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:17:43 by Axel              #+#    #+#             */
/*   Updated: 2024/05/18 10:24:16 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Config.hpp"
#include "../includes/Log.hpp"
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

    /* TODO: PARSE LOGIC */
    getInstance()._ports.push_back(8080);
    getInstance()._ports.push_back(9000);

    /* load errors provided into the conf file*/
    /* if not provided load the rest or the errors supported by our server*/
    getInstance()._errors[404] =
        (t_error){"Not Found", "<h1>404 Not Found</h1>"};
    getInstance()._errors[500] = (t_error){
        "Internal Server Error", "<h1>500 Internal Server Error</h1>"};
    getInstance()._errors[400] =
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
}

std::vector<int>& Config::getPorts(void) { return (getInstance()._ports); }

std::map<std::string, std::string>& Config::getResources(void)
{
    return (getInstance()._resources);
}

std::map<int, t_error>& Config::getErrors(void)
{
    return (getInstance()._errors);
}
