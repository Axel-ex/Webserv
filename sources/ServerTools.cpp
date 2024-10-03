/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerTools.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 13:22:10 by ebmarque          #+#    #+#             */
/*   Updated: 2024/10/03 13:01:36 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/utils.hpp"
#include "../includes/Parser.hpp"

std::string getMatch(const std::vector<Route> &routes, std::string &resource, std::string method)
{
	std::string match = "";
	for (size_t i = 0; i < routes.size(); i++)
	{
		std::vector<std::string>::const_iterator it = routes[i].methods.begin();
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

Route getBestRoute(const Request& request, const std::vector<Route> &routes)
{
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
