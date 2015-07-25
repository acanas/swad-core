// swad_course.c: edition of courses

#ifndef _SWAD_CRS
#define _SWAD_CRS
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_bool.h"
#include "swad_degree.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Crs_MAX_COURSES_PER_USR		100 // Used in list of my courses and list of distinct courses in sent or received messages

#define Crs_LENGTH_INSTITUTIONAL_CRS_COD	  7
#define Crs_MAX_LENGTH_COURSE_SHORT_NAME	 32
#define Crs_MAX_LENGTH_COURSE_FULL_NAME		127

#define Crs_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS  6
#define Crs_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS 12
#define Crs_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS 60

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   Crs_STATUS_BIT_PENDING = (1 << 0),	// Course is requested, but not yet activated
   Crs_STATUS_BIT_REMOVED = (1 << 1),	// Course has been removed
  } Crs_Status_Bits_t;
typedef unsigned Crs_Status_t;

#define Crs_NUM_STATUS_TXT	4
typedef enum
  {
   Crs_STATUS_UNKNOWN = 0,	// Other
   Crs_STATUS_ACTIVE  = 1,	// 00 (Status == 0)
   Crs_STATUS_PENDING = 2,	// 01 (Status == Crs_STATUS_BIT_PENDING)
   Crs_STATUS_REMOVED = 3,	// 1- (Status & Crs_STATUS_BIT_REMOVED)
  } Crs_StatusTxt_t;

typedef enum
  {
   Crs_ACTIVE_COURSES = 0,			// Courses with all Status bits == 0
   Crs_ALL_COURSES_EXCEPT_REMOVED = 1,	// Courses with Status bit Crs_STATUS_BIT_REMOVED == 0
  } Crs_WhatCourses_t;

struct Course
  {
   long CrsCod;
   char InstitutionalCrsCod[Crs_LENGTH_INSTITUTIONAL_CRS_COD+1];	// Institutional code of the course
   long DegCod;
   unsigned Year;					// Year: 0 (optatives), 1, 2, 3...
   unsigned Semester;					// Semester: 0 (full year), 1 or 2
   bool AllowDirectLogIn;				// Students may enter directly, or must enter via external plugin?
   Crs_Status_t Status;					// Course status
   long RequesterUsrCod;				// User code of the person who requested the creation of this course
   char ShortName[Crs_MAX_LENGTH_COURSE_SHORT_NAME+1];	// Short name of course
   char FullName[Crs_MAX_LENGTH_COURSE_FULL_NAME+1];	// Full name of course
   unsigned NumStds;					// Number of students
   unsigned NumTchs;					// Number of teachers
   unsigned NumUsrs;					// Number of users (students + teachers)
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Crs_ShowIntroduction (void);
void Crs_PrintConfiguration (void);
void Crs_ChangeCourseConfig (void);

unsigned Crs_GetNumCrssTotal (void);
unsigned Crs_GetNumCrssInCty (long CtyCod);
unsigned Crs_GetNumCrssInIns (long InsCod);
unsigned Crs_GetNumCrssInCtr (long CtrCod);
unsigned Crs_GetNumCrssInDeg (long DegCod);
unsigned Crs_GetNumCrssWithUsrs (Rol_Role_t Role,const char *SubQuery);

void Crs_WriteSelectorOfCourse (void);
void Crs_ShowCrssOfCurrentDeg (void);
void Crs_ReqEditCourses (void);
void Crs_FreeListCoursesInDegree (struct Degree *Deg);

void Crs_WriteSelectorMyCourses (void);

void Crs_RecFormReqCrs (void);
void Crs_RecFormNewCrs (void);

void Crs_RemoveCourse (void);
bool Crs_GetDataOfCourseByCod (struct Course *Crs);
void Crs_GetShortNamesByCod (long CrsCod,char *CrsShortName,char *DegShortName);
void Crs_RemoveCourseCompletely (long CrsCod);
void Crs_ChangeInsCrsCod (void);
void Crs_ChangeCrsDegree (void);
void Crs_ChangeCrsYear (void);
void Crs_ChangeCrsSemester (void);
void Crs_UpdateCurrentInstitutionalCrsCod (struct Course *Crs,const char *NewInstitutionalCrsCod);
void Crs_RenameCourseShort (void);
void Crs_RenameCourseFull (void);
void Crs_ChangeCrsStatus (void);
void Crs_ContEditAfterChgCrs (void);

void Crs_PutFormToSelectMyCourses (void);
void Crs_PutParamCrsCod (long CrsCod);

void Crs_ReqSelectOneOfMyCourses (void);

void Crs_GetAndWriteCrssOfAUsr (long UsrCod,Rol_Role_t Role);

unsigned Crs_ListCrssFound (const char *Query);

void Crs_UpdateCrsLast (void);
void Crs_AskRemoveOldCrss (void);
void Crs_RemoveOldCrss (void);

#endif
