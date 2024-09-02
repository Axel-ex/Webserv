/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 11:38:11 by Axel              #+#    #+#             */
/*   Updated: 2024/09/02 12:06:51 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gtest/gtest.h"
#include "../includes/Parser.hpp"

int main(int argc, char **argv)
{
	Parser parser;

	parser.parse("resources/config/default.conf");
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

