#include<iostream>
#include<filesystem>
#include<string>
using namespace std;
#include<io.h>
#include<vector>
//// ����Ϊc++17���ϲź��еĿ��ļ� ����ֱ�ӱ�����ָ���̷��µ��ļ�
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
	unsigned attrib;       //���ڱ�ʾ�ļ�����(�����浵�����أ�ֻ�����ļ��е�)
	time_t time_create;    //�ļ�����ʱ��
	time_t time_access;    //�ļ����һ�δ�ʱ��
	time_t time_write;	   //�ļ����һ���޸�ʱ��
	_fsize_t size;	       //�ļ���С
	char name[_MAX_FNAME]; //�ļ�����
};
*/
//long _findfirst(char* filespec, struct _finddata_t* fileinfo)��//�ɹ����ؾ����findnextʹ�ã�ʧ�ܷ���-1
void getFiles(string path, vector<string>& file)
{
	intptr_t hald=0;
	struct _finddata_t fileinfo;
	string p;
	if ((hald =_findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		//�鿴�ڲ��ļ����Ƿ����
		//���ھ͵ݹ����
		do
		{
			if (fileinfo.attrib & _A_SUBDIR)//_A_SUBDIR�ļ�������
			{
				if (strcmp(fileinfo.name,".")!=0 &&strcmp(fileinfo.name,"..")!=0)
				{
					getFiles(p.assign(path).append("\\").append(fileinfo.name), file);
				}
				else
					file.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hald,&fileinfo)==0);//_findnext�ɹ�����0��ʧ�ܷ���-1
		_findclose(hald);
	}
}
