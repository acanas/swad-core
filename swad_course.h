// swad_course.h: edition of courses

#ifndef _SWAD_CRS
#define _SWAD_CRS
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
#include <stdbool.h>		// For boolean type

#include "swad_constant.h"
#include "swad_degree.h"
#include "swad_hierarchy_level.h"
#include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Crs_MAX_COURSES_PER_USR		100 // Used in list of my courses and list of distinct courses in sent or received messages

#define Crs_MAX_CHARS_INSTITUTIONAL_COD  (16 - 1)	// 15
#define Crs_MAX_BYTES_INSTITUTIONAL_COD  ((Crs_MAX_CHARS_INSTITUTIONAL_COD + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 255

#define Crs_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS  6
#define Crs_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS 12
#define Crs_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS 60

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct Crs_Course
  {
   long Cod;			// Course code
   long PrtCod;			// Parent (degree) code
   char InstitutionalCod[Crs_MAX_BYTES_INSTITUTIONAL_COD + 1];	// Institutional code of the course
   unsigned Year;		// Year: 0 (optatives), 1, 2, 3...
   Hie_Status_t Status;		// Course status
   long RequesterUsrCod;	// User code of the person who requested the creation of this course
   char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1];	// Short name of course
   char FullName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1];	// Full name of course
  };

struct Crs_ListCrss
  {
   unsigned Num;		// Number of courses
   struct Crs_Course *Lst;	// List of courses
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Crs_ShowIntroduction (void);

unsigned Crs_GetCachedNumCrssInSys (void);

void Crs_FlushCacheNumCrssInCty (void);
unsigned Crs_GetNumCrssInCty (long CtyCod);
unsigned Crs_GetCachedNumCrssInCty (long CtyCod);

void Crs_FlushCacheNumCrssInIns (void);
unsigned Crs_GetNumCrssInIns (long InsCod);
unsigned Crs_GetCachedNumCrssInIns (long InsCod);

void Crs_FlushCacheNumCrssInCtr (void);
unsigned Crs_GetNumCrssInCtr (long CtrCod);
unsigned Crs_GetCachedNumCrssInCtr (long CtrCod);

void Crs_FlushCacheNumCrssInDeg (void);
unsigned Crs_GetNumCrssInDeg (long DegCod);
unsigned Crs_GetCachedNumCrssInDeg (long DegCod);

unsigned Crs_GetCachedNumCrssWithUsrs (Rol_Role_t Role);

void Crs_WriteSelectorOfCourse (void);
void Crs_ShowCrssOfCurrentDeg (void);
void Crs_FreeListCoursesInCurrentDegree (void);
void Crs_WriteSelectorMyCoursesInBreadcrumb (void);

void Crs_EditCourses (void);

void Crs_ReceiveFormReqCrs (void);
void Crs_ReceiveFormNewCrs (void);

void Crs_RemoveCourse (void);
bool Crs_GetCourseDataByCod (struct Crs_Course *Crs);
void Crs_RemoveCourseCompletely (long CrsCod);
void Crs_ChangeInsCrsCod (void);
void Crs_ChangeCrsYear (void);

void Crs_UpdateInstitutionalCrsCod (struct Crs_Course *Crs,
                                    const char *NewInstitutionalCrsCod);
void Crs_UpdateCrsYear (struct Crs_Course *Crs,unsigned NewYear);

void Crs_RenameCourseShort (void);
void Crs_RenameCourseFull (void);
void Crs_RenameCourse (struct Crs_Course *Crs,Cns_ShrtOrFullName_t ShrtOrFullName);

void Crs_ChangeCrsStatus (void);
void Crs_ContEditAfterChgCrs (void);

void Crs_PutIconToSelectMyCoursesInBreadcrumb (void);
void Crs_PutIconToSelectMyCourses (__attribute__((unused)) void *Args);

void Crs_ReqSelectOneOfMyCourses (void);

void Crs_GetAndWriteCrssOfAUsr (const struct Usr_Data *UsrDat,Rol_Role_t Role);

void Crs_ListCrssFound (MYSQL_RES **mysql_res,unsigned NumCrss);

void Crs_UpdateCrsLast (void);

void Crs_PutLinkToRemoveOldCrss (void);
void Crs_AskRemoveOldCrss (void);
void Crs_RemoveOldCrss (void);

#endif
