#include "enlightenment.h"
/* keys.c -- this file is gonna have all the keybindings stuff in it */

/* made 07/23/97 by Mandrake (mandrake@mandrake.net) */

void GrabTheKeys (Window win) {
	int i;

	for(i=0;i<KeyBind_num;i++)
		XGrabKey(disp, 
				XKeysymToKeycode(disp,
						XStringToKeysym(KeyBind_list[i].keycombo)),
			   	Emodifier(KeyBind_list[i].modifier),win,False, 
				GrabModeAsync,GrabModeAsync);
}

void GrabTheButtons(Window win) {
	XGrabButton(disp,1,AnyModifier,win,True,ButtonPressMask|ButtonReleaseMask,
			GrabModeAsync,GrabModeAsync,None,None);
	XGrabButton(disp,2,AnyModifier,win,True,ButtonPressMask|ButtonReleaseMask,
			GrabModeAsync,GrabModeAsync,None,None);
	XGrabButton(disp,3,AnyModifier,win,True,ButtonPressMask|ButtonReleaseMask,
			GrabModeAsync,GrabModeAsync,None,None);
}

void UnGrabTheButtons(Window win) {
	XUngrabButton(disp,1,AnyModifier,win);
	XUngrabButton(disp,2,AnyModifier,win);
	XUngrabButton(disp,3,AnyModifier,win);
}

unsigned int Emodifier(int mod) {
	switch (mod) {
		case 0:
			return 0;
		case 1:
			return ShiftMask;
		case 2:
			return ControlMask;
		case 3:
			return Mod1Mask;
	}
	return 0;
}
