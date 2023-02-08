// swad_parameter.h: CGI parameters

#ifndef _SWAD_PAR
#define _SWAD_PAR
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Cañas Vargas

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

#include "swad_string.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

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
   Par_METHOD_GET,
   Par_METHOD_POST
  } Par_Method_t;

typedef enum
  {
   Par_PARAM_SINGLE,
   Par_PARAM_MULTIPLE,
  } Par_ParamType_t; // Parameter is present only one time / multiple times

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

Act_Content_t Par_GetContentReceivedByCGI (void);

bool Par_GetQueryString (void);
Par_Method_t Par_GetMethod (void);
void Par_CreateListOfParams (void);
void Par_FreeParams (void);
unsigned Par_GetParameter (Par_ParamType_t ParamType,const char *ParamName,
                           char *ParamValue,size_t MaxBytes,
                           struct Param **ParamPtr);

void Par_GetMainParams (void);

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
void Par_ReplaceSeparatorMultipleByComma (char *Str);

void Par_PutHiddenParamUnsigned (const char *Id,const char *ParamName,unsigned Value);
void Par_PutHiddenParamUnsignedDisabled (const char *Id,const char *ParamName,unsigned Value);
void Par_PutHiddenParamLong (const char *Id,const char *ParamName,long Value);
void Par_PutHiddenParamChar (const char *ParamName,char Value);
void Par_PutHiddenParamString (const char *Id,const char *ParamName,
                               const char *Value);
void Par_PutHiddenParamOrder (unsigned SelectedOrder);

//----------------------------- Client IP address -----------------------------
void Par_SetIP (void);
const char *Par_GetIP (void);

#endif
