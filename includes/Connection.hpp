/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 10:25:36 by Axel              #+#    #+#             */
/*   Updated: 2024/05/24 12:01:41 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP_
#define CONNECTION_HPP_

#include <string>

class Connection
{
    public:
        Connection(void);
        ~Connection(void);

		bool isRequestOver(void) const;
		void appendBuffer(const std::string &buffer, size_t size);
		std::string &getBuffer(void);

		size_t getContentLength(void) const;
		size_t getCurrentLength(void) const;

    private:
        std::string _buffer;
        size_t _content_length;
        size_t _current_length;
		bool _connection_over;

		void _getContentLength(void);
};

#endif // CONNECTION_HPP_
