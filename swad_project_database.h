// swad_project_database.h: projects (final degree projects, thesis), operations with database

#ifndef _SWAD_PRJ_DB
#define _SWAD_PRJ_DB
/*
    SWAD (Shared Workspace At a Distance),
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

#include "swad_hierarchy_level.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Prj_DB_UpdateCrsPrjsConfig (bool Editable);

void Prj_DB_LockProjectEdition (long PrjCod);
void Prj_DB_UnlockProjectEdition (long PrjCod);
long Prj_DB_CreateProject (const struct Prj_Project *Prj);
void Prj_DB_UpdateProject (const struct Prj_Project *Prj);
void Prj_DB_AddUsrToPrj (long PrjCod,Prj_RoleInProject_t RoleInProject,long UsrCod);
void Prj_DB_HideOrUnhideProject (long PrjCod,bool Hide);
void Prj_DB_UpdateReview (const struct Prj_Project *Prj);

unsigned Prj_DB_GetListProjects (MYSQL_RES **mysql_res,
                                 const struct Prj_Projects *Projects,
                                 const char *UsrsSubQuery);	// NULL if no users
unsigned Prj_DB_GetCrsPrjsConfig (MYSQL_RES **mysql_res);
unsigned Prj_DB_GetProjectDataByCod (MYSQL_RES **mysql_res,long PrjCod);
void Prj_DB_GetProjectTitle (long PrjCod,char Title[Prj_MAX_BYTES_TITLE + 1]);
unsigned Prj_DB_GetPrjDataToCheckFaults (MYSQL_RES **mysql_res,long PrjCod);
unsigned Prj_DB_GetNumUsrsInPrj (long PrjCod,Prj_RoleInProject_t RoleInProject);
unsigned Prj_DB_GetUsrsInPrj (MYSQL_RES **mysql_res,
                              long PrjCod,Prj_RoleInProject_t RoleInProject);
unsigned Prj_DB_GetMyRolesInPrj (MYSQL_RES **mysql_res,long PrjCod);
long Prj_DB_GetCrsOfPrj (long PrjCod);
unsigned Prj_DB_GetNumCoursesWithProjects (HieLvl_Level_t Scope);
unsigned Prj_DB_GetNumProjects (HieLvl_Level_t Scope);

void Prj_DB_RemoveUsrFromPrj (long PrjCod,Prj_RoleInProject_t RoleInPrj,long UsrCod);
void Prj_DB_RemoveUsrFromProjects (long UsrCod);
void Prj_DB_RemoveUsrsFromPrj (long PrjCod);
void Prj_DB_RemoveUsrsFromCrsPrjs (long CrsCod);
void Prj_DB_RemoveConfigOfCrsPrjs (long CrsCod);
void Prj_DB_RemovePrj (long PrjCod);
void Prj_DB_RemoveCrsPrjs (long CrsCod);

#endif
