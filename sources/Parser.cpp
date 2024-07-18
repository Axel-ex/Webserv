/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 08:46:08 by Axel              #+#    #+#             */
/*   Updated: 2024/07/18 11:42:29 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"
#include "../includes/Log.hpp"
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

Parser ::Parser()
{
    _token_definition.insert(std::make_pair("{", OPEN_BRACKET));
    _token_definition.insert(std::make_pair("}", CLOSE_BRACKET));
    _token_definition.insert(std::make_pair("server", SERVER));
    _token_definition.insert(std::make_pair("location", LOCATION));
    _token_definition.insert(std::make_pair(";", SEMICOLON));
}

Parser ::~Parser() {}

void Parser ::parse(const std::string& config_file)
{
    std::stringstream file_content;

    file_content = _readFile(config_file);
    _tokenize(file_content);
    Log::debugTokenList(_token_list);

    // check the synthax
    // loop over the token
}

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

void Parser ::_tokenize(std::stringstream& file_content)
{
    std::string line;

    while (std::getline(file_content, line))
    {
        std::stringstream line_stream(line);
        std::string word;

        while (line_stream >> word)
        {
            Token new_token;

            new_token.content = word;
            // look if word can be find in token _token_definition
            std::map<std::string, TokenType>::iterator it =
                _token_definition.find(word);
            if (it != _token_definition.end())
                new_token.type = it->second;
            // If not find, either it is a directive or an argument
            // arguments need to follow directive, location or other arguments.
            else
            {
                if (_token_list.back().type == DIRECTIVE ||
                    _token_list.back().type == LOCATION ||
                    _token_list.back().type == ARGUMENT)
                    new_token.type = ARGUMENT;
                else
                    new_token.type = DIRECTIVE;
            }
            _token_list.push_back(new_token);

			//If semicolon is found at the end of the token we just inserted, remove it and insert a semicolon token
			if (_token_list.back().content[word.size() - 1] == ';'){
				_token_list.back().content.erase(word.size() - 1);
				_token_list.push_back((Token){SEMICOLON, ";"});
			}
        }
    }
}
