// swad_announcement.c: Global announcement

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_announcement.h"
#include "swad_announcement_database.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hidden_visible.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_role.h"

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

typedef enum
  {
   Ann_DONT_SHOW_ALL = 0,
   Ann_SHOW_ALL      = 1,
  } Ann_ShowAll_t;

typedef enum
  {
   Ann_I_CAN_NOT_EDIT = 0,
   Ann_I_CAN_EDIT     = 1,
  } Ann_ICanEdit_t;

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ann_PutIconToAddNewAnnouncement (__attribute__((unused)) void *Args);

static void Ann_GetAnnouncementDataFromRow (MYSQL_RES *mysql_res,
                                            struct Ann_Announcement *Announcement);

static void Ann_DrawAnAnnouncement (struct Ann_Announcement *Announcement,
                                    Ann_ShowAll_t ShowAll,
                                    Ann_ICanEdit_t ICanEdit);
static void Ann_PutParAnnCod (void *AnnCod);
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
   struct Ann_Announcement Announcement;
   unsigned NumAnnouncements;
   unsigned NumAnn;
   Ann_ICanEdit_t ICanEdit = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM) ? Ann_I_CAN_EDIT :
									Ann_I_CAN_NOT_EDIT;

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
   Box_BoxBegin (Txt_Announcements,
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
	 /* Get announcement data */
	 Ann_GetAnnouncementDataFromRow (mysql_res,&Announcement);

	 /* Show the announcement */
	 Ann_DrawAnAnnouncement (&Announcement,
	                         Ann_SHOW_ALL,	// Show all announcements
	                         ICanEdit);
	}

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
   Ico_PutContextualIconToAdd (ActWriAnn,NULL,NULL,NULL);
  }

/*****************************************************************************/
/************************** Show global announcements ************************/
/*****************************************************************************/

void Ann_ShowMyAnnouncementsNotMarkedAsSeen (void)
  {
   MYSQL_RES *mysql_res;
   unsigned NumAnnouncements;
   unsigned NumAnn;
   struct Ann_Announcement Announcement;

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
	    /* Get announcement data */
	    Ann_GetAnnouncementDataFromRow (mysql_res,&Announcement);

	    /* Show the announcement */
	    Ann_DrawAnAnnouncement (&Announcement,
	                            Ann_DONT_SHOW_ALL,	// Don't show all announcements
	                            Ann_I_CAN_NOT_EDIT);		// I can not edit
	   }

      HTM_DIV_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Get announcement data from row ***********************/
/*****************************************************************************/

static void Ann_GetAnnouncementDataFromRow (MYSQL_RES *mysql_res,
                                            struct Ann_Announcement *Announcement)
  {
   MYSQL_ROW row;
   unsigned UnsignedNum;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get announcement code (row[0]) *****/
   if (sscanf (row[0],"%ld",&Announcement->AnnCod) != 1)
      Err_WrongAnnouncementExit ();

   /***** Get status of the announcement (row[1]) *****/
   Announcement->Status = Ann_OBSOLETE_ANNOUNCEMENT;
   if (sscanf (row[1],"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Ann_NUM_STATUS)
	 Announcement->Status = (Ann_Status_t) UnsignedNum;

   /***** Get roles (row[2]) *****/
   if (sscanf (row[2],"%u",&Announcement->Roles) != 1)
      Err_ShowErrorAndExit ("Error when reading roles of announcement.");

   /***** Get the subject (row[3]), the content (row[4]), and insert links *****/
   Str_Copy (Announcement->Subject,row[3],sizeof (Announcement->Subject) - 1);
   Str_Copy (Announcement->Content,row[4],sizeof (Announcement->Content) - 1);
   ALn_InsertLinks (Announcement->Content,Cns_MAX_BYTES_TEXT,50);
  }

/*****************************************************************************/
/****************** Draw an announcement as a yellow note ********************/
/*****************************************************************************/

static void Ann_DrawAnAnnouncement (struct Ann_Announcement *Announcement,
                                    Ann_ShowAll_t ShowAll,
                                    Ann_ICanEdit_t ICanEdit)
  {
   extern const char *Txt_Users;
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Do_not_show_again;
   static const char *ContainerClass[Ann_NUM_STATUS] =
     {
      [Ann_ACTIVE_ANNOUNCEMENT  ] = "NOTICE_BOX NOTICE_BOX_WIDE",
      [Ann_OBSOLETE_ANNOUNCEMENT] = "NOTICE_BOX NOTICE_BOX_WIDE LIGHT",
     };
   static Act_Action_t ActionHideUnhide[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = ActUnhAnn,	// Hidden ==> action to unhide
      [HidVis_VISIBLE] = ActHidAnn,	// Visible ==> action to hide
     };
   Rol_Role_t Role;
   bool SomeRolesAreSelected;

   /***** Begin yellow note *****/
   HTM_DIV_Begin ("class=\"NOTICE_CONT\"");
      HTM_DIV_Begin ("class=\"%s\"",ContainerClass[Announcement->Status]);

	 if (ICanEdit == Ann_I_CAN_EDIT)
	   {
	    /***** Icon to remove announcement *****/
	    Ico_PutContextualIconToRemove (ActRemAnn,NULL,
					   Ann_PutParAnnCod,&Announcement->AnnCod);

	    /***** Icon to hide/unhide the announcement *****/
	    Ico_PutContextualIconToHideUnhide (ActionHideUnhide,NULL,	// TODO: Put anchor
					       Ann_PutParAnnCod,&Announcement->AnnCod,
					       Announcement->Status == Ann_OBSOLETE_ANNOUNCEMENT ? HidVis_HIDDEN :
												   HidVis_VISIBLE);
	   }

	 /***** Write the subject of the announcement *****/
	 HTM_DIV_Begin ("class=\"NOTICE_SUBJECT NOTICE_SUBJECT_%s\"",
			The_GetSuffix ());
	    HTM_Txt (Announcement->Subject);
	 HTM_DIV_End ();

	 /***** Write the content of the announcement *****/
	 HTM_DIV_Begin ("class=\"NOTICE_TEXT NOTICE_TEXT_%s\"",
			The_GetSuffix ());
	    HTM_Txt (Announcement->Content);
	 HTM_DIV_End ();

	 /***** Write announcement foot *****/
	 /* Begin container for foot */
	 HTM_DIV_Begin ("class=\"NOTICE_USERS\"");

	    switch (ShowAll)
	      {
	       case Ann_DONT_SHOW_ALL:
		  /***** Put form to mark announcement as seen *****/
		  Lay_PutContextualLinkIconText (ActAnnSee,NULL,
						 Ann_PutParAnnCod,&Announcement->AnnCod,
						 "times.svg",Ico_BLACK,
						 Txt_Do_not_show_again,NULL);
		  break;
	       case Ann_SHOW_ALL:
		  /* Users' roles who can view this announcement */
		  HTM_TxtColon (Txt_Users);
		  for (Role  = Rol_UNK, SomeRolesAreSelected = false;
		       Role <= Rol_TCH;
		       Role++)
		     if (Announcement->Roles & (1 << Role))
		       {
			if (SomeRolesAreSelected)
			   HTM_Comma ();
			SomeRolesAreSelected = true;
			HTM_SPTxt (Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN]);
		       }
		  break;
	      }

	 /* End container for foot */
	 HTM_DIV_End ();

      /***** End yellow note *****/
      HTM_DIV_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************** Params used to edit an assignment **********************/
/*****************************************************************************/

static void Ann_PutParAnnCod (void *AnnCod)
  {
   if (AnnCod)
      ParCod_PutPar (ParCod_Ann,*((long *) AnnCod));
  }

/*****************************************************************************/
/***************** Show form to create a new announcement ********************/
/*****************************************************************************/

void Ann_ShowFormAnnouncement (void)
  {
   extern const char *Hlp_COMMUNICATION_Announcements;
   extern const char *Txt_Announcement;
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_MSG_Content;
   extern const char *Txt_Users;
   extern const char *Txt_Create;

   /***** Begin form *****/
   Frm_BeginForm (ActNewAnn);

      /***** Begin box and table *****/
      Box_BoxTableBegin (Txt_Announcement,NULL,NULL,
			 Hlp_COMMUNICATION_Announcements,Box_NOT_CLOSABLE,2);

	 /***** Announcement subject and body *****/
	 Ann_PutSubjectMessage ("Subject",Txt_MSG_Subject, 2);
	 Ann_PutSubjectMessage ("Content",Txt_MSG_Content,20);

	 /***** Users' roles who can view the announcement *****/
	 HTM_TR_Begin (NULL);
	    HTM_TD_TxtColon (Txt_Users);
	    HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
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
				       false,
				       HTM_DONT_SUBMIT_ON_CHANGE);
	    HTM_TD_End ();
	 HTM_TR_End ();

      /***** End table, send button and end box *****/
      Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*********** Put form field for message subject or message content ***********/
/*****************************************************************************/

static void Ann_PutSubjectMessage (const char *Field,const char *Label,
                                   unsigned Rows)
  {
   /***** Subject or content *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",Field,Label);

      /* Data */
      HTM_TD_Begin ("class=\"LT\"");
	 HTM_TEXTAREA_Begin ("id=\"%s\" name=\"%s\" cols=\"75\" rows=\"%u\""
		             " class=\"INPUT_%s\"",
			     Field,Field,Rows,
			     The_GetSuffix ());
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
   Par_GetParHTML ("Subject",Subject,Cns_MAX_BYTES_SUBJECT);

   /* Get the content of the announcement */
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_TEXT,
                              Str_TO_RIGOROUS_HTML,Str_REMOVE_SPACES);

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

void Ann_HideAnnouncement (void)
  {
   long AnnCod;

   /***** Get the code of the global announcement to hide *****/
   AnnCod = ParCod_GetAndCheckPar (ParCod_Ann);

   /***** Set global announcement as hidden *****/
   Ann_DB_HideAnnouncement (AnnCod);
  }

/*****************************************************************************/
/*********** Mark as active a global announcement that was hidden ************/
/*****************************************************************************/

void Ann_UnhideAnnouncement (void)
  {
   long AnnCod;

   /***** Get the code of the global announcement to show *****/
   AnnCod = ParCod_GetAndCheckPar (ParCod_Ann);

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
   AnnCod = ParCod_GetAndCheckPar (ParCod_Ann);

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
   AnnCod = ParCod_GetAndCheckPar (ParCod_Ann);

   /***** Mark announcement as seen *****/
   Ann_DB_MarkAnnouncementAsSeenByMe (AnnCod);

   /***** Show other announcements again *****/
   Ann_ShowMyAnnouncementsNotMarkedAsSeen ();
  }
