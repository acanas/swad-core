// swad_connected.c: connected users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Con_PutIconToUpdateConnected (__attribute__((unused)) void *Args);
static void Con_PutParScope (__attribute__((unused)) void *Args);

static void Con_ShowGlobalConnectedUsrsRole (Rol_Role_t Role,unsigned UsrsTotal);

static void Con_ComputeConnectedUsrsWithARoleBelongingToCurrentCrs (Rol_Role_t Role);
static void Con_ShowConnectedUsrsBelongingToLocation (void);

static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_Role_t Role);
static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_Role_t Role);
static unsigned Con_GetConnectedUsrsTotal (Rol_Role_t Role);

static void Con_GetNumConnectedWithARoleBelongingToCurrentScope (Rol_Role_t Role,
                                                                 struct Con_ConnectedUsrs *Usrs);
static void Con_ComputeConnectedUsrsWithARoleCurrentCrsOneByOne (Rol_Role_t Role);
static void Con_ShowConnectedUsrsCurrentCrsOneByOneOnRightColumn (Rol_Role_t Role);
static void Con_WriteRowConnectedUsrOnRightColumn (Rol_Role_t Role);
static void Con_ShowConnectedUsrsCurrentLocationOneByOneOnMainZone (Rol_Role_t Role);

/*****************************************************************************/
/************************** Show connected users *****************************/
/*****************************************************************************/

void Con_ShowConnectedUsrs (void)
  {
   extern const char *Hlp_USERS_Connected;
   extern const char *Txt_Connected_users;
   extern const char *Txt_Sessions;
   extern const char *Txt_Connected_PLURAL;
   char *Title;

   /***** Contextual menu *****/
   if (Gbl.Usrs.Me.Logged)
     {
      Mnu_ContextMenuBegin ();
	 Log_PutLinkToLastClicks ();	// Show last clicks in real time
      Mnu_ContextMenuEnd ();
     }

   /***** Get scope *****/
   Sco_SetAllowedScopesForListingStudents ();
   Sco_GetScope ("ScopeCon",Hie_CRS);

   /***** Begin box *****/
   /* Current time */
   if (asprintf (&Title,"%s"
	                "<div id=\"connected_current_time\">"
	                "</div>",
	         Txt_Connected_users) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (NULL,Title,
                 Con_PutIconToUpdateConnected,NULL,
		 Hlp_USERS_Connected,Box_NOT_CLOSABLE);
   free (Title);

      /***** Current time *****/
      Dat_WriteLocalDateHMSFromUTC ("connected_current_time",Dat_GetStartExecutionTimeUTC (),
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				    false,false,true,0x7);

      /***** Number of connected users in the whole platform *****/
      HTM_FIELDSET_Begin ("class=\"CON CON_%s\"",The_GetSuffix ());
	 HTM_LEGEND (Txt_Sessions);
	 Con_ShowGlobalConnectedUsrs ();
      HTM_FIELDSET_End ();

      /***** Show connected users in the current location *****/
      if (Gbl.Scope.Current != Hie_UNK)
	{
	 HTM_FIELDSET_Begin ("class=\"CON CON_%s\"",The_GetSuffix ());
	    HTM_LEGEND (Txt_Connected_PLURAL);
	    Con_ShowConnectedUsrsBelongingToLocation ();
	 HTM_FIELDSET_End ();
	}

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************** Put icon to update connected users *********************/
/*****************************************************************************/

static void Con_PutIconToUpdateConnected (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToUpdate (ActLstCon,NULL,
                                  Con_PutParScope,NULL);
  }

static void Con_PutParScope (__attribute__((unused)) void *Args)
  {
   Sco_PutParScope ("ScopeCon",Gbl.Scope.Current);
  }

/*****************************************************************************/
/******************** Show total number of connected users *******************/
/*****************************************************************************/

void Con_ShowGlobalConnectedUsrs (void)
  {
   extern const char *Txt_Sessions;
   extern const char *Txt_Connected_users;
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
      HTM_BUTTON_Submit_Begin (Txt_Connected_users,"class=\"BT_LINK\"");

	 /* Write total number of sessions */
	 HTM_TxtF ("%u&nbsp;%s",Gbl.Session.NumSessions,
				Gbl.Session.NumSessions == 1 ? Txt_session :
							       Txt_sessions);

      HTM_BUTTON_End ();
   Frm_EndForm ();

   if (NumUsrsTotal)
     {
      HTM_DIV_Begin ("class=\"CON_LIST\"");

	 /***** Write total number of users *****/
	 HTM_TxtF ("%u&nbsp;%s:",NumUsrsTotal,
				 NumUsrsTotal == 1 ? Txt_user[Usr_SEX_UNKNOWN] :
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
      HTM_TxtF ("%u&nbsp;%s",UsrsTotal,
			     UsrsTotal == 1 ? Txt_ROLES_SINGUL_abc[Role][Usr_SEX_UNKNOWN] :
					      Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN]);
     }
  }

/*****************************************************************************/
/*********** Compute connected users who belong to current course ************/
/*****************************************************************************/

void Con_ComputeConnectedUsrsBelongingToCurrentCrs (void)
  {
   if ((Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN) &&	// Right column visible
       Gbl.Hierarchy.Level == Hie_CRS &&		// Course selected
       (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] ||	// I can view users
        Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM))
     {
      Gbl.Usrs.Connected.NumUsrs       =
      Gbl.Usrs.Connected.NumUsrsToList = 0;
      Gbl.Scope.Current = Hie_CRS;

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
   Con_GetNumConnectedWithARoleBelongingToCurrentScope (Role,&Gbl.Usrs.Connected.Usrs[Role]);

   /***** Get list connected users belonging to this course *****/
   Con_ComputeConnectedUsrsWithARoleCurrentCrsOneByOne (Role);
  }

/*****************************************************************************/
/****** Show number of connected users who belong to current location ********/
/*****************************************************************************/

static void Con_ShowConnectedUsrsBelongingToLocation (void)
  {
   extern const char *Txt_from;
   struct Con_ConnectedUsrs Usrs;

   /***** Number of connected users who belong to scope *****/
   Con_GetNumConnectedWithARoleBelongingToCurrentScope (Rol_UNK,&Usrs);

   /* Write number of connected users */
   HTM_TxtF ("%u %s ",Usrs.NumUsrs,Txt_from);

   /* Put form to change scope */
   Frm_BeginForm (ActLstCon);
      Sco_PutSelectorScope ("ScopeCon",HTM_SUBMIT_ON_CHANGE);
   Frm_EndForm ();

   /***** Number of teachers and students *****/
   HTM_TABLE_Begin ("CON_LIST");
      Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_TCH);
      Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_NET);
      Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_STD);
      if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	 Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_GST);
   HTM_TABLE_End ();

   /***** Put link to register students *****/
   Enr_CheckStdsAndPutButtonToRegisterStdsInCurrentCrs ();
  }

/*****************************************************************************/
/* Show number of connected users who belong to current course on right col. */
/*****************************************************************************/

void Con_ShowConnectedUsrsBelongingToCurrentCrs (void)
  {
   extern const char *Txt_Connected_users;
   extern const char *Txt_from;
   char CourseName[Cns_MAX_BYTES_SHRT_NAME + 1];
   struct Con_ConnectedUsrs Usrs;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)	// No course selected
      return;

   /***** Number of connected users who belong to course *****/
   /* Link to view more details about connected users */
   Frm_BeginForm (ActLstCon);
      HTM_BUTTON_Submit_Begin (Txt_Connected_users,"class=\"BT_LINK\"");
	 Str_Copy (CourseName,Gbl.Hierarchy.Node[Hie_CRS].ShrtName,sizeof (CourseName) - 1);
	 Con_GetNumConnectedWithARoleBelongingToCurrentScope (Rol_UNK,&Usrs);
	 HTM_TxtF ("%u %s %s",Usrs.NumUsrs,Txt_from,CourseName);
      HTM_BUTTON_End ();
   Frm_EndForm ();

   /***** Number of teachers and students *****/
   HTM_TABLE_Begin ("CON_LIST");
      Gbl.Usrs.Connected.NumUsr        = 0;
      Gbl.Usrs.Connected.NumUsrs       = 0;
      Gbl.Usrs.Connected.NumUsrsToList = 0;
      Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_TCH);
      Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_NET);
      Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_STD);
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/* Show number of connected users with a role who belong to current location */
/*****************************************************************************/

static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_Role_t Role)
  {
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   struct Con_ConnectedUsrs Usrs;

   /***** Write number of connected users who belong to current course *****/
   Con_GetNumConnectedWithARoleBelongingToCurrentScope (Role,&Usrs);
   if (Usrs.NumUsrs)
     {
      HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("colspan=\"3\" class=\"CT\"");
	    HTM_TxtF ("%u&nbsp;%s",
	              Usrs.NumUsrs,
		      Usrs.NumUsrs == 1 ? Txt_ROLES_SINGUL_abc[Role][Usrs.Sex] :
					  Txt_ROLES_PLURAL_abc[Role][Usrs.Sex]);
	 HTM_TD_End ();
      HTM_TR_End ();

      /***** I can see connected users *****/
      Con_ShowConnectedUsrsCurrentLocationOneByOneOnMainZone (Role);
     }
  }

/*****************************************************************************/
/** Show number of connected users with a role who belong to current course **/
/*****************************************************************************/

static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_Role_t Role)
  {
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Connected_users;
   unsigned NumUsrsThisRole = Gbl.Usrs.Connected.Usrs[Role].NumUsrs;
   Usr_Sex_t UsrSex = Gbl.Usrs.Connected.Usrs[Role].Sex;

   if (NumUsrsThisRole)
     {
      /***** Write number of connected users who belong to current course *****/
      Gbl.Usrs.Connected.NumUsrs       += NumUsrsThisRole;
      Gbl.Usrs.Connected.NumUsrsToList += NumUsrsThisRole;
      if (Gbl.Usrs.Connected.NumUsrsToList > Cfg_MAX_CONNECTED_SHOWN)
	 Gbl.Usrs.Connected.NumUsrsToList = Cfg_MAX_CONNECTED_SHOWN;

      HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("colspan=\"3\" class=\"CON_USR_NARROW_TIT\"");
	    HTM_TxtF ("%u&nbsp;%s",NumUsrsThisRole,
				   NumUsrsThisRole == 1 ? Txt_ROLES_SINGUL_abc[Role][UsrSex] :
							  Txt_ROLES_PLURAL_abc[Role][UsrSex]);
	 HTM_TD_End ();
      HTM_TR_End ();

      /***** I can see connected users *****/
      Con_ShowConnectedUsrsCurrentCrsOneByOneOnRightColumn (Role);

      /***** Write message with number of users not listed *****/
      if (Gbl.Usrs.Connected.NumUsrsToList < Gbl.Usrs.Connected.NumUsrs)
	{
	 HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"3\" class=\"CM\"");
	       Frm_BeginForm (ActLstCon);
		  Sco_PutParScope ("ScopeCon",Hie_CRS);
		  HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,"ellipsis-h.svg",
		                   Txt_Connected_users,
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

static void Con_GetNumConnectedWithARoleBelongingToCurrentScope (Rol_Role_t Role,
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
   if (Con_DB_GetNumConnectedFromCurrentLocation (&mysql_res,Role))
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

static void Con_ComputeConnectedUsrsWithARoleCurrentCrsOneByOne (Rol_Role_t Role)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr = Gbl.Usrs.Connected.NumUsrs;	// Save current number of users

   /***** Get connected users who belong to current course from database *****/
   NumUsrs = Con_DB_GetConnectedFromCurrentLocation (&mysql_res,Role);

   Gbl.Usrs.Connected.NumUsrs       += NumUsrs;
   Gbl.Usrs.Connected.NumUsrsToList += NumUsrs;
   if (Gbl.Usrs.Connected.NumUsrsToList > Cfg_MAX_CONNECTED_SHOWN)
      Gbl.Usrs.Connected.NumUsrsToList = Cfg_MAX_CONNECTED_SHOWN;

   /***** Write list of connected users *****/
   for (;
	NumUsr < Gbl.Usrs.Connected.NumUsrsToList;
	NumUsr++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get user code (row[0]) */
      Gbl.Usrs.Connected.Lst[NumUsr].UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get course code (row[1]) */
      Gbl.Usrs.Connected.Lst[NumUsr].ThisCrs = (Str_ConvertStrCodToLongCod (row[1]) ==
	                                        Gbl.Hierarchy.Node[Hie_CRS].HieCod);

      /* Compute elapsed time from last access */
      if (sscanf (row[2],"%ld",&Gbl.Usrs.Connected.Lst[NumUsr].TimeDiff) != 1)
         Gbl.Usrs.Connected.Lst[NumUsr].TimeDiff = (time_t) 0;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Show connected users one by one *************************/
/*****************************************************************************/

static void Con_ShowConnectedUsrsCurrentCrsOneByOneOnRightColumn (Rol_Role_t Role)
  {
   /***** Write list of connected users *****/
   for (;
	Gbl.Usrs.Connected.NumUsr < Gbl.Usrs.Connected.NumUsrsToList;
	Gbl.Usrs.Connected.NumUsr++)
      /* Write row in screen */
      Con_WriteRowConnectedUsrOnRightColumn (Role);
  }

/*****************************************************************************/
/********************* Write the name of a connected user ********************/
/*****************************************************************************/

static void Con_WriteRowConnectedUsrOnRightColumn (Rol_Role_t Role)
  {
   extern const char *Txt_View_record_for_this_course;
   static const Act_Action_t NextAction[Rol_NUM_ROLES] =
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
   struct Usr_Data *UsrDat;
   struct Usr_Data OtherUsrDat;

   /***** Get user's code from list *****/
   UsrCod = Gbl.Usrs.Connected.Lst[Gbl.Usrs.Connected.NumUsr].UsrCod;
   MeOrOther = Usr_ItsMe (UsrCod);

   switch (MeOrOther)
     {
      case Usr_ME:
         UsrDat = &Gbl.Usrs.Me.UsrDat;
	 break;
      case Usr_OTHER:
      default:
	 /***** Initialize structure with user's data *****/
	 OtherUsrDat.UsrCod = UsrCod;
	 Usr_UsrDataConstructor (&OtherUsrDat);

	 /***** Get user's data *****/
	 Usr_GetAllUsrDataFromUsrCod (&OtherUsrDat,
				      Usr_DONT_GET_PREFS,
				      Usr_DONT_GET_ROLE_IN_CURRENT_CRS);

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
      ClassTxt = (Gbl.Usrs.Connected.Lst[Gbl.Usrs.Connected.NumUsr].ThisCrs) ? "CON_NAME_NARROW CON_CRS" :
									       "CON_NAME_NARROW CON_NO_CRS";
      HTM_TD_Begin ("class=\"%s %s\"",ClassTxt,The_GetColorRows ());
         if (!NextAction[Role])
	    Err_WrongRoleExit ();
         Frm_BeginForm (NextAction[Role]);
	    Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);

	    HTM_DIV_Begin ("class=\"CON_NAME_NARROW\"");	// Limited width
	       HTM_BUTTON_Submit_Begin (Txt_View_record_for_this_course,
	                                "class=\"LT BT_LINK\"");
		  Usr_WriteFirstNameBRSurnames (UsrDat);
	       HTM_BUTTON_End ();
	    HTM_DIV_End ();

	 Frm_EndForm ();
      HTM_TD_End ();

      /***** Write time from last access *****/
      ClassTxt = (Gbl.Usrs.Connected.Lst[Gbl.Usrs.Connected.NumUsr].ThisCrs ? "CON_SINCE CON_CRS" :
									      "CON_SINCE CON_NO_CRS");
      HTM_TD_Begin ("class=\"%s %s\"",ClassTxt,The_GetColorRows ());
	 HTM_DIV_Begin ("id=\"hm%u\"",Gbl.Usrs.Connected.NumUsr);	// Used for automatic update, only when displayed on right column
	    Dat_WriteHoursMinutesSecondsFromSeconds (Gbl.Usrs.Connected.Lst[Gbl.Usrs.Connected.NumUsr].TimeDiff);
	 HTM_DIV_End ();						// Used for automatic update, only when displayed on right column
      HTM_TD_End ();

   HTM_TR_End ();

   switch (MeOrOther)
     {
      case Usr_ME:
	 break;
      case Usr_OTHER:
      default:
	 /***** Free memory used for user's data *****/
         Usr_UsrDataDestructor (&OtherUsrDat);
         break;
     }

   The_ChangeRowColor ();
  }

/*****************************************************************************/
/******************** Show connected users one by one ************************/
/*****************************************************************************/

static void Con_ShowConnectedUsrsCurrentLocationOneByOneOnMainZone (Rol_Role_t Role)
  {
   static const Act_Action_t NextAction[Rol_NUM_ROLES] =
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
   bool PutLinkToRecord = (Gbl.Hierarchy.Level == Hie_CRS &&	// Course selected
	                   Gbl.Scope.Current   == Hie_CRS &&	// Scope is current course
	                   (Role == Rol_STD ||			// Role is student,...
	                    Role == Rol_NET ||			// ...non-editing teacher...
	                    Role == Rol_TCH));			// ...or teacher

   /***** Get connected users who belong to current location from database *****/
   if ((NumUsrs = Con_DB_GetConnectedFromCurrentLocation (&mysql_res,Role)))
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
                                                      Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
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

		  if (PutLinkToRecord)
		    {
		     if (!NextAction[Role])
			Err_WrongRoleExit ();
		     Frm_BeginForm (NextAction[Role]);
			Usr_PutParUsrCodEncrypted (UsrDat.EnUsrCod);
		    }

		  if (PutLinkToRecord)
		     HTM_BUTTON_Submit_Begin (UsrDat.FullName,
					      "class=\"LT BT_LINK\"");
		  Usr_WriteFirstNameBRSurnames (&UsrDat);
		  if (PutLinkToRecord)
		     HTM_BUTTON_End ();

		  if (PutLinkToRecord)
		     Frm_EndForm ();

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

   HTM_TxtF ("\tNumUsrsCon = %u;\n",Gbl.Usrs.Connected.NumUsrsToList);
   for (NumUsr = 0;
	NumUsr < Gbl.Usrs.Connected.NumUsrsToList;
	NumUsr++)
      HTM_TxtF ("\tListSeconds[%u] = %ld;\n",
                NumUsr,Gbl.Usrs.Connected.Lst[NumUsr].TimeDiff);
   HTM_Txt ("\twriteClockConnected();\n");
  }
