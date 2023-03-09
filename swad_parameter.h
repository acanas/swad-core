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

typedef enum
  {
   Par_None,		// Special code
   Par_AgdCod,		// Agenda event code
   Par_AnnCod,		// Announcement code
   Par_AsgCod,		// Assignment code
   Par_AttCod,		// Attendance event code
   Par_BanCod,		// Banner code
   Par_BldCod,		// Building code
   Par_CrsCod,		// Course code
   Par_CtrCod,		// Center code
   Par_CtyCod,		// Country code
   Par_DegCod,		// Degree code
   Par_DptCod,		// Department code
   Par_ExaCod,		// Exam code in calls-for-exam/exam
   Par_FilCod,		// File code
   Par_FldCod,		// Field code in course record
   Par_GamCod,		// Game code
   Par_GrpCod,		// Group code
   Par_GrpTypCod,	// Group type code
   Par_HldCod,		// Holiday code
   Par_InsCod,		// Institution code
   Par_ItmCod,		// Item code in course program
   Par_LnkCod,		// Link code
   Par_MaiCod,		// Domain mail code
   Par_MchCod,		// Match code in game
   Par_MsgCod,		// Message code
   Par_NotCod,		// Note code in timeline
   Par_OrgActCod,	// Original action code
   Par_OthCrsCod,	// Other course code
   Par_OthCtrCod,	// Other center code
   Par_OthCtyCod,	// Other country code
   Par_OthDegCod,	// Other degree code
   Par_OthDegTypCod,	// Other degree type code
   Par_OthHieCod,	// Other hierarchy code
   Par_OthInsCod,	// Other institution code
   Par_PlcCod,		// Place code
   Par_PlgCod,		// Plugin code
   Par_PrjCod,		// Project code
   Par_PrnCod,		// Print code in assessment test
   Par_PstCod,		// Post code in forum
   Par_PubCod,		// Publication code in timeline
   Par_QstCod,		// Question code in assessment/survey
   Par_RooCod,		// Room code
   Par_RscCod,		// Resource code in course program
   Par_RubCod,		// Rubric code
   Par_SesCod,		// Session code in exam
   Par_SvyCod,		// Syrvey code
   Par_TagCod,		// Tag code in assessment question
   Par_ThrCod,		// Thread code in forum
  } Par_Code_t;

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
long Par_GetAndCheckParCode (Par_Code_t ParCod);
long Par_GetAndCheckParCodeMin (Par_Code_t ParCod,long MinCodAllowed);
long Par_GetParCode (Par_Code_t ParCod);
bool Par_GetParBool (const char *ParName);
unsigned Par_GetParHTML (const char *ParName,char *ParValue,size_t MaxBytes);
unsigned Par_GetParMultiToText (const char *ParName,char *ParValue,size_t MaxBytes);
unsigned Par_GetParAndChangeFormat (const char *ParName,char *ParValue,size_t MaxBytes,
                                    Str_ChangeTo_t ChangeTo,bool RemoveLeadingAndTrailingSpaces);

bool Par_GetNextStrUntilSeparParMult (const char **StrSrc,char *StrDst,size_t LongMax);
bool Par_GetNextStrUntilComma (const char **StrSrc,char *StrDst,size_t LongMax);
void Par_ReplaceSeparatorMultipleByComma (char *Str);

//------------------------------ Put parameters -------------------------------
void Par_PutParUnsigned (const char *Id,const char *ParName,unsigned Value);
void Par_PutParUnsignedDisabled (const char *Id,const char *ParName,unsigned Value);
void Par_PutParCode (Par_Code_t ParCod,long Cod);
void Par_PutParLong (const char *Id,const char *ParName,long Value);
void Par_PutParChar (const char *ParName,char Value);
void Par_PutParString (const char *Id,const char *ParName,const char *Value);
void Par_PutParOrder (unsigned SelectedOrder);

//----------------------------- Client IP address -----------------------------
void Par_SetIP (void);
const char *Par_GetIP (void);

#endif
