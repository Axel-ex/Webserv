/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 14:20:49 by achabrer          #+#    #+#             */
/*   Updated: 2024/10/08 11:43:58 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include "Request.hpp"
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

typedef enum ProcessStatus
{
    EXITED,
    SIGNALED
} t_exitType;

typedef struct s_chldProcess
{
        pid_t pid;
        int status;
        t_exitType type;
} t_chldProcess;

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

typedef struct Route
{
        std::string url;
        std::string root;
        std::vector<std::string> methods;
        std::string upload_store;
        std::string index;
        std::vector<std::string> cgi_path;
        std::vector<std::string> cgi_extension;
        bool autoindex;
} Route;

template <typename T>

std::string toString(T val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}

extern bool stopFlag;

void sigHandler(int signum);
void sigHandler2(int signum);

namespace ServerTools
{
	double getTime(void);
	std::string getMatch(const std::vector<Route>& routes, std::string& resource,
						std::string method);
	Route getBestRoute(const Request& request, const std::vector<Route>& routes);
}

#endif // UTILS_HPP_
