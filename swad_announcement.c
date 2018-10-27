// swad_announcement.c: Global announcement

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf

#include "swad_announcement.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_role.h"

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
static void Ann_PutParams (void);
static long Ann_GetParamAnnCod (void);
static void Ann_PutSubjectMessage (const char *Field,const char *Label,
                                   unsigned Rows);
static void Ann_CreateAnnouncement (unsigned Roles,const char *Subject,const char *Content);

/*****************************************************************************/
/************************** Show global announcements ************************/
/*****************************************************************************/

void Ann_ShowAllAnnouncements (void)
  {
   extern const char *Hlp_MESSAGES_Announcements;
   extern const char *Txt_Announcements;
   extern const char *Txt_No_announcements;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAnnouncements;
   unsigned NumAnn;
   long AnnCod;
   unsigned Roles;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Content[Cns_MAX_BYTES_TEXT + 1];
   unsigned UnsignedNum;
   Ann_Status_t Status;
   bool ICanEdit = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);

   /***** Get announcements from database *****/
   if (ICanEdit)
     {
      /* Select all announcements */
      DB_BuildQuery ("SELECT AnnCod,Status,Roles,Subject,Content"
		     " FROM announcements"
		     " ORDER BY AnnCod DESC");
     }
   else if (Gbl.Usrs.Me.Logged)
     {
      /* Select only announcements I can see */
      Rol_GetRolesInAllCrssIfNotYetGot (&Gbl.Usrs.Me.UsrDat);
      DB_BuildQuery ("SELECT AnnCod,Status,Roles,Subject,Content"
		     " FROM announcements"
		     " WHERE (Roles&%u)<>0 "
		     " ORDER BY AnnCod DESC",
                     (unsigned) Gbl.Usrs.Me.UsrDat.Roles.InCrss);	// All my roles in different courses
     }
   else // No user logged
     {
      /* Select only active announcements for unknown users */
      DB_BuildQuery ("SELECT AnnCod,Status,Roles,Subject,Content"
		     " FROM announcements"
		     " WHERE Status=%u AND (Roles&%u)<>0 "
		     " ORDER BY AnnCod DESC",
                     (unsigned) Ann_ACTIVE_ANNOUNCEMENT,
                     (unsigned) (1 << Rol_UNK));
     }
   NumAnnouncements = (unsigned) DB_QuerySELECT_new (&mysql_res,"can not get announcements");

   /***** Start box *****/
   Box_StartBox ("550px",Txt_Announcements,
                 ICanEdit ? Ann_PutIconToAddNewAnnouncement :
			    NULL,
		 Hlp_MESSAGES_Announcements,Box_NOT_CLOSABLE);
   if (!NumAnnouncements)
      Ale_ShowAlert (Ale_INFO,Txt_No_announcements);

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
      Str_Copy (Subject,row[3],
                Cns_MAX_BYTES_SUBJECT);

      /* Get the content (row[4]) and insert links */
      Str_Copy (Content,row[4],
                Cns_MAX_BYTES_TEXT);
      Str_InsertLinks (Content,Cns_MAX_BYTES_TEXT,50);

      /* Show the announcement */
      Ann_DrawAnAnnouncement (AnnCod,Status,Subject,Content,
                              Roles,true,ICanEdit);
     }

   /***** Button to add new announcement *****/
   if (ICanEdit)
      Ann_PutButtonToAddNewAnnouncement ();

   /***** End box *****/
   Box_EndBox ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Put icon to add a new announcement *********************/
/*****************************************************************************/

static void Ann_PutIconToAddNewAnnouncement (void)
  {
   extern const char *Txt_New_announcement;

   Lay_PutContextualLink (ActWriAnn,NULL,NULL,
                          "plus64x64.png",
                          Txt_New_announcement,NULL,
                          NULL);
  }

/*****************************************************************************/
/******************* Put button to add a new announcement ********************/
/*****************************************************************************/

static void Ann_PutButtonToAddNewAnnouncement (void)
  {
   extern const char *Txt_New_announcement;

   Act_StartForm (ActWriAnn);
   Btn_PutConfirmButton (Txt_New_announcement);
   Act_EndForm ();
  }

/*****************************************************************************/
/************************** Show global announcements ************************/
/*****************************************************************************/

void Ann_ShowMyAnnouncementsNotMarkedAsSeen (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAnnouncements;
   unsigned NumAnn;
   long AnnCod;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Content[Cns_MAX_BYTES_TEXT + 1];

   /***** Select announcements not seen *****/
   Rol_GetRolesInAllCrssIfNotYetGot (&Gbl.Usrs.Me.UsrDat);
   DB_BuildQuery ("SELECT AnnCod,Subject,Content FROM announcements"
		  " WHERE Status=%u AND (Roles&%u)<>0 "
		  " AND AnnCod NOT IN"
		  " (SELECT AnnCod FROM ann_seen WHERE UsrCod=%ld)"
		  " ORDER BY AnnCod DESC",	// Newest first
                  (unsigned) Ann_ACTIVE_ANNOUNCEMENT,
                  (unsigned) Gbl.Usrs.Me.UsrDat.Roles.InCrss,	// All my roles in different courses
                  Gbl.Usrs.Me.UsrDat.UsrCod);
   NumAnnouncements = (unsigned) DB_QuerySELECT_new (&mysql_res,"can not get announcements");

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
	 Str_Copy (Subject,row[1],
	           Cns_MAX_BYTES_SUBJECT);

	 /* Get the content (row[2]) and insert links */
	 Str_Copy (Content,row[2],
	           Cns_MAX_BYTES_TEXT);
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
   extern const char *Txt_Users;
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
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

   Gbl.Announcements.AnnCod = AnnCod;	// Parameter for forms

   /***** Start yellow note *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"width:500px;\">",
	    ContainerClass[Status]);

   if (ICanEdit)
     {
      /***** Put form to remove announcement *****/
      Ico_PutContextualIconToRemove (ActRemAnn,Ann_PutParams);

      /***** Put form to change the status of the announcement *****/
      switch (Status)
	{
	 case Ann_ACTIVE_ANNOUNCEMENT:
	    Lay_PutContextualLink (ActHidAnn,NULL,Ann_PutParams,
				   "eye-on64x64.png",
				   Txt_NOTICE_Active_Mark_as_obsolete,NULL,
				   NULL);
	    break;
	 case Ann_OBSOLETE_ANNOUNCEMENT:
	    Lay_PutContextualLink (ActRevAnn,NULL,Ann_PutParams,
				   "eye-slash-on64x64.png",
				   Txt_NOTICE_Obsolete_Mark_as_active,NULL,
				   NULL);
	    break;
	}
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
      for (Role = Rol_UNK, SomeRolesAreSelected = false;
	   Role <= Rol_TCH;
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
      /***** Put form to mark announcement as seen *****/
      Lay_PutContextualLink (ActAnnSee,NULL,Ann_PutParams,
			     "remove-on64x64.png",
			     Txt_Do_not_show_again,Txt_Do_not_show_again,
			     NULL);

   fprintf (Gbl.F.Out,"</div>");

   /***** End yellow note *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******************** Params used to edit an assignment **********************/
/*****************************************************************************/

static void Ann_PutParams (void)
  {
   Par_PutHiddenParamLong ("AnnCod",Gbl.Announcements.AnnCod);
  }

/*****************************************************************************/
/************** Get parameter with the code of an announcement ***************/
/*****************************************************************************/

static long Ann_GetParamAnnCod (void)
  {
   long AnnCod;

   /***** Get announcement code *****/
   if ((AnnCod = Par_GetParToLong ("AnnCod")) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of announcement.");

   return AnnCod;
  }

/*****************************************************************************/
/***************** Show form to create a new announcement ********************/
/*****************************************************************************/

void Ann_ShowFormAnnouncement (void)
  {
   extern const char *Hlp_MESSAGES_Announcements;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_New_announcement;
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_MSG_Content;
   extern const char *Txt_Users;
   extern const char *Txt_Create_announcement;

   /***** Start form *****/
   Act_StartForm (ActRcvAnn);

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_New_announcement,NULL,
                      Hlp_MESSAGES_Announcements,Box_NOT_CLOSABLE,2);

   /***** Announcement subject and body *****/
   Ann_PutSubjectMessage ("Subject",Txt_MSG_Subject, 2);
   Ann_PutSubjectMessage ("Content",Txt_MSG_Content,20);

   /***** Users' roles who can view the announcement *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_TOP\">"
                      "%s: "
                      "</td>"
                      "<td class=\"DAT LEFT_TOP\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Users);
   Rol_WriteSelectorRoles (1 << Rol_UNK     |
                           1 << Rol_GST     |
                           1 << Rol_STD     |
                           1 << Rol_NET |
                           1 << Rol_TCH,
	                   1 << Rol_UNK     |
                           1 << Rol_GST     |
                           1 << Rol_STD     |
                           1 << Rol_NET |
                           1 << Rol_TCH,
                           false,false);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** End table, send button and end box *****/
   Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_announcement);

   /***** End form *****/
   Act_EndForm ();
  }

/*****************************************************************************/
/*********** Put form field for message subject or message content ***********/
/*****************************************************************************/

static void Ann_PutSubjectMessage (const char *Field,const char *Label,
                                   unsigned Rows)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];

   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP\">"
	              "<label for=\"%s\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea id=\"%s\" name=\"%s\""
                      " cols=\"75\" rows=\"%u\">"
                      "</textarea>"
	              "</td>"
	              "</tr>",
	    Field,The_ClassForm[Gbl.Prefs.Theme],Label,
	    Field,Field,Rows);
  }

/*****************************************************************************/
/****** Receive a new announcement from a form and store it in database ******/
/*****************************************************************************/

void Ann_ReceiveAnnouncement (void)
  {
   extern const char *Txt_Announcement_created;
   unsigned Roles;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Content[Cns_MAX_BYTES_TEXT + 1];

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
   Ale_ShowAlert (Ale_SUCCESS,Txt_Announcement_created);

   /***** Refresh list of announcements *****/
   Ann_ShowAllAnnouncements ();
  }

/*****************************************************************************/
/************************ Create a new announcement **************************/
/*****************************************************************************/

static void Ann_CreateAnnouncement (unsigned Roles,const char *Subject,const char *Content)
  {
   /***** Select announcements not seen *****/
   DB_BuildQuery ("INSERT INTO announcements"
		  " (Roles,Subject,Content)"
		  " VALUES"
		  " (%u,'%s','%s')",
                  Roles,Subject,Content);
   DB_QueryINSERT_new ("can not create announcement");
  }

/*****************************************************************************/
/*********** Mark as hidden a global announcement that was active ************/
/*****************************************************************************/

void Ann_HideActiveAnnouncement (void)
  {
   char *Query;
   long AnnCod;

   /***** Get the code of the global announcement to hide *****/
   AnnCod = Ann_GetParamAnnCod ();

   /***** Set global announcement as hidden *****/
   if (asprintf (&Query,"UPDATE announcements SET Status=%u"
                        " WHERE AnnCod=%ld",
                 (unsigned) Ann_OBSOLETE_ANNOUNCEMENT,AnnCod) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryUPDATE_free (Query,"can not hide announcement");
  }

/*****************************************************************************/
/*********** Mark as active a global announcement that was hidden ************/
/*****************************************************************************/

void Ann_RevealHiddenAnnouncement (void)
  {
   char *Query;
   long AnnCod;

   /***** Get the code of the global announcement to show *****/
   AnnCod = Ann_GetParamAnnCod ();

   /***** Set global announcement as shown *****/
   if (asprintf (&Query,"UPDATE announcements SET Status=%u"
                        " WHERE AnnCod=%ld",
                 (unsigned) Ann_ACTIVE_ANNOUNCEMENT,AnnCod) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryUPDATE_free (Query,"can not reveal announcement");
  }

/*****************************************************************************/
/********************** Remove a global announcement *************************/
/*****************************************************************************/

void Ann_RemoveAnnouncement (void)
  {
   extern const char *Txt_Announcement_removed;
   char *Query;
   long AnnCod;

   /***** Get the code of the global announcement *****/
   AnnCod = Ann_GetParamAnnCod ();

   /***** Remove announcement *****/
   if (asprintf (&Query,"DELETE FROM announcements WHERE AnnCod=%ld",
                 AnnCod) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryDELETE_free (Query,"can not remove announcement");

   /***** Remove users who have seen the announcement *****/
   if (asprintf (&Query,"DELETE FROM ann_seen WHERE AnnCod=%ld",
                 AnnCod) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryDELETE_free (Query,"can not remove announcement");

   /***** Write message of success *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Announcement_removed);

   /***** Refresh list of announcements *****/
   Ann_ShowAllAnnouncements ();
  }

/*****************************************************************************/
/************************ Create a new announcement **************************/
/*****************************************************************************/

void Ann_MarkAnnouncementAsSeen (void)
  {
   char *Query;
   long AnnCod;

   /***** Get the code of the global announcement *****/
   AnnCod = Ann_GetParamAnnCod ();

   /***** Mark announcement as seen *****/
   if (asprintf (&Query,"REPLACE INTO ann_seen"
	                " (AnnCod,UsrCod)"
	                " VALUES"
	                " (%ld,%ld)",
                 AnnCod,Gbl.Usrs.Me.UsrDat.UsrCod) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryREPLACE_free (Query,"can not mark announcement as seen");

   /***** Show other announcements again *****/
   Ann_ShowMyAnnouncementsNotMarkedAsSeen ();
  }

/*****************************************************************************/
/******************** Remove user from seen announcements ********************/
/*****************************************************************************/

void Ann_RemoveUsrFromSeenAnnouncements (long UsrCod)
  {
   char *Query;

   /***** Remove user from seen announcements *****/
   if (asprintf (&Query,"DELETE FROM ann_seen WHERE UsrCod=%ld",
                 UsrCod) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryDELETE_free (Query,"can not remove user from seen announcements");
  }
