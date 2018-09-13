#include "pch.h"
#include "FileManage.h"
#include<string>
#include<iostream>
#include<stdio.h>
#include<memory.h>
#include<fstream>
#include<vector>
#include <algorithm>
#include <time.h>
#include <iomanip>
using namespace std;

/*全局变量定义*/
char content[BLKSIZE * 10];

FileManage::FileManage()
{
	fp = NULL;
	baseAddr = (char*)malloc(sizeof(char) * DISKSIZE);
	memset(baseAddr, 0, DISKSIZE);
	fcbnum = 1;
	fcb_min = 1;
	fcb_emp = 1;
	fcb_cur = 0;
	ifmount = false;
	string rootName = "root";
	fcb[0].initialize();
	strcpy_s(fcb[0].fileName, rootName.c_str());
	fcb[0].FCBNum = 0;
	fcb[0].fileType = true;
	string curtime = getTime();
	strcpy_s(fcb[0].fileTime, curtime.c_str());
	dx = 1;
	for (int i = 0; i < BLKNUM / 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			bitmap[8 * i + j] = false;
		}
	}
	for (int i = 1; i < BLKNUM; i++)
		fcb[i].initialize();
	memset(layers, 1, sizeof(layers));
	layer = 0;
	dFlag = 0;
}


FileManage::~FileManage()
{
	free(baseAddr);
	close();
}

int FileManage::create(char name[])
{
	string symPath ="D:/";
	symPath += name;
	fp =fopen(symPath.c_str(), "w+");
	fwrite(baseAddr, sizeof(char), sizeof(char) * DISKSIZE, fp);
	fclose(fp);
	free(baseAddr);
	return 0;
}
int FileManage::mount(char name[])
{
	char Map[32768];
	//zm add
	strcpy(sysname, "d:\\");
	strcat(sysname, name);
	string symPath = "D:/";
	symPath += name;
	strcpy(systemName, name);
	minifile = systemName;						//临时的空间名
	string str = minifile;
	minifile1 = str.append(":");			//临时的空间名:
	minifile2 = str.append("/"); 			//临时的空间名:/
	curpath = minifile2;
	fp = fopen(symPath.c_str(), "r+");
	if (fp == NULL)
	{
		return 0;
	}
	fseek(fp, 0,SEEK_SET);
	fread(&ifmount, sizeof(bool), 1, fp);
	
	if (ifmount == false)
	{
		ifmount = true;
	}
	else
	{
		fread(&fcbnum, 4, 1, fp);
		fread(&fcb_min, 4, 1, fp);
		fread(&fcb_emp, 4, 1, fp);
		fread(&fcb_cur, 4, 1, fp);
		fseek(fp, 4096, SEEK_SET);
		fread(Map, 1, BLKNUM / 8, fp);
		for (int i = 0; i < BLKNUM / 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				bitmap[8 * i + j] = (Map[i] & (1 << j)) / (1 << j);
			}
		}
		for (int i = 0; i < BLKNUM; i++)
		{
			fread(fcb[i].fileName, 1, 14, fp);
			fread(&fcb[i].fileSize, 4, 1, fp);
			fread(&fcb[i].fileType, sizeof(bool), 1, fp);
			fread(&fcb[i].fileTime, 1, 20, fp);
			fread(&fcb[i].delFlag, sizeof(bool), 1, fp);
			fread(&fcb[i].FCBNum, 4, 1, fp);
			fread(&fcb[i].fileContent, 4, 1, fp);
			fread(&fcb[i].fFCB, 4, 1, fp);
			fread(&fcb[i].lFCB, 4, 1, fp);
			fread(&fcb[i].rFCB, 4, 1, fp);
			fread(&fcb[i].sFCB, 4, 1, fp);
		}
	}	
	fclose(fp);
	return 1;
	/*getFcbArray();*/
}
//需加上对fcb的重新写入
int FileManage::close()
{
	char Map[32768];
	string symPath = "D:/";
	symPath += systemName;
	fp = fopen(symPath.c_str(), "r+");
	fseek(fp, 0, SEEK_SET);
	fwrite(&ifmount, sizeof(bool), 1, fp);
	fwrite(&fcbnum, sizeof(int), 1, fp);
	
	fwrite(&fcb_min, 4, 1, fp);
	fwrite(&fcb_emp, 4, 1, fp);
	fwrite(&fcb_cur, 4, 1, fp);
	fseek(fp, 4096, SEEK_SET);

	for (int i = 0; i < BLKNUM / 8; i++)
	{
		Map[i] = 0;
		for (int j = 0; j < 8; j++)
		{
			Map[i] = ((int)bitmap[8 * i + j]) << j | Map[i];
		}
	}

	fwrite(Map, 1, BLKNUM / 8, fp);

	//开始写入fcb的值
	for (int i = 0; i < BLKNUM; i++) {
		fwrite(fcb[i].fileName, 1, 14, fp);
		fwrite(&fcb[i].fileSize, 4, 1, fp);
		fwrite(&fcb[i].fileType, sizeof(bool), 1, fp);
		fwrite(&fcb[i].fileTime, 1, 20, fp);
		fwrite(&fcb[i].delFlag, sizeof(bool), 1, fp);
		fwrite(&fcb[i].FCBNum, 4, 1, fp);
		fwrite(&fcb[i].fileContent, 4, 1, fp);
		fwrite(&fcb[i].fFCB, 4, 1, fp);
		fwrite(&fcb[i].lFCB, 4, 1, fp);
		fwrite(&fcb[i].rFCB, 4, 1, fp);
		fwrite(&fcb[i].sFCB, 4, 1, fp);
	}
	fclose(fp);
	return 0;
}
int FileManage::format()
{
	memset(&fcbnum, 0,sizeof(int));
	memset(bitmap, 0, sizeof(bitmap));
	memset(fcb, 0, sizeof(fcb));
	close();
	return 1;
}


//从外存中获取fcb数组
void FileManage::getFcbArray() {
	
	fp = fopen(systemName, "r+");//以只读的形式打开系统文件
	fseek(fp, FCBSTART, 0);

	for (int i = 0; i < fcbnum; i++) {
		//读取文件名
		char s[20];
		fread(s, 1, 20, fp);
		strcpy(fcb[i].fileName,s);
		//读取文件大小
		int a[2];
		fread(a, 4, 1, fp);
		fcb[i].fileSize = a[0];
		//读取文件类型
		bool filetype[2];
		fread(filetype, 1, 1, fp);
		fcb[i].fileType = filetype[0];
		//读取文件修改时间
		char strTime[14];
		fread(strTime, 14, 1, fp);
		strcpy(fcb[i].fileTime,strTime);
		//读取文件删除标志
		bool flagDel[2];
		fread(flagDel, 1, 1, fp);
		fcb[i].delFlag = flagDel[0];
		//读取文件FCB的位置
		int positionFcb[2];
		fread(positionFcb, 4, 1, fp);
		fcb[i].FCBNum = positionFcb[0];
		//读取文件内容起始盘块号
		int contentFcb[2];
		fread(contentFcb, 4, 1, fp);
		fcb[i].fileContent = contentFcb[0];
		//读取文件父节点fcb
		int fFcb[2];
		fread(fFcb, 4, 1, fp);
		fcb[i].fFCB = fFcb[0];
		//读取文件左兄弟节点fcb
		int lFcb[2];
		fread(lFcb, 4, 1, fp);
		fcb[i].lFCB = lFcb[2];
		//读取文件右兄弟节点fcb
		int rFcb[2];
		fread(rFcb, 4, 1, fp);
		fcb[i].rFCB = rFcb[2];
		//读取文件子节点fcb
		int sFcb[2];
		fread(sFcb, 4, 1, fp);
		fcb[i].rFCB = sFcb[2];
	}

	fclose(fp);
}

////获取文件在磁盘中所占用的所有的盘块
//void FileManage::getMap(char str[20]) {
//	//读取文件的路径获取文件的fcb的值
//	if (cd(str)) {
//		int startblock = fcb[fcb_cur].fileContent;//获取第一个盘块
//		int count = fcb[fcb_cur].fileSize / 4096;//获取所占的盘块数
//		for (int j = 0; j <= count; j++) {
//			if (j == count) {
//				printf("%d", startblock + j);
//			}
//			else {
//
//				printf("%d,", startblock + j);
//			}
//		}
//	}
//}





// string分割函数
void FileManage::split(const string&s, vector<string>&v, const string&key)
{
	string::size_type pos1 = 0;
	string::size_type pos2 = s.find(key);
	while (pos2 != string::npos)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + key.size();
		pos2 = s.find(key, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

// 获取系统时间
string FileManage::getTime()
{
	struct tm t;   //tm结构指针
	time_t now;  //声明time_t类型变量
	time(&now);      //获取系统日期和时间
	localtime_s(&t, &now);   //获取当地日期和时间
	string time = "";
	string year, mon = "0", day = "0", hour = "0", min = "0", sec = "0";
	// 年
	year = std::to_string(t.tm_year + 1900);
	// 月
	if (t.tm_mon >= 0 && t.tm_mon <= 8) {
		mon.append(std::to_string(t.tm_mon + 1));
	}
	else {
		mon = std::to_string(t.tm_mon + 1);
	}
	// 日
	if (t.tm_mday > 0 && t.tm_mday <= 9) {
		day.append(std::to_string(t.tm_mday));
	}
	else {
		day = std::to_string(t.tm_mday);
	}
	// 时
	if (t.tm_hour >= 0 && t.tm_hour <= 9) {
		hour.append(std::to_string(t.tm_hour));
	}
	else {
		hour = std::to_string(t.tm_hour);
	}
	// 分
	if (t.tm_min >= 0 && t.tm_min <= 9) {
		min.append(std::to_string(t.tm_min));
	}
	else {
		min = std::to_string(t.tm_min);
	}
	//秒
	if (t.tm_sec >= 0 && t.tm_sec <= 9) {
		sec.append(std::to_string(t.tm_sec));
	}
	else {
		sec = std::to_string(t.tm_sec);
	}
	time.append(year).append(mon).append(day).append(hour).append(min).append(sec);
	return time;
}

// 计算dx
int FileManage::cal_dx(int curfcb) {
	curfcb = fcb_min;
	int i;
	do
	{
		for (i = fcb_min; i < BLKNUM; i++) {
			if (fcb[i].FCBNum == -1 || fcb[i].delFlag == true) {
				fcb_min = i;
				break;
			}
		}
	} while (i == BLKNUM);
	dx = fcb_min - curfcb;
	return dx;
}

// 改变目录(目标路径)
bool FileManage::cd(string str) {
	int fcb_tmp = fcb_cur;// 当前目录FCB号
	// cd ..
	if (str.compare("..") == 0) {
		if (fcb_cur == 0) {
			cout << "你已经在根目录下" << endl;
		}
		else {
			fcb_cur = fcb[fcb_tmp].fFCB;
			int tmp_size = strlen(fcb[fcb_tmp].fileName);
			if (fcb[fcb_tmp].fFCB != 0)
				tmp_size += 1;
			curpath.erase(curpath.size() - tmp_size);// 返回上一级
		}
		return true;
	}
	// cd .	路径处理
	vector<string>strArr;
	split(str, strArr, "/");

	if (strArr.size() == 1) {
		// cd 当前路径一级目录
		if (strArr[0].compare(minifile1) != 0) {
			if (fcb[fcb_tmp].sFCB != -1) {
				int fcb_flg;
				fcb_tmp = fcb[fcb_tmp].sFCB;
				int flag = 0;
				do {
					string sFileName(fcb[fcb_tmp].fileName);
					// 找到目标目录
					if (strArr[0].compare(sFileName) == 0) {
						if (fcb_cur != 0)
							curpath.append("/");
						curpath.append(sFileName);
						fcb_cur = fcb_tmp;
						flag = 1;
						break;
					}
					else {
						fcb_flg = fcb_tmp;
						fcb_tmp = fcb[fcb_tmp].rFCB;
					}
				} while (fcb_tmp != -1);
				// 目标目录不存在
				if (flag == 0) {
					cout << "系统找不到指定的路径" << endl;
					return false;
				}
			}
		}
		// 返回根目录
		else {
			fcb_cur = 0;
			curpath = minifile2;
		}
	}
	else {
		string path_tmp;
		int i;
		// cd 当前路径多级目录
		if (strArr[0].compare(minifile1) != 0) {
			for (i = 0; i < strArr.size(); i++) {
				int fcb_flg = fcb_tmp;
				if (fcb[fcb_tmp].sFCB != -1) {
					fcb_tmp = fcb[fcb_tmp].sFCB;
					int flag = 0;
					do {
						string sFileName(fcb[fcb_tmp].fileName);
						// 找到下一级目录
						if (strArr[i].compare(sFileName) == 0) {
							if (fcb[fcb_tmp].fFCB != 0)
								path_tmp.append("/");
							path_tmp.append(sFileName);
							flag = 1;
							break;
						}
						else {
							fcb_flg = fcb_tmp;
							fcb_tmp = fcb[fcb_tmp].rFCB;
						}
					} while (fcb_tmp != -1);
					// 下一级目录不存在
					if (flag == 0) {
						cout << "系统找不到指定的路径" << endl;
						return false;
					}
				}
				else {
					cout << "系统找不到指定的路径" << endl;
					return false;
				}
			}
			// 找到目标目录
			if (i == strArr.size()) {
				fcb_cur = fcb_tmp;
				curpath.append(path_tmp);
			}
		}
		// cd 根目录下多级目录(绝对路径)
		else {
			fcb_tmp = 0;
			for (i = 1; i < strArr.size(); i++) {
				if (fcb[fcb_tmp].sFCB != -1) {
					int fcb_flg;
					fcb_tmp = fcb[fcb_tmp].sFCB;
					int flag = 0;
					do {
						string sFileName(fcb[fcb_tmp].fileName);
						// 找到下一级目录
						if (strArr[i].compare(sFileName) == 0) {
							if (fcb[fcb_tmp].fFCB != 0)
								path_tmp.append("/");
							path_tmp.append(sFileName);
							flag = 1;
							break;
						}
						else {
							fcb_flg = fcb_tmp;
							fcb_tmp = fcb[fcb_tmp].rFCB;
						}
					} while (fcb_tmp != -1);
					// 下一级目录不存在
					if (flag == 0) {
						cout << "系统找不到指定的路径" << endl;
						return false;
					}
				}
				else {
					cout << "系统找不到指定的路径" << endl;
					return false;
				}
			}
			// 找到目标目录
			if (i == strArr.size()) {
				curpath = minifile1;
				fcb_cur = fcb_tmp;
				curpath.append(path_tmp);
			}
		}
	}
	return true;
}

// 当前目录(不为空)下创建文件夹
bool FileManage::mkdir_cur_not_empty(string strArr, int cur_tmp) {
	fcb_emp += cal_dx(cur_tmp);
	int fcb_flg;
	int fcb_tmp = fcb[cur_tmp].sFCB;
	int flag = 0;
	do {
		string sFileName(fcb[fcb_tmp].fileName);
		// 找到相同文件名
		if (strArr.compare(sFileName) == 0) {
			flag = 1;
			break;
		}
		else {
			fcb_flg = fcb_tmp;
			fcb_tmp = fcb[fcb_tmp].rFCB;
		}
	} while (fcb_tmp != -1);
	// 没有相同文件名
	if (flag == 0) {
		fcb_tmp = fcb_flg;
		fcb[fcb_tmp].rFCB = fcb_emp;
		fcb[fcb_emp].initialize();
		strcpy_s(fcb[fcb_emp].fileName, strArr.c_str());
		fcb[fcb_emp].fileType = true;
		string curtime = getTime();
		strcpy_s(fcb[fcb_emp].fileTime, curtime.c_str());
		fcb[fcb_emp].FCBNum = fcb_emp;
		fcb[fcb_emp].lFCB = fcb_tmp;
		fcb[fcb_emp].fFCB = cur_tmp;
	}
	// 有相同文件名
	else {
		tmp = fcb_tmp;
		return false;
	}
	fcbnum++;
	return true;
}

// 当前目录(为空)下创建文件夹
void FileManage::mkdir_cur_empty(string strArr, int cur_tmp) {
	fcb_emp += cal_dx(cur_tmp);
	fcb[cur_tmp].sFCB = fcb_emp;
	fcb[fcb_emp].initialize();
	strcpy_s(fcb[fcb_emp].fileName, strArr.c_str());
	fcb[fcb_emp].fileType = true;
	string curtime = getTime();
	strcpy_s(fcb[fcb_emp].fileTime, curtime.c_str());
	fcb[fcb_emp].FCBNum = fcb_emp;
	fcb[fcb_emp].fFCB = cur_tmp;
	fcbnum++;
}

// 创建子目录
bool FileManage::mkdir(string str) {
	int fcb_tmp = fcb_cur;// 当前目录FCB号
	// mkdir 路径处理
	vector<string>strArr;
	split(str, strArr, "/");

	// mkdir 当前目录下创建
	if (strArr.size() == 1) {
		// 当前目录不为空
		if (fcb[fcb_cur].sFCB != -1) {
			if (!mkdir_cur_not_empty(strArr[0], fcb_tmp)) {
				cout << strArr[0] << "文件名已存在" << endl;
				return false;
			}
		}
		// 当前目录为空
		else {
			mkdir_cur_empty(strArr[0], fcb_tmp);
		}
	}
	else {
		int i;
		// mkdir 当前多级目录下创建
		if (strArr[0].compare(minifile1) != 0) {
			for (i = 0; i < strArr.size(); i++) {
				if (fcb[fcb_tmp].sFCB != -1) {
					if (!mkdir_cur_not_empty(strArr[i], fcb_tmp) && i == strArr.size() - 1) {
						cout << strArr[i] << "文件名已存在" << endl;
						return false;
					}
					if (!mkdir_cur_not_empty(strArr[i], fcb_tmp) && i != strArr.size()) {
						fcb_tmp = tmp;
					}
				}
				else {
					mkdir_cur_empty(strArr[i], fcb_tmp);
					fcb_tmp = fcb_emp - dx;// 获取所要创建空文件夹的父目录的fcb块号
				}
			}
		}
		// mkdir 绝对路径创建
		else {
			fcb_tmp = 0;
			for (i = 1; i < strArr.size(); i++) {
				if (fcb[fcb_tmp].sFCB != -1) {
					if (!mkdir_cur_not_empty(strArr[i], fcb_tmp)) {
						return false;
					}
				}
				else {
					mkdir_cur_empty(strArr[i], fcb_tmp);
				}
				fcb_tmp = fcb_emp - dx;
			}
		}
	}
	return true;
}







//获取文件的大小
int FileManage::file_size(char* filename) {
	FILE *f = fopen(filename, "r");
	if (!f) return -1;
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	return size;
}

//创建新的文件
int FileManage::newFile(char filename[20]) {
	struct FCB newFcb;
	newFcb.initialize();
	struct FCB fcbPre = fcb[fcb_cur];
	//当前的这个节点没有子节点，说明创建的这个节点是当前节点的第一个子节点
	
	strcpy(newFcb.fileName, filename);
	newFcb.fileType = false;
	strcpy(newFcb.fileTime, "0");//之后讨论存取什么时间
	newFcb.delFlag = false;
	for (int i = 0; i < BLKNUM; i++) {
		if (fcb[i].delFlag == true || fcb[i].FCBNum == -1) {
			//找到第一个可以存fcb的地方
			newFcb.FCBNum = i;
			break;
		}
	}

	
	if (fcbPre.sFCB == -1) {
		fcb[fcb_cur].sFCB = newFcb.FCBNum;
		newFcb.fFCB = fcb[fcb_cur].FCBNum;
	}
	//当前这个节点有子节点，要找到当前节点的最后一个儿子节点
	else {
		//获取第一个兄弟节点
		struct FCB broFcb = fcb[fcbPre.sFCB];
		while (1) {
			//判断是否存在同名文件
			if (strcmp(broFcb.fileName, newFcb.fileName) == 0) {
				printf("该文件已存在！");
				return -1;
			}
			if (broFcb.rFCB == -1) {
				//找到了最后一个兄弟节点

				fcb[broFcb.FCBNum].rFCB = newFcb.FCBNum;
				newFcb.lFCB = broFcb.FCBNum;
				
				break;
			}
			//下一个兄弟节点为当前节点的右兄弟节点
			broFcb = fcb[broFcb.rFCB];
		}
	}
	newFcb.rFCB = -1;

	fcb[newFcb.FCBNum] = newFcb;
	return newFcb.FCBNum;
}
//打印目录，fcb_cur不改变
void FileManage::showDir()
{
	struct FCB currentFCB = fcb[fcb_cur];//该操作完成后
	if (currentFCB.sFCB == -1)
	{
		cout << "该目录下没有文件或文件夹" << endl;
	}
	else
	{
		currentFCB = fcb[currentFCB.sFCB];
		do
		{
			cout << currentFCB.fileName << endl;
			if (currentFCB.rFCB != -1)
				currentFCB = fcb[currentFCB.rFCB];
			else
				break;
		} while (1);
	}
}

//显示属性，fcb_cur不改变
void FileManage::showAtt(char* filename)
{
	struct FCB currentFCB = fcb[fcb_cur];//该操作完成后
	int flag = 0;
	if (currentFCB.sFCB == -1)//没有子文件
	{
		cout << "没有找到该文件或文件夹" << endl;
	}
	else//存在子文件
	{
		currentFCB = fcb[currentFCB.sFCB];
		do
		{
			int match = match_string(filename, currentFCB.fileName);
			if (match == 0)//文件名匹配成功
			{
				char *str = new char[8];
				if (currentFCB.fileType == 0)
					strcpy(str, "文件  ");
				else
					strcpy(str, "文件夹");
				char time[20];
				memset(time, 0, sizeof(time));
				int j = 0;
				for (int i = 0; i < 4; i++)
				{
					time[j] = currentFCB.fileTime[i];
					j++;
				}
				time[j] = '/';
				j++;
				for (int i = 4; i < 6; i++)
				{
					time[j] = currentFCB.fileTime[i];
					j++;
				}
				time[j] = '/';
				j++;
				for (int i = 6; i < 8; i++)
				{
					time[j] = currentFCB.fileTime[i];
					j++;
				}
				time[j] = ' ';
				j++;
				for (int i = 8; i < 10; i++)
				{
					time[j] = currentFCB.fileTime[i];
					j++;
				}
				time[j] = ':';
				j++;
				for (int i = 10; i < 12; i++)
				{
					time[j] = currentFCB.fileTime[i];
					j++;
				}
				time[j] = ':';
				j++;
				for (int i = 12; i < 14; i++)
				{
					time[j] = currentFCB.fileTime[i];
					j++;
				}
				time[j] = '\0';
				
				printf("%-16s", currentFCB.fileName);
				printf("%6d bytes      ", currentFCB.fileSize);
				cout << str << "      ";
				cout << time << "      ";
				cout << currentFCB.fileContent << "      " << endl;
				flag++;
			}
			if (currentFCB.rFCB != -1)
				currentFCB = fcb[currentFCB.rFCB];
			else
				break;
		} while (1);
		if (flag == 0)
		{
			cout << "没有找到该文件或文件夹" << endl;
		}
	}
}

/*write by zm*/
//写入文件内容（已知filename）
void FileManage::write(char *filename)
{
	int bitmapflag, blockNum;														//bitmapflag：标识bitmap起始；blocknum:需要存入的盘块数目
	int charcount = 0;																//charcount:记录输入的长度;
	char tmpChar;
	int fcb_son = fcb[fcb_cur].sFCB;												//当前目录的子节点对应的fcb下标号，即孩子节点所在层级
	if (fcb_son == -1) {
		printf("当前目录下不存在此文件，请先创建。\n");
		return;
	}
	else {
		//查找内容块对应的盘块号(ibNum)
		for (;; fcb_son = fcb[fcb_son].rFCB)										//同级移动查找
		{
			if (strcmp(fcb[fcb_son].fileName, filename) == 0) {
				if (fcb[fcb_son].fileType != 0) {
					cout << "无法写入文件夹，请输入文件名。" << endl;
					return;
				}
				if (fcb[fcb_son].fileContent != -1)									//如果原来就有内容
				{
					char judgeChar;
					cout << "重新写入会覆盖原有内容，输入y/n以确认你的操作：" << endl;
					judgeChar = getchar();
					getchar();														//吃掉y/n后面的回车
					if (judgeChar == 'n' || judgeChar == 'N')
						return;
				}
				int count = 0, flag = 0;
				int contentStart;
				cout << "(O_O) 请输入你的内容，并以#作为结束标识：" << endl;
				printf("input:/>");
				//getchar();
				for (int i = 0; i < sizeof(content); i++)
				{
					tmpChar = getchar();
					charcount++;
					if (tmpChar == '#')
					{
						getchar();													//adapt to lc
						content[i] = '\0';
						break;
					}
					content[i] = tmpChar;

				}
				fcb[fcb_son].fileSize = charcount;
				blockNum = fcb[fcb_son].fileSize / 4096 + 1;
				for (int i = 0; i < BLKNUM; i++) {
					if (bitmap[i] == false) {
						//找到第一个为空的盘块;
						count = 1;
						bitmapflag = i, contentStart = i + 4105;
						if (count == blockNum) {
							flag = 1;
							break;
						}
						for (int j = i + 1; j < BLKNUM; j++) {
							if (bitmap[j] == false) {
								count++;
								if (count == blockNum) {
									flag = 1;
									break;
								}
							}
							else {
								count = 0;
								break;
							}
						}
						if (flag = 1) {
							break;
						}
					}
				}
				//首先需要修改bitmap的值
				for (int i = bitmapflag; i < bitmapflag + blockNum; i++)
				{
					bitmap[i] = true;
				}
				//找到contentStart（磁盘其实序号），将其赋给fileContent
				fcb[fcb_son].fileContent = contentStart;
				//写入内容到磁盘

				FILE *fpp = fopen(sysname, "r+");
				fseek(fpp, contentStart*BLKSIZE, SEEK_SET);
				fwrite(content, fcb[fcb_son].fileSize, 1, fpp);
				fclose(fpp);
				break;
			}
			if (fcb[fcb_son].rFCB == -1)
			{
				cout << "当前目录下不存在此文件，请先创建。" << endl;
				return;
			}
		}
	}
}

//将IB盘块号为blocknum的读入content
void FileManage::read(int fcbnum, int blocknum)
{
	FILE *fpp = fopen(sysname, "r");
	//定位到盘块号的起始指针
	fseek(fpp, BLKSIZE * blocknum, SEEK_SET);
	//将内容读取到缓冲区
	fread(content, 1, fcb[fcbnum].fileSize, fpp);
	fclose(fpp);
}

//显示文件内容
void FileManage::show(char *filename)
{
	int fcbFlag;													//内容块对应的fcb的序号
	int ibnum;														//内容文件对应的盘块号									
	int fcb_son = fcb[fcb_cur].sFCB;								//当前目录的子节点对应的fcb下标号
	if (fcb_son == -1) {
		printf("当前目录下不存在此文件，请先创建。\n");
		return;
	}
	else {
		//查找内容块对应的盘块号(ibnum)
		for (;; fcb_son = fcb[fcb_son].rFCB)//同级移动查找
		{
			fcbFlag = fcb_son;
			if (strcmp(fcb[fcb_son].fileName, filename) == 0) {
				if (fcb[fcb_son].fileType != 0) {
					cout << "无法读取文件夹，请输入文件名。" << endl;
					return;
				}
				if (fcb[fcb_son].fileContent == -1)
				{
					cout << "文件内容为空，请先写入内容。" << endl;
					return;
				}
				ibnum = fcb[fcb_son].fileContent;
				break;
			}
			//退出条件：
			if (fcb[fcb_son].rFCB == -1)
			{
				cout << "当前目录下不存在此文件，请先创建。" << endl;
				return;
			}
		}
		read(fcbFlag, ibnum);
		cout << content << endl;
	}
}

//查看当前文件所占的盘块号
void FileManage::map(char *filename)
{
	int fcbFlag, blocknum;											//内容块对应的fcb的序号
	int ibnum;														//内容文件对应的盘块号									
	int fcb_son = fcb[fcb_cur].sFCB;								//当前目录的子节点对应的fcb下标号
	if (fcb_son == -1) {
		cout << "当前目录下不存在此文件，请先创建。" << endl;
		return;
	}
	else {
		//查找内容块对应的盘块号(ibnum)
		for (;; fcb_son = fcb[fcb_son].rFCB)//同级移动查找
		{
			fcbFlag = fcb_son;
			if (strcmp(fcb[fcb_son].fileName, filename) == 0) {
				if (fcb[fcb_son].fileType != 0) {
					cout << "文件夹无法执行map操作，请输入文件名。" << endl;
					return;
				}
				if (fcb[fcb_son].fileContent == -1) {
					cout << "当前文件为空，未进行分配磁盘。" << endl;
					return;
				}
				ibnum = fcb[fcb_son].fileContent;
				break;
			}
			if (fcb[fcb_son].rFCB == -1)
			{
				cout << "当前目录下不存在此文件，请先创建。" << endl;
				return;
			}
		}
		//起始盘块号 ：ibnum，fcb[fcbFlag].fileSize 就是所占几个盘块号 
		cout << fcb[fcbFlag].fileName << " 所占内容块的盘块号: " << endl;
		blocknum = fcb[fcbFlag].fileSize / 4096 + 1;
		for (int i = 0; i < blocknum; i++)
		{
			printf("%d ", ibnum++);
		}
		printf("\n");
	}
}

//查找文件、文件夹的fcb序列号，不存在就返回-1
int FileManage::fcbSearch(char *filename)
{
	int columnFcbNum = fcb[0].sFCB;
	//cout << "第一个columnFcbNum: "<<columnFcbNum << endl;
	if (columnFcbNum == -1)
		return -1;//不存在
	else
	{
		//bfs查找
		for (;; columnFcbNum = fcb[columnFcbNum].sFCB)
		{
			int rowFcbNum = columnFcbNum;
			for (;; rowFcbNum = fcb[rowFcbNum].rFCB)
			{
				//cout << fcb[rowFcbNum].fileName << endl;
				if (strcmp(fcb[rowFcbNum].fileName, filename) == 0)
				{
					//cout << "目录的fcb下标是： " << rowFcbNum << endl;
					return rowFcbNum;
				}
				if (fcb[rowFcbNum].rFCB == -1)
					break;
			}
			if (fcb[columnFcbNum].sFCB == -1)
				break;
		}
		return -1;//不存在
	}
}

//将当前路径下的文件移动到新的路径中
void FileManage::move(char *filename, char *dirname)
{
	int fileflag = -1;
	int dirflag = 0;
	int moveflag;
	//首先查找当前目录下是否存在此文件,tempFcbNum（fileflag）是需要移动的文件fcb下标
	int tempFcbNum = fcb[fcb_cur].sFCB;
	if (tempFcbNum == -1) {
		printf("当前目录为空，请重新操作。\n");
		return;
	}
	for (;; tempFcbNum = fcb[tempFcbNum].rFCB)
	{
		//cout << "当前文件名：" << fcb[tempFcbNum].fileName << endl;
		if (strcmp(fcb[tempFcbNum].fileName, filename) == 0)
		{
			fileflag = tempFcbNum;
			break;//存在文件就退出循环
		}
		if (fcb[tempFcbNum].rFCB == -1)
		{
			printf("不存在此文件，请重新输入。\n");
			return;
		}
	}
	//fileflag是要移动的文件的fcb号，dirflag是目录的fcb号
	dirflag = fcbSearch(dirname);
	if (dirflag == -1)
	{
		printf("不存在该目录，请新建或重新输入。\n");
		return;
	}
	else
	{
		//首先得修改原有fileflag文件的父元素的sFCB成另一个,得有三种情况的位置判断
		if (fcb[fileflag].lFCB == -1)							//当前层最左元素
		{
			int ffcbnum = fcb[fileflag].fFCB;
			int rfcbnum = fcb[fileflag].rFCB;
			fcb[ffcbnum].sFCB = rfcbnum;
			fcb[rfcbnum].lFCB = -1;
		}
		else if (fcb[fileflag].rFCB == -1)						//当前层最右元素
		{
			int lfcbnum = fcb[fileflag].lFCB;
			fcb[lfcbnum].rFCB = -1;
		}
		else
		{
			int lfcbnum = fcb[fileflag].lFCB;					//当前层中间元素
			int rfcbnum = fcb[fileflag].rFCB;
			fcb[lfcbnum].rFCB = rfcbnum;
			fcb[rfcbnum].lFCB = lfcbnum;
		}
		if (fcb[dirflag].sFCB == -1)							//所移到的目录下没有文件
		{
			fcb[fileflag].fFCB = dirflag;
			fcb[fileflag].lFCB = -1;
			fcb[fileflag].rFCB = -1;
			fcb[dirflag].sFCB = fileflag;
		}
		else {													//目录下有文件
			moveflag = fcb[dirflag].sFCB;
			while (fcb[moveflag].rFCB != -1)					//移动moveflag直到右兄弟是空
			{
				moveflag = fcb[moveflag].rFCB;
			}
			fcb[fileflag].fFCB = dirflag;
			fcb[fileflag].lFCB = moveflag;
			fcb[fileflag].rFCB = -1;
			fcb[moveflag].rFCB = fileflag;
		}
		cout << "移动成功！" << endl;
		return;
	}
}


//删除文件，fcb_cur改变为被删除文件的父节点

bool FileManage::match_string(const char* str, const char* strpattern)
{
	int nStr = strlen(str);
	int nPatt = strlen(strpattern);
	int** pTable = new int*[nStr + 1];
	for (int k = 0; k <= nStr; k++) {
		pTable[k] = new int[nPatt + 1];
		memset(pTable[k], 0, (nPatt + 1) * sizeof(int));
	}

	if (strpattern[0] == '*')
	{
		for (int i = 0; i <= nPatt; ++i)
		{
			pTable[0][i] = 1;
		}
	}
	pTable[0][0] = 1;

	for (int j = 1; j <= nPatt; ++j)
	{
		for (int i = 1; i <= nStr; ++i)
		{
			if ((strpattern[j - 1] == '?' && str[i - 1] != '\0') || strpattern[j - 1] == str[i - 1]) {
				pTable[i][j] = pTable[i - 1][j - 1];
			}
			else if (strpattern[j - 1] == '*') {
				if (pTable[i][j - 1] == 1 || pTable[i - 1][j] == 1 || pTable[i - 1][j - 1] == 1)
					pTable[i][j] = 1;
			}
		}
	}

	bool ret = (pTable[nStr][nPatt] == 1 ? true : false);
	for (int k = 0; k <= nStr; k++)
		delete[] pTable[k];
	delete pTable;

	return ret;
}
bool FileManage::delFile(char* filename)
{
	int cur = fcb_cur;//记录当前目录
	char command;// "y" or "n"
	int check = 0;//是否有路径
	int len = strlen(filename);
	char path[100];//路径
	string file;//被删除文件名
	for (int i = len - 1; i >= 0; i--)
	{
		if (filename[i] == '/')
		{
			len = i + 1;
			file = filename;
			strncpy(path, filename, len - 1);
			file = file.substr(len);
			//cout << path << endl;
			//cout << file.c_str() << endl;
			path[len - 1] = '\0';
			if (cd(path) == 0)
				return 0;
			check = 1;
			break;
		}
	}
	if (check == 0)
	{
		file = filename;
		//cout << file.c_str() << endl;
	}
	struct FCB *currentFCB;
	struct FCB *fatherFCB = &fcb[fcb_cur];//被删除文件的父节点

	if (fatherFCB->sFCB != -1)
	{
		currentFCB = &fcb[fatherFCB->sFCB];
		do {
			if (match_string(currentFCB->fileName, file.c_str()))
			{
				dFlag++;
				currentFCB->fFCB = -1;
				//改变FCB连接
				if (currentFCB->lFCB != -1)//不是该层第一个节点
				{
					struct FCB *leftFCB = &fcb[currentFCB->lFCB];
					if (currentFCB->rFCB != -1)//不是该层最后一个节点
					{
						struct FCB *rightFCB = &fcb[currentFCB->rFCB];
						leftFCB->rFCB = rightFCB->FCBNum;
						rightFCB->lFCB = leftFCB->FCBNum;
					}
					else//该层最后一个节点
					{
						leftFCB->rFCB = -1;
					}
				}
				else//该层第一个节点
				{
					if (currentFCB->rFCB != -1)//不是该层最后一个节点
					{
						struct FCB *rightFCB = &fcb[currentFCB->rFCB];
						fcb[fcb_cur].sFCB = rightFCB->FCBNum;
						rightFCB->lFCB = -1;
					}
					else//该层最后一个节点
					{
						fcb[fcb_cur].sFCB = -1;
					}
				}
				//改变FCB删除标记、盘块删除标记
				if (currentFCB->fileType == 0)//删除的为单个文件
				{
					currentFCB->delFlag = 1;
					fcbnum--;
					int blockNum = 0;
					blockNum = ceil(currentFCB->fileSize / BLKSIZE);
					for (int i = currentFCB->FCBNum; i < blockNum; i++)
						bitmap[i] = 0;
					if (currentFCB->FCBNum < delMin)
						delMin = currentFCB->FCBNum;
					while (fcb_cur != cur)
						cd("..");
				}
				else//删除一个文件夹
				{
					currentFCB->delFlag = 1;
					fcbnum--;
					if (currentFCB->FCBNum < delMin)
						delMin = currentFCB->FCBNum;
					if (currentFCB->sFCB != -1)//子节点不为空
					{
						currentFCB = &fcb[currentFCB->sFCB];
						do
						{
							currentFCB->delFlag = 1;
							fcbnum--;
							if (currentFCB->FCBNum < delMin)
								delMin = currentFCB->FCBNum;
							if (!delFile(currentFCB->fileName))
							{
								cout << "删除文件" << currentFCB->fileName << "不成功" << endl;
							}
							if (currentFCB->rFCB != -1)
								currentFCB = &fcb[currentFCB->rFCB];
							else
								break;
						} while (1);
						while (fcb_cur != cur)
							cd("..");
					}
					else//子节点为空
					{
						while (fcb_cur != cur)
							cd("..");
					}
				}
			}
			if (currentFCB->rFCB != -1)
				currentFCB = &fcb[currentFCB->rFCB];
			else
				break;
		} while (1);
		if (dFlag != 0)
			return 1;
		else
			return 0;
	}
	else
	{
		return 0;
	}
}

//遍历该文件夹下所有文件
void FileManage::tree()
{
	const char *ch1 = "│", *ch2 = "└──", *ch3 = "├──";
	struct FCB currentFCB = fcb[fcb_cur];
	do
	{
		if (layer >= 2)
		{
			if (fcb[currentFCB.fFCB].rFCB == -1)
				layers[layer - 2] = 0;
			else
				layers[layer - 2] = 1;
		}

		for (int i = 0; i < layer; i++)
		{
			if (i == layer - 1)
			{
				if (currentFCB.rFCB == -1)
				{
					cout << ch2;
				}
				else
					cout << ch3;
			}
			else if (layers[i])
				cout << ch1 << "  ";
			else
				cout << "   ";
		}
		cout << currentFCB.fileName << endl;

		if (currentFCB.sFCB != -1)
		{
			currentFCB = fcb[currentFCB.sFCB];
			cd(currentFCB.fileName);
			layer++;
			tree();
			layer--;
			currentFCB = fcb[fcb_cur];
		}
		else
		{
			//cout << "该目录下没有文件或文件夹" << endl;
		}
		if (currentFCB.rFCB != -1 && layer > 0)
		{
			currentFCB = fcb[currentFCB.rFCB];
			cd("..");
			cd(currentFCB.fileName);
		}
		else//对该层最后一个节点进行处理
		{
			if (layer == 0)
				break;
			/*if (currentFCB.sFCB != -1)
			{
				currentFCB = fcb[currentFCB.sFCB];
				cd(currentFCB.fileName);
				layer++;
				tree();
				layer--;
				currentFCB = fcb[fcb_cur];
			}
			else
			{
				cd("..");
				currentFCB = fcb[fcb_cur];
				return;
			}*/
			cd("..");
			currentFCB = fcb[fcb_cur];
			break;
		}
	} while (1);
}
//获取文件在磁盘中所占用的所有的盘块
//void FileManage::getMap(string str) {
//	//读取文件的路径获取文件的fcb的值
//	if (cd(str)) {
//		int startblock = fcb[fcb_cur].fileContent;//获取第一个盘块
//		int count = fcb[fcb_cur].fileSize / 4096;//获取所占的盘块数
//		for (int j = 0; j <= count; j++) {
//			if (j == count) {
//				printf("%d", startblock + j);
//			}
//			else {
//
//				printf("%d,", startblock + j);
//			}
//		}
//	}
//	else {
//		printf("该文件不存在");
//	}
//}

int FileManage::judge_exist(string filename) {
	struct FCB father = fcb[fcb_cur];//获取当前父节点的fcb
	if (father.sFCB == -1) {
		//该目录下没有子节点
		return -1;
	}
	else {
		struct FCB broFcb = fcb[father.sFCB];//获取第一个儿子节点
		while (1) {
			if (strcmp(broFcb.fileName, filename.c_str()) == 0) {
				//两个名字相同
				return broFcb.FCBNum;//返货该节点的fcb序号
			}
			else {
				if (broFcb.rFCB == -1) break;//到了最后一个fcb节点
			}

			broFcb = fcb[broFcb.rFCB];
		}
	}
	return -1;

}

bool FileManage::copy(string filename1, string filename2) {
	string first;
	string second;
	first = filename1.substr(0, 1);//获取首字母
	second = filename2.substr(0, 1);

	if (first == "D" || first == "C" || first == "E") {
		//第一个文件名是windows系统下的文件名
		int flag = judge_exist(filename2);
		if (flag != -1) {
			printf("在当前目录下该文件已存在，如果要覆盖当前文件请输入y,不覆盖输入n\n");
			char command;
			cin >> command;
			if (command == 'y') {
				//用户选择覆盖
				char c[14]="";
				strcpy(c, filename1.c_str());
				int sum = file_size(c);//获取文件的大小
				struct FCB prefcb = fcb[flag];//获取要覆盖的fcb
				int preSize = prefcb.fileSize;//获取要覆盖的fcb的大小
				int preBitMapstart = prefcb.fileContent;//获取要覆盖的fcb的文件内容起始序号
				int countpre = (preSize / 4096) + 1;//获取文件内容块的数量
				if (preSize > 0) {
					for (int i = preBitMapstart; i < countpre; i++) {
						bitmap[i] = false;//将位视图被占用的地方重新至为false
					}
				}

				int count = (sum / 4096) + 1;//获取现在该文件索要占的盘块数

				int f = 0;
				int nowcount = 0;
				int i;
				for (i = 0; i < BLKNUM; i++) {
					if (bitmap[i] == false) {
						nowcount++;
						if (nowcount == count) {
							f = 1;
							break;
						}
						for (int j = 1; j < count; j++) {
							if (bitmap[j + i] != false) {
								break;
							}
							else {
								nowcount++;
							}
						}
						if (nowcount == count ) {
							f = 1;
							break;
						}
						else {
							nowcount = 0;
						}
					}
				}

				if (f == 0) {
					printf("空间不足");
					return false;
				}
				else {
					fcb[flag].fileContent = i+4105;//给找到的第一文件内容块赋值
					fcb[flag].fileSize = sum;
					for (int j = 0; j < count; j++) {
						bitmap[j + i] = true;
					}
					//开始写入数据
					char temp[4096];
					int remaining = sum;
					FILE *fp = fopen(c, "r");//读取文件
					FILE *fw = fopen(sysname, "w+");//打开系统文件
					for (int j = 0; j < count; j++) {
						if (remaining >= 4096) {
							fread(temp, 1, 4096, fp);//将4096个字符读入内存
							fseek(fw, (9 + i + j + 4096) * 4096, SEEK_SET);//偏移到当前盘块
							fwrite(temp,  4096,1, fw);//将内容写入
							remaining -= 4096;
						}
						else {
							fread(temp, 1, remaining, fp);//将4096个字符读入内存
							fseek(fw, (9 + i + j + 4096) * 4096, SEEK_SET);//偏移到当前盘块
							fwrite(temp,  remaining, 1,fw);//将内容写入
							remaining -= remaining;
						}

					}
					fclose(fp);
					fclose(fw);
					return true;
				}


			}
			else {
				return false;
			}
		}
		else {
			//说明在当前目录下该文件不存在
			char h[14]="";
			strcpy(h, filename2.c_str());
			int newFcbNum = newFile(h);
			char c[14]="";
			strcpy(c, filename1.c_str());
			int sum = file_size(c);//获取文件的大小
			int count = (sum / 4096) + 1;//获取现在该文件索要占的盘块数

			int f = 0;
			int nowcount = 0;
			int i;
			for (i = 0; i < BLKNUM; i++) {
				if (bitmap[i] == false) {
					nowcount++;
					if (nowcount == count ) {
						f = 1;
						break;
					}
					for (int j = 1; j < count; j++) {
						if (bitmap[j + i] != false) {
							break;
						}
						else {
							nowcount++;
						}
					}
					if (nowcount == count ) {
						f = 1;
						break;
					}
					else {
						nowcount = 0;
					}
				}
			}
			if (f == 0) {
				printf("空间不足");
				return false;
			}
			else {
				fcb[newFcbNum].fileContent = i+4105;//给找到的第一文件内容块赋值
				fcb[newFcbNum].fileSize = sum;
				for (int j = 0; j < count; j++) {
					bitmap[j + i] = true;
				}


				//开始写入数据
				char temp[4096];
				int remaining = sum;
				FILE *fp = fopen(c, "r");//读取文件
				FILE *fw = fopen(sysname, "w+");//打开系统文件
				for (int j = 0; j < count; j++) {
					if (remaining >= 4096) {
						fread(temp, 1, 4096, fp);//将4096个字符读入内存
						fseek(fw, (9 + i + j+4096)*4096, SEEK_SET);//偏移到当前盘块
						fwrite(temp, 4096, 1,fw);//将内容写入
						remaining -= 4096;
					}
					else {
						fread(temp, 1, remaining, fp);//将4096个字符读入内存
						fseek(fw,( 9 + i + j + 4096)*4096 , SEEK_SET);//偏移到当前盘块
						fwrite(temp, remaining, 1,fw);//将内容写入
						remaining -= remaining;
					}

				}
				fclose(fp);
				fclose(fw);
				return true;
			}

		}
	}
	else if (second == "D" || second == "C" || second == "E") {
		//第二个文件名是windows系统下的文件名
		int flag = judge_exist(filename1);
		if (flag == -1) {
			printf("被复制的该文件不存在");
			return false;
		}
		else {
			//被复制的这个文件存在
			char y[14]="";
			strcpy(y, filename2.c_str());
			struct FCB firstFcb = fcb[flag];//获取被复制文件的fcb
			int firstSize = firstFcb.fileSize;//获取被复制文件的大小
			int count = (firstSize / 4096) + 1;
			int firstStartContent = firstFcb.fileContent;
			char temp[4096];
			int remaining = firstSize;
			FILE *fw = fopen(y, "w+");
			FILE *fp = fopen(sysname, "r");//以只读的形式打开当前系统文件
			for (int i = 0; i < count; i++) {
				if (remaining >= 4096) {
					fseek(fp, 4096 * (i + firstStartContent), SEEK_SET);
					fread(temp, 1, 4096, fp);
					fwrite(temp, 1, 4096, fw);
					remaining -= 4096;
				}
				else {
					fseek(fp, 4096 * (i + firstStartContent), SEEK_SET);
					fread(temp, 1, remaining, fp);
					fwrite(temp, 1, remaining, fw);
					remaining -= remaining;
				}
			}
			fclose(fp);
			fclose(fw);
			return true;
		}
	}
	else {
		//两个文件都是该系统下的

		int flag1 = judge_exist(filename1);
		int flag2 = judge_exist(filename2);
		if (flag1 == -1) {
			printf("被复制的文件不存在");
			return false;
		}
		else {
			//被复制的文件存在
			if (flag2 != -1) {
				//复制到的文件存在
				printf("该文件已存在，是否覆盖？覆盖的话输入y,不覆盖的话输入n\n");
				char  command;
				cin >> command;
				if (command == 'y') {
					struct FCB firstFile = fcb[flag1];//获取第一个文件的fcb
					struct FCB secondFile = fcb[flag2];//获取第二个文件的fcb
					int count = 0;
					if (secondFile.fileSize > 0) {
						count = (secondFile.fileSize / 4096) + 1;
						for (int j = secondFile.fileContent; j < count + secondFile.fileContent; j++) {
							bitmap[j] = false;
						}
					}
	
					count = (firstFile.fileSize / 4096) + 1;
					int i;
					int newcount = 0;
					for (i = 0; i < BLKNUM; i++) {
						if (bitmap[i] == false) {
							newcount++;
							if (newcount == count) {
								break;
							}
							for (int j = 1; j < count; j++) {
								if (bitmap[i + j] == false) {
									newcount++;
								}
								else {
									break;
								}
							}
							if (newcount == count) {
								break;
							}
							else {
								newcount = 0;
							}
						}
					}

					for (int j = 0; j < count; j++) {
						bitmap[j + i] = true;
					}
					fcb[flag2].fileSize = fcb[flag1].fileSize;
					fcb[flag2].fileContent = i+4105;
					secondFile.fileContent = i+4105;

					char temp[4096]="";
					FILE *fp = fopen(sysname, "r+");
					int remaining = fcb[flag1].fileSize;
					for (int j = 0; j < count; j++) {
						if (remaining >= 4096) {
							fseek(fp, (j + firstFile.fileContent) * 4096, SEEK_SET);
							fread(temp, 1, 4096, fp);
							fseek(fp, (j + secondFile.fileContent ) * 4096, SEEK_SET);
							fwrite(temp, 1, 4096, fp);
							remaining -= 4096;
						}
						else {
							fseek(fp, (j + firstFile.fileContent) * 4096, SEEK_SET);
							fread(temp, 1, remaining, fp);
							fseek(fp, (j + secondFile.fileContent ) * 4096, SEEK_SET);
							fwrite(temp, 1, remaining, fp);
							remaining -= remaining;
						}
					}

					fclose(fp);
					return true;
				}
				else {
					return false;
				}

			}
			//复制到文件不存在
			else {

				char c[14]="";
				strcpy(c, filename2.c_str());
				flag2 = newFile(c);
				struct FCB firstFile = fcb[flag1];//获取第一个文件的fcb
				struct FCB secondFile = fcb[flag2];//获取第二个文件的fcb
				int firstSize = fcb[flag1].fileSize;
				int count = (firstSize / 4096) + 1;
				int i;
				int newcount = 0;
				for (i = 0; i < BLKNUM; i++) {
					if (bitmap[i] == false) {
						newcount++;
						if (newcount == count) {
							break;
						}
						for (int j = 1; j < count; j++) {
							if (bitmap[i + j] == false) {
								newcount++;
							}
							else {
								break;
							}
						}
						if (newcount == count) {
							break;
						}
						else {
							newcount = 0;
						}
					}
				}
				for (int j = 0; j < count; j++) {
					bitmap[j + i] = true;
				}

				fcb[flag2].fileSize = fcb[flag1].fileSize;
				fcb[flag2].fileContent = i+4105;
				secondFile.fileContent = i+4105;

				char temp[4096]="";
				FILE *fp = fopen(sysname, "r+");
				int remaining = fcb[flag1].fileSize;
				for (int j = 0; j < count; j++) {
					if (remaining >= 4096) {
						fseek(fp, ( j + firstFile.fileContent) * 4096, SEEK_SET);
						fread(temp, 1, 4096, fp);
						fseek(fp, (j + secondFile.fileContent) * 4096, SEEK_SET);
						fwrite(temp, 1, 4096, fp);
						remaining -= 4096;
					}
					else {
						fseek(fp, (j + firstFile.fileContent)*4096, SEEK_SET);
						fread(temp, 1, remaining, fp);
						fseek(fp, (j + secondFile.fileContent)* 4096, SEEK_SET);
						fwrite(temp, remaining,1, fp);
						remaining -= remaining;
					}
				}

				fclose(fp);
				return true;
			}
		}



	}
	return false;
}