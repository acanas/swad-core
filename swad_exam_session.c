// swad_exam_session.c: exam sessions (each ocurrence of an exam)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_error.h"
#include "swad_exam.h"
#include "swad_exam_database.h"
#include "swad_exam_print.h"
#include "swad_exam_result.h"
#include "swad_exam_session.h"
#include "swad_exam_set.h"
#include "swad_exam_type.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_group_database.h"
#include "swad_hidden_visible.h"
#include "swad_HTML.h"
#include "swad_parameter_code.h"
#include "swad_role.h"
#include "swad_setting.h"
#include "swad_test.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaSes_PutIconsInListOfSessions (void *Exams);
static void ExaSes_PutIconToCreateNewSession (struct Exa_Exams *Exams);

static void ExaSes_ListOneOrMoreSessions (struct Exa_Exams *Exams,
      					  Frm_PutForm_t PutFormSession,
				          unsigned NumSessions,
                                          MYSQL_RES *mysql_res);
static void ExaSes_ListOneOrMoreSessionsHeading (bool ICanEditSessions);
static bool ExaSes_CheckIfICanEditSessions (void);
static bool ExaSes_CheckIfICanEditThisSession (long UsrCod);
static bool ExaSes_CheckIfVisibilityOfResultsCanBeChanged (const struct ExaSes_Session *Session);
static void ExaSes_ListOneOrMoreSessionsIcons (struct Exa_Exams *Exams,
                                               const struct ExaSes_Session *Session,
					       const char *Anchor);
static void ExaSes_ListOneOrMoreSessionsAuthor (const struct ExaSes_Session *Session);
static void ExaSes_ListOneOrMoreSessionsTimes (const struct ExaSes_Session *Session,
                                               unsigned UniqueId);
static void ExaSes_ListOneOrMoreSessionsTitleGrps (struct Exa_Exams *Exams,
                                                   const struct ExaSes_Session *Session,
                                                   const char *Anchor);
static void ExaSes_GetAndWriteNamesOfGrpsAssociatedToSession (const struct ExaSes_Session *Session);
static void ExaSes_ListOneOrMoreSessionsResult (struct Exa_Exams *Exams,
                                                const struct ExaSes_Session *Session);
static void ExaSes_ListOneOrMoreSessionsResultStd (struct Exa_Exams *Exams,
                                                   const struct ExaSes_Session *Session);
static void ExaSes_ListOneOrMoreSessionsResultTch (struct Exa_Exams *Exams,
                                                   const struct ExaSes_Session *Session);

static void ExaSes_GetSessionDataFromRow (MYSQL_RES *mysql_res,
				          struct ExaSes_Session *Session);

static void ExaSes_HideUnhideSession (HidVis_HiddenOrVisible_t HiddenOrVisible);

static void ExaSes_PutFormSession (struct ExaSes_Session *Session);
static void ExaSes_ParsFormSession (void *Session);

static void ExaSes_ShowLstGrpsToCreateSession (long SesCod);

static void ExaSes_CreateSession (struct ExaSes_Session *Session);
static void ExaSes_UpdateSession (struct ExaSes_Session *Session);

static void ExaSes_CreateGrpsAssociatedToExamSession (long SesCod,
                                                      const struct ListCodGrps *LstGrpsSel);

/*****************************************************************************/
/***************************** Reset exam session ****************************/
/*****************************************************************************/

void ExaSes_ResetSession (struct ExaSes_Session *Session)
  {
   Dat_StartEndTime_t StartEndTime;

   /***** Initialize to empty match *****/
   Session->SesCod                   = -1L;
   Session->ExaCod                   = -1L;
   Session->UsrCod                   = -1L;
   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
      Session->TimeUTC[StartEndTime] = (time_t) 0;
   Session->Title[0]                 = '\0';
   Session->HiddenOrVisible	     = HidVis_VISIBLE;
   Session->Open	             = false;
   Session->ShowUsrResults           = false;
  };

/*****************************************************************************/
/************************ List the sessions of an exam ***********************/
/*****************************************************************************/

void ExaSes_ListSessions (struct Exa_Exams *Exams,
                          Frm_PutForm_t PutFormSession)
  {
   extern const char *Hlp_ASSESSMENT_Exams_sessions;
   extern const char *Txt_Sessions;
   MYSQL_RES *mysql_res;
   unsigned NumSessions;

   /***** Get data of sessions from database *****/
   NumSessions = Exa_DB_GetSessions (&mysql_res,Exams->Exam.ExaCod);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Sessions,
                 ExaSes_PutIconsInListOfSessions,Exams,
                 Hlp_ASSESSMENT_Exams_sessions,Box_NOT_CLOSABLE);

      /***** Select whether show only my groups or all groups *****/
      if (Gbl.Crs.Grps.NumGrps && ExaSes_CheckIfICanEditSessions ())
	{
	 Set_BeginSettingsHead ();
	    Grp_ShowFormToSelWhichGrps (ActSeeOneExa,Exa_PutPars,Exams);
	 Set_EndSettingsHead ();
	}

      /***** Show the table with the sessions *****/
      ExaSes_ListOneOrMoreSessions (Exams,PutFormSession,
				    NumSessions,mysql_res);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************* Get exam session data using its code ********************/
/*****************************************************************************/

void ExaSes_GetSessionDataByCod (struct ExaSes_Session *Session)
  {
   MYSQL_RES *mysql_res;

   /***** Trivial check *****/
   if (Session->SesCod <= 0)
     {
      /* Initialize to empty exam session */
      ExaSes_ResetSession (Session);
      return;
     }

   /***** Get exam data session from database *****/
   if (Exa_DB_GetSessionDataByCod (&mysql_res,Session->SesCod)) // Session found...
      /* Get exam session data from row */
      ExaSes_GetSessionDataFromRow (mysql_res,Session);
   else
      /* Initialize to empty exam session */
      ExaSes_ResetSession (Session);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Put icons in list of sessions of an exam ******************/
/*****************************************************************************/

static void ExaSes_PutIconsInListOfSessions (void *Exams)
  {
   bool ICanEditSessions;

   if (Exams)
     {
      /***** Put icon to create a new exam session in current exam *****/
      ICanEditSessions = ExaSes_CheckIfICanEditSessions ();
      if (ICanEditSessions)
	 ExaSes_PutIconToCreateNewSession ((struct Exa_Exams *) Exams);
     }
  }

/*****************************************************************************/
/******************* Put icon to create a new exam session *******************/
/*****************************************************************************/

static void ExaSes_PutIconToCreateNewSession (struct Exa_Exams *Exams)
  {
   Ico_PutContextualIconToAdd (ActReqNewExaSes,ExaSes_NEW_SESSION_SECTION_ID,
                               Exa_PutPars,Exams);
  }

/*****************************************************************************/
/********************** List exam sessions for edition ***********************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessions (struct Exa_Exams *Exams,
      					  Frm_PutForm_t PutFormSession,
				          unsigned NumSessions,
                                          MYSQL_RES *mysql_res)
  {
   unsigned NumSession;
   unsigned UniqueId;
   struct ExaSes_Session Session;
   char *Anchor;
   bool ICanEditSessions = ExaSes_CheckIfICanEditSessions ();
   long SesCodToBeEdited = PutFormSession == Frm_PUT_FORM &&
			   Exams->SesCod > 0 ? Exams->SesCod :
					       -1L;

   /***** Reset session *****/
   ExaSes_ResetSession (&Session);

   /***** Begin table with sessions *****/
   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Write the heading *****/
      if (NumSessions)
	 ExaSes_ListOneOrMoreSessionsHeading (ICanEditSessions);

      /***** Write rows *****/
      for (NumSession = 0, UniqueId = 1, The_ResetRowColor ();
	   NumSession < NumSessions;
	   NumSession++, UniqueId++, The_ChangeRowColor ())
	{
	 /***** Get exam session data from row *****/
	 ExaSes_GetSessionDataFromRow (mysql_res,&Session);

	 if (ExaSes_CheckIfICanListThisSessionBasedOnGrps (Session.SesCod))
	   {
	    /***** Build anchor string *****/
	    if (asprintf (&Anchor,"evt_%ld_%ld",Exams->Exam.ExaCod,Session.SesCod) < 0)
	       Err_NotEnoughMemoryExit ();

	    /***** First row for this session ****/
	    HTM_TR_Begin (NULL);

	       /* Icons */
	       if (ICanEditSessions)
		  ExaSes_ListOneOrMoreSessionsIcons (Exams,&Session,Anchor);

	       /* Start/end date/time */
	       ExaSes_ListOneOrMoreSessionsTimes (&Session,UniqueId);

	       /* Title and groups */
	       ExaSes_ListOneOrMoreSessionsTitleGrps (Exams,&Session,Anchor);

	       /* Session result visible? */
	       ExaSes_ListOneOrMoreSessionsResult (Exams,&Session);

	    HTM_TR_End ();

	    /***** Second row: session author */
	    HTM_TR_Begin (NULL);
	       ExaSes_ListOneOrMoreSessionsAuthor (&Session);
	    HTM_TR_End ();

	    /***** Third row: form to edit this session ****/
	    if (ICanEditSessions && PutFormSession == Frm_PUT_FORM &&	// Editing...
		Session.SesCod == SesCodToBeEdited)			// ...this session
	      {
	       HTM_TR_Begin (NULL);
		  HTM_TD_Begin ("colspan=\"5\" class=\"LT %s\"",
		                The_GetColorRows ());
		     ExaSes_PutFormSession (&Session);	// Form to edit this session
		  HTM_TD_End ();
	       HTM_TR_End ();
	      }

	    /***** Free anchor string *****/
	    free (Anchor);
	   }
	}

      /***** Put form to create a new exam session in this exam *****/
      if (ICanEditSessions && PutFormSession == Frm_PUT_FORM &&
	  SesCodToBeEdited <= 0)
	{
	 /* Reset session */
	 ExaSes_ResetSession (&Session);
	 Session.ExaCod = Exams->Exam.ExaCod;
	 Session.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();		// Now
	 Session.TimeUTC[Dat_END_TIME] = Session.TimeUTC[Dat_STR_TIME] + (1 * 60 * 60);	// Now + 1 hour
	 Str_Copy (Session.Title,Exams->Exam.Title,sizeof (Session.Title) - 1);

	 /* Put form to create new session */
	 HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"5\" class=\"LT %s\"",The_GetColorRows ());
	       ExaSes_PutFormSession (&Session);	// Form to create session
	    HTM_TD_End ();
	 HTM_TR_End ();
	}

   /***** End table with sessions *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************* Put a column for exam session start and end times *************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsHeading (bool ICanEditSessions)
  {
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Session;
   extern const char *Txt_Results;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Column for icons *****/
      if (ICanEditSessions)
	 HTM_TH_Empty (1);

      /***** The rest of columns *****/
      HTM_TH (Txt_START_END_TIME[Exa_ORDER_BY_START_DATE]   ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_START_END_TIME[Exa_ORDER_BY_END_DATE  ]   ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Session                                   ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Results                                   ,HTM_HEAD_CENTER);

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/********************** Check if I can edit sessions *************************/
/*****************************************************************************/

static bool ExaSes_CheckIfICanEditSessions (void)
  {
   static const bool ICanEditSessions[Rol_NUM_ROLES] =
     {
      [Rol_NET    ] = true,
      [Rol_TCH    ] = true,
      [Rol_SYS_ADM] = true,
     };

   return ICanEditSessions[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/************ Check if I can edit (remove/resume) an exam session ************/
/*****************************************************************************/

static bool ExaSes_CheckIfICanEditThisSession (long UsrCod)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
	 return (UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);	// Only if I am the creator
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/********** Check if visibility of session results can be changed ************/
/*****************************************************************************/

static bool ExaSes_CheckIfVisibilityOfResultsCanBeChanged (const struct ExaSes_Session *Session)
  {
   if (Session->ShowUsrResults ||						// Results are currently visible
       Session->TimeUTC[Dat_END_TIME] < Dat_GetStartExecutionTimeUTC ())	// End of time is in the past
      if (ExaSes_CheckIfICanEditThisSession (Session->UsrCod))
	 return true;

   return false;
  }

/*****************************************************************************/
/************************* Put a column for icons ****************************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsIcons (struct Exa_Exams *Exams,
                                               const struct ExaSes_Session *Session,
					       const char *Anchor)
  {
   static const Act_Action_t ActionHideUnhide[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = ActUnhExaSes,	// Hidden ==> action to unhide
      [HidVis_VISIBLE] = ActHidExaSes,	// Visible ==> action to hide
     };

   Exams->Exam.ExaCod = Session->ExaCod;
   Exams->SesCod      = Session->SesCod;

   /***** Begin cell *****/
   HTM_TD_Begin ("rowspan=\"2\" class=\"BT %s\"",The_GetColorRows ());

      if (ExaSes_CheckIfICanEditThisSession (Session->UsrCod))
	{
	 /***** Icon to remove the exam session *****/
	 Ico_PutContextualIconToRemove (ActReqRemExaSes,NULL,
					ExaSes_PutParsEdit,Exams);

	 /***** Icon to hide/unhide the exam session *****/
	 Ico_PutContextualIconToHideUnhide (ActionHideUnhide,Anchor,
					    ExaSes_PutParsEdit,Exams,
					    Session->HiddenOrVisible);

	 /***** Icon to edit the exam session *****/
	 Ico_PutContextualIconToEdit (ActReqChgExaSes,Anchor,
				      ExaSes_PutParsEdit,Exams);
	}

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/********** Put a column for teacher who created the exam session ************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsAuthor (const struct ExaSes_Session *Session)
  {
   /***** Session author (teacher) *****/
   HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",The_GetColorRows ());
      Usr_WriteAuthor1Line (Session->UsrCod,Session->HiddenOrVisible);
   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Put a column for exam session start and end times *************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsTimes (const struct ExaSes_Session *Session,
                                               unsigned UniqueId)
  {
   extern const char *HidVis_DateGreenClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_DateRedClass[HidVis_NUM_HIDDEN_VISIBLE];
   Dat_StartEndTime_t StartEndTime;
   const char *DateClass;
   char *Id;

   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      DateClass = Session->Open ? HidVis_DateGreenClass[Session->HiddenOrVisible] :
			          HidVis_DateRedClass[Session->HiddenOrVisible];

      if (asprintf (&Id,"exa_time_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
		    Id,DateClass,The_GetSuffix (),The_GetColorRows ());
	 Dat_WriteLocalDateHMSFromUTC (Id,Session->TimeUTC[StartEndTime],
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       true,true,true,0x6);
      HTM_TD_End ();
      free (Id);
     }
  }

/*****************************************************************************/
/************** Put a column for exam session title and grous ****************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsTitleGrps (struct Exa_Exams *Exams,
                                                   const struct ExaSes_Session *Session,
                                                   const char *Anchor)
  {
   extern const char *Txt_Play;
   extern const char *Txt_Resume;
   extern const char *HidVis_TitleClass[HidVis_NUM_HIDDEN_VISIBLE];

   HTM_TD_Begin ("rowspan=\"2\" class=\"LT %s\"",The_GetColorRows ());

      /***** Session title *****/
      HTM_ARTICLE_Begin (Anchor);
	 if (ExaSes_CheckIfICanAnswerThisSession (&Exams->Exam,Session))
	   {
	    Frm_BeginForm (ActSeeExaPrn);
	       Exa_PutPars (Exams);
	       ParCod_PutPar (ParCod_Ses,Session->SesCod);
	       HTM_BUTTON_Submit_Begin (Gbl.Usrs.Me.Role.Logged == Rol_STD ? Txt_Play :
									     Txt_Resume,
					"class=\"LT BT_LINK %s_%s\"",
					HidVis_TitleClass[Session->HiddenOrVisible],
					The_GetSuffix ());
		  HTM_Txt (Session->Title);
	       HTM_BUTTON_End ();
	    Frm_EndForm ();
	   }
	 else
	   {
	    HTM_SPAN_Begin ("class=\"%s_%s\"",
			    HidVis_TitleClass[Session->HiddenOrVisible],
			    The_GetSuffix ());
	       HTM_Txt (Session->Title);
	    HTM_SPAN_End ();
	   }
      HTM_ARTICLE_End ();

      /***** Groups whose students can answer this exam session *****/
      if (Gbl.Crs.Grps.NumGrps)
	 ExaSes_GetAndWriteNamesOfGrpsAssociatedToSession (Session);

   HTM_TD_End ();
  }

/*****************************************************************************/
/********* Get and write the names of the groups of an exam session **********/
/*****************************************************************************/

static void ExaSes_GetAndWriteNamesOfGrpsAssociatedToSession (const struct ExaSes_Session *Session)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_and;
   extern const char *Txt_The_whole_course;
   extern const char *HidVis_GroupClass[HidVis_NUM_HIDDEN_VISIBLE];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrps;
   unsigned NumGrp;

   /***** Get groups *****/
   NumGrps = Exa_DB_GetGrpsAssociatedToSes (&mysql_res,Session->SesCod);

   /***** Write heading *****/
   HTM_DIV_Begin ("class=\"%s_%s\"",
                  HidVis_GroupClass[Session->HiddenOrVisible],The_GetSuffix ());

      HTM_TxtColonNBSP (NumGrps == 1 ? Txt_Group  :
				       Txt_Groups);

      /***** Write groups *****/
      if (NumGrps) // Groups found...
	{
	 /* Get and write the group types and names */
	 for (NumGrp = 0;
	      NumGrp < NumGrps;
	      NumGrp++)
	   {
	    /* Get next group */
	    row = mysql_fetch_row (mysql_res);

	    /* Write group type name (row[0]) and group name (row[1]) */
	    HTM_TxtF ("%s&nbsp;%s",row[0],row[1]);

	    if (NumGrps >= 2)
	      {
	       if (NumGrp == NumGrps - 2)
		  HTM_TxtF (" %s ",Txt_and);
	       if (NumGrps >= 3)
		 if (NumGrp < NumGrps - 2)
		     HTM_Txt (", ");
	      }
	   }
	}
      else
	 HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,
		   Gbl.Hierarchy.Node[Hie_CRS].ShrtName);

   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Put a column for visibility of exam session result *************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsResult (struct Exa_Exams *Exams,
                                                const struct ExaSes_Session *Session)
  {
   static void (*Function[Rol_NUM_ROLES]) (struct Exa_Exams *Exams,
	                                   const struct ExaSes_Session *Session) =
     {
      [Rol_STD    ] = ExaSes_ListOneOrMoreSessionsResultStd,
      [Rol_NET    ] = ExaSes_ListOneOrMoreSessionsResultTch,
      [Rol_TCH    ] = ExaSes_ListOneOrMoreSessionsResultTch,
      [Rol_SYS_ADM] = ExaSes_ListOneOrMoreSessionsResultTch,
     };

   HTM_TD_Begin ("rowspan=\"2\" class=\"CT DAT_%s %s\"",
                 The_GetSuffix (),The_GetColorRows ());

      if (Function[Gbl.Usrs.Me.Role.Logged])
	 Function[Gbl.Usrs.Me.Role.Logged] (Exams,Session);
      else
	 Err_WrongRoleExit ();

   HTM_TD_End ();
  }

static void ExaSes_ListOneOrMoreSessionsResultStd (struct Exa_Exams *Exams,
                                                   const struct ExaSes_Session *Session)
  {
   /***** Is exam session result visible or hidden? *****/
   if (Session->ShowUsrResults)
     {
      /* Result is visible by me */
      Exams->Exam.ExaCod = Session->ExaCod;
      Exams->SesCod      = Session->SesCod;
      Lay_PutContextualLinkOnlyIcon (ActSeeMyExaResSes,ExaRes_RESULTS_BOX_ID,
				     ExaSes_PutParsEdit,Exams,
				     "trophy.svg",Ico_BLACK);
     }
   else
      /* Result is forbidden to me */
      Ico_PutIconNotVisible ();
  }

static void ExaSes_ListOneOrMoreSessionsResultTch (struct Exa_Exams *Exams,
                                                   const struct ExaSes_Session *Session)
  {
   extern const char *Txt_Visible_results;
   extern const char *Txt_Hidden_results;

   Exams->Exam.ExaCod = Session->ExaCod;
   Exams->SesCod      = Session->SesCod;

   /***** Show exam session results *****/
   if (ExaSes_CheckIfICanEditThisSession (Session->UsrCod))
      Lay_PutContextualLinkOnlyIcon (ActSeeUsrExaResSes,ExaRes_RESULTS_BOX_ID,
				     ExaSes_PutParsEdit,Exams,
				     "trophy.svg",Ico_BLACK);

   /***** Check if visibility of session results can be changed *****/
   if (ExaSes_CheckIfVisibilityOfResultsCanBeChanged (Session))
     {
      /***** Put form to change visibility of session results *****/
      if (Session->ShowUsrResults)
	 Lay_PutContextualLinkOnlyIcon (ActChgVisExaRes,NULL,
					ExaSes_PutParsEdit,Exams,
					"eye.svg",Ico_GREEN);
      else
	 Lay_PutContextualLinkOnlyIcon (ActChgVisExaRes,NULL,
					ExaSes_PutParsEdit,Exams,
					"eye-slash.svg",Ico_RED);
     }
   else	// Don't put form
     {
      /***** Put icon showing the current visibility of session results *****/
      if (Session->ShowUsrResults)
	 Ico_PutIconOff ("eye.svg"      ,Ico_GREEN,Txt_Visible_results);
      else
	 Ico_PutIconOff ("eye-slash.svg",Ico_RED  ,Txt_Hidden_results);
     }
  }

/*****************************************************************************/
/***************** Toggle visibility of exam session results *****************/
/*****************************************************************************/

void ExaSes_ToggleVisResultsSesUsr (void)
  {
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);
   ExaSes_ResetSession (&Session);

   /***** Get and check parameters *****/
   ExaSes_GetAndCheckPars (&Exams,&Session);

   /***** Check if visibility of session results can be changed *****/
   if (!ExaSes_CheckIfVisibilityOfResultsCanBeChanged (&Session))
      Err_NoPermissionExit ();

   /***** Toggle visibility of exam session results *****/
   Session.ShowUsrResults = !Session.ShowUsrResults;
   Exa_DB_ToggleVisResultsSesUsr (&Session);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,Frm_DONT_PUT_FORM);
  }

/*****************************************************************************/
/******************** Get exam data from a database row **********************/
/*****************************************************************************/

static void ExaSes_GetSessionDataFromRow (MYSQL_RES *mysql_res,
				          struct ExaSes_Session *Session)
  {
   MYSQL_ROW row;
   Dat_StartEndTime_t StartEndTime;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]	SesCod
   row[1]	ExaCod
   row[2]	Hidden
   row[3]	UsrCod
   row[4]	UNIX_TIMESTAMP(StartTime)
   row[5]	UNIX_TIMESTAMP(EndTime)
   row[6]	Open = NOW() BETWEEN StartTime AND EndTime
   row[7]	Title
   row[8]	ShowUsrResults
   */

   /***** Get session data *****/
   /* Code of the session (row[0]) */
   if ((Session->SesCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongExamSessionExit ();

   /* Code of the exam (row[1]) */
   if ((Session->ExaCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
      Err_WrongExamExit ();

   /* Get whether the session is hidden (row[2]) */
   Session->HiddenOrVisible = HidVid_GetHiddenOrVisible (row[2][0]);

   /* Get session teacher (row[3]) */
   Session->UsrCod = Str_ConvertStrCodToLongCod (row[3]);

   /* Get start/end times (row[4], row[5] hold start/end UTC times) */
   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
      Session->TimeUTC[StartEndTime] = Dat_GetUNIXTimeFromStr (row[4 + StartEndTime]);

   /* Get whether the session is open or closed (row(6)) */
   Session->Open = (row[6][0] == '1');

   /* Get the title of the session (row[7]) */
   if (row[7])
      Str_Copy (Session->Title,row[7],sizeof (Session->Title) - 1);
   else
      Session->Title[0] = '\0';

   /* Get whether to show user results or not (row(8)) */
   Session->ShowUsrResults = (row[8][0] == 'Y');
  }

/*****************************************************************************/
/********** Request the removal of an exam session (exam instance) ***********/
/*****************************************************************************/

void ExaSes_ReqRemSession (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_session_X;
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);
   ExaSes_ResetSession (&Session);

   /***** Get and check parameters *****/
   ExaSes_GetAndCheckPars (&Exams,&Session);

   /***** Show question and button to remove question *****/
   Exams.Exam.ExaCod = Session.ExaCod;
   Exams.SesCod      = Session.SesCod;
   Ale_ShowAlertRemove (ActRemExaSes,NULL,
			ExaSes_PutParsEdit,&Exams,
			Txt_Do_you_really_want_to_remove_the_session_X,
	                Session.Title);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,Frm_DONT_PUT_FORM);
  }

/*****************************************************************************/
/****************** Remove an exam session (exam instance) *******************/
/*****************************************************************************/

void ExaSes_RemoveSession (void)
  {
   extern const char *Txt_Session_X_removed;
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);
   ExaSes_ResetSession (&Session);

   /***** Get and check parameters *****/
   ExaSes_GetAndCheckPars (&Exams,&Session);

   /***** Check if I can remove this exam session *****/
   if (!ExaSes_CheckIfICanEditThisSession (Session.UsrCod))
      Err_NoPermissionExit ();

   /***** Remove questions of exams prints, and exam prints, in this session *****/
   //* TODO: DO NOT REMOVE EXAMS PRINTS. Instead move them to tables of deleted prints
   /* To delete orphan exam prints:
   // DELETE FROM exa_print_questions WHERE PrnCod IN (SELECT PrnCod FROM exa_prints WHERE SesCod NOT IN (SELECT SesCod FROM exa_sessions));
   // DELETE FROM exa_prints WHERE SesCod NOT IN (SELECT SesCod FROM exa_sessions);
   */
   Exa_DB_RemovePrintQstsFromSes (Session.SesCod);
   Exa_DB_RemoveAllPrintsFromSes (Session.SesCod);

   /***** Remove the exam session from all database tables *****/
   Exa_DB_RemoveSessionFromAllTables (Session.SesCod);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Session_X_removed,
		  Session.Title);

   /***** Get exam data again to update it after changes in session *****/
   Exa_GetExamDataByCod (&Exams.Exam);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,Frm_DONT_PUT_FORM);
  }

/*****************************************************************************/
/*************************** Hide/unhide a session ***************************/
/*****************************************************************************/

void ExaSes_HideSession (void)
  {
   ExaSes_HideUnhideSession (HidVis_HIDDEN);
  }

void ExaSes_UnhideSession (void)
  {
   ExaSes_HideUnhideSession (HidVis_VISIBLE);
  }

static void ExaSes_HideUnhideSession (HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);
   ExaSes_ResetSession (&Session);

   /***** Get and check parameters *****/
   ExaSes_GetAndCheckPars (&Exams,&Session);

   /***** Check if I can remove this exam session *****/
   if (!ExaSes_CheckIfICanEditThisSession (Session.UsrCod))
      Err_NoPermissionExit ();

   /***** Hide session *****/
   Exa_DB_HideUnhideSession (&Session,HiddenOrVisible);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,Frm_DONT_PUT_FORM);
  }

/*****************************************************************************/
/******************* Params used to edit an exam session *********************/
/*****************************************************************************/

void ExaSes_PutParsEdit (void *Exams)
  {
   if (Exams)
     {
      Exa_PutPars (Exams);
      ParCod_PutPar (ParCod_Ses,((struct Exa_Exams *) Exams)->SesCod);
     }
  }

/*****************************************************************************/
/************************** Get and check parameters *************************/
/*****************************************************************************/

void ExaSes_GetAndCheckPars (struct Exa_Exams *Exams,
                             struct ExaSes_Session *Session)
  {
   /***** Get parameters *****/
   Exa_GetPars (Exams,Exa_CHECK_EXA_COD);
   Grp_GetParWhichGroups ();
   Session->SesCod = ParCod_GetAndCheckPar (ParCod_Ses);

   /***** Get exam data from database *****/
   Exa_GetExamDataByCod (&Exams->Exam);
   if (Exams->Exam.CrsCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
      Err_WrongExamExit ();

   /***** Get set data from database *****/
   ExaSes_GetSessionDataByCod (Session);
   if (Session->ExaCod != Exams->Exam.ExaCod)
      Err_WrongSetExit ();
   Exams->SesCod = Session->SesCod;
  }

/*****************************************************************************/
/* Put a big button to play exam session (start a new session) as a teacher **/
/*****************************************************************************/

static void ExaSes_PutFormSession (struct ExaSes_Session *Session)
  {
   extern const char *Txt_Title;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME] = Dat_HMS_DO_NOT_SET
     };
   bool ItsANewSession = (Session->SesCod <= 0);

   /***** Begin section for exam session *****/
   HTM_SECTION_Begin (ExaSes_NEW_SESSION_SECTION_ID);

      /***** Begin form to create/edit *****/
      Frm_BeginFormTable (ItsANewSession ? ActNewExaSes :
				           ActChgExaSes,
			  ExaSes_NEW_SESSION_SECTION_ID,
			  ExaSes_ParsFormSession,Session);

	 /***** Session title *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","Title",Txt_Title);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_INPUT_TEXT ("Title",ExaSes_MAX_CHARS_TITLE,Session->Title,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "id=\"Title\" class=\"REC_C2_BOT_INPUT INPUT_%s\""
			       " required=\"required\"",
			       The_GetSuffix ());
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Start and end dates *****/
	 Dat_PutFormStartEndClientLocalDateTimes (Session->TimeUTC,
						  Dat_FORM_SECONDS_OFF,
						  SetHMS);

	 /***** Groups *****/
	 ExaSes_ShowLstGrpsToCreateSession (Session->SesCod);

      /***** End form to create *****/
      Frm_EndFormTable (ItsANewSession ? Btn_CREATE_BUTTON :
					 Btn_CONFIRM_BUTTON);

   /***** End section for exam session *****/
   HTM_SECTION_End ();
  }

static void ExaSes_ParsFormSession (void *Session)
  {
   ParCod_PutPar (ParCod_Exa,((struct ExaSes_Session *) Session)->ExaCod);
   ParCod_PutPar (ParCod_Ses,((struct ExaSes_Session *) Session)->SesCod);
  }

/*****************************************************************************/
/************* Show list of groups to create a new exam session **************/
/*****************************************************************************/

static void ExaSes_ShowLstGrpsToCreateSession (long SesCod)
  {
   extern const char *Txt_Groups;
   extern const char *Txt_The_whole_course;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   if (Gbl.Crs.Grps.GrpTypes.NumGrpTypes)
     {
      /***** Begin box and table *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_TxtColon (Txt_Groups);
	 HTM_TD_Begin ("class=\"LT\"");
	    Box_BoxTableBegin ("100%",NULL,
			       NULL,NULL,
			       NULL,Box_NOT_CLOSABLE,0);

	       /***** First row: checkbox to select the whole course *****/
	       HTM_TR_Begin (NULL);

		  HTM_TD_Begin ("colspan=\"7\" class=\"LM DAT_%s\"",
		                The_GetSuffix ());
		     HTM_LABEL_Begin (NULL);
			HTM_INPUT_CHECKBOX ("WholeCrs",HTM_DONT_SUBMIT_ON_CHANGE,
					    "id=\"WholeCrs\" value=\"Y\"%s"
					    " onclick=\"uncheckChildren(this,'GrpCods')\"",
					    Grp_DB_CheckIfAssociatedToGrps ("exa_groups",
					                                    "SesCod",
					                                    SesCod) ? "" :
										      " checked=\"checked\"");
			HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,
				  Gbl.Hierarchy.Node[Hie_CRS].ShrtName);
		     HTM_LABEL_End ();
		  HTM_TD_End ();

	       HTM_TR_End ();

	       /***** List the groups for each group type *****/
	       for (NumGrpTyp = 0;
		    NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
		    NumGrpTyp++)
		  if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
		     Grp_ListGrpsToEditAsgAttSvyEvtMch (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],
							Grp_EXA_EVENT,SesCod);

	    /***** End table and box *****/
	    Box_BoxTableEnd ();
	 HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/*************** Request the creation or edition of a session ****************/
/*****************************************************************************/

void ExaSes_ReqCreatOrEditSes (void)
  {
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;
   bool ItsANewSession;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);
   ExaSes_ResetSession (&Session);

   /***** Get parameters *****/
   Exa_GetPars (&Exams,Exa_CHECK_EXA_COD);
   Grp_GetParWhichGroups ();
   Session.SesCod = ParCod_GetPar (ParCod_Ses);
   ItsANewSession = (Session.SesCod <= 0);

   /***** Get exam data from database *****/
   Exa_GetExamDataByCod (&Exams.Exam);
   if (Exams.Exam.CrsCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
      Err_WrongExamExit ();

   /***** Get session data *****/
   if (ItsANewSession)
      /* Initialize to empty session */
      ExaSes_ResetSession (&Session);
   else
     {
      /* Get session data from database */
      ExaSes_GetSessionDataByCod (&Session);
      if (Exams.Exam.ExaCod != Session.ExaCod)
	 Err_WrongExamExit ();
      Exams.SesCod = Session.SesCod;
     }

   /***** Show exam *****/
   Exa_ShowOnlyOneExam (&Exams,Frm_PUT_FORM);	// Put form for session
  }

/*****************************************************************************/
/****************** Create a new exam session (by a teacher) *****************/
/*****************************************************************************/

void ExaSes_ReceiveFormSession (void)
  {
   extern const char *Txt_Created_new_session_X;
   extern const char *Txt_The_session_has_been_modified;
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;
   bool ItsANewSession;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);
   ExaSes_ResetSession (&Session);

   /***** Get main parameters *****/
   Exa_GetPars (&Exams,Exa_CHECK_EXA_COD);
   Grp_GetParWhichGroups ();
   Session.SesCod = ParCod_GetPar (ParCod_Ses);
   ItsANewSession = (Session.SesCod <= 0);

   /***** Get exam data from database *****/
   Exa_GetExamDataByCod (&Exams.Exam);
   if (Exams.Exam.CrsCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
      Err_WrongExamExit ();

   /***** Get session data from database *****/
   if (ItsANewSession)
     {
      /* Initialize to empty session */
      ExaSes_ResetSession (&Session);
      Session.ExaCod = Exams.Exam.ExaCod;
     }
   else
     {
      /* Get session data from database */
      ExaSes_GetSessionDataByCod (&Session);
      if (Session.ExaCod != Exams.Exam.ExaCod)
	 Err_WrongExamExit ();
      Exams.SesCod = Session.SesCod;
     }

   /***** Get parameters from form *****/
   /* Get session title */
   Par_GetParText ("Title",Session.Title,ExaSes_MAX_BYTES_TITLE);

   /* Get start/end date-times */
   Session.TimeUTC[Dat_STR_TIME] = Dat_GetTimeUTCFromForm (Dat_STR_TIME);
   Session.TimeUTC[Dat_END_TIME] = Dat_GetTimeUTCFromForm (Dat_END_TIME);

   /* Get groups associated to the session */
   Grp_GetParCodsSeveralGrps ();

   /***** Create/update session *****/
   if (ItsANewSession)
      ExaSes_CreateSession (&Session);
   else
     {
      if (Session.TimeUTC[Dat_END_TIME] >= Dat_GetStartExecutionTimeUTC ())	// End of time is in the future
         Session.ShowUsrResults = false;	// Force results to be hidden
      ExaSes_UpdateSession (&Session);
     }

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();

   /***** Write success message *****/
   if (ItsANewSession)
      Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_session_X,
		     Session.Title);
   else
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_session_has_been_modified);

   /***** Get exam data again to update it after changes in session *****/
   Exa_GetExamDataByCod (&Exams.Exam);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,Frm_DONT_PUT_FORM);
  }

/*****************************************************************************/
/*************************** Create a new session ****************************/
/*****************************************************************************/

static void ExaSes_CreateSession (struct ExaSes_Session *Session)
  {
   /***** Insert this new exam session into database *****/
   Session->SesCod = Exa_DB_CreateSession (Session);

   /***** Create groups associated to the exam session *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      ExaSes_CreateGrpsAssociatedToExamSession (Session->SesCod,&Gbl.Crs.Grps.LstGrpsSel);
  }

/*****************************************************************************/
/************************ Update an existing session *************************/
/*****************************************************************************/

static void ExaSes_UpdateSession (struct ExaSes_Session *Session)
  {
   /***** Insert this new exam session into database *****/
   Exa_DB_UpdateSession (Session);

   /***** Update groups associated to the exam session *****/
   Exa_DB_RemoveAllGrpsFromSes (Session->SesCod);	// Remove all groups associated to this session
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      ExaSes_CreateGrpsAssociatedToExamSession (Session->SesCod,&Gbl.Crs.Grps.LstGrpsSel);	// Associate new groups
  }

/*****************************************************************************/
/*************** Create groups associated to an exam session *****************/
/*****************************************************************************/

static void ExaSes_CreateGrpsAssociatedToExamSession (long SesCod,
                                                      const struct ListCodGrps *LstGrpsSel)
  {
   unsigned NumGrpSel;

   /***** Create groups associated to the exam session *****/
   for (NumGrpSel = 0;
	NumGrpSel < LstGrpsSel->NumGrps;
	NumGrpSel++)
      /* Create group */
      Exa_DB_CreateGrpAssociatedToSes (SesCod,LstGrpsSel->GrpCods[NumGrpSel]);
  }

/*****************************************************************************/
/******** Check if I belong to any of the groups of an exam session **********/
/*****************************************************************************/

bool ExaSes_CheckIfICanAnswerThisSession (const struct Exa_Exam *Exam,
                                          const struct ExaSes_Session *Session)
  {
   /***** 1. Sessions in hidden exams are not accesible
          2. Hidden or closed sessions are not accesible *****/
   if (Exam->HiddenOrVisible == HidVis_HIDDEN ||
       Session->HiddenOrVisible == HidVis_HIDDEN ||
       !Session->Open)
      return false;

   /***** Exam is visible, session is visible and open ==>
          ==> I can answer this session if I can list it based on groups *****/
   return ExaSes_CheckIfICanListThisSessionBasedOnGrps (Session->SesCod);
  }

bool ExaSes_CheckIfICanListThisSessionBasedOnGrps (long SesCod)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 /***** Check if I belong to any of the groups
	        associated to the exam session *****/
	 return Exa_DB_CheckIfICanListThisSessionBasedOnGrps (SesCod);
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }
