// swad_parameter.h: CGI parameters

#ifndef _SWAD_PAR
#define _SWAD_PAR
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_action.h"
#include "swad_string.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

struct StartLength
  {
   unsigned long Start;
   size_t Length;
  };

struct Par_Param
  {
   struct StartLength Name;		// Parameter name
   struct StartLength FileName;		// optional, present only when uploading files
   struct StartLength ContentType;	// optional, present only when uploading files
   struct StartLength Value;		// Parameter value or file content
   struct Par_Param *Next;
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
void Par_CreateListOfPars (void);
void Par_FreePars (void);
unsigned Par_GetPar (Par_ParamType_t ParType,const char *ParName,
                     char *ParValue,size_t MaxBytes,
                     struct Par_Param **ParPtr);

void Par_GetMainPars (void);

//------------------------------ Get parameters -------------------------------
unsigned Par_GetParText (const char *ParName,char *ParValue,size_t MaxBytes);
unsigned long Par_GetParUnsignedLong (const char *ParName,
                                      unsigned long Min,
                                      unsigned long Max,
                                      unsigned long Default);
long Par_GetParLong (const char *ParName);
bool Par_GetParBool (const char *ParName);
unsigned Par_GetParHTML (const char *ParName,char *ParValue,size_t MaxBytes);
unsigned Par_GetParMultiToText (const char *ParName,char *ParValue,size_t MaxBytes);
unsigned Par_GetParAndChangeFormat (const char *ParName,char *ParValue,size_t MaxBytes,
                                    Str_ChangeTo_t ChangeTo,
                                    Str_RemoveSpaces_t RemoveLeadingAndTrailingSpaces);

bool Par_GetNextStrUntilSeparParMult (const char **StrSrc,char *StrDst,
				      size_t LongMax);
bool Par_GetNextStrUntilComma (const char **StrSrc,char *StrDst,size_t LongMax);
void Par_ReplaceSeparatorMultipleByComma (char *Str);

unsigned Par_CountNumCodesInList (const char *List);

//------------------------------ Put parameters -------------------------------
void Par_PutParUnsigned (const char *Id,const char *ParName,unsigned ParValue);
void Par_PutParUnsignedDisabled (const char *Id,const char *ParName,unsigned ParValue);
void Par_PutParLong (const char *Id,const char *ParName,long ParValue);
void Par_PutParChar (const char *ParName,char ParValue);
void Par_PutParString (const char *Id,const char *ParName,const char *ParValue);
void Par_PutParOrder (unsigned SelectedOrder);

//----------------------------- Client IP address -----------------------------
void Par_SetIP (void);
const char *Par_GetIP (void);

#endif
