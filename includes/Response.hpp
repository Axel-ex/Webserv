/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 14:43:01 by Axel              #+#    #+#             */
/*   Updated: 2024/10/08 11:24:47 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include "../includes/Request.hpp"
#include "../includes/Config.hpp"
#include <string>

//Error codes
#define BAD_REQUEST 400
#define FORBIDDEN 403
#define NOT_FOUND 404
#define METHOD_NOT_ALLOWED 405
#define INTERNAL_ERROR 500
#define PAYLOAD_LARGE 413

class Response
{
    public:
        Response(const Request& request, Config &server_config);
        ~Response();

        const std::string& getBody(void) const;
        const std::string& getHeaders(void) const;
		const std::string getResponseBuffer(void) const;

        void setHeaders(const std::string& headers);
        void setBody(const std::string& body);
		
		const Config &getServerConfig(void) const;

    private:
		Config _server_config;
        std::string _headers;
        std::string _body;
};

#endif // RESPONSE_HPP_
