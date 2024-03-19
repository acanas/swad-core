// swad_menu.c: menu (horizontal or vertical) selection

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_layout.h"
#include "swad_menu.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_setting_database.h"
#include "swad_tab.h"
#include "swad_user_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Public constants ****************************/
/*****************************************************************************/

const char *Mnu_MenuIcons[Mnu_NUM_MENUS] =
  {
   [Mnu_MENU_HORIZONTAL] = "ellipsis-h.svg",
   [Mnu_MENU_VERTICAL  ] = "ellipsis-v.svg",
  };

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

#define MAX_MENU_ID 16

// Actions not initialized are 0 by default
static const struct Mnu_Menu
  {
   Act_Action_t Action;
   const char *Icon;
  } Mnu_Menu[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB] =
  {
   [TabUnk] =  {
	       },
   [TabStr] =  {
		[ 0] = {ActFrmLogIn		,"power-off"		},
		[ 1] = {ActReqSch		,"search"		},
		[ 2] = {ActSeeGblTL		,"comment-dots"		},
		[ 3] = {ActSeeSocPrf		,"user-circle"		},
		[ 4] = {ActSeeCal		,"calendar"		},
		[ 5] = {ActSeeNtf		,"bell"			},
	       },
   [TabSys] =  {
		[ 0] = {ActSeeSysInf		,"info"			},
		[ 1] = {ActSeeCty		,"globe-americas"	},
		[ 2] = {ActSeePen		,"sitemap"		},
		[ 3] = {ActSeeLnk		,"up-right-from-square"	},
		[ 4] = {ActSeePlg		,"puzzle-piece"		},
		[ 5] = {ActMtn			,"tools"		},
	       },
   [TabCty] =  {
		[ 0] = {ActSeeCtyInf		,"info"			},
		[ 1] = {ActSeeIns		,"university"		},
	       },
   [TabIns] =  {
		[ 0] = {ActSeeInsInf		,"info"			},
		[ 1] = {ActSeeCtr		,"building"		},
		[ 2] = {ActSeePlc		,"map-marker-alt"	},
		[ 3] = {ActSeeDpt		,"users"		},
		[ 4] = {ActSeeHld		,"calendar-day"		},
	       },
   [TabCtr] =  {
		[ 0] = {ActSeeCtrInf		,"info"			},
		[ 1] = {ActSeeDeg		,"graduation-cap"	},
		[ 2] = {ActSeeBld		,"building"		},
		[ 3] = {ActSeeRoo		,"chalkboard-teacher"	},
	       },
   [TabDeg] =  {
		[ 0] = {ActSeeDegInf		,"info"			},
		[ 1] = {ActSeeCrs		,"chalkboard-teacher"	},
	       },
   [TabCrs] =  {
		[ 0] = {ActSeeCrsInf		,"info"			},
		[ 1] = {ActSeePrg		,"clipboard-list"	},
		[ 2] = {ActSeeTchGui		,"book-open"		},
		[ 3] = {ActSeeSyl		,"list-ol"		},
		[ 4] = {ActSeeBib		,"book"			},
		[ 5] = {ActSeeFAQ		,"question"		},
		[ 6] = {ActSeeCrsLnk		,"up-right-from-square"	},
		[ 7] = {ActSeeAss		,"check"		},
		[ 8] = {ActSeeCrsTT		,"clock"		},
	       },
   [TabAss] =  {
		[ 0] = {ActSeeAllAsg		,"edit"			},
		[ 1] = {ActSeeAllPrj		,"file-invoice"		},
		[ 2] = {ActSeeAllCfe		,"bullhorn"		},
		[ 3] = {ActEdiTstQst		,"clipboard-question"	},
		[ 4] = {ActReqTst		,"check"		},
		[ 5] = {ActSeeAllExa		,"file-signature"	},
		[ 6] = {ActSeeAllGam		,"gamepad"		},
		[ 7] = {ActSeeAllRub		,"tasks"		},
	       },
   [TabFil] =  {
		[ 0] = {ActSeeAdmDocIns		,"folder-open"		},
		[ 1] = {ActAdmShaIns		,"folder-open"		},
		[ 2] = {ActSeeAdmDocCtr		,"folder-open"		},
		[ 3] = {ActAdmShaCtr		,"folder-open"		},
		[ 4] = {ActSeeAdmDocDeg		,"folder-open"		},
		[ 5] = {ActAdmShaDeg		,"folder-open"		},
		[ 6] = {ActSeeAdmDocCrsGrp	,"folder-open"		},
		[ 7] = {ActAdmTchCrsGrp		,"folder-open"		},
		[ 8] = {ActAdmShaCrsGrp		,"folder-open"		},
		[ 9] = {ActAdmAsgWrkUsr		,"folder-open"		},
		[10] = {ActReqAsgWrkCrs		,"folder-open"		},
		[11] = {ActSeeAdmMrk		,"list-alt"		},
		[12] = {ActAdmBrf		,"briefcase"		},
	       },
   [TabUsr] =  {
		[ 0] = {ActReqSelGrp		,"sitemap"		},
		[ 1] = {ActLstStd		,"users"		},
		[ 2] = {ActLstTch		,"user-tie"		},
		[ 3] = {ActLstOth		,"user-friends"		},
		[ 4] = {ActSeeAllAtt		,"calendar-check"	},
		[ 5] = {ActReqSignUp		,"hand-point-up"	},
		[ 6] = {ActSeeSignUpReq		,"hand-point-up"	},
		[ 7] = {ActLstCon		,"user-clock"		},
		},
   [TabMsg] =  {
		[ 0] = {ActSeeAnn		,"bullhorn"		},
		[ 1] = {ActSeeAllNot		,"sticky-note"		},
		[ 2] = {ActSeeFor		,"comments"		},
		[ 3] = {ActSeeChtRms		,"comments"		},
		[ 4] = {ActSeeRcvMsg		,"envelope"		},
		[ 5] = {ActReqMaiUsr		,"at"			},
	       },
   [TabAna] =  {
		[ 0] = {ActReqUseGbl		,"chart-pie"		},
		[ 1] = {ActSeePhoDeg		,"graduation-cap"	},
		[ 2] = {ActReqStaCrs		,"tasks"		},
		[ 3] = {ActSeeAllSvy		,"poll"			},
		[ 4] = {ActReqAccGbl		,"chart-line"		},
		[ 5] = {ActReqMyUsgRep		,"file-alt"		},
		[ 6] = {ActMFUAct		,"heart"		},
	       },
   [TabPrf] =  {
		[ 0] = {ActFrmRolSes		,"power-off"		},
		[ 1] = {ActMyCrs		,"sitemap"		},
		[ 2] = {ActSeeMyTT		,"clock"		},
		[ 3] = {ActSeeMyAgd		,"calendar"		},
		[ 4] = {ActFrmMyAcc		,"at"			},
		[ 5] = {ActReqEdiRecSha		,"address-card"		},
		[ 6] = {ActReqEdiSet		,"cog"			},
	       },
  };

  /*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

static void Mnu_PutIconsMenu (__attribute__((unused)) void *Args);

/*****************************************************************************/
/***** Get icon associated to a number of option in menu of current tab ******/
/*****************************************************************************/

const char *Mnu_GetIconFromNumOptInMenu (unsigned NumOptInMenu)
  {
   return Mnu_Menu[Gbl.Action.Tab][NumOptInMenu].Icon;
  }

/*****************************************************************************/
/********************** Get icon associated to an action *********************/
/*****************************************************************************/

const char *Mnu_GetIconFromAction (Act_Action_t Action)
  {
   return Mnu_Menu[Act_GetTab (Action)][Act_GetIndexInMenu (Action)].Icon;
  }

/*****************************************************************************/
/******* When I change to another tab, go to the first option allowed ********/
/*****************************************************************************/

Act_Action_t Mnu_GetFirstActionAvailableInCurrentTab (void)
  {
   unsigned NumOptInMenu;
   Act_Action_t Action;

   /* Change current action to the first allowed action in current tab */
   for (NumOptInMenu = 0;
        NumOptInMenu < Act_MAX_OPTIONS_IN_MENU_PER_TAB;
        NumOptInMenu++)
     {
      if ((Action = Mnu_Menu[Gbl.Action.Tab][NumOptInMenu].Action) == 0)
         return ActUnk;
      if (Act_CheckIfIHavePermissionToExecuteAction (Action))
         return Action;
     }
   return ActUnk;
  }

/*****************************************************************************/
/******************* Write horizontal menu of current tab ********************/
/*****************************************************************************/

void Mnu_WriteMenuThisTab (void)
  {
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   static const char *ClassIcoMenu[Ico_NUM_ICON_SETS] =
     {
      [Ico_ICON_SET_AWESOME] = "MENU_ICO",
      [Ico_ICON_SET_NUVOLA ] = NULL,
     };
   unsigned NumOptInMenu;
   Act_Action_t Action;
   const char *Title;
   bool IsTheSelectedAction;

   /***** Begin container *****/
   HTM_Txt ("<nav class=\"MENU_LIST_CONT\">");

      /***** Begin option list *****/
      HTM_UL_Begin ("class=\"MENU_LIST\"");

	 /***** Loop to write all options in menu. Each row holds an option *****/
	 for (NumOptInMenu = 0;
	      NumOptInMenu < Act_MAX_OPTIONS_IN_MENU_PER_TAB;
	      NumOptInMenu++)
	   {
	    Action = Mnu_Menu[Gbl.Action.Tab][NumOptInMenu].Action;
	    if (Action == 0)  // At the end of each tab, actions are initialized to 0, so 0 marks the end of the menu
	       break;
	    if (Act_CheckIfIHavePermissionToExecuteAction (Action))
	      {
	       IsTheSelectedAction = (Action == Act_GetSuperAction (Gbl.Action.Act));

	       Title = Act_GetActionText (Action);

	       /***** Begin option *****/
	       HTM_LI_Begin ("class=\"MENU_LIST_ITEM %s\"",
	                     IsTheSelectedAction ? "MENU_OPT_ON" :
						   "MENU_OPT_OFF");

		  /***** Begin form *****/
		  Frm_BeginForm (Action);

		     /***** Begin link *****/
		     HTM_BUTTON_Submit_Begin (Title,"class=\"BT_LINK\"");

			/***** Icon and text *****/
			HTM_DIV_Begin ("class=\"MENU_ICO_TXT\"");
			   if (ClassIcoMenu[Gbl.Prefs.IconSet])
			      HTM_DIV_Begin ("class=\"MENU_ICO %s_%s\""
					     " style=\"background-image:url('%s/%s/%s');\"",
					     ClassIcoMenu[Gbl.Prefs.IconSet],The_GetSuffix (),
					     Cfg_URL_ICON_SETS_PUBLIC,Ico_IconSetId[Gbl.Prefs.IconSet],
					     Ico_GetIcon (Mnu_GetIconFromNumOptInMenu (NumOptInMenu)));
			   else
			      HTM_DIV_Begin ("class=\"MENU_ICO\""
					     " style=\"background-image:url('%s/%s/%s');\"",
					     Cfg_URL_ICON_SETS_PUBLIC,Ico_IconSetId[Gbl.Prefs.IconSet],
					     Ico_GetIcon (Mnu_GetIconFromNumOptInMenu (NumOptInMenu)));
			   HTM_DIV_End ();
			   HTM_DIV_Begin ("class=\"MENU_TXT MENU_TXT_%s\"",
			                  The_GetSuffix ());
			      HTM_Txt (Act_GetTitleAction (Action));
			   HTM_DIV_End ();
			HTM_DIV_End ();

		     /***** End link *****/
		     HTM_BUTTON_End ();

		  /***** End form *****/
		  Frm_EndForm ();

	       /***** End option *****/
	       HTM_LI_End ();
	      }
	   }

      /***** End option list *****/
      HTM_UL_End ();

   /***** End container *****/
   HTM_Txt ("</nav>");
  }

/*****************************************************************************/
/************* Put icons to select menu (horizontal or vertical) *************/
/*****************************************************************************/

void Mnu_PutIconsToSelectMenu (void)
  {
   extern const char *Hlp_PROFILE_Settings_menu;
   extern const char *Txt_Menu;
   extern const char *Txt_MENU_NAMES[Mnu_NUM_MENUS];
   Mnu_Menu_t Menu;

   Box_BoxBegin (Txt_Menu,Mnu_PutIconsMenu,NULL,
                 Hlp_PROFILE_Settings_menu,Box_NOT_CLOSABLE);

      Set_BeginSettingsHead ();
	 Set_BeginOneSettingSelector ();
	    for (Menu  = (Mnu_Menu_t) 0;
		 Menu <= (Mnu_Menu_t) (Mnu_NUM_MENUS - 1);
		 Menu++)
	      {
	       Set_BeginPref (Menu == Gbl.Prefs.Menu);
		  Frm_BeginForm (ActChgMnu);
		     Par_PutParUnsigned (NULL,"Menu",(unsigned) Menu);
		     Ico_PutSettingIconLink (Mnu_MenuIcons[Menu],Ico_BLACK,
					     Txt_MENU_NAMES[Menu]);
		  Frm_EndForm ();
	       Set_EndPref ();
	      }
	 Set_EndOneSettingSelector ();
      Set_EndSettingsHead ();

   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************* Put contextual icons in menu setting ********************/
/*****************************************************************************/

static void Mnu_PutIconsMenu (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_MENUS);
  }

/*****************************************************************************/
/******************************** Change menu ********************************/
/*****************************************************************************/

void Mnu_ChangeMenu (void)
  {
   /***** Get param with menu *****/
   Gbl.Prefs.Menu = Mnu_GetParMenu ();

   /***** Store menu in database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsAboutMenu (Gbl.Prefs.Menu);

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/************************* Get parameter with menu ***************************/
/*****************************************************************************/

Mnu_Menu_t Mnu_GetParMenu (void)
  {
   return (Mnu_Menu_t)
	  Par_GetParUnsignedLong ("Menu",
	                          0,
	                          Mnu_NUM_MENUS - 1,
	                          (unsigned long) Mnu_MENU_DEFAULT);
  }

/*****************************************************************************/
/*************************** Get menu from string ****************************/
/*****************************************************************************/

Mnu_Menu_t Mnu_GetMenuFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Mnu_NUM_MENUS)
         return (Mnu_Menu_t) UnsignedNum;

   return Mnu_MENU_DEFAULT;
  }

/*****************************************************************************/
/******************************** Context menu *******************************/
/*****************************************************************************/

void Mnu_ContextMenuBegin (void)
  {
   HTM_DIV_Begin ("class=\"CONTEXT_MENU\"");
  }

void Mnu_ContextMenuEnd (void)
  {
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*********** Get and show number of users who have chosen a menu *************/
/*****************************************************************************/

void Mnu_GetAndShowNumUsrsPerMenu (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_menu;
   extern const char *Mnu_MenuIcons[Mnu_NUM_MENUS];
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Menu;
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_MENU_NAMES[Mnu_NUM_MENUS];
   Mnu_Menu_t Menu;
   char *SubQuery;
   unsigned NumUsrs[Mnu_NUM_MENUS];
   unsigned NumUsrsTotal = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_MENUS],NULL,NULL,
                      Hlp_ANALYTICS_Figures_menu,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Menu            ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_Number_of_users ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** For each menu... *****/
      for (Menu  = (Mnu_Menu_t) 0;
	   Menu <= (Mnu_Menu_t) (Mnu_NUM_MENUS - 1);
	   Menu++)
	{
	 /* Get number of users who have chosen this menu from database */
	 if (asprintf (&SubQuery,"usr_data.Menu=%u",
		       (unsigned) Menu) < 0)
	    Err_NotEnoughMemoryExit ();
	 NumUsrs[Menu] = Usr_DB_GetNumUsrsWhoChoseAnOption (SubQuery);
	 free (SubQuery);

	 /* Update total number of users */
	 NumUsrsTotal += NumUsrs[Menu];
	}

      /***** Write number of users who have chosen each menu *****/
      for (Menu  = (Mnu_Menu_t) 0;
	   Menu <= (Mnu_Menu_t) (Mnu_NUM_MENUS - 1);
	   Menu++)
	{
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"CM\"");
	       Ico_PutIcon (Mnu_MenuIcons[Menu],Ico_BLACK,
	                    Txt_MENU_NAMES[Menu],"ICOx20");
	    HTM_TD_End ();

	    HTM_TD_Unsigned (NumUsrs[Menu]);
	    HTM_TD_Percentage (NumUsrs[Menu],NumUsrsTotal);

	 HTM_TR_End ();
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
