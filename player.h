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
		vector<int> acc_payoff_history; //debug
		vector<int> action_history; //debug
		int m_action_size;

		// constructor
		Player(StrategyType strategy_type, int ind, int action_size) : m_acc_payoffs(0)
		{
			// cout << "Player " << ind << ", ";
			current_strategy = strategy_Mgr.createNewStrategy(strategy_type, action_size);
			index = ind;
			m_info.m_action_size = action_size;
			m_info.m_acc_payoffs_by_action.resize(action_size,0);
			m_info.m_counts_by_action.resize(action_size,0);
		}

		// return an action by its strategy
		int select_action();

		// helper functions
		void print_payoffs(){cout << "Accumulated Payoff:" << m_acc_payoffs << endl;}
		void print_action_history(){for(auto i: action_history){cout<<i<<"-- ";} cout<<endl;}
		void print_action_statistic();
		void print_payoff_history(){for(auto i: payoff_history){cout<<i<<"-- ";} cout<<endl;}
		void print_acc_regret(){ cout << "Accumulated Regrets:" << m_info.m_acc_regret << endl;}
};


