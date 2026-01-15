// swad_resource_type.h: definition of types for resources

#ifndef _SWAD_RSC_TYP
#define _SWAD_RSC_TYP
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_constant.h"
#include "swad_hidden_visible.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/


#define Rsc_MAX_CHARS_RESOURCE_TITLE	(128 - 1)	// 127
#define Rsc_MAX_BYTES_RESOURCE_TITLE	((Rsc_MAX_CHARS_RESOURCE_TITLE + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 2047

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

struct Rsc_Resource
  {
   struct Rsc_Link Link;
   char Title[Rsc_MAX_BYTES_RESOURCE_TITLE + 1];
  };

#endif

