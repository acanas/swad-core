// swad_hierarchy_level.h: hierarchy levels: system, institution, center, degree, course

#ifndef _SWAD_HIE_LVL
#define _SWAD_HIE_LVL
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

// Levels in the hierarchy
#define HieLvl_NUM_LEVELS	7
typedef enum
  {
   HieLvl_UNK = 0,	// Unknown
   HieLvl_SYS = 1,	// System
   HieLvl_CTY = 2,	// Country
   HieLvl_INS = 3,	// Institution
   HieLvl_CTR = 4,	// Center
   HieLvl_DEG = 5,	// Degree
   HieLvl_CRS = 6,	// Course
  } HieLvl_Level_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

#endif
