#include <iostream>
#include <vector>
#include <numeric>
#include "process.h"
#include <cassert>
#include "easylogging++.h"

#pragma once
using namespace std;

class GameParser
{
	public:
		bool parser(string filename);
		void selftest();
		vector<vector<float>> copyMat(){return m_matrix;};
		vector<float> queryByVec(vector<int> &v) const;
		// void printByPlayer(int n) const;
		int getIndex(vector<int> &vec_query) const;
		void traverseMat() const;
		vector<int> getActionSize(){return m_act_dim;};
		bool indexCheck(int &ind) const{return (ind < m_index_max);};
		template<typename T>
		void printVec(vector<T> &v) const{for(auto &e : v) cout << e << " ,"; cout<<endl;};
    void normalize_reward(vector<float> &avg_payoff);
    void getMaxMinReward();
    void pringActionSize(){string total; for(auto e : m_act_dim) total += to_string(e) + ","; LOG(ERROR) << total;}
    float getMaxReward() const noexcept {return max_reward;} 
    float getMinReward() const noexcept {return min_reward;} 

	private:
		vector<vector<float>> m_matrix;
		vector<int> m_act_dim;
		int m_index_max;
    float max_reward;
    float min_reward;
};

inline int multi(const vector<int> &v)
{
  return std::accumulate(v.begin(), v.end(), 1, multiplies<int>());
}

inline int multi_range(const vector<int> &v, int off)
{
  return std::accumulate(v.begin(), v.end() - off, 1, multiplies<int>());
}
