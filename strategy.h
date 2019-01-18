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



// Any algorithm will be implemented as a Strategy object 
class Strategy
{
	protected:
		int index;
	public:
		vector<string> strategy_names = {"Random", "UCB1"};
		Strategy(uint ind){index = ind;}
		// agent, that may or may not use opponenet(s)' information, executes the strategy to select its action. 
		virtual int exec(Info &i) = 0;
		string getname(){return strategy_names[index];}
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
		Strategy_Random() : Strategy(0)
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
	
	public:
		// Variables
		int initial_counts;
		bool initial_flag;
		// Constructor
		Strategy_UCB1() : Strategy(1), initial_counts(0), initial_flag(true)
		{
			cout << "Strategy UCB1 is selected." << endl;
		}

		int exec(Info &inf);

};

class Strategy_EXP3: public Strategy 
{

};

// Compostion: all common apis can be put here rather than in pure Strategy class
static class Strategy_Mgr
{
	public:
		Strategy* createNewStrategy(int strategy_index) {
			if(strategy_index == 0) return new Strategy_Random();
			else if(strategy_index == 1) return new Strategy_UCB1();
			else cerr << "strategy index " << strategy_index << " is not supported!" << endl;
		}
};
