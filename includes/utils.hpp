/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 14:20:49 by achabrer          #+#    #+#             */
/*   Updated: 2024/09/23 19:19:15 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <map>
#include <unistd.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <ctime>
#include <fcntl.h>
#include <cstdio>
#include <fstream>
#include <sys/time.h>

template <typename T>

std::string toString(T val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}

extern bool stopFlag;
void    sigHandler(int signum);
void    sigHandler2(int signum);
double	getTime(void);

#endif  // UTILS_HPP_
