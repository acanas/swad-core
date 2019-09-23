// swad_parameter.h: CGI parameters

#ifndef _SWAD_PAR
#define _SWAD_PAR
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

#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For size_t

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Par_MAX_BYTES_BOUNDARY_WITHOUT_CR_LF	(128 - 1)
#define Par_MAX_BYTES_BOUNDARY_WITH_CR_LF	(2 + Par_MAX_BYTES_BOUNDARY_WITHOUT_CR_LF)

struct StartLength
  {
   unsigned long Start;
   size_t Length;
  };
struct Param
  {
   struct StartLength Name;		// Parameter name
   struct StartLength FileName;		// optional, present only when uploading files
   struct StartLength ContentType;	// optional, present only when uploading files
   struct StartLength Value;		// Parameter value or file content
   struct Param *Next;
  };

typedef enum
  {
   Par_PARAM_SINGLE,
   Par_PARAM_MULTIPLE,
  } tParamType; // Parameter is present only one time / multiple times

#define Par_SEPARATOR_PARAM_MULTIPLE	((char) 0x0A)		// Must be 1 <= character <= 31

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

bool Par_GetQueryString (void);
void Par_CreateListOfParams (void);
void Par_FreeParams (void);
unsigned Par_GetParameter (tParamType ParamType,const char *ParamName,
                           char *ParamValue,size_t MaxBytes,
                           struct Param **ParamPtr);

void Par_GetMainParameters (void);

unsigned Par_GetParToText (const char *ParamName,char *ParamValue,size_t MaxBytes);
unsigned long Par_GetParToUnsignedLong (const char *ParamName,
                                        unsigned long Min,
                                        unsigned long Max,
                                        unsigned long Default);
long Par_GetParToLong (const char *ParamName);
bool Par_GetParToBool (const char *ParamName);
unsigned Par_GetParToHTML (const char *ParamName,char *ParamValue,size_t MaxBytes);
unsigned Par_GetParMultiToText (const char *ParamName,char *ParamValue,size_t MaxBytes);
unsigned Par_GetParAndChangeFormat (const char *ParamName,char *ParamValue,size_t MaxBytes,
                                    Str_ChangeTo_t ChangeTo,bool RemoveLeadingAndTrailingSpaces);

bool Par_GetNextStrUntilSeparParamMult (const char **StrSrc,char *StrDst,size_t LongMax);
bool Par_GetNextStrUntilComma (const char **StrSrc,char *StrDst,size_t LongMax);
void Par_ReplaceSeparatorMultipleByComma (const char *StrSrc,char *StrDst);
void Par_ReplaceCommaBySeparatorMultiple (char *Str);

void Par_PutHiddenParamUnsigned (const char *ParamName,unsigned Value);
void Par_PutHiddenParamLong (const char *ParamName,long Value);
void Par_PutHiddenParamChar (const char *ParamName,char Value);
void Par_PutHiddenParamString (const char *ParamName,const char *Value);

#endif
