#include "game.h"


void Game::single_step()
{
	m_cur_round++;
	// action selection
  m_selected_actions.clear();	
	for(auto player : m_players)
	{
		// update rounds
		player->m_info.m_cur_round = m_cur_round;
		// exec player's algorithm
		m_selected_actions.push_back(player->select_action());
	}
	// calculate payoffs by looking up the matrix 
	// TODO: adaptive to n actions
	// int ind = m_selected_actions[0] + m_selected_actions[1] * NUM_OF_ACTIONS;
	vector<float> cur_payoffs = getPayoffs();
	// cout << "action(p0):" << m_selected_actions[0] << ", action(p1):" << m_selected_actions[1] << ",ind:" << ind << endl;
	for(int i=0; i< m_num_of_players; i++)
	{
		Player *p = m_players[i];
		//reward for player i
		// int reward = mat_payoffs[ind][i];
		int reward = cur_payoffs[i]; 
		//store to m_info by selected action
	  p->m_info.m_acc_payoffs_by_action[p->current_action] += reward;
		p->m_info.m_counts_by_action[p->current_action] += 1;
		//store to player data member for all actions 
	  p->m_acc_payoffs += reward;
		p->payoff_history.push_back(reward);
		p->acc_payoff_history.push_back(p->m_acc_payoffs);
	   	
		// DEBUG
		// cout << "player " << i << " get " << mat_payoffs[ind][i] << endl;
	}

	// regret calcuation (regret = reward of the best action - reward of the current action
	// assume player 1 play UCB , and calculate regret for player 1
	// 2 player version,  TODO: for n players
	auto f_regret_cal = [](int player_index, int my_action, int opp_action, int &acc_regret){
		int ind = opp_action + my_action * NUM_OF_ACTIONS; 		
		// vector<float> cur_payoffs = getPayoffs();
		int cur_reward = mat_payoffs[ind][player_index];
		// int cur_reward = cur_payoffs[player_index]; 
		int	max = cur_reward; 
		// find the best action		
		for(int i = 0; i < NUM_OF_ACTIONS; i++)
		{
		  ind = opp_action + i * NUM_OF_ACTIONS; //TODO: assume player 1 use no-regret so that evaluating its diff. actions 
			// int tmp = mat_payoffs[ind][player_index];
			int tmp = mat_payoffs[ind][player_index]; // TODO: change opp_action to all n opponets actions
			if(tmp > cur_reward)
				max = tmp;
		}
		acc_regret += max - cur_reward;
	};
	int my_ind = 1;
	int opp_ind = 0;
	f_regret_cal(my_ind, m_selected_actions[my_ind], m_selected_actions[opp_ind], m_players[my_ind]->m_info.m_acc_regret);
	vec_acc_regret.push_back(m_players[my_ind]->m_info.m_acc_regret);
	
	//show result
	print_player_info();
	
}

// print each player's action history, payoff history, and acc_payoff 
void Game::print_player_info()
{
  if(m_cur_round <= m_print_top || m_cur_round > m_rounds - m_print_last)
	{
		cout << "---step " << m_cur_round << " ----" << endl;
		for(auto p : m_players)
		{
			cout << "player: " << p->index << ", "<< p->current_strategy->getname() << endl;
			if(m_cur_round <= m_print_top)
			{
				p->print_action_history();
				p->print_payoff_history();
			}
			p->print_action_statistic();
			p->print_payoffs();
			p->print_acc_regret();
			cout << "Avg. payoff:" << (float)(p->m_acc_payoffs) / m_cur_round << endl;
			cout << "----------" << endl;
		}
	}
}

void Game::dataToFile()
{
	io_Handler.writeToCSV(vec_acc_regret, "regret.csv");
	io_Handler.writeVectorsToCSV(m_players[0]->acc_payoff_history, m_players[1]->acc_payoff_history, "acc_payoffs.csv");
	io_Handler.writeVectorsToCSV(m_players[0]->action_history, m_players[1]->action_history, "action_history.csv");
}
