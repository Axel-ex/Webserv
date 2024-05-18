/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 14:28:23 by Axel              #+#    #+#             */
/*   Updated: 2024/05/17 09:12:08 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>

class Request
{
    public:
        // TODO: Parse the request in the constructor
        Request(std::string buffer);
        ~Request();

        const std::string& getMethod(void) const;
        const std::string& getResource(void) const;
        const std::string& getProtocol(void) const;
        const std::string& getHeaders(void) const;
        const std::string& getBody(void) const;

    private:
        std::string _method;   // GET POST DELETE
        std::string _resource; // / (root) /whatever
        std::string _protocol;
        std::string _headers;
        std::string _body;
};

#endif // INCLUDE_INCLUDES_REQUEST_HPP_
