// swad_icon.c: icons

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

#include <string.h>

#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_icon.h"
#include "swad_layout.h"
#include "swad_parameter.h"
#include "swad_setting.h"

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
      snprintf (IconWithExtension,sizeof (IconWithExtension),
		"%s.%s",
		IconWithoutExtension,Ico_IconExtensions[NumExt]);
      snprintf (PathIcon,sizeof (PathIcon),
		"%s/%s/%s",
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
   extern const char *Hlp_PROFILE_Settings_icons;
   extern const char *Txt_Icons;
   Ico_IconSet_t IconSet;
   char Icon[PATH_MAX + 1];

   Box_BoxBegin (NULL,Txt_Icons,Ico_PutIconsIconSet,
                 Hlp_PROFILE_Settings_icons,Box_NOT_CLOSABLE);
   Set_StartSettingsHead ();
   Set_StartOneSettingSelector ();
   for (IconSet = (Ico_IconSet_t) 0;
	IconSet < Ico_NUM_ICON_SETS;
	IconSet++)
     {
      HTM_DIV_Begin ("class=\"%s\"",
                     IconSet == Gbl.Prefs.IconSet ? "PREF_ON" :
        	                                    "PREF_OFF");
      Frm_StartForm (ActChgIco);
      Par_PutHiddenParamString ("IconSet",Ico_IconSetId[IconSet]);
      snprintf (Icon,sizeof (Icon),
		"%s/%s/cog.svg",
		Cfg_ICON_FOLDER_SETS,
                Ico_IconSetId[IconSet]);
      Ico_PutSettingIconLink (Icon,Ico_IconSetNames[IconSet]);
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

static void Ico_PutIconsIconSet (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_ICON_SETS;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/***************************** Change icon set *******************************/
/*****************************************************************************/

void Ico_ChangeIconSet (void)
  {
   /***** Get param with icon set *****/
   Gbl.Prefs.IconSet = Ico_GetParamIconSet ();
   snprintf (Gbl.Prefs.URLIconSet,sizeof (Gbl.Prefs.URLIconSet),
	     "%s/%s",
             Cfg_URL_ICON_SETS_PUBLIC,
             Ico_IconSetId[Gbl.Prefs.IconSet]);

   /***** Store icon set in database *****/
   if (Gbl.Usrs.Me.Logged)
      DB_QueryUPDATE ("can not update your setting about icon set",
		      "UPDATE usr_data SET IconSet='%s' WHERE UsrCod=%ld",
		      Ico_IconSetId[Gbl.Prefs.IconSet],
		      Gbl.Usrs.Me.UsrDat.UsrCod);

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
/*** Show contextual icons to add, remove, edit, view, hide, unhide, print ***/
/*****************************************************************************/

void Ico_PutContextualIconToAdd (Act_Action_t NextAction,const char *Anchor,
				 void (*FuncParams) (void),
				 const char *Txt)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncParams,
				  "plus.svg",
				  Txt);
  }

void Ico_PutContextualIconToRemove (Act_Action_t NextAction,void (*FuncParams) (void))
  {
   extern const char *Txt_Remove;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncParams,
				  "trash.svg",
				  Txt_Remove);
  }

void Ico_PutContextualIconToEdit (Act_Action_t NextAction,void (*FuncParams) (void))
  {
   extern const char *Txt_Edit;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncParams,
				  "pen.svg",
				  Txt_Edit);
  }

void Ico_PutContextualIconToViewFiles (Act_Action_t NextAction,void (*FuncParams) (void))
  {
   extern const char *Txt_Files;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncParams,
				  "folder-open.svg",
				  Txt_Files);
  }

void Ico_PutContextualIconToView (Act_Action_t NextAction,void (*FuncParams) (void))
  {
   extern const char *Txt_View;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncParams,
				  "eye.svg",
				  Txt_View);
  }

void Ico_PutContextualIconToConfigure (Act_Action_t NextAction,void (*FuncParams) (void))
  {
   extern const char *Txt_Configure;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncParams,
				  "cog.svg",
				  Txt_Configure);
  }

void Ico_PutContextualIconToHide (Act_Action_t NextAction,const char *Anchor,void (*FuncParams) (void))
  {
   extern const char *Txt_Hide;

   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncParams,
				  "eye.svg",
				  Txt_Hide);
  }

void Ico_PutContextualIconToUnhide (Act_Action_t NextAction,const char *Anchor,void (*FuncParams) (void))
  {
   extern const char *Txt_Show;

   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncParams,
				  "eye-slash.svg",
				  Txt_Show);
  }

void Ico_PutContextualIconToPrint (Act_Action_t NextAction,void (*FuncParams) (void))
  {
   extern const char *Txt_Print;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncParams,
				  "print.svg",
				  Txt_Print);
  }

void Ico_PutContextualIconToCopy (Act_Action_t NextAction,void (*FuncParams) (void))
  {
   extern const char *Txt_Copy;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncParams,
				  "copy.svg",
				  Txt_Copy);
  }

void Ico_PutContextualIconToPaste (Act_Action_t NextAction,void (*FuncParams) (void))
  {
   extern const char *Txt_Paste;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncParams,
				  "paste.svg",
				  Txt_Paste);
  }

void Ico_PutContextualIconToCreateInFolder (Act_Action_t NextAction,void (*FuncParams) (void),bool Open)
  {
   extern const char *Txt_Upload_file_or_create_folder;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncParams,
				  Open ? "folder-open-yellow-plus.png" :
				  	 "folder-yellow-plus.png",
				  Txt_Upload_file_or_create_folder);
  }

void Ico_PutContextualIconToZIP (Act_Action_t NextAction,void (*FuncParams) (void))
  {
   extern const char *Txt_Create_ZIP_file;

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncParams,
				  "download.svg",
				  Txt_Create_ZIP_file);
  }

/*****************************************************************************/
/**************** Show an icon inside a div (without text) *******************/
/*****************************************************************************/

void Ico_PutDivIcon (const char *DivClass,const char *Icon,const char *Title)
  {
   HTM_DIV_Begin ("class=\"%s\"",DivClass);
   Ico_PutIcon (Icon,Title,"CONTEXT_ICO_16x16");
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*********** Show an icon with a link inside a div (without text) ************/
/*****************************************************************************/

void Ico_PutDivIconLink (const char *DivClass,const char *Icon,const char *Title)
  {
   HTM_DIV_Begin ("class=\"%s\"",DivClass);
   Ico_PutIconLink (Icon,Title);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/****************** Show an icon with a link (without text) ******************/
/*****************************************************************************/

void Ico_PutIconLink (const char *Icon,const char *Title)
  {
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"CONTEXT_OPT ICO_HIGHLIGHT CONTEXT_ICO_16x16\" />",
	    Cfg_URL_ICON_PUBLIC,Icon,
	    Title,Title);
  }

/*****************************************************************************/
/******************* Show an icon with a link (with text) ********************/
/*****************************************************************************/

void Ico_PutIconTextLink (const char *Icon,const char *Text)
  {
   /***** Print icon and optional text *****/
   HTM_DIV_Begin ("class=\"CONTEXT_OPT ICO_HIGHLIGHT\"");
   Ico_PutIcon (Icon,Text,"CONTEXT_ICO_x16");
   fprintf (Gbl.F.Out,"&nbsp;%s",Text);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************************** Show a setting selector *************************/
/*****************************************************************************/

void Ico_PutSettingIconLink (const char *Icon,const char *Title)
  {
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO_HIGHLIGHT ICOx20\" />",
	    Cfg_URL_ICON_PUBLIC,Icon,
	    Title,Title);
  }

/*****************************************************************************/
/********************** Put an inactive/disabled icon ************************/
/*****************************************************************************/

void Ico_PutIconOff (const char *Icon,const char *Title)
  {
   Ico_PutIcon (Icon,Title,"CONTEXT_OPT ICO_HIDDEN CONTEXT_ICO_16x16");
  }

/*****************************************************************************/
/******************************* Put an icon *********************************/
/*****************************************************************************/

void Ico_PutIcon (const char *Icon,const char *Title,const char *Class)
  {
   HTM_IMG (Cfg_URL_ICON_PUBLIC,Icon,Title,
	    Class,NULL,NULL);
  }

/*****************************************************************************/
/********** Put a icon to submit a form.                            **********/
/********** When clicked, the icon will be replaced by an animation **********/
/*****************************************************************************/

void Ico_PutCalculateIcon (const char *Title)
  {
   HTM_DIV_Begin ("class=\"CONTEXT_OPT ICO_HIGHLIGHT\"");

   fprintf (Gbl.F.Out,"<img id=\"update_%d\" src=\"%s/recycle16x16.gif\""	// TODO: change name and resolution to refresh64x64.png
	              " alt=\"%s\" title=\"%s\""
		      " class=\"CONTEXT_ICO_16x16\" />",
	    Gbl.Form.Num,Cfg_URL_ICON_PUBLIC,Title,Title);

   fprintf (Gbl.F.Out,"<img id=\"updating_%d\" src=\"%s/working16x16.gif\""	// TODO: change name and resolution to refreshing64x64.gif
		      " alt=\"%s\" title=\"%s\""
		      " class=\"CONTEXT_ICO_16x16\" style=\"display:none;\" />",	// Animated icon hidden
	    Gbl.Form.Num,Cfg_URL_ICON_PUBLIC,Title,Title);

   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Put a icon with a text to submit a form.                **********/
/********** When clicked, the icon will be replaced by an animation **********/
/*****************************************************************************/

void Ico_PutCalculateIconWithText (const char *Text)
  {
   HTM_DIV_Begin ("class=\"ICO_HIGHLIGHT\" style=\"margin:0 6px 0 0; display:inline;\"");

   fprintf (Gbl.F.Out,"<img id=\"update_%d\" src=\"%s/recycle16x16.gif\""
	              " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />",
	    Gbl.Form.Num,Cfg_URL_ICON_PUBLIC,Text,Text);

   fprintf (Gbl.F.Out,"<img id=\"updating_%d\" src=\"%s/working16x16.gif\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" style=\"display:none;\" />",	// Animated icon hidden
	    Gbl.Form.Num,Cfg_URL_ICON_PUBLIC,Text,Text);

   fprintf (Gbl.F.Out,"&nbsp;%s",Text);

   HTM_DIV_End ();
  }

/*****************************************************************************/
/******** Put a disabled icon indicating that removal is not allowed *********/
/*****************************************************************************/

void Ico_PutIconRemovalNotAllowed (void)
  {
   extern const char *Txt_Removal_not_allowed;

   Ico_PutIconOff ("trash.svg",Txt_Removal_not_allowed);
  }

/*****************************************************************************/
/************************ Put an icon to remove ******************************/
/*****************************************************************************/

void Ico_PutIconRemove (void)
  {
   extern const char *Txt_Remove;

   Ico_PutIconLink ("trash.svg",Txt_Remove);
  }

/*****************************************************************************/
/*************************** Put an icon to cut ******************************/
/*****************************************************************************/

void Ico_PutIconCut (void)
  {
   extern const char *Txt_Cut;

   Ico_PutIconLink ("cut.svg",Txt_Cut);
  }

/*****************************************************************************/
/************************** Put an icon to paste *****************************/
/*****************************************************************************/

void Ico_PutIconPaste (void)
  {
   extern const char *Txt_Paste;

   Ico_PutIconLink ("paste.svg",Txt_Paste);
  }
