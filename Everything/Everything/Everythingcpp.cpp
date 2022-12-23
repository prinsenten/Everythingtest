#include"Everything.h"
#include<cstring>
//// 以上为c++17以上才含有的库文件 可以直接遍历出指定盘符下的文件
//int main() 
// {
//	for (auto& i : filesystem::directory_iterator("D:\\")) {
//		cout << i.path().string() << endl;
//	}
//}
int main()
{
	vector<string>filename;//vector容器接收文件名
	string path("D:\\test");//指定路径
	GetFiles1(path,filename);
	for (const auto& ph : filename)
		cout << ph << endl;
	return 0;
}


