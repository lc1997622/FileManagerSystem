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

	//对fcb函数进行测试，初始化fcb的数组
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
//需加上对fcb的重新写入
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
	//开始写入fcb的值
	for (int i = 0; i < 512; i++) {
		//读取文件名
		fwrite(fcb[i].fileName, 1, 20, fp);
		//读取文件大小
		fwrite(&fcb[i].fileSize, 4, 1, fp);
		//写入文件类型
		fwrite(&fcb[i].fileType, 1, 1, fp);
		//读取文件修改时间
		fwrite(fcb[i].fileTime, 14, 1, fp);
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

////创建新的文件
//bool FileManage::newFile(char* oldfilename, char *filename) {
//	if (!cd(filename)) {
//		printf("获取当前路径失败");
//		return false;//获取当前的系统路径错误
//
//	}
//	struct FCB *newFcb;	//创建新的FCB
//	newFcb->initialize();
//	newFcb->fileName = filename;
//	newFcb->fileType = false;
//	newFcb->fileTime = "0";//之后讨论存取什么时间
//	nexFcb->delFlag = false;
//	newFcb->FCBNum = fcbnum;
//	//搜索位示图找到第一个为值为0的连续盘块号	
//	newFcb->fileSize = file_size(oldfilename);
//	int blockNum = newFcb->fileSize / 4096 + 1;//获取一个文件要存的盘块数
//	int count = 0;
//	int flag = 0;
//	int i;
//	for (i = 4105; i < BLKNUM; i++) {
//		if (bitmap[i] == false) {
//			//找到第一个为空的盘块;
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
//		newFcb->fileContent = i;//赋值该文件的起始的内容信息块
//		for (int t = 0; t <= count; t++) {
//			bitmap[t + i] = true;
//		}
//	}
//	else {
//		printf("创建失败");
//		return false;
//	}
//
//	//开始写文件，以一个盘块为单位进行写入
//	fp = fopen(minifile, "w");
//	fseek(fp, CONTENTSTART，SEEK_SET);
//
//	FILE *oldfp = fopen(oldfilename, "r");
//	if (oldfp == NULL) {
//		printf("获取路径失败");//获取操作系统下的路径失败；
//		return false;
//	}
//	char *temp[4096];
//	int remaining = newFcb->fileSize;//刚开始剩余的字节数就是总的字节数;
//	for (i = 0; i < blockNum; i++) {
//		if (remaining > 4096) {
//			//说明该盘块要存满
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
//	//当前的这个节点没有子节点，说明创建的这个节点是当前节点的第一个子节点
//	if (fcbPre->sFCB == -1) {
//		fcbPre.sFCB = newFcb->FCBNum;
//		newFcb->fFCB = fcbPre.FCBNum;
//	}
//	//当前这个节点有子节点，要找到当前节点的最后一个儿子节点
//	else {
//		//获取第一个兄弟节点
//		struct FCB broFcb = fcb[fcbPre.sFCB];
//		while (1) {
//			if (broFcb.rFCB == -1) {
//				//找到了最后一个兄弟节点
//				broFcb.rFCB = newFcb->FCBNUM;
//				newFcb->lFCB = broFcb.FCBNUM;
//				break;
//			}
//			//下一个兄弟节点为当前节点的右兄弟节点
//			broFcb = fcb[broFcb.rFCB];
//		}
//	}
//	fcb[fcbnum++] = newFcb;
//	return true;
//}