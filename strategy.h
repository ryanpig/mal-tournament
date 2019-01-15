#include <iostream>
#include <vector>
#include <random>
#include <math.h>
#include <algorithm>
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
		int m_cur_round;

		// constructor 
		Info() : m_action_size(0){;}
		void print_history()
		{
			for(int i : my_history) cout << i << "-" << endl;
		}
};

// Compostion: all common apis can be put here rather than in pure Strategy class
class Strategy_Mgr
{
	public:
};


// Any algorithm will be implemented as a Strategy object 
class Strategy
{
	protected:
		int index;
	public:
		vector<string> strategy_names = {"Random", "UCB1"};
		Strategy(){;}
		// agent, that may or may not use opponenet(s)' information, executes the strategy to select its action. 
		virtual int exec(Info &i) = 0;
		string getname(int i){return strategy_names[i];}
		void set_index(int i){index = i;}
		int get_index(){return index;}
};

/* Random algorithm
 */
// algorithm related parameter
static std::uniform_int_distribution<int> distr(0,99); //99 is included

class Strategy_Random : public Strategy 
{
	public:		
		// using Strategy::Strategy;
		// equivalent to Str_random() : Strategy() {}
		std::default_random_engine eng;
		Strategy_Random() : Strategy()
		{
			cout << "Strategy Random is selected." << endl;
			uint seed = std::chrono::steady_clock::now().time_since_epoch().count();
			eng.seed(seed);
		}

		int exec(Info &inf) 
		{
			// cout << a << endl;
			if(distr(eng) > 50) return 1;
			else return 0;
		}
};

static const float constant1 = 1 + (M_PI * M_PI / 3);

class Strategy_UCB1 : public Strategy 
{
  // Action selection based on maximizing (Xavg[i] + optimized term)
	//	1. average observed rewards of action i (Xavg[i]) = accumulated payoffs of action i / the number of iterations t
	//	2. optimized term = math.sqrt(2*log(t)/ni)  (ni: the number that the action i has been played)
	// Regret bound (8 * 1st_Sum + (constant1) * 2nd_Sum)
	//  1. 1st_Sum: sum of (log(t) / delta(i) ) 
	//  2. constant1: (1+ PI*2 / 3) 
	//  3. 2nd_Sum: sum of delta(i)
	//  4. delta(i) = u* - ui  (note:best expected mean payoff , expected mean payoff of action i)
	
	public:
		// Variables
		int initial_counts;
		bool initial_flag;
		// Constructor
		Strategy_UCB1() : Strategy(), initial_counts(0), initial_flag(true)
		{
			cout << "Strategy UCB1 is selected." << endl;
		}

		int exec(Info &inf)
		{
			//initial
			if(initial_flag)
			{
				initial_counts++;
				if(initial_counts == inf.m_action_size)
					initial_flag = false;
				cout << "select_action(ini):" << initial_counts -1 << endl;
				// each action has to be played once
				return initial_counts - 1;
			}
			else
			{
				// normal 
				cout << "action size(UCB1):" << inf.m_action_size << endl;
				vector<float> x_avg(inf.m_action_size, 0.0);
				vector<float> evaluated(inf.m_action_size, 0.0);
				int t = inf.m_cur_round; 

				for(size_t i = 0; i < x_avg.size(); i++)
				{
					// 1st term
					x_avg[i] = (float)(inf.m_acc_payoffs_by_action[i]) / inf.m_counts_by_action[i];	
					// 2nd term
					float opt	= sqrt(2 * log(t) / inf.m_counts_by_action[i]);
					// sum of 1st and 2nd term
					evaluated[i] = x_avg[i] + opt;
					cout << "x_avg[i]:" << x_avg[i] << ", opt:" << opt << ", evaluated[i]:" << evaluated[i] << endl;
				}
				cout << endl;
				// select action based on evaluated result 
				int select_action = max_element(evaluated.begin(), evaluated.end()) - evaluated.begin();
				cout << "select_action:" << select_action << ",evaluted:" << evaluated[select_action] << endl;

				return select_action;
			}
		}

};
