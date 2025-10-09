// swad_exam_session.c: exam sessions (each ocurrence of an exam)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include "swad_ID.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_role.h"
#include "swad_setting.h"
#include "swad_test.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

const char *ExaSes_ModalityIcon[ExaSes_NUM_MODALITIES] =
  {
   [ExaSes_NONE  ] = NULL,
   [ExaSes_ONLINE] = "display.svg",		// "computer.svg",
   [ExaSes_PAPER ] = "file-signature.svg"	// "print.svg"
  };

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

#define ExaSes_MIN_COLS			1
#define ExaSes_MAX_COLS			4
#define ExaSes_NUM_COLS_DEFAULT		2
#define ExaSes_SHOW_PHOTOS_DEFAULT	true

#define ExaSes_SESSION_BOX_ID		"exases_box"

static const char *ExaSes_NumColsIcon[1 + ExaSes_MAX_COLS] =
  {
   "",		// Not used
   "1col.png",	// 1 column,	ExaSes_MIN_COLS
   "2col.png",	// 2 columns
   "3col.png",	// 3 columns
   "4col.png",	// 4 columns,	ExaSes_MAX_COLS
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaSes_ShowFormColumns (unsigned CurrentNumCols);
static void ExaSes_ShowFormShowPhotos (Pho_ShowPhotos_t ShowPhotos);

static void ExaSes_ShowHeaderResults (void);
static void ExaSes_WriteRowUsrInSession (struct Exa_Exams *Exams,
				         unsigned NumUsr,struct Usr_Data *UsrDat);
static Frm_PutForm_t ExaSes_SetListUsrsAllowedActions (Usr_Can_t AllowedActions[Usr_LIST_USRS_NUM_OPTIONS]);

static void ExaSes_PutIconsInListOfSessions (void *Exams);
static void ExaSes_PutIconToCreateNewSession (struct Exa_Exams *Exams);

static void ExaSes_ListOneOrMoreSessions (struct Exa_Exams *Exams,
      					  Frm_PutForm_t PutFormSession,
				          unsigned NumSessions,
                                          MYSQL_RES *mysql_res);
static void ExaSes_ListOneOrMoreSessionsHeading (Usr_Can_t ICanEditSessions);
static Usr_Can_t ExaSes_CheckIfICanEditSessions (void);
static Usr_Can_t ExaSes_CheckIfICanEditThisSession (long UsrCod);
static Usr_Can_t ExaSes_CheckIfICanChangeVisibilityOfResults (const struct ExaSes_Session *Session);
static void ExaSes_ListOneOrMoreSessionsIcons (struct Exa_Exams *Exams,
                                               const struct ExaSes_Session *Session,
                                               const char *BgColor,
					       const char *Anchor);
static void ExaSes_ListOneOrMoreSessionsAuthor (const struct ExaSes_Session *Session,
						const char *BgColor);
static void ExaSes_ListOneOrMoreSessionsTimes (const struct ExaSes_Session *Session,
					       const char *BgColor,
                                               unsigned UniqueId);
static void ExaSes_ListOneOrMoreSessionsMainData (struct Exa_Exams *Exams,
                                                  const struct ExaSes_Session *Session,
                                                  const char *BgColor,
                                                  const char *Anchor);
static void ExaSes_ListOneOrMoreSessionsPrints (const struct ExaSes_Session *Session,
						unsigned NumPrints,
						const char *BgColor);
static void ExaSes_PutLinkSession (struct Exa_Exams *Exams,
				   const struct ExaSes_Session *Session,
			           const char *Txt);
static void ExaSes_WriteModality (const struct ExaSes_Session *Session);
static void ExaSes_GetAndWriteNamesOfGrpsAssociatedToSession (const struct ExaSes_Session *Session);
static void ExaSes_ListOneOrMoreSessionsResult (struct Exa_Exams *Exams,
                                                const struct ExaSes_Session *Session,
                                                const char *BgColor);
static void ExaSes_ListOneOrMoreSessionsResultStd (struct Exa_Exams *Exams,
                                                   const struct ExaSes_Session *Session);
static void ExaSes_ListOneOrMoreSessionsResultTch (struct Exa_Exams *Exams,
                                                   const struct ExaSes_Session *Session);

static void ExaSes_GetSessionDataFromRow (MYSQL_RES *mysql_res,
				          struct ExaSes_Session *Session);
static ExaSes_Modality_t ExaSes_GetModalityFromString (const char *Str);

static void ExaSes_HideUnhideSession (HidVis_HiddenOrVisible_t HiddenOrVisible);

static void ExaSes_PutFormSession (struct ExaSes_Session *Session,
				   Usr_Can_t ICanChangeModality);
static void ExaSes_ParsFormSession (void *Session);

static void ExaSes_PutSessionModalities (const struct ExaSes_Session *Session,
					 Usr_Can_t ICanChangeModality);
static void ExaSes_ShowLstGrpsToCreateSession (long SesCod);
static void ExaSes_PutSessionNumCols (const struct ExaSes_Session *Session);

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
   Session->SesCod		= -1L;
   Session->ExaCod		= -1L;
   Session->UsrCod		= -1L;
   Session->Modality		= ExaSes_NONE;
   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
      Session->TimeUTC[StartEndTime] = (time_t) 0;
   Session->Title[0]		= '\0';
   Session->Hidden		= HidVis_VISIBLE;
   Session->Open		= CloOpe_CLOSED;
   Session->Show_UsrResults	= Lay_DONT_SHOW;
   Session->NumCols		= ExaSes_NUM_COLS_DEFAULT;
   Session->ShowPhotos		= ExaSes_SHOW_PHOTOS_DEFAULT;
  };

/*****************************************************************************/
/************************ List the sessions of an exam ***********************/
/*****************************************************************************/

void ExaSes_ListSessions (struct Exa_Exams *Exams,Frm_PutForm_t PutFormSession)
  {
   extern const char *Hlp_ASSESSMENT_Exams_sessions;
   extern const char *Txt_Sessions;
   MYSQL_RES *mysql_res;
   unsigned NumSessions;

   /***** Get data of sessions from database *****/
   NumSessions = Exa_DB_GetSessions (&mysql_res,Exams->Exam.ExaCod);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Sessions,ExaSes_PutIconsInListOfSessions,Exams,
		    Hlp_ASSESSMENT_Exams_sessions,Box_NOT_CLOSABLE);

	 /***** Select whether show only my groups or all groups *****/
	 if (Gbl.Crs.Grps.NumGrps && ExaSes_CheckIfICanEditSessions () == Usr_CAN)
	   {
	    Set_BeginSettingsHead ();
	       Grp_ShowFormToSelMyAllGrps (ActSeeOneExa,Exa_PutPars,Exams);
	    Set_EndSettingsHead ();
	   }

	 /***** Show the table with the sessions *****/
	 ExaSes_ListOneOrMoreSessions (Exams,PutFormSession,NumSessions,mysql_res);

      /***** End box *****/
      Box_BoxEnd ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************************* Show one session ****************************/
/*****************************************************************************/

void ExaSes_ShowOneSession (void)
  {
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);
   ExaSes_ResetSession (&Session);

   /***** Get parameters *****/
   Exa_GetPars (&Exams,Exa_CHECK_EXA_COD);
   Exams.SesCod.Sel =
   Exams.SesCod.Par = Session.SesCod = ParCod_GetAndCheckPar (ParCod_Ses);

   /***** Get exam data and session *****/
   Exa_GetExamDataByCod (&Exams.Exam);
   ExaSes_GetSessionDataByCod (&Session);

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,Frm_DONT_PUT_FORM);

      /***** List of users for selection *****/
      ExaSes_ListUsersForSelection (&Exams,&Session);

   /***** Exam end *****/
   Exa_ShowOnlyOneExamEnd ();
  }

/*****************************************************************************/
/**************** List users in an exam session for selection ****************/
/*****************************************************************************/

void ExaSes_ListUsersForSelection (struct Exa_Exams *Exams,
				   const struct ExaSes_Session *Session)
  {
   extern const char *Hlp_ASSESSMENT_Exams;
   extern const char *Txt_Session_X;
   char *Title;
   unsigned NumUsr;
   struct Usr_Data UsrDat;
   Usr_Can_t ICanChooseOption[Usr_LIST_USRS_NUM_OPTIONS];

   /***** Get groups to show ******/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get list of students in this course *****/
   Usr_GetListUsrs (Hie_CRS,Rol_STD);

   /***** Begin section *****/
   HTM_SECTION_Begin (ExaSes_SESSION_BOX_ID);

      /***** Begin box *****/
      if (asprintf (&Title,Txt_Session_X,Session->Title) < 0)
	 Err_NotEnoughMemoryExit ();
      Box_BoxBegin (Title,NULL,NULL,
		    Hlp_ASSESSMENT_Exams,Box_NOT_CLOSABLE);
      free (Title);

	 /***** Form to select groups *****/
         Exams->SesCod.Par = Exams->SesCod.Sel;	// To be used as parameter
	 Grp_ShowFormToSelectSeveralGroups (Exa_PutPars,Exams,NULL);

	 /***** Begin section with user list *****/
	 HTM_SECTION_Begin (Usr_USER_LIST_SECTION_ID);

	    if (Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs)
	      {
	       /***** Initialize structure with user's data *****/
	       Usr_UsrDataConstructor (&UsrDat);

		  /* Form to select users */
		  Frm_BeginFormIdAnchor (Act_DoAct_ExaSes,Usr_FORM_TO_SELECT_USRS_ID,
					 ExaSes_EXAM_SHEET_OPTIONS_SECTION_ID);
		     Exa_PutPars (Exams);
		     Grp_PutParsCodGrps ();
		  Frm_EndForm ();

		  /* Begin table */
		  HTM_TABLE_Begin ("TBL_SCROLL");

		     /***** Begin table with list of students *****/
		     if (!Gbl.Crs.Grps.AllGrpsSel)
		       {
			HTM_TR_Begin (NULL);
			   HTM_TD_Begin ("colspan=\"18\" class=\"TIT CM\"");
			      Grp_WriteNamesOfSelectedGrps ();
			   HTM_TD_End ();
			HTM_TR_End ();
		       }

		     /***** Put a row to select all users *****/
		     HTM_TR_Begin (NULL);
			HTM_TD_Begin ("colspan=\"18\" class=\"LT\"");
			   Usr_PutCheckboxToSelectAllUsers (&Gbl.Usrs.Selected,Rol_STD);
			HTM_TD_End ();
		     HTM_TR_End ();

		     /* Header */
		     ExaSes_ShowHeaderResults ();

		     /* List of students */
		     for (NumUsr = 0;
			  NumUsr < Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs;
			  NumUsr++)
		       {
			/* Copy user's basic data from list */
			Usr_CopyBasicUsrDataFromList (&UsrDat,&Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr]);

			/* Get list of user's IDs */
			ID_GetListIDsFromUsrCod (&UsrDat);

			/* Write a row for this user */
			ExaSes_WriteRowUsrInSession (Exams,NumUsr + 1,&UsrDat);
		       }

		  /* End table */
		  HTM_TABLE_End ();

		  /***** Show pending alerts *****/
		  Ale_ShowAlerts (Usr_USER_LIST_SECTION_ID);

		  /* Options and continue button */
		  HTM_SECTION_Begin (ExaSes_EXAM_SHEET_OPTIONS_SECTION_ID);
		     ExaSes_SetListUsrsAllowedActions (ICanChooseOption);
		     Usr_PutListUsrsActions (ICanChooseOption);
		  HTM_SECTION_End ();

	       /***** Free memory used for user's data *****/
	       Usr_UsrDataDestructor (&UsrDat);
	      }
	    else	// Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs == 0
	       /***** Show warning indicating no students found *****/
	       Usr_ShowWarningNoUsersFound (Rol_STD);

	 /***** End section with user list *****/
	 HTM_SECTION_End ();

      /***** End box *****/
      Box_BoxEnd ();

   /***** End section *****/
   HTM_SECTION_End ();

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (Rol_STD);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/******************** Show form to change settings in exam sheets ************/
/*****************************************************************************/

void ExaSes_ShowFormSettings (const struct ExaSes_Session *Session)
  {
   Set_BeginSettingsHead ();

      /***** Show form to select whether display photos *****/
      ExaSes_ShowFormColumns (Session->NumCols);

      /***** Show form to select columns *****/
      ExaSes_ShowFormShowPhotos (Session->ShowPhotos);

   Set_EndSettingsHead ();
  }

/*****************************************************************************/
/**** Show form to choice whether to show answers in 1, 2, 3 or 4 columns ****/
/*****************************************************************************/

static void ExaSes_ShowFormColumns (unsigned CurrentNumCols)
  {
   extern const char *Txt_column;
   extern const char *Txt_columns;
   char *Title;
   unsigned NumCols;

   Set_BeginOneSettingSelector ();
      for (NumCols  = ExaSes_MIN_COLS;
	   NumCols <= ExaSes_MAX_COLS;
	   NumCols++)
	{
	 Set_BeginPref (NumCols == CurrentNumCols);
	    if (asprintf (&Title,"%u %s",NumCols,
					 NumCols == 1 ? Txt_column :
							Txt_columns) < 0)
	       Err_NotEnoughMemoryExit ();

	    // Input image can not be used to pass a value to the form,
	    // so use a button with an image inside
	    HTM_BUTTON_Submit_Begin (NULL,Usr_FORM_TO_SELECT_USRS_ID,
				     "name=\"NumCols\" value=\"%u\" class=\"BT_LINK\"",
				     NumCols);
	       HTM_IMG (Cfg_URL_ICON_PUBLIC,ExaSes_NumColsIcon[NumCols],Title,
			"class=\"ICO_HIGHLIGHT ICOx20 ICO_%s_%s\"",
			Ico_GetPreffix (Ico_BLACK),The_GetSuffix ());
	    HTM_BUTTON_End ();

	    free (Title);
	 Set_EndPref ();
	}
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/*********** Show form to choice whether to display users' photos ************/
/*****************************************************************************/

static void ExaSes_ShowFormShowPhotos (Pho_ShowPhotos_t ShowPhotos)
  {
   Set_BeginOneSettingSelector ();
      Set_BeginPref (ShowPhotos == Pho_PHOTOS_SHOW);
	 Pho_PutButtonShowPhotos (ShowPhotos,Usr_FORM_TO_SELECT_USRS_ID,NULL);
      Set_EndPref ();
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/**************** Get parameter with the number of columns *******************/
/*****************************************************************************/

unsigned ExaSes_GetParNumCols (unsigned DefaultCols)
  {
   return (unsigned) Par_GetParUnsignedLong ("NumCols",
                                             (unsigned long) ExaSes_MIN_COLS,
                                             (unsigned long) ExaSes_MAX_COLS,
                                             (unsigned long) DefaultCols);	// If parameter does not exists
  }

/*****************************************************************************/
/****************** Update number of columns in database *********************/
/*****************************************************************************/

void ExaSes_UpdateNumCols (struct ExaSes_Session *Session,unsigned NumColsFromForm)
  {
   if (NumColsFromForm != Session->NumCols)	// Different from stored in database
     {
      Session->NumCols = NumColsFromForm;
      Exa_DB_UpdateNumCols (Session);
     }
  }

/*****************************************************************************/
/************ Update whether to display users' photos in database ************/
/*****************************************************************************/

void ExaSes_UpdateShowPhotos (struct ExaSes_Session *Session,
			      Pho_ShowPhotos_t ShowPhotosFromForm)
  {
   if (ShowPhotosFromForm != Pho_PHOTOS_UNKNOWN &&
       ShowPhotosFromForm != Session->ShowPhotos)	// Different from stored in database
     {
      Session->ShowPhotos = ShowPhotosFromForm;
      Exa_DB_UpdateShowPhotos (Session);
     }
  }

/*****************************************************************************/
/********************* Show header of my sessions results *********************/
/*****************************************************************************/

static void ExaSes_ShowHeaderResults (void)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_Valid_answers;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   extern const char *Txt_total;
   extern const char *Txt_QUESTIONS_valid;
   extern const char *Txt_QUESTIONS_invalid;
   extern const char *Txt_ANSWERS_correct;
   extern const char *Txt_ANSWERS_wrong;
   extern const char *Txt_ANSWERS_blank;
   extern const char *Txt_average;

   /***** First row *****/
   HTM_TR_Begin (NULL);
      HTM_TH_Span (NULL                                 ,HTM_HEAD_CENTER,3,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_No_INDEX				,HTM_HEAD_RIGHT ,3,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_User[Usr_SEX_UNKNOWN]		,HTM_HEAD_CENTER,3,2,"LINE_BOTTOM");
      HTM_TH_Span (Txt_START_END_TIME[Dat_STR_TIME]     ,HTM_HEAD_LEFT  ,3,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_START_END_TIME[Dat_END_TIME]     ,HTM_HEAD_LEFT  ,3,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_Questions                        ,HTM_HEAD_CENTER,1,3,"LINE_LEFT");
      HTM_TH_Span (Txt_Valid_answers                    ,HTM_HEAD_CENTER,1,5,"LINE_LEFT");
      HTM_TH_Span (Txt_Score                            ,HTM_HEAD_CENTER,1,2,"LINE_LEFT");
      HTM_TH_Span (Txt_Grade                            ,HTM_HEAD_RIGHT ,3,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span (NULL                                 ,HTM_HEAD_CENTER,3,1,"LINE_BOTTOM LINE_LEFT");
   HTM_TR_End ();

   /***** Second row *****/
   HTM_TR_Begin (NULL);
      HTM_TH_Span (Txt_total                            ,HTM_HEAD_RIGHT ,2,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span (Txt_QUESTIONS_valid                  ,HTM_HEAD_RIGHT ,2,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_QUESTIONS_invalid                ,HTM_HEAD_RIGHT ,2,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_ANSWERS_correct                  ,HTM_HEAD_RIGHT ,1,1,"LINE_LEFT");
      HTM_TH_Span (Txt_ANSWERS_wrong                    ,HTM_HEAD_CENTER,1,3,NULL);
      HTM_TH_Span (Txt_ANSWERS_blank                    ,HTM_HEAD_RIGHT ,1,1,NULL);
      HTM_TH_Span (Txt_total                            ,HTM_HEAD_RIGHT ,1,1,"LINE_LEFT");
      HTM_TH_Span (Txt_average                          ,HTM_HEAD_RIGHT ,1,1,NULL);
   HTM_TR_End ();

   /***** Third row *****/
   HTM_TR_Begin (NULL);
      HTM_TH_Span ("{<em>p<sub>i</sub></em>=1}"         ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span ("{-1&le;<em>p<sub>i</sub></em>&lt;0}",HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM");
      HTM_TH_Span ("{<em>p<sub>i</sub></em>=0}"         ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM");
      HTM_TH_Span ("{0&lt;<em>p<sub>i</sub></em>&lt;1}" ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM");
      HTM_TH_Span ("{<em>p<sub>i</sub></em>=0}"         ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM");
      HTM_TH_Span ("<em>&Sigma;p<sub>i</sub></em>"      ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span ("-1&le;"
	           "<em style=\"text-decoration:overline;\">p</em>"
	           "&le;1"                              ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM");
   HTM_TR_End ();
  }

/*****************************************************************************/
/************** Write a row of a table with the data of a user ***************/
/*****************************************************************************/

static void ExaSes_WriteRowUsrInSession (struct Exa_Exams *Exams,
				         unsigned NumUsr,struct Usr_Data *UsrDat)
  {
   MYSQL_RES *mysql_res;
   unsigned NumResults;
   static unsigned UniqueId = 0;
   char *Id;
   struct ExaPrn_Print Print;
   Dat_StartEndTime_t StartEndTime;
   unsigned NumQstsInvalid;
   double Grade;

   /***** Make database query *****/
   // Do not filter by groups, because a student who has changed groups
   // must be able to access exams taken in other groups
   NumResults = Exa_DB_GetResults (&mysql_res,Usr_OTHER,UsrDat->UsrCod,
				   Exams->SesCod.Sel,-1L,NULL);

   /***** Begin table row *****/
   HTM_TR_Begin (NULL);

      /***** Checkbox to select user *****/
      HTM_TD_Begin ("class=\"CM LINE_BOTTOM %s\"",The_GetColorRows ());
	 Usr_PutCheckboxToSelectUser (Rol_STD,UsrDat,&Gbl.Usrs.Selected);
      HTM_TD_End ();

      /***** Write number of user in the list *****/
      HTM_TD_Begin ("class=\"RM LINE_BOTTOM %s\"",The_GetColorRows ());
	 HTM_Unsigned (NumUsr);
      HTM_TD_End ();

      /***** Show user's data *****/
      Usr_ShowTableCellWithUsrData (UsrDat,NumResults);

      /***** Get and print sessions results *****/
      if (NumResults)	// Only one result per session-user is possible
	{
	 /* Get print code (row[0]) */
	 if ((Print.PrnCod = DB_GetNextCode (mysql_res)) <= 0)
	    Err_WrongExamExit ();

	 /* Get print data */
	 ExaPrn_GetPrintDataByPrnCod (&Print);
	 Str_Copy (Print.EnUsrCod,UsrDat->EnUsrCod,sizeof (Print.EnUsrCod) - 1);

	 /* Get and accumulate questions and score */
	 /* Get questions and user's answers of exam print from database */
	 ExaPrn_GetPrintQuestionsFromDB (&Print);

	 /* Compute score taking into account only valid questions */
	 ExaRes_ComputeValidPrintScore (&Print);
	}

      /* Write start/end times */
      for (StartEndTime  = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
	{
	 UniqueId++;
	 if (asprintf (&Id,"exa_res_time_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	    Err_NotEnoughMemoryExit ();
	 HTM_TD_Begin ("id=\"%s\" class=\"LT DAT_%s LINE_BOTTOM %s\"",
		       Id,The_GetSuffix (),The_GetColorRows ());
	    // If an exam print has been created by a teacher
	    // but has not been answered yet online by the student ==>
	    // ==> the time is 0 ==> don't print anything
	    if (Print.TimeUTC[StartEndTime])
	       Dat_WriteLocalDateHMSFromUTC (Id,Print.TimeUTC[StartEndTime],
					     Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
					     Dat_WRITE_TODAY |
					     Dat_WRITE_DATE_ON_SAME_DAY |
					     Dat_WRITE_HOUR |
					     Dat_WRITE_MINUTE |
					     Dat_WRITE_SECOND);
	 HTM_TD_End ();
	 free (Id);
	}

      /* Write total number of questions */
      HTM_TD_Begin ("class=\"RT DAT_%s LINE_BOTTOM LINE_LEFT %s\"",
		    The_GetSuffix (),The_GetColorRows ());
	 if (NumResults)
	    HTM_Unsigned (Print.NumQsts.All);
      HTM_TD_End ();

      /* Valid questions */
      HTM_TD_Begin ("class=\"RT DAT_GREEN_%s LINE_BOTTOM %s\"",
		    The_GetSuffix (),The_GetColorRows ());
	 if (NumResults)
	    HTM_UnsignedLight0 (Print.NumQsts.Valid.Total);
      HTM_TD_End ();

      /* Invalid questions */
      HTM_TD_Begin ("class=\"RT DAT_RED_%s LINE_BOTTOM %s\"",
		    The_GetSuffix (),The_GetColorRows ());
	 if (NumResults)
	   {
	    NumQstsInvalid = Print.NumQsts.All -
			     Print.NumQsts.Valid.Total;
	    HTM_UnsignedLight0 (NumQstsInvalid);
	   }
      HTM_TD_End ();

      /* Write number of correct questions */
      HTM_TD_Begin ("class=\"RT DAT_%s LINE_BOTTOM LINE_LEFT %s\"",
		    The_GetSuffix (),The_GetColorRows ());
	 if (NumResults)
	    HTM_UnsignedLight0 (Print.NumQsts.Valid.Correct);
      HTM_TD_End ();

      /* Write number of wrong questions */
      HTM_TD_Begin ("class=\"RT DAT_%s LINE_BOTTOM %s\"",
		    The_GetSuffix (),The_GetColorRows ());
	 if (NumResults)
	    HTM_UnsignedLight0 (Print.NumQsts.Valid.Wrong.Negative);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RT DAT_%s LINE_BOTTOM %s\"",
		    The_GetSuffix (),The_GetColorRows ());
	 if (NumResults)
	    HTM_UnsignedLight0 (Print.NumQsts.Valid.Wrong.Zero);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"RT DAT_%s LINE_BOTTOM %s\"",
		    The_GetSuffix (),The_GetColorRows ());
	 if (NumResults)
	    HTM_UnsignedLight0 (Print.NumQsts.Valid.Wrong.Positive);
      HTM_TD_End ();

      /* Write number of blank questions */
      HTM_TD_Begin ("class=\"RT DAT_%s LINE_BOTTOM %s\"",
		    The_GetSuffix (),The_GetColorRows ());
	 if (NumResults)
	    HTM_UnsignedLight0 (Print.NumQsts.Valid.Blank);
      HTM_TD_End ();

      /* Write score valid (taking into account only valid questions) */
      HTM_TD_Begin ("class=\"RT DAT_%s LINE_BOTTOM LINE_LEFT %s\"",
		    The_GetSuffix (),The_GetColorRows ());
	 if (NumResults)
	    HTM_DoublePartOfUnsigned (Print.Score.Valid,
				      Print.NumQsts.Valid.Total);
      HTM_TD_End ();

      /* Write average score per question (taking into account only valid questions) */
      HTM_TD_Begin ("class=\"RT DAT_%s LINE_BOTTOM %s\"",
		    The_GetSuffix (),The_GetColorRows ());
	 if (NumResults)
	    HTM_Double2Decimals (Print.NumQsts.Valid.Total ? Print.Score.Valid /
							     (double) Print.NumQsts.Valid.Total :
							     0.0);
      HTM_TD_End ();

      /* Write grade over maximum grade (taking into account only valid questions) */
      HTM_TD_Begin ("class=\"RT DAT_%s LINE_BOTTOM LINE_LEFT %s\"",
		    The_GetSuffix (),The_GetColorRows ());
	 if (NumResults)
	   {
	    Grade = TstPrn_ComputeGrade (Print.NumQsts.Valid.Total,
					 Print.Score.Valid,
					 Exams->Exam.MaxGrade);
	    HTM_DoublePartOfDouble (Grade,Exams->Exam.MaxGrade);
	   }
      HTM_TD_End ();

      /* Link to show this result */
      HTM_TD_Begin ("class=\"RT LINE_BOTTOM LINE_LEFT %s\"",The_GetColorRows ());
	 if (NumResults)
	   {
	    Frm_BeginForm (ActSeeOneExaResOth);
	       Exa_PutPars (Exams);
	       Usr_PutParOtherUsrCodEncrypted (UsrDat->EnUsrCod);
	       Ico_PutIconLink ("tasks.svg",Ico_BLACK,ActSeeOneExaResOth);
	    Frm_EndForm ();
	   }
      HTM_TD_End ();

   /***** End last row *****/
   HTM_TR_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   The_ChangeRowColor ();
  }

/*****************************************************************************/
/**************** Set allowed options to do with several users ***************/
/*****************************************************************************/
// Returns true if any option is allowed

static Frm_PutForm_t ExaSes_SetListUsrsAllowedActions (Usr_Can_t AllowedActions[Usr_LIST_USRS_NUM_OPTIONS])
  {
   Usr_ListUsrsAction_t Act;

   /***** Check which actions I can choose *****/
   /* Set default (I can not choose action) */
   for (Act  = (Usr_ListUsrsAction_t) 1;	// Skip unknown action
	Act <= (Usr_ListUsrsAction_t) (Usr_LIST_USRS_NUM_OPTIONS - 1);
	Act++)
      AllowedActions[Act] = Usr_CAN_NOT;

   /* Activate some options */
   AllowedActions[Usr_ACT_EXAMS_QST_SHEETS		] =
   AllowedActions[Usr_ACT_BLANK_EXAMS_ANS_SHEETS	] =
   AllowedActions[Usr_ACT_SOLVD_EXAMS_ANS_SHEETS	] = Usr_CAN;

   return Frm_PUT_FORM;
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
   switch (Exa_DB_GetSessionDataByCod (&mysql_res,Session->SesCod))
     {
      case Exi_EXISTS:
	 /* Get exam session data from row */
	 ExaSes_GetSessionDataFromRow (mysql_res,Session);
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 /* Initialize to empty exam session */
	 ExaSes_ResetSession (Session);
	 break;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Put icons in list of sessions of an exam ******************/
/*****************************************************************************/

static void ExaSes_PutIconsInListOfSessions (void *Exams)
  {
   if (Exams)
      if (ExaSes_CheckIfICanEditSessions () == Usr_CAN)
         /***** Put icon to create a new exam session in current exam *****/
	 ExaSes_PutIconToCreateNewSession ((struct Exa_Exams *) Exams);
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
   const char *BgColor;
   char *Anchor;
   unsigned NumPrints;
   Usr_Can_t ICanEditSessions = ExaSes_CheckIfICanEditSessions ();
   Usr_Can_t ICanChangeModality = false;

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

	 Exams->SesCod.Par = Session.SesCod;	// To be used as hidden parameter in forms

	 if (ExaSes_CheckIfICanListThisSessionBasedOnGrps (Session.SesCod) == Usr_CAN)
	   {
	    BgColor = Session.SesCod == Exams->SesCod.Sel ? "BG_HIGHLIGHT" :
							    The_GetColorRows ();

	    /***** Build anchor string *****/
	    if (asprintf (&Anchor,"evt_%ld_%ld",Exams->Exam.ExaCod,Session.SesCod) < 0)
	       Err_NotEnoughMemoryExit ();

	    /***** First row for this session ****/
	    HTM_TR_Begin (NULL);

	       /* Icons */
	       if (ICanEditSessions == Usr_CAN)
		  ExaSes_ListOneOrMoreSessionsIcons (Exams,&Session,BgColor,Anchor);

	       /* Start/end date/time */
	       ExaSes_ListOneOrMoreSessionsTimes (&Session,BgColor,UniqueId);

	       /* Title, modality and groups */
	       ExaSes_ListOneOrMoreSessionsMainData (Exams,&Session,BgColor,Anchor);

	       /* Prints in the session */
	       if (ICanEditSessions == Usr_CAN)
	         {
		  NumPrints = Exa_DB_GetNumPrintsInSes (Session.SesCod);
		  ICanChangeModality = NumPrints == 0 ? Usr_CAN : // Only if there are no exam prints yet
							Usr_CAN_NOT;
	          ExaSes_ListOneOrMoreSessionsPrints (&Session,NumPrints,BgColor);
	         }

	       /* Session result visible? */
	       ExaSes_ListOneOrMoreSessionsResult (Exams,&Session,BgColor);

	    HTM_TR_End ();

	    /***** Second row: session author */
	    HTM_TR_Begin (NULL);
	       ExaSes_ListOneOrMoreSessionsAuthor (&Session,BgColor);
	    HTM_TR_End ();

	    /***** Third row: form to edit this session ****/
	    if (ICanEditSessions == Usr_CAN &&
		PutFormSession == Frm_PUT_FORM &&	// Editing...
		Session.SesCod == Exams->SesCod.Sel)	// ...this session
	      {
	       HTM_TR_Begin (NULL);
		  HTM_TD_Begin ("colspan=\"6\" class=\"LT %s\"",BgColor);
		     ExaSes_PutFormSession (&Session,ICanChangeModality);
		  HTM_TD_End ();
	       HTM_TR_End ();
	      }

	    /***** Free anchor string *****/
	    free (Anchor);
	   }
	}

      /***** Put form to create a new exam session in this exam *****/
      if (ICanEditSessions == Usr_CAN &&
	  PutFormSession == Frm_PUT_FORM &&
	  Exams->SesCod.Sel <= 0)
	{
	 /* Reset session */
	 ExaSes_ResetSession (&Session);
	 Session.ExaCod = Exams->Exam.ExaCod;
	 Session.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();		// Now
	 Session.TimeUTC[Dat_END_TIME] = Session.TimeUTC[Dat_STR_TIME] + (1 * 60 * 60);	// Now + 1 hour
	 Str_Copy (Session.Title,Exams->Exam.Title,sizeof (Session.Title) - 1);

	 /* Put form to create new session */
	 HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"6\" class=\"LT %s\"",The_GetColorRows ());
	       ExaSes_PutFormSession (&Session,
				      Usr_CAN);	// Modality can be changed
	    HTM_TD_End ();
	 HTM_TR_End ();
	}

   /***** End table with sessions *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************* Put a column for exam session start and end times *************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsHeading (Usr_Can_t ICanEditSessions)
  {
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Session;
   extern const char *Txt_Exam_printouts;
   extern const char *Txt_Results;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Column for icons *****/
      if (ICanEditSessions == Usr_CAN)
	 HTM_TH_Empty (1);

      /***** The rest of columns *****/
      HTM_TH (Txt_START_END_TIME[Exa_ORDER_BY_STR_DATE]	,HTM_HEAD_LEFT);
      HTM_TH (Txt_START_END_TIME[Exa_ORDER_BY_END_DATE]	,HTM_HEAD_LEFT);
      HTM_TH (Txt_Session				,HTM_HEAD_LEFT);
      if (ICanEditSessions == Usr_CAN)
	 HTM_TH (Txt_Exam_printouts			,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Results				,HTM_HEAD_CENTER);

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/********************** Check if I can edit sessions *************************/
/*****************************************************************************/

static Usr_Can_t ExaSes_CheckIfICanEditSessions (void)
  {
   static Usr_Can_t ICanEditSessions[Rol_NUM_ROLES] =
     {
      [Rol_NET    ] = Usr_CAN,
      [Rol_TCH    ] = Usr_CAN,
      [Rol_SYS_ADM] = Usr_CAN,
     };

   return ICanEditSessions[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/************ Check if I can edit (remove/resume) an exam session ************/
/*****************************************************************************/

static Usr_Can_t ExaSes_CheckIfICanEditThisSession (long UsrCod)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
	 return UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod ? Usr_CAN :	// Only if I am the creator
						      Usr_CAN_NOT;
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return Usr_CAN;
      default:
	 return Usr_CAN_NOT;
     }
  }

/*****************************************************************************/
/********** Check if visibility of session results can be changed ************/
/*****************************************************************************/

static Usr_Can_t ExaSes_CheckIfICanChangeVisibilityOfResults (const struct ExaSes_Session *Session)
  {
   if (Session->Show_UsrResults == Lay_SHOW ||					// Results are currently visible
       Session->TimeUTC[Dat_END_TIME] < Dat_GetStartExecutionTimeUTC ())	// End of time is in the past
      return ExaSes_CheckIfICanEditThisSession (Session->UsrCod);

   return Usr_CAN_NOT;
  }

/*****************************************************************************/
/************************* Put a column for icons ****************************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsIcons (struct Exa_Exams *Exams,
                                               const struct ExaSes_Session *Session,
                                               const char *BgColor,
					       const char *Anchor)
  {
   static Act_Action_t ActionHideUnhide[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = ActUnhExaSes,	// Hidden ==> action to unhide
      [HidVis_VISIBLE] = ActHidExaSes,	// Visible ==> action to hide
     };

   /***** Begin cell *****/
   HTM_TD_Begin ("rowspan=\"2\" class=\"BT %s\"",BgColor);

      if (ExaSes_CheckIfICanEditThisSession (Session->UsrCod) == Usr_CAN)
	{
	 /***** Icon to remove the exam session *****/
	 Ico_PutContextualIconToRemove (ActReqRemExaSes,NULL,
					Exa_PutPars,Exams);

	 /***** Icon to hide/unhide the exam session *****/
	 Ico_PutContextualIconToHideUnhide (ActionHideUnhide,Anchor,
					    Exa_PutPars,Exams,
					    Session->Hidden);

	 /***** Icon to edit the exam session *****/
	 Ico_PutContextualIconToEdit (ActReqChgExaSes,Anchor,
				      Exa_PutPars,Exams);
	}

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/********** Put a column for teacher who created the exam session ************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsAuthor (const struct ExaSes_Session *Session,
						const char *BgColor)
  {
   /***** Session author (teacher) *****/
   HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",BgColor);
      Usr_WriteAuthor1Line (Session->UsrCod,Session->Hidden);
   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Put a column for exam session start and end times *************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsTimes (const struct ExaSes_Session *Session,
					       const char *BgColor,
                                               unsigned UniqueId)
  {
   extern const char *CloOpe_Class[CloOpe_NUM_CLOSED_OPEN][HidVis_NUM_HIDDEN_VISIBLE];
   Dat_StartEndTime_t StartEndTime;
   char *Id;

   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      if (asprintf (&Id,"exa_time_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
		    Id,
		    CloOpe_Class[Session->Open][Session->Hidden],The_GetSuffix (),
		    BgColor);
	 Dat_WriteLocalDateHMSFromUTC (Id,Session->TimeUTC[StartEndTime],
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       Dat_WRITE_TODAY |
				       Dat_WRITE_DATE_ON_SAME_DAY |
				       Dat_WRITE_WEEK_DAY |
				       Dat_WRITE_HOUR |
				       Dat_WRITE_MINUTE);
      HTM_TD_End ();
      free (Id);
     }
  }

/*****************************************************************************/
/************** Put a column for exam session title and groups ***************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsMainData (struct Exa_Exams *Exams,
                                                  const struct ExaSes_Session *Session,
                                                  const char *BgColor,
                                                  const char *Anchor)
  {
   extern const char *HidVis_TitleClass[HidVis_NUM_HIDDEN_VISIBLE];


   HTM_TD_Begin ("rowspan=\"2\" class=\"LT %s\"",BgColor);

      /***** Session title *****/
      HTM_ARTICLE_Begin (Anchor);

	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_STD:
	       switch (ExaSes_CheckIfICanAnswerThisSession (&Exams->Exam,Session))
		 {
		  case Usr_CAN:
		     Frm_BeginForm (ActSeeExaPrn);
			Exa_PutPars (Exams);
			HTM_BUTTON_Submit_Begin (Act_GetActionText (ActSeeExaPrn),NULL,
						 "class=\"LT BT_LINK %s_%s\"",
						 HidVis_TitleClass[Session->Hidden],
						 The_GetSuffix ());
			   HTM_Txt (Session->Title);
			HTM_BUTTON_End ();
		     Frm_EndForm ();
		     break;
		  case Usr_CAN_NOT:
		  default:
		     HTM_SPAN_Begin ("class=\"%s_%s\"",
				     HidVis_TitleClass[Session->Hidden],
				     The_GetSuffix ());
			HTM_Txt (Session->Title);
		     HTM_SPAN_End ();
		     break;
		 }
	       break;
	    case Rol_NET:
	    case Rol_TCH:
	    case Rol_SYS_ADM:
	       ExaSes_PutLinkSession (Exams,Session,Session->Title);
	       break;
	    default:
	       Err_NoPermissionExit ();
	   }

      HTM_ARTICLE_End ();

      /***** Modality *****/
      ExaSes_WriteModality (Session);

      /***** Groups whose students can answer this exam session *****/
      if (Gbl.Crs.Grps.NumGrps)
	 ExaSes_GetAndWriteNamesOfGrpsAssociatedToSession (Session);

   HTM_TD_End ();
  }

/*****************************************************************************/
/************ Put a column for number of prints in an exam session ***********/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsPrints (const struct ExaSes_Session *Session,
						unsigned NumPrints,
						const char *BgColor)
  {
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];

   HTM_TD_Begin ("rowspan=\"2\" class=\"RT %s_%s %s\"",
                 HidVis_DataClass[Session->Hidden],The_GetSuffix (),BgColor);
      HTM_Unsigned (NumPrints);
   HTM_TD_End ();
  }

/*****************************************************************************/
/********************* Put link to view an exam session **********************/
/*****************************************************************************/

static void ExaSes_PutLinkSession (struct Exa_Exams *Exams,
				   const struct ExaSes_Session *Session,
			           const char *Txt)
  {
   extern const char *HidVis_TitleClass[HidVis_NUM_HIDDEN_VISIBLE];

   /***** Begin form *****/
   Frm_BeginFormAnchor (ActSeeOneExaSes,ExaSes_SESSION_BOX_ID);
      Exa_PutPars (Exams);
      Grp_PutParsCodGrpsAssociated (Grp_EXAM_SESSION,Exams->SesCod.Par);

      /***** Link to view attendance event *****/
      HTM_BUTTON_Submit_Begin (Act_GetActionText (ActSeeOneExaSes),NULL,
			       "class=\"LT BT_LINK %s_%s\"",
			       HidVis_TitleClass[Session->Hidden],
			       The_GetSuffix ());
	 HTM_Txt (Txt);
      HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************************** Write session modality **************************/
/*****************************************************************************/

static void ExaSes_WriteModality (const struct ExaSes_Session *Session)
  {
   extern const char *HidVis_GroupClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_EXAM_SESSION_Modality;
   extern const char *Txt_EXAM_SESSION_MODALITIES[ExaSes_NUM_MODALITIES];

   HTM_DIV_Begin ("class=\"%s_%s\"",
                  HidVis_GroupClass[Session->Hidden],The_GetSuffix ());

      /* Label */
      HTM_Txt (Txt_EXAM_SESSION_Modality); HTM_Colon (); HTM_NBSP ();

      /* Modality */
      Ico_PutIconOff (ExaSes_ModalityIcon[Session->Modality],Ico_BLACK,
		      Txt_EXAM_SESSION_MODALITIES[Session->Modality]);
      HTM_NBSP ();
      HTM_Txt (Txt_EXAM_SESSION_MODALITIES[Session->Modality]);

   HTM_DIV_End ();
  }

/*****************************************************************************/
/********* Get and write the names of the groups of an exam session **********/
/*****************************************************************************/

static void ExaSes_GetAndWriteNamesOfGrpsAssociatedToSession (const struct ExaSes_Session *Session)
  {
   extern const char *HidVis_GroupClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrps;
   unsigned NumGrp;

   /***** Get groups *****/
   NumGrps = Exa_DB_GetGrpsAssociatedToSes (&mysql_res,Session->SesCod);

   /***** Write heading *****/
   HTM_DIV_Begin ("class=\"%s_%s\"",
                  HidVis_GroupClass[Session->Hidden],The_GetSuffix ());

      HTM_Txt (NumGrps == 1 ? Txt_Group  :
			      Txt_Groups);
      HTM_Colon (); HTM_NBSP ();

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
	    HTM_Txt (row[0]);
	    HTM_SP ();
	    HTM_Txt (row[1]);

	    /* Write separator */
	    HTM_ListSeparator (NumGrp,NumGrps);
	   }
	}
      else
	 Grp_WriteTheWholeCourse ();

   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Put a column for visibility of exam session result *************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsResult (struct Exa_Exams *Exams,
                                                const struct ExaSes_Session *Session,
                                                const char *BgColor)
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
                 The_GetSuffix (),BgColor);

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
   switch (Session->Show_UsrResults)
     {
      case Lay_SHOW:
	 /* Result is visible by me */
	 Exams->Exam.ExaCod = Session->ExaCod;
	 Exams->SesCod.Par  = Session->SesCod;
	 Lay_PutContextualLinkOnlyIcon (ActSeeMyExaResSes,ExaRes_RESULTS_BOX_ID,
					Exa_PutPars,Exams,
					"trophy.svg",Ico_BLACK);
	 break;
      case Lay_DONT_SHOW:
      default:
	 /* Result is forbidden to me */
	 Ico_PutIconNotVisible ();
	 break;
     }
  }

// TODO: this function is similar to Mch_ListOneOrMoreMatchesResultTch
static void ExaSes_ListOneOrMoreSessionsResultTch (struct Exa_Exams *Exams,
                                                   const struct ExaSes_Session *Session)
  {
   extern const char *Txt_Visible_results;
   extern const char *Txt_Hidden_results;
   static struct
     {
      const char *Icon;
      Ico_Color_t Color;
      const char **Title;
     } IconPars[Lay_NUM_SHOW] =
     {
      [Lay_DONT_SHOW] =	// Don't show user's results
        {
         .Icon  = "eye-slash.svg",
         .Color = Ico_RED,
         .Title = &Txt_Hidden_results
        },
      [Lay_SHOW] =	// Show user's results
        {
         .Icon  = "eye.svg",
         .Color = Ico_GREEN,
         .Title = &Txt_Visible_results
        },
     };


   Exams->Exam.ExaCod = Session->ExaCod;
   Exams->SesCod.Par  = Session->SesCod;

   /***** Show exam session results *****/
   if (ExaSes_CheckIfICanEditThisSession (Session->UsrCod) == Usr_CAN)
      Lay_PutContextualLinkOnlyIcon (ActSeeUsrExaResSes,ExaRes_RESULTS_BOX_ID,
				     Exa_PutPars,Exams,
				     "trophy.svg",Ico_BLACK);

   /***** Check if visibility of session results can be changed *****/
   switch (ExaSes_CheckIfICanChangeVisibilityOfResults (Session))
     {
      case Usr_CAN:
	 /***** Put form to change visibility of session results *****/
	 Lay_PutContextualLinkOnlyIcon (ActChgVisExaRes,NULL,
					Exa_PutPars,Exams,
					IconPars[Session->Show_UsrResults].Icon,
					IconPars[Session->Show_UsrResults].Color);
	 break;
      case Usr_CAN_NOT:	// Don't put form
      default:
	 /***** Put icon showing the current visibility of session results *****/
	 Ico_PutIconOff ( IconPars[Session->Show_UsrResults].Icon,
			  IconPars[Session->Show_UsrResults].Color,
			 *IconPars[Session->Show_UsrResults].Title);
	 break;
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
   if (ExaSes_CheckIfICanChangeVisibilityOfResults (&Session) == Usr_CAN_NOT)
      Err_NoPermissionExit ();

   /***** Toggle visibility of exam session results *****/
   Session.Show_UsrResults = Session.Show_UsrResults == Lay_SHOW ? Lay_DONT_SHOW :
								   Lay_SHOW;
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
   row[ 0]	SesCod
   row[ 1]	ExaCod
   row[ 2]	Hidden
   row[ 3]	UsrCod
   row[ 4]	Modality
   row[ 5]	UNIX_TIMESTAMP(StartTime)
   row[ 6]	UNIX_TIMESTAMP(EndTime)
   row[ 7]	Open = NOW() BETWEEN StartTime AND EndTime
   row[ 8]	Title
   row[ 9]	ShowUsrResults
   row[10]	NumCols
   row[11]	ShowPhotos
   */

   /***** Get session data *****/
   /* Code of the session (row[0]) */
   if ((Session->SesCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongExamSessionExit ();

   /* Code of the exam (row[1]) */
   if ((Session->ExaCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
      Err_WrongExamExit ();

   /* Get whether the session is hidden (row[2]) */
   Session->Hidden = HidVis_GetHiddenFromYN (row[2][0]);

   /* Get session teacher (row[3]) */
   Session->UsrCod = Str_ConvertStrCodToLongCod (row[3]);

   /* Get modality (row[4]) */
   Session->Modality = ExaSes_GetModalityFromString (row[4]);

   /* Get start/end times (row[5], row[6] hold start/end UTC times) */
   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
      Session->TimeUTC[StartEndTime] = Dat_GetUNIXTimeFromStr (row[5 + StartEndTime]);

   /* Get whether the session is open or closed (row(7)) */
   Session->Open = CloOpe_GetClosedOrOpenFrom01 (row[7][0]);

   /* Get the title of the session (row[8]) */
   Str_Copy (Session->Title,row[8],sizeof (Session->Title) - 1);

   /* Get whether to show user results or not (row(9)) */
   Session->Show_UsrResults = Lay_GetShowFromYN (row[9][0]);

   /* Get number of columns (row[10]) */
   Session->NumCols = Str_ConvertStrToUnsigned (row[10]);
   if (Session->NumCols < ExaSes_MIN_COLS ||
       Session->NumCols > ExaSes_MAX_COLS)
      Session->NumCols = ExaSes_NUM_COLS_DEFAULT;

   /* Get whether to display users' photos */
   Session->ShowPhotos = row[11][0] == 'Y' ? Pho_PHOTOS_SHOW :
					     Pho_PHOTOS_DONT_SHOW;
  }

/*****************************************************************************/
/********************** Convert from string to type **************************/
/*****************************************************************************/

static ExaSes_Modality_t ExaSes_GetModalityFromString (const char *Str)
  {
   extern const char *Exa_DB_Modality[ExaSes_NUM_MODALITIES];
   ExaSes_Modality_t Modality;

   /***** Compare string with all string modalities *****/
   for (Modality  = (ExaSes_Modality_t) 0;
	Modality <= (ExaSes_Modality_t) (ExaSes_NUM_MODALITIES - 1);
	Modality++)
      if (!strcmp (Exa_DB_Modality[Modality],Str))
	 return Modality;

   return ExaSes_NONE;
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
   Exams.SesCod.Sel  =
   Exams.SesCod.Par  = Session.SesCod;
   Ale_ShowAlertRemove (ActRemExaSes,NULL,
			Exa_PutPars,&Exams,
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
   if (ExaSes_CheckIfICanEditThisSession (Session.UsrCod) == Usr_CAN_NOT)
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
   Ale_ShowAlert (Ale_SUCCESS,Txt_Session_X_removed,Session.Title);

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
   if (ExaSes_CheckIfICanEditThisSession (Session.UsrCod) == Usr_CAN_NOT)
      Err_NoPermissionExit ();

   /***** Hide session *****/
   Exa_DB_HideUnhideSession (&Session,HiddenOrVisible);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,Frm_DONT_PUT_FORM);
  }

/*****************************************************************************/
/************************** Get and check parameters *************************/
/*****************************************************************************/

void ExaSes_GetAndCheckPars (struct Exa_Exams *Exams,
                             struct ExaSes_Session *Session)
  {
   /***** Get parameters *****/
   Exa_GetPars (Exams,Exa_CHECK_EXA_COD);
   Grp_GetParMyAllGrps ();
   Session->SesCod = ParCod_GetAndCheckPar (ParCod_Ses);

   /***** Get exam data from database *****/
   Exa_GetExamDataByCod (&Exams->Exam);
   if (Exams->Exam.CrsCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
      Err_WrongExamExit ();

   /***** Get set data from database *****/
   ExaSes_GetSessionDataByCod (Session);
   if (Session->ExaCod != Exams->Exam.ExaCod)
      Err_WrongSetExit ();
   Exams->SesCod.Sel =
   Exams->SesCod.Par = Session->SesCod;
  }

/*****************************************************************************/
/* Put a big button to play exam session (start a new session) as a teacher **/
/*****************************************************************************/

static void ExaSes_PutFormSession (struct ExaSes_Session *Session,
				   Usr_Can_t ICanChangeModality)
  {
   extern const char *Txt_Title;
   extern const char *Txt_EXAM_SESSION_Modality;
   extern const char *Txt_Columns;
   static struct
     {
      Act_Action_t Action;
      Btn_Button_t Button;
     } Forms[OldNew_NUM_OLD_NEW] =
     {
      [OldNew_OLD] = {ActChgExaSes,Btn_SAVE_CHANGES},
      [OldNew_NEW] = {ActNewExaSes,Btn_CREATE      }
     };
   static Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME] = Dat_HMS_DO_NOT_SET
     };
   OldNew_OldNew_t OldNewSession = Session->SesCod > 0 ? OldNew_OLD :
							 OldNew_NEW;

   /***** Begin section for exam session *****/
   HTM_SECTION_Begin (ExaSes_NEW_SESSION_SECTION_ID);

      /***** Begin form to create/edit *****/
      Frm_BeginFormTable (Forms[OldNewSession].Action,
			  ExaSes_NEW_SESSION_SECTION_ID,
			  ExaSes_ParsFormSession,Session,
			  "TBL_WIDE");

	 /***** Title *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","Title",Txt_Title);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	       HTM_INPUT_TEXT ("Title",ExaSes_MAX_CHARS_TITLE,Session->Title,
			       HTM_REQUIRED,
			       "id=\"Title\" class=\"Frm_C2_INPUT INPUT_%s\"",
			       The_GetSuffix ());
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Modality *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","Modality",Txt_EXAM_SESSION_Modality);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	       ExaSes_PutSessionModalities (Session,ICanChangeModality);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Start and end dates *****/
	 Dat_PutFormStartEndClientLocalDateTimes (Session->TimeUTC,
						  Dat_FORM_SECONDS_OFF,
						  SetHMS);

	 /***** Groups *****/
	 ExaSes_ShowLstGrpsToCreateSession (Session->SesCod);

	 /***** Number of columns *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","NumCols",Txt_Columns);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	       ExaSes_PutSessionNumCols (Session);
	    HTM_TD_End ();

	 HTM_TR_End ();

      /***** End form to create *****/
      Frm_EndFormTable (Forms[OldNewSession].Button);

   /***** End section for exam session *****/
   HTM_SECTION_End ();
  }

static void ExaSes_ParsFormSession (void *Session)
  {
   ParCod_PutPar (ParCod_Exa,((struct ExaSes_Session *) Session)->ExaCod);
   ParCod_PutPar (ParCod_Ses,((struct ExaSes_Session *) Session)->SesCod);
  }

/*****************************************************************************/
/*********************** Modalities in an exam session ***********************/
/*****************************************************************************/

static void ExaSes_PutSessionModalities (const struct ExaSes_Session *Session,
					 Usr_Can_t ICanChangeModality)
  {
   extern const char *Txt_EXAM_SESSION_MODALITIES[ExaSes_NUM_MODALITIES];
   ExaSes_Modality_t Modality;

   /***** Begin list of checkboxes *****/
   HTM_UL_Begin ("class=\"LIST_LEFT FORM_IN_%s\"",The_GetSuffix ());

      /***** *****/
      for (Modality  = (ExaSes_Modality_t) 1;
	   Modality <= (ExaSes_Modality_t) (ExaSes_NUM_MODALITIES - 1);
	   Modality++)
        {
	 HTM_LI_Begin (NULL);
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_RADIO ("Modality",
				(Modality == Session->Modality ? HTM_REQUIRED | HTM_CHECKED :
				(ICanChangeModality == Usr_CAN ? HTM_REQUIRED :
								 HTM_DISABLED)),
				" value=\"%u\"",(unsigned) Modality);
	       Ico_PutIconOn (ExaSes_ModalityIcon[Modality],Ico_BLACK,
			      Txt_EXAM_SESSION_MODALITIES[Modality]);
	       HTM_NBSP ();
	       HTM_Txt (Txt_EXAM_SESSION_MODALITIES[Modality]);
	    HTM_LABEL_End ();
	 HTM_LI_End ();
        }

   /***** End list of checkboxes *****/
   HTM_UL_End ();
  }

/*****************************************************************************/
/************* Show list of groups to create a new exam session **************/
/*****************************************************************************/

static void ExaSes_ShowLstGrpsToCreateSession (long SesCod)
  {
   extern const char *Txt_Groups;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_GRP_TYPES_WITH_GROUPS);

   if (Gbl.Crs.Grps.GrpTypes.NumGrpTypes)
     {
      /***** Begin row *****/
      HTM_TR_Begin (NULL);

         /* Label */
	 Frm_LabelColumn ("Frm_C1 RT","",Txt_Groups);

	 /* Groups */
	 HTM_TD_Begin ("class=\"Frm_C2 LT\"");

	    /***** First row: checkbox to select the whole course *****/
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_CHECKBOX ("WholeCrs",
				   Grp_DB_CheckIfAssociatedToGrps ("exa_groups",
								   "SesCod",
								   SesCod) ? HTM_NO_ATTR :
									     HTM_CHECKED,
				   "id=\"WholeCrs\" value=\"Y\""
				   " onclick=\"uncheckChildren(this,'GrpCods')\"");
	       Grp_WriteTheWholeCourse ();
	    HTM_LABEL_End ();

	    /***** List the groups for each group type *****/
	    Grp_ListGrpsToEditAsgAttSvyEvtMch (Grp_EXA_EVENT,SesCod);

	 HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/******************** Number of columns in an exam session *******************/
/*****************************************************************************/

static void ExaSes_PutSessionNumCols (const struct ExaSes_Session *Session)
  {
   extern const char *Txt_column;
   extern const char *Txt_columns;
   unsigned NumCols;		// Number of columns in exam sheets
   char *Title;

   /***** Begin list of checkboxes *****/
   HTM_UL_Begin ("class=\"LIST_LEFT FORM_IN_%s\"",The_GetSuffix ());

      for (NumCols  = ExaSes_MIN_COLS;
	   NumCols <= ExaSes_MAX_COLS;
	   NumCols++)
	{
	 if (asprintf (&Title,"%u %s",NumCols,
				      NumCols == 1 ? Txt_column :
						     Txt_columns) < 0)
	    Err_NotEnoughMemoryExit ();

	 /* Begin label */
	 HTM_LABEL_Begin ("class=\"DAT_%s\"",The_GetSuffix ());

	    /* Checkbox with icon */
	    HTM_INPUT_RADIO ("NumCols",
			     NumCols == Session->NumCols ? HTM_CHECKED :
							   HTM_NO_ATTR,
			     "value=\"%u\"",NumCols);
	    Ico_PutIconOn (ExaSes_NumColsIcon[NumCols],Ico_BLACK,Title);
	    HTM_Unsigned (NumCols);

	 /* End label */
	 HTM_LABEL_End ();
	 HTM_NBSP (); HTM_NBSP (); HTM_NBSP ();

 	 free (Title);
	}

   /***** End list of checkboxes *****/
   HTM_UL_End ();
  }

/*****************************************************************************/
/*************** Request the creation or edition of a session ****************/
/*****************************************************************************/

void ExaSes_ReqCreatOrEditSes (void)
  {
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);

   /***** Get parameters *****/
   Exa_GetPars (&Exams,Exa_CHECK_EXA_COD);
   Grp_GetParMyAllGrps ();

   /***** Get exam data from database *****/
   Exa_GetExamDataByCod (&Exams.Exam);
   if (Exams.Exam.CrsCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
      Err_WrongExamExit ();

   /***** Get session data *****/
   switch (Gbl.Action.Act)
     {
      case ActReqNewExaSes:
	 /* Initialize to empty session */
	 ExaSes_ResetSession (&Session);
	 Session.ExaCod = Exams.Exam.ExaCod;
	 break;
      case ActReqChgExaSes:
	 /* Get code of session from form */
         Session.SesCod = ParCod_GetAndCheckPar (ParCod_Ses);

	 /* Get session data from database */
	 ExaSes_GetSessionDataByCod (&Session);
	 if (Exams.Exam.ExaCod != Session.ExaCod)
	    Err_WrongExamExit ();
	 break;
      default:
	 Err_WrongActionExit ();
	 break;
     }
   Exams.SesCod.Sel =
   Exams.SesCod.Par = Session.SesCod;

   /***** Show exam *****/
   Exa_ShowOnlyOneExam (&Exams,Frm_PUT_FORM);	// Put form for session
  }

/*****************************************************************************/
/****************** Create a new exam session (by a teacher) *****************/
/*****************************************************************************/

void ExaSes_ReceiveSession (void)
  {
   extern const char *Txt_Created_new_session_X;
   extern const char *Txt_The_session_has_been_modified;
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;
   OldNew_OldNew_t OldNewSession;
   Usr_Can_t ICanChangeModality;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);
   ExaSes_ResetSession (&Session);

   /***** Get main parameters *****/
   Exa_GetPars (&Exams,Exa_CHECK_EXA_COD);
   Grp_GetParMyAllGrps ();
   Session.SesCod = ParCod_GetPar (ParCod_Ses);
   OldNewSession = Session.SesCod > 0 ? OldNew_OLD :
					OldNew_NEW;

   /***** Get exam data from database *****/
   Exa_GetExamDataByCod (&Exams.Exam);
   if (Exams.Exam.CrsCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
      Err_WrongExamExit ();

   /***** Get session data from database *****/
   switch (OldNewSession)
     {
      case OldNew_OLD:
	 /* Get session data from database */
	 ExaSes_GetSessionDataByCod (&Session);
	 if (Session.ExaCod != Exams.Exam.ExaCod)
	    Err_WrongExamSessionExit ();
	 ICanChangeModality = Exa_DB_GetNumPrintsInSes (Session.SesCod) == 0 ? Usr_CAN :	// Only if there are no exam prints yet
									       Usr_CAN_NOT;
	 break;
      case OldNew_NEW:
      default:
	 /* Initialize to empty session */
	 ExaSes_ResetSession (&Session);
	 Session.ExaCod = Exams.Exam.ExaCod;
	 ICanChangeModality = Usr_CAN;
	 break;
     }
   Exams.SesCod.Sel =
   Exams.SesCod.Par = Session.SesCod;

   /***** Get parameters from form *****/
   /* Get session title */
   Par_GetParText ("Title",Session.Title,ExaSes_MAX_BYTES_TITLE);

   /* Get modality if it can be changed */
   if (ICanChangeModality == Usr_CAN)
      Session.Modality = (ExaSes_Modality_t)
			 Par_GetParUnsignedLong ("Modality",
						 0,
						 ExaSes_NUM_MODALITIES - 1,
						 (unsigned long) ExaSes_MODALITY_DEFAULT);
   if (Session.Modality == ExaSes_NONE)
      Session.Modality = ExaSes_MODALITY_DEFAULT;

   /* Get start/end date-times */
   Session.TimeUTC[Dat_STR_TIME] = Dat_GetTimeUTCFromForm (Dat_STR_TIME);
   Session.TimeUTC[Dat_END_TIME] = Dat_GetTimeUTCFromForm (Dat_END_TIME);

   /* Get groups associated to the session */
   Grp_GetParCodsSeveralGrps ();

   /* Get number of columns in exam sheets */
   Session.NumCols = ExaSes_GetParNumCols (Session.NumCols);

   /***** Create/update session *****/
   switch (OldNewSession)
     {
      case OldNew_OLD:
	 if (Session.TimeUTC[Dat_END_TIME] >= Dat_GetStartExecutionTimeUTC ())	// End of time is in the future
	    Session.Show_UsrResults = Lay_DONT_SHOW;	// Force results to be hidden
	 ExaSes_UpdateSession (&Session);
	 Ale_ShowAlert (Ale_SUCCESS,Txt_The_session_has_been_modified);
	 break;
      case OldNew_NEW:
      default:
	 ExaSes_CreateSession (&Session);
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_session_X,Session.Title);
	 break;
     }

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();

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
      ExaSes_CreateGrpsAssociatedToExamSession (Session->SesCod,
						&Gbl.Crs.Grps.LstGrpsSel);
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
      ExaSes_CreateGrpsAssociatedToExamSession (Session->SesCod,
						&Gbl.Crs.Grps.LstGrpsSel);	// Associate new groups
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

Usr_Can_t ExaSes_CheckIfICanAnswerThisSession (const struct Exa_Exam *Exam,
                                               const struct ExaSes_Session *Session)
  {
   /***** 1. Sessions in hidden exams are not accesible
          2. Hidden or closed sessions are not accesible *****/
   if (Exam->Hidden == HidVis_HIDDEN ||
       Session->Hidden == HidVis_HIDDEN ||
       Session->Open == CloOpe_CLOSED)
      return Usr_CAN_NOT;

   /***** Exam is visible, session is visible and open ==>
          ==> I can answer this session if I can list it based on groups *****/
   return ExaSes_CheckIfICanListThisSessionBasedOnGrps (Session->SesCod);
  }

Usr_Can_t ExaSes_CheckIfICanListThisSessionBasedOnGrps (long SesCod)
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
	 return Usr_CAN;
      default:
	 return Usr_CAN_NOT;
     }
  }
