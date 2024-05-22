/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 09:41:40 by Axel              #+#    #+#             */
/*   Updated: 2024/05/20 10:55:10 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHANDLERS_HPP_
#define REQUESTHANDLERS_HPP_

#include "../includes/Request.hpp"
#include "../includes/Response.hpp"

class ARequestHandler
{
    public:
        ARequestHandler(void);
        virtual ~ARequestHandler(void);

        void handleRequest(const Request& request, Response& response);
        void setNextHandler(ARequestHandler* next);

    protected:
        void createErrorResponse(int error_code, Response& response) const;
        virtual bool canProcess(const Request& request) const = 0;
        virtual void processRequest(const Request& request,
                                    Response& response) const = 0;

    private:
        ARequestHandler* _next;
};

class GetRequestHandler : public ARequestHandler
{
    public:
        bool canProcess(const Request& request) const;
        void processRequest(const Request& request, Response& response) const;
};

class PostRequestHandler : public ARequestHandler
{
    public:
        bool canProcess(const Request& request) const;
        void processRequest(const Request& request, Response& response) const;
};

class DeleteRequestHandler : public ARequestHandler
{
    public:
        bool canProcess(const Request& request) const;
        void processRequest(const Request& request, Response& response) const;
};

class CgiRequestHandler : public ARequestHandler
{
    public:
        bool canProcess(const Request& request) const;
        void processRequest(const Request& request, Response& response) const;
};

#endif  // REQUESTHANDLERS_HPP_
