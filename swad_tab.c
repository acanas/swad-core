// swad_tab.c: tabs drawing

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_action_list.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"
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

static Usr_ICan_t Tab_CheckIfICanViewTab (Tab_Tab_t Tab);
static const char *Tab_GetIcon (Tab_Tab_t Tab);

/*****************************************************************************/
/**************** Draw tabs with the current tab highlighted *****************/
/*****************************************************************************/

void Tab_DrawTabs (void)
  {
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   Tab_Tab_t NumTab;
   const char *TabTxt;
   char URLIconSet[PATH_MAX + 1];
   static const char *ClassIcoTab[Ico_NUM_ICON_SETS] =
     {
      [Ico_ICON_SET_AWESOME] = "TAB_ICO",
      [Ico_ICON_SET_NUVOLA ] = NULL,
     };

   /***** Begin tabs container *****/
   HTM_DIV_Begin ("class=\"HEAD_ROW_3_%s\"",The_GetSuffix ());
      HTM_Txt ("<nav id=\"tabs\">");
      HTM_UL_Begin ("class=\"TAB_LIST\"");

	 /***** Draw the tabs *****/
	 for (NumTab  = (Tab_Tab_t) 1;
	      NumTab <= (Tab_Tab_t) (Tab_NUM_TABS - 1);
	      NumTab++)
	   {
	    /* If current tab is unknown, then activate the first one with access allowed */
	    if (Gbl.Action.Tab == TabUnk)
	      {
	       Gbl.Action.Tab = NumTab;
	       Tab_DisableIncompatibleTabs ();
	      }

	    if (Tab_CheckIfICanViewTab (NumTab) == Usr_I_CAN)	// Don't show the first hidden tabs
	      {
	       TabTxt = Tab_GetTxt (NumTab);

	       /* Form, icon (at top) and text (at bottom) of the tab */
	       HTM_LI_Begin ("class=\"%s%s\"",
			     NumTab == Gbl.Action.Tab ? "TAB_ON TAB_ON_" :
							"TAB_OFF TAB_OFF_",
			     The_GetSuffix ());

		  if (NumTab == Gbl.Action.Tab)
		     HTM_DIV_Begin (NULL);	// This div must be present even in current tab in order to render properly the tab
		  else
		     HTM_DIV_Begin ("class=\"ICO_HIGHLIGHT\"");

		  Frm_BeginForm (ActMnu);
		     Par_PutParUnsigned (NULL,"NxtTab",(unsigned) NumTab);
		     HTM_BUTTON_Submit_Begin (TabTxt,"class=\"BT_LINK\"");
			snprintf (URLIconSet,sizeof (URLIconSet),"%s/%s",
				  Cfg_URL_ICON_SETS_PUBLIC,Ico_IconSetId[Gbl.Prefs.IconSet]);
		        if (ClassIcoTab[Gbl.Prefs.IconSet])
			   HTM_IMG (URLIconSet,Tab_GetIcon (NumTab),TabTxt,
				    "class=\"TAB_ICO %s_%s\"",
				    ClassIcoTab[Gbl.Prefs.IconSet],The_GetSuffix ());
		        else
			   HTM_IMG (URLIconSet,Tab_GetIcon (NumTab),TabTxt,
				    "class=\"TAB_ICO\"");
			HTM_DIV_Begin ("class=\"TAB_TXT TAB_%s_TXT_%s\"",
				       NumTab == Gbl.Action.Tab ? "ON" :
								  "OFF",
				       The_GetSuffix ());
			   HTM_Txt (TabTxt);
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

static Usr_ICan_t Tab_CheckIfICanViewTab (Tab_Tab_t Tab)
  {
   switch (Tab)
     {
      case TabUnk:
	 return Usr_I_CAN_NOT;
      case TabSys:
	 return (Gbl.Hierarchy.Level == Hie_SYS) ? Usr_I_CAN :	// Institution selected
						   Usr_I_CAN_NOT;
      case TabCty:
	 return (Gbl.Hierarchy.Level == Hie_CTY) ? Usr_I_CAN :	// Institution selected
						   Usr_I_CAN_NOT;
      case TabIns:
	 return (Gbl.Hierarchy.Level == Hie_INS) ? Usr_I_CAN :	// Institution selected
						   Usr_I_CAN_NOT;
      case TabCtr:
	 return (Gbl.Hierarchy.Level == Hie_CTR) ? Usr_I_CAN :	// Center selected
						   Usr_I_CAN_NOT;
      case TabDeg:
	 return (Gbl.Hierarchy.Level == Hie_DEG) ? Usr_I_CAN :	// Degree selected
						   Usr_I_CAN_NOT;
      case TabCrs:
	 return (Gbl.Hierarchy.Level == Hie_CRS) ? Usr_I_CAN :	// Course selected
						   Usr_I_CAN_NOT;
      case TabAss:
	 return (Gbl.Hierarchy.Level == Hie_CRS &&		// Course selected
	         Gbl.Usrs.Me.Role.Logged >= Rol_STD) ? Usr_I_CAN :	// I belong to course or I am an admin
						       Usr_I_CAN_NOT;
      case TabFil:
      	 return (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0 ||	// Institution selected
	         Gbl.Usrs.Me.Logged) ? Usr_I_CAN :		// I'm logged
				       Usr_I_CAN_NOT;
      default:
	 return Usr_I_CAN;
     }
  }

/*****************************************************************************/
/************************ Get icon associated to a tab ***********************/
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
/************************ Get text associated to a tab ***********************/
/*****************************************************************************/

const char *Tab_GetTxt (Tab_Tab_t NumTab)
  {
   extern const char *Txt_TABS_TXT[Tab_NUM_TABS];

   return Txt_TABS_TXT[NumTab];
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
	 if (Gbl.Hierarchy.Node[Hie_CTY].HieCod <= 0)		// No country selected
	   {
	    Gbl.Action.Act = ActSeeCty;
            Gbl.Action.Tab = Act_GetTab (Gbl.Action.Act);
	   }
	 break;
      case TabIns:
	 if (Gbl.Hierarchy.Node[Hie_INS].HieCod <= 0)		// No institution selected
	   {
	    if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)		// Country selected, but no institution selected
	       Gbl.Action.Act = ActSeeIns;
	    else						// No country selected
	       Gbl.Action.Act = ActSeeCty;
            Gbl.Action.Tab = Act_GetTab (Gbl.Action.Act);
	  }
	break;
      case TabCtr:
	 if (Gbl.Hierarchy.Node[Hie_CTR].HieCod <= 0)		// No center selected
	   {
	    if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)		// Institution selected, but no center selected
	       Gbl.Action.Act = ActSeeCtr;
	    else if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)	// Country selected, but no institution selected
	       Gbl.Action.Act = ActSeeIns;
	    else						// No country selected
	       Gbl.Action.Act = ActSeeCty;
            Gbl.Action.Tab = Act_GetTab (Gbl.Action.Act);
	   }
         break;
      case TabDeg:
         if (Gbl.Hierarchy.Node[Hie_DEG].HieCod <= 0)		// No degree selected
	   {
	    if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)		// Center selected, but no degree selected
	       Gbl.Action.Act = ActSeeDeg;
	    else if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)	// Institution selected, but no center selected
	       Gbl.Action.Act = ActSeeCtr;
	    else if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)	// Country selected, but no institution selected
	       Gbl.Action.Act = ActSeeIns;
	    else						// No country selected
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
	 Gbl.Hierarchy.Node[Hie_CTY].HieCod = -1L;
	 /* falls through */
	 /* no break */
      case TabCty:
	 Gbl.Hierarchy.Node[Hie_INS].HieCod = -1L;
	 /* falls through */
	 /* no break */
      case TabIns:
	 Gbl.Hierarchy.Node[Hie_CTR].HieCod = -1L;
	 /* falls through */
	 /* no break */
      case TabCtr:
	 Gbl.Hierarchy.Node[Hie_DEG].HieCod = -1L;
	 /* falls through */
	 /* no break */
      case TabDeg:
	 Gbl.Hierarchy.Node[Hie_CRS].HieCod = -1L;
	 break;
      default:
         break;
     }
  }
