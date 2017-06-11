// swad_table.c: tables

/*
    SWAD (Shared Workspace At a Distance),
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <stdio.h>		// For fprintf

#include "swad_table.h"
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Start/end table *****************************/
/*****************************************************************************/

void Tbl_StartTable (unsigned CellPadding)
  {
   fprintf (Gbl.F.Out,"<table");
   if (CellPadding)
      fprintf (Gbl.F.Out," class=\"CELLS_PAD_%u\"",CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10
   fprintf (Gbl.F.Out,">");
  }

void Tbl_StartTableCenter (unsigned CellPadding)
  {
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_CENTER");
   if (CellPadding)
      fprintf (Gbl.F.Out," CELLS_PAD_%u",CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10
   fprintf (Gbl.F.Out,"\">");
  }

void Tbl_StartTableWide (unsigned CellPadding)
  {
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE");
   if (CellPadding)
      fprintf (Gbl.F.Out," CELLS_PAD_%u",CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10
   fprintf (Gbl.F.Out,"\">");
  }

void Tbl_StartTableWideMargin (unsigned CellPadding)
  {
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE_MARGIN");
   if (CellPadding)
      fprintf (Gbl.F.Out," CELLS_PAD_%u",CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10
   fprintf (Gbl.F.Out,"\">");
  }

void Tbl_EndTable (void)
  {
   fprintf (Gbl.F.Out,"</table>");
  }
