// swad_enrolment_database.h: enrolment (registration) or removing of users, operations with database

#ifndef _SWAD_ENR_DB
#define _SWAD_ENR_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************** Headers **********************************/
/*****************************************************************************/

// #include "swad_course.h"
#include "swad_hierarchy.h"
// #include "swad_notification.h"
// #include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Enr_DB_UpdateInstitutionCenterDepartment (void);

void Enr_DB_AcceptUsrInCrs (long UsrCod);

void Enr_DB_RemCrsRequests (long CrsCod);
void Enr_DB_RemUsrRequests (long UsrCod);
void Enr_DB_RemAllUsrsFromCrsSettings (long CrsCod);
void Enr_DB_RemAllUsrsFromCrs (long CrsCod);
void Enr_DB_RemUsrFromAllCrss (long UsrCod);
void Enr_DB_RemUsrAsAdmin (long UsrCod);
void Enr_DB_RemAdmins (HieLvl_Level_t Scope,long Cod);

#endif
