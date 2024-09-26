/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 14:43:01 by Axel              #+#    #+#             */
/*   Updated: 2024/09/26 14:15:51 by ebmarque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include "../includes/Request.hpp"
#include <string>
#include <vector>

typedef struct s_cookie
{
	std::string name;
	std::string value;
	std::string expires;
	std::string maxAge;
	// std::string domain;
	std::string path;
	bool secure;
	bool httpOnly;
}              Cookie;

// Error codes
#define BAD_REQUEST 400
#define NOT_FOUND 404
#define INTERNAL_ERROR 500
#define PAYLOAD_LARGE 413

class Response
{
public:
	Response(const Request &request);
	~Response();

	const std::string &getBody(void) const;
	const std::string &getHeaders(void) const;
	const std::string getResponseBuffer(void) const;

	void setHeaders(const std::string &headers);
	void setBody(const std::string &body);
	void setCookie(const Cookie& cookie);
private:
	std::string _headers;
	std::string _body;
	std::vector<Cookie> _cookies;
};

#endif // RESPONSE_HPP_
