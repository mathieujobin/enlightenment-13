#include "enlightenment.h"
#include <stdarg.h>
#include <stdio.h>

/* Mod. 5/1/97 Bill Garrett <garrett@qcsn.com>
   Let Alert() take a variable argument list, ie
   Alert( "Can't find file %s in directory %s", filename, dirname );

   Use GNU extension "vsnprintf" for size limiting if available. */
void Alert(char *fmt, ...) {
	char text[4096];
	va_list ap;

	va_start( ap, fmt );
#ifdef __USE_VSNPRINTF
	vsnprintf(text,4096,fmt,ap);
#else
	vsprintf( text, fmt, ap );
#endif
	va_end( ap );

	if (imd) {
		if (cfg.font) 
			_Alert_Imlib_Font(text);
		else 
			_Alert_Imlib(text);
	} else 
		_Alert(text);
}

void _Alert_Imlib_Font(char *text) {
	Window win;
	int wid,hih;
	int c1,c2,c3;
	int r,g,b;
	int w,h;
	XGCValues gcv;
	GC gc;
	char line[1024];
	int i,j,k;
	XEvent ev;

	if (!text) 
		return;

	r=192,g=192,b=192;
	c1=ImlibBestColorMatch(imd,&r,&g,&b);
	r=128,g=128,b=128;
	c2=ImlibBestColorMatch(imd,&r,&g,&b);
	r=64,g=64,b=64;
	c3=ImlibBestColorMatch(imd,&r,&g,&b);
	wid=DisplayWidth(disp,screen);
	hih=DisplayHeight(disp,screen);
	w=(wid-512)/2;
	h=(hih-384)/2;
	win=XCreateSimpleWindow(disp,root,w,h,512,384,0,0,c2);
	gc=XCreateGC(disp,win,0,&gcv);
	XMapWindow(disp,win);
	XSync(disp,False);
	XSetForeground(disp,gc,c1);
	XDrawLine(disp,win,gc,0,0,0,382);
	XDrawLine(disp,win,gc,0,0,510,0);
	XSetForeground(disp,gc,c3);
	XDrawLine(disp,win,gc,0,383,511,383);
	XDrawLine(disp,win,gc,511,0,511,383);

	i=0;
	j=0;
	k=0;
	while(text[i]) {
		line[j++]=text[i++];
		if (line[j-1]=='\n') {
			line[j-1]=0;
			j=0;
			DrawText(win,line,6,6+k,500,14);
			k+=14;
		}
	}
	DrawText(win,"LeftButton: Restart   MiddleButton: Ignore   RightButton: Quit",6,6+k+14+14,500,14);
	XSync(disp,False);
	XSelectInput(disp,win,ButtonPressMask);
	XWindowEvent(disp,win,ButtonPressMask,&ev);
	_Alert_Handle_Click(ev.xbutton.button);
	XDestroyWindow(disp,win);
}

void _Alert_Imlib(char *text) {

	Window win;
	int wid,hih;
	int c1,c2,c3;
	int r,g,b;
	int w,h;
	XGCValues gcv;
	GC gc;
	char line[1024];
	int i,j,k;
	XEvent ev;

	XFontStruct *xfs;
	Font font;

	if (!text) 
		return;

	r=192,g=192,b=192;
	c1=ImlibBestColorMatch(imd,&r,&g,&b);
	r=128,g=128,b=128;
	c2=ImlibBestColorMatch(imd,&r,&g,&b);
	r=64,g=64,b=64;
	c3=ImlibBestColorMatch(imd,&r,&g,&b);
	wid=DisplayWidth(disp,screen);
	hih=DisplayHeight(disp,screen);
	w=(wid-512)/2;
	h=(hih-384)/2;
	win=XCreateSimpleWindow(disp,root,w,h,512,384,0,0,c2);
	gc=XCreateGC(disp,win,0,&gcv);
	XMapWindow(disp,win);
	XSync(disp,False);
	XSetForeground(disp,gc,c1);
	XDrawLine(disp,win,gc,0,0,0,382);
	XDrawLine(disp,win,gc,0,0,510,0);
	XSetForeground(disp,gc,c3);
	XDrawLine(disp,win,gc,0,383,511,383);
	XDrawLine(disp,win,gc,511,0,511,383);

	font=XLoadFont(disp,"fixed");   
	xfs=XLoadQueryFont(disp,"fixed");
	XSetForeground(disp,gc,BlackPixel(disp,screen));
	XSetFont(disp,gc,font);
	i=0;
	j=0;
	k=0;
	while(text[i]) {
		line[j++]=text[i++];
		if (line[j-1]=='\n') {
			line[j-1]=0;
			j=0;
			XDrawString(disp,win,gc,6,6+k+14,line,strlen(line));
			k+=14;
		}
	}
	XDrawString(disp,win,gc,6,6+k+14+14+14,
			"LeftButton: Restart   MiddleButton: Ignore   RightButton: Quit",
			62);
	XFreeFont(disp,xfs);
	XSync(disp,False);
	XSelectInput(disp,win,ButtonPressMask);
	XWindowEvent(disp,win,ButtonPressMask,&ev);
	XDestroyWindow(disp,win);
	_Alert_Handle_Click(ev.xbutton.button);
}

void _Alert(char *text) {
	Window win;
	int wid,hih;
	int w,h;
	XGCValues gcv;
	GC gc;
	char line[1024];
	int i,j,k;
	XEvent ev;

	XFontStruct *xfs;
	Font font;

	if (!text) 
		return;

	wid=DisplayWidth(disp,screen);
	hih=DisplayHeight(disp,screen);
	w=(wid-512)/2;
	h=(hih-384)/2;
	win=XCreateSimpleWindow(disp,root,w,h,512,384,1,
			BlackPixel(disp,screen),WhitePixel(disp,screen));
	gc=XCreateGC(disp,win,0,&gcv);
	XMapWindow(disp,win);
	XSync(disp,False);
	XSetForeground(disp,gc,WhitePixel(disp,screen));

	font=XLoadFont(disp,"fixed");   
	xfs=XLoadQueryFont(disp,"fixed");
	XSetFont(disp,gc,font);
	i=0;
	j=0;
	k=0;
	while(text[i]) {
		line[j++]=text[i++];
		if (line[j-1]=='\n') {
			line[j-1]=0;
			j=0;
			XDrawString(disp,win,gc,6,6+k+14,line,strlen(line));
			k+=14;
		}
	}
	XDrawString(disp,win,gc,6,6+k+14+14+14,
			"LeftButton: Restart   MiddleButton: Ignore   RightButton: Quit",
			62);
	XSync(disp,False);
	XSelectInput(disp,win,ButtonPressMask);
	XWindowEvent(disp,win,ButtonPressMask,&ev);
	XDestroyWindow(disp,win);
	_Alert_Handle_Click(ev.xbutton.button);
}

void _Alert_Handle_Click(int button) {
	printf("Th-th-th-th-tha-that's all folks! (Segmentation occured)\n");
	switch (button) {
		case 1:
			Do_Restart();
			break;
		case 2:
			return;
			break;
		case 3:
			Do_Exit();
		default:
			break;
	}
}

