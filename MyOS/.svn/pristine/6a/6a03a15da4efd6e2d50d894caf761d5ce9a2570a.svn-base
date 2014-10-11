
#include "string.h"


int32 _strlen(char* str)
{
	int32 i = 0;
	while(str[i])i++;
	return i;
}

int32 _strcat(char* dst, char* src)
{
	int32 i = 0, j = 0;
	while(dst[i])i++;
	while(src[j]) {
		dst[i] = src[j];
		i++;
		j++;
	}
	return j;
}

int32 _strncat(char* dst, char* src, int32 num)
{
	int32 i = 0, j = 0;
	while(dst[i])i++;
	while(j < num && src[j]) {
		dst[i] = src[j];
		i++;
		j++;
	}
	return j;
}

int32 _strcpy(char* dst, char* src)
{
	int32 i = 0;
	while(src[i]) {
		dst[i] = src[i];
		i++;
	}
	return i;
}

int32 _strncpy(char* dst, char* src, int32 num)
{
	int32 i = 0;
	for(; i < num; i++) {
		dst[i] = src[i];
	}
	return i;
}

void _memset(void* ptr, int32 value, int32 num)
{
	while(num) {
		((int32*)ptr)[num++] = value;
	}
}
