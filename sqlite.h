#include <sqlite3.h>
#include <iostream>
#include <vector>
#define UNUSED(x) (void)(x)
#pragma once
using namespace std;

static const string Qu = "'";
// string header = "INSERT INTO " + table + " (round, actions, players, steps, type_p0, payoff_p0, type_p1, payoff_p1) ";
struct Record
{
	public:
		Record(int round, int actions, int players, int steps,  string type_p0 ,float payoff_p0, string type_p1, float payoff_p1) :
			round(round), actions(actions), players(players), steps(steps), type_p0(type_p0), payoff_p0(payoff_p0), type_p1(type_p1), payoff_p1(payoff_p1)
			{}
		string makeSQLString() const
		{
			string sql{};
			sql += "VALUES (";
			sql += to_string(round) + "," + to_string(actions) + "," + to_string(players) + "," + to_string(steps) + ","
				+ quote(type_p0) + "," + to_string(payoff_p0) + "," + quote(type_p1) + "," + to_string(payoff_p1);
			sql += ");";
			return sql;
		}	
	private:
		int round, actions, players, steps;
		string type_p0;
		float payoff_p0;
		string type_p1;
		float payoff_p1;
		string quote(const string &str) const { return Qu + str + Qu;}
};

class SQLMgr
{
	public:
		static SQLMgr* getInstance(string dbname, string current_table)
		{
			if(!m_instance){
				m_instance = new SQLMgr(dbname);
				m_instance->setTable(current_table);
				return m_instance;
			}else{
				m_instance->setTable(current_table);
				return m_instance;
			}
		}
					
		void connectToDB(string dbname);
		void setTable(string table){m_table = table;}

		void createTable();
		void insertRecords(vector<Record> &vec);
		void queryAll();
		void deleteTable();
		void selfTest();

	private:
		// variables
		static SQLMgr *m_instance;
		sqlite3 *db;
		string m_table;
		// hidde constructor for singleton design pattern 
		SQLMgr(string dbname) : db(nullptr)
		{
		 	connectToDB(dbname);
		}

		~SQLMgr()
		{
			sqlite3_close(db);
			db = nullptr;
		}
		void sqlExeution(string &sql);
		// int exe_callback(void *unused, int count, char **data, char **columns);
};
	
