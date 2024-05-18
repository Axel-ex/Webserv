/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 14:43:01 by Axel              #+#    #+#             */
/*   Updated: 2024/05/17 10:18:16 by Axel             ###   ########.fr       */
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

        const std::string &getBody(void) const;
        const std::string &getHeaders(void) const;

    private:
        std::string _headers;
        std::string _body;

        void _createErrorResponse(int error_code);
        void _createGetResponse(const Request& request);
        void _createPostResponse(void);
        void _createDeleteResponse(void);
};

#endif // RESPONSE_HPP_
