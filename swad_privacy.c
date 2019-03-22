// swad_privacy.c: users' photo and public profile visibility

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

#include "swad_action.h"
#include "swad_box.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_privacy.h"
#include "swad_theme.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/***** Visibility (who can see user's photo or public profile) *****/
const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY] =
  {
   "unknown",	// Pri_VISIBILITY_UNKNOWN
   "user",	// Pri_VISIBILITY_USER
   "course",	// Pri_VISIBILITY_COURSE
   "system",	// Pri_VISIBILITY_SYSTEM
   "world",	// Pri_VISIBILITY_WORLD
  };

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Pri_PRIVACY_ID	"privacy"

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Pri_PutIconsPrivacy (void);

static void Pri_PutFormVisibility (const char *TxtLabel,
                                   Act_Action_t Action,const char *ParamName,
                                   Pri_Visibility_t CurrentVisibilityInDB,
                                   unsigned MaskAllowedVisibility);

/*****************************************************************************/
/******************************* Edit my privacy *****************************/
/*****************************************************************************/

void Pri_EditMyPrivacy (void)
  {
   extern const char *Hlp_PROFILE_Preferences_privacy;
   extern const char *Txt_Please_review_your_privacy_preferences;
   extern const char *Txt_Privacy;
   extern const char *Txt_Photo;
   extern const char *Txt_Basic_public_profile;
   extern const char *Txt_Extended_public_profile;
   extern const char *Txt_Timeline;

   /***** Start section with preferences on privacy *****/
   Lay_StartSection (Pri_PRIVACY_ID);

   /***** If any of my preferences about privacy is unknown *****/
   if (Gbl.Usrs.Me.UsrDat.PhotoVisibility == Pri_VISIBILITY_UNKNOWN ||
       Gbl.Usrs.Me.UsrDat.BaPrfVisibility == Pri_VISIBILITY_UNKNOWN ||
       Gbl.Usrs.Me.UsrDat.ExPrfVisibility == Pri_VISIBILITY_UNKNOWN)
      Ale_ShowAlert (Ale_WARNING,Txt_Please_review_your_privacy_preferences);

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_Privacy,Pri_PutIconsPrivacy,
                      Hlp_PROFILE_Preferences_privacy,Box_NOT_CLOSABLE,2);

   /***** Edit photo visibility *****/
   Pri_PutFormVisibility (Txt_Photo,
                          ActChgPriPho,"VisPho",
                          Gbl.Usrs.Me.UsrDat.PhotoVisibility,
			  Pri_PHOTO_ALLOWED_VIS);

   /***** Edit basic public profile visibility *****/
   Pri_PutFormVisibility (Txt_Basic_public_profile,
                          ActChgBasPriPrf,"VisBasPrf",
                          Gbl.Usrs.Me.UsrDat.BaPrfVisibility,
                          Pri_BASIC_PROFILE_ALLOWED_VIS);

   /***** Edit extended public profile visibility *****/
   Pri_PutFormVisibility (Txt_Extended_public_profile,
                          ActChgExtPriPrf,"VisExtPrf",
                          Gbl.Usrs.Me.UsrDat.ExPrfVisibility,
                          Pri_EXTENDED_PROFILE_ALLOWED_VIS);

   /***** Edit public activity (timeline) visibility *****/
   Pri_PutFormVisibility (Txt_Timeline,
                          ActUnk,"VisTml",
                          Pri_VISIBILITY_SYSTEM,
                          Pri_TIMELINE_ALLOWED_VIS);

   /***** End table and box *****/
   Box_EndBoxTable ();

   /***** End section with preferences on privacy *****/
   Lay_EndSection ();
  }

/*****************************************************************************/
/***************** Put contextual icons in privacy preference ****************/
/*****************************************************************************/

static void Pri_PutIconsPrivacy (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_PRIVACY;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/************************** Select photo visibility **************************/
/*****************************************************************************/

static void Pri_PutFormVisibility (const char *TxtLabel,
                                   Act_Action_t Action,const char *ParamName,
                                   Pri_Visibility_t CurrentVisibilityInDB,
                                   unsigned MaskAllowedVisibility)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_PRIVACY_OPTIONS[Pri_NUM_OPTIONS_PRIVACY];
   Pri_Visibility_t Visibility;

   /***** Select visibility *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>"
	              "<td class=\"LEFT_TOP\">",
	    The_ClassFormInBox[Gbl.Prefs.Theme],TxtLabel);

   /***** Form with list of options *****/
   if (Action != ActUnk)
      Frm_StartFormAnchor (Action,Pri_PRIVACY_ID);
   fprintf (Gbl.F.Out,"<ul class=\"PRI_LIST LIST_LEFT\">");
   for (Visibility = Pri_VISIBILITY_USER;
	Visibility <= Pri_VISIBILITY_WORLD;
	Visibility++)
      if (MaskAllowedVisibility & (1 << Visibility))
	{
	 fprintf (Gbl.F.Out,"<li class=\"%s\">"
	                    "<label>"
			    "<input type=\"radio\" name=\"%s\" value=\"%u\"",
		  (Visibility == CurrentVisibilityInDB) ? "DAT_N LIGHT_BLUE" :
							  "DAT",
		  ParamName,(unsigned) Visibility);
	 if (Visibility == CurrentVisibilityInDB)
	    fprintf (Gbl.F.Out," checked=\"checked\"");
	 if (Action == ActUnk)
	    fprintf (Gbl.F.Out," disabled=\"disabled\"");
	 else
	    fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
		     Gbl.Form.Id);
	 fprintf (Gbl.F.Out," />"
			    "%s"
	                    "</label>"
			    "</li>",
		  Txt_PRIVACY_OPTIONS[Visibility]);
	}

   /***** End list and form *****/
   fprintf (Gbl.F.Out,"</ul>");
   if (Action != ActUnk)
      Frm_EndForm ();
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");
  }

/*****************************************************************************/
/************************ Get visibility from string *************************/
/*****************************************************************************/

Pri_Visibility_t Pri_GetVisibilityFromStr (const char *Str)
  {
   Pri_Visibility_t Visibility;

   for (Visibility = (Pri_Visibility_t) 0;
	Visibility < Pri_NUM_OPTIONS_PRIVACY;
	Visibility++)
      if (!strcasecmp (Str,Pri_VisibilityDB[Visibility]))
	 return Visibility;

   return Pri_VISIBILITY_UNKNOWN;
  }

/*****************************************************************************/
/**************** Get parameter with visibility from form ********************/
/*****************************************************************************/

Pri_Visibility_t Pri_GetParamVisibility (const char *ParamName)
  {
   return (Pri_Visibility_t)
	  Par_GetParToUnsignedLong (ParamName,
                                    0,
                                    Pri_NUM_OPTIONS_PRIVACY - 1,
                                    (unsigned long) Pri_VISIBILITY_UNKNOWN);
  }

/*****************************************************************************/
/*********** Check if user's photo of public profile can be shown ************/
/*****************************************************************************/
// Returns true if it can be shown and false if not.

bool Pri_ShowingIsAllowed (Pri_Visibility_t Visibility,struct UsrData *UsrDat)
  {
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);

   /***** I always can see my things *****/
   if (ItsMe)
      return true;

   /***** System admins always can see others' profiles *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      return true;

   /***** Check if I can see the other's photo *****/
   switch (Visibility)
     {
      case Pri_VISIBILITY_UNKNOWN:
	 return false;			// It's not me
      case Pri_VISIBILITY_USER:		// Only visible
					// by me and my teachers if I am a student
					// or me and my students if I am a teacher
         // Do both users share the same course but whit different role?
	 return Usr_CheckIfUsrSharesAnyOfMyCrsWithDifferentRole (UsrDat->UsrCod);
      case Pri_VISIBILITY_COURSE:	// Visible by users sharing courses with me
	 // Do both users share the same course?
         return Usr_CheckIfUsrSharesAnyOfMyCrs (UsrDat);
      case Pri_VISIBILITY_SYSTEM:	// Visible by any user logged in platform
         return Gbl.Usrs.Me.Logged;
      case Pri_VISIBILITY_WORLD:	// Public, visible by everyone, even unlogged visitors
         return true;
     }

   return false;	// Never reached. To avoid warning
  }
