/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:13:18 by Axel              #+#    #+#             */
/*   Updated: 2024/05/14 08:49:22 by Axel             ###   ########.fr       */
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
        void parseFile(std::string file);
        static Config& getInstance(void);
        ~Config();

		std::vector<int> getPorts(void) const;
		std::map<std::string, std::string> getResources(void) const;

    private:
        Config(void);

        std::vector<int> _ports;
        std::map<std::string, std::string> _resources;
};

#endif // INCLUDE_INCLUDES_CONFIG_HPP_
