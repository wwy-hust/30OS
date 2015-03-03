
#ifndef _STRING_H_
#define _STRING_H_

#include "typedef.h"

int32 _strlen(char* str);
int32 _strcat(char* dst, char* src);
int32 _strncat(char* dst, char* src, int32 num);
int32 _strcpy(char* dst, char* src);
int32 _strncpy(char* dst, char* src, int32 num);
void _memset(void* ptr, int32 value, int32 num);

#endif
