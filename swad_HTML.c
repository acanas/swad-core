// swad_HTML.c: tables, divs

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
#include <stdio.h>		// For vasprintf
#include <stdlib.h>		// For free

#include "swad_alert.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_HTML.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

static const char *ClassAlign[HTM_NUM_HEAD_ALIGN] =
  {
   [HTM_HEAD_LEFT  ] = "LT",
   [HTM_HEAD_CENTER] = "CT",
   [HTM_HEAD_RIGHT ] = "RT",
  };

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static unsigned HTM_TABLE_NestingLevel    = 0;
static unsigned HTM_TR_NestingLevel       = 0;
static unsigned HTM_TH_NestingLevel       = 0;
static unsigned HTM_TD_NestingLevel       = 0;
static unsigned HTM_DIV_NestingLevel      = 0;
static unsigned HTM_SPAN_NestingLevel     = 0;
static unsigned HTM_OL_NestingLevel       = 0;
static unsigned HTM_UL_NestingLevel       = 0;
static unsigned HTM_LI_NestingLevel       = 0;
static unsigned HTM_DL_NestingLevel       = 0;
static unsigned HTM_DT_NestingLevel       = 0;
static unsigned HTM_DD_NestingLevel       = 0;
static unsigned HTM_A_NestingLevel        = 0;
static unsigned HTM_SCRIPT_NestingLevel   = 0;
static unsigned HTM_FIELDSET_NestingLevel = 0;
static unsigned HTM_LABEL_NestingLevel    = 0;
static unsigned HTM_BUTTON_NestingLevel   = 0;
static unsigned HTM_TEXTAREA_NestingLevel = 0;
static unsigned HTM_SELECT_NestingLevel   = 0;
static unsigned HTM_OPTGROUP_NestingLevel = 0;
static unsigned HTM_STRONG_NestingLevel   = 0;
static unsigned HTM_EM_NestingLevel       = 0;
static unsigned HTM_U_NestingLevel        = 0;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void HTM_TABLE_BeginWithoutAttr (void);

static void HTM_TBODY_BeginWithoutAttr (void);

static void HTM_TR_BeginWithoutAttr (void);

static void HTM_TD_BeginWithoutAttr (void);

static void HTM_DIV_BeginWithoutAttr (void);

static void HTM_SPAN_BeginWithoutAttr (void);

static void HTM_UL_BeginWithoutAttr (void);
static void HTM_LI_BeginWithoutAttr (void);

static void HTM_A_BeginWithoutAttr (void);

static void HTM_FIELDSET_BeginWithoutAttr (void);

static void HTM_LABEL_BeginWithoutAttr (void);

static void HTM_TEXTAREA_BeginWithoutAttr (void);

static void HTM_SELECT_BeginWithoutAttr (void);

static void HTM_SPTxt (const char *Txt);

/*****************************************************************************/
/******************************* Begin/end title *****************************/
/*****************************************************************************/

void HTM_TITLE_Begin (void)
  {
   HTM_Txt ("<title>");
  }

void HTM_TITLE_End (void)
  {
   HTM_Txt ("</title>\n");
  }

/*****************************************************************************/
/******************************* Begin/end table *****************************/
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
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("<table class=\"%s\">",Class);

	 HTM_TABLE_NestingLevel++;

	 free (Class);
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
      HTM_TxtF ("<table class=\"CELLS_PAD_%u\">",
	        CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10

      HTM_TABLE_NestingLevel++;
     }
   else
      HTM_TABLE_BeginWithoutAttr ();
  }

static void HTM_TABLE_BeginWithoutAttr (void)
  {
   HTM_Txt ("<table>");

   HTM_TABLE_NestingLevel++;
  }

void HTM_TABLE_BeginCenterPadding (unsigned CellPadding)
  {
   if (CellPadding)
     {
      HTM_TxtF ("<table class=\"FRAME_TBL_CENTER CELLS_PAD_%u\">",
	        CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10

      HTM_TABLE_NestingLevel++;
     }
   else
      HTM_TABLE_BeginCenter ();
  }

void HTM_TABLE_BeginCenter (void)
  {
   HTM_Txt ("<table class=\"FRAME_TBL_CENTER\">");

   HTM_TABLE_NestingLevel++;
  }

void HTM_TABLE_BeginWidePadding (unsigned CellPadding)
  {
   if (CellPadding)
     {
      HTM_TxtF ("<table class=\"FRAME_TBL_WIDE CELLS_PAD_%u\">",
	        CellPadding);	// CellPadding must be 0, 1, 2, 5 or 10

      HTM_TABLE_NestingLevel++;
     }
   else
      HTM_TABLE_BeginWide ();
  }

void HTM_TABLE_BeginWide (void)
  {
   HTM_Txt ("<table class=\"FRAME_TBL_WIDE\">");

   HTM_TABLE_NestingLevel++;
  }

void HTM_TABLE_BeginWideMarginPadding (unsigned CellPadding)
  {
   if (CellPadding)
      HTM_TxtF ("<table class=\"FRAME_TBL_WIDE_MARGIN CELLS_PAD_%u\">",
	        CellPadding);	// CellPadding must be 0, 1, 2, 5, 10, 20

   else
      HTM_Txt ("<table class=\"FRAME_TBL_WIDE_MARGIN\">");

   HTM_TABLE_NestingLevel++;
  }

void HTM_TABLE_End (void)
  {
   if (HTM_TABLE_NestingLevel == 0)	// No TABLE open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TABLE.");

   HTM_Txt ("</table>");

   HTM_TABLE_NestingLevel--;
  }

/*****************************************************************************/
/********************************* Table body ********************************/
/*****************************************************************************/

void HTM_TBODY_Begin (const char *fmt,...)
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
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("<tbody %s>",Attr);

	 free (Attr);
	}
      else
         HTM_TBODY_BeginWithoutAttr ();
     }
   else
      HTM_TBODY_BeginWithoutAttr ();
  }

static void HTM_TBODY_BeginWithoutAttr (void)
  {
   HTM_Txt ("<tbody>");
  }

void HTM_TBODY_End (void)
  {
   HTM_Txt ("</tbody>");
  }

/*****************************************************************************/
/**************************** Begin/end table row ****************************/
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
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("<tr %s>",Attr);

	 free (Attr);
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
   HTM_Txt ("<tr>");
  }

void HTM_TR_End (void)
  {
   if (HTM_TR_NestingLevel == 0)	// No TR open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TR.");

   HTM_Txt ("</tr>");

   HTM_TR_NestingLevel--;
  }

/*****************************************************************************/
/***************************** Table heading cells ***************************/
/*****************************************************************************/

void HTM_TH (const char *Title,HTM_HeadAlign HeadAlign)
  {
   HTM_TH_Span (Title,HeadAlign,1,1,NULL);
  }

void HTM_TH_Begin (HTM_HeadAlign HeadAlign)
  {
   HTM_TH_Span_Begin (HeadAlign,1,1,NULL);
  }

void HTM_TH_Span (const char *Title,HTM_HeadAlign HeadAlign,
                  unsigned RowSpan,unsigned ColSpan,
                  const char *ClassFmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr = NULL;

   if (ClassFmt)
      if (ClassFmt[0])
	{
	 va_start (ap,ClassFmt);
	 NumBytesPrinted = vasprintf (&Attr,ClassFmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();
	}

   if (RowSpan > 1 && ColSpan > 1)
     {
      if (Attr)
	 HTM_TxtF ("<th rowspan=\"%u\" colspan=\"%u\" class=\"TIT_TBL_%s %s %s\">",
		   RowSpan,ColSpan,
		   The_GetSuffix (),ClassAlign[HeadAlign],Attr);
      else
	 HTM_TxtF ("<th rowspan=\"%u\" colspan=\"%u\" class=\"TIT_TBL_%s %s\">",
		   RowSpan,ColSpan,
		   The_GetSuffix (),ClassAlign[HeadAlign]);
     }
   else if (RowSpan > 1)
     {
      if (Attr)
	 HTM_TxtF ("<th rowspan=\"%u\" class=\"TIT_TBL_%s %s %s\">",
		   RowSpan,
		   The_GetSuffix (),ClassAlign[HeadAlign],Attr);
      else
	 HTM_TxtF ("<th rowspan=\"%u\" class=\"TIT_TBL_%s %s\">",
		   RowSpan,
		   The_GetSuffix (),ClassAlign[HeadAlign]);
     }
   else if (ColSpan > 1)
     {
      if (Attr)
	 HTM_TxtF ("<th colspan=\"%u\" class=\"TIT_TBL_%s %s %s\">",
		   ColSpan,
		   The_GetSuffix (),ClassAlign[HeadAlign],Attr);
      else
	 HTM_TxtF ("<th colspan=\"%u\" class=\"TIT_TBL_%s %s\">",
		   ColSpan,
		   The_GetSuffix (),ClassAlign[HeadAlign]);
     }
   else
     {
      if (Attr)
	 HTM_TxtF ("<th class=\"TIT_TBL_%s %s %s\">",
		   The_GetSuffix (),ClassAlign[HeadAlign],Attr);
      else
	 HTM_TxtF ("<th class=\"TIT_TBL_%s %s\">",
		   The_GetSuffix (),ClassAlign[HeadAlign]);
     }

   if (ClassFmt)
      if (ClassFmt[0])
	 free (Attr);

   HTM_Txt (Title);
   HTM_Txt ("</th>");
  }

void HTM_TH_Span_Begin (HTM_HeadAlign HeadAlign,
                        unsigned RowSpan,unsigned ColSpan,
                        const char *ClassFmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr = NULL;

   if (ClassFmt)
      if (ClassFmt[0])
	{
	 va_start (ap,ClassFmt);
	 NumBytesPrinted = vasprintf (&Attr,ClassFmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();
	}

   if (RowSpan > 1 && ColSpan > 1)
     {
      if (Attr)
	 HTM_TxtF ("<th rowspan=\"%u\" colspan=\"%u\" class=\"TIT_TBL_%s %s %s\">",
		   RowSpan,ColSpan,
		   The_GetSuffix (),ClassAlign[HeadAlign],Attr);
      else
	 HTM_TxtF ("<th rowspan=\"%u\" colspan=\"%u\" class=\"TIT_TBL_%s %s\">",
		   RowSpan,ColSpan,
		   The_GetSuffix (),ClassAlign[HeadAlign]);
     }
   else if (RowSpan > 1)
     {
      if (Attr)
	 HTM_TxtF ("<th rowspan=\"%u\" class=\"TIT_TBL_%s %s %s\">",
		   RowSpan,
		   The_GetSuffix (),ClassAlign[HeadAlign],Attr);
      else
	 HTM_TxtF ("<th rowspan=\"%u\" class=\"TIT_TBL_%s %s\">",
		   RowSpan,
		   The_GetSuffix (),ClassAlign[HeadAlign]);
     }
   else if (ColSpan > 1)
     {
      if (Attr)
	 HTM_TxtF ("<th colspan=\"%u\" class=\"TIT_TBL_%s %s %s\">",
		   ColSpan,
		   The_GetSuffix (),ClassAlign[HeadAlign],Attr);
      else
	 HTM_TxtF ("<th colspan=\"%u\" class=\"TIT_TBL_%s %s\">",
		   ColSpan,
		   The_GetSuffix (),ClassAlign[HeadAlign]);
     }
   else
     {
      if (Attr)
	 HTM_TxtF ("<th class=\"TIT_TBL_%s %s %s\">",
		   The_GetSuffix (),ClassAlign[HeadAlign],Attr);
      else
	 HTM_TxtF ("<th class=\"TIT_TBL_%s %s\">",
		   The_GetSuffix (),ClassAlign[HeadAlign]);
     }

   if (ClassFmt)
      if (ClassFmt[0])
	 free (Attr);

   HTM_TH_NestingLevel++;
  }

void HTM_TH_End (void)
  {
   if (HTM_TH_NestingLevel == 0)	// No TH open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TR.");

   HTM_Txt ("</th>");

   HTM_TH_NestingLevel--;
  }

void HTM_TH_Empty (unsigned NumColumns)
  {
   unsigned NumCol;

   for (NumCol = 0;
	NumCol < NumColumns;
	NumCol++)
      HTM_Txt ("<th></th>");
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
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("<td %s>",Attr);

	 free (Attr);
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
   HTM_Txt ("<td>");
  }

void HTM_TD_End (void)
  {
   if (HTM_TD_NestingLevel == 0)	// No TD open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TD.");

   HTM_Txt ("</td>");

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
      HTM_TD_Begin ("class=\"%s\"",The_GetColorRows ());
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
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("<div %s>",Attr);

	 free (Attr);
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
   HTM_Txt ("<div>");
  }

void HTM_DIV_End (void)
  {
   if (HTM_DIV_NestingLevel == 0)	// No DIV open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened DIV.");

   HTM_Txt ("</div>");

   HTM_DIV_NestingLevel--;
  }

/*****************************************************************************/
/******************************** Main zone **********************************/
/*****************************************************************************/

void HTM_MAIN_Begin (const char *Class)
  {
   HTM_TxtF ("<main class=\"%s\">",Class);
  }

void HTM_MAIN_End (void)
  {
   HTM_Txt ("</main>");
  }

/*****************************************************************************/
/********************************* Articles **********************************/
/*****************************************************************************/

void HTM_ARTICLE_Begin (const char *ArticleId)
  {
   HTM_TxtF ("<article id=\"%s\">",ArticleId);
  }

void HTM_ARTICLE_End (void)
  {
   HTM_Txt ("</article>");
  }

/*****************************************************************************/
/********************************* Sections **********************************/
/*****************************************************************************/

void HTM_SECTION_Begin (const char *SectionId)
  {
   HTM_TxtF ("<section id=\"%s\">",SectionId);
  }

void HTM_SECTION_End (void)
  {
   HTM_Txt ("</section>");
  }

/*****************************************************************************/
/*********************************** Spans ***********************************/
/*****************************************************************************/

void HTM_SPAN_Begin (const char *fmt,...)
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
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("<span %s>",Attr);

	 free (Attr);
	}
      else
         HTM_SPAN_BeginWithoutAttr ();
     }
   else
      HTM_SPAN_BeginWithoutAttr ();

   HTM_SPAN_NestingLevel++;
  }

static void HTM_SPAN_BeginWithoutAttr (void)
  {
   HTM_Txt ("<span>");
  }

void HTM_SPAN_End (void)
  {
   if (HTM_SPAN_NestingLevel == 0)	// No SPAN open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened SPAN.");

   HTM_Txt ("</span>");

   HTM_SPAN_NestingLevel--;
  }

/*****************************************************************************/
/*********************************** Lists ***********************************/
/*****************************************************************************/

void HTM_OL_Begin (void)
  {
   HTM_Txt ("<ol>");

   HTM_OL_NestingLevel++;
  }

void HTM_OL_End (void)
  {
   if (HTM_OL_NestingLevel == 0)	// No OL open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened OL.");

   HTM_Txt ("</ol>");

   HTM_OL_NestingLevel--;
  }

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
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("<ul %s>",Attr);

	 free (Attr);
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
   HTM_Txt ("<ul>");
  }

void HTM_UL_End (void)
  {
   if (HTM_UL_NestingLevel == 0)	// No UL open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened UL.");

   HTM_Txt ("</ul>");

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
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("<li %s>",Attr);

	 free (Attr);
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
   HTM_Txt ("<li>");
  }

void HTM_LI_End (void)
  {
   if (HTM_LI_NestingLevel == 0)	// No LI open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened LI.");

   HTM_Txt ("</li>");

   HTM_LI_NestingLevel--;
  }

/*****************************************************************************/
/****************************** Definition lists *****************************/
/*****************************************************************************/

void HTM_DL_Begin (void)
  {
   HTM_Txt ("<dl>");

   HTM_DL_NestingLevel++;
  }

void HTM_DL_End (void)
  {
   if (HTM_DL_NestingLevel == 0)	// No DL open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened DL.");

   HTM_Txt ("</dl>");

   HTM_DL_NestingLevel--;
  }

void HTM_DT_Begin (void)
  {
   HTM_Txt ("<dt>");

   HTM_DT_NestingLevel++;
  }

void HTM_DT_End (void)
  {
   if (HTM_DL_NestingLevel == 0)	// No DT open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened DT.");

   HTM_Txt ("</dt>");

   HTM_DT_NestingLevel--;
  }

void HTM_DD_Begin (void)
  {
   HTM_Txt ("<dd>");

   HTM_DD_NestingLevel++;
  }

void HTM_DD_End (void)
  {
   if (HTM_DD_NestingLevel == 0)	// No DD open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened DD.");

   HTM_Txt ("</dd>");

   HTM_DD_NestingLevel--;
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
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("<a %s>",Attr);

	 free (Attr);
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
   HTM_Txt ("<a>");
  }

void HTM_A_End (void)
  {
   if (HTM_A_NestingLevel == 0)	// No A open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened A.");

   HTM_Txt ("</a>");

   HTM_A_NestingLevel--;
  }

/*****************************************************************************/
/*********************************** Scripts *********************************/
/*****************************************************************************/

void HTM_SCRIPT_Begin (const char *URL,const char *CharSet)
  {
   HTM_Txt ("<script type=\"text/javascript\"");
   if (URL)
      if (URL[0])
         HTM_TxtF (" src=\"%s\"",URL);
   if (CharSet)
      if (CharSet[0])
         HTM_TxtF (" charset=\"%s\"",CharSet);
   HTM_Txt (">\n");

   HTM_SCRIPT_NestingLevel++;
  }

void HTM_SCRIPT_End (void)
  {
   if (HTM_SCRIPT_NestingLevel == 0)	// No SCRIPT open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened SCRIPT.");

   HTM_Txt ("</script>\n");

   HTM_SCRIPT_NestingLevel--;
  }

/*****************************************************************************/
/********************************* Parameters ********************************/
/*****************************************************************************/

void HTM_PARAM (const char *Name,
		const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Value;

   if (fmt)
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Value,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("\n<param name=\"%s\" value=\"%s\">",Name,Value);

	 free (Value);
	}
  }

/*****************************************************************************/
/********************************* Fieldsets *********************************/
/*****************************************************************************/

void HTM_FIELDSET_Begin (const char *fmt,...)
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
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("<fieldset %s>",Attr);

	 free (Attr);
	}
      else
         HTM_FIELDSET_BeginWithoutAttr ();
     }
   else
      HTM_FIELDSET_BeginWithoutAttr ();

   HTM_FIELDSET_NestingLevel++;
  }

static void HTM_FIELDSET_BeginWithoutAttr (void)
  {
   HTM_Txt ("<fieldset>");
  }

void HTM_FIELDSET_End (void)
  {
   if (HTM_FIELDSET_NestingLevel == 0)	// No FIELDSET open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened FIELDSET.");

   HTM_Txt ("</fieldset>");

   HTM_FIELDSET_NestingLevel--;
  }

void HTM_LEGEND (const char *Txt)
  {
   HTM_Txt ("<legend>");
      HTM_Txt (Txt);
   HTM_Txt ("</legend>");
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
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("<label %s>",Attr);

	 free (Attr);
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
   HTM_Txt ("<label>");
  }

void HTM_LABEL_End (void)
  {
   if (HTM_LABEL_NestingLevel == 0)	// No LABEL open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened LABEL.");

   HTM_Txt ("</label>");

   HTM_LABEL_NestingLevel--;
  }

/*****************************************************************************/
/************************* Input text, email, url ****************************/
/*****************************************************************************/

void HTM_INPUT_TEXT (const char *Name,unsigned MaxLength,const char *Value,
                     HTM_SubmitOnChange_t SubmitOnChange,
	             const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   HTM_TxtF ("<input type=\"text\" name=\"%s\" maxlength=\"%u\" value=\"%s\"",
	     Name,MaxLength,Value);

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   if (SubmitOnChange == HTM_SUBMIT_ON_CHANGE)
      HTM_Txt (" onchange=\"this.form.submit();return false;\"");

   HTM_Txt (" />");
  }

void HTM_INPUT_SEARCH (const char *Name,unsigned MaxLength,const char *Value,
	               const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   HTM_TxtF ("<input type=\"search\" name=\"%s\" maxlength=\"%u\" value=\"%s\"",
	     Name,MaxLength,Value);

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   HTM_Txt (" />");
  }

void HTM_INPUT_TEL (const char *Name,const char *Value,
                    HTM_SubmitOnChange_t SubmitOnChange,
	            const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   HTM_TxtF ("<input type=\"tel\" name=\"%s\" maxlength=\"%u\" value=\"%s\"",
	     Name,Usr_MAX_CHARS_PHONE,Value);

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   if (SubmitOnChange == HTM_SUBMIT_ON_CHANGE)
      HTM_Txt (" onchange=\"this.form.submit();return false;\"");

   HTM_Txt (" />");
  }

void HTM_INPUT_EMAIL (const char *Name,unsigned MaxLength,const char *Value,
	              const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   HTM_TxtF ("<input type=\"email\" name=\"%s\" maxlength=\"%u\" value=\"%s\"",
	     Name,MaxLength,Value);

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   HTM_Txt (" />");
  }

void HTM_INPUT_URL (const char *Name,const char *Value,
                    HTM_SubmitOnChange_t SubmitOnChange,
	            const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   HTM_TxtF ("<input type=\"url\" name=\"%s\" maxlength=\"%u\" value=\"%s\"",
	     Name,Cns_MAX_CHARS_WWW,Value);

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   if (SubmitOnChange == HTM_SUBMIT_ON_CHANGE)
      HTM_Txt (" onchange=\"this.form.submit();return false;\"");

   HTM_Txt (" />");
  }

void HTM_INPUT_FILE (const char *Name,const char *Accept,
                     HTM_SubmitOnChange_t SubmitOnChange,
	             const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   HTM_TxtF ("<input type=\"file\" name=\"%s\" accept=\"%s\"",
	     Name,Accept);

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   if (SubmitOnChange == HTM_SUBMIT_ON_CHANGE)
      HTM_Txt (" onchange=\"this.form.submit();return false;\"");
   HTM_Txt (" />");
  }

void HTM_INPUT_BUTTON (const char *Name,const char *Value,const char *Attr)
  {
   HTM_TxtF ("<input type=\"button\" name=\"%s\" value=\"%s\" class=\"INPUT_%s\"%s />",
	     Name,Value,
	     The_GetSuffix (),
	     Attr);
  }

void HTM_INPUT_IMAGE (const char *URL,const char *Icon,const char *Title,
	              const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   HTM_TxtF ("<input type=\"image\" src=\"%s",URL);
   if (Icon)
      if (Icon[0])
         HTM_TxtF ("/%s",Icon);
   HTM_Txt ("\"");

   HTM_TxtF (" alt=\"%s\" title=\"%s\"",
	     Title,Title);

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   HTM_Txt (" />");
  }

void HTM_INPUT_PASSWORD (const char *Name,const char *PlaceHolder,
			 const char *AutoComplete,bool Required,
	                 const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   HTM_TxtF ("<input type=\"password\" name=\"%s\" size=\"18\" maxlength=\"%u\"",
	     Name,Pwd_MAX_CHARS_PLAIN_PASSWORD);
   if (PlaceHolder)
      if (PlaceHolder[0])
	 HTM_TxtF (" placeholder=\"%s\"",PlaceHolder);
   if (AutoComplete)
      if (AutoComplete[0])
         HTM_TxtF (" autocomplete=\"%s\"",AutoComplete);
   if (Required)
      HTM_Txt (" required=\"required\"");

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   HTM_Txt (" />");
  }

void HTM_INPUT_LONG (const char *Name,long Min,long Max,long Value,
                     HTM_SubmitOnChange_t SubmitOnChange,bool Disabled,
	             const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   HTM_TxtF ("<input type=\"number\" name=\"%s\" min=\"%ld\" max=\"%ld\" value=\"%ld\"",
	     Name,Min,Max,Value);
   if (Disabled)
      HTM_Txt (" disabled=\"disabled\"");

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   if (SubmitOnChange == HTM_SUBMIT_ON_CHANGE)
      HTM_Txt (" onchange=\"this.form.submit();return false;\"");

   HTM_Txt (" />");
  }

void HTM_INPUT_FLOAT (const char *Name,double Min,double Max,
		      double Step,	// Use 0 for "any"
		      double Value,
                      HTM_SubmitOnChange_t SubmitOnChange,bool Disabled,
	              const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   Str_SetDecimalPointToUS ();		// To print the floating point as a dot
   HTM_TxtF ("<input type=\"number\" name=\"%s\""
	     " min=\"%.15lg\" max=\"%.15lg\"",
	     Name,
	     Min,Max);
   if (Step == 0.0)
      HTM_Txt (" step=\"any\"");
   else
      HTM_TxtF (" step=\"%.15lg\"",Step);
   HTM_TxtF (" value=\"%.15lg\"",Value);
   Str_SetDecimalPointToLocal ();	// Return to local system
   if (Disabled)
      HTM_Txt (" disabled=\"disabled\"");

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   if (SubmitOnChange == HTM_SUBMIT_ON_CHANGE)
      HTM_Txt (" onchange=\"this.form.submit();return false;\"");

   HTM_Txt (" />");
  }

void HTM_INPUT_RADIO (const char *Name,HTM_SubmitOnClick_t SubmitOnClick,
		      const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   HTM_TxtF ("<input type=\"radio\" name=\"%s\"",Name);

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   if (SubmitOnClick == HTM_SUBMIT_ON_CLICK)
      HTM_Txt (" onchange=\"this.form.submit();return false;\"");
   HTM_Txt (" />");
  }

void HTM_INPUT_CHECKBOX (const char *Name,HTM_SubmitOnChange_t SubmitOnChange,
		         const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   HTM_TxtF ("<input type=\"checkbox\" name=\"%s\"",Name);

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   if (SubmitOnChange == HTM_SUBMIT_ON_CHANGE)
      HTM_Txt (" onchange=\"this.form.submit();return false;\"");

   HTM_Txt (" />");
  }

/*****************************************************************************/
/********************************** Buttons **********************************/
/*****************************************************************************/

void HTM_BUTTON_Submit_Begin (const char *Title,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   HTM_Txt ("<button type=\"submit\"");
   if (Title)
      if (Title[0])
         HTM_TxtF (" title=\"%s\"",Title);

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }
   HTM_Txt (">");

   HTM_BUTTON_NestingLevel++;
  }

void HTM_BUTTON_Begin (const char *Title,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   HTM_Txt ("<button type=\"button\"");
   if (Title)
      if (Title[0])
         HTM_TxtF (" title=\"%s\"",Title);

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   HTM_Txt (">");

   HTM_BUTTON_NestingLevel++;
  }

void HTM_BUTTON_End (void)
  {
   if (HTM_BUTTON_NestingLevel == 0)	// No BUTTON open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened BUTTON.");

   HTM_Txt ("</button>");

   HTM_BUTTON_NestingLevel--;
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
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("<textarea %s>",Attr);

	 free (Attr);
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
   HTM_Txt ("<textarea>");
  }

void HTM_TEXTAREA_End (void)
  {
   if (HTM_TEXTAREA_NestingLevel == 0)	// No TEXTAREA open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened TEXTAREA.");

   HTM_Txt ("</textarea>");

   HTM_TEXTAREA_NestingLevel--;
  }

/*****************************************************************************/
/********************************** Selectors ********************************/
/*****************************************************************************/

void HTM_SELECT_Begin (HTM_SubmitOnChange_t SubmitOnChange,
                       const char *FuncsOnChange,	// if not null ==> must include ending ";"
		       const char *fmt,...)
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
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_TxtF ("<select %s",Attr);

	 free (Attr);
	}
      else
         HTM_SELECT_BeginWithoutAttr ();
     }
   else
      HTM_SELECT_BeginWithoutAttr ();

   if (SubmitOnChange == HTM_SUBMIT_ON_CHANGE || FuncsOnChange)
     {
      HTM_Txt (" onchange=\"");
      // 1. List of functions
      if (FuncsOnChange)
         if (FuncsOnChange[0])
            HTM_Txt (FuncsOnChange);
      // 2. submit
      if (SubmitOnChange == HTM_SUBMIT_ON_CHANGE)
         HTM_Txt ("this.form.submit();");
      HTM_Txt ("return false;\"");
     }

   HTM_Txt (" />");
   HTM_SELECT_NestingLevel++;
  }

static void HTM_SELECT_BeginWithoutAttr (void)
  {
   HTM_Txt ("<select");
  }

void HTM_SELECT_End (void)
  {
   if (HTM_SELECT_NestingLevel == 0)	// No SELECT open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened SELECT.");

   HTM_Txt ("</select>");

   HTM_SELECT_NestingLevel--;
  }

void HTM_OPTGROUP_Begin (const char *Label)
  {
   HTM_TxtF ("<optgroup label=\"%s\">",Label);

   HTM_OPTGROUP_NestingLevel++;
  }

void HTM_OPTGROUP_End (void)
  {
   if (HTM_OPTGROUP_NestingLevel == 0)	// No OPTGROUP open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened OPTGROUP.");

   HTM_Txt ("</optgroup>");

   HTM_OPTGROUP_NestingLevel--;
  }

void HTM_OPTION (HTM_Type_t Type,const void *ValuePtr,
                 bool Selected,HTM_OptionEnabled_t Disabled,
		 const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Content;

   HTM_Txt ("<option value=\"");
   switch (Type)
     {
      case HTM_Type_UNSIGNED:
	 HTM_Unsigned (*((unsigned *) ValuePtr));
	 break;
      case HTM_Type_LONG:
	 HTM_Long (*((long *) ValuePtr));
	 break;
      case HTM_Type_STRING:
	 HTM_Txt ((char *) ValuePtr);
	 break;
     }
   HTM_Txt ("\"");
   if (Selected)
      HTM_Txt (" selected=\"selected\"");
   if (Disabled == HTM_OPTION_DISABLED)
      HTM_Txt (" disabled=\"disabled\"");
   HTM_Txt (">");

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Content,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_Txt (Content);

	 free (Content);
	}
     }

   HTM_Txt ("</option>");
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

   HTM_TxtF ("<img src=\"%s",URL);
   if (Icon)
      if (Icon[0])
         HTM_TxtF ("/%s",Icon);
   HTM_Txt ("\"");

   if (Title)
     {
      if (Title[0])
         HTM_TxtF (" alt=\"%s\" title=\"%s\"",Title,Title);
      else
         HTM_Txt (" alt=\"\"");
     }
   else
      HTM_Txt (" alt=\"\"");

   if (fmt)
     {
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 HTM_SPTxt (Attr);

	 free (Attr);
	}
     }

   HTM_Txt (" />");
  }

/*****************************************************************************/
/********************************** Strong ***********************************/
/*****************************************************************************/

void HTM_STRONG_Begin (void)
  {
   HTM_Txt ("<strong>");

   HTM_STRONG_NestingLevel++;
  }

void HTM_STRONG_End (void)
  {
   if (HTM_STRONG_NestingLevel == 0)	// No STRONG open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened STRONG.");

   HTM_Txt ("</strong>");

   HTM_STRONG_NestingLevel--;
  }

/*****************************************************************************/
/********************************** Emphasis *********************************/
/*****************************************************************************/

void HTM_EM_Begin (void)
  {
   HTM_Txt ("<em>");

   HTM_EM_NestingLevel++;
  }

void HTM_EM_End (void)
  {
   if (HTM_EM_NestingLevel == 0)	// No EM open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened EM.");

   HTM_Txt ("</em>");

   HTM_EM_NestingLevel--;
  }

/*****************************************************************************/
/******************************* Underlines **********************************/
/*****************************************************************************/

void HTM_U_Begin (void)
  {
   HTM_Txt ("<u>");

   HTM_U_NestingLevel++;
  }

void HTM_U_End (void)
  {
   if (HTM_U_NestingLevel == 0)	// No U open
      Ale_ShowAlert (Ale_ERROR,"Trying to close unopened U.");

   HTM_Txt ("</u>");

   HTM_U_NestingLevel--;
  }

/*****************************************************************************/
/****************************** Breaking lines *******************************/
/*****************************************************************************/

void HTM_BR (void)
  {
   HTM_Txt ("<br />");
  }

/*****************************************************************************/
/********************************** Text *************************************/
/*****************************************************************************/

void HTM_TxtF (const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Attr;

   if (fmt)
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Attr,fmt,ap);
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print HTML *****/
	 HTM_Txt (Attr);

	 free (Attr);
	}
  }

static void HTM_SPTxt (const char *Txt)
  {
   HTM_Txt (" ");
   HTM_Txt (Txt);
  }

void HTM_Txt (const char *Txt)
  {
   if (Txt)
      if (Txt[0])
         fputs (Txt,Fil_GetOutputFile ());
  }

void HTM_TxtColon (const char *Txt)
  {
   HTM_Txt (Txt);
   HTM_Colon ();
  }

void HTM_TxtSemicolon (const char *Txt)
  {
   HTM_Txt (Txt);
   HTM_Semicolon ();
  }

void HTM_TxtColonNBSP (const char *Txt)
  {
   HTM_Txt (Txt);
   HTM_Colon ();
   HTM_NBSP ();
  }

void HTM_NBSP (void)
  {
   HTM_Txt ("&nbsp;");
  }

void HTM_Colon (void)
  {
   HTM_Txt (":");
  }

void HTM_Semicolon (void)
  {
   HTM_Txt (";");
  }

void HTM_Comma (void)
  {
   HTM_Txt (",");
  }

void HTM_Hyphen (void)
  {
   HTM_Txt ("-");
  }

void HTM_Asterisk (void)
  {
   HTM_Txt ("*");
  }

void HTM_Unsigned (unsigned Num)
  {
   HTM_TxtF ("%u",Num);
  }

void HTM_Light0 (void)
  {
   HTM_SPAN_Begin ("class=\"VERY_LIGHT\"");
      HTM_Txt ("0");
   HTM_SPAN_End ();
  }

void HTM_Int (int Num)
  {
   HTM_TxtF ("%d",Num);
  }

void HTM_UnsignedLong (unsigned long Num)
  {
   HTM_TxtF ("%lu",Num);
  }

void HTM_Long (long Num)
  {
   HTM_TxtF ("%ld",Num);
  }

void HTM_Double (double Num)
  {
   char *Str;

   /***** Write from floating point number to string ****/
   Str_DoubleNumToStr (&Str,Num);

   /***** Write number from string to file *****/
   HTM_Txt (Str);

   /***** Free memory allocated for string *****/
   free (Str);
  }

void HTM_DoubleFewDigits (double Num)
  {
   char *Str;

   /***** Write from floating point number to string with few digits ****/
   Str_DoubleNumToStrFewDigits (&Str,Num);

   /***** Write number from string to file *****/
   HTM_Txt (Str);

   /***** Free memory allocated for string *****/
   free (Str);
  }

void HTM_Double2Decimals (double Num)
  {
   HTM_TxtF ("%.2lf",Num);
  }

void HTM_Percentage (double Percentage)
  {
   HTM_TxtF ("%5.2lf%%",Percentage);
  }
