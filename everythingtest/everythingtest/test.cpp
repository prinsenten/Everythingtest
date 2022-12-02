#include"test.h"
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

#define BUF_LEN 4096

bool GetNTFS(LPCSTR volName)
{
	//1.判读是否为NTFS盘
	char sysNameBuf[MAX_PATH] = { 0 };
	GetVolumeInformationA(volName,			//文件路径
		NULL,			//下面的我们都不需要获取
		0,				//
		NULL,			//
		NULL,			//
		NULL,			//
		sysNameBuf,		//文件系统名称 是我们获取ntfs需要的
		MAX_PATH);		//名称长度
	printf(" 文件系统名 : %s\n", volName);
	if (0 == strcmp(sysNameBuf, "NTFS"))
		cout << "磁盘为NTFS格式" << endl;
	else
	{
		cout << "此盘非NTFS格式" << endl;
		return 0;
	}

	//2.获取驱动盘句柄
	char fileName[MAX_PATH];
	fileName[0] = '\0';
	// 传入的文件名必须为\\.\C:的形式  
	strcpy_s(fileName, "\\\\.\\");
	strcat_s(fileName, volName);

	string fileNameStr = (string)fileName;
	fileNameStr.erase(fileNameStr.find_last_of(":") + 1);

	cout << "驱动盘地址:" << fileNameStr << endl;

	//函数用管理员身份打开才能成功
	HANDLE handl = CreateFileA(fileNameStr.data(),	//需要打开文件的名字
		GENERIC_READ | GENERIC_WRITE,		//允许对设备进行读写
		FILE_SHARE_READ | FILE_SHARE_WRITE,	//允许对文件进行共享访问
		NULL,								//默认安全属性
		OPEN_EXISTING,						//文件必须已经存在，有设备提出要求
		FILE_ATTRIBUTE_READONLY,			//文件属性只读
		NULL);								//不复制句柄

	if (INVALID_HANDLE_VALUE != handl)
		printf("获取句柄成功\n");
	else
	{
		cout << "获取驱动盘失败" << endl;
		//return 0;
	}
	//3.初始化USN日志文件
	DWORD br;
	CREATE_USN_JOURNAL_DATA cujd;
	cujd.MaximumSize = 0;//使用默认值
	cujd.AllocationDelta = 0;//使用默认值
	int 
		status = DeviceIoControl(handl,	//句柄
		FSCTL_CREATE_USN_JOURNAL,	//打开日志
		&cujd,						//cujd数据
		sizeof(cujd),				//数据大小
		NULL,						//输出缓冲区大小0
		0,							//
		&br,						//指向接收缓冲区指针
		NULL);						//

	if (0 != status)
		cout << "初始化UNSN日志成功" << endl;
	else
	{
		cout << "初始化UNSN日志失败" << endl;
	//	return 0;
	}
	
	//4.获取日志操作
	USN_JOURNAL_DATA UsnInfo;
	status = DeviceIoControl(handl,
		FSCTL_QUERY_USN_JOURNAL,
		NULL, 0,
		&UsnInfo,
		sizeof(UsnInfo),
		&br,
		NULL);
	if (0 != status)
		cout << "获取日志成功" << endl;
	else
	{
		cout << "获取USN日志基本信息失败 —— status:" << status << "   error:" << GetLastError() << endl;
	//	return 0;
	}
	
	//5.列出usn文件
	MFT_ENUM_DATA med;
	med.StartFileReferenceNumber = 0;
	med.LowUsn = 0;//UsnInfo的first
	med.HighUsn = UsnInfo.NextUsn;//的NEXT
	char buffer[BUF_LEN];//存储记录缓冲
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
			//打印获取到的信息
			//int strLen = UsnRecord->FileNameLength;
			WideCharToMultiByte(CP_OEMCP,//当前系统OEM代码页，一种原始设备制造商硬件扫描码
				0,//一般为0，运行效率更快
				UsnRecord->FileName,//代转换的宽字符串
				-1,//代转换字符串长度
				filesName,//接收转换后输出新串的缓冲区
				iSize,//输出缓冲区大小
				NULL,//系统默认字符
				NULL);//开关变量的指针
			printf("FileName:%s\n", filesName);
			//获取文件路径信息
			printf("FileReferenceNumber:%xI64\n", UsnRecord->FileReferenceNumber);
			printf("ParentFileReferenceNumber:%xI64\n", UsnRecord->ParentFileReferenceNumber);
			printf("\n");
			char* pathBuffer[BUF_LEN];

			//获取下一个记录
			DWORD recordLen = UsnRecord->RecordLength;
			dwRetBytes -= recordLen;
			UsnRecord = (PUSN_RECORD)((PCHAR)UsnRecord + recordLen);
		}
		med.StartFileReferenceNumber = *(USN*)&buffer;
	}
	///** * step 06. 删除 USN 日志文件 ( 当然也可以不删除 ) */
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
	//	printf(" 成功删除 USN 日志文件 !\n");
	//}
	//else
	//{
	//	printf(" 删除 USN 日志文件失败 —— status:%x error:%d\n", status, GetLastError());
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
////把测试值插入map
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
////把路径组合起来
//static string BuildPath(string currentNode, string parentNode)
//{
//	return parentNode + "\\" + currentNode;
//}
//
//
//int main()
//{
//	//先插入磁盘"D:" 当前为9，无父目录，路径"D:"
//	string drivefrn = "9";
//	FrnFilePath* ff = new FrnFilePath("D:", drivefrn, "", "D:");
//	fileHash.insert(make_pair(drivefrn, *ff));
//
//	//插入a,b,c,d,e,f测试数据
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
//	//把a,b,c,d,e,f都插入map中
//	for_each(result.begin(), result.end(), fun);
//
//	//需要一个栈来存储暂时用不到的子目录
//	stack<string> treestack;
//
//	auto iter = fileHash.begin();
//	//把map都遍历一遍
//	while (iter != fileHash.end())
//	{
//		//每次开始都清空栈以防万一
//		clear(treestack);
//		//当前位置
//		FrnFilePath current = iter->second;
//		//如果当前位置路径为空，且有父目录就进去
//		if (current.path.empty() && !current.pfrn.empty())
//		{
//			//取到父目录信息
//			FrnFilePath parent = fileHash[current.pfrn];
//			//如果父目录路径为空，且父目录还有父目录
//			while (parent.path.empty() && !parent.pfrn.empty())
//			{
//				//定义零时变量，把当前目录变成父目录
//				FrnFilePath temp = current;
//				current = parent;
//				//如果当前目录路径为空，且有目录
//				if (current.path.empty() && !current.pfrn.empty())
//				{
//					//把第存储的子目录插入栈中，然后把父目录变成改变过的当前目录的父目录
//					treestack.push(temp.frn);
//					parent = fileHash[current.pfrn];
//				}
//			}
//			//当前目录路径名
//			current.path = BuildPath(current.filename, parent.path);
//			//如果栈不为空就一直继续
//			while (!treestack.empty())
//			{
//				//取头去头
//				string head = treestack.top();
//				treestack.pop();
//				FrnFilePath headNode = fileHash[head];
//				//拼接目录
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


	//下面都是垃圾代码，网上代码不可信 不如自己写一遍
	// 
	// 
	//遍历map容器
	//while (iter != fileHash.end())
	//{
	//	clear(treestack);
	//	//拿到值
	//	FrnFilePath current = iter->second;
	//	FrnFilePath* parent=NULL ;
	//	//如果当前路径为空，父目录不为空并且存在父目录
	//	if ((current.path.empty()) && !current.pfrn.empty() && fileHash.find(current.pfrn) != fileHash.end())
	//	{
	//		//当前目录的父目录为
	//		FrnFilePath parentVal = fileHash[current.pfrn];

	//		//如果父目录还有父目录，就将父目录变成当前目录，继续找有没有父目录
	//		while (current.path.empty() && !current.pfrn.empty() && fileHash.find(current.pfrn) != fileHash.end())
	//		{
	//			FrnFilePath temp = current;
	//			current = parentVal;

	//			//如果当前目录父目录不为空。并且存在父目录，就将第一次查找的当前目录存入栈中
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
	//			//当前目录为 父目录+当前
	//			current.path = BuildPath(current, parentVal);
	//			//栈不为空代表还有子目录
	//			while (!treestack.empty())
	//			{
	//				//存下最后插入的目录 ，即为当前目录的子目录
	//				string walkedkey = treestack.top();
	//				//拿完然后删除，准备取下个子目录
	//				treestack.pop();
	//				//拿到栈上最后插入的数据
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



