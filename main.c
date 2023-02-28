#include "enlightenment.h"

/* this just cleans up after dead child processes.... */
void Reap() {
	int status;

	while (wait3(&status, WNOHANG, 0)>0);
}

void err(char *s) {
	fprintf(stderr,s);
}

void X_Connect(void)
{
   XWindowAttributes xwa;
   disp=XOpenDisplay(NULL); /* open the display connection */
   if (disp==NULL) /* if we couldn't open the connection.. error */
     {
	fprintf(stderr,"Enlightenment: cannot connect to Display \n");
	EExit(1); /* bad error.. we have to exit */
     }
   /* for recoverable X errors, runt his routine */
   XSetErrorHandler((XErrorHandler)handleError);
   /* for fatal X errors.. runthis routine */
   XSetIOErrorHandler((XIOErrorHandler)handleIOError);
   /* now get some imperative info about the display */
   screen=DefaultScreen(disp); /* the screen number */
   root=DefaultRootWindow(disp); /* the root window id */
   visual=DefaultVisual(disp,screen); /* the visual type */
   depth=DefaultDepth(disp,screen); /* the depth of the screen in bpp */
   scr_width=DisplayWidth(disp,screen); /* the width of the screen */
   scr_height=DisplayHeight(disp,screen); /* height of the screen */
   XSelectInput(disp,root,SubstructureNotifyMask|ButtonPressMask|
		ButtonReleaseMask|EnterWindowMask|LeaveWindowMask|
		ButtonMotionMask|PropertyChangeMask|SubstructureRedirectMask|
		KeyPressMask|KeyReleaseMask|PointerMotionMask);
   if (!debug_mode) UnmapClients(1);
   /* set up an apporpriate event mask */
   if (XGetWindowAttributes(disp,root,&xwa))
     {
	if (xwa.colormap) root_cmap=xwa.colormap;
	else root_cmap=0;
     }
   else root_cmap=0;
}

Image *LoadImage(ImlibData *d, char *file, ImColor *icl)
{
   Image *im;
   char ss[4096];
   unsigned char *ptr;
   int x,y;
   int a,b,c;
   int load_err;
   im=NULL;
   load_err=0;
   ss[0]=0;
   if (!file) return NULL;
   if (!d) return NULL;
   if (file[0]!='/')
     {
	     sprintf(ss,".enlightenment/%s",file);
	     if (!exists(ss))
	       {
		  sprintf(ss,"%s/.enlightenment/%s",getenv("HOME"),file);
		  if (!exists(ss))
		    {
		  if (Theme_Path[0])
		    sprintf(ss,"%s/%s",Theme_Path,file);
		  else
		    strcpy(ss,file);
		  if (!exists(ss))
		    {
		       Alert("Allrighty then...\nThat damned %s file does not exits!\nYou're going to be in for a nasty day!\n",file);
		       load_err=1;
		    }
	       }
	  }
     }
   else
     strcpy(ss,file);
   if (!load_err) im=Imlib_load_image(d,ss);
   if ((!im)&&(!load_err))
     Alert("Holy Carumba! Geez oh crikey!\nI couldn't for the life of me load %s\nI'll probably run screaming off in a fit of rage now...\n",ss);
   if (!im)
     {
	im=malloc(sizeof(Image));
	im->rgb_width=50;
	im->rgb_height=50;
	im->rgb_data=malloc(3*im->rgb_width*im->rgb_height);
	im->width=0;
	im->height=0;
	im->shape_color.r=-1;
	im->shape_color.g=-1;
	im->shape_color.b=-1;
	im->pixmap=0;
	im->shape_mask=0;
	ptr=im->rgb_data;
	a=rand()%256;
	b=rand()%256;
	c=rand()%256;
	for (y=0;y<im->rgb_height;y++)
	  {
	     for (x=0;x<im->rgb_width;x++)
	       {
		  ptr[0]=rand()%a;
		  ptr[1]=rand()%b;
		  ptr[2]=rand()%c;
		  ptr+=3;
	       }
	  }
     }
   if (icl) Imlib_set_image_shape(d,im,icl);
   return im;
}

void main(int argc, char **argv) {
   char s[1024];
   XEvent event;
   listhead *l;
   struct sigaction sa;
   int i,j;
   unsigned char buf[16];
   FILE *f;

   restart=0;
   imd=NULL;
   timer_mode=0;
   srand(time(NULL));
   nodel=0;
   debug_mode=0;
   Theme_Tar_Ball[0]=0;
   Theme_Name[0]=0;
   Theme_Path[0]=0;
   states=0;
   statefile[0]=0;
   AlreadyWinListed=0;
   sa.sa_handler = Reap;
   sa.sa_flags = 0;
   sigemptyset (&sa.sa_mask);
   sigaction(SIGCHLD, &sa, (struct sigaction *)0);
   /* when child processes die.. clean up after them */
   sa.sa_handler = SIG_IGN;
   sigemptyset(&sa.sa_mask);
   sigaction(SIGALRM, &sa, (struct sigaction *)0);
   /* no timer iterrupt signal yet */
   sa.sa_handler = Segfault;
   sigemptyset (&sa.sa_mask);
   sigaction(SIGSEGV, &sa, (struct sigaction *)0);
   /* Segmentation Fault.... capture it*/
   sa.sa_handler = Hup;
   sigemptyset (&sa.sa_mask);
   sigaction(SIGHUP, &sa, (struct sigaction *)0);
   /* Hup signal sent... respond to it */
   sa.sa_handler = Quit;
   sigemptyset (&sa.sa_mask);
   sigaction(SIGQUIT, &sa, (struct sigaction *)0);
   /* Quit signal... */
   sa.sa_handler = Bus;
   sigemptyset (&sa.sa_mask);
   sigaction(SIGBUS, &sa, (struct sigaction *)0);
   /* Bus Error... Wohooooo! */
   sa.sa_handler = Illegal;
   sigemptyset (&sa.sa_mask);
   sigaction(SIGILL, &sa, (struct sigaction *)0);
   /* Illegal instruction.. ??? Geez.. how did we get this one! */
   sa.sa_handler = Abort;
   sigemptyset (&sa.sa_mask);
   sigaction(SIGABRT, &sa, (struct sigaction *)0);
   /* Abort signal recieved */
   sa.sa_handler = FloatError;
   sigemptyset (&sa.sa_mask);
   sigaction(SIGFPE, &sa, (struct sigaction *)0);
   /* Floating Point Error */
   sa.sa_handler = Terminate;
   sigemptyset (&sa.sa_mask);
   sigaction(SIGTERM, &sa, (struct sigaction *)0);
   /* Terminate Signal......... */
   sa.sa_handler = User1;
   sigemptyset (&sa.sa_mask);
   sigaction(SIGUSR1, &sa, (struct sigaction *)0);
   /* User Signal 1 */
   sa.sa_handler = User2;
   sigemptyset (&sa.sa_mask);
   sigaction(SIGUSR2, &sa, (struct sigaction *)0);
   /* User signal 2 */
   for (j=1;j<argc;j++)
     {
	/* printf("%s\n",argv[j]); */
	if (!strcmp("-debug",argv[j])) debug_mode=1;
	else if (!strcmp("-restart",argv[j])) restart=1;
	else if ((!strcmp("-theme",argv[j]))&&((argc-j)>1))
	  {
	     strcpy(Theme_Tar_Ball,argv[j+1]);
	     strcpy(Theme_Name,Theme_Tar_Ball);
	     j++;
	  }
	else if ((!strcmp("-statefile",argv[j]))&&((argc-j)>1))
	  {
	     states=1;
	     strcpy(statefile,argv[j+1]);
	     j++;
	  }
     }
   if (!Theme_Tar_Ball[0])
     {
	strcpy(Theme_Tar_Ball,"DEFAULT");
	strcpy(Theme_Name,"DEFAULT");
     }
   if (Theme_Tar_Ball[0])
     {
	i=getpid();
	if (!exists(Theme_Tar_Ball))
	  {
	     sprintf(s,"%s%s",THEMES_DIR,Theme_Tar_Ball);
	     strcpy(Theme_Tar_Ball,s);
	  }
	if (exists(Theme_Tar_Ball))
	  {
	     if (isfile(Theme_Tar_Ball))
	       {
		  sprintf(Theme_Path,"/tmp/enlightenment_theme_%i",i);
		  md(Theme_Path);
		  f=fopen(Theme_Tar_Ball,"r");
		  if (f)
		    {
		       fread(&buf[0],1,1,f);
		       fread(&buf[1],1,1,f);
		       fclose (f);
		       if ((buf[0]==31)&&(buf[1]==139))
			 {
			    /*gzipped tarball*/
			    /*sprintf(s,"gzip -d -c < %s | tar -xf - -C %s",Theme_Tar_Ball,Theme_Path);*/
			    sprintf(s,"gzip -d -c < %s | (cd %s ; tar -xf -)",Theme_Tar_Ball,Theme_Path);
			 }
		       else
			 {
			    /*vanilla tarball*/
			    /*sprintf(s,"tar -xf - -C %s < %s",Theme_Path,Theme_Tar_Ball);*/
			    sprintf(s,"(cd %s ; tar -xf %s",Theme_Path,Theme_Tar_Ball);
			 }
		       Do_Exec(s);
		       wait(&i);
		    }
	       }
	     else if (isdir(Theme_Tar_Ball))
	       {
		  nodel=1;
		  strcpy(Theme_Path,Theme_Tar_Ball);
		  Theme_Tar_Ball[0]=0;
	       }
	     else
	       {
		  Theme_Path[0]=0;
		  Theme_Tar_Ball[0]=0;
	       }
	  }
	else
	  {
	     Theme_Path[0]=0;
	     Theme_Tar_Ball[0]=0;
	  }
     }
   argv1=malloc(strlen(argv[0])+1);
   strcpy(argv1, argv[0]);

   X_Connect(); /* connect to the display nominated */
   imd=ImlibInit(disp); /* Inititalise Imlib..... */
   if (!imd) /* uh oh.. problems... imlib had a bad day... */
     {
	fprintf(stderr,"FATAL.... cannot initialise Imlib!!! Outa here!\n");
	Alert("FATAL ERROR!\n Cannot initialise Imlib!\n Quitting.\n");
	EExit(1);
     }
   depth=imd->x.depth;
   visual=imd->x.visual;
   l=ListInit(); /* initialise the window list */
   WM_STATE=XInternAtom(disp,"WM_STATE", False);
   global_l=l;
   InitConfig();
   LoadConfig("MAIN");
   Kill_StatusWin();
   newicon.ewin=NULL;
   HighlightDeskInfo(0,1);
   SetRoot();
   MapDeskInfo();
   MapClients(l);
/*   if (restart) ConformAllEwinToState();*/
   restart=0;
   while(1) /* loop continuously and reacting to events */
     {
	if (global_killewin)
	  {
	     ListDelWinID(l,global_killewin->frame_win);
	     global_killewin=NULL;
	  }
	if ((ifb.on)&&(!raisewin)&&(!newicon.ewin))
	  {
	     struct sigaction sa;
	     struct itimerval tv1,tv2;

	     if (timer_mode < TIMER_INFOBOX) {
	       for (getitimer(ITIMER_REAL, &tv1);
		    tv1.it_value.tv_sec != 0 || tv1.it_value.tv_usec != 0
		      || tv1.it_interval.tv_sec != 0
		      || tv1.it_interval.tv_usec != 0;
		    getitimer(ITIMER_REAL, &tv1));
	     }
	     timer_mode = TIMER_INFOBOX;
	     sa.sa_handler = InfoBox;
	     sa.sa_flags = 0;
	     sigemptyset (&sa.sa_mask);
	     tv1.it_value.tv_sec=(unsigned long)(ifb.timeout*1000 / 1000000);
	     tv1.it_value.tv_usec=(long)(ifb.timeout*1000 % 1000000);
	     tv2.it_value.tv_sec=(unsigned long)(ifb.timeout*1000 / 1000000);
	     tv2.it_value.tv_usec=(long)(ifb.timeout*1000 % 1000000);
	     tv1.it_interval.tv_sec=0;
	     tv1.it_interval.tv_usec=0;
	     tv2.it_interval.tv_sec=0;
	     tv2.it_interval.tv_usec=0;
	     setitimer(ITIMER_REAL,&tv1,&tv2);
	     sigaction(SIGALRM,&sa,(struct sigaction *)0);
	  }
	ifb.nodo=0;
	XNextEvent(disp,&event); /* Get the next event to the display */
	ifb.nodo=1;
	XGrabServer(disp); /* grab the server.. dont let clients play around */
	handleEvent(&event,l);
	XSync(disp,0x00);
	XUngrabServer(disp);
     }
   /* EExit(0); */
}

