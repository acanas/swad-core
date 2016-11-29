// swad_location.c: teacher's location

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_location.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_notification.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_string.h"

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
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Loc_ShowAllLocations (void);
static bool Loc_CheckIfICanCreateLocations (void);
static void Loc_PutIconsListLocations (void);
static void Loc_PutIconToCreateNewLoc (void);
static void Loc_PutButtonToCreateNewLoc (void);
static void Loc_PutParamsToCreateNewLoc (void);
static void Loc_ShowOneLocation (long LocCod);
static void Loc_WriteLocAuthor (struct Location *Loc);
static void Loc_GetParamLocOrderType (void);

static void Loc_PutFormsToRemEditOneLoc (long LocCod,bool Hidden);
static void Loc_PutParams (void);
static void Loc_GetDataOfLocation (struct Location *Loc,const char *Query);
static void Loc_GetLocationTxtFromDB (long LocCod,char *Txt);
static void Loc_PutParamLocCod (long LocCod);
static bool Loc_CheckIfSimilarLocationExists (const char *Field,const char *Value,long LocCod);
static void Loc_UpdateNumUsrsNotifiedByEMailAboutLocation (long LocCod,unsigned NumUsrsToBeNotifiedByEMail);
static void Loc_CreateLocation (struct Location *Loc,const char *Txt);
static void Loc_UpdateLocation (struct Location *Loc,const char *Txt);
static bool Loc_CheckIfIBelongToCrsThisLocation (long LocCod);

/*****************************************************************************/
/************************* List all the locations ****************************/
/*****************************************************************************/

void Loc_SeeLocations (void)
  {
   /***** Get parameters *****/
   Loc_GetParamLocOrderType ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Show all the locations *****/
   Loc_ShowAllLocations ();
  }

/*****************************************************************************/
/************************* Show all the locations ****************************/
/*****************************************************************************/

static void Loc_ShowAllLocations (void)
  {
   extern const char *Hlp_PROFILE_Location;
   extern const char *Txt_Locations;
   extern const char *Txt_ASG_ATT_OR_SVY_HELP_ORDER[2];
   extern const char *Txt_ASG_ATT_OR_SVY_ORDER[2];
   extern const char *Txt_Location;
   extern const char *Txt_Upload_files_QUESTION;
   extern const char *Txt_Folder;
   extern const char *Txt_No_locations;
   Loc_Order_t Order;
   struct Pagination Pagination;
   unsigned NumLoc;

   /***** Get list of locations *****/
   Loc_GetListLocations ();

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Gbl.Usrs.Me.Locs.Num;
   Pagination.CurrentPage = (int) Gbl.Pag.CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Gbl.Pag.CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Write links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (Pag_ASSIGNMENTS,0,&Pagination);

   /***** Start frame *****/
   Lay_StartRoundFrame ("100%",Txt_Locations,
                        Loc_PutIconsListLocations,Hlp_PROFILE_Location);

   if (Gbl.Usrs.Me.Locs.Num)
     {
      /***** Table head *****/
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_MARGIN CELLS_PAD_2\">"
                         "<tr>");
      for (Order = Loc_ORDER_BY_START_DATE;
	   Order <= Loc_ORDER_BY_END_DATE;
	   Order++)
	{
	 fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">");
	 Act_FormStart (ActSeeMyLoc);
	 Grp_PutParamWhichGrps ();
	 Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Act_LinkFormSubmit (Txt_ASG_ATT_OR_SVY_HELP_ORDER[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Usrs.Me.Locs.SelectedOrderType)
	    fprintf (Gbl.F.Out,"<u>");
	 fprintf (Gbl.F.Out,"%s",Txt_ASG_ATT_OR_SVY_ORDER[Order]);
	 if (Order == Gbl.Usrs.Me.Locs.SelectedOrderType)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Act_FormEnd ();
	 fprintf (Gbl.F.Out,"</th>");
	}
      fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">"
			 "%s"
			 "</th>"
			 "<th class=\"CENTER_MIDDLE\">"
			 "%s"
			 "</th>"
			 "<th class=\"CENTER_MIDDLE\">"
			 "%s"
			 "</th>"
			 "</tr>",
	       Txt_Location,
	       Txt_Upload_files_QUESTION,
	       Txt_Folder);

      /***** Write all the locations *****/
      for (NumLoc = Pagination.FirstItemVisible;
	   NumLoc <= Pagination.LastItemVisible;
	   NumLoc++)
	 Loc_ShowOneLocation (Gbl.Usrs.Me.Locs.LstLocCods[NumLoc - 1]);

      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>");
     }
   else	// No locations created
      Lay_ShowAlert (Lay_INFO,Txt_No_locations);

   /***** Button to create a new location *****/
   if (Loc_CheckIfICanCreateLocations ())
      Loc_PutButtonToCreateNewLoc ();

   /***** End frame *****/
   Lay_EndRoundFrame ();

   /***** Write again links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (Pag_ASSIGNMENTS,0,&Pagination);

   /***** Free list of locations *****/
   Loc_FreeListLocations ();
  }

/*****************************************************************************/
/********************* Check if I can create locations ***********************/
/*****************************************************************************/

static bool Loc_CheckIfICanCreateLocations (void)
  {
   return (bool) (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER ||
                  Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);
  }

/*****************************************************************************/
/**************** Put contextual icons in list of locations ******************/
/*****************************************************************************/

static void Loc_PutIconsListLocations (void)
  {
   /***** Put icon to create a new location *****/
   if (Loc_CheckIfICanCreateLocations ())
      Loc_PutIconToCreateNewLoc ();

   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_ASSIGNMENTS;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/******************** Put icon to create a new location **********************/
/*****************************************************************************/

static void Loc_PutIconToCreateNewLoc (void)
  {
   extern const char *Txt_New_location;

   /***** Put form to create a new location *****/
   Lay_PutContextualLink (ActFrmNewLoc,Loc_PutParamsToCreateNewLoc,
                          "plus64x64.png",
                          Txt_New_location,NULL,
                          NULL);
  }

/*****************************************************************************/
/******************* Put button to create a new location *********************/
/*****************************************************************************/

static void Loc_PutButtonToCreateNewLoc (void)
  {
   extern const char *Txt_New_location;

   Act_FormStart (ActFrmNewLoc);
   Loc_PutParamsToCreateNewLoc ();
   Lay_PutConfirmButton (Txt_New_location);
   Act_FormEnd ();
  }

/*****************************************************************************/
/****************** Put parameters to create a new location ******************/
/*****************************************************************************/

static void Loc_PutParamsToCreateNewLoc (void)
  {
   Loc_PutHiddenParamLocOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
  }

/*****************************************************************************/
/**************************** Show one location ******************************/
/*****************************************************************************/

static void Loc_ShowOneLocation (long LocCod)
  {
   extern const char *Txt_Today;
   extern const char *Txt_ASSIGNMENT_TYPES[Loc_NUM_TYPES_SEND_WORK];
   extern const char *Txt_Yes;
   extern const char *Txt_No;
   static unsigned UniqueId = 0;
   struct Location Loc;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Get data of this location *****/
   Loc.LocCod = LocCod;
   Loc_GetDataOfLocationByCod (&Loc);

   /***** Write first row of data of this location *****/
   /* Start date/time */
   UniqueId++;
   fprintf (Gbl.F.Out,"<tr>"
	              "<td id=\"asg_date_start_%u\" class=\"%s LEFT_TOP COLOR%u\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('asg_date_start_%u',%ld,'<br />','%s');"
                      "</script>"
	              "</td>",
	    UniqueId,
            Loc.Hidden ? (Loc.Open ? "DATE_GREEN_LIGHT" :
        	                     "DATE_RED_LIGHT") :
                         (Loc.Open ? "DATE_GREEN" :
                                     "DATE_RED"),
            Gbl.RowEvenOdd,
            UniqueId,Loc.TimeUTC[Loc_START_TIME],Txt_Today);

   /* End date/time */
   UniqueId++;
   fprintf (Gbl.F.Out,"<td id=\"asg_date_end_%u\" class=\"%s LEFT_TOP COLOR%u\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('asg_date_end_%u',%ld,'<br />','%s');"
                      "</script>"
	              "</td>",
	    UniqueId,
            Loc.Hidden ? (Loc.Open ? "DATE_GREEN_LIGHT" :
        	                     "DATE_RED_LIGHT") :
                         (Loc.Open ? "DATE_GREEN" :
                                     "DATE_RED"),
            Gbl.RowEvenOdd,
            UniqueId,Loc.TimeUTC[Loc_END_TIME],Txt_Today);

   /* Location title */
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">"
                      "<div class=\"%s\">%s</div>",
            Gbl.RowEvenOdd,
            Loc.Hidden ? "ASG_TITLE_LIGHT" :
        	         "ASG_TITLE",
            Loc.Title);
   fprintf (Gbl.F.Out,"</td>");

   /* Send work? */
   fprintf (Gbl.F.Out,"<td rowspan=\"2\" class=\"%s CENTER_TOP COLOR%u\">"
                      "<img src=\"%s/%s16x16.gif\""
                      " alt=\"%s\" title=\"%s\" class=\"ICO20x20\" />"
                      "<br />%s"
                      "</td>",
            (Loc.SendWork == Loc_SEND_WORK) ? "DAT_N" :
        	                              "DAT",
            Gbl.RowEvenOdd,
            Gbl.Prefs.IconsURL,
            (Loc.SendWork == Loc_SEND_WORK) ? "file_on" :
        	                              "file_off",
            Txt_ASSIGNMENT_TYPES[Loc.SendWork],
            Txt_ASSIGNMENT_TYPES[Loc.SendWork],
            (Loc.SendWork == Loc_SEND_WORK) ? Txt_Yes :
        	                              Txt_No);

   /***** Write second row of data of this location *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"LEFT_TOP COLOR%u\">",
            Gbl.RowEvenOdd);

   /* Author of the location */
   Loc_WriteLocAuthor (&Loc);

   /* Forms to remove/edit this location */
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_TEACHER:
      case Rol_SYS_ADM:
         Loc_PutFormsToRemEditOneLoc (Loc.LocCod,Loc.Hidden);
         break;
      default:
         break;
     }
   fprintf (Gbl.F.Out,"</td>");

   /* Text of the location */
   Loc_GetLocationTxtFromDB (Loc.LocCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">",
            Gbl.RowEvenOdd);

   fprintf (Gbl.F.Out,"<p class=\"%s\">"
                      "%s"
                      "</p>"
                      "</td>"
                      "</tr>",
            Loc.Hidden ? "DAT_LIGHT" :
        	         "DAT",
            Txt);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

   /***** Mark possible notification as seen *****/
   Ntf_MarkNotifAsSeen (Ntf_EVENT_ASSIGNMENT,
	               LocCod,Gbl.CurrentCrs.Crs.CrsCod,
	               Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************** Write the author of an location **********************/
/*****************************************************************************/

static void Loc_WriteLocAuthor (struct Location *Loc)
  {
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX+1];
   char FirstName[Usr_MAX_BYTES_NAME+1];
   char Surnames[2*(Usr_MAX_BYTES_NAME+1)];
   struct UsrData UsrDat;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Get data of author *****/
   UsrDat.UsrCod = Loc->UsrCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);

   /***** Show photo *****/
   Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                	                 NULL,
	             "PHOTO15x20",Pho_ZOOM,false);

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
            Loc->Hidden ? "MSG_AUT_LIGHT" :
        	          "MSG_AUT",
            FirstName,Surnames);

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/******** Get parameter with the type or order in list of locations **********/
/*****************************************************************************/

static void Loc_GetParamLocOrderType (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("Order",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      Gbl.Usrs.Me.Locs.SelectedOrderType = (Loc_Order_t) UnsignedNum;
   else
      Gbl.Usrs.Me.Locs.SelectedOrderType = Loc_DEFAULT_ORDER_TYPE;
  }

/*****************************************************************************/
/**** Put a hidden parameter with the type of order in list of locations *****/
/*****************************************************************************/

void Loc_PutHiddenParamLocOrderType (void)
  {
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.Usrs.Me.Locs.SelectedOrderType);
  }

/*****************************************************************************/
/****************** Put a link (form) to edit one location *******************/
/*****************************************************************************/

static void Loc_PutFormsToRemEditOneLoc (long LocCod,bool Hidden)
  {
   extern const char *Txt_Remove;
   extern const char *Txt_Show;
   extern const char *Txt_Hide;
   extern const char *Txt_Edit;

   fprintf (Gbl.F.Out,"<div>");

   Gbl.Usrs.Me.Locs.LocCodToEdit = LocCod;	// Used as parameter in contextual links

   /***** Put form to remove location *****/
   Lay_PutContextualLink (ActReqRemLoc,Loc_PutParams,
                          "remove-on64x64.png",
                          Txt_Remove,NULL,
                          NULL);

   /***** Put form to hide/show location *****/
   if (Hidden)
      Lay_PutContextualLink (ActShoLoc,Loc_PutParams,
                             "eye-slash-on64x64.png",
			     Txt_Show,NULL,
                             NULL);
   else
      Lay_PutContextualLink (ActHidLoc,Loc_PutParams,
                             "eye-on64x64.png",
			     Txt_Hide,NULL,
                             NULL);

   /***** Put form to edit location *****/
   Lay_PutContextualLink (ActEdiOneLoc,Loc_PutParams,
                          "edit64x64.png",
                          Txt_Edit,NULL,
                          NULL);

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************* Params used to edit a location ************************/
/*****************************************************************************/

static void Loc_PutParams (void)
  {
   Loc_PutParamLocCod (Gbl.Usrs.Me.Locs.LocCodToEdit);
   Loc_PutHiddenParamLocOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
  }

/*****************************************************************************/
/************************* List all the locations ****************************/
/*****************************************************************************/

void Loc_GetListLocations (void)
  {
   char HiddenSubQuery[256];
   char OrderBySubQuery[256];
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumLoc;

   if (Gbl.Usrs.Me.Locs.LstIsRead)
      Loc_FreeListLocations ();

   /***** Get list of locations from database *****/
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_TEACHER:
      case Rol_SYS_ADM:
         HiddenSubQuery[0] = '\0';
         break;
      default:
         sprintf (HiddenSubQuery,"AND Hidden='N'");
         break;
     }
   switch (Gbl.Usrs.Me.Locs.SelectedOrderType)
     {
      case Loc_ORDER_BY_START_DATE:
         sprintf (OrderBySubQuery,"StartTime DESC,EndTime DESC,Title DESC");
         break;
      case Loc_ORDER_BY_END_DATE:
         sprintf (OrderBySubQuery,"EndTime DESC,StartTime DESC,Title DESC");
         break;
     }
   sprintf (Query,"SELECT LocCod"
		  " FROM locations"
		  " WHERE CrsCod='%ld'%s"
		  " ORDER BY %s",
	    Gbl.CurrentCrs.Crs.CrsCod,HiddenSubQuery,OrderBySubQuery);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get locations");

   if (NumRows) // Locations found...
     {
      Gbl.Usrs.Me.Locs.Num = (unsigned) NumRows;

      /***** Create list of locations *****/
      if ((Gbl.Usrs.Me.Locs.LstLocCods = (long *) calloc (NumRows,sizeof (long))) == NULL)
          Lay_ShowErrorAndExit ("Not enough memory to store list of locations.");

      /***** Get the locations codes *****/
      for (NumLoc = 0;
	   NumLoc < Gbl.Usrs.Me.Locs.Num;
	   NumLoc++)
        {
         /* Get next location code */
         row = mysql_fetch_row (mysql_res);
         if ((Gbl.Usrs.Me.Locs.LstLocCods[NumLoc] = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Error: wrong location code.");
        }
     }
   else
      Gbl.Usrs.Me.Locs.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.Usrs.Me.Locs.LstIsRead = true;
  }

/*****************************************************************************/
/******************** Get location data using its code ***********************/
/*****************************************************************************/

void Loc_GetDataOfLocationByCod (struct Location *Loc)
  {
   char Query[1024];

   /***** Build query *****/
   sprintf (Query,"SELECT LocCod,Hidden,UsrCod,"
                  "UNIX_TIMESTAMP(StartTime),"
                  "UNIX_TIMESTAMP(EndTime),"
                  "NOW() BETWEEN StartTime AND EndTime,"
                  "Title,Folder"
                  " FROM locations"
                  " WHERE LocCod='%ld' AND CrsCod='%ld'",
            Loc->LocCod,Gbl.CurrentCrs.Crs.CrsCod);

   /***** Get data of location *****/
   Loc_GetDataOfLocation (Loc,Query);
  }

/*****************************************************************************/
/*************************** Get location data *******************************/
/*****************************************************************************/

static void Loc_GetDataOfLocation (struct Location *Loc,const char *Query)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Clear all location data *****/
   Loc->LocCod = -1L;
   Loc->Hidden = false;
   Loc->UsrCod = -1L;
   Loc->TimeUTC[Loc_START_TIME] =
   Loc->TimeUTC[Loc_END_TIME  ] = (time_t) 0;
   Loc->Open = false;
   Loc->Title[0] = '\0';
   Loc->SendWork = false;
   Loc->Folder[0] = '\0';
   Loc->IBelongToCrsOrGrps = false;

   /***** Get data of location from database *****/
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get location data");

   if (NumRows) // Location found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get code of the location (row[0]) */
      Loc->LocCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get whether the location is hidden or not (row[1]) */
      Loc->Hidden = (row[1][0] == 'Y');

      /* Get author of the location (row[2]) */
      Loc->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get start date (row[3] holds the start UTC time) */
      Loc->TimeUTC[Loc_START_TIME] = Dat_GetUNIXTimeFromStr (row[3]);

      /* Get end date   (row[4] holds the end   UTC time) */
      Loc->TimeUTC[Loc_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[4]);

      /* Get whether the location is open or closed (row(5)) */
      Loc->Open = (row[5][0] == '1');

      /* Get the title of the location (row[6]) */
      strcpy (Loc->Title,row[6]);

      /* Get the folder for the location files (row[7]) */
      strcpy (Loc->Folder,row[7]);
      Loc->SendWork = (Loc->Folder[0] != '\0');

      /* Can I do this location? */
      Loc->IBelongToCrsOrGrps = Loc_CheckIfIBelongToCrsThisLocation (Loc->LocCod);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************** Free list of locations ***************************/
/*****************************************************************************/

void Loc_FreeListLocations (void)
  {
   if (Gbl.Usrs.Me.Locs.LstIsRead && Gbl.Usrs.Me.Locs.LstLocCods)
     {
      /***** Free memory used by the list of locations *****/
      free ((void *) Gbl.Usrs.Me.Locs.LstLocCods);
      Gbl.Usrs.Me.Locs.LstLocCods = NULL;
      Gbl.Usrs.Me.Locs.Num = 0;
      Gbl.Usrs.Me.Locs.LstIsRead = false;
     }
  }

/*****************************************************************************/
/********************* Get location text from database ***********************/
/*****************************************************************************/

static void Loc_GetLocationTxtFromDB (long LocCod,char *Txt)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get text of location from database *****/
   sprintf (Query,"SELECT Txt FROM locations"
	          " WHERE LocCod='%ld' AND CrsCod='%ld'",
            LocCod,Gbl.CurrentCrs.Crs.CrsCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get location text");

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
      Lay_ShowErrorAndExit ("Error when getting location text.");
  }

/*****************************************************************************/
/****************** Get summary and content of a location  *******************/
/*****************************************************************************/
// This function may be called inside a web service

void Loc_GetNotifLocation (char *SummaryStr,char **ContentStr,
                             long LocCod,unsigned MaxChars,bool GetContent)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Build query *****/
   sprintf (Query,"SELECT Title,Txt FROM locations WHERE LocCod='%ld'",
            LocCod);
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
               strcpy (*ContentStr,row[1]);
              }
           }
         mysql_free_result (mysql_res);
        }
  }

/*****************************************************************************/
/****************** Write parameter with code of location ********************/
/*****************************************************************************/

static void Loc_PutParamLocCod (long LocCod)
  {
   Par_PutHiddenParamLong ("LocCod",LocCod);
  }

/*****************************************************************************/
/******************* Get parameter with code of location *********************/
/*****************************************************************************/

long Loc_GetParamLocCod (void)
  {
   char LongStr[1+10+1];

   /***** Get parameter with code of location *****/
   Par_GetParToText ("LocCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/************* Ask for confirmation of removing of a location ****************/
/*****************************************************************************/

void Loc_AskRemLocation (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_location_X;
   extern const char *Txt_Remove_location;
   struct Location Loc;

   /***** Get parameters *****/
   Loc_GetParamLocOrderType ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Get location code *****/
   if ((Loc.LocCod = Loc_GetParamLocCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of location is missing.");

   /***** Get data of the location from database *****/
   Loc_GetDataOfLocationByCod (&Loc);

   /***** Button of confirmation of removing *****/
   Act_FormStart (ActRemLoc);
   Loc_PutParamLocCod (Loc.LocCod);
   Loc_PutHiddenParamLocOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);

   /***** Ask for confirmation of removing *****/
   sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_location_X,
            Loc.Title);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);
   Lay_PutRemoveButton (Txt_Remove_location);
   Act_FormEnd ();

   /***** Show locations again *****/
   Loc_SeeLocations ();
  }

/*****************************************************************************/
/***************************** Remove a location *****************************/
/*****************************************************************************/

void Loc_RemoveLocation (void)
  {
   extern const char *Txt_Location_X_removed;
   char Query[512];
   struct Location Loc;

   /***** Get location code *****/
   if ((Loc.LocCod = Loc_GetParamLocCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of location is missing.");

   /***** Get data of the location from database *****/
   Loc_GetDataOfLocationByCod (&Loc);	// Inside this function, the course is checked to be the current one

   /***** Remove location *****/
   sprintf (Query,"DELETE FROM locations"
                  " WHERE LocCod='%ld' AND CrsCod='%ld'",
            Loc.LocCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryDELETE (Query,"can not remove location");

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_ASSIGNMENT,Loc.LocCod);

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Location_X_removed,
            Loc.Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show locations again *****/
   Loc_SeeLocations ();
  }

/*****************************************************************************/
/****************************** Hide a location ******************************/
/*****************************************************************************/

void Loc_HideLocation (void)
  {
   extern const char *Txt_Location_X_is_now_hidden;
   char Query[512];
   struct Location Loc;

   /***** Get location code *****/
   if ((Loc.LocCod = Loc_GetParamLocCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of location is missing.");

   /***** Get data of the location from database *****/
   Loc_GetDataOfLocationByCod (&Loc);

   /***** Hide location *****/
   sprintf (Query,"UPDATE locations SET Hidden='Y'"
                  " WHERE LocCod='%ld' AND CrsCod='%ld'",
            Loc.LocCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not hide location");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Location_X_is_now_hidden,
            Loc.Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show locations again *****/
   Loc_SeeLocations ();
  }

/*****************************************************************************/
/****************************** Show a location ******************************/
/*****************************************************************************/

void Loc_ShowLocation (void)
  {
   extern const char *Txt_Location_X_is_now_visible;
   char Query[512];
   struct Location Loc;

   /***** Get location code *****/
   if ((Loc.LocCod = Loc_GetParamLocCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of location is missing.");

   /***** Get data of the location from database *****/
   Loc_GetDataOfLocationByCod (&Loc);

   /***** Hide location *****/
   sprintf (Query,"UPDATE locations SET Hidden='N'"
                  " WHERE LocCod='%ld' AND CrsCod='%ld'",
            Loc.LocCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not show location");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Location_X_is_now_visible,
            Loc.Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show locations again *****/
   Loc_SeeLocations ();
  }

/*****************************************************************************/
/********* Check if the title or the folder of an location exists ************/
/*****************************************************************************/

static bool Loc_CheckIfSimilarLocationExists (const char *Field,const char *Value,long LocCod)
  {
   char Query[512];

   /***** Get number of locations with a field value from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM locations"
	          " WHERE CrsCod='%ld' AND %s='%s' AND LocCod<>'%ld'",
            Gbl.CurrentCrs.Crs.CrsCod,Field,Value,LocCod);
   return (DB_QueryCOUNT (Query,"can not get similar locations") != 0);
  }

/*****************************************************************************/
/******************* Put a form to create a new location *********************/
/*****************************************************************************/

void Loc_RequestCreatOrEditLoc (void)
  {
   extern const char *Hlp_PROFILE_Location_new_location;
   extern const char *Hlp_PROFILE_Location_edit_location;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_New_location;
   extern const char *Txt_Edit_location;
   extern const char *Txt_Title;
   extern const char *Txt_Upload_files_QUESTION;
   extern const char *Txt_Folder;
   extern const char *Txt_Description;
   extern const char *Txt_Create_location;
   extern const char *Txt_Save;
   struct Location Loc;
   bool ItsANewLocation;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Get parameters *****/
   Loc_GetParamLocOrderType ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Get the code of the location *****/
   ItsANewLocation = ((Loc.LocCod = Loc_GetParamLocCod ()) == -1L);

   /***** Get from the database the data of the location *****/
   if (ItsANewLocation)
     {
      /* Initialize to empty location */
      Loc.LocCod = -1L;
      Loc.TimeUTC[Loc_START_TIME] = Gbl.StartExecutionTimeUTC;
      Loc.TimeUTC[Loc_END_TIME  ] = Gbl.StartExecutionTimeUTC + (2 * 60 * 60);	// +2 hours
      Loc.Open = true;
      Loc.Title[0] = '\0';
      Loc.SendWork = false;
      Loc.Folder[0] = '\0';
      Loc.IBelongToCrsOrGrps = false;
     }
   else
     {
      /* Get data of the location from database */
      Loc_GetDataOfLocationByCod (&Loc);

      /* Get text of the location from database */
      Loc_GetLocationTxtFromDB (Loc.LocCod,Txt);
     }

   /***** Start form *****/
   if (ItsANewLocation)
      Act_FormStart (ActFrmNewLoc);
   else
     {
      Act_FormStart (ActChgLoc);
      Loc_PutParamLocCod (Loc.LocCod);
     }
   Loc_PutHiddenParamLocOrderType ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);

   /***** Table start *****/
   Lay_StartRoundFrameTable (NULL,
                             ItsANewLocation ? Txt_New_location :
                                                 Txt_Edit_location,
                             NULL,
                             ItsANewLocation ? Hlp_PROFILE_Location_new_location :
                        	                 Hlp_PROFILE_Location_edit_location,
                             2);

   /***** Location title *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_MIDDLE\">"
	              "%s:"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" name=\"Title\""
                      " size=\"45\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Title,
            Loc_MAX_LENGTH_ASSIGNMENT_TITLE,Loc.Title);

   /***** Location start and end dates *****/
   Dat_PutFormStartEndClientLocalDateTimes (Loc.TimeUTC);

   /***** Send work? *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_MIDDLE\">"
	              "%s:"
	              "</td>"
                      "<td class=\"DAT LEFT_MIDDLE\">"
                      "%s: "
                      "<input type=\"text\" name=\"Folder\""
                      " size=\"%u\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Upload_files_QUESTION,
            Txt_Folder,
            Loc_MAX_LENGTH_FOLDER,Loc_MAX_LENGTH_FOLDER,Loc.Folder);

   /***** Location text *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea name=\"Txt\" cols=\"60\" rows=\"10\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Description);
   if (!ItsANewLocation)
      fprintf (Gbl.F.Out,"%s",Txt);
   fprintf (Gbl.F.Out,"</textarea>"
                      "</td>"
                      "</tr>");

   /***** New location *****/
   if (ItsANewLocation)
      Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_location);
   else
      Lay_EndRoundFrameTableWithButton (Lay_CONFIRM_BUTTON,Txt_Save);
   Act_FormEnd ();

   /***** Show current locations, if any *****/
   Loc_ShowAllLocations ();
  }

/*****************************************************************************/
/******************** Receive form to create a new location ******************/
/*****************************************************************************/

void Loc_RecFormLocation (void)
  {
   extern const char *Txt_Already_existed_an_location_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_location;
   extern const char *Txt_Created_new_location_X;
   extern const char *Txt_The_location_has_been_modified;
   struct Location OldLoc;
   struct Location NewLoc;
   bool ItsANewLocation;
   bool NewLocationIsCorrect = true;
   unsigned NumUsrsToBeNotifiedByEMail;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Get the code of the location *****/
   ItsANewLocation = ((NewLoc.LocCod = Loc_GetParamLocCod ()) == -1L);

   if (!ItsANewLocation)
     {
      /* Get data of the old (current) location from database */
      OldLoc.LocCod = NewLoc.LocCod;
      Loc_GetDataOfLocationByCod (&OldLoc);
     }

   /***** Get start/end date-times *****/
   NewLoc.TimeUTC[Loc_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   NewLoc.TimeUTC[Loc_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /***** Get location title *****/
   Par_GetParToText ("Title",NewLoc.Title,Loc_MAX_LENGTH_ASSIGNMENT_TITLE);

   /***** Get folder name where to send works of the location *****/
   Par_GetParToText ("Folder",NewLoc.Folder,Loc_MAX_LENGTH_FOLDER);
   NewLoc.SendWork = (NewLoc.Folder[0]) ? Loc_SEND_WORK :
	                                  Loc_DO_NOT_SEND_WORK;

   /***** Get location text *****/
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewLoc.TimeUTC[Loc_START_TIME] == 0)
      NewLoc.TimeUTC[Loc_START_TIME] = Gbl.StartExecutionTimeUTC;
   if (NewLoc.TimeUTC[Loc_END_TIME] == 0)
      NewLoc.TimeUTC[Loc_END_TIME] = NewLoc.TimeUTC[Loc_START_TIME] + 2*60*60;	// +2 hours

   /***** Check if title is correct *****/
   if (NewLoc.Title[0])	// If there's an location title
     {
      /* If title of location was in database... */
      if (Loc_CheckIfSimilarLocationExists ("Title",NewLoc.Title,NewLoc.LocCod))
        {
         NewLocationIsCorrect = false;
         sprintf (Gbl.Message,Txt_Already_existed_an_location_with_the_title_X,
                  NewLoc.Title);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
     }
   else	// If there is not an location title
     {
      NewLocationIsCorrect = false;
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_title_of_the_location);
     }

   /***** Create a new location or update an existing one *****/
   if (NewLocationIsCorrect)
     {
      if (ItsANewLocation)
	{
         Loc_CreateLocation (&NewLoc,Txt);	// Add new location to database

	 /***** Write success message *****/
	 sprintf (Gbl.Message,Txt_Created_new_location_X,NewLoc.Title);
	 Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
	}
      else
        {
         if (OldLoc.Folder[0] && NewLoc.Folder[0])
            if (strcmp (OldLoc.Folder,NewLoc.Folder))	// Folder name has changed
               NewLocationIsCorrect = Brw_UpdateFoldersAssigmentsIfExistForAllUsrs (OldLoc.Folder,NewLoc.Folder);
         if (NewLocationIsCorrect)
           {
            Loc_UpdateLocation (&NewLoc,Txt);

	    /***** Write success message *****/
	    Lay_ShowAlert (Lay_SUCCESS,Txt_The_location_has_been_modified);
           }
        }

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();

      /***** Notify by email about the new location *****/
      if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_ASSIGNMENT,NewLoc.LocCod)))
	 Loc_UpdateNumUsrsNotifiedByEMailAboutLocation (NewLoc.LocCod,NumUsrsToBeNotifiedByEMail);
      Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);

      /***** Show locations again *****/
      Loc_SeeLocations ();
     }
   else
      // TODO: The form should be filled with partial data, now is always empty
      Loc_RequestCreatOrEditLoc ();
  }

/*****************************************************************************/
/********* Update number of users notified in table of locations *************/
/*****************************************************************************/

static void Loc_UpdateNumUsrsNotifiedByEMailAboutLocation (long LocCod,unsigned NumUsrsToBeNotifiedByEMail)
  {
   char Query[512];

   /***** Update number of users notified *****/
   sprintf (Query,"UPDATE locations SET NumNotif=NumNotif+'%u'"
                  " WHERE LocCod='%ld'",
            NumUsrsToBeNotifiedByEMail,LocCod);
   DB_QueryUPDATE (Query,"can not update the number of notifications of an location");
  }

/*****************************************************************************/
/************************* Create a new location *****************************/
/*****************************************************************************/

static void Loc_CreateLocation (struct Location *Loc,const char *Txt)
  {
   char Query[1024+Cns_MAX_BYTES_TEXT];

   /***** Create a new location *****/
   sprintf (Query,"INSERT INTO locations"
	          " (CrsCod,UsrCod,StartTime,EndTime,Title,Folder,Txt)"
                  " VALUES"
                  " ('%ld','%ld',FROM_UNIXTIME('%ld'),FROM_UNIXTIME('%ld'),"
                  "'%s','%s','%s')",
            Gbl.CurrentCrs.Crs.CrsCod,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Loc->TimeUTC[Loc_START_TIME],
            Loc->TimeUTC[Loc_END_TIME  ],
            Loc->Title,
            Loc->Folder,
            Txt);
   Loc->LocCod = DB_QueryINSERTandReturnCode (Query,"can not create new location");
  }

/*****************************************************************************/
/********************** Update an existing location **************************/
/*****************************************************************************/

static void Loc_UpdateLocation (struct Location *Loc,const char *Txt)
  {
   char Query[1024+Cns_MAX_BYTES_TEXT];

   /***** Update the data of the location *****/
   sprintf (Query,"UPDATE locations SET "
	          "StartTime=FROM_UNIXTIME('%ld'),"
	          "EndTime=FROM_UNIXTIME('%ld'),"
                  "Title='%s',Folder='%s',Txt='%s'"
                  " WHERE LocCod='%ld' AND CrsCod='%ld'",
            Loc->TimeUTC[Loc_START_TIME],
            Loc->TimeUTC[Loc_END_TIME  ],
            Loc->Title,
            Loc->Folder,
            Txt,
            Loc->LocCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not update location");
  }

/*****************************************************************************/
/******************* Remove all the locations of a course ********************/
/*****************************************************************************/

void Loc_RemoveCrsLocations (long CrsCod)
  {
   char Query[128];

   /***** Remove locations *****/
   sprintf (Query,"DELETE FROM locations WHERE CrsCod='%ld'",CrsCod);
   DB_QueryDELETE (Query,"can not remove all the locations of a course");
  }

/*****************************************************************************/
/********** Check if I belong to any of the groups of an location ************/
/*****************************************************************************/

static bool Loc_CheckIfIBelongToCrsThisLocation (long LocCod)
  {
   char Query[512];

   if (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT ||
       Gbl.Usrs.Me.LoggedRole == Rol_TEACHER)
     {
      // Students and teachers can edit locations depending on groups
      /***** Get if I can edit an location from database *****/
      sprintf (Query,"SELECT COUNT(*) FROM locations"
		     " WHERE LocCod='%ld'",
	       LocCod);
      return (DB_QueryCOUNT (Query,"can not check if I can do an location") != 0);
     }
   else
      return (Gbl.Usrs.Me.LoggedRole > Rol_TEACHER);	// Admins can edit locations
  }

/*****************************************************************************/
/******************* Get number of locations in a course *********************/
/*****************************************************************************/

unsigned Loc_GetNumLocationsInCrs (long CrsCod)
  {
   char Query[256];

   /***** Get number of locations in a course from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM locations WHERE CrsCod='%ld'",
            CrsCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of locations in course");
  }

/*****************************************************************************/
/******************* Get number of courses with locations ********************/
/*****************************************************************************/
// Returns the number of courses with locations
// in this location (all the platform, current degree or current course)

unsigned Loc_GetNumCoursesWithLocations (Sco_Scope_t Scope)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with locations from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(DISTINCT CrsCod)"
                        " FROM locations"
                        " WHERE CrsCod>'0'");
         break;
       case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(DISTINCT locations.CrsCod)"
                        " FROM institutions,centres,degrees,courses,locations"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=locations.CrsCod",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
       case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(DISTINCT locations.CrsCod)"
                        " FROM centres,degrees,courses,locations"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=locations.CrsCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(DISTINCT locations.CrsCod)"
                        " FROM degrees,courses,locations"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=locations.CrsCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(DISTINCT locations.CrsCod)"
                        " FROM courses,locations"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=locations.CrsCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(DISTINCT CrsCod)"
                        " FROM locations"
                        " WHERE CrsCod='%ld'",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of courses with locations");

   /***** Get number of courses *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumCourses) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of courses with locations.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCourses;
  }

/*****************************************************************************/
/************************* Get number of locations ***************************/
/*****************************************************************************/
// Returns the number of locations
// in this location (all the platform, current degree or current course)

unsigned Loc_GetNumLocations (Sco_Scope_t Scope,unsigned *NumNotif)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumLocations;

   /***** Get number of locations from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM locations"
                        " WHERE CrsCod>'0'");
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(*),SUM(locations.NumNotif)"
                        " FROM institutions,centres,degrees,courses,locations"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=locations.CrsCod",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(*),SUM(locations.NumNotif)"
                        " FROM centres,degrees,courses,locations"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=locations.CrsCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(*),SUM(locations.NumNotif)"
                        " FROM degrees,courses,locations"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=locations.CrsCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(*),SUM(locations.NumNotif)"
                        " FROM courses,locations"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=locations.CrsCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM locations"
                        " WHERE CrsCod='%ld'",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of locations");

   /***** Get number of locations *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumLocations) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of locations.");

   /***** Get number of notifications by email *****/
   if (row[1])
     {
      if (sscanf (row[1],"%u",NumNotif) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of notifications of locations.");
     }
   else
      *NumNotif = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumLocations;
  }
