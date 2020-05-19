struct swlist
{
   struct swlist *next;
   Window win;
   Pixmap pmap;
   Pixmap mask;
};

struct scfg_
{
   char *icons_bg;
   ImColor *icons_icl;
   int icons_x;
   int icons_y;
   int icons_w;
   int icons_h;
   Pixmap icons_pmap;
   Pixmap icons_mask;
   Window icons_win;
   char *iconsbox_bg;
   ImColor *iconsbox_icl;
   int iconsbox_x;
   int iconsbox_y;
   int iconsbox_w;
   int iconsbox_h;
   Pixmap iconsbox_pmap;
   Pixmap iconsbox_mask;
   Window iconsbox_win;
   char *text_bg;
   ImColor *text_icl;
   int text_x;
   int text_y;
   int text_w;
   int text_h;
   Pixmap text_pmap;
   Pixmap text_mask;
   Window text_win;
   char *textbox_bg;
   ImColor *textbox_icl;
   int textbox_x;
   int textbox_y;
   int textbox_w;
   int textbox_h;
   Pixmap textbox_pmap;
   Pixmap textbox_mask;
   Window textbox_win;
   int nexticon_y;
   int nexticon_x;
   struct swlist *iconwin_list;
};

void Init_StatusWin();
void Kill_StatusWin();
void ShowStatus(char *text, char *icon, ImColor *icl);

