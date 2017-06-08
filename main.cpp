#include <iostream>
#include <Windows.h>
#include <ctime> /*add to text */
#include <conio.h>
#include <math.h>

#include "FlashInfo.h"
#include "File.h"
#include "Steganography.h"

#pragma warning (disable:4996)
using namespace std;

#define DEVICE_NAME 'G'

HANDLE OpenDisk(char ch);


int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
	)
{
	if ((DeviceH = OpenDisk(DEVICE_NAME))== HANDLE(0xFFFFFFFF))
		return 0x01;

	FlashInfo::ReadFlasfInfo(DeviceH);
	Steganography stg;
	

	
	stg.NumberOfCoverFiles = 1;
	stg.Modify = 1;
	stg.files = new File*[stg.NumberOfCoverFiles];

	string str;

	/*FILE *f = fopen("A:\\Test\\test.bmp", "rb");
	int sizefile = 0;
	while (!feof(f))
	{
		char ch = fgetc(f);
		sizefile++;
	}
	str.resize(sizefile);
	fseek(f, 0, SEEK_SET);
	fread(&str[0], sizefile, 1, f);*/

	//stg.files[0] = File::FindFile("2.txt", RootDirectory);
	//stg.files[1] = File::FindFile("1.bmp", RootDirectory);

	for (int i = 0; i < 100; i++)
	{
		stg.files[0] = File::FindFile("1.txt", RootDirectory);
		//stg.files[1] = File::FindFile("2.bmp", RootDirectory);
		//stg.files[2] = File::FindFile("3.txt", RootDirectory);
		//stg.files[3] = File::FindFile("4.txt", RootDirectory);
		//stg.files[4] = File::FindFile("5.txt", RootDirectory);
		//stg.files[5] = File::FindFile("6.txt", RootDirectory);
		//stg.files[6] = File::FindFile("7.txt", RootDirectory);
		//stg.files[7] = File::FindFile("8.txt", RootDirectory);
		//stg.Modify = (i & 0x1);

		//stg.Hiding("G", stg.files);
		str = stg.Extracting(stg.files);
		/*FILE *ff= fopen("A:\\test.bmp", "wb");
		fwrite(&str[0], str.size(), 1, ff);*/
	}
//	stg.MessageToSteganoBlocks("qw");

//	stg.CompleteTotalClusterChain(stg.files);
//	stg.ModifyMixTotalClusterChainForMessage();

//	stg.Hiding("qw", stg.files);

//	stg.Extracting(stg.files);
	return 0;
}

HANDLE OpenDisk(char ch)
{
	char device_name[20];
	sprintf(device_name, "\\\\.\\%c:", ch);
	return CreateFile(device_name,									//имя
		GENERIC_WRITE| GENERIC_READ,
		FILE_SHARE_READ,											//дуплекс\симплекс
		NULL,														//дескриптор защиты
		OPEN_EXISTING,												//действие в случае ошибки
		FILE_ATTRIBUTE_NORMAL,										//флаг атр. файла (для девайса - NULL))
		NULL);														//шаблон
}