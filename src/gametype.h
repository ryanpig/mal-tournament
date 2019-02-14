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
  {"Bertrand Oligopoly", 0, 0, true, true}, 
  {"Bidirectional LEG", 0, 0, true, true},
  {"Chicken", 2, 2, false, false},
  {"Collaboration Game", 0, 2, true, false}, 
  {"RandomGame", 2, 2, true, true}
};
// use singleton pattern
class GameTypeMgr
{

  private:
    // for singleton
    struct _constructor_tag{ explicit _constructor_tag(){;}};
    // Define all types of game including user-defined types
    vector<string> vec_gamelist;

  public:
    static unique_ptr<GameTypeMgr> & getInstance()
    {
        static unique_ptr<GameTypeMgr> m_instance = make_unique<GameTypeMgr>(_constructor_tag());
        return m_instance;
    }
    // It only allows getInstance() to call constructor since it can access internal private member _constructor_tag
    GameTypeMgr(_constructor_tag){initAvailableGames();}
    ~GameTypeMgr(){}
    const gameTypeVector& getCollection() const {return vec_gametypes;}
    const vector<string> & getGameLists() const {return vec_gamelist;}

    void initAvailableGames() ;
    void selftest();
};


