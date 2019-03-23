// Goals: Test all types of game can be generated correctly
// Guide:
//  Test case is supposed to be as small as possible.
//  Each test only evaluates a simple idea, which avoids crossed effect.
// Toolbox:
//  1. Using fixtures if tests share similar configuration. (TEST_F) 
//  2. Using SetUp(), TearDown() etc to initialize some variables
//  3. More -> Google Test Prime document

#include "process.h"
#include "gamut_parser.h"
#include "gametype.h"
#include <gtest/gtest.h>
// #include <iostream>
#include "common.h"
// #include <chrono>
#include "strategy.h"
#include <thread>
#include "game.h"

using namespace std;
INITIALIZE_EASYLOGGINGPP

bool generateTest()
{
	// generate a new game from Gamut
	std::string fname = "RandTest";
  Process_Mgr p;
  GameType gt{"RandomGame", 2, 2, true, true};
	p.generateGame(fname, gt, 999991); // action size, players
	GameParser gp;
	if(!gp.parser(fname + ".game")){
		cout << "parsing failed" << endl;
		return false;
	}
	cout << endl;
  return true;
}

class generategame: public ::testing::Test
{
    void SetUp() override 
    {
      gtm = make_unique<GameTypeMgr>(); 
    }
    void TearDown() override{;}
  public:
    std::unique_ptr<GameTypeMgr> gtm;
};

class multiplayer: public ::testing::Test
{
    void SetUp() override 
    {
      gtm = make_unique<GameTypeMgr>(); 
    }
    void TearDown() override{;}
  public:
    std::unique_ptr<GameTypeMgr> gtm;
};

TEST_F(generategame, listparaminfo){
  Process_Mgr p;
  p.listParamInfo();
}

TEST_F(generategame, anygame) {
  
  Process_Mgr p;
  // retrieve all game names from the list. 
  for(auto a : gtm->getCollection()){
    // GameType a{s, 2, 2, true, true};
    cout << "testing..." << a.name << endl;
    EXPECT_TRUE(p.generateGame("unittest", a, 999992));
  }
}


// repeat test the same game type
TEST_F(generategame, bogame) {
  GameType a{"BertrandOligopoly", 2, 2, true, true};
  Process_Mgr p;
  for(int i = 0; i < 20; i++)
  {
    cout << "Single game test:" << a.name << endl;
    EXPECT_TRUE(p.generateGame("unittest", a, 999993));
  }
}


// Need to specify more parameters to limit the game size, and make it normalizable
// TEST_F(generategame, greedygame) {
//   GameType a{"GreedyGame", 0, 0, false, false};
//   Process_Mgr p;
//   for(int i; i < 20; i++)
//   {
//     cout << "Single game test:" << a.name << endl;
//     EXPECT_TRUE(p.generateGame("unittest", a));
//   }
// }
//

TEST_F(multiplayer, allalgorithm2p) {
  // configuration
  const int taskid{999994}; 
  const int set_players{2};
  const int set_actions{2};
  const int set_steps{200};
  const bool set_permute{true}; 
  const string set_gametype{"RandomGame"};
	const bool set_print_flag{false};
  const int set_print_top{0};
  const int set_print_last{0};
	const int set_opp_strategy = 0;
	const int total_stratagies = 10; // strategy_Mgr.getTypeVector().size();
  const string fname_root{"unittest"};
  const string fname{fname_root + to_string(taskid)};
  int global_count{0};
  // game generation
  Process_Mgr p;
  GameType a{set_gametype, set_players, set_actions, true, true};
  EXPECT_TRUE(p.generateGame(fname, a, taskid));
  // game generation check
  string checkname = "check" + to_string(taskid) + ".out";
  int timeout{0};
  while(!process_Mgr.generation_check(checkname)){
    EXPECT_TRUE(timeout <=1000);
    timeout += 100;
    this_thread::sleep_for(chrono::milliseconds(100));
  }
  // game parsing
	GameParser gp;
	EXPECT_TRUE(gp.parser(fname + ".game"));
  // play game for each algorithm
  
  // start a game
  int iterations{1};
  // allow to iterate player to use the single strategy to eliminate the bias
  if(set_permute) iterations = set_players; 
    
  // run all algorithms v.s. random in a single game 
  for(int set_main_strategy = 0; set_main_strategy < total_stratagies; ++set_main_strategy)
  {
    StrategyType s_type  = static_cast<StrategyType>(set_main_strategy); 
    StrategyType opp_type  = static_cast<StrategyType>(set_opp_strategy);
    for(int permuteid = 0; permuteid < iterations; permuteid++)
    {
      Game testgame(permuteid, set_steps, set_players, set_print_top, set_print_last, gp, permuteid, set_print_flag, s_type, opp_type);
      testgame.run();
      vector<float> results = testgame.getFinalResult();
      for(auto& r : results)
      {
        EXPECT_TRUE(r > 0.0 && r <= 1.0);
      }
      global_count++;
    }
  }

  LOG(INFO) << "Toatl instances" << global_count << "Total strategies:" << total_stratagies;
  EXPECT_TRUE(global_count == iterations * total_stratagies);
}

TEST_F(multiplayer, allalgorithm3p) {
  // configuration
  const int taskid{999995}; 
  const int set_players{3};
  const int set_actions{2};
  const int set_steps{200};
  const bool set_permute{true}; 
  const string set_gametype{"RandomGame"};
	const bool set_print_flag{false};
  const int set_print_top{0};
  const int set_print_last{0};
	const int set_opp_strategy = 0;
	const int total_stratagies = 10; // strategy_Mgr.getTypeVector().size();
  const string fname_root{"unittest"};
  const string fname{fname_root + to_string(taskid)};
  int global_count{0};
  // game generation
  Process_Mgr p;
  GameType a{set_gametype, set_players, set_actions, true, true};
  EXPECT_TRUE(p.generateGame(fname, a, taskid));
  // game generation check
  string checkname = "check" + to_string(taskid) + ".out";
  int timeout{0};
  while(!process_Mgr.generation_check(checkname)){
    EXPECT_TRUE(timeout <=1000);
    timeout += 100;
    this_thread::sleep_for(chrono::milliseconds(100));
  }
  // game parsing
	GameParser gp;
	EXPECT_TRUE(gp.parser(fname + ".game"));
  // play game for each algorithm
  
  // start a game
  int iterations{1};
  // allow to iterate player to use the single strategy to eliminate the bias
  if(set_permute) iterations = set_players; 
    
  // run all algorithms v.s. random in a single game 
  for(int set_main_strategy = 0; set_main_strategy < total_stratagies; ++set_main_strategy)
  {
    StrategyType s_type  = static_cast<StrategyType>(set_main_strategy); 
    StrategyType opp_type  = static_cast<StrategyType>(set_opp_strategy);
    for(int permuteid = 0; permuteid < iterations; permuteid++)
    {
      Game testgame(permuteid, set_steps, set_players, set_print_top, set_print_last, gp, permuteid, set_print_flag, s_type, opp_type);
      testgame.run();
      vector<float> results = testgame.getFinalResult();
      for(auto& r : results)
      {
        EXPECT_TRUE(r > 0.0 && r <= 1.0);
      }
      global_count++;
    }
  }

  LOG(INFO) << "Toatl instances" << global_count << "Total strategies:" << total_stratagies;
  EXPECT_TRUE(global_count == iterations * total_stratagies);
}

TEST_F(multiplayer, allalgorithmnp) {
  // configuration
  const int taskid{999996}; 
  // const int set_players{4}; // the number of players until n
  const int set_actions{2};
  const int set_steps{200};
  const bool set_permute{true}; 
  const string set_gametype{"RandomGame"};
	const bool set_print_flag{false};
  const int set_print_top{0};
  const int set_print_last{0};
	const int set_opp_strategy = 0;
	const int total_stratagies = 10; // strategy_Mgr.getTypeVector().size();
  const string fname_root{"unittest"};
  const string fname{fname_root + to_string(taskid)};
  const vector<int> playerSet{4, 5, 6, 7, 8, 9, 10, 11}; // TODO:failed in 12 with the game generation failure. 1. remove normalization?
  for(auto set_players : playerSet)
  {
    int global_count{0};
    // game generation
    Process_Mgr p;
    GameType a{set_gametype, set_players, set_actions, true, true};
    EXPECT_TRUE(p.generateGame(fname, a, taskid));
    // game generation check
    string checkname = "check" + to_string(taskid) + ".out";
    int timeout{0};
    while(!process_Mgr.generation_check(checkname)){
      EXPECT_TRUE(timeout <=1000);
      timeout += 100;
      this_thread::sleep_for(chrono::milliseconds(100));
    }
    // game parsing
    GameParser gp;
    EXPECT_TRUE(gp.parser(fname + ".game"));
    // play game for each algorithm
    
    // start a game
    int iterations{1};
    // allow to iterate player to use the single strategy to eliminate the bias
    if(set_permute) iterations = set_players; 
      
    // run all algorithms v.s. random in a single game 
    for(int set_main_strategy = 0; set_main_strategy < total_stratagies; ++set_main_strategy)
    {
      StrategyType s_type  = static_cast<StrategyType>(set_main_strategy); 
      StrategyType opp_type  = static_cast<StrategyType>(set_opp_strategy);
      for(int permuteid = 0; permuteid < iterations; permuteid++)
      {
        Game testgame(permuteid, set_steps, set_players, set_print_top, set_print_last, gp, permuteid, set_print_flag, s_type, opp_type);
        testgame.run();
        vector<float> results = testgame.getFinalResult();
        for(auto& r : results)
        {
          EXPECT_TRUE(r > 0.0 && r <= 1.0);
        }
        global_count++;
      }
    }

    LOG(INFO) << "Toatl instances" << global_count << "Total strategies:" << total_stratagies;
    EXPECT_TRUE(global_count == iterations * total_stratagies );
  }

}

TEST_F(multiplayer, generategame) {
  // configuration
  int taskid{999997}; 
  // const int set_players{4}; // the number of players until n
  const int set_actions{2};
  const string fname_root{"unittest"};
  const vector<int> playerSet{3, 4, 5, 8, 10, 11}; // TODO:failed in 12 with the game generation failure. 1. remove normalization?
  // const vector<int> playerSet{8}; // TODO:failed in 12 with the game generation failure. 1. remove normalization?
  Process_Mgr p;
  int global_count{0};
  vector<string> error_summary;
  // retrieve all game names from the list. 
  for(auto set_players : playerSet)
  {
    for(auto gt : gtm->getCollection())
    {
      if(gt.allow_more_players)
      {
        if(gt.name == "CollaborationGame" || gt.name == "CoordinationGame") continue;
        string fname{fname_root + to_string(++taskid)};
        // game generation
        Process_Mgr p;
        GameType a = gt;
        a.players = set_players;
        a.actions = set_actions;
        GameParser *gp{nullptr};
        EXPECT_TRUE(p.generateGame(fname, a, taskid));
        // game generation check
        string checkname = "check" + to_string(taskid) + ".out";
        int timecount{0};
        bool r{false};
        // combine generation check and parsing check, because generation check may pass but the game file is not ready for use. 
        while((timecount++) <= 30){
          LOG(INFO) << "trying:" << timecount;
          if(process_Mgr.generation_check(checkname))
          {
            // game parsing
            auto gp_tmp = make_unique<GameParser>(); 
            if(gp_tmp->parser(fname + ".game"))
            {
              r = true;
              gp = gp_tmp.release();
              break;
            }
          }
          if(timecount == 25)
            error_summary.push_back(string(gt.name + "-" + to_string(set_players)));
          this_thread::sleep_for(chrono::milliseconds(100));
        }
        EXPECT_TRUE(r);
        EXPECT_TRUE(gp != nullptr);
        global_count++;
      }
    }
  }
  LOG(INFO) << "Toatl instances" << global_count; 
  for(auto &s : error_summary)
    LOG(INFO) << s;
}
int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

