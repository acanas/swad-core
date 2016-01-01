// swad_department.h: departments

#ifndef _SWAD_DPT
#define _SWAD_DPT
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

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define MAX_LENGTH_DEPARTMENT_SHORT_NAME	 32
#define MAX_LENGTH_DEPARTMENT_FULL_NAME		255

struct Department
  {
   long DptCod;
   long InsCod;
   char ShortName[MAX_LENGTH_DEPARTMENT_SHORT_NAME+1];
   char FullName[MAX_LENGTH_DEPARTMENT_FULL_NAME+1];
   char WWW[Cns_MAX_LENGTH_WWW+1];
   unsigned NumTchs;
  };

typedef enum
  {
   Dpt_ORDER_BY_DEPARTMENT = 0,
   Dpt_ORDER_BY_NUM_TCHS   = 1,
  } tDptsOrderType;

#define Dpt_DEFAULT_ORDER_TYPE Dpt_ORDER_BY_DEPARTMENT

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Dpt_SeeDepts (void);
void Dpt_EditDepartments (void);
void Dpt_GetListDepartments (long InsCod);
void Dpt_FreeListDepartments (void);
unsigned Dpt_GetNumDepartmentsInInstitution (long InsCod);
void Dpt_GetDataOfDepartmentByCod (struct Department *Dpt);
long Dpt_GetParamDptCod (void);
void Dpt_RemoveDepartment (void);
void Dpt_ChangeDepartIns (void);
void Dpt_RenameDepartShort (void);
void Dpt_RenameDepartFull (void);
void Dpt_ChangeDptWWW (void);
void Dpt_RecFormNewDpt (void);
unsigned Dpt_GetTotalNumberOfDepartments (void);
unsigned Dpt_GetNumberOfDepartmentsInInstitution (long InsCod);

void Dpt_WriteSelectorDepartment (long InsCod);

#endif
