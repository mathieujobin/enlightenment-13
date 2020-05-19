typedef struct
{
   struct list *first;
   int num;
   struct list *last;
} listhead;

struct list
{
   struct list *next;
   EWin *win;
};

listhead *ListInit();
void ToFront(EWin *ewin);         
void ToBack(EWin *ewin);
void ListKill(listhead *l);
void ListAdd(listhead *l, EWin *data);
void ListDelWinID(listhead *l, Window w);
EWin *ListGetWinID(listhead *l, Window w);
EWin *ListGetClientWinID(listhead *l, Window w);
EWin *ListGetSubWinID(listhead *l, Window w);

