// swad_connected.c: connected users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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
#include "swad_role.h"
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

static void Con_PutIconToUpdateConnected (void);

static void Con_ComputeConnectedUsrsWithARoleBelongingToCurrentCrs (Rol_Role_t Role);
static void Con_ShowConnectedUsrsBelongingToLocation (void);

static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_Role_t Role);
static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_Role_t Role);
static unsigned Con_GetConnectedGuestsTotal (void);
static unsigned Con_GetConnectedStdsTotal (void);
static unsigned Con_GetConnectedTchsTotal (void);
static void Con_GetNumConnectedUsrsWithARoleBelongingCurrentLocation (Rol_Role_t Role,struct ConnectedUsrs *Usrs);
static void Con_ComputeConnectedUsrsWithARoleCurrentCrsOneByOne (Rol_Role_t Role);
static void Con_ShowConnectedUsrsCurrentCrsOneByOneOnRightColumn (Rol_Role_t Role);
static void Con_WriteRowConnectedUsrOnRightColumn (Rol_Role_t Role);
static void Con_ShowConnectedUsrsCurrentLocationOneByOneOnMainZone (Rol_Role_t Role);
static void Con_WriteHoursMinutesSecondsFromSeconds (time_t Seconds);

/*****************************************************************************/
/************************** Show connected users *****************************/
/*****************************************************************************/

void Con_ShowConnectedUsrs (void)
  {
   extern const char *Hlp_USERS_Connected;
   extern const char *Txt_Connected_users;
   extern const char *Txt_MONTHS_SMALL_SHORT[12];

   /***** Link to show last clicks in real time *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Con_PutLinkToLastClicks ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Get scope *****/
   Sco_SetScopesForListingStudents ();
   Sco_GetScope ("ScopeCon");

   /***** Start frame *****/
   /* Current time */
   sprintf (Gbl.Title,"%s<br />%s %u, %u:%02u",
	    Txt_Connected_users,
            Txt_MONTHS_SMALL_SHORT[Gbl.Now.Date.Month-1],
            Gbl.Now.Date.Day,
            Gbl.Now.Time.Hour,
            Gbl.Now.Time.Minute);
   Lay_StartRoundFrame (NULL,Gbl.Title,
                        Con_PutIconToUpdateConnected,Hlp_USERS_Connected);

   /***** Number of connected users in the whole platform *****/
   Con_ShowGlobalConnectedUsrs ();

   /***** Show connected users in the current location *****/
   Con_ShowConnectedUsrsBelongingToLocation ();

   /***** End frame *****/
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/******************** Put icon to update connected users *********************/
/*****************************************************************************/

static void Con_PutIconToUpdateConnected (void)
  {
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_Update;

   Act_FormStart (ActLstCon);
   Sco_PutParamScope ("ScopeCon",Gbl.Scope.Current);
   Act_LinkFormSubmitAnimated (Txt_Update,The_ClassFormBold[Gbl.Prefs.Theme],
                               NULL);
   Lay_PutCalculateIcon (Txt_Update);
   Act_FormEnd ();
  }

/*****************************************************************************/
/*************** Put a link to show last clicks in real time *****************/
/*****************************************************************************/

void Con_PutLinkToLastClicks (void)
  {
   extern const char *Txt_Last_clicks;

   Lay_PutContextualLink (ActLstClk,NULL,
                          "mouse-pointer64x64.png",
                          Txt_Last_clicks,Txt_Last_clicks,
                          NULL);
  }

/*****************************************************************************/
/****************************** Show last clicks *****************************/
/*****************************************************************************/

void Con_ShowLastClicks (void)
  {
   extern const char *Hlp_USERS_Connected_last_clicks;
   extern const char *Txt_Last_clicks_in_real_time;

   /***** Start frame *****/
   Lay_StartRoundFrame (NULL,Txt_Last_clicks_in_real_time,
                        NULL,Hlp_USERS_Connected_last_clicks);

   /***** Get and show last clicks *****/
   fprintf (Gbl.F.Out,"<div id=\"lastclicks\""	// Used for AJAX based refresh
	              " class=\"CENTER_MIDDLE\">");
   Con_GetAndShowLastClicks ();
   fprintf (Gbl.F.Out,"</div>");		// Used for AJAX based refresh

   /***** End frame *****/
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/**************** Get last clicks from database and show them ****************/
/*****************************************************************************/

void Con_GetAndShowLastClicks (void)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   extern Act_Action_t Act_FromActCodToAction[1+Act_MAX_ACTION_COD];
   extern const char *Txt_Click;
   extern const char *Txt_ELAPSED_TIME;
   extern const char *Txt_Role;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Degree;
   extern const char *Txt_Action;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRow;
   unsigned NumRows;
   long ActCod;
   const char *ClassRow;
   time_t TimeDiff;
   struct Country Cty;
   struct Instit Ins;
   struct Centre Ctr;
   struct Degree Deg;

   /***** Get last clicks from database *****/
   /* Important for maximum performance:
      do the LIMIT in the big log table before the JOIN */
   sprintf (Query,"SELECT last_logs.LogCod,last_logs.ActCod,last_logs.Dif,last_logs.Role,"
	          "last_logs.CtyCod,last_logs.InsCod,last_logs.CtrCod,last_logs.DegCod,actions.Txt"
	          " FROM"
	          " (SELECT LogCod,ActCod,UNIX_TIMESTAMP()-UNIX_TIMESTAMP(ClickTime) AS Dif,Role,CtyCod,InsCod,CtrCod,DegCod"
                  " FROM log_recent ORDER BY LogCod DESC LIMIT 20)"
                  " AS last_logs,actions"
	          " WHERE last_logs.ActCod=actions.ActCod AND actions.Language='es'");
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get last clicks");

   /***** Write list of connected users *****/
   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_1\">"
                      "<tr>"
                      "<th class=\"LEFT_MIDDLE\""
                      " style=\"width:85px;\">"
                      "%s"				// Click
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\""
                      " style=\"width:50px;\">"
                      "%s"				// Elapsed time
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\""
                      " style=\"width:100px;\">"
                      "%s"				// Role
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\""
                      " style=\"width:100px;\">"
                      "%s"				// Country
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\""
                      " style=\"width:150px;\">"
                      "%s"				// Institution
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\""
                      " style=\"width:150px;\">"
                      "%s"				// Centre
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\""
                      " style=\"width:200px;\">"
                      "%s"				// Degree
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\""
                      " style=\"width:275px;\">"
                      "%s"				// Action
                      "</th>"
                      "</tr>",
               Txt_Click,
               Txt_ELAPSED_TIME,
               Txt_Role,
               Txt_Country,
               Txt_Institution,
               Txt_Centre,
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
      ClassRow = (Act_Actions[Act_FromActCodToAction[ActCod]].BrowserWindow == Act_DOWNLD_FILE) ? "DAT_SMALL_YELLOW LEFT_MIDDLE" :
	         (ActCod == Act_Actions[ActLogIn].ActCod ||
	          ActCod == Act_Actions[ActLogInNew].ActCod) ? "DAT_SMALL_GREEN" :
                 (ActCod == Act_Actions[ActLogOut].ActCod   ) ? "DAT_SMALL_RED" :
                 (ActCod == Act_Actions[ActWebSvc].ActCod   ) ? "DAT_SMALL_BLUE" :
                                                                "DAT_SMALL_GREY";

      /* Compute elapsed time from last access */
      if (sscanf (row[2],"%ld",&TimeDiff) != 1)
         TimeDiff = (time_t) 0;

      /* Get country code (row[4]) */
      Cty.CtyCod = Str_ConvertStrCodToLongCod (row[4]);
      Cty_GetCountryName (Cty.CtyCod,Cty.Name[Gbl.Prefs.Language]);

      /* Get institution code (row[5]) */
      Ins.InsCod = Str_ConvertStrCodToLongCod (row[5]);
      Ins_GetShortNameOfInstitutionByCod (&Ins);

      /* Get centre code (row[6]) */
      Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[6]);
      Ctr_GetShortNameOfCentreByCod (&Ctr);

      /* Get degree code (row[7]) */
      Deg.DegCod = Str_ConvertStrCodToLongCod (row[7]);
      Deg_GetShortNameOfDegreeByCod (&Deg);

      /* Print table row */
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"%s LEFT_MIDDLE\">"
                         "%s"					// Click
                         "</td>"
                         "<td class=\"%s RIGHT_MIDDLE\">"	// Elapsed time
                         "",
               ClassRow,row[0],
               ClassRow);
      Con_WriteHoursMinutesSecondsFromSeconds (TimeDiff);
      fprintf (Gbl.F.Out,"</td>"
                         "<td class=\"%s LEFT_MIDDLE\">"
                         "%s"					// Role
                         "</td>"
                         "<td class=\"%s LEFT_MIDDLE\">"
                         "%s"					// Country
                         "</td>"
                         "<td class=\"%s LEFT_MIDDLE\">"
                         "%s"					// Institution
                         "</td>"
                         "<td class=\"%s LEFT_MIDDLE\">"
                         "%s"					// Centre
                         "</td>"
                         "<td class=\"%s LEFT_MIDDLE\">"
                         "%s"					// Degree
                         "</td>"
                         "<td class=\"%s LEFT_MIDDLE\">"
                         "%s"					// Action
                         "</td>"
			 "</tr>",
               ClassRow,Txt_ROLES_SINGUL_Abc[Rol_ConvertUnsignedStrToRole (row[3])][Usr_SEX_UNKNOWN],
               ClassRow,Cty.Name[Gbl.Prefs.Language],
               ClassRow,Ins.ShrtName,
               ClassRow,Ctr.ShrtName,
               ClassRow,Deg.ShrtName,
	       ClassRow,row[8]);
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
   extern const char *The_ClassConnected[The_NUM_THEMES];
   extern const char *Txt_Connected_users;
   extern const char *Txt_session;
   extern const char *Txt_sessions;
   extern const char *Txt_user[Usr_NUM_SEXS];
   extern const char *Txt_users[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   unsigned StdsTotal = Con_GetConnectedStdsTotal ();
   unsigned TchsTotal = Con_GetConnectedTchsTotal ();
   unsigned WithoutCoursesTotal = Con_GetConnectedGuestsTotal ();
   unsigned UsrsTotal = StdsTotal + TchsTotal + WithoutCoursesTotal;

   /***** Container start *****/
   fprintf (Gbl.F.Out,"<div class=\"CONNECTED LEFT_RIGHT_CONTENT_WIDTH\">");

   /***** Number of sessions *****/
   /* Link to view more details about connected users */
   Act_FormStartUnique (ActLstCon);	// Must be unique because
					// the list of connected users
					// is dynamically updated via AJAX
   Act_LinkFormSubmitUnique (Txt_Connected_users,The_ClassConnected[Gbl.Prefs.Theme]);

   /* Write total number of sessions */
   fprintf (Gbl.F.Out,"%u %s",
            Gbl.Session.NumSessions,
            (Gbl.Session.NumSessions == 1) ? Txt_session :
        	                             Txt_sessions);
   /* End of link to view more details about connected users */
   fprintf (Gbl.F.Out,"</a>");
   Act_FormEnd ();

   if (UsrsTotal)
     {
      fprintf (Gbl.F.Out,"<div class=\"CONNECTED_LIST\">");

      /***** Write total number of users *****/
      fprintf (Gbl.F.Out,"%u %s:",
	       UsrsTotal,
	       (UsrsTotal == 1) ? Txt_user[Usr_SEX_UNKNOWN] :
				  Txt_users[Usr_SEX_UNKNOWN]);

      /***** Write total number of students *****/
      if (StdsTotal)
	 fprintf (Gbl.F.Out,"<br />"
			    "%u %s",
		  StdsTotal,
		  (StdsTotal == 1) ? Txt_ROLES_SINGUL_abc[Rol_STUDENT][Usr_SEX_UNKNOWN] :
				     Txt_ROLES_PLURAL_abc[Rol_STUDENT][Usr_SEX_UNKNOWN]);

      /***** Write total number of teachers *****/
      if (TchsTotal)
	 fprintf (Gbl.F.Out,"<br />"
			    "%u %s",
		  TchsTotal,
		  (TchsTotal == 1) ? Txt_ROLES_SINGUL_abc[Rol_TEACHER][Usr_SEX_UNKNOWN] :
				     Txt_ROLES_PLURAL_abc[Rol_TEACHER][Usr_SEX_UNKNOWN]);

      /***** Write total number of users who do not belong to any course *****/
      if (WithoutCoursesTotal)
	 fprintf (Gbl.F.Out,"<br />"
			    "%u %s",
		  WithoutCoursesTotal,
		  (WithoutCoursesTotal == 1) ? Txt_ROLES_SINGUL_abc[Rol__GUEST_][Usr_SEX_UNKNOWN] :
					       Txt_ROLES_PLURAL_abc[Rol__GUEST_][Usr_SEX_UNKNOWN]);

      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Container end *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/*********** Compute connected users who belong to current course ************/
/*****************************************************************************/

void Con_ComputeConnectedUsrsBelongingToCurrentCrs (void)
  {
   if ((Gbl.Prefs.SideCols & Lay_SHOW_RIGHT_COLUMN) &&	// Right column visible
       Gbl.CurrentCrs.Crs.CrsCod > 0 &&			// There is a course selected
       (Gbl.Usrs.Me.IBelongToCurrentCrs ||		// I can view users
        Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM))
     {
      Gbl.Usrs.Connected.NumUsrs       = 0;
      Gbl.Usrs.Connected.NumUsrsToList = 0;
      Gbl.Scope.Current = Sco_SCOPE_CRS;

      /***** Number of teachers *****/
      Con_ComputeConnectedUsrsWithARoleBelongingToCurrentCrs (Rol_TEACHER);

      /***** Number of students *****/
      Con_ComputeConnectedUsrsWithARoleBelongingToCurrentCrs (Rol_STUDENT);
     }
  }

/*****************************************************************************/
/** Compute number of connected users of a type who belong to current course */
/*****************************************************************************/

static void Con_ComputeConnectedUsrsWithARoleBelongingToCurrentCrs (Rol_Role_t Role)
  {
   /***** Get number of connected users who belong to current course *****/
   Con_GetNumConnectedUsrsWithARoleBelongingCurrentLocation (Role,&Gbl.Usrs.Connected.Usrs[Role]);

   /***** Get list connected users belonging to this course *****/
   Con_ComputeConnectedUsrsWithARoleCurrentCrsOneByOne (Role);
  }

/*****************************************************************************/
/****** Show number of connected users who belong to current location ********/
/*****************************************************************************/

static void Con_ShowConnectedUsrsBelongingToLocation (void)
  {
   extern const char *The_ClassConnected[The_NUM_THEMES];
   extern const char *Txt_from;
   char LocationName[Deg_MAX_LENGTH_LOCATION_SHORT_NAME_SPEC_CHAR + 1];
   struct ConnectedUsrs Usrs;

   /***** Set location name depending on scope *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:		// Show connected users in the whole platform
         Str_Copy (LocationName,Cfg_PLATFORM_SHORT_NAME,
                   Deg_MAX_LENGTH_LOCATION_SHORT_NAME_SPEC_CHAR);
         break;
      case Sco_SCOPE_CTY:		// Show connected users in the current country
         if (Gbl.CurrentCty.Cty.CtyCod <= 0)	// There is no country selected
            return;

         Str_Copy (LocationName,Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language],
                   Deg_MAX_LENGTH_LOCATION_SHORT_NAME_SPEC_CHAR);
         break;
      case Sco_SCOPE_INS:		// Show connected users in the current institution
         if (Gbl.CurrentIns.Ins.InsCod <= 0)	// There is no institution selected
            return;

         Str_Copy (LocationName,Gbl.CurrentIns.Ins.ShrtName,
                   Deg_MAX_LENGTH_LOCATION_SHORT_NAME_SPEC_CHAR);
         break;
      case Sco_SCOPE_CTR:		// Show connected users in the current centre
         if (Gbl.CurrentCtr.Ctr.CtrCod <= 0)	// There is no centre selected
            return;

         Str_Copy (LocationName,Gbl.CurrentCtr.Ctr.ShrtName,
                   Deg_MAX_LENGTH_LOCATION_SHORT_NAME_SPEC_CHAR);
         break;
      case Sco_SCOPE_DEG:		// Show connected users in the current degree
         if (Gbl.CurrentDeg.Deg.DegCod <= 0)	// There is no degree selected
            return;

         Str_Copy (LocationName,Gbl.CurrentDeg.Deg.ShrtName,
                   Deg_MAX_LENGTH_LOCATION_SHORT_NAME_SPEC_CHAR);
         break;
      case Sco_SCOPE_CRS:		// Show connected users in the current course
         if (Gbl.CurrentCrs.Crs.CrsCod <= 0)	// There is no course selected
            return;

         Str_Copy (LocationName,Gbl.CurrentCrs.Crs.ShrtName,
                   Deg_MAX_LENGTH_LOCATION_SHORT_NAME_SPEC_CHAR);
         break;
      default:
	 return;
     }

   /***** Start container *****/
   fprintf (Gbl.F.Out,"<div class=\"CONNECTED\""
		      " style=\"width:390px; margin-top:6px;\">");

   /***** Number of connected users who belong to scope *****/
   Con_GetNumConnectedUsrsWithARoleBelongingCurrentLocation (Rol_UNKNOWN,&Usrs);
   fprintf (Gbl.F.Out,"<div class=\"%s\">%u %s ",
	    The_ClassConnected[Gbl.Prefs.Theme],
	    Usrs.NumUsrs,
	    Txt_from);
   if (Gbl.Usrs.Me.LoggedRole >= Rol_DEG_ADM)
     {
      /* Put form to change scope */
      Act_FormStart (ActLstCon);
      Sco_PutSelectorScope ("ScopeCon",true);
      Act_FormEnd ();
     }
   else
     {
      /* Write location name depending on the scope */
      Str_LimitLengthHTMLStr (LocationName,40);
      fprintf (Gbl.F.Out,"%s",LocationName);
     }
   fprintf (Gbl.F.Out,"</div>");

   /***** Number of teachers and students *****/
   fprintf (Gbl.F.Out,"<table class=\"CONNECTED_LIST\">");
   Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_TEACHER);
   Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_STUDENT);
   if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
      Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol__GUEST_);
   fprintf (Gbl.F.Out,"</table>");

   /***** End container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/* Show number of connected users who belong to current course on right col. */
/*****************************************************************************/

void Con_ShowConnectedUsrsBelongingToCurrentCrs (void)
  {
   extern const char *The_ClassConnected[The_NUM_THEMES];
   extern const char *Txt_Connected_users;
   extern const char *Txt_from;
   char CourseName[Crs_MAX_LENGTH_COURSE_SHRT_NAME + 1];
   struct ConnectedUsrs Usrs;

   if (Gbl.CurrentCrs.Crs.CrsCod <= 0)	// There is no course selected
      return;

   /***** Start container *****/
   fprintf (Gbl.F.Out,"<div class=\"CONNECTED LEFT_RIGHT_CONTENT_WIDTH\">");

   /***** Number of connected users who belong to course *****/
   /* Link to view more details about connected users */
   Act_FormStartUnique (ActLstCon);	// Must be unique because
					// the list of connected users
					// is dynamically updated via AJAX
   Act_LinkFormSubmitUnique (Txt_Connected_users,The_ClassConnected[Gbl.Prefs.Theme]);

   /* Write total number of connected users belonging to the current course */
   Str_Copy (CourseName,Gbl.CurrentCrs.Crs.ShrtName,
             Crs_MAX_LENGTH_COURSE_SHRT_NAME);
   Str_LimitLengthHTMLStr (CourseName,12);
   Con_GetNumConnectedUsrsWithARoleBelongingCurrentLocation (Rol_UNKNOWN,&Usrs);
   fprintf (Gbl.F.Out,"%u %s %s",
            Usrs.NumUsrs,Txt_from,CourseName);

   /* End of link to view more details about connected users */
   fprintf (Gbl.F.Out,"</a>");
   Act_FormEnd ();

   /***** Number of teachers and students *****/
   fprintf (Gbl.F.Out,"<table class=\"CONNECTED_LIST\">");
   Gbl.Usrs.Connected.NumUsr        = 0;
   Gbl.Usrs.Connected.NumUsrs       = 0;
   Gbl.Usrs.Connected.NumUsrsToList = 0;
   Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_TEACHER);
   Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_STUDENT);
   fprintf (Gbl.F.Out,"</table>");

   /***** End container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/* Show number of connected users with a role who belong to current location */
/*****************************************************************************/

static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentLocationOnMainZone (Rol_Role_t Role)
  {
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   struct ConnectedUsrs Usrs;

   /***** Write number of connected users who belong to current course *****/
   Con_GetNumConnectedUsrsWithARoleBelongingCurrentLocation (Role,&Usrs);
   if (Usrs.NumUsrs)
     {
      fprintf (Gbl.F.Out,"<tr>"
			 "<td colspan=\"3\" class=\"CENTER_TOP\">"
			 "%u %s"
			 "</td>"
			 "</tr>",
	       Usrs.NumUsrs,
	       (Usrs.NumUsrs == 1) ? Txt_ROLES_SINGUL_abc[Role][Usrs.Sex] :
				     Txt_ROLES_PLURAL_abc[Role][Usrs.Sex]);

      /***** List connected users belonging to this location *****/
      switch (Gbl.Scope.Current)
	{
	 case Sco_SCOPE_SYS:		// Show connected users in the whole platform
	 case Sco_SCOPE_CTY:		// Show connected users in the current country
	    if (Gbl.Usrs.Me.LoggedRole != Rol_SYS_ADM)
	       return;
	    break;
	 case Sco_SCOPE_INS:		// Show connected users in the current institution
	    if (Gbl.Usrs.Me.LoggedRole < Rol_INS_ADM)
	       return;
	    break;
	 case Sco_SCOPE_CTR:		// Show connected users in the current centre
	    if (Gbl.Usrs.Me.LoggedRole < Rol_CTR_ADM)
	       return;
	    break;
	 case Sco_SCOPE_DEG:		// Show connected users in the current degree
	    if (Gbl.Usrs.Me.LoggedRole < Rol_DEG_ADM)
	       return;
	    break;
	 case Sco_SCOPE_CRS:		// Show connected users in the current course
	    if (Gbl.Usrs.Me.LoggedRole < Rol_STUDENT)
	       return;
	    break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
	}

      /***** I can see connected users *****/
      Con_ShowConnectedUsrsCurrentLocationOneByOneOnMainZone (Role);
     }
  }

/*****************************************************************************/
/** Show number of connected users with a role who belong to current course **/
/*****************************************************************************/

static void Con_ShowConnectedUsrsWithARoleBelongingToCurrentCrsOnRightColumn (Rol_Role_t Role)
  {
   extern const char *The_ClassConnected[The_NUM_THEMES];
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

      fprintf (Gbl.F.Out,"<tr>"
			 "<td colspan=\"3\" class=\"CENTER_TOP LEFT_RIGHT_CONTENT_WIDTH\">"
			 "%u %s"
			 "</td>"
			 "</tr>",
	       NumUsrsThisRole,
	       (NumUsrsThisRole == 1) ? Txt_ROLES_SINGUL_abc[Role][UsrSex] :
					Txt_ROLES_PLURAL_abc[Role][UsrSex]);

      /***** I can see connected users *****/
      Con_ShowConnectedUsrsCurrentCrsOneByOneOnRightColumn (Role);

      /***** Write message with number of users not listed *****/
      if (Gbl.Usrs.Connected.NumUsrsToList < Gbl.Usrs.Connected.NumUsrs)
	{
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td colspan=\"3\" class=\"CENTER_TOP\">");
	 Act_FormStartUnique (ActLstCon);	// Must be unique because
						// the list of connected users
						// is dynamically updated via AJAX
	 Sco_PutParamScope ("ScopeCon",Sco_SCOPE_CRS);
	 Act_LinkFormSubmitUnique (Txt_Connected_users,The_ClassConnected[Gbl.Prefs.Theme]);
	 fprintf (Gbl.F.Out,"<img src=\"%s/ellipsis32x32.gif\""
			    " alt=\"%s\" title=\"%s\" class=\"ICO40x40\" />"
			    "</a>",
		  Gbl.Prefs.IconsURL,
		  Txt_Connected_users,Txt_Connected_users);
	 Act_FormEnd ();
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");
	}
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
   MyRoleInConnected = (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT ||
                        Gbl.Usrs.Me.LoggedRole == Rol_TEACHER) ? Gbl.Usrs.Me.LoggedRole :
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
            (unsigned) Rol__GUEST_);
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
            (unsigned) Rol_STUDENT);
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
            (unsigned) Rol_TEACHER);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of connected teachers");
  }

/*****************************************************************************/
/************** Get connected users belonging to current course **************/
/*****************************************************************************/
// Return user's sex in UsrSex

static void Con_GetNumConnectedUsrsWithARoleBelongingCurrentLocation (Rol_Role_t Role,struct ConnectedUsrs *Usrs)
  {
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumSexs;
   Usr_Sex_t Sex;

   /***** Get number of connected users who belong to current course from database *****/
   switch (Role)
     {
      case Rol_UNKNOWN:	// Here Rol_ROLE_UNKNOWN means "any role"
	 switch (Gbl.Scope.Current)
	   {
	    case Sco_SCOPE_SYS:		// Show connected users in the whole platform
	       sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),"
		              "COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
			      " FROM connected,usr_data"
			      " WHERE connected.UsrCod=usr_data.UsrCod");
	       break;
	    case Sco_SCOPE_CTY:		// Show connected users in the current country
	       sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),"
		              "COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
			      " FROM institutions,centres,degrees,courses,crs_usr,connected,usr_data"
			      " WHERE institutions.CtyCod='%ld'"
			      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=connected.UsrCod"
			      " AND connected.UsrCod=usr_data.UsrCod",
			Gbl.CurrentCty.Cty.CtyCod);
	       break;
	    case Sco_SCOPE_INS:		// Show connected users in the current institution
	       sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),"
		              "COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
			      " FROM centres,degrees,courses,crs_usr,connected,usr_data"
			      " WHERE centres.InsCod='%ld'"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=connected.UsrCod"
			      " AND connected.UsrCod=usr_data.UsrCod",
			Gbl.CurrentIns.Ins.InsCod);
	       break;
	    case Sco_SCOPE_CTR:		// Show connected users in the current centre
	       sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),"
		              "COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
			      " FROM degrees,courses,crs_usr,connected,usr_data"
			      " WHERE degrees.CtrCod='%ld'"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=connected.UsrCod"
			      " AND connected.UsrCod=usr_data.UsrCod",
			Gbl.CurrentCtr.Ctr.CtrCod);
	       break;
	    case Sco_SCOPE_DEG:		// Show connected users in the current degree
	       sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),"
		              "COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
			      " FROM courses,crs_usr,connected,usr_data"
			      " WHERE courses.DegCod='%ld'"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=connected.UsrCod"
			      " AND connected.UsrCod=usr_data.UsrCod",
			Gbl.CurrentDeg.Deg.DegCod);
	       break;
	    case Sco_SCOPE_CRS:		// Show connected users in the current course
	       sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),"
		              "COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
			      " FROM crs_usr,connected,usr_data"
			      " WHERE crs_usr.CrsCod='%ld'"
			      " AND crs_usr.UsrCod=connected.UsrCod"
			      " AND connected.UsrCod=usr_data.UsrCod",
			Gbl.CurrentCrs.Crs.CrsCod);
	       break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong scope.");
	       break;
	   }
	 break;
      case Rol__GUEST_:
	 sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),"
	                "COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
			" FROM connected,usr_data"
			" WHERE connected.UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
			" AND connected.UsrCod=usr_data.UsrCod");
	 break;
      case Rol_STUDENT:
      case Rol_TEACHER:
	 switch (Gbl.Scope.Current)
	   {
	    case Sco_SCOPE_SYS:		// Show connected users in the whole platform
	       sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),"
		              "COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
			      " FROM connected,crs_usr,usr_data"
			      " WHERE connected.UsrCod=crs_usr.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " AND connected.UsrCod=usr_data.UsrCod",
			(unsigned) Role);
	       break;
	    case Sco_SCOPE_CTY:		// Show connected users in the current country
	       sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),"
		              "COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
			      " FROM institutions,centres,degrees,courses,crs_usr,connected,usr_data"
			      " WHERE institutions.CtyCod='%ld'"
			      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod=connected.UsrCod"
			      " AND connected.UsrCod=usr_data.UsrCod",
			Gbl.CurrentCty.Cty.CtyCod,
			(unsigned) Role);
	       break;
	    case Sco_SCOPE_INS:		// Show connected users in the current institution
	       sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),"
		              "COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
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
	    case Sco_SCOPE_CTR:		// Show connected users in the current centre
	       sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),"
		              "COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
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
	    case Sco_SCOPE_DEG:		// Show connected users in the current degree
	       sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),"
		              "COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
			      " FROM courses,crs_usr,connected,usr_data"
			      " WHERE courses.DegCod='%ld'"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod=connected.UsrCod"
			      " AND connected.UsrCod=usr_data.UsrCod",
			Gbl.CurrentDeg.Deg.DegCod,
			(unsigned) Role);
	       break;
	    case Sco_SCOPE_CRS:		// Show connected users in the current course
	       sprintf (Query,"SELECT COUNT(DISTINCT connected.UsrCod),"
		              "COUNT(DISTINCT usr_data.Sex),MIN(usr_data.Sex)"
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
	 break;
      default:
	 Lay_ShowErrorAndExit ("Wrong role.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of connected users who belong to this location");

   row = mysql_fetch_row (mysql_res);

   /***** Get number of users (row[0]) *****/
   if (sscanf (row[0],"%u",&(Usrs->NumUsrs)) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of connected users who belong to this location.");

   /***** Get number of distinct sexs (row[1]) *****/
   if (sscanf (row[1],"%u",&NumSexs) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of sexs in connected users who belong to this location.");

   /***** Get users' sex (row[2]) *****/
   Usrs->Sex = Usr_SEX_UNKNOWN;
   if (NumSexs == 1)
      for (Sex = (Usr_Sex_t) 0;
	   Sex < Usr_NUM_SEXS;
	   Sex++)
         if (!strcasecmp (row[2],Usr_StringsSexDB[Sex]))
           {
            Usrs->Sex = Sex;
            break;
           }

   /***** Free structure that stores the query result *****/
   mysql_free_result (mysql_res);
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

   /***** Get connected users who belong to current course from database *****/
   sprintf (Query,"SELECT connected.UsrCod,connected.LastCrsCod,"
                  "UNIX_TIMESTAMP()-UNIX_TIMESTAMP(connected.LastTime) AS Dif"
                  " FROM connected,crs_usr"
                  " WHERE crs_usr.CrsCod='%ld' AND crs_usr.Role='%u'"
                  " AND crs_usr.UsrCod=connected.UsrCod"
                  " ORDER BY Dif",
            Gbl.CurrentCrs.Crs.CrsCod,
            (unsigned) Role);
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

      /* Compute elapsed time from last access */
      if (sscanf (row[2],"%ld",&Gbl.Usrs.Connected.Lst[NumUsr].TimeDiff) != 1)
         Gbl.Usrs.Connected.Lst[NumUsr].TimeDiff = (time_t) 0;
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
   extern const char *Txt_View_record_for_this_course;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];
   const char *Font = (Gbl.Usrs.Connected.Lst[Gbl.Usrs.Connected.NumUsr].ThisCrs ? "CON_CRS" :
	                                                                           "CON");
   struct UsrData UsrDat;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Get user's data *****/
   UsrDat.UsrCod = Gbl.Usrs.Connected.Lst[Gbl.Usrs.Connected.NumUsr].UsrCod;
   Usr_GetAllUsrDataFromUsrCod (&UsrDat);

   /***** Show photo *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"LEFT_MIDDLE COLOR%u\""
	              " style=\"width:22px;\">",
	    Gbl.RowEvenOdd);
   Act_FormStartUnique (ActSeePubPrf);	// Must be unique because
					// the list of connected users
					// is dynamically updated via AJAX
   Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
   Act_LinkFormSubmitUnique (UsrDat.FullName,NULL);
   ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                	                 NULL,
                     "PHOTO21x28",Pho_ZOOM,false);
   fprintf (Gbl.F.Out,"</a>");
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>");

   /***** Write full name and link *****/
   fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE COLOR%u\""
	              " style=\"width:68px;\">",
	    Font,Gbl.RowEvenOdd);
   Act_FormStartUnique ((Role == Rol_STUDENT) ? ActSeeRecOneStd :
	                                        ActSeeRecOneTch);	// Must be unique because
									// the list of connected users
									// is dynamically updated via AJAX
   Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
   Act_LinkFormSubmitUnique (Txt_View_record_for_this_course,Font);
   Usr_RestrictLengthAndWriteName (&UsrDat,8);
   fprintf (Gbl.F.Out,"</a>");
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>");

   /***** Write time from last access *****/
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE COLOR%u\""
	              " style=\"width:48px;\">",
            Font,Gbl.RowEvenOdd);

   fprintf (Gbl.F.Out,"<div id=\"hm%u\">",
            Gbl.Usrs.Connected.NumUsr);	// Used for automatic update, only when displayed on right column
   Con_WriteHoursMinutesSecondsFromSeconds (Gbl.Usrs.Connected.Lst[Gbl.Usrs.Connected.NumUsr].TimeDiff);
   fprintf (Gbl.F.Out,"</div>");	// Used for automatic update, only when displayed on right column

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
   bool PutLinkToRecord = (Gbl.CurrentCrs.Crs.CrsCod > 0 &&
	                   Gbl.Scope.Current == Sco_SCOPE_CRS);

   /***** Get connected users who belong to current location from database *****/
   switch (Role)
     {
      case Rol__GUEST_:
	 sprintf (Query,"SELECT UsrCod,LastCrsCod,"
			"UNIX_TIMESTAMP()-UNIX_TIMESTAMP(LastTime) AS Dif"
			" FROM connected"
			" WHERE UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
			" ORDER BY Dif");
	 break;
      case Rol_STUDENT:
      case Rol_TEACHER:
	 switch (Gbl.Scope.Current)
	   {
	    case Sco_SCOPE_SYS:		// Show connected users in the whole platform
	       sprintf (Query,"SELECT DISTINCTROW connected.UsrCod,connected.LastCrsCod,"
			      "UNIX_TIMESTAMP()-UNIX_TIMESTAMP(connected.LastTime) AS Dif"
			      " FROM connected,crs_usr"
			      " WHERE connected.UsrCod=crs_usr.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " ORDER BY Dif",
			(unsigned) Role);
	       break;
	    case Sco_SCOPE_CTY:		// Show connected users in the current country
	       sprintf (Query,"SELECT DISTINCTROW connected.UsrCod,connected.LastCrsCod,"
			      "UNIX_TIMESTAMP()-UNIX_TIMESTAMP(connected.LastTime) AS Dif"
			      " FROM institutions,centres,degrees,courses,crs_usr,connected"
			      " WHERE institutions.CtyCod='%ld'"
			      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod=connected.UsrCod"
			      " ORDER BY Dif",
			Gbl.CurrentCty.Cty.CtyCod,
			(unsigned) Role);
	       break;
	    case Sco_SCOPE_INS:		// Show connected users in the current institution
	       sprintf (Query,"SELECT DISTINCTROW connected.UsrCod,connected.LastCrsCod,"
			      "UNIX_TIMESTAMP()-UNIX_TIMESTAMP(connected.LastTime) AS Dif"
			      " FROM centres,degrees,courses,crs_usr,connected"
			      " WHERE centres.InsCod='%ld'"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod=connected.UsrCod"
			      " ORDER BY Dif",
			Gbl.CurrentIns.Ins.InsCod,
			(unsigned) Role);
	       break;
	    case Sco_SCOPE_CTR:		// Show connected users in the current centre
	       sprintf (Query,"SELECT DISTINCTROW connected.UsrCod,connected.LastCrsCod,"
			      "UNIX_TIMESTAMP()-UNIX_TIMESTAMP(connected.LastTime) AS Dif"
			      " FROM degrees,courses,crs_usr,connected"
			      " WHERE degrees.CtrCod='%ld'"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod=connected.UsrCod"
			      " ORDER BY Dif",
			Gbl.CurrentCtr.Ctr.CtrCod,
			(unsigned) Role);
	       break;
	    case Sco_SCOPE_DEG:		// Show connected users in the current degree
	       sprintf (Query,"SELECT DISTINCTROW connected.UsrCod,connected.LastCrsCod,"
			      "UNIX_TIMESTAMP()-UNIX_TIMESTAMP(connected.LastTime) AS Dif"
			      " FROM courses,crs_usr,connected"
			      " WHERE courses.DegCod='%ld'"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod=connected.UsrCod"
			      " ORDER BY Dif",
			Gbl.CurrentDeg.Deg.DegCod,
			(unsigned) Role);
	       break;
	    case Sco_SCOPE_CRS:		// Show connected users in the current course
	       sprintf (Query,"SELECT connected.UsrCod,connected.LastCrsCod,"
			      "UNIX_TIMESTAMP()-UNIX_TIMESTAMP(connected.LastTime) AS Dif"
			      " FROM crs_usr,connected"
			      " WHERE crs_usr.CrsCod='%ld'"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod=connected.UsrCod"
			      " ORDER BY Dif",
			Gbl.CurrentCrs.Crs.CrsCod,
			(unsigned) Role);
	       break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong scope.");
	       break;
	   }
	 break;
      default:
	 Lay_ShowErrorAndExit ("Wrong role.");
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
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // Existing user
           {
	    /* Get course code (row[1]) */
	    ThisCrs = (Str_ConvertStrCodToLongCod (row[1]) == Gbl.CurrentCrs.Crs.CrsCod);
	    Font = (ThisCrs ? "CON_CRS" :
			      "CON");

	    /* Compute time from last access */
	    if (sscanf (row[2],"%ld",&TimeDiff) != 1)
	       TimeDiff = (time_t) 0;

	    /***** Show photo *****/
	    fprintf (Gbl.F.Out,"<tr>"
			       "<td class=\"LEFT_MIDDLE COLOR%u\""
			       " style=\"width:22px;\">",
		     Gbl.RowEvenOdd);
	    ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
	    Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
						  NULL,
			      "PHOTO21x28",Pho_ZOOM,false);
	    fprintf (Gbl.F.Out,"</td>");

	    /***** Write full name and link *****/
	    fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE COLOR%u\""
			       " style=\"width:320px;\">",
		     Font,Gbl.RowEvenOdd);
	    if (PutLinkToRecord)
	      {
	       Act_FormStart ((Role == Rol_STUDENT) ? ActSeeRecOneStd :
							   ActSeeRecOneTch);
	       Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
	       Act_LinkFormSubmit (UsrDat.FullName,Font,NULL);
	      }
	    Usr_RestrictLengthAndWriteName (&UsrDat,40);
	    if (PutLinkToRecord)
	      {
	       fprintf (Gbl.F.Out,"</a>");
	       Act_FormEnd ();
	      }
	    fprintf (Gbl.F.Out,"</td>");

	    /***** Write time from last access *****/
	    fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE COLOR%u\""
			       " style=\"width:48px;\">",
		     Font,Gbl.RowEvenOdd);
	    Con_WriteHoursMinutesSecondsFromSeconds (TimeDiff);
	    fprintf (Gbl.F.Out,"</td>"
			       "</tr>");

	    Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
	   }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********* Write time difference in seconds as hours:minutes:seconds *********/
/*****************************************************************************/
// TimeDiff must be in seconds

static void Con_WriteHoursMinutesSecondsFromSeconds (time_t Seconds)
  {
   time_t Hours = Seconds / (60*60);
   time_t Minutes = (Seconds / 60) % 60;

   Seconds %= 60;
   if (Hours)
      fprintf (Gbl.F.Out,"%ld:%02ld'%02ld&quot;",
               (long) Hours,
               (long) Minutes,
               (long) Seconds);
   else if (Minutes)
      fprintf (Gbl.F.Out,"%ld'%02ld&quot;",
               (long) Minutes,
               (long) Seconds);
   else
      fprintf (Gbl.F.Out,"%ld&quot;",
               (long) Seconds);
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
      fprintf (Gbl.F.Out,"	ListSeconds[%u] = %ld;\n",
               NumUsr,Gbl.Usrs.Connected.Lst[NumUsr].TimeDiff);
   fprintf (Gbl.F.Out,"	writeClockConnected();\n");
  }
