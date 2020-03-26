// swad_attendance.c: control of attendance

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
/********************************** Headers **********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <mysql/mysql.h>	// To access MySQL databases
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_attendance.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_QR.h"
#include "swad_setting.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

#define Att_ATTENDANCE_TABLE_ID		"att_table"
#define Att_ATTENDANCE_DETAILS_ID	"att_details"

/*****************************************************************************/
/******************************** Private types ******************************/
/*****************************************************************************/

typedef enum
  {
   Att_VIEW_ONLY_ME,	// View only me
   Att_VIEW_SEL_USR,	// View selected users
   Att_PRNT_ONLY_ME,	// Print only me
   Att_PRNT_SEL_USR,	// Print selected users
  } Att_TypeOfView_t;

/*****************************************************************************/
/****************************** Private variables ****************************/
/*****************************************************************************/

Att_TypeOfView_t Att_TypeOfView;

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static void Att_ShowAllAttEvents (void);
static void Att_ParamsWhichGroupsToShow (void *Args);
static void Att_PutIconsInListOfAttEvents (void *Args);
static void Att_PutIconToCreateNewAttEvent (void);
static void Att_PutButtonToCreateNewAttEvent (void);
static void Att_PutParamsToCreateNewAttEvent (void *Args);
static void Att_PutParamsToListUsrsAttendance (void *Args);

static void Att_ShowOneAttEvent (struct AttendanceEvent *Att,bool ShowOnlyThisAttEventComplete);
static void Att_WriteAttEventAuthor (struct AttendanceEvent *Att);
static void Att_GetParamAttOrder (void);

static void Att_PutFormsToRemEditOneAttEvent (const struct AttendanceEvent *Att,
                                              const char *Anchor);
static void Att_PutParams (void *Args);
static void Att_GetListAttEvents (Att_OrderNewestOldest_t OrderNewestOldest);
static void Att_GetDataOfAttEventByCodAndCheckCrs (struct AttendanceEvent *Att);
static void Att_ResetAttendanceEvent (struct AttendanceEvent *Att);
static void Att_GetAttEventDescriptionFromDB (long AttCod,char Description[Cns_MAX_BYTES_TEXT + 1]);
static bool Att_CheckIfSimilarAttEventExists (const char *Field,const char *Value,long AttCod);
static void Att_ShowLstGrpsToEditAttEvent (long AttCod);
static void Att_RemoveAllTheGrpsAssociatedToAnAttEvent (long AttCod);
static void Att_CreateGrps (long AttCod);
static void Att_GetAndWriteNamesOfGrpsAssociatedToAttEvent (struct AttendanceEvent *Att);

static void Att_RemoveAllUsrsFromAnAttEvent (long AttCod);
static void Att_RemoveAttEventFromCurrentCrs (long AttCod);

static void Att_ListAttOnlyMeAsStudent (struct AttendanceEvent *Att);
static void Att_ListAttStudents (struct AttendanceEvent *Att);
static void Att_WriteRowUsrToCallTheRoll (unsigned NumUsr,
                                          struct UsrData *UsrDat,
                                          struct AttendanceEvent *Att);
static void Att_PutLinkAttEvent (struct AttendanceEvent *AttEvent,
				 const char *Title,const char *Txt,
				 const char *Class);
static void Att_PutParamsCodGrps (long AttCod);
static void Att_GetNumStdsTotalWhoAreInAttEvent (struct AttendanceEvent *Att);
static unsigned Att_GetNumUsrsFromAListWhoAreInAttEvent (long AttCod,
							 long LstSelectedUsrCods[],
							 unsigned NumUsrsInList);
static bool Att_CheckIfUsrIsInTableAttUsr (long AttCod,long UsrCod,bool *Present);
static bool Att_CheckIfUsrIsPresentInAttEvent (long AttCod,long UsrCod);
static bool Att_CheckIfUsrIsPresentInAttEventAndGetComments (long AttCod,long UsrCod,
                                                             char CommentStd[Cns_MAX_BYTES_TEXT + 1],
                                                             char CommentTch[Cns_MAX_BYTES_TEXT + 1]);
static void Att_RegUsrInAttEventChangingComments (long AttCod,long UsrCod,bool Present,
                                                  const char *CommentStd,const char *CommentTch);
static void Att_RemoveUsrFromAttEvent (long AttCod,long UsrCod);

static void Att_ListOrPrintMyAttendanceCrs (void);
static void Att_GetUsrsAndListOrPrintAttendanceCrs (void);
static void Att_ListOrPrintUsrsAttendanceCrs (void);

static void Att_GetListSelectedAttCods (char **StrAttCodsSelected);

static void Att_PutIconsMyAttList (void *Args);
static void Att_PutFormToPrintMyListParams (void *Args);
static void Att_PutIconsStdsAttList (void *Args);
static void Att_PutParamsToPrintStdsList (void *Args);

static void Att_PutButtonToShowDetails (void);
static void Att_ListEventsToSelect (void);
static void Att_PutIconToViewAttEvents (void *Args);
static void Att_PutIconToEditAttEvents (void *Args);
static void Att_ListUsrsAttendanceTable (unsigned NumUsrsInList,
                                         long *LstSelectedUsrCods);
static void Att_WriteTableHeadSeveralAttEvents (void);
static void Att_WriteRowUsrSeveralAttEvents (unsigned NumUsr,struct UsrData *UsrDat);
static void Att_PutCheckOrCross (bool Present);
static void Att_ListStdsWithAttEventsDetails (unsigned NumUsrsInList,
                                              long *LstSelectedUsrCods);
static void Att_ListAttEventsForAStd (unsigned NumUsr,struct UsrData *UsrDat);

/*****************************************************************************/
/********************** List all the attendance events ***********************/
/*****************************************************************************/

void Att_SeeAttEvents (void)
  {
   /***** Get parameters *****/
   Att_GetParamAttOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.AttEvents.CurrentPage = Pag_GetParamPagNum (Pag_ATT_EVENTS);

   /***** Get list of attendance events *****/
   Att_GetListAttEvents (Att_NEWEST_FIRST);

   /***** Show all the attendance events *****/
   Att_ShowAllAttEvents ();
  }

/*****************************************************************************/
/********************** Show all the attendance events ***********************/
/*****************************************************************************/

static void Att_ShowAllAttEvents (void)
  {
   extern const char *Hlp_USERS_Attendance;
   extern const char *Txt_Events;
   extern const char *Txt_START_END_TIME_HELP[Dat_NUM_START_END_TIME];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Event;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_No_events;
   Dat_StartEndTime_t Order;
   struct Pagination Pagination;
   unsigned NumAttEvent;
   bool ICanEdit = (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
		    Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Gbl.AttEvents.Num;
   Pagination.CurrentPage = (int) Gbl.AttEvents.CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Gbl.AttEvents.CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Begin box *****/
   Box_BoxBegin ("100%",Txt_Events,
                 Att_PutIconsInListOfAttEvents,(void *) &Gbl,
		 Hlp_USERS_Attendance,Box_NOT_CLOSABLE);

   /***** Select whether show only my groups or all groups *****/
   if (Gbl.Crs.Grps.NumGrps)
     {
      Set_StartSettingsHead ();
      Grp_ShowFormToSelWhichGrps (ActSeeAtt,
                                  Att_ParamsWhichGroupsToShow,(void *) &Gbl);
      Set_EndSettingsHead ();
     }

   /***** Write links to pages *****/
   Pag_WriteLinksToPagesCentered (Pag_ATT_EVENTS,
				  &Pagination,
				  0);

   if (Gbl.AttEvents.Num)
     {
      /***** Table head *****/
      HTM_TABLE_BeginWideMarginPadding (2);
      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"CONTEXT_COL",NULL);	// Column for contextual icons
      for (Order = Dat_START_TIME;
	   Order <= Dat_END_TIME;
	   Order++)
	{
	 HTM_TH_Begin (1,1,"LM");

	 Frm_StartForm (ActSeeAtt);
	 Grp_PutParamWhichGrps ((void *) Grp_GetParamWhichGrps ());
	 Pag_PutHiddenParamPagNum (Pag_ATT_EVENTS,Gbl.AttEvents.CurrentPage);
	 Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
	 HTM_BUTTON_SUBMIT_Begin (Txt_START_END_TIME_HELP[Order],"BT_LINK TIT_TBL",NULL);
	 if (Order == Gbl.AttEvents.SelectedOrder)
	    HTM_U_Begin ();
	 HTM_Txt (Txt_START_END_TIME[Order]);
	 if (Order == Gbl.AttEvents.SelectedOrder)
	    HTM_U_End ();
	 HTM_BUTTON_End ();
	 Frm_EndForm ();

	 HTM_TH_End ();
	}
      HTM_TH (1,1,"LM",Txt_Event);
      HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_Abc[Rol_STD][Usr_SEX_UNKNOWN]);

      HTM_TR_End ();

      /***** Write all the attendance events *****/
      for (NumAttEvent = Pagination.FirstItemVisible, Gbl.RowEvenOdd = 0;
	   NumAttEvent <= Pagination.LastItemVisible;
	   NumAttEvent++)
	 Att_ShowOneAttEvent (&Gbl.AttEvents.Lst[NumAttEvent - 1],false);

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No events created
      Ale_ShowAlert (Ale_INFO,Txt_No_events);

   /***** Write again links to pages *****/
   Pag_WriteLinksToPagesCentered (Pag_ATT_EVENTS,
				  &Pagination,
				  0);

   /***** Button to create a new attendance event *****/
   if (ICanEdit)
      Att_PutButtonToCreateNewAttEvent ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of attendance events *****/
   Att_FreeListAttEvents ();
  }

/*****************************************************************************/
/***************** Put params to select which groups to show *****************/
/*****************************************************************************/

static void Att_ParamsWhichGroupsToShow (void *Args)
  {
   if (Args)
     {
      Att_PutHiddenParamAttOrder ();
      Pag_PutHiddenParamPagNum (Pag_ATT_EVENTS,Gbl.AttEvents.CurrentPage);
     }
  }

/*****************************************************************************/
/************* Put contextual icons in list of attendance events *************/
/*****************************************************************************/

static void Att_PutIconsInListOfAttEvents (void *Args)
  {
   bool ICanEdit;

   if (Args)
     {
      /***** Put icon to create a new attendance event *****/
      ICanEdit = (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
		  Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
      if (ICanEdit)
	 Att_PutIconToCreateNewAttEvent ();

      /***** Put icon to show attendance list *****/
      if (Gbl.AttEvents.Num)
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_STD:
	       Ico_PutContextualIconToShowAttendanceList (ActSeeLstMyAtt,
	                                                  NULL,NULL);
	       break;
	    case Rol_NET:
	    case Rol_TCH:
	    case Rol_SYS_ADM:
	       Ico_PutContextualIconToShowAttendanceList (ActReqLstUsrAtt,
							  Att_PutParamsToListUsrsAttendance,(void *) &Gbl);
	       break;
	    default:
	       break;
	   }

      /***** Put icon to print my QR code *****/
      QR_PutLinkToPrintQRCode (ActPrnUsrQR,
			       Usr_PutParamMyUsrCodEncrypted,(void *) Gbl.Usrs.Me.UsrDat.EncryptedUsrCod);
     }
  }

/*****************************************************************************/
/**************** Put icon to create a new attendance event ******************/
/*****************************************************************************/

static void Att_PutIconToCreateNewAttEvent (void)
  {
   extern const char *Txt_New_event;

   /***** Put icon to create a new attendance event *****/
   Ico_PutContextualIconToAdd (ActFrmNewAtt,NULL,
			       Att_PutParamsToCreateNewAttEvent,(void *) &Gbl,
			       Txt_New_event);
  }

/*****************************************************************************/
/**************** Put button to create a new attendance event ****************/
/*****************************************************************************/

static void Att_PutButtonToCreateNewAttEvent (void)
  {
   extern const char *Txt_New_event;

   Frm_StartForm (ActFrmNewAtt);
   Att_PutParamsToCreateNewAttEvent ((void *) &Gbl);
   Btn_PutConfirmButton (Txt_New_event);
   Frm_EndForm ();
  }

/*****************************************************************************/
/************** Put parameters to create a new attendance event **************/
/*****************************************************************************/

static void Att_PutParamsToCreateNewAttEvent (void *Args)
  {
   if (Args)
     {
      Att_PutHiddenParamAttOrder ();
      Grp_PutParamWhichGrps ((void *) Grp_GetParamWhichGrps ());
      Pag_PutHiddenParamPagNum (Pag_ATT_EVENTS,Gbl.AttEvents.CurrentPage);
     }
  }

/*****************************************************************************/
/***************** Put parameters to list users attendance *******************/
/*****************************************************************************/

static void Att_PutParamsToListUsrsAttendance (void *Args)
  {
   if (Args)
     {
      Att_PutHiddenParamAttOrder ();
      Grp_PutParamWhichGrps ((void *) Grp_GetParamWhichGrps ());
      Pag_PutHiddenParamPagNum (Pag_ATT_EVENTS,Gbl.AttEvents.CurrentPage);
     }
  }

/*****************************************************************************/
/************************* Show one attendance event *************************/
/*****************************************************************************/
// Only Att->AttCod must be filled

static void Att_ShowOneAttEvent (struct AttendanceEvent *Att,bool ShowOnlyThisAttEventComplete)
  {
   extern const char *Txt_View_event;
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   char *Id;
   Dat_StartEndTime_t StartEndTime;
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Get data of this attendance event *****/
   Att_GetDataOfAttEventByCodAndCheckCrs (Att);
   Att_GetNumStdsTotalWhoAreInAttEvent (Att);

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Att->AttCod,&Anchor);

   /***** Write first row of data of this attendance event *****/
   /* Forms to remove/edit this attendance event */
   HTM_TR_Begin (NULL);

   if (ShowOnlyThisAttEventComplete)
      HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL\"");
   else
      HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL COLOR%u\"",Gbl.RowEvenOdd);
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
         Att_PutFormsToRemEditOneAttEvent (Att,Anchor);
	 break;
      default:
         break;
     }
   HTM_TD_End ();

   /* Start/end date/time */
   UniqueId++;
   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      if (asprintf (&Id,"att_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      if (ShowOnlyThisAttEventComplete)
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LB\"",
		       Id,
		       Att->Hidden ? (Att->Open ? "DATE_GREEN_LIGHT" :
						  "DATE_RED_LIGHT") :
				     (Att->Open ? "DATE_GREEN" :
						  "DATE_RED"));
      else
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LB COLOR%u\"",
		       Id,
		       Att->Hidden ? (Att->Open ? "DATE_GREEN_LIGHT" :
						  "DATE_RED_LIGHT") :
				     (Att->Open ? "DATE_GREEN" :
						  "DATE_RED"),
		       Gbl.RowEvenOdd);
      Dat_WriteLocalDateHMSFromUTC (Id,Att->TimeUTC[StartEndTime],
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				    true,true,true,0x7);
      HTM_TD_End ();
      free (Id);
     }

   /* Attendance event title */
   if (ShowOnlyThisAttEventComplete)
      HTM_TD_Begin ("class=\"LT\"");
   else
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   HTM_ARTICLE_Begin (Anchor);
   Att_PutLinkAttEvent (Att,Txt_View_event,Att->Title,
	                Att->Hidden ? "BT_LINK LT ASG_TITLE_LIGHT" :
	                              "BT_LINK LT ASG_TITLE");
   HTM_ARTICLE_End ();
   HTM_TD_End ();

   /* Number of students in this event */
   if (ShowOnlyThisAttEventComplete)
      HTM_TD_Begin ("class=\"%s RT\"",
		    Att->Hidden ? "ASG_TITLE_LIGHT" :
				  "ASG_TITLE");
   else
      HTM_TD_Begin ("class=\"%s RT COLOR%u\"",
		    Att->Hidden ? "ASG_TITLE_LIGHT" :
				  "ASG_TITLE",
		    Gbl.RowEvenOdd);
   HTM_Unsigned (Att->NumStdsTotal);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Write second row of data of this attendance event *****/
   HTM_TR_Begin (NULL);

   /* Author of the attendance event */
   if (ShowOnlyThisAttEventComplete)
      HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
   else
      HTM_TD_Begin ("colspan=\"2\" class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   Att_WriteAttEventAuthor (Att);
   HTM_TD_End ();

   /* Text of the attendance event */
   Att_GetAttEventDescriptionFromDB (Att->AttCod,Description);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Description,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
   Str_InsertLinks (Description,Cns_MAX_BYTES_TEXT,60);	// Insert links
   if (ShowOnlyThisAttEventComplete)
      HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
   else
      HTM_TD_Begin ("colspan=\"2\" class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   if (Gbl.Crs.Grps.NumGrps)
      Att_GetAndWriteNamesOfGrpsAssociatedToAttEvent (Att);
   HTM_DIV_Begin ("class=\"%s\"",Att->Hidden ? "DAT_LIGHT" :
        	                               "DAT");
   HTM_Txt (Description);
   HTM_DIV_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/****************** Write the author of an attendance event ******************/
/*****************************************************************************/

static void Att_WriteAttEventAuthor (struct AttendanceEvent *Att)
  {
   Usr_WriteAuthor1Line (Att->UsrCod,Att->Hidden);
  }

/*****************************************************************************/
/**** Get parameter with the type or order in list of attendance events ******/
/*****************************************************************************/

static void Att_GetParamAttOrder (void)
  {
   Gbl.AttEvents.SelectedOrder = (Dat_StartEndTime_t)
	                         Par_GetParToUnsignedLong ("Order",
                                                           0,
                                                           Dat_NUM_START_END_TIME - 1,
                                                           (unsigned long) Att_ORDER_DEFAULT);
  }

/*****************************************************************************/
/*** Put a hidden parameter with the type of order in list of att. events ****/
/*****************************************************************************/

void Att_PutHiddenParamAttOrder (void)
  {
   Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Gbl.AttEvents.SelectedOrder);
  }

/*****************************************************************************/
/************** Put a link (form) to edit one attendance event ***************/
/*****************************************************************************/

static void Att_PutFormsToRemEditOneAttEvent (const struct AttendanceEvent *Att,
                                              const char *Anchor)
  {
   Gbl.AttEvents.AttCod = Att->AttCod;	// Used as parameters in contextual links

   /***** Put form to remove attendance event *****/
   Ico_PutContextualIconToRemove (ActReqRemAtt,
                                  Att_PutParams,(void *) &Gbl);

   /***** Put form to hide/show attendance event *****/
   if (Att->Hidden)
      Ico_PutContextualIconToUnhide (ActShoAtt,Anchor,
                                     Att_PutParams,(void *) &Gbl);
   else
      Ico_PutContextualIconToHide (ActHidAtt,Anchor,
                                   Att_PutParams,(void *) &Gbl);

   /***** Put form to edit attendance event *****/
   Ico_PutContextualIconToEdit (ActEdiOneAtt,NULL,
                                Att_PutParams,(void *) &Gbl);
  }

/*****************************************************************************/
/***************** Params used to edit an attendance event *******************/
/*****************************************************************************/

static void Att_PutParams (void *Args)
  {
   if (Args)
     {
      Att_PutParamAttCod (Gbl.AttEvents.AttCod);
      Att_PutHiddenParamAttOrder ();
      Grp_PutParamWhichGrps ((void *) Grp_GetParamWhichGrps ());
      Pag_PutHiddenParamPagNum (Pag_ATT_EVENTS,Gbl.AttEvents.CurrentPage);
     }
  }

/*****************************************************************************/
/********************* List all the attendance events ************************/
/*****************************************************************************/

static void Att_GetListAttEvents (Att_OrderNewestOldest_t OrderNewestOldest)
  {
   static const char *HiddenSubQuery[Rol_NUM_ROLES] =
     {
      [Rol_UNK    ] = " AND Hidden='N'",
      [Rol_GST    ] = " AND Hidden='N'",
      [Rol_USR    ] = " AND Hidden='N'",
      [Rol_STD    ] = " AND Hidden='N'",
      [Rol_NET    ] = " AND Hidden='N'",
      [Rol_TCH    ] = "",
      [Rol_DEG_ADM] = " AND Hidden='N'",
      [Rol_CTR_ADM] = " AND Hidden='N'",
      [Rol_INS_ADM] = " AND Hidden='N'",
      [Rol_SYS_ADM] = "",
     };
   static const char *OrderBySubQuery[Dat_NUM_START_END_TIME][Att_NUM_ORDERS_NEWEST_OLDEST] =
     {
      [Dat_START_TIME][Att_NEWEST_FIRST] = "StartTime DESC,EndTime DESC,Title DESC",
      [Dat_START_TIME][Att_OLDEST_FIRST] = "StartTime,EndTime,Title",

      [Dat_END_TIME  ][Att_NEWEST_FIRST] = "EndTime DESC,StartTime DESC,Title DESC",
      [Dat_END_TIME  ][Att_OLDEST_FIRST] = "EndTime,StartTime,Title",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumAttEvent;

   if (Gbl.AttEvents.LstIsRead)
      Att_FreeListAttEvents ();

   /***** Get list of attendance events from database *****/
   if (Gbl.Crs.Grps.WhichGrps == Grp_MY_GROUPS)
      NumRows = DB_QuerySELECT (&mysql_res,"can not get attendance events",
				"SELECT AttCod"
				" FROM att_events"
				" WHERE CrsCod=%ld%s"
				" AND (AttCod NOT IN (SELECT AttCod FROM att_grp) OR"
				" AttCod IN (SELECT att_grp.AttCod FROM att_grp,crs_grp_usr"
				" WHERE crs_grp_usr.UsrCod=%ld"
				" AND att_grp.GrpCod=crs_grp_usr.GrpCod))"
				" ORDER BY %s",
				Gbl.Hierarchy.Crs.CrsCod,
				HiddenSubQuery[Gbl.Usrs.Me.Role.Logged],
				Gbl.Usrs.Me.UsrDat.UsrCod,
				OrderBySubQuery[Gbl.AttEvents.SelectedOrder][OrderNewestOldest]);
   else	// Gbl.Crs.Grps.WhichGrps == Grp_ALL_GROUPS
      NumRows = DB_QuerySELECT (&mysql_res,"can not get attendance events",
				"SELECT AttCod"
				" FROM att_events"
				" WHERE CrsCod=%ld%s"
				" ORDER BY %s",
				Gbl.Hierarchy.Crs.CrsCod,
				HiddenSubQuery[Gbl.Usrs.Me.Role.Logged],
				OrderBySubQuery[Gbl.AttEvents.SelectedOrder][OrderNewestOldest]);

   if (NumRows) // Attendance events found...
     {
      Gbl.AttEvents.Num = (unsigned) NumRows;

      /***** Create list of attendance events *****/
      if ((Gbl.AttEvents.Lst = (struct AttendanceEvent *) calloc (NumRows,sizeof (struct AttendanceEvent))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the attendance events codes *****/
      for (NumAttEvent = 0;
	   NumAttEvent < Gbl.AttEvents.Num;
	   NumAttEvent++)
        {
         /* Get next attendance event code */
         row = mysql_fetch_row (mysql_res);
         if ((Gbl.AttEvents.Lst[NumAttEvent].AttCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Error: wrong attendance event code.");
        }
     }
   else
      Gbl.AttEvents.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.AttEvents.LstIsRead = true;
  }

/*****************************************************************************/
/********* Get attendance event data using its code and check course *********/
/*****************************************************************************/

static void Att_GetDataOfAttEventByCodAndCheckCrs (struct AttendanceEvent *Att)
  {
   if (Att_GetDataOfAttEventByCod (Att))
     {
      if (Att->CrsCod != Gbl.Hierarchy.Crs.CrsCod)
         Lay_ShowErrorAndExit ("Attendance event does not belong to current course.");
     }
   else	// Attendance event not found
      Lay_ShowErrorAndExit ("Error when getting attendance event.");
  }

/*****************************************************************************/
/**************** Get attendance event data using its code *******************/
/*****************************************************************************/
// Returns true if attendance event exists
// This function can be called from web service, so do not display messages

bool Att_GetDataOfAttEventByCod (struct AttendanceEvent *Att)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   bool Found = false;

   /***** Reset attendance event data *****/
   Att_ResetAttendanceEvent (Att);

   if (Att->AttCod > 0)
     {
      /***** Build query *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get attendance event data",
	                        "SELECT AttCod,CrsCod,Hidden,UsrCod,"
				"UNIX_TIMESTAMP(StartTime),"
				"UNIX_TIMESTAMP(EndTime),"
				"NOW() BETWEEN StartTime AND EndTime,"
				"CommentTchVisible,"
				"Title"
				" FROM att_events"
				" WHERE AttCod=%ld",
				Att->AttCod);

      /***** Get data of attendance event from database *****/
      if ((Found = (NumRows != 0))) // Attendance event found...
	{
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get code of the attendance event (row[0]) */
	 Att->AttCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get code of the course (row[1]) */
	 Att->CrsCod = Str_ConvertStrCodToLongCod (row[1]);

	 /* Get whether the attendance event is hidden or not (row[2]) */
	 Att->Hidden = (row[2][0] == 'Y');

	 /* Get author of the attendance event (row[3]) */
	 Att->UsrCod = Str_ConvertStrCodToLongCod (row[3]);

	 /* Get start date (row[4] holds the start UTC time) */
	 Att->TimeUTC[Att_START_TIME] = Dat_GetUNIXTimeFromStr (row[4]);

	 /* Get end   date (row[5] holds the end   UTC time) */
	 Att->TimeUTC[Att_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[5]);

	 /* Get whether the attendance event is open or closed (row(6)) */
	 Att->Open = (row[6][0] == '1');

	 /* Get whether the attendance event is visible or not (row[7]) */
	 Att->CommentTchVisible = (row[7][0] == 'Y');

	 /* Get the title of the attendance event (row[8]) */
	 Str_Copy (Att->Title,row[8],
	           Att_MAX_BYTES_ATTENDANCE_EVENT_TITLE);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return Found;
  }

/*****************************************************************************/
/********************** Clear all attendance event data **********************/
/*****************************************************************************/

static void Att_ResetAttendanceEvent (struct AttendanceEvent *Att)
  {
   if (Att->AttCod <= 0)	// If > 0 ==> keep values of AttCod and Selected
     {
      Att->AttCod = -1L;
      Att->NumStdsTotal = 0;
      Att->NumStdsFromList = 0;
      Att->Selected = false;
     }
   Att->CrsCod = -1L;
   Att->Hidden = false;
   Att->UsrCod = -1L;
   Att->TimeUTC[Att_START_TIME] =
   Att->TimeUTC[Att_END_TIME  ] = (time_t) 0;
   Att->Open = false;
   Att->Title[0] = '\0';
   Att->CommentTchVisible = false;
  }

/*****************************************************************************/
/********************** Free list of attendance events ***********************/
/*****************************************************************************/

void Att_FreeListAttEvents (void)
  {
   if (Gbl.AttEvents.LstIsRead && Gbl.AttEvents.Lst)
     {
      /***** Free memory used by the list of attendance events *****/
      free (Gbl.AttEvents.Lst);
      Gbl.AttEvents.Lst = NULL;
      Gbl.AttEvents.Num = 0;
      Gbl.AttEvents.LstIsRead = false;
     }
  }

/*****************************************************************************/
/***************** Get attendance event text from database *******************/
/*****************************************************************************/

static void Att_GetAttEventDescriptionFromDB (long AttCod,char Description[Cns_MAX_BYTES_TEXT + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get text of attendance event from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get attendance event text",
			     "SELECT Txt FROM att_events"
			     " WHERE AttCod=%ld AND CrsCod=%ld",
			     AttCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** The result of the query must have one row or none *****/
   if (NumRows == 1)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get info text */
      Str_Copy (Description,row[0],
                Cns_MAX_BYTES_TEXT);
     }
   else
      Description[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumRows > 1)
      Lay_ShowErrorAndExit ("Error when getting attendance event text.");
  }

/*****************************************************************************/
/************** Write parameter with code of attendance event ****************/
/*****************************************************************************/

void Att_PutParamSelectedAttCod (void *Args)
  {
   if (Args)
      Att_PutParamAttCod (Gbl.AttEvents.AttCod);
  }

void Att_PutParamAttCod (long AttCod)
  {
   Par_PutHiddenParamLong (NULL,"AttCod",AttCod);
  }

/*****************************************************************************/
/*************** Get parameter with code of attendance event *****************/
/*****************************************************************************/

long Att_GetParamAttCod (void)
  {
   /***** Get code of attendance event *****/
   return Par_GetParToLong ("AttCod");
  }

/*****************************************************************************/
/********* Ask for confirmation of removing of an attendance event ***********/
/*****************************************************************************/

void Att_AskRemAttEvent (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_event_X;
   extern const char *Txt_Remove_event;
   struct AttendanceEvent Att;

   /***** Get parameters *****/
   Att_GetParamAttOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.AttEvents.CurrentPage = Pag_GetParamPagNum (Pag_ATT_EVENTS);

   /***** Get attendance event code *****/
   if ((Att.AttCod = Att_GetParamAttCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of attendance event is missing.");

   /***** Get data of the attendance event from database *****/
   Att_GetDataOfAttEventByCodAndCheckCrs (&Att);

   /***** Button of confirmation of removing *****/
   Frm_StartForm (ActRemAtt);
   Att_PutParamAttCod (Att.AttCod);
   Att_PutHiddenParamAttOrder ();
   Grp_PutParamWhichGrps ((void *) Grp_GetParamWhichGrps ());
   Pag_PutHiddenParamPagNum (Pag_ATT_EVENTS,Gbl.AttEvents.CurrentPage);

   /* Ask for confirmation of removing */
   Ale_ShowAlert (Ale_WARNING,Txt_Do_you_really_want_to_remove_the_event_X,
                  Att.Title);

   Btn_PutRemoveButton (Txt_Remove_event);
   Frm_EndForm ();

   /***** Show attendance events again *****/
   Att_SeeAttEvents ();
  }

/*****************************************************************************/
/** Get param., remove an attendance event and show attendance events again **/
/*****************************************************************************/

void Att_GetAndRemAttEvent (void)
  {
   extern const char *Txt_Event_X_removed;
   struct AttendanceEvent Att;

   /***** Get attendance event code *****/
   if ((Att.AttCod = Att_GetParamAttCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of attendance event is missing.");

   /***** Get data of the attendance event from database *****/
   // Inside this function, the course is checked to be the current one
   Att_GetDataOfAttEventByCodAndCheckCrs (&Att);

   /***** Remove the attendance event from database *****/
   Att_RemoveAttEventFromDB (Att.AttCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Event_X_removed,
	          Att.Title);

   /***** Show attendance events again *****/
   Att_SeeAttEvents ();
  }

/*****************************************************************************/
/**************** Remove an attendance event from database *******************/
/*****************************************************************************/

void Att_RemoveAttEventFromDB (long AttCod)
  {
   /***** Remove users registered in the attendance event *****/
   Att_RemoveAllUsrsFromAnAttEvent (AttCod);

   /***** Remove all the groups of this attendance event *****/
   Att_RemoveAllTheGrpsAssociatedToAnAttEvent (AttCod);

   /***** Remove attendance event *****/
   Att_RemoveAttEventFromCurrentCrs (AttCod);
  }

/*****************************************************************************/
/************************* Hide an attendance event **************************/
/*****************************************************************************/

void Att_HideAttEvent (void)
  {
   struct AttendanceEvent Att;

   /***** Get attendance event code *****/
   if ((Att.AttCod = Att_GetParamAttCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of attendance event is missing.");

   /***** Get data of the attendance event from database *****/
   Att_GetDataOfAttEventByCodAndCheckCrs (&Att);

   /***** Hide attendance event *****/
   DB_QueryUPDATE ("can not hide attendance event",
		   "UPDATE att_events SET Hidden='Y'"
		   " WHERE AttCod=%ld AND CrsCod=%ld",
                   Att.AttCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Show attendance events again *****/
   Att_SeeAttEvents ();
  }

/*****************************************************************************/
/************************* Show an attendance event **************************/
/*****************************************************************************/

void Att_ShowAttEvent (void)
  {
   struct AttendanceEvent Att;

   /***** Get attendance event code *****/
   if ((Att.AttCod = Att_GetParamAttCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of attendance event is missing.");

   /***** Get data of the attendance event from database *****/
   Att_GetDataOfAttEventByCodAndCheckCrs (&Att);

   /***** Hide attendance event *****/
   DB_QueryUPDATE ("can not show attendance event",
		   "UPDATE att_events SET Hidden='N'"
		   " WHERE AttCod=%ld AND CrsCod=%ld",
                   Att.AttCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Show attendance events again *****/
   Att_SeeAttEvents ();
  }

/*****************************************************************************/
/***** Check if the title or the folder of an attendance event exists ********/
/*****************************************************************************/

static bool Att_CheckIfSimilarAttEventExists (const char *Field,const char *Value,long AttCod)
  {
   /***** Get number of attendance events
          with a field value from database *****/
   return (DB_QueryCOUNT ("can not get similar attendance events",
			  "SELECT COUNT(*) FROM att_events"
			  " WHERE CrsCod=%ld"
			  " AND %s='%s' AND AttCod<>%ld",
			  Gbl.Hierarchy.Crs.CrsCod,
			  Field,Value,AttCod) != 0);
  }

/*****************************************************************************/
/*************** Put a form to create a new attendance event *****************/
/*****************************************************************************/

void Att_RequestCreatOrEditAttEvent (void)
  {
   extern const char *Hlp_USERS_Attendance_new_event;
   extern const char *Hlp_USERS_Attendance_edit_event;
   extern const char *Txt_New_event;
   extern const char *Txt_Edit_event;
   extern const char *Txt_Teachers_comment;
   extern const char *Txt_Title;
   extern const char *Txt_Hidden_MALE_PLURAL;
   extern const char *Txt_Visible_MALE_PLURAL;
   extern const char *Txt_Description;
   extern const char *Txt_Create_event;
   extern const char *Txt_Save_changes;
   struct AttendanceEvent Att;
   bool ItsANewAttEvent;
   char Description[Cns_MAX_BYTES_TEXT + 1];
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      Dat_HMS_DO_NOT_SET,
      Dat_HMS_DO_NOT_SET
     };

   /***** Get parameters *****/
   Att_GetParamAttOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.AttEvents.CurrentPage = Pag_GetParamPagNum (Pag_ATT_EVENTS);

   /***** Get the code of the attendance event *****/
   Att.AttCod = Att_GetParamAttCod ();
   ItsANewAttEvent = (Att.AttCod <= 0);

   /***** Get from the database the data of the attendance event *****/
   if (ItsANewAttEvent)
     {
      /* Reset attendance event data */
      Att.AttCod = -1L;
      Att_ResetAttendanceEvent (&Att);

      /* Initialize some fields */
      Att.CrsCod = Gbl.Hierarchy.Crs.CrsCod;
      Att.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
      Att.TimeUTC[Att_START_TIME] = Gbl.StartExecutionTimeUTC;
      Att.TimeUTC[Att_END_TIME  ] = Gbl.StartExecutionTimeUTC + (2 * 60 * 60);	// +2 hours
      Att.Open = true;
     }
   else
     {
      /* Get data of the attendance event from database */
      Att_GetDataOfAttEventByCodAndCheckCrs (&Att);

      /* Get text of the attendance event from database */
      Att_GetAttEventDescriptionFromDB (Att.AttCod,Description);
     }

   /***** Begin form *****/
   if (ItsANewAttEvent)
      Frm_StartForm (ActNewAtt);
   else
     {
      Frm_StartForm (ActChgAtt);
      Att_PutParamAttCod (Att.AttCod);
     }
   Att_PutHiddenParamAttOrder ();
   Grp_PutParamWhichGrps ((void *) Grp_GetParamWhichGrps ());
   Pag_PutHiddenParamPagNum (Pag_ATT_EVENTS,Gbl.AttEvents.CurrentPage);

   /***** Begin box and table *****/
   if (ItsANewAttEvent)
      Box_BoxTableBegin (NULL,Txt_New_event,
                         NULL,NULL,
			 Hlp_USERS_Attendance_new_event,Box_NOT_CLOSABLE,2);
   else
      Box_BoxTableBegin (NULL,
                         Att.Title[0] ? Att.Title :
                	                Txt_Edit_event,
                	 NULL,NULL,
			 Hlp_USERS_Attendance_edit_event,Box_NOT_CLOSABLE,2);

   /***** Attendance event title *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","Title",Txt_Title);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_INPUT_TEXT ("Title",Att_MAX_CHARS_ATTENDANCE_EVENT_TITLE,Att.Title,false,
		   "id=\"Title\" required=\"required\""
		   " class=\"TITLE_DESCRIPTION_WIDTH\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Assignment start and end dates *****/
   Dat_PutFormStartEndClientLocalDateTimes (Att.TimeUTC,
					    Dat_FORM_SECONDS_ON,
					    SetHMS);

   /***** Visibility of comments *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","ComTchVisible",Txt_Teachers_comment);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_SELECT_Begin (false,"id=\"ComTchVisible\" name=\"ComTchVisible\"");
   HTM_OPTION (HTM_Type_STRING,"N",!Att.CommentTchVisible,false,
	       "%s",Txt_Hidden_MALE_PLURAL);
   HTM_OPTION (HTM_Type_STRING,"Y",Att.CommentTchVisible,false,
	       "%s",Txt_Visible_MALE_PLURAL);
   HTM_SELECT_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Attendance event description *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","Txt",Txt_Description);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_TEXTAREA_Begin ("id=\"Txt\" name=\"Txt\" rows=\"5\""
	               " class=\"TITLE_DESCRIPTION_WIDTH\"");
   if (!ItsANewAttEvent)
      HTM_Txt (Description);
   HTM_TEXTAREA_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Groups *****/
   Att_ShowLstGrpsToEditAttEvent (Att.AttCod);

   /***** End table, send button and end box *****/
   if (ItsANewAttEvent)
      Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_event);
   else
      Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** Show current attendance events *****/
   Att_GetListAttEvents (Att_NEWEST_FIRST);
   Att_ShowAllAttEvents ();
  }

/*****************************************************************************/
/************* Show list of groups to edit and attendance event **************/
/*****************************************************************************/

static void Att_ShowLstGrpsToEditAttEvent (long AttCod)
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
      Box_BoxTableBegin ("100%",NULL,
                         NULL,NULL,
                         NULL,Box_NOT_CLOSABLE,0);

      /***** First row: checkbox to select the whole course *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("colspan=\"7\" class=\"DAT LM\"");
      HTM_LABEL_Begin (NULL);
      HTM_INPUT_CHECKBOX ("WholeCrs",HTM_DONT_SUBMIT_ON_CHANGE,
		          "id=\"WholeCrs\" value=\"Y\"%s"
		          " onclick=\"uncheckChildren(this,'GrpCods')\"",
			  Att_CheckIfAttEventIsAssociatedToGrps (AttCod) ? "" : " checked=\"checked\"");
      HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,Gbl.Hierarchy.Crs.ShrtName);
      HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TR_End ();

      /***** List the groups for each group type *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
         if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
            Grp_ListGrpsToEditAsgAttSvyMch (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],
                                            AttCod,Grp_ATT_EVENT);

      /***** End table and box *****/
      Box_BoxTableEnd ();
      HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/*************** Receive form to create a new attendance event ***************/
/*****************************************************************************/

void Att_RecFormAttEvent (void)
  {
   extern const char *Txt_Already_existed_an_event_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_event;
   extern const char *Txt_Created_new_event_X;
   extern const char *Txt_The_event_has_been_modified;
   struct AttendanceEvent OldAtt;
   struct AttendanceEvent ReceivedAtt;
   bool ItsANewAttEvent;
   bool ReceivedAttEventIsCorrect = true;
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Get the code of the attendance event *****/
   ItsANewAttEvent = ((ReceivedAtt.AttCod = Att_GetParamAttCod ()) == -1L);

   if (!ItsANewAttEvent)
     {
      /* Get data of the old (current) attendance event from database */
      OldAtt.AttCod = ReceivedAtt.AttCod;
      Att_GetDataOfAttEventByCodAndCheckCrs (&OldAtt);
      ReceivedAtt.Hidden = OldAtt.Hidden;
     }

   /***** Get start/end date-times *****/
   ReceivedAtt.TimeUTC[Att_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   ReceivedAtt.TimeUTC[Att_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /***** Get boolean parameter that indicates if teacher's comments are visible by students *****/
   ReceivedAtt.CommentTchVisible = Par_GetParToBool ("ComTchVisible");

   /***** Get attendance event title *****/
   Par_GetParToText ("Title",ReceivedAtt.Title,Att_MAX_BYTES_ATTENDANCE_EVENT_TITLE);

   /***** Get attendance event description *****/
   Par_GetParToHTML ("Txt",Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (ReceivedAtt.TimeUTC[Att_START_TIME] == 0)
      ReceivedAtt.TimeUTC[Att_START_TIME] = Gbl.StartExecutionTimeUTC;
   if (ReceivedAtt.TimeUTC[Att_END_TIME] == 0)
      ReceivedAtt.TimeUTC[Att_END_TIME] = ReceivedAtt.TimeUTC[Att_START_TIME] + 2 * 60 * 60;	// +2 hours // TODO: 2 * 60 * 60 should be in a #define in swad_config.h

   /***** Check if title is correct *****/
   if (ReceivedAtt.Title[0])	// If there's an attendance event title
     {
      /* If title of attendance event was in database... */
      if (Att_CheckIfSimilarAttEventExists ("Title",ReceivedAtt.Title,ReceivedAtt.AttCod))
        {
         ReceivedAttEventIsCorrect = false;

	 Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_an_event_with_the_title_X,
                        ReceivedAtt.Title);
        }
     }
   else	// If there is not an attendance event title
     {
      ReceivedAttEventIsCorrect = false;
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_title_of_the_event);
     }

   /***** Create a new attendance event or update an existing one *****/
   if (ReceivedAttEventIsCorrect)
     {
      /* Get groups for this attendance events */
      Grp_GetParCodsSeveralGrps ();

      if (ItsANewAttEvent)
	{
	 ReceivedAtt.Hidden = false;	// New attendance events are visible by default
         Att_CreateAttEvent (&ReceivedAtt,Description);	// Add new attendance event to database

         /***** Write success message *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_event_X,
		        ReceivedAtt.Title);
	}
      else
	{
         Att_UpdateAttEvent (&ReceivedAtt,Description);

	 /***** Write success message *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_The_event_has_been_modified);
	}

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();
     }
   else
      Att_RequestCreatOrEditAttEvent ();

   /***** Show attendance events again *****/
   Att_SeeAttEvents ();
  }

/*****************************************************************************/
/********************* Create a new attendance event *************************/
/*****************************************************************************/

void Att_CreateAttEvent (struct AttendanceEvent *Att,const char *Description)
  {
   /***** Create a new attendance event *****/
   Att->AttCod =
   DB_QueryINSERTandReturnCode ("can not create new attendance event",
				"INSERT INTO att_events"
				" (CrsCod,Hidden,UsrCod,"
				"StartTime,EndTime,CommentTchVisible,Title,Txt)"
				" VALUES"
				" (%ld,'%c',%ld,"
				"FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),'%c','%s','%s')",
				Gbl.Hierarchy.Crs.CrsCod,
				Att->Hidden ? 'Y' :
					      'N',
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Att->TimeUTC[Att_START_TIME],
				Att->TimeUTC[Att_END_TIME  ],
				Att->CommentTchVisible ? 'Y' :
							 'N',
				Att->Title,
				Description);

   /***** Create groups *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Att_CreateGrps (Att->AttCod);
  }

/*****************************************************************************/
/****************** Update an existing attendance event **********************/
/*****************************************************************************/

void Att_UpdateAttEvent (struct AttendanceEvent *Att,const char *Description)
  {
   /***** Update the data of the attendance event *****/
   DB_QueryUPDATE ("can not update attendance event",
		   "UPDATE att_events SET "
		   "Hidden='%c',"
		   "StartTime=FROM_UNIXTIME(%ld),"
		   "EndTime=FROM_UNIXTIME(%ld),"
		   "CommentTchVisible='%c',Title='%s',Txt='%s'"
		   " WHERE AttCod=%ld AND CrsCod=%ld",
                   Att->Hidden ? 'Y' :
        	                 'N',
                   Att->TimeUTC[Att_START_TIME],
                   Att->TimeUTC[Att_END_TIME  ],
                   Att->CommentTchVisible ? 'Y' :
        	                            'N',
                   Att->Title,
                   Description,
                   Att->AttCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Update groups *****/
   /* Remove old groups */
   Att_RemoveAllTheGrpsAssociatedToAnAttEvent (Att->AttCod);

   /* Create new groups */
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Att_CreateGrps (Att->AttCod);
  }

/*****************************************************************************/
/******** Check if an attendance event is associated to any group ************/
/*****************************************************************************/

bool Att_CheckIfAttEventIsAssociatedToGrps (long AttCod)
  {
   /***** Get if an attendance event is associated to a group from database *****/
   return (DB_QueryCOUNT ("can not check if an attendance event"
			  " is associated to groups",
		          "SELECT COUNT(*) FROM att_grp WHERE AttCod=%ld",
			  AttCod) != 0);
  }

/*****************************************************************************/
/********* Check if an attendance event is associated to a group *************/
/*****************************************************************************/

bool Att_CheckIfAttEventIsAssociatedToGrp (long AttCod,long GrpCod)
  {
   /***** Get if an attendance event is associated to a group from database *****/
   return (DB_QueryCOUNT ("can not check if an attendance event"
			  " is associated to a group",
			  "SELECT COUNT(*) FROM att_grp"
			  " WHERE AttCod=%ld AND GrpCod=%ld",
			  AttCod,GrpCod) != 0);
  }

/*****************************************************************************/
/****************** Remove groups of an attendance event *********************/
/*****************************************************************************/

static void Att_RemoveAllTheGrpsAssociatedToAnAttEvent (long AttCod)
  {
   /***** Remove groups of the attendance event *****/
   DB_QueryDELETE ("can not remove the groups"
		   " associated to an attendance event",
		   "DELETE FROM att_grp WHERE AttCod=%ld",
		   AttCod);
  }

/*****************************************************************************/
/************* Remove one group from all the attendance events ***************/
/*****************************************************************************/

void Att_RemoveGroup (long GrpCod)
  {
   /***** Remove group from all the attendance events *****/
   DB_QueryDELETE ("can not remove group from the associations"
	           " between attendance events and groups",
		   "DELETE FROM att_grp WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/******** Remove groups of one type from all the attendance events ***********/
/*****************************************************************************/

void Att_RemoveGroupsOfType (long GrpTypCod)
  {
   /***** Remove group from all the attendance events *****/
   DB_QueryDELETE ("can not remove groups of a type from the associations"
		   " between attendance events and groups",
		   "DELETE FROM att_grp USING crs_grp,att_grp"
		   " WHERE crs_grp.GrpTypCod=%ld"
		   " AND crs_grp.GrpCod=att_grp.GrpCod",
                   GrpTypCod);
  }

/*****************************************************************************/
/***************** Create groups of an attendance event **********************/
/*****************************************************************************/

static void Att_CreateGrps (long AttCod)
  {
   unsigned NumGrpSel;

   /***** Create groups of the attendance event *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
      /* Create group */
      DB_QueryINSERT ("can not associate a group to an attendance event",
		      "INSERT INTO att_grp"
		      " (AttCod,GrpCod)"
		      " VALUES"
		      " (%ld,%ld)",
                      AttCod,
		      Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
  }

/*****************************************************************************/
/****** Get and write the names of the groups of an attendance event *********/
/*****************************************************************************/

static void Att_GetAndWriteNamesOfGrpsAssociatedToAttEvent (struct AttendanceEvent *Att)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_and;
   extern const char *Txt_The_whole_course;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrp;
   unsigned NumGrps;

   /***** Get groups associated to an attendance event from database *****/
   NumGrps = (unsigned) DB_QuerySELECT (&mysql_res,"can not get groups of an attendance event",
				        "SELECT crs_grp_types.GrpTypName,"
				               "crs_grp.GrpName,"
				               "classrooms.ShortName"
					" FROM (att_grp,crs_grp,crs_grp_types)"
				        " LEFT JOIN classrooms"
				        " ON crs_grp.ClaCod=classrooms.ClaCod"
					" WHERE att_grp.AttCod=%ld"
					" AND att_grp.GrpCod=crs_grp.GrpCod"
					" AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
					" ORDER BY crs_grp_types.GrpTypName,crs_grp.GrpName",
					Att->AttCod);

   /***** Write heading *****/
   HTM_DIV_Begin ("class=\"%s\"",Att->Hidden ? "ASG_GRP_LIGHT" :
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

         /* Write group type name (row[0]) and group name (row[1]) */
         HTM_TxtF ("%s&nbsp;%s",row[0],row[1]);

         /* Write the name of the classroom (row[2]) */
	 if (row[2])	// May be NULL because of LEFT JOIN
	    if (row[2][0])
               HTM_TxtF ("&nbsp;(%s)",row[2]);

	 /* Write separator */
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
/*********** Remove all users registered in an attendance event **************/
/*****************************************************************************/

static void Att_RemoveAllUsrsFromAnAttEvent (long AttCod)
  {
   DB_QueryDELETE ("can not remove attendance event",
		   "DELETE FROM att_usr WHERE AttCod=%ld",
		   AttCod);
  }

/*****************************************************************************/
/* Remove one user from all the attendance events where he/she is registered */
/*****************************************************************************/

void Att_RemoveUsrFromAllAttEvents (long UsrCod)
  {
   /***** Remove group from all the attendance events *****/
   DB_QueryDELETE ("can not remove user from all attendance events",
		   "DELETE FROM att_usr WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/*********** Remove one student from all the attendance events ***************/
/*****************************************************************************/

void Att_RemoveUsrFromCrsAttEvents (long UsrCod,long CrsCod)
  {
   /***** Remove group from all the attendance events *****/
   DB_QueryDELETE ("can not remove user from attendance events of a course",
		   "DELETE FROM att_usr USING att_events,att_usr"
		   " WHERE att_events.CrsCod=%ld"
		   " AND att_events.AttCod=att_usr.AttCod"
		   " AND att_usr.UsrCod=%ld",
                   CrsCod,UsrCod);
  }

/*****************************************************************************/
/*********************** Remove an attendance event **************************/
/*****************************************************************************/

static void Att_RemoveAttEventFromCurrentCrs (long AttCod)
  {
   DB_QueryDELETE ("can not remove attendance event",
		   "DELETE FROM att_events"
		   " WHERE AttCod=%ld AND CrsCod=%ld",
                   AttCod,Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/*************** Remove all the attendance events of a course ****************/
/*****************************************************************************/

void Att_RemoveCrsAttEvents (long CrsCod)
  {
   /***** Remove students *****/
   DB_QueryDELETE ("can not remove all the students registered"
		   " in events of a course",
		   "DELETE FROM att_usr USING att_events,att_usr"
		   " WHERE att_events.CrsCod=%ld"
		   " AND att_events.AttCod=att_usr.AttCod",
                   CrsCod);

   /***** Remove groups *****/
   DB_QueryDELETE ("can not remove all the groups associated"
		   " to attendance events of a course",
		   "DELETE FROM att_grp USING att_events,att_grp"
		   " WHERE att_events.CrsCod=%ld"
		   " AND att_events.AttCod=att_grp.AttCod",
                   CrsCod);

   /***** Remove attendance events *****/
   DB_QueryDELETE ("can not remove all the attendance events of a course",
		   "DELETE FROM att_events WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/*************** Get number of attendance events in a course *****************/
/*****************************************************************************/

unsigned Att_GetNumAttEventsInCrs (long CrsCod)
  {
   /***** Get number of attendance events in a course from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of attendance events"
			     " in course",
			     "SELECT COUNT(*) FROM att_events"
			     " WHERE CrsCod=%ld",
			     CrsCod);
  }

/*****************************************************************************/
/*************** Get number of courses with attendance events ****************/
/*****************************************************************************/
// Returns the number of courses with attendance events
// in this location (all the platform, current degree or current course)

unsigned Att_GetNumCoursesWithAttEvents (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with attendance events from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with attendance events",
			 "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM att_events"
			 " WHERE CrsCod>0");
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with attendance events",
			 "SELECT COUNT(DISTINCT att_events.CrsCod)"
			 " FROM centres,degrees,courses,att_events"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=att_events.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with attendance events",
			 "SELECT COUNT(DISTINCT att_events.CrsCod)"
			 " FROM degrees,courses,att_events"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=att_events.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with attendance events",
			 "SELECT COUNT(DISTINCT att_events.CrsCod)"
			 " FROM courses,att_events"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=att_events.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with attendance events",
			 "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM att_events"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of courses *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumCourses) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of courses with attendance events.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCourses;
  }

/*****************************************************************************/
/********************* Get number of attendance events ***********************/
/*****************************************************************************/
// Returns the number of attendance events
// in this location (all the platform, current degree or current course)

unsigned Att_GetNumAttEvents (Hie_Level_t Scope,unsigned *NumNotif)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAttEvents;

   /***** Get number of attendance events from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of attendance events",
			 "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM att_events"
			 " WHERE CrsCod>0");
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of attendance events",
			 "SELECT COUNT(*),SUM(att_events.NumNotif)"
			 " FROM centres,degrees,courses,att_events"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=att_events.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of attendance events",
			 "SELECT COUNT(*),SUM(att_events.NumNotif)"
			 " FROM degrees,courses,att_events"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=att_events.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of attendance events",
			 "SELECT COUNT(*),SUM(att_events.NumNotif)"
			 " FROM courses,att_events"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=att_events.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of attendance events",
			 "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM att_events"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of attendance events *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumAttEvents) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of attendance events.");

   /***** Get number of notifications by email *****/
   if (row[1])
     {
      if (sscanf (row[1],"%u",NumNotif) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of notifications of attendance events.");
     }
   else
      *NumNotif = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumAttEvents;
  }

/*****************************************************************************/
/************************ Show one attendance event **************************/
/*****************************************************************************/

void Att_SeeOneAttEvent (void)
  {
   extern const char *Hlp_USERS_Attendance;
   extern const char *Txt_Event;
   struct AttendanceEvent Att;

   /***** Get attendance event code *****/
   if (Gbl.AttEvents.AttCod <= 0)
      if ((Gbl.AttEvents.AttCod = Att_GetParamAttCod ()) == -1L)
         Lay_ShowErrorAndExit ("Code of attendance event is missing.");

   /***** Get parameters *****/
   Att_GetParamAttOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.AttEvents.CurrentPage = Pag_GetParamPagNum (Pag_ATT_EVENTS);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_Event,
                      NULL,NULL,
                      Hlp_USERS_Attendance,Box_NOT_CLOSABLE,2);

   Att.AttCod = Gbl.AttEvents.AttCod;
   Att_ShowOneAttEvent (&Att,true);

   /***** End table and box *****/
   Box_BoxTableEnd ();

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 Att_ListAttOnlyMeAsStudent (&Att);
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Show list of students *****/
         Att_ListAttStudents (&Att);
         break;
      default:
         break;
     }
  }

/*****************************************************************************/
/*********************** List me as student in one event *********************/
/*****************************************************************************/
// Att must be filled before calling this function

static void Att_ListAttOnlyMeAsStudent (struct AttendanceEvent *Att)
  {
   extern const char *Hlp_USERS_Attendance;
   extern const char *Txt_Attendance;
   extern const char *Txt_Student_comment;
   extern const char *Txt_Teachers_comment;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Save_changes;

   /***** Get my setting about photos in users' list for current course *****/
   Usr_GetMyPrefAboutListWithPhotosFromDB ();

   /***** Begin form *****/
   if (Att->Open)
     {
      Frm_StartForm (ActRecAttMe);
      Att_PutParamAttCod (Att->AttCod);
     }

   /***** List students (only me) *****/
   /* Begin box */
   Box_BoxBegin (NULL,Txt_Attendance,
                 NULL,NULL,
                 Hlp_USERS_Attendance,Box_NOT_CLOSABLE);

   /* Begin table */
   HTM_TABLE_BeginWideMarginPadding (2);

   /* Header */
   HTM_TR_Begin (NULL);

   HTM_TH_Empty (3);
   if (Gbl.Usrs.Listing.WithPhotos)
      HTM_TH_Empty (1);
   HTM_TH (1,2,"TIT_TBL LM",Txt_ROLES_SINGUL_Abc[Rol_STD][Usr_SEX_UNKNOWN]);
   HTM_TH (1,1,"LM",Txt_Student_comment);
   HTM_TH (1,1,"LM",Txt_Teachers_comment);

   HTM_TR_End ();

   /* List of students (only me) */
   Att_WriteRowUsrToCallTheRoll (1,&Gbl.Usrs.Me.UsrDat,Att);

   /* End table */
   HTM_TABLE_End ();

   /* Send button */
   if (Att->Open)
     {
      Btn_PutConfirmButton (Txt_Save_changes);
      Frm_EndForm ();
     }

   /* End box */
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*************** List students who attended to one event *********************/
/*****************************************************************************/
// Att must be filled before calling this function

static void Att_ListAttStudents (struct AttendanceEvent *Att)
  {
   extern const char *Hlp_USERS_Attendance;
   extern const char *Txt_Attendance;
   extern const char *Txt_Student_comment;
   extern const char *Txt_Teachers_comment;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Save_changes;
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Get groups to show ******/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get and order list of students in this course *****/
   Usr_GetListUsrs (Hie_CRS,Rol_STD);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Attendance,
                 NULL,NULL,
                 Hlp_USERS_Attendance,Box_NOT_CLOSABLE);

   /***** Form to select groups *****/
   Grp_ShowFormToSelectSeveralGroups (Att_PutParamSelectedAttCod,(void *) &Gbl,
                                      Grp_MY_GROUPS);

   /***** Start section with user list *****/
   HTM_SECTION_Begin (Usr_USER_LIST_SECTION_ID);

   if (Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs)
     {
      /***** Get my preference about photos in users' list for current course *****/
      Usr_GetMyPrefAboutListWithPhotosFromDB ();

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /* Begin form */
      Frm_StartForm (ActRecAttStd);
      Att_PutParamAttCod (Att->AttCod);
      Grp_PutParamsCodGrps ();

      /* Begin table */
      HTM_TABLE_BeginWideMarginPadding (2);

      /* Header */
      HTM_TR_Begin (NULL);

      HTM_TH_Empty (3);
      if (Gbl.Usrs.Listing.WithPhotos)
         HTM_TH_Empty (1);
      HTM_TH (1,2,"LM",Txt_ROLES_SINGUL_Abc[Rol_STD][Usr_SEX_UNKNOWN]);
      HTM_TH (1,1,"LM",Txt_Student_comment);
      HTM_TH (1,1,"LM",Txt_Teachers_comment);

      HTM_TR_End ();

      /* List of students */
      for (NumUsr = 0, Gbl.RowEvenOdd = 0;
	   NumUsr < Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs;
	   NumUsr++)
        {
	 /* Copy user's basic data from list */
         Usr_CopyBasicUsrDataFromList (&UsrDat,&Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr]);

	 /* Get list of user's IDs */
         ID_GetListIDsFromUsrCod (&UsrDat);

         Att_WriteRowUsrToCallTheRoll (NumUsr + 1,&UsrDat,Att);
        }

      /* End table */
      HTM_TABLE_End ();

      /* Send button */
      Btn_PutConfirmButton (Txt_Save_changes);

      /***** End form *****/
      Frm_EndForm ();

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

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (Rol_STD);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/************** Write a row of a table with the data of a user ***************/
/*****************************************************************************/

static void Att_WriteRowUsrToCallTheRoll (unsigned NumUsr,
                                          struct UsrData *UsrDat,
                                          struct AttendanceEvent *Att)
  {
   bool Present;
   char PhotoURL[PATH_MAX + 1];
   bool ShowPhoto;
   char CommentStd[Cns_MAX_BYTES_TEXT + 1];
   char CommentTch[Cns_MAX_BYTES_TEXT + 1];
   bool ItsMe;
   bool ICanChangeStdAttendance;
   bool ICanEditStdComment;
   bool ICanEditTchComment;

   /***** Set who can edit *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 // A student can see only her/his attendance
	 ItsMe = Usr_ItsMe (UsrDat->UsrCod);
	 if (!ItsMe)
	    Lay_ShowErrorAndExit ("Wrong call.");
	 ICanChangeStdAttendance = false;
	 ICanEditStdComment = Att->Open;	// Attendance event is open
	 ICanEditTchComment = false;
	 break;
      case Rol_TCH:
	 ICanChangeStdAttendance = true;
	 ICanEditStdComment = false;
	 ICanEditTchComment = true;
	 break;
      case Rol_SYS_ADM:
	 ICanChangeStdAttendance = true;
	 ICanEditStdComment = false;
	 ICanEditTchComment = false;
	 break;
      default:
	 ICanChangeStdAttendance = false;
	 ICanEditStdComment = false;
	 ICanEditTchComment = false;
	 break;
     }

   /***** Check if this student is already present in the current event *****/
   Present = Att_CheckIfUsrIsPresentInAttEventAndGetComments (Att->AttCod,UsrDat->UsrCod,CommentStd,CommentTch);

   /***** Icon to show if the user is already present *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);
   HTM_LABEL_Begin ("for=\"Std%u\"",NumUsr);
   Att_PutCheckOrCross (Present);
   HTM_LABEL_End ();
   HTM_TD_End ();

   /***** Checkbox to select user *****/
   HTM_TD_Begin ("class=\"CT COLOR%u\"",Gbl.RowEvenOdd);
   HTM_INPUT_CHECKBOX ("UsrCodStd",HTM_DONT_SUBMIT_ON_CHANGE,
		       "id=\"Std%u\" value=\"%s\"%s%s",
	               NumUsr,UsrDat->EncryptedUsrCod,
		       Present ? " checked=\"checked\"" : "",
		       ICanChangeStdAttendance ? "" : " disabled=\"disabled\"");
   HTM_TD_End ();

   /***** Write number of student in the list *****/
   HTM_TD_Begin ("class=\"%s RT COLOR%u\"",
		 UsrDat->Accepted ? "DAT_N" :
				    "DAT",
		 Gbl.RowEvenOdd);
   HTM_Unsigned (NumUsr);
   HTM_TD_End ();

   /***** Show student's photo *****/
   if (Gbl.Usrs.Listing.WithPhotos)
     {
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO45x60",Pho_ZOOM,false);
      HTM_TD_End ();
     }

   /***** Write user's ID ******/
   HTM_TD_Begin ("class=\"%s LT COLOR%u\"",
		 UsrDat->Accepted ? "DAT_SMALL_N" :
				    "DAT_SMALL",
		 Gbl.RowEvenOdd);
   ID_WriteUsrIDs (UsrDat,NULL);
   HTM_TD_End ();

   /***** Write student's name *****/
   HTM_TD_Begin ("class=\"%s LT COLOR%u\"",
		 UsrDat->Accepted ? "DAT_SMALL_N" :
				    "DAT_SMALL",
		 Gbl.RowEvenOdd);
   HTM_Txt (UsrDat->Surname1);
   if (UsrDat->Surname2[0])
      HTM_TxtF ("&nbsp;%s",UsrDat->Surname2);
   HTM_TxtF (", %s",UsrDat->FirstName);
   HTM_TD_End ();

   /***** Student's comment: write form or text */
   HTM_TD_Begin ("class=\"DAT_SMALL LT COLOR%u\"",Gbl.RowEvenOdd);
   if (ICanEditStdComment)	// Show with form
     {
      HTM_TEXTAREA_Begin ("name=\"CommentStd%s\" cols=\"40\" rows=\"3\"",
	                  UsrDat->EncryptedUsrCod);
      HTM_Txt (CommentStd);
      HTM_TEXTAREA_End ();
     }
   else				// Show without form
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        CommentStd,Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (CommentStd);
     }
   HTM_TD_End ();

   /***** Teacher's comment: write form, text or nothing */
   HTM_TD_Begin ("class=\"DAT_SMALL LT COLOR%u\"",Gbl.RowEvenOdd);
   if (ICanEditTchComment)		// Show with form
     {
      HTM_TEXTAREA_Begin ("name=\"CommentTch%s\" cols=\"40\" rows=\"3\"",
			  UsrDat->EncryptedUsrCod);
      HTM_Txt (CommentTch);
      HTM_TEXTAREA_End ();
     }
   else	if (Att->CommentTchVisible)	// Show without form
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        CommentTch,Cns_MAX_BYTES_TEXT,false);
      HTM_Txt (CommentTch);
     }
   HTM_TD_End ();
   HTM_TR_End ();

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/**************** Put link to view one attendance event **********************/
/*****************************************************************************/

static void Att_PutLinkAttEvent (struct AttendanceEvent *AttEvent,
				 const char *Title,const char *Txt,
				 const char *Class)
  {
   Frm_StartForm (ActSeeOneAtt);
   Att_PutParamAttCod (AttEvent->AttCod);
   Att_PutParamsCodGrps (AttEvent->AttCod);
   HTM_BUTTON_SUBMIT_Begin (Title,Class,NULL);
   HTM_Txt (Txt);
   HTM_BUTTON_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/****** Put parameters with the default groups in an attendance event ********/
/*****************************************************************************/

static void Att_PutParamsCodGrps (long AttCod)
  {
   extern const char *Par_SEPARATOR_PARAM_MULTIPLE;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrp;
   unsigned NumGrps;
   size_t MaxLengthGrpCods;
   char *GrpCods;

   /***** Get groups associated to an attendance event from database *****/
   if (Gbl.Crs.Grps.NumGrps)
      NumGrps = (unsigned) DB_QuerySELECT (&mysql_res,"can not get groups of an attendance event",
					   "SELECT GrpCod FROM att_grp"
					   " WHERE att_grp.AttCod=%ld",
					   AttCod);
   else
      NumGrps = 0;

   /***** Get groups *****/
   if (NumGrps) // Groups found...
     {
      MaxLengthGrpCods = NumGrps * (1 + 20) - 1;
      if ((GrpCods = (char *) malloc (MaxLengthGrpCods + 1)) == NULL)
	 Lay_NotEnoughMemoryExit ();
      GrpCods[0] = '\0';

      /* Get groups */
      for (NumGrp = 0;
	   NumGrp < NumGrps;
	   NumGrp++)
        {
         /* Get next group */
         row = mysql_fetch_row (mysql_res);

         /* Append group code to list */
         if (NumGrp)
            Str_Concat (GrpCods,Par_SEPARATOR_PARAM_MULTIPLE,MaxLengthGrpCods);
         Str_Concat (GrpCods,row[0],MaxLengthGrpCods);
        }

      Par_PutHiddenParamString (NULL,"GrpCods",GrpCods);
      free (GrpCods);
     }
   else
      /***** Write the boolean parameter that indicates if all the groups must be listed *****/
      Par_PutHiddenParamChar ("AllGroups",'Y');

   /***** Free structure that stores the query result *****/
   if (Gbl.Crs.Grps.NumGrps)
      DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************** Save me as students who attended to an event ****************/
/*****************************************************************************/

void Att_RegisterMeAsStdInAttEvent (void)
  {
   extern const char *Txt_Your_comment_has_been_updated;
   struct AttendanceEvent Att;
   bool Present;
   char CommentStd[Cns_MAX_BYTES_TEXT + 1];
   char CommentTch[Cns_MAX_BYTES_TEXT + 1];

   /***** Get attendance event code *****/
   if ((Att.AttCod = Att_GetParamAttCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of attendance event is missing.");
   Att_GetDataOfAttEventByCodAndCheckCrs (&Att);	// This checks that event belong to current course

   if (Att.Open)
     {
      /***** Get comments for this student *****/
      Present = Att_CheckIfUsrIsPresentInAttEventAndGetComments (Att.AttCod,Gbl.Usrs.Me.UsrDat.UsrCod,
	                                                         CommentStd,CommentTch);
      Par_GetParToHTML (Str_BuildStringStr ("CommentStd%s",
					    Gbl.Usrs.Me.UsrDat.EncryptedUsrCod),
			CommentStd,Cns_MAX_BYTES_TEXT);
      Str_FreeString ();

      if (Present ||
	  CommentStd[0] ||
	  CommentTch[0])
	 /***** Register student *****/
	 Att_RegUsrInAttEventChangingComments (Att.AttCod,Gbl.Usrs.Me.UsrDat.UsrCod,
	                                       Present,CommentStd,CommentTch);
      else
	 /***** Remove student *****/
	 Att_RemoveUsrFromAttEvent (Att.AttCod,Gbl.Usrs.Me.UsrDat.UsrCod);

      /***** Write final message *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_Your_comment_has_been_updated);
     }

   /***** Show the attendance event again *****/
   Gbl.AttEvents.AttCod = Att.AttCod;
   Att_SeeOneAttEvent ();
  }

/*****************************************************************************/
/***************** Save students who attended to an event ********************/
/*****************************************************************************/
/* Algorithm:
   1. Get list of students in the groups selected: Gbl.Usrs.LstUsrs[Rol_STD]
   2. Mark all students in the groups selected setting Remove=true
   3. Get list of students marked as present by me: Gbl.Usrs.Selected.List[Rol_STD]
   4. Loop over the list Gbl.Usrs.Selected.List[Rol_STD],
      that holds the list of the students marked as present,
      marking the students in Gbl.Usrs.LstUsrs[Rol_STD].Lst as Remove=false
   5. Delete from att_usr all the students marked as Remove=true
   6. Replace (insert without duplicated) into att_usr all the students marked as Remove=false
 */
void Att_RegisterStudentsInAttEvent (void)
  {
   extern const char *Txt_Presents;
   extern const char *Txt_Absents;
   struct AttendanceEvent Att;
   unsigned NumUsr;
   const char *Ptr;
   bool Present;
   unsigned NumStdsPresent;
   unsigned NumStdsAbsent;
   struct UsrData UsrData;
   char CommentStd[Cns_MAX_BYTES_TEXT + 1];
   char CommentTch[Cns_MAX_BYTES_TEXT + 1];

   /***** Get attendance event code *****/
   if ((Att.AttCod = Att_GetParamAttCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of attendance event is missing.");
   Att_GetDataOfAttEventByCodAndCheckCrs (&Att);	// This checks that event belong to current course

   /***** Get groups selected *****/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** 1. Get list of students in the groups selected: Gbl.Usrs.LstUsrs[Rol_STD] *****/
   /* Get list of students in the groups selected */
   Usr_GetListUsrs (Hie_CRS,Rol_STD);

   if (Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs)	// If there are students in the groups selected...
     {
      /***** 2. Mark all students in the groups selected setting Remove=true *****/
      for (NumUsr = 0;
           NumUsr < Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs;
           NumUsr++)
         Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].Remove = true;

      /***** 3. Get list of students marked as present by me: Gbl.Usrs.Selected.List[Rol_STD] *****/
      Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrData);

      /***** 4. Loop over the list Gbl.Usrs.Selected.List[Rol_STD],
                that holds the list of the students marked as present,
                marking the students in Gbl.Usrs.LstUsrs[Rol_STD].Lst as Remove=false *****/
      Ptr = Gbl.Usrs.Selected.List[Rol_STD];
      while (*Ptr)
	{
	 Par_GetNextStrUntilSeparParamMult (&Ptr,UsrData.EncryptedUsrCod,
	                                    Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
	 Usr_GetUsrCodFromEncryptedUsrCod (&UsrData);
	 if (UsrData.UsrCod > 0)	// Student exists in database
	    /***** Mark student to not be removed *****/
	    for (NumUsr = 0;
		 NumUsr < Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs;
		 NumUsr++)
	       if (Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].UsrCod == UsrData.UsrCod)
		 {
		  Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].Remove = false;
	          break;	// Found! Exit loop
	         }
	}

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrData);

      /***** Free memory *****/
      /* Free memory used by list of selected students' codes */
      Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

      // 5. Delete from att_usr all the students marked as Remove=true
      // 6. Replace (insert without duplicated) into att_usr all the students marked as Remove=false
      for (NumUsr = 0, NumStdsAbsent = NumStdsPresent = 0;
	   NumUsr < Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs;
	   NumUsr++)
	{
	 /***** Get comments for this student *****/
	 Att_CheckIfUsrIsPresentInAttEventAndGetComments (Att.AttCod,Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].UsrCod,CommentStd,CommentTch);
	 Par_GetParToHTML (Str_BuildStringStr ("CommentTch%s",
					       Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].EncryptedUsrCod),
			   CommentTch,Cns_MAX_BYTES_TEXT);
	 Str_FreeString ();

	 Present = !Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].Remove;

	 if (Present ||
	     CommentStd[0] ||
	     CommentTch[0])
	    /***** Register student *****/
	    Att_RegUsrInAttEventChangingComments (Att.AttCod,Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].UsrCod,
					          Present,CommentStd,CommentTch);
	 else
	    /***** Remove student *****/
	    Att_RemoveUsrFromAttEvent (Att.AttCod,Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].UsrCod);

	 if (Present)
            NumStdsPresent++;
      	 else
	    NumStdsAbsent++;
	}

      /***** Free memory for students list *****/
      Usr_FreeUsrsList (Rol_STD);

      /***** Write final message *****/
      Ale_ShowAlert (Ale_INFO,"%s: %u<br />"
		              "%s: %u",
		     Txt_Presents,NumStdsPresent,
		     Txt_Absents ,NumStdsAbsent );
     }
   else	// Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs == 0
      /***** Show warning indicating no students found *****/
      Usr_ShowWarningNoUsersFound (Rol_STD);

   /***** Show the attendance event again *****/
   Gbl.AttEvents.AttCod = Att.AttCod;
   Att_SeeOneAttEvent ();

   /***** Free memory for list of groups selected *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/******* Get number of students from a list who attended to an event *********/
/*****************************************************************************/

static void Att_GetNumStdsTotalWhoAreInAttEvent (struct AttendanceEvent *Att)
  {
   /***** Count number of students registered in an event in database *****/
   Att->NumStdsTotal =
   (unsigned) DB_QueryCOUNT ("can not get number of students"
			     " who are registered in an event",
			     "SELECT COUNT(*) FROM att_usr"
			     " WHERE AttCod=%ld AND Present='Y'",
			     Att->AttCod);
  }

/*****************************************************************************/
/********* Get number of users from a list who attended to an event **********/
/*****************************************************************************/

static unsigned Att_GetNumUsrsFromAListWhoAreInAttEvent (long AttCod,
							 long LstSelectedUsrCods[],
							 unsigned NumUsrsInList)
  {
   char *SubQueryUsrs;
   unsigned NumUsrsInAttEvent;

   if (NumUsrsInList)
     {
      /***** Create subquery string *****/
      Usr_CreateSubqueryUsrCods (LstSelectedUsrCods,NumUsrsInList,
				 &SubQueryUsrs);

      /***** Get number of users in attendance event from database ****/
      NumUsrsInAttEvent =
      (unsigned) DB_QueryCOUNT ("can not get number of students"
			        " from a list who are registered in an event",
				"SELECT COUNT(*) FROM att_usr"
				" WHERE AttCod=%ld"
				" AND UsrCod IN (%s) AND Present='Y'",
				AttCod,SubQueryUsrs);

      /***** Free memory for subquery string *****/
      Usr_FreeSubqueryUsrCods (SubQueryUsrs);
     }
   else
      NumUsrsInAttEvent = 0;

   return NumUsrsInAttEvent;
  }

/*****************************************************************************/
/***************** Check if a student attended to an event *******************/
/*****************************************************************************/

static bool Att_CheckIfUsrIsInTableAttUsr (long AttCod,long UsrCod,bool *Present)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   bool InDBTable;

   /***** Check if a student is registered in an event in database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get if a student"
					" is already registered"
					" in an event",
			     "SELECT Present FROM att_usr"
			     " WHERE AttCod=%ld AND UsrCod=%ld",
			     AttCod,UsrCod);
   if (NumRows)
     {
      InDBTable = true;

      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get if present (row[0]) */
      *Present = (row[0][0] == 'Y');
     }
   else	// User is not present
     {
      InDBTable = false;
      *Present = false;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return InDBTable;
  }

/*****************************************************************************/
/***************** Check if a student attended to an event *******************/
/*****************************************************************************/

static bool Att_CheckIfUsrIsPresentInAttEvent (long AttCod,long UsrCod)
  {
   bool Present;

   Att_CheckIfUsrIsInTableAttUsr (AttCod,UsrCod,&Present);

   return Present;
  }

/*****************************************************************************/
/***************** Check if a student attended to an event *******************/
/*****************************************************************************/

static bool Att_CheckIfUsrIsPresentInAttEventAndGetComments (long AttCod,long UsrCod,
                                                             char CommentStd[Cns_MAX_BYTES_TEXT + 1],
                                                             char CommentTch[Cns_MAX_BYTES_TEXT + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   bool Present;

   /***** Check if a students is registered in an event in database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get if a student"
				        " is already registered"
				        " in an event",
			     "SELECT Present,CommentStd,CommentTch"
			     " FROM att_usr"
			     " WHERE AttCod=%ld AND UsrCod=%ld",
			     AttCod,UsrCod);
   if (NumRows)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get if present (row[0]) */
      Present = (row[0][0] == 'Y');

      /* Get student's comment (row[1]) */
      Str_Copy (CommentStd,row[1],
                Cns_MAX_BYTES_TEXT);

      /* Get teacher's comment (row[2]) */
      Str_Copy (CommentTch,row[2],
                Cns_MAX_BYTES_TEXT);
     }
   else	// User is not present
     {
      Present = false;
      CommentStd[0] =
      CommentTch[0] = '\0';
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Present;
  }

/*****************************************************************************/
/******* Register a user in an attendance event not changing comments ********/
/*****************************************************************************/

void Att_RegUsrInAttEventNotChangingComments (long AttCod,long UsrCod)
  {
   bool Present;

   /***** Check if user is already in table att_usr (present or not) *****/
   if (Att_CheckIfUsrIsInTableAttUsr (AttCod,UsrCod,&Present))	// User is in table att_usr
     {
      // If already present ==> nothing to do
      if (!Present)
	 /***** Set user as present in database *****/
	 DB_QueryUPDATE ("can not set user as present in an event",
			 "UPDATE att_usr SET Present='Y'"
			 " WHERE AttCod=%ld AND UsrCod=%ld",
		         AttCod,UsrCod);
     }
   else			// User is not in table att_usr
      Att_RegUsrInAttEventChangingComments (AttCod,UsrCod,true,"","");
  }

/*****************************************************************************/
/********* Register a user in an attendance event changing comments **********/
/*****************************************************************************/

static void Att_RegUsrInAttEventChangingComments (long AttCod,long UsrCod,bool Present,
                                                  const char *CommentStd,const char *CommentTch)
  {
   /***** Register user as assistant to an event in database *****/
   DB_QueryREPLACE ("can not register user in an event",
		    "REPLACE INTO att_usr"
		    " (AttCod,UsrCod,Present,CommentStd,CommentTch)"
		    " VALUES"
		    " (%ld,%ld,'%c','%s','%s')",
                    AttCod,UsrCod,
                    Present ? 'Y' :
        	              'N',
                    CommentStd,
                    CommentTch);
  }

/*****************************************************************************/
/********************** Remove a user from an event **************************/
/*****************************************************************************/

static void Att_RemoveUsrFromAttEvent (long AttCod,long UsrCod)
  {
   /***** Remove user if there is no comment in database *****/
   DB_QueryDELETE ("can not remove student from an event",
		   "DELETE FROM att_usr WHERE AttCod=%ld AND UsrCod=%ld",
                   AttCod,UsrCod);
  }

/*****************************************************************************/
/************ Remove users absent without comments from an event *************/
/*****************************************************************************/

void Att_RemoveUsrsAbsentWithoutCommentsFromAttEvent (long AttCod)
  {
   /***** Clean table att_usr *****/
   DB_QueryDELETE ("can not remove users absent"
	           " without comments from an event",
		   "DELETE FROM att_usr"
		   " WHERE AttCod=%ld AND Present='N'"
		   " AND CommentStd='' AND CommentTch=''",
	           AttCod);
  }

/*****************************************************************************/
/********** Request listing attendance of users to several events ************/
/*****************************************************************************/

void Att_ReqListUsrsAttendanceCrs (void)
  {
   extern const char *Hlp_USERS_Attendance_attendance_list;
   extern const char *Txt_Attendance_list;
   extern const char *Txt_View_attendance;

   /***** Get list of attendance events *****/
   Att_GetListAttEvents (Att_OLDEST_FIRST);

   /***** List users to select some of them *****/
   Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
				     ActSeeLstUsrAtt,
				     NULL,NULL,
				     Txt_Attendance_list,
	                             Hlp_USERS_Attendance_attendance_list,
	                             Txt_View_attendance,
				     false);	// Do not put form with date range

   /***** Free list of attendance events *****/
   Att_FreeListAttEvents ();
  }

/*****************************************************************************/
/********** List my attendance (I am a student) to several events ************/
/*****************************************************************************/

void Att_ListMyAttendanceCrs (void)
  {
   Att_TypeOfView = Att_VIEW_ONLY_ME;
   Att_ListOrPrintMyAttendanceCrs ();
  }

void Att_PrintMyAttendanceCrs (void)
  {
   Att_TypeOfView = Att_PRNT_ONLY_ME;
   Att_ListOrPrintMyAttendanceCrs ();
  }

static void Att_ListOrPrintMyAttendanceCrs (void)
  {
   extern const char *Hlp_USERS_Attendance_attendance_list;
   extern const char *Txt_Attendance;
   unsigned NumAttEvent;

   /***** Get list of attendance events *****/
   Att_GetListAttEvents (Att_OLDEST_FIRST);

   /***** Get boolean parameter that indicates if details must be shown *****/
   Gbl.AttEvents.ShowDetails = Par_GetParToBool ("ShowDetails");

   /***** Get list of groups selected ******/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get number of students in each event *****/
   for (NumAttEvent = 0;
	NumAttEvent < Gbl.AttEvents.Num;
	NumAttEvent++)
      /* Get number of students in this event */
      Gbl.AttEvents.Lst[NumAttEvent].NumStdsFromList =
      Att_GetNumUsrsFromAListWhoAreInAttEvent (Gbl.AttEvents.Lst[NumAttEvent].AttCod,
					       &Gbl.Usrs.Me.UsrDat.UsrCod,1);

   /***** Get list of attendance events selected *****/
   Att_GetListSelectedAttCods (&Gbl.AttEvents.StrAttCodsSelected);

   /***** Begin box *****/
   if (Att_TypeOfView == Att_VIEW_ONLY_ME)
      Box_BoxBegin (NULL,Txt_Attendance,
		    Att_PutIconsMyAttList,(void *) &Gbl,
		    Hlp_USERS_Attendance_attendance_list,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,Txt_Attendance,
		    NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);

   /***** List events to select *****/
   Att_ListEventsToSelect ();

   /***** Get my preference about photos in users' list for current course *****/
   Usr_GetMyPrefAboutListWithPhotosFromDB ();

   /***** Show table with attendances for every student in list *****/
   Att_ListUsrsAttendanceTable (1,&Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Show details or put button to show details *****/
   if (Gbl.AttEvents.ShowDetails)
      Att_ListStdsWithAttEventsDetails (1,&Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free memory for list of attendance events selected *****/
   free (Gbl.AttEvents.StrAttCodsSelected);

   /***** Free list of groups selected *****/
   Grp_FreeListCodSelectedGrps ();

   /***** Free list of attendance events *****/
   Att_FreeListAttEvents ();
  }

/*****************************************************************************/
/*************** List attendance of users to several events ******************/
/*****************************************************************************/

void Att_ListUsrsAttendanceCrs (void)
  {
   Att_TypeOfView = Att_VIEW_SEL_USR;
   Att_GetUsrsAndListOrPrintAttendanceCrs ();
  }

void Att_PrintUsrsAttendanceCrs (void)
  {
   Att_TypeOfView = Att_PRNT_SEL_USR;
   Att_GetUsrsAndListOrPrintAttendanceCrs ();
  }

static void Att_GetUsrsAndListOrPrintAttendanceCrs (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  Att_ListOrPrintUsrsAttendanceCrs,
                                  Att_ReqListUsrsAttendanceCrs);
  }

static void Att_ListOrPrintUsrsAttendanceCrs (void)
  {
   extern const char *Hlp_USERS_Attendance_attendance_list;
   extern const char *Txt_Attendance_list;
   unsigned NumUsrsInList;
   long *LstSelectedUsrCods;
   unsigned NumAttEvent;

   /***** Get parameters *****/
   /* Get boolean parameter that indicates if details must be shown */
   Gbl.AttEvents.ShowDetails = Par_GetParToBool ("ShowDetails");

   /* Get list of groups selected */
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Count number of valid users in list of encrypted user codes *****/
   NumUsrsInList = Usr_CountNumUsrsInListOfSelectedEncryptedUsrCods (&Gbl.Usrs.Selected);

   if (NumUsrsInList)
     {
      /***** Get list of students selected to show their attendances *****/
      Usr_GetListSelectedUsrCods (&Gbl.Usrs.Selected,NumUsrsInList,&LstSelectedUsrCods);

      /***** Get list of attendance events *****/
      Att_GetListAttEvents (Att_OLDEST_FIRST);

      /***** Get number of students in each event *****/
      for (NumAttEvent = 0;
	   NumAttEvent < Gbl.AttEvents.Num;
	   NumAttEvent++)
	 /* Get number of students in this event */
	 Gbl.AttEvents.Lst[NumAttEvent].NumStdsFromList =
	 Att_GetNumUsrsFromAListWhoAreInAttEvent (Gbl.AttEvents.Lst[NumAttEvent].AttCod,
						  LstSelectedUsrCods,NumUsrsInList);

      /***** Get list of attendance events selected *****/
      Att_GetListSelectedAttCods (&Gbl.AttEvents.StrAttCodsSelected);

      /***** Begin box *****/
      if (Att_TypeOfView == Att_VIEW_SEL_USR)
         Box_BoxBegin (NULL,Txt_Attendance_list,
		       Att_PutIconsStdsAttList,(void *) &Gbl,
		       Hlp_USERS_Attendance_attendance_list,Box_NOT_CLOSABLE);
      else
	 Box_BoxBegin (NULL,Txt_Attendance_list,
		       NULL,NULL,
		       NULL,Box_NOT_CLOSABLE);

      /***** List events to select *****/
      Att_ListEventsToSelect ();

      /***** Get my preference about photos in users' list for current course *****/
      Usr_GetMyPrefAboutListWithPhotosFromDB ();

      /***** Show table with attendances for every student in list *****/
      Att_ListUsrsAttendanceTable (NumUsrsInList,LstSelectedUsrCods);

      /***** Show details or put button to show details *****/
      if (Gbl.AttEvents.ShowDetails)
	 Att_ListStdsWithAttEventsDetails (NumUsrsInList,LstSelectedUsrCods);

      /***** End box *****/
      Box_BoxEnd ();

      /***** Free memory for list of attendance events selected *****/
      free (Gbl.AttEvents.StrAttCodsSelected);

      /***** Free list of attendance events *****/
      Att_FreeListAttEvents ();

      /***** Free list of user codes *****/
      Usr_FreeListSelectedUsrCods (LstSelectedUsrCods);
     }

   /***** Free list of groups selected *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/****************** Get list of attendance events selected *******************/
/*****************************************************************************/

static void Att_GetListSelectedAttCods (char **StrAttCodsSelected)
  {
   size_t MaxSizeListAttCodsSelected;
   unsigned NumAttEvent;
   const char *Ptr;
   long AttCod;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrpsInThisEvent;
   unsigned NumGrpInThisEvent;
   long GrpCodInThisEvent;
   unsigned NumGrpSel;

   /***** Allocate memory for list of attendance events selected *****/
   MaxSizeListAttCodsSelected = (size_t) Gbl.AttEvents.Num * (Cns_MAX_DECIMAL_DIGITS_LONG + 1);
   if ((*StrAttCodsSelected = (char *) malloc (MaxSizeListAttCodsSelected + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   /***** Get parameter multiple with list of attendance events selected *****/
   Par_GetParMultiToText ("AttCods",*StrAttCodsSelected,MaxSizeListAttCodsSelected);

   /***** Set which attendance events will be shown as selected (checkboxes on) *****/
   if ((*StrAttCodsSelected)[0])	// There are events selected
     {
      /* Reset selection */
      for (NumAttEvent = 0;
	   NumAttEvent < Gbl.AttEvents.Num;
	   NumAttEvent++)
	 Gbl.AttEvents.Lst[NumAttEvent].Selected = false;

      /* Set some events as selected */
      for (Ptr = *StrAttCodsSelected;
	   *Ptr;
	  )
	{
	 /* Get next attendance event selected */
	 Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
	 AttCod = Str_ConvertStrCodToLongCod (LongStr);

	 /* Set each event in *StrAttCodsSelected as selected */
	 for (NumAttEvent = 0;
	      NumAttEvent < Gbl.AttEvents.Num;
	      NumAttEvent++)
	    if (Gbl.AttEvents.Lst[NumAttEvent].AttCod == AttCod)
	      {
	       Gbl.AttEvents.Lst[NumAttEvent].Selected = true;
	       break;
	      }
	}
     }
   else				// No events selected
     {
      /***** Set which events will be marked as selected by default *****/
      if (!Gbl.Crs.Grps.NumGrps ||	// Course has no groups
          Gbl.Usrs.ClassPhoto.AllGroups)	// All groups selected
	 /* Set all events as selected */
	 for (NumAttEvent = 0;
	      NumAttEvent < Gbl.AttEvents.Num;
	      NumAttEvent++)
	    Gbl.AttEvents.Lst[NumAttEvent].Selected = true;
      else					// Course has groups and not all of them are selected
	 for (NumAttEvent = 0;
	      NumAttEvent < Gbl.AttEvents.Num;
	      NumAttEvent++)
	   {
	    /* Reset selection */
	    Gbl.AttEvents.Lst[NumAttEvent].Selected = false;

	    /* Set this event as selected? */
	    if (Gbl.AttEvents.Lst[NumAttEvent].NumStdsFromList)	// Some students attended to this event
	       Gbl.AttEvents.Lst[NumAttEvent].Selected = true;
	    else						// No students attended to this event
	      {
	       /***** Get groups associated to an attendance event from database *****/
	       NumGrpsInThisEvent = (unsigned) DB_QuerySELECT (&mysql_res,"can not get groups"
									  " of an attendance event",
							       "SELECT GrpCod FROM att_grp"
							       " WHERE att_grp.AttCod=%ld",
							       Gbl.AttEvents.Lst[NumAttEvent].AttCod);
	       if (NumGrpsInThisEvent)	// This event is associated to groups
		  /* Get groups associated to this event */
		  for (NumGrpInThisEvent = 0;
		       NumGrpInThisEvent < NumGrpsInThisEvent &&
		       !Gbl.AttEvents.Lst[NumAttEvent].Selected;
		       NumGrpInThisEvent++)
		    {
		     /* Get next group associated to this event */
		     row = mysql_fetch_row (mysql_res);
		     if ((GrpCodInThisEvent = Str_ConvertStrCodToLongCod (row[0])) > 0)
			/* Check if this group is selected */
			for (NumGrpSel = 0;
			     NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps &&
			     !Gbl.AttEvents.Lst[NumAttEvent].Selected;
			     NumGrpSel++)
			   if (Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel] == GrpCodInThisEvent)
			      Gbl.AttEvents.Lst[NumAttEvent].Selected = true;
		    }
	       else			// This event is not associated to groups
		  Gbl.AttEvents.Lst[NumAttEvent].Selected = true;

	       /***** Free structure that stores the query result *****/
	       DB_FreeMySQLResult (&mysql_res);
	      }
	   }
     }
  }

/*****************************************************************************/
/******* Put contextual icons when listing my assistance (as student) ********/
/*****************************************************************************/

static void Att_PutIconsMyAttList (void *Args)
  {
   if (Args)
     {
      /***** Put icon to print my assistance (as student) to several events *****/
      Ico_PutContextualIconToPrint (ActPrnLstMyAtt,
				    Att_PutFormToPrintMyListParams,(void *) &Gbl);

      /***** Put icon to print my QR code *****/
      QR_PutLinkToPrintQRCode (ActPrnUsrQR,
			       Usr_PutParamMyUsrCodEncrypted,(void *) Gbl.Usrs.Me.UsrDat.EncryptedUsrCod);
     }
  }

static void Att_PutFormToPrintMyListParams (void *Args)
  {
   if (Args)
     {
      if (Gbl.AttEvents.ShowDetails)
	 Par_PutHiddenParamChar ("ShowDetails",'Y');
      if (Gbl.AttEvents.StrAttCodsSelected)
	 if (Gbl.AttEvents.StrAttCodsSelected[0])
	    Par_PutHiddenParamString (NULL,"AttCods",Gbl.AttEvents.StrAttCodsSelected);
     }
  }

/*****************************************************************************/
/******** Put icon to print assistance of students to several events *********/
/*****************************************************************************/

static void Att_PutIconsStdsAttList (void *Args)
  {
   if (Args)
     {
      /***** Put icon to print assistance of students to several events *****/
      Ico_PutContextualIconToPrint (ActPrnLstUsrAtt,
				    Att_PutParamsToPrintStdsList,(void *) &Gbl);

      /***** Put icon to print my QR code *****/
      QR_PutLinkToPrintQRCode (ActPrnUsrQR,
			       Usr_PutParamMyUsrCodEncrypted,(void *) Gbl.Usrs.Me.UsrDat.EncryptedUsrCod);
     }
  }

static void Att_PutParamsToPrintStdsList (void *Args)
  {
   if (Args)
     {
      if (Gbl.AttEvents.ShowDetails)
	 Par_PutHiddenParamChar ("ShowDetails",'Y');
      Grp_PutParamsCodGrps ();
      Usr_PutHiddenParSelectedUsrsCods (&Gbl.Usrs.Selected);
      if (Gbl.AttEvents.StrAttCodsSelected)
	 if (Gbl.AttEvents.StrAttCodsSelected[0])
	    Par_PutHiddenParamString (NULL,"AttCods",Gbl.AttEvents.StrAttCodsSelected);
     }
  }

/*****************************************************************************/
/**** Put a link (form) to list assistance of students to several events *****/
/*****************************************************************************/

static void Att_PutButtonToShowDetails (void)
  {
   extern const char *Txt_Show_more_details;

   /***** Button to show more details *****/
   Frm_StartFormAnchor (Gbl.Action.Act,Att_ATTENDANCE_DETAILS_ID);
   Par_PutHiddenParamChar ("ShowDetails",'Y');
   Grp_PutParamsCodGrps ();
   Usr_PutHiddenParSelectedUsrsCods (&Gbl.Usrs.Selected);
   if (Gbl.AttEvents.StrAttCodsSelected)
      if (Gbl.AttEvents.StrAttCodsSelected[0])
	 Par_PutHiddenParamString (NULL,"AttCods",Gbl.AttEvents.StrAttCodsSelected);
   Btn_PutConfirmButton (Txt_Show_more_details);
   Frm_EndForm ();
  }

/*****************************************************************************/
/********** Write list of those attendance events that have students *********/
/*****************************************************************************/

static void Att_ListEventsToSelect (void)
  {
   extern const char *The_ClassFormLinkInBoxBold[The_NUM_THEMES];
   extern const char *Txt_Events;
   extern const char *Txt_Event;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Update_attendance;
   unsigned UniqueId;
   char *Id;
   unsigned NumAttEvent;
   bool NormalView = (Att_TypeOfView == Att_VIEW_ONLY_ME ||
                      Att_TypeOfView == Att_VIEW_SEL_USR);

   /***** Begin box *****/
   switch (Att_TypeOfView)
     {
      case Att_VIEW_ONLY_ME:
	 Box_BoxBegin (NULL,Txt_Events,
		       Att_PutIconToViewAttEvents,(void *) &Gbl,
		       NULL,Box_NOT_CLOSABLE);
	 break;
      case Att_VIEW_SEL_USR:
	 Box_BoxBegin (NULL,Txt_Events,
		       Att_PutIconToEditAttEvents,(void *) &Gbl,
		       NULL,Box_NOT_CLOSABLE);
	 break;
      case Att_PRNT_ONLY_ME:
      case Att_PRNT_SEL_USR:
	 Box_BoxBegin (NULL,Txt_Events,
		       NULL,NULL,
		       NULL,Box_NOT_CLOSABLE);
	 break;
     }

   /***** Begin form to update the attendance
	  depending on the events selected *****/
   if (NormalView)
     {
      Frm_StartFormAnchor (Gbl.Action.Act,Att_ATTENDANCE_TABLE_ID);
      Grp_PutParamsCodGrps ();
      Usr_PutHiddenParSelectedUsrsCods (&Gbl.Usrs.Selected);
     }

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

   /***** Heading row *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,4,"LM",Txt_Event);
   HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_Abc[Rol_STD][Usr_SEX_UNKNOWN]);

   HTM_TR_End ();

   /***** List the events *****/
   for (NumAttEvent = 0, UniqueId = 1, Gbl.RowEvenOdd = 0;
	NumAttEvent < Gbl.AttEvents.Num;
	NumAttEvent++, UniqueId++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
     {
      /* Get data of the attendance event from database */
      Att_GetDataOfAttEventByCodAndCheckCrs (&Gbl.AttEvents.Lst[NumAttEvent]);
      Att_GetNumStdsTotalWhoAreInAttEvent (&Gbl.AttEvents.Lst[NumAttEvent]);

      /* Write a row for this event */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT CT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_INPUT_CHECKBOX ("AttCods",HTM_DONT_SUBMIT_ON_CHANGE,
			  "id=\"Att%u\" value=\"%ld\"%s",
			  NumAttEvent,Gbl.AttEvents.Lst[NumAttEvent].AttCod,
			  Gbl.AttEvents.Lst[NumAttEvent].Selected ? " checked=\"checked\"" : "");
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_LABEL_Begin ("for=\"Att%u\"",NumAttEvent);
      HTM_TxtF ("%u:",NumAttEvent + 1);
      HTM_LABEL_End ();
      HTM_TD_End ();

      if (asprintf (&Id,"att_date_start_%u",UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      HTM_TD_Begin ("class=\"DAT LT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_LABEL_Begin ("for=\"Att%u\"",NumAttEvent);
      HTM_SPAN_Begin ("id=\"%s\"",Id);
      HTM_SPAN_End ();
      HTM_LABEL_End ();
      Dat_WriteLocalDateHMSFromUTC (Id,Gbl.AttEvents.Lst[NumAttEvent].TimeUTC[Att_START_TIME],
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				    true,true,true,0x7);
      HTM_TD_End ();
      free (Id);

      HTM_TD_Begin ("class=\"DAT LT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_Txt (Gbl.AttEvents.Lst[NumAttEvent].Title);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_Unsigned (Gbl.AttEvents.Lst[NumAttEvent].NumStdsTotal);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Put button to refresh *****/
   if (NormalView)
     {
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("colspan=\"5\" class=\"CM\"");
      HTM_BUTTON_Animated_Begin (Txt_Update_attendance,
	                         The_ClassFormLinkInBoxBold[Gbl.Prefs.Theme],
				 NULL);
      Ico_PutCalculateIconWithText (Txt_Update_attendance);
      HTM_BUTTON_End ();
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** End form *****/
   if (NormalView)
      Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*********** Put icon to list (without edition) attendance events ************/
/*****************************************************************************/

static void Att_PutIconToViewAttEvents (void *Args)
  {
   if (Args)
      Ico_PutContextualIconToView (ActSeeAtt,
                                   NULL,NULL);
  }

/*****************************************************************************/
/************ Put icon to list (with edition) attendance events **************/
/*****************************************************************************/

static void Att_PutIconToEditAttEvents (void *Args)
  {
   if (Args)
      Ico_PutContextualIconToEdit (ActSeeAtt,NULL,
				   NULL,NULL);
  }

/*****************************************************************************/
/************ Show table with attendances for every user in list *************/
/*****************************************************************************/

static void Att_ListUsrsAttendanceTable (unsigned NumUsrsInList,
                                         long *LstSelectedUsrCods)
  {
   extern const char *Txt_Number_of_users;
   struct UsrData UsrDat;
   unsigned NumUsr;
   unsigned NumAttEvent;
   unsigned Total;
   bool PutButtonShowDetails = (Att_TypeOfView == Att_VIEW_ONLY_ME ||
	                        Att_TypeOfView == Att_VIEW_SEL_USR) &&
	                        !Gbl.AttEvents.ShowDetails;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Start section with attendance table *****/
   HTM_SECTION_Begin (Att_ATTENDANCE_TABLE_ID);

   /***** Begin table *****/
   HTM_TABLE_BeginCenterPadding (2);

   /***** Heading row *****/
   Att_WriteTableHeadSeveralAttEvents ();

   /***** List the users *****/
   for (NumUsr = 0, Gbl.RowEvenOdd = 0;
	NumUsr < NumUsrsInList;
	NumUsr++)
     {
      UsrDat.UsrCod = LstSelectedUsrCods[NumUsr];
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS))		// Get from the database the data of the student
	 if (Usr_CheckIfICanViewAtt (&UsrDat))
	   {
	    UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&UsrDat);
	    Att_WriteRowUsrSeveralAttEvents (NumUsr,&UsrDat);
	   }
     }

   /***** Last row with the total of users present in each event *****/
   if (NumUsrsInList > 1)
     {
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("colspan=\"%u\" class=\"DAT_N_LINE_TOP RM\"",
		    Gbl.Usrs.Listing.WithPhotos ? 4 :
						  3);
      HTM_TxtF ("%s:",Txt_Number_of_users);
      HTM_TD_End ();

      for (NumAttEvent = 0, Total = 0;
	   NumAttEvent < Gbl.AttEvents.Num;
	   NumAttEvent++)
	 if (Gbl.AttEvents.Lst[NumAttEvent].Selected)
	   {
	    HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM\"");
	    HTM_Unsigned (Gbl.AttEvents.Lst[NumAttEvent].NumStdsFromList);
	    HTM_TD_End ();

	    Total += Gbl.AttEvents.Lst[NumAttEvent].NumStdsFromList;
	   }

      HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM\"");
      HTM_Unsigned (Total);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Button to show more details *****/
   if (PutButtonShowDetails)
      Att_PutButtonToShowDetails ();

   /***** End section with attendance table *****/
   HTM_SECTION_End ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/* Write table heading for listing of students in several attendance events **/
/*****************************************************************************/

static void Att_WriteTableHeadSeveralAttEvents (void)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Attendance;
   unsigned NumAttEvent;
   char StrNumAttEvent[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   HTM_TR_Begin (NULL);

   HTM_TH (1,Gbl.Usrs.Listing.WithPhotos ? 4 :
				           3,
           "LM",Txt_ROLES_SINGUL_Abc[Rol_USR][Usr_SEX_UNKNOWN]);

   for (NumAttEvent = 0;
	NumAttEvent < Gbl.AttEvents.Num;
	NumAttEvent++)
      if (Gbl.AttEvents.Lst[NumAttEvent].Selected)
	{
	 /***** Get data of this attendance event *****/
	 Att_GetDataOfAttEventByCodAndCheckCrs (&Gbl.AttEvents.Lst[NumAttEvent]);

	 /***** Put link to this attendance event *****/
	 HTM_TH_Begin (1,1,"CM");
	 snprintf (StrNumAttEvent,sizeof (StrNumAttEvent),
		   "%u",
		   NumAttEvent + 1);
	 Att_PutLinkAttEvent (&Gbl.AttEvents.Lst[NumAttEvent],
			      Gbl.AttEvents.Lst[NumAttEvent].Title,
			      StrNumAttEvent,
			      "BT_LINK TIT_TBL");
	 HTM_TH_End ();
	}

   HTM_TH (1,1,"RM",Txt_Attendance);

   HTM_TR_End ();
  }

/*****************************************************************************/
/************** Write a row of a table with the data of a user ***************/
/*****************************************************************************/

static void Att_WriteRowUsrSeveralAttEvents (unsigned NumUsr,struct UsrData *UsrDat)
  {
   char PhotoURL[PATH_MAX + 1];
   bool ShowPhoto;
   unsigned NumAttEvent;
   bool Present;
   unsigned NumTimesPresent;

   /***** Write number of user in the list *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RM COLOR%u\"",
		 UsrDat->Accepted ? "DAT_N" :
				    "DAT",
		 Gbl.RowEvenOdd);
   HTM_Unsigned (NumUsr + 1);
   HTM_TD_End ();

   /***** Show user's photo *****/
   if (Gbl.Usrs.Listing.WithPhotos)
     {
      HTM_TD_Begin ("class=\"LM COLOR%u\"",Gbl.RowEvenOdd);
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO21x28",Pho_ZOOM,false);
      HTM_TD_End ();
     }

   /***** Write user's ID ******/
   HTM_TD_Begin ("class=\"%s LM COLOR%u\"",
		 UsrDat->Accepted ? "DAT_SMALL_N" :
				    "DAT_SMALL",
		 Gbl.RowEvenOdd);
   ID_WriteUsrIDs (UsrDat,NULL);
   HTM_TD_End ();

   /***** Write user's name *****/
   HTM_TD_Begin ("class=\"%s LM COLOR%u\"",
		 UsrDat->Accepted ? "DAT_SMALL_N" :
				    "DAT_SMALL",
		 Gbl.RowEvenOdd);
   HTM_Txt (UsrDat->Surname1);
   if (UsrDat->Surname2[0])
      HTM_TxtF ("&nbsp;%s",UsrDat->Surname2);
   HTM_TxtF (", %s",UsrDat->FirstName);
   HTM_TD_End ();

   /***** Check/cross to show if the user is present/absent *****/
   for (NumAttEvent = 0, NumTimesPresent = 0;
	NumAttEvent < Gbl.AttEvents.Num;
	NumAttEvent++)
      if (Gbl.AttEvents.Lst[NumAttEvent].Selected)
	{
	 /* Check if this student is already registered in the current event */
	 // Here it is not necessary to get comments
	 Present = Att_CheckIfUsrIsPresentInAttEvent (Gbl.AttEvents.Lst[NumAttEvent].AttCod,
	                                              UsrDat->UsrCod);

	 /* Write check or cross */
	 HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
	 Att_PutCheckOrCross (Present);
	 HTM_TD_End ();

	 if (Present)
	    NumTimesPresent++;
	}

   /***** Last column with the number of times this user is present *****/
   HTM_TD_Begin ("class=\"DAT_N RM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_Unsigned (NumTimesPresent);
   HTM_TD_End ();

   HTM_TR_End ();

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/*********************** Put check or cross character ************************/
/*****************************************************************************/

static void Att_PutCheckOrCross (bool Present)
  {
   extern const char *Txt_Present;
   extern const char *Txt_Absent;

   if (Present)
     {
      HTM_DIV_Begin ("class=\"ATT_CHECK\" title=\"%s\"",Txt_Present);
      HTM_Txt ("&check;");
     }
   else
     {
      HTM_DIV_Begin ("class=\"ATT_CROSS\" title=\"%s\"",Txt_Absent);
      HTM_Txt ("&cross;");
     }
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************** List the students with details and comments ****************/
/*****************************************************************************/

static void Att_ListStdsWithAttEventsDetails (unsigned NumUsrsInList,
                                              long *LstSelectedUsrCods)
  {
   extern const char *Txt_Details;
   struct UsrData UsrDat;
   unsigned NumUsr;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Start section with attendance details *****/
   HTM_SECTION_Begin (Att_ATTENDANCE_DETAILS_ID);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_Details,
                      NULL,NULL,
	              NULL,Box_NOT_CLOSABLE,2);

   /***** List students with attendance details *****/
   for (NumUsr = 0, Gbl.RowEvenOdd = 0;
	NumUsr < NumUsrsInList;
	NumUsr++)
     {
      UsrDat.UsrCod = LstSelectedUsrCods[NumUsr];
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS))	// Get from the database the data of the student
	 if (Usr_CheckIfICanViewAtt (&UsrDat))
	   {
	    UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&UsrDat);
	    Att_ListAttEventsForAStd (NumUsr,&UsrDat);
	   }
     }

   /***** End table and box *****/
   Box_BoxTableEnd ();

   /***** End section with attendance details *****/
   HTM_SECTION_End ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/*************** Write list of attendance events for a student ***************/
/*****************************************************************************/

static void Att_ListAttEventsForAStd (unsigned NumUsr,struct UsrData *UsrDat)
  {
   extern const char *Txt_Student_comment;
   extern const char *Txt_Teachers_comment;
   char PhotoURL[PATH_MAX + 1];
   bool ShowPhoto;
   unsigned NumAttEvent;
   unsigned UniqueId;
   char *Id;
   bool Present;
   bool ShowCommentStd;
   bool ShowCommentTch;
   char CommentStd[Cns_MAX_BYTES_TEXT + 1];
   char CommentTch[Cns_MAX_BYTES_TEXT + 1];

   /***** Write number of student in the list *****/
   NumUsr++;
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RM COLOR%u\"",
		 UsrDat->Accepted ? "DAT_N" :
				    "DAT",
		 Gbl.RowEvenOdd);
   HTM_TxtF ("%u:",NumUsr);
   HTM_TD_End ();

   /***** Show student's photo *****/
   HTM_TD_Begin ("colspan=\"2\" class=\"RM COLOR%u\"",Gbl.RowEvenOdd);
   ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
				        NULL,
		     "PHOTO21x28",Pho_ZOOM,false);
   HTM_TD_End ();

   /***** Write user's ID ******/
   HTM_TD_Begin ("class=\"LM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TABLE_Begin (NULL);
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s LM\"",
		 UsrDat->Accepted ? "DAT_N" :
				    "DAT");
   ID_WriteUsrIDs (UsrDat,NULL);
   HTM_TD_End ();

   /***** Write student's name *****/
   HTM_TD_Begin ("class=\"%s LM\"",
		 UsrDat->Accepted ? "DAT_SMALL_N" :
				    "DAT_SMALL");
   HTM_Txt (UsrDat->Surname1);
   if (UsrDat->Surname2[0])
      HTM_TxtF ("&nbsp;%s",UsrDat->Surname2);
   HTM_TxtF (", %s",UsrDat->FirstName);
   HTM_TD_End ();

   HTM_TR_End ();
   HTM_TABLE_End ();
   HTM_TD_End ();
   HTM_TR_End ();

   /***** List the events with students *****/
   for (NumAttEvent = 0, UniqueId = 1;
	NumAttEvent < Gbl.AttEvents.Num;
	NumAttEvent++, UniqueId++)
      if (Gbl.AttEvents.Lst[NumAttEvent].Selected)
	{
	 /***** Get data of the attendance event from database *****/
	 Att_GetDataOfAttEventByCodAndCheckCrs (&Gbl.AttEvents.Lst[NumAttEvent]);
         Att_GetNumStdsTotalWhoAreInAttEvent (&Gbl.AttEvents.Lst[NumAttEvent]);

	 /***** Get comments for this student *****/
	 Present = Att_CheckIfUsrIsPresentInAttEventAndGetComments (Gbl.AttEvents.Lst[NumAttEvent].AttCod,UsrDat->UsrCod,CommentStd,CommentTch);
         ShowCommentStd = CommentStd[0];
	 ShowCommentTch = CommentTch[0] &&
	                  (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
	                   Gbl.AttEvents.Lst[NumAttEvent].CommentTchVisible);

	 /***** Write a row for this event *****/
	 HTM_TR_Begin (NULL);

	 HTM_TD_ColouredEmpty (1);

	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",
		       Present ? "DAT_GREEN" :
				 "DAT_RED",
		       Gbl.RowEvenOdd);
	 HTM_TxtF ("%u:",NumAttEvent + 1);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);
         Att_PutCheckOrCross (Present);
	 HTM_TD_End ();

	 if (asprintf (&Id,"att_date_start_%u_%u",NumUsr,UniqueId) < 0)
	    Lay_NotEnoughMemoryExit ();
	 HTM_TD_Begin ("class=\"DAT LT COLOR%u\"",Gbl.RowEvenOdd);
	 HTM_SPAN_Begin ("id=\"%s\"",Id);
	 HTM_SPAN_End ();
         HTM_BR ();
	 HTM_Txt (Gbl.AttEvents.Lst[NumAttEvent].Title);
	 Dat_WriteLocalDateHMSFromUTC (Id,Gbl.AttEvents.Lst[NumAttEvent].TimeUTC[Att_START_TIME],
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				       true,true,true,0x7);
	 HTM_TD_End ();
         free (Id);

	 HTM_TR_End ();

	 /***** Write comments for this student *****/
	 if (ShowCommentStd || ShowCommentTch)
	   {
	    HTM_TR_Begin (NULL);

	    HTM_TD_ColouredEmpty (2);

	    HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"DAT LM COLOR%u\"",Gbl.RowEvenOdd);

	    HTM_DL_Begin ();
	    if (ShowCommentStd)
	      {
	       Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				 CommentStd,Cns_MAX_BYTES_TEXT,false);
	       HTM_DT_Begin ();
	       HTM_TxtF ("%s:",Txt_Student_comment);
	       HTM_DT_End ();
	       HTM_DD_Begin ();
	       HTM_Txt (CommentStd);
	       HTM_DD_End ();
	      }
	    if (ShowCommentTch)
	      {
	       Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				 CommentTch,Cns_MAX_BYTES_TEXT,false);
	       HTM_DT_Begin ();
	       HTM_TxtF ("%s:",Txt_Teachers_comment);
	       HTM_DT_End ();
	       HTM_DD_Begin ();
	       HTM_Txt (CommentTch);
	       HTM_DD_End ();
	      }
	    HTM_DL_End ();

	    HTM_TD_End ();

	    HTM_TR_End ();
	   }
	}

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }
