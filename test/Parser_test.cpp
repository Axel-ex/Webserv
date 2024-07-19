/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 09:20:15 by Axel              #+#    #+#             */
/*   Updated: 2024/07/19 16:41:10 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Config.hpp"
#include "../includes/Parser.hpp"
#include "gtest/gtest.h"
#include <string>
#include <vector>

TEST(Parser, simple_config)
{
    Parser parser;

    parser.parse("resources/config/default.conf");
    ASSERT_EQ(Config::getPorts()[0], 80);
    ASSERT_EQ(Config::getPorts().size(), 1);
    ASSERT_EQ(Config::getServerName(), "example.com");
    ASSERT_EQ(Config::getMaxBodySize(), 10);
    Config::clear();
}

TEST(Parser, Routes)
{
    Parser parser;

    parser.parse("resources/config/default.conf");
    std::vector<Route> routes = Config::getRoutes();
    ASSERT_EQ(routes.size(), 1);
	ASSERT_EQ(routes[0].methods.size(), 2);
	ASSERT_EQ(routes[0].methods[0], "GET");
	ASSERT_EQ(routes[0].methods[1], "POST");
	ASSERT_EQ(routes[0].root, "./resources");
	ASSERT_EQ(routes[0].index, "index.html");
	ASSERT_EQ(routes[0].upload_store, "./uploads");
	ASSERT_EQ(routes[0].cgi_extension, ".php");
}
