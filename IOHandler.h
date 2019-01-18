#include <iostream>
#include <fstream>
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
		void writeVectorsToCSV(vector<T> &vec1, vector<T> &vec2, string filename){
			of.open(filename);
			assert(vec1.size() == vec2.size());
			for(size_t i = 0; i < vec1.size(); i++)	
			{
				of << vec1[i] << "," << vec2[i] << endl;	
			}
			of.close();
		}

}io_Handler;
