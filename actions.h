/* create constants for all of the possible actions */
#define ACTION_MOVE		1
#define ACTION_RESIZE		2
#define ACTION_ICONIFY		3
#define ACTION_KILL		4
#define ACTION_RAISE		5
#define ACTION_LOWER		6
#define ACTION_RAISE_LOWER	7
#define ACTION_MAX_HEIGHT	8
#define ACTION_MAX_WIDTH	9
#define ACTION_MAX_SIZE		10
#define ACTION_CONFIGURE	11
#define ACTION_MENU		12
#define ACTION_EXEC		13
#define ACTION_EXIT		14
#define ACTION_KILL_NASTY	15
#define ACTION_RESTART		16
#define ACTION_RESTART_THEME	17
#define ACTION_BACKGROUND	18
#define ACTION_DESKTOP		19
#define ACTION_STICKY           20
#define ACTION_CYCLE_NEXT       21
#define ACTION_CYCLE_PREV       22
#define ACTION_MOUSEFOCUS	23
#define ACTION_DRAG      	24
#define ACTION_NEXT_DESK	25
#define ACTION_PREV_DESK	26
#define ACTION_WINDOWOP		27
#define ACTION_MOUSECURSOR	28
#define ACTION_SKIP	        29
#define ACTION_SNAPSHOT		30
#define ACTION_UNSNAPSHOT	31
#define ACTION_SHADE		32
#define ACTION_CLICKRAISE	33
#define ACTION_NONE		255

void Do_Background(char *background);
void Do_DesktopSwitch(char *desktop);
void Do_RaiseFromMenu(char *itemtoraise);                
void DoBaseMenu(Menu *m, int x, int y);
void DoRootMenu(int btn, int mod, int x, int y);
void NextDesk();
void PrevDesk();
void Do_Exec(char *line);
void Do_Exit();
void Do_Restart();
void Do_ChangeMouseFocus(char *newfocus);
void Do_Restart_Theme(char *file);
void Do_MenuAction (char *PassedMenu, Window win);
void DoButton(BWin *bwin, int btn, int mod);
void DoMenu(Menu *m, int num);
void RunContents(int action_id, char *params);
void Draw_Cursor(EWin *ewin, int a, int b, char ch, int start);
void DoIcon(Icon *icon, int btn, int mod);
void Do_ChangeMouseCursor(char *newcursor);
