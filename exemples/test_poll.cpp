/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_poll.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 13:30:35 by Axel              #+#    #+#             */
/*   Updated: 2024/05/10 12:40:42 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/poll.h>
#include <unistd.h>

int main(void)
{
    std::string name;
    struct pollfd mypoll;
    int counter;

    /* Init the struct */
    memset(&mypoll, 0, sizeof(mypoll));
    mypoll.fd = STDIN_FILENO;
    mypoll.events = POLLIN;

    std::cout << "Enter your name" << std::endl;

    while (true)
    {
        /* struct, index of the fd in the list (here only one), timeout*/
        if (poll(&mypoll, 1, 100) == 1)
        {
            std::getline(std::cin, name);
            std::cout << "your name is: " << name << std::endl;
            break;
        }
        /* poll wits only for 100ms, so we can execute other code while the fd
         * we are reading to is not ready to be read*/
        else
            counter++;
    }
    std::cout << "it took you " << counter * 100 << "ms to type in your name";
    return (EXIT_SUCCESS);
}
