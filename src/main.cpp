#include <boost/program_options.hpp>
#include <chrono>
#include "main.h"
#include "common.h"


using namespace boost::program_options;

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv)
{
	// logging library configuration
	logging_configuration();
	LOG(INFO) << "Logging start!";
	LOG(ERROR) << "Error Print Test!";

	// configuration variables
	int set_rounds = NUM_OF_ROUNDS; 
	int set_print_top = PRINT_TOP;
	int set_print_last = PRINT_LAST;
	int set_players = NUM_OF_PLAYERS;
	int set_actions = NUM_OF_ACTIONS;
	bool set_print_flag = true;
	bool set_permute = false;
	int set_main_strategy = 1;
	int set_opp_strategy = 0;
	bool set_tournament = false;

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
	("opp_strategy,e", po::value<int>()->required(), "set opponent strategy for comparison, in 2 player game")
	("tournament,o", po::value<bool>()->required(), "run tournament.")
	;
	variables_map vm;


	// parsing arguments
	LOG(INFO) << "---Start to parse commandline arguments---" << endl;
	try
 	{
		store(parse_command_line(argc, argv, desc), vm);

		if (vm.count("help")) {  
			std::cout << desc << "\n";
			strategy_Mgr.listAllStrategies();
			std::cout << std::endl;
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
		  set_actions = vm["actions"].as<int>();
	 	if (vm.count("strategy"))
		 	set_main_strategy = vm["strategy"].as<int>();
	 	if (vm.count("opp_strategy"))
		 	set_opp_strategy = vm["opp_strategy"].as<int>();
		if (vm.count("tournament"))
		  set_tournament = vm["tournament"].as<bool>();

		cout << "CMD, rounds:" << set_rounds << ", actions:" << set_actions << ", players:" << set_players << endl;
		cout << "print flag:" << set_print_flag << ", permute flag:" << set_permute << endl; 
		cout << "main strategy:" <<  strategy_Mgr.getname(static_cast<StrategyType>(set_main_strategy)) << endl;
		cout << "opponent strategy:" << strategy_Mgr.getname(static_cast<StrategyType>(set_opp_strategy)) << endl;
	}
	catch(exception& e) {
		std::cerr << e.what() << "\n";
	}
	cout << endl;
	
	if(set_tournament){
		GameGenerator gg;
		if(!gg.run_tournament(set_rounds))
			LOG(ERROR) << "tournament failed";
		else
			LOG(INFO) << "tournament finished";
		return 0;
	}

	// generate a new game from Gamut
	LOG(INFO) << "---Generate a game from Gamut ---" << endl;
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
	StrategyType s_type  = static_cast<StrategyType>(set_main_strategy); 
	// StrategyType opp_type  = StrategyType::Random;
	StrategyType opp_type  = static_cast<StrategyType>(set_opp_strategy);
		
	LOG(INFO) << "---Game start---" << endl;
	for(int permuteid = 0; permuteid < iterations; permuteid++)
	{
		Game testgame(permuteid, set_rounds, set_players, set_print_top, set_print_last, gp, permuteid, set_print_flag, s_type, opp_type);
		// wait to see basic information
		// while(a != 'y'){cout << "Please enter y to continue the game" << endl; cin >> a;}
		// run a single game;
		testgame.run();
		//statistic analysis
		// cout << "---Save data to the CSV file ---" << endl;
		testgame.dataToFile();
	}

	LOG(INFO) << "Program End!";
}


bool GameGenerator::run_tournament(int total_rounds)
{
	// access fo available strategies
	// generate all combinations (e.g Random v.s. Random, Random v.s. UCB1, Random v.s. EXP3 etc)
	// create a new game instance (including parsing)
	// play in the same instance w/ player permutation 
	//
	// configuration of each game
	int set_actions{2}, set_players{2}, set_rounds{total_rounds};
	int iterations{set_players};
	// size_t total_stratagies = strategy_Mgr.getTypeVector().size();
	size_t total_stratagies = 10; 
	std::string fname = "RandTournament";
	vector<float> result;
	// initializae the database connection
  auto db_mgr = std::move(SQLMgr::getInstance(SQLITE_DB_PATH, "TESTTABLE"));
	db_mgr->createTable();
	Strategy_Mgr *str_mgr = &strategy_Mgr;
	vector<Record> vec_records;
	// db_mgr->selfTest();
	// db_mgr->createTable();
	db_mgr->deleteTable();

	for(size_t i = 0; i < total_stratagies; i++)
	{
		for(size_t j = 0; j < total_stratagies; j++)
		{
			LOG(INFO) << "Game(i,j):" << i << ", " << j;
			// generate a new game
			process_Mgr.generateGame(fname, set_actions, set_players); // action size, players
			GameParser gp;
			if(!gp.parser(fname + ".game")){
				cout << "parsing failed" << endl;
				return -1;
			}
			// swap players
			vector<float> sum_vec(set_players, 0.0);
			for(int permuteid = 0; permuteid < iterations; permuteid++)
			{
				StrategyType s_type  = static_cast<StrategyType>(i); 
				StrategyType opp_type  = static_cast<StrategyType>(j); 

				Game testgame(permuteid, set_rounds, set_players, 0, 0, gp, permuteid, false, s_type, opp_type);
				testgame.run();
				vector<float> tmp = testgame.getFinalResult();
				Record r{permuteid, set_actions, set_players, set_rounds, 
					str_mgr->getname(s_type), tmp[0],
				 	str_mgr->getname(opp_type), tmp[1]};
				vec_records.push_back(r);
					
				// sum_vec = addTwoVectors(sum_vec, tmp);
				// testgame.dataToFile();
			}
			// transform(sum_vec.begin(), sum_vec.end(), sum_vec.begin(), [&](float v){ return v / iterations;});
		}
	}

	// Insert to database
	db_mgr->insertRecords(vec_records);
	db_mgr->queryAll();

	return true;
}
