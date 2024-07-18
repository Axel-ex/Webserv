/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achabrer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 10:11:15 by achabrer          #+#    #+#             */
/*   Updated: 2024/07/18 09:06:48 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <list>
#include <sstream>
#include <string>
#include <strstream>

enum token_type
{
    CURLY_BRACKET,
    SERVER,
    LOCATION,
    DIRECTIVE,
    ARGUMENT,
};

struct token
{
        token_type type;
        std::string content;
};

class Parser
{
    public:
        Parser(const std::string& config_file);
        ~Parser();

    private:
        std::list<token> token_list;

        std::stringstream _readFile(const std::string& config_file);
        void _tokenize(std::stringstream& ifs);
};
