// swad_department.h: departments

#ifndef _SWAD_DPT
#define _SWAD_DPT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

#include "swad_hierarchy.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

struct Department
  {
   long DptCod;
   long InsCod;
   char ShrtName[Hie_MAX_BYTES_SHRT_NAME + 1];
   char FullName[Hie_MAX_BYTES_FULL_NAME + 1];
   char WWW[Cns_MAX_BYTES_WWW + 1];
   unsigned NumTchs;	// Non-editing teachers and teachers
  };

#define Dpt_NUM_ORDERS 2
typedef enum
  {
   Dpt_ORDER_BY_DEPARTMENT = 0,
   Dpt_ORDER_BY_NUM_TCHS   = 1,
  } Dpt_Order_t;
#define Dpt_ORDER_DEFAULT Dpt_ORDER_BY_DEPARTMENT

#define Dpt_PARAM_DPT_COD_NAME	"DptCod"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Dpt_SeeDepts (void);
void Dpt_EditDepartments (void);
void Dpt_GetListDepartments (long InsCod);
void Dpt_FreeListDepartments (void);
unsigned Dpt_GetNumDepartmentsInInstitution (long InsCod);
void Dpt_GetDataOfDepartmentByCod (struct Department *Dpt);
long Dpt_GetAndCheckParamDptCod (long MinCodAllowed);
void Dpt_RemoveDepartment (void);
void Dpt_ChangeDepartIns (void);
void Dpt_RenameDepartShort (void);
void Dpt_RenameDepartFull (void);
void Dpt_ChangeDptWWW (void);
void Dpt_RecFormNewDpt (void);
unsigned Dpt_GetTotalNumberOfDepartments (void);
unsigned Dpt_GetNumDptsInIns (long InsCod);

void Dpt_WriteSelectorDepartment (long InsCod,long DptCod,
                                  const char *SelectClass,
                                  long FirstOptionSelectable,
                                  const char *TextWhenNoDptSelected,
                                  bool SubmitFormOnChange);

#endif
