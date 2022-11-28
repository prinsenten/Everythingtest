#include"Everything.h"
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
bool GetNTFS(LPCSTR volName, DISK_GEOMETRY* pdg)
{
	//1.�ж��Ƿ�ΪNTFS��
	bool isNTFS = false;
	char sysNameBuf[MAX_PATH] = { 0 };
	int status = GetVolumeInformationA(volName,
										NULL,
										0,
										NULL,
										NULL,
										NULL,
										sysNameBuf,
										MAX_PATH);

	if (0 == status)
	{
		if (0 == strcmp(sysNameBuf, "NTFS")) {
			cout << "����ΪNTFS��ʽ" << endl;
			isNTFS = true;
		}
		else
			cout << "���̷�NTFS��ʽ" << endl;
	}

	//2.��ȡ�����̾��
	char fileName[MAX_PATH];
	fileName[0] = '\0';
	bool getHandleSucces = false;

	strcpy(fileName, "\\\\.\\");
	strcat(fileName, volName);

	string fileNameStr = (string)fileName;
	fileNameStr.erase(fileNameStr.find_last_of(":") + 1);

	cout << "�����̵�ַ��%s\n" << fileNameStr << endl;

	HANDLE handl = CreateFileA(fileNameStr.data(),					//��Ҫ���ļ�������
		GENERIC_READ | GENERIC_WRITE,		//������豸���ж�д
		FILE_SHARE_READ | FILE_SHARE_WRITE,	//������ļ����й������
		NULL,								//Ĭ�ϰ�ȫ����
		OPEN_EXISTING,						//�ļ������Ѿ����ڣ����豸���Ҫ��
		FILE_ATTRIBUTE_READONLY,			//�ļ�����ֻ��
		NULL);								//�����ƾ��

	if (INVALID_HANDLE_VALUE != handl)
		getHandleSucces = true;
	else
		cout << "��ȡ������ʧ��" << endl;


	//3.��ʼ��USN��־�ļ�
	bool initUsnJournalSuccess = false;
	DWORD br;
	CREATE_USN_JOURNAL_DATA cujd;
	cujd.MaximumSize = 0;//ʹ��Ĭ��ֵ
	cujd.AllocationDelta = 0;//ʹ��Ĭ��ֵ
	status = DeviceIoControl(handl,						//���
		FSCTL_CREATE_USN_JOURNAL,	//
		&cujd,						//cujd����
		sizeof(cujd),				//���ݴ�С
		NULL,						//�����������С0
		0,							//
		&br,						//ָ����ջ�����ָ��
		NULL);						//

	if (0 != status)
		initUsnJournalSuccess = true;
	else
		cout << "��ʼ��UNSN��־ʧ��" << endl;

	//4.��ȡ��־����
	bool getBasicInfoSuccess = false;

	DWORD bw;
	status = DeviceIoControl(handl,
		FSCTL_QUERY_USN_JOURNAL,
		NULL, 0,
		&pdg,
		sizeof(pdg),
		&bw,
		NULL);
	if (0 != status)
		getBasicInfoSuccess = true;
	else
	{
		cout << "��ȡUSN��־������Ϣʧ�� ���� status:%x " << status << "error:%d\n" << GetLastError() << endl;
	}
}

int main()
{
	DISK_GEOMETRY pdg = { 0 };
	GetNTFS("c", &pdg);


}
