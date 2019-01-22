#include <iostream>
#include <vector>
#include <numeric>
#include "process.h"

#pragma once
using namespace std;

class GameParser
{
	public:
		bool parser(string filename);
		void selftest();
		vector<vector<float>> copyMat(){return m_matrix;};
		vector<float> queryByVec(vector<int> &v);
		void printByPlayer(int n);
		int getIndex(vector<int> &vec_query);

	private:
		vector<vector<float>> m_matrix;
		vector<int> m_act_dim;

};

inline int multi(const vector<int> &v)
{
  return std::accumulate(v.begin(), v.end(), 1, multiplies<int>());
}

inline int multi_range(const vector<int> &v, int off)
{
  return std::accumulate(v.begin(), v.end() - off, 1, multiplies<int>());
}
