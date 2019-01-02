// swad_classroom.c: classrooms, laboratories or other classrooms where classes are taught

/*
    SWAD (Shared Workspace At a Distance),
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <linux/stddef.h>	// For NULL
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_classroom.h"
#include "swad_table.h"

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

static void Cla_GetParamClaOrder (void);
static bool Cla_CheckIfICanCreateClassrooms (void);
static void Cla_PutIconsListingClassrooms (void);
static void Cla_PutIconToEditClassrooms (void);
static void Cla_PutIconsEditingClassrooms (void);

static void Cla_ListClassroomsForEdition (void);
static void Cla_PutParamClaCod (long ClaCod);

static void Cla_RenameClassroom (Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Cla_CheckIfClassroomNameExists (const char *FieldName,const char *Name,long ClaCod);
static void Cla_UpdateClaNameDB (long ClaCod,const char *FieldName,const char *NewClaName);

static void Cla_WriteCapacity (unsigned Capacity);

static void Cla_PutFormToCreateClassroom (void);
static void Cla_PutHeadClassrooms (void);
static void Cla_CreateClassroom (struct Classroom *Cla);

/*****************************************************************************/
/************************* List all the classrooms ***************************/
/*****************************************************************************/

void Cla_SeeClassrooms (void)
  {
   extern const char *Hlp_CENTRE_Classrooms;
   extern const char *Txt_Classrooms;
   extern const char *Txt_CLASSROOMS_HELP_ORDER[2];
   extern const char *Txt_CLASSROOMS_ORDER[2];
   extern const char *Txt_New_classroom;
   Cla_Order_t Order;
   unsigned NumCla;

   if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
     {
      /***** Get parameter with the type of order in the list of classrooms *****/
      Cla_GetParamClaOrder ();

      /***** Get list of classrooms *****/
      Cla_GetListClassrooms ();

      /***** Table head *****/
      Box_StartBox (NULL,Txt_Classrooms,Cla_PutIconsListingClassrooms,
                    Hlp_CENTRE_Classrooms,Box_NOT_CLOSABLE);
      Tbl_StartTableWideMargin (2);
      fprintf (Gbl.F.Out,"<tr>");
      for (Order = Cla_ORDER_BY_CLASSROOM;
	   Order <= Cla_ORDER_BY_CAPACITY;
	   Order++)
	{
	 fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">");
	 Frm_StartForm (ActSeeCla);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Frm_LinkFormSubmit (Txt_CLASSROOMS_HELP_ORDER[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Classrooms.SelectedOrder)
	    fprintf (Gbl.F.Out,"<u>");
	 fprintf (Gbl.F.Out,"%s",Txt_CLASSROOMS_ORDER[Order]);
	 if (Order == Gbl.Classrooms.SelectedOrder)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Frm_EndForm ();
	 fprintf (Gbl.F.Out,"</th>");
	}
      fprintf (Gbl.F.Out,"</tr>");

      /***** Write all classrooms and their maximum of students *****/
      for (NumCla = 0;
	   NumCla < Gbl.Classrooms.Num;
	   NumCla++)
	{
	 /* Write data of this classroom */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "%s"
			    "</td>"
	                    "<td class=\"DAT RIGHT_MIDDLE\">",
		  Gbl.Classrooms.Lst[NumCla].FullName);
	 Cla_WriteCapacity (Gbl.Classrooms.Lst[NumCla].Capacity);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");
	}

      /***** End table *****/
      Tbl_EndTable ();

      /***** Button to create classroom *****/
      if (Cla_CheckIfICanCreateClassrooms ())
	{
	 Frm_StartForm (ActEdiCla);
	 Btn_PutConfirmButton (Txt_New_classroom);
	 Frm_EndForm ();
	}

      /***** End box *****/
      Box_EndBox ();

      /***** Free list of classrooms *****/
      Cla_FreeListClassrooms ();
     }
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
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM);
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
   extern const char *Hlp_CENTRE_Classrooms_edit;
   extern const char *Txt_Classrooms;

   /***** Get list of classrooms *****/
   Cla_GetListClassrooms ();

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Classrooms,Cla_PutIconsEditingClassrooms,
                 Hlp_CENTRE_Classrooms_edit,Box_NOT_CLOSABLE);

   /***** Put a form to create a new classroom *****/
   Cla_PutFormToCreateClassroom ();

   /***** Forms to edit current classrooms *****/
   if (Gbl.Classrooms.Num)
      Cla_ListClassroomsForEdition ();

   /***** End box *****/
   Box_EndBox ();

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

   Lay_PutContextualLink (ActSeeCla,NULL,NULL,
                          "classroom.svg",
                          Txt_Classrooms,NULL,
                          NULL);
  }

/*****************************************************************************/
/************************** List all the classrooms **************************/
/*****************************************************************************/

void Cla_GetListClassrooms (void)
  {
   char OrderBySubQuery[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumCla;
   struct Classroom *Cla;

   /***** Get classrooms from database *****/
   switch (Gbl.Classrooms.SelectedOrder)
     {
      case Cla_ORDER_BY_CLASSROOM:
         sprintf (OrderBySubQuery,"FullName");
         break;
      case Cla_ORDER_BY_CAPACITY:
         sprintf (OrderBySubQuery,"Capacity DESC,FullName");
         break;
     }
   NumRows = DB_QuerySELECT (&mysql_res,"can not get classrooms",
			     "SELECT ClaCod,"
				    "ShortName,"
				    "FullName,"
				    "Capacity"
			     " FROM classrooms"
			     " WHERE CtrCod=%ld"
			     " ORDER BY %s",
			     Gbl.CurrentCtr.Ctr.CtrCod,
			     OrderBySubQuery);

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
                   Cla_MAX_BYTES_CLASSROOM_SHRT_NAME);

         /* Get the full name of the classroom (row[2]) */
         Str_Copy (Cla->FullName,row[2],
                   Cla_MAX_BYTES_CLASSROOM_FULL_NAME);

         /* Get seating capacity in this classroom (row[3]) */
         if (sscanf (row[3],"%u",&Cla->Capacity) != 1)
            Cla->Capacity = Cla_UNLIMITED_CAPACITY;
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
   extern const char *Txt_Classroom_unspecified;
   extern const char *Txt_Another_classroom;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Clear data *****/
   Cla->ShrtName[0] = '\0';
   Cla->FullName[0] = '\0';
   Cla->Capacity = Cla_UNLIMITED_CAPACITY;

   /***** Check if classroom code is correct *****/
   if (Cla->ClaCod < 0)
     {
      Str_Copy (Cla->ShrtName,Txt_Classroom_unspecified,
                Cla_MAX_BYTES_CLASSROOM_SHRT_NAME);
      Str_Copy (Cla->FullName,Txt_Classroom_unspecified,
                Cla_MAX_BYTES_CLASSROOM_FULL_NAME);
     }
   else if (Cla->ClaCod == 0)
     {
      Str_Copy (Cla->ShrtName,Txt_Another_classroom,
                Cla_MAX_BYTES_CLASSROOM_SHRT_NAME);
      Str_Copy (Cla->FullName,Txt_Another_classroom,
                Cla_MAX_BYTES_CLASSROOM_FULL_NAME);
     }
   else if (Cla->ClaCod > 0)
     {
      /***** Get data of a classroom from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a classroom",
			        "SELECT ShortName,"
				       "FullName,"
				       "Capacity"
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
                   Cla_MAX_BYTES_CLASSROOM_SHRT_NAME);

         /* Get the full name of the classroom (row[1]) */
         Str_Copy (Cla->FullName,row[1],
                   Cla_MAX_BYTES_CLASSROOM_FULL_NAME);

         /* Get seating capacity in this classroom (row[2]) */
         if (sscanf (row[2],"%u",&Cla->Capacity) != 1)
            Cla->Capacity = Cla_UNLIMITED_CAPACITY;
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
      free ((void *) Gbl.Classrooms.Lst);
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

   /***** Write heading *****/
   Tbl_StartTableWide (2);
   Cla_PutHeadClassrooms ();

   /***** Write all the classrooms *****/
   for (NumCla = 0;
	NumCla < Gbl.Classrooms.Num;
	NumCla++)
     {
      Cla = &Gbl.Classrooms.Lst[NumCla];

      /* Put icon to remove classroom */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"BM\">");
      Frm_StartForm (ActRemCla);
      Cla_PutParamClaCod (Cla->ClaCod);
      Ico_PutIconRemove ();
      Frm_EndForm ();
      fprintf (Gbl.F.Out,"</td>");

      /* Classroom code */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%ld"
	                 "</td>",
               Cla->ClaCod);

      /* Classroom short name */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Frm_StartForm (ActRenClaSho);
      Cla_PutParamClaCod (Cla->ClaCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
	                 " maxlength=\"%u\" value=\"%s\""
                         " class=\"INPUT_SHORT_NAME\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Cla_MAX_CHARS_CLASSROOM_SHRT_NAME,Cla->ShrtName,Gbl.Form.Id);
      Frm_EndForm ();
      fprintf (Gbl.F.Out,"</td>");

      /* Classroom full name */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Frm_StartForm (ActRenClaFul);
      Cla_PutParamClaCod (Cla->ClaCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
	                 " maxlength=\"%u\" value=\"%s\""
                         " class=\"INPUT_FULL_NAME\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Cla_MAX_CHARS_CLASSROOM_FULL_NAME,Cla->FullName,Gbl.Form.Id);
      Frm_EndForm ();
      fprintf (Gbl.F.Out,"</td>");

      /* Seating capacity */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Frm_StartForm (ActChgClaMaxStd);
      Cla_PutParamClaCod (Cla->ClaCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Capacity\""
			 " size=\"3\" maxlength=\"10\" value=\"");
      Cla_WriteCapacity (Cla->Capacity);
      fprintf (Gbl.F.Out,"\" onchange=\"document.getElementById('%s').submit();\" />",
	       Gbl.Form.Id);
      Frm_EndForm ();
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   /***** End table *****/
   Tbl_EndTable ();
  }

/*****************************************************************************/
/****************** Write parameter with code of classroom *******************/
/*****************************************************************************/

static void Cla_PutParamClaCod (long ClaCod)
  {
   Par_PutHiddenParamLong ("ClaCod",ClaCod);
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
   struct Classroom Cla;

   /***** Get classroom code *****/
   if ((Cla.ClaCod = Cla_GetParamClaCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of classroom is missing.");

   /***** Get data of the classroom from database *****/
   Cla_GetDataOfClassroomByCod (&Cla);

   /***** Remove classroom *****/
   DB_QueryDELETE ("can not remove a classroom",
		   "DELETE FROM classrooms WHERE ClaCod=%ld",
		   Cla.ClaCod);

   /***** Write message to show the change made *****/
   snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	     Txt_Classroom_X_removed,
	     Cla.FullName);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);

   /***** Show the form again *****/
   Cla_EditClassrooms ();
  }

/*****************************************************************************/
/******************* Change the short name of a classroom ********************/
/*****************************************************************************/

void Cla_RenameClassroomShort (void)
  {
   Cla_RenameClassroom (Cns_SHRT_NAME);
  }

/*****************************************************************************/
/******************* Change the full name of a classroom *********************/
/*****************************************************************************/

void Cla_RenameClassroomFull (void)
  {
   Cla_RenameClassroom (Cns_FULL_NAME);
  }

/*****************************************************************************/
/********************** Change the name of a classroom ***********************/
/*****************************************************************************/

static void Cla_RenameClassroom (Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_classroom_X_empty;
   extern const char *Txt_The_classroom_X_already_exists;
   extern const char *Txt_The_classroom_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_classroom_X_has_not_changed;
   struct Classroom *Cla;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentClaName = NULL;		// Initialized to avoid warning
   char NewClaName[Cla_MAX_BYTES_CLASSROOM_FULL_NAME + 1];

   Cla = &Gbl.Classrooms.EditingCla;
   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Cla_MAX_BYTES_CLASSROOM_SHRT_NAME;
         CurrentClaName = Cla->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Cla_MAX_BYTES_CLASSROOM_FULL_NAME;
         CurrentClaName = Cla->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the classroom */
   if ((Cla->ClaCod = Cla_GetParamClaCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of classroom is missing.");

   /* Get the new name for the classroom */
   Par_GetParToText (ParamName,NewClaName,MaxBytes);

   /***** Get from the database the old names of the classroom *****/
   Cla_GetDataOfClassroomByCod (Cla);

   /***** Check if new name is empty *****/
   if (!NewClaName[0])
     {
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        Txt_You_can_not_leave_the_name_of_the_classroom_X_empty,
                CurrentClaName);
      Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
     }
   else
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentClaName,NewClaName))	// Different names
        {
         /***** If classroom was in database... *****/
         if (Cla_CheckIfClassroomNameExists (ParamName,NewClaName,Cla->ClaCod))
           {
            snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	              Txt_The_classroom_X_already_exists,
                      NewClaName);
            Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
           }
         else
           {
            /* Update the table changing old name by new name */
            Cla_UpdateClaNameDB (Cla->ClaCod,FieldName,NewClaName);

            /* Write message to show the change made */
            snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	              Txt_The_classroom_X_has_been_renamed_as_Y,
                      CurrentClaName,NewClaName);
            Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
           }
        }
      else	// The same name
        {
         snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	           Txt_The_name_of_the_classroom_X_has_not_changed,
                   CurrentClaName);
         Ale_ShowAlert (Ale_INFO,Gbl.Alert.Txt);
        }
     }

   /***** Show the form again *****/
   Str_Copy (CurrentClaName,NewClaName,
             MaxBytes);
   Cla_EditClassrooms ();
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
			  Gbl.CurrentCtr.Ctr.CtrCod,
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
/************** Change maximum number of students in a classroom *************/
/*****************************************************************************/

void Cla_ChangeMaxStudents (void)
  {
   extern const char *Txt_The_capacity_of_classroom_X_has_not_changed;
   extern const char *Txt_The_classroom_X_does_not_have_a_limited_capacity_now;
   extern const char *Txt_The_capacity_of_classroom_X_is_now_Y;
   struct Classroom *Cla;
   unsigned NewCapacity;

   /***** Use current editing classroom *****/
   Cla = &Gbl.Classrooms.EditingCla;

   /***** Get parameters from form *****/
   /* Get the code of the classroom */
   if ((Cla->ClaCod = Cla_GetParamClaCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of classroom is missing.");

   /* Get the seating capacity of the classroom */
   NewCapacity = (unsigned)
	        Par_GetParToUnsignedLong ("Capacity",
                                          0,
                                          Cla_MAX_CAPACITY_OF_A_CLASSROOM,
                                          Cla_UNLIMITED_CAPACITY);

   /***** Get data of the classroom from database *****/
   Cla_GetDataOfClassroomByCod (Cla);

   /***** Check if the old capacity equals the new one
          (this happens when return is pressed without changes) *****/
   if (Cla->Capacity == NewCapacity)
     {
      /***** Message to show no changes made *****/
      Gbl.Alert.Type = Ale_INFO;
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
		Txt_The_capacity_of_classroom_X_has_not_changed,
		Cla->FullName);
     }
   else
     {
      /***** Update the table of groups changing the old capacity to the new *****/
      DB_QueryUPDATE ("can not update the capacity of a classroom",
		      "UPDATE classrooms SET Capacity=%u WHERE ClaCod=%ld",
                      NewCapacity,Cla->ClaCod);
      Cla->Capacity = NewCapacity;

      /***** Message to show the change made *****/
      Gbl.Alert.Type = Ale_SUCCESS;
      if (NewCapacity > Grp_MAX_STUDENTS_IN_A_GROUP)
         snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	           Txt_The_classroom_X_does_not_have_a_limited_capacity_now,
                   Cla->FullName);
      else
         snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	           Txt_The_capacity_of_classroom_X_is_now_Y,
                   Cla->FullName,NewCapacity);
     }
   Ale_ShowAlert (Gbl.Alert.Type,Gbl.Alert.Txt);

   /***** Show the form again *****/
   Cla_EditClassrooms ();
  }

/*****************************************************************************/
/****************** Write seating capacity of a classroom ********************/
/*****************************************************************************/

static void Cla_WriteCapacity (unsigned Capacity)
  {
   if (Capacity <= Cla_MAX_CAPACITY_OF_A_CLASSROOM)
      fprintf (Gbl.F.Out,"%u",Capacity);
  }

/*****************************************************************************/
/******************* Put a form to create a new classroom ********************/
/*****************************************************************************/

static void Cla_PutFormToCreateClassroom (void)
  {
   extern const char *Txt_New_classroom;
   extern const char *Txt_Create_classroom;
   struct Classroom *Cla;

   /***** Classroom data *****/
   Cla = &Gbl.Classrooms.EditingCla;

   /***** Start form *****/
   Frm_StartForm (ActNewCla);

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_New_classroom,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Cla_PutHeadClassrooms ();

   /***** Column to remove classroom, disabled here *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"BM\"></td>");

   /***** Classroom code *****/
   fprintf (Gbl.F.Out,"<td class=\"CODE\"></td>");

   /***** Classroom short name *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"ShortName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_SHORT_NAME\""
                      " required=\"required\" />"
                      "</td>",
            Cla_MAX_CHARS_CLASSROOM_SHRT_NAME,Cla->ShrtName);

   /***** Classroom full name *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"FullName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_FULL_NAME\""
                      " required=\"required\" />"
                      "</td>",
            Cla_MAX_CHARS_CLASSROOM_FULL_NAME,Cla->FullName);

   /***** Seating capacity *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
	              "<input type=\"text\" name=\"Capacity\""
	              " size=\"3\" maxlength=\"10\" value=\"");
   Cla_WriteCapacity (Cla->Capacity);
   fprintf (Gbl.F.Out,"\" />"
	              "</td>"
		      "</tr>");

   /***** End table, send button and end box *****/
   Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_classroom);

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

   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"BM\"></th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Code,
            Txt_Short_name,
            Txt_Full_name,
            Txt_Capacity_OF_A_CLASSROOM);
  }

/*****************************************************************************/
/***************** Receive form to create a new classroom ********************/
/*****************************************************************************/

void Cla_RecFormNewClassroom (void)
  {
   extern const char *Txt_The_classroom_X_already_exists;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_classroom;
   struct Classroom *Cla;

   Cla = &Gbl.Classrooms.EditingCla;

   /***** Get parameters from form *****/
   /* Get classroom short name */
   Par_GetParToText ("ShortName",Cla->ShrtName,Cla_MAX_BYTES_CLASSROOM_SHRT_NAME);

   /* Get classroom full name */
   Par_GetParToText ("FullName",Cla->FullName,Cla_MAX_BYTES_CLASSROOM_FULL_NAME);

   /* Get seating capacity */
   Cla->Capacity = (unsigned)
	           Par_GetParToUnsignedLong ("Capacity",
                                             0,
                                             Cla_MAX_CAPACITY_OF_A_CLASSROOM,
                                             Cla_UNLIMITED_CAPACITY);

   if (Cla->ShrtName[0] && Cla->FullName[0])	// If there's a classroom name
     {
      /***** If name of classroom was in database... *****/
      if (Cla_CheckIfClassroomNameExists ("ShortName",Cla->ShrtName,-1L))
        {
         snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	           Txt_The_classroom_X_already_exists,
                   Cla->ShrtName);
         Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
        }
      else if (Cla_CheckIfClassroomNameExists ("FullName",Cla->FullName,-1L))
        {
         snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	           Txt_The_classroom_X_already_exists,
                   Cla->FullName);
         Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
        }
      else	// Add new classroom to database
         Cla_CreateClassroom (Cla);
     }
   else	// If there is not a classroom name
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_classroom);

   /***** Show the form again *****/
   Cla_EditClassrooms ();
  }

/*****************************************************************************/
/************************** Create a new classroom ***************************/
/*****************************************************************************/

static void Cla_CreateClassroom (struct Classroom *Cla)
  {
   extern const char *Txt_Created_new_classroom_X;

   /***** Create a new classroom *****/
   DB_QueryINSERT ("can not create classroom",
		   "INSERT INTO classrooms"
		   " (CtrCod,ShortName,FullName,Capacity)"
		   " VALUES"
		   " (%ld,'%s','%s',%u)",
                   Gbl.CurrentCtr.Ctr.CtrCod,
		   Cla->ShrtName,Cla->FullName,Cla->Capacity);

   /***** Write success message *****/
   snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	     Txt_Created_new_classroom_X,
             Cla->FullName);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
  }
