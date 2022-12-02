#include"test.h"
#include<cstring>
//// ����Ϊc++17���ϲź��еĿ��ļ� ����ֱ�ӱ�����ָ���̷��µ��ļ�
//int main() 
// {
//	for (auto& i : filesystem::directory_iterator("D:\\")) {
//		cout << i.path().string() << endl;
//	}
//}
//int main()
//{
//	vector<string>filename;//vector���������ļ���
//	string path("E:\\");//ָ��·��
//	GetFiles(path,filename);
//	for (const auto& ph : filename)
//		cout << ph << endl;
//	return 0;
//}
// ��ȡ�̷���Ϣ����
//DWORD WINAPI GetLogicalDriveStrings{
//	_In_  DWORD  nBufferLength,//��TCHAR�У���lpBufferָ��Ļ�����������С�� �ô�С��������ֹ���ַ��� ����˲���Ϊ�㣬��ʹ��lpBuffer��
//	_Out_ LPTSTR lpBuffer//ָ�򻺳�����ָ�룬������һϵ����null��β���ַ�����һ������ϵͳ�е�ÿ����Ч�����������⻹��һ������Ŀ��ַ��� ÿ���ַ��������豸���ơ�
//};


/*  ��ȡ�̷���Ϣ����Ҫ�ĺ���
GetVolumeInformation(
	lpRootPathName: PChar; { ���������������ַ���}
lpVolumeNameBuffer: PChar; { �����������������}
nVolumeNameSize: DWORD; { ����������������Ƴ���}
lpVolumeSerialNumber: PDWORD; { ����������������к�}
var lpMaximumComponentLength : DWORD; { ϵͳ���������ļ�������}
var lpFileSystemFlags : DWORD; { �ļ�ϵͳ��ʶ}
lpFileSystemNameBuffer: PChar; { �ļ�����ϵͳ����}
nFileSystemNameSize: DWORD{ �ļ�����ϵͳ���Ƴ��� }
) : BOOL;
*/

#define BUF_LEN 4096

bool GetNTFS(LPCSTR volName)
{
	//1.�ж��Ƿ�ΪNTFS��
	char sysNameBuf[MAX_PATH] = { 0 };
	GetVolumeInformationA(volName,			//�ļ�·��
		NULL,			//��������Ƕ�����Ҫ��ȡ
		0,				//
		NULL,			//
		NULL,			//
		NULL,			//
		sysNameBuf,		//�ļ�ϵͳ���� �����ǻ�ȡntfs��Ҫ��
		MAX_PATH);		//���Ƴ���
	printf(" �ļ�ϵͳ�� : %s\n", volName);
	if (0 == strcmp(sysNameBuf, "NTFS"))
		cout << "����ΪNTFS��ʽ" << endl;
	else
	{
		cout << "���̷�NTFS��ʽ" << endl;
		return 0;
	}

	//2.��ȡ�����̾��
	char fileName[MAX_PATH];
	fileName[0] = '\0';
	// ������ļ�������Ϊ\\.\C:����ʽ  
	strcpy_s(fileName, "\\\\.\\");
	strcat_s(fileName, volName);

	string fileNameStr = (string)fileName;
	fileNameStr.erase(fileNameStr.find_last_of(":") + 1);

	cout << "�����̵�ַ:" << fileNameStr << endl;

	//�����ù���Ա��ݴ򿪲��ܳɹ�
	HANDLE handl = CreateFileA(fileNameStr.data(),	//��Ҫ���ļ�������
		GENERIC_READ | GENERIC_WRITE,		//������豸���ж�д
		FILE_SHARE_READ | FILE_SHARE_WRITE,	//������ļ����й������
		NULL,								//Ĭ�ϰ�ȫ����
		OPEN_EXISTING,						//�ļ������Ѿ����ڣ����豸���Ҫ��
		FILE_ATTRIBUTE_READONLY,			//�ļ�����ֻ��
		NULL);								//�����ƾ��

	if (INVALID_HANDLE_VALUE != handl)
		printf("��ȡ����ɹ�\n");
	else
	{
		cout << "��ȡ������ʧ��" << endl;
		//return 0;
	}
	//3.��ʼ��USN��־�ļ�
	DWORD br;
	CREATE_USN_JOURNAL_DATA cujd;
	cujd.MaximumSize = 0;//ʹ��Ĭ��ֵ
	cujd.AllocationDelta = 0;//ʹ��Ĭ��ֵ
	int 
		status = DeviceIoControl(handl,	//���
		FSCTL_CREATE_USN_JOURNAL,	//����־
		&cujd,						//cujd����
		sizeof(cujd),				//���ݴ�С
		NULL,						//�����������С0
		0,							//
		&br,						//ָ����ջ�����ָ��
		NULL);						//

	if (0 != status)
		cout << "��ʼ��UNSN��־�ɹ�" << endl;
	else
	{
		cout << "��ʼ��UNSN��־ʧ��" << endl;
	//	return 0;
	}
	
	//4.��ȡ��־����
	USN_JOURNAL_DATA UsnInfo;
	status = DeviceIoControl(handl,
		FSCTL_QUERY_USN_JOURNAL,
		NULL, 0,
		&UsnInfo,
		sizeof(UsnInfo),
		&br,
		NULL);
	if (0 != status)
		cout << "��ȡ��־�ɹ�" << endl;
	else
	{
		cout << "��ȡUSN��־������Ϣʧ�� ���� status:" << status << "   error:" << GetLastError() << endl;
	//	return 0;
	}
	
	//5.�г�usn�ļ�
	MFT_ENUM_DATA med;
	med.StartFileReferenceNumber = 0;
	med.LowUsn = 0;//UsnInfo��first
	med.HighUsn = UsnInfo.NextUsn;//��NEXT
	char buffer[BUF_LEN];//�洢��¼����
	DWORD usnDataSize;
	PUSN_RECORD UsnRecord;
	while (0 != DeviceIoControl(handl,
		FSCTL_ENUM_USN_DATA,
		&med,
		sizeof(med),
		buffer,
		BUF_LEN ,
		&usnDataSize,
		NULL));
	{
		DWORD dwRetBytes = usnDataSize - sizeof(USN);
		UsnRecord = (PUSN_RECORD)(((PCHAR)buffer) + sizeof(USN));
		int iSize;
		while (dwRetBytes > 0)
		{
			iSize = WideCharToMultiByte(CP_OEMCP, 0, UsnRecord->FileName, -1, NULL, 0, NULL, NULL);
			char* filesName;
			filesName = (char*)malloc(iSize * sizeof(char));
			//��ӡ��ȡ������Ϣ
			//int strLen = UsnRecord->FileNameLength;
			WideCharToMultiByte(CP_OEMCP,//��ǰϵͳOEM����ҳ��һ��ԭʼ�豸������Ӳ��ɨ����
				0,//һ��Ϊ0������Ч�ʸ���
				UsnRecord->FileName,//��ת���Ŀ��ַ���
				-1,//��ת���ַ�������
				filesName,//����ת��������´��Ļ�����
				iSize,//�����������С
				NULL,//ϵͳĬ���ַ�
				NULL);//���ر�����ָ��
			printf("FileName:%s\n", filesName);
			//��ȡ�ļ�·����Ϣ
			printf("FileReferenceNumber:%xI64\n", UsnRecord->FileReferenceNumber);
			printf("ParentFileReferenceNumber:%xI64\n", UsnRecord->ParentFileReferenceNumber);
			printf("\n");
			char* pathBuffer[BUF_LEN];

			//��ȡ��һ����¼
			DWORD recordLen = UsnRecord->RecordLength;
			dwRetBytes -= recordLen;
			UsnRecord = (PUSN_RECORD)((PCHAR)UsnRecord + recordLen);
		}
		med.StartFileReferenceNumber = *(USN*)&buffer;
	}
	///** * step 06. ɾ�� USN ��־�ļ� ( ��ȻҲ���Բ�ɾ�� ) */
	//DELETE_USN_JOURNAL_DATA dujd;
	//dujd.UsnJournalID = UsnInfo.UsnJournalID;
	//dujd.DeleteFlags = USN_DELETE_FLAG_DELETE;
	//status = DeviceIoControl(handl,
	//	FSCTL_DELETE_USN_JOURNAL,
	//	&dujd,
	//	sizeof(dujd),
	//	NULL,
	//	0,
	//	&br,
	//	NULL);
	//if (0 != status)
	//{
	//	printf(" �ɹ�ɾ�� USN ��־�ļ� !\n");
	//}
	//else
	//{
	//	printf(" ɾ�� USN ��־�ļ�ʧ�� ���� status:%x error:%d\n", status, GetLastError());
	//}
	//return true;
}

int main()
{
	DISK_GEOMETRY pdg = { 0 };
	GetNTFS("C:\\");

	system("pause");
}



//
//#include"fileInfo.cpp"
//#include"FrnFilePath.cpp"
//#include<list>
//#include<map>
//#include<stack>
//
//
//list<fileInfo> result;
//map <string, FrnFilePath> fileHash;
//
////�Ѳ���ֵ����map
//void fun(fileInfo& fi)
//{
//	FrnFilePath* ff = new FrnFilePath(fi.filename, fi.frn, fi.pfrn,"");
//	fileHash.insert(make_pair(fi.frn, *ff));
//}
//
////
//void clear(stack<string> treestack)
//{
//	while (!treestack.empty())
//		treestack.pop();
//}
////��·���������
//static string BuildPath(string currentNode, string parentNode)
//{
//	return parentNode + "\\" + currentNode;
//}
//
//
//int main()
//{
//	//�Ȳ������"D:" ��ǰΪ9���޸�Ŀ¼��·��"D:"
//	string drivefrn = "9";
//	FrnFilePath* ff = new FrnFilePath("D:", drivefrn, "", "D:");
//	fileHash.insert(make_pair(drivefrn, *ff));
//
//	//����a,b,c,d,e,f��������
//	fileInfo* fi = new fileInfo("a", "1", "2");
//	result.push_back(*fi);
//	fi = new fileInfo("b", "2", "4");
//	result.push_back(*fi);
//	fi = new fileInfo("c", "5", "3");
//	result.push_back(*fi);
//	fi = new fileInfo("d", "4", "6");
//	result.push_back(*fi);
//	fi = new fileInfo("e", "3", "9");
//	result.push_back(*fi);
//	fi = new fileInfo("f", "6", "9");
//	result.push_back(*fi);
//	//��a,b,c,d,e,f������map��
//	for_each(result.begin(), result.end(), fun);
//
//	//��Ҫһ��ջ���洢��ʱ�ò�������Ŀ¼
//	stack<string> treestack;
//
//	auto iter = fileHash.begin();
//	//��map������һ��
//	while (iter != fileHash.end())
//	{
//		//ÿ�ο�ʼ�����ջ�Է���һ
//		clear(treestack);
//		//��ǰλ��
//		FrnFilePath current = iter->second;
//		//�����ǰλ��·��Ϊ�գ����и�Ŀ¼�ͽ�ȥ
//		if (current.path.empty() && !current.pfrn.empty())
//		{
//			//ȡ����Ŀ¼��Ϣ
//			FrnFilePath parent = fileHash[current.pfrn];
//			//�����Ŀ¼·��Ϊ�գ��Ҹ�Ŀ¼���и�Ŀ¼
//			while (parent.path.empty() && !parent.pfrn.empty())
//			{
//				//������ʱ�������ѵ�ǰĿ¼��ɸ�Ŀ¼
//				FrnFilePath temp = current;
//				current = parent;
//				//�����ǰĿ¼·��Ϊ�գ�����Ŀ¼
//				if (current.path.empty() && !current.pfrn.empty())
//				{
//					//�ѵڴ洢����Ŀ¼����ջ�У�Ȼ��Ѹ�Ŀ¼��ɸı���ĵ�ǰĿ¼�ĸ�Ŀ¼
//					treestack.push(temp.frn);
//					parent = fileHash[current.pfrn];
//				}
//			}
//			//��ǰĿ¼·����
//			current.path = BuildPath(current.filename, parent.path);
//			//���ջ��Ϊ�վ�һֱ����
//			while (!treestack.empty())
//			{
//				//ȡͷȥͷ
//				string head = treestack.top();
//				treestack.pop();
//				FrnFilePath headNode = fileHash[head];
//				//ƴ��Ŀ¼
//				current.path = BuildPath(headNode.filename, current.path);
//			}
//			iter->second.path = current.path;
//		}
//		iter++;
//	}
//
//	iter = fileHash.begin();
//	while (iter != fileHash.end())
//	{
//		cout << iter->second.filename << " " << iter->second.path << endl;
//		iter++;
//	}
//
//}
//


	//���涼���������룬���ϴ��벻���� �����Լ�дһ��
	// 
	// 
	//����map����
	//while (iter != fileHash.end())
	//{
	//	clear(treestack);
	//	//�õ�ֵ
	//	FrnFilePath current = iter->second;
	//	FrnFilePath* parent=NULL ;
	//	//�����ǰ·��Ϊ�գ���Ŀ¼��Ϊ�ղ��Ҵ��ڸ�Ŀ¼
	//	if ((current.path.empty()) && !current.pfrn.empty() && fileHash.find(current.pfrn) != fileHash.end())
	//	{
	//		//��ǰĿ¼�ĸ�Ŀ¼Ϊ
	//		FrnFilePath parentVal = fileHash[current.pfrn];

	//		//�����Ŀ¼���и�Ŀ¼���ͽ���Ŀ¼��ɵ�ǰĿ¼����������û�и�Ŀ¼
	//		while (current.path.empty() && !current.pfrn.empty() && fileHash.find(current.pfrn) != fileHash.end())
	//		{
	//			FrnFilePath temp = current;
	//			current = parentVal;

	//			//�����ǰĿ¼��Ŀ¼��Ϊ�ա����Ҵ��ڸ�Ŀ¼���ͽ���һ�β��ҵĵ�ǰĿ¼����ջ��
	//			if (!current.pfrn.empty() && fileHash.find(current.pfrn) != fileHash.end())
	//			{
	//				treestack.push(temp.frn);
	//				parentVal = fileHash[current.pfrn];
	//			}
	//			else
	//			{
	//				parent = &parentVal;
	//				break;
	//			}
	//		}
	//		if (parent != NULL)
	//		{
	//			//��ǰĿ¼Ϊ ��Ŀ¼+��ǰ
	//			current.path = BuildPath(current, parentVal);
	//			//ջ��Ϊ�մ�������Ŀ¼
	//			while (!treestack.empty())
	//			{
	//				//�����������Ŀ¼ ����Ϊ��ǰĿ¼����Ŀ¼
	//				string walkedkey = treestack.top();
	//				//����Ȼ��ɾ����׼��ȡ�¸���Ŀ¼
	//				treestack.pop();
	//				//�õ�ջ�������������
	//				FrnFilePath walkedNode = fileHash[walkedkey];
	//				FrnFilePath parentNode = fileHash[walkedNode.pfrn];
	//				
	//				walkedNode.path = BuildPath(walkedNode, parentNode);
	//				current.path += walkedNode.path;
	//			}
	//			iter->second.path = current.path;
	//		}
	//	}
	//	iter++;
	//}
	//iter = fileHash.begin();
	//while (iter != fileHash.end())
	//{
	//	cout << iter->second.filename<<" " << iter->second.path << endl;
	//	iter++;
	//}



