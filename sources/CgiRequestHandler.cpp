/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequestHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:36:42 by ebmarque          #+#    #+#             */
/*   Updated: 2024/08/10 18:23:10 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CgiRequestHandler.hpp"


CgiRequestHandler::CgiRequestHandler(const CgiRequestHandler &other)
{
	_ch_env = other._ch_env;
	_env = other._env;
	_poll_fd = other._poll_fd;	
	_method = other._method;
	_resource = other._resource;
	_protocol = other._protocol;
	_headers = other._headers;
	_body = other._body;
}

CgiRequestHandler &CgiRequestHandler::operator=(const CgiRequestHandler &other)
{
	if (this != &other)
	{
		_ch_env = other._ch_env;
		_env = other._env;
		_poll_fd = other._poll_fd;
		_method = other._method;
		_resource = other._resource;
		_protocol = other._protocol;
		_headers = other._headers;
		_body = other._body;
	}
	return (*this);
}

CgiRequestHandler::~CgiRequestHandler()
{
}

// =============================================================================
//                                    CGI HANDLER
// =============================================================================

std::string get_working_path()
{
   char temp[MAXPATHLEN];
   return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
}

void CgiRequestHandler::init_ch_env(void)
{
	
}

void CgiRequestHandler::init_cgi_env(void)
{
	std::string wd = get_working_path();
	
	
}

CgiRequestHandler::CgiRequestHandler(const Request& request, int fd)
{
	_poll_fd = fd;	
	_method = request.getMethod();
	_resource = request.getResource();
	_protocol = request.getProtocol();
	_headers = request.getHeaders();
	_body = request.getBody();
	init_cgi_env();
	init_ch_env();
}

bool CgiRequestHandler ::_canProcess(const Request &request)
{
	std::vector<Route> routes = Config::getRoutes();
	std::string r_url = request.getResource();

	/*Verify if the requested resource exist in the previously defined locations*/
	for (size_t i = 0; i < routes.size(); i++)
	{
		// std::cout << "CGI VERIFYING...\n" << "requested url: " << r_url << "\nlocation url: " << routes[i].url << "\n";
		if (r_url.find(routes[i].url) == 0 &&
			std::find(routes[i].methods.begin(), routes[i].methods.end(), request.getMethod()) != routes[i].methods.end())
		{
			std::cout << "CGI got here...\n";
			if (routes[i].cgi_extension.empty() == false)
			{
				std::string cgi_ext = routes[i].cgi_extension;
				if (r_url.find(cgi_ext) != std::string::npos)
					return (true);
				continue;
			}
		}
	}
	// std::cout << "CGI CANNOT PROCESS...\n";
	return (false);
}

void CgiRequestHandler ::processRequest(const Request &request)
{
	(void)request;
	std::cout << "hello world from cgi" << std::endl;
}
