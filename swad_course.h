// swad_course.h: edition of courses

#ifndef _SWAD_CRS
#define _SWAD_CRS
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_constant.h"
#include "swad_degree.h"
#include "swad_figure_cache.h"
#include "swad_hierarchy_type.h"
#include "swad_name.h"
#include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Crs_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS  6
#define Crs_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS 12
#define Crs_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_CRSS 60

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Crs_ShowIntroduction (void);

unsigned Crs_GetCachedNumCrssWithUsrs (Hie_Level_t HieLvl,Rol_Role_t Role);

void Crs_WriteSelectorOfCourse (void);
void Crs_ShowCrssOfCurrentDeg (void);
void Crs_WriteSelectorMyCoursesInBreadcrumb (void);

void Crs_EditCourses (void);

void Crs_ReceiveReqCrs (void);
void Crs_ReceiveNewCrs (void);

void Crs_RemoveCourse (void);
Err_SuccessOrError_t Crs_GetCourseDataByCod (struct Hie_Node *Crs);
void Crs_RemoveCourseCompletely (long HieCod);
void Crs_ChangeInsCrsCod (void);
void Crs_ChangeCrsYear (void);

void Crs_UpdateInstitutionalCrsCod (struct Hie_Node *Crs,
                                    const char *NewInstitutionalCrsCod);
void Crs_UpdateCrsYear (struct Hie_Node *Crs,unsigned NewYear);

void Crs_RenameCourseShrt (void);
void Crs_RenameCourseFull (void);
void Crs_RenameCourse (struct Hie_Node *Crs,Nam_ShrtOrFullName_t ShrtOrFull);

void Crs_ChangeCrsStatus (void);
void Crs_ContEditAfterChgCrs (void);

void Crs_GetAndWriteCrssOfAUsr (const struct Usr_Data *UsrDat,Rol_Role_t Role);

void Crs_ListCrssFound (MYSQL_RES **mysql_res,unsigned NumCrss);

void Crs_UpdateCrsLast (void);

void Crs_PutLinkToRemoveOldCrss (void);
void Crs_AskRemoveOldCrss (void);
void Crs_RemoveOldCrss (void);

#endif
