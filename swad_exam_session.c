// swad_exam_session.c: exam sessions (each ocurrence of an exam)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include "swad_database.h"
#include "swad_date.h"
#include "swad_exam.h"
#include "swad_exam_print.h"
#include "swad_exam_result.h"
#include "swad_exam_session.h"
#include "swad_exam_set.h"
#include "swad_exam_type.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_role.h"
#include "swad_setting.h"
#include "swad_test.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaSes_PutIconsInListOfSessions (void *Exams);
static void ExaSes_PutIconToCreateNewSession (struct Exa_Exams *Exams);

static void ExaSes_ListOneOrMoreSessions (struct Exa_Exams *Exams,
                                          const struct Exa_Exam *Exam,
                                          long EvtCodToBeEdited,
				          unsigned NumSessions,
                                          MYSQL_RES *mysql_res);
static void ExaSes_ListOneOrMoreSessionsHeading (bool ICanEditSessions);
static bool ExaSes_CheckIfICanEditSessions (void);
static bool ExaSes_CheckIfICanEditThisSession (const struct ExaSes_Session *Session);
static bool ExaSes_CheckIfVisibilityOfResultsCanBeChanged (const struct ExaSes_Session *Session);
static void ExaSes_ListOneOrMoreSessionsIcons (struct Exa_Exams *Exams,
                                               const struct ExaSes_Session *Session,
					       const char *Anchor);
static void ExaSes_ListOneOrMoreSessionsAuthor (const struct ExaSes_Session *Session);
static void ExaSes_ListOneOrMoreSessionsTimes (const struct ExaSes_Session *Session,
                                               unsigned UniqueId);
static void ExaSes_ListOneOrMoreSessionsTitleGrps (struct Exa_Exams *Exams,
                                                   const struct Exa_Exam *Exam,
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

static void ExaSes_RemoveSessionFromAllTables (long SesCod);
static void ExaSes_RemoveUsrSesResultsInCrs (long UsrCod,long CrsCod,const char *TableName);

static void ExaSes_PutFormSession (const struct ExaSes_Session *Session);
static void ExaSes_ShowLstGrpsToCreateSession (long SesCod);

static void ExaSes_CreateSession (struct ExaSes_Session *Session);
static void ExaSes_UpdateSession (struct ExaSes_Session *Session);

static void ExaSes_CreateGrps (long SesCod);
static void ExaSes_RemoveGroups (long SesCod);

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
   Session->Hidden		     = false;
   Session->Open	             = false;
   Session->ShowUsrResults           = false;
  };

/*****************************************************************************/
/************************ List the sessions of an exam ***********************/
/*****************************************************************************/

void ExaSes_ListSessions (struct Exa_Exams *Exams,
                          struct Exa_Exam *Exam,
		          struct ExaSes_Session *Session,
                          bool PutFormSession)
  {
   extern const char *Hlp_ASSESSMENT_Exams_sessions;
   extern const char *Txt_Sessions;
   char *HiddenSubQuery;
   char *GroupsSubQuery;
   MYSQL_RES *mysql_res;
   unsigned NumSessions;
   long SesCodToBeEdited;
   bool PutFormNewSession;

   /***** Subquery: get hidden sessions depending on user's role *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
         if (asprintf (&HiddenSubQuery," AND Hidden='N'") < 0)
	    Lay_NotEnoughMemoryExit ();
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 if (asprintf (&HiddenSubQuery,"%s","") < 0)
	    Lay_NotEnoughMemoryExit ();
	 break;
      default:
	 Rol_WrongRoleExit ();
	 break;
     }

   /***** Subquery: get sessions depending on groups *****/
   if (Gbl.Crs.Grps.WhichGrps == Grp_MY_GROUPS)
     {
      if (asprintf (&GroupsSubQuery," AND"
				    "(SesCod NOT IN"
				    " (SELECT SesCod FROM exa_groups)"
				    " OR"
				    " SesCod IN"
				    " (SELECT exa_groups.SesCod"
				       " FROM exa_groups,"
				             "grp_users"
				      " WHERE grp_users.UsrCod=%ld"
				        " AND exa_groups.GrpCod=grp_users.GrpCod))",
		     Gbl.Usrs.Me.UsrDat.UsrCod) < 0)
	  Lay_NotEnoughMemoryExit ();
      }
    else	// Gbl.Crs.Grps.WhichGrps == Grp_ALL_GROUPS
       if (asprintf (&GroupsSubQuery,"%s","") < 0)
	  Lay_NotEnoughMemoryExit ();

   /***** Get data of sessions from database *****/
   NumSessions = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get sessions",
		   "SELECT SesCod,"					// row[0]
			  "ExaCod,"					// row[1]
			  "Hidden,"					// row[2]
			  "UsrCod,"					// row[3]
			  "UNIX_TIMESTAMP(StartTime),"		// row[4]
			  "UNIX_TIMESTAMP(EndTime),"		// row[5]
			  "NOW() BETWEEN StartTime AND EndTime,"	// row[6]
			  "Title,"					// row[7]
			  "ShowUsrResults"				// row[8]
		    " FROM exa_sessions"
		   " WHERE ExaCod=%ld%s%s"
		   " ORDER BY SesCod",
		   Exam->ExaCod,HiddenSubQuery,GroupsSubQuery);

   /***** Free allocated memory for subqueries *****/
   free (GroupsSubQuery);
   free (HiddenSubQuery);

   /***** Begin box *****/
   Exams->ExaCod = Exam->ExaCod;
   Box_BoxBegin ("100%",Txt_Sessions,
                 ExaSes_PutIconsInListOfSessions,Exams,
                 Hlp_ASSESSMENT_Exams_sessions,Box_NOT_CLOSABLE);

   /***** Select whether show only my groups or all groups *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 if (Gbl.Crs.Grps.NumGrps)
	   {
	    Set_BeginSettingsHead ();
	    Grp_ShowFormToSelWhichGrps (ActSeeExa,
					Exa_PutParams,Exams);
	    Set_EndSettingsHead ();
	   }
	 break;
      default:
	 break;
     }

   /***** Show the table with the sessions *****/
   if (NumSessions)
     {
      SesCodToBeEdited = PutFormSession && Session->SesCod > 0 ? Session->SesCod :
	                                                         -1L;
      ExaSes_ListOneOrMoreSessions (Exams,Exam,SesCodToBeEdited,NumSessions,mysql_res);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Put button to create a new exam session in this exam *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 PutFormNewSession = PutFormSession && Session->SesCod <= 0;
	 if (PutFormNewSession)
	   {
	    /* Reset session */
	    ExaSes_ResetSession (Session);
	    Session->ExaCod = Exam->ExaCod;
	    Session->TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;			// Now
	    Session->TimeUTC[Dat_END_TIME  ] = Gbl.StartExecutionTimeUTC + (1 * 60 * 60);	// Now + 1 hour
            Str_Copy (Session->Title,Exam->Title,sizeof (Session->Title) - 1);

	    /* Put form to create new session */
	    ExaSes_PutFormSession (Session);	// Form to create session
	   }
	 else
	    ExaSes_PutButtonNewSession (Exams,Exam->ExaCod);	// Button to create a new exam session
	 break;
      default:
	 break;
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************* Get exam session data using its code ********************/
/*****************************************************************************/

void ExaSes_GetDataOfSessionByCod (struct ExaSes_Session *Session)
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
   if (DB_QuerySELECT (&mysql_res,"can not get sessions",
		       "SELECT SesCod,"					// row[0]
			      "ExaCod,"					// row[1]
			      "Hidden,"					// row[2]
			      "UsrCod,"					// row[3]
			      "UNIX_TIMESTAMP(StartTime),"		// row[4]
			      "UNIX_TIMESTAMP(EndTime),"		// row[5]
			      "NOW() BETWEEN StartTime AND EndTime,"	// row[6]
			      "Title,"					// row[7]
			      "ShowUsrResults"				// row[8]
		        " FROM exa_sessions"
		       " WHERE SesCod=%ld"
			 " AND ExaCod IN"		// Extra check
			     " (SELECT ExaCod"
			        " FROM exa_exams"
			       " WHERE CrsCod='%ld')",
		       Session->SesCod,
		       Gbl.Hierarchy.Crs.CrsCod)) // Session found...
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
   extern const char *Txt_New_session;

   /***** Put form to create a new exam session *****/
   Ico_PutContextualIconToAdd (ActReqNewExaSes,ExaSes_NEW_SESSION_SECTION_ID,
                               Exa_PutParams,Exams,
			       Txt_New_session);
  }

/*****************************************************************************/
/********************** List exam sessions for edition ***********************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessions (struct Exa_Exams *Exams,
                                          const struct Exa_Exam *Exam,
                                          long EvtCodToBeEdited,
				          unsigned NumSessions,
                                          MYSQL_RES *mysql_res)
  {
   unsigned NumSession;
   unsigned UniqueId;
   char *Anchor;
   struct ExaSes_Session Session;
   bool ICanEditSessions = ExaSes_CheckIfICanEditSessions ();

   /***** Trivial check *****/
   if (!NumSessions)
      return;

   /***** Reset session *****/
   ExaSes_ResetSession (&Session);

   /***** Write the heading *****/
   HTM_TABLE_BeginWidePadding (2);
   ExaSes_ListOneOrMoreSessionsHeading (ICanEditSessions);

   /***** Write rows *****/
   for (NumSession = 0, UniqueId = 1;
	NumSession < NumSessions;
	NumSession++, UniqueId++)
     {
      Gbl.RowEvenOdd = NumSession % 2;

      /***** Get exam session data from row *****/
      ExaSes_GetSessionDataFromRow (mysql_res,&Session);

      if (ExaSes_CheckIfICanListThisSessionBasedOnGrps (Session.SesCod))
	{
	 /***** Build anchor string *****/
	 if (asprintf (&Anchor,"evt_%ld_%ld",Exam->ExaCod,Session.SesCod) < 0)
	    Lay_NotEnoughMemoryExit ();

	 /***** Begin row for this exam session ****/
	 HTM_TR_Begin (NULL);

	 /* Icons */
	 if (ICanEditSessions)
	    if (ExaSes_CheckIfICanEditThisSession (&Session))
	       ExaSes_ListOneOrMoreSessionsIcons (Exams,&Session,Anchor);

	 /* Session participant */
	 ExaSes_ListOneOrMoreSessionsAuthor (&Session);

	 /* Start/end date/time */
	 ExaSes_ListOneOrMoreSessionsTimes (&Session,UniqueId);

	 /* Title and groups */
	 ExaSes_ListOneOrMoreSessionsTitleGrps (Exams,Exam,&Session,Anchor);

	 /* Session result visible? */
	 ExaSes_ListOneOrMoreSessionsResult (Exams,&Session);

	 /***** End row for this session ****/
	 HTM_TR_End ();

	 /***** For to edit this session ****/
	 if (Session.SesCod == EvtCodToBeEdited)
	   {
	    HTM_TR_Begin (NULL);
            HTM_TD_Begin ("colspan=\"6\" class=\"CT COLOR%u\"",Gbl.RowEvenOdd);
	    ExaSes_PutFormSession (&Session);	// Form to edit existing session
            HTM_TD_End ();
	    HTM_TR_End ();
	   }

	 /***** Free anchor string *****/
	 free (Anchor);
	}
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************* Put a column for exam session start and end times *************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsHeading (bool ICanEditSessions)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Session;
   extern const char *Txt_Results;

   /***** Start row *****/
   HTM_TR_Begin (NULL);

   /***** Column for icons *****/
   if (ICanEditSessions)
      HTM_TH_Empty (1);

   /***** The rest of columns *****/
   HTM_TH (1,1,"LT",Txt_ROLES_SINGUL_Abc[Rol_TCH][Usr_SEX_UNKNOWN]);
   HTM_TH (1,1,"LT",Txt_START_END_TIME[Exa_ORDER_BY_START_DATE]);
   HTM_TH (1,1,"LT",Txt_START_END_TIME[Exa_ORDER_BY_END_DATE  ]);
   HTM_TH (1,1,"LT",Txt_Session);
   HTM_TH (1,1,"CT",Txt_Results);

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/********************** Check if I can edit sessions *************************/
/*****************************************************************************/

static bool ExaSes_CheckIfICanEditSessions (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
  }

/*****************************************************************************/
/************ Check if I can edit (remove/resume) an exam session ************/
/*****************************************************************************/

static bool ExaSes_CheckIfICanEditThisSession (const struct ExaSes_Session *Session)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
	 return (Session->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);	// Only if I am the creator
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
   if (Session->ShowUsrResults ||					// Results are currently visible
       Session->TimeUTC[Dat_END_TIME] < Gbl.StartExecutionTimeUTC)	// End of time is in the past
      if (ExaSes_CheckIfICanEditThisSession (Session))
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
   /***** Begin cell *****/
   HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);

   Exams->ExaCod = Session->ExaCod;
   Exams->SesCod = Session->SesCod;

   /***** Icon to remove the exam session *****/
   Ico_PutContextualIconToRemove (ActReqRemExaSes,NULL,
                                  ExaSes_PutParamsEdit,Exams);

   /***** Icon to hide/unhide the exam session *****/
   if (Session->Hidden)
      Ico_PutContextualIconToUnhide (ActUnhExaSes,Anchor,
				     ExaSes_PutParamsEdit,Exams);
   else
      Ico_PutContextualIconToHide (ActHidExaSes,Anchor,
				   ExaSes_PutParamsEdit,Exams);

   /***** Icon to edit the exam session *****/
   Ico_PutContextualIconToEdit (ActEdiOneExaSes,Anchor,
                                ExaSes_PutParamsEdit,Exams);

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/********** Put a column for teacher who created the exam session ************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsAuthor (const struct ExaSes_Session *Session)
  {
   /***** Session author (teacher) *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   Usr_WriteAuthor1Line (Session->UsrCod,Session->Hidden);
   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Put a column for exam session start and end times *************/
/*****************************************************************************/

static void ExaSes_ListOneOrMoreSessionsTimes (const struct ExaSes_Session *Session,
                                               unsigned UniqueId)
  {
   Dat_StartEndTime_t StartEndTime;
   const char *Color;
   char *Id;

   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      Color = Session->Open ? (Session->Hidden ? "DATE_GREEN_LIGHT":
					     "DATE_GREEN") :
			    (Session->Hidden ? "DATE_RED_LIGHT":
					     "DATE_RED");

      if (asprintf (&Id,"exa_time_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"%s LT COLOR%u\"",
		    Id,Color,Gbl.RowEvenOdd);
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
                                                   const struct Exa_Exam *Exam,
                                                   const struct ExaSes_Session *Session,
                                                   const char *Anchor)
  {
   extern const char *Txt_Play;
   extern const char *Txt_Resume;

   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);

   /***** Session title *****/
   HTM_ARTICLE_Begin (Anchor);
   if (ExaSes_CheckIfICanAnswerThisSession (Exam,Session))
     {
      Frm_BeginForm (ActSeeExaPrn);
      Exa_PutParams (Exams);
      ExaSes_PutParamSesCod (Session->SesCod);
      HTM_BUTTON_SUBMIT_Begin (Gbl.Usrs.Me.Role.Logged == Rol_STD ? Txt_Play :
								    Txt_Resume,
			       Session->Hidden ? "BT_LINK LT ASG_TITLE_LIGHT":
					         "BT_LINK LT ASG_TITLE",
			       NULL);
      HTM_Txt (Session->Title);
      HTM_BUTTON_End ();
      Frm_EndForm ();
     }
   else
     {
      HTM_SPAN_Begin ("class=\"%s\"",Session->Hidden ? "LT ASG_TITLE_LIGHT":
					               "LT ASG_TITLE");
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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrps;
   unsigned NumGrp;

   /***** Get groups associated to an exam session from database *****/
   NumGrps = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get groups of an exam session",
		   "SELECT grp_types.GrpTypName,"	// row[0]
			  "grp_groups.GrpName"	// row[1]
		    " FROM exa_groups,"
			  "grp_groups,"
			  "grp_types"
		   " WHERE exa_groups.SesCod=%ld"
		     " AND exa_groups.GrpCod=grp_groups.GrpCod"
		     " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
		   " ORDER BY grp_types.GrpTypName,"
			     "grp_groups.GrpName",
		   Session->SesCod);

   /***** Write heading *****/
   HTM_DIV_Begin ("class=\"%s\"",Session->Hidden ? "ASG_GRP_LIGHT":
					           "ASG_GRP");
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

         /* Write group type name and group name */
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
      HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,Gbl.Hierarchy.Crs.ShrtName);

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
   HTM_TD_Begin ("class=\"DAT CT COLOR%u\"",Gbl.RowEvenOdd);

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ExaSes_ListOneOrMoreSessionsResultStd (Exams,Session);
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 ExaSes_ListOneOrMoreSessionsResultTch (Exams,Session);
	 break;
      default:
	 Rol_WrongRoleExit ();
	 break;
     }

   HTM_TD_End ();
  }

static void ExaSes_ListOneOrMoreSessionsResultStd (struct Exa_Exams *Exams,
                                                   const struct ExaSes_Session *Session)
  {
   extern const char *Txt_Results;

   /***** Is exam session result visible or hidden? *****/
   if (Session->ShowUsrResults)
     {
      /* Result is visible by me */
      Exams->ExaCod = Session->ExaCod;
      Exams->SesCod = Session->SesCod;
      Lay_PutContextualLinkOnlyIcon (ActSeeMyExaResSes,ExaRes_RESULTS_BOX_ID,
				     ExaSes_PutParamsEdit,Exams,
				     "trophy.svg",
				     Txt_Results);
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
   extern const char *Txt_Results;

   Exams->ExaCod = Session->ExaCod;
   Exams->SesCod = Session->SesCod;

   /***** Show exam session results *****/
   if (ExaSes_CheckIfICanEditThisSession (Session))
      Lay_PutContextualLinkOnlyIcon (ActSeeUsrExaResSes,ExaRes_RESULTS_BOX_ID,
				     ExaSes_PutParamsEdit,Exams,
				     "trophy.svg",
				     Txt_Results);

   /***** Check if visibility of session results can be changed *****/
   if (ExaSes_CheckIfVisibilityOfResultsCanBeChanged (Session))
     {
      /***** Put form to change visibility of session results *****/
      Lay_PutContextualLinkOnlyIcon (ActChgVisExaRes,NULL,
				     ExaSes_PutParamsEdit,Exams,
				     Session->ShowUsrResults ? "eye-green.svg" :
							       "eye-slash-red.svg",
				     Session->ShowUsrResults ? Txt_Visible_results :
							       Txt_Hidden_results);
     }
   else	// Don't put form
      /***** Put icon showing the current visibility of session results *****/
      Ico_PutIconOff (Session->ShowUsrResults ? "eye-green.svg" :
					        "eye-slash-red.svg",
		      Session->ShowUsrResults ? Txt_Visible_results :
					        Txt_Hidden_results);
  }

/*****************************************************************************/
/***************** Toggle visibility of exam session results *****************/
/*****************************************************************************/

void ExaSes_ToggleVisResultsSesUsr (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSes_Session Session;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSes_ResetSession (&Session);

   /***** Get and check parameters *****/
   ExaSes_GetAndCheckParameters (&Exams,&Exam,&Session);

   /***** Check if visibility of session results can be changed *****/
   if (!ExaSes_CheckIfVisibilityOfResultsCanBeChanged (&Session))
      Lay_NoPermissionExit ();

   /***** Toggle visibility of exam session results *****/
   Session.ShowUsrResults = !Session.ShowUsrResults;
   DB_QueryUPDATE ("can not toggle visibility of session results",
		   "UPDATE exa_sessions,"
		          "exa_exams"
		     " SET exa_sessions.ShowUsrResults='%c'"
		   " WHERE exa_sessions.SesCod=%ld"
		     " AND exa_sessions.ExaCod=%ld"	// Extra check
		     " AND exa_sessions.ExaCod=exa_exams.ExaCod"
		     " AND exa_exams.CrsCod=%ld",		// Extra check
		   Session.ShowUsrResults ? 'Y' :
			                    'N',
		   Session.SesCod,
		   Session.ExaCod,
		   Gbl.Hierarchy.Crs.CrsCod);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Session,
	                false);	// Do not put form for session
  }

/*****************************************************************************/
/******************** Get exam data from a database row **********************/
/*****************************************************************************/

static void ExaSes_GetSessionDataFromRow (MYSQL_RES *mysql_res,
				          struct ExaSes_Session *Session)
  {
   MYSQL_ROW row;
   Dat_StartEndTime_t StartEndTime;

   /***** Get exam session data *****/
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
      Lay_WrongExamSessionExit ();

   /* Code of the exam (row[1]) */
   if ((Session->ExaCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
      Lay_WrongExamExit ();

   /* Get whether the session is hidden (row[2]) */
   Session->Hidden = (row[2][0] == 'Y');

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

void ExaSes_RequestRemoveSession (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_session_X;
   extern const char *Txt_Remove_session;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSes_Session Session;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSes_ResetSession (&Session);

   /***** Get and check parameters *****/
   ExaSes_GetAndCheckParameters (&Exams,&Exam,&Session);

   /***** Show question and button to remove question *****/
   Exams.ExaCod = Session.ExaCod;
   Exams.SesCod = Session.SesCod;
   Ale_ShowAlertAndButton (ActRemExaSes,NULL,NULL,
                           ExaSes_PutParamsEdit,&Exams,
			   Btn_REMOVE_BUTTON,Txt_Remove_session,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_session_X,
	                   Session.Title);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Session,
	                false);	// Do not put form for session
  }

/*****************************************************************************/
/****************** Remove an exam session (exam instance) *******************/
/*****************************************************************************/

void ExaSes_RemoveSession (void)
  {
   extern const char *Txt_Session_X_removed;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSes_Session Session;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSes_ResetSession (&Session);

   /***** Get and check parameters *****/
   ExaSes_GetAndCheckParameters (&Exams,&Exam,&Session);

   /***** Check if I can remove this exam session *****/
   if (!ExaSes_CheckIfICanEditThisSession (&Session))
      Lay_NoPermissionExit ();

   /***** Remove the exam session from all database tables *****/
   ExaSes_RemoveSessionFromAllTables (Session.SesCod);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Session_X_removed,
		  Session.Title);

   /***** Get exam data again to update it after changes in session *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Session,
	                false);	// Do not put form for session
  }

/*****************************************************************************/
/******************* Remove exam session from all tables *********************/
/*****************************************************************************/

static void ExaSes_RemoveSessionFromAllTables (long SesCod)
  {
   /* To delete orphan exam prints:
   // DELETE FROM exa_print_questions WHERE PrnCod IN (SELECT PrnCod FROM exa_prints WHERE SesCod NOT IN (SELECT SesCod FROM exa_sessions));
   // DELETE FROM exa_prints WHERE SesCod NOT IN (SELECT SesCod FROM exa_sessions);
   */
   /***** Remove exam prints in this session *****/
   /*
    * TODO: DO NOT REMOVE EXAMS PRINTS. Instead move them to tables of deleted prints
   DB_QueryDELETE ("can not remove exam print questions in exam session",
		   "DELETE FROM exa_print_questions"
		   " USING exa_print_questions,
		          "exa_prints"
                   " WHERE exa_prints.SesCod=%ld"
                   " AND exa_prints.PrnCod=exa_print_questions.PrnCod",
		   SesCod);
   DB_QueryDELETE ("can not remove exam prints in exam session",
		   "DELETE FROM exa_prints"
                   " WHERE exa_prints.SesCod=%ld",
		   SesCod);
   */

   /***** Remove groups associated to this exam session *****/
   DB_QueryDELETE ("can not remove groups associated to exam session",
		   "DELETE FROM exa_groups"
		   " WHERE SesCod=%ld",
		   SesCod);

   /***** Remove exam session from main table *****/
   DB_QueryDELETE ("can not remove exam session",
		   "DELETE FROM exa_sessions"
		   " WHERE SesCod=%ld",
		   SesCod);
  }

/*****************************************************************************/
/**************** Remove exam session in exam from all tables ****************/
/*****************************************************************************/

void ExaSes_RemoveSessionsInExamFromAllTables (long ExaCod)
  {
   /***** Remove exam prints in this session *****/
   /*
    * TODO: DO NOT REMOVE EXAMS PRINTS. Instead move them to tables of deleted prints
   DB_QueryDELETE ("can not remove exam print questions in exam",
		   "DELETE FROM exa_print_questions"
		   " USING exa_print_questions,
		          "exa_prints,
		          "exa_sessions"
                   " WHERE exa_sessions.ExaCod=%ld"
                     " AND exa_sessions.SesCod=exa_prints.SesCod"
                     " AND exa_prints.PrnCod=exa_print_questions.PrnCod",
		   ExaCod);
   DB_QueryDELETE ("can not remove exam prints in exam",
		   "DELETE FROM exa_prints"
		   " USING exa_prints,
		          "exa_sessions"
                   " WHERE exa_sessions.ExaCod=%ld"
                     " AND exa_sessions.SesCod=exa_prints.SesCod",
		   ExaCod);
   */

   /***** Remove groups associated to exam sessions of this exam *****/
   DB_QueryDELETE ("can not remove groups associated to sessions of an exam",
		   "DELETE FROM exa_groups"
		   " USING exa_sessions,"
		          "exa_groups"
		   " WHERE exa_sessions.ExaCod=%ld"
		     " AND exa_sessions.SesCod=exa_groups.SesCod",
		   ExaCod);

   /***** Remove sessions from main table *****/
   DB_QueryDELETE ("can not remove sessions of an exam",
		   "DELETE FROM exa_sessions"
		   " WHERE ExaCod=%ld",
		   ExaCod);
  }

/*****************************************************************************/
/*************** Remove exam session in course from all tables ***************/
/*****************************************************************************/

void ExaSes_RemoveSessionInCourseFromAllTables (long CrsCod)
  {
   /***** Remove exam prints in this course *****/
   /*
    * TODO: DO NOT REMOVE EXAMS PRINTS. Instead move them to tables of deleted prints
   DB_QueryDELETE ("can not remove exam print questions in course",
		   "DELETE FROM exa_print_questions"
		   " USING exa_print_questions,
		          "exa_prints,
		          "exa_sessions,
		          "exa_exams"
		   " WHERE exa_exams.CrsCod=%ld"
                     " AND exa_exams.ExaCod=exa_sessions"
                     " AND exa_sessions.SesCod=exa_prints.SesCod"
                     " AND exa_prints.PrnCod=exa_print_questions.PrnCod",
		   CrsCod);
   DB_QueryDELETE ("can not remove exam print questions in course",
		   "DELETE FROM exa_prints"
		   " USING exa_prints,
		          "exa_sessions,
		          "exa_exams"
		   " WHERE exa_exams.CrsCod=%ld"
                     " AND exa_exams.ExaCod=exa_sessions"
                     " AND exa_sessions.SesCod=exa_prints.SesCod",
		   CrsCod);
   */

   /***** Remove sessions from table of sessions groups *****/
   DB_QueryDELETE ("can not remove sessions of a course",
		   "DELETE FROM exa_groups"
		   " USING exa_exams,"
		          "exa_sessions,"
		          "exa_groups"
		   " WHERE exa_exams.CrsCod=%ld"
		     " AND exa_exams.ExaCod=exa_sessions.ExaCod"
		     " AND exa_sessions.SesCod=exa_groups.SesCod",
		   CrsCod);

   /***** Remove sessions from exam sessions table *****/
   DB_QueryDELETE ("can not remove sessions of a course",
		   "DELETE FROM exa_sessions"
		   " USING exa_exams,"
		          "exa_sessions"
		   " WHERE exa_exams.CrsCod=%ld"
		     " AND exa_exams.ExaCod=exa_sessions.ExaCod",
		   CrsCod);
  }

/*****************************************************************************/
/************* Remove user from secondary exam session tables ****************/
/*****************************************************************************/

void ExaSes_RemoveUsrFromSessionTablesInCrs (long UsrCod,long CrsCod)
  {
   /***** Remove student from secondary tables *****/
   ExaSes_RemoveUsrSesResultsInCrs (UsrCod,CrsCod,"exa_prints");
  }

static void ExaSes_RemoveUsrSesResultsInCrs (long UsrCod,long CrsCod,const char *TableName)
  {
   /***** Remove sessions in course from secondary table *****/
   DB_QueryDELETE ("can not remove sessions of a user from table",
		   "DELETE FROM %s"
		   " USING exa_exams,"
		          "exa_sessions,"
		          "%s"
		   " WHERE exa_exams.CrsCod=%ld"
		     " AND exa_exams.ExaCod=exa_sessions.ExaCod"
		     " AND exa_sessions.SesCod=%s.SesCod"
		     " AND %s.UsrCod=%ld",
		   TableName,
		   TableName,
		   CrsCod,
		   TableName,
		   TableName,
		   UsrCod);
  }

/*****************************************************************************/
/******************************* Hide an session *****************************/
/*****************************************************************************/

void ExaSes_HideSession (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSes_Session Session;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSes_ResetSession (&Session);

   /***** Get and check parameters *****/
   ExaSes_GetAndCheckParameters (&Exams,&Exam,&Session);

   /***** Check if I can remove this exam session *****/
   if (!ExaSes_CheckIfICanEditThisSession (&Session))
      Lay_NoPermissionExit ();

   /***** Hide session *****/
   DB_QueryUPDATE ("can not hide exam sessions",
		   "UPDATE exa_sessions,"
		          "exa_exams"
		     " SET exa_sessions.Hidden='Y'"
		   " WHERE exa_sessions.SesCod=%ld"
		     " AND exa_sessions.ExaCod=%ld"	// Extra check
		     " AND exa_sessions.ExaCod=exa_exams.ExaCod"
		     " AND exa_exams.CrsCod=%ld",	// Extra check
		   Session.SesCod,
		   Session.ExaCod,
		   Gbl.Hierarchy.Crs.CrsCod);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Session,
	                false);	// Do not put form for session
  }

/*****************************************************************************/
/***************************** Unhide an session *****************************/
/*****************************************************************************/

void ExaSes_UnhideSession (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSes_Session Session;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSes_ResetSession (&Session);

   /***** Get and check parameters *****/
   ExaSes_GetAndCheckParameters (&Exams,&Exam,&Session);

   /***** Check if I can remove this exam session *****/
   if (!ExaSes_CheckIfICanEditThisSession (&Session))
      Lay_NoPermissionExit ();

   /***** Unhide session *****/
   DB_QueryUPDATE ("can not unhide exam session",
		   "UPDATE exa_sessions,"
		          "exa_exams"
		     " SET exa_sessions.Hidden='N'"
		   " WHERE exa_sessions.SesCod=%ld"
		     " AND exa_sessions.ExaCod=%ld"	// Extra check
		     " AND exa_sessions.ExaCod=exa_exams.ExaCod"
		     " AND exa_exams.CrsCod=%ld",	// Extra check
		   Session.SesCod,
		   Session.ExaCod,
		   Gbl.Hierarchy.Crs.CrsCod);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Session,
	                false);	// Do not put form for session
  }

/*****************************************************************************/
/******************* Params used to edit an exam session *********************/
/*****************************************************************************/

void ExaSes_PutParamsEdit (void *Exams)
  {
   if (Exams)
     {
      Exa_PutParams (Exams);
      ExaSes_PutParamSesCod (((struct Exa_Exams *) Exams)->SesCod);
     }
  }

/*****************************************************************************/
/**************** Write parameter with code of exam session ******************/
/*****************************************************************************/

void ExaSes_PutParamSesCod (long SesCod)
  {
   Par_PutHiddenParamLong (NULL,"SesCod",SesCod);
  }

/*****************************************************************************/
/************************** Get and check parameters *************************/
/*****************************************************************************/

void ExaSes_GetAndCheckParameters (struct Exa_Exams *Exams,
                                   struct Exa_Exam *Exam,
                                   struct ExaSes_Session *Session)
  {
   /***** Get parameters *****/
   Exa_GetParams (Exams);
   if (Exams->ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam->ExaCod = Exams->ExaCod;
   Grp_GetParamWhichGroups ();
   if ((Session->SesCod = ExaSes_GetParamSesCod ()) <= 0)
      Lay_WrongExamSessionExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (Exam);
   if (Exam->CrsCod != Gbl.Hierarchy.Crs.CrsCod)
      Lay_WrongExamExit ();
   Exams->ExaCod = Exam->ExaCod;

   /***** Get set data from database *****/
   ExaSes_GetDataOfSessionByCod (Session);
   if (Session->ExaCod != Exam->ExaCod)
      Lay_WrongSetExit ();
   Exams->SesCod = Session->SesCod;
  }

/*****************************************************************************/
/**************** Get parameter with code of exam session ********************/
/*****************************************************************************/

long ExaSes_GetParamSesCod (void)
  {
   /***** Get code of exam session *****/
   return Par_GetParToLong ("SesCod");
  }

/*****************************************************************************/
/* Put a big button to play exam session (start a new session) as a teacher **/
/*****************************************************************************/

static void ExaSes_PutFormSession (const struct ExaSes_Session *Session)
  {
   extern const char *Hlp_ASSESSMENT_Exams_sessions;
   extern const char *Txt_New_session;
   extern const char *Txt_Title;
   extern const char *Txt_Create_session;
   extern const char *Txt_Save_changes;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_START_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME  ] = Dat_HMS_DO_NOT_SET
     };
   bool ItsANewSession = Session->SesCod <= 0;

   /***** Begin section for a new exam session *****/
   HTM_SECTION_Begin (ExaSes_NEW_SESSION_SECTION_ID);

   /***** Begin form *****/
   Frm_BeginForm (ItsANewSession ? ActNewExaSes :	// New session
	                           ActChgExaSes);	// Existing session
   Exa_PutParamExamCod (Session->ExaCod);
   if (!ItsANewSession)	// Existing session
      ExaSes_PutParamSesCod (Session->SesCod);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,ItsANewSession ? Txt_New_session :
					    Session->Title,
                      NULL,NULL,
		      Hlp_ASSESSMENT_Exams_sessions,Box_NOT_CLOSABLE,2);

   /***** Session title *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","Title",Txt_Title);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_INPUT_TEXT ("Title",ExaSes_MAX_CHARS_TITLE,Session->Title,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "id=\"Title\" size=\"45\" required=\"required\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Start and end dates *****/
   Dat_PutFormStartEndClientLocalDateTimes (Session->TimeUTC,
                                            Dat_FORM_SECONDS_OFF,
					    SetHMS);

   /***** Groups *****/
   ExaSes_ShowLstGrpsToCreateSession (Session->SesCod);

   /***** End table, send button and end box *****/
   if (ItsANewSession)
      Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_session);
   else
      Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** End section for a new exam session *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/************* Show list of groups to create a new exam session **************/
/*****************************************************************************/

static void ExaSes_ShowLstGrpsToCreateSession (long SesCod)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Groups;
   extern const char *Txt_The_whole_course;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   if (Gbl.Crs.Grps.GrpTypes.Num)
     {
      /***** Begin box and table *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_TxtColon (Txt_Groups);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LT\"");
      Box_BoxTableBegin ("95%",NULL,
                         NULL,NULL,
                         NULL,Box_NOT_CLOSABLE,0);

      /***** First row: checkbox to select the whole course *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("colspan=\"7\" class=\"DAT LM\"");
      HTM_LABEL_Begin (NULL);
      HTM_INPUT_CHECKBOX ("WholeCrs",HTM_DONT_SUBMIT_ON_CHANGE,
		          "id=\"WholeCrs\" value=\"Y\"%s"
		          " onclick=\"uncheckChildren(this,'GrpCods')\"",
			  Grp_CheckIfAssociatedToGrps ("exa_groups","SesCod",SesCod) ? "" :
				                                                       " checked=\"checked\"");
      HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,Gbl.Hierarchy.Crs.ShrtName);
      HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TR_End ();

      /***** List the groups for each group type *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
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
/******************** Put button to create a new session *********************/
/*****************************************************************************/

void ExaSes_PutButtonNewSession (struct Exa_Exams *Exams,long ExaCod)
  {
   extern const char *Txt_New_session;

   Exams->ExaCod = ExaCod;
   Frm_StartFormAnchor (ActReqNewExaSes,ExaSes_NEW_SESSION_SECTION_ID);
   Exa_PutParams (Exams);
   Btn_PutConfirmButton (Txt_New_session);
   Frm_EndForm ();
  }

/*****************************************************************************/
/****************** Request the creation of a new session ********************/
/*****************************************************************************/

void ExaSes_RequestCreatOrEditSession (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSes_Session Session;
   bool ItsANewSession;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSes_ResetSession (&Session);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   Grp_GetParamWhichGroups ();
   Session.SesCod = ExaSes_GetParamSesCod ();
   ItsANewSession = (Session.SesCod <= 0);

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   if (Exam.CrsCod != Gbl.Hierarchy.Crs.CrsCod)
      Lay_WrongExamExit ();
   Exams.ExaCod = Exam.ExaCod;

   /***** Get session data *****/
   if (ItsANewSession)
      /* Initialize to empty session */
      ExaSes_ResetSession (&Session);
   else
     {
      /* Get session data from database */
      ExaSes_GetDataOfSessionByCod (&Session);
      if (Exam.ExaCod != Session.ExaCod)
	 Lay_WrongExamExit ();
      Exams.SesCod = Session.SesCod;
     }

   /***** Show exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Session,
                        true);	// Put form for session
  }

/*****************************************************************************/
/****************** Create a new exam session (by a teacher) *****************/
/*****************************************************************************/

void ExaSes_ReceiveFormSession (void)
  {
   extern const char *Txt_Created_new_event_X;
   extern const char *Txt_The_event_has_been_modified;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSes_Session Session;
   bool ItsANewSession;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSes_ResetSession (&Session);

   /***** Get main parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   Grp_GetParamWhichGroups ();
   Session.SesCod = ExaSes_GetParamSesCod ();
   ItsANewSession = (Session.SesCod <= 0);

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   if (Exam.CrsCod != Gbl.Hierarchy.Crs.CrsCod)
      Lay_WrongExamExit ();
   Exams.ExaCod = Exam.ExaCod;

   /***** Get session data from database *****/
   if (ItsANewSession)
     {
      /* Initialize to empty session */
      ExaSes_ResetSession (&Session);
      Session.ExaCod = Exam.ExaCod;
     }
   else
     {
      /* Get session data from database */
      ExaSes_GetDataOfSessionByCod (&Session);
      if (Session.ExaCod != Exam.ExaCod)
	 Lay_WrongExamExit ();
      Exams.SesCod = Session.SesCod;
     }

   /***** Get parameters from form *****/
   /* Get session title */
   Par_GetParToText ("Title",Session.Title,ExaSes_MAX_BYTES_TITLE);

   /* Get start/end date-times */
   Session.TimeUTC[Dat_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   Session.TimeUTC[Dat_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /* Get groups associated to the session */
   Grp_GetParCodsSeveralGrps ();

   /***** Create/update session *****/
   if (ItsANewSession)
      ExaSes_CreateSession (&Session);
   else
     {
      if (Session.TimeUTC[Dat_END_TIME] >= Gbl.StartExecutionTimeUTC)	// End of time is in the future
         Session.ShowUsrResults = false;	// Force results to be hidden
      ExaSes_UpdateSession (&Session);
     }

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();

   /***** Write success message *****/
   if (ItsANewSession)
      Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_event_X,
		     Session.Title);
   else
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_event_has_been_modified);

   /***** Get exam data again to update it after changes in session *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Session,
	                false);	// Do not put form for session
  }

/*****************************************************************************/
/*************************** Create a new session ****************************/
/*****************************************************************************/

static void ExaSes_CreateSession (struct ExaSes_Session *Session)
  {
   /***** Insert this new exam session into database *****/
   Session->SesCod =
   DB_QueryINSERTandReturnCode ("can not create exam session",
				"INSERT exa_sessions"
				" (ExaCod,"
				  "Hidden,"
				  "UsrCod,"
				  "StartTime,"
				  "EndTime,"
				  "Title,"
				  "ShowUsrResults)"
				" VALUES"
				" (%ld,"		// ExaCod
                                 "'%c',"		// Hidden
				 "%ld,"			// UsrCod
                                 "FROM_UNIXTIME(%ld),"	// Start time
                                 "FROM_UNIXTIME(%ld),"	// End time
				 "'%s',"		// Title
				 "'N')",		// ShowUsrResults: Don't show user results initially
				Session->ExaCod,
				Session->Hidden ? 'Y' :
					          'N',
				Gbl.Usrs.Me.UsrDat.UsrCod,		// Session creator
				Session->TimeUTC[Dat_START_TIME],	// Start time
				Session->TimeUTC[Dat_END_TIME  ],	// End time
				Session->Title);

   /***** Create groups associated to the exam session *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      ExaSes_CreateGrps (Session->SesCod);
  }

/*****************************************************************************/
/************************ Update an existing session *************************/
/*****************************************************************************/

static void ExaSes_UpdateSession (struct ExaSes_Session *Session)
  {
   /***** Insert this new exam session into database *****/
   DB_QueryUPDATE ("can not update exam session",
		   "UPDATE exa_sessions,"
		          "exa_exams"
		     " SET exa_sessions.Hidden='%c',"
		          "exa_sessions.StartTime=FROM_UNIXTIME(%ld),"
                          "exa_sessions.EndTime=FROM_UNIXTIME(%ld),"
                          "exa_sessions.Title='%s',"
                          "exa_sessions.ShowUsrResults='%c'"
		   " WHERE exa_sessions.SesCod=%ld"
		     " AND exa_sessions.ExaCod=%ld"	// Extra check
		     " AND exa_sessions.ExaCod=exa_exams.ExaCod"
		     " AND exa_exams.CrsCod=%ld",	// Extra check
		   Session->Hidden ? 'Y' :
			             'N',
	           Session->TimeUTC[Dat_START_TIME],	// Start time
		   Session->TimeUTC[Dat_END_TIME  ],	// End time
		   Session->Title,
		   Session->ShowUsrResults ? 'Y' :
			                     'N',
		   Session->SesCod,
		   Session->ExaCod,
		   Gbl.Hierarchy.Crs.CrsCod);

   /***** Update groups associated to the exam session *****/
   ExaSes_RemoveGroups (Session->SesCod);	// Remove all groups associated to this session
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      ExaSes_CreateGrps (Session->SesCod);	// Associate new groups
  }

/*****************************************************************************/
/*************** Create groups associated to an exam session *****************/
/*****************************************************************************/

static void ExaSes_CreateGrps (long SesCod)
  {
   unsigned NumGrpSel;

   /***** Create groups associated to the exam session *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
      /* Create group */
      DB_QueryINSERT ("can not associate a group to an exam session",
		      "INSERT INTO exa_groups"
		      " (SesCod,GrpCod)"
		      " VALUES"
		      " (%ld,%ld)",
                      SesCod,
                      Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
  }

/*****************************************************************************/
/******************** Remove all groups from one session *********************/
/*****************************************************************************/

static void ExaSes_RemoveGroups (long SesCod)
  {
   /***** Remove all groups from one session *****/
   DB_QueryDELETE ("can not remove groups associated to a session",
		   "DELETE FROM exa_groups"
		   " WHERE SesCod=%ld",
		   SesCod);
  }

/*****************************************************************************/
/******************** Remove one group from all sessions *********************/
/*****************************************************************************/

void ExaSes_RemoveGroup (long GrpCod)
  {
   /***** Remove group from all the sessions *****/
   DB_QueryDELETE ("can not remove group"
	           " from the associations between sessions and groups",
		   "DELETE FROM exa_groups"
		   " WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/**************** Remove groups of one type from all sessions ****************/
/*****************************************************************************/

void ExaSes_RemoveGroupsOfType (long GrpTypCod)
  {
   /***** Remove group from all the sessions *****/
   DB_QueryDELETE ("can not remove groups of a type"
	           " from the associations between sessions and groups",
		   "DELETE FROM exa_groups"
		   " USING grp_groups,"
		          "exa_groups"
		   " WHERE grp_groups.GrpTypCod=%ld"
		     " AND grp_groups.GrpCod=exa_groups.GrpCod",
                   GrpTypCod);
  }

/*****************************************************************************/
/********************* Get number of sessions in an exam *********************/
/*****************************************************************************/

unsigned ExaSes_GetNumSessionsInExam (long ExaCod)
  {
   /***** Trivial check *****/
   if (ExaCod < 0)	// A non-existing exam...
      return 0;		// ...has no sessions

   /***** Get number of sessions in an exam from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of sessions of an exam",
		  "SELECT COUNT(*)"
		   " FROM exa_sessions"
		  " WHERE ExaCod=%ld",
		  ExaCod);
  }

/*****************************************************************************/
/***************** Get number of open sessions in an exam ********************/
/*****************************************************************************/

unsigned ExaSes_GetNumOpenSessionsInExam (long ExaCod)
  {
   /***** Trivial check *****/
   if (ExaCod < 0)	// A non-existing exam...
      return 0;		// ...has no sessions

   /***** Get number of open sessions in an exam from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of open sessions of an exam",
		  "SELECT COUNT(*)"
		   " FROM exa_sessions"
		  " WHERE ExaCod=%ld"
		    " AND NOW() BETWEEN StartTime AND EndTime",
		  ExaCod);
  }

/*****************************************************************************/
/******** Check if I belong to any of the groups of an exam session **********/
/*****************************************************************************/

bool ExaSes_CheckIfICanAnswerThisSession (const struct Exa_Exam *Exam,
                                          const struct ExaSes_Session *Session)
  {
   /***** 1. Sessions in hidden exams are not accesible
          2. Hidden or closed sessions are not accesible *****/
   if (Exam->Hidden ||
       Session->Hidden ||
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
	 return (DB_QueryCOUNT ("can not check if I can play an exam session",
				"SELECT COUNT(*)"
				 " FROM exa_sessions"
				" WHERE SesCod=%ld"
				" AND (SesCod NOT IN"
				     " (SELECT SesCod FROM exa_groups)"
				     " OR"
				     " SesCod IN"
				     " (SELECT exa_groups.SesCod"
					" FROM exa_groups,"
					      "grp_users"
				       " WHERE grp_users.UsrCod=%ld"
					 " AND grp_users.GrpCod=exa_groups.GrpCod))",
				SesCod,Gbl.Usrs.Me.UsrDat.UsrCod) != 0);
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }
