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
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   Hie_Level_t Level;
   unsigned ScopeUnsigned;
   bool WriteScope;

   HTM_SELECT_Begin (SubmitOnChange,NULL,
		     "id=\"%s\" name=\"%s\" class=\"REC_C2_BOT_INPUT INPUT_%s\"",
		     ParName,ParName,The_GetSuffix ());

      for (Level  = (Hie_Level_t) 1;
	   Level <= (Hie_Level_t) (Hie_NUM_LEVELS - 1);
	   Level++)
	 if ((Gbl.Scope.Allowed & (1 << Level)))
	   {
	    /* Don't put forbidden options in selectable list */
	    WriteScope = false;
	    switch (Level)
	      {
	       case Hie_SYS:
		  WriteScope = true;
		  break;
	       case Hie_CTY:
	       case Hie_INS:
	       case Hie_CTR:
	       case Hie_DEG:
	       case Hie_CRS:
		  if (Gbl.Hierarchy.Node[Level].HieCod > 0)
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
			   Txt_HIERARCHY_SINGUL_Abc[Level],
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
      Sco_PutParScope ("ScopeUsr",*((Hie_Level_t *) Level));
  }

void Sco_PutParScope (const char *ParName,Hie_Level_t Level)
  {
   Par_PutParUnsigned (NULL,ParName,(unsigned) Level);
  }

/*****************************************************************************/
/*************************** Get parameter scope *****************************/
/*****************************************************************************/

void Sco_GetScope (const char *ParName,Hie_Level_t DefaultScope)
  {
   /***** Get parameter with scope *****/
   Gbl.Scope.Current = (Hie_Level_t)
	               Par_GetParUnsignedLong (ParName,
                                               0,
                                               Hie_NUM_LEVELS - 1,
                                               (unsigned long) Hie_UNK);

   /***** Adjust scope avoiding impossible or forbidden scopes *****/
   Sco_AdjustScope (DefaultScope);
  }

/*****************************************************************************/
/*********** Adjust scope avoiding impossible or forbidden scopes ************/
/*****************************************************************************/

void Sco_AdjustScope (Hie_Level_t DefaultScope)
  {
   Hie_Level_t Level;

   /***** Is scope is unknow, use default scope *****/
   if (Gbl.Scope.Current == Hie_UNK)
      Gbl.Scope.Current = DefaultScope;

   /***** Avoid impossible scopes *****/
   for (Level  = Hie_CRS;
	Level >= Hie_CTY;
	Level--)
      if (Gbl.Scope.Current == Level && Gbl.Hierarchy.Node[Level].HieCod <= 0)
	 Gbl.Scope.Current = Level - 1;	// Go up to parent level

   /***** Avoid forbidden scopes *****/
   if ((Gbl.Scope.Allowed & (1 << Gbl.Scope.Current)) == 0)
      Gbl.Scope.Current = Hie_UNK;
  }

/*****************************************************************************/
/****************** Set allowed scopes when listing guests *******************/
/*****************************************************************************/

void Sco_SetAllowedScopesForListingGuests (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_SYS_ADM:
	 Gbl.Scope.Allowed = 1 << Hie_SYS;
	 break;
      default:
      	 Gbl.Scope.Allowed = 0;
	 break;
     }
  }

/*****************************************************************************/
/**************** Set allowed scopes when listing students *******************/
/*****************************************************************************/

void Sco_SetAllowedScopesForListingStudents (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 Gbl.Scope.Allowed = 1 << Hie_CRS;
	 break;
      case Rol_DEG_ADM:
	 Gbl.Scope.Allowed = 1 << Hie_DEG |
		             1 << Hie_CRS;
	 break;
      case Rol_CTR_ADM:
	 Gbl.Scope.Allowed = 1 << Hie_CTR |
		             1 << Hie_DEG |
		             1 << Hie_CRS;
	 break;
      case Rol_INS_ADM:
	 Gbl.Scope.Allowed = 1 << Hie_INS |
		             1 << Hie_CTR |
		             1 << Hie_DEG |
		             1 << Hie_CRS;
	 break;
      case Rol_SYS_ADM:
	 Gbl.Scope.Allowed = 1 << Hie_SYS |
	                     1 << Hie_CTY |
		             1 << Hie_INS |
		             1 << Hie_CTR |
		             1 << Hie_DEG |
		             1 << Hie_CRS;
	 break;
      default:
      	 Gbl.Scope.Allowed = 0;
	 break;
     }
  }

/*****************************************************************************/
/*********************** Get scope from unsigned string **********************/
/*****************************************************************************/

Hie_Level_t Sco_GetScopeFromUnsignedStr (const char *UnsignedStr)
  {
   unsigned UnsignedNum;

   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Hie_NUM_LEVELS)
         return (Hie_Level_t) UnsignedNum;

   return Hie_UNK;
  }

/*****************************************************************************/
/***************** Get hierarchy level from database string ******************/
/*****************************************************************************/

Hie_Level_t Hie_GetLevelFromDBStr (const char *LevelDBStr)
  {
   Hie_Level_t Level;

   for (Level  = (Hie_Level_t) 0;
	Level <= (Hie_Level_t) (Hie_NUM_LEVELS - 1);
	Level++)
      if (!strcmp (Hie_GetDBStrFromLevel (Level),LevelDBStr))
	 return Level;

   return Hie_UNK;
  }

/*****************************************************************************/
/****************** Get database string from hierarchy level *****************/
/*****************************************************************************/

const char *Hie_GetDBStrFromLevel (Hie_Level_t Level)
  {
   static const char *Sco_ScopeDB[Hie_NUM_LEVELS] =
     {
      [Hie_UNK] = "Unk",
      [Hie_SYS] = "Sys",
      [Hie_CTY] = "Cty",
      [Hie_INS] = "Ins",
      [Hie_CTR] = "Ctr",
      [Hie_DEG] = "Deg",
      [Hie_CRS] = "Crs",
     };

   if (Level >= Hie_NUM_LEVELS)
      Level = Hie_UNK;

   return Sco_ScopeDB[Level];
  }

/*****************************************************************************/
/************************** Get current hierarchy code ***********************/
/*****************************************************************************/

long Hie_GetCurrentCod (void)
  {
   switch (Gbl.Scope.Current)
     {
      case Hie_SYS:
      case Hie_CTY:
      case Hie_INS:
      case Hie_CTR:
      case Hie_DEG:
      case Hie_CRS:
	 return Gbl.Hierarchy.Node[Gbl.Scope.Current].HieCod;
      default:
	 Err_WrongHierarchyLevelExit ();
	 return -1L;	// Not reached
     }
  }
