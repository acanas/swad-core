// swad_banner.c: banners

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

#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_banner.h"
#include "swad_box.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_language.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_table.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct Banner *Ban_EditingBan = NULL;	// Static variable to keep the banner being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ban_WriteListOfBanners (void);
static void Ban_PutIconsListingBanners (void);
static void Ban_PutIconToEditBanners (void);
static void Ban_EditBannersInternal (void);
static void Ban_GetListBanners (MYSQL_RES **mysql_res,unsigned long NumRows);

static void Ban_PutIconsEditingBanners (void);

static void Ban_ListBannersForEdition (void);
static void Ban_PutParamBanCodToEdit (void);
static void Ban_PutParamBanCod (long BanCod);
static void Ban_ShowOrHideBanner (bool Hide);

static void Ban_RenameBanner (Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Ban_CheckIfBannerNameExists (const char *FieldName,const char *Name,long BanCod);
static void Ban_UpdateBanNameDB (long BanCod,const char *FieldName,
				 const char *NewBanName);

static void Ban_PutFormToCreateBanner (void);
static void Ban_PutHeadBanners (void);
static void Ban_CreateBanner (struct Banner *Ban);

static void Ban_EditingBannerConstructor (void);
static void Ban_EditingBannerDestructor (void);

/*****************************************************************************/
/***************************** List all banners ******************************/
/*****************************************************************************/

void Ban_SeeBanners (void)
  {
   extern const char *Hlp_SYSTEM_Banners;
   extern const char *Txt_Banners;
   extern const char *Txt_No_banners;
   extern const char *Txt_New_banner;
   MYSQL_RES *mysql_res;
   unsigned long NumRows;

   /***** Get list of banners *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get banners",
			     "SELECT BanCod,Hidden,ShortName,FullName,Img,WWW"
			     " FROM banners"
			     " WHERE Hidden='N'"
			     " ORDER BY ShortName");
   Ban_GetListBanners (&mysql_res,NumRows);

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Banners,Ban_PutIconsListingBanners,
                 Hlp_SYSTEM_Banners,Box_NOT_CLOSABLE);

   /***** Write all banners *****/
   if (Gbl.Banners.Num)	// There are banners
      Ban_WriteListOfBanners ();
   else			// No banners created
      Ale_ShowAlert (Ale_INFO,Txt_No_banners);

   /***** Button to create banner *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
     {
      Frm_StartForm (ActEdiBan);
      Btn_PutConfirmButton (Txt_New_banner);
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_EndBox ();

   /***** Free list of banners *****/
   Ban_FreeListBanners ();
  }

/*****************************************************************************/
/*************************** Write list of banners ***************************/
/*****************************************************************************/

static void Ban_WriteListOfBanners (void)
  {
   unsigned NumBan;

   /***** List start *****/
   fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">");

   /***** Write all the banners *****/
   for (NumBan = 0;
	NumBan < Gbl.Banners.Num;
	NumBan++)
      /* Write data of this banner */
      fprintf (Gbl.F.Out,"<li>"
			 "<a href=\"%s\" title=\"%s\" class=\"DAT\" target=\"_blank\">"
                         "<img src=\"%s/%s\""
                         " alt=\"%s\" title=\"%s\""
                         " class=\"BANNER\" />"
                         "</a>"
			 "</li>",
               Gbl.Banners.Lst[NumBan].WWW,
               Gbl.Banners.Lst[NumBan].FullName,
               Cfg_URL_BANNER_PUBLIC,
               Gbl.Banners.Lst[NumBan].Img,
               Gbl.Banners.Lst[NumBan].ShrtName,
               Gbl.Banners.Lst[NumBan].FullName);

   /***** List end *****/
   fprintf (Gbl.F.Out,"</ul>");
  }

/*****************************************************************************/
/***************** Put contextual icons in list of banners *******************/
/*****************************************************************************/

static void Ban_PutIconsListingBanners (void)
  {
   /***** Put icon to view banners *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      Ban_PutIconToEditBanners ();

   /***** Put icon to view links *****/
   Lnk_PutIconToViewLinks ();
  }

/*****************************************************************************/
/************************* Put an icon to edit banners ***********************/
/*****************************************************************************/

static void Ban_PutIconToEditBanners (void)
  {
   Ico_PutContextualIconToEdit (ActEdiBan,NULL);
  }

/*****************************************************************************/
/************************** Put forms to edit banners ************************/
/*****************************************************************************/

void Ban_EditBanners (void)
  {
   /***** Banner constructor *****/
   Ban_EditingBannerConstructor ();

   /***** Edit banners *****/
   Ban_EditBannersInternal ();

   /***** Banner destructor *****/
   Ban_EditingBannerDestructor ();
  }

static void Ban_EditBannersInternal (void)
  {
   extern const char *Hlp_SYSTEM_Banners_edit;
   extern const char *Txt_Banners;
   MYSQL_RES *mysql_res;
   unsigned long NumRows;

   /***** Get list of banners *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get banners",
			     "SELECT BanCod,Hidden,ShortName,FullName,Img,WWW"
			     " FROM banners ORDER BY ShortName");
   Ban_GetListBanners (&mysql_res,NumRows);

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Banners,Ban_PutIconsEditingBanners,
                 Hlp_SYSTEM_Banners_edit,Box_NOT_CLOSABLE);

   /***** Put a form to create a new banner *****/
   Ban_PutFormToCreateBanner ();

   /***** Forms to edit current banners *****/
   if (Gbl.Banners.Num)
      Ban_ListBannersForEdition ();

   /***** End box *****/
   Box_EndBox ();

   /***** Free list of banners *****/
   Ban_FreeListBanners ();
  }

/*****************************************************************************/
/**************************** List all the banners ***************************/
/*****************************************************************************/

static void Ban_GetListBanners (MYSQL_RES **mysql_res,unsigned long NumRows)
  {
   MYSQL_ROW row;
   unsigned NumBan;
   struct Banner *Ban;

   /***** Get banners from database *****/
   if (NumRows) // Banners found...
     {
      Gbl.Banners.Num = (unsigned) NumRows;

      /***** Create list with banners *****/
      if ((Gbl.Banners.Lst = (struct Banner *)
			     calloc (NumRows,sizeof (struct Banner))) == NULL)
	 Lay_NotEnoughMemoryExit ();

      /***** Get the banners *****/
      for (NumBan = 0;
	   NumBan < Gbl.Banners.Num;
	   NumBan++)
	{
	 Ban = &(Gbl.Banners.Lst[NumBan]);

	 /* Get next banner */
	 row = mysql_fetch_row (*mysql_res);

	 /* Get banner code (row[0]) */
	 if ((Ban->BanCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of banner.");

	 /* Get if banner is hidden (row[1]) */
	 Ban->Hidden = (row[1][0] == 'Y');

	 /* Get the short name of the banner (row[2]) */
	 Str_Copy (Ban->ShrtName,row[2],
		   Ban_MAX_BYTES_SHRT_NAME);

	 /* Get the full name of the banner (row[3]) */
	 Str_Copy (Ban->FullName,row[3],
		   Ban_MAX_BYTES_FULL_NAME);

	 /* Get the image of the banner (row[4]) */
	 Str_Copy (Ban->Img,row[4],
		   Ban_MAX_BYTES_IMAGE);

	 /* Get the URL of the banner (row[5]) */
	 Str_Copy (Ban->WWW,row[5],
		   Cns_MAX_BYTES_WWW);
	}
     }
   else
      Gbl.Banners.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/*************************** Get banner full name ****************************/
/*****************************************************************************/

void Ban_GetDataOfBannerByCod (struct Banner *Ban)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Clear data *****/
   Ban->Hidden = false;
   Ban->ShrtName[0] = Ban->FullName[0] = Ban->Img[0] = Ban->WWW[0] = '\0';

   /***** Check if banner code is correct *****/
   if (Ban->BanCod > 0)
     {
      /***** Get data of a banner from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a banner",
				"SELECT Hidden,ShortName,FullName,Img,WWW"
			        " FROM banners WHERE BanCod=%ld",
			        Ban->BanCod);
      if (NumRows) // Banner found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get if the banner is hidden (row[0]) */
         Ban->Hidden = (row[0][0] == 'Y');

         /* Get the short name of the banner (row[1]) */
         Str_Copy (Ban->ShrtName,row[1],
                   Ban_MAX_BYTES_SHRT_NAME);

         /* Get the full name of the banner (row[2]) */
         Str_Copy (Ban->FullName,row[2],
                   Ban_MAX_BYTES_FULL_NAME);

         /* Get the image of the banner (row[3]) */
         Str_Copy (Ban->Img,row[3],
                   Ban_MAX_BYTES_IMAGE);

         /* Get the URL of the banner (row[4]) */
         Str_Copy (Ban->WWW,row[4],
                   Cns_MAX_BYTES_WWW);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/*************************** Free list of banners ****************************/
/*****************************************************************************/

void Ban_FreeListBanners (void)
  {
   if (Gbl.Banners.Lst)
     {
      /***** Free memory used by the list of banners *****/
      free ((void *) Gbl.Banners.Lst);
      Gbl.Banners.Lst = NULL;
      Gbl.Banners.Num = 0;
     }
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of banners *****************/
/*****************************************************************************/

static void Ban_PutIconsEditingBanners (void)
  {
   /***** Put icon to view banners *****/
   Ban_PutIconToViewBanners ();

   /***** Put icon to view links *****/
   Lnk_PutIconToViewLinks ();
  }

/*****************************************************************************/
/************************* Put icon to view banners **************************/
/*****************************************************************************/

void Ban_PutIconToViewBanners (void)
  {
   extern const char *Txt_Banners;

   Lay_PutContextualLinkOnlyIcon (ActSeeBan,NULL,NULL,
                                  "flag.svg",
                                  Txt_Banners);
  }

/*****************************************************************************/
/*************************** List all the banners ****************************/
/*****************************************************************************/

static void Ban_ListBannersForEdition (void)
  {
   unsigned NumBan;
   struct Banner *Ban;
   char *Anchor = NULL;

   /***** Start table *****/
   Tbl_TABLE_BeginWidePadding (2);

   /***** Table head *****/
   Ban_PutHeadBanners ();

   /***** Write all the banners *****/
   for (NumBan = 0;
	NumBan < Gbl.Banners.Num;
	NumBan++)
     {
      Ban = &Gbl.Banners.Lst[NumBan];
      Gbl.Banners.BanCodToEdit = Ban->BanCod;

      /* Set anchor string */
      Frm_SetAnchorStr (Ban->BanCod,&Anchor);

      /* Put icon to remove banner */
      Tbl_TR_Begin (NULL);

      Tbl_TD_Begin ("class=\"BM\"");
      Ico_PutContextualIconToRemove (ActRemBan,Ban_PutParamBanCodToEdit);
      Tbl_TD_End ();

      /* Put icon to hide/show banner */
      Tbl_TD_Begin ("class=\"BM\"");
      if (Ban->Hidden)
         Ico_PutContextualIconToUnhide (ActShoBan,Anchor,Ban_PutParamBanCodToEdit);
      else
         Ico_PutContextualIconToHide (ActHidBan,Anchor,Ban_PutParamBanCodToEdit);
      Tbl_TD_End ();

      /* Banner code */
      Tbl_TD_Begin ("class=\"%s RIGHT_MIDDLE\"",
		    Ban->Hidden ? "DAT_LIGHT" :
				  "DAT");
      Lay_StartArticle (Anchor);
      fprintf (Gbl.F.Out,"%ld",Ban->BanCod);
      Lay_EndArticle ();
      Tbl_TD_End ();

      /* Banner short name */
      Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
      Frm_StartForm (ActRenBanSho);
      Ban_PutParamBanCodToEdit ();
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
	                 " maxlength=\"%u\" value=\"%s\""
                         " class=\"INPUT_SHORT_NAME\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Ban_MAX_CHARS_SHRT_NAME,Ban->ShrtName,Gbl.Form.Id);
      Frm_EndForm ();
      Tbl_TD_End ();

      /* Banner full name */
      Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
      Frm_StartForm (ActRenBanFul);
      Ban_PutParamBanCodToEdit ();
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
	                 " maxlength=\"%u\" value=\"%s\""
                         " class=\"INPUT_FULL_NAME\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Ban_MAX_CHARS_FULL_NAME,Ban->FullName,Gbl.Form.Id);
      Frm_EndForm ();
      Tbl_TD_End ();

      /* Banner image */
      Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
      Frm_StartForm (ActChgBanImg);
      Ban_PutParamBanCodToEdit ();
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Img\""
	                 " size=\"12\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Ban_MAX_CHARS_IMAGE,Ban->Img,Gbl.Form.Id);
      Frm_EndForm ();
      Tbl_TD_End ();

      /* Banner WWW */
      Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
      Frm_StartForm (ActChgBanWWW);
      Ban_PutParamBanCodToEdit ();
      fprintf (Gbl.F.Out,"<input type=\"url\" name=\"WWW\""
	                 " maxlength=\"%u\" value=\"%s\""
                         " class=\"INPUT_WWW\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Cns_MAX_CHARS_WWW,Ban->WWW,Gbl.Form.Id);
      Frm_EndForm ();
      Tbl_TD_End ();

      Tbl_TR_End ();

      /* Free anchor string */
      Frm_FreeAnchorStr (Anchor);
     }

   /***** End table *****/
   Tbl_TABLE_End ();
  }

/*****************************************************************************/
/******************* Write parameter with code of banner *********************/
/*****************************************************************************/

static void Ban_PutParamBanCodToEdit (void)
  {
   Ban_PutParamBanCod (Gbl.Banners.BanCodToEdit);
  }

static void Ban_PutParamBanCod (long BanCod)
  {
   Par_PutHiddenParamLong ("BanCod",BanCod);
  }

/*****************************************************************************/
/******************** Get parameter with code of banner **********************/
/*****************************************************************************/

long Ban_GetParamBanCod (void)
  {
   /***** Get code of banner *****/
   return Par_GetParToLong ("BanCod");
  }

/*****************************************************************************/
/******************************* Remove a banner *****************************/
/*****************************************************************************/

void Ban_RemoveBanner (void)
  {
   extern const char *Txt_Banner_X_removed;

   /***** Banner constructor *****/
   Ban_EditingBannerConstructor ();

   /***** Get banner code *****/
   if ((Ban_EditingBan->BanCod = Ban_GetParamBanCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of banner is missing.");

   /***** Get data of the banner from database *****/
   Ban_GetDataOfBannerByCod (Ban_EditingBan);

   /***** Remove banner *****/
   DB_QueryDELETE ("can not remove a banner",
		   "DELETE FROM banners WHERE BanCod=%ld",
		   Ban_EditingBan->BanCod);

   /***** Write message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_Banner_X_removed,
                    Ban_EditingBan->ShrtName);
  }

/*****************************************************************************/
/**************************** Show a hidden banner ***************************/
/*****************************************************************************/

void Ban_ShowBanner (void)
  {
   /***** Banner constructor *****/
   Ban_EditingBannerConstructor ();

   /***** Set banner as visible *****/
   Ban_ShowOrHideBanner (false);
  }

/*****************************************************************************/
/**************************** Hide a visible banner **************************/
/*****************************************************************************/

void Ban_HideBanner (void)
  {
   /***** Banner constructor *****/
   Ban_EditingBannerConstructor ();

   /***** Set banner as hidden *****/
   Ban_ShowOrHideBanner (true);
  }

/*****************************************************************************/
/*************** Change hiddeness of banner in the database ******************/
/*****************************************************************************/

static void Ban_ShowOrHideBanner (bool Hide)
  {
   /***** Get banner code *****/
   if ((Ban_EditingBan->BanCod = Ban_GetParamBanCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of banner is missing.");

   /***** Get data of the banner from database *****/
   Ban_GetDataOfBannerByCod (Ban_EditingBan);

   /***** Mark file as hidden/visible in database *****/
   if (Ban_EditingBan->Hidden != Hide)
      DB_QueryUPDATE ("can not change status of a banner in database",
		      "UPDATE banners SET Hidden='%c'"
		      " WHERE BanCod=%ld",
	              Hide ? 'Y' :
		             'N',
	              Ban_EditingBan->BanCod);
  }

/*****************************************************************************/
/********************* Change the short name of a banner *********************/
/*****************************************************************************/

void Ban_RenameBannerShort (void)
  {
   /***** Banner constructor *****/
   Ban_EditingBannerConstructor ();

   /***** Rename banner *****/
   Ban_RenameBanner (Cns_SHRT_NAME);
  }

/*****************************************************************************/
/********************* Change the full name of a banner **********************/
/*****************************************************************************/

void Ban_RenameBannerFull (void)
  {
   /***** Banner constructor *****/
   Ban_EditingBannerConstructor ();

   /***** Rename banner *****/
   Ban_RenameBanner (Cns_FULL_NAME);
  }

/*****************************************************************************/
/*********************** Change the name of a banner *************************/
/*****************************************************************************/

static void Ban_RenameBanner (Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_banner_X_empty;
   extern const char *Txt_The_banner_X_already_exists;
   extern const char *Txt_The_banner_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_banner_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentBanName = NULL;		// Initialized to avoid warning
   char NewBanName[Ban_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Ban_MAX_BYTES_SHRT_NAME;
         CurrentBanName = Ban_EditingBan->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Ban_MAX_BYTES_FULL_NAME;
         CurrentBanName = Ban_EditingBan->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the banner */
   if ((Ban_EditingBan->BanCod = Ban_GetParamBanCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of banner is missing.");

   /* Get the new name for the banner */
   Par_GetParToText (ParamName,NewBanName,MaxBytes);

   /***** Get banner data from the database *****/
   Ban_GetDataOfBannerByCod (Ban_EditingBan);

   /***** Check if new name is empty *****/
   if (!NewBanName[0])
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_leave_the_name_of_the_banner_X_empty,
                       CurrentBanName);
   else
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentBanName,NewBanName))	// Different names
        {
         /***** If banner was in database... *****/
         if (Ban_CheckIfBannerNameExists (ParamName,NewBanName,Ban_EditingBan->BanCod))
	    Ale_CreateAlert (Ale_WARNING,NULL,
		             Txt_The_banner_X_already_exists,
                             NewBanName);
         else
           {
            /* Update the table changing old name by new name */
            Ban_UpdateBanNameDB (Ban_EditingBan->BanCod,FieldName,NewBanName);

            /* Write message to show the change made */
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
		             Txt_The_banner_X_has_been_renamed_as_Y,
                             CurrentBanName,NewBanName);
           }
        }
      else	// The same name
	 Ale_CreateAlert (Ale_INFO,NULL,
	                  Txt_The_name_of_the_banner_X_has_not_changed,
                          CurrentBanName);
     }

   /***** Update name *****/
   Str_Copy (CurrentBanName,NewBanName,
             MaxBytes);
  }

/*****************************************************************************/
/********************* Check if the name of banner exists ********************/
/*****************************************************************************/

static bool Ban_CheckIfBannerNameExists (const char *FieldName,const char *Name,long BanCod)
  {
   /***** Get number of banners with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a banner"
			  " already existed",
		          "SELECT COUNT(*) FROM banners"
			  " WHERE %s='%s' AND BanCod<>%ld",
			  FieldName,Name,BanCod) != 0);
  }

/*****************************************************************************/
/***************** Update banner name in table of banners ********************/
/*****************************************************************************/

static void Ban_UpdateBanNameDB (long BanCod,const char *FieldName,
				 const char *NewBanName)
  {
   /***** Update banner changing old name by new name *****/
   DB_QueryUPDATE ("can not update the name of a banner",
		   "UPDATE banners SET %s='%s' WHERE BanCod=%ld",
	           FieldName,NewBanName,BanCod);
  }

/*****************************************************************************/
/*********************** Change the image of a banner ************************/
/*****************************************************************************/

void Ban_ChangeBannerImg (void)
  {
   extern const char *Txt_The_new_image_is_X;
   extern const char *Txt_You_can_not_leave_the_image_empty;
   char NewImg[Ban_MAX_BYTES_IMAGE + 1];

   /***** Banner constructor *****/
   Ban_EditingBannerConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the banner */
   if ((Ban_EditingBan->BanCod = Ban_GetParamBanCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of banner is missing.");

   /* Get the new WWW for the banner */
   Par_GetParToText ("Img",NewImg,Ban_MAX_BYTES_IMAGE);

   /***** Get banner data from the database *****/
   Ban_GetDataOfBannerByCod (Ban_EditingBan);

   /***** Check if new image is empty *****/
   if (NewImg[0])
     {
      /* Update the table changing old image by new image */
      DB_QueryUPDATE ("can not update the image of a banner",
		      "UPDATE banners SET Img='%s' WHERE BanCod=%ld",
                      NewImg,Ban_EditingBan->BanCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_image_is_X,
                       NewImg);
     }
   else
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_leave_the_image_empty);

   /***** Update image *****/
   Str_Copy (Ban_EditingBan->Img,NewImg,
             Ban_MAX_BYTES_IMAGE);
  }

/*****************************************************************************/
/*********************** Change the WWW of a banner **************************/
/*****************************************************************************/

void Ban_ChangeBannerWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   extern const char *Txt_You_can_not_leave_the_web_address_empty;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Banner constructor *****/
   Ban_EditingBannerConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the banner */
   if ((Ban_EditingBan->BanCod = Ban_GetParamBanCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of banner is missing.");

   /* Get the new WWW for the banner */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get banner data from the database *****/
   Ban_GetDataOfBannerByCod (Ban_EditingBan);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /* Update the table changing old WWW by new WWW */
      DB_QueryUPDATE ("can not update the web of a banner",
		      "UPDATE banners SET WWW='%s' WHERE BanCod=%ld",
                      NewWWW,Ban_EditingBan->BanCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_web_address_is_X,
                       NewWWW);
     }
   else
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_leave_the_web_address_empty);

   /***** Update web *****/
   Str_Copy (Ban_EditingBan->WWW,NewWWW,
             Cns_MAX_BYTES_WWW);
  }

/*****************************************************************************/
/********* Show alerts after changing a banner and continue editing **********/
/*****************************************************************************/

void Ban_ContEditAfterChgBan (void)
  {
   /***** Write message to show the change made *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Ban_EditBannersInternal ();

   /***** Banner destructor *****/
   Ban_EditingBannerDestructor ();
  }

/*****************************************************************************/
/********************* Put a form to create a new banner *********************/
/*****************************************************************************/

static void Ban_PutFormToCreateBanner (void)
  {
   extern const char *Hlp_SYSTEM_Banners_edit;
   extern const char *Txt_New_banner;
   extern const char *Txt_Create_banner;

   /***** Start form *****/
   Frm_StartForm (ActNewBan);

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_New_banner,NULL,
                      Hlp_SYSTEM_Banners_edit,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Ban_PutHeadBanners ();

   /***** Banner code *****/
   Tbl_TR_Begin (NULL);

   Tbl_TD_Begin ("class=\"BM\"");
   Tbl_TD_End ();

   Tbl_TD_Begin ("class=\"BM\"");
   Tbl_TD_End ();

   Tbl_TD_Empty (1);

   /***** Banner short name *****/
   Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_SHORT_NAME\""
                      " required=\"required\" />",
            Ban_MAX_CHARS_SHRT_NAME,Ban_EditingBan->ShrtName);
   Tbl_TD_End ();

   /***** Banner full name *****/
   Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_FULL_NAME\""
                      " required=\"required\" />",
            Ban_MAX_CHARS_FULL_NAME,Ban_EditingBan->FullName);
   Tbl_TD_End ();

   /***** Banner image *****/
   Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Img\""
                      " size=\"12\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />",
            Ban_MAX_CHARS_IMAGE,Ban_EditingBan->Img);
   Tbl_TD_End ();

   /***** Banner WWW *****/
   Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
   fprintf (Gbl.F.Out,"<input type=\"url\" name=\"WWW\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_WWW\""
                      " required=\"required\" />",
            Cns_MAX_CHARS_WWW,Ban_EditingBan->WWW);
   Tbl_TD_End ();

   Tbl_TR_End ();

   /***** End table, send button and end box *****/
   Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_banner);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Write header with fields of a banner *******************/
/*****************************************************************************/

static void Ban_PutHeadBanners (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Short_name;
   extern const char *Txt_Full_name;
   extern const char *Txt_Image;
   extern const char *Txt_WWW;

   Tbl_TR_Begin (NULL);
   fprintf (Gbl.F.Out,"<th class=\"BM\"></th>"
                      "<th class=\"BM\"></th>"
                      "<th class=\"RIGHT_MIDDLE\">%s</th>"
                      "<th class=\"LEFT_MIDDLE\">%s</th>"
                      "<th class=\"LEFT_MIDDLE\">%s</th>"
                      "<th class=\"LEFT_MIDDLE\">%s</th>"
                      "<th class=\"LEFT_MIDDLE\">%s</th>",
            Txt_Code,
            Txt_Short_name,
            Txt_Full_name,
            Txt_Image,
            Txt_WWW);
   Tbl_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new banner *********************/
/*****************************************************************************/

void Ban_RecFormNewBanner (void)
  {
   extern const char *Txt_The_banner_X_already_exists;
   extern const char *Txt_You_must_specify_the_image_of_the_new_banner;
   extern const char *Txt_You_must_specify_the_URL_of_the_new_banner;
   extern const char *Txt_Created_new_banner_X;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_banner;

   /***** Banner constructor *****/
   Ban_EditingBannerConstructor ();

   /***** Get parameters from form *****/
   /* Get banner short name */
   Par_GetParToText ("ShortName",Ban_EditingBan->ShrtName,Ban_MAX_BYTES_SHRT_NAME);

   /* Get banner full name */
   Par_GetParToText ("FullName",Ban_EditingBan->FullName,Ban_MAX_BYTES_FULL_NAME);

   /* Get banner image */
   Par_GetParToText ("Img",Ban_EditingBan->Img,Ban_MAX_BYTES_IMAGE);

   /* Get banner URL */
   Par_GetParToText ("WWW",Ban_EditingBan->WWW,Cns_MAX_BYTES_WWW);

   if (Ban_EditingBan->ShrtName[0] &&
       Ban_EditingBan->FullName[0])	// If there's a banner name
     {
      /***** If name of banner was in database... *****/
      if (Ban_CheckIfBannerNameExists ("ShortName",Ban_EditingBan->ShrtName,-1L))
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_The_banner_X_already_exists,
                          Ban_EditingBan->ShrtName);
      else if (Ban_CheckIfBannerNameExists ("FullName",Ban_EditingBan->FullName,-1L))
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_The_banner_X_already_exists,
                          Ban_EditingBan->FullName);
      else if (!Ban_EditingBan->Img[0])
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_You_must_specify_the_image_of_the_new_banner);
      else if (!Ban_EditingBan->WWW[0])
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_You_must_specify_the_URL_of_the_new_banner);
      else	// Add new banner to database
        {
         Ban_CreateBanner (Ban_EditingBan);
	 Ale_CreateAlert (Ale_SUCCESS,Txt_Created_new_banner_X,
			  Ban_EditingBan->ShrtName);
        }
     }
   else	// If there is not a banner name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_banner);
  }

/*****************************************************************************/
/**************************** Create a new banner ****************************/
/*****************************************************************************/

static void Ban_CreateBanner (struct Banner *Ban)
  {
   /***** Create a new banner *****/
   DB_QueryINSERT ("can not create banner",
		   "INSERT INTO banners"
		   " (Hidden,ShortName,FullName,Img,WWW)"
		   " VALUES"
		   " ('N','%s','%s','%s','%s')",
                   Ban->ShrtName,Ban->FullName,Ban->Img,Ban->WWW);
  }

/*****************************************************************************/
/************************* Write menu with some banners **********************/
/*****************************************************************************/

void Ban_WriteMenuWithBanners (void)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumRows;
   unsigned NumBan;

   /***** Get random banner *****/
   // The banner(s) will change once in a while
   NumRows = DB_QuerySELECT (&mysql_res,"can not get banners",
			     "SELECT BanCod,Hidden,ShortName,FullName,Img,WWW"
			     " FROM banners"
			     " WHERE Hidden='N'"
			     " ORDER BY RAND(%lu) LIMIT %u",
			     (unsigned long) (Gbl.StartExecutionTimeUTC / Cfg_TIME_TO_CHANGE_BANNER),
			     Cfg_NUMBER_OF_BANNERS);
   Ban_GetListBanners (&mysql_res,NumRows);

   /***** Write all the banners *****/
   for (NumBan = 0;
	NumBan < Gbl.Banners.Num;
	NumBan++)
     {
      /* Write data of this banner */
      fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
      Frm_StartForm (ActClkBan);
      Ban_PutParamBanCod (Gbl.Banners.Lst[NumBan].BanCod);
      Par_PutHiddenParamString ("URL",Gbl.Banners.Lst[NumBan].WWW);
      Frm_LinkFormSubmit (Gbl.Banners.Lst[NumBan].FullName,"BANNER",NULL);
      fprintf (Gbl.F.Out,"<img src=\"%s/%s\""
                         " alt=\"%s\" title=\"%s\""
                         " class=\"BANNER\" />"
                         "</a>",
               Cfg_URL_BANNER_PUBLIC,
               Gbl.Banners.Lst[NumBan].Img,
               Gbl.Banners.Lst[NumBan].ShrtName,
               Gbl.Banners.Lst[NumBan].FullName);
      Frm_EndForm ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Free list of banners *****/
   Ban_FreeListBanners ();
  }

/*****************************************************************************/
/************************* Go to a banner when clicked ***********************/
/*****************************************************************************/

void Ban_ClickOnBanner (void)
  {
   struct Banner Ban;

   /***** Get banner code *****/
   if ((Ban.BanCod = Ban_GetParamBanCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of banner is missing.");

   /***** Get data of the banner from database *****/
   Ban_GetDataOfBannerByCod (&Ban);

   /***** Set banner clicked in order to log it *****/
   Gbl.Banners.BanCodClicked = Ban.BanCod;

   /***** Download the file *****/
   fprintf (stdout,"Location: %s\n\n",Ban.WWW);
   // TODO: Put headers Content-type and Content-disposition:
   // See: http://stackoverflow.com/questions/381954/how-do-i-fix-firefox-trying-to-save-image-as-htm
   // http://elouai.com/force-download.php
   Gbl.Layout.HTMLStartWritten =
   Gbl.Layout.DivsEndWritten   =
   Gbl.Layout.HTMLEndWritten   = true;	// Don't write HTML at all
  }

/*****************************************************************************/
/************************* Banner constructor/destructor **********************/
/*****************************************************************************/

static void Ban_EditingBannerConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Ban_EditingBan != NULL)
      Lay_ShowErrorAndExit ("Error initializing banner.");

   /***** Allocate memory for banner *****/
   if ((Ban_EditingBan = (struct Banner *) malloc (sizeof (struct Banner))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for banner.");

   /***** Reset banner *****/
   Ban_EditingBan->BanCod      = -1L;
   Ban_EditingBan->Hidden      = true;
   Ban_EditingBan->ShrtName[0] = '\0';
   Ban_EditingBan->FullName[0] = '\0';
   Ban_EditingBan->Img[0]      = '\0';
   Ban_EditingBan->WWW[0]      = '\0';
  }

static void Ban_EditingBannerDestructor (void)
  {
   /***** Free memory used for place *****/
   if (Ban_EditingBan != NULL)
     {
      free ((void *) Ban_EditingBan);
      Ban_EditingBan = NULL;
     }
  }
