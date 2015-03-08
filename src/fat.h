/** @file fat.h
 *  @brief functions related to fat16.
 *
 *  no directory supproted now. Need improvement.
 */

#ifndef _FAT_H_
#define _FAT_H_

#include "asmhead.h"

/** @addtogroup fat
 *  @brief this module in charge of the directory & file. the OS doesn't support directory now.
 *  @{
 */

#define FAT_FILE_MAX	256

/// infomation related to one file in fat.
typedef struct FILEINFO{
	uint8 name[8], ext[3], type;
	uint8 reserve[10];
	uint16 time, date, clustno;
	uint32 size;
}FILEINFO;

/// internal expression of one file. 
typedef struct FILE_NODE {
	uint8 name[9];
	uint8 ext[4];
	uint32 fileSize;
	uint8* buf;
	FILEINFO* fileInfo;
}FILE_NODE;

/// global FAT table structure. OS use this to show directory and get files. 
typedef struct FAT_CTL {
	uint32 fileCnt;
	int32* fatBase;
	int32	 fatSize;
	FILE_NODE fileNode[FAT_FILE_MAX];
	FILE_NODE* fontFile;
}FAT_CTL;

boolean fat_init();
FILE_NODE* fat_searchFile1(uint8* fileName);
FILE_NODE* fat_searchFile(uint8* fileName, uint8* ext);
boolean fat_readFile(FILE_NODE* file);


void fat_decodeFat(int32* fatBase, int32 fatSize, uint8* img);
void fat_loadFile(uint16 clustno, uint32 size, int8* buf, int32* fatBase, uint8* img);

/// @}

#endif
