#pragma once
//
#include <sqlite3/sqlite3.h>
//#include "publicfun.h"
#include <string>
#include <vector>
#include <map>
#define MAXCOUNT  2000

struct TLimitUnit
{
	unsigned int pos;
	unsigned int len;
	TLimitUnit()
	{
		pos = len = 0;
	}



};
struct tblockbytes
{
	char* pdata;
	unsigned int plen;
	tblockbytes()
	{
		pdata = NULL;
		plen = 0;
	}
};

class tsqliteparsing
{

public:
	tsqliteparsing(const char* databasename);

	~tsqliteparsing();

	bool  isopenok;
private:

	sqlite3* pdb;       //数据库据并
	char* errMsg;       //执行指令错误返回信息
	int res;            //执行指令返回值

public:

	bool SqlExe(char* sql);    //执行指令

	bool SqlQuery(const char* sql, std::vector<std::map<std::string, std::string> >& str);//查询专用
	int  SqlQuery(const char* sql, std::string& str, const char* splite1 = "\2", char splite2 = '\1');
	bool SqlQuery(const char* sql, std::vector<std::map<std::string, tblockbytes> >& str);
	const char* GetLastError();
	bool SqlQuery_Huawei(const char* sql, std::map<int, std::string>& filenamemap);
	bool text();
	int InitDataBaseToWal(std::string sPath, bool isWal);//开始WAL

};

