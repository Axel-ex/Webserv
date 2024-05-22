/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 10:11:30 by Axel              #+#    #+#             */
/*   Updated: 2024/05/18 13:04:01 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Log.hpp"
#include <iostream>
#include <unistd.h>

Log ::Log(void) {}

Log::~Log(void) {}

Log& Log::getInstance()
{
    static Log instance;

    return (instance);
}

void Log::setLoglevel(s_log_level level) { getInstance()._log_level = level; }

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
    if (getInstance()._log_level >= ERROR)
        std::cerr << "[" << RED << "ERROR" << RESET << "] " << msg << std::endl;
}

void Log::_logWarning(const std::string& msg) const
{
    if (getInstance()._log_level >= WARNING)
        std::cout << "[" << YELLOW << "WARNING" << RESET << "] " << msg << std::endl;
}

void Log::_logInfo(const std::string& msg) const
{
    if (getInstance()._log_level >= INFO)
        std::cout << "[" << "INFO" << "] " << msg << std::endl;
}

void Log::_logDebug(const std::string& msg) const
{
    if (getInstance()._log_level >= DEBUG)
        std::cout << "[" << CYAN << "DEBUG" << RESET << "] " << msg << std::endl;
}
