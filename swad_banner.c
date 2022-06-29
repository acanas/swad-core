// swad_banner.c: banners

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include <stddef.h>		// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_banner.h"
#include "swad_banner_database.h"
#include "swad_box.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_link.h"
#include "swad_parameter.h"
#include "swad_photo.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct Ban_Banners
  {
   unsigned Num;	// Number of banners
   struct Ban_Banner *Lst;	// List of banners
   long BanCodToEdit;	// Used as parameter in contextual links
   long BanCodClicked;	// Banned clicked, used to log it
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct Ban_Banner *Ban_EditingBan;
static long Ban_BanCodClicked;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ban_SetEditingBanner (struct Ban_Banner *Ban);
static struct Ban_Banner *Ban_GetEditingBanner (void);

static void Ban_WriteListOfBanners (const struct Ban_Banners *Banners);
static void Ban_PutIconsListingBanners (__attribute__((unused)) void *Args);
static void Ban_PutIconToEditBanners (void);
static void Ban_EditBannersInternal (struct Ban_Banners *Banners,
                                     const struct Ban_Banner *Ban);
static void Ban_GetListBanners (struct Ban_Banners *Banners,
                                MYSQL_RES **mysql_res);
static void Ban_FreeListBanners (struct Ban_Banners *Banners);

static void Ban_PutIconsEditingBanners (__attribute__((unused)) void *Args);

static void Ban_ListBannersForEdition (struct Ban_Banners *Banners);
static void Ban_PutParamBanCodToEdit (void *BanCod);
static void Ban_PutParamBanCod (long BanCod);
static void Ban_ShowOrHideBanner (struct Ban_Banner *Ban,bool Hide);

static void Ban_RenameBanner (struct Ban_Banner *Ban,
                              Cns_ShrtOrFullName_t ShrtOrFullName);

static void Ban_PutFormToCreateBanner (const struct Ban_Banner *Ban);
static void Ban_PutHeadBanners (void);

static void Ban_SetBanCodClicked (long BanCod);

static void Ban_ResetBanners (struct Ban_Banners *Banners);
static void Ban_ResetBanner (struct Ban_Banner *Ban);

/*****************************************************************************/
/************************** Access to editing banner *************************/
/*****************************************************************************/

static void Ban_SetEditingBanner (struct Ban_Banner *Ban)
  {
   Ban_EditingBan = Ban;
  }

static struct Ban_Banner *Ban_GetEditingBanner (void)
  {
   return Ban_EditingBan;
  }

/*****************************************************************************/
/***************************** List all banners ******************************/
/*****************************************************************************/

void Ban_ShowAllBanners (void)
  {
   extern const char *Hlp_SYSTEM_Banners;
   extern const char *Txt_Banners;
   extern const char *Txt_No_banners;
   extern const char *Txt_New_banner;
   struct Ban_Banners Banners;
   MYSQL_RES *mysql_res;

   /***** Reset banners *****/
   Ban_ResetBanners (&Banners);

   /***** Get list of banners *****/
   Banners.Num = Ban_DB_GetVisibleBanners (&mysql_res);
   Ban_GetListBanners (&Banners,&mysql_res);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Banners,
                 Ban_PutIconsListingBanners,NULL,
                 Hlp_SYSTEM_Banners,Box_NOT_CLOSABLE);

      /***** Write all banners *****/
      if (Banners.Num)	// There are banners
	 Ban_WriteListOfBanners (&Banners);
      else		// No banners created
	 Ale_ShowAlert (Ale_INFO,Txt_No_banners);

      /***** Button to create banner *****/
      if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	{
	 Frm_BeginForm (ActEdiBan);
	    Btn_PutConfirmButton (Txt_New_banner);
	 Frm_EndForm ();
	}

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of banners *****/
   Ban_FreeListBanners (&Banners);
  }

/*****************************************************************************/
/*************************** Write list of banners ***************************/
/*****************************************************************************/

static void Ban_WriteListOfBanners (const struct Ban_Banners *Banners)
  {
   unsigned NumBan;

   /***** List start *****/
   HTM_UL_Begin ("class=\"LIST_LEFT\"");

      /***** Write all banners *****/
      for (NumBan = 0;
	   NumBan < Banners->Num;
	   NumBan++)
	{
	 /* Write data of this banner */
	 HTM_LI_Begin (NULL);
	    HTM_A_Begin ("href=\"%s\" title=\"%s\" class=\"DAT_%s\" target=\"_blank\"",
			 Banners->Lst[NumBan].WWW,
			 Banners->Lst[NumBan].FullName,
			 The_GetSuffix ());
	       HTM_IMG (Cfg_URL_BANNER_PUBLIC,Banners->Lst[NumBan].Img,Banners->Lst[NumBan].FullName,
			"class=\"BANNER\"");
	    HTM_A_End ();
	 HTM_LI_End ();
	}

   /***** List end *****/
   HTM_UL_End ();
  }

/*****************************************************************************/
/***************** Put contextual icons in list of banners *******************/
/*****************************************************************************/

static void Ban_PutIconsListingBanners (__attribute__((unused)) void *Args)
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
   Ico_PutContextualIconToEdit (ActEdiBan,NULL,
                                NULL,NULL);
  }

/*****************************************************************************/
/************************** Put forms to edit banners ************************/
/*****************************************************************************/

void Ban_EditBanners (void)
  {
   struct Ban_Banners Banners;
   struct Ban_Banner Ban;

   /***** Reset banners *****/
   Ban_ResetBanners (&Banners);

   /***** Reset banner *****/
   Ban_ResetBanner (&Ban);

   /***** Edit banners *****/
   Ban_EditBannersInternal (&Banners,&Ban);
  }

static void Ban_EditBannersInternal (struct Ban_Banners *Banners,
                                     const struct Ban_Banner *Ban)
  {
   extern const char *Hlp_SYSTEM_Banners_edit;
   extern const char *Txt_Banners;
   MYSQL_RES *mysql_res;

   /***** Get list of banners *****/
   Banners->Num = Ban_DB_GetAllBanners (&mysql_res);
   Ban_GetListBanners (Banners,&mysql_res);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Banners,
                 Ban_PutIconsEditingBanners,NULL,
                 Hlp_SYSTEM_Banners_edit,Box_NOT_CLOSABLE);

      /***** Put a form to create a new banner *****/
      Ban_PutFormToCreateBanner (Ban);

      /***** Forms to edit current banners *****/
      if (Banners->Num)
	 Ban_ListBannersForEdition (Banners);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of banners *****/
   Ban_FreeListBanners (Banners);
  }

/*****************************************************************************/
/****************************** List all banners *****************************/
/*****************************************************************************/

static void Ban_GetListBanners (struct Ban_Banners *Banners,
                                MYSQL_RES **mysql_res)
  {
   MYSQL_ROW row;
   unsigned NumBan;
   struct Ban_Banner *Ban;

   /***** Get banners from database *****/
   if (Banners->Num) // Banners found...
     {
      /***** Create list with banners *****/
      if ((Banners->Lst = calloc ((size_t) Banners->Num,sizeof (*Banners->Lst))) == NULL)
	 Err_NotEnoughMemoryExit ();

      /***** Get the banners *****/
      for (NumBan = 0;
	   NumBan < Banners->Num;
	   NumBan++)
	{
	 Ban = &(Banners->Lst[NumBan]);

	 /* Get next banner */
	 row = mysql_fetch_row (*mysql_res);

	 /* Get banner code (row[0]) */
	 if ((Ban->BanCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
	    Err_WrongBannerExit ();

	 /* Get if banner is hidden (row[1]) */
	 Ban->Hidden = (row[1][0] == 'Y');

	 /* Get short name (row[2]), full name (row[3]),
	    image (row[4]) and URL (row[5]) of the banner */
	 Str_Copy (Ban->ShrtName,row[2],sizeof (Ban->ShrtName) - 1);
	 Str_Copy (Ban->FullName,row[3],sizeof (Ban->FullName) - 1);
	 Str_Copy (Ban->Img     ,row[4],sizeof (Ban->Img     ) - 1);
	 Str_Copy (Ban->WWW     ,row[5],sizeof (Ban->WWW     ) - 1);
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/*************************** Get banner full name ****************************/
/*****************************************************************************/

void Ban_GetDataOfBannerByCod (struct Ban_Banner *Ban)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Clear data *****/
   Ban->Hidden = false;
   Ban->ShrtName[0] = Ban->FullName[0] = Ban->Img[0] = Ban->WWW[0] = '\0';

   /***** Check if banner code is correct *****/
   if (Ban->BanCod > 0)
     {
      /***** Get data of a banner from database *****/
      if (Ban_DB_GetDataOfBannerByCod (&mysql_res,Ban->BanCod)) // Banner found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get if the banner is hidden (row[0]) */
         Ban->Hidden = (row[0][0] == 'Y');

	 /* Get short name (row[1]), full name (row[2]),
	    image (row[3]) and URL (row[4]) of the banner */
         Str_Copy (Ban->ShrtName,row[1],sizeof (Ban->ShrtName) - 1);
         Str_Copy (Ban->FullName,row[2],sizeof (Ban->FullName) - 1);
         Str_Copy (Ban->Img     ,row[3],sizeof (Ban->Img     ) - 1);
         Str_Copy (Ban->WWW     ,row[4],sizeof (Ban->WWW     ) - 1);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/*************************** Free list of banners ****************************/
/*****************************************************************************/

static void Ban_FreeListBanners (struct Ban_Banners *Banners)
  {
   if (Banners->Lst)
     {
      /***** Free memory used by the list of banners *****/
      free (Banners->Lst);
      Banners->Lst = NULL;
      Banners->Num = 0;
     }
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of banners *****************/
/*****************************************************************************/

static void Ban_PutIconsEditingBanners (__attribute__((unused)) void *Args)
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
   Lay_PutContextualLinkOnlyIcon (ActSeeBan,NULL,
                                  NULL,NULL,
                                  "flag.svg",Ico_BLACK,
                                  Act_GetActionText (ActSeeBan));
  }

/*****************************************************************************/
/***************************** List all banners ******************************/
/*****************************************************************************/

static void Ban_ListBannersForEdition (struct Ban_Banners *Banners)
  {
   unsigned NumBan;
   struct Ban_Banner *Ban;
   char *Anchor = NULL;

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

      /***** Table head *****/
      Ban_PutHeadBanners ();

      /***** Write all banners *****/
      for (NumBan = 0;
	   NumBan < Banners->Num;
	   NumBan++)
	{
	 Ban = &Banners->Lst[NumBan];
	 Banners->BanCodToEdit = Ban->BanCod;

	 /* Set anchor string */
	 Frm_SetAnchorStr (Ban->BanCod,&Anchor);

	 /* Begin table row */
	 HTM_TR_Begin (NULL);

	    /* Put icon to remove banner */
	    HTM_TD_Begin ("class=\"BM\"");
	       Ico_PutContextualIconToRemove (ActRemBan,NULL,
					      Ban_PutParamBanCodToEdit,
					      &Banners->BanCodToEdit);
	    HTM_TD_End ();

	    /* Put icon to hide/show banner */
	    HTM_TD_Begin ("class=\"BM\"");
	       if (Ban->Hidden)
		  Ico_PutContextualIconToUnhide (ActShoBan,Anchor,
						 Ban_PutParamBanCodToEdit,
						 &Banners->BanCodToEdit);
	       else
		  Ico_PutContextualIconToHide (ActHidBan,Anchor,
					       Ban_PutParamBanCodToEdit,
					       &Banners->BanCodToEdit);
	    HTM_TD_End ();

	    /* Banner code */
	    HTM_TD_Begin ("class=\"RM %s_%s\"",
			  Ban->Hidden ? "DAT_LIGHT" :
					"DAT",
			  The_GetSuffix ());
	       HTM_ARTICLE_Begin (Anchor);
		  HTM_Long (Ban->BanCod);
	       HTM_ARTICLE_End ();
	    HTM_TD_End ();

	    /* Banner short name */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginForm (ActRenBanSho);
		  Ban_PutParamBanCodToEdit (&Banners->BanCodToEdit);
		  HTM_INPUT_TEXT ("ShortName",Ban_MAX_CHARS_SHRT_NAME,Ban->ShrtName,
				  HTM_SUBMIT_ON_CHANGE,
				  "class=\"INPUT_SHORT_NAME INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Banner full name */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginForm (ActRenBanFul);
		  Ban_PutParamBanCodToEdit (&Banners->BanCodToEdit);
		  HTM_INPUT_TEXT ("FullName",Ban_MAX_CHARS_FULL_NAME,Ban->FullName,
				  HTM_SUBMIT_ON_CHANGE,
				  "class=\"INPUT_FULL_NAME INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Banner image */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginForm (ActChgBanImg);
		  Ban_PutParamBanCodToEdit (&Banners->BanCodToEdit);
		  HTM_INPUT_TEXT ("Img",Ban_MAX_CHARS_IMAGE,Ban->Img,
				  HTM_SUBMIT_ON_CHANGE,
				  "size=\"12\" class=\"INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Banner WWW */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginForm (ActChgBanWWW);
		  Ban_PutParamBanCodToEdit (&Banners->BanCodToEdit);
		  HTM_INPUT_URL ("WWW",Ban->WWW,HTM_SUBMIT_ON_CHANGE,
				 "class=\"INPUT_WWW_NARROW INPUT_%s\""
				 " required=\"required\"",
				 The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /* Free anchor string */
	 Frm_FreeAnchorStr (Anchor);
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************* Write parameter with code of banner *********************/
/*****************************************************************************/

static void Ban_PutParamBanCodToEdit (void *BanCod)
  {
   if (BanCod)
      Ban_PutParamBanCod (*((long *) BanCod));
  }

static void Ban_PutParamBanCod (long BanCod)
  {
   Par_PutHiddenParamLong (NULL,"BanCod",BanCod);
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
   struct Ban_Banner Ban;

   /***** Reset banner *****/
   Ban_ResetBanner (&Ban);

   /***** Get banner code *****/
   if ((Ban.BanCod = Ban_GetParamBanCod ()) <= 0)
      Err_WrongBannerExit ();

   /***** Get data of the banner from database *****/
   Ban_GetDataOfBannerByCod (&Ban);

   /***** Remove banner *****/
   Ban_DB_RemoveBanner (Ban.BanCod);

   /***** Write message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_Banner_X_removed,
                    Ban.ShrtName);

   /***** Set editing banner to use ot in a posterior function *****/
   Ban_SetEditingBanner (&Ban);
  }

/*****************************************************************************/
/**************************** Show a hidden banner ***************************/
/*****************************************************************************/

void Ban_ShowBanner (void)
  {
   struct Ban_Banner Ban;

   /***** Reset banner *****/
   Ban_ResetBanner (&Ban);

   /***** Set banner as visible *****/
   Ban_ShowOrHideBanner (&Ban,false);

   /***** Set editing banner to use ot in a posterior function *****/
   Ban_SetEditingBanner (&Ban);
  }

/*****************************************************************************/
/**************************** Hide a visible banner **************************/
/*****************************************************************************/

void Ban_HideBanner (void)
  {
   struct Ban_Banner Ban;

   /***** Reset banner *****/
   Ban_ResetBanner (&Ban);

   /***** Set banner as hidden *****/
   Ban_ShowOrHideBanner (&Ban,true);

   /***** Set editing banner to use ot in a posterior function *****/
   Ban_SetEditingBanner (&Ban);
  }

/*****************************************************************************/
/*************** Change hiddeness of banner in the database ******************/
/*****************************************************************************/

static void Ban_ShowOrHideBanner (struct Ban_Banner *Ban,bool Hide)
  {
   /***** Get banner code *****/
   if ((Ban->BanCod = Ban_GetParamBanCod ()) <= 0)
      Err_WrongBannerExit ();

   /***** Get data of the banner from database *****/
   Ban_GetDataOfBannerByCod (Ban);

   /***** Mark file as hidden/visible in database *****/
   if (Ban->Hidden != Hide)
      Ban_DB_HideOrUnhideBanner (Ban->BanCod,Hide);
  }

/*****************************************************************************/
/********************* Change the short name of a banner *********************/
/*****************************************************************************/

void Ban_RenameBannerShort (void)
  {
   struct Ban_Banner Ban;

   /***** Reset banner *****/
   Ban_ResetBanner (&Ban);

   /***** Rename banner *****/
   Ban_RenameBanner (&Ban,Cns_SHRT_NAME);

   /***** Set editing banner to use ot in a posterior function *****/
   Ban_SetEditingBanner (&Ban);
  }

/*****************************************************************************/
/********************* Change the full name of a banner **********************/
/*****************************************************************************/

void Ban_RenameBannerFull (void)
  {
   struct Ban_Banner Ban;

   /***** Reset banner *****/
   Ban_ResetBanner (&Ban);

   /***** Rename banner *****/
   Ban_RenameBanner (&Ban,Cns_FULL_NAME);

   /***** Set editing banner to use ot in a posterior function *****/
   Ban_SetEditingBanner (&Ban);
  }

/*****************************************************************************/
/*********************** Change the name of a banner *************************/
/*****************************************************************************/

static void Ban_RenameBanner (struct Ban_Banner *Ban,
                              Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_banner_X_already_exists;
   extern const char *Txt_The_banner_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
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
         CurrentBanName = Ban->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Ban_MAX_BYTES_FULL_NAME;
         CurrentBanName = Ban->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the banner */
   if ((Ban->BanCod = Ban_GetParamBanCod ()) <= 0)
      Err_WrongBannerExit ();

   /* Get the new name for the banner */
   Par_GetParToText (ParamName,NewBanName,MaxBytes);

   /***** Get banner data from the database *****/
   Ban_GetDataOfBannerByCod (Ban);

   /***** Check if new name is empty *****/
   if (!NewBanName[0])
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
   else
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentBanName,NewBanName))	// Different names
        {
         /***** If banner was in database... *****/
         if (Ban_DB_CheckIfBannerNameExists (ParamName,NewBanName,Ban->BanCod))
	    Ale_CreateAlert (Ale_WARNING,NULL,
		             Txt_The_banner_X_already_exists,
                             NewBanName);
         else
           {
            /* Update the table changing old name by new name */
            Ban_DB_UpdateBanName (Ban->BanCod,FieldName,NewBanName);

            /* Write message to show the change made */
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
		             Txt_The_banner_X_has_been_renamed_as_Y,
                             CurrentBanName,NewBanName);
           }
        }
      else	// The same name
         /* Write warning message */
	 Ale_CreateAlert (Ale_INFO,NULL,
	                  Txt_The_name_X_has_not_changed,CurrentBanName);
     }

   /***** Update name *****/
   Str_Copy (CurrentBanName,NewBanName,MaxBytes);
  }

/*****************************************************************************/
/*********************** Change the image of a banner ************************/
/*****************************************************************************/

void Ban_ChangeBannerImg (void)
  {
   extern const char *Txt_The_new_image_is_X;
   struct Ban_Banner Ban;
   char NewImg[Ban_MAX_BYTES_IMAGE + 1];

   /***** Reset banner *****/
   Ban_ResetBanner (&Ban);

   /***** Get parameters from form *****/
   /* Get the code of the banner */
   if ((Ban.BanCod = Ban_GetParamBanCod ()) <= 0)
      Err_WrongBannerExit ();

   /* Get the new WWW for the banner */
   Par_GetParToText ("Img",NewImg,Ban_MAX_BYTES_IMAGE);

   /***** Get banner data from the database *****/
   Ban_GetDataOfBannerByCod (&Ban);

   /***** Check if new image is empty *****/
   if (NewImg[0])
     {
      /* Update the table changing old image by new image */
      Ban_DB_UpdateBanImg (Ban.BanCod,NewImg);

      /* Write message to show the change made */
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_image_is_X,
                       NewImg);
     }
   else
      /* Write warning message */
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update image *****/
   Str_Copy (Ban.Img,NewImg,sizeof (Ban.Img) - 1);

   /***** Set editing banner to use ot in a posterior function *****/
   Ban_SetEditingBanner (&Ban);
  }

/*****************************************************************************/
/*********************** Change the WWW of a banner **************************/
/*****************************************************************************/

void Ban_ChangeBannerWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   struct Ban_Banner Ban;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Reset banner *****/
   Ban_ResetBanner (&Ban);

   /***** Get parameters from form *****/
   /* Get the code of the banner */
   if ((Ban.BanCod = Ban_GetParamBanCod ()) <= 0)
      Err_WrongBannerExit ();

   /* Get the new WWW for the banner */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get banner data from the database *****/
   Ban_GetDataOfBannerByCod (&Ban);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /* Update the table changing old WWW by new WWW */
      Ban_DB_UpdateBanWWW (Ban.BanCod,NewWWW);

      /* Write message to show the change made */
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_web_address_is_X,
                       NewWWW);
     }
   else
      /* Write warning message */
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update web *****/
   Str_Copy (Ban.WWW,NewWWW,sizeof (Ban.WWW) - 1);

   /***** Set editing banner to use ot in a posterior function *****/
   Ban_SetEditingBanner (&Ban);
  }

/*****************************************************************************/
/********* Show alerts after changing a banner and continue editing **********/
/*****************************************************************************/

void Ban_ContEditAfterChgBan (void)
  {
   struct Ban_Banners Banners;
   struct Ban_Banner *Ban = Ban_GetEditingBanner ();

   /***** Reset banners *****/
   Ban_ResetBanners (&Banners);

   /***** Write message to show the change made *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Ban_EditBannersInternal (&Banners,Ban);
  }

/*****************************************************************************/
/********************* Put a form to create a new banner *********************/
/*****************************************************************************/

static void Ban_PutFormToCreateBanner (const struct Ban_Banner *Ban)
  {
   extern const char *Hlp_SYSTEM_Banners_edit;
   extern const char *Txt_New_banner;
   extern const char *Txt_Create_banner;

   /***** Begin form *****/
   Frm_BeginForm (ActNewBan);

      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_New_banner,
			 NULL,NULL,
			 Hlp_SYSTEM_Banners_edit,Box_NOT_CLOSABLE,2);

	 /***** Write heading *****/
	 Ban_PutHeadBanners ();

	 /***** Table row for input fields *****/
	 /* Begin table row */
	 HTM_TR_Begin (NULL);

	    /* Banner code */
	    HTM_TD_Begin ("class=\"BM\"");
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"BM\"");
	    HTM_TD_End ();

	    HTM_TD_Empty (1);

	    /* Banner short name */
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_INPUT_TEXT ("ShortName",Ban_MAX_CHARS_SHRT_NAME,Ban->ShrtName,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "class=\"INPUT_SHORT_NAME INPUT_%s\""
			       " required=\"required\"",
			       The_GetSuffix ());
	    HTM_TD_End ();

	    /* Banner full name */
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_INPUT_TEXT ("FullName",Ban_MAX_CHARS_FULL_NAME,Ban->FullName,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "class=\"INPUT_FULL_NAME INPUT_%s\""
			       " required=\"required\"",
			       The_GetSuffix ());
	    HTM_TD_End ();

	    /* Banner image */
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_INPUT_TEXT ("Img",Ban_MAX_CHARS_IMAGE,Ban->Img,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "size=\"12\" class=\"INPUT_%s\""
			       " required=\"required\"",
			       The_GetSuffix ());
	    HTM_TD_End ();

	    /* Banner WWW */
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_INPUT_URL ("WWW",Ban->WWW,HTM_DONT_SUBMIT_ON_CHANGE,
			      "class=\"INPUT_WWW_NARROW INPUT_%s\""
			      " required=\"required\"",
			      The_GetSuffix ());
	    HTM_TD_End ();

	 /* End table row */
	 HTM_TR_End ();

      /***** End table, send button and end box *****/
      Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_banner);

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

   /***** Begin table row *****/
   HTM_TR_Begin (NULL);

      /****** Head cells *****/
      HTM_TH_Span (NULL     ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH_Span (NULL     ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH (Txt_Code      ,HTM_HEAD_RIGHT );
      HTM_TH (Txt_Short_name,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Full_name ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Image     ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_WWW       ,HTM_HEAD_LEFT  );

   /***** End table row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new banner *********************/
/*****************************************************************************/

void Ban_ReceiveFormNewBanner (void)
  {
   extern const char *Txt_The_banner_X_already_exists;
   extern const char *Txt_You_must_specify_the_image_of_the_new_banner;
   extern const char *Txt_You_must_specify_the_web_address;
   extern const char *Txt_Created_new_banner_X;
   struct Ban_Banner Ban;

   /***** Reset banner *****/
   Ban_ResetBanner (&Ban);

   /***** Get parameters from form *****/
   /* Get banner short name */
   Par_GetParToText ("ShortName",Ban.ShrtName,Ban_MAX_BYTES_SHRT_NAME);

   /* Get banner full name */
   Par_GetParToText ("FullName",Ban.FullName,Ban_MAX_BYTES_FULL_NAME);

   /* Get banner image */
   Par_GetParToText ("Img",Ban.Img,Ban_MAX_BYTES_IMAGE);

   /* Get banner URL */
   Par_GetParToText ("WWW",Ban.WWW,Cns_MAX_BYTES_WWW);

   if (Ban.ShrtName[0] &&
       Ban.FullName[0])	// If there's a banner name
     {
      /***** If name of banner was in database... *****/
      if (Ban_DB_CheckIfBannerNameExists ("ShortName",Ban.ShrtName,-1L))
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_The_banner_X_already_exists,
                          Ban.ShrtName);
      else if (Ban_DB_CheckIfBannerNameExists ("FullName",Ban.FullName,-1L))
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_The_banner_X_already_exists,
                          Ban.FullName);
      else if (!Ban.Img[0])
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_You_must_specify_the_image_of_the_new_banner);
      else if (!Ban.WWW[0])
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_You_must_specify_the_web_address);
      else	// Add new banner to database
        {
         Ban_DB_CreateBanner (&Ban);
	 Ale_CreateAlert (Ale_SUCCESS,Txt_Created_new_banner_X,
			  Ban.ShrtName);
        }
     }
   else	// If there is not a banner name
      Ale_ShowAlertYouMustSpecifyTheShortNameAndTheFullName ();

   /***** Set editing banner to use ot in a posterior function *****/
   Ban_SetEditingBanner (&Ban);
  }

/*****************************************************************************/
/************************* Write menu with some banners **********************/
/*****************************************************************************/

void Ban_WriteMenuWithBanners (void)
  {
   struct Ban_Banners Banners;
   MYSQL_RES *mysql_res;
   unsigned NumBan;

   /***** Reset banners *****/
   Ban_ResetBanners (&Banners);

   /***** Get random banner *****/
   // The banner(s) will change once in a while
   Banners.Num = Ban_DB_GetRandomBanners (&mysql_res);
   Ban_GetListBanners (&Banners,&mysql_res);

   /***** Write all banners *****/
   for (NumBan = 0;
	NumBan < Banners.Num;
	NumBan++)
     {
      /***** Write data of this banner *****/
      /* Begin container */
      HTM_DIV_Begin ("class=\"CM\"");

         /* Begin form */
	 Frm_BeginForm (ActClkBan);
	    Ban_PutParamBanCod (Banners.Lst[NumBan].BanCod);
	    Par_PutHiddenParamString (NULL,"URL",Banners.Lst[NumBan].WWW);

	    /* Banner image */
	    HTM_INPUT_IMAGE (Cfg_URL_BANNER_PUBLIC,Banners.Lst[NumBan].Img,
			     Banners.Lst[NumBan].FullName,
			     "class=\"BANNER\"");

	 /* End form */
	 Frm_EndForm ();

      /* End container */
      HTM_DIV_End ();
     }

   /***** Free list of banners *****/
   Ban_FreeListBanners (&Banners);
  }

/*****************************************************************************/
/************************* Go to a banner when clicked ***********************/
/*****************************************************************************/

void Ban_ClickOnBanner (void)
  {
   struct Ban_Banner Ban;

   /***** Get banner code *****/
   if ((Ban.BanCod = Ban_GetParamBanCod ()) <= 0)
      Err_WrongBannerExit ();

   /***** Get data of the banner from database *****/
   Ban_GetDataOfBannerByCod (&Ban);

   /***** Set banner clicked in order to log it *****/
   Ban_SetBanCodClicked (Ban.BanCod);

   /***** Download the file *****/
   fprintf (stdout,"Location: %s\n\n",Ban.WWW);
   // TODO: Put headers Content-type and Content-disposition:
   // See: http://stackoverflow.com/questions/381954/how-do-i-fix-firefox-trying-to-save-image-as-htm
   // http://elouai.com/force-download.php
   Gbl.Layout.HTMLStartWritten =
   Gbl.Layout.DivsEndWritten   =
   Gbl.Layout.HTMLEndWritten   = true;	// Don't write HTML at all
  }

static void Ban_SetBanCodClicked (long BanCod)
  {
   Ban_BanCodClicked = BanCod;
  }

long Ban_GetBanCodClicked (void)
  {
   return Ban_BanCodClicked;
  }

/*****************************************************************************/
/******************************* Reset banners *******************************/
/*****************************************************************************/

static void Ban_ResetBanners (struct Ban_Banners *Banners)
  {
   Banners->Num           = 0;
   Banners->Lst           = NULL;
   Banners->BanCodToEdit  = -1L;
   Banners->BanCodClicked = 0L;
  }

/*****************************************************************************/
/************************* Reset banner/destructor **********************/
/*****************************************************************************/

static void Ban_ResetBanner (struct Ban_Banner *Ban)
  {
   /***** Reset banner *****/
   Ban->BanCod      = -1L;
   Ban->Hidden      = true;
   Ban->ShrtName[0] = '\0';
   Ban->FullName[0] = '\0';
   Ban->Img[0]      = '\0';
   Ban->WWW[0]      = '\0';
  }
