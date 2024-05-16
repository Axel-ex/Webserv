/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:13:18 by Axel              #+#    #+#             */
/*   Updated: 2024/05/16 13:35:09 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDE_INCLUDES_CONFIG_HPP_
#define INCLUDE_INCLUDES_CONFIG_HPP_

#include <map>
#include <string>
#include <vector>

class Config
{
    public:
        static Config& getInstance(void);
        static void parseFile(std::string file);
        ~Config();

		static std::vector<int> getPorts(void);
		static std::map<std::string, std::string> getResources(void);

    private:
        Config(void);

        std::vector<int> _ports;
        std::map<std::string, std::string> _resources;
};

#endif // INCLUDE_INCLUDES_CONFIG_HPP_
