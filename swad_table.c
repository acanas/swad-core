// swad_table.c: tables

/*
    SWAD (Shared Workspace At a Distance),
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

#define _GNU_SOURCE 		// For vasprintf
#include <stdarg.h>		// For va_start, va_end
#include <stdio.h>		// For fprintf, vasprintf
#include <stdlib.h>		// For free

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

static void Tbl_TR_BeginWithoutAttr (void);
static void Tbl_TH_BeginWithoutAttr (void);

static void Tbl_TH_BeginAttr (const char *fmt,...);
static void Tbl_TD_BeginWithoutAttr (void);

/*****************************************************************************/
/******************************* Start/end table *****************************/
/*****************************************************************************/

void Tbl_TABLE_Begin (const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Class;

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Class,fmt,ap);
	 va_end (ap);

	 if (NumBytesPrinted < 0)	// If memory allocation wasn't possible,
				      // or some other error occurs,
				      // vasprintf will return -1
	    Lay_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 fprintf (Gbl.F.Out,"<table class=\"%s\">",Class);

	 free ((void *) Class);
	}
      else
         Tbl_TABLE_BeginWithoutAttr ();
     }
   else
      Tbl_TABLE_BeginWithoutAttr ();
  }

void Tbl_TABLE_BeginPadding (unsigned CellPadding)
  {
   if (CellPadding)
      fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_%u\">",
	       CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10
   else
      Tbl_TABLE_BeginWithoutAttr ();
  }

void Tbl_TABLE_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<table>");
  }

void Tbl_TABLE_BeginCenterPadding (unsigned CellPadding)
  {
   if (CellPadding)
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_CENTER CELLS_PAD_%u\">",
	       CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10
   else
      Tbl_TABLE_BeginCenter ();
  }

void Tbl_TABLE_BeginCenter (void)
  {
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_CENTER\">");
  }

void Tbl_TABLE_BeginWidePadding (unsigned CellPadding)
  {
   if (CellPadding)
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE CELLS_PAD_%u\">",
	       CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10
   else
      Tbl_TABLE_BeginWide ();
  }

void Tbl_TABLE_BeginWide (void)
  {
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE\">");
  }

void Tbl_TABLE_BeginWideMarginPadding (unsigned CellPadding)
  {
   if (CellPadding)
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE_MARGIN CELLS_PAD_%u\">",
	       CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10
   else
      Tbl_TABLE_BeginWideMargin ();
  }

void Tbl_TABLE_BeginWideMargin (void)
  {
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE_MARGIN\">");
  }

void Tbl_TABLE_End (void)
  {
   fprintf (Gbl.F.Out,"</table>");
  }

/*****************************************************************************/
/**************************** Start/end table row ****************************/
/*****************************************************************************/

void Tbl_TR_Begin (const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);

	 if (NumBytesPrinted < 0)	// If memory allocation wasn't possible,
					// or some other error occurs,
					// vasprintf will return -1
	    Lay_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 fprintf (Gbl.F.Out,"<tr %s>",Attr);

	 free ((void *) Attr);
	}
      else
         Tbl_TR_BeginWithoutAttr ();
     }
   else
      Tbl_TR_BeginWithoutAttr ();
  }

static void Tbl_TR_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<tr>");
  }

void Tbl_TR_End (void)
  {
   fprintf (Gbl.F.Out,"</tr>");
  }

/*****************************************************************************/
/***************************** Table heading cells ***************************/
/*****************************************************************************/

void Tbl_TH_Begin (unsigned RowSpan,unsigned ColSpan,const char *Class)
  {
   if (RowSpan > 1 && ColSpan > 1)
     {
      if (Class)
	 Tbl_TH_BeginAttr ("rowspan=\"%u\" colspan=\"%u\" class=\"%s\"",
		       RowSpan,ColSpan,Class);
      else
	 Tbl_TH_BeginAttr ("rowspan=\"%u\" colspan=\"%u\"",
		       RowSpan,ColSpan);
     }
   else if (RowSpan > 1)
     {
      if (Class)
	 Tbl_TH_BeginAttr ("rowspan=\"%u\" class=\"%s\"",
		       RowSpan,Class);
      else
	 Tbl_TH_BeginAttr ("rowspan=\"%u\"",
		       RowSpan);
     }
   else if (ColSpan > 1)
     {
      if (Class)
	 Tbl_TH_BeginAttr ("colspan=\"%u\" class=\"%s\"",
		       ColSpan,Class);
      else
	 Tbl_TH_BeginAttr ("colspan=\"%u\"",
		       ColSpan);
     }
   else
     {
      if (Class)
         Tbl_TH_BeginAttr ("class=\"%s\"",
		       Class);
      else
	 Tbl_TH_BeginWithoutAttr ();
     }
  }

void Tbl_TH_BeginAttr (const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);

	 if (NumBytesPrinted < 0)	// If memory allocation wasn't possible,
					// or some other error occurs,
					// vasprintf will return -1
	    Lay_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 fprintf (Gbl.F.Out,"<th %s>",Attr);

	 free ((void *) Attr);
	}
      else
         Tbl_TH_BeginWithoutAttr ();
     }
   else
      Tbl_TH_BeginWithoutAttr ();
  }

static void Tbl_TH_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<th>");
  }

void Tbl_TH_End (void)
  {
   fprintf (Gbl.F.Out,"</th>");
  }

void Tbl_TH_Empty (unsigned NumColumns)
  {
   unsigned NumCol;

   for (NumCol = 0;
	NumCol < NumColumns;
	NumCol++)
     {
      Tbl_TH_BeginAttr (NULL);
      Tbl_TH_End ();
     }
  }

/*****************************************************************************/
/********************************* Table cells *******************************/
/*****************************************************************************/

void Tbl_TD_Begin (const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);

	 if (NumBytesPrinted < 0)	// If memory allocation wasn't possible,
					// or some other error occurs,
					// vasprintf will return -1
	    Lay_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 fprintf (Gbl.F.Out,"<td %s>",Attr);

	 free ((void *) Attr);
	}
      else
         Tbl_TD_BeginWithoutAttr ();
     }
   else
      Tbl_TD_BeginWithoutAttr ();
  }

static void Tbl_TD_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<td>");
  }

void Tbl_TD_End (void)
  {
   fprintf (Gbl.F.Out,"</td>");
  }

void Tbl_TD_Empty (unsigned NumColumns)
  {
   unsigned NumCol;

   for (NumCol = 0;
	NumCol < NumColumns;
	NumCol++)
     {
      Tbl_TD_Begin (NULL);
      Tbl_TD_End ();
     }
  }

void Tbl_TD_ColouredEmpty (unsigned NumColumns)
  {
   unsigned NumCol;

   for (NumCol = 0;
	NumCol < NumColumns;
	NumCol++)
     {
      Tbl_TD_Begin ("class=\"COLOR%u\"",Gbl.RowEvenOdd);
      Tbl_TD_End ();
     }
  }
