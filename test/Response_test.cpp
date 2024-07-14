/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_test.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 10:11:03 by Axel              #+#    #+#             */
/*   Updated: 2024/05/24 17:48:22 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gtest/gtest.h"
#include "../includes/Request.hpp"
#include "../includes/Response.hpp"
#include <sstream>
#include <string>

TEST(GETResponse, simple_valid)
{
	std::string buff("GET / HTTP/1.1\r\nHost: whatever\r\nAccept:whatever\r\ncontent-length: 40");
	Request request(buff);
	Response response(request);

	std::istringstream stream(response.getHeaders());
	std::string status_line;
	std::getline(stream, status_line);

	ASSERT_EQ(status_line, "HTTP/1.1 200 OK\r");
}

TEST(GETResponse, invalid_resource)
{
	std::string buff("GET /not_exist HTTP/1.1\r\nHost: whatever\r\nAccept:whatever\r\ncontent-length: 40");
	Request request(buff);
	Response response(request);

	std::istringstream stream(response.getHeaders());
	std::string status_line;
	std::getline(stream, status_line);

	ASSERT_EQ(status_line, "HTTP/1.1 404 Not Found\r");
}

TEST(GETResponse, invalid_protocol)
{
	std::string buff("GET / HTTP/2.1\r\nHost: whatever\r\nAccept:whatever\r\ncontent-length: 40");
	Request request(buff);
	Response response(request);

	std::istringstream stream(response.getHeaders());
	std::string status_line;
	std::getline(stream, status_line);

	ASSERT_EQ(status_line, "HTTP/1.1 400 Bad Request\r");
}
