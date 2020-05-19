int CheckClientExists(EWin *ewin);
EWin *InitEWin(Window client);
void ModifyEWin(EWin *ewin, int nx, int ny, int nw, int nh);
void KillEWin(EWin *win);
int GetEWinButtonID(EWin *ewin, Window win);

