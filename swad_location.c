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

#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_global.h"
#include "swad_location.h"
#include "swad_parameter.h"
#include "swad_text.h"

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

static void Loc_GetParamLocOrderType (void);
static void Loc_PutIconToEditLocs (void);

static void Loc_GetDataOfLocationByCod (struct Location *Loc);

static void Loc_ListLocationsForEdition (void);
static void Loc_PutParamLocCod (long LocCod);
static void Loc_ChangeDate (Loc_StartOrEndDate_t StartOrEndDate);
static void Loc_PutFormToCreateLocation (void);
static void Loc_PutHeadLocations (void);
static void Loc_CreateLocation (struct Location *Loc);

/*****************************************************************************/
/*************************** List all my locations ***************************/
/*****************************************************************************/

void Loc_SeeLocations (void)
  {
   extern const char *Hlp_PROFILE_Location;
   extern const char *Txt_Locations;
   extern const char *Txt_LOCATIONS_HELP_ORDER[2];
   extern const char *Txt_LOCATIONS_ORDER[2];
   extern const char *Txt_Location;
   Loc_OrderType_t Order;
   unsigned NumLoc;

   /***** Get parameter with the type of order in the list of locations *****/
   Loc_GetParamLocOrderType ();

   /***** Get list of locations *****/
   Loc_GetListLocations ();

   /***** Table head *****/
   Lay_StartRoundFrameTable (NULL,Txt_Locations,
			     Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM ? Loc_PutIconToEditLocs :
								     NULL,
			     Hlp_PROFILE_Location,2);
   fprintf (Gbl.F.Out,"<tr>");
   for (Order = Loc_ORDER_BY_START_DATE;
	Order <= Loc_ORDER_BY_END_DATE;
	Order++)
     {
      fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">");
      Act_FormStart (ActSeeMyLoc);
      Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
      Act_LinkFormSubmit (Txt_LOCATIONS_HELP_ORDER[Order],"TIT_TBL",NULL);
      if (Order == Gbl.Usrs.Me.Locs.SelectedOrderType)
	 fprintf (Gbl.F.Out,"<u>");
      fprintf (Gbl.F.Out,"%s",Txt_LOCATIONS_ORDER[Order]);
      if (Order == Gbl.Usrs.Me.Locs.SelectedOrderType)
	 fprintf (Gbl.F.Out,"</u>");
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</th>");
     }
   fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">"
		      "%s"
		      "</th>"
		      "</tr>",
	    Txt_Location);

   /***** Write all the locations *****/
   for (NumLoc = 0;
	NumLoc < Gbl.Usrs.Me.Locs.Num;
	NumLoc++)
     {
      /* Write data of this location */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"DAT LEFT_MIDDLE\">"
			 "&nbsp;%04u-%02u-%02u"
			 "</td>",
	       Gbl.Usrs.Me.Locs.Lst[NumLoc].StartDate.Year,
	       Gbl.Usrs.Me.Locs.Lst[NumLoc].StartDate.Month,
	       Gbl.Usrs.Me.Locs.Lst[NumLoc].StartDate.Day);
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">"
			 "&nbsp;%04u-%02u-%02u"
			 "</td>",
	       Gbl.Usrs.Me.Locs.Lst[NumLoc].EndDate.Year,
	       Gbl.Usrs.Me.Locs.Lst[NumLoc].EndDate.Month,
	       Gbl.Usrs.Me.Locs.Lst[NumLoc].EndDate.Day);
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">"
			 "&nbsp;%s"
			 "</td>"
			 "</tr>",
	       Gbl.Usrs.Me.Locs.Lst[NumLoc].Location);
     }

   /***** End table *****/
   Lay_EndRoundFrameTable ();

   /***** Free list of locations *****/
   Loc_FreeListLocations ();
  }

/*****************************************************************************/
/********* Get parameter with the type or order in list of locations *********/
/*****************************************************************************/

static void Loc_GetParamLocOrderType (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("Order",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      Gbl.Usrs.Me.Locs.SelectedOrderType = (Loc_OrderType_t) UnsignedNum;
   else
      Gbl.Usrs.Me.Locs.SelectedOrderType = Loc_DEFAULT_ORDER_TYPE;
  }

/*****************************************************************************/
/************************* Put icon to edit locations ************************/
/*****************************************************************************/

static void Loc_PutIconToEditLocs (void)
  {
   extern const char *Txt_Edit;

   Lay_PutContextualLink (ActEdiLoc,NULL,
                          "edit64x64.png",
                          Txt_Edit,NULL,
                          NULL);
  }

/*****************************************************************************/
/************************ Put forms to edit degree types *********************/
/*****************************************************************************/

void Loc_EditLocations (void)
  {
   extern const char *Txt_There_are_no_locations;

   /***** Get list of places *****/
   Plc_GetListPlaces ();

   /***** Get list of locations *****/
   Loc_GetListLocations ();

   if (!Gbl.Usrs.Me.Locs.Num)
      /***** Help message *****/
      Lay_ShowAlert (Lay_INFO,Txt_There_are_no_locations);

   /***** Put a form to create a new location *****/
   Loc_PutFormToCreateLocation ();

   /***** Forms to edit current locations *****/
   if (Gbl.Usrs.Me.Locs.Num)
      Loc_ListLocationsForEdition ();

   /***** Free list of locations *****/
   Loc_FreeListLocations ();

   /***** Free list of places *****/
   Plc_FreeListPlaces ();
  }

/*****************************************************************************/
/*************************** List all the locations **************************/
/*****************************************************************************/

void Loc_GetListLocations (void)
  {
   char OrderBySubQuery[256];
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumLoc;
   struct Location *Loc;

   if (Gbl.DB.DatabaseIsOpen)
     {
      if (Gbl.Usrs.Me.Locs.LstIsRead)
	 Loc_FreeListLocations ();

      /***** Get locations from database *****/
      switch (Gbl.Usrs.Me.Locs.SelectedOrderType)
	{
	 case Loc_ORDER_BY_START_DATE:
	    sprintf (OrderBySubQuery,"StartDate,EndDate,Location");
	    break;
	 case Loc_ORDER_BY_END_DATE:
	    sprintf (OrderBySubQuery,"EndDate,StartDate,Location");
	    break;
	}
      sprintf (Query,"SELECT LocCod,"
	             "DATE_FORMAT(StartDate,'%%Y%%m%%d') AS StartDate,"
	             "DATE_FORMAT(EndDate,'%%Y%%m%%d') AS EndDate,"
	             "Location"
		     " FROM locations"
		     " ORDER BY %s",
	       OrderBySubQuery);
      if ((Gbl.Usrs.Me.Locs.Num = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get locations"))) // Locations found...
	{
	 /***** Create list of locations *****/
	 if ((Gbl.Usrs.Me.Locs.Lst = (struct Location *) calloc ((size_t) Gbl.Usrs.Me.Locs.Num,sizeof (struct Location))) == NULL)
	     Lay_ShowErrorAndExit ("Not enough memory to store locations.");

	 /***** Get the locations *****/
	 for (NumLoc = 0;
	      NumLoc < Gbl.Usrs.Me.Locs.Num;
	      NumLoc++)
	   {
	    Loc = &(Gbl.Usrs.Me.Locs.Lst[NumLoc]);

	    /* Get next location */
	    row = mysql_fetch_row (mysql_res);

	    /* Get location code (row[0]) */
	    if ((Loc->LocCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	       Lay_ShowErrorAndExit ("Wrong location code.");

	    /* Get start date (row[1] holds the start date in YYYYMMDD format) */
	    if (!(Dat_GetDateFromYYYYMMDD (&(Loc->StartDate),row[1])))
	       Lay_ShowErrorAndExit ("Wrong start date.");

	    /* Get end date (row[2] holds the end date in YYYYMMDD format) */
	    if (!(Dat_GetDateFromYYYYMMDD (&(Loc->EndDate),row[2])))
	       Lay_ShowErrorAndExit ("Wrong end date.");

	    /* Get the location (row[3]) */
	    strcpy (Loc->Location,row[3]);
	   }
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      Gbl.Usrs.Me.Locs.LstIsRead = true;
     }
  }

/*****************************************************************************/
/************************* Get location data by code *************************/
/*****************************************************************************/

static void Loc_GetDataOfLocationByCod (struct Location *Loc)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Clear data *****/
   Loc->StartDate.Day   =
   Loc->StartDate.Month =
   Loc->StartDate.Year  = 0;
   Loc->Location[0] = '\0';

   /***** Check if location code is correct *****/
   if (Loc->LocCod <= 0)
      Lay_ShowErrorAndExit ("Wrong code of location.");

   /***** Get data of location from database *****/
   sprintf (Query,"SELECT "
	          "DATE_FORMAT(StartDate,'%%Y%%m%%d'),"
                  "DATE_FORMAT(EndDate,'%%Y%%m%%d'),"
                  "Location"
                  " FROM locations"
                  " WHERE LocCod='%ld'",
            Loc->LocCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get data of a location")) // Location found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get start date (row[0] holds the start date in YYYYMMDD format) */
      if (!(Dat_GetDateFromYYYYMMDD (&(Loc->StartDate),row[0])))
	 Lay_ShowErrorAndExit ("Wrong start date.");

      /* Get end date (row[1] holds the end date in YYYYMMDD format) */
      if (!(Dat_GetDateFromYYYYMMDD (&(Loc->EndDate),row[1])))
	 Lay_ShowErrorAndExit ("Wrong end date.");

      /* Get the location (row[2]) */
      strcpy (Loc->Location,row[2]);
     }

  /***** Free structure that stores the query result *****/
  DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************************** Free list of locations *************************/
/*****************************************************************************/

void Loc_FreeListLocations (void)
  {
   if (Gbl.Usrs.Me.Locs.LstIsRead && Gbl.Usrs.Me.Locs.Lst)
     {
      /***** Free memory used by the list of courses in degree *****/
      free ((void *) Gbl.Usrs.Me.Locs.Lst);
      Gbl.Usrs.Me.Locs.Lst = NULL;
      Gbl.Usrs.Me.Locs.Num = 0;
      Gbl.Usrs.Me.Locs.LstIsRead = false;
     }
  }

/*****************************************************************************/
/********************* List all the locations for edition ********************/
/*****************************************************************************/

static void Loc_ListLocationsForEdition (void)
  {
   extern const char *Hlp_PROFILE_Location_edit;
   extern const char *Txt_Locations;
   unsigned NumLoc;
   struct Location *Loc;

   Lay_StartRoundFrameTable (NULL,Txt_Locations,
                             NULL,Hlp_PROFILE_Location_edit,2);

   /***** Table head *****/
   Loc_PutHeadLocations ();

   /***** Write all the locations *****/
   for (NumLoc = 0;
	NumLoc < Gbl.Usrs.Me.Locs.Num;
	NumLoc++)
     {
      Loc = &Gbl.Usrs.Me.Locs.Lst[NumLoc];

      /* Put icon to remove location */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"BM\">");
      Act_FormStart (ActRemLoc);
      Loc_PutParamLocCod (Loc->LocCod);
      Lay_PutIconRemove ();
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Location start date */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActChgLocStrDat);
      Loc_PutParamLocCod (Loc->LocCod);
      Dat_WriteFormDate (Gbl.Now.Date.Year - 1,
	                 Gbl.Now.Date.Year + 1,
	                 "Start",
                         &(Gbl.Usrs.Me.Locs.Lst[NumLoc].StartDate),
                         true,false);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Location end date */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActChgLocEndDat);
      Loc_PutParamLocCod (Loc->LocCod);
      Dat_WriteFormDate (Gbl.Now.Date.Year - 1,
	                 Gbl.Now.Date.Year + 1,
	                 "End",
                         &(Gbl.Usrs.Me.Locs.Lst[NumLoc].EndDate),
                         true,false);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Location */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActRenLoc);
      Loc_PutParamLocCod (Loc->LocCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Location\""
	                 " size=\"20\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Loc_MAX_LENGTH_LOCATION,Loc->Location,Gbl.Form.Id);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>"
                         "</tr>");
     }

   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/******************** Write parameter with code of location ******************/
/*****************************************************************************/

static void Loc_PutParamLocCod (long LocCod)
  {
   Par_PutHiddenParamLong ("LocCod",LocCod);
  }

/*****************************************************************************/
/********************* Get parameter with code of location *******************/
/*****************************************************************************/

long Loc_GetParamLocCod (void)
  {
   char LongStr[1+10+1];

   /***** Get parameter with code of location *****/
   Par_GetParToText ("LocCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/******************************* Remove a location ***************************/
/*****************************************************************************/

void Loc_RemoveLocation (void)
  {
   extern const char *Txt_Location_removed;
   char Query[128];
   struct Location Loc;

   /***** Get location code *****/
   if ((Loc.LocCod = Loc_GetParamLocCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of location is missing.");

   /***** Get data of the location from database *****/
   Loc_GetDataOfLocationByCod (&Loc);

   /***** Remove location *****/
   sprintf (Query,"DELETE FROM locations WHERE LocCod='%ld'",Loc.LocCod);
   DB_QueryDELETE (Query,"can not remove a location");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Location_removed,Loc.Location);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the form again *****/
   Loc_EditLocations ();
  }

/*****************************************************************************/
/******************* Change the start date of a location *********************/
/*****************************************************************************/

void Loc_ChangeStartDate (void)
  {
   Loc_ChangeDate (LOC_START_DATE);
  }

/*****************************************************************************/
/******************** Change the end date of a location **********************/
/*****************************************************************************/

void Loc_ChangeEndDate (void)
  {
   Loc_ChangeDate (LOC_END_DATE);
  }

/*****************************************************************************/
/**************** Change the start/end date of a location ********************/
/*****************************************************************************/

static void Loc_ChangeDate (Loc_StartOrEndDate_t StartOrEndDate)
  {
   extern const char *Txt_The_date_of_the_location_has_changed_to_X;
   char Query[512];
   struct Location *Loc;
   struct Date NewDate;
   struct Date *PtrDate = NULL;	// Initialized to avoid warning
   const char *StrStartOrEndDate;
   char StrDate[11];

   Loc = &Gbl.Usrs.Me.Locs.EditingLoc;

   /***** Get the code of the location *****/
   if ((Loc->LocCod = Loc_GetParamLocCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of location is missing.");

   /***** Get from the database the data of the location *****/
   Loc_GetDataOfLocationByCod (Loc);

   /***** Get the new date for the location *****/
   switch (StartOrEndDate)
     {
      case LOC_START_DATE:
         StrStartOrEndDate = "StartDate";
         PtrDate = &(Loc->StartDate);
         Dat_GetDateFromForm ("StartDay","StartMonth","StartYear",
                              &(NewDate.Day),&(NewDate.Month),&(NewDate.Year));
         if (NewDate.Day   == 0 ||
             NewDate.Month == 0 ||
             NewDate.Year  == 0)
            Dat_AssignDate (&NewDate,&Gbl.Now.Date);
         break;
      case LOC_END_DATE:
         StrStartOrEndDate = "EndDate";
         PtrDate = &(Loc->EndDate);
	 Dat_GetDateFromForm ("EndDay","EndMonth","EndYear",
			      &(NewDate.Day),&(NewDate.Month),&(NewDate.Year));
	 if (NewDate.Day   == 0 ||
	     NewDate.Month == 0 ||
	     NewDate.Year  == 0)
	    Dat_AssignDate (&NewDate,&Gbl.Now.Date);
         break;
     }

   /***** Update the date in database *****/
   sprintf (Query,"UPDATE locations SET %s='%04u%02u%02u' WHERE LocCod='%ld'",
            StrStartOrEndDate,
            NewDate.Year,
            NewDate.Month,
            NewDate.Day,
            Loc->LocCod);
   DB_QueryUPDATE (Query,"can not update the date of a location");

   /***** Write message to show the change made *****/
   sprintf (StrDate,"%04u-%02u-%02u",
            NewDate.Year,NewDate.Month,NewDate.Day);	// Change format depending on location
   sprintf (Gbl.Message,Txt_The_date_of_the_location_has_changed_to_X,StrDate);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the form again *****/
   Dat_AssignDate (PtrDate,&NewDate);
   Loc_EditLocations ();
  }

/*****************************************************************************/
/***************************** Rename the location ***************************/
/*****************************************************************************/

void Loc_RenameLocation (void)
  {
   extern const char *Txt_You_can_not_leave_the_location_empty;
   extern const char *Txt_The_location_has_changed_to_X;
   extern const char *Txt_The_location_has_not_changed;
   char Query[512];
   struct Location *Loc;
   char NewLoc[Loc_MAX_LENGTH_LOCATION+1];

   Loc = &Gbl.Usrs.Me.Locs.EditingLoc;

   /***** Get parameters from form *****/
   /* Get the code of the location */
   if ((Loc->LocCod = Loc_GetParamLocCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of location is missing.");

   /* Get the new location */
   Par_GetParToText ("Location",NewLoc,Loc_MAX_LENGTH_LOCATION);

   /***** Get from the database the old location *****/
   Loc_GetDataOfLocationByCod (Loc);

   /***** Check if new location is empty *****/
   if (!NewLoc[0])
      Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_leave_the_location_empty);
   else
     {
      /***** Check if old and new locations are the same
             (this happens when user press enter
              with no changes in the form) *****/
      if (strcmp (Loc->Location,NewLoc))	// Different locations
        {
         /***** If degree was in database... *****/
	 /* Update the table changing old location by new location */
	 sprintf (Query,"UPDATE locations SET Location='%s' WHERE LocCod='%ld'",
		  NewLoc,Loc->LocCod);
	 DB_QueryUPDATE (Query,"can not update the location");

	 /***** Write message to show the change made *****/
	 sprintf (Gbl.Message,Txt_The_location_has_changed_to_X,NewLoc);
	 Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
        }
      else					// The same location
         Lay_ShowAlert (Lay_INFO,Txt_The_location_has_not_changed);
     }

   /***** Show the form again *****/
   strcpy (Loc->Location,NewLoc);
   Loc_EditLocations ();
  }

/*****************************************************************************/
/********************* Put a form to create a new location *******************/
/*****************************************************************************/

static void Loc_PutFormToCreateLocation (void)
  {
   extern const char *Hlp_PROFILE_Location_edit;
   extern const char *Txt_New_location;
   extern const char *Txt_Create_location;
   struct Location *Loc;

   Loc = &Gbl.Usrs.Me.Locs.EditingLoc;

   /***** Start form *****/
   Act_FormStart (ActNewLoc);

   /***** Start of frame *****/
   Lay_StartRoundFrameTable (NULL,Txt_New_location,
                             NULL,Hlp_PROFILE_Location_edit,2);

   /***** Table head *****/
   Loc_PutHeadLocations ();

   /***** Put disabled icon to remove centre *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"BM\">");
   Lay_PutIconRemovalNotAllowed ();
   fprintf (Gbl.F.Out,"</td>");

   /***** Location start date *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
   Dat_WriteFormDate (Gbl.Now.Date.Year - 1,
	              Gbl.Now.Date.Year + 1,
	              "Start",
                      &(Loc->StartDate),
                      false,false);
   fprintf (Gbl.F.Out,"</td>");

   /***** Location end date *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
   Dat_WriteFormDate (Gbl.Now.Date.Year - 1,
	              Gbl.Now.Date.Year + 1,
	              "End",
                      &(Loc->EndDate),
                      false,false);
   fprintf (Gbl.F.Out,"</td>");

   /***** Location *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"Location\""
                      " size=\"20\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>"
                      "<td></td>"
                      "</tr>",
            Loc_MAX_LENGTH_LOCATION,Loc->Location);

   /***** Send button and end of frame *****/
   Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_location);

   /***** End of form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************* Write header with fields of a location ******************/
/*****************************************************************************/

static void Loc_PutHeadLocations (void)
  {
   extern const char *Txt_Start_date;
   extern const char *Txt_End_date;
   extern const char *Txt_Location;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"BM\"></th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Start_date,
            Txt_End_date,
            Txt_Location);
  }

/*****************************************************************************/
/******************* Receive form to create a new location *******************/
/*****************************************************************************/

void Loc_RecFormNewLocation (void)
  {
   extern const char *Txt_You_must_specify_the_new_location;
   struct Location *Loc;

   Loc = &Gbl.Usrs.Me.Locs.EditingLoc;

   /***** Get start date *****/
   Dat_GetDateFromForm ("StartDay","StartMonth","StartYear",
                        &(Loc->StartDate.Day),&(Loc->StartDate.Month),&(Loc->StartDate.Year));
   if (Loc->StartDate.Day   == 0 ||
       Loc->StartDate.Month == 0 ||
       Loc->StartDate.Year  == 0)
      Dat_AssignDate (&Loc->StartDate,&Gbl.Now.Date);

   /***** Get end date *****/
   Dat_GetDateFromForm ("EndDay","EndMonth","EndYear",
			&(Loc->EndDate.Day),&(Loc->EndDate.Month),&(Loc->EndDate.Year));
   if (Loc->EndDate.Day   == 0 ||
       Loc->EndDate.Month == 0 ||
       Loc->EndDate.Year  == 0)
      Dat_AssignDate (&Loc->EndDate,&Gbl.Now.Date);

   /***** Get location *****/
   Par_GetParToText ("Location",Loc->Location,Loc_MAX_LENGTH_LOCATION);

   /***** Create the new location or write warning message *****/
   if (Loc->Location[0])	// If there's a location
      Loc_CreateLocation (Loc);
   else				// If there is not a location
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_new_location);

   /***** Show the form again *****/
   Loc_EditLocations ();
  }

/*****************************************************************************/
/**************************** Create a new location **************************/
/*****************************************************************************/

static void Loc_CreateLocation (struct Location *Loc)
  {
   extern const char *Txt_Created_new_location;
   char Query[256+Loc_MAX_LENGTH_LOCATION];

   /***** Create a new location *****/
   sprintf (Query,"INSERT INTO locations (StartDate,EndDate,Location)"
	          " VALUES ('%04u%02u%02u','%04u%02u%02u','%s')",
            Loc->StartDate.Year,
            Loc->StartDate.Month,
            Loc->StartDate.Day,
            Loc->EndDate.Year,
            Loc->EndDate.Month,
            Loc->EndDate.Day,
            Loc->Location);
   DB_QueryINSERT (Query,"can not create location");

   /***** Write success message *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_Created_new_location);
  }
