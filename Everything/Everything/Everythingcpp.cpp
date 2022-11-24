#include<iostream>
#include<filesystem>
#include<string>
using namespace std;
#include<io.h>
#include<vector>
//// 以上为c++17以上才含有的库文件 可以直接遍历出指定盘符下的文件
//int main() 
// {
//	for (auto& i : filesystem::directory_iterator("D:\\")) {
//		cout << i.path().string() << endl;
//	}
//}


void getFiles(string path,vector<string>& file);

int main()
{
	vector<string>filename;
	string path("D:\\");
	getFiles(path, filename);
	for (const auto& ph : filename) 
		cout << ph << "\n";
	return 0;
}

/*struct _finddata_t
{
	unsigned attrib;       //用于表示文件属性(包括存档，隐藏，只读，文件夹等)
	time_t time_create;    //文件创造时间
	time_t time_access;    //文件最后一次打开时间
	time_t time_write;	   //文件最后一次修改时间
	_fsize_t size;	       //文件大小
	char name[_MAX_FNAME]; //文件名称
};
*/
//long _findfirst(char* filespec, struct _finddata_t* fileinfo)；//成功返回句柄被findnext使用，失败返回-1
void getFiles(string path, vector<string>& file)
{
	intptr_t hald=0;
	struct _finddata_t fileinfo;
	string p;
	if ((hald =_findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		//查看内部文件夹是否存在
		//存在就递归调用
		do
		{
			if (fileinfo.attrib & _A_SUBDIR)//_A_SUBDIR文件夹属性
			{
				if (strcmp(fileinfo.name,".")!=0 &&strcmp(fileinfo.name,"..")!=0)
				{
					getFiles(p.assign(path).append("\\").append(fileinfo.name), file);
				}
				else
					file.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hald,&fileinfo)==0);//_findnext成功返回0，失败返回-1
		_findclose(hald);
	}
}
