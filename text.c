#include "enlightenment.h"

void FontField(char *font, int num, char *ret)
{
   int count;
   int i,j,k,l;
   
   i=0;
   count=-1;
   while (count<num)
     {
	if (font[i++]=='-') count++;
     }
   j=i;
   while (font[j++]!='-');
   l=0;
   for (k=i;k<j-1;k++)
     ret[l++]=font[k];
   ret[l]=0;
}

void InitTextInfo()
{
   char **fonts;
   char pat[1024];
   int num;
   int i;
   int flag;
   char field[1024];
   
   sprintf(pat,"-*-%s-*-*-*-*-*-*-*-*-*-*-*-*",cfg.font);
   fonts=XListFonts(disp,pat,10240,&num);
   flag=0;
	for (i=0;i<num;i++)
     {
	FontField(fonts[i],1,field);
	if (!strcmp(cfg.font,field)) flag=1;
     }
   if (!flag)
     {
	free(cfg.font);
	cfg.font=malloc(6);
	strcpy(cfg.font,"fixed");
     }
   if (cfg.font_style&BOLD)
     {
	flag=0;
	for (i=0;i<num;i++)
	  {
	     FontField(fonts[i],2,field);
	     if (!strcmp("bold",field)) flag=1;
	  }
	if (flag) strcpy(font_weight,"bold");
	else strcpy(font_weight,"*");
     }
   else 
     {
	flag=0;
	for (i=0;i<num;i++)
	  {
	     FontField(fonts[i],2,field);
	     if (!strcmp("medium",field)) flag=1;
			 }
	if (flag) strcpy(font_weight,"medium");
	else strcpy(font_weight,"*");
     }
   if (cfg.font_style&ITALIC)
     {
	flag=0;
		  for (i=0;i<num;i++)
	  {
	     FontField(fonts[i],3,field);
	     if (!strcmp("i",field)) flag=1;
	  }
	if (flag) strcpy(font_slant,"i");
	else strcpy (font_slant,"*");
     }
   else 
     {
	flag=0;
	for (i=0;i<num;i++)
	  {
	     FontField(fonts[i],3,field);
	     if (!strcmp("r",field)) flag=1;
			 }
	if (flag) strcpy(font_slant,"r");
	else strcpy(font_slant,"*");
     }
   XFreeFontNames(fonts);					  
}

void DrawText(Pixmap p, char *txt, int px, int py, int w, int h)
{
   GC fg;
   GC bg;
   XGCValues gcv;
   int hh;
   int i;
   int x,y,ww;
   XFontStruct *xfs;
   XCharStruct dummy2;
   Font font;
   int dummy;
   int ascent;
   int descent;
   char *font_string;
   /* char tstr[32]; */
   char mode;
   
   hh=h-2;
   if (hh<2) return;
   font_string = malloc(strlen(cfg.font)+
	   strlen(font_weight)+
	   strlen(font_slant)+33);
   for (i=hh;i>2;i--)
     {
	sprintf(font_string,"-*-%s-%s-%s-*-*-%i-*-*-*-*-*-*-*",
		cfg.font,font_weight,font_slant,i);
	font=XLoadFont(disp,font_string);
	if (!font) return;
	xfs=XLoadQueryFont(disp,font_string);
	if (!xfs) return;
	XTextExtents(xfs,txt,strlen(txt),&dummy,&ascent,&descent,&dummy2);
	if ((ascent+descent)<=hh) break;
	XFreeFont(disp,xfs);
     }
   fg=XCreateGC(disp,p,0,&gcv);
   bg=XCreateGC(disp,p,0,&gcv);
   XSetFont(disp,fg,font);
   XSetFont(disp,bg,font);
   XSetForeground(disp,fg,cfg.font_fg);
   XSetForeground(disp,bg,cfg.font_bg);
   
   if (cfg.font_style&OUTLINE) mode=1;
   else if (cfg.font_style&SHADOW) mode=2;
   else mode=0;
   
   for (x=strlen(txt);x>0;x--)
     {
	i=XTextWidth(xfs,txt,x);
	if (i<(w-2))
	  {
	     y=1+ascent;
	     ww=x;
	     if (cfg.font_style&J_RIGHT) x=w-i-1;
	     else if (cfg.font_style&J_CENTER) x=(w>>1)-(i>>1);
	     else x=1;
	     if (mode==0)
	       {
		  XDrawString(disp,p,fg,px+x,py+y,txt,ww);
	       }
	     else if (mode==1)
	       {
		  XDrawString(disp,p,bg,px+x-1,py+y,txt,ww);
		  XDrawString(disp,p,bg,px+x+1,py+y,txt,ww);
		  XDrawString(disp,p,bg,px+x,py+y-1,txt,ww);
		  XDrawString(disp,p,bg,px+x,py+y+1,txt,ww);
		  XDrawString(disp,p,fg,px+x,py+y,txt,ww);
	       }
	     else if (mode==2)
	       {
		  XDrawString(disp,p,bg,px+x+1,py+y+1,txt,ww);
		  XDrawString(disp,p,fg,px+x,py+y,txt,ww);
	       }
	     x=-1;
	     break;
	  }
     }
   XFreeFont(disp,xfs);
   free(font_string);
   XFreeGC(disp,fg);
   XFreeGC(disp,bg);
}

void DrawTextMask(Pixmap p, char *txt, int px, int py, int w, int h)
{
   GC or;
   XGCValues gcv;
   int hh;
   int i;
   int x,y,ww;
   XFontStruct *xfs;
   XCharStruct dummy2;
   Font font;
   int dummy;
   int ascent;
   int descent;
   char *font_string;
   /* char tstr[32]; */
   char mode;

   
   hh=h-2;
   if (hh<2) return;
   font_string= malloc(strlen(cfg.font)+
	            strlen(font_weight)+
		    strlen(font_slant)+33);
   for (i=hh;i>2;i--)
     {
	sprintf(font_string,"-*-%s-%s-%s-*-*-%i-*-*-*-*-*-*-*",
		cfg.font,font_weight,font_slant,i);
	font=XLoadFont(disp,font_string);
	if (!font) return;
	xfs=XLoadQueryFont(disp,font_string);
	if (!xfs) return;
	XTextExtents(xfs,txt,strlen(txt),&dummy,&ascent,&descent,&dummy2);
	if ((ascent+descent)<=hh) break;
	XFreeFont(disp,xfs);
     }
   gcv.function=GXor;
   or=XCreateGC(disp,p,GCFunction,&gcv);
   XSetForeground(disp,or,1);
   XSetFont(disp,or,font);
   
   if (cfg.font_style&OUTLINE) mode=1;
   else if (cfg.font_style&SHADOW) mode=2;
   else mode=0;
   
   for (x=strlen(txt);x>0;x--)
     {
	i=XTextWidth(xfs,txt,x);
	if (i<(w-2))
	  {
	     y=1+ascent;
	     ww=x;
	     if (cfg.font_style&J_RIGHT) x=w-i-1;
	     else if (cfg.font_style&J_CENTER) x=(w>>1)-(i>>1);
	     else x=1;
	     if (mode==0)
	       {
		  XDrawString(disp,p,or,px+x,py+y,txt,ww);
	       }
	     else if (mode==1)
	       {
		  XDrawString(disp,p,or,px+x-1,py+y,txt,ww);
		  XDrawString(disp,p,or,px+x+1,py+y,txt,ww);
		  XDrawString(disp,p,or,px+x,py+y-1,txt,ww);
		  XDrawString(disp,p,or,px+x,py+y+1,txt,ww);
		  XDrawString(disp,p,or,px+x,py+y,txt,ww);
	       }
	     else if (mode==2)
	       {
		  XDrawString(disp,p,or,px+x+1,py+y+1,txt,ww);
		  XDrawString(disp,p,or,px+x,py+y,txt,ww);
	       }
	     x=-1;
	     break;
	  }
     }
   free(font_string);
   XFreeFont(disp,xfs);
   XFreeGC(disp,or);
}
