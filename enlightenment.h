/*
 * Defines for various system dpendant stuff
 */

#define ImColor ImlibColor
#define Image ImlibImage
#define ImlibInit(d) Imlib_init(d)
#define ImlibFreePixmap(id,p) Imlib_free_pixmap(id,p)
#define ImlibBestColorMatch(id,r,g,b) Imlib_best_color_match(id,r,g,b)
#define ImlibRender(id,i,w,h) Imlib_render(id,i,w,h)
#define ImlibMoveImageToPixmap(id,i) Imlib_move_image(id,i)
#define ImlibMoveMaskToPixmap(id,i) Imlib_move_mask(id,i)
#define ImlibCopyImageToPixmap(id,i) Imlib_copy_image(id,i)
#define ImlibCopyMaskToPixmap(id,i) Imlib_copy_mask(id,i)
#define ImlibDestroyImage(id,i) Imlib_destroy_image(id,i)

/*
 * A workaround for people with borken sun headers.. I found this problem for
 * Solaris-x86 2.5.1 at uni....
 * uncomment this and it should help.
 * #ifdef sun
 * #define SOLARIS_SUX
 * #endif
 */

/* Some Xservers have a broken XQueryBestCursor function. You'll know this if
 * your cursor looks like  garbled mess whenever you move or resize windows
 * or if your server goes down in a ball of flames when you move or resize.
 * Report this bug to your Xserver vendors. This is a workaround. Uncomment it
 * and maybe fiddle with the width & height defines until it works. (common
 * sizes are 64x64, 32x32 and 16x16 )
 *
 * #define BROKEN_XSERVER
 * #define CURSOR_MAX_WIDTH  64
 * #define CUSROS_MAX_HEIGHT 64
 */


/*
 * Standard system includes
 */
#ifdef USE_PTHREADS
#include <pthread.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <ctype.h>
/*
 * X includes
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/extensions/shape.h>
#include <Imlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

/*
 * Constant defines
 */
#define MAX_WIDTH 2048
#define MAX_HEIGHT 1536
#define VERSION "0.13.3"

/*
 * This is where enlightenment will look if it can't find config files in
 * ./.enlightenment/ or ~/.enlightnement/
 */
#ifndef SYSCONFIG_DIR
#define SYSCONFIG_DIR "/usr/local/enlightenment/system_config/"
#endif
#ifndef THEMES_DIR
#define THEMES_DIR "/usr/local/enlightenment/themes/"
#endif

/*
 * Data structures global to enlightenment
 */
/*States for an EWin window to be in */
#define SELECTED    (1<<0)
#define ICONIFIED   (1<<1)
#define MAPPED      (1<<2)
#define UNMAPPED    (1<<3)
#define NEW_WIN     (1<<4)
#define ALL         (0xffffffff)

/*Changes to be flaged for an EWin */
#define MOD_SIZE    (1<<0)
#define MOD_TITLE   (1<<1)
#define MOD_SELECT  (1<<2)
#define MOD_STATE   (1<<3)
#define MOD_ALL     (0xffffffff)

/*Realtive pos's for EWin Subwindows */
#define POS_TL    (1<<0)
#define POS_TR    (1<<1)
#define POS_BL    (1<<2)
#define POS_BR    (1<<3)

/*Subwin states */
#define NORM 0
#define CLICKED 1

/*Text formatting defined flags*/
#define SHADOW     (1<<0)
#define OUTLINE    (1<<1)
#define J_RIGHT    (1<<2)
#define J_CENTER   (1<<3)
#define ITALIC     (1<<4)
#define BOLD       (1<<5)

/*Modes that enlightenment can be in during its event handling loop*/
#define MODE_NORMAL 0
#define MODE_MOVE   1
#define MODE_RESIZE 2
#define MODE_MENU   3

/* Symbolic Names for Keyboard/Mouse Focus Policies */

#define PointerFocus 	0
#define SloppyFocus	1
#define ClickToFocus	2

/* Timer mode constants */
#define TIMER_NONE          0
#define TIMER_INFOBOX       1
#define TIMER_ICONIFY       2
#define TIMER_AUTORAISE     3
#define TIMER_SNAPSHOTPAGER 4

/*
 * I put this is becuase the solaris X86 boxes at uni were fucked.. and this
 * was the only way to make it compile and run.
 */

#ifdef SOLARIS_SUX
typedef void (*__sighandler_t)(int);
typedef unsigned long sigset_t;
struct sigaction
{
   int sa_flags;
   void (*sa_handler)();
   sigset_t sa_mask;
   int sa_resv[2];
};
#endif

typedef struct ewn EWin;
typedef struct icn Icon;

Atom WM_STATE;

struct icn
{
   Window win;
   Pixmap pmap;
   Pixmap mask;
   EWin *ewin;
   int x;
   int y;
   int width;
   int height;
};

struct ewn
{
   Window frame_win;
   Window client_win;
   Window group_win;
   Window icon_win;
   int state;
   char title[256];
   int changes;
   Colormap colormap;
   int frame_x;
   int frame_y;
   int frame_width;
   int frame_height;
   int sizeinc_x;
   int sizeinc_y;
   int min_width;
   int min_height;
   int max_width;
   int max_height;
   int base_width;
   int base_height;
   int client_x;
   int client_y;
   int client_width;
   int client_height;
   int border_l;
   int border_r;
   int border_t;
   int border_b;

   int num_subwins;
   Window subwins[64];
   int subwin_state[64];
   Pixmap subwin_pm_clk[64];
   Pixmap subwin_pm_sel[64];
   Pixmap subwin_pm_uns[64];
   Pixmap subwin_pm_clk_mask[64];
   Pixmap subwin_pm_sel_mask[64];
   Pixmap subwin_pm_uns_mask[64];
   Pixmap mask_sel;
   Pixmap mask_uns;
   int lastop;
   int prev_frame_x;
   int prev_frame_y;
   int prev_client_width;
   int prev_client_height;
   int top;
   Icon *icon;
   struct
     {
	Window shadow_win;
     } fx;
   int desk;
   int snapshotok;
   int skip;
   Window di_win;
   int state_entry;
};

typedef struct
{
   int id;
   char params[256];
} Action;

typedef struct
{
   int shape_mode;
   int move_mode;
   int resize_mode;
   int border_l;
   int border_r;
   int border_t;
   int border_b;
   int num_subwins;
   int subwin_type[64];
   int subwin_level[64];
   int subwin_scale_method[64];
   int subwin_pos_method1[64];
   int subwin_pos_method2[64];
   int subwin_pos_x1[64];
   int subwin_pos_y1[64];
   int subwin_pos_x2[64];
   int subwin_pos_y2[64];
   Action subwin_action[64][3][4];
   char *subwin_pmname_clk[64];
   char *subwin_pmname_sel[64];
   char *subwin_pmname_uns[64];
   ImColor subwin_transp_clk[64];
   ImColor subwin_transp_sel[64];
   ImColor subwin_transp_uns[64];
   Image *subwin_img_clk[64];
   Image *subwin_img_sel[64];
   Image *subwin_img_uns[64];
   char root_pname[1024];
   int root_width;
   int root_height;
   char *font;
   int font_style;
   int font_fg;
   int font_bg;
   int autoraise;
   int autoraise_time;
   int focus_policy;
   int click_raise;
} ConfigWin;

typedef struct
{
   int root_fg_r;
   int root_fg_g;
   int root_fg_b;
   int root_bg_r;
   int root_bg_g;
   int root_bg_b;
   char *root_pix;
   char *root_mask;
   char *cursor_name;
   int doneroot;
} ECursor;

typedef struct
{
   int num_fg_r;
   int num_fg_g;
   int num_fg_b;
   int num_bg_r;
   int num_bg_g;
   int num_bg_b;
   int num_size;
   int doneroot;
} ConfigCursor;

typedef struct
{
   int mode;
   EWin *ewin;
   int wbtn;
   int pw;
   int ph;
   int px;
   int py;
   int resize_mode;
   int x1;
   int x2;
   int y1;
   int y2;
   char mname[1024];
} Event_Mode_Data;

typedef struct
{
   struct
     {
	int on;
	int x;
	int y;
	int r;
	int g;
	int b;
     } shadow;
} Fx;

/*
 * Global Variables
 */

/* for X */
extern Display *disp;
extern int screen;
extern Window root;
extern Visual *visual;
extern int depth;
extern int scr_width;
extern int scr_height;
extern Colormap root_cmap;

/* Imlib */

extern ImlibData *imd;

/*
 * Enligthenment includes
 */
#include "main.h"
#include "lists.h"
#include "draw.h"
#include "events.h"
#include "wininfo.h"
#include "buttons.h"
#include "ewin.h"
#include "loadcfg.h"
#include "root.h"
#include "text.h"
#include "status.h"
#include "alert.h"
#include "iconify.h"
#include "menus.h"
#include "actions.h"
#include "infobox.h"
#include "file.h"
#include "regexp.h"
#include "desktops.h"
#include "misc.h"
#include "keys.h"
#include "windowops.h"
#include "states.h"

/* for Enlightenment */

extern Window FocusWin;
extern Window FocusWin2;
extern ConfigWin cfg;
extern ConfigCursor ccfg;
extern Icon_cfg icfg;
extern listhead *global_l;
extern EWin *global_killewin;
extern Pixmap root_pm;
extern Event_Mode_Data evmd;
extern Window raisewin;
extern Window sel_win;
extern char font_weight[32];
extern char font_slant[32];
extern int mouse_x;
extern int mouse_y;
extern int debug_mode;
extern char *argv1;
extern struct _bl bl;
extern struct _btmd btmd;
extern struct icon_list ilist;
extern struct _newicon newicon;
extern struct scfg_ scfg;
extern struct winlist *Wlist;
extern struct winlist winlist;
extern int num_menus;
extern char Theme_Tar_Ball[1024];
extern char Theme_Path[1024];
extern char Theme_Name[1024];
extern int nodel;
extern struct menulist mlist;
extern struct menulist active_mlist;
extern Menu *tmp_menu;
extern Fx fx;
extern int predef_num;
extern int KeyBind_num;
extern int Cursors_num;
extern struct predef_icon_list *predef_list;
extern struct KeyBind_list_type *KeyBind_list;
extern ECursor *Cursors_list;
extern struct infobox ifb;
extern struct _desk desk;
extern char statefile[1024];
extern int timer_mode;
extern char AlreadyWinListed;
extern MenuItem *tmplt_mi;
extern int restart;
extern int states;
