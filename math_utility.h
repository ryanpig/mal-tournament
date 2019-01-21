#include <algorithm>
using namespace std;

template<typename T> //int, float
T sum(vector<T> &v){
	return accumulate(v.begin(), v.end(), 0);
}

// return average of all elements in the array
template<typename T>
float avg(vector<T> &v){
	return sum(v) / (float)v.size();
}

