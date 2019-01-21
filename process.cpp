
#include "process.h"
#include <unistd.h>
#include <chrono>
#include <thread>

using namespace std;
/* for test */
/*
int main()
{
	process_Mgr.generateGame();
}
*/

void Process_Mgr::generateGame()
{
	// filename
	string filename = "Random1";
	filename += ".game";
	// clean up 
	bool r = process_Mgr.file_exist(filename);
	if(r)
		string res = process_Mgr.cmd_exec("rm " + filename);
	// command w/ a file flag
  string file_flag = " -f " + filename; 
	string cmd = "java -jar gamut.jar -g RandomGame -actions 3 -players 2 -output GambitOutput -normalize -min_payoff 1 -max_payoff 100 -int_payoffs -int_mult 1";
	cmd += file_flag;
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
