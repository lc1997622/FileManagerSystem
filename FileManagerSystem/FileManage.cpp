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

/*ȫ�ֱ�������*/
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
	minifile = systemName;						//��ʱ�Ŀռ���
	string str = minifile;
	minifile1 = str.append(":");			//��ʱ�Ŀռ���:
	minifile2 = str.append("/"); 			//��ʱ�Ŀռ���:/
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
//����϶�fcb������д��
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

	//��ʼд��fcb��ֵ
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





// string�ָ��
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

// ��ȡϵͳʱ��
string FileManage::getTime()
{
	struct tm t;   //tm�ṹָ��
	time_t now;  //����time_t���ͱ���
	time(&now);      //��ȡϵͳ���ں�ʱ��
	localtime_s(&t, &now);   //��ȡ�������ں�ʱ��
	string time = "";
	string year, mon = "0", day = "0", hour = "0", min = "0", sec = "0";
	// ��
	year = std::to_string(t.tm_year + 1900);
	// ��
	if (t.tm_mon >= 0 && t.tm_mon <= 8) {
		mon.append(std::to_string(t.tm_mon + 1));
	}
	else {
		mon = std::to_string(t.tm_mon + 1);
	}
	// ��
	if (t.tm_mday > 0 && t.tm_mday <= 9) {
		day.append(std::to_string(t.tm_mday));
	}
	else {
		day = std::to_string(t.tm_mday);
	}
	// ʱ
	if (t.tm_hour >= 0 && t.tm_hour <= 9) {
		hour.append(std::to_string(t.tm_hour));
	}
	else {
		hour = std::to_string(t.tm_hour);
	}
	// ��
	if (t.tm_min >= 0 && t.tm_min <= 9) {
		min.append(std::to_string(t.tm_min));
	}
	else {
		min = std::to_string(t.tm_min);
	}
	//��
	if (t.tm_sec >= 0 && t.tm_sec <= 9) {
		sec.append(std::to_string(t.tm_sec));
	}
	else {
		sec = std::to_string(t.tm_sec);
	}
	time.append(year).append(mon).append(day).append(hour).append(min).append(sec);
	return time;
}

// ����dx
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

// �ı�Ŀ¼(Ŀ��·��)
bool FileManage::cd(string str) {
	int fcb_tmp = fcb_cur;// ��ǰĿ¼FCB��
	// cd ..
	if (str.compare("..") == 0) {
		if (fcb_cur == 0) {
			cout << "���Ѿ��ڸ�Ŀ¼��" << endl;
		}
		else {
			fcb_cur = fcb[fcb_tmp].fFCB;
			int tmp_size = strlen(fcb[fcb_tmp].fileName);
			if (fcb[fcb_tmp].fFCB != 0)
				tmp_size += 1;
			curpath.erase(curpath.size() - tmp_size);// ������һ��
		}
		return true;
	}
	// cd .	·������
	vector<string>strArr;
	split(str, strArr, "/");

	if (strArr.size() == 1) {
		// cd ��ǰ·��һ��Ŀ¼
		if (strArr[0].compare(minifile1) != 0) {
			if (fcb[fcb_tmp].sFCB != -1) {
				int fcb_flg;
				fcb_tmp = fcb[fcb_tmp].sFCB;
				int flag = 0;
				do {
					string sFileName(fcb[fcb_tmp].fileName);
					// �ҵ�Ŀ��Ŀ¼
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
				// Ŀ��Ŀ¼������
				if (flag == 0) {
					cout << "ϵͳ�Ҳ���ָ����·��" << endl;
					return false;
				}
			}
		}
		// ���ظ�Ŀ¼
		else {
			fcb_cur = 0;
			curpath = minifile2;
		}
	}
	else {
		string path_tmp;
		int i;
		// cd ��ǰ·���༶Ŀ¼
		if (strArr[0].compare(minifile1) != 0) {
			for (i = 0; i < strArr.size(); i++) {
				int fcb_flg = fcb_tmp;
				if (fcb[fcb_tmp].sFCB != -1) {
					fcb_tmp = fcb[fcb_tmp].sFCB;
					int flag = 0;
					do {
						string sFileName(fcb[fcb_tmp].fileName);
						// �ҵ���һ��Ŀ¼
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
					// ��һ��Ŀ¼������
					if (flag == 0) {
						cout << "ϵͳ�Ҳ���ָ����·��" << endl;
						return false;
					}
				}
				else {
					cout << "ϵͳ�Ҳ���ָ����·��" << endl;
					return false;
				}
			}
			// �ҵ�Ŀ��Ŀ¼
			if (i == strArr.size()) {
				fcb_cur = fcb_tmp;
				curpath.append(path_tmp);
			}
		}
		// cd ��Ŀ¼�¶༶Ŀ¼(����·��)
		else {
			fcb_tmp = 0;
			for (i = 1; i < strArr.size(); i++) {
				if (fcb[fcb_tmp].sFCB != -1) {
					int fcb_flg;
					fcb_tmp = fcb[fcb_tmp].sFCB;
					int flag = 0;
					do {
						string sFileName(fcb[fcb_tmp].fileName);
						// �ҵ���һ��Ŀ¼
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
					// ��һ��Ŀ¼������
					if (flag == 0) {
						cout << "ϵͳ�Ҳ���ָ����·��" << endl;
						return false;
					}
				}
				else {
					cout << "ϵͳ�Ҳ���ָ����·��" << endl;
					return false;
				}
			}
			// �ҵ�Ŀ��Ŀ¼
			if (i == strArr.size()) {
				curpath = minifile1;
				fcb_cur = fcb_tmp;
				curpath.append(path_tmp);
			}
		}
	}
	return true;
}

// ��ǰĿ¼(��Ϊ��)�´����ļ���
bool FileManage::mkdir_cur_not_empty(string strArr, int cur_tmp) {
	fcb_emp += cal_dx(cur_tmp);
	int fcb_flg;
	int fcb_tmp = fcb[cur_tmp].sFCB;
	int flag = 0;
	do {
		string sFileName(fcb[fcb_tmp].fileName);
		// �ҵ���ͬ�ļ���
		if (strArr.compare(sFileName) == 0) {
			flag = 1;
			break;
		}
		else {
			fcb_flg = fcb_tmp;
			fcb_tmp = fcb[fcb_tmp].rFCB;
		}
	} while (fcb_tmp != -1);
	// û����ͬ�ļ���
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
	// ����ͬ�ļ���
	else {
		tmp = fcb_tmp;
		return false;
	}
	fcbnum++;
	return true;
}

// ��ǰĿ¼(Ϊ��)�´����ļ���
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

// ������Ŀ¼
bool FileManage::mkdir(string str) {
	int fcb_tmp = fcb_cur;// ��ǰĿ¼FCB��
	// mkdir ·������
	vector<string>strArr;
	split(str, strArr, "/");

	// mkdir ��ǰĿ¼�´���
	if (strArr.size() == 1) {
		// ��ǰĿ¼��Ϊ��
		if (fcb[fcb_cur].sFCB != -1) {
			if (!mkdir_cur_not_empty(strArr[0], fcb_tmp)) {
				cout << strArr[0] << "�ļ����Ѵ���" << endl;
				return false;
			}
		}
		// ��ǰĿ¼Ϊ��
		else {
			mkdir_cur_empty(strArr[0], fcb_tmp);
		}
	}
	else {
		int i;
		// mkdir ��ǰ�༶Ŀ¼�´���
		if (strArr[0].compare(minifile1) != 0) {
			for (i = 0; i < strArr.size(); i++) {
				if (fcb[fcb_tmp].sFCB != -1) {
					if (!mkdir_cur_not_empty(strArr[i], fcb_tmp) && i == strArr.size() - 1) {
						cout << strArr[i] << "�ļ����Ѵ���" << endl;
						return false;
					}
					if (!mkdir_cur_not_empty(strArr[i], fcb_tmp) && i != strArr.size()) {
						fcb_tmp = tmp;
					}
				}
				else {
					mkdir_cur_empty(strArr[i], fcb_tmp);
					fcb_tmp = fcb_emp - dx;// ��ȡ��Ҫ�������ļ��еĸ�Ŀ¼��fcb���
				}
			}
		}
		// mkdir ����·������
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







//��ȡ�ļ��Ĵ�С
int FileManage::file_size(char* filename) {
	FILE *f = fopen(filename, "r");
	if (!f) return -1;
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	return size;
}

//�����µ��ļ�
int FileManage::newFile(char filename[20]) {
	struct FCB newFcb;
	newFcb.initialize();
	struct FCB fcbPre = fcb[fcb_cur];
	//��ǰ������ڵ�û���ӽڵ㣬˵������������ڵ��ǵ�ǰ�ڵ�ĵ�һ���ӽڵ�
	
	strcpy(newFcb.fileName, filename);
	newFcb.fileType = false;
	strcpy(newFcb.fileTime, "0");//֮�����۴�ȡʲôʱ��
	newFcb.delFlag = false;
	for (int i = 0; i < BLKNUM; i++) {
		if (fcb[i].delFlag == true || fcb[i].FCBNum == -1) {
			//�ҵ���һ�����Դ�fcb�ĵط�
			newFcb.FCBNum = i;
			break;
		}
	}

	
	if (fcbPre.sFCB == -1) {
		fcb[fcb_cur].sFCB = newFcb.FCBNum;
		newFcb.fFCB = fcb[fcb_cur].FCBNum;
	}
	//��ǰ����ڵ����ӽڵ㣬Ҫ�ҵ���ǰ�ڵ�����һ�����ӽڵ�
	else {
		//��ȡ��һ���ֵܽڵ�
		struct FCB broFcb = fcb[fcbPre.sFCB];
		while (1) {
			//�ж��Ƿ����ͬ���ļ�
			if (strcmp(broFcb.fileName, newFcb.fileName) == 0) {
				printf("���ļ��Ѵ��ڣ�");
				return -1;
			}
			if (broFcb.rFCB == -1) {
				//�ҵ������һ���ֵܽڵ�

				fcb[broFcb.FCBNum].rFCB = newFcb.FCBNum;
				newFcb.lFCB = broFcb.FCBNum;
				
				break;
			}
			//��һ���ֵܽڵ�Ϊ��ǰ�ڵ�����ֵܽڵ�
			broFcb = fcb[broFcb.rFCB];
		}
	}
	newFcb.rFCB = -1;

	fcb[newFcb.FCBNum] = newFcb;
	return newFcb.FCBNum;
}
//��ӡĿ¼��fcb_cur���ı�
void FileManage::showDir()
{
	struct FCB currentFCB = fcb[fcb_cur];//�ò�����ɺ�
	if (currentFCB.sFCB == -1)
	{
		cout << "��Ŀ¼��û���ļ����ļ���" << endl;
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

//��ʾ���ԣ�fcb_cur���ı�
void FileManage::showAtt(char* filename)
{
	struct FCB currentFCB = fcb[fcb_cur];//�ò�����ɺ�
	int flag = 0;
	if (currentFCB.sFCB == -1)//û�����ļ�
	{
		cout << "û���ҵ����ļ����ļ���" << endl;
	}
	else//�������ļ�
	{
		currentFCB = fcb[currentFCB.sFCB];
		do
		{
			int match = match_string(filename, currentFCB.fileName);
			if (match == 0)//�ļ���ƥ��ɹ�
			{
				char *str = new char[8];
				if (currentFCB.fileType == 0)
					strcpy(str, "�ļ�  ");
				else
					strcpy(str, "�ļ���");
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
			cout << "û���ҵ����ļ����ļ���" << endl;
		}
	}
}

/*write by zm*/
//д���ļ����ݣ���֪filename��
void FileManage::write(char *filename)
{
	int bitmapflag, blockNum;														//bitmapflag����ʶbitmap��ʼ��blocknum:��Ҫ������̿���Ŀ
	int charcount = 0;																//charcount:��¼����ĳ���;
	char tmpChar;
	int fcb_son = fcb[fcb_cur].sFCB;												//��ǰĿ¼���ӽڵ��Ӧ��fcb�±�ţ������ӽڵ����ڲ㼶
	if (fcb_son == -1) {
		printf("��ǰĿ¼�²����ڴ��ļ������ȴ�����\n");
		return;
	}
	else {
		//�������ݿ��Ӧ���̿��(ibNum)
		for (;; fcb_son = fcb[fcb_son].rFCB)										//ͬ���ƶ�����
		{
			if (strcmp(fcb[fcb_son].fileName, filename) == 0) {
				if (fcb[fcb_son].fileType != 0) {
					cout << "�޷�д���ļ��У��������ļ�����" << endl;
					return;
				}
				if (fcb[fcb_son].fileContent != -1)									//���ԭ����������
				{
					char judgeChar;
					cout << "����д��Ḳ��ԭ�����ݣ�����y/n��ȷ����Ĳ�����" << endl;
					judgeChar = getchar();
					getchar();														//�Ե�y/n����Ļس�
					if (judgeChar == 'n' || judgeChar == 'N')
						return;
				}
				int count = 0, flag = 0;
				int contentStart;
				cout << "(O_O) ������������ݣ�����#��Ϊ������ʶ��" << endl;
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
						//�ҵ���һ��Ϊ�յ��̿�;
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
				//������Ҫ�޸�bitmap��ֵ
				for (int i = bitmapflag; i < bitmapflag + blockNum; i++)
				{
					bitmap[i] = true;
				}
				//�ҵ�contentStart��������ʵ��ţ������丳��fileContent
				fcb[fcb_son].fileContent = contentStart;
				//д�����ݵ�����

				FILE *fpp = fopen(sysname, "r+");
				fseek(fpp, contentStart*BLKSIZE, SEEK_SET);
				fwrite(content, fcb[fcb_son].fileSize, 1, fpp);
				fclose(fpp);
				break;
			}
			if (fcb[fcb_son].rFCB == -1)
			{
				cout << "��ǰĿ¼�²����ڴ��ļ������ȴ�����" << endl;
				return;
			}
		}
	}
}

//��IB�̿��Ϊblocknum�Ķ���content
void FileManage::read(int fcbnum, int blocknum)
{
	FILE *fpp = fopen(sysname, "r");
	//��λ���̿�ŵ���ʼָ��
	fseek(fpp, BLKSIZE * blocknum, SEEK_SET);
	//�����ݶ�ȡ��������
	fread(content, 1, fcb[fcbnum].fileSize, fpp);
	fclose(fpp);
}

//��ʾ�ļ�����
void FileManage::show(char *filename)
{
	int fcbFlag;													//���ݿ��Ӧ��fcb�����
	int ibnum;														//�����ļ���Ӧ���̿��									
	int fcb_son = fcb[fcb_cur].sFCB;								//��ǰĿ¼���ӽڵ��Ӧ��fcb�±��
	if (fcb_son == -1) {
		printf("��ǰĿ¼�²����ڴ��ļ������ȴ�����\n");
		return;
	}
	else {
		//�������ݿ��Ӧ���̿��(ibnum)
		for (;; fcb_son = fcb[fcb_son].rFCB)//ͬ���ƶ�����
		{
			fcbFlag = fcb_son;
			if (strcmp(fcb[fcb_son].fileName, filename) == 0) {
				if (fcb[fcb_son].fileType != 0) {
					cout << "�޷���ȡ�ļ��У��������ļ�����" << endl;
					return;
				}
				if (fcb[fcb_son].fileContent == -1)
				{
					cout << "�ļ�����Ϊ�գ�����д�����ݡ�" << endl;
					return;
				}
				ibnum = fcb[fcb_son].fileContent;
				break;
			}
			//�˳�������
			if (fcb[fcb_son].rFCB == -1)
			{
				cout << "��ǰĿ¼�²����ڴ��ļ������ȴ�����" << endl;
				return;
			}
		}
		read(fcbFlag, ibnum);
		cout << content << endl;
	}
}

//�鿴��ǰ�ļ���ռ���̿��
void FileManage::map(char *filename)
{
	int fcbFlag, blocknum;											//���ݿ��Ӧ��fcb�����
	int ibnum;														//�����ļ���Ӧ���̿��									
	int fcb_son = fcb[fcb_cur].sFCB;								//��ǰĿ¼���ӽڵ��Ӧ��fcb�±��
	if (fcb_son == -1) {
		cout << "��ǰĿ¼�²����ڴ��ļ������ȴ�����" << endl;
		return;
	}
	else {
		//�������ݿ��Ӧ���̿��(ibnum)
		for (;; fcb_son = fcb[fcb_son].rFCB)//ͬ���ƶ�����
		{
			fcbFlag = fcb_son;
			if (strcmp(fcb[fcb_son].fileName, filename) == 0) {
				if (fcb[fcb_son].fileType != 0) {
					cout << "�ļ����޷�ִ��map�������������ļ�����" << endl;
					return;
				}
				if (fcb[fcb_son].fileContent == -1) {
					cout << "��ǰ�ļ�Ϊ�գ�δ���з�����̡�" << endl;
					return;
				}
				ibnum = fcb[fcb_son].fileContent;
				break;
			}
			if (fcb[fcb_son].rFCB == -1)
			{
				cout << "��ǰĿ¼�²����ڴ��ļ������ȴ�����" << endl;
				return;
			}
		}
		//��ʼ�̿�� ��ibnum��fcb[fcbFlag].fileSize ������ռ�����̿�� 
		cout << fcb[fcbFlag].fileName << " ��ռ���ݿ���̿��: " << endl;
		blocknum = fcb[fcbFlag].fileSize / 4096 + 1;
		for (int i = 0; i < blocknum; i++)
		{
			printf("%d ", ibnum++);
		}
		printf("\n");
	}
}

//�����ļ����ļ��е�fcb���кţ������ھͷ���-1
int FileManage::fcbSearch(char *filename)
{
	int columnFcbNum = fcb[0].sFCB;
	//cout << "��һ��columnFcbNum: "<<columnFcbNum << endl;
	if (columnFcbNum == -1)
		return -1;//������
	else
	{
		//bfs����
		for (;; columnFcbNum = fcb[columnFcbNum].sFCB)
		{
			int rowFcbNum = columnFcbNum;
			for (;; rowFcbNum = fcb[rowFcbNum].rFCB)
			{
				//cout << fcb[rowFcbNum].fileName << endl;
				if (strcmp(fcb[rowFcbNum].fileName, filename) == 0)
				{
					//cout << "Ŀ¼��fcb�±��ǣ� " << rowFcbNum << endl;
					return rowFcbNum;
				}
				if (fcb[rowFcbNum].rFCB == -1)
					break;
			}
			if (fcb[columnFcbNum].sFCB == -1)
				break;
		}
		return -1;//������
	}
}

//����ǰ·���µ��ļ��ƶ����µ�·����
void FileManage::move(char *filename, char *dirname)
{
	int fileflag = -1;
	int dirflag = 0;
	int moveflag;
	//���Ȳ��ҵ�ǰĿ¼���Ƿ���ڴ��ļ�,tempFcbNum��fileflag������Ҫ�ƶ����ļ�fcb�±�
	int tempFcbNum = fcb[fcb_cur].sFCB;
	if (tempFcbNum == -1) {
		printf("��ǰĿ¼Ϊ�գ������²�����\n");
		return;
	}
	for (;; tempFcbNum = fcb[tempFcbNum].rFCB)
	{
		//cout << "��ǰ�ļ�����" << fcb[tempFcbNum].fileName << endl;
		if (strcmp(fcb[tempFcbNum].fileName, filename) == 0)
		{
			fileflag = tempFcbNum;
			break;//�����ļ����˳�ѭ��
		}
		if (fcb[tempFcbNum].rFCB == -1)
		{
			printf("�����ڴ��ļ������������롣\n");
			return;
		}
	}
	//fileflag��Ҫ�ƶ����ļ���fcb�ţ�dirflag��Ŀ¼��fcb��
	dirflag = fcbSearch(dirname);
	if (dirflag == -1)
	{
		printf("�����ڸ�Ŀ¼�����½����������롣\n");
		return;
	}
	else
	{
		//���ȵ��޸�ԭ��fileflag�ļ��ĸ�Ԫ�ص�sFCB����һ��,�������������λ���ж�
		if (fcb[fileflag].lFCB == -1)							//��ǰ������Ԫ��
		{
			int ffcbnum = fcb[fileflag].fFCB;
			int rfcbnum = fcb[fileflag].rFCB;
			fcb[ffcbnum].sFCB = rfcbnum;
			fcb[rfcbnum].lFCB = -1;
		}
		else if (fcb[fileflag].rFCB == -1)						//��ǰ������Ԫ��
		{
			int lfcbnum = fcb[fileflag].lFCB;
			fcb[lfcbnum].rFCB = -1;
		}
		else
		{
			int lfcbnum = fcb[fileflag].lFCB;					//��ǰ���м�Ԫ��
			int rfcbnum = fcb[fileflag].rFCB;
			fcb[lfcbnum].rFCB = rfcbnum;
			fcb[rfcbnum].lFCB = lfcbnum;
		}
		if (fcb[dirflag].sFCB == -1)							//���Ƶ���Ŀ¼��û���ļ�
		{
			fcb[fileflag].fFCB = dirflag;
			fcb[fileflag].lFCB = -1;
			fcb[fileflag].rFCB = -1;
			fcb[dirflag].sFCB = fileflag;
		}
		else {													//Ŀ¼�����ļ�
			moveflag = fcb[dirflag].sFCB;
			while (fcb[moveflag].rFCB != -1)					//�ƶ�moveflagֱ�����ֵ��ǿ�
			{
				moveflag = fcb[moveflag].rFCB;
			}
			fcb[fileflag].fFCB = dirflag;
			fcb[fileflag].lFCB = moveflag;
			fcb[fileflag].rFCB = -1;
			fcb[moveflag].rFCB = fileflag;
		}
		cout << "�ƶ��ɹ���" << endl;
		return;
	}
}


//ɾ���ļ���fcb_cur�ı�Ϊ��ɾ���ļ��ĸ��ڵ�

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
	int cur = fcb_cur;//��¼��ǰĿ¼
	char command;// "y" or "n"
	int check = 0;//�Ƿ���·��
	int len = strlen(filename);
	char path[100];//·��
	string file;//��ɾ���ļ���
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
	struct FCB *fatherFCB = &fcb[fcb_cur];//��ɾ���ļ��ĸ��ڵ�

	if (fatherFCB->sFCB != -1)
	{
		currentFCB = &fcb[fatherFCB->sFCB];
		do {
			if (match_string(currentFCB->fileName, file.c_str()))
			{
				dFlag++;
				currentFCB->fFCB = -1;
				//�ı�FCB����
				if (currentFCB->lFCB != -1)//���Ǹò��һ���ڵ�
				{
					struct FCB *leftFCB = &fcb[currentFCB->lFCB];
					if (currentFCB->rFCB != -1)//���Ǹò����һ���ڵ�
					{
						struct FCB *rightFCB = &fcb[currentFCB->rFCB];
						leftFCB->rFCB = rightFCB->FCBNum;
						rightFCB->lFCB = leftFCB->FCBNum;
					}
					else//�ò����һ���ڵ�
					{
						leftFCB->rFCB = -1;
					}
				}
				else//�ò��һ���ڵ�
				{
					if (currentFCB->rFCB != -1)//���Ǹò����һ���ڵ�
					{
						struct FCB *rightFCB = &fcb[currentFCB->rFCB];
						fcb[fcb_cur].sFCB = rightFCB->FCBNum;
						rightFCB->lFCB = -1;
					}
					else//�ò����һ���ڵ�
					{
						fcb[fcb_cur].sFCB = -1;
					}
				}
				//�ı�FCBɾ����ǡ��̿�ɾ�����
				if (currentFCB->fileType == 0)//ɾ����Ϊ�����ļ�
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
				else//ɾ��һ���ļ���
				{
					currentFCB->delFlag = 1;
					fcbnum--;
					if (currentFCB->FCBNum < delMin)
						delMin = currentFCB->FCBNum;
					if (currentFCB->sFCB != -1)//�ӽڵ㲻Ϊ��
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
								cout << "ɾ���ļ�" << currentFCB->fileName << "���ɹ�" << endl;
							}
							if (currentFCB->rFCB != -1)
								currentFCB = &fcb[currentFCB->rFCB];
							else
								break;
						} while (1);
						while (fcb_cur != cur)
							cd("..");
					}
					else//�ӽڵ�Ϊ��
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

//�������ļ����������ļ�
void FileManage::tree()
{
	const char *ch1 = "��", *ch2 = "������", *ch3 = "������";
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
			//cout << "��Ŀ¼��û���ļ����ļ���" << endl;
		}
		if (currentFCB.rFCB != -1 && layer > 0)
		{
			currentFCB = fcb[currentFCB.rFCB];
			cd("..");
			cd(currentFCB.fileName);
		}
		else//�Ըò����һ���ڵ���д���
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
//��ȡ�ļ��ڴ�������ռ�õ����е��̿�
//void FileManage::getMap(string str) {
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
//	else {
//		printf("���ļ�������");
//	}
//}

int FileManage::judge_exist(string filename) {
	struct FCB father = fcb[fcb_cur];//��ȡ��ǰ���ڵ��fcb
	if (father.sFCB == -1) {
		//��Ŀ¼��û���ӽڵ�
		return -1;
	}
	else {
		struct FCB broFcb = fcb[father.sFCB];//��ȡ��һ�����ӽڵ�
		while (1) {
			if (strcmp(broFcb.fileName, filename.c_str()) == 0) {
				//����������ͬ
				return broFcb.FCBNum;//�����ýڵ��fcb���
			}
			else {
				if (broFcb.rFCB == -1) break;//�������һ��fcb�ڵ�
			}

			broFcb = fcb[broFcb.rFCB];
		}
	}
	return -1;

}

bool FileManage::copy(string filename1, string filename2) {
	string first;
	string second;
	first = filename1.substr(0, 1);//��ȡ����ĸ
	second = filename2.substr(0, 1);

	if (first == "D" || first == "C" || first == "E") {
		//��һ���ļ�����windowsϵͳ�µ��ļ���
		int flag = judge_exist(filename2);
		if (flag != -1) {
			printf("�ڵ�ǰĿ¼�¸��ļ��Ѵ��ڣ����Ҫ���ǵ�ǰ�ļ�������y,����������n\n");
			char command;
			cin >> command;
			if (command == 'y') {
				//�û�ѡ�񸲸�
				char c[14]="";
				strcpy(c, filename1.c_str());
				int sum = file_size(c);//��ȡ�ļ��Ĵ�С
				struct FCB prefcb = fcb[flag];//��ȡҪ���ǵ�fcb
				int preSize = prefcb.fileSize;//��ȡҪ���ǵ�fcb�Ĵ�С
				int preBitMapstart = prefcb.fileContent;//��ȡҪ���ǵ�fcb���ļ�������ʼ���
				int countpre = (preSize / 4096) + 1;//��ȡ�ļ����ݿ������
				if (preSize > 0) {
					for (int i = preBitMapstart; i < countpre; i++) {
						bitmap[i] = false;//��λ��ͼ��ռ�õĵط�������Ϊfalse
					}
				}

				int count = (sum / 4096) + 1;//��ȡ���ڸ��ļ���Ҫռ���̿���

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
					printf("�ռ䲻��");
					return false;
				}
				else {
					fcb[flag].fileContent = i+4105;//���ҵ��ĵ�һ�ļ����ݿ鸳ֵ
					fcb[flag].fileSize = sum;
					for (int j = 0; j < count; j++) {
						bitmap[j + i] = true;
					}
					//��ʼд������
					char temp[4096];
					int remaining = sum;
					FILE *fp = fopen(c, "r");//��ȡ�ļ�
					FILE *fw = fopen(sysname, "w+");//��ϵͳ�ļ�
					for (int j = 0; j < count; j++) {
						if (remaining >= 4096) {
							fread(temp, 1, 4096, fp);//��4096���ַ������ڴ�
							fseek(fw, (9 + i + j + 4096) * 4096, SEEK_SET);//ƫ�Ƶ���ǰ�̿�
							fwrite(temp,  4096,1, fw);//������д��
							remaining -= 4096;
						}
						else {
							fread(temp, 1, remaining, fp);//��4096���ַ������ڴ�
							fseek(fw, (9 + i + j + 4096) * 4096, SEEK_SET);//ƫ�Ƶ���ǰ�̿�
							fwrite(temp,  remaining, 1,fw);//������д��
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
			//˵���ڵ�ǰĿ¼�¸��ļ�������
			char h[14]="";
			strcpy(h, filename2.c_str());
			int newFcbNum = newFile(h);
			char c[14]="";
			strcpy(c, filename1.c_str());
			int sum = file_size(c);//��ȡ�ļ��Ĵ�С
			int count = (sum / 4096) + 1;//��ȡ���ڸ��ļ���Ҫռ���̿���

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
				printf("�ռ䲻��");
				return false;
			}
			else {
				fcb[newFcbNum].fileContent = i+4105;//���ҵ��ĵ�һ�ļ����ݿ鸳ֵ
				fcb[newFcbNum].fileSize = sum;
				for (int j = 0; j < count; j++) {
					bitmap[j + i] = true;
				}


				//��ʼд������
				char temp[4096];
				int remaining = sum;
				FILE *fp = fopen(c, "r");//��ȡ�ļ�
				FILE *fw = fopen(sysname, "w+");//��ϵͳ�ļ�
				for (int j = 0; j < count; j++) {
					if (remaining >= 4096) {
						fread(temp, 1, 4096, fp);//��4096���ַ������ڴ�
						fseek(fw, (9 + i + j+4096)*4096, SEEK_SET);//ƫ�Ƶ���ǰ�̿�
						fwrite(temp, 4096, 1,fw);//������д��
						remaining -= 4096;
					}
					else {
						fread(temp, 1, remaining, fp);//��4096���ַ������ڴ�
						fseek(fw,( 9 + i + j + 4096)*4096 , SEEK_SET);//ƫ�Ƶ���ǰ�̿�
						fwrite(temp, remaining, 1,fw);//������д��
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
		//�ڶ����ļ�����windowsϵͳ�µ��ļ���
		int flag = judge_exist(filename1);
		if (flag == -1) {
			printf("�����Ƶĸ��ļ�������");
			return false;
		}
		else {
			//�����Ƶ�����ļ�����
			char y[14]="";
			strcpy(y, filename2.c_str());
			struct FCB firstFcb = fcb[flag];//��ȡ�������ļ���fcb
			int firstSize = firstFcb.fileSize;//��ȡ�������ļ��Ĵ�С
			int count = (firstSize / 4096) + 1;
			int firstStartContent = firstFcb.fileContent;
			char temp[4096];
			int remaining = firstSize;
			FILE *fw = fopen(y, "w+");
			FILE *fp = fopen(sysname, "r");//��ֻ������ʽ�򿪵�ǰϵͳ�ļ�
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
		//�����ļ����Ǹ�ϵͳ�µ�

		int flag1 = judge_exist(filename1);
		int flag2 = judge_exist(filename2);
		if (flag1 == -1) {
			printf("�����Ƶ��ļ�������");
			return false;
		}
		else {
			//�����Ƶ��ļ�����
			if (flag2 != -1) {
				//���Ƶ����ļ�����
				printf("���ļ��Ѵ��ڣ��Ƿ񸲸ǣ����ǵĻ�����y,�����ǵĻ�����n\n");
				char  command;
				cin >> command;
				if (command == 'y') {
					struct FCB firstFile = fcb[flag1];//��ȡ��һ���ļ���fcb
					struct FCB secondFile = fcb[flag2];//��ȡ�ڶ����ļ���fcb
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
			//���Ƶ��ļ�������
			else {

				char c[14]="";
				strcpy(c, filename2.c_str());
				flag2 = newFile(c);
				struct FCB firstFile = fcb[flag1];//��ȡ��һ���ļ���fcb
				struct FCB secondFile = fcb[flag2];//��ȡ�ڶ����ļ���fcb
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