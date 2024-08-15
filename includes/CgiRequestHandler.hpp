/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRequestHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/09 15:34:18 by ebmarque          #+#    #+#             */
/*   Updated: 2024/08/15 14:37:29 by ebmarque         ###   ########.fr       */
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

using std::map, std::string;
class CgiRequestHandler
{
	public:
		CgiRequestHandler(const Request &request, int fd);
		CgiRequestHandler(const CgiRequestHandler &other);
		CgiRequestHandler &operator=(const CgiRequestHandler &other);
		~CgiRequestHandler();

		static bool _canProcess(const Request &request);
		void processRequest(const Request &request);
		void init_cgi_env(const Request &request);
		void init_ch_env(const Request &request);

	private:
		map<string, string>	_env;
		string				_method;
		string				_resource;
		string				_protocol;
		string				_headers;
		string				_body;
		Route				_location;
		int					_in_pipe[2];
		int					_out_pipe[2];
		int					_client_fd;
		char				**_ch_env;
};

#endif // CGIREQUESTHANDLER_HPP
