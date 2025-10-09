// swad_department_database.c: departments operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_database.h"
#include "swad_department.h"
#include "swad_department_database.h"
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

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
                   Dpt->HieCod,
                   Dpt->ShrtName,
                   Dpt->FullName,
                   Dpt->WWW);
  }

/*****************************************************************************/
/************** Get list of departments in a given institution ***************/
/*****************************************************************************/

unsigned Dpt_DB_GetListDepartmentsInIns (MYSQL_RES **mysql_res,
                                         long HieCod,Dpt_Order_t SelectedOrder)
  {
   static const char *OrderBySubQuery[Dpt_NUM_ORDERS] =
     {
      [Dpt_ORDER_BY_DEPARTMENT] = "FullName",
      [Dpt_ORDER_BY_NUM_TCHS  ] = "NumTchs DESC,"
	                          "FullName",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get departments",
		   "(SELECT dpt_departments.DptCod,"			// row[0]
			   "dpt_departments.InsCod,"			// row[1]
			   "dpt_departments.ShortName,"			// row[2]
			   "dpt_departments.FullName,"			// row[3]
			   "dpt_departments.WWW,"			// row[4]
			   "COUNT(DISTINCT usr_data.UsrCod) AS NumTchs"	// row[5]
		     " FROM dpt_departments,"
			   "usr_data,"
			   "crs_users"
		    " WHERE dpt_departments.InsCod=%ld"
		      " AND dpt_departments.DptCod=usr_data.DptCod"
		      " AND usr_data.UsrCod=crs_users.UsrCod"
		      " AND crs_users.Role IN (%u,%u)"
		 " GROUP BY dpt_departments.DptCod)"
		   " UNION "
		   "(SELECT DptCod,"					// row[0]
			   "InsCod,"					// row[1]
			   "ShortName,"					// row[2]
			   "FullName,"					// row[3]
			   "WWW,"					// row[4]
			   "0 AS NumTchs"				// row[5]
		     " FROM dpt_departments"
		    " WHERE InsCod=%ld"
		      " AND DptCod NOT IN"
			  " (SELECT DISTINCT "
			           "usr_data.DptCod"
			     " FROM usr_data,crs_users"
		  	    " WHERE crs_users.Role IN (%u,%u)"
			      " AND crs_users.UsrCod=usr_data.UsrCod))"
	         " ORDER BY %s",
		   HieCod,(unsigned) Rol_NET,(unsigned) Rol_TCH,
		   HieCod,(unsigned) Rol_NET,(unsigned) Rol_TCH,
		   OrderBySubQuery[SelectedOrder]);
  }

/*****************************************************************************/
/****************** Get data of department using its code ********************/
/*****************************************************************************/

Exi_Exist_t Dpt_DB_GetDepartmentDataByCod (MYSQL_RES **mysql_res,long DptCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get data of a department",
			 "(SELECT dpt_departments.DptCod,"			// row[0]
				 "dpt_departments.InsCod,"			// row[1]
				 "dpt_departments.ShortName,"			// row[2]
				 "dpt_departments.FullName,"			// row[3]
				 "dpt_departments.WWW,"			// row[4]
				 "COUNT(DISTINCT usr_data.UsrCod) AS NumTchs"	// row[5]
			   " FROM dpt_departments,"
				 "usr_data,"
				 "crs_users"
			  " WHERE dpt_departments.DptCod=%ld"
			    " AND dpt_departments.DptCod=usr_data.DptCod"
			    " AND usr_data.UsrCod=crs_users.UsrCod"
			    " AND crs_users.Role=%u"
		       " GROUP BY dpt_departments.DptCod)"
			 " UNION "
			 "(SELECT DptCod,"					// row[0]
				 "InsCod,"					// row[1]
				 "ShortName,"					// row[2]
				 "FullName,"					// row[3]
				 "WWW,"					// row[4]
				 "0"						// row[5]
			   " FROM dpt_departments"
			  " WHERE DptCod=%ld"
			    " AND DptCod NOT IN"
				" (SELECT DISTINCT "
					 "usr_data.DptCod"
				   " FROM usr_data,"
					 "crs_users"
				  " WHERE crs_users.Role=%u"
				    " AND crs_users.UsrCod=usr_data.UsrCod))",
			 DptCod,(unsigned) Rol_TCH,
			 DptCod,(unsigned) Rol_TCH);
  }

/*****************************************************************************/
/******************* Check if the name of department exists ******************/
/*****************************************************************************/

Exi_Exist_t Dpt_DB_CheckIfDepartmentNameExists (const char *FldName,const char *Name,long Cod,
						__attribute__((unused)) long PrtCod,
						__attribute__((unused)) unsigned Year)
  {
   return
   DB_QueryEXISTS ("can not check if the department name already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM dpt_departments"
		    " WHERE %s='%s'"
		      " AND DptCod<>%ld)",
		   FldName,Name,
		   Cod);
  }

/*****************************************************************************/
/************************** Get number of departments ************************/
/*****************************************************************************/

unsigned Dpt_DB_GetTotalNumberOfDepartments (void)
  {
   return (unsigned) DB_GetNumRowsTable ("dpt_departments");
  }

/*****************************************************************************/
/********+*** Get number of departments in a given institution ***************/
/*****************************************************************************/

unsigned Dpt_DB_GetNumDepartmentsInIns (long HieCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of departments in an institution",
		  "SELECT COUNT(*)"
		   " FROM dpt_departments"
		  " WHERE InsCod=%ld",
		  HieCod);
  }

/*****************************************************************************/
/* Get number of teachers in courses of the current instit. in a department **/
/*****************************************************************************/

unsigned Dpt_DB_GetNumTchsCurrentInsInDepartment (long DptCod)
  {
   /***** Get the number of teachers
          from the current institution in a department *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of teachers in a department",
		  "SELECT COUNT(DISTINCT usr_data.UsrCod)"
		   " FROM usr_data,"
		         "crs_users,"
		         "crs_courses,"
		         "deg_degrees,"
		         "ctr_centers"
		  " WHERE usr_data.InsCod=%ld"				// User in the current institution...
		    " AND usr_data.DptCod=%ld"				// ...and the specified department...
		    " AND usr_data.UsrCod=crs_users.UsrCod"		// ...who is...
		    " AND crs_users.Role IN (%u,%u)"			// ...a teacher...
		    " AND crs_users.CrsCod=crs_courses.CrsCod"		// ...in a course...
		    " AND crs_courses.DegCod=deg_degrees.DegCod"	// ...of a degree...
		    " AND deg_degrees.CtrCod=ctr_centers.InsCod"	// ...of a center...
		    " AND ctr_centers.InsCod=%ld",			// ...of the current institution
		  Gbl.Hierarchy.Node[Hie_INS].HieCod,DptCod,
		  (unsigned) Rol_NET,(unsigned) Rol_TCH,
		  Gbl.Hierarchy.Node[Hie_INS].HieCod);
  }

/*****************************************************************************/
/****************** Update institution in table of departments ***************/
/*****************************************************************************/

void Dpt_DB_UpdateDptIns (long DptCod,long NewInsCod)
  {
   DB_QueryUPDATE ("can not update the institution of a department",
		   "UPDATE dpt_departments"
		     " SET InsCod=%ld"
		   " WHERE DptCod=%ld",
                   NewInsCod,
                   DptCod);
  }

/*****************************************************************************/
/************* Update department name in table of departments ****************/
/*****************************************************************************/

void Dpt_DB_UpdateDptName (long DptCod,
			   const char *FldName,const char *NewDptName)
  {
   DB_QueryUPDATE ("can not update the name of a department",
		   "UPDATE dpt_departments"
		     " SET %s='%s'"
		   " WHERE DptCod=%ld",
	           FldName,NewDptName,
	           DptCod);
  }

/*****************************************************************************/
/**************** Update the table changing old WWW by new WWW ***************/
/*****************************************************************************/

void Dpt_DB_UpdateDptWWW (long DptCod,char NewWWW[WWW_MAX_BYTES_WWW + 1])
  {
   DB_QueryUPDATE ("can not update the web of a department",
		   "UPDATE dpt_departments"
		     " SET WWW='%s'"
		   " WHERE DptCod=%ld",
		   NewWWW,
		   DptCod);
  }

/*****************************************************************************/
/***************************** Remove a department ***************************/
/*****************************************************************************/

void Dpt_DB_RemoveDepartment (long DptCod)
  {
   DB_QueryDELETE ("can not remove a department",
		   "DELETE FROM dpt_departments"
		   " WHERE DptCod=%ld",
		   DptCod);
  }
