#include <fstream>
#include <sstream>
#include "gamut_parser.h"
#include <vector>
#include <iostream>
// #include "process.h"

int main()
{
	// std::string fname = "Rand1"; // Players:3 , actions:{3,3,3}
	std::string fname = "Rand2"; // players:4 , actions: {2,3,5,4}
	// process_Mgr.generateGame(fname, 3, 3);
  GameParser g;
	g.parser(fname + ".game");
}


void GameParser::parser(string filename)
{
	std::ifstream infile(filename);	
	if(!infile.is_open())
		std::cerr << filename << " can't be opened!" << std::endl;
	
	std::string line;
	bool start_flag = false;
	std::vector<float> vec;

	while(std::getline(infile, line))
	{
		if(start_flag)
		{
			std::istringstream iss(line);
			std::string token;
			char delim = ' ';
			while(std::getline(iss, token, delim))
			{
				vec.push_back(std::stof(token.c_str()));	
			}

			std::cout << line << std::endl;
		}
		if(line.size() == 0)
			start_flag = true;	
		
	}


	// 3 players , 3 actions for each one
	// vector<int> act{3,3,3} //action
	// vector<int> test1{1,1,1}; //expect = {72,7,8}
	// vector<int> test2{2,1,0}; //expect = {95,39,12}
	// 4 players, various action number
	const vector<int> act{2,3,5,4}; // action
	// Debug
	int cur{0};
	int size_rewards = multi(act);
	std::cout << "total size:" << vec.size() << ", plyaers:" << act.size() << ", each player:" << size_rewards << std::endl;
	vector<vector<int>> matrix;

	// loop players, loop their rewards
	// for(int p = 0; p < act.size(); p++){ 
	//   cur = p;
	//   std::cout << "player:" << p << std::endl;
		for(int i = 0; i < size_rewards ; i++)
		{
			std::cout << "row number:" << i << "-- ";
			vector<int> tmp;
			for(int p =0; p < act.size(); p++)
			{
				tmp.push_back(vec[cur]);
				std::cout << vec[cur++] << ",";
			}
			matrix.push_back(tmp);
			// if(i !=0 && i % size_rewards == 0)
			std::cout << std::endl;
			// std::cout << vec[cur] << "," ;
			// cur += step;
		}
		
		//calculation
		vector<int> test1{1,2,3,3}; // Expect: 81,67,10,5
		int sum{0};
		for(int i = 0; i < act.size(); i++)
		{
			int pop = test1[test1.size() - 1 - i];
			if(i != act.size() - 1)
				sum += (pop)* multi_range(act, i + 1);
			else
				sum += (pop);
		}	
		
		std::cout << std::endl;
		int query = sum;	
		cout << "row:" << query << endl;
		for(auto &e : test1)
			cout << e << ",";
		cout << endl;
		for(auto &e : matrix[query])
			cout <<  e << ",";
		cout << endl;
	// }

	//query
}
	
