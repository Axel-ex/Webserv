/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:13:18 by Axel              #+#    #+#             */
/*   Updated: 2024/05/17 11:31:55 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDE_INCLUDES_CONFIG_HPP_
#define INCLUDE_INCLUDES_CONFIG_HPP_

#include <map>
#include <string>
#include <vector>

typedef struct s_error
{
	std::string reason;
	std::string body;

} t_error;

class Config
{
    public:
        static Config& getInstance(void);
        static void parseFile(std::string file);
        ~Config();

        static std::vector<int> &getPorts(void);
        static std::map<std::string, std::string> &getResources(void);
        static std::map<int, t_error> &getErrors(void);

    private:
        Config(void);

        std::vector<int> _ports;
        std::map<std::string, std::string> _resources;
        std::map<int, t_error> _errors;
};

#endif // INCLUDE_INCLUDES_CONFIG_HPP_
