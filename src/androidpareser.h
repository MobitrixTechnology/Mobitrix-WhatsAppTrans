#pragma once
//android 到 IOS装换
#include "publicfun.h"
#include "sqliteparsing.h"

typedef struct DEAL_CHAT_INFO {
	void* androidtoios;
	std::string mmsgstoredbfile;
	int tindex;
}*lpdealchatinfo, dealchatinfo;
class androidpareser
{

public:
	int towork(const char* androidpath);
	int readmessageinffromandroid();
	//bool isexitgrougmember(std::string jid_chatid);

	bool ishadiosmsgdb;


	int m_chatlimitindex;
	int m_runthreadnumber;

	std::string mselfjid;

	int m_totalnum;
private:
	std::string m_androiddatapath;//Android 备份数据目录
	
	std::string mNewChatStoragedbfile;//
	std::string mmsgstoredbfile;
	std::string mwadbfile;
	std::string mxmlfile;
	std::string mandroidpath;
	std::string mdatabasepath;
	std::string mtxtpath;


	std::string  mlimxmlfile;
	std::string androidmainpath;
	std::string mpermediapath;
	std::string mrestorepath;


	std::string mideviceid;



};