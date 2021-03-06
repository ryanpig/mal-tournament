#include <iostream>
#include <vector>
#include <random>
#include <math.h>
#include <algorithm>
#include "math_utility.h"
#include "random_user.h"
#include <chrono>
#include "gamut_parser.h"
#include <memory>
#include "easylogging++.h"
#include <mutex>

#pragma once
using namespace std;

enum StrategyType {Random , UCB1 , EXP3, Satisficing, EGreedy, NGreedy, Softmax, NoRegret, FP, QL, BrFP, Markov}; 

class Info
{
	public:
		// variables
		int m_action_size;
		vector<int> my_history; // last m steps
		vector<Info*> opp_history; //TODO: using vector<Info*> 
		vector<float> m_acc_payoffs_by_action;
		vector<int> m_counts_by_action; // for Softmax, E-Greedy, N-Greedy, UCB1 
		float m_acc_regrets;
		vector<float> m_acc_hypo_reward_by_action; //for No-Regret
		int m_cur_round;
		int last_action;
		float last_reward;
		GameParser *gp;
		int player_index;

		// constructor 
		Info(int action_size, int player_index) : m_action_size(action_size), m_acc_regrets(0), last_action(0), last_reward(0.0f), gp(nullptr), player_index(player_index){
			m_acc_payoffs_by_action.resize(action_size,0.0f);
			m_counts_by_action.resize(action_size,0);
			m_acc_hypo_reward_by_action.resize(action_size,0);
		}
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
		//base constructor
		Strategy(int act_size, StrategyType str_type) : action_size(act_size), type(str_type) 
		{index = str_type;}
		// agent, that may or may not use opponenet(s)' information, executes the strategy to select its action. 
		virtual int exec(Info &i) = 0;
		virtual ~Strategy(){}
		int get_index() const {return type;}
	  int action_size;
		StrategyType type;
};

/* Random algorithm
 */
class Strategy_Random : public Strategy 
{
	public:		
		// using Strategy::Strategy;
		// equivalent to Str_random() : Strategy() {}
		RNG m_rng;
		// std::default_random_engine eng;
		Strategy_Random(int act) : Strategy(act, StrategyType::Random)
		{
			// cout << "Strategy Random is selected." << endl;
			// uint seed = std::chrono::steady_clock::now().time_since_epoch().count();
			// eng.seed(seed);
		}
		~Strategy_Random(){}	
		// Functions
		int exec(Info &inf) override;
};

static const float constant1 = 1 + (M_PI * M_PI / 3);
class Strategy_UCB1 : public Strategy 
{
	public:
		// Variables
		int initial_counts;
		bool initial_flag;
		// Constructor
		Strategy_UCB1(int act) : Strategy(act, StrategyType::UCB1), initial_counts(0), initial_flag(true){;}
		~Strategy_UCB1(){}
		// Functions
		int exec(Info &inf) override;
};

class Strategy_EXP3: public Strategy 
{
	public:
		// Variables
		vector<float> weights, probs;
		vector<vector<float>> weights_history, probs_history;
		float gamma; 
		default_random_engine random_eng;
    float rewardMax, rewardMin;
		// Constructor
		Strategy_EXP3(int act) : Strategy(act, StrategyType::EXP3), gamma(0.07) 
		{
			weights.resize(action_size, 1.0f);
			probs.resize(action_size, 0.0f);
			// To randomly pick an action, we initialize the random engine w/ a seed
			uint seed = std::chrono::steady_clock::now().time_since_epoch().count();
			random_eng.seed(seed);
		}
		~Strategy_EXP3(){}
		// Functions	
		int exec(Info &inf) override;
		void prob_distr_calc();
		int draw_action();
		void checkMaxRange(vector<float> &w, float factor, float thres);
};

class Strategy_Satisficing: public Strategy 
{
	public:		
		// variables
		float m_aspiration_level = 50.0; // set to average of the range of rewards (0,100) / 2
		float m_current_reward = 0.0;
		int m_current_action = 0;
		RNG m_rng;

		// constructor
		Strategy_Satisficing(int act) : Strategy(act, StrategyType::Satisficing)
		{
			;
		}
		~Strategy_Satisficing(){}
		// functions	
		int exec(Info &inf) override;
};

class Strategy_EGreedy: public Strategy 
{
	public:		
		// variables
		RNG m_rng;
		float epsilon = 0.1f;
		// int m_rounds_initial = 20; // always exploration to initialize average payoffs
		// constructor
		Strategy_EGreedy(int act) : Strategy(act, StrategyType::EGreedy)
		{
			;
		}
		// functions	
		int exec(Info &inf) override;
};

class Strategy_NGreedy: public Strategy 
{
	public:		
		// variables
		RNG m_rng;
		// int m_rounds_initial = 1; // always exploration to initialize average payoffs
		// constructor
		Strategy_NGreedy(int act) : Strategy(act, StrategyType::NGreedy)
		{
			;
		}
		~Strategy_NGreedy(){}
		// functions	
		int exec(Info &inf) override;
};

class Strategy_Softmax: public Strategy 
{
	public:		
		// variables
		RNG m_rng;
		// constructor
		Strategy_Softmax(int act) : Strategy(act, StrategyType::Softmax)
		{
			;
		}
		~Strategy_Softmax(){}
		// functions	
		int exec(Info &inf) override;
};

class Strategy_NoRegret: public Strategy 
{
	public:		
		// variables
		// constructor
		Strategy_NoRegret(int act) : Strategy(act, StrategyType::NoRegret)
		{
			;
		}
		~Strategy_NoRegret(){}
		// functions	
		int exec(Info &inf) override;
};

class Strategy_FP: public Strategy 
{
	public:		
		// variables
		RNG m_rng;
		// constructor
		Strategy_FP(int act) : Strategy(act, StrategyType::FP)
		{
			;
		}
		~Strategy_FP(){}
		// functions	
		int exec(Info &inf) override;
};

class Strategy_QL: public Strategy 
{
	public:		
		// variables
		RNG m_rng;
		float learning_rate = 0.1; // learning rate
		float discount_factor = 0.9; // discount factor , close to 1 
		float epsilon = 0.5; // e-greedy policy
		float decreasing_exploration = 0.9;
		vector<float> q_history;		

		// constructor
		Strategy_QL(int act) : Strategy(act, StrategyType::QL)
		{
			// initialize q_matrix
			q_history.resize(action_size, 0.0f);
		}
		~Strategy_QL(){}
		// functions	
		int exec(Info &inf) override;
};
class Strategy_BrFP: public Strategy 
{
	public:		
		// variables
		// constructor
		Strategy_BrFP(int act) : Strategy(act, StrategyType::BrFP)
		{
			;
		}
		~Strategy_BrFP(){}
		// functions	
		int exec(Info &inf) override;
};

class Strategy_Markov: public Strategy 
{
	public:		
		// variables
		// constructor
		Strategy_Markov(int act) : Strategy(act, StrategyType::Markov)
		{
			;
		}
		~Strategy_Markov(){}
		// functions	
		int exec(Info &inf) override;
};

// Compostion: all common apis can be put here rather than in pure Strategy class
static class Strategy_Mgr
{
  private:
    mutex m_mtx;

	public:
		const vector<string> vec_strategy_type{"Random" , "UCB1" , "EXP3", "Satisficing", "EGreedy", "NGreedy", "Softmax", "NoRegret", "FP", "QL", "BrFP", "Markov"}; 
		unique_ptr<Strategy> createNewStrategy(StrategyType type, int action_size) {

			if(type == StrategyType::Random) return make_unique<Strategy_Random>(action_size); 
			else if(type == StrategyType::UCB1) return make_unique<Strategy_UCB1>(action_size);
			else if(type ==  StrategyType::EXP3) return make_unique<Strategy_EXP3>(action_size);
			else if(type ==  StrategyType::Satisficing) return make_unique<Strategy_Satisficing>(action_size);
			else if(type ==  StrategyType::EGreedy) return make_unique<Strategy_EGreedy>(action_size);
			else if(type ==  StrategyType::NGreedy) return make_unique<Strategy_NGreedy>(action_size);
			else if(type ==  StrategyType::Softmax) return make_unique<Strategy_Softmax>(action_size);
			else if(type ==  StrategyType::NoRegret) return make_unique<Strategy_NoRegret>(action_size);
			else if(type ==  StrategyType::FP) return make_unique<Strategy_FP>(action_size);
			else if(type ==  StrategyType::QL) return make_unique<Strategy_QL>(action_size);
			else {
				LOG(ERROR) << "strategy type is not supported!" << endl;
				return nullptr;
			}
		}

		string getname(StrategyType type) const {
      unique_lock<mutex> m_mtx;
			int index = static_cast<int>(type);
			if(index < (int)vec_strategy_type.size())
				return vec_strategy_type[index];
			else
				return "Out of range of StrategyType";
		}

		void listAllStrategies() const {
      cout << "All available strategies: " << endl;
      for(size_t i = 0; i < vec_strategy_type.size(); i++)
        cout << vec_strategy_type[i] << " = " << i << " ; ";
      cout << endl;
    }

		const vector<string>& getTypeVector() const {return vec_strategy_type;}

		template<typename T>
		void printVec(vector<T> &v) const {
      string print{};
      for(auto e : v)
        print += to_string(e) + ", ";
      LOG(INFO) << print;
    }


} strategy_Mgr;
