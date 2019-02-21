#include "threading.h"

// client functions
void ThreadMgr::addTask(Task &t)
{
  m_deque.push_back(t);
}

void ThreadMgr::startJobServer()
{
  createThreads();
  for(auto &t : m_threadpool)
    t.join();
}

void ThreadMgr::createThreads()
{
  const int available_cores = thread::hardware_concurrency();
  for(int i = 0; i < available_cores; i++){
    m_threadpool.push_back(thread(&ThreadMgr::checkTask, this));
  }
  LOG(INFO) << "create " << available_cores << " worker thread!";
}


// worker thread
void ThreadMgr::checkTask()
{
  bool enable = true;
  while(enable)
  {
    unique_lock<mutex> lck(m_mtx_deque);
    if(m_deque.empty()){
      // finish the thread
      enable = false;
    }else{
      Task t = m_deque.back();
      m_deque.pop_back();
      lck.unlock();
      if(!doTask(t))
        LOG(ERROR) << "Do task failed";
    }
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

    // run 
    Game testgame(permuteid, set_rounds, set_players, 0, 0, gp, permuteid, false, s_type, opp_type);
    testgame.run();

    // result
    vector<float> tmp = testgame.getFinalResult();
    
    // write to DB
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

void ThreadMgr::saveToRecords(Record &record)
{
  unique_lock<mutex> lck(m_mtx_record);
  SQLMgr db_mgr(SQLITE_DB_PATH, "TESTTABLE");
  db_mgr.insertRecord(record);
}
