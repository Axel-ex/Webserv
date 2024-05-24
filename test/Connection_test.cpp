/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection_test.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 10:24:12 by Axel              #+#    #+#             */
/*   Updated: 2024/05/24 13:15:37 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gtest/gtest.h"
#include "../includes/Connection.hpp"

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

TEST(Connection, content_size)
{
	Connection connection;

	connection.appendBuffer(buffer, buffer.size());
	ASSERT_EQ(connection.getContentLength(), 316);
}

TEST(Connection, isRequestOver)
{
	Connection connection;

	connection.appendBuffer(buffer, buffer.size());
	connection.appendBuffer(buffer2, buffer2.size());
	ASSERT_EQ(connection.isRequestOver(), true);
	ASSERT_EQ(connection.getContentLength(), 316);
}
