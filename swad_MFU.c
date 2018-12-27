// swad_role.c: user's roles

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <mysql/mysql.h>	// To access MySQL databases
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For malloc and free
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_MFU.h"
#include "swad_tab.h"
#include "swad_theme.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define MFU_MAX_CHARS_TAB	(128 - 1)	// 127
#define MFU_MAX_BYTES_TAB	((MFU_MAX_CHARS_TAB + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define MFU_MAX_CHARS_MENU	(128 - 1)	// 127
#define MFU_MAX_BYTES_MENU	((MFU_MAX_CHARS_MENU + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/************** Allocate list of most frequently used actions ****************/
/*****************************************************************************/

void MFU_AllocateMFUActions (struct MFU_ListMFUActions *ListMFUActions,unsigned MaxActionsShown)
  {
   if ((ListMFUActions->Actions = (Act_Action_t *) malloc (sizeof (Act_Action_t) * MaxActionsShown)) == NULL)
      Lay_ShowErrorAndExit ("Can not allocate memory for list of most frequently used actions.");
  }

/*****************************************************************************/
/**************** Free list of most frequently used actions ******************/
/*****************************************************************************/

void MFU_FreeMFUActions (struct MFU_ListMFUActions *ListMFUActions)
  {
   if (ListMFUActions->Actions != NULL)
      free ((void *) ListMFUActions->Actions);
  }

/*****************************************************************************/
/*************** Get and write most frequently used actions ******************/
/*****************************************************************************/
// ListMFUActions->Actions must have space for MaxActionsShown actions

void MFU_GetMFUActions (struct MFU_ListMFUActions *ListMFUActions,unsigned MaxActionsShown)
  {
   extern Act_Action_t Act_FromActCodToAction[1 + Act_MAX_ACTION_COD];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;
   long ActCod;
   Act_Action_t Action;

   /***** Get most frequently used actions *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get most frequently used actions",
			     "SELECT ActCod FROM actions_MFU"
			     " WHERE UsrCod=%ld ORDER BY Score DESC,LastClick DESC",
			     Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Write list of frequently used actions *****/
   for (NumRow = 0, ListMFUActions->NumActions = 0;
        NumRow < NumRows && ListMFUActions->NumActions < MaxActionsShown;
        NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get action code (row[0]) */
      ActCod = Str_ConvertStrCodToLongCod (row[0]);
      if (ActCod >= 0 && ActCod <= Act_MAX_ACTION_COD)
         if ((Action = Act_FromActCodToAction[ActCod]) >= 0)
            if (Act_GetIndexInMenu (Action) >= 0)	// MFU actions must be only actions shown on menu (database could contain wrong action numbers)
               if (Act_CheckIfIHavePermissionToExecuteAction (Action))
                  ListMFUActions->Actions[ListMFUActions->NumActions++] = Action;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Get my last action in the current tab ********************/
/*****************************************************************************/

Act_Action_t MFU_GetMyLastActionInCurrentTab (void)
  {
   extern Act_Action_t Act_FromActCodToAction[1 + Act_MAX_ACTION_COD];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumActions;
   unsigned NumAct;
   long ActCod;
   Act_Action_t Action;
   Act_Action_t MoreRecentActionInCurrentTab = ActUnk;

   if (Gbl.Usrs.Me.UsrDat.UsrCod > 0)
     {
      /***** Get my most frequently used actions *****/
      NumActions =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get"
					    " most frequently used actions",
				 "SELECT ActCod FROM actions_MFU"
				 " WHERE UsrCod=%ld"
				 " ORDER BY LastClick DESC,Score DESC",
				 Gbl.Usrs.Me.UsrDat.UsrCod);

      /***** Loop over list of frequently used actions *****/
      for (NumAct = 0;
	   NumAct < NumActions;
	   NumAct++)
        {
         row = mysql_fetch_row (mysql_res);

         /* Get action code (row[0]) */
         ActCod = Str_ConvertStrCodToLongCod (row[0]);
         if (ActCod >= 0 && ActCod <= Act_MAX_ACTION_COD)
            if ((Action = Act_FromActCodToAction[ActCod]) >= 0)
               if (Act_GetTab (Act_GetSuperAction (Action)) == Gbl.Action.Tab)
                  if (Act_CheckIfIHavePermissionToExecuteAction (Action))
                    {
                     MoreRecentActionInCurrentTab = Action;
                     break;
                    }
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return MoreRecentActionInCurrentTab;
  }

/*****************************************************************************/
/************* Show a list of my most frequently used actions ****************/
/*****************************************************************************/

void MFU_ShowMyMFUActions (void)
  {
   struct MFU_ListMFUActions ListMFUActions;

   MFU_AllocateMFUActions (&ListMFUActions,10);

   MFU_GetMFUActions (&ListMFUActions,10);
   MFU_WriteBigMFUActions (&ListMFUActions);

   MFU_FreeMFUActions (&ListMFUActions);
  }

/*****************************************************************************/
/*************** Write list of most frequently used actions ******************/
/*****************************************************************************/

void MFU_WriteBigMFUActions (struct MFU_ListMFUActions *ListMFUActions)
  {
   extern const char *Hlp_ANALYTICS_Frequent;
   extern const char *The_ClassFormNoWrap[The_NUM_THEMES];
   extern const char *Txt_My_frequent_actions;
   extern const char *Txt_TABS_TXT[Tab_NUM_TABS];
   unsigned NumAct;
   Act_Action_t Action;
   const char *Title;
   char TabStr[MFU_MAX_BYTES_TAB + 1];
   char MenuStr[MFU_MAX_BYTES_MENU + 1];
   char TabMenuStr[MFU_MAX_BYTES_TAB + 6 + MFU_MAX_BYTES_MENU + 1];

   /***** Start box *****/
   Box_StartBox (NULL,Txt_My_frequent_actions,NULL,
                 Hlp_ANALYTICS_Frequent,Box_NOT_CLOSABLE);
   fprintf (Gbl.F.Out,"<div id=\"MFU_actions_big\">");

   /***** Write list of frequently used actions *****/
   fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">");
   for (NumAct = 0;
	NumAct < ListMFUActions->NumActions;
	NumAct++)
     {
      Action = ListMFUActions->Actions[NumAct];

      if ((Title = Act_GetTitleAction (Action)) != NULL)
        {
	 /* Action string */
	 Str_Copy (TabStr,Txt_TABS_TXT[Act_GetTab (Act_GetSuperAction (Action))],
	           MFU_MAX_BYTES_TAB);
	 Str_Copy (MenuStr,Title,
	           MFU_MAX_BYTES_MENU);
         snprintf (TabMenuStr,sizeof (TabMenuStr),
                   "%s &gt; %s",
		   TabStr,MenuStr);

         /* Icon and text */
         fprintf (Gbl.F.Out,"<li>");
         Frm_StartForm (Action);
         Frm_LinkFormSubmit (TabMenuStr,The_ClassFormNoWrap[Gbl.Prefs.Theme],NULL);
	 fprintf (Gbl.F.Out,"<img src=\"%s/%s\" alt=\"%s\" />",
		  Gbl.Prefs.PathIconSet,
		  Act_GetIcon (Action),
		  MenuStr);
         fprintf (Gbl.F.Out," %s</a>",TabMenuStr);
         Frm_EndForm ();
         fprintf (Gbl.F.Out,"</li>");
        }
     }

   /***** End box *****/
   fprintf (Gbl.F.Out,"</div>");
   Box_EndBox ();
  }

/*****************************************************************************/
/*************** Get and write most frequently used actions ******************/
/*****************************************************************************/

void MFU_WriteSmallMFUActions (struct MFU_ListMFUActions *ListMFUActions)
  {
   extern const char *Txt_My_frequent_actions;
   extern const char *Txt_Frequent_ACTIONS;
   extern const char *Txt_TABS_TXT[Tab_NUM_TABS];
   unsigned NumAct;
   Act_Action_t Action;
   const char *Title;
   char TabStr[MFU_MAX_BYTES_TAB + 1];
   char MenuStr[MFU_MAX_BYTES_MENU + 1];
   char TabMenuStr[MFU_MAX_BYTES_TAB + 6 + MFU_MAX_BYTES_MENU + 1];

   /***** Start div and link *****/
   fprintf (Gbl.F.Out,"<div id=\"MFU_actions\">");
   Frm_StartForm (ActMFUAct);
   Frm_LinkFormSubmit (Txt_My_frequent_actions,NULL,NULL);
   fprintf (Gbl.F.Out," %s"
	              "</a>",
	    Txt_Frequent_ACTIONS);
   Frm_EndForm ();

   /***** Write list of frequently used actions *****/
   fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">");
   for (NumAct = 0;
	NumAct < ListMFUActions->NumActions;
	NumAct++)
     {
      Action = ListMFUActions->Actions[NumAct];

      if ((Title = Act_GetTitleAction (Action)) != NULL)
        {
	 /* Action string */
	 Str_Copy (TabStr,Txt_TABS_TXT[Act_GetTab (Act_GetSuperAction (Action))],
	           MFU_MAX_BYTES_TAB);
	 Str_Copy (MenuStr,Title,
	           MFU_MAX_BYTES_MENU);
         snprintf (TabMenuStr,sizeof (TabMenuStr),
                   "%s &gt; %s",
		   TabStr,MenuStr);

         /* Icon and text */
         fprintf (Gbl.F.Out,"<li>");
         Frm_StartForm (Action);
         Frm_LinkFormSubmit (TabMenuStr,NULL,NULL);
         fprintf (Gbl.F.Out,"<img src=\"%s/%s\" alt=\"%s\" />",
                  Gbl.Prefs.PathIconSet,
                  Act_GetIcon (Action),
                  MenuStr);
	 fprintf (Gbl.F.Out," %s</a>",MenuStr);
         Frm_EndForm ();
         fprintf (Gbl.F.Out,"</li>");
        }
     }
   fprintf (Gbl.F.Out,"</ul>");

   /***** End div *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******************** Update most frequently used actions ********************/
/*****************************************************************************/

#define MFU_MIN_SCORE		  0.5
#define MFU_MAX_SCORE		100.0
#define MFU_INCREASE_FACTOR	  1.2
#define MFU_DECREASE_FACTOR	  0.99

void MFU_UpdateMFUActions (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   float Score;
   long ActCod;
   Act_Action_t SuperAction;

   /***** In some cases, don't register action *****/
   if (!Gbl.Usrs.Me.Logged)
      return;
   if (Act_GetIndexInMenu (Gbl.Action.Act) < 0)
      return;
   SuperAction = Act_GetSuperAction (Gbl.Action.Act);
   if (SuperAction == ActMFUAct)
      return;

   ActCod = Act_GetActCod (SuperAction);

   Str_SetDecimalPointToUS ();	// To get the decimal point as a dot

   /***** Get current score *****/
   if (DB_QuerySELECT (&mysql_res,"can not get score for current action",
	               "SELECT Score FROM actions_MFU"
		       " WHERE UsrCod=%ld AND ActCod=%ld",
		       Gbl.Usrs.Me.UsrDat.UsrCod,ActCod))
     {
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%f",&Score) != 1)
         Lay_ShowErrorAndExit ("Error when getting score for current action.");
      Score *= MFU_INCREASE_FACTOR;
      if (Score > MFU_MAX_SCORE)
         Score = MFU_MAX_SCORE;
     }
   else
      Score = MFU_MIN_SCORE;	// Initial score for a new action not present in MFU table

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Update score for the current action *****/
   DB_QueryREPLACE ("can not update most frequently used actions",
		    "REPLACE INTO actions_MFU"
		    " (UsrCod,ActCod,Score,LastClick)"
		    " VALUES"
		    " (%ld,%ld,'%f',NOW())",
	            Gbl.Usrs.Me.UsrDat.UsrCod,ActCod,Score);

   /***** Update score for other actions *****/
   DB_QueryUPDATE ("can not update most frequently used actions",
		   "UPDATE actions_MFU SET Score=GREATEST(Score*'%f','%f')"
		   " WHERE UsrCod=%ld AND ActCod<>%ld",
                   MFU_DECREASE_FACTOR,MFU_MIN_SCORE,
                   Gbl.Usrs.Me.UsrDat.UsrCod,ActCod);

   Str_SetDecimalPointToLocal ();	// Return to local system
  }
