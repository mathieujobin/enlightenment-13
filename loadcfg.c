#include "enlightenment.h"

void subvars(char *s)
{
   char *var;
   char ss[FILENAME_MAX];
   char s2[2];
   char name[FILENAME_MAX];
   int i,j;
   
   if (!s) return;
   ss[0]=0;
   i=0;
   s2[1]=0;
   while(s[i])
     {
	if (s[i]=='~')
	  {
	     var=getenv("HOME");
	     strcat(ss,var);
	     i++;
	  }
	else if (s[i]=='$')
	  {
	     j=0;
	     i++;
	     
	     while(isalnum(s[i])||s[i]=='_') name[j++]=s[i++];
	     name[j]=0;
	     var=getenv(name);
	     if (var) strcat(ss,var);
	     else 
	       {
		  strcat(ss,"$");
		  strcat(ss,name);
	       }
	  }
	else
	  {
	     s2[0]=s[i];
	     strcat(ss,s2);
	     i++;
	  }
     }
   strcpy(s,ss);
}

void InitConfig()
{
   int i,j,k;
   for(k=0;k<64;k++)
     {
	for(j=0;j<3;j++)
	  {
	     for(i=0;i<4;i++)
	       {
		  cfg.subwin_action[k][j][i].id=0;
		  cfg.subwin_action[k][j][i].params[0]=0;
	       }
	  }
     }
   cfg.shape_mode=0;
   cfg.move_mode=0;
   cfg.resize_mode=0;
   cfg.border_l=0;
   cfg.border_r=0;
   cfg.border_t=0;
   cfg.border_b=0;
   cfg.num_subwins=0;
   cfg.root_width=0;
   cfg.root_height=0;
   cfg.font=NULL;
   cfg.font_style=0;
   cfg.autoraise=0;
   cfg.autoraise_time=0;
   cfg.click_raise=0;
   cfg.focus_policy=PointerFocus;
   bl.first=NULL;
   for (i=0;i<64;i++)
     {
	cfg.subwin_type[i]=0;
	cfg.subwin_level[i]=0;
	cfg.subwin_scale_method[i]=0;
	cfg.subwin_pos_method1[i]=0;
	cfg.subwin_pos_method2[i]=0;
	cfg.subwin_pos_x1[i]=0;
	cfg.subwin_pos_y1[i]=0;
	cfg.subwin_pos_x2[i]=0;
	cfg.subwin_pos_y2[i]=0;
	cfg.subwin_pmname_clk[i]=NULL;
	cfg.subwin_pmname_sel[i]=NULL;
	cfg.subwin_pmname_uns[i]=NULL;
	cfg.subwin_transp_clk[i].r=-1;
	cfg.subwin_transp_clk[i].g=-1;
	cfg.subwin_transp_clk[i].b=-1;
	cfg.subwin_transp_sel[i].r=-1;
	cfg.subwin_transp_sel[i].g=-1;
	cfg.subwin_transp_sel[i].b=-1;
	cfg.subwin_transp_uns[i].r=-1;
	cfg.subwin_transp_uns[i].g=-1;
	cfg.subwin_transp_uns[i].b=-1;
     }
   evmd.mode=MODE_NORMAL;
   evmd.ewin=NULL;
   evmd.wbtn=-1;
   icfg.left_arrow_im=NULL;
   icfg.right_arrow_im=NULL;
   icfg.background_im=NULL;
   icfg.x=0;
   icfg.y=0;
   icfg.width=0;
   icfg.height=0;
   icfg.orientation=0;
   icfg.snapshot_time=0;
   icfg.x_scroll=0;
   icfg.y_scroll=0;
   icfg.level=0;
   icfg.bx=0;
   icfg.by=0;
   ccfg.num_fg_r=255;
   ccfg.num_fg_g=255;
   ccfg.num_fg_b=255; 
   ccfg.num_bg_r=0;
   ccfg.num_bg_g=0;
   ccfg.num_bg_b=0;
   ccfg.num_size=10;
   sel_win=0;
   ilist.first=NULL;
   InitMenuList();
   fx.shadow.on=0;
   predef_num=0;
   KeyBind_num=0;
   ifb.on=0;
   ifb.moveresize=1;
   ccfg.doneroot=0;
   desk.current=0;
   desk.snapshotpager=0;
   for (i=0;i<MAX_DESKS;i++)
     {
	desk.background[i][0]=0;
	desk.bg_pmap[i]=0;
	desk.di[i].win=0;
     }
   cfg.root_pname[0]=0;
   tmplt_mi=NULL;
   statefile[0]=0;
}

void LoadConfig(char *file)
{
   FILE *f;
   char s[FILENAME_MAX];
   char ss[FILENAME_MAX];
   char s1[256],s2[256],s3[256];
   char s4[256],s5[256],s6[256];
   int currentcursor=0;
   int line=0;
   int num;
   int numm;
   int num1,num2,num3,num4;
   int r,g,b;
   char *home;
   int nest_level=0;
   int nest_stack[256];
   int wait_till_end=0;
   int wait_nest_level=-1;
   int done=0;
   int i,j; /* ,k; */
   ImColor icl;
   BWin *bwin;
   int b_maxw,b_maxh;
   Image *im;
   Menu *menu;
   MenuItem *mi;
   Menu mm;
   MenuItem mmi;
   int tx_x,tx_y,tx_w,tx_h;
   char m_unsel[4096],m_sel[4096];
   ImColor icltmp1,icltmp2;
   char stmp1[4096],stmp2[4096];
   ImColor m_unsel_icl,m_sel_icl;
   struct predef_icon_list pdl;
   /* struct KeyBind_list_type kdl; */
   Pixmap pmap;
   Pixmap mask;
   int desknum;
   struct deskinfo di;
   char working_environment[300];
   
   mmi.text=NULL;
   f=0;
   if (!f)
     {
	strcpy(s1,"./.enlightenment/");
	sprintf(working_environment,"ECONFIGDIR=%s",s1);
	putenv(working_environment);
	strcat(s1,file);
	f=fopen(s1,"r");
     }
   if (!f)
     {
	home=getenv("HOME");
	if (home)
	  {
	     strcpy(s1,home);
	     strcat(s1,"/.enlightenment/");
	     sprintf(working_environment,"ECONFIGDIR=%s",s1);
		 putenv(working_environment);
	     strcat(s1,file);
	     f=fopen(s1,"r");
	  }
     }
   if (!f)
     {
	if (Theme_Path[0])
	  {
	     strcpy(s1,Theme_Path);
	     strcat(s1,"/");
	     sprintf(working_environment,"ECONFIGDIR=%s",s1);
		 putenv(working_environment);
	     strcat(s1,file);
	     f=fopen(s1,"r");
	  }
     }
   if (!f)
     {
	strcpy(s1,SYSCONFIG_DIR);
	sprintf(working_environment,"ECONFIGDIR=%s",s1);
	putenv(working_environment);
	strcat(s1,file);
	f=fopen(s1,"r");
     }
   if (!f)
     {
	sprintf(ss,"Warning, cannot find %s config file in:\n ./.enlightenment/\n ~/.enlightenment/\n %s\n.... I'm going to quit now.\n",
		file,SYSCONFIG_DIR);Alert(ss);
  	EExit(1);
     }
	{
		char working_environment2[300];
		sprintf(working_environment2,"EVERSION=%s",VERSION);
   		putenv(working_environment2);
	}
   nest_stack[0]=CTXT_NONE;
   while(fgets(s,sizeof(s),f))
     {
	line++;
        if (s[0]=='#') continue;
	if (s[0]=='\n') continue;
	if (s[0]==0) continue;
	s[strlen(s)-1]=0;
	subvars(s);
	if (s[0]=='%')
	  {
	     sscanf(s,"%s %s %s %s %s %i %i %i",s1,s2,s3,s4,s5,&num1,&num2,&num3);
	     if(!strcmp(s2, "version"))
		{
		 if(strcmp(s3,VERSION))
			 Alert("Warning: This theme was designed for %s and this is version %s of enlightenment\n",s3,VERSION);
		 continue;
		}
	     if (!strcmp(s2,"status"))
	       {
		  if (strcmp(s4,"shapemode")) continue;
		  if (!strcmp(s5,"off")) 
		    {
		       icl.r=-1;icl.g=-1;icl.b=-1;
		    }
		  else
		    {
		       icl.r=num1;icl.g=num2;icl.b=num3;
		    }
		  numm=8;
		  i=0;
		  while (numm>0)
		    {
		       if (s[i++]==' ') numm--;
		    }
		  j=0;
		  while (s[i]) ss[j++]=s[i++];
		  ss[j]=0;
		  ShowStatus(ss,s3,&icl);
		  continue;
	       }
	     else if (!strcmp(s2,"sleep"))
	       {
		  sscanf(s,"%s %s %i",s1,s2,&num);
		  sleep(num);
		  continue;
	       }
	     else if (!strcmp(s2,"black"))
	       {
		  r=0,g=0;b=0;
		  XSetWindowBackground(disp,root,ImlibBestColorMatch(imd,&r,&g,&b));
		  XClearWindow(disp,root);
		  XSync(disp,False);
		  continue;
	       }
	     else if (!strcmp(s2,"include"))
	       {
		  sscanf(s,"%s %s %s",s1,s2,s3);
		  LoadConfig(s3);
		  continue;
	       }
	     else if (!strcmp(s2,"exec"))
	       {
		  sscanf(s,"%s %s %s",s1,s2,s3);
		  strcpy(ss,strstr(s,s3));
		  Do_Exec(ss);
		  continue;
	       }
	  }
	done=0;
	sscanf(s,"%s",s1);
	if (wait_till_end)
	  {
	     done=1;
	     if (wait_nest_level==nest_level) wait_till_end=0;
	  }
	if (!strcmp(s1,"begin"))
	  {
	     done=1;
	     sscanf(s,"%s %s",s1,s2);
	     if (!strcmp(s2,"window"))
	       {
		  nest_stack[++nest_level]=CTXT_WINDOW;
	       }
	     else if (!strcmp(s2,"border"))
	       {
		  nest_stack[++nest_level]=CTXT_BORDER;
	       }
	     else if (!strcmp(s2,"subwin"))
	       {
		  nest_stack[++nest_level]=CTXT_SUBWIN;
		  cfg.num_subwins++;
	       }
	     else if (!strcmp(s2,"root"))
	       {
		  nest_stack[++nest_level]=CTXT_ROOT;
	       }
	     else if (!strcmp(s2,"text"))
	       {
		  nest_stack[++nest_level]=CTXT_TEXT;
	       }
	     else if (!strcmp(s2,"status"))
	       {
		  nest_stack[++nest_level]=CTXT_STATUS;
	       }
	     else if (!strcmp(s2,"buttons"))
	       {
		  nest_stack[++nest_level]=CTXT_BUTTONS;
	       }
	     else if (!strcmp(s2,"icons"))
	       {
		  nest_stack[++nest_level]=CTXT_ICONS;
	       }
	     else if (!strcmp(s2,"cursors"))
	       {
		  nest_stack[++nest_level]=CTXT_CURSORS;
	       }
	     else if (!strcmp(s2,"cursor"))
	       {
		  sscanf(s,"%s %s",s1,s2);
                  Cursors_num++;
                  if(!Cursors_list)
                        Cursors_list=malloc(sizeof(ECursor));
                  else
                        Cursors_list=realloc(Cursors_list,sizeof(ECursor)*Cursors_num);
		  nest_stack[++nest_level]=CTXT_NEWCURSOR;
	       }
	     else if (!strcmp(s2,"button"))
	       {
		  b_maxw=0;
		  b_maxh=0;
		  nest_stack[++nest_level]=CTXT_BUTTON;
		  bwin=malloc(sizeof(BWin));
	       }
	     else if (!strcmp(s2,"menu"))
	       {
		  nest_stack[++nest_level]=CTXT_MENU;

		  /* added 7/21/97 trp@cyberoptics.com */
		  /* initialize the menu item defaults */
		  mm.item_w = mm.item_h = 0;
		  mm.itemtext_x = mm.itemtext_y = 0;
		  mm.itemtext_w = mm.itemtext_h = 0;
		  mm.itemdelta_x = mm.itemdelta_y = 0;
		  mm.item_unsel = mm.item_sel = NULL;

		  /* initialize the first menu item position */
		  mmi.x = mmi.y = 0;
	       }
	     else if (!strcmp(s2,"menuitem"))
	       {
		  mmi.text=NULL;
		  nest_stack[++nest_level]=CTXT_MENUITEM;

		  /* added 7/21/97 trp@cyberoptics.com */
		  /* copy any default information */
		  mmi.x += mm.item_x;
		  mmi.y = mm.item_y;
		  mm.item_x += mm.itemdelta_x;
		  mm.item_y += mm.itemdelta_y;
		  mmi.width = mm.item_w;
		  mmi.height = mm.item_h;
		  tx_x = mm.itemtext_x;
		  tx_y = mm.itemtext_y;
		  tx_w = mm.itemtext_w;
		  tx_h = mm.itemtext_h;
		  if (mm.item_unsel) strcpy(m_unsel,mm.item_unsel);
		  else m_unsel[0]=0;
		  if (mm.item_sel) strcpy(m_sel,mm.item_sel);
		  else m_sel[0]=0;
		  m_unsel_icl = mm.item_unsel_icl;
		  m_sel_icl = mm.item_sel_icl;

	       }
	     else if (!strcmp(s2,"fx"))
	       {
		  nest_stack[++nest_level]=CTXT_FX;
	       }
	     else if (!strcmp(s2,"desktops"))
	       {
		  nest_stack[++nest_level]=CTXT_DESKTOPS;
	       }
	     else if (!strcmp(s2,"desktop"))
	       {
		  nest_stack[++nest_level]=CTXT_DESKTOP;
	       }
	     else if (!strcmp(s2,"control"))
	       {
		  nest_stack[++nest_level]=CTXT_CONTROL;
	       }
	     else if (!strcmp(s2,"predef_icon"))
	       {
		  nest_stack[++nest_level]=CTXT_PREDEF_ICON;
	       }
	     else if (!strcmp(s2,"infobox"))
	       {
		  nest_stack[++nest_level]=CTXT_INFOBOX;
	       }
	     else if (!strcmp(s2,"keybindings"))
	       {
		  /* added 07/21/97 Mandrake (mandrake@mandrake.net) */
		  nest_stack[++nest_level]=CTXT_KEYBINDING;
	       }
	     else
	       {
		  if (!wait_till_end)
		    {
		       sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %sSection context %s Unknown.\nIgnoring entire context chunk\n"
			       ,file,line,s,s2);Alert(ss);
		       wait_till_end=1;
		       wait_nest_level=nest_level++;
		       nest_stack[nest_level]=CTXT_NONE;
		    }
		  else
		    {
		       nest_level++;
		    }
	       }
	  }
	
	else if (!strcmp(s1,"end"))
	  {
	     if (nest_stack[nest_level]==CTXT_TEXT) 
	       InitTextInfo();
	     if (nest_stack[nest_level]==CTXT_STATUS) 
	       Init_StatusWin();
	     if (nest_stack[nest_level]==CTXT_BUTTON) 
	       AddButton(bwin);
	     if (nest_stack[nest_level]==CTXT_ICONS) 
	       InitIcons();
	     if (nest_stack[nest_level]==CTXT_MENUITEM) 
	       AddMenuItem(menu,mi);
	     if (nest_stack[nest_level]==CTXT_INFOBOX) 
	       InitInfoBox();
             if (nest_stack[nest_level]==CTXT_DESKTOP)
	       {
		  CreateDeskInfo(desknum,di.root_x,di.root_y,
				 di.root_width,di.root_height,
				 di.x,di.y,di.width,di.height,
				 stmp2,&icltmp2,stmp1,&icltmp1,di.above);
	       }
             if (nest_stack[nest_level]==CTXT_DESKTOPS)
	       InitDesks();
	     if (nest_stack[nest_level]==CTXT_PREDEF_ICON) 
	       {
		  predef_num++;
		  if (!predef_list) 
		    predef_list=malloc(sizeof(struct predef_icon_list));
		  else
		    predef_list=realloc(predef_list,sizeof(struct predef_icon_list)*predef_num);
		  predef_list[predef_num-1].title=pdl.title;
		  predef_list[predef_num-1].image_file=pdl.image_file;
		  predef_list[predef_num-1].transp=pdl.transp;
	       }
	     if (nest_stack[nest_level]==CTXT_MENU)
	       {
		  RenderMenu(menu);
		  AddMenuToList(menu);

		  mm.item_unsel=NULL;
		  mm.item_sel=NULL;
	       }
	     done=1;
	     if (nest_level>0)
	       {
		  nest_level--;
	       }
	     else
	       {
		  nest_level=0;
		  sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nKeyword end with no matching begin.\nIgnoring Keyword.\n"
			  ,file,line,s);Alert(ss);
	       }
	  }
	if (!wait_till_end)
	  {
	     if (strcmp(s1,"begin")){
	     if(nest_stack[nest_level]==CTXT_WINDOW)
	       {
		  if (!strcmp(s1,"shaped"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"on")) cfg.shape_mode=1;
		       else if (!strcmp(s2,"off")) cfg.shape_mode=0;
		       else 
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "shaped",s2,
				    "on,off",
				    "off");Alert(ss);
			    cfg.shape_mode=0;
			 }
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_SUBWIN)
	       {
		  if (!strcmp(s1,"type"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"decoration")) cfg.subwin_type[cfg.num_subwins-1]=0;
		       else if (!strcmp(s2,"button")) cfg.subwin_type[cfg.num_subwins-1]=1;
		       else if (!strcmp(s2,"title")) cfg.subwin_type[cfg.num_subwins-1]=2;
		       else 
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "type",s2,
				    "decoration,button,title",
				    "decoration");Alert(ss);
			    cfg.subwin_type[cfg.num_subwins-1]=0;
			 }
		    }
		  else if (!strcmp(s1,"coord1_relative"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"topleft")) cfg.subwin_pos_method1[cfg.num_subwins-1]=0;
		       else if (!strcmp(s2,"topright")) cfg.subwin_pos_method1[cfg.num_subwins-1]=1;
		       else if (!strcmp(s2,"bottomleft")) cfg.subwin_pos_method1[cfg.num_subwins-1]=2;
		       else if (!strcmp(s2,"bottomright")) cfg.subwin_pos_method1[cfg.num_subwins-1]=3;
		       else 
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "coor_relative",s2,
				    "topleft,topright,bottomleft,bottomright",
				    "topleft");Alert(ss);
			    cfg.subwin_pos_method1[cfg.num_subwins-1]=0;
			 }
		    }
		  else if (!strcmp(s1,"coord1"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i",s1,&num,&numm);
		       cfg.subwin_pos_x1[cfg.num_subwins-1]=num;
		       cfg.subwin_pos_y1[cfg.num_subwins-1]=numm;
		    }
		  else if (!strcmp(s1,"coord2_relative"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"topleft")) cfg.subwin_pos_method2[cfg.num_subwins-1]=0;
		       else if (!strcmp(s2,"topright")) cfg.subwin_pos_method2[cfg.num_subwins-1]=1;
		       else if (!strcmp(s2,"bottomleft")) cfg.subwin_pos_method2[cfg.num_subwins-1]=2;
		       else if (!strcmp(s2,"bottomright")) cfg.subwin_pos_method2[cfg.num_subwins-1]=3;
		       else 
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "coor_relative",s2,
				    "topleft,topright,bottomleft,bottomright",
				    "topleft");Alert(ss);
			    cfg.subwin_pos_method2[cfg.num_subwins-1]=0;
			 }
		    }
		  else if (!strcmp(s1,"coord2"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i",s1,&num,&numm);
		       cfg.subwin_pos_x2[cfg.num_subwins-1]=num;
		       cfg.subwin_pos_y2[cfg.num_subwins-1]=numm;
		    }
		  else if (!strcmp(s1,"image"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"clicked")) 
			 {
			    sscanf(s,"%s %s %s %s %s %i %i %i",s1,s2,s3,s4,s5,&num1,&num2,&num3);
			    cfg.subwin_pmname_clk[cfg.num_subwins-1]=malloc(sizeof(char)*(strlen(s3)+1));
			    strcpy(cfg.subwin_pmname_clk[cfg.num_subwins-1],s3);
			    if (!strcmp(s4,"shapecolor"))
			      {
				 if (!strcmp(s5,"on"))
				   {
				      cfg.subwin_transp_clk[cfg.num_subwins-1].r=num1;
				      cfg.subwin_transp_clk[cfg.num_subwins-1].g=num2;
				      cfg.subwin_transp_clk[cfg.num_subwins-1].b=num3;
				   }
				 else if (!strcmp(s5,"off"))
				   {
				      cfg.subwin_transp_clk[cfg.num_subwins-1].r=-1;
				      cfg.subwin_transp_clk[cfg.num_subwins-1].g=-1;
				      cfg.subwin_transp_clk[cfg.num_subwins-1].b=-1;
				   }
				 else
				   {
				      sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					      "image shapecolor",s5,
					      "on,off",
					      "off");Alert(ss);
				      cfg.subwin_transp_clk[cfg.num_subwins-1].r=-1;
				      cfg.subwin_transp_clk[cfg.num_subwins-1].g=-1;
				      cfg.subwin_transp_clk[cfg.num_subwins-1].b=-1;
				   }
			      }
			    else
			      {
				 sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					 "image",s4,
					 "shapecolor",
					 "chapecolor off");Alert(ss);
				 cfg.subwin_transp_clk[cfg.num_subwins-1].r=-1;
				 cfg.subwin_transp_clk[cfg.num_subwins-1].g=-1;
				 cfg.subwin_transp_clk[cfg.num_subwins-1].b=-1;
			      }
			 }
		       else if (!strcmp(s2,"selected")) 
			 {
			    sscanf(s,"%s %s %s %s %s %i %i %i",s1,s2,s3,s4,s5,&num1,&num2,&num3);
			    cfg.subwin_pmname_sel[cfg.num_subwins-1]=malloc(sizeof(char)*(strlen(s3)+1));
			    strcpy(cfg.subwin_pmname_sel[cfg.num_subwins-1],s3);
			    if (!strcmp(s4,"shapecolor"))
			      {
				 if (!strcmp(s5,"on"))
				   {
				      cfg.subwin_transp_sel[cfg.num_subwins-1].r=num1;
				      cfg.subwin_transp_sel[cfg.num_subwins-1].g=num2;
				      cfg.subwin_transp_sel[cfg.num_subwins-1].b=num3;
				   }
				 else if (!strcmp(s5,"off"))
				   {
				      cfg.subwin_transp_sel[cfg.num_subwins-1].r=-1;
				      cfg.subwin_transp_sel[cfg.num_subwins-1].g=-1;
				      cfg.subwin_transp_sel[cfg.num_subwins-1].b=-1;
				   }
				 else
				   {
				      sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					      "image shapecolor",s5,
					      "on,off",
					      "off");Alert(ss);
				      cfg.subwin_transp_sel[cfg.num_subwins-1].r=-1;
				      cfg.subwin_transp_sel[cfg.num_subwins-1].g=-1;
				      cfg.subwin_transp_sel[cfg.num_subwins-1].b=-1;
				   }
			      }
			    else
			      {
				 sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					 "image",s4,
					 "shapecolor",
					 "shapecolor off");Alert(ss);
				 cfg.subwin_transp_sel[cfg.num_subwins-1].r=-1;
				 cfg.subwin_transp_sel[cfg.num_subwins-1].g=-1;
				 cfg.subwin_transp_sel[cfg.num_subwins-1].b=-1;
			      }
			 }
		       else if (!strcmp(s2,"unselected")) 
			 {
			    sscanf(s,"%s %s %s %s %s %i %i %i",s1,s2,s3,s4,s5,&num1,&num2,&num3);
			    cfg.subwin_pmname_uns[cfg.num_subwins-1]=malloc(sizeof(char)*(strlen(s3)+1));
			    strcpy(cfg.subwin_pmname_uns[cfg.num_subwins-1],s3);
			    if (!strcmp(s4,"shapecolor"))
			      {
				 if (!strcmp(s5,"on"))
				   {
				      cfg.subwin_transp_uns[cfg.num_subwins-1].r=num1;
				      cfg.subwin_transp_uns[cfg.num_subwins-1].g=num2;
				      cfg.subwin_transp_uns[cfg.num_subwins-1].b=num3;
				   }
				 else if (!strcmp(s5,"off"))
				   {
				      cfg.subwin_transp_uns[cfg.num_subwins-1].r=-1;
				      cfg.subwin_transp_uns[cfg.num_subwins-1].g=-1;
				      cfg.subwin_transp_uns[cfg.num_subwins-1].b=-1;
				   }
				 else
				   {
				      sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					      "image shapecolor",s5,
					      "on,off",
					      "off");Alert(ss);
				      cfg.subwin_transp_uns[cfg.num_subwins-1].r=-1;
				      cfg.subwin_transp_uns[cfg.num_subwins-1].g=-1;
				      cfg.subwin_transp_uns[cfg.num_subwins-1].b=-1;
				   }
			      }
			    else
			      {
				 sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					 "image",s4,
					 "shapecolor",
					 "shapecolor off");Alert(ss);
				 cfg.subwin_transp_uns[cfg.num_subwins-1].r=-1;
				 cfg.subwin_transp_uns[cfg.num_subwins-1].g=-1;
				 cfg.subwin_transp_uns[cfg.num_subwins-1].b=-1;
			      }
			 }
		       else if (!strcmp(s2,"scale_method")) 
			 {
			    sscanf(s,"%s %s %s",s1,s2,s3);
			    if (!strcmp(s3,"tile")) cfg.subwin_scale_method[cfg.num_subwins-1]=0;
			    else if (!strcmp(s3,"scaletile")) cfg.subwin_scale_method[cfg.num_subwins-1]=1;
			    else if (!strcmp(s3,"scale")) cfg.subwin_scale_method[cfg.num_subwins-1]=2;
			    else 
			      {
				 sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					 "scale_method",s3,
					 "tile,scaletime,scale",
					 "tile");Alert(ss);
				 cfg.subwin_scale_method[cfg.num_subwins-1]=0;
			      }
			 }
		       else 
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "image detail",s2,
				    "clicked,selected,unselected,scale_method",
				    "NOTHING");Alert(ss);
			 }
		    }
		  else if (!strcmp(s1,"level"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"above")) cfg.subwin_level[cfg.num_subwins-1]=1;
		       else if (!strcmp(s2,"below")) cfg.subwin_level[cfg.num_subwins-1]=0;
		       else 
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "level",s2,
				    "above,below",
				    "below");Alert(ss);
			    cfg.subwin_level[cfg.num_subwins-1]=0;
			 }
		    }
		  else if (!strcmp(s1,"action"))
		    {
		       done=1;
		       sscanf(s,"%s %s %i %s %s",s1,s2,&num,s3,s4);
		       if ((num>0)&&(num<4))
			 {
			    if (!strcmp(s2,"none")) 
			      {
				 numm=0;
			      }
			    else if (!strcmp(s2,"shift")) 
			      {
				 numm=1;
			      }
			    else if (!strcmp(s2,"ctrl")) 
			      {
				 numm=2;
			      }
			    else if (!strcmp(s2,"alt")) 
			      {
				 numm=3;
			      }
			    else 
			      {
				 sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					 "action modifier",s2,
					 "none,shift,ctrl,alt",
					 "none");Alert(ss);
				 numm=0;
			      }
			    if (!strcmp(s3,"move")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_MOVE;
			      }
			    else if (!strcmp(s3,"resize")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_RESIZE;
			      }
			    else if (!strcmp(s3,"iconify")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_ICONIFY;
			      }
			    else if (!strcmp(s3,"kill")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_KILL;
			      }
			    else if (!strcmp(s3,"raise")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_RAISE;
			      }
			    else if (!strcmp(s3,"lower")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_LOWER;
			      }
			    else if (!strcmp(s3,"raise_lower")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_RAISE_LOWER;
			      }
			    else if (!strcmp(s3,"max_height")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_MAX_HEIGHT;
			      }
			    else if (!strcmp(s3,"max_width")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_MAX_WIDTH;
			      }
			    else if (!strcmp(s3,"max_size")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_MAX_SIZE;
			      }
			    else if (!strcmp(s3,"configure")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_CONFIGURE;
			      }
			    else if (!strcmp(s3,"menu")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_MENU;
				 strcpy(cfg.subwin_action[cfg.num_subwins-1][num-1][numm].params,s4);
			      }
			    else if (!strcmp(s3,"exec")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_EXEC;
				 strcpy(cfg.subwin_action[cfg.num_subwins-1][num-1][numm].params,strstr(s,s4));
			      }
			    else if (!strcmp(s3,"exit")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_EXIT;
			      }
			    else if (!strcmp(s3,"kill_nasty")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_KILL_NASTY;
			      }
			    else if (!strcmp(s3,"sticky")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_STICKY;
			      }
			    else if (!strcmp(s3,"drag")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_DRAG;
			      }
			    else if (!strcmp(s3,"snapshot")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_SNAPSHOT;
			      }
			    else if (!strcmp(s3,"unsnapshot")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_UNSNAPSHOT;
			      }
			    else if(!strcmp(s3,"shade"))
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_SHADE;
			      }
			    else if (!strcmp(s3,"windowop"))
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_WINDOWOP;
				 strcpy(cfg.subwin_action[cfg.num_subwins-1][num-1][numm].params,strstr(s,s4));
			      }
			    else if (!strcmp(s3,"none")) 
			      {
				 cfg.subwin_action[cfg.num_subwins-1][num-1][numm].id=ACTION_NONE;
			      }
			    else
			      {
				 sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					 "action",s3,
					 "move,resize,iconify,kill,raise,lower,raise_lower,max_height,max_width,max_size,configure,menu,exec,kill_nasty,sticky,none",
					 "NOTHING");Alert(ss);
			      }
			 }
		       else
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "action button ","!",
				    "1,2,3",
				    "NOTHING");Alert(ss);
			 }
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_BORDER)
	       {
		  if (!strcmp(s1,"left"))
		    {
		       done=1;
		       sscanf(s,"%s %i",s1,&num);
		       cfg.border_l=num;
		    }
		  else if (!strcmp(s1,"right"))
		    {
		       done=1;
		       sscanf(s,"%s %i",s1,&num);
		       cfg.border_r=num;
		    }
		  else if (!strcmp(s1,"top"))
		    {
		       done=1;
		       sscanf(s,"%s %i",s1,&num);
		       cfg.border_t=num;
		    }
		  else if (!strcmp(s1,"bottom"))
		    {
		       done=1;
		       sscanf(s,"%s %i",s1,&num);
		       cfg.border_b=num;
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_ICONS)
	       {
		  if (!strcmp(s1,"location"))
		    {
		       done=1;
		       sscanf(s,"%s %s %i %i",s1,s2,&num1,&num2);
		       if (!strcmp(s2,"topleft"))
			 {
			    icfg.x=num1;
			    icfg.y=num2;
			 }
		       else if (!strcmp(s2,"bottomleft"))
			 {
			    icfg.x=num1;
			    icfg.y=DisplayHeight(disp,screen)-num2-icfg.height;
			 }
		       else if (!strcmp(s2,"topright"))
			 {
			    icfg.x=DisplayWidth(disp,screen)-num1-icfg.width;
			    icfg.y=num2;
			 }
		       else if (!strcmp(s2,"bottomright"))
			 {
			    icfg.x=DisplayWidth(disp,screen)-num1-icfg.width;
			    icfg.y=DisplayHeight(disp,screen)-num2-icfg.height;
			 }
			 
		    }
		  else if (!strcmp(s1,"size"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i",s1,&num1,&num2);
		       icfg.width=num1;
		       icfg.height=num2;
		    }
		  else if (!strcmp(s1,"border"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i",s1,&num1,&num2);
		       icfg.bx=num1;
		       icfg.by=num2;
		    }
		  else if (!strcmp(s1,"scroll_speed"))
		    {
		       done=1;
		       sscanf(s,"%s %i",s1,&num);
		       icfg.scroll_speed=num;
		    }
		  else if (!strcmp(s1,"snapshot_time"))
		    {
		       done=1;
		       sscanf(s,"%s %i",s1,&num);
		       icfg.snapshot_time=num;
		    }
		  else if (!strcmp(s1,"orientation"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"vertical")) icfg.orientation=1;
		       else icfg.orientation=0;
		    }
		  else if (!strcmp(s1,"level"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"above")) icfg.level=1;
		       else icfg.level=0;
		    }
		  else if (!strcmp(s1,"image"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"background")) 
			 {
			    sscanf(s,"%s %s %s",s1,s2,s3);
			    icfg.background_im=malloc(sizeof(char)*(strlen(s3)+1));
			    strcpy(icfg.background_im,s3);
			 }
		       else if (!strcmp(s2,"left"))
			 {
			    sscanf(s,"%s %s %s %s %s %s %i %i %i",s1,s2,s3,s4,s5,s6,&num1,&num2,&num3);
			    if (!strcmp(s3,"unselected"))
			      {
				 icfg.left_arrow_im=malloc(sizeof(char)*(strlen(s4)+1));
				 strcpy(icfg.left_arrow_im,s4);
				 if (!strcmp(s5,"shapemode"))
				   {
				      if (!strcmp(s6,"on"))
					{
					   icfg.left_transparent.r=num1;
					   icfg.left_transparent.g=num2;
					   icfg.left_transparent.b=num3;
					}
				      else if (!strcmp(s6,"off"))
					{
					   icfg.left_transparent.r=-1;
					   icfg.left_transparent.g=-1;
					   icfg.left_transparent.b=-1;
					}
				      else
					{
					   sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
						   "image shapecolor",s5,
						   "on,off",
						   "off");Alert(ss);
					   icfg.left_transparent.r=-1;
					   icfg.left_transparent.g=-1;
					   icfg.left_transparent.b=-1;
					}
				   }
				 else
				   {
				      sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					      "image",s4,
					      "shapemode",
					      "shapemode off");Alert(ss);
				      icfg.left_transparent.r=-1;
				      icfg.left_transparent.g=-1;
				      icfg.left_transparent.b=-1;
				   }
			      }
			    else if (!strcmp(s3,"selected"))
			      {
				 icfg.left_sel_arrow_im=malloc(sizeof(char)*(strlen(s4)+1));
				 strcpy(icfg.left_sel_arrow_im,s4);
				 if (!strcmp(s5,"shapemode"))
				   {
				      if (!strcmp(s6,"on"))
					{
					   icfg.left_sel_transparent.r=num1;
					   icfg.left_sel_transparent.g=num2;
					   icfg.left_sel_transparent.b=num3;
					}
				      else if (!strcmp(s6,"off"))
					{
					   icfg.left_sel_transparent.r=-1;
					   icfg.left_sel_transparent.g=-1;
					   icfg.left_sel_transparent.b=-1;
					}
				      else
					{
					   sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
						   "image shapecolor",s5,
						   "on,off",
						   "off");Alert(ss);
					   icfg.left_sel_transparent.r=-1;
					   icfg.left_sel_transparent.g=-1;
					   icfg.left_sel_transparent.b=-1;
					}
				   }
				 else
				   {
				      sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					      "image",s4,
					      "shapemode",
					      "shapemode off");Alert(ss);
				      icfg.left_sel_transparent.r=-1;
				      icfg.left_sel_transparent.g=-1;
				      icfg.left_sel_transparent.b=-1;
				   }
			      }
			    else if (!strcmp(s3,"clicked"))
			      {
				 icfg.left_clk_arrow_im=malloc(sizeof(char)*(strlen(s4)+1));
				 strcpy(icfg.left_clk_arrow_im,s4);
				 if (!strcmp(s5,"shapemode"))
				   {
				      if (!strcmp(s6,"on"))
					{
					   icfg.left_clk_transparent.r=num1;
					   icfg.left_clk_transparent.g=num2;
					   icfg.left_clk_transparent.b=num3;
					}
				      else if (!strcmp(s6,"off"))
					{
					   icfg.left_clk_transparent.r=-1;
					   icfg.left_clk_transparent.g=-1;
					   icfg.left_clk_transparent.b=-1;
					}
				      else
					{
					   sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
						   "image shapecolor",s5,
						   "on,off",
						   "off");Alert(ss);
					   icfg.left_clk_transparent.r=-1;
					   icfg.left_clk_transparent.g=-1;
					   icfg.left_clk_transparent.b=-1;
					}
				   }
				 else
				   {
				      sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					      "image",s4,
					      "shapemode",
					      "shapemode off");Alert(ss);
				      icfg.left_clk_transparent.r=-1;
				      icfg.left_clk_transparent.g=-1;
				      icfg.left_clk_transparent.b=-1;
				   }
			      }
			 }
		       else if (!strcmp(s2,"right"))
			 {
			    sscanf(s,"%s %s %s %s %s %s %i %i %i",s1,s2,s3,s4,s5,s6,&num1,&num2,&num3);
			    if (!strcmp(s3,"unselected"))
			      {
				 icfg.right_arrow_im=malloc(sizeof(char)*(strlen(s4)+1));
				 strcpy(icfg.right_arrow_im,s4);
				 if (!strcmp(s5,"shapemode"))
				   {
				      if (!strcmp(s6,"on"))
					{
					   icfg.right_transparent.r=num1;
					   icfg.right_transparent.g=num2;
					   icfg.right_transparent.b=num3;
					}
				      else if (!strcmp(s6,"off"))
					{
					   icfg.right_transparent.r=-1;
					   icfg.right_transparent.g=-1;
					   icfg.right_transparent.b=-1;
					}
				      else
					{
					   sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
						   "image shapecolor",s5,
						   "on,off",
						   "off");Alert(ss);
					   icfg.right_transparent.r=-1;
					   icfg.right_transparent.g=-1;
					   icfg.right_transparent.b=-1;
					}
				   }
				 else
				   {
				      sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					      "image",s4,
					      "shapemode",
					      "shapemode off");Alert(ss);
				      icfg.right_transparent.r=-1;
				      icfg.right_transparent.g=-1;
				      icfg.right_transparent.b=-1;
				   }
			      }
			    else if (!strcmp(s3,"selected"))
			      {
				 icfg.right_sel_arrow_im=malloc(sizeof(char)*(strlen(s4)+1));
				 strcpy(icfg.right_sel_arrow_im,s4);
				 if (!strcmp(s5,"shapemode"))
				   {
				      if (!strcmp(s6,"on"))
					{
					   icfg.right_sel_transparent.r=num1;
					   icfg.right_sel_transparent.g=num2;
					   icfg.right_sel_transparent.b=num3;
					}
				      else if (!strcmp(s6,"off"))
					{
					   icfg.right_sel_transparent.r=-1;
					   icfg.right_sel_transparent.g=-1;
					   icfg.right_sel_transparent.b=-1;
					}
				      else
					{
					   sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
						   "image shapecolor",s5,
						   "on,off",
						   "off");Alert(ss);
					   icfg.right_sel_transparent.r=-1;
					   icfg.right_sel_transparent.g=-1;
					   icfg.right_sel_transparent.b=-1;
					}
				   }
				 else
				   {
				      sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					      "image",s4,
					      "shapemode",
					      "shapemode off");Alert(ss);
				      icfg.right_sel_transparent.r=-1;
				      icfg.right_sel_transparent.g=-1;
				      icfg.right_sel_transparent.b=-1;
				   }
			      }
			    else if (!strcmp(s3,"clicked"))
			      {
				 icfg.right_clk_arrow_im=malloc(sizeof(char)*(strlen(s4)+1));
				 strcpy(icfg.right_clk_arrow_im,s4);
				 if (!strcmp(s5,"shapemode"))
				   {
				      if (!strcmp(s6,"on"))
					{
					   icfg.right_clk_transparent.r=num1;
					   icfg.right_clk_transparent.g=num2;
					   icfg.right_clk_transparent.b=num3;
					}
				      else if (!strcmp(s6,"off"))
					{
					   icfg.right_clk_transparent.r=-1;
					   icfg.right_clk_transparent.g=-1;
					   icfg.right_clk_transparent.b=-1;
					}
				      else
					{
					   sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
						   "image shapecolor",s5,
						   "on,off",
						   "off");Alert(ss);
					   icfg.right_clk_transparent.r=-1;
					   icfg.right_clk_transparent.g=-1;
					   icfg.right_clk_transparent.b=-1;
					}
				   }
				 else
				   {
				      sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					      "image",s4,
					      "shapemode",
					      "shapemode off");Alert(ss);
				      icfg.right_clk_transparent.r=-1;
				      icfg.right_clk_transparent.g=-1;
				      icfg.right_clk_transparent.b=-1;
				   }
			      }
			 }
		    }
		  else if (!strcmp(s1,"action"))
		    {
		       done=1;
		       sscanf(s,"%s %s %i %s %s",s1,s2,&num,s3,s4);
		       if ((num>0)&&(num<4))
			 {
			    if (!strcmp(s2,"none")) 
			      {
				 numm=0;
			      }
			    else if (!strcmp(s2,"shift")) 
			      {
				 numm=1;
			      }
			    else if (!strcmp(s2,"ctrl")) 
			      {
				 numm=2;
			      }
			    else if (!strcmp(s2,"alt")) 
			      {
				 numm=3;
			      }
			    else 
			      {
				 sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					 "action modifier",s2,
					 "none,shift,ctrl,alt",
					 "none");Alert(ss);
				 numm=0;
			      }
			    if (!strcmp(s3,"iconify")) 
			      {
				 icfg.action[num-1][numm].id=ACTION_ICONIFY;
			      }
			    else if (!strcmp(s3,"kill")) 
			      {
				 icfg.action[num-1][numm].id=ACTION_KILL;
			      }
			    else if (!strcmp(s3,"configure")) 
			      {
				 icfg.action[num-1][numm].id=ACTION_CONFIGURE;
			      }
			    else if (!strcmp(s3,"menu")) 
			      {
				 icfg.action[num-1][numm].id=ACTION_MENU;
				 strcpy(icfg.action[num-1][numm].params,s4);
			      }
			    else if (!strcmp(s3,"exec")) 
			      {
				 icfg.action[num-1][numm].id=ACTION_EXEC;
				 strcpy(icfg.action[num-1][numm].params,strstr(s,s4));
			      }
			    else if (!strcmp(s3,"kill_nasty")) 
			      {
				 icfg.action[num-1][numm].id=ACTION_KILL_NASTY;
			      }
			    else if (!strcmp(s3,"drag")) 
			      {
				 icfg.action[num-1][numm].id=ACTION_DRAG;
			      }
			    else if (!strcmp(s3,"windowop"))
			      {
				 icfg.action[num-1][numm].id=ACTION_WINDOWOP;
				 strcpy(icfg.action[num-1][numm].params,strstr(s,s4));
			      }
			    else if (!strcmp(s3,"none")) 
			      {
				 icfg.action[num-1][numm].id=ACTION_NONE;
			      }
			    else
			      {
				 sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					 "action",s3,
					 "iconify,configure,menu,exec,kill_nasty,none",
					 "NOTHING");Alert(ss);
			      }
			 }
		       else
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "action button ","!",
				    "1,2,3",
				    "NOTHING");Alert(ss);
			 }
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_ROOT)
	       {
		  if (!strcmp(s1,"backgroundimage"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i %s",s1,&num,&numm,s2);
		       desk.bg_width[0]=num;
		       desk.bg_height[0]=numm;
		       strcpy(desk.background[0],s2);
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_CONTROL)
	       {
		  if (!strcmp(s1,"move"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"box")) cfg.move_mode=0;
		       else if (!strcmp(s2,"shape")) cfg.move_mode=1;
		       else if (!strcmp(s2,"opaque")) cfg.move_mode=2;
		       else 
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "move",s2,
				    "box,shape,opaque",
				    "box");Alert(ss);
			    cfg.move_mode=0;
			 }
		    }
		  else if (!strcmp(s1,"snapshotpager"))
		    {
			done=1;
			sscanf(s,"%s %s",s1,s2);
			if(!strcmp(s2,"on")) {
				desk.snapshotpager=1;
			} else {
				desk.snapshotpager=0;
			}
		    }
		  else if (!strcmp(s1,"autoraise"))
		    {
		       done=1;
		       sscanf(s,"%s %s %i",s1,s2,&num);
		       cfg.autoraise_time=num;
		       if (!strcmp(s2,"on")) cfg.autoraise=1;
		       else if (!strcmp(s2,"off")) cfg.autoraise=0;
		       else 
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "autoraise",s2,
				    "on,off",
				    "off");Alert(ss);
			    cfg.autoraise=0;
			 }
		    }
		  else if (!strcmp(s1,"statesfile"))
		    {
		       done=1;
		       sscanf(s,"%s %s %s",s1,s2,s3);
		       if (!states)
			 {
			    if (!strcmp(s2,"on")) states=1;
			    strcmp(statefile,s3);
			 }
		    }
		  else if (!strcmp(s1,"focus"))
		    {
			done=1;
			sscanf(s,"%s %s",s1,s2);
			if(!strcmp(s2,"sloppy")) cfg.focus_policy=SloppyFocus;
			else if(!strcmp(s2,"click")) cfg.focus_policy=ClickToFocus;
			else if(!strcmp(s2,"pointer")) cfg.focus_policy=PointerFocus;
			else {
				sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed [%s]\n Assuming %s.\n",file,line,s,"focus",s2,"pointer,sloppy,click","pointer");Alert(ss);
				cfg.focus_policy=PointerFocus;
			}
			}
		  else if (!strcmp(s1,"click_raise"))
		  {
			  done=1;
			  sscanf(s,"%s %s",s1,s2);
			  if(!strcmp(s2,"on")) cfg.click_raise=1;

		    }
		  else if (!strcmp(s1,"resize"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"box")) cfg.resize_mode=0;
		       else if (!strcmp(s2,"shape")) cfg.resize_mode=1;
		       else if (!strcmp(s2,"opaque")) cfg.resize_mode=2;
		       else 
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "resize",s2,
				    "box,shape,opaque",
				    "box");Alert(ss);
			    cfg.resize_mode=0;
			 }
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_DESKTOPS)
	       {
		  if (!strcmp(s1,"image"))
		    {
		       done=1;
		       sscanf(s,"%s %s %s %s %s %i %i %i",s1,s2,s3,s4,s5,&num1,&num2,&num3);
		       if (!strcmp(s2,"selected")) 
			 {
			    if (!strcmp(s5,"on"))
			      {icltmp1.r=num1;icltmp1.g=num2;icltmp1.b=num3;}
			    else
			      {icltmp1.r=-1;icltmp1.g=-1;icltmp1.b=-1;}
			    desk.im_sel=LoadImage(imd,s3,&icltmp1);
			 }
		       else if (!strcmp(s2,"unselected")) 
			 {
			    if (!strcmp(s5,"on"))
			      {icltmp1.r=num1;icltmp1.g=num2;icltmp1.b=num3;}
			    else
			      {icltmp1.r=-1;icltmp1.g=-1;icltmp1.b=-1;}
			    desk.im_unsel=LoadImage(imd,s3,&icltmp1);
			 }
		       else if (!strcmp(s2,"drag")) 
			 {
			    if (!strcmp(s5,"on"))
			      {icltmp1.r=num1;icltmp1.g=num2;icltmp1.b=num3;}
			    else
			      {icltmp1.r=-1;icltmp1.g=-1;icltmp1.b=-1;}
			    LoadImageWithImlib(s3,&icltmp1,&desk.icon_pmap,&desk.icon_mask,&desk.icon_width,&desk.icon_height);
			 }
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_DESKTOP)
	       {
		  if (!strcmp(s1,"number"))
		    {
		       done=1;
		       sscanf(s,"%s %i",s1,&num);
		       desknum=num;
		    }
		  if (!strcmp(s1,"level"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"above")) di.above=1;
		       else di.above=0;
		    }
		  else if (!strcmp(s1,"backgroundimage"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i %s",s1,&num,&numm,s2);
		       desk.bg_width[desknum]=num;
		       desk.bg_height[desknum]=numm;
		       strcpy(desk.background[desknum],s2);
		    }
		  else if (!strcmp(s1,"root_geom"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i %i %i",s1,&num,&num1,&num2,&num3);
		       di.root_x=num;di.root_y=num1;
		       di.root_width=num2;di.root_height=num3;
		    }
		  else if (!strcmp(s1,"border_geom"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i %i %i",s1,&num,&num1,&num2,&num3);
		       di.x=num;di.y=num1;
		       di.width=num2;di.height=num3;
		    }
		  else if (!strcmp(s1,"image"))
		    {
		       done=1;
		       sscanf(s,"%s %s %s %s %s %i %i %i",s1,s2,s3,s4,s5,&num1,&num2,&num3);
		       if (!strcmp(s2,"selected")) 
			 {
			    strcpy(stmp1,s3);
			    if (!strcmp(s5,"on"))
			      {icltmp1.r=num1;icltmp1.g=num2;icltmp1.b=num3;}
			    else
			      {icltmp1.r=-1;icltmp1.g=-1;icltmp1.b=-1;}
			 }
		       else if (!strcmp(s2,"unselected")) 
			 {
			    strcpy(stmp2,s3);
			    if (!strcmp(s5,"on"))
			      {icltmp2.r=num1;icltmp2.g=num2;icltmp2.b=num3;}
			    else
			      {icltmp2.r=-1;icltmp2.g=-1;icltmp2.b=-1;}
			 }
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_INFOBOX)
	       {
		  if (!strcmp(s1,"status"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"on")) ifb.on=1;
		       else ifb.on=0;
		    }
		  else if (!strcmp(s1,"size"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i",s1,&num1,&num2);
		       ifb.width=num1;
		       ifb.height=num2;
		    }
		  else if (!strcmp(s1,"key_width"))
		    {
		       done=1;
		       sscanf(s,"%s %i",s1,&num);
		       ifb.key_width=num;
		    }
		  else if (!strcmp(s1,"mouse_width"))
		    {
		       done=1;
		       sscanf(s,"%s %i",s1,&num);
		       ifb.mouse_width=num;
		    }
		  else if (!strcmp(s1,"timeout"))
		    {
		       done=1;
		       sscanf(s,"%s %i",s1,&num);
		       ifb.timeout=num;
		    }
		  else if (!strcmp(s1,"image"))
		    {
		       sscanf(s,"%s %s %s %i %i %i",s1,s2,s3,&num1,&num2,&num3);
		       icl.r=num1;
		       icl.g=num2;
		       icl.b=num3;
		       if (!strcmp(s2,"shift"))
			 {
			    done=1;
			    LoadImageSizeWithImlib(s3,&icl,&pmap,&mask,ifb.key_width,ifb.height);
			    ifb.k2_pmap=pmap;
			    ifb.k2_mask=mask;
			 }
		       if (!strcmp(s2,"ctrl"))
			 {
			    done=1;
			    LoadImageSizeWithImlib(s3,&icl,&pmap,&mask,ifb.key_width,ifb.height);
			    ifb.k3_pmap=pmap;
			    ifb.k3_mask=mask;
			 }
		       if (!strcmp(s2,"alt"))
			 {
			    done=1;
			    LoadImageSizeWithImlib(s3,&icl,&pmap,&mask,ifb.key_width,ifb.height);
			    ifb.k4_pmap=pmap;
			    ifb.k4_mask=mask;
			 }
		       if (!strcmp(s2,"mouse1"))
			 {
			    done=1;
			    LoadImageSizeWithImlib(s3,&icl,&pmap,&mask,ifb.mouse_width,ifb.height);
			    ifb.m1_pmap=pmap;
			    ifb.m1_mask=mask;
			 }
		       if (!strcmp(s2,"mouse2"))
			 {
			    done=1;
			    LoadImageSizeWithImlib(s3,&icl,&pmap,&mask,ifb.mouse_width,ifb.height);
			    ifb.m2_pmap=pmap;
			    ifb.m2_mask=mask;
			 }
		       if (!strcmp(s2,"mouse3"))
			 {
			    done=1;
			    LoadImageSizeWithImlib(s3,&icl,&pmap,&mask,ifb.mouse_width,ifb.height);
			    ifb.m3_pmap=pmap;
			    ifb.m3_mask=mask;
			 }
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_KEYBINDING) 
	       {
		  s5[0] = 0;
		  sscanf(s,"%s %s %s %s %s",s1,s2,s3,s4,s5);
		  if (!strcmp(s1,"key"))
		    {
		       done=1;
		       KeyBind_num++;
		  /* read the keybind config in here */
		  /* added 07/21/97 Mandrake (mandrake@mandrake.net) */
		       if (!KeyBind_list) 
			 KeyBind_list=malloc(sizeof(struct KeyBind_list_type));
		       else
			 KeyBind_list=realloc(KeyBind_list,sizeof(struct KeyBind_list_type)*KeyBind_num);
		       
		       KeyBind_list[KeyBind_num-1].keycombo=malloc(strlen(s3)+1);
		       strcpy(KeyBind_list[KeyBind_num-1].keycombo,s3);
		       
		       if (!strcmp(s2,"none"))
			 KeyBind_list[KeyBind_num-1].modifier=0;
		       else if (!strcmp(s2,"shift"))
			 KeyBind_list[KeyBind_num-1].modifier=1;
		       else if (!strcmp(s2,"ctrl"))
			 KeyBind_list[KeyBind_num-1].modifier=2;
		       else if (!strcmp(s2,"alt"))
			 KeyBind_list[KeyBind_num-1].modifier=3;
		       
		       if (!strcmp(s4,"move"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_MOVE;
		       else if (!strcmp(s4,"resize"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_RESIZE;
		       else if (!strcmp(s4,"iconify"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_ICONIFY;
		       else if (!strcmp(s4,"kill"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_KILL;
		       else if (!strcmp(s4,"raise"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_RAISE;
		       else if (!strcmp(s4,"lower"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_LOWER;
		       else if (!strcmp(s4,"raise_lower"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_RAISE_LOWER;
		       else if (!strcmp(s4,"max_height"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_MAX_HEIGHT;
		       else if (!strcmp(s4,"max_width"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_MAX_WIDTH;
		       else if (!strcmp(s4,"max_size"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_MAX_SIZE;
		       else if (!strcmp(s4,"configure"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_CONFIGURE;
		       else if (!strcmp(s4,"menu"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_MENU;
		       else if (!strcmp(s4,"exec"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_EXEC;
		       else if (!strcmp(s4,"exit"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_EXIT;
		       else if (!strcmp(s4,"kill_nasty"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_KILL_NASTY;
		       else if (!strcmp(s4,"restart"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_RESTART;
		       else if (!strcmp(s4,"restart_theme"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_RESTART_THEME;
		       else if (!strcmp(s4,"background"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_BACKGROUND;
		       else if (!strcmp(s4,"desktop"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_DESKTOP;
		       else if (!strcmp(s4,"sticky"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_STICKY;
                       else if (!strcmp(s4,"mfocus"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_MOUSEFOCUS;
                       else if (!strcmp(s4,"windowop"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_WINDOWOP;
			   else if (!strcmp(s4,"click_raise"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_CLICKRAISE;
		       else if ((!strcmp(s4,"cycle"))&&(!strcmp(s5,"next")))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_CYCLE_NEXT;
		       else if ((!strcmp(s4,"cycle"))&&(!strcmp(s5,"previous")))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_CYCLE_PREV;

		       else if ((!strcmp(s4,"cycle"))&&(!strcmp(s5,"nextdesk")))
                         KeyBind_list[KeyBind_num-1].action_type=ACTION_NEXT_DESK;
                       else if ((!strcmp(s4,"cycle"))&&(!strcmp(s5,"previousdesk")))
                         KeyBind_list[KeyBind_num-1].action_type=ACTION_PREV_DESK;      

		       else if (!strcmp(s4,"none"))
			 KeyBind_list[KeyBind_num-1].action_type=ACTION_NONE;
		       num=KeyBind_list[KeyBind_num-1].action_type;
		       if ((num==ACTION_MENU)||(num==ACTION_EXEC)||
			   (num==ACTION_RESTART_THEME)||(num==ACTION_BACKGROUND)||
			   (num==ACTION_DESKTOP)||(num==ACTION_WINDOWOP))
			 {
			    KeyBind_list[KeyBind_num-1].execstring=malloc(strlen(s)+1);
			    strcpy(KeyBind_list[KeyBind_num-1].execstring,strstr(s,s5));
			 }
		       else if (num==ACTION_MOUSEFOCUS) {
				if(s5[0]) {
			    		KeyBind_list[KeyBind_num-1].execstring=malloc(strlen(s)+1);
			    		strcpy(KeyBind_list[KeyBind_num-1].execstring,strstr(s,s5));
				} else {
					KeyBind_list[KeyBind_num-1].execstring=NULL;
				}
			 }
		       else
			 KeyBind_list[KeyBind_num-1].execstring=NULL;
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_PREDEF_ICON)
	       {
		  if (!strcmp(s1,"title"))
		    {
		       int ii,jj;
		       int copy;
		       
		       done=1;
		       copy=0;
		       jj=0;
		       pdl.title=malloc(strlen(s));
		       for (ii=0;ii<strlen(s);ii++)
			 {
			    if (!copy)
			      {
				 if (s[ii]=='"') copy=1;
			      }
			    else
			      {
				 if (s[ii]=='"') copy=0;
				 else pdl.title[jj++]=s[ii];
			      }
			 }
		       pdl.title[jj]=0;
		    }
		  if (!strcmp(s1,"image"))
		    {
		       done=1;
		       sscanf(s,"%s %s %s %s %i %i %i",s1,s2,s3,s4,&num1,&num2,&num3);
		       pdl.image_file=malloc(strlen(s2)+1);
		       strcpy(pdl.image_file,s2);
		       if (!strcmp("on",s4))
			 {
			    pdl.transp=malloc(sizeof(ImColor));
			    pdl.transp->r=num1;
			    pdl.transp->g=num2;
			    pdl.transp->b=num3;
			 }
		       else
			 pdl.transp=NULL;
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_FX)
	       {
		  if (!strcmp(s1,"shadow"))
		    {
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"state"))
			 {
			    done=1;
			    sscanf(s,"%s %s %s",s1,s2,s3);
			    if (!strcmp(s3,"on")) fx.shadow.on=1;
			 }
		       if (!strcmp(s2,"dist"))
			 {
			    done=1;
			    sscanf(s,"%s %s %i %i",s1,s2,&num1,&num2);
			    fx.shadow.x=num1;
			    fx.shadow.y=num2;
			 }
		       if (!strcmp(s2,"color"))
			 {
			    done=1;
			    sscanf(s,"%s %s %i %i %i",s1,s2,&num1,&num2,&num3);
			    fx.shadow.r=num1;
			    fx.shadow.g=num2;
			    fx.shadow.b=num3;
			 }
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_CURSORS)
	       {

		       sscanf(s,"%s",s2);
		       if (!strcmp(s2,"fg_color"))
			 {
			    sscanf(s,"%s %i %i %i",s2,&num1,&num2,&num3);
			    done=1;
			    ccfg.num_fg_r=num1;
			    ccfg.num_fg_g=num2;
			    ccfg.num_fg_b=num3;
			 }
		       else if (!strcmp(s2,"bg_color"))
			 {
			    sscanf(s,"%s %i %i %i",s2,&num1,&num2,&num3);
			    done=1;
			    ccfg.num_bg_r=num1;
			    ccfg.num_bg_g=num2;
			    ccfg.num_bg_b=num3;
			 }
		       else if (!strcmp(s2,"font_size"))
			 {
			    sscanf(s,"%s %i",s2,&num);
			    ccfg.num_size=num;
			    done=1;
			 }
		}
	     if(nest_stack[nest_level]==CTXT_NEWCURSOR)
		{
		   sscanf(s,"%s",s1);
		   if(!strcmp(s1,"type")) {
			sscanf(s,"%s %s",s1,s2);
			done=1;
		        Cursors_list[Cursors_num-1].cursor_name=malloc(strlen(s2)+1);
		        strcpy(Cursors_list[Cursors_num-1].cursor_name,s2);
		        currentcursor++;
		   }
		   else if(!strcmp(s1,"fg_color")) {
			sscanf(s,"%s %i %i %i",s2,&num1,&num2,&num3);
			done=1;
			Cursors_list[currentcursor-1].root_fg_r=num1;
			Cursors_list[currentcursor-1].root_fg_g=num2;
			Cursors_list[currentcursor-1].root_fg_b=num3;
		   } else if (!strcmp(s1,"bg_color")) {
			sscanf(s,"%s %i %i %i",s2,&num1,&num2,&num3);
			done=1;
			Cursors_list[currentcursor-1].root_bg_r=num1;
			Cursors_list[currentcursor-1].root_bg_g=num2;
			Cursors_list[currentcursor-1].root_bg_b=num3;
		   } else if(!strcmp(s1,"pixmap")) {
			sscanf(s,"%s %s",s2,s1);
			done=1;
			Cursors_list[currentcursor-1].root_pix=malloc(strlen(s1)+1);
			strcpy(Cursors_list[currentcursor-1].root_pix,s1);
		   } else if(!strcmp(s1,"mask")) {
			sscanf(s,"%s %s",s2,s1);
			done=1;
			Cursors_list[currentcursor-1].root_mask=malloc(strlen(s1)+1);
			strcpy(Cursors_list[currentcursor-1].root_mask,s1);
		   }
	       }
	     if(nest_stack[nest_level]==CTXT_TEXT)
	       {
		  if (!strcmp(s1,"fontface"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       cfg.font=malloc(sizeof(char)*(strlen(s2)+1));
		       strcpy(cfg.font,s2);
		    }
		  else if (!strcmp(s1,"style"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"bold")) cfg.font_style|=BOLD;
		       if (!strcmp(s2,"italic")) cfg.font_style|=ITALIC;
		    }
		  else if (!strcmp(s1,"appearance"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"plain")) ;
		       if (!strcmp(s2,"outline")) cfg.font_style|=OUTLINE;
		       if (!strcmp(s2,"shadow")) cfg.font_style|=SHADOW;
		    }
		  else if (!strcmp(s1,"justification"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"left")) ;
		       if (!strcmp(s2,"center")) cfg.font_style|=J_CENTER;
		       if (!strcmp(s2,"right")) cfg.font_style|=J_RIGHT;
		    }
		  else if (!strcmp(s1,"foreground"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i %i",s1,&num1,&num2,&num3);
		       cfg.font_fg=ImlibBestColorMatch(imd,&num1,&num2,&num3);
		    }
		  else if (!strcmp(s1,"background"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i %i",s1,&num1,&num2,&num3);
		       cfg.font_bg=ImlibBestColorMatch(imd,&num1,&num2,&num3);
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_STATUS)
	       {
		  if (!strcmp(s1,"icons"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"background"))
			 {
			    sscanf(s,"%s %s %i %i %i %i %s %s %s %i %i %i",s1,s1,&num1,&num2,&num3,&num4,s2,s3,s4,&r,&g,&b);
			    scfg.icons_x=num1;
			    scfg.icons_y=num2;
			    scfg.icons_w=num3;
			    scfg.icons_h=num4;
			    scfg.icons_bg=malloc(strlen(s2)+1);
			    strcpy(scfg.icons_bg,s2);
			    if (!strcmp(s4,"off")) scfg.icons_icl=NULL;
			    else
			      {
				 scfg.icons_icl=malloc(sizeof(ImColor));
				 scfg.icons_icl->r=r;
				 scfg.icons_icl->g=g;
				 scfg.icons_icl->b=b;
			      }
			 }
		       else if (!strcmp(s2,"box"))
			 {
			    sscanf(s,"%s %s %i %i %i %i %s %s %s %i %i %i",s1,s1,&num1,&num2,&num3,&num4,s2,s3,s4,&r,&g,&b);
			    scfg.iconsbox_x=num1;
			    scfg.iconsbox_y=num2;
			    scfg.iconsbox_w=num3;
			    scfg.iconsbox_h=num4;
			    scfg.iconsbox_bg=malloc(strlen(s2)+1);
			    strcpy(scfg.iconsbox_bg,s2);
			    if (!strcmp(s4,"off")) scfg.iconsbox_icl=NULL;
			    else
			      {
				 scfg.iconsbox_icl=malloc(sizeof(ImColor));
				 scfg.iconsbox_icl->r=r;
				 scfg.iconsbox_icl->g=g;
				 scfg.iconsbox_icl->b=b;
			      }
			 }
		    }
		  else if (!strcmp(s1,"text"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"background"))
			 {
			    sscanf(s,"%s %s %i %i %i %i %s %s %s %i %i %i",s1,s1,&num1,&num2,&num3,&num4,s2,s3,s4,&r,&g,&b);
			    scfg.text_x=num1;
			    scfg.text_y=num2;
			    scfg.text_w=num3;
			    scfg.text_h=num4;
			    scfg.text_bg=malloc(strlen(s2)+1);
			    strcpy(scfg.text_bg,s2);
			    if (!strcmp(s4,"off")) scfg.text_icl=NULL;
			    else
			      {
				 scfg.text_icl=malloc(sizeof(ImColor));
				 scfg.text_icl->r=r;
				 scfg.text_icl->g=g;
				 scfg.text_icl->b=b;
			      }
			 }
		       else if (!strcmp(s2,"box"))
			 {
			    sscanf(s,"%s %s %i %i %i %i %s %s %s %i %i %i",s1,s1,&num1,&num2,&num3,&num4,s2,s3,s4,&r,&g,&b);
			    scfg.textbox_x=num1;
			    scfg.textbox_y=num2;
			    scfg.textbox_w=num3;
			    scfg.textbox_h=num4;
			    scfg.textbox_bg=malloc(strlen(s2)+1);
			    strcpy(scfg.textbox_bg,s2);
			    if (!strcmp(s4,"off")) scfg.textbox_icl=NULL;
			    else
			      {
				 scfg.textbox_icl=malloc(sizeof(ImColor));
				 scfg.textbox_icl->r=r;
				 scfg.textbox_icl->g=g;
				 scfg.textbox_icl->b=b;
			      }
			 }
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_MENU)
	       {
		  if (!strcmp(s1,"name"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       strncpy(mm.name,s2,255);
		    }
		  else if (!strcmp(s1,"type"))
		    {
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"custom"))
			 {
			    done=1;
			    mm.type=MENU_CUSTOM;
			 }
                      else if (!strcmp(s2,"windowlist"))
                        {
                           done=1;
                           mm.type=MENU_WINDOWLIST;
                        }
                      else
                      {
                          sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s.\nAllowed: [%s]\n Assuming %s.\n",
                                  file,line,s,
                                   "menu type ",
                                   "custom,windowlist",
                                   "custom");Alert(ss);
                           done=1;
                           mm.type=MENU_CUSTOM;
                      }
		    }
		  else if (!strcmp(s1,"dimensions"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i",s1,&num1,&num2);
		       mm.width=num1;
		       mm.height=num2;
		    }
		  else if (!strcmp(s1,"popupdist"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i",s1,&num1,&num2);
		       mm.popup_x=num1;
		       mm.popup_y=num2;
		    }
		  else if (!strcmp(s1,"items"))
		    {
		       done=1;
		       sscanf(s,"%s %i",s1,&num);
		       menu=CreateMenu(mm.name,mm.type,num);
		       menu->width=mm.width;
		       menu->height=mm.height;
		       menu->popup_x=mm.popup_x;
		       menu->popup_y=mm.popup_y;

			/* copy the item defaults */
			menu->item_x = mm.item_x;
			menu->item_y = mm.item_y;
                       menu->item_w = mm.item_w;
                       menu->item_h = mm.item_h;
                       menu->itemtext_x = mm.itemtext_x;
                       menu->itemtext_y = mm.itemtext_y;
                       menu->itemtext_w = mm.itemtext_w;
                       menu->itemtext_h = mm.itemtext_h;
                       menu->itemdelta_x = mm.itemdelta_x;
                       menu->itemdelta_y = mm.itemdelta_y;
                       menu->item_unsel = mm.item_unsel;
                       menu->item_sel = mm.item_sel;
                       menu->item_unsel_icl = mm.item_unsel_icl;
                       menu->item_sel_icl = mm.item_sel_icl;

			/* added 07/24/97 by Troy Pesola (trp@cyberoptics.com) */
			/* initialize the number of static items */
			menu->static_num_items = 0;
			menu->static_items=NULL;
                    }
                  else if (!strcmp(s1,"itemsize"))
                    {                                            
                       done=1;
                       sscanf(s,"%s %i %i",s1,&num1,&num2);
                       mm.item_w=num1;
                       mm.item_h=num2;
                    }
                  else if (!strcmp(s1,"itemdelta"))
                    {
                       done=1;
                       sscanf(s,"%s %i %i",s1,&num1,&num2);
                       mm.itemdelta_x=num1;
                       mm.itemdelta_y=num2;
                    }
                  else if (!strcmp(s1,"itemtextlocation"))
                    {
                       done=1;
                       sscanf(s,"%s %i %i",s1,&num1,&num2);
                       mm.itemtext_x=num1;
                       mm.itemtext_y=num2;
                    }
                  else if (!strcmp(s1,"itemtextsize"))
                    {
                       done=1;
                       sscanf(s,"%s %i %i",s1,&num1,&num2);
                       mm.itemtext_w=num1;
                       mm.itemtext_h=num2;
                    }
                  else if (!strcmp(s1,"itemimage"))
                    {
                       done=1;
                       s1[0]=0;s2[0]=0;s3[0]=0;s4[0]=0;s5[0]=0;
                       sscanf(s,"%s %s %s %s %s %i %i %i",s1,s2,s3,s4,s5,&num1,&num2,&num3);
                       if ((s1[0])&&(s2[0])&&(s3[0])&&(s4[0])&&(s5[0]))
                         {
                            if (!strcmp(s2,"unselected"))
                              {
                                 if (!mm.item_unsel) mm.item_unsel=malloc(4096);
                                 strcpy(mm.item_unsel,s3);
                                 if (!strcmp(s5,"on"))
                                   {
                                      mm.item_unsel_icl.r=num1;
                                      mm.item_unsel_icl.g=num2;
                                      mm.item_unsel_icl.b=num3;
                                   }
                                 else
                                   {
                                      mm.item_unsel_icl.r=-1;
                                      mm.item_unsel_icl.g=-1;
   				      mm.item_unsel_icl.b=-1;
                                   }
                              }
                            else if (!strcmp(s2,"selected"))
                              {
                                 if (!mm.item_sel) mm.item_sel=malloc(4096);
                                 strcpy(mm.item_sel,s3);
                                 if (!strcmp(s5,"on"))
                                   {
                                      mm.item_sel_icl.r=num1;
                                      mm.item_sel_icl.g=num2;
                                      mm.item_sel_icl.b=num3;
                                   }
                                 else
                                   {
                                      mm.item_sel_icl.r=-1;
                                      mm.item_sel_icl.g=-1;
                                      mm.item_sel_icl.b=-1;
                                   }
                              }
                         }
                    }
               }  
	     if(nest_stack[nest_level]==CTXT_MENUITEM)
	       {
		  if (!strcmp(s1,"text"))
		    {
		       done=1;
		       s2[0]=0;
		       sscanf(s,"%s %s",s1,s2);
		       if (s2[0])
			 {
			    mmi.text=malloc(strlen(s)+1);
			    strcpy(mmi.text,strstr(s,s2));
			 }
		    }
		  else if (!strcmp(s1,"type"))
		    {
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"decor")) 
			 {
			    done=1;
			    mmi.type=DECOR;
			 }
		       else if (!strcmp(s2,"element")) 
			 {
			    done=1;
			    mmi.type=ELEMENT;
			 }
		    }
		  else if (!strcmp(s1,"location"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i",s1,&num1,&num2);
		       mmi.x=num1;
		       mmi.y=num2;
		    }
		  else if (!strcmp(s1,"size"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i",s1,&num1,&num2);
		       mmi.width=num1;
		       mmi.height=num2;
		    }
		  else if (!strcmp(s1,"text_location"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i",s1,&num1,&num2);
		       tx_x=num1;
		       tx_y=num2;
		    }
		  else if (!strcmp(s1,"text_size"))
		    {
		       done=1;
		       sscanf(s,"%s %i %i",s1,&num1,&num2);
		       tx_w=num1;
		       tx_h=num2;
		    }
		  else if (!strcmp(s1,"image"))
		    {
		       done=1;
		       s1[0]=0;s2[0]=0;s3[0]=0;s4[0]=0;s5[0]=0;
		       sscanf(s,"%s %s %s %s %s %i %i %i",s1,s2,s3,s4,s5,&num1,&num2,&num3);
		       if ((s1[0])&&(s2[0])&&(s3[0])&&(s4[0])&&(s5[0]))
			 {
			    if (!strcmp(s2,"unselected"))
			      {
				 strcpy(m_unsel,s3);
				 if (!strcmp(s5,"on"))
				   {
				      m_unsel_icl.r=num1;
				      m_unsel_icl.g=num2;
				      m_unsel_icl.b=num3;
				   }
				 else
				   {
				      m_unsel_icl.r=-1;
				      m_unsel_icl.g=-1;
				      m_unsel_icl.b=-1;
				   }
			      }
			    else if (!strcmp(s2,"selected"))
			      {
				 strcpy(m_sel,s3);
				 if (!strcmp(s5,"on"))
				   {
				      m_sel_icl.r=num1;
				      m_sel_icl.g=num2;
				      m_sel_icl.b=num3;
				   }
				 else
				   {
				      m_sel_icl.r=-1;
				      m_sel_icl.g=-1;
				      m_sel_icl.b=-1;
				   }
			      }
			 }
		    }
		  else if (!strcmp(s1,"action"))
		    {
		       done=1;
		       mi=CreateMenuItem(mmi.type,mmi.x,mmi.y,mmi.width,
					 mmi.height,mmi.text,tx_x,tx_y,
					 tx_w,tx_h,m_unsel,&m_unsel_icl,
					 m_sel,&m_sel_icl);
		       if (mmi.text) 
			 {
			    free(mmi.text);
			    mmi.text=NULL;
			 }
		       s3[0] = 0;
		       sscanf(s,"%s %s %s",s1,s2,s3);
		       if (!strcmp(s2,"configure")) 
			 {
			    mi->action.id=ACTION_CONFIGURE;
			 }
		       else if (!strcmp(s2,"menu")) 
			 {
			    mi->action.id=ACTION_MENU;
			    strcpy(mi->action.params,s3);
			 }
		       else if (!strcmp(s2,"windowop")) 
			 {
			    mi->action.id=ACTION_WINDOWOP;
			    strcpy(mi->action.params,s3);
			 }
		       else if (!strcmp(s2,"mfocus"))
			 {
			    mi->action.id=ACTION_MOUSEFOCUS;
			    if(s3[0]) strcpy(mi->action.params,s3);
			 }
			   else if(!strcmp(s2,"click_raise"))
			   {
				   mi->action.id=ACTION_CLICKRAISE;
			   }
		       else if (!strcmp(s2,"exec")) 
			 {
			    mi->action.id=ACTION_EXEC;
			    strcpy(mi->action.params,strstr(s,s3));
			 }
		       else if (!strcmp(s2,"exit")) 
			 {
			    mi->action.id=ACTION_EXIT;
			 }
		       else if (!strcmp(s2,"restart")) 
			 {
			    mi->action.id=ACTION_RESTART;
			 }
		       else if (!strcmp(s2,"restart_theme")) 
			 {
			    strcpy(mi->action.params,strstr(s,s3));
			    mi->action.id=ACTION_RESTART_THEME;
			 }
		       else if (!strcmp(s2,"background")) 
			 {
			    strcpy(mi->action.params,strstr(s,s3));
			    mi->action.id=ACTION_BACKGROUND;
			 }
		       else if (!strcmp(s2,"desktop")) 
			 {
			    strcpy(mi->action.params,s3);
			    mi->action.id=ACTION_DESKTOP;
			 }
		       else if (!strcmp(s2,"none")) 
			 {
			    mi->action.id=ACTION_NONE;
			 }
		    }
	       }
	     if(nest_stack[nest_level]==CTXT_BUTTONS)
	       {
	       }
	     if(nest_stack[nest_level]==CTXT_BUTTON)
	       {
		  if (!strcmp(s1,"location"))
		    {
		       done=1;
		       sscanf(s,"%s %s %i %i",s1,s2,&num,&numm);
		       if (!strcmp(s2,"topleft"))
			 bwin->win=CreateBasicWin(root,num,numm,b_maxw,b_maxh);
		       else if (!strcmp(s2,"topright"))
			 bwin->win=CreateBasicWin(root,(DisplayWidth(disp,screen)-b_maxw)-num,numm,
						       b_maxw,b_maxh);
		       else if (!strcmp(s2,"bottomleft"))
			 bwin->win=CreateBasicWin(root,num,(DisplayHeight(disp,screen)-b_maxh)-numm,
						       b_maxw,b_maxh);
		       else if (!strcmp(s2,"bottomright"))
			 bwin->win=CreateBasicWin(root,(DisplayWidth(disp,screen)-b_maxw)-num,(DisplayHeight(disp,screen)-b_maxh)-numm,
						       b_maxw,b_maxh);
		       else
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "button location",s2,
				    "topleft,topright,bottomleft,bottomright",
				    "topleft");Alert(ss);
			    bwin->win=CreateBasicWin(root,num,numm,b_maxw,b_maxh);
			 }
		       XSelectInput(disp,bwin->win,LeaveWindowMask|
				    EnterWindowMask|SubstructureNotifyMask|
				    PropertyChangeMask|SubstructureRedirectMask);
		       bwin->state=BTN_NORM;
		       ButtonDraw(bwin);
		    }
		  else if (!strcmp(s1,"image"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"clicked")) 
			 {
			    sscanf(s,"%s %s %s %s %s %i %i %i",s1,s2,s3,s4,s5,&num1,&num2,&num3);
			    if (!strcmp(s4,"shapecolor"))
			      {
				 if (!strcmp(s5,"on"))
				   {icl.r=num1;icl.g=num2;icl.b=num3;}
				 else if (!strcmp(s5,"off"))
				   {icl.r=-1;icl.g=-1;icl.b=-1;}
				 else
				   {
				      sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					      "image shapecolor",s5,
					      "on,off",
					      "off");Alert(ss);
				      icl.r=-1;icl.g=-1;icl.b=-1;
				   }
			      }
			    else
			      {
				 sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					 "image",s4,
					 "shapecolor",
					 "shapecolor off");Alert(ss);
				 icl.r=-1;icl.g=-1;icl.b=-1;
			      }
			    im=LoadImage(imd,s3,&icl);
			    if (!im) 
			      {
				 sprintf(ss,"PANIC!\nERROR.........\nCannot Load image:\n%s\nQuitting..\n",s3
					 );Alert(ss);
				 EExit(1);
			      }
			    ImlibRender(imd,im,im->rgb_width,im->rgb_height);
			    bwin->click_pmap=ImlibMoveImageToPixmap(imd,im);
			    bwin->click_mask=ImlibMoveMaskToPixmap(imd,im);
			    if (im->rgb_width>b_maxw) b_maxw=im->rgb_width;
			    if (im->rgb_height>b_maxh) b_maxh=im->rgb_height;
			    ImlibDestroyImage(imd,im);
			 }
		       else if (!strcmp(s2,"selected")) 
			 {
			    sscanf(s,"%s %s %s %s %s %i %i %i",s1,s2,s3,s4,s5,&num1,&num2,&num3);
			    if (!strcmp(s4,"shapecolor"))
			      {
				 if (!strcmp(s5,"on"))
				   {icl.r=num1;icl.g=num2;icl.b=num3;}
				 else if (!strcmp(s5,"off"))
				   {icl.r=-1;icl.g=-1;icl.b=-1;}
				 else
				   {
				      sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					      "image shapecolor",s5,
					      "on,off",
					      "off");Alert(ss);
				      icl.r=-1;icl.g=-1;icl.b=-1;
				   }
			      }
			    else
			      {
				 sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					 "image",s4,
					 "shapecolor",
					 "shapecolor off");Alert(ss);
				 icl.r=-1;icl.g=-1;icl.b=-1;
			      }
			    im=LoadImage(imd,s3,&icl);
			    if (!im) 
			      {
				 sprintf(ss,"PANIC!\nERROR.........\nCannot Load image:\n%s\nQuitting..\n",s3
					 );Alert(ss);
				 EExit(1);
			      }
			    ImlibRender(imd,im,im->rgb_width,im->rgb_height);
			    bwin->sel_pmap=ImlibMoveImageToPixmap(imd,im);
			    bwin->sel_mask=ImlibMoveMaskToPixmap(imd,im);
			    if (im->rgb_width>b_maxw) b_maxw=im->rgb_width;
			    if (im->rgb_height>b_maxh) b_maxh=im->rgb_height;
			    ImlibDestroyImage(imd,im);
			 }
		       else if (!strcmp(s2,"unselected")) 
			 {
			    sscanf(s,"%s %s %s %s %s %i %i %i",s1,s2,s3,s4,s5,&num1,&num2,&num3);
			    if (!strcmp(s4,"shapecolor"))
			      {
				 if (!strcmp(s5,"on"))
				   {icl.r=num1;icl.g=num2;icl.b=num3;}
				 else if (!strcmp(s5,"off"))
				   {icl.r=-1;icl.g=-1;icl.b=-1;}
				 else
				   {
				      sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					      "image shapecolor",s5,
					      "on,off",
					      "off");Alert(ss);
				      icl.r=-1;icl.g=-1;icl.b=-1;
				   }
			      }
			    else
			      {
				 sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					 "image",s4,
					 "shapecolor",
					 "shapecolor off");Alert(ss);
				 icl.r=-1;icl.g=-1;icl.b=-1;
			      }
			    im=LoadImage(imd,s3,&icl);
			    if (!im) 
			      {
				 sprintf(ss,"PANIC!\nERROR.........\nCannot Load image:\n%s\nQuitting..\n",s3
					 );Alert(ss);
				 EExit(1);
			      }
			    ImlibRender(imd,im,im->rgb_width,im->rgb_height);
			    bwin->unsel_pmap=ImlibMoveImageToPixmap(imd,im);
			    bwin->unsel_mask=ImlibMoveMaskToPixmap(imd,im);
			    if (im->rgb_width>b_maxw) b_maxw=im->rgb_width;
			    if (im->rgb_height>b_maxh) b_maxh=im->rgb_height;
			    ImlibDestroyImage(imd,im);
			 }
		       else 
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "image detail",s2,
				    "clicked,selected,unselected",
				    "NOTHING");Alert(ss);
			 }
		    }
		  else if (!strcmp(s1,"level"))
		    {
		       done=1;
		       sscanf(s,"%s %s",s1,s2);
		       if (!strcmp(s2,"above")) bwin->above=1;
		       else if (!strcmp(s2,"below")) bwin->above=0;
		       else 
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "level",s2,
				    "above,below",
				    "below");Alert(ss);
			    bwin->above=0;
			 }
		    }
		  else if (!strcmp(s1,"action"))
		    {
		       done=1;
		       s4[0] = 0;
		       sscanf(s,"%s %s %i %s %s",s1,s2,&num,s3,s4);
		       if ((num>0)&&(num<4))
			 {
			    if (!strcmp(s2,"none")) 
			      {
				 numm=0;
			      }
			    else if (!strcmp(s2,"shift")) 
			      {
				 numm=1;
			      }
			    else if (!strcmp(s2,"ctrl")) 
			      {
				 numm=2;
			      }
			    else if (!strcmp(s2,"alt")) 
			      {
				 numm=3;
			      }
			    else 
			      {
				 sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					 "action modifier",s2,
					 "none,shift,ctrl,alt",
					 "none");Alert(ss);
				 numm=0;
			      }
			    if (!strcmp(s3,"configure")) 
			      {
				 bwin->action[num-1][numm].id=ACTION_CONFIGURE;
			      }
			    else if (!strcmp(s3,"menu")) 
			      {
				 bwin->action[num-1][numm].id=ACTION_MENU;
				 strcpy(bwin->action[num-1][numm].params,s4);
			      }
			    else if (!strcmp(s3,"exec")) 
			      {
				 bwin->action[num-1][numm].id=ACTION_EXEC;
				 strcpy(bwin->action[num-1][numm].params,strstr(s,s4));
			      }
			    else if (!strcmp(s3,"windowop")) 
			      {
				 bwin->action[num-1][numm].id=ACTION_WINDOWOP;
				 strcpy(bwin->action[num-1][numm].params,strstr(s,s4));
			      }
			    else if (!strcmp(s3,"mfocus"))
			      {
			         bwin->action[num-1][numm].id=ACTION_MOUSEFOCUS;
				 if (s4[0]) strcpy(bwin->action[num-1][numm].params,strstr(s,s4));
			      }
				else if (!strcmp(s3,"click_raise"))
				{
					bwin->action[num-1][numm].id=ACTION_CLICKRAISE;
				}
			    else if (!strcmp(s3,"exit")) 
			      {
				 bwin->action[num-1][numm].id=ACTION_EXIT;
			      }
			    else if (!strcmp(s3,"restart")) 
			      {
				 bwin->action[num-1][numm].id=ACTION_RESTART;
			      }
			    else if (!strcmp(s3,"restart_theme")) 
			      {
				 strcpy(bwin->action[num-1][numm].params,strstr(s,s4));
				 bwin->action[num-1][numm].id=ACTION_RESTART_THEME;
			      }
			    else if (!strcmp(s3,"background")) 
			      {
				 strcpy(bwin->action[num-1][numm].params,strstr(s,s4));
				 bwin->action[num-1][numm].id=ACTION_BACKGROUND;
			      }
			    else if (!strcmp(s3,"desktop")) 
			      {
				 strcpy(bwin->action[num-1][numm].params,s4);
				 bwin->action[num-1][numm].id=ACTION_DESKTOP;
			      }
			    else if (!strcmp(s3,"none")) 
			      {
				 bwin->action[num-1][numm].id=ACTION_NONE;
			      }
			    else
			      {
				 sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
					 "action",s3,
					 "configure,menu,exec,restart,none",
					 "none");Alert(ss);
			      }
			 }
		       else
			 {
			    sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
				    "action button ","!",
				    "1,2,3",
				    "NOTHING");Alert(ss);
			 }
		    }
	       }
	     }
	  }
	if (!done)
	  {
	     sprintf(ss,"SYNTAX ERROR!\nFile: %s, Line %i: ERROR.........\n---> %s\nUnknown %s mode %s.\nAllowed: [%s]\n Assuming %s.\n",file,line,s,
		     "Unrecognised Keyword",s1,
		     "Nothing Else",
		     "corrupt config file");Alert(ss);
	     EExit(1);
	  }
	done=0;
     }
   if (nest_level>0)
     {
	sprintf(ss,"WARNING!\nFile: %s, Line %i: ERROR.........\nPremature EOF.\n Section begun wiht begin not terminated with end.\n",file,line
		);Alert(ss);
     }
   fclose(f);
   for (i=0;i<cfg.num_subwins;i++)
     {
	cfg.subwin_img_clk[i]=LoadImage(imd,cfg.subwin_pmname_clk[i],&cfg.subwin_transp_clk[i]);
	if (!cfg.subwin_img_clk[i]) 
	  {
	     sprintf(ss,"PANIC!\nERROR.........\nCannot Load image:\n%s\nQuitting..\n",cfg.subwin_pmname_clk[i]
		     );Alert(ss);
	     EExit(1);
	  }
	cfg.subwin_img_sel[i]=LoadImage(imd,cfg.subwin_pmname_sel[i],&cfg.subwin_transp_sel[i]);
	if (!cfg.subwin_img_sel[i]) 
	  {
	     sprintf(ss,"PANIC!\nERROR.........\nCannot Load image:\n%s\nQuitting..\n",cfg.subwin_pmname_sel[i]
		     );Alert(ss);
	     EExit(1);
	  }
	cfg.subwin_img_uns[i]=LoadImage(imd,cfg.subwin_pmname_uns[i],&cfg.subwin_transp_uns[i]);
	if (!cfg.subwin_img_uns[i]) 
	  {
	     sprintf(ss,"PANIC!\nERROR.........\nCannot Load image:\n%s\nQuitting..\n",cfg.subwin_pmname_uns[i]
		     );Alert(ss);
	     EExit(1);
	  }
     }
}

