#include"Everything.h"
#include<cstring>
//// ����Ϊc++17���ϲź��еĿ��ļ� ����ֱ�ӱ�����ָ���̷��µ��ļ�
//int main() 
// {
//	for (auto& i : filesystem::directory_iterator("D:\\")) {
//		cout << i.path().string() << endl;
//	}
//}
int main()
{
	vector<string>filename;//vector���������ļ���
	string path("D:\\test");//ָ��·��
	GetFiles1(path,filename);
	for (const auto& ph : filename)
		cout << ph << endl;
	return 0;
}


