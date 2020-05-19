#include "enlightenment.h"

void TileFill(Pixmap dst, Pixmap src, int dst_w, 
		int dst_h, int src_w, int src_h) {
	int i;
	GC gc;
	XGCValues gcv;

	if (!dst) 
		return;
	if ((dst_w<=0)||(dst_h<=0)||(src_w<=0)||(src_h<=0)) 
		return;

	gc=XCreateGC(disp,dst,0,&gcv);
	XSetForeground(disp,gc,1);
	if (!src) {
		XFillRectangle(disp,dst,gc,0,0,dst_w,dst_h);
	} else if (src_w<=dst_w) {
		for (i=0;i<(dst_h/src_h);i++) {
			XCopyArea(disp,src,dst,gc,0,0,src_w,src_h,0,i*src_h);
		}
		XCopyArea(disp,src,dst,gc,0,0,src_w,dst_h-((i-1)*src_h),0,i*src_h);
		for (i=0;i<(dst_w/src_w);i++) {
			XCopyArea(disp,dst,dst,gc,0,0,src_w,dst_h,i*src_w,0);
		}
		XCopyArea(disp,dst,dst,gc,0,0,dst_w-((i-1)*src_w),dst_h,i*src_w,0);
	} else {
		for (i=0;i<(dst_h/src_h);i++) {
			XCopyArea(disp,src,dst,gc,0,0,dst_w,src_h,0,i*src_h);
		}
		XCopyArea(disp,src,dst,gc,0,0,dst_w,dst_h-((i-1)*src_h),0,i*src_h);
	}
	XFreeGC(disp,gc);
}

void DrawButton(EWin *ewin,int btn) {
	int i;
	GC or;
	GC solid;
	GC transparent;
	XGCValues gcv;
	int bx,by,bw,bh;
	int x,y,w,h;
	int ww,hh;
	int xws, yws, xbs, ybs;
	unsigned wws, hws, wbs, hbs;
	int boundingShaped, clipShaped;
	Pixmap uns,sel,clk;
	int num_x,num_y;

	XShapeQueryExtents(disp,ewin->client_win,&boundingShaped,&xws,&yws,&wws,
			&hws,&clipShaped,&xbs,&ybs,&wbs,&hbs);
	if (cfg.subwin_pos_method1[btn]==0) {
		bx=cfg.subwin_pos_x1[btn];
		by=cfg.subwin_pos_y1[btn];
	} else if (cfg.subwin_pos_method1[btn]==1) {
		bx=ewin->frame_width+cfg.subwin_pos_x1[btn];
		by=cfg.subwin_pos_y1[btn];
	} else if (cfg.subwin_pos_method1[btn]==2) {
		bx=cfg.subwin_pos_x1[btn];
		by=ewin->frame_height+cfg.subwin_pos_y1[btn];
	} else if (cfg.subwin_pos_method1[btn]==3) {
		bx=ewin->frame_width+cfg.subwin_pos_x1[btn];
		by=ewin->frame_height+cfg.subwin_pos_y1[btn];
	}
	if (cfg.subwin_pos_method2[btn]==0) {
		bw=cfg.subwin_pos_x2[btn]-bx;
		bh=cfg.subwin_pos_y2[btn]-by;
	} else if (cfg.subwin_pos_method2[btn]==1) {
		bw=ewin->frame_width+cfg.subwin_pos_x2[btn]-bx;
		bh=cfg.subwin_pos_y2[btn]-by;
	} else if (cfg.subwin_pos_method2[btn]==2) {
		bw=cfg.subwin_pos_x2[btn]-bx;
		bh=ewin->frame_height+cfg.subwin_pos_y2[btn]-by;
	} else if (cfg.subwin_pos_method2[btn]==3) {
		bw=ewin->frame_width+cfg.subwin_pos_x2[btn]-bx;
		bh=ewin->frame_height+cfg.subwin_pos_y2[btn]-by;
	}
	if (cfg.shape_mode) {
		gcv.function=GXor;
		or=XCreateGC(disp,ewin->mask_uns,GCFunction,&gcv);
		gcv.foreground=1;
		solid=XCreateGC(disp,ewin->mask_uns,GCForeground,&gcv);
		gcv.foreground=0;
		transparent=XCreateGC(disp,ewin->mask_uns,GCForeground,&gcv);
		XFillRectangle(disp,ewin->mask_uns,transparent,bx,by,bw,bh);
		XFillRectangle(disp,ewin->mask_sel,transparent,bx,by,bw,bh);
		for(i=0;i<ewin->num_subwins;i++) {
			if (cfg.subwin_pos_method1[i]==0) {
				x=cfg.subwin_pos_x1[i];
				y=cfg.subwin_pos_y1[i];
			} else if (cfg.subwin_pos_method1[i]==1) {
				x=ewin->frame_width+cfg.subwin_pos_x1[i];
				y=cfg.subwin_pos_y1[i];
			} else if (cfg.subwin_pos_method1[i]==2) {
				x=cfg.subwin_pos_x1[i];
				y=ewin->frame_height+cfg.subwin_pos_y1[i];
			} else if (cfg.subwin_pos_method1[i]==3) {
				x=ewin->frame_width+cfg.subwin_pos_x1[i];
				y=ewin->frame_height+cfg.subwin_pos_y1[i];
			}
			if (cfg.subwin_pos_method2[i]==0) {
				w=cfg.subwin_pos_x2[i]-x;
				h=cfg.subwin_pos_y2[i]-y;
			} else if (cfg.subwin_pos_method2[i]==1) {
				w=ewin->frame_width+cfg.subwin_pos_x2[i]-x;
				h=cfg.subwin_pos_y2[i]-y;
			} else if (cfg.subwin_pos_method2[i]==2) {
				w=cfg.subwin_pos_x2[i]-x;
				h=ewin->frame_height+cfg.subwin_pos_y2[i]-y;
			} else if (cfg.subwin_pos_method2[i]==3) {
				w=ewin->frame_width+cfg.subwin_pos_x2[i]-x;
				h=ewin->frame_height+cfg.subwin_pos_y2[i]-y;
			}
			if ((x<(bx+bw))&&(y<by+bh)&&((x+w)>=bx)&&((y+h)>=by)) {
				if (ewin->subwin_state[i]==CLICKED) {
					if (ewin->subwin_pm_clk_mask[i]) 
						XShapeCombineMask(disp,ewin->subwins[i],
								ShapeBounding,0,0,ewin->subwin_pm_clk_mask[i],
								ShapeSet);
					if (ewin->subwin_pm_clk_mask[i]) {
						XCopyArea(disp,ewin->subwin_pm_clk_mask[i],
								ewin->mask_sel,or,0,0,w,h,x,y);
						XCopyArea(disp,ewin->subwin_pm_clk_mask[i],
								ewin->mask_uns,or,0,0,w,h,x,y);
					}
				} else if (ewin->state&SELECTED) {
					if (ewin->subwin_pm_sel_mask[i]) 
						XShapeCombineMask(disp,ewin->subwins[i],
								ShapeBounding,0,0,ewin->subwin_pm_sel_mask[i],
								ShapeSet);
					if (ewin->subwin_pm_sel_mask[i])
						XCopyArea(disp,ewin->subwin_pm_sel_mask[i],
								ewin->mask_sel,or,0,0,w,h,x,y);
					if (ewin->subwin_pm_uns_mask[i])
						XCopyArea(disp,ewin->subwin_pm_uns_mask[i],
								ewin->mask_uns,or,0,0,w,h,x,y);
				} else {
					if (ewin->subwin_pm_uns_mask[i]) 
						XShapeCombineMask(disp,ewin->subwins[i],
								ShapeBounding,0,0,ewin->subwin_pm_uns_mask[i],
								ShapeSet);
					if (ewin->subwin_pm_sel_mask[i])
						XCopyArea(disp,ewin->subwin_pm_sel_mask[i],
								ewin->mask_sel,or,0,0,w,h,x,y);
					if (ewin->subwin_pm_uns_mask[i])
						XCopyArea(disp,ewin->subwin_pm_uns_mask[i],
								ewin->mask_uns,or,0,0,w,h,x,y);
				}
			}
		}
		if ((ewin->client_x<(bx+bw))&&
				(ewin->client_y<by+bh)&&
				((ewin->client_x+ewin->client_width)>=bx)&&
				((ewin->client_y+ewin->client_height)>=by)) {
			if (!boundingShaped) {
				XFillRectangle(disp,ewin->mask_uns,solid,ewin->client_x,
						ewin->client_y,ewin->client_width,ewin->client_height);
				XFillRectangle(disp,ewin->mask_sel,solid,ewin->client_x,
						ewin->client_y,ewin->client_width,ewin->client_height);
			}
		}
	}
	if (cfg.subwin_type[btn]==2) {
		if (ewin->subwin_pm_clk[btn]) 
			ImlibFreePixmap(imd,ewin->subwin_pm_clk[btn]);
		if (ewin->subwin_pm_sel[btn]) 
			ImlibFreePixmap(imd,ewin->subwin_pm_sel[btn]);
		if (ewin->subwin_pm_uns[btn]) 
			ImlibFreePixmap(imd,ewin->subwin_pm_uns[btn]);
		if (cfg.subwin_scale_method[btn]==0) {
			ewin->subwin_pm_clk[btn]=XCreatePixmap(disp,root,w,h,depth);
			ewin->subwin_pm_sel[btn]=XCreatePixmap(disp,root,w,h,depth);
			ewin->subwin_pm_uns[btn]=XCreatePixmap(disp,root,w,h,depth);
			ww=cfg.subwin_img_clk[btn]->rgb_width;
			hh=cfg.subwin_img_clk[btn]->rgb_height;
			ImlibRender(imd,cfg.subwin_img_clk[btn],ww,hh);
			clk=ImlibMoveImageToPixmap(imd,cfg.subwin_img_clk[btn]);
			ImlibFreePixmap(imd,ImlibMoveMaskToPixmap(imd,
					cfg.subwin_img_clk[btn]));
			TileFill(ewin->subwin_pm_clk[btn],clk,w,h,ww,hh);
			if (clk) 
				ImlibFreePixmap(imd,clk);
			ww=cfg.subwin_img_uns[btn]->rgb_width;
			hh=cfg.subwin_img_uns[btn]->rgb_height;
			ImlibRender(imd,cfg.subwin_img_uns[btn],ww,hh);
			uns=ImlibMoveImageToPixmap(imd,cfg.subwin_img_uns[btn]);
			ImlibFreePixmap(imd,ImlibMoveMaskToPixmap(imd,
					cfg.subwin_img_uns[btn]));
			TileFill(ewin->subwin_pm_uns[btn],uns,w,h,ww,hh);
			if (uns) 
				ImlibFreePixmap(imd,uns);
			ww=cfg.subwin_img_sel[btn]->rgb_width;
			hh=cfg.subwin_img_sel[btn]->rgb_height;
			ImlibRender(imd,cfg.subwin_img_sel[btn],ww,hh);
			sel=ImlibMoveImageToPixmap(imd,cfg.subwin_img_sel[btn]);
			ImlibFreePixmap(imd,ImlibMoveMaskToPixmap(imd,
					cfg.subwin_img_sel[btn]));
			TileFill(ewin->subwin_pm_sel[btn],sel,w,h,ww,hh);
			if (sel) 
				ImlibFreePixmap(imd,sel);
		} else if (cfg.subwin_scale_method[btn]==1) {
			ewin->subwin_pm_clk[btn]=XCreatePixmap(disp,root,w,h,depth);
			ewin->subwin_pm_sel[btn]=XCreatePixmap(disp,root,w,h,depth);
			ewin->subwin_pm_uns[btn]=XCreatePixmap(disp,root,w,h,depth);
			num_x=w/cfg.subwin_img_clk[btn]->rgb_width;
			num_y=h/cfg.subwin_img_clk[btn]->rgb_height;
			if (num_x<1) 
				num_x=1;
			if (num_y<1) 
				num_y=1;
			ww=w/num_x;
			hh=h/num_y;
			ImlibRender(imd,cfg.subwin_img_clk[btn],ww,hh);
			clk=ImlibMoveImageToPixmap(imd,cfg.subwin_img_clk[btn]);
			ImlibFreePixmap(imd,ImlibMoveMaskToPixmap(imd,
					cfg.subwin_img_clk[btn]));
			TileFill(ewin->subwin_pm_clk[btn],clk,w,h,ww,hh);
			if (clk) 
				ImlibFreePixmap(imd,clk);
			num_x=w/cfg.subwin_img_uns[btn]->rgb_width;
			num_y=h/cfg.subwin_img_uns[btn]->rgb_height;
			if (num_x<1) 
				num_x=1;
			if (num_y<1) 
				num_y=1;
			ww=w/num_x;
			hh=h/num_y;
			ImlibRender(imd,cfg.subwin_img_uns[btn],ww,hh);
			uns=ImlibMoveImageToPixmap(imd,cfg.subwin_img_uns[btn]);
			ImlibFreePixmap(imd,ImlibMoveMaskToPixmap(imd,
					cfg.subwin_img_uns[btn]));
			TileFill(ewin->subwin_pm_uns[btn],uns,w,h,ww,hh);
			if (uns) 
				ImlibFreePixmap(imd,uns);
			num_x=w/cfg.subwin_img_sel[btn]->rgb_width;
			num_y=h/cfg.subwin_img_sel[btn]->rgb_height;
			if (num_x<1) 
				num_x=1;
			if (num_y<1) 
				num_y=1;
			ww=w/num_x;
			hh=h/num_y;
			ImlibRender(imd,cfg.subwin_img_sel[btn],ww,hh);
			sel=ImlibMoveImageToPixmap(imd,cfg.subwin_img_sel[btn]);
			ImlibFreePixmap(imd,ImlibMoveMaskToPixmap(imd,
					cfg.subwin_img_sel[btn]));
			TileFill(ewin->subwin_pm_sel[btn],sel,w,h,ww,hh);
			if (sel) 
				ImlibFreePixmap(imd,sel);
		} else {
			ImlibRender(imd,cfg.subwin_img_clk[btn],bw,bh);
			ewin->subwin_pm_clk[btn]=ImlibCopyImageToPixmap(imd,
					cfg.subwin_img_clk[btn]);
			ImlibRender(imd,cfg.subwin_img_sel[btn],bw,bh);
			ewin->subwin_pm_sel[btn]=ImlibCopyImageToPixmap(imd,
					cfg.subwin_img_sel[btn]);
			ImlibRender(imd,cfg.subwin_img_uns[btn],bw,bh);
			ewin->subwin_pm_uns[btn]=ImlibCopyImageToPixmap(imd,
					cfg.subwin_img_uns[btn]);
		ImlibFreePixmap(imd,ImlibMoveMaskToPixmap(imd,cfg.subwin_img_clk[btn]));
		ImlibFreePixmap(imd,ImlibMoveImageToPixmap(imd,cfg.subwin_img_clk[btn]));
		ImlibFreePixmap(imd,ImlibMoveMaskToPixmap(imd,cfg.subwin_img_sel[btn]));
		ImlibFreePixmap(imd,ImlibMoveImageToPixmap(imd,cfg.subwin_img_sel[btn]));
		ImlibFreePixmap(imd,ImlibMoveMaskToPixmap(imd,cfg.subwin_img_uns[btn]));
		ImlibFreePixmap(imd,ImlibMoveImageToPixmap(imd,cfg.subwin_img_uns[btn]));
		}
		DrawText(ewin->subwin_pm_sel[btn],ewin->title,0,0,bw,bh);
		DrawText(ewin->subwin_pm_uns[btn],ewin->title,0,0,bw,bh);
		DrawText(ewin->subwin_pm_clk[btn],ewin->title,0,0,bw,bh);
	}
	if (ewin->subwin_state[btn]==CLICKED) {
		if (ewin->subwin_pm_clk[btn]) 
			XSetWindowBackgroundPixmap(disp,ewin->subwins[btn],
					ewin->subwin_pm_clk[btn]);
	} else if (ewin->state&SELECTED) {
		if (ewin->subwin_pm_sel[btn]) 
			XSetWindowBackgroundPixmap(disp,ewin->subwins[btn],
					ewin->subwin_pm_sel[btn]);
	} else {
		if (ewin->subwin_pm_uns[btn]) 
			XSetWindowBackgroundPixmap(disp,ewin->subwins[btn],
					ewin->subwin_pm_uns[btn]);
	}
	XClearWindow(disp,ewin->subwins[btn]);
	if (cfg.shape_mode) {
		if (boundingShaped) {
			XRectangle *xr;
			int count,order;
			int iii;

			xr=XShapeGetRectangles(disp,ewin->client_win,
					ShapeBounding,&count,&order);
			for (iii=0;iii<count;iii++) {
				xr[iii].x+=ewin->client_x;
				xr[iii].y+=ewin->client_y;
			}
			if (count) {
				XFillRectangles(disp,ewin->mask_sel,solid,xr,count);
				XFillRectangles(disp,ewin->mask_uns,solid,xr,count);
				if (xr) 
					XFree(xr);
			}
		}
		if (ewin->state&SELECTED) 
			XShapeCombineMask(disp,ewin->frame_win,
					ShapeBounding,0,0,ewin->mask_sel,ShapeSet);
		else
			XShapeCombineMask(disp,ewin->frame_win,
					ShapeBounding,0,0,ewin->mask_uns,ShapeSet);
		if (fx.shadow.on) {
			if (ewin->state&SELECTED)
				XShapeCombineMask(disp,ewin->fx.shadow_win,
						ShapeBounding,0,0,ewin->mask_sel,ShapeSet);
	 		else
				XShapeCombineMask(disp,ewin->fx.shadow_win,
						ShapeBounding,0,0,ewin->mask_uns,ShapeSet);
		}
		XFreeGC(disp,or);
		XFreeGC(disp,solid);
		XFreeGC(disp,transparent);
	}
	ewin->changes&=~MOD_TITLE;
}

void DrawWindowBorder(EWin *ewin) {
	int i;
	int x,y,w,h;
	unsigned int pw,ph,dummyui;
	int dummyi;
	Window wdummy;
	int num_x,num_y;
	GC gc;
	GC or;
	GC solid;
	GC transparent;
	XGCValues gcv;
	Pixmap sel,uns,clk,sel_mask,uns_mask,clk_mask;
	int ww,hh;
	int xws, yws, xbs, ybs;
	unsigned wws, hws, wbs, hbs;
	int boundingShaped, clipShaped;

	XShapeQueryExtents(disp,ewin->client_win,&boundingShaped,&xws,&yws,&wws,
			&hws,&clipShaped,&xbs,&ybs,&wbs,&hbs);
	gc=XCreateGC(disp,ewin->frame_win,0,&gcv);
	if (ewin->changes&MOD_SIZE) {
		if (cfg.shape_mode) {
			if (ewin->mask_uns) 
				ImlibFreePixmap(imd,ewin->mask_uns);
			if (ewin->mask_sel) 
				ImlibFreePixmap(imd,ewin->mask_sel);
			ewin->mask_uns=XCreatePixmap(disp,ewin->frame_win,
					ewin->frame_width,ewin->frame_height,1);
			ewin->mask_sel=XCreatePixmap(disp,ewin->frame_win,
					ewin->frame_width,ewin->frame_height,1);
			gcv.function=GXor;
			or=XCreateGC(disp,ewin->mask_uns,GCFunction,&gcv);
			gcv.foreground=1;
			solid=XCreateGC(disp,ewin->mask_uns,GCForeground,&gcv);
			gcv.foreground=0;
			transparent=XCreateGC(disp,ewin->mask_uns,GCForeground,&gcv);
			XFillRectangle(disp,ewin->mask_uns,transparent,
					0,0,ewin->frame_width,ewin->frame_height);
			XFillRectangle(disp,ewin->mask_sel,transparent,
					0,0,ewin->frame_width,ewin->frame_height);
			if (!boundingShaped) {
				XFillRectangle(disp,ewin->mask_uns,solid,
						ewin->client_x,ewin->client_y,ewin->client_width,
						ewin->client_height);
				XFillRectangle(disp,ewin->mask_sel,solid,ewin->client_x,
						ewin->client_y,ewin->client_width,ewin->client_height);
			}
		}
		for(i=0;i<ewin->num_subwins;i++) {
			if (cfg.subwin_pos_method1[i]==0) {
				x=cfg.subwin_pos_x1[i];
				y=cfg.subwin_pos_y1[i];
			} else if (cfg.subwin_pos_method1[i]==1) {
				x=ewin->frame_width+cfg.subwin_pos_x1[i];
				y=cfg.subwin_pos_y1[i];
			} else if (cfg.subwin_pos_method1[i]==2) {
				x=cfg.subwin_pos_x1[i];
				y=ewin->frame_height+cfg.subwin_pos_y1[i];
			} else if (cfg.subwin_pos_method1[i]==3) {
				x=ewin->frame_width+cfg.subwin_pos_x1[i];
				y=ewin->frame_height+cfg.subwin_pos_y1[i];
			}
			if (cfg.subwin_pos_method2[i]==0) {
				w=cfg.subwin_pos_x2[i]-x;
				h=cfg.subwin_pos_y2[i]-y;
			} else if (cfg.subwin_pos_method2[i]==1) {
				w=ewin->frame_width+cfg.subwin_pos_x2[i]-x;
				h=cfg.subwin_pos_y2[i]-y;
			} else if (cfg.subwin_pos_method2[i]==2) {
				w=cfg.subwin_pos_x2[i]-x;
				h=ewin->frame_height+cfg.subwin_pos_y2[i]-y;
			} else if (cfg.subwin_pos_method2[i]==3) {
				w=ewin->frame_width+cfg.subwin_pos_x2[i]-x;
				h=ewin->frame_height+cfg.subwin_pos_y2[i]-y;
			}
			if (((x+w)<=ewin->frame_width)&&((y+h)<=ewin->frame_height)
					&&(w>0)&&(h>0)) {
				XGetGeometry(disp,ewin->subwins[i],&wdummy,&dummyi,
						&dummyi,&pw,&ph,&dummyui,&dummyui);
				if ((pw!=w)||(ph!=h)) {
					if (ewin->subwin_pm_clk[i]) 
						ImlibFreePixmap(imd,ewin->subwin_pm_clk[i]);
					if (ewin->subwin_pm_clk_mask[i]) 
						ImlibFreePixmap(imd,ewin->subwin_pm_clk_mask[i]);
					if (ewin->subwin_pm_sel[i]) 
						ImlibFreePixmap(imd,ewin->subwin_pm_sel[i]);
					if (ewin->subwin_pm_sel_mask[i]) 
						ImlibFreePixmap(imd,ewin->subwin_pm_sel_mask[i]);
					if (ewin->subwin_pm_uns[i]) 
						ImlibFreePixmap(imd,ewin->subwin_pm_uns[i]);
					if (ewin->subwin_pm_uns_mask[i]) 
						ImlibFreePixmap(imd,ewin->subwin_pm_uns_mask[i]);
					if (cfg.subwin_scale_method[i]==0) {
						ewin->subwin_pm_clk[i]=XCreatePixmap(disp,root,
								w,h,depth);
						ewin->subwin_pm_sel[i]=XCreatePixmap(disp,root,
								w,h,depth);
						ewin->subwin_pm_uns[i]=XCreatePixmap(disp,root,
								w,h,depth);
						if (cfg.shape_mode) {
							ewin->subwin_pm_clk_mask[i]=XCreatePixmap(disp,
									root,w,h,1);
							ewin->subwin_pm_sel_mask[i]=XCreatePixmap(disp,
									root,w,h,1);
							ewin->subwin_pm_uns_mask[i]=XCreatePixmap(disp,
									root,w,h,1);
						}
						ww=cfg.subwin_img_clk[i]->rgb_width;
						hh=cfg.subwin_img_clk[i]->rgb_height;
						ImlibRender(imd,cfg.subwin_img_clk[i],ww,hh);
						clk=ImlibMoveImageToPixmap(imd,cfg.subwin_img_clk[i]);
						clk_mask=ImlibMoveMaskToPixmap(imd,
								cfg.subwin_img_clk[i]);
						TileFill(ewin->subwin_pm_clk[i],clk,w,h,ww,hh);
						TileFill(ewin->subwin_pm_clk_mask[i],clk_mask,
								w,h,ww,hh);
						if (clk) 
							ImlibFreePixmap(imd,clk);
						if (clk_mask) 
							ImlibFreePixmap(imd,clk_mask);
						ww=cfg.subwin_img_uns[i]->rgb_width;
						hh=cfg.subwin_img_uns[i]->rgb_height;
						ImlibRender(imd,cfg.subwin_img_uns[i],ww,hh);
						uns=ImlibMoveImageToPixmap(imd,cfg.subwin_img_uns[i]);
						uns_mask=ImlibMoveMaskToPixmap(imd,
								cfg.subwin_img_uns[i]);
						TileFill(ewin->subwin_pm_uns[i],uns,w,h,ww,hh);
						TileFill(ewin->subwin_pm_uns_mask[i],uns_mask,
								w,h,ww,hh);
						if (uns) 
							ImlibFreePixmap(imd,uns);
						if (uns_mask) 
							ImlibFreePixmap(imd,uns_mask);
						ww=cfg.subwin_img_sel[i]->rgb_width;
						hh=cfg.subwin_img_sel[i]->rgb_height;
						ImlibRender(imd,cfg.subwin_img_sel[i],ww,hh);
						sel=ImlibMoveImageToPixmap(imd,cfg.subwin_img_sel[i]);
						sel_mask=ImlibMoveMaskToPixmap(imd,
								cfg.subwin_img_sel[i]);
						TileFill(ewin->subwin_pm_sel[i],sel,w,h,ww,hh);
						TileFill(ewin->subwin_pm_sel_mask[i],sel_mask,
								w,h,ww,hh);
						if (sel) 
							ImlibFreePixmap(imd,sel);
						if (sel_mask) 
							ImlibFreePixmap(imd,sel_mask);
					} else if (cfg.subwin_scale_method[i]==1) {
						ewin->subwin_pm_clk[i]=XCreatePixmap(disp,root,
								w,h,depth);
						ewin->subwin_pm_sel[i]=XCreatePixmap(disp,root,
								w,h,depth);
						ewin->subwin_pm_uns[i]=XCreatePixmap(disp,root,
								w,h,depth);
						if (cfg.shape_mode) {
							ewin->subwin_pm_clk_mask[i]=XCreatePixmap(disp,
									root,w,h,1);
							ewin->subwin_pm_sel_mask[i]=XCreatePixmap(disp,
									root,w,h,1);
							ewin->subwin_pm_uns_mask[i]=XCreatePixmap(disp,
									root,w,h,1);
						}
						num_x=w/cfg.subwin_img_clk[i]->rgb_width;
						num_y=h/cfg.subwin_img_clk[i]->rgb_height;
						if (num_x<1) 
							num_x=1;
						if (num_y<1) 
							num_y=1;
						ww=w/num_x;
						hh=h/num_y;
						ImlibRender(imd,cfg.subwin_img_clk[i],ww,hh);
						clk=ImlibMoveImageToPixmap(imd,cfg.subwin_img_clk[i]);
						clk_mask=ImlibMoveMaskToPixmap(imd,
								cfg.subwin_img_clk[i]);
						TileFill(ewin->subwin_pm_clk[i],clk,w,h,ww,hh);
						TileFill(ewin->subwin_pm_clk_mask[i],clk_mask,
								w,h,ww,hh);
						if (clk) 
							ImlibFreePixmap(imd,clk);
						if (clk_mask) 
							ImlibFreePixmap(imd,clk_mask);
						num_x=w/cfg.subwin_img_uns[i]->rgb_width;
						num_y=h/cfg.subwin_img_uns[i]->rgb_height;
						if (num_x<1) 
							num_x=1;
						if (num_y<1) 
							num_y=1;
						ww=w/num_x;
						hh=h/num_y;
						ImlibRender(imd,cfg.subwin_img_uns[i],ww,hh);
						uns=ImlibMoveImageToPixmap(imd,cfg.subwin_img_uns[i]);
						uns_mask=ImlibMoveMaskToPixmap(imd,
								cfg.subwin_img_uns[i]);
						TileFill(ewin->subwin_pm_uns[i],uns,w,h,ww,hh);
						TileFill(ewin->subwin_pm_uns_mask[i],uns_mask,
								w,h,ww,hh);
						if (uns) 
							ImlibFreePixmap(imd,uns);
						if (uns_mask) 
							ImlibFreePixmap(imd,uns_mask);
						num_x=w/cfg.subwin_img_sel[i]->rgb_width;
						num_y=h/cfg.subwin_img_sel[i]->rgb_height;
						if (num_x<1) 
							num_x=1;
						if (num_y<1) 
							num_y=1;
						ww=w/num_x;
						hh=h/num_y;
						ImlibRender(imd,cfg.subwin_img_sel[i],ww,hh);
						sel=ImlibMoveImageToPixmap(imd,cfg.subwin_img_sel[i]);
						sel_mask=ImlibMoveMaskToPixmap(imd,
								cfg.subwin_img_sel[i]);
						TileFill(ewin->subwin_pm_sel[i],sel,w,h,ww,hh);
						TileFill(ewin->subwin_pm_sel_mask[i],sel_mask,
								w,h,ww,hh);
						if (sel) 
							ImlibFreePixmap(imd,sel);
						if (sel_mask) 
							ImlibFreePixmap(imd,sel_mask);
					} else {
						if (cfg.subwin_type[i]==2) {
							ImlibRender(imd,cfg.subwin_img_clk[i],w,h);
							ewin->subwin_pm_clk[i]=ImlibCopyImageToPixmap(imd,
								        cfg.subwin_img_clk[i]);
							ewin->subwin_pm_clk_mask[i]=
								ImlibCopyMaskToPixmap(imd,
									cfg.subwin_img_clk[i]);
		   ImlibFreePixmap(imd,
			ImlibMoveMaskToPixmap(imd,cfg.subwin_img_clk[i]));
		   ImlibFreePixmap(imd,
			ImlibMoveImageToPixmap(imd,cfg.subwin_img_clk[i]));
						        
							ImlibRender(imd,cfg.subwin_img_sel[i],w,h);
							ewin->subwin_pm_sel[i]=ImlibCopyImageToPixmap(imd,
									cfg.subwin_img_sel[i]);
							ewin->subwin_pm_sel_mask[i]=
								ImlibCopyMaskToPixmap(imd,
									cfg.subwin_img_sel[i]);
		   ImlibFreePixmap(imd,
			ImlibMoveMaskToPixmap(imd,cfg.subwin_img_sel[i]));
		   ImlibFreePixmap(imd,
			ImlibMoveImageToPixmap(imd,cfg.subwin_img_sel[i]));
							ImlibRender(imd,cfg.subwin_img_uns[i],w,h);
							ewin->subwin_pm_uns[i]=ImlibCopyImageToPixmap(imd,
									cfg.subwin_img_uns[i]);
							ewin->subwin_pm_uns_mask[i]=
								ImlibCopyMaskToPixmap(imd,
									cfg.subwin_img_uns[i]);
		   ImlibFreePixmap(imd,
			ImlibMoveMaskToPixmap(imd,cfg.subwin_img_uns[i]));
		   ImlibFreePixmap(imd,
			ImlibMoveImageToPixmap(imd,cfg.subwin_img_uns[i]));
						} else {
							ImlibRender(imd,cfg.subwin_img_clk[i],w,h);
							ewin->subwin_pm_clk[i]=ImlibMoveImageToPixmap(imd,
									cfg.subwin_img_clk[i]);
							ewin->subwin_pm_clk_mask[i]=
								ImlibMoveMaskToPixmap(imd,
									cfg.subwin_img_clk[i]);
							ImlibRender(imd,cfg.subwin_img_sel[i],w,h);
							ewin->subwin_pm_sel[i]=ImlibMoveImageToPixmap(imd,
									cfg.subwin_img_sel[i]);
							ewin->subwin_pm_sel_mask[i]=
								ImlibMoveMaskToPixmap(imd,
									cfg.subwin_img_sel[i]);
							ImlibRender(imd,cfg.subwin_img_uns[i],w,h);
							ewin->subwin_pm_uns[i]=ImlibMoveImageToPixmap(imd,
									cfg.subwin_img_uns[i]);
							ewin->subwin_pm_uns_mask[i]=
								ImlibMoveMaskToPixmap(imd,
									cfg.subwin_img_uns[i]);
						}
					}
					if (cfg.shape_mode) {
						if (!ewin->subwin_pm_clk_mask[i]) {
							ewin->subwin_pm_clk_mask[i]=XCreatePixmap(disp,root,
									w,h,1);
							XFillRectangle(disp,ewin->subwin_pm_clk_mask[i],
									solid,0,0,w,h);
						}
						if (!ewin->subwin_pm_sel_mask[i]) {
							ewin->subwin_pm_sel_mask[i]=XCreatePixmap(disp,root,
									w,h,1);
							XFillRectangle(disp,ewin->subwin_pm_sel_mask[i],
									solid,0,0,w,h);
						}
						if (!ewin->subwin_pm_uns_mask[i]) {
							ewin->subwin_pm_uns_mask[i]=XCreatePixmap(disp,root,
									w,h,1);
							XFillRectangle(disp,ewin->subwin_pm_uns_mask[i],
									solid,0,0,w,h);
						}
					}
				}
				if (cfg.subwin_type[i]==2) {
					DrawText(ewin->subwin_pm_sel[i],ewin->title,0,0,w,h);
					DrawText(ewin->subwin_pm_uns[i],ewin->title,0,0,w,h);
					DrawText(ewin->subwin_pm_clk[i],ewin->title,0,0,w,h);
				}
				if (ewin->subwin_state[i]==CLICKED) {
					if (ewin->subwin_pm_clk_mask[i])
						XCopyArea(disp,ewin->subwin_pm_clk_mask[i],
								ewin->mask_sel,or,0,0,w,h,x,y);
					if (ewin->subwin_pm_clk_mask[i])
						XCopyArea(disp,ewin->subwin_pm_clk_mask[i],
								ewin->mask_uns,or,0,0,w,h,x,y);
					if (ewin->subwin_pm_clk[i]) 
						XSetWindowBackgroundPixmap(disp,ewin->subwins[i],
								ewin->subwin_pm_clk[i]);
					if (ewin->subwin_pm_clk_mask[i]) 
						XShapeCombineMask(disp,ewin->subwins[i],ShapeBounding,
								0,0,ewin->subwin_pm_clk_mask[i],ShapeSet);
				} else if (ewin->state&SELECTED) {
					if (ewin->subwin_pm_sel_mask[i])
						XCopyArea(disp,ewin->subwin_pm_sel_mask[i],
								ewin->mask_sel,or,0,0,w,h,x,y);
					if (ewin->subwin_pm_uns_mask[i])
						XCopyArea(disp,ewin->subwin_pm_uns_mask[i],
								ewin->mask_uns,or,0,0,w,h,x,y);
					if (ewin->subwin_pm_sel[i]) 
						XSetWindowBackgroundPixmap(disp,ewin->subwins[i],
								ewin->subwin_pm_sel[i]);
					if (ewin->subwin_pm_sel_mask[i]) 
						XShapeCombineMask(disp,ewin->subwins[i],ShapeBounding,
								0,0,ewin->subwin_pm_sel_mask[i],ShapeSet);
				} else {
					if (ewin->subwin_pm_sel_mask[i])
						XCopyArea(disp,ewin->subwin_pm_sel_mask[i],
								ewin->mask_sel,or,0,0,w,h,x,y);
					if (ewin->subwin_pm_uns_mask[i])
						XCopyArea(disp,ewin->subwin_pm_uns_mask[i],
								ewin->mask_uns,or,0,0,w,h,x,y);
					if (ewin->subwin_pm_uns[i]) 
						XSetWindowBackgroundPixmap(disp,ewin->subwins[i],
								ewin->subwin_pm_uns[i]);
					if (ewin->subwin_pm_uns_mask[i]) 
						XShapeCombineMask(disp,ewin->subwins[i],ShapeBounding,
								0,0,ewin->subwin_pm_uns_mask[i],ShapeSet);
				}
				XMoveWindow(disp,ewin->subwins[i],x,y);
				XResizeWindow(disp,ewin->subwins[i],w,h);
				XClearWindow(disp,ewin->subwins[i]);
			} else {
				XMoveWindow(disp,ewin->subwins[i],-10,-10);
				XResizeWindow(disp,ewin->subwins[i],1,1);
			}
		}
		if (cfg.shape_mode) {
			if (boundingShaped) {
				XRectangle *xr;
				int count,order;
				int iii;

				xr=XShapeGetRectangles(disp,ewin->client_win,
						ShapeBounding,&count,&order);
				for (iii=0;iii<count;iii++) {
					xr[iii].x+=ewin->client_x;
					xr[iii].y+=ewin->client_y;
				}
				if (count) {
					XFillRectangles(disp,ewin->mask_sel,solid,xr,count);
					XFillRectangles(disp,ewin->mask_uns,solid,xr,count);
					if (xr) 
						XFree(xr);
				}
			}
			if (ewin->state&SELECTED)
				XShapeCombineMask(disp,ewin->frame_win,
						ShapeBounding,0,0,ewin->mask_sel,ShapeSet);
			else
				XShapeCombineMask(disp,ewin->frame_win,
						ShapeBounding,0,0,ewin->mask_uns,ShapeSet);
			if (fx.shadow.on) {
				if (ewin->state&SELECTED)
					XShapeCombineMask(disp,ewin->fx.shadow_win,
							ShapeBounding,0,0,ewin->mask_sel,ShapeSet);
				else
					XShapeCombineMask(disp,ewin->fx.shadow_win,
							ShapeBounding,0,0,ewin->mask_uns,ShapeSet);
			}
			XFreeGC(disp,or);
			XFreeGC(disp,solid);
			XFreeGC(disp,transparent);
		}
		ewin->changes=0;
	}
	if (ewin->changes&MOD_SELECT) {
		for(i=0;i<ewin->num_subwins;i++) {
			if (ewin->subwin_state[i]==CLICKED) {
				if (ewin->subwin_pm_clk[i]) 
					XSetWindowBackgroundPixmap(disp,ewin->subwins[i],
							ewin->subwin_pm_clk[i]);
				if (ewin->subwin_pm_clk_mask[i]) 
					XShapeCombineMask(disp,ewin->subwins[i],ShapeBounding,
							0,0,ewin->subwin_pm_clk_mask[i],ShapeSet);
			} else if (ewin->state&SELECTED) {
				if (ewin->subwin_pm_sel[i]) 
					XSetWindowBackgroundPixmap(disp,ewin->subwins[i],
							ewin->subwin_pm_sel[i]);
				if (ewin->subwin_pm_sel_mask[i]) 
					XShapeCombineMask(disp,ewin->subwins[i],ShapeBounding,
							0,0,ewin->subwin_pm_sel_mask[i],ShapeSet);
			} else {
				if (ewin->subwin_pm_uns[i]) 
					XSetWindowBackgroundPixmap(disp,ewin->subwins[i],
							ewin->subwin_pm_uns[i]);
				if (ewin->subwin_pm_uns_mask[i]) 
					XShapeCombineMask(disp,ewin->subwins[i],ShapeBounding,
							0,0,ewin->subwin_pm_uns_mask[i],ShapeSet);
			}
			XClearWindow(disp,ewin->subwins[i]);
		}
		if (cfg.shape_mode) { 
			if (ewin->state&SELECTED)
				XShapeCombineMask(disp,ewin->frame_win,ShapeBounding,
						0,0,ewin->mask_sel,ShapeSet);
			else
				XShapeCombineMask(disp,ewin->frame_win,ShapeBounding,
						0,0,ewin->mask_uns,ShapeSet);
			if (fx.shadow.on) {
				if (ewin->state&SELECTED)
					XShapeCombineMask(disp,ewin->fx.shadow_win,ShapeBounding,
							0,0,ewin->mask_sel,ShapeSet);
				else
					XShapeCombineMask(disp,ewin->fx.shadow_win,ShapeBounding,
							0,0,ewin->mask_uns,ShapeSet);
			}
		}
		XSync(disp,False);
		ewin->changes&=~MOD_SELECT;
	}
	if (ewin->changes&MOD_TITLE) {
		for(i=0;i<ewin->num_subwins;i++)
			if (cfg.subwin_type[i]==2) 
				DrawButton(ewin,i);
		ewin->changes&=~MOD_TITLE;
	}
	if (ewin->changes&MOD_STATE)
		ewin->changes&=~MOD_STATE;
	ewin->changes=0;
	XFreeGC(disp,gc);
	XSync(disp,False);
}

void DrawSetupWindowBorder(EWin *ewin) {
	DrawWindowBorder(ewin);
}
