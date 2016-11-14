// swad_tab.c: tabs drawing

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_tab.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

const char *Tab_TabIcons[Tab_NUM_TABS] =
  {
   /* TabUnk */	NULL,
   /* TabSys */	"sys64x64.gif",
   /* TabCty */	"cty64x64.gif",
   /* TabIns */	"ins64x64.gif",
   /* TabCtr */	"ctr64x64.gif",
   /* TabDeg */	"deg64x64.gif",
   /* TabCrs */	"crs64x64.gif",
   /* TabAss */	"ass64x64.gif",
   /* TabUsr */	"usr64x64.gif",
   /* TabSoc */ "soc64x64.png",
   /* TabMsg */	"msg64x64.gif",
   /* TabSta */	"sta64x64.gif",
   /* TabPrf */	"prf64x64.gif",
  };

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static bool Tab_CheckIfICanViewTab (Tab_Tab_t Tab);

static void Tab_WriteBreadcrumbHome (void);
static void Tab_WriteBreadcrumbTab (void);
static void Tab_WriteBreadcrumbAction (void);

/*****************************************************************************/
/**************** Draw tabs with the current tab highlighted *****************/
/*****************************************************************************/

void Tab_DrawTabs (void)
  {
   extern const char *The_ClassTxtTabOn[The_NUM_THEMES];
   extern const char *The_ClassTxtTabOff[The_NUM_THEMES];
   extern const char *The_TabOnBgColors[The_NUM_THEMES];
   extern const char *The_TabOffBgColors[The_NUM_THEMES];
   extern const char *Txt_TABS_FULL_TXT[Tab_NUM_TABS];
   extern const char *Txt_TABS_SHORT_TXT[Tab_NUM_TABS];
   Tab_Tab_t NumTab;
   bool ICanViewTab;

   /***** Table start *****/
   fprintf (Gbl.F.Out,"<div id=\"tabs\">"
	              "<ul class=\"LIST_CENTER\">");

   /***** Draw the tabs *****/
   for (NumTab = (Tab_Tab_t) 1;
        NumTab <= (Tab_Tab_t) Tab_NUM_TABS - 1;
        NumTab++)
     {
      ICanViewTab = Tab_CheckIfICanViewTab (NumTab);

      /* If current tab is unknown, then activate the first one with access allowed */
      if (Gbl.Action.Tab == TabUnk)
	{
	 Gbl.Action.Tab = NumTab;
	 Tab_DisableIncompatibleTabs ();
	}

      if (ICanViewTab || NumTab > TabCrs)	// Don't show the first hidden tabs
	{
	 /* Form, icon (at top) and text (at bottom) of the tab */
	 fprintf (Gbl.F.Out,"<li class=\"%s %s\">",
		  NumTab == Gbl.Action.Tab ? "TAB_ON" :
					     "TAB_OFF",
		  NumTab == Gbl.Action.Tab ? The_TabOnBgColors[Gbl.Prefs.Theme] :
					     The_TabOffBgColors[Gbl.Prefs.Theme]);
	 if (ICanViewTab)
	   {
	    fprintf (Gbl.F.Out,"<div");	// This div must be present even in current tab in order to render properly the tab
	    if (NumTab != Gbl.Action.Tab)
	       fprintf (Gbl.F.Out," class=\"ICO_HIGHLIGHT\"");
	    fprintf (Gbl.F.Out,">");
	    Act_FormStart (ActMnu);
	    Par_PutHiddenParamUnsigned ("NxtTab",(unsigned) NumTab);
	    Act_LinkFormSubmit (Txt_TABS_FULL_TXT[NumTab],
	                        NumTab == Gbl.Action.Tab ? The_ClassTxtTabOn[Gbl.Prefs.Theme] :
			                                   The_ClassTxtTabOff[Gbl.Prefs.Theme],NULL);
	    fprintf (Gbl.F.Out,"<img src=\"%s/%s/%s\""
			       " alt=\"%s\" title=\"%s\""
			       " class=\"ICO40x40\" style=\"margin:3px;\" />"
			       "<div class=\"TAB_TXT %s\">%s</div>"
			       "</a>",
		     Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION,
		     Tab_TabIcons[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     NumTab == Gbl.Action.Tab ? The_ClassTxtTabOn[Gbl.Prefs.Theme] :
			                        The_ClassTxtTabOff[Gbl.Prefs.Theme],
		     Txt_TABS_SHORT_TXT[NumTab]);
	    Act_FormEnd ();
	   }
	 else
	    fprintf (Gbl.F.Out,"<div class=\"ICO_HIDDEN\">"
			       "<img src=\"%s/%s/%s\""
			       " alt=\"%s\" title=\"%s\""
			       " class=\"ICO40x40\" style=\"margin:3px;\" />"
			       "<div class=\"TAB_TXT %s\">%s</div>",
		     Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION,
		     Tab_TabIcons[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     The_ClassTxtTabOff[Gbl.Prefs.Theme],
		     Txt_TABS_SHORT_TXT[NumTab]);

	 fprintf (Gbl.F.Out,"</div>"
			    "</li>");
	}
     }

   /***** End of the table *****/
   fprintf (Gbl.F.Out,"</ul>"
	              "</div>");
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
	 return (Gbl.CurrentCty.Cty.CtyCod <= 0);
      case TabCty:
	 return (Gbl.CurrentCty.Cty.CtyCod > 0 &&
	         Gbl.CurrentIns.Ins.InsCod <= 0);
      case TabIns:
	 return (Gbl.CurrentIns.Ins.InsCod > 0 &&
	         Gbl.CurrentCtr.Ctr.CtrCod <= 0);
      case TabCtr:
	 return (Gbl.CurrentCtr.Ctr.CtrCod > 0 &&
	         Gbl.CurrentDeg.Deg.DegCod <= 0);
      case TabDeg:
	 return (Gbl.CurrentDeg.Deg.DegCod > 0 &&
	         Gbl.CurrentCrs.Crs.CrsCod <= 0);
      case TabCrs:
	 return (Gbl.CurrentCrs.Crs.CrsCod > 0);
      default:
	 return true;
     }
  }


/*****************************************************************************/
/********************* Draw breadcrumb with tab and action *******************/
/*****************************************************************************/

void Tab_DrawBreadcrumb (void)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   extern const char *The_TabOnBgColors[The_NUM_THEMES];
   extern const char *The_ClassTxtTabOn[The_NUM_THEMES];

   fprintf (Gbl.F.Out,"<div id=\"breadcrumb_container\" class=\"%s\">",
	    The_TabOnBgColors[Gbl.Prefs.Theme]);

   /***** Home *****/
   Tab_WriteBreadcrumbHome ();

   if (Gbl.Action.Act == ActMnu ||
       Act_Actions[Act_Actions[Gbl.Action.Act].SuperAction].IndexInMenu >= 0)
     {
      /***** Tab *****/
      fprintf (Gbl.F.Out,"<span class=\"%s\"> &gt; </span>",
               The_ClassTxtTabOn[Gbl.Prefs.Theme]);
      Tab_WriteBreadcrumbTab ();

      if (Act_Actions[Act_Actions[Gbl.Action.Act].SuperAction].IndexInMenu >= 0)
        {
         /***** Menu *****/
         fprintf (Gbl.F.Out,"<span class=\"%s\"> &gt; </span>",
                  The_ClassTxtTabOn[Gbl.Prefs.Theme]);
         Tab_WriteBreadcrumbAction ();
        }
     }

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************************ Write home in breadcrumb ***************************/
/*****************************************************************************/

static void Tab_WriteBreadcrumbHome (void)
  {
   extern const char *The_ClassTxtTabOn[The_NUM_THEMES];
   extern const char *Txt_Home_PAGE;

   Act_FormStart (ActHom);
   Act_LinkFormSubmit (Txt_Home_PAGE,The_ClassTxtTabOn[Gbl.Prefs.Theme],NULL);
   fprintf (Gbl.F.Out,"%s</a>",
	    Txt_Home_PAGE);
   Act_FormEnd ();
  }

/*****************************************************************************/
/************ Write icon and title associated to the current tab *************/
/*****************************************************************************/

static void Tab_WriteBreadcrumbTab (void)
  {
   extern const char *The_ClassTxtTabOn[The_NUM_THEMES];
   extern const char *Txt_TABS_FULL_TXT[Tab_NUM_TABS];

   /***** Start form *****/
   Act_FormStart (ActMnu);
   Par_PutHiddenParamUnsigned ("NxtTab",(unsigned) Gbl.Action.Tab);
   Act_LinkFormSubmit (Txt_TABS_FULL_TXT[Gbl.Action.Tab],The_ClassTxtTabOn[Gbl.Prefs.Theme],NULL);

   /***** Title and end of form *****/
   fprintf (Gbl.F.Out,"%s</a>",Txt_TABS_FULL_TXT[Gbl.Action.Tab]);
   Act_FormEnd ();
  }

/*****************************************************************************/
/***************** Write title associated to the current action **************/
/*****************************************************************************/

static void Tab_WriteBreadcrumbAction (void)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   extern const char *The_ClassTxtTabOn[The_NUM_THEMES];
   const char *Title = Act_GetTitleAction (Gbl.Action.Act);

   /***** Start form *****/
   Act_FormStart (Act_Actions[Gbl.Action.Act].SuperAction);
   Act_LinkFormSubmit (Title,The_ClassTxtTabOn[Gbl.Prefs.Theme],NULL);

   /***** Title and end of form *****/
   fprintf (Gbl.F.Out,"%s</a>",
	    Title);
   Act_FormEnd ();
  }

/*****************************************************************************/
/*************** Set current tab depending on current action *****************/
/*****************************************************************************/

void Tab_SetCurrentTab (void)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   Act_Action_t SuperAction;

   SuperAction = Act_Actions[Gbl.Action.Act].SuperAction;
   Gbl.Action.Tab = Act_Actions[SuperAction].Tab;

   /***** Change action and tab if country, institution, centre or degree
          are incompatible with the current tab *****/
   switch (Gbl.Action.Tab)
     {
      case TabCty:
	 if (Gbl.CurrentCty.Cty.CtyCod <= 0)		// No country selected
	    Gbl.Action.Act = ActSeeCty;
	 break;
      case TabIns:
	 if (Gbl.CurrentIns.Ins.InsCod <= 0)		// No institution selected
	   {
	    if (Gbl.CurrentCty.Cty.CtyCod > 0)		// Country selected, but no institution selected
	       Gbl.Action.Act = ActSeeIns;
	    else					// No country selected
	       Gbl.Action.Act = ActSeeCty;
	  }
	break;
      case TabCtr:
	 if (Gbl.CurrentCtr.Ctr.CtrCod <= 0)		// No centre selected
	   {
	    if (Gbl.CurrentIns.Ins.InsCod > 0)		// Institution selected, but no centre selected
	       Gbl.Action.Act = ActSeeCtr;
	    else if (Gbl.CurrentCty.Cty.CtyCod > 0)	// Country selected, but no institution selected
	       Gbl.Action.Act = ActSeeIns;
	    else					// No country selected
	       Gbl.Action.Act = ActSeeCty;
	   }
         break;
      case TabDeg:
         if (Gbl.CurrentDeg.Deg.DegCod <= 0)		// No degree selected
	   {
	    if (Gbl.CurrentCtr.Ctr.CtrCod > 0)		// Centre selected, but no degree selected
	       Gbl.Action.Act = ActSeeDeg;
	    else if (Gbl.CurrentIns.Ins.InsCod > 0)	// Institution selected, but no centre selected
	       Gbl.Action.Act = ActSeeCtr;
	    else if (Gbl.CurrentCty.Cty.CtyCod > 0)	// Country selected, but no institution selected
	       Gbl.Action.Act = ActSeeIns;
	    else					// No country selected
	       Gbl.Action.Act = ActSeeCty;
	   }
         break;
      default:
         break;
     }

   SuperAction = Act_Actions[Gbl.Action.Act].SuperAction;
   Gbl.Action.Tab = Act_Actions[SuperAction].Tab;

   Tab_DisableIncompatibleTabs ();
  }

/*****************************************************************************/
/************************** Disable incompatible tabs ************************/
/*****************************************************************************/

void Tab_DisableIncompatibleTabs (void)
  {
   /***** Set country, institution, centre, degree and course depending on the current tab.
          This will disable tabs incompatible with the current one. *****/
   switch (Gbl.Action.Tab)
     {
      case TabSys:
	 Gbl.CurrentCty.Cty.CtyCod = -1L;
	 // no break
      case TabCty:
	 Gbl.CurrentIns.Ins.InsCod = -1L;
	 // no break
      case TabIns:
	 Gbl.CurrentCtr.Ctr.CtrCod = -1L;
	 // no break
      case TabCtr:
	 Gbl.CurrentDeg.Deg.DegCod = -1L;
	 // no break
      case TabDeg:
	 Gbl.CurrentCrs.Crs.CrsCod = -1L;
	 break;
      default:
         break;
     }
  }
