#include "enlightenment.h"

BWin *GetButtonWinID(Window w) {
	struct blist *ptr;

	ptr=bl.first;
	while (ptr) {
		if (ptr->bwin->win==w) 
			return ptr->bwin;
		ptr=ptr->next;
	}
	return 0;
}

void AddButton(BWin *bwin) {
	struct blist *pptr;
	struct blist *ptr;

	pptr=NULL;
	ptr=bl.first;
	while (ptr) {
		pptr=ptr;
		ptr=ptr->next;
	}
	if (pptr==NULL) {
		bl.first=malloc(sizeof(struct blist));
		bl.first->next=NULL;
		bl.first->bwin=bwin;
	} else {
		pptr->next=malloc(sizeof(struct blist));
		pptr->next->next=NULL;
		pptr->next->bwin=bwin;
	}
}

void DelButton(Window w)
{
}

void ButtonState(Pixmap *win, Pixmap *pmap, Pixmap *mask) {
	if (pmap) 
		XSetWindowBackgroundPixmap(disp,*win,*pmap);
	if (mask) 
		XShapeCombineMask(disp,*win,ShapeBounding,0,0,*mask,ShapeSet);
	XClearWindow(disp,*win);
	XSync(disp,False);
}

void ButtonDraw(BWin *bwin) {

	if (!bwin) 
		return;

	switch (bwin->state) {
		case BTN_NORM:
			ButtonState(&bwin->win,&bwin->unsel_pmap,&bwin->unsel_mask);
			break;
		case BTN_SEL:
			ButtonState(&bwin->win,&bwin->sel_pmap,&bwin->sel_mask);
			break;
		case BTN_CLICK:
			ButtonState(&bwin->win,&bwin->click_pmap,&bwin->click_mask);
			break;
	}
}

void MapButtons() {
	struct blist *ptr;

	ptr=bl.first;
	while (ptr) {
		XMapWindow(disp,ptr->bwin->win);
		ptr=ptr->next;
	}
}

void RaiseLowerButtons() {
	struct blist *ptr;

	ptr=bl.first;
	while (ptr) {
		if (ptr->bwin->above) 
			XRaiseWindow(disp,ptr->bwin->win);
		else 
			XLowerWindow(disp,ptr->bwin->win);
		ptr=ptr->next;
	}
}
