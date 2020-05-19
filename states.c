#include "enlightenment.h"

void WriteAllWindowStates() {
	char s[FILENAME_MAX];
	struct list *ptr;
	int i;

	if (restart) {
		strcpy(s,statefile);
		strcpy(statefile,".E-restart_states");
	}
	DeleteWindowStates();
	ptr=global_l->first;
	i=0;
	while (ptr) {
		AppendWindowState(ptr->win);
		ptr=ptr->next;
	}
	if (restart) 
		strcpy(statefile,s);
}

void DeleteWindowStates() {
	char s[FILENAME_MAX];
	char *home;

	if (!statefile[0]) 
		strcpy(statefile,".E-window_states");
	home=getenv("HOME");
	sprintf(s,"%s/.enlightenment",home);
	if (!exists(s)) 
		md(s);
	else if (statefile[0]=='/') 
		strcpy (s,statefile);
	else 
		sprintf(s,"%s/.enlightenment/%s",home,statefile);
	if (exists(s)) 
		rm(s);
}

void AppendWindowState(EWin *ewin) {
	char s[FILENAME_MAX];
	char ss[FILENAME_MAX];
	char *home;
	FILE *f;
	int argc;
	char **argv;
	int i;
	XClassHint xch;
	int num;

	if (!statefile[0]) 
		strcpy(statefile,".E-window_states");
	home=getenv("HOME");
	sprintf(s,"%s/.enlightenment",home);
	if (!exists(s)) 
		md(s);
	if (restart) 
		sprintf(s,"%s/.enlightenment/.E-restart_states",home);
	else if (statefile[0]=='/') 
		strcpy (s,statefile);
	else 
		sprintf(s,"%s/.enlightenment/%s",home,statefile);
	num=0;
	if (exists(s)) {
		f=fopen(s,"r");
		while (fgets(ss,sizeof(ss),f)) 
			num++;
		if (num>7) 
			num/=8;
		fclose (f);
	}
	f=fopen(s,"a");
	ewin->state_entry=num;
	if (!f) 
		return;
/* format on a line:
 * Shell_Command OR - (char)
 * Title  OR -        (char)
 * Resource_Name OR - (char)
 * Window_ID          (int)
 * X Y                (int) (int)
 * Width Height       (int) (int)
 * Desktop_Number     (int)
 * Iconified_Sate     (int: 0/1)
 */
	argv=NULL;
	argc=0;
	if (XGetCommand(disp,ewin->client_win,&argv,&argc)) {
		for (i=0;i<argc;i++) 
			if (argv[i]) 
				fprintf(f,"%s ",argv[i]);
		if (argc!=0) 
			fprintf(f,"\n");
		else 
			fprintf(f,"-\n");
		if ((argv!=NULL)&&(argc>0)) 
			XFreeStringList(argv);
	} else
		fprintf(f,"-\n");
	fprintf(f,"%s\n",ewin->title);
	xch.res_name=NULL;
	xch.res_class=NULL;
	if (XGetClassHint(disp,ewin->client_win,&xch)) {
		if (xch.res_name) 
			fprintf(f,"%s\n",xch.res_name);
		else 
			fprintf(f,"-\n");
		if (xch.res_name) 
			XFree(xch.res_name);
		if (xch.res_class) 
			XFree(xch.res_class);
	} else
		fprintf(f,"-\n");
	fprintf(f,"%i\n",(int)ewin->client_win);
	fprintf(f,"%i %i\n",ewin->frame_x,ewin->frame_y);
	fprintf(f,"%i %i\n",ewin->client_width,ewin->client_height);
	fprintf(f,"%i\n",ewin->desk);
	if (ewin->state&ICONIFIED) 
		fprintf(f,"1\n");
	else 
		fprintf(f,"0\n");
	fclose(f);
}

void ConformAllEwinToState() {
	struct list *ptr;
	int i;

	ptr=global_l->first;
	i=0;
	while (ptr) {
		ConformEwinToState(ptr->win);
		ptr=ptr->next;
	}
}

int StateEntryUsed(int num) {
	struct list *ptr;
	int i;

	if (restart) 
		return 0;

	ptr=global_l->first;
	i=0;
	while (ptr) {
		if (ptr->win->state_entry==num) 
			return 1;
		ptr=ptr->next;
	}
	return 0;
}

void DeleteEwinState(EWin *ewin) {
	char s[FILENAME_MAX];
	char s1[FILENAME_MAX];
	char ss[FILENAME_MAX];
	char *home;
	FILE *f;
	FILE *ff;
	int i;

	if (ewin->state_entry<0) 
		return;
	if (!statefile[0]) 
		strcpy(statefile,".E-window_states");
	home=getenv("HOME");
	sprintf(s,"%s/.enlightenment",home);
	if (!exists(s)) 
		md(s);
	sprintf(s1,"%s/.enlightenment/%s",home,statefile);
	f=fopen(s1,"r");
	sprintf(ss,"/tmp/enl_tmp_%i",getpid());
	ff=fopen(ss,"w");
	if (!f) 
		return;
	if (!ff) 
		return;
	for (i=0;i<(8*(ewin->state_entry));i++) {
		if (!fgets(s,sizeof(s),f)) {
			fclose(f);
			fclose (ff);
			return;
		}
		fputs(s,ff);
	}
	for (i=0;i<8;i++) {
		if (!fgets(s,sizeof(s),f)) {
			fclose(f);
			fclose (ff);
			cp(ss,s1);
			rm(ss);
			return;
		}
	}
	while(fgets(s,sizeof(s),f))
		fputs(s,ff);
	fclose (ff);
	fclose(f);
	cp(ss,s1);
	rm(ss);
}

void ModifyEwinState(EWin *ewin) {
	char s[FILENAME_MAX];
	char s1[FILENAME_MAX];
	char ss[FILENAME_MAX];
	char sss[FILENAME_MAX];
	char *home;
	FILE *f;
	FILE *ff;
	int i;

	if (ewin->state_entry<0) {
		AppendWindowState(ewin);
		return;
	}
	if (!statefile[0]) 
		strcpy(statefile,".E-window_states");
	home=getenv("HOME");
	sprintf(s,"%s/.enlightenment",home);
	if (!exists(s)) 
		md(s);
	if (statefile[0]=='/') 
		strcpy(s1,statefile);
	else 
		sprintf(s1,"%s/.enlightenment/%s",home,statefile);
	sprintf(ss,"/tmp/enl_tmp_%i",getpid());
	f=fopen(s1,"r");
	ff=fopen(ss,"w");
	if (!ff) 
		return;
	if (f) {
		for (i=0;i<(8*ewin->state_entry);i++) {
			if (!fgets(s,sizeof(s),f)) {
				fclose(f);
				i=ewin->state_entry*8+1;
				f=NULL;
			} else
				fputs(s,ff);
		}
	}
	fclose (ff);
	strcpy(sss,statefile);
	strcpy(statefile,ss);
	AppendWindowState(ewin);   
	strcpy(statefile,sss);
	ff=fopen(ss,"a");
	for (i=0;i<8;i++) {
		if (!fgets(s,sizeof(s),f)) {
			fclose(f);
			i=8;
			f=NULL;
		}
	}
	if (f) 
		while(fgets(s,sizeof(s),f)) 
			fputs(s,ff);
	fclose (ff);
	if (f) 
		fclose(f);
	cp(ss,s1);
	rm(ss);
}

void ConformEwinToState(EWin *ewin) {
	char s[FILENAME_MAX];
	char cmd[4096];
	char name[4096];
	char s_title[4096];
	char s_cmd[4096];
	char s_name[4096];
	int s_winid;
	int s_x,s_y;
	int s_width,s_height;
	int s_desk;
	int s_iconified;
	char *home;
	FILE *f;
	int argc;
	char **argv;
	int i;
	XClassHint xch;
	unsigned char match;
	int num;

	if (!statefile[0]) 
		strcpy(statefile,".E-window_states");
	home=getenv("HOME");
	sprintf(s,"%s/.enlightenment",home);
	if (!exists(s)) 
		md(s);
	if (restart) 
		sprintf(s,"%s/.enlightenment/.E-restart_states",home);
	else if (statefile[0]=='/') 
		strcpy (s,statefile);
	else 
		sprintf(s,"%s/.enlightenment/%s",home,statefile);
	f=fopen(s,"r");
	if (!f) 
		return;
	num=0;
	argv=NULL;
	argc=0;
	cmd[0]=0;
	name[0]=0;
	if (XGetCommand(disp,ewin->client_win,&argv,&argc)) {
		for (i=0;i<argc;i++) 
			if (argv[i]) 
				sprintf(cmd,"%s ",argv[i]);
			else 
				sprintf(cmd,"-");
		if ((argv!=NULL)&&(argc>0)) 
			XFreeStringList(argv);
	} else
		sprintf(cmd,"-");
	xch.res_name=NULL;
	xch.res_class=NULL;
	if (XGetClassHint(disp,ewin->client_win,&xch)) {
		if (xch.res_name) 
			sprintf(name,"%s",xch.res_name);
		else 
			sprintf(name,"-");
		if (xch.res_name) 
			XFree(xch.res_name);
		if (xch.res_class) 
			XFree(xch.res_class);
	} else
		sprintf(name,"-");
	while (1) {
		for (i=0;i<8;i++) {
			if (!fgets(s,sizeof(s),f)) {
				fclose(f);
				return;
			}
			s[strlen(s)-1]=0;
			if (i==0) 
				strcpy(s_cmd,s);
			else if (i==1) 
				strcpy(s_title,s);
			else if (i==2) 
				strcpy(s_name,s);
			else if (i==3) 
				sscanf(s,"%i",&s_winid);
			else if (i==4) 
				sscanf(s,"%i %i",&s_x,&s_y);
			else if (i==5) 
				sscanf(s,"%i %i",&s_width,&s_height);
			else if (i==6) 
				sscanf(s,"%i",&s_desk);
			else if (i==7) 
				sscanf(s,"%i",&s_iconified);
		}
		match=0;
		if (restart) {
			if (ewin->client_win==(Window)s_winid) 
				match=1;
		} else {
			if ((s_cmd[0]!='-')&&(cmd[0]!='-')) {
				if (!strcmp(s_cmd,cmd)) 
					match=1;
			} else {
				if ((s_name[0]!='-')&&(name[0]!='-')) {
					if (!strcmp(s_name,name)) 
						match=1;
				} else {
					if (!strcmp(s_title,ewin->title)) 
						match=1;
				}
			}
		}
		if ((match)&&(!StateEntryUsed(num))) {
			if (!restart) 
				ewin->state_entry=num;
			XSync(disp,False);
			ModifyEWin(ewin,s_x,s_y,s_width,s_height);
			if (s_iconified) {
				Do_IconifyWin(ewin);
				while (newicon.ewin) 
					usleep(100000); /* wait for iconification */
				ewin->desk=s_desk;
			} else {
				if (s_desk>=0) 
					MoveEwinToDesk(ewin,s_desk);
				else 
					ewin->desk=s_desk;
			}
			MimickEwin(ewin);
			XSync(disp,False);
			fclose(f);
			return;
		}
		num++;
	}
/* 	fclose(f); */
}
