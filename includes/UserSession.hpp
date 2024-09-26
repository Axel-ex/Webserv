/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserSession.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 15:03:40 by ebmarque          #+#    #+#             */
/*   Updated: 2024/09/26 15:07:34 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USERSESSION_HPP
#define USERSESSION_HPP
#include <string>
#include <ctime>

class UserSession
{
public:
	std::string username;
	time_t lastActiveTime;

	UserSession() : username(""), lastActiveTime(time(NULL)) {}
	UserSession(const std::string &user) : username(user), lastActiveTime(time(NULL)) {}

	void updateLastActive()
	{
		lastActiveTime = time(NULL);
	}
	~UserSession(){};
};

#endif // USERSESSION_HPP