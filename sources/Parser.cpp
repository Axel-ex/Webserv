/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 08:46:08 by Axel              #+#    #+#             */
/*   Updated: 2024/07/19 16:21:56 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"
#include "../includes/Config.hpp"
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

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
    // _debugTokenList();
    _parseTokenList();
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

/**
 * @brief split the input into a token list. first check if the word can be
 * found in the token definition. if not, either it is a directive or an
 * argument. A directive as to follow a semicolon or a bracket. Add a semicolon
 * if it is found at the end of the argument which is usefull to know when a
 * directive ends.
 *
 * @param file_content
 */
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
            // remove it and insert a semicolon token.
            if (_token_list.back().content[word.size() - 1] == ';')
            {
                _token_list.back().content.erase(word.size() - 1);
                _token_list.push_back((Token){SEMICOLON, ";", line_nb});
            }
        }
    }
}

void Parser ::_parseTokenList(void)
{
    std::list<Token>::iterator it;

    // Check unclosed bracket and directives without argument
    _matchBrackets();
    _checkInvalidDirective();
    for (it = _token_list.begin(); it != _token_list.end(); it++)
    {
        if (it->type == SERVER)
            _parseServerDirective(it);
        if (it->type == CLOSE_BRACKET)
            continue;
        else
            throw SynthaxException((Token){it->type, it->content, it->line_nb},
                                   "Out of context directive");
    }
}

/**
 * @brief check if there is the same amout of opened and closed brackets.
 */
void Parser ::_matchBrackets(void) const
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
}

/**
 * @brief checks if there is any directive without argument
 */
void Parser ::_checkInvalidDirective(void) const
{
    std::list<Token>::const_iterator it;
    std::list<Token>::const_iterator next_it;

    for (it = _token_list.begin(); it != _token_list.end(); it++)
    {
        next_it = it;
        next_it++;

        if (next_it == _token_list.end())
            break;
        if ((it->type == DIRECTIVE && next_it->type != ARGUMENT) ||
            (it->type == LOCATION && next_it->type != ARGUMENT))
            throw SynthaxException((Token){it->type, it->content, it->line_nb},
                                   "Directive with no arguments");
    }
}

/**
 * @brief parse server block
 *
 * @param it token list iterator
 */
void Parser ::_parseServerDirective(std::list<Token>::iterator& it) const
{
    // While we dont reach the end of the block we parse the tokens. We ignore
    // any token that are not directives or location.
    for (; it->type != CLOSE_BRACKET; it++)
    {
        if (it->type != DIRECTIVE && it->type != LOCATION)
            continue;
        else if (it->type == LOCATION)
            _parseLocationDirective(it);
        else if (it->content == "server_name")
            Config::setServerName((++it)->content);
        else if (it->content == "error_page")
            ;
        else if (it->content == "listen")
        {
            while ((++it)->type != SEMICOLON)
            {
                int port_nb = std::atoi((it)->content.c_str());
                Config::setPort(port_nb);
            }
        }
        else if (it->content == "client_max_body_size")
        {
            it++;
            int body_size = std::atoi(it->content.c_str());
            Config::setMaxBodySize(body_size);
        }
        else
            throw SynthaxException((Token){it->type, it->content, it->line_nb},
                                   "Unrecognised directive");
    }
}

/**
 * @brief parse location block
 *
 * @param it token list iterator
 */
void Parser::_parseLocationDirective(std::list<Token>::iterator& it) const
{
    // Init the Route struct with empty values.
    std::vector<std::string> methods;
    Route route = (Route){it->content, "", methods, "", "", ""};
    for (; it->type != CLOSE_BRACKET; it++)
    {
        if (it->type != DIRECTIVE)
            continue;
        else if (it->content == "root")
            route.root = (++it)->content;
        else if (it->content == "index")
            route.index = (++it)->content;
        else if (it->content == "upload_store")
            route.upload_store = (++it)->content;
        else if (it->content == "cgi_extension")
            route.cgi_extension = (++it)->content;
        else if (it->content == "methods")
        {
            while ((++it)->type == ARGUMENT)
                route.methods.push_back(it->content);
        }
        else
            throw SynthaxException((Token){it->type, it->content, it->line_nb},
                                   "Unrecognised directive");
    }
    Config::setRoutes(route);
}

// DEBUG PURPOSE
/**
 * @brief Print the token list on stdout
 */
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

/**
 * @brief makes correspondance between token type and their string
 * representation
 *
 * @param type
 */
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
