// swad_tab.c: tabs drawing

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

#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_tab.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static bool Tab_CheckIfICanViewTab (Tab_Tab_t Tab);
static const char *Tab_GetIcon (Tab_Tab_t Tab);

/*****************************************************************************/
/**************** Draw tabs with the current tab highlighted *****************/
/*****************************************************************************/

void Tab_DrawTabs (void)
  {
   extern const char *The_Colors[The_NUM_THEMES];
   extern const char *Txt_TABS_TXT[Tab_NUM_TABS];
   extern const char *Txt_TABS_TXT[Tab_NUM_TABS];
   Tab_Tab_t NumTab;
   bool ICanViewTab;
   static const char *ClassHeadRow3[The_NUM_THEMES] =
     {
      [The_THEME_WHITE ] = "HEAD_ROW_3_WHITE",
      [The_THEME_GREY  ] = "HEAD_ROW_3_GREY",
      [The_THEME_PURPLE] = "HEAD_ROW_3_PURPLE",
      [The_THEME_BLUE  ] = "HEAD_ROW_3_BLUE",
      [The_THEME_YELLOW] = "HEAD_ROW_3_YELLOW",
      [The_THEME_PINK  ] = "HEAD_ROW_3_PINK",
      [The_THEME_DARK  ] = "HEAD_ROW_3_DARK",
      };
   static const char *ClassIcoTab[Ico_NUM_ICON_SETS][The_NUM_THEMES] =
     {
      [Ico_ICON_SET_AWESOME][The_THEME_WHITE ] = " TAB_ICO_WHITE",
      [Ico_ICON_SET_AWESOME][The_THEME_GREY  ] = " TAB_ICO_GREY",
      [Ico_ICON_SET_AWESOME][The_THEME_PURPLE] = " TAB_ICO_PURPLE",
      [Ico_ICON_SET_AWESOME][The_THEME_BLUE  ] = " TAB_ICO_BLUE",
      [Ico_ICON_SET_AWESOME][The_THEME_YELLOW] = " TAB_ICO_YELLOW",
      [Ico_ICON_SET_AWESOME][The_THEME_PINK  ] = " TAB_ICO_PINK",
      [Ico_ICON_SET_AWESOME][The_THEME_DARK  ] = " TAB_ICO_DARK",

      [Ico_ICON_SET_NUVOLA ][The_THEME_WHITE ] = "",
      [Ico_ICON_SET_NUVOLA ][The_THEME_GREY  ] = "",
      [Ico_ICON_SET_NUVOLA ][The_THEME_PURPLE] = "",
      [Ico_ICON_SET_NUVOLA ][The_THEME_BLUE  ] = "",
      [Ico_ICON_SET_NUVOLA ][The_THEME_YELLOW] = "",
      [Ico_ICON_SET_NUVOLA ][The_THEME_PINK  ] = "",
      [Ico_ICON_SET_NUVOLA ][The_THEME_DARK  ] = "",
     };

   /***** Begin tabs container *****/
   HTM_DIV_Begin ("class=\"%s\"",ClassHeadRow3[Gbl.Prefs.Theme]);
      HTM_Txt ("<nav id=\"tabs\">");
      HTM_UL_Begin ("class=\"TAB_LIST\"");

	 /***** Draw the tabs *****/
	 for (NumTab  = (Tab_Tab_t) 1;
	      NumTab <= (Tab_Tab_t) (Tab_NUM_TABS - 1);
	      NumTab++)
	   {
	    ICanViewTab = Tab_CheckIfICanViewTab (NumTab);

	    /* If current tab is unknown, then activate the first one with access allowed */
	    if (Gbl.Action.Tab == TabUnk)
	      {
	       Gbl.Action.Tab = NumTab;
	       Tab_DisableIncompatibleTabs ();
	      }

	    if (ICanViewTab)	// Don't show the first hidden tabs
	      {
	       /* Form, icon (at top) and text (at bottom) of the tab */
	       HTM_LI_Begin ("class=\"%s%s\"",
			     NumTab == Gbl.Action.Tab ? "TAB_ON TAB_ON_" :
							"TAB_OFF TAB_OFF_",
			     The_Colors[Gbl.Prefs.Theme]);

		  if (NumTab == Gbl.Action.Tab)
		     HTM_DIV_Begin (NULL);	// This div must be present even in current tab in order to render properly the tab
		  else
		     HTM_DIV_Begin ("class=\"ICO_HIGHLIGHT\"");

		  Frm_BeginForm (ActMnu);
		     Par_PutHiddenParamUnsigned (NULL,"NxtTab",(unsigned) NumTab);
		     HTM_BUTTON_OnSubmit_Begin (Txt_TABS_TXT[NumTab],
		                                "BT_LINK",NULL);
			HTM_IMG (Gbl.Prefs.URLIconSet,Tab_GetIcon (NumTab),Txt_TABS_TXT[NumTab],
				 "class=\"TAB_ICO%s\"",ClassIcoTab[Gbl.Prefs.IconSet][Gbl.Prefs.Theme]);
			HTM_DIV_Begin ("class=\"TAB_TXT TAB_%s_TXT_%s\"",
				       NumTab == Gbl.Action.Tab ? "ON" :
								  "OFF",
				       The_Colors[Gbl.Prefs.Theme]);
			   HTM_Txt (Txt_TABS_TXT[NumTab]);
			HTM_DIV_End ();
		     HTM_BUTTON_End ();
		  Frm_EndForm ();

		  HTM_DIV_End ();
	       HTM_LI_End ();
	      }
	   }

      /***** End tabs container *****/
      HTM_UL_End ();
      HTM_Txt ("</nav>");
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************* Check if I can view a tab *************************/
/*****************************************************************************/

static bool Tab_CheckIfICanViewTab (Tab_Tab_t Tab)
  {
   switch (Tab)
     {
      case TabUnk:
	 return false;
      case TabSys:
	 return (Gbl.Hierarchy.Cty.CtyCod <= 0);	// No country selected
      case TabCty:
	 return (Gbl.Hierarchy.Cty.CtyCod > 0 &&	// Country selected
	         Gbl.Hierarchy.Ins.InsCod <= 0);	// No institution selected
      case TabIns:
	 return (Gbl.Hierarchy.Ins.InsCod > 0 &&	// Institution selected
	         Gbl.Hierarchy.Ctr.CtrCod <= 0);	// No center selected
      case TabCtr:
	 return (Gbl.Hierarchy.Ctr.CtrCod > 0 &&	// Center selected
	         Gbl.Hierarchy.Deg.DegCod <= 0);	// No degree selected
      case TabDeg:
	 return (Gbl.Hierarchy.Deg.DegCod > 0 &&	// Degree selected
	         Gbl.Hierarchy.Crs.CrsCod <= 0);	// No course selected
      case TabCrs:
	 return (Gbl.Hierarchy.Level == HieLvl_CRS);	// Course selected
      case TabAss:
	 return (Gbl.Hierarchy.Level == HieLvl_CRS ||	// Course selected
	         Gbl.Usrs.Me.Role.Logged >= Rol_STD);	// I belong to course or I am an admin
      case TabFil:
      	 return (Gbl.Hierarchy.Ins.InsCod > 0 ||	// Institution selected
	         Gbl.Usrs.Me.Logged);			// I'm logged
      default:
	 return true;
     }
  }

/*****************************************************************************/
/********************** Get icon associated to an action *********************/
/*****************************************************************************/

static const char *Tab_GetIcon (Tab_Tab_t NumTab)
  {
   static const char *Tab_TabIcons[Tab_NUM_TABS] =
     {
      [TabUnk] = NULL,
      [TabStr] = "house-user",
      [TabSys] = "sitemap",
      [TabCty] = "globe-americas",
      [TabIns] = "university",
      [TabCtr] = "building",
      [TabDeg] = "graduation-cap",
      [TabCrs] = "chalkboard-teacher",
      [TabAss] = "check",
      [TabFil] = "folder-open",
      [TabUsr] = "users",
      [TabMsg] = "comment",
      [TabAna] = "chart-bar",
      [TabPrf] = "user",
     };

   if (NumTab < (Tab_Tab_t) 1 ||
       NumTab > (Tab_Tab_t) (Tab_NUM_TABS - 1))
      return NULL;

   return Ico_GetIcon (Tab_TabIcons[NumTab]);
  }

/*****************************************************************************/
/*************** Set current tab depending on current action *****************/
/*****************************************************************************/

void Tab_SetCurrentTab (void)
  {
   Gbl.Action.Tab = Act_GetTab (Gbl.Action.Act);

   /***** Change action and tab if country, institution, center or degree
          are incompatible with the current tab *****/
   switch (Gbl.Action.Tab)
     {
      case TabCty:
	 if (Gbl.Hierarchy.Cty.CtyCod <= 0)		// No country selected
	   {
	    Gbl.Action.Act = ActSeeCty;
            Gbl.Action.Tab = Act_GetTab (Gbl.Action.Act);
	   }
	 break;
      case TabIns:
	 if (Gbl.Hierarchy.Ins.InsCod <= 0)		// No institution selected
	   {
	    if (Gbl.Hierarchy.Cty.CtyCod > 0)		// Country selected, but no institution selected
	       Gbl.Action.Act = ActSeeIns;
	    else					// No country selected
	       Gbl.Action.Act = ActSeeCty;
            Gbl.Action.Tab = Act_GetTab (Gbl.Action.Act);
	  }
	break;
      case TabCtr:
	 if (Gbl.Hierarchy.Ctr.CtrCod <= 0)		// No center selected
	   {
	    if (Gbl.Hierarchy.Ins.InsCod > 0)		// Institution selected, but no center selected
	       Gbl.Action.Act = ActSeeCtr;
	    else if (Gbl.Hierarchy.Cty.CtyCod > 0)	// Country selected, but no institution selected
	       Gbl.Action.Act = ActSeeIns;
	    else					// No country selected
	       Gbl.Action.Act = ActSeeCty;
            Gbl.Action.Tab = Act_GetTab (Gbl.Action.Act);
	   }
         break;
      case TabDeg:
         if (Gbl.Hierarchy.Deg.DegCod <= 0)		// No degree selected
	   {
	    if (Gbl.Hierarchy.Ctr.CtrCod > 0)		// Center selected, but no degree selected
	       Gbl.Action.Act = ActSeeDeg;
	    else if (Gbl.Hierarchy.Ins.InsCod > 0)	// Institution selected, but no center selected
	       Gbl.Action.Act = ActSeeCtr;
	    else if (Gbl.Hierarchy.Cty.CtyCod > 0)	// Country selected, but no institution selected
	       Gbl.Action.Act = ActSeeIns;
	    else					// No country selected
	       Gbl.Action.Act = ActSeeCty;
            Gbl.Action.Tab = Act_GetTab (Gbl.Action.Act);
	   }
         break;
      default:
         break;
     }

   Tab_DisableIncompatibleTabs ();
  }

/*****************************************************************************/
/************************** Disable incompatible tabs ************************/
/*****************************************************************************/

void Tab_DisableIncompatibleTabs (void)
  {
   /***** Set country, institution, center, degree and course depending on the current tab.
          This will disable tabs incompatible with the current one. *****/
   switch (Gbl.Action.Tab)
     {
      case TabSys:
	 Gbl.Hierarchy.Cty.CtyCod = -1L;
	 /* falls through */
	 /* no break */
      case TabCty:
	 Gbl.Hierarchy.Ins.InsCod = -1L;
	 /* falls through */
	 /* no break */
      case TabIns:
	 Gbl.Hierarchy.Ctr.CtrCod = -1L;
	 /* falls through */
	 /* no break */
      case TabCtr:
	 Gbl.Hierarchy.Deg.DegCod = -1L;
	 /* falls through */
	 /* no break */
      case TabDeg:
	 Gbl.Hierarchy.Crs.CrsCod = -1L;
	 break;
      default:
         break;
     }
  }
