// swad_classroom.c: classrooms, laboratories or other classrooms where classes are taught

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <malloc.h>		// For calloc, free
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_classroom.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"

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

static struct Classroom *Cla_EditingCla = NULL;	// Static variable to keep the classroom being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Cla_GetParamClaOrder (void);
static bool Cla_CheckIfICanCreateClassrooms (void);
static void Cla_PutIconsListingClassrooms (void);
static void Cla_PutIconToEditClassrooms (void);
static void Cla_PutIconsEditingClassrooms (void);

static void Cla_EditClassroomsInternal (void);

static void Cla_ListClassroomsForEdition (void);
static void Cla_PutParamClaCod (long ClaCod);

static void Cla_RenameClassroom (Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Cla_CheckIfClassroomNameExists (const char *FieldName,const char *Name,long ClaCod);
static void Cla_UpdateClaNameDB (long ClaCod,const char *FieldName,const char *NewClaName);

static void Cla_WriteCapacity (char Str[Cns_MAX_DECIMAL_DIGITS_UINT + 1],unsigned Capacity);

static void Cla_PutFormToCreateClassroom (void);
static void Cla_PutHeadClassrooms (void);
static void Cla_CreateClassroom (struct Classroom *Cla);

static void Cla_EditingClassroomConstructor (void);
static void Cla_EditingClassroomDestructor (void);

/*****************************************************************************/
/************************* List all the classrooms ***************************/
/*****************************************************************************/

#define A SIZE_MAX

void Cla_SeeClassrooms (void)
  {
   extern const char *Hlp_CENTRE_Classrooms;
   extern const char *Txt_Classrooms;
   extern const char *Txt_CLASSROOMS_HELP_ORDER[Cla_NUM_ORDERS];
   extern const char *Txt_CLASSROOMS_ORDER[Cla_NUM_ORDERS];
   extern const char *Txt_New_classroom;
   Cla_Order_t Order;
   unsigned NumCla;
   unsigned RowEvenOdd;
   char StrCapacity[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Ctr.CtrCod <= 0)		// No centre selected
      return;

   /***** Get parameter with the type of order in the list of classrooms *****/
   Cla_GetParamClaOrder ();

   /***** Get list of classrooms *****/
   Cla_GetListClassrooms (Cla_ALL_DATA);

   /***** Table head *****/
   Box_BoxBegin (NULL,Txt_Classrooms,Cla_PutIconsListingClassrooms,
		 Hlp_CENTRE_Classrooms,Box_NOT_CLOSABLE);
   HTM_TABLE_BeginWideMarginPadding (2);
   HTM_TR_Begin (NULL);
   for (Order  = (Cla_Order_t) 0;
	Order <= (Cla_Order_t) (Cla_NUM_ORDERS - 1);
	Order++)
     {
      HTM_TH_Begin (1,1,"LM");
      Frm_StartForm (ActSeeCla);
      Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
      HTM_BUTTON_SUBMIT_Begin (Txt_CLASSROOMS_HELP_ORDER[Order],"BT_LINK TIT_TBL",NULL);
      if (Order == Gbl.Classrooms.SelectedOrder)
	 HTM_U_Begin ();
      HTM_Txt (Txt_CLASSROOMS_ORDER[Order]);
      if (Order == Gbl.Classrooms.SelectedOrder)
	 HTM_U_End ();
      HTM_BUTTON_End ();
      Frm_EndForm ();
      HTM_TH_End ();
     }
   HTM_TR_End ();

   /***** Write list of classrooms *****/
   for (NumCla = 0, RowEvenOdd = 1;
	NumCla < Gbl.Classrooms.Num;
	NumCla++, RowEvenOdd = 1 - RowEvenOdd)
     {
      HTM_TR_Begin (NULL);

      /* Short name */
      HTM_TD_Begin ("class=\"DAT LM %s\"",Gbl.ColorRows[RowEvenOdd]);
      HTM_Txt (Gbl.Classrooms.Lst[NumCla].ShrtName);
      HTM_TD_End ();

      /* Full name */
      HTM_TD_Begin ("class=\"DAT LM %s\"",Gbl.ColorRows[RowEvenOdd]);
      HTM_Txt (Gbl.Classrooms.Lst[NumCla].FullName);
      HTM_TD_End ();

      /* Capacity */
      HTM_TD_Begin ("class=\"DAT RM %s\"",Gbl.ColorRows[RowEvenOdd]);
      Cla_WriteCapacity (StrCapacity,Gbl.Classrooms.Lst[NumCla].Capacity);
      HTM_Txt (StrCapacity);
      HTM_TD_End ();

      /* Location */
      HTM_TD_Begin ("class=\"DAT LM %s\"",Gbl.ColorRows[RowEvenOdd]);
      HTM_Txt (Gbl.Classrooms.Lst[NumCla].Location);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Button to create classroom *****/
   if (Cla_CheckIfICanCreateClassrooms ())
     {
      Frm_StartForm (ActEdiCla);
      Btn_PutConfirmButton (Txt_New_classroom);
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of classrooms *****/
   Cla_FreeListClassrooms ();
  }

/*****************************************************************************/
/******** Get parameter with the type or order in list of classrooms *********/
/*****************************************************************************/

static void Cla_GetParamClaOrder (void)
  {
   Gbl.Classrooms.SelectedOrder = (Cla_Order_t)
	                          Par_GetParToUnsignedLong ("Order",
                                                            0,
                                                            Cla_NUM_ORDERS - 1,
                                                            (unsigned long) Cla_ORDER_DEFAULT);
  }

/*****************************************************************************/
/******************** Check if I can create classrooms ***********************/
/*****************************************************************************/

static bool Cla_CheckIfICanCreateClassrooms (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM);
  }

/*****************************************************************************/
/**************** Put contextual icons in list of classrooms *****************/
/*****************************************************************************/

static void Cla_PutIconsListingClassrooms (void)
  {
   /***** Put icon to edit classrooms *****/
   if (Cla_CheckIfICanCreateClassrooms ())
      Cla_PutIconToEditClassrooms ();
  }

/*****************************************************************************/
/******************** Put a link (form) to edit classrooms *******************/
/*****************************************************************************/

static void Cla_PutIconToEditClassrooms (void)
  {
   Ico_PutContextualIconToEdit (ActEdiCla,NULL);
  }

/*****************************************************************************/
/************************* Put forms to edit classrooms **********************/
/*****************************************************************************/

void Cla_EditClassrooms (void)
  {
   /***** Classroom constructor *****/
   Cla_EditingClassroomConstructor ();

   /***** Edit classrooms *****/
   Cla_EditClassroomsInternal ();

   /***** Classroom destructor *****/
   Cla_EditingClassroomDestructor ();
  }

static void Cla_EditClassroomsInternal (void)
  {
   extern const char *Hlp_CENTRE_Classrooms_edit;
   extern const char *Txt_Classrooms;

   /***** Get list of classrooms *****/
   Cla_GetListClassrooms (Cla_ALL_DATA);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Classrooms,Cla_PutIconsEditingClassrooms,
                 Hlp_CENTRE_Classrooms_edit,Box_NOT_CLOSABLE);

   /***** Put a form to create a new classroom *****/
   Cla_PutFormToCreateClassroom ();

   /***** Forms to edit current classrooms *****/
   if (Gbl.Classrooms.Num)
      Cla_ListClassroomsForEdition ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of classrooms *****/
   Cla_FreeListClassrooms ();
  }

/*****************************************************************************/
/*************** Put contextual icons in edition of classrooms ***************/
/*****************************************************************************/

static void Cla_PutIconsEditingClassrooms (void)
  {
   /***** Put icon to view classrooms *****/
   Cla_PutIconToViewClassrooms ();
  }

/*****************************************************************************/
/************************ Put icon to view classrooms ************************/
/*****************************************************************************/

void Cla_PutIconToViewClassrooms (void)
  {
   extern const char *Txt_Classrooms;

   Lay_PutContextualLinkOnlyIcon (ActSeeCla,NULL,NULL,
                                  "classroom.svg",
                                  Txt_Classrooms);
  }

/*****************************************************************************/
/************************** List all the classrooms **************************/
/*****************************************************************************/

void Cla_GetListClassrooms (Cla_WhichData_t WhichData)
  {
   static const char *OrderBySubQuery[Cla_NUM_ORDERS] =
     {
      [Cla_ORDER_BY_SHRT_NAME] = "ShortName",
      [Cla_ORDER_BY_FULL_NAME] = "FullName",
      [Cla_ORDER_BY_CAPACITY ] = "Capacity DESC,ShortName",
      [Cla_ORDER_BY_LOCATION ] = "Location,ShortName",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumCla;
   struct Classroom *Cla;

   /***** Get classrooms from database *****/
   switch (WhichData)
     {
      case Cla_ALL_DATA:
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get classrooms",
				   "SELECT ClaCod,"
					  "ShortName,"
					  "FullName,"
					  "Capacity,"
					  "Location"
				   " FROM classrooms"
				   " WHERE CtrCod=%ld"
				   " ORDER BY %s",
				   Gbl.Hierarchy.Ctr.CtrCod,
				   OrderBySubQuery[Gbl.Classrooms.SelectedOrder]);
	 break;
      case Cla_ONLY_SHRT_NAME:
      default:
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get classrooms",
				   "SELECT ClaCod,"
					  "ShortName"
				   " FROM classrooms"
				   " WHERE CtrCod=%ld"
				   " ORDER BY ShortName",
				   Gbl.Hierarchy.Ctr.CtrCod);
	 break;
     }

   /***** Count number of rows in result *****/
   if (NumRows) // Classrooms found...
     {
      Gbl.Classrooms.Num = (unsigned) NumRows;

      /***** Create list with courses in centre *****/
      if ((Gbl.Classrooms.Lst = (struct Classroom *) calloc (NumRows,sizeof (struct Classroom))) == NULL)
          Lay_NotEnoughMemoryExit ();

      /***** Get the classrooms *****/
      for (NumCla = 0;
	   NumCla < Gbl.Classrooms.Num;
	   NumCla++)
        {
         Cla = &(Gbl.Classrooms.Lst[NumCla]);

         /* Get next classroom */
         row = mysql_fetch_row (mysql_res);

         /* Get classroom code (row[0]) */
         if ((Cla->ClaCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of classroom.");

         /* Get the short name of the classroom (row[1]) */
         Str_Copy (Cla->ShrtName,row[1],
                   Cla_MAX_BYTES_SHRT_NAME);

         if (WhichData == Cla_ALL_DATA)
           {
	    /* Get the full name of the classroom (row[2]) */
	    Str_Copy (Cla->FullName,row[2],
		      Cla_MAX_BYTES_FULL_NAME);

	    /* Get seating capacity in this classroom (row[3]) */
	    if (sscanf (row[3],"%u",&Cla->Capacity) != 1)
	       Cla->Capacity = Cla_UNLIMITED_CAPACITY;

	    /* Get the full name of the classroom (row[4]) */
	    Str_Copy (Cla->Location,row[4],
		      Cla_MAX_BYTES_LOCATION);
           }
        }
     }
   else
      Gbl.Classrooms.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************** Get classroom full name **************************/
/*****************************************************************************/

void Cla_GetDataOfClassroomByCod (struct Classroom *Cla)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Clear data *****/
   Cla->ShrtName[0] = '\0';
   Cla->FullName[0] = '\0';
   Cla->Capacity = Cla_UNLIMITED_CAPACITY;
   Cla->Location[0] = '\0';

   /***** Check if classroom code is correct *****/
   if (Cla->ClaCod > 0)
     {
      /***** Get data of a classroom from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a classroom",
			        "SELECT ShortName,"
				       "FullName,"
				       "Capacity,"
				       "Location"
				" FROM classrooms"
				" WHERE ClaCod=%ld",
				Cla->ClaCod);

      /***** Count number of rows in result *****/
      if (NumRows) // Classroom found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get the short name of the classroom (row[0]) */
         Str_Copy (Cla->ShrtName,row[0],
                   Cla_MAX_BYTES_SHRT_NAME);

         /* Get the full name of the classroom (row[1]) */
         Str_Copy (Cla->FullName,row[1],
                   Cla_MAX_BYTES_FULL_NAME);

         /* Get seating capacity in this classroom (row[2]) */
         if (sscanf (row[2],"%u",&Cla->Capacity) != 1)
            Cla->Capacity = Cla_UNLIMITED_CAPACITY;

         /* Get the location of the classroom (row[3]) */
         Str_Copy (Cla->Location,row[3],
                   Cla_MAX_BYTES_LOCATION);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/************************** Free list of classrooms **************************/
/*****************************************************************************/

void Cla_FreeListClassrooms (void)
  {
   if (Gbl.Classrooms.Lst)
     {
      /***** Free memory used by the list of classrooms in institution *****/
      free (Gbl.Classrooms.Lst);
      Gbl.Classrooms.Lst = NULL;
      Gbl.Classrooms.Num = 0;
     }
  }

/*****************************************************************************/
/************************* List all the classrooms ***************************/
/*****************************************************************************/

static void Cla_ListClassroomsForEdition (void)
  {
   unsigned NumCla;
   struct Classroom *Cla;
   char StrCapacity[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Write heading *****/
   HTM_TABLE_BeginWidePadding (2);
   Cla_PutHeadClassrooms ();

   /***** Write all the classrooms *****/
   for (NumCla = 0;
	NumCla < Gbl.Classrooms.Num;
	NumCla++)
     {
      Cla = &Gbl.Classrooms.Lst[NumCla];

      HTM_TR_Begin (NULL);

      /* Put icon to remove classroom */
      HTM_TD_Begin ("class=\"BM\"");
      Frm_StartForm (ActRemCla);
      Cla_PutParamClaCod (Cla->ClaCod);
      Ico_PutIconRemove ();
      Frm_EndForm ();
      HTM_TD_End ();

      /* Classroom code */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Long (Cla->ClaCod);
      HTM_TD_End ();

      /* Classroom short name */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartForm (ActRenClaSho);
      Cla_PutParamClaCod (Cla->ClaCod);
      HTM_INPUT_TEXT ("ShortName",Cla_MAX_CHARS_SHRT_NAME,Cla->ShrtName,true,
		      "size=\"10\" class=\"INPUT_SHORT_NAME\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Classroom full name */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartForm (ActRenClaFul);
      Cla_PutParamClaCod (Cla->ClaCod);
      HTM_INPUT_TEXT ("FullName",Cla_MAX_CHARS_FULL_NAME,Cla->FullName,true,
		      "size=\"20\" class=\"INPUT_FULL_NAME\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Seating capacity */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartForm (ActChgClaMaxStd);
      Cla_PutParamClaCod (Cla->ClaCod);
      Cla_WriteCapacity (StrCapacity,Cla->Capacity);
      HTM_INPUT_TEXT ("Capacity",Cns_MAX_DECIMAL_DIGITS_UINT,StrCapacity,true,
		      "size=\"3\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Classroom location */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartForm (ActRenClaLoc);
      Cla_PutParamClaCod (Cla->ClaCod);
      HTM_INPUT_TEXT ("Location",Cla_MAX_CHARS_LOCATION,Cla->Location,true,
		      "size=\"15\" class=\"INPUT_FULL_NAME\"");
      Frm_EndForm ();
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/****************** Write parameter with code of classroom *******************/
/*****************************************************************************/

static void Cla_PutParamClaCod (long ClaCod)
  {
   Par_PutHiddenParamLong (NULL,"ClaCod",ClaCod);
  }

/*****************************************************************************/
/******************* Get parameter with code of classroom ********************/
/*****************************************************************************/

long Cla_GetParamClaCod (void)
  {
   /***** Get code of classroom *****/
   return Par_GetParToLong ("ClaCod");
  }

/*****************************************************************************/
/***************************** Remove a classroom ****************************/
/*****************************************************************************/

void Cla_RemoveClassroom (void)
  {
   extern const char *Txt_Classroom_X_removed;

   /***** Classroom constructor *****/
   Cla_EditingClassroomConstructor ();

   /***** Get classroom code *****/
   if ((Cla_EditingCla->ClaCod = Cla_GetParamClaCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of classroom is missing.");

   /***** Get data of the classroom from database *****/
   Cla_GetDataOfClassroomByCod (Cla_EditingCla);

   /***** Update groups assigned to this classroom *****/
   DB_QueryUPDATE ("can not update classroom in groups",
		   "UPDATE crs_grp SET ClaCod=0"	// 0 means another classroom
		   " WHERE ClaCod=%ld",
		   Cla_EditingCla->ClaCod);

   /***** Remove classroom *****/
   DB_QueryDELETE ("can not remove a classroom",
		   "DELETE FROM classrooms WHERE ClaCod=%ld",
		   Cla_EditingCla->ClaCod);

   /***** Create message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_Classroom_X_removed,
	            Cla_EditingCla->FullName);
  }

/*****************************************************************************/
/******************** Remove all classrooms in a centre **********************/
/*****************************************************************************/

void Cla_RemoveAllClassroomsInCtr (long CtrCod)
  {
   /***** Remove all classrooms in centre *****/
   DB_QueryDELETE ("can not remove classrooms",
		   "DELETE FROM classrooms"
                   " WHERE CtrCod=%ld",
		   CtrCod);
  }

/*****************************************************************************/
/******************* Change the short name of a classroom ********************/
/*****************************************************************************/

void Cla_RenameClassroomShort (void)
  {
   /***** Classroom constructor *****/
   Cla_EditingClassroomConstructor ();

   /***** Rename classroom *****/
   Cla_RenameClassroom (Cns_SHRT_NAME);
  }

/*****************************************************************************/
/******************* Change the full name of a classroom *********************/
/*****************************************************************************/

void Cla_RenameClassroomFull (void)
  {
   /***** Classroom constructor *****/
   Cla_EditingClassroomConstructor ();

   /***** Rename classroom *****/
   Cla_RenameClassroom (Cns_FULL_NAME);
  }

/*****************************************************************************/
/********************** Change the name of a classroom ***********************/
/*****************************************************************************/

static void Cla_RenameClassroom (Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_classroom_X_already_exists;
   extern const char *Txt_The_classroom_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_classroom_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentClaName = NULL;		// Initialized to avoid warning
   char NewClaName[Cla_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Cla_MAX_BYTES_SHRT_NAME;
         CurrentClaName = Cla_EditingCla->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Cla_MAX_BYTES_FULL_NAME;
         CurrentClaName = Cla_EditingCla->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the classroom */
   if ((Cla_EditingCla->ClaCod = Cla_GetParamClaCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of classroom is missing.");

   /* Get the new name for the classroom */
   Par_GetParToText (ParamName,NewClaName,MaxBytes);

   /***** Get from the database the old names of the classroom *****/
   Cla_GetDataOfClassroomByCod (Cla_EditingCla);

   /***** Check if new name is empty *****/
   if (NewClaName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentClaName,NewClaName))	// Different names
        {
         /***** If classroom was in database... *****/
         if (Cla_CheckIfClassroomNameExists (ParamName,NewClaName,Cla_EditingCla->ClaCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_classroom_X_already_exists,
                             NewClaName);
         else
           {
            /* Update the table changing old name by new name */
            Cla_UpdateClaNameDB (Cla_EditingCla->ClaCod,FieldName,NewClaName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_classroom_X_has_been_renamed_as_Y,
                             CurrentClaName,NewClaName);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_of_the_classroom_X_has_not_changed,
                          CurrentClaName);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update classroom name *****/
   Str_Copy (CurrentClaName,NewClaName,
             MaxBytes);
  }

/*****************************************************************************/
/******************** Check if the name of classroom exists ******************/
/*****************************************************************************/

static bool Cla_CheckIfClassroomNameExists (const char *FieldName,const char *Name,long ClaCod)
  {
   /***** Get number of classrooms with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a classroom"
			  " already existed",
			  "SELECT COUNT(*) FROM classrooms"
			  " WHERE CtrCod=%ld"
			  " AND %s='%s' AND ClaCod<>%ld",
			  Gbl.Hierarchy.Ctr.CtrCod,
			  FieldName,Name,ClaCod) != 0);
  }

/*****************************************************************************/
/************** Update classroom name in table of classrooms *****************/
/*****************************************************************************/

static void Cla_UpdateClaNameDB (long ClaCod,const char *FieldName,const char *NewClaName)
  {
   /***** Update classroom changing old name by new name */
   DB_QueryUPDATE ("can not update the name of a classroom",
		   "UPDATE classrooms SET %s='%s' WHERE ClaCod=%ld",
		   FieldName,NewClaName,ClaCod);
  }

/*****************************************************************************/
/****************** Change sitting capacity of a classroom *******************/
/*****************************************************************************/

void Cla_ChangeCapacity (void)
  {
   extern const char *Txt_The_capacity_of_classroom_X_has_not_changed;
   extern const char *Txt_The_classroom_X_does_not_have_a_limited_capacity_now;
   extern const char *Txt_The_capacity_of_classroom_X_is_now_Y;
   unsigned NewCapacity;

   /***** Classroom constructor *****/
   Cla_EditingClassroomConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the classroom */
   if ((Cla_EditingCla->ClaCod = Cla_GetParamClaCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of classroom is missing.");

   /* Get the seating capacity of the classroom */
   NewCapacity = (unsigned)
	          Par_GetParToUnsignedLong ("Capacity",
                                            0,
                                            Cla_MAX_CAPACITY,
                                            Cla_UNLIMITED_CAPACITY);

   /***** Get data of the classroom from database *****/
   Cla_GetDataOfClassroomByCod (Cla_EditingCla);

   /***** Check if the old capacity equals the new one
          (this happens when return is pressed without changes) *****/
   if (Cla_EditingCla->Capacity == NewCapacity)
      /***** Message to show no changes made *****/
      Ale_CreateAlert (Ale_INFO,NULL,
	               Txt_The_capacity_of_classroom_X_has_not_changed,
		       Cla_EditingCla->FullName);
   else
     {
      /***** Update the table of groups changing the old capacity to the new *****/
      DB_QueryUPDATE ("can not update the capacity of a classroom",
		      "UPDATE classrooms SET Capacity=%u WHERE ClaCod=%ld",
                      NewCapacity,Cla_EditingCla->ClaCod);
      Cla_EditingCla->Capacity = NewCapacity;

      /***** Message to show the change made *****/
      if (NewCapacity > Grp_MAX_STUDENTS_IN_A_GROUP)
         Ale_CreateAlert (Ale_SUCCESS,NULL,
                          Txt_The_classroom_X_does_not_have_a_limited_capacity_now,
                          Cla_EditingCla->FullName);
      else
         Ale_CreateAlert (Ale_SUCCESS,NULL,
                          Txt_The_capacity_of_classroom_X_is_now_Y,
                          Cla_EditingCla->FullName,NewCapacity);
     }
  }

/*****************************************************************************/
/****************** Write seating capacity of a classroom ********************/
/*****************************************************************************/

static void Cla_WriteCapacity (char Str[Cns_MAX_DECIMAL_DIGITS_UINT + 1],unsigned Capacity)
  {
   if (Capacity <= Cla_MAX_CAPACITY)
      snprintf (Str,Cns_MAX_DECIMAL_DIGITS_UINT + 1,
		"%u",
		Capacity);
   else
      Str[0] = '\0';
  }

/*****************************************************************************/
/******************** Change the location of a classroom *********************/
/*****************************************************************************/

void Cla_ChangeClassroomLocation (void)
  {
   extern const char *Txt_The_location_of_the_classroom_X_has_changed_to_Y;
   extern const char *Txt_The_location_of_the_classroom_X_has_not_changed;
   char NewLocation[Cla_MAX_BYTES_FULL_NAME + 1];

   /***** Classroom constructor *****/
   Cla_EditingClassroomConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the classroom */
   if ((Cla_EditingCla->ClaCod = Cla_GetParamClaCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of classroom is missing.");

   /* Get the new location for the classroom */
   Par_GetParToText ("Location",NewLocation,Cla_MAX_BYTES_LOCATION);

   /***** Get from the database the old location of the classroom *****/
   Cla_GetDataOfClassroomByCod (Cla_EditingCla);

   /***** Check if old and new locations are the same
	  (this happens when return is pressed without changes) *****/
   if (strcmp (Cla_EditingCla->Location,NewLocation))	// Different locations
     {
      /* Update the table changing old name by new name */
      Cla_UpdateClaNameDB (Cla_EditingCla->ClaCod,"Location",NewLocation);
      Str_Copy (Cla_EditingCla->Location,NewLocation,
		Cla_MAX_BYTES_LOCATION);

      /* Write message to show the change made */
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_location_of_the_classroom_X_has_changed_to_Y,
		       Cla_EditingCla->FullName,NewLocation);
     }
   else	// The same location
      Ale_CreateAlert (Ale_INFO,NULL,
	               Txt_The_location_of_the_classroom_X_has_not_changed,
		       Cla_EditingCla->FullName);
  }

/*****************************************************************************/
/******* Show alerts after changing a classroom and continue editing *********/
/*****************************************************************************/

void Cla_ContEditAfterChgCla (void)
  {
   /***** Write message to show the change made *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Cla_EditClassroomsInternal ();

   /***** Classroom destructor *****/
   Cla_EditingClassroomDestructor ();
  }

/*****************************************************************************/
/******************* Put a form to create a new classroom ********************/
/*****************************************************************************/

static void Cla_PutFormToCreateClassroom (void)
  {
   extern const char *Txt_New_classroom;
   extern const char *Txt_Create_classroom;
   char StrCapacity[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Begin form *****/
   Frm_StartForm (ActNewCla);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_classroom,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Cla_PutHeadClassrooms ();

   HTM_TR_Begin (NULL);

   /***** Column to remove classroom, disabled here *****/
   HTM_TD_Begin ("class=\"BM\"");
   HTM_TD_End ();

   /***** Classroom code *****/
   HTM_TD_Begin ("class=\"CODE\"");
   HTM_TD_End ();

   /***** Classroom short name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("ShortName",Cla_MAX_CHARS_SHRT_NAME,Cla_EditingCla->ShrtName,false,
		   "size=\"10\" class=\"INPUT_SHORT_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Classroom full name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("FullName",Cla_MAX_CHARS_FULL_NAME,Cla_EditingCla->FullName,false,
		   "size=\"20\" class=\"INPUT_FULL_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Seating capacity *****/
   HTM_TD_Begin ("class=\"LM\"");
   Cla_WriteCapacity (StrCapacity,Cla_EditingCla->Capacity);
   HTM_INPUT_TEXT ("Capacity",Cns_MAX_DECIMAL_DIGITS_UINT,StrCapacity,false,
		   "size=\"3\"");
   HTM_TD_End ();

   /***** Classroom location *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("Location",Cla_MAX_CHARS_LOCATION,Cla_EditingCla->Location,false,
		   "size=\"15\" class=\"INPUT_FULL_NAME\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_classroom);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/****************** Write header with fields of a classroom ******************/
/*****************************************************************************/

static void Cla_PutHeadClassrooms (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Short_name;
   extern const char *Txt_Full_name;
   extern const char *Txt_Capacity_OF_A_CLASSROOM;
   extern const char *Txt_Location;

   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"BM",NULL);
   HTM_TH (1,1,"RM",Txt_Code);
   HTM_TH (1,1,"LM",Txt_Short_name);
   HTM_TH (1,1,"LM",Txt_Full_name);
   HTM_TH (1,1,"LM",Txt_Capacity_OF_A_CLASSROOM);
   HTM_TH (1,1,"LM",Txt_Location);

   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Receive form to create a new classroom ********************/
/*****************************************************************************/

void Cla_RecFormNewClassroom (void)
  {
   extern const char *Txt_The_classroom_X_already_exists;
   extern const char *Txt_Created_new_classroom_X;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_classroom;

   /***** Classroom constructor *****/
   Cla_EditingClassroomConstructor ();

   /***** Get parameters from form *****/
   /* Get classroom short name */
   Par_GetParToText ("ShortName",Cla_EditingCla->ShrtName,Cla_MAX_BYTES_SHRT_NAME);

   /* Get classroom full name */
   Par_GetParToText ("FullName",Cla_EditingCla->FullName,Cla_MAX_BYTES_FULL_NAME);

   /* Get seating capacity */
   Cla_EditingCla->Capacity = (unsigned)
	                      Par_GetParToUnsignedLong ("Capacity",
                                                        0,
                                                        Cla_MAX_CAPACITY,
                                                        Cla_UNLIMITED_CAPACITY);

   /* Get classroom location */
   Par_GetParToText ("Location",Cla_EditingCla->Location,Cla_MAX_BYTES_LOCATION);

   if (Cla_EditingCla->ShrtName[0] &&
       Cla_EditingCla->FullName[0])	// If there's a classroom name
     {
      /***** If name of classroom was in database... *****/
      if (Cla_CheckIfClassroomNameExists ("ShortName",Cla_EditingCla->ShrtName,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_classroom_X_already_exists,
                          Cla_EditingCla->ShrtName);
      else if (Cla_CheckIfClassroomNameExists ("FullName",Cla_EditingCla->FullName,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_classroom_X_already_exists,
                          Cla_EditingCla->FullName);
      else	// Add new classroom to database
        {
         Cla_CreateClassroom (Cla_EditingCla);
	 Ale_CreateAlert (Ale_SUCCESS,NULL,
	                  Txt_Created_new_classroom_X,
			  Cla_EditingCla->FullName);
        }
     }
   else	// If there is not a classroom name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_classroom);
  }

/*****************************************************************************/
/************************** Create a new classroom ***************************/
/*****************************************************************************/

static void Cla_CreateClassroom (struct Classroom *Cla)
  {
   /***** Create a new classroom *****/
   DB_QueryINSERT ("can not create classroom",
		   "INSERT INTO classrooms"
		   " (CtrCod,ShortName,FullName,Capacity,Location)"
		   " VALUES"
		   " (%ld,'%s','%s',%u,'%s')",
                   Gbl.Hierarchy.Ctr.CtrCod,
		   Cla->ShrtName,Cla->FullName,Cla->Capacity,Cla->Location);
  }

/*****************************************************************************/
/************************* Classroom constructor/destructor **********************/
/*****************************************************************************/

static void Cla_EditingClassroomConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Cla_EditingCla != NULL)
      Lay_ShowErrorAndExit ("Error initializing classroom.");

   /***** Allocate memory for classroom *****/
   if ((Cla_EditingCla = (struct Classroom *) malloc (sizeof (struct Classroom))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for classroom.");

   /***** Reset classroom *****/
   Cla_EditingCla->ClaCod      = -1L;
   Cla_EditingCla->InsCod      = -1L;
   Cla_EditingCla->ShrtName[0] = '\0';
   Cla_EditingCla->FullName[0] = '\0';
   Cla_EditingCla->Capacity    = Cla_UNLIMITED_CAPACITY;
   Cla_EditingCla->Location[0] = '\0';
  }

static void Cla_EditingClassroomDestructor (void)
  {
   /***** Free memory used for classroom *****/
   if (Cla_EditingCla != NULL)
     {
      free (Cla_EditingCla);
      Cla_EditingCla = NULL;
     }
  }
