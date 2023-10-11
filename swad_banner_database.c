// swad_banner_database.c: banners operations with database

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

#include "swad_banner.h"
#include "swad_banner_database.h"
#include "swad_config.h"
#include "swad_database.h"

/*****************************************************************************/
/************************** Get list of all banners **************************/
/*****************************************************************************/

unsigned Ban_DB_GetAllBanners (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get banners",
		   "SELECT BanCod,"	// row[0]
			  "Hidden,"	// row[1]
			  "ShortName,"	// row[2]
			  "FullName,"	// row[3]
			  "Img,"	// row[4]
			  "WWW"		// row[5]
		    " FROM ban_banners"
		   " ORDER BY ShortName");
  }

/*****************************************************************************/
/********************** Get list of visible banners **************************/
/*****************************************************************************/

unsigned Ban_DB_GetVisibleBanners (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get banners",
		   "SELECT BanCod,"	// row[0]
			  "Hidden,"	// row[1]
			  "ShortName,"	// row[2]
			  "FullName,"	// row[3]
			  "Img,"	// row[4]
			  "WWW"		// row[5]
		    " FROM ban_banners"
		   " WHERE Hidden='N'"
		   " ORDER BY ShortName");
  }

/*****************************************************************************/
/********************** Get list of visible banners **************************/
/*****************************************************************************/

unsigned Ban_DB_GetRandomBanners (MYSQL_RES **mysql_res)
  {
   // The banner(s) will change once in a while
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get banners",
		   "SELECT BanCod,"	// row[0]
			  "Hidden,"	// row[1]
			  "ShortName,"	// row[2]
			  "FullName,"	// row[3]
			  "Img,"	// row[4]
			  "WWW"		// row[5]
		    " FROM ban_banners"
		   " WHERE Hidden='N'"
		   " ORDER BY RAND(%lu)"
		   " LIMIT %u",
		   (unsigned long) (Dat_GetStartExecutionTimeUTC () /
				    Cfg_TIME_TO_CHANGE_BANNER),
		   Cfg_NUMBER_OF_BANNERS);
  }

/*****************************************************************************/
/********************* Get data of a banner from database ********************/
/*****************************************************************************/

unsigned Ban_DB_GetBannerDataByCod (MYSQL_RES **mysql_res,long BanCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a banner",
		   "SELECT BanCod,"	// row[0]
			  "Hidden,"	// row[1]
			  "ShortName,"	// row[2]
			  "FullName,"	// row[3]
			  "Img,"	// row[4]
			  "WWW"		// row[5]
		    " FROM ban_banners"
		   " WHERE BanCod=%ld",
		   BanCod);
  }

/*****************************************************************************/
/********************* Check if the name of banner exists ********************/
/*****************************************************************************/

bool Ban_DB_CheckIfBannerNameExists (const char *FldName,const char *Name,long Cod,
				     __attribute__((unused)) long PrtCod,
				     __attribute__((unused)) unsigned Year)
  {
   return
   DB_QueryEXISTS ("can not check if the name of a banner already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM ban_banners"
		    " WHERE %s='%s'"
		      " AND BanCod<>%ld)",
		   FldName,Name,
		   Cod);
  }

/*****************************************************************************/
/**************************** Create a new banner ****************************/
/*****************************************************************************/

void Ban_DB_CreateBanner (const struct Ban_Banner *Ban)
  {
   /***** Create a new banner *****/
   DB_QueryINSERT ("can not create banner",
		   "INSERT INTO ban_banners"
		   " (Hidden,ShortName,FullName,Img,WWW)"
		   " VALUES"
		   " ('N','%s','%s','%s','%s')",
                   Ban->ShrtName,
                   Ban->FullName,
                   Ban->Img,
                   Ban->WWW);
  }

/*****************************************************************************/
/**************************** Hide/unhide a banner ***************************/
/*****************************************************************************/

void Ban_DB_HideOrUnhideBanner (long BanCod,HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char HidVis_YN[HidVis_NUM_HIDDEN_VISIBLE];

   DB_QueryUPDATE ("can not hide/unhide banner",
		   "UPDATE ban_banners"
		     " SET Hidden='%c'"
		   " WHERE BanCod=%ld",
		   HidVis_YN[HiddenOrVisible],
		   BanCod);
  }

/*****************************************************************************/
/***************** Update banner name in table of banners ********************/
/*****************************************************************************/

void Ban_DB_UpdateBanName (long BanCod,
			   const char *FldName,const char *NewBanName)
  {
   DB_QueryUPDATE ("can not update the name of a banner",
		   "UPDATE ban_banners"
		     " SET %s='%s'"
		   " WHERE BanCod=%ld",
	           FldName,NewBanName,
	           BanCod);
  }

/*****************************************************************************/
/************** Update the table changing old image by new image *************/
/*****************************************************************************/

void Ban_DB_UpdateBanImg (long BanCod,
                          const char NewImg[Ban_MAX_BYTES_IMAGE + 1])
  {
   DB_QueryUPDATE ("can not update the image of a banner",
		   "UPDATE ban_banners"
		     " SET Img='%s'"
		   " WHERE BanCod=%ld",
		   NewImg,
		   BanCod);
  }

/*****************************************************************************/
/*************** Update the table changing old WWW by new WWW ****************/
/*****************************************************************************/

void Ban_DB_UpdateBanWWW (long BanCod,
                          const char NewWWW[Cns_MAX_BYTES_WWW + 1])
  {
   DB_QueryUPDATE ("can not update the web of a banner",
		   "UPDATE ban_banners"
		     " SET WWW='%s'"
		   " WHERE BanCod=%ld",
		   NewWWW,
		   BanCod);
  }

/*****************************************************************************/
/******************************* Remove a banner *****************************/
/*****************************************************************************/

void Ban_DB_RemoveBanner (long BanCod)
  {
   DB_QueryDELETE ("can not remove a banner",
		   "DELETE FROM ban_banners"
		   " WHERE BanCod=%ld",
		   BanCod);
 }
