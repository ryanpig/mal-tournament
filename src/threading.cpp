#include "threading.h"

// client functions
void ThreadMgr::addTask(Task &t)
{
  m_deque.push_back(t);
}

void ThreadMgr::copyDeque()
{
  for( Task &t : m_deque)
  {
    m_deque_for_creation.push_front(t);
  }
  LOG(INFO) << "copy , the size of (m_deque, m_deque_for_creation):" << m_deque.size() << ", " << m_deque_for_creation.size();
}

void ThreadMgr::startJobServer()
{
  createThreads();
  for(auto &t : m_threadpool)
    t.join();
}

void ThreadMgr::createThreads()
{
  const int available_cores = enable_multithreading ? thread::hardware_concurrency() : 1;

  // game creation 
  vector<thread> threadpool_gamecreation;
  for(int i = 0; i < available_cores; i++){
    threadpool_gamecreation.push_back(thread(&ThreadMgr::checkTask_gamecreation, this));
  }
  for(auto &t : threadpool_gamecreation)
    t.join();

  // game running
  for(int i = 0; i < available_cores; i++){
    m_threadpool.push_back(thread(&ThreadMgr::checkTask, this));
  }
  LOG(INFO) << "create " << available_cores << " worker thread!";
}

// worker thread of game creation
void ThreadMgr::checkTask_gamecreation()
{
  bool enable = true;
  while(enable)
  {
    unique_lock<mutex> lck(m_mtx_deque_for_creation);
    if(m_deque_for_creation.empty()){
      // finish the thread
      enable = false;
      this_thread::sleep_for(chrono::seconds(1));
    }else{
      Task t = m_deque_for_creation.back();
      m_deque_for_creation.pop_back();
      lck.unlock();
      if(!doGameCreationTask(t))
        LOG(ERROR) << "Do game creation task failed";
    }
  }
}

bool ThreadMgr::doGameCreationTask(Task& t)
{
  int taskid = t.taskid;
	std::string fname = "AllGamesTournament_" + to_string(taskid);
  LOG(INFO) << fname;
  // run game creation
  if(!process_Mgr.generateGame(fname, t.gt, t.taskid)){
    LOG(ERROR) << t.gt.name << " generation failed";
    return false;
  }
  return true;
}


// worker thread of game running
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
  int taskid = t.taskid;

	std::string fname = "AllGamesTournament_" + to_string(taskid);

  //game generation check 
  string checkname = "check" + to_string(taskid) + ".out";
  if(!process_Mgr.generation_check(checkname)){
    LOG(ERROR) << t.gt.name << " generation failed";
    return false;
  }
  
  //parse game
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
