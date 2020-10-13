// swad_department.c: departments

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_department.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_institution.h"
#include "swad_language.h"
#include "swad_parameter.h"
#include "swad_string.h"

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

static struct Dpt_Department *Dpt_EditingDpt = NULL;	// Static variable to keep the department being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Dpt_ResetDepartments (struct Dpt_Departments *Departments);

static Dpt_Order_t Dpt_GetParamDptOrder (void);
static void Dpt_PutIconToEditDpts (__attribute__((unused)) void *Args);
static void Dpt_EditDepartmentsInternal (void);

static void Dpt_GetListDepartments (struct Dpt_Departments *Departments,long InsCod);

static void Dpt_ListDepartmentsForEdition (const struct Dpt_Departments *Departments);
static void Dpt_PutParamDptCod (void *DptCod);

static void Dpt_RenameDepartment (Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Dpt_CheckIfDepartmentNameExists (const char *FieldName,const char *Name,long DptCod);
static void Dpt_UpdateDegNameDB (long DptCod,const char *FieldName,const char *NewDptName);

static void Dpt_PutFormToCreateDepartment (void);
static void Dpt_PutHeadDepartments (void);
static void Dpt_CreateDepartment (struct Dpt_Department *Dpt);

static void Dpt_EditingDepartmentConstructor (void);
static void Dpt_EditingDepartmentDestructor (void);

/*****************************************************************************/
/************************* Reset departments context *************************/
/*****************************************************************************/

static void Dpt_ResetDepartments (struct Dpt_Departments *Departments)
  {
   Departments->Num           = 0;
   Departments->Lst           = NULL;
   Departments->SelectedOrder = Dpt_ORDER_DEFAULT;
  }

/*****************************************************************************/
/************************* List all the departments **************************/
/*****************************************************************************/

void Dpt_SeeDepts (void)
  {
   extern const char *Hlp_INSTITUTION_Departments;
   extern const char *Txt_Departments_of_INSTITUTION_X;
   extern const char *Txt_DEPARTMENTS_HELP_ORDER[2];
   extern const char *Txt_DEPARTMENTS_ORDER[2];
   extern const char *Txt_Other_departments;
   extern const char *Txt_Department_unspecified;
   struct Dpt_Departments Departments;
   Dpt_Order_t Order;
   unsigned NumDpt;
   unsigned NumTchsInsInOtherDpts;
   unsigned NumTchsInsWithNoDpt;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Ins.InsCod <= 0)	// No institution selected
      return;

   /***** Reset departments context *****/
   Dpt_ResetDepartments (&Departments);

   /***** Get parameter with the type of order in the list of departments *****/
   Departments.SelectedOrder = Dpt_GetParamDptOrder ();

   /***** Get list of departments *****/
   Dpt_GetListDepartments (&Departments,Gbl.Hierarchy.Ins.InsCod);

   /***** Begin box and table *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      Box_BoxTableBegin (NULL,Str_BuildStringStr (Txt_Departments_of_INSTITUTION_X,
						  Gbl.Hierarchy.Ins.FullName),
			 Dpt_PutIconToEditDpts,NULL,
			 Hlp_INSTITUTION_Departments,Box_NOT_CLOSABLE,2);
   else
      Box_BoxTableBegin (NULL,Str_BuildStringStr (Txt_Departments_of_INSTITUTION_X,
						  Gbl.Hierarchy.Ins.FullName),
			 NULL,NULL,
			 Hlp_INSTITUTION_Departments,Box_NOT_CLOSABLE,2);
   Str_FreeString ();

   /***** Write heading *****/
   HTM_TR_Begin (NULL);
   for (Order  = Dpt_ORDER_BY_DEPARTMENT;
	Order <= Dpt_ORDER_BY_NUM_TCHS;
	Order++)
     {
      HTM_TH_Begin (1,1,Order == Dpt_ORDER_BY_NUM_TCHS ? "RM" :
	                                                 "LM");

      Frm_StartForm (ActSeeDpt);
      Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
      HTM_BUTTON_SUBMIT_Begin (Txt_DEPARTMENTS_HELP_ORDER[Order],"BT_LINK TIT_TBL",NULL);
      if (Order == Departments.SelectedOrder)
	 HTM_U_Begin ();
      HTM_Txt (Txt_DEPARTMENTS_ORDER[Order]);
      if (Order == Departments.SelectedOrder)
	 HTM_U_End ();
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_TH_End ();
     }
   HTM_TR_End ();

   /***** Write all the Dpt_GetListDepartmentsdepartments and their nuber of teachers *****/
   for (NumDpt = 0;
	NumDpt < Departments.Num;
	NumDpt++)
     {
      /* Write data of this department */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"LM\"");
      HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"DAT\"",
	           Departments.Lst[NumDpt].WWW);
      HTM_Txt (Departments.Lst[NumDpt].FullName);
      HTM_A_End ();
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Unsigned (Departments.Lst[NumDpt].NumTchs);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Separation row *****/
   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("colspan=\"3\" class=\"DAT\"");
   HTM_NBSP ();
   HTM_TD_End ();
   HTM_TR_End ();

   /***** Write teachers of this institution with other department *****/
   NumTchsInsInOtherDpts = Usr_GetNumTchsCurrentInsInDepartment (0);

   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_Txt (Txt_Other_departments);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (NumTchsInsInOtherDpts);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Write teachers with no department *****/
   NumTchsInsWithNoDpt = Usr_GetNumTchsCurrentInsInDepartment (-1L);

   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_Txt (Txt_Department_unspecified);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (NumTchsInsWithNoDpt);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();

   /***** Free list of departments *****/
   Dpt_FreeListDepartments (&Departments);
  }

/*****************************************************************************/
/******** Get parameter with the type or order in list of departments ********/
/*****************************************************************************/

static Dpt_Order_t Dpt_GetParamDptOrder (void)
  {
   return (Dpt_Order_t) Par_GetParToUnsignedLong ("Order",
						  0,
						  Dpt_NUM_ORDERS - 1,
						  (unsigned long) Dpt_ORDER_DEFAULT);
  }

/*****************************************************************************/
/************************ Put icon to edit departments ***********************/
/*****************************************************************************/

static void Dpt_PutIconToEditDpts (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToEdit (ActEdiDpt,NULL,
				NULL,NULL);
  }

/*****************************************************************************/
/******* Put forms to edit the departments of the current institution ********/
/*****************************************************************************/

void Dpt_EditDepartments (void)
  {
   /***** Department constructor *****/
   Dpt_EditingDepartmentConstructor ();

   /***** Edit departments *****/
   Dpt_EditDepartmentsInternal ();

   /***** Department destructor *****/
   Dpt_EditingDepartmentDestructor ();
  }

static void Dpt_EditDepartmentsInternal (void)
  {
   extern const char *Hlp_INSTITUTION_Departments_edit;
   extern const char *Txt_Departments_of_INSTITUTION_X;
   struct Dpt_Departments Departments;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Ins.InsCod <= 0)	// An institution must be selected
      return;

   /***** Reset departments context *****/
   Dpt_ResetDepartments (&Departments);

   /***** Get list of institutions *****/
   Ins_GetBasicListOfInstitutions (Gbl.Hierarchy.Cty.CtyCod);

   /***** Get list of departments *****/
   Dpt_GetListDepartments (&Departments,Gbl.Hierarchy.Ins.InsCod);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Str_BuildStringStr (Txt_Departments_of_INSTITUTION_X,
				          Gbl.Hierarchy.Ins.FullName),
                 NULL,NULL,
                 Hlp_INSTITUTION_Departments_edit,Box_NOT_CLOSABLE);
   Str_FreeString ();

   /***** Put a form to create a new department *****/
   Dpt_PutFormToCreateDepartment ();

   /***** Forms to edit current departments *****/
   if (Departments.Num)
      Dpt_ListDepartmentsForEdition (&Departments);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of departments *****/
   Dpt_FreeListDepartments (&Departments);

   /***** Free list of institutions *****/
   Ins_FreeListInstitutions ();
  }

/*****************************************************************************/
/************************** Get list of departments **************************/
/*****************************************************************************/
// If InsCod  > 0 ==> get departments of an institution
// If InsCod <= 0 ==> an empty list is returned

static void Dpt_GetListDepartments (struct Dpt_Departments *Departments,long InsCod)
  {
   static const char *OrderBySubQuery[Dpt_NUM_ORDERS] =
     {
      [Dpt_ORDER_BY_DEPARTMENT] = "FullName",
      [Dpt_ORDER_BY_NUM_TCHS  ] = "NumTchs DESC,FullName",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumDpt;
   struct Dpt_Department *Dpt;

   /***** Free list of departments *****/
   Dpt_FreeListDepartments (Departments);	// List is initialized to empty

   if (InsCod > 0)	// Institution specified
     {
      /***** Get departments from database *****/
      Departments->Num = (unsigned)
	                 DB_QuerySELECT (&mysql_res,"can not get departments",
				         "(SELECT departments.DptCod,"		// row[0]
				                 "departments.InsCod,"		// row[1]
				                 "departments.ShortName,"	// row[2]
				                 "departments.FullName,"	// row[3]
				                 "departments.WWW,"		// row[4]
				                 "COUNT(DISTINCT usr_data.UsrCod) AS NumTchs"	// row[5]
				         " FROM departments,usr_data,crs_usr"
				         " WHERE departments.InsCod=%ld"
				         " AND departments.DptCod=usr_data.DptCod"
				         " AND usr_data.UsrCod=crs_usr.UsrCod"
				         " AND crs_usr.Role IN (%u,%u)"
				         " GROUP BY departments.DptCod)"
				         " UNION "
				         "(SELECT DptCod,"			// row[0]
				                 "InsCod,"			// row[1]
				                 "ShortName,"			// row[2]
				                 "FullName,"			// row[3]
				                 "WWW,"				// row[4]
				                 "0 AS NumTchs"			// row[5]
				         " FROM departments"
				         " WHERE InsCod=%ld AND DptCod NOT IN"
				         " (SELECT DISTINCT usr_data.DptCod"
				         " FROM usr_data,crs_usr"
				         " WHERE crs_usr.Role IN (%u,%u)"
				         " AND crs_usr.UsrCod=usr_data.UsrCod))"
				         " ORDER BY %s",
				         InsCod,(unsigned) Rol_NET,(unsigned) Rol_TCH,
				         InsCod,(unsigned) Rol_NET,(unsigned) Rol_TCH,
				         OrderBySubQuery[Departments->SelectedOrder]);
      if (Departments->Num) // Departments found...
	{
	 /***** Create list with courses in degree *****/
	 if ((Departments->Lst = (struct Dpt_Department *)
		                 calloc ((size_t) Departments->Num,
	                                 sizeof (struct Dpt_Department))) == NULL)
	    Lay_NotEnoughMemoryExit ();

	 /***** Get the departments *****/
	 for (NumDpt = 0;
	      NumDpt < Departments->Num;
	      NumDpt++)
	   {
	    Dpt = &(Departments->Lst[NumDpt]);

	    /* Get next department */
	    row = mysql_fetch_row (mysql_res);

	    /* Get department code (row[0]) */
	    if ((Dpt->DptCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	       Lay_ShowErrorAndExit ("Wrong code of department.");

	    /* Get institution code (row[1]) */
	    if ((Dpt->InsCod = Str_ConvertStrCodToLongCod (row[1])) < 0)
	       Lay_ShowErrorAndExit ("Wrong code of institution.");

	    /* Get the short name of the department (row[2]) */
	    Str_Copy (Dpt->ShrtName,row[2],
		      Hie_MAX_BYTES_SHRT_NAME);

	    /* Get the full name of the department (row[3]) */
	    Str_Copy (Dpt->FullName,row[3],
		      Hie_MAX_BYTES_FULL_NAME);

	    /* Get the URL of the department (row[4]) */
	    Str_Copy (Dpt->WWW,row[4],
		      Cns_MAX_BYTES_WWW);

	    /* Get number of non-editing teachers and teachers in this department (row[5]) */
	    if (sscanf (row[5],"%u",&Dpt->NumTchs) != 1)
	       Dpt->NumTchs = 0;
	   }
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/************************** Get department full name *************************/
/*****************************************************************************/

void Dpt_GetDataOfDepartmentByCod (struct Dpt_Department *Dpt)
  {
   extern const char *Txt_Another_department;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Clear data *****/
   Dpt->InsCod = -1L;
   Dpt->ShrtName[0] = Dpt->FullName[0] = Dpt->WWW[0] = '\0';
   Dpt->NumTchs = 0;

   /***** Check if department code is correct *****/
   if (Dpt->DptCod == 0)
     {
      Str_Copy (Dpt->ShrtName,Txt_Another_department,
                Hie_MAX_BYTES_SHRT_NAME);
      Str_Copy (Dpt->FullName,Txt_Another_department,
                Hie_MAX_BYTES_FULL_NAME);
     }
   else if (Dpt->DptCod > 0)
     {
      /***** Get data of a department from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a department",
				"(SELECT departments.InsCod,departments.ShortName,departments.FullName,departments.WWW,"
				"COUNT(DISTINCT usr_data.UsrCod) AS NumTchs"
				" FROM departments,usr_data,crs_usr"
				" WHERE departments.DptCod=%ld"
				" AND departments.DptCod=usr_data.DptCod"
				" AND usr_data.UsrCod=crs_usr.UsrCod"
				" AND crs_usr.Role=%u"
				" GROUP BY departments.DptCod)"
				" UNION "
				"(SELECT InsCod,ShortName,FullName,WWW,0"
				" FROM departments"
				" WHERE DptCod=%ld AND DptCod NOT IN"
				" (SELECT DISTINCT usr_data.DptCod FROM usr_data,crs_usr"
				" WHERE crs_usr.Role=%u AND crs_usr.UsrCod=usr_data.UsrCod))",
				Dpt->DptCod,(unsigned) Rol_TCH,
				Dpt->DptCod,(unsigned) Rol_TCH);
      if (NumRows) // Department found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get the code of the institution (row[0]) */
         Dpt->InsCod = Str_ConvertStrCodToLongCod (row[0]);

         /* Get the short name of the department (row[1]) */
         Str_Copy (Dpt->ShrtName,row[1],
                   Hie_MAX_BYTES_SHRT_NAME);

         /* Get the full name of the department (row[2]) */
         Str_Copy (Dpt->FullName,row[2],
                   Hie_MAX_BYTES_FULL_NAME);

         /* Get the URL of the department (row[3]) */
         Str_Copy (Dpt->WWW,row[3],
                   Cns_MAX_BYTES_WWW);

         /* Get number of teachers in this department (row[4]) */
         if (sscanf (row[4],"%u",&Dpt->NumTchs) != 1)
            Dpt->NumTchs = 0;
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/************************** Free list of departments *************************/
/*****************************************************************************/

void Dpt_FreeListDepartments (struct Dpt_Departments *Departments)
  {
   if (Departments->Lst)
      /***** Free memory used by the list of departments *****/
      free (Departments->Lst);

   Departments->Lst = NULL;
   Departments->Num = 0;
  }

/*****************************************************************************/
/************** Get number of departments in an institution ******************/
/*****************************************************************************/

unsigned Dpt_GetNumDepartmentsInInstitution (long InsCod)
  {
   /***** Get number of departments in an institution from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of departments"
	                     " in an institution",
			     "SELECT COUNT(*) FROM departments"
			     " WHERE InsCod=%ld",
			     InsCod);
  }

/*****************************************************************************/
/************************** List all the departments *************************/
/*****************************************************************************/

static void Dpt_ListDepartmentsForEdition (const struct Dpt_Departments *Departments)
  {
   extern const char *Txt_Another_institution;
   unsigned NumDpt;
   struct Dpt_Department *Dpt;
   struct Instit Ins;
   unsigned NumIns;

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

   /***** Write heading *****/
   Dpt_PutHeadDepartments ();

   /***** Write all the departments *****/
   for (NumDpt = 0;
	NumDpt < Departments->Num;
	NumDpt++)
     {
      Dpt = &Departments->Lst[NumDpt];

      /* Get data of institution of this department */
      Ins.InsCod = Dpt->InsCod;
      Ins_GetDataOfInstitutionByCod (&Ins);

      HTM_TR_Begin (NULL);

      /* Put icon to remove department */
      HTM_TD_Begin ("class=\"BM\"");
      if (Dpt->NumTchs)	// Department has teachers ==> deletion forbidden
         Ico_PutIconRemovalNotAllowed ();
      else
	 Ico_PutContextualIconToRemove (ActRemDpt,NULL,
					Dpt_PutParamDptCod,&Dpt->DptCod);
      HTM_TD_End ();

      /* Department code */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_TxtF ("%ld&nbsp;",Dpt->DptCod);
      HTM_TD_End ();

      /* Institution */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActChgDptIns);
      Dpt_PutParamDptCod (&Dpt->DptCod);
      HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			"name=\"OthInsCod\" class=\"HIE_SEL_NARROW\"");
      HTM_OPTION (HTM_Type_STRING,"0",Dpt->InsCod == 0,false,
		  "%s",Txt_Another_institution);
      for (NumIns = 0;
	   NumIns < Gbl.Hierarchy.Inss.Num;
	   NumIns++)
	 HTM_OPTION (HTM_Type_LONG,&Gbl.Hierarchy.Inss.Lst[NumIns].InsCod,
		     Gbl.Hierarchy.Inss.Lst[NumIns].InsCod == Dpt->InsCod,false,
		     "%s",Gbl.Hierarchy.Inss.Lst[NumIns].ShrtName);
      HTM_SELECT_End ();
      Frm_EndForm ();
      HTM_TD_End ();

      /* Department short name */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActRenDptSho);
      Dpt_PutParamDptCod (&Dpt->DptCod);
      HTM_INPUT_TEXT ("ShortName",Hie_MAX_CHARS_SHRT_NAME,Dpt->ShrtName,
                      HTM_SUBMIT_ON_CHANGE,
		      "class=\"INPUT_SHORT_NAME\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Department full name */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActRenDptFul);
      Dpt_PutParamDptCod (&Dpt->DptCod);
      HTM_INPUT_TEXT ("FullName",Hie_MAX_CHARS_FULL_NAME,Dpt->FullName,
                      HTM_SUBMIT_ON_CHANGE,
		      "class=\"INPUT_FULL_NAME\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Department WWW */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActChgDptWWW);
      Dpt_PutParamDptCod (&Dpt->DptCod);
      HTM_INPUT_URL ("WWW",Dpt->WWW,HTM_SUBMIT_ON_CHANGE,
		     "class=\"INPUT_WWW_NARROW\" required=\"required\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Number of teachers */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Unsigned (Dpt->NumTchs);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/****************** Write parameter with code of department ******************/
/*****************************************************************************/

static void Dpt_PutParamDptCod (void *DptCod)
  {
   if (DptCod)
      Par_PutHiddenParamLong (NULL,Dpt_PARAM_DPT_COD_NAME,*((long *) DptCod));
  }

/*****************************************************************************/
/******************* Get parameter with code of department *******************/
/*****************************************************************************/

long Dpt_GetAndCheckParamDptCod (long MinCodAllowed)
  {
   long DptCod;

   /***** Get and check parameter with code of department *****/
   if ((DptCod = Par_GetParToLong (Dpt_PARAM_DPT_COD_NAME)) < MinCodAllowed)
      Lay_ShowErrorAndExit ("Code of department is missing or invalid.");

   return DptCod;
  }

/*****************************************************************************/
/***************************** Remove a department ***************************/
/*****************************************************************************/

void Dpt_RemoveDepartment (void)
  {
   extern const char *Txt_To_remove_a_department_you_must_first_remove_all_teachers_in_the_department;
   extern const char *Txt_Department_X_removed;

   /***** Department constructor *****/
   Dpt_EditingDepartmentConstructor ();

   /***** Get department code *****/
   Dpt_EditingDpt->DptCod = Dpt_GetAndCheckParamDptCod (1);

   /***** Get data of the department from database *****/
   Dpt_GetDataOfDepartmentByCod (Dpt_EditingDpt);

   /***** Check if this department has teachers *****/
   if (Dpt_EditingDpt->NumTchs)	// Department has teachers ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_a_department_you_must_first_remove_all_teachers_in_the_department);
   else	// Department has no teachers ==> remove it
     {
      /***** Remove department *****/
      DB_QueryDELETE ("can not remove a department",
		      "DELETE FROM departments WHERE DptCod=%ld",
		      Dpt_EditingDpt->DptCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Department_X_removed,
		       Dpt_EditingDpt->FullName);
     }
  }

/*****************************************************************************/
/****************** Change the institution of a department *******************/
/*****************************************************************************/

void Dpt_ChangeDepartIns (void)
  {
   extern const char *Txt_The_institution_of_the_department_has_changed;
   long NewInsCod;

   /***** Department constructor *****/
   Dpt_EditingDepartmentConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the department */
   Dpt_EditingDpt->DptCod = Dpt_GetAndCheckParamDptCod (1);

   /* Get parameter with institution code */
   NewInsCod = Ins_GetAndCheckParamOtherInsCod (1);

   /***** Get data of the department from database *****/
   Dpt_GetDataOfDepartmentByCod (Dpt_EditingDpt);

   /***** Update institution in table of departments *****/
   DB_QueryUPDATE ("can not update the institution of a department",
		   "UPDATE departments SET InsCod=%ld WHERE DptCod=%ld",
                   NewInsCod,Dpt_EditingDpt->DptCod);

   /***** Write message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_institution_of_the_department_has_changed);
  }

/*****************************************************************************/
/******************* Change the short name of a department *******************/
/*****************************************************************************/

void Dpt_RenameDepartShort (void)
  {
   /***** Department constructor *****/
   Dpt_EditingDepartmentConstructor ();

   /***** Rename department *****/
   Dpt_RenameDepartment (Cns_SHRT_NAME);
  }

/*****************************************************************************/
/******************* Change the full name of a department ********************/
/*****************************************************************************/

void Dpt_RenameDepartFull (void)
  {
   /***** Department constructor *****/
   Dpt_EditingDepartmentConstructor ();

   /***** Rename department *****/
   Dpt_RenameDepartment (Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a degree ************************/
/*****************************************************************************/

static void Dpt_RenameDepartment (Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_department_X_already_exists;
   extern const char *Txt_The_department_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_department_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   size_t MaxBytes = 0;			// Initialized to avoid warning
   char *CurrentDptName = NULL;		// Initialized to avoid warning
   char NewDptName[Hie_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Hie_MAX_BYTES_SHRT_NAME;
         CurrentDptName = Dpt_EditingDpt->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Hie_MAX_BYTES_FULL_NAME;
         CurrentDptName = Dpt_EditingDpt->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the department */
   Dpt_EditingDpt->DptCod = Dpt_GetAndCheckParamDptCod (1);

   /* Get the new name for the department */
   Par_GetParToText (ParamName,NewDptName,MaxBytes);

   /***** Get from the database the old names of the department *****/
   Dpt_GetDataOfDepartmentByCod (Dpt_EditingDpt);

   /***** Check if new name is empty *****/
   if (NewDptName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentDptName,NewDptName))	// Different names
        {
         /***** If degree was in database... *****/
         if (Dpt_CheckIfDepartmentNameExists (ParamName,NewDptName,Dpt_EditingDpt->DptCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_department_X_already_exists,
                             NewDptName);
         else
           {
            /* Update the table changing old name by new name */
            Dpt_UpdateDegNameDB (Dpt_EditingDpt->DptCod,FieldName,NewDptName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_department_X_has_been_renamed_as_Y,
                             CurrentDptName,NewDptName);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_of_the_department_X_has_not_changed,
                          CurrentDptName);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update name *****/
   Str_Copy (CurrentDptName,NewDptName,
             MaxBytes);
  }

/*****************************************************************************/
/******************* Check if the name of department exists ******************/
/*****************************************************************************/

static bool Dpt_CheckIfDepartmentNameExists (const char *FieldName,const char *Name,long DptCod)
  {
   /***** Get number of departments with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name"
	                  " of a department already existed",
			  "SELECT COUNT(*) FROM departments"
			  " WHERE %s='%s' AND DptCod<>%ld",
			  FieldName,Name,DptCod) != 0);
  }

/*****************************************************************************/
/************* Update department name in table of departments ****************/
/*****************************************************************************/

static void Dpt_UpdateDegNameDB (long DptCod,const char *FieldName,const char *NewDptName)
  {
   /***** Update department changing old name by new name *****/
   DB_QueryUPDATE ("can not update the name of a department",
		   "UPDATE departments SET %s='%s' WHERE DptCod=%ld",
	           FieldName,NewDptName,DptCod);
  }

/******************************************************************************/
/*********************** Change the URL of a department *********************/
/*****************************************************************************/

void Dpt_ChangeDptWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Department constructor *****/
   Dpt_EditingDepartmentConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the department */
   Dpt_EditingDpt->DptCod = Dpt_GetAndCheckParamDptCod (1);

   /* Get the new WWW for the department */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get data of the department from database *****/
   Dpt_GetDataOfDepartmentByCod (Dpt_EditingDpt);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /* Update the table changing old WWW by new WWW */
      DB_QueryUPDATE ("can not update the web of a department",
		      "UPDATE departments SET WWW='%s' WHERE DptCod=%ld",
                      NewWWW,Dpt_EditingDpt->DptCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_web_address_is_X,
                       NewWWW);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update web *****/
   Str_Copy (Dpt_EditingDpt->WWW,NewWWW,
             Cns_MAX_BYTES_WWW);
  }

/*****************************************************************************/
/******* Show alerts after changing a department and continue editing ********/
/*****************************************************************************/

void Dpt_ContEditAfterChgDpt (void)
  {
   /***** Write message to show the change made *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Dpt_EditDepartmentsInternal ();

   /***** Department destructor *****/
   Dpt_EditingDepartmentDestructor ();
  }

/*****************************************************************************/
/******************* Put a form to create a new department *******************/
/*****************************************************************************/

static void Dpt_PutFormToCreateDepartment (void)
  {
   extern const char *Txt_New_department;
   extern const char *Txt_Institution;
   extern const char *Txt_Short_name;
   extern const char *Txt_Full_name;
   extern const char *Txt_WWW;
   extern const char *Txt_Another_institution;
   extern const char *Txt_Create_department;
   unsigned NumIns;

   /***** Begin form *****/
   Frm_StartForm (ActNewDpt);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_department,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"LM",Txt_Institution);
   HTM_TH (1,1,"LM",Txt_Short_name);
   HTM_TH (1,1,"LM",Txt_Full_name);
   HTM_TH (1,1,"LM",Txt_WWW);

   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Institution *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "name=\"OthInsCod\" class=\"HIE_SEL_NARROW\"");
   HTM_OPTION (HTM_Type_STRING,"0",Dpt_EditingDpt->InsCod == 0,false,
	       "%s",Txt_Another_institution);
   for (NumIns = 0;
	NumIns < Gbl.Hierarchy.Inss.Num;
	NumIns++)
      HTM_OPTION (HTM_Type_LONG,&Gbl.Hierarchy.Inss.Lst[NumIns].InsCod,
		  Gbl.Hierarchy.Inss.Lst[NumIns].InsCod == Dpt_EditingDpt->InsCod,false,
		  "%s",Gbl.Hierarchy.Inss.Lst[NumIns].ShrtName);
   HTM_SELECT_End ();
   HTM_TD_End ();

   /***** Department short name *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_INPUT_TEXT ("ShortName",Hie_MAX_CHARS_SHRT_NAME,Dpt_EditingDpt->ShrtName,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "class=\"INPUT_SHORT_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Department full name *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_INPUT_TEXT ("FullName",Hie_MAX_CHARS_FULL_NAME,Dpt_EditingDpt->FullName,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "class=\"INPUT_FULL_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Department WWW *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_INPUT_URL ("WWW",Dpt_EditingDpt->WWW,HTM_DONT_SUBMIT_ON_CHANGE,
		  "class=\"INPUT_WWW_NARROW\" required=\"required\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_department);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Write header with fields of a degree *******************/
/*****************************************************************************/

static void Dpt_PutHeadDepartments (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Institution;
   extern const char *Txt_Short_name;
   extern const char *Txt_Full_name;
   extern const char *Txt_WWW;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];

   HTM_TR_Begin (NULL);

   HTM_TH_Empty (1);
   HTM_TH (1,1,"RM",Txt_Code);
   HTM_TH (1,1,"LM",Txt_Institution);
   HTM_TH (1,1,"LM",Txt_Short_name);
   HTM_TH (1,1,"LM",Txt_Full_name);
   HTM_TH (1,1,"LM",Txt_WWW);
   HTM_TH (1,1,"RM",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);

   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Receive form to create a new department *******************/
/*****************************************************************************/

void Dpt_ReceiveFormNewDpt (void)
  {
   extern const char *Txt_The_department_X_already_exists;
   extern const char *Txt_Created_new_department_X;
   extern const char *Txt_You_must_specify_the_web_address_of_the_new_department;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_department;

   /***** Department constructor *****/
   Dpt_EditingDepartmentConstructor ();

   /***** Get parameters from form *****/
   /* Get institution */
   Dpt_EditingDpt->InsCod = Ins_GetAndCheckParamOtherInsCod (1);

   /* Get department short name */
   Par_GetParToText ("ShortName",Dpt_EditingDpt->ShrtName,Hie_MAX_BYTES_SHRT_NAME);

   /* Get department full name */
   Par_GetParToText ("FullName",Dpt_EditingDpt->FullName,Hie_MAX_BYTES_FULL_NAME);

   /* Get department WWW */
   Par_GetParToText ("WWW",Dpt_EditingDpt->WWW,Cns_MAX_BYTES_WWW);

   if (Dpt_EditingDpt->ShrtName[0] &&
       Dpt_EditingDpt->FullName[0])	// If there's a department name
     {
      if (Dpt_EditingDpt->WWW[0])
        {
         /***** If name of department was in database... *****/
         if (Dpt_CheckIfDepartmentNameExists ("ShortName",Dpt_EditingDpt->ShrtName,-1L))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_department_X_already_exists,
                             Dpt_EditingDpt->ShrtName);
         else if (Dpt_CheckIfDepartmentNameExists ("FullName",Dpt_EditingDpt->FullName,-1L))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_department_X_already_exists,
                             Dpt_EditingDpt->FullName);
         else	// Add new department to database
           {
            Dpt_CreateDepartment (Dpt_EditingDpt);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
		             Txt_Created_new_department_X,
			     Dpt_EditingDpt->FullName);
           }
        }
      else	// If there is not a web
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_You_must_specify_the_web_address_of_the_new_department);
     }
   else	// If there is not a department name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_department);
  }

/*****************************************************************************/
/************************** Create a new department **************************/
/*****************************************************************************/

static void Dpt_CreateDepartment (struct Dpt_Department *Dpt)
  {
   /***** Create a new department *****/
   DB_QueryINSERT ("can not create a new department",
		   "INSERT INTO departments"
		   " (InsCod,ShortName,FullName,WWW)"
		   " VALUES"
		   " (%ld,'%s','%s','%s')",
                   Dpt->InsCod,Dpt->ShrtName,Dpt->FullName,Dpt->WWW);
  }

/*****************************************************************************/
/************************** Get number of departments ************************/
/*****************************************************************************/

unsigned Dpt_GetTotalNumberOfDepartments (void)
  {
   /***** Get number of departments from database *****/
   return (unsigned) DB_GetNumRowsTable ("departments");
  }

/*****************************************************************************/
/**************** Get number of departments in a institution *****************/
/*****************************************************************************/

void Dpt_FlushCacheNumDptsInIns (void)
  {
   Gbl.Cache.NumDptsInIns.InsCod  = -1L;
   Gbl.Cache.NumDptsInIns.NumDpts = 0;
  }

unsigned Dpt_GetNumDptsInIns (long InsCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (InsCod <= 0)
      return 0;

   /***** 2. Fast check: If cached... *****/
   if (InsCod == Gbl.Cache.NumDptsInIns.InsCod)
      return Gbl.Cache.NumDptsInIns.NumDpts;

   /***** 3. Slow: number of departments of an institution from database *****/
   Gbl.Cache.NumDptsInIns.InsCod  = InsCod;
   Gbl.Cache.NumDptsInIns.NumDpts =
   (unsigned) DB_QueryCOUNT ("can not get number of departments"
			     " in an institution",
			     "SELECT COUNT(*) FROM departments"
			     " WHERE InsCod=%ld",
			     InsCod);
   return Gbl.Cache.NumDptsInIns.NumDpts;
  }

/*****************************************************************************/
/*********************** Put selector for department *************************/
/*****************************************************************************/

void Dpt_WriteSelectorDepartment (long InsCod,long DptCod,
		                  const char *SelectClass,
                                  long FirstOption,
                                  const char *TextWhenNoDptSelected,
                                  bool SubmitFormOnChange)
  {
   extern const char *Txt_Another_department;
   struct Dpt_Departments Departments;
   unsigned NumDpt;
   bool NoDptSelectable;

   /***** Reset departments context *****/
   Dpt_ResetDepartments (&Departments);

   /***** Get list of departments *****/
   Dpt_GetListDepartments (&Departments,InsCod);

   /***** Selector to select department *****/
   /* Start selector */
   HTM_SELECT_Begin (SubmitFormOnChange,
		     "id=\"%s\" name=\"%s\" class=\"%s\"",
		     Dpt_PARAM_DPT_COD_NAME,Dpt_PARAM_DPT_COD_NAME,SelectClass);

   if (FirstOption <= 0)
     {
      /* Option when no department selected */
      if (FirstOption < 0)
	{
	 NoDptSelectable = false;
	 if (TextWhenNoDptSelected)
	    if (TextWhenNoDptSelected[0])
	       NoDptSelectable = true;

	 HTM_OPTION (HTM_Type_STRING,"-1",DptCod < 0,!NoDptSelectable,
		     "%s",TextWhenNoDptSelected);
	}

      /* Another department selected (different to all departments listed) */
      HTM_OPTION (HTM_Type_STRING,"0",DptCod == 0,false,
		  "%s",Txt_Another_department);
     }

   /* List all departments */
   for (NumDpt = 0;
	NumDpt < Departments.Num;
	NumDpt++)
      HTM_OPTION (HTM_Type_LONG,&Departments.Lst[NumDpt].DptCod,
		  Departments.Lst[NumDpt].DptCod == DptCod,false,
		  "%s",Departments.Lst[NumDpt].FullName);

   /* End selector */
   HTM_SELECT_End ();

   /***** Free list of departments *****/
   Dpt_FreeListDepartments (&Departments);
  }

/*****************************************************************************/
/********************** Department constructor/destructor ********************/
/*****************************************************************************/

static void Dpt_EditingDepartmentConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Dpt_EditingDpt != NULL)
      Lay_ShowErrorAndExit ("Error initializing department.");

   /***** Allocate memory for department *****/
   if ((Dpt_EditingDpt = (struct Dpt_Department *) malloc (sizeof (struct Dpt_Department))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for department.");

   /***** Reset department *****/
   Dpt_EditingDpt->DptCod      = -1L;
   Dpt_EditingDpt->InsCod      = -1L;
   Dpt_EditingDpt->ShrtName[0] = '\0';
   Dpt_EditingDpt->FullName[0] = '\0';
   Dpt_EditingDpt->WWW[0]      = '\0';
   Dpt_EditingDpt->NumTchs     = 0;
  }

static void Dpt_EditingDepartmentDestructor (void)
  {
   /***** Free memory used for department *****/
   if (Dpt_EditingDpt != NULL)
     {
      free (Dpt_EditingDpt);
      Dpt_EditingDpt = NULL;
     }
  }
