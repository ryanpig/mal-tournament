#include <fstream>
#include <sstream>
#include "gamut_parser.h"
#include <vector>
#include <iostream>
#include <set>
// #include "process.h"

int main()
{
	std::string fname = "Rand1"; // Players:3 , actions:{3,3,3}
	// std::string fname = "RandTest"; // players:4 , actions: {2,3,5,4}
	// process_Mgr.generateGame(fname, 3, 3);
  GameParser g;
	g.parser(fname + ".game");
}


void GameParser::parser(string filename)
{
	// file open
	std::ifstream infile(filename);	
	if(!infile.is_open())
		std::cerr << filename << " can't be opened!" << std::endl;
	
	// parsing line by line, and save result to a vector
	std::string line;
	bool start_flag = false;
	std::vector<float> parsed_vec;

	while(std::getline(infile, line))
	{
		if(start_flag)
		{
			std::istringstream iss(line);
			std::string token;
			char delim = ' ';
			while(std::getline(iss, token, delim))
			{
				parsed_vec.push_back(std::stof(token.c_str()));	
			}

			std::cout << line << std::endl;
		}
		if(line.size() == 0)
			start_flag = true;	
	}


	// 3 players , 3 actions for each one
	const vector<int> act{3,3,3}; //action
	// 4 players, various action number
	// const vector<int> act{2,3,5,4}; // action TODO:get from parsing
	// Debug

	// convert vector to matrix by traversing all rows of each player (size_reward: the size of elements that each player has)
	auto vecToMatrix = [](vector<float> &vec, const vector<int> &act_dim){
		vector<vector<float>> mat;
		int size_rewards = multi(act_dim); 
		int cur{0};
		std::cout << "total size:" << vec.size() << ", plyaers:" << act_dim.size() << ", each player:" << size_rewards << std::endl;
		for(int i = 0; i < size_rewards ; i++)
		{
			std::cout << "row number:" << i << "-- ";
			vector<float> tmp;
			for(int p =0; p < act_dim.size(); p++)
			{
				tmp.push_back(vec[cur]);
				std::cout << vec[cur++] << ",";
			}
			mat.push_back(tmp);
			std::cout << std::endl;
		};
		return mat;
	};

	auto matrix = vecToMatrix(parsed_vec, act);

	// calculation the corresponding query index
	auto funcQuery = [](const vector<int> &act_dim, vector<int> &vec_query){
		int index{0};
		for(int i = 0; i < act_dim.size(); i++)
		{
			int pop = vec_query[act_dim.size() - 1 - i];
			if(i != act_dim.size() - 1)
				index += pop * multi_range(act_dim, i + 1);
			else
				index += pop;
			// cout << "i:" << i << ", pop:" << pop << ", multi:" << multi_range(act, i + 1) << ", index:" << index << endl;
		}	
		return index;
	};
	auto print_vec_i = [](vector<int> &v){for(auto &e : v) cout << e << ","; cout << endl;};
	auto print_vec_f = [](vector<float> &v){for(auto &e : v) cout << e << ","; cout << endl;};

	// sample test 
	// vector<int> test1{1,1,1}; //expect = {72,7,8}
	vector<int> test1{2,1,0}; //expect = {95,39,12}
	// vector<int> test1{1,2,3,2}; // Expect: 81,67,10,5
	int query_index = funcQuery(act, test1);
	std::cout << std::endl;
	// show query result
	cout << "row:" << query_index << endl;
	print_vec_i(test1);
	print_vec_f(matrix[query_index]);

	// naive way to traverse all rows TODO:change to recursive code or other adaptive function
	int count ={0};
	set<int> set1;
	for(size_t i0 = 0; i0 < act[0]; i0++){
		for(size_t i1 = 0; i1 < act[1]; i1++){
			for(size_t i2 = 0; i2 < act[2]; i2++){
				// for(size_t i3 = 0; i3 < act[3]; i3++) {
					vector<int> v;
					v.push_back(i0);
					v.push_back(i1);
					v.push_back(i2);
					// v.push_back(i3);
					int ind = funcQuery(act, v);
					cout << "row " << ind  << " :";
					print_vec_i(v);
				  print_vec_f(matrix[ind]); 
					count++;
					set1.insert(ind);
				// }
			}
		}
	}
	cout << "total:" << count << endl;
	cout << "set total:" << set1.size() << endl;

}
//TODO: test different game sizes
void GameParser::selftest()
{
	
}

