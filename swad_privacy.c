// swad_privacy.c: users' photo and public profile visibility

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

#include <string.h>

#include "swad_action.h"
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
   "user",	// Pri_VISIBILITY_USER
   "course",	// Pri_VISIBILITY_COURSE
   "system",	// Pri_VISIBILITY_SYSTEM
   "world",	// Pri_VISIBILITY_WORLD
  };

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Pri_PutFormVisibility (const char *TxtLabel,
                                   Act_Action_t Action,const char *ParamName,
                                   Pri_Visibility_t CurrentVisibilityInDB);

/*****************************************************************************/
/*************** Put a link to the action to edit my privacy *****************/
/*****************************************************************************/

void Pri_PutLinkToChangeMyPrivacy (void)
  {
   extern const char *Txt_Privacy;

   /***** Link to edit my privacy *****/
   Lay_PutContextualLink (ActEdiPri,NULL,"lock64x64.png",
                          Txt_Privacy,Txt_Privacy);
  }

/*****************************************************************************/
/******************************* Edit my privacy *****************************/
/*****************************************************************************/

void Pri_EditMyPrivacy (void)
  {
   extern const char *Txt_Privacy;
   extern const char *Txt_Photo;
   extern const char *Txt_Public_profile;

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,2,Txt_Privacy);

   /***** Edit photo visibility *****/
   Pri_PutFormVisibility (Txt_Photo,
                          ActChgPriPho,"VisPho",
                          Gbl.Usrs.Me.UsrDat.PhotoVisibility);

   /***** Edit public profile visibility *****/
   Pri_PutFormVisibility (Txt_Public_profile,
                          ActChgPriPrf,"VisPrf",
                          Gbl.Usrs.Me.UsrDat.ProfileVisibility);

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/************************** Select photo visibility **************************/
/*****************************************************************************/

static void Pri_PutFormVisibility (const char *TxtLabel,
                                   Act_Action_t Action,const char *ParamName,
                                   Pri_Visibility_t CurrentVisibilityInDB)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_PRIVACY_OPTIONS[Pri_NUM_OPTIONS_PRIVACY];
   Pri_Visibility_t Visibility;

   /***** Select visibility *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>"
	              "<td class=\"LEFT_TOP\">",
	    The_ClassForm[Gbl.Prefs.Theme],TxtLabel);

   /***** Form with list of options *****/
   Act_FormStart (Action);
   fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">");
   for (Visibility = (Pri_Visibility_t) 0;
	Visibility < Pri_NUM_OPTIONS_PRIVACY;
	Visibility++)
     {
      fprintf (Gbl.F.Out,"<li class=\"%s\">"
                         "<input type=\"radio\" name=\"%s\" value=\"%u\"",
               (Visibility == CurrentVisibilityInDB) ? "DAT_N LIGHT_BLUE" :
        	                                       "DAT",
               ParamName,(unsigned) Visibility);
      if (Visibility == CurrentVisibilityInDB)
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"document.getElementById('%s').submit();\" />"
	                 "%s"
                         "</li>",
               Gbl.FormId,Txt_PRIVACY_OPTIONS[Visibility]);
     }

   /***** End of list and form *****/
   fprintf (Gbl.F.Out,"</ul>");
   Act_FormEnd ();
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

   return Pri_VISIBILITY_DEFAULT;
  }

/*****************************************************************************/
/**************** Get parameter with visibility from form ********************/
/*****************************************************************************/

Pri_Visibility_t Pri_GetParamVisibility (const char *ParamName)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText (ParamName,UnsignedStr,10);
   if (UnsignedStr[0])
     {
      if (sscanf (UnsignedStr,"%u",&UnsignedNum) != 1)
         Lay_ShowErrorAndExit ("Visibility is missing.");
      if (UnsignedNum >= Pri_NUM_OPTIONS_PRIVACY)
         Lay_ShowErrorAndExit ("Visibility is missing.");
      return (Pri_Visibility_t) UnsignedNum;
     }

   return Pri_VISIBILITY_DEFAULT;
  }

/*****************************************************************************/
/*********** Check if user's photo of public profile can be shown ************/
/*****************************************************************************/
// Returns true if it can be shown and false if not.

bool Pri_ShowIsAllowed (Pri_Visibility_t Visibility,long OtherUsrCod)
  {
   /***** System admins always can see others' profiles *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
      return true;

   /***** Check if I can see the other's photo *****/
   switch (Visibility)
     {
      case Pri_VISIBILITY_USER:		// Only visible by me and my teachers if I am a student or me and my students if I am a teacher
         if (Gbl.Usrs.Me.UsrDat.UsrCod == OtherUsrCod)		// It's me, I always can see my things
            return true;
         else
            return Usr_CheckIfUsrSharesAnyOfMyCrsWithDifferentRole (OtherUsrCod);	// Both users share the same course but whit different role
      case Pri_VISIBILITY_COURSE:	// Visible by users sharing courses with me
         return Usr_CheckIfUsrSharesAnyOfMyCrs (OtherUsrCod);	// Both users share the same course
      case Pri_VISIBILITY_SYSTEM:	// Visible by any user logged in platform
         return Gbl.Usrs.Me.Logged;
      case Pri_VISIBILITY_WORLD:	// Public, visible by everyone, even unlogged visitors
         return true;
     }

   return false;	// Never reached. To avoid warning
  }
