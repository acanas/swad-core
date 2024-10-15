// swad_resource.h: resources for course program and rubrics

#ifndef _SWAD_RSC
#define _SWAD_RSC
/*
    SWAD (Shared Workspace At a Distance),
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_form.h"
#include "swad_HTML.h"
#include "swad_string.h"
#include "swad_user.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Rsc_MAX_CHARS_RESOURCE_TITLE	(128 - 1)	// 127
#define Rsc_MAX_BYTES_RESOURCE_TITLE	((Rsc_MAX_CHARS_RESOURCE_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Rsc_NUM_TYPES 24
typedef enum
  {
  Rsc_NONE,
  Rsc_INFORMATION,	// Link to course information
  Rsc_TEACH_GUIDE,	// Link to course teaching guide
  Rsc_LECTURES,		// Link to course topics (lectures)
  Rsc_PRACTICALS,	// Link to course topics (practicals)
  Rsc_BIBLIOGRAPHY,	// Link to course bibliography
  Rsc_FAQ,		// Link to course FAQ
  Rsc_LINKS,		// Link to course links
  Rsc_ASSESSMENT,	// Link to course assessment system
  Rsc_TIMETABLE,	// Link to timetable
  Rsc_ASSIGNMENT,	// Link to assigment
  Rsc_PROJECT,		// Link to project
  Rsc_CALL_FOR_EXAM,	// Link to call for exam
  Rsc_TEST,		// Link to self-assessment test
  Rsc_EXAM,		// Link to exam
  Rsc_GAME,		// Link to game
  Rsc_RUBRIC,		// Link to rubric
  Rsc_DOCUMENT,		// Link to document file
  Rsc_MARKS,		// Link to marks file
  Rsc_GROUPS,		// Link to group type
  Rsc_TEACHER,		// Link to teacher's record card
  Rsc_ATT_EVENT,	// Link to attendance event
  Rsc_FORUM_THREAD,	// Link to forum thread
  Rsc_SURVEY,		// Link to survey
  } Rsc_Type_t;

struct Rsc_Link
  {
   Rsc_Type_t Type;
   long Cod;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Rsc_ShowClipboard (void);
void Rsc_ShowClipboardToChangeLink (const struct Rsc_Link *CurrentLink);
void Rsc_WriteLinkName (const struct Rsc_Link *Link,Frm_PutForm_t PutFormToGo);

void Rsc_GetResourceTitleFromLink (const struct Rsc_Link *Link,
                                   char Title[Rsc_MAX_BYTES_RESOURCE_TITLE + 1]);

void Rsc_GetLinkDataFromRow (MYSQL_RES *mysql_res,struct Rsc_Link *Link);
Rsc_Type_t Rsc_GetTypeFromString (const char *Str);

bool Rsc_GetParLink (struct Rsc_Link *Link);

Usr_Can_t Rsc_CheckIfICanGetLink (void);

#endif
