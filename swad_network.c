// swad_network.c: users' webs and social networks

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include <string.h>

#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_profile.h"
#include "swad_theme.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Net_NUM_WEBS_AND_SOCIAL_NETWORKS 29
typedef enum
  {
   Net_WWW,		// Personal web page
   Net_500PX,
   Net_DELICIOUS,
   Net_DEVIANTART,
   Net_DIASPORA,
   Net_EDMODO,
   Net_FACEBOOK,
   Net_FLICKR,
   Net_FOURSQUARE,
   Net_GITHUB,
   Net_GNU_SOCIAL,
   Net_GOOGLE_PLUS,
   Net_GOOGLE_SCHOLAR,
   Net_IDENTICA,
   Net_INSTAGRAM,
   Net_LINKEDIN,
   Net_ORCID,
   Net_PAPERLI,
   Net_PINTEREST,
   Net_QUITTER,
   Net_RESEARCH_GATE,
   Net_RESEARCHERID,
   Net_SCOOPIT,
   Net_SLIDESHARE,
   Net_STORIFY,
   Net_TUMBLR,
   Net_TWITTER,
   Net_WIKIPEDIA,
   Net_YOUTUBE,
  } Net_WebsAndSocialNetworks_t;

const char *Net_WebsAndSocialNetworksDB[Net_NUM_WEBS_AND_SOCIAL_NETWORKS] =
  {
   "www",		// Net_WWW
   "500px",		// Net_500PX
   "delicious",		// Net_DELICIOUS
   "deviantart",	// Net_DEVIANTART
   "diaspora",		// Net_DIASPORA
   "edmodo",		// Net_EDMODO
   "facebook",		// Net_FACEBOOK
   "flickr",		// Net_FLICKR
   "foursquare",	// Net_FOURSQUAREhub
   "github",		// Net_GITHUB
   "gnusocial",		// Net_GNU_SOCIAL
   "googleplus",	// Net_GOOGLE_PLUS
   "googlescholar",	// Net_GOOGLE_SCHOLAR
   "identica",		// Net_IDENTICA
   "instagram",		// Net_INSTAGRAM
   "linkedin",		// Net_LINKEDIN
   "orcid",		// Net_ORCID
   "paperli",		// Net_PAPERLI
   "pinterest",		// Net_PINTEREST
   "quitter",		// Net_QUITTER
   "researchgate",	// Net_RESEARCH_GATE
   "researcherid",	// Net_RESEARCHERID
   "scoopit",		// Net_SCOOPIT
   "slideshare",	// Net_SLIDESHARE
   "storify",		// Net_STORIFY
   "tumblr",		// Net_TUMBLR
   "twitter",		// Net_TWITTER
   "wikipedia",		// Net_WIKIPEDIA
   "youtube",		// Net_YOUTUBE
  };

const char *Net_TitleWebsAndSocialNetworks[Net_NUM_WEBS_AND_SOCIAL_NETWORKS] =
  {
   "WWW",		// Net_WWW
   "500px",		// Net_500PX
   "Delicious",		// Net_DELICIOUS
   "DeviantArt",	// Net_DEVIANTART
   "Diaspora",		// Net_DIASPORA
   "Edmodo",		// Net_EDMODO
   "Facebook",		// Net_FACEBOOK
   "Flickr",		// Net_FLICKR
   "Foursquare",	// Net_FOURSQUARE
   "GitHub",		// Net_GITHUB
   "GNU Social",	// Net_GNU_SOCIAL
   "Google+",		// Net_GOOGLE_PLUS
   "Google Scholar",	// Net_GOOGLE_SCHOLAR
   "identi.ca",		// Net_IDENTICA
   "Instagram",		// Net_INSTAGRAM
   "LinkedIn",		// Net_LINKEDIN
   "ORCID",		// Net_ORCID
   "Paper.li",		// Net_PAPERLI
   "Pinterest",		// Net_PINTEREST
   "quitter",		// Net_QUITTER
   "Research Gate",	// Net_RESEARCH_GATE
   "ResearcherID",	// Net_RESEARCHERID
   "Scoop.it",		// Net_SCOOPIT
   "Slideshare",	// Net_SLIDESHARE
   "Storify",		// Net_STORIFY
   "Tumblr",		// Net_TUMBLR
   "Twitter",		// Net_TWITTER
   "Wikipedia",		// Net_WIKIPEDIA
   "YouTube",		// Net_YOUTUBE
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Net_ShowAWebOrSocialNet (const char *URL,
                                     const char *Icon,const char *Title);
static void Net_GetMyWebsAndSocialNetsFromForm (void);

/*****************************************************************************/
/************************** Show webs / social networks **********************/
/*****************************************************************************/

void Net_ShowWebsAndSocialNets (const struct UsrData *UsrDat)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Net_WebsAndSocialNetworks_t NumURL;
   char URL[Cns_MAX_BYTES_URL+1];

   /***** Start container *****/
   fprintf (Gbl.F.Out,"<div style=\"width:180px; margin:0 auto;\">");

   /***** Show link to public profile *****/
   Net_ShowAWebOrSocialNet (Prf_GetURLPublicProfile (URL,UsrDat->Nickname),
                            "swad",Cfg_PLATFORM_PAGE_TITLE);

   /***** Show the rest of webs / social networks *****/
   for (NumURL = (Net_WebsAndSocialNetworks_t) 0;
	NumURL < Net_NUM_WEBS_AND_SOCIAL_NETWORKS;
	NumURL++)
     {
      /***** Get user's web / social network from database *****/
      sprintf (Query,"SELECT URL FROM usr_webs"
		     " WHERE UsrCod='%ld' AND Web='%s'",
	       UsrDat->UsrCod,Net_WebsAndSocialNetworksDB[NumURL]);

      /***** Check if exists the web / social network for this user *****/
      if (DB_QuerySELECT (Query,&mysql_res,"can not get user's web / social network"))
	{
	 /* Get URL */
	 row = mysql_fetch_row (mysql_res);
	 strncpy (URL,row[0],Cns_MAX_BYTES_URL);
	 URL[Cns_MAX_BYTES_URL] = '\0';

	 /* Show the web / social network */
	 Net_ShowAWebOrSocialNet (URL,
	                          Net_WebsAndSocialNetworksDB[NumURL],
	                          Net_TitleWebsAndSocialNetworks[NumURL]);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** End container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************************** Show a web / social network **********************/
/*****************************************************************************/

static void Net_ShowAWebOrSocialNet (const char *URL,
                                     const char *Icon,const char *Title)
  {
   /***** Write link and icon *****/
   fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\""
		      " style=\"display:inline;\">"
		      "<a href=\"%s\" target=\"_blank\" title=\"%s\">"
		      "<img src=\"%s/%s16x16.gif\""
		      " alt=\"%s\" title=\"%s\""
                      " class=\"ICON16x16\" />"
		      "</a>"
		      "</div>",
	    URL,Title,
	    Gbl.Prefs.IconsURL,Icon,
	    Title,Title);
  }

/*****************************************************************************/
/********************* Show form to edit my social networks ******************/
/*****************************************************************************/

#define Net_COL2_WIDTH 500

void Net_ShowFormMyWebsAndSocialNets (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Webs_social_networks;
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Net_WebsAndSocialNetworks_t NumURL;
   char URL[Cns_MAX_BYTES_URL+1];

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,2,Txt_Webs_social_networks);

   for (NumURL = (Net_WebsAndSocialNetworks_t) 0;
	NumURL < Net_NUM_WEBS_AND_SOCIAL_NETWORKS;
	NumURL++)
     {
      /***** Get user's web / social network from database *****/
      sprintf (Query,"SELECT URL FROM usr_webs"
		     " WHERE UsrCod='%ld' AND Web='%s'",
	       Gbl.Usrs.Me.UsrDat.UsrCod,
	       Net_WebsAndSocialNetworksDB[NumURL]);

      /***** Check number of rows in result *****/
      if (DB_QuerySELECT (Query,&mysql_res,"can not get user's web / social network"))
	{
	 /***** Read the data comunes a all the users *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get URL */
	 strncpy (URL,row[0],Cns_MAX_BYTES_URL);
	 URL[Cns_MAX_BYTES_URL] = '\0';
	}
      else
	 URL[0] = '\0';

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Row for this web / social network *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s LEFT_MIDDLE\">"
			 "<img src=\"%s/%s16x16.gif\""
			 " alt=\"%s\" title=\"%s\""
                         " class=\"ICON16x16\""
			 " style=\"margin-right:12px;\" />"
			 "%s:</td>"
			 "<td class=\"LEFT_MIDDLE\" style=\"width:%upx;\">",
	       The_ClassForm[Gbl.Prefs.Theme],
	       Gbl.Prefs.IconsURL,Net_WebsAndSocialNetworksDB[NumURL],
	       Net_TitleWebsAndSocialNetworks[NumURL],
	       Net_TitleWebsAndSocialNetworks[NumURL],
	       Net_TitleWebsAndSocialNetworks[NumURL],
	       Net_COL2_WIDTH);
      Act_FormStart (ActChgMyNet);
      Par_PutHiddenParamUnsigned ("Web",(unsigned) NumURL);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"URL\""
			 " style=\"width:500px;\" maxlength=\"%u\" value=\"%s\""
			 " onchange=\"document.getElementById('%s').submit();\" />",
	       Cns_MAX_LENGTH_URL,URL,
	       Gbl.FormId);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/**************** Update and show data from identified user ******************/
/*****************************************************************************/

void Net_UpdateMyWebsAndSocialNets (void)
  {
   /***** Get my web and social networks from form *****/
   Net_GetMyWebsAndSocialNetsFromForm ();

   /***** Show form again *****/
   Net_ShowFormMyWebsAndSocialNets ();
  }

/*****************************************************************************/
/********* Get data fields about web and social networks from form ***********/
/*****************************************************************************/

static void Net_GetMyWebsAndSocialNetsFromForm (void)
  {
   char Query[256+Cns_MAX_BYTES_URL];
   char UnsignedStr[10+1];
   unsigned UnsignedNum;
   Net_WebsAndSocialNetworks_t Web;
   char URL[Cns_MAX_BYTES_URL+1];

   /***** Get parameter with the type of web / social network *****/
   Par_GetParToText ("Web",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) != 1)
      Lay_ShowErrorAndExit ("Web / social network is missing.");
   if (UnsignedNum >= Net_NUM_WEBS_AND_SOCIAL_NETWORKS)
      Lay_ShowErrorAndExit ("Wrong web / social network.");
   Web = (Net_WebsAndSocialNetworks_t) UnsignedNum;

   /***** Get URL *****/
   Par_GetParToText ("URL",URL,Cns_MAX_BYTES_URL);
   if (URL[0])
     {
      /***** Insert or replace web / social network *****/
      sprintf (Query,"REPLACE INTO usr_webs (UsrCod,Web,URL)"
		     " VALUES ('%ld','%s','%s')",
	       Gbl.Usrs.Me.UsrDat.UsrCod,
	       Net_WebsAndSocialNetworksDB[Web],
	       URL);
      DB_QueryREPLACE (Query,"can not update user's web / social network");
     }
   else
     {
      /***** Remove web / social network *****/
      sprintf (Query,"DELETE FROM usr_webs WHERE UsrCod='%ld' AND Web='%s'",
	       Gbl.Usrs.Me.UsrDat.UsrCod,
	       Net_WebsAndSocialNetworksDB[Web]);
      DB_QueryREPLACE (Query,"can not remove user's web / social network");
     }
  }

/*****************************************************************************/
/************** Show stats about users' webs / social networks ***************/
/*****************************************************************************/

void Net_ShowWebAndSocialNetworksStats (void)
  {
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Web_social_network;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows,NumRow;
   Net_WebsAndSocialNetworks_t Web;
   char WebStr[32];
   unsigned NumUsrsTotalInPlatform;
   unsigned NumUsrs;

   /***** Get total number of users in platform *****/
   NumUsrsTotalInPlatform = Sta_GetTotalNumberOfUsersInCourses (Gbl.Scope.Current,Rol_UNKNOWN);

   /***** Get number of users with a web / social network *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT Web,COUNT(*) AS N"
                        " FROM usr_webs"
                        " GROUP BY Web"
                        " ORDER BY N DESC,Web");
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT usr_webs.Web,COUNT(DISTINCT usr_webs.UsrCod) AS N"
                        " FROM institutions,centres,degrees,courses,crs_usr,usr_webs"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_webs.UsrCod"
                        " GROUP BY usr_webs.Web"
                        " ORDER BY N DESC,usr_webs.Web",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT usr_webs.Web,COUNT(DISTINCT usr_webs.UsrCod) AS N"
                        " FROM centres,degrees,courses,crs_usr,usr_webs"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_webs.UsrCod"
                        " GROUP BY usr_webs.Web"
                        " ORDER BY N DESC,usr_webs.Web",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT usr_webs.Web,COUNT(DISTINCT usr_webs.UsrCod) AS N"
                        " FROM degrees,courses,crs_usr,usr_webs"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_webs.UsrCod"
                        " GROUP BY usr_webs.Web"
                        " ORDER BY N DESC,usr_webs.Web",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT usr_webs.Web,COUNT(DISTINCT usr_webs.UsrCod) AS N"
                        " FROM courses,crs_usr,usr_webs"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_webs.UsrCod"
                        " GROUP BY usr_webs.Web"
                        " ORDER BY N DESC,usr_webs.Web",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT usr_webs.Web,COUNT(DISTINCT usr_webs.UsrCod) AS N"
                        " FROM crs_usr,usr_webs"
                        " WHERE crs_usr.CrsCod='%ld'"
                        " AND crs_usr.UsrCod=usr_webs.UsrCod"
                        " GROUP BY usr_webs.Web"
                        " ORDER BY N DESC,usr_webs.Web",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   NumRows = (unsigned) DB_QuerySELECT (Query,&mysql_res,
                                        "can not get number of users with webs / social networks");

   /***** Number of users *****/
   Lay_StartRoundFrameTable (NULL,2,Txt_STAT_USE_STAT_TYPES[Sta_SOCIAL_NETWORKS]);

   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Web_social_network,
            Txt_No_of_users,
            Txt_PERCENT_of_users);

   /***** For each web / social network... *****/
   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get web / social network (row[0]) */
      strncpy (WebStr,row[0],sizeof (WebStr) - 1);
      WebStr[sizeof (WebStr) - 1] = '\0';
      for (Web = (Net_WebsAndSocialNetworks_t) 0;
	   Web < Net_NUM_WEBS_AND_SOCIAL_NETWORKS;
	   Web++)
	 if (!strcmp (Net_WebsAndSocialNetworksDB[Web],WebStr))
	    break;
      if (Web < Net_NUM_WEBS_AND_SOCIAL_NETWORKS)
	{
	 /* Get number of users (row[1]) */
	 if (sscanf (row[1],"%u",&NumUsrs) != 1)
	    Lay_ShowErrorAndExit ("Error when getting number of files.");

	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "<img src=\"%s/%s16x16.gif\""
			    " alt=\"%s\" title=\"%s\""
                            " class=\"ICON16x16\""
			    " style=\"margin-right:12px;\" />"
			    "%s</td>"
			    "<td class=\"DAT RIGHT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "<td class=\"DAT RIGHT_MIDDLE\">"
			    "%.2f%%"
			    "</td>"
			    "</tr>",
		  Gbl.Prefs.IconsURL,Net_WebsAndSocialNetworksDB[Web],
		  Net_TitleWebsAndSocialNetworks[Web],
		  Net_TitleWebsAndSocialNetworks[Web],
		  Net_TitleWebsAndSocialNetworks[Web],
		  NumUsrs,
		  NumUsrsTotalInPlatform ? 100.0 * (float) NumUsrs / (float) NumUsrsTotalInPlatform :
			                   0.0);
	}
     }

   /***** End frame *****/
   Lay_EndRoundFrameTable ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
