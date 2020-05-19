#define BTN_NORM  0
#define BTN_SEL   1
#define BTN_CLICK 2

typedef struct _BWin
{
   Window win;
   Pixmap unsel_pmap;
   Pixmap unsel_mask;
   Pixmap sel_pmap;
   Pixmap sel_mask;
   Pixmap click_pmap;
   Pixmap click_mask;
   int above;
   int state;
   Action action[3][4];
} BWin;

struct blist
{
   struct blist *next;
   BWin *bwin;
};

struct _bl
{
   struct blist *first;
};

struct _btmd
{
   BWin *bwin;
};

BWin *GetButtonWinID(Window w);
void AddButton(BWin *bwin);
void DelButton(Window w);
void ButtonDraw(BWin *bwin);
void MapButtons();
void RaiseLowerButtons();

