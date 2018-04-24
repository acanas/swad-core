// swad_text.h: text messages on screen, in several languages

#ifndef _SWAD_TXT
#define _SWAD_TXT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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
/********************************** Headers **********************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Txt_NUM_LANGUAGES 9	// From 1 to 9. 0 is reserved to unknown language

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

typedef enum
  {
   Txt_LANGUAGE_UNKNOWN = 0,
   Txt_LANGUAGE_CA = 1,
   Txt_LANGUAGE_DE = 2,
   Txt_LANGUAGE_EN = 3,
   Txt_LANGUAGE_ES = 4,
   Txt_LANGUAGE_FR = 5,
   Txt_LANGUAGE_GN = 6,
   Txt_LANGUAGE_IT = 7,
   Txt_LANGUAGE_PL = 8,
   Txt_LANGUAGE_PT = 9,
  } Txt_Language_t; // ISO 639-1 language codes

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

#endif
