// swad_log.c: access log stored in database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <stdlib.h>		// For free
#include <string.h>		// For strlen

#include "swad_action.h"
#include "swad_banner.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_exam_log.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_HTML.h"
#include "swad_log.h"
#include "swad_profile.h"
#include "swad_role.h"
#include "swad_statistic.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Log_SECONDS_IN_RECENT_LOG ((time_t) (Cfg_DAYS_IN_RECENT_LOG * 24UL * 60UL * 60UL))	// Remove entries in recent log oldest than this time

/*****************************************************************************/
/****************************** Private types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Log access in database *************************/
/*****************************************************************************/

void Log_LogAccess (const char *Comments)
  {
   long LogCod;
   long ActCod = Act_GetActCod (Gbl.Action.Act);
   size_t MaxLength;
   char *CommentsDB;
   long BanCodClicked;
   Rol_Role_t RoleToStore = (Gbl.Action.Act == ActLogOut) ? Gbl.Usrs.Me.Role.LoggedBeforeCloseSession :
                                                            Gbl.Usrs.Me.Role.Logged;

   /***** Insert access into database *****/
   /* Log access in historical log */
   LogCod =
   DB_QueryINSERTandReturnCode ("can not log access",
				"INSERT INTO log "
				"(ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,"
				"Role,ClickTime,TimeToGenerate,TimeToSend,IP)"
				" VALUES "
				"(%ld,%ld,%ld,%ld,%ld,%ld,%ld,"
				"%u,NOW(),%ld,%ld,'%s')",
				ActCod,
				Gbl.Hierarchy.Cty.CtyCod,
				Gbl.Hierarchy.Ins.InsCod,
				Gbl.Hierarchy.Ctr.CtrCod,
				Gbl.Hierarchy.Deg.DegCod,
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				(unsigned) RoleToStore,
				Gbl.TimeGenerationInMicroseconds,
				Gbl.TimeSendInMicroseconds,
				Gbl.IP);

   /* Log access in recent log (log_recent) */
   DB_QueryINSERT ("can not log access (recent)",
		   "INSERT INTO log_recent "
	           "(LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,UsrCod,"
	           "Role,ClickTime,TimeToGenerate,TimeToSend,IP)"
                   " VALUES "
                   "(%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,"
                   "%u,NOW(),%ld,%ld,'%s')",
		   LogCod,ActCod,
		   Gbl.Hierarchy.Cty.CtyCod,
		   Gbl.Hierarchy.Ins.InsCod,
		   Gbl.Hierarchy.Ctr.CtrCod,
		   Gbl.Hierarchy.Deg.DegCod,
		   Gbl.Hierarchy.Crs.CrsCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) RoleToStore,
		   Gbl.TimeGenerationInMicroseconds,
		   Gbl.TimeSendInMicroseconds,
		   Gbl.IP);

   /* Log access while answering exam prints */
   ExaLog_LogAccess (LogCod);

   /* Log comments */
   if (Comments)
     {
      MaxLength = strlen (Comments) * Str_MAX_BYTES_PER_CHAR;
      if ((CommentsDB = (char *) malloc (MaxLength + 1)) != NULL)
	{
	 Str_Copy (CommentsDB,Comments,
	           MaxLength);
	 Str_ChangeFormat (Str_FROM_TEXT,Str_TO_TEXT,
			   CommentsDB,MaxLength,true);	// Avoid SQL injection
	 DB_QueryINSERT ("can not log access (comments)",
			 "INSERT INTO log_comments"
			 " (LogCod,Comments)"
			 " VALUES"
			 " (%ld,'%s')",
			 LogCod,CommentsDB);
	 free (CommentsDB);
	}
     }

   /* Log search string */
   if (Gbl.Search.LogSearch && Gbl.Search.Str[0])
      DB_QueryINSERT ("can not log access (search)",
		      "INSERT INTO log_search"
		      " (LogCod,SearchStr)"
		      " VALUES"
		      " (%ld,'%s')",
		      LogCod,Gbl.Search.Str);

   if (Gbl.WebService.IsWebService)
      /* Log web service plugin and function */
      DB_QueryINSERT ("can not log access (comments)",
		      "INSERT INTO log_ws"
	              " (LogCod,PlgCod,FunCod)"
                      " VALUES"
                      " (%ld,%ld,%u)",
	              LogCod,Gbl.WebService.PlgCod,
		      (unsigned) Gbl.WebService.Function);
   else
     {
      BanCodClicked = Ban_GetBanCodClicked ();
      if (BanCodClicked > 0)
	 /* Log banner clicked */
	 DB_QueryINSERT ("can not log banner clicked",
			 "INSERT INTO log_banners"
			 " (LogCod,BanCod)"
			 " VALUES"
			 " (%ld,%ld)",
			 LogCod,BanCodClicked);
     }

   /***** Increment my number of clicks *****/
   if (Gbl.Usrs.Me.Logged)
      Prf_IncrementNumClicksUsr (Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************ Sometimes, we delete old entries in recent log table ***********/
/*****************************************************************************/

void Log_RemoveOldEntriesRecentLog (void)
  {
   /***** Remove all expired clipboards *****/
   DB_QueryDELETE ("can not remove old entries from recent log",
		   "DELETE LOW_PRIORITY FROM log_recent"
                   " WHERE ClickTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
		   Log_SECONDS_IN_RECENT_LOG);
  }

/*****************************************************************************/
/*************** Put a link to show last clicks in real time *****************/
/*****************************************************************************/

void Log_PutLinkToLastClicks (void)
  {
   extern const char *Txt_Last_clicks;

   Lay_PutContextualLinkIconText (ActLstClk,NULL,
                                  NULL,NULL,
				  "mouse-pointer.svg",
				  Txt_Last_clicks);
  }

/*****************************************************************************/
/****************************** Show last clicks *****************************/
/*****************************************************************************/

void Log_ShowLastClicks (void)
  {
   extern const char *Hlp_USERS_Connected_last_clicks;
   extern const char *Txt_Last_clicks_in_real_time;

   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
   Sta_PutLinkToGlobalHits ();	// Global hits
   Sta_PutLinkToCourseHits ();	// Course hits
   Mnu_ContextMenuEnd ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Last_clicks_in_real_time,
                 NULL,NULL,
                 Hlp_USERS_Connected_last_clicks,Box_NOT_CLOSABLE);

   /***** Get and show last clicks *****/
   HTM_DIV_Begin ("id=\"lastclicks\" class=\"CM\"");	// Used for AJAX based refresh
   Log_GetAndShowLastClicks ();
   HTM_DIV_End ();					// Used for AJAX based refresh

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/**************** Get last clicks from database and show them ****************/
/*****************************************************************************/

void Log_GetAndShowLastClicks (void)
  {
   extern const char *Txt_Click;
   extern const char *Txt_ELAPSED_TIME;
   extern const char *Txt_Role;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Degree;
   extern const char *Txt_Action;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;
   long ActCod;
   Act_Action_t Action;
   const char *ClassRow;
   time_t TimeDiff;
   struct Hie_Hierarchy Hie;

   /***** Get last clicks from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get last clicks",
			     "SELECT LogCod,"				// row[0]
			            "ActCod,"				// row[1]
			            "UNIX_TIMESTAMP()-"
			            "UNIX_TIMESTAMP(ClickTime),"	// row[2]
			            "Role,"				// row[3]
			            "CtyCod,"				// row[4]
			            "InsCod,"				// row[5]
			            "CtrCod,"				// row[6]
			            "DegCod"				// row[7]
			     " FROM log_recent"
			     " ORDER BY LogCod DESC LIMIT 20");

   /***** Write list of connected users *****/
   HTM_TABLE_BeginCenterPadding (1);
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"LC_CLK",Txt_Click);		// Click
   HTM_TH (1,1,"LC_TIM",Txt_ELAPSED_TIME);	// Elapsed time
   HTM_TH (1,1,"LC_ROL",Txt_Role);		// Role
   HTM_TH (1,1,"LC_CTY",Txt_Country);		// Country
   HTM_TH (1,1,"LC_INS",Txt_Institution);	// Institution
   HTM_TH (1,1,"LC_CTR",Txt_Centre);		// Centre
   HTM_TH (1,1,"LC_DEG",Txt_Degree);		// Degree
   HTM_TH (1,1,"LC_ACT",Txt_Action);		// Action

   HTM_TR_End ();

   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get action code (row[1]) */
      ActCod = Str_ConvertStrCodToLongCod (row[1]);
      Action = Act_GetActionFromActCod (ActCod);

      /* Use a special color for this row depending on the action */
      ClassRow = (Act_GetBrowserTab (Action) == Act_DOWNLD_FILE) ? "DAT_SMALL_YELLOW" :
	         (ActCod == Act_GetActCod (ActLogIn   ) ||
	          ActCod == Act_GetActCod (ActLogInNew)) ? "DAT_SMALL_GREEN" :
                 (ActCod == Act_GetActCod (ActLogOut  )) ? "DAT_SMALL_RED" :
                 (ActCod == Act_GetActCod (ActWebSvc  )) ? "DAT_SMALL_BLUE" :
                                                           "DAT_SMALL_GREY";

      /* Compute elapsed time from last access */
      if (sscanf (row[2],"%ld",&TimeDiff) != 1)
         TimeDiff = (time_t) 0;

      /* Get country code (row[4]) */
      Hie.Cty.CtyCod = Str_ConvertStrCodToLongCod (row[4]);
      Cty_GetCountryName (Hie.Cty.CtyCod,Gbl.Prefs.Language,
			  Hie.Cty.Name[Gbl.Prefs.Language]);

      /* Get institution code (row[5]) */
      Hie.Ins.InsCod = Str_ConvertStrCodToLongCod (row[5]);
      Ins_GetShortNameOfInstitution (&Hie.Ins);

      /* Get centre code (row[6]) */
      Hie.Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[6]);
      Ctr_GetShortNameOfCentreByCod (&Hie.Ctr);

      /* Get degree code (row[7]) */
      Hie.Deg.DegCod = Str_ConvertStrCodToLongCod (row[7]);
      Deg_GetShortNameOfDegreeByCod (&Hie.Deg);

      /* Print table row */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"LC_CLK %s\"",ClassRow);
      HTM_Txt (row[0]);						// Click
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LC_TIM %s\"",ClassRow);		// Elapsed time
      Dat_WriteHoursMinutesSecondsFromSeconds (TimeDiff);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LC_ROL %s\"",ClassRow);
      HTM_Txt (							// Role
	       Txt_ROLES_SINGUL_Abc[Rol_ConvertUnsignedStrToRole (row[3])][Usr_SEX_UNKNOWN]);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LC_CTY %s\"",ClassRow);
      HTM_Txt (Hie.Cty.Name[Gbl.Prefs.Language]);		// Country
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LC_INS %s\"",ClassRow);
      HTM_Txt (Hie.Ins.ShrtName);				// Institution
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LC_CTR %s\"",ClassRow);
      HTM_Txt (Hie.Ctr.ShrtName);				// Centre
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LC_DEG %s\"",ClassRow);
      HTM_Txt (Hie.Deg.ShrtName);				// Degree
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LC_ACT %s\"",ClassRow);
      HTM_Txt (Act_GetActionText (Action));			// Action
      HTM_TD_End ();

      HTM_TR_End ();
     }
   HTM_TABLE_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
