// swad_icon.c: icons

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
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hidden_visible.h"
#include "swad_HTML.h"
#include "swad_icon.h"
#include "swad_layout.h"
#include "swad_parameter.h"
#include "swad_resource.h"
#include "swad_setting.h"
#include "swad_setting_database.h"
#include "swad_user_database.h"

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

struct Ico_IconColor Ico_HiddenVisible[HidVis_NUM_HIDDEN_VISIBLE] =
  {
   [HidVis_HIDDEN ] =
     {
      .Icon  = "eye-slash.svg",
      .Color = Ico_RED
     },
   [HidVis_VISIBLE] =
     {
      .Icon = "eye.svg",
      .Color = Ico_GREEN
     }
  };

struct Ico_IconColor Ico_PrivatePublic[PriPub_NUM_PRIVATE_PUBLIC] =
  {
   [PriPub_PRIVATE] =
     {
      .Icon  = "lock.svg",
      .Color = Ico_RED
     },
   [PriPub_PUBLIC ] =
     {
      .Icon = "unlock.svg",
      .Color = Ico_GREEN
     }
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ico_PutIconsIconSet (__attribute__((unused)) void *Args);

/*****************************************************************************/
/******************* Get color name for CSS class preffix ********************/
/*****************************************************************************/

const char *Ico_GetPreffix (Ico_Color_t Color)
  {
   static const char *Ico_CSS_Preffix[Ico_NUM_COLORS] =
     {
      [Ico_UNCHANGED] = NULL,
      [Ico_BLACK    ] = "BLACK",
      [Ico_BLUE     ] = "BLUE",
      [Ico_GREEN    ] = "GREEN",
      [Ico_RED      ] = "RED",
      [Ico_WHITE    ] = "WHITE",
      [Ico_YELLOW   ] = "YELLOW",
     };

   return Ico_CSS_Preffix[Color];
  }

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
   extern const char *Hlp_PROFILE_Settings_icons;
   extern const char *Txt_Icons;
   Ico_IconSet_t IconSet;
   char Icon[PATH_MAX + 1];

   Box_BoxBegin (Txt_Icons,Ico_PutIconsIconSet,NULL,
                 Hlp_PROFILE_Settings_icons,Box_NOT_CLOSABLE);
      Set_BeginSettingsHead ();
	 Set_BeginOneSettingSelector ();
	    for (IconSet  = (Ico_IconSet_t) 0;
		 IconSet <= (Ico_IconSet_t) (Ico_NUM_ICON_SETS - 1);
		 IconSet++)
	      {
	       Set_BeginPref (IconSet == Gbl.Prefs.IconSet);
		  Frm_BeginForm (ActChgIco);
		     Par_PutParString (NULL,"IconSet",Ico_IconSetId[IconSet]);
		     snprintf (Icon,sizeof (Icon),"%s/%s/cog.svg",
			       Cfg_ICON_FOLDER_SETS,
			       Ico_IconSetId[IconSet]);
		     Ico_PutSettingIconLink (Icon,Ico_UNCHANGED,Ico_IconSetNames[IconSet]);
		  Frm_EndForm ();
	       Set_EndPref ();
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
   Gbl.Prefs.IconSet = Ico_GetParIconSet ();

   /***** Store icon set in database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsAboutIconSet (Ico_IconSetId[Gbl.Prefs.IconSet]);

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/*********************** Get parameter with icon set *************************/
/*****************************************************************************/

Ico_IconSet_t Ico_GetParIconSet (void)
  {
   char IconSetId[Ico_MAX_BYTES_ICON_SET_ID + 1];
   Ico_IconSet_t IconSet;

   Par_GetParText ("IconSet",IconSetId,Ico_MAX_BYTES_ICON_SET_ID);
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
				 void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncPars,Args,
				  "plus.svg",Ico_GREEN);
  }

void Ico_PutContextualIconToRemove (Act_Action_t NextAction,const char *Anchor,
                                    void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncPars,Args,
				  "trash.svg",Ico_RED);
  }

void Ico_PutContextualIconToReset (Act_Action_t NextAction,const char *Anchor,
                                   void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncPars,Args,
				  "recycle.svg",Ico_RED);
  }

void Ico_PutContextualIconToUpdate (Act_Action_t NextAction,const char *Anchor,
                                    void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncPars,Args,
				  "recycle.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToEdit (Act_Action_t NextAction,const char *Anchor,
				  void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncPars,Args,
				  "pen.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToViewFiles (Act_Action_t NextAction,
                                       void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncPars,Args,
				  "folder-open.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToView (Act_Action_t NextAction,const char *Anchor,
                                  void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncPars,Args,
				  "list.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToConfigure (Act_Action_t NextAction,const char *Anchor,
                                       void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncPars,Args,
				  "cog.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToHideUnhide (Act_Action_t NextAction[HidVis_NUM_HIDDEN_VISIBLE],
                                        const char *Anchor,
                                        void (*FuncPars) (void *Args),void *Args,
                                        HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction[HiddenOrVisible],Anchor,
                                  FuncPars,Args,
				  Ico_HiddenVisible[HiddenOrVisible].Icon,
				  Ico_HiddenVisible[HiddenOrVisible].Color);
  }

void Ico_PutContextualIconToPrivatePublic (Act_Action_t NextAction[PriPub_NUM_PRIVATE_PUBLIC],
                                           const char *Anchor,
                                           void (*FuncPars) (void *Args),void *Args,
                                           PriPub_PrivateOrPublic_t PrivateOrPublic)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction[PrivateOrPublic],Anchor,
                                  FuncPars,Args,
				  Ico_PrivatePublic[PrivateOrPublic].Icon,
				  Ico_PrivatePublic[PrivateOrPublic].Color);
  }

void Ico_PutContextualIconToExpand (Act_Action_t NextAction,const char *Anchor,
                                    void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncPars,Args,
				  "caret-right.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToContract (Act_Action_t NextAction,const char *Anchor,
                                      void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncPars,Args,
				  "caret-down.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToPrint (Act_Action_t NextAction,
                                   void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncPars,Args,
				  "print.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToGetLink (Act_Action_t NextAction,const char *Anchor,
                                     void (*FuncPars) (void *Args),void *Args)
  {
   if (Rsc_CheckIfICanGetLink () == Usr_CAN)
      Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncPars,Args,
				     "link.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToCopy (Act_Action_t NextAction,
                                  void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncPars,Args,
				  "copy.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToViewClipboard (Act_Action_t NextAction,const char *Anchor,
                                           void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncPars,Args,
				  "clipboard.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToPaste (Act_Action_t NextAction,
                                   void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncPars,Args,
				  "paste.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToCreateInFolder (Act_Action_t NextAction,
                                            void (*FuncPars) (void *Args),void *Args,
                                            CloOpe_ClosedOrOpen_t ClosedOrOpen)
  {
   static const char *CloOpe_Icon[CloOpe_NUM_CLOSED_OPEN] =
     {
      [CloOpe_CLOSED] = "folder-closed-yellow-plus.png",
      [CloOpe_OPEN  ] = "folder-open-yellow-plus.png",
     };

   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncPars,Args,
				  CloOpe_Icon[ClosedOrOpen],Ico_UNCHANGED);
  }

void Ico_PutContextualIconToShowResults (Act_Action_t NextAction,const char *Anchor,
                                         void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,Anchor,FuncPars,Args,
				  "trophy.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToShowAttendanceList (Act_Action_t NextAction,
                                                void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncPars,Args,
				  "tasks.svg",Ico_BLACK);
  }

void Ico_PutContextualIconToZIP (Act_Action_t NextAction,
                                 void (*FuncPars) (void *Args),void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (NextAction,NULL,FuncPars,Args,
				  "download.svg",Ico_BLACK);
  }

/*****************************************************************************/
/**************** Show an icon inside a div (without text) *******************/
/*****************************************************************************/

void Ico_PutDivIcon (const char *DivClass,
                     const char *Icon,Ico_Color_t Color,const char *Title)
  {
   HTM_DIV_Begin ("class=\"%s\"",DivClass);
      Ico_PutIcon (Icon,Color,Title,"CONTEXT_ICO16x16");
   HTM_DIV_End ();
  }

/*****************************************************************************/
/****************** Show an icon with a link (without text) ******************/
/*****************************************************************************/

void Ico_PutIconLink (const char *Icon,Ico_Color_t Color,Act_Action_t NextAction)
  {
   if (Color == Ico_UNCHANGED)
      HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,Icon,Act_GetActionText (NextAction),
		       "class=\"CONTEXT_OPT ICO_HIGHLIGHT CONTEXT_ICO16x16\"");
   else
      HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,Icon,Act_GetActionText (NextAction),
		       "class=\"CONTEXT_OPT ICO_HIGHLIGHT CONTEXT_ICO16x16 ICO_%s_%s\"",
		       Ico_GetPreffix (Color),The_GetSuffix ());
  }

/*****************************************************************************/
/******************** Show an icon and a text to update **********************/
/*****************************************************************************/

void Ico_PutIconTextUpdate (const char *Text)
  {
   Ico_PutIconTextLink ("recycle.svg",Ico_BLACK,Text);
  }

/*****************************************************************************/
/******************* Show an icon with a link (with text) ********************/
/*****************************************************************************/

void Ico_PutIconTextLink (const char *Icon,Ico_Color_t Color,const char *Text)
  {
   /***** Print icon and optional text *****/
   HTM_DIV_Begin ("class=\"CONTEXT_OPT ICO_HIGHLIGHT\"");
      Ico_PutIcon (Icon,Color,Text,"CONTEXT_ICOx16");
      HTM_NBSP ();
      HTM_Txt (Text);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************************** Show a setting selector *************************/
/*****************************************************************************/

void Ico_PutSettingIconLink (const char *Icon,Ico_Color_t Color,const char *Title)
  {
   if (Color == Ico_UNCHANGED)
      HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,Icon,Title,
		       "class=\"ICO_HIGHLIGHT ICOx20\"");
   else
      HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,Icon,Title,
		       "class=\"ICO_HIGHLIGHT ICOx20 ICO_%s_%s\"",
		       Ico_GetPreffix (Color),The_GetSuffix ());
  }

/*****************************************************************************/
/********************* Put an active or disabled icon ************************/
/*****************************************************************************/

void Ico_PutIconOn (const char *Icon,Ico_Color_t Color,const char *Title)
  {
   Ico_PutIcon (Icon,Color,Title,"CONTEXT_OPT CONTEXT_ICO16x16");
  }

void Ico_PutIconOff (const char *Icon,Ico_Color_t Color,const char *Title)
  {
   Ico_PutIcon (Icon,Color,Title,"CONTEXT_OPT CONTEXT_ICO16x16 ICO_HIDDEN");
  }

/*****************************************************************************/
/******************************* Put an icon *********************************/
/*****************************************************************************/

void Ico_PutIcon (const char *Icon,Ico_Color_t Color,const char *Title,const char *Class)
  {
   if (Color == Ico_UNCHANGED)
      HTM_IMG (Cfg_URL_ICON_PUBLIC,Icon,Title,
	       "class=\"%s\"",
	       Class);
   else
      HTM_IMG (Cfg_URL_ICON_PUBLIC,Icon,Title,
	       "class=\"%s ICO_%s_%s\"",
	       Class,Ico_GetPreffix (Color),The_GetSuffix ());
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

void Ico_PutIconCut (Act_Action_t NextAction)
  {
   Ico_PutIconLink ("cut.svg",Ico_BLACK,NextAction);
  }

/*****************************************************************************/
/************************** Put an icon to paste *****************************/
/*****************************************************************************/

void Ico_PutIconPaste (Act_Action_t NextAction)
  {
   Ico_PutIconLink ("paste.svg",Ico_BLACK,NextAction);
  }

/*****************************************************************************/
/************* Put icon indicating that a content is not visible *************/
/*****************************************************************************/

void Ico_PutIconNotVisible (void)
  {
   extern const char *Txt_Not_visible;

   Ico_PutIconOff ("eye-slash.svg",Ico_RED,Txt_Not_visible);
  }

/*****************************************************************************/
/********* Get and show number of users who have chosen an icon set **********/
/*****************************************************************************/

void Ico_GetAndShowNumUsrsPerIconSet (Hie_Level_t HieLvl)
  {
   extern const char *Hlp_ANALYTICS_Figures_icons;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Icons;
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   static Ico_Color_t Color[Ico_NUM_ICON_SETS] =
     {
      [Ico_ICON_SET_AWESOME] = Ico_BLACK,
      [Ico_ICON_SET_NUVOLA ] = Ico_UNCHANGED,
     };
   Ico_IconSet_t IconSet;
   char *SubQuery;
   char *URL;
   unsigned NumUsrs[Ico_NUM_ICON_SETS];
   unsigned NumUsrsTotal = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_ICON_SETS],NULL,NULL,
                      Hlp_ANALYTICS_Figures_icons,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Icons           ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_Number_of_users ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** For each icon set... *****/
      for (IconSet  = (Ico_IconSet_t) 0;
	   IconSet <= (Ico_IconSet_t) (Ico_NUM_ICON_SETS - 1);
	   IconSet++)
	{
	 /* Get the number of users who have chosen this icon set from database */
	 if (asprintf (&SubQuery,"usr_data.IconSet='%s'",Ico_IconSetId[IconSet]) < 0)
	    Err_NotEnoughMemoryExit ();
	 NumUsrs[IconSet] = Usr_DB_GetNumUsrsWhoChoseAnOption (HieLvl,SubQuery);
	 free (SubQuery);

	 /* Update total number of users */
	 NumUsrsTotal += NumUsrs[IconSet];
	}

      /***** Write number of users who have chosen each icon set *****/
      for (IconSet  = (Ico_IconSet_t) 0;
	   IconSet <= (Ico_IconSet_t) (Ico_NUM_ICON_SETS - 1);
	   IconSet++)
	{
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"LM\"");
	       if (asprintf (&URL,"%s/%s",
			     Cfg_URL_ICON_SETS_PUBLIC,Ico_IconSetId[IconSet]) < 0)
		  Err_NotEnoughMemoryExit ();
	       if (Color[IconSet] == Ico_UNCHANGED)
		  HTM_IMG (URL,"cog.svg",Ico_IconSetNames[IconSet],
			   "class=\"ICO20x20\"");
	       else
		  HTM_IMG (URL,"cog.svg",Ico_IconSetNames[IconSet],
			   "class=\"ICO20x20 ICO_%s_%s\"",
			   Ico_GetPreffix (Color[IconSet]),The_GetSuffix ());
	       free (URL);
	    HTM_TD_End ();

	    HTM_TD_Unsigned (NumUsrs[IconSet]);
	    HTM_TD_Percentage (NumUsrs[IconSet],NumUsrsTotal);

	 HTM_TR_End ();
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
