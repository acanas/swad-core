// swad_banner.h: banners

#ifndef _SWAD_BANNER
#define _SWAD_BANNER
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Ban_MAX_LENGTH_SHORT_NAME	 32
#define Ban_MAX_LENGTH_FULL_NAME	127
#define Ban_MAX_LENGTH_IMAGE		255

struct Banner
  {
   long BanCod;
   bool Hidden;
   char ShortName[Ban_MAX_LENGTH_SHORT_NAME+1];
   char FullName[Ban_MAX_LENGTH_FULL_NAME+1];
   char Img[Ban_MAX_LENGTH_IMAGE+1];
   char WWW[Cns_MAX_LENGTH_WWW+1];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Ban_SeeBanners (void);
void Ban_EditBanners (void);
void Ban_FreeListBanners (void);
void Ban_GetDataOfBannerByCod (struct Banner *Ban);
long Ban_GetParamLnkCod (void);
void Ban_RemoveBanner (void);
void Ban_ShowBanner (void);
void Ban_HideBanner (void);
void Ban_RenameBannerShort (void);
void Ban_RenameBannerFull (void);
void Ban_ChangeBannerImg (void);
void Ban_ChangeBannerWWW (void);
void Ban_RecFormNewBanner (void);
void Ban_WriteMenuWithBanners (void);
void Ban_ClickOnBanner (void);

#endif
