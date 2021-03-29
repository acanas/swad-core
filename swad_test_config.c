// swad_test_config.c: self-assessment tests configuration

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_global.h"
#include "swad_test.h"
#include "swad_test_config.h"
#include "swad_test_visibility.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

const char *TstCfg_PluggableDB[TstCfg_NUM_OPTIONS_PLUGGABLE] =
  {
   [TstCfg_PLUGGABLE_UNKNOWN] = "unknown",
   [TstCfg_PLUGGABLE_NO     ] = "N",
   [TstCfg_PLUGGABLE_YES    ] = "Y",
  };

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

struct TstCfg_Config
  {
   TstCfg_Pluggable_t Pluggable;
   unsigned Min;	// Minimum number of questions
   unsigned Def;	// Default number of questions
   unsigned Max;	// Maximum number of questions
   unsigned long MinTimeNxtTstPerQst;
   unsigned Visibility;	// One bit for each visibility item
  };

struct TstCfg_Config TstCfg_Config;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static TstCfg_Pluggable_t TstCfg_GetPluggableFromForm (void);
static void TstCfg_CheckAndCorrectMinDefMax (void);

/*****************************************************************************/
/*************** Get configuration of test for current course ****************/
/*****************************************************************************/

void TstCfg_GetConfigFromDB (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get configuration of test for current course from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get configuration of test",
			     "SELECT Pluggable,"		// row[0]
				    "Min,"			// row[1]
				    "Def,"			// row[2]
				    "Max,"			// row[3]
				    "MinTimeNxtTstPerQst,"	// row[4]
				    "Visibility"		// row[5]
			      " FROM tst_config"
			     " WHERE CrsCod=%ld",
			     Gbl.Hierarchy.Crs.CrsCod);

   TstCfg_SetConfigMinTimeNxtTstPerQst (0UL);
   TstCfg_SetConfigVisibility (TstVis_VISIBILITY_DEFAULT);
   if (NumRows == 0)
     {
      TstCfg_SetConfigPluggable (TstCfg_PLUGGABLE_UNKNOWN);
      TstCfg_SetConfigMin (TstCfg_DEFAULT_MIN_QUESTIONS);
      TstCfg_SetConfigDef (TstCfg_DEFAULT_DEF_QUESTIONS);
      TstCfg_SetConfigMax (TstCfg_DEFAULT_MAX_QUESTIONS);
     }
   else // NumRows == 1
     {
      /***** Get minimun, default and maximum *****/
      row = mysql_fetch_row (mysql_res);
      TstCfg_GetConfigFromRow (row);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Get configuration values from a database table row *************/
/*****************************************************************************/

void TstCfg_GetConfigFromRow (MYSQL_ROW row)
  {
   int IntNum;
   long LongNum;
   TstCfg_Pluggable_t Pluggable;

   /***** Get whether test are visible via plugins or not *****/
   TstCfg_SetConfigPluggable (TstCfg_PLUGGABLE_UNKNOWN);
   for (Pluggable  = TstCfg_PLUGGABLE_NO;
	Pluggable <= TstCfg_PLUGGABLE_YES;
	Pluggable++)
      if (!strcmp (row[0],TstCfg_PluggableDB[Pluggable]))
        {
         TstCfg_SetConfigPluggable (Pluggable);
         break;
        }

   /***** Get number of questions *****/
   if (sscanf (row[1],"%d",&IntNum) == 1)
      TstCfg_SetConfigMin ((IntNum < 1) ? 1 :
	                               (unsigned) IntNum);
   else
      TstCfg_SetConfigMin (TstCfg_DEFAULT_MIN_QUESTIONS);

   if (sscanf (row[2],"%d",&IntNum) == 1)
      TstCfg_SetConfigDef ((IntNum < 1) ? 1 :
	                               (unsigned) IntNum);
   else
      TstCfg_SetConfigDef (TstCfg_DEFAULT_DEF_QUESTIONS);

   if (sscanf (row[3],"%d",&IntNum) == 1)
      TstCfg_SetConfigMax ((IntNum < 1) ? 1 :
	                               (unsigned) IntNum);
   else
      TstCfg_SetConfigMax (TstCfg_DEFAULT_MAX_QUESTIONS);

   /***** Check and correct numbers *****/
   TstCfg_CheckAndCorrectMinDefMax ();

   /***** Get minimum time between consecutive tests, per question (row[4]) *****/
   if (sscanf (row[4],"%ld",&LongNum) == 1)
      TstCfg_SetConfigMinTimeNxtTstPerQst ((LongNum < 1L) ? 0UL :
	                                                 (unsigned long) LongNum);
   else
      TstCfg_SetConfigMinTimeNxtTstPerQst (0UL);

   /***** Get visibility (row[5]) *****/
   TstCfg_SetConfigVisibility (TstVis_GetVisibilityFromStr (row[5]));
  }

/*****************************************************************************/
/************* Receive configuration of test for current course **************/
/*****************************************************************************/

void TstCfg_ReceiveConfigTst (void)
  {
   extern const char *Txt_The_test_configuration_has_been_updated;

   /***** Get whether test are visible via plugins or not *****/
   TstCfg_SetConfigPluggable (TstCfg_GetPluggableFromForm ());

   /***** Get number of questions *****/
   /* Get minimum number of questions */
   TstCfg_SetConfigMin ((unsigned)
	             Par_GetParToUnsignedLong ("NumQstMin",
	                                       1,
	                                       UINT_MAX,
	                                       1));

   /* Get default number of questions */
   TstCfg_SetConfigDef ((unsigned)
	             Par_GetParToUnsignedLong ("NumQstDef",
	                                       1,
	                                       UINT_MAX,
	                                       1));

   /* Get maximum number of questions */
   TstCfg_SetConfigMax ((unsigned)
	             Par_GetParToUnsignedLong ("NumQstMax",
	                                       1,
	                                       UINT_MAX,
	                                       1));

   /* Check and correct numbers */
   TstCfg_CheckAndCorrectMinDefMax ();

   /***** Get minimum time between consecutive tests, per question *****/
   TstCfg_SetConfigMinTimeNxtTstPerQst (Par_GetParToUnsignedLong ("MinTimeNxtTstPerQst",
                                                               0,
                                                               ULONG_MAX,
                                                               0));

   /***** Get visibility from form *****/
   TstCfg_SetConfigVisibility (TstVis_GetVisibilityFromForm ());

   /***** Update database *****/
   DB_QueryREPLACE ("can not save configuration of tests",
		    "REPLACE INTO tst_config"
	            " (CrsCod,Pluggable,Min,Def,Max,MinTimeNxtTstPerQst,Visibility)"
                    " VALUES"
                    " (%ld,'%s',%u,%u,%u,'%lu',%u)",
		    Gbl.Hierarchy.Crs.CrsCod,
		    TstCfg_PluggableDB[TstCfg_GetConfigPluggable ()],
		    TstCfg_GetConfigMin (),
		    TstCfg_GetConfigDef (),
		    TstCfg_GetConfigMax (),
		    TstCfg_GetConfigMinTimeNxtTstPerQst (),
		    TstCfg_GetConfigVisibility ());

   /***** Show confirmation message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_test_configuration_has_been_updated);

   /***** Show again the form to configure test *****/
   Tst_ShowFormConfig ();
  }

/*****************************************************************************/
/******************* Get if tests are pluggable from form ********************/
/*****************************************************************************/

static TstCfg_Pluggable_t TstCfg_GetPluggableFromForm (void)
  {
   return (TstCfg_Pluggable_t)
	  Par_GetParToUnsignedLong ("Pluggable",
	                            0,
                                    TstCfg_NUM_OPTIONS_PLUGGABLE - 1,
                                    (unsigned long) TstCfg_PLUGGABLE_UNKNOWN);
  }

/*****************************************************************************/
/**** Check and correct minimum, default and maximum numbers of questions ****/
/*****************************************************************************/

static void TstCfg_CheckAndCorrectMinDefMax (void)
  {
   /***** Check if minimum is correct *****/
   if (TstCfg_GetConfigMin () < 1)
      TstCfg_SetConfigMin (1);
   else if (TstCfg_GetConfigMin () > TstCfg_MAX_QUESTIONS_PER_TEST)
      TstCfg_SetConfigMin (TstCfg_MAX_QUESTIONS_PER_TEST);

   /***** Check if maximum is correct *****/
   if (TstCfg_GetConfigMax () < 1)
      TstCfg_SetConfigMax (1);
   else if (TstCfg_GetConfigMax () > TstCfg_MAX_QUESTIONS_PER_TEST)
      TstCfg_SetConfigMax (TstCfg_MAX_QUESTIONS_PER_TEST);

   /***** Check if minimum is lower than maximum *****/
   if (TstCfg_GetConfigMin () > TstCfg_GetConfigMax ())
      TstCfg_SetConfigMin (TstCfg_GetConfigMax ());

   /***** Check if default is correct *****/
   if (TstCfg_GetConfigDef () < TstCfg_GetConfigMin ())
      TstCfg_SetConfigDef (TstCfg_GetConfigMin ());
   else if (TstCfg_GetConfigDef () > TstCfg_GetConfigMax ())
      TstCfg_SetConfigDef (TstCfg_GetConfigMax ());
  }

/*****************************************************************************/
/********* Get fields of current test configuration for this course **********/
/*****************************************************************************/

void TstCfg_SetConfigPluggable (TstCfg_Pluggable_t Pluggable)
  {
   TstCfg_Config.Pluggable = Pluggable;
  }

TstCfg_Pluggable_t TstCfg_GetConfigPluggable (void)
  {
   return TstCfg_Config.Pluggable;
  }

void TstCfg_SetConfigMin (unsigned Min)
  {
   TstCfg_Config.Min = Min;
  }

unsigned TstCfg_GetConfigMin (void)
  {
   return TstCfg_Config.Min;
  }

void TstCfg_SetConfigDef (unsigned Def)
  {
   TstCfg_Config.Def = Def;
  }

unsigned TstCfg_GetConfigDef (void)
  {
   return TstCfg_Config.Def;
  }

void TstCfg_SetConfigMax (unsigned Max)
  {
   TstCfg_Config.Max = Max;
  }

unsigned TstCfg_GetConfigMax (void)
  {
   return TstCfg_Config.Max;
  }

void TstCfg_SetConfigMinTimeNxtTstPerQst (unsigned long MinTimeNxtTstPerQst)
  {
   TstCfg_Config.MinTimeNxtTstPerQst = MinTimeNxtTstPerQst;
  }

unsigned long TstCfg_GetConfigMinTimeNxtTstPerQst (void)
  {
   return TstCfg_Config.MinTimeNxtTstPerQst;
  }

void TstCfg_SetConfigVisibility (unsigned Visibility)
  {
   TstCfg_Config.Visibility = Visibility;
  }

unsigned TstCfg_GetConfigVisibility (void)
  {
   return TstCfg_Config.Visibility;
  }
