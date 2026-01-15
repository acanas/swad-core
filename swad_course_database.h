// swad_course_database.c: edition of courses operations with database

#ifndef _SWAD_CRS_DB
#define _SWAD_CRS_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
#include "swad_course.h"
#include "swad_role.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Crs_DB_CreateCourse (struct Hie_Node *Crs,Hie_Status_t Status);

unsigned Crs_DB_GetAllCrss (MYSQL_RES **mysql_res);
unsigned Crs_DB_GetCrssInDeg (MYSQL_RES **mysql_res,long HieCod);
unsigned Crs_DB_GetCrssInCurrentDegBasic (MYSQL_RES **mysql_res);
unsigned Crs_DB_GetCrssInCurrentDegFull (MYSQL_RES **mysql_res);
Exi_Exist_t Crs_DB_GetCourseDataByCod (MYSQL_RES **mysql_res,long HieCod);
long Crs_DB_GetDegCodOfCourseByCod (long HieCod);
void Crs_DB_GetShortNamesByCod (long HieCod,
                                char CrsShortName[Nam_MAX_BYTES_SHRT_NAME + 1],
                                char DegShortName[Nam_MAX_BYTES_SHRT_NAME + 1]);
Exi_Exist_t Crs_DB_CheckIfCrsCodExists (long HieCod);
Exi_Exist_t Crs_DB_CheckIfCrsNameExistsInYearOfDeg (const char *FldName,const char *Name,
						    long Cod,long PrtCod,unsigned Year);
unsigned Crs_DB_GetCrssOfAUsr (MYSQL_RES **mysql_res,long UsrCod,Rol_Role_t Role);
unsigned Crs_DB_GetOldCrss (MYSQL_RES **mysql_res,unsigned long SecondsWithoutAccess);

unsigned Crs_DB_SearchCrss (MYSQL_RES **mysql_res,
                            const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1],
                            const char *RangeQuery);

unsigned Crs_DB_GetNumCrssInSys (__attribute__((unused)) long HieCod);
unsigned Crs_DB_GetNumCrssInCty (long HieCod);
unsigned Crs_DB_GetNumCrssInIns (long HieCod);
unsigned Crs_DB_GetNumCrssInCtr (long HieCod);
unsigned Crs_DB_GetNumCrssInDeg (long HieCod);
unsigned Crs_DB_GetNumCrssWithUsrs (Hie_Level_t HieLvl,long HieCod,Rol_Role_t Role);

unsigned Crs_DB_GetCrssFromUsr (MYSQL_RES **mysql_res,long UsrCod,long PrtCod);

void Crs_DB_UpdateInstitutionalCrsCod (long HieCod,const char *NewInstitutionalCrsCod);
void Crs_DB_UpdateCrsYear (long HieCod,unsigned NewYear);
void Crs_DB_UpdateCrsName (long HieCod,
			   const char *FldName,const char *NewCrsName);
void Crs_DB_UpdateDegOfCurrentCrs (long HieCod);
void Crs_DB_UpdateCrsStatus (long HieCod,Hie_Status_t Status);

void Crs_DB_UpdateCrsLastClick (void);

void Crs_DB_RemoveCrsInfo (long HieCod);
void Crs_DB_RemoveCrsTimetable (long HieCod);
void Crs_DB_RemoveCrsLast (long HieCod);
void Crs_DB_RemoveCrs (long HieCod);

#endif
