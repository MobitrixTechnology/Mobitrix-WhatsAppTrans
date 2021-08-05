#pragma once
//IOS转Android

#include "publicfun.h"
#include "tsqliteparsing.h"

class twaios2android
{

public:
	int towork(const char* datapath);
	


	int readsessiondata();
	int readgroupinfodata();
	int readmessagesdata();

	std::string getgroupgjid(int z_pk);
	bool getkeyremotejid(int id, std::string& jid, int& gtype);
	//bool getmediainfo(sqliteparsing* osqliteparser ,int id);
	bool getmediainfo(tsqliteparsing* osqliteparser, int id, std::string& media_url, std::string& media_mime_type, std::string& media_size, std::string& media_name, std::string& media_hash, int& media_duration, int& origin, std::string& latitude, std::string& longitude, std::string& media_caption, std::string& thumbpath);

	std::string getmemberjid(int id);



	int Writetrandat(const char* deviceid, const char* scrfile, const char* dstfile);

private:

	std::string mdeviceid;
	std::string mandroidid;
	//group.net.whatsapp.WhatsApp.shared
	std::string miosdatapath;
	std::string mioschatdb;
	std::string mtoandroidpath;//
	std::string mtxtpath;
	//媒体文件目录


	std::string mnewdb;
	std::string mWhatsAppMediapath;

	std::string mtranfiledat;
	std::string mnewmsgstoredb;

	std::string mserpath;
	std::map<int, sessioninfo> msessionmap;//
	std::map<int ,groupmemberinfo>  mgroupmemberinfomap;

	//Media 信息
	std::map<std::string, int> mchat_jidarr;
	std::map<int, messengesmediainfo>  mchatmediasarr;
	std::map<int,messengesinfo>  mchatmessagesarr;

	std::string mxmlfile;
	std::string  mlimxmlfile;
	std::string mjid;//自己的ID
	std::string mselfjid;
	std::string mphonestr;
	std::string mdstmsgdbfile;
	//文件传输、目录创建【命令】

	FILE* mpfile;
};

