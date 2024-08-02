/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:13:18 by Axel              #+#    #+#             */
/*   Updated: 2024/08/02 13:21:10 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include "../includes/Parser.hpp"
#include <map>
#include <string>
#include <vector>

class Config
{
    public:
        static Config& getInstance(void);
        static void parseFile(std::string file);
        static void clear(void);
        ~Config();

        static std::vector<int>& getPorts(void);
        static std::map<std::string, std::string>& getResources(void);
        static std::map<int, std::string>& getDefaultErrors(void);
		static std::map<int, std::string>& getErrorPath(void);
        static std::string getServerName(void);
        static std::vector<Route> getRoutes(void);
        static int getMaxBodySize(void);

        static void setServerName(const std::string& server_name);
        static void setPort(int port_nb);
        static void setMaxBodySize(int max_body_size);
        static void setRoutes(const Route& route);
		static void setErrorPath(int error_code, const std::string &path);
		static void setDefaultErrors(int error_code, const std::string& content);

    private:
        Config(void);

        std::string _server_name;
        std::vector<int> _ports;
        int _max_body_size;
        std::vector<Route> _routes;

        std::map<std::string, std::string> _resources;
        std::map<int, std::string> _default_errors;
        std::map<int, std::string> _error_path;
};

#endif
