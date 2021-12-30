// swad_icon.c: icons

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Ca�as Vargas

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
#include <stdio.h>		// For asprintf
#include <string.h>

#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_icon.h"
#include "swad_layout.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_setting_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************** Public constants ***************************/
/*****************************************************************************/

#define Ico_MAX_BYTES_ICON_SET_ID 16

const char *Ico_IconSetId[Ico_NUM_ICON_SETS] =
  {
   [Ico_ICON_SET_AWESOME] = "awesome",
   [Ico_ICON_SET_NUVOLA ] = "nuvola",
  };

const char *Ico_IconSetNames[Ico_NUM_ICON_SETS] =
  {
   [Ico_ICON_SET_AWESOME] = "Awesome",
   [Ico_ICON_SET_NUVOLA ] = "Nuvola",
  };

const char *Ico_ClassColor[Ico_NUM_COLORS][The_NUM_THEMES] =
     {
      [Ico_UNCHANGED][The_THEME_WHITE ] = "",
      [Ico_UNCHANGED][The_THEME_GREY  ] = "",
      [Ico_UNCHANGED][The_THEME_PURPLE] = "",
      [Ico_UNCHANGED][The_THEME_BLUE  ] = "",
      [Ico_UNCHANGED][The_THEME_YELLOW] = "",
      [Ico_UNCHANGED][The_THEME_PINK  ] = "",
      [Ico_UNCHANGED][The_THEME_DARK  ] = "",

      [Ico_BLACK    ][The_THEME_WHITE ] = "BLACK_ICO_WHITE",
      [Ico_BLACK    ][The_THEME_GREY  ] = "BLACK_ICO_GREY",
      [Ico_BLACK    ][The_THEME_PURPLE] = "BLACK_ICO_PURPLE",
      [Ico_BLACK    ][The_THEME_BLUE  ] = "BLACK_ICO_BLUE",
      [Ico_BLACK    ][The_THEME_YELLOW] = "BLACK_ICO_YELLOW",
      [Ico_BLACK    ][The_THEME_PINK  ] = "BLACK_ICO_PINK",
      [Ico_BLACK    ][The_THEME_DARK  ] = "BLACK_ICO_DARK",

      [Ico_GREEN    ][The_THEME_WHITE ] = "GREEN_ICO_WHITE",
      [Ico_GREEN    ][The_THEME_GREY  ] = "GREEN_ICO_GREY",
      [Ico_GREEN    ][The_THEME_PURPLE] = "GREEN_ICO_PURPLE",
      [Ico_GREEN    ][The_THEME_BLUE  ] = "GREEN_ICO_BLUE",
      [Ico_GREEN    ][The_THEME_YELLOW] = "GREEN_ICO_YELLOW",
      [Ico_GREEN    ][The_THEME_PINK  ] = "GREEN_ICO_PINK",
      [Ico_GREEN    ][The_THEME_DARK  ] = "GREEN_ICO_DARK",

      [Ico_RED      ][The_THEME_WHITE ] = "RED_ICO_WHITE",
      [Ico_RED      ][The_THEME_GREY  ] = "RED_ICO_GREY",
      [Ico_RED      ][The_THEME_PURPLE] = "RED_ICO_PURPLE",
      [Ico_RED      ][The_THEME_BLUE  ] = "RED_ICO_BLUE",
      [Ico_RED      ][The_THEME_YELLOW] = "RED_ICO_YELLOW",
      [Ico_RED      ][The_THEME_PINK  ] = "RED_ICO_PINK",
      [Ico_RED      ][The_THEME_DARK  ] = "RED_ICO_DARK",

      [Ico_WHITE    ][The_THEME_WHITE ] = "WHITE_ICO_WHITE",
      [Ico_WHITE    ][The_THEME_GREY  ] = "WHITE_ICO_GREY",
      [Ico_WHITE    ][The_THEME_PURPLE] = "WHITE_ICO_PURPLE",
      [Ico_WHITE    ][The_THEME_BLUE  ] = "WHITE_ICO_BLUE",
      [Ico_WHITE    ][The_THEME_YELLOW] = "WHITE_ICO_YELLOW",
      [Ico_WHITE    ][The_THEME_PINK  ] = "WHITE_ICO_PINK",
      [Ico_WHITE    ][The_THEME_DARK  ] = "WHITE_ICO_DARK",
     };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ico_PutIconsIconSet (__attribute__((unused)) void *Args);

/*****************************************************************************/
/*********** Get icon with extension from icon without extension *************/
/*****************************************************************************/

#define Ico_NUM_ICON_EXTENSIONS 3

const char *Ico_GetIcon (const char *IconWithoutExtension)
  {
   static const char *Ico_IconExtensions[Ico_NUM_ICON_EXTENSIONS] =
     {	// In order of preference
      "svg",
      "png",
      "gif",
     };
   static char IconWithExtension[NAME_MAX + 1];
   char PathIcon[PATH_MAX + 1];
   unsigned NumExt;

   for (NumExt = 0;
        NumExt < Ico_NUM_ICON_EXTENSIONS;
        NumExt++)
     {
      snprintf (IconWithExtension,sizeof (IconWithExtension),"%s.%s",
		IconWithoutExtension,Ico_IconExtensions[NumExt]);
      snprintf (PathIcon,sizeof (PathIcon),"%s/%s/%s",
		Cfg_PATH_ICON_SETS_PUBLIC,
		Ico_IconSetId[Gbl.Prefs.IconSet],
		IconWithExtension);
      if (Fil_CheckIfPathExists (PathIcon))
	 return IconWithExtension;
     }

   return "default.svg";
  }

/*****************************************************************************/
/************************ Put icons to select a IconSet **********************/
/*****************************************************************************/

void Ico_PutIconsToSelectIconSet (void)
  {
   extern const char *The_Colors[The_NUM_THEMES];
   extern const char *Hlp_PROFILE_Settings_icons;
   extern const char *Txt_Icons;
   Ico_IconSet_t IconSet;
   char Icon[PATH_MAX + 1];

   Box_BoxBegin (NULL,Txt_Icons,
                 Ico_PutIconsIconSet,NULL,
                 Hlp_PROFILE_Settings_icons,Box_NOT_CLOSABLE);
      Set_BeginSettingsHead ();
	 Set_BeginOneSettingSelector ();
	    for (IconSet  = (Ico_IconSet_t) 0;
		 IconSet <= (Ico_IconSet_t) (Ico_NUM_ICON_SETS - 1);
		 IconSet++)
	      {
	       if (IconSet == Gbl.Prefs.IconSet)
		  HTM_DIV_Begin ("class=\"PREF_ON PREF_ON_%s\"",
		                 The_Colors[Gbl.Prefs.Theme]);
	       else
		  HTM_DIV_Begin ("class=\"PREF_OFF\"");
	       Frm_BeginForm (ActChgIco);
		  Par_PutHiddenParamString (NULL,"IconSet",Ico_IconSetId[IconSet]);
		  snprintf (Icon,sizeof (Icon),"%s/%s/cog.svg",
			    Cfg_ICON_FOLDER_SETS,
			    Ico_IconSetId[IconSet]);
		  Ico_PutSettingIconLink (Icon,Ico_UNCHANGED,Ico_IconSetNames[IconSet]);
	       Frm_EndForm ();
	       HTM_DIV_End ();
	      }
	 Set_EndOneSettingSelector ();
      Set_EndSettingsHead ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/***************** Put contextual icons in icon-set setting *******************/
/*****************************************************************************/

static void Ico_PutIconsIconSet (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_ICON_SETS);
  }

/*****************************************************************************/
/***************************** Change icon set *******************************/
/*****************************************************************************/

void Ico_ChangeIconSet (void)
  {
   /***** Get param with icon set *****/
   Gbl.Prefs.IconSet = Ico_GetParamIconSet ();
   snprintf (Gbl.Prefs.URLIconSet,sizeof (Gbl.Prefs.URLIconSet),"%s/%s",
             Cfg_URL_ICON_SETS_PUBLIC,
             Ico_IconSetId[Gbl.Prefs.IconSet]);

   /***** Store icon set in database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsAboutIconSet (Ico_IconSetId[Gbl.Prefs.IconSet]);

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/*********************** Get parameter with icon set *************************/
/*****************************************************************************/

Ico_IconSet_t Ico_GetParamIconSet (void)
  {
   char IconSetId[Ico_MAX_BYTES_ICON_SET_ID + 1];
   Ico_IconSet_t IconSet;

   Par_GetParToText ("IconSet",IconSetId,Ico_MAX_BYTES_ICON_SET_ID);
   for (IconSet  = (Ico_IconSet_t) 0;
	IconSet <= (Ico_IconSet_t) (Ico_NUM_ICON_SETS - 1);
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

   for (IconSet  = (Ico_IconSet_t) 0;
	IconSet <= (Ico_IconSet_t) (Ico_NUM_ICON_SETS - 1);
	IconSet++)
      if (!strcasecmp (Str,Ico_IconSetId[IconSet]))
	 return IconSet;

   return Ico_ICON_SET_DEFAULT;
  }

/*****************************************************************************/
/*** Show contextual icons to add, remove, edit, view, hide, unhide, print ***/
/*****************************************************************************/

void Ico_PutContextualIconToAdd (Act_Action_t NextAction,const char *Anchor,
				 void (*FuncParams) (void *Args),void *Args,
				 const char *Txt)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,
                                  FuncParams,Args,
				  "plus.svg",Ico_BLACK,
				  Txt);
  }

void Ico_PutContextualIconToRemove (Act_Action_t NextAction,const char *Anchor,
                                    void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_Remove;

   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,
                                  FuncParams,Args,
				  "trash.svg",Ico_RED,
				  Txt_Remove);
  }

void Ico_PutContextualIconToEdit (Act_Action_t NextAction,const char *Anchor,
				  void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_Edit;

   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,
                                  FuncParams,Args,
				  "pen.svg",Ico_BLACK,
				  Txt_Edit);
  }

void Ico_PutContextualIconToViewFiles (Act_Action_t NextAction,
                                       void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_Files;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,
                                  FuncParams,Args,
				  "folder-open.svg",Ico_BLACK,
				  Txt_Files);
  }

void Ico_PutContextualIconToView (Act_Action_t NextAction,
                                  void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_View;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,
                                  FuncParams,Args,
				  "list.svg",Ico_BLACK,
				  Txt_View);
  }

void Ico_PutContextualIconToConfigure (Act_Action_t NextAction,
                                       void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_Configure;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,
                                  FuncParams,Args,
				  "cog.svg",Ico_BLACK,
				  Txt_Configure);
  }

void Ico_PutContextualIconToHide (Act_Action_t NextAction,const char *Anchor,
                                  void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_Hide;

   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,
                                  FuncParams,Args,
				  "eye.svg",Ico_GREEN,
				  Txt_Hide);
  }

void Ico_PutContextualIconToUnhide (Act_Action_t NextAction,const char *Anchor,
                                    void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_Show;

   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,
                                  FuncParams,Args,
				  "eye-slash.svg",Ico_RED,
				  Txt_Show);
  }

void Ico_PutContextualIconToPrint (Act_Action_t NextAction,
                                   void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_Print;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,
                                  FuncParams,Args,
				  "print.svg",Ico_BLACK,
				  Txt_Print);
  }

void Ico_PutContextualIconToCopy (Act_Action_t NextAction,
                                  void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_Copy;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,
                                  FuncParams,Args,
				  "copy.svg",Ico_BLACK,
				  Txt_Copy);
  }

void Ico_PutContextualIconToPaste (Act_Action_t NextAction,
                                   void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_Paste;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,
                                  FuncParams,Args,
				  "paste.svg",Ico_BLACK,
				  Txt_Paste);
  }

void Ico_PutContextualIconToCreateInFolder (Act_Action_t NextAction,
                                            void (*FuncParams) (void *Args),void *Args,
                                            bool Open)
  {
   extern const char *Txt_Upload_file_or_create_folder;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,
                                  FuncParams,Args,
				  Open ? "folder-open-yellow-plus.png" :
				  	 "folder-yellow-plus.png",
				  Ico_UNCHANGED,
				  Txt_Upload_file_or_create_folder);
  }

void Ico_PutContextualIconToShowResults (Act_Action_t NextAction,const char *Anchor,
                                         void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_Results;

   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,
                                  FuncParams,Args,
				  "trophy.svg",Ico_BLACK,
				  Txt_Results);
  }

void Ico_PutContextualIconToShowAttendanceList (Act_Action_t NextAction,
                                                void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_Attendance_list;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,
                                  FuncParams,Args,
				  "tasks.svg",Ico_BLACK,
				  Txt_Attendance_list);
  }

void Ico_PutContextualIconToZIP (Act_Action_t NextAction,
                                 void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_Create_ZIP_file;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,
                                  FuncParams,Args,
				  "download.svg",Ico_BLACK,
				  Txt_Create_ZIP_file);
  }

/*****************************************************************************/
/**************** Show an icon inside a div (without text) *******************/
/*****************************************************************************/

void Ico_PutDivIcon (const char *DivClass,const char *Icon,Ico_Color_t Color,const char *Title)
  {
   char *Class;

   HTM_DIV_Begin ("class=\"%s\"",DivClass);

      if (asprintf (&Class,"CONTEXT_ICO_16x16 %s",
                    Ico_ClassColor[Color][Gbl.Prefs.Theme]) < 0)
	 Err_NotEnoughMemoryExit ();
      Ico_PutIcon (Icon,Color,Title,Class);
      free (Class);

   HTM_DIV_End ();
  }

/*****************************************************************************/
/****************** Show an icon with a link (without text) ******************/
/*****************************************************************************/

void Ico_PutIconLink (const char *Icon,Ico_Color_t Color,const char *Title)
  {
   char *Class;

   if (asprintf (&Class,"CONTEXT_OPT ICO_HIGHLIGHT CONTEXT_ICO_16x16 %s",
		 Ico_ClassColor[Color][Gbl.Prefs.Theme]) < 0)
      Err_NotEnoughMemoryExit ();
   HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,Icon,Title,Class);
   free (Class);
  }

/*****************************************************************************/
/******************* Show an icon with a link (with text) ********************/
/*****************************************************************************/

void Ico_PutIconTextLink (const char *Icon,Ico_Color_t Color,const char *Text)
  {
   /***** Print icon and optional text *****/
   HTM_DIV_Begin ("class=\"CONTEXT_OPT ICO_HIGHLIGHT\"");
      Ico_PutIcon (Icon,Color,Text,"CONTEXT_ICO_x16");
      HTM_TxtF ("&nbsp;%s",Text);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************************** Show a setting selector *************************/
/*****************************************************************************/

void Ico_PutSettingIconLink (const char *Icon,Ico_Color_t Color,const char *Title)
  {
   char *Class;

   if (asprintf (&Class,"ICO_HIGHLIGHT ICOx20 %s",
		 Ico_ClassColor[Color][Gbl.Prefs.Theme]) < 0)
      Err_NotEnoughMemoryExit ();
   HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,Icon,Title,Class);
   free (Class);
  }

/*****************************************************************************/
/********************* Put an active or disabled icon ************************/
/*****************************************************************************/

void Ico_PutIconOn (const char *Icon,Ico_Color_t Color,const char *Title)
  {
   Ico_PutIcon (Icon,Color,Title,"CONTEXT_OPT CONTEXT_ICO_16x16");
  }

void Ico_PutIconOff (const char *Icon,Ico_Color_t Color,const char *Title)
  {
   Ico_PutIcon (Icon,Color,Title,"CONTEXT_OPT CONTEXT_ICO_16x16 ICO_HIDDEN");
  }

/*****************************************************************************/
/******************************* Put an icon *********************************/
/*****************************************************************************/

void Ico_PutIcon (const char *Icon,Ico_Color_t Color,const char *Title,const char *Class)
  {
   const char *CC = Ico_ClassColor[Color][Gbl.Prefs.Theme];

   HTM_IMG (Cfg_URL_ICON_PUBLIC,Icon,Title,
	    "class=\"%s%s%s\"",Class,CC[0] ? " " : "",CC);
  }

/*****************************************************************************/
/******** Put a disabled icon indicating that removal is not allowed *********/
/*****************************************************************************/

void Ico_PutIconRemovalNotAllowed (void)
  {
   extern const char *Txt_Removal_not_allowed;

   Ico_PutIconOff ("trash.svg",Ico_RED,Txt_Removal_not_allowed);
  }

/*****************************************************************************/
/*************************** Put an icon to cut ******************************/
/*****************************************************************************/

void Ico_PutIconCut (void)
  {
   extern const char *Txt_Cut;

   Ico_PutIconLink ("cut.svg",Ico_BLACK,Txt_Cut);
  }

/*****************************************************************************/
/************************** Put an icon to paste *****************************/
/*****************************************************************************/

void Ico_PutIconPaste (void)
  {
   extern const char *Txt_Paste;

   Ico_PutIconLink ("paste.svg",Ico_BLACK,Txt_Paste);
  }

/*****************************************************************************/
/************* Put icon indicating that a content is not visible *************/
/*****************************************************************************/

void Ico_PutIconNotVisible (void)
  {
   extern const char *Txt_Not_visible;

   Ico_PutIconOff ("eye-slash.svg",Ico_RED,Txt_Not_visible);
  }
