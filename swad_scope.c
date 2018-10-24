// swad_scope.c: scope (platform, centre, degree, course...)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

#include <string.h>		// For string functions

#include "swad_config.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_scope.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *Sco_ScopeDB[Sco_NUM_SCOPES] =
  {
   "Unk",	// Sco_SCOPE_UNK
   "Sys",	// Sco_SCOPE_SYS
   "Cty",	// Sco_SCOPE_CTY
   "Ins",	// Sco_SCOPE_INS
   "Ctr",	// Sco_SCOPE_CTR
   "Deg",	// Sco_SCOPE_DEG
   "Crs",	// Sco_SCOPE_CRS
  };

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

void Sco_PutSelectorScope (const char *ParamName,bool SendOnChange)
  {
   extern const char *Txt_System;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   Sco_Scope_t Scope;
   bool WriteScope;

   fprintf (Gbl.F.Out,"<select id=\"%s\" name=\"%s\"",ParamName,ParamName);
   if (SendOnChange)
      fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
               Gbl.Form.Id);
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
	    case Sco_SCOPE_SYS:
	       WriteScope = true;
	       break;
	    case Sco_SCOPE_CTY:
	       if (Gbl.CurrentCty.Cty.CtyCod > 0)
		  WriteScope = true;
	       break;
	    case Sco_SCOPE_INS:
	       if (Gbl.CurrentIns.Ins.InsCod > 0)
		  WriteScope = true;
	       break;
	    case Sco_SCOPE_CTR:
	       if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
		  WriteScope = true;
	       break;
	    case Sco_SCOPE_DEG:
	       if (Gbl.CurrentDeg.Deg.DegCod > 0)
		  WriteScope = true;
	       break;
	    case Sco_SCOPE_CRS:
	       if (Gbl.CurrentCrs.Crs.CrsCod > 0)
		  WriteScope = true;
	       break;
	    default:
	       Lay_WrongScopeExit ();
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
	       case Sco_SCOPE_SYS:
		  fprintf (Gbl.F.Out,"%s: %s",
			   Txt_System,Cfg_PLATFORM_SHORT_NAME);
		  break;
	       case Sco_SCOPE_CTY:
		  fprintf (Gbl.F.Out,"%s: %s",
			   Txt_Country,
			   Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
		  break;
	       case Sco_SCOPE_INS:
		  fprintf (Gbl.F.Out,"%s: %s",
			   Txt_Institution,
			   Gbl.CurrentIns.Ins.ShrtName);
		  break;
	       case Sco_SCOPE_CTR:
		  fprintf (Gbl.F.Out,"%s: %s",
			   Txt_Centre,
			   Gbl.CurrentCtr.Ctr.ShrtName);
		  break;
	       case Sco_SCOPE_DEG:
		  fprintf (Gbl.F.Out,"%s: %s",
			   Txt_Degree,
			   Gbl.CurrentDeg.Deg.ShrtName);
		  break;
	       case Sco_SCOPE_CRS:
		  fprintf (Gbl.F.Out,"%s: %s",
			   Txt_Course,
			   Gbl.CurrentCrs.Crs.ShrtName);
		  break;
	       default:
		  Lay_WrongScopeExit ();
		  break;
	      }
	    fprintf (Gbl.F.Out,"</option>");
	   }
	}

   fprintf (Gbl.F.Out,"</select>");
  }

/*****************************************************************************/
/********************** Put hidden parameter scope ***************************/
/*****************************************************************************/

void Sco_PutParamScope (const char *ParamName,Sco_Scope_t Scope)
  {
   Par_PutHiddenParamUnsigned (ParamName,(unsigned) Scope);
  }

/*****************************************************************************/
/*************************** Get parameter scope *****************************/
/*****************************************************************************/

void Sco_GetScope (const char *ParamName)
  {
   /***** Get parameter with scope *****/
   Gbl.Scope.Current = (Sco_Scope_t)
	               Par_GetParToUnsignedLong (ParamName,
                                                 0,
                                                 Sco_NUM_SCOPES - 1,
                                                 (unsigned long) Sco_SCOPE_UNK);

   /***** Adjust scope avoiding impossible or forbidden scopes *****/
   Sco_AdjustScope ();
  }

/*****************************************************************************/
/*********** Adjust scope avoiding impossible or forbidden scopes ************/
/*****************************************************************************/

void Sco_AdjustScope (void)
  {
   /***** Is scope is unknow, use default scope *****/
   if (Gbl.Scope.Current == Sco_SCOPE_UNK)
      Gbl.Scope.Current = Gbl.Scope.Default;

   /***** Avoid impossible scopes *****/
   if (Gbl.Scope.Current == Sco_SCOPE_CRS && Gbl.CurrentCrs.Crs.CrsCod <= 0)
      Gbl.Scope.Current = Sco_SCOPE_DEG;

   if (Gbl.Scope.Current == Sco_SCOPE_DEG && Gbl.CurrentDeg.Deg.DegCod <= 0)
      Gbl.Scope.Current = Sco_SCOPE_CTR;

   if (Gbl.Scope.Current == Sco_SCOPE_CTR && Gbl.CurrentCtr.Ctr.CtrCod <= 0)
      Gbl.Scope.Current = Sco_SCOPE_INS;

   if (Gbl.Scope.Current == Sco_SCOPE_INS && Gbl.CurrentIns.Ins.InsCod <= 0)
      Gbl.Scope.Current = Sco_SCOPE_CTY;

   if (Gbl.Scope.Current == Sco_SCOPE_CTY && Gbl.CurrentCty.Cty.CtyCod <= 0)
      Gbl.Scope.Current = Sco_SCOPE_SYS;

   /***** Avoid forbidden scopes *****/
   if ((Gbl.Scope.Allowed & (1 << Gbl.Scope.Current)) == 0)
      Gbl.Scope.Current = Sco_SCOPE_UNK;
  }

/*****************************************************************************/
/****************** Set allowed scopes when listing guests *******************/
/*****************************************************************************/

void Sco_SetScopesForListingGuests (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_CTR_ADM:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_CTR;
	 Gbl.Scope.Default = Sco_SCOPE_CTR;
	 break;
      case Rol_INS_ADM:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_INS |
		             1 << Sco_SCOPE_CTR;
	 Gbl.Scope.Default = Sco_SCOPE_INS;
	 break;
      case Rol_SYS_ADM:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	                     1 << Sco_SCOPE_CTY |
		             1 << Sco_SCOPE_INS |
		             1 << Sco_SCOPE_CTR;
	 Gbl.Scope.Default = Sco_SCOPE_SYS;
	 break;
      default:
      	 Gbl.Scope.Allowed = 0;
      	 Gbl.Scope.Default = Sco_SCOPE_UNK;
	 break;
     }
  }

/*****************************************************************************/
/**************** Set allowed scopes when listing students *******************/
/*****************************************************************************/

void Sco_SetScopesForListingStudents (void)
  {
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_CRS;
	 break;
      case Rol_DEG_ADM:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_DEG |
		             1 << Sco_SCOPE_CRS;
	 break;
      case Rol_CTR_ADM:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_CTR |
		             1 << Sco_SCOPE_DEG |
		             1 << Sco_SCOPE_CRS;
	 break;
      case Rol_INS_ADM:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_INS |
		             1 << Sco_SCOPE_CTR |
		             1 << Sco_SCOPE_DEG |
		             1 << Sco_SCOPE_CRS;
	 break;
      case Rol_SYS_ADM:
	 Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	                     1 << Sco_SCOPE_CTY |
		             1 << Sco_SCOPE_INS |
		             1 << Sco_SCOPE_CTR |
		             1 << Sco_SCOPE_DEG |
		             1 << Sco_SCOPE_CRS;
	 break;
      default:
      	 Gbl.Scope.Allowed = 0;
      	 Gbl.Scope.Default = Sco_SCOPE_UNK;
	 break;
     }
  }

/*****************************************************************************/
/*********************** Get scope from unsigned string **********************/
/*****************************************************************************/

Sco_Scope_t Sco_GetScopeFromUnsignedStr (const char *UnsignedStr)
  {
   unsigned UnsignedNum;

   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Sco_NUM_SCOPES)
         return (Sco_Scope_t) UnsignedNum;

   return Sco_SCOPE_UNK;
  }

/*****************************************************************************/
/*********************** Get scope from database string **********************/
/*****************************************************************************/

Sco_Scope_t Sco_GetScopeFromDBStr (const char *ScopeDBStr)
  {
   Sco_Scope_t Scope;

   for (Scope = Sco_SCOPE_UNK;
	Scope < Sco_NUM_SCOPES;
	Scope++)
      if (!strcmp (Sco_ScopeDB[Scope],ScopeDBStr))
	 return Scope;

   return Sco_SCOPE_UNK;
  }
