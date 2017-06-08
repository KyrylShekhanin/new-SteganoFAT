#include "File.h"

File::File()
{
}


File::~File()
{
}

string File::GetSubFileName(string &FileName)
{

	int SleshCounter = FileName.find('\\', 0);
	string SubFileName = FileName.substr(0, SleshCounter);
	FileName = FileName.substr(SleshCounter + 1, strlen(FileName.c_str()));
	return SubFileName;
}
vector<unsigned int>* File::SetClusterChain(unsigned int NumberOfFirstCluster)
{
	if (!NumberOfFirstCluster)
		return NULL;
	unsigned int* FATTableBuffer = new unsigned int[FileSystemInformation->bytes_per_sector/4];
	this->ClusterChain = new vector<unsigned int>;

	for (;;)
	{
		int OffsetFATSector = (NumberOfFirstCluster / (FileSystemInformation->bytes_per_sector / 4));
		SetFilePointer(DeviceH, FileSystemInformation->bytes_per_sector * (FileSystemInformation->num_reserv_sector + OffsetFATSector), NULL, FILE_BEGIN);
		ReadFile(DeviceH, FATTableBuffer, FileSystemInformation->bytes_per_sector, &Bytes_READ, NULL);
		for (;;)
		{
			this->ClusterChain->insert(this->ClusterChain->end(),NumberOfFirstCluster);
			NumberOfFirstCluster = FATTableBuffer[NumberOfFirstCluster % (FileSystemInformation->bytes_per_sector / 4)];
			if (NumberOfFirstCluster == 0x0FFFFFFF)
			{
				delete[]FATTableBuffer;/*table*/
				return  this->ClusterChain;
			}
			else if (NumberOfFirstCluster / (FileSystemInformation->bytes_per_sector / 4) != (unsigned _int32)OffsetFATSector)
				break;

		}
	}


}


bool File::IsStandart(string str)
{
	int PrefixCounter = str.find_last_of('.', strlen(str.c_str()));
	int PosfixCounter = strlen(str.c_str())-PrefixCounter-1;

	if (PrefixCounter == -1)
	{
		PrefixCounter = PosfixCounter;
		PosfixCounter = 0;
	}

	if (PrefixCounter > 8)
		return false;
	if (PosfixCounter > 3)
		return false;
	for (int pref = 0; pref<PrefixCounter;pref++)
		switch (str[pref])
		{
		case ',': return false;
		case '[': return false;
		case ']': return false;
		case ';': return false;
		case '=': return false;
		case '+': return false;
		case ' ': return false;
		case '.': return false;
		default:
			break;
		}
	for (int post = 0; post<PosfixCounter; post++)
		switch (str[strlen(str.c_str())-post])
		{
		case ',': return false;
		case '[': return false;
		case ']': return false;
		case ';': return false;
		case '=': return false;
		case '+': return false;
		case ' ': return false;
		case '.': return false;
		default:
			break;
		}
	return true;
}
string File::PrefixConvert8dot3(string str, bool isStandart)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		str[i] = toupper(str[i]);
		switch (str[i])
		{
		case ',': str[i] = '_'; break;
		case '[': str[i] = '_'; break;
		case ']': str[i] = '_'; break;
		case ';': str[i] = '_'; break;
		case '=': str[i] = '_'; break;
		case '+': str[i] = '_'; break;
		case ' ': str.erase(i, 1); i--; continue;
		case '.': str.erase(i, 1); i--; continue;
		default:
			break;
		}
	}
	if (!isStandart)
	{
		if (strlen(str.c_str()) <= 6)
			str += "~1";
		else { str.insert(6, "~1"); str.erase(8); }
	}
	while(strlen(str.c_str())<8)
		str += " ";
	return str;
}
string File::PosfixConvert8dot3(string str)
{
	str = str.substr(0, 3);
	for (int i = 0; str[i] != '\0'; i++)
	{
		str[i] = toupper(str[i]);
		switch (str[i])
		{
		case ',': str[i] = '_'; break;
		case '[': str[i] = '_'; break;
		case ']': str[i] = '_'; break;
		case ';': str[i] = '_'; break;
		case '=': str[i] = '_'; break;
		case '+': str[i] = '_'; break;
		case ' ': str.erase(i, 1); i--; continue;
		case '.': str.erase(i, 1); i--; continue;
		default:
			break;
		}
	}
	while (strlen(str.c_str())<3)
		str += " ";
	return str;
}
string File::ConvertTo8dot3Standart(string FileName)
{
	int PointCounter = FileName.rfind('.', strlen(FileName.c_str()));

	string prefix = FileName.substr(0, PointCounter);
	string posfix = FileName.substr(PointCounter+1, strlen(FileName.c_str()));

	bool isStandart = IsStandart(FileName);

	prefix = PrefixConvert8dot3(prefix, isStandart);
	if (PointCounter == -1)
		posfix = "   ";
	else { posfix = PosfixConvert8dot3(posfix); }
	
	return prefix+posfix;

}

File* File::FindRootDirectory(FLASH_INFO FlashInformation)
{
	File* RootDirectory = new File;
	
	RootDirectory->OffsetSectorInFileData = FlashInformation.num_reserv_sector + (FlashInformation.num_of_FAT * FlashInformation.num_of_sector_per_FAT_32);
	RootDirectory->OffsetSectorInFAT = FlashInformation.num_reserv_sector;
		
	RootDirectory->SetClusterChain(0x02);
	return RootDirectory;
}




/**********************************************/
File* File::FindFile(string FileName, File *PreviousFile)
{
	if(!RootDirectory)
		PreviousFile = RootDirectory = FindRootDirectory(*FileSystemInformation);
	if (!PreviousFile)
		return (File*)NULL;
	if(!PreviousFile->ClusterChain)
		return PreviousFile;
	unsigned int BytesPerCluster = FileSystemInformation->sector_per_cluster*FileSystemInformation->bytes_per_sector;
	unsigned int OffsetToDataFAT = PreviousFile->OffsetSectorInFileData * FileSystemInformation->bytes_per_sector;
	unsigned int FirstCluster = PreviousFile->ClusterChain->at(0);

	File* file = new File;
	if (PreviousFile == RootDirectory)
		file->SetFileName(FileName);
	else file->SetFileName(PreviousFile->FileName);


	string SubName = File::GetSubFileName(FileName);
		SubName = file->ConvertTo8dot3Standart(SubName);

	string Buffer;
	Buffer.resize(BytesPerCluster);
	for (unsigned int i = 0; i<PreviousFile->ClusterChain->size(); i++)
	{ 
		SetFilePointer(DeviceH, OffsetToDataFAT + (PreviousFile->ClusterChain->at(i) - FirstCluster)*BytesPerCluster, NULL, FILE_BEGIN);
		ReadFile(DeviceH, (LPVOID) Buffer.c_str(), BytesPerCluster, &Bytes_READ, NULL);

		int OffsetFileInfo = Buffer.find(SubName, 0);
		if (OffsetFileInfo == -1)
			continue;
		else
		{
			Buffer.copy((char *)&file->FileInfo, 32, OffsetFileInfo);
			file->SetClusterChain(((unsigned _int32)(file->FileInfo.FstClusHI) << 16) | file->FileInfo.FstClusLO);
			file->OffsetSectorInFileInformation = PreviousFile->OffsetSectorInFileData + ((PreviousFile->ClusterChain->at(i) - FirstCluster)*FileSystemInformation->sector_per_cluster);
			if (file->ClusterChain)
			{
				file->OffsetSectorInFileData = PreviousFile->OffsetSectorInFileData + ((file->ClusterChain->at(0) - FirstCluster)*FileSystemInformation->sector_per_cluster);
				file->OffsetSectorInFAT = FileSystemInformation->num_reserv_sector + (file->ClusterChain->at(0) / (FileSystemInformation->bytes_per_sector / 4));
			}
			if (PreviousFile != RootDirectory)
			{
				PreviousFile = NULL; 
				delete PreviousFile;
			}
			return file = FindFile(FileName, file);
		}
	}
	delete file;
	return PreviousFile;
}
/**********************************************/
int File::SetFileName(string String)
{
	this->FileName = String;
	return strlen(String.c_str());
}
string File::GetFileName()
{
	return this->FileName;
}
