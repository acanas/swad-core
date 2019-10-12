// swad_table.h: tables

#ifndef _SWAD_TBL
#define _SWAD_TBL
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

void Tbl_TABLE_Begin (const char *fmt,...);
void Tbl_TABLE_BeginPadding (unsigned CellPadding);
void Tbl_TABLE_BeginWithoutAttr (void);
void Tbl_TABLE_BeginCenterPadding (unsigned CellPadding);
void Tbl_TABLE_BeginCenter (void);
void Tbl_TABLE_BeginWidePadding (unsigned CellPadding);
void Tbl_TABLE_BeginWide (void);
void Tbl_TABLE_BeginWideMarginPadding (unsigned CellPadding);
void Tbl_TABLE_BeginWideMargin (void);
void Tbl_TABLE_End (void);

void Tbl_TR_Begin (const char *fmt,...);
void Tbl_TR_End (void);

void Tbl_TH_Begin (unsigned RowSpan,unsigned ColSpan,const char *Class);
void Tbl_TH_End (void);
void Tbl_TH_Empty (unsigned NumColumns);

void Tbl_TD_Begin (const char *fmt,...);
void Tbl_TD_End (void);
void Tbl_TD_Empty (unsigned NumColumns);
void Tbl_TD_ColouredEmpty (unsigned NumColumns);

#endif
