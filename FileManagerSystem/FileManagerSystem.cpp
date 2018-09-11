// FileManagerSystem.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

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
	char command[100];
	char command2[100];
	int mountflag = 0;
	cout << "                                                              " << endl;
	cout << "        Welcome to Mobile Garverment FileManngeSystem" << endl;
	cout << "--------------------------------------------------------------" << endl;
	cout << "                                                              " << endl;
	cout << "mini-FS:/>";
	cin >> command;
	while (1)     
	{
		if (strcmp(command,"create") == 0)
		{
			cin >> command;
			if( fm->create(command) == 0 )
				cout << "创建成功" << endl;
			else
			{
				cout << "创建失败" << endl;
			}
		}
		else if(strcmp(command, "mount") == 0)
		{
			cin >> command;
			if (fm->mount(command)==0)
			{
				cout << "该空间不存在，请创建" << endl;
			}
			else
			{
				cout << "安装成功" << endl;
				mountflag = 1;
			}
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
		else if (strcmp(command, "move") == 0)
		{
			cin >> command;
			cin >> command2;
			fm->move(command,command2);
			cout << endl;
		}
		else if(strcmp(command, "quit") == 0) {
			fm->close();
		}
		else if (strcmp(command, "new") == 0) {
			cin >> command;
			if (fm->newFile(command)) {
				cout << "创建成功" << endl;
			}
			else {
				cout << "创建失败" << endl;
			}
		
		}
		else if (strcmp(command,"mkdir") == 0)
		{
			string str;
			cin >> str;
			fm->mkdir(str);
			cout << endl;
		}
		else if (strcmp(command, "cd") == 0)
		{
			string str;
			cin >> str;
			fm->cd(str);
			cout << endl;
		}
		else if (strcmp(command, "ls") == 0)
		{
			fm->showDir();
			cout << endl;
		}
		else if (strcmp(command, "att") == 0)
		{
			char str[20];
			cin >> str;
			fm->showAtt(str);
			cout << endl;
		}
		else if (strcmp(command, "dl") == 0)
		{
			char str[100];
			cin >> str;
			if (fm->delFile(str))
				cout << "删除成功" << endl;
			else
				cout << "删除失败" << endl;
			cout << endl;
		}
		else if (strcmp(command, "write") == 0)
		{
			char str[20];
			cin >> str;
			fm->write(str);
			cout << endl;
		}
		else
		{
			cout << "指令错误" << endl;
		}
		if (mountflag == 1)
		{
			cout << fm->systemName << fm->curpath << ">";
		}
		else
			cout << "mini-FS:/>";
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
		cout << "create [SFName]" << endl;
	}
	else if (strcmp(command.c_str(), "mount") == 0)
	{
		cout << "mount       安装一个min-FS 系统" << endl;
		cout << "monut [SFName]" << endl;
	}
	else if (strcmp(command.c_str(), "close") == 0)
	{
		cout << "close       退出打开的min-FS 系统" << endl;
		cout << "close" << endl;
	}
	else if (strcmp(command.c_str(), "new") == 0)
	{
		cout << "new         创建文件" << endl;
		cout << "new [FileName]" << endl;
	}
	else if (strcmp(command.c_str(), "tp") == 0)
	{
		cout << "tp          显示文件" << endl;
		cout << "tp [FileName]" << endl;
	}
	else if (strcmp(command.c_str(), "more") == 0)
	{
		cout << "more        分页显示文件" << endl;
		cout << "more [FileName]" << endl;
	}
	else if (strcmp(command.c_str(), "dr") == 0)
	{
		cout << "dr          显示文件目录" << endl;
		cout << "dr [FileName]" << endl;
	}
	else if (strcmp(command.c_str(), "att") == 0)
	{
		cout << "att         显示文件属性" << endl;
		cout << "att [FileName]" << endl;
	}
	else if (strcmp(command.c_str(), "dl") == 0)
	{
		cout << "dl          删除文件" << endl;
		cout << "dl [FileName]" << endl;
	}
	else if (strcmp(command.c_str(), "mkdir") == 0)
	{
		cout << "mkdir       创建文件夹" << endl;
		cout << "mkdir [FileName]" << endl;
	}
	else if (strcmp(command.c_str(), "rmdir") == 0)
	{
		cout << "rmdir       删除文件夹" << endl;
		cout << "rmdir [FileName]" << endl;
	}
	else if (strcmp(command.c_str(), "ls") == 0)
	{
		cout << "ls          显示前目录" << endl;
		cout << "ls" << endl;
	}
	else if (strcmp(command.c_str(), "cd") == 0)
	{
		cout << "cd          更改当前目录" << endl;
		cout << "cd [Path]" << endl;
	}
}
