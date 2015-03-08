/** @file fat.c
 *  @brief functions related to fat16.
 *
 *  no directory supproted now. Need improvement.
 */

#include "fat.h"

FAT_CTL fatCtl;		///< OS use this function to manage the root file directory.

/** @brief init fat root directory.
 *  this function get data from memory, extract useful information and store them in fatCtl.
 *  @param NULL
 *  @return whether the function executed successful.
 */
boolean fat_init()
{
	FILEINFO* finfo = (FILEINFO*)(ADR_DISKIMG + 0x002600);
	int32 i, endPos;

	fatCtl.fileCnt = 0;
	fatCtl.fatSize = 2880;
	if(mem_alloc(4 * 2880, &(fatCtl.fatBase)) == FALSE)
		return FALSE;
	fat_decodeFat(fatCtl.fatBase, 2880, ADR_DISKIMG + 0x000200);
	
	for(i = 0; i < 224; i++) {
		if(finfo[i].name[0] == 0x00) break;
		if(finfo[i].name[0] != 0xe5) {
			
			for(endPos = 7; finfo[i].name[endPos] == ' '&& endPos > 0; endPos--);
			strncpy(fatCtl.fileNode[i].name, finfo[i].name, endPos + 1);
			fatCtl.fileNode[fatCtl.fileCnt].name[endPos + 1] = 0;

			for(endPos = 2; finfo[i].ext[endPos] == ' '&& endPos > 0; endPos--);
			strncpy(fatCtl.fileNode[fatCtl.fileCnt].ext, finfo[i].ext, endPos + 1);
			fatCtl.fileNode[fatCtl.fileCnt].ext[endPos + 1] = 0;
			
			fatCtl.fileNode[fatCtl.fileCnt].fileInfo = &(finfo[i]);
			fatCtl.fileNode[fatCtl.fileCnt].fileSize = finfo[i].size;
			fatCtl.fileNode[fatCtl.fileCnt].buf = NULL;
			fatCtl.fileCnt++;
		}
	}
	fatCtl.fontFile = fat_searchFile1("HZK16.FNT");
	fat_readFile(fatCtl.fontFile);
	
	return TRUE;
}

/** @brief search for specific file name.
 *  @param fileName name of the file you want to search
 *  @return the file pointer of the file if founded. Else will return NULL.
 */
FILE_NODE* fat_searchFile1(uint8* fileName)
{
	int32 i;
	int8 name[9] = "", ext[4] = "";
	
	for(i = 0; fileName[i] != '.' && fileName[i] != 0; i++);
	if(fileName[i] == 0) {
		return NULL;
	}
	strcpy(name, fileName);
	for(i = 0; name[i] != '.'; i++);
	name[i] = 0;
	strcpy(ext, fileName + i + 1);
	
	return fat_searchFile(name, ext);
}

/** @brief search for specific file name.
 *  @param fileName name of the file you want to search
 *  @param ext suffix of the file
 *  @return the file pointer of the file if founded. Else will return NULL.
 */
FILE_NODE* fat_searchFile(uint8* fileName, uint8* ext)
{
	int32 i;
	for(i = 0; i < fatCtl.fileCnt; i++) {
		if((strcmp(fatCtl.fileNode[i].name, fileName) == 0) && (strcmp(fatCtl.fileNode[i].ext, ext) == 0)) {
			return &(fatCtl.fileNode[i]);
		}
	}
	return NULL;
}

/** @brief read one file throught its FILE_NODE.
 *  This function would read the data of the file into its internal buffer. 
 *  It calls the intercal function fat_loadFile() to copy data into its buffer.
 *  @param file the pointer of the FILE_NODE. 
 *  @return whether the function executed successful.
 */
boolean fat_readFile(FILE_NODE* file)
{
	int32 i;
	for(i = 0; i < fatCtl.fileCnt && file != &(fatCtl.fileNode[i]); i++);
	if(i >= fatCtl.fileCnt) {
		return FALSE;
	}
	if(file->buf != NULL) {
		return TRUE;
	}
	if(mem_alloc(file->fileSize, &(file->buf)) == FALSE) {
		return FALSE;
	}
	i = file->fileInfo->clustno;
	fat_loadFile(i, file->fileSize, file->buf, fatCtl.fatBase, ADR_DISKIMG + 0x003e00);
	return TRUE;
}

/** @brief decode FAT table.
 *  @param fatBase base address of the dst FAT table, extracted data are listed here.
 *  @param fatSize size of the FAT table
 *  @param img the src start address of the FAT table
 *  @return NULL
 */
void fat_decodeFat(int32* fatBase, int32 fatSize, uint8* img)
{
	int32 i, j = 0;
	for(i = 0; i < fatSize; i += 2) {
		fatBase[i + 0] = (img[j + 0] | (img[j + 1] << 8)) & 0xFFF;
		fatBase[i + 1] = ((img[j + 1] >> 4) | (img[j + 2] << 4)) & 0xFFF;
		j += 3;
	}
}

/** @brief load file with specific cluster number, size.
 *  @param clustno the cluster number used.
 *  @param size size you want to load.
 *  @param buf buffer to store the data
 *  @param fatBase base address of the dst FAT table.
 *  @param img start address of the original file block
 *  @return NULL
 */
void fat_loadFile(uint16 clustno, uint32 size, int8* buf, int32* fatBase, uint8* img)
{
	int i;
	while(1) {
		if(size <= 512) {
			for(i = 0; i < size; i++)
				buf[i] = img[clustno * 512 + i];
			break;
		}
		for(i = 0; i < 512; i++) {
			buf[i] = img[clustno * 512 + i];
		}
		size -= 512;
		buf += 512;
		clustno = fatBase[clustno];
	}
}
