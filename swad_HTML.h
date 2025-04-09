// swad_HTML.h: tables, divs

#ifndef _SWAD_HTM
#define _SWAD_HTM
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include <stdbool.h>		// For boolean type

#include "swad_constant.h"

/*****************************************************************************/
/********************************* Public types ******************************/
/*****************************************************************************/

typedef enum
  {
   HTM_Type_UNSIGNED,
   HTM_Type_LONG,
   HTM_Type_STRING,
   HTM_Type_CHAR,
  } HTM_Type_t;

typedef unsigned HTM_Attributes_t;
#define HTM_NO_ATTR		0
#define HTM_CHECKED 		(1 << 0)
#define HTM_SELECTED		(1 << 1)
#define HTM_REQUIRED		(1 << 2)
#define HTM_AUTOFOCUS		(1 << 3)
#define HTM_DISABLED		(1 << 4)
#define HTM_SUBMIT_ON_CHANGE	(1 << 5)
#define HTM_SUBMIT_ON_CLICK	(1 << 6)

#define HTM_NUM_HEAD_ALIGN 3
typedef enum
  {
   HTM_HEAD_LEFT   = 0,
   HTM_HEAD_CENTER = 1,
   HTM_HEAD_RIGHT  = 2,
  } HTM_HeadAlign;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void HTM_TITLE_Begin (void);
void HTM_TITLE_End (void);

void HTM_TABLE_Begin (const char *fmt,...);
void HTM_TABLE_BeginPadding (unsigned CellPadding);
void HTM_TABLE_BeginCenterPadding (unsigned CellPadding);
void HTM_TABLE_BeginCenter (void);
void HTM_TABLE_BeginWidePadding (unsigned CellPadding);
void HTM_TABLE_BeginWide (void);
void HTM_TABLE_BeginWideMarginPadding (unsigned CellPadding);
void HTM_TABLE_End (void);

void HTM_TBODY_Begin (const char *fmt,...);
void HTM_TBODY_End (void);

void HTM_TR_Begin (const char *fmt,...);
void HTM_TR_End (void);

void HTM_TH (const char *Txt,HTM_HeadAlign HeadAlign);
void HTM_TH_Begin (HTM_HeadAlign HeadAlign);
void HTM_TH_Span (const char *Title,HTM_HeadAlign HeadAlign,
                  unsigned RowSpan,unsigned ColSpan,
                  const char *ClassFmt,...);
void HTM_TH_Span_Begin (HTM_HeadAlign HeadAlign,
                        unsigned RowSpan,unsigned ColSpan,
                        const char *ClassFmt,...);
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

void HTM_SPAN_Begin (const char *fmt,...);
void HTM_SPAN_End (void);

void HTM_OL_Begin (void);
void HTM_OL_End (void);
void HTM_UL_Begin (const char *fmt,...);
void HTM_UL_End (void);
void HTM_LI_Begin (const char *fmt,...);
void HTM_LI_End (void);

void HTM_DL_Begin (void);
void HTM_DL_End (void);
void HTM_DT_Begin (void);
void HTM_DT_End (void);
void HTM_DD_Begin (void);
void HTM_DD_End (void);

void HTM_A_Begin (const char *fmt,...);
void HTM_A_End (void);

void HTM_SCRIPT_Begin (const char *URL,const char *CharSet);
void HTM_SCRIPT_End (void);

void HTM_PARAM (const char *Name,const char *fmt,...);

void HTM_FIELDSET_Begin (const char *fmt,...);
void HTM_FIELDSET_End (void);
void HTM_LEGEND (const char *Txt);

void HTM_LABEL_Begin (const char *fmt,...);
void HTM_LABEL_End (void);

void HTM_INPUT_TEXT (const char *Name,unsigned MaxLength,const char *Value,
		     HTM_Attributes_t Attributes,const char *fmt,...);
void HTM_INPUT_SEARCH (const char *Name,unsigned MaxLength,const char *Value,
	               const char *fmt,...);
void HTM_INPUT_TEL (const char *Name,const char *Value,
                    HTM_Attributes_t Attributes,const char *fmt,...);
void HTM_INPUT_EMAIL (const char *Name,unsigned MaxLength,const char *Value,
		      HTM_Attributes_t Attributes,const char *fmt,...);
void HTM_INPUT_URL (const char *Name,const char *Value,
		    HTM_Attributes_t Attributes,const char *fmt,...);
void HTM_INPUT_FILE (const char *Name,const char *Accept,
                      HTM_Attributes_t Attributes,const char *fmt,...);
void HTM_INPUT_BUTTON (const char *Name,const char *Value,const char *Attr);
void HTM_INPUT_IMAGE (const char *URL,const char *Icon,const char *Title,
	              const char *fmt,...);
void HTM_INPUT_PASSWORD (const char *Name,
			 const char *PlaceHolder,const char *AutoComplete,
			 HTM_Attributes_t Attributes,const char *fmt,...);
void HTM_INPUT_LONG (const char *Name,long Min,long Max,long Value,
                     HTM_Attributes_t Attributes,const char *fmt,...);
void HTM_INPUT_FLOAT (const char *Name,double Min,double Max,
		      double Step,	// Use 0 for "any"
		      double Value,
                      HTM_Attributes_t Attributes,const char *fmt,...);
void HTM_INPUT_RADIO (const char *Name,HTM_Attributes_t Attributes,
      		      const char *fmt,...);
void HTM_INPUT_CHECKBOX (const char *Name,HTM_Attributes_t Attributes,
			 const char *fmt,...);

void HTM_BUTTON_Submit_Begin (const char *Title,const char *fmt,...);
void HTM_BUTTON_Begin (const char *Title,const char *fmt,...);
void HTM_BUTTON_End (void);

void HTM_TEXTAREA_Begin (HTM_Attributes_t Attributes,const char *fmt,...);
void HTM_TEXTAREA_End (void);

void HTM_SELECT_Begin (HTM_Attributes_t Attributes,
                       const char *FuncsOnChange,
		       const char *fmt,...);
void HTM_SELECT_End (void);
void HTM_OPTGROUP_Begin (const char *Label);
void HTM_OPTGROUP_End (void);
void HTM_OPTION (HTM_Type_t Type,const void *ValuePtr,
                 HTM_Attributes_t Attributes,const char *fmt,...);

void HTM_IMG (const char *URL,const char *Icon,const char *Title,
	      const char *fmt,...);

void HTM_STRONG_Begin (void);
void HTM_STRONG_End (void);

void HTM_EM_Begin (void);
void HTM_EM_End (void);

void HTM_U_Begin (void);
void HTM_U_End (void);

void HTM_BR (void);

void HTM_TxtF (const char *fmt,...);
void HTM_Txt (const char *Txt);
void HTM_Char (char Ch);
void HTM_ParTxtPar (const char *Txt);
void HTM_ListSeparator (unsigned i,unsigned n);
void HTM_Semicolon (void);
void HTM_SP (void);
void HTM_NBSP (void);
void HTM_LT (void);
void HTM_GT (void);
void HTM_EllipsisDots (void);
void HTM_Colon (void);
void HTM_Dot (void);
void HTM_Comma (void);
void HTM_Hyphen (void);
void HTM_Asterisk (void);
void HTM_Slash (void);
void HTM_Question (void);
void HTM_Percent (void);
void HTM_OpenParenthesis (void);
void HTM_CloseParenthesis (void);
void HTM_OpenBracket (void);
void HTM_CloseBracket (void);
void HTM_Unsigned (unsigned Num);
void HTM_UnsignedColon (unsigned Num);
void HTM_Light0 (void);
void HTM_0 (void);
void HTM_Int (int Num);
void HTM_UnsignedLong (unsigned long Num);
void HTM_Long (long Num);
void HTM_Double (double Num);
void HTM_DoubleFewDigits (double Num);
void HTM_Double2Decimals (double Num);
void HTM_DoublePartOfUnsigned (double Part,unsigned Whole);
void HTM_DoublePartOfDouble (double Part,double Whole);
void HTM_DoubleRange (double Min,double Max);
void HTM_Percentage (double Percentage);

void HTM_TD_Txt_Left (const char *Txt);
void HTM_TD_Txt_Right (const char *Txt);
void HTM_TD_TxtColon (const char *Txt);
void HTM_TD_LINE_TOP_Txt (const char *Txt);
void HTM_TD_Unsigned (unsigned Num);
void HTM_TD_LINE_TOP_Unsigned (unsigned Num);
void HTM_TD_UnsignedLong (unsigned long Num);
void HTM_TD_LINE_TOP_UnsignedLong (unsigned long Num);
void HTM_TD_Percentage (unsigned Num,unsigned NumTotal);
void HTM_TD_LINE_TOP_Percentage (unsigned Num,unsigned NumTotal);
void HTM_TD_Ratio (unsigned Num,unsigned NumTotal);
void HTM_TD_LINE_TOP_Ratio (unsigned Num,unsigned NumTotal);
void HTM_TD_Double2Decimals (double Num);
void HTM_TD_LINE_TOP_Double2Decimals (double Num);

void HTM_ElementEnd (void);

#endif
