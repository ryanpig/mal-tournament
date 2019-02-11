/* Player class provide the information of its accumulated payoffs in the game and their strategy selection
 *
 */

#include "strategy.h"

class Player
{
	public:
		int m_acc_payoffs;
		int index;
		int m_action_size;
		float m_acc_regrets;

		int current_action; // not neccessary
		vector<int> payoff_history; 
		vector<int> acc_payoff_history; 
		vector<int> action_history; 
		vector<float> acc_regret_history; 

		unique_ptr<Strategy> current_strategy;
		Info m_info;
		GameParser *gp;

		// constructor
		Player(StrategyType strategy_type, int ind, int action_size, GameParser *gp) : m_acc_payoffs(0), index(ind), m_action_size(action_size), m_acc_regrets(0), m_info(action_size, ind), gp(gp)
		{
			// cout << "Player " << ind << ", ";
			current_strategy = strategy_Mgr.createNewStrategy(strategy_type, action_size);
			m_info.gp = gp;
		}

		// deconstructor
		~Player(){}
		// return an action by its strategy
		int select_action();

		// helper functions
		void print_payoffs(){cout << "Accumulated Payoff:" << m_acc_payoffs << endl;}
		void print_action_history(){for(auto i: action_history){cout<<i<<"-- ";} cout<<endl;}
		void print_action_statistic();
		void print_payoff_history(){for(auto i: payoff_history){cout<<i<<"-- ";} cout<<endl;}
		void print_acc_regret(){ cout << "Accumulated Regrets:" << m_info.m_acc_regrets << endl;}
		int getActionSize(){return m_action_size;}
		StrategyType getCurStrategyType(){return current_strategy->type;}
};


