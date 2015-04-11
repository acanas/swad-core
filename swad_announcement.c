// swad_announcement.c: Global announcement

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*****************************************************************************/
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <string.h>	// For strncpy...

#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal constants ****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;
// extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

static void Ann_ListAnnouncements (void);
static void Ann_ShowAnnouncement (long AnnCod,const char *Subject,const char *Content,
                                  unsigned Roles,bool ShowAllAnnouncements);
static void Ann_PutHiddenParamAnnCod (long AnnCod);
static long Ann_GetParamAnnCod (void);
static void Ann_CreateAnnouncement (unsigned Roles,const char *Subject,const char *Content);

/*****************************************************************************/
/************************** Show global announcements ************************/
/*****************************************************************************/

void Ann_ShowAllAnnouncements (void)
  {
   extern const char *Txt_New_announcement;

   /***** Put link (form) to create a new announcement *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Act_PutContextualLink (ActWriAnn,NULL,"new",Txt_New_announcement);
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** List announcements *****/
   Ann_ListAnnouncements ();
  }

/*****************************************************************************/
/************************** Show global announcements ************************/
/*****************************************************************************/

static void Ann_ListAnnouncements (void)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAnnouncements;
   unsigned NumAnn;
   long AnnCod;
   unsigned Roles;
   char Subject[Cns_MAX_BYTES_SUBJECT+1];
   char Content[Cns_MAX_BYTES_TEXT+1];

   /***** Select announcements *****/
   sprintf (Query,"SELECT AnnCod,Roles,Subject,Content FROM announcements"
                  " ORDER BY AnnCod DESC");
   NumAnnouncements = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get announcements");

   /***** Show the announcements *****/
   for (NumAnn = 0;
	NumAnn < NumAnnouncements;
	NumAnn++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get announcement code (row[0]) */
      if (sscanf (row[0],"%ld",&AnnCod) != 1)
	 Lay_ShowErrorAndExit ("Wrong code of announcement.");

      /* Get roles (row[1]) */
      if (sscanf (row[1],"%u",&Roles) != 1)
      	 Lay_ShowErrorAndExit ("Error when reading roles of announcement.");

      /* Get the content (row[2]) */
      strncpy (Subject,row[2],Cns_MAX_BYTES_SUBJECT);
      Content[Cns_MAX_BYTES_SUBJECT] = '\0';

      /* Get the content (row[3]) and insert links */
      strncpy (Content,row[3],Cns_MAX_BYTES_TEXT);
      Content[Cns_MAX_BYTES_TEXT] = '\0';
      Str_InsertLinkInURLs (Content,Cns_MAX_BYTES_TEXT,50);

      /* Show the announcement */
      Ann_ShowAnnouncement (AnnCod,Subject,Content,Roles,true);
     }
  }

/*****************************************************************************/
/************************** Show global announcements ************************/
/*****************************************************************************/

void Ann_ShowMyAnnouncementsNotMarkedAsSeen (void)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAnnouncements;
   unsigned NumAnn;
   long AnnCod;
   char Subject[Cns_MAX_BYTES_SUBJECT+1];
   char Content[Cns_MAX_BYTES_TEXT+1];

   /***** Select announcements not seen *****/
   // Roles == 24 ==> Teachers and students
   sprintf (Query,"SELECT AnnCod,Subject,Content FROM announcements"
                  " WHERE (Roles&%u)<>0 AND AnnCod NOT IN"
                  " (SELECT AnnCod FROM ann_seen WHERE UsrCod='%ld')"
                  " ORDER BY AnnCod DESC",	// Newest first
            Gbl.Usrs.Me.UsrDat.Roles,	// All my roles in different courses
            Gbl.Usrs.Me.UsrDat.UsrCod);
   NumAnnouncements = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get announcements");

   /***** Show the announcements *****/
   if (NumAnnouncements)
     {
      fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");

      for (NumAnn = 0;
	   NumAnn < NumAnnouncements;
	   NumAnn++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get announcement code (row[0]) */
	 if (sscanf (row[0],"%ld",&AnnCod) != 1)
	    Lay_ShowErrorAndExit ("Wrong code of announcement.");

	 /* Get the content (row[1]) */
	 strncpy (Subject,row[1],Cns_MAX_BYTES_SUBJECT);
	 Content[Cns_MAX_BYTES_SUBJECT] = '\0';

	 /* Get the content (row[2]) and insert links */
	 strncpy (Content,row[2],Cns_MAX_BYTES_TEXT);
	 Content[Cns_MAX_BYTES_TEXT] = '\0';
	 Str_InsertLinkInURLs (Content,Cns_MAX_BYTES_TEXT,50);

	 /* Show the announcement */
	 Ann_ShowAnnouncement (AnnCod,Subject,Content,0,false);
	}

      fprintf (Gbl.F.Out,"</div>");
     }
  }

/*****************************************************************************/
/****************** Draw an announcement as a yellow note ********************/
/*****************************************************************************/

static void Ann_ShowAnnouncement (long AnnCod,const char *Subject,const char *Content,
                                  unsigned Roles,bool ShowAllAnnouncements)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Users;
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Remove;
   extern const char *Txt_Do_not_show_again;
   Rol_Role_t Role;
   bool RolesSelected;

   /***** Start yellow note *****/
   fprintf (Gbl.F.Out,"<div class=\"NOTICE_CONTAINER\""
	              " style=\"width:400px;\">");

   /***** Write the content of the announcement *****/
   fprintf (Gbl.F.Out,"<div class=\"NOTICE_SUBJECT\">%s</div>",
            Subject);

   /***** Write the content of the announcement *****/
   fprintf (Gbl.F.Out,"<div class=\"NOTICE_TEXT\">%s</div>",
            Content);

   /***** Write form *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\""
	              " style=\"text-align:center; margin:10px;\">",
	    The_ClassFormul[Gbl.Prefs.Theme]);

   if (ShowAllAnnouncements)
     {
      /* Users' roles who can view this announcement */
      fprintf (Gbl.F.Out,"<p class=\"DAT\">%s:",
	       Txt_Users);
      for (Role = Rol_STUDENT, RolesSelected = false;
	   Role <= Rol_TEACHER;
	   Role++)
	 if (Roles & (1 << Role))
	   {
	    if (RolesSelected)
	       fprintf (Gbl.F.Out,",");
	    else
	       RolesSelected = true;
	    fprintf (Gbl.F.Out," %s",Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN]);
	   }
      fprintf (Gbl.F.Out,"</p>");

      /* Form to remove announcement */
      Act_FormStart (ActRemAnn);
      Ann_PutHiddenParamAnnCod (AnnCod);
      Act_LinkFormSubmit (Txt_Remove,The_ClassFormul[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"<img src=\"%s/delon16x16.gif\""
			 " alt=\"%s\" class=\"ICON16x16\" />"
			 " %s</a>",
	       Gbl.Prefs.IconsURL,
	       Txt_Remove,
	       Txt_Remove);
      Act_FormEnd ();
     }
   else
     {
      /* Form to mark announcement as seen */
      Act_FormStart (ActAnnSee);
      Ann_PutHiddenParamAnnCod (AnnCod);
      Act_LinkFormSubmit (Txt_Do_not_show_again,The_ClassFormul[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"<img src=\"%s/delon16x16.gif\""
			 " alt=\"%s\" class=\"ICON16x16\" />"
			 " %s</a>",
	       Gbl.Prefs.IconsURL,
	       Txt_Do_not_show_again,
	       Txt_Do_not_show_again);
      Act_FormEnd ();
     }

   fprintf (Gbl.F.Out,"</div>");

   /***** End yellow note *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************** Put parameter with the code of an announcement ***************/
/*****************************************************************************/

static void Ann_PutHiddenParamAnnCod (long AnnCod)
  {
   Par_PutHiddenParamLong ("AnnCod",AnnCod);
  }

/*****************************************************************************/
/************** Get parameter with the code of an announcement ***************/
/*****************************************************************************/

static long Ann_GetParamAnnCod (void)
  {
   char LongStr[1+10+1];	// String that holds the announcement code
   long AnnCod;

   /* Get notice code */
   Par_GetParToText ("AnnCod",LongStr,1+10);
   if (sscanf (LongStr,"%ld",&AnnCod) != 1)
      Lay_ShowErrorAndExit ("Wrong code of announcement.");

   return AnnCod;
  }

/*****************************************************************************/
/***************** Show form to create a new announcement ********************/
/*****************************************************************************/

void Ann_ShowFormAnnouncement (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_New_announcement;
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_MSG_Message;
   extern const char *Txt_Users;
   extern const char *Txt_Create_announcement;

   /***** Start form *****/
   Act_FormStart (ActRcvAnn);

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,2,Txt_New_announcement);

   /***** Message subject and body *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s\" style=\"text-align:right; vertical-align:top;\">"
	              "%s: "
                      "</td>"
                      "<td style=\"text-align:left;\">"
                      "<textarea name=\"Subject\" cols=\"75\" rows=\"2\">"
                      "</textarea>"
	              "</td>"
	              "</tr>"
                      "<tr>"
                      "<td class=\"%s\" style=\"text-align:right; vertical-align:top;\">"
                      "%s: "
                      "</td>"
                      "<td style=\"text-align:left;\">"
                      "<textarea name=\"Content\" cols=\"75\" rows=\"20\">"
                      "</textarea>"
                      "</td>"
                      "</tr>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_MSG_Subject,
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_MSG_Message);

   /***** Users' roles who can view the announcement *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"TIT_TBL\""
                      " style=\"text-align:right; vertical-align:top;\">"
                      "%s: "
                      "</td>"
                      "<td class=\"DAT\""
                      " style=\"text-align:left; vertical-align:middle;\">",
            Txt_Users);
   Rol_WriteSelectorRoles (1 << Rol_STUDENT |
                           1 << Rol_TEACHER);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Button to create announcement *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" style=\"text-align:center;\">");
   Lay_PutCreateButton (Txt_Create_announcement);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** End frame *****/
   Lay_EndRoundFrameTable10 ();

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/****** Receive a new announcement from a form and store it in database ******/
/*****************************************************************************/

void Ann_ReceiveAnnouncement (void)
  {
   extern const char *Txt_Announcement_created;
   unsigned Roles;
   char Subject[Cns_MAX_BYTES_SUBJECT+1];
   char Content[Cns_MAX_BYTES_TEXT+1];

   /***** Get data from form *****/
   /* Get the subject of the announcement */
   Par_GetParToHTML ("Subject",Subject,Cns_MAX_BYTES_SUBJECT);

   /* Get the content of the announcement */
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_TEXT,
                              Str_TO_RIGOROUS_HTML,true);

   /* Get users who can view this announcement */
   Rol_GetSelectedRoles (&Roles);

   /***** Create a new announcement in database *****/
   Ann_CreateAnnouncement (Roles,Subject,Content);

   /***** Write message of success *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_Announcement_created);

   /***** Refresh list of announcements *****/
   Ann_ShowAllAnnouncements ();
  }

/*****************************************************************************/
/************************ Create a new announcement **************************/
/*****************************************************************************/

static void Ann_CreateAnnouncement (unsigned Roles,const char *Subject,const char *Content)
  {
   char Query[128+Cns_MAX_BYTES_SUBJECT+Cns_MAX_BYTES_TEXT];

   /***** Select announcements not seen *****/
   sprintf (Query,"INSERT INTO announcements (Roles,Subject,Content)"
                  " VALUES ('%u','%s','%s')",
            Roles,Subject,Content);
   DB_QueryINSERT (Query,"can not create announcement");
  }

/*****************************************************************************/
/********************** Remove a global announcement *************************/
/*****************************************************************************/

void Ann_RemoveAnnouncement (void)
  {
   extern const char *Txt_Announcement_removed;
   char Query[128];
   long AnnCod;

   /***** Get the code of the global announcement *****/
   AnnCod = Ann_GetParamAnnCod ();

   /***** Remove announcement *****/
   sprintf (Query,"DELETE FROM announcements WHERE AnnCod='%ld'",
            AnnCod);
   DB_QueryDELETE (Query,"can not remove announcement");

   /***** Remove users who have seen the announcement *****/
   sprintf (Query,"DELETE FROM ann_seen WHERE AnnCod='%ld'",
            AnnCod);
   DB_QueryDELETE (Query,"can not remove announcement");

   /***** Write message of success *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_Announcement_removed);

   /***** Refresh list of announcements *****/
   Ann_ShowAllAnnouncements ();
  }

/*****************************************************************************/
/************************ Create a new announcement **************************/
/*****************************************************************************/

void Ann_MarkAnnouncementAsSeen (void)
  {
   char Query[128];
   long AnnCod;

   /***** Get the code of the global announcement *****/
   AnnCod = Ann_GetParamAnnCod ();

   /***** Mark announcement as seen *****/
   sprintf (Query,"REPLACE INTO ann_seen (AnnCod,UsrCod) VALUES ('%ld','%ld')",
            AnnCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryREPLACE (Query,"can not mark announcement as seen");

   /***** Show other announcements again *****/
   Ann_ShowMyAnnouncementsNotMarkedAsSeen ();
  }

/*****************************************************************************/
/******************** Remove user from seen announcements ********************/
/*****************************************************************************/

void Ann_RemoveUsrFromSeenAnnouncements (long UsrCod)
  {
   char Query[128];

   /***** Remove user from seen announcements *****/
   sprintf (Query,"DELETE FROM ann_seen WHERE UsrCod='%ld'",
            UsrCod);
   DB_QueryDELETE (Query,"can not remove user from seen announcements");
  }
