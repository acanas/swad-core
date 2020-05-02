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
   FigCch_NUM_CTYS		=  1,	// Number of countries
   FigCch_NUM_INSS		=  2,	// Number of institutions
   FigCch_NUM_CTRS		=  3,	// Number of centres
   FigCch_NUM_CTRS_WITH_MAP	=  4,	// Number of centres with map
   FigCch_NUM_DEGS		=  5,	// Number of degrees
   FigCch_NUM_CRSS		=  6,	// Number of courses
   //--------------------------------------------------------------------------
   FigCch_NUM_STDS_IN_CRSS	=  7,	// Number of students in courses
   FigCch_NUM_NETS_IN_CRSS	=  8,	// Number of non-editing teachers in courses
   FigCch_NUM_TCHS_IN_CRSS	=  9,	// Number of teachers in courses
   FigCch_NUM_USRS_IN_CRSS	= 10,	// Number of users in courses
   FigCch_NUM_GSTS		= 11,	// Number of guests (users not belonging to any course)
   //--------------------------------------------------------------------------
   FigCch_NUM_CRSS_PER_USR	= 12,	// Number of courses per user
   FigCch_NUM_CRSS_PER_STD	= 13,	// Number of courses per student
   FigCch_NUM_CRSS_PER_NET	= 14,	// Number of courses per non-editing teacher
   FigCch_NUM_CRSS_PER_TCH	= 15,	// Number of courses per teacher
   //--------------------------------------------------------------------------
   FigCch_NUM_USRS_PER_CRS	= 16,	// Number of users per course
   FigCch_NUM_STDS_PER_CRS	= 17,	// Number of students per course
   FigCch_NUM_NETS_PER_CRS	= 18,	// Number of non-editing teachers per course
   FigCch_NUM_TCHS_PER_CRS	= 19,	// Number of teachers per course
   //--------------------------------------------------------------------------
   FigCch_NUM_USRS_CTY		= 20,	// Number of users who claim to belong to country
  } FigCch_FigureCached_t;

#define FigCch_NUM_TYPES 2
typedef enum
  {
   FigCch_Type_UNSIGNED,
   FigCch_Type_DOUBLE,
  } FigCch_Type_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void FigCch_UpdateFigureIntoCache (FigCch_FigureCached_t Figure,
                                   Hie_Level_t Scope,long Cod,
                                   FigCch_Type_t Type,const void *ValuePtr);
bool FigCch_GetFigureFromCache (FigCch_FigureCached_t Figure,
                                Hie_Level_t Scope,long Cod,
                                FigCch_Type_t Type,void *ValuePtr);

#endif
