// swad_holiday.c: holidays

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

#include <stddef.h>		// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_calendar.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_holiday.h"
#include "swad_holiday_database.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct Hld_Holiday *Hld_EditingHld = NULL;	// Static variable to keep the holiday being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static Usr_Can_t Hld_CheckIfICanEditHlds (void);

static Hld_Order_t Hld_GetParHldOrder (void);
static void Hld_PutIconsSeeHolidays (__attribute__((unused)) void *Args);
static void Hld_PutIconsEditHolidays (__attribute__((unused)) void *Args);

static void Hld_EditHolidaysInternal (void);

static void Hld_GetHolidayDataByCod (struct Hld_Holiday *Hld);
static void Hld_GetHolidayDataFromRow (MYSQL_RES *mysql_res,
                                       struct Hld_Holiday *Hld);

static Hld_HolidayType_t Hld_GetParHldType (void);
static Hld_HolidayType_t Hld_GetTypeOfHoliday (const char *UnsignedStr);
static void Hld_ListHolidaysForEdition (const struct Hld_Holidays *Holidays,
					const struct Plc_Places *Places);
static void Hld_PutParHldCod (void *HldCod);
static void Hld_ChangeDate (Hld_StartOrEndDate_t StartOrEndDate);
static void Hld_PutFormToCreateHoliday (const struct Plc_Places *Places);
static void Hld_PutHeadHolidays (void);

static void Hld_EditingHolidayConstructor (void);
static void Hld_EditingHolidayDestructor (void);

/*****************************************************************************/
/************************ Check if I can edit holidays ***********************/
/*****************************************************************************/

static Usr_Can_t Hld_CheckIfICanEditHlds (void)
  {
   static Usr_Can_t Hld_ICanEditHlds[Rol_NUM_ROLES] =
     {
      /* Users who can edit */
      [Rol_INS_ADM] = Usr_CAN,
      [Rol_SYS_ADM] = Usr_CAN,
     };

   return Hld_ICanEditHlds[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/************************* Reset departments context *************************/
/*****************************************************************************/

void Hld_ResetHolidays (struct Hld_Holidays *Holidays)
  {
   Holidays->LstIsRead     = false;	// List is not read
   Holidays->Num           = 0;
   Holidays->Lst           = NULL;
   Holidays->SelectedOrder = Hld_DEFAULT_ORDER_TYPE;
  }

/*****************************************************************************/
/****************************** List all holidays ****************************/
/*****************************************************************************/

void Hld_SeeAllHolidays (void)
  {
   extern const char *Hlp_INSTITUTION_Holidays;
   extern const char *Txt_Holidays;
   extern const char *Txt_HOLIDAYS_HELP_ORDER[2];
   extern const char *Txt_HOLIDAYS_ORDER[2];
   extern const char *Txt_End_date;
   extern const char *Txt_Holiday;
   extern const char *Txt_All_places;
   extern const char *Txt_No_holidays;
   Hld_Order_t Order;
   unsigned NumHld;
   char StrDate[Cns_MAX_BYTES_DATE + 1];
   struct Hld_Holidays Holidays;

   if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
     {
      /***** Reset places context *****/
      Hld_ResetHolidays (&Holidays);

      /***** Get parameter with the type of order in the list of holidays *****/
      Holidays.SelectedOrder = Hld_GetParHldOrder ();

      /***** Get list of holidays *****/
      Hld_GetListHolidays (&Holidays);

      /***** Table head *****/
      Box_BoxBegin (Txt_Holidays,Hld_PutIconsSeeHolidays,NULL,
                    Hlp_INSTITUTION_Holidays,Box_NOT_CLOSABLE);

	 if (Holidays.Num)
	    {
	    HTM_TABLE_BeginWideMarginPadding (2);
	       HTM_TR_Begin (NULL);

		  for (Order  = (Hld_Order_t) 0;
		       Order <= (Hld_Order_t) (Hld_NUM_ORDERS - 1);
		       Order++)
		    {
                     HTM_TH_Begin (HTM_HEAD_LEFT);
			Frm_BeginForm (ActSeeHld);
			   Par_PutParUnsigned (NULL,"Order",(unsigned) Order);
			   HTM_BUTTON_Submit_Begin (Txt_HOLIDAYS_HELP_ORDER[Order],
			                            "class=\"BT_LINK\"");
			      if (Order == Holidays.SelectedOrder)
				 HTM_U_Begin ();
			      HTM_Txt (Txt_HOLIDAYS_ORDER[Order]);
			      if (Order == Holidays.SelectedOrder)
				 HTM_U_End ();
			   HTM_BUTTON_End ();
			Frm_EndForm ();
		     HTM_TH_End ();
		    }
	          HTM_TH (Txt_End_date,HTM_HEAD_LEFT);
	          HTM_TH (Txt_Holiday,HTM_HEAD_LEFT);

	       HTM_TR_End ();

	       /***** Write all holidays *****/
	       for (NumHld = 0;
		    NumHld < Holidays.Num;
		    NumHld++)
		 {
		  /* Write data of this holiday */
		  HTM_TR_Begin (NULL);

		     HTM_TD_Txt_Left (Holidays.Lst[NumHld].PlcCod <= 0 ? Txt_All_places :
								         Holidays.Lst[NumHld].PlaceFullName);

		     Dat_ConvDateToDateStr (&Holidays.Lst[NumHld].StartDate,StrDate);

		     HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
			HTM_Txt (StrDate);
		     HTM_TD_End ();

		     HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
			switch (Holidays.Lst[NumHld].HldTyp)
			  {
			   case Hld_HOLIDAY:
			      break;
			   case Hld_NON_SCHOOL_PERIOD:
			      Dat_ConvDateToDateStr (&Holidays.Lst[NumHld].EndDate,StrDate);
			      HTM_Txt (StrDate);
			      break;
			  }
		     HTM_TD_End ();

		     HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
			HTM_Txt (Holidays.Lst[NumHld].Name);
		     HTM_TD_End ();

		  HTM_TR_End ();
		 }
	    HTM_TABLE_End ();
	   }
	 else	// No holidays created in the current institution
	    Ale_ShowAlert (Ale_INFO,Txt_No_holidays);

      /***** End box *****/
      Box_BoxEnd ();

      /***** Free list of holidays *****/
      Hld_FreeListHolidays (&Holidays);
     }
  }

/*****************************************************************************/
/********* Get parameter with the type or order in list of holidays **********/
/*****************************************************************************/

static Hld_Order_t Hld_GetParHldOrder (void)
  {
   return (Hld_Order_t) Par_GetParUnsignedLong ("Order",
						0,
						Hld_NUM_ORDERS - 1,
						(unsigned long) Hld_DEFAULT_ORDER_TYPE);
  }

/*****************************************************************************/
/******************** Put contextual icons in calendar ***********************/
/*****************************************************************************/

static void Hld_PutIconsSeeHolidays (__attribute__((unused)) void *Args)
  {
   /***** Edit holidays *****/
   if (Hld_CheckIfICanEditHlds () == Usr_CAN)
      Ico_PutContextualIconToEdit (ActEdiHld,NULL,NULL,NULL);

   /***** View calendar *****/
   Cal_PutIconToSeeCalendar ();
  }

static void Hld_PutIconsEditHolidays (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view holidays *****/
   Ico_PutContextualIconToView (ActSeeHld,NULL,NULL,NULL);

   /***** View calendar *****/
   Cal_PutIconToSeeCalendar ();
  }

/*****************************************************************************/
/************************** Put icon to see holidays *************************/
/*****************************************************************************/

void Hld_PutIconToSeeHlds (void)
  {
   Lay_PutContextualLinkOnlyIcon (ActSeeHld,NULL,
                                  NULL,NULL,
				  "calendar-day.svg",Ico_BLACK);
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
   struct Hld_Holidays Holidays;
   struct Plc_Places Places;

   /***** Reset places context *****/
   Hld_ResetHolidays (&Holidays);

   /***** Reset places context *****/
   Plc_ResetPlaces (&Places);

   /***** Get list of places *****/
   Plc_GetListPlaces (&Places);

   /***** Get list of holidays *****/
   Hld_GetListHolidays (&Holidays);

   /***** Forms to edit current holidays *****/
   Hld_ListHolidaysForEdition (&Holidays,&Places);

   /***** Free list of holidays *****/
   Hld_FreeListHolidays (&Holidays);

   /***** Free list of places *****/
   Plc_FreeListPlaces (&Places);
  }

/*****************************************************************************/
/***************************** List all holidays *****************************/
/*****************************************************************************/

void Hld_GetListHolidays (struct Hld_Holidays *Holidays)
  {
   MYSQL_RES *mysql_res;
   unsigned NumHld;

   if (DB_CheckIfDatabaseIsOpen ())
     {
      if (Holidays->LstIsRead)
	 Hld_FreeListHolidays (Holidays);

      /***** Get holidays from database *****/
      if ((Holidays->Num = Hld_DB_GetListHolidays (&mysql_res,Holidays->SelectedOrder))) // Holidays found...
	{
	 /***** Create list of holidays *****/
	 if ((Holidays->Lst = calloc (Holidays->Num,
	                              sizeof (*Holidays->Lst))) == NULL)
	     Err_NotEnoughMemoryExit ();

	 /***** Get the holidays *****/
	 for (NumHld = 0;
	      NumHld < Holidays->Num;
	      NumHld++)
	    Hld_GetHolidayDataFromRow (mysql_res,&Holidays->Lst[NumHld]);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      Holidays->LstIsRead = true;
     }
  }

/*****************************************************************************/
/************************* Get holiday data by code **************************/
/*****************************************************************************/

static void Hld_GetHolidayDataByCod (struct Hld_Holiday *Hld)
  {
   MYSQL_RES *mysql_res;

   /***** Clear data *****/
   Hld->PlcCod = -1L;
   Hld->PlaceFullName[0] = '\0';
   Hld->StartDate.Day   =
   Hld->StartDate.Month =
   Hld->StartDate.Year  = 0;
   Hld->Name[0] = '\0';

   /***** Check if holiday code is correct *****/
   if (Hld->HldCod <= 0)
      Err_WrongHolidayExit ();

   /***** Get data of holiday from database *****/
   if (Hld_DB_GetHolidayDataByCod (&mysql_res,Hld->HldCod)) // Holiday found...
      Hld_GetHolidayDataFromRow (mysql_res,Hld);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Get holiday data from database row *********************/
/*****************************************************************************/

static void Hld_GetHolidayDataFromRow (MYSQL_RES *mysql_res,
                                       struct Hld_Holiday *Hld)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]:	HldCod
   row[1]:	PlcCod
   row[2]:	Place
   row[3]:	HldTyp
   row[4]:	StartDate
   row[5]:	EndDate
   row[6]:	Name
   */
   /***** Get holiday code (row[0]) and place code (row[1]) *****/
   Hld->HldCod = Str_ConvertStrCodToLongCod (row[0]);
   Hld->PlcCod = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get the full name of the place (row[2]) *****/
   Str_Copy (Hld->PlaceFullName,row[2],sizeof (Hld->PlaceFullName) - 1);

   /***** Get type (row[3]) *****/
   Hld->HldTyp = Hld_GetTypeOfHoliday (row[3]);

   /***** Get start date (row[4] holds the date in YYYYMMDD format) *****/
   if (!(Dat_GetDateFromYYYYMMDD (&(Hld->StartDate),row[4])))
      Err_WrongDateExit ();

   /***** Set / get end date *****/
   switch (Hld->HldTyp)
     {
      case Hld_HOLIDAY:		// Only one day
	 /* Assign end date = start date */
	 Dat_AssignDate (&Hld->EndDate,&Hld->StartDate);
	 break;
      case Hld_NON_SCHOOL_PERIOD:	// One or more days
	 /* Get end date (row[5] holds the date in YYYYMMDD format) */
	 if (!(Dat_GetDateFromYYYYMMDD (&(Hld->EndDate),row[5])))
	    Err_WrongDateExit ();
	 break;
     }

   /***** Get the name of the holiday/non school period (row[6]) *****/
   Str_Copy (Hld->Name,row[6],sizeof (Hld->Name) - 1);
  }

/*****************************************************************************/
/************ Get parameter from form with the type of a holiday *************/
/*****************************************************************************/

static Hld_HolidayType_t Hld_GetParHldType (void)
  {
   return (Hld_HolidayType_t)
	  Par_GetParUnsignedLong ("HldTyp",
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
      Err_WrongHolidayExit ();

   if (UnsignedNum >= Hld_NUM_TYPES_HOLIDAY)
      Err_WrongHolidayExit ();

   return (Hld_HolidayType_t) UnsignedNum;
  }

/*****************************************************************************/
/**************************** Free list of holidays **************************/
/*****************************************************************************/

void Hld_FreeListHolidays (struct Hld_Holidays *Holidays)
  {
   if (Holidays->LstIsRead && Holidays->Lst)
     {
      /***** Free memory used by the list of courses in degree *****/
      free (Holidays->Lst);
      Holidays->Lst = NULL;
      Holidays->Num = 0;
      Holidays->LstIsRead = false;
     }
  }

/*****************************************************************************/
/*********************** List all holidays for edition ***********************/
/*****************************************************************************/

static void Hld_ListHolidaysForEdition (const struct Hld_Holidays *Holidays,
					const struct Plc_Places *Places)
  {
   extern const char *Hlp_INSTITUTION_Holidays_edit;
   extern const char *Txt_Holidays;
   extern const char *Txt_All_places;
   extern const char *Txt_HOLIDAY_TYPES[Hld_NUM_TYPES_HOLIDAY];
   unsigned NumHld;
   struct Hld_Holiday *Hld;
   unsigned NumPlc;
   const struct Plc_Place *Plc;
   Hld_HolidayType_t HolidayType;
   unsigned HolidayTypeUnsigned;
   unsigned CurrentYear = Dat_GetCurrentYear ();

   /***** Begin box *****/
   Box_BoxBegin (Txt_Holidays,Hld_PutIconsEditHolidays,NULL,
                 Hlp_INSTITUTION_Holidays_edit,Box_NOT_CLOSABLE);

      /***** Put a form to create a new holiday *****/
      Hld_PutFormToCreateHoliday (Places);

      if (Places->Num)
	{
	 /***** Begin table *****/
	 HTM_TABLE_Begin ("TBL_SCROLL");

	    /***** Write heading *****/
	    Hld_PutHeadHolidays ();

	    /***** Write all holidays *****/
	    for (NumHld = 0;
		 NumHld < Holidays->Num;
		 NumHld++)
	      {
	       Hld = &Holidays->Lst[NumHld];

	       HTM_TR_Begin (NULL);

		  /* Put icon to remove holiday */
		  HTM_TD_Begin ("class=\"BM\"");
		     Ico_PutContextualIconToRemove (ActRemHld,NULL,
						    Hld_PutParHldCod,&Hld->HldCod);
		  HTM_TD_End ();

		  /* Holiday code */
		  HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
		     HTM_TxtF ("%ld&nbsp;",Hld->HldCod);
		  HTM_TD_End ();

		  /* Holiday place */
		  HTM_TD_Begin ("class=\"CM\"");
		     Frm_BeginForm (ActChgHldPlc);
			ParCod_PutPar (ParCod_Hld,Hld->HldCod);
			HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
					  "name=\"PlcCod\" class=\"PLC_SEL INPUT_%s\"",
					  The_GetSuffix ());
			   HTM_OPTION (HTM_Type_STRING,"-1",
				       (Hld->PlcCod <= 0) ? HTM_SELECTED :
							    HTM_NO_ATTR,
				       "%s",Txt_All_places);
			   for (NumPlc = 0;
				NumPlc < Places->Num;
				NumPlc++)
			     {
			      Plc = &Places->Lst[NumPlc];
			      HTM_OPTION (HTM_Type_LONG,&Plc->PlcCod,
					  (Plc->PlcCod == Hld->PlcCod) ? HTM_SELECTED :
								         HTM_NO_ATTR,
					  "%s",Plc->ShrtName);
			     }
			HTM_SELECT_End ();
		     Frm_EndForm ();
		  HTM_TD_End ();

		  /* Holiday type */
		  HTM_TD_Begin ("class=\"CM\"");
		     Frm_BeginForm (ActChgHldTyp);
			ParCod_PutPar (ParCod_Hld,Hld->HldCod);
			HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
					  "name=\"HldTyp\" class=\"INPUT_%s\""
					  " style=\"width:62px;\"",	// TODO: Use a CSS class
					  The_GetSuffix ());
			   for (HolidayType  = (Hld_HolidayType_t) 0;
				HolidayType <= (Hld_HolidayType_t) (Hld_NUM_TYPES_HOLIDAY - 1);
				HolidayType++)
			     {
			      HolidayTypeUnsigned = (unsigned) HolidayType;
			      HTM_OPTION (HTM_Type_UNSIGNED,&HolidayTypeUnsigned,
					  (HolidayType == Hld->HldTyp) ? HTM_SELECTED :
								         HTM_NO_ATTR,
					  "%s",Txt_HOLIDAY_TYPES[HolidayType]);
			     }
			HTM_SELECT_End ();
		     Frm_EndForm ();
		  HTM_TD_End ();

		  /* Holiday date / Non school period start date */
		  HTM_TD_Begin ("class=\"CM\"");
		     Frm_BeginForm (ActChgHldStrDat);
			ParCod_PutPar (ParCod_Hld,Hld->HldCod);
			Dat_WriteFormDate (CurrentYear - 1,
					   CurrentYear + 1,
					   "Start",
					   &(Holidays->Lst[NumHld].StartDate),
					   HTM_SUBMIT_ON_CHANGE);
		     Frm_EndForm ();
		  HTM_TD_End ();

		  /* Non school period end date */
		  HTM_TD_Begin ("class=\"CM\"");
		     Frm_BeginForm (ActChgHldEndDat);
			ParCod_PutPar (ParCod_Hld,Hld->HldCod);
			Dat_WriteFormDate (CurrentYear - 1,
					   CurrentYear + 1,
					   "End",
					   &(Holidays->Lst[NumHld].EndDate),
					   ((Hld->HldTyp == Hld_HOLIDAY) ? HTM_DISABLED :
									   HTM_NO_ATTR) | HTM_SUBMIT_ON_CHANGE);
		     Frm_EndForm ();
		  HTM_TD_End ();

		  /* Holiday name */
		  HTM_TD_Begin ("class=\"CM\"");
		     Frm_BeginForm (ActRenHld);
			ParCod_PutPar (ParCod_Hld,Hld->HldCod);
			HTM_INPUT_TEXT ("Name",Hld_MAX_CHARS_HOLIDAY_NAME,Hld->Name,
					HTM_SUBMIT_ON_CHANGE,
					"size=\"20\" class=\"INPUT_%s\"",
					The_GetSuffix ());
		     Frm_EndForm ();
		  HTM_TD_End ();
	       HTM_TR_End ();
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();
	}

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************** Write parameter with code of holiday *******************/
/*****************************************************************************/

static void Hld_PutParHldCod (void *HldCod)
  {
   if (HldCod)
      ParCod_PutPar (ParCod_Hld,*((long *) HldCod));
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
   Hld_EditingHld->HldCod = ParCod_GetAndCheckPar (ParCod_Hld);

   /***** Get data of the holiday from database *****/
   Hld_GetHolidayDataByCod (Hld_EditingHld);

   /***** Remove holiday *****/
   Hld_DB_RemoveHoliday (Hld_EditingHld->HldCod);

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
   struct Plc_Place NewPlace;

   /***** Holiday constructor *****/
   Hld_EditingHolidayConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the holiday */
   Hld_EditingHld->HldCod = ParCod_GetAndCheckPar (ParCod_Hld);

   /* Get the new place for the holiday */
   // <0 (place unspecified/all places) and 0 (another place) are allowed here
   NewPlace.PlcCod = ParCod_GetPar (ParCod_Plc);

   /***** Get place and holiday data from database *****/
   Plc_GetPlaceDataByCod (&NewPlace);
   Hld_GetHolidayDataByCod (Hld_EditingHld);

   /***** Update the place in database *****/
   Hld_DB_ChangePlace (Hld_EditingHld->HldCod,NewPlace.PlcCod);
   Hld_EditingHld->PlcCod = NewPlace.PlcCod;
   Str_Copy (Hld_EditingHld->PlaceFullName,NewPlace.FullName,
             sizeof (Hld_EditingHld->PlaceFullName) - 1);

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
   Hld_EditingHld->HldCod = ParCod_GetAndCheckPar (ParCod_Hld);

   /***** Get from the database the data of the holiday *****/
   Hld_GetHolidayDataByCod (Hld_EditingHld);

   /***** Get the new type for the holiday *****/
   Hld_EditingHld->HldTyp = Hld_GetParHldType ();

   /***** Update holiday/no school period in database *****/
   Dat_AssignDate (&Hld_EditingHld->EndDate,&Hld_EditingHld->StartDate);
   Hld_DB_ChangeType (Hld_EditingHld->HldCod,Hld_EditingHld->HldTyp);

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
   Hld_ChangeDate (Hld_START_DATE);
  }

/*****************************************************************************/
/*************** Change the end date of a non school period ******************/
/*****************************************************************************/

void Hld_ChangeEndDate (void)
  {
   Hld_ChangeDate (Hld_END_DATE);
  }

/*****************************************************************************/
/**************** Change the start/end date of a holiday *********************/
/*****************************************************************************/

static void Hld_ChangeDate (Hld_StartOrEndDate_t StartOrEndDate)
  {
   extern const char *Txt_The_date_of_the_holiday_X_has_changed_to_Y;
   struct Dat_Date NewDate;
   struct Dat_Date *PtrDate = NULL;			// Initialized to avoid warning
   const char *StrStartOrEndDate = NULL;	// Initialized to avoid warning
   char StrDate[Cns_MAX_BYTES_DATE + 1];

   /***** Holiday constructor *****/
   Hld_EditingHolidayConstructor ();

   /***** Get the code of the holiday *****/
   Hld_EditingHld->HldCod = ParCod_GetAndCheckPar (ParCod_Hld);

   /***** Get from the database the data of the holiday *****/
   Hld_GetHolidayDataByCod (Hld_EditingHld);

   /***** Get the new date for the holiday *****/
   switch (StartOrEndDate)
     {
      case Hld_START_DATE:
         StrStartOrEndDate = "StartDate";
         PtrDate = &Hld_EditingHld->StartDate;
         Dat_GetDateFromForm ("StartDay","StartMonth","StartYear",
                              &(NewDate.Day),&(NewDate.Month),&(NewDate.Year));
         if (NewDate.Day   == 0 ||
             NewDate.Month == 0 ||
             NewDate.Year  == 0)
            Dat_AssignDate (&NewDate,Dat_GetCurrentDate ());
         break;
      case Hld_END_DATE:
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
        	  Dat_AssignDate (&NewDate,Dat_GetCurrentDate ());
               break;
           }
         break;
     }

   /***** Update the date in database *****/
   Hld_DB_ChangeDate (Hld_EditingHld->HldCod,StrStartOrEndDate,&NewDate);
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
   extern const char *Txt_The_holiday_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   char NewHldName[Hld_MAX_BYTES_HOLIDAY_NAME + 1];

   /***** Holiday constructor *****/
   Hld_EditingHolidayConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the holiday */
   Hld_EditingHld->HldCod = ParCod_GetAndCheckPar (ParCod_Hld);

   /* Get the new name for the holiday */
   Par_GetParText ("Name",NewHldName,Hld_MAX_BYTES_HOLIDAY_NAME);

   /***** Get from the database the old names of the holiday *****/
   Hld_GetHolidayDataByCod (Hld_EditingHld);

   /***** Check if new name is empty *****/
   if (NewHldName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (Hld_EditingHld->Name,NewHldName))	// Different names
        {
         /***** If degree was in database... *****/
	 /* Update the table changing old name by new name */
	 Hld_DB_ChangeName (Hld_EditingHld->HldCod,NewHldName);
	 Str_Copy (Hld_EditingHld->Name,NewHldName,
		   sizeof (Hld_EditingHld->Name) - 1);

	 /***** Write message to show the change made *****/
         Ale_CreateAlert (Ale_SUCCESS,NULL,
                          Txt_The_holiday_X_has_been_renamed_as_Y,
		          Hld_EditingHld->Name,NewHldName);
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_X_has_not_changed,Hld_EditingHld->Name);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
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

static void Hld_PutFormToCreateHoliday (const struct Plc_Places *Places)
  {
   extern const char *Txt_All_places;
   extern const char *Txt_HOLIDAY_TYPES[Hld_NUM_TYPES_HOLIDAY];
   unsigned NumPlc;
   const struct Plc_Place *Plc;
   Hld_HolidayType_t HolidayType;
   unsigned HolidayTypeUnsigned;
   unsigned CurrentYear = Dat_GetCurrentYear ();

   /***** Begin form to create *****/
   Frm_BeginFormTable (ActNewHld,NULL,NULL,NULL,"TBL_SCROLL");

      /***** Write heading *****/
      Hld_PutHeadHolidays ();

      HTM_TR_Begin (NULL);

	 /***** Column to remove holiday, disabled here *****/
	 HTM_TD_Begin ("class=\"BM\"");
	 HTM_TD_End ();

	 /***** Holiday code *****/
	 HTM_TD_Begin ("class=\"CODE\"");
	 HTM_TD_End ();

	 /***** Holiday place *****/
	 HTM_TD_Begin ("class=\"CM\"");
	    HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
			      "name=\"PlcCod\" class=\"PLC_SEL INPUT_%s\"",
			      The_GetSuffix ());
	       HTM_OPTION (HTM_Type_STRING,"-1",
			   (Hld_EditingHld->PlcCod <= 0) ? HTM_SELECTED :
							   HTM_NO_ATTR,
			   "%s",Txt_All_places);
	       for (NumPlc = 0;
		    NumPlc < Places->Num;
		    NumPlc++)
		 {
		  Plc = &Places->Lst[NumPlc];
		  HTM_OPTION (HTM_Type_LONG,&Plc->PlcCod,
			      (Plc->PlcCod == Hld_EditingHld->PlcCod) ? HTM_SELECTED :
								        HTM_NO_ATTR,
			      "%s",Plc->ShrtName);
		 }
	    HTM_SELECT_End ();
	 HTM_TD_End ();

	 /***** Holiday type *****/
	 HTM_TD_Begin ("class=\"CM\"");
	    HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
			      "name=\"HldTyp\" class=\"INPUT_%s\""
			      " style=\"width:62px;\"",	// TODO: Use a CSS class
			      The_GetSuffix ());
	       for (HolidayType  = (Hld_HolidayType_t) 0;
		    HolidayType <= (Hld_HolidayType_t) (Hld_NUM_TYPES_HOLIDAY - 1);
		    HolidayType++)
		 {
		  HolidayTypeUnsigned = (unsigned) HolidayType;
		  HTM_OPTION (HTM_Type_UNSIGNED,&HolidayTypeUnsigned,
			      (HolidayType == Hld_EditingHld->HldTyp) ? HTM_SELECTED :
								        HTM_NO_ATTR,
			      "%s",Txt_HOLIDAY_TYPES[HolidayType]);
		 }
	    HTM_SELECT_End ();
	 HTM_TD_End ();

	 /***** Holiday date / Non school period start date *****/
	 HTM_TD_Begin ("class=\"CM\"");
	    Dat_WriteFormDate (CurrentYear - 1,
			       CurrentYear + 1,
			       "Start",
			       &Hld_EditingHld->StartDate,
			       HTM_NO_ATTR);
	 HTM_TD_End ();

	 /***** Non school period end date *****/
	 HTM_TD_Begin ("class=\"CM\"");
	    Dat_WriteFormDate (CurrentYear - 1,
			       CurrentYear + 1,
			       "End",
			       &Hld_EditingHld->EndDate,
			       HTM_NO_ATTR);
	 HTM_TD_End ();

	 /***** Holiday name *****/
	 HTM_TD_Begin ("class=\"CM\"");
	    HTM_INPUT_TEXT ("Name",Hld_MAX_CHARS_HOLIDAY_NAME,Hld_EditingHld->Name,
			    HTM_REQUIRED,
			    "size=\"20\" class=\"INPUT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

	 HTM_TD_Empty (1);

      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE_BUTTON);
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
      HTM_TH_Span (NULL                       ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH (Txt_Code                        ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Place                       ,HTM_HEAD_CENTER);
      HTM_TH (Txt_Type                        ,HTM_HEAD_CENTER);
      HTM_TH (Txt_START_END_TIME[Dat_STR_TIME],HTM_HEAD_CENTER);
      HTM_TH (Txt_START_END_TIME[Dat_END_TIME],HTM_HEAD_CENTER);
      HTM_TH (Txt_Holiday                     ,HTM_HEAD_CENTER);
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new holiday ********************/
/*****************************************************************************/

void Hld_ReceiveNewHoliday (void)
  {
   extern const char *Txt_Created_new_holiday_X;
   extern const char *Txt_You_must_specify_the_name;

   /***** Holiday constructor *****/
   Hld_EditingHolidayConstructor ();

   /***** Get place code *****/
   // <0 (place unspecified/all places) and 0 (another place) are allowed here
   Hld_EditingHld->PlcCod = ParCod_GetPar (ParCod_Plc);

   /***** Get the type of holiday *****/
   Hld_EditingHld->HldTyp = Hld_GetParHldType ();

   /***** Get start date *****/
   Dat_GetDateFromForm ("StartDay","StartMonth","StartYear",
                        &Hld_EditingHld->StartDate.Day,
			&Hld_EditingHld->StartDate.Month,
			&Hld_EditingHld->StartDate.Year);
   if (Hld_EditingHld->StartDate.Day   == 0 ||
       Hld_EditingHld->StartDate.Month == 0 ||
       Hld_EditingHld->StartDate.Year  == 0)
      Dat_AssignDate (&Hld_EditingHld->StartDate,Dat_GetCurrentDate ());

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
            Dat_AssignDate (&Hld_EditingHld->EndDate,Dat_GetCurrentDate ());
         break;
     }

   /***** Get holiday name *****/
   Par_GetParText ("Name",Hld_EditingHld->Name,Hld_MAX_BYTES_HOLIDAY_NAME);

   /***** Create the new holiday or set warning message *****/
   if (Hld_EditingHld->Name[0])	// If there's a holiday name
     {
      /* Create the new holiday */
      Hld_DB_CreateHoliday (Hld_EditingHld);

      /* Success message */
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Created_new_holiday_X,
		       Hld_EditingHld->Name);
     }
   else	// If there is not a holiday name
      /* Error message */
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_name);
  }

/*****************************************************************************/
/************************* Holiday constructor/destructor ********************/
/*****************************************************************************/

static void Hld_EditingHolidayConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Hld_EditingHld != NULL)
      Err_WrongHolidayExit ();

   /***** Allocate memory for holiday *****/
   if ((Hld_EditingHld = malloc (sizeof (*Hld_EditingHld))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Reset place *****/
   Hld_EditingHld->HldCod = -1L;
   Hld_EditingHld->PlcCod = -1L;
   Hld_EditingHld->PlaceFullName[0] = '\0';
   Hld_EditingHld->HldTyp = Hld_HOLIDAY;
   Hld_EditingHld->StartDate.Year  = Hld_EditingHld->EndDate.Year  = Dat_GetCurrentYear ();
   Hld_EditingHld->StartDate.Month = Hld_EditingHld->EndDate.Month = Dat_GetCurrentMonth ();
   Hld_EditingHld->StartDate.Day   = Hld_EditingHld->EndDate.Day   = Dat_GetCurrentDay ();
   Hld_EditingHld->Name[0] = '\0';
  }

static void Hld_EditingHolidayDestructor (void)
  {
   /***** Free memory used for holiday *****/
   if (Hld_EditingHld != NULL)
     {
      free (Hld_EditingHld);
      Hld_EditingHld = NULL;
     }
  }
