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

	sqlite3* pdb;       //���ݿ�ݲ�
	char* errMsg;       //ִ��ָ����󷵻���Ϣ
	int res;            //ִ��ָ���ֵ

public:

	bool SqlExe(char* sql);    //ִ��ָ��

	bool SqlQuery(const char* sql, std::vector<std::map<std::string, std::string> >& str);//��ѯר��
	int  SqlQuery(const char* sql, std::string& str, const char* splite1 = "\2", char splite2 = '\1');
	bool SqlQuery(const char* sql, std::vector<std::map<std::string, tblockbytes> >& str);
	const char* GetLastError();
	bool SqlQuery_Huawei(const char* sql, std::map<int, std::string>& filenamemap);
	bool text();
	int InitDataBaseToWal(std::string sPath, bool isWal);//��ʼWAL

};

