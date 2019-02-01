#include "sqlite.h"
// callback for sql execution
SQLMgr *SQLMgr::m_instance = nullptr;

int exe_callback(void *unused, int count, char **data, char **columns)
{
	UNUSED(unused);
	cout << "total " << count << " columns!" << endl;
	for(int i = 0; i < count; i++)
		cout << "column " << columns[i] << ", value " << data[i] << endl;
	return 0;
}

void SQLMgr::connectToDB(string dbname)
{
	if(sqlite3_open(dbname.data(), &db) != SQLITE_OK)
		cerr << "can't open db" << endl;
	else
		cout << "connect successfully" << endl;
}

void SQLMgr::createTable()
{
	string sql = "CREATE TABLE " + m_table + " ("
									"round INT NOT NULL,"
									"actions INT NOT NULL,"  
									"steps INT NOT NULL,"  
									"players INT NOT NULL,"  
									"type_p0 TEXT NOT NULL," 
									"payoff_p0 REAL NOT NULL,"  
									"type_p1 TEXT NOT NULL,"  
									"payoff_p1 REAL NOT NULL" 
	");";
	sqlExeution(sql);
}

void SQLMgr::insertRecords(vector<Record> &vec)
{
	string header = "INSERT INTO " + m_table + " (round, actions, players, steps, type_p0, payoff_p0, type_p1, payoff_p1) ";
	string sql{};
	for(const auto &v : vec)
		sql += header + v.makeSQLString(); 
	sqlExeution(sql);
}

void SQLMgr::queryAll()
{
	string sql = "SELECT * FROM " + m_table;
	sqlExeution(sql);
}

void SQLMgr::deleteTable()
{
	string sql = "DELETE FROM " + m_table;
	sqlExeution(sql);
}

void SQLMgr::sqlExeution(string &sql)
{
	char* zErrMsg = nullptr;
	cout << sql << endl;
	if(sqlite3_exec(db, sql.data(), exe_callback, 0 , &zErrMsg) != SQLITE_OK)
		cerr << "sql execution failed:" << zErrMsg << endl;
	else
		cout << "sql execution succeeded." << endl;
}

void SQLMgr::selfTest()
{
	// createTable
	createTable();
	// insert records
	const Record sample1{ 0, 2, 2, 1000, "random", 65.3, "UCB1", 57.2};
	const Record sample2{ 1, 2, 2, 1000, "random", 65.3, "UCB1", 57.2};
	vector<Record> vec{sample1, sample2};
	insertRecords(vec);
	// update record
	// TODO:
	
	// query result
	queryAll();
	// delete result
	deleteTable();
}
