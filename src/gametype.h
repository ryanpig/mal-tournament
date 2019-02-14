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
  {"MajorityVoting", 2, 2, true, true},
  {"TravelersDilemma", 2, 2, true, true},
  {"RockPaperScissors", 2, 2, true, true},
  // {"SimpleInspectionGame", 2, 2, true, true},
  // {"LocationGame", 2, 2, true, true},
  // {"PolymatrixGame", 2, 2, true, true},
  // {"RandomZeroSum", 2, 2, true, true},
  // {"BertrandOligopoly", 2, 2, true, true},
  // {"RandomCompoundGame", 2, 2, true, true},
  // {"ArmsRace", 2, 2, true, true},
  // {"PrisonersDilemma", 2, 2, true, true},
  // {"GreedyGame", 2, 2, true, true},
  // {"Chicken", 2, 2, true, true},
  // {"DispersionGame", 2, 2, true, true},
  // {"CollaborationGame", 2, 2, true, true},
  // {"RandomGraphicalGame", 2, 2, true, true},
  // {"TwoByTwoGame", 2, 2, true, true},
  // {"BattleOfTheSexes", 2, 2, true, true},
  // {"CournotDuopoly", 2, 2, true, true},
  // {"NPlayerPrisonersDilemma", 2, 2, true, true},
  // {"BidirectionalLEG", 2, 2, true, true},
  // {"CongestionGame", 2, 2, true, true},
  // {"MatchingPennies", 2, 2, true, true},
  // {"HawkAndDove", 2, 2, true, true},
  // {"RandomLEG", 2, 2, true, true},
  // {"GuessTwoThirdsAve", 2, 2, true, true},
  // {"UniformLEG", 2, 2, true, true},
  // {"CovariantGame", 2, 2, true, true},
  // {"GrabTheDollar", 2, 2, true, true},
  // {"WarOfAttrition", 2, 2, true, true},
  // {"NPlayerChicken", 2, 2, true, true},
  // {"CoordinationGame", 2, 2, true, true},
  // {"RandomGame", 2, 2, true, true},
  // {"MinimumEffortGame", 2, 2, true, true},
  // {"ShapleysGame", 2, 2, true, true}
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


