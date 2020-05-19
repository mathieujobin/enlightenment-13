#define MAX_DESKS 32

struct deskinfo
{
   int x;
   int y;
   int width;
   int height;
   int root_x;
   int root_y;
   int root_width;
   int root_height;
   int above;
   Window win;
   Window root_win;
   Pixmap bg_pmap;
   Pixmap bg_mask;
   Pixmap hl_pmap;
   Pixmap hl_mask;
   Pixmap root_pmap;
};

struct _desk
{
   int num_desks;
   int current;
   Image *im_sel;
   Image *im_unsel;
   Pixmap icon_pmap;
   Pixmap icon_mask;
   int icon_width;
   int icon_height;
   Window icon_win;
   int snapshotpager;
   char background[MAX_DESKS][1024];
   int bg_width[MAX_DESKS];
   int bg_height[MAX_DESKS];
   Pixmap bg_pmap[MAX_DESKS];
   struct deskinfo di[MAX_DESKS];
};

void InitDesks();
void ChangeToDesk(int dsk);
void MapDesk(int dsk, int map);
void MoveEwinToDesk(EWin *ewin, int dsk);
void CreateDeskInfo(int dsk, int x, int y, int w, int h, int bx, int by, int bw, int bh, char *bg, ImColor *bicl, char *hl, ImColor *hicl, int above);
void MapDeskInfo();
void HighlightDeskInfo(int dsk,int onoff);
void MimickEwin(EWin *ewin);
int WindowIsDeskInfo(Window win);
EWin *WindowIsDeskInfoWin(Window win);
void StartIconDrag(EWin *ewin);
