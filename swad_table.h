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

void Tbl_StartTableClass (const char *fmt,...);
void Tbl_StartTablePadding (unsigned CellPadding);
void Tbl_StartTable (void);
void Tbl_StartTableCenterPadding (unsigned CellPadding);
void Tbl_StartTableCenter (void);
void Tbl_StartTableWidePadding (unsigned CellPadding);
void Tbl_StartTableWide (void);
void Tbl_StartTableWideMarginPadding (unsigned CellPadding);
void Tbl_StartTableWideMargin (void);
void Tbl_EndTable (void);

void Tbl_StartRowAttr (const char *fmt,...);
void Tbl_StartRow (void);
void Tbl_EndRow (void);

void Tbl_EndCell (void);
void Tbl_PutEmptyCells (unsigned NumColumns);
void Tbl_PutEmptyColouredCells (unsigned NumColumns);

#endif
