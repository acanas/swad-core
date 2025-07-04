// swad_attendance.c: control of attendance

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
/********************************** Headers **********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <mysql/mysql.h>	// To access MySQL databases
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_attendance.h"
#include "swad_attendance_database.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_group_database.h"
#include "swad_hidden_visible.h"
#include "swad_hierarchy_type.h"
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

#define Att_TYPES_OF_VIEW 4
typedef enum
  {
   Att_VIEW_ONLY_ME,	// View only me
   Att_VIEW_SEL_USR,	// View selected users
   Att_PRNT_ONLY_ME,	// Print only me
   Att_PRNT_SEL_USR,	// Print selected users
  } Att_TypeOfView_t;

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static void Att_ResetEvents (struct Att_Events *Events);

static void Att_ShowAllEvents (struct Att_Events *Events);
static void Att_ParsMyAllGrps (void *Events);
static void Att_PutIconsInListOfEvents (void *Events);
static void Att_PutIconToCreateNewEvent (struct Att_Events *Events);
static void Att_PutParsToCreateNewEvent (void *Events);
static void Att_PutParsToListUsrsAttendance (void *Events);

static void Att_ShowOneEventRow (struct Att_Events *Events,
                                 bool ShowOnlyThisAttEventComplete);
static void Att_WriteEventAuthor (struct Att_Event *Event);
static Dat_StartEndTime_t Att_GetParAttOrder (void);

static void Att_PutFormsToRemEditOneEvent (struct Att_Events *Events,
                                           const char *Anchor);
static void Att_PutPars (void *Events);
static void Att_GetListEvents (struct Att_Events *Events,
                               Att_OrderNewestOldest_t OrderNewestOldest);
static void Att_GetEventDataByCodAndCheckCrs (struct Att_Event *Event);
static void Att_ResetEvent (struct Att_Event *Event);

static void Att_FreeListEvents (struct Att_Events *Events);

static void Att_HideUnhideEvent (HidVis_HiddenOrVisible_t HiddenOrVisible);

static void Att_PutParAttCod (void *Events);

static void Att_ShowLstGrpsToEditEvent (long AttCod);
static void Att_CreateGroups (long AttCod);
static void Att_GetAndWriteNamesOfGrpsAssociatedToEvent (struct Att_Event *Event);

static void Att_ShowEvent (struct Att_Events *Events);
static void Att_PutIconsOneEvent (void *Events);

static void Att_ListEventOnlyMeAsStudent (struct Att_Event *Event);
static void Att_ListEventStudents (struct Att_Events *Events);
static void Att_WriteRowUsrToCallTheRoll (unsigned NumUsr,
                                          struct Usr_Data *UsrDat,
                                          struct Att_Event *Event,
                                          Pho_ShowPhotos_t ShowPhotos);
static void Att_PutLinkEvent (struct Att_Event *Event,
			      const char *Title,const char *Txt);
static unsigned Att_GetNumUsrsFromAListWhoAreInEvent (long AttCod,
						      long *LstSelectedUsrCods,
						      unsigned NumUsrsInList);
static Att_AbsentOrPresent_t Att_CheckIfUsrIsPresentInEvent (long AttCod,long UsrCod);
static Att_AbsentOrPresent_t Att_CheckIfUsrIsPresentInEventAndGetComments (long AttCod,long UsrCod,
								   char CommentStd[Cns_MAX_BYTES_TEXT + 1],
								   char CommentTch[Cns_MAX_BYTES_TEXT + 1]);

static void Att_ReqListOrPrintUsrsAttendanceCrs (__attribute__((unused)) void *Args);
static void Att_ListOrPrintMyAttendanceCrs (Att_TypeOfView_t TypeOfView);
static void Att_GetUsrsAndListOrPrintAttendanceCrs (Att_TypeOfView_t TypeOfView);
static void Att_ListOrPrintUsrsAttendanceCrs (void *TypeOfView);

static void Att_GetListSelectedAttCods (struct Att_Events *Events);

static void Att_PutIconsMyAttList (void *Events);
static void Att_PutFormToPrintMyListPars (void *Events);
static void Att_PutIconsStdsAttList (void *Events);
static void Att_PutParsToPrintStdsList (void *Events);

static void Att_PutButtonToShowDetails (const struct Att_Events *Events);
static void Att_ListEventsToSelect (struct Att_Events *Events,
                                    Att_TypeOfView_t TypeOfView);
static void Att_PutIconToViewAttEvents (__attribute__((unused)) void *Args);
static void Att_PutIconToEditAttEvents (__attribute__((unused)) void *Args);
static void Att_ListUsrsAttendanceTable (struct Att_Events *Events,
                                         Att_TypeOfView_t TypeOfView,
	                                 unsigned NumUsrsInList,
                                         long *LstSelectedUsrCods);
static void Att_WriteTableHeadSeveralAttEvents (struct Att_Events *Events,
						Pho_ShowPhotos_t ShowPhotos);
static void Att_WriteRowUsrSeveralAttEvents (const struct Att_Events *Events,
                                             unsigned NumUsr,struct Usr_Data *UsrDat,
                                             Pho_ShowPhotos_t ShowPhotos);
static void Att_PutCheckOrCross (Att_AbsentOrPresent_t Present);
static void Att_ListStdsWithAttEventsDetails (struct Att_Events *Events,
                                              unsigned NumUsrsInList,
                                              long *LstSelectedUsrCods);
static void Att_ListAttEventsForAStd (struct Att_Events *Events,
                                      unsigned NumUsr,struct Usr_Data *UsrDat);

/*****************************************************************************/
/************************** Reset attendance events **************************/
/*****************************************************************************/

static void Att_ResetEvents (struct Att_Events *Events)
  {
   Events->LstIsRead          = false;		// List is not read from database
   Events->Num                = 0;		// Number of attendance events
   Events->Lst                = NULL;		// List of attendance events
   Events->SelectedOrder      = Att_ORDER_DEFAULT;
   Events->Event.AttCod       = -1L;
   Events->ShowDetails        = false;
   Events->StrAttCodsSelected = NULL;
   Events->CurrentPage        = 0;
  }

/*****************************************************************************/
/************************ List all attendance events *************************/
/*****************************************************************************/

void Att_ShowEvents (void)
  {
   struct Att_Events Events;

   /***** Reset attendance events *****/
   Att_ResetEvents (&Events);

   /***** Get parameters *****/
   Events.SelectedOrder = Att_GetParAttOrder ();
   Grp_GetParMyAllGrps ();
   Events.CurrentPage = Pag_GetParPagNum (Pag_ATT_EVENTS);

   /***** Get list of attendance events *****/
   Att_GetListEvents (&Events,Att_NEWEST_FIRST);

   /***** Show all attendance events *****/
   Att_ShowAllEvents (&Events);
  }

/*****************************************************************************/
/************************* Show all attendance events ************************/
/*****************************************************************************/

static void Att_ShowAllEvents (struct Att_Events *Events)
  {
   extern const char *Hlp_USERS_Attendance;
   extern const char *Txt_Events;
   extern const char *Txt_START_END_TIME_HELP[Dat_NUM_START_END_TIME];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Event;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_No_events;
   struct Pag_Pagination Pagination;
   Dat_StartEndTime_t Order;
   Grp_MyAllGrps_t MyAllGrps;
   unsigned NumAttEvent;

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Events->Num;
   Pagination.CurrentPage = Events->CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Events->CurrentPage = Pagination.CurrentPage;

   /***** Begin box *****/
   Box_BoxBegin (Txt_Events,Att_PutIconsInListOfEvents,Events,
		 Hlp_USERS_Attendance,Box_NOT_CLOSABLE);

      /***** Select whether show only my groups or all groups *****/
      if (Gbl.Crs.Grps.NumGrps)
	{
	 Set_BeginSettingsHead ();
	    Grp_ShowFormToSelMyAllGrps (ActSeeAllAtt,
					Att_ParsMyAllGrps,&Events);
	 Set_EndSettingsHead ();
	}

      /***** Write links to pages *****/
      Pag_WriteLinksToPagesCentered (Pag_ATT_EVENTS,&Pagination,
				     Events,-1L);

      if (Events->Num)
	{
	 /***** Begin table *****/
	 HTM_TABLE_Begin ("TBL_SCROLL");

	    /***** Table head *****/
	    HTM_TR_Begin (NULL);

	       HTM_TH_Span (NULL,HTM_HEAD_CENTER,1,1,"CONTEXT_COL");	// Column for contextual icons
	       for (Order  = (Dat_StartEndTime_t) 0;
		    Order <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
		    Order++)
		 {
                  HTM_TH_Begin (HTM_HEAD_LEFT);

		     Frm_BeginForm (ActSeeAllAtt);
			MyAllGrps = Grp_GetParMyAllGrps ();
			Grp_PutParMyAllGrps (&MyAllGrps);
			Pag_PutParPagNum (Pag_ATT_EVENTS,Events->CurrentPage);
			Par_PutParOrder ((unsigned) Order);

			HTM_BUTTON_Submit_Begin (Txt_START_END_TIME_HELP[Order],NULL,
			                         "class=\"BT_LINK\"");

			   if (Order == Events->SelectedOrder)
			      HTM_U_Begin ();
			   HTM_Txt (Txt_START_END_TIME[Order]);
			   if (Order == Events->SelectedOrder)
			      HTM_U_End ();

			HTM_BUTTON_End ();

		     Frm_EndForm ();

		  HTM_TH_End ();
		 }
	       HTM_TH (Txt_Event                                     ,HTM_HEAD_LEFT );
	       HTM_TH (Txt_ROLES_PLURAL_Abc[Rol_STD][Usr_SEX_UNKNOWN],HTM_HEAD_RIGHT);

	    HTM_TR_End ();

	    /***** Write all attendance events *****/
	    for (NumAttEvent  = Pagination.FirstItemVisible, The_ResetRowColor ();
		 NumAttEvent <= Pagination.LastItemVisible;
		 NumAttEvent++, The_ChangeRowColor ())
	      {
	       /***** Get data of this attendance event *****/
	       Events->Event.AttCod = Events->Lst[NumAttEvent - 1].AttCod;
	       Att_GetEventDataByCodAndCheckCrs (&Events->Event);
	       Events->Event.NumStdsTotal = Att_DB_GetNumStdsTotalWhoAreInEvent (Events->Event.AttCod);

	       /***** Show one attendance event *****/
	       Att_ShowOneEventRow (Events,
				    false);	// Don't show only this event
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();
	}
      else	// No events created
	 Ale_ShowAlert (Ale_INFO,Txt_No_events);

      /***** Write again links to pages *****/
      Pag_WriteLinksToPagesCentered (Pag_ATT_EVENTS,&Pagination,
				     Events,-1L);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of attendance events *****/
   Att_FreeListEvents (Events);
  }

/*****************************************************************************/
/***************** Put params to select which groups to show *****************/
/*****************************************************************************/

static void Att_ParsMyAllGrps (void *Events)
  {
   if (Events)
     {
      Par_PutParOrder ((unsigned) ((struct Att_Events *) Events)->SelectedOrder);
      Pag_PutParPagNum (Pag_ATT_EVENTS,((struct Att_Events *) Events)->CurrentPage);
     }
  }

/*****************************************************************************/
/************* Put contextual icons in list of attendance events *************/
/*****************************************************************************/

static void Att_PutIconsInListOfEvents (void *Events)
  {
   Usr_Can_t ICanEdit;

   if (Events)
     {
      /***** Put icon to create a new attendance event *****/
      ICanEdit = (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
		  Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM) ? Usr_CAN :
							    Usr_CAN_NOT;
      if (ICanEdit == Usr_CAN)
	 Att_PutIconToCreateNewEvent ((struct Att_Events *) Events);

      /***** Put icon to show attendance list *****/
      if (((struct Att_Events *) Events)->Num)
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
							  Att_PutParsToListUsrsAttendance,Events);
	       break;
	    default:
	       break;
	   }

      /***** Put icon to print my QR code *****/
      QR_PutLinkToPrintQRCode (ActPrnUsrQR,
			       Usr_PutParMyUsrCodEncrypted,Gbl.Usrs.Me.UsrDat.EnUsrCod);

      /***** Put icon to get resource link *****/
      Ico_PutContextualIconToGetLink (ActReqLnkAtt,NULL,Att_PutPars,Events);
     }
  }

/*****************************************************************************/
/**************** Put icon to create a new attendance event ******************/
/*****************************************************************************/

static void Att_PutIconToCreateNewEvent (struct Att_Events *Events)
  {
   Ico_PutContextualIconToAdd (ActFrmNewAtt,NULL,
                               Att_PutParsToCreateNewEvent,Events);
  }

/*****************************************************************************/
/************** Put parameters to create a new attendance event **************/
/*****************************************************************************/

static void Att_PutParsToCreateNewEvent (void *Events)
  {
   Grp_MyAllGrps_t MyAllGrps;

   if (Events)
     {
      Par_PutParOrder ((unsigned) ((struct Att_Events *) Events)->SelectedOrder);
      MyAllGrps = Grp_GetParMyAllGrps ();
      Grp_PutParMyAllGrps (&MyAllGrps);
      Pag_PutParPagNum (Pag_ATT_EVENTS,((struct Att_Events *) Events)->CurrentPage);
     }
  }

/*****************************************************************************/
/***************** Put parameters to list users attendance *******************/
/*****************************************************************************/

static void Att_PutParsToListUsrsAttendance (void *Events)
  {
   Grp_MyAllGrps_t MyAllGrps;

   if (Events)
     {
      Par_PutParOrder ((unsigned) ((struct Att_Events *) Events)->SelectedOrder);
      MyAllGrps = Grp_GetParMyAllGrps ();
      Grp_PutParMyAllGrps (&MyAllGrps);
      Pag_PutParPagNum (Pag_ATT_EVENTS,((struct Att_Events *) Events)->CurrentPage);
     }
  }

/*****************************************************************************/
/************************* Show one attendance event *************************/
/*****************************************************************************/
// Only Event->AttCod must be filled

static void Att_ShowOneEventRow (struct Att_Events *Events,
                                 bool ShowOnlyThisAttEventComplete)
  {
   extern const char *CloOpe_Class[CloOpe_NUM_CLOSED_OPEN][HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_TitleClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   char *Id;
   Dat_StartEndTime_t StartEndTime;
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Events->Event.AttCod,&Anchor);

   /***** Write first row of data of this attendance event *****/
   /* Forms to remove/edit this attendance event */
   HTM_TR_Begin (NULL);

      if (!ShowOnlyThisAttEventComplete)
	{
	 HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL %s\"",
	               The_GetColorRows ());
            Att_PutFormsToRemEditOneEvent (Events,Anchor);
         HTM_TD_End ();
	}

      /* Start/end date/time */
      UniqueId++;
      for (StartEndTime  = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
	{
	 if (asprintf (&Id,"att_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	    Err_NotEnoughMemoryExit ();
	 if (ShowOnlyThisAttEventComplete)
	    HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s\"",
			  Id,
			  CloOpe_Class[Events->Event.ClosedOrOpen][Events->Event.Hidden],
			  The_GetSuffix ());
	 else
	    HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
			  Id,
			  CloOpe_Class[Events->Event.ClosedOrOpen][Events->Event.Hidden],
			  The_GetSuffix (),The_GetColorRows ());
	 Dat_WriteLocalDateHMSFromUTC (Id,Events->Event.TimeUTC[StartEndTime],
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       Dat_WRITE_TODAY |
				       Dat_WRITE_DATE_ON_SAME_DAY |
				       Dat_WRITE_WEEK_DAY |
				       Dat_WRITE_HOUR |
				       Dat_WRITE_MINUTE |
				       Dat_WRITE_SECOND);
	 HTM_TD_End ();
	 free (Id);
	}

      /* Attendance event title */
      if (ShowOnlyThisAttEventComplete)
	 HTM_TD_Begin ("class=\"LT\"");
      else
	 HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
      HTM_ARTICLE_Begin (Anchor);
	 Att_PutLinkEvent (&Events->Event,Act_GetActionText (ActSeeOneAtt),
			   Events->Event.Title);
      HTM_ARTICLE_End ();
      HTM_TD_End ();

      /* Number of students in this event */
      if (ShowOnlyThisAttEventComplete)
	 HTM_TD_Begin ("class=\"RT\"");
      else
	 HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());
      HTM_SPAN_Begin ("class=\"%s_%s\"",
		       HidVis_TitleClass[Events->Event.Hidden],The_GetSuffix ());
         HTM_Unsigned (Events->Event.NumStdsTotal);
      HTM_SPAN_End ();
      HTM_TD_End ();

   HTM_TR_End ();

   /***** Write second row of data of this attendance event *****/
   HTM_TR_Begin (NULL);

      /* Author of the attendance event */
      if (ShowOnlyThisAttEventComplete)
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
      else
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",The_GetColorRows ());
      Att_WriteEventAuthor (&Events->Event);
      HTM_TD_End ();

      /* Text of the attendance event */
      Att_DB_GetEventDescription (Events->Event.AttCod,Description);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			Description,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
      ALn_InsertLinks (Description,Cns_MAX_BYTES_TEXT,60);	// Insert links
      if (ShowOnlyThisAttEventComplete)
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
      else
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",The_GetColorRows ());
      if (Gbl.Crs.Grps.NumGrps)
	 Att_GetAndWriteNamesOfGrpsAssociatedToEvent (&Events->Event);

      HTM_DIV_Begin ("class=\"%s_%s\"",
                     HidVis_DataClass[Events->Event.Hidden],The_GetSuffix ());
	 HTM_Txt (Description);
      HTM_DIV_End ();

      HTM_TD_End ();

   HTM_TR_End ();

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (&Anchor);
  }

/*****************************************************************************/
/****************** Write the author of an attendance event ******************/
/*****************************************************************************/

static void Att_WriteEventAuthor (struct Att_Event *Event)
  {
   Usr_WriteAuthor1Line (Event->UsrCod,Event->Hidden);
  }

/*****************************************************************************/
/**** Get parameter with the type or order in list of attendance events ******/
/*****************************************************************************/

static Dat_StartEndTime_t Att_GetParAttOrder (void)
  {
   return (Dat_StartEndTime_t)
	  Par_GetParUnsignedLong ("Order",
				  0,
				  Dat_NUM_START_END_TIME - 1,
				  (unsigned long) Att_ORDER_DEFAULT);
  }

/*****************************************************************************/
/************** Put a link (form) to edit one attendance event ***************/
/*****************************************************************************/

static void Att_PutFormsToRemEditOneEvent (struct Att_Events *Events,
                                           const char *Anchor)
  {
   static Act_Action_t ActionHideUnhide[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = ActUnhAtt,	// Hidden ==> action to unhide
      [HidVis_VISIBLE] = ActHidAtt,	// Visible ==> action to hide
     };

   if (Att_CheckIfICanEditEvents () == Usr_CAN)
     {
      /***** Icon to remove attendance event *****/
      Ico_PutContextualIconToRemove (ActReqRemAtt,NULL,
				     Att_PutPars,Events);

      /***** Icon to hide/unhide attendance event *****/
      Ico_PutContextualIconToHideUnhide (ActionHideUnhide,Anchor,
					 Att_PutPars,Events,
					 Events->Event.Hidden);

      /***** Icon to edit attendance event *****/
      Ico_PutContextualIconToEdit (ActEdiOneAtt,NULL,Att_PutPars,Events);
     }

   /***** Icon to get resource link *****/
   Ico_PutContextualIconToGetLink (ActReqLnkAtt,NULL,Att_PutPars,Events);
  }

/*****************************************************************************/
/******************* Check if I can edit calls for exams *********************/
/*****************************************************************************/

Usr_Can_t Att_CheckIfICanEditEvents (void)
  {
   static Usr_Can_t ICanEditAttEvents[Rol_NUM_ROLES] =
     {
      [Rol_TCH    ] = Usr_CAN,
      [Rol_SYS_ADM] = Usr_CAN,
     };

   return ICanEditAttEvents[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/***************** Params used to edit an attendance event *******************/
/*****************************************************************************/

static void Att_PutPars (void *Events)
  {
   Grp_MyAllGrps_t MyAllGrps;

   if (Events)
     {
      ParCod_PutPar (ParCod_Att,((struct Att_Events *) Events)->Event.AttCod);
      Par_PutParOrder ((unsigned) ((struct Att_Events *) Events)->SelectedOrder);
      MyAllGrps = Grp_GetParMyAllGrps ();
      Grp_PutParMyAllGrps (&MyAllGrps);
      Pag_PutParPagNum (Pag_ATT_EVENTS,((struct Att_Events *) Events)->CurrentPage);
     }
  }

/*****************************************************************************/
/************************ List all attendance events *************************/
/*****************************************************************************/

static void Att_GetListEvents (struct Att_Events *Events,
                               Att_OrderNewestOldest_t OrderNewestOldest)
  {
   extern unsigned (*Att_DB_GetListAttEvents[Grp_NUM_MY_ALL_GROUPS]) (MYSQL_RES **mysql_res,
							              Dat_StartEndTime_t SelectedOrder,
							              Att_OrderNewestOldest_t OrderNewestOldest);
   MYSQL_RES *mysql_res;
   unsigned NumAttEvent;

   if (Events->LstIsRead)
      Att_FreeListEvents (Events);

   /***** Get list of attendance events from database *****/
   Events->Num = Att_DB_GetListAttEvents[Gbl.Crs.Grps.MyAllGrps] (&mysql_res,
	                                                          Events->SelectedOrder,
	                                                          OrderNewestOldest);
   if (Events->Num) // Attendance events found...
     {
      /***** Create list of attendance events *****/
      if ((Events->Lst = calloc ((size_t) Events->Num,
                                 sizeof (*Events->Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the attendance events codes *****/
      for (NumAttEvent = 0;
	   NumAttEvent < Events->Num;
	   NumAttEvent++)
        {
         /* Get next attendance event code */
         if ((Events->Lst[NumAttEvent].AttCod = DB_GetNextCode (mysql_res)) < 0)
            Err_WrongEventExit ();
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Events->LstIsRead = true;
  }

/*****************************************************************************/
/********* Get attendance event data using its code and check course *********/
/*****************************************************************************/

static void Att_GetEventDataByCodAndCheckCrs (struct Att_Event *Event)
  {
   if (Att_GetEventDataByCod (Event))
     {
      if (Event->CrsCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
         Err_WrongEventExit ();
     }
   else	// Attendance event not found
      Err_WrongEventExit ();
  }

/*****************************************************************************/
/**************** Get attendance event data using its code *******************/
/*****************************************************************************/
// Returns true if attendance event exists
// This function can be called from web service, so do not display messages

bool Att_GetEventDataByCod (struct Att_Event *Event)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAttEvents;
   bool Found = false;

   /***** Reset attendance event data *****/
   Att_ResetEvent (Event);

   if (Event->AttCod > 0)
     {
      /***** Build query *****/
      NumAttEvents = Att_DB_GetEventDataByCod (&mysql_res,Event->AttCod);

      /***** Get data of attendance event from database *****/
      if ((Found = (NumAttEvents != 0))) // Attendance event found...
	{
         /* Get next row from result */
	 row = mysql_fetch_row (mysql_res);

	 /* Get attendance event (except Txt) */
	 Att_GetEventDataFromRow (row,Event);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return Found;
  }

/*****************************************************************************/
/********************** Clear all attendance event data **********************/
/*****************************************************************************/

static void Att_ResetEvent (struct Att_Event *Event)
  {
   if (Event->AttCod <= 0)	// If > 0 ==> keep values of AttCod and Selected
     {
      Event->AttCod = -1L;
      Event->NumStdsTotal = 0;
     }
   Event->CrsCod = -1L;
   Event->Hidden = HidVis_VISIBLE;
   Event->UsrCod = -1L;
   Event->TimeUTC[Dat_STR_TIME] =
   Event->TimeUTC[Dat_END_TIME] = (time_t) 0;
   Event->ClosedOrOpen = CloOpe_CLOSED;
   Event->CommentTchVisible = HidVis_HIDDEN;
   Event->Title[0] = '\0';
  }

/*****************************************************************************/
/************************* Get attendance event data *************************/
/*****************************************************************************/

void Att_GetEventDataFromRow (MYSQL_ROW row,struct Att_Event *Event)
  {
   /***** Get code of attendance event (row[0]) and code of course (row[1]) *****/
   Event->AttCod = Str_ConvertStrCodToLongCod (row[0]);
   Event->CrsCod = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get whether the attendance event is hidden or not (row[2]) *****/
   Event->Hidden = HidVis_GetHiddenFromYN (row[2][0]);

   /***** Get author of the attendance event (row[3]) *****/
   Event->UsrCod = Str_ConvertStrCodToLongCod (row[3]);

   /***** Get start date (row[4]) and end date (row[5]) in UTC time *****/
   Event->TimeUTC[Dat_STR_TIME] = Dat_GetUNIXTimeFromStr (row[4]);
   Event->TimeUTC[Dat_END_TIME] = Dat_GetUNIXTimeFromStr (row[5]);

   /***** Get whether the attendance event is open or closed (row(6)) *****/
   Event->ClosedOrOpen = CloOpe_GetClosedOrOpenFrom01 (row[6][0]);

   /***** Get whether the attendance event is visible or not (row[7]) *****/
   Event->CommentTchVisible = HidVis_GetVisibleFromYN (row[7][0]);

   /***** Get the title of the attendance event (row[8]) *****/
   Str_Copy (Event->Title,row[8],sizeof (Event->Title) - 1);
  }

/*****************************************************************************/
/********************** Free list of attendance events ***********************/
/*****************************************************************************/

static void Att_FreeListEvents (struct Att_Events *Events)
  {
   if (Events->LstIsRead && Events->Lst)
     {
      /***** Free memory used by the list of attendance events *****/
      free (Events->Lst);
      Events->Lst       = NULL;
      Events->Num       = 0;
      Events->LstIsRead = false;
     }
  }

/*****************************************************************************/
/********* Ask for confirmation of removing of an attendance event ***********/
/*****************************************************************************/

void Att_AskRemEvent (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_event_X;
   struct Att_Events Events;

   /***** Reset attendance events *****/
   Att_ResetEvents (&Events);

   /***** Get parameters *****/
   Events.SelectedOrder = Att_GetParAttOrder ();
   Grp_GetParMyAllGrps ();
   Events.CurrentPage = Pag_GetParPagNum (Pag_ATT_EVENTS);

   /***** Get attendance event code *****/
   Events.Event.AttCod = ParCod_GetAndCheckPar (ParCod_Att);

   /***** Get data of the attendance event from database *****/
   Att_GetEventDataByCodAndCheckCrs (&Events.Event);

   /***** Show question and button to remove event *****/
   Ale_ShowAlertRemove (ActRemAtt,NULL,
                        Att_PutPars,&Events,
			Txt_Do_you_really_want_to_remove_the_event_X,
	                Events.Event.Title);

   /***** Show attendance events again *****/
   Att_ShowEvents ();
  }

/*****************************************************************************/
/** Get param., remove an attendance event and show attendance events again **/
/*****************************************************************************/

void Att_GetAndRemEvent (void)
  {
   extern const char *Txt_Event_X_removed;
   struct Att_Event Event;

   /***** Get attendance event code *****/
   Event.AttCod = ParCod_GetAndCheckPar (ParCod_Att);

   /***** Get data of the attendance event from database *****/
   // Inside this function, the course is checked to be the current one
   Att_GetEventDataByCodAndCheckCrs (&Event);

   /***** Remove the attendance event from database *****/
   Att_RemoveEventFromDB (Event.AttCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Event_X_removed,
	          Event.Title);

   /***** Show attendance events again *****/
   Att_ShowEvents ();
  }

/*****************************************************************************/
/**************** Remove an attendance event from database *******************/
/*****************************************************************************/

void Att_RemoveEventFromDB (long AttCod)
  {
   /***** Remove users registered in the attendance event *****/
   Att_DB_RemoveAllUsrsFromAnEvent (AttCod);

   /***** Remove all groups of this attendance event *****/
   Att_DB_RemoveGrpsAssociatedToAnEvent (AttCod);

   /***** Remove attendance event *****/
   Att_DB_RemoveEventFromCurrentCrs (AttCod);
  }

/*****************************************************************************/
/********************* Hide/unhide an attendance event ***********************/
/*****************************************************************************/

void Att_HideEvent (void)
  {
   Att_HideUnhideEvent (HidVis_HIDDEN);
  }

void Att_UnhideEvent (void)
  {
   Att_HideUnhideEvent (HidVis_VISIBLE);
  }

static void Att_HideUnhideEvent (HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   struct Att_Event Event;

   /***** Get attendance event code *****/
   Event.AttCod = ParCod_GetAndCheckPar (ParCod_Att);

   /***** Get data of the attendance event from database *****/
   Att_GetEventDataByCodAndCheckCrs (&Event);

   /***** Unhide attendance event *****/
   Att_DB_HideOrUnhideEvent (Event.AttCod,HiddenOrVisible);

   /***** Show attendance events again *****/
   Att_ShowEvents ();
  }

/*****************************************************************************/
/*************** Put a form to create a new attendance event *****************/
/*****************************************************************************/

void Att_ReqCreatOrEditEvent (void)
  {
   extern const char HidVis_Visible_YN[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Hlp_USERS_Attendance_edit_event;
   extern const char *Txt_Hidden_MALE_PLURAL;
   extern const char *Txt_Visible_MALE_PLURAL;
   extern const char *Txt_Event;
   extern const char *Txt_Teachers_comment;
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME] = Dat_HMS_DO_NOT_SET
     };
   static const char **CommentTchVisibleTxt[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = &Txt_Hidden_MALE_PLURAL,
      [HidVis_VISIBLE] = &Txt_Visible_MALE_PLURAL
     };
   struct Att_Events Events;
   OldNew_OldNew_t OldNewEvent;
   Grp_MyAllGrps_t MyAllGrps;
   HidVis_HiddenOrVisible_t HiddenOrVisible;
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset attendance events *****/
   Att_ResetEvents (&Events);

   /***** Get parameters *****/
   Events.SelectedOrder = Att_GetParAttOrder ();
   Grp_GetParMyAllGrps ();
   Events.CurrentPage = Pag_GetParPagNum (Pag_ATT_EVENTS);

   /***** Get the code of the attendance event *****/
   OldNewEvent = ((Events.Event.AttCod = ParCod_GetPar (ParCod_Att)) > 0) ? OldNew_OLD :
									    OldNew_NEW;

   /***** Get from the database the data of the attendance event *****/
   switch (OldNewEvent)
     {
      case OldNew_OLD:
	 /* Get data of the attendance event from database */
	 Att_GetEventDataByCodAndCheckCrs (&Events.Event);

	 /* Get text of the attendance event from database */
	 Att_DB_GetEventDescription (Events.Event.AttCod,Description);
	 break;
      case OldNew_NEW:
      default:
	 /* Reset attendance event data */
	 Events.Event.AttCod = -1L;
	 Att_ResetEvent (&Events.Event);

	 /* Initialize some fields */
	 Events.Event.CrsCod = Gbl.Hierarchy.Node[Hie_CRS].HieCod;
	 Events.Event.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
	 Events.Event.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
	 Events.Event.TimeUTC[Dat_END_TIME] = Events.Event.TimeUTC[Dat_STR_TIME] + (2 * 60 * 60);	// +2 hours
	 Events.Event.ClosedOrOpen = CloOpe_OPEN;
	 Description[0] = '\0';
	 break;
     }

   /***** Begin form *****/
   switch (OldNewEvent)
     {
      case OldNew_OLD:
	 Frm_BeginForm (ActChgAtt);
	    ParCod_PutPar (ParCod_Att,Events.Event.AttCod);
	 break;
      case OldNew_NEW:
      default:
	 Frm_BeginForm (ActNewAtt);
	 break;
     }

      Par_PutParOrder ((unsigned) Events.SelectedOrder);
      MyAllGrps = Grp_GetParMyAllGrps ();
      Grp_PutParMyAllGrps (&MyAllGrps);
      Pag_PutParPagNum (Pag_ATT_EVENTS,Events.CurrentPage);

      /***** Begin box and table *****/
      Box_BoxTableBegin (Events.Event.Title[0] ? Events.Event.Title :
						 Txt_Event,
			 NULL,NULL,
			 Hlp_USERS_Attendance_edit_event,Box_NOT_CLOSABLE,2);

	 /***** Attendance event title *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","Title",Txt_Title);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	       HTM_INPUT_TEXT ("Title",Att_MAX_CHARS_ATTENDANCE_EVENT_TITLE,Events.Event.Title,
			       HTM_REQUIRED,
			       "id=\"Title\" class=\"Frm_C2_INPUT INPUT_%s\"",
			       The_GetSuffix ());
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Assignment start and end dates *****/
	 Dat_PutFormStartEndClientLocalDateTimes (Events.Event.TimeUTC,
						  Dat_FORM_SECONDS_ON,
						  SetHMS);

	 /***** Visibility of comments *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","ComTchVisible",Txt_Teachers_comment);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	       HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
				 "id=\"ComTchVisible\" name=\"ComTchVisible\""
				 " class=\"Frm_C2_INPUT INPUT_%s\"",
				 The_GetSuffix ());
	          for (HiddenOrVisible  = (HidVis_HiddenOrVisible_t) 0;
	               HiddenOrVisible <= (HidVis_HiddenOrVisible_t) (HidVis_NUM_HIDDEN_VISIBLE - 1);
	               HiddenOrVisible++)
		     HTM_OPTION (HTM_Type_CHAR,&HidVis_Visible_YN[HiddenOrVisible],
				 Events.Event.CommentTchVisible == HiddenOrVisible ? HTM_SELECTED :
										     HTM_NO_ATTR,
				 "%s",*CommentTchVisibleTxt[HiddenOrVisible]);
	       HTM_SELECT_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Attendance event description *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","Txt",Txt_Description);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	       HTM_TEXTAREA_Begin (HTM_NO_ATTR,
				   "id=\"Txt\" name=\"Txt\" rows=\"5\""
				   " class=\"Frm_C2_INPUT INPUT_%s\"",
				   The_GetSuffix ());
		  HTM_Txt (Description);
	       HTM_TEXTAREA_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Groups *****/
	 Att_ShowLstGrpsToEditEvent (Events.Event.AttCod);

      /***** End table, send button and end box *****/
      Box_BoxTableWithButtonSaveCreateEnd (OldNewEvent);

   /***** End form *****/
   Frm_EndForm ();

   /***** Show current attendance events *****/
   Att_GetListEvents (&Events,Att_NEWEST_FIRST);
   Att_ShowAllEvents (&Events);
  }

/*****************************************************************************/
/************* Show list of groups to edit and attendance event **************/
/*****************************************************************************/

static void Att_ShowLstGrpsToEditEvent (long AttCod)
  {
   extern const char *Txt_Groups;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_GRP_TYPES_WITH_GROUPS);

   if (Gbl.Crs.Grps.GrpTypes.NumGrpTypes)
     {
      /***** Begin box and table *****/
      HTM_TR_Begin (NULL);

         /* Label */
	 Frm_LabelColumn ("Frm_C1 RT","",Txt_Groups);

	 /* Groups */
	 HTM_TD_Begin ("class=\"Frm_C2 LT\"");

	    /***** First row: checkbox to select the whole course *****/
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_CHECKBOX ("WholeCrs",
				   Grp_DB_CheckIfAssociatedToGrps ("att_groups",
								   "AttCod",
								   AttCod) ? HTM_NO_ATTR :
									     HTM_CHECKED,
				   "id=\"WholeCrs\" value=\"Y\""
				   " onclick=\"uncheckChildren(this,'GrpCods')\"");
	       Grp_WriteTheWholeCourse ();
	    HTM_LABEL_End ();

	    /***** List the groups for each group type *****/
	    Grp_ListGrpsToEditAsgAttSvyEvtMch (Grp_ATT_EVENT,AttCod);

	 HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/*************** Receive form to create a new attendance event ***************/
/*****************************************************************************/

void Att_ReceiveEvent (void)
  {
   extern const char *Txt_Already_existed_an_event_with_the_title_X;
   extern const char *Txt_Created_new_event_X;
   extern const char *Txt_The_event_has_been_modified;
   struct Att_Event OldAtt;
   struct Att_Event ReceivedAtt;
   OldNew_OldNew_t OldNewEvent;
   bool ReceivedAttEventIsCorrect = true;
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Get the code of the attendance event *****/
   OldNewEvent = ((ReceivedAtt.AttCod = ParCod_GetPar (ParCod_Att)) > 0) ? OldNew_OLD :
									   OldNew_NEW;

   if (OldNewEvent == OldNew_OLD)
     {
      /* Get data of the old (current) attendance event from database */
      OldAtt.AttCod = ReceivedAtt.AttCod;
      Att_GetEventDataByCodAndCheckCrs (&OldAtt);
      ReceivedAtt.Hidden = OldAtt.Hidden;
     }

   /***** Get start/end date-times *****/
   ReceivedAtt.TimeUTC[Dat_STR_TIME] = Dat_GetTimeUTCFromForm (Dat_STR_TIME);
   ReceivedAtt.TimeUTC[Dat_END_TIME] = Dat_GetTimeUTCFromForm (Dat_END_TIME);

   /***** Get boolean parameter that indicates if teacher's comments are visible by students *****/
   ReceivedAtt.CommentTchVisible = Par_GetParBool ("ComTchVisible") ? HidVis_VISIBLE :
								      HidVis_HIDDEN;

   /***** Get attendance event title *****/
   Par_GetParText ("Title",ReceivedAtt.Title,Att_MAX_BYTES_ATTENDANCE_EVENT_TITLE);

   /***** Get attendance event description *****/
   Par_GetParHTML ("Txt",Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (ReceivedAtt.TimeUTC[Dat_STR_TIME] == 0)
      ReceivedAtt.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
   if (ReceivedAtt.TimeUTC[Dat_END_TIME] == 0)
      ReceivedAtt.TimeUTC[Dat_END_TIME] = ReceivedAtt.TimeUTC[Dat_STR_TIME] + 2 * 60 * 60;	// +2 hours // TODO: 2 * 60 * 60 should be in a #define in swad_config.h

   /***** Check if title is correct *****/
   if (ReceivedAtt.Title[0])	// If there's an attendance event title
     {
      /* If title of attendance event was in database... */
      if (Att_DB_CheckIfSimilarEventExists ("Title",
					    ReceivedAtt.Title,
					    ReceivedAtt.AttCod) == Exi_EXISTS)
        {
         ReceivedAttEventIsCorrect = false;
	 Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_an_event_with_the_title_X,
                        ReceivedAtt.Title);
        }
     }
   else	// If there is not an attendance event title
     {
      ReceivedAttEventIsCorrect = false;
      Ale_CreateAlertYouMustSpecifyTheTitle ();
     }

   /***** Create a new attendance event or update an existing one *****/
   if (ReceivedAttEventIsCorrect)
     {
      /* Get groups for this attendance events */
      Grp_GetParCodsSeveralGrps ();

      switch (OldNewEvent)
	{
	 case OldNew_OLD:
	    Att_UpdateEvent (&ReceivedAtt,Description);

	    /***** Write success message *****/
	    Ale_ShowAlert (Ale_SUCCESS,Txt_The_event_has_been_modified);
	    break;
	 case OldNew_NEW:
	 default:
	    ReceivedAtt.Hidden = HidVis_VISIBLE;	// New attendance events are visible by default
	    Att_CreateEvent (&ReceivedAtt,Description);		// Add new attendance event to database

	    /***** Write success message *****/
	    Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_event_X,
			   ReceivedAtt.Title);
	    break;
	}

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();
     }
   else
      Att_ReqCreatOrEditEvent ();

   /***** Show attendance events again *****/
   Att_ShowEvents ();
  }

/*****************************************************************************/
/********************* Create a new attendance event *************************/
/*****************************************************************************/

void Att_CreateEvent (struct Att_Event *Event,const char *Description)
  {
   /***** Create a new attendance event *****/
   Event->AttCod = Att_DB_CreateEvent (Event,Description);

   /***** Create groups *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Att_CreateGroups (Event->AttCod);
  }

/*****************************************************************************/
/****************** Update an existing attendance event **********************/
/*****************************************************************************/

void Att_UpdateEvent (struct Att_Event *Event,const char *Description)
  {
   /***** Update the data of the attendance event *****/
   Att_DB_UpdateEvent (Event,Description);

   /***** Update groups *****/
   /* Remove old groups */
   Att_DB_RemoveGrpsAssociatedToAnEvent (Event->AttCod);

   /* Create new groups */
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Att_CreateGroups (Event->AttCod);
  }

/*****************************************************************************/
/***************** Create groups of an attendance event **********************/
/*****************************************************************************/

static void Att_CreateGroups (long AttCod)
  {
   unsigned NumGrp;

   /***** Create groups of the attendance event *****/
   for (NumGrp = 0;
	NumGrp < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrp++)
      Att_DB_CreateGroupAssociatedToEvent (AttCod,Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrp]);
  }

/*****************************************************************************/
/****** Get and write the names of the groups of an attendance event *********/
/*****************************************************************************/

static void Att_GetAndWriteNamesOfGrpsAssociatedToEvent (struct Att_Event *Event)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *HidVis_GroupClass[HidVis_NUM_HIDDEN_VISIBLE];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrps;
   unsigned NumGrp;

   /***** Get groups associated to an attendance event from database *****/
   NumGrps = Att_DB_GetGroupsAssociatedToEvent (&mysql_res,Event->AttCod);

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"%s_%s\"",
                  HidVis_GroupClass[Event->Hidden],The_GetSuffix ());

      /***** Write heading *****/
      HTM_Txt (NumGrps == 1 ? Txt_Group  :
			      Txt_Groups);
      HTM_Colon ();
      HTM_NBSP ();

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

	    /* Write the name of the room (row[2]) */
	    if (row[2])	// May be NULL because of LEFT JOIN
	       if (row[2][0])
	         {
		  HTM_NBSP ();
		  HTM_ParTxtPar (row[2]);
	         }

	    /* Write separator */
	    HTM_ListSeparator (NumGrp,NumGrps);
	   }
	}
      else
	 Grp_WriteTheWholeCourse ();

   /***** End container *****/
   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Remove all attendance events of a course ******************/
/*****************************************************************************/

void Att_RemoveCrsEvents (long HieCod)
  {
   /***** Remove students *****/
   Att_DB_RemoveUsrsFromCrsEvents (HieCod);

   /***** Remove groups *****/
   Att_DB_RemoveGrpsAssociatedToCrsEvents (HieCod);

   /***** Remove attendance events *****/
   Att_DB_RemoveCrsEvents (HieCod);
  }

/*****************************************************************************/
/********************* Get number of attendance events ***********************/
/*****************************************************************************/
// Returns the number of attendance events
// in this location (all the platform, the current degree or the current course)

unsigned Att_GetNumEvents (Hie_Level_t HieLvl,unsigned *NumNotif)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAttEvents;

   /***** Set default values *****/
   NumAttEvents = 0;
   *NumNotif = 0;

   /***** Get number of attendance events from database *****/
   if (Att_DB_GetNumEvents (&mysql_res,HieLvl))
     {
      /***** Get number of attendance events *****/
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",&NumAttEvents) != 1)
	 Err_ShowErrorAndExit ("Error when getting number of attendance events.");

      /***** Get number of notifications by email *****/
      if (row[1])
	 if (sscanf (row[1],"%u",NumNotif) != 1)
	    Err_ShowErrorAndExit ("Error when getting number of notifications of attendance events.");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumAttEvents;
  }

/*****************************************************************************/
/************************ Show one attendance event **************************/
/*****************************************************************************/

void Att_ShowOneEvent (void)
  {
   struct Att_Events Events;

   /***** Reset attendance events *****/
   Att_ResetEvents (&Events);

   /***** Get attendance event code *****/
   Events.Event.AttCod = ParCod_GetAndCheckPar (ParCod_Att);

   /***** Show event *****/
   Att_ShowEvent (&Events);
  }

static void Att_ShowEvent (struct Att_Events *Events)
  {
   extern const char *Hlp_USERS_Attendance;
   extern const char *Txt_Event;

   /***** Get parameters *****/
   Events->SelectedOrder = Att_GetParAttOrder ();
   Grp_GetParMyAllGrps ();
   Events->CurrentPage = Pag_GetParPagNum (Pag_ATT_EVENTS);

   /***** Get data of this attendance event *****/
   Att_GetEventDataByCodAndCheckCrs (&Events->Event);
   Events->Event.NumStdsTotal = Att_DB_GetNumStdsTotalWhoAreInEvent (Events->Event.AttCod);

   /***** Show attendance event inside a box *****/
   Box_BoxTableBegin (Events->Event.Title[0] ? Events->Event.Title :
					       Txt_Event,
                      Att_PutIconsOneEvent,Events,
                      Hlp_USERS_Attendance,Box_NOT_CLOSABLE,2);
      Att_ShowOneEventRow (Events,
			   true);	// Show only this event
   Box_BoxTableEnd ();
   HTM_BR ();

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 Att_ListEventOnlyMeAsStudent (&Events->Event);
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Show list of students *****/
         Att_ListEventStudents (Events);
         break;
      default:
         break;
     }
  }

/*****************************************************************************/
/*************** Put contextual icons in an attendance event *****************/
/*****************************************************************************/

static void Att_PutIconsOneEvent (void *Events)
  {
   char *Anchor = NULL;

   /***** Set anchor string *****/
   Frm_SetAnchorStr (((struct Att_Events *) Events)->Event.AttCod,&Anchor);

   /***** Put icons to edit resource link *****/
   Att_PutFormsToRemEditOneEvent ((struct Att_Events *) Events,Anchor);

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (&Anchor);
  }

/*****************************************************************************/
/*********************** List me as student in one event *********************/
/*****************************************************************************/
// Event must be filled before calling this function

static void Att_ListEventOnlyMeAsStudent (struct Att_Event *Event)
  {
   extern const char *Hlp_USERS_Attendance;
   extern const char *Txt_Attendance;
   extern const char *Txt_Student_comment;
   extern const char *Txt_Teachers_comment;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   static Usr_Can_t ICanMakeAnyChange;
   Pho_ShowPhotos_t ShowPhotos;

   /***** Set who can edit *****/
   ICanMakeAnyChange = Usr_CAN_NOT;
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:	// A student can see only her/his attendance
	 if (Event->ClosedOrOpen == CloOpe_OPEN)
	    ICanMakeAnyChange = Usr_CAN;
	 break;
      default:
	 Err_ShowErrorAndExit ("Wrong call.");
	 break;
     }

   /***** Get my setting about photos in users' list for current course *****/
   ShowPhotos = Pho_GetMyPrefAboutListWithPhotosFromDB ();

   /***** List students (only me) *****/
   /* Begin box */
   Box_BoxBegin (Txt_Attendance,NULL,NULL,
                 Hlp_USERS_Attendance,Box_NOT_CLOSABLE);

      /***** Begin form *****/
      if (ICanMakeAnyChange)
	{
	 Frm_BeginForm (ActRecAttMe);
	    ParCod_PutPar (ParCod_Att,Event->AttCod);
	}

      /***** List students (only me) *****/
      /* Begin table */
      HTM_TABLE_Begin ("TBL_SCROLL");

	 /* Header */
	 HTM_TR_Begin (NULL);

	    HTM_TH_Empty (3);
	    if (ShowPhotos == Pho_PHOTOS_SHOW)
	       HTM_TH_Empty (1);
	    HTM_TH_Span (Txt_ROLES_SINGUL_Abc[Rol_STD][Usr_SEX_UNKNOWN],HTM_HEAD_LEFT,1,2,NULL);
	    HTM_TH      (Txt_Student_comment                           ,HTM_HEAD_LEFT);
	    HTM_TH      (Txt_Teachers_comment                          ,HTM_HEAD_LEFT);

	 HTM_TR_End ();

	 /* List of students (only me) */
	 Att_WriteRowUsrToCallTheRoll (1,&Gbl.Usrs.Me.UsrDat,Event,ShowPhotos);

      /* End table */
      HTM_TABLE_End ();

      /* Send button and end form */
      if (ICanMakeAnyChange == Usr_CAN)
	{
	    Btn_PutButton (Btn_SAVE_CHANGES,NULL);
	 Frm_EndForm ();
	}

   /* End box */
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*************** List students who attended to one event *********************/
/*****************************************************************************/
// Event must be filled before calling this function

static void Att_ListEventStudents (struct Att_Events *Events)
  {
   extern const char *Hlp_USERS_Attendance;
   extern const char *Txt_Attendance;
   extern const char *Txt_Student_comment;
   extern const char *Txt_Teachers_comment;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   unsigned NumUsr;
   struct Usr_Data UsrDat;
   static Usr_Can_t ICanMakeAnyChange;
   Pho_ShowPhotos_t ShowPhotos;

   /***** Set who can edit *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 ICanMakeAnyChange = Usr_CAN;
	 break;
      default:
	 ICanMakeAnyChange = Usr_CAN_NOT;
	 break;
     }

   /***** Get groups to show ******/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get list of students in this course *****/
   Usr_GetListUsrs (Hie_CRS,Rol_STD);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Attendance,NULL,NULL,
                 Hlp_USERS_Attendance,Box_NOT_CLOSABLE);

      /***** Form to select groups *****/
      Grp_ShowFormToSelectSeveralGroups (Att_PutParAttCod,Events,NULL);

      /***** Begin section with user list *****/
      HTM_SECTION_Begin (Usr_USER_LIST_SECTION_ID);

	 if (Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs)
	   {
	    /***** Get my preference about photos in users' list for current course *****/
	    ShowPhotos = Pho_GetMyPrefAboutListWithPhotosFromDB ();

	    /***** Initialize structure with user's data *****/
	    Usr_UsrDataConstructor (&UsrDat);

	    /* Begin form */
	    if (ICanMakeAnyChange)
	      {
	       Frm_BeginForm (ActRecAttStd);
		  ParCod_PutPar (ParCod_Att,Events->Event.AttCod);
		  Grp_PutParsCodGrps ();
	      }

	    /* Begin table */
	    HTM_TABLE_Begin ("TBL_SCROLL");

	       /* Header */
	       HTM_TR_Begin (NULL);

		  HTM_TH_Empty (3);
		  if (ShowPhotos == Pho_PHOTOS_SHOW)
		     HTM_TH_Empty (1);
		  HTM_TH_Span (Txt_ROLES_SINGUL_Abc[Rol_STD][Usr_SEX_UNKNOWN],HTM_HEAD_LEFT,1,2,NULL);
		  HTM_TH      (Txt_Student_comment                           ,HTM_HEAD_LEFT);
		  HTM_TH      (Txt_Teachers_comment                          ,HTM_HEAD_LEFT);

	       HTM_TR_End ();

	       /* List of students */
	       for (NumUsr = 0;
		    NumUsr < Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs;
		    NumUsr++)
		 {
		  /* Copy user's basic data from list */
		  Usr_CopyBasicUsrDataFromList (&UsrDat,&Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr]);

		  /* Get list of user's IDs */
		  ID_GetListIDsFromUsrCod (&UsrDat);

		  Att_WriteRowUsrToCallTheRoll (NumUsr + 1,&UsrDat,&Events->Event,
						ShowPhotos);
		 }

	    /* End table */
	    HTM_TABLE_End ();

            /* Send button and end form */
	    if (ICanMakeAnyChange)
	      {
		  Btn_PutButton (Btn_SAVE_CHANGES,NULL);
	       Frm_EndForm ();
	      }

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
/************** Write parameter with code of attendance event ****************/
/*****************************************************************************/

static void Att_PutParAttCod (void *Events)
  {
   if (Events)
      ParCod_PutPar (ParCod_Att,((struct Att_Events *) Events)->Event.AttCod);
  }

/*****************************************************************************/
/************** Write a row of a table with the data of a user ***************/
/*****************************************************************************/

static void Att_WriteRowUsrToCallTheRoll (unsigned NumUsr,
                                          struct Usr_Data *UsrDat,
                                          struct Att_Event *Event,
                                          Pho_ShowPhotos_t ShowPhotos)
  {
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC45x60",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE45x60",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO45x60",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR45x60",
     };
   Att_AbsentOrPresent_t Present;
   char CommentStd[Cns_MAX_BYTES_TEXT + 1];
   char CommentTch[Cns_MAX_BYTES_TEXT + 1];
   Usr_Can_t ICanChangeStdAttendance;
   Usr_Can_t ICanEditStdComment;
   Usr_Can_t ICanEditTchComment;

   /***** Set who can edit *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 // A student can see only her/his attendance
	 if (Usr_ItsMe (UsrDat->UsrCod) == Usr_OTHER)
	    Err_ShowErrorAndExit ("Wrong call.");
	 ICanChangeStdAttendance = Usr_CAN_NOT;
	 ICanEditStdComment	 = (Event->ClosedOrOpen == CloOpe_OPEN) ? Usr_CAN :	// Attendance event is open
									  Usr_CAN_NOT;
	 ICanEditTchComment	 = Usr_CAN_NOT;
	 break;
      case Rol_TCH:
	 ICanChangeStdAttendance = Usr_CAN;
	 ICanEditStdComment	 = Usr_CAN_NOT;
	 ICanEditTchComment	 = Usr_CAN;
	 break;
      case Rol_SYS_ADM:
	 ICanChangeStdAttendance = Usr_CAN;
	 ICanEditStdComment	 = Usr_CAN_NOT;
	 ICanEditTchComment	 = Usr_CAN_NOT;
	 break;
      default:
	 ICanChangeStdAttendance = Usr_CAN_NOT;
	 ICanEditStdComment	 = Usr_CAN_NOT;
	 ICanEditTchComment	 = Usr_CAN_NOT;
	 break;
     }

   /***** Check if this student is already present in the current event *****/
   Present = Att_CheckIfUsrIsPresentInEventAndGetComments (Event->AttCod,UsrDat->UsrCod,
							   CommentStd,CommentTch);

   /***** Begin table row *****/
   HTM_TR_Begin (NULL);

      /***** Icon to show if the user is already present *****/
      HTM_TD_Begin ("class=\"BT %s\"",The_GetColorRows ());
	 HTM_LABEL_Begin ("for=\"Std%u\"",NumUsr);
	    Att_PutCheckOrCross (Present);
	 HTM_LABEL_End ();
      HTM_TD_End ();

      /***** Checkbox to select user *****/
      HTM_TD_Begin ("class=\"CT %s\"",The_GetColorRows ());
	 HTM_INPUT_CHECKBOX ("UsrCodStd",
			     ((Present == Att_PRESENT) ? HTM_CHECKED :
					                 HTM_NO_ATTR) |
			     ((ICanChangeStdAttendance == Usr_CAN) ? HTM_NO_ATTR :
								     HTM_DISABLED),
			     "id=\"Std%u\" value=\"%s\"",
			     NumUsr,UsrDat->EnUsrCod);
      HTM_TD_End ();

      /***** Write number of student in the list *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",
		    UsrDat->Accepted ? "DAT_STRONG" :
				       "DAT",
		    The_GetSuffix (),The_GetColorRows ());
	 HTM_Unsigned (NumUsr);
      HTM_TD_End ();

      /***** Show student's photo *****/
      if (ShowPhotos == Pho_PHOTOS_SHOW)
	{
	 HTM_TD_Begin ("class=\"%s LT\"",The_GetColorRows ());
	    Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                               ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
	 HTM_TD_End ();
	}

      /***** Write user's ID ******/
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",
		    UsrDat->Accepted ? "DAT_SMALL_STRONG" :
				       "DAT_SMALL",
		    The_GetSuffix (),The_GetColorRows ());
	 ID_WriteUsrIDs (UsrDat,NULL);
      HTM_TD_End ();

      /***** Write student's name *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",
		    UsrDat->Accepted ? "DAT_SMALL_STRONG" :
				       "DAT_SMALL",
		    The_GetSuffix (),The_GetColorRows ());
	 HTM_Txt (UsrDat->Surname1);
	 if (UsrDat->Surname2[0])
	   {
	    HTM_SP ();
	    HTM_Txt (UsrDat->Surname2);
	   }
	 HTM_Comma ();
	 HTM_SP ();
	 HTM_Txt (UsrDat->FrstName);
      HTM_TD_End ();

      /***** Student's comment: write form or text */
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",
		    UsrDat->Accepted ? "DAT_SMALL_STRONG" :
				       "DAT_SMALL",
		    The_GetSuffix (),The_GetColorRows ());
	 switch (ICanEditStdComment)
	   {
	    case Usr_CAN:	// Show with form
	       HTM_TEXTAREA_Begin (HTM_NO_ATTR,
				   "name=\"CommentStd%s\" cols=\"40\" rows=\"3\""
				   " class=\"INPUT_%s\"",
				   UsrDat->EnUsrCod,The_GetSuffix ());
		  HTM_Txt (CommentStd);
	       HTM_TEXTAREA_End ();
	       break;
	    case Usr_CAN_NOT:	// Show without form
	    default:
	       Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				 CommentStd,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
	       HTM_Txt (CommentStd);
	       break;
	   }
      HTM_TD_End ();

      /***** Teacher's comment: write form, text or nothing */
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",
		    UsrDat->Accepted ? "DAT_SMALL_STRONG" :
				       "DAT_SMALL",
		    The_GetSuffix (),The_GetColorRows ());
	 switch (ICanEditTchComment)
	   {
	    case Usr_CAN:			// Show with form
	       HTM_TEXTAREA_Begin (HTM_NO_ATTR,
				   "name=\"CommentTch%s\" cols=\"40\" rows=\"3\""
				   " class=\"INPUT_%s\"",
				   UsrDat->EnUsrCod,The_GetSuffix ());
		  HTM_Txt (CommentTch);
	       HTM_TEXTAREA_End ();
	       break;
	    case Usr_CAN_NOT:			// Show without form
	    default:
	       if (Event->CommentTchVisible == HidVis_VISIBLE)
		 {
		  Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				    CommentTch,Cns_MAX_BYTES_TEXT,
				    Str_DONT_REMOVE_SPACES);
		  HTM_Txt (CommentTch);
		 }
	       break;
	  }
      HTM_TD_End ();

   /***** End table row *****/
   HTM_TR_End ();

   The_ChangeRowColor ();
  }

/*****************************************************************************/
/***************** Put link to view an attendance event **********************/
/*****************************************************************************/

static void Att_PutLinkEvent (struct Att_Event *Event,
			      const char *Title,const char *Txt)
  {
   extern const char *HidVis_TitleClass[HidVis_NUM_HIDDEN_VISIBLE];

   /***** Begin form *****/
   Frm_BeginForm (ActSeeOneAtt);
      ParCod_PutPar (ParCod_Att,Event->AttCod);
      Grp_PutParsCodGrpsAssociated (Grp_ATTENDANCE_EVENT,Event->AttCod);

      /***** Link to view attendance event *****/
      HTM_BUTTON_Submit_Begin (Title,NULL,
			       "class=\"LT BT_LINK %s_%s\"",
			       HidVis_TitleClass[Event->Hidden],The_GetSuffix ());
	 HTM_Txt (Txt);
      HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************** Save me as students who attended to an event ****************/
/*****************************************************************************/

void Att_RegisterMeAsStdInEvent (void)
  {
   extern const char *Txt_Your_comment_has_been_updated;
   struct Att_Events Events;
   Att_AbsentOrPresent_t Present;
   char *ParName;
   char CommentStd[Cns_MAX_BYTES_TEXT + 1];
   char CommentTch[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset attendance events *****/
   Att_ResetEvents (&Events);

   /***** Get attendance event code *****/
   Events.Event.AttCod = ParCod_GetAndCheckPar (ParCod_Att);
   Att_GetEventDataByCodAndCheckCrs (&Events.Event);	// This checks that event belong to current course

   if (Events.Event.ClosedOrOpen == CloOpe_OPEN)
     {
      /***** Get comments for this student *****/
      Present = Att_CheckIfUsrIsPresentInEventAndGetComments (Events.Event.AttCod,Gbl.Usrs.Me.UsrDat.UsrCod,
	                                                      CommentStd,CommentTch);
      if (asprintf (&ParName,"CommentStd%s",Gbl.Usrs.Me.UsrDat.EnUsrCod) < 0)
         Err_NotEnoughMemoryExit ();
      Par_GetParHTML (ParName,CommentStd,Cns_MAX_BYTES_TEXT);
      free (ParName);

      if (Present ||
	  CommentStd[0] ||
	  CommentTch[0])
	 /***** Register student *****/
	 Att_DB_RegUsrInEventChangingComments (Events.Event.AttCod,Gbl.Usrs.Me.UsrDat.UsrCod,
	                                       Present,CommentStd,CommentTch);
      else
	 /***** Remove student *****/
	 Att_DB_RemoveUsrFromEvent (Events.Event.AttCod,Gbl.Usrs.Me.UsrDat.UsrCod);

      /***** Write final message *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_Your_comment_has_been_updated);
     }

   /***** Show the attendance event again *****/
   Att_ShowEvent (&Events);
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
   5. Delete from att_users all students marked as Remove=true
   6. Replace (insert without duplicated) into att_users all students marked as Remove=false
 */
void Att_RegisterStudentsInEvent (void)
  {
   extern const char *Txt_Presents;
   extern const char *Txt_Absents;
   struct Att_Events Events;
   unsigned NumUsr;
   const char *Ptr;
   Att_AbsentOrPresent_t Present;
   char *ParName;
   unsigned NumStds[Att_NUM_PRESENT];
   struct Usr_Data UsrData;
   char CommentStd[Cns_MAX_BYTES_TEXT + 1];
   char CommentTch[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset attendance events *****/
   Att_ResetEvents (&Events);

   /***** Get attendance event code *****/
   Events.Event.AttCod = ParCod_GetAndCheckPar (ParCod_Att);
   Att_GetEventDataByCodAndCheckCrs (&Events.Event);	// This checks that event belong to current course

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
      Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected,
					     Usr_DONT_GET_LIST_ALL_USRS);

      /***** 4. Loop over the list Gbl.Usrs.Selected.List[Rol_STD],
                that holds the list of the students marked as present,
                marking the students in Gbl.Usrs.LstUsrs[Rol_STD].Lst as Remove=false *****/
      /* Initialize structure with user's data */
      Usr_UsrDataConstructor (&UsrData);

         /* For each students marked as present... */
	 for (Ptr = Gbl.Usrs.Selected.List[Rol_STD];
	      *Ptr;
	     )
	   {
	    Par_GetNextStrUntilSeparParMult (&Ptr,UsrData.EnUsrCod,
					     Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
	    Usr_GetUsrCodFromEncryptedUsrCod (&UsrData);
	    if (UsrData.UsrCod > 0)	// Student exists in database
	       /* Mark student to not be removed */
	       for (NumUsr = 0;
		    NumUsr < Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs;
		    NumUsr++)
		  if (Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].UsrCod == UsrData.UsrCod)
		    {
		     Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].Remove = false;
		     break;	// Found! Exit loop
		    }
	   }

      /* Free memory used for user's data */
      Usr_UsrDataDestructor (&UsrData);

      /***** Free memory *****/
      /* Free memory used by list of selected students' codes */
      Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

      // 5. Delete from att_users all students marked as Remove=true
      // 6. Replace (insert without duplicated) into att_users all students marked as Remove=false
      for (Present  = Att_ABSENT;
	   Present <= Att_PRESENT;
	   Present++)
	 NumStds[Present] = 0;
      for (NumUsr = 0;
	   NumUsr < Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs;
	   NumUsr++)
	{
	 /***** Get comments for this student *****/
	 Present = Att_CheckIfUsrIsPresentInEventAndGetComments (Events.Event.AttCod,
								 Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].UsrCod,
								 CommentStd,CommentTch);
	 if (asprintf (&ParName,"CommentTch%s",
	               Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].EnUsrCod) < 0)
	    Err_NotEnoughMemoryExit ();
	 Par_GetParHTML (ParName,CommentTch,Cns_MAX_BYTES_TEXT);
	 free (ParName);

	 Present = Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].Remove ? Att_ABSENT :
								  Att_PRESENT;

	 if (Present == Att_PRESENT ||
	     CommentStd[0] ||
	     CommentTch[0])
	    /***** Register student *****/
	    Att_DB_RegUsrInEventChangingComments (Events.Event.AttCod,Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].UsrCod,
					          Present,CommentStd,CommentTch);
	 else
	    /***** Remove student *****/
	    Att_DB_RemoveUsrFromEvent (Events.Event.AttCod,Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].UsrCod);

	 NumStds[Present]++;
	}

      /***** Free memory for students list *****/
      Usr_FreeUsrsList (Rol_STD);

      /***** Write final message *****/
      Ale_ShowAlert (Ale_INFO,"%s: %u<br>"
		              "%s: %u",
		     Txt_Presents,NumStds[Att_PRESENT],
		     Txt_Absents ,NumStds[Att_ABSENT ]);
     }
   else	// Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs == 0
      /***** Show warning indicating no students found *****/
      Usr_ShowWarningNoUsersFound (Rol_STD);

   /***** Show the attendance event again *****/
   Att_ShowEvent (&Events);

   /***** Free memory for list of groups selected *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/********* Get number of users from a list who attended to an event **********/
/*****************************************************************************/

static unsigned Att_GetNumUsrsFromAListWhoAreInEvent (long AttCod,
						      long *LstSelectedUsrCods,
						      unsigned NumUsrsInList)
  {
   char *SubQueryUsrs;
   unsigned NumUsrsInAttEvent;

   if (NumUsrsInList)
     {
      /***** Create subquery string *****/
      Usr_CreateSubqueryUsrCods (LstSelectedUsrCods,NumUsrsInList,
				 &SubQueryUsrs);

      /***** Get number of users from list in attendance event from database ****/
      NumUsrsInAttEvent = Att_DB_GetNumStdsFromListWhoAreInEvent (AttCod,SubQueryUsrs);

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

static Att_AbsentOrPresent_t Att_CheckIfUsrIsPresentInEvent (long AttCod,long UsrCod)
  {
   Att_AbsentOrPresent_t Present;

   Att_DB_CheckIfUsrIsInTableAttUsr (AttCod,UsrCod,&Present);

   return Present;
  }

/*****************************************************************************/
/***************** Check if a student attended to an event *******************/
/*****************************************************************************/

static Att_AbsentOrPresent_t Att_CheckIfUsrIsPresentInEventAndGetComments (long AttCod,long UsrCod,
								   char CommentStd[Cns_MAX_BYTES_TEXT + 1],
								   char CommentTch[Cns_MAX_BYTES_TEXT + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Att_AbsentOrPresent_t Present;

   /***** Check if a students is registered in an event in database *****/
   if (Att_DB_GetPresentAndComments (&mysql_res,AttCod,UsrCod))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get if present (row[0]) */
      Present = Att_GetPresentFromYN (row[0][0]);

      /* Get student's (row[1]) and teacher's (row[2]) comment */
      Str_Copy (CommentStd,row[1],Cns_MAX_BYTES_TEXT);
      Str_Copy (CommentTch,row[2],Cns_MAX_BYTES_TEXT);
     }
   else	// User is not present
     {
      Present = Att_ABSENT;
      CommentStd[0] =
      CommentTch[0] = '\0';
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Present;
  }

/*****************************************************************************/
/************ Get if absent or present from a 'Y'/'N' character **************/
/*****************************************************************************/

Att_AbsentOrPresent_t Att_GetPresentFromYN (char Ch)
  {
   return (Ch == 'Y') ? Att_PRESENT :
			Att_ABSENT;
  }

/*****************************************************************************/
/********** Request listing attendance of users to several events ************/
/*****************************************************************************/

void Att_ReqListUsrsAttendanceCrs (void)
  {
   Att_ReqListOrPrintUsrsAttendanceCrs (NULL);
  }

static void Att_ReqListOrPrintUsrsAttendanceCrs (__attribute__((unused)) void *Args)
  {
   extern const char *Hlp_USERS_Attendance_attendance_list;
   extern const char *Txt_Attendance_list;
   struct Att_Events Events;

   /***** Reset attendance events *****/
   Att_ResetEvents (&Events);

   /***** Get list of attendance events *****/
   Att_GetListEvents (&Events,Att_OLDEST_FIRST);

   /***** List users to select some of them *****/
   Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
				     ActSeeLstUsrAtt,
				     NULL,NULL,
				     Txt_Attendance_list,
				     Hlp_USERS_Attendance_attendance_list,
				     Btn_CONTINUE,
				     Frm_DONT_PUT_FORM);	// Do not put form with date range

   /***** Free list of attendance events *****/
   Att_FreeListEvents (&Events);
  }

/*****************************************************************************/
/********** List my attendance (I am a student) to several events ************/
/*****************************************************************************/

void Att_ListMyAttendanceCrs (void)
  {
   Att_ListOrPrintMyAttendanceCrs (Att_VIEW_ONLY_ME);
  }

void Att_PrintMyAttendanceCrs (void)
  {
   Att_ListOrPrintMyAttendanceCrs (Att_PRNT_ONLY_ME);
  }

static void Att_ListOrPrintMyAttendanceCrs (Att_TypeOfView_t TypeOfView)
  {
   extern const char *Hlp_USERS_Attendance_attendance_list;
   extern const char *Txt_Attendance;
   struct Att_Events Events;
   unsigned NumAttEvent;

   switch (TypeOfView)
     {
      case Att_VIEW_ONLY_ME:
      case Att_PRNT_ONLY_ME:
	 /***** Reset attendance events *****/
	 Att_ResetEvents (&Events);

	 /***** Get list of attendance events *****/
	 Att_GetListEvents (&Events,Att_OLDEST_FIRST);

	 /***** Get boolean parameter that indicates if details must be shown *****/
	 Events.ShowDetails = Par_GetParBool ("ShowDetails");

	 /***** Get list of groups selected ******/
	 Grp_GetParCodsSeveralGrpsToShowUsrs ();

	 /***** Get number of students in each event *****/
	 for (NumAttEvent = 0;
	      NumAttEvent < Events.Num;
	      NumAttEvent++)
	    /* Get number of students in this event */
	    Events.Lst[NumAttEvent].NumStdsFromList =
	    Att_GetNumUsrsFromAListWhoAreInEvent (Events.Lst[NumAttEvent].AttCod,
						  &Gbl.Usrs.Me.UsrDat.UsrCod,1);

	 /***** Get list of attendance events selected *****/
	 Att_GetListSelectedAttCods (&Events);

	 /***** Begin box *****/
	 switch (TypeOfView)
	   {
	    case Att_VIEW_ONLY_ME:
	       Box_BoxBegin (Txt_Attendance,Att_PutIconsMyAttList,&Events,
			     Hlp_USERS_Attendance_attendance_list,Box_NOT_CLOSABLE);
	       break;
	    case Att_PRNT_ONLY_ME:
	       Box_BoxBegin (Txt_Attendance,NULL,NULL,NULL,Box_NOT_CLOSABLE);
	       break;
	    default:
	       Err_WrongTypeExit ();
	       break;
	   }

	 /***** List events to select *****/
	 Att_ListEventsToSelect (&Events,TypeOfView);

	 /***** Show table with attendances for every student in list *****/
	 Att_ListUsrsAttendanceTable (&Events,TypeOfView,1,&Gbl.Usrs.Me.UsrDat.UsrCod);

	 /***** Show details or put button to show details *****/
	 if (Events.ShowDetails)
	    Att_ListStdsWithAttEventsDetails (&Events,1,&Gbl.Usrs.Me.UsrDat.UsrCod);

	 /***** End box *****/
	 Box_BoxEnd ();

	 /***** Free memory for list of attendance events selected *****/
	 free (Events.StrAttCodsSelected);

	 /***** Free list of groups selected *****/
	 Grp_FreeListCodSelectedGrps ();

	 /***** Free list of attendance events *****/
	 Att_FreeListEvents (&Events);
	 break;
      default:
	 Err_WrongTypeExit ();
	 break;
     }
  }

/*****************************************************************************/
/*************** List attendance of users to several events ******************/
/*****************************************************************************/

void Att_ListUsrsAttendanceCrs (void)
  {
   Att_GetUsrsAndListOrPrintAttendanceCrs (Att_VIEW_SEL_USR);
  }

void Att_PrintUsrsAttendanceCrs (void)
  {
   Att_GetUsrsAndListOrPrintAttendanceCrs (Att_PRNT_SEL_USR);
  }

static void Att_GetUsrsAndListOrPrintAttendanceCrs (Att_TypeOfView_t TypeOfView)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  Att_ListOrPrintUsrsAttendanceCrs,&TypeOfView,
                                  Att_ReqListOrPrintUsrsAttendanceCrs,NULL);
  }

static void Att_ListOrPrintUsrsAttendanceCrs (void *TypeOfView)
  {
   extern const char *Hlp_USERS_Attendance_attendance_list;
   extern const char *Txt_Attendance_list;
   struct Att_Events Events;
   unsigned NumUsrsInList;
   long *LstSelectedUsrCods;
   unsigned NumAttEvent;

   switch (*((Att_TypeOfView_t *) TypeOfView))
     {
      case Att_VIEW_SEL_USR:
      case Att_PRNT_SEL_USR:
	 /***** Reset attendance events *****/
	 Att_ResetEvents (&Events);

	 /***** Get parameters *****/
	 /* Get boolean parameter that indicates if details must be shown */
	 Events.ShowDetails = Par_GetParBool ("ShowDetails");

	 /* Get list of groups selected */
	 Grp_GetParCodsSeveralGrpsToShowUsrs ();

	 /***** Count number of valid users in list of encrypted user codes *****/
	 NumUsrsInList = Usr_CountNumUsrsInListOfSelectedEncryptedUsrCods (&Gbl.Usrs.Selected);

	 if (NumUsrsInList)
	   {
	    /***** Get list of students selected to show their attendances *****/
	    Usr_GetListSelectedUsrCods (&Gbl.Usrs.Selected,NumUsrsInList,&LstSelectedUsrCods);

	    /***** Get list of attendance events *****/
	    Att_GetListEvents (&Events,Att_OLDEST_FIRST);

	    /***** Get number of students in each event *****/
	    for (NumAttEvent = 0;
		 NumAttEvent < Events.Num;
		 NumAttEvent++)
	       /* Get number of students in this event */
	       Events.Lst[NumAttEvent].NumStdsFromList =
	       Att_GetNumUsrsFromAListWhoAreInEvent (Events.Lst[NumAttEvent].AttCod,
						     LstSelectedUsrCods,NumUsrsInList);

	    /***** Get list of attendance events selected *****/
	    Att_GetListSelectedAttCods (&Events);

	    /***** Begin box *****/
	    switch (*((Att_TypeOfView_t *) TypeOfView))
	      {
	       case Att_VIEW_SEL_USR:
		  Box_BoxBegin (Txt_Attendance_list,
				Att_PutIconsStdsAttList,&Events,
				Hlp_USERS_Attendance_attendance_list,Box_NOT_CLOSABLE);
		  break;
	       case Att_PRNT_SEL_USR:
		  Box_BoxBegin (Txt_Attendance_list,NULL,NULL,
				NULL,Box_NOT_CLOSABLE);
		  break;
	       default:
		  Err_WrongTypeExit ();
	      }

	    /***** List events to select *****/
	    Att_ListEventsToSelect (&Events,*((Att_TypeOfView_t *) TypeOfView));

	    /***** Show table with attendances for every student in list *****/
	    Att_ListUsrsAttendanceTable (&Events,*((Att_TypeOfView_t *) TypeOfView),
	                                 NumUsrsInList,LstSelectedUsrCods);

	    /***** Show details or put button to show details *****/
	    if (Events.ShowDetails)
	       Att_ListStdsWithAttEventsDetails (&Events,NumUsrsInList,LstSelectedUsrCods);

	    /***** End box *****/
	    Box_BoxEnd ();

	    /***** Free memory for list of attendance events selected *****/
	    free (Events.StrAttCodsSelected);

	    /***** Free list of attendance events *****/
	    Att_FreeListEvents (&Events);

	    /***** Free list of user codes *****/
	    Usr_FreeListSelectedUsrCods (LstSelectedUsrCods);
	   }

	 /***** Free list of groups selected *****/
	 Grp_FreeListCodSelectedGrps ();
	 break;
      default:
	 Err_WrongTypeExit ();
	 break;
     }
  }

/*****************************************************************************/
/****************** Get list of attendance events selected *******************/
/*****************************************************************************/

static void Att_GetListSelectedAttCods (struct Att_Events *Events)
  {
   size_t MaxSizeListAttCodsSelected;
   unsigned NumAttEvent;
   const char *Ptr;
   long AttCod;
   char LongStr[Cns_MAX_DIGITS_LONG + 1];
   MYSQL_RES *mysql_res;
   unsigned NumGrpsInThisEvent;
   unsigned NumGrpInThisEvent;
   long GrpCodInThisEvent;
   unsigned NumGrpSel;

   /***** Allocate memory for list of attendance events selected *****/
   MaxSizeListAttCodsSelected = (size_t) Events->Num * (Cns_MAX_DIGITS_LONG + 1);
   if ((Events->StrAttCodsSelected = malloc (MaxSizeListAttCodsSelected + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Get parameter multiple with list of attendance events selected *****/
   Par_GetParMultiToText ("AttCods",Events->StrAttCodsSelected,MaxSizeListAttCodsSelected);

   /***** Set which attendance events will be shown as selected (checkboxes on) *****/
   if (Events->StrAttCodsSelected[0])	// There are events selected
     {
      /* Reset selection */
      for (NumAttEvent = 0;
	   NumAttEvent < Events->Num;
	   NumAttEvent++)
	 Events->Lst[NumAttEvent].Checked = HTM_NO_ATTR;

      /* Set some events as selected */
      for (Ptr = Events->StrAttCodsSelected;
	   *Ptr;
	  )
	{
	 /* Get next attendance event selected */
	 Par_GetNextStrUntilSeparParMult (&Ptr,LongStr,Cns_MAX_DIGITS_LONG);
	 AttCod = Str_ConvertStrCodToLongCod (LongStr);

	 /* Set each event in *StrAttCodsSelected as selected */
	 for (NumAttEvent = 0;
	      NumAttEvent < Events->Num;
	      NumAttEvent++)
	    if (Events->Lst[NumAttEvent].AttCod == AttCod)
	      {
	       Events->Lst[NumAttEvent].Checked = HTM_CHECKED;
	       break;
	      }
	}
     }
   else				// No events selected
     {
      /***** Set which events will be marked as selected by default *****/
      if (!Gbl.Crs.Grps.NumGrps ||	// Course has no groups
          Gbl.Crs.Grps.AllGrpsSel)		// All groups selected
	 /* Set all events as selected */
	 for (NumAttEvent = 0;
	      NumAttEvent < Events->Num;
	      NumAttEvent++)
	    Events->Lst[NumAttEvent].Checked = HTM_CHECKED;
      else					// Course has groups and not all of them are selected
	 for (NumAttEvent = 0;
	      NumAttEvent < Events->Num;
	      NumAttEvent++)
	   {
	    /* Reset selection */
	    Events->Lst[NumAttEvent].Checked = HTM_NO_ATTR;

	    /* Set this event as selected? */
	    if (Events->Lst[NumAttEvent].NumStdsFromList)	// Some students attended to this event
	       Events->Lst[NumAttEvent].Checked = HTM_CHECKED;
	    else						// No students attended to this event
	      {
	       /***** Get groups associated to an attendance event from database *****/
	       if ((NumGrpsInThisEvent = Att_DB_GetGrpCodsAssociatedToEvent (&mysql_res,
	                                                                     Events->Lst[NumAttEvent].AttCod)))	// This event is associated to groups
		  /* Get groups associated to this event */
		  for (NumGrpInThisEvent = 0;
		       NumGrpInThisEvent < NumGrpsInThisEvent &&
		       Events->Lst[NumAttEvent].Checked == HTM_NO_ATTR;
		       NumGrpInThisEvent++)
		    {
		     /* Get next group associated to this event */
		     if ((GrpCodInThisEvent = DB_GetNextCode (mysql_res)) > 0)
			/* Check if this group is selected */
			for (NumGrpSel = 0;
			     NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps &&
			     Events->Lst[NumAttEvent].Checked == HTM_NO_ATTR;
			     NumGrpSel++)
			   if (Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel] == GrpCodInThisEvent)
			      Events->Lst[NumAttEvent].Checked = HTM_CHECKED;
		    }
	       else			// This event is not associated to groups
		  Events->Lst[NumAttEvent].Checked = HTM_CHECKED;

	       /***** Free structure that stores the query result *****/
	       DB_FreeMySQLResult (&mysql_res);
	      }
	   }
     }
  }

/*****************************************************************************/
/******* Put contextual icons when listing my assistance (as student) ********/
/*****************************************************************************/

static void Att_PutIconsMyAttList (void *Events)
  {
   if (Events)
     {
      /***** Put icon to print my assistance (as student) to several events *****/
      Ico_PutContextualIconToPrint (ActPrnLstMyAtt,
				    Att_PutFormToPrintMyListPars,Events);

      /***** Put icon to print my QR code *****/
      QR_PutLinkToPrintQRCode (ActPrnUsrQR,
			       Usr_PutParMyUsrCodEncrypted,Gbl.Usrs.Me.UsrDat.EnUsrCod);
     }
  }

static void Att_PutFormToPrintMyListPars (void *Events)
  {
   if (Events)
     {
      if (((struct Att_Events *) Events)->ShowDetails)
	 Par_PutParChar ("ShowDetails",'Y');
      if (((struct Att_Events *) Events)->StrAttCodsSelected)
	 if (((struct Att_Events *) Events)->StrAttCodsSelected[0])
	    Par_PutParString (NULL,"AttCods",((struct Att_Events *) Events)->StrAttCodsSelected);
     }
  }

/*****************************************************************************/
/******** Put icon to print assistance of students to several events *********/
/*****************************************************************************/

static void Att_PutIconsStdsAttList (void *Events)
  {
   if (Events)
     {
      /***** Put icon to print assistance of students to several events *****/
      Ico_PutContextualIconToPrint (ActPrnLstUsrAtt,
				    Att_PutParsToPrintStdsList,Events);

      /***** Put icon to print my QR code *****/
      QR_PutLinkToPrintQRCode (ActPrnUsrQR,
			       Usr_PutParMyUsrCodEncrypted,Gbl.Usrs.Me.UsrDat.EnUsrCod);
     }
  }

static void Att_PutParsToPrintStdsList (void *Events)
  {
   if (Events)
     {
      if (((struct Att_Events *) Events)->ShowDetails)
	 Par_PutParChar ("ShowDetails",'Y');
      Grp_PutParsCodGrps ();
      Usr_PutParSelectedUsrsCods (&Gbl.Usrs.Selected);
      if (((struct Att_Events *) Events)->StrAttCodsSelected)
	 if (((struct Att_Events *) Events)->StrAttCodsSelected[0])
	    Par_PutParString (NULL,"AttCods",((struct Att_Events *) Events)->StrAttCodsSelected);
     }
  }

/*****************************************************************************/
/**** Put a link (form) to list assistance of students to several events *****/
/*****************************************************************************/

static void Att_PutButtonToShowDetails (const struct Att_Events *Events)
  {
   /***** Button to show more details *****/
   /* Begin form */
   Frm_BeginFormAnchor (Gbl.Action.Act,Att_ATTENDANCE_DETAILS_ID);
      Par_PutParChar ("ShowDetails",'Y');
      Grp_PutParsCodGrps ();
      Usr_PutParSelectedUsrsCods (&Gbl.Usrs.Selected);
      if (Events->StrAttCodsSelected)
	 if (Events->StrAttCodsSelected[0])
	    Par_PutParString (NULL,"AttCods",Events->StrAttCodsSelected);

      /* Button */
      Btn_PutButton (Btn_SHOW_MORE_DETAILS,NULL);

   /* End form */
   Frm_EndForm ();
  }

/*****************************************************************************/
/********** Write list of those attendance events that have students *********/
/*****************************************************************************/

static void Att_ListEventsToSelect (struct Att_Events *Events,
                                    Att_TypeOfView_t TypeOfView)
  {
   extern const char *Txt_Events;
   extern const char *Txt_Event;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Update_attendance;
   static void (*FunctionToDrawContextualIcons[Att_TYPES_OF_VIEW]) (void *Args) =
     {
      [Att_VIEW_ONLY_ME] = Att_PutIconToViewAttEvents,
      [Att_VIEW_SEL_USR] = Att_PutIconToEditAttEvents,
      [Att_PRNT_ONLY_ME] = NULL,
      [Att_PRNT_SEL_USR] = NULL,
     };
   unsigned UniqueId;
   char *Id;
   unsigned NumAttEvent;
   bool NormalView = (TypeOfView == Att_VIEW_ONLY_ME ||
                      TypeOfView == Att_VIEW_SEL_USR);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Events,FunctionToDrawContextualIcons[TypeOfView],NULL,
		 NULL,Box_NOT_CLOSABLE);

      /***** Begin form to update the attendance
	     depending on the events selected *****/
      if (NormalView)
	{
	 Frm_BeginFormAnchor (Gbl.Action.Act,Att_ATTENDANCE_TABLE_ID);
	    Grp_PutParsCodGrps ();
	    Usr_PutParSelectedUsrsCods (&Gbl.Usrs.Selected);
	}

      /***** Begin table *****/
      HTM_TABLE_Begin ("TBL_SCROLL");

	 /***** Heading row *****/
	 HTM_TR_Begin (NULL);
	    HTM_TH_Span (Txt_Event                                     ,HTM_HEAD_LEFT ,1,4,NULL);
	    HTM_TH      (Txt_ROLES_PLURAL_Abc[Rol_STD][Usr_SEX_UNKNOWN],HTM_HEAD_RIGHT);
	 HTM_TR_End ();

	 /***** List the events *****/
	 for (NumAttEvent = 0, UniqueId = 1;
	      NumAttEvent < Events->Num;
	      NumAttEvent++, UniqueId++, The_ChangeRowColor ())
	   {
	    /* Get data of the attendance event from database */
	    Events->Event.AttCod = Events->Lst[NumAttEvent].AttCod;
	    Att_GetEventDataByCodAndCheckCrs (&Events->Event);
	    Events->Event.NumStdsTotal = Att_DB_GetNumStdsTotalWhoAreInEvent (Events->Event.AttCod);

	    /* Write a row for this event */
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"CT DAT_%s %s\"",
	                     The_GetSuffix (),The_GetColorRows ());
		  HTM_INPUT_CHECKBOX ("AttCods",
				      Events->Lst[NumAttEvent].Checked,
				      "id=\"Event%u\" value=\"%ld\"",
				      NumAttEvent,Events->Event.AttCod);
	       HTM_TD_End ();

	       HTM_TD_Begin ("class=\"RT DAT_%s %s\"",
	                     The_GetSuffix (),The_GetColorRows ());
		  HTM_LABEL_Begin ("for=\"Event%u\"",NumAttEvent);
		     HTM_Unsigned (NumAttEvent + 1); HTM_Colon ();
		  HTM_LABEL_End ();
	       HTM_TD_End ();

	       HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	                     The_GetSuffix (),The_GetColorRows ());
		  if (asprintf (&Id,"att_date_start_%u",UniqueId) < 0)
		     Err_NotEnoughMemoryExit ();
		  HTM_LABEL_Begin ("for=\"Event%u\"",NumAttEvent);
		     HTM_SPAN_Begin ("id=\"%s\"",Id);
		     HTM_SPAN_End ();
		  HTM_LABEL_End ();
		  Dat_WriteLocalDateHMSFromUTC (Id,Events->Event.TimeUTC[Dat_STR_TIME],
						Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
						Dat_WRITE_TODAY |
						Dat_WRITE_DATE_ON_SAME_DAY |
						Dat_WRITE_WEEK_DAY |
						Dat_WRITE_HOUR |
						Dat_WRITE_MINUTE |
						Dat_WRITE_SECOND);
		  free (Id);
	       HTM_TD_End ();

	       HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	                     The_GetSuffix (),The_GetColorRows ());
		  HTM_Txt (Events->Event.Title);
	       HTM_TD_End ();

	       HTM_TD_Begin ("class=\"RT DAT_%s %s\"",
	                     The_GetSuffix (),The_GetColorRows ());
		  HTM_Unsigned (Events->Event.NumStdsTotal);
	       HTM_TD_End ();

	    HTM_TR_End ();
	   }

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Put button to refresh *****/
      if (NormalView)
	 Lay_WriteLinkToUpdate (Txt_Update_attendance,NULL);

      /***** End form *****/
      if (NormalView)
	 Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*********** Put icon to list (without edition) attendance events ************/
/*****************************************************************************/

static void Att_PutIconToViewAttEvents (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToView (ActSeeAllAtt,NULL,NULL,NULL);
  }

/*****************************************************************************/
/************ Put icon to list (with edition) attendance events **************/
/*****************************************************************************/

static void Att_PutIconToEditAttEvents (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToEdit (ActSeeAllAtt,NULL,NULL,NULL);
  }

/*****************************************************************************/
/************ Show table with attendances for every user in list *************/
/*****************************************************************************/

static void Att_ListUsrsAttendanceTable (struct Att_Events *Events,
                                         Att_TypeOfView_t TypeOfView,
	                                 unsigned NumUsrsInList,
                                         long *LstSelectedUsrCods)
  {
   extern const char *Txt_Number_of_users;
   struct Usr_Data UsrDat;
   unsigned NumUsr;
   unsigned NumAttEvent;
   unsigned Total;
   Pho_ShowPhotos_t ShowPhotos;
   bool PutButtonShowDetails = (TypeOfView == Att_VIEW_ONLY_ME ||
	                        TypeOfView == Att_VIEW_SEL_USR) &&
	                        !Events->ShowDetails;

   /***** Get my preference about photos in users' list for current course *****/
   ShowPhotos = Pho_GetMyPrefAboutListWithPhotosFromDB ();

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Begin section with attendance table *****/
   HTM_SECTION_Begin (Att_ATTENDANCE_TABLE_ID);

      /***** Begin table *****/
      HTM_TABLE_Begin ("TBL_SCROLL");

	 /***** Heading row *****/
	 Att_WriteTableHeadSeveralAttEvents (Events,ShowPhotos);

	 /***** List the users *****/
	 for (NumUsr = 0;
	      NumUsr < NumUsrsInList;
	      NumUsr++)
	   {
	    UsrDat.UsrCod = LstSelectedUsrCods[NumUsr];
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,		// Get from the database the data of the student
							 Usr_DONT_GET_PREFS,
							 Usr_DONT_GET_ROLE_IN_CRS))
	       if (Usr_CheckIfICanViewAtt (&UsrDat) == Usr_CAN)
		 {
		  UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&UsrDat);
		  Att_WriteRowUsrSeveralAttEvents (Events,NumUsr,&UsrDat,ShowPhotos);
		 }
	   }

	 /***** Last row with the total of users present in each event *****/
	 if (NumUsrsInList > 1)
	   {
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("colspan=\"%u\" class=\"RM DAT_STRONG_%s LINE_TOP\"",
			     ShowPhotos == Pho_PHOTOS_SHOW ? 4 :
							     3,
			     The_GetSuffix ());
		  HTM_Txt (Txt_Number_of_users); HTM_Colon ();
	       HTM_TD_End ();

	       for (NumAttEvent = 0, Total = 0;
		    NumAttEvent < Events->Num;
		    NumAttEvent++)
		  if (Events->Lst[NumAttEvent].Checked == HTM_CHECKED)
		    {
		     HTM_TD_LINE_TOP_Unsigned (Events->Lst[NumAttEvent].NumStdsFromList);
		     Total += Events->Lst[NumAttEvent].NumStdsFromList;
		    }

	       HTM_TD_LINE_TOP_Unsigned (Total);

	    HTM_TR_End ();
	   }

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Button to show more details *****/
      if (PutButtonShowDetails)
	 Att_PutButtonToShowDetails (Events);

   /***** End section with attendance table *****/
   HTM_SECTION_End ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/* Write table heading for listing of students in several attendance events **/
/*****************************************************************************/

static void Att_WriteTableHeadSeveralAttEvents (struct Att_Events *Events,
						Pho_ShowPhotos_t ShowPhotos)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Attendance;
   unsigned NumAttEvent;
   char StrNumAttEvent[Cns_MAX_DIGITS_UINT + 1];

   HTM_TR_Begin (NULL);

      HTM_TH_Span (Txt_ROLES_SINGUL_Abc[Rol_USR][Usr_SEX_UNKNOWN],HTM_HEAD_LEFT,
	           1,ShowPhotos == Pho_PHOTOS_SHOW ? 4 :
	        				     3,NULL);

      for (NumAttEvent = 0;
	   NumAttEvent < Events->Num;
	   NumAttEvent++)
	 if (Events->Lst[NumAttEvent].Checked == HTM_CHECKED)
	   {
	    /***** Get data of this attendance event *****/
	    Events->Event.AttCod = Events->Lst[NumAttEvent].AttCod;
	    Att_GetEventDataByCodAndCheckCrs (&Events->Event);

	    /***** Put link to this attendance event *****/
            HTM_TH_Begin (HTM_HEAD_CENTER);
	       snprintf (StrNumAttEvent,sizeof (StrNumAttEvent),"%u",NumAttEvent + 1);
	       Att_PutLinkEvent (&Events->Event,Events->Event.Title,StrNumAttEvent);
	    HTM_TH_End ();
	   }

      HTM_TH (Txt_Attendance ,HTM_HEAD_RIGHT);

   HTM_TR_End ();
  }

/*****************************************************************************/
/************** Write a row of a table with the data of a user ***************/
/*****************************************************************************/

static void Att_WriteRowUsrSeveralAttEvents (const struct Att_Events *Events,
                                             unsigned NumUsr,struct Usr_Data *UsrDat,
                                             Pho_ShowPhotos_t ShowPhotos)
  {
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   unsigned NumAttEvent;
   Att_AbsentOrPresent_t Present;
   unsigned NumTimesPresent;

   /***** Write number of user in the list *****/
   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
		    UsrDat->Accepted ? "DAT_STRONG" :
				       "DAT",
		    The_GetSuffix (),The_GetColorRows ());
	 HTM_Unsigned (NumUsr + 1);
      HTM_TD_End ();

      /***** Show user's photo *****/
      if (ShowPhotos == Pho_PHOTOS_SHOW)
	{
	 HTM_TD_Begin ("class=\"LM %s\"",The_GetColorRows ());
	    Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                               ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
	 HTM_TD_End ();
	}

      /***** Write user's ID ******/
      HTM_TD_Begin ("class=\"LM %s_%s %s\"",
		    UsrDat->Accepted ? "DAT_SMALL_STRONG" :
				       "DAT_SMALL",
		    The_GetSuffix (),The_GetColorRows ());
	 ID_WriteUsrIDs (UsrDat,NULL);
      HTM_TD_End ();

      /***** Write user's name *****/
      HTM_TD_Begin ("class=\"LM %s_%s %s\"",
		    UsrDat->Accepted ? "DAT_SMALL_STRONG" :
				       "DAT_SMALL",
		    The_GetSuffix (),The_GetColorRows ());
	 HTM_Txt (UsrDat->Surname1);
	 if (UsrDat->Surname2[0])
	   {
	    HTM_SP ();
	    HTM_Txt (UsrDat->Surname2);
	   }
	 HTM_Comma ();
	 HTM_SP ();
	 HTM_Txt (UsrDat->FrstName);
      HTM_TD_End ();

      /***** Check/cross to show if the user is present/absent *****/
      for (NumAttEvent = 0, NumTimesPresent = 0;
	   NumAttEvent < Events->Num;
	   NumAttEvent++)
	 if (Events->Lst[NumAttEvent].Checked == HTM_CHECKED)
	   {
	    /* Check if this student is already registered in the current event */
	    // Here it is not necessary to get comments
	    Present = Att_CheckIfUsrIsPresentInEvent (Events->Lst[NumAttEvent].AttCod,
						      UsrDat->UsrCod);

	    /* Write check or cross */
	    HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
	       Att_PutCheckOrCross (Present);
	    HTM_TD_End ();

	    if (Present == Att_PRESENT)
	       NumTimesPresent++;
	   }

      /***** Last column with the number of times this user is present *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 HTM_Unsigned (NumTimesPresent);
      HTM_TD_End ();

   HTM_TR_End ();

   The_ChangeRowColor ();
  }

/*****************************************************************************/
/*********************** Put check or cross character ************************/
/*****************************************************************************/

static void Att_PutCheckOrCross (Att_AbsentOrPresent_t Present)
  {
   extern const char *Txt_Present;
   extern const char *Txt_Absent;
   static const char *Class[Att_NUM_PRESENT] =
     {
      [Att_ABSENT ] = "ATT_CROSS",
      [Att_PRESENT] = "ATT_CHECK",
     };
   static const char **Title[Att_NUM_PRESENT] =
     {
      [Att_ABSENT ] = &Txt_Absent,
      [Att_PRESENT] = &Txt_Present,
     };
   static const char *Txt[Att_NUM_PRESENT] =
     {
      [Att_ABSENT ] = "&cross;",
      [Att_PRESENT] = "&check;",
     };

   HTM_DIV_Begin ("class=\"%s %s_%s\" title=\"%s\"",
		  Class[Present],
		  Class[Present],The_GetSuffix (),
		  *Title[Present]);
      HTM_Txt (Txt[Present]);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************** List the students with details and comments ****************/
/*****************************************************************************/

static void Att_ListStdsWithAttEventsDetails (struct Att_Events *Events,
                                              unsigned NumUsrsInList,
                                              long *LstSelectedUsrCods)
  {
   extern const char *Txt_Details;
   struct Usr_Data UsrDat;
   unsigned NumUsr;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Begin section with attendance details *****/
   HTM_SECTION_Begin (Att_ATTENDANCE_DETAILS_ID);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Details,NULL,NULL,NULL,Box_NOT_CLOSABLE);

	 /***** Begin table *****/
	 HTM_TABLE_Begin ("TBL_SCROLL");

	    /***** List students with attendance details *****/
	    for (NumUsr = 0;
		 NumUsr < NumUsrsInList;
		 NumUsr++)
	      {
	       UsrDat.UsrCod = LstSelectedUsrCods[NumUsr];
	       if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,	// Get from the database the data of the student
							    Usr_DONT_GET_PREFS,
							    Usr_DONT_GET_ROLE_IN_CRS))
		  if (Usr_CheckIfICanViewAtt (&UsrDat) == Usr_CAN)
		    {
		     UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&UsrDat);
		     Att_ListAttEventsForAStd (Events,NumUsr,&UsrDat);
		    }
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();

      /***** End box *****/
      Box_BoxEnd ();

   /***** End section with attendance details *****/
   HTM_SECTION_End ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/*************** Write list of attendance events for a student ***************/
/*****************************************************************************/

static void Att_ListAttEventsForAStd (struct Att_Events *Events,
                                      unsigned NumUsr,struct Usr_Data *UsrDat)
  {
   extern const char *Txt_Student_comment;
   extern const char *Txt_Teachers_comment;
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   static const char *Class[Att_NUM_PRESENT] =
     {
      [Att_ABSENT ] = "DAT_RED",
      [Att_PRESENT] = "DAT_GREEN",
     };
   unsigned NumAttEvent;
   unsigned UniqueId;
   char *Id;
   Att_AbsentOrPresent_t Present;
   bool ShowCommentStd;
   bool ShowCommentTch;
   char CommentStd[Cns_MAX_BYTES_TEXT + 1];
   char CommentTch[Cns_MAX_BYTES_TEXT + 1];

   /***** Write number of student in the list *****/
   NumUsr++;
   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
		    UsrDat->Accepted ? "DAT_STRONG" :
				       "DAT",
		    The_GetSuffix (),The_GetColorRows ());
	 HTM_Unsigned (NumUsr); HTM_Colon ();
      HTM_TD_End ();

      /***** Show student's photo *****/
      HTM_TD_Begin ("colspan=\"2\" class=\"RM %s\"",
                    The_GetColorRows ());
	 Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                            ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LM %s\"",The_GetColorRows ());

	 HTM_TABLE_Begin (NULL);
	    HTM_TR_Begin (NULL);

	       /***** Write user's ID ******/
	       HTM_TD_Begin ("class=\"LM %s_%s\"",
			     UsrDat->Accepted ? "DAT_SMALL_STRONG" :
						"DAT_SMALL",
			     The_GetSuffix ());
		  ID_WriteUsrIDs (UsrDat,NULL);
	       HTM_TD_End ();

	       /***** Write student's name *****/
	       HTM_TD_Begin ("class=\"LM %s_%s\"",
			     UsrDat->Accepted ? "DAT_SMALL_STRONG" :
						"DAT_SMALL",
			     The_GetSuffix ());
		  HTM_Txt (UsrDat->Surname1);
		  if (UsrDat->Surname2[0])
		    {
		     HTM_SP ();
		     HTM_Txt (UsrDat->Surname2);
		    }
		  HTM_Comma ();
		  HTM_SP ();
		  HTM_Txt (UsrDat->FrstName);
	       HTM_TD_End ();

	    HTM_TR_End ();
	 HTM_TABLE_End ();

      HTM_TD_End ();

   HTM_TR_End ();

   /***** List the events with students *****/
   for (NumAttEvent = 0, UniqueId = 1;
	NumAttEvent < Events->Num;
	NumAttEvent++, UniqueId++)
      if (Events->Lst[NumAttEvent].Checked == HTM_CHECKED)
	{
	 /***** Get data of the attendance event from database *****/
	 Events->Event.AttCod = Events->Lst[NumAttEvent].AttCod;
	 Att_GetEventDataByCodAndCheckCrs (&Events->Event);
         Events->Event.NumStdsTotal = Att_DB_GetNumStdsTotalWhoAreInEvent (Events->Event.AttCod);

	 /***** Get comments for this student *****/
	 Present = Att_CheckIfUsrIsPresentInEventAndGetComments (Events->Event.AttCod,UsrDat->UsrCod,
								 CommentStd,CommentTch);
         ShowCommentStd = CommentStd[0];
	 ShowCommentTch = CommentTch[0] &&
	                  (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
	                   Events->Event.CommentTchVisible == HidVis_VISIBLE);

	 /***** Write a row for this event *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_ColouredEmpty (1);

	    HTM_TD_Begin ("class=\"RT %s_%s %s\"",
			  Class[Present],The_GetSuffix (),The_GetColorRows ());
	       HTM_Unsigned (NumAttEvent + 1); HTM_Colon ();
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"BT %s\"",The_GetColorRows ());
	       Att_PutCheckOrCross (Present);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	                  The_GetSuffix (),The_GetColorRows ());
	       if (asprintf (&Id,"att_date_start_%u_%u",NumUsr,UniqueId) < 0)
		  Err_NotEnoughMemoryExit ();
	       HTM_SPAN_Begin ("id=\"%s\"",Id);
	       HTM_SPAN_End ();
	       HTM_BR ();
	       HTM_Txt (Events->Event.Title);
	       Dat_WriteLocalDateHMSFromUTC (Id,Events->Event.TimeUTC[Dat_STR_TIME],
					     Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
					     Dat_WRITE_TODAY |
					     Dat_WRITE_DATE_ON_SAME_DAY |
					     Dat_WRITE_WEEK_DAY |
					     Dat_WRITE_HOUR |
					     Dat_WRITE_MINUTE |
					     Dat_WRITE_SECOND);
	       free (Id);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Write comments for this student *****/
	 if (ShowCommentStd || ShowCommentTch)
	   {
	    HTM_TR_Begin (NULL);

	       HTM_TD_ColouredEmpty (2);

	       HTM_TD_Begin ("class=\"BT %s\"",The_GetColorRows ());
	       HTM_TD_End ();

	       HTM_TD_Begin ("class=\"LM DAT_%s %s\"",
	                     The_GetSuffix (),The_GetColorRows ());

		  HTM_DL_Begin ();
		     if (ShowCommentStd)
		       {
			Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
					  CommentStd,Cns_MAX_BYTES_TEXT,
					  Str_DONT_REMOVE_SPACES);
			HTM_DT_Begin ();
			   HTM_Txt (Txt_Student_comment); HTM_Colon ();
			HTM_DT_End ();
			HTM_DD_Begin ();
			   HTM_Txt (CommentStd);
			HTM_DD_End ();
		       }
		     if (ShowCommentTch)
		       {
			Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
					  CommentTch,Cns_MAX_BYTES_TEXT,
					  Str_DONT_REMOVE_SPACES);
			HTM_DT_Begin ();
			   HTM_Txt (Txt_Teachers_comment); HTM_Colon ();
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

   The_ChangeRowColor ();
  }
