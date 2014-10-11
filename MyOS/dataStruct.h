
#ifndef _DATA_STRUCT_H_
#define _DATA_STRUCT_H_

#include "typedef.h"

// ***** Loop Array ***** //
#define LOOP_ARRAY_SIZE 128
typedef struct LoopArray {
	uint8 data[LOOP_ARRAY_SIZE + 1];	//用一个位置来标记满或者空
	uint8 start;		//开始位置
	uint8 end;			//结束位置
}LoopArray;
void		InitLoopArray(LoopArray* array);
boolean	IsLoopArrayEmpty(LoopArray* array);
boolean	IsLoopArrayFull(LoopArray* array);
boolean	PutIntoLoopArray(LoopArray* array, uint8 data);
boolean	GetFromLoopArray(LoopArray* array, uint8* data);
uint8 	GetLoopArrayDataCnt(LoopArray* array);
// ***** Loop Array ***** //




#endif

