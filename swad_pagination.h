// swad_pagination.h: pagination of assignments, forums and messages

#ifndef _SWAD_PAG
#define _SWAD_PAG
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_forum.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Pag_ITEMS_PER_PAGE 10

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Pag_NUM_WHAT_PAGINATE 14
typedef enum
  {
   Pag_NONE			=  0,
   Pag_ASSIGNMENTS		=  1,
   Pag_PROJECTS			=  2,
   Pag_EXAMS			=  3,
   Pag_GAMES			=  4,
   Pag_RUBRICS			=  5,
   Pag_ATT_EVENTS		=  6,
   Pag_THREADS_FORUM		=  7,
   Pag_POSTS_FORUM		=  8,
   Pag_MESSAGES_RECEIVED	=  9,
   Pag_MESSAGES_SENT		= 10,
   Pag_SURVEYS			= 11,
   Pag_MY_AGENDA		= 12,
   Pag_ANOTHER_AGENDA		= 13,
  } Pag_WhatPaginate_t;

struct Pag_Pagination	// Used for threads and messages pagination
  {
   unsigned NumItems;
   unsigned StartPage;
   unsigned LeftPage;
   unsigned CurrentPage;	// 1, 2, 3...
   unsigned RightPage;
   unsigned EndPage;
   unsigned NumPags;
   unsigned FirstItemVisible;	// 1, 2, 3...
   unsigned LastItemVisible;	// 1, 2, 3...
   bool MoreThanOnePage;
   char *Anchor;	// Anchor in forms. Use NULL when not needed
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Pag_CalculatePagination (struct Pag_Pagination *Pagination);
void Pag_WriteLinksToPagesCentered (Pag_WhatPaginate_t WhatPaginate,
                                    struct Pag_Pagination *Pagination,
                                    const void *Context,long Cod);
void Pag_WriteLinksToPages (Pag_WhatPaginate_t WhatPaginate,
                            struct Pag_Pagination *Pagination,
                            const void *Context,long Cod,
                            Cns_DisabledOrEnabled_t FirstMsgEnabled,
			    const char *Subject,const char *ClassTxt,
                            bool LinkToPagCurrent);
void Pag_PutParPagNum (Pag_WhatPaginate_t WhatPaginate,unsigned NumPage);
unsigned Pag_GetParPagNum (Pag_WhatPaginate_t WhatPaginate);

unsigned Pag_GetLastPageMsgFromSession (Pag_WhatPaginate_t WhatPaginate);

#endif
