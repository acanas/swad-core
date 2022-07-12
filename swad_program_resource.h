// swad_program_resource.h: course program (resources)

#ifndef _SWAD_PRG_RSC
#define _SWAD_PRG_RSC
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

// #include <stdbool.h>		// For boolean type
// #include <time.h>		// For time

// #include "swad_database.h"
// #include "swad_date.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define PrgRsc_MAX_CHARS_PROGRAM_RESOURCE_TITLE	(128 - 1)	// 127
#define PrgRsc_MAX_BYTES_PROGRAM_RESOURCE_TITLE	((PrgRsc_MAX_CHARS_PROGRAM_RESOURCE_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047
/*
struct Prg_ItemHierarchy
  {
   long ItmCod;
   unsigned Index;
   unsigned Level;
   bool Hidden;
  };
*/
struct PrgRsc_Resource
  {
   long ItmCod;
   long RscCod;
   bool Hidden;
   char Title[PrgRsc_MAX_BYTES_PROGRAM_RESOURCE_TITLE + 1];
  };
/*
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
*/
/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void PrgRsc_ShowAllResources (long ItmCod);
void PrgRsc_RequestCreateResource (void);

#endif
