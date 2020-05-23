// swad_exam_log.c: exam access log

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

#include "swad_action.h"
#include "swad_database.h"
#include "swad_exam_log.h"
#include "swad_global.h"

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

static struct
  {
   long PrnCod;			// Exam print code
   int  QstInd;			// Exam print question index
   ExaLog_Action_t Action;	// Action performed by user
   bool Open;			// Exam print is open and accesible to answer by the user
  } ExaLog_Log =
  {
   .PrnCod        = -1L,	// -1 means no print code set
   .QstInd        = -1,		// -1 means no question index set
   .Action        = ExaLog_UNKNOWN_ACTION,
   .Open          = false,
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/************* Set and get current exam print code (used in log) *************/
/*****************************************************************************/

void ExaLog_SetAction (ExaLog_Action_t Action)
  {
   ExaLog_Log.Action = Action;
  }

ExaLog_Action_t ExaLog_GetAction (void)
  {
   return ExaLog_Log.Action;
  }

/*****************************************************************************/
/************* Set and get current exam print code (used in log) *************/
/*****************************************************************************/

void ExaLog_SetPrnCod (long PrnCod)
  {
   ExaLog_Log.PrnCod = PrnCod;
  }

long ExaLog_GetPrnCod (void)
  {
   return ExaLog_Log.PrnCod;
  }

/*****************************************************************************/
/****** Set and get current question index in exam print (used in log) *******/
/*****************************************************************************/

void ExaLog_SetQstInd (unsigned QstInd)
  {
   ExaLog_Log.QstInd = (int) QstInd;
  }

int ExaLog_GetQstInd (void)
  {
   return ExaLog_Log.QstInd;
  }

/*****************************************************************************/
/******** Set and get if answer is saved in exam print (used in log) *********/
/*****************************************************************************/

void ExaLog_SetOpen (bool Open)
  {
   ExaLog_Log.Open = Open;
  }

bool ExaLog_GetOpen (void)
  {
   return ExaLog_Log.Open;
  }

/*****************************************************************************/
/**************************** Log access in database *************************/
/*****************************************************************************/

void ExaLog_LogAccess (long LogCod)
  {
   ExaLog_Action_t Action;
   long PrnCod;

   Action = ExaLog_GetAction ();
   if (Action != ExaLog_UNKNOWN_ACTION)
     {
      PrnCod = ExaLog_GetPrnCod ();

      if (PrnCod > 0)	// Only if exam print is accesible (visible, open...)
	 /***** Insert access into database *****/
	 /* Log access in exam log.
	    Redundant data (also present in log table) are stored for speed */
	 DB_QueryINSERT ("can not log exam access",
			 "INSERT INTO exa_log "
			 "(LogCod,PrnCod,ActCod,QstInd,Open,ClickTime,IP,SessionId)"
			 " VALUES "
			 "(%ld,%ld,%ld,%d,'%c',NOW(),'%s','%s')",
			 LogCod,
			 PrnCod,
			 (unsigned) Action,
			 ExaLog_GetQstInd (),
			 ExaLog_GetOpen () ? 'Y' :
				             'N',
			 // NOW()   	   Redundant, for speed
			 Gbl.IP,	// Redundant, for speed
			 Gbl.Session.Id);
     }
  }
