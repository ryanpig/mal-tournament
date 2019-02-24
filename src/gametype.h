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
  // No need to use random values for unset parameters 
  {"Chicken", 0, 0, false, false},
  {"CollaborationGame", 2, 0, true, false}, 
  {"RandomGame", 2, 2, true, true}, 
  {"MajorityVoting", 2, 2, true, true},
  {"RockPaperScissors", 2, 2, false, false}, 
  {"RandomZeroSum", 0, 2, false, true},
  {"RandomCompoundGame", 2, 0, true, false}, 
  {"PrisonersDilemma", 0, 0, false, false},
  {"DispersionGame", 2, 2, true, true},
  {"BattleOfTheSexes", 0, 0, false, false},
  {"MatchingPennies", 0, 0, false, false}, 
  {"HawkAndDove", 0, 0, false, false},
  {"GuessTwoThirdsAve", 2, 2, true, true}, 
  {"CovariantGame", 2, 2, true, true},
  {"GrabTheDollar", 0, 2, false, true},
  {"CoordinationGame", 2, 0, true, false}, 
  {"CoordinationGame", 2, 0, true, false}, 
  {"ShapleysGame", 0, 0, false, false},

  // Need to use random values for unset parameters
  {"BertrandOligopoly", 2, 2, true, true}, //cost_func
  {"BidirectionalLEG", 2, 2, true, true}, //graph class
  {"TravelersDilemma", 2, 2, true, true}, // need -reward
  // {"SimpleInspectionGame", 0, 0, false, false}, 
// -set_size:    number of elements in set S from which the players choose 
// elements.  Must be > 0 but <= 8 in order o keep the number of actions 
  {"LocationGame", 0, 2, false, true},
// -a:    distance between the location of player 1's store and his end of 
// the street.  Must fall between 0 and 1000.
//   {"PolymatrixGame", 2, 2, true, true}
// -graph:    the name of the graph structure class to use

  {"BertrandOligopoly", 2, 2, true, true},
// -cost_func:    the name of the function class to use for the cost 
// function.
  
  {"ArmsRace", 0, 2, false, true},
// -c_func:    the name of the function class to use for the cost function 
// C.  The function supplied should be SMOOTH in order to stick to the 
// strict definition of an Arms Race.
  {"GreedyGame", 0, 0, false, false}, 
// -set_size:    number of elements in set S from which the players choose 
// elements.  Must be > 0 and <= 8 for the sake of keeping the number of 
// actions reasonable.

  {"RandomGraphicalGame", 2, 2, true, true},
// -graph:    the name of the graph structure class to use
  {"TwoByTwoGame", 0, 0, false, false},
// -type:    type of the 2x2 game in Rappoport's classification, in [1,85]

  {"CournotDuopoly", 0, 2, false, true},
// -cost_func1:    the name of the function class to use for the cost 
// function for the first player.
//   {"NPlayerPrisonersDilemma", 2, 2, true, true}
// -players:    the number of players. Must be >= 2 and <= 100.
// -function_X:    X in payoff functions (see description).  Must be set 
// such that 0 < Z - Y < X and all parameters must be less than 100,000.
// -function_Y:    Y in payoff functions (see description).  Must be set 
// such that 0 < Z - Y < X and all parameters must be less than 100,000.
// -function_Z:    Z in payoff functions (see description).  Must be set 
// such that 0 < Z - Y < X and all parameters must be less than 100,000.

  // {"CongestionGame", 2, 0, true, false},
// -facilities:    number of facilities in set.  Since each player chooses a 
// subset of the facilities, the number of actions available to each 
// player is 2 to the number of facilities.  A maximum of five facilities is 
// allowed because of this extremely fast growth in matrix size.
  
  {"RandomLEG", 2, 2, true, true},
// -graph:    the name of the graph class to use
  
  {"UniformLEG", 2, 2, true, true},
// -graph:    the name of the graph class to use
  
  {"WarOfAttrition", 0, 2, false, true},
// -valuation_high:    upper bound on the players' valuations for the item. 
// Must be >= valuation_low.
//   {"NPlayerChicken", 2, 2, true, true},
// -cutoff:    the number of players who need to cooperate to get the 
// reward.  Must be > 0 and <= players.
  
  {"MinimumEffortGame", 2, 2, true, true}
// -a:    constant a used in formula a + bM - cE.  Should be between -100 
// and 100.
// -b:    coefficient b used in formula a + bM - cE.  Should be between 0 
// and 100.
// -c:    coefficient used in formula a + bM - cE.  Should be between 0 and 
// 100 but must be < b.
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
    // helper function
    void listAllGames() const {
      cout << endl;
      cout << "Available game types:" << endl;
      vector<const GameType*> tt,ft,tf,ff;
      // sorting
      for(const auto &g : vec_gametypes)
      {
        if(g.allow_more_players && g.allow_more_actions)
          tt.push_back(&g);
        else if(!g.allow_more_players && g.allow_more_actions)
          ft.push_back(&g);
        else if(g.allow_more_players && !g.allow_more_actions)
          tf.push_back(&g);
        else
          ff.push_back(&g);
      }
      // cout << "All available game types:" << endl;
      cout << "1. Extentable players && extentable actions:" << endl;
      for(auto p : tt) cout << p->name << ", ";
      cout << endl << endl;
      cout << "2. Extentable actions:" << endl;
      for(auto p : ft) cout << p->name << ", ";
      cout << endl << endl;
      cout << "3. Extentable players:" << endl;
      for(auto p : tf) cout << p->name << ", ";
      cout << endl << endl;
      cout << "4. No parameter needed:" << endl;
      for(auto p : ff) cout << p->name << ", ";
      cout << endl;

      // for(const auto &g : vec_gametypes)
      //   cout << g.name << ", ";
      // cout << endl;
      cout << "Total " << vec_gametypes.size() <<  " types" << endl;
      
    }

    bool typeCheck(string input_str)
    {
      for(const auto &g : vec_gametypes)
      {
        if(g.name == input_str)
          return true;
      }
      return false;
    }
};


