// swad_timeline_who.c: select users whom timeline is displayed

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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
/**************************** Private constants ******************************/
/*****************************************************************************/

static unsigned TmlWho_Allowed[Rol_NUM_ROLES] =
  {
   [Rol_UNK	] = 0,
   [Rol_GST	] = 1 << Usr_WHO_ME | 1 << Usr_WHO_FOLLOWED | 1 << Usr_WHO_ALL,
   [Rol_USR	] = 1 << Usr_WHO_ME | 1 << Usr_WHO_FOLLOWED | 1 << Usr_WHO_ALL,
   [Rol_STD	] = 1 << Usr_WHO_ME | 1 << Usr_WHO_FOLLOWED | 1 << Usr_WHO_ALL,
   [Rol_NET	] = 1 << Usr_WHO_ME | 1 << Usr_WHO_FOLLOWED | 1 << Usr_WHO_ALL,
   [Rol_TCH	] = 1 << Usr_WHO_ME | 1 << Usr_WHO_FOLLOWED | 1 << Usr_WHO_ALL,
   [Rol_DEG_ADM	] = 1 << Usr_WHO_ME | 1 << Usr_WHO_FOLLOWED | 1 << Usr_WHO_ALL,
   [Rol_CTR_ADM	] = 1 << Usr_WHO_ME | 1 << Usr_WHO_FOLLOWED | 1 << Usr_WHO_ALL,
   [Rol_INS_ADM	] = 1 << Usr_WHO_ME | 1 << Usr_WHO_FOLLOWED | 1 << Usr_WHO_ALL,
   [Rol_SYS_ADM	] = 1 << Usr_WHO_ME | 1 << Usr_WHO_FOLLOWED | 1 << Usr_WHO_ALL,
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

struct Tml_WhosePosts Tml_GlobalWhosePosts;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TmlWho_GetWhoFromDB (struct Tml_WhosePosts *WhosePosts);

static void TmlWho_SetGlobalWhosePosts (struct Tml_WhosePosts *WhosePosts);

static void TmlWho_ShowWarningYouDontFollowAnyUser (void);

/*****************************************************************************/
/******** Show form to select users whom public activity is displayed ********/
/*****************************************************************************/

void TmlWho_PutFormWho (struct Tml_Timeline *Timeline)
  {
   Usr_Who_t Who;

   /***** Setting selector for which users *****/
   Set_BeginSettingsHead ();
      Set_BeginOneSettingSelector ();
	 for (Who  = (Usr_Who_t) 0;
	      Who <= (Usr_Who_t) (Usr_NUM_WHO - 1);
	      Who++)
	    if (TmlWho_Allowed[Gbl.Usrs.Me.Role.Logged] & (1 << Who))
	      {
	       Set_BeginPref (Who == Timeline->WhosePosts.Who);
		  Frm_BeginForm (ActSeeGblTL);
		     Par_PutParUnsigned (NULL,"Who",(unsigned) Who);
		     Usr_PutWhoIcon (Who);
		  Frm_EndForm ();
	       Set_EndPref ();
	      }
      Set_EndOneSettingSelector ();
   Set_EndSettingsHead ();

   /***** Show warning if I do not follow anyone *****/
   if (Timeline->WhosePosts.Who == Usr_WHO_FOLLOWED)
      TmlWho_ShowWarningYouDontFollowAnyUser ();
  }

/*****************************************************************************/
/********* Get parameter with which users to view in global timeline *********/
/*****************************************************************************/

void TmlWho_GetParWho (void)
  {
   struct Tml_WhosePosts WhosePosts;

   /***** Get which users I want to see *****/
   WhosePosts.Who = Usr_GetParWho ();
   if (WhosePosts.Who == Usr_WHO_UNKNOWN)	// Parameter not present
     {
      /* If parameter is not present, get it from database */
      TmlWho_GetWhoFromDB (&WhosePosts);
      WhosePosts.WhoShouldBeStoredInDB = Tml_WHO_SHOULD_NOT_BE_STORED_IN_DB;
     }
   else
     {
      /* If parameter is present, check if allowed */
      if ((TmlWho_Allowed[Gbl.Usrs.Me.Role.Logged] & (1 << WhosePosts.Who)) == 0)
	 WhosePosts.Who = TmlWho_DEFAULT_WHO;
      WhosePosts.WhoShouldBeStoredInDB = Tml_WHO_SHOULD_BE_STORED_IN_DB;
     }

   /***** If parameter is unknown, set it to default *****/
   if (WhosePosts.Who == Usr_WHO_UNKNOWN)
     {
      WhosePosts.Who = TmlWho_DEFAULT_WHO;
      WhosePosts.WhoShouldBeStoredInDB = Tml_WHO_SHOULD_BE_STORED_IN_DB;
     }

   /***** Set global variable *****/
   TmlWho_SetGlobalWhosePosts (&WhosePosts);
  }

/*****************************************************************************/
/********* Get which users to view in global timeline from database **********/
/*****************************************************************************/

static void TmlWho_GetWhoFromDB (struct Tml_WhosePosts *WhosePosts)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;

   /***** Get which users from database *****/
   if (Tml_DB_GetWho (&mysql_res) == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get who */
      if (sscanf (row[0],"%u",&UnsignedNum) == 1)
         if (UnsignedNum < Usr_NUM_WHO)
           {
            WhosePosts->Who = (Usr_Who_t) UnsignedNum;

	    if ((TmlWho_Allowed[Gbl.Usrs.Me.Role.Logged] & (1 << WhosePosts->Who)) == 0)
	      {
	       WhosePosts->Who = TmlWho_DEFAULT_WHO;
	       WhosePosts->WhoShouldBeStoredInDB = Tml_WHO_SHOULD_BE_STORED_IN_DB;
	      }
           }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**** Set/get global variable with which users to view in global timeline ****/
/*****************************************************************************/

static void TmlWho_SetGlobalWhosePosts (struct Tml_WhosePosts *WhosePosts)
  {
   Tml_GlobalWhosePosts.Who = WhosePosts->Who;
   Tml_GlobalWhosePosts.WhoShouldBeStoredInDB = WhosePosts->WhoShouldBeStoredInDB;
  }

void TmlWho_GetGlobalWhosePosts (struct Tml_WhosePosts *WhosePosts)
  {
   WhosePosts->Who = Tml_GlobalWhosePosts.Who;
   WhosePosts->WhoShouldBeStoredInDB = Tml_GlobalWhosePosts.WhoShouldBeStoredInDB;
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
