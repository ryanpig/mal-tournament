#include <memory>
#include <vector>
#include <fstream>
#include <iostream>
#include "easylogging++.h"

#pragma once
using namespace std;
// GameType
struct GameType
{
    string name;
    int players;
    int actions;
    // TODO: think if two bool variables are needed since we can make uses of values of players and actions as branch conditions
    bool allow_more_players;
    bool allow_more_actions;
};

using gameTypeVector = vector<GameType>;
// find exact parameter Info of specific game, "java -jar gamut.jar -helpgame RandomGame"
static const gameTypeVector vec_gametypes{
  // {"BertrandOligopoly", 2, 2, true, true}, //cost_func
  // {"BidirectionalLEG", 2, 2, true, true}, //graph class
  {"Chicken", 0, 0, false, false},
  {"CollaborationGame", 2, 0, true, false}, 
  {"RandomGame", 2, 2, true, true}

  // {"MajorityVotin", 2, 2, true, true},
  // {"TravelersDilemm", 2, 2, true, true},
  // {"RockPaperScissor", 2, 2, true, true},
  // {"SimpleInspectionGam", 2, 2, true, true},
  // {"LocationGam", 2, 2, true, true},
  // {"PolymatrixGam", 2, 2, true, true},
  // {"RandomZeroSu", 2, 2, true, true},
  // {"BertrandOligopol", 2, 2, true, true},
  // {"RandomCompoundGam", 2, 2, true, true},
  // {"ArmsRac", 2, 2, true, true},
  // {"PrisonersDilemm", 2, 2, true, true},
  // {"GreedyGam", 2, 2, true, true},
  // {"Chicke", 2, 2, true, true},
  // {"DispersionGam", 2, 2, true, true},
  // {"CollaborationGam", 2, 2, true, true},
  // {"RandomGraphicalGam", 2, 2, true, true},
  // {"TwoByTwoGam", 2, 2, true, true},
  // {"BattleOfTheSexe", 2, 2, true, true},
  // {"CournotDuopol", 2, 2, true, true},
  // {"NPlayerPrisonersDilemm", 2, 2, true, true},
  // {"BidirectionalLE", 2, 2, true, true},
  // {"CongestionGam", 2, 2, true, true},
  // {"MatchingPennie", 2, 2, true, true},
  // {"HawkAndDov", 2, 2, true, true},
  // {"RandomLE", 2, 2, true, true},
  // {"GuessTwoThirdsAv", 2, 2, true, true},
  // {"UniformLE", 2, 2, true, true},
  // {"CovariantGam", 2, 2, true, true},
  // {"GrabTheDolla", 2, 2, true, true},
  // {"WarOfAttritio", 2, 2, true, true},
  // {"NPlayerChicke", 2, 2, true, true},
  // {"CoordinationGam", 2, 2, true, true},
  // {"RandomGam", 2, 2, true, true},
  // {"MinimumEffortGam", 2, 2, true, true},
  // {"ShapleysGam", 2, 2, true, true}
};
// use singleton pattern
class GameTypeMgr
{

  private:
    // Define all types of game including user-defined types
    vector<string> vec_gamelist;

  public:
    GameTypeMgr(){
      initAvailableGames();
    }
    ~GameTypeMgr(){}
    const gameTypeVector& getCollection() const {return vec_gametypes;}

    const vector<string> & getGameLists() const {return vec_gamelist;}

    void initAvailableGames() ;
    void selftest();
};


