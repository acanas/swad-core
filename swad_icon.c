// swad_icon.c: icon selection

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include <stdio.h>	// For fprintf, etc.
#include <string.h>

#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_icon.h"
#include "swad_layout.h"
#include "swad_parameter.h"
#include "swad_preference.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************** Private constants **************************/
/*****************************************************************************/

#define Ico_MAX_BYTES_ICON_SET_ID 16

const char *Ico_IconSetId[Ico_NUM_ICON_SETS] =
  {
   "awesome",
   "nuvola",
  };

const char *Ico_IconSetNames[Ico_NUM_ICON_SETS] =
  {
   "Awesome",
   "Nuvola",
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ico_PutIconsIconSet (void);

/*****************************************************************************/
/************************ Put icons to select a IconSet **********************/
/*****************************************************************************/

void Ico_PutIconsToSelectIconSet (void)
  {
   extern const char *Hlp_PROFILE_Preferences_icons;
   extern const char *Txt_Icons;
   Ico_IconSet_t IconSet;

   Box_StartBox (NULL,Txt_Icons,
                        Ico_PutIconsIconSet,Hlp_PROFILE_Preferences_icons,
                        false);	// Not closable
   fprintf (Gbl.F.Out,"<div class=\"PREF_CONTAINER\">");
   for (IconSet = (Ico_IconSet_t) 0;
	IconSet < Ico_NUM_ICON_SETS;
	IconSet++)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\">",
               IconSet == Gbl.Prefs.IconSet ? "PREF_ON" :
        	                              "PREF_OFF");
      Act_FormStart (ActChgIco);
      Par_PutHiddenParamString ("IconSet",Ico_IconSetId[IconSet]);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s/%s/%s/heart64x64.gif\""
	                 " alt=\"%s\" title=\"%s\" class=\"ICO25x25B\" />",
               Gbl.Prefs.IconsURL,
               Cfg_ICON_FOLDER_ICON_SETS,
               Ico_IconSetId[IconSet],
               Cfg_ICON_ACTION,
               Ico_IconSetNames[IconSet],
               Ico_IconSetNames[IconSet]);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");
     }
   fprintf (Gbl.F.Out,"</div>");
   Box_EndBox ();
  }

/*****************************************************************************/
/*************** Put contextual icons in icon-set preference *****************/
/*****************************************************************************/

static void Ico_PutIconsIconSet (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_ICON_SETS;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/***************************** Change icon set *******************************/
/*****************************************************************************/

void Ico_ChangeIconSet (void)
  {
   char Query[512];

   /***** Get param with icon set *****/
   Gbl.Prefs.IconSet = Ico_GetParamIconSet ();
   sprintf (Gbl.Prefs.PathIconSet,"%s/%s/%s",
            Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_ICON_SETS,
            Ico_IconSetId[Gbl.Prefs.IconSet]);

   /***** Store icon set in database *****/
   if (Gbl.Usrs.Me.Logged)
     {
      sprintf (Query,"UPDATE usr_data SET IconSet='%s' WHERE UsrCod=%ld",
               Ico_IconSetId[Gbl.Prefs.IconSet],
               Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE (Query,"can not update your preference about icon set");
     }

   /***** Set preferences from current IP *****/
   Pre_SetPrefsFromIP ();
  }

/*****************************************************************************/
/*********************** Get parameter with icon set *************************/
/*****************************************************************************/

Ico_IconSet_t Ico_GetParamIconSet (void)
  {
   char IconSetId[Ico_MAX_BYTES_ICON_SET_ID + 1];
   Ico_IconSet_t IconSet;

   Par_GetParToText ("IconSet",IconSetId,Ico_MAX_BYTES_ICON_SET_ID);
   for (IconSet = (Ico_IconSet_t) 0;
	IconSet < Ico_NUM_ICON_SETS;
	IconSet++)
      if (!strcmp (IconSetId,Ico_IconSetId[IconSet]))
         return IconSet;

   return Ico_ICON_SET_DEFAULT;
  }

/*****************************************************************************/
/************************* Get icon set from string **************************/
/*****************************************************************************/

Ico_IconSet_t Ico_GetIconSetFromStr (const char *Str)
  {
   Ico_IconSet_t IconSet;

   for (IconSet = (Ico_IconSet_t) 0;
	IconSet < Ico_NUM_ICON_SETS;
	IconSet++)
      if (!strcasecmp (Str,Ico_IconSetId[IconSet]))
	 return IconSet;

   return Ico_ICON_SET_DEFAULT;
  }
