// swad_icon.c: icons

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

   Box_StartBox (NULL,Txt_Icons,Ico_PutIconsIconSet,
                 Hlp_PROFILE_Preferences_icons,Box_NOT_CLOSABLE);
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

/*****************************************************************************/
/***** Show contextual icons to remove, edit, view, hide, unhide, print ******/
/*****************************************************************************/

void Ico_PutContextualIconToRemove (Act_Action_t NextAction,void (*FuncParams) ())
  {
   extern const char *Txt_Remove;

   Lay_PutContextualLink (NextAction,NULL,FuncParams,
                          "remove-on64x64.png",
                          Txt_Remove,NULL,
                          NULL);
  }

void Ico_PutContextualIconToEdit (Act_Action_t NextAction,void (*FuncParams) ())
  {
   extern const char *Txt_Edit;

   Lay_PutContextualLink (NextAction,NULL,FuncParams,
                          "edit64x64.png",
                          Txt_Edit,NULL,
                          NULL);
  }

void Ico_PutContextualIconToView (Act_Action_t NextAction,void (*FuncParams) ())
  {
   extern const char *Txt_View;

   Lay_PutContextualLink (NextAction,NULL,FuncParams,
			  "eye-on64x64.png",
			  Txt_View,NULL,
                          NULL);
  }

void Ico_PutContextualIconToHide (Act_Action_t NextAction,void (*FuncParams) ())
  {
   extern const char *Txt_Hide;

   Lay_PutContextualLink (NextAction,NULL,FuncParams,
                          "eye-on64x64.png",
                          Txt_Hide,NULL,
                          NULL);
  }

void Ico_PutContextualIconToUnhide (Act_Action_t NextAction,void (*FuncParams) ())
  {
   extern const char *Txt_Show;

   Lay_PutContextualLink (NextAction,NULL,FuncParams,
                          "eye-slash-on64x64.png",
                          Txt_Show,NULL,
                          NULL);
  }

void Ico_PutContextualIconToPrint (Act_Action_t NextAction,void (*FuncParams) ())
  {
   extern const char *Txt_Print;

   Lay_PutContextualLink (NextAction,NULL,FuncParams,
                          "print64x64.png",
                          Txt_Print,NULL,
                          NULL);
  }

/*****************************************************************************/
/****************** Show an icon with a link (without text) ******************/
/*****************************************************************************/

void Ico_PutIconLink (const char *Icon,const char *Title,const char *Text,
                      const char *LinkStyle,const char *OnSubmit)
  {
   Act_LinkFormSubmit (Title,LinkStyle,OnSubmit);
   Ico_PutIconWithText (Icon,Title,Text);
   fprintf (Gbl.F.Out,"</a>");
  }

/*****************************************************************************/
/**************** Put a icon with a text to submit a form ********************/
/*****************************************************************************/

void Ico_PutIconWithText (const char *Icon,const char *Alt,const char *Text)
  {
   /***** Print icon and optional text *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_OPT ICO_HIGHLIGHT\">"
	              "<img src=\"%s/%s\" alt=\"%s\" title=\"%s\""
	              " class=\"ICO20x20\" />",
            Gbl.Prefs.IconsURL,Icon,
            Alt,Text ? Text : Alt);
   if (Text)
      if (Text[0])
	 fprintf (Gbl.F.Out,"&nbsp;%s",Text);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********** Put a icon to submit a form.                            **********/
/********** When clicked, the icon will be replaced by an animation **********/
/*****************************************************************************/

void Ico_PutCalculateIcon (const char *Alt)
  {
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_OPT ICO_HIGHLIGHT\">"
	              "<img id=\"update_%d\" src=\"%s/recycle16x16.gif\""	// TODO: change name and resolution to refresh64x64.png
	              " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "<img id=\"updating_%d\" src=\"%s/working16x16.gif\""	// TODO: change name and resolution to refreshing64x64.gif
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" style=\"display:none;\" />"	// Animated icon hidden
		      "</div>"
		      "</a>",
	    Gbl.Form.Num,Gbl.Prefs.IconsURL,Alt,Alt,
	    Gbl.Form.Num,Gbl.Prefs.IconsURL,Alt,Alt);
  }

/*****************************************************************************/
/********** Put a icon with a text to submit a form.                **********/
/********** When clicked, the icon will be replaced by an animation **********/
/*****************************************************************************/

void Ico_PutCalculateIconWithText (const char *Alt,const char *Text)
  {
   fprintf (Gbl.F.Out,"<div class=\"ICO_HIGHLIGHT\""
	              " style=\"margin:0 6px 0 0; display:inline;\">"
	              "<img id=\"update_%d\" src=\"%s/recycle16x16.gif\""
	              " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "<img id=\"updating_%d\" src=\"%s/working16x16.gif\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" style=\"display:none;\" />"	// Animated icon hidden
		      "&nbsp;%s"
		      "</div>"
		      "</a>",
	    Gbl.Form.Num,Gbl.Prefs.IconsURL,Alt,Text,
	    Gbl.Form.Num,Gbl.Prefs.IconsURL,Alt,Text,
	    Text);
  }

/*****************************************************************************/
/******** Put a disabled icon indicating that removal is not allowed *********/
/*****************************************************************************/

void Ico_PutIconRemovalNotAllowed (void)
  {
   extern const char *Txt_Removal_not_allowed;

   fprintf (Gbl.F.Out,"<img src=\"%s/remove-off64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />",
	    Gbl.Prefs.IconsURL,
	    Txt_Removal_not_allowed,
	    Txt_Removal_not_allowed);
  }

void Ico_PutIconBRemovalNotAllowed (void)
  {
   extern const char *Txt_Removal_not_allowed;

   fprintf (Gbl.F.Out,"<img src=\"%s/remove-off64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20B\" />",
	    Gbl.Prefs.IconsURL,
	    Txt_Removal_not_allowed,
	    Txt_Removal_not_allowed);
  }

/*****************************************************************************/
/******** Put an icon indicating that removal is not allowed *********/
/*****************************************************************************/

void Ico_PutIconRemove (void)
  {
   extern const char *Txt_Remove;

   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/remove-on64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />",
	    Gbl.Prefs.IconsURL,
	    Txt_Remove,
	    Txt_Remove);
  }

