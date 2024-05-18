/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Axel <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 10:08:45 by Axel              #+#    #+#             */
/*   Updated: 2024/05/18 12:11:23 by Axel             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_HPP_
#define LOG_HPP_

#include <string>

#define RED "\033[31m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define RESET "\033[0m"
#define CLEAR "\033[2J\033[H"

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
        ~Log();

        static void setLoglevel(e_log_level level);
        static void log(e_log_level level, const std::string& msg);
		static void clearScreen(void);
        static void printCurrentDirectory(void);

    private:
        e_log_level _log_level;

        Log();
        void _logWarning(const std::string &msg) const;
        void _logError(const std::string &msg) const;
        void _logDebug(const std::string &msg) const;
        void _logInfo(const std::string &msg) const;
};

#endif // LOG_HPP_
