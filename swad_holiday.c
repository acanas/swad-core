// swad_holiday.c: holidays

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

#include "swad_box.h"
#include "swad_calendar.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_holiday.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_parameter.h"

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

static struct Holiday *Hld_EditingHld = NULL;	// Static variable to keep the holiday being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Hld_GetParamHldOrder (void);
static void Hld_PutIconsSeeHolidays (void);

static void Hld_EditHolidaysInternal (void);

static void Hld_GetDataOfHolidayByCod (struct Holiday *Hld);

static Hld_HolidayType_t Hld_GetParamHldType (void);
static Hld_HolidayType_t Hld_GetTypeOfHoliday (const char *UnsignedStr);
static void Hld_ListHolidaysForEdition (void);
static void Hld_PutParamHldCod (long HldCod);
static void Hld_ChangeDate (Hld_StartOrEndDate_t StartOrEndDate);
static void Hld_PutFormToCreateHoliday (void);
static void Hld_PutHeadHolidays (void);
static void Hld_CreateHoliday (struct Holiday *Hld);

static void Hld_EditingHolidayConstructor (void);
static void Hld_EditingHolidayDestructor (void);

/*****************************************************************************/
/*************************** List all the holidays ***************************/
/*****************************************************************************/

void Hld_SeeHolidays (void)
  {
   extern const char *Hlp_INSTITUTION_Holidays;
   extern const char *Txt_Holidays;
   extern const char *Txt_HOLIDAYS_HELP_ORDER[2];
   extern const char *Txt_HOLIDAYS_ORDER[2];
   extern const char *Txt_End_date;
   extern const char *Txt_Holiday;
   extern const char *Txt_All_places;
   extern const char *Txt_No_holidays;
   extern const char *Txt_New_holiday;
   Hld_Order_t Order;
   unsigned NumHld;
   char StrDate[Cns_MAX_BYTES_DATE + 1];

   if (Gbl.Hierarchy.Ins.InsCod > 0)
     {
      /***** Get parameter with the type of order in the list of holidays *****/
      Hld_GetParamHldOrder ();

      /***** Get list of holidays *****/
      Hld_GetListHolidays ();

      /***** Table head *****/
      Box_BoxBegin (NULL,Txt_Holidays,Hld_PutIconsSeeHolidays,
                    Hlp_INSTITUTION_Holidays,Box_NOT_CLOSABLE);
      if (Gbl.Hlds.Num)
	 {
         HTM_TABLE_BeginWideMarginPadding (2);
         HTM_TR_Begin (NULL);

	 for (Order = Hld_ORDER_BY_PLACE;
	      Order <= Hld_ORDER_BY_START_DATE;
	      Order++)
	   {
	    HTM_TH_Begin (1,1,"LM");

	    Frm_StartForm (ActSeeHld);
	    Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	    Frm_LinkFormSubmit (Txt_HOLIDAYS_HELP_ORDER[Order],"TIT_TBL",NULL);
	    if (Order == Gbl.Hlds.SelectedOrder)
	       fprintf (Gbl.F.Out,"<u>");
	    fprintf (Gbl.F.Out,"%s",Txt_HOLIDAYS_ORDER[Order]);
	    if (Order == Gbl.Hlds.SelectedOrder)
	       fprintf (Gbl.F.Out,"</u>");
	    fprintf (Gbl.F.Out,"</a>");
	    Frm_EndForm ();

	    HTM_TH_End ();
	   }

	 HTM_TH_Begin (1,1,"LM");
	 fprintf (Gbl.F.Out,"&nbsp;%s&nbsp;",Txt_End_date);
	 HTM_TH_End ();

	 HTM_TH (1,1,"LM",Txt_Holiday);

	 HTM_TR_End ();

	 /***** Write all the holidays *****/
	 for (NumHld = 0;
	      NumHld < Gbl.Hlds.Num;
	      NumHld++)
	   {
	    /* Write data of this holiday */
	    HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"DAT LM\"");
	    fprintf (Gbl.F.Out,"%s",
		     Gbl.Hlds.Lst[NumHld].PlcCod <= 0 ? Txt_All_places :
							Gbl.Hlds.Lst[NumHld].PlaceFullName);
	    HTM_TD_End ();

	    Dat_ConvDateToDateStr (&Gbl.Hlds.Lst[NumHld].StartDate,StrDate);
	    HTM_TD_Begin ("class=\"DAT LM\"");
	    fprintf (Gbl.F.Out,"&nbsp;%s",StrDate);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"DAT LM\"");
	    fprintf (Gbl.F.Out,"&nbsp;");
	    switch (Gbl.Hlds.Lst[NumHld].HldTyp)
	      {
	       case Hld_HOLIDAY:
		  break;
	       case Hld_NON_SCHOOL_PERIOD:
	          Dat_ConvDateToDateStr (&Gbl.Hlds.Lst[NumHld].EndDate,StrDate);
		  fprintf (Gbl.F.Out,"%s",StrDate);
		  break;
	      }
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"DAT LM\"");
	    fprintf (Gbl.F.Out,"&nbsp;%s",Gbl.Hlds.Lst[NumHld].Name);
	    HTM_TD_End ();

	    HTM_TR_End ();
	   }
	 HTM_TABLE_End ();
	}
      else	// No holidays created in the current institution
	 Ale_ShowAlert (Ale_INFO,Txt_No_holidays);

      /***** Button to create centre *****/
      if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)	// Institution admin or system admin
	{
	 Frm_StartForm (ActEdiHld);
	 Btn_PutConfirmButton (Txt_New_holiday);
	 Frm_EndForm ();
	}

      /***** End box *****/
      Box_BoxEnd ();

      /***** Free list of holidays *****/
      Hld_FreeListHolidays ();
     }
  }

/*****************************************************************************/
/********* Get parameter with the type or order in list of holidays **********/
/*****************************************************************************/

static void Hld_GetParamHldOrder (void)
  {
   Gbl.Hlds.SelectedOrder = (Hld_Order_t)
	                    Par_GetParToUnsignedLong ("Order",
	                                              0,
	                                              Hld_NUM_ORDERS - 1,
	                                              (unsigned long) Hld_DEFAULT_ORDER_TYPE);
  }

/*****************************************************************************/
/******************** Put contextual icons in calendar ***********************/
/*****************************************************************************/

static void Hld_PutIconsSeeHolidays (void)
  {
   /***** Edit holidays calendar *****/
   if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      Ico_PutContextualIconToEdit (ActEdiHld,NULL);

   /***** View calendar *****/
   Cal_PutIconToSeeCalendar ();
  }

/*****************************************************************************/
/************************** Put icon to see holidays *************************/
/*****************************************************************************/

void Hld_PutIconToSeeHlds (void)
  {
   extern const char *Txt_Holidays;

   Lay_PutContextualLinkOnlyIcon (ActSeeHld,NULL,NULL,
				  "calendar-day.svg",
				  Txt_Holidays);
  }

/*****************************************************************************/
/************************ Put forms to edit degree types *********************/
/*****************************************************************************/

void Hld_EditHolidays (void)
  {
   /***** Holiday constructor *****/
   Hld_EditingHolidayConstructor ();

   /***** Edit holidays *****/
   Hld_EditHolidaysInternal ();

   /***** Holiday destructor *****/
   Hld_EditingHolidayDestructor ();
  }

static void Hld_EditHolidaysInternal (void)
  {
   /***** Get list of places *****/
   Plc_GetListPlaces ();

   /***** Get list of holidays *****/
   Hld_GetListHolidays ();

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
   static const char *OrderBySubQuery[Hld_NUM_ORDERS] =
     {
      "Place,StartDate",	// Hld_ORDER_BY_PLACE
      "StartDate,Place",	// Hld_ORDER_BY_START_DATE
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumHld;
   struct Holiday *Hld;

   if (Gbl.DB.DatabaseIsOpen)
     {
      if (Gbl.Hlds.LstIsRead)
	 Hld_FreeListHolidays ();

      /***** Get holidays from database *****/
      Gbl.Hlds.Num =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get holidays",
				 "(SELECT holidays.HldCod,"
				         "holidays.PlcCod,"
				         "places.FullName as Place,"
				         "holidays.HldTyp,"
				         "DATE_FORMAT(holidays.StartDate,'%%Y%%m%%d') AS StartDate,"
				         "DATE_FORMAT(holidays.EndDate,'%%Y%%m%%d') AS EndDate,"
				         "holidays.Name"
				 " FROM holidays,places"
				 " WHERE holidays.InsCod=%ld"
				 " AND holidays.PlcCod=places.PlcCod"
				 " AND places.InsCod=%ld)"
				 " UNION "
				 "(SELECT HldCod,"
					 "PlcCod,"
					 "'' as Place,"
					 "HldTyp,"
				         "DATE_FORMAT(StartDate,'%%Y%%m%%d') AS StartDate,"
					 "DATE_FORMAT(EndDate,'%%Y%%m%%d') AS EndDate,"
					 "Name"
				 " FROM holidays"
				 " WHERE InsCod=%ld"
				 " AND PlcCod NOT IN"
				 "(SELECT DISTINCT PlcCod FROM places"
				 " WHERE InsCod=%ld))"
				 " ORDER BY %s",
				 Gbl.Hierarchy.Ins.InsCod,
				 Gbl.Hierarchy.Ins.InsCod,
				 Gbl.Hierarchy.Ins.InsCod,
				 Gbl.Hierarchy.Ins.InsCod,
				 OrderBySubQuery[Gbl.Hlds.SelectedOrder]);
      if (Gbl.Hlds.Num) // Holidays found...
	{
	 /***** Create list of holidays *****/
	 if ((Gbl.Hlds.Lst = (struct Holiday *) calloc ((size_t) Gbl.Hlds.Num,sizeof (struct Holiday))) == NULL)
	     Lay_NotEnoughMemoryExit ();

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
	    Str_Copy (Hld->PlaceFullName,row[2],
	              Plc_MAX_BYTES_PLACE_FULL_NAME);

	    /* Get type (row[3]) */
	    Hld->HldTyp = Hld_GetTypeOfHoliday (row[3]);

	    /* Get start date (row[4] holds the start date in YYYYMMDD format) */
	    if (!(Dat_GetDateFromYYYYMMDD (&(Hld->StartDate),row[4])))
	       Lay_ShowErrorAndExit ("Wrong start date.");

	    /* Set / get end date */
	    switch (Hld->HldTyp)
	      {
	       case Hld_HOLIDAY:		// Only one day
		  /* Set end date = start date */
		  Dat_AssignDate (&Hld->EndDate,&Hld->StartDate);
		  break;
	       case Hld_NON_SCHOOL_PERIOD:	// One or more days
	          /* Get end date (row[5] holds the end date in YYYYMMDD format) */
		  if (!(Dat_GetDateFromYYYYMMDD (&(Hld->EndDate),row[5])))
	             Lay_ShowErrorAndExit ("Wrong end date.");
		  break;
	      }

	    /* Get the name of the holiday/non school period (row[6]) */
	    Str_Copy (Hld->Name,row[6],
	              Hld_MAX_BYTES_HOLIDAY_NAME);
	   }
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      Gbl.Hlds.LstIsRead = true;
     }
  }

/*****************************************************************************/
/************************* Get holiday data by code **************************/
/*****************************************************************************/

static void Hld_GetDataOfHolidayByCod (struct Holiday *Hld)
  {
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
   if (DB_QuerySELECT (&mysql_res,"can not get data of a holiday",
		       "(SELECT holidays.PlcCod,"
		               "places.FullName as Place,"
		               "holidays.HldTyp,"
		               "DATE_FORMAT(holidays.StartDate,'%%Y%%m%%d'),"
		               "DATE_FORMAT(holidays.EndDate,'%%Y%%m%%d'),"
		               "holidays.Name"
		       " FROM holidays,places"
		       " WHERE holidays.HldCod=%ld"
		       " AND holidays.InsCod=%ld"
		       " AND holidays.PlcCod=places.PlcCod"
		       " AND places.InsCod=%ld)"
		       " UNION "
		       "(SELECT PlcCod,"
		               "'' as Place,"
		               "HldTyp,"
		               "DATE_FORMAT(StartDate,'%%Y%%m%%d'),"
		               "DATE_FORMAT(EndDate,'%%Y%%m%%d'),"
		               "Name"
		       " FROM holidays"
		       " WHERE HldCod=%ld"
		       " AND InsCod=%ld"
		       " AND PlcCod NOT IN"
		       "(SELECT DISTINCT PlcCod FROM places"
		       " WHERE InsCod=%ld))",
		       Hld->HldCod,
		       Gbl.Hierarchy.Ins.InsCod,
		       Gbl.Hierarchy.Ins.InsCod,
		       Hld->HldCod,
		       Gbl.Hierarchy.Ins.InsCod,
		       Gbl.Hierarchy.Ins.InsCod)) // Holiday found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get place code (row[0]) */
      Hld->PlcCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get the full name of the place (row[1]) */
      Str_Copy (Hld->PlaceFullName,row[1],
                Plc_MAX_BYTES_PLACE_FULL_NAME);

      /* Get type (row[2]) */
      Hld->HldTyp = Hld_GetTypeOfHoliday (row[2]);

      /* Get start date (row[3] holds the start date in YYYYMMDD format) */
      if (!(Dat_GetDateFromYYYYMMDD (&(Hld->StartDate),row[3])))
	 Lay_ShowErrorAndExit ("Wrong start date.");

      /* Set / get end date */
      switch (Hld->HldTyp)
	{
	 case Hld_HOLIDAY:		// Only one day
	    /* Assign end date = start date */
	    Dat_AssignDate (&Hld->EndDate,&Hld->StartDate);
	    break;
	 case Hld_NON_SCHOOL_PERIOD:	// One or more days
	    /* Get end date (row[4] holds the end date in YYYYMMDD format) */
	    if (!(Dat_GetDateFromYYYYMMDD (&(Hld->EndDate),row[4])))
	       Lay_ShowErrorAndExit ("Wrong end date.");
	    break;
	}

      /* Get the name of the holiday/non school period (row[5]) */
      Str_Copy (Hld->Name,row[5],
                Hld_MAX_BYTES_HOLIDAY_NAME);
     }

  /***** Free structure that stores the query result *****/
  DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Get parameter from form with the type of a holiday *************/
/*****************************************************************************/

static Hld_HolidayType_t Hld_GetParamHldType (void)
  {
   return (Hld_HolidayType_t)
	  Par_GetParToUnsignedLong ("HldTyp",
	                            0,
	                            Hld_NUM_TYPES_HOLIDAY - 1,
	                            (unsigned long) Hld_HOLIDAY_TYPE_DEFAULT);
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
   extern const char *Hlp_INSTITUTION_Holidays_edit;
   extern const char *Txt_Holidays;
   extern const char *Txt_All_places;
   extern const char *Txt_HOLIDAY_TYPES[Hld_NUM_TYPES_HOLIDAY];
   unsigned NumHld;
   unsigned NumPlc;
   struct Holiday *Hld;
   Hld_HolidayType_t HolidayType;

   /***** Begin box and table *****/
   Box_StartBoxTable (NULL,Txt_Holidays,Cal_PutIconToSeeCalendar,
                      Hlp_INSTITUTION_Holidays_edit,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Hld_PutHeadHolidays ();

   /***** Write all the holidays *****/
   for (NumHld = 0;
	NumHld < Gbl.Hlds.Num;
	NumHld++)
     {
      Hld = &Gbl.Hlds.Lst[NumHld];

      HTM_TR_Begin (NULL);

      /* Put icon to remove holiday */
      HTM_TD_Begin ("class=\"BM\"");
      Frm_StartForm (ActRemHld);
      Hld_PutParamHldCod (Hld->HldCod);
      Ico_PutIconRemove ();
      Frm_EndForm ();
      HTM_TD_End ();

      /* Holiday code */
      HTM_TD_Begin ("class=\"DAT RM\"");
      fprintf (Gbl.F.Out,"%ld&nbsp;",Hld->HldCod);
      HTM_TD_End ();

      /* Holiday place */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActChgHldPlc);
      Hld_PutParamHldCod (Hld->HldCod);
      fprintf (Gbl.F.Out,"<select name=\"PlcCod\" class=\"PLC_COD\""
	                 " onchange=\"document.getElementById('%s').submit();\">"
                         "<option value=\"-1\"",
	       Gbl.Form.Id);
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
                  Gbl.Plcs.Lst[NumPlc].ShrtName);
      fprintf (Gbl.F.Out,"</select>");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Holiday type */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActChgHldTyp);
      Hld_PutParamHldCod (Hld->HldCod);
      fprintf (Gbl.F.Out,"<select name=\"HldTyp\" style=\"width:62px;\""
	                 " onchange=\"document.getElementById('%s').submit();\">",
	       Gbl.Form.Id);
      for (HolidayType = (Hld_HolidayType_t) 0;
	   HolidayType < Hld_NUM_TYPES_HOLIDAY;
	   HolidayType++)
         fprintf (Gbl.F.Out,"<option value=\"%u\"%s>%s</option>",
                  (unsigned) HolidayType,
                  HolidayType == Hld->HldTyp ? " selected=\"selected\"" :
                	                       "",
                  Txt_HOLIDAY_TYPES[HolidayType]);
      fprintf (Gbl.F.Out,"</select>");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Holiday date / Non school period start date */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActChgHldStrDat);
      Hld_PutParamHldCod (Hld->HldCod);
      Dat_WriteFormDate (Gbl.Now.Date.Year - 1,
	                 Gbl.Now.Date.Year + 1,
	                 "Start",
                         &(Gbl.Hlds.Lst[NumHld].StartDate),
                         true,false);
      Frm_EndForm ();
      HTM_TD_End ();

      /* Non school period end date */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActChgHldEndDat);
      Hld_PutParamHldCod (Hld->HldCod);
      Dat_WriteFormDate (Gbl.Now.Date.Year - 1,
	                 Gbl.Now.Date.Year + 1,
	                 "End",
                         &(Gbl.Hlds.Lst[NumHld].EndDate),
                         true,(Hld->HldTyp == Hld_HOLIDAY));
      Frm_EndForm ();
      HTM_TD_End ();

      /* Holiday name */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActRenHld);
      Hld_PutParamHldCod (Hld->HldCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Name\""
	                 " size=\"20\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Hld_MAX_CHARS_HOLIDAY_NAME,Hld->Name,Gbl.Form.Id);
      Frm_EndForm ();
      HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** End table and box *****/
   Box_EndBoxTable ();
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
   /***** Get code of holiday *****/
   return Par_GetParToLong ("HldCod");
  }

/*****************************************************************************/
/******************************* Remove a holiday ****************************/
/*****************************************************************************/

void Hld_RemoveHoliday (void)
  {
   extern const char *Txt_Holiday_X_removed;

   /***** Holiday constructor *****/
   Hld_EditingHolidayConstructor ();

   /***** Get holiday code *****/
   if ((Hld_EditingHld->HldCod = Hld_GetParamHldCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of holiday is missing.");

   /***** Get data of the holiday from database *****/
   Hld_GetDataOfHolidayByCod (Hld_EditingHld);

   /***** Remove holiday *****/
   DB_QueryDELETE ("can not remove a holiday",
		   "DELETE FROM holidays WHERE HldCod=%ld",
		   Hld_EditingHld->HldCod);

   /***** Write message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_Holiday_X_removed,
	            Hld_EditingHld->Name);
  }

/*****************************************************************************/
/************************* Change the place of a holiday *********************/
/*****************************************************************************/

void Hld_ChangeHolidayPlace (void)
  {
   extern const char *Txt_The_place_of_the_holiday_X_has_changed_to_Y;
   struct Place NewPlace;

   /***** Holiday constructor *****/
   Hld_EditingHolidayConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the holiday */
   if ((Hld_EditingHld->HldCod = Hld_GetParamHldCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of holiday is missing.");

   /* Get the new place for the holiday */
   NewPlace.PlcCod = Plc_GetParamPlcCod ();

   /***** Get from the database the data of the place *****/
   Plc_GetDataOfPlaceByCod (&NewPlace);

   /***** Get from the database the data of the holiday *****/
   Hld_GetDataOfHolidayByCod (Hld_EditingHld);

   /***** Update the place in database *****/
   DB_QueryUPDATE ("can not update the place of a holiday",
		   "UPDATE holidays SET PlcCod=%ld WHERE HldCod=%ld",
                   NewPlace.PlcCod,Hld_EditingHld->HldCod);
   Hld_EditingHld->PlcCod = NewPlace.PlcCod;
   Str_Copy (Hld_EditingHld->PlaceFullName,NewPlace.FullName,
             Plc_MAX_BYTES_PLACE_FULL_NAME);

   /***** Write message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_place_of_the_holiday_X_has_changed_to_Y,
                    Hld_EditingHld->Name,NewPlace.FullName);
  }

/*****************************************************************************/
/************************* Change the type of a holiday **********************/
/*****************************************************************************/

void Hld_ChangeHolidayType (void)
  {
   extern const char *Txt_The_type_of_the_holiday_X_has_changed;

   /***** Holiday constructor *****/
   Hld_EditingHolidayConstructor ();

   /***** Get the code of the holiday *****/
   if ((Hld_EditingHld->HldCod = Hld_GetParamHldCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of holiday is missing.");

   /***** Get from the database the data of the holiday *****/
   Hld_GetDataOfHolidayByCod (Hld_EditingHld);

   /***** Get the new type for the holiday *****/
   Hld_EditingHld->HldTyp = Hld_GetParamHldType ();

   /***** Update holiday/no school period in database *****/
   Dat_AssignDate (&Hld_EditingHld->EndDate,&Hld_EditingHld->StartDate);
   DB_QueryUPDATE ("can not update the type of a holiday",
		   "UPDATE holidays SET HldTyp=%u,EndDate=StartDate"
		   " WHERE HldCod=%ld",
	           (unsigned) Hld_EditingHld->HldTyp,Hld_EditingHld->HldCod);

   /***** Write message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_type_of_the_holiday_X_has_changed,
                    Hld_EditingHld->Name);
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
   struct Date NewDate;
   struct Date *PtrDate = NULL;			// Initialized to avoid warning
   const char *StrStartOrEndDate = NULL;	// Initialized to avoid warning
   char StrDate[Cns_MAX_BYTES_DATE + 1];

   /***** Holiday constructor *****/
   Hld_EditingHolidayConstructor ();

   /***** Get the code of the holiday *****/
   if ((Hld_EditingHld->HldCod = Hld_GetParamHldCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of holiday is missing.");

   /***** Get from the database the data of the holiday *****/
   Hld_GetDataOfHolidayByCod (Hld_EditingHld);

   /***** Get the new date for the holiday *****/
   switch (StartOrEndDate)
     {
      case HLD_START_DATE:
         StrStartOrEndDate = "StartDate";
         PtrDate = &Hld_EditingHld->StartDate;
         Dat_GetDateFromForm ("StartDay","StartMonth","StartYear",
                              &(NewDate.Day),&(NewDate.Month),&(NewDate.Year));
         if (NewDate.Day   == 0 ||
             NewDate.Month == 0 ||
             NewDate.Year  == 0)
            Dat_AssignDate (&NewDate,&Gbl.Now.Date);
         break;
      case HLD_END_DATE:
         StrStartOrEndDate = "EndDate";
         PtrDate = &Hld_EditingHld->EndDate;
         switch (Hld_EditingHld->HldTyp)
           {
            case Hld_HOLIDAY:
               Dat_AssignDate (&NewDate,&Hld_EditingHld->StartDate);
               break;
            case Hld_NON_SCHOOL_PERIOD:
               Dat_GetDateFromForm ("EndDay","EndMonth","EndYear",
                                    &(NewDate.Day),&(NewDate.Month),&(NewDate.Year));
               if (NewDate.Day   == 0 ||
        	   NewDate.Month == 0 ||
        	   NewDate.Year  == 0)
        	  Dat_AssignDate (&NewDate,&Gbl.Now.Date);
               break;
           }
         break;
     }

   /***** Update the date in database *****/
   DB_QueryUPDATE ("can not update the date of a holiday",
		   "UPDATE holidays SET %s='%04u%02u%02u' WHERE HldCod=%ld",
	           StrStartOrEndDate,
	           NewDate.Year,
	           NewDate.Month,
	           NewDate.Day,
	           Hld_EditingHld->HldCod);
   Dat_AssignDate (PtrDate,&NewDate);

   /***** Write message to show the change made *****/
   Dat_ConvDateToDateStr (&NewDate,StrDate);
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_date_of_the_holiday_X_has_changed_to_Y,
                    Hld_EditingHld->Name,StrDate);
  }

/*****************************************************************************/
/************************ Change the name of a degree ************************/
/*****************************************************************************/

void Hld_RenameHoliday (void)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_holiday_X_empty;
   extern const char *Txt_The_name_of_the_holiday_X_has_changed_to_Y;
   extern const char *Txt_The_name_of_the_holiday_X_has_not_changed;
   char NewHldName[Hld_MAX_BYTES_HOLIDAY_NAME + 1];

   /***** Holiday constructor *****/
   Hld_EditingHolidayConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the holiday */
   if ((Hld_EditingHld->HldCod = Hld_GetParamHldCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of holiday is missing.");

   /* Get the new name for the holiday */
   Par_GetParToText ("Name",NewHldName,Hld_MAX_BYTES_HOLIDAY_NAME);

   /***** Get from the database the old names of the holiday *****/
   Hld_GetDataOfHolidayByCod (Hld_EditingHld);

   /***** Check if new name is empty *****/
   if (!NewHldName[0])
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_leave_the_name_of_the_holiday_X_empty,
                       Hld_EditingHld->Name);
   else
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (Hld_EditingHld->Name,NewHldName))	// Different names
        {
         /***** If degree was in database... *****/
	 /* Update the table changing old name by new name */
	 DB_QueryUPDATE ("can not update the text of a holiday",
			 "UPDATE holidays SET Name='%s' WHERE HldCod=%ld",
		         NewHldName,Hld_EditingHld->HldCod);
	 Str_Copy (Hld_EditingHld->Name,NewHldName,
		   Hld_MAX_BYTES_HOLIDAY_NAME);

	 /***** Write message to show the change made *****/
         Ale_CreateAlert (Ale_SUCCESS,NULL,
                          Txt_The_name_of_the_holiday_X_has_changed_to_Y,
		          Hld_EditingHld->Name,NewHldName);
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_of_the_holiday_X_has_not_changed,
                          Hld_EditingHld->Name);
     }
  }

/*****************************************************************************/
/******** Show alerts after changing a holiday and continue editing **********/
/*****************************************************************************/

void Hld_ContEditAfterChgHld (void)
  {
   /***** Write message to show the change made *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Hld_EditHolidaysInternal ();

   /***** Holiday destructor *****/
   Hld_EditingHolidayDestructor ();
  }

/*****************************************************************************/
/********************* Put a form to create a new holiday ********************/
/*****************************************************************************/

static void Hld_PutFormToCreateHoliday (void)
  {
   extern const char *Hlp_INSTITUTION_Holidays_edit;
   extern const char *Txt_All_places;
   extern const char *Txt_New_holiday;
   extern const char *Txt_Place;
   extern const char *Txt_Type;
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Holiday;
   extern const char *Txt_HOLIDAY_TYPES[Hld_NUM_TYPES_HOLIDAY];
   extern const char *Txt_Create_holiday;
   unsigned NumPlc;
   Hld_HolidayType_t HolidayType;

   /***** Begin form *****/
   Frm_StartForm (ActNewHld);

   /***** Begin box and table *****/
   Box_StartBoxTable (NULL,Txt_New_holiday,NULL,
                      Hlp_INSTITUTION_Holidays_edit,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"LM",Txt_Place);
   HTM_TH (1,1,"LM",Txt_Type);
   HTM_TH (1,1,"LM",Txt_START_END_TIME[Dat_START_TIME]);
   HTM_TH (1,1,"LM",Txt_START_END_TIME[Dat_END_TIME]);
   HTM_TH (1,1,"LM",Txt_Holiday);

   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Holiday place *****/
   HTM_TD_Begin ("class=\"CM\"");
   fprintf (Gbl.F.Out,"<select name=\"PlcCod\" class=\"PLC_COD\">"
                      "<option value=\"-1\"");
   if (Hld_EditingHld->PlcCod <= 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",Txt_All_places);
   for (NumPlc = 0;
	NumPlc < Gbl.Plcs.Num;
	NumPlc++)
      fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
               Gbl.Plcs.Lst[NumPlc].PlcCod,
               Gbl.Plcs.Lst[NumPlc].PlcCod == Hld_EditingHld->PlcCod ? " selected=\"selected\"" :
        	                                                       "",
               Gbl.Plcs.Lst[NumPlc].ShrtName);
   fprintf (Gbl.F.Out,"</select>");
   HTM_TD_End ();

   /***** Holiday type *****/
   HTM_TD_Begin ("class=\"CM\"");
   fprintf (Gbl.F.Out,"<select name=\"HldTyp\" style=\"width:62px;\">");
   for (HolidayType = (Hld_HolidayType_t) 0;
	HolidayType < Hld_NUM_TYPES_HOLIDAY;
	HolidayType++)
      fprintf (Gbl.F.Out,"<option value=\"%u\"%s>%s</option>",
               (unsigned) HolidayType,
               HolidayType == Hld_EditingHld->HldTyp ? " selected=\"selected\"" :
        	                                       "",
               Txt_HOLIDAY_TYPES[HolidayType]);
   fprintf (Gbl.F.Out,"</select>");
   HTM_TD_End ();

   /***** Holiday date / Non school period start date *****/
   HTM_TD_Begin ("class=\"CM\"");
   Dat_WriteFormDate (Gbl.Now.Date.Year - 1,
	              Gbl.Now.Date.Year + 1,
	              "Start",
                      &Hld_EditingHld->StartDate,
                      false,false);
   HTM_TD_End ();

   /***** Non school period end date *****/
   HTM_TD_Begin ("class=\"CM\"");
   Dat_WriteFormDate (Gbl.Now.Date.Year - 1,
	              Gbl.Now.Date.Year + 1,
	              "End",
                      &Hld_EditingHld->EndDate,
                      false,false);
   HTM_TD_End ();

   /***** Holiday name *****/
   HTM_TD_Begin ("class=\"CM\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Name\""
                      " size=\"20\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />",
            Hld_MAX_CHARS_HOLIDAY_NAME,Hld_EditingHld->Name);
   HTM_TD_End ();

   HTM_TD_Empty (1);

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_holiday);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************* Write header with fields of a holiday *******************/
/*****************************************************************************/

static void Hld_PutHeadHolidays (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Place;
   extern const char *Txt_Type;
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Holiday;

   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"BM",NULL);
   HTM_TH (1,1,"RM",Txt_Code);
   HTM_TH (1,1,"LM",Txt_Place);
   HTM_TH (1,1,"LM",Txt_Type);
   HTM_TH (1,1,"LM",Txt_START_END_TIME[Dat_START_TIME]);
   HTM_TH (1,1,"LM",Txt_START_END_TIME[Dat_END_TIME]);
   HTM_TH (1,1,"LM",Txt_Holiday);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new holiday ********************/
/*****************************************************************************/

void Hld_RecFormNewHoliday (void)
  {
   extern const char *Txt_Created_new_holiday_X;
   extern const char *Txt_You_must_specify_the_name_of_the_new_holiday;

   /***** Holiday constructor *****/
   Hld_EditingHolidayConstructor ();

   /***** Get place code *****/
   Hld_EditingHld->PlcCod = Plc_GetParamPlcCod ();

   /***** Get the type of holiday *****/
   Hld_EditingHld->HldTyp = Hld_GetParamHldType ();

   /***** Get start date *****/
   Dat_GetDateFromForm ("StartDay","StartMonth","StartYear",
                        &Hld_EditingHld->StartDate.Day,
			&Hld_EditingHld->StartDate.Month,
			&Hld_EditingHld->StartDate.Year);
   if (Hld_EditingHld->StartDate.Day   == 0 ||
       Hld_EditingHld->StartDate.Month == 0 ||
       Hld_EditingHld->StartDate.Year  == 0)
      Dat_AssignDate (&Hld_EditingHld->StartDate,&Gbl.Now.Date);

   /***** Set end date *****/
   switch (Hld_EditingHld->HldTyp)
     {
      case Hld_HOLIDAY:
	 /* Set end date = start date (ignore end date from form) */
	 Dat_AssignDate (&Hld_EditingHld->EndDate,&Hld_EditingHld->StartDate);
         break;
      case Hld_NON_SCHOOL_PERIOD:
	 /* Get end date from form */
	 Dat_GetDateFromForm ("EndDay","EndMonth","EndYear",
			      &Hld_EditingHld->EndDate.Day,
			      &Hld_EditingHld->EndDate.Month,
			      &Hld_EditingHld->EndDate.Year);
	 if (Hld_EditingHld->EndDate.Day   == 0 ||
             Hld_EditingHld->EndDate.Month == 0 ||
             Hld_EditingHld->EndDate.Year  == 0)
            Dat_AssignDate (&Hld_EditingHld->EndDate,&Gbl.Now.Date);
         break;
     }

   /***** Get holiday name *****/
   Par_GetParToText ("Name",Hld_EditingHld->Name,Hld_MAX_BYTES_HOLIDAY_NAME);

   /***** Create the new holiday or set warning message *****/
   if (Hld_EditingHld->Name[0])	// If there's a holiday name
     {
      /* Create the new holiday */
      Hld_CreateHoliday (Hld_EditingHld);

      /* Success message */
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Created_new_holiday_X,
		       Hld_EditingHld->Name);
     }
   else	// If there is not a holiday name
      /* Error message */
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_name_of_the_new_holiday);
  }

/*****************************************************************************/
/**************************** Create a new holiday ***************************/
/*****************************************************************************/

static void Hld_CreateHoliday (struct Holiday *Hld)
  {
   /***** Create a new holiday or no school period *****/
   DB_QueryINSERT ("can not create holiday",
		   "INSERT INTO holidays"
		   " (InsCod,PlcCod,HldTyp,StartDate,EndDate,Name)"
		   " VALUES"
		   " (%ld,%ld,%u,'%04u%02u%02u','%04u%02u%02u','%s')",
	           Gbl.Hierarchy.Ins.InsCod,Hld->PlcCod,(unsigned) Hld->HldTyp,
	           Hld->StartDate.Year,
	           Hld->StartDate.Month,
	           Hld->StartDate.Day,
	           Hld->EndDate.Year,
	           Hld->EndDate.Month,
	           Hld->EndDate.Day,
	           Hld->Name);
  }

/*****************************************************************************/
/************************* Holiday constructor/destructor ********************/
/*****************************************************************************/

static void Hld_EditingHolidayConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Hld_EditingHld != NULL)
      Lay_ShowErrorAndExit ("Error initializing holiday.");

   /***** Allocate memory for holiday *****/
   if ((Hld_EditingHld = (struct Holiday *) malloc (sizeof (struct Holiday))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for holiday.");

   /***** Reset place *****/
   Hld_EditingHld->HldCod = -1L;
   Hld_EditingHld->PlcCod = -1L;
   Hld_EditingHld->PlaceFullName[0] = '\0';
   Hld_EditingHld->HldTyp = Hld_HOLIDAY;
   Hld_EditingHld->StartDate.Year  = Hld_EditingHld->EndDate.Year  = Gbl.Now.Date.Year;
   Hld_EditingHld->StartDate.Month = Hld_EditingHld->EndDate.Month = Gbl.Now.Date.Month;
   Hld_EditingHld->StartDate.Day   = Hld_EditingHld->EndDate.Day   = Gbl.Now.Date.Day;
   Hld_EditingHld->Name[0] = '\0';
  }

static void Hld_EditingHolidayDestructor (void)
  {
   /***** Free memory used for holiday *****/
   if (Hld_EditingHld != NULL)
     {
      free ((void *) Hld_EditingHld);
      Hld_EditingHld = NULL;
     }
  }
