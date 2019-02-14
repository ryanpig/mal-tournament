#include "gametype.h"

using namespace std;
void GameTypeMgr::initAvailableGames() 
{
  ifstream f;
  f.open("gamelist.txt");
  if(!f.is_open())
    LOG(ERROR) << "game list file is not exist!";
  string line;
  while(getline(f, line))
  {
    vec_gamelist.push_back(line);
  }
}

void GameTypeMgr::selftest() 
{
  initAvailableGames();
}
