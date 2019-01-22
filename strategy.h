#include <iostream>
#include <vector>
#include <random>
#include <math.h>
#include <algorithm>
#include "math_utility.h"

using namespace std;

// Info class is useless now (player maintains these info.), but it would've been used to pass opponent history in MAL algorithms later on.
class Info
{
	public:
		// variables
		int m_action_size;
		vector<int> my_history; // last m steps
		vector<Info*> opp_history; //TODO: using vector<Info*> 
		vector<int> m_acc_payoffs_by_action;
		vector<int> m_counts_by_action;
		int m_acc_regret;
		int m_cur_round;

		// constructor 
		Info() : m_action_size(0), m_acc_regret(0){;}
		void print_history()
		{
			for(int i : my_history) cout << i << "-" << endl;
		}
};



// Any algorithm will be implemented as a Strategy object 
class Strategy
{
	protected:
		int index;
	public:
		vector<string> strategy_names = {"Random", "UCB1", "EXP3"}; //TODO:Change to Enum
		//base constructor
		Strategy(int ind, int act_size) : index(ind), action_size(act_size)
		{index = ind;}
		// agent, that may or may not use opponenet(s)' information, executes the strategy to select its action. 
		virtual int exec(Info &i) = 0;
		string getname(){return strategy_names[index];}
		int get_index(){return index;}
	  int action_size;
};

/* Random algorithm
 */
// algorithm related parameter

class Strategy_Random : public Strategy 
{
	public:		
		// using Strategy::Strategy;
		// equivalent to Str_random() : Strategy() {}
		std::default_random_engine eng;
		Strategy_Random(int act) : Strategy(0, act)
		{
			cout << "Strategy Random is selected." << endl;
			uint seed = std::chrono::steady_clock::now().time_since_epoch().count();
			eng.seed(seed);
		}

		int exec(Info &inf) 
		{
			std::uniform_int_distribution<int> distr(0, action_size - 1);
			int choice = distr(eng);
			return choice; 
		}
};

static const float constant1 = 1 + (M_PI * M_PI / 3);

class Strategy_UCB1 : public Strategy 
{
	
	public:
		// Variables
		int initial_counts;
		bool initial_flag;
		// Constructor
		Strategy_UCB1(int act) : Strategy(1, act), initial_counts(0), initial_flag(true)
		{
			cout << "Strategy UCB1 is selected." << endl;
		}

		int exec(Info &inf);

};

class Strategy_EXP3: public Strategy 
{
	public:
		// Variables
		vector<float> weights;
		vector<float> probs;
		vector<vector<float>> weights_history;
		float gamma; // tunnable parameter
		default_random_engine random_eng;

		Strategy_EXP3(int act) : Strategy(2, act), gamma(0.07)
		{
			weights.resize(action_size, 1.0f);
			probs.resize(action_size, 0.0f);
			// To randomly pick an action, we initialize the random engine w/ a seed
			uint seed = std::chrono::steady_clock::now().time_since_epoch().count();
			random_eng.seed(seed);
		}
		
		int exec(Info &inf);
		void prob_distr_calc();
		int draw_action();
};

// Compostion: all common apis can be put here rather than in pure Strategy class
static class Strategy_Mgr
{
	public:
		static Strategy* createNewStrategy(int strategy_index, int action_size) {
			if(strategy_index == 0) return new Strategy_Random(action_size); //TODO:change to Enum StrategyType {Random, UCB1, EXP3}
			else if(strategy_index == 1) return new Strategy_UCB1(action_size);
			// else if(strategy_index == 2) return new Strategy_EXP3(action_size);
			else {
				cerr << "strategy index " << strategy_index << " is not supported!" << endl;
				return nullptr;
			}
		}
} strategy_Mgr;
