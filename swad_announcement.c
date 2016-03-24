// swad_announcement.c: Global announcement

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include "swad_announcement.h"
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

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

static void Ann_PutIconToAddNewAnnouncement (void);
static void Ann_PutButtonToAddNewAnnouncement (void);
static void Ann_DrawAnAnnouncement (long AnnCod,Ann_Status_t Status,
                                    const char *Subject,const char *Content,
                                    unsigned Roles,
                                    bool ShowAllAnnouncements,
                                    bool ICanEdit);
static void Ann_PutHiddenParamAnnCod (long AnnCod);
static long Ann_GetParamAnnCod (void);
static void Ann_CreateAnnouncement (unsigned Roles,const char *Subject,const char *Content);

/*****************************************************************************/
/************************** Show global announcements ************************/
/*****************************************************************************/

void Ann_ShowAllAnnouncements (void)
  {
   extern const char *Txt_Announcements;
   extern const char *Txt_No_announcements;
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAnnouncements;
   unsigned NumAnn;
   long AnnCod;
   unsigned Roles;
   char Subject[Cns_MAX_BYTES_SUBJECT+1];
   char Content[Cns_MAX_BYTES_TEXT+1];
   unsigned UnsignedNum;
   Ann_Status_t Status;
   bool ICanEdit = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);

   /***** Get announcements from database *****/
   if (ICanEdit)
      /* Select all announcements */
      sprintf (Query,"SELECT AnnCod,Status,Roles,Subject,Content"
		     " FROM announcements"
		     " ORDER BY AnnCod DESC");
   else if (Gbl.Usrs.Me.Logged)
      /* Select only announcements I can see */
      sprintf (Query,"SELECT AnnCod,Status,Roles,Subject,Content"
		     " FROM announcements"
                     " WHERE (Roles&%u)<>0 "
		     " ORDER BY AnnCod DESC",
            Gbl.Usrs.Me.UsrDat.Roles);	// All my roles in different courses
   else // No user logged
      /* Select only active announcements for unknown users */
      sprintf (Query,"SELECT AnnCod,Status,Roles,Subject,Content"
		     " FROM announcements"
                     " WHERE Status='%u' AND (Roles&%u)<>0 "
		     " ORDER BY AnnCod DESC",
            (unsigned) Ann_ACTIVE_ANNOUNCEMENT,
            (unsigned) (1 << Rol_UNKNOWN));
   NumAnnouncements = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get announcements");

   /***** Start frame *****/
   Lay_StartRoundFrame ("550px",Txt_Announcements,
                        ICanEdit ? Ann_PutIconToAddNewAnnouncement :
				   NULL);

   if (!NumAnnouncements)
      Lay_ShowAlert (Lay_INFO,Txt_No_announcements);

   /***** Show the announcements *****/
   for (NumAnn = 0;
	NumAnn < NumAnnouncements;
	NumAnn++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get announcement code (row[0]) */
      if (sscanf (row[0],"%ld",&AnnCod) != 1)
	 Lay_ShowErrorAndExit ("Wrong code of announcement.");

      /* Get status of the announcement (row[1]) */
      Status = Ann_OBSOLETE_ANNOUNCEMENT;
      if (sscanf (row[1],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Ann_NUM_STATUS)
	   Status = (Ann_Status_t) UnsignedNum;

      /* Get roles (row[2]) */
      if (sscanf (row[2],"%u",&Roles) != 1)
      	 Lay_ShowErrorAndExit ("Error when reading roles of announcement.");

      /* Get the content (row[3]) */
      strncpy (Subject,row[3],Cns_MAX_BYTES_SUBJECT);
      Content[Cns_MAX_BYTES_SUBJECT] = '\0';

      /* Get the content (row[4]) and insert links */
      strncpy (Content,row[4],Cns_MAX_BYTES_TEXT);
      Content[Cns_MAX_BYTES_TEXT] = '\0';
      Str_InsertLinks (Content,Cns_MAX_BYTES_TEXT,50);

      /* Show the announcement */
      Ann_DrawAnAnnouncement (AnnCod,Status,Subject,Content,
                              Roles,true,ICanEdit);
     }

   /***** Button to add new announcement *****/
   if (ICanEdit)
      Ann_PutButtonToAddNewAnnouncement ();

   /***** End frame *****/
   Lay_EndRoundFrame ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Put icon to add a new announcement *********************/
/*****************************************************************************/

static void Ann_PutIconToAddNewAnnouncement (void)
  {
   extern const char *Txt_New_announcement;

   Lay_PutContextualLink (ActWriAnn,NULL,"plus64x64.png",
                          Txt_New_announcement,NULL);
  }

/*****************************************************************************/
/******************* Put button to add a new announcement ********************/
/*****************************************************************************/

static void Ann_PutButtonToAddNewAnnouncement (void)
  {
   extern const char *Txt_New_announcement;

   Act_FormStart (ActWriAnn);
   Lay_PutConfirmButton (Txt_New_announcement);
   Act_FormEnd ();
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
                  " WHERE Status='%u' AND (Roles&%u)<>0 "
                  " AND AnnCod NOT IN"
                  " (SELECT AnnCod FROM ann_seen WHERE UsrCod='%ld')"
                  " ORDER BY AnnCod DESC",	// Newest first
            (unsigned) Ann_ACTIVE_ANNOUNCEMENT,
            Gbl.Usrs.Me.UsrDat.Roles,	// All my roles in different courses
            Gbl.Usrs.Me.UsrDat.UsrCod);
   NumAnnouncements = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get announcements");

   /***** Show the announcements *****/
   if (NumAnnouncements)
     {
      fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");

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
	 Str_InsertLinks (Content,Cns_MAX_BYTES_TEXT,50);

	 /* Show the announcement */
	 Ann_DrawAnAnnouncement (AnnCod,Ann_ACTIVE_ANNOUNCEMENT,Subject,Content,
	                         0,false,false);
	}

      fprintf (Gbl.F.Out,"</div>");
     }
  }

/*****************************************************************************/
/****************** Draw an announcement as a yellow note ********************/
/*****************************************************************************/

static void Ann_DrawAnAnnouncement (long AnnCod,Ann_Status_t Status,
                                    const char *Subject,const char *Content,
                                    unsigned Roles,
                                    bool ShowAllAnnouncements,
                                    bool ICanEdit)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Users;
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Remove;
   extern const char *Txt_NOTICE_Active_Mark_as_obsolete;
   extern const char *Txt_NOTICE_Obsolete_Mark_as_active;
   extern const char *Txt_Do_not_show_again;
   static const char *ContainerClass[Ann_NUM_STATUS] =
     {
      "NOTICE_CONTAINER_ACTIVE",	// Ann_ACTIVE_ANNOUNCEMENT
      "NOTICE_CONTAINER_OBSOLETE",	// Ann_OBSOLETE_ANNOUNCEMENT
     };
   static const char *SubjectClass[Ann_NUM_STATUS] =
     {
      "NOTICE_SUBJECT_ACTIVE",		// Ann_ACTIVE_ANNOUNCEMENT
      "NOTICE_SUBJECT_OBSOLETE",	// Ann_OBSOLETE_ANNOUNCEMENT
     };
   static const char *ContentClass[Ann_NUM_STATUS] =
     {
      "NOTICE_TEXT_ACTIVE",		// Ann_ACTIVE_ANNOUNCEMENT
      "NOTICE_TEXT_OBSOLETE",		// Ann_OBSOLETE_ANNOUNCEMENT
     };
   static const char *UsersClass[Ann_NUM_STATUS] =
     {
      "NOTICE_AUTHOR_ACTIVE",		// Ann_ACTIVE_ANNOUNCEMENT
      "NOTICE_AUTHOR_OBSOLETE",		// Ann_OBSOLETE_ANNOUNCEMENT
     };
   Rol_Role_t Role;
   bool SomeRolesAreSelected;

   /***** Start yellow note *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"width:500px;\">",
	    ContainerClass[Status]);

   if (ICanEdit)
     {
      /* Form to remove announcement */
      Act_FormStart (ActRemAnn);
      Ann_PutHiddenParamAnnCod (AnnCod);
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_OPT ICON_HIGHLIGHT\">"
        	         "<input type=\"image\""
	                 " src=\"%s/remove-on64x64.png\""
			 " alt=\"%s\" title=\"%s\""
			 " class=\"ICON20x20\" />"
			 "</div>",
	       Gbl.Prefs.IconsURL,
	       Txt_Remove,
	       Txt_Remove);
      Act_FormEnd ();

      /* Put form to change the status of the notice */
      switch (Status)
	{
	 case Ann_ACTIVE_ANNOUNCEMENT:
	    Act_FormStart (ActHidAnn);
	    Ann_PutHiddenParamAnnCod (AnnCod);
	    fprintf (Gbl.F.Out,"<div class=\"CONTEXT_OPT ICON_HIGHLIGHT\">"
			       "<input type=\"image\""
			       " src=\"%s/eye-on64x64.png\""
			       " alt=%s\" title=\"%s\""
			       " class=\"ICON20x20\" />"
			       "</div>",
		     Gbl.Prefs.IconsURL,
		     Txt_NOTICE_Active_Mark_as_obsolete,
		     Txt_NOTICE_Active_Mark_as_obsolete);
	    break;
	 case Ann_OBSOLETE_ANNOUNCEMENT:
	    Act_FormStart (ActRevAnn);
	    Ann_PutHiddenParamAnnCod (AnnCod);
	    fprintf (Gbl.F.Out,"<div class=\"CONTEXT_OPT ICON_HIGHLIGHT\">"
			       "<input type=\"image\""
			       " src=\"%s/eye-slash-on64x64.png\""
			       " alt=\"%s\" title=\"%s\""
			       " class=\"ICON20x20\" />"
			       "</div>",
		     Gbl.Prefs.IconsURL,
		     Txt_NOTICE_Obsolete_Mark_as_active,
		     Txt_NOTICE_Obsolete_Mark_as_active);
	    break;
	}
      Act_FormEnd ();
     }

   /***** Write the subject of the announcement *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s</div>",
            SubjectClass[Status],Subject);

   /***** Write the content of the announcement *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s</div>",
            ContentClass[Status],Content);

   /***** Write form *****/
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\" style=\"margin:12px;\">");

   if (ShowAllAnnouncements)
     {
      /* Users' roles who can view this announcement */
      fprintf (Gbl.F.Out,"<p class=\"%s\">%s:",
	       UsersClass[Status],Txt_Users);
      for (Role = Rol_UNKNOWN, SomeRolesAreSelected = false;
	   Role <= Rol_TEACHER;
	   Role++)
	 if (Roles & (1 << Role))
	   {
	    if (SomeRolesAreSelected)
	       fprintf (Gbl.F.Out,",");
	    SomeRolesAreSelected = true;
	    fprintf (Gbl.F.Out," %s",Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN]);
	   }
      fprintf (Gbl.F.Out,"</p>");
     }
   else
     {
      /* Form to mark announcement as seen */
      Act_FormStart (ActAnnSee);
      Ann_PutHiddenParamAnnCod (AnnCod);
      Act_LinkFormSubmit (Txt_Do_not_show_again,The_ClassForm[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"<img src=\"%s/remove-on64x64.png\""
			 " alt=\"%s\" title=\"%s\""
			 " class=\"ICON20x20\" />"
			 " %s</a>",
	       Gbl.Prefs.IconsURL,
	       Txt_Do_not_show_again,
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

   /* Get announcement code */
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
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_New_announcement;
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_MSG_Message;
   extern const char *Txt_Users;
   extern const char *Txt_Create_announcement;

   /***** Start form *****/
   Act_FormStart (ActRcvAnn);

   /***** Start frame *****/
   Lay_StartRoundFrameTable (NULL,2,Txt_New_announcement);

   /***** Message subject and body *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s: "
                      "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea name=\"Subject\" cols=\"75\" rows=\"2\">"
                      "</textarea>"
	              "</td>"
	              "</tr>"
                      "<tr>"
                      "<td class=\"%s RIGHT_TOP\">"
                      "%s: "
                      "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea name=\"Content\" cols=\"75\" rows=\"20\">"
                      "</textarea>"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_MSG_Subject,
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_MSG_Message);

   /***** Users' roles who can view the announcement *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_TOP\">"
                      "%s: "
                      "</td>"
                      "<td class=\"DAT LEFT_TOP\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Users);
   Rol_WriteSelectorRoles (1 << Rol_UNKNOWN |
                           1 << Rol__GUEST_ |
                           1 << Rol_STUDENT |
                           1 << Rol_TEACHER,
	                   1 << Rol_UNKNOWN |
                           1 << Rol__GUEST_ |
                           1 << Rol_STUDENT |
                           1 << Rol_TEACHER,
                           false,false);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Button to create announcement and end frame *****/
   Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_announcement);

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
   Roles = Rol_GetSelectedRoles ();

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
/*********** Mark as hidden a global announcement that was active ************/
/*****************************************************************************/

void Ann_HideActiveAnnouncement (void)
  {
   char Query[256];
   long AnnCod;

   /***** Get the code of the global announcement to hide *****/
   AnnCod = Ann_GetParamAnnCod ();

   /***** Set global announcement as hidden *****/
   sprintf (Query,"UPDATE announcements SET Status='%u'"
                  " WHERE AnnCod='%ld'",
            (unsigned) Ann_OBSOLETE_ANNOUNCEMENT,AnnCod);
   DB_QueryUPDATE (Query,"can not hide announcement");
  }

/*****************************************************************************/
/*********** Mark as active a global announcement that was hidden ************/
/*****************************************************************************/

void Ann_RevealHiddenAnnouncement (void)
  {
   char Query[256];
   long AnnCod;

   /***** Get the code of the global announcement to show *****/
   AnnCod = Ann_GetParamAnnCod ();

   /***** Set global announcement as shown *****/
   sprintf (Query,"UPDATE announcements SET Status='%u'"
                  " WHERE AnnCod='%ld'",
            (unsigned) Ann_ACTIVE_ANNOUNCEMENT,AnnCod);
   DB_QueryUPDATE (Query,"can not reveal announcement");
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
