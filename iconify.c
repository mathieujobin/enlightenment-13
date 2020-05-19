#include "enlightenment.h"

Icon *GrabClient(EWin *ewin) {
	Icon *ic;
	Pixmap tmp;
	Pixmap grab;
	XImage *xim;
	int w,h;
	int ww,hh;
	int cw,ch;
	int gw,gh,gx,gy;
	int i,j;
	float x,y;
	float inc;
	GC gc;
	XGCValues gcv;
	int r,g,b;

	ic=malloc(sizeof(Icon));
	ic->ewin=ewin;ic->pmap=0;ic->mask=0;ic->win=0;
	ic->ewin->icon=ic;
	cw=ewin->client_width;
	ch=ewin->client_height;
	gc=XCreateGC(disp,ewin->client_win,0,&gcv);

	if (icfg.orientation) {
		if ((4*ch)>(3*cw)) {
			/* if client is higher that it is wide, limit the widht to 4/3 */
			/* times the width of the iconbox */
			h=(((icfg.width-(icfg.bx*2))*3)/4);
			w=((cw*h)/ch);
		} else {
			w=(icfg.width-(icfg.bx*2));
			h=((ch*w)/cw);
		}
	} else {
		if ((3*cw)>(4*ch)) {
			/* if client is wider that it is high, limit the widht to 4/3 */
			/* times the height of the iconbox */
			w=(((icfg.height-(icfg.by*2))*4)/3);
			h=((ch*w)/cw);
		} else {
			h=(icfg.height-(icfg.by*2));
			w=((cw*h)/ch);
		}
	}
	ww=w-1;if (ww<1) w=1;
	hh=h-1;if (hh<1) h=1;
	ic->pmap=XCreatePixmap(disp,ewin->client_win,w,h,depth);
	ic->mask=0;
	ic->win=CreateWin(icfg.bg_win,0,0,w,h);
	ic->width=w;
	ic->height=h;
	ic->x=0;
	ic->y=0;
	tmp=XCreatePixmap(disp,ewin->client_win,cw,hh,depth);
	grab=XCreatePixmap(disp,ewin->client_win,cw,ch,depth);
	gx=0;gy=0;gw=cw;gh=ch;
	if (ewin->frame_x+ewin->client_x<0) {
		gx=-(ewin->frame_x+ewin->client_x);
		gw-=gx;
	}
	if (ewin->frame_y+ewin->client_y<0) {
		gy=-(ewin->frame_y+ewin->client_y);
		gh-=gy;
	}
	if (ewin->frame_x+ewin->client_x+ewin->client_width>=DisplayWidth(disp,
			screen))
		gw=DisplayWidth(disp,screen)-(ewin->frame_x+ewin->client_x+gx);
	if (ewin->frame_y+ewin->client_y+ewin->client_height>=DisplayHeight(disp,
			screen))
		gh=DisplayHeight(disp,screen)-(ewin->frame_y+ewin->client_y+gy);
	xim=NULL;
	if ((gw>0)&&(gh>0))
		xim=XGetImage(disp,ewin->client_win,gx,gy,gw,gh,0xffffffff,ZPixmap);
	if (!xim) {
		ImlibFreePixmap(imd,grab);
		grab=ewin->client_win;
	} else {
		XPutImage(disp,grab,gc,xim,0,0,gx,gy,gw,gh);
		XDestroyImage(xim);
	}
	inc=(float)ch/(float)hh;
	j=0;
	for (y=0;;y+=inc) {
		i=(int)y;
		if (i>=ch) 
			break;
		XCopyArea(disp,grab,tmp,gc,0,i,cw,1,0,j++);
	}
	inc=(float)cw/(float)ww;
	j=0;
	for (x=0;;x+=inc) {
		i=(int)x;
		if (i>=cw) 
			break;
		XCopyArea(disp,tmp,ic->pmap,gc,i,0,1,hh,1+j++,1);
	}
	/* draw bevels around icon */
	r=255,g=255,b=255;
	XSetForeground(disp,gc,ImlibBestColorMatch(imd,&r,&g,&b));
	XDrawLine(disp,ic->pmap,gc,0,0,ic->width-1,0);
	XDrawLine(disp,ic->pmap,gc,0,0,0,ic->height-1);
	r=0,g=0,b=0;
	XSetForeground(disp,gc,ImlibBestColorMatch(imd,&r,&g,&b));
	XSetWindowBackgroundPixmap(disp,ic->win,ic->pmap);
	XDrawLine(disp,ic->pmap,gc,0,ic->height-1,ic->width-1,ic->height-1);
	XDrawLine(disp,ic->pmap,gc,ic->width-1,0,ic->width-1,ic->height-1);
	/* free up after uourselves */
	ImlibFreePixmap(imd,tmp);
	ImlibFreePixmap(imd,grab);
	XFreeGC(disp,gc); 
	return ic;
}

void LoadImageWithImlib(char *reg_im, ImColor *reg_trans, 
		Pixmap *pmap, Pixmap *mask, int *width, int *height) {

	Image *im;

	im=LoadImage(imd,reg_im,reg_trans);
	if (!im) {
		Alert("Error 2: Unable to load the image file:\n%s\n",reg_im);
		EExit(1);
	}
	ImlibRender(imd,im,im->rgb_width,im->rgb_height);
	*pmap=ImlibMoveImageToPixmap(imd,im);
	*mask=ImlibMoveMaskToPixmap(imd,im);
	*width=im->rgb_width;
	*height=im->rgb_height;
	ImlibDestroyImage(imd,im);
}

void LoadImageSizeWithImlib(char *reg_im, ImColor *reg_trans, Pixmap *pmap, 
		Pixmap *mask, int width, int height) {

	Image *im;

	im=LoadImage(imd,reg_im,reg_trans);
	if (!im) {
		Alert("Error 2: Unable to load the image file:\n%s\n",reg_im);
		EExit(1);
	}
	ImlibRender(imd,im,width,height);
	*pmap=ImlibMoveImageToPixmap(imd,im);
	*mask=ImlibMoveMaskToPixmap(imd,im);
	ImlibDestroyImage(imd,im);
}

void InitIcons() {

	Pixmap dummy;

	LoadImageWithImlib(icfg.left_arrow_im, &icfg.left_transparent, 
			&icfg.left_pmap, &icfg.left_mask, &icfg.left_w, &icfg.left_h);
	LoadImageWithImlib(icfg.left_sel_arrow_im, &icfg.left_sel_transparent, 
			&icfg.left_sel_pmap, &icfg.left_sel_mask, &icfg.left_sel_w, 
			&icfg.left_sel_h);
	LoadImageWithImlib(icfg.left_clk_arrow_im, &icfg.left_clk_transparent, 
			&icfg.left_clk_pmap, &icfg.left_clk_mask, &icfg.left_clk_w, 
			&icfg.left_clk_h);
	LoadImageWithImlib(icfg.right_arrow_im, &icfg.right_transparent, 
			&icfg.right_pmap, &icfg.right_mask, &icfg.right_w, &icfg.right_h);
	LoadImageWithImlib(icfg.right_sel_arrow_im, &icfg.right_sel_transparent, 
			&icfg.right_sel_pmap, &icfg.right_sel_mask, &icfg.right_sel_w, 
			&icfg.right_sel_h);
	LoadImageWithImlib(icfg.right_clk_arrow_im, &icfg.right_clk_transparent, 
			&icfg.right_clk_pmap, &icfg.right_clk_mask, &icfg.right_clk_w, 
			&icfg.right_clk_h);
	LoadImageSizeWithImlib(icfg.background_im, NULL, &icfg.bg_pmap, &dummy, 
			icfg.width, icfg.height);

	icfg.bg_win=CreateBasicWin(root,icfg.x,icfg.y,icfg.width,icfg.height);
	XSelectInput(disp,icfg.bg_win,SubstructureNotifyMask|ButtonPressMask|
			ButtonReleaseMask|EnterWindowMask|LeaveWindowMask|
			ButtonMotionMask|FocusChangeMask|ColormapChangeMask|
			PropertyChangeMask|SubstructureRedirectMask|
			PointerMotionMask);
	if (icfg.orientation) {
		icfg.left_win=CreateBasicWin(root,icfg.x+((icfg.width-icfg.left_w)/2),
				icfg.y-icfg.left_h,
				icfg.left_w,icfg.left_h);
		icfg.right_win=CreateBasicWin(root,
				icfg.x+((icfg.width-icfg.right_w)/2),icfg.y+icfg.height,
				icfg.right_w,icfg.right_h);
	} else {
		icfg.left_win=CreateBasicWin(root,icfg.x-icfg.left_w,
				icfg.y+((icfg.height-icfg.left_h)/2),
				icfg.left_w,icfg.left_h);
		icfg.right_win=CreateBasicWin(root,icfg.x+icfg.width,
				icfg.y+((icfg.height-icfg.right_h)/2),
				icfg.right_w,icfg.right_h);
	}
	XSelectInput(disp,icfg.left_win,LeaveWindowMask|
			EnterWindowMask|SubstructureNotifyMask|
			PropertyChangeMask|SubstructureRedirectMask);
	XSelectInput(disp,icfg.right_win,LeaveWindowMask|
			EnterWindowMask|SubstructureNotifyMask|
			PropertyChangeMask|SubstructureRedirectMask);
	XSetWindowBackgroundPixmap(disp,icfg.bg_win,icfg.bg_pmap);
	XSetWindowBackgroundPixmap(disp,icfg.left_win,icfg.left_pmap);
	XSetWindowBackgroundPixmap(disp,icfg.right_win,icfg.right_pmap);
	if (icfg.left_mask)
		XShapeCombineMask(disp,icfg.left_win,ShapeBounding,0,0,
				icfg.left_mask,ShapeSet);
	if (icfg.right_mask)
		XShapeCombineMask(disp,icfg.right_win,ShapeBounding,0,0,
				icfg.right_mask,ShapeSet);
}

Icon *GetIconWinID(Window w) {

	struct i_member *ptr;

	ptr=ilist.first;
	while(ptr) {
		if (ptr->icon->win==w) 
			return ptr->icon;
		ptr=ptr->next;
	}
	return NULL;
}

void AddIcon(Icon *ic) {
	struct i_member *ptr;
	struct i_member *pptr;
	int x,y;

	if (ilist.first) {
		ptr=ilist.first;
		x=ptr->icon->x;
		y=ptr->icon->y;
		while(ptr) {
			pptr=ptr;
			x+=ptr->icon->width+(icfg.bx*2);
			y+=ptr->icon->height+(icfg.by*2);
			ptr=ptr->next;
		}
		pptr->next=malloc(sizeof(struct i_member));
		pptr->next->next=NULL;
		pptr->next->icon=ic;
		ilist.last=pptr->next;
		if (icfg.orientation) {
			pptr->next->icon->x=icfg.bx;
			pptr->next->icon->y=y;
		} else {
			pptr->next->icon->x=x;
			pptr->next->icon->y=icfg.by;
		}
		XMoveWindow(disp,pptr->next->icon->win,pptr->next->icon->x,
				pptr->next->icon->y);
		XMapWindow(disp,pptr->next->icon->win);
		if (ic->mask) 
			XShapeCombineMask(disp,ic->win,ShapeBounding,0,0,ic->mask,
					ShapeSet);
	} else {
		ilist.first=malloc(sizeof(struct i_member));
		ilist.first->next=NULL;
		ilist.first->icon=ic;
		ilist.first->icon->y=icfg.by;
		ilist.first->icon->x=icfg.bx;
		ilist.last=ilist.first;
		XMoveWindow(disp,ilist.first->icon->win,ilist.first->icon->x,
				ilist.first->icon->y);
		XMapWindow(disp,ilist.first->icon->win);
		if (ic->mask) 
			XShapeCombineMask(disp,ic->win,ShapeBounding,0,0,ic->mask,
					ShapeSet);
	}
	XSync(disp,False);
}

void DelIcon(Window w) {
	struct i_member *ptr;
	struct i_member *pptr;
	int found;
	int wid;

	pptr=NULL;
	found=0;
	if (ilist.first) {
		ptr=ilist.first;
		while(ptr) {
			if (ptr->icon->win==w) {
				found=1;
				break;
			}
			pptr=ptr;
			ptr=ptr->next;
		}
		if (pptr) 
			pptr->next=ptr->next;
		else 
			ilist.first=ptr->next;
		if (icfg.orientation)
			wid=ptr->icon->height;
		else
			wid=ptr->icon->width;
		if (ptr->icon->win) 
			XDestroyWindow(disp,ptr->icon->win);
		if (ptr->icon->pmap) 
			ImlibFreePixmap(imd,ptr->icon->pmap);
		if (ptr->icon->mask) 
			ImlibFreePixmap(imd,ptr->icon->mask);
		ptr->icon->ewin->icon=NULL;
		free(ptr->icon);
		if (ilist.last==ptr) {
			if (pptr) 
				ilist.last=pptr;
			else 
				ilist.last=NULL;
		}
		free(ptr);
		if (pptr) 
			ptr=pptr->next;
		else 
			ptr=ilist.first;
		while(ptr) {
			if (icfg.orientation)
				ptr->icon->y-=wid+(icfg.by*2);
			else
				ptr->icon->x-=wid+(icfg.bx*2);
			XMoveWindow(disp,ptr->icon->win,ptr->icon->x,ptr->icon->y);
			XMapWindow(disp,ptr->icon->win);
			XSync(disp,False);
			pptr=ptr;
			ptr=ptr->next;
		}
	}
	XSync(disp,False);
}

void Msg_Iconify(EWin *ewin) {
	XSync(disp,False);
	ewin->state|=ICONIFIED;
	XSetInputFocus(disp,root,RevertToNone,CurrentTime);
	MimickEwin(ewin);
	XSync(disp,False);
}

void DeIconify(EWin *ewin) {

	Icon *ic;
	struct i_member *ptr;

	/* find the icon in the iconlist */
	ptr=ilist.first;
	while(ptr) {
		if(ptr->icon->ewin==ewin) break;
		ptr=ptr->next;
	}

	/* not in the list, get outta here */
	if(!ptr) return;

	ic = ptr->icon;

	/* restore the window */
	XMapWindow(disp,ewin->frame_win);
	if (fx.shadow.on) XMapWindow(disp,ewin->fx.shadow_win);
	Msg_DeIconify(ewin);
	DelIcon(ic->win);
	XSync(disp,False);

}

void Msg_DeIconify(EWin *ewin) {
	ewin->state&=~ICONIFIED;
	ewin->state&=~UNMAPPED;
	ewin->state|=MAPPED;
	ewin->desk=desk.current;
	MimickEwin(ewin);
	XSync(disp,False);
}

Icon *LoadIconFromDisk(EWin *ewin, char *IconInfo, ImColor *icl) {

	Icon *ic;
	Pixmap pmap;
	Pixmap mask;
	int w,h;

	ic=malloc(sizeof(Icon));
	LoadImageWithImlib(IconInfo,icl,&pmap,&mask,&w,&h);
	ic->ewin=ewin;
	ic->ewin->icon=ic;
	ic->win=CreateWin(icfg.bg_win,0,0,w,h);
	ic->width=w;
	ic->height=h;
	ic->pmap=pmap;
	ic->mask=mask;
	ic->x=0;
	ic->y=0;
	XSetWindowBackgroundPixmap(disp,ic->win,ic->pmap);

	return ic;
}

Icon *PredefinedIconLoad(EWin *ewin, char *TitleBarContents) {

	char *IconInfo;
	ImColor *icl;

	IconInfo = ReturnIconPath(TitleBarContents,&icl);
	if (!IconInfo) 
		return NULL;
	if (IconInfo[0]) {
		return LoadIconFromDisk(ewin,IconInfo,icl);
	} 
	return NULL;
}

char *ReturnIconPath(char *TitleBarContents, ImColor **icl) {
/*   
 * this function should parse through the predefined icon configuration
 * and determine if the window has a predefined icon associated with it
 * NOTE: consider adding a PreDefIconPath to struct Ewin - next release?
 * This function returns the line number containing information about
 * the particular window's predefined icon
 */
	int i;

	for(i=0;i<predef_num;i++) {
		if (matchregexp(predef_list[i].title,TitleBarContents)) {
			*icl=predef_list[i].transp;
			return predef_list[i].image_file;
		}
	}
	return NULL;
}

void Finish_Iconify() {

	Icon *ic;
   
	if (!CheckClientExists(newicon.ewin)) {
		ListDelWinID(global_l,newicon.ewin->frame_win);
		XSetInputFocus(disp,root,RevertToNone,CurrentTime);
		XSync(disp,False);
		return;
	}
	ic=PredefinedIconLoad(newicon.ewin,newicon.ewin->title);
	if(!ic) 
		ic=GrabClient(newicon.ewin);
	AddIcon(ic);
	XUnmapWindow(disp,newicon.ewin->frame_win);
	if (fx.shadow.on) 
		XUnmapWindow(disp,newicon.ewin->fx.shadow_win);
	Msg_Iconify(newicon.ewin);
	/* allow other iconifications ot go ahead */
	if (newicon.kill) 
		Do_KillWin(newicon.ewin, newicon.kill-1);
	newicon.ewin=NULL;
	newicon.win=0;
	newicon.kill=0;
	timer_mode = 0;
}

void MapIconBox() {
	XMapWindow(disp,icfg.bg_win);
	XMapWindow(disp,icfg.left_win);
	XMapWindow(disp,icfg.right_win);
	XSync(disp,False);
}

void IconLeft() {
	Window dummyw;
	int dummy;
	unsigned int btns;

	btns=1;
	while (btns) {
		IconsScroll(8);
		XQueryPointer(disp,root,&dummyw,&dummyw,&dummy,&dummy,&dummy,
				&dummy,&btns);
		usleep(icfg.scroll_speed*1000);
	}
}

void IconRight() {
	Window dummyw;
	int dummy;
	unsigned int btns;

	btns=1;
	while (btns) {
		IconsScroll(-8);
		XQueryPointer(disp,root,&dummyw,&dummyw,&dummy,&dummy,&dummy,&dummy,
				&btns);
		usleep(icfg.scroll_speed*1000);
	}
}

void IconsScroll(int x) {
	struct i_member *ptr;
	unsigned char ok=0;

	ptr=ilist.first;
	if (ptr) {
		if (icfg.orientation) {
			if (x<0) {
				if (ilist.last->icon->y>
						(icfg.height-icfg.by-ilist.last->icon->height+x)) 
					ok=1;
			}
			if (x>0) {
				if (ilist.first->icon->y<(icfg.by+x)) 
					ok=1;
			}
		} else {
			if (x<0) {
				if (ilist.last->icon->x>
						(icfg.width-icfg.bx-ilist.last->icon->width+x)) 
					ok=1;
			}
			if (x>0) {
				if (ilist.first->icon->x<(icfg.bx+x)) 
					ok=1;
			}
		}
	}
	if (!ok) 
		return;
	while(ptr) {
		if (icfg.orientation)
			ptr->icon->y+=x;
		else
			ptr->icon->x+=x;
		XMoveWindow(disp,ptr->icon->win,ptr->icon->x,ptr->icon->y);
		ptr=ptr->next;
	}
	XSync(disp,False);
}

void RaiseLowerIcons() {
	if (icfg.level) {
		XRaiseWindow(disp,icfg.bg_win);
		XRaiseWindow(disp,icfg.left_win);
		XRaiseWindow(disp,icfg.right_win);
		XSync(disp,False);
	} else {
		XLowerWindow(disp,icfg.bg_win);
		XLowerWindow(disp,icfg.left_win);
		XLowerWindow(disp,icfg.right_win);
		XSync(disp,False);
	}
}
