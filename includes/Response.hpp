/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 14:43:01 by Axel              #+#    #+#             */
/*   Updated: 2024/05/14 09:22:31 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include <string>

#define DUMMY_RESPONSE                                                         \
    "<!DOCTYPE html><html><head><title>Test "                                  \
    "Page</title></head><body><h1>Hello, World!</h1></body></html>"

class Response
{
    public:
        Response(void);
        ~Response();
        const char* get_body(void);
		std::string get_headers(void);

    private:
        std::string _headers;
        std::string _body;
};

#endif // RESPONSE_HPP_
