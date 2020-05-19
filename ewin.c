#include "enlightenment.h"

int CheckClientExists(EWin *ewin) {

	int d;
	unsigned int dui;
	Window wd;

	if (!XGetGeometry(disp,ewin->client_win,&wd,&d,&d,&dui,&dui,&dui,&dui)) {
		fprintf(stderr,"Error 1: Unexpected Termination by client\n");
		global_killewin=ewin;
		return 0;
	}
	return 1;
}

EWin *InitEWin(Window client) {

	EWin *ewin;
	XSetWindowAttributes xatt;
	int i,j;
	Window wl[65];

	ewin=(EWin *)malloc(sizeof(EWin));/* malloc ram for the new ewin */

	ewin->border_l=cfg.border_l;
	ewin->border_r=cfg.border_r;
	ewin->border_t=cfg.border_t;
	ewin->border_b=cfg.border_b;
	ewin->state=NEW_WIN; /* set its state to UNMAPPED.... */
	ewin->client_win=client; /* the actual client window */
	ewin->group_win=0;
	ewin->icon_win=0;
	ewin->client_x=ewin->border_l; /* offset it into the frame */
	ewin->client_y=ewin->border_t;
	ewin->lastop=0;
	ewin->top=0;
	ewin->icon=NULL;
	GetWinName(client,ewin->title); 
	GetWinSize(client,ewin);
	GetWinColors(client,ewin);
	ewin->num_subwins=cfg.num_subwins; /* no subwins yet */
	ewin->di_win=0;
	ewin->snapshotok=0; /* Ready to be snapshotted for pager if this is on*/   
	ewin->skip=0;
	xatt.save_under=True;
	xatt.override_redirect=False;
	xatt.background_pixel=0;

	XSetWindowBorderWidth(disp,client,0);
	ewin->frame_win=XCreateWindow(disp,root,ewin->frame_x,ewin->frame_y,
			ewin->frame_width,ewin->frame_height,0,
			depth,CopyFromParent,visual,
			CWOverrideRedirect|CWSaveUnder|CWBackPixel,
			&xatt); 
	if (fx.shadow.on) {
		int r,g,b;

		r=fx.shadow.r;
		g=fx.shadow.g;
		b=fx.shadow.b;
		xatt.background_pixel=ImlibBestColorMatch(imd,&r,&g,&b);
		ewin->fx.shadow_win=XCreateWindow(disp,root,ewin->frame_x+fx.shadow.x,
				ewin->frame_y+fx.shadow.y,ewin->frame_width,ewin->frame_height,
				0, depth,CopyFromParent,visual,CWOverrideRedirect|CWBackPixel,
				&xatt);
		XLowerWindow(disp,ewin->fx.shadow_win);
	}
	for (i=0;i<ewin->num_subwins;i++) {
		ewin->subwins[i]=CreateWin(ewin->frame_win,0,0,10,10);
		XSelectInput(disp,ewin->subwins[i],PointerMotionMask);
		XMapWindow(disp,ewin->subwins[i]);
		ewin->subwin_pm_clk[i]=0;
		ewin->subwin_pm_sel[i]=0;
		ewin->subwin_pm_uns[i]=0;
		ewin->subwin_pm_clk_mask[i]=0;
		ewin->subwin_pm_sel_mask[i]=0;
		ewin->subwin_pm_uns_mask[i]=0;
		ewin->subwin_state[i]=NORM;
	}
	ewin->mask_sel=0;
	ewin->mask_uns=0;
	XAddToSaveSet(disp,client);
	GrabTheButtons(ewin->client_win);
	GrabTheKeys(ewin->frame_win);
	XReparentWindow(disp,client,ewin->frame_win,
			ewin->client_x,ewin->client_y);
	j=0;
	for (i=0;i<ewin->num_subwins;i++)
		if (cfg.subwin_level[i]) 
			wl[j++]=ewin->subwins[i];
	wl[j++]=ewin->client_win;
	for (i=0;i<ewin->num_subwins;i++)
		if (!cfg.subwin_level[i]) 
			wl[j++]=ewin->subwins[i];
	XRestackWindows(disp,wl,j);
	XSelectInput(disp,client,PropertyChangeMask);
	XSelectInput(disp,ewin->frame_win,SubstructureNotifyMask|ButtonPressMask|
			ButtonReleaseMask|EnterWindowMask|LeaveWindowMask|
			ButtonMotionMask|FocusChangeMask|ColormapChangeMask|
			PropertyChangeMask|SubstructureRedirectMask|
			PointerMotionMask); 
	ewin->changes=MOD_ALL;
	ewin->state=0;
	ewin->desk=desk.current;
	ewin->state_entry=-1;
	DrawSetupWindowBorder(ewin);
	MimickEwin(ewin);
	return ewin;
}

void ModifyEWin(EWin *ewin, int nx, int ny, int nw, int nh) {

	XWindowChanges xwc;
	int mask;
	XEvent ev;

	mask=0;
	if (nx!=ewin->frame_x) 
		mask|=CWX;
	if (ny!=ewin->frame_y) 
		mask|=CWY;
	if (nw!=ewin->client_width) 
		mask|=CWWidth;
	if (nh!=ewin->client_height) 
		mask|=CWHeight;

	ewin->frame_x=nx; /* record the new x & y in the ewin */
	ewin->frame_y=ny;
	if (nw>ewin->max_width) 
		nw=ewin->max_width;
	if (nh>ewin->max_height) 
		nh=ewin->max_height;
	if (nw<ewin->min_width) 
		nw=ewin->min_width;
	if (nh<ewin->min_height) 
		nh=ewin->min_height;
	if ((nw!=ewin->client_width)||(nh!=ewin->client_height)) {
		ewin->client_width=nw; /* new width & height for client */
		ewin->client_height=nh;
		ewin->frame_width=nw+ewin->border_l+ewin->border_r; 
		ewin->frame_height=nh+ewin->border_t+ewin->border_b; 
		ewin->changes|=MOD_SIZE;
	}
	xwc.x=ewin->frame_x;
	xwc.y=ewin->frame_y;
	xwc.width=ewin->frame_width;
	xwc.height=ewin->frame_height;
	XConfigureWindow(disp,ewin->frame_win,mask,&xwc);
	if (fx.shadow.on) {
		xwc.x=ewin->frame_x+fx.shadow.x;
		xwc.y=ewin->frame_y+fx.shadow.y;
		XConfigureWindow(disp,ewin->fx.shadow_win,mask,&xwc);
		XLowerWindow(disp,ewin->fx.shadow_win);
	}
	xwc.x=ewin->client_x;
	xwc.y=ewin->client_y;
	xwc.width=ewin->client_width;
	xwc.height=ewin->client_height;
	XConfigureWindow(disp,ewin->client_win,mask,&xwc);
	ev.type=ConfigureNotify;
	ev.xconfigure.display=disp;
	ev.xconfigure.event=ewin->client_win;
	ev.xconfigure.window=ewin->client_win;
	ev.xconfigure.x=ewin->frame_x+ewin->client_x;
	ev.xconfigure.y=ewin->frame_y+ewin->client_y;
	ev.xconfigure.width=ewin->client_width;
	ev.xconfigure.height=ewin->client_height;
	ev.xconfigure.border_width=0;
	ev.xconfigure.above=ewin->frame_win;
	ev.xconfigure.override_redirect=False;
	XSendEvent(disp,ewin->client_win,False,StructureNotifyMask,&ev);
	DrawWindowBorder(ewin);
	MimickEwin(ewin);
	CheckClientExists(ewin);
}

void KillEWin(EWin *ewin) {

	int i;

	XDestroySubwindows(disp,ewin->frame_win);
	XDestroyWindow(disp,ewin->frame_win);
	XDestroyWindow(disp,ewin->di_win);
	if (fx.shadow.on) 
		XDestroyWindow(disp,ewin->fx.shadow_win);
	for (i=0;i<ewin->num_subwins;i++) {
		if (ewin->subwin_pm_clk[i]) 
			ImlibFreePixmap(imd,ewin->subwin_pm_clk[i]);
		if (ewin->subwin_pm_uns[i]) 
			ImlibFreePixmap(imd,ewin->subwin_pm_uns[i]);
		if (ewin->subwin_pm_sel[i]) 
			ImlibFreePixmap(imd,ewin->subwin_pm_sel[i]);
		if (ewin->subwin_pm_clk_mask[i]) 
			ImlibFreePixmap(imd,ewin->subwin_pm_clk_mask[i]);
		if (ewin->subwin_pm_uns_mask[i]) 
			ImlibFreePixmap(imd,ewin->subwin_pm_uns_mask[i]);
		if (ewin->subwin_pm_sel_mask[i]) 
			ImlibFreePixmap(imd,ewin->subwin_pm_sel_mask[i]);
	}
	if (ewin->mask_sel) 
		ImlibFreePixmap(imd,ewin->mask_sel);
	if (ewin->mask_uns) 
		ImlibFreePixmap(imd,ewin->mask_uns);
	if (ewin->icon) 
		DelIcon(ewin->icon->win);
	free(ewin);
	XSetInputFocus(disp,root,RevertToNone,CurrentTime);

}

int GetEWinButtonID(EWin *ewin, Window win) {
	int i;

	for (i=0;i<ewin->num_subwins;i++) {
		if (ewin->subwins[i]==win) 
			return i;
	}
	return -1;
}
