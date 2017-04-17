// swad_pagination.h: pagination of assignments, forums and messages

#ifndef _SWAD_PAG
#define _SWAD_PAG
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#define Pag_ITEMS_PER_PAGE 10

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Pag_NUM_WHAT_PAGINATE 9
typedef enum
  {
   Pag_ASSIGNMENTS		= 0,
   Pag_ATT_EVENTS		= 1,
   Pag_THREADS_FORUM		= 2,
   Pag_POSTS_FORUM		= 3,
   Pag_MESSAGES_RECEIVED	= 4,
   Pag_MESSAGES_SENT		= 5,
   Pag_SURVEYS			= 6,
   Pag_MY_AGENDA		= 7,
   Pag_ANOTHER_AGENDA		= 8,
  } Pag_WhatPaginate_t;

struct Pagination	// Used for threads and messages pagination
  {
   unsigned NumItems;
   int StartPage;
   int LeftPage;
   int CurrentPage;	// 1, 2, 3...
   int RightPage;
   int EndPage;
   int NumPags;
   unsigned FirstItemVisible;	// 1, 2, 3...
   unsigned LastItemVisible;	// 1, 2, 3...
   bool MoreThanOnePage;
   char *Anchor;	// Anchor in forms. Use NULL when not needed
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Pag_CalculatePagination (struct Pagination *Pagination);
void Pag_WriteLinksToPagesCentered (Pag_WhatPaginate_t WhatPaginate,
                                    long ThrCod,
                                    struct Pagination *Pagination);
void Pag_WriteLinksToPages (Pag_WhatPaginate_t WhatPaginate,
                            long ThrCod,
                            struct Pagination *Pagination,
                            bool FirstMsgEnabled,const char *Subject,const char *Font,
                            bool LinkToPagCurrent);
void Pag_PutHiddenParamPagNum (Pag_WhatPaginate_t WhatPaginate,unsigned NumPage);
unsigned Pag_GetParamPagNum (Pag_WhatPaginate_t WhatPaginate);

void Pag_SaveLastPageMsgIntoSession (Pag_WhatPaginate_t WhatPaginate,unsigned NumPage);
unsigned Pag_GetLastPageMsgFromSession (Pag_WhatPaginate_t WhatPaginate);

#endif
