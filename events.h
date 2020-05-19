struct KeyBind_list_type
{
   char *execstring;
   int action_type;
   int modifier;
   char *keycombo;
};  

XErrorHandler handleError(Display *d, XErrorEvent *ev);
XIOErrorHandler handleIOError(Display *d);
void handleEvent(XEvent *ev,listhead *l);
void handleKeyDown(XEvent *ev,listhead *l);
void handleKeyUp(XEvent *ev,listhead *l);
void handleButtonDown(XEvent *ev,listhead *l);
void handleButtonUp(XEvent *ev,listhead *l);
void handleMotion(XEvent *ev,listhead *l);
void handleEnter(XEvent *ev,listhead *l);
void handleLeave(XEvent *ev,listhead *l);
void handleFocusIn(XEvent *ev,listhead *l);
void handleFocusOut(XEvent *ev,listhead *l);
void handleDestroy(XEvent *ev,listhead *l);
void handleUnmap(XEvent *ev,listhead *l);
void handleMap(XEvent *ev,listhead *l);
void handleReparent(XEvent *ev,listhead *l);
void handleConfigure(XEvent *ev,listhead *l);
void handleClientMessage(XEvent *ev, listhead *l);
void handleResize(XEvent *ev,listhead *l);
void handleCirculate(XEvent *ev,listhead *l);
void handleProperty(XEvent *ev,listhead *l);
void AutoRaise();
void FocWin(Window tmp_win, int clk);
