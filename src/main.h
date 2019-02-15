#include "config.h"
#include "gamut_parser.h"
#include "game.h"
#include "common.h"
#include "sqlite.h"
#include "gametype.h"

class GameGenerator
{
	public:
		bool run_tournament(int total_rounds, GameType& gt);
    bool run_all_games(int total_rounds);
};
