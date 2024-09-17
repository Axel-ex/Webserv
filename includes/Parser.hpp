/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 10:11:15 by achabrer          #+#    #+#             */
/*   Updated: 2024/09/05 11:53:26 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <exception>
#include <list>
#include <map>
#include <string>
#include <vector>

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
	std::vector<std::string> cgi_path;
	std::vector<std::string> cgi_extension;
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
                SynthaxException(const Token& token, const std::string& reason);
                virtual ~SynthaxException() throw(){};
                virtual const char* what() const throw();

            private:
                Token _token;
                std::string _reason;
                mutable std::string _msg;
        };

    private:
        std::map<std::string, TokenType> _token_definition;
        std::list<Token> _token_list;

        static std::string _readFile(const std::string& path);
        void _tokenize(std::string& file_content);
		void _setTokenType(Token &token);
        void _parseTokenList(void);
		void _matchBrackets(void) const;
		void _checkInvalidDirective(void) const;
		void _parseServerDirective(std::list<Token>::iterator &it);
		void _parseLocationDirective(std::list<Token>::iterator &it) const;
		void _loadErrors(void) const;
		// Helper
		bool _isHttpMethod(const std::string &method) const;
        bool _isCgiPath(const std::string& path) const;
        bool _isCgiExtension(const std::string& path) const;
		bool _isValidPort(int port, const std::vector<int> &ports) const;
		bool _isDuplicatedLocation(const std::string &url) const;
        // Debug
        void _debugTokenList(void) const;
        static std::string _tokenTypeToString(TokenType type);
};

#endif // PARSER_HPP_
