// swad_view.h: types and constants related to view/edit/print

#ifndef _SWAD_VIEW
#define _SWAD_VIEW
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
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Vie_NUM_VIEW_TYPES 4

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   Vie_VIEW	= 0,
   Vie_EDIT	= 1,
   Vie_CONFIG	= 2,
   Vie_PRINT	= 3,
  } Vie_ViewType_t;

#define Vie_NUM_USRS_VIEW_TYPES 4
typedef enum
  {
   Vie_VIEW_ONLY_ME,	// View only me
   Vie_VIEW_SEL_USR,	// View selected users
   Vie_PRNT_ONLY_ME,	// Print only me
   Vie_PRNT_SEL_USR,	// Print selected users
  } Vie_UsrsViewType_t;

#endif
