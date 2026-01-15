// swad_language.h: user's settings on language

#ifndef _SWAD_LAN
#define _SWAD_LAN
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
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Lan_NUM_LANGUAGES 10	// From 1 to 10. 0 is reserved to unknown language

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   Lan_LANGUAGE_UNKNOWN =  0,
   Lan_LANGUAGE_CA      =  1,
   Lan_LANGUAGE_DE      =  2,
   Lan_LANGUAGE_EN      =  3,
   Lan_LANGUAGE_ES      =  4,
   Lan_LANGUAGE_FR      =  5,
   Lan_LANGUAGE_GN      =  6,
   Lan_LANGUAGE_IT      =  7,
   Lan_LANGUAGE_PL      =  8,
   Lan_LANGUAGE_PT      =  9,
   Lan_LANGUAGE_TR      = 10,
  } Lan_Language_t; // ISO 639-1 language codes

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Lan_PutLinkToChangeLanguage (void);

void Lan_PutBoxToSelectLanguage (void);
void Lan_PutSelectorToSelectLanguage (void);

void Lan_AskChangeLanguage (void);
void Lan_ChangeLanguage (void);
void Lan_UpdateMyLanguageToCurrentLanguage (void);

Lan_Language_t Lan_GetParLanguage (void);
Lan_Language_t Lan_GetLanguageFromStr (const char *Str);

//-------------------------------- Figures ------------------------------------
void Lan_GetAndShowNumUsrsPerLanguage (Hie_Level_t HieLvl);

#endif
