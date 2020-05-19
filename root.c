#include "enlightenment.h"

void SetRoot(void) {
	Image *root_im;
	unsigned int w,h,dummyui;
	Pixmap pmap;
	Window wdummy;
	int im_w,im_h,dummy;

	Do_ChangeMouseCursor("default");
	if (!desk.background[desk.current][0]) 
		return;
	strcpy(cfg.root_pname,desk.background[desk.current]);
	root_im=LoadImage(imd,cfg.root_pname,NULL);
	XGetGeometry(disp,root,&wdummy,&dummy,&dummy,&w,&h,&dummyui,&dummyui);
	if (desk.bg_width[desk.current]==0) 
		im_w=root_im->rgb_width;
	else if (desk.bg_width[desk.current]==-1) 
		im_w=w;
	else 
		im_w=desk.bg_width[desk.current];
	if (desk.bg_height[desk.current]==0) 
		im_h=root_im->rgb_height;
	else if (desk.bg_height[desk.current]==-1) 
		im_h=h;
	else 
		im_w=desk.bg_height[desk.current];
	ImlibRender(imd,root_im,im_w,im_h);
	pmap=ImlibMoveImageToPixmap(imd,root_im);
	if (desk.di[desk.current].root_pmap) 
		ImlibFreePixmap(imd,desk.di[desk.current].root_pmap);
	ImlibRender(imd,root_im,(desk.di[desk.current].root_width*im_w)/w,
			(desk.di[desk.current].root_height*im_h)/h);
	desk.di[desk.current].root_pmap=ImlibMoveImageToPixmap(imd,root_im);
	XSetWindowBackgroundPixmap(disp,desk.di[desk.current].root_win,
			desk.di[desk.current].root_pmap);
	XClearWindow(disp,desk.di[desk.current].root_win);
	root_pm=pmap;
	desk.bg_pmap[desk.current]=pmap;
	ImlibDestroyImage(imd,root_im);
	XSetWindowBackgroundPixmap(disp,root,pmap);
	XClearWindow(disp,root);
	XSync(disp,False);
}
