// swad_HTML.c: tables, divs

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

#include "swad_global.h"
#include "swad_HTML.h"

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

static unsigned HTM_TABLE_NestingLevel = 0;
static unsigned HTM_TR_NestingLevel = 0;
static unsigned HTM_TH_NestingLevel = 0;
static unsigned HTM_TD_NestingLevel = 0;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void HTM_TABLE_BeginWithoutAttr (void);

static void HTM_TR_BeginWithoutAttr (void);

static void HTM_TH_BeginWithoutAttr (void);
static void HTM_TH_BeginAttr (const char *fmt,...);

static void HTM_TD_BeginWithoutAttr (void);

/*****************************************************************************/
/******************************* Start/end table *****************************/
/*****************************************************************************/

void HTM_TABLE_Begin (const char *fmt,...)
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

	 HTM_TABLE_NestingLevel++;

	 free ((void *) Class);
	}
      else
         HTM_TABLE_BeginWithoutAttr ();
     }
   else
      HTM_TABLE_BeginWithoutAttr ();
  }

void HTM_TABLE_BeginPadding (unsigned CellPadding)
  {
   if (CellPadding)
     {
      fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_%u\">",
	       CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10

      HTM_TABLE_NestingLevel++;
     }
   else
      HTM_TABLE_BeginWithoutAttr ();
  }

static void HTM_TABLE_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<table>");

   HTM_TABLE_NestingLevel++;
  }

void HTM_TABLE_BeginCenterPadding (unsigned CellPadding)
  {
   if (CellPadding)
     {
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_CENTER CELLS_PAD_%u\">",
	       CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10

      HTM_TABLE_NestingLevel++;
     }
   else
      HTM_TABLE_BeginCenter ();
  }

void HTM_TABLE_BeginCenter (void)
  {
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_CENTER\">");

   HTM_TABLE_NestingLevel++;
  }

void HTM_TABLE_BeginWidePadding (unsigned CellPadding)
  {
   if (CellPadding)
     {
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE CELLS_PAD_%u\">",
	       CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10

      HTM_TABLE_NestingLevel++;
     }
   else
      HTM_TABLE_BeginWide ();
  }

void HTM_TABLE_BeginWide (void)
  {
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE\">");

   HTM_TABLE_NestingLevel++;
  }

void HTM_TABLE_BeginWideMarginPadding (unsigned CellPadding)
  {
   if (CellPadding)
     {
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE_MARGIN CELLS_PAD_%u\">",
	       CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10

      HTM_TABLE_NestingLevel++;
     }
   else
      HTM_TABLE_BeginWideMargin ();
  }

void HTM_TABLE_BeginWideMargin (void)
  {
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE_MARGIN\">");

   HTM_TABLE_NestingLevel++;
  }

void HTM_TABLE_End (void)
  {
   if (HTM_TABLE_NestingLevel == 0)	// No TABLE open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TABLE.");

   fprintf (Gbl.F.Out,"</table>");

   HTM_TABLE_NestingLevel--;
  }

/*****************************************************************************/
/**************************** Start/end table row ****************************/
/*****************************************************************************/

void HTM_TR_Begin (const char *fmt,...)
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
         HTM_TR_BeginWithoutAttr ();
     }
   else
      HTM_TR_BeginWithoutAttr ();

   HTM_TR_NestingLevel++;
  }

static void HTM_TR_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<tr>");
  }

void HTM_TR_End (void)
  {
   if (HTM_TR_NestingLevel == 0)	// No TR open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TR.");

   fprintf (Gbl.F.Out,"</tr>");

   HTM_TR_NestingLevel--;
  }

/*****************************************************************************/
/***************************** Table heading cells ***************************/
/*****************************************************************************/

void HTM_TH (unsigned RowSpan,unsigned ColSpan,const char *Class,const char *Txt)
  {
   HTM_TH_Begin (RowSpan,ColSpan,Class);
   if (Txt)
      if (Txt[0])
         fprintf (Gbl.F.Out,"%s",Txt);
   HTM_TH_End ();
  }

void HTM_TH_Begin (unsigned RowSpan,unsigned ColSpan,const char *Class)
  {
   if (RowSpan > 1 && ColSpan > 1)
     {
      if (Class)
	 HTM_TH_BeginAttr ("rowspan=\"%u\" colspan=\"%u\" class=\"%s\"",
		       RowSpan,ColSpan,Class);
      else
	 HTM_TH_BeginAttr ("rowspan=\"%u\" colspan=\"%u\"",
		       RowSpan,ColSpan);
     }
   else if (RowSpan > 1)
     {
      if (Class)
	 HTM_TH_BeginAttr ("rowspan=\"%u\" class=\"%s\"",
		       RowSpan,Class);
      else
	 HTM_TH_BeginAttr ("rowspan=\"%u\"",
		       RowSpan);
     }
   else if (ColSpan > 1)
     {
      if (Class)
	 HTM_TH_BeginAttr ("colspan=\"%u\" class=\"%s\"",
		       ColSpan,Class);
      else
	 HTM_TH_BeginAttr ("colspan=\"%u\"",
		       ColSpan);
     }
   else
     {
      if (Class)
         HTM_TH_BeginAttr ("class=\"%s\"",
		       Class);
      else
	 HTM_TH_BeginWithoutAttr ();
     }
  }

static void HTM_TH_BeginAttr (const char *fmt,...)
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
         HTM_TH_BeginWithoutAttr ();
     }
   else
      HTM_TH_BeginWithoutAttr ();

   HTM_TH_NestingLevel++;
  }

static void HTM_TH_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<th>");
  }

void HTM_TH_End (void)
  {
   if (HTM_TH_NestingLevel == 0)	// No TH open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TR.");

   fprintf (Gbl.F.Out,"</th>");

   HTM_TH_NestingLevel--;
  }

void HTM_TH_Empty (unsigned NumColumns)
  {
   unsigned NumCol;

   for (NumCol = 0;
	NumCol < NumColumns;
	NumCol++)
     {
      HTM_TH_BeginAttr (NULL);
      HTM_TH_End ();
     }
  }

/*****************************************************************************/
/********************************* Table cells *******************************/
/*****************************************************************************/

void HTM_TD_Begin (const char *fmt,...)
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
         HTM_TD_BeginWithoutAttr ();
     }
   else
      HTM_TD_BeginWithoutAttr ();

   HTM_TD_NestingLevel++;
  }

static void HTM_TD_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<td>");
  }

void HTM_TD_End (void)
  {
   if (HTM_TD_NestingLevel == 0)	// No TH open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TD.");

   fprintf (Gbl.F.Out,"</td>");

   HTM_TD_NestingLevel--;
  }

void HTM_TD_Empty (unsigned NumColumns)
  {
   unsigned NumCol;

   for (NumCol = 0;
	NumCol < NumColumns;
	NumCol++)
     {
      HTM_TD_Begin (NULL);
      HTM_TD_End ();
     }
  }

void HTM_TD_ColouredEmpty (unsigned NumColumns)
  {
   unsigned NumCol;

   for (NumCol = 0;
	NumCol < NumColumns;
	NumCol++)
     {
      HTM_TD_Begin ("class=\"COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();
     }
  }

/*****************************************************************************/
/************************************ Divs ***********************************/
/*****************************************************************************/

void HTM_DIV_End (void)
  {
   fprintf (Gbl.F.Out,"</div>");
  }
