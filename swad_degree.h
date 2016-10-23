// swad_degree.h: degrees

#ifndef _SWAD_DEG
#define _SWAD_DEG
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include "swad_action.h"
#include "swad_constant.h"
#include "swad_role.h"
#include "swad_string.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Deg_MAX_YEARS_PER_DEGREE	 12 // Max number of academic years per degree

#define Deg_MAX_DEGREES_PER_USR		 20 // Used in list of my degrees

#define Deg_MAX_LENGTH_DEGREE_SHORT_NAME	 32
#define Deg_MAX_LENGTH_DEGREE_FULL_NAME		127

#define Deg_MAX_LENGTH_LOCATION_SHORT_NAME	 32
#define Deg_MAX_LENGTH_LOCATION_SHORT_NAME_SPEC_CHAR (Deg_MAX_LENGTH_LOCATION_SHORT_NAME*Str_MAX_LENGTH_SPEC_CHAR_HTML)

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   Deg_STATUS_BIT_PENDING = (1 << 0),	// Degree is requested, but not yet activated
   Deg_STATUS_BIT_REMOVED = (1 << 1),	// Degree has been removed
  } Deg_Status_Bits_t;
typedef unsigned Deg_Status_t;

#define Deg_NUM_STATUS_TXT	4
typedef enum
  {
   Deg_STATUS_UNKNOWN = 0,	// Other
   Deg_STATUS_ACTIVE  = 1,	// 00 (Status == 0)
   Deg_STATUS_PENDING = 2,	// 01 (Status == Deg_STATUS_BIT_PENDING)
   Deg_STATUS_REMOVED = 3,	// 1- (Status & Deg_STATUS_BIT_REMOVED)
  } Deg_StatusTxt_t;

struct Degree
  {
   long DegTypCod;					// Degree type code
   long DegCod;						// Degree code
   long CtrCod;						// Centre code
   Deg_Status_t Status;					// Degree status
   long RequesterUsrCod;				// User code of the person who requested the creation of this degree
   char ShortName[Deg_MAX_LENGTH_DEGREE_SHORT_NAME+1];	// Short name of degree
   char FullName[Deg_MAX_LENGTH_DEGREE_FULL_NAME+1];	// Full name of degree
   char WWW[Cns_MAX_LENGTH_WWW+1];
   struct Course *LstCrss;				// List of courses in this degree
  };

struct ListDegrees
  {
   unsigned Num;			// Number of degrees
   struct Degree *Lst;		// List of degrees
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Deg_SeePending (void);
void Deg_SeeDegWithPendingCrss (void);

void Deg_DrawDegreeLogoAndNameWithLink (struct Degree *Deg,Act_Action_t Action,
                                        const char *ClassLink,const char *ClassLogo);

void Deg_ShowConfiguration (void);
void Deg_PrintConfiguration (void);

void Deg_WriteMenuAllCourses (void);
void Deg_WriteHierarchyBreadcrumb (void);
void Deg_WriteBigNameCtyInsCtrDegCrs (void);
void Deg_InitCurrentCourse (void);

void Deg_ShowDegsOfCurrentCtr (void);

unsigned Deg_ConvStrToYear (const char *StrYear);

void Deg_EditDegrees (void);

void Deg_GetListAllDegsWithStds (struct ListDegrees *Degs);
void Deg_GetListDegsOfCurrentCtr (void);
void Deg_FreeListDegs (struct ListDegrees *Degs);

void Deg_RecFormReqDeg (void);
void Deg_RecFormNewDeg (void);
void Deg_RemoveDegree (void);

void Deg_PutParamDegCod (long DegCod);
long Deg_GetAndCheckParamOtherDegCod (void);

bool Deg_GetDataOfDegreeByCod (struct Degree *Deg);
void Deg_GetShortNameOfDegreeByCod (struct Degree *Deg);
long Deg_GetCtrCodOfDegreeByCod (long DegCod);
long Deg_GetInsCodOfDegreeByCod (long DegCod);
void Deg_RemoveDegreeCompletely (long DegCod);
void Deg_RenameDegreeShort (void);
void Deg_RenameDegreeShortInConfig (void);
void Deg_RenameDegreeFull (void);
void Deg_RenameDegreeFullInConfig (void);
void Deg_ChangeDegCtrInConfig (void);
void Deg_ContEditAfterChgDegInConfig (void);
void Deg_ChangeDegWWW (void);
void Deg_ChangeDegWWWInConfig (void);
void Deg_ChangeDegStatus (void);
void Deg_ContEditAfterChgDeg (void);

void Deg_PutButtonToGoToDeg (struct Degree *Deg);

void Deg_RequestLogo (void);
void Deg_ReceiveLogo (void);
void Deg_RemoveLogo (void);

unsigned Deg_GetNumDegsTotal (void);
unsigned Deg_GetNumDegsInCty (long InsCod);
unsigned Deg_GetNumDegsInIns (long InsCod);
unsigned Deg_GetNumDegsInCtr (long CtrCod);
unsigned Deg_GetNumDegsWithCrss (const char *SubQuery);
unsigned Deg_GetNumDegsWithUsrs (Rol_Role_t Role,const char *SubQuery);

void Deg_GetAndWriteInsCtrDegAdminBy (long UsrCod,unsigned ColSpan);

unsigned Deg_ListDegsFound (const char *Query);

#endif
