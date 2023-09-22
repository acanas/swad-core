// swad_network_database.c: users' webs and social networks, operations with database

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_network.h"
#include "swad_network_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *Net_DB_WebsAndSocialNetworks[Net_NUM_WEBS_AND_SOCIAL_NETWORKS] =
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
   [Net_TWITCH        ] = "twitch",
   [Net_TWITTER       ] = "twitter",
   [Net_WIKIPEDIA     ] = "wikipedia",
   [Net_YOUTUBE       ] = "youtube",
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************* Insert or replace web / social network ******************/
/*****************************************************************************/

void Net_DB_UpdateMyWeb (Net_WebsAndSocialNetworks_t NumURL,
                         const char URL[Cns_MAX_BYTES_WWW + 1])
  {
   DB_QueryREPLACE ("can not update user's web / social network",
		    "REPLACE INTO usr_webs"
		    " (UsrCod,Web,URL)"
		    " VALUES"
		    " (%ld,'%s','%s')",
		    Gbl.Usrs.Me.UsrDat.UsrCod,
		    Net_DB_WebsAndSocialNetworks[NumURL],
		    URL);
  }

/*****************************************************************************/
/*************************** Get web / social network ************************/
/*****************************************************************************/

void Net_DB_GetURL (long UsrCod,Net_WebsAndSocialNetworks_t NumURL,
                    char URL[Cns_MAX_BYTES_WWW + 1])
  {
   DB_QuerySELECTString (URL,Cns_MAX_BYTES_WWW,
			 "can not get user's web / social network",
			 "SELECT URL"
			  " FROM usr_webs"
			 " WHERE UsrCod=%ld"
			   " AND Web='%s'",
			 UsrCod,
			 Net_DB_WebsAndSocialNetworks[NumURL]);
  }

/*****************************************************************************/
/*************** Get stats about users' webs / social networks ***************/
/*****************************************************************************/

unsigned Net_DB_GetWebAndSocialNetworksStats (MYSQL_RES **mysql_res)
  {
   /***** Get number of users with a web / social network *****/
   switch (Gbl.Scope.Current)
     {
      case Hie_SYS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of users"
				    " with webs / social networks",
			 "SELECT Web,"					// row[0]
			        "COUNT(*) AS N"				// row[1]
			  " FROM usr_webs"
		      " GROUP BY Web"
		      " ORDER BY N DESC,"
			        "Web");
      case Hie_CTY:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of users"
				    " with webs / social networks",
			 "SELECT usr_webs.Web,"				// row[0]
			        "COUNT(DISTINCT usr_webs.UsrCod) AS N"	// row[1]
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_webs"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_webs.UsrCod"
		      " GROUP BY usr_webs.Web"
		      " ORDER BY N DESC,"
			        "usr_webs.Web",
			 Gbl.Hierarchy.Node[Hie_CTY].HieCod);
      case Hie_INS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of users"
				    " with webs / social networks",
			 "SELECT usr_webs.Web,"				// row[0]
			        "COUNT(DISTINCT usr_webs.UsrCod) AS N"	// row[1]
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_webs"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_webs.UsrCod"
		      " GROUP BY usr_webs.Web"
		      " ORDER BY N DESC,"
			        "usr_webs.Web",
			 Gbl.Hierarchy.Node[Hie_INS].HieCod);
      case Hie_CTR:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of users"
				    " with webs / social networks",
			 "SELECT usr_webs.Web,"				// row[0]
			        "COUNT(DISTINCT usr_webs.UsrCod) AS N"	// row[1]
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_webs"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_webs.UsrCod"
		      " GROUP BY usr_webs.Web"
		      " ORDER BY N DESC,"
			        "usr_webs.Web",
			 Gbl.Hierarchy.Node[Hie_CTR].HieCod);
      case Hie_DEG:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of users"
				    " with webs / social networks",
			 "SELECT usr_webs.Web,"				// row[0]
			        "COUNT(DISTINCT usr_webs.UsrCod) AS N"	// row[1]
			  " FROM crs_courses,"
			        "crs_users,"
			        "usr_webs"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_webs.UsrCod"
		      " GROUP BY usr_webs.Web"
		      " ORDER BY N DESC,"
			        "usr_webs.Web",
			 Gbl.Hierarchy.Node[Hie_DEG].HieCod);
      case Hie_CRS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of users"
				    " with webs / social networks",
			 "SELECT usr_webs.Web,"				// row[0]
			        "COUNT(DISTINCT usr_webs.UsrCod) AS N"	// row[1]
			  " FROM crs_users,"
			        "usr_webs"
			 " WHERE crs_users.CrsCod=%ld"
			   " AND crs_users.UsrCod=usr_webs.UsrCod"
		      " GROUP BY usr_webs.Web"
		      " ORDER BY N DESC,"
			        "usr_webs.Web",
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod);
         break;
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/*********************** Remove my web / social network **********************/
/*****************************************************************************/

void Net_DB_RemoveMyWeb (Net_WebsAndSocialNetworks_t NumURL)
  {
   DB_QueryDELETE ("can not remove user's web / social network",
		   "DELETE FROM usr_webs"
		   " WHERE UsrCod=%ld"
		     " AND Web='%s'",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Net_DB_WebsAndSocialNetworks[NumURL]);
  }

/*****************************************************************************/
/******************* Remove user's webs / social networks ********************/
/*****************************************************************************/

void Net_DB_RemoveUsrWebs (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user's webs / social networks",
		   "DELETE FROM usr_webs"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }
