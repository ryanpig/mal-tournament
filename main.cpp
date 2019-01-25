#include <boost/program_options.hpp>
#include <chrono>
#include "main.h"
// #include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP


using namespace boost::program_options;

int main(int argc, char** argv)
{
	// logging library configuration
	logging_configuration();
	LOG(INFO) << "sss";
	LOG(ERROR) << "error!";

	// configuration variables
	int set_rounds = NUM_OF_ROUNDS; 
	int set_print_top = PRINT_TOP;
	int set_print_last = PRINT_LAST;
	int set_players = NUM_OF_PLAYERS;
	int set_actions = NUM_OF_ACTIONS;
	bool set_print_flag = true;
	bool set_permute = false;
	int set_main_strategy = 1;

	// command line parser
	namespace po = boost::program_options;
	options_description desc("Allowed options");
	desc.add_options()
	("help,h", "print usage message")
	("rounds,r", po::value<int>()->required(), "the number of rounds in a game")
	("actions,a", po::value<int>()->required(), "the number of actions for each player")
	("players,p", po::value<int>()->required(), "the number of players in a game")
	("print_top,t", po::value<int>()->required(), "print top n rounds info")
	("print_last,l", po::value<int>()->required(), "print last n rounds info")
	("print_flag,z", po::value<bool>()->required(), "true to print more info.") //TODO:change to logging module 
	("permute,y", po::value<bool>()->required(), "run permutation of payoffs.")
	("strategy,s", po::value<int>()->required(), "set main strategy for comparison, e.g. 0:random, 1:UCB1, 2:EXP3")
	;
	variables_map vm;


	// parsing arguments
	cout << "---Start to parse commandline arguments---" << endl;
	try
 	{
		store(parse_command_line(argc, argv, desc), vm);

		if (vm.count("help")) {  
			std::cout << desc << "\n";
				return 0;
		}
		if (vm.count("rounds"))
		  set_rounds = vm["rounds"].as<int>();
		if (vm.count("print_top"))
		  set_print_top = vm["print_top"].as<int>();
		if (vm.count("print_last"))
		  set_print_last = vm["print_last"].as<int>();
		if (vm.count("actions"))
		  set_actions = vm["actions"].as<int>();
		if (vm.count("players"))
		  set_players = vm["players"].as<int>();
		if (vm.count("print_flag"))
		  set_print_flag = vm["print_flag"].as<bool>();
		if (vm.count("permute"))
		  set_permute = vm["permute"].as<bool>();
		if (vm.count("actions"))
		  set_actions = vm["actions"].as<int>(); if (vm.count("strategy")) set_main_strategy = vm["strategy"].as<int>();

		cout << "CMD, rounds:" << set_rounds << ", actions:" << set_actions << ", players:" << set_players << endl;
		cout << "print flag:" << set_print_flag << ", permute flag:" << set_permute << endl; 
	}
	catch(exception& e) {
		std::cerr << e.what() << "\n";
	}
	cout << endl;

	// generate a new game from Gamut
	cout << "---Generate a game from Gamut ---" << endl;
	std::string fname = "RandNew1";
	process_Mgr.generateGame(fname, set_actions, set_players); // action size, players
	GameParser gp;
	if(!gp.parser(fname + ".game")){
		cout << "parsing failed" << endl;
		return -1;
	}
	cout << endl;

	// start a game
	// char a{0};
	int iterations{1};
	// allow to iterate player to use the single strategy to eliminate the bias
	if(set_permute) iterations = set_players; 
	// StrategyType s_type = StrategyType::UCB1;
	StrategyType s_type; 
	switch(set_main_strategy)
	{
		case 0: s_type = StrategyType::Random;break;
		case 1: s_type = StrategyType::UCB1;break;
		case 2: s_type = StrategyType::EXP3;break;
		default:s_type = StrategyType::Random;
	}
		
	cout << "---Game start---" << endl;
	for(int permuteid = 0; permuteid < iterations; permuteid++)
	{
		Game testgame(permuteid, set_rounds, set_players, set_print_top, set_print_last, gp, permuteid, set_print_top, set_permute, s_type);
		// wait to see basic information
		// while(a != 'y'){cout << "Please enter y to continue the game" << endl; cin >> a;}
		// run a single game;
		testgame.run();
		//statistic analysis
		// cout << "---Save data to the CSV file ---" << endl;
		testgame.dataToFile();
	}
}
