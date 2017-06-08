#include "FlashInfo.h"




FlashInfo::FlashInfo()
{

}


FlashInfo::~FlashInfo()
{
}

FLASH_INFO * FlashInfo::ReadFlasfInfo(HANDLE flash)
{
	FileSystemInformation = new FLASH_INFO;
	SetFilePointer(flash, 0, NULL, FILE_BEGIN);
	ReadFile(flash, FileSystemInformation, 512, &Bytes_READ, NULL);
	SetFilePointer(flash, 0, NULL, FILE_BEGIN);

	return FileSystemInformation;
}

