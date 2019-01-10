#include "player.h"

#define NUM_OF_PLAYERS 2
#define NUM_OF_TURNS 100
#define NUM_OF_ACTIONS 2
#define SEED_NUMBER 235

//temporary payoff matrix for testing 
static const int mat_payoffs[NUM_OF_PLAYERS * NUM_OF_ACTIONS][NUM_OF_PLAYERS] = {{3,3}, {5,0}, {0,5}, {1,1}};

class Game
{
	public:
		int m_turns; // the number of turns in a game
		int m_num_of_players;
		int m_cur_turn;
		vector<Player*> m_players; // the number of players in a game
		vector<int> m_selected_actions; // actions selected by each players

		// constructor	
		Game(uint turns, uint num_of_players) : m_cur_turn(0)
		{
			// initialization
			m_turns = turns;
			m_num_of_players = num_of_players;
			
			// random seed
			srand(SEED_NUMBER);

			// print basic inforamtion of the game
			cout << "Total players: " << m_num_of_players << ", Total turns: " << m_turns << endl;
			cout << "-- Payoff Matrix --" << endl;
			for(int i = 0; i < m_num_of_players; i++)
			{
				for(int j = 0; j < m_num_of_players * NUM_OF_ACTIONS;j++)
				{
					cout << "i:" << i << ", j:" << j << ", payoff:" << mat_payoffs[j][i] << endl; 
				}
			}

			// create n players & set its strategy (now only random strategy is available) 
			for(int i = 0; i < m_num_of_players; i++)
			{
				Strategy s;
				Player* p = new Player(s, i);
				m_players.push_back(p);
			}
		}

		void single_step();
		void print_player_info();
			
};

void Game::single_step()
{
	m_cur_turn++;
	// action selection
  m_selected_actions.clear();	
	for(auto player : m_players)
	{
		// cout << "player:" << player->index << endl;
		m_selected_actions.push_back(player->select_action());
	}
	// calculate payoffs by looking up the matrix
	int ind = m_selected_actions[0] + m_selected_actions[1] * NUM_OF_ACTIONS;
	cout << "action(p0):" << m_selected_actions[0] << ", action(p1):" << m_selected_actions[1] << ",ind:" << ind << endl;
	for(int i=0; i< NUM_OF_PLAYERS; i++)
	{
	 //p1: action 0,1 / p2:action 0,1 (mat_payoffs[4][2] 
	  m_players[i]->m_acc_payoffs += mat_payoffs[ind][i];
		m_players[i]->payoff_history.push_back(mat_payoffs[ind][i]);
		cout << "player " << i << " get " << mat_payoffs[ind][i] << endl;
	}

	//show result
	print_player_info();
	
}

// print each player's action history, payoff history, and acc_payoff 
void Game::print_player_info()
{

	for(auto player : m_players)
	{
		cout << "player: " << player->index << endl;
		player->print_action_history();
		player->print_payoff_history();
		player->print_payoffs();
		cout << "Avg. payoff:" << (float)(player->m_acc_payoffs) / m_cur_turn << endl;
		cout << "----------" << endl;
	}
}
