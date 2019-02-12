#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sys/stat.h>

static class Process_Mgr{
	public:
		std::string cmd_exec(std::string cmd); 
		bool file_exist(const std::string& name); 
		void generateGame(std::string fname, int actions, int players);
}process_Mgr;


