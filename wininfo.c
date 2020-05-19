#include "enlightenment.h"

void GetWinName(Window win, char *s) {
	XTextProperty xtp;

	if (XGetWMName(disp,win,&xtp))
		strncpy(s,(const char *) xtp.value,256);
	else 
		strcpy(s,"No Title");
}

void GetWinSize(Window win,EWin *ewin) {

	Window www;
	unsigned int nw,nh;
	int dum,dumm;
	int nx,ny;
	XWMHints *hints1;
	XSizeHints hints2;
	long mask;

	nx = ewin->frame_x;
	ny = ewin->frame_y;

	XGetGeometry(disp,win,&www,&nx,&ny,&nw,&nh,&dum,&dum);
	ewin->base_width=0;
	ewin->base_height=0;
	if (!XGetTransientForHint(disp,win,&ewin->group_win)) 
		ewin->group_win=0;
	hints1=XGetWMHints(disp,win);
	if (hints1) {
		if (hints1->flags&StateHint) {

		}
		XFree (hints1);
	}
	if (XGetWMNormalHints(disp,win,&hints2,&mask)) {
		if (hints2.flags&PResizeInc) {
			ewin->sizeinc_x=hints2.width_inc;
			ewin->sizeinc_y=hints2.height_inc;
			if (ewin->sizeinc_x<=0) 
				ewin->sizeinc_x=1;
			if (ewin->sizeinc_y<=0) 
				ewin->sizeinc_y=1;
		} else {
			ewin->sizeinc_x=1;
			ewin->sizeinc_y=1;
		}
		if (hints2.flags&PBaseSize) {
			if (hints2.flags&PMinSize) {
				ewin->min_width=hints2.min_width;
				ewin->min_height=hints2.min_height;
			} else {
				ewin->min_width=hints2.base_width;
				ewin->min_height=hints2.base_height;
			}
			ewin->base_width=hints2.base_width;
			ewin->base_height=hints2.base_height;
		} else if (hints2.flags&PMinSize) {
			ewin->min_width=hints2.min_width;
			ewin->min_height=hints2.min_height;
		} else { 
			ewin->min_width=1;
			ewin->min_height=1;
		}
		if (hints2.flags&PMaxSize) {
			ewin->max_width=hints2.max_width;
			ewin->max_height=hints2.max_height;
		} else {
			ewin->max_width=MAX_WIDTH;
			ewin->max_height=MAX_HEIGHT;
		}
		if (ewin->min_height<=0) 
			ewin->min_height=1;
		if (ewin->min_width<=0) 
			ewin->min_width=1;
		if (ewin->max_width<ewin->min_width) 
			ewin->max_width=ewin->min_width;
		if (ewin->max_height<ewin->min_height) 
			ewin->max_height=ewin->min_height;
		if (nw>ewin->max_width) 
			nw=ewin->max_width;
		if (nh>ewin->max_height) 
			nh=ewin->max_height;
		if (nw<ewin->min_width) 
			nw=ewin->min_width;
		if (nh<ewin->min_height) 
			nh=ewin->min_height;

		/* width & height for frame */
		ewin->frame_width=nw+ewin->border_l+ewin->border_r; 
		ewin->frame_height=nh+ewin->border_t+ewin->border_b; 

		if (hints2.flags&PPosition) {
		} else if (hints2.flags&USPosition) {
		} else {
			dum=DisplayWidth(disp,screen)-ewin->frame_width;
			dumm=DisplayHeight(disp,screen)-ewin->frame_height;
			if (dum>0) 
				nx=rand()%dum;
			else 
				nx=0;
			if (dumm>0) 
				ny=rand()%dumm;
			else 
				ny=0;
		}
	} else {
		ewin->sizeinc_x=1;
		ewin->sizeinc_y=1;
		ewin->min_width=1;
		ewin->min_height=1;
		ewin->max_width=MAX_WIDTH;
		ewin->max_height=MAX_HEIGHT;
		if (nw>ewin->max_width) 
			nw=ewin->max_width;
		if (nh>ewin->max_height) 
			nh=ewin->max_height;
		if (nw<ewin->min_width) 
			nw=ewin->min_width;
		if (nh<ewin->min_height) 
			nh=ewin->min_height;

		/* width & height for frame */
		ewin->frame_width=nw+ewin->border_l+ewin->border_r; 
		ewin->frame_height=nh+ewin->border_t+ewin->border_b; 

		dum=DisplayWidth(disp,screen)-ewin->frame_width;
		dumm=DisplayHeight(disp,screen)-ewin->frame_height;
		if (dum>0) 
			nx=rand()%dum;
		else 
			nx=0;
		if (dumm>0) 
			ny=rand()%dumm;
		else 
			ny=0;
	}
	/* record the client info for width & height */
	/* dont allow windows to move themselves off 
	 * the screen under application control */
	if (ewin->state&NEW_WIN) {
		ewin->client_width=nw; 
		ewin->client_height=nh;
		XResizeWindow(disp,ewin->client_win,
				ewin->client_width,ewin->client_height);
		ewin->frame_x=nx; /* X & Y locations desired by client */
		ewin->frame_y=ny;
		ewin->state&=~NEW_WIN;
	} else if(nw != ewin->client_width || nh != ewin->client_height)
		ModifyEWin(ewin,ewin->frame_x,ewin->frame_y,nw,nh);
}

void GetWinColors(Window win,EWin *ewin) {
	XWindowAttributes xwa;
	if (XGetWindowAttributes(disp,win,&xwa)) {
		if (xwa.colormap)
			ewin->colormap=xwa.colormap;
		else 
			ewin->colormap=0;
	} else 
		ewin->colormap=0;
}

void UnmapClients(int all) {
	Window rt;
	Window par;
	Window *list;
	unsigned int num;
	int i;
	XWindowAttributes attr;

	if (debug_mode) 
		return;
	XQueryTree(disp,root,&rt,&par,&list,&num);
	Wlist=NULL;  

	if (list) {
		Wlist=malloc(sizeof(struct winlist)*(num+1));
		for (i=0;i<num;i++) {
			XGetWindowAttributes(disp, list[i], &attr);
			Wlist[i].win=list[i];
			if(attr.override_redirect) {
				if(attr.map_state==IsUnmapped)
					Wlist[i].state = REDIR;
				else
					Wlist[i].state = REDIRM;
			} else if(attr.map_state==IsUnmapped)
				Wlist[i].state = UNMAP;
			else
				Wlist[i].state = MAP;
			if((!attr.override_redirect&&attr.map_state!=IsUnmapped)||all==1)
				XUnmapWindow(disp,list[i]);
		}
		Wlist[num].state=NOEXIST;
		Wlist[num].win=0;
		XFlush(disp);
		free(list);
	}
}

void MapClients(listhead *l) {
	Window rt;
	Window par;
	Window *list;
	unsigned int num;
	int i,j;
	EWin *ewin;
	XWindowAttributes attr;

	XQueryTree(disp,root,&rt,&par,&list,&num);
	if (list) {
		for (i=0;i<num;i++) {
			XGetWindowAttributes(disp, list[i], &attr);
			if (GetButtonWinID(list[i])) 
				XMapWindow(disp,list[i]);
			else if (list[i]==icfg.bg_win) 
				XMapWindow(disp,list[i]);
			else if (list[i]==icfg.left_win) 
				XMapWindow(disp,list[i]);
			else if (list[i]==icfg.right_win) 
				XMapWindow(disp,list[i]);
			else if (GetMenuWin(list[i])) ;
			else if (!GetButtonWinID(list[i])) {
				if (Wlist) {
					for(j=0;Wlist[j].state!=NOEXIST;j++) {
						if (list[i]==Wlist[j].win) {
							switch(Wlist[j].state) {
								case MAP:
									ewin=InitEWin(Wlist[i].win); 
									ListAdd(l,ewin);
									XRaiseWindow(disp,ewin->frame_win);
									XMapSubwindows(disp,ewin->frame_win);
									if (!(ewin->state&ICONIFIED)) {
										XMapWindow(disp,ewin->frame_win);
										if(fx.shadow.on) 
											XMapWindow(disp,
													ewin->fx.shadow_win);
									}
									ewin->state|=MAPPED;
									if(restart) 
										ConformEwinToState(ewin);
									break;
								case REDIRM:
									XMapWindow(disp, list[i]);
									break;
								default:
									break;
							}
						}
					}
				}
			}
		}
		XFlush(disp);
		free(list);
		if (Wlist) 
			free(Wlist);
	}
}
