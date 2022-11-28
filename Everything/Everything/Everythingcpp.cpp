#include"Everything.h"
#include<cstring>
//// 以上为c++17以上才含有的库文件 可以直接遍历出指定盘符下的文件
//int main() 
// {
//	for (auto& i : filesystem::directory_iterator("D:\\")) {
//		cout << i.path().string() << endl;
//	}
//}
//int main()
//{
//	vector<string>filename;//vector容器接收文件名
//	string path("E:\\");//指定路径
//	GetFiles(path,filename);
//	for (const auto& ph : filename)
//		cout << ph << endl;
//	return 0;
//}
// 获取盘符信息函数
//DWORD WINAPI GetLogicalDriveStrings{
//	_In_  DWORD  nBufferLength,//在TCHAR中，由lpBuffer指向的缓冲区的最大大小。 该大小不包括终止空字符。 如果此参数为零，则不使用lpBuffer。
//	_Out_ LPTSTR lpBuffer//指向缓冲区的指针，它接收一系列以null结尾的字符串，一个用于系统中的每个有效驱动器，另外还有一个额外的空字符。 每个字符串都是设备名称。
//};


/*  获取盘符信息所需要的函数
GetVolumeInformation(
	lpRootPathName: PChar; { 磁盘驱动器代码字符串}
lpVolumeNameBuffer: PChar; { 磁盘驱动器卷标名称}
nVolumeNameSize: DWORD; { 磁盘驱动器卷标名称长度}
lpVolumeSerialNumber: PDWORD; { 磁盘驱动器卷标序列号}
var lpMaximumComponentLength : DWORD; { 系统允许的最大文件名长度}
var lpFileSystemFlags : DWORD; { 文件系统标识}
lpFileSystemNameBuffer: PChar; { 文件操作系统名称}
nFileSystemNameSize: DWORD{ 文件操作系统名称长度 }
) : BOOL;
*/
bool GetNTFS(LPCSTR volName, DISK_GEOMETRY* pdg)
{
	//1.判读是否为NTFS盘
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
			cout << "磁盘为NTFS格式" << endl;
			isNTFS = true;
		}
		else
			cout << "此盘非NTFS格式" << endl;
	}

	//2.获取驱动盘句柄
	char fileName[MAX_PATH];
	fileName[0] = '\0';
	bool getHandleSucces = false;

	strcpy(fileName, "\\\\.\\");
	strcat(fileName, volName);

	string fileNameStr = (string)fileName;
	fileNameStr.erase(fileNameStr.find_last_of(":") + 1);

	cout << "驱动盘地址：%s\n" << fileNameStr << endl;

	HANDLE handl = CreateFileA(fileNameStr.data(),					//需要打开文件的名字
		GENERIC_READ | GENERIC_WRITE,		//允许对设备进行读写
		FILE_SHARE_READ | FILE_SHARE_WRITE,	//允许对文件进行共享访问
		NULL,								//默认安全属性
		OPEN_EXISTING,						//文件必须已经存在，有设备提出要求
		FILE_ATTRIBUTE_READONLY,			//文件属性只读
		NULL);								//不复制句柄

	if (INVALID_HANDLE_VALUE != handl)
		getHandleSucces = true;
	else
		cout << "获取驱动盘失败" << endl;


	//3.初始化USN日志文件
	bool initUsnJournalSuccess = false;
	DWORD br;
	CREATE_USN_JOURNAL_DATA cujd;
	cujd.MaximumSize = 0;//使用默认值
	cujd.AllocationDelta = 0;//使用默认值
	status = DeviceIoControl(handl,						//句柄
		FSCTL_CREATE_USN_JOURNAL,	//
		&cujd,						//cujd数据
		sizeof(cujd),				//数据大小
		NULL,						//输出缓冲区大小0
		0,							//
		&br,						//指向接收缓冲区指针
		NULL);						//

	if (0 != status)
		initUsnJournalSuccess = true;
	else
		cout << "初始化UNSN日志失败" << endl;

	//4.获取日志操作
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
		cout << "获取USN日志基本信息失败 —— status:%x " << status << "error:%d\n" << GetLastError() << endl;
	}
}

int main()
{
	DISK_GEOMETRY pdg = { 0 };
	GetNTFS("c", &pdg);


}
