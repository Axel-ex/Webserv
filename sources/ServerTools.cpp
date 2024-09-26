/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerTools.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 13:22:10 by ebmarque          #+#    #+#             */
/*   Updated: 2024/09/26 14:38:15 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/utils.hpp"

#include <random>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string getMatch(std::vector<Route> &routes, std::string &resource, std::string method)
{
	std::string match = "";
	for (size_t i = 0; i < routes.size(); i++)
	{
		std::vector<std::string>::iterator it = routes[i].methods.begin();
		for (; it != routes[i].methods.end(); it++)
		{
			if (*it == method)
				break;
		}
		if (it == routes[i].methods.end())
			continue;
		if (resource.find(routes[i].url) == 0 && routes[i].url.size() > match.size())
			match = routes[i].url;
	}
	return (match);
}

Route getBestRoute(const Request& request)
{
	std::vector<Route> routes = Config::getRoutes();
	std::string resource = request.getResource();
	std::string match = getMatch(routes, resource, request.getMethod());
	Route emptyRoute;

	for (size_t i = 0; i < routes.size(); i++)
	{
		if (routes[i].url == match)
			return (routes[i]);
	}
	return (emptyRoute);
}

std::string generateUniqueSessionID()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    unsigned char buffer[16];
    for (int i = 0; i < sizeof(buffer); ++i)
    {
        buffer[i] = dis(gen);
    }

    std::stringstream ss;
    for (int i = 0; i < sizeof(buffer); ++i)
    {
        ss << std::hex << (int)buffer[i];
    }

    return ss.str();
}


std::string generateExpiryDate(int maxAgeInSeconds)
{
    time_t now = time(0);
    time_t expiryTime = now + maxAgeInSeconds;

    struct tm gmtTime;
    gmtime_r(&expiryTime, &gmtTime);

    std::stringstream ss;
    ss << std::put_time(&gmtTime, "%a, %d %b %Y %H:%M:%S GMT");

    return (ss.str());
}