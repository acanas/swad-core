// swad_program_resource.h: course program (resources)

#ifndef _SWAD_PRG_RSC
#define _SWAD_PRG_RSC
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define PrgRsc_MAX_CHARS_PROGRAM_RESOURCE_TITLE	(128 - 1)	// 127
#define PrgRsc_MAX_BYTES_PROGRAM_RESOURCE_TITLE	((PrgRsc_MAX_CHARS_PROGRAM_RESOURCE_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

struct PrgRsc_Rsc
  {
   long Cod;
   unsigned Ind;	// 1, 2, 3...
  };

struct PrgRsc_Resource
  {
   long ItmCod;
   struct PrgRsc_Rsc Rsc;
   bool Hidden;
   char Title[PrgRsc_MAX_BYTES_PROGRAM_RESOURCE_TITLE + 1];
  };

#define PrgRsc_NUM_TYPES 10
typedef enum
  {
  PrgRsc_NONE,
  // gui TEACHING_GUIDE	// Link to teaching guide
  // bib BIBLIOGRAPHY	// Link to bibliography
  // faq FAQ		// Link to FAQ
  // lnk LINKS		// Link to links
  // tmt TIMETABLE	// Link to timetable
  PrgRsc_ASSIGNMENT,
  // prj PROJECT	// A project is only for some students
  PrgRsc_CALL_FOR_EXAM,
  // tst TEST		// User selects tags, teacher should select
  PrgRsc_EXAM,
  PrgRsc_GAME,
  PrgRsc_SURVEY,
  PrgRsc_DOCUMENT,
  PrgRsc_MARKS,
  // grp GROUPS		// ??? User select groups
  PrgRsc_ATTENDANCE_EVENT,
  PrgRsc_FORUM_THREAD,
  } PrgRsc_Type_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void PrgRsc_ViewResourcesAfterEdit (void);
void PrgRsc_EditResources (void);

void PrgRsc_ListItemResources (Prg_ListingType_t ListingType,long ItmCod);

void PrgRsc_CreateResource (void);

void PrgRsc_RenameResource (void);

void PrgRsc_ReqRemResource (void);
void PrgRsc_RemoveResource (void);

void PrgRsc_HideResource (void);
void PrgRsc_UnhideResource (void);

void PrgRsc_MoveUpResource (void);
void PrgRsc_MoveDownResource (void);

void PrgRsc_ChangeResourceLink (void);

#endif
