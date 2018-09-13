#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <string.h>
#include<memory.h>
#include<fstream>
#include<vector>
#include <algorithm>
#include <algorithm>
using namespace std;

#define BLKSIZE 4096                 //每个盘块的大小
#define BLKNUM 262144                //盘块的数量
#define DISKSIZE 1073741824		 //整个磁盘空间的字节数
#define FCBSTART 36864               //FCB的起始盘块前的字节数
#define FCBSTART 36864               //FCB的起始盘块前的字节数
#define CONTENTSTART  16814080       //文件内容盘块前的字节数

struct FCB
{
	char fileName[14];	             //文件名
	int fileSize;		             //文件大小
	bool fileType;		             //文件类型，0为文件，1位文件夹
	char fileTime[20];	             //文件最后修改时间
	bool delFlag;		             //删除标志
	int FCBNum;			             //第几个FCB
	int fileContent;	             //文件内容盘块号
	int fFCB;			             //父节点FCB
	int lFCB;			             //左兄弟节点FCB
	int rFCB;			             //右兄弟节点FCB
	int sFCB;			             //子节点FCB

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
	bool ifmount;                                                 //空间安装标记
	FILE *fp;
	char sysname[20];
	char systemName[20];
	int fcb_cur;                                                  //当前目录
	bool bitmap[BLKNUM];                                          //位视图，0~4104 not care
	int fcbnum;                                                   //FCB当前总数目
	void getFcbArray();                                           //从外存中获取fcb数组
	struct FCB fcb[BLKNUM];	                                      //创建一个fcb容器
	int delMin = INT_MAX;
	int fcb_min;									//删除的文件（夹）中的最小的块号
	int fcb_emp;									//第一个为空的fcb盘块号	
	string minifile;									//临时的空间名
	string minifile1;							//临时的空间名:
	string minifile2; 							//临时的空间名:"\"
	string curpath;										//当前路径
	int dx;					//创建新文件夹时fcbnum与上一级新建文件夹间的位移量			
	int tmp;
	const string commands;
	int layer;
	bool layers[20];
	void tree();
	bool match_string(const char*, const char* );
	int dFlag;

	//zm
	void write(char *);
	void read(int,int);					                          //读数据
	void show(char* );					                          //显示文件
	void move(char *, char * );			                          //将指定路径下的文件移动到新的路径中
	int fcbSearch(char *);
	void map(char *);

	//hl
	//void getMap(char str[20]);                                  //获取文件在磁盘中所占用的所有的盘块
	int newFile(char *filename);              //创建一个新的文件
	int file_size(char* filename);                                //获取文件大小
	bool copy(string filename1, string filename2);//实现三种copy
	int judge_exist(string filename);//根据文件名判断该文件是否存在

	//zdj
	string getTime();													// 获取系统时间
	void split(const string&s, vector<string>&v, const string&key);		// string分割函数
	bool mkdir_cur_not_empty(string strArr, int cur_tmp);				// 当前目录(不为空)下创建文件夹
	void mkdir_cur_empty(string strArr, int cur_tmp);					// 当前目录(为空)下创建文件夹
	int cal_dx(int curfcb);												// 计算dx
	bool cd(string str);												// 改变目录(目标路径)
	bool mkdir(string str);												// 创建子目录

};

