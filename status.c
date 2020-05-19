#include "enlightenment.h"

void Init_StatusWin()
{
   /* Image *im; */
   XSetWindowAttributes xatt;

/* 07/19/97 Mandrake (mandrake@mandrake.net - http://mandrake.net)
 * I changed Init_StatusWin to use the imlib functions I wrote for iconify.c
 */
   
   LoadImageSizeWithImlib(scfg.icons_bg, scfg.icons_icl, &scfg.icons_pmap, &scfg.icons_mask, scfg.icons_w, scfg.icons_h);
   LoadImageSizeWithImlib(scfg.iconsbox_bg, scfg.iconsbox_icl, &scfg.iconsbox_pmap, &scfg.iconsbox_mask, scfg.iconsbox_w, scfg.iconsbox_h);
   LoadImageSizeWithImlib(scfg.text_bg, scfg.text_icl, &scfg.text_pmap, &scfg.text_mask, scfg.text_w, scfg.text_h);
   LoadImageSizeWithImlib(scfg.textbox_bg, scfg.textbox_icl, &scfg.textbox_pmap, &scfg.textbox_mask, scfg.textbox_w, scfg.textbox_h);
   xatt.override_redirect=True;
   scfg.icons_win=XCreateWindow(disp,root,scfg.icons_x,scfg.icons_y,scfg.icons_w,scfg.icons_h,
				0,depth,InputOutput,visual,CWOverrideRedirect,&xatt);
   scfg.iconsbox_win=XCreateWindow(disp,scfg.icons_win,scfg.iconsbox_x,scfg.iconsbox_y,scfg.iconsbox_w,scfg.iconsbox_h,
				0,depth,InputOutput,visual,CWOverrideRedirect,&xatt);
   scfg.text_win=XCreateWindow(disp,root,scfg.text_x,scfg.text_y,scfg.text_w,scfg.text_h,
				0,depth,InputOutput,visual,CWOverrideRedirect,&xatt);
   scfg.textbox_win=XCreateWindow(disp,scfg.text_win,scfg.textbox_x,scfg.textbox_y,scfg.textbox_w,scfg.textbox_h,
				0,depth,InputOutput,visual,CWOverrideRedirect,&xatt);
   if (scfg.icons_pmap) XSetWindowBackgroundPixmap(disp,scfg.icons_win,scfg.icons_pmap);
   if (scfg.icons_mask) XShapeCombineMask(disp,scfg.icons_win,ShapeBounding,0,0,scfg.icons_mask,ShapeSet);
   if (scfg.iconsbox_pmap) XSetWindowBackgroundPixmap(disp,scfg.iconsbox_win,scfg.iconsbox_pmap);
   if (scfg.iconsbox_mask) XShapeCombineMask(disp,scfg.iconsbox_win,ShapeBounding,0,0,scfg.iconsbox_mask,ShapeSet);
   if (scfg.text_pmap) XSetWindowBackgroundPixmap(disp,scfg.text_win,scfg.text_pmap);
   if (scfg.text_mask) XShapeCombineMask(disp,scfg.text_win,ShapeBounding,0,0,scfg.text_mask,ShapeSet);
   if (scfg.textbox_pmap) XSetWindowBackgroundPixmap(disp,scfg.textbox_win,scfg.textbox_pmap);
   if (scfg.textbox_mask) XShapeCombineMask(disp,scfg.textbox_win,ShapeBounding,0,0,scfg.textbox_mask,ShapeSet);
				        
   XMapWindow(disp,scfg.iconsbox_win);
   XMapWindow(disp,scfg.icons_win);
   XMapWindow(disp,scfg.textbox_win);
   XMapWindow(disp,scfg.text_win);
   scfg.nexticon_x=0;
   scfg.nexticon_y=0;
   scfg.iconwin_list=NULL;
   XSync(disp,False);
}

void Kill_StatusWin()
{
   struct swlist *ptr;
   
   if (scfg.icons_pmap) ImlibFreePixmap(imd,scfg.icons_pmap);    
   if (scfg.iconsbox_pmap) ImlibFreePixmap(imd,scfg.iconsbox_pmap);    
   if (scfg.text_pmap) ImlibFreePixmap(imd,scfg.text_pmap);    
   if (scfg.textbox_pmap) ImlibFreePixmap(imd,scfg.textbox_pmap);    
   if (scfg.icons_bg) free(scfg.icons_bg);
   if (scfg.iconsbox_bg) free(scfg.iconsbox_bg);
   if (scfg.text_bg) free(scfg.text_bg);
   if (scfg.textbox_bg) free(scfg.textbox_bg);
   ptr=scfg.iconwin_list;
   while(ptr)
     {
	if (ptr->win) XDestroyWindow(disp,ptr->win);
	if (ptr->pmap) ImlibFreePixmap(imd,ptr->pmap);
	if (ptr->mask) ImlibFreePixmap(imd,ptr->mask);
	ptr=ptr->next;
     }
   XDestroyWindow(disp,scfg.icons_win);
   XDestroyWindow(disp,scfg.iconsbox_win);
   XDestroyWindow(disp,scfg.text_win);
   XDestroyWindow(disp,scfg.textbox_win);
   if (scfg.icons_pmap) ImlibFreePixmap(imd,scfg.icons_pmap);
   if (scfg.icons_mask) ImlibFreePixmap(imd,scfg.icons_mask);
   if (scfg.iconsbox_pmap) ImlibFreePixmap(imd,scfg.iconsbox_pmap);
   if (scfg.iconsbox_mask) ImlibFreePixmap(imd,scfg.iconsbox_mask);
   if (scfg.text_pmap) ImlibFreePixmap(imd,scfg.text_pmap);
   if (scfg.text_mask) ImlibFreePixmap(imd,scfg.text_mask);
   if (scfg.textbox_pmap) ImlibFreePixmap(imd,scfg.textbox_pmap);
   if (scfg.textbox_mask) ImlibFreePixmap(imd,scfg.textbox_mask);
}

void ShowStatus(char *text, char *icon, ImColor *icl)
{
   struct swlist *ptr;
   struct swlist *pptr;
   Image *im;
   int w,h;
   
   im=LoadImage(imd,icon,icl);
   ptr=scfg.iconwin_list;
   while(ptr)
     {
	pptr=ptr;
	ptr=ptr->next;
     }
   ptr=malloc(sizeof(struct swlist));
   ptr->next=NULL;
   if (scfg.iconwin_list) pptr->next=ptr;
   else scfg.iconwin_list=ptr;
   h=scfg.iconsbox_h;
   w=(im->rgb_width*scfg.iconsbox_h)/im->rgb_height;
   ImlibRender(imd,im,w,h);
   ptr->pmap=ImlibMoveImageToPixmap(imd,im);
   ptr->mask=ImlibMoveMaskToPixmap(imd,im);
   ImlibDestroyImage(imd,im);
   ptr->win=CreateWin(scfg.iconsbox_win,scfg.nexticon_x,scfg.nexticon_y,w,h);
   scfg.nexticon_x+=w;
   if (ptr->pmap) XSetWindowBackgroundPixmap(disp,ptr->win,ptr->pmap);
   if (ptr->mask) XShapeCombineMask(disp,ptr->win,ShapeBounding,0,0,ptr->mask,ShapeSet);
   XClearWindow(disp,ptr->win);
   XMapWindow(disp,ptr->win);
   XClearWindow(disp,scfg.textbox_win);
   DrawText(scfg.textbox_win,text,0,0,scfg.textbox_w,scfg.textbox_h);
   XSync(disp,False);
}
