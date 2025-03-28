// swad_notice.h: notices (yellow notes)

#ifndef _SWAD_NOT
#define _SWAD_NOT
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_constant.h"
#include "swad_hidden_visible.h"

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

#define Not_NUM_TYPES_LISTING 2
typedef enum
  {
   Not_LIST_BRIEF_NOTICES = 0,
   Not_LIST_FULL_NOTICES  = 1,
  } Not_Listing_t;

struct Not_Notice
  {
   long NotCod;
   long UsrCod;
   time_t CreatTime;
   HidVis_HiddenOrVisible_t HiddenOrVisible;
   // TODO: char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Content[Cns_MAX_BYTES_TEXT + 1];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Not_ShowFormNotice (void);
void Not_ReceiveNotice (void);
void Not_ListNoticesAfterRemoval (void);
void Not_ListFullNotices (void);
void Not_GetParHighlightNotCod (void);

void Not_HideNotice (void);
void Not_UnhideNotice (void);

void Not_ReqRemNotice (void);
void Not_RemoveNotice (void);

void Not_ShowNotices (Not_Listing_t TypeNoticesListing,long HighlightNotCod);

void Not_GetSummaryAndContentNotice (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                     char **ContentStr,
                                     long NotCod,Ntf_GetContent_t GetContent);
unsigned Not_GetNumNotices (Hie_Level_t HieLvl,HidVis_HiddenOrVisible_t HiddenOrVisible,
			    unsigned *NumNotif);
unsigned Not_GetNumNoticesDeleted (Hie_Level_t HieLvl,unsigned *NumNotif);

//-------------------------------- Figures ------------------------------------
void Not_GetAndShowNoticesStats (Hie_Level_t HieLvl);

#endif
