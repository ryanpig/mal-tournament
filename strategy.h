//TODO: this should be changed to abstract class
//TODO:use C++ <random> to generate certain distribution
// #include <rand>
#include <iostream>
#include <vector>
using namespace std;

// Info class is useless now, but it would've been used to pass opponent history in MAL algorithms later on.
// Info could be seen as the knowledge of the agent
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


// any algorithm will be implemented as a Strategy object 
class Strategy
{
	public:
		Strategy(){;}
		// agent, that may or may not use opponenet(s)' information, executes the strategy to select its action. 
		virtual int exec(Info i) = 0;
};

class Strategy_Random : public Strategy 
{
	public:		
		using Strategy::Strategy;
		// equivalent to Str_random() : Strategy() {}

		int exec(Info inf) 
		{
			int a = rand() % 100 + 1;
			// cout << a << endl;
			if(a > 50) return 1;
			else return 0;
		}
};

