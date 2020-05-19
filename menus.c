#include "enlightenment.h"

void InitMenuList() {
	mlist.next=NULL;
	mlist.menu=NULL;
	active_mlist.next=NULL;
	active_mlist.menu=NULL;
}

void AddMenuToList(Menu *menu) {
	struct menulist *ptr;

	ptr=malloc(sizeof(struct menulist));
	ptr->menu=menu;
	ptr->next=mlist.next;
	mlist.next=ptr;
}

Menu *FindMenu(char *name) {
   struct menulist *ptr;

	ptr=mlist.next;
	while(ptr) {
		if (!strcmp(name,ptr->menu->name)) return ptr->menu;
			ptr=ptr->next;
	}
	return NULL;
}

Menu *GetMenuWin(Window w) {
	struct menulist *ptr;

	ptr=mlist.next;
	while(ptr) {
		if (w==ptr->menu->win) 
			return ptr->menu;
		ptr=ptr->next;
	}
	return NULL;
}

void AddActiveMenuToList(Menu *menu) {
	struct menulist *ptr;

	ptr=malloc(sizeof(struct menulist));
	ptr->menu=menu;
	ptr->next=active_mlist.next;
	active_mlist.next=ptr;

}

void DeleteToActiveMenu(char *name) {
	struct menulist *ptr;
	struct menulist *pptr;

	ptr=active_mlist.next;
	while(ptr) {
		if (!strcmp(name,ptr->menu->name)) {
			active_mlist.next=ptr;
			return;
		}
		pptr=ptr;
		HideMenu(ptr->menu);
		ptr=ptr->next;
		free(pptr);
	}
	active_mlist.next=NULL;
}

Menu *GetActiveMenuWin(Window w) {
	struct menulist *ptr;

	ptr=mlist.next;
	while(ptr) {
		if (w==ptr->menu->win) 
			return ptr->menu;
		ptr=ptr->next;
	}
	return NULL;
}

Menu *CreateMenu(char *name, int type, int num_items) {
	Menu *m;
	int i;

	m=malloc(sizeof(Menu));
	if (!m) return NULL;
	m->type=type;
	m->num_items=num_items;
	m->width=0;
	m->height=0;
	if (name) 
		strncpy(m->name,name,255);
	else 
		name[0]=0; 
	m->popup_x=0;
	m->popup_y=0;
	m->win=0;
	m->mask=0;
	m->items=malloc(sizeof(MenuItem *)*num_items);
	m->sel_item=-1;
	for (i=0;i<num_items;i++) {
		m->items[i]=NULL;
	}
	return m;
}

void AddMenuItem(Menu *m, MenuItem* mi) {

	/* modified 07/21/97 Troy Pesola (trp@cyberoptics.com) */

	int i,found=0;
	/* char ss[4096]; */
	MenuItem **items;

	for (i=0;i<m->num_items;i++) {
		if (!m->items[i]) {
			m->items[i]=mi;
			found=1;
			break;
		}
	}
	if(!found) {
		items = (MenuItem**)malloc(sizeof(MenuItem*) * m->num_items+1);
		memcpy(items,m->items, (sizeof(MenuItem*) * m->num_items));
		items[m->num_items++] = mi;
		free(m->items);
		m->items = items;

		/* step up the size if needed */
		if (m->width<(mi->x+mi->width))
			m->width = mi->x+mi->width;
		if (m->height<(mi->y+mi->height))
			m->height = mi->y+mi->height;
	}
}

void RenderMenu(Menu *m) {
	int i;
	XGCValues gcv;
	GC gc;
	GC or;

	/* added 07/24/97 by Troy Pesola (trp@cyberoptics.com) */
	if (m->static_num_items) {
		/* map the window to the root */
		for (i=0; i<m->static_num_items; i++)
			XReparentWindow(disp,m->items[i]->win,root,0,0);
	}

	if (m->mask) 
		ImlibFreePixmap(imd,m->mask);
	if (m->win) 
		XDestroyWindow(disp,m->win);

	m->win=CreateBasicWin(root,0,0,m->width,m->height);
	m->mask=XCreatePixmap(disp,m->win,m->width,m->height,1);

	gcv.function=GXor;
	gc=XCreateGC(disp,m->mask,0,&gcv);
	or=XCreateGC(disp,m->mask,GCFunction,&gcv);
	XSetForeground(disp,gc,0);
	XSetForeground(disp,or,1);
	XFillRectangle(disp,m->mask,gc,0,0,m->width,m->height);
	XSetForeground(disp,gc,1);
	for(i=0;i<m->num_items;i++) {
		if (m->items[i]) {
			if (m->items[i]->unsel_mask)
				XCopyArea(disp,m->items[i]->unsel_mask,m->mask,or,
					0,0,m->items[i]->width,m->items[i]->height,
					m->items[i]->x,m->items[i]->y);
			else
				XFillRectangle(disp,m->mask,gc,m->items[i]->x,
					m->items[i]->y,m->items[i]->width,
					m->items[i]->height);
			XReparentWindow(disp,m->items[i]->win,m->win,
				m->items[i]->x,m->items[i]->y);
				XMapWindow(disp,m->items[i]->win);
		}
	}
	XShapeCombineMask(disp,m->win,ShapeBounding,0,0,m->mask,ShapeSet);
	XFreeGC(disp,gc);
	XFreeGC(disp,or);
}

MenuItem *CreateMenuItem(int type, 
			int x, int y, int width, int height, char *text, 
			int text_x, int text_y, int text_w, int text_h, 
			char *unsel, ImColor *unsel_icl,
			char *sel, ImColor *sel_icl) {
	MenuItem *mi;
	Image *im1,*im2;

	mi=malloc(sizeof(MenuItem));
	mi->x=x;
	mi->y=y;
	mi->width=width;
	mi->height=height;
	mi->text=NULL;
	mi->type=type;
	mi->unsel_pmap=0;
	mi->unsel_mask=0;
	mi->sel_pmap=0;
	mi->sel_mask=0;

	if (mi->type==DECOR)
	   sel=NULL;
	mi->win=CreateWin(root,0,0,mi->width,mi->height);
	im1=LoadImage(imd,unsel,unsel_icl);
	ImlibRender(imd,im1,mi->width,mi->height);
	mi->unsel_pmap=ImlibCopyImageToPixmap(imd,im1);
	mi->unsel_mask=ImlibCopyMaskToPixmap(imd,im1);
    ImlibFreePixmap(imd,ImlibMoveMaskToPixmap(imd,im1));
    ImlibFreePixmap(imd,ImlibMoveImageToPixmap(imd,im1));		    
	ImlibDestroyImage(imd,im1);
	if (sel) {
		im2=LoadImage(imd,sel,sel_icl);
		ImlibRender(imd,im2,mi->width,mi->height);
		mi->sel_pmap=ImlibCopyImageToPixmap(imd,im2);
		mi->sel_mask=ImlibCopyMaskToPixmap(imd,im2);
		ImlibDestroyImage(imd,im2);
	}
	if (text) {
		mi->text=malloc(strlen(text)+1);
		strcpy(mi->text,text);
		DrawText(mi->unsel_pmap,mi->text,text_x,text_y,text_w,text_h);
		if (mi->unsel_mask) 
			DrawTextMask(mi->unsel_mask,mi->text,text_x,text_y,text_w,text_h);
		if (mi->sel_pmap) 
			DrawText(mi->sel_pmap,mi->text,text_x,text_y,text_w,text_h);
		if (mi->sel_mask) 
			DrawTextMask(mi->sel_mask,mi->text,text_x,text_y,text_w,text_h);
	} 
	XSetWindowBackgroundPixmap(disp,mi->win,mi->unsel_pmap);
	if (mi->unsel_mask) 
		XShapeCombineMask(disp,mi->win,ShapeBounding,0,0,mi->unsel_mask,ShapeSet);
	return mi;
}

MenuItem *CreateMenuWinlistItem(int type, 
			int x, int y, int width, int height, char *text, 
			int text_x, int text_y, int text_w, int text_h, 
			char *unsel, ImColor *unsel_icl,
			char *sel, ImColor *sel_icl) {
	MenuItem *mi;
	/* Image *im1,*im2; */
	GC gc,gcm;
	XGCValues gcv;

	if (!tmplt_mi) 
		return NULL;
	mi=malloc(sizeof(MenuItem));
	mi->x=x;
	mi->y=y;
	mi->width=width;
	mi->height=height;
	mi->text=NULL;
	mi->type=type;
	mi->unsel_pmap=0;
	mi->unsel_mask=0;
	mi->sel_pmap=0;
	mi->sel_mask=0;

	gc=XCreateGC(disp,tmplt_mi->unsel_pmap,0,&gcv);
	if (tmplt_mi->unsel_mask) 
		gcm=XCreateGC(disp,tmplt_mi->unsel_mask,0,&gcv);

	mi->win=CreateWin(root,0,0,mi->width,mi->height);
	if (mi->type==DECOR) {
		sel=NULL;
	} else {
		mi->unsel_pmap=XCreatePixmap(disp,root,mi->width,mi->height,depth);
		XCopyArea(disp,tmplt_mi->unsel_pmap,mi->unsel_pmap,gc,0,0,
			mi->width,mi->height,0,0);
		mi->sel_pmap=XCreatePixmap(disp,root,mi->width,mi->height,depth);
		XCopyArea(disp,tmplt_mi->sel_pmap,mi->sel_pmap,gc,0,0,
			mi->width,mi->height,0,0);
		if (tmplt_mi->unsel_mask) {
			mi->unsel_mask=XCreatePixmap(disp,root,mi->width,mi->height,1);
			XCopyArea(disp,tmplt_mi->unsel_mask,mi->unsel_mask,gcm,0,0,
				mi->width,mi->height,0,0);
			mi->sel_mask=XCreatePixmap(disp,root,mi->width,mi->height,1);
			XCopyArea(disp,tmplt_mi->sel_mask,mi->sel_mask,gcm,0,0,
				mi->width,mi->height,0,0);
		}
	}
	if (text) {
		mi->text=malloc(strlen(text)+1);
		strcpy(mi->text,text);
		DrawText(mi->unsel_pmap,mi->text,text_x,text_y,text_w,text_h);
		if (mi->unsel_mask) 
			DrawTextMask(mi->unsel_mask,mi->text,text_x,text_y,text_w,text_h);
		if (mi->sel_pmap) 
			DrawText(mi->sel_pmap,mi->text,text_x,text_y,text_w,text_h);
		if (mi->sel_mask) 
			DrawTextMask(mi->sel_mask,mi->text,text_x,text_y,text_w,text_h);
	} 
	XSetWindowBackgroundPixmap(disp,mi->win,mi->unsel_pmap);
	if (mi->unsel_mask) 
		XShapeCombineMask(disp,mi->win,ShapeBounding,0,0,
				mi->unsel_mask,ShapeSet);
	XFreeGC(disp,gc);
	if (tmplt_mi->unsel_mask) 
		XFreeGC(disp,gcm);
	return mi;
}

void DrawMenuItem(Menu *m, int num, int state) {
	XGCValues gcv;
	GC gc;
	GC or;
	int i;
	int in;
	int inx,iny;
	int xx1,xx2,yy1,yy2;
 
	gcv.function=GXor;
	gc=XCreateGC(disp,m->mask,0,&gcv);
	or=XCreateGC(disp,m->mask,GCFunction,&gcv);
	XSetForeground(disp,gc,0);
	XSetForeground(disp,or,1);
	XFillRectangle(disp,m->mask,gc,m->items[num]->x,m->items[num]->y,
			m->items[num]->width,m->items[num]->height);
	XSetForeground(disp,gc,1);

	if (state==0) {
		XSetWindowBackgroundPixmap(disp,m->items[num]->win,
				m->items[num]->unsel_pmap);
		if (m->items[num]->unsel_mask) 
			XShapeCombineMask(disp,m->items[num]->win,
					ShapeBounding,0,0,m->items[num]->unsel_mask,ShapeSet);
	} else {
		XSetWindowBackgroundPixmap(disp,m->items[num]->win,
				m->items[num]->sel_pmap);
		if (m->items[num]->sel_mask) 
			XShapeCombineMask(disp,m->items[num]->win,
					ShapeBounding,0,0,m->items[num]->sel_mask,ShapeSet);
	}
	XClearWindow(disp,m->items[num]->win);
	xx1=m->items[num]->x;
	xx2=m->items[num]->x+m->items[num]->width;
	yy1=m->items[num]->y;
	yy2=m->items[num]->y+m->items[num]->height;
	for(i=0;i<m->num_items;i++) {
		int x1,x2,y1,y2;
		inx=0,iny=0;in=0;
		x1=m->items[i]->x;
		x2=m->items[i]->x+m->items[i]->width;
		y1=m->items[i]->y;
		y2=m->items[i]->y+m->items[i]->height;
		if (((x1>=xx1)&&(x1<=xx2))|| ((x2>=xx1)&&(x2<=xx2))||
	    		((xx1>=x1)&&(xx2<=x2))) 
			inx=1;
		if (((y1>=yy1)&&(y1<=yy2))|| ((y2>=yy1)&&(y2<=yy2))||
	    		((yy1>=y1)&&(yy2<=y2))) 
			iny=1;
		if (inx&&iny) 
			in=1;
		if (in) {
			if ((i==num)&&(m->items[i]->type!=DECOR)) {
				if (state==0) {
					if (m->items[i]->unsel_mask)
						XCopyArea(disp,m->items[i]->unsel_mask,m->mask,or,
								0,0,m->items[i]->width,m->items[i]->height,
								m->items[i]->x,m->items[i]->y);
					else
						XFillRectangle(disp,m->mask,gc,m->items[i]->x,
								m->items[i]->y,m->items[i]->width,
								m->items[i]->height);
				} else {
					if (m->items[i]->sel_mask)
						XCopyArea(disp,m->items[i]->sel_mask,m->mask,or,
								0,0,m->items[i]->width,m->items[i]->height,
								m->items[i]->x,m->items[i]->y);
					else if (m->items[i]->type==DECOR)
						XCopyArea(disp,m->items[i]->unsel_mask,m->mask,or,
								0,0,m->items[i]->width,m->items[i]->height,
								m->items[i]->x,m->items[i]->y);
					else
						XFillRectangle(disp,m->mask,gc,m->items[i]->x,
								m->items[i]->y,m->items[i]->width,
								m->items[i]->height);
				}
			} else {
				if (m->items[i]->unsel_mask)
					XCopyArea(disp,m->items[i]->unsel_mask,m->mask,or,
							0,0,m->items[i]->width,m->items[i]->height,
							m->items[i]->x,m->items[i]->y);
				else
					XFillRectangle(disp,m->mask,gc,m->items[i]->x,
							m->items[i]->y,m->items[i]->width,
							m->items[i]->height);
			}
		}
	}
	XShapeCombineMask(disp,m->win,ShapeBounding,0,0,m->mask,ShapeSet);
	XFreeGC(disp,gc);
	XFreeGC(disp,or);
	XSync(disp,False);
}

void ShowMenu(Menu *menu) {

    /* modified 07/24/97 Troy Pesola (trp@cyberoptics.com) */
    /*    --now use the window id instead of the title     */
    /*    --supports having items from the loadcfg         */
    /* modified 07/21/97 Troy Pesola (trp@cyberoptics.com) */

    /* int px,py; */
    struct list *l;
    MenuItem *mi;

	/* check for special types of menus */
	if (menu->type==MENU_WINDOWLIST) {
		if(!AlreadyWinListed) {
		/* save the static menu items */
			if (menu->num_items) {
				menu->static_items = 
					(MenuItem**)malloc(sizeof(MenuItem*) * menu->num_items);
				memcpy(menu->static_items,
						menu->items, sizeof(MenuItem*) * menu->num_items);
			} else {
				menu->static_items=NULL;
			}
			menu->static_num_items = menu->num_items;
			menu->item_x = menu->itemdelta_x * menu->num_items;
			menu->item_y = menu->itemdelta_y * menu->num_items;
			menu->static_item_x = menu->item_x;
			menu->static_item_y = menu->item_y;
			if (!tmplt_mi) {
				tmplt_mi=CreateMenuItem(ELEMENT,
						menu->item_x,menu->item_y,
						menu->item_w,menu->item_h,
						NULL, menu->itemtext_x,menu->itemtext_y,
						menu->itemtext_w,menu->itemtext_h,
						menu->item_unsel,&menu->item_unsel_icl,
						menu->item_sel,&menu->item_sel_icl);
			}
			/* create the menu in real time */
			for (l=global_l->first; l; l=l->next) {
				if(!l->win->skip) {
					mi=CreateMenuWinlistItem(ELEMENT,
							menu->item_x,menu->item_y,
							menu->item_w,menu->item_h,
							l->win->title,
							menu->itemtext_x,menu->itemtext_y,
							menu->itemtext_w,menu->itemtext_h,
							menu->item_unsel,&menu->item_unsel_icl,
							menu->item_sel,&menu->item_sel_icl);
					menu->item_x += menu->itemdelta_x;
					menu->item_y += menu->itemdelta_y;
					mi->action.id=ACTION_RAISE;
					sprintf(mi->action.params,"%lu",l->win->frame_win);
					AddMenuItem(menu,mi);
				}
			}
			AlreadyWinListed=1;
			RenderMenu(menu);

			/* position the menu */
			XMoveWindow(disp,menu->win,menu->px,menu->py);
		}
	}
	XRaiseWindow(disp,menu->win);
	XMapWindow(disp,menu->win);
	XSync(disp,False);
}

void HideMenu(Menu *menu) {

	/* modified 07/24/97 Troy Pesola (trp@cyberoptics.com) */
	/*   --added support for static items in the menu      */
	/* modified 07/21/97 Troy Pesola (trp@cyberoptics.com) */

	XUnmapWindow(disp,menu->win);
	if (menu->sel_item>=0) 
		DrawMenuItem(menu,menu->sel_item,0);
	menu->sel_item=-1;
	XSync(disp,False);

	if (menu->type==MENU_WINDOWLIST) {
		/* remove the menu items */
		int i; /* cnt=0; */
		for (i=menu->static_num_items; i<menu->num_items; i++) {
			XDestroyWindow(disp,menu->items[i]->win);
			ImlibFreePixmap(imd,menu->items[i]->unsel_pmap);
			ImlibFreePixmap(imd,menu->items[i]->sel_pmap);
			if (menu->items[i]->unsel_mask)
				ImlibFreePixmap(imd,menu->items[i]->unsel_mask);
			if (menu->items[i]->sel_mask)
				ImlibFreePixmap(imd,menu->items[i]->sel_mask);
			if (menu->items[i]->text) free(menu->items[i]->text);
				free(menu->items[i]);
		}

		free(menu->items);

		/* restore the static menu items */
		menu->items = menu->static_items;
		menu->num_items = menu->static_num_items;
		menu->item_x = menu->static_item_x;
		menu->item_y = menu->static_item_y;
		menu->static_items = NULL;
		AlreadyWinListed=0;
	}
}

void HideAllMenus() {
	struct menulist *ptr;

	ptr=mlist.next;
	while(ptr) {
		HideMenu(ptr->menu);
		ptr=ptr->next;
	}
}
