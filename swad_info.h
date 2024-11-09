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

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

#define Inf_NUM_TYPES 9
typedef enum
  {
   Inf_UNKNOWN_TYPE	= 0,
   Inf_INFORMATION	= 1,
   Inf_TEACH_GUIDE	= 2,
   Inf_SYLLABUS_LEC	= 3,
   Inf_SYLLABUS_PRA	= 4,
   Inf_BIBLIOGRAPHY	= 5,
   Inf_FAQ		= 6,
   Inf_LINKS		= 7,
   Inf_ASSESSMENT	= 8,
  } Inf_Type_t;
#define Inf_DEFAULT_WHICH_SYLLABUS Inf_SYLLABUS_LEC

#define Inf_NUM_SOURCES 6
typedef enum
  {
   Inf_SRC_NONE		= 0,
   Inf_EDITOR		= 1,
   Inf_PLAIN_TEXT	= 2,
   Inf_RICH_TEXT	= 3,
   Inf_PAGE		= 4,
   Inf_URL		= 5,
  } Inf_Src_t;

struct Inf_FromDB
  {
   Inf_Src_t Src;	// Info source
   bool MustBeRead;	// Must be read by students?
  };

struct Inf_Info
  {
   Inf_Type_t Type;
   struct Inf_FromDB FromDB;
   char URL[WWW_MAX_BYTES_WWW + 1];
   bool MustBeRead[Inf_NUM_TYPES];	// Students must read info?
   bool ShowMsgMustBeRead;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Inf_ShowInfo (void);
void Inf_EditTree (void);
void Inf_ViewNodeAfterEdit (void);
void Inf_ReqChangeNode (void);
void Inf_ReqCreateNode (void);
void Inf_ReceiveChgNode (void);
void Inf_ReceiveNewNode (void);
void Inf_ReqRemNode (void);
void Inf_RemoveNode (void);
void Inf_HideNode (void);
void Inf_UnhideNode (void);
void Inf_MoveUpNode (void);
void Inf_MoveDownNode (void);
void Inf_MoveLeftNode (void);
void Inf_MoveRightNode (void);
void Inf_ExpandNodeSeeing (void);
void Inf_ContractNodeSeeing (void);
void Inf_ExpandNodeEditing (void);
void Inf_ContractNodeEditing (void);

//---------------------- Force students to read info? -------------------------
bool Inf_GetIfIMustReadAnyCrsInfoInThisCrs (void);
void Inf_WriteMsgYouMustReadInfo (void);
void Inf_ChangeForceReadInfo (void);
void Inf_ChangeIHaveReadInfo (void);

//-----------------------------------------------------------------------------
void Inf_BuildPathPage (long CrsCod,Inf_Type_t InfoType,char PathDir[PATH_MAX + 1]);
void Inf_WriteURLIntoTxtBuffer (char TxtBuffer[WWW_MAX_BYTES_WWW + 1]);
void Inf_SetInfoSrc (void);
void Inf_ConfigInfo (void);
void Inf_AsignInfoType (struct Inf_Info *Info);
Inf_Type_t Inf_GetParInfoType (void);
void Inf_PutParInfoType (void *InfoType);
Inf_Src_t Inf_GetInfoSrcFromForm (void);
Inf_Src_t Inf_GetInfoSrcFromDB (long CrsCod,Inf_Type_t InfoType);
void Inf_GetAndCheckInfoSrcFromDB (struct Inf_Info *Info);

void Inf_GetInfoTxtFromDB (long CrsCod,Inf_Type_t InfoType,
                           char InfoTxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1],
                           char InfoTxtMD  [Cns_MAX_BYTES_LONG_TEXT + 1]);

void Inf_EditPlainTxtInfo (void);
void Inf_EditRichTxtInfo (void);

void Inf_ReceivePlainTxtInfo (void);
void Inf_ReceiveRichTxtInfo (void);

void Inf_EditPagInfo (void);
void Inf_ReceivePagInfo (void);

void Inf_EditURLInfo (void);
void Inf_ReceiveURLInfo (void);

void Inf_EditorCourseInfo (void);
void Inf_EditorTeachingGuide (void);
void Inf_EditorBibliography (void);
void Inf_EditorFAQ (void);
void Inf_EditorLinks (void);
void Inf_EditorAssessment (void);

#endif
