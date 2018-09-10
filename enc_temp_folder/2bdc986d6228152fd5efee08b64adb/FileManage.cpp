#include "pch.h"
#include "FileManage.h"
#include<string>
#include<iostream>
using namespace std;


FileManage::FileManage()
{
	fp = NULL;
	baseAddr = (char*)malloc(sizeof(char) * DISKSIZE);
	memset(baseAddr, 0, DISKSIZE);
	fcbnum = 0;
	fcb_cur = 0;
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
	systemName = name;
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
	for (int i = 0; i < 1024; i++)
	{
		fread(&fcb[i], 1, 64, fp);	
	}
	/*getFcbArray();*/

	//��fcb�������в��ԣ���ʼ��fcb������
	struct FCB f;
	strcpy(f.fileName, "first_FCB");
	f.fileSize = 0;
	f.fileType = true;
	f.delFlag = false;
	f.FCBNum = 1;
	f.fileContent = -1;
	f.fFCB = -1;
	f.lFCB = -1;
	f.rFCB = -1;
	f.sFCB = -1;

	fcb[fcb_cur] = f;
	fclose(fp);
	return 1;
}
//����϶�fcb������д��
int FileManage::close()
{
	char Map[32768];
	string symPath = "D:/";
	symPath += "hl";
	fp = fopen(symPath.c_str(), "r+");
	fseek(fp, 0, SEEK_SET);
	fcbnum = 100;
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
	//��ʼд��fcb��ֵ
	for (int i = 0; i < 512; i++) {
		//��ȡ�ļ���
		fwrite(fcb[i].fileName, 1, 20, fp);
		//��ȡ�ļ���С
		fwrite(&fcb[i].fileSize, 4, 1, fp);
		//д���ļ�����
		fwrite(&fcb[i].fileType, 1, 1, fp);
		//��ȡ�ļ��޸�ʱ��
		fwrite(fcb[i].fileTime, 14, 1, fp);
		//��ȡ�ļ�ɾ����־
		fwrite(&fcb[i].delFlag , 1, 1, fp);
		//��ȡ�ļ�FCB��λ��
		fwrite(&fcb[i].FCBNum, 4, 1, fp);
		//��ȡ�ļ�������ʼ�̿��
		fwrite(&fcb[i].fileContent, 4, 1, fp);
		//��ȡ�ļ����ڵ�fcb
		fwrite(&fcb[i].fFCB, 4, 1, fp);
		//��ȡ�ļ����ֵܽڵ�fcb
		fwrite(&fcb[i].lFCB, 4, 1, fp);
		//��ȡ�ļ����ֵܽڵ�fcb
		fwrite(&fcb[i].rFCB, 4, 1, fp);
		//��ȡ�ļ��ӽڵ�fcb
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


//������л�ȡfcb����
void FileManage::getFcbArray() {
	
	fp = fopen(systemName, "r+");//��ֻ������ʽ��ϵͳ�ļ�
	fseek(fp, FCBSTART, 0);

	for (int i = 0; i < fcbnum; i++) {
		//��ȡ�ļ���
		char s[20];
		fread(s, 1, 20, fp);
		strcpy(fcb[i].fileName,s);
		//��ȡ�ļ���С
		int a[2];
		fread(a, 4, 1, fp);
		fcb[i].fileSize = a[0];
		//��ȡ�ļ�����
		bool filetype[2];
		fread(filetype, 1, 1, fp);
		fcb[i].fileType = filetype[0];
		//��ȡ�ļ��޸�ʱ��
		char strTime[14];
		fread(strTime, 14, 1, fp);
		strcpy(fcb[i].fileTime,strTime);
		//��ȡ�ļ�ɾ����־
		bool flagDel[2];
		fread(flagDel, 1, 1, fp);
		fcb[i].delFlag = flagDel[0];
		//��ȡ�ļ�FCB��λ��
		int positionFcb[2];
		fread(positionFcb, 4, 1, fp);
		fcb[i].FCBNum = positionFcb[0];
		//��ȡ�ļ�������ʼ�̿��
		int contentFcb[2];
		fread(contentFcb, 4, 1, fp);
		fcb[i].fileContent = contentFcb[0];
		//��ȡ�ļ����ڵ�fcb
		int fFcb[2];
		fread(fFcb, 4, 1, fp);
		fcb[i].fFCB = fFcb[0];
		//��ȡ�ļ����ֵܽڵ�fcb
		int lFcb[2];
		fread(lFcb, 4, 1, fp);
		fcb[i].lFCB = lFcb[2];
		//��ȡ�ļ����ֵܽڵ�fcb
		int rFcb[2];
		fread(rFcb, 4, 1, fp);
		fcb[i].rFCB = rFcb[2];
		//��ȡ�ļ��ӽڵ�fcb
		int sFcb[2];
		fread(sFcb, 4, 1, fp);
		fcb[i].rFCB = sFcb[2];
	}

	fclose(fp);
}

////��ȡ�ļ��ڴ�������ռ�õ����е��̿�
//void FileManage::getMap(char str[20]) {
//	//��ȡ�ļ���·����ȡ�ļ���fcb��ֵ
//	if (cd(str)) {
//		int startblock = fcb[fcb_cur].fileContent;//��ȡ��һ���̿�
//		int count = fcb[fcb_cur].fileSize / 4096;//��ȡ��ռ���̿���
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

////�����µ��ļ�
//bool FileManage::newFile(char* oldfilename, char *filename) {
//	if (!cd(filename)) {
//		printf("��ȡ��ǰ·��ʧ��");
//		return false;//��ȡ��ǰ��ϵͳ·������
//
//	}
//	struct FCB *newFcb;	//�����µ�FCB
//	newFcb->initialize();
//	newFcb->fileName = filename;
//	newFcb->fileType = false;
//	newFcb->fileTime = "0";//֮�����۴�ȡʲôʱ��
//	nexFcb->delFlag = false;
//	newFcb->FCBNum = fcbnum;
//	//����λʾͼ�ҵ���һ��ΪֵΪ0�������̿��	
//	newFcb->fileSize = file_size(oldfilename);
//	int blockNum = newFcb->fileSize / 4096 + 1;//��ȡһ���ļ�Ҫ����̿���
//	int count = 0;
//	int flag = 0;
//	int i;
//	for (i = 4105; i < BLKNUM; i++) {
//		if (bitmap[i] == false) {
//			//�ҵ���һ��Ϊ�յ��̿�;
//			count = 1;
//			for (int j = i + 1; j < BLKNUM; i++) {
//				if (bitmap[j] == false) {
//					count++;
//					if (count == blockNum) {
//						flag = 1;
//						break;
//					}
//				}
//				else {
//					count = 0;
//					break;
//				}
//			}
//			if (flag = 1) {
//				break;
//			}
//		}
//
//	}
//	if (flag == 1) {
//		newFcb->fileContent = i;//��ֵ���ļ�����ʼ��������Ϣ��
//		for (int t = 0; t <= count; t++) {
//			bitmap[t + i] = true;
//		}
//	}
//	else {
//		printf("����ʧ��");
//		return false;
//	}
//
//	//��ʼд�ļ�����һ���̿�Ϊ��λ����д��
//	fp = fopen(minifile, "w");
//	fseek(fp, CONTENTSTART��SEEK_SET);
//
//	FILE *oldfp = fopen(oldfilename, "r");
//	if (oldfp == NULL) {
//		printf("��ȡ·��ʧ��");//��ȡ����ϵͳ�µ�·��ʧ�ܣ�
//		return false;
//	}
//	char *temp[4096];
//	int remaining = newFcb->fileSize;//�տ�ʼʣ����ֽ��������ܵ��ֽ���;
//	for (i = 0; i < blockNum; i++) {
//		if (remaining > 4096) {
//			//˵�����̿�Ҫ����
//			fread(temp, 1, 4096, oldfp);
//			fwrite(temp, 1, 4096, fp);
//			remaining -= 4096;
//		}
//		else {
//			fread(temp, 1, remaining, oldfp);
//			fwrite(temp, 1, remaining, fp);
//		}
//	}
//	struct FCB fcbPre = fcb[fcb_cur];
//	//��ǰ������ڵ�û���ӽڵ㣬˵������������ڵ��ǵ�ǰ�ڵ�ĵ�һ���ӽڵ�
//	if (fcbPre->sFCB == -1) {
//		fcbPre.sFCB = newFcb->FCBNum;
//		newFcb->fFCB = fcbPre.FCBNum;
//	}
//	//��ǰ����ڵ����ӽڵ㣬Ҫ�ҵ���ǰ�ڵ�����һ�����ӽڵ�
//	else {
//		//��ȡ��һ���ֵܽڵ�
//		struct FCB broFcb = fcb[fcbPre.sFCB];
//		while (1) {
//			if (broFcb.rFCB == -1) {
//				//�ҵ������һ���ֵܽڵ�
//				broFcb.rFCB = newFcb->FCBNUM;
//				newFcb->lFCB = broFcb.FCBNUM;
//				break;
//			}
//			//��һ���ֵܽڵ�Ϊ��ǰ�ڵ�����ֵܽڵ�
//			broFcb = fcb[broFcb.rFCB];
//		}
//	}
//	fcb[fcbnum++] = newFcb;
//	return true;
//}