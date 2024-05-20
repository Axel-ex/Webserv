/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 14:43:01 by Axel              #+#    #+#             */
/*   Updated: 2024/05/20 10:49:08 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include "../includes/Request.hpp"
#include <string>

#define DUMMY_RESPONSE                                                         \
    "<!DOCTYPE html><html><head><title>Test "                                  \
    "Page</title></head><body><h1>Hello, World!</h1></body></html>"

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
