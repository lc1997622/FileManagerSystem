// FileManagerSystem.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <string.h>
#include "FileManage.h"
#include <stdio.h>


#define BLKSIZE 4096
#define BLKNUM 262144
#define DISKSIZE 1073741824
using namespace std;

FileManage*  fm = new FileManage();
void help(string aommand);
int main()
{
	char command[20];
	cout << "                                                              " << endl;
	cout << "        Welcome to Mobile Garverment FileManngeSystem" << endl;
	cout << "--------------------------------------------------------------" << endl;
	cout << "                                                              " << endl;
	cout << "min-FS:\\>";
	cin >> command;
	while (strcmp(command,"quit") != 0)     
	{
		if (strcmp(command,"create") == 0)
		{
			cin >> command;
			fm->create(command);
			cout << "创建成功" << endl;
		}
		else if(strcmp(command, "mount") == 0)
		{
			cin >> command;
			fm->mount(command);
			cout << "安装成功" << endl;
		}
		else if (strcmp(command,"help") == 0)
		{
			char b;
			cin.get(b);
			if (b == '\n')
			{
				help("no");
			}
			else if(b == ' ')
			{
				cin >> command;
				help(command);
			}
		}
		else {
			cout << "command error" << endl;
		}
		cout << "min-FS:\\>";
		cin >> command;
	}
}

void help(string command)
{
	if (strcmp(command.c_str(),"no") == 0)
	{
		cout << "关于某个命令的详细信息，请键入help" << endl;
		cout << "create      建立一个新的min-FS系统。" << endl;
		cout << "mount       安装一个min-FS 系统" << endl;
		cout << "close       退出打开的min-FS 系统" << endl;
		cout << "new         创建文件" << endl;
		cout << "tp          显示文件" << endl;
		cout << "more        分页显示文件" << endl;
		cout << "dr          显示文件目录" << endl;
		cout << "att         显示文件属性" << endl;
		cout << "dl          删除文件" << endl;
		cout << "mkdir       创建文件夹" << endl;
		cout << "rmdir       删除文件夹" << endl;
		cout << "ls          显示前目录" << endl;
		cout << "cd          更改当前目录" << endl;
	}
	else if (strcmp(command.c_str(), "create") == 0)
	{
		cout << "create      建立一个新的min-FS系统。" << endl;
	}
	else if (strcmp(command.c_str(), "mount") == 0)
	{
		cout << "mount       安装一个min-FS 系统" << endl;
	}
	else if (strcmp(command.c_str(), "close") == 0)
	{
		cout << "close       退出打开的min-FS 系统" << endl;
	}
	else if (strcmp(command.c_str(), "new") == 0)
	{
		cout << "new         创建文件" << endl;
	}
	else if (strcmp(command.c_str(), "tp") == 0)
	{
		cout << "tp          显示文件" << endl;
	}
	else if (strcmp(command.c_str(), "more") == 0)
	{
		cout << "more        分页显示文件" << endl;
	}
	else if (strcmp(command.c_str(), "dr") == 0)
	{
		cout << "dr          显示文件目录" << endl;
	}
	else if (strcmp(command.c_str(), "att") == 0)
	{
		cout << "att         显示文件属性" << endl;
	}
	else if (strcmp(command.c_str(), "dl") == 0)
	{
		cout << "dl          删除文件" << endl;
	}
	else if (strcmp(command.c_str(), "mkdir") == 0)
	{
		cout << "mkdir       创建文件夹" << endl;
	}
	else if (strcmp(command.c_str(), "rmdir") == 0)
	{
		cout << "rmdir       删除文件夹" << endl;
	}
	else if (strcmp(command.c_str(), "ls") == 0)
	{
		cout << "ls          显示前目录" << endl;
	}
	else if (strcmp(command.c_str(), "cd") == 0)
	{
		cout << "cd          更改当前目录" << endl;
	}
}
