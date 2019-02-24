#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <chrono>
#include <algorithm>

#include "easylogging++.h"
#include "game.h"
#include "gametype.h"
#include "gamut_parser.h"
#include "sqlite.h"

using namespace std;
struct Task
{
  const GameType gt;
  int set_players;
  int set_actions;
  int set_rounds;
  int iterations;
  int i;
  int j;
};

class ThreadMgr 
{
  private:
    vector<thread> m_threadpool;
    deque<Task> m_deque;
    mutex m_mtx_deque, m_mtx_record;
    condition_variable cv;
    bool enable_multithreading;
    
  public:
    ThreadMgr(bool set_mt) : enable_multithreading(set_mt){;}
    // client function
    void addTask(Task &t);
    int totalTasks(){return m_deque.size();}

    void startJobServer();
    void createThreads();

    // worker thread
    void checkTask();
    bool doTask(Task& t);
    void saveToRecords(Record &record);
};
