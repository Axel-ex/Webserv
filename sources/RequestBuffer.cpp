/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuffer.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 10:31:17 by Axel              #+#    #+#             */
/*   Updated: 2024/05/27 06:32:45 by axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"
#include "../includes/Log.hpp"
#include <iterator>
#include <cstdlib>

RequestBuffer ::RequestBuffer(void)
    : _content_length(0), _current_length(0), _connection_over(false){};

RequestBuffer ::~RequestBuffer(void) {}

std::string& RequestBuffer::getBuffer(void) { return (_buffer); }

size_t RequestBuffer::getContentLength(void) const { return (_content_length); }

size_t RequestBuffer::getCurrentLength(void) const { return (_current_length); }

bool RequestBuffer ::isRequestOver(void) const { return (_connection_over); }

void RequestBuffer ::appendBuffer(const std::string& buffer, size_t size)
{
    _buffer.append(buffer.c_str(), size);
    _getContentLength();
}

void RequestBuffer::_getContentLength(void)
{
    std::string to_find("Content-Length: ");

    if (_content_length == 0)
    {
        size_t found = _buffer.find(to_find);
        if (found == std::string::npos)
            _connection_over = true;
        else
        {
            std::string::iterator it =
                _buffer.begin() + found + to_find.length();
            std::string length;
            while (it != _buffer.end() && std::isdigit(*it))
            {
                length += *it;
                it++;
            }
            _content_length = std::atoi(length.c_str());
        }
    }

    // return if no body
    if (_connection_over)
        return;
    // current size
    size_t found = _buffer.find("\r\n\r\n");
    if (found == std::string::npos && _content_length != 0)
        Log::log(ERROR, "could not find body");
    std::string::iterator it = _buffer.begin() + found + 4;
	_current_length = std::distance(it, _buffer.end());

    if (_content_length == _current_length)
        _connection_over = true;
}
