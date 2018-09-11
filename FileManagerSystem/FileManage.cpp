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
using namespace std;

/*全局变量定义*/
char content[BLKSIZE * 10];//文件内容


FileManage::FileManage()
{
	fp = NULL;
	baseAddr = (char*)malloc(sizeof(char) * DISKSIZE);
	memset(baseAddr, 0, DISKSIZE);
	fcbnum = 1;
	fcb_min = 1;
	fcb_emp = 1;
	fcb_cur = 0;
	string rootName = "root";
	fcb[0].initialize();
	strcpy_s(fcb[0].fileName, rootName.c_str());
	fcb[0].FCBNum = 0;
	fcb[0].fileType = true;
	string curtime = getTime();
	strcpy_s(fcb[0].fileTime, curtime.c_str());
	string minifile(systemName);						//临时的空间名
	string minifile1 = minifile.append(":");			//临时的空间名:
	string minifile2 = minifile1.append("/"); 			//临时的空间名:"\"
	curpath = minifile2;
	dx = 1;
	for (int i = 1; i < BLKNUM; i++)
		fcb[i].initialize();
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
	string symPath = "D:/";
	symPath += name;
	//d:\\zm
	strcpy(systemName, "d:\\");
	strcat(systemName, name);
	fp = fopen(symPath.c_str(), "r+");
	if (fp == NULL)
	{
		return 0;
	}
	fseek(fp, 0,SEEK_SET);
	fread(&fcbnum,4, 1, fp);
	fseek(fp, 4096, SEEK_SET);
	fread(Map, 1, BLKNUM/8 ,fp);


	for (int i = 0; i < BLKNUM / 8; i++)
	{
		for(int j=0; j < 8; j++)
		{
			bitmap[8 * i + j] = (Map[i] & (1 << j)) / (1 << j);
		}
	}
	for (int i = 0; i < BLKNUM; i++)
	{
		fread(&fcb[i], 1, 64, fp);	
	}
	fcb[0].FCBNum = 0;
	/*getFcbArray();*/
}
//需加上对fcb的重新写入
int FileManage::close()
{
	char Map[32768];
	string symPath = "D:";
	symPath += systemName;
	fp = fopen(symPath.c_str(), "r+");
	fseek(fp, 0, SEEK_SET);
	fwrite(&fcbnum, sizeof(int), 1, fp);
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
		//读取文件名
		fwrite(fcb[i].fileName, 1, 14, fp);
		//读取文件大小
		fwrite(&fcb[i].fileSize, 4, 1, fp);
		//写入文件类型
		fwrite(&fcb[i].fileType, 1, 1, fp);
		//读取文件修改时间
		fwrite(fcb[i].fileTime, 1, 20, fp);
		//读取文件删除标志
		fwrite(&fcb[i].delFlag , 1, 1, fp);
		//读取文件FCB的位置
		fwrite(&fcb[i].FCBNum, 4, 1, fp);
		//读取文件内容起始盘块号
		fwrite(&fcb[i].fileContent, 4, 1, fp);
		//读取文件父节点fcb
		fwrite(&fcb[i].fFCB, 4, 1, fp);
		//读取文件左兄弟节点fcb
		fwrite(&fcb[i].lFCB, 4, 1, fp);
		//读取文件右兄弟节点fcb
		fwrite(&fcb[i].rFCB, 4, 1, fp);
		//读取文件子节点fcb
		fwrite(&fcb[i].rFCB, 4, 1, fp);
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
		for (i = fcb_min + 1; i < BLKNUM; i++) {
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
			while (fcb[fcb_tmp].lFCB != -1) // 找到上一级目录的子节点
			{
				fcb_tmp = fcb[fcb_tmp].lFCB;
			}
			fcb_cur = fcb[fcb_tmp].fFCB;
			int tmp_size = strlen(fcb[fcb_tmp].fileName);
			curpath.erase(curpath.size() - tmp_size - 1);// 返回上一级
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
				int fcb_flg;
				if (fcb[fcb_tmp].sFCB != -1) {
					fcb_tmp = fcb[fcb_tmp].sFCB;
					int flag = 0;
					do {
						string sFileName(fcb[fcb_tmp].fileName);
						// 找到下一级目录
						if (strArr[i].compare(sFileName) == 0) {
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
							path_tmp.append("\\");
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
	}
	return true;
}

// 当前目录(不为空)下创建文件夹
bool FileManage::mkdir_cur_not_empty(string strArr, int cur_tmp) {
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
		strcpy_s(fcb[fcbnum].fileTime, curtime.c_str());
		fcb[fcb_emp].FCBNum = fcb_emp;
		fcb[fcb_emp].lFCB = fcb_tmp;
	}
	// 有相同文件名
	else {
		tmp = fcb_tmp;
		return false;
	}
	fcb_emp += cal_dx(cur_tmp);
	fcbnum++;
	return true;
}

// 当前目录(为空)下创建文件夹
void FileManage::mkdir_cur_empty(string strArr, int cur_tmp) {
	fcb[cur_tmp].sFCB = fcb_emp;
	fcb[fcb_emp].initialize();
	strcpy_s(fcb[fcb_emp].fileName, strArr.c_str());
	fcb[fcb_emp].fileType = true;
	string curtime = getTime();
	strcpy_s(fcb[fcbnum].fileTime, curtime.c_str());
	fcb[fcb_emp].FCBNum = fcb_emp;
	fcb[fcb_emp].fFCB = cur_tmp;
	fcb_emp += cal_dx(cur_tmp);
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
bool FileManage::newFile(char filename[20]) {
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
				return false;
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
	return true;
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
			flag = strcmp(filename, currentFCB.fileName);
			if (flag == 0)//文件名匹配成功
			{
				char *str = new char[8];
				if (currentFCB.fileType == 0)
					strcpy(str, "文件");
				else
					strcpy(str, "文件夹");
				cout << "文件名: " << currentFCB.fileName << endl;
				cout << "文件大小: " << currentFCB.fileSize << endl;
				cout << "文件类型: " << str << endl;
				cout << "最后修改时间: " << currentFCB.fileTime << endl;
				cout << "文件内容盘块号: " << currentFCB.fileContent << endl;
				break;
			}
			else
			{
				if (currentFCB.rFCB != -1)
					currentFCB = fcb[currentFCB.rFCB];
				else
					break;
			}
		} while (1);
		if (flag != 0)
		{
			cout << "没有找到该文件或文件夹" << endl;
		}
	}
}

/*write by zm*/
//写入文件内容（已知filename）
void FileManage::write(char *filename)
{
	char tmpChar;
	//当前目录的子节点对应的fcb下标号，即孩子节点所在层级
	int fcb_son = fcb[fcb_cur].sFCB;
	if (fcb_son == -1) {
		printf("当前文件夹下没有文件，是否新建？");
		if (getchar() == 'y')
		{
			newFile(filename);
			printf("已创建文件：%s。\n", filename);
		}
		return;
	}
	else {
		//查找内容块对应的盘块号(ibNum)
		do
		{
			if (strcmp(fcb[fcb_son].fileName, filename) == 0) {
				int count = 0, flag = 0;
				int contentStart;
				//若匹配，则此时fcb_son的值就是对应需要写入的fcb下标
				printf("请输入你需要的文件大小：0~40960字节\n");
				cin >> fcb[fcb_son].fileSize;
				int blockNum = fcb[fcb_son].fileSize / 4096 + 1;//需要存入的盘块数目
				printf("请输入你的内容，以#作为结束标识：\n");
				for (int i = 0; i < sizeof(content); i++)
				{
					tmpChar = getchar();
					if (tmpChar == '#')
						break;
					content[i] = tmpChar;
					
				}//内容存入缓冲区
				//测试：
				//cout << "你的输入是：" << content << endl;
				for (int i = 4105; i < BLKNUM; i++) {
					if (bitmap[i] == false) {
						//找到第一个为空的盘块;
						count = 1;
						contentStart = i;
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

				}//找到contentStart，将其赋给fileContent
				fcb[fcb_son].fileContent = contentStart;
				//写入内容到磁盘
				fp = fopen(systemName, "w+");
				//fp = fopen("d:\\zm", "w+");
				//定位到盘块号的起始指针,这里blocknum从1开始
				fseek(fp, contentStart*BLKSIZE, SEEK_SET);
				fwrite(content, fcb[fcb_son].fileSize, 1, fp);
				fclose(fp);
				break;
			}
			else
				fcb_son = fcb[fcb_son].rFCB;//同级移动查找
		} while (fcb[fcb_son].rFCB != -1);
	}
}

//将盘块号为blocknum的读入content
void FileManage::read(int blocknum)
{
	fp = fopen(systemName, "r");
	//定位到盘块号的起始指针
	fseek(fp, BLKSIZE * blocknum , SEEK_SET);
	//将内容读取到缓冲区
	fread(content, 1, fcb[blocknum].fileSize, fp);
	fclose(fp);
}

//显示文件内容
void FileManage::show(char *filename)
{
	//filename文件对应的盘块号
	int ibnum;
	//当前目录的子节点对应的fcb下标号
	int fcb_son = fcb[fcb_cur].sFCB;
	if (fcb_son == -1) {
		printf("当前文件夹下没有文件，是否新建？");
		if (getchar() == 'y')
		{
			newFile(filename);
			printf("%s 文件已创建。", filename);
		}
		return;
	}
	else {
		//查找内容块对应的盘块号(ibnum)
		do
		{
			if (strcmp(fcb[fcb_son].fileName, filename) == 0) {
				ibnum = fcb[fcb_son].fileContent;
				break;
			}
			else
				fcb_son = fcb[fcb_son].rFCB;//同级移动查找
		} while (fcb[fcb_son].rFCB != -1);
		read(ibnum);
		printf("%s", content);
		printf("\n");
	}
}

//查找文件的fcb序列号，不存在就返回-1
int FileManage::fcbSearch(char *filename)
{
	int columnFcbNum = fcb[0].sFCB;
	if (columnFcbNum == -1)
		return -1;//不存在
	else
	{
		do
		{
			int rowFcbNum = columnFcbNum;
			do
			{
				if (fcb[rowFcbNum].fileName == filename)
					return rowFcbNum;
				else
					rowFcbNum = fcb[rowFcbNum].rFCB;
			} while (fcb[rowFcbNum].rFCB == -1);
			columnFcbNum = fcb[columnFcbNum].sFCB;
		} while (fcb[columnFcbNum].sFCB == -1);
		return -1;//不存在
	}
}

//将指定路径下的文件移动到新的路径中
void FileManage::move(char *filename, char * dirname)
{
	int fileflag = 0;
	int dirflag = 0;
	int moveflag;
	//首先查找当前目录下是否存在此文件
	int tempFcbNum = fcb[fcb_cur].sFCB;
	if (tempFcbNum == -1) {
		printf("不存在此文件，请重新输入。\n");
		return;
	}
	do
	{
		if (fcb[tempFcbNum].fileName == filename)
		{
			fileflag = tempFcbNum;
			break;//存在文件就退出循环
		}
		else
		{
			tempFcbNum = fcb[tempFcbNum].rFCB;
		}
	} while (fcb[tempFcbNum].rFCB == -1);
	if (fileflag = 0) {
		printf("不存在此文件，请重新输入。\n");
		return;
	}
	else
	{
		//fileflag是要移动的文件的fcb号，dirflag是目录的fcb号
		dirflag = fcbSearch(dirname);
		if (dirflag == -1)
		{
			printf("不存在该目录，请新建或重新输入。\n");
			return;
		}
		else
		{
			if (fcb[dirflag].sFCB == -1)//目录下没有文件
			{
				fcb[fileflag].fFCB = dirflag;
				fcb[fileflag].lFCB = -1;
				fcb[fileflag].rFCB = -1;
				fcb[dirflag].sFCB = fileflag;
				return;
			}
			//目录下有文件
			moveflag = fcb[dirflag].sFCB;
			//移动moveflag直到右兄弟是空
			while (fcb[moveflag].rFCB != -1)
			{
				moveflag = fcb[moveflag].rFCB;
			}
			fcb[fileflag].fFCB = dirflag;
			fcb[fileflag].lFCB = moveflag;
			fcb[fileflag].rFCB = -1;
			fcb[moveflag].rFCB = fileflag;
			return;
		}
	}
}


//删除文件，fcb_cur改变为被删除文件的父节点
bool FileManage::delFile(char* filename)
{
	char flag;
	int cur = fcb_cur;
	if (cd(filename) == 0)
		return 0;
	struct FCB *currentFCB = &fcb[fcb_cur];//被删除文件
	fcb_cur = fcb[currentFCB->fFCB].FCBNum;//fcb_cur为当前文件的父节点
	
again:
	cout << "确认删除" << filename << "[y,n]?";
	cin >> flag;
	if (flag == 'n' || flag == 'N')//确认不删除
		return 0;
	else if (flag == 'y' || flag == 'Y')//确认删除
	{
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
			return 1;
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
				return 1;
			}
			else//子节点为空
			{
				while (fcb_cur != cur)
					cd("..");
				return 1;
			}
		}
	}
	else//输入有误
	{
		goto again;
	}

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