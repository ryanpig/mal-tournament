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

// any algorithm will be implemented as a Strategy object 
class Strategy
{
	public:
		Strategy(){;}
		// agent, that may or may not use opponenet(s)' information, executes the strategy to select its action. 
		int exec(Info inf)
		{
			int a = rand() % 100 + 1;
			cout << a << endl;
			if(a > 50) return 0;
			else return 1;
		}
};

