#include "enlightenment.h"


static int ShowIdLine(int id, char *s, char *params);


void InitInfoBox()
{
}

void ShowLine(int *x,int *y,int i,int j,
		char *s,Window *wlst,Pixmap *plst,Pixmap *mlst) {
	int k;
	GC gcp;
	GC gcm;
	XGCValues gcv;

	k=0;
	while (wlst[k]) k++;

	wlst[k]=CreateBasicWin(root,*x,*y,ifb.width,ifb.height);
	*y=*y+ifb.height;
	plst[k]=XCreatePixmap(disp,wlst[k],ifb.width,ifb.height,depth);
	mlst[k]=XCreatePixmap(disp,wlst[k],ifb.width,ifb.height,1);
	gcp=XCreateGC(disp,plst[k],0,&gcv);
	gcm=XCreateGC(disp,mlst[k],0,&gcv);
	XSetForeground(disp,gcm,0);
	XFillRectangle(disp,mlst[k],gcm,0,0,ifb.width,ifb.height);
	DrawText(plst[k],s,ifb.mouse_width+ifb.key_width,0,
			ifb.width-(ifb.mouse_width+ifb.key_width),ifb.height);
	DrawTextMask(mlst[k],s,ifb.mouse_width+ifb.key_width,0,
			ifb.width-(ifb.mouse_width+ifb.key_width),ifb.height);
	XSetForeground(disp,gcp,1);
	XSetForeground(disp,gcm,1);
	if (i==1) {
		XCopyArea(disp,ifb.k2_pmap,plst[k],gcp,0,0,
				ifb.key_width,ifb.height,0,0);
		XCopyArea(disp,ifb.k2_mask,mlst[k],gcm,0,0,
				ifb.key_width,ifb.height,0,0);
	}
	if (i==2) {
		XCopyArea(disp,ifb.k3_pmap,plst[k],gcp,0,0,
				ifb.key_width,ifb.height,0,0);
		XCopyArea(disp,ifb.k3_mask,mlst[k],gcm,0,0,
				ifb.key_width,ifb.height,0,0);
	}
	if (i==3) {
		XCopyArea(disp,ifb.k4_pmap,plst[k],gcp,0,0,
				ifb.key_width,ifb.height,0,0);
		XCopyArea(disp,ifb.k4_mask,mlst[k],gcm,0,0,
				ifb.key_width,ifb.height,0,0);
	}
	if (j==0) {
		XCopyArea(disp,ifb.m1_pmap,plst[k],gcp,0,0,
				ifb.mouse_width,ifb.height,ifb.key_width,0);
		XCopyArea(disp,ifb.m1_mask,mlst[k],gcm,0,0,
				ifb.mouse_width,ifb.height,ifb.key_width,0);
	}
	if (j==1) {
		XCopyArea(disp,ifb.m2_pmap,plst[k],gcp,0,0,
				ifb.mouse_width,ifb.height,ifb.key_width,0);
		XCopyArea(disp,ifb.m2_mask,mlst[k],gcm,0,0,
				ifb.mouse_width,ifb.height,ifb.key_width,0);
	}
	if (j==2) {
		XCopyArea(disp,ifb.m3_pmap,plst[k],gcp,0,0,
				ifb.mouse_width,ifb.height,ifb.key_width,0);
		XCopyArea(disp,ifb.m3_mask,mlst[k],gcm,0,0,
				ifb.mouse_width,ifb.height,ifb.key_width,0);
	}
	XShapeCombineMask(disp,wlst[k],ShapeBounding,0,0,mlst[k],ShapeSet);
	XSetWindowBackgroundPixmap(disp,wlst[k],plst[k]);
	XMapWindow(disp,wlst[k]);
	XFreeGC(disp,gcp);
	XFreeGC(disp,gcm);
}

static int ShowIdLine(int id,char *s, char *params) {
	unsigned char showit=1;
	if(id < ACTION_KILL_NASTY) 
	switch(id) {
		case ACTION_MOVE:
			sprintf(s,"Move Window");break;
		case ACTION_RESIZE:
			sprintf(s,"Resize Window");break;
		case ACTION_ICONIFY:
			sprintf(s,"Iconify Window");break;
		case ACTION_KILL:
			sprintf(s,"Kill Window");break;
		case ACTION_RAISE:
			sprintf(s,"Raise Window To Top");break;
		case ACTION_LOWER:
			sprintf(s,"Lower Window To Bottom");break;
		case ACTION_RAISE_LOWER:
			sprintf(s,"Raise/Lower Window");break;
		case ACTION_MAX_HEIGHT:
			sprintf(s,"Toggle Window Max Height");break;
		case ACTION_MAX_WIDTH:
			sprintf(s,"Toggle Window Max Width");break;
		case ACTION_MAX_SIZE:
			sprintf(s,"Toggle Window Max Size");break;
		case ACTION_CONFIGURE:
			sprintf(s,"Configure Enlightenment");break;
		case ACTION_MENU:
			sprintf(s,"Menu : %s",params);break;
		case ACTION_EXEC:
			sprintf(s,"Execute : %s",params);break;
		case ACTION_EXIT:
			sprintf(s,"Exit");break;
	       }
	else
	switch(id) {
		case ACTION_KILL_NASTY:
			sprintf(s,"Forcibly Kill Window");break;
		case ACTION_RESTART:
			sprintf(s,"Restart");break;
		case ACTION_RESTART_THEME:
			sprintf(s,"Restart Theme : %s",params);break;
		case ACTION_BACKGROUND:
			sprintf(s,"Root Background : %s",params);break;
		case ACTION_DESKTOP:
			sprintf(s,"Change Desktop : %s",params);break;
		case ACTION_STICKY:
			sprintf(s,"Toggle Sticky Window");break;
		case ACTION_CYCLE_NEXT:
			sprintf(s,"Cycle to Next Window");break;
		case ACTION_CYCLE_PREV:
			sprintf(s,"Cycle to Previous Window");break;
		case ACTION_MOUSEFOCUS:
			sprintf(s,"Change Mouse Focus : %s",params);break;
		case ACTION_DRAG:
			sprintf(s,"Drag Window Icon");break;
		case ACTION_SKIP:
			sprintf(s,"Skip Window");break;
		case ACTION_SNAPSHOT:
			sprintf(s,"Snapshot Window");break;
		case ACTION_UNSNAPSHOT:
			sprintf(s,"Unsnapshot Window");break;
		case ACTION_SHADE:
			sprintf(s,"Shade Window"); break;
		default:
			showit=0;
			break;
	}
	return showit;

}

void GenListBtn(BWin *bwin) {
	int i,j,th;
	int x,y,d;
	unsigned int dui;
	Window w1,w2;
	char s[1024];
	Window wlst[64];
	Pixmap plst[64];
	Pixmap mlst[64];
	XEvent xev;

	for (i=0;i<64;i++) {
		wlst[i]=0;
		plst[i]=0;
		mlst[i]=0;
	}

	XQueryPointer(disp,root,&w1,&w2,&x,&y,&d,&d,&dui);
	th=0;
	for(i=0;i<4;i++)
		for(j=0;j<3;j++) 
			if(ShowIdLine(bwin->action[j][i].id,s,bwin->action[j][i].params)) 
				th+=ifb.height;
	if ((DisplayHeight(disp,screen)-y) < th)
		y=DisplayHeight(disp,screen)-th;

	if ((DisplayWidth(disp,screen)-x)<ifb.width)
		x=DisplayWidth(disp,screen)-ifb.width;

	for (i=0;i<4;i++)
		for (j=0;j<3;j++)
			if(ShowIdLine(bwin->action[j][i].id,s,bwin->action[j][i].params))
				ShowLine(&x,&y,i,j,s,wlst,plst,mlst);
	XSync(disp,False);
	XNextEvent(disp,&xev);
	XPutBackEvent(disp,&xev);
	i=0;
	while(wlst[i]) {
		XDestroyWindow(disp,wlst[i]);
		ImlibFreePixmap(imd,plst[i]);
		ImlibFreePixmap(imd,mlst[i]);
		i++;
	}
}


void GenListEWin(int b) {
	int i,j, th;
	int x,y,d;
	unsigned int dui;
	Window w1,w2;
	char s[1024];
	Window wlst[64];
	Pixmap plst[64];
	Pixmap mlst[64];
	XEvent xev;

	ifb.moveresize=0;
	for (i=0;i<64;i++) {
		wlst[i]=0;
		plst[i]=0;
		mlst[i]=0;
	}


	XQueryPointer(disp,root,&w1,&w2,&x,&y,&d,&d,&dui);
	th=0;
	for(i=0;i<4;i++)
		for(j=0;j<3;j++) 
			if(ShowIdLine(cfg.subwin_action[b][j][i].id,
					s,cfg.subwin_action[b][j][j].params))
			   th+=ifb.height;
	if ((DisplayHeight(disp,screen)-y) < th)
		y=DisplayHeight(disp,screen)-th;


	if ((DisplayWidth(disp,screen)-x)<ifb.width)
		x=DisplayWidth(disp,screen)-ifb.width;
	for (i=0;i<4;i++)
		for (j=0;j<3;j++)
			if(ShowIdLine(cfg.subwin_action[b][j][i].id,s,
					cfg.subwin_action[b][j][j].params))
				ShowLine(&x,&y,i,j,s,wlst,plst,mlst);
	i=0;
	while(!i) {
		XSync(disp,False);
		XNextEvent(disp,&xev);
		switch (xev.type) {
		case KeyPress:
		case KeyRelease:
		case ButtonPress:
		case ButtonRelease:
		case MotionNotify:
			i=1;
			break;
		default:
			break;
		}
	}
	i=0;
	while(wlst[i]) {
		XDestroyWindow(disp,wlst[i]);
		ImlibFreePixmap(imd,plst[i]);
		ImlibFreePixmap(imd,mlst[i]);
		i++;
	}
	ifb.moveresize=1;
}

void GenListIcon(Icon *Icon_We_Are_Over) {

	int i,j,th;
	int x,y,d;
	unsigned int dui;
	Window w1,w2;
	char s[1024];
	Window wlst[64];
	Pixmap plst[64];
	Pixmap mlst[64];
	XEvent xev;

	for (i=0;i<64;i++) {
		wlst[i]=0;
		plst[i]=0;
		mlst[i]=0;
	}


	XQueryPointer(disp,root,&w1,&w2,&x,&y,&d,&d,&dui);
	th=0;
	for(i=0;i<4;i++)
		for(j=0;j<3;j++) 
			if (ShowIdLine(icfg.action[j][i].id,s,icfg.action[j][i].params))
				th+=ifb.height;
	if ((DisplayHeight(disp,screen)-y) < th)
		y=DisplayHeight(disp,screen)-th;

	if ((DisplayWidth(disp,screen)-x)<ifb.width)
		x=DisplayWidth(disp,screen)-ifb.width;
	if(Icon_We_Are_Over)
		ShowLine(&x,&y,i,j,Icon_We_Are_Over->ewin->title,wlst,plst,mlst);
	for (i=0;i<4;i++)
		for (j=0;j<3;j++)
			if (ShowIdLine(icfg.action[j][i].id,s,icfg.action[j][i].params))
				ShowLine(&x,&y,i,j,s,wlst,plst,mlst);
	XSync(disp,False);
	XNextEvent(disp,&xev);
	XPutBackEvent(disp,&xev);
	i=0;
	while(wlst[i]) {
		XDestroyWindow(disp,wlst[i]);
		ImlibFreePixmap(imd,plst[i]);
		ImlibFreePixmap(imd,mlst[i]);
		i++;
	}
}

void InfoBox() {
	int x,y,d;
	Window w1,w2;
	unsigned int dui;

	if (ifb.nodo)
		ifb.moveresize=1;
	if (ifb.nodo) 
		return;
	XQueryPointer(disp,root,&w1,&w2,&x,&y,&d,&d,&dui);
	if (w2) {
		ShowInfo(w2);
	} else {
		ifb.moveresize=1;
	}
	timer_mode = 0;
}

void ShowInfo(Window win) {
	EWin *ewin;
	BWin *bwin;
	int wb,dsk;
	int x,y,d;
	unsigned int dui;
	Window w1,w2;
	EWin *wwin;
	Window win2;
	int i; /* j; */
	Window wlst[2];
	Pixmap plst[2];
	Pixmap mlst[2];
	XEvent xev;

	ewin=ListGetWinID(global_l,win);
	if (ewin) {
		XQueryPointer(disp,win,&w1,&w2,&x,&y,&d,&d,&dui);
		win=w2;
	}
	ewin=ListGetSubWinID(global_l,win);
	if ((dsk=WindowIsDeskInfo(win))>=0) {
		XQueryPointer(disp,win,&w1,&win2,&d,&d,&d,&d,&dui);
		wwin=WindowIsDeskInfoWin(win2);
		if(wwin) {
			XQueryPointer(disp,root,&w1,&w2,&x,&y,&d,&d,&dui);
			if ((DisplayWidth(disp,screen)-x)<ifb.width)
				x=DisplayWidth(disp,screen)-ifb.width;
			for(i=0;i<2;i++) {
				wlst[i]=0;
				plst[i]=0;
				mlst[i]=0;
			}
			ShowLine(&x,&y,4,4,wwin->title,wlst,plst,mlst);
   			XSync(disp,False);
   			XNextEvent(disp,&xev);
   			XPutBackEvent(disp,&xev);
			for(i=0;i<2;i++) {
				XDestroyWindow(disp,wlst[i]);
				ImlibFreePixmap(imd,plst[i]);
				ImlibFreePixmap(imd,mlst[i]);
			}
		}
	}
	if (ewin) {
		wb=GetEWinButtonID(ewin,win);
		if((evmd.mode!=MODE_MOVE) && (evmd.mode!=MODE_RESIZE))
			GenListEWin(wb);
	} else {
		bwin=GetButtonWinID(win);
		if (bwin) {
			GenListBtn(bwin);
		} else if (win==icfg.bg_win) {
			XQueryPointer(disp,win,&w1,&w2,&x,&y,&d,&d,&dui);
			win=w2;
			GenListIcon(GetIconWinID(win));
		}
	}
}
