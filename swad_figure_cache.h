// swad_figure_cache.c: figures (global stats) cached in database

#ifndef _SWAD_FIG_CCH
#define _SWAD_FIG_CCH
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

#include <stdbool.h>		// For boolean type

#include "swad_hierarchy.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

// If numbers change, clean database table figures: "DELETE FROM figures;"
typedef enum
  {
   FigCch_UNKNOWN		=  0,	// Unknown figure
   //--------------------------------------------------------------------------
   FigCch_NUM_STDS_IN_CRSS	=  1,	// Number of students in courses
   FigCch_NUM_NETS_IN_CRSS	=  2,	// Number of non-editing teachers in courses
   FigCch_NUM_TCHS_IN_CRSS	=  3,	// Number of teachers in courses
   FigCch_NUM_USRS_IN_CRSS	=  4,	// Number of users in courses
   //--------------------------------------------------------------------------
   FigCch_NUM_CTYS		=  5,	// Number of countries
   FigCch_NUM_INSS		=  6,	// Number of institutions
   FigCch_NUM_CTRS		=  7,	// Number of centres
   FigCch_NUM_CTRS_WITH_MAP	=  8,	// Number of centres with map
   FigCch_NUM_DEGS		=  9,	// Number of degrees
   FigCch_NUM_CRSS		= 10,	// Number of courses
  } FigCch_FigureCached_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void FigCch_UpdateFigureIntoCache (FigCch_FigureCached_t Figure,
                                   Hie_Level_t Scope,long Cod,
                                   unsigned Value);
bool FigCch_GetFigureFromCache (FigCch_FigureCached_t Figure,
                                Hie_Level_t Scope,long Cod,
                                unsigned *Value);

#endif
