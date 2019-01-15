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
		int strategy_index;
		int current_action; // not neccessary
		vector<int> payoff_history; //debug
		vector<int> action_history; //debug
		int m_action_size;

		//const int* pmat_payoffs; 
		
		// constructor
		Player(Strategy* s, int ind, int action_size) : m_acc_payoffs(0)
		{
			current_strategy = s;
			strategy_index = current_strategy->get_index();
			index = ind;
			m_info.m_action_size = action_size;
			m_info.m_acc_payoffs_by_action.resize(action_size,0);
			m_info.m_counts_by_action.resize(action_size,0);
		}

		// return an action by its strategy
		int select_action();

		// helper functions
		void print_payoffs(){cout << "Acculated Payoff:" << m_acc_payoffs << endl;}
		void print_action_history(){for(auto i: action_history){cout<<i<<"-- ";} cout<<endl;}
		void print_action_statistic();
		void print_payoff_history(){for(auto i: payoff_history){cout<<i<<"-- ";} cout<<endl;}
};

// based on strategy and info, the agent selects best action.(not really best)
int Player::select_action()
{
	current_action = current_strategy->exec(m_info);
	action_history.push_back(current_action);
	return current_action;
}

void Player::print_action_statistic()
{
	// print all action relevant information 
	cout << "Actions info:" << endl;
	for(size_t i = 0; i < m_info.m_acc_payoffs_by_action.size(); i++)
	{
		float acc = m_info.m_acc_payoffs_by_action[i]; 
		float count = m_info.m_counts_by_action[i];
		float avg = acc / count;
		cout << "action " << i << ":" << acc << "(acc_payoffs), " << count << "(times)," << avg << "(average)" << endl;
	}
}

