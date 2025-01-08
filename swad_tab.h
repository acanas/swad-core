// swad_tab.h: tabs drawing

#ifndef _SWAD_TAB
#define _SWAD_TAB
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

#define Tab_NUM_TABS	(1 + 13)

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   TabUnk =  0,
   TabStr =  1,
   TabSys =  2,
   TabCty =  3,
   TabIns =  4,
   TabCtr =  5,
   TabDeg =  6,
   TabCrs =  7,
   TabAss =  8,
   TabFil =  9,
   TabUsr = 10,
   TabMsg = 11,
   TabAna = 12,
   TabPrf = 13,
  } Tab_Tab_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Tab_DrawTabs (void);

const char *Tab_GetTxt (Tab_Tab_t NumTab);

void Tab_SetCurrentTab (void);
void Tab_DisableIncompatibleTabs (void);

#endif
