// swad_network.c: users' webs and social networks

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <string.h>

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_profile.h"
#include "swad_network.h"
#include "swad_network_database.h"
#include "swad_theme.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Net_WEB_SOCIAL_NET_DEFAULT Net_WWW

#define Net_MAX_BYTES_NETWORK_NAME 32

static const char *Net_WebsAndSocialNetworksIcons[Net_NUM_WEBS_AND_SOCIAL_NETWORKS] =
  {
   [Net_WWW           ] = "globe-americas.svg",
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
   [Net_TWITCH        ] = "twitch-brands.svg",
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
   [Net_TWITCH        ] = "Twitch",
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

static void Net_PutIconsWebsSocialNetworks (__attribute__((unused)) void *Args);

static void Net_GetMyWebsAndSocialNetsFromForm (void);

/*****************************************************************************/
/************************** Show webs / social networks **********************/
/*****************************************************************************/

void Net_ShowWebsAndSocialNets (const struct Usr_Data *UsrDat)
  {
   Net_WebsAndSocialNetworks_t NumURL;
   char URL[WWW_MAX_BYTES_WWW + 1];

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"REC_WEBS\"");

      /***** Show link to public profile *****/
      Net_ShowAWebOrSocialNet (Prf_GetURLPublicProfile (URL,UsrDat->Nickname),
			       "swad64x64.png",Cfg_PLATFORM_SHORT_NAME);

      /***** Show the rest of webs / social networks *****/
      for (NumURL  = (Net_WebsAndSocialNetworks_t) 0;
	   NumURL <= (Net_WebsAndSocialNetworks_t) (Net_NUM_WEBS_AND_SOCIAL_NETWORKS - 1);
	   NumURL++)
	{
	 /***** Check if exists the web / social network for this user *****/
	 Net_DB_GetURL (UsrDat->UsrCod,NumURL,URL);
	 if (URL[0])
	    /* Show the web / social network */
	    Net_ShowAWebOrSocialNet (URL,
				     Net_WebsAndSocialNetworksIcons[NumURL],
				     Net_WebsAndSocialNetworksTitle[NumURL]);
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
	 Ico_PutIcon (Icon,Ico_BLACK,Title,"ICO16x16");
      HTM_A_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Show form to edit my social networks ******************/
/*****************************************************************************/

void Net_ShowFormMyWebsAndSocialNets (void)
  {
   extern const char *Hlp_PROFILE_Webs;
   extern const char *Txt_Webs_social_networks;
   Net_WebsAndSocialNetworks_t NumURL;
   char URL[WWW_MAX_BYTES_WWW + 1];
   char StrName[32];

   /***** Begin section *****/
   HTM_SECTION_Begin (Net_MY_WEBS_ID);

      /***** Begin box *****/
      HTM_DIV_Begin ("class=\"REC_CONT\"");
	 Box_BoxBegin (Txt_Webs_social_networks,
		       Net_PutIconsWebsSocialNetworks,NULL,
		       Hlp_PROFILE_Webs,Box_NOT_CLOSABLE);

	    /***** Begin form *****/
	    Frm_BeginFormAnchor (ActChgMyNet,Net_MY_WEBS_ID);

	       /***** Begin table *****/
	       HTM_TABLE_BeginPadding (2);

		  /***** List webs and social networks *****/
		  for (NumURL  = (Net_WebsAndSocialNetworks_t) 0;
		       NumURL <= (Net_WebsAndSocialNetworks_t) (Net_NUM_WEBS_AND_SOCIAL_NETWORKS - 1);
		       NumURL++)
		    {
		     /***** Get user's web / social network from database *****/
		     Net_DB_GetURL (Gbl.Usrs.Me.UsrDat.UsrCod,NumURL,URL);

		     /***** Row for this web / social network *****/
		     snprintf (StrName,sizeof (StrName),"URL%u",(unsigned) NumURL);
		     HTM_TR_Begin (NULL);

			HTM_TD_Begin ("class=\"REC_C1_BOT LM\"");
			   HTM_LABEL_Begin ("for=\"%s\" class=\"FORM_IN_%s\"",
					    StrName,The_GetSuffix ());
			      Ico_PutIcon (Net_WebsAndSocialNetworksIcons[NumURL],Ico_BLACK,
					   Net_WebsAndSocialNetworksTitle[NumURL],
					   "CONTEXT_OPT CONTEXT_ICO16x16");
			      HTM_TxtF ("&nbsp;%s:",Net_WebsAndSocialNetworksTitle[NumURL]);
			   HTM_LABEL_End ();
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"REC_C2_BOT LM\"");
			   HTM_INPUT_URL (StrName,URL,
					  HTM_NO_ATTR,
					  "id=\"%s\""
					  " class=\"REC_C2_BOT_INPUT INPUT_%s\"",
					  StrName,The_GetSuffix ());
			HTM_TD_End ();

		     HTM_TR_End ();
		    }

	       /***** End table *****/
	       HTM_TABLE_End ();

	       /***** Confirm button *****/
	       Btn_PutButton (Btn_SAVE_CHANGES);

	    /***** End form *****/
	    Frm_EndForm ();

	 /***** End box *****/
	 Box_BoxEnd ();
      HTM_DIV_End ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/************** Put contextual icons in my web / social networks *************/
/*****************************************************************************/

static void Net_PutIconsWebsSocialNetworks (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_SOCIAL_NETWORKS);
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
   char ParName[3 + 10 + 1];
   char URL[WWW_MAX_BYTES_WWW + 1];

   /***** Get URLs *****/
   for (NumURL  = (Net_WebsAndSocialNetworks_t) 0;
	NumURL <= (Net_WebsAndSocialNetworks_t) (Net_NUM_WEBS_AND_SOCIAL_NETWORKS - 1);
	NumURL++)
     {
      /***** Get URL from the form *****/
      snprintf (ParName,sizeof (ParName),"URL%u",(unsigned) NumURL);
      Par_GetParText (ParName,URL,WWW_MAX_BYTES_WWW);

      if (URL[0])
	 /***** Insert or replace web / social network *****/
	 Net_DB_UpdateMyWeb (NumURL,URL);
      else
	 /***** Remove web / social network *****/
	 Net_DB_RemoveMyWeb (NumURL);
     }
  }

/*****************************************************************************/
/************** Show stats about users' webs / social networks ***************/
/*****************************************************************************/

void Net_ShowWebAndSocialNetworksStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_webs_social_networks;
   extern const char *Net_DB_WebsAndSocialNetworks[Net_NUM_WEBS_AND_SOCIAL_NETWORKS];
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Web_social_network;
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNetworks;
   unsigned NumNetwork;
   Net_WebsAndSocialNetworks_t Web;
   char NetName[Net_MAX_BYTES_NETWORK_NAME + 1];
   unsigned NumUsrsTotal;
   unsigned NumUsrs;

   /***** Get total number of users in current scope *****/
   NumUsrsTotal = Usr_GetTotalNumberOfUsers ();

   /***** Get number of users with a web / social network *****/
   NumNetworks = Net_DB_GetWebAndSocialNetworksStats (&mysql_res);

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_SOCIAL_NETWORKS],NULL,NULL,
                      Hlp_ANALYTICS_Figures_webs_social_networks,Box_NOT_CLOSABLE,2);

      /***** Write heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Web_social_network,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_users   ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users  ,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** For each web / social network... *****/
      for (NumNetwork = 0;
	   NumNetwork < NumNetworks;
	   NumNetwork++)
	{
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get web / social network (row[0]) */
	 Str_Copy (NetName,row[0],sizeof (NetName) - 1);
	 for (Web  = (Net_WebsAndSocialNetworks_t) 0;
	      Web <= (Net_WebsAndSocialNetworks_t) (Net_NUM_WEBS_AND_SOCIAL_NETWORKS - 1);
	      Web++)
	    if (!strcmp (Net_DB_WebsAndSocialNetworks[Web],NetName))
	       break;
	 if (Web < Net_NUM_WEBS_AND_SOCIAL_NETWORKS)
	   {
	    /* Get number of users (row[1]) */
	    if (sscanf (row[1],"%u",&NumUsrs) != 1)
	       Err_ShowErrorAndExit ("Error when getting number of files.");

	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	          Ico_PutIcon (Net_WebsAndSocialNetworksIcons[Web],Ico_BLACK,
	                       Net_WebsAndSocialNetworksTitle[Web],"ICOx16");
	          HTM_NBSPTxt (Net_WebsAndSocialNetworksTitle[Web]);
	       HTM_TD_End ();

	       HTM_TD_Unsigned (NumUsrs);
	       HTM_TD_Percentage (NumUsrs,NumUsrsTotal);

	    HTM_TR_End ();
	   }
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
