#include <boost/program_options.hpp>
#include <chrono>
#include "main.h"
#include "common.h"
#define ELPP_THREAD_SAFE 1


using namespace boost::program_options;

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv)
{
	// logging library configuration
	logging_configuration();
	LOG(INFO) << "Logging start!";

	// configuration variables
	int set_rounds = NUM_OF_ROUNDS; 
	int set_print_top = PRINT_TOP;
	int set_print_last = PRINT_LAST;
	int set_players = NUM_OF_PLAYERS;
	int set_actions = NUM_OF_ACTIONS;
  string set_gametype = "RandomGame";
	bool set_print_flag = true;
	bool set_permute = false;
	int set_main_strategy = 0;
	int set_opp_strategy = 1;
	bool set_tournament = false;
	bool set_tournament_all_games = false;
	bool set_mt = true;

	// command line parser
	namespace po = boost::program_options;
	options_description desc("Allowed options");
	desc.add_options()
	("help,h", "print usage message")
	("gametype,g", po::value<string>()->required(), "the game type for normal single game (Default:RandomGame)")
	("players,p", po::value<int>()->required(), "the number of players in a game (Default:2)")
	("actions,a", po::value<int>()->required(), "the number of actions for each player (Default:2)")
	("rounds,r", po::value<int>()->required(), "the number of rounds in a game (Default:10000)")
	("strategy,s", po::value<int>()->required(), "set main strategy in a single game for comparison (check available strategies)  (Default:0)")
	("opp_strategy,e", po::value<int>()->required(), "set opponent strategy for comparison, in 2 player game (check available strategies) (Default:1)")
	("print_top,t", po::value<int>()->required(), "print top n rounds info (Default:3)")
	("print_last,l", po::value<int>()->required(), "print last n rounds info (Default:1)")
	("print_flag,z", po::value<bool>()->required(), "true to print more info. (Default:true)") 
	("permute,y", po::value<bool>()->required(), "run permutation of payoffs. (Default:false)")
	("tournament,o", po::value<bool>()->required(), "run tournament w/ single game in all algorithm pairs. (Default:false)")
	("tournament_all_games,q", po::value<bool>()->required(), "run tournament w/ all game types and all algorithms. (Default:false)")
	("enable_multithreading,m", po::value<bool>()->required(), "enable enable_multithreading. (Default:true)")
	;
	variables_map vm;


	// parsing arguments
	LOG(INFO) << "---Start to parse commandline arguments---" << endl;
	try
 	{
		store(parse_command_line(argc, argv, desc), vm);

		if (vm.count("help")) {  
			cout << desc << "\n";
			strategy_Mgr.listAllStrategies();
      auto gt_mgr = make_unique<GameTypeMgr>();
      gt_mgr->listAllGames();
			std::cout << std::endl;
				return 0;
		}

		if (vm.count("print_top"))
		  set_print_top = vm["print_top"].as<int>();
		if (vm.count("print_last"))
		  set_print_last = vm["print_last"].as<int>();
		if (vm.count("print_flag"))
		  set_print_flag = vm["print_flag"].as<bool>();

		if (vm.count("gametype")){
		  set_gametype = vm["gametype"].as<string>();
      auto gt_mgr = make_unique<GameTypeMgr>();
      if(!gt_mgr->typeCheck(set_gametype)){
        LOG(ERROR) << "Invalid game type:" << set_gametype << ", please check available types below";
        gt_mgr->listAllGames();
        return -1;
      }
    }
		if (vm.count("rounds"))
		  set_rounds = vm["rounds"].as<int>();
		if (vm.count("actions"))
		  set_actions = vm["actions"].as<int>();
		if (vm.count("players"))
		  set_players = vm["players"].as<int>();
		if (vm.count("permute"))
		  set_permute = vm["permute"].as<bool>();
	 	if (vm.count("strategy"))
		 	set_main_strategy = vm["strategy"].as<int>();
	 	if (vm.count("opp_strategy"))
		 	set_opp_strategy = vm["opp_strategy"].as<int>();

		if (vm.count("tournament")){
		  set_tournament = vm["tournament"].as<bool>();
      LOG(INFO) << " Set Tournament Mode. (Run all algorithm pairs)";
    }
		if (vm.count("tournament_all_games")){
		  set_tournament_all_games = vm["tournament_all_games"].as<bool>();
      LOG(INFO) << " Set Tournament All Game Mode. (Run all available games)";
    }

		LOG(INFO) << "CMD, rounds:" << set_rounds << ", actions:" << set_actions << ", players:" << set_players;
    LOG(INFO) << "Game type:" << set_gametype;
		LOG(INFO) << "print flag:" << set_print_flag << ", permute flag:" << set_permute; 
		LOG(INFO) << "main strategy:" <<  strategy_Mgr.getname(static_cast<StrategyType>(set_main_strategy));
		LOG(INFO) << "opponent strategy:" << strategy_Mgr.getname(static_cast<StrategyType>(set_opp_strategy));
	}
	catch(exception& e) {
		std::cerr << e.what() << "\n";
	}
	
  // --------------- Tournament Mode w/ all algorithm pairs --------------
	if(set_tournament){
    LOG(INFO) << "---Tournament Mode w/ all algorithm pairs--- ";
		GameGenerator gg;
    GameType gt{set_gametype, set_players, set_actions, true, true};
		if(!gg.run_tournament(set_rounds, gt))
			LOG(ERROR) << "tournament failed";
		else
			LOG(INFO) << "tournament finished";
    cleanTempFiles();
		return -1;
	}
  // --------------- Tournament All Game Mode w/ all game types--------------
	if(set_tournament_all_games){
    LOG(INFO) << "---Tournament All Game Mode w/ all game types---";
		GameGenerator gg;
		// if(!gg.run_all_games(set_rounds))
		if(!gg.run_all_games_mt(set_rounds, set_mt))
			LOG(ERROR) << "tournament failed";
		else
			LOG(INFO) << "tournament finished";
    cleanTempFiles();
		return -1;
	}
 
  // --------------- Normal single game mode w/ single algorithm pair --------------
  if(!set_tournament && !set_tournament_all_games)
  {
    LOG(INFO) << "---Normal single game mode w/ single algorithm pair---";
    LOG(INFO) << "---Generate a game from Gamut ---" << endl;
    std::string fname = "RandNew1";

    // generate a new game from Gamut
    GameType a{set_gametype, set_players, set_actions, true, true};

    if(!process_Mgr.generateGame(fname, a)){
      LOG(ERROR) << "game genearation failed";
      return -1;
    }

    // parse the game 
    GameParser gp;
    if(!gp.parser(fname + ".game")){
      LOG(ERROR) << "parsing failed" << endl;
      return -1;
    }
    // start a game
    int iterations{1};
    // allow to iterate player to use the single strategy to eliminate the bias
    if(set_permute) iterations = set_players; 
    StrategyType s_type  = static_cast<StrategyType>(set_main_strategy); 
    StrategyType opp_type  = static_cast<StrategyType>(set_opp_strategy);
      
    LOG(INFO) << "---Game start---" << endl;
    for(int permuteid = 0; permuteid < iterations; permuteid++)
    {
      // initialization
      Game testgame(permuteid, set_rounds, set_players, set_print_top, set_print_last, gp, permuteid, set_print_flag, s_type, opp_type);
      // run
      testgame.run();
      // write result to db
      testgame.dataToFile();
    }
    LOG(INFO) << "single Game finished";
    cleanTempFiles();
    return -1;
  }
}

void cleanTempFiles()
{
  string cmd = "rm *.out && rm *.game";
  string res = process_Mgr.cmd_exec(cmd);
}


bool GameGenerator::run_tournament(int total_iterations, GameType &gt)
{
	// access fo available strategies
	// generate all combinations (e.g Random v.s. Random, Random v.s. UCB1, Random v.s. EXP3 etc)
	// create a new game instance (including parsing)
	// play in the same instance w/ player permutation 
	//
	// configuration of each game
	int set_actions = gt.actions, set_players = gt.players, set_rounds{total_iterations};
  string set_gametype = gt.name;
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
  db_mgr->createTable();
	db_mgr->deleteTable();
  int total_instatnces{0};

	for(size_t i = 0; i < total_stratagies; i++)
	{
		for(size_t j = 0; j < total_stratagies; j++)
		{
			LOG(INFO) << "Game(i,j):" << i << ", " << j;
			// generate a new game
			// if(!process_Mgr.generateGame(fname, set_actions, set_players)){
			if(!process_Mgr.generateGame(fname, gt)){
        LOG(ERROR) << "game generation failed";
        return -1;
      }
			GameParser gp;
			if(!gp.parser(fname + ".game")){
				LOG(ERROR) << "parsing failed";
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
				Record r{
          set_gametype, 
          permuteid, set_actions, set_players, set_rounds, 
          str_mgr->getname(s_type), tmp[0],
          str_mgr->getname(opp_type), tmp[1]
        };
				vec_records.push_back(r);
					
        total_instatnces++; 
			}
		}
	}

	// Insert to database
	db_mgr->insertRecords(vec_records);
	db_mgr->queryAll();
  LOG(INFO) << "Total play " << total_instatnces << " instances in the tournamenet";
	return true;
}

bool GameGenerator::run_all_games(int total_iterations)
{

	// configuration of each game
	int set_actions{2}, set_players{2}, set_rounds{total_iterations};
	int iterations{set_players};
	// size_t total_stratagies = strategy_Mgr.getTypeVector().size();
	size_t total_stratagies = 10; 
	std::string fname = "AllGamesTournament";
	// initializae the database connection
  auto db_mgr = std::move(SQLMgr::getInstance(SQLITE_DB_PATH, "TESTTABLE"));
	db_mgr->createTable();
	Strategy_Mgr *str_mgr = &strategy_Mgr;
	vector<Record> vec_records;
  // db_mgr->createTable();
	db_mgr->deleteTable();
  // retrieve all available games
  auto gt_mgr = make_unique<GameTypeMgr>();
  int total_instatnces{0};

	for(size_t i = 0; i < total_stratagies; i++)
	{
		for(size_t j = 0; j < total_stratagies; j++)
		{
			LOG(INFO) << "Game(i,j):" << i << ", " << j;
      // loop games
      for(auto gt : gt_mgr->getCollection())
      {
        // generate game
        if(!process_Mgr.generateGame(fname, gt)){
          LOG(ERROR) << "game generation failed";
          return false;
        }
        // parse matrix
        GameParser gp;
        if(!gp.parser(fname + ".game")){
          LOG(ERROR) << "parsing failed";
          return false;
        }
        // play games w/ swapped players
        vector<float> sum_vec(set_players, 0.0);
        for(int permuteid = 0; permuteid < iterations; permuteid++)
        {
          StrategyType s_type  = static_cast<StrategyType>(i); 
          StrategyType opp_type  = static_cast<StrategyType>(j); 

          Game testgame(permuteid, set_rounds, set_players, 0, 0, gp, permuteid, false, s_type, opp_type);
          testgame.run();
          // result
          vector<float> tmp = testgame.getFinalResult();
          Record r{
            gt.name,
            permuteid, set_actions, set_players, set_rounds, 
            str_mgr->getname(s_type), tmp[0],
            str_mgr->getname(opp_type), tmp[1]
          };
          vec_records.push_back(r);
          
          total_instatnces++; 
        }
      }
		}
	}

	// Insert to database
	db_mgr->insertRecords(vec_records);
	db_mgr->queryAll();
  LOG(INFO) << "Total play " << total_instatnces << " instances in the tournamenet";

	return true;
}

bool GameGenerator::run_all_games_mt(int total_iterations, bool set_mt)
{

	// configuration of each game
	int set_actions{2}, set_players{2}, set_rounds{total_iterations};
	int iterations{set_players};
	// size_t total_stratagies = strategy_Mgr.getTypeVector().size();
	int total_stratagies = 10; 
	// initializae the database connection
  SQLMgr db_mgr(SQLITE_DB_PATH, "TESTTABLE");
  // clean data
  db_mgr.createTable();
  db_mgr.deleteTable();
  // retrieve all available games
  auto gt_mgr = make_unique<GameTypeMgr>();
  int total_instatnces{0};

  ThreadMgr tmgr(set_mt);

  for(int i = 0; i < total_stratagies; i++)
	{
		for(int j = 0; j < total_stratagies; j++)
		{
			LOG(INFO) << "Game(i,j):" << i << ", " << j;
      // loop games
      for(const auto gt : gt_mgr->getCollection())
      {
        Task task{gt, set_players, set_actions, set_rounds, iterations, i, j};
        tmgr.addTask(task);
        total_instatnces += 2; 
      }
		}
	}
  
  // start the job server
  LOG(INFO) << "start job server...";
  LOG(INFO) << "total tasks:" << tmgr.totalTasks();
  tmgr.startJobServer();

	// Insert to database
  db_mgr.queryAll();
  LOG(INFO) << "Total play " << total_instatnces << " instances in the tournamenet";

	return true;
}

// vim: set sw=2 ts=2 noet:
