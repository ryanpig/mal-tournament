#include <iostream>
#include <vector>
#include <numeric>

using namespace std;

class GameParser
{
	public:
		void parser(string filename);
		void selftest();
};

inline int multi(const vector<int> &v)
{
  return std::accumulate(v.begin(), v.end(), 1, multiplies<int>());
}

inline int multi_range(const vector<int> &v, int off)
{
  return std::accumulate(v.begin(), v.end() - off, 1, multiplies<int>());
}
