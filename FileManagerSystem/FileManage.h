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
	char fileName[17];	//�ļ���
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
	int fcb_cur;//��ǰĿ¼
	bool bitmap[BLKNUM];//λ��ͼ��0~4104 not care
	int fcbnum;//FCB��ǰ����Ŀ
};

