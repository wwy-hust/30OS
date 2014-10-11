
#ifndef _TYPE_DEF_H_
#define _TYPE_DEF_H_

#define IN
#define OUT
#define INOUT

#ifndef NULL
	#define NULL	((void *)0)
#endif

typedef unsigned char		uint8;
typedef unsigned short	uint16;
typedef unsigned int		uint32;

typedef signed char			int8;
typedef signed short		int16;
typedef signed int			int32;

typedef signed char			boolean;
#define FALSE						((boolean)(0))
#define TRUE						((boolean)(!FALSE))

typedef struct RECT {
	int32 topLeftX;
	int32 topLeftY;
	int32 bottomRightX;
	int32 bottomRightY;
}RECT;

typedef struct POS{
	int32 x;
	int32 y;
}POS;



#endif
