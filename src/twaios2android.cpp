#include "twaios2android.h"
#include "publicfun.h"
#include "totalpara.h"
#include <time.h>


std::string tCMGetDateTimeFromTimet(int lTime)
{
	//time_t lTime;

	time_t tmNow = lTime;
	tm* ptmNow = localtime(&tmNow);  
	char curTime[64] = { 0 };
	strftime(curTime, 35, "[%Y-%m-%d %H:%M:%S] ", ptmNow);
	return std::string(curTime);
	//	cout<<curTime<<endl;

}
int twaios2android::towork(const char* datapath)
{
	miosdatapath = datapath;
	miosdatapath += "group.net.whatsapp.WhatsApp.shared\\ChatStorage.sqlite";
	if (!IsPathExist(miosdatapath.c_str()))
	{
		printf("Sqlite is Error\r\n");
		printf("Failed\r\n");
		return -1;
	}
	mtoandroidpath = datapath;
	CreatePath(mtoandroidpath.c_str());

	mtxtpath = datapath;
	mtxtpath +="\\txt\\";
	CreatePath(mtxtpath.c_str());

	mWhatsAppMediapath = mtoandroidpath;
	mWhatsAppMediapath += "\\Media\\";
	
	CreatePath(mWhatsAppMediapath.c_str());
	
	printf("Read Session\r\n");
	readsessiondata();
	printf("Read Group\r\n");
	readgroupinfodata();
	printf("Read Chat\r\n");
	if (readmessagesdata() == 0)
	{
		printf("Success\r\n");
	}
	else
	{
		printf("Failed\r\n");
	}
	
	return 0;
}
int twaios2android::readsessiondata()
{

	tsqliteparsing osqliteparser(miosdatapath.c_str());
	char slqstr[1024];
	int sqllen = sprintf(slqstr, "select Z_PK,ZCONTACTJID,ZLASTMESSAGEDATE,ZPARTNERNAME,ZGROUPINFO,ZSESSIONTYPE from ZWACHATSESSION");
	slqstr[sqllen] = 0;
	std::vector<std::map<std::string, std::string> > vmapstr;
	if (osqliteparser.SqlQuery(slqstr, vmapstr))
	{
		for (int n = 0; n < vmapstr.size(); n++)
		{
	
			sessioninfo osessioninfo;
			std::string Z_PK = FindDataFromMap(vmapstr.at(n), "Z_PK");
			std::string ZCONTACTJID = FindDataFromMap(vmapstr.at(n), "ZCONTACTJID");
			std::string ZLASTMESSAGEDATE = FindDataFromMap(vmapstr.at(n), "ZLASTMESSAGEDATE");
			std::string ZGROUPINFO = FindDataFromMap(vmapstr.at(n), "ZGROUPINFO");
			std::string ZPARTNERNAME = FindDataFromMap(vmapstr.at(n), "ZPARTNERNAME");
			std::string ZSESSIONTYPE = FindDataFromMap(vmapstr.at(n), "ZSESSIONTYPE");
			osessioninfo.id = atoi(Z_PK.c_str());
			osessioninfo.key_remote_jid = ZCONTACTJID;
			//if (!ZGROUPINFO.empty())
			if (ZCONTACTJID == "status@broadcast")
			{
				osessioninfo.gtype = 5;
				osessioninfo.subject = ZPARTNERNAME;
			}
			else if(strstr(ZCONTACTJID.c_str(),".0:0@s.whatsapp.net"))
			{
				osessioninfo.gtype = 17;
				osessioninfo.subject = ZPARTNERNAME;
			}
			else if (strstr(ZCONTACTJID.c_str(), "@s.whatsapp.net"))
			{
				osessioninfo.gtype = 0;
				osessioninfo.subject = ZPARTNERNAME;
			}
			else if (strstr(ZCONTACTJID.c_str(), "@temp"))
			{
				osessioninfo.gtype = 2;
				osessioninfo.subject = ZPARTNERNAME;
			}
			else
			{
				osessioninfo.gtype = 1;
				osessioninfo.subject = ZPARTNERNAME;
			}
			//osessioninfo.creation

			osessioninfo.creation = formatiostm_wm(ZLASTMESSAGEDATE);
			osessioninfo.last_message_table_id = 0;
			osessioninfo.message_table_id = 0;
			//int64_t formatiostm_wm(std::string iostm)
			msessionmap.insert(std::make_pair(osessioninfo.id, osessioninfo));

		}
	}
	
	return 0;
}
//


std::string twaios2android::getgroupgjid(int z_pk)
{
	std::map<int, sessioninfo>::iterator itt;
	itt = msessionmap.find(z_pk);
	if (itt != msessionmap.end())
	{
		return itt->second.key_remote_jid;

	}


	return "";

}
bool twaios2android::getkeyremotejid(int id, std::string& jid, int& gtype)
{

	std::map<int, sessioninfo>::iterator itt;
	itt = msessionmap.find(id);
	if (itt != msessionmap.end())
	{
		jid= itt->second.key_remote_jid;

		gtype = itt->second.gtype;
		return true;
	}

	return false;
}

bool twaios2android::getmediainfo(tsqliteparsing* osqliteparser,int id,std::string & media_url,std::string &media_mime_type,std::string & media_size,std::string& media_name,std::string &media_hash,int & media_duration,int & origin,std::string &latitude,std::string & longitude,std::string & media_caption,std::string &thumbpath)
{//

	char slqstr[1024];
	int sqllen = sprintf(slqstr, "select ZFILESIZE,ZMOVIEDURATION,ZMEDIAORIGIN,ZMESSAGE,ZLATITUDE,ZLONGITUDE,ZMEDIAURLDATE,ZAUTHORNAME,ZMEDIALOCALPATH,ZMEDIAURL,ZTITLE,ZVCARDNAME,ZVCARDSTRING,ZXMPPTHUMBPATH from ZWAMEDIAITEM where Z_PK==\'%d\'",id);
	slqstr[sqllen] = 0;
	std::vector<std::map<std::string, std::string> > vmapstr;
	if (osqliteparser->SqlQuery(slqstr, vmapstr))
	{
		if (vmapstr.size() != 1)
		{
			return false;
		}

		media_name = FindDataFromMap(vmapstr.at(0), "ZMEDIALOCALPATH");
		media_mime_type= FindDataFromMap(vmapstr.at(0), "ZVCARDSTRING");
		media_size= FindDataFromMap(vmapstr.at(0), "ZFILESIZE");
		media_url = FindDataFromMap(vmapstr.at(0), "ZMEDIAURL");
		media_hash = FindDataFromMap(vmapstr.at(0), "ZVCARDNAME");
		thumbpath = FindDataFromMap(vmapstr.at(0), "ZXMPPTHUMBPATH");	
		std::string  media_durationstr = FindDataFromMap(vmapstr.at(0), "ZMOVIEDURATION");
		if (media_durationstr.empty())
		{
			media_duration = 0;
		}
		else
		{
			media_duration = atoi(media_durationstr.c_str());
		}

		std::string originstr=  FindDataFromMap(vmapstr.at(0), "ZMEDIAORIGIN");
		if (originstr.empty())
		{
			origin = 0;
		}
		else
		{
			origin = atoi(originstr.c_str());
		}

		latitude= FindDataFromMap(vmapstr.at(0), "ZLATITUDE");
		longitude = FindDataFromMap(vmapstr.at(0), "ZLONGITUDE");

		media_caption = FindDataFromMap(vmapstr.at(0), "ZAUTHORNAME");
		if (media_caption.empty())
		{
			media_caption = FindDataFromMap(vmapstr.at(0), "ZTITLE");
		}
	}

	return true;

}


//
int twaios2android::readgroupinfodata()
{


	tsqliteparsing* osqliteparser = new tsqliteparsing(miosdatapath.c_str());
	char slqstr[1024];
	int sqllen = sprintf(slqstr, "select Z_PK,ZISADMIN,ZCHATSESSION,ZMEMBERJID,ZSENDERKEYSENT,ZCONTACTABID from ZWAGROUPMEMBER");
	slqstr[sqllen] = 0;
	std::vector<std::map<std::string, std::string> > vmapstr;
	if (osqliteparser->SqlQuery(slqstr, vmapstr))
	{
		for (int n = 0; n < vmapstr.size(); n++)
		{
			groupmemberinfo ogroupmemberinfo;
			std::string ZISADMIN = FindDataFromMap(vmapstr.at(n), "ZISADMIN");
			std::string ZCHATSESSION = FindDataFromMap(vmapstr.at(n), "ZCHATSESSION");
			std::string ZMEMBERJID = FindDataFromMap(vmapstr.at(n), "ZMEMBERJID");
			std::string ZSENDERKEYSENT = FindDataFromMap(vmapstr.at(n), "ZSENDERKEYSENT");
			std::string ZCONTACTABID = FindDataFromMap(vmapstr.at(n), "ZCONTACTABID");
			std::string Z_PK = FindDataFromMap(vmapstr.at(n), "Z_PK");
			if (Z_PK == "" || Z_PK.length() == 0)
			{
				continue;
			}
			if (ZISADMIN.empty() || ZISADMIN.length() == 0)
			{
				ogroupmemberinfo.admin = 0;
			}
			else
			{
				int tmpadmin = atoi(ZISADMIN.c_str());
				if (tmpadmin == 1)
				{
					ogroupmemberinfo.admin = 2;
				}
				else
				{
					ogroupmemberinfo.admin = 0;
				}

			}
			if (ZCHATSESSION == "" || ZCHATSESSION.length() == 0)
			{
				continue;
			}

			std::string gjid = getgroupgjid(atoi(ZCHATSESSION.c_str()));
			if (gjid.empty() || gjid == "")
			{
				continue;
			}
			ogroupmemberinfo.gjid = gjid;
			ogroupmemberinfo.jid = ZMEMBERJID;

			if (ZSENDERKEYSENT.empty())
			{
				ogroupmemberinfo.sent_sender_key = "";
			}
			else
			{
				ogroupmemberinfo.sent_sender_key = ZSENDERKEYSENT;
			}
			ogroupmemberinfo.id = atoi(Z_PK.c_str());

			ogroupmemberinfo.pending = 0;

			mgroupmemberinfomap.insert(std::make_pair(ogroupmemberinfo.id, ogroupmemberinfo));
			//mgroupmemberinfoarr.push_back(ogroupmemberinfo);


		}
	}
	delete osqliteparser;
	return 0;
}
std::string twaios2android::getmemberjid(int id)
{


	std::map<int, groupmemberinfo>::iterator itt;
	itt = mgroupmemberinfomap.find(id);
	if (itt != mgroupmemberinfomap.end())
	{
		return itt->second.jid;
	}
	return "";


}

int  twaios2android::readmessagesdata()
{

	tsqliteparsing* osqliteparser = new tsqliteparsing(miosdatapath.c_str());
	char slqstr[1024];
	int sqllen = sprintf(slqstr, "select Z_PK,ZISFROMME,ZMESSAGESTATUS,ZGROUPMEMBER,ZMESSAGETYPE,ZCHATSESSION,ZMEDIAITEM,ZMESSAGEDATE,ZSENTDATE,ZFROMJID,ZSTANZAID,ZTEXT,ZTOJID from ZWAMESSAGE");
	slqstr[sqllen] = 0;
	std::vector<std::map<std::string, std::string> > vmapstr;
	if (osqliteparser->SqlQuery(slqstr, vmapstr))
	{

		int msize = vmapstr.size();
		int _id = 2;
		//AWriteLog("twaios2android", "readmessagesdata Size:%d", vmapstr.size());
		for (int n = 0; n < vmapstr.size(); n++)
		{

			std::string ZMESSAGETYPE = FindDataFromMap(vmapstr.at(n), "ZMESSAGETYPE");
			if (ZMESSAGETYPE.empty())
			{
				continue;
			}
			std::string ZMESSAGESTATUS = FindDataFromMap(vmapstr.at(n), "ZMESSAGESTATUS");


			std::string ZISFROMME = FindDataFromMap(vmapstr.at(n), "ZISFROMME");


			int keyfromme = atoi(ZISFROMME.c_str());
			int status = 13;
			int mediawatype = 0;

			if (ZMESSAGESTATUS == "3")
			{
				status = 0;
			}
			int ztype = atoi(ZMESSAGETYPE.c_str());
			if (ztype == 0)
			{

				mediawatype = 0;
			}
			else if (ztype == 1)
			{
				mediawatype = 1;
			}
			else if (ztype == 2)
			{
				mediawatype = 3;
			}
			else if (ztype == 3)
			{
				mediawatype = 2;
			}
			else if (ztype == 4)
			{
				mediawatype = 4;
			}
			else if (ztype == 5)
			{
				mediawatype = 5;
			}
			else if (ztype == 6)
			{
				continue;

			}
			else if (ztype == 7)
			{
				mediawatype = 97;
			}

			else if (ztype == 8)
			{
				mediawatype = 9;
			}

			else if (ztype == 10)
			{
				continue;
			}

			else if (ztype == 15)
			{

				mediawatype = 20;
				
				continue;
			}
			else {
				continue;
			}

			std::string ZFROMJID = FindDataFromMap(vmapstr.at(n), "ZFROMJID");
			std::string ZTOJID = FindDataFromMap(vmapstr.at(n), "ZTOJID");
			std::string ZCHATSESSION = FindDataFromMap(vmapstr.at(n), "ZCHATSESSION");
			if (ZCHATSESSION.empty())
			{
				continue;
			}

			int sessionid = atoi(ZCHATSESSION.c_str());
			std::string jid;
			int gtype = 0;
			if (!getkeyremotejid(sessionid, jid, gtype))
			{

				continue;

			}
			std::string ZSTANZAID = FindDataFromMap(vmapstr.at(n), "ZSTANZAID");
			std::string ZMEDIAITEM = FindDataFromMap(vmapstr.at(n), "ZMEDIAITEM");
			std::string ZMESSAGEDATE = FindDataFromMap(vmapstr.at(n), "ZMESSAGEDATE");
			std::string ZSENTDATE = FindDataFromMap(vmapstr.at(n), "ZMESSAGEDATE");
			int64_t timestamp = formatiostm_wm(ZMESSAGEDATE);
			int64_t send_timestamp = formatiostm_wm(ZSENTDATE);
			std::string fromjid = "";
			char tmbuf[32] = { 0 };
			sprintf(tmbuf,"%lld", timestamp);

			std::string creationstr = std::string(tmbuf);
			if (creationstr.size() > 10)
			{
				creationstr = creationstr.substr(0, 10);
			}
			fromjid = jid;
			std::string remoteresource = jid;
			if (gtype == 1 && keyfromme == 0)
			{

				std::string ZGROUPMEMBER = FindDataFromMap(vmapstr.at(n), "ZGROUPMEMBER");
				if (ZGROUPMEMBER.empty())
				{
					//
					continue;
				}
				else
				{
					int zgroupmemberid = atoi(ZGROUPMEMBER.c_str());

					remoteresource = getmemberjid(zgroupmemberid);
					if (remoteresource.empty())
					{
						continue;

					}
					fromjid = remoteresource;
				}

			}
			std::string metext = FindDataFromMap(vmapstr.at(n), "ZTEXT");;
			std::string media_url;
			std::string media_mime_type;
			std::string media_size = "0";
			std::string media_name;
			std::string media_hash;
			int media_duration = 0;
			int origin = 0;
			std::string latitude = "0.0";
			std::string longitude = "0.0";
			std::string media_caption;
			std::string thumbpath;
			bool ishadmedia = false;
			if (!ZMEDIAITEM.empty())
			{
				int zid = atoi(ZMEDIAITEM.c_str());
				if (getmediainfo(osqliteparser, zid, media_url, media_mime_type, media_size, media_name, media_hash, media_duration, origin, latitude, longitude, media_caption, thumbpath))
				{
					ishadmedia = true;
				}

			}

			bool isdown = false;


			std::string attlocalfile = "";
		
			std::string remoteattfile = "";


			if (mediawatype == 4)
			{

				metext = std::string(media_mime_type);
				media_mime_type = "";

				media_name = std::string(media_hash);
				media_hash = "";
			}
			else if (mediawatype == 97)
			{
				mediawatype = 0;
			}
			else if (mediawatype == 1 || mediawatype == 2 || mediawatype == 3 || mediawatype == 9 || mediawatype == 20)
			{
				if (!media_name.empty())
				{
					isdown = true;
					media_name = GetFileName_Unxi(media_name.c_str());
					attlocalfile = mWhatsAppMediapath + media_name;
#ifdef WIN32
					Replace(attlocalfile, "/", "\\");
#endif
				}

				if (mediawatype == 1)
				{
					if (!media_name.empty())
					{
						media_name = GetFileName_Unxi(media_name.c_str());
						remoteattfile = mWhatsAppMediapath + media_name;//	WhatsApp Images

					}
				


				}
				else if (mediawatype == 20)
				{
					
					if (!media_name.empty())
					{
						media_name = GetFileName_Unxi(media_name.c_str());
						remoteattfile = mWhatsAppMediapath + media_name;//	WhatsApp Images

					}
					//	media_name = "";
				}

				else if (mediawatype == 2)
				{

					if (!media_name.empty())
					{
						media_name = GetFileName_Unxi(media_name.c_str());
						remoteattfile = mWhatsAppMediapath + media_name;//	WhatsApp Images

					}
				

				}
				else if (mediawatype == 3)
				{

					if (!media_name.empty())
					{
						media_name = GetFileName_Unxi(media_name.c_str());
						remoteattfile = mWhatsAppMediapath + media_name;//	WhatsApp Images

					}
				

				}
				else if (mediawatype == 9)
				{

					if (!media_name.empty())
					{
						media_name = GetFileName_Unxi(media_name.c_str());
						//media_caption
						media_name = media_caption;
						remoteattfile = mWhatsAppMediapath + media_caption;//	WhatsApp Images

					}
					

				}
			}

			if (attlocalfile.length() != 0)
				Writetrandat(mandroidid.c_str(), attlocalfile.c_str(), remoteattfile.c_str());

			if (metext.length() == 0|| metext == "")
			{

				metext = GetFileName(attlocalfile.c_str());
			}
			if (metext.length() == 0)
			{
				continue;
			}
		//	jid  metext  fromjid 
			std::string datastr = tCMGetDateTimeFromTimet(atoi(creationstr.c_str()));

			int len = metext.length();
			len += 128;
			char* buff = new char[len];
			int isize = sprintf(buff, "%s%s: %s\r\n", datastr.c_str(), fromjid.c_str(), metext.c_str());
			
			std::string chatfilename = mtxtpath + jid;
			chatfilename += ".txt";
			FILE* pfile = fopen(chatfilename.c_str(), "ab+");
			if (pfile)
			{
				fwrite(buff, 1, isize, pfile);
				fclose(pfile);
			}
			delete[] buff;
			printf("progress[%d/%d]\r\n", n, msize);
		}
	}
	else
	{
	return -1;
	}
	delete osqliteparser;
	return 0;
}

int twaios2android::Writetrandat(const char* deviceid, const char* scrfile, const char* dstfile)
{

	std::string tmpdstfile = std::string(dstfile);
	Replace(tmpdstfile, "/", "\\");
	//AWriteLog("waios2android","scrfile:%s, dstfile:%s", scrfile, tmpdstfile.c_str());
	CreatePath(GetFilePath(tmpdstfile.c_str()).c_str());

	copyfile(scrfile, tmpdstfile.c_str());


	return 0;

}