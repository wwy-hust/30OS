/** @file dataStruct.h
 *  @brief basic data structure.
 *
 *  This files contains basic data structure utils.
 *  now just include LoopArray. will extend in future.
 */
 
#ifndef _DATA_STRUCT_H_
#define _DATA_STRUCT_H_

#include "typedef.h"

// ***** Loop Array ***** //
#define LOOP_ARRAY_SIZE 128

/** @brief loop array structure */
typedef struct LoopArray {
	uint8 data[LOOP_ARRAY_SIZE + 1];	///< used to store data, +1 used to distinguish empty from full
	uint8 start;		///< start pos
	uint8 end;			///< end pos
}LoopArray;

void		InitLoopArray(LoopArray* array);
boolean	IsLoopArrayEmpty(LoopArray* array);
boolean	IsLoopArrayFull(LoopArray* array);
boolean	PutIntoLoopArray(LoopArray* array, uint8 data);
boolean	GetFromLoopArray(LoopArray* array, uint8* data);
uint8 	GetLoopArrayDataCnt(LoopArray* array);
// ***** Loop Array ***** //




#endif

