/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 15:00:56 by Axel              #+#    #+#             */
/*   Updated: 2024/05/14 09:23:49 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"

Response ::Response(void)
{
    /*TODO: parse a request*/
    _body = DUMMY_RESPONSE;
}

Response ::~Response(void) {}

const char* Response::get_body(void) { return (_body.c_str()); }

std::string Response::get_headers(void)
{
    std::string headers = "HTTP/1.1 200 OK\r\n";
    headers += "Content-Type: text/html\r\n";
    headers += "Content-Length: " + std::to_string(_body.length()) + "\r\n\r\n";
    return (headers);
}
