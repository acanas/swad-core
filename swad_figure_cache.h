// swad_figure_cache.c: figures (global stats) cached in database

#ifndef _SWAD_FIG_CCH
#define _SWAD_FIG_CCH
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_hierarchy_type.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

// If numbers change, clean database table figures: "DELETE FROM figures;"
#define FigCch_NUM_FIGURES 2
typedef enum
  {
   FigCch_UNKNOWN		=  0,	// Unknown figure (do not change this constant to any value other than 0)
   //--------------------------------------------------------------------------
   FigCch_NUM_CTYS		=  1,	// Number of countries
   FigCch_NUM_INSS		=  2,	// Number of institutions
   FigCch_NUM_CTRS		=  3,	// Number of centers
   FigCch_NUM_CTRS_WITH_MAP	=  4,	// Number of centers with map
   FigCch_NUM_DEGS		=  5,	// Number of degrees
   FigCch_NUM_CRSS		=  6,	// Number of courses
   //--------------------------------------------------------------------------
   FigCch_NUM_CTYS_WITH_INSS	=  7,	// Number of countries with institutions
   FigCch_NUM_CTYS_WITH_CTRS	=  8,	// Number of countries with centers
   FigCch_NUM_CTYS_WITH_DEGS	=  9,	// Number of countries with degrees
   FigCch_NUM_CTYS_WITH_CRSS	= 10,	// Number of countries with courses
   FigCch_NUM_CTYS_WITH_TCHS	= 11,	// Number of countries with teachers
   FigCch_NUM_CTYS_WITH_NETS	= 12,	// Number of countries with non-editing teachers
   FigCch_NUM_CTYS_WITH_STDS	= 13,	// Number of countries with students
   //--------------------------------------------------------------------------
   FigCch_NUM_INSS_WITH_CTRS	= 14,	// Number of institutions with centers
   FigCch_NUM_INSS_WITH_DEGS	= 15,	// Number of institutions with degrees
   FigCch_NUM_INSS_WITH_CRSS	= 16,	// Number of institutions with courses
   FigCch_NUM_INSS_WITH_TCHS	= 17,	// Number of institutions with teachers
   FigCch_NUM_INSS_WITH_NETS	= 18,	// Number of institutions with non-editing teachers
   FigCch_NUM_INSS_WITH_STDS	= 19,	// Number of institutions with students
   //--------------------------------------------------------------------------
   FigCch_NUM_CTRS_WITH_DEGS	= 20,	// Number of centers with degrees
   FigCch_NUM_CTRS_WITH_CRSS	= 21,	// Number of centers with courses
   FigCch_NUM_CTRS_WITH_TCHS	= 22,	// Number of centers with teachers
   FigCch_NUM_CTRS_WITH_NETS	= 23,	// Number of centers with non-editing teachers
   FigCch_NUM_CTRS_WITH_STDS	= 24,	// Number of centers with students
   //--------------------------------------------------------------------------
   FigCch_NUM_DEGS_WITH_CRSS	= 25,	// Number of degrees with courses
   FigCch_NUM_DEGS_WITH_TCHS	= 26,	// Number of degrees with teachers
   FigCch_NUM_DEGS_WITH_NETS	= 27,	// Number of degrees with non-editing teachers
   FigCch_NUM_DEGS_WITH_STDS	= 28,	// Number of degrees with students
   //--------------------------------------------------------------------------
   FigCch_NUM_CRSS_WITH_TCHS	= 29,	// Number of courses with teachers
   FigCch_NUM_CRSS_WITH_NETS	= 30,	// Number of courses with non-editing teachers
   FigCch_NUM_CRSS_WITH_STDS	= 31,	// Number of courses with students
   //--------------------------------------------------------------------------
   FigCch_NUM_STDS_IN_CRSS	= 32,	// Number of students in courses
   FigCch_NUM_NETS_IN_CRSS	= 33,	// Number of non-editing teachers in courses
   FigCch_NUM_TCHS_IN_CRSS	= 34,	// Number of teachers in courses
   FigCch_NUM_ALLT_IN_CRSS	= 35,	// Number of non-editing teachers or teachers in courses
   FigCch_NUM_USRS_IN_CRSS	= 36,	// Number of users in courses
   FigCch_NUM_GSTS		= 37,	// Number of guests (users not belonging to any course)
   //--------------------------------------------------------------------------
   FigCch_NUM_CRSS_PER_USR	= 38,	// Number of courses per user
   FigCch_NUM_CRSS_PER_STD	= 39,	// Number of courses per student
   FigCch_NUM_CRSS_PER_NET	= 40,	// Number of courses per non-editing teacher
   FigCch_NUM_CRSS_PER_TCH	= 41,	// Number of courses per teacher
   //--------------------------------------------------------------------------
   FigCch_NUM_USRS_PER_CRS	= 42,	// Number of users per course
   FigCch_NUM_STDS_PER_CRS	= 43,	// Number of students per course
   FigCch_NUM_NETS_PER_CRS	= 44,	// Number of non-editing teachers per course
   FigCch_NUM_TCHS_PER_CRS	= 45,	// Number of teachers per course
   //--------------------------------------------------------------------------
   FigCch_NUM_USRS_BELONG_CTY	= 46,	// Number of users who claim to belong to country
   FigCch_NUM_USRS_BELONG_INS	= 47,	// Number of users who claim to belong to institution
   FigCch_NUM_USRS_BELONG_CTR	= 48,	// Number of users who claim to belong to center
  } FigCch_FigureCached_t;

#define FigCch_NUM_TYPES 2
typedef enum
  {
   FigCch_UNSIGNED,
   FigCch_DOUBLE,
  } FigCch_Type_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void FigCch_UpdateFigureIntoCache (FigCch_FigureCached_t Figure,
                                   Hie_Level_t HieLvl,long HieCod,
                                   FigCch_Type_t Type,const void *ValuePtr);
Exi_Exist_t FigCch_GetFigureFromCache (FigCch_FigureCached_t Figure,
				       Hie_Level_t HieLvl,long HieCod,
				       FigCch_Type_t Type,void *ValuePtr);

#endif
