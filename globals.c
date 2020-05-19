#include "enlightenment.h"

/* for X */
Display *disp;
int screen;
Window root;
Visual *visual;
int depth;
int scr_width;
int scr_height;
Colormap root_cmap;

/* Imlib */

ImlibData *imd;

/* Rest... */

char AlreadyWinListed;
Window FocusWin;
Window FocusWin2;
ConfigWin cfg;
ConfigCursor ccfg;
Icon_cfg icfg;
listhead *global_l;
EWin *global_killewin;
Pixmap root_pm;
Event_Mode_Data evmd;
Window raisewin;
Window sel_win;
char font_weight[32];
char font_slant[32];
int mouse_x;
int mouse_y;
int debug_mode;
char *argv1;
struct _bl bl;
struct _btmd btmd;
struct icon_list ilist;
struct _newicon newicon;
struct scfg_ scfg;
struct winlist *Wlist;
struct winlist winlist;
int num_menus;
char Theme_Tar_Ball[1024];
char Theme_Path[1024];
char Theme_Name[1024];
int nodel;
struct menulist mlist;
struct menulist active_mlist;
Menu *tmp_menu;
Fx fx;
int predef_num;
int KeyBind_num;
int Cursors_num;
struct predef_icon_list *predef_list;
struct KeyBind_list_type *KeyBind_list;
ECursor *Cursors_list;
struct infobox ifb;
struct _desk desk;
char statefile[1024];
int timer_mode;
MenuItem *tmplt_mi;
int restart;
int states;
