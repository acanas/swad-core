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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <string.h>		// For string functions

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
   bool ICanAnswer;		// Exam print is open and accesible to answer by the user
  } ExaLog_Log =
  {
   .PrnCod     = -1L,	// -1 means no print code set
   .QstInd     = -1,	// -1 means no question index set
   .Action     = ExaLog_UNKNOWN_ACTION,
   .ICanAnswer = false,
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
/******************** Set and get current exam print code ********************/
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
/************* Set and get current question index in exam print **************/
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
/************* Set and get if exam print is open and accessible **************/
/*****************************************************************************/

void ExaLog_SetIfCanAnswer (bool ICanAnswer)
  {
   ExaLog_Log.ICanAnswer = ICanAnswer;
  }

bool ExaLog_GetIfCanAnswer (void)
  {
   return ExaLog_Log.ICanAnswer;
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
			 "(LogCod,PrnCod,ActCod,QstInd,CanAnswer,ClickTime,IP,SessionId)"
			 " VALUES "
			 "(%ld,%ld,%ld,%d,'%c',NOW(),'%s','%s')",
			 LogCod,
			 PrnCod,
			 (unsigned) Action,
			 ExaLog_GetQstInd (),
			 ExaLog_GetIfCanAnswer () ? 'Y' :
				                    'N',
			 // NOW()   	   Redundant, for speed
			 Gbl.IP,	// Redundant, for speed
			 Gbl.Session.Id);
     }
  }

/*****************************************************************************/
/****************************** Show exam log ********************************/
/*****************************************************************************/

void ExaLog_ShowExamLog (const struct ExaPrn_Print *Print)
  {
   extern const char *Txt_Hits;
   extern const char *Txt_Date_and_time;
   extern const char *Txt_Action;
   extern const char *Txt_Question;
   extern const char *Txt_EXAM_Open;
   extern const char *Txt_IP;
   extern const char *Txt_Session;
   extern const char *Txt_EXAM_LOG_ACTIONS[ExaLog_NUM_ACTIONS];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumClicks;
   unsigned NumClick;
   unsigned ActCod;
   int QstInd;
   bool UsrCouldAnswer;
   time_t ClickTimeUTC;
   char IP[Cns_MAX_BYTES_IP + 1];
   char SessionId[Cns_BYTES_SESSION_ID + 1];
   char *Id;
   size_t Length;
   char Anonymized[14 + 1];	// XXX&hellip;XXX
				// 12345678901234

   /***** Check if I can view this print result *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 break;
      default:	// Other users can not see log
	 return;
     }

   /***** Get print log from database *****/
   NumClicks = (unsigned)
	       DB_QuerySELECT (&mysql_res,"can not get exam print log",
			       "SELECT ActCod,"				// row[0]
			              "QstInd,"				// row[1]
			              "CanAnswer,"			// row[2]
			              "UNIX_TIMESTAMP(ClickTime),"	// row[3]
			              "IP,"				// row[4]
			              "SessionId"			// row[5]
			       " FROM exa_log"
			       " WHERE PrnCod=%ld"
			       " ORDER BY LogCod",
			       Print->PrnCod);

   if (NumClicks)
     {
      /***** Begin box *****/
      Box_BoxTableBegin (NULL,Txt_Hits,
                         NULL,NULL,
                         NULL,Box_CLOSABLE,2);

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (2);

      /***** Write heading *****/
      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"LB",Txt_Date_and_time);
      HTM_TH (1,1,"LB",Txt_Action);
      HTM_TH (1,1,"RB",Txt_Question);
      HTM_TH (1,1,"CB",Txt_EXAM_Open);
      HTM_TH (1,1,"LB",Txt_IP);
      HTM_TH (1,1,"LB",Txt_Session);

      HTM_TR_End ();

      /***** Write clicks *****/
      for (NumClick = 0;
	   NumClick < NumClicks;
	   NumClick++)
	{
	 /***** Get row *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get code of action (row[0]) */
	 ActCod = Str_ConvertStrToUnsigned (row[0]);
	 if (ActCod >= ExaLog_NUM_ACTIONS)
	    ActCod = ExaLog_UNKNOWN_ACTION;

	 /* Get question index (row[1]) */
	 QstInd = (int) Str_ConvertStrCodToLongCod (row[1]);

	 /* Get if the user could answer (row[2]) */
	 UsrCouldAnswer = (row[2][0] == 'Y');

	 /* Get click time (row[3] holds the UTC time) */
	 ClickTimeUTC = Dat_GetUNIXTimeFromStr (row[3]);

	 /* Get IP (row[4]) */
	 Str_Copy (IP,row[4],
		   Cns_MAX_BYTES_IP);

	 /* Get session id (row[5]) */
	 Str_Copy (SessionId,row[5],
		   Cns_BYTES_SESSION_ID);

	 /***** Write row *****/
	 HTM_TR_Begin (NULL);

	 /* Write click time */
	 if (asprintf (&Id,"click_date_%u",NumClick) < 0)
	    Lay_NotEnoughMemoryExit ();
	 HTM_TD_Begin ("id=\"%s\" class=\"LM DAT\"",Id);
	 Dat_WriteLocalDateHMSFromUTC (Id,ClickTimeUTC,
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				       true,true,true,0x7);
	 free (Id);
	 HTM_TD_End ();

	 /* Write action */
	 HTM_TD_Begin ("class=\"LM DAT\"");
	 HTM_Txt (Txt_EXAM_LOG_ACTIONS[ActCod]);
	 HTM_TD_End ();

	 /* Write number of question */
	 HTM_TD_Begin ("class=\"RM DAT\"");
	 if (QstInd >= 0)
	    HTM_Unsigned ((unsigned) QstInd + 1);
	 HTM_TD_End ();

	 /* Write if exam print was open and accesible to answer */
	 HTM_TD_Begin ("class=\"CM %s\"",UsrCouldAnswer ? "DAT_GREEN" :
		                                          "DAT_RED");
	 HTM_Txt (UsrCouldAnswer ? "&check;" :
				   "&cross;");
	 HTM_TD_End ();

	 /* Write IP */
	 HTM_TD_Begin ("class=\"CM DAT\"");
	 Length = strlen (IP);
	 if (Length > 6)
	   {
	    sprintf (Anonymized,"%c%c%c&hellip;%c%c%c",
		     IP[0],
		     IP[1],
		     IP[2],
		     IP[Length - 3],
		     IP[Length - 2],
		     IP[Length - 1]);
	    HTM_Txt (Anonymized);
	   }
	 else
	    HTM_Txt ("&hellip;");
	 HTM_TD_End ();

	 /* Write session id */
	 HTM_TD_Begin ("class=\"CM DAT\"");
	 Length = strlen (SessionId);
	 Length = strlen (IP);
	 if (Length > 6)
	   {
	    sprintf (Anonymized,"%c%c%c&hellip;%c%c%c",
		     SessionId[0],
		     SessionId[1],
		     SessionId[2],
		     SessionId[Length - 3],
		     SessionId[Length - 2],
		     SessionId[Length - 1]);
	    HTM_Txt (Anonymized);
	   }
	 else
	    HTM_Txt ("&hellip;");
	 HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
