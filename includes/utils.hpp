/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 14:20:49 by achabrer          #+#    #+#             */
/*   Updated: 2024/09/26 14:38:35 by ebmarque         ###   ########.fr       */
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
#include "Config.hpp"
#include "Request.hpp"

template <typename T>

std::string toString(T val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}

extern bool stopFlag;
void        sigHandler(int signum);
void        sigHandler2(int signum);
double	    getTime(void);
std::string getMatch(std::vector<Route> &routes, std::string &resource, std::string method);
Route       getBestRoute(const Request& request);
std::string generateUniqueSessionID();
std::string generateExpiryDate(int maxAgeInSeconds);


#endif  // UTILS_HPP_
