
#ifndef _MOUSE_KEYBOARD_H_
#define _MOUSE_KEYBOARD_H_

#include "interrupt.h"
#include "funcUtils.h"
#include "dataStruct.h"
#include "graphic.h"
#include "mem.h"
#include "sheet.h"
#include "wnd.h"

// ASCII code Abbr
#define ASC_BS			0x08
#define ASC_HT			0x09
#define ASC_CR			0x0D
#define ASC_ESC			0x1B



// pos in keytable of these keyCodes
#define KEY_ESC					1			//1
#define KEY_BACK				14		//14
#define KEY_TAB					15		//15
#define KEY_ENTER				28		//28
#define KEY_CTRL_D			29		//0x1D
#define KEY_CTRL_U			157		//0x9D
#define KEY_LSHIFT_D		42		//0x2A
#define KEY_LSHIFT_U		170		//0xAA
#define KEY_RSHIFT_D		54		//0x36
#define KEY_RSHIFT_U		182		//0xB6
#define KEY_ALT_D				56		//0x38
#define KEY_ALT_U				184		//0xB8
#define KEY_CAPSLOCK_D	58		//0x3A
#define KEY_CAPSLOCK_U	186		//0xBA
#define KEY_F1_D				59		//0x3B
#define KEY_F1_U				187		//0xBB
#define KEY_F2_D				60		//0x3C
#define KEY_F2_U				188		//0xBC
#define KEY_F3_D				61		//0x3D
#define KEY_F3_U				189		//0xBD
#define KEY_F4_D				62		//0x3E
#define KEY_F4_U				190		//0xBE
#define KEY_F5_D				63		//0x3F
#define KEY_F5_U				191		//0xBF
#define KEY_F6_D				64		//0x40
#define KEY_F6_U				192		//0xC0
#define KEY_F7_D				65		//0x41
#define KEY_F7_U				193		//0xC1
#define KEY_F8_D				66		//0x42
#define KEY_F8_U				194		//0xC2
#define KEY_F9_D				67		//0x43
#define KEY_F9_U				195		//0xC3
#define KEY_F10_D				68		//0x44
#define KEY_F10_U				196		//0xC4
#define KEY_NUMLOCK_D		250		//0x45		**
#define KEY_NUMLOCK_U		197		//0xC5		**
#define KEY_SRL_LOCK_D	250		//0xFA		**
#define KEY_SRL_LOCK_U	197		//0xC5		**
#define KEY_SYSRQ_D			84		//0x37		**
#define KEY_SYSRQ_U			170		//0xAA		**
#define KEY_F11_D				87		//0x57
#define KEY_F11_U				215		//0xD7
#define KEY_F12_D				88		//0x58
#define KEY_F12_U				216		//0xD8

#define KEY_WIN_D				91		//0x5B
#define KEY_WIN_U				219		//0xDB

#define KEY_DEL_D				83		//0x53
#define KEY_DEL_U				211		//0xD3


#define KEYCMD_LED		0xed

/** @brief the pressed state of mouse keys. */
typedef struct KEY_DOWN{
	boolean LKeyDown;  /**< Left Key on Mouse pressed */ 
	boolean CKeyDown;  /**< Center Key on Mouse pressed */
	boolean RKeyDown;  /**< Right Key on Mouse pressed */
}KEY_DOWN;

/** @brief mouse information. including pos, buffer & its sheet. etc. */
typedef struct MOUSE_CTL{
	int32 x;   /**< x value pos mouse pos. */
	int32 y;   /**< y value pos mouse pos. */
	boolean isMouseOff;   /**< is mouse enabled. */
	uint8	key;   /**< contains the pressed key on mouse. Program could get this from MouseGetKey(). */
	SHEET* sheet;   /**< sheet which contains this mouse. */
	uint8* buffer;  /**< buffer which contains pixels. */
}MOUSE_CTL;

#define KEYBOARD_BUFFER_LEN 1024

/** @brief keyboard information. including key buffer and function keys. */
typedef struct KEYBOARD_CTL{
	int8 buf[KEYBOARD_BUFFER_LEN];  /**< key buffer */
	int32 bufPos;  /**< pos of buffer */
	boolean ShiftOn; /**< is shift pressed */
	boolean AltOn;  /**< is alt pressed */
	boolean CtrlOn; /**< is Ctrl pressed */
	boolean WinOn;  /**< is Win pressed */
	
	boolean F1_On;  /**< is F1 pressed */
	boolean F2_On;  /**< is F2 pressed */
	boolean F3_On;  /**< is F3 pressed */
	boolean F4_On;  /**< is F4 pressed */
	boolean F5_On;  /**< is F5 pressed */
	boolean F6_On;  /**< is F6 pressed */
	boolean F7_On;  /**< is F7 pressed */
	boolean F8_On;  /**< is F8 pressed */
	boolean F9_On;  /**< is F9 pressed */
	boolean F10_On; /**< is F10 pressed */
	boolean F11_On; /**< is F11 pressed */
	boolean F12_On; /**< is F12 pressed */
	
	boolean DelOn; /**< is del pressed */
	boolean CapsLockOn; /**< is CapsLock on */
	boolean NumLockOn;  /**< is NumLock on */
	int32 keyleds;   /**< store the state of led on keyboard, not work currently. fix me. */
}KEYBOARD_CTL;

#define MOUSE_HEIGHT	10
#define MOUSE_WIDTH		10

void interrupt_handler_0x21(int32 *esp);	// Keyboard interrupt
void interrupt_handler_0x27(int32 *esp);	// Mouse interrupt
void interrupt_handler_0x2c(int32 *esp);

void waitForKBCSendReady();
void enableMouse();

void InitPeripheralBuffer();
void MouseKeyboardDeamon();

// ***** Mouse *****
boolean InitMouse();
void MouseGetPos(POS* pos);
void MouseGetKey(KEY_DOWN* keydown);

// ***** Keyboard *****
void InitKeyboard();
int32 KeyboardGetChar(int8* buffer, int32 cnt);


#endif

