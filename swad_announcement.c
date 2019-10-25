// swad_announcement.c: Global announcement

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

#include "swad_announcement.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
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
      NumAnnouncements = (unsigned) DB_QuerySELECT (&mysql_res,"can not get announcements",
	                                            "SELECT AnnCod,Status,Roles,Subject,Content"
						    " FROM announcements"
						    " ORDER BY AnnCod DESC");
     }
   else if (Gbl.Usrs.Me.Logged)
     {
      /* Select only announcements I can see */
      Rol_GetRolesInAllCrssIfNotYetGot (&Gbl.Usrs.Me.UsrDat);
      NumAnnouncements = (unsigned) DB_QuerySELECT (&mysql_res,"can not get announcements",
	                                            "SELECT AnnCod,Status,Roles,Subject,Content"
						    " FROM announcements"
						    " WHERE (Roles&%u)<>0 "
						    " ORDER BY AnnCod DESC",
						    (unsigned) Gbl.Usrs.Me.UsrDat.Roles.InCrss);	// All my roles in different courses
     }
   else // No user logged
     {
      /* Select only active announcements for unknown users */
      NumAnnouncements = (unsigned) DB_QuerySELECT (&mysql_res,"can not get announcements",
	                                            "SELECT AnnCod,Status,Roles,Subject,Content"
						    " FROM announcements"
						    " WHERE Status=%u AND (Roles&%u)<>0 "
						    " ORDER BY AnnCod DESC",
						    (unsigned) Ann_ACTIVE_ANNOUNCEMENT,
						    (unsigned) (1 << Rol_UNK));
     }

   /***** Start box *****/
   Box_BoxBegin ("550px",Txt_Announcements,
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
   Box_BoxEnd ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Put icon to add a new announcement *********************/
/*****************************************************************************/

static void Ann_PutIconToAddNewAnnouncement (void)
  {
   extern const char *Txt_New_announcement;

   Ico_PutContextualIconToAdd (ActWriAnn,NULL,NULL,
			       Txt_New_announcement);
  }

/*****************************************************************************/
/******************* Put button to add a new announcement ********************/
/*****************************************************************************/

static void Ann_PutButtonToAddNewAnnouncement (void)
  {
   extern const char *Txt_New_announcement;

   Frm_StartForm (ActWriAnn);
   Btn_PutConfirmButton (Txt_New_announcement);
   Frm_EndForm ();
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
   NumAnnouncements = (unsigned) DB_QuerySELECT (&mysql_res,"can not get announcements",
	                                         "SELECT AnnCod,Subject,Content FROM announcements"
						 " WHERE Status=%u AND (Roles&%u)<>0 "
						 " AND AnnCod NOT IN"
						 " (SELECT AnnCod FROM ann_seen WHERE UsrCod=%ld)"
						 " ORDER BY AnnCod DESC",	// Newest first
						 (unsigned) Ann_ACTIVE_ANNOUNCEMENT,
						 (unsigned) Gbl.Usrs.Me.UsrDat.Roles.InCrss,	// All my roles in different courses
						 Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Show the announcements *****/
   if (NumAnnouncements)
     {
      HTM_DIV_Begin ("class=\"CM\"");

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

      HTM_DIV_End ();
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
   extern const char *Txt_Do_not_show_again;
   static const char *ContainerClass[Ann_NUM_STATUS] =
     {
      "NOTICE_CONTAINER_ACTIVE NOTICE_CONTAINER_WIDE",		// Ann_ACTIVE_ANNOUNCEMENT
      "NOTICE_CONTAINER_OBSOLETE NOTICE_CONTAINER_WIDE",	// Ann_OBSOLETE_ANNOUNCEMENT
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
      "NOTICE_USERS_ACTIVE",		// Ann_ACTIVE_ANNOUNCEMENT
      "NOTICE_USERS_OBSOLETE",		// Ann_OBSOLETE_ANNOUNCEMENT
     };
   Rol_Role_t Role;
   bool SomeRolesAreSelected;

   Gbl.Announcements.AnnCod = AnnCod;	// Parameter for forms

   /***** Start yellow note *****/
   HTM_DIV_Begin ("class=\"%s\"",ContainerClass[Status]);

   if (ICanEdit)
     {
      /***** Put form to remove announcement *****/
      Ico_PutContextualIconToRemove (ActRemAnn,Ann_PutParams);

      /***** Put form to change the status of the announcement *****/
      switch (Status)
	{
	 case Ann_ACTIVE_ANNOUNCEMENT:
	    Ico_PutContextualIconToHide (ActHidAnn,NULL,Ann_PutParams);
	    break;
	 case Ann_OBSOLETE_ANNOUNCEMENT:
	    Ico_PutContextualIconToUnhide (ActRevAnn,NULL,Ann_PutParams);
	    break;
	}
     }

   /***** Write the subject of the announcement *****/
   HTM_DIV_Begin ("class=\"%s\"",SubjectClass[Status]);
   fprintf (Gbl.F.Out,"%s",Subject);
   HTM_DIV_End ();

   /***** Write the content of the announcement *****/
   HTM_DIV_Begin ("class=\"%s\"",ContentClass[Status]);
   fprintf (Gbl.F.Out,"%s",Content);
   HTM_DIV_End ();

   /***** Write form *****/
   HTM_DIV_Begin ("class=\"NOTICE_USERS %s\"",UsersClass[Status]);

   if (ShowAllAnnouncements)
     {
      /* Users' roles who can view this announcement */
      fprintf (Gbl.F.Out,"%s:",Txt_Users);
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
      Lay_PutContextualLinkIconText (ActAnnSee,NULL,Ann_PutParams,
				     "close.svg",
				     Txt_Do_not_show_again);

   HTM_DIV_End ();

   /***** End yellow note *****/
   HTM_DIV_End ();
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
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_New_announcement;
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_MSG_Content;
   extern const char *Txt_Users;
   extern const char *Txt_Create_announcement;

   /***** Begin form *****/
   Frm_StartForm (ActRcvAnn);

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_New_announcement,NULL,
                      Hlp_MESSAGES_Announcements,Box_NOT_CLOSABLE,2);

   /***** Announcement subject and body *****/
   Ann_PutSubjectMessage ("Subject",Txt_MSG_Subject, 2);
   Ann_PutSubjectMessage ("Content",Txt_MSG_Content,20);

   /***** Users' roles who can view the announcement *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s: ",Txt_Users);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LT\"");
   Rol_WriteSelectorRoles (1 << Rol_UNK |
                           1 << Rol_GST |
                           1 << Rol_STD |
                           1 << Rol_NET |
                           1 << Rol_TCH,
	                   1 << Rol_UNK |
                           1 << Rol_GST |
                           1 << Rol_STD |
                           1 << Rol_NET |
                           1 << Rol_TCH,
                           false,false);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_announcement);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*********** Put form field for message subject or message content ***********/
/*****************************************************************************/

static void Ann_PutSubjectMessage (const char *Field,const char *Label,
                                   unsigned Rows)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];

   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RT\"");
   fprintf (Gbl.F.Out,"<label for=\"%s\" class=\"%s\">%s:</label>",
	    Field,The_ClassFormInBox[Gbl.Prefs.Theme],Label);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LT\"");
   fprintf (Gbl.F.Out,"<textarea id=\"%s\" name=\"%s\""
                      " cols=\"75\" rows=\"%u\">"
                      "</textarea>",
	    Field,Field,Rows);
   HTM_TD_End ();

   HTM_TR_End ();
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
   DB_QueryINSERT ("can not create announcement",
		   "INSERT INTO announcements"
		   " (Roles,Subject,Content)"
		   " VALUES"
		   " (%u,'%s','%s')",
                   Roles,Subject,Content);
  }

/*****************************************************************************/
/*********** Mark as hidden a global announcement that was active ************/
/*****************************************************************************/

void Ann_HideActiveAnnouncement (void)
  {
   long AnnCod;

   /***** Get the code of the global announcement to hide *****/
   AnnCod = Ann_GetParamAnnCod ();

   /***** Set global announcement as hidden *****/
   DB_QueryUPDATE ("can not hide announcement",
		   "UPDATE announcements SET Status=%u"
		   " WHERE AnnCod=%ld",
                   (unsigned) Ann_OBSOLETE_ANNOUNCEMENT,AnnCod);
  }

/*****************************************************************************/
/*********** Mark as active a global announcement that was hidden ************/
/*****************************************************************************/

void Ann_RevealHiddenAnnouncement (void)
  {
   long AnnCod;

   /***** Get the code of the global announcement to show *****/
   AnnCod = Ann_GetParamAnnCod ();

   /***** Set global announcement as shown *****/
   DB_QueryUPDATE ("can not reveal announcement",
		   "UPDATE announcements SET Status=%u"
		   " WHERE AnnCod=%ld",
                   (unsigned) Ann_ACTIVE_ANNOUNCEMENT,AnnCod);
  }

/*****************************************************************************/
/********************** Remove a global announcement *************************/
/*****************************************************************************/

void Ann_RemoveAnnouncement (void)
  {
   extern const char *Txt_Announcement_removed;
   long AnnCod;

   /***** Get the code of the global announcement *****/
   AnnCod = Ann_GetParamAnnCod ();

   /***** Remove announcement *****/
   DB_QueryDELETE ("can not remove announcement",
		   "DELETE FROM announcements WHERE AnnCod=%ld",
		   AnnCod);

   /***** Remove users who have seen the announcement *****/
   DB_QueryDELETE ("can not remove announcement",
		   "DELETE FROM ann_seen WHERE AnnCod=%ld",
		   AnnCod);

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
   long AnnCod;

   /***** Get the code of the global announcement *****/
   AnnCod = Ann_GetParamAnnCod ();

   /***** Mark announcement as seen *****/
   DB_QueryREPLACE ("can not mark announcement as seen",
		    "REPLACE INTO ann_seen"
		    " (AnnCod,UsrCod)"
		    " VALUES"
		    " (%ld,%ld)",
                    AnnCod,Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Show other announcements again *****/
   Ann_ShowMyAnnouncementsNotMarkedAsSeen ();
  }

/*****************************************************************************/
/******************** Remove user from seen announcements ********************/
/*****************************************************************************/

void Ann_RemoveUsrFromSeenAnnouncements (long UsrCod)
  {
   /***** Remove user from seen announcements *****/
   DB_QueryDELETE ("can not remove user from seen announcements",
		   "DELETE FROM ann_seen WHERE UsrCod=%ld",
		   UsrCod);
  }
