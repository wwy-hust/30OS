
#ifndef _CMD_H_
#define _CMD_H_

#include "typedef.h"
#include "funcUtils.h"
#include "asmhead.h"
#include "globalTbl.h"
#include "wnd_console.h"
#include "fat.h"


boolean top(WND_CONSOLE* console, int32 argc, int8* argv[]);
boolean mem(WND_CONSOLE* console, int32 argc, int8* argv[]);
boolean help(WND_CONSOLE* console, int32 argc, int8* argv[]);
boolean ls(WND_CONSOLE* console, int32 argc, int8* argv[]);
boolean clear(WND_CONSOLE* console, int32 argc, int8* argv[]);
boolean type(WND_CONSOLE* console, int32 argc, int8* argv[]);
boolean hex(WND_CONSOLE* console, int32 argc, int8* argv[]);
boolean showWnd(WND_CONSOLE* console, int32 argc, int8* argv[]);
boolean exit(WND_CONSOLE* console, int32 argc, int8* argv[]);


boolean exec_bin(WND_CONSOLE* console, int32 argc, int8* argv[]);

#endif
