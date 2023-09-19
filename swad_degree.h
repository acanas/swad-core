// swad_degree.h: degrees

#ifndef _SWAD_DEG
#define _SWAD_DEG
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Ca�as Vargas

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

#include "swad_action.h"
#include "swad_constant.h"
#include "swad_hierarchy_level.h"
#include "swad_role_type.h"
#include "swad_string.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Deg_MAX_YEARS_PER_DEGREE	12 // Max number of academic years per degree

#define Deg_MAX_DEGREES_PER_USR		20 // Used in list of my degrees

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Deg_SeeDegWithPendingCrss (void);

void Deg_DrawDegreeLogoAndNameWithLink (struct Hie_Node *Deg,Act_Action_t Action,
                                        const char *ClassLogo);

void Deg_WriteSelectorOfDegree (void);

void Deg_ShowDegsOfCurrentCtr (void);

unsigned Deg_ConvStrToYear (const char *StrYear);

void Deg_EditDegrees (void);

void Deg_PutIconToViewDegrees (void);

void Deg_GetListAllDegsWithStds (struct Hie_List *Degs);
void Deg_GetListDegsInCurrentCtr (void);
void Deg_FreeListAllDegsWithStds (struct Hie_List *Degs);

void Deg_ReceiveFormReqDeg (void);
void Deg_ReceiveFormNewDeg (void);
void Deg_RemoveDegree (void);

bool Deg_GetDegreeDataByCod (struct Hie_Node *Deg);
void Deg_RemoveDegreeCompletely (long DegCod);
void Deg_RenameDegreeShort (void);
void Deg_RenameDegreeFull (void);
void Deg_RenameDegree (struct Hie_Node *Deg,Cns_ShrtOrFullName_t ShrtOrFullName);
void Deg_ChangeDegreeType (void);
void Deg_ChangeDegWWW (void);
void Deg_ChangeDegStatus (void);
void Deg_ContEditAfterChgDeg (void);

unsigned Deg_GetCachedNumDegsInSys (void);

void Deg_FlushCacheNumDegsInCty (void);
unsigned Deg_GetNumDegsInCty (long CtyCod);
unsigned Deg_GetCachedNumDegsInCty (long CtyCod);

void Deg_FlushCacheNumDegsInIns (void);
unsigned Deg_GetNumDegsInIns (long InsCod);
unsigned Deg_GetCachedNumDegsInIns (long InsCod);

void Deg_FlushCacheNumDegsInCtr (void);
unsigned Deg_GetNumDegsInCtr (long CtrCod);
unsigned Deg_GetCachedNumDegsInCtr (long CtrCod);

unsigned Deg_GetCachedNumDegsWithCrss (void);
unsigned Deg_GetCachedNumDegsWithUsrs (Rol_Role_t Role);

void Deg_ListDegsFound (MYSQL_RES **mysql_res,unsigned NumCrss);

void Deg_GetMyDegrees (void);
void Deg_FreeMyDegrees (void);
bool Deg_CheckIfIBelongToDeg (long DegCod);
void Deg_GetUsrMainDeg (long UsrCod,
		        char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1],
		        Rol_Role_t *MaxRole);
void Deg_FlushCacheUsrBelongsToDeg (void);
bool Deg_CheckIfUsrBelongsToDeg (long UsrCod,long DegCod);

#endif
