/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmoutinh <tmoutinh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:17:43 by Axel              #+#    #+#             */
/*   Updated: 2024/10/09 14:33:42 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Config.hpp"
#include <climits>
#include <ostream>
#include <utility>

Config ::Config(void) : _max_body_size(INT_MAX) {}

Config ::~Config(void) {}

// _/=\_/=\_/=\_/=\_/=\_/=\_/=\_/ GETTERS \_/=\_/=\_/=\_/=\_/=\_/=\_/=\_
const std::string Config::getServerName(void) const { return (_server_name); }

std::vector<int>& Config::getPorts(void) { return (_ports); }

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


std::ostream &operator<<(std::ostream &ofs,  Config &config)
{
    ofs << "Server Name: " << config.getServerName() << "\n";

    ofs << "Ports: ";
    std::vector<int>& ports = config.getPorts();
    for (size_t i = 0; i < ports.size(); ++i) {
        ofs << ports[i];
        if (i != ports.size() - 1) {
            ofs << ", ";
        }
    }
    ofs << "\n";

    ofs << "Max Body Size: " << config.getMaxBodySize() << "\n";

    ofs << "Routes:\n";
    const std::vector<Route>& routes = config.getRoutes();
    for (size_t i = 0; i < routes.size(); ++i) {
        ofs << "  Route " << i + 1 << ": " << routes[i] << "\n";
    }

    ofs << "Error Paths:\n";
    const std::map<int, std::string>& errorPaths = config.getErrorPath();
    for (std::map<int, std::string>::const_iterator it = errorPaths.begin(); it != errorPaths.end(); ++it) {
        ofs << "  Error Code: " << it->first << " - Path: " << it->second << "\n";
    }

    ofs << "Resources:\n";
    const std::map<std::string, std::string>& resources = config.getResources();
    for (std::map<std::string, std::string>::const_iterator it = resources.begin(); it != resources.end(); ++it) {
        ofs << "  " << it->first << ": " << it->second << "\n";
    }

    return ofs; 
}

std::ostream &operator<<(std::ostream &ofs, const Route &route)
{
    ofs << "URL: " << route.url << "\n";

    ofs << "Root: " << route.root << "\n";

    ofs << "Allowed Methods: ";
    for (size_t i = 0; i < route.methods.size(); ++i) {
        ofs << route.methods[i];
        if (i != route.methods.size() - 1) {
            ofs << ", ";
        }
    }
    ofs << "\n";

    ofs << "Upload Store: " << route.upload_store << "\n";

    ofs << "Index: " << route.index << "\n";

    ofs << "CGI Paths: ";
    for (size_t i = 0; i < route.cgi_path.size(); ++i) {
        ofs << route.cgi_path[i];
        if (i != route.cgi_path.size() - 1) {
            ofs << ", ";
        }
    }
    ofs << "\n";

    ofs << "CGI Extensions: ";
    for (size_t i = 0; i < route.cgi_extension.size(); ++i) {
        ofs << route.cgi_extension[i];
        if (i != route.cgi_extension.size() - 1) {
            ofs << ", ";
        }
    }
    ofs << "\n";

    ofs << "Autoindex: " << (route.autoindex ? "Enabled" : "Disabled") << "\n";

    return ofs;
}
