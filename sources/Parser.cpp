/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 08:46:08 by Axel              #+#    #+#             */
/*   Updated: 2024/07/19 10:38:05 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <stack>
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
    _debugTokenList();
    _checkSynthax();

    // check the synthax
    // loop over the token and store into config
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
    int line_nb = 0;

    while (std::getline(file_content, line))
    {
        std::stringstream line_stream(line);
        std::string word;
        line_nb++;

        if (line.empty())
            continue;
        while (line_stream >> word)
        {
            Token new_token;

            new_token.content = word;
            new_token.line_nb = line_nb;
            // look if word can be find in token _token_definition
            std::map<std::string, TokenType>::iterator it =
                _token_definition.find(word);
            if (it != _token_definition.end())
                new_token.type = it->second;
            // If not find, either it is a directive or an argument
            // directives always follow semicolon or brackets.
            else
            {
                if (_token_list.back().type == SEMICOLON ||
                    _token_list.back().type == OPEN_BRACKET ||
                    _token_list.back().type == CLOSE_BRACKET)
                    new_token.type = DIRECTIVE;
                else
                    new_token.type = ARGUMENT;
            }
            _token_list.push_back(new_token);

            // If semicolon is found at the end of the token we just inserted,
            // remove it and insert a semicolon token. The semicolon is usefull
            // to now when a directive ends.
            if (_token_list.back().content[word.size() - 1] == ';')
            {
                _token_list.back().content.erase(word.size() - 1);
                _token_list.push_back((Token){SEMICOLON, ";", line_nb});
            }
        }
    }
}

void Parser ::_checkSynthax(void) const
{
    std::stack<Token> brackets;
    std::list<Token>::const_iterator it;

    for (it = _token_list.begin(); it != _token_list.end(); it++)
    {
        if (it->type == OPEN_BRACKET)
            brackets.push((Token){OPEN_BRACKET, "{", it->line_nb});
        if (it->type == CLOSE_BRACKET)
        {
            if (brackets.empty())
                throw SynthaxException((Token){CLOSE_BRACKET, "}", it->line_nb},
                                       "Unmatched bracket");
            brackets.pop();
        }
    }
    if (!brackets.empty())
        throw SynthaxException(brackets.top(), "Unmatched bracket");

    // NO DIRECTIVE OUTSIDE SERVER OR LOCATION BLOCK
    // UNMATCH BRACKETS
}

// DEBUG PURPOSE
void Parser ::_debugTokenList(void) const
{
    std::list<Token>::const_iterator it;

    for (it = _token_list.begin(); it != _token_list.end(); it++)
    {
        std::cout << "content: " << it->content << std::endl;
        std::cout << "type: " << _tokenTypeToString(it->type) << std::endl;
        std::cout << std::endl;
    }
}

std::string Parser ::_tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case OPEN_BRACKET:
        return "OPEN_BRACKET";
    case CLOSE_BRACKET:
        return "CLOSE_BRACKET";
    case SERVER:
        return "SERVER";
    case LOCATION:
        return "LOCATION";
    case DIRECTIVE:
        return "DIRECTIVE";
    case ARGUMENT:
        return "ARGUMENT";
    case SEMICOLON:
        return "SEMICOLON";
    default:
        return "UNKNOWN";
    }
}
