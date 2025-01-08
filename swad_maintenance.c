// swad_maintenance.c: platform maintenance

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_action_list.h"
#include "swad_course.h"
#include "swad_database.h"
#include "swad_match_print.h"
#include "swad_menu.h"

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Mtn_PutLinkToSetUp (void);

/*****************************************************************************/
/******************************** Maintenance ********************************/
/*****************************************************************************/

void Mtn_Maintenance (void)
  {
   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
      Mtn_PutLinkToSetUp ();				// Set up
      Crs_PutLinkToRemoveOldCrss ();			// Remove old courses
   Mnu_ContextMenuEnd ();
  }

/*****************************************************************************/
/************************ Put link to set up platform ************************/
/*****************************************************************************/

static void Mtn_PutLinkToSetUp (void)
  {
   extern const char *Txt_Set_up;

   /***** Put form to set up platform *****/
   Lay_PutContextualLinkIconText (ActSetUp,NULL,
                                  NULL,NULL,
				  "bolt.svg",Ico_BLACK,
				  Txt_Set_up,NULL);
  }

/*****************************************************************************/
/****************************** Initial set up *******************************/
/*****************************************************************************/

void Mtn_SetUp (void)
  {
   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
      Crs_PutLinkToRemoveOldCrss ();	// Remove old courses
   Mnu_ContextMenuEnd ();

   /***** Create database tables if not exist *****/
   DB_CreateTablesIfNotExist ();
  }

/*****************************************************************************/
/****************************** Initial set up *******************************/
/*****************************************************************************/

void Mtn_RemoveOldCrss (void)
  {
   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
      Mtn_PutLinkToSetUp ();		// Set up
   Mnu_ContextMenuEnd ();

   /***** Write form to remove old courses *****/
   Crs_AskRemoveOldCrss ();
  }
