#include<iostream>
#include<filesystem>
#include<string>
using namespace std;
#include<io.h>
#include<vector>
#include<windows.h>
#include<algorithm>
#include<string.h>
#include<stack>
#include<queue>

//dfs递归
void GetFiles1(string path, vector<string>& files)
{
	intptr_t Hfile = 0;
	string p;
	struct _finddata_t fileinfo; //库函数
	if ((Hfile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//判断是否为文件夹，是的话就进去继续重复判断，故可用递归实现
			if (fileinfo.attrib & _A_SUBDIR)
			{
				//在系统中打开一个文件夹前两个默认是“.”和".."（就像在压缩文件中看到的一样）
				//如果不加这一句会导致无限迭代。
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					GetFiles1(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(Hfile, &fileinfo) == 0);
		_findclose(Hfile);
	}
}


//dfs 非递归 怎么压栈
void GetFiles2(string path, vector<string>& files)
{
	intptr_t Hfile = 0;
	string p;
	struct _finddata_t fileinfo; //库函数
	//用栈的方法，先遍历当前节点，然后先右孩子，再压左孩子。然后重复操作
	stack<string> stack;
	if ((Hfile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		stack.push(p);
		while (!stack.empty())
		{
			p = stack.top();
			files.push_back(p);
			stack.pop();
			if (fileinfo.attrib & _A_SUBDIR)
			{

			}


		}
	}
}

//bfs //队列怎么第二遍遍历
void GetFiles3(string path, vector<string>& files)
{
	intptr_t Hfile = 0;
	string p;
	struct _finddata_t fileinfo;
	queue<string> queue;
	queue.push(p.assign(path));
	while (!queue.empty())
	{
		if ((Hfile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
		{
			do
			{
				if (fileinfo.attrib & _A_SUBDIR)
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
						queue.push(p.assign(path).append("\\").append(fileinfo.name));
				}
				else
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			} while (_findnext(Hfile, &fileinfo) == 0);
		}
	}
	_findclose(Hfile);
}
int main()
{
	vector<string>filename;//vector容器接收文件名
	string path("D:\\Everything");//指定路径
	GetFiles1(path, filename);
	for (const auto& ph : filename)
		cout << ph << endl;
	return 0;
}