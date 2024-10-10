/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 14:20:49 by achabrer          #+#    #+#             */
/*   Updated: 2024/10/10 10:46:48 by Axel             ###   ########.fr       */
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
#include <map>

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
		std::pair<int, std::string> redirect_url;
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

namespace CgiTools
{
	std::string getFileExtension(const std::string &url);
	bool isExtensionAllowed(const std::string &url, const std::vector<std::string> &cgi_extensions);
	void sendHttpErrorResponse(int client_fd, int error_code, const std::map<int, std::string> &errors);
	unsigned int convertHex(const std::string &nb);
	void sendErrorPage(int client_fd, std::string file_path, int status_code);
	int getStatusCode(int error_code);
}

#endif // UTILS_HPP_
