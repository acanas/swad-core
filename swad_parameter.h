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

typedef enum
  {
   Par_AgdCod,
   Par_AnnCod,
   Par_AsgCod,
   Par_AttCod,
   Par_BanCod,
   Par_BldCod,
   Par_DptCod,
   Par_ExaCod,
   Par_FilCod,
   Par_FilterCrsCod,
   Par_GamCod,
   Par_GrpCod,
   Par_GrpTypCod,
   Par_HldCod,
   Par_LnkCod,
   Par_MaiCod,
   Par_MchCod,
   Par_MsgCod,
   Par_NotCod,
   Par_OrgActCod,
   Par_OthCtrCod,
   Par_OthCtyCod,
   Par_OthDegCod,
   Par_OthDegTypCod,
   Par_OthHieCod,
   Par_OthInsCod,
   Par_PlcCod,
   Par_PlgCod,
   Par_PstCod,
   Par_PubCod,
   Par_QstCod,
   Par_RscCod,
   Par_SesCod,
   Par_SvyCod,
   Par_ThrCod,
  } Par_Code_t;

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

//------------------------------ Get parameters -------------------------------
unsigned Par_GetParText (const char *ParamName,char *ParamValue,size_t MaxBytes);
unsigned long Par_GetParUnsignedLong (const char *ParamName,
                                      unsigned long Min,
                                      unsigned long Max,
                                      unsigned long Default);
long Par_GetParLong (const char *ParamName);
long Par_GetAndCheckParCode (Par_Code_t ParCod);
long Par_GetAndCheckParCodeMin (Par_Code_t ParCod,long MinCodAllowed);
long Par_GetParCode (Par_Code_t ParCod);
bool Par_GetParBool (const char *ParamName);
unsigned Par_GetParHTML (const char *ParamName,char *ParamValue,size_t MaxBytes);
unsigned Par_GetParMultiToText (const char *ParamName,char *ParamValue,size_t MaxBytes);
unsigned Par_GetParAndChangeFormat (const char *ParamName,char *ParamValue,size_t MaxBytes,
                                    Str_ChangeTo_t ChangeTo,bool RemoveLeadingAndTrailingSpaces);

bool Par_GetNextStrUntilSeparParamMult (const char **StrSrc,char *StrDst,size_t LongMax);
bool Par_GetNextStrUntilComma (const char **StrSrc,char *StrDst,size_t LongMax);
void Par_ReplaceSeparatorMultipleByComma (char *Str);

//------------------------------ Put parameters -------------------------------
void Par_PutParUnsigned (const char *Id,const char *ParamName,unsigned Value);
void Par_PutParUnsignedDisabled (const char *Id,const char *ParamName,unsigned Value);
void Par_PutParCode (Par_Code_t ParCod,long Cod);
void Par_PutParLong (const char *Id,const char *ParamName,long Value);
void Par_PutParChar (const char *ParamName,char Value);
void Par_PutParString (const char *Id,const char *ParamName,const char *Value);
void Par_PutParOrder (unsigned SelectedOrder);

//----------------------------- Client IP address -----------------------------
void Par_SetIP (void);
const char *Par_GetIP (void);

#endif
