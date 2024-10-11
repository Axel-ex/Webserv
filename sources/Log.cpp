/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 10:11:30 by Axel              #+#    #+#             */
/*   Updated: 2024/10/11 11:53:51 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Log.hpp"
#include <cstddef>
#include <cstring>
#include <ctime>
#include <iostream>
#include <ostream>
#include <unistd.h>

Log ::Log(void) {}

Log::~Log(void) {}

Log& Log::getInstance()
{
    static Log instance;

    return (instance);
}

void Log::setLogLevel(s_log_level level) { getInstance()._log_level = level; }

void Log::clearScreen(void) { std::cout << CLEAR; }

void Log::log(e_log_level level, const std::string& msg)
{
    switch (level)
    {
    case ERROR:
        getInstance()._logError(msg);
        break;
    case WARNING:
        getInstance()._logWarning(msg);
        break;
    case INFO:
        getInstance()._logInfo(msg);
        break;
    case DEBUG:
        getInstance()._logDebug(msg);
        break;
    }
}

void Log::_logError(const std::string& msg) const
{
    if (getInstance()._log_level < ERROR)
        return;
    _printTimeStamp();
    std::string level = "[ERROR]";
    std::cerr << RED << _formatField(level) << RESET << msg << std::endl;
}

void Log::_logWarning(const std::string& msg) const
{
    if (getInstance()._log_level < WARNING)
        return;
    _printTimeStamp();
    std::string level = "[WARNING]";
    std::cout << YELLOW << _formatField(level) << RESET << msg << std::endl;
}

void Log::_logInfo(const std::string& msg) const
{
    if (getInstance()._log_level < INFO)
        return;
    _printTimeStamp();
    std::string level = "[INFO]";
    std::cout << _formatField(level) << msg << std::endl;
}

void Log::_logDebug(const std::string& msg) const
{
    if (getInstance()._log_level < DEBUG)
        return;
    _printTimeStamp();
    std::string level = "[DEBUG]";
    std::cout << CYAN << _formatField(level) << RESET << msg << std::endl;
}

void Log ::logRequest(const Request& request, const std::string &server_name)
{

    std::string method = "[" + request.getMethod() + "]";
	std::string name = "[" + server_name + "]";
    std::cout << GREY;
    _printTimeStamp();

    std::cout << name << _formatField(method) << request.getResource() << " " << request.getHost();
    std::cout << RESET << std::endl;
}

void Log ::_printTimeStamp(void)
{
    std::time_t time_sec = std::time(NULL);
    std::tm* time_mark = std::localtime(&time_sec);
    char buffer[20];
    std::memset(buffer, 0, sizeof(buffer));

    std::strftime(buffer, sizeof(buffer), "[%H:%M:%S]", time_mark);
    std::cout << buffer;
}

std::string Log::_formatField(std::string field)
{
    for (int i = field.size(); i < OUTPUT_OFFSET; i++)
        field += " ";
    return field;
}
