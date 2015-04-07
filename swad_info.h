// swad_info.c: info about course

#ifndef _SWAD_INF
#define _SWAD_INF
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include "swad_bool.h"
#include "swad_constant.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

#define Inf_NUM_INFO_TYPES 8
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
  } Inf_InfoType_t;

#define Inf_NUM_INFO_SOURCES 6
typedef enum
  {
   Inf_INFO_SRC_NONE		= 0,
   Inf_INFO_SRC_EDITOR		= 1,
   Inf_INFO_SRC_PLAIN_TEXT	= 2,
   Inf_INFO_SRC_RICH_TEXT	= 3,
   Inf_INFO_SRC_PAGE		= 4,
   Inf_INFO_SRC_URL		= 5,
  } Inf_InfoSrc_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Inf_ShowInfo (void);
void Inf_WriteMsgYouMustReadInfo (void);
void Inf_ChangeForceReadInfo (void);
void Inf_ChangeIHaveReadInfo (void);
bool Inf_CheckIfIHaveReadInfo (Inf_InfoType_t InfoType);
bool Inf_GetIfIMustReadAnyCrsInfoInThisCrs (void);
void Inf_RemoveUsrFromCrsInfoRead (long UsrCod,long CrsCod);
int Inf_WritePageIntoHTMLBuffer (Inf_InfoType_t InfoType,char **HTMLBuffer);
void Inf_WriteURLIntoTxtBuffer (Inf_InfoType_t InfoType,char TxtBuffer[Cns_MAX_BYTES_URL+1]);
void Inf_SetInfoSrc (void);
void Inf_FormsToSelSendInfo (void);
void Inf_FormToEnterIntegratedEditor (Inf_InfoSrc_t InfoSrc,Inf_InfoType_t InfoType);
void Inf_FormToEnterPlainTextEditor (Inf_InfoSrc_t InfoSrc,Inf_InfoType_t InfoType);
void Inf_FormToEnterRichTextEditor (Inf_InfoSrc_t InfoSrc,Inf_InfoType_t InfoType);
void Inf_FormToSendPage (Inf_InfoSrc_t InfoSrc,Inf_InfoType_t InfoType);
void Inf_FormToSendURL (Inf_InfoSrc_t InfoSrc,Inf_InfoType_t InfoType);
Inf_InfoType_t Inf_AsignInfoType (void);
Inf_InfoSrc_t Inf_GetInfoSrcFromForm (void);
void Inf_SetInfoSrcIntoDB (long CrsCod,Inf_InfoType_t InfoType,Inf_InfoSrc_t InfoSrc);
void Inf_GetInfoSrcFromDB (long CrsCod,Inf_InfoType_t InfoType,Inf_InfoSrc_t *InfoSrc,bool *MustBeRead);
Inf_InfoType_t Inf_ConvertFromStrDBToInfoType (const char *StrInfoTypeDB);
Inf_InfoSrc_t Inf_ConvertFromStrDBToInfoSrc (const char *StrInfoSrcDB);
void Inf_SetInfoTxtIntoDB (long CrsCod,Inf_InfoType_t InfoType,
                           const char *InfoTxtHTML,const char *InfoTxtMD);
void Inf_GetInfoTxtFromDB (Inf_InfoType_t InfoType,
                           char *InfoTxtHTML,char *InfoTxtMD,size_t MaxLength);
bool Inf_CheckIfInfoTxtIsNotEmpty (long CrsCod,Inf_InfoType_t InfoType);
int Inf_WritePlainTextIntoHTMLBuffer (Inf_InfoType_t InfoType,char **HTMLBuffer);
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
