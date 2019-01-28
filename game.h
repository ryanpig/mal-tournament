#include "player.h"
#include "config.h"
#include "IOHandler.h"
#include "gamut_parser.h"

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
		vector<Player*> m_players; // the number of players in a game
		vector<int> m_selected_actions; // actions selected by each players
		//Printing parameter
		int m_print_top;
		int m_print_last;
		// vector<int> vec_acc_regret;
		GameParser *m_game_parser;
		vector<float> getPayoffs(){return m_game_parser->queryByVec(m_selected_actions);};
		bool f_print;
		bool f_permute;
		StrategyType main_strategy;


		// constructor	
		Game(int gameid, uint rounds, uint num_of_players, int print_top, int print_last, GameParser &gp, int assign_strategy, bool f_print, bool f_permute, StrategyType str_type) : m_gameid(gameid), m_cur_round(0), m_print_top(3), m_print_last(1), f_print(f_print), f_permute(f_permute), main_strategy(str_type)
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
			for(int player_ind = 0; player_ind < m_num_of_players; player_ind++)
			{
				Player* p;
				if(player_ind==assign_strategy)
					p = new Player(main_strategy, player_ind, action_size[player_ind]); 
				else
					p = new Player(StrategyType::Random, player_ind, action_size[player_ind]); 
				m_players.push_back(p);
			}
		}
		// deconstructor
		~Game(){}

		void single_step();
		void run();
		float regret_cal(Player *p, vector<int> &select_actions);
			
		// print
		void print_player_info();
		void print_manual_payoff();
		void print_final_result();
		void dataToFile();
		int getGameID(){return m_gameid;};
};

