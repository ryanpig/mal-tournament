#include "player.h"

// based on strategy and info, the agent selects best action.(not really best)
int Player::select_action()
{
	current_action = current_strategy->exec(m_info);
	action_history.push_back(current_action);
	return current_action;
}

void Player::print_action_statistic()
{
	// print all action relevant information 
	cout << "Actions info:" << endl;
	for(size_t i = 0; i < m_info.m_acc_payoffs_by_action.size(); i++)
	{
		float acc = m_info.m_acc_payoffs_by_action[i]; 
		float count = m_info.m_counts_by_action[i];
		float avg = acc / count;
		cout << "action " << i << ":" << acc << "(acc_payoffs), " << count << "(times)," << avg << "(average)" << endl;
	}
}
