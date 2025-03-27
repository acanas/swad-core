// swad_statistic_database.c: statistics, operations with database

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
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_statistic.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************** Compute and show access statistics ********************/
/*****************************************************************************/

#define Sta_DB_MAX_BYTES_QUERY (1024 + (10 + ID_MAX_BYTES_USR_ID) * 5000 - 1)

unsigned Sta_DB_GetHits (MYSQL_RES **mysql_res,
			 Hie_Level_t HieLvl,
                         const struct Sta_Stats *Stats,
                         const char *LogTable,
                         const char BrowserTimeZone[Dat_MAX_BYTES_TIME_ZONE + 1],
                         unsigned NumUsrsInList,
			 const long *LstSelectedUsrCods)
  {
   char SubQueryCountType[256];
   char SubQueryRole[256];
   char SubQuery[512];
   char *Query = NULL;
   long LengthQuery;
   unsigned NumUsr;
   unsigned NumHits;

   /***** Build subquery depending on the type of count *****/
   switch (Stats->CountType)
     {
      case Sta_TOTAL_CLICKS:
         Str_Copy (SubQueryCountType,"COUNT(*)",sizeof (SubQueryCountType) - 1);
	 break;
      case Sta_DISTINCT_USRS:
         sprintf (SubQueryCountType,"COUNT(DISTINCT %s.UsrCod)",LogTable);
	 break;
      case Sta_CLICKS_PER_USR:
         sprintf (SubQueryCountType,
                  "COUNT(*)/GREATEST(COUNT(DISTINCT %s.UsrCod),1)+0.000000",LogTable);
	 break;
      case Sta_GENERATION_TIME:
         sprintf (SubQueryCountType,
                  "(AVG(%s.TimeToGenerate)/1E6)+0.000000",LogTable);
	 break;
      case Sta_SEND_TIME:
         sprintf (SubQueryCountType,
                  "(AVG(%s.TimeToSend)/1E6)+0.000000",LogTable);
	 break;
     }

   /***** Allocate memory for the query *****/
   if ((Query = malloc (Sta_DB_MAX_BYTES_QUERY + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Begin building the query *****/
   switch (Stats->ClicksGroupedBy)
     {
      case Sta_CLICKS_CRS_DETAILED_LIST:
   	 snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE LogCod,"
   	                               "UsrCod,"
   	                               "Role,"
   		                       "UNIX_TIMESTAMP(ClickTime) AS F,"
   		                       "ActCod"
   		    " FROM %s",
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_USR:
	 snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE UsrCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   SubQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_DAY:
      case Sta_CLICKS_GBL_PER_DAY:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%Y%%m%%d') AS Day,"
                                       "%s"
                    " FROM %s",
                   BrowserTimeZone,
                   SubQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_DAY_AND_HOUR:
      case Sta_CLICKS_GBL_PER_DAY_AND_HOUR:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%Y%%m%%d') AS Day,"
                                       "DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%H') AS Hour,"
                                       "%s"
                    " FROM %s",
                   BrowserTimeZone,
                   BrowserTimeZone,
                   SubQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_WEEK:
      case Sta_CLICKS_GBL_PER_WEEK:
	 /* With %x%v the weeks are counted from monday to sunday.
	    With %X%V the weeks are counted from sunday to saturday. */
	 snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           (Gbl.Prefs.FirstDayOfWeek == 0) ?
	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%x%%v') AS Week,"// Weeks start on monday
		                       "%s"
		    " FROM %s" :
		   "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%X%%V') AS Week,"// Weeks start on sunday
		                       "%s"
		    " FROM %s",
		   BrowserTimeZone,
		   SubQueryCountType,
		   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_MONTH:
      case Sta_CLICKS_GBL_PER_MONTH:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%Y%%m') AS Month,"
                                       "%s"
                    " FROM %s",
                   BrowserTimeZone,
                   SubQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_YEAR:
      case Sta_CLICKS_GBL_PER_YEAR:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%Y') AS Year,"
                                       "%s"
                    " FROM %s",
                   BrowserTimeZone,
                   SubQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_HOUR:
      case Sta_CLICKS_GBL_PER_HOUR:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%H') AS Hour,"
                                       "%s"
                   " FROM %s",
                   BrowserTimeZone,
                   SubQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_MINUTE:
      case Sta_CLICKS_GBL_PER_MINUTE:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE DATE_FORMAT(CONVERT_TZ(ClickTime,@@session.time_zone,'%s'),'%%H%%i') AS Minute,"
                                       "%s"
                    " FROM %s",
                   BrowserTimeZone,
                   SubQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_CRS_PER_ACTION:
      case Sta_CLICKS_GBL_PER_ACTION:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE ActCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   SubQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_PLUGIN:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE log_api.PlgCod,"
   	                                "%s AS Num"
   	            " FROM %s,"
   	                  "log_api",
                   SubQueryCountType,
                   LogTable);
         break;
      case Sta_CLICKS_GBL_PER_API_FUNCTION:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE log_api.FunCod,"
   	                               "%s AS Num"
   	            " FROM %s,"
   	                  "log_api",
                   SubQueryCountType,
                   LogTable);
         break;
      case Sta_CLICKS_GBL_PER_BANNER:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE log_banners.BanCod,"
   	                               "%s AS Num"
   	            " FROM %s,"
   	                  "log_banners",
                   SubQueryCountType,
                   LogTable);
         break;
      case Sta_CLICKS_GBL_PER_COUNTRY:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE CtyCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   SubQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_INSTITUTION:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE InsCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   SubQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_CENTER:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE CtrCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   SubQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_DEGREE:
         snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE DegCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   SubQueryCountType,
                   LogTable);
	 break;
      case Sta_CLICKS_GBL_PER_COURSE:
	 snprintf (Query,Sta_DB_MAX_BYTES_QUERY + 1,
   	           "SELECT SQL_NO_CACHE CrsCod,"
   	                               "%s AS Num"
   	            " FROM %s",
                   SubQueryCountType,
                   LogTable);
	 break;
     }
   sprintf (SubQuery," WHERE %s.ClickTime"
	             " BETWEEN FROM_UNIXTIME(%ld)"
	                 " AND FROM_UNIXTIME(%ld)",
            LogTable,
            (long) Dat_GetRangeTimeUTC (Dat_STR_TIME),
            (long) Dat_GetRangeTimeUTC (Dat_END_TIME));
   Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);

   switch (Stats->GlobalOrCourse)
     {
      case Sta_SHOW_GLOBAL_ACCESSES:
	 /* Scope */
	 switch (HieLvl)
	   {
	    case Hie_UNK:
	    case Hie_SYS:
               break;
	    case Hie_CTY:
               if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)
		 {
		  sprintf (SubQuery," AND %s.CtyCod=%ld",
			   LogTable,Gbl.Hierarchy.Node[Hie_CTY].HieCod);
		  Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
		 }
               break;
	    case Hie_INS:
	       if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
		 {
		  sprintf (SubQuery," AND %s.InsCod=%ld",
			   LogTable,Gbl.Hierarchy.Node[Hie_INS].HieCod);
		  Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
		 }
	       break;
	    case Hie_CTR:
               if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)
		 {
		  sprintf (SubQuery," AND %s.CtrCod=%ld",
			   LogTable,Gbl.Hierarchy.Node[Hie_CTR].HieCod);
		  Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
		 }
               break;
	    case Hie_DEG:
	       if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)
		 {
		  sprintf (SubQuery," AND %s.DegCod=%ld",
			   LogTable,Gbl.Hierarchy.Node[Hie_DEG].HieCod);
		  Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
		 }
	       break;
	    case Hie_CRS:
	       if (Gbl.Hierarchy.HieLvl == Hie_CRS)
		 {
		  sprintf (SubQuery," AND %s.CrsCod=%ld",
			   LogTable,Gbl.Hierarchy.Node[Hie_CRS].HieCod);
		  Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
		 }
	       break;
	   }

         /* Type of users */
	 switch (Stats->Role)
	   {
	    case Sta_ROLE_IDENTIFIED_USRS:
               sprintf (SubQueryRole," AND %s.Role<>%u",
                        LogTable,(unsigned) Rol_UNK);
	       break;
	    case Sta_ROLE_ALL_USRS:
               switch (Stats->CountType)
                 {
                  case Sta_TOTAL_CLICKS:
                  case Sta_GENERATION_TIME:
                  case Sta_SEND_TIME:
                     SubQueryRole[0] = '\0';
	             break;
                  case Sta_DISTINCT_USRS:
                  case Sta_CLICKS_PER_USR:
                     sprintf (SubQueryRole," AND %s.Role<>%u",
                              LogTable,(unsigned) Rol_UNK);
                     break;
                    }
	       break;
	    case Sta_ROLE_INS_ADMINS:
               sprintf (SubQueryRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_INS_ADM);
	       break;
	    case Sta_ROLE_CTR_ADMINS:
               sprintf (SubQueryRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_CTR_ADM);
	       break;
	    case Sta_ROLE_DEG_ADMINS:
               sprintf (SubQueryRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_DEG_ADM);
	       break;
	    case Sta_ROLE_TEACHERS:
               sprintf (SubQueryRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_TCH);
	       break;
	    case Sta_ROLE_NON_EDITING_TEACHERS:
               sprintf (SubQueryRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_NET);
	       break;
	    case Sta_ROLE_STUDENTS:
               sprintf (SubQueryRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_STD);
	       break;
	    case Sta_ROLE_USERS:
               sprintf (SubQueryRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_USR);
               break;
	    case Sta_ROLE_GUESTS:
               sprintf (SubQueryRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_GST);
               break;
	    case Sta_ROLE_UNKNOWN_USRS:
               sprintf (SubQueryRole," AND %s.Role=%u",
                        LogTable,(unsigned) Rol_UNK);
               break;
	    case Sta_ROLE_ME:
               sprintf (SubQueryRole," AND %s.UsrCod=%ld",
                        LogTable,Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	   }
         Str_Concat (Query,SubQueryRole,Sta_DB_MAX_BYTES_QUERY);

         switch (Stats->ClicksGroupedBy)
           {
            case Sta_CLICKS_GBL_PER_PLUGIN:
            case Sta_CLICKS_GBL_PER_API_FUNCTION:
               sprintf (SubQuery," AND %s.LogCod=log_api.LogCod",
                        LogTable);
               Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
               break;
            case Sta_CLICKS_GBL_PER_BANNER:
               sprintf (SubQuery," AND %s.LogCod=log_banners.LogCod",
                        LogTable);
               Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
               break;
            default:
               break;
           }
	 break;
      case Sta_SHOW_COURSE_ACCESSES:
         sprintf (SubQuery," AND %s.CrsCod=%ld",
                  LogTable,Gbl.Hierarchy.Node[Hie_CRS].HieCod);
	 Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);

	 if (NumUsrsInList)
	   {
	    LengthQuery = strlen (Query);

	    for (NumUsr = 0;
		 NumUsr < NumUsrsInList;
		 NumUsr++)
	      {
	       LengthQuery += 25 + 10 + 1;
	       if (LengthQuery > Sta_DB_MAX_BYTES_QUERY - 128)
                  Err_QuerySizeExceededExit ();
	       sprintf (SubQuery,
			NumUsr ? " OR %s.UsrCod=%ld" :
				 " AND (%s.UsrCod=%ld",
			LogTable,LstSelectedUsrCods[NumUsr]);
	       Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
	      }
	    Str_Concat (Query,")",Sta_DB_MAX_BYTES_QUERY);
	   }
	 break;
     }

   /* Select action */
   if (Stats->NumAction != ActAll)
     {
      sprintf (SubQuery," AND %s.ActCod=%ld",
               LogTable,Act_GetActCod (Stats->NumAction));
      Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
     }

   /* End the query */
   switch (Stats->ClicksGroupedBy)
     {
      case Sta_CLICKS_CRS_DETAILED_LIST:
	 Str_Concat (Query," ORDER BY F",
	             Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_CRS_PER_USR:
	 sprintf (SubQuery," GROUP BY %s.UsrCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_CRS_PER_DAY:
      case Sta_CLICKS_GBL_PER_DAY:
	 Str_Concat (Query," GROUP BY Day"
		           " ORDER BY Day DESC",
		     Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_CRS_PER_DAY_AND_HOUR:
      case Sta_CLICKS_GBL_PER_DAY_AND_HOUR:
	 Str_Concat (Query," GROUP BY Day,Hour"
		           " ORDER BY Day DESC,Hour",
		     Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_CRS_PER_WEEK:
      case Sta_CLICKS_GBL_PER_WEEK:
	 Str_Concat (Query," GROUP BY Week"
		           " ORDER BY Week DESC",
		     Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_CRS_PER_MONTH:
      case Sta_CLICKS_GBL_PER_MONTH:
	 Str_Concat (Query," GROUP BY Month"
		           " ORDER BY Month DESC",
		     Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_CRS_PER_YEAR:
      case Sta_CLICKS_GBL_PER_YEAR:
	 Str_Concat (Query," GROUP BY Year"
		           " ORDER BY Year DESC",
		     Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_CRS_PER_HOUR:
      case Sta_CLICKS_GBL_PER_HOUR:
	 Str_Concat (Query," GROUP BY Hour"
		           " ORDER BY Hour",
		     Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_CRS_PER_MINUTE:
      case Sta_CLICKS_GBL_PER_MINUTE:
	 Str_Concat (Query," GROUP BY Minute"
		           " ORDER BY Minute",
		     Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_CRS_PER_ACTION:
      case Sta_CLICKS_GBL_PER_ACTION:
	 sprintf (SubQuery," GROUP BY %s.ActCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_GBL_PER_PLUGIN:
         Str_Concat (Query," GROUP BY log_api.PlgCod"
        	           " ORDER BY Num DESC",
                     Sta_DB_MAX_BYTES_QUERY);
         break;
      case Sta_CLICKS_GBL_PER_API_FUNCTION:
         Str_Concat (Query," GROUP BY log_api.FunCod"
        	           " ORDER BY Num DESC",
                     Sta_DB_MAX_BYTES_QUERY);
         break;
      case Sta_CLICKS_GBL_PER_BANNER:
         Str_Concat (Query," GROUP BY log_banners.BanCod"
        	           " ORDER BY Num DESC",
                     Sta_DB_MAX_BYTES_QUERY);
         break;
      case Sta_CLICKS_GBL_PER_COUNTRY:
	 sprintf (SubQuery," GROUP BY %s.CtyCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_GBL_PER_INSTITUTION:
	 sprintf (SubQuery," GROUP BY %s.InsCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_GBL_PER_CENTER:
	 sprintf (SubQuery," GROUP BY %s.CtrCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_GBL_PER_DEGREE:
	 sprintf (SubQuery," GROUP BY %s.DegCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
	 break;
      case Sta_CLICKS_GBL_PER_COURSE:
	 sprintf (SubQuery," GROUP BY %s.CrsCod"
		           " ORDER BY Num DESC",
		  LogTable);
         Str_Concat (Query,SubQuery,Sta_DB_MAX_BYTES_QUERY);
	 break;
     }
   /***** Write query for debug *****/
   /*
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      Ale_ShowAlert (Ale_INFO,Query);
   */

   /***** Make the query *****/
   NumHits = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get clicks",
		   "%s",
		   Query);

   /* Free memory for the query */
   free (Query);

   return NumHits;
  }
