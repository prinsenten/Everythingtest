#include<iostream>
#include<string>
using namespace std;

class FrnFilePath 
{
public:
	string filename;
	string frn;
	string pfrn;
	string path;
	FrnFilePath(string filename, string frn, string pfrn, string path)
	{
		this->filename = filename;
		this->frn = frn;
		this->path = path;
		this->pfrn = pfrn;
	}
	FrnFilePath(){}
};