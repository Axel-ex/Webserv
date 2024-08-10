/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequestHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:34:18 by ebmarque          #+#    #+#             */
/*   Updated: 2024/08/10 18:21:53 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIREQUESTHANDLER_HPP
#define CGIREQUESTHANDLER_HPP

#include "Request.hpp"
#include "Config.hpp"
#include "Log.hpp"
#include "RequestHandlers.hpp"
#include "Response.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <map>
#include <unistd.h>

#define MAXPATHLEN 4096

class CgiRequestHandler
{
	public:
		CgiRequestHandler(const Request &request, int fd);
		CgiRequestHandler(const CgiRequestHandler &other);
		CgiRequestHandler &operator=(const CgiRequestHandler &other);
		~CgiRequestHandler();

		static bool _canProcess(const Request &request);
		void processRequest(const Request &request);
		void init_cgi_env(void);
		void init_ch_env(void);

	private:
		std::map<std::string, std::string> _env;
		std::string _method;
		std::string _resource;
		std::string _protocol;
		std::string _headers;
		std::string _body;
		int _poll_fd;
		char **_ch_env;
};

#endif // CGIREQUESTHANDLER_HPP
