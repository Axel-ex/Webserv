/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 10:11:15 by achabrer          #+#    #+#             */
/*   Updated: 2024/10/10 10:57:06 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "../includes/Cluster.hpp"
#include <exception>
#include <list>
#include <map>
#include <string>
#include <vector>
#include "../includes/utils.hpp"

class Config;

class Parser
{
    public:
        Parser();
        ~Parser();
        void parse(const std::string& config_file, Cluster& cluster);

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
        void _setTokenType(Token& token);
        void _parseTokenList(Cluster& cluster);
        void _parseServerDirective(std::list<Token>::iterator& it,
                                   Cluster& cluster);
        void _parseLocationDirective(std::list<Token>::iterator& it,
                                     Config& config) const;
        void _loadErrors(Cluster& cluster) const;
		void _checkDuplicatedPorts(Cluster &cluster) const;

        // Helper
        void _matchBrackets(void) const;
        void _checkInvalidDirective(void) const;
        bool _isHttpMethod(const std::string& method) const;
        bool _isCgiPath(const std::string& path) const;
        bool _isCgiExtension(const std::string& path) const;
        bool _isValidPort(int port, const std::vector<int>& ports) const;
        bool _isDuplicatedLocation(const std::string& url,
                                   const std::vector<Route>& routes) const;

        // Debug
        void _debugTokenList(void) const;
		void _debugConfigs(Cluster &cluster) const;
        static std::string _tokenTypeToString(TokenType type);
};

#endif // PARSER_HPP_
