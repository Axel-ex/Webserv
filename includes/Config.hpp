/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:13:18 by Axel              #+#    #+#             */
/*   Updated: 2024/08/02 11:55:12 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <map>
#include <string>
#include <vector>
#include "../includes/Parser.hpp"

class Config
{
    public:
        static Config& getInstance(void);
        static void parseFile(std::string file);
		static void clear(void);
        ~Config();

        static std::vector<int> &getPorts(void);
        static std::map<std::string, std::string> &getResources(void);
        static std::map<int, std::string> &getErrors(void);
		static std::string getServerName(void);
		static std::vector<Route> getRoutes(void);
		static int getMaxBodySize(void);

		static void setServerName(const std::string &server_name);
		static void setPort(int port_nb);
		static void setMaxBodySize(int max_body_size);
		static void setRoutes(const Route &route);

    private:
        Config(void);
		
		std::string _server_name;
        std::vector<int> _ports;
		int _max_body_size;
		std::vector<Route> _routes;

        std::map<std::string, std::string> _resources;
        std::map<int, std::string> _errors;
};

#endif
