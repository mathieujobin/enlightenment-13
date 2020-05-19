struct infobox
{
   Pixmap m1_pmap;
   Pixmap m1_mask;
   Pixmap m2_pmap;
   Pixmap m2_mask;
   Pixmap m3_pmap;
   Pixmap m3_mask;
   Pixmap k2_pmap;
   Pixmap k2_mask;
   Pixmap k3_pmap;
   Pixmap k3_mask;
   Pixmap k4_pmap;
   Pixmap k4_mask;
   int mouse_width;
   int key_width;
   int width;
   int height;
   int on;
   int timeout;
   int nodo;
   int moveresize;
};

void InitInfoBox();
void ShowLine(int *x,int *y,int i,int j,char *s,Window *wlist,Pixmap *plst,Pixmap *mlst);
void GenListBtn(BWin *bwin);
void GenListEWin(int b);
void GenListIcon(Icon *Icon_We_Are_Over);
void InfoBox();
void ShowInfo(Window win);


