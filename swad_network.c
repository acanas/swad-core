// swad_network.c: users' webs and social networks

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
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

static const char *Net_WebsAndSocialNetworksDB[Net_NUM_WEBS_AND_SOCIAL_NETWORKS] =
  {
   [Net_WWW           ] = "www",
   [Net_500PX         ] = "500px",
   [Net_DELICIOUS     ] = "delicious",
   [Net_DEVIANTART    ] = "deviantart",
   [Net_DIASPORA      ] = "diaspora",
   [Net_EDMODO        ] = "edmodo",
   [Net_FACEBOOK      ] = "facebook",
   [Net_FLICKR        ] = "flickr",
   [Net_FOURSQUARE    ] = "foursquare",
   [Net_GITHUB        ] = "github",
   [Net_GNU_SOCIAL    ] = "gnusocial",
   [Net_GOOGLE_PLUS   ] = "googleplus",
   [Net_GOOGLE_SCHOLAR] = "googlescholar",
   [Net_IDENTICA      ] = "identica",
   [Net_INSTAGRAM     ] = "instagram",
   [Net_LINKEDIN      ] = "linkedin",
   [Net_ORCID         ] = "orcid",
   [Net_PAPERLI       ] = "paperli",
   [Net_PINTEREST     ] = "pinterest",
   [Net_RESEARCH_GATE ] = "researchgate",
   [Net_RESEARCHERID  ] = "researcherid",
   [Net_SCOOPIT       ] = "scoopit",
   [Net_SLIDESHARE    ] = "slideshare",
   [Net_STACK_OVERFLOW] = "stackoverflow",
   [Net_STORIFY       ] = "storify",
   [Net_TUMBLR        ] = "tumblr",
   [Net_TWITTER       ] = "twitter",
   [Net_WIKIPEDIA     ] = "wikipedia",
   [Net_YOUTUBE       ] = "youtube",
  };

static const char *Net_WebsAndSocialNetworksIcons[Net_NUM_WEBS_AND_SOCIAL_NETWORKS] =
  {
   [Net_WWW           ] = "globe.svg",
   [Net_500PX         ] = "500px-brands.svg",
   [Net_DELICIOUS     ] = "delicious-brands.svg",
   [Net_DEVIANTART    ] = "deviantart-brands.svg",
   [Net_DIASPORA      ] = "diaspora-brands.svg",
   [Net_EDMODO        ] = "edmodo64x64.png",
   [Net_FACEBOOK      ] = "facebook-brands.svg",
   [Net_FLICKR        ] = "flickr-brands.svg",
   [Net_FOURSQUARE    ] = "foursquare-brands.svg",
   [Net_GITHUB        ] = "github-brands.svg",
   [Net_GNU_SOCIAL    ] = "gnusocial64x64.png",
   [Net_GOOGLE_PLUS   ] = "google-plus-brands.svg",
   [Net_GOOGLE_SCHOLAR] = "googlescholar.svg",
   [Net_IDENTICA      ] = "identica64x64.png",
   [Net_INSTAGRAM     ] = "instagram-brands.svg",
   [Net_LINKEDIN      ] = "linkedin-brands.svg",
   [Net_ORCID         ] = "orcid64x64.png",
   [Net_PAPERLI       ] = "paperli64x64.png",
   [Net_PINTEREST     ] = "pinterest-brands.svg",
   [Net_RESEARCH_GATE ] = "researchgate-brands.svg",
   [Net_RESEARCHERID  ] = "researcherid64x64.png",
   [Net_SCOOPIT       ] = "scoopit64x64.png",
   [Net_SLIDESHARE    ] = "slideshare-brands.svg",
   [Net_STACK_OVERFLOW] = "stack-overflow-brands.svg",
   [Net_STORIFY       ] = "storify64x64.png",
   [Net_TUMBLR        ] = "tumblr-brands.svg",
   [Net_TWITTER       ] = "twitter-brands.svg",
   [Net_WIKIPEDIA     ] = "wikipedia-w-brands.svg",
   [Net_YOUTUBE       ] = "youtube-brands.svg",
  };

static const char *Net_WebsAndSocialNetworksTitle[Net_NUM_WEBS_AND_SOCIAL_NETWORKS] =
  {
   [Net_WWW           ] = "WWW",
   [Net_500PX         ] = "500px",
   [Net_DELICIOUS     ] = "Delicious",
   [Net_DEVIANTART    ] = "DeviantArt",
   [Net_DIASPORA      ] = "Diaspora",
   [Net_EDMODO        ] = "Edmodo",
   [Net_FACEBOOK      ] = "Facebook",
   [Net_FLICKR        ] = "Flickr",
   [Net_FOURSQUARE    ] = "Foursquare",
   [Net_GITHUB        ] = "GitHub",
   [Net_GNU_SOCIAL    ] = "GNU Social",
   [Net_GOOGLE_PLUS   ] = "Google+",
   [Net_GOOGLE_SCHOLAR] = "Google Scholar",
   [Net_IDENTICA      ] = "identi.ca",
   [Net_INSTAGRAM     ] = "Instagram",
   [Net_LINKEDIN      ] = "LinkedIn",
   [Net_ORCID         ] = "ORCID",
   [Net_PAPERLI       ] = "Paper.li",
   [Net_PINTEREST     ] = "Pinterest",
   [Net_RESEARCH_GATE ] = "Research Gate",
   [Net_RESEARCHERID  ] = "ResearcherID",
   [Net_SCOOPIT       ] = "Scoop.it",
   [Net_SLIDESHARE    ] = "Slideshare",
   [Net_STACK_OVERFLOW] = "Stack Overflow",
   [Net_STORIFY       ] = "Storify",
   [Net_TUMBLR        ] = "Tumblr",
   [Net_TWITTER       ] = "Twitter",
   [Net_WIKIPEDIA     ] = "Wikipedia",
   [Net_YOUTUBE       ] = "YouTube",
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
   HTM_DIV_Begin ("class=\"REC_WEBS\"");

   /***** Show link to public profile *****/
   Net_ShowAWebOrSocialNet (Prf_GetURLPublicProfile (URL,UsrDat->Nickname),
                            "swad64x64.png",Cfg_PLATFORM_SHORT_NAME);

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
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************** Show a web / social network **********************/
/*****************************************************************************/

static void Net_ShowAWebOrSocialNet (const char *URL,
                                     const char *Icon,const char *Title)
  {
   /***** Write link and icon *****/
   HTM_DIV_Begin ("class=\"ICO_HIGHLIGHT\" style=\"display:inline;\"");
   HTM_A_Begin ("href=\"%s\" target=\"_blank\" title=\"%s\"",URL,Title);
   Ico_PutIcon (Icon,Title,"ICO16x16");
   HTM_A_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Show form to edit my social networks ******************/
/*****************************************************************************/

void Net_ShowFormMyWebsAndSocialNets (void)
  {
   extern const char *Hlp_PROFILE_Webs;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Webs_social_networks;
   extern const char *Txt_Save_changes;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Net_WebsAndSocialNetworks_t NumURL;
   char URL[Cns_MAX_BYTES_WWW + 1];
   char StrRecordWidth[Cns_MAX_DECIMAL_DIGITS_UINT + 2 + 1];
   char StrName[32];

   /***** Start section *****/
   HTM_SECTION_Begin (Net_MY_WEBS_ID);

   /***** Begin box *****/
   snprintf (StrRecordWidth,sizeof (StrRecordWidth),
	     "%upx",
	     Rec_RECORD_WIDTH);
   Box_BoxBegin (StrRecordWidth,Txt_Webs_social_networks,
	         Net_PutIconsWebsSocialNetworks,
                 Hlp_PROFILE_Webs,Box_NOT_CLOSABLE);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

   /***** Begin form *****/
   Frm_StartFormAnchor (ActChgMyNet,Net_MY_WEBS_ID);

   /***** List webs and social networks *****/
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
      snprintf (StrName,sizeof (StrName),
		"URL%u",
		(unsigned) NumURL);
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"REC_C1_BOT LM\"");
      HTM_LABEL_Begin ("for=\"%s\" class=\"%s\"",
	               StrName,The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_IMG (Cfg_URL_ICON_PUBLIC,Net_WebsAndSocialNetworksIcons[NumURL],Net_WebsAndSocialNetworksTitle[NumURL],
	       "class=\"CONTEXT_ICO_16x16\" style=\"margin-right:6px;\"");
      HTM_TxtF ("%s:",Net_WebsAndSocialNetworksTitle[NumURL]);
      HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"REC_C2_BOT LM\"");
      HTM_INPUT_URL (StrName,URL,false,
		     "id=\"%s\" class=\"REC_C2_BOT_INPUT\"",StrName);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Confirm button *****/
   Btn_PutConfirmButton (Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/************** Put contextual icons in my web / social networks *************/
/*****************************************************************************/

static void Net_PutIconsWebsSocialNetworks (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_SOCIAL_NETWORKS;
   Fig_PutIconToShowFigure ();
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
   Net_WebsAndSocialNetworks_t NumURL;
   char ParamName[3 + 10 + 1];
   char URL[Cns_MAX_BYTES_WWW + 1];

   /***** Get URLs *****/
   for (NumURL = (Net_WebsAndSocialNetworks_t) 0;
	NumURL < Net_NUM_WEBS_AND_SOCIAL_NETWORKS;
	NumURL++)
     {
      /***** Get URL from the form *****/
      snprintf (ParamName,sizeof (ParamName),
		"URL%u",
		(unsigned) NumURL);
      Par_GetParToText (ParamName,URL,Cns_MAX_BYTES_WWW);

      if (URL[0])
	 /***** Insert or replace web / social network *****/
	 DB_QueryREPLACE ("can not update user's web / social network",
			  "REPLACE INTO usr_webs"
			  " (UsrCod,Web,URL)"
			  " VALUES"
			  " (%ld,'%s','%s')",
			  Gbl.Usrs.Me.UsrDat.UsrCod,
			  Net_WebsAndSocialNetworksDB[NumURL],
			  URL);
      else
	 /***** Remove web / social network *****/
	 DB_QueryDELETE ("can not remove user's web / social network",
			 "DELETE FROM usr_webs WHERE UsrCod=%ld AND Web='%s'",
			 Gbl.Usrs.Me.UsrDat.UsrCod,
			 Net_WebsAndSocialNetworksDB[NumURL]);
     }
  }

/*****************************************************************************/
/************** Show stats about users' webs / social networks ***************/
/*****************************************************************************/

void Net_ShowWebAndSocialNetworksStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_webs_social_networks;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
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
   NumUsrsTotal = (Gbl.Scope.Current == Hie_SYS) ? Usr_GetTotalNumberOfUsersInPlatform () :
                                                         Usr_GetTotalNumberOfUsersInCourses (Gbl.Scope.Current,
                                                                                             1 << Rol_STD |
                                                                                             1 << Rol_NET |
                                                                                             1 << Rol_TCH);

   /***** Get number of users with a web / social network *****/
   switch (Gbl.Scope.Current)
     {
      case Hie_SYS:
         NumRows =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get number of users"
					       " with webs / social networks",
				    "SELECT Web,COUNT(*) AS N"
				    " FROM usr_webs"
				    " GROUP BY Web"
				    " ORDER BY N DESC,Web");
         break;
      case Hie_CTY:
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
				    Gbl.Hierarchy.Cty.CtyCod);
         break;
      case Hie_INS:
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
				    Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
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
				    Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
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
				    Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
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
				    Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_FIGURE_TYPES[Fig_SOCIAL_NETWORKS],NULL,
                      Hlp_ANALYTICS_Figures_webs_social_networks,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"LM",Txt_Web_social_network);
   HTM_TH (1,1,"RM",Txt_No_of_users);
   HTM_TH (1,1,"RM",Txt_PERCENT_of_users);

   HTM_TR_End ();

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

	 HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"DAT LM\"");
	 HTM_IMG (Cfg_URL_ICON_PUBLIC,Net_WebsAndSocialNetworksIcons[Web],Net_WebsAndSocialNetworksTitle[Web],
	          "class=\"CONTEXT_ICO_16x16\" style=\"margin-right:6px;\"");
	 HTM_Txt (Net_WebsAndSocialNetworksTitle[Web]);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT RM\"");
	 HTM_Unsigned (NumUsrs);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT RM\"");
	 HTM_Percentage (NumUsrsTotal ? 100.0 * (double) NumUsrs /
	                                (double) NumUsrsTotal :
			                0.0);
	 HTM_TD_End ();

	 HTM_TR_End ();
	}
     }

   /***** End table and box *****/
   Box_BoxTableEnd ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
