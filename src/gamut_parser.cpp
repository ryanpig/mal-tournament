#include <fstream>
#include <sstream>
#include "gamut_parser.h"
#include <vector>
#include <iostream>
#include <set>
#include <algorithm>
bool GameParser::parser(string filename)
{
	// file open
	std::ifstream infile(filename);	
	if(!infile.is_open())
	{
		std::cerr << filename << " can't be opened!" << std::endl;
		return false;
	}
	
	// parsing line by line, and save result to a vector
  const int maxActionSize{4};
  // const int maxElementCount{590491}; // 10240 (10 player 2 actions) , 590490 (10 players 3 actions) , 524288 (8 players 4 actions)
  const int maxElementCount{10485790}; // 10240 (10 player 2 actions) , 590490 (10 players 3 actions) , 524288 (8 players 4 actions)
	std::string line;
	bool start_flag = false;
	std::vector<float> parsed_vec;
	string find_action = "Actions:";
  LOG(INFO) << "pasring start";
  try
  {
    while(std::getline(infile, line))
    {
      // parsing action size
      size_t pos = line.find(find_action);
      if(pos != string::npos)
      {
        line.erase(pos, find_action.length()); 
        std::istringstream iss1(line);
        std::string token1;
        while(std::getline(iss1, token1,' '))
        {
          m_act_dim.push_back(std::stoi(token1.c_str()));	
          if(std::stoi(token1.c_str()) > maxActionSize){
            LOG(ERROR) << "action size > " << maxActionSize << " : " << token1.c_str();
            return false;
          }
        }
      }
      // parsing payoff data, save to a vector 
      if(start_flag)
      {
        std::istringstream iss(line);
        std::string token;
        char delim = ' ';
        int elementCount{0};
        while(std::getline(iss, token, delim))
        {
          if(elementCount++ > maxElementCount){
            LOG(ERROR) << "parsing elementCount " << elementCount << ">" << maxElementCount;
            return false;
          }
          parsed_vec.push_back(std::stof(token.c_str()));	
        }
        // std::cout << line << std::endl;
      }
      // if empty line, start to parse data
      if(line.size() == 0){
        start_flag = true;	
      }
    }
  } catch(exception &e) {
    LOG(ERROR) << "exception: " << e.what();
    return false;
  }

	// parsing result
  string concatenateStr;
	for(auto &e : m_act_dim)
		concatenateStr += to_string(e) + ",";
	LOG(INFO) << "Parsed action size:" << concatenateStr;

	// convert vector to matrix by traversing all rows of each player (size_reward: the size of elements that each player has)
	auto vecToMatrix = [](vector<float> &vec, vector<int> &act_dim){
		vector<vector<float>> mat;
		int size_rewards = multi(act_dim); 
		int cur{0};
		LOG(INFO) << "Total size:" << vec.size() << ", Plyaers:" << act_dim.size() << ", Each player:" << size_rewards;
		for(int i = 0; i < size_rewards ; i++)
		{
			// std::cout << "row number:" << i << "-- ";
			vector<float> tmp;
			for(size_t p = 0; p < act_dim.size(); p++)
			{
				tmp.push_back(vec[cur]);
				cur++;
				// std::cout << vec[cur] << ",";
			}
			mat.push_back(tmp);
			// std::cout << std::endl;
		};
			
		return mat;
	};
  // empty check for parsed vector 
  if(parsed_vec.size() == 0){LOG(ERROR) << "zero parsing size"; return false;}
	m_matrix = vecToMatrix(parsed_vec, m_act_dim);
  // 1. check cell number , 2. check each cell has the number of player 3. check cell number is not too much huge
	if((int)m_matrix.size() == multi(m_act_dim) && m_matrix[0].size() == m_act_dim.size() && m_matrix.size() <= maxElementCount) {
		m_index_max = multi(m_act_dim);
    // set max min reward;
    getMaxMinReward();
    LOG(INFO) << "Parsing succeeded!";
    return true;
	}else{
    LOG(ERROR) << "Parsing failed: " << m_matrix.size() <<  " != " << multi(m_act_dim) << " or " << m_matrix[0].size() << " != " << m_act_dim.size() << " or " <<  m_matrix.size() << " >= " << maxElementCount;
    return false;
	}
}

// query payoff vector by a given action vector
vector<float> GameParser::queryByVec(vector<int> &vec_query) const
{
	int ind = getIndex(vec_query);
	try
	{
		if(indexCheck(ind))
		{
			return m_matrix[ind];
		}
		else
		{
			printVec(vec_query);
			LOG(ERROR) << "ind:" << ind << " exceeds max " << m_index_max;
			// return m_matrix[0];
			throw ind;
		}
	}
		catch(const std::exception &e)
		{
			LOG(ERROR) << "ERROR:" << "Out of range of index" << " ," << e.what();
      return vector<float>{};
		}
	}

// return index by a given action vector
int GameParser::getIndex(vector<int> &vec_query) const{
		int index{0};
		for(size_t i = 0; i < m_act_dim.size(); i++)
		{
			int pop = vec_query[m_act_dim.size() - 1 - i];
			if(i != m_act_dim.size() - 1)
				index += pop * multi_range(m_act_dim, i + 1);
			else
				index += pop;
		}	
		return index;
}

//TODO: test different game sizes
void GameParser::selftest()
{
	cout << endl;
	cout << "Performing self test..." << endl;
	auto print_vec = [](vector<int> &vi, vector<float> &vf)
	{
		cout << " action: ";
		for(auto &e : vi) 
			cout << e << ",";
		cout << " payoffs: ";
		for(auto &e : vf) 
			cout << e << ",";
		cout << endl;
	};

	// Start to test 
	// sample test 
	vector<int> test1{2,1,0}; //expect = {95,39,12}
	// vector<int> test1{1,2,3,2}; // Expect: 81,67,10,5

	// show query result
	cout << "Test single vector:" << endl;
	int query_index = getIndex(test1);
	cout << "row:" << query_index << endl;
	print_vec(test1, m_matrix[query_index]);

	vector<float> expect{95,39,12};
	assert(queryByVec(test1) == expect);
	cout << "passed" << endl;

	// naive way to traverse all rows TODO:change to recursive code or other adaptive function
	cout << "Test traverse all rows:" << endl;
	int count ={0};
	set<int> set1;
	for(int i0 = 0; i0 < m_act_dim[0]; i0++){
		for(int i1 = 0; i1 < m_act_dim[1]; i1++){
			for(int i2 = 0; i2 < m_act_dim[2]; i2++){
				// for(int i3 = 0; i3 < m_act_dim[3]; i3++) {
					vector<int> v;
					v.push_back(i0);
					v.push_back(i1);
					v.push_back(i2);
					// v.push_back(i3);
					int ind = getIndex(v);
					cout << "row " << ind  << " :";
					print_vec(v, m_matrix[ind]); 
					count++;
					set1.insert(ind);
				// }
			}
		}
	}
	cout << "total:" << count << endl;
	cout << "set total:" << set1.size() << endl;
	assert(count == (int)m_matrix.size());
	assert(set1.size() == m_matrix.size());
	cout << "passed" << endl;
}
void GameParser::traverseMat() const
{
	//helper function
	auto print_vec = [](const vector<int> &vi)
	{
		for(auto &e : vi) 
			cout << e << ",";
		cout << endl;
	};

	auto print_vec2 = [](vector<int> &vi, vector<float> &vf)
	{
		cout << " action: ";
		for(auto &e : vi) 
			cout << e << ",";
		cout << " payoffs: ";
		for(auto &e : vf) 
			cout << e << ",";
		cout << endl;
	};
	// set element that postion is smaller than pos to zero
	auto resetZero = [](vector<int> &v, int pos){
		for(int p = 0; p < pos; p++){
			v[p] = 0;
		}
	};

	// input
	vector<int> cur(m_act_dim.size(), 0);
	vector<int> max = m_act_dim;
	for_each(max.begin(), max.end(), [](int &e){e--;});
	int epos = 0;	
	int count = 1;
	bool flag_plus{false};
	bool overflow{false};
	// start
	cout << "action size vector:";
 	print_vec(m_act_dim);
	vector<float> ini = queryByVec(cur);
	print_vec2(cur, ini);
	while(cur != max)
	{
		flag_plus = false;
		epos = 0;
		// each digit check
		while(!flag_plus){
			//if current digit is larger than its max value, the overflow occurrs.  
			if((cur[epos] + 1) <= max[epos])
			{
				// no overflow
				if(!overflow){
					cur[epos]++;
					flag_plus = true;
				}else{
					// overflow
					cur[epos]++;
					// set previous digits to 0;
					resetZero(cur,epos);
					overflow = false;
					flag_plus = true;
				}
			}else{
				//overflow
				overflow = true;
				epos++; // go to next digit
			}
		}
		vector<float> res = queryByVec(cur);
		print_vec2(cur, res);		
		count++;
	}

	cout << "Total items:" << count << endl;
}

void GameParser::getMaxMinReward(){
	// set element that postion is smaller than pos to zero
	auto resetZero = [](vector<int> &v, int pos){
		for(int p = 0; p < pos; p++){
			v[p] = 0;
		}
	};
	// input
	vector<int> cur(m_act_dim.size(), 0);
	vector<int> max = m_act_dim;
	for_each(max.begin(), max.end(), [](int &e){e--;});
	int epos = 0;	
	int count = 1;
	bool flag_plus{false};
	bool overflow{false};
  // initialize
  max_reward = 0;
  min_reward = 99999999;
	vector<float> ini = queryByVec(cur);
  float max_ini = *max_element(ini.begin(), ini.end());
  float min_ini = *min_element(ini.begin(), ini.end());
  if(max_ini > max_reward)
    max_reward = max_ini;
  if(min_ini < min_reward)
    min_reward = min_ini;
	// start
	while(cur != max)
	{
		flag_plus = false;
		epos = 0;
		while(!flag_plus){
			if((cur[epos] + 1) <= max[epos])
			{
				if(!overflow){
					cur[epos]++;
					flag_plus = true;
				}else{
					cur[epos]++;
					resetZero(cur,epos);
					overflow = false;
					flag_plus = true;
				}
			}else{
				overflow = true;
				epos++; 
			}
		}
		vector<float> res = queryByVec(cur);
    float max_res = *max_element(res.begin(), res.end());
    float min_res = *min_element(res.begin(), res.end());
    if(max_res > max_reward)
      max_reward = max_res;
    if(min_res < min_reward)
      min_reward = min_res;
		count++;
	}

	LOG(INFO) << "Total items:" << count;
  LOG(INFO) << "Maximum reward:" << max_reward << ", Minimum reward:" << min_reward;
  // checking
  if(max_reward == min_reward)
    LOG(ERROR) << "(Normalization check) " << "max_reward:" << max_reward << ", min_reward:" << min_reward;
  if(count >= (2<<20))
    LOG(ERROR) << "Total action action size is too huge!" << count;
}

void GameParser::normalize_reward(vector<float> &avg_payoff)
{
  for(size_t i = 0; i < avg_payoff.size(); i++)
    avg_payoff[i] = (avg_payoff[i] - min_reward) / (max_reward - min_reward); 
}

