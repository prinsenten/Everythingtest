#include"Everything.h"

///*struct _finddata_t
//{
//	unsigned attrib;       //用于表示文件属性(包括存档，隐藏，只读，文件夹等)
//	time_t time_create;    //文件创造时间
//	time_t time_access;    //文件最后一次打开时间
//	time_t time_write;	   //文件最后一次修改时间
//	_fsize_t size;	       //文件大小
//	char name[_MAX_FNAME]; //文件名称
//};
//*/
//long _findfirst(char* filespec, struct _finddata_t* fileinfo)；//成功返回句柄被findnext使用，失败返回-1
void GetFiles(string path, vector<string>& files)
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
					GetFiles(p.assign(path).append("\\").append(fileinfo.name), files);
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

