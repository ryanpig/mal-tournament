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
	p.generateGame(fname, 2, 2); // action size, players
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



TEST_F(generategame, Random) {
	// ASSERT_TRUE(generateTest());
}

TEST_F(generategame, anygame) {
  
  Process_Mgr p;
  // retrieve all game names from the list. 
  for(auto s : gtm->getGameLists()){
    GameType a{s, 2, 2, true, true};
    cout << s << endl;
    EXPECT_TRUE(p.generateGame("unittest", a));
  }
}


int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

