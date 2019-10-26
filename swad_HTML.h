// swad_HTML.h: tables, divs

#ifndef _SWAD_HTM
#define _SWAD_HTM
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/********************************* Public types ******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void HTM_TABLE_Begin (const char *fmt,...);
void HTM_TABLE_BeginPadding (unsigned CellPadding);
void HTM_TABLE_BeginCenterPadding (unsigned CellPadding);
void HTM_TABLE_BeginCenter (void);
void HTM_TABLE_BeginWidePadding (unsigned CellPadding);
void HTM_TABLE_BeginWide (void);
void HTM_TABLE_BeginWideMarginPadding (unsigned CellPadding);
void HTM_TABLE_BeginWideMargin (void);
void HTM_TABLE_End (void);

void HTM_TR_Begin (const char *fmt,...);
void HTM_TR_End (void);

void HTM_TH (unsigned RowSpan,unsigned ColSpan,const char *Class,const char *Txt);
void HTM_TH_Begin (unsigned RowSpan,unsigned ColSpan,const char *Class);
void HTM_TH_End (void);
void HTM_TH_Empty (unsigned NumColumns);

void HTM_TD_Begin (const char *fmt,...);
void HTM_TD_End (void);
void HTM_TD_Empty (unsigned NumColumns);
void HTM_TD_ColouredEmpty (unsigned NumColumns);

void HTM_DIV_Begin (const char *fmt,...);
void HTM_DIV_End (void);

void HTM_MAIN_Begin (const char *Class);
void HTM_MAIN_End (void);

void HTM_ARTICLE_Begin (const char *ArticleId);
void HTM_ARTICLE_End (void);

void HTM_SECTION_Begin (const char *SectionId);
void HTM_SECTION_End (void);

void HTM_UL_Begin (const char *fmt,...);
void HTM_UL_End (void);

#endif
