// swad_scope.c: scope (platform, centre, degree, course...)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_config.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_scope.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal constants ****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

/*****************************************************************************/
/** Put a selector to choice between ranges when getting users for listing ***/
/*****************************************************************************/

void Sco_PutSelectorScope (bool SendOnChange)
  {
   extern const char *Txt_System;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   Sco_Scope_t Scope;
   bool WriteScope;

   fprintf (Gbl.F.Out,"<select name=\"Scope\"");
   if (SendOnChange)
      fprintf (Gbl.F.Out," onchange=\"javascript:document.getElementById('%s').submit();\"",
               Gbl.FormId);
   fprintf (Gbl.F.Out,">");

   for (Scope = (Sco_Scope_t) 0;
	Scope < Sco_NUM_SCOPES;
	Scope++)
      if ((Gbl.Scope.Allowed & (1 << Scope)))
	{
	 /* Don't put forbidden options in selectable list */
	 WriteScope = false;
	 switch (Scope)
	   {
	    case Sco_SCOPE_PLATFORM:
	       WriteScope = true;
	       break;
	    case Sco_SCOPE_COUNTRY:
	       if (Gbl.CurrentCty.Cty.CtyCod > 0)
		  WriteScope = true;
	       break;
	    case Sco_SCOPE_INSTITUTION:
	       if (Gbl.CurrentIns.Ins.InsCod > 0)
		  WriteScope = true;
	       break;
	    case Sco_SCOPE_CENTRE:
	       if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
		  WriteScope = true;
	       break;
	    case Sco_SCOPE_DEGREE:
	       if (Gbl.CurrentDeg.Deg.DegCod > 0)
		  WriteScope = true;
	       break;
	    case Sco_SCOPE_COURSE:
	       if (Gbl.CurrentCrs.Crs.CrsCod > 0)
		  WriteScope = true;
	       break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong scope.");
	       break;
	   }

	 if (WriteScope)
	   {
	    /***** Write allowed option *****/
	    fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Scope);
	    if (Gbl.Scope.Current == Scope)
	       fprintf (Gbl.F.Out," selected=\"selected\"");
	    fprintf (Gbl.F.Out,">");
	    switch (Scope)
	      {
	       case Sco_SCOPE_PLATFORM:
		  fprintf (Gbl.F.Out,"%s: %s",
			   Txt_System,Cfg_PLATFORM_PAGE_TITLE);
		  break;
	       case Sco_SCOPE_COUNTRY:
		  fprintf (Gbl.F.Out,"%s: %s",
			   Txt_Country,
			   Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
		  break;
	       case Sco_SCOPE_INSTITUTION:
		  fprintf (Gbl.F.Out,"%s: %s",
			   Txt_Institution,
			   Gbl.CurrentIns.Ins.ShortName);
		  break;
	       case Sco_SCOPE_CENTRE:
		  fprintf (Gbl.F.Out,"%s: %s",
			   Txt_Centre,
			   Gbl.CurrentCtr.Ctr.ShortName);
		  break;
	       case Sco_SCOPE_DEGREE:
		  fprintf (Gbl.F.Out,"%s: %s",
			   Txt_Degree,
			   Gbl.CurrentDeg.Deg.ShortName);
		  break;
	       case Sco_SCOPE_COURSE:
		  fprintf (Gbl.F.Out,"%s: %s",
			   Txt_Course,
			   Gbl.CurrentCrs.Crs.ShortName);
		  break;
	       default:
		  Lay_ShowErrorAndExit ("Wrong scope.");
		  break;
	      }
	    fprintf (Gbl.F.Out,"</option>");
	   }
	}

   fprintf (Gbl.F.Out,"</select>");
  }

/*****************************************************************************/
/************* Put hidden parameter with location range  *********************/
/*****************************************************************************/

void Sco_PutParamScope (Sco_Scope_t Scope)
  {
   Par_PutHiddenParamUnsigned ("Scope",(unsigned) Scope);
  }

/*****************************************************************************/
/************************* Get users range for listing ***********************/
/*****************************************************************************/

void Sco_GetScope (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Gbl.Scope.Current = Gbl.Scope.Default;

   /***** Get parameter location range if exists *****/
   Par_GetParToText ("Scope",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Sco_NUM_SCOPES)
         Gbl.Scope.Current = (Sco_Scope_t) UnsignedNum;

   /***** Avoid impossible scopes *****/
   if (Gbl.Scope.Current == Sco_SCOPE_COURSE      && Gbl.CurrentCrs.Crs.CrsCod <= 0)
      Gbl.Scope.Current = Sco_SCOPE_DEGREE;

   if (Gbl.Scope.Current == Sco_SCOPE_DEGREE      && Gbl.CurrentDeg.Deg.DegCod <= 0)
      Gbl.Scope.Current = Sco_SCOPE_CENTRE;

   if (Gbl.Scope.Current == Sco_SCOPE_CENTRE      && Gbl.CurrentCtr.Ctr.CtrCod <= 0)
      Gbl.Scope.Current = Sco_SCOPE_INSTITUTION;

   if (Gbl.Scope.Current == Sco_SCOPE_INSTITUTION && Gbl.CurrentIns.Ins.InsCod <= 0)
      Gbl.Scope.Current = Sco_SCOPE_COUNTRY;

   if (Gbl.Scope.Current == Sco_SCOPE_COUNTRY     && Gbl.CurrentCty.Cty.CtyCod <= 0)
      Gbl.Scope.Current = Sco_SCOPE_PLATFORM;

   /***** Avoid forbidden scopes *****/
   if ((Gbl.Scope.Allowed & (1 << Gbl.Scope.Current)) == 0)
      Gbl.Scope.Current = Sco_SCOPE_NONE;
  }

/*****************************************************************************/
/****************** Set allowed scopes when listing guests *******************/
/*****************************************************************************/

void Sco_SetScopesForListingGuests (void)
  {
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_CTR_ADMIN:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_CENTRE;
	 Gbl.Scope.Default = Sco_SCOPE_CENTRE;
	 break;
      case Rol_ROLE_INS_ADMIN:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_INSTITUTION |
		             1 << Sco_SCOPE_CENTRE;
	 Gbl.Scope.Default = Sco_SCOPE_INSTITUTION;
	 break;
      case Rol_ROLE_SUPERUSER:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_PLATFORM    |
	                     1 << Sco_SCOPE_COUNTRY     |
		             1 << Sco_SCOPE_INSTITUTION |
		             1 << Sco_SCOPE_CENTRE;
	 Gbl.Scope.Default = Sco_SCOPE_INSTITUTION;
	 break;
      default:
      	 Gbl.Scope.Allowed = 0;
      	 Gbl.Scope.Default = Sco_SCOPE_NONE;
	 break;
     }
  }

/*****************************************************************************/
/**************** Set allowed scopes when listing students *******************/
/*****************************************************************************/

void Sco_SetScopesForListingStudents (void)
  {
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_STUDENT:
      case Rol_ROLE_TEACHER:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_COURSE;
	 Gbl.Scope.Default = Sco_SCOPE_COURSE;
	 break;
      case Rol_ROLE_DEG_ADMIN:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_DEGREE;
	 Gbl.Scope.Default = Sco_SCOPE_DEGREE;
	 break;
      case Rol_ROLE_SUPERUSER:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_PLATFORM    |
	                     1 << Sco_SCOPE_COUNTRY     |
		             1 << Sco_SCOPE_INSTITUTION |
		             1 << Sco_SCOPE_CENTRE      |
		             1 << Sco_SCOPE_DEGREE      |
		             1 << Sco_SCOPE_COURSE;
	 Gbl.Scope.Default = Sco_SCOPE_COURSE;
	 break;
      default:
      	 Gbl.Scope.Allowed = 0;
      	 Gbl.Scope.Default = Sco_SCOPE_NONE;
	 break;
     }
  }
