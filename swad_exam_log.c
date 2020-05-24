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
#include <stdlib.h>		// For system, getenv, etc.
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

static void ExaLog_LogSession (long LogCod,long PrnCod);
static void ExaLog_LogUsrAgent (long LogCod,long PrnCod);

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
	{
	 /***** Insert access into database *****/
	 /* Log access in exam log.
	    Redundant data (also present in log table) are stored for speed */
	 DB_QueryINSERT ("can not log exam access",
			 "INSERT INTO exa_log "
			 "(LogCod,PrnCod,ActCod,QstInd,CanAnswer,ClickTime,IP)"
			 " VALUES "
			 "(%ld,%ld,%ld,%d,'%c',NOW(),'%s')",
			 LogCod,
			 PrnCod,
			 (unsigned) Action,
			 ExaLog_GetQstInd (),
			 ExaLog_GetIfCanAnswer () ? 'Y' :
				                    'N',
			 // NOW()   	   Redundant, for speed
			 Gbl.IP);	// Redundant, for speed

	 /***** Log session and user agent *****/
	 ExaLog_LogSession (LogCod,PrnCod);
	 ExaLog_LogUsrAgent (LogCod,PrnCod);
	}
     }
  }

/*****************************************************************************/
/*************************** Log session in database *************************/
/*****************************************************************************/

static void ExaLog_LogSession (long LogCod,long PrnCod)
  {
   bool TheSameAsTheLast;

   /***** Get if the current session id
          is the same as the last stored in database *****/
   TheSameAsTheLast = (DB_QueryCOUNT ("can not check session",
				      "SELECT COUNT(*)"
				      " FROM exa_log_session"
				      " WHERE LogCod="
				      "(SELECT MAX(LogCod)"
				      " FROM exa_log_session"
				      " WHERE PrnCod=%ld)"
				      " AND SessionId='%s'",
				      PrnCod,
				      Gbl.Session.Id) != 0);


   /***** Insert session id into database
          only if it's not the same as the last one stored *****/
   if (!TheSameAsTheLast)
      DB_QueryINSERT ("can not log session",
		      "INSERT INTO exa_log_session "
		      "(LogCod,PrnCod,SessionId)"
		      " VALUES "
		      "(%ld,%ld,'%s')",
		      LogCod,
		      PrnCod,
		      Gbl.Session.Id);
  }

/*****************************************************************************/
/************************** Log user agent in database ***********************/
/*****************************************************************************/
/* Examples of HTTP_USER_AGENT:
Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0
Mozilla/5.0 (Macintosh; Intel Mac OS X x.y; rv:42.0) Gecko/20100101 Firefox/42.0
Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36
Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_1 like Mac OS X) AppleWebKit/603.1.30 (KHTML, like Gecko) Version/10.0 Mobile/14E304 Safari/602.1
Mozilla/5.0 (compatible; MSIE 9.0; Windows Phone OS 7.5; Trident/5.0; IEMobile/9.0)
Googlebot/2.1 (+http://www.google.com/bot.html)
*/

static void ExaLog_LogUsrAgent (long LogCod,long PrnCod)
  {
   bool TheSameAsTheLast;
   const char *UserAgent;
   char *UserAgentDB;
   size_t MaxBytes;

   /***** Get current user agent *****/
   UserAgent = getenv ("HTTP_USER_AGENT");
   if (UserAgent)
      MaxBytes = strlen (UserAgent) * Str_MAX_BYTES_PER_CHAR;
   else
      MaxBytes = 0;
   if ((UserAgentDB = (char *) malloc (MaxBytes + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();
   if (UserAgent)
     {
      Str_Copy (UserAgentDB,UserAgent,
                MaxBytes);
      Str_ChangeFormat (Str_FROM_TEXT,Str_TO_TEXT,
			UserAgentDB,MaxBytes,true);
     }
   else
      UserAgentDB[0] = '\0';

   /***** Get if the current user agent
          is the same as the last stored in database *****/
   TheSameAsTheLast = (DB_QueryCOUNT ("can not check user agent",
				      "SELECT COUNT(*)"
				      " FROM exa_log_user_agent"
				      " WHERE LogCod="
				      "(SELECT MAX(LogCod)"
				      " FROM exa_log_user_agent"
				      " WHERE PrnCod=%ld)"
				      " AND UserAgent='%s'",
				      PrnCod,
				      UserAgentDB) != 0);


   /***** Insert user agent into database
          only if it's not the same as the last one stored *****/
   if (!TheSameAsTheLast)
      DB_QueryINSERT ("can not log user agent",
		      "INSERT INTO exa_log_user_agent "
		      "(LogCod,PrnCod,UserAgent)"
		      " VALUES "
		      "(%ld,%ld,'%s')",
		      LogCod,
		      PrnCod,
		      UserAgentDB);

   /***** Free user agent *****/
   free (UserAgentDB);
  }

/*****************************************************************************/
/****************************** Show exam log ********************************/
/*****************************************************************************/

void ExaLog_ShowExamLog (const struct ExaPrn_Print *Print)
  {
   extern const char *Txt_Hits;
   extern const char *Txt_Click;
   extern const char *Txt_Date_and_time;
   extern const char *Txt_Action;
   extern const char *Txt_Question;
   extern const char *Txt_EXAM_Open;
   extern const char *Txt_IP;
   extern const char *Txt_Session;
   extern const char *Txt_Web_browser;
   extern const char *Txt_EXAM_LOG_ACTIONS[ExaLog_NUM_ACTIONS];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumClicks;
   unsigned NumClick;
   unsigned ActCod;
   ExaLog_Action_t Action;
   int QstInd;
   bool UsrCouldAnswer;
   time_t ClickTimeUTC;
   char IP[Cns_MAX_BYTES_IP + 1];
   char *Id;
   size_t Length;
   char Anonymized[14 + 1];	// ***&hellip;***
				// 12345678901234
   char SessionId[Cns_BYTES_SESSION_ID + 1];
   char *UserAgent;
   const char *Class;

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
			       "SELECT exa_log.ActCod,"				// row[0]
			              "exa_log.QstInd,"				// row[1]
			              "exa_log.CanAnswer,"			// row[2]
			              "UNIX_TIMESTAMP(exa_log.ClickTime),"	// row[3]
			              "exa_log.IP,"				// row[4]
			              "exa_log_session.SessionId,"		// row[5]
				      "exa_log_user_agent.UserAgent"		// row[6]
			       " FROM exa_log LEFT JOIN exa_log_session"
	                       " ON exa_log.LogCod=exa_log_session.LogCod"
			       " LEFT JOIN exa_log_user_agent"
	                       " ON exa_log.LogCod=exa_log_user_agent.LogCod"
			       " WHERE exa_log.PrnCod=%ld"
			       " ORDER BY exa_log.LogCod",
			       Print->PrnCod);

   if (NumClicks)
     {
      /***** Initialize last session id and last user agent ******/
      SessionId[0] = '\0';
      UserAgent = NULL;

      /***** Begin box *****/
      Box_BoxTableBegin (NULL,Txt_Hits,
                         NULL,NULL,
                         NULL,Box_CLOSABLE,2);

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (2);

      /***** Write heading *****/
      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"RB",Txt_Click);
      HTM_TH (1,1,"LB",Txt_Date_and_time);
      HTM_TH (1,1,"LB",Txt_Action);
      HTM_TH (1,1,"RB",Txt_Question);
      HTM_TH (1,1,"CB",Txt_EXAM_Open);
      HTM_TH (1,1,"LB",Txt_IP);
      HTM_TH (1,1,"LB",Txt_Session);
      HTM_TH (1,1,"LB",Txt_Web_browser);

      HTM_TR_End ();

      /***** Write clicks *****/
      for (NumClick = 0;
	   NumClick < NumClicks;
	   NumClick++)
	{
	 Gbl.RowEvenOdd = NumClick % 2;

	 /***** Get row *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get code of action (row[0]) */
	 ActCod = Str_ConvertStrToUnsigned (row[0]);
	 if (ActCod < ExaLog_NUM_ACTIONS)
	    Action = (ExaLog_Action_t) ActCod;
	 else
	    Action = ExaLog_UNKNOWN_ACTION;

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
	 if (row[5])	// This row has a user agent stored in database
	    Str_Copy (SessionId,row[5],
		      Cns_BYTES_SESSION_ID);
	 else
	    Str_Copy (SessionId,"=",
		      Cns_BYTES_SESSION_ID);

	 /* Get session id (row[6]) */
	 if (asprintf (&UserAgent,"%s",row[6] ? row[6] :
						"=") < 0)
	    Lay_NotEnoughMemoryExit ();

	 /***** Set color of row depending on action *****/
	 if (UsrCouldAnswer)
	    switch (Action)
	      {
	       case ExaLog_START_EXAM:
	       case ExaLog_RESUME_EXAM:
	       case ExaLog_FINISH_EXAM:
	          Class = "DAT_SMALL_N";
	          break;
	       case ExaLog_ANSWER_QUESTION:
	          Class = "DAT_SMALL";
		  break;
	       default:
	          Class = "DAT_SMALL_LIGHT";
	          break;
	      }
	 else	// Closed or not accesible exam print
	    Class = "DAT_SMALL_LIGHT";

	 /***** Write row *****/
	 HTM_TR_Begin (NULL);

	 /* Write number of click */
	 HTM_TD_Begin ("class=\"RT COLOR%u %s\"",
	               Gbl.RowEvenOdd,UsrCouldAnswer ? "DAT_SMALL" :
	        		                       "DAT_SMALL_LIGHT");
	 HTM_Unsigned (NumClick + 1);
	 HTM_TD_End ();

	 /* Write click time */
	 if (asprintf (&Id,"click_date_%u",NumClick) < 0)
	    Lay_NotEnoughMemoryExit ();
	 HTM_TD_Begin ("id=\"%s\" class=\"LT COLOR%u %s\"",Id,Gbl.RowEvenOdd,Class);
	 Dat_WriteLocalDateHMSFromUTC (Id,ClickTimeUTC,
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				       true,true,true,0x7);
	 free (Id);
	 HTM_TD_End ();

	 /* Write action */
	 HTM_TD_Begin ("class=\"LT COLOR%u %s\"",Gbl.RowEvenOdd,Class);
	 HTM_Txt (Txt_EXAM_LOG_ACTIONS[Action]);
	 HTM_TD_End ();

	 /* Write number of question */
	 HTM_TD_Begin ("class=\"RT COLOR%u %s\"",Gbl.RowEvenOdd,Class);
	 if (QstInd >= 0)
	    HTM_Unsigned ((unsigned) QstInd + 1);
	 HTM_TD_End ();

	 /* Write if exam print was open and accesible to answer */
	 HTM_TD_Begin ("class=\"CT COLOR%u %s\"",
	               Gbl.RowEvenOdd,UsrCouldAnswer ? "DAT_SMALL_GREEN" :
		                                       "DAT_SMALL_RED");
	 HTM_Txt (UsrCouldAnswer ? "&check;" :
				   "&cross;");
	 HTM_TD_End ();

	 /* Write IP */
	 HTM_TD_Begin ("class=\"LT COLOR%u %s\"",Gbl.RowEvenOdd,Class);
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
	    HTM_Txt (IP);
	 HTM_TD_End ();

	 /* Write session id */
	 HTM_TD_Begin ("class=\"LT COLOR%u %s\"",Gbl.RowEvenOdd,Class);
	 if (SessionId[0])
	   {
	    Length = strlen (SessionId);
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
	       HTM_Txt (SessionId);
	   }
	 HTM_TD_End ();

	 /* Write user agent (row[6]) */
	 HTM_TD_Begin ("class=\"LT COLOR%u %s\"",Gbl.RowEvenOdd,Class);
	 if (UserAgent[0])
	    HTM_Txt (UserAgent);
	 HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Free user agent *****/
	 free (UserAgent);
	}

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
