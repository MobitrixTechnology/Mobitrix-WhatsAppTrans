#include "tsqliteparsing.h"

#include "BaseLog.h"
tsqliteparsing::tsqliteparsing(const char* databasename)
{
	std::string dbfilename = databasename;
	//dbfilename=GBKToUTF8(dbfilename);
	///sdzw/db/infor.db
	pdb = NULL;
	isopenok = false;
	res = sqlite3_open(dbfilename.c_str(), &pdb);
	if (res)
	{
		//AWriteLog("work","SQLITE3 sqlite3_open error [%s] [%s]", sqlite3_errmsg(pdb), databasename);
		sqlite3_close(pdb);
		pdb = NULL;
	}
	else
	{
	//	//AWriteLog("whmiddleware", "SQLITE3 sqlite3_open OK [%s]", databasename);
		isopenok = true;

	}

}

tsqliteparsing::~tsqliteparsing()
{


	isopenok = false;
	if (pdb != NULL)
	{
	

		sqlite3_close(pdb);
		pdb = NULL;
	}


}

int tsqliteparsing::InitDataBaseToWal(std::string sPath, bool isWal)
{
	char* zErrMsg;

	sqlite3* db = NULL;

	int rc = sqlite3_open_v2(sPath.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX, NULL);

	if (rc != SQLITE_OK)
	{
	
		sqlite3_close(db);

		return -1;
	}

	if (isWal == true)
	{
		rc = sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, 0, &zErrMsg);

		if (rc != SQLITE_OK)
		{
			sqlite3_free(zErrMsg);

			sqlite3_close(db);

			return -1;
		}

		rc = sqlite3_exec(db, "PRAGMA wal_autocheckpoint=100;", NULL, 0, &zErrMsg);

		if (rc != SQLITE_OK)
		{
			sqlite3_free(zErrMsg);

			sqlite3_close(db);

			return -1;
		}
	}
	else
	{
		rc = sqlite3_exec(db, "PRAGMA journal_mode=DELETE;", NULL, 0, &zErrMsg);

		if (rc != SQLITE_OK)
		{
			sqlite3_free(zErrMsg);

			sqlite3_close(db);

			return -1;
		}
	}

	return true;
}



//执行指令 除却查询
bool tsqliteparsing::SqlExe(char* sql)
{

	if (!isopenok)
	{
		return false;
	}
	//加锁
	res = sqlite3_exec(pdb, sql, 0, 0, &errMsg);
	//解锁  
	if (res != SQLITE_OK)
	{
	////AWriteLog("whmiddleware", "SQLITE3 sqlite3_open error [%s]", sqlite3_errmsg(pdb));
		return false;
	}
	return true;
}
bool tsqliteparsing::SqlQuery(const char* sql, std::vector<std::map<std::string, std::string> >& str)//查询专用
{
	//查询数据
	if (!isopenok)
	{
		//AWriteLog("work", "SQLITE3 no isopenok  ");
		return false;
	}
	char** dbResult; //是 char ** 类型，两个*号
	int nRow, nColumn;//行   列
	int i, j;        //遍历基数
	int index;        //列
	//AWriteLog("work", "SQLITE3 [%s] ", sql);
	do{
		res = sqlite3_get_table(pdb, sql, &dbResult, &nRow, &nColumn, &errMsg);

		if (SQLITE_OK == res)
		{
			//查询成功
			str.clear();
			index = nColumn; //前面说过 dbResult 前面第一行数据是字段名称，从 nColumn 索引开始才是真正的数据
			// 		printf( "查到%d条记录\n", nRow );
		//	//AWriteLog("whmiddleware", "nRow [%d] ", nRow);
			for (i = 0; i < nRow; i++)
			{

				//	printf("nRow [%d] ", i);
				//	//AWriteLog("whmiddleware", "i [%d] ", i);
				std::map<std::string, std::string> unit;
				for (j = 0; j < nColumn; j++)
				{
					if (dbResult[index] != NULL)
					{
	#ifdef _DEBUG
						//	CString key=dbResult[j];
						//	CString value=dbResult [index];
						//	ConvertUtf8ToGBK(key);
						//	ConvertUtf8ToGBK(value);
						//	unit.insert(make_pair(key,value));
						//	unit.insert(make_pair(dbResult[j],dbResult [index]));
	#else


						unit.insert(std::make_pair(dbResult[j], dbResult[index]));

	#endif
					}
					++index;
				}
				if (unit.size() > 0)
				{
					str.push_back(unit);
				}
			}
			break;
		}
		else
		{
			//AWriteLog("work", "SQLITE3 [%s] sqlite3_open error [%s]", sql, errMsg);
			if (res == SQLITE_BUSY)
			{
				Sleep(1);
				continue;
			}
			else
			{
				//AWriteLog("work", "###ERROR SQLITE3 [%s] sqlite3_open error [%s]", sql, errMsg);
				break;
			}
		
			//sqlite3_free_table(dbResult);
		//	return false;
		}
	}while (1);
	//到这里，不论数据库查询是否成功，都释放 char** 查询结果，使用 sqlite 提供的功能来释放
	sqlite3_free_table(dbResult);
	return true;

}

int tsqliteparsing::SqlQuery(const char* sql, std::string& str, const char* splite1, char splite2)
{
	//查询数据

	if (!isopenok)
	{
		printf("数据库未成功打开\r\n");
		return -1;
	}

	char** dbResult; //是 char ** 类型，两个*号
	int nRow, nColumn;//行   列
	int i, j;        //遍历基数
	int index;        //列

	res = sqlite3_get_table(pdb, sql, &dbResult, &nRow, &nColumn, &errMsg);

	if (SQLITE_OK == res)
	{
		//查询成功
		str.clear();
		index = nColumn; //前面说过 dbResult 前面第一行数据是字段名称，从 nColumn 索引开始才是真正的数据
// 		printf( "查到%d条记录\n", nRow );
		for (i = 0; i < nRow; i++)
		{
			//printf( "第 %d 条记录\n", i+1 );
			for (j = 0; j < nColumn; j++)
			{
				//printf( "字段名:%s  ß> 字段值:%s\n",  dbResult[j], dbResult [index] );

				if (dbResult[index] != NULL)
				{
					str += dbResult[index];
				}

				str += splite1;
				++index;

			}

			str[str.size() - 1] = splite2;
		}
	}
	else
	{

		sqlite3_free_table(dbResult);
		//AWriteLog("work", "SQLITE3 [%s]sqlite3_open error [%s]", sql, errMsg);
		return -1;

	}
	//到这里，不论数据库查询是否成功，都释放 char** 查询结果，使用 sqlite 提供的功能来释放
	sqlite3_free_table(dbResult);

	int len = str.size();
	if (len >= 1)
	{
		str[len - 1] = 0;
		str.resize(len - 1, ' ');

	}




	return nRow;
}



bool tsqliteparsing::SqlQuery(const char* sql, std::vector<std::map<std::string, tblockbytes> >& str)
{
	//查询数据

	if (!isopenok)
	{
		//printf("数据库未成功打开\r\n");
		return false;
	}
	bool flg = false;

	sqlite3_stmt* stmt = NULL;


	res = sqlite3_prepare(pdb, sql, -1, &stmt, 0);

	if (res == SQLITE_OK)
	{
		while (SQLITE_ROW == sqlite3_step(stmt))
		{
			flg = true;
			std::map<std::string, tblockbytes> unit;
			int count = sqlite3_column_count(stmt);
			for (int m = 0; m < count; m++)
			{
				std::string name = sqlite3_column_name(stmt, m);
				tblockbytes block;
				char* data = (char*)sqlite3_column_blob(stmt, m);//得到纪录中的BLOB字段
				block.plen = sqlite3_column_bytes(stmt, m);//得到字段中数据的长度
				block.pdata = new char[block.plen + 1];
				block.pdata[block.plen] = 0;
				memcpy(block.pdata, data, block.plen);
				unit.insert(std::make_pair(name, block));



			}
			str.push_back(unit);


		}
		sqlite3_finalize(stmt);

	}

	return flg;
}



//
bool tsqliteparsing::SqlQuery_Huawei(const char* sql, std::map<int, std::string>& filenamemap)
{
	//查询数据

	if (!isopenok)
	{
		//printf("数据库未成功打开\r\n");
		return false;
	}
	bool flg = false;

	sqlite3_stmt* stmt = NULL;


	res = sqlite3_prepare(pdb, sql, -1, &stmt, 0);

	if (res == SQLITE_OK)
	{
		while (SQLITE_ROW == sqlite3_step(stmt))
		{
			int count = sqlite3_column_count(stmt);
			if (count == 4)
			{
				char* file_index_data = (char*)sqlite3_column_blob(stmt, 3);//得到纪录中的BLOB字段
				int file_index_data_len = sqlite3_column_bytes(stmt, 3);//得到字段中数据的长度
				char* pdata = new char[file_index_data_len + 1];
				pdata[file_index_data_len] = 0;
				memcpy(pdata, file_index_data, file_index_data_len);
				int  file_index = atoi(pdata);
				delete[] pdata;


				char* cfile_data_mlen = (char*)sqlite3_column_blob(stmt, 1);//得到纪录中的BLOB字段
				int file_data_mlen_len = sqlite3_column_bytes(stmt, 1);//得到字段中数据的长度
				pdata = new char[file_data_mlen_len + 1];
				pdata[file_data_mlen_len] = 0;
				memcpy(pdata, cfile_data_mlen, file_data_mlen_len);
				int  file_data_mlen = atoi(pdata);
				delete[] pdata;

				char* file_data = (char*)sqlite3_column_blob(stmt, 0);//得到纪录中的BLOB字段
				int file_data_len = sqlite3_column_bytes(stmt, 0);//得到字段中数据的长度

				if (file_data_len != file_data_mlen)
				{
					file_data_mlen = file_data_len < file_data_mlen ? file_data_len : file_data_mlen;
				}
				std::map<int, std::string>::iterator fitx;
				fitx = filenamemap.find(file_index);
				if (fitx != filenamemap.end())
				{
					FILE* pdfile = fopen(fitx->second.c_str(), "ab+");
					if (pdfile)
					{
						fwrite(file_data, 1, file_data_mlen, pdfile);
						fclose(pdfile);
					}


				}

			}



		}
		sqlite3_finalize(stmt);

	}

	return flg;
}

const char* tsqliteparsing::GetLastError()
{

	return errMsg;


}

void GetLimit(unsigned int totalnum, unsigned int maxlen, std::vector<TLimitUnit>& vect)
{

	if (totalnum <= maxlen)		//可以一次查询完毕
	{
		TLimitUnit unit;
		unit.pos = 0;
		unit.len = totalnum;
		vect.push_back(unit);
	}
	else
	{
		int len = totalnum / maxlen;		//总数
		int left = totalnum % maxlen;		//剩余
		for (int m = 0; m < len; m++)
		{
			TLimitUnit unit;
			unit.pos = m * maxlen;
			unit.len = maxlen;
			vect.push_back(unit);
		}
		if (left > 0)
		{
			TLimitUnit unit;
			unit.pos = len * maxlen;
			unit.len = left;
			vect.push_back(unit);

		}

	}
}



