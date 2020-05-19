#include "enlightenment.h"

void DoBaseMenu(Menu *m, int x, int y) {
	int px,py;

	if (m) {
		px=x-m->popup_x;
		py=y-m->popup_y;
		if (px<0) 
			px=0;
		if (py<0) 
			py=0;
		if (px+m->width>DisplayWidth(disp,screen)) 
			px=DisplayWidth(disp,screen)-m->width-1;
		if (py+m->height>DisplayHeight(disp,screen)) 
			py=DisplayHeight(disp,screen)-m->height-1;
		/* save the location in the menu structure */
		m->px = px;
		m->py = py;
		XMoveWindow(disp,m->win,m->px,m->py);
		ShowMenu(m);
		AddActiveMenuToList(m);
	}
}

void DoRootMenu(int btn, int mod, int x, int y) {
	Menu *m;
	char *s;

	s = malloc(btn+6);
	sprintf(s,"root%i",btn);
	strcpy(evmd.mname,s);
	m=FindMenu(s);
	free(s);
	DoBaseMenu(m,x,y);
}

void Do_Exec(char *line) {
#ifdef USE_PTHREADS
	pthread_t thread;
	pthread_create(&thread,0,(void*)system,line);
#else
	if(fork()) return;
	setsid();
	if(execl("/bin/sh","sh","-c",line,0)==-1) exit(100);
	exit(0);
#endif
}

void Do_Exit() {
	char *s;
	s = malloc(strlen(Theme_Path)+8);
	if ((Theme_Path[0])&&(!nodel)) {
		sprintf(s,"rm -rf %s",Theme_Path);
		system(s); 
	}
	XSetInputFocus(disp, PointerRoot, RevertToPointerRoot, CurrentTime);
	XSelectInput(disp, root, 0);
	MapClients(global_l);
	free(s);
	exit(0);
}

void Do_Restart() {
	char s[2048];

	if ((Theme_Path[0])&&(!nodel)) {
		sprintf(s,"rm -rf %s",Theme_Path);
		system(s); 
	}
	restart=1;
	WriteAllWindowStates();
	UnmapClients(0);
	XCloseDisplay(disp);
	sleep(5);
	if (Theme_Name[0]!=0) {
		sprintf(s,"%s -restart -theme %s",argv1,Theme_Name);
		execl("/bin/sh","/bin/sh","-c",s,NULL);
	} else {
		execl("/bin/sh","/bin/sh","-c",argv1,"-restart",NULL);
	}
	exit(10);
}

void Do_ChangeMouseFocus(char *newfocus) {

	if(!newfocus) {
		switch(cfg.focus_policy) {
			case PointerFocus:
				cfg.focus_policy = SloppyFocus;
				return;
			case SloppyFocus:
				cfg.focus_policy = ClickToFocus;
				return;
			case ClickToFocus:
				cfg.focus_policy = PointerFocus;
				return;
		}
	} else {
		if(!strcmp(newfocus,"click")) {
			cfg.focus_policy = ClickToFocus;
			return;
		} else if(!strcmp(newfocus,"sloppy")) {
			cfg.focus_policy = SloppyFocus;
			return;
		} else if(!strcmp(newfocus,"mouse")) {
			cfg.focus_policy = PointerFocus;
			return;
		}
	}
}

void Do_ToggleClickRaise(int i) {
	
	if(i) {
		cfg.click_raise=i;
	} else {
		if(cfg.click_raise)
			cfg.click_raise=0;
		else
			cfg.click_raise=1;
	}
}

void Do_ChangeMouseCursor(char *newcursor) {

 /* created 08/09/97 by mandrake */

	XColor fg, bg;
	Cursor cur;
	Pixmap curs, curs_mask;
	char ss[FILENAME_MAX];
	char c1[FILENAME_MAX];
   	char c2[FILENAME_MAX]; 
	unsigned int pw,ph,mw,mh;
	int i,defaultcursor,chsx,chsy,mhsx,mhsy;
	char usedefault=1;

	for(i=0;i<Cursors_num;i++) {
		if(!strcmp(newcursor,Cursors_list[i].cursor_name)) {
			usedefault=0;
			break;
		}
		if(!strcmp("default",Cursors_list[i].cursor_name)) {
			defaultcursor=i;
		}
	}
	if(usedefault) {
		i=defaultcursor;
	}

   	if ((Cursors_list[i].root_pix!=NULL)&&(Cursors_list[i].root_mask!=NULL)&&(!ccfg.doneroot)) {
      		ccfg.doneroot=1;
      		if (Cursors_list[i].root_pix[0]!='/') {
			sprintf(ss,".enlightenment/%s",Cursors_list[i].root_pix);
			if (!exists(ss)) {
		     		sprintf(ss,"%s/.enlightenment/%s",
							getenv("HOME"),Cursors_list[i].root_pix);
		     		if (!exists(ss)) {
		     			if (Theme_Path[0])
		       				sprintf(ss,"%s/%s",Theme_Path,
									Cursors_list[i].root_pix);
		     			else
		       				strcpy(ss,Cursors_list[i].root_pix);
						if (!exists(ss)) {
							Alert("Allrighty then Mr. I cant find that damned %s file!\nYou're going to be in for a nasty day!\n",Cursors_list[i].root_pix);
		       			}
					}
				}
				strcpy(c1,ss);
			} else
				strcpy(c1,Cursors_list[i].root_pix);
      		if (Cursors_list[i].root_mask[0]!='/') {
			sprintf(ss,".enlightenment/%s",Cursors_list[i].root_mask);
			if (!exists(ss)) {
		     		sprintf(ss,"%s/.enlightenment/%s",getenv("HOME"),Cursors_list[i].root_mask);
			  	if (!exists(ss)) {
		     			if (Theme_Path[0])
		       				sprintf(ss,"%s/%s",Theme_Path,Cursors_list[i].root_mask);
		     			else
		       				strcpy(ss,Cursors_list[i].root_mask);
		     			if (!exists(ss)) {
			  			Alert("Allrighty then Mr. I cant find that damned %s file!\nYou're going to be in for a nasty day!\n",Cursors_list[i].root_mask);
		       			}
		  		}
	     		}
	   		strcpy(c2,ss);
		} else
			strcpy(c2,Cursors_list[i].root_mask);
		if(XReadBitmapFile(disp,root,c1,&pw,&ph,&curs,&chsx,&chsy) == BitmapSuccess) {
			if(XReadBitmapFile(disp,root,c2,&mw,&mh,&curs_mask, &mhsx,&mhsy) == BitmapSuccess) {
				fg.red=(Cursors_list[i].root_fg_r<<8)|Cursors_list[i].root_fg_r;
				fg.green=(Cursors_list[i].root_fg_g<<8)|Cursors_list[i].root_fg_g;
				fg.blue=(Cursors_list[i].root_fg_b<<8)|Cursors_list[i].root_fg_b;
				bg.red=(Cursors_list[i].root_bg_r<<8)|Cursors_list[i].root_bg_r;
				bg.green=(Cursors_list[i].root_bg_g<<8)|Cursors_list[i].root_bg_g;
				bg.blue=(Cursors_list[i].root_bg_b<<8)|Cursors_list[i].root_bg_b; 
				XAllocColor(disp,root_cmap,&fg);
				XAllocColor(disp,root_cmap,&bg); 
				cur = XCreatePixmapCursor(disp,curs,curs_mask,&fg,&bg,chsx,chsy); XDefineCursor(disp,root,cur); 
				ImlibFreePixmap(imd,curs);
				ImlibFreePixmap(imd,curs_mask);
			}
		}
	}
	XSync(disp,False);
}

void Do_Restart_Theme(char *file) {
	char *s;
	s = malloc(strlen(Theme_Path)+strlen(argv1)+18);
	if ((Theme_Path[0])&&(!nodel)) {
		sprintf(s,"rm -rf %s",Theme_Path);
		system(s); 
	}
	sprintf(s,"%s -restart -theme %s",argv1,file);
	restart=1;
	WriteAllWindowStates();
	UnmapClients(0);
	XCloseDisplay(disp);
	sleep(5);
	execl("/bin/sh","/bin/sh","-c",s,NULL);
	free(s);
	exit(100);
}

void Do_MenuAction (char *PassedMenu, Window win) {

	/* created 07/20/97 by Mandrake (mandrake@mandrake.net) */

	XWindowAttributes xwa;
	Menu *m;

   	evmd.mode=MODE_MENU;
	tmp_menu=NULL;
	m=FindMenu(PassedMenu);
	if (m) {
		XGetWindowAttributes(disp,win,&xwa);
		DoBaseMenu(m,xwa.x+(xwa.width/2),xwa.y+(xwa.height/2));
	}

}

void DoButton(BWin *bwin, int btn, int mod) {
   
   /* if it wasnt a left, middle or right mouse button... ignore it */
   if ((btn<1)||(btn>3)) return;
   ButtonDraw(bwin);
   switch(bwin->action[btn-1][mod].id) {
	case ACTION_MENU: 
		Do_MenuAction(bwin->action[btn-1][mod].params,bwin->win);
		break;
	case ACTION_EXEC:
		Do_Exec(bwin->action[btn-1][mod].params);
		break;
	case ACTION_EXIT:
		Do_Exit();
		break;
	case ACTION_RESTART:
		Do_Restart();
		break;
	case ACTION_RESTART_THEME:
		Do_Restart_Theme(bwin->action[btn-1][mod].params);
		break;
	case ACTION_BACKGROUND:
		Do_Background(bwin->action[btn-1][mod].params);
		break;
	case ACTION_DESKTOP:
		Do_DesktopSwitch(bwin->action[btn-1][mod].params);
		break;
	case ACTION_MOUSEFOCUS:
		Do_ChangeMouseFocus(bwin->action[btn-1][mod].params);
		break;
	case ACTION_WINDOWOP:
		PerformWindowOp(ParamToInt(bwin->action[btn-1][mod].params));
		break;
	case ACTION_MOUSECURSOR:
		Do_ChangeMouseCursor(bwin->action[btn-1][mod].params);
		break;
	default:
		btmd.bwin=bwin;
		break;
   }
   return;
}

void Do_DesktopSwitch(char *desktop) {

	/* Added 07/20/97 by mandrake (mandrake@mandrake.net) */
	int n1;

	sscanf(desktop,"%i",&n1);
	ChangeToDesk(n1);

}


void Do_Background(char *background) {

	/* Added 07/20/97 by mandrake (mandrake@mandrake.net) */

	int n1,n2;
	char s[1024];

	sscanf(background,"%i %i %s",&n1,&n2,s);
	strcpy(desk.background[desk.current],s);
	if (desk.bg_pmap[desk.current])
  		ImlibFreePixmap(imd,desk.bg_pmap[desk.current]);
	desk.bg_pmap[desk.current]=0;
	desk.bg_width[desk.current]=n1;
	desk.bg_height[desk.current]=n2;
	SetRoot();

}


void RunContents(int action_id, char *params) {
	/* Slight speedup */
	if(action_id < ACTION_DESKTOP) switch(action_id) {
		case ACTION_RAISE: 
			if(params) 
				Do_RaiseFromMenu(params);
			break;
		case ACTION_EXEC: 
			if(params) 
				Do_Exec(params);
			break; 
		case ACTION_EXIT: 
			Do_Exit();
			break;
		case ACTION_RESTART: 
			Do_Restart();
			break;
		case ACTION_RESTART_THEME: 
			if(params) 
				Do_Restart_Theme(params);
			break;
		case ACTION_BACKGROUND: 
			if(params) 
				Do_Background(params);
			break;
		default: 
			break;
	} else switch(action_id) {
		case ACTION_DESKTOP: 
			if(params) 
				Do_DesktopSwitch(params);
			break;
		case ACTION_CYCLE_NEXT: 
			CycleToNextWindow();
			break;
		case ACTION_CYCLE_PREV: 
			CycleToPreviousWindow();
			break;
		case ACTION_NEXT_DESK: 
			NextDesk();
			break;
		case ACTION_PREV_DESK: 
			PrevDesk();
			break;
		case ACTION_MOUSEFOCUS: 
			if(params) 
				Do_ChangeMouseFocus(params);
			break;
		case ACTION_WINDOWOP: 
			if(params) 
				PerformWindowOp(ParamToInt(params));
			break;
		default: 
			break;
	}
}


void DoMenu(Menu *m, int num) {
	if(!m) 
		return;
	/* Let's do a lil' minor speedup here too */
if(m->items[num]->action.id < ACTION_BACKGROUND)
 switch(m->items[num]->action.id)
 	{
	 case ACTION_RAISE: Do_RaiseFromMenu(m->items[num]->action.params);
	      break;
	 case ACTION_EXEC: Do_Exec(m->items[num]->action.params);
	      break;
	 case ACTION_EXIT: Do_Exit();
	      break;
	 case ACTION_RESTART: Do_Restart();
	      break;
	 case ACTION_RESTART_THEME: Do_Restart_Theme(m->items[num]->action.params);
	      break;
	 default: break;
 	}
 else
 switch(m->items[num]->action.id)
	{
	 case ACTION_BACKGROUND: Do_Background(m->items[num]->action.params);
	      break;
	 case ACTION_DESKTOP: Do_DesktopSwitch(m->items[num]->action.params);
	      break;
	 case ACTION_MOUSEFOCUS: Do_ChangeMouseFocus(m->items[num]->action.params);
	      break;
	 case ACTION_WINDOWOP: PerformWindowOp(ParamToInt(
					       m->items[num]->action.params));
	      break;
	 case ACTION_CLICKRAISE:
		  Do_ToggleClickRaise(0);
	 default: break;
	}
}

void Do_RaiseFromMenu(char *itemtoraise) {
	
	/* 07/24/97 modified to use window id by Troy Pesola (trp@cyberoptics.com)*/
	/* created 07/21/97 by mandrake (mandrake@mandrake.net) */
	/* code initially by Troy Pesola (trp@cyberoptics.net) */

	struct list *l;
	EWin *ewin;
	int x,y,i;
	XWindowAttributes xatt;

	/* check if the param is a number */
	if (isdigit(*itemtoraise)) {
		Window www;
		sscanf(itemtoraise,"%lu",&www);
		ewin = ListGetWinID(global_l,www);
	} else 
		ewin=NULL;
	if (!ewin) {
		/* search for a title if we still don't know which window */
		for (l=global_l->first; l; l=l->next) {
			if (!strcmp(l->win->title,itemtoraise))
				break;
		}
		ewin = (l)?l->win:NULL;
	}

	/* raise the window and warp the cursor */
	if (ewin) {
		for (i=0;i<cfg.num_subwins;i++) {
			if (cfg.subwin_type[i]==2) {
				XGetWindowAttributes(disp,ewin->subwins[i],&xatt);
				x=ewin->frame_x+xatt.x+(xatt.width/2);
				y=ewin->frame_y+xatt.y+(xatt.height/2);
				i=64;
			}
		}
		XWarpPointer(disp,None,root,0,0,1,1,x,y);
		FocWin(ewin->client_win,1);
		Do_RaiseWin(ewin);
		evmd.ewin=ewin;
		evmd.wbtn=-1;
		evmd.mode=MODE_NORMAL;
	}
}

void DoIcon(Icon *icon, int btn, int mod)
{
   EWin *ewin;
 /* if it wasnt a left, middle or right mouse button... ignore it */
 if ((btn<1)||(btn>3)) return;
 if(icfg.action[btn-1][mod].id < ACTION_EXEC)
 switch(icfg.action[btn-1][mod].id)
	{
	 case ACTION_ICONIFY: Do_RaiseWin(icon->ewin);
	      break;
	 case ACTION_KILL: Do_KillWin(icon->ewin,0);
	      break;
	 case ACTION_CONFIGURE: Do_Configure();
	      break;
	 case ACTION_MENU: Do_MenuWin(icon->ewin);
	      break;
	 default: break;
	}
 else
 switch(icfg.action[btn-1][mod].id)
	{
	 case ACTION_EXEC: Do_Exec(icfg.action[btn-1][mod].params);
	     break;
	 case ACTION_KILL_NASTY: Do_KillWin(icon->ewin,1);
	     break;
	 case ACTION_DRAG: 
	   ewin = icon->ewin;
	   StartIconDrag(icon->ewin);
           if (ewin->desk==desk.current)
	     {
		XMapWindow(disp,ewin->frame_win);
		Do_RaiseWin(ewin);
	     }
	     break;
	 case ACTION_WINDOWOP:
		PerformWindowOp(ParamToInt(icfg.action[btn-1][mod].params));
	     break;
	 default: break;
   	}
}

void Draw_Cursor(EWin *ewin, int a, int b, char ch, int start) {
	static Cursor cursor;
	static Pixmap pix, mask;
	static GC cursorgc;
	XGCValues gv;
	static XSetWindowAttributes attr;
	unsigned long msk;
	char font_string[512];
	char geom[32];
	static int height, width, ow, oh;
	static XColor col, col2;
	static XFontStruct *font;
	char *neg;
	unsigned int wid, hgt;

	if(start==1) {	
		sprintf(font_string,"-*-%s-%s-%s-*-*-%i-*-*-*-*-*-*-*",
		cfg.font,font_weight,font_slant,ccfg.num_size);
		font=XLoadQueryFont(disp,font_string);
		if (!font) 
			return;
		height = font->max_bounds.ascent+font->max_bounds.descent + 2;
		width = 10*(font->max_bounds.rbearing-font->max_bounds.lbearing);
#ifdef BROKEN_XSERVER
		if (width>CURSOR_MAX_WIDTH) width=CURSOR_MAX_WIDTH;
		if (height>CURSOR_MAX_HEIGHT) height=CURSOR_MAX_HEIGHT;
#else
		XQueryBestCursor(disp,root,width,height,&wid,&hgt);
		if (width>wid) width=wid;
		if (height>hgt) height=hgt;
#endif
		pix = XCreatePixmap(disp, root, width, height, 1);
		mask = XCreatePixmap(disp, root, width, height, 1);
		msk = GCFont|GCForeground;
		gv.foreground=0;
		gv.font=font->fid;
		ow=0; oh=0;
		cursorgc=XCreateGC(disp, pix, msk, &gv);
		col.red=(ccfg.num_fg_r<<8)|ccfg.num_fg_r;
		col.green=(ccfg.num_fg_g<<8)|ccfg.num_fg_g;
		col.blue=(ccfg.num_fg_b<<8)|ccfg.num_fg_b;
		col2.red=(ccfg.num_bg_r<<8)|ccfg.num_bg_r;
		col2.green=(ccfg.num_bg_g<<8)|ccfg.num_bg_g;
		col2.blue=(ccfg.num_bg_b<<8)|ccfg.num_bg_b;
		XAllocColor(disp, root_cmap, &col);
		XAllocColor(disp, root_cmap, &col2);
		XFillRectangle(disp, mask, cursorgc, 0, 0, width, height);
		XFillRectangle(disp, pix, cursorgc, 0, 0, width, height);
	} else if (start==2) {
		XFreeCursor(disp, cursor);
		XDefineCursor(disp, ewin->frame_win,None);
		XFreeGC(disp, cursorgc);
		ImlibFreePixmap(imd, pix);
		ImlibFreePixmap(imd, mask);
		XFreeFont(disp, font);
		return;
	}
	if(!font) 
		return;
	if(ow!=a || oh!=b || start==1) {
		neg = "";
		if(a>=0 && ch == '+')
			neg = "+";
		else if(ch == '+') {
			neg = "-";
			a *= -1;
		}
		if(b<0) {
			ch = '-';
			b *= -1;
		}
		sprintf(geom, "%s%d%c%d", neg, a, ch, b);
		msk = strlen(geom);
		hgt = font->max_bounds.ascent;
		XSetForeground(disp, cursorgc, 1);
		XDrawString(disp, pix, cursorgc, 0, hgt, geom, msk);
		XDrawString(disp, pix, cursorgc, 0, hgt+2, geom, msk);
		XDrawString(disp, pix, cursorgc, 2, hgt, geom, msk);
		XDrawString(disp, pix, cursorgc, 2, hgt+2, geom, msk);

		XCopyArea(disp, pix, mask, cursorgc, 0, 0, width, height, 0, 0);

		XDrawString(disp, mask, cursorgc, 1, hgt+1, geom, msk);
		XSetForeground(disp, cursorgc, 0);
		XDrawString(disp, pix, cursorgc, 1, hgt+1, geom, msk);

		if (cursor) 
			XFreeCursor(disp, cursor);
		cursor = XCreatePixmapCursor(disp, pix, mask, &col2, &col, 0, 0);
		msk = CWCursor;
		attr.cursor = cursor;
		XChangeWindowAttributes(disp, ewin->frame_win, msk, &attr);

		XFillRectangle(disp, mask, cursorgc, 0, 0, width, height);
		XFillRectangle(disp, pix, cursorgc, 0, 0, width, height);
		ow = (evmd.x2-evmd.ewin->base_width)/evmd.ewin->sizeinc_x;
		oh = (evmd.y2-evmd.ewin->base_height)/evmd.ewin->sizeinc_y;
	}
}
