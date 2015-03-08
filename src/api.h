/** @file api.h
 *  @brief OS api callback by INT 0x40.
 *
 *  Everytime application call os api. It actually trigger an interrupt with INT 0x40.
 *  OS use asm_os_console_api as the interrupt handler. you could refer it in globalTbl.c.
 *  API_xxx is called by asm_os_console_api according to edx. The routine array is listed in wnd_console.c named api_routine_array.
 *  Every API_xxx has an version named api_xxx. api_xxx is called by application.
 */
 
#ifndef _API_H_
#define _API_H_

#include "string.h"

#include "app_funcUtils.h"
#include "typedef.h"
//#include "cmd.h"
#include "wnd_console.h"

/** @addtogroup OS_API
 *  @brief Os api called by application
 *  @{
 */

/** @addtogroup api_internal_wraper
 *  @brief call actually dispatch to these functions.
 *  @{
 */
typedef int32 (*API_ROUNTINE)(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
#define API_RETURN(val) (*(((int*)(&eax) + 18)) = val)

// console naked puts
int32 API_putchar(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_puts(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_putn_chars(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
// app end
int32 API_end_app(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
// wnd related api
int32 API_create_wnd(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_delete_wnd(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_wnd_fillbox(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_wnd_draw_text(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_wnd_draw_point(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_wnd_draw_line(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_wnd_draw_content(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_wnd_get_keyboard(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_wnd_refresh(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_cp_os_mem(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
// timer related api
int32 API_timer_alloc(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_timer_delete(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_timer_set_timeout(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_timer_is_timeout(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);

int32 API_beep(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);

// file operation related api
int32 API_open(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_close(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_seek(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_read(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
int32 API_file_size(int32 edi, int32 esi, int32 ebp, int32 esp, int32 ebx, int32 edx, int32 ecx, int32 eax);
/// @}
/// @}

#endif
