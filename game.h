#include "player.h"



class Game
{
	public:
		int m_turns; // the number of turns in a game
		vector<Player*> m_players; // the number of players in a game

		// constructor	
		Game(uint turns, uint num_of_players)
		{
			m_turns = turns;
			cout << "Total players: " << num_of_players << ", Total turns: " << m_turns;

			for(size_t i = 0; i < num_of_players; i++)
			{
				Strategy s;
				Player* p = new Player(s, i);
				m_players.push_back(p);
			}
		}

		void single_step();
			
};

void Game::single_step()
{
	for(auto player : m_players)
	{
		cout << "player:" << player->index << endl;
		player->exec();
		player->m_info.print_history();
		player->print_payoffs();
	}
}
