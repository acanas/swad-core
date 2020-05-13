// swad_exam_event.c: exam events (each ocurrence of an exam)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_date.h"
#include "swad_exam.h"
#include "swad_exam_event.h"
#include "swad_exam_print.h"
#include "swad_exam_result.h"
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

#define ExaEvt_ICON_CLOSE		"fas fa-times"
#define ExaEvt_ICON_PLAY		"fas fa-play"
#define ExaEvt_ICON_PAUSE		"fas fa-pause"
#define ExaEvt_ICON_PREVIOUS	"fas fa-step-backward"
#define ExaEvt_ICON_NEXT		"fas fa-step-forward"
#define ExaEvt_ICON_RESULTS	"fas fa-chart-bar"

#define ExaEvt_COUNTDOWN_SECONDS_LARGE  60
#define ExaEvt_COUNTDOWN_SECONDS_MEDIUM 30
#define ExaEvt_COUNTDOWN_SECONDS_SMALL  10

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

typedef enum
  {
   ExaEvt_CHANGE_STATUS_BY_STUDENT,
   ExaEvt_REFRESH_STATUS_BY_SERVER,
  } ExaEvt_Update_t;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaEvt_PutIconsInListOfEvents (void *Exams);
static void ExaEvt_PutIconToCreateNewEvent (struct Exa_Exams *Exams);

static void ExaEvt_ListOneOrMoreEvents (struct Exa_Exams *Exams,
                                        const struct Exa_Exam *Exam,
                                        long EvtCodToBeEdited,
				        unsigned NumEvents,
                                        MYSQL_RES *mysql_res);
static void ExaEvt_ListOneOrMoreEventsHeading (bool ICanEditEvents);
static bool ExaEvt_CheckIfICanEditEvents (void);
static bool ExaEvt_CheckIfICanEditThisEvent (const struct ExaEvt_Event *Event);
static void ExaEvt_ListOneOrMoreEventsIcons (struct Exa_Exams *Exams,
                                             const struct ExaEvt_Event *Event,
					     const char *Anchor);
static void ExaEvt_ListOneOrMoreEventsAuthor (const struct ExaEvt_Event *Event);
static void ExaEvt_ListOneOrMoreEventsTimes (const struct ExaEvt_Event *Event,unsigned UniqueId);
static void ExaEvt_ListOneOrMoreEventsTitleGrps (struct Exa_Exams *Exams,
                                                 const struct ExaEvt_Event *Event,
                                                 const char *Anchor);
static void ExaEvt_GetAndWriteNamesOfGrpsAssociatedToEvent (const struct ExaEvt_Event *Event);
static void ExaEvt_ListOneOrMoreEventsResult (struct Exa_Exams *Exams,
                                              const struct ExaEvt_Event *Event);
static void ExaEvt_ListOneOrMoreEventsResultStd (struct Exa_Exams *Exams,
                                                 const struct ExaEvt_Event *Event);
static void ExaEvt_ListOneOrMoreEventsResultTch (struct Exa_Exams *Exams,
                                                 const struct ExaEvt_Event *Event);

static void ExaEvt_GetEventDataFromRow (MYSQL_RES *mysql_res,
				        struct ExaEvt_Event *Event);

static void ExaEvt_RemoveEventFromAllTables (long EvtCod);
static void ExaEvt_RemoveEventFromTable (long EvtCod,const char *TableName);
static void ExaEvt_RemoveEventsInExamFromTable (long ExaCod,const char *TableName);
static void ExaEvt_RemoveEventInCourseFromTable (long CrsCod,const char *TableName);
static void ExaEvt_RemoveUsrEvtResultsInCrs (long UsrCod,long CrsCod,const char *TableName);

static void ExaEvt_PutParamEvtCod (long EvtCod);

static void ExaEvt_PutFormEvent (const struct ExaEvt_Event *Event);
static void ExaEvt_ShowLstGrpsToCreateEvent (long EvtCod);

static void ExaEvt_CreateEvent (struct ExaEvt_Event *Event);
static void ExaEvt_UpdateEvent (struct ExaEvt_Event *Event);

static void ExaEvt_CreateGrps (long EvtCod);
static void ExaEvt_RemoveGroups (long EvtCod);

/*****************************************************************************/
/****************************** Reset exam event *****************************/
/*****************************************************************************/

void ExaEvt_ResetEvent (struct ExaEvt_Event *Event)
  {
   Dat_StartEndTime_t StartEndTime;

   /***** Initialize to empty match *****/
   Event->EvtCod                  = -1L;
   Event->ExaCod                  = -1L;
   Event->UsrCod                  = -1L;
   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
      Event->TimeUTC[StartEndTime] = (time_t) 0;
   Event->Title[0]                = '\0';
   Event->Hidden		  = false;
   Event->Open			  = false;
   Event->ShowUsrResults          = false;
  };

/*****************************************************************************/
/************************* List the events of an exam ************************/
/*****************************************************************************/

void ExaEvt_ListEvents (struct Exa_Exams *Exams,
                        struct Exa_Exam *Exam,
		        struct ExaEvt_Event *Event,
                        bool PutFormEvent)
  {
   extern const char *Hlp_ASSESSMENT_Exams_events;
   extern const char *Txt_Events;
   char *SubQuery;
   MYSQL_RES *mysql_res;
   unsigned NumEvents;
   long EvtCodToBeEdited;
   bool PutFormNewEvent;

   /***** Get data of events from database *****/
   /* Fill subquery for exam */
   if (Gbl.Crs.Grps.WhichGrps == Grp_MY_GROUPS)
     {
      if (asprintf (&SubQuery," AND"
			      "(EvtCod NOT IN"
			      " (SELECT EvtCod FROM exa_groups)"
			      " OR"
			      " EvtCod IN"
			      " (SELECT exa_groups.EvtCod"
			      " FROM exa_groups,crs_grp_usr"
			      " WHERE crs_grp_usr.UsrCod=%ld"
			      " AND exa_groups.GrpCod=crs_grp_usr.GrpCod))",
		     Gbl.Usrs.Me.UsrDat.UsrCod) < 0)
	  Lay_NotEnoughMemoryExit ();
      }
    else	// Gbl.Crs.Grps.WhichGrps == Grp_ALL_GROUPS
       if (asprintf (&SubQuery,"%s","") < 0)
	  Lay_NotEnoughMemoryExit ();

   /* Make query */
   NumEvents = (unsigned)
	       DB_QuerySELECT (&mysql_res,"can not get events",
			       "SELECT EvtCod,"					// row[0]
				      "ExaCod,"					// row[1]
				      "Hidden,"					// row[2]
				      "UsrCod,"					// row[3]
				      "UNIX_TIMESTAMP(StartTime),"		// row[4]
				      "UNIX_TIMESTAMP(EndTime),"		// row[5]
				      "NOW() BETWEEN StartTime AND EndTime,"	// row[6]
				      "Title,"					// row[7]
				      "ShowUsrResults"				// row[8]
			       " FROM exa_events"
			       " WHERE ExaCod=%ld%s"
			       " ORDER BY EvtCod",
			       Exam->ExaCod,
			       SubQuery);

   /* Free allocated memory for subquery */
   free (SubQuery);

   /***** Begin box *****/
   Exams->ExaCod = Exam->ExaCod;
   Box_BoxBegin ("100%",Txt_Events,
                 ExaEvt_PutIconsInListOfEvents,Exams,
                 Hlp_ASSESSMENT_Exams_events,Box_NOT_CLOSABLE);

   /***** Select whether show only my groups or all groups *****/
   if (Gbl.Crs.Grps.NumGrps)
     {
      Set_StartSettingsHead ();
      Grp_ShowFormToSelWhichGrps (ActSeeExa,
                                  Exa_PutParams,Exams);
      Set_EndSettingsHead ();
     }

   if (NumEvents)
     {
      /***** Show the table with the events *****/
      EvtCodToBeEdited = PutFormEvent && Event->EvtCod > 0 ? Event->EvtCod :
	                                                     -1L;
      ExaEvt_ListOneOrMoreEvents (Exams,Exam,EvtCodToBeEdited,NumEvents,mysql_res);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Put button to create a new exam event in this exam *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 PutFormNewEvent = PutFormEvent && Event->EvtCod <= 0;
	 if (PutFormNewEvent)
	   {
	    /* Reset event */
	    ExaEvt_ResetEvent (Event);
	    Event->ExaCod = Exam->ExaCod;
	    Event->TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;			// Now
	    Event->TimeUTC[Dat_END_TIME  ] = Gbl.StartExecutionTimeUTC + (1 * 60 * 60);	// Now + 1 hour
            Str_Copy (Event->Title,Exam->Title,
                      ExaEvt_MAX_BYTES_TITLE);

	    /* Put form to create new event */
	    ExaEvt_PutFormEvent (Event);	// Form to create event
	   }
	 else
	    ExaEvt_PutButtonNewEvent (Exams,Exam->ExaCod);	// Button to create a new exam event
	 break;
      default:
	 break;
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************** Get exam event data using its code *********************/
/*****************************************************************************/

void ExaEvt_GetDataOfEventByCod (struct ExaEvt_Event *Event)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumRows;

   /***** Trivial check *****/
   if (Event->EvtCod <= 0)
     {
      /* Initialize to empty exam event */
      ExaEvt_ResetEvent (Event);
      return;
     }

   /***** Get exam data event from database *****/
   NumRows = (unsigned)
	     DB_QuerySELECT (&mysql_res,"can not get events",
			     "SELECT EvtCod,"					// row[ 0]
				    "ExaCod,"					// row[ 1]
				    "Hidden,"					// row[ 2]
				    "UsrCod,"					// row[ 3]
				    "UNIX_TIMESTAMP(StartTime),"		// row[ 4]
				    "UNIX_TIMESTAMP(EndTime),"			// row[ 5]
	                     	    "NOW() BETWEEN StartTime AND EndTime,"	// row[ 6]
				    "Title,"					// row[ 7]
				    "QstInd,"					// row[ 8]
				    "QstCod,"					// row[ 9]
				    "Showing,"					// row[10]
				    "Countdown,"				// row[11]
				    "NumCols,"					// row[12]
				    "ShowQstResults,"				// row[13]
				    "ShowUsrResults"				// row[14]
			     " FROM exa_events"
			     " WHERE EvtCod=%ld"
			     " AND ExaCod IN"		// Extra check
			     " (SELECT ExaCod FROM exa_exams"
			     " WHERE CrsCod='%ld')",
			     Event->EvtCod,
			     Gbl.Hierarchy.Crs.CrsCod);
   if (NumRows) // Event found...
      /* Get exam event data from row */
      ExaEvt_GetEventDataFromRow (mysql_res,Event);
   else
      /* Initialize to empty exam event */
      ExaEvt_ResetEvent (Event);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Put icons in list of events of an exam *******************/
/*****************************************************************************/

static void ExaEvt_PutIconsInListOfEvents (void *Exams)
  {
   bool ICanEditEvents;

   if (Exams)
     {
      /***** Put icon to create a new exam event in current exam *****/
      ICanEditEvents = ExaEvt_CheckIfICanEditEvents ();
      if (ICanEditEvents)
	 ExaEvt_PutIconToCreateNewEvent ((struct Exa_Exams *) Exams);
     }
  }

/*****************************************************************************/
/******************* Put icon to create a new exam event *********************/
/*****************************************************************************/

static void ExaEvt_PutIconToCreateNewEvent (struct Exa_Exams *Exams)
  {
   extern const char *Txt_New_event;

   /***** Put form to create a new exam event *****/
   Ico_PutContextualIconToAdd (ActReqNewExaEvt,ExaEvt_NEW_EVENT_SECTION_ID,
                               Exa_PutParams,Exams,
			       Txt_New_event);
  }

/*****************************************************************************/
/*********************** List exam events for edition ************************/
/*****************************************************************************/

static void ExaEvt_ListOneOrMoreEvents (struct Exa_Exams *Exams,
                                        const struct Exa_Exam *Exam,
                                        long EvtCodToBeEdited,
				        unsigned NumEvents,
                                        MYSQL_RES *mysql_res)
  {
   unsigned NumEvent;
   unsigned UniqueId;
   char *Anchor;
   struct ExaEvt_Event Event;
   bool ICanEditEvents = ExaEvt_CheckIfICanEditEvents ();

   /***** Trivial check *****/
   if (!NumEvents)
      return;

   /***** Reset event *****/
   ExaEvt_ResetEvent (&Event);

   /***** Write the heading *****/
   HTM_TABLE_BeginWidePadding (2);
   ExaEvt_ListOneOrMoreEventsHeading (ICanEditEvents);

   /***** Write rows *****/
   for (NumEvent = 0, UniqueId = 1;
	NumEvent < NumEvents;
	NumEvent++, UniqueId++)
     {
      Gbl.RowEvenOdd = NumEvent % 2;

      /***** Get exam event data from row *****/
      ExaEvt_GetEventDataFromRow (mysql_res,&Event);

      if (ExaEvt_CheckIfICanPlayThisEventBasedOnGrps (&Event))
	{
	 /***** Build anchor string *****/
	 if (asprintf (&Anchor,"evt_%ld_%ld",Exam->ExaCod,Event.EvtCod) < 0)
	    Lay_NotEnoughMemoryExit ();

	 /***** Begin row for this exam event ****/
	 HTM_TR_Begin (NULL);

	 /* Icons */
	 if (ICanEditEvents)
	    if (ExaEvt_CheckIfICanEditThisEvent (&Event))
	       ExaEvt_ListOneOrMoreEventsIcons (Exams,&Event,Anchor);

	 /* Event participant */
	 ExaEvt_ListOneOrMoreEventsAuthor (&Event);

	 /* Start/end date/time */
	 ExaEvt_ListOneOrMoreEventsTimes (&Event,UniqueId);

	 /* Title and groups */
	 ExaEvt_ListOneOrMoreEventsTitleGrps (Exams,&Event,Anchor);

	 /* Event result visible? */
	 ExaEvt_ListOneOrMoreEventsResult (Exams,&Event);

	 /***** End row for this event ****/
	 HTM_TR_End ();

	 /***** For to edit this event ****/
	 if (Event.EvtCod == EvtCodToBeEdited)
	   {
	    HTM_TR_Begin (NULL);
            HTM_TD_Begin ("colspan=\"6\" class=\"CT COLOR%u\"",Gbl.RowEvenOdd);
	    ExaEvt_PutFormEvent (&Event);	// Form to edit existing event
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
/************** Put a column for exam event start and end times **************/
/*****************************************************************************/

static void ExaEvt_ListOneOrMoreEventsHeading (bool ICanEditEvents)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Event;
   extern const char *Txt_Results;

   /***** Start row *****/
   HTM_TR_Begin (NULL);

   /***** Column for icons *****/
   if (ICanEditEvents)
      HTM_TH_Empty (1);

   /***** The rest of columns *****/
   HTM_TH (1,1,"LT",Txt_ROLES_SINGUL_Abc[Rol_TCH][Usr_SEX_UNKNOWN]);
   HTM_TH (1,1,"LT",Txt_START_END_TIME[Exa_ORDER_BY_START_DATE]);
   HTM_TH (1,1,"LT",Txt_START_END_TIME[Exa_ORDER_BY_END_DATE  ]);
   HTM_TH (1,1,"LT",Txt_Event);
   HTM_TH (1,1,"CT",Txt_Results);

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*********************** Check if I can edit events **************************/
/*****************************************************************************/

static bool ExaEvt_CheckIfICanEditEvents (void)
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
/************** Check if I can edit (remove/resume) an exam event ************/
/*****************************************************************************/

static bool ExaEvt_CheckIfICanEditThisEvent (const struct ExaEvt_Event *Event)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
	 return (Event->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);	// Only if I am the creator
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/************************* Put a column for icons ****************************/
/*****************************************************************************/

static void ExaEvt_ListOneOrMoreEventsIcons (struct Exa_Exams *Exams,
                                             const struct ExaEvt_Event *Event,
					     const char *Anchor)
  {
   /***** Begin cell *****/
   HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);

   Exams->ExaCod = Event->ExaCod;
   Exams->EvtCod = Event->EvtCod;

   /***** Icon to remove the exam event *****/
   Frm_StartForm (ActReqRemExaEvt);
   ExaEvt_PutParamsEdit (Exams);
   Ico_PutIconRemove ();
   Frm_EndForm ();

   /***** Icon to hide/unhide the exam event *****/
   if (Event->Hidden)
      Ico_PutContextualIconToUnhide (ActShoExaEvt,Anchor,
				     ExaEvt_PutParamsEdit,Exams);
   else
      Ico_PutContextualIconToHide (ActHidExaEvt,Anchor,
				   ExaEvt_PutParamsEdit,Exams);

   /***** Icon to edit the exam event *****/
   Ico_PutContextualIconToEdit (ActEdiOneExaEvt,Anchor,
                                ExaEvt_PutParamsEdit,Exams);

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/*********** Put a column for teacher who created the exam event *************/
/*****************************************************************************/

static void ExaEvt_ListOneOrMoreEventsAuthor (const struct ExaEvt_Event *Event)
  {
   /***** Event author (teacher) *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   Usr_WriteAuthor1Line (Event->UsrCod,Event->Hidden);
   HTM_TD_End ();
  }

/*****************************************************************************/
/*************** Put a column for exam event start and end times *************/
/*****************************************************************************/

static void ExaEvt_ListOneOrMoreEventsTimes (const struct ExaEvt_Event *Event,unsigned UniqueId)
  {
   Dat_StartEndTime_t StartEndTime;
   const char *Color;
   char *Id;

   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      Color = Event->Open ? (Event->Hidden ? "DATE_GREEN_LIGHT":
					     "DATE_GREEN") :
			    (Event->Hidden ? "DATE_RED_LIGHT":
					     "DATE_RED");

      if (asprintf (&Id,"exa_time_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"%s LT COLOR%u\"",
		    Id,Color,Gbl.RowEvenOdd);
      Dat_WriteLocalDateHMSFromUTC (Id,Event->TimeUTC[StartEndTime],
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				    true,true,true,0x7);
      HTM_TD_End ();
      free (Id);
     }
  }

/*****************************************************************************/
/*************** Put a column for exam event title and grous *****************/
/*****************************************************************************/

static void ExaEvt_ListOneOrMoreEventsTitleGrps (struct Exa_Exams *Exams,
                                                 const struct ExaEvt_Event *Event,
                                                 const char *Anchor)
  {
   extern const char *Txt_Play;
   extern const char *Txt_Resume;

   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);

   /***** Event title *****/
   HTM_ARTICLE_Begin (Anchor);
   Frm_StartForm (ActSeeExaPrn);
   Exa_PutParams (Exams);
   ExaEvt_PutParamEvtCod (Event->EvtCod);
   HTM_BUTTON_SUBMIT_Begin (Gbl.Usrs.Me.Role.Logged == Rol_STD ? Txt_Play :
								 Txt_Resume,
			    Event->Hidden ? "BT_LINK LT ASG_TITLE_LIGHT":
					    "BT_LINK LT ASG_TITLE",
			    NULL);
   HTM_Txt (Event->Title);
   HTM_BUTTON_End ();
   Frm_EndForm ();
   HTM_ARTICLE_End ();

   /***** Groups whose students can answer this exam event *****/
   if (Gbl.Crs.Grps.NumGrps)
      ExaEvt_GetAndWriteNamesOfGrpsAssociatedToEvent (Event);

   HTM_TD_End ();
  }

/*****************************************************************************/
/********** Get and write the names of the groups of an exam event ***********/
/*****************************************************************************/

static void ExaEvt_GetAndWriteNamesOfGrpsAssociatedToEvent (const struct ExaEvt_Event *Event)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_and;
   extern const char *Txt_The_whole_course;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;

   /***** Get groups associated to an exam event from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get groups of an exam event",
			     "SELECT crs_grp_types.GrpTypName,crs_grp.GrpName"
			     " FROM exa_groups,crs_grp,crs_grp_types"
			     " WHERE exa_groups.EvtCod=%ld"
			     " AND exa_groups.GrpCod=crs_grp.GrpCod"
			     " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
			     " ORDER BY crs_grp_types.GrpTypName,crs_grp.GrpName",
			     Event->EvtCod);

   /***** Write heading *****/
   HTM_DIV_Begin ("class=\"%s\"",Event->Hidden ? "ASG_GRP_LIGHT":
					         "ASG_GRP");
   HTM_TxtColonNBSP (NumRows == 1 ? Txt_Group  :
                                    Txt_Groups);

   /***** Write groups *****/
   if (NumRows) // Groups found...
     {
      /* Get and write the group types and names */
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
        {
         /* Get next group */
         row = mysql_fetch_row (mysql_res);

         /* Write group type name and group name */
         HTM_TxtF ("%s&nbsp;%s",row[0],row[1]);

         if (NumRows >= 2)
           {
            if (NumRow == NumRows-2)
               HTM_TxtF (" %s ",Txt_and);
            if (NumRows >= 3)
              if (NumRow < NumRows-2)
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
/************* Put a column for visibility of exam event result **************/
/*****************************************************************************/

static void ExaEvt_ListOneOrMoreEventsResult (struct Exa_Exams *Exams,
                                              const struct ExaEvt_Event *Event)
  {
   HTM_TD_Begin ("class=\"DAT CT COLOR%u\"",Gbl.RowEvenOdd);

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ExaEvt_ListOneOrMoreEventsResultStd (Exams,Event);
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 ExaEvt_ListOneOrMoreEventsResultTch (Exams,Event);
	 break;
      default:
	 Rol_WrongRoleExit ();
	 break;
     }

   HTM_TD_End ();
  }

static void ExaEvt_ListOneOrMoreEventsResultStd (struct Exa_Exams *Exams,
                                                 const struct ExaEvt_Event *Event)
  {
   extern const char *Txt_Results;

   /***** Is exam event result visible or hidden? *****/
   if (Event->ShowUsrResults)
     {
      /* Result is visible by me */
      Exams->ExaCod = Event->ExaCod;
      Exams->EvtCod = Event->EvtCod;
      Lay_PutContextualLinkOnlyIcon (ActSeeMyExaEvtResEvt,ExaRes_RESULTS_BOX_ID,
				     ExaEvt_PutParamsEdit,Exams,
				     "trophy.svg",
				     Txt_Results);
     }
   else
      /* Result is forbidden to me */
      Ico_PutIconNotVisible ();
  }

static void ExaEvt_ListOneOrMoreEventsResultTch (struct Exa_Exams *Exams,
                                                 const struct ExaEvt_Event *Event)
  {
   extern const char *Txt_Visible_results;
   extern const char *Txt_Hidden_results;
   extern const char *Txt_Results;

   /***** Can I edit exam event vivibility? *****/
   if (ExaEvt_CheckIfICanEditThisEvent (Event))
     {
      Exams->ExaCod = Event->ExaCod;
      Exams->EvtCod = Event->EvtCod;

      /* Show exam event results */
      Lay_PutContextualLinkOnlyIcon (ActSeeAllExaEvtResEvt,ExaRes_RESULTS_BOX_ID,
				     ExaEvt_PutParamsEdit,Exams,
				     "trophy.svg",
				     Txt_Results);

      /* I can edit visibility */
      Lay_PutContextualLinkOnlyIcon (ActChgVisResExaEvtUsr,NULL,
				     ExaEvt_PutParamsEdit,Exams,
				     Event->ShowUsrResults ? "eye-green.svg" :
							     "eye-slash-red.svg",
				     Event->ShowUsrResults ? Txt_Visible_results :
							     Txt_Hidden_results);
     }
   else
      /* I can not edit visibility */
      Ico_PutIconOff (Event->ShowUsrResults ? "eye-green.svg" :
					      "eye-slash-red.svg",
		      Event->ShowUsrResults ? Txt_Visible_results :
					      Txt_Hidden_results);
  }

/*****************************************************************************/
/****************** Toggle visibility of exam event results ******************/
/*****************************************************************************/

void ExaEvt_ToggleVisibilResultsEvtUsr (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);

   /***** Get and check parameters *****/
   ExaEvt_GetAndCheckParameters (&Exams,&Exam,&Event);

   /***** Check if I have permission to change visibility *****/
   if (!ExaEvt_CheckIfICanEditThisEvent (&Event))
      Lay_NoPermissionExit ();

   /***** Toggle visibility of exam event results *****/
   Event.ShowUsrResults = !Event.ShowUsrResults;
   DB_QueryUPDATE ("can not toggle visibility of exam event results",
		   "UPDATE exa_events"
		   " SET ShowUsrResults='%c'"
		   " WHERE EvtCod=%ld",
		   Event.ShowUsrResults ? 'Y' :
			                  'N',
		   Event.EvtCod);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Event,
	                false);	// Do not put form for event
  }

/*****************************************************************************/
/******************** Get exam data from a database row **********************/
/*****************************************************************************/

static void ExaEvt_GetEventDataFromRow (MYSQL_RES *mysql_res,
				        struct ExaEvt_Event *Event)
  {
   MYSQL_ROW row;
   Dat_StartEndTime_t StartEndTime;

   /***** Get exam event data *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]	EvtCod
   row[1]	ExaCod
   row[2]	Hidden
   row[3]	UsrCod
   row[4]	UNIX_TIMESTAMP(StartTime)
   row[5]	UNIX_TIMESTAMP(EndTime)
   row[6]	Open = NOW() BETWEEN StartTime AND EndTime
   row[7]	Title
   row[8]	ShowUsrResults
   */
   /***** Get event data *****/
   /* Code of the event (row[0]) */
   if ((Event->EvtCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of exam event.");

   /* Code of the exam (row[1]) */
   if ((Event->ExaCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of exam.");

   /* Get whether the event is hidden (row[2]) */
   Event->Hidden = (row[2][0] == 'Y');

   /* Get event teacher (row[3]) */
   Event->UsrCod = Str_ConvertStrCodToLongCod (row[3]);

   /* Get start/end times (row[4], row[5] hold start/end UTC times) */
   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
      Event->TimeUTC[StartEndTime] = Dat_GetUNIXTimeFromStr (row[4 + StartEndTime]);

   /* Get whether the event is open or closed (row(6)) */
   Event->Open = (row[6][0] == '1');

   /* Get the title of the event (row[7]) */
   if (row[7])
      Str_Copy (Event->Title,row[7],
		ExaEvt_MAX_BYTES_TITLE);
   else
      Event->Title[0] = '\0';

   /* Get whether to show user results or not (row(8)) */
   Event->ShowUsrResults = (row[8][0] == 'Y');
  }

/*****************************************************************************/
/*********** Request the removal of an exam event (exam instance) ************/
/*****************************************************************************/

void ExaEvt_RequestRemoveEvent (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_event_X;
   extern const char *Txt_Remove_event;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);

   /***** Get and check parameters *****/
   ExaEvt_GetAndCheckParameters (&Exams,&Exam,&Event);

   /***** Show question and button to remove question *****/
   Exams.ExaCod = Event.ExaCod;
   Exams.EvtCod = Event.EvtCod;
   Ale_ShowAlertAndButton (ActRemExaEvt,NULL,NULL,
                           ExaEvt_PutParamsEdit,&Exams,
			   Btn_REMOVE_BUTTON,Txt_Remove_event,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_event_X,
	                   Event.Title);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Event,
	                false);	// Do not put form for event
  }

/*****************************************************************************/
/******************* Remove an exam event (exam instance) ********************/
/*****************************************************************************/

void ExaEvt_RemoveEvent (void)
  {
   extern const char *Txt_Event_X_removed;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);

   /***** Get and check parameters *****/
   ExaEvt_GetAndCheckParameters (&Exams,&Exam,&Event);

   /***** Check if I can remove this exam event *****/
   if (!ExaEvt_CheckIfICanEditThisEvent (&Event))
      Lay_NoPermissionExit ();

   /***** Remove the exam event from all database tables *****/
   ExaEvt_RemoveEventFromAllTables (Event.EvtCod);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Event_X_removed,
		  Event.Title);

   /***** Get exam data again to update it after changes in event *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Event,
	                false);	// Do not put form for event
  }

/*****************************************************************************/
/******************** Remove exam event from all tables **********************/
/*****************************************************************************/
/*
mysql> SELECT table_name FROM information_schema.tables WHERE table_name LIKE 'exa%';
*/
static void ExaEvt_RemoveEventFromAllTables (long EvtCod)
  {
   /***** Remove exam event from secondary tables *****/
   ExaEvt_RemoveEventFromTable (EvtCod,"exa_participants");
   ExaEvt_RemoveEventFromTable (EvtCod,"exa_happening");
   ExaEvt_RemoveEventFromTable (EvtCod,"exa_results");
   ExaEvt_RemoveEventFromTable (EvtCod,"exa_answers");
   ExaEvt_RemoveEventFromTable (EvtCod,"exa_times");
   ExaEvt_RemoveEventFromTable (EvtCod,"exa_groups");
   ExaEvt_RemoveEventFromTable (EvtCod,"exa_indexes");

   /***** Remove exam event from main table *****/
   DB_QueryDELETE ("can not remove exam event",
		   "DELETE FROM exa_events WHERE EvtCod=%ld",
		   EvtCod);
  }

static void ExaEvt_RemoveEventFromTable (long EvtCod,const char *TableName)
  {
   /***** Remove exam event from secondary table *****/
   DB_QueryDELETE ("can not remove exam event from table",
		   "DELETE FROM %s WHERE EvtCod=%ld",
		   TableName,
		   EvtCod);
  }

/*****************************************************************************/
/****************** Remove exam event in exam from all tables ****************/
/*****************************************************************************/

void ExaEvt_RemoveEventsInExamFromAllTables (long ExaCod)
  {
   /***** Remove events from secondary tables *****/
   ExaEvt_RemoveEventsInExamFromTable (ExaCod,"exa_participants");
   ExaEvt_RemoveEventsInExamFromTable (ExaCod,"exa_happening");
   ExaEvt_RemoveEventsInExamFromTable (ExaCod,"exa_results");
   ExaEvt_RemoveEventsInExamFromTable (ExaCod,"exa_answers");
   ExaEvt_RemoveEventsInExamFromTable (ExaCod,"exa_times");
   ExaEvt_RemoveEventsInExamFromTable (ExaCod,"exa_groups");
   ExaEvt_RemoveEventsInExamFromTable (ExaCod,"exa_indexes");

   /***** Remove events from main table *****/
   DB_QueryDELETE ("can not remove events of an exam",
		   "DELETE FROM exa_events WHERE ExaCod=%ld",
		   ExaCod);
  }

static void ExaEvt_RemoveEventsInExamFromTable (long ExaCod,const char *TableName)
  {
   /***** Remove events in exam from secondary table *****/
   DB_QueryDELETE ("can not remove events of an exam from table",
		   "DELETE FROM %s"
		   " USING exa_events,%s"
		   " WHERE exa_events.ExaCod=%ld"
		   " AND exa_events.EvtCod=%s.EvtCod",
		   TableName,
		   TableName,
		   ExaCod,
		   TableName);
  }

/*****************************************************************************/
/***************** Remove exam event in course from all tables ***************/
/*****************************************************************************/

void ExaEvt_RemoveEventInCourseFromAllTables (long CrsCod)
  {
   /***** Remove events from secondary tables *****/
   ExaEvt_RemoveEventInCourseFromTable (CrsCod,"exa_participants");
   ExaEvt_RemoveEventInCourseFromTable (CrsCod,"exa_happening");
   ExaEvt_RemoveEventInCourseFromTable (CrsCod,"exa_results");
   ExaEvt_RemoveEventInCourseFromTable (CrsCod,"exa_answers");
   ExaEvt_RemoveEventInCourseFromTable (CrsCod,"exa_times");
   ExaEvt_RemoveEventInCourseFromTable (CrsCod,"exa_groups");
   ExaEvt_RemoveEventInCourseFromTable (CrsCod,"exa_indexes");

   /***** Remove events from main table *****/
   DB_QueryDELETE ("can not remove events of a course",
		   "DELETE FROM exa_events"
		   " USING exa_exams,exa_events"
		   " WHERE exa_exams.CrsCod=%ld"
		   " AND exa_exams.ExaCod=exa_events.ExaCod",
		   CrsCod);
  }

static void ExaEvt_RemoveEventInCourseFromTable (long CrsCod,const char *TableName)
  {
   /***** Remove events in course from secondary table *****/
   DB_QueryDELETE ("can not remove events of a course from table",
		   "DELETE FROM %s"
		   " USING exa_exams,exa_events,%s"
		   " WHERE exa_exams.CrsCod=%ld"
		   " AND exa_exams.ExaCod=exa_events.ExaCod"
		   " AND exa_events.EvtCod=%s.EvtCod",
		   TableName,
		   TableName,
		   CrsCod,
		   TableName);
  }

/*****************************************************************************/
/************** Remove user from secondary exam event tables *****************/
/*****************************************************************************/

void ExaEvt_RemoveUsrFromEventTablesInCrs (long UsrCod,long CrsCod)
  {
   /***** Remove student from secondary tables *****/
   ExaEvt_RemoveUsrEvtResultsInCrs (UsrCod,CrsCod,"exa_participants");
   ExaEvt_RemoveUsrEvtResultsInCrs (UsrCod,CrsCod,"exa_results");
   ExaEvt_RemoveUsrEvtResultsInCrs (UsrCod,CrsCod,"exa_answers");
  }

static void ExaEvt_RemoveUsrEvtResultsInCrs (long UsrCod,long CrsCod,const char *TableName)
  {
   /***** Remove events in course from secondary table *****/
   DB_QueryDELETE ("can not remove events of a user from table",
		   "DELETE FROM %s"
		   " USING exa_exams,exa_events,%s"
		   " WHERE exa_exams.CrsCod=%ld"
		   " AND exa_exams.ExaCod=exa_events.ExaCod"
		   " AND exa_events.EvtCod=%s.EvtCod"
		   " AND %s.UsrCod=%ld",
		   TableName,
		   TableName,
		   CrsCod,
		   TableName,
		   TableName,
		   UsrCod);
  }

/*****************************************************************************/
/******************************** Hide an event ******************************/
/*****************************************************************************/

void ExaEvt_HideEvent (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);

   /***** Get and check parameters *****/
   ExaEvt_GetAndCheckParameters (&Exams,&Exam,&Event);

   /***** Check if I can remove this exam event *****/
   if (!ExaEvt_CheckIfICanEditThisEvent (&Event))
      Lay_NoPermissionExit ();

   /***** Hide event *****/
   DB_QueryUPDATE ("can not hide exam event",
		   "UPDATE exa_events SET Hidden='Y'"
		   " WHERE EvtCod=%ld"
		   " AND ExaCod=%ld",	// Extra check
		   Event.EvtCod,Event.ExaCod);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Event,
	                false);	// Do not put form for event
  }

/*****************************************************************************/
/****************************** Unhide an event ******************************/
/*****************************************************************************/

void ExaEvt_UnhideEvent (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);

   /***** Get and check parameters *****/
   ExaEvt_GetAndCheckParameters (&Exams,&Exam,&Event);

   /***** Check if I can remove this exam event *****/
   if (!ExaEvt_CheckIfICanEditThisEvent (&Event))
      Lay_NoPermissionExit ();

   /***** Unhide event *****/
   DB_QueryUPDATE ("can not unhide exam event",
		   "UPDATE exa_events SET Hidden='N'"
		   " WHERE EvtCod=%ld"
		   " AND ExaCod=%ld",	// Extra check
		   Event.EvtCod,Event.ExaCod);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Event,
	                false);	// Do not put form for event
  }

/*****************************************************************************/
/******************** Params used to edit an exam event **********************/
/*****************************************************************************/

void ExaEvt_PutParamsEdit (void *Exams)
  {
   if (Exams)
     {
      Exa_PutParams (Exams);
      ExaEvt_PutParamEvtCod (((struct Exa_Exams *) Exams)->EvtCod);
     }
  }

/*****************************************************************************/
/***************** Write parameter with code of exam event *******************/
/*****************************************************************************/

static void ExaEvt_PutParamEvtCod (long EvtCod)
  {
   Par_PutHiddenParamLong (NULL,"EvtCod",EvtCod);
  }

/*****************************************************************************/
/************************** Get and check parameters *************************/
/*****************************************************************************/

void ExaEvt_GetAndCheckParameters (struct Exa_Exams *Exams,
                                   struct Exa_Exam *Exam,
                                   struct ExaEvt_Event *Event)
  {
   /***** Get parameters *****/
   Exa_GetParams (Exams);
   if (Exams->ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam->ExaCod = Exams->ExaCod;
   Grp_GetParamWhichGroups ();
   if ((Event->EvtCod = ExaEvt_GetParamEvtCod ()) <= 0)
      Lay_WrongEventExit ();

   /***** Get exam data and event from database *****/
   Exa_GetDataOfExamByCod (Exam);
   if (Exam->CrsCod != Gbl.Hierarchy.Crs.CrsCod)
      Lay_WrongExamExit ();
   Exams->ExaCod = Exam->ExaCod;
   ExaEvt_GetDataOfEventByCod (Event);

   /***** Ensure parameters are correct *****/
   if (Exam->ExaCod != Event->ExaCod ||
       Exam->CrsCod != Gbl.Hierarchy.Crs.CrsCod)
      Lay_WrongExamExit ();
  }

/*****************************************************************************/
/***************** Get parameter with code of exam event *********************/
/*****************************************************************************/

long ExaEvt_GetParamEvtCod (void)
  {
   /***** Get code of exam event *****/
   return Par_GetParToLong ("EvtCod");
  }

/*****************************************************************************/
/* Put a big button to play exam event (start a new exam event) as a teacher */
/*****************************************************************************/

static void ExaEvt_PutFormEvent (const struct ExaEvt_Event *Event)
  {
   extern const char *Hlp_ASSESSMENT_Exams_events;
   extern const char *Txt_New_event;
   extern const char *Txt_Title;
   extern const char *Txt_Create_event;
   extern const char *Txt_Save_changes;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_START_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME  ] = Dat_HMS_DO_NOT_SET
     };
   bool ItsANewEvent = Event->EvtCod <= 0;

   /***** Start section for a new exam event *****/
   HTM_SECTION_Begin (ExaEvt_NEW_EVENT_SECTION_ID);

   /***** Begin form *****/
   Frm_StartForm (ItsANewEvent ? ActNewExaEvt :	// New event
	                         ActChgExaEvt);	// Existing event
   Exa_PutParamExamCod (Event->ExaCod);
   if (!ItsANewEvent)	// Existing event
      ExaEvt_PutParamEvtCod (Event->EvtCod);

   // Exa_PutParamQstInd (0);	// Start by first question in exam

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,ItsANewEvent ? Txt_New_event :
					  Event->Title,
                      NULL,NULL,
		      Hlp_ASSESSMENT_Exams_events,Box_NOT_CLOSABLE,2);

   /***** Event title *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","Title",Txt_Title);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_INPUT_TEXT ("Title",ExaEvt_MAX_CHARS_TITLE,Event->Title,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "id=\"Title\" size=\"45\" required=\"required\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Start and end dates *****/
   Dat_PutFormStartEndClientLocalDateTimes (Event->TimeUTC,
                                            Dat_FORM_SECONDS_ON,
					    SetHMS);

   /***** Groups *****/
   ExaEvt_ShowLstGrpsToCreateEvent (Event->EvtCod);

   /***** End table, send button and end box *****/
   if (ItsANewEvent)
      Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_event);
   else
      Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** End section for a new exam event *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/************** Show list of groups to create a new exam event ***************/
/*****************************************************************************/

static void ExaEvt_ShowLstGrpsToCreateEvent (long EvtCod)
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
      HTM_TxtF ("%s:",Txt_Groups);
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
			  Grp_CheckIfAssociatedToGrps ("exa_groups","EvtCod",EvtCod) ? "" :
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
                                            EvtCod,
					    Grp_EXA_EVENT);

      /***** End table and box *****/
      Box_BoxTableEnd ();
      HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/********************* Put button to create a new event **********************/
/*****************************************************************************/

void ExaEvt_PutButtonNewEvent (struct Exa_Exams *Exams,long ExaCod)
  {
   extern const char *Txt_New_event;

   Exams->ExaCod = ExaCod;
   Frm_StartFormAnchor (ActReqNewExaEvt,ExaEvt_NEW_EVENT_SECTION_ID);
   Exa_PutParams (Exams);
   Btn_PutConfirmButton (Txt_New_event);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************* Request the creation of a new event *********************/
/*****************************************************************************/

void ExaEvt_RequestCreatOrEditEvent (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;
   bool ItsANewEvent;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   Grp_GetParamWhichGroups ();
   Event.EvtCod = ExaEvt_GetParamEvtCod ();
   ItsANewEvent = (Event.EvtCod <= 0);

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   if (Exam.CrsCod != Gbl.Hierarchy.Crs.CrsCod)
      Lay_WrongExamExit ();
   Exams.ExaCod = Exam.ExaCod;

   /***** Get event data *****/
   if (ItsANewEvent)
      /* Initialize to empty event */
      ExaEvt_ResetEvent (&Event);
   else
     {
      /* Get event data from database */
      ExaEvt_GetDataOfEventByCod (&Event);
      if (Exam.ExaCod != Event.ExaCod)
	 Lay_WrongExamExit ();
      Exams.EvtCod = Event.EvtCod;
     }

   /***** Show exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Event,
                        true);	// Put form for event
  }

/*****************************************************************************/
/******************* Create a new exam event (by a teacher) ******************/
/*****************************************************************************/

void ExaEvt_ReceiveFormEvent (void)
  {
   extern const char *Txt_Created_new_event_X;
   extern const char *Txt_The_event_has_been_modified;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;
   bool ItsANewEvent;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);

   /***** Get main parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   Grp_GetParamWhichGroups ();
   Event.EvtCod = ExaEvt_GetParamEvtCod ();
   ItsANewEvent = (Event.EvtCod <= 0);

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   if (Exam.CrsCod != Gbl.Hierarchy.Crs.CrsCod)
      Lay_WrongExamExit ();
   Exams.ExaCod = Exam.ExaCod;

   /***** Get event data from database *****/
   if (ItsANewEvent)
     {
      /* Initialize to empty event */
      ExaEvt_ResetEvent (&Event);
      Event.ExaCod = Exam.ExaCod;
     }
   else
     {
      /* Get event data from database */
      ExaEvt_GetDataOfEventByCod (&Event);
      if (Event.ExaCod != Exam.ExaCod)
	 Lay_WrongExamExit ();
      Exams.EvtCod = Event.EvtCod;
     }

   /***** Get parameters from form *****/
   /* Get event title */
   Par_GetParToText ("Title",Event.Title,ExaEvt_MAX_BYTES_TITLE);

   /* Get start/end date-times */
   Event.TimeUTC[Dat_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   Event.TimeUTC[Dat_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /* Get groups associated to the event */
   Grp_GetParCodsSeveralGrps ();

   /***** Create/update event *****/
   if (ItsANewEvent)
      ExaEvt_CreateEvent (&Event);
   else
      ExaEvt_UpdateEvent (&Event);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();

   /***** Write success message *****/
   if (ItsANewEvent)
      Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_event_X,
		     Event.Title);
   else
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_event_has_been_modified);

   /***** Get exam data again to update it after changes in event *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Event,
	                false);	// Do not put form for event
  }

/*****************************************************************************/
/**************************** Create a new event *****************************/
/*****************************************************************************/

static void ExaEvt_CreateEvent (struct ExaEvt_Event *Event)
  {
   /***** Insert this new exam event into database *****/
   Event->EvtCod =
   DB_QueryINSERTandReturnCode ("can not create exam event",
				"INSERT exa_events "
				"(ExaCod,Hidden,UsrCod,StartTime,EndTime,Title,ShowUsrResults)"
				" VALUES "
				"(%ld,"			// ExaCod
                                "'%c',"			// Hidden
				"%ld,"			// UsrCod
                                "FROM_UNIXTIME(%ld),"	// Start time
                                "FROM_UNIXTIME(%ld),"	// End time
				"'%s',"			// Title
				"'N')",			// ShowUsrResults: Don't show user results initially
				Event->ExaCod,
				Event->Hidden ? 'Y' :
					        'N',
				Gbl.Usrs.Me.UsrDat.UsrCod,	// Event creator
				Event->TimeUTC[Dat_START_TIME],	// Start time
				Event->TimeUTC[Dat_END_TIME  ],	// End time
				Event->Title);

   /***** Create indexes for answers *****/
   // ExaEvt_CreateIndexes (ExaCod,EvtCod);

   /***** Create groups associated to the exam event *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      ExaEvt_CreateGrps (Event->EvtCod);
  }

/*****************************************************************************/
/************************* Update an existing event **************************/
/*****************************************************************************/

static void ExaEvt_UpdateEvent (struct ExaEvt_Event *Event)
  {
   /***** Insert this new exam event into database *****/
   DB_QueryUPDATE ("can not update exam event",
		   "UPDATE exa_events,exa_exams"
		   " SET exa_events.StartTime=FROM_UNIXTIME(%ld),"
                        "exa_events.EndTime=FROM_UNIXTIME(%ld),"
                        "exa_events.Title='%s',"
			"exa_events.Hidden='%c'"
		   " WHERE exa_events.EvtCod=%ld"
		   " AND exa_events.ExaCod=exa_exams.ExaCod"
		   " AND exa_exams.CrsCod=%ld",		// Extra check
		   Event->TimeUTC[Dat_START_TIME],	// Start time
		   Event->TimeUTC[Dat_END_TIME  ],	// End time
		   Event->Title,
		   Event->Hidden ? 'Y' :
			           'N',
		   Event->EvtCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Update groups associated to the exam event *****/
   ExaEvt_RemoveGroups (Event->EvtCod);		// Remove all groups associated to this event
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      ExaEvt_CreateGrps (Event->EvtCod);	// Associate new groups
  }

/*****************************************************************************/
/**************** Create groups associated to an exam event ******************/
/*****************************************************************************/

static void ExaEvt_CreateGrps (long EvtCod)
  {
   unsigned NumGrpSel;

   /***** Create groups associated to the exam event *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
      /* Create group */
      DB_QueryINSERT ("can not associate a group to an exam event",
		      "INSERT INTO exa_groups"
		      " (EvtCod,GrpCod)"
		      " VALUES"
		      " (%ld,%ld)",
                      EvtCod,Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
  }

/*****************************************************************************/
/********************* Remove all groups from one event **********************/
/*****************************************************************************/

static void ExaEvt_RemoveGroups (long EvtCod)
  {
   /***** Remove all groups from one event *****/
   DB_QueryDELETE ("can not remove groups associated to and event",
		   "DELETE FROM exa_groups WHERE EvtCod=%ld",
		   EvtCod);
  }

/*****************************************************************************/
/********************* Remove one group from all events **********************/
/*****************************************************************************/

void ExaEvt_RemoveGroup (long GrpCod)
  {
   /***** Remove group from all the events *****/
   DB_QueryDELETE ("can not remove group"
	           " from the associations between events and groups",
		   "DELETE FROM exa_groups WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/***************** Remove groups of one type from all events *****************/
/*****************************************************************************/

void ExaEvt_RemoveGroupsOfType (long GrpTypCod)
  {
   /***** Remove group from all the events *****/
   DB_QueryDELETE ("can not remove groups of a type"
	           " from the associations between events and groups",
		   "DELETE FROM exa_groups"
		   " USING crs_grp,exa_groups"
		   " WHERE crs_grp.GrpTypCod=%ld"
		   " AND crs_grp.GrpCod=exa_groups.GrpCod",
                   GrpTypCod);
  }

/*****************************************************************************/
/********************** Get number of events in an exam **********************/
/*****************************************************************************/

unsigned ExaEvt_GetNumEventsInExam (long ExaCod)
  {
   /***** Trivial check *****/
   if (ExaCod < 0)	// A non-existing exam...
      return 0;		// ...has no events

   /***** Get number of events in an exam from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of events of an exam",
			     "SELECT COUNT(*) FROM exa_events"
			     " WHERE ExaCod=%ld",
			     ExaCod);
  }

/*****************************************************************************/
/****************** Get number of open events in an exam *********************/
/*****************************************************************************/

unsigned ExaEvt_GetNumOpenEventsInExam (long ExaCod)
  {
   /***** Trivial check *****/
   if (ExaCod < 0)	// A non-existing exam...
      return 0;		// ...has no events

   /***** Get number of open events in an exam from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of open events of an exam",
			     "SELECT COUNT(*) FROM exa_events"
			     " WHERE ExaCod=%ld"
                             " AND NOW() BETWEEN StartTime AND EndTime",
			     ExaCod);
  }

/*****************************************************************************/
/********* Check if I belong to any of the groups of an exam event ***********/
/*****************************************************************************/

bool ExaEvt_CheckIfICanPlayThisEventBasedOnGrps (const struct ExaEvt_Event *Event)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 /***** Check if I belong to any of the groups
	        associated to the exam event *****/
	 return (DB_QueryCOUNT ("can not check if I can play an exam event",
				"SELECT COUNT(*) FROM exa_events"
				" WHERE EvtCod=%ld"
				" AND"
				"(EvtCod NOT IN"
				" (SELECT EvtCod FROM exa_groups)"
				" OR"
				" EvtCod IN"
				" (SELECT exa_groups.EvtCod"
				" FROM exa_groups,crs_grp_usr"
				" WHERE crs_grp_usr.UsrCod=%ld"
				" AND exa_groups.GrpCod=crs_grp_usr.GrpCod))",
				Event->EvtCod,Gbl.Usrs.Me.UsrDat.UsrCod) != 0);
	 break;
      case Rol_NET:
	 /***** Only if I am the creator *****/
	 return (Event->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/**** Receive previous question answer in an exam event question from database *****/
/*****************************************************************************/

void ExaEvt_GetQstAnsFromDB (long EvtCod,long UsrCod,unsigned QstInd,
		             struct ExaEvt_UsrAnswer *UsrAnswer)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;

   /***** Set default values for number of option and answer index *****/
   UsrAnswer->NumOpt = -1;	// < 0 ==> no answer selected
   UsrAnswer->AnsInd = -1;	// < 0 ==> no answer selected

   /***** Get student's answer *****/
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get user's answer to an exam event question",
					"SELECT NumOpt,"	// row[0]
					       "AnsInd"		// row[1]
					" FROM exa_answers"
					" WHERE EvtCod=%ld"
					" AND UsrCod=%ld"
					" AND QstInd=%u",
					EvtCod,UsrCod,QstInd);
   if (NumRows) // Answer found...
     {
      row = mysql_fetch_row (mysql_res);

      /***** Get number of option index (row[0]) *****/
      if (sscanf (row[0],"%d",&(UsrAnswer->NumOpt)) != 1)
	 Lay_ShowErrorAndExit ("Error when getting student's answer to an exam event question.");

      /***** Get answer index (row[1]) *****/
      if (sscanf (row[1],"%d",&(UsrAnswer->AnsInd)) != 1)
	 Lay_ShowErrorAndExit ("Error when getting student's answer to an exam event question.");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
