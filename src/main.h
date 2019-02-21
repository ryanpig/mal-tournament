#include "config.h"
#include "gamut_parser.h"
#include "game.h"
#include "common.h"
#include "sqlite.h"
#include "gametype.h"
#include <mutex>
#include <thread>
#include "threading.h"

void cleanTempFiles();

class GameGenerator
{
	public:
		bool run_tournament(int total_rounds, GameType& gt);
    bool run_all_games(int total_rounds);
    bool run_all_games_mt(int total_rounds);
};
