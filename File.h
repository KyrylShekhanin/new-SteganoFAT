#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <Windows.h>
#include "FlashInfo.h"


using namespace std;
#pragma once
#pragma pack (1)
#pragma warning(disable:4996)  
class File
{
private:
	
	string FileName;




	
	static string GetSubFileName(string &FileName);
	vector<unsigned int>* SetClusterChain(unsigned int NumberOfFirstCluster);

	bool IsStandart(string str);
	string PrefixConvert8dot3(string str, bool isStandart);
	string PosfixConvert8dot3(string str);
	string ConvertTo8dot3Standart(string FileName);

	static File* FindRootDirectory(FLASH_INFO FlashInformation);

public:
	struct FILE_INFO
	{
		unsigned char name[8];
		unsigned char type[3];
		unsigned _int8 attr;
		unsigned _int8 WinNT;
		unsigned _int8 CrtTimeTenth;
		unsigned _int16 CrtTime;
		unsigned _int16 CrtDate;
		unsigned _int16 LstAccDate;
		unsigned _int16 FstClusHI;
		unsigned _int16 WrtTime;
		unsigned _int16 WrtDate;
		unsigned _int16 FstClusLO;
		unsigned _int32 FileSize;
	}FileInfo;

	unsigned _int32 OffsetSectorInFAT;
	unsigned _int32 OffsetSectorInFileInformation;
	unsigned _int32 OffsetSectorInFileData;

	vector<unsigned int>* ClusterChain;

	File();
	~File();
	
	static File* FindFile(string FileName, File *PreviousFile);
	int SetFileName(string String);
	string GetFileName();

}
*RootDirectory;

