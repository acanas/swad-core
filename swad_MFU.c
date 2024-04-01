// swad_MFU.c: Most Frequently Used actions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include <mysql/mysql.h>	// To access MySQL databases
#include <stdlib.h>		// For malloc and free
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_MFU.h"
#include "swad_MFU_database.h"
#include "swad_tab.h"
#include "swad_theme.h"

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define MFU_MAX_CHARS_TAB	(128 - 1)	// 127
#define MFU_MAX_BYTES_TAB	((MFU_MAX_CHARS_TAB + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define MFU_MAX_CHARS_MENU	(128 - 1)	// 127
#define MFU_MAX_BYTES_MENU	((MFU_MAX_CHARS_MENU + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void MFU_PutIconAndText (Act_Action_t Action,
                                const char MenuStr[MFU_MAX_BYTES_MENU + 1]);

/*****************************************************************************/
/************** Allocate list of most frequently used actions ****************/
/*****************************************************************************/

void MFU_AllocateMFUActions (struct MFU_ListMFUActions *ListMFUActions,unsigned MaxActionsShown)
  {
   if ((ListMFUActions->Actions = malloc (MaxActionsShown *
                                          sizeof (*ListMFUActions->Actions))) == NULL)
      Err_ShowErrorAndExit ("Can not allocate memory for list of most frequently used actions.");
  }

/*****************************************************************************/
/**************** Free list of most frequently used actions ******************/
/*****************************************************************************/

void MFU_FreeMFUActions (struct MFU_ListMFUActions *ListMFUActions)
  {
   if (ListMFUActions->Actions != NULL)
      free (ListMFUActions->Actions);
  }

/*****************************************************************************/
/*************** Get and write most frequently used actions ******************/
/*****************************************************************************/
// ListMFUActions->Actions must have space for MaxActionsShown actions

void MFU_GetMFUActions (struct MFU_ListMFUActions *ListMFUActions,unsigned MaxActionsShown)
  {
   MYSQL_RES *mysql_res;
   unsigned NumActions;
   unsigned NumAction;
   long ActCod;
   Act_Action_t Action;

   /***** Get most frequently used actions *****/
   NumActions = MFU_DB_GetMFUActionsOrderByScore (&mysql_res);

   /***** Write list of frequently used actions *****/
   for (NumAction = 0, ListMFUActions->NumActions = 0;
        NumAction < NumActions && ListMFUActions->NumActions < MaxActionsShown;
        NumAction++)
     {
      /* Get action code */
      ActCod = DB_GetNextCode (mysql_res);
      if ((Action = Act_GetActionFromActCod (ActCod)) != ActUnk)
	 if (Act_GetIndexInMenu (Action) >= 0)	// MFU actions must be only actions shown on menu (database could contain wrong action numbers)
	    if (Act_CheckIfICanExecuteAction (Action) == Usr_I_CAN)
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
   MYSQL_RES *mysql_res;
   unsigned NumActions;
   unsigned NumAct;
   long ActCod;
   Act_Action_t Action;
   Act_Action_t MoreRecentActionInCurrentTab = ActUnk;

   if (Gbl.Usrs.Me.UsrDat.UsrCod > 0)
     {
      /***** Get my most frequently used actions *****/
      NumActions = MFU_DB_GetMFUActionsOrderByLastClick (&mysql_res);

      /***** Loop over list of frequently used actions *****/
      for (NumAct = 0;
	   NumAct < NumActions;
	   NumAct++)
        {
         /* Get action code */
         ActCod = DB_GetNextCode (mysql_res);
         if (ActCod >= 0 && ActCod <= ActLst_MAX_ACTION_COD)
            if ((Action = Act_GetActionFromActCod (ActCod)) >= 0)
               if (Act_GetTab (Action) == Gbl.Action.Tab)
                  if (Act_CheckIfICanExecuteAction (Action) == Usr_I_CAN)
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
   extern const char *Txt_My_frequent_actions;
   unsigned NumAct;
   Act_Action_t Action;
   const char *Title;
   char TabStr[MFU_MAX_BYTES_TAB + 1];
   char MenuStr[MFU_MAX_BYTES_MENU + 1];
   char TabMenuStr[MFU_MAX_BYTES_TAB + 6 + MFU_MAX_BYTES_MENU + 1];

   /***** Begin box *****/
   Box_BoxBegin (Txt_My_frequent_actions,NULL,NULL,
                 Hlp_ANALYTICS_Frequent,Box_NOT_CLOSABLE);

      /***** Begin container *****/
      HTM_DIV_Begin ("id=\"MFU_actions_big\"");

	 /***** Begin list of frequently used actions *****/
	 HTM_UL_Begin ("class=\"LIST_LEFT\"");

	    for (NumAct = 0;
		 NumAct < ListMFUActions->NumActions;
		 NumAct++)
	      {
	       Action = ListMFUActions->Actions[NumAct];

	       if ((Title = Act_GetTitleAction (Action)) != NULL)
		 {
		  /* Action string */
		  Str_Copy (TabStr,Tab_GetTxt (Act_GetTab (Action)),
			    sizeof (TabStr) - 1);
		  Str_Copy (MenuStr,Title,sizeof (MenuStr) - 1);
		  snprintf (TabMenuStr,sizeof (TabMenuStr),"%s &gt; %s",TabStr,MenuStr);

		  /* Icon and text */
		  HTM_LI_Begin ("class=\"ICO_HIGHLIGHT\"");
		     Frm_BeginForm (Action);
			HTM_BUTTON_Submit_Begin (TabMenuStr,
			                         "class=\"BT_LINK FORM_IN_%s NOWRAP\"",
			                         The_GetSuffix ());
			   MFU_PutIconAndText (Action,MenuStr);
			HTM_BUTTON_End ();
		     Frm_EndForm ();
		  HTM_LI_End ();
		 }
	      }

	 /***** End list of frequently used actions *****/
	 HTM_UL_End ();

      /***** End container *****/
      HTM_DIV_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*************** Get and write most frequently used actions ******************/
/*****************************************************************************/

void MFU_WriteSmallMFUActions (struct MFU_ListMFUActions *ListMFUActions)
  {
   extern const char *Txt_Frequent_ACTIONS;
   unsigned NumAct;
   Act_Action_t Action;
   const char *Title;
   char TabStr[MFU_MAX_BYTES_TAB + 1];
   char MenuStr[MFU_MAX_BYTES_MENU + 1];
   char TabMenuStr[MFU_MAX_BYTES_TAB + 6 + MFU_MAX_BYTES_MENU + 1];

   /***** Begin fieldset *****/
   HTM_FIELDSET_Begin ("id=\"MFU_actions\" class=\"MFU_%s\"",The_GetSuffix ());
      HTM_LEGEND (Txt_Frequent_ACTIONS);

      /***** List of frequently used actions *****/
      HTM_UL_Begin (NULL);

	 for (NumAct = 0;
	      NumAct < ListMFUActions->NumActions;
	      NumAct++)
	   {
	    Action = ListMFUActions->Actions[NumAct];

	    if ((Title = Act_GetTitleAction (Action)) != NULL)
	      {
	       /* Action string */
	       Str_Copy (TabStr,Tab_GetTxt (Act_GetTab (Action)),
			 sizeof (TabStr) - 1);
	       Str_Copy (MenuStr,Title,sizeof (MenuStr) - 1);
	       snprintf (TabMenuStr,sizeof (TabMenuStr),"%s &gt; %s",TabStr,MenuStr);

	       /* Icon and text */
	       HTM_LI_Begin ("class=\"ICO_HIGHLIGHT\"");
		  Frm_BeginForm (Action);
		     HTM_BUTTON_Submit_Begin (TabMenuStr,"class=\"BT_LINK\"");
			MFU_PutIconAndText (Action,MenuStr);
		     HTM_BUTTON_End ();
		  Frm_EndForm ();
	       HTM_LI_End ();
	      }
	   }

      HTM_UL_End ();

      /***** Link to view more frequently used actions *****/
      Lay_PutContextualLinkOnlyIcon (ActMFUAct,NULL,
				     NULL,NULL,
				     "ellipsis-h.svg",Ico_BLACK);

   /***** End fieldset *****/
   HTM_FIELDSET_End ();
  }

/*****************************************************************************/
/*********************** Put action icon and text ****************************/
/*****************************************************************************/

static void MFU_PutIconAndText (Act_Action_t Action,
                                const char MenuStr[MFU_MAX_BYTES_MENU + 1])
  {
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   static const Ico_Color_t Color[Ico_NUM_ICON_SETS] =
     {
      [Ico_ICON_SET_AWESOME] = Ico_BLACK,
      [Ico_ICON_SET_NUVOLA ] = Ico_UNCHANGED,
     };
   char URLIconSet[PATH_MAX + 1];

   snprintf (URLIconSet,sizeof (URLIconSet),"%s/%s",
	     Cfg_URL_ICON_SETS_PUBLIC,Ico_IconSetId[Gbl.Prefs.IconSet]);
   if (Color[Gbl.Prefs.IconSet] == Ico_UNCHANGED)
      HTM_IMG (URLIconSet,Act_GetIconFromAction (Action),MenuStr,
	       NULL);
   else
      HTM_IMG (URLIconSet,Act_GetIconFromAction (Action),MenuStr,
	       "class=\"ICO_%s_%s\"",
	       Ico_GetPreffix (Color[Gbl.Prefs.IconSet]),
	       The_GetSuffix ());
   HTM_NBSPTxt (MenuStr);
  }

/*****************************************************************************/
/******************** Update most frequently used actions ********************/
/*****************************************************************************/

void MFU_UpdateMFUActions (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   double Score;
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
   if (MFU_DB_GetScoreForCurrentAction (&mysql_res,ActCod))
     {
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%lf",&Score) != 1)
         Err_ShowErrorAndExit ("Error when getting score for current action.");
      Score *= MFU_INCREASE_FACTOR;
      if (Score > MFU_MAX_SCORE)
         Score = MFU_MAX_SCORE;
     }
   else
      Score = MFU_MIN_SCORE;	// Initial score for a new action not present in MFU table

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Update score for the current action *****/
   MFU_DB_UpdateScoreForCurrentAction (ActCod,Score);

   /***** Update score for other actions *****/
   MFU_DB_UpdateScoreForOtherActions (ActCod);

   Str_SetDecimalPointToLocal ();	// Return to local system
  }
