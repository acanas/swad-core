// swad_program.h: course program

#ifndef _SWAD_PRG
#define _SWAD_PRG
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include <stdbool.h>		// For boolean type
#include <time.h>		// For time

#include "swad_database.h"
#include "swad_date.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Prg_MAX_CHARS_PROGRAM_ITEM_TITLE	(128 - 1)	// 127
#define Prg_MAX_BYTES_PROGRAM_ITEM_TITLE	((Prg_MAX_CHARS_PROGRAM_ITEM_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

struct Prg_ItemHierarchy
  {
   long ItmCod;
   unsigned Index;
   unsigned Level;
   bool Hidden;
  };

struct Prg_Item
  {
   struct Prg_ItemHierarchy Hierarchy;
   unsigned NumItem;
   long UsrCod;
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   bool Open;
   char Title[Prg_MAX_BYTES_PROGRAM_ITEM_TITLE + 1];
  };

struct Prg_ItemRange
  {
   unsigned Begin;	// Index of the first item in the subtree
   unsigned End;	// Index of the last item in the subtree
  };

#define Prg_NUM_MOVEMENTS_LEFT_RIGHT 2
typedef enum
  {
   Prg_MOVE_LEFT,
   Prg_MOVE_RIGHT,
  } Prg_MoveLeftRight_t;

#define Prg_NUM_LISTING_TYPES 7
typedef enum
  {
   Prg_PRINT,		// List items ready to be printed
   Prg_VIEW,		// List items without any edition
   Prg_EDIT_LIST,	// Buttons to edit list of items
   Prg_NEW_ITEM,	// Form to create a new item
   Prg_EDIT_ITEM,	// Form to edit a selected item
   Prg_EDIT_RESOURCES,	// List resources of a selected item for edition
   Prg_END_EDIT_RES,	// List resources of a selected item after edition
  } Prg_ListingType_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Prg_ShowCourseProgram (void);
void Prg_EditCourseProgram (void);
void Prg_EditCourseProgramHighlightingItem (const struct Prg_ItemRange *ToHighlight);

void Prg_ShowAllItems (Prg_ListingType_t ListingType,
                       const struct Prg_ItemRange *ToHighlight,
                       long ParentItmCod,long ItmCod,unsigned FormLevel);

bool Prg_CheckIfICanEditProgram (void);
void Prg_PutParams (void *ItmCod);

void Prg_GetListItems (void);
void Prg_FreeListItems (void);
long Prg_GetParamItmCod (void);
unsigned Prg_GetNumItemFromItmCod (long ItmCod);
unsigned Prg_GetLevelFromNumItem (unsigned NumItem);

void Prg_RequestCreateItem (void);
void Prg_RequestChangeItem (void);
void Prg_ReceiveFormNewItem (void);
void Prg_ReceiveFormChgItem (void);

void Prg_ReqRemItem (void);
void Prg_RemoveItem (void);
void Prg_HideItem (void);
void Prg_UnhideItem (void);

void Prg_MoveUpItem (void);
void Prg_MoveDownItem (void);
void Prg_MoveLeftItem (void);
void Prg_MoveRightItem (void);

//-------------------------------- Figures ------------------------------------
void Prg_GetAndShowCourseProgramStats (void); // TODO: Change function from assignments to schedule

#endif
