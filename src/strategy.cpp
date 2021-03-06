#include "strategy.h"

// choosing action by probability vector
inline int choose_by_probability(vector<float> &probs, RNG &rng)
{
	float rand_choice = rng.getReal();
	for(size_t i = 0; i < probs.size(); i++){
		rand_choice -= probs[i];
		if(rand_choice < 0.0f)
			return i;
	}

  LOG(ERROR) << "wrong operation of choose_by_probability:" <<  endl;
	strategy_Mgr.printVec(probs);
	return 0;
}

// generate average rewards by giving accumulated payfoss and counts by each action
// in1:acc payoff, in2:acc counts, out:prob
inline void get_average_vector(vector<float> &in1, vector<int> &in2, vector<float> &out)
{
  assert(in1.size() == in2.size());
  assert(in2.size() == out.size());

	for(size_t i = 0; i < in1.size(); i++){
		if(in2[i] == 0)
			out[i] = 0.0f;
		else
			out[i] = in1[i] / in2[i]; 
	}

  for(auto v : out){
    if(isnan(v))
      LOG(ERROR) << "nan!! here";
  }
    
}	

// return action w/ maximum average reward
inline int argmax_pick(Info &inf)
{
	// action w/ maximum reward
	// float max{0.0f};
	int max_reward_action{0};
	vector<float> vec_avg(inf.m_action_size, 0.0f);
	get_average_vector(inf.m_acc_payoffs_by_action, inf.m_counts_by_action, vec_avg);
	auto it  = max_element(vec_avg.begin(), vec_avg.end());
	// max = *it;
	max_reward_action = it - vec_avg.begin();
	return max_reward_action;
}


int Strategy_Random::exec(Info &inf)
{
		int choice = m_rng.getInt(0, action_size - 1);
		return choice; 
}

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
		// cout << "select_action(ini):" << initial_counts -1 << endl;
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

/* Exp3 algorithm 
 * 1. Initialization
 *  -  wi = 1 
 * 2. Draw an action according to weights 
 * 3. Generate probability distribution 
 *  -  pi(t) = (1 - gamma) * wi / sum(wi(t)) + gamma / N (note: pi: probability of action i ; wi: weight of action i )
 * 4. get reward r(t)
 * 5. Update weights
 *  - r^i(t) = r(t) / pi(t)  if action(t) = i else 0
 *  - wi *= math.exp(gamma * r^i(t) / N ) (note: N: the number of actions)
 *  */

// TODO: think how to pass reward to calculation (e.g. update weights by previous reward?)
int Strategy_EXP3::exec(Info &inf)
{
  // initialization 
  if(inf.m_cur_round == 1)
  {
    rewardMax = inf.gp->getMaxReward();
    rewardMin = inf.gp->getMinReward();
  }
	// retrive info from previous action/reward
	float reward = inf.last_reward; 	
	int last_action = inf.last_action;
	// calculate probability distribution by the formula in 3
	prob_distr_calc(); 
	// update weights
	float scaledReward = (reward - rewardMin) / (rewardMax - rewardMin);
	float estimated_reward = scaledReward / probs[last_action]; 
	weights[last_action] *= exp(gamma * estimated_reward / weights.size());
	// for output
	// weights_history.push_back(weights);
	// probs_history.push_back(probs);
	// TODO: check this compared to normalization
  // check if any weight is too large to be out of range
	checkMaxRange(weights, 0.5, 1000000000.0f);

	//debug
	bool m_f_print = false;
	// cout << "last reward:" << reward << endl;
	if(inf.m_cur_round % 5000 == 0 && m_f_print == true){
		cout << "rounds:" << inf.m_cur_round << endl;
		cout << "probs:";
		strategy_Mgr.printVec(probs);
		cout << "weights:";
		strategy_Mgr.printVec(weights);
		cout << "last act:" << last_action << ", weight:" << weights[last_action] << ", estimated_reward:"<< estimated_reward << ", exp:" << exp(gamma * estimated_reward / weights.size()) << endl;

	}
	// draw a action based on weights
	uint select_action = draw_action();
	return select_action;
}

// generate probability distribution
void Strategy_EXP3::prob_distr_calc()
{
	float sum_weights = sum(weights);
	float a = ( 1 - gamma) / sum_weights;
	float b = gamma / weights.size();
  for(size_t i = 0; i < weights.size(); i++){
		probs[i] = a * weights[i] + b;
		if(probs[i] < 0.0f || probs[i] > 1.0f)
      LOG(ERROR) << "ERROR: probs[i]:" << probs[i] << ", a:" << a << ",b:" << b << endl;
	}
}

void Strategy_EXP3::checkMaxRange(vector<float> &w, float degradedFactor, float threshold) 
{
	// strategy_Mgr.printVec(w);
	float max = *std::max_element(w.begin(), w.end());
	if(max > threshold)
	{
		for(size_t i = 0; i < w.size(); i++){
			w[i] *= degradedFactor; 
		}
		// strategy_Mgr.printVec(w);
		// cout << "weights is too larger, the scaling occurs." << endl;
	}
}

// randomly pick one action according to weights
int Strategy_EXP3::draw_action()
{
	float sum_weights = sum(weights);
	std::uniform_real_distribution<> distr(0, sum_weights); // TODO:Verify the randomness
	float rand_choice = distr(random_eng);
	// debug:
	// cout << "Weights:";
	// strategy_Mgr.printVec(weights);	
	// cout << "sum_weights:" << sum_weights << ", rand_choice:" << rand_choice << endl;
	for(size_t i = 0; i < weights.size(); i++){
		rand_choice -= weights[i];
		if(rand_choice < 0.0f)
		{
			// cout << "choice:" << i << endl;
			return i;
		}
	}
	return 0;
}

int Strategy_Satisficing::exec(Info &inf)
{
	int select_action{0};
	// retrive info from previous action/reward
	float current_reward = inf.last_reward; 	
	int current_action = inf.last_action;
	// calculate
	m_aspiration_level = 0.95f * m_aspiration_level + 0.05 * current_reward;
	select_action = current_reward < m_aspiration_level ? m_rng.getInt(0, action_size - 1) : current_action; 
	// debug:
	// cout << "m_aspiration_level:" << m_aspiration_level << ", current_reward:" << current_reward << ", pre_act:" << current_action << ", new:" << select_action << endl;
	return select_action;
}

int Strategy_EGreedy::exec(Info &inf)
{
	int select_action{0};
	if(inf.m_cur_round <= inf.m_action_size )
		return inf.m_cur_round - 1;
	else
	{
		select_action = (m_rng.getReal() < epsilon) ? m_rng.getInt(0, action_size -1) : argmax_pick(inf);
		//debug:
		// cout << "max:"<< max << ", max(act):" << argmax_pick(inf) << ", select_action:" << select_action << endl;
	}

	return select_action;
}

int Strategy_NGreedy::exec(Info &inf)
{
	int select_action{0};
	if(inf.m_cur_round <= inf.m_action_size )
		return inf.m_cur_round - 1;
	else
	{
		float r = m_rng.getReal();
		float threshold = (float)(inf.m_action_size) / (float)(inf.m_cur_round);
		// select_action = (m_rng.getReal() < (inf.m_action_size / inf.m_cur_round)) ? m_rng.getInt(0, action_size -1) : argmax_pick(inf);
		select_action = (r < threshold) ? m_rng.getInt(0, action_size -1) : argmax_pick(inf);
		//debug:
		// cout << inf.m_action_size << ", " << inf.m_cur_round << endl;
		// cout << "prob:"<< r << ", threshold:" << threshold << "max_act:" <<  argmax_pick(inf) << ", select_action:" << select_action << endl;
	}

	return select_action;
}

int Strategy_Softmax::exec(Info &inf)
{

	// using initialization
	if(inf.m_cur_round <= inf.m_action_size )
		return inf.m_cur_round - 1;
	else{
		int select_action{0};
		vector<float> vec_avg(inf.m_action_size, 0.0f);
		get_average_vector(inf.m_acc_payoffs_by_action, inf.m_counts_by_action, vec_avg);

    // do the maximux check 
    auto find_maxQ = [](vector<float> &v){ return max_element(v.begin(), v.end());};
    auto count_maxQ = [](vector<float> &v, float max_value){ return std::count(v.begin(), v.end(), max_value);};
    auto iter_maxQ = find_maxQ(vec_avg);
    auto beg = vec_avg.begin();
    auto end = vec_avg.end();
    float maxQ = *iter_maxQ;
    int counts_of_max = count_maxQ(vec_avg, maxQ);

    // branch if there are more than 1 maximum values 
    if(counts_of_max == 1){
      softmax(vec_avg.begin(), vec_avg.end(), vec_avg.begin(), true, 0.3f); // temperature = 1.0f;
      //DEBUG
      // strategy_Mgr.printVec(vec_avg);
      select_action = choose_by_probability(vec_avg, m_rng);
    }else{
			vector<int> matches;			
			for(auto it = beg; it != end; it++)
				if(*it == maxQ) 
					matches.push_back(it - beg);
			select_action = matches[m_rng.getInt(0, matches.size() - 1)];
    }

		return select_action;
	}
}

int Strategy_NoRegret::exec(Info &inf)
{
	if(inf.m_cur_round <= inf.m_action_size )
		return inf.m_cur_round - 1;
	else{
		int select_action{0};
		vector<float> vec = inf.m_acc_hypo_reward_by_action;
		select_action = max_element(vec.begin(), vec.end()) - vec.begin();
		//debug:
		// cout << "hypo reward" << endl;
		// strategy_Mgr.printVec(vec);
		return select_action;
	}
}
// argmax(argmax(opp.actions).avg_payoffs)
int Strategy_FP::exec(Info &inf)
{
	// initialization
	if(inf.m_cur_round <= inf.m_action_size )
		return inf.m_cur_round - 1;
	else
	{
		int select_action{0};
		// find each player's the most frequnet action by action counts
		vector<int> frequent_actions;
		for(auto info : inf.opp_history)
		{
			vector<int> v = info->m_counts_by_action;
			int frequent_action{0};
			// if the count of actions are equal, pick a random action.
			if(std::adjacent_find( v.begin(), v.end(), std::not_equal_to<>() ) == v.end())
				frequent_action = m_rng.getInt(0, action_size - 1);
			else
				frequent_action = max_element(v.begin(), v.end()) - v.begin();
			frequent_actions.push_back(frequent_action);
		}
		// debug
		// cout << "the most frequent actions for each player" << ends;
		// strategy_Mgr.printVec(frequent_actions);

		// pick the best action based on average payoff of each action
		float max_payoffs{0.0f};
		int max_action{0};
		int pid = inf.player_index;
		for(int i = 0; i < inf.m_action_size; i++)
		{
			frequent_actions[pid] = i;
			float payoff = inf.gp->queryByVec(frequent_actions)[pid];
			if(payoff > max_payoffs)
			{
				max_payoffs = payoff;
				max_action = i;
			}
		}
		select_action = max_action;
		
		return select_action;
	}
}
int Strategy_QL::exec(Info &inf)
{
	int current_state = inf.last_action;
	int reward = inf.last_reward;
	auto find_maxQ = [](vector<float> &v){ return max_element(v.begin(), v.end());};
	auto count_maxQ = [](vector<float> &v, float max_value){ return std::count(v.begin(), v.end(), max_value);};
	int best_action{0};
	
	// get max Q
	auto iter_maxQ = find_maxQ(q_history);
	auto beg = q_history.begin();
	auto end = q_history.end();
	float maxQ = *iter_maxQ;
	int counts_of_max = count_maxQ(q_history, maxQ);

	// deploy e-greedy policy w/ decreasing epsilon
	if(m_rng.getReal() < epsilon)
	{
		best_action = m_rng.getInt(0, action_size - 1);
    // LOG(DEBUG) << "random action:" << best_action ;
	}
	else
	{
		// branch by found maximum counts
		if(counts_of_max == 1)
		{
		  best_action = iter_maxQ - beg;
			// LOG(DEBUG) << "choose action by best Q:" << best_action ; 
		}else if (counts_of_max >= 1)
		{
			vector<int> matches;			
			for(auto it = beg; it != end; it++)
				if(*it == maxQ) 
					matches.push_back(it - beg);
			best_action = matches[m_rng.getInt(0, matches.size() - 1)];
			// LOG(DEBUG) << "maxQ:" << maxQ << ", choose" << best_action << " due to more than 2 maximum values" ;
		}
	}

	// LOG(DEBUG) << "before Q-matrix: "; 
	// strategy_Mgr.printVec(q_history);
	
	// update q-matrix (NOTE: check if the maxQ is needed to set to 1 )
	q_history[current_state] = learning_rate * (reward + discount_factor * 1) + (1 - learning_rate) * q_history[current_state];

	// LOG(DEBUG) << "update Q-matrix: ";
	// strategy_Mgr.printVec(q_history);

	epsilon *= decreasing_exploration;
	return best_action;
}

int Strategy_BrFP::exec(Info &inf)
{
	int select_action{0};
	return select_action;
}

int Strategy_Markov::exec(Info &inf)
{
	int select_action{0};
	return select_action;
}

