
#include "dataStruct.h"

// ¡Á¡Á¡Á¡Á¡Á¡Á Loop Array ¡Á¡Á¡Á¡Á¡Á¡Á
void InitLoopArray(LoopArray* array)
{
	array->start = 0;
	array->end = 0;
}

boolean IsLoopArrayEmpty(LoopArray* array)
{
	if(array->start == array->end) {
		return TRUE;
	}
	return FALSE;
}

boolean IsLoopArrayFull(LoopArray* array)
{
	if((array->end + 1) % (LOOP_ARRAY_SIZE + 1) == array->start) {
		return TRUE;
	}
	return FALSE;
}

boolean PutIntoLoopArray(LoopArray* array, uint8 data)
{
	if(FALSE == IsLoopArrayFull(array)) {
		array->data[array->end] = data;
		array->end = (array->end + 1) % (LOOP_ARRAY_SIZE + 1);
		return TRUE;
	}
	return FALSE;
}

boolean GetFromLoopArray(LoopArray* array, uint8* data)
{
	if(FALSE == IsLoopArrayEmpty(array)) {
		*data = array->data[array->start];
		array->start = (array->start + 1) % (LOOP_ARRAY_SIZE + 1);
		return TRUE;
	}
	return FALSE;
}

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

// ¡Á¡Á¡Á¡Á¡Á¡Á Loop Array ¡Á¡Á¡Á¡Á¡Á¡Á
