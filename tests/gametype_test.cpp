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
#include <iostream>
#include "common.h"
using namespace std;
INITIALIZE_EASYLOGGINGPP

bool generateTest()
{
	// generate a new game from Gamut
	std::string fname = "RandTest";
  Process_Mgr p;
  GameType gt{"RandomGame", 2, 2, true, true};
	p.generateGame(fname, gt); // action size, players
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
    EXPECT_TRUE(p.generateGame("unittest", a));
  }
}

// repeat tet the same game type
TEST_F(generategame, bogame) {
  GameType a{"BertrandOligopoly", 2, 2, true, true};
  Process_Mgr p;
  for(int i; i < 20; i++)
  {
    cout << "Single game test:" << a.name << endl;
    EXPECT_TRUE(p.generateGame("unittest", a));
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
int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

