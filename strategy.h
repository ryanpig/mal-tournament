#include <iostream>
#include <vector>
#include <random>
using namespace std;

// Info class is useless now (player maintains these info.), but it would've been used to pass opponent history in MAL algorithms later on.
class Info
{
	public:
		int action_size;
		vector<int> my_history; // last m steps
		vector<int> opp_history; //TODO:it could be matrix (n,m) , n players for last m steps
		Info() : action_size(0){;}
		void print_history()
		{
			for(int i : my_history) cout << i << "-" << endl;
			for(int j : opp_history) cout << j << "-" << endl;
		}
};

// Compostion: all common apis can be put here rather than in pure Strategy class
class Strategy_Mgr
{

};


// Any algorithm will be implemented as a Strategy object 
class Strategy
{
	public:
		Strategy(){;}
		// agent, that may or may not use opponenet(s)' information, executes the strategy to select its action. 
		virtual int exec(Info i) = 0;
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
			uint seed = std::chrono::steady_clock::now().time_since_epoch().count();
			eng.seed(seed);
		}

		int exec(Info inf) 
		{
			// cout << a << endl;
			if(distr(eng) > 50) return 1;
			else return 0;
		}
};

