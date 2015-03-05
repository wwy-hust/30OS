/** @file dataStruct.c
 *  @brief basic data structure.
 *
 *  This files contains basic data structure utils.
 *  now just include LoopArray. will extend in future.
 */
 
#include "dataStruct.h"

/** @brief init one loop array.
 *  @param array the loop array structure you want to init.
 *  @return NULL
 */
void InitLoopArray(LoopArray* array)
{
	array->start = 0;
	array->end = 0;
}

/** @brief test whether a loop array is empty
 *  @param array the loop array structure you want to use.
 *  @return empty or not.
 */
boolean IsLoopArrayEmpty(LoopArray* array)
{
	if(array->start == array->end) {
		return TRUE;
	}
	return FALSE;
}

/** @brief test whether a loop array is full
 *  @param array the loop array structure you want to use.
 *  @return full or not.
 */
boolean IsLoopArrayFull(LoopArray* array)
{
	if((array->end + 1) % (LOOP_ARRAY_SIZE + 1) == array->start) {
		return TRUE;
	}
	return FALSE;
}

/** @brief put one byte to loop array
 *  @param array the loop array structure you want to use.
 *  @param data the data to put
 *  @return whether put successful
 */
boolean PutIntoLoopArray(LoopArray* array, uint8 data)
{
	if(FALSE == IsLoopArrayFull(array)) {
		array->data[array->end] = data;
		array->end = (array->end + 1) % (LOOP_ARRAY_SIZE + 1);
		return TRUE;
	}
	return FALSE;
}

/** @brief get one byte to loop array
 *  @param array the loop array structure you want to use.
 *  @param data the pointer of data get from the loop array.
 *  @return whether get successful
 */
boolean GetFromLoopArray(LoopArray* array, uint8* data)
{
	if(FALSE == IsLoopArrayEmpty(array)) {
		*data = array->data[array->start];
		array->start = (array->start + 1) % (LOOP_ARRAY_SIZE + 1);
		return TRUE;
	}
	return FALSE;
}

/** @brief get data cnt in loop array
 *  @param array the loop array structure you want to use.
 *  @return the number of existed data
 */
uint8 GetLoopArrayDataCnt(LoopArray* array)
{
	if(array->start == array->end) {
		return 0;
	}
	else if(array->start == (array->end + 1) % (LOOP_ARRAY_SIZE + 1)) {
		return LOOP_ARRAY_SIZE;
	}
	else if(array->start < array->end) {
		return (array->end - array->start);
	}
	return (array->end - array->start + LOOP_ARRAY_SIZE + 1);
}

