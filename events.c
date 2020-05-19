#include "enlightenment.h"

XErrorHandler handleError(Display *d, XErrorEvent *ev) {
/*
 *  printf("X error trapped\n");
 *  printf("+----------------------------------------------------------\n");
 *  printf("| serial        : %x\n",ev->serial);
 *  printf("| error code    : %x\n",ev->error_code);
 *  printf("| request code  : %x\n",ev->request_code);
 *  printf("| minor code    : %x\n",ev->minor_code);
 *  printf("| resource id   : %x\n",ev->resourceid);
 *  printf("+----------------------------------------------------------\n");
 */
	if  ((ev->request_code == X_ChangeWindowAttributes) && (ev->error_code   == BadAccess)&&(imd==NULL)) {
		fprintf(stderr,"Another window manager is already running. Sorry.. No go mate.");
		Alert("Another window manager is already running. Sorry.. No go mate.");
		EExit(1);
	}
	return 0;
}

XIOErrorHandler handleIOError(Display *d) {
	printf("X IO Fatal Error.\n");
	disp=NULL;
	EExit(99);
	return 0;
}

void handleEvent(XEvent *ev,listhead *l) {
	if(FocusOut > ev->type) {
		switch (ev->type) {
			case KeyPress:
				handleKeyDown(ev,l);
				break;
			case KeyRelease:
				handleKeyUp(ev,l);
				break;
			case ButtonPress:
				handleButtonDown(ev,l);
				break;
			case ButtonRelease:
				handleButtonUp(ev,l);
				break;
			case MotionNotify:
				handleMotion(ev,l);
				break;
			case EnterNotify:
				handleEnter(ev,l);
				break;
			case LeaveNotify:
				handleLeave(ev,l);
				break;
			case FocusIn:
				handleFocusIn(ev,l);
				break;
			default: 
				break;
		}
	} else {
		switch(ev->type) {
			case FocusOut:
				handleFocusOut(ev,l);
				break;
			case DestroyNotify:
				handleDestroy(ev,l);
				break;
			case UnmapNotify:
				handleUnmap(ev,l);
				break;
			case MapRequest:
				handleMap(ev,l);
				break;
			case ConfigureRequest:
				handleConfigure(ev,l);
				break;
			case ResizeRequest:
				handleResize(ev,l);
				break;
			case CirculateRequest:
				handleCirculate(ev,l);
				break;
			case PropertyNotify:
				handleProperty(ev,l);
				break;
			case ClientMessage:
				handleClientMessage(ev, l);
				break;
			default: 
				break;
		}
	} 
}

void handleKeyDown(XEvent *ev,listhead *l) {

	char buffer[256];
	int bufsize=255;
	int count;
	KeySym keysym;
	KeySym keysym2;
	XComposeStatus compose;
	int i;
	int x,y,d;
	Window wd;
	int modifier;
	unsigned int mask;

   /* Convert our lovely KeyPress into a nice convenient hex string
    * that we can look up versus the <X11/keysym.h> definitions
    */
	XQueryPointer(disp,root,&wd,&wd,&x,&y,&d,&d,&mask);
	if      (mask==1) modifier=1;
	else if (mask==4) modifier=2;
	else if (mask==8) modifier=3;
	else             modifier=0;
	count = XLookupString((void *) ev,buffer,bufsize,&keysym,&compose);
	buffer[count]=0; /* make sure string terminates */
   /* Then we'll test to see if our key combo was selected */
	for(i=0;i<KeyBind_num;i++) {
		keysym2=XStringToKeysym(KeyBind_list[i].keycombo);
		if ((keysym2==keysym)&&(modifier==KeyBind_list[i].modifier))
			RunContents(KeyBind_list[i].action_type,KeyBind_list[i].execstring);
	}
}

void handleKeyUp(XEvent *ev,listhead *l)
{
}

void handleButtonDown(XEvent *ev,listhead *l) {
	Window tmp_win;
	Window wwin,dwin;
	int dummy;
	unsigned int dummyui;
	int button;
	int modifier;
	int win_button;
	int dsk;
	EWin *ewin;
	BWin *bwin;
	Icon *icon;

	tmp_win=ev->xbutton.subwindow;
	button=ev->xbutton.button;
	modifier=ev->xbutton.state;
	XQueryPointer(disp,root,&dwin,&wwin,&dummy,&dummy,&dummy,&dummy,&dummyui);
	if (modifier==0) modifier=0;
	else if (modifier==1) modifier=1;
	else if (modifier==4) modifier=2;
	else if (modifier==8) modifier=3;
	else modifier=0;
	if (cfg.focus_policy==ClickToFocus) {
		FocWin(wwin,1);
		XSync(disp,False);
	}
	if(cfg.click_raise) {
		ewin=ListGetWinID(global_l,wwin);
		if(ewin) {
			Do_RaiseWin(ewin);
			XSendEvent(disp,ewin->client_win,False,ButtonPressMask,ev);
		}
	}
	if (wwin==0) {
		evmd.mode=MODE_MENU;
		tmp_menu=NULL;
		DoRootMenu(button,modifier,ev->xbutton.x,ev->xbutton.y);
	} else if ((dsk=WindowIsDeskInfo(wwin))>=0) {
		XQueryPointer(disp,wwin,&dwin,&wwin,&dummy,&dummy,&dummy,&dummy,&dummyui);
		ewin=WindowIsDeskInfoWin(wwin);
		if (ewin)
			StartIconDrag(ewin);
		else
			ChangeToDesk(dsk);
	} else {
		ewin=ListGetSubWinID(l,tmp_win);
		if (ewin) {
			win_button=GetEWinButtonID(ewin,tmp_win);
			evmd.px=ev->xbutton.x_root;
			evmd.py=ev->xbutton.y_root;
			DoWindowButton(ewin,tmp_win,button,modifier,win_button);
		} else {
			bwin=GetButtonWinID(tmp_win);
			if (bwin) {
				bwin->state=BTN_CLICK;
				btmd.bwin=bwin;
				DoButton(bwin,button,modifier);
			} else {
				icon=GetIconWinID(tmp_win);
				if(icon)
					DoIcon(icon,button,modifier);
			}
		}
	}
	if (tmp_win==icfg.left_win) {
		XSetWindowBackgroundPixmap(disp,icfg.left_win,icfg.left_clk_pmap);
		if (icfg.left_clk_mask)
			XShapeCombineMask(disp,icfg.left_win,ShapeBounding,0,0,icfg.left_clk_mask,ShapeSet);
		sel_win=icfg.left_win;
		XClearWindow(disp,icfg.left_win);
		XSync(disp,False);
		IconLeft();
	} else if (tmp_win==icfg.right_win) {
		XSetWindowBackgroundPixmap(disp,icfg.right_win,icfg.right_clk_pmap);
		if (icfg.right_clk_mask)
			XShapeCombineMask(disp,icfg.right_win,ShapeBounding,0,0,icfg.right_clk_mask,ShapeSet);
		sel_win=icfg.right_win;
		XClearWindow(disp,icfg.right_win);
		XSync(disp,False);
		IconRight();
	}
}

void handleButtonUp(XEvent *ev,listhead *l)
{
   /* Menu *m; */
   
	if (evmd.ewin) evmd.ewin->subwin_state[evmd.wbtn]=NORM;
	if (evmd.mode==MODE_MENU) {
		if ((tmp_menu)&&(tmp_menu->sel_item>=0))
			DoMenu(tmp_menu,tmp_menu->sel_item);
		DeleteToActiveMenu("");
		tmp_menu=NULL;
		evmd.mname[0]=0;
	}
	if ((evmd.wbtn>=0)&&(evmd.ewin)) DrawButton(evmd.ewin,evmd.wbtn);
	if (btmd.bwin) {
		btmd.bwin->state=BTN_SEL;
		ButtonDraw(btmd.bwin);
		btmd.bwin=NULL;
	}
	if (sel_win==icfg.left_win) {
		XSetWindowBackgroundPixmap(disp,icfg.left_win,icfg.left_sel_pmap);
		if (icfg.left_sel_mask)
			XShapeCombineMask(disp,icfg.left_win,ShapeBounding,0,0,icfg.left_sel_mask,ShapeSet);
		sel_win=0;
		XClearWindow(disp,icfg.left_win);
		XSync(disp,False);
	} else if (sel_win==icfg.right_win) {
		XSetWindowBackgroundPixmap(disp,icfg.right_win,icfg.right_sel_pmap);
		if (icfg.right_sel_mask)
			XShapeCombineMask(disp,icfg.right_win,ShapeBounding,0,0,icfg.right_sel_mask,ShapeSet);
		sel_win=0;
		XClearWindow(disp,icfg.right_win);
		XSync(disp,False);
	}
	if(evmd.mode==MODE_MOVE) {
		Draw_Cursor(evmd.ewin, evmd.x1, evmd.y1, '+', 2);
		XUngrabPointer(disp, CurrentTime);
		ifb.nodo=0;
	} else if(evmd.mode==MODE_RESIZE) {
		Draw_Cursor(evmd.ewin, evmd.x2, evmd.y2, 'x', 2);
		XUngrabPointer(disp, CurrentTime);
		ifb.nodo=0;
	}
	evmd.mode=MODE_NORMAL;
	evmd.ewin=NULL;
}

void handleMotion(XEvent *ev,listhead *l) {
	int sx,sy;
	Window w1,w2;
	int x,y,xx,yy;
	unsigned int d;
	int wx,wy;
	Window dummyw,focwin;
	int dummy;
	unsigned int dummyui;

   /* 07/19/97 Mandrake (mandrake@mandrake.net - http://mandrake.net)
     * I cleaned up this function quite a bit
    */

	XQueryPointer(disp,root,&dummyw,&focwin,&mouse_x,&mouse_y,&dummy,&dummy,&dummyui);
	switch(evmd.mode) {
		case MODE_NORMAL:
			if(cfg.focus_policy!=ClickToFocus) 
				if (FocusWin != focwin)
					FocWin(focwin,0);
			return;
		case MODE_MOVE:
			/* if (XCheckTypedEvent(disp,MotionNotify,&xev)) return; */
			XQueryPointer(disp,root,&w1,&w2,&sx,&sy,&xx,&yy,&d);
			x=evmd.ewin->frame_x+sx-evmd.px;
			y=evmd.ewin->frame_y+sy-evmd.py;
			Draw_Cursor(evmd.ewin, x, y, '+', 0);
			ModifyEWin(evmd.ewin,x,y,evmd.ewin->client_width,evmd.ewin->client_height);
			evmd.px=sx;
			evmd.py=sy; 
			break;
		case MODE_RESIZE:
			/* if (XCheckTypedEvent(disp,MotionNotify,&xev)) return; */
			XQueryPointer(disp,root,&w1,&w2,&sx,&sy,&xx,&yy,&d);
			wx=sx-evmd.px;
			while ((wx%evmd.ewin->sizeinc_x)>0) wx--;
			while ((wx%evmd.ewin->sizeinc_x)<0) wx++;
			wy=sy-evmd.py;
			while ((wy%evmd.ewin->sizeinc_y)>0) wy--;
			while ((wy%evmd.ewin->sizeinc_y)<0) wy++;
			if ((wx!=0)||(wy!=0)) {
				switch(evmd.resize_mode) {
					case 0:
						if((wx>0)&&(evmd.x2<wx)) wx=0;
						if((wy>0)&&(evmd.y2<wy)) wy=0;
						evmd.x1+=wx;
						evmd.y1+=wy;
						evmd.x2-=wx;
						evmd.y2-=wy;
						break;
					case 1:
						if((wx<0)&&(evmd.x2<-wx)) wx=0;
						if((wy>0)&&(evmd.y2<wy)) wy=0;
						evmd.y1+=wy;
						evmd.x2+=wx;
						evmd.y2-=wy;
						break;
					case 2:
						if ((wx>0)&&(evmd.x2<wx)) wx=0;
						if ((wy<0)&&(evmd.y2<-wy)) wy=0;
						evmd.x1+=wx;
						evmd.x2-=wx;
						evmd.y2+=wy;
						break;
					case 3:
						if ((wx<0)&&(evmd.x2<-wx)) wx=0;
						if ((wy<0)&&(evmd.y2<-wy)) wy=0;
						evmd.x2+=wx;
						evmd.y2+=wy;
						break;
				}
				if (evmd.x2>evmd.ewin->max_width) evmd.x2=evmd.ewin->max_width;
				if (evmd.x2<evmd.ewin->min_width) evmd.x2=evmd.ewin->min_width;
				if (evmd.y2>evmd.ewin->max_height) evmd.y2=evmd.ewin->max_height;
				if (evmd.y2<evmd.ewin->min_height) evmd.y2=evmd.ewin->min_height;
				ModifyEWin(evmd.ewin,evmd.x1,evmd.y1,evmd.x2,evmd.y2);
				Draw_Cursor(evmd.ewin,(evmd.x2-evmd.ewin->base_width)/evmd.ewin->sizeinc_x,(evmd.y2-evmd.ewin->base_height)/evmd.ewin->sizeinc_y, '+', 0);
				evmd.px+=wx;
				evmd.py+=wy;
			}
			break;
		case MODE_MENU: 
			{
				Menu *m;
				Menu *mm;
				int i;
				int px,py;
				XWindowAttributes xwa;
				unsigned char selected=0;

				XQueryPointer(disp,root,&w1,&w2,&sx,&sy,&xx,&yy,&d);
				m=GetMenuWin(w2);
				if (m) {
					tmp_menu=m;
					XQueryPointer(disp,w2,&w1,&w2,&sx,&sy,&xx,&yy,&d);
					for (i=0;i<m->num_items;i++) {
						if (m->items[i]->win==w2) {
							selected=1;
							if (m->sel_item!=i) {
								if (m->sel_item>=0) 
									DrawMenuItem(m,m->sel_item,0);
								DrawMenuItem(m,i,1);
								m->sel_item=i;
								i=m->num_items;
								DeleteToActiveMenu(m->name);
								if (m->items[m->sel_item]->action.id==12) {
									mm=FindMenu(m->items[m->sel_item]->action.params);
									if (mm) {
										XGetWindowAttributes(disp,m->win,&xwa);
										px=xwa.x+m->items[m->sel_item]->x+(m->items[m->sel_item]->width/2);
										py=xwa.y+m->items[m->sel_item]->y+(m->items[m->sel_item]->height/2);
										DoBaseMenu(mm,px,py);
										tmp_menu=mm;
									}
								}
							}
						}
					}
				}
				if ((!selected)&&(tmp_menu)&&(tmp_menu->sel_item>=0)) {
					DrawMenuItem(tmp_menu,tmp_menu->sel_item,0);
					tmp_menu->sel_item=-1;
				}
			}
			break;
		default:
			if ((tmp_menu)&&(tmp_menu->sel_item>=0)) {
				DrawMenuItem(tmp_menu,tmp_menu->sel_item,0);
				tmp_menu->sel_item=-1;
				tmp_menu=NULL;
			}
			break;
	}
}

void handleEnter(XEvent *ev,listhead *l) {
	Window tmp_win;
	Window dummyw,focwin;
	int dummyi;
	unsigned int dummyui;
	if (evmd.mode!=MODE_NORMAL) return;
	tmp_win=ev->xcrossing.window;
	XQueryPointer(disp,root,&dummyw,&focwin,&dummyi,&dummyi,&dummyi,&dummyi,&dummyui);
	if (tmp_win!=focwin) tmp_win=focwin;
	FocWin(tmp_win,0);
}

void handleLeave(XEvent *ev,listhead *l)
{
}

void handleFocusIn(XEvent *ev,listhead *l)
{
}

void handleFocusOut(XEvent *ev,listhead *l)
{
}

void handleDestroy(XEvent *ev,listhead *l) {
	Window tmp_win;
	EWin *ewin;

	tmp_win=ev->xdestroywindow.window;
	ewin=ListGetClientWinID(l,tmp_win);
	if (ewin) /* if it was a client */
		ListDelWinID(l,ewin->frame_win); /* get rid of the ewin */
	if ((evmd.mode==MODE_MOVE)||(evmd.mode==MODE_RESIZE)) {
		Draw_Cursor(evmd.ewin, evmd.x1, evmd.y1, '+', 2);
		XUngrabPointer(disp, CurrentTime);
	}
	if (evmd.mode!=MODE_MENU) {
		evmd.mode=MODE_NORMAL;
		evmd.ewin=NULL;
	}
/*   XSetInputFocus(disp,root,RevertToNone,CurrentTime);*/
	XSync(disp,False);
}

void handleUnmap(XEvent *ev,listhead *l) {
	Window tmp_win;
	EWin *ewin;
	tmp_win=ev->xunmap.event;
	if ((ewin=ListGetWinID(l,tmp_win))&& (ewin->state&MAPPED)) {
		/* if its a client and the frame was mapped then */ 
		XUnmapWindow(disp,ewin->client_win); /* unmap the frame */
		XUnmapWindow(disp,ewin->frame_win); /* unmap the frame */
		if (fx.shadow.on) XUnmapWindow(disp,ewin->fx.shadow_win);
		ewin->state|=UNMAPPED;
		ewin->state&=~MAPPED;
		XReparentWindow(disp,ewin->client_win,root,ewin->frame_x,ewin->frame_y);
		XRemoveFromSaveSet(disp,ewin->client_win);
		ListDelWinID(l,ewin->frame_win);
		if ((evmd.mode==MODE_MOVE)||(evmd.mode==MODE_RESIZE)) {
			Draw_Cursor(evmd.ewin, evmd.x1, evmd.y1, '+', 2);
			XUngrabPointer(disp, CurrentTime);
		}
		if (evmd.mode!=MODE_MENU) {
			evmd.mode=MODE_NORMAL;
			evmd.ewin=NULL;
		}
/*	if (XCheckTypedWindowEvent(disp,ev->xunmap.event,DestroyNotify,&xev))
 *	  {
 *	     XDestroyWindow(disp,ewin->client_win);
 *	  }
 */
	}
}

void handleMap(XEvent *ev,listhead *l) {
	Window tmp_win;
	EWin *ewin;
	unsigned long c[2];
	tmp_win=ev->xmap.window;
	if (ListGetWinID(l,tmp_win)) return;
	if (GetButtonWinID(tmp_win)) return;
	if (tmp_win==icfg.bg_win) return;
	if (tmp_win==icfg.left_win) return;
	if (tmp_win==icfg.right_win) return;
	if (!(ewin=ListGetClientWinID(l,tmp_win))) {
		ewin=InitEWin(tmp_win); 
		if (cfg.focus_policy==ClickToFocus) {
			FocWin(tmp_win,1);
			XSync(disp,False);
		}
		ListAdd(l,ewin);
		ewin->state&=~ICONIFIED;
	}
	XRaiseWindow(disp,ewin->frame_win);
	XMapSubwindows(disp,ewin->frame_win);
	if (ewin->desk>=0) ewin->desk=desk.current;
	if (fx.shadow.on) XMapWindow(disp,ewin->fx.shadow_win);
	if (!(ewin->state&ICONIFIED)) {
		XMapWindow(disp,ewin->frame_win);
		if (fx.shadow.on) XMapWindow(disp,ewin->fx.shadow_win);
		ewin->state&=~UNMAPPED;
		ewin->state|=MAPPED;
	} else {
		XMapWindow(disp,ewin->frame_win);
		if (fx.shadow.on) XMapWindow(disp,ewin->fx.shadow_win);
		ewin->state&=~UNMAPPED;
		ewin->state|=MAPPED;
	}
/*   RaiseLowerButtons();*/
/*   RaiseLowerIcons();*/
	c[0]=NormalState;
	c[1]=0;
	XChangeProperty(disp, ewin->client_win, WM_STATE, WM_STATE, 32, PropModeReplace, (unsigned char *) c, 2);
	if (states) ConformEwinToState(ewin);
	XSync(disp,False);
}

void handleReparent(XEvent *ev,listhead *l) {
	Window tmp_win;
	EWin *ewin;

	tmp_win=ev->xreparent.window;
	if (ListGetClientWinID(l,tmp_win)) {
		if (!(ewin=ListGetWinID(l,ev->xreparent.parent))) 
			ListDelWinID(l,ewin->frame_win); 
	}
}

void handleConfigure(XEvent *ev,listhead *l) {
	Window tmp_win;
	EWin *ewin;
	int x,y,w,h;
	XWindowChanges val;
	int mask;
	tmp_win=ev->xconfigurerequest.window;
	ewin=ListGetClientWinID(l,tmp_win);
	if (ewin) {	
		x=ewin->frame_x;
		y=ewin->frame_y;
		w=ewin->client_width;
		h=ewin->client_height;
		if (ev->xconfigurerequest.value_mask&CWX) 
			x=ev->xconfigurerequest.x;
		if (ev->xconfigurerequest.value_mask&CWY) 
			y=ev->xconfigurerequest.y;
		if (ev->xconfigurerequest.value_mask&CWWidth) 
			w=ev->xconfigurerequest.width;
		if (ev->xconfigurerequest.value_mask&CWHeight) 
			h=ev->xconfigurerequest.height;
		GetWinName(tmp_win,ewin->title);
		ewin->changes|=MOD_TITLE;
		GetWinColors(tmp_win,ewin);
		ModifyEWin(ewin,x,y,w,h);
	} else {
		val.x=ev->xconfigurerequest.x;
		val.y=ev->xconfigurerequest.y;
		val.width=ev->xconfigurerequest.width;
		val.height=ev->xconfigurerequest.height;
		mask=0;
		if (ev->xconfigurerequest.value_mask&CWX) 
			mask|=CWX;
		if (ev->xconfigurerequest.value_mask&CWY) 
			mask|=CWY;
		if (ev->xconfigurerequest.value_mask&CWWidth) 
			mask|=CWWidth;
		if (ev->xconfigurerequest.value_mask&CWHeight) 
			mask|=CWHeight;
		XConfigureWindow(disp,tmp_win,mask,&val);
	}
	RaiseLowerButtons();
}

void handleClientMessage(XEvent *ev, listhead *l) {
	Window tmp_win;
	EWin *ewin;

	tmp_win=ev->xclient.window;
	ewin=ListGetClientWinID(l,tmp_win);
	if(ewin)
		if (ev->xclient.data.l[0]==IconicState && !ewin->state&ICONIFIED)
			Do_IconifyWin(ewin);
}

void handleResize(XEvent *ev,listhead *l) {
	Window tmp_win;
	EWin *ewin;
	int x,y,w,h;

	tmp_win=ev->xresizerequest.window;
	ewin=ListGetClientWinID(l,tmp_win);
	if (ewin) {	
		x=ewin->frame_x;
		y=ewin->frame_y;
		w=ev->xresizerequest.width;
		h=ev->xresizerequest.height;
		GetWinName(tmp_win,ewin->title);
		ewin->changes|=MOD_TITLE;
		GetWinColors(tmp_win,ewin);
		ModifyEWin(ewin,x,y,w,h);
	} else {
		w=ev->xresizerequest.width;
		h=ev->xresizerequest.height;
		XResizeWindow(disp,tmp_win,w,h);
	}
	RaiseLowerButtons();
}

void handleCirculate(XEvent *ev,listhead *l)
{
}

void handleProperty(XEvent *ev,listhead *l) {
	EWin *ewin;
	Window tmp_win;
	int state;

	tmp_win=ev->xproperty.window;

	ewin=ListGetClientWinID(l,tmp_win);
	if (ewin) {
		switch (ev->xproperty.atom) {
			case XA_WM_NAME:
				GetWinName(tmp_win,ewin->title);
				ewin->changes|=MOD_TITLE;
				DrawWindowBorder(ewin);
				break;
			/* case XA_WM_ICON_NAME:
				break; */
			case XA_WM_HINTS:
				state=ewin->state;
				GetWinSize(tmp_win,ewin);
				if (state!=ewin->state) 
					ewin->changes|=MOD_STATE;
				DrawWindowBorder(ewin);
				break;
			case XA_WM_NORMAL_HINTS:
				state=ewin->state;
				GetWinSize(tmp_win,ewin);
				if (state!=ewin->state) 
					ewin->changes|=MOD_STATE;
				DrawWindowBorder(ewin);
				break;
			default:
				break;
		}
	}
}

void AutoRaise() {
	EWin *ewin;

	if (FocusWin==raisewin) {
		ewin=ListGetWinID(global_l,raisewin);
		if (ewin) {
			Do_RaiseWin(ewin);
			RaiseLowerButtons();
			XSync(disp,False);
		}
	}
	ifb.nodo=0;
	raisewin=0;
	timer_mode = 0;
}

void FocWin(Window tmp_win, int clk) {
	EWin *ewin;
	EWin *ewin2;
	BWin *bwin;
	struct itimerval tv1,tv2;

	if (cfg.focus_policy!=ClickToFocus) 
		clk=1;
	raisewin=0;
	if (FocusWin) /* if there was a previous focus win */ {
		ewin=ListGetWinID(global_l,FocusWin);
		if (ewin) /* if it exists */ {
			if (FocusWin!=tmp_win) {
				ewin2=ListGetWinID(global_l,tmp_win);
				if ((ewin2)||(cfg.focus_policy!=SloppyFocus)) {
					/* if the new focus isnt the current one */
					if (!((tmp_win==0)&&(cfg.focus_policy==SloppyFocus))) {
						if (clk) {
							GrabTheButtons(ewin->client_win);
							ewin->state&=~SELECTED; /* unfocus it */
							ewin->changes|=MOD_SELECT;
							DrawWindowBorder(ewin); /* redraw the window */
							XSync(disp,False);
							MimickEwin(ewin);
						}
					}
				}
			}
		}
	}
	if (FocusWin2) {
		if (FocusWin2!=tmp_win) {
			bwin=GetButtonWinID(FocusWin2);
			if (bwin) {
				if (bwin->state==BTN_SEL) {
					bwin->state=BTN_NORM;
					ButtonDraw(bwin);
				}
			} else if (FocusWin2==icfg.left_win) {
				XSetWindowBackgroundPixmap(disp,icfg.left_win,icfg.left_pmap);
				if (icfg.left_mask)
					XShapeCombineMask(disp,icfg.left_win,ShapeBounding,0,0,icfg.left_mask,ShapeSet);
				XClearWindow(disp,icfg.left_win);
				XSync(disp,False);
			} else if (FocusWin2==icfg.right_win) {
				XSetWindowBackgroundPixmap(disp,icfg.right_win,icfg.right_pmap);
				if (icfg.right_mask)
					XShapeCombineMask(disp,icfg.right_win,
							ShapeBounding,0,0,icfg.right_mask,ShapeSet);
				XClearWindow(disp,icfg.right_win);
				XSync(disp,False);
			}
			FocusWin2=0;
		}
	}
	if ((tmp_win==0)&&(cfg.focus_policy==SloppyFocus)) {
		FocusWin2=0; /* change the focus to the new window */
		return;
	}
	if ((tmp_win==0)&&(cfg.focus_policy!=ClickToFocus)) {
		/* if the new focus win is root */
		FocusWin2=0; /* change the focus to the new window */
		FocusWin=0; /* nullify the focus... so all windows */
		XInstallColormap(disp,root_cmap);		  
		XSetInputFocus(disp,root,RevertToNone,CurrentTime);
		return; /* are unfocused..... */
	}

	ewin=ListGetWinID(global_l,tmp_win);
	if (ewin) /* if it exists */ {
		if (clk) {
			FocusWin=tmp_win; /* change the focus to the new window */
			ewin->state|=SELECTED; /* focus it */
			if (ewin->colormap) 
				XInstallColormap(disp,ewin->colormap);
			else 
				XInstallColormap(disp,root_cmap);
			ewin->changes|=MOD_SELECT;
			UnGrabTheButtons(ewin->client_win);
			XSetInputFocus(disp,ewin->client_win,RevertToNone,CurrentTime);
			DrawWindowBorder(ewin); /* redraw the window */
			XSync(disp,False);
			MimickEwin(ewin);
			if (cfg.autoraise) {
				struct sigaction sa;

				ifb.nodo=1;
				if (timer_mode < TIMER_AUTORAISE) {
					for (getitimer(ITIMER_REAL, &tv1); 
						tv1.it_value.tv_sec != 0 || tv1.it_value.tv_usec != 0 
						|| tv1.it_interval.tv_sec != 0 || 
						tv1.it_interval.tv_usec != 0;
					   	getitimer(ITIMER_REAL, &tv1));
				}
				timer_mode = TIMER_AUTORAISE;
				sa.sa_handler = AutoRaise;
				sa.sa_flags = 0;
				sigemptyset (&sa.sa_mask);
				tv1.it_value.tv_sec=(unsigned long)(cfg.autoraise_time*1000 / 1000000);
				tv1.it_value.tv_usec=(long)(cfg.autoraise_time*1000 % 1000000);
				tv2.it_value.tv_sec=(unsigned long)(cfg.autoraise_time*1000 / 1000000);
				tv2.it_value.tv_usec=(long)(cfg.autoraise_time*1000 % 1000000);
				tv1.it_interval.tv_sec=0;
				tv1.it_interval.tv_usec=0;
				tv2.it_interval.tv_sec=0;
				tv2.it_interval.tv_usec=0;
				raisewin=FocusWin;
				setitimer(ITIMER_REAL,&tv1,&tv2);
				sigaction(SIGALRM,&sa,(struct sigaction *)0);
			}
		}
	} else {
		bwin=GetButtonWinID(tmp_win);
		if (bwin) {
			FocusWin2=tmp_win; /* change the focus to the new window */
			if (bwin->state==BTN_NORM) {
				bwin->state=BTN_SEL;
				ButtonDraw(bwin);
			}
		} else if (tmp_win==icfg.left_win) {
			FocusWin2=tmp_win; /* change the focus to the new window */
			XSetWindowBackgroundPixmap(disp,icfg.left_win,icfg.left_sel_pmap);
			if (icfg.left_sel_mask)
				XShapeCombineMask(disp,icfg.left_win,ShapeBounding,0,0,icfg.left_sel_mask,ShapeSet);
			XClearWindow(disp,icfg.left_win);
			XSync(disp,False);
		} else if (tmp_win==icfg.right_win) {
			FocusWin2=tmp_win; /* change the focus to the new window */
			XSetWindowBackgroundPixmap(disp,icfg.right_win,icfg.right_sel_pmap);
			if (icfg.right_sel_mask)
				XShapeCombineMask(disp,icfg.right_win,ShapeBounding,0,0,icfg.right_sel_mask,ShapeSet);
			XClearWindow(disp,icfg.right_win);
			XSync(disp,False);
		}
	}
}
