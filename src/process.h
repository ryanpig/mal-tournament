#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sys/stat.h>
#include "gametype.h"
#include "easylogging++.h"
#include <fstream>
#include <mutex>

#pragma once

static class Process_Mgr{
	public:
		std::string cmd_exec(std::string cmd); 
		bool file_exist(const std::string& name); 
    bool generation_check(string filename);
    bool generateGame(string fname, const GameType gt, int taskid); // main
    void selfTest();
    void listParamInfo() const;
  private:
    std::mutex m_mtx;
}process_Mgr;



