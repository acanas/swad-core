// swad_theme.h: themes (colour layouts)

#ifndef _SWAD_THE
#define _SWAD_THE
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

#define The_NUM_THEMES 7

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   The_THEME_WHITE   = 0,
   The_THEME_GREY    = 1,
   The_THEME_PURPLE  = 2,
   The_THEME_BLUE    = 3,
   The_THEME_YELLOW  = 4,
   The_THEME_PINK    = 5,
   The_THEME_DARK    = 6,
   } The_Theme_t;
#define The_THEME_DEFAULT The_THEME_GREY

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void The_PutIconsToSelectTheme (void);
void The_ChangeTheme (void);
The_Theme_t The_GetParTheme (void);
The_Theme_t The_GetThemeFromStr (const char *Str);

const char *The_GetSuffix (void);

const char *The_GetColorRows (void);
const char *The_GetColorRows1 (unsigned Level);

void The_ResetRowColor (void);
void The_ResetRowColor1 (unsigned Level);

void The_ChangeRowColor (void);
void The_ChangeRowColor1 (unsigned Level);

//-------------------------------- Figures ------------------------------------
void The_GetAndShowNumUsrsPerTheme (Hie_Level_t HieLvl);

#endif
