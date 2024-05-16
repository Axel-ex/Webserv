/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 11:38:11 by Axel              #+#    #+#             */
/*   Updated: 2024/05/16 14:51:42 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gtest/gtest.h"
#include "../includes/Config.hpp"
#include "../includes/Request.hpp"

TEST(dummytests, test_addition)
{
	int a = 2 + 2;
	
	ASSERT_EQ(a, 4);
}

TEST(Config, instanciation)
{
	Config::parseFile("dummy_file");

	ASSERT_EQ(Config::getPorts()[0], 8080);
	ASSERT_EQ(Config::getPorts()[1], 9000);
}

TEST(Request, simple_request)
{
	std::string buff("GET / HTTP/1.1\r\nHost: whatever\r\nAccept:whatever\r\ncontent-length: 40");
	Request request(buff);

	ASSERT_EQ(request.getMethod(), "GET");
	ASSERT_EQ(request.getResource(), "/");
	ASSERT_EQ(request.getProtocol(), "HTTP/1.1");
	ASSERT_EQ(request.getHeaders(), "\r\nHost: whatever\r\nAccept:whatever\r\ncontent-length: 40");
}

TEST(Request, request_with_body)
{
	std::string buff("POST file_to_post HTTP/1.1\r\nHost: whatever\r\nAccept:whatever\r\ncontent-length: 40\n\nbody");
	Request request(buff);

	ASSERT_EQ(request.getMethod(), "POST");
	ASSERT_EQ(request.getResource(), "file_to_post");
	ASSERT_EQ(request.getProtocol(), "HTTP/1.1");
	ASSERT_EQ(request.getHeaders(), "\r\nHost: whatever\r\nAccept:whatever\r\ncontent-length: 40");
	ASSERT_EQ(request.getBody(), "body");
}

TEST(Request, icomplete_request)
{
	std::string buff("POST HTTP/1.1\r\nHost: whatever\r\nAccept:whatever\r\ncontent-length: 40\n\nbody");
	Request request(buff);

	ASSERT_EQ(request.getMethod(), "POST");
	ASSERT_EQ(request.getResource(), "HTTP/1.1");
	ASSERT_EQ(request.getProtocol(), "Host:");
	ASSERT_EQ(request.getHeaders(), " whatever\r\nAccept:whatever\r\ncontent-length: 40");
	ASSERT_EQ(request.getBody(), "body");
}

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

