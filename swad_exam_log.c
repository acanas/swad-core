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
   long PrnCod;
   int  QstInd;
   bool AnswerIsSaved;
  } ExaLog_CurrentPrint =
  {
   .PrnCod        = -1L,	// -1 means no print code set
   .QstInd        = -1,		// -1 means no question index set
   .AnswerIsSaved = false,	// By default, answer is not saved
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/************* Set and get current exam print code (used in log) *************/
/*****************************************************************************/

void ExaLog_SetCurrentPrnCod (long PrnCod)
  {
   ExaLog_CurrentPrint.PrnCod = PrnCod;
  }

long ExaLog_GetCurrentPrnCod (void)
  {
   return ExaLog_CurrentPrint.PrnCod;
  }

/*****************************************************************************/
/****** Set and get current question index in exam print (used in log) *******/
/*****************************************************************************/

void ExaLog_SetCurrentQstInd (unsigned QstInd)
  {
   ExaLog_CurrentPrint.QstInd = (int) QstInd;
  }

int  ExaLog_GetCurrentQstInd (void)
  {
   return ExaLog_CurrentPrint.QstInd;
  }

/*****************************************************************************/
/******** Set and get if answer is saved in exam print (used in log) *********/
/*****************************************************************************/

void ExaLog_SetAnswerIsSaved (void)
  {
   ExaLog_CurrentPrint.AnswerIsSaved = true;
  }

bool ExaLog_GetAnswerIsSaved (void)
  {
   return ExaLog_CurrentPrint.AnswerIsSaved;
  }

/*****************************************************************************/
/**************************** Log access in database *************************/
/*****************************************************************************/

void ExaLog_LogAccess (long LogCod)
  {
   long PrnCod;

   /* WARNING: Don't change the codes os the actions.
               If the codes of the actions change ==> change them in exam log table */
   if (Gbl.Action.Act == ActAnsExaPrn ||	// Answer question
       Gbl.Action.Act == ActSeeExaPrn ||	// Create/resume print exam
       Gbl.Action.Act == ActEndExaPrn)		// End print exam
     {
      PrnCod = ExaLog_GetCurrentPrnCod ();

      if (PrnCod > 0)	// Only if exam print is accesible (visible, open...)
	 /***** Insert access into database *****/
	 /* Log access in exam log.
	    Redundant data (also present in log table) are stored for speed */
	 DB_QueryINSERT ("can not log exam access",
			 "INSERT INTO exa_log "
			 "(LogCod,PrnCod,ActCod,QstInd,Saved,ClickTime,IP,SessionId)"
			 " VALUES "
			 "(%ld,%ld,%ld,%d,'%c',NOW(),'%s','%s')",
			 LogCod,
			 PrnCod,
			 Act_GetActCod (Gbl.Action.Act),	// Redundant, for speed
			 ExaLog_GetCurrentQstInd (),
			 ExaLog_GetAnswerIsSaved () ? 'Y' :
				                      'N',
			 // NOW()			  	   Redundant, for speed
			 Gbl.IP,				// Redundant, for speed
			 Gbl.Session.Id);
     }
  }
