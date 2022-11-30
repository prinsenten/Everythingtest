#include<string>
using namespace std;

class fileInfo
{
public:
	string filename;
	string frn;
	string pfrn;
	fileInfo(string filename, string frn, string pfrn)
	{
		this->filename = filename;
		this->frn = frn;
		this->pfrn = pfrn;
	}
};