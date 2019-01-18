
#include "strategy.h"

// Action selection based on maximizing (Xavg[i] + optimized term)
//	1. average observed rewards of action i (Xavg[i]) = accumulated payoffs of action i / the number of iterations t
//	2. optimized term = math.sqrt(2*log(t)/ni)  (ni: the number that the action i has been played)
// Regret bound (8 * 1st_Sum + (constant1) * 2nd_Sum)
//  1. 1st_Sum: sum of (log(t) / delta(i) ) 
//  2. constant1: (1+ PI*2 / 3) 
//  3. 2nd_Sum: sum of delta(i)
//  4. delta(i) = u* - ui  (note:best expected mean payoff , expected mean payoff of action i)
int Strategy_UCB1::exec(Info &inf)
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
		// cout << "action size(UCB1):" << inf.m_action_size << endl;
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
			// cout << "x_avg[i]:" << x_avg[i] << ", opt:" << opt << ", evaluated[i]:" << evaluated[i] << endl;
		}
		// cout << endl;
		// select action based on evaluated result 
		int select_action = max_element(evaluated.begin(), evaluated.end()) - evaluated.begin();
		// cout << "select_action:" << select_action << ",evaluted:" << evaluated[select_action] << endl;

		return select_action;
	}
}
