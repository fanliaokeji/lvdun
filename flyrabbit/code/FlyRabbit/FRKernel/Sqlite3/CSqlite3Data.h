#pragma once
#include "sqlite3\sqlite3.h"
#include <string>
#include <vector>
#include <map>
//#pragma comment(lib,"sqlite3.lib")
class Sqlite3Data
{
public:
	Sqlite3Data():m_db(NULL){};
	~Sqlite3Data()
	{
		db_close();
		free();
	};
	BOOL Init();
	int execDML(const char* sql,BOOL bQuery = TRUE);
	void db_close();
	void free();
	typedef int (*psqlite3_callback)(void*,int,char**, char**);
	typedef int (*psqlite3_open_v2)( const char *filename, sqlite3 **ppDb, int flags, const char *zVfs);  
	typedef int (*psqlite3_close)(sqlite3 *);  
	typedef int (*psqlite3_exec)( sqlite3*, const char *sql, int (*psqlite3_callback)(void*,int,char**,char**), void *, char **errmsg );  
	typedef void (*psqlite3_free)(void*);   
public:
	static int callback(void* pData, int columns,char** column_value, char** column_name);
	sqlite3* m_db;
	std::vector<std::map<std::string,std::string>> m_vQuery ;
	psqlite3_open_v2 m_pfn_sqlite3_open_v2;
	psqlite3_close m_pfn_sqlite3_close;
	psqlite3_exec m_pfn_sqlite3_exec;
	psqlite3_free m_pfn_sqlite3_free;
};