#include <vector>
#include <map>
#include <string>
#include "File.h"
#include "FlashInfo.h"
using namespace std;
#pragma once

struct FilePair
{
	unsigned int File;
	unsigned int Index;

	bool operator==(const FilePair Right);
};

auto FilePairCompair = [](unsigned int FileName, unsigned int IndexFileCluster) 
{
	FilePair fp; 
	fp.File = FileName;
	fp.Index = IndexFileCluster; 
	return fp;
};

class Steganography
{
private:
	
public:

	File **files;
	int NumberOfCoverFiles = 0;
	int Modify = 0;
	vector<unsigned int> *SteganoBloks = new vector<unsigned int>;
	map<unsigned int, FilePair> *OldTotalClusterChain;
	map<unsigned int, FilePair> *NewTotalClusterChain;


	bool SelfLoop(map<unsigned int, FilePair>::iterator First, map<unsigned int, FilePair>::iterator Second);
	vector<unsigned int> *MessageToSteganoBlocks(string Message);
	map<unsigned int, FilePair> *CompleteTotalClusterChain(File  *FileArray[]);
	map<unsigned int, FilePair> *MixTotalClusterChainForMessage();
	map<unsigned int, FilePair> *ModifyMixTotalClusterChainForMessage();
	int DataRewrite();
	int ReadCluster(unsigned int NumCluster, char* Buf);
	int WriteCluster(unsigned int NumCluster, char* Buf);
	int SetNewClusterChain(File *FileArray[]);
	int ReloadFileInfo(File *FileArray[]);
	
	
	
	int Hiding(string Message, File *FileArray[]);
	string Extracting(File *FileArray[]);

};


