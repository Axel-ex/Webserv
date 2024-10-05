/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:13:18 by Axel              #+#    #+#             */
/*   Updated: 2024/10/03 13:21:06 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <map>
#include <string>
#include <vector>
#include "utils.hpp"


class Config
{
    public:
        Config(void);
        ~Config();

        const std::vector<int>& getPorts(void) const;
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

#endif
