/* Player class provide the information of its accumulated payoffs in the game and their strategy selection
 *
 */

#include "strategy.h"

class Player
{
	public:
		int acc_payoffs;
		Strategy current_strategy;
		
		Player(Strategy s) : acc_payoffs(0)
		{
			current_strategy = s;
		}
};
