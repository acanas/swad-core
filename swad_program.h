// swad_program.h: course program

#ifndef _SWAD_PRG
#define _SWAD_PRG
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

#include "swad_date.h"
#include "swad_file_browser.h"
#include "swad_notification.h"
#include "swad_user.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Prg_MAX_CHARS_PROGRAM_ITEM_TITLE	(128 - 1)	// 127
#define Prg_MAX_BYTES_PROGRAM_ITEM_TITLE	((Prg_MAX_CHARS_PROGRAM_ITEM_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

struct ProgramItemHierarchy
  {
   long ItmCod;
   unsigned Index;
   unsigned Level;
   bool Hidden;
  };

struct ProgramItem
  {
   struct ProgramItemHierarchy Hierarchy;
   bool Hidden;
   long UsrCod;
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   bool Open;
   char Title[Prg_MAX_BYTES_PROGRAM_ITEM_TITLE + 1];
   bool IBelongToCrsOrGrps;	// I can do this program item
				// (it is associated to no groups
				// or, if associated to groups,
				// I belong to any of the groups)
  };

#define Prg_ORDER_DEFAULT Dat_START_TIME

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Prg_SeeCourseProgram (void);

void Prg_RequestCreatOrEditPrgItem (void);
void Prg_GetDataOfItemByCod (struct ProgramItem *PrgItem);
void Prg_FreeListItems (void);

long Prg_GetParamItemCode (void);

void Prg_ReqRemPrgItem (void);
void Prg_RemovePrgItem (void);
void Prg_HidePrgItem (void);
void Prg_ShowPrgItem (void);

void Prg_MoveUpPrgItem (void);
void Prg_MoveDownPrgItem (void);
void Prg_MoveLeftPrgItem (void);
void Prg_MoveRightPrgItem (void);

void Prg_RecFormPrgItem (void);
bool Prg_CheckIfItemIsAssociatedToGrp (long PrgItmCod,long GrpCod);
void Prg_RemoveGroup (long GrpCod);
void Prg_RemoveGroupsOfType (long GrpTypCod);
void Prg_RemoveCrsItems (long CrsCod);
unsigned Prg_GetNumItemsInCrsProgram(long CrsCod);

unsigned Prg_GetNumCoursesWithItems (Hie_Level_t Scope);
unsigned Prg_GetNumItems (Hie_Level_t Scope);

#endif
