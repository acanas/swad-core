// swad_scope.c: scope (platform, center, degree, course...)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

void Sco_PutSelectorScope (const char *ParName,HTM_SubmitOnChange_t SubmitOnChange)
  {
   extern const char *Txt_System;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Center;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   HieLvl_Level_t Level;
   unsigned ScopeUnsigned;
   bool WriteScope;
   static const char **TxtScope[HieLvl_NUM_LEVELS] =
     {
      [HieLvl_SYS] = &Txt_System,
      [HieLvl_CTY] = &Txt_Country,
      [HieLvl_INS] = &Txt_Institution,
      [HieLvl_CTR] = &Txt_Center,
      [HieLvl_DEG] = &Txt_Degree,
      [HieLvl_CRS] = &Txt_Course,
     };

   HTM_SELECT_Begin (SubmitOnChange,NULL,
		     "id=\"%s\" name=\"%s\" class=\"INPUT_%s\"",
		     ParName,ParName,The_GetSuffix ());

      for (Level  = (HieLvl_Level_t) 1;
	   Level <= (HieLvl_Level_t) (HieLvl_NUM_LEVELS - 1);
	   Level++)
	 if ((Gbl.Scope.Allowed & (1 << Level)))
	   {
	    /* Don't put forbidden options in selectable list */
	    WriteScope = false;
	    switch (Level)
	      {
	       case HieLvl_SYS:
		  WriteScope = true;
		  break;
	       case HieLvl_CTY:
		  if (Gbl.Hierarchy.Node[HieLvl_CTY].HieCod > 0)
		     WriteScope = true;
		  break;
	       case HieLvl_INS:
		  if (Gbl.Hierarchy.Node[HieLvl_INS].HieCod > 0)
		     WriteScope = true;
		  break;
	       case HieLvl_CTR:
		  if (Gbl.Hierarchy.Node[HieLvl_CTR].HieCod > 0)
		     WriteScope = true;
		  break;
	       case HieLvl_DEG:
		  if (Gbl.Hierarchy.Node[HieLvl_DEG].HieCod > 0)
		     WriteScope = true;
		  break;
	       case HieLvl_CRS:
		  if (Gbl.Hierarchy.Level == HieLvl_CRS)
		     WriteScope = true;
		  break;
	       default:
		  break;
	      }

	    if (WriteScope)
	      {
	       /***** Write allowed option *****/
	       ScopeUnsigned = (unsigned) Level;
	       HTM_OPTION (HTM_Type_UNSIGNED,&ScopeUnsigned,
			   Level == Gbl.Scope.Current ? HTM_OPTION_SELECTED :
							HTM_OPTION_UNSELECTED,
			   HTM_OPTION_ENABLED,
			   "%s: %s",
			   *TxtScope[Level],
			   Gbl.Hierarchy.Node[Level].ShrtName);
	      }
	   }

   HTM_SELECT_End ();
  }

/*****************************************************************************/
/********************** Put hidden parameter scope ***************************/
/*****************************************************************************/

void Sco_PutParCurrentScope (void *Level)
  {
   if (Level)
      Sco_PutParScope ("ScopeUsr",*((HieLvl_Level_t *) Level));
  }

void Sco_PutParScope (const char *ParName,HieLvl_Level_t Level)
  {
   Par_PutParUnsigned (NULL,ParName,(unsigned) Level);
  }

/*****************************************************************************/
/*************************** Get parameter scope *****************************/
/*****************************************************************************/

void Sco_GetScope (const char *ParName)
  {
   /***** Get parameter with scope *****/
   Gbl.Scope.Current = (HieLvl_Level_t)
	               Par_GetParUnsignedLong (ParName,
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
   if (Gbl.Scope.Current == HieLvl_CRS && Gbl.Hierarchy.Node[HieLvl_CRS].HieCod <= 0)
      Gbl.Scope.Current = HieLvl_DEG;

   if (Gbl.Scope.Current == HieLvl_DEG && Gbl.Hierarchy.Node[HieLvl_DEG].HieCod <= 0)
      Gbl.Scope.Current = HieLvl_CTR;

   if (Gbl.Scope.Current == HieLvl_CTR && Gbl.Hierarchy.Node[HieLvl_CTR].HieCod <= 0)
      Gbl.Scope.Current = HieLvl_INS;

   if (Gbl.Scope.Current == HieLvl_INS && Gbl.Hierarchy.Node[HieLvl_INS].HieCod <= 0)
      Gbl.Scope.Current = HieLvl_CTY;

   if (Gbl.Scope.Current == HieLvl_CTY && Gbl.Hierarchy.Node[HieLvl_CTY].HieCod <= 0)
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
      case Rol_SYS_ADM:
	 Gbl.Scope.Allowed = 1 << HieLvl_SYS;
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
/***************** Get hierarchy level from database string ******************/
/*****************************************************************************/

HieLvl_Level_t Hie_GetLevelFromDBStr (const char *LevelDBStr)
  {
   HieLvl_Level_t Level;

   for (Level  = (HieLvl_Level_t) 0;
	Level <= (HieLvl_Level_t) (HieLvl_NUM_LEVELS - 1);
	Level++)
      if (!strcmp (Hie_GetDBStrFromLevel (Level),LevelDBStr))
	 return Level;

   return HieLvl_UNK;
  }

/*****************************************************************************/
/****************** Get database string from hierarchy level *****************/
/*****************************************************************************/

const char *Hie_GetDBStrFromLevel (HieLvl_Level_t Level)
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

   if (Level >= HieLvl_NUM_LEVELS)
      Level = HieLvl_UNK;

   return Sco_ScopeDB[Level];
  }

/*****************************************************************************/
/************************** Get current hierarchy code ***********************/
/*****************************************************************************/

long Hie_GetCurrentCod (void)
  {
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:
      case HieLvl_CTY:
      case HieLvl_INS:
      case HieLvl_CTR:
      case HieLvl_DEG:
      case HieLvl_CRS:
	 return Gbl.Hierarchy.Node[Gbl.Scope.Current].HieCod;
      default:
	 Err_WrongHierarchyLevelExit ();
	 return -1L;	// Not reached
     }
  }
