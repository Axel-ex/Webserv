/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 14:20:49 by achabrer          #+#    #+#             */
/*   Updated: 2024/08/09 15:36:09 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>
#include <sstream>
#include <iostream>
#include <map>

template <typename T>
std::string toString(T val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}

extern bool stopFlag;
void sigHandler(int signum);
void sigHandler2(int signum);

#endif  // UTILS_HPP_
