/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 11:38:11 by Axel              #+#    #+#             */
/*   Updated: 2024/05/18 10:37:11 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gtest/gtest.h"
#include <map>
#include "../includes/Config.hpp"

TEST(Config, instanciation)
{
	ASSERT_EQ(Config::getPorts()[0], 8080);
	ASSERT_EQ(Config::getPorts()[1], 9000);
}

TEST(Config, resources)
{
	std::map<std::string, std::string>::iterator it;

	it = Config::getResources().begin();
	ASSERT_EQ(it->first, "/");
}

int main(int argc, char **argv)
{
	Config::parseFile("dummy");

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

