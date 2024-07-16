/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achabrer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 10:11:15 by achabrer          #+#    #+#             */
/*   Updated: 2024/07/16 12:47:02 by achabrer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <list>
#include <string>

enum token_type
{
    SEMICOLON,
    CURLY_BRACKET,
    DIRECTIVE,
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
};
