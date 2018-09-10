#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <string.h>
#include<memory.h>
#include<fstream>
#include<vector>
#include <algorithm>
using namespace std;

#define BLKSIZE 4096//每个盘块的大小
#define BLKNUM 262144//盘块的数量
#define DISKSIZE 1073741824/64 //整个磁盘空间的字节数
#define FCBSTART 36864 //FCB的起始盘块前的字节数
#define FCBSTART 36864 //FCB的起始盘块前的字节数
#define CONTENTSTART  16814080//文件内容盘块前的字节数

struct FCB
{
	char fileName[20];	//文件名
	int fileSize;		//文件大小
	bool fileType;		//文件类型，0为文件，1位文件夹
	char fileTime[14];	//文件最后修改时间
	bool delFlag;		//删除标志
	int FCBNum;			//第几个FCB
	int fileContent;	//文件内容盘块号
	int fFCB;			//父节点FCB
	int lFCB;			//左兄弟节点FCB
	int rFCB;			//右兄弟节点FCB
	int sFCB;			//子节点FCB

	void initialize() {
		*fileName = NULL;
		fileSize = 0;
		fileType = false;
		*fileTime = NULL;
		delFlag = false;
		FCBNum = -1;
		fileContent = -1;
		fFCB = -1;
		lFCB = -1;
		rFCB = -1;
		sFCB = -1;
	}
};
class FileManage
{
public:
	FileManage();
	~FileManage();
	int  create(char[]);
	int mount(char[]);
	int close();
	int format();
	void showDir();
	void showAtt(char*);
	bool delFile(char*);
	char * baseAddr;
	int blockSize;
	FILE *fp;
	char systemName[20];
	int fcb_cur;//当前目录
	bool bitmap[BLKNUM];//位视图，0~4104 not care
	int fcbnum;//FCB当前总数目
	void getFcbArray();//从外存中获取fcb数组
	struct FCB fcb[BLKNUM];	//创建一个fcb容器
	int delMin = INT_MAX;

	string curpath;				//当前路径
	const string commands;
	//zm
	void read(int);					//读数据
	void show(char* );					//显示文件
	void move(char *, char * );					//将指定路径下的文件移动到新的路径中
	int fcbSearch(char *);

	//hl
	//void getMap(char str[20]);//获取文件在磁盘中所占用的所有的盘块
	bool newFile(char* oldfilename, char *filename);//创建一个新的文件
	int file_size(char* filename);//获取文件大小

	//zdj
	void split(const string&s, vector<string>&v, const string&key);
	string changeSlash(string str);
	bool cd(string str);

};

