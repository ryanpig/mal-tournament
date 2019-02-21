#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <chrono>

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
    condition_variable cv, cv_monitor;
    vector<Record> vec_records;
    bool flag_finish_all_job; 

    
  public:
    ThreadMgr() : flag_finish_all_job(false){;}
    // client function
    void addTask(Task &t);
    vector<Record>& getResult(){return vec_records;}
    void startJobServer();

    void createThreads();
    void destroyThreads();

    // monitor thread
    void monitorThread();
    bool checkStatus();

    // worker thread
    void checkTask(int id);
    bool doTask(Task& t);
    void saveToRecords(Record &record);

};
