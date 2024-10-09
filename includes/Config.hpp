/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:13:18 by Axel              #+#    #+#             */
/*   Updated: 2024/10/09 14:33:52 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include "utils.hpp"
#include <map>
#include <ostream>
#include <string>
#include <vector>

class Config
{
    public:
        Config(void);
        ~Config();

        std::vector<int>& getPorts(void);
        const std::map<std::string, std::string>& getResources(void) const;
        const std::map<int, std::string>& getErrors(void) const;
        const std::map<int, std::string>& getErrorPath(void) const;
        const std::string getServerName(void) const;
        const std::vector<Route> getRoutes(void) const;
        int getMaxBodySize(void) const;

        void setServerName(const std::string& server_name);
        void setPort(int port_nb);
        void setMaxBodySize(int max_body_size);
        void setRoutes(const Route& route);
        void setErrorPath(int error_code, const std::string& path);
        void setErrors(int error_code, const std::string& content);
        void clear(void);

    private:
        std::string _server_name;
        std::vector<int> _ports;
        int _max_body_size;
        std::vector<Route> _routes;

        std::map<std::string, std::string> _resources;
        std::map<int, std::string> _errors;
        std::map<int, std::string> _error_path;
};

std::ostream& operator<<(std::ostream& ofs, Config& config);
std::ostream& operator<<(std::ostream& ofs, const Route& route);

#endif
