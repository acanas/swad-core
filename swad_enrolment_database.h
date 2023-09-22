// swad_enrolment_database.h: enrolment (registration) or removing of users, operations with database

#ifndef _SWAD_ENR_DB
#define _SWAD_ENR_DB
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_enrolment.h"
#include "swad_hierarchy.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

//------------------------------ Users in courses -----------------------------
void Enr_DB_InsertUsrInCurrentCrs (long UsrCod,long CrsCod,Rol_Role_t NewRole,
                                   Enr_KeepOrSetAccepted_t KeepOrSetAccepted);
void Enr_DB_AcceptUsrInCrs (long UsrCod,long CrsCod);

void Enr_DB_CreateTmpTableMyCourses (void);
unsigned Enr_DB_GetMyCourses (MYSQL_RES **mysql_res);
void Enr_DB_DropTmpTableMyCourses (void);
unsigned Enr_DB_GetMyCoursesNames (MYSQL_RES **mysql_res);

bool Enr_DB_CheckIfUsrBelongsToCrs (long UsrCod,long HieCod,
                                    bool CountOnlyAcceptedCourses);
bool Enr_DB_CheckIfUsrSharesAnyOfMyCrs (long UsrCod);
bool Enr_DB_CheckIfUsrSharesAnyOfMyCrsWithDifferentRole (long UsrCod);
long Enr_DB_GetRamdomStdFromCrs (long CrsCod);
unsigned Enr_DB_GetUsrsFromCurrentCrs (MYSQL_RES **mysql_res);
unsigned Enr_DB_GetUsrsFromCurrentCrsExceptMe (MYSQL_RES **mysql_res);
unsigned Enr_DB_GetTchsFromCurrentCrsExceptMe (MYSQL_RES **mysql_res);
unsigned Enr_DB_GetNumCrssOfUsr (long UsrCod);
unsigned Enr_DB_GetNumCrssOfUsrNotAccepted (long UsrCod);
unsigned Enr_DB_GetNumCrssOfUsrWithARole (long UsrCod,Rol_Role_t Role);
unsigned Enr_DB_GetNumCrssOfUsrWithARoleNotAccepted (long UsrCod,Rol_Role_t Role);
unsigned Enr_DB_GetNumUsrsInCrssOfAUsr (long UsrCod,Rol_Role_t UsrRole,
                                        unsigned OthersRoles);
unsigned Enr_DB_GetNumUsrsInCrss (Hie_Level_t Level,long Cod,unsigned Roles,
                                  bool AnyUserInCourses);
unsigned Enr_DB_GetNumUsrsNotBelongingToAnyCrs (void);
double Enr_DB_GetAverageNumUsrsPerCrs (Hie_Level_t Level,long Cod,Rol_Role_t Role);
double Enr_DB_GetAverageNumCrssPerUsr (Hie_Level_t Level,long Cod,Rol_Role_t Role);

void Enr_DB_RemUsrFromCrs (long UsrCod,long CrsCod);
void Enr_DB_RemUsrFromAllCrss (long UsrCod);
void Enr_DB_RemAllUsrsFromCrs (long CrsCod);

//-------------------------- Enrolment requests -------------------------------
long Enr_DB_CreateMyEnrolmentRequestInCurrentCrs (Rol_Role_t NewRole);
void Enr_DB_UpdateMyEnrolmentRequestInCurrentCrs (long ReqCod,Rol_Role_t NewRole);

unsigned Enr_DB_GetEnrolmentRequests (MYSQL_RES **mysql_res,unsigned RolesSelected);
unsigned Enr_DB_GetEnrolmentRequestByCod (MYSQL_RES **mysql_res,long ReqCod);
long Enr_DB_GetUsrEnrolmentRequestInCrs (long UsrCod,long CrsCod);

void Enr_DB_RemRequest (long ReqCod);
void Enr_DB_RemCrsRequests (long CrsCod);
void Enr_DB_RemUsrRequests (long UsrCod);
void Enr_DB_RemoveExpiredEnrolmentRequests (void);

#endif
