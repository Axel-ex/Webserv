/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlers.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebmarque <ebmarque@student.42porto.com     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 09:41:40 by Axel              #+#    #+#             */
/*   Updated: 2024/10/10 11:03:59 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTHANDLERS_HPP_
#define REQUESTHANDLERS_HPP_

#include "../includes/Request.hpp"
#include "../includes/Response.hpp"
#include <map>

class ARequestHandler
{
    public:
        ARequestHandler(void);
        virtual ~ARequestHandler(void);

        void handleRequest(const Request& request, Response& response);
        void setNextHandler(ARequestHandler* next);
        void initializeMimeTypes();

    protected:
        std::map<std::string, std::string> mimeTypes;
        void _createErrorResponse(int error_code, Response& response) const;
        void _createOkResponse(std::string resource, Response& response) const;
        virtual bool _canProcess(const Request& request,
                                 const std::vector<Route>& routes) const = 0;
        virtual void processRequest(const Request& request,
                                    Response& response) const = 0;
        std::string _getErrorReason(int error_code) const;

    private:
        ARequestHandler* _next;
};

class GetRequestHandler : public ARequestHandler
{
    public:
        void processRequest(const Request& request, Response& response) const;

    private:
        bool _canProcess(const Request& request,
                         const std::vector<Route>& routes) const;
        std::string _get_file_content(std::string path,
                                      Response& response) const;
		void _createAutoIndexResponse( const std::string &true_path, const std::string &route_path, Response &response) const;
		void _createRedirectResponse(Response &response, Route &route) const;
};

class PostRequestHandler : public ARequestHandler
{
    public:
        void processRequest(const Request& request, Response& response) const;

    private:
        bool _canProcess(const Request& request,
                         const std::vector<Route>& routes) const;

        bool _bodySizeCheck(const Request& request, const Config& config) const;
        std::string _getContentType(const std::string& headers) const;
        std::string _getBoundary(const std::string& headers) const;
        std::string _getFileContent(const std::string& body,
                                    const std::string& boundary) const;
        std::string _getFileName(const std::string& body) const;
        void createResponse(std::string resource, Response& response) const;
        void _createDir(std::string dir_name) const;
};

class DeleteRequestHandler : public ARequestHandler
{
    public:
        void processRequest(const Request& request, Response& response) const;
        std::string _getPath(const Request& request, Response& response) const;

    private:
        bool _canProcess(const Request& request,
                         const std::vector<Route>& routes) const;
};

#endif // REQUESTHANDLERS_HPP_
