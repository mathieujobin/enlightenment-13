#include "enlightenment.h"

void InitDesks() {
	int i;
	Image *im;
	int im_w,im_h;
	int dummy;
	unsigned int w,h,dummyui;
	Window wdummy;
	
	desk.icon_win=CreateBasicWin(root,0,0,desk.icon_width,desk.icon_height);
	if (desk.icon_pmap) 
		XSetWindowBackgroundPixmap(disp,desk.icon_win,desk.icon_pmap);
	if (desk.icon_mask) 
		XShapeCombineMask(disp,desk.icon_win,ShapeBounding,0,0,
				desk.icon_mask,ShapeSet);
	XSelectInput(disp,desk.icon_win,ButtonMotionMask|PointerMotionMask|
			ButtonPressMask|ButtonReleaseMask);
	for (i=0;i<MAX_DESKS;i++) {
		if (desk.background[i][0]) {
			im=LoadImage(imd,desk.background[i],NULL);
			XGetGeometry(disp,root,&wdummy,&dummy,&dummy,&w,&h,
					&dummyui,&dummyui);
			if (desk.bg_width[i]==0) 
				im_w=im->rgb_width;
			else if (desk.bg_width[i]==-1) 
				im_w=w;
			else 
				im_w=desk.bg_width[i];
			if (desk.bg_height[i]==0) 
				im_h=im->rgb_height;
			else if (desk.bg_height[i]==-1) 
				im_h=h;
			else 
				im_w=desk.bg_height[i];
			ImlibRender(imd,im,im_w,im_h);
			desk.bg_pmap[i]=ImlibMoveImageToPixmap(imd,im);
			ImlibRender(imd,im,(desk.di[i].root_width*im_w)/w,
					(desk.di[i].root_height*im_h)/h);
			desk.di[i].root_pmap=ImlibMoveImageToPixmap(imd,im);
			XSetWindowBackgroundPixmap(disp,desk.di[i].root_win,
					desk.di[i].root_pmap);
			XClearWindow(disp,desk.di[i].root_win);
			ImlibDestroyImage(imd,im);
		}
	}
}


void NextDesk(void) {
	ChangeToDesk(desk.current<(desk.num_desks)?desk.current+1:0);
}

void PrevDesk(void) {
	ChangeToDesk(desk.current>0?desk.current-1:desk.num_desks);
}

void ChangeToDesk(int dsk) {
	struct list *ptr;

	if (dsk==desk.current) 
		return;
	if (dsk<0) 
		return;
	if (dsk>=MAX_DESKS) 
		return;

	XSetInputFocus(disp,root,RevertToNone,CurrentTime);
	HighlightDeskInfo(desk.current,0);
	MapDesk(desk.current,0);
	if (desk.bg_pmap[dsk]) {
		XSetWindowBackgroundPixmap(disp,root,desk.bg_pmap[dsk]);
		XClearWindow(disp,root);
	}
	MapDesk(dsk,1);
	XSync(disp,False);
	desk.current=dsk;
	ptr=global_l->first;
	while(ptr) {
		MimickEwin(ptr->win);
		ptr=ptr->next;
	}
	HighlightDeskInfo(dsk,1);
}

void MapDesk(int dsk, int map) {

	struct list *node;

	node=global_l->first;

	while(node) {
		if ((node->win->desk<0) || (node->win->desk==dsk)) {
			if (map) {
				if ((node->win->state&MAPPED)&&
						(!(node->win->state&ICONIFIED))&&
						(!(node->win->state&UNMAPPED))) {
					/* unmap the frame */
					XMapWindow(disp,node->win->frame_win); 
					if (fx.shadow.on) 
						XMapWindow(disp,node->win->fx.shadow_win);
					MimickEwin(node->win);
				}
			} else {
				if ((node->win->state&MAPPED)&&
						(!(node->win->state&ICONIFIED))&&
						(!(node->win->state&UNMAPPED))&&(node->win->desk>-1)) {
					XUnmapWindow(disp,node->win->frame_win); 
					if (fx.shadow.on) 
						XUnmapWindow(disp,node->win->fx.shadow_win);
				}
			}
		}
		node=node->next;
	}
}

void MoveEwinToDesk(EWin *ewin, int dsk) {

	if (ewin->desk==dsk) 
		return;

	if (ewin->desk==desk.current) {
		if ((ewin->state&MAPPED)&&(!(ewin->state&ICONIFIED))&&
				(!(ewin->state&UNMAPPED))) {
			XUnmapWindow(disp,ewin->frame_win); /* unmap the frame */
			if (fx.shadow.on) 
				XUnmapWindow(disp,ewin->fx.shadow_win);
		}
	}
	if (dsk==desk.current) {
		if ((ewin->state&MAPPED)&&(!(ewin->state&ICONIFIED))&&
				(!(ewin->state&UNMAPPED))) {
			XMapWindow(disp,ewin->frame_win); /* unmap the frame */
			if (fx.shadow.on) 
				XMapWindow(disp,ewin->fx.shadow_win);
		}
	}
	ewin->desk=dsk;
	ewin->snapshotok=0;
}

void CreateDeskInfo(int dsk, int x, int y, int w, int h, 
		    int bx, int by, int bw, int bh, 
		    char *bg, ImColor *bicl, char *hl, ImColor *hicl,
		    int above) {
	if (dsk<0) 
		return;
	if (dsk>MAX_DESKS) 
		return;
	if (x<0) 
		x=DisplayWidth(disp,screen)+1+x-w;
	if (y<0) 
		y=DisplayHeight(disp,screen)+1+y-h;
	if (bx<0) 
		bx=DisplayWidth(disp,screen)+1+bx-bw;
	if (by<0) 
		by=DisplayHeight(disp,screen)+1+by-bh;
	desk.di[dsk].above=above;
	desk.di[dsk].root_x=x;
	desk.di[dsk].root_y=y;
	desk.di[dsk].root_width=w;
	desk.di[dsk].root_height=h;
	desk.di[dsk].x=bx;
	desk.di[dsk].y=by;
	desk.di[dsk].width=bw;
	desk.di[dsk].height=bh;
	desk.di[dsk].root_win=CreateBasicWin(root,x,y,w,h);
	desk.di[dsk].win=CreateBasicWin(root,bx,by,bw,bh);
	desk.num_desks=dsk>desk.num_desks?dsk:desk.num_desks;
	XSelectInput(disp,desk.di[dsk].root_win,ButtonMotionMask|
			PointerMotionMask|ButtonPressMask|ButtonReleaseMask);
	XSelectInput(disp,desk.di[dsk].win,ButtonMotionMask|
			PointerMotionMask|ButtonPressMask|ButtonReleaseMask);
	LoadImageSizeWithImlib(bg,bicl,&desk.di[dsk].bg_pmap,
			&desk.di[dsk].bg_mask,bw,bh);
	LoadImageSizeWithImlib(hl,hicl,&desk.di[dsk].hl_pmap,
			&desk.di[dsk].hl_mask,bw,bh);
	HighlightDeskInfo(dsk,0);
}

void MapDeskInfo() {

	int i;

	for (i=0;i<MAX_DESKS;i++) {
		if (desk.di[i].root_win) {
			XMapWindow(disp,desk.di[i].root_win);
			XMapWindow(disp,desk.di[i].win);
		}
	}
	XSync(disp,False);
}

void HighlightDeskInfo(int dsk,int onoff) {
	if (onoff) {
		if (desk.di[dsk].hl_pmap) 
			XSetWindowBackgroundPixmap(disp,desk.di[dsk].win,
					desk.di[dsk].hl_pmap);
		if (desk.di[dsk].hl_mask) 
			XShapeCombineMask(disp,desk.di[dsk].win,ShapeBounding,0,0,
					desk.di[dsk].hl_mask,ShapeSet);
	} else {
		if (desk.di[dsk].bg_pmap) 
			XSetWindowBackgroundPixmap(disp,desk.di[dsk].win,
					desk.di[dsk].bg_pmap);
		if (desk.di[dsk].bg_mask) 
			XShapeCombineMask(disp,desk.di[dsk].win,ShapeBounding,0,0,
					desk.di[dsk].bg_mask,ShapeSet);
	}
	XClearWindow(disp,desk.di[dsk].win);
	XSync(disp,False);
}

void MimickEwin(EWin *ewin) {

	int dw,dh;
	int dsk;
	int w,h,j,i;
	int r,g,b;
   	float x,y,inc;
	XWindowChanges xwc;
	Pixmap pmap,grab,tmp;
	GC gc; 
	XGCValues gcv; 
	XImage *xim;
	
	gc=XCreateGC(disp,ewin->client_win,0,&gcv);

	if (ewin->desk>=0)
		dsk=ewin->desk;
	else
		dsk=desk.current;
	if (!ewin->di_win) {
		ewin->di_win=CreateWin(desk.di[dsk].root_win,-10,-10,2,2);
		XSelectInput(disp,ewin->di_win,ButtonPressMask|ButtonReleaseMask);
				xwc.border_width=0;
		XConfigureWindow(disp,ewin->di_win,CWBorderWidth,&xwc);
	}
	dw=desk.di[dsk].root_width;
	dh=desk.di[dsk].root_height;
	w=(ewin->frame_width*dw)/DisplayWidth(disp,screen);
	h=(ewin->frame_height*dh)/DisplayHeight(disp,screen);
	XReparentWindow(disp,ewin->di_win,desk.di[dsk].root_win,
			(ewin->frame_x*dw)/DisplayWidth(disp,screen),
			(ewin->frame_y*dh)/DisplayHeight(disp,screen));
	XResizeWindow(disp,ewin->di_win,w,h);
	if (ewin->state&ICONIFIED) 
		XUnmapWindow(disp,ewin->di_win);
	else 
		XMapWindow(disp,ewin->di_win);
	if (ewin->state&UNMAPPED) 
		XUnmapWindow(disp,ewin->di_win);
	if((ewin->snapshotok)&&(desk.snapshotpager)) {
		/* Perhaps update a border? */
	} else if((desk.snapshotpager)&&!(ewin->state&UNMAPPED)&&
			((ewin->desk==-1)||(ewin->desk==desk.current))) { 
		/* using w & h for a sizes */
		tmp=XCreatePixmap(disp,ewin->client_win,ewin->client_width,h,depth); 
		pmap=XCreatePixmap(disp,ewin->client_win,w,h,depth); 
		xim=XGetImage(disp,ewin->client_win,0,0,ewin->client_width,
				ewin->client_height,0xffffffff,ZPixmap);  
		if (!xim) {
           grab=ewin->client_win;
		} else {
			grab=XCreatePixmap(disp,ewin->client_win,ewin->client_width,
					ewin->client_height,depth);
			XPutImage(disp,grab,gc,xim,0,0,0,0,ewin->client_width,
					ewin->client_height);
			XDestroyImage(xim);
		}
		inc=(float)ewin->client_height/(float)h;
		j=0;
		for (y=0;;y+=inc) {
			i=(int)y;
			if (i>=ewin->client_height) 
				break;
			XCopyArea(disp,grab,tmp,gc,0,i,ewin->client_width,1,0,j++);
		}
		inc=(float)ewin->client_width/(float)w;
		j=0;
		for (x=0;;x+=inc) {
			i=(int)x;
			if (i>=ewin->client_width) 
				break;
			XCopyArea(disp,tmp,pmap,gc,i,0,1,h,1+j++,1);
		}                                                        

		r=255,g=255,b=255;
		XSetForeground(disp,gc,ImlibBestColorMatch(imd,&r,&g,&b));
		XDrawLine(disp,pmap,gc,0,0,w-1,0);
		XDrawLine(disp,pmap,gc,0,0,0,h-1);
		r=0,g=0,b=0;
		XSetForeground(disp,gc,ImlibBestColorMatch(imd,&r,&g,&b));
		XDrawLine(disp,pmap,gc,0,h-1,w-1,h-1);
		XDrawLine(disp,pmap,gc,w-1,0,w-1,h-1);  
		XSetWindowBackgroundPixmap(disp,ewin->di_win,pmap);
		ImlibFreePixmap(imd,grab);
		ImlibFreePixmap(imd,pmap);
		ImlibFreePixmap(imd,tmp);
		XFreeGC(disp,gc);
		ewin->snapshotok=1; /* Flag so that I don't redraw unless necessary */
	} else /* Standard Pager */ {
		if (ewin->state&SELECTED) {
			ImlibRender(imd,desk.im_sel,w,h);
			pmap=ImlibMoveImageToPixmap(imd,desk.im_sel);
			if (pmap) 
				XSetWindowBackgroundPixmap(disp,ewin->di_win,pmap);
			ImlibFreePixmap(imd,pmap);
			pmap=ImlibMoveMaskToPixmap(imd,desk.im_sel);
			if (pmap) 
				XShapeCombineMask(disp,ewin->di_win,ShapeBounding,0,0,
						pmap,ShapeSet);
			ImlibFreePixmap(imd,pmap);
			XClearWindow(disp,ewin->di_win);
		} else {
			ImlibRender(imd,desk.im_unsel,w,h);
			pmap=ImlibMoveImageToPixmap(imd,desk.im_unsel);
			if (pmap) 
				XSetWindowBackgroundPixmap(disp,ewin->di_win,pmap);
			ImlibFreePixmap(imd,pmap);
			pmap=ImlibMoveMaskToPixmap(imd,desk.im_unsel);
			if (pmap) 
				XShapeCombineMask(disp,ewin->di_win,ShapeBounding,0,0,
						pmap,ShapeSet);
			ImlibFreePixmap(imd,pmap);
			XClearWindow(disp,ewin->di_win);
		}
	}
	XSync(disp,False);
}

int WindowIsDeskInfo(Window win) {

	int i;

	if (!win) 
		return -1;
	for(i=0;i<MAX_DESKS;i++)
		if ((win==desk.di[i].root_win)||(win==desk.di[i].win)) 
			return i;
	return -1;
}

EWin *WindowIsDeskInfoWin(Window win) {

	struct list *ptr;

	ptr=global_l->first;
	while (ptr) {
		if (ptr->win->di_win==win) 
			return ptr->win;
		ptr=ptr->next;
	}
	return NULL;
}

void StartIconDrag(EWin *ewin) {
/* This code was modified by CmdrTaco to allow the windows
** to behave friendlier.  These changes include moving windows
** on a desktop, and the icon_win is now sized relative
** to it's size on the pager, instead of relative to the pixmap
** size. 	7/5/97
*/
	Window w1,w2;
	XEvent xev;
	int sx,sy,xx,yy, wx,wy; /* my, mx; */
	int dsk;
	Window r;
	unsigned int  wr, dui, hr, br, dr;
	int dx, dy,tx,ty,rx,ry,d;

	if (ewin->state&ICONIFIED) {
		DelIcon(ewin->icon->win);
		Msg_DeIconify(ewin);
	}	

	XSetInputFocus(disp,root,RevertToNone,CurrentTime);
	XGetGeometry(disp, ewin->di_win, &r , &rx, &ry, &wr, &hr, &br, &dr);
	XUnmapWindow(disp,ewin->di_win);

	XQueryPointer(disp,root,&w1,&w2,&sx,&sy,&wx,&wy,&dui);
	if ((w2==icfg.bg_win)||(ListGetWinID(global_l,w2))) {
		XQueryPointer(disp,root,&w1,&w2,&sx,&sy,&wx,&wy,&dui);
		wx=wr/2;
		wy=hr/2;
		XReparentWindow(disp,ewin->di_win,root,sx-wx,sy-wy);
	} else {
		XQueryPointer(disp,ewin->di_win,&w1,&w2,&sx,&sy,&wx,&wy,&dui); 
		XReparentWindow(disp,ewin->di_win,root,sx-wx,sy-wy);
	}

	XGrabServer(disp);
	XGrabPointer(disp,ewin->di_win,True, ButtonMotionMask|PointerMotionMask|ButtonReleaseMask, GrabModeAsync,GrabModeAsync,None,None,CurrentTime);
	XSync(disp,False);

	for(;;) {
 		XRaiseWindow(disp,ewin->di_win);
		XMapWindow(disp,ewin->di_win);
		XMoveWindow(disp,ewin->di_win,sx-wx,sy-wy);

		XSync(disp,False);  

		XMaskEvent(disp,ButtonMotionMask|PointerMotionMask|ButtonReleaseMask,&xev);
		XQueryPointer(disp,root,&w1,&w2,&sx,&sy,&xx,&yy,&dui);
		if (xev.type==ButtonRelease) {
			XUnmapWindow(disp,ewin->di_win);
			XSync(disp,False);
			XUngrabPointer(disp, CurrentTime);
			XUngrabServer(disp);
			XQueryPointer(disp,root,&w1,&w2,&sx,&sy,&d,&d,&dui);
			if ((dsk=WindowIsDeskInfo(w2))>=0) {
				ewin->snapshotok=0;
				MoveEwinToDesk(ewin,dsk);
				/* Move the Ewin to match location on pager */
				XGetGeometry(disp,desk.di[dsk].root_win,&r,&rx,&ry,&wr,&hr,&br,&dr);
				dx=DisplayWidth(disp,screen)/wr;
				dy=DisplayHeight(disp,screen)/hr;    

				tx=dx*((sx-wx)-rx);
				ty=dy*((sy-wy)-ry);
				ModifyEWin(ewin,tx,ty,ewin->client_width,ewin->client_height);
				/*Do_RaiseWin(ewin);*/

			} else {
				if (w2==icfg.bg_win) /* Dragged off to Iconbox */ {
					if (!(ewin->state&ICONIFIED))
						Do_IconifyWin(ewin);
				} else /* Dragged off to desktop */ {
					ModifyEWin(ewin,(sx-wx-(ewin->frame_width/2)), (sy-wy-(ewin->frame_height/2)), ewin->client_width, ewin->client_height);
					MoveEwinToDesk(ewin,desk.current);
					MimickEwin(ewin);
				}
			}
			return;
		}
	}
}
