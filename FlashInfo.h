#include <Windows.h>
#pragma once
#pragma pack (1)

HANDLE DeviceH;
DWORD Bytes_READ; //Need for ReadFile


struct FLASH_INFO
{
	unsigned char start[3];
	unsigned char dos_vers[8];
	unsigned _int16 bytes_per_sector;
	unsigned _int8 sector_per_cluster;
	unsigned _int16 num_reserv_sector;
	unsigned _int8 num_of_FAT;
	unsigned _int16 num_of_dir_entries;
	unsigned _int16 LOW_total_sector;
	unsigned _int8 media_descr_type;
	unsigned _int16 num_of_sector_per_FAT_12_16;
	unsigned _int16 num_of_sector_per_track;
	unsigned _int16 num_of_heads;
	unsigned _int32 num_of_hidden_sector;
	unsigned _int32 LARGE_total_sector;

	unsigned _int32 num_of_sector_per_FAT_32;
	unsigned _int16 flags;
	unsigned _int16 FAT_version;
	unsigned _int32 cluster_num_of_root_dir;
	unsigned _int16 sector_num_FSInfo;
	unsigned _int16 sector_num_backup_boot;
	unsigned char reserved[12];
	unsigned _int8 drive_num;
	unsigned _int8 winNT_flags;
	unsigned _int8 signature;
	unsigned _int32 serial_number;
	unsigned char volum_label_string[11];
	unsigned char system_id_string[8];
	unsigned char boot_code[420];
	unsigned _int16 AA55;
}*FileSystemInformation;

class FlashInfo
{
	
public:
	FlashInfo();
	~FlashInfo();
	static FLASH_INFO * ReadFlasfInfo(HANDLE flash);


	
};

