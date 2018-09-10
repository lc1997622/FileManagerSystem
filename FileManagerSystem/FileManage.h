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

#define BLKSIZE 4096//ÿ���̿�Ĵ�С
#define BLKNUM 262144//�̿������
#define DISKSIZE 1073741824/64 //�������̿ռ���ֽ���
#define FCBSTART 36864 //FCB����ʼ�̿�ǰ���ֽ���
#define FCBSTART 36864 //FCB����ʼ�̿�ǰ���ֽ���
#define CONTENTSTART  16814080//�ļ������̿�ǰ���ֽ���

struct FCB
{
	char fileName[20];	//�ļ���
	int fileSize;		//�ļ���С
	bool fileType;		//�ļ����ͣ�0Ϊ�ļ���1λ�ļ���
	char fileTime[14];	//�ļ�����޸�ʱ��
	bool delFlag;		//ɾ����־
	int FCBNum;			//�ڼ���FCB
	int fileContent;	//�ļ������̿��
	int fFCB;			//���ڵ�FCB
	int lFCB;			//���ֵܽڵ�FCB
	int rFCB;			//���ֵܽڵ�FCB
	int sFCB;			//�ӽڵ�FCB

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
	int fcb_cur;//��ǰĿ¼
	bool bitmap[BLKNUM];//λ��ͼ��0~4104 not care
	int fcbnum;//FCB��ǰ����Ŀ
	void getFcbArray();//������л�ȡfcb����
	struct FCB fcb[BLKNUM];	//����һ��fcb����
	int delMin = INT_MAX;

	string curpath;				//��ǰ·��
	const string commands;
	//zm
	void read(int);					//������
	void show(char* );					//��ʾ�ļ�
	void move(char *, char * );					//��ָ��·���µ��ļ��ƶ����µ�·����
	int fcbSearch(char *);

	//hl
	//void getMap(char str[20]);//��ȡ�ļ��ڴ�������ռ�õ����е��̿�
	bool newFile(char* oldfilename, char *filename);//����һ���µ��ļ�
	int file_size(char* filename);//��ȡ�ļ���С

	//zdj
	void split(const string&s, vector<string>&v, const string&key);
	string changeSlash(string str);
	bool cd(string str);

};

