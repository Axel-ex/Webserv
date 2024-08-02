/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 14:43:01 by Axel              #+#    #+#             */
/*   Updated: 2024/08/02 11:53:45 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include "../includes/Request.hpp"
#include <string>

#define DUMMY_RESPONSE                                                         \
    "<!DOCTYPE html><html><head><title>Test "                                  \
    "Page</title></head><body><h1>Hello, World!</h1></body></html>"

//Error codes
#define BAD_REQUEST 400
#define NOT_FOUND 404
#define INTERNAL_ERROR 500

class Response
{
    public:
        Response(const Request& request);
        ~Response();

        const std::string& getBody(void) const;
        const std::string& getHeaders(void) const;

        void setHeaders(const std::string& headers);
        void setBody(const std::string& body);

    private:
        std::string _headers;
        std::string _body;
};

#endif // RESPONSE_HPP_
