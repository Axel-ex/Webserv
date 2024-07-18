/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 08:46:08 by Axel              #+#    #+#             */
/*   Updated: 2024/07/18 09:15:54 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <strstream>

Parser ::Parser(const std::string& config_file)
{
    std::stringstream file_content;

    file_content = _readFile(config_file);
    _tokenize(file_content);

    // check the synthax
    // loop over the token
}

Parser ::~Parser() {}

std::stringstream Parser ::_readFile(const std::string& config_file)
{
    std::ifstream ifs;
    std::string line;
    std::stringstream file_content;

    ifs.open(config_file);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Couldn't open the config file: file doesn't exist");

    while (ifs.good())
    {
		std::getline(ifs, line);
		file_content << line << '\n';
    }
	ifs.close();

	return (file_content);
}

void Parser ::_tokenize(std::stringstream& ifs) {}
