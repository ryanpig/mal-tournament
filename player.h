/* Player class provide the information of its accumulated payoffs in the game and their strategy selection
 *
 */

#include "strategy.h"

class Player
{
	public:
		int m_acc_payoffs;
		Info m_info;
		int index;
		Strategy* current_strategy;
		int current_action; // not neccessary
		vector<int> payoff_history; //debug
		vector<int> action_history; //debug

		//const int* pmat_payoffs; 
		
		// constructor
		Player(Strategy* s, int ind) : m_acc_payoffs(0)
		{
			current_strategy = s;
			index = ind;
		}

		// return an action by its strategy
		int select_action();

		// helper functions
		void print_payoffs(){cout << "Acculated Payoff:" << m_acc_payoffs << endl;}
		void print_action_history(){for(auto i: action_history){cout<<i<<"-- ";} cout<<endl;}
		void print_payoff_history(){for(auto i: payoff_history){cout<<i<<"-- ";} cout<<endl;}
};

// based on strategy and info, the agent selects best action.(not really best)
int Player::select_action()
{
	current_action = current_strategy->exec(m_info);
	action_history.push_back(current_action);
	return current_action;
}

