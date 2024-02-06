// swad_timeline_who.c: select users whom timeline is displayed

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

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_database.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_who.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

Usr_Who_t Tml_GlobalWho;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static Usr_Who_t TmlWho_GetWhoFromDB (void);

static void TmlWho_SetGlobalWho (Usr_Who_t Who);

static void TmlWho_ShowWarningYouDontFollowAnyUser (void);

/*****************************************************************************/
/******** Show form to select users whom public activity is displayed ********/
/*****************************************************************************/

void TmlWho_PutFormWho (struct Tml_Timeline *Timeline)
  {
   Usr_Who_t Who;
   unsigned Mask = 1 << Usr_WHO_ME       |
	           1 << Usr_WHO_FOLLOWED |
		   1 << Usr_WHO_ALL;

   /***** Setting selector for which users *****/
   Set_BeginSettingsHead ();
      Set_BeginOneSettingSelector ();
	 for (Who  = (Usr_Who_t) 0;
	      Who <= (Usr_Who_t) (Usr_NUM_WHO - 1);
	      Who++)
	    if (Mask & (1 << Who))
	      {
	       Set_BeginPref (Who == Timeline->Who);
		  Frm_BeginForm (ActSeeGblTL);
		     Par_PutParUnsigned (NULL,"Who",(unsigned) Who);
		     Usr_PutWhoIcon (Who);
		  Frm_EndForm ();
	       Set_EndPref ();
	      }
      Set_EndOneSettingSelector ();
   Set_EndSettingsHead ();

   /***** Show warning if I do not follow anyone *****/
   if (Timeline->Who == Usr_WHO_FOLLOWED)
      TmlWho_ShowWarningYouDontFollowAnyUser ();
  }

/*****************************************************************************/
/********* Get parameter with which users to view in global timeline *********/
/*****************************************************************************/

void TmlWho_GetParWho (void)
  {
   Usr_Who_t Who;

   /***** Get which users I want to see *****/
   Who = Usr_GetParWho ();

   /***** If parameter Who is not present, get it from database *****/
   if (Who == Usr_WHO_UNKNOWN)
      Who = TmlWho_GetWhoFromDB ();

   /***** If parameter Who is unknown, set it to default *****/
   if (Who == Usr_WHO_UNKNOWN)
      Who = TmlWho_DEFAULT_WHO;

   /***** Set global variable *****/
   TmlWho_SetGlobalWho (Who);
  }

/*****************************************************************************/
/********* Get which users to view in global timeline from database **********/
/*****************************************************************************/

static Usr_Who_t TmlWho_GetWhoFromDB (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;
   Usr_Who_t Who = Usr_WHO_UNKNOWN;

   /***** Get which users from database *****/
   if (Tml_DB_GetWho (&mysql_res) == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get who */
      if (sscanf (row[0],"%u",&UnsignedNum) == 1)
         if (UnsignedNum < Usr_NUM_WHO)
            Who = (Usr_Who_t) UnsignedNum;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Who;
  }

/*****************************************************************************/
/******** Save which users to view in global timeline into database **********/
/*****************************************************************************/

void TmlWho_SaveWhoInDB (struct Tml_Timeline *Timeline)
  {
   if (Gbl.Usrs.Me.Logged)	// Save only if I am logged
     {
      if (Timeline->Who == Usr_WHO_UNKNOWN)
	 Timeline->Who = TmlWho_DEFAULT_WHO;

      /***** Update which users in database *****/
      // Who is stored in usr_last for next time I log in
      Tml_DB_UpdateWho (Timeline->Who);
     }
  }

/*****************************************************************************/
/**** Set/get global variable with which users to view in global timeline ****/
/*****************************************************************************/

static void TmlWho_SetGlobalWho (Usr_Who_t Who)
  {
   Tml_GlobalWho = Who;
  }

Usr_Who_t TmlWho_GetGlobalWho (void)
  {
   return Tml_GlobalWho;
  }

/*****************************************************************************/
/********* Get parameter with which users to view in global timeline *********/
/*****************************************************************************/

static void TmlWho_ShowWarningYouDontFollowAnyUser (void)
  {
   extern const char *Txt_You_dont_follow_any_user;
   unsigned NumFollowing;
   unsigned NumFollowers;

   /***** Check if I follow someone *****/
   Fol_GetNumFollow (Gbl.Usrs.Me.UsrDat.UsrCod,&NumFollowing,&NumFollowers);
   if (!NumFollowing)
     {
      /***** Show warning if I do not follow anyone *****/
      Ale_ShowAlert (Ale_WARNING,Txt_You_dont_follow_any_user);

      /***** Contextual menu *****/
      Mnu_ContextMenuBegin ();
	 Fol_PutLinkWhoToFollow ();	// Users to follow
      Mnu_ContextMenuEnd ();
     }
  }
