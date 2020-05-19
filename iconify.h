struct i_member
{
   struct i_member *next;
   Icon *icon;
};

struct icon_list
{
   struct i_member *first;
   struct i_member *last;
};

struct predef_icon_list
{
   char *title;
   char *image_file;
   ImColor *transp;
};

typedef struct
{
   char *left_arrow_im;
   char *left_sel_arrow_im;
   char *left_clk_arrow_im;
   char *right_arrow_im;
   char *right_sel_arrow_im;
   char *right_clk_arrow_im;
   char *background_im;
   ImColor left_transparent;
   ImColor left_sel_transparent;
   ImColor left_clk_transparent;
   ImColor right_transparent;
   ImColor right_sel_transparent;
   ImColor right_clk_transparent;
   int x;
   int y;
   int width;
   int height;
   int orientation;
   int snapshot_time;
   int x_scroll;
   int y_scroll;
   int level;
   int scroll_speed;
   Pixmap left_pmap;
   Pixmap left_mask;
   Pixmap left_sel_pmap;
   Pixmap left_sel_mask;
   Pixmap left_clk_pmap;
   Pixmap left_clk_mask;
   Pixmap right_pmap;
   Pixmap right_mask;
   Pixmap right_sel_pmap;
   Pixmap right_sel_mask;
   Pixmap right_clk_pmap;
   Pixmap right_clk_mask;
   Pixmap bg_pmap;
   int left_w;
   int left_h;
   int left_sel_w;
   int left_sel_h;
   int left_clk_w;
   int left_clk_h;
   int right_w;
   int right_h;
   int right_sel_w;
   int right_sel_h;
   int right_clk_w;
   int right_clk_h;
   Window bg_win;
   Window left_win;
   Window right_win;
   Action action[3][4];
   int bx;
   int by;
} Icon_cfg;

struct _newicon
{
   EWin *ewin;
   Window win;
   int kill;
};

Icon *GrabClient(EWin *ewin);
Icon *LoadIconFromDisk(EWin *ewin, char *IconInfo, ImColor *icl);
Icon *PredefinedIconLoad(EWin *ewin, char *TitleBarContents);
char *ReturnIconPath(char *TitleBarContents, ImColor **icl);
Icon *LoadPredefinedIcon(unsigned short PredefinedIconLineNumber);
void LoadImageWithImlib(char *reg_im, ImColor *reg_trans, Pixmap *pmap, Pixmap *mask, int *width, int *height);
void LoadImageSizeWithImlib(char *reg_im, ImColor *reg_trans, Pixmap *pmap, Pixmap *mask, int width, int height);
void InitIcons();
Icon *GetIconWinID(Window w);
void AddIcon(Icon *ic);
void DelIcon(Window w);
void Msg_Iconify(EWin *ewin);
void Msg_DeIconify(EWin *ewin);
void Finish_Iconify();
void MapIconBox();
void IconLeft();
void IconRight();
void IconsScroll(int x);
void RaiseLowerIcons();
void DeIconify(EWin *ewin);
