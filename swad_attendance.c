// swad_attendance.c: control of attendance

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <mysql/mysql.h>	// To access MySQL databases
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_attendance.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_ID.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_QR.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;
extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Private types ******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Private variables ****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static void Att_ShowAllAttEvents (void);
static void Att_PutFormToSelectWhichGroupsToShow (void);
static void Att_ShowOneAttEvent (struct AttendanceEvent *Att,bool ShowOnlyThisAttEventComplete);
static void Att_WriteAttEventAuthor (struct AttendanceEvent *Att);
static void Att_GetParamAttOrderType (void);
static void Att_PutFormToListStds (void);
static void Att_PutFormToCreateNewAttEvent (void);
static void Att_PutFormsToRemEditOneAttEvent (long AttCod,bool Hidden);
static void Att_GetListAttEvents (Att_OrderTime_t Order);
static void Att_GetDataOfAttEventByCodAndCheckCrs (struct AttendanceEvent *Att);
static void Att_GetAttEventTxtFromDB (long AttCod,char *Txt);
static bool Att_CheckIfSimilarAttEventExists (const char *Field,const char *Value,long AttCod);
static void Att_ShowLstGrpsToEditAttEvent (long AttCod);
static void Att_RemoveAllTheGrpsAssociatedToAnAttEvent (long AttCod);
static void Att_CreateGrps (long AttCod);
static void Att_GetAndWriteNamesOfGrpsAssociatedToAttEvent (struct AttendanceEvent *Att);

static void Att_ListAttOnlyMeAsStudent (struct AttendanceEvent *Att);
static void Att_ListAttStudents (struct AttendanceEvent *Att);
static void Att_WriteRowStdToCallTheRoll (unsigned NumStd,struct UsrData *UsrDat,struct AttendanceEvent *Att);
static void Att_PutParamsCodGrps (long AttCod);
static void Att_GetNumStdsTotalWhoAreInAttEvent (struct AttendanceEvent *Att);
static unsigned Att_GetNumStdsFromAListWhoAreInAttEvent (long AttCod,long LstSelectedUsrCods[],unsigned NumStdsInList);
static bool Att_CheckIfUsrIsInTableAttUsr (long AttCod,long UsrCod,bool *Present);
static bool Att_CheckIfUsrIsPresentInAttEvent (long AttCod,long UsrCod);
static bool Att_CheckIfUsrIsPresentInAttEventAndGetComments (long AttCod,long UsrCod,char *CommentStd,char *CommentTch);
static void Att_RegUsrInAttEventChangingComments (long AttCod,long UsrCod,bool Present,
                                                  const char *CommentStd,const char *CommentTch);
static void Att_RemoveUsrFromAttEvent (long AttCod,long UsrCod);

static void Att_GetListSelectedUsrCods (unsigned NumStdsInList,long **LstSelectedUsrCods);
static void Att_GetListSelectedAttCods (char **StrAttCodsSelected);
static void Att_PutFormToPrintListStds (bool ShowDetails,char *StrAttCodsSelected);
static void Att_PutButtonToShowDetails (char *StrAttCodsSelected);
static void Att_ListEventsToSelect (void);
static void Att_ListStdsAttendanceTable (unsigned NumStdsInList,long *LstSelectedUsrCods);
static void Att_WriteTableHeadSeveralAttEvents (void);
static void Att_WriteRowStdSeveralAttEvents (unsigned NumStd,struct UsrData *UsrDat);
static void Att_ListStdsWithAttEventsDetails (unsigned NumStdsInList,long *LstSelectedUsrCods);
static void Att_ListAttEventsForAStd (unsigned NumStd,struct UsrData *UsrDat);

/*****************************************************************************/
/********************** List all the attendance events ***********************/
/*****************************************************************************/

void Att_SeeAttEvents (void)
  {
   /***** Get parameters *****/
   Att_GetParamAttOrderType ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_ATT_EVENTS);

   /***** Put other options *****/
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");

   /* Put link (form) to create a bew attendance event */
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_TEACHER:
      case Rol_ROLE_SUPERUSER:
         Att_PutFormToCreateNewAttEvent ();
	 Att_PutFormToListStds ();
         break;
      default:
         break;
     }

   /* Put link to my QR code */
   if (Gbl.Usrs.Me.UsrDat.Nickname[0])
      QR_PutLinkToPrintQRCode (QR_NICKNAME,&Gbl.Usrs.Me.UsrDat,true);

   fprintf (Gbl.F.Out,"</div>");

   /***** Show all the attendance events *****/
   Att_ShowAllAttEvents ();
  }

/*****************************************************************************/
/********************** Show all the attendance events ***********************/
/*****************************************************************************/

static void Att_ShowAllAttEvents (void)
  {
   extern const char *Txt_ASG_ATT_OR_SVY_HELP_ORDER[2];
   extern const char *Txt_ASG_ATT_OR_SVY_ORDER[2];
   extern const char *Txt_Event;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_No_events;
   Att_EventsOrderType_t Order;
   struct Pagination Pagination;
   unsigned NumAttEvent;

   /***** Get list of attendance events *****/
   Att_GetListAttEvents (Att_NEWEST_FIRST);

   /***** Select whether show only my groups or all groups *****/
   if (Gbl.CurrentCrs.Grps.NumGrps)
      Att_PutFormToSelectWhichGroupsToShow ();

   if (Gbl.AttEvents.Num)	// There are attendance events in current course
     {
      /***** Compute variables related to pagination *****/
      Pagination.NumItems = Gbl.AttEvents.Num;
      Pagination.CurrentPage = (int) Gbl.Pag.CurrentPage;
      Pag_CalculatePagination (&Pagination);
      Gbl.Pag.CurrentPage = (unsigned) Pagination.CurrentPage;

      /***** Write links to pages *****/
      if (Pagination.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (Pag_ATT_EVENTS,0,&Pagination);

      /***** Table head *****/
      Lay_StartRoundFrameTable10 (NULL,2,NULL);
      fprintf (Gbl.F.Out,"<tr>");
      for (Order = Att_ORDER_BY_START_DATE;
	   Order <= Att_ORDER_BY_END_DATE;
	   Order++)
        {
         fprintf (Gbl.F.Out,"<th align=\"left\" class=\"TIT_TBL\">");
         Act_FormStart (ActSeeAtt);
         Grp_PutParamWhichGrps ();
         Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
         Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
         Act_LinkFormSubmit (Txt_ASG_ATT_OR_SVY_HELP_ORDER[Order],"TIT_TBL");
         if (Order == Gbl.AttEvents.SelectedOrderType)
            fprintf (Gbl.F.Out,"<u>");
         fprintf (Gbl.F.Out,"%s",Txt_ASG_ATT_OR_SVY_ORDER[Order]);
         if (Order == Gbl.AttEvents.SelectedOrderType)
            fprintf (Gbl.F.Out,"</u>");
         fprintf (Gbl.F.Out,"</a>"
                            "</form>"
                            "</th>");
        }
      fprintf (Gbl.F.Out,"<th align=\"left\" class=\"TIT_TBL\">%s</th>"
	                 "<th align=\"right\" class=\"TIT_TBL\">%s</th>"
	                 "</tr>",
               Txt_Event,
               Txt_ROLES_PLURAL_Abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN]);

      /***** Write all the attendance events *****/
      for (NumAttEvent = Pagination.FirstItemVisible, Gbl.RowEvenOdd = 0;
           NumAttEvent <= Pagination.LastItemVisible;
           NumAttEvent++)
         Att_ShowOneAttEvent (&Gbl.AttEvents.Lst[NumAttEvent-1],false);

      /***** Table end *****/
      Lay_EndRoundFrameTable10 ();

      /***** Write again links to pages *****/
      if (Pagination.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (Pag_ATT_EVENTS,0,&Pagination);
     }
   else	// There are no attendance events in current course
      Lay_ShowAlert (Lay_INFO,Txt_No_events);

   /***** Free list of attendance events *****/
   Att_FreeListAttEvents ();
  }

/*****************************************************************************/
/***************** Put form to select which groups to show *******************/
/*****************************************************************************/

static void Att_PutFormToSelectWhichGroupsToShow (void)
  {
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
   Act_FormStart (ActSeeAtt);
   Att_PutHiddenParamAttOrderType ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   Grp_ShowSelectorWhichGrps ();
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");
  }

/*****************************************************************************/
/************************* Show one attendance event *************************/
/*****************************************************************************/
// Only Att->AttCod must be filled

static void Att_ShowOneAttEvent (struct AttendanceEvent *Att,bool ShowOnlyThisAttEventComplete)
  {
   extern const char *Txt_View_event;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Get data of this attendance event *****/
   Att_GetDataOfAttEventByCodAndCheckCrs (Att);
   Att_GetNumStdsTotalWhoAreInAttEvent (Att);

   /***** Start date/time *****/
   fprintf (Gbl.F.Out,"<tr>"  \
	              "<td class=\"%s\""
	              " style=\"text-align:left; vertical-align:top;\"",
            Att->Hidden ? (Att->Open ? "DATE_GREEN_LIGHT" :
        	                       "DATE_RED_LIGHT") :
                          (Att->Open ? "DATE_GREEN" :
                        	       "DATE_RED"));
   if (!ShowOnlyThisAttEventComplete)
      fprintf (Gbl.F.Out," bgcolor=\"%s\"",Gbl.ColorRows[Gbl.RowEvenOdd]);
   fprintf (Gbl.F.Out,">%02u/%02u/%02u<br />%02u:%02u h</td>",
            Att->DateTimes[Att_START_TIME].Date.Day,
            Att->DateTimes[Att_START_TIME].Date.Month,
	    Att->DateTimes[Att_START_TIME].Date.Year % 100,
	    Att->DateTimes[Att_START_TIME].Time.Hour,
	    Att->DateTimes[Att_START_TIME].Time.Minute);

   /***** End date/time *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\""
	              " style=\"text-align:left; vertical-align:top;",
            Att->Hidden ? (Att->Open ? "DATE_GREEN_LIGHT" :
        	                       "DATE_RED_LIGHT") :
                          (Att->Open ? "DATE_GREEN" :
                        	       "DATE_RED"));
   if (!ShowOnlyThisAttEventComplete)
      fprintf (Gbl.F.Out," background-color:%s;",
               Gbl.ColorRows[Gbl.RowEvenOdd]);
   fprintf (Gbl.F.Out,"\">"
	              "%02u/%02u/%02u<br />%02u:%02u h"
	              "</td>",
            Att->DateTimes[Att_END_TIME  ].Date.Day,
            Att->DateTimes[Att_END_TIME  ].Date.Month,
            Att->DateTimes[Att_END_TIME  ].Date.Year % 100,
	    Att->DateTimes[Att_END_TIME  ].Time.Hour,
	    Att->DateTimes[Att_END_TIME  ].Time.Minute);

   /***** Attendance event title *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:left; vertical-align:top;");
   if (!ShowOnlyThisAttEventComplete)
      fprintf (Gbl.F.Out," background-color:%s;",
               Gbl.ColorRows[Gbl.RowEvenOdd]);
   fprintf (Gbl.F.Out,"\">");

   /* Put form to view attendance event */
   Act_FormStart (ActSeeOneAtt);
   Att_PutParamAttCod (Att->AttCod);
   Att_PutParamsCodGrps (Att->AttCod);
   Act_LinkFormSubmit (Txt_View_event,Att->Hidden ? "ASG_TITLE_LIGHT" :
	                                            "ASG_TITLE");
   fprintf (Gbl.F.Out,"%s</a></form></td>",
            Att->Title);

   /***** Number of students in this event *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT_N\""
	              " style=\"text-align:right; vertical-align:top;");
   if (!ShowOnlyThisAttEventComplete)
      fprintf (Gbl.F.Out," background-color:%s;",
               Gbl.ColorRows[Gbl.RowEvenOdd]);
   fprintf (Gbl.F.Out,"\">"
	              "%u"
	              "</td>"
	              "</tr>",
            Att->NumStdsTotal);

   /***** Write second row of data of this attendance event *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" style=\"text-align:left;"
	              " vertical-align:top; background-color:%s;\">",
            Gbl.ColorRows[Gbl.RowEvenOdd]);

   /* Author of the attendance event */
   Att_WriteAttEventAuthor (Att);

   /* Forms to remove/edit this attendance event */
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_TEACHER:
      case Rol_ROLE_SUPERUSER:
         Att_PutFormsToRemEditOneAttEvent (Att->AttCod,Att->Hidden);
         break;
      default:
         break;
     }
   fprintf (Gbl.F.Out,"</td>");

   /* Text of the attendance event */
   Att_GetAttEventTxtFromDB (Att->AttCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
   Str_InsertLinkInURLs (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   fprintf (Gbl.F.Out,"<td colspan=\"2\""
	              " style=\"text-align:left; background-color:%s;\">",
            Gbl.ColorRows[Gbl.RowEvenOdd]);

   if (Gbl.CurrentCrs.Grps.NumGrps)
      Att_GetAndWriteNamesOfGrpsAssociatedToAttEvent (Att);

   fprintf (Gbl.F.Out,"<p align=\"justify\" class=\"%s\">%s</p>",
            Att->Hidden ? "DAT_LIGHT" :
        	          "DAT",
            Txt);

   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/****************** Write the author of an attendance event ******************/
/*****************************************************************************/

static void Att_WriteAttEventAuthor (struct AttendanceEvent *Att)
  {
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX+1];
   char FirstName[Usr_MAX_BYTES_NAME+1];
   char Surnames[2*(Usr_MAX_BYTES_NAME+1)];
   struct UsrData UsrDat;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Get data of author *****/
   UsrDat.UsrCod = Att->UsrCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))	// Get of the database the data of the author
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);

   /***** Show photo *****/
   Pho_ShowUsrPhoto (&UsrDat,
                     ShowPhoto ? PhotoURL :
                	         NULL,
                     12,16,true);

   /***** Write name *****/
   strcpy (FirstName,UsrDat.FirstName);
   strcpy (Surnames,UsrDat.Surname1);
   if (UsrDat.Surname2[0])
     {
      strcat (Surnames," ");
      strcat (Surnames,UsrDat.Surname2);
     }
   Str_LimitLengthHTMLStr (FirstName,9);
   Str_LimitLengthHTMLStr (Surnames,9);
   fprintf (Gbl.F.Out,"<span class=\"%s\">%s %s</span>",
            Att->Hidden ? "MSG_AUT_LIGHT" :
        	          "MSG_AUT",
            FirstName,Surnames);

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/**** Get parameter with the type or order in list of attendance events ******/
/*****************************************************************************/

static void Att_GetParamAttOrderType (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("Order",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      Gbl.AttEvents.SelectedOrderType = (Att_EventsOrderType_t) UnsignedNum;
   else
      Gbl.AttEvents.SelectedOrderType = Att_DEFAULT_ORDER_TYPE;
  }

/*****************************************************************************/
/*** Put a hidden parameter with the type of order in list of att. events ****/
/*****************************************************************************/

void Att_PutHiddenParamAttOrderType (void)
  {
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.AttEvents.SelectedOrderType);
  }

/*****************************************************************************/
/**** Put a link (form) to list assistance of students to several events *****/
/*****************************************************************************/

static void Att_PutFormToListStds (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Attendance_list;

   /***** Put form to create a new attendance event *****/
   Act_FormStart (ActReqLstAttStd);
   Att_PutHiddenParamAttOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   Act_LinkFormSubmit (Txt_Attendance_list,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("list",Txt_Attendance_list,Txt_Attendance_list);
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/*********** Put a link (form) to create a new attendance event **************/
/*****************************************************************************/

static void Att_PutFormToCreateNewAttEvent (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_New_event;

   /***** Put form to create a new attendance event *****/
   Act_FormStart (ActFrmNewAtt);
   Att_PutHiddenParamAttOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   Act_LinkFormSubmit (Txt_New_event,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("new",Txt_New_event,Txt_New_event);
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/************** Put a link (form) to edit one attendance event ***************/
/*****************************************************************************/

static void Att_PutFormsToRemEditOneAttEvent (long AttCod,bool Hidden)
  {
   extern const char *Txt_Remove;
   extern const char *Txt_Show;
   extern const char *Txt_Hide;
   extern const char *Txt_Edit;

   /***** Put form to remove attendance event *****/
   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_4\">"
	              "<tr>"
                      "<td style=\"text-align:left;\">");
   Act_FormStart (ActReqRemAtt);
   Att_PutParamAttCod (AttCod);
   Att_PutHiddenParamAttOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/delon16x16.gif\""
	              " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />"
                      "</form>"
                      "</td>",
            Gbl.Prefs.IconsURL,
            Txt_Remove,
            Txt_Remove);

   /***** Put form to hide/show attendance event *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:left;\">");
   Act_FormStart (Hidden ? ActShoAtt :
	                   ActHidAtt);
   Att_PutParamAttCod (AttCod);
   Att_PutHiddenParamAttOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   if (Hidden)
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/hidden_on16x16.gif\""
			 " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />",
	       Gbl.Prefs.IconsURL,
	       Txt_Show,
	       Txt_Show);
   else
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/visible_on16x16.gif\""
			 " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />",
	       Gbl.Prefs.IconsURL,
	       Txt_Hide,
	       Txt_Hide);
   fprintf (Gbl.F.Out,"</form>"
	              "</td>");

   /***** Put form to edit attendance event *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:left;\">");
   Act_FormStart (ActEdiOneAtt);
   Att_PutParamAttCod (AttCod);
   Att_PutHiddenParamAttOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/edit16x16.gif\""
	              " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />"
                      "</form>"
                      "</td>"
                      "</tr>"
	              "</table>",
            Gbl.Prefs.IconsURL,
            Txt_Edit,
            Txt_Edit);
  }

/*****************************************************************************/
/********************* List all the attendance events ************************/
/*****************************************************************************/

static void Att_GetListAttEvents (Att_OrderTime_t Order)
  {
   char HiddenSubQuery[256];
   char OrderBySubQuery[256];
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumAttEvent;

   if (Gbl.AttEvents.LstIsRead)
      Att_FreeListAttEvents ();

   /***** Get list of attendance events from database *****/
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_TEACHER:
      case Rol_ROLE_SUPERUSER:
         HiddenSubQuery[0] = '\0';
         break;
      default:
         sprintf (HiddenSubQuery,"AND Hidden='N'");
         break;
     }
   switch (Gbl.AttEvents.SelectedOrderType)
     {
      case Att_ORDER_BY_START_DATE:
	 if (Order == Att_NEWEST_FIRST)
            sprintf (OrderBySubQuery,"StartTime DESC,EndTime DESC,Title DESC");
	 else
            sprintf (OrderBySubQuery,"StartTime,EndTime,Title");
         break;
      case Att_ORDER_BY_END_DATE:
	 if (Order == Att_NEWEST_FIRST)
            sprintf (OrderBySubQuery,"EndTime DESC,StartTime DESC,Title DESC");
	 else
	    sprintf (OrderBySubQuery,"EndTime,StartTime,Title");
	 break;
     }
   if (Gbl.CurrentCrs.Grps.WhichGrps == Grp_ONLY_MY_GROUPS)
      sprintf (Query,"SELECT AttCod"
                     " FROM att_events"
                     " WHERE CrsCod='%ld'%s"
                     " AND (AttCod NOT IN (SELECT AttCod FROM att_grp) OR"
                     " AttCod IN (SELECT att_grp.AttCod FROM att_grp,crs_grp_usr"
                     " WHERE crs_grp_usr.UsrCod='%ld' AND att_grp.GrpCod=crs_grp_usr.GrpCod))"
                     " ORDER BY %s",
               Gbl.CurrentCrs.Crs.CrsCod,HiddenSubQuery,Gbl.Usrs.Me.UsrDat.UsrCod,OrderBySubQuery);
   else	// Gbl.CurrentCrs.Grps.WhichGrps == Grp_ALL_GROUPS
      sprintf (Query,"SELECT AttCod"
                     " FROM att_events"
                     " WHERE CrsCod='%ld'%s"
                     " ORDER BY %s",
               Gbl.CurrentCrs.Crs.CrsCod,HiddenSubQuery,OrderBySubQuery);

   if ((NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get attendance events"))) // Attendance events found...
     {
      Gbl.AttEvents.Num = (unsigned) NumRows;

      /***** Create list of attendance events *****/
      if ((Gbl.AttEvents.Lst = (struct AttendanceEvent *) calloc (NumRows,sizeof (struct AttendanceEvent))) == NULL)
          Lay_ShowErrorAndExit ("Not enough memory to store list of attendance events.");

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
      if (Att->CrsCod != Gbl.CurrentCrs.Crs.CrsCod)
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
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool Found;

   /***** Build query *****/
   sprintf (Query,"SELECT AttCod,CrsCod,Hidden,UsrCod,"
                  "DATE_FORMAT(StartTime,'%%Y%%m%%d%%H%%i%%S'),"
                  "DATE_FORMAT(EndTime,'%%Y%%m%%d%%H%%i%%S'),"
                  "NOW() BETWEEN StartTime AND EndTime,"
                  "CommentTchVisible,"
                  "Title"
                  " FROM att_events"
                  " WHERE AttCod='%ld'",
            Att->AttCod);

   /***** Clear data *****/
   Att->DateTimes[Att_START_TIME].Date.Day   =
   Att->DateTimes[Att_START_TIME].Date.Month =
   Att->DateTimes[Att_START_TIME].Date.Year  = 0;
   Att->Title[0] = '\0';

   /***** Get data of attendance event from database *****/
   if ((Found = (DB_QuerySELECT (Query,&mysql_res,"can not get attendance event data") != 0))) // Attendance event found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get code of the attendance event (row[0]) */
      Att->AttCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get code of the course (row[1]) */
      Att->CrsCod = Str_ConvertStrCodToLongCod (row[1]);

      /* Get wether the attendance event is hidden or not (row[2]) */
      Att->Hidden = (Str_ConvertToUpperLetter (row[2][0]) == 'Y');

      /* Get author of the attendance event (row[3]) */
      Att->UsrCod = Str_ConvertStrCodToLongCod (row[3]);

      /* Get start date (row[4] holds the start date in YYYYMMDDHHMMSS format) */
      if (!(Dat_GetDateTimeFromYYYYMMDDHHMMSS (&(Att->DateTimes[Att_START_TIME]),row[4])))
	 Lay_ShowErrorAndExit ("Error when reading start date of attendance event.");

      /* Get end date (row[5] holds the end date in YYYYMMDDHHMMSS format) */
      if (!(Dat_GetDateTimeFromYYYYMMDDHHMMSS (&(Att->DateTimes[Att_END_TIME]),row[5])))
	 Lay_ShowErrorAndExit ("Error when reading end date of attendance event.");

      /* Get whether the attendance event is open or closed (row(6)) */
      Att->Open = (row[6][0] == '1');

      /* Get whether the attendance event is visible or not (row[7]) */
      Att->CommentTchVisible = (row[7][0] == 'Y');

      /* Get the title of the attendance event (row[8]) */
      strcpy (Att->Title,row[8]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Found;
  }

/*****************************************************************************/
/********************** Free list of attendance events ***********************/
/*****************************************************************************/

void Att_FreeListAttEvents (void)
  {
   if (Gbl.AttEvents.LstIsRead && Gbl.AttEvents.Lst)
     {
      /***** Free memory used by the list of attendance events *****/
      free ((void *) Gbl.AttEvents.Lst);
      Gbl.AttEvents.Lst = NULL;
      Gbl.AttEvents.Num = 0;
      Gbl.AttEvents.LstIsRead = false;
     }
  }

/*****************************************************************************/
/***************** Get attendance event text from database *******************/
/*****************************************************************************/

static void Att_GetAttEventTxtFromDB (long AttCod,char *Txt)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get text of attendance event from database *****/
   sprintf (Query,"SELECT Txt FROM att_events WHERE AttCod='%ld' AND CrsCod='%ld'",
            AttCod,Gbl.CurrentCrs.Crs.CrsCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get attendance event text");

   /***** The result of the query must have one row or none *****/
   if (NumRows == 1)
     {
      /* Get info text */
      row = mysql_fetch_row (mysql_res);
      strcpy (Txt,row[0]);
     }
   else
      Txt[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumRows > 1)
      Lay_ShowErrorAndExit ("Error when getting attendance event text.");
  }

/*****************************************************************************/
/************** Get summary and content of an attendance event ***************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void Att_GetNotifAttEvent (char *SummaryStr,char **ContentStr,long AttCod,unsigned MaxChars,bool GetContent)
  {
   extern const char *Txt_Start_date;
   extern const char *Txt_End_date;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct DateTime DateTimes[Att_NUM_DATES];

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Build query *****/
   sprintf (Query,"SELECT Title,"
                  "DATE_FORMAT(StartTime,'%%Y%%m%%d%%H%%i%%S'),"
                  "DATE_FORMAT(EndTime,'%%Y%%m%%d%%H%%i%%S'),"
                  "Txt"
                  " FROM att_events"
                  " WHERE AttCod='%ld'",
            AttCod);
   if (!mysql_query (&Gbl.mysql,Query))
      if ((mysql_res = mysql_store_result (&Gbl.mysql)) != NULL)
        {
         /***** Result should have a unique row *****/
         if (mysql_num_rows (mysql_res) == 1)
           {
            /***** Get row *****/
            row = mysql_fetch_row (mysql_res);

            /***** Get summary *****/
            strcpy (SummaryStr,row[0]);
            if (MaxChars)
               Str_LimitLengthHTMLStr (SummaryStr,MaxChars);

            /***** Get content *****/
            if (GetContent)
              {
               if ((*ContentStr = (char *) malloc (512+Cns_MAX_BYTES_TEXT)) == NULL)
                  Lay_ShowErrorAndExit ("Error allocating memory for notification content.");
               (*ContentStr)[0] = '\0';	// Return nothing on error

               /* Get start date (row[1] holds the start date in YYYYMMDDHHMMSS format) */
               if (!(Dat_GetDateTimeFromYYYYMMDDHHMMSS (&DateTimes[Att_START_TIME],row[1])))
	          Lay_ShowErrorAndExit ("Error when reading start date of attendance event.");

               /* Get end date (row[2] holds the end date in YYYYMMDDHHMMSS format) */
               if (!(Dat_GetDateTimeFromYYYYMMDDHHMMSS (&DateTimes[Att_END_TIME  ],row[2])))
	          Lay_ShowErrorAndExit ("Error when reading end date of attendance event.");

               sprintf (*ContentStr,"%s: %02u/%02u/%04u %02u:%02u<br />%s: %02u/%02u/%04u %02u:%02u<br />%s",
                        Txt_Start_date,
                        DateTimes[Att_START_TIME].Date.Day,
                        DateTimes[Att_START_TIME].Date.Month,
                        DateTimes[Att_START_TIME].Date.Year,
                        DateTimes[Att_START_TIME].Time.Hour,
                        DateTimes[Att_START_TIME].Time.Minute,
                        Txt_End_date,
                        DateTimes[Att_END_TIME].Date.Day,
                        DateTimes[Att_END_TIME].Date.Month,
                        DateTimes[Att_END_TIME].Date.Year,
                        DateTimes[Att_END_TIME].Time.Hour,
                        DateTimes[Att_END_TIME].Time.Minute,
                        row[3]);
              }
           }
         mysql_free_result (mysql_res);
        }
  }

/*****************************************************************************/
/************** Write parameter with code of attendance event ****************/
/*****************************************************************************/

void Att_PutParamAttCod (long AttCod)
  {
   Par_PutHiddenParamLong ("AttCod",AttCod);
  }

/*****************************************************************************/
/*************** Get parameter with code of attendance event *****************/
/*****************************************************************************/

long Att_GetParamAttCod (void)
  {
   char LongStr[1+10+1];

   /***** Get parameter with code of attendance event *****/
   Par_GetParToText ("AttCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
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
   Att_GetParamAttOrderType ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_ATT_EVENTS);

   /***** Get attendance event code *****/
   if ((Att.AttCod = Att_GetParamAttCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of attendance event is missing.");

   /***** Get data of the attendance event from database *****/
   Att_GetDataOfAttEventByCodAndCheckCrs (&Att);

   /***** Button of confirmation of removing *****/
   Act_FormStart (ActRemAtt);
   Att_PutParamAttCod (Att.AttCod);
   Att_PutHiddenParamAttOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);

   /***** Ask for confirmation of removing *****/
   sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_event_X,
            Att.Title);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">"
	              "<input type=\"submit\" value=\"%s\" />"
	              "</div>"
	              "</form>",
            Txt_Remove_event);

   /***** Show attendance events again *****/
   Att_SeeAttEvents ();
  }

/*****************************************************************************/
/************************ Remove an attendance event *************************/
/*****************************************************************************/

void Att_RemoveAttEvent (void)
  {
   extern const char *Txt_Event_X_removed;
   char Query[512];
   struct AttendanceEvent Att;

   /***** Get attendance event code *****/
   if ((Att.AttCod = Att_GetParamAttCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of attendance event is missing.");

   /***** Get data of the attendance event from database *****/
   Att_GetDataOfAttEventByCodAndCheckCrs (&Att);	// Inside this function, the course is checked to be the current one

   /***** Remove users registered in the attendance event *****/
   sprintf (Query,"DELETE FROM att_usr"
                  " WHERE AttCod='%ld'",
            Att.AttCod);
   DB_QueryDELETE (Query,"can not remove attendance event");

   /***** Remove all the groups of this attendance event *****/
   Att_RemoveAllTheGrpsAssociatedToAnAttEvent (Att.AttCod);

   /***** Remove attendance event *****/
   sprintf (Query,"DELETE FROM att_events"
                  " WHERE AttCod='%ld' AND CrsCod='%ld'",
            Att.AttCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryDELETE (Query,"can not remove attendance event");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Event_X_removed,
            Att.Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show attendance events again *****/
   Att_SeeAttEvents ();
  }

/*****************************************************************************/
/************************* Hide an attendance event **************************/
/*****************************************************************************/

void Att_HideAttEvent (void)
  {
   extern const char *Txt_Event_X_is_now_hidden;
   char Query[512];
   struct AttendanceEvent Att;

   /***** Get attendance event code *****/
   if ((Att.AttCod = Att_GetParamAttCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of attendance event is missing.");

   /***** Get data of the attendance event from database *****/
   Att_GetDataOfAttEventByCodAndCheckCrs (&Att);

   /***** Hide attendance event *****/
   sprintf (Query,"UPDATE att_events SET Hidden='Y'"
                  " WHERE AttCod='%ld' AND CrsCod='%ld'",
            Att.AttCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not hide attendance event");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Event_X_is_now_hidden,
            Att.Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show attendance events again *****/
   Att_SeeAttEvents ();
  }

/*****************************************************************************/
/************************* Show an attendance event **************************/
/*****************************************************************************/

void Att_ShowAttEvent (void)
  {
   extern const char *Txt_Event_X_is_now_visible;
   char Query[512];
   struct AttendanceEvent Att;

   /***** Get attendance event code *****/
   if ((Att.AttCod = Att_GetParamAttCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of attendance event is missing.");

   /***** Get data of the attendance event from database *****/
   Att_GetDataOfAttEventByCodAndCheckCrs (&Att);

   /***** Hide attendance event *****/
   sprintf (Query,"UPDATE att_events SET Hidden='N'"
                  " WHERE AttCod='%ld' AND CrsCod='%ld'",
            Att.AttCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not show attendance event");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Event_X_is_now_visible,
            Att.Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show attendance events again *****/
   Att_SeeAttEvents ();
  }

/*****************************************************************************/
/***** Check if the title or the folder of an attendance event exists ********/
/*****************************************************************************/

static bool Att_CheckIfSimilarAttEventExists (const char *Field,const char *Value,long AttCod)
  {
   char Query[512];

   /***** Get number of attendance events with a field value from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM att_events"
	          " WHERE CrsCod='%ld' AND %s='%s' AND AttCod<>'%ld'",
            Gbl.CurrentCrs.Crs.CrsCod,Field,Value,AttCod);
   return (DB_QueryCOUNT (Query,"can not get similar attendance events") != 0);
  }

/*****************************************************************************/
/*************** Put a form to create a new attendance event *****************/
/*****************************************************************************/

void Att_RequestCreatOrEditAttEvent (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_New_event;
   extern const char *Txt_Edit_event;
   extern const char *Txt_Teachers_comment;
   extern const char *Txt_Start_date;
   extern const char *Txt_End_date;
   extern const char *Txt_Title;
   extern const char *Txt_Hidden_MALE_PLURAL;
   extern const char *Txt_Visible_MALE_PLURAL;
   extern const char *Txt_Description;
   extern const char *Txt_Create_event;
   extern const char *Txt_Modify_event;
   struct AttendanceEvent Att;
   bool ItsANewAttEvent;
   Att_StartOrEndTime_t StartOrEndTime;
   const char *NameSelectYear  [Att_NUM_DATES] = {"StartYear"  ,"EndYear"  };
   const char *NameSelectMonth [Att_NUM_DATES] = {"StartMonth" ,"EndMonth" };
   const char *NameSelectDay   [Att_NUM_DATES] = {"StartDay"   ,"EndDay"   };
   const char *NameSelectHour  [Att_NUM_DATES] = {"StartHour"  ,"EndHour"  };
   const char *NameSelectMinute[Att_NUM_DATES] = {"StartMinute","EndMinute"};
   const char *Dates[Att_NUM_DATES] = {Txt_Start_date,Txt_End_date};
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Get parameters *****/
   Att_GetParamAttOrderType ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_ATT_EVENTS);

   /***** Get the code of the attendance event *****/
   ItsANewAttEvent = ((Att.AttCod = Att_GetParamAttCod ()) == -1L);

   /***** Get from the database the data of the attendance event *****/
   if (ItsANewAttEvent)
     {
      /* Initialize to empty attendance event */
      Att.AttCod = -1L;
      Att.DateTimes[Att_START_TIME].Date.Day    = Gbl.Now.Date.Day;
      Att.DateTimes[Att_START_TIME].Date.Month  = Gbl.Now.Date.Month;
      Att.DateTimes[Att_START_TIME].Date.Year   = Gbl.Now.Date.Year;
      Att.DateTimes[Att_START_TIME].Time.Hour   = Gbl.Now.Time.Hour;
      Att.DateTimes[Att_START_TIME].Time.Minute = Gbl.Now.Time.Minute;
      Att.DateTimes[Att_START_TIME].Time.Second = Gbl.Now.Time.Second;

      Att.DateTimes[Att_END_TIME  ].Date.Day    = Gbl.Now.Date.Day;
      Att.DateTimes[Att_END_TIME  ].Date.Month  = Gbl.Now.Date.Month;
      Att.DateTimes[Att_END_TIME  ].Date.Year   = Gbl.Now.Date.Year;
      Att.DateTimes[Att_END_TIME  ].Time.Hour   = 23;
      Att.DateTimes[Att_END_TIME  ].Time.Minute = 59;
      Att.Open = true;
      Att.Title[0] = '\0';
     }
   else
     {
      /* Get data of the attendance event from database */
      Att_GetDataOfAttEventByCodAndCheckCrs (&Att);

      /* Get text of the attendance event from database */
      Att_GetAttEventTxtFromDB (Att.AttCod,Txt);
     }

   /***** Form start *****/
   if (ItsANewAttEvent)
      Act_FormStart (ActNewAtt);
   else
     {
      Act_FormStart (ActChgAtt);
      Att_PutParamAttCod (Att.AttCod);
     }
   Att_PutHiddenParamAttOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);

   /***** Table start *****/
   Lay_StartRoundFrameTable10 (NULL,2,ItsANewAttEvent ? Txt_New_event :
                                                        Txt_Edit_event);

   /***** Attendance event title *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"TIT_TBL\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>"
                      "<td style=\"text-align:left; vertical-align:top;\">"
                      "<input type=\"text\" name=\"Title\" size=\"80\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>"
                      "</tr>",
            Txt_Title,
            Att_MAX_LENGTH_ATTENDANCE_EVENT_TITLE,Att.Title);

   /***** Attendance event start and end dates *****/
   for (StartOrEndTime = Att_START_TIME;
	StartOrEndTime <= Att_END_TIME;
	StartOrEndTime++)
     {
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"TIT_TBL\""
	                 " style=\"text-align:right; vertical-align:top;\">"
	                 "%s:"
	                 "</td>"
                         "<td style=\"text-align:left; vertical-align:top;\">"
                         "<table class=\"CELLS_PAD_2\">"
                         "<tr>"
                         "<td style=\"text-align:left; vertical-align:top;\">",
               Dates[StartOrEndTime]);

      /* Date */
      Dat_WriteFormDate (Gbl.Now.Date.Year-1,Gbl.Now.Date.Year+1,
                         NameSelectDay  [StartOrEndTime],
                         NameSelectMonth[StartOrEndTime],
                         NameSelectYear [StartOrEndTime],
                         &(Att.DateTimes[StartOrEndTime].Date),
                         false,false);

      fprintf (Gbl.F.Out,"</td>"
                         "<td style=\"text-align:left; vertical-align:top;\">");

      /* Time */
      Dat_WriteFormHourMinute (NameSelectHour  [StartOrEndTime],
                               NameSelectMinute[StartOrEndTime],
		               &(Att.DateTimes[StartOrEndTime].Time),
                               false,false);

      fprintf (Gbl.F.Out,"</td>"
	                 "</tr></table></td>"
	                 "</tr>");
     }

   /***** Visibility of comments *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"TIT_TBL\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>"
                      "<td style=\"text-align:left; vertical-align:top;\">"
                      "<select name=\"CommentTchVisible\">",
            Txt_Teachers_comment);

   fprintf (Gbl.F.Out,"<option value=\"N\"");
   if (!Att.CommentTchVisible)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",
	    Txt_Hidden_MALE_PLURAL);

   fprintf (Gbl.F.Out,"<option value=\"Y\"");
   if (Att.CommentTchVisible)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",
	    Txt_Visible_MALE_PLURAL);

   fprintf (Gbl.F.Out,"</select>"
		      "</td>"
		      "</tr>");

   /***** Attendance event description *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"TIT_TBL\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "%s:"
	              "</td>"
                      "<td style=\"text-align:left; vertical-align:top;\">"
                      "<textarea name=\"Txt\" cols=\"60\" rows=\"5\">",
            Txt_Description);
   if (!ItsANewAttEvent)
      fprintf (Gbl.F.Out,"%s",Txt);
   fprintf (Gbl.F.Out,"</textarea>"
                      "</td>"
                      "</tr>");

   /***** Groups *****/
   Att_ShowLstGrpsToEditAttEvent (Att.AttCod);

   /***** Table end *****/
   Lay_EndRoundFrameTable10 ();

   /***** New attendance event *****/
   Lay_PutSendButton (ItsANewAttEvent ? Txt_Create_event :
                                        Txt_Modify_event);

   /***** Form end *****/
   fprintf (Gbl.F.Out,"</form><br />");

   /***** Show current attendance events *****/
   Att_ShowAllAttEvents ();
  }

/*****************************************************************************/
/************* Show list of groups to edit and attendance event **************/
/*****************************************************************************/

static void Att_ShowLstGrpsToEditAttEvent (long AttCod)
  {
   extern const char *Txt_Groups;
   extern const char *Txt_The_whole_course;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   if (Gbl.CurrentCrs.Grps.GrpTypes.Num)
     {
      /***** Start table *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"TIT_TBL\""
	                 " style=\"text-align:right; vertical-align:top;\">"
	                 "%s:"
	                 "</td>"
                         "<td style=\"text-align:left; vertical-align:top;\">",
               Txt_Groups);
      Lay_StartRoundFrameTable10 (NULL,0,NULL);

      /***** First row: checkbox to select the whole course *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td colspan=\"7\" class=\"DAT\""
	                 " style=\"text-align:left; vertical-align:middle;\">"
                         "<input type=\"checkbox\" id=\"WholeCrs\" name=\"WholeCrs\" value=\"Y\"");
      if (!Att_CheckIfAttEventIsAssociatedToGrps (AttCod))
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"uncheckChildren(this,'GrpCods')\" />%s %s</td>"
	                 "</tr>",
               Txt_The_whole_course,Gbl.CurrentCrs.Crs.ShortName);

      /***** List the groups for each group type *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
         if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
            Grp_ListGrpsToEditAsgAttOrSvy (&Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],AttCod,Grp_ATT_EVENT);

      /***** End table *****/
      Lay_EndRoundFrameTable10 ();
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");
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
   struct AttendanceEvent OldAtt,NewAtt;
   char YN[1+1];
   bool ItsANewAttEvent;
   bool NewAttEventIsCorrect = true;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Get the code of the attendance event *****/
   ItsANewAttEvent = ((NewAtt.AttCod = Att_GetParamAttCod ()) == -1L);

   if (!ItsANewAttEvent)
     {
      /* Get data of the old (current) attendance event from database */
      OldAtt.AttCod = NewAtt.AttCod;
      Att_GetDataOfAttEventByCodAndCheckCrs (&OldAtt);
     }

   /***** Get start date *****/
   Dat_GetDateFromForm ("StartDay","StartMonth","StartYear",
                        &(NewAtt.DateTimes[Att_START_TIME].Date.Day),
                        &(NewAtt.DateTimes[Att_START_TIME].Date.Month),
                        &(NewAtt.DateTimes[Att_START_TIME].Date.Year));
   Dat_GetHourMinuteFromForm ("StartHour","StartMinute",
                              &(NewAtt.DateTimes[Att_START_TIME].Time.Hour),
                              &(NewAtt.DateTimes[Att_START_TIME].Time.Minute));
   NewAtt.DateTimes[Att_START_TIME].Time.Second = 0;

   /***** Get end date *****/
   Dat_GetDateFromForm ("EndDay","EndMonth","EndYear",
                        &(NewAtt.DateTimes[Att_END_TIME].Date.Day),
                        &(NewAtt.DateTimes[Att_END_TIME].Date.Month),
                        &(NewAtt.DateTimes[Att_END_TIME].Date.Year));
   Dat_GetHourMinuteFromForm ("EndHour","EndMinute",
                              &(NewAtt.DateTimes[Att_END_TIME].Time.Hour),
                              &(NewAtt.DateTimes[Att_END_TIME].Time.Minute));
   NewAtt.DateTimes[Att_END_TIME].Time.Second = 59;

   /***** Get boolean parameter that indicates if teacher's comments are visible by students *****/
   Par_GetParToText ("CommentTchVisible",YN,1);
   NewAtt.CommentTchVisible = (Str_ConvertToUpperLetter (YN[0]) == 'Y');

   /***** Get attendance event title *****/
   Par_GetParToText ("Title",NewAtt.Title,Att_MAX_LENGTH_ATTENDANCE_EVENT_TITLE);

   /***** Get attendance event text *****/
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewAtt.DateTimes[Att_START_TIME].Date.Day   == 0 ||
       NewAtt.DateTimes[Att_START_TIME].Date.Month == 0 ||
       NewAtt.DateTimes[Att_START_TIME].Date.Year  == 0)
     {
      NewAtt.DateTimes[Att_START_TIME].Date.Day    = Gbl.Now.Date.Day;
      NewAtt.DateTimes[Att_START_TIME].Date.Month  = Gbl.Now.Date.Month;
      NewAtt.DateTimes[Att_START_TIME].Date.Year   = Gbl.Now.Date.Year;
      NewAtt.DateTimes[Att_START_TIME].Time.Hour   = Gbl.Now.Time.Hour;
      NewAtt.DateTimes[Att_START_TIME].Time.Minute = Gbl.Now.Time.Minute;
     }
   if (NewAtt.DateTimes[Att_END_TIME].Date.Day   == 0 ||
       NewAtt.DateTimes[Att_END_TIME].Date.Month == 0 ||
       NewAtt.DateTimes[Att_END_TIME].Date.Year  == 0)
     {
      NewAtt.DateTimes[Att_END_TIME].Date.Day    = Gbl.Now.Date.Day;
      NewAtt.DateTimes[Att_END_TIME].Date.Month  = Gbl.Now.Date.Month;
      NewAtt.DateTimes[Att_END_TIME].Date.Year   = Gbl.Now.Date.Year;
      NewAtt.DateTimes[Att_END_TIME].Time.Hour   = 23;
      NewAtt.DateTimes[Att_END_TIME].Time.Minute = 59;
     }

   /***** Check if title is correct *****/
   if (NewAtt.Title[0])	// If there's an attendance event title
     {
      /* If title of attendance event was in database... */
      if (Att_CheckIfSimilarAttEventExists ("Title",NewAtt.Title,NewAtt.AttCod))
        {
         NewAttEventIsCorrect = false;
         sprintf (Gbl.Message,Txt_Already_existed_an_event_with_the_title_X,
                  NewAtt.Title);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
     }
   else	// If there is not an attendance event title
     {
      NewAttEventIsCorrect = false;
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_title_of_the_event);
     }

   /***** Create a new attendance event or update an existing one *****/
   if (NewAttEventIsCorrect)
     {
      /* Get groups for this attendance events */
      Grp_GetParCodsSeveralGrpsToEditAsgAttOrSvy ();

      if (ItsANewAttEvent)
	{
         Att_CreateAttEvent (&NewAtt,Txt);	// Add new attendance event to database

         /***** Write success message *****/
	 sprintf (Gbl.Message,Txt_Created_new_event_X,NewAtt.Title);
	 Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
	}
      else
	{
         Att_UpdateAttEvent (&NewAtt,Txt);

	 /***** Write success message *****/
	 Lay_ShowAlert (Lay_SUCCESS,Txt_The_event_has_been_modified);
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

void Att_CreateAttEvent (struct AttendanceEvent *Att,const char *Txt)
  {
   char Query[1024+Cns_MAX_BYTES_TEXT];

   /***** Create a new attendance event *****/
   sprintf (Query,"INSERT INTO att_events (CrsCod,UsrCod,StartTime,EndTime,CommentTchVisible,Title,Txt)"
                  " VALUES ('%ld','%ld','%04u%02u%02u%02u%02u%02u','%04u%02u%02u%02u%02u%02u','%c','%s','%s')",
            Gbl.CurrentCrs.Crs.CrsCod,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Att->DateTimes[Att_START_TIME].Date.Year,
            Att->DateTimes[Att_START_TIME].Date.Month,
            Att->DateTimes[Att_START_TIME].Date.Day,
            Att->DateTimes[Att_START_TIME].Time.Hour,
            Att->DateTimes[Att_START_TIME].Time.Minute,
            Att->DateTimes[Att_START_TIME].Time.Second,
            Att->DateTimes[Att_END_TIME  ].Date.Year,
            Att->DateTimes[Att_END_TIME  ].Date.Month,
            Att->DateTimes[Att_END_TIME  ].Date.Day,
            Att->DateTimes[Att_END_TIME  ].Time.Hour,
            Att->DateTimes[Att_END_TIME  ].Time.Minute,
            Att->DateTimes[Att_END_TIME  ].Time.Second,
            Att->CommentTchVisible ? 'Y' :
        	                     'N',
            Att->Title,
            Txt);
   Att->AttCod = DB_QueryINSERTandReturnCode (Query,"can not create new attendance event");

   /***** Create groups *****/
   if (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps)
      Att_CreateGrps (Att->AttCod);
  }

/*****************************************************************************/
/****************** Update an existing attendance event **********************/
/*****************************************************************************/

void Att_UpdateAttEvent (struct AttendanceEvent *Att,const char *Txt)
  {
   char Query[1024+Cns_MAX_BYTES_TEXT];

   /***** Update the data of the attendance event *****/
   sprintf (Query,"UPDATE att_events"
	          " SET StartTime='%04u%02u%02u%02u%02u%02u',EndTime='%04u%02u%02u%02u%02u%02u',"
                  "CommentTchVisible='%c',Title='%s',Txt='%s'"
                  " WHERE AttCod='%ld' AND CrsCod='%ld'",
            Att->DateTimes[Att_START_TIME].Date.Year,
            Att->DateTimes[Att_START_TIME].Date.Month,
            Att->DateTimes[Att_START_TIME].Date.Day,
            Att->DateTimes[Att_START_TIME].Time.Hour,
            Att->DateTimes[Att_START_TIME].Time.Minute,
            Att->DateTimes[Att_START_TIME].Time.Second,
            Att->DateTimes[Att_END_TIME  ].Date.Year,
            Att->DateTimes[Att_END_TIME  ].Date.Month,
            Att->DateTimes[Att_END_TIME  ].Date.Day   ,
            Att->DateTimes[Att_END_TIME  ].Time.Hour,
            Att->DateTimes[Att_END_TIME  ].Time.Minute,
            Att->DateTimes[Att_END_TIME  ].Time.Second,
            Att->CommentTchVisible ? 'Y' :
        	                     'N',
            Att->Title,
            Txt,
            Att->AttCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not update attendance event");

   /***** Update groups *****/
   /* Remove old groups */
   Att_RemoveAllTheGrpsAssociatedToAnAttEvent (Att->AttCod);

   /* Create new groups */
   if (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps)
      Att_CreateGrps (Att->AttCod);
  }

/*****************************************************************************/
/******** Check if an attendance event is associated to any group ************/
/*****************************************************************************/

bool Att_CheckIfAttEventIsAssociatedToGrps (long AttCod)
  {
   char Query[256];

   /***** Get if an attendance event is associated to a group from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM att_grp WHERE AttCod='%ld'",
            AttCod);
   return (DB_QueryCOUNT (Query,"can not check if an attendance event is associated to groups") != 0);
  }

/*****************************************************************************/
/********* Check if an attendance event is associated to a group *************/
/*****************************************************************************/

bool Att_CheckIfAttEventIsAssociatedToGrp (long AttCod,long GrpCod)
  {
   char Query[512];

   /***** Get if an attendance event is associated to a group from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM att_grp WHERE AttCod='%ld' AND GrpCod='%ld'",
            AttCod,GrpCod);
   return (DB_QueryCOUNT (Query,"can not check if an attendance event is associated to a group") != 0);
  }

/*****************************************************************************/
/****************** Remove groups of an attendance event *********************/
/*****************************************************************************/

static void Att_RemoveAllTheGrpsAssociatedToAnAttEvent (long AttCod)
  {
   char Query[256];

   /***** Remove groups of the attendance event *****/
   sprintf (Query,"DELETE FROM att_grp WHERE AttCod='%ld'",AttCod);
   DB_QueryDELETE (Query,"can not remove the groups associated to an attendance event");
  }

/*****************************************************************************/
/*********** Remove one student from all the attendance events ***************/
/*****************************************************************************/

void Att_RemoveUsrFromAllAttEvents (long UsrCod)
  {
   char Query[256];

   /***** Remove group from all the attendance events *****/
   sprintf (Query,"DELETE FROM att_usr WHERE UsrCod='%ld'",UsrCod);
   DB_QueryDELETE (Query,"can not remove user from all attendance events");
  }

/*****************************************************************************/
/*********** Remove one student from all the attendance events ***************/
/*****************************************************************************/

void Att_RemoveUsrFromCrsAttEvents (long UsrCod,long CrsCod)
  {
   char Query[512];

   /***** Remove group from all the attendance events *****/
   sprintf (Query,"DELETE FROM att_usr USING att_events,att_usr"
                  " WHERE att_events.CrsCod='%ld'"
                  " AND att_events.AttCod=att_usr.AttCod AND att_usr.UsrCod='%ld'",
            CrsCod,UsrCod);
   DB_QueryDELETE (Query,"can not remove user from attendance events of a course");
  }

/*****************************************************************************/
/************* Remove one group from all the attendance events ***************/
/*****************************************************************************/

void Att_RemoveGroup (long GrpCod)
  {
   char Query[256];

   /***** Remove group from all the attendance events *****/
   sprintf (Query,"DELETE FROM att_grp WHERE GrpCod='%ld'",GrpCod);
   DB_QueryDELETE (Query,"can not remove group from the associations between attendance events and groups");
  }

/*****************************************************************************/
/******** Remove groups of one type from all the attendance events ***********/
/*****************************************************************************/

void Att_RemoveGroupsOfType (long GrpTypCod)
  {
   char Query[256];

   /***** Remove group from all the attendance events *****/
   sprintf (Query,"DELETE FROM att_grp USING crs_grp,att_grp"
                  " WHERE crs_grp.GrpTypCod='%ld' AND crs_grp.GrpCod=att_grp.GrpCod",
            GrpTypCod);
   DB_QueryDELETE (Query,"can not remove groups of a type from the associations between attendance events and groups");
  }

/*****************************************************************************/
/***************** Create groups of an attendance event **********************/
/*****************************************************************************/

static void Att_CreateGrps (long AttCod)
  {
   unsigned NumGrpSel;
   char Query[256];

   /***** Create groups of the attendance event *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
     {
      /* Create group */
      sprintf (Query,"INSERT INTO att_grp (AttCod,GrpCod) VALUES ('%ld','%ld')",
               AttCod,Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCod[NumGrpSel]);
      DB_QueryINSERT (Query,"can not associate a group to an attendance event");
     }
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
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrp;
   unsigned NumGrps;

   /***** Get groups associated to an attendance event from database *****/
   sprintf (Query,"SELECT crs_grp_types.GrpTypName,crs_grp.GrpName"
	          " FROM att_grp,crs_grp,crs_grp_types"
                  " WHERE att_grp.AttCod='%ld' AND att_grp.GrpCod=crs_grp.GrpCod"
                  " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
                  " ORDER BY crs_grp_types.GrpTypName,crs_grp.GrpName",
            Att->AttCod);
   NumGrps = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get groups of an attendance event");

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<p class=\"%s\">%s: ",
            Att->Hidden ? "ASG_GRP_LIGHT" :
        	          "ASG_GRP",
            (NumGrps == 1) ? Txt_Group  :
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
         fprintf (Gbl.F.Out,"%s %s",row[0],row[1]);

         if (NumGrps >= 2)
           {
            if (NumGrp == NumGrps-2)
               fprintf (Gbl.F.Out," %s ",Txt_and);
            if (NumGrps >= 3)
              if (NumGrp < NumGrps-2)
                  fprintf (Gbl.F.Out,", ");
           }
        }
     }
   else
      fprintf (Gbl.F.Out,"%s %s",
               Txt_The_whole_course,Gbl.CurrentCrs.Crs.ShortName);

   fprintf (Gbl.F.Out,"</p>");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************** Remove all the attendance events of a course ****************/
/*****************************************************************************/

void Att_RemoveCrsAttEvents (long CrsCod)
  {
   char Query[512];

   /***** Remove students *****/
   sprintf (Query,"DELETE FROM att_usr USING att_events,att_usr"
                  " WHERE att_events.CrsCod='%ld' AND att_events.AttCod=att_usr.AttCod",
            CrsCod);
   DB_QueryDELETE (Query,"can not remove all the students registered in events of a course");

   /***** Remove groups *****/
   sprintf (Query,"DELETE FROM att_grp USING att_events,att_grp"
                  " WHERE att_events.CrsCod='%ld' AND att_events.AttCod=att_grp.AttCod",
            CrsCod);
   DB_QueryDELETE (Query,"can not remove all the groups associated to attendance events of a course");

   /***** Remove attendance events *****/
   sprintf (Query,"DELETE FROM att_events WHERE CrsCod='%ld'",
	    CrsCod);
   DB_QueryDELETE (Query,"can not remove all the attendance events of a course");
  }

/*****************************************************************************/
/*************** Get number of attendance events in a course *****************/
/*****************************************************************************/

unsigned Att_GetNumAttEventsInCrs (long CrsCod)
  {
   char Query[256];

   /***** Get number of attendance events in a course from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM att_events WHERE CrsCod='%ld'",
            CrsCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of attendance events in course");
  }

/*****************************************************************************/
/*************** Get number of courses with attendance events ****************/
/*****************************************************************************/
// Returns the number of courses with attendance events
// in this location (all the platform, current degree or current course)

unsigned Att_GetNumCoursesWithAttEvents (Sco_Scope_t Scope)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with attendance events from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_PLATFORM:
         sprintf (Query,"SELECT COUNT(DISTINCT (CrsCod))"
                        " FROM att_events"
                        " WHERE CrsCod>'0'");
         break;
      case Sco_SCOPE_INSTITUTION:
         sprintf (Query,"SELECT COUNT(DISTINCT (att_events.CrsCod))"
                        " FROM centres,degrees,courses,att_events"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=att_events.CrsCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CENTRE:
         sprintf (Query,"SELECT COUNT(DISTINCT (att_events.CrsCod))"
                        " FROM degrees,courses,att_events"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=att_events.CrsCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEGREE:
         sprintf (Query,"SELECT COUNT(DISTINCT (att_events.CrsCod))"
                        " FROM courses,att_events"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=att_events.CrsCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_COURSE:
         sprintf (Query,"SELECT COUNT(DISTINCT (CrsCod))"
                        " FROM att_events"
                        " WHERE CrsCod='%ld'",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of courses with attendance events");

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

unsigned Att_GetNumAttEvents (Sco_Scope_t Scope,unsigned *NumNotif)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAttEvents;

   /***** Get number of attendance events from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_PLATFORM:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM att_events"
                        " WHERE CrsCod>'0'");
         break;
      case Sco_SCOPE_INSTITUTION:
         sprintf (Query,"SELECT COUNT(*),SUM(att_events.NumNotif)"
                        " FROM centres,degrees,courses,att_events"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=att_events.CrsCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CENTRE:
         sprintf (Query,"SELECT COUNT(*),SUM(att_events.NumNotif)"
                        " FROM degrees,courses,att_events"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=att_events.CrsCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEGREE:
         sprintf (Query,"SELECT COUNT(*),SUM(att_events.NumNotif)"
                        " FROM courses,att_events"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=att_events.CrsCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_COURSE:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM att_events"
                        " WHERE CrsCod='%ld'",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of attendance events");

   /***** Get number of attendance events *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumAttEvents) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of attendance events.");

   /***** Get number of notifications by e-mail *****/
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
   struct AttendanceEvent Att;

   /***** Get attendance event code *****/
   if (Gbl.AttEvents.AttCod <= 0)
      if ((Gbl.AttEvents.AttCod = Att_GetParamAttCod ()) == -1L)
         Lay_ShowErrorAndExit ("Code of attendance event is missing.");

   /***** Get parameters *****/
   Att_GetParamAttOrderType ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_ATT_EVENTS);

   /***** Show attendance *****/
   Lay_StartRoundFrameTable10 (NULL,2,NULL);
   Att.AttCod = Gbl.AttEvents.AttCod;
   Att_ShowOneAttEvent (&Att,true);
   Lay_EndRoundFrameTable10 ();

   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_STUDENT:
	 Att_ListAttOnlyMeAsStudent (&Att);
	 break;
      case Rol_ROLE_TEACHER:
      case Rol_ROLE_SUPERUSER:
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
   extern const char *Txt_Student_comment;
   extern const char *Txt_Teachers_comment;
   extern const char *Txt_ROLES_SINGULAR_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Save;

   /***** Get my preference about photos in users' list for current course *****/
   Usr_GetMyPrefAboutListWithPhotosFromDB ();

   /***** Form start *****/
   if (Att->Open)
     {
      Act_FormStart (ActRecAttMe);
      Att_PutParamAttCod (Att->AttCod);
     }

   /***** List students' data *****/
   /* Header */
   Lay_StartRoundFrameTable10 (NULL,2,NULL);
   fprintf (Gbl.F.Out,"<tr>"
		      "<th align=\"left\" class=\"TIT_TBL\"></th>"
		      "<th align=\"left\" class=\"TIT_TBL\"></th>"
		      "<th align=\"left\" class=\"TIT_TBL\"></th>");
   if (Gbl.Usrs.Listing.WithPhotos)
      fprintf (Gbl.F.Out,"<th align=\"left\" class=\"TIT_TBL\" width=\"18\"></th>");
   fprintf (Gbl.F.Out,"<th align=\"left\" colspan=\"2\" class=\"TIT_TBL\">%s</th>"
		      "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
		      "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
		      "</tr>",
	    Txt_ROLES_SINGULAR_Abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN],
	    Txt_Student_comment,
	    Txt_Teachers_comment);

   /* List of students (only me) */
   Att_WriteRowStdToCallTheRoll (1,&Gbl.Usrs.Me.UsrDat,Att);

   /* Footer */
   Lay_EndRoundFrameTable10 ();

   if (Att->Open)
     {
      /***** Send button *****/
      Lay_PutSendButton (Txt_Save);
      fprintf (Gbl.F.Out,"</form>");
     }
  }

/*****************************************************************************/
/*************** List students who attended to one event *********************/
/*****************************************************************************/
// Att must be filled before calling this function

static void Att_ListAttStudents (struct AttendanceEvent *Att)
  {
   extern const char *Txt_Student_comment;
   extern const char *Txt_Teachers_comment;
   extern const char *Txt_ROLES_SINGULAR_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Save;
   unsigned NumStd;
   struct UsrData UsrDat;

   /***** Form to select groups *****/
   Grp_ShowFormToSelectSeveralGroups (ActSeeOneAtt);

   /***** Get and order list of students in this course *****/
   Usr_GetUsrsLst (Rol_ROLE_STUDENT,Sco_SCOPE_COURSE,NULL,false);

   if (Gbl.Usrs.LstStds.NumUsrs)
     {
      /***** Get my preference about photos in users' list for current course *****/
      Usr_GetMyPrefAboutListWithPhotosFromDB ();

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Form start *****/
      Act_FormStart (ActRecAttStd);
      Att_PutParamAttCod (Att->AttCod);
      Grp_PutParamsCodGrps ();

      /***** List students' data *****/
      /* Header */
      Lay_StartRoundFrameTable10 (NULL,2,NULL);
      fprintf (Gbl.F.Out,"<tr>"
                         "<th align=\"left\" class=\"TIT_TBL\"></th>"
                         "<th align=\"left\" class=\"TIT_TBL\"></th>"
                         "<th align=\"left\" class=\"TIT_TBL\"></th>");
      if (Gbl.Usrs.Listing.WithPhotos)
         fprintf (Gbl.F.Out,"<th align=\"left\" class=\"TIT_TBL\" width=\"18\"></th>");
      fprintf (Gbl.F.Out,"<th align=\"left\" colspan=\"2\" class=\"TIT_TBL\">%s</th>"
                         "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
                         "<th align=\"left\" class=\"TIT_TBL\">%s</th>"
                         "</tr>",
               Txt_ROLES_SINGULAR_Abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN],
               Txt_Student_comment,
               Txt_Teachers_comment);

      /* List of students */
      for (NumStd = 0, Gbl.RowEvenOdd = 0;
	   NumStd < Gbl.Usrs.LstStds.NumUsrs;
	   NumStd++)
        {
         UsrDat.UsrCod = Gbl.Usrs.LstStds.Lst[NumStd].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))	// If user's data exist...
           {
            UsrDat.Accepted = Gbl.Usrs.LstStds.Lst[NumStd].Accepted;
            Att_WriteRowStdToCallTheRoll (NumStd + 1,&UsrDat,Att);
           }
        }

      /* Footer */
      Lay_EndRoundFrameTable10 ();

      /***** Send button *****/
      Lay_PutSendButton (Txt_Save);
      fprintf (Gbl.F.Out,"</form>"
	                 "</div>");

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else
      Usr_ShowWarningNoUsersFound (Rol_ROLE_STUDENT);

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstStds);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/************ Write a row of a table with the data of a student **************/
/*****************************************************************************/

static void Att_WriteRowStdToCallTheRoll (unsigned NumStd,struct UsrData *UsrDat,struct AttendanceEvent *Att)
  {
   extern const char *Txt_Present;
   extern const char *Txt_Absent;
   bool Present;
   const char *BgColor;
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   char CommentStd[Cns_MAX_BYTES_TEXT+1];
   char CommentTch[Cns_MAX_BYTES_TEXT+1];

   BgColor = Gbl.ColorRows[Gbl.RowEvenOdd];

   /***** Check if this student is already registered in the current event *****/
   Present = Att_CheckIfUsrIsPresentInAttEventAndGetComments (Att->AttCod,UsrDat->UsrCod,CommentStd,CommentTch);

   /***** Icon to show if the user is already registered *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"BT%d",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">"
	              "<img src=\"%s/%s16x16.gif\""
	              " alt=\"\" title=\"%s\" class=\"ICON16x16\" />"
	              "</td>",
            Gbl.Prefs.IconsURL,
            Present ? "check" :
        	      "check-empty",
            Present ? Txt_Present :
        	      Txt_Absent);

   /***** Checkbox to select user *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:center; vertical-align:top; background-color:%s;\">"
	              "<input type=\"checkbox\" name=\"UsrCodStd\" value=\"%s\"",
	    BgColor,UsrDat->EncryptedUsrCod);
   if (Present)	// This student has attended to the event?
      fprintf (Gbl.F.Out," checked=\"checked\"");
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_STUDENT)	// A student can not change his attendance
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out," />"
	              "</td>");

   /***** Write number of student in the list *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:right;"
	              " vertical-align:top; background-color:%s;\">"
	              "%u"
	              "</td>",
            UsrDat->Accepted ? "DAT_SMALL_N" :
        	               "DAT_SMALL",
            BgColor,
            NumStd);

   /***** Show student's photo *****/
   if (Gbl.Usrs.Listing.WithPhotos)
     {
      fprintf (Gbl.F.Out,"<td style=\"text-align:left; vertical-align:top;"
	                 " background-color:%s;\">",
	       BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,
                        ShowPhoto ? PhotoURL :
                                    NULL,
                        36,48,true);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Write user's ID ******/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
	              " vertical-align:top; background-color:%s;\">",
            UsrDat->Accepted ? "DAT_SMALL_N" :
        	               "DAT_SMALL",
            BgColor);
   ID_WriteUsrIDs (UsrDat,true);
   fprintf (Gbl.F.Out,"</td>");

   /***** Write student's name *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
	              " vertical-align:top; background-color:%s;\">%s",
	    UsrDat->Accepted ? "DAT_SMALL_N" :
		               "DAT_SMALL",
	    BgColor,
	    UsrDat->Surname1);
   if (UsrDat->Surname2[0])
     fprintf (Gbl.F.Out," %s",UsrDat->Surname2);
   fprintf (Gbl.F.Out,", %s</td>",
	    UsrDat->FirstName);

   /***** Student's comment: write form or text */
   fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL\" style=\"text-align:left;"
	              " vertical-align:top; background-color:%s;\">",
	    BgColor);
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_STUDENT && Att->Open)	// Show with form
      fprintf (Gbl.F.Out,"<textarea name=\"CommentStd%ld\" cols=\"40\" rows=\"3\">%s</textarea>",
	       UsrDat->UsrCod,CommentStd);
   else								// Show without form
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        CommentStd,Cns_MAX_BYTES_TEXT,false);
      fprintf (Gbl.F.Out,"%s",CommentStd);
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Teacher's comment: write form, text or nothing */
   fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL\" style=\"text-align:left;"
	              " vertical-align:top; background-color:%s;\">",
	    BgColor);
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER)	// Show with form
      fprintf (Gbl.F.Out,"<textarea name=\"CommentTch%ld\" cols=\"40\" rows=\"3\">%s</textarea>",
	       UsrDat->UsrCod,CommentTch);
   else	if (Att->CommentTchVisible)			// Show without form
     {
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        CommentTch,Cns_MAX_BYTES_TEXT,false);
      fprintf (Gbl.F.Out,"%s",CommentTch);
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/****** Put parameters with the default groups in an attendance event ********/
/*****************************************************************************/

static void Att_PutParamsCodGrps (long AttCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrp;
   unsigned NumGrps;

   /***** Get groups associated to an attendance event from database *****/
   if (Gbl.CurrentCrs.Grps.NumGrps)
     {
      sprintf (Query,"SELECT GrpCod FROM att_grp WHERE att_grp.AttCod='%ld'",
               AttCod);
      NumGrps = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get groups of an attendance event");
     }
   else
      NumGrps = 0;

   /***** Get groups *****/
   if (NumGrps) // Groups found...
     {
      fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"GrpCods\" value=\"");

      /* Get groups */
      for (NumGrp = 0;
	   NumGrp < NumGrps;
	   NumGrp++)
        {
         /* Get next group */
         row = mysql_fetch_row (mysql_res);

         /* Write group code */
         if (NumGrp)
            fprintf (Gbl.F.Out,"%c",Par_SEPARATOR_PARAM_MULTIPLE);
         fprintf (Gbl.F.Out,"%s",row[0]);
        }

      fprintf (Gbl.F.Out,"\" />");
     }
   else
      /***** Write the boolean parameter that indicates if all the groups must be listed *****/
      Par_PutHiddenParamChar ("AllGroups",'Y');

   /***** Free structure that stores the query result *****/
   if (Gbl.CurrentCrs.Grps.NumGrps)
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
   char CommentParamName[10+10+1];
   char CommentStd[Cns_MAX_BYTES_TEXT+1];
   char CommentTch[Cns_MAX_BYTES_TEXT+1];

   /***** Get attendance event code *****/
   if ((Att.AttCod = Att_GetParamAttCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of attendance event is missing.");
   Att_GetDataOfAttEventByCodAndCheckCrs (&Att);	// This checks that event belong to current course

   if (Att.Open)
     {
      /***** Get comments for this student *****/
      Present = Att_CheckIfUsrIsPresentInAttEventAndGetComments (Att.AttCod,Gbl.Usrs.Me.UsrDat.UsrCod,CommentStd,CommentTch);
      sprintf (CommentParamName,"CommentStd%ld",Gbl.Usrs.Me.UsrDat.UsrCod);
      Par_GetParToHTML (CommentParamName,CommentStd,Cns_MAX_BYTES_TEXT);

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
      Lay_ShowAlert (Lay_SUCCESS,Txt_Your_comment_has_been_updated);
     }

   /***** Show the attendance event again *****/
   Gbl.AttEvents.AttCod = Att.AttCod;
   Att_SeeOneAttEvent ();
  }

/*****************************************************************************/
/***************** Save students who attended to an event ********************/
/*****************************************************************************/
/* Algorithm:
   1. Get list of students in the groups selected: Gbl.Usrs.LstStds
   2. Mark all students in the groups selected setting Remove=true
   3. Get list of students marked as present by me: Gbl.Usrs.Select.Std
   4. Loop over the list Gbl.Usrs.Select.Std,
      that holds the list of the students marked as present,
      marking the students in Gbl.Usrs.LstStds.Lst as Remove=false
   5. Delete from att_usr all the students marked as Remove=true
   6. Replace (insert without duplicated) into att_usr all the students marked as Remove=false
 */
void Att_RegisterStudentsInAttEvent (void)
  {
   extern const char *Txt_Presents;
   extern const char *Txt_Absents;
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   struct AttendanceEvent Att;
   char Format[256];
   unsigned NumStd;
   const char *Ptr;
   bool Present;
   unsigned NumStdsPresent;
   unsigned NumStdsAbsent;
   struct UsrData UsrData;
   char CommentParamName[10+10+1];
   char CommentStd[Cns_MAX_BYTES_TEXT+1];
   char CommentTch[Cns_MAX_BYTES_TEXT+1];

   /***** Get attendance event code *****/
   if ((Att.AttCod = Att_GetParamAttCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of attendance event is missing.");
   Att_GetDataOfAttEventByCodAndCheckCrs (&Att);	// This checks that event belong to current course

   /***** Get groups selected *****/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** 1. Get list of students in the groups selected: Gbl.Usrs.LstStds *****/
   /* Get list of students in the groups selected */
   Usr_GetUsrsLst (Rol_ROLE_STUDENT,Sco_SCOPE_COURSE,NULL,false);

   if (Gbl.Usrs.LstStds.NumUsrs)	// If there are students in the groups selected...
     {
      /***** 2. Mark all students in the groups selected setting Remove=true *****/
      for (NumStd = 0;
           NumStd < Gbl.Usrs.LstStds.NumUsrs;
           NumStd++)
         Gbl.Usrs.LstStds.Lst[NumStd].Remove = true;

      /***** 3. Get list of students marked as present by me: Gbl.Usrs.Select.Std *****/
      Usr_GetListSelectedUsrs ();

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrData);

      /***** 4. Loop over the list Gbl.Usrs.Select.Std,
                that holds the list of the students marked as present,
                marking the students in Gbl.Usrs.LstStds.Lst as Remove=false *****/
      Ptr = Gbl.Usrs.Select.Std;
      while (*Ptr)
	{
	 Par_GetNextStrUntilSeparParamMult (&Ptr,UsrData.EncryptedUsrCod,Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
	 Usr_GetUsrCodFromEncryptedUsrCod (&UsrData);
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrData))		// Get student's data from the database
	    /***** Mark student to not be removed *****/
	    for (NumStd = 0;
		 NumStd < Gbl.Usrs.LstStds.NumUsrs;
		 NumStd++)
	       if (Gbl.Usrs.LstStds.Lst[NumStd].UsrCod == UsrData.UsrCod)
		 {
		  Gbl.Usrs.LstStds.Lst[NumStd].Remove = false;
	          break;	// Found! Exit loop
	         }
	}

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrData);

      /***** Free memory *****/
      /* Free memory used for list of students */
      Usr_FreeListsEncryptedUsrCods ();

      // 5. Delete from att_usr all the students marked as Remove=true
      // 6. Replace (insert without duplicated) into att_usr all the students marked as Remove=false
      for (NumStd = 0, NumStdsAbsent = NumStdsPresent = 0;
	   NumStd < Gbl.Usrs.LstStds.NumUsrs;
	   NumStd++)
	{
	 /***** Get comments for this student *****/
	 Att_CheckIfUsrIsPresentInAttEventAndGetComments (Att.AttCod,Gbl.Usrs.LstStds.Lst[NumStd].UsrCod,CommentStd,CommentTch);
	 sprintf (CommentParamName,"CommentTch%ld",Gbl.Usrs.LstStds.Lst[NumStd].UsrCod);
	 Par_GetParToHTML (CommentParamName,CommentTch,Cns_MAX_BYTES_TEXT);

	 Present = !Gbl.Usrs.LstStds.Lst[NumStd].Remove;

	 if (Present ||
	     CommentStd[0] ||
	     CommentTch[0])
	    /***** Register student *****/
	    Att_RegUsrInAttEventChangingComments (Att.AttCod,Gbl.Usrs.LstStds.Lst[NumStd].UsrCod,
					          Present,CommentStd,CommentTch);
	 else
	    /***** Remove student *****/
	    Att_RemoveUsrFromAttEvent (Att.AttCod,Gbl.Usrs.LstStds.Lst[NumStd].UsrCod);

	 if (Present)
            NumStdsPresent++;
      	 else
	    NumStdsAbsent++;
	}

      /***** Free memory for students list *****/
      Usr_FreeUsrsList (&Gbl.Usrs.LstStds);

      /***** Write final message *****/
      sprintf (Format,"%s: %%u<br />%s: %%u",Txt_Presents,Txt_Absents);
      sprintf (Gbl.Message,Format,
	       NumStdsPresent,NumStdsAbsent);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);
     }
   else	// Gbl.Usrs.LstStds.NumUsrs == 0
      Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_ROLE_STUDENT]);

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
   char Query[128];

   /***** Count number of students registered in an event in database *****/
   sprintf (Query,"SELECT COUNT(*) FROM att_usr"
                  " WHERE AttCod='%ld' AND Present='Y'",
            Att->AttCod);
   Att->NumStdsTotal = (unsigned) DB_QueryCOUNT (Query,"can not get number of students who are registered in an event");
  }

/*****************************************************************************/
/******* Get number of students from a list who attended to an event *********/
/*****************************************************************************/

static unsigned Att_GetNumStdsFromAListWhoAreInAttEvent (long AttCod,long LstSelectedUsrCods[],unsigned NumStdsInList)
  {
   char *Query;
   char SubQuery[1+1+10+1];
   unsigned NumStd;
   unsigned NumStdsInAttEvent = 0;

   if (NumStdsInList)
     {
      /***** Allocate space for query *****/
      if ((Query = (char *) malloc ((size_t) (256+NumStdsInList*(1+1+10)))) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory for query.");

      /***** Count number of students registered in an event in database *****/
      sprintf (Query,"SELECT COUNT(*) FROM att_usr"
                     " WHERE AttCod='%ld'"
	             " AND UsrCod IN (",
               AttCod);
      for (NumStd = 0;
	   NumStd < NumStdsInList;
	   NumStd++)
	{
         sprintf (SubQuery,
                  NumStd ? ",%ld" :
                	   "%ld",
                  LstSelectedUsrCods[NumStd]);
	 strcat (Query,SubQuery);
	}
      strcat (Query,") AND Present='Y'");
      // Lay_ShowAlert (Lay_INFO,Query);

      NumStdsInAttEvent = (unsigned) DB_QueryCOUNT (Query,"can not get number of students from a list who are registered in an event");

      /***** Free query *****/
      free ((void *) Query);
     }
   return NumStdsInAttEvent;
  }

/*****************************************************************************/
/***************** Check if a student attended to an event *******************/
/*****************************************************************************/

static bool Att_CheckIfUsrIsInTableAttUsr (long AttCod,long UsrCod,bool *Present)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   bool InDBTable;

   /***** Check if a student is registered in an event in database *****/
   sprintf (Query,"SELECT Present FROM att_usr"
                  " WHERE AttCod='%ld' AND UsrCod='%ld'",
            AttCod,UsrCod);
   if ((NumRows = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get if a student is already registered in an event")))
     {
      InDBTable = true;

      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get if present (row[0]) */
      *Present = (Str_ConvertToUpperLetter (row[0][0]) == 'Y');
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

static bool Att_CheckIfUsrIsPresentInAttEventAndGetComments (long AttCod,long UsrCod,char *CommentStd,char *CommentTch)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   bool Present;

   /***** Check if a students is registered in an event in database *****/
   sprintf (Query,"SELECT Present,CommentStd,CommentTch FROM att_usr"
                  " WHERE AttCod='%ld' AND UsrCod='%ld'",
            AttCod,UsrCod);
   if ((NumRows = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get if a student is already registered in an event")))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get if present (row[0]) */
      Present = (Str_ConvertToUpperLetter (row[0][0]) == 'Y');

      /* Get student's comment (row[1]) */
      strncpy (CommentStd,row[1],Cns_MAX_BYTES_TEXT);
      CommentStd[Cns_MAX_BYTES_TEXT] = '\0';

      /* Get teacher's comment (row[2]) */
      strncpy (CommentTch,row[2],Cns_MAX_BYTES_TEXT);
      CommentTch[Cns_MAX_BYTES_TEXT] = '\0';
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
   char Query[128];

   /***** Check if user is already in table att_usr (present or not) *****/
   if (Att_CheckIfUsrIsInTableAttUsr (AttCod,UsrCod,&Present))	// User is in table att_usr
     {
      // If already present ==> nothing to do
      if (!Present)
	{
	 /***** Set user as present in database *****/
	 sprintf (Query,"UPDATE att_usr SET Present='Y'"
			" WHERE AttCod='%ld' AND UsrCod='%ld'",
		  AttCod,UsrCod);
         DB_QueryUPDATE (Query,"can not set user as present in an event");
	}
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
   char Query[256+Cns_MAX_BYTES_TEXT*2];

   /***** Register user as assistant to an event in database *****/
   sprintf (Query,"REPLACE INTO att_usr (AttCod,UsrCod,Present,CommentStd,CommentTch)"
                  " VALUES ('%ld','%ld','%c','%s','%s')",
            AttCod,UsrCod,
            Present ? 'Y' :
        	      'N',
            CommentStd,
            CommentTch);
   DB_QueryREPLACE (Query,"can not register user in an event");
  }

/*****************************************************************************/
/********************** Remove a user from an event **************************/
/*****************************************************************************/

static void Att_RemoveUsrFromAttEvent (long AttCod,long UsrCod)
  {
   char Query[256];

   /***** Remove user if there is no comment in database *****/
   sprintf (Query,"DELETE FROM att_usr"
	          " WHERE AttCod='%ld' AND UsrCod='%ld'",
            AttCod,UsrCod);
   DB_QueryREPLACE (Query,"can not remove student from an event");
  }

/*****************************************************************************/
/************ Remove users absent without comments from an event *************/
/*****************************************************************************/

void Att_RemoveUsrsAbsentWithoutCommentsFromAttEvent (long AttCod)
  {
   char Query[256];

   /***** Clean table att_usr *****/
   sprintf (Query,"DELETE FROM att_usr"
		  " WHERE AttCod='%ld' AND Present='N'"
		  " AND CommentStd='' AND CommentTch=''",
	    AttCod);
   DB_QueryDELETE (Query,"can not remove users absent without comments from an event");
  }

/*****************************************************************************/
/********* Request listing attendance of students to several events **********/
/*****************************************************************************/

void Usr_ReqListAttendanceStdsCrs (void)
  {
   extern const char *Txt_Show_list;

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Form to select groups *****/
   Grp_ShowFormToSelectSeveralGroups (ActReqLstAttStd);

   /***** Form to select type of list used for select several users *****/
   Usr_ShowFormsToSelectUsrListType (ActReqLstAttStd);

   /***** Get and order lists of users from current course *****/
   Usr_GetUsrsLst (Rol_ROLE_STUDENT,Sco_SCOPE_COURSE,NULL,false);

   if (Gbl.Usrs.LstStds.NumUsrs)
     {
      if (Usr_GetIfShowBigList (Gbl.Usrs.LstStds.NumUsrs))
        {
         /***** Get list of selected users *****/
         Usr_GetListSelectedUsrs ();

         /***** Draw a class photo with students of the course *****/
         /* Form start */
         fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
         Act_FormStart (ActSeeLstAttStd);
         Grp_PutParamsCodGrps ();

         /* Write list of students to select some of them */
         Lay_StartRoundFrameTable10 (NULL,0,NULL);
         Usr_ListUsersToSelect (Rol_ROLE_STUDENT);
         Lay_EndRoundFrameTable10 ();

	 /* Free list of attendance events */
         Att_FreeListAttEvents ();

         /* Send button */
         Lay_PutSendButton (Txt_Show_list);
         fprintf (Gbl.F.Out,"</form>"
	                    "</div>");

         /***** Free memory used for by the list of users *****/
         Usr_FreeListsEncryptedUsrCods ();
        }
     }
   else
      Usr_ShowWarningNoUsersFound (Rol_ROLE_STUDENT);

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstStds);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/************* List attendance of students to several events *****************/
/*****************************************************************************/

void Usr_ListAttendanceStdsCrs (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_You_must_select_one_ore_more_students;
   unsigned NumStdsInList;
   long *LstSelectedUsrCods;
   unsigned NumAttEvent;
   char *StrAttCodsSelected;
   char YN[1+1];
   bool ShowDetails;

   /***** Get list of attendance events *****/
   Att_GetListAttEvents (Att_OLDEST_FIRST);

   /***** Get list of selected students *****/
   Usr_GetListSelectedUsrs ();

   /* Check the number of students to list */
   if ((NumStdsInList = Usr_CountNumUsrsInEncryptedList ()))
     {
      /***** Get boolean parameter that indicates if details must be shown *****/
      Par_GetParToText ("ShowDetails",YN,1);
      ShowDetails = (Str_ConvertToUpperLetter (YN[0]) == 'Y');

      /***** Get list of groups selected ******/
      Grp_GetParCodsSeveralGrpsToShowUsrs ();

      /***** Get list of students selected to show their attendances *****/
      Att_GetListSelectedUsrCods (NumStdsInList,&LstSelectedUsrCods);

      /***** Get number of students in each event *****/
      for (NumAttEvent = 0;
	   NumAttEvent < Gbl.AttEvents.Num;
	   NumAttEvent++)
	 /* Get number of students in this event */
	 Gbl.AttEvents.Lst[NumAttEvent].NumStdsFromList = Att_GetNumStdsFromAListWhoAreInAttEvent (Gbl.AttEvents.Lst[NumAttEvent].AttCod,
	                                                                                           LstSelectedUsrCods,NumStdsInList);

      /***** Get list of attendance events selected *****/
      Att_GetListSelectedAttCods (&StrAttCodsSelected);

      /***** Put link to print *****/
      if (Gbl.CurrentAct == ActSeeLstAttStd)
	 Att_PutFormToPrintListStds (ShowDetails,StrAttCodsSelected);

      /***** List events to select *****/
      Att_ListEventsToSelect ();

      /***** Get my preference about photos in users' list for current course *****/
      Usr_GetMyPrefAboutListWithPhotosFromDB ();

      /***** Show table with attendances for every student in list *****/
      Att_ListStdsAttendanceTable (NumStdsInList,LstSelectedUsrCods);

      /***** Show details or put button to show details *****/
      if (ShowDetails)
         Att_ListStdsWithAttEventsDetails (NumStdsInList,LstSelectedUsrCods);
      else if (Gbl.CurrentAct == ActSeeLstAttStd)
	 Att_PutButtonToShowDetails (StrAttCodsSelected);

      /***** Free memory for list of attendance events selected *****/
      free ((void *) StrAttCodsSelected);

      /***** Free list of user codes *****/
      free ((void *) LstSelectedUsrCods);

      /***** Free list of groups selected *****/
      Grp_FreeListCodSelectedGrps ();
     }
   else	// No students selected
     {
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_select_one_ore_more_students);
      Usr_ReqListAttendanceStdsCrs ();		// ...show again the form
     }

   /***** Free memory used for by the list of users *****/
   Usr_FreeListsEncryptedUsrCods ();

   /***** Free list of attendance events *****/
   Att_FreeListAttEvents ();
  }

/*****************************************************************************/
/********** Get list of students selected to show their attendances **********/
/*****************************************************************************/

static void Att_GetListSelectedUsrCods (unsigned NumStdsInList,long **LstSelectedUsrCods)
  {
   unsigned NumStd;
   const char *Ptr;
   struct UsrData UsrDat;

   /***** Create list of user codes *****/
   if ((*LstSelectedUsrCods = (long *) calloc ((size_t) NumStdsInList,sizeof (long))) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store list of user codes.");

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Loop over the list Gbl.Usrs.Select.All getting users' codes *****/
   for (NumStd = 0, Ptr = Gbl.Usrs.Select.All;
	NumStd < NumStdsInList && *Ptr;
	NumStd++)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EncryptedUsrCod,Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
      (*LstSelectedUsrCods)[NumStd] = UsrDat.UsrCod;
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/****************** Get list of attendance events selected *******************/
/*****************************************************************************/

static void Att_GetListSelectedAttCods (char **StrAttCodsSelected)
  {
   unsigned MaxSizeListAttCodsSelected;
   unsigned NumAttEvent;
   const char *Ptr;
   long AttCod;
   char LongStr[1+10+1];
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrpsInThisEvent;
   unsigned NumGrpInThisEvent;
   long GrpCodInThisEvent;
   unsigned NumGrpSel;

   /***** Allocate memory for list of attendance events selected *****/
   MaxSizeListAttCodsSelected = Gbl.AttEvents.Num * (1+10+1);
   if ((*StrAttCodsSelected = (char *) malloc (MaxSizeListAttCodsSelected+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store list of attendance events selected.");

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
	 Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,1+10);
	 AttCod = Str_ConvertStrCodToLongCod (LongStr);

	 /* Set each event in *StrAttCodsSelected as selected */
	 for (NumAttEvent = 0;
	      NumAttEvent < Gbl.AttEvents.Num;
	      NumAttEvent++)
	    if (Gbl.AttEvents.Lst[NumAttEvent].AttCod == AttCod)
	       Gbl.AttEvents.Lst[NumAttEvent].Selected = true;
	}
     }
   else				// No events selected
     {
      /***** Set which events will be marked as selected by default *****/
      if (!Gbl.CurrentCrs.Grps.NumGrps ||	// Course has no groups
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
	       sprintf (Query,"SELECT GrpCod FROM att_grp" \
			      " WHERE att_grp.AttCod='%ld'",
			Gbl.AttEvents.Lst[NumAttEvent].AttCod);
	       NumGrpsInThisEvent = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get groups of an attendance event");

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
			     NumGrpSel < Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps &&
			     !Gbl.AttEvents.Lst[NumAttEvent].Selected;
			     NumGrpSel++)
			   if (Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCod[NumGrpSel] == GrpCodInThisEvent)
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
/**** Put a link (form) to list assistance of students to several events *****/
/*****************************************************************************/

static void Att_PutFormToPrintListStds (bool ShowDetails,char *StrAttCodsSelected)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Print_view;

   /***** Link to print view *****/
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
   Act_FormStart (ActPrnLstAttStd);
   if (ShowDetails)
      Par_PutHiddenParamChar ("ShowDetails",'Y');
   Grp_PutParamsCodGrps ();
   Usr_PutHiddenParUsrCodAll (ActPrnLstAttStd,Gbl.Usrs.Select.All);
   if (StrAttCodsSelected[0])
      Par_PutHiddenParamString ("AttCods",StrAttCodsSelected);
   Act_LinkFormSubmit (Txt_Print_view,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("print",Txt_Print_view,Txt_Print_view);
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");
  }

/*****************************************************************************/
/**** Put a link (form) to list assistance of students to several events *****/
/*****************************************************************************/

static void Att_PutButtonToShowDetails (char *StrAttCodsSelected)
  {
   extern const char *Txt_Show_more_details;

   /***** Button to show more details *****/
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
   Act_FormStart (ActSeeLstAttStd);
   Par_PutHiddenParamChar ("ShowDetails",'Y');
   Grp_PutParamsCodGrps ();
   Usr_PutHiddenParUsrCodAll (ActSeeLstAttStd,Gbl.Usrs.Select.All);
   if (StrAttCodsSelected[0])
      Par_PutHiddenParamString ("AttCods",StrAttCodsSelected);
   Lay_PutSendButton (Txt_Show_more_details);
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");
  }

/*****************************************************************************/
/********** Write list of those attendance events that have students *********/
/*****************************************************************************/

static void Att_ListEventsToSelect (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Events;
   extern const char *Txt_Event;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Update_attendance_according_to_selected_events;
   extern const char *Txt_Update_attendance;
   const char *BgColor;
   unsigned NumAttEvent;

   /***** Start form to update the attendance
	  depending on the events selected *****/
   if (Gbl.CurrentAct == ActSeeLstAttStd)
     {
      Act_FormStart (ActSeeLstAttStd);
      Grp_PutParamsCodGrps ();
      Usr_PutHiddenParUsrCodAll (ActSeeLstAttStd,Gbl.Usrs.Select.All);
     }

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,2,Txt_Events);

   /***** Heading row *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<th colspan=\"2\" align=\"left\" class=\"TIT_TBL\">%s</th>"
		      "<th align=\"right\" class=\"TIT_TBL\">%s</th>"
		      "</tr>",
	    Txt_Event,
	    Txt_ROLES_PLURAL_Abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN]);

   /***** List the events *****/
   for (NumAttEvent = 0, Gbl.RowEvenOdd = 0;
	NumAttEvent < Gbl.AttEvents.Num;
	NumAttEvent++)
     {
      BgColor = Gbl.ColorRows[Gbl.RowEvenOdd];

      /* Get data of the attendance event from database */
      Att_GetDataOfAttEventByCodAndCheckCrs (&Gbl.AttEvents.Lst[NumAttEvent]);
      Att_GetNumStdsTotalWhoAreInAttEvent (&Gbl.AttEvents.Lst[NumAttEvent]);

      /* Write a row for this event */
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"DAT\""
			 " style=\"text-align:center; background-color:%s;\">"
			 "<input type=\"checkbox\" name=\"AttCods\" value=\"%ld\"",
	       BgColor,
	       Gbl.AttEvents.Lst[NumAttEvent].AttCod);
      if (Gbl.AttEvents.Lst[NumAttEvent].Selected)
	 fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," />"
	                 "</td>"
			 "<td class=\"DAT\""
			 " style=\"text-align:right; background-color:%s;\">"
			 "%u:"
			 "</td>"
			 "<td class=\"DAT\""
			 " style=\"text-align:left; background-color:%s;\">"
			 "%02u/%02u/%04u %02u:%02u h %s"
			 "</td>"
			 "<td class=\"DAT\""
			 " style=\"text-align:right; background-color:%s;\">"
			 "%u"
			 "</td>"
			 "</tr>",
	       BgColor,
	       NumAttEvent + 1,
	       BgColor,
	       Gbl.AttEvents.Lst[NumAttEvent].DateTimes[Att_START_TIME].Date.Day,
	       Gbl.AttEvents.Lst[NumAttEvent].DateTimes[Att_START_TIME].Date.Month,
	       Gbl.AttEvents.Lst[NumAttEvent].DateTimes[Att_START_TIME].Date.Year,
	       Gbl.AttEvents.Lst[NumAttEvent].DateTimes[Att_START_TIME].Time.Hour,
	       Gbl.AttEvents.Lst[NumAttEvent].DateTimes[Att_START_TIME].Time.Minute,
	       Gbl.AttEvents.Lst[NumAttEvent].Title,
	       BgColor,
	       Gbl.AttEvents.Lst[NumAttEvent].NumStdsTotal);

      Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
     }

   /***** Put button to refresh *****/
   if (Gbl.CurrentAct == ActSeeLstAttStd)
     {
      fprintf (Gbl.F.Out,"<tr>"
			 "<td colspan=\"4\" class=\"DAT\""
			 " style=\"text-align:center;\">");
      Act_LinkFormSubmit (Txt_Update_attendance_according_to_selected_events,The_ClassFormul[Gbl.Prefs.Theme]);
      Lay_PutSendIcon ("recycle",Txt_Update_attendance_according_to_selected_events,Txt_Update_attendance);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   /***** End frame *****/
   Lay_EndRoundFrameTable10 ();

   /***** End form *****/
   if (Gbl.CurrentAct == ActSeeLstAttStd)
      fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/*********** Show table with attendances for every student in list ***********/
/*****************************************************************************/

static void Att_ListStdsAttendanceTable (unsigned NumStdsInList,long *LstSelectedUsrCods)
  {
   extern const char *Txt_Attendance;
   extern const char *Txt_Number_of_students;
   struct UsrData UsrDat;
   unsigned NumStd;
   unsigned NumAttEvent;
   unsigned Total;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,2,Txt_Attendance);

   /***** Heading row *****/
   Att_WriteTableHeadSeveralAttEvents ();

   /***** List the students *****/
   for (NumStd = 0, Gbl.RowEvenOdd = 0;
	NumStd < NumStdsInList;
	NumStd++)
     {
      UsrDat.UsrCod = LstSelectedUsrCods[NumStd];
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))		// Get from the database the data of the student
	{
	 UsrDat.Accepted = Usr_GetIfUserHasAcceptedEnrollmentInCurrentCrs (UsrDat.UsrCod);
	 Att_WriteRowStdSeveralAttEvents (NumStd,&UsrDat);
	}
     }

   /***** Last row with the total of students present in each event *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td colspan=\"%u\" class=\"DAT_N\""
		      " style=\"text-align:right;"
		      " border-style:solid none none none;"
		      " border-width:1px;\">"
		      "%s:"
		      "</td>",
	    Gbl.Usrs.Listing.WithPhotos ? 4 :
					  3,
	    Txt_Number_of_students);
   for (NumAttEvent = 0, Total = 0;
	NumAttEvent < Gbl.AttEvents.Num;
	NumAttEvent++)
      if (Gbl.AttEvents.Lst[NumAttEvent].Selected)
	{
	 fprintf (Gbl.F.Out,"<td class=\"DAT_N\""
			    " style=\"text-align:center;"
			    " border-style:solid none none none;"
			    " border-width:1px;\">"
			    "%u"
			    "</td>",
		  Gbl.AttEvents.Lst[NumAttEvent].NumStdsFromList);
	 Total += Gbl.AttEvents.Lst[NumAttEvent].NumStdsFromList;
	}
   fprintf (Gbl.F.Out,"<td class=\"DAT_N\""
		      " style=\"text-align:right;"
		      " border-style:solid none none none;"
		      " border-width:1px;\">"
		      "%u"
		      "</td>"
		      "</tr>",
	    Total);

   /***** End frame *****/
   Lay_EndRoundFrameTable10 ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/* Write table heading for listing of students in several attendance events **/
/*****************************************************************************/

static void Att_WriteTableHeadSeveralAttEvents (void)
  {
   extern const char *Txt_ROLES_SINGULAR_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Attendance;
   unsigned NumAttEvent;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th align=\"left\" colspan=\"%u\" class=\"TIT_TBL\">%s</th>",
            Gbl.Usrs.Listing.WithPhotos ? 4 :
        	                          3,
            Txt_ROLES_SINGULAR_Abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN]);

   for (NumAttEvent = 0;
	NumAttEvent < Gbl.AttEvents.Num;
	NumAttEvent++)
      if (Gbl.AttEvents.Lst[NumAttEvent].Selected)
	{
	 /***** Get data of this attendance event *****/
	 Att_GetDataOfAttEventByCodAndCheckCrs (&Gbl.AttEvents.Lst[NumAttEvent]);

	 fprintf (Gbl.F.Out,"<th align=\"center\" class=\"TIT_TBL\">"
			    "<span class=\"TIT_TBL\" title=\"%s\">%u</span>"
			    "</th>",
		  Gbl.AttEvents.Lst[NumAttEvent].Title,NumAttEvent+1);
	}

   fprintf (Gbl.F.Out,"<th align=\"right\" class=\"TIT_TBL\">%s</th>"
                      "</tr>",
            Txt_Attendance);
  }

/*****************************************************************************/
/************ Write a row of a table with the data of a student **************/
/*****************************************************************************/

static void Att_WriteRowStdSeveralAttEvents (unsigned NumStd,struct UsrData *UsrDat)
  {
   extern const char *Txt_Present;
   extern const char *Txt_Absent;
   const char *BgColor;
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   unsigned NumAttEvent;
   bool Present;
   unsigned NumTimesPresent;

   BgColor = Gbl.ColorRows[Gbl.RowEvenOdd];

   /***** Write number of student in the list *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s\" style=\"text-align:right;"
	              " vertical-align:middle; background-color:%s;\">"
	              "%u"
	              "</td>",
            UsrDat->Accepted ? "DAT_SMALL_N" :
        	               "DAT_SMALL",
            BgColor,
            NumStd + 1);

   /***** Show student's photo *****/
   if (Gbl.Usrs.Listing.WithPhotos)
     {
      fprintf (Gbl.F.Out,"<td style=\"width:18px; text-align:left;"
	                 " vertical-align:middle; background-color:%s;\">",
	       BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,
                        ShowPhoto ? PhotoURL :
                                    NULL,
                        18,24,
			Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Write user's ID ******/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
	              " vertical-align:middle; background-color:%s;\">",
            UsrDat->Accepted ? "DAT_SMALL_N" :
        	               "DAT_SMALL",
            BgColor);
   ID_WriteUsrIDs (UsrDat,true);
   fprintf (Gbl.F.Out,"</td>");

   /***** Write student's name *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
	              " vertical-align:middle; background-color:%s;\">"
	              "%s",
	    UsrDat->Accepted ? "DAT_SMALL_N" :
		               "DAT_SMALL",
	    BgColor,
	    UsrDat->Surname1);
   if (UsrDat->Surname2[0])
     fprintf (Gbl.F.Out," %s",UsrDat->Surname2);
   fprintf (Gbl.F.Out,", %s</td>",
	    UsrDat->FirstName);

   /***** Icon to show if the user is already registered *****/
   for (NumAttEvent = 0, NumTimesPresent = 0;
	NumAttEvent < Gbl.AttEvents.Num;
	NumAttEvent++)
      if (Gbl.AttEvents.Lst[NumAttEvent].Selected)
	{
	 /***** Check if this student is already registered in the current event *****/
	 // Here it is not necessary to get comments
	 Present = Att_CheckIfUsrIsPresentInAttEvent (Gbl.AttEvents.Lst[NumAttEvent].AttCod,UsrDat->UsrCod);

	 fprintf (Gbl.F.Out,"<td class=\"BM%d\">"
	                    "<img src=\"%s/%s16x16.gif\""
	                    " alt=\"\" title=\"%s\" class=\"ICON16x16\" />"
	                    "</td>",
		  Gbl.RowEvenOdd,
		  Gbl.Prefs.IconsURL,
		  Present ? "check" :
			    "check-empty",
		  Present ? Txt_Present :
			    Txt_Absent);

	 if (Present)
	    NumTimesPresent++;
	}

   /***** Last column with the number of times this user is present *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT_N\" style=\"text-align:right;"
	              " vertical-align:middle; background-color:%s;\">"
	              "%u"
	              "</td>"
                      "</tr>",
	    BgColor,
	    NumTimesPresent);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/**************** List the students with details and comments ****************/
/*****************************************************************************/

static void Att_ListStdsWithAttEventsDetails (unsigned NumStdsInList,long *LstSelectedUsrCods)
  {
   extern const char *Txt_Details;
   struct UsrData UsrDat;
   unsigned NumStd;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,2,Txt_Details);

   /***** List students with attendance details *****/
   for (NumStd = 0, Gbl.RowEvenOdd = 0;
	NumStd < NumStdsInList;
	NumStd++)
     {
      UsrDat.UsrCod = LstSelectedUsrCods[NumStd];
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))	// Get from the database the data of the student
	{
	 UsrDat.Accepted = Usr_GetIfUserHasAcceptedEnrollmentInCurrentCrs (UsrDat.UsrCod);
	 Att_ListAttEventsForAStd (NumStd,&UsrDat);
	}
     }

   /***** End frame *****/
   Lay_EndRoundFrameTable10 ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/*************** Write list of attendance events for a student ***************/
/*****************************************************************************/

static void Att_ListAttEventsForAStd (unsigned NumStd,struct UsrData *UsrDat)
  {
   extern const char *Txt_Present;
   extern const char *Txt_Absent;
   extern const char *Txt_Student_comment;
   extern const char *Txt_Teachers_comment;
   const char *BgColor;
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   unsigned NumAttEvent;
   bool Present;
   char CommentStd[Cns_MAX_BYTES_TEXT+1];
   char CommentTch[Cns_MAX_BYTES_TEXT+1];

   BgColor = Gbl.ColorRows[Gbl.RowEvenOdd];

   /***** Write number of student in the list *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s\" style=\"text-align:right;"
	              " vertical-align:middle; background-color:%s;\">"
	              "%u:"
	              "</td>",
	    UsrDat->Accepted ? "DAT_SMALL_N" :
			       "DAT_SMALL",
	    BgColor,
	    NumStd + 1);

   /***** Show student's photo *****/
   fprintf (Gbl.F.Out,"<td style=\"width:18px; text-align:left;"
	              " vertical-align:middle; background-color:%s;\">",
            BgColor);
   ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (UsrDat,
		     ShowPhoto ? PhotoURL :
				 NULL,
		     18,24,
		     Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW);
   fprintf (Gbl.F.Out,"</td>");

   /***** Write user's ID ******/
   fprintf (Gbl.F.Out,"<td style=\"text-align:left; background-color:%s;\">"
                      "<table>"
                      "<tr>"
                      "<td class=\"%s\""
                      " style=\"text-align:left; vertical-align:middle;\">",
            BgColor,
	    UsrDat->Accepted ? "DAT_SMALL_N" :
			       "DAT_SMALL");
   ID_WriteUsrIDs (UsrDat,true);
   fprintf (Gbl.F.Out,"</td>");

   /***** Write student's name *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\""
	              " style=\"text-align:left; vertical-align:middle;\">%s",
	    UsrDat->Accepted ? "DAT_SMALL_N" :
			       "DAT_SMALL",
	    UsrDat->Surname1);
   if (UsrDat->Surname2[0])
     fprintf (Gbl.F.Out," %s",UsrDat->Surname2);
   fprintf (Gbl.F.Out,", %s</td>"
                      "</tr>"
                      "</table>"
                      "</td>"
                      "</tr>",
	    UsrDat->FirstName);

   /***** List the events with students *****/
   for (NumAttEvent = 0;
	NumAttEvent < Gbl.AttEvents.Num;
	NumAttEvent++)
      if (Gbl.AttEvents.Lst[NumAttEvent].Selected)
	{
	 /***** Get data of the attendance event from database *****/
	 Att_GetDataOfAttEventByCodAndCheckCrs (&Gbl.AttEvents.Lst[NumAttEvent]);
         Att_GetNumStdsTotalWhoAreInAttEvent (&Gbl.AttEvents.Lst[NumAttEvent]);

	 /***** Get comments for this student *****/
	 Present = Att_CheckIfUsrIsPresentInAttEventAndGetComments (Gbl.AttEvents.Lst[NumAttEvent].AttCod,UsrDat->UsrCod,CommentStd,CommentTch);

	 /***** Write a row for this event *****/
	 fprintf (Gbl.F.Out,"<tr>"
		            "<td style=\"background-color:%s;\"></td>"
			    "<td class=\"DAT\""
			    " style=\"text-align:right; background-color:%s;\">"
			    "%u:"
			    "</td>"
			    "<td class=\"DAT\""
			    " style=\"text-align:left; background-color:%s;\">"
	                    "<img src=\"%s/%s16x16.gif\""
			    " alt=\"\" title=\"%s\" class=\"ICON16x16\" />"
	                    " %02u/%02u/%04u %02u:%02u h %s</td>"
			    "</tr>",
	          BgColor,
	          BgColor,
		  NumAttEvent + 1,
	          BgColor,
		  Gbl.Prefs.IconsURL,
		  Present ? "check" :
			    "check-empty",
		  Present ? Txt_Present :
			    Txt_Absent,
	          Gbl.AttEvents.Lst[NumAttEvent].DateTimes[Att_START_TIME].Date.Day,
		  Gbl.AttEvents.Lst[NumAttEvent].DateTimes[Att_START_TIME].Date.Month,
		  Gbl.AttEvents.Lst[NumAttEvent].DateTimes[Att_START_TIME].Date.Year,
		  Gbl.AttEvents.Lst[NumAttEvent].DateTimes[Att_START_TIME].Time.Hour,
		  Gbl.AttEvents.Lst[NumAttEvent].DateTimes[Att_START_TIME].Time.Minute,
		  Gbl.AttEvents.Lst[NumAttEvent].Title);

	 /***** Write comments for this student *****/
	 if (CommentStd[0] ||
	     CommentTch[0])
	   {
	    fprintf (Gbl.F.Out,"<tr>"
			       "<td style=\"background-color:%s;\"></td>"
			       "<td style=\"background-color:%s;\"></td>"
			       "<td class=\"DAT\" style=\"text-align:left;"
			       " background-color:%s;\">"
	                       "<dl>",
	             BgColor,
	             BgColor,
	             BgColor);
	    if (CommentStd[0])
	      {
	       Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				 CommentStd,Cns_MAX_BYTES_TEXT,false);
	       fprintf (Gbl.F.Out,"<dt>%s:</dt><dd>%s</dd>",
			Txt_Student_comment,
			CommentStd);
	      }
	    if (CommentTch[0])
	      {
	       Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				 CommentTch,Cns_MAX_BYTES_TEXT,false);
	       fprintf (Gbl.F.Out,"<dt>%s:</dt><dd>%s</dd>",
			Txt_Teachers_comment,
			CommentTch);
	      }
	    fprintf (Gbl.F.Out,"</dl>"
	                       "</td>"
			       "</tr>");
	   }
	}

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }
