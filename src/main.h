#include "config.h"
#include "gamut_parser.h"
#include "game.h"
#include "common.h"
#include "sqlite.h"
#include "gametype.h"
#include "threading.h"

void cleanTempFiles();

class GameGenerator
{
	public:
		bool run_tournament(int total_rounds, GameType& gt, size_t total_stratagies);
    bool run_all_games(int total_rounds);
    bool run_all_games_mt(int total_rounds, int set_players, int set_actions, bool set_mt, size_t total_stratagies);
};
