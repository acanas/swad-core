// swad_holiday.c: holidays

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_global.h"
#include "swad_holiday.h"
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

static void Hld_GetParamHldOrderType (void);
static void Hld_PutFormToEditHlds (void);
static Hld_HolidayType_t Hld_GetParamHldType (void);
static Hld_HolidayType_t Hld_GetTypeOfHoliday (const char *UnsignedStr);
static void Hld_ListHolidaysForEdition (void);
static void Hld_PutParamHldCod (long HldCod);
static void Hld_ChangeDate (Hld_StartOrEndDate_t StartOrEndDate);
static void Hld_PutFormToCreateHoliday (void);
static void Hld_PutHeadHolidays (void);
static void Hld_CreateHoliday (struct Holiday *Hld);

/*****************************************************************************/
/*************************** List all the holidays ***************************/
/*****************************************************************************/

void Hld_SeeHolidays (void)
  {
   extern const char *Txt_Holidays;
   extern const char *Txt_HOLIDAYS_HELP_ORDER[2];
   extern const char *Txt_HOLIDAYS_ORDER[2];
   extern const char *Txt_End_date;
   extern const char *Txt_Holiday;
   extern const char *Txt_All_places;
   Hld_OrderType_t Order;
   unsigned NumHld;

   if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      /***** Get parameter with the type of order in the list of holidays *****/
      Hld_GetParamHldOrderType ();

      /***** Get list of holidays *****/
      Hld_GetListHolidays ();

      /***** Put link (form) to edit holidays *****/
      if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
	 Hld_PutFormToEditHlds ();

      /***** Table head *****/
      Lay_StartRoundFrameTable10 (NULL,2,Txt_Holidays);
      fprintf (Gbl.F.Out,"<tr>");
      for (Order = Hld_ORDER_BY_PLACE;
	   Order <= Hld_ORDER_BY_START_DATE;
	   Order++)
	{
	 fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:center;\">");
	 Act_FormStart (ActSeeHld);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Act_LinkFormSubmit (Txt_HOLIDAYS_HELP_ORDER[Order],"TIT_TBL");
	 if (Order == Gbl.Hlds.SelectedOrderType)
	    fprintf (Gbl.F.Out,"<u>");
	 fprintf (Gbl.F.Out,"%s",Txt_HOLIDAYS_ORDER[Order]);
	 if (Order == Gbl.Hlds.SelectedOrderType)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Act_FormEnd ();
	 fprintf (Gbl.F.Out,"</td>");
	}
      fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:center;\">"
	                 "&nbsp;%s&nbsp;"
	                 "</td>"
			 "<td class=\"TIT_TBL\" style=\"text-align:center;\">"
			 "%s"
			 "</td>"
			 "</tr>",
	       Txt_End_date,
	       Txt_Holiday);

      /***** Write all the holidays *****/
      for (NumHld = 0;
	   NumHld < Gbl.Hlds.Num;
	   NumHld++)
	{
	 /* Write data of this holiday */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"DAT\" style=\"text-align:left;\">"
			    "%s"
			    "</td>",
		  Gbl.Hlds.Lst[NumHld].PlcCod <= 0 ? Txt_All_places :
						     Gbl.Hlds.Lst[NumHld].PlaceFullName);
	 fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:center;\">"
	                    "&nbsp;%02u/%02u/%04u"
	                    "</td>",
		  Gbl.Hlds.Lst[NumHld].StartDate.Day,
		  Gbl.Hlds.Lst[NumHld].StartDate.Month,
		  Gbl.Hlds.Lst[NumHld].StartDate.Year);
	 fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:center;\">"
	                    "&nbsp;");
	 switch (Gbl.Hlds.Lst[NumHld].HldTyp)
	   {
	    case Hld_HOLIDAY:
	       fprintf (Gbl.F.Out,"-");
	       break;
	    case Hld_NON_SCHOOL_PERIOD:
	       fprintf (Gbl.F.Out,"%02u/%02u/%04u",
			Gbl.Hlds.Lst[NumHld].EndDate.Day,
			Gbl.Hlds.Lst[NumHld].EndDate.Month,
			Gbl.Hlds.Lst[NumHld].EndDate.Year);
	       break;
	   }
	 fprintf (Gbl.F.Out,"</td>"
			    "<td class=\"DAT\" style=\"text-align:center;\">"
			    "&nbsp;%s"
			    "</td>"
			    "</tr>",
		  Gbl.Hlds.Lst[NumHld].Name);
	}

      /***** Table end *****/
      Lay_EndRoundFrameTable10 ();

      /***** Free list of holidays *****/
      Hld_FreeListHolidays ();
     }
  }

/*****************************************************************************/
/********* Get parameter with the type or order in list of holidays **********/
/*****************************************************************************/

static void Hld_GetParamHldOrderType (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("Order",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      Gbl.Hlds.SelectedOrderType = (Hld_OrderType_t) UnsignedNum;
   else
      Gbl.Hlds.SelectedOrderType = Hld_DEFAULT_ORDER_TYPE;
  }

/*****************************************************************************/
/********************* Put a link (form) to edit holidays ********************/
/*****************************************************************************/

static void Hld_PutFormToEditHlds (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Edit;

   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
   Act_FormStart (ActEdiHld);
   Act_LinkFormSubmit (Txt_Edit,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("edit",Txt_Edit,Txt_Edit);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************************ Put forms to edit degree types *********************/
/*****************************************************************************/

void Hld_EditHolidays (void)
  {
   extern const char *Txt_There_are_no_holidays;

   /***** Get list of places *****/
   Plc_GetListPlaces ();

   /***** Get list of holidays *****/
   Hld_GetListHolidays ();

   if (Gbl.Hlds.Num)
      /***** Put link (form) to view degree types *****/
      Lay_PutFormToView (ActSeeHld);
   else
      /***** Help message *****/
      Lay_ShowAlert (Lay_INFO,Txt_There_are_no_holidays);

   /***** Put a form to create a new holiday *****/
   Hld_PutFormToCreateHoliday ();

   /***** Forms to edit current holidays *****/
   if (Gbl.Hlds.Num)
      Hld_ListHolidaysForEdition ();

   /***** Free list of holidays *****/
   Hld_FreeListHolidays ();

   /***** Free list of places *****/
   Plc_FreeListPlaces ();
  }

/*****************************************************************************/
/*************************** List all the holidays ***************************/
/*****************************************************************************/

void Hld_GetListHolidays (void)
  {
   char OrderBySubQuery[256];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumHld;
   struct Holiday *Hld;

   if (Gbl.DB.DatabaseIsOpen)
     {
      if (Gbl.Hlds.LstIsRead)
	 Hld_FreeListHolidays ();

      /***** Get holidays from database *****/
      switch (Gbl.Hlds.SelectedOrderType)
	{
	 case Hld_ORDER_BY_PLACE:
	    sprintf (OrderBySubQuery,"Place,StartDate");
	    break;
	 case Hld_ORDER_BY_START_DATE:
	    sprintf (OrderBySubQuery,"StartDate,Place");
	    break;
	}
      sprintf (Query,"(SELECT holidays.HldCod,holidays.PlcCod,"
	             "places.FullName as Place,HldTyp,"
	             "DATE_FORMAT(holidays.StartDate,'%%Y%%m%%d') AS StartDate,"
	             "DATE_FORMAT(holidays.EndDate,'%%Y%%m%%d') AS EndDate,"
	             "holidays.Name"
		     " FROM holidays,places"
		     " WHERE holidays.InsCod='%ld'"
		     " AND holidays.PlcCod=places.PlcCod"
		     " AND places.InsCod='%ld')"
		     " UNION "
		     "(SELECT HldCod,PlcCod,'' as Place,HldTyp,"
		     "DATE_FORMAT(StartDate,'%%Y%%m%%d') AS StartDate,"
		     "DATE_FORMAT(EndDate,'%%Y%%m%%d') AS EndDate,Name"
		     " FROM holidays"
		     " WHERE InsCod='%ld'"
		     " AND PlcCod NOT IN"
		     "(SELECT DISTINCT PlcCod FROM places WHERE InsCod='%ld'))"
		     " ORDER BY %s",
	       Gbl.CurrentIns.Ins.InsCod,
	       Gbl.CurrentIns.Ins.InsCod,
	       Gbl.CurrentIns.Ins.InsCod,
	       Gbl.CurrentIns.Ins.InsCod,
	       OrderBySubQuery);
      if ((Gbl.Hlds.Num = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get holidays"))) // Holidays found...
	{
	 /***** Create list of holidays *****/
	 if ((Gbl.Hlds.Lst = (struct Holiday *) calloc ((size_t) Gbl.Hlds.Num,sizeof (struct Holiday))) == NULL)
	     Lay_ShowErrorAndExit ("Not enough memory to store holidays.");

	 /***** Get the holidays *****/
	 for (NumHld = 0;
	      NumHld < Gbl.Hlds.Num;
	      NumHld++)
	   {
	    Hld = &(Gbl.Hlds.Lst[NumHld]);

	    /* Get next holiday */
	    row = mysql_fetch_row (mysql_res);

	    /* Get holiday code (row[0]) */
	    if ((Hld->HldCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	       Lay_ShowErrorAndExit ("Wrong holiday code.");

	    /* Get place code (row[1]) */
	    Hld->PlcCod = Str_ConvertStrCodToLongCod (row[1]);

	    /* Get the full name of the place (row[2]) */
	    strcpy (Hld->PlaceFullName,row[2]);

	    /* Get type (row[3]) */
	    Hld->HldTyp = Hld_GetTypeOfHoliday (row[3]);

	    /* Get start date (row[4] holds the start date in YYYYMMDD format) */
	    if (!(Dat_GetDateFromYYYYMMDD (&(Hld->StartDate),row[4])))
	       Lay_ShowErrorAndExit ("Wrong start date.");

	    /* Get end date (row[5] holds the end date in YYYYMMDD format) */
	    if (!(Dat_GetDateFromYYYYMMDD (&(Hld->EndDate),row[5])))
	       Lay_ShowErrorAndExit ("Wrong end date.");

	    /* Get the name of the holiday/non school period (row[6]) */
	    strcpy (Hld->Name,row[6]);
	   }
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      Gbl.Hlds.LstIsRead = true;
     }
  }

/*****************************************************************************/
/**************************** Get holiday full name **************************/
/*****************************************************************************/

void Hld_GetDataOfHolidayByCod (struct Holiday *Hld)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Clear data *****/
   Hld->PlcCod = -1L;
   Hld->PlaceFullName[0] = '\0';
   Hld->StartDate.Day   =
   Hld->StartDate.Month =
   Hld->StartDate.Year  = 0;
   Hld->Name[0] = '\0';

   /***** Check if holiday code is correct *****/
   if (Hld->HldCod <= 0)
      Lay_ShowErrorAndExit ("Wrong code of holiday.");

   /***** Get data of holiday from database *****/
   sprintf (Query,"(SELECT holidays.PlcCod,places.FullName as Place,HldTyp,"
                  "DATE_FORMAT(holidays.StartDate,'%%Y%%m%%d'),"
                  "DATE_FORMAT(holidays.EndDate,'%%Y%%m%%d'),holidays.Name"
                  " FROM holidays,places"
                  " WHERE holidays.HldCod='%ld'"
                  " AND holidays.InsCod='%ld'"
                  " AND holidays.PlcCod=places.PlcCod"
                  " AND places.InsCod='%ld')"
                  " UNION "
                  "(SELECT PlcCod,'' as Place,HldTyp,"
                  "DATE_FORMAT(StartDate,'%%Y%%m%%d'),"
                  "DATE_FORMAT(EndDate,'%%Y%%m%%d'),Name"
                  " FROM holidays"
                  " WHERE HldCod='%ld'"
                  " AND InsCod='%ld'"
                  " AND PlcCod NOT IN"
                  "(SELECT DISTINCT PlcCod FROM places WHERE InsCod='%ld'))",
            Hld->HldCod,
            Gbl.CurrentIns.Ins.InsCod,
            Gbl.CurrentIns.Ins.InsCod,
            Hld->HldCod,
            Gbl.CurrentIns.Ins.InsCod,
            Gbl.CurrentIns.Ins.InsCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get data of a holiday")) // Holiday found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get place code (row[0]) */
      Hld->PlcCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get the full name of the place (row[1]) */
      strcpy (Hld->PlaceFullName,row[1]);

      /* Get type (row[2]) */
      Hld->HldTyp = Hld_GetTypeOfHoliday (row[2]);

      /* Get start date (row[3] holds the start date in YYYYMMDD format) */
      if (!(Dat_GetDateFromYYYYMMDD (&(Hld->StartDate),row[3])))
	 Lay_ShowErrorAndExit ("Wrong start date.");

      /* Get end date (row[4] holds the end date in YYYYMMDD format) */
      if (!(Dat_GetDateFromYYYYMMDD (&(Hld->EndDate),row[4])))
	 Lay_ShowErrorAndExit ("Wrong end date.");

      /* Get the name of the holiday/non school period (row[5]) */
      strcpy (Hld->Name,row[5]);
     }

  /***** Free structure that stores the query result *****/
  DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Get parameter from form with the type of a holiday *************/
/*****************************************************************************/

static Hld_HolidayType_t Hld_GetParamHldType (void)
  {
   char UnsignedStr[10+1];

   Par_GetParToText ("HldTyp",UnsignedStr,10);
   return Hld_GetTypeOfHoliday (UnsignedStr);
  }

/*****************************************************************************/
/********************* Get type of holiday from string ***********************/
/*****************************************************************************/

static Hld_HolidayType_t Hld_GetTypeOfHoliday (const char *UnsignedStr)
  {
   unsigned UnsignedNum;

   if (sscanf (UnsignedStr,"%u",&UnsignedNum) != 1)
      Lay_ShowErrorAndExit ("Wrong type of holiday.");

   if (UnsignedNum >= Hld_NUM_TYPES_HOLIDAY)
      Lay_ShowErrorAndExit ("Wrong type of holiday.");

   return (Hld_HolidayType_t) UnsignedNum;
  }

/*****************************************************************************/
/**************************** Free list of holidays **************************/
/*****************************************************************************/

void Hld_FreeListHolidays (void)
  {
   if (Gbl.Hlds.LstIsRead && Gbl.Hlds.Lst)
     {
      /***** Free memory used by the list of courses in degree *****/
      free ((void *) Gbl.Hlds.Lst);
      Gbl.Hlds.Lst = NULL;
      Gbl.Hlds.Num = 0;
      Gbl.Hlds.LstIsRead = false;
     }
  }

/*****************************************************************************/
/********************* List all the holidays for edition *********************/
/*****************************************************************************/

static void Hld_ListHolidaysForEdition (void)
  {
   extern const char *Txt_Holidays;
   extern const char *Txt_Remove_holiday;
   extern const char *Txt_All_places;
   extern const char *Txt_HOLIDAY_TYPES[Hld_NUM_TYPES_HOLIDAY];
   unsigned NumHld;
   unsigned NumPlc;
   struct Holiday *Hld;
   Hld_HolidayType_t HolidayType;

   Lay_StartRoundFrameTable10 (NULL,2,Txt_Holidays);

   /***** Table head *****/
   Hld_PutHeadHolidays ();

   /***** Write all the holidays *****/
   for (NumHld = 0;
	NumHld < Gbl.Hlds.Num;
	NumHld++)
     {
      Hld = &Gbl.Hlds.Lst[NumHld];

      /* Put icon to remove holiday */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"BM\">");
      Act_FormStart (ActRemHld);
      Hld_PutParamHldCod (Hld->HldCod);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/delon16x16.gif\""
	                 " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />",
               Gbl.Prefs.IconsURL,
               Txt_Remove_holiday,
               Txt_Remove_holiday);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Holiday code */
      fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:right;\">"
	                 "%ld&nbsp;"
	                 "</td>",
               Hld->HldCod);

      /* Holiday place */
      fprintf (Gbl.F.Out,"<td style=\"text-align:center;"
	                 " vertical-align:middle;\">");
      Act_FormStart (ActChgHldPlc);
      Hld_PutParamHldCod (Hld->HldCod);
      fprintf (Gbl.F.Out,"<select name=\"PlcCod\" style=\"width:50px;\""
	                 " onchange=\"javascript:document.getElementById('%s').submit();\">"
                         "<option value=\"-1\"",
	       Gbl.FormId);
      if (Hld->PlcCod <= 0)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",Txt_All_places);
      for (NumPlc = 0;
	   NumPlc < Gbl.Plcs.Num;
	   NumPlc++)
         fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
                  Gbl.Plcs.Lst[NumPlc].PlcCod,
                  Gbl.Plcs.Lst[NumPlc].PlcCod == Hld->PlcCod ? " selected=\"selected\"" :
                	                                       "",
                  Gbl.Plcs.Lst[NumPlc].ShortName);
      fprintf (Gbl.F.Out,"</select>");
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Holiday type */
      fprintf (Gbl.F.Out,"<td style=\"text-align:center;"
	                 " vertical-align:middle;\">");
      Act_FormStart (ActChgHldTyp);
      Hld_PutParamHldCod (Hld->HldCod);
      fprintf (Gbl.F.Out,"<select name=\"HldTyp\" style=\"width:50px;\""
	                 " onchange=\"javascript:document.getElementById('%s').submit();\">",
	       Gbl.FormId);
      for (HolidayType = (Hld_HolidayType_t) 0;
	   HolidayType < Hld_NUM_TYPES_HOLIDAY;
	   HolidayType++)
         fprintf (Gbl.F.Out,"<option value=\"%u\"%s>%s</option>",
                  (unsigned) HolidayType,
                  HolidayType == Hld->HldTyp ? " selected=\"selected\"" :
                	                       "",
                  Txt_HOLIDAY_TYPES[HolidayType]);
      fprintf (Gbl.F.Out,"</select>");
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Holiday date / Non school period start date */
      fprintf (Gbl.F.Out,"<td style=\"text-align:center;"
	                 " vertical-align:middle;\">");
      Act_FormStart (ActChgHldStrDat);
      Hld_PutParamHldCod (Hld->HldCod);
      Dat_WriteFormDate (Gbl.Now.Date.Year-1,
	                 Gbl.Now.Date.Year+1,
	                 "StartDay","StartMonth","StartYear",
                         &(Gbl.Hlds.Lst[NumHld].StartDate),
                         true,false);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Non school period end date */
      fprintf (Gbl.F.Out,"<td style=\"text-align:center;"
	                 " vertical-align:middle;\">");
      Act_FormStart (ActChgHldEndDat);
      Hld_PutParamHldCod (Hld->HldCod);
      Dat_WriteFormDate (Gbl.Now.Date.Year-1,
	                 Gbl.Now.Date.Year+1,
	                 "EndDay","EndMonth","EndYear",
                         &(Gbl.Hlds.Lst[NumHld].EndDate),
                         true,(Hld->HldTyp == Hld_HOLIDAY));
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Holiday name */
      fprintf (Gbl.F.Out,"<td style=\"text-align:center;"
	                 " vertical-align:middle;\">");
      Act_FormStart (ActRenHld);
      Hld_PutParamHldCod (Hld->HldCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Name\" size=\"20\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"javascript:document.getElementById('%s').submit();\" />",
               Hld_MAX_LENGTH_HOLIDAY_NAME,Hld->Name,Gbl.FormId);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>"
                         "</tr>");
     }

   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/******************** Write parameter with code of holiday *******************/
/*****************************************************************************/

static void Hld_PutParamHldCod (long HldCod)
  {
   Par_PutHiddenParamLong ("HldCod",HldCod);
  }

/*****************************************************************************/
/********************* Get parameter with code of holiday ********************/
/*****************************************************************************/

long Hld_GetParamHldCod (void)
  {
   char LongStr[1+10+1];

   /***** Get parameter with code of holiday *****/
   Par_GetParToText ("HldCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/******************************* Remove a holiday ****************************/
/*****************************************************************************/

void Hld_RemoveHoliday (void)
  {
   extern const char *Txt_Holiday_X_removed;
   char Query[512];
   struct Holiday Hld;

   /***** Get holiday code *****/
   if ((Hld.HldCod = Hld_GetParamHldCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of holiday is missing.");

   /***** Get data of the holiday from database *****/
   Hld_GetDataOfHolidayByCod (&Hld);

   /***** Remove holiday *****/
   sprintf (Query,"DELETE FROM holidays WHERE HldCod='%ld'",Hld.HldCod);
   DB_QueryDELETE (Query,"can not remove a holiday");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Holiday_X_removed,
            Hld.Name);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the form again *****/
   Hld_EditHolidays ();
  }

/*****************************************************************************/
/************************* Change the place of a holiday *********************/
/*****************************************************************************/

void Hld_ChangeHolidayPlace (void)
  {
   extern const char *Txt_The_place_of_the_holiday_X_has_changed_to_Y;
   char Query[512];
   struct Holiday *Hld;
   struct Place NewPlace;

   Hld = &Gbl.Hlds.EditingHld;

   /***** Get parameters from form *****/
   /* Get the code of the holiday */
   if ((Hld->HldCod = Hld_GetParamHldCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of holiday is missing.");

   /* Get the new place for the holiday */
   NewPlace.PlcCod = Plc_GetParamPlcCod ();

   /***** Get from the database the data of the place *****/
   Plc_GetDataOfPlaceByCod (&NewPlace);

   /***** Get from the database the data of the holiday *****/
   Hld_GetDataOfHolidayByCod (Hld);

   /***** Update the place in database *****/
   sprintf (Query,"UPDATE holidays SET PlcCod='%ld' WHERE HldCod='%ld'",
            NewPlace.PlcCod,Hld->HldCod);
   DB_QueryUPDATE (Query,"can not update the place of a holiday");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_The_place_of_the_holiday_X_has_changed_to_Y,
            Hld->Name,NewPlace.FullName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the form again *****/
   Hld->PlcCod = NewPlace.PlcCod;
   strcpy (Hld->PlaceFullName,NewPlace.FullName);
   Hld_EditHolidays ();
  }

/*****************************************************************************/
/************************* Change the type of a holiday **********************/
/*****************************************************************************/

void Hld_ChangeHolidayType (void)
  {
   extern const char *Txt_The_type_of_the_holiday_X_has_changed;
   char Query[512];
   struct Holiday *Hld;

   Hld = &Gbl.Hlds.EditingHld;

   /***** Get the code of the holiday *****/
   if ((Hld->HldCod = Hld_GetParamHldCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of holiday is missing.");

   /***** Get from the database the data of the holiday *****/
   Hld_GetDataOfHolidayByCod (Hld);

   /***** Get the new type for the holiday *****/
   Hld->HldTyp = Hld_GetParamHldType ();

   /***** Update holiday/no school period in database *****/
   switch (Hld->HldTyp)
     {
      case Hld_HOLIDAY:
         Hld->EndDate.Day = Hld->EndDate.Month = Hld->EndDate.Year = 0;
         sprintf (Query,"UPDATE holidays SET HldTyp='%u',EndDate='00000000'"
                        " WHERE HldCod='%ld'",
                  (unsigned) Hld_HOLIDAY,Hld->HldCod);
         break;
      case Hld_NON_SCHOOL_PERIOD:
         Hld->EndDate.Day   = Hld->StartDate.Day;
         Hld->EndDate.Month = Hld->StartDate.Month;
         Hld->EndDate.Year  = Hld->StartDate.Year;
         sprintf (Query,"UPDATE holidays SET HldTyp='%u',EndDate=StartDate"
                        " WHERE HldCod='%ld'",
                  (unsigned) Hld_NON_SCHOOL_PERIOD,Hld->HldCod);
         break;
     }
   DB_QueryUPDATE (Query,"can not update the type of a holiday");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_The_type_of_the_holiday_X_has_changed,
            Hld->Name);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the form again *****/
   Hld_EditHolidays ();
  }

/*****************************************************************************/
/*** Change the date of a holiday / the start date of a non school period ****/
/*****************************************************************************/

void Hld_ChangeStartDate (void)
  {
   Hld_ChangeDate (HLD_START_DATE);
  }

/*****************************************************************************/
/*************** Change the end date of a non school period ******************/
/*****************************************************************************/

void Hld_ChangeEndDate (void)
  {
   Hld_ChangeDate (HLD_END_DATE);
  }

/*****************************************************************************/
/**************** Change the start/end date of a holiday *********************/
/*****************************************************************************/

static void Hld_ChangeDate (Hld_StartOrEndDate_t StartOrEndDate)
  {
   extern const char *Txt_The_date_of_the_holiday_X_has_changed_to_Y;
   char Query[512];
   struct Holiday *Hld;
   struct Date NewDate;
   struct Date *PtrDate;
   const char *StrStartOrEndDate;
   char StrDate[11];

   Hld = &Gbl.Hlds.EditingHld;

   /***** Get the code of the holiday *****/
   if ((Hld->HldCod = Hld_GetParamHldCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of holiday is missing.");

   /***** Get from the database the data of the holiday *****/
   Hld_GetDataOfHolidayByCod (Hld);

   /***** Get the new date for the holiday *****/
   switch (StartOrEndDate)
     {
      case HLD_START_DATE:
         StrStartOrEndDate = "StartDate";
         PtrDate = &(Hld->StartDate);
         Dat_GetDateFromForm ("StartDay","StartMonth","StartYear",
                              &(NewDate.Day),&(NewDate.Month),&(NewDate.Year));
         if (NewDate.Day == 0 || NewDate.Month == 0 || NewDate.Year == 0)
           {
            NewDate.Day   = Gbl.Now.Date.Day;
            NewDate.Month = Gbl.Now.Date.Month;
            NewDate.Year  = Gbl.Now.Date.Year;
           }
         break;
      default:	// HLD_END_DATE
         StrStartOrEndDate = "EndDate";
         PtrDate = &(Hld->EndDate);
         switch (Hld->HldTyp)
           {
            case Hld_HOLIDAY:
               NewDate.Day = NewDate.Month = NewDate.Year = 0;
               break;
            case Hld_NON_SCHOOL_PERIOD:
               Dat_GetDateFromForm ("EndDay","EndMonth","EndYear",
                                    &(NewDate.Day),&(NewDate.Month),&(NewDate.Year));
               if (NewDate.Day == 0 || NewDate.Month == 0 || NewDate.Year == 0)
                 {
                  NewDate.Day   = Gbl.Now.Date.Day;
                  NewDate.Month = Gbl.Now.Date.Month;
                  NewDate.Year  = Gbl.Now.Date.Year;
                 }
               break;
           }
         break;
     }

   /***** Update the date in database *****/
   sprintf (Query,"UPDATE holidays SET %s='%04u%02u%02u' WHERE HldCod='%ld'",
            StrStartOrEndDate,NewDate.Year,NewDate.Month,NewDate.Day,Hld->HldCod);
   DB_QueryUPDATE (Query,"can not update the date of a holiday");

   /***** Write message to show the change made *****/
   sprintf (StrDate,"%02u/%02u/%04u",
            NewDate.Day,NewDate.Month,NewDate.Year);	// Change format depending on location
   sprintf (Gbl.Message,Txt_The_date_of_the_holiday_X_has_changed_to_Y,
            Hld->Name,StrDate);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the form again *****/
   Dat_AssignDate (PtrDate,&NewDate);
   Hld_EditHolidays ();
  }

/*****************************************************************************/
/************************ Change the name of a degree ************************/
/*****************************************************************************/

void Hld_RenameHoliday (void)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_holiday_X_empty;
   extern const char *Txt_The_name_of_the_holiday_X_has_changed_to_Y;
   extern const char *Txt_The_name_of_the_holiday_X_has_not_changed;
   char Query[512];
   struct Holiday *Hld;
   char NewHldName[Hld_MAX_LENGTH_HOLIDAY_NAME+1];

   Hld = &Gbl.Hlds.EditingHld;

   /***** Get parameters from form *****/
   /* Get the code of the holiday */
   if ((Hld->HldCod = Hld_GetParamHldCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of holiday is missing.");

   /* Get the new name for the holiday */
   Par_GetParToText ("Name",NewHldName,Hld_MAX_LENGTH_HOLIDAY_NAME);

   /***** Get from the database the old names of the holiday *****/
   Hld_GetDataOfHolidayByCod (Hld);

   /***** Check if new name is empty *****/
   if (!NewHldName[0])
     {
      sprintf (Gbl.Message,Txt_You_can_not_leave_the_name_of_the_holiday_X_empty,
               Hld->Name);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
   else
     {
      /***** Check if old and new names are the same (this happens when user press enter with no changes in the form) *****/
      if (strcmp (Hld->Name,NewHldName))	// Different names
        {
         /***** If degree was in database... *****/
	 /* Update the table changing old name by new name */
	 sprintf (Query,"UPDATE holidays SET Name='%s' WHERE HldCod='%ld'",
		  NewHldName,Hld->HldCod);
	 DB_QueryUPDATE (Query,"can not update the text of a holiday");

	 /***** Write message to show the change made *****/
	 sprintf (Gbl.Message,Txt_The_name_of_the_holiday_X_has_changed_to_Y,
		  Hld->Name,NewHldName);
	 Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
        }
      else	// The same name
        {
         sprintf (Gbl.Message,Txt_The_name_of_the_holiday_X_has_not_changed,
                 Hld->Name);
         Lay_ShowAlert (Lay_INFO,Gbl.Message);
        }
     }

   /***** Show the form again *****/
   strcpy (Hld->Name,NewHldName);
   Hld_EditHolidays ();
  }

/*****************************************************************************/
/********************* Put a form to create a new holiday ********************/
/*****************************************************************************/

static void Hld_PutFormToCreateHoliday (void)
  {
   extern const char *Txt_All_places;
   extern const char *Txt_New_holiday;
   extern const char *Txt_Place;
   extern const char *Txt_Type;
   extern const char *Txt_Start_date;
   extern const char *Txt_End_date;
   extern const char *Txt_Holiday;
   extern const char *Txt_HOLIDAY_TYPES[Hld_NUM_TYPES_HOLIDAY];
   extern const char *Txt_Create_holiday;
   struct Holiday *Hld;
   unsigned NumPlc;
   Hld_HolidayType_t HolidayType;

   Hld = &Gbl.Hlds.EditingHld;

   /***** Start form *****/
   Act_FormStart (ActNewHld);

   /***** Start of frame *****/
   Lay_StartRoundFrameTable10 (NULL,2,Txt_New_holiday);

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "</tr>",
            Txt_Place,
            Txt_Type,
            Txt_Start_date,
            Txt_End_date,
            Txt_Holiday);

   /***** Holiday place *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td style=\"text-align:center; vertical-align:middle;\">"
                      "<select name=\"PlcCod\" style=\"width:50px;\">"
                      "<option value=\"-1\"");
   if (Hld->PlcCod <= 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",Txt_All_places);
   for (NumPlc = 0;
	NumPlc < Gbl.Plcs.Num;
	NumPlc++)
      fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
               Gbl.Plcs.Lst[NumPlc].PlcCod,
               Gbl.Plcs.Lst[NumPlc].PlcCod == Hld->PlcCod ? " selected=\"selected\"" :
        	                                            "",
               Gbl.Plcs.Lst[NumPlc].ShortName);
   fprintf (Gbl.F.Out,"</select>"
	              "</td>");

   /***** Holiday type *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:center; vertical-align:middle;\">"
                      "<select name=\"HldTyp\" style=\"width:50px;\">");
   for (HolidayType = (Hld_HolidayType_t) 0;
	HolidayType < Hld_NUM_TYPES_HOLIDAY;
	HolidayType++)
      fprintf (Gbl.F.Out,"<option value=\"%u\"%s>%s</option>",
               (unsigned) HolidayType,
               HolidayType == Hld->HldTyp ? " selected=\"selected\"" :
        	                            "",
               Txt_HOLIDAY_TYPES[HolidayType]);
   fprintf (Gbl.F.Out,"</select>"
	              "</td>");

   /***** Holiday date / Non school period start date *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:center;"
	              " vertical-align:middle;\">");
   Dat_WriteFormDate (Gbl.Now.Date.Year-1,
	              Gbl.Now.Date.Year+1,
	              "StartDay","StartMonth","StartYear",
                      &(Hld->StartDate),
                      false,false);
   fprintf (Gbl.F.Out,"</td>");

   /***** Non school period end date *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:center;"
	              " vertical-align:middle;\">");
   Dat_WriteFormDate (Gbl.Now.Date.Year-1,
	              Gbl.Now.Date.Year+1,
	              "EndDay","EndMonth","EndYear",
                      &(Hld->EndDate),
                      false,false);
   fprintf (Gbl.F.Out,"</td>");

   /***** Holiday name *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:center; vertical-align:middle;\">"
                      "<input type=\"text\" name=\"Name\" size=\"20\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>"
                      "<td></td>"
                      "</tr>",
            Hld_MAX_LENGTH_HOLIDAY_NAME,Hld->Name);

   /***** Send button *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"5\" style=\"text-align:center;\">"
                      "<input type=\"submit\" value=\"%s\" />"
	              "</td>"
	              "</tr>",
            Txt_Create_holiday);

   /***** End of frame *****/
   Lay_EndRoundFrameTable10 ();

   /***** End of form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************* Write header with fields of a holiday *******************/
/*****************************************************************************/

static void Hld_PutHeadHolidays (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Place;
   extern const char *Txt_Type;
   extern const char *Txt_Start_date;
   extern const char *Txt_End_date;
   extern const char *Txt_Holiday;

   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"BM\"></td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:right;\">"
                      "%s"
                      "</td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "</tr>",
            Txt_Code,
            Txt_Place,
            Txt_Type,
            Txt_Start_date,
            Txt_End_date,
            Txt_Holiday);
  }

/*****************************************************************************/
/******************* Receive form to create a new holiday ********************/
/*****************************************************************************/

void Hld_RecFormNewHoliday (void)
  {
   extern const char *Txt_You_must_specify_the_name_of_the_new_holiday;
   struct Holiday *Hld;

   Hld = &Gbl.Hlds.EditingHld;

   /***** Get place code *****/
   Hld->PlcCod = Plc_GetParamPlcCod ();

   /***** Get the type of holiday *****/
   Hld->HldTyp = Hld_GetParamHldType ();

   /***** Get start date *****/
   Dat_GetDateFromForm ("StartDay","StartMonth","StartYear",
                        &(Hld->StartDate.Day),&(Hld->StartDate.Month),&(Hld->StartDate.Year));

   /***** Get end date *****/
   Dat_GetDateFromForm ("EndDay","EndMonth","EndYear",
                        &(Hld->EndDate.Day),&(Hld->EndDate.Month),&(Hld->EndDate.Year));

   /***** Get holiday name *****/
   Par_GetParToText ("Name",Hld->Name,Hld_MAX_LENGTH_HOLIDAY_NAME);

   /***** Adjust dates *****/
   if (Hld->StartDate.Day == 0 || Hld->StartDate.Month == 0 || Hld->StartDate.Year == 0)
     {
      Hld->StartDate.Day   = Gbl.Now.Date.Day;
      Hld->StartDate.Month = Gbl.Now.Date.Month;
      Hld->StartDate.Year  = Gbl.Now.Date.Year;
     }
   switch (Hld->HldTyp)
     {
      case Hld_HOLIDAY:
         Hld->EndDate.Day = Hld->EndDate.Month = Hld->EndDate.Year = 0;
         break;
      case Hld_NON_SCHOOL_PERIOD:
         if (Hld->EndDate.Day == 0 || Hld->EndDate.Month == 0 || Hld->EndDate.Year == 0)
           {
            Hld->EndDate.Day   = Gbl.Now.Date.Day;
            Hld->EndDate.Month = Gbl.Now.Date.Month;
            Hld->EndDate.Year  = Gbl.Now.Date.Year;
           }
         break;
     }

   /***** Create the new holiday or write warning message *****/
   if (Hld->Name[0])	// If there's a holiday name
      Hld_CreateHoliday (Hld);
   else	// If there is not a holiday name
     {
      sprintf (Gbl.Message,"%s",Txt_You_must_specify_the_name_of_the_new_holiday);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }

   /***** Show the form again *****/
   Hld_EditHolidays ();
  }

/*****************************************************************************/
/**************************** Create a new holiday ***************************/
/*****************************************************************************/

static void Hld_CreateHoliday (struct Holiday *Hld)
  {
   extern const char *Txt_Created_new_holiday_X;
   char Query[1024];

   /***** Create a new holiday or no school period *****/
   sprintf (Query,"INSERT INTO holidays (InsCod,PlcCod,HldTyp,StartDate,EndDate,Name)"
	          " VALUES ('%ld','%ld','%u','%04u%02u%02u','%04u%02u%02u','%s')",
            Gbl.CurrentIns.Ins.InsCod,Hld->PlcCod,(unsigned) Hld->HldTyp,
            Hld->StartDate.Year,Hld->StartDate.Month,Hld->StartDate.Day,
            Hld->EndDate.Year,Hld->EndDate.Month,Hld->EndDate.Day,
            Hld->Name);
   DB_QueryINSERT (Query,"can not create holiday");

   /***** Write success message *****/
   sprintf (Gbl.Message,Txt_Created_new_holiday_X,
            Hld->Name);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }
