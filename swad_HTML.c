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
static unsigned HTM_DIV_NestingLevel = 0;
static unsigned HTM_UL_NestingLevel = 0;
static unsigned HTM_LI_NestingLevel = 0;
static unsigned HTM_A_NestingLevel = 0;
static unsigned HTM_SCRIPT_NestingLevel = 0;
static unsigned HTM_LABEL_NestingLevel = 0;
static unsigned HTM_TEXTAREA_NestingLevel = 0;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void HTM_TABLE_BeginWithoutAttr (void);

static void HTM_TR_BeginWithoutAttr (void);

static void HTM_TH_BeginWithoutAttr (void);
static void HTM_TH_BeginAttr (const char *fmt,...);

static void HTM_TD_BeginWithoutAttr (void);

static void HTM_DIV_BeginWithoutAttr (void);

static void HTM_UL_BeginWithoutAttr (void);
static void HTM_LI_BeginWithoutAttr (void);

static void HTM_A_BeginWithoutAttr (void);

static void HTM_LABEL_BeginWithoutAttr (void);

static void HTM_TEXTAREA_BeginWithoutAttr (void);

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
   if (HTM_TD_NestingLevel == 0)	// No TD open
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

void HTM_DIV_Begin (const char *fmt,...)
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
	 fprintf (Gbl.F.Out,"<div %s>",Attr);

	 free ((void *) Attr);
	}
      else
         HTM_DIV_BeginWithoutAttr ();
     }
   else
      HTM_DIV_BeginWithoutAttr ();

   HTM_DIV_NestingLevel++;
  }

static void HTM_DIV_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<div>");
  }

void HTM_DIV_End (void)
  {
   if (HTM_DIV_NestingLevel == 0)	// No DIV open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened DIV.");

   fprintf (Gbl.F.Out,"</div>");

   HTM_DIV_NestingLevel--;
  }

/*****************************************************************************/
/******************************** Main zone **********************************/
/*****************************************************************************/

void HTM_MAIN_Begin (const char *Class)
  {
   fprintf (Gbl.F.Out,"<main class=\"%s\">",Class);
  }

void HTM_MAIN_End (void)
  {
   fprintf (Gbl.F.Out,"</main>");
  }

/*****************************************************************************/
/********************************* Articles **********************************/
/*****************************************************************************/

void HTM_ARTICLE_Begin (const char *ArticleId)
  {
   fprintf (Gbl.F.Out,"<article id=\"%s\">",ArticleId);
  }

void HTM_ARTICLE_End (void)
  {
   fprintf (Gbl.F.Out,"</article>");
  }

/*****************************************************************************/
/********************************* Sections **********************************/
/*****************************************************************************/

void HTM_SECTION_Begin (const char *SectionId)
  {
   fprintf (Gbl.F.Out,"<section id=\"%s\">",SectionId);
  }

void HTM_SECTION_End (void)
  {
   fprintf (Gbl.F.Out,"</section>");
  }

/*****************************************************************************/
/****************************** Unordered lists ******************************/
/*****************************************************************************/

void HTM_UL_Begin (const char *fmt,...)
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
	 fprintf (Gbl.F.Out,"<ul %s>",Attr);

	 free ((void *) Attr);
	}
      else
         HTM_UL_BeginWithoutAttr ();
     }
   else
      HTM_UL_BeginWithoutAttr ();

   HTM_UL_NestingLevel++;
  }

static void HTM_UL_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<ul>");
  }

void HTM_UL_End (void)
  {
   if (HTM_UL_NestingLevel == 0)	// No UL open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened UL.");

   fprintf (Gbl.F.Out,"</ul>");

   HTM_UL_NestingLevel--;
  }

/*****************************************************************************/
/******************************** List items *********************************/
/*****************************************************************************/

void HTM_LI_Begin (const char *fmt,...)
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
	 fprintf (Gbl.F.Out,"<li %s>",Attr);

	 free ((void *) Attr);
	}
      else
         HTM_LI_BeginWithoutAttr ();
     }
   else
      HTM_LI_BeginWithoutAttr ();

   HTM_LI_NestingLevel++;
  }

static void HTM_LI_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<li>");
  }

void HTM_LI_End (void)
  {
   if (HTM_LI_NestingLevel == 0)	// No LI open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened LI.");

   fprintf (Gbl.F.Out,"</li>");

   HTM_LI_NestingLevel--;
  }

/*****************************************************************************/
/********************************** Anchors **********************************/
/*****************************************************************************/

void HTM_A_Begin (const char *fmt,...)
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
	 fprintf (Gbl.F.Out,"<a %s>",Attr);

	 free ((void *) Attr);
	}
      else
         HTM_A_BeginWithoutAttr ();
     }
   else
      HTM_A_BeginWithoutAttr ();

   HTM_A_NestingLevel++;
  }

static void HTM_A_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<a>");
  }

void HTM_A_End (void)
  {
   if (HTM_A_NestingLevel == 0)	// No A open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened A.");

   fprintf (Gbl.F.Out,"</a>");

   HTM_A_NestingLevel--;
  }

/*****************************************************************************/
/*********************************** Scripts *********************************/
/*****************************************************************************/

void HTM_SCRIPT_Begin (const char *URL,const char *CharSet)
  {
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\"");
   if (URL)
      if (URL[0])
         fprintf (Gbl.F.Out," src=\"%s\"",URL);
   if (CharSet)
      if (CharSet[0])
         fprintf (Gbl.F.Out," charset=\"%s\"",CharSet);
   fprintf (Gbl.F.Out,">");

   HTM_SCRIPT_NestingLevel++;
  }

void HTM_SCRIPT_End (void)
  {
   if (HTM_SCRIPT_NestingLevel == 0)	// No SCRIPT open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened SCRIPT.");

   fprintf (Gbl.F.Out,"</script>");

   HTM_SCRIPT_NestingLevel--;
  }

/*****************************************************************************/
/*********************************** Labels **********************************/
/*****************************************************************************/

void HTM_LABEL_Begin (const char *fmt,...)
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
	 fprintf (Gbl.F.Out,"<label %s>",Attr);

	 free ((void *) Attr);
	}
      else
         HTM_LABEL_BeginWithoutAttr ();
     }
   else
      HTM_LABEL_BeginWithoutAttr ();

   HTM_LABEL_NestingLevel++;
  }

static void HTM_LABEL_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<label>");
  }

void HTM_LABEL_End (void)
  {
   if (HTM_LABEL_NestingLevel == 0)	// No LABEL open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened LABEL.");

   fprintf (Gbl.F.Out,"</label>");

   HTM_LABEL_NestingLevel--;
  }

/*****************************************************************************/
/************************* Input text, email, url ****************************/
/*****************************************************************************/

void HTM_INPUT_TEXT (const char *Name,unsigned MaxLength,const char *Value,bool SubmitOnChange,
	             const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   fprintf (Gbl.F.Out,"<input type=\"text\" id=\"%s\" name=\"%s\""
		      " maxlength=\"%u\" value=\"%s\"",
	    Name,Name,MaxLength,Value);

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

	 /***** Print attributes *****/
	 fprintf (Gbl.F.Out," %s",Attr);

	 free ((void *) Attr);
	}
     }

   if (SubmitOnChange)
      fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
	       Gbl.Form.Id);

   fprintf (Gbl.F.Out," />");
  }

void HTM_INPUT_EMAIL (const char *Name,unsigned MaxLength,const char *Value,
	              const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   fprintf (Gbl.F.Out,"<input type=\"email\" id=\"%s\" name=\"%s\""
		      " maxlength=\"%u\" value=\"%s\"",
	    Name,Name,MaxLength,Value);

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

	 /***** Print attributes *****/
	 fprintf (Gbl.F.Out," %s",Attr);

	 free ((void *) Attr);
	}
     }

   fprintf (Gbl.F.Out," />");
  }

void HTM_INPUT_URL (const char *Name,const char *Value,bool SubmitOnChange,
	            const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   fprintf (Gbl.F.Out,"<input type=\"url\" id=\"%s\" name=\"%s\""
		      " maxlength=\"%u\" value=\"%s\"",
	    Name,Name,Cns_MAX_CHARS_WWW,Value);

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

	 /***** Print attributes *****/
	 fprintf (Gbl.F.Out," %s",Attr);

	 free ((void *) Attr);
	}
     }

   if (SubmitOnChange)
      fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
	       Gbl.Form.Id);

   fprintf (Gbl.F.Out," />");
  }

void HTM_INPUT_FILE (const char *Accept,bool SubmitOnChange)
  {
   fprintf (Gbl.F.Out,"<input type=\"file\" name=\"%s\" accept=\"%s\"",
	    Fil_NAME_OF_PARAM_FILENAME_ORG,Accept);
   if (SubmitOnChange)
      fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
	       Gbl.Form.Id);
   fprintf (Gbl.F.Out," />");
  }

void HTM_INPUT_BUTTON (const char *Name,const char *Value,const char *Attr)
  {
   fprintf (Gbl.F.Out,"<input type=\"button\" name=\"%s\" value=\"%s\"%s />",
	    Name,Value,Attr);
  }

void HTM_INPUT_IMAGE (const char *ImgFile,const char *Title,const char *Class)
  {
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s\""
		      " alt=\"%s\" title=\"%s\" class=\"%s\" />",
	    Cfg_URL_ICON_PUBLIC,ImgFile,
	    Title,Title,Class);
  }

void HTM_INPUT_PASSWORD (const char *Name,const char *PlaceHolder,
			 const char *AutoComplete,bool Required)
  {
   fprintf (Gbl.F.Out,"<input type=\"password\" id=\"%s\" name=\"%s\""
                      " size=\"18\" maxlength=\"%u\"",
	    Name,Name,Pwd_MIN_CHARS_PLAIN_PASSWORD);
   if (PlaceHolder)
      if (PlaceHolder[0])
	 fprintf (Gbl.F.Out," placeholder=\"%s\"",PlaceHolder);
   if (AutoComplete)
      if (AutoComplete[0])
         fprintf (Gbl.F.Out," autocomplete=\"%s\"",AutoComplete);
   if (Required)
      fprintf (Gbl.F.Out," required=\"required\"");
   fprintf (Gbl.F.Out," />");
  }

/*****************************************************************************/
/********************************* Text areas ********************************/
/*****************************************************************************/

void HTM_TEXTAREA_Begin (const char *fmt,...)
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
	 fprintf (Gbl.F.Out,"<textarea %s>",Attr);

	 free ((void *) Attr);
	}
      else
         HTM_TEXTAREA_BeginWithoutAttr ();
     }
   else
      HTM_TEXTAREA_BeginWithoutAttr ();

   HTM_TEXTAREA_NestingLevel++;
  }

static void HTM_TEXTAREA_BeginWithoutAttr (void)
  {
   fprintf (Gbl.F.Out,"<textarea>");
  }

void HTM_TEXTAREA_End (void)
  {
   if (HTM_TEXTAREA_NestingLevel == 0)	// No TEXTAREA open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TEXTAREA.");

   fprintf (Gbl.F.Out,"</textarea>");

   HTM_TEXTAREA_NestingLevel--;
  }

/*****************************************************************************/
/********************************** Images ***********************************/
/*****************************************************************************/

void HTM_IMG (const char *URL,const char *Icon,const char *Title,
	      const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   fprintf (Gbl.F.Out,"<img src=\"%s",URL);
   if (Icon)
      if (Icon[0])
         fprintf (Gbl.F.Out,"/%s",Icon);
   fprintf (Gbl.F.Out,"\"");

   if (Title)
     {
      if (Title[0])
         fprintf (Gbl.F.Out," alt=\"%s\" title=\"%s\"",Title,Title);
      else
         fprintf (Gbl.F.Out," alt=\"\"");
     }
   else
      fprintf (Gbl.F.Out," alt=\"\"");

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

	 /***** Print attributes *****/
	 fprintf (Gbl.F.Out," %s",Attr);

	 free ((void *) Attr);
	}
     }

   fprintf (Gbl.F.Out," />");
  }
