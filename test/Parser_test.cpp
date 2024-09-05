/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser_test.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 09:20:15 by Axel              #+#    #+#             */
/*   Updated: 2024/09/05 08:40:38 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Config.hpp"
#include "../includes/Parser.hpp"
#include "gtest/gtest.h"
#include <vector>

TEST(Parser, simple_config)
{
    ASSERT_EQ(Config::getPorts()[0], 5000);
    ASSERT_EQ(Config::getPorts().size(), 2);
    ASSERT_EQ(Config::getServerName(), "example.com");
    ASSERT_EQ(Config::getMaxBodySize(), 10);
}

TEST(Parser, Routes)
{
    std::vector<Route> routes = Config::getRoutes();
    ASSERT_EQ(routes.size(), 2);
    ASSERT_EQ(routes[0].methods.size(), 3);
    ASSERT_EQ(routes[0].methods[0], "GET");
    ASSERT_EQ(routes[0].methods[1], "POST");
    ASSERT_EQ(routes[0].root, "./resources");
    ASSERT_EQ(routes[0].index, "index.html");
    ASSERT_EQ(routes[0].upload_store, "./uploads");
}

TEST(Parser, default_errors)
{
	ASSERT_EQ(3, Config::getDefaultErrors().size());
}
