// swad_department_database.c: departments operations with database

/*
    SWAD (Shared Workspace At a Distance),
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
/********************************* Headers ***********************************/
/*****************************************************************************/

// #include <stdbool.h>		// For boolean type
// #include <stddef.h>		// For NULL
// #include <stdlib.h>		// For calloc
// #include <string.h>		// For string functions

// #include "swad_box.h"
// #include "swad_constant.h"
#include "swad_database.h"
#include "swad_department.h"
#include "swad_department_database.h"
// #include "swad_error.h"
// #include "swad_form.h"
#include "swad_global.h"
// #include "swad_HTML.h"
// #include "swad_institution.h"
// #include "swad_language.h"
// #include "swad_parameter.h"
// #include "swad_string.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************** Create a new department **************************/
/*****************************************************************************/

void Dpt_DB_CreateDepartment (const struct Dpt_Department *Dpt)
  {
   /***** Create a new department *****/
   DB_QueryINSERT ("can not create a new department",
		   "INSERT INTO dpt_departments"
		   " (InsCod,ShortName,FullName,WWW)"
		   " VALUES"
		   " (%ld,'%s','%s','%s')",
                   Dpt->InsCod,
                   Dpt->ShrtName,
                   Dpt->FullName,
                   Dpt->WWW);
  }

/*****************************************************************************/
/******************* Check if the name of department exists ******************/
/*****************************************************************************/

bool Dpt_DB_CheckIfDepartmentNameExists (const char *FieldName,const char *Name,long DptCod)
  {
   /***** Get number of departments with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the department name already existed",
			  "SELECT COUNT(*)"
			   " FROM dpt_departments"
			  " WHERE %s='%s'"
			    " AND DptCod<>%ld",
			  FieldName,Name,
			  DptCod) != 0);
  }

/*****************************************************************************/
/************** Get number of departments in an institution ******************/
/*****************************************************************************/

unsigned Dpt_DB_GetNumDepartmentsInInstitution (long InsCod)
  {
   /***** Get number of departments in an institution from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of departments in an institution",
		  "SELECT COUNT(*)"
		   " FROM dpt_departments"
		  " WHERE InsCod=%ld",
		  InsCod);
  }

/*****************************************************************************/
/************* Update department name in table of departments ****************/
/*****************************************************************************/

void Dpt_DB_UpdateDegName (long DptCod,const char *FieldName,const char *NewDptName)
  {
   /***** Update department changing old name by new name *****/
   DB_QueryUPDATE ("can not update the name of a department",
		   "UPDATE dpt_departments"
		     " SET %s='%s'"
		   " WHERE DptCod=%ld",
	           FieldName,NewDptName,
	           DptCod);
  }
