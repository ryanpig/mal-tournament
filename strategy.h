#include <iostream>
#include <vector>
#include <random>
#include <math.h>
#include <algorithm>
#include "math_utility.h"

using namespace std;

enum StrategyType {Random , UCB1 , EXP3}; 
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
		float m_acc_regrets;
		int m_cur_round;
		int last_action;
		float last_reward;

		// constructor 
		Info(int action_size) : m_action_size(action_size), m_acc_regrets(0), last_action(0), last_reward(0.0f){
			m_acc_payoffs_by_action.resize(action_size,0);
			m_counts_by_action.resize(action_size,0);
		}
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
		//base constructor
		Strategy(int ind, int act_size, StrategyType s) : index(ind), action_size(act_size), type(s) 
		{index = ind;}
		// agent, that may or may not use opponenet(s)' information, executes the strategy to select its action. 
		virtual int exec(Info &i) = 0;
		int get_index(){return index;}
	  int action_size;
		StrategyType type;
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
		Strategy_Random(int act) : Strategy(0, act, StrategyType::Random)
		{
			// cout << "Strategy Random is selected." << endl;
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
		Strategy_UCB1(int act) : Strategy(1, act, StrategyType::UCB1), initial_counts(0), initial_flag(true)
		{
			// cout << "Strategy UCB1 is selected." << endl;
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
		float gamma; // TODO: tunnable parameter
		default_random_engine random_eng;

		Strategy_EXP3(int act) : Strategy(2, act, StrategyType::EXP3), gamma(0.07)
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
		static Strategy* createNewStrategy(StrategyType type, int action_size) {
			if(type == StrategyType::Random) return new Strategy_Random(action_size); 
			else if(type == StrategyType::UCB1) return new Strategy_UCB1(action_size);
			else if(type ==  StrategyType::EXP3) return new Strategy_EXP3(action_size);
			else {
				cerr << "strategy type is not supported!" << endl;
				return nullptr;
			}
		}
		string getname(StrategyType type){
			switch(type){
				case StrategyType::Random: return "Random";break;
				case StrategyType::UCB1: return "UCB1";break;
				case StrategyType::EXP3: return "EXP3";break;
			  default: return "Unknown Strategy";													
			}

		}

		template<typename T>
		void printVec(vector<T> &v){for(auto e : v) cout<<e<<" ,"; cout << endl;};
} strategy_Mgr;
