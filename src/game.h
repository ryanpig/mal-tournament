#include "player.h"
#include "config.h"
#include "IOHandler.h"
#include "gamut_parser.h"
#include <memory>

#pragma once
//temporary payoff matrix for testing 
static const int mat_payoffs[NUM_OF_PLAYERS * NUM_OF_ACTIONS][NUM_OF_PLAYERS] = {{3,3}, {5,0}, {0,5}, {1,1}};

class Game
{
	public:
		int m_gameid;
		int m_rounds; // the number of rounds in a game
		int m_num_of_players;
		int m_cur_round;
		vector<unique_ptr<Player>> m_players; // the number of players in a game
		vector<int> m_selected_actions; // actions selected by each players
		//Printing parameter
		int m_print_top;
		int m_print_last;
		// vector<int> vec_acc_regret;
		GameParser *m_game_parser;
		vector<float> getPayoffs(){return m_game_parser->queryByVec(m_selected_actions);};
		bool f_print;
		StrategyType main_strategy;
		StrategyType opp_strategy;
		vector<float> m_avg_result;


		// constructor	
		Game(int gameid, uint rounds, uint num_of_players, int print_top, int print_last, GameParser &gp, int assign_strategy, bool f_print, StrategyType main, StrategyType opp) : m_gameid(gameid), m_cur_round(0), m_print_top(3), m_print_last(1), f_print(f_print), main_strategy(main), opp_strategy(opp) 
	{	
			// initialization
			m_rounds = rounds;
			m_num_of_players = num_of_players;
			m_print_top = print_top;
			m_print_last = print_last;
			m_game_parser = &gp;

			// print basic inforamtion of the game
			if(f_print){
				cout << "Total players: " << m_num_of_players << ", Total rounds: " << m_rounds << ", Action size:" << NUM_OF_ACTIONS << endl;
				cout << "-- Payoff Matrix --" << endl;
				m_game_parser->traverseMat();
			}
			// print_manual_payoff();

			// create n players & set its strategy  
			vector<int> action_size = m_game_parser->getActionSize();
			vector<Info*> vec_ptr_infos;
			for(int player_ind = 0; player_ind < m_num_of_players; player_ind++)
			{
        // std::unique_ptr<Player> p;
				if(player_ind==assign_strategy)
        {
          m_players.push_back(make_unique<Player>(main_strategy, player_ind, action_size[player_ind], m_game_parser)); 
        }
				else
        {
          m_players.push_back(make_unique<Player>(opp_strategy, player_ind, action_size[player_ind], m_game_parser)); 
        }
			}
			for(auto &p: m_players)
				vec_ptr_infos.push_back(&p->m_info);

			// initialize Info for each player
			for(auto &p : m_players)
				p->m_info.opp_history = vec_ptr_infos;
		}
		// deconstructor
		~Game()
    {
      // m_players.clear(); 
      // m_game_parser = nullptr;
    
    }

		void single_step();
		void run();
		float regret_cal(unique_ptr<Player> &p, vector<int> &select_actions);
		void update_hypo_rewards_by_action(unique_ptr<Player> &p, vector<int> &select_actions);
			
		// print
		void print_player_info() const;
		void print_manual_payoff() const;
		void print_final_result() ; // save final result in m_avg_result
		void dataToFile() const;
		int getGameID() const {return m_gameid;};
		const vector<float>& getFinalResult() const {return m_avg_result;};
};

