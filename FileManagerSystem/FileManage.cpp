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
		for(int j; j < 8; j++)
		{
			bitmap[8 * i + j] = (Map[i] & (1 << j)) / (1 << j);
		}
	}
	for (int i = 0; i < 1024; i++)
	{
		fread(&menu[i], 1, 64, fp);	
	}       
	fclose(fp);
	return 0;
}

int FileManage::close()
{
	char Map[32768];
	string symPath = "D:/";
	symPath += systemName;
	fp = fopen(symPath.c_str(), "r+");
	fseek(fp, 0, SEEK_SET);
	fwrite(&fcbnum, 4, 1, fp);
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