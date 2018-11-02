// swad_network.c: users' webs and social networks

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <string.h>

#include "swad_box.h"
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

#define Net_NUM_WEBS_AND_SOCIAL_NETWORKS 30
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
   Net_STACK_OVERFLOW,
   Net_STORIFY,
   Net_TUMBLR,
   Net_TWITTER,
   Net_WIKIPEDIA,
   Net_YOUTUBE,
  } Net_WebsAndSocialNetworks_t;
#define Net_WEB_SOCIAL_NET_DEFAULT Net_WWW

#define Net_MAX_BYTES_NETWORK_NAME 32

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
   "stackoverflow",	// Net_STACK_OVERFLOW
   "storify",		// Net_STORIFY
   "tumblr",		// Net_TUMBLR
   "twitter",		// Net_TWITTER
   "wikipedia",		// Net_WIKIPEDIA
   "youtube",		// Net_YOUTUBE
  };

const char *Net_WebsAndSocialNetworksIcons[Net_NUM_WEBS_AND_SOCIAL_NETWORKS] =
  {
   "www16x16.gif",		// Net_WWW
   "500px16x16.gif",		// Net_500PX
   "delicious16x16.gif",	// Net_DELICIOUS
   "deviantart16x16.gif",	// Net_DEVIANTART
   "diaspora16x16.gif",		// Net_DIASPORA
   "edmodo16x16.gif",		// Net_EDMODO
   "facebook16x16.gif",		// Net_FACEBOOK
   "flickr16x16.gif",		// Net_FLICKR
   "foursquare16x16.gif",	// Net_FOURSQUAREhub
   "github16x16.gif",		// Net_GITHUB
   "gnusocial16x16.gif",	// Net_GNU_SOCIAL
   "googleplus16x16.gif",	// Net_GOOGLE_PLUS
   "googlescholar16x16.gif",	// Net_GOOGLE_SCHOLAR
   "identica16x16.gif",		// Net_IDENTICA
   "instagram16x16.gif",	// Net_INSTAGRAM
   "linkedin16x16.gif",		// Net_LINKEDIN
   "orcid16x16.gif",		// Net_ORCID
   "paperli16x16.gif",		// Net_PAPERLI
   "pinterest16x16.gif",	// Net_PINTEREST
   "quitter16x16.gif",		// Net_QUITTER
   "researchgate16x16.gif",	// Net_RESEARCH_GATE
   "researcherid16x16.gif",	// Net_RESEARCHERID
   "scoopit16x16.gif",		// Net_SCOOPIT
   "slideshare16x16.gif",	// Net_SLIDESHARE
   "stackoverflow64x64.png",	// Net_STACK_OVERFLOW
   "storify16x16.gif",		// Net_STORIFY
   "tumblr16x16.gif",		// Net_TUMBLR
   "twitter16x16.gif",		// Net_TWITTER
   "wikipedia16x16.gif",	// Net_WIKIPEDIA
   "youtube16x16.gif",		// Net_YOUTUBE
  };

const char *Net_WebsAndSocialNetworksTitle[Net_NUM_WEBS_AND_SOCIAL_NETWORKS] =
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
   "Stack Overflow",	// Net_STACK_OVERFLOW
   "Storify",		// Net_STORIFY
   "Tumblr",		// Net_TUMBLR
   "Twitter",		// Net_TWITTER
   "Wikipedia",		// Net_WIKIPEDIA
   "YouTube",		// Net_YOUTUBE
  };

#define Net_MY_WEBS_ID	"my_webs_section"

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Net_ShowAWebOrSocialNet (const char *URL,
                                     const char *Icon,const char *Title);

static void Net_PutIconsWebsSocialNetworks (void);

static void Net_GetMyWebsAndSocialNetsFromForm (void);

/*****************************************************************************/
/************************** Show webs / social networks **********************/
/*****************************************************************************/

void Net_ShowWebsAndSocialNets (const struct UsrData *UsrDat)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Net_WebsAndSocialNetworks_t NumURL;
   char URL[Cns_MAX_BYTES_WWW + 1];

   /***** Start container *****/
   fprintf (Gbl.F.Out,"<div class=\"REC_WEBS\">");

   /***** Show link to public profile *****/
   Net_ShowAWebOrSocialNet (Prf_GetURLPublicProfile (URL,UsrDat->Nickname),
                            "swad64x64.gif",Cfg_PLATFORM_SHORT_NAME);

   /***** Show the rest of webs / social networks *****/
   for (NumURL = (Net_WebsAndSocialNetworks_t) 0;
	NumURL < Net_NUM_WEBS_AND_SOCIAL_NETWORKS;
	NumURL++)
     {
      /***** Check if exists the web / social network for this user *****/
      if (DB_QuerySELECT (&mysql_res,"can not get user's web / social network",
	                  "SELECT URL FROM usr_webs"
			  " WHERE UsrCod=%ld AND Web='%s'",
			  UsrDat->UsrCod,Net_WebsAndSocialNetworksDB[NumURL]))
	{
	 /* Get URL */
	 row = mysql_fetch_row (mysql_res);
	 Str_Copy (URL,row[0],
	           Cns_MAX_BYTES_WWW);

	 /* Show the web / social network */
	 Net_ShowAWebOrSocialNet (URL,
	                          Net_WebsAndSocialNetworksIcons[NumURL],
	                          Net_WebsAndSocialNetworksTitle[NumURL]);
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
   fprintf (Gbl.F.Out,"<div class=\"ICO_HIGHLIGHT\""
		      " style=\"display:inline;\">"
		      "<a href=\"%s\" target=\"_blank\" title=\"%s\">"
		      "<img src=\"%s/%s\""
		      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />"
		      "</a>"
		      "</div>",
	    URL,Title,
	    Gbl.Prefs.IconsURL,Icon,
	    Title,Title);
  }

/*****************************************************************************/
/********************* Show form to edit my social networks ******************/
/*****************************************************************************/

void Net_ShowFormMyWebsAndSocialNets (void)
  {
   extern const char *Hlp_PROFILE_Webs;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Webs_social_networks;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Net_WebsAndSocialNetworks_t NumURL;
   char URL[Cns_MAX_BYTES_WWW + 1];
   char StrRecordWidth[10 + 1];

   /***** Start section *****/
   Lay_StartSection (Net_MY_WEBS_ID);

   /***** Start box and table *****/
   snprintf (StrRecordWidth,sizeof (StrRecordWidth),
	     "%upx",
	     Rec_RECORD_WIDTH);
   Box_StartBoxTable (StrRecordWidth,
                      Txt_Webs_social_networks,Net_PutIconsWebsSocialNetworks,
                      Hlp_PROFILE_Webs,Box_NOT_CLOSABLE,2);

   for (NumURL = (Net_WebsAndSocialNetworks_t) 0;
	NumURL < Net_NUM_WEBS_AND_SOCIAL_NETWORKS;
	NumURL++)
     {
      /***** Get user's web / social network from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get user's web / social network",
			  "SELECT URL FROM usr_webs"
			  " WHERE UsrCod=%ld AND Web='%s'",
			  Gbl.Usrs.Me.UsrDat.UsrCod,
			  Net_WebsAndSocialNetworksDB[NumURL]))
	{
	 /***** Read the data comunes a all the users *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get URL */
	 Str_Copy (URL,row[0],
	           Cns_MAX_BYTES_WWW);
	}
      else
	 URL[0] = '\0';

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Row for this web / social network *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"REC_C1_BOT LEFT_MIDDLE\">"
			 "<label for=\"URL%u\" class=\"%s\">"
			 "<img src=\"%s/%s\""
			 " alt=\"%s\" title=\"%s\""
                         " class=\"ICO20x20\""
			 " style=\"margin-right:6px;\" />"
			 "%s:"
			 "</label>"
			 "</td>"
			 "<td class=\"REC_C2_BOT LEFT_MIDDLE\">",
	       (unsigned) NumURL,The_ClassForm[Gbl.Prefs.Theme],
	       Gbl.Prefs.IconsURL,Net_WebsAndSocialNetworksIcons[NumURL],
	       Net_WebsAndSocialNetworksTitle[NumURL],
	       Net_WebsAndSocialNetworksTitle[NumURL],
	       Net_WebsAndSocialNetworksTitle[NumURL]);
      Act_StartFormAnchor (ActChgMyNet,Net_MY_WEBS_ID);
      Par_PutHiddenParamUnsigned ("Web",(unsigned) NumURL);
      fprintf (Gbl.F.Out,"<input type=\"url\" id=\"URL%u\" name=\"URL\""
			 " maxlength=\"%u\" value=\"%s\""
		         " class=\"REC_C2_BOT_INPUT\""
			 " onchange=\"document.getElementById('%s').submit();\" />",
	       (unsigned) NumURL,
	       Cns_MAX_CHARS_WWW,URL,
	       Gbl.Form.Id);
      Act_EndForm ();
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   /***** End table and box *****/
   Box_EndBoxTable ();

   /***** End section *****/
   Lay_EndSection ();
  }

/*****************************************************************************/
/************** Put contextual icons in my web / social networks *************/
/*****************************************************************************/

static void Net_PutIconsWebsSocialNetworks (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_SOCIAL_NETWORKS;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/**************** Update and show data from identified user ******************/
/*****************************************************************************/

void Net_UpdateMyWebsAndSocialNets (void)
  {
   /***** Get my web and social networks from form *****/
   Net_GetMyWebsAndSocialNetsFromForm ();

   /***** Show form again *****/
   Rec_ShowMySharedRecordAndMore ();
  }

/*****************************************************************************/
/********* Get data fields about web and social networks from form ***********/
/*****************************************************************************/

static void Net_GetMyWebsAndSocialNetsFromForm (void)
  {
   Net_WebsAndSocialNetworks_t Web;
   char URL[Cns_MAX_BYTES_WWW + 1];

   /***** Get parameter with the type of web / social network *****/
   Web = (Net_WebsAndSocialNetworks_t)
	 Par_GetParToUnsignedLong ("Web",
                                   0,
                                   Net_NUM_WEBS_AND_SOCIAL_NETWORKS - 1,
                                   (unsigned long) Net_WEB_SOCIAL_NET_DEFAULT);

   /***** Get URL *****/
   Par_GetParToText ("URL",URL,Cns_MAX_BYTES_WWW);
   if (URL[0])
      /***** Insert or replace web / social network *****/
      DB_QueryREPLACE ("can not update user's web / social network",
		       "REPLACE INTO usr_webs"
		       " (UsrCod,Web,URL)"
		       " VALUES"
		       " (%ld,'%s','%s')",
		       Gbl.Usrs.Me.UsrDat.UsrCod,
		       Net_WebsAndSocialNetworksDB[Web],
		       URL);
   else
     {
      /***** Remove web / social network *****/
      DB_BuildQuery ("DELETE FROM usr_webs WHERE UsrCod=%ld AND Web='%s'",
		     Gbl.Usrs.Me.UsrDat.UsrCod,
		     Net_WebsAndSocialNetworksDB[Web]);
      DB_QueryDELETE_new ("can not remove user's web / social network");
     }
  }

/*****************************************************************************/
/************** Show stats about users' webs / social networks ***************/
/*****************************************************************************/

void Net_ShowWebAndSocialNetworksStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_webs_social_networks;
   extern const char *Txt_STAT_USE_STAT_TYPES[Sta_NUM_FIGURES];
   extern const char *Txt_Web_social_network;
   extern const char *Txt_No_of_users;
   extern const char *Txt_PERCENT_of_users;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows = 0;	// Initialized to avoid warning
   unsigned NumRow;
   Net_WebsAndSocialNetworks_t Web;
   char NetName[Net_MAX_BYTES_NETWORK_NAME + 1];
   unsigned NumUsrsTotal;
   unsigned NumUsrs;

   /***** Get total number of users in current scope *****/
   NumUsrsTotal = (Gbl.Scope.Current == Sco_SCOPE_SYS) ? Usr_GetTotalNumberOfUsersInPlatform () :
                                                         Usr_GetTotalNumberOfUsersInCourses (Gbl.Scope.Current,
                                                                                             1 << Rol_STD |
                                                                                             1 << Rol_NET |
                                                                                             1 << Rol_TCH);

   /***** Get number of users with a web / social network *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
         NumRows =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get number of users"
					       " with webs / social networks",
				    "SELECT Web,COUNT(*) AS N"
				    " FROM usr_webs"
				    " GROUP BY Web"
				    " ORDER BY N DESC,Web");
         break;
      case Sco_SCOPE_CTY:
         NumRows =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get number of users"
					       " with webs / social networks",
				    "SELECT usr_webs.Web,"
				    "COUNT(DISTINCT usr_webs.UsrCod) AS N"
				    " FROM institutions,centres,degrees,courses,crs_usr,usr_webs"
				    " WHERE institutions.CtyCod=%ld"
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
         NumRows =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get number of users"
					       " with webs / social networks",
				    "SELECT usr_webs.Web,"
				    "COUNT(DISTINCT usr_webs.UsrCod) AS N"
				    " FROM centres,degrees,courses,crs_usr,usr_webs"
				    " WHERE centres.InsCod=%ld"
				    " AND centres.CtrCod=degrees.CtrCod"
				    " AND degrees.DegCod=courses.DegCod"
				    " AND courses.CrsCod=crs_usr.CrsCod"
				    " AND crs_usr.UsrCod=usr_webs.UsrCod"
				    " GROUP BY usr_webs.Web"
				    " ORDER BY N DESC,usr_webs.Web",
				    Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         NumRows =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get number of users"
					       " with webs / social networks",
				    "SELECT usr_webs.Web,"
				    "COUNT(DISTINCT usr_webs.UsrCod) AS N"
				    " FROM degrees,courses,crs_usr,usr_webs"
				    " WHERE degrees.CtrCod=%ld"
				    " AND degrees.DegCod=courses.DegCod"
				    " AND courses.CrsCod=crs_usr.CrsCod"
				    " AND crs_usr.UsrCod=usr_webs.UsrCod"
				    " GROUP BY usr_webs.Web"
				    " ORDER BY N DESC,usr_webs.Web",
				    Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         NumRows =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get number of users"
					       " with webs / social networks",
				    "SELECT usr_webs.Web,"
				    "COUNT(DISTINCT usr_webs.UsrCod) AS N"
				    " FROM courses,crs_usr,usr_webs"
				    " WHERE courses.DegCod=%ld"
				    " AND courses.CrsCod=crs_usr.CrsCod"
				    " AND crs_usr.UsrCod=usr_webs.UsrCod"
				    " GROUP BY usr_webs.Web"
				    " ORDER BY N DESC,usr_webs.Web",
				    Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         NumRows =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get number of users"
					       " with webs / social networks",
				    "SELECT usr_webs.Web,"
				    "COUNT(DISTINCT usr_webs.UsrCod) AS N"
				    " FROM crs_usr,usr_webs"
				    " WHERE crs_usr.CrsCod=%ld"
				    " AND crs_usr.UsrCod=usr_webs.UsrCod"
				    " GROUP BY usr_webs.Web"
				    " ORDER BY N DESC,usr_webs.Web",
				    Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_STAT_USE_STAT_TYPES[Sta_SOCIAL_NETWORKS],NULL,
                      Hlp_ANALYTICS_Figures_webs_social_networks,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
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
      Str_Copy (NetName,row[0],
                Net_MAX_BYTES_NETWORK_NAME);
      for (Web = (Net_WebsAndSocialNetworks_t) 0;
	   Web < Net_NUM_WEBS_AND_SOCIAL_NETWORKS;
	   Web++)
	 if (!strcmp (Net_WebsAndSocialNetworksDB[Web],NetName))
	    break;
      if (Web < Net_NUM_WEBS_AND_SOCIAL_NETWORKS)
	{
	 /* Get number of users (row[1]) */
	 if (sscanf (row[1],"%u",&NumUsrs) != 1)
	    Lay_ShowErrorAndExit ("Error when getting number of files.");

	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "<img src=\"%s/%s\""
			    " alt=\"%s\" title=\"%s\""
                            " class=\"ICO20x20\""
			    " style=\"margin-right:12px;\" />"
			    "%s</td>"
			    "<td class=\"DAT RIGHT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "<td class=\"DAT RIGHT_MIDDLE\">"
			    "%.2f%%"
			    "</td>"
			    "</tr>",
		  Gbl.Prefs.IconsURL,Net_WebsAndSocialNetworksIcons[Web],
		  Net_WebsAndSocialNetworksTitle[Web],
		  Net_WebsAndSocialNetworksTitle[Web],
		  Net_WebsAndSocialNetworksTitle[Web],
		  NumUsrs,
		  NumUsrsTotal ? 100.0 * (float) NumUsrs / (float) NumUsrsTotal :
			         0.0);
	}
     }

   /***** End table and box *****/
   Box_EndBoxTable ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
