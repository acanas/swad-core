// swad_tab.c: tabs drawing

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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
extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

const char *Tab_TabIcons[Tab_NUM_TABS] =
  {
   /* TabUnk */	NULL,
   /* TabSys */	"sys",
   /* TabCty */	"cty",
   /* TabIns */	"ins",
   /* TabCtr */	"ctr",
   /* TabDeg */	"deg",
   /* TabCrs */	"crs",
   /* TabAss */	"ass",
   /* TabUsr */	"usr",
   /* TabMsg */	"msg",
   /* TabSta */	"sta",
   /* TabPrf */	"prf",
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

static void Tab_DrawTabsDeskTop (void);
static bool Tab_CheckIfICanViewTab (Act_Tab_t Tab);

static void Tab_WriteBreadcrumbHome (void);
static void Tab_WriteBreadcrumbTab (void);
static void Tab_WriteBreadcrumbAction (void);

/*****************************************************************************/
/**************** Draw tabs with the current tab highlighted *****************/
/*****************************************************************************/

void Tab_DrawTabs (void)
  {
   unsigned ColspanCentralPart = 3;

   if (Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP)
     {
      if (Gbl.Prefs.SideCols == Lay_SHOW_BOTH_COLUMNS)	// 11: both side columns visible, left and right
	 ColspanCentralPart = 1;
      else if (Gbl.Prefs.SideCols != Lay_HIDE_BOTH_COLUMNS)	// 10 or 01: only one side column visible, left or right
	 ColspanCentralPart = 2;
     }
   fprintf (Gbl.F.Out,"<td colspan=\"%u\" class=\"CENTER_TOP\""
	              " style=\"height:84px;\">"
		      "<div id=\"tabs_container\">",
            ColspanCentralPart);
   switch (Gbl.Prefs.Layout)
     {
      case Lay_LAYOUT_DESKTOP:
         Tab_DrawTabsDeskTop ();
         break;
      case Lay_LAYOUT_MOBILE:
         Tab_DrawBreadcrumb ();
         break;
      default:
      	 break;
     }
   fprintf (Gbl.F.Out,"</div>"
	              "</td>");
  }

/*****************************************************************************/
/**************** Draw tabs with the current tab highlighted *****************/
/*****************************************************************************/

static void Tab_DrawTabsDeskTop (void)
  {
   extern const char *The_ClassTabOn[The_NUM_THEMES];
   extern const char *The_ClassTabOff[The_NUM_THEMES];
   extern const char *The_TabOnBgColors[The_NUM_THEMES];
   extern const char *The_TabOffBgColors[The_NUM_THEMES];
   extern const char *Txt_TABS_FULL_TXT[Tab_NUM_TABS];
   extern const char *Txt_TABS_SHORT_TXT[Tab_NUM_TABS];
   Act_Tab_t NumTab;
   bool ICanViewTab;

   /***** Table start *****/
   fprintf (Gbl.F.Out,"<ul class=\"LIST_CENTER\">");

   /***** Draw the tabs *****/
   for (NumTab = (Act_Tab_t) 1;
        NumTab <= (Act_Tab_t) Tab_NUM_TABS - 1;
        NumTab++)
     {
      ICanViewTab = Tab_CheckIfICanViewTab (NumTab);

      /* If current tab is unknown, then activate the first one with access allowed */
      if (Gbl.CurrentTab == TabUnk)
	{
	 Gbl.CurrentTab = NumTab;
	 Tab_DisableIncompatibleTabs ();
	}

      if (ICanViewTab || NumTab > TabCrs)	// Don't show the first hidden tabs
	{
	 /* Form, icon (at top) and text (at bottom) of the tab */
	 fprintf (Gbl.F.Out,"<li class=\"%s %s\">",
		  NumTab == Gbl.CurrentTab ? "TAB_ON" :
					     "TAB_OFF",
		  NumTab == Gbl.CurrentTab ? The_TabOnBgColors[Gbl.Prefs.Theme] :
					     The_TabOffBgColors[Gbl.Prefs.Theme]);
	 if (ICanViewTab)
	   {
	    fprintf (Gbl.F.Out,"<div");	// This div must be present even in current tab in order to render properly the tab
	    if (NumTab != Gbl.CurrentTab)
	       fprintf (Gbl.F.Out," class=\"ICON_HIGHLIGHT\"");
	    fprintf (Gbl.F.Out,">");
	    Act_FormStart (ActMnu);
	    Par_PutHiddenParamUnsigned ("NxtTab",(unsigned) NumTab);
	    Act_LinkFormSubmit (Txt_TABS_FULL_TXT[NumTab],
				NumTab == Gbl.CurrentTab ? The_ClassTabOn[Gbl.Prefs.Theme] :
							   The_ClassTabOff[Gbl.Prefs.Theme]);
	    fprintf (Gbl.F.Out,"<img src=\"%s/%s/%s64x64.gif\""
			       " alt=\"%s\" title=\"%s\""
			       " class=\"ICON32x32\" style=\"margin:6px;\" />"
			       "<div>%s</div>"
			       "</a>",
		     Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION,
		     Tab_TabIcons[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_SHORT_TXT[NumTab]);
	    Act_FormEnd ();
	   }
	 else
	    fprintf (Gbl.F.Out,"<div class=\"ICON_HIDDEN\">"
			       "<img src=\"%s/%s/%s64x64.gif\""
			       " alt=\"%s\" title=\"%s\""
			       " class=\"ICON32x32\" style=\"margin:6px;\" />"
			       "<div class=\"%s\">%s</div>",
		     Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION,
		     Tab_TabIcons[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     The_ClassTabOff[Gbl.Prefs.Theme],
		     Txt_TABS_SHORT_TXT[NumTab]);

	 fprintf (Gbl.F.Out,"</div>"
			    "</li>");
	}
     }

   /***** End of the table *****/
   fprintf (Gbl.F.Out,"</ul>");
  }

/*****************************************************************************/
/************************ Draw vertical menu with tabs ***********************/
/*****************************************************************************/

void Tab_DrawTabsMobile (void)
  {
   extern const char *The_ClassMenuOff[The_NUM_THEMES];
   extern const char *Txt_TABS_FULL_TXT[Tab_NUM_TABS];
   unsigned NumTabVisible;
   Act_Tab_t NumTab;
   bool ICanViewTab;

   /***** Table start *****/
   fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">");

   /***** Loop to write all tabs. Each row holds a tab *****/
   for (NumTabVisible = 0, NumTab = (Act_Tab_t) 1;
        NumTab <= (Act_Tab_t) Tab_NUM_TABS - 1;
        NumTab++)
     {
      ICanViewTab = Tab_CheckIfICanViewTab (NumTab);

      if (ICanViewTab || NumTab > TabCrs)	// Don't show the first hidden tabs
	{
	 if (NumTabVisible % Cfg_LAYOUT_MOBILE_NUM_COLUMNS == 0)
	    fprintf (Gbl.F.Out,"<tr>");

	 /* Icon at top and text at bottom */
	 fprintf (Gbl.F.Out,"<td class=\"CENTER_TOP\" style=\"width:25%%;\">");
	 if (ICanViewTab)
	   {
	    fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\">");
	    Act_FormStart (ActMnu);
	    Par_PutHiddenParamUnsigned ("NxtTab",(unsigned) NumTab);
	    Act_LinkFormSubmit (Txt_TABS_FULL_TXT[NumTab],The_ClassMenuOff[Gbl.Prefs.Theme]);
	    fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s/%s64x64.gif\""
			       " alt=\"%s\" title=\"%s\" class=\"ICON64x64\""
			       " style=\"margin:6px;\" />"
			       "<div>%s</div>"
			       "</a>",
		     Gbl.Prefs.PathIconSet,Cfg_ICON_ACTION,
		     Tab_TabIcons[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_FULL_TXT[NumTab]);
	    Act_FormEnd ();
	    fprintf (Gbl.F.Out,"</div>");
	   }
	 else
	    fprintf (Gbl.F.Out,"<div class=\"ICON_HIDDEN\">"
			       "<img src=\"%s/%s/%s64x64.gif\""
			       " alt=\"%s\" title=\"%s\""
			       " class=\"ICON64x64\" style=\"margin:6px;\" />"
			       "<div class=\"%s\">%s</div>"
			       "</div>",
		     Gbl.Prefs.PathIconSet,
		     Cfg_ICON_ACTION,
		     Tab_TabIcons[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     Txt_TABS_FULL_TXT[NumTab],
		     The_ClassMenuOff[Gbl.Prefs.Theme],
		     Txt_TABS_FULL_TXT[NumTab]);
	 fprintf (Gbl.F.Out,"</td>");
	 if ((NumTabVisible % Cfg_LAYOUT_MOBILE_NUM_COLUMNS) == (Cfg_LAYOUT_MOBILE_NUM_COLUMNS-1))
	    fprintf (Gbl.F.Out,"</tr>");

	 NumTabVisible++;
	}
     }

   /***** End of the table *****/
   fprintf (Gbl.F.Out,"</table>");
  }

/*****************************************************************************/
/************************* Check if I can view a tab *************************/
/*****************************************************************************/

static bool Tab_CheckIfICanViewTab (Act_Tab_t Tab)
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
      case TabAss:
	 return (Gbl.CurrentCrs.Crs.CrsCod > 0);
      case TabMsg:
	 return (Gbl.Usrs.Me.Logged ||
	         Gbl.CurrentCrs.Crs.CrsCod > 0);
      default:
	 return true;
     }
  }


/*****************************************************************************/
/********************* Draw breadcrumb with tab and action *******************/
/*****************************************************************************/

void Tab_DrawBreadcrumb (void)
  {
   extern const char *The_TabOnBgColors[The_NUM_THEMES];
   extern const char *The_ClassTabOn[The_NUM_THEMES];

   fprintf (Gbl.F.Out,"<div class=\"TAB_ON %s\">",
	    The_TabOnBgColors[Gbl.Prefs.Theme]);

   /***** Home *****/
   Tab_WriteBreadcrumbHome ();

   if (Gbl.CurrentAct == ActMnu ||
       Act_Actions[Act_Actions[Gbl.CurrentAct].SuperAction].IndexInMenu >= 0)
     {
      /***** Tab *****/
      fprintf (Gbl.F.Out,"<span class=\"%s\"> &gt; </span>",
               The_ClassTabOn[Gbl.Prefs.Theme]);
      Tab_WriteBreadcrumbTab ();

      if (Act_Actions[Act_Actions[Gbl.CurrentAct].SuperAction].IndexInMenu >= 0)
        {
         /***** Menu *****/
         fprintf (Gbl.F.Out,"<span class=\"%s\"> &gt; </span>",
                  The_ClassTabOn[Gbl.Prefs.Theme]);
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
   extern const char *The_ClassTabOn[The_NUM_THEMES];
   extern const char *Txt_Home_PAGE;

   Act_FormStart (ActHom);
   Act_LinkFormSubmit (Txt_Home_PAGE,The_ClassTabOn[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s</a>",
	    Txt_Home_PAGE);
   Act_FormEnd ();
  }

/*****************************************************************************/
/************ Write icon and title associated to the current tab *************/
/*****************************************************************************/

static void Tab_WriteBreadcrumbTab (void)
  {
   extern const char *The_ClassTabOn[The_NUM_THEMES];
   extern const char *Txt_TABS_FULL_TXT[Tab_NUM_TABS];

   /***** Start form *****/
   Act_FormStart (ActMnu);
   Par_PutHiddenParamUnsigned ("NxtTab",(unsigned) Gbl.CurrentTab);
   Act_LinkFormSubmit (Txt_TABS_FULL_TXT[Gbl.CurrentTab],The_ClassTabOn[Gbl.Prefs.Theme]);

   /***** Title and end of form *****/
   fprintf (Gbl.F.Out,"%s</a>",
	    Txt_TABS_FULL_TXT[Gbl.CurrentTab]);
   Act_FormEnd ();
  }

/*****************************************************************************/
/***************** Write title associated to the current action **************/
/*****************************************************************************/

static void Tab_WriteBreadcrumbAction (void)
  {
   extern const char *The_ClassTabOn[The_NUM_THEMES];
   const char *Title = Act_GetTitleAction (Gbl.CurrentAct);

   /***** Start form *****/
   Act_FormStart (Act_Actions[Gbl.CurrentAct].SuperAction);
   Act_LinkFormSubmit (Title,The_ClassTabOn[Gbl.Prefs.Theme]);

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
   Gbl.CurrentTab = Act_Actions[Gbl.CurrentAct].Tab;

   /***** Change action and tab if country, institution, centre or degree
          are incompatible with the current tab *****/
   switch (Gbl.CurrentTab)
     {
      case TabCty:
	 if (Gbl.CurrentCty.Cty.CtyCod <= 0)		// No country selected
	    Gbl.CurrentAct = ActSeeCty;
	 break;
      case TabIns:
	 if (Gbl.CurrentIns.Ins.InsCod <= 0)		// No institution selected
	   {
	    if (Gbl.CurrentCty.Cty.CtyCod > 0)		// Country selected, but no institution selected
	       Gbl.CurrentAct = ActSeeIns;
	    else					// No country selected
	       Gbl.CurrentAct = ActSeeCty;
	  }
	break;
      case TabCtr:
	 if (Gbl.CurrentCtr.Ctr.CtrCod <= 0)		// No centre selected
	   {
	    if (Gbl.CurrentIns.Ins.InsCod > 0)		// Institution selected, but no centre selected
	       Gbl.CurrentAct = ActSeeCtr;
	    else if (Gbl.CurrentCty.Cty.CtyCod > 0)	// Country selected, but no institution selected
	       Gbl.CurrentAct = ActSeeIns;
	    else					// No country selected
	       Gbl.CurrentAct = ActSeeCty;
	   }
         break;
      case TabDeg:
         if (Gbl.CurrentDeg.Deg.DegCod <= 0)		// No degree selected
	   {
	    if (Gbl.CurrentCtr.Ctr.CtrCod > 0)		// Centre selected, but no degree selected
	       Gbl.CurrentAct = ActSeeDeg;
	    else if (Gbl.CurrentIns.Ins.InsCod > 0)	// Institution selected, but no centre selected
	       Gbl.CurrentAct = ActSeeCtr;
	    else if (Gbl.CurrentCty.Cty.CtyCod > 0)	// Country selected, but no institution selected
	       Gbl.CurrentAct = ActSeeIns;
	    else					// No country selected
	       Gbl.CurrentAct = ActSeeCty;
	   }
         break;
      default:
         break;
     }
   Gbl.CurrentTab = Act_Actions[Gbl.CurrentAct].Tab;

   Tab_DisableIncompatibleTabs ();
  }

/*****************************************************************************/
/************************** Disable incompatible tabs ************************/
/*****************************************************************************/

void Tab_DisableIncompatibleTabs (void)
  {
   /***** Set country, institution, centre, degree and course depending on the current tab.
          This will disable tabs incompatible with the current one. *****/
   switch (Gbl.CurrentTab)
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
