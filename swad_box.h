// swad_box.h: layout of boxes

#ifndef _SWAD_BOX
#define _SWAD_BOX
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <stdbool.h>		// For boolean type

#include "swad_button.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Box_MAX_NESTED 5

/*****************************************************************************/
/********************************* Public types ******************************/
/*****************************************************************************/

typedef enum
  {
   Box_NOT_CLOSABLE,
   Box_CLOSABLE,
  } Box_Closable_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Box_BoxTableBegin (const char *Title,
                        void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                        const char *HelpLink,Box_Closable_t Closable,
                        unsigned CellPadding);
void Box_BoxBegin (const char *Title,
                   void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                   const char *HelpLink,Box_Closable_t Closable);
void Box_BoxShadowBegin (const char *Title,
                         void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                         const char *HelpLink);
void Box_BoxTableShadowBegin (const char *Title,
                              void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                              const char *HelpLink,
                              unsigned CellPadding);
void Box_BoxTableEnd (void);
void Box_BoxTableWithButtonEnd (Btn_Button_t Button,const char *TxtButton);
void Box_BoxWithButtonEnd (Btn_Button_t Button,const char *TxtButton);
void Box_BoxEnd (void);

#endif
