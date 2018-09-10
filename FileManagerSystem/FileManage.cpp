#include "pch.h"
#include "FileManage.h"
#include<string>
#include<iostream>
using namespace std;

/*ȫ�ֱ�������*/
int fcb_cur;//��ǰĿ¼���ڼ���fcb
bool bitmap[BLKNUM];//λ��ͼ��0~4104 not care
int fcbnum;//��ʹ�õ�FCB��ǰ����Ŀ
char minifile[20];//ϵͳ������
struct FCB fcb[BLKNUM];//����һ��fcb����
char content[BLKSIZE * 10];//�ļ�����
FILE *fp;//���ļ����ݿ�ָ��

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
	strcmp(systemName, name);
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

// ��(��)б�ܱ�Ϊ˫��б��	
string FileManage::changeSlash(string str) {
	string::size_type pos = 0;
	while ((pos = str.find_first_of('\\', pos)) != string::npos)
	{
		str.insert(pos, "\\");//����
		pos = pos + 2;
	}
	return str;
}

// �ı�Ŀ¼(Ŀ��·��)
bool FileManage::cd(string str) {
	str = changeSlash(str);
	int fcb_tmp = fcb_cur;// ��ǰĿ¼FCB��
	// cd ..
	if (str.compare("..") == 0) {
		if (fcb_cur == 0) {
			cout << "���Ѿ��ڸ�Ŀ¼��" << endl << endl;
		}
		else {
			fcb_cur = fcb[fcb_tmp].fFCB;
			int tmp_size = strlen(fcb[fcb_tmp].fileName);
			curpath.erase(curpath.size() - tmp_size - 1);// ������һ��
		}
		return true;
	}
	// cd .	·������
	vector<string>strArr;
	split(str, strArr, "\\");

	if (strArr.size() == 1) {
		// cd ��ǰ·��һ��Ŀ¼
		if (strArr[0].compare("mini-FS:") != 0) {
			if (fcb[fcb_tmp].sFCB != -1) {
				fcb_tmp = fcb[fcb_tmp].sFCB;
				int flag = 0;
				do {
					string sFileName(fcb[fcb_tmp].fileName);
					// �ҵ�Ŀ��Ŀ¼
					if (strArr[0].compare(sFileName) == 0) {
						fcb_cur = fcb_tmp;
						curpath.append("\\");
						curpath.append(sFileName);
						flag = 1;
						break;
					}
					else {
						fcb_tmp = fcb[fcb_tmp].rFCB;
					}
				} while (fcb_tmp != NULL);
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
			curpath = "mini-FS:\\";
		}
	}
	else {
		string path_tmp;
		int i;
		// cd ��ǰ·���༶Ŀ¼
		if (strArr[0].compare("mini-FS:") != 0) {
			for (i = 0; i < strArr.size(); i++) {
				if (fcb[fcb_tmp].sFCB != -1) {
					fcb_tmp = fcb[fcb_tmp].sFCB;
					int flag = 0;
					do {
						string sFileName(fcb[fcb_tmp].fileName);
						// �ҵ���һ��Ŀ¼
						if (strArr[i].compare(sFileName) == 0) {
							path_tmp.append("\\");
							path_tmp.append(sFileName);
							flag = 1;
							break;
						}
						else {
							fcb_tmp = fcb[fcb_tmp].rFCB;
						}
					} while (fcb_tmp != NULL);
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
					fcb_tmp = fcb[fcb_tmp].sFCB;
					int flag = 0;
					do {
						string sFileName(fcb[fcb_tmp].fileName);
						// �ҵ���һ��Ŀ¼
						if (strArr[i].compare(sFileName) == 0) {
							path_tmp.append("\\");
							path_tmp.append(sFileName);
							flag = 1;
							break;
						}
						else {
							fcb_tmp = fcb[fcb_tmp].rFCB;
						}
					} while (fcb_tmp != NULL);
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
bool FileManage::newFile(char* oldfilename, char filename[20]) {
	struct FCB newFcb= {
		"",
		0,
		false,
		NULL,
		false,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1
	};

	strcpy(newFcb.fileName, filename);
	newFcb.fileType = false;
	strcpy(newFcb.fileTime, "0");//֮�����۴�ȡʲôʱ��
	newFcb.delFlag = false;
	for (int i = 0; i < 4096; i++) {
		if (fcb[i].delFlag == true || fcb[i].fileName == NULL) {
			//�ҵ���һ�����Դ�fcb�ĵط�
			newFcb.FCBNum = i;
			break;
		}
	}
	//����λʾͼ�ҵ���һ��ΪֵΪ0�������̿��	
	newFcb.fileSize = file_size(oldfilename);
	int blockNum = newFcb.fileSize / 4096 + 1;//��ȡһ���ļ�Ҫ����̿���
	int count = 0;
	int flag = 0;
	int i;
	for (i = 4105; i < BLKNUM; i++) {
		if (bitmap[i] == false) {
			//�ҵ���һ��Ϊ�յ��̿�;
			count = 1;
			if (count == blockNum) {
				flag = 1;
				break;
			}
			for (int j = i + 1; j < BLKNUM; i++) {
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
	if (flag == 1) {
		newFcb.fileContent = i;//��ֵ���ļ�����ʼ��������Ϣ��
		printf("��һ����Ϣ��%d\n", i);
		for (int t = 0; t <= count; t++) {
			bitmap[t + i] = true;
		}
	}
	else {
		printf("����ʧ��");
		return false;
	}

	//��ʼд�ļ�����һ���̿�Ϊ��λ����д��
	fp = fopen(systemName, "w");
	fseek(fp, CONTENTSTART,SEEK_SET);

	FILE *oldfp = fopen(oldfilename, "r");
	if (oldfp == NULL) {
		printf("��ȡ·��ʧ��");//��ȡ����ϵͳ�µ�·��ʧ�ܣ�
		return false;
	}
	char *temp[4096];
	int remaining = newFcb.fileSize;//�տ�ʼʣ����ֽ��������ܵ��ֽ���;
	for (i = 0; i < blockNum; i++) {
		if (remaining > 4096) {
			//˵�����̿�Ҫ����
			
			fread(temp, 1, 4096, oldfp);
			printf("��һ���ַ�%c",temp[0]);
			fwrite(temp, 1, 4096, fp);
			remaining -= 4096;
		}
		else {
			fread(temp, 1, remaining, oldfp);
			fwrite(temp, 1, remaining, fp);
		}
	}
	struct FCB fcbPre = fcb[fcb_cur];
	//��ǰ������ڵ�û���ӽڵ㣬˵������������ڵ��ǵ�ǰ�ڵ�ĵ�һ���ӽڵ�
	if (fcbPre.sFCB == -1) {
		fcbPre.sFCB = newFcb.FCBNum;
		newFcb.fFCB = fcbPre.FCBNum;
	}
	//��ǰ����ڵ����ӽڵ㣬Ҫ�ҵ���ǰ�ڵ�����һ�����ӽڵ�
	else {
		//��ȡ��һ���ֵܽڵ�
		struct FCB broFcb = fcb[fcbPre.sFCB];
		while (1) {
			if (broFcb.rFCB == -1) {
				//�ҵ������һ���ֵܽڵ�
				broFcb.rFCB = newFcb.FCBNum;
				newFcb.lFCB = broFcb.FCBNum;
				break;
			}
			//��һ���ֵܽڵ�Ϊ��ǰ�ڵ�����ֵܽڵ�
			broFcb = fcb[broFcb.rFCB];
		}
	}
	fcb[i]=newFcb;
	return true;
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
			flag = strcmp(filename, currentFCB.fileName);
			if (flag == 0)//�ļ���ƥ��ɹ�
			{
				char *str = new char[8];
				if (currentFCB.fileType == 0)
					strcpy(str, "�ļ�");
				else
					strcpy(str, "�ļ���");
				cout << "�ļ���:" << currentFCB.fileName << endl;
				cout << "�ļ���С:" << currentFCB.fileSize << endl;
				cout << "�ļ�����" << str << endl;
				cout << "����޸�ʱ��:" << currentFCB.fileTime << endl;
				cout << "�ļ������̿��:" << currentFCB.fileContent << endl;
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
			cout << "û���ҵ����ļ����ļ���" << endl;
		}
	}
}

/*write by zm*/
//���̿��Ϊblocknum�Ķ���content
void FileManage::read(int blocknum)
{
	int i, j;
	char ch;
	char buffer[BLKSIZE];
	fp = fopen(minifile, "r");
	//��λ���̿�ŵ���ʼָ��
	fseek(fp, BLKSIZE * (blocknum - 1), SEEK_SET);
	//�����ݶ�ȡ��������
	fread(content, 1, fcb[blocknum].fileSize + 1, fp);
	fclose(fp);
}

//��ʾ�ļ�����
void FileManage::show(char *filename)
{
	//filename�ļ���Ӧ���̿��
	int ibNum;
	//��ǰĿ¼���ӽڵ��Ӧ��fcb�±��
	int fcb_son = fcb[fcb_cur].sFCB;
	if (fcb_son == -1) {
		printf("��ǰ�ļ�����û���ļ����Ƿ��½���");
		if (getchar() == 'y')
		{
			//newFile(filename);
			printf("%s �ļ��Ѵ�����", filename);
		}
		return;
	}
	else {
		//�������ݿ��Ӧ���̿��(ibNum)
		while (fcb[fcb_son].rFCB != -1)
		{
			if (strcmp(fcb[fcb_son].fileName, filename)) {
				ibNum = fcb[fcb_son].fileContent;
				break;
			}
			else
				fcb_son = fcb[fcb_son].rFCB;//ͬ���ƶ�����
		}
		read(ibNum);
		printf("%s", content);
		printf("\n");
		//close();
	}
}

//�����ļ���fcb���кţ������ھͷ���-1
int FileManage::fcbSearch(char *filename)
{
	int columnFcbNum = fcb[0].sFCB;
	if (columnFcbNum == -1)
		return -1;//������
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
		return -1;//������
	}
}

//��ָ��·���µ��ļ��ƶ����µ�·����
void FileManage::move(char *filename, char * dirname)
{
	int fileflag = 0;
	int dirflag = 0;
	int moveflag;
	//���Ȳ��ҵ�ǰĿ¼���Ƿ���ڴ��ļ�
	int tempFcbNum = fcb[fcb_cur].rFCB;
	if (tempFcbNum == -1) {
		printf("�����ڴ��ļ������������롣\n");
		return;
	}
	do
	{
		if (fcb[tempFcbNum].fileName == filename)
		{
			fileflag = tempFcbNum;
			break;//�����ļ����˳�ѭ��
		}
		else
		{
			tempFcbNum = fcb[tempFcbNum].rFCB;
		}
	} while (fcb[tempFcbNum].rFCB == -1);
	if (fileflag = 0) {
		printf("�����ڴ��ļ������������롣\n");
		return;
	}
	else
	{
		//fileflag��Ҫ�ƶ����ļ���fcb�ţ�dirflag��Ŀ¼��fcb��
		dirflag = fcbSearch(dirname);
		if (dirflag == -1)
		{
			printf("�����ڸ�Ŀ¼�����½����������롣\n");
			return;
		}
		else
		{
			if (fcb[dirflag].sFCB == -1)//Ŀ¼��û���ļ�
			{
				fcb[fileflag].fFCB = dirflag;
				fcb[fileflag].lFCB = -1;
				fcb[fileflag].rFCB = -1;
				fcb[dirflag].sFCB = fileflag;
				return;
			}
			//Ŀ¼�����ļ�
			moveflag = fcb[dirflag].sFCB;
			//�ƶ�moveflagֱ�����ֵ��ǿ�
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

/*
//ɾ���ļ���fcb_cur�ı�Ϊ��ɾ���ļ��ĸ��ڵ�
bool FileManage::delFile(char* filename)
{
	char flag;
	if (cd(filename) == 0)
		return 0;
	struct FCB *currentFCB = &fcb[fcb_cur];//��ɾ���ļ�
	fcb_cur = fcb[currentFCB->fFCB].FCBNum;//fcb_curΪ��ǰ�ļ��ĸ��ڵ�
again:
	cout << "ȷ��ɾ��" << filename << "[y,n]?" << endl;
	cin >> flag;
	if (flag == 'n' || flag == 'N')//ȷ�ϲ�ɾ��
		return 0;
	else if (flag == 'y' || flag == 'Y')//ȷ��ɾ��
	{
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
			return 1;
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
				return 1;
			}
			else//�ӽڵ�Ϊ��
			{
				return 1;
			}
		}
	}
	else//��������
	{
		goto again;
	}

}*/