/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 11:38:11 by Axel              #+#    #+#             */
/*   Updated: 2024/05/14 08:50:24 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gtest/gtest.h"
#include "../includes/Server.hpp"
#include "../includes/Config.hpp"

//PUT YOUR TEST HERE
//
TEST(dummytests, test_addition)
{
	int a = 2 + 2;
	
	ASSERT_EQ(a, 4);
}

TEST(config, instanciation)
{
	Config &config = Config::getInstance();

	config.parseFile("dummy_file");

	ASSERT_EQ(config.getPorts()[0], 8080);
	ASSERT_EQ(config.getPorts()[1], 9000);

	Config &config2 = Config::getInstance();

	ASSERT_EQ(config2.getPorts()[0], 8080);
	ASSERT_EQ(config2.getPorts()[1], 9000);

}

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

