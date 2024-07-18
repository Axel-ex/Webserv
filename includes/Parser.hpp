/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achabrer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 10:11:15 by achabrer          #+#    #+#             */
/*   Updated: 2024/07/18 11:14:41 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP_
#define PARSER_HPP_



#include <list>
#include <map>
#include <sstream>
#include <string>

enum TokenType
{
    OPEN_BRACKET,
    CLOSE_BRACKET,
	SEMICOLON,
    SERVER,
    LOCATION,
    DIRECTIVE,
    ARGUMENT,
};

struct Token
{
        TokenType type;
        std::string content;
};

class Parser
{
    public:
        Parser();
        ~Parser();

        void parse(const std::string& config_file);

    private:
        std::map<std::string, TokenType> _token_definition;
        std::list<Token> _token_list;

        std::stringstream _readFile(const std::string& config_file);
        void _tokenize(std::stringstream& file_content);
};


#endif  // PARSER_HPP_
