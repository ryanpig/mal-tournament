#include "threading.h"

/* producer side */
void ThreadMgr::addTask(Task &t)
{
  m_deque.push_back(t);
}

void ThreadMgr::createThreads()
{
  const int available_cores = thread::hardware_concurrency();
  for(int i = 0; i < available_cores; i++)
    m_threadpool.push_back(thread(&ThreadMgr::checkTask, this, i));
  LOG(INFO) << "create " << available_cores << " worker thread!";
}

void ThreadMgr::startJobServer()
{
  createThreads();
  thread t_monitor(&ThreadMgr::monitorThread, this);
  while(!checkStatus())
    this_thread::sleep_for(chrono::milliseconds(500));
  destroyThreads();
  t_monitor.join();
}

void ThreadMgr::monitorThread()
{
  LOG(INFO) << "create monitor thread!";
  while(!flag_finish_all_job)
  {
    unique_lock<mutex> lck(m_mtx_deque);
    if(!m_deque.empty()){
      LOG(INFO) << "Total available tasks:" << m_deque.size();
      cv.notify_all();
      this_thread::sleep_for(chrono::milliseconds(100));
    }
    else
    {
      flag_finish_all_job = true;
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
  while(true)
  {
    unique_lock<mutex> lck(m_mtx_deque);
    if(!m_deque.empty()){
      LOG(INFO) << "start to work, id:" << id;
      Task t = m_deque.back();
      doTask(t);
      m_deque.pop_back();
      cout << "I am worker " << id << ", and finish one task!" << endl;
      lck.unlock();
      cv.wait(lck);
    }
  }
}

void ThreadMgr::saveToRecords(Record &record)
{
  unique_lock<mutex> lck(m_mtx_record);
    vec_records.push_back(record);
}

void ThreadMgr::destroyThreads()
{
  for(auto &t : m_threadpool)
    t.join();
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
    LOG(ERROR) << "game generation failed";
    return false;
  }

  // parse matrix
  GameParser gp;
  if(!gp.parser(fname + ".game")){
    LOG(ERROR) << "parsing failed";
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
    // key section, write to shared resource
    // unique_lock<mutex> lck_vec_records(m_mtx_record);
    // {
    //   vec_records.push_back(r);
    // }
    // lck_vec_records.unlock();
  }
  return true;
}
