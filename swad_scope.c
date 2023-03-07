// swad_scope.c: scope (platform, center, degree, course...)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Cañas Vargas

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
#include "swad_error.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_scope.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/** Put a selector to choice between ranges when getting users for listing ***/
/*****************************************************************************/

void Sco_PutSelectorScope (const char *ParamName,HTM_SubmitOnChange_t SubmitOnChange)
  {
   extern const char *Txt_System;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Center;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   HieLvl_Level_t Scope;
   unsigned ScopeUnsigned;
   bool WriteScope;

   HTM_SELECT_Begin (SubmitOnChange,
		     "id=\"%s\" name=\"%s\" class=\"INPUT_%s\"",
		     ParamName,ParamName,The_GetSuffix ());

      for (Scope  = (HieLvl_Level_t) 0;
	   Scope <= (HieLvl_Level_t) (HieLvl_NUM_LEVELS - 1);
	   Scope++)
	 if ((Gbl.Scope.Allowed & (1 << Scope)))
	   {
	    /* Don't put forbidden options in selectable list */
	    WriteScope = false;
	    switch (Scope)
	      {
	       case HieLvl_SYS:
		  WriteScope = true;
		  break;
	       case HieLvl_CTY:
		  if (Gbl.Hierarchy.Cty.CtyCod > 0)
		     WriteScope = true;
		  break;
	       case HieLvl_INS:
		  if (Gbl.Hierarchy.Ins.InsCod > 0)
		     WriteScope = true;
		  break;
	       case HieLvl_CTR:
		  if (Gbl.Hierarchy.Ctr.CtrCod > 0)
		     WriteScope = true;
		  break;
	       case HieLvl_DEG:
		  if (Gbl.Hierarchy.Deg.DegCod > 0)
		     WriteScope = true;
		  break;
	       case HieLvl_CRS:
		  if (Gbl.Hierarchy.Level == HieLvl_CRS)
		     WriteScope = true;
		  break;
	       default:
		  Err_WrongScopeExit ();
		  break;
	      }

	    if (WriteScope)
	      {
	       /***** Write allowed option *****/
	       ScopeUnsigned = (unsigned) Scope;
	       switch (Scope)
		 {
		  case HieLvl_SYS:
		     HTM_OPTION (HTM_Type_UNSIGNED,&ScopeUnsigned,
				 Gbl.Scope.Current == Scope,false,
				 "%s: %s",
				 Txt_System,
				 Cfg_PLATFORM_SHORT_NAME);
		     break;
		  case HieLvl_CTY:
		     HTM_OPTION (HTM_Type_UNSIGNED,&ScopeUnsigned,
				 Gbl.Scope.Current == Scope,false,
				 "%s: %s",
				 Txt_Country,
				 Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);
		     break;
		  case HieLvl_INS:
		     HTM_OPTION (HTM_Type_UNSIGNED,&ScopeUnsigned,
				 Gbl.Scope.Current == Scope,false,
				 "%s: %s",
				 Txt_Institution,
				 Gbl.Hierarchy.Ins.ShrtName);
		     break;
		  case HieLvl_CTR:
		     HTM_OPTION (HTM_Type_UNSIGNED,&ScopeUnsigned,
				 Gbl.Scope.Current == Scope,false,
				 "%s: %s",
				 Txt_Center,
				 Gbl.Hierarchy.Ctr.ShrtName);
		     break;
		  case HieLvl_DEG:
		     HTM_OPTION (HTM_Type_UNSIGNED,&ScopeUnsigned,
				 Gbl.Scope.Current == Scope,false,
				 "%s: %s",
				 Txt_Degree,
				 Gbl.Hierarchy.Deg.ShrtName);
		     break;
		  case HieLvl_CRS:
		     HTM_OPTION (HTM_Type_UNSIGNED,&ScopeUnsigned,
				 Gbl.Scope.Current == Scope,false,
				 "%s: %s",
				 Txt_Course,
				 Gbl.Hierarchy.Crs.ShrtName);
		     break;
		  default:
		     Err_WrongScopeExit ();
		     break;
		 }
	      }
	   }

   HTM_SELECT_End ();
  }

/*****************************************************************************/
/********************** Put hidden parameter scope ***************************/
/*****************************************************************************/

void Sco_PutParamCurrentScope (void *Scope)
  {
   if (Scope)
      Sco_PutParamScope ("ScopeUsr",*((HieLvl_Level_t *) Scope));
  }

void Sco_PutParamScope (const char *ParamName,HieLvl_Level_t Scope)
  {
   Par_PutParUnsigned (NULL,ParamName,(unsigned) Scope);
  }

/*****************************************************************************/
/*************************** Get parameter scope *****************************/
/*****************************************************************************/

void Sco_GetScope (const char *ParamName)
  {
   /***** Get parameter with scope *****/
   Gbl.Scope.Current = (HieLvl_Level_t)
	               Par_GetParUnsignedLong (ParamName,
                                                 0,
                                                 HieLvl_NUM_LEVELS - 1,
                                                 (unsigned long) HieLvl_UNK);

   /***** Adjust scope avoiding impossible or forbidden scopes *****/
   Sco_AdjustScope ();
  }

/*****************************************************************************/
/*********** Adjust scope avoiding impossible or forbidden scopes ************/
/*****************************************************************************/

void Sco_AdjustScope (void)
  {
   /***** Is scope is unknow, use default scope *****/
   if (Gbl.Scope.Current == HieLvl_UNK)
      Gbl.Scope.Current = Gbl.Scope.Default;

   /***** Avoid impossible scopes *****/
   if (Gbl.Scope.Current == HieLvl_CRS && Gbl.Hierarchy.Crs.CrsCod <= 0)
      Gbl.Scope.Current = HieLvl_DEG;

   if (Gbl.Scope.Current == HieLvl_DEG && Gbl.Hierarchy.Deg.DegCod <= 0)
      Gbl.Scope.Current = HieLvl_CTR;

   if (Gbl.Scope.Current == HieLvl_CTR && Gbl.Hierarchy.Ctr.CtrCod <= 0)
      Gbl.Scope.Current = HieLvl_INS;

   if (Gbl.Scope.Current == HieLvl_INS && Gbl.Hierarchy.Ins.InsCod <= 0)
      Gbl.Scope.Current = HieLvl_CTY;

   if (Gbl.Scope.Current == HieLvl_CTY && Gbl.Hierarchy.Cty.CtyCod <= 0)
      Gbl.Scope.Current = HieLvl_SYS;

   /***** Avoid forbidden scopes *****/
   if ((Gbl.Scope.Allowed & (1 << Gbl.Scope.Current)) == 0)
      Gbl.Scope.Current = HieLvl_UNK;
  }

/*****************************************************************************/
/****************** Set allowed scopes when listing guests *******************/
/*****************************************************************************/

void Sco_SetScopesForListingGuests (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_CTR_ADM:
	 Gbl.Scope.Allowed = 1 << HieLvl_CTR;
	 Gbl.Scope.Default = HieLvl_CTR;
	 break;
      case Rol_INS_ADM:
	 Gbl.Scope.Allowed = 1 << HieLvl_INS |
		             1 << HieLvl_CTR;
	 Gbl.Scope.Default = HieLvl_INS;
	 break;
      case Rol_SYS_ADM:
	 Gbl.Scope.Allowed = 1 << HieLvl_SYS |
	                     1 << HieLvl_CTY |
		             1 << HieLvl_INS |
		             1 << HieLvl_CTR;
	 Gbl.Scope.Default = HieLvl_SYS;
	 break;
      default:
      	 Gbl.Scope.Allowed = 0;
      	 Gbl.Scope.Default = HieLvl_UNK;
	 break;
     }
  }

/*****************************************************************************/
/**************** Set allowed scopes when listing students *******************/
/*****************************************************************************/

void Sco_SetScopesForListingStudents (void)
  {
   Gbl.Scope.Default = HieLvl_CRS;
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 Gbl.Scope.Allowed = 1 << HieLvl_CRS;
	 break;
      case Rol_DEG_ADM:
	 Gbl.Scope.Allowed = 1 << HieLvl_DEG |
		             1 << HieLvl_CRS;
	 break;
      case Rol_CTR_ADM:
	 Gbl.Scope.Allowed = 1 << HieLvl_CTR |
		             1 << HieLvl_DEG |
		             1 << HieLvl_CRS;
	 break;
      case Rol_INS_ADM:
	 Gbl.Scope.Allowed = 1 << HieLvl_INS |
		             1 << HieLvl_CTR |
		             1 << HieLvl_DEG |
		             1 << HieLvl_CRS;
	 break;
      case Rol_SYS_ADM:
	 Gbl.Scope.Allowed = 1 << HieLvl_SYS |
	                     1 << HieLvl_CTY |
		             1 << HieLvl_INS |
		             1 << HieLvl_CTR |
		             1 << HieLvl_DEG |
		             1 << HieLvl_CRS;
	 break;
      default:
      	 Gbl.Scope.Allowed = 0;
      	 Gbl.Scope.Default = HieLvl_UNK;
	 break;
     }
  }

/*****************************************************************************/
/*********************** Get scope from unsigned string **********************/
/*****************************************************************************/

HieLvl_Level_t Sco_GetScopeFromUnsignedStr (const char *UnsignedStr)
  {
   unsigned UnsignedNum;

   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < HieLvl_NUM_LEVELS)
         return (HieLvl_Level_t) UnsignedNum;

   return HieLvl_UNK;
  }

/*****************************************************************************/
/*********************** Get scope from database string **********************/
/*****************************************************************************/

HieLvl_Level_t Sco_GetScopeFromDBStr (const char *ScopeDBStr)
  {
   HieLvl_Level_t Scope;

   for (Scope  = (HieLvl_Level_t) 0;
	Scope <= (HieLvl_Level_t) (HieLvl_NUM_LEVELS - 1);
	Scope++)
      if (!strcmp (Sco_GetDBStrFromScope (Scope),ScopeDBStr))
	 return Scope;

   return HieLvl_UNK;
  }

/*****************************************************************************/
/*********************** Get scope from database string **********************/
/*****************************************************************************/

const char *Sco_GetDBStrFromScope (HieLvl_Level_t Scope)
  {
   static const char *Sco_ScopeDB[HieLvl_NUM_LEVELS] =
     {
      [HieLvl_UNK] = "Unk",
      [HieLvl_SYS] = "Sys",
      [HieLvl_CTY] = "Cty",
      [HieLvl_INS] = "Ins",
      [HieLvl_CTR] = "Ctr",
      [HieLvl_DEG] = "Deg",
      [HieLvl_CRS] = "Crs",
     };

   if (Scope >= HieLvl_NUM_LEVELS)
      Scope = HieLvl_UNK;

   return Sco_ScopeDB[Scope];
  }

/*****************************************************************************/
/**************************** Get current scope code *************************/
/*****************************************************************************/

long Sco_GetCurrentCod (void)
  {
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:
	 return -1L;
      case HieLvl_CTY:
	 return Gbl.Hierarchy.Cty.CtyCod;
      case HieLvl_INS:
	 return Gbl.Hierarchy.Ins.InsCod;
      case HieLvl_CTR:
	 return Gbl.Hierarchy.Ctr.CtrCod;
      case HieLvl_DEG:
	 return Gbl.Hierarchy.Deg.DegCod;
      case HieLvl_CRS:
	 return Gbl.Hierarchy.Crs.CrsCod;
      default:
	 Err_WrongScopeExit ();
	 return -1L;	// Not reached
     }
  }
