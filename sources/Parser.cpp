/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 08:46:08 by Axel              #+#    #+#             */
/*   Updated: 2024/10/11 15:54:36 by axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"
#include "../includes/Config.hpp"
#include "../includes/Log.hpp"
#include "../includes/Server.hpp"
#include "../includes/utils.hpp"
#include <cstddef>
#include <cstdlib>
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
#include <algorithm>

Parser ::Parser()
{
    _token_definition.insert(std::make_pair("{", OPEN_BRACKET));
    _token_definition.insert(std::make_pair("}", CLOSE_BRACKET));
    _token_definition.insert(std::make_pair("server", SERVER));
    _token_definition.insert(std::make_pair("location", LOCATION));
    _token_definition.insert(std::make_pair(";", SEMICOLON));
}

Parser ::~Parser() {}

void Parser ::parse(const std::string& config_file, Cluster& cluster)
{
    std::string file_content;

    file_content = _readFile(config_file);
    _tokenize(file_content);
    _parseTokenList(cluster);
    _loadErrors(cluster);
    _findVirtualServers(cluster);
}

std::string Parser ::_readFile(const std::string& path)
{
    std::ifstream ifs;
    std::string line;
    std::stringstream file_content;

    ifs.open(path.c_str());
    if (!ifs.is_open())
        throw std::runtime_error("Couldn't open the file " + path +
                                 ": file doesn't exist");
    while (ifs.good())
    {
        std::getline(ifs, line);
        file_content << line << '\n';
    }
    ifs.close();
    return (file_content.str());
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
void Parser ::_tokenize(std::string& file_content)
{
    std::string line;
    std::stringstream file_stream(file_content);
    int line_nb = 0;

    while (std::getline(file_stream, line))
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
            _setTokenType(new_token);
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

/**
 * @brief Check if the token content matches any of the known content from
 * _token_defintion. If not it is either a directive or an argument. a directive
 * always follows a semicolon or a bracket.
 *
 * @param token
 */
void Parser ::_setTokenType(Token& token)
{
    std::map<std::string, TokenType>::iterator it =
        _token_definition.find(token.content);
    if (it != _token_definition.end())
        token.type = it->second;
    else
    {
        if (_token_list.back().type == SEMICOLON ||
            _token_list.back().type == OPEN_BRACKET ||
            _token_list.back().type == CLOSE_BRACKET)
            token.type = DIRECTIVE;
        else
            token.type = ARGUMENT;
    }
}

void Parser ::_parseTokenList(Cluster& cluster)
{
    std::list<Token>::iterator it;

    // Check unclosed bracket and directives without argument
    _matchBrackets();
    _checkInvalidDirective();
    for (it = _token_list.begin(); it != _token_list.end(); it++)
    {
        if (it->type == SERVER)
            _parseServerDirective(it, cluster);
        else
            throw SynthaxException(*it, "Out of context directive");
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
            brackets.push(*it);
        if (it->type == CLOSE_BRACKET)
        {
            if (brackets.empty())
                throw SynthaxException(*it, "Unmatched bracket");
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
            throw SynthaxException(*it, "Directive with no arguments");
    }
}

/**
 * @brief parse server block and create Server instance from the parsed
 * information
 *
 * @param it token list iterator
 */
void Parser ::_parseServerDirective(std::list<Token>::iterator& it,
                                    Cluster& cluster)
{
    Config config;

    // While we dont reach the end of the block we parse the tokens. We
    // ignore any token that are not directives or location.
    for (; it->type != CLOSE_BRACKET; it++)
    {
        if (it->type != DIRECTIVE && it->type != LOCATION)
            continue;
        else if (it->type == LOCATION)
            _parseLocationDirective(++it, config);
        else if (it->content == "server_name")
            config.setServerName((++it)->content);
        else if (it->content == "error_page")
        {
            int error_code = std::atoi((++it)->content.c_str());
            std::string path_to_html = (++it)->content;
            config.setErrorPath(error_code, path_to_html);
        }
        else if (it->content == "listen")
        {
            while ((++it)->type != SEMICOLON)
            {
                int port_nb = std::atoi((it)->content.c_str());
                if (!_isValidPort(port_nb, config.getPorts()))
                    throw SynthaxException(*it, "Invalid port number");
                config.setPort(port_nb);
            }
        }
        else if (it->content == "client_max_body_size")
        {
            it++;
            int body_size = std::atoi(it->content.c_str());
            config.setMaxBodySize(body_size);
        }
        else
            throw SynthaxException(*it, "Unrecognised directive");
    }
    if (config.getServerName().empty())
        throw std::runtime_error("missing Server name");
    if (config.getPorts().empty())
        throw std::runtime_error("missing ports for server " +
                                 config.getServerName());

    cluster.getServers().push_back(Server(config));
}

/**
 * @brief parse location block and create route for the config passed as
 * parameter
 *
 * @param it token list iterator
 */
void Parser::_parseLocationDirective(std::list<Token>::iterator& it,
                                     Config& config) const
{
    // Init the Route struct with empty values.
    std::vector<std::string> methods;
    std::vector<std::string> cgi_path;
    std::vector<std::string> cgi_extension;

    if (_isDuplicatedLocation(it->content, config.getRoutes()))
        throw SynthaxException(*it,
                               "Config contains duplicated location blocks");
    Route route =
        (Route){it->content, "",       methods,       "./uploads",
                "",          cgi_path, cgi_extension, std::make_pair(0, ""),
                false};

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
        else if (it->content == "autoindex")
            route.autoindex = (++it)->content == "on" ? true : false;
        else if (it->content == "redirect")
        {
            int code = std::atoi((++it)->content.c_str());
            if (code != 301 && code != 302 && code != 307 && code != 308)
                throw SynthaxException(*it, "Invalid redirect code");
            route.redirect_url = std::make_pair(code, (++it)->content);
        }
        else if (it->content == "cgi_path")
        {
            while ((++it)->type == ARGUMENT)
            {
                if (!_isCgiPath(it->content))
                    throw SynthaxException(*it, "Unrecognised CGI path");
                route.cgi_path.push_back(it->content);
            }
        }
        else if (it->content == "cgi_extension")
        {
            while ((++it)->type == ARGUMENT)
            {
                if (!_isCgiExtension(it->content))
                    throw SynthaxException(*it,
                                           "Syntax error on CGI extension");
                route.cgi_extension.push_back(it->content);
            }
        }
        else if (it->content == "methods")
        {
            while ((++it)->type == ARGUMENT)
            {
                if (!_isHttpMethod(it->content))
                    throw SynthaxException(*it, "Unrecognised method");
                route.methods.push_back(it->content);
            }
        }
        else
            throw SynthaxException(*it, "Unrecognised directive");
    }
    config.setRoutes(route);
}

/**
 * @brief load default error pages into the configs. Error page are first loaded
 * from the paths specified in the config file, and the missing error are loaded
 * from a default location.
 */
void Parser::_loadErrors(Cluster& cluster) const
{
    std::vector<Server>& servers = cluster.getServers();

    for (size_t i = 0; i < servers.size(); i++)
    {
        std::map<int, std::string> error_path =
            servers[i].getConfig().getErrorPath();
        std::map<int, std::string>::iterator it_error_path;

        for (it_error_path = error_path.begin();
             it_error_path != error_path.end(); it_error_path++)
        {
            std::string content = _readFile(it_error_path->second);
            servers[i].getConfig().setErrors(it_error_path->first, content);
        }

        // Then load the default if not provided
        int tmp[] = {400, 403, 404, 405, 413, 500, 504};
        std::map<int, std::string> errors = servers[i].getConfig().getErrors();
        std::vector<int> error_codes(tmp, tmp + sizeof(tmp) / sizeof(int));
        for (size_t j = 0; j < error_codes.size(); j++)
        {
            if (errors.find(error_codes[j]) != errors.end())
                continue;
            std::string path =
                "./resources/errors/" + toString(error_codes[j]) + ".html";
            std::string content = _readFile(path);
            servers[i].getConfig().setErrors(error_codes[j], content);
        }
    }
}

/**
 * @brief find servers which should run on ports already used by other server,
 * and store them in a separate vector of our cluster
 *
 * @param cluster
 */

void Parser::_findVirtualServers(Cluster& cluster) const
{
    std::vector<int> already_in_use;
    std::vector<Server>& servers = cluster.getServers();

    std::vector<Server>::iterator it_servers = servers.begin();
    
    while (it_servers != servers.end())
    {
        std::vector<int> ports = it_servers->getConfig().getPorts();
        bool is_virtual_server = false;

        for (size_t i = 0; i < ports.size(); i++)
        {
            // If a server is running on a port already in use, we make it a
            // "virtual server"
            if (std::find(already_in_use.begin(), already_in_use.end(),
                          (ports[i])) != already_in_use.end())
            {
                cluster.addVirtualServer(*it_servers);
                it_servers = servers.erase(it_servers); // Erase and update iterator
                is_virtual_server = true;
                break; // Exit the loop if the server is virtual
            }
        }

        // If the server wasn't erased, we add its ports and move to the next one
        if (!is_virtual_server)
        {
            for (size_t i = 0; i < ports.size(); i++)
                already_in_use.push_back(ports[i]);

            ++it_servers;
        }
    }
}

// =============================================================================
//                               Helper
// =============================================================================
bool Parser::_isHttpMethod(const std::string& method) const
{
    return (method == "GET" || method == "POST" || method == "DELETE");
}

bool Parser::_isDuplicatedLocation(const std::string& url,
                                   const std::vector<Route>& routes) const
{
    for (size_t i = 0; i < routes.size(); i++)
    {
        if (routes[i].url == url)
            return (true);
    }
    return (false);
}

bool Parser::_isCgiPath(const std::string& path) const
{
    std::string command = "which " + path + " > /dev/null 2>&1";
    int result = system(command.c_str());
    return (result == 0);
}

bool Parser::_isCgiExtension(const std::string& extension) const
{
    if (extension.empty() || extension[0] != '.')
        return false;
    for (size_t i = 0; i < extension.size(); i++)
    {
        int dot = 0;
        if (extension[i] == '.')
            dot++;
        if (dot > 1)
            return (false);
    }
    return true;
}

/**
 * @brief check if port is a valid port or if it is not already in use by other
 * servers.
 *
 * @param port_nb
 * @param ports
 * @return
 */
bool Parser::_isValidPort(int port_nb, const std::vector<int>& ports) const
{
    if (port_nb < 0 || port_nb > 65535)
        return (false);
    for (size_t i = 0; i < ports.size(); i++)
        if (port_nb == ports[i])
            return (false);
    return true;
}
// =============================================================================
//                               Debug
// =============================================================================
/**
 * @brief Print the token list on stdout
 */
void Parser ::_debugTokenList(void) const
{
    std::list<Token>::const_iterator it;

    for (it = _token_list.begin(); it != _token_list.end(); it++)
    {
        std::cout << _tokenTypeToString(it->type);
        std::cout << " | " << it->content << std::endl;
    }
    std::cout << std::endl;
}

void Parser::_debugConfigs(Cluster& cluster) const
{
    std::vector<Server>& servers = cluster.getServers();

    for (size_t i = 0; i < servers.size(); i++)
    {
        std::cout << CYAN << "CONFIG " << i << RESET << std::endl;
        std::cout << servers[i].getConfig() << std::endl;
        std::cout << "\n\n" << std::endl;
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

// =============================================================================
//                               Exception
// =============================================================================
Parser ::SynthaxException::SynthaxException(const Token& token,
                                            const std::string& reason)
    : _token(token), _reason(reason)
{
}

const char* Parser::SynthaxException::what() const throw()
{
    std::stringstream msg;
    msg << _reason << " line " << _token.line_nb << ": ";
    msg << "content: " << _token.content
        << ", type: " << _tokenTypeToString(_token.type) << std::endl;

    _msg = msg.str();
    return (_msg.c_str());
}
