#define CTXT_NONE            0
#define CTXT_WINDOW          1
#define CTXT_SUBWIN          2
#define CTXT_BORDER          3
#define CTXT_ROOT            4
#define CTXT_TEXT            5
#define CTXT_STATUS          6
#define CTXT_BUTTONS         7
#define CTXT_BUTTON          8
#define CTXT_ICONS           9
#define CTXT_CURSORS         10
#define CTXT_MENU            11
#define CTXT_MENUITEM        12
#define CTXT_FX              13
#define CTXT_PREDEF_ICON     14
#define CTXT_INFOBOX         15
#define CTXT_DESKTOPS        16
#define CTXT_DESKTOP         17
#define CTXT_CONTROL         18
#define CTXT_KEYBINDING      19
#define CTXT_NEWCURSOR	     20

void subvars(char *s);
void InitConfig();
void LoadConfig(char *file);

