#include "easylogging++.h"

#define UNUSED(x) (void)(x)
#pragma once

static void logging_configuration()
{
	// Load configuration from file
	el::Configurations conf("logging.conf");
	// // Reconfigure single logger
	el::Loggers::reconfigureLogger("default", conf);
	// // Actually reconfigure all loggers instead
	el::Loggers::reconfigureAllLoggers(conf);
	el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput); 
	// Now all the loggers will use configuration from file
}
