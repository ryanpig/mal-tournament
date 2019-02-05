#include "game.h"


void Game::single_step()
{
	m_cur_round++;
	// action selection
  m_selected_actions.clear();	
	for(auto &player : m_players)
	{
		// update rounds
		player->m_info.m_cur_round = m_cur_round;
		// exec player's algorithm
		m_selected_actions.push_back(player->select_action());
	}

	// payoffs calculation 
	vector<float> cur_payoffs = getPayoffs();

	// store data to each player
  for(auto &p : m_players)
	{
		// int reward = mat_payoffs[ind][i];
		float reward = cur_payoffs[p->index]; 
		// store to m_info : accumulated payoffs by selected action, counts by selected action
	  p->m_info.m_acc_payoffs_by_action[p->current_action] += reward;
		p->m_info.m_counts_by_action[p->current_action] += 1;
		p->m_info.last_reward = reward;
		p->m_info.last_action = p->current_action;
		// store accumulated payoffs 
	  p->m_acc_payoffs += reward;
		// store payoff and accumulated payoffs history
		p->payoff_history.push_back(reward);
		p->acc_payoff_history.push_back(p->m_acc_payoffs);
		// store accumulated regret
		float regret = regret_cal(p, m_selected_actions);
		p->m_acc_regrets += regret;		
		p->m_info.m_acc_regrets += regret;
		p->acc_regret_history.push_back(p->m_acc_regrets);
		// store accumulated regret by action
		update_hypo_rewards_by_action(p, m_selected_actions);
	}
	
	// data out
	// show result
	if(f_print)
		print_player_info();
	
}
// regret calculation (using opposite way, calculate hypothetical rewards)
void Game::update_hypo_rewards_by_action(unique_ptr<Player> &p, vector<int> &select_actions)
{
	int pid = p->index;
	int action_size = p->getActionSize(); 
	vector<int> acts;
	acts = select_actions; // using copy assignment
	float cur_reward = getPayoffs()[pid];

	for(int i = 0; i < action_size; i++)
	{
		acts[pid] = i;
		float hypo_reward = m_game_parser->queryByVec(acts)[pid];
		p->m_info.m_acc_hypo_reward_by_action[i] += hypo_reward - cur_reward;
	}
	
}
// regret calcuation (regret = reward of the best action - reward of the current action
float Game::regret_cal(unique_ptr<Player> &p, vector<int> &select_actions)
{
	float regret{0};
	int pid = p->index;
	int action_size = p->getActionSize(); 
	vector<int> acts;
	acts = select_actions; // using copy assignment

	// set initial value of current reward and maximum reward
	float cur_reward = getPayoffs()[pid];
	float max = cur_reward; 

	// find the best action		
	for(int i = 0; i < action_size; i++)
	{
		vector<int> actions =	acts; 
		acts[pid] = i;
		float tmp = m_game_parser->queryByVec(acts)[pid];
		if(tmp > cur_reward)
			max = tmp;
	}
	regret += max - cur_reward;
	// debug
	// cout << "regret:" << regret << ", max:" << max << ", current:" << cur_reward << endl;
	return regret;
}

// print each player's action history, payoff history, and acc_payoff 
void Game::print_player_info()
{
  if(m_cur_round <= m_print_top || m_cur_round > m_rounds - m_print_last)
	{
		cout << "---step " << m_cur_round << " ----" << endl;
		for(auto &p : m_players)
		{
			cout << "player: " << p->index << ", "<< strategy_Mgr.getname(p->current_strategy->type) << endl;
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
	// Game data
	vector<vector<float>> vecs_acc_regret;
	vector<vector<int>> vecs_acc_payoff;
	// Algorithm data
	vector<vector<float>> vec_weights;
	vector<vector<float>> vec_probs; 
	bool writeEXP3 = false;

	// retrieve data
	for(auto &p : m_players)
	{
		vecs_acc_regret.push_back(p->acc_regret_history);
		vecs_acc_payoff.push_back(p->acc_payoff_history);

		// EXP3
		if(p->getCurStrategyType() == StrategyType::EXP3)
		{
			Strategy* ptr_base = p->current_strategy.get();
			Strategy_EXP3 *ptr_exp3 = static_cast<Strategy_EXP3*>(ptr_base);
			vec_weights = std::move(ptr_exp3->weights_history);
			vec_probs = std::move(ptr_exp3->probs_history);
			writeEXP3 = true;
		}
	}
	
	// type definition
	vector<string> vec_type{"regret", "acc_payoffs", "action_history"};
	vector<string> vec_exp3_type{"weights", "probs"};

	// write game data
	string fname = vec_type[0] + std::to_string(getGameID()) + ".csv";
	io_Handler.writeVectorsToCSV(vecs_acc_regret, fname);
	fname = vec_type[1] + std::to_string(getGameID()) + ".csv";
	io_Handler.writeVectorsToCSV(vecs_acc_payoff, fname);
	fname = vec_type[2] + std::to_string(getGameID()) + ".csv";
	io_Handler.writeTwoVectorsToCSV(m_players[0]->action_history, m_players[1]->action_history, fname);
	if(writeEXP3)
	{
		// write algorithm data
		fname = "EXP3_" + vec_exp3_type[0] + std::to_string(getGameID()) + ".csv";
		io_Handler.writeAlgoVectorsToCSV(vec_weights, fname);
		fname = "EXP3_" + vec_exp3_type[1] + std::to_string(getGameID()) + ".csv";
		io_Handler.writeAlgoVectorsToCSV(vec_probs, fname);
	}

}

void Game::run()
{
	for(int i=0;i<m_rounds;i++)
	{
	  single_step();	
	}
	print_final_result();
}

void Game::print_manual_payoff()
{
	// DEBUG: Use manual payoff matrix
	for(int i = 0; i < m_num_of_players; i++)
	{
		cout << "--Payoff of player " << i << endl;
		for(int j = 0; j < m_num_of_players * NUM_OF_ACTIONS;j++)
		{
			// cout << "i:" << i << ", j:" << j << ", payoff:" << mat_payoffs[j][i] << endl; 
			cout << mat_payoffs[j][i] << "," ;
			if(j % NUM_OF_ACTIONS == 1)
				cout << endl;
		}
	}
}

void Game::print_final_result()
{
	string padding  = "         ";
	string padding1 = "Avg. Pay."; 
	string padding2 = "Avg. Reg."; 
	cout << "---FINAL---" << endl;
	cout << padding;
	for(auto &p : m_players)
		cout << strategy_Mgr.getname(p->current_strategy->type) << ",   "; 
	cout << endl;
	// accumulated payoffs table
	cout << padding1;
	for(auto &p : m_players)
	{
		float avg_payoff = (float)(p->m_acc_payoffs) / m_cur_round;
		cout << avg_payoff << " ,";
		m_avg_result.push_back(avg_payoff);
	}
	cout << endl;
	cout << padding2;
	// accumulated regrets table
	for(auto &p : m_players)
	{
		float avg_regrets = (float)(p->m_acc_regrets) / m_cur_round;
		cout << avg_regrets << " ,";
	}

	cout << "----------" << endl;
	
}
