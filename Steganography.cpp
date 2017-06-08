#include "Steganography.h"


bool FilePair::operator==(const FilePair Right)
{
	if (this->File == Right.File)
		if (this->Index == Right.Index)
			return true;
	return false;
}


bool Steganography::SelfLoop(map<unsigned int, FilePair>::iterator First, map<unsigned int, FilePair>::iterator Second)
{
	if (First->first == Second->first)
		if(First->second == Second->second)
			return true;
	return false;
}
vector<unsigned int> *Steganography::MessageToSteganoBlocks(string Message)
{
	int LogFromNumerOfCoverFiles = ((int)log2(this->NumberOfCoverFiles)+this->Modify);
	int SizeMessage = Message.size();
	int SizeSteganoBloks = ceil(SizeMessage * 8 / (float)LogFromNumerOfCoverFiles);
	this->SteganoBloks->resize(SizeSteganoBloks);

	for (unsigned _int32 IndexBit = 0, IndexSteg = 0; IndexBit < (SizeMessage * 8); IndexBit++)
	{
		if ((IndexBit%LogFromNumerOfCoverFiles) == 0 && IndexBit != 0)
			IndexSteg++;
		this->SteganoBloks->at(IndexSteg) = (this->SteganoBloks->at(IndexSteg) << 1) | ((Message[IndexBit / 8] >> (7 - (IndexBit % 8))) & 0x1);
	}
	return this->SteganoBloks;
}

map<unsigned int, FilePair> *Steganography::CompleteTotalClusterChain(File *FileArray[])
{
	int size = 0;
	for (int i = 0; i < NumberOfCoverFiles; i++)
		size += FileArray[i]->ClusterChain->size();


	auto FilePairCompair = [](unsigned int i, unsigned int j) {FilePair fp; fp.File = i; fp.Index = j; return fp; };
	map<unsigned int, FilePair> *Chain = new map<unsigned int, FilePair>;
	for (unsigned int i = 0; i < NumberOfCoverFiles; i++)
	{

		for (unsigned int j = 0; j < FileArray[i]->ClusterChain->size(); j++)
		{
			unsigned int tmp = FileArray[i]->ClusterChain->at(j);
			Chain->insert(make_pair(FileArray[i]->ClusterChain->at(j), FilePairCompair(i,j)));
		}
	}
	this->OldTotalClusterChain = Chain;
	return Chain;
}

map<unsigned int, FilePair> *Steganography::MixTotalClusterChainForMessage()
{
	map<unsigned int, FilePair> *Chain = new map<unsigned int, FilePair>;
	int *lesssize = new int[this->NumberOfCoverFiles];
	int *size = new int[this->NumberOfCoverFiles];
	for (int i = 0; i < this->NumberOfCoverFiles; i++)
	{
		size[i] = 0;
		lesssize[i] = 0;
	}


	for (auto it = this->OldTotalClusterChain->begin(); it != this->OldTotalClusterChain->end();it++)
	{
		size[it->second.File]++;
		lesssize[it->second.File]++;
	}



	auto iterrChain = this->OldTotalClusterChain->begin();
	int fullcounter = 0;
	for (int i=0; iterrChain != this->OldTotalClusterChain->end(); ++iterrChain,++i)
	{
		if (i < this->SteganoBloks->size())
		{
			if (!lesssize[this->SteganoBloks->at(i)])
			{
				return NULL;
				delete[] lesssize;
				delete[] size;

			}
			Chain->insert(make_pair(iterrChain->first,
				FilePairCompair(this->SteganoBloks->at(i), size[this->SteganoBloks->at(i)] - lesssize[this->SteganoBloks->at(i)])));
			lesssize[this->SteganoBloks->at(i)]--;
			continue;
		}
		else
		{
			if (!lesssize[fullcounter])
				fullcounter++;
			Chain->insert(make_pair(iterrChain->first, FilePairCompair( fullcounter, size[fullcounter]-lesssize[fullcounter])));
			lesssize[fullcounter]--;
		}
	}
	this->NewTotalClusterChain = Chain;

	delete[] lesssize;
	delete[] size;
	return Chain;
}


map<unsigned int, FilePair> *Steganography::ModifyMixTotalClusterChainForMessage()
{
	map<unsigned int, FilePair> *Chain = new map<unsigned int, FilePair>;


	struct FileInform
	{
		int Hind = 0;
		int Tind = 0;
		int ChainSize;


		int LessH = 0;
		int LessT = 0;

		bool Firstind = 1;

	}*FileInf = new FileInform[this->NumberOfCoverFiles];
	for (int i = 0; i < this->NumberOfCoverFiles; i++)
	{
		FileInf[i].Tind = FileInf[i].ChainSize = this->files[i]->ClusterChain->size();
		FileInf[i].Tind--;
		FileInf[i].Hind = 0;
	}


	int SteganoBloksIndex = 0;
	int Padding_FileName = 0;
	for (auto iterrChain = this->OldTotalClusterChain->begin(); iterrChain != this->OldTotalClusterChain->end(); ++iterrChain)
	{
		if (SteganoBloksIndex < this->SteganoBloks->size())
		{
			if (!(FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].LessH || FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].LessT))
			{
				int BitSeries = this->SteganoBloks->at(SteganoBloksIndex) & 0x1;

				if (BitSeries)
					FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].LessT++;
				else
					FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].LessH++;

				for (int i = SteganoBloksIndex + 1; i < this->SteganoBloks->size(); i++)
				{
					if ((this->SteganoBloks->at(SteganoBloksIndex) >> 1) == (this->SteganoBloks->at(i) >> 1))
					{
						if ((this->SteganoBloks->at(i) & 0x1) == BitSeries)
						{
							if (BitSeries)
								FileInf[this->SteganoBloks->at(i) >> 1].LessT++;
							else
								FileInf[this->SteganoBloks->at(i) >> 1].LessH++;
						}
						else
						{
							if (BitSeries)
							{
								FileInf[this->SteganoBloks->at(i) >> 1].LessH++;
								FileInf[this->SteganoBloks->at(i) >> 1].LessT--;
							}
							else
							{
								FileInf[this->SteganoBloks->at(i) >> 1].LessH--;
								FileInf[this->SteganoBloks->at(i) >> 1].LessT++;
							}
							break;
						}
					}
				}
			}

			int CurrentInd = 0;
			if (FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].Firstind)
			{
				if (this->SteganoBloks->at(SteganoBloksIndex) & 0x1)
					CurrentInd = FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].Tind--;
				else CurrentInd = FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].Hind++;

				Chain->insert(make_pair(iterrChain->first, FilePairCompair(this->SteganoBloks->at(SteganoBloksIndex) >> 1, CurrentInd)));
				FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].Firstind = false;
			}
			else
			{
				if (this->SteganoBloks->at(SteganoBloksIndex) & 0x1)
				{
					if (FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].LessT)
					{
						CurrentInd = FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].Tind--;
						FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].LessT--;
					}
					else
					{
						CurrentInd = FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].Hind++;
						FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].LessH--;
					}

				}
				else
				{
					if (FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].LessH)
					{
						CurrentInd = FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].Hind++;
						FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].LessH--;
					}
					else
					{
						CurrentInd = FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].Tind--;
						FileInf[this->SteganoBloks->at(SteganoBloksIndex) >> 1].LessT--;
					}
				}


				Chain->insert(make_pair(iterrChain->first, FilePairCompair(this->SteganoBloks->at(SteganoBloksIndex) >> 1, CurrentInd)));
				SteganoBloksIndex++;
			}
		}
		else
		{
			Chain->insert(make_pair(iterrChain->first, FilePairCompair(Padding_FileName, FileInf[Padding_FileName].Hind++)));
			if (FileInf[Padding_FileName].Hind > FileInf[Padding_FileName].Tind)
				Padding_FileName++;
		}
	}

	delete[]FileInf;
	this->NewTotalClusterChain = Chain;
	return Chain;
}
int Steganography::DataRewrite()
{
	//Is Correct input?
	if (this->OldTotalClusterChain->size() != this->NewTotalClusterChain->size())
		return -1;

	//Create deque
	int ClusterSize = FileSystemInformation->bytes_per_sector * FileSystemInformation->sector_per_cluster;
	char *Buf_0 = new char[ClusterSize];
	char *Buf_1 = new char[ClusterSize];
	char *Buf_tmp = 0;

	//Function Director
	_int8 *Director = new _int8[this->OldTotalClusterChain->size()];
	for (int i = 0; i < this->OldTotalClusterChain->size(); i++)
		Director[i] = 0;


	//Main FuncLoop
	for (;;)
	{
		//Go from begin
		auto OldItter = this->OldTotalClusterChain->begin();
		auto NewItter = this->NewTotalClusterChain->begin();
		for (int i = 0;;)
		{

			if (OldItter == this->OldTotalClusterChain->end() || NewItter == this->NewTotalClusterChain->end())
				break;
			
			// is SelfLoop?
			if (this->SelfLoop(OldItter, NewItter))
				Director[i] = 1;




			int debug = 0;
			int _One = 0;
			int Zero = 0;
			int One = 0;
			if (debug)
			{

				for (int i = 0; i < this->OldTotalClusterChain->size(); i++)
				{
					switch (Director[i])
					{
					case -1: _One++; break;
					case 0: Zero++; break;
					case 1: One++; break;
					default:
						break;
					}
				}
			}


			//States Director
			switch (Director[i])
			{
			case 0:
				this->ReadCluster(NewItter->first, Buf_1);
				Director[i] = -1;
			case -1:
				if (OldItter->first != NewItter->first)
				{
					this->WriteCluster(NewItter->first, Buf_0);
					OldItter = this->OldTotalClusterChain->find(NewItter->first);
					Director[i] = 1;
				}
				break;
			case 1:
				if (OldItter->first != NewItter->first)
					OldItter = this->OldTotalClusterChain->find(NewItter->first);
				OldItter++;
				NewItter++;
				i++;
				continue;
			}

			//Jump to the next position
			i = 0;
			for (auto FinderItter = this->NewTotalClusterChain->begin(); FinderItter != this->NewTotalClusterChain->end(); FinderItter++)
			{
				if (FinderItter->second.File == OldItter->second.File)
				{
					if (FinderItter->second.Index == OldItter->second.Index)
					{
						NewItter = FinderItter;

						Buf_tmp = Buf_0;
						Buf_0 = Buf_1;
						Buf_1 = Buf_tmp;
						break;
					}
				}
				i++;
			}
		}

		//is Fin?
		bool fin = true;
		for (int i = 0; i < this->OldTotalClusterChain->size(); i++)
			if (Director[i] != 1)
				fin = false;

		if (fin)
		{
			return 0;
		}


	}
}

int Steganography::ReadCluster(unsigned int NumCluster, char* Buf)
{
	if (!Buf)
		return -1;

	unsigned int BytesPerCluster = FileSystemInformation->sector_per_cluster*FileSystemInformation->bytes_per_sector;
	unsigned _int32 OffsetSector = FileSystemInformation->num_reserv_sector + (FileSystemInformation->num_of_FAT * FileSystemInformation->num_of_sector_per_FAT_32);

	SetFilePointer(DeviceH, OffsetSector*FileSystemInformation->bytes_per_sector + (NumCluster - 2)*BytesPerCluster, NULL, FILE_BEGIN);
	ReadFile(DeviceH, Buf, BytesPerCluster, &Bytes_READ, NULL);
	return Bytes_READ;
}


int Steganography::WriteCluster(unsigned int NumCluster, char* Buf)
{
	if (!Buf)
		return -1;

	unsigned int BytesPerCluster = FileSystemInformation->sector_per_cluster*FileSystemInformation->bytes_per_sector;
	unsigned _int32 OffsetSector = FileSystemInformation->num_reserv_sector + (FileSystemInformation->num_of_FAT * FileSystemInformation->num_of_sector_per_FAT_32);

	Bytes_READ = 0;
	SetFilePointer(DeviceH, OffsetSector*FileSystemInformation->bytes_per_sector + (NumCluster - 2)*BytesPerCluster, NULL, FILE_BEGIN);
	WriteFile(DeviceH, Buf, BytesPerCluster, &Bytes_READ, NULL);
	return Bytes_READ;

}



int Steganography::SetNewClusterChain(File *FileArray[])
{

	unsigned int* FATTableBuffer = new unsigned int[FileSystemInformation->bytes_per_sector / 4];
	
	for (auto ChainItertor = this->NewTotalClusterChain->begin(); ChainItertor != this->NewTotalClusterChain->end(); ChainItertor++)
		FileArray[ChainItertor->second.File]->ClusterChain->at(ChainItertor->second.Index) = ChainItertor->first;

	for (int NumberFile = 0; NumberFile < this->NumberOfCoverFiles; NumberFile++)
	{

		int fat_sector = (FileArray[NumberFile]->ClusterChain->at(0) / (FileSystemInformation->bytes_per_sector / 4));
		SetFilePointer(DeviceH, FileSystemInformation->bytes_per_sector * (FileSystemInformation->num_reserv_sector + fat_sector), NULL, FILE_BEGIN);
		ReadFile(DeviceH, FATTableBuffer, FileSystemInformation->bytes_per_sector, &Bytes_READ, NULL);
		for (unsigned _int32 NumberCluster = 0; NumberCluster < FileArray[NumberFile]->ClusterChain->size(); NumberCluster++)
		{
			if (FileArray[NumberFile]->ClusterChain->at(NumberCluster) / (FileSystemInformation->bytes_per_sector / 4) != fat_sector)
			{
				SetFilePointer(DeviceH, (-1)*FileSystemInformation->bytes_per_sector, NULL, FILE_CURRENT);
				WriteFile(DeviceH, FATTableBuffer, FileSystemInformation->bytes_per_sector, &Bytes_READ, NULL);

				fat_sector = (FileArray[NumberFile]->ClusterChain->at(NumberCluster) / (FileSystemInformation->bytes_per_sector / 4));

				SetFilePointer(DeviceH, FileSystemInformation->bytes_per_sector * (FileSystemInformation->num_reserv_sector + fat_sector), NULL, FILE_BEGIN);
				ReadFile(DeviceH, FATTableBuffer, FileSystemInformation->bytes_per_sector, &Bytes_READ, NULL);
			}
			if (NumberCluster + 1 == FileArray[NumberFile]->ClusterChain->size())
			{
				FATTableBuffer[FileArray[NumberFile]->ClusterChain->at(NumberCluster) % (FileSystemInformation->bytes_per_sector / 4)] = 0x0FFFFFFF;
				SetFilePointer(DeviceH, (-1)*FileSystemInformation->bytes_per_sector, NULL, FILE_CURRENT);
				WriteFile(DeviceH, FATTableBuffer, FileSystemInformation->bytes_per_sector, &Bytes_READ, NULL);
				break;
			}
			FATTableBuffer[FileArray[NumberFile]->ClusterChain->at(NumberCluster) % (FileSystemInformation->bytes_per_sector / 4)] = FileArray[NumberFile]->ClusterChain->at(NumberCluster + 1);
		}
	}
	delete[]FATTableBuffer;
	return 0;
}
int Steganography::ReloadFileInfo(File *FileArray[])
{
	
	string SectorBuffer;
	SectorBuffer.resize(FileSystemInformation->bytes_per_sector);

	for (int NumberFile = 0; NumberFile < this->NumberOfCoverFiles; NumberFile++)
	{
		SetFilePointer(DeviceH, FileArray[NumberFile]->OffsetSectorInFileInformation*FileSystemInformation->bytes_per_sector, NULL, FILE_BEGIN);
		ReadFile(DeviceH, (LPVOID)SectorBuffer.c_str(), FileSystemInformation->bytes_per_sector, &Bytes_READ, NULL);

		string SubString = string((const char*)FileArray[NumberFile]->FileInfo.name, 32);
		int OffsetFileInfo = SectorBuffer.find(SubString);

		FileArray[NumberFile]->FileInfo.FstClusHI = (FileArray[NumberFile]->ClusterChain->at(0) >> 16) & 0xFFFF;
		FileArray[NumberFile]->FileInfo.FstClusLO = FileArray[NumberFile]->ClusterChain->at(0) & 0xFFFF;

		SubString = string((const char*)FileArray[NumberFile]->FileInfo.name, 32);

		SectorBuffer.erase(OffsetFileInfo, 32);
		SectorBuffer.insert(OffsetFileInfo, SubString);

		SetFilePointer(DeviceH, (-1)*FileSystemInformation->bytes_per_sector, NULL, FILE_CURRENT);
		WriteFile(DeviceH, (LPVOID)SectorBuffer.c_str(), FileSystemInformation->bytes_per_sector, &Bytes_READ, NULL);
	}
	return Bytes_READ;
}

int Steganography::Hiding(string Message, File *FileArray[])
{
	this->MessageToSteganoBlocks(Message);

	this->CompleteTotalClusterChain(FileArray);

	if(this->Modify)
		this->ModifyMixTotalClusterChainForMessage();
	else this->MixTotalClusterChainForMessage();
	
	this->DataRewrite();

	this->SetNewClusterChain(FileArray);
	this->ReloadFileInfo(FileArray);

	this->SteganoBloks->clear();
	this->OldTotalClusterChain->clear();
	this->NewTotalClusterChain->clear();

	return 0;
}

string Steganography::Extracting(File *FileArray[])
{
	this->CompleteTotalClusterChain(FileArray);


	int SizeSteganoBloks = this->OldTotalClusterChain->size();
	this->SteganoBloks->resize(SizeSteganoBloks);

	

	if (this->Modify)
	{
		int *IsFirst = new int[this->NumberOfCoverFiles];
		int *PreviousClusterIndex = new int[this->NumberOfCoverFiles];

		int index = 0;
		for (auto Itter = this->OldTotalClusterChain->begin(); Itter != this->OldTotalClusterChain->end(); Itter++)
		{
			if (IsFirst[Itter->second.File])
			{
				PreviousClusterIndex[Itter->second.File] = Itter->second.Index;
				IsFirst[Itter->second.File] = false;
			}
			else
			{
				SteganoBloks->at(index) = (Itter->second.File << 0x1)|((Itter->second.Index > PreviousClusterIndex[Itter->second.File]) ? 0 : 1);
				PreviousClusterIndex[Itter->second.File] = Itter->second.Index;
				index++;
			}
		}

		delete[]IsFirst;
		delete[]PreviousClusterIndex;

		//this->SteganoBloks->resize(SizeSteganoBloks - this->NumberOfCoverFiles);
	}
	else
	{
		int index = 0;
		for (auto Itter = this->OldTotalClusterChain->begin(); Itter != this->OldTotalClusterChain->end(); Itter++)
		{
			SteganoBloks->at(index) = Itter->second.File;
			index++;
		}
	}


	int Mask = ((int)log2(this->NumberOfCoverFiles) + this->Modify);
	string Message;
	Message.resize((this->OldTotalClusterChain->size()*Mask) / 8);


	for (int i = 0; i < (this->OldTotalClusterChain->size()*Mask); i++)
		Message[i / 8] = (Message[i / 8] << 1) | ((SteganoBloks->at(i / Mask) >> ((Mask - 1) - (i%Mask))) & 0x1);


	this->SteganoBloks->clear();
	this->OldTotalClusterChain->clear();


	return Message;
}