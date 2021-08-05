#include "androidpareser.h"
#include <publicfun.h>
#include "tsqliteparsing.h"

#include <time.h>
#include "plistapi.h"
#include "totalpara.h"
#ifdef WIN32
#include <process.h>
#endif
int androidpareser::towork( const char* androidpath)
{
	printf("Start parsing\n");

	mandroidpath = std::string(androidpath);
	
	mdatabasepath = mandroidpath+"\\chatpath\\";
	mtxtpath = mandroidpath + "\\txt\\";
	CreatePath(mtxtpath.c_str());


	mmsgstoredbfile = androidpath;
	mmsgstoredbfile += "\\chatpath\\msgstore.db";

	
	if (!IsPathExist(mmsgstoredbfile.c_str()))
	{

		printf("Database file not found.\n");
		return -1;

	}
	readmessageinffromandroid();
	printf("Parsing is complete\r\n");
	return 0;
}

std::string CMGetDateTimeFromTimet(int lTime)
{
	//time_t lTime;

	time_t tmNow = lTime;
	tm* ptmNow = localtime(&tmNow); 
	char curTime[64] = { 0 };
	strftime(curTime, 35, "[%Y-%m-%d %H:%M:%S] ", ptmNow);
	return std::string(curTime);
	//	cout<<curTime<<endl;

}
int androidpareser::readmessageinffromandroid()
{
	mselfjid = "me";
	printf("Read chat information, please wait\n");
	int msize = 0;
	tsqliteparsing osqliteparser(mmsgstoredbfile.c_str());
	char slqstr[1024];
	int sqllen = sprintf(slqstr, "select _id,key_remote_jid,key_from_me,key_id,status,data,timestamp,media_wa_type,remote_resource,received_timestamp,send_timestamp,media_url,media_mime_type,media_name,media_size,media_caption from messages limit 1,300000");
	slqstr[sqllen] = 0;
	std::vector<std::map<std::string, std::string> > vmapstr;
	if (osqliteparser.SqlQuery(slqstr, vmapstr))
	{


		msize = vmapstr.size();
		printf("A total of %d chat data found", vmapstr.size());
		for (int n = 0; n < vmapstr.size(); n++)
		{

			int m_id = atoi(FindDataFromMap(vmapstr.at(n), "_id").c_str());
			std::string media_wa_type = FindDataFromMap(vmapstr.at(n), "media_wa_type");
			if (media_wa_type.empty())
			{
				continue;
			}
			std::string key_remote_jid = FindDataFromMap(vmapstr.at(n), "key_remote_jid");
			if (key_remote_jid.empty()|| key_remote_jid=="-1")
			{
				continue;
			}

			std::string statusstr = FindDataFromMap(vmapstr.at(n), "status");


			std::string key_id = FindDataFromMap(vmapstr.at(n), "key_id");
			if (key_id.empty())
			{
				continue;
			}

			std::string key_from_me = FindDataFromMap(vmapstr.at(n), "key_from_me");

			int type= atoi(media_wa_type.c_str());
			int status = atoi(statusstr.c_str());

			if (status == 6)
			{
				continue;
			}
			else {
				status = 8;
			}

			int itype = type;

			if (type == 0)
			{

				itype = 0;
				
			}
			else if (type == 1)
			{
				itype = 1;
			}
			else if (type == 2)
			{
				itype = 3;
			}
			else if (type == 3|| type == 13)
			{
				itype = 2;
			}
			else if (type == 4)
			{
				itype = 4;
			}
			else if (type == 5)
			{
				itype = 5;
			}
			
			else if (type ==9)
			{
				itype = 8;
			}

			else if (type == 10)
			{
				continue;
			}

			else if (type == 20)
			{

				itype = 15;
				//continue;
			}
			else {
				continue;
			}
			

		//	printf("androidpareser", "code:10 itype:%d", itype);
			int keyfromme = atoi(key_from_me.c_str());
			bool isgroup = false;
			if(strstr(key_remote_jid.c_str(), "@g.us"))
			{
				isgroup = true;
			}
			std::string groupmemberjid;
			std::string greoupmember = "";
			if (keyfromme == 1)
			{
				groupmemberjid = mselfjid;
			}
			else
			{
				if(isgroup)
					groupmemberjid = FindDataFromMap(vmapstr.at(n), "remote_resource");
				else
				{
					groupmemberjid = key_remote_jid;
				}
			}
		
	
		
			std::string creationstr = FindDataFromMap(vmapstr.at(n), "timestamp");
			if (creationstr.size() > 10)
			{
				creationstr=creationstr.substr(0,10);
			}
			std::string datatxt="";
			if (itype == 0)
			{
				datatxt = FindDataFromMap(vmapstr.at(n), "data");
	
			}
			else
			{
				datatxt = FindDataFromMap(vmapstr.at(n), "media_name");
			}
			if (datatxt.length() == 0 || datatxt == "")
			{
				continue;
			}
			std::string datastr=CMGetDateTimeFromTimet(atoi(creationstr.c_str()));

			int len = datatxt.length();
			len += 128;
			char* buff = new char[len];
			int isize =sprintf(buff,"%s%s: %s\r\n", datastr.c_str(), groupmemberjid.c_str(), datatxt.c_str());
			
			std::string chatfilename = mtxtpath + key_remote_jid;
			chatfilename += ".txt";
			FILE* pfile = fopen(chatfilename.c_str(), "ab+");
			if (pfile)
			{
				fwrite(buff, 1, isize, pfile);
				fclose(pfile);
			}
			delete[] buff;
			printf("progress[%d/%d]\r\n",n, msize);


		}
	}
	vmapstr.clear();
	
	return 0;
}
