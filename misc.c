#include "enlightenment.h"

Window CreateBasicWin(Window par,int x, int y, int w, int h) {
	XSetWindowAttributes xatt;
	Window win;

	xatt.save_under=True;
	xatt.background_pixel=0;
	xatt.override_redirect=True;
	win=XCreateWindow(disp,par,x,y,w,h,0,depth,InputOutput,visual,
			CWOverrideRedirect,&xatt);
	return win;
}

Window CreateWin(Window par,int x, int y, int w, int h) {
	XSetWindowAttributes xatt;
	Window win;

	xatt.background_pixel=0;
	xatt.override_redirect=True;
	win=XCreateWindow(disp,par,x,y,w,h,0,depth,InputOutput,visual,
			CWOverrideRedirect,&xatt);
	return win;
}

void EExit(int code) {
	char s[1024];

	if ((Theme_Path[0])&&(!nodel)) {
		sprintf(s,"rm -rf %s",Theme_Path);
		system(s); 
	}
	if (code!=99) {
		XUngrabServer(disp);
		XSetInputFocus(disp,None,RevertToNone,CurrentTime);
		XSetInputFocus(disp, PointerRoot, RevertToPointerRoot, CurrentTime);
		XSelectInput(disp, root, 0);
		MapClients(global_l);
	}
	printf("Clean Exit.\n");
	exit(code);
}

void Segfault() {
	char *s;
	/* E segfaulted */
	/* I have changed this, so now E will ignore Segfaults and keep working */
	/* since you may lose work and clients if it exits.*/
	/* This is merely for robustness sake */
	s=malloc(272+sizeof(VERSION));
	sprintf(s,"Enlightenment version %s\nThe system has caused a Segmentation Fault\nThis ismost likely the result\nof a misassigned pointer or reference.\n\nIf possible, please note the steps\nthat led you to this exact segmentation fault\nand send mail\nto: mandrake@mandrake.net\n",VERSION);
	Alert(s);
	free(s);
}

void Hup() {
	/* Hup signal */
	/* When E gets a Hup signal, it will restart.. possibly a handy feature */
	printf("Got Restart Signal (SIGHUP)\n");
	Do_Restart();
}

void Quit() {
	/* Quit from Keyboard */
	/* Well, when E gets this.. it will Exit gracefully. */
	printf("Got Quit signal (SIGQUIT)\n");
	Do_Exit();
}

void Bus() {
	/* Bus Error */
	/* E encountered a Bus error..... how this happened I don't know */
	/* but E will alert the user, and continue about its business */
	Alert("Yikes! A Bus Error\n Now This is Strange!\nWell I'm ignoring it and continuing.\nWhat happens from now on may be unpredictable\n");
}

void Illegal() {
	/* Illegal instruction Executed */
	/* E executed an illegal processor instruction. This could only */
	/* happen if E is compiled for the wrong CPU I'd guess */
	Alert("Enlightenment seems to have\nexecuted an illegal processor instruction.\nPlease compile from source instead\nof using a binary distribution.\n");
}

void Abort() {
	/* Abort signal recieved */
	/* Well E will ignore abort singals too.... */
	Alert("Enlightenment has received a User Abort\nPlease Select An Option To Continue\n"); 
}

void FloatError() {
	/* Floating Point Error */
	/* Yup. a Floating Point Error.. most probably cause by a divide by 0 */
	/* Ignore it.... */
	char *s;
	s = malloc(165+sizeof(VERSION));
	sprintf(s,"Enlightenment version %s\nA Floating Point Error has occured.\nThis is most likely\nthe result of division by zero.\n\nE can most likely continue to\nrun smoothly.\n",VERSION);
	Alert(s);
	free(s);
}

void Terminate() {
	/* Termination signal recieved */
	/* Yes.. send E a terminate signal and it will.. most happily Terminate */
	printf("Got Terminate signal (SIGTERM)\n");
	Do_Exit();
}

void User1() {
	/* User defined signal 1 */
	/* Nup. E doesnt do anyhting wiht a USR1 signal... yet */
}

void User2() {
	/* User defined signal 2 */
	/* Neither does it do anything wiht a USR2 signal */
}
