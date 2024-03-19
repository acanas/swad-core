// swad_info.h: info about course

#ifndef _SWAD_INF
#define _SWAD_INF
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

#include "swad_constant.h"
#include "swad_syllabus.h"

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

#define Inf_NUM_TYPES 8
typedef enum
  {
   Inf_INTRODUCTION   = 0,
   Inf_TEACHING_GUIDE = 1,
   Inf_LECTURES       = 2,
   Inf_PRACTICALS     = 3,
   Inf_BIBLIOGRAPHY   = 4,
   Inf_FAQ            = 5,
   Inf_LINKS          = 6,
   Inf_ASSESSMENT     = 7,
  } Inf_Type_t;

#define Inf_NUM_SOURCES 6
typedef enum
  {
   Inf_NONE       = 0,
   Inf_EDITOR     = 1,
   Inf_PLAIN_TEXT = 2,
   Inf_RICH_TEXT  = 3,
   Inf_PAGE       = 4,
   Inf_URL        = 5,
  } Inf_Src_t;

struct Inf_Info
  {
   Inf_Type_t Type;
   char URL[Cns_MAX_BYTES_WWW + 1];
   bool MustBeRead[Inf_NUM_TYPES];	// Students must read info?
   bool ShowMsgMustBeRead;
  };

struct Inf_FromDB
  {
   Inf_Src_t Src;	// Info source
   bool MustBeRead;	// Must be read by students?
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Inf_ShowInfo (void);
void Inf_PutIconToEditInfo (void *Type);
void Inf_WriteMsgYouMustReadInfo (void);
void Inf_ChangeForceReadInfo (void);
void Inf_ChangeIHaveReadInfo (void);
bool Inf_GetIfIMustReadAnyCrsInfoInThisCrs (void);
void Inf_BuildPathPage (long CrsCod,Inf_Type_t InfoType,char PathDir[PATH_MAX + 1]);
void Inf_WriteURLIntoTxtBuffer (char TxtBuffer[Cns_MAX_BYTES_WWW + 1]);
void Inf_SetInfoSrc (void);
void Inf_FormsToSelSendInfo (void);
void Inf_FormToEnterIntegratedEditor (struct Syl_Syllabus *Syllabus,Inf_Src_t InfoSrc);
void Inf_FormToEnterPlainTextEditor (struct Syl_Syllabus *Syllabus,Inf_Src_t InfoSrc);
void Inf_FormToEnterRichTextEditor (struct Syl_Syllabus *Syllabus,Inf_Src_t InfoSrc);
void Inf_FormToSendPage (struct Syl_Syllabus *Syllabus,Inf_Src_t InfoSrc);
void Inf_FormToSendURL (struct Syl_Syllabus *Syllabus,Inf_Src_t InfoSrc);
Inf_Src_t Inf_GetInfoSrcFromForm (void);
Inf_Src_t Inf_GetInfoSrcFromDB (long CrsCod,Inf_Type_t InfoType);
void Inf_GetAndCheckInfoSrcFromDB (struct Syl_Syllabus *Syllabus,
                                   long CrsCod,
                                   Inf_Type_t InfoType,
                                   struct Inf_FromDB *FromDB);

void Inf_GetInfoTxtFromDB (long CrsCod,Inf_Type_t InfoType,
                           char InfoTxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1],
                           char InfoTxtMD  [Cns_MAX_BYTES_LONG_TEXT + 1]);

void Inf_EditPlainTxtInfo (void);
void Inf_EditRichTxtInfo (void);
void Inf_RecAndChangePlainTxtInfo (void);
void Inf_RecAndChangeRichTxtInfo (void);
void Inf_ReceiveURLInfo (void);
void Inf_ReceivePagInfo (void);

void Inf_EditorCourseInfo (void);
void Inf_EditorTeachingGuide (void);
void Inf_EditorBibliography (void);
void Inf_EditorFAQ (void);
void Inf_EditorLinks (void);
void Inf_EditorAssessment (void);

#endif
