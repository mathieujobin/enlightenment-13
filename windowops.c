#include "enlightenment.h"

/* This file is for all of the code pertaining to windowops
 * file created 08/04/97 by mandrake
 */

int ParamToInt(char *params) {

	int i;
	char s[5];

	for(i=0;i<ACTION_NONE;i++) {
		sprintf(s,"%d",i);
		if(!strcmp(s,params)) return i;
	}
	return 0;

}

void DoWindowButton(EWin *ewin, Window win, int btn, int mod, int wbtn) {
   /* int i,j; */
   
   /* if it wasnt a left, middle or right mouse button... ignore it */
   if ((btn<1)||(btn>3)) return;
   /* if the subwindow clicked is a decoration subwindow.. ignore the click */
   if (cfg.subwin_type[wbtn]==0) return;
   if (cfg.subwin_action[wbtn][btn-1][mod].id!=ACTION_DRAG) {
   	/* flag the button as clicked ... */
   	ewin->subwin_state[wbtn]=CLICKED;
   	/* redraw the button */
   	DrawButton(ewin,wbtn);
   }
   /* set the event mode windowbutton pressed to this one */
   evmd.wbtn=wbtn;
   if(cfg.subwin_action[wbtn][btn-1][mod].id<ACTION_CONFIGURE)
   switch(cfg.subwin_action[wbtn][btn-1][mod].id)
       {
	case ACTION_MOVE:
		Do_RaiseWin(ewin);
		Do_MoveWin(ewin, win);
		return;
	case ACTION_RESIZE:
		Do_RaiseWin(ewin);
		Do_ResizeWin(ewin, win);
		return;
	case ACTION_ICONIFY:
		Do_IconifyWin(ewin);	
		break;
	case ACTION_KILL:
		Do_KillWin(ewin,0);
		break;
	case ACTION_RAISE:
		Do_RaiseWin(ewin);
		break;
	case ACTION_LOWER:
		Do_LowerWin(ewin);
		break;
	case ACTION_RAISE_LOWER:
		Do_RaiseLowerWin(ewin);
		break;
	case ACTION_MAX_HEIGHT:
		Do_RaiseWin(ewin);
		Do_MaxHeightWin(ewin);
		break;
	case ACTION_MAX_WIDTH:
		Do_RaiseWin(ewin);
		Do_MaxWidthWin(ewin);
		break;
	case ACTION_MAX_SIZE:
		Do_RaiseWin(ewin);
		Do_MaxSizeWin(ewin);
		break;
	default: break;
       }
 else
 switch(cfg.subwin_action[wbtn][btn-1][mod].id)
       {
	case ACTION_CONFIGURE:
		Do_Configure();
		break;
	case ACTION_MENU:
		Do_MenuWin(ewin);
		break;
	case ACTION_EXEC:
		Do_Exec(cfg.subwin_action[wbtn][btn-1][mod].params);
		break;
	case ACTION_KILL_NASTY:
		Do_KillWin(ewin,1);
		break;
	case ACTION_STICKY:
		ewin->desk = (ewin->desk<0) ? desk.current : -1;
		break;
	case ACTION_DRAG:
		StartIconDrag(ewin);
		break;
	case ACTION_WINDOWOP:
		PerformWindowOp(ParamToInt(cfg.subwin_action[wbtn][btn-1][mod].params));
		break;
	case ACTION_SKIP:
		ewin->skip=!ewin->skip;
		break;
	case ACTION_SNAPSHOT:
		ModifyEwinState(ewin);
		break;
	case ACTION_UNSNAPSHOT:
		DeleteEwinState(ewin);
		break;
	case ACTION_SHADE:
		Do_RaiseWin(ewin);
		DoShade(ewin);
		break;
	default:
		break;
	}
   evmd.ewin=ewin;
   evmd.wbtn=wbtn;
   evmd.mode=MODE_NORMAL;
   return;
}

void Do_MoveWin(EWin *ewin, Window win) {

	if(evmd.mode!=MODE_NORMAL)
		return;
	evmd.ewin=ewin;
	ewin->snapshotok=0;
	if(timer_mode==0 && ifb.nodo==0) {
		ifb.nodo=1;
	} else if(ifb.moveresize > 0) {
		if (cfg.move_mode==2) {
			evmd.mode=MODE_MOVE;
			ifb.nodo=1;
			Draw_Cursor(ewin, ewin->frame_x, ewin->frame_y, '+', 1);
			XGrabPointer(disp, win, True, ButtonMotionMask|PointerMotionMask|ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
			return;
		} else {
			ifb.nodo=1;
			MoveLoop(ewin, win);
			ifb.nodo=0;
		}
	}
}

void Do_ResizeWin(EWin *ewin, Window win) {

	/* XEvent ev;
	int x,y;
	int sx,sy,d;
	Window w1,w2; */
	int wx,wy;
	int mode;
	int x1,y1,x2,y2;
   
/*   XQueryPointer(disp,root,&w1,&w2,&sx,&sy,&x,&y,&d);*/
/*   evmd.px=sx;*/
/*   evmd.py=sy;*/
	if(evmd.mode!=MODE_NORMAL) return;
	x1=ewin->frame_x;
	y1=ewin->frame_y;
	x2=ewin->client_width;
	y2=ewin->client_height;
	wx=evmd.px-x1;
	wy=evmd.py-y1;
	if ((wx<ewin->frame_width/2)&&(wy<ewin->frame_height/2)) mode=0;
	if((wx>=ewin->frame_width/2)&&(wy<ewin->frame_height/2)) mode=1;
	if ((wx<ewin->frame_width/2)&&(wy>=ewin->frame_height/2)) mode=2;
	if ((wx>=ewin->frame_width/2)&&(wy>=ewin->frame_height/2)) mode=3;
	evmd.x1=x1;
	evmd.x2=x2;
	evmd.y1=y1;
	evmd.y2=y2;
	evmd.resize_mode=mode;
	evmd.ewin=ewin;
	ewin->snapshotok=0;
	if(timer_mode==0 && ifb.nodo==0) {
		ifb.nodo=1;
	} else if(ifb.moveresize) {
		if (cfg.resize_mode==2) {
			evmd.mode=MODE_RESIZE;
			ifb.nodo=1;
			Draw_Cursor(ewin, (ewin->client_width-ewin->base_width)/ewin->sizeinc_x, (ewin->client_height-ewin->base_height)/ewin->sizeinc_y, 'x', 1);
			XGrabPointer(disp, win, True, ButtonMotionMask|PointerMotionMask|ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
			return;
		} else {
			ifb.nodo=1;
			ResizeLoop(ewin, win);
			ifb.nodo=0;
		}
	}
}

void Do_IconifyWin(EWin *ewin)
{
   struct itimerval tv1,tv2;
   struct sigaction sa;
   
   /* concurrency issues! wait for previous iconify requests to */
   /* be completeed by checking every 10th of a second to see if they */
   /* finished yet */
   if ((timer_mode < TIMER_ICONIFY)&&(timer_mode!=TIMER_INFOBOX)) {
      for (getitimer(ITIMER_REAL, &tv1);
	   tv1.it_value.tv_sec != 0 || tv1.it_value.tv_usec != 0
	   || tv1.it_interval.tv_sec != 0
	   || tv1.it_interval.tv_usec != 0;
	   getitimer(ITIMER_REAL, &tv1));
   }
/*   while (newicon.ewin) usleep(100000);
 *  if ((timer_mode < TIMER_ICONIFY)&&(timer_mode!=TIMER_INFOBOX)) {
 *     for (getitimer(ITIMER_REAL, &tv1); 
 *	   tv1.it_value.tv_sec != 0 || tv1.it_value.tv_usec != 0;
 *	   getitimer(ITIMER_REAL, &tv1)) usleep(100000);
 *  }
 */
   timer_mode = TIMER_ICONIFY;
   tv1.it_value.tv_sec=(unsigned long)(icfg.snapshot_time*1000 / 1000000);
   tv1.it_value.tv_usec=(long)(icfg.snapshot_time*1000 % 1000000);
   tv2.it_value.tv_sec=(unsigned long)(icfg.snapshot_time*1000 / 1000000);
   tv2.it_value.tv_usec=(long)(icfg.snapshot_time*1000 % 1000000);
   tv1.it_interval.tv_sec=0;
   tv1.it_interval.tv_usec=0;
   tv2.it_interval.tv_sec=0;
   tv2.it_interval.tv_usec=0;
   newicon.ewin=ewin;
   newicon.win=ewin->client_win;
   XRaiseWindow(disp,ewin->frame_win);
   setitimer(ITIMER_REAL,&tv1,&tv2);
   sa.sa_handler = Finish_Iconify;
   sa.sa_flags = 0;
   sigemptyset (&sa.sa_mask);
   sigaction(SIGALRM, &sa, (struct sigaction *)0);
}

void Do_KillWin(EWin *ewin, int nasty_kill)
{
   XClientMessageEvent xev;
   Atom atm[2];
   /* Window dummyw; */
   int dummy;
   int wmdel;
   int num;
   Atom *prot;
   Atom dummya;
   int i;
   unsigned long dummyl,numl;
   
   prot=NULL;
   wmdel=0;
   if (!(ewin->state&ICONIFIED))
     {
	newicon.kill=nasty_kill+1;
	Do_IconifyWin(ewin);
	return;
     }
   atm[0]=XInternAtom(disp,"WM_DELETE_WINDOW",False);
   atm[1]=XInternAtom(disp,"WM_PROTOCOLS",False);
   /* work out if client wants to have the wm ask it to quit */
   if (!XGetWMProtocols (disp,ewin->client_win,&prot, &num))
     {
	/* have to work it out the hard way.. nasty client */
	XGetWindowProperty(disp,ewin->client_win, atm[1],0,10,False,atm[1],
			   &dummya,&dummy,&numl,&dummyl,
			   (unsigned char **)&prot);
	num=(int)numl;
     }
   /* check if it has a "delet me" flag set */
   if (prot)
     {
	for (i=0;i<num;i++)
	  {
	     if (prot[i]==atm[0]) wmdel=1;
	  }
     }
   if (prot) XFree(prot);
   if (wmdel)
     {
	/* delete */
	xev.type=ClientMessage;
	xev.window=ewin->client_win;
	xev.message_type=atm[1];
	xev.format=32;
	xev.data.l[0]=atm[0];
	xev.data.l[1]=CurrentTime;
	XSendEvent(disp,ewin->client_win,False,0,(XEvent *)&xev);
     }
   else
     {
	/* kill */
	XKillClient(disp,(XID)ewin->client_win);
     }
   if (nasty_kill)
     {
	/* kill */
	XKillClient(disp,(XID)ewin->client_win);
     }
   XSync(disp,False);
}

void Do_RaiseWin(EWin *ewin)
{
   Window *wl;
   int nc,nb;
   int i,j;
   struct blist *bptr;
   struct list *ptr;
   
   if(ewin->state&ICONIFIED) DeIconify(ewin);
   if((ewin->desk>=0) && (ewin->desk != desk.current)) ChangeToDesk(ewin->desk);
   if (ewin->top) return;
   ToFront(ewin);
   j=0;
   nc=global_l->num;
   nb=0;
   bptr=bl.first;
   while (bptr)
     {
	nb++;
	bptr=bptr->next;
     }
   wl=malloc(sizeof(Window)*(nc+nb+3+(MAX_DESKS*2)));
   bptr=bl.first;
   while (bptr)
     {
	if (bptr->bwin->above) wl[j++]=bptr->bwin->win;
	bptr=bptr->next;
     }
   if (icfg.level)
     {
	wl[j++]=icfg.left_win;
	wl[j++]=icfg.right_win;
	wl[j++]=icfg.bg_win;
     }
   for (i=0;i<MAX_DESKS;i++)
     {
	if (desk.di[i].above)
	  {
	     if (desk.di[i].win) wl[j++]=desk.di[i].win;
	     if (desk.di[i].root_win) wl[j++]=desk.di[i].root_win;
	  }
     }
   ptr=global_l->first;
   while (ptr)
     {
	wl[j++]=ptr->win->frame_win;
	ptr=ptr->next;
     }
   bptr=bl.first;
   while (bptr)
     {
	if (!bptr->bwin->above) wl[j++]=bptr->bwin->win;
	bptr=bptr->next;
     }
   if (!icfg.level)
     {
	wl[j++]=icfg.left_win;
	wl[j++]=icfg.right_win;
	wl[j++]=icfg.bg_win;
     }
   for (i=0;i<MAX_DESKS;i++)
     {
	if (!desk.di[i].above)
	  {
	     if (desk.di[i].win) wl[j++]=desk.di[i].win;
	     if (desk.di[i].root_win) wl[j++]=desk.di[i].root_win;
	  }
     }
   XRestackWindows(disp,wl,j);
   free(wl);
}

void Do_LowerWin(EWin *ewin)
{
   Window *wl;
   int nc,nb;
   int i,j;
   struct blist *bptr;
   struct list *ptr;
   
   ToBack(ewin);
   j=0;
   nc=global_l->num;
   nb=0;
   bptr=bl.first;
   while (bptr)
     {
	nb++;
	bptr=bptr->next;
     }
   wl=malloc(sizeof(Window)*(nc+nb+3+(MAX_DESKS*2)));
   bptr=bl.first;
   while (bptr)
     {
	if (bptr->bwin->above) wl[j++]=bptr->bwin->win;
	bptr=bptr->next;
     }
   if (icfg.level)
     {
	wl[j++]=icfg.left_win;
	wl[j++]=icfg.right_win;
	wl[j++]=icfg.bg_win;
     }
   for (i=0;i<MAX_DESKS;i++)
     {
	if (!desk.di[i].above)
	  {
	     if (desk.di[i].win) wl[j++]=desk.di[i].win;
	     if (desk.di[i].root_win) wl[j++]=desk.di[i].root_win;
	  }
     }
   ptr=global_l->first;
   while (ptr)
     {
	wl[j++]=ptr->win->frame_win;
	ptr=ptr->next;
     }
   bptr=bl.first;
   while (bptr)
     {
	if (!bptr->bwin->above) wl[j++]=bptr->bwin->win;
	bptr=bptr->next;
     }
   if (!icfg.level)
     {
	wl[j++]=icfg.left_win;
	wl[j++]=icfg.right_win;
	wl[j++]=icfg.bg_win;
     }
   for (i=0;i<MAX_DESKS;i++)
     {
	if (!desk.di[i].above)
	  {
	     if (desk.di[i].win) wl[j++]=desk.di[i].win;
	     if (desk.di[i].root_win) wl[j++]=desk.di[i].root_win;
	  }
     }
   XRestackWindows(disp,wl,j);
   free(wl);
}

void Do_RaiseLowerWin(EWin *ewin)
{
   if (ewin->top) Do_LowerWin(ewin);
   else Do_RaiseWin(ewin);
}
void DoShade(EWin *ewin)
{
 int h;
 if(ewin->lastop==OP_SHADE)
 	{
 	 h = ewin->prev_client_height;
	 ewin->prev_client_height = ewin->client_height;
	}
 else
	{
	 ewin->prev_client_height=ewin->client_height;
	 h = 0;
	}
 ModifyEWin(ewin,ewin->frame_x,ewin->frame_y,ewin->client_width,h);
 ewin->lastop=OP_SHADE;
}
void Do_MaxHeightWin(EWin *ewin)
{
   int h;
   int y;
   
   if (ewin->lastop==OP_MAXHEIGHT)
     {
	h=ewin->prev_client_height;
	y=ewin->prev_frame_y;
	ewin->prev_client_height=ewin->client_height;
	ewin->prev_client_width=ewin->client_width;
	ewin->prev_frame_x=ewin->frame_x;
	ewin->prev_frame_y=ewin->frame_y;
     }
   else
     {
	ewin->prev_client_height=ewin->client_height;
	ewin->prev_client_width=ewin->client_width;
	ewin->prev_frame_x=ewin->frame_x;
	ewin->prev_frame_y=ewin->frame_y;
	h=DisplayHeight(disp,screen);
	h-=ewin->border_t;
	h-=ewin->border_b;
	while (((h-ewin->base_height)%ewin->sizeinc_y)>0) h--;
	while (((h-ewin->base_height)%ewin->sizeinc_y)<0) h++;
	y=0;
     }
   ModifyEWin(ewin,ewin->frame_x,y,ewin->client_width,h);
   ewin->lastop=OP_MAXHEIGHT;
}

void Do_MaxWidthWin(EWin *ewin)
{
   int w;
   int x;
   
   if (ewin->lastop==OP_MAXWIDTH)
     {
	w=ewin->prev_client_width;
	x=ewin->prev_frame_x;
	ewin->prev_client_height=ewin->client_height;
	ewin->prev_client_width=ewin->client_width;
	ewin->prev_frame_x=ewin->frame_x;
	ewin->prev_frame_y=ewin->frame_y;
     }
   else
     {
	ewin->prev_client_height=ewin->client_height;
	ewin->prev_client_width=ewin->client_width;
	ewin->prev_frame_x=ewin->frame_x;
	ewin->prev_frame_y=ewin->frame_y;
	w=DisplayWidth(disp,screen);
	w-=ewin->border_l;
	w-=ewin->border_r;
	while (((w-ewin->base_width)%ewin->sizeinc_x)>0) w--;
	while (((w-ewin->base_width)%ewin->sizeinc_x)<0) w++;
	x=0;
     }
   ModifyEWin(ewin,x,ewin->frame_y,w,ewin->client_height);
   ewin->lastop=OP_MAXWIDTH;
}

void Do_MaxSizeWin(EWin *ewin)
{
   int h;
   int y;
   int w;
   int x;
   
   if (ewin->lastop==OP_MAXSIZE)
     {
	h=ewin->prev_client_height;
	y=ewin->prev_frame_y;
	w=ewin->prev_client_width;
	x=ewin->prev_frame_x;
	ewin->prev_client_height=ewin->client_height;
	ewin->prev_client_width=ewin->client_width;
	ewin->prev_frame_x=ewin->frame_x;
	ewin->prev_frame_y=ewin->frame_y;
     }
   else
     {
	ewin->prev_client_height=ewin->client_height;
	ewin->prev_client_width=ewin->client_width;
	ewin->prev_frame_x=ewin->frame_x;
	ewin->prev_frame_y=ewin->frame_y;
	h=DisplayHeight(disp,screen);
	h-=ewin->border_t;
	h-=ewin->border_b;
	while (((h-ewin->base_height)%ewin->sizeinc_y)>0) h--;
	while (((h-ewin->base_height)%ewin->sizeinc_y)<0) h++;
	y=0;
	w=DisplayWidth(disp,screen);
	w-=ewin->border_l;
	w-=ewin->border_r;
	while (((w-ewin->base_width)%ewin->sizeinc_x)>0) w--;
	while (((w-ewin->base_width)%ewin->sizeinc_x)<0) w++;
	x=0;
     }
   ModifyEWin(ewin,x,y,w,h);
   ewin->lastop=OP_MAXSIZE;
}

void Do_Configure()
{
}

void Do_MenuWin(EWin *ewin)
{
}

void CycleToNextWindow()
{
   struct list *l;
   EWin *ewin1;
   EWin *ewin2;
   int i,x,y;
   XWindowAttributes xatt;
  
   do {   
       l=global_l->first;
       if (!l) return;
       if (!l->next) return;
       ewin1=l->win;
       ewin2=l->next->win;
       ToBack(ewin1);
      } while(ewin2->skip); 


   for (i=0;i<cfg.num_subwins;i++)
     {
	if (cfg.subwin_type[i]==2)
	  {
	     XGetWindowAttributes(disp,ewin2->subwins[i],&xatt);
	     x=ewin2->frame_x+xatt.x+(xatt.width/2);
	     y=ewin2->frame_y+xatt.y+(xatt.height/2);
	     i=64;
	  }
     }
   XWarpPointer(disp,None,root,0,0,1,1,x,y);
   FocWin(ewin2->client_win,1);
   Do_RaiseWin(ewin2);
}

void CycleToPreviousWindow()
{
   struct list *l;
   EWin *ewin2, *ewin1, *ewin3;
   int i,x,y;
   XWindowAttributes xatt;
   
   l=global_l->first;
   if (!l) return;
   if (!l->next) return;
   ewin1=global_l->last->win;
   ewin3=global_l->first->win; 
   ewin2=global_l->last->win;
   while((!ewin3->skip)&&(ewin3!=ewin1))
     {
       l=l->next;
       if (!l) return;
       if (!l->next) return;   
       ewin3=l->next->win;
       if((!ewin3->skip)&&(ewin3!=ewin1)) ewin2=ewin3;
     }


   for (i=0;i<cfg.num_subwins;i++)
     {
	if (cfg.subwin_type[i]==2)
	  {
	     XGetWindowAttributes(disp,ewin2->subwins[i],&xatt);
	     x=ewin2->frame_x+xatt.x+(xatt.width/2);
	     y=ewin2->frame_y+xatt.y+(xatt.height/2);
	     i=64;
	  }
     }
   XWarpPointer(disp,None,root,0,0,1,1,x,y);
   FocWin(ewin2->client_win,1);
   Do_RaiseWin(ewin2);
}

void ResizeLoop(EWin *ewin, Window win) {

	GC gc;
	XGCValues gcv;
	XEvent xev;     
	int first;
	int wx,wy;
	int col;

	if (depth>8) 
		col=WhitePixel(disp,screen);
	else 
		col=1;
	first=0;
	XGrabServer(disp);
	XGrabPointer(disp, win, True, ButtonMotionMask|PointerMotionMask|
			ButtonReleaseMask|ButtonPressMask, GrabModeAsync, 
			GrabModeAsync, None, None, CurrentTime);
	gc=XCreateGC(disp,root,0,&gcv);
	XSetSubwindowMode(disp,gc,IncludeInferiors);
	XSetForeground(disp,gc,col);
	XSetFunction(disp,gc,GXxor);

	Draw_Cursor(ewin,(evmd.ewin->client_width-evmd.ewin->base_width)/evmd.ewin->sizeinc_x,(evmd.ewin->client_height-evmd.ewin->base_height)/evmd.ewin->sizeinc_y, 'x', 1);
	while (1) {
		XMaskEvent(disp,ButtonMotionMask|PointerMotionMask|
				ButtonReleaseMask,&xev);
		/* XQueryPointer(disp,root,&w1,&w2,&sx,&sy,&xx,&yy,&d); */
		if (xev.type==ButtonRelease) {
			if (first) {
				XPutBackEvent(disp,&xev);
				switch(cfg.resize_mode) {
					case 0:
						XDrawRectangle(disp,root,gc,evmd.x1,evmd.y1,
								evmd.x2+ewin->border_l+ewin->border_r,
								evmd.y2+ewin->border_t+ewin->border_b);
						XDrawRectangle(disp,root,gc, evmd.x1+ewin->border_l,
								evmd.y1+ewin->border_t,evmd.x2,evmd.y2);
						break;
					case 1:
						XFillRectangle(disp,root,gc,evmd.x1,evmd.y1,
								evmd.x2+ewin->border_l+ewin->border_r,
								evmd.y2+ewin->border_t+ewin->border_b);
						XFillRectangle(disp,root,gc, evmd.x1+ewin->border_l,
								evmd.y1+ewin->border_t,evmd.x2,evmd.y2);
						break;
				}
				break;
			} else {
				XPutBackEvent(disp,&xev);
				break;
			}
		}
		if (first) {
			if (cfg.resize_mode==0) {
				XDrawRectangle(disp,root,gc,evmd.x1,evmd.y1,
						evmd.x2+ewin->border_l+ewin->border_r,
						evmd.y2+ewin->border_t+ewin->border_b);
				XDrawRectangle(disp,root,gc,evmd.x1+ewin->border_l,
						evmd.y1+ewin->border_t,evmd.x2,evmd.y2);
			} else if (cfg.resize_mode==1) {
				XFillRectangle(disp,root,gc,evmd.x1,evmd.y1,
						evmd.x2+ewin->border_l+ewin->border_r,
						evmd.y2+ewin->border_t+ewin->border_b);
				XFillRectangle(disp,root,gc,evmd.x1+ewin->border_l,
						evmd.y1+ewin->border_t,evmd.x2,evmd.y2);
			}
		}
		wx=xev.xmotion.x_root-evmd.px;
		while ((wx%evmd.ewin->sizeinc_x)>0) 
			wx--;
		while ((wx%evmd.ewin->sizeinc_x)<0)
			wx++;
		wy=xev.xmotion.y_root-evmd.py;
		while ((wy%evmd.ewin->sizeinc_y)>0) 
			wy--;
		while ((wy%evmd.ewin->sizeinc_y)<0) 
			wy++;
		if ((wx!=0)||(wy!=0)) {
			switch(evmd.resize_mode) {
				case 0:
					if ((wx>0)&&(evmd.x2<wx)) 
						wx=0;
					if ((wy>0)&&(evmd.y2<wy)) 
						wy=0;
					evmd.x1+=wx;
					evmd.y1+=wy;
					evmd.x2-=wx;
					evmd.y2-=wy;
					break;
				case 1:
					if ((wx<0)&&(evmd.x2<-wx)) 
						wx=0;
					if ((wy>0)&&(evmd.y2<wy)) 
						wy=0;
					evmd.y1+=wy;
					evmd.x2+=wx;
					evmd.y2-=wy;
					break;
				case 2:
					if ((wx>0)&&(evmd.x2<wx)) 
						wx=0;
					if ((wy<0)&&(evmd.y2<-wy)) 
						wy=0;
					evmd.x1+=wx;
					evmd.x2-=wx;
					evmd.y2+=wy;
					break;
				case 3:
					if ((wx<0)&&(evmd.x2<-wx)) 
						wx=0;
					if ((wy<0)&&(evmd.y2<-wy)) 
						wy=0;
					evmd.x2+=wx;
					evmd.y2+=wy;
					break;
			}
			if (evmd.x2>evmd.ewin->max_width) 
				evmd.x2=evmd.ewin->max_width;
			if (evmd.x2<evmd.ewin->min_width) 
				evmd.x2=evmd.ewin->min_width;
			if (evmd.y2>evmd.ewin->max_height) 
				evmd.y2=evmd.ewin->max_height;
			if (evmd.y2<evmd.ewin->min_height) 
				evmd.y2=evmd.ewin->min_height;
		}
		switch(cfg.resize_mode) {
			case 0:
				XDrawRectangle(disp,root,gc,evmd.x1,evmd.y1,
						evmd.x2+ewin->border_l+ewin->border_r,
						evmd.y2+ewin->border_t+ewin->border_b);
				XDrawRectangle(disp,root,gc,evmd.x1+ewin->border_l,
						evmd.y1+ewin->border_t,evmd.x2,evmd.y2);
				break;
			case 1:
				XFillRectangle(disp,root,gc,evmd.x1,evmd.y1,
						evmd.x2+ewin->border_l+ewin->border_r,
						evmd.y2+ewin->border_t+ewin->border_b);
				XFillRectangle(disp,root,gc,evmd.x1+ewin->border_l,
						evmd.y1+ewin->border_t,evmd.x2,evmd.y2);
				break;
		}
		first=1;
		Draw_Cursor(ewin,(evmd.x2-evmd.ewin->base_width)/evmd.ewin->sizeinc_x,
			(evmd.y2-evmd.ewin->base_height)/evmd.ewin->sizeinc_y, 'x', 0);
		XSync(disp,False);
		evmd.px+=wx;
		evmd.py+=wy;
	}
	XFreeGC(disp,gc);
	XUngrabServer(disp);
	Draw_Cursor(ewin, evmd.x1,evmd.y1, '+', 2);
	XUngrabPointer(disp, CurrentTime);
	ModifyEWin(evmd.ewin,evmd.x1,evmd.y1,evmd.x2,evmd.y2);
	XSync(disp,False);
}

void MoveLoop(EWin *ewin, Window win) {

	GC gc;
	XGCValues gcv;
	XEvent xev;     
	int first;
	int col;

	if (depth>8) 
		col=WhitePixel(disp,screen);
	else 
		col=1;
	first=0;
	XGrabServer(disp);
	XGrabPointer(disp, win, True, ButtonMotionMask|PointerMotionMask|
			ButtonReleaseMask|ButtonPressMask, GrabModeAsync, 
			GrabModeAsync, None, None, CurrentTime);
	Draw_Cursor(ewin, ewin->frame_x, ewin->frame_y, '+', 1);
	gc=XCreateGC(disp,root,0,&gcv);
	XSetSubwindowMode(disp,gc,IncludeInferiors);
	XSetForeground(disp,gc,col);
	XSetFunction(disp,gc,GXxor);
	evmd.x1=ewin->frame_x;
	evmd.y1=ewin->frame_y;
	evmd.x2=ewin->client_width;
	evmd.y2=ewin->client_height;
	while (1) {
		XMaskEvent(disp,ButtonMotionMask|PointerMotionMask|ButtonReleaseMask,
				&xev);
		if (xev.type==ButtonRelease) {
			if (first) {
				XPutBackEvent(disp,&xev);
				if (cfg.move_mode==0) {
					XDrawRectangle(disp,root,gc,evmd.x1,evmd.y1,
							evmd.x2+ewin->border_l+ewin->border_r,
							evmd.y2+ewin->border_t+ewin->border_b);
					XDrawRectangle(disp,root,gc,evmd.x1+ewin->border_l,
							evmd.y1+ewin->border_t,evmd.x2,evmd.y2);
				} else if (cfg.move_mode==1) {
					XFillRectangle(disp,root,gc,evmd.x1,evmd.y1,
							evmd.x2+ewin->border_l+ewin->border_r,
							evmd.y2+ewin->border_t+ewin->border_b);
					XFillRectangle(disp,root,gc,evmd.x1+ewin->border_l,
							evmd.y1+ewin->border_t,evmd.x2,evmd.y2);
				}
				break;
			} else {
				XPutBackEvent(disp,&xev);
				break;
			}
		}
		if (first) {
			if (cfg.move_mode==0) {
				XDrawRectangle(disp,root,gc,evmd.x1,evmd.y1,
						evmd.x2+ewin->border_l+ewin->border_r,
						evmd.y2+ewin->border_t+ewin->border_b);
				XDrawRectangle(disp,root,gc,evmd.x1+ewin->border_l,
						evmd.y1+ewin->border_t,evmd.x2,evmd.y2);
			} else if (cfg.move_mode==1) {
				XFillRectangle(disp,root,gc,evmd.x1,evmd.y1,
						evmd.x2+ewin->border_l+ewin->border_r,
						evmd.y2+ewin->border_t+ewin->border_b);
				XFillRectangle(disp,root,gc,evmd.x1+ewin->border_l,
						evmd.y1+ewin->border_t,evmd.x2,evmd.y2);
			}
		}
		evmd.x1+=xev.xmotion.x_root-evmd.px;
		evmd.y1+=xev.xmotion.y_root-evmd.py;
		if (cfg.move_mode==0) {
			XDrawRectangle(disp,root,gc,evmd.x1,evmd.y1,
					evmd.x2+ewin->border_l+ewin->border_r,
					evmd.y2+ewin->border_t+ewin->border_b);
			XDrawRectangle(disp,root,gc,evmd.x1+ewin->border_l,
					evmd.y1+ewin->border_t,evmd.x2,evmd.y2);
		} else if (cfg.move_mode==1) {
			XFillRectangle(disp,root,gc,evmd.x1,evmd.y1,
		    		evmd.x2+ewin->border_l+ewin->border_r,
					evmd.y2+ewin->border_t+ewin->border_b);
			XFillRectangle(disp,root,gc,evmd.x1+ewin->border_l,
					evmd.y1+ewin->border_t,evmd.x2,evmd.y2);
		}
		first=1;
		Draw_Cursor(ewin, evmd.x1,evmd.y1, '+', 0);
		XSync(disp,False);
		evmd.px=xev.xmotion.x_root;
		evmd.py=xev.xmotion.y_root;
	}
	XFreeGC(disp,gc);
	XUngrabServer(disp);
	Draw_Cursor(ewin, evmd.x1,evmd.y1, '+', 2);
	XUngrabPointer(disp, CurrentTime);
	ModifyEWin(evmd.ewin,evmd.x1,evmd.y1,evmd.x2,evmd.y2);
	XSync(disp,False);
}

void PerformWindowOp (int OpToPerform) {

/* created 08/04/97 mandrake */
/* This code is SUPPOSED to behave in roughly the following:
 * If you're in pointerfocus, automatically performs the selected windowop.
 * (if you're already over a window)
 * otherwise it will allow you to move the mouse pointer to another window,
 * click that window, and then have the specified windowop performed.
 * It should, also, change the cursor to a predefined select cursor
 * (via pointer hotswapping code)
 */

    EWin *ewin;
    Window wwin;
	
/* first we're going to let the user select a window (this will
 * grab the server, temporarily, too
 */
	int dummy;
	unsigned int dummyui;
	int finished=0;
	Window dwin,tmp_win;
	XEvent xev;
	char NeedToQuery=1;


	if(!OpToPerform) return;
	/* XGrabServer(disp); */

	/* I'm not totally sure if this part in here will mess up if you're
	 * calling it from a root menu... 
	 */
	XQueryPointer(disp,root,&dwin,&wwin,&dummy,&dummy,&dummy,&dummy,&dummyui);
	ewin=ListGetWinID(global_l,wwin);
	if(ewin) {
		finished=1;
		NeedToQuery=0;
	}

	if(NeedToQuery)
		Do_ChangeMouseCursor("windowselect");

	while (finished==0) {
		XNextEvent(disp,&xev);
		tmp_win=xev.xbutton.subwindow;
		if(xev.type == ButtonPress) finished=1;
	}

	if(NeedToQuery) {
		XQueryPointer(disp,root,&dwin,&wwin,&dummy,&dummy,&dummy,&dummy,&dummyui);
		if(!(ewin=ListGetWinID(global_l,wwin)))
			return;
	}
	/* Do_ChangeMouseCursor("default"); */
	/* XUngrabServer(disp); */
        if(OpToPerform < ACTION_MAX_WIDTH)
	switch(OpToPerform) {
		case ACTION_MOVE:
			Do_RaiseWin(ewin);
			Do_MoveWin(ewin,wwin);
			Do_ChangeMouseCursor("default");
			return;
		case ACTION_RESIZE:
			Do_RaiseWin(ewin);
			Do_ResizeWin(ewin,wwin);
			Do_ChangeMouseCursor("default");
			return;
		case ACTION_ICONIFY:
			Do_IconifyWin(ewin);
			return;
		case ACTION_KILL:
			Do_KillWin(ewin,0);
			return;
		case ACTION_RAISE:
			Do_RaiseWin(ewin);
			return;
		case ACTION_LOWER:
			Do_LowerWin(ewin);
			return;
		case ACTION_RAISE_LOWER:
			Do_RaiseLowerWin(ewin);
			return;
		case ACTION_MAX_HEIGHT:
			Do_RaiseWin(ewin);
			Do_MaxHeightWin(ewin);
			return;
	       }
	      else
              switch(OpToPerform)
	       {	     
		case ACTION_MAX_WIDTH:
			Do_RaiseWin(ewin);
			Do_MaxWidthWin(ewin);
			return;
		case ACTION_MAX_SIZE:
			Do_RaiseWin(ewin);
			Do_MaxSizeWin(ewin);
			return;
		case ACTION_KILL_NASTY:
			Do_KillWin(ewin,1);
			return;
		case ACTION_STICKY:
			ewin->desk = (ewin->desk<0) ? desk.current : -1;
			return;
		case ACTION_DRAG:
			StartIconDrag(ewin);
			return;
		case ACTION_SKIP:
			ewin->skip=ewin->skip?0:1;
			return;
		case ACTION_SHADE:
			Do_RaiseWin(ewin);
			DoShade(ewin);
			break;
		default:
			break;
	}

}
