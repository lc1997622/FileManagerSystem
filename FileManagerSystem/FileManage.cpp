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
}


FileManage::~FileManage()
{
	free(baseAddr);
}

int FileManage::create(char name[])
{
	string symPath ="D:/";
	symPath += name;
	fp =fopen(symPath.c_str(), "w+");
	fwrite(baseAddr, sizeof(char), sizeof(char) * DISKSIZE/10, fp);
	fclose(fp);
	free(baseAddr);
	return 1;
}
int FileManage::mount(char name[])
{
	string symPath = "D:/";
	symPath += name;
	systemName = name;
	fp = fopen(symPath.c_str(), "r+");
	fseek(fp, 0,SEEK_SET);
	fread(&fcbnum,4, 1, fp);
	fseek(fp, 4096, SEEK_SET);
	fread(bitmap, 1, 4104,fp);
	for (int i = 0; i < 1024; i++)
	{
		fread(&menu[i], 1, 64, fp);	
	}
	fclose(fp);
	return 0;
}

int FileManage::close()
{
	string symPath = "D:/";
	symPath += systemName;
	fp = fopen(symPath.c_str(), "r+");
	fseek(fp, 0, SEEK_SET);
	fwrite(&fcbnum, 4, 1, fp);
	fseek(fp, 4096, SEEK_SET);
	fwrite(bitmap, 1, 4104, fp);
	for (int i = 0; i < 4096; i++)
	{
		fwrite(&menu[i], 1, 64, fp);
	}
	fclose(fp);
	return 0;
}
int FileManage::format()
{
	memset(&fcbnum, 0,sizeof(int));
	memset(bitmap, 0, sizeof(bitmap));
	memset(menu, 0, sizeof(menu));
	close();
	return 1;
}