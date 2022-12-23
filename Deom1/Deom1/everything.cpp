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

//dfs�ݹ�
void GetFiles1(string path, vector<string>& files)
{
	intptr_t Hfile = 0;
	string p;
	struct _finddata_t fileinfo; //�⺯��
	if ((Hfile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�ж��Ƿ�Ϊ�ļ��У��ǵĻ��ͽ�ȥ�����ظ��жϣ��ʿ��õݹ�ʵ��
			if (fileinfo.attrib & _A_SUBDIR)
			{
				//��ϵͳ�д�һ���ļ���ǰ����Ĭ���ǡ�.����".."��������ѹ���ļ��п�����һ����
				//���������һ��ᵼ�����޵�����
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


//dfs �ǵݹ� ��ôѹջ
void GetFiles2(string path, vector<string>& files)
{
	intptr_t Hfile = 0;
	string p;
	struct _finddata_t fileinfo; //�⺯��
	//��ջ�ķ������ȱ�����ǰ�ڵ㣬Ȼ�����Һ��ӣ���ѹ���ӡ�Ȼ���ظ�����
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

//bfs //������ô�ڶ������
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
	vector<string>filename;//vector���������ļ���
	string path("D:\\Everything");//ָ��·��
	GetFiles1(path, filename);
	for (const auto& ph : filename)
		cout << ph << endl;
	return 0;
}