/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infinitLoop.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/23 11:20:40 by ebmarque          #+#    #+#             */
/*   Updated: 2024/08/23 11:21:33 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>


int	main(int argc, char *argv[], char *envp[])
{
	(void)argc;
	(void)argv;
	(void)envp;
	
	while(true)
		std::cout << "You're an idiot!" << std::endl;

	return (0);
}
