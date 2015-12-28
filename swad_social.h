// swad_social.c: social networking

#ifndef _SWAD_SOC
#define _SWAD_SOC
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************** Headers **********************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Soc_NUM_SOCIAL_EVENTS	14
// If the numbers assigned to each event type change,
// it is necessary to change old numbers to new ones in database table social

typedef enum
  {
   Soc_EVENT_UNKNOWN			=  0,

   /* Institution tab */
   Soc_EVENT_INS_DOC_PUB_FILE		=  1,
   Soc_EVENT_INS_SHA_PUB_FILE		=  2,

   /* Centre tab */
   Soc_EVENT_CTR_DOC_PUB_FILE		=  3,
   Soc_EVENT_CTR_SHA_PUB_FILE		=  4,

   /* Degree tab */
   Soc_EVENT_DEG_DOC_PUB_FILE		=  5,
   Soc_EVENT_DEG_SHA_PUB_FILE		=  6,

   /* Course tab */
   Soc_EVENT_CRS_DOC_PUB_FILE		=  7,
   Soc_EVENT_CRS_SHA_PUB_FILE		=  8,

   /* Assessment tab */
   Soc_EVENT_EXAM_ANNOUNCEMENT		=  9,
   Soc_EVENT_MARKS_FILE			= 10,

   /* Enrollment tab */

   /* Social tab */
   Soc_EVENT_SOCIAL_POST		= 11,

   /* Messages tab */
   Soc_EVENT_NOTICE			= 12,
   Soc_EVENT_FORUM_POST			= 13,

   /* Statistics tab */

   /* Profile tab */

  } Soc_SocialEvent_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Soc_ShowSocialActivity (void);

void Soc_StoreSocialEvent (Soc_SocialEvent_t SocialEvent,long Cod);

#endif
