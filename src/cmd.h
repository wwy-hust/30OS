/** @file cmd.h
 *  @brief cmd you could used in windows console.
 *
 *  This file implemented some useful command for use in console. There are top, mem, help, ls, type, hex etc.
 */
 
#ifndef _CMD_H_
#define _CMD_H_

#include "typedef.h"
#include "funcUtils.h"
#include "asmhead.h"
#include "globalTbl.h"
#include "wnd_console.h"
#include "fat.h"

/** @addtogroup commands
 *  @brief useful commands in console, you can call it directly from console.
 *  @{
 */

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
/// @}

#endif
