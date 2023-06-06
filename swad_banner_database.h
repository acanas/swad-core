// swad_banner_database.h: banners operations with database

#ifndef _SWAD_BAN_DB
#define _SWAD_BAN_DB
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_banner.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

unsigned Ban_DB_GetAllBanners (MYSQL_RES **mysql_res);
unsigned Ban_DB_GetVisibleBanners (MYSQL_RES **mysql_res);
unsigned Ban_DB_GetRandomBanners (MYSQL_RES **mysql_res);
unsigned Ban_DB_GetBannerDataByCod (MYSQL_RES **mysql_res,long BanCod);
bool Ban_DB_CheckIfBannerNameExists (const char *FldName,const char *Name,long BanCod);

void Ban_DB_CreateBanner (const struct Ban_Banner *Ban);
void Ban_DB_HideOrUnhideBanner (long BanCod,Cns_HiddenOrVisible_t HiddenOrVisible);
void Ban_DB_UpdateBanName (long BanCod,const char *FldName,
			   const char *NewBanName);
void Ban_DB_UpdateBanImg (long BanCod,
                          const char NewImg[Ban_MAX_BYTES_IMAGE + 1]);
void Ban_DB_UpdateBanWWW (long BanCod,
                          const char NewWWW[Cns_MAX_BYTES_WWW + 1]);

void Ban_DB_RemoveBanner (long BanCod);

#endif
