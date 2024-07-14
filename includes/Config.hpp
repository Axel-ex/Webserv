/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 23:13:18 by Axel              #+#    #+#             */
/*   Updated: 2024/07/14 09:13:54 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

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

#endif
