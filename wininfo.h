struct winlist
{
   Window win;
   int state;
};

#define NOEXIST -1
#define MAP      0
#define UNMAP    1
#define REDIR    2
#define REDIRM   3

void GetWinName(Window win, char *s);
void GetWinSize(Window win,EWin *ewin);
void GetWinColors(Window win,EWin *ewin);
void UnmapClients();
void MapClients(listhead *l);
