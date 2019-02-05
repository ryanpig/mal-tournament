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
	struct _constructor_tag{ explicit _constructor_tag(){}};
	public:
		static unique_ptr<SQLMgr>& getInstance(string dbname, string current_table)
		{
			static unique_ptr<SQLMgr> m_instance = make_unique<SQLMgr>(dbname, _constructor_tag());
			m_instance->setTable(current_table);
			return m_instance;
		}
					
		void connectToDB(string dbname);
		void setTable(string table){m_table = table;}

		void createTable();
		void insertRecords(vector<Record> &vec);
		void queryAll();
		void deleteTable();
		void selfTest();
		// hidde constructor for singleton design pattern 
		// BUT, make_unique needs public constructor so we use the private "struct tag" to make workarounds
		SQLMgr(string dbname, _constructor_tag) : db(nullptr)
		{
		 	connectToDB(dbname);
		}

		~SQLMgr()
		{
			sqlite3_close(db);
			db = nullptr;
		}

	private:
		// variables
		sqlite3 *db;
		string m_table;
		void sqlExeution(string &sql);
		// int exe_callback(void *unused, int count, char **data, char **columns);
};
	
