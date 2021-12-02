// swad_photo_shape.h: User photo shape

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

#include "swad_action.h"
#include "swad_config.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_photo_shape.h"
#include "swad_setting_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Pho_PutIconsPhotoShape (__attribute__((unused)) void *Args);

/*****************************************************************************/
/******************* Put icons to select user photo shape ********************/
/*****************************************************************************/

void Pho_PutIconsToSelectPhotoShape (void)
  {
   extern const char *Hlp_PROFILE_Settings_user_photos;
   extern const char *Txt_User_photos;
   extern const char *Txt_PHOTO_SHAPES[Pho_NUM_SHAPES];
   static const char *ClassPhoto[Pho_NUM_SHAPES] =
     {
      [Pho_SHAPE_CIRCLE   ] = "ICO_HIGHLIGHT PHOTOC15x20B",
      [Pho_SHAPE_ELLIPSE  ] = "ICO_HIGHLIGHT PHOTOE15x20B",
      [Pho_SHAPE_OVAL     ] = "ICO_HIGHLIGHT PHOTOO15x20B",
      [Pho_SHAPE_RECTANGLE] = "ICO_HIGHLIGHT PHOTOR15x20B",
     };
   Pho_Shape_t Shape;

   Box_BoxBegin (NULL,Txt_User_photos,
                 Pho_PutIconsPhotoShape,NULL,
                 Hlp_PROFILE_Settings_user_photos,Box_NOT_CLOSABLE);
      Set_BeginSettingsHead ();
	 Set_BeginOneSettingSelector ();
	 for (Shape  = (Pho_Shape_t) 0;
	      Shape <= (Pho_Shape_t) (Pho_NUM_SHAPES - 1);
	      Shape++)
	   {
	    HTM_DIV_Begin ("class=\"%s\"",Shape == Gbl.Prefs.PhotoShape ? "PREF_ON" :
									  "PREF_OFF");
	       Frm_BeginForm (ActChgUsrPho);
		  Par_PutHiddenParamUnsigned (NULL,"PhotoShape",Shape);
		  HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,"user.svg",Txt_PHOTO_SHAPES[Shape],ClassPhoto[Shape]);
	       Frm_EndForm ();
	    HTM_DIV_End ();
	   }
	 Set_EndOneSettingSelector ();
      Set_EndSettingsHead ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************** Put contextual icons in side-columns setting *****************/
/*****************************************************************************/

static void Pho_PutIconsPhotoShape (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_PHOTO_SHAPES);
  }

/*****************************************************************************/
/************************** Change user photo shape **************************/
/*****************************************************************************/

void Pho_ChangePhotoShape (void)
  {
   /***** Get param with user photo shape *****/
   Gbl.Prefs.PhotoShape = Pho_GetParamPhotoShape ();

   /***** Store side colums in database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsAboutPhotoShape ();

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/************** Get parameter used to change user photo shape ****************/
/*****************************************************************************/

Pho_Shape_t Pho_GetParamPhotoShape (void)
  {
   return (Pho_Shape_t) Par_GetParToUnsignedLong ("PhotoShape",
						  (Pho_Shape_t) 0,
						  (Pho_Shape_t) (Pho_NUM_SHAPES - 1),
						  Pho_SHAPE_DEFAULT);
  }

/*****************************************************************************/
/*********************** Get photo shape from string *************************/
/*****************************************************************************/

Pho_Shape_t Pho_GetShapeFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Pho_NUM_SHAPES)
	 return (Pho_Shape_t) UnsignedNum;

   return Pho_SHAPE_DEFAULT;
  }
