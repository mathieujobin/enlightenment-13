typedef struct
{
   int type; /* if this menu element is DECOR, only the bg_unsel are set */
   Pixmap unsel_pmap;
   Pixmap unsel_mask;
   Pixmap sel_pmap;
   Pixmap sel_mask;

   Window win;
   
   int x;
   int y;
   int width;
   int height;

   char *text;
   Action action;
} MenuItem;

typedef struct
{
   int type; /* Menu type */
   int num_items; /* number of items in the menuitme list */   
   int width; /* width of the menu */
   int height; /* height of the menu */
   char name[256]; /* name identifier of menu (up to 255 chars) */

   int popup_x;
   int popup_y;
   Window win;
   Pixmap mask;
   int sel_item;
   
   MenuItem **items; /* list of menu items */
	
   /* temp storage for static menu items on a windowlist menu */
   MenuItem **static_items;
   int static_num_items;
   int static_item_x,static_item_y;
	

   /* location of the popup when rendered live */
   int px,py;

   /* variables to store default menuitem values */

   /* default location */
   int item_x,item_y;

   /* default size */
   int item_w,item_h;

   /* default text location and size */
   int itemtext_x,itemtext_y,itemtext_w,itemtext_h;

   /* default relative location of the next item */
   int itemdelta_x,itemdelta_y;

   /* default image information */
   char *item_unsel,*item_sel;
   ImColor item_unsel_icl,item_sel_icl;
} Menu;

struct menulist
{
   struct menulist *next;
   Menu *menu;
};

/* menu types */
#define MENU_CUSTOM        0
#define MENU_WINDOWLIST	   1

/* menu element types */
#define DECOR              0
#define ELEMENT            1

void InitMenuList();
void AddMenuToList(Menu *menu);
Menu *FindMenu(char *name);
Menu *GetMenuWin(Window w);
void AddActiveMenuToList(Menu *menu);
void DeleteToActiveMenu(char *name);
Menu *GetActiveMenuWin(Window w);
Menu *CreateMenu(char *name, int type, int num_items);
void AddMenuItem(Menu *m, MenuItem* mi);
void RenderMenu(Menu *m);
MenuItem *CreateMenuItem(int type, int x, int y, int width, int height, char *text,
			int text_x, int text_y, int text_w, int text_h,
			char *unsel, ImColor *unsel_icl,
			char *sel, ImColor *sel_icl);
MenuItem *CreateMenuWinlistItem(int type, int x, int y, int width, int height, char *text, 
			int text_x, int text_y, int text_w, int text_h, 
			char *unsel, ImColor *unsel_icl,
			char *sel, ImColor *sel_icl);
void DrawMenuItem(Menu *m, int num, int state);
void ShowMenu(Menu *menu);
void HideMenu(Menu *menu);
void HideAllMenus();

