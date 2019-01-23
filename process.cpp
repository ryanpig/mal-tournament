
#include "process.h"
#include <unistd.h>
#include <chrono>
#include <thread>

using namespace std;
/* for test */
// int main()
// {
//   process_Mgr.generateGame("Rand2", 3, 4);
// }

void Process_Mgr::generateGame(string fname, int actions, int players)
{
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
	// execute command 
	std::string result = process_Mgr.cmd_exec(cmd);

	std::cout << "Return:" << result << std::endl;
	r = false;
	uint t_sleep = 1;
	while(!r) {
		r = process_Mgr.file_exist(filename);
		if(!r) {
			std::cout << "file:" << filename << " - " << r << std::endl;
		  std::this_thread::sleep_for (std::chrono::seconds(t_sleep));
		}
	}

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