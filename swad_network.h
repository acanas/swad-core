// swad_network.h: users' webs and social networks

#ifndef _SWAD_NET
#define _SWAD_NET
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Net_NUM_WEBS_AND_SOCIAL_NETWORKS 30

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

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
   Net_TWITCH,
   Net_TWITTER,
   Net_WIKIPEDIA,
   Net_YOUTUBE,
  } Net_WebsAndSocialNetworks_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Net_ShowWebsAndSocialNets (const struct Usr_Data *UsrDat);

void Net_ShowFormMyWebsAndSocialNets (void);
void Net_UpdateMyWebsAndSocialNets (void);
void Net_ShowWebAndSocialNetworksStats (void);

void Net_DB_RemoveUsrWebs (long UsrCod);

#endif
