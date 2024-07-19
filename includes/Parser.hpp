/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achabrer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 10:11:15 by achabrer          #+#    #+#             */
/*   Updated: 2024/07/19 15:09:48 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <exception>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "../includes/Request.hpp"

enum TokenType
{
    OPEN_BRACKET,
    CLOSE_BRACKET,
    SEMICOLON,
    SERVER,
    LOCATION,
    DIRECTIVE,
    ARGUMENT,
    UNKNOWN,
};

struct Token
{
        TokenType type;
        std::string content;
        int line_nb;
};

struct Route
{
	std::string url;
	std::string root;
	std::vector<std::string> methods;
	std::string upload_store;
	std::string index;
	std::string cgi_extension;
};

class Parser
{
    public:
        Parser();
        ~Parser();

        void parse(const std::string& config_file);

        class SynthaxException : public std::exception
        {

            public:
                SynthaxException(const Token& token, const std::string& reason)
                    : _token(token), _reason(reason)
                {
                }
                virtual ~SynthaxException() throw(){};

                virtual const char* what() const throw()
                {
                    std::stringstream msg;
                    msg << _reason << " line " << _token.line_nb << ": "
                        << std::endl;
                    msg << "content: " << _token.content
                        << ", type: " << _tokenTypeToString(_token.type)
                        << std::endl;

                    _msg = msg.str();
                    return (_msg.c_str());
                }

            private:
                Token _token;
                std::string _reason;
                mutable std::string _msg;
        };

    private:
        std::map<std::string, TokenType> _token_definition;
        std::list<Token> _token_list;

        std::stringstream _readFile(const std::string& config_file);
        void _tokenize(std::stringstream& file_content);
        void _parseTokenList(void);
		void _matchBrackets(void) const;
		void _checkInvalidDirective(void) const;
		void _parseServerDirective(std::list<Token>::iterator &it) const;
		void _parseLocationDirective(std::list<Token>::iterator &it) const;

        // DEBUG
        void _debugTokenList(void) const;
        static std::string _tokenTypeToString(TokenType type);
};

#endif // PARSER_HPP_
