/*
 *	
 * S4412 Dev Board key-pad header file 
 *
 */
#ifndef	_S4412_KEYCODE_H_

#define	_S4412_KEYCODE_H_
#if defined(CONFIG_KEYPAD_SM7S4412) 
	
#define	MAX_KEYCODE_CNT		3
	
int S4412_Keycode[MAX_KEYCODE_CNT] = {
	KEY_VOLUMEUP,	KEY_VOLUMEDOWN,	KEY_POWER
};
	
#if	defined(DEBUG_MSG)
	const char S4412_KeyMapStr[MAX_KEYCODE_CNT][20] = {
		"KEY_VOLUMEUP\n",	"KEY_VOLUMEDOWN\n",	"KEY_POWER\n"
	};
#endif	
#endif
#endif		
