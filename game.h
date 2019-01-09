#include "player.h"



class Game
{
	public:
		uint m_turns; // the number of turns in a game
		vector<Player*> m_players; // the number of players in a game

		// constructor	
		Game(uint turns, uint num_of_players)
		{
			m_turns = turns;
			for(size_t i = 0; i < num_of_players; i++)
			{
				Strategy s;
				Player* p = new Player(s);
				m_players.push_back(p);
			}
		}

		
};

