/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmoutinh <tmoutinh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:17:43 by Axel              #+#    #+#             */
/*   Updated: 2024/10/03 12:28:31 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Config.hpp"
#include <utility>

Config ::Config(void) {}

Config ::~Config(void) {}

// _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/ GETTERS \_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
const std::string Config::getServerName(void) const { return (_server_name); }

const std::vector<int>& Config::getPorts(void) const { return (_ports); }

const std::map<std::string, std::string>& Config::getResources(void) const
{
    return (_resources);
}

const std::map<int, std::string>& Config::getErrors(void) const
{
    return (_errors);
}

const std::map<int, std::string>& Config::getErrorPath(void) const
{
    return (_error_path);
}

const std::vector<Route> Config::getRoutes(void) const { return (_routes); }

int Config ::getMaxBodySize(void) const { return (_max_body_size); }

// _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/ SETTERS \_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
void Config::setServerName(const std::string& server_name)
{
    _server_name = server_name;
}

void Config::setPort(int port_nb) { _ports.push_back(port_nb); }

void Config ::setMaxBodySize(int max_body_size)
{
    _max_body_size = max_body_size;
}

void Config ::setRoutes(const Route& route) { _routes.push_back(route); }

void Config::setErrors(int error_code, const std::string& content)
{
    _errors.insert(std::make_pair(error_code, content));
}

void Config::setErrorPath(int error_code, const std::string& path)
{
    _error_path.insert(std::make_pair(error_code, path));
}

// _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/ HELPER \_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
void Config ::clear(void)
{
    _ports.clear();
    _server_name.clear();
    _resources.clear();
    _errors.clear();
    _routes.clear();
    _errors.clear();
    _max_body_size = 100;
}
