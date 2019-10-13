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
/***************************** Private vatiables *****************************/
/*****************************************************************************/

static unsigned Tbl_TABLE_NestingLevel = 0;
static unsigned Tbl_TR_NestingLevel = 0;
static unsigned Tbl_TH_NestingLevel = 0;
static unsigned Tbl_TD_NestingLevel = 0;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Tbl_TABLE_BeginWithoutAttr (void);

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

	 Tbl_TABLE_NestingLevel++;

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
     {
      fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_%u\">",
	       CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10

      Tbl_TABLE_NestingLevel++;
     }
   else
      Tbl_TABLE_BeginWithoutAttr ();
  }

static void Tbl_TABLE_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<table>");

   Tbl_TABLE_NestingLevel++;
  }

void Tbl_TABLE_BeginCenterPadding (unsigned CellPadding)
  {
   if (CellPadding)
     {
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_CENTER CELLS_PAD_%u\">",
	       CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10

      Tbl_TABLE_NestingLevel++;
     }
   else
      Tbl_TABLE_BeginCenter ();
  }

void Tbl_TABLE_BeginCenter (void)
  {
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_CENTER\">");

   Tbl_TABLE_NestingLevel++;
  }

void Tbl_TABLE_BeginWidePadding (unsigned CellPadding)
  {
   if (CellPadding)
     {
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE CELLS_PAD_%u\">",
	       CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10

      Tbl_TABLE_NestingLevel++;
     }
   else
      Tbl_TABLE_BeginWide ();
  }

void Tbl_TABLE_BeginWide (void)
  {
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE\">");

   Tbl_TABLE_NestingLevel++;
  }

void Tbl_TABLE_BeginWideMarginPadding (unsigned CellPadding)
  {
   if (CellPadding)
     {
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE_MARGIN CELLS_PAD_%u\">",
	       CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10

      Tbl_TABLE_NestingLevel++;
     }
   else
      Tbl_TABLE_BeginWideMargin ();
  }

void Tbl_TABLE_BeginWideMargin (void)
  {
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE_MARGIN\">");

   Tbl_TABLE_NestingLevel++;
  }

void Tbl_TABLE_End (void)
  {
   if (Tbl_TABLE_NestingLevel == 0)	// No TABLE open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TABLE.");

   fprintf (Gbl.F.Out,"</table>");

   Tbl_TABLE_NestingLevel--;
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

   Tbl_TR_NestingLevel++;
  }

static void Tbl_TR_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<tr>");
  }

void Tbl_TR_End (void)
  {
   if (Tbl_TR_NestingLevel == 0)	// No TR open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TR.");

   fprintf (Gbl.F.Out,"</tr>");

   Tbl_TR_NestingLevel--;
  }

/*****************************************************************************/
/***************************** Table heading cells ***************************/
/*****************************************************************************/

void Tbl_TH (unsigned RowSpan,unsigned ColSpan,const char *Class,const char *Txt)
  {
   Tbl_TH_Begin (RowSpan,ColSpan,Class);
   if (Txt)
      if (Txt[0])
         fprintf (Gbl.F.Out,"%s",Txt);
   Tbl_TH_End ();
  }

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

static void Tbl_TH_BeginAttr (const char *fmt,...)
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

   Tbl_TH_NestingLevel++;
  }

static void Tbl_TH_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<th>");
  }

void Tbl_TH_End (void)
  {
   if (Tbl_TH_NestingLevel == 0)	// No TH open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TR.");

   fprintf (Gbl.F.Out,"</th>");

   Tbl_TH_NestingLevel--;
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

   Tbl_TD_NestingLevel++;
  }

static void Tbl_TD_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<td>");
  }

void Tbl_TD_End (void)
  {
   if (Tbl_TD_NestingLevel == 0)	// No TH open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TR.");

   fprintf (Gbl.F.Out,"</td>");

   Tbl_TD_NestingLevel--;
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
