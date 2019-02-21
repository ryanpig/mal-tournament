#include "threading.h"

/* producer side */
void ThreadMgr::addTask(Task &t)
{
  m_deque.push_back(t);
}

void ThreadMgr::createThreads()
{
  const int available_cores = thread::hardware_concurrency();
  for(int i = 0; i < available_cores; i++){
    m_threadpool.push_back(thread(&ThreadMgr::checkTask, this, i));
    m_worker_flags.push_back(false);
  }
  LOG(INFO) << "create " << available_cores << " worker thread!";
}

void ThreadMgr::startJobServer()
{
  createThreads();
  thread t_monitor(&ThreadMgr::monitorThread, this);
  while(!checkStatus())
    this_thread::sleep_for(chrono::milliseconds(500));
}

void ThreadMgr::monitorThread()
{
  LOG(INFO) << "create monitor thread!";
  while(!flag_finish_all_job)
  {
    unique_lock<mutex> lck(m_mtx_deque);
    LOG(INFO) << "Total available tasks:" << m_deque.size();
    if(!m_deque.empty()){
      // cv.notify_all();
      lck.unlock();
      this_thread::sleep_for(chrono::milliseconds(1000));
    }
    else
    {
      for(bool a : m_worker_flags)
        cout << a << ",";
      cout << endl;

      if(all_of(m_worker_flags.begin(), m_worker_flags.end(), [](bool b){return b;}))
      {
        flag_finish_all_job = true;
      }
    }
  }
}

// This is for main thread to check if all tasks have been finished 
bool ThreadMgr::checkStatus()
{
  return flag_finish_all_job;
}


/* consumer side */
void ThreadMgr::checkTask(int id)
{
  // LOG(INFO) << "Let's start to work, id " << id;
  bool enable = true;
  while(enable)
  {
    unique_lock<mutex> lck(m_mtx_deque);
    while(m_deque.empty()){
      setFlag(id);
      enable = false;
      cv.wait(lck);
    }
    
    Task t = m_deque.back();
    m_deque.pop_back();
    lck.unlock();
    bool r = doTask(t);
  }
  cout << "finished";
}

void ThreadMgr::setFlag(int id)
{
  unique_lock<mutex> lck(m_mtx_flags);
  m_worker_flags[id] = true; 
}

void ThreadMgr::saveToRecords(Record &record)
{
  unique_lock<mutex> lck(m_mtx_record);
  SQLMgr db_mgr(SQLITE_DB_PATH, "TESTTABLE");
  db_mgr.insertRecord(record);
    // vec_records.push_back(record);
}

void ThreadMgr::destroyThreads()
{
  int count{0};
  for(auto &t : m_threadpool)
  {
    t.join();
    cout << count++ << endl; 
  }
}

bool ThreadMgr::doTask(Task& t)
{
  int set_players = t.set_players;
  int set_actions = t.set_actions;
  int set_rounds = t.set_rounds;
  int iterations = t.iterations;
  int i = t.i;
  int j = t.j;

  string tmp = to_string(std::rand());
	std::string fname = "AllGamesTournament_" + tmp;

  // generate game
  if(!process_Mgr.generateGame(fname, t.gt)){
    // LOG(ERROR) << "game generation failed";
    return false;
  }

  // parse matrix
  GameParser gp;
  if(!gp.parser(fname + ".game")){
    // LOG(ERROR) << "parsing failed";
    return false;
  }
  
  // play games w/ swapped players
  vector<float> sum_vec(set_players, 0.0);
  for(int permuteid = 0; permuteid < iterations; permuteid++)
  {
    StrategyType s_type  = static_cast<StrategyType>(i); 
    StrategyType opp_type  = static_cast<StrategyType>(j); 

    Game testgame(permuteid, set_rounds, set_players, 0, 0, gp, permuteid, false, s_type, opp_type);
    testgame.run();
    // result
    vector<float> tmp = testgame.getFinalResult();
    
    Record r{
      t.gt.name,
      permuteid, set_actions, set_players, set_rounds, 
      strategy_Mgr.getname(s_type), tmp[0],
      strategy_Mgr.getname(opp_type), tmp[1]
    };
    
    saveToRecords(r);
  }
  return true;
}
