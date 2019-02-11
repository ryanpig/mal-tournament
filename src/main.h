#include "config.h"
#include "gamut_parser.h"
#include "game.h"
#include "common.h"
#include "sqlite.h"

void logging_configuration()
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

class GameGenerator
{
	public:
		bool run_tournament(int total_rounds);
};
