#include <algorithm> //max_element, transform
#include <vector>
#include <iostream>
#include <numeric> //accumulate
#include <math.h> // exp
#include <functional>
#include "easylogging++.h"

using namespace std;

template<typename T> //int, float
inline T sum(vector<T> &v){
	return accumulate(v.begin(), v.end(), 0.0f);
}

// return average of all elements in the array
template<typename T>
inline float avg(vector<T> &v){
	return sum(v) / (float)v.size();
}

// sum of two vectors
template <typename T>
inline std::vector<T> addTwoVectors(const std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());

	std::vector<T> result;
	result.reserve(a.size());

	std::transform(a.begin(), a.end(), b.begin(), 
	std::back_inserter(result), std::plus<T>());
	return result;
}

template<typename IterIn, typename IterOut = IterIn>
inline void softmax(IterIn beg, IterIn end, IterOut dest, bool normalizd, float temp)
{
		
	using VType = typename std::iterator_traits<IterIn>::value_type;
	// VType temperature = 1.0; // default
	// VType temperature = 0.4; // smaller temprature results in more exploitation
	// VType temperature = 10; // larger temprature results in more exploration 
	VType temperature = static_cast<VType>(temp);

	//find maximum and minimum value
  const auto max{*max_element(beg, end)};
  const auto min{*min_element(beg, end)};

	// normalization
	if(normalizd)
		transform(beg, end, dest, [&](VType val){
				VType a = (val - min) / (max - min);
				return a;});

	VType exptot{0}; 
	const auto max_normalized{*max_element(beg, end)}; //FIXED: using normaized maximum value rather than unnormalized one

	// convert value into exp(value)
	transform(beg, end, dest, [&](VType val){
		auto ex = exp((val - max_normalized) / temperature);
		exptot += ex; // sum of exp(value) -> Done inside of function
		return ex;
	});
	
	// division
	transform(beg, end, dest, [&](VType val){
		return val / exptot;});
	// cout << "sum:" << accumulate<IterIn, VType>(beg, end, 0.0) << endl;
}
