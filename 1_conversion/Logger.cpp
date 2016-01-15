
#include "Logger.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include <typeinfo>


//---------------------- Helper functions ----------------------------------------------------

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}


namespace imaging
{

//----------------------- Logger class implementation -----------------------------------------

	Logger * Logger::logger_instance = NULL;
	
	void Logger::destroy()
	{
		if (logger_instance != NULL)
		{
			delete logger_instance;
			logger_instance = NULL;
		}
	}

	Logger * Logger::request()
	{
		return logger_instance;
	}

	void Logger::create(std::string logfile)
	{
		if (logger_instance!=NULL)
			return;
		else
		{
			logger_instance = new Logger(logfile);
			std::ofstream out(logfile.c_str(), std::ios::out );
			out << "Log file created on " << currentDateTime() << std::endl;
			out.close();
		}
	}

	void Logger::write(std::string entry)
	{
		std::ofstream out(log_filename.c_str(), std::ios::out | std::ios::app);
		out << currentDateTime() << " - " << entry << std::endl;
		out.close();
	}


//----------------------- Logged class implementation -----------------------------------------


	Logged::Logged()
	{
		logger = Logger::request();
	}

	void Logged::addLogEntry(std::string entry)
	{
		if (!logger) // for possible delayed logger creation
			logger = Logger::request();
		
		if (logger)
			logger->write(typeid(*this).name() + std::string(" : ") + entry);
	}
	

	void createLogger(std::string logfile) 
	{
		Logger::create(logfile);
	}

	void destroyLogger()
	{
		Logger::destroy();
	}


} // namespace imaging
