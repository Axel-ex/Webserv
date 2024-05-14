/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 14:28:23 by Axel              #+#    #+#             */
/*   Updated: 2024/05/14 09:27:59 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>

class Request
{
    public:
        // TODO: Parse the request in the constructor
        Request(const char& buffer);
        ~Request();

        const std::string& get_method(void);
        const std::string& get_resource(void);
        const std::string& get_protocol(void);

    private:
        std::string _method;   // GET POST DELETE
        std::string _resource; // / (root) /whatever
        std::string _body;
};

#endif // INCLUDE_INCLUDES_REQUEST_HPP_
