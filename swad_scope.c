// swad_scope.c: scope (platform, center, degree, course...)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

void Sco_PutSelectorScope (const char *ParName,HTM_Attributes_t Attributes,
			   Hie_Level_t HieLvl,unsigned AllowedLvls)
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   Hie_Level_t L;
   unsigned ScopeUnsigned;
   Sho_Show_t ShowScope;

   HTM_SELECT_Begin (Attributes,NULL,
		     "id=\"%s\" name=\"%s\""
		     " class=\"Frm_C2_INPUT INPUT_%s\"",
		     ParName,ParName,The_GetSuffix ());

      for (L  = Hie_SYS;
	   L <= Hie_CRS;
	   L++)
	 if ((AllowedLvls & (1 << L)))
	   {
	    /* Don't put forbidden options in selectable list */
	    ShowScope = Sho_DONT_SHOW;
	    switch (L)
	      {
	       case Hie_SYS:
		  ShowScope = Sho_SHOW;
		  break;
	       case Hie_CTY:
	       case Hie_INS:
	       case Hie_CTR:
	       case Hie_DEG:
	       case Hie_CRS:
		  if (Gbl.Hierarchy.Node[L].HieCod > 0)
		     ShowScope = Sho_SHOW;
		  break;
	       default:
		  break;
	      }

	    if (ShowScope == Sho_SHOW)
	      {
	       /***** Write allowed option *****/
	       ScopeUnsigned = (unsigned) L;
	       HTM_OPTION (HTM_Type_UNSIGNED,&ScopeUnsigned,
			   (L == HieLvl) ? HTM_SELECTED :
					   HTM_NO_ATTR,
			   "%s: %s",
			   Txt_HIERARCHY_SINGUL_Abc[L],
			   Gbl.Hierarchy.Node[L].ShrtName);
	      }
	   }

   HTM_SELECT_End ();
  }

/*****************************************************************************/
/********************** Put hidden parameter scope ***************************/
/*****************************************************************************/

void Sco_PutParCurrentScope (void *HieLvl)
  {
   if (HieLvl)
      Sco_PutParScope ("ScopeUsr",*((Hie_Level_t *) HieLvl));
  }

void Sco_PutParScope (const char *ParName,Hie_Level_t HieLvl)
  {
   Par_PutParUnsigned (NULL,ParName,(unsigned) HieLvl);
  }

/*****************************************************************************/
/*************************** Get parameter scope *****************************/
/*****************************************************************************/

Hie_Level_t Sco_GetScope (const char *ParName,
			  Hie_Level_t DefaultHieLvl,unsigned AllowedLvls)
  {
   Hie_Level_t HieLvl;

   /***** Get parameter with scope *****/
   HieLvl = (Hie_Level_t) Par_GetParUnsignedLong (ParName,
                                                  0,
                                                  Hie_NUM_LEVELS - 1,
                                                  (unsigned long) Hie_UNK);

   /***** Adjust scope avoiding impossible or forbidden scopes *****/
   Sco_AdjustScope (&HieLvl,AllowedLvls,DefaultHieLvl);

   return HieLvl;
  }

/*****************************************************************************/
/*********** Adjust scope avoiding impossible or forbidden scopes ************/
/*****************************************************************************/

void Sco_AdjustScope (Hie_Level_t *HieLvl,unsigned AllowedLvls,Hie_Level_t DefaultHieLvl)
  {
   Hie_Level_t L;

   /***** Is scope is unknow, use default scope *****/
   if (*HieLvl == Hie_UNK)
      *HieLvl = DefaultHieLvl;

   /***** Avoid impossible scopes *****/
   for (L  = Hie_CRS;
	L >= Hie_CTY;
	L--)
      if (*HieLvl == L && Gbl.Hierarchy.Node[L].HieCod <= 0)
	 *HieLvl = L - 1;	// Go up to parent level

   /***** Avoid forbidden scopes *****/
   if ((AllowedLvls & (1 << *HieLvl)) == 0)
      *HieLvl = Hie_UNK;
  }

/*****************************************************************************/
/******************** Allowed scopes when listing guests *********************/
/*****************************************************************************/

unsigned Sco_GetAllowedScopesForListingGuests (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_SYS_ADM:
	 return 1 << Hie_SYS;
      default:
      	 return 0;
     }
  }

/*****************************************************************************/
/****************** Allowed scopes when listing students *********************/
/*****************************************************************************/

unsigned Sco_GetAllowedScopesForListingStudents (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 return 1 << Hie_CRS;
      case Rol_DEG_ADM:
	 return 1 << Hie_DEG |
		1 << Hie_CRS;
      case Rol_CTR_ADM:
	 return 1 << Hie_CTR |
		1 << Hie_DEG |
		1 << Hie_CRS;
      case Rol_INS_ADM:
	 return 1 << Hie_INS |
		1 << Hie_CTR |
		1 << Hie_DEG |
		1 << Hie_CRS;
      case Rol_SYS_ADM:
	 return 1 << Hie_SYS |
	        1 << Hie_CTY |
		1 << Hie_INS |
		1 << Hie_CTR |
		1 << Hie_DEG |
		1 << Hie_CRS;
      default:
      	 return 0;
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
