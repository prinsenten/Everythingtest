#include"Everything.h"

///*struct _finddata_t
//{
//	unsigned attrib;       //���ڱ�ʾ�ļ�����(�����浵�����أ�ֻ�����ļ��е�)
//	time_t time_create;    //�ļ�����ʱ��
//	time_t time_access;    //�ļ����һ�δ�ʱ��
//	time_t time_write;	   //�ļ����һ���޸�ʱ��
//	_fsize_t size;	       //�ļ���С
//	char name[_MAX_FNAME]; //�ļ�����
//};
//*/
//long _findfirst(char* filespec, struct _finddata_t* fileinfo)��//�ɹ����ؾ����findnextʹ�ã�ʧ�ܷ���-1
void GetFiles(string path, vector<string>& files)
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

