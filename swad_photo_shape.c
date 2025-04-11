// swad_photo_shape.h: User photo shape

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

#define _GNU_SOURCE 		// For asprintf
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_photo_shape.h"
#include "swad_setting_database.h"
#include "swad_user_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void PhoSha_PutIconsPhotoShape (__attribute__((unused)) void *Args);

/*****************************************************************************/
/******************* Put icons to select user photo shape ********************/
/*****************************************************************************/

void PhoSha_PutIconsToSelectPhotoShape (void)
  {
   extern const char *Hlp_PROFILE_Settings_user_photos;
   extern const char *Txt_User_photos;
   extern const char *Txt_PHOTO_SHAPES[PhoSha_NUM_SHAPES];
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "ICO_HIGHLIGHT PHOTOC15x20B",
      [PhoSha_SHAPE_ELLIPSE  ] = "ICO_HIGHLIGHT PHOTOE15x20B",
      [PhoSha_SHAPE_OVAL     ] = "ICO_HIGHLIGHT PHOTOO15x20B",
      [PhoSha_SHAPE_RECTANGLE] = "ICO_HIGHLIGHT PHOTOR15x20B",
     };
   PhoSha_Shape_t Shape;

   Box_BoxBegin (Txt_User_photos,PhoSha_PutIconsPhotoShape,NULL,
                 Hlp_PROFILE_Settings_user_photos,Box_NOT_CLOSABLE);

      Set_BeginSettingsHead ();
	 Set_BeginOneSettingSelector ();
	 for (Shape  = (PhoSha_Shape_t) 0;
	      Shape <= (PhoSha_Shape_t) (PhoSha_NUM_SHAPES - 1);
	      Shape++)
	   {
	    Set_BeginPref (Shape == Gbl.Prefs.PhotoShape);
	       Frm_BeginForm (ActChgUsrPho);
		  Par_PutParUnsigned (NULL,"PhotoShape",Shape);
		  HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,"user.svg",
				   Txt_PHOTO_SHAPES[Shape],
				   "class=\"%s ICO_%s_%s\"",
				   ClassPhoto[Shape],
				   Ico_GetPreffix (Ico_BLACK),The_GetSuffix ());
	       Frm_EndForm ();
	    Set_EndPref ();
	   }
	 Set_EndOneSettingSelector ();
      Set_EndSettingsHead ();

   Box_BoxEnd ();
  }

/*****************************************************************************/
/************** Put contextual icons in side-columns setting *****************/
/*****************************************************************************/

static void PhoSha_PutIconsPhotoShape (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_PHOTO_SHAPES);
  }

/*****************************************************************************/
/************************** Change user photo shape **************************/
/*****************************************************************************/

void PhoSha_ChangePhotoShape (void)
  {
   /***** Get param with user photo shape *****/
   Gbl.Prefs.PhotoShape = PhoSha_GetParPhotoShape ();

   /***** Store side colums in database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsAboutPhotoShape ();

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/************** Get parameter used to change user photo shape ****************/
/*****************************************************************************/

PhoSha_Shape_t PhoSha_GetParPhotoShape (void)
  {
   return (PhoSha_Shape_t) Par_GetParUnsignedLong ("PhotoShape",
						  (PhoSha_Shape_t) 0,
						  (PhoSha_Shape_t) (PhoSha_NUM_SHAPES - 1),
						  PhoSha_SHAPE_DEFAULT);
  }

/*****************************************************************************/
/*********************** Get photo shape from string *************************/
/*****************************************************************************/

PhoSha_Shape_t PhoSha_GetShapeFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < PhoSha_NUM_SHAPES)
	 return (PhoSha_Shape_t) UnsignedNum;

   return PhoSha_SHAPE_DEFAULT;
  }

/*****************************************************************************/
/****** Get and show number of users who have chosen a user photo shape ******/
/*****************************************************************************/

void PhoSha_GetAndShowNumUsrsPerPhotoShape (Hie_Level_t HieLvl)
  {
   extern const char *Hlp_ANALYTICS_Figures_user_photos;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_User_photos;
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_PHOTO_SHAPES[PhoSha_NUM_SHAPES];
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC15x20B",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE15x20B",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO15x20B",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR15x20B",
     };
   PhoSha_Shape_t Shape;
   char *SubQuery;
   unsigned NumUsrs[PhoSha_NUM_SHAPES];
   unsigned NumUsrsTotal = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_PHOTO_SHAPES],NULL,NULL,
                      Hlp_ANALYTICS_Figures_user_photos,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_User_photos     ,HTM_HEAD_CENTER);
	 HTM_TH (Txt_Number_of_users ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** For each user photo shape... *****/
      for (Shape  = (PhoSha_Shape_t) 0;
	   Shape <= (PhoSha_Shape_t) (PhoSha_NUM_SHAPES - 1);
	   Shape++)
	{
	 /* Get the number of users who have chosen this layout of columns from database */
	 if (asprintf (&SubQuery,"usr_data.PhotoShape=%u",(unsigned) Shape) < 0)
	    Err_NotEnoughMemoryExit ();
	 NumUsrs[Shape] = Usr_DB_GetNumUsrsWhoChoseAnOption (HieLvl,SubQuery);
	 free (SubQuery);

	 /* Update total number of users */
	 NumUsrsTotal += NumUsrs[Shape];
	}

      /***** Write number of users who have chosen this user photo shape *****/
      for (Shape  = (PhoSha_Shape_t) 0;
	   Shape <= (PhoSha_Shape_t) (PhoSha_NUM_SHAPES - 1);
	   Shape++)
	{
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_IMG (Cfg_URL_ICON_PUBLIC,"user.svg",Txt_PHOTO_SHAPES[Shape],
			"class=\"%s ICO_%s_%s\"",
			ClassPhoto[Shape],
			Ico_GetPreffix (Ico_BLACK),The_GetSuffix ());
	    HTM_TD_End ();

	    HTM_TD_Unsigned (NumUsrs[Shape]);
	    HTM_TD_Percentage (NumUsrs[Shape],NumUsrsTotal);

	 HTM_TR_End ();
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
