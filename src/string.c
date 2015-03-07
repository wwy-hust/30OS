/** @file string.c
 *  @brief basic functions related to string.
 */
 
#include "string.h"

/** @brief strlen.
 *  @param str string you want to use
 *  @return length of string
 */
int32 _strlen(char* str)
{
	int32 i = 0;
	while(str[i])i++;
	return i;
}

/** @brief strcat.
 *  @param dst dst str string you want to use
 *  @param src src str string you want to use
 *  @return length of chars copyed
 */
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

/** @brief strncat.
 *  @param dst dst str string you want to use
 *  @param src src str string you want to use
 *  @param num number of char to cat
 *  @return length of chars copyed
 */
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


/** @brief strcpy.
 *  @param dst dst str string you want to use
 *  @param src src str string you want to use
 *  @return length of chars copyed
 */
int32 _strcpy(char* dst, char* src)
{
	int32 i = 0;
	while(src[i]) {
		dst[i] = src[i];
		i++;
	}
	return i;
}


/** @brief strncpy.
 *  @param dst dst str string you want to use
 *  @param src src str string you want to use
 *  @param num number of char to cat
 *  @return length of chars copyed
 */
int32 _strncpy(char* dst, char* src, int32 num)
{
	int32 i = 0;
	for(; i < num; i++) {
		dst[i] = src[i];
	}
	return i;
}


/** @brief memset.
 *  @param ptr start addr to set
 *  @param value value to put
 *  @param num number of chars to put
 *  @return NULL
 */
void _memset(void* ptr, int32 value, int32 num)
{
	while(num) {
		((int32*)ptr)[num++] = value;
	}
}
