/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 14:28:23 by Axel              #+#    #+#             */
/*   Updated: 2024/07/19 14:40:02 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>

enum RequestMethod{
	GET,
	POST,
	DELETE,
};

class Request
{
    public:
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

class RequestBuffer
{
    public:
        RequestBuffer(void);
        ~RequestBuffer(void);

		bool isRequestOver(void) const;
		void appendBuffer(const std::string &buffer, size_t size);
		std::string &getBuffer(void);

		size_t getContentLength(void) const;
		size_t getCurrentLength(void) const;

    private:
        std::string _buffer;
        size_t _content_length;
        size_t _current_length;
		bool _request_over;

		void _findContentLength(void);
		void _setContentLength(void);
		void _setRequestOver(void);
};

#endif // INCLUDE_INCLUDES_REQUEST_HPP_
