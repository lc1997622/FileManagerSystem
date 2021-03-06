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
	char command[100] = {0};
	char command1[100];
	char command2[100];
	char command3[100];
	int commandflag = 0;
	int mountflag = 0;
	cout << "                                                              " << endl;
	cout << "        Welcome to Mobile Government FileManageSystem" << endl;
	cout << "--------------------------------------------------------------" << endl;
	cout << "                                                              " << endl;
	cout << "mini-FS:/>";
	gets_s(command);
	while (1)     
	{
		commandflag = sscanf(command, "%s%s%s", command1, command2,command3);
		if (commandflag == -1 && mountflag == 0)
		{
			cout << "mini-FS:/>";
			gets_s(command);
			continue;
		}
		else if(commandflag == -1 && mountflag == 1)
		{
			cout << fm->curpath << ">";
			gets_s(command);
			continue;
		}
		if (strcmp(command1,"create") == 0)
		{
			if (commandflag == 1)
			{
				cout << "命令格式不正确，输入help获取帮助" << endl;
				cout << "mini-FS:/>";
				gets_s(command);
				continue;
			}
			if( fm->create(command2) == 0 )
				cout << "创建成功" << endl;
			else
			{
				cout << "创建失败" << endl;
			}
		}
		else if(strcmp(command1, "mount") == 0)
		{
			if (commandflag == 1)
			{
				cout << "命令格式不正确，输入help获取帮助" << endl;
				cout << "mini-FS:/>";
				gets_s(command);
				continue;
			}
			if (fm->mount(command2)==0)
			{
				cout << "该空间不存在，请创建" << endl;
			}
			else
			{
				cout << "安装成功" << endl;
				mountflag = 1;
			}
		}
		else if (strcmp(command1,"help") == 0)
		{
			if (commandflag == 1)
			{
				help("no");
			}
			else
			{
				help(command2);
			}
		}
		
		else if(strcmp(command1, "quit") == 0) {
			fm->close();
			cout << "Bye" << endl;
			break;
		}
		else if (strcmp(command1, "new") == 0) {
			if (commandflag == 1)
			{
				cout << "命令格式不正确，输入help获取帮助" << endl;
				cout << fm->curpath << ">";
				gets_s(command);
				continue;
			}
			if (fm->newFile(command2)!=-1){
				cout << "创建成功" << endl;
			}
			else {
				cout << "创建失败" << endl;
			}
		
		}
		else if (strcmp(command1,"mkdir") == 0)
		{
			if (commandflag == 1)
			{
				cout << "命令格式不正确，输入help获取帮助" << endl;
				cout << fm->curpath << ">";
				gets_s(command);
				continue;
			}
			if (fm->mkdir(command2)) {
				cout <<  "创建成功" << endl;
			}
		}
		else if (strcmp(command1, "cd") == 0)
		{
			if (commandflag == 1 )
			{
				cout << "命令格式不正确，输入help获取帮助" << endl;
				cout << fm->curpath << ">";
				gets_s(command);
				continue;
			}
			fm->cd(command2);
			cout << endl;
		}
		else if (strcmp(command1, "ls") == 0)
		{
			fm->showDir();
			cout << endl;
		}
		else if (strcmp(command1, "format") == 0)
		{
			fm->format();
			mountflag = 0;	
			cout<< endl;
		}
		else if (strcmp(command1, "clear") == 0)
		{
			fm->clear();
			cout << endl;
		}
		else if (strcmp(command1, "att") == 0)
		{
			if (commandflag == 1)
			{
				cout << "命令格式不正确，输入help获取帮助" << endl;
				cout << fm->curpath << ">";
				gets_s(command);
				continue;
			}
			fm->showAtt(command2);
			cout << endl;
		}
		else if (strcmp(command1, "dl") == 0)
		{
			if (commandflag == 1)
			{
				cout << "命令格式不正确，输入help获取帮助" << endl;
				cout << fm->curpath << ">";
				gets_s(command);
				continue;
			}
		again:
			fm->dFlag = 0;
			char con;
			cout << "确认删除" << command2 << "[y,n]?";
			cin >> con ;
			getchar();
			if (con == 'n' || con == 'N')//确认不删除
				cout << endl;
			else if (con == 'y' || con == 'Y')
			{
				string path_tmp = fm->curpath;
				if (fm->delFile(command2))
					cout << "删除成功" << endl;
				else
					cout << "删除失败" << endl;
				fm->cd(path_tmp);
				cout << endl;
			}
			else
				goto again;
		}
		else if (strcmp(command1, "write") == 0)
		{
			if (commandflag == 1)
			{
				cout << "命令格式不正确，输入help获取帮助" << endl;
				cout << fm->curpath << ">";
				gets_s(command);
				continue;
			}
			fm->write(command2);
		}
		else if (strcmp(command1, "show") == 0)
		{
			if (commandflag == 1)
			{
				cout << "命令格式不正确，输入help获取帮助" << endl;
				cout << fm->curpath << ">";
				gets_s(command);
				continue;
			}
			fm->show(command2);
		}
		else if (strcmp(command1, "map") == 0)
		{
			if (commandflag == 1)
			{
				cout << "命令格式不正确，输入help获取帮助" << endl;
				cout << fm->curpath << ">";
				gets_s(command);
				continue;
			}
			fm->map(command2);
		}
		else if (strcmp(command1, "move") == 0)
		{
			//cout << command2 << command3 << endl;
			fm->move(command2, command3);
		}
		else if (strcmp(command1, "tree") == 0)
		{
			string path_tmp = fm->curpath;
			fm->tree();
			fm->cd(path_tmp);
		}
		else if (strcmp(command1, "copy") == 0) 
		{
			if (fm->copy(command2,command3)) {
				cout << "copy成功" << endl;
			}
			else {
				cout << "copy失败" << endl;
			}
		}
		else
		{
			cout << "指令错误" << endl;
		}
		if (mountflag == 1)
		{
			cout << fm->curpath << ">";
		}
		else
			cout << "mini-FS:/>";
		gets_s(command);
	}
}

void help(string command)
{
	if (strcmp(command.c_str(),"no") == 0)
	{
		cout << endl;
		cout << "			移动政务的mini-fs使用指南" << endl;
		cout << endl;
		cout << "	create + [文件名]             新建一个新的min-FS系统。" << endl;
		cout << "	mount  + [系统名]             安装一个mini-FS 系统" << endl;
		cout << "	new    + [文件名]             新建文件" << endl;
		cout << "	write  + [文件名]             向文件中写入内容" << endl;
		cout << "	show   + [文件名]             显示文件的内容" << endl;
		cout << "	move   + [目录名]             移动文件到新的目录下" << endl;
		cout <<	"	att    + [文件名]             显示文件属性（*实现统配功能）" << endl;
		cout << "	dl     + [文件名]             删除文件/文件夹（*实现通配功能）" << endl;
		cout << "	mkdir  + [目录名]             创建文件夹" << endl;
		cout << "	map    + [文件名]             显示文件盘块号" << endl;
		cout << "	ls     + [文件名]             显示当前目录下的文件和子目录名" << endl;
		cout << "	cd     + [目录名]             更改当前目录" << endl;
		cout << "	copy   + [文件名]             拷贝文件到新的目录下（不存在会新建）" << endl;
		cout << "	format                       格式化当前系统" << endl;
		cout << "	clear                        清空屏幕内容" << endl;
		cout << "	quit                         保存并且退出当前的mini-FS系统" << endl;
		cout << endl;
		cout << endl;
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
	else if (strcmp(command.c_str(), "copy") == 0) {
		cout << "copy          复制文件" << endl;
		cout << "copy [Path1,Path2]" << endl;
	}
}
