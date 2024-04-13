// swad_closed_open.h: types and constants related to closed/open

#ifndef _SWAD_CLO_OPE
#define _SWAD_CLO_OPE
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#define CloOpe_NUM_CLOSED_OPEN 2

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   CloOpe_CLOSED = 0,
   CloOpe_OPEN   = 1,
  } CloOpe_ClosedOrOpen_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

CloOpe_ClosedOrOpen_t CloOpe_GetClosedOrOpenFrom01 (char Ch);
CloOpe_ClosedOrOpen_t CloOpe_GetClosedOrOpenFromYN (char Ch);

#endif
