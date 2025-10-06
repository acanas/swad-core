// swad_exist.h: types and constants related to existence of something

#ifndef _SWAD_EXI
#define _SWAD_EXI
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Exi_NUM_EXIST 2

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   Exi_DOES_NOT_EXIST  = 0,
   Exi_EXISTS          = 1,
  } Exi_Exist_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

#endif
