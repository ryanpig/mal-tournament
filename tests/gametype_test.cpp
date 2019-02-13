// Goals: Test all types of game can be generated correctly
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

TEST(GenerateGames, Random) {
	ASSERT_TRUE(generateTest());
}


int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

