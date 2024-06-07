// swad_department.h: departments

#ifndef _SWAD_DPT
#define _SWAD_DPT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

struct Dpt_Department
  {
   long DptCod;
   long InsCod;
   char ShrtName[Nam_MAX_BYTES_SHRT_NAME + 1];
   char FullName[Nam_MAX_BYTES_FULL_NAME + 1];
   char WWW[WWW_MAX_BYTES_WWW + 1];
   unsigned NumTchs;	// Non-editing teachers and teachers
  };

#define Dpt_NUM_ORDERS 2
typedef enum
  {
   Dpt_ORDER_BY_DEPARTMENT = 0,
   Dpt_ORDER_BY_NUM_TCHS   = 1,
  } Dpt_Order_t;
#define Dpt_ORDER_DEFAULT Dpt_ORDER_BY_DEPARTMENT

struct Dpt_Departments
  {
   unsigned Num;		// Number of departments
   struct Dpt_Department *Lst;	// List of departments
   Dpt_Order_t SelectedOrder;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Dpt_SeeAllDepts (void);
void Dpt_EditDepartments (void);
void Dpt_FreeListDepartments (struct Dpt_Departments *Departments);
void Dpt_GetDepartmentDataByCod (struct Dpt_Department *Dpt);
void Dpt_RemoveDepartment (void);
void Dpt_ChangeDepartIns (void);
void Dpt_RenameDepartShrt (void);
void Dpt_RenameDepartFull (void);
void Dpt_ChangeDptWWW (void);
void Dpt_ContEditAfterChgDpt (void);

void Dpt_ReceiveNewDpt (void);
unsigned Dpt_GetTotalNumberOfDepartments (void);
void Dpt_FlushCacheNumDptsInIns (void);
unsigned Dpt_GetNumDptsInIns (long InsCod);

void Dpt_WriteSelectorDepartment (long InsCod,long DptCod,
                                  const char *ParName,
		                  const char *SelectClass,
                                  long FirstOption,
                                  const char *TextWhenNoDptSelected,
                                  HTM_Attributes_t Attributes);

#endif
