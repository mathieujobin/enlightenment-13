#define OP_MAXHEIGHT 1
#define OP_MAXWIDTH 2
#define OP_MAXSIZE 3
#define OP_SHADE 4
void DoWindowButton(EWin *ewin, Window win, int btn, int mod, int wbtn);
void Do_MoveWin(EWin *ewin, Window win);
void Do_ResizeWin(EWin *ewin, Window win);
void Do_IconifyWin(EWin *ewin);
void Do_KillWin(EWin *ewin, int nasty_kill);
void CycleToNextWindow();
void CycleToPreviousWindow();
void Do_RaiseWin(EWin *ewin);
void Do_LowerWin(EWin *ewin);
void Do_RaiseLowerWin(EWin *ewin);
void Do_MaxHeightWin(EWin *ewin);
void Do_MaxWidthWin(EWin *ewin);
void Do_MaxSizeWin(EWin *ewin);
void Do_Configure();
void Do_MenuWin(EWin *ewin);
void ResizeLoop(EWin *ewin, Window win);
void MoveLoop(EWin *ewin, Window win);
void PerformWindowOp(int OpToPerform);
void DoShade(EWin *);
int ParamToInt(char *params);
