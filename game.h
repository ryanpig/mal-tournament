#include "player.h"
#include "config.h"



//temporary payoff matrix for testing 
static const int mat_payoffs[NUM_OF_PLAYERS * NUM_OF_ACTIONS][NUM_OF_PLAYERS] = {{3,3}, {5,0}, {0,5}, {1,1}};

class Game
{
	public:
		int m_rounds; // the number of rounds in a game
		int m_num_of_players;
		int m_cur_round;
		vector<Player*> m_players; // the number of players in a game
		vector<int> m_selected_actions; // actions selected by each players
		//Printing parameter
		int m_print_top;
		int m_print_last;


		// constructor	
		Game(uint rounds, uint num_of_players, int print_top, int print_last) : m_cur_round(0), m_print_top(3), m_print_last(1)
		{
			// initialization
			m_rounds = rounds;
			m_num_of_players = num_of_players;
			m_print_top = print_top;
			m_print_last = print_last;

			// print basic inforamtion of the game
			cout << "Total players: " << m_num_of_players << ", Total rounds: " << m_rounds << ", Action size:" << NUM_OF_ACTIONS << endl;
			cout << "-- Payoff Matrix --" << endl;
			for(int i = 0; i < m_num_of_players; i++)
			{
				cout << "--Payoff of player " << i << endl;
				for(int j = 0; j < m_num_of_players * NUM_OF_ACTIONS;j++)
				{
					// cout << "i:" << i << ", j:" << j << ", payoff:" << mat_payoffs[j][i] << endl; 
					cout << mat_payoffs[j][i] << "," ;
					if(j % NUM_OF_ACTIONS == 1)
						cout << endl;
				}
			}

			// create n players & set its strategy  
			for(int player_ind = 0; player_ind < m_num_of_players; player_ind++)
			{
				Player* p;
				if(player_ind==0)
					p = new Player(0, player_ind, NUM_OF_ACTIONS); 
				else
					p = new Player(1, player_ind, NUM_OF_ACTIONS); 
				m_players.push_back(p);
			}
		}

		void single_step();
		void print_player_info();
			
};

void Game::single_step()
{
	m_cur_round++;
	// action selection
  m_selected_actions.clear();	
	for(auto player : m_players)
	{
		// update rounds
		player->m_info.m_cur_round = m_cur_round;
		// exec player's algorithm
		m_selected_actions.push_back(player->select_action());
	}
	// calculate payoffs by looking up the matrix 
	// TODO: adaptive to n actions
	int ind = m_selected_actions[0] + m_selected_actions[1] * NUM_OF_ACTIONS;
	// cout << "action(p0):" << m_selected_actions[0] << ", action(p1):" << m_selected_actions[1] << ",ind:" << ind << endl;
	for(int i=0; i< NUM_OF_PLAYERS; i++)
	{
		Player *p = m_players[i];
		//reward for player i
		int reward = mat_payoffs[ind][i];
	  p->m_info.m_acc_payoffs_by_action[p->current_action] += reward;
		p->m_info.m_counts_by_action[p->current_action] += 1;
	  //p1: action 0,1 / p2:action 0,1 (mat_payoffs[4][2] 
	  p->m_acc_payoffs += reward;
		p->payoff_history.push_back(reward);
		// cout << "player " << i << " get " << mat_payoffs[ind][i] << endl;
	}

	//show result
	print_player_info();
	
}

// print each player's action history, payoff history, and acc_payoff 
void Game::print_player_info()
{
  if(m_cur_round <= m_print_top || m_cur_round > m_rounds - m_print_last)
	{
		cout << "---step " << m_cur_round << " ----" << endl;
		for(auto p : m_players)
		{
			cout << "player: " << p->index << ", "<< p->current_strategy->getname() << endl;
			if(m_cur_round <= m_print_top)
			{
				p->print_action_history();
				p->print_payoff_history();
			}
			p->print_action_statistic();
			p->print_payoffs();
			cout << "Avg. payoff:" << (float)(p->m_acc_payoffs) / m_cur_round << endl;
			cout << "----------" << endl;
		}
	}
}
