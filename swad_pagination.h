// swad_pagination.h: pagination of assignments, forums and messages

#ifndef _SWAD_PAG
#define _SWAD_PAG
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

typedef enum
  {
   Pag_ASSIGNMENTS,
   Pag_USR_AGENDA,
   Pag_ATT_EVENTS,
   Pag_MESSAGES_RECEIVED,
   Pag_MESSAGES_SENT,
   Pag_THREADS_FORUM,
   Pag_POSTS_FORUM,
   Pag_SURVEYS,
   Pag_MY_AGENDA,
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
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Pag_CalculatePagination (struct Pagination *Pagination);
void Pag_WriteLinksToPagesCentered (Pag_WhatPaginate_t WhatPaginate,long ThrCod,struct Pagination *Pagination);
void Pag_WriteLinksToPages (Pag_WhatPaginate_t WhatPaginate,long ThrCod,struct Pagination *Pagination,
                            bool FirstMsgEnabled,const char *Subject,const char *Font,bool LinkToPagCurrent);
void Pag_PutHiddenParamPagNum (unsigned NumPage);
void Pag_GetParamPagNum (Pag_WhatPaginate_t WhatPaginate);

void Pag_SaveLastPageMsgIntoSession (Pag_WhatPaginate_t WhatPaginate,unsigned NumPage);
unsigned Pag_GetLastPageMsgFromSession (Pag_WhatPaginate_t WhatPaginate);

#endif
