// swad_hierarchy_level.h: hierarchy levels: system, institution, centre, degree, course

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
#define Hie_Lvl_NUM_LEVELS	7
typedef enum
  {
   Hie_Lvl_UNK = 0,	// Unknown
   Hie_Lvl_SYS = 1,	// System
   Hie_Lvl_CTY = 2,	// Country
   Hie_Lvl_INS = 3,	// Institution
   Hie_Lvl_CTR = 4,	// Centre
   Hie_Lvl_DEG = 5,	// Degree
   Hie_Lvl_CRS = 6,	// Course
  } Hie_Lvl_Level_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

#endif
