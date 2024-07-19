/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 10:08:45 by Axel              #+#    #+#             */
/*   Updated: 2024/07/18 11:55:01 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_HPP_
#define LOG_HPP_

#include <string>
#include "../includes/Request.hpp"

#define RED "\033[31m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define GREY "\033[90m"
#define RESET "\033[0m"
#define CLEAR "\033[2J\033[H"

#define OUTPUT_OFFSET 12

typedef enum s_log_level
{
    ERROR,
    WARNING,
    INFO,
    DEBUG,
} e_log_level;

class Log
{
    public:
        static Log& getInstance(void);
        ~Log(void);

        static void setLogLevel(e_log_level level);
        static void log(e_log_level level, const std::string& msg);

		static void logRequest(const Request &request);
		static void clearScreen(void);


    private:
        e_log_level _log_level;

        Log();
        void _logWarning(const std::string &msg) const;
        void _logError(const std::string &msg) const;
        void _logDebug(const std::string &msg) const;
        void _logInfo(const std::string &msg) const;

		static void _printTimeStamp(void);
		static std::string _formatField(std::string field);
};

#endif // LOG_HPP_
