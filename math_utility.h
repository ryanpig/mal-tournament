#include <algorithm> //max_element, transform
#include <vector>
#include <iostream>
#include <numeric> //accumulate
#include <math.h> // exp

using namespace std;

template<typename T> //int, float
T sum(vector<T> &v){
	return accumulate(v.begin(), v.end(), 0.0f);
}

// return average of all elements in the array
template<typename T>
float avg(vector<T> &v){
	return sum(v) / (float)v.size();
}

template<typename IterIn, typename IterOut = IterIn>
void softmax(IterIn beg, IterIn end, IterOut dest, bool normalizd)
{
		
	using VType = typename std::iterator_traits<IterIn>::value_type;
	VType temperature = 1.0; // default
	// VType temperature = 0.4; // smaller temprature results in more exploitation
	// VType temperature = 10; // larger temprature results in more exploration 

	//find maximum and minimum value
	const auto max{*max_element(beg, end)};
	const auto min{*min_element(beg, end)};

	// normalization
	if(normalizd)
		transform(beg, end, dest, [&](VType val){return (val - min) / (max - min);});
	VType exptot{0}; 

	// convert value into exp(value)
	transform(beg, end, dest, [&](VType val){
		auto ex = exp((val - max) / temperature);
		exptot += ex; // sum of exp(value) -> Done inside of function
		return ex;
	});

	// division
	transform(beg, end, dest, [&](VType val){
		return val / exptot;});
	// cout << "sum:" << accumulate<IterIn, VType>(beg, end, 0.0) << endl;
}
