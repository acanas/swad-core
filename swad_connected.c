// swad_connected.c: connected users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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

#include <limits.h>		// For maximum values
#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For fprintf
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_string.h"
#include "swad_user.h"

/*****************************************************************************/
/*************************** Internal constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*************************** Internal prototypes *****************************/
/*****************************************************************************/

static void Con_ComputeConnectedUsrsOfTypeBelongingToCurrentCrs (Rol_Role_t Role);
static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_Role_t Role);
static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnRightColumn (Rol_Role_t Role);
static unsigned Con_GetConnectedGuestsTotal (void);
static unsigned Con_GetConnectedStdsTotal (void);
static unsigned Con_GetConnectedTchsTotal (void);
static unsigned Con_GetNumConnectedUsrsWithARoleBelongingCurrentLocation (Rol_Role_t Role,Usr_Sex_t *UsrSex);
static void Con_ComputeConnectedUsrsWithARoleCurrentCrsOneByOne (Rol_Role_t Role);
static void Con_ShowConnectedUsrsCurrentCrsOneByOneOnRightColumn (Rol_Role_t Role);
static void Con_WriteRowConnectedUsrOnRightColumn (Rol_Role_t Role);
static void Con_ShowConnectedUsrsCurrentLocationOneByOneOnMainZone (Rol_Role_t Role);

/*****************************************************************************/
/************************** Show connected users *****************************/
/*****************************************************************************/

void Con_ShowConnectedUsrs (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Update_connected_users;
   extern const char *Txt_Scope;
   extern const char *Txt_MONTHS_SMALL_SHORT[12];
   extern const char *Txt_Connected_users;

   fprintf (Gbl.F.Out,"<div align=\"center\">");

   /***** Put form to update connected users *****/
   Act_FormStart (ActLstCon);
   /* Users connected belonging to the platform, current degree or current course */
   Gbl.Scope.Current = Sco_SCOPE_COURSE;
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\">%s: ",
	       The_ClassFormul[Gbl.Prefs.Theme],Txt_Scope);
      Sco_SetScopesForListingStudents ();
      Sco_GetScope ();
      Sco_PutSelectorScope (false);
      fprintf (Gbl.F.Out,"</div>");
     }
   Act_LinkFormSubmit (Txt_Update_connected_users,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("recycle",Txt_Update_connected_users,Txt_Update_connected_users);
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");

   /***** Show connected users *****/
   Gbl.Usrs.Connected.WhereToShow = Con_SHOW_ON_MAIN_ZONE;

   /* Current time */
   sprintf (Gbl.Title,"%s<br />%u %s, %u:%02u",
	    Txt_Connected_users,
            Gbl.Now.Date.Day,
            Txt_MONTHS_SMALL_SHORT[Gbl.Now.Date.Month-1],
            Gbl.Now.Time.Hour,
            Gbl.Now.Time.Minute);
   Lay_StartRoundFrameTable10 (NULL,0,Gbl.Title);
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"center\">");

   /* Number of connected users in the whole platform */
   Con_ShowGlobalConnectedUsrs ();

   /* Show connected users in the current course */
   Con_ShowConnectedUsrsBelongingToScope ();

   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/****************************** Show last clicks *****************************/
/*****************************************************************************/

void Con_ShowLastClicks (void)
  {
   Lay_StartRoundFrameTable10 (NULL,0,NULL);
   fprintf (Gbl.F.Out,"<tr>"
	              "<td>");

   fprintf (Gbl.F.Out,"<div align=\"center\" id=\"lastclicks\">");	// Used for AJAX based refresh
   Con_GetAndShowLastClicks ();
   fprintf (Gbl.F.Out,"</div>");					// Used for AJAX based refresh

   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/**************** Get last clicks from database and show them ****************/
/*****************************************************************************/

void Con_GetAndShowLastClicks (void)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   extern Act_Action_t Act_FromActCodToAction[1+Act_MAX_ACTION_COD];
   extern const char *Txt_Click;
   extern const char *Txt_Hour;
   extern const char *Txt_Role;
   extern const char *Txt_Degree;
   extern const char *Txt_Action;
   extern const char *Txt_ROLES_SINGULAR_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRow;
   unsigned NumRows;
   long ActCod;
   const char *ClassRow;
   struct Degree Deg;

   /***** Get last clicks from database *****/
   /* Important for maximum performance: do the LIMIT in the big log table before the JOIN */
   sprintf (Query,"SELECT last_logs.LogCod,last_logs.ActCod,last_logs.T,last_logs.Role,last_logs.DegCod,actions.Txt"
	          " FROM"
	          " (SELECT LogCod,ActCod,DATE_FORMAT(ClickTime,'%%H:%%i:%%S') AS T,Role,DegCod"
                  " FROM log_recent ORDER BY LogCod DESC LIMIT 20)"
                  " AS last_logs,actions"
	          " WHERE last_logs.ActCod=actions.ActCod AND actions.Language='es'");
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get last clicks");

   /***** Write list of connected users *****/
   fprintf (Gbl.F.Out,"<table>");

   /* Print table row */
   fprintf (Gbl.F.Out,"<tr>"
                      "<th align=\"left\" class=\"TIT_TBL\" width=\"70\">%s</th>"
                      "<th align=\"left\" class=\"TIT_TBL\" width=\"60\">%s</th>"
                      "<th align=\"left\" class=\"TIT_TBL\" width=\"100\">%s</th>"
                      "<th align=\"left\" class=\"TIT_TBL\" width=\"150\">%s</th>"
                      "<th align=\"left\" class=\"TIT_TBL\" width=\"220\">%s</th>"
                      "</tr>",
               Txt_Click,
               Txt_Hour,
               Txt_Role,
               Txt_Degree,
               Txt_Action);

   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get action code (row[1]) */
      ActCod = Str_ConvertStrCodToLongCod (row[1]);

      /* Use a special color for this row depending on the action */
      ClassRow = (Act_Actions[Act_FromActCodToAction[ActCod]].BrowserWindow == Act_DOWNLD_FILE) ? "DAT_SMALL_YELLOW" :
	         (ActCod == Act_Actions[ActAutUsrInt].ActCod ||
                  ActCod == Act_Actions[ActAutUsrExt].ActCod) ? "DAT_SMALL_GREEN" :
                 (ActCod == Act_Actions[ActLogOut].ActCod   ) ? "DAT_SMALL_RED" :
                 (ActCod == Act_Actions[ActWebSvc].ActCod   ) ? "DAT_SMALL_BLUE" :
                                                                "DAT_SMALL";

      /* Get degree code (row[4]) */
      Deg.DegCod = Str_ConvertStrCodToLongCod (row[4]);
      Deg_GetShortNameOfDegreeByCod (&Deg);

      /* Print table row */
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"%s\">%s</td>"
                         "<td class=\"%s\">%s</td>"
                         "<td class=\"%s\">%s</td>"
                         "<td class=\"%s\">%s</td>"
                         "<td class=\"%s\">%s</td>"
			 "</tr>",
               ClassRow,row[0],
               ClassRow,row[2],
               ClassRow,Txt_ROLES_SINGULAR_Abc[Rol_ConvertUnsignedStrToRole (row[3])][Usr_SEX_UNKNOWN],
               ClassRow,Deg.ShortName,
	       ClassRow,row[5]);
     }
   fprintf (Gbl.F.Out,"</table>");

   /***** Free structure that stores the query result *****/
   mysql_free_result (mysql_res);
  }

/*****************************************************************************/
/******************** Show total number of connected users *******************/
/*****************************************************************************/

void Con_ShowGlobalConnectedUsrs (void)
  {
   extern const char *Txt_session;
   extern const char *Txt_sessions;
   extern const char *Txt_user[Usr_NUM_SEXS];
   extern const char *Txt_users;
   extern const char *Txt_ROLES_SINGULAR_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   unsigned StdsTotal = Con_GetConnectedStdsTotal ();
   unsigned TchsTotal = Con_GetConnectedTchsTotal ();
   unsigned WithoutCoursesTotal = Con_GetConnectedGuestsTotal ();
   unsigned UsrsTotal = StdsTotal + TchsTotal + WithoutCoursesTotal;


   /***** Start table *****/
   fprintf (Gbl.F.Out,"<div class=\"CONNECTED\" style=\"width:110px;\">"
                      "<table cellpadding=\"0\" width=\"110\">");

   /***** Write total number of sessions *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td width=\"110\" align=\"center\" valign=\"top\">%u %s</td>"
                      "</tr>",
            Gbl.Session.NumSessions,
            (Gbl.Session.NumSessions == 1) ? Txt_session :
        	                             Txt_sessions);

   /***** Write total number of users *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td width=\"110\" align=\"center\" valign=\"top\">%u %s:</td>"
                      "</tr>",
            UsrsTotal,
            (UsrsTotal == 1) ? Txt_user[Usr_SEX_UNKNOWN] :
        	               Txt_users);

   /***** Write total number of students *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td width=\"110\" align=\"center\" valign=\"top\">%u %s</td>"
                      "</tr>",
            StdsTotal,
            (StdsTotal == 1) ? Txt_ROLES_SINGULAR_abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN] :
                               Txt_ROLES_PLURAL_abc  [Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN]);

   /***** Write total number of teachers *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td width=\"110\" align=\"center\" valign=\"top\">%u %s</td>"
                      "</tr>",
            TchsTotal,
            (TchsTotal == 1) ? Txt_ROLES_SINGULAR_abc[Rol_ROLE_TEACHER][Usr_SEX_UNKNOWN] :
                               Txt_ROLES_PLURAL_abc  [Rol_ROLE_TEACHER][Usr_SEX_UNKNOWN]);

   /***** Write total number of users who do not belong to any course *****/
   if (WithoutCoursesTotal)
      fprintf (Gbl.F.Out,"<tr>"
                         "<td width=\"110\" align=\"center\" valign=\"top\">%u %s</td>"
                         "</tr>",
               WithoutCoursesTotal,
               (WithoutCoursesTotal == 1) ? Txt_ROLES_SINGULAR_abc[Rol_ROLE_GUEST][Usr_SEX_UNKNOWN] :
                                            Txt_ROLES_PLURAL_abc  [Rol_ROLE_GUEST][Usr_SEX_UNKNOWN]);

   /***** End table *****/
   fprintf (Gbl.F.Out,"</table>"
	              "</div>");
  }

/*****************************************************************************/
/*********** Compute connected users who belong to current course ************/
/*****************************************************************************/

void Con_ComputeConnectedUsrsBelongingToCurrentCrs (void)
  {
   Gbl.Usrs.Connected.NumUsrs       = 0;
   Gbl.Usrs.Connected.NumUsrsToList = 0;

   /***** Number of teachers *****/
   Con_ComputeConnectedUsrsOfTypeBelongingToCurrentCrs (Rol_ROLE_TEACHER);

   /***** Number of students *****/
   Con_ComputeConnectedUsrsOfTypeBelongingToCurrentCrs (Rol_ROLE_STUDENT);
  }

/*****************************************************************************/
/******** Show number of connected users who belong to current course ********/
/*****************************************************************************/

void Con_ShowConnectedUsrsBelongingToScope (void)
  {
   extern const char *The_ClassConnected[The_NUM_THEMES];
   extern const char *Txt_from;
   char LocationName[Deg_MAX_LENGTH_LOCATION_SHORT_NAME_SPEC_CHAR+1];
   Usr_Sex_t UsrSex;

   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_PLATFORM:		// Show connected users in the whole platform
         break;
      case Sco_SCOPE_INSTITUTION:	// Show connected users in the current institution
         if (Gbl.CurrentIns.Ins.InsCod <= 0)	// There is no institution selected
            return;
         break;
      case Sco_SCOPE_CENTRE:		// Show connected users in the current centre
         if (Gbl.CurrentCtr.Ctr.CtrCod <= 0)	// There is no centre selected
            return;
         break;
      case Sco_SCOPE_DEGREE:		// Show connected users in the current degree
         if (Gbl.CurrentDeg.Deg.DegCod <= 0)	// There is no degree selected
            return;
         break;
      case Sco_SCOPE_COURSE:		// Show connected users in the current course
         if (Gbl.CurrentCrs.Crs.CrsCod <= 0)	// There is no course selected
            return;
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }

   /***** Number of connected users who belong to current course *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_PLATFORM:		// Show connected users in the whole platform
         strcpy (LocationName,Cfg_PLATFORM_SHORT_NAME);
         break;
      case Sco_SCOPE_INSTITUTION:	// Show connected users in the current institution
         strcpy (LocationName,Gbl.CurrentIns.Ins.ShortName);
         break;
      case Sco_SCOPE_CENTRE:		// Show connected users in the current centre
         strcpy (LocationName,Gbl.CurrentCtr.Ctr.ShortName);
         break;
      case Sco_SCOPE_DEGREE:		// Show connected users in the current degree
         strcpy (LocationName,Gbl.CurrentDeg.Deg.ShortName);
         break;
      case Sco_SCOPE_COURSE:		// Show connected users in the current course
         strcpy (LocationName,Gbl.CurrentCrs.Crs.ShortName);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   switch (Gbl.Usrs.Connected.WhereToShow)
     {
      case Con_SHOW_ON_MAIN_ZONE:
         Str_LimitLengthHTMLStr (LocationName,40);
         break;
      case Con_SHOW_ON_RIGHT_COLUMN:
         Str_LimitLengthHTMLStr (LocationName,12);
         Str_ReplaceSpecialCharByCodes (LocationName,Deg_MAX_LENGTH_LOCATION_SHORT_NAME_SPEC_CHAR);
         break;
     }
   fprintf (Gbl.F.Out,"<span class=\"%s\">%u %s %s</span>",
            The_ClassConnected[Gbl.Prefs.Theme],
            Con_GetNumConnectedUsrsWithARoleBelongingCurrentLocation (Rol_ROLE_UNKNOWN,&UsrSex),
            Txt_from,
            LocationName);

   /***** Start table *****/
   switch (Gbl.Usrs.Connected.WhereToShow)
     {
      case Con_SHOW_ON_MAIN_ZONE:
         fprintf (Gbl.F.Out,"<div class=\"CONNECTED\" style=\"width:312px;\">"
                            "<table cellpadding=\"0\">");
	 break;
      case Con_SHOW_ON_RIGHT_COLUMN:
         fprintf (Gbl.F.Out,"<div class=\"CONNECTED\" style=\"width:110px;\">"
                            "<table cellpadding=\"0\" width=\"110\">");
         break;
     }

   switch (Gbl.Usrs.Connected.WhereToShow)
     {
      case Con_SHOW_ON_MAIN_ZONE:
         Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_ROLE_TEACHER);
         Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_ROLE_STUDENT);
         break;
      case Con_SHOW_ON_RIGHT_COLUMN:
         Gbl.Usrs.Connected.NumUsr        = 0;
         Gbl.Usrs.Connected.NumUsrs       = 0;
         Gbl.Usrs.Connected.NumUsrsToList = 0;
         Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnRightColumn (Rol_ROLE_TEACHER);
         Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnRightColumn (Rol_ROLE_STUDENT);
         break;
     }

   /***** End table *****/
   fprintf (Gbl.F.Out,"</table>"
		      "</div>");
  }

/*****************************************************************************/
/** Compute number of connected users of a type who belong to current course */
/*****************************************************************************/

static void Con_ComputeConnectedUsrsOfTypeBelongingToCurrentCrs (Rol_Role_t Role)
  {
   Gbl.Scope.Current = Sco_SCOPE_COURSE;

   /***** Get number of connected users who belong to current course *****/
   switch (Role)
     {
      case Rol_ROLE_TEACHER:
         Gbl.Usrs.Connected.NumTchs = Con_GetNumConnectedUsrsWithARoleBelongingCurrentLocation (Role,&Gbl.Usrs.Connected.SexTchs);
         break;
      case Rol_ROLE_STUDENT:
         Gbl.Usrs.Connected.NumStds = Con_GetNumConnectedUsrsWithARoleBelongingCurrentLocation (Role,&Gbl.Usrs.Connected.SexStds);
         break;
      default:
         return;
     }

   /***** List connected users belonging to this course *****/
   if (Gbl.Usrs.Me.IHaveAccessToCurrentCrs)	// If I belongs to this course or I am superuser
      Con_ComputeConnectedUsrsWithARoleCurrentCrsOneByOne (Role);
  }

/*****************************************************************************/
/* Show number of connected users with a role who belong to current location */
/*****************************************************************************/

static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_Role_t Role)
  {
   extern const char *Txt_ROLES_SINGULAR_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   unsigned NumUsrsThisRole;
   Usr_Sex_t UsrSex;

   /***** Write number of connected users who belong to current course *****/
   NumUsrsThisRole = Con_GetNumConnectedUsrsWithARoleBelongingCurrentLocation (Role,&UsrSex);
   fprintf (Gbl.F.Out,"<tr>"
                      "<td colspan=\"3\" align=\"center\" valign=\"top\">"
                      "%u %s"
                      "</td>"
                      "</tr>",
            NumUsrsThisRole,
            (NumUsrsThisRole == 1) ? Txt_ROLES_SINGULAR_abc[Role][UsrSex] :
                                     Txt_ROLES_PLURAL_abc  [Role][UsrSex]);

   /***** List connected users belonging to this location *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_PLATFORM:		// Show connected users in the whole platform
      case Sco_SCOPE_INSTITUTION:	// Show connected users in the current centre
      case Sco_SCOPE_CENTRE:		// Show connected users in the current centre
         if (Gbl.Usrs.Me.LoggedRole != Rol_ROLE_SUPERUSER)
            return;
         break;
      case Sco_SCOPE_DEGREE:		// Show connected users in the current degree
         if (!(Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN ||
               Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER))
            return;
         break;
      case Sco_SCOPE_COURSE:		// Show connected users in the current course
         if (!Gbl.Usrs.Me.IHaveAccessToCurrentCrs)
            return;
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }

   /***** I can see connected users *****/
   Con_ShowConnectedUsrsCurrentLocationOneByOneOnMainZone (Role);
  }

/*****************************************************************************/
/* Show number of connected users with a role who belong to current location */
/*****************************************************************************/

static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnRightColumn (Rol_Role_t Role)
  {
   extern const char *The_ClassConnected[The_NUM_THEMES];
   extern const char *Txt_ROLES_SINGULAR_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Connected_users;
   unsigned NumUsrsThisRole;
   Usr_Sex_t UsrSex;

   /***** Write number of connected users who belong to current course *****/
   switch (Role)
     {
      case Rol_ROLE_TEACHER:
         NumUsrsThisRole = Gbl.Usrs.Connected.NumTchs;
         UsrSex = Gbl.Usrs.Connected.SexTchs;
         break;
      case Rol_ROLE_STUDENT:
         NumUsrsThisRole = Gbl.Usrs.Connected.NumStds;
         UsrSex = Gbl.Usrs.Connected.SexStds;
         break;
      default:
         return;
     }
   Gbl.Usrs.Connected.NumUsrs       += NumUsrsThisRole;
   Gbl.Usrs.Connected.NumUsrsToList += NumUsrsThisRole;
   if (Gbl.Usrs.Connected.NumUsrsToList > Cfg_MAX_CONNECTED_SHOWN)
      Gbl.Usrs.Connected.NumUsrsToList = Cfg_MAX_CONNECTED_SHOWN;
   fprintf (Gbl.F.Out,"<tr>"
                      "<td colspan=\"3\" width=\"110\" align=\"center\" valign=\"top\">"
                      "%u %s"
                      "</td>"
                      "</tr>",
            NumUsrsThisRole,
            (NumUsrsThisRole == 1) ? Txt_ROLES_SINGULAR_abc[Role][UsrSex] :
                                     Txt_ROLES_PLURAL_abc  [Role][UsrSex]);

   /***** List connected users belonging to this location *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_PLATFORM:		// Show connected users in the whole platform
      case Sco_SCOPE_INSTITUTION:	// Show connected users in the current institution
      case Sco_SCOPE_CENTRE:		// Show connected users in the current centre
         if (Gbl.Usrs.Me.LoggedRole != Rol_ROLE_SUPERUSER)
            return;
         break;
      case Sco_SCOPE_DEGREE:		// Show connected users in the current degree
         if (!(Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN ||
               Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER))
            return;
         break;
      case Sco_SCOPE_COURSE:		// Show connected users in the current course
         if (!Gbl.Usrs.Me.IHaveAccessToCurrentCrs)
            return;
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }

   /***** I can see connected users *****/
   Con_ShowConnectedUsrsCurrentCrsOneByOneOnRightColumn (Role);

   /***** Write message with number of users not listed *****/
   if (Gbl.Usrs.Connected.NumUsrsToList < Gbl.Usrs.Connected.NumUsrs)
     {
      sprintf (Gbl.FormId,"form_con_%d",++Gbl.NumFormConnectedUsrs);
      fprintf (Gbl.F.Out,"<tr>"
			 "<td align=\"center\" colspan=\"3\">");
      Act_FormStartId (ActLstCon,Gbl.FormId);
      Sco_PutParamScope (Sco_SCOPE_COURSE);
      Act_LinkFormSubmitId (Txt_Connected_users,The_ClassConnected[Gbl.Prefs.Theme],Gbl.FormId);
      fprintf (Gbl.F.Out,"<img src=\"%s/ellipsis32x32.gif\""
	                 " alt=\"%s\" class=\"ICON32x32\" /></a>"
	                 "</form>"
			 "</td>"
			 "</tr>",
	       Gbl.Prefs.IconsURL,
	       Txt_Connected_users);
     }
  }

/*****************************************************************************/
/********************* Modify my entry in connected list *********************/
/*****************************************************************************/

void Con_UpdateMeInConnectedList (void)
  {
   char Query[512];
   Rol_Role_t MyRoleInConnected;

   /***** Which role will be stored in connected table? *****/
   MyRoleInConnected = (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_STUDENT ||
                        Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER) ? Gbl.Usrs.Me.LoggedRole :
                                                                      Gbl.Usrs.Me.MaxRole;

   /***** Update my entry in connected list. The role which is stored is the role of the last click *****/
   sprintf (Query,"REPLACE INTO connected (UsrCod,RoleInLastCrs,LastCrsCod,LastTime)"
                  " VALUES ('%ld','%u','%ld',NOW())",
            Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) MyRoleInConnected,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryREPLACE (Query,"can not update list of connected users");
  }

/*****************************************************************************/
/************************** Remove old connected uses ************************/
/*****************************************************************************/

void Con_RemoveOldConnected (void)
  {
   char Query[512];

   /***** Remove old users from connected list *****/
   sprintf (Query,"DELETE FROM connected WHERE UsrCod NOT IN"
                  " (SELECT DISTINCT(UsrCod) FROM sessions)");
   DB_QueryDELETE (Query,"can not remove old users from list of connected users");
  }

/*****************************************************************************/
/*************************** Get connected guests ***************************/
/*****************************************************************************/

static unsigned Con_GetConnectedGuestsTotal (void)
  {
   char Query[128];

   if (!Gbl.DB.DatabaseIsOpen)
      return 0;

   /***** Get number of connected users not belonging to any course *****/
   sprintf (Query,"SELECT COUNT(*) FROM connected"
	          " WHERE RoleInLastCrs='%u'",
            (unsigned) Rol_ROLE_GUEST);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of connected users who not belong to any course");
  }

/*****************************************************************************/
/*************************** Get connected students **************************/
/*****************************************************************************/

static unsigned Con_GetConnectedStdsTotal (void)
  {
   char Query[128];

   if (!Gbl.DB.DatabaseIsOpen)
      return 0;

   /***** Get number of connected students from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM connected WHERE RoleInLastCrs='%u'",
            (unsigned) Rol_ROLE_STUDENT);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of connected students");
  }

/*****************************************************************************/
/*************************** Get connected teachers **************************/
/*****************************************************************************/

static unsigned Con_GetConnectedTchsTotal (void)
  {
   char Query[256];

   if (!Gbl.DB.DatabaseIsOpen)
      return 0;

   /***** Get number of connected teachers from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM connected WHERE RoleInLastCrs='%u'",
            (unsigned) Rol_ROLE_TEACHER);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of connected teachers");
  }

/*****************************************************************************/
/************** Get connected users belonging to current course **************/
/*****************************************************************************/
// Return user's sex in UsrSex

static unsigned Con_GetNumConnectedUsrsWithARoleBelongingCurrentLocation (Rol_Role_t Role,Usr_Sex_t *UsrSex)
  {
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumSexs;
   Usr_Sex_t Sex;

   /***** Get number of connected users who belong to current course from database *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_PLATFORM:		// Show connected users in the whole platform
         if (Role == Rol_ROLE_UNKNOWN)	// Here Rol_ROLE_UNKNOWN means "any role"
            sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
                           " FROM connected,usr_data"
                           " WHERE connected.UsrCod=usr_data.UsrCod");
         else
            sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
                           " FROM connected,crs_usr,usr_data"
                           " WHERE connected.UsrCod=crs_usr.UsrCod"
                           " AND crs_usr.Role='%u'"
                           " AND connected.UsrCod=usr_data.UsrCod",
                     (unsigned) Role);
         break;
      case Sco_SCOPE_INSTITUTION:	// Show connected users in the current institution
         if (Role == Rol_ROLE_UNKNOWN)	// Here Rol_ROLE_UNKNOWN means "any role"
            sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
                           " FROM centres,degrees,courses,crs_usr,connected,usr_data"
                           " WHERE centres.InsCod='%ld'"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=connected.UsrCod"
                           " AND connected.UsrCod=usr_data.UsrCod",
                     Gbl.CurrentIns.Ins.InsCod);
         else
            sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
                           " FROM centres,degrees,courses,crs_usr,connected,usr_data"
                           " WHERE centres.InsCod='%ld'"
                           " AND centres.CtrCod=degrees.CtrCod"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.Role='%u'"
                           " AND crs_usr.UsrCod=connected.UsrCod"
                           " AND connected.UsrCod=usr_data.UsrCod",
                     Gbl.CurrentIns.Ins.InsCod,
                     (unsigned) Role);
         break;
      case Sco_SCOPE_CENTRE:		// Show connected users in the current centre
         if (Role == Rol_ROLE_UNKNOWN)	// Here Rol_ROLE_UNKNOWN means "any role"
            sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
                           " FROM degrees,courses,crs_usr,connected,usr_data"
                           " WHERE degrees.CtrCod='%ld'"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=connected.UsrCod"
                           " AND connected.UsrCod=usr_data.UsrCod",
                     Gbl.CurrentCtr.Ctr.CtrCod);
         else
            sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
                           " FROM degrees,courses,crs_usr,connected,usr_data"
                           " WHERE degrees.CtrCod='%ld'"
                           " AND degrees.DegCod=courses.DegCod"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.Role='%u'"
                           " AND crs_usr.UsrCod=connected.UsrCod"
                           " AND connected.UsrCod=usr_data.UsrCod",
                     Gbl.CurrentCtr.Ctr.CtrCod,
                     (unsigned) Role);
         break;
      case Sco_SCOPE_DEGREE:		// Show connected users in the current degree
         if (Role == Rol_ROLE_UNKNOWN)	// Here Rol_ROLE_UNKNOWN means "any role"
            sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
                           " FROM courses,crs_usr,connected,usr_data"
                           " WHERE courses.DegCod='%ld'"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.UsrCod=connected.UsrCod"
                           " AND connected.UsrCod=usr_data.UsrCod",
                     Gbl.CurrentDeg.Deg.DegCod);
         else
            sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
                           " FROM courses,crs_usr,connected,usr_data"
                           " WHERE courses.DegCod='%ld'"
                           " AND courses.CrsCod=crs_usr.CrsCod"
                           " AND crs_usr.Role='%u'"
                           " AND crs_usr.UsrCod=connected.UsrCod"
                           " AND connected.UsrCod=usr_data.UsrCod",
                     Gbl.CurrentDeg.Deg.DegCod,
                     (unsigned) Role);
         break;
      case Sco_SCOPE_COURSE:		// Show connected users in the current course
         if (Role == Rol_ROLE_UNKNOWN)	// Here Rol_ROLE_UNKNOWN means "any role"
            sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
                           " FROM crs_usr,connected,usr_data"
                           " WHERE crs_usr.CrsCod='%ld'"
                           " AND crs_usr.UsrCod=connected.UsrCod"
                           " AND connected.UsrCod=usr_data.UsrCod",
                     Gbl.CurrentCrs.Crs.CrsCod);
         else
            sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
                           " FROM crs_usr,connected,usr_data"
                           " WHERE crs_usr.CrsCod='%ld'"
                           " AND crs_usr.Role='%u'"
                           " AND crs_usr.UsrCod=connected.UsrCod"
                           " AND connected.UsrCod=usr_data.UsrCod",
                     Gbl.CurrentCrs.Crs.CrsCod,
                     (unsigned) Role);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of connected users who belong to this course");

   row = mysql_fetch_row (mysql_res);

   /***** Get number of users (row[0]) *****/
   if (sscanf (row[0],"%u",&NumUsrs) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of connected users who belong to this course.");

   /***** Get number of distinct sexs (row[1]) *****/
   if (sscanf (row[1],"%u",&NumSexs) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of sexs in connected users who belong to this course.");

   /***** Get users' sex (row[2]) *****/
   *UsrSex = Usr_SEX_UNKNOWN;
   if (NumSexs == 1)
      for (Sex = (Usr_Sex_t) 0;
	   Sex < Usr_NUM_SEXS;
	   Sex++)
         if (!strcasecmp (row[2],Usr_StringsSexDB[Sex]))
           {
            *UsrSex = Sex;
            break;
           }

   /***** Free structure that stores the query result *****/
   mysql_free_result (mysql_res);

   return NumUsrs;
  }

/*****************************************************************************/
/******************* Compute connected users one by one **********************/
/*****************************************************************************/

static void Con_ComputeConnectedUsrsWithARoleCurrentCrsOneByOne (Rol_Role_t Role)
  {
   unsigned NumUsr = Gbl.Usrs.Connected.NumUsrs;
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   time_t TimeDiff;

   /***** Get connected users who belong to current course from database *****/
   sprintf (Query,"SELECT connected.UsrCod,connected.LastCrsCod,"
                  "UNIX_TIMESTAMP()-UNIX_TIMESTAMP(connected.LastTime) AS Dif"
                  " FROM connected,crs_usr,usr_data"
                  " WHERE crs_usr.CrsCod='%ld' AND crs_usr.Role='%u'"
                  " AND crs_usr.UsrCod=connected.UsrCod"
                  " AND crs_usr.UsrCod=usr_data.UsrCod ORDER BY Dif",
            Gbl.CurrentCrs.Crs.CrsCod,(unsigned) Role);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get list of connected users who belong to this course");
   Gbl.Usrs.Connected.NumUsrs       += (unsigned) NumRows;
   Gbl.Usrs.Connected.NumUsrsToList += (unsigned) NumRows;
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
      Gbl.Usrs.Connected.Lst[NumUsr].ThisCrs = (Str_ConvertStrCodToLongCod (row[1]) == Gbl.CurrentCrs.Crs.CrsCod);

      /* Compute time from last access */
      Gbl.Usrs.Connected.Lst[NumUsr].Seconds = 0;
      if (sscanf (row[2],"%ld",&TimeDiff) == 1)
         Gbl.Usrs.Connected.Lst[NumUsr].Seconds = (unsigned) TimeDiff;
     }

   /***** Free structure that stores the query result *****/
   mysql_free_result (mysql_res);
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
   const char *Color = Gbl.ColorRows[Gbl.RowEvenOdd];
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];
   const char *Font = (Gbl.Usrs.Connected.Lst[Gbl.Usrs.Connected.NumUsr].ThisCrs ? "CON_CRS" :
	                                                                           "CON");
   struct UsrData UsrDat;
   unsigned Hours;
   unsigned Minutes;
   unsigned Seconds;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Get user's data *****/
   UsrDat.UsrCod = Gbl.Usrs.Connected.Lst[Gbl.Usrs.Connected.NumUsr].UsrCod;
   Usr_GetAllUsrDataFromUsrCod (&UsrDat);

   /***** Show photo *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"left\" width=\"18\" valign=\"middle\" bgcolor=\"%s\">",Color);
   sprintf (Gbl.FormId,"form_con_%d",++Gbl.NumFormConnectedUsrs);
   Act_FormStartId ((Role == Rol_ROLE_STUDENT) ? ActSeeRecOneStd :
	                                         ActSeeRecOneTch,
	            Gbl.FormId);
   Usr_PutParamOtherUsrCodEncrypted (UsrDat.EncryptedUsrCod);
   Act_LinkFormSubmitId (NULL,NULL,Gbl.FormId);
   ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&UsrDat,
                     ShowPhoto ? PhotoURL :
                	         NULL,
                     18,24,true);
   fprintf (Gbl.F.Out,"</a>"
		      "</form>"
		      "</td>");

   /***** Write full name and link *****/
   fprintf (Gbl.F.Out,"<td align=\"left\" width=\"54\" valign=\"middle\" bgcolor=\"%s\" class=\"%s\">",
	    Color,Font);
   sprintf (Gbl.FormId,"form_con_%d",++Gbl.NumFormConnectedUsrs);
   Act_FormStartId ((Role == Rol_ROLE_STUDENT) ? ActSeeRecOneStd :
	                                         ActSeeRecOneTch,
	            Gbl.FormId);
   Usr_PutParamOtherUsrCodEncrypted (UsrDat.EncryptedUsrCod);
   Act_LinkFormSubmitId (UsrDat.FullName,Font,Gbl.FormId);
   Usr_RestrictLengthAndWriteName (&UsrDat,8);
   fprintf (Gbl.F.Out,"</a>"
		      "</form>"
		      "</td>");

   /***** Write time from last access *****/
   fprintf (Gbl.F.Out,"<td width=\"38\" align=\"right\" valign=\"middle\" bgcolor=\"%s\" class=\"%s\">",
            Color,Font);

   fprintf (Gbl.F.Out,"<div id=\"hm%u\">",Gbl.Usrs.Connected.NumUsr);	// Used for automatic update, only when displayed on right column
   Hours   =  Gbl.Usrs.Connected.Lst[Gbl.Usrs.Connected.NumUsr].Seconds / (60*60);
   Minutes = (Gbl.Usrs.Connected.Lst[Gbl.Usrs.Connected.NumUsr].Seconds / 60) % 60;
   Seconds =  Gbl.Usrs.Connected.Lst[Gbl.Usrs.Connected.NumUsr].Seconds % 60;
   if (Hours)
      fprintf (Gbl.F.Out,"%u:%02u'%02u&quot;",
               Hours,
               Minutes,
               Seconds);
   else if (Minutes)
      fprintf (Gbl.F.Out,"%u'%02u&quot;",
               Minutes,
               Seconds);
   else
      fprintf (Gbl.F.Out,"%u&quot;",
               Seconds);
   fprintf (Gbl.F.Out,"</div>");					// Used for automatic update, only when displayed on right column

   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/******************** Show connected users one by one ************************/
/*****************************************************************************/

static void Con_ShowConnectedUsrsCurrentLocationOneByOneOnMainZone (Rol_Role_t Role)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   bool ThisCrs;
   time_t TimeDiff;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];
   const char *Font;
   struct UsrData UsrDat;
   unsigned Hours;
   unsigned Minutes;
   unsigned Seconds;
   bool PutLinkToRecord = (Gbl.CurrentCrs.Crs.CrsCod > 0 &&
	                   Gbl.Scope.Current == Sco_SCOPE_COURSE);

   /***** Get connected users who belong to current location from database *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_PLATFORM:		// Show connected users in the whole platform
         sprintf (Query,"SELECT DISTINCTROW connected.UsrCod,connected.LastCrsCod,"
                        "UNIX_TIMESTAMP()-UNIX_TIMESTAMP(connected.LastTime) AS Dif"
                        " FROM connected,crs_usr,usr_data"
                        " WHERE connected.UsrCod=crs_usr.UsrCod"
                        " AND crs_usr.Role='%u'"
                        " AND connected.UsrCod=usr_data.UsrCod ORDER BY Dif",
                  (unsigned) Role);
         break;
      case Sco_SCOPE_INSTITUTION:	// Show connected users in the current institution
         sprintf (Query,"SELECT DISTINCTROW connected.UsrCod,connected.LastCrsCod,"
                        "UNIX_TIMESTAMP()-UNIX_TIMESTAMP(connected.LastTime) AS Dif"
                        " FROM centres,degrees,courses,crs_usr,connected,usr_data"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.Role='%u'"
                        " AND crs_usr.UsrCod=connected.UsrCod"
                        " AND crs_usr.UsrCod=usr_data.UsrCod ORDER BY Dif",
                  Gbl.CurrentIns.Ins.InsCod,
                  (unsigned) Role);
         break;
      case Sco_SCOPE_CENTRE:		// Show connected users in the current centre
         sprintf (Query,"SELECT DISTINCTROW connected.UsrCod,connected.LastCrsCod,"
                        "UNIX_TIMESTAMP()-UNIX_TIMESTAMP(connected.LastTime) AS Dif"
                        " FROM degrees,courses,crs_usr,connected,usr_data"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.Role='%u'"
                        " AND crs_usr.UsrCod=connected.UsrCod"
                        " AND crs_usr.UsrCod=usr_data.UsrCod ORDER BY Dif",
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  (unsigned) Role);
         break;
      case Sco_SCOPE_DEGREE:		// Show connected users in the current degree
         sprintf (Query,"SELECT DISTINCTROW connected.UsrCod,connected.LastCrsCod,"
                        "UNIX_TIMESTAMP()-UNIX_TIMESTAMP(connected.LastTime) AS Dif"
                        " FROM courses,crs_usr,connected,usr_data"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.Role='%u'"
                        " AND crs_usr.UsrCod=connected.UsrCod"
                        " AND crs_usr.UsrCod=usr_data.UsrCod ORDER BY Dif",
                  Gbl.CurrentDeg.Deg.DegCod,
                  (unsigned) Role);
         break;
      case Sco_SCOPE_COURSE:		// Show connected users in the current course
         sprintf (Query,"SELECT connected.UsrCod,connected.LastCrsCod,"
                        "UNIX_TIMESTAMP()-UNIX_TIMESTAMP(connected.LastTime) AS Dif"
                        " FROM crs_usr,connected,usr_data"
                        " WHERE crs_usr.CrsCod='%ld'"
                        " AND crs_usr.Role='%u'"
                        " AND crs_usr.UsrCod=connected.UsrCod"
                        " AND crs_usr.UsrCod=usr_data.UsrCod ORDER BY Dif",
                  Gbl.CurrentCrs.Crs.CrsCod,
                  (unsigned) Role);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get list of connected users who belong to this location");

   if (NumUsrs)
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
         Usr_GetAllUsrDataFromUsrCod (&UsrDat);

         /* Get course code (row[1]) */
         ThisCrs = (Str_ConvertStrCodToLongCod (row[1]) == Gbl.CurrentCrs.Crs.CrsCod);
         Font = (ThisCrs ? "CON_CRS" :
                           "CON");

         /* Compute time from last access */
         Seconds = 0;
         if (sscanf (row[2],"%ld",&TimeDiff) == 1)
            Seconds = (unsigned) TimeDiff;

         /***** Show photo *****/
         fprintf (Gbl.F.Out,"<tr>"
                            "<td align=\"left\" width=\"18\" valign=\"middle\" bgcolor=\"%s\">",
                  Gbl.ColorRows[Gbl.RowEvenOdd]);
         if (PutLinkToRecord)
           {
	    Act_FormStart ((Role == Rol_ROLE_STUDENT) ? ActSeeRecOneStd :
							ActSeeRecOneTch);
            Usr_PutParamOtherUsrCodEncrypted (UsrDat.EncryptedUsrCod);
            Act_LinkFormSubmit (NULL,NULL);
           }
         ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
         Pho_ShowUsrPhoto (&UsrDat,
                           ShowPhoto ? PhotoURL :
                        	       NULL,
                           18,24,true);
         if (PutLinkToRecord)
	    fprintf (Gbl.F.Out,"</a>"
			       "</form>");
         fprintf (Gbl.F.Out,"</td>");

         /***** Write full name and link *****/
         fprintf (Gbl.F.Out,"<td align=\"left\" width=\"256\" valign=\"middle\" bgcolor=\"%s\" class=\"%s\">",
                  Gbl.ColorRows[Gbl.RowEvenOdd],Font);
         if (PutLinkToRecord)
           {
	    Act_FormStart ((Role == Rol_ROLE_STUDENT) ? ActSeeRecOneStd :
							ActSeeRecOneTch);
            Usr_PutParamOtherUsrCodEncrypted (UsrDat.EncryptedUsrCod);
	    Act_LinkFormSubmit (UsrDat.FullName,Font);
           }
         Usr_RestrictLengthAndWriteName (&UsrDat,40);
         if (PutLinkToRecord)
	    fprintf (Gbl.F.Out,"</a>"
			       "</form>");
         fprintf (Gbl.F.Out,"</td>");

         /***** Write time from last access *****/
         fprintf (Gbl.F.Out,"<td width=\"38\" align=\"right\" valign=\"middle\" bgcolor=\"%s\" class=\"%s\">",
                  Gbl.ColorRows[Gbl.RowEvenOdd],Font);
	 Hours   = Seconds / (60 * 60);
	 Minutes = (Seconds / 60) % 60;
	 Seconds %= 60;
	 if (Hours)
	    fprintf (Gbl.F.Out,"%u:%02u'%02u&quot;",
		     Hours,
		     Minutes,
		     Seconds);
	 else if (Minutes)
	    fprintf (Gbl.F.Out,"%u'%02u&quot;",
		     Minutes,
		     Seconds);
	 else
	    fprintf (Gbl.F.Out,"%u&quot;",
		     Seconds);
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");

         Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   mysql_free_result (mysql_res);
  }

/*****************************************************************************/
/****** Write script to automatically update clocks of connected users *******/
/*****************************************************************************/

void Con_WriteScriptClockConnected (void)
  {
   unsigned NumUsr;

   fprintf (Gbl.F.Out,"	NumUsrsCon = %u;\n",
            Gbl.Usrs.Connected.NumUsrsToList);
   for (NumUsr = 0;
	NumUsr < Gbl.Usrs.Connected.NumUsrsToList;
	NumUsr++)
      fprintf (Gbl.F.Out,"	ListSeconds[%u] = %u;\n",
               NumUsr,Gbl.Usrs.Connected.Lst[NumUsr].Seconds);
   fprintf (Gbl.F.Out,"	writeClockConnected();\n");
  }
