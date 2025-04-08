// swad_connected.c: connected users

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <limits.h>		// For maximum values
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_connected.h"
#include "swad_connected_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_log.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_role.h"
#include "swad_string.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static struct
  {
   unsigned long TimeToRefresh;
   unsigned NumUsrs;
   unsigned NumUsrsToList;
   struct Con_ConnectedUsrs Usrs[Rol_NUM_ROLES];
   struct
     {
      long UsrCod;
      bool ThisCrs;
      time_t TimeDiff;
     } Lst[Cfg_MAX_CONNECTED_SHOWN];
  } Con_Connected =
  {
   .TimeToRefresh = Con_MAX_TIME_TO_REFRESH_CONNECTED_IN_MS,
  };

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static unsigned long Con_GetTimeToRefresh (void);

static void Con_PutIconToUpdateConnected (void *Scope);
static void Con_PutParScope (void *Scope);

static void Con_ShowGlobalConnectedUsrsRole (Rol_Role_t Role,unsigned UsrsTotal);

static void Con_ComputeConnectedUsrsWithARoleBelongingToCurrentCrs (Rol_Role_t Role);
static void Con_ShowConnectedUsrsBelongingToScope (Hie_Level_t HieLvl,
						   unsigned AllowedLvls);

static void Con_ShowConnectedUsrsWithARoleBelongingToScopeOnMainZone (Hie_Level_t HieLvl,
								      Rol_Role_t Role);
static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_Role_t Role,unsigned *NumUsr);
static unsigned Con_GetConnectedUsrsTotal (Rol_Role_t Role);

static void Con_GetNumConnectedWithARole (Hie_Level_t HieLvl,Rol_Role_t Role,
					  struct Con_ConnectedUsrs *Usrs);
static void Con_ComputeConnectedUsrsWithARoleCurrentCrsOneByOne (Rol_Role_t Role,
								 unsigned *NumUsrsConnected,
								 unsigned *NumUsrsToList);
static void Con_ShowConnectedUsrsCurrentCrsOneByOneOnRightColumn (Rol_Role_t Role,unsigned *NumUsr);
static void Con_WriteRowConnectedUsrOnRightColumn (Rol_Role_t Role,unsigned NumUsr);
static void Con_ShowConnectedUsrsCurrentLocationOneByOneOnMainZone (Hie_Level_t HieLvl,
								    Rol_Role_t Role);

/*****************************************************************************/
/******** Update time to refresh depending on number of open sessions ********/
/*****************************************************************************/

void Con_SetTimeToRefresh (unsigned NumSessions)
  {
   Con_Connected.TimeToRefresh = (unsigned long) (NumSessions /
	                                          Cfg_TIMES_PER_SECOND_REFRESH_CONNECTED)
	                         * 1000UL;

   if (Con_Connected.TimeToRefresh < Con_MIN_TIME_TO_REFRESH_CONNECTED_IN_MS)
      Con_Connected.TimeToRefresh = Con_MIN_TIME_TO_REFRESH_CONNECTED_IN_MS;
   else if (Con_Connected.TimeToRefresh > Con_MAX_TIME_TO_REFRESH_CONNECTED_IN_MS)
      Con_Connected.TimeToRefresh = Con_MAX_TIME_TO_REFRESH_CONNECTED_IN_MS;
  }

static unsigned long Con_GetTimeToRefresh (void)
  {
   return Con_Connected.TimeToRefresh;
  }

/*****************************************************************************/
/******************** Refresh connected users via AJAX ***********************/
/*****************************************************************************/

void Con_RefreshConnected (void)
  {
   unsigned NumUsr;
   bool ShowConnected = (Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN) &&
                        Gbl.Hierarchy.HieLvl == Hie_CRS;	// Right column visible && There is a course selected

   /***** Refresh time *****/
   HTM_TxtF ("%lu|",Con_GetTimeToRefresh ());

   /***** Number of notications *****/
   if (Gbl.Usrs.Me.Logged)
      Ntf_WriteNumberOfNewNtfs ();
   HTM_Char ('|');

   /***** Number of global connected users *****/
   Con_ShowGlobalConnectedUsrs ();
   HTM_Char ('|');

   /***** Number of course connected users *****/
   if (ShowConnected)
      Con_ShowConnectedUsrsBelongingToCurrentCrs ();
   HTM_Char ('|');

   /***** Number of users to list *****/
   if (ShowConnected)
      HTM_Unsigned (Con_Connected.NumUsrsToList);
   HTM_Char ('|');

   /***** Time differences *****/
   if (ShowConnected)
      for (NumUsr = 0;
	   NumUsr < Con_Connected.NumUsrsToList;
	   NumUsr++)
         HTM_TxtF ("%ld|",Con_Connected.Lst[NumUsr].TimeDiff);
  }

/*****************************************************************************/
/************************** Show connected users *****************************/
/*****************************************************************************/

void Con_ShowConnectedUsrs (void)
  {
   extern const char *Hlp_USERS_Connected;
   extern const char *Txt_Connected_users;
   extern const char *Txt_Sessions;
   extern const char *Txt_Connected_PLURAL;
   Hie_Level_t HieLvl;
   unsigned AllowedLvls;

   /***** Contextual menu *****/
   if (Gbl.Usrs.Me.Logged)
     {
      Mnu_ContextMenuBegin ();
	 Log_PutLinkToLastClicks ();	// Show last clicks in real time
      Mnu_ContextMenuEnd ();
     }

   /***** Get scope *****/
   AllowedLvls = Sco_GetAllowedScopesForListingStudents ();
   HieLvl = Sco_GetScope ("ScopeCon",Hie_CRS,AllowedLvls);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Connected_users,Con_PutIconToUpdateConnected,&HieLvl,
		 Hlp_USERS_Connected,Box_NOT_CLOSABLE);

      /***** Current time *****/
      HTM_DIV_Begin ("id=\"connected_current_time\" class=\"CON_%s\"",
		     The_GetSuffix ());
      HTM_DIV_End ();
      Dat_WriteLocalDateHMSFromUTC ("connected_current_time",Dat_GetStartExecutionTimeUTC (),
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				    Dat_WRITE_WEEK_DAY |
				    Dat_WRITE_HOUR |
				    Dat_WRITE_MINUTE |
				    Dat_WRITE_SECOND);

      /***** Number of connected users in the whole platform *****/
      HTM_FIELDSET_Begin ("class=\"CON CON_%s\"",The_GetSuffix ());
	 HTM_LEGEND (Txt_Sessions);
	 Con_ShowGlobalConnectedUsrs ();
      HTM_FIELDSET_End ();

      /***** Show connected users in the current scope *****/
      if (HieLvl != Hie_UNK)
	{
	 HTM_FIELDSET_Begin ("class=\"CON CON_%s\"",The_GetSuffix ());
	    HTM_LEGEND (Txt_Connected_PLURAL);
	    Con_ShowConnectedUsrsBelongingToScope (HieLvl,AllowedLvls);
	 HTM_FIELDSET_End ();
	}

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************** Put icon to update connected users *********************/
/*****************************************************************************/

static void Con_PutIconToUpdateConnected (void *Scope)
  {
   Ico_PutContextualIconToUpdate (ActLstCon,NULL,
                                  Con_PutParScope,Scope);
  }

static void Con_PutParScope (void *Scope)
  {
   Sco_PutParScope ("ScopeCon",*((Hie_Level_t *) Scope));
  }

/*****************************************************************************/
/******************** Show total number of connected users *******************/
/*****************************************************************************/

void Con_ShowGlobalConnectedUsrs (void)
  {
   extern const char *Txt_Sessions;
   extern const char *Txt_session;
   extern const char *Txt_sessions;
   extern const char *Txt_user[Usr_NUM_SEXS];
   extern const char *Txt_users[Usr_NUM_SEXS];
   Rol_Role_t Role;
   unsigned NumUsrs[Rol_NUM_ROLES];
   unsigned NumUsrsTotal;

   /***** Get number of connected users *****/
   for (Role  = Rol_GST, NumUsrsTotal = 0;
	Role <= Rol_SYS_ADM;
	Role++)
     {
      NumUsrs[Role] = Con_GetConnectedUsrsTotal (Role);
      NumUsrsTotal += NumUsrs[Role];
     }

   /***** Number of sessions *****/
   /* Link to view more details about connected users */
   Frm_BeginForm (ActLstCon);
      HTM_BUTTON_Submit_Begin (Act_GetActionText (ActLstCon),"class=\"BT_LINK\"");

	 /* Write total number of sessions */
         HTM_Unsigned (Gbl.Session.NumSessions);
	 HTM_NBSPTxt (Gbl.Session.NumSessions == 1 ? Txt_session :
						     Txt_sessions);

      HTM_BUTTON_End ();
   Frm_EndForm ();

   if (NumUsrsTotal)
     {
      HTM_DIV_Begin ("class=\"CON_LIST\"");

	 /***** Write total number of users *****/
         HTM_Unsigned (NumUsrsTotal);
         HTM_NBSP ();
         HTM_TxtColon (NumUsrsTotal == 1 ? Txt_user[Usr_SEX_UNKNOWN] :
					   Txt_users[Usr_SEX_UNKNOWN]);

	 /***** Write total number of users with each role *****/
	 for (Role  = Rol_GST, NumUsrsTotal = 0;
	      Role <= Rol_SYS_ADM;
	      Role++)
	    Con_ShowGlobalConnectedUsrsRole (Role,NumUsrs[Role]);

      HTM_DIV_End ();
     }
  }

static void Con_ShowGlobalConnectedUsrsRole (Rol_Role_t Role,unsigned UsrsTotal)
  {
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   if (UsrsTotal)
     {
      HTM_BR ();
      HTM_Unsigned (UsrsTotal);
      HTM_NBSPTxt (UsrsTotal == 1 ? Txt_ROLES_SINGUL_abc[Role][Usr_SEX_UNKNOWN] :
			            Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN]);
     }
  }

/*****************************************************************************/
/*********** Compute connected users who belong to current course ************/
/*****************************************************************************/

void Con_ComputeConnectedUsrsBelongingToCurrentCrs (void)
  {
   if ((Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN) &&			// Right column visible
       Gbl.Hierarchy.HieLvl == Hie_CRS &&				// Course selected
       (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] == Usr_BELONG ||	// I can view users
        Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM))
     {
      Con_Connected.NumUsrs       =
      Con_Connected.NumUsrsToList = 0;

      /***** Number of teachers, non-editing teachers, and students *****/
      Con_ComputeConnectedUsrsWithARoleBelongingToCurrentCrs (Rol_TCH);
      Con_ComputeConnectedUsrsWithARoleBelongingToCurrentCrs (Rol_NET);
      Con_ComputeConnectedUsrsWithARoleBelongingToCurrentCrs (Rol_STD);
     }
  }

/*****************************************************************************/
/** Compute number of connected users of a type who belong to current course */
/*****************************************************************************/

static void Con_ComputeConnectedUsrsWithARoleBelongingToCurrentCrs (Rol_Role_t Role)
  {
   /***** Get number of connected users who belong to current course *****/
   Con_GetNumConnectedWithARole (Hie_CRS,Role,&Con_Connected.Usrs[Role]);

   /***** Get list connected users belonging to this course *****/
   Con_ComputeConnectedUsrsWithARoleCurrentCrsOneByOne (Role,
							&Con_Connected.NumUsrs,
							&Con_Connected.NumUsrsToList);
  }

/*****************************************************************************/
/******** Show number of connected users who belong to a given scope *********/
/*****************************************************************************/

static void Con_ShowConnectedUsrsBelongingToScope (Hie_Level_t HieLvl,
						   unsigned AllowedLvls)
  {
   extern const char *Txt_from;
   struct Con_ConnectedUsrs Usrs;

   /***** Number of connected users who belong to scope *****/
   Con_GetNumConnectedWithARole (HieLvl,Rol_UNK,&Usrs);

   /* Write number of connected users */
   HTM_TxtF ("%u %s ",Usrs.NumUsrs,Txt_from);

   /* Put form to change scope */
   Frm_BeginForm (ActLstCon);
      Sco_PutSelectorScope ("ScopeCon",HTM_SUBMIT_ON_CHANGE,
			    HieLvl,AllowedLvls);
   Frm_EndForm ();

   /***** Number of teachers and students *****/
   HTM_TABLE_Begin ("CON_LIST");
      Con_ShowConnectedUsrsWithARoleBelongingToScopeOnMainZone (HieLvl,Rol_TCH);
      Con_ShowConnectedUsrsWithARoleBelongingToScopeOnMainZone (HieLvl,Rol_NET);
      Con_ShowConnectedUsrsWithARoleBelongingToScopeOnMainZone (HieLvl,Rol_STD);
      if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	 Con_ShowConnectedUsrsWithARoleBelongingToScopeOnMainZone (HieLvl,Rol_GST);
   HTM_TABLE_End ();

   /***** Put link to register students *****/
   Enr_CheckStdsAndPutButtonToEnrolStdsInCurrentCrs ();
  }

/*****************************************************************************/
/* Show number of connected users who belong to current course on right col. */
/*****************************************************************************/

void Con_ShowConnectedUsrsBelongingToCurrentCrs (void)
  {
   extern const char *Txt_from;
   char CourseName[Nam_MAX_BYTES_SHRT_NAME + 1];
   struct Con_ConnectedUsrs Usrs;
   unsigned NumUsr = 0;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)	// No course selected
      return;

   /***** Number of connected users who belong to course *****/
   /* Link to view more details about connected users */
   Frm_BeginForm (ActLstCon);
      HTM_BUTTON_Submit_Begin (Act_GetActionText (ActLstCon),"class=\"BT_LINK\"");
	 Str_Copy (CourseName,Gbl.Hierarchy.Node[Hie_CRS].ShrtName,sizeof (CourseName) - 1);
	 Con_GetNumConnectedWithARole (Hie_CRS,Rol_UNK,&Usrs);
	 HTM_TxtF ("%u %s %s",Usrs.NumUsrs,Txt_from,CourseName);
      HTM_BUTTON_End ();
   Frm_EndForm ();

   /***** Number of teachers and students *****/
   HTM_TABLE_Begin ("CON_LIST");
      Con_Connected.NumUsrs       = 0;
      Con_Connected.NumUsrsToList = 0;
      Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_TCH,&NumUsr);
      Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_NET,&NumUsr);
      Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_STD,&NumUsr);
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/* Show number of connected users with a role who belong to current location */
/*****************************************************************************/

static void Con_ShowConnectedUsrsWithARoleBelongingToScopeOnMainZone (Hie_Level_t HieLvl,
								      Rol_Role_t Role)
  {
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   struct Con_ConnectedUsrs Usrs;

   /***** Write number of connected users who belong to current course *****/
   Con_GetNumConnectedWithARole (HieLvl,Role,&Usrs);
   if (Usrs.NumUsrs)
     {
      HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("colspan=\"3\" class=\"CT\"");
	    HTM_Unsigned (Usrs.NumUsrs);
	    HTM_NBSPTxt (Usrs.NumUsrs == 1 ? Txt_ROLES_SINGUL_abc[Role][Usrs.Sex] :
					     Txt_ROLES_PLURAL_abc[Role][Usrs.Sex]);
	 HTM_TD_End ();
      HTM_TR_End ();

      /***** I can see connected users *****/
      Con_ShowConnectedUsrsCurrentLocationOneByOneOnMainZone (HieLvl,Role);
     }
  }

/*****************************************************************************/
/** Show number of connected users with a role who belong to current course **/
/*****************************************************************************/

static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_Role_t Role,unsigned *NumUsr)
  {
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   unsigned NumUsrsThisRole = Con_Connected.Usrs[Role].NumUsrs;
   Usr_Sex_t UsrSex = Con_Connected.Usrs[Role].Sex;

   if (NumUsrsThisRole)
     {
      /***** Write number of connected users who belong to current course *****/
      Con_Connected.NumUsrs       += NumUsrsThisRole;
      Con_Connected.NumUsrsToList += NumUsrsThisRole;
      if (Con_Connected.NumUsrsToList > Cfg_MAX_CONNECTED_SHOWN)
	 Con_Connected.NumUsrsToList = Cfg_MAX_CONNECTED_SHOWN;

      HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("colspan=\"3\" class=\"CON_USR_NARROW_TIT\"");
	    HTM_Unsigned (NumUsrsThisRole);
	    HTM_NBSPTxt (NumUsrsThisRole == 1 ? Txt_ROLES_SINGUL_abc[Role][UsrSex] :
						Txt_ROLES_PLURAL_abc[Role][UsrSex]);
	 HTM_TD_End ();
      HTM_TR_End ();

      /***** I can see connected users *****/
      Con_ShowConnectedUsrsCurrentCrsOneByOneOnRightColumn (Role,NumUsr);

      /***** Write message with number of users not listed *****/
      if (Con_Connected.NumUsrsToList < Con_Connected.NumUsrs)
	{
	 HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"3\" class=\"CM\"");
	       Frm_BeginForm (ActLstCon);
		  Sco_PutParScope ("ScopeCon",Hie_CRS);
		  HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,"ellipsis-h.svg",
		                   Act_GetActionText (ActLstCon),
				   "class=\"ICO16x16 ICO_HIGHLIGHT ICO_BLACK_%s\"",
				   The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();
	 HTM_TR_End ();
	}
     }
  }

/*****************************************************************************/
/********************* Get connected users with a role ***********************/
/*****************************************************************************/

static unsigned Con_GetConnectedUsrsTotal (Rol_Role_t Role)
  {
   if (!DB_CheckIfDatabaseIsOpen ())
      return 0;

   /***** Get number of connected users with a role from database *****/
   return Con_DB_GetConnectedUsrsTotal (Role);
  }

/*****************************************************************************/
/************** Get connected users belonging to current scope ***************/
/*****************************************************************************/
// Return user's sex in UsrSex

static void Con_GetNumConnectedWithARole (Hie_Level_t HieLvl,Rol_Role_t Role,
					  struct Con_ConnectedUsrs *Usrs)
  {
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumSexs;
   Usr_Sex_t Sex;

   /***** Default values *****/
   Usrs->NumUsrs = 0;
   Usrs->Sex = Usr_SEX_UNKNOWN;

   /***** Get number of connected users who belong to current course from database *****/
   if (Con_DB_GetNumConnected (&mysql_res,HieLvl,Role))
     {
      row = mysql_fetch_row (mysql_res);

      /***** Get number of users (row[0]) *****/
      if (sscanf (row[0],"%u",&(Usrs->NumUsrs)) != 1)
	 Err_ShowErrorAndExit ("Error when getting number of connected users who belong to this location.");

      /***** Get number of distinct sexs (row[1]) *****/
      if (sscanf (row[1],"%u",&NumSexs) != 1)
	 Err_ShowErrorAndExit ("Error when getting number of sexs in connected users who belong to this location.");

      /***** Get users' sex (row[2]) *****/
      if (NumSexs == 1)
	 for (Sex  = (Usr_Sex_t) 0;
	      Sex <= (Usr_Sex_t) (Usr_NUM_SEXS - 1);
	      Sex++)
	    if (!strcasecmp (row[2],Usr_StringsSexDB[Sex]))
	      {
	       Usrs->Sex = Sex;
	       break;
	      }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Compute connected users one by one **********************/
/*****************************************************************************/

static void Con_ComputeConnectedUsrsWithARoleCurrentCrsOneByOne (Rol_Role_t Role,
								 unsigned *NumUsrsConnected,
								 unsigned *NumUsrsToList)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr = *NumUsrsConnected;	// Save current number of users

   /***** Get connected users who belong to current course from database *****/
   NumUsrs = Con_DB_GetConnectedFromScope (&mysql_res,Hie_CRS,Role);

   *NumUsrsConnected += NumUsrs;
   *NumUsrsToList    += NumUsrs;
   if (*NumUsrsToList > Cfg_MAX_CONNECTED_SHOWN)
      *NumUsrsToList = Cfg_MAX_CONNECTED_SHOWN;

   /***** Write list of connected users *****/
   for (;
	NumUsr < *NumUsrsToList;
	NumUsr++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get user code (row[0]) */
      Con_Connected.Lst[NumUsr].UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get course code (row[1]) */
      Con_Connected.Lst[NumUsr].ThisCrs = (Str_ConvertStrCodToLongCod (row[1]) ==
	                                   Gbl.Hierarchy.Node[Hie_CRS].HieCod);

      /* Compute elapsed time from last access */
      if (sscanf (row[2],"%ld",&Con_Connected.Lst[NumUsr].TimeDiff) != 1)
         Con_Connected.Lst[NumUsr].TimeDiff = (time_t) 0;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Show connected users one by one *************************/
/*****************************************************************************/

static void Con_ShowConnectedUsrsCurrentCrsOneByOneOnRightColumn (Rol_Role_t Role,unsigned *NumUsr)
  {
   /***** Write list of connected users *****/
   for (;
	(*NumUsr) < Con_Connected.NumUsrsToList;
	(*NumUsr)++)
      /* Write row in screen */
      Con_WriteRowConnectedUsrOnRightColumn (Role,*NumUsr);
  }

/*****************************************************************************/
/********************* Write the name of a connected user ********************/
/*****************************************************************************/

static void Con_WriteRowConnectedUsrOnRightColumn (Rol_Role_t Role,unsigned NumUsr)
  {
   static Act_Action_t NextAction[Rol_NUM_ROLES] =
     {
      [Rol_STD] = ActSeeRecOneStd,
      [Rol_NET] = ActSeeRecOneTch,
      [Rol_TCH] = ActSeeRecOneTch,
     };
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   const char *ClassTxt;
   long UsrCod;
   Usr_MeOrOther_t MeOrOther;
   struct Usr_Data *UsrDat = NULL;	// To avoid warning
   struct Usr_Data OtherUsrDat;

   /***** Get user's code from list *****/
   UsrCod = Con_Connected.Lst[NumUsr].UsrCod;
   MeOrOther = Usr_ItsMe (UsrCod);

   switch (MeOrOther)
     {
      case Usr_ME:
         UsrDat = &Gbl.Usrs.Me.UsrDat;
	 break;
      case Usr_OTHER:
	 /***** Initialize structure with user's data *****/
	 OtherUsrDat.UsrCod = UsrCod;
	 Usr_UsrDataConstructor (&OtherUsrDat);

	 /***** Get user's data *****/
	 Usr_GetAllUsrDataFromUsrCod (&OtherUsrDat,
				      Usr_DONT_GET_PREFS,
				      Usr_DONT_GET_ROLE_IN_CRS);

	 UsrDat = &OtherUsrDat;
	 break;
     }

   HTM_TR_Begin (NULL);

      /***** Show photo *****/
      HTM_TD_Begin ("class=\"CON_PHOTO %s\"",The_GetColorRows ());
	 Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                            ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
      HTM_TD_End ();

      /***** Write full name and link *****/
      ClassTxt = (Con_Connected.Lst[NumUsr].ThisCrs) ? "CON_NAME_NARROW CON_CRS" :
						       "CON_NAME_NARROW CON_NO_CRS";
      HTM_TD_Begin ("class=\"%s %s\"",ClassTxt,The_GetColorRows ());
         if (!NextAction[Role])
	    Err_WrongRoleExit ();
         Frm_BeginForm (NextAction[Role]);
	    Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);

	    HTM_DIV_Begin ("class=\"CON_NAME_NARROW\"");	// Limited width
	       HTM_BUTTON_Submit_Begin (Act_GetActionText (NextAction[Role]),
	                                "class=\"LT BT_LINK\"");
		  Usr_WriteFirstNameBRSurnames (UsrDat);
	       HTM_BUTTON_End ();
	    HTM_DIV_End ();

	 Frm_EndForm ();
      HTM_TD_End ();

      /***** Write time from last access *****/
      ClassTxt = (Con_Connected.Lst[NumUsr].ThisCrs ? "CON_SINCE CON_CRS" :
						      "CON_SINCE CON_NO_CRS");
      HTM_TD_Begin ("class=\"%s %s\"",ClassTxt,The_GetColorRows ());
	 HTM_DIV_Begin ("id=\"hm%u\"",NumUsr);	// Used for automatic update, only when displayed on right column
	    Dat_WriteHoursMinutesSecondsFromSeconds (Con_Connected.Lst[NumUsr].TimeDiff);
	 HTM_DIV_End ();			// Used for automatic update, only when displayed on right column
      HTM_TD_End ();

   HTM_TR_End ();

   switch (MeOrOther)
     {
      case Usr_ME:
	 break;
      case Usr_OTHER:
	 /***** Free memory used for user's data *****/
         Usr_UsrDataDestructor (&OtherUsrDat);
         break;
     }

   The_ChangeRowColor ();
  }

/*****************************************************************************/
/******************** Show connected users one by one ************************/
/*****************************************************************************/

static void Con_ShowConnectedUsrsCurrentLocationOneByOneOnMainZone (Hie_Level_t HieLvl,
								    Rol_Role_t Role)
  {
   static Act_Action_t NextAction[Rol_NUM_ROLES] =
     {
      [Rol_STD] = ActSeeRecOneStd,
      [Rol_NET] = ActSeeRecOneTch,
      [Rol_TCH] = ActSeeRecOneTch,
     };
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs = 0;	// Initialized to avoid warning
   unsigned NumUsr;
   bool ThisCrs;
   time_t TimeDiff;
   const char *ClassTxt;
   struct Usr_Data UsrDat;
   Frm_PutForm_t PutFormRecord = (Gbl.Hierarchy.HieLvl == Hie_CRS &&	// Course selected
	                          HieLvl == Hie_CRS &&			// Scope is current course
	                          (Role == Rol_STD ||			// Role is student,...
	                           Role == Rol_NET ||			// ...non-editing teacher...
	                           Role == Rol_TCH)) ? Frm_PUT_FORM :	// ...or teacher
	                        		       Frm_DONT_PUT_FORM;

   /***** Get connected users who belong to current location from database *****/
   if ((NumUsrs = Con_DB_GetConnectedFromScope (&mysql_res,HieLvl,Role)))
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Write list of connected users *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
        {
         row = mysql_fetch_row (mysql_res);

         /* Get user's data */
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,	// Existing user
                                                      Usr_DONT_GET_PREFS,
                                                      Usr_DONT_GET_ROLE_IN_CRS))
           {
	    /* Get course code (row[1]) */
	    ThisCrs = (Str_ConvertStrCodToLongCod (row[1]) == Gbl.Hierarchy.Node[Hie_CRS].HieCod);

	    /* Compute time from last access */
	    if (sscanf (row[2],"%ld",&TimeDiff) != 1)
	       TimeDiff = (time_t) 0;

	    HTM_TR_Begin (NULL);

	       /***** Show photo *****/
	       HTM_TD_Begin ("class=\"CON_PHOTO %s\"",The_GetColorRows ());
		  Pho_ShowUsrPhotoIfAllowed (&UsrDat,
		                             ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
	       HTM_TD_End ();

	       /***** Write full name and link *****/
	       ClassTxt = ThisCrs ? "CON_NAME_WIDE CON_CRS" :
			            "CON_NAME_WIDE CON_NO_CRS";
	       HTM_TD_Begin ("class=\"%s %s\"",
	                     ClassTxt,The_GetColorRows ());

		  if (PutFormRecord == Frm_PUT_FORM)
		    {
		     if (!NextAction[Role])
			Err_WrongRoleExit ();
		     Frm_BeginForm (NextAction[Role]);
		     Usr_PutParUsrCodEncrypted (UsrDat.EnUsrCod);
			HTM_BUTTON_Submit_Begin (UsrDat.FullName,
						 "class=\"LT BT_LINK\"");
		    }
		  Usr_WriteFirstNameBRSurnames (&UsrDat);
		  if (PutFormRecord == Frm_PUT_FORM)
		    {
		        HTM_BUTTON_End ();
		     Frm_EndForm ();
                    }

	       HTM_TD_End ();

	       /***** Write time from last access *****/
	       ClassTxt = ThisCrs ? "CON_SINCE CON_CRS" :
				    "CON_SINCE CON_NO_CRS";
	       HTM_TD_Begin ("class=\"%s %s\"",
	                     ClassTxt,The_GetColorRows ());
		  Dat_WriteHoursMinutesSecondsFromSeconds (TimeDiff);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    The_ChangeRowColor ();
	   }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****** Write script to automatically update clocks of connected users *******/
/*****************************************************************************/

void Con_WriteScriptClockConnected (void)
  {
   unsigned NumUsr;

   HTM_TxtF ("\tNumUsrsCon = %u;\n",Con_Connected.NumUsrsToList);
   for (NumUsr = 0;
	NumUsr < Con_Connected.NumUsrsToList;
	NumUsr++)
      HTM_TxtF ("\tListSeconds[%u] = %ld;\n",
                NumUsr,Con_Connected.Lst[NumUsr].TimeDiff);
   HTM_Txt ("\twriteClockConnected();\n");
   HTM_TxtF ("\tsetTimeout('refreshConnected()',%lu);\n",
	     Con_GetTimeToRefresh ());
  }
