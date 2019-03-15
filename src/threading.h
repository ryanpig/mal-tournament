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
  int taskid;
};

class ThreadMgr 
{
  private:
    vector<thread> m_threadpool;
    deque<Task> m_deque;
    deque<Task> m_deque_for_creation;
    mutex m_mtx_deque, m_mtx_record;
    mutex m_mtx_deque_for_creation;
    mutex m_mtx_processMgr;
    condition_variable cv;
    bool enable_multithreading;
    
  public:
    ThreadMgr(bool set_mt) : enable_multithreading(set_mt){;}
    // client function
    void addTask(Task &t);
    int totalTasks(){return m_deque.size();}

    void startJobServer();
    void createThreads();

    // worker for game creation 
    void copyDeque();
    void checkTask_gamecreation();
    bool doGameCreationTask(Task& t);

    // worker for game running
    void checkTask();
    bool doTask(Task& t);
    void saveToRecords(Record &record);

};
