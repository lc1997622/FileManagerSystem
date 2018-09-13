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

#define BLKSIZE 4096                 //ÿ���̿�Ĵ�С
#define BLKNUM 262144                //�̿������
#define DISKSIZE 1073741824		 //�������̿ռ���ֽ���
#define FCBSTART 36864               //FCB����ʼ�̿�ǰ���ֽ���
#define FCBSTART 36864               //FCB����ʼ�̿�ǰ���ֽ���
#define CONTENTSTART  16814080       //�ļ������̿�ǰ���ֽ���

struct FCB
{
	char fileName[14];	             //�ļ���
	int fileSize;		             //�ļ���С
	bool fileType;		             //�ļ����ͣ�0Ϊ�ļ���1λ�ļ���
	char fileTime[20];	             //�ļ�����޸�ʱ��
	bool delFlag;		             //ɾ����־
	int FCBNum;			             //�ڼ���FCB
	int fileContent;	             //�ļ������̿��
	int fFCB;			             //���ڵ�FCB
	int lFCB;			             //���ֵܽڵ�FCB
	int rFCB;			             //���ֵܽڵ�FCB
	int sFCB;			             //�ӽڵ�FCB

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
	bool ifmount;                                                 //�ռ䰲װ���
	FILE *fp;
	char sysname[20];
	char systemName[20];
	int fcb_cur;                                                  //��ǰĿ¼
	bool bitmap[BLKNUM];                                          //λ��ͼ��0~4104 not care
	int fcbnum;                                                   //FCB��ǰ����Ŀ
	void getFcbArray();                                           //������л�ȡfcb����
	struct FCB fcb[BLKNUM];	                                      //����һ��fcb����
	int delMin = INT_MAX;
	int fcb_min;									//ɾ�����ļ����У��е���С�Ŀ��
	int fcb_emp;									//��һ��Ϊ�յ�fcb�̿��	
	string minifile;									//��ʱ�Ŀռ���
	string minifile1;							//��ʱ�Ŀռ���:
	string minifile2; 							//��ʱ�Ŀռ���:"\"
	string curpath;										//��ǰ·��
	int dx;					//�������ļ���ʱfcbnum����һ���½��ļ��м��λ����			
	int tmp;
	const string commands;
	int layer;
	bool layers[20];
	void tree();
	bool match_string(const char*, const char* );
	int dFlag;

	//zm
	void write(char *);
	void read(int,int);					                          //������
	void show(char* );					                          //��ʾ�ļ�
	void move(char *, char * );			                          //��ָ��·���µ��ļ��ƶ����µ�·����
	int fcbSearch(char *);
	void map(char *);

	//hl
	//void getMap(char str[20]);                                  //��ȡ�ļ��ڴ�������ռ�õ����е��̿�
	int newFile(char *filename);              //����һ���µ��ļ�
	int file_size(char* filename);                                //��ȡ�ļ���С
	bool copy(string filename1, string filename2);//ʵ������copy
	int judge_exist(string filename);//�����ļ����жϸ��ļ��Ƿ����

	//zdj
	string getTime();													// ��ȡϵͳʱ��
	void split(const string&s, vector<string>&v, const string&key);		// string�ָ��
	bool mkdir_cur_not_empty(string strArr, int cur_tmp);				// ��ǰĿ¼(��Ϊ��)�´����ļ���
	void mkdir_cur_empty(string strArr, int cur_tmp);					// ��ǰĿ¼(Ϊ��)�´����ļ���
	int cal_dx(int curfcb);												// ����dx
	bool cd(string str);												// �ı�Ŀ¼(Ŀ��·��)
	bool mkdir(string str);												// ������Ŀ¼

};

