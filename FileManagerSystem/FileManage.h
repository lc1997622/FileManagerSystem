#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <string.h>


#define BLKSIZE 4096
#define BLKNUM 262144
#define DISKSIZE 1073741824 

struct FCB
{
	char fileName[17];	//文件名
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
	char * baseAddr;
	int blockSize;
	FILE *fp;
	char* systemName;
	struct FCB menu[4096];
	int fcb_cur;//当前目录
	bool bitmap[BLKNUM];//位视图，0~4104 not care
	int fcbnum;//FCB当前总数目
};

