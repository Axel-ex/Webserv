/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_test.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/17 10:29:42 by Axel              #+#    #+#             */
/*   Updated: 2024/05/26 14:43:14 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gtest/gtest.h"
#include "../includes/Request.hpp"

std::string buffer = "POST /upload HTTP/1.1"
"Host: localhost:8080"
"User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:125.0) Gecko/20100101 Firefox/125.0"
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8"
"Accept-Language: en-GB,en;q=0.5"
"Accept-Encoding: gzip, deflate, br"
"Content-Type: multipart/form-data; boundary=---------------------------37916820296649094161480493629"
"Content-Length: 316"
"Origin: http://localhost:8080"
"Connection: keep-alive"
"Referer: http://localhost:8080/form2"
"Upgrade-Insecure-Requests: 1"
"Sec-Fetch-Dest: document"
"Sec-Fetch-Mode: navigate"
"Sec-Fetch-Site: same-origin"
"Sec-Fetch-User: ?1"
"\r\n\r\n"
"-----------------------------37916820296649094161480493629"
"Content-Disposition: form-data; name=\"file\"; filename=\"new_data.csv\""
"Content-Type: text/csv"
"\r\n"
"whatever";

std::string buffer2 = "-----------------------------37916820296649094161480493629"
"Content-Disposition: form-data; name=\"file\"; filename=\"new_data.csv\""
"Content-Type: text/csv"
"\r\n"
"whatever";


TEST(Request, simple_request)
{
	std::string buff("GET / HTTP/1.1\r\nHost: whatever\r\nAccept:whatever\r\ncontent-length: 40");
	Request request(buff);

	ASSERT_EQ(request.getMethod(), "GET");
	ASSERT_EQ(request.getResource(), "/");
	ASSERT_EQ(request.getProtocol(), "HTTP/1.1");
	ASSERT_EQ(request.getHeaders(), "Host: whatever\r\nAccept:whatever\r\ncontent-length: 40");
}

TEST(Request, request_with_body)
{
	std::string buff("POST file_to_post HTTP/1.1\r\nHost: whatever\r\nAccept:whatever\r\ncontent-length: 40\r\n\r\nbody");
	Request request(buff);

	ASSERT_EQ(request.getMethod(), "POST");
	ASSERT_EQ(request.getResource(), "file_to_post");
	ASSERT_EQ(request.getProtocol(), "HTTP/1.1");
	ASSERT_EQ(request.getHeaders(), "Host: whatever\r\nAccept:whatever\r\ncontent-length: 40");
	ASSERT_EQ(request.getBody(), "body");
}

TEST(Request, icomplete_request)
{
	std::string buff("POST HTTP/1.1\r\nHost: whatever\r\nAccept:whatever\r\ncontent-length: 40\r\n\r\nbody");
	Request request(buff);

	ASSERT_EQ(request.getMethod(), "POST");
	ASSERT_EQ(request.getResource(), "HTTP/1.1");
	ASSERT_EQ(request.getProtocol(), "");
	ASSERT_EQ(request.getHeaders(), "Host: whatever\r\nAccept:whatever\r\ncontent-length: 40");
	ASSERT_EQ(request.getBody(), "body");
}

TEST(RequestBuffer, content_size)
{
	RequestBuffer request_buffer;

	request_buffer.appendBuffer(buffer, buffer.size());
	ASSERT_EQ(request_buffer.getContentLength(), 316);
}

TEST(RequestBuffer, isRequestOver)
{
	RequestBuffer request_buffer;

	request_buffer.appendBuffer(buffer, buffer.size());
	request_buffer.appendBuffer(buffer2, buffer2.size());
	ASSERT_EQ(request_buffer.getContentLength(), 316);
	ASSERT_EQ(request_buffer.isRequestOver(), true);
}


