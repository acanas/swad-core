// swad_announcement.c: Global announcement

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Ca�as Vargas

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
#include "swad_announcement_database.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_role.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ann_PutIconToAddNewAnnouncement (__attribute__((unused)) void *Args);
static void Ann_PutButtonToAddNewAnnouncement (void);
static void Ann_DrawAnAnnouncement (long AnnCod,Ann_Status_t Status,
                                    const char *Subject,const char *Content,
                                    unsigned Roles,
                                    bool ShowAllAnnouncements,
                                    bool ICanEdit);
static void Ann_PutParams (void *AnnCod);
static long Ann_GetParamAnnCod (void);
static void Ann_PutSubjectMessage (const char *Field,const char *Label,
                                   unsigned Rows);

/*****************************************************************************/
/************************** Show global announcements ************************/
/*****************************************************************************/

void Ann_ShowAllAnnouncements (void)
  {
   extern const char *Hlp_COMMUNICATION_Announcements;
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
      /* Select all announcements */
      NumAnnouncements = Ann_DB_GetAllAnnouncements (&mysql_res);
   else if (Gbl.Usrs.Me.Logged)
      /* Select only announcements I can see */
      NumAnnouncements = Ann_DB_GetAnnouncementsICanSee (&mysql_res);
   else // No user logged
      /* Select only active announcements for unknown users */
      NumAnnouncements = Ann_DB_GetAnnouncementsForUnknownUsers (&mysql_res);

   /***** Begin box *****/
   Box_BoxBegin ("550px",Txt_Announcements,
                 ICanEdit ? Ann_PutIconToAddNewAnnouncement :
			    NULL,NULL,
		 Hlp_COMMUNICATION_Announcements,Box_NOT_CLOSABLE);

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
	    Err_WrongAnnouncementExit ();

	 /* Get status of the announcement (row[1]) */
	 Status = Ann_OBSOLETE_ANNOUNCEMENT;
	 if (sscanf (row[1],"%u",&UnsignedNum) == 1)
	    if (UnsignedNum < Ann_NUM_STATUS)
	      Status = (Ann_Status_t) UnsignedNum;

	 /* Get roles (row[2]) */
	 if (sscanf (row[2],"%u",&Roles) != 1)
	    Err_ShowErrorAndExit ("Error when reading roles of announcement.");

	 /* Get the subject (row[3]), the content (row[4]), and insert links */
	 Str_Copy (Subject,row[3],sizeof (Subject) - 1);
	 Str_Copy (Content,row[4],sizeof (Content) - 1);
	 ALn_InsertLinks (Content,Cns_MAX_BYTES_TEXT,50);

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

static void Ann_PutIconToAddNewAnnouncement (__attribute__((unused)) void *Args)
  {
   extern const char *Txt_New_announcement;

   Ico_PutContextualIconToAdd (ActWriAnn,NULL,
                               NULL,NULL,
			       Txt_New_announcement);
  }

/*****************************************************************************/
/******************* Put button to add a new announcement ********************/
/*****************************************************************************/

static void Ann_PutButtonToAddNewAnnouncement (void)
  {
   extern const char *Txt_New_announcement;

   Frm_BeginForm (ActWriAnn);
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
   Rol_GetRolesInAllCrss (&Gbl.Usrs.Me.UsrDat);
   NumAnnouncements = Ann_DB_GetAnnouncementsNotSeen (&mysql_res);

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
	       Err_WrongAnnouncementExit ();

	    /* Get the subject (row[1]), the content (row[2]), and insert links */
	    Str_Copy (Subject,row[1],sizeof (Subject) - 1);
	    Str_Copy (Content,row[2],sizeof (Content) - 1);
	    ALn_InsertLinks (Content,Cns_MAX_BYTES_TEXT,50);

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
   extern const char *The_ClassNoticeTextColor[The_NUM_THEMES];
   extern const char *Txt_Users;
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Do_not_show_again;
   static const char *ContainerClass[Ann_NUM_STATUS] =
     {
      [Ann_ACTIVE_ANNOUNCEMENT  ] = "NOTICE_CONT NOTICE_CONT_WIDE",
      [Ann_OBSOLETE_ANNOUNCEMENT] = "NOTICE_CONT NOTICE_CONT_WIDE LIGHT",
     };
   static const char *The_ClassNoticeSubjectColor[The_NUM_THEMES] =
     {
      [The_THEME_WHITE ] = "NOTICE_SUBJECT_WHITE",
      [The_THEME_GREY  ] = "NOTICE_SUBJECT_GREY",
      [The_THEME_PURPLE] = "NOTICE_SUBJECT_PURPLE",
      [The_THEME_BLUE  ] = "NOTICE_SUBJECT_BLUE",
      [The_THEME_YELLOW] = "NOTICE_SUBJECT_YELLOW",
      [The_THEME_PINK  ] = "NOTICE_SUBJECT_PINK",
      [The_THEME_DARK  ] = "NOTICE_SUBJECT_DARK",
     };
   Rol_Role_t Role;
   bool SomeRolesAreSelected;

   /***** Begin yellow note *****/
   HTM_DIV_Begin ("class=\"%s\"",ContainerClass[Status]);

      if (ICanEdit)
	{
	 /***** Put form to remove announcement *****/
	 Ico_PutContextualIconToRemove (ActRemAnn,NULL,
					Ann_PutParams,&AnnCod);

	 /***** Put form to change the status of the announcement *****/
	 switch (Status)
	   {
	    case Ann_ACTIVE_ANNOUNCEMENT:
	       Ico_PutContextualIconToHide (ActHidAnn,NULL,
					    Ann_PutParams,&AnnCod);
	       break;
	    case Ann_OBSOLETE_ANNOUNCEMENT:
	       Ico_PutContextualIconToUnhide (ActRevAnn,NULL,
					      Ann_PutParams,&AnnCod);
	       break;
	   }
	}

      /***** Write the subject of the announcement *****/
      HTM_DIV_Begin ("class=\"NOTICE_SUBJECT %s\"",The_ClassNoticeSubjectColor[Gbl.Prefs.Theme]);
	 HTM_Txt (Subject);
      HTM_DIV_End ();

      /***** Write the content of the announcement *****/
      HTM_DIV_Begin ("class=\"NOTICE_TEXT %s\"",
                     The_ClassNoticeTextColor[Gbl.Prefs.Theme]);
	 HTM_Txt (Content);
      HTM_DIV_End ();

      /***** Write announcement foot *****/
      /* Begin container for foot */
      HTM_DIV_Begin ("class=\"NOTICE_USERS\"");

	 if (ShowAllAnnouncements)
	   {
	    /* Users' roles who can view this announcement */
	    HTM_TxtColon (Txt_Users);
	    for (Role = Rol_UNK, SomeRolesAreSelected = false;
		 Role <= Rol_TCH;
		 Role++)
	       if (Roles & (1 << Role))
		 {
		  if (SomeRolesAreSelected)
		     HTM_Comma ();
		  SomeRolesAreSelected = true;
		  HTM_TxtF ("&nbsp;%s",Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN]);
		 }
	   }
	 else
	    /***** Put form to mark announcement as seen *****/
	    Lay_PutContextualLinkIconText (ActAnnSee,NULL,
					   Ann_PutParams,&AnnCod,
					   "times.svg",Ico_BLACK,
					   Txt_Do_not_show_again,NULL);

      /* End container for foot */
      HTM_DIV_End ();

   /***** End yellow note *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************** Params used to edit an assignment **********************/
/*****************************************************************************/

static void Ann_PutParams (void *AnnCod)
  {
   Par_PutHiddenParamLong (NULL,"AnnCod",*((long *) AnnCod));
  }

/*****************************************************************************/
/************** Get parameter with the code of an announcement ***************/
/*****************************************************************************/

static long Ann_GetParamAnnCod (void)
  {
   long AnnCod;

   /***** Get announcement code *****/
   if ((AnnCod = Par_GetParToLong ("AnnCod")) <= 0)
      Err_WrongAnnouncementExit ();

   return AnnCod;
  }

/*****************************************************************************/
/***************** Show form to create a new announcement ********************/
/*****************************************************************************/

void Ann_ShowFormAnnouncement (void)
  {
   extern const char *Hlp_COMMUNICATION_Announcements;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *Txt_New_announcement;
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_MSG_Content;
   extern const char *Txt_Users;
   extern const char *Txt_Create_announcement;

   /***** Begin form *****/
   Frm_BeginForm (ActRcvAnn);

      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_New_announcement,
			 NULL,NULL,
			 Hlp_COMMUNICATION_Announcements,Box_NOT_CLOSABLE,2);

	 /***** Announcement subject and body *****/
	 Ann_PutSubjectMessage ("Subject",Txt_MSG_Subject, 2);
	 Ann_PutSubjectMessage ("Content",Txt_MSG_Content,20);

	 /***** Users' roles who can view the announcement *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
	       HTM_TxtColonNBSP (Txt_Users);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"%s LT\"",The_ClassDat[Gbl.Prefs.Theme]);
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
      Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_announcement);

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
   extern const char *The_ClassInput[The_NUM_THEMES];

   /***** Subject or content *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",Field,Label);

      /* Data */
      HTM_TD_Begin ("class=\"LT\"");
	 HTM_TEXTAREA_Begin ("id=\"%s\" name=\"%s\" cols=\"75\" rows=\"%u\""
		             " class=\"%s\"",
			     Field,Field,Rows,
			     The_ClassInput[Gbl.Prefs.Theme]);
	 HTM_TEXTAREA_End ();
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
   Ann_DB_CreateAnnouncement (Roles,Subject,Content);

   /***** Write message of success *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Announcement_created);

   /***** Refresh list of announcements *****/
   Ann_ShowAllAnnouncements ();
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
   Ann_DB_HideAnnouncement (AnnCod);
  }

/*****************************************************************************/
/*********** Mark as active a global announcement that was hidden ************/
/*****************************************************************************/

void Ann_RevealHiddenAnnouncement (void)
  {
   long AnnCod;

   /***** Get the code of the global announcement to show *****/
   AnnCod = Ann_GetParamAnnCod ();

   /***** Set global announcement as not hidden *****/
   Ann_DB_UnhideAnnouncement (AnnCod);
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

   /***** Remove users who have seen the announcement *****/
   Ann_DB_RemoveUsrsWhoSawAnnouncement (AnnCod);

   /***** Remove the announcement *****/
   Ann_DB_RemoveAnnouncement (AnnCod);

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
   Ann_DB_MarkAnnouncementAsSeenByMe (AnnCod);

   /***** Show other announcements again *****/
   Ann_ShowMyAnnouncementsNotMarkedAsSeen ();
  }
