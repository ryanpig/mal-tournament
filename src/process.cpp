#include "process.h"
#include <unistd.h>
#include <chrono>
#include <thread>

using namespace std;

bool Process_Mgr::generateGame(string fname, GameType &gt)
{
  // listParamInfo();
  cout << "generating games...." << endl;
  int final_actions = gt.actions;
  int final_players = gt.players;

  // find the GameType by its name
  unique_ptr<GameTypeMgr> gtm = make_unique<GameTypeMgr>();
  const auto& findtype = [&](const GameType &g){
    for(const auto &e : gtm->getCollection()){
      if(e.name == g.name)
         return e; }
    return GameType{"empty",0,0,false,false};
  };

  const GameType found = findtype(gt);
  if(found.name == "empty")
  {
    LOG(ERROR) << "can't find " << gt.name << " from the available game list";
    return false;
  }
  // set final values
  string game_flag;
	// action flag
  if(found.allow_more_actions){
    final_actions = found.actions;
    game_flag = " -actions " + std::to_string(final_actions);
  }
	// player flag
  if(found.allow_more_players){
    final_players = found.players;
    game_flag += " -players " + std::to_string(final_players);
  }
  
	string filename = fname;
	filename += ".game";
  string output_flag = " -f " + filename; 
	
	// clean up 
	bool r = process_Mgr.file_exist(filename);
	if(r)
		string res = process_Mgr.cmd_exec("rm " + filename);
	// flag setting
	string cmd = "java -jar gamut.jar -g " + gt.name;
	cmd += game_flag + " -normalize -min_payoff 1 -max_payoff 100 -int_payoffs -int_mult 1";
	cmd += output_flag + " -output GambitOutput";
  cmd += " >check.out 2>&1";

	// execute command 
	std::string result = process_Mgr.cmd_exec(cmd);
  return process_Mgr.generation_check();
}

bool Process_Mgr::generateGame(string fname, int actions, int players)
{
  // self test of available type of games
  selfTest();

	// filename flag
	string filename = fname;
	filename += ".game";
  string output_flag = " -f " + filename; 
	// action flag
	string game_flag = " -actions " + std::to_string(actions);
	// string game_flag = " -actions 2 3 5 4" ;
	// player flag
	game_flag += " -players " + std::to_string(players);
	
	// clean up 
	bool r = process_Mgr.file_exist(filename);
	if(r)
		string res = process_Mgr.cmd_exec("rm " + filename);
	// command w/ a file flag
	// string cmd = "java -jar gamut.jar -g RandomGame -actions 3 -players 2 -output GambitOutput -normalize -min_payoff 1 -max_payoff 100 -int_payoffs -int_mult 1";
	string cmd = "java -jar gamut.jar -g RandomGame";
	cmd += game_flag + " -normalize -min_payoff 1 -max_payoff 100 -int_payoffs -int_mult 1";
	cmd += output_flag + " -output GambitOutput";
  cmd += " >check.out 2>&1";
	// execute command 
	std::string result = process_Mgr.cmd_exec(cmd);
  return process_Mgr.generation_check();
}

std::string Process_Mgr::cmd_exec(std::string cmd) {
	std::cout << cmd << std::endl;
	std::array<char, 128> buffer;
	std::string result;
	// 3 ways to convert string into a const char* (1. str.data(), 2. str.c_str(), 3. &*str[0])
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.data(), "r"), pclose);
	if (!pipe) {
					throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
					result += buffer.data();
	}
	return result;
}

inline bool Process_Mgr::file_exist(const std::string& name) {
	struct stat buffer;   
	return (stat (name.c_str(), &buffer) == 0); 
}

inline bool Process_Mgr::generation_check(){
  // game generation check (Note: since Gamut only printed single line if it successfully generates)
  if(!process_Mgr.file_exist("check.out"))
    LOG(ERROR) << "check.out doesn't exist!";
	string result = process_Mgr.cmd_exec("wc -l < check.out");
  if(stoi(result) == 1){
    LOG(INFO) << "Game generation succeeded";
    return true;
  }else{
    LOG(ERROR) << "Game ganeration failed!";
    string cmd = "cat check.out >> error_report.txt";
    string res = process_Mgr.cmd_exec(cmd);
    return false;
  }
}

void Process_Mgr::selfTest(){
  auto gtm = make_unique<GameTypeMgr>();
  cout << "available type of games" << endl;
  for(auto &e : gtm->getCollection())
    cout << e.name << endl;
}


void Process_Mgr::listParamInfo() const{
  auto gtm = make_unique<GameTypeMgr>();
  for(const auto gt : gtm->getCollection()){
    ofstream f;
    f.open("paramInfo.txt", std::ofstream::app);
    f << "----------" << gt.name << "--------" << endl;
    f.close();
    string cmd2 = "java -jar gamut.jar -helpgame " + gt.name + " |tee -a paramInfo.txt";
    string res2 = process_Mgr.cmd_exec(cmd2);
  }
}
