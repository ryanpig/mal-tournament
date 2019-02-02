#include <iostream>
#include <fstream>
#include <cassert>

static class IOHandler
{
	public:
	  ofstream of;	
		template<typename T>
		void writeToCSV(vector<T> &vec, string filename){
			of.open(filename);
			for(T &e : vec)	
			{
				of <<  e << endl;	
			}
			of.close();
		}

		template<typename T>
		void writeTwoVectorsToCSV(vector<T> &vec1, vector<T> &vec2, string filename){
			of.open(filename);
      assert(vec1.size() == vec2.size());
			for(size_t i = 0; i < vec1.size(); i++)	
			{
				of << vec1[i] << "," << vec2[i] << endl;	
			}
			of.close();
		}

		template<typename T>
		void writeVectorsToCSV(vector<vector<T>> &vecs, string filename){
			of.open(filename);
			// assert(vec1.size() == vec2.size());
			for(size_t i = 0; i < vecs.size(); i++)	// number of players
				of << "plyaer" << i << ",";
			of << endl;
			for(size_t j = 0; j < vecs[0].size(); j++)  // mumber of rounds
			{
				for(size_t i = 0; i < vecs.size(); i++)	// number of players
				{
					of << std::to_string(vecs[i][j]) << ","; 	
				}
				of << endl;
			}
			of.close();
		}

		// WARNING , the index of i and j are swapped from the wreiteVectorsToCSV function!!!
		template<typename T>
		void writeAlgoVectorsToCSV(vector<vector<T>> &vecs, string filename){
			of.open(filename);
			for(size_t i = 0; i < vecs[0].size(); i++)	// number of actions 
				of << "action" << i << ",";
			of << endl;
			for(size_t j = 0; j < vecs.size(); j++)  // mumber of rounds
			{
				for(size_t i = 0; i < vecs[0].size(); i++)	// number of actions 
				{
					of << std::to_string(vecs[j][i]) << ","; 	
				}
				of << endl;
			}
			of.close();
		}

}io_Handler;
