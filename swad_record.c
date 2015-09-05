// swad_record.c: users' record cards

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>

#include "swad_action.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_enrollment.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_logo.h"
#include "swad_network.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_preference.h"
#include "swad_QR.h"
#include "swad_record.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;
extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Rec_INSTITUTION_LOGO_SIZE	48
#define Rec_DEGREE_LOGO_SIZE		48
#define Rec_SHOW_OFFICE_HOURS_DEFAULT	true

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Rec_WriteHeadingRecordFields (void);
static void Rec_GetFieldByCod (long FieldCod,char *Name,unsigned *NumLines,Rec_VisibilityRecordFields_t *Visibility);

static void Rec_ShowRecordOneStdCrs (void);
static void Rec_ShowRecordOneTchCrs (void);

static void Rec_GetParamRecordsPerPage (void);
static void Rec_WriteFormShowOfficeHours (bool ShowOfficeHours,const char *ListUsrCods);
static bool Rec_GetParamShowOfficeHours (void);
static void Rec_ShowMyCrsRecordUpdated (void);
static void Rec_ShowOtherCrsRecordUpdated (void);
static void Rec_WriteLinkToDataProtectionClause (void);

static void Rec_GetUsrCommentsFromForm (struct UsrData *UsrDat);
static void Rec_PutLinkToChangeMyInsCtrDpt (void);
static void Rec_PutLinkToChangeMySocialNetworks (void);

/*****************************************************************************/
/*************** Create, edit and remove fields of records *******************/
/*****************************************************************************/

void Rec_ReqEditRecordFields (void)
  {
   extern const char *Txt_There_are_no_record_fields_in_the_course_X;
   extern const char *Txt_Record_fields;

   /***** Get list of fields of records in current course *****/
   Rec_GetListRecordFieldsInCurrentCrs ();

   /***** List the current fields of records for edit them *****/
   if (Gbl.CurrentCrs.Records.LstFields.Num)	// Fields found...
     {
      Lay_StartRoundFrameTable (NULL,2,Txt_Record_fields);
      Rec_ListFieldsRecordsForEdition ();
      Lay_EndRoundFrameTable ();
     }
   else	// No fields of records found for current course in the database
     {
      sprintf (Gbl.Message,Txt_There_are_no_record_fields_in_the_course_X,
               Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);
     }

   /***** Put a form to create a new record field *****/
   Rec_ShowFormCreateRecordField ();

   /* Free list of fields of records */
   Rec_FreeListFields ();
  }

/*****************************************************************************/
/****** Create a list with the fields of records from current course *********/
/*****************************************************************************/

void Rec_GetListRecordFieldsInCurrentCrs (void)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned Vis;

   if (++Gbl.CurrentCrs.Records.LstFields.NestedCalls > 1) // If the list is already created, don't do anything
      return;

   /***** Get fields of cards of a course from database *****/
   sprintf (Query,"SELECT FieldCod,FieldName,NumLines,Visibility"
	          " FROM crs_record_fields"
                  " WHERE CrsCod='%ld' ORDER BY FieldName",
            Gbl.CurrentCrs.Crs.CrsCod);
   Gbl.CurrentCrs.Records.LstFields.Num = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get fields of cards of a course");

   /***** Get the fields of records *****/
   if (Gbl.CurrentCrs.Records.LstFields.Num)
     {
      /***** Create a list of fields *****/
      if ((Gbl.CurrentCrs.Records.LstFields.Lst = (struct RecordField *) calloc (Gbl.CurrentCrs.Records.LstFields.Num,sizeof (struct RecordField))) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store fields of records in current course.");

      /***** Get the fields *****/
      for (NumRow = 0;
	   NumRow < Gbl.CurrentCrs.Records.LstFields.Num;
	   NumRow++)
        {
         /* Get next field */
         row = mysql_fetch_row (mysql_res);

         /* Get the code of field (row[0]) */
         if ((Gbl.CurrentCrs.Records.LstFields.Lst[NumRow].FieldCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of field.");

         /* Name of the field (row[1]) */
         strncpy (Gbl.CurrentCrs.Records.LstFields.Lst[NumRow].Name,row[1],Rec_MAX_LENGTH_NAME_FIELD);

         /* Number of lines (row[2]) */
         Gbl.CurrentCrs.Records.LstFields.Lst[NumRow].NumLines = Rec_ConvertToNumLinesField (row[2]);

         /* Visible or editable by students? (row[3]) */
         if (sscanf (row[3],"%u",&Vis) != 1)
	    Lay_ShowErrorAndExit ("Error when getting field of record in current course.");
         if (Vis < Rec_NUM_TYPES_VISIBILITY)
            Gbl.CurrentCrs.Records.LstFields.Lst[NumRow].Visibility = (Rec_VisibilityRecordFields_t) Vis;
         else
            Gbl.CurrentCrs.Records.LstFields.Lst[NumRow].Visibility = Rec_HIDDEN_FIELD;
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********* List the fields of records already present in database ************/
/*****************************************************************************/

void Rec_ListFieldsRecordsForEdition (void)
  {
   extern const char *Txt_RECORD_FIELD_VISIBILITY_MENU[Rec_NUM_TYPES_VISIBILITY];
   unsigned NumField;
   Rec_VisibilityRecordFields_t Vis;

   /***** Write heading *****/
   Rec_WriteHeadingRecordFields ();

   /***** List the fields *****/
   for (NumField = 0;
	NumField < Gbl.CurrentCrs.Records.LstFields.Num;
	NumField++)
     {
      fprintf (Gbl.F.Out,"<tr>");

      /* Write icon to remove the field */
      fprintf (Gbl.F.Out,"<td class=\"BM%u\">",Gbl.RowEvenOdd);
      Act_FormStart (ActReqRemFie);
      Par_PutHiddenParamLong ("FieldCod",Gbl.CurrentCrs.Records.LstFields.Lst[NumField].FieldCod);
      Lay_PutIconRemove ();
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Name of the field */
      fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">");
      Act_FormStart (ActRenFie);
      Par_PutHiddenParamLong ("FieldCod",Gbl.CurrentCrs.Records.LstFields.Lst[NumField].FieldCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FieldName\""
	                 " style=\"width:400px;\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"javascript:document.getElementById('%s').submit();\" />",
               Rec_MAX_LENGTH_NAME_FIELD,
               Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Name,
               Gbl.FormId);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Number of lines in the form */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActChgRowFie);
      Par_PutHiddenParamLong ("FieldCod",Gbl.CurrentCrs.Records.LstFields.Lst[NumField].FieldCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"NumLines\" size=\"2\""
                         " maxlength=\"2\" value=\"%u\""
                         " onchange=\"javascript:document.getElementById('%s').submit();\" />",
               Gbl.CurrentCrs.Records.LstFields.Lst[NumField].NumLines,
               Gbl.FormId);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Visibility of a field */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActChgVisFie);
      Par_PutHiddenParamLong ("FieldCod",Gbl.CurrentCrs.Records.LstFields.Lst[NumField].FieldCod);
      fprintf (Gbl.F.Out,"<select name=\"Visibility\""
                         " onchange=\"javascript:document.getElementById('%s').submit();\">",
               Gbl.FormId);
      for (Vis = (Rec_VisibilityRecordFields_t) 0;
	   Vis < (Rec_VisibilityRecordFields_t) Rec_NUM_TYPES_VISIBILITY;
	   Vis++)
        {
         fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Vis);
         if (Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Visibility == Vis)
	    fprintf (Gbl.F.Out," selected=\"selected\"");
         fprintf (Gbl.F.Out,">%s</option>",
                  Txt_RECORD_FIELD_VISIBILITY_MENU[Vis]);
        }
      fprintf (Gbl.F.Out,"</select>");
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");
     }
  }

/*****************************************************************************/
/******************* Show form to create a new record field ******************/
/*****************************************************************************/

void Rec_ShowFormCreateRecordField (void)
  {
   extern const char *Txt_New_record_field;
   extern const char *Txt_RECORD_FIELD_VISIBILITY_MENU[Rec_NUM_TYPES_VISIBILITY];
   extern const char *Txt_Create_record_field;
   Rec_VisibilityRecordFields_t Vis;

   /***** Start form *****/
   Act_FormStart (ActNewFie);

   /***** Start of frame *****/
   Lay_StartRoundFrameTable (NULL,0,Txt_New_record_field);

   /***** Write heading *****/
   Rec_WriteHeadingRecordFields ();

   /***** Write disabled icon to remove the field *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"BM\">");
   Lay_PutIconRemovalNotAllowed ();
   fprintf (Gbl.F.Out,"</td>");

   /***** Field name *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" name=\"FieldName\""
                      " style=\"width:400px;\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            Rec_MAX_LENGTH_NAME_FIELD,Gbl.CurrentCrs.Records.Field.Name);

   /***** Number of lines in form ******/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
	              "<input type=\"text\" name=\"NumLines\" size=\"2\" maxlength=\"2\" value=\"%u\" />"
	              "</td>",
            Gbl.CurrentCrs.Records.Field.NumLines);

   /***** Visibility to students *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
	              "<select name=\"Visibility\">");
   for (Vis = (Rec_VisibilityRecordFields_t) 0;
	Vis < (Rec_VisibilityRecordFields_t) Rec_NUM_TYPES_VISIBILITY;
	Vis++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Vis);
      if (Gbl.CurrentCrs.Records.Field.Visibility == Vis)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",
	       Txt_RECORD_FIELD_VISIBILITY_MENU[Vis]);
     }

   fprintf (Gbl.F.Out,"</select>"
	              "</td>"
	              "</tr>");

   /***** Send button and end frame *****/
   Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_record_field);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/************************** Write heading of groups **************************/
/*****************************************************************************/

static void Rec_WriteHeadingRecordFields (void)
  {
   extern const char *Txt_Field_BR_name;
   extern const char *Txt_No_of_BR_lines;
   extern const char *Txt_Visible_by_BR_the_student;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>"
                      "<th class=\"TIT_TBL CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Field_BR_name,
            Txt_No_of_BR_lines,
            Txt_Visible_by_BR_the_student);
  }

/*****************************************************************************/
/*************** Receive data from a form of record fields *******************/
/*****************************************************************************/

void Rec_ReceiveFormField (void)
  {
   extern const char *Txt_The_record_field_X_already_exists;
   extern const char *Txt_You_must_specify_the_name_of_the_new_record_field;
   char UnsignedStr[10+1];
   unsigned Vis;

   /***** Get parameters from the form *****/
   /* Get the name of the field */
   Par_GetParToText ("FieldName",Gbl.CurrentCrs.Records.Field.Name,Rec_MAX_LENGTH_NAME_FIELD);

   /* Get the number of lines */
   Par_GetParToText ("NumLines",UnsignedStr,10);
   Gbl.CurrentCrs.Records.Field.NumLines = Rec_ConvertToNumLinesField (UnsignedStr);

   /* Get the field visibility by students */
   Par_GetParToText ("Visibility",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Vis) != 1)
      Lay_ShowErrorAndExit ("Visibility is missing.");
   if (Vis < Rec_NUM_TYPES_VISIBILITY)
      Gbl.CurrentCrs.Records.Field.Visibility = (Rec_VisibilityRecordFields_t) Vis;
   else
      Gbl.CurrentCrs.Records.Field.Visibility = Rec_HIDDEN_FIELD;

   if (Gbl.CurrentCrs.Records.Field.Name[0])	// If there's a name
     {
      /***** If the field already was in the database... *****/
      if (Rec_CheckIfRecordFieldIsRepeated (Gbl.CurrentCrs.Records.Field.Name))
        {
         sprintf (Gbl.Message,Txt_The_record_field_X_already_exists,
                  Gbl.CurrentCrs.Records.Field.Name);
         Lay_ShowAlert (Lay_ERROR,Gbl.Message);
        }
      else	// Add the new field to the database
         Rec_CreateRecordField ();
     }
   else		// If there is not name
      Lay_ShowAlert (Lay_ERROR,Txt_You_must_specify_the_name_of_the_new_record_field);

   /***** Show the form again *****/
   Rec_ReqEditRecordFields ();
  }

/*****************************************************************************/
/********* Get number of lines of the form to edit a record field ************/
/*****************************************************************************/

unsigned Rec_ConvertToNumLinesField (const char *StrNumLines)
  {
   int NumLines;

   if (sscanf (StrNumLines,"%d",&NumLines) != 1)
      return Rec_MIN_LINES_IN_EDITION_FIELD;
   else if (NumLines < Rec_MIN_LINES_IN_EDITION_FIELD)
      return Rec_MIN_LINES_IN_EDITION_FIELD;
   else if (NumLines > Rec_MAX_LINES_IN_EDITION_FIELD)
      return Rec_MAX_LINES_IN_EDITION_FIELD;
   return (unsigned) NumLines;
  }

/*****************************************************************************/
/* Check if the name of the field of record equals any of the existing ones **/
/*****************************************************************************/

bool Rec_CheckIfRecordFieldIsRepeated (const char *FieldName)
  {
   bool FieldIsRepeated = false;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;

   /* Query database */
   if ((NumRows = Rec_GetAllFieldsInCurrCrs (&mysql_res)) > 0)	// If se han encontrado groups...
      /* Compare with all the tipos of group from the database */
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
        {
	 /* Get next type of group */
	 row = mysql_fetch_row (mysql_res);

         /* The name of the field is in row[1] */
         if (!strcasecmp (FieldName,row[1]))
           {
            FieldIsRepeated = true;
            break;
           }
	}

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   return FieldIsRepeated;
  }

/*****************************************************************************/
/******* Get the fields of records already present in current course *********/
/*****************************************************************************/

unsigned long Rec_GetAllFieldsInCurrCrs (MYSQL_RES **mysql_res)
  {
   char Query[512];

   /***** Get fields of cards of current course from database *****/
   sprintf (Query,"SELECT FieldCod,FieldName,Visibility"
	          " FROM crs_record_fields"
                  " WHERE CrsCod='%ld' ORDER BY FieldName",
            Gbl.CurrentCrs.Crs.CrsCod);
   return DB_QuerySELECT (Query,mysql_res,
                          "can not get fields of cards of a course");
  }

/*****************************************************************************/
/************************* Create a field of record **************************/
/*****************************************************************************/

void Rec_CreateRecordField (void)
  {
   extern const char *Txt_Created_new_record_field_X;
   char Query[1024];

   /***** Create the new field *****/
   sprintf (Query,"INSERT INTO crs_record_fields"
	          " (CrsCod,FieldName,NumLines,Visibility)"
	          " VALUES ('%ld','%s','%u','%u')",
            Gbl.CurrentCrs.Crs.CrsCod,
            Gbl.CurrentCrs.Records.Field.Name,
            Gbl.CurrentCrs.Records.Field.NumLines,
            (unsigned) Gbl.CurrentCrs.Records.Field.Visibility);
   DB_QueryINSERT (Query,"can not create field of record");

   /***** Write message of success *****/
   sprintf (Gbl.Message,Txt_Created_new_record_field_X,
            Gbl.CurrentCrs.Records.Field.Name);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/**************** Request the removing of a field of records *****************/
/*****************************************************************************/

void Rec_ReqRemField (void)
  {
   unsigned NumRecords;

   /***** Get the code of field *****/
   if ((Gbl.CurrentCrs.Records.Field.FieldCod = Rec_GetFieldCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of field is missing.");

   /***** Check if exists any record with that field filled *****/
   if ((NumRecords = Rec_CountNumRecordsInCurrCrsWithField (Gbl.CurrentCrs.Records.Field.FieldCod)))	// There are records with that field filled
      Rec_AskConfirmRemFieldWithRecords (NumRecords);
   else			// There are no records with that field filled
      Rec_RemoveFieldFromDB ();
  }

/*****************************************************************************/
/************ Get a parameter with a code of field of records ****************/
/*****************************************************************************/

long Rec_GetFieldCod (void)
  {
   char LongStr[1+10+1];

   /***** Get the code of the field *****/
   Par_GetParToText ("FieldCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/*************** Get the number of records with a field filled ***************/
/*****************************************************************************/

unsigned Rec_CountNumRecordsInCurrCrsWithField (long FieldCod)
  {
   char Query[128];

   /***** Get number of cards with a given field in a course from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM crs_records WHERE FieldCod='%ld'",
            FieldCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of cards with a given field not empty in a course");
  }

/*****************************************************************************/
/******* Request confirmation for the removing of a field with records *******/
/*****************************************************************************/

void Rec_AskConfirmRemFieldWithRecords (unsigned NumRecords)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_field_X_from_the_records_of_X;
   extern const char *Txt_this_field_is_filled_in_the_record_of_one_student;
   extern const char *Txt_this_field_is_filled_in_the_records_of_X_students;
   extern const char *Txt_Remove_record_field;
   char Message_part2[512];

   /***** Get from the database the name of the field *****/
   Rec_GetFieldByCod (Gbl.CurrentCrs.Records.Field.FieldCod,Gbl.CurrentCrs.Records.Field.Name,&Gbl.CurrentCrs.Records.Field.NumLines,&Gbl.CurrentCrs.Records.Field.Visibility);

   /***** Write mensaje to ask confirmation of deletion *****/
   sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_field_X_from_the_records_of_X,
            Gbl.CurrentCrs.Records.Field.Name,Gbl.CurrentCrs.Crs.FullName);
   if (NumRecords == 1)
      strcat (Gbl.Message,Txt_this_field_is_filled_in_the_record_of_one_student);
   else
     {
      sprintf (Message_part2,Txt_this_field_is_filled_in_the_records_of_X_students,
               NumRecords);
      strcat (Gbl.Message,Message_part2);
     }
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);

   /***** Button to confirm removing *****/
   Act_FormStart (ActRemFie);
   Par_PutHiddenParamLong ("FieldCod",Gbl.CurrentCrs.Records.Field.FieldCod);
   Lay_PutRemoveButton (Txt_Remove_record_field);
   Act_FormEnd ();
  }

/*****************************************************************************/
/************** Remove from the database a field of records ******************/
/*****************************************************************************/

void Rec_RemoveFieldFromDB (void)
  {
   extern const char *Txt_Record_field_X_removed;
   char Query[512];

   /***** Get from the database the name of the field *****/
   Rec_GetFieldByCod (Gbl.CurrentCrs.Records.Field.FieldCod,Gbl.CurrentCrs.Records.Field.Name,&Gbl.CurrentCrs.Records.Field.NumLines,&Gbl.CurrentCrs.Records.Field.Visibility);

   /***** Remove field from all records *****/
   sprintf (Query,"DELETE FROM crs_records WHERE FieldCod='%ld'",
            Gbl.CurrentCrs.Records.Field.FieldCod);
   DB_QueryDELETE (Query,"can not remove field from all students' records");

   /***** Remove the field *****/
   sprintf (Query,"DELETE FROM crs_record_fields WHERE FieldCod='%ld'",
            Gbl.CurrentCrs.Records.Field.FieldCod);
   DB_QueryDELETE (Query,"can not remove field of record");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Record_field_X_removed,
            Gbl.CurrentCrs.Records.Field.Name);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the form again *****/
   Rec_ReqEditRecordFields ();
  }

/*****************************************************************************/
/************** Get the data of a field of records from its code *************/
/*****************************************************************************/

static void Rec_GetFieldByCod (long FieldCod,char *Name,unsigned *NumLines,Rec_VisibilityRecordFields_t *Visibility)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned Vis;

   /***** Get a field of a record in a course from database *****/
   sprintf (Query,"SELECT FieldName,NumLines,Visibility FROM crs_record_fields"
                  " WHERE CrsCod='%ld' AND FieldCod='%ld'",Gbl.CurrentCrs.Crs.CrsCod,FieldCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get a field of a record in a course");

   /***** Count number of rows in result *****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Error when getting a field of a record in a course.");

   /***** Get the field *****/
   row = mysql_fetch_row (mysql_res);

   /* Name of the field */
   strcpy (Name,row[0]);

   /* Number of lines of the field (row[1]) */
   *NumLines = Rec_ConvertToNumLinesField (row[1]);

   /* Visible or editable by students? (row[2]) */
   if (sscanf (row[2],"%u",&Vis) != 1)
      Lay_ShowErrorAndExit ("Error when getting a field of a record in a course.");
   if (Vis < Rec_NUM_TYPES_VISIBILITY)
      *Visibility = (Rec_VisibilityRecordFields_t) Vis;
   else
      *Visibility = Rec_HIDDEN_FIELD;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************* Remove a field of records *************************/
/*****************************************************************************/

void Rec_RemoveField (void)
  {
   /***** Get the code of the field *****/
   if ((Gbl.CurrentCrs.Records.Field.FieldCod = Rec_GetFieldCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of field is missing.");

   /***** Borrarlo from the database *****/
   Rec_RemoveFieldFromDB ();
  }

/*****************************************************************************/
/************************** Rename a field of records ************************/
/*****************************************************************************/

void Rec_RenameField (void)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_field_X_empty;
   extern const char *Txt_The_record_field_X_already_exists;
   extern const char *Txt_The_record_field_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_field_X_has_not_changed;
   char Query[1024];
   char NewFieldName[Rec_MAX_LENGTH_NAME_FIELD+1];

   /***** Get parameters of the form *****/
   /* Get the code of the field */
   if ((Gbl.CurrentCrs.Records.Field.FieldCod = Rec_GetFieldCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of field is missing.");

   /* Get the new group name */
   Par_GetParToText ("FieldName",NewFieldName,Rec_MAX_LENGTH_NAME_FIELD);

   /***** Get from the database the antiguo group name *****/
   Rec_GetFieldByCod (Gbl.CurrentCrs.Records.Field.FieldCod,Gbl.CurrentCrs.Records.Field.Name,&Gbl.CurrentCrs.Records.Field.NumLines,&Gbl.CurrentCrs.Records.Field.Visibility);

   /***** Check if new name is empty *****/
   if (!NewFieldName[0])
     {
      sprintf (Gbl.Message,Txt_You_can_not_leave_the_name_of_the_field_X_empty,
               Gbl.CurrentCrs.Records.Field.Name);
      Lay_ShowAlert (Lay_ERROR,Gbl.Message);
     }
   else
     {
      /***** Check if the name of the olde field match the new one
             (this happens when return is pressed without changes) *****/
      if (strcmp (Gbl.CurrentCrs.Records.Field.Name,NewFieldName))	// Different names
        {
         /***** If the group ya estaba in the database... *****/
         if (Rec_CheckIfRecordFieldIsRepeated (NewFieldName))
           {
            sprintf (Gbl.Message,Txt_The_record_field_X_already_exists,
                     NewFieldName);
            Lay_ShowAlert (Lay_ERROR,Gbl.Message);
           }
         else
           {
            /* Update the table of fields changing then old name by the new one */
            sprintf (Query,"UPDATE crs_record_fields SET FieldName='%s' WHERE FieldCod='%ld'",
                     NewFieldName,Gbl.CurrentCrs.Records.Field.FieldCod);
            DB_QueryUPDATE (Query,"can not update name of field of record");

            /***** Write message to show the change made *****/
            sprintf (Gbl.Message,Txt_The_record_field_X_has_been_renamed_as_Y,
                     Gbl.CurrentCrs.Records.Field.Name,NewFieldName);
            Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
           }
        }
      else	// The same name
        {
         sprintf (Gbl.Message,Txt_The_name_of_the_field_X_has_not_changed,
                  NewFieldName);
         Lay_ShowAlert (Lay_INFO,Gbl.Message);
        }
     }

   /***** Show the form again *****/
   strcpy (Gbl.CurrentCrs.Records.Field.Name,NewFieldName);
   Rec_ReqEditRecordFields ();
  }

/*****************************************************************************/
/********* Change number of lines of the form of a field of records **********/
/*****************************************************************************/

void Rec_ChangeLinesField (void)
  {
   extern const char *Txt_The_number_of_editing_lines_in_the_record_field_X_has_not_changed;
   extern const char *Txt_From_now_on_the_number_of_editing_lines_of_the_field_X_is_Y;
   char Query[1024];
   char UnsignedStr[10+1];
   unsigned NewNumLines;

   /***** Get parameters of the form *****/
   /* Get the code of field */
   if ((Gbl.CurrentCrs.Records.Field.FieldCod = Rec_GetFieldCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of field is missing.");

   /* Get the new number of lines */
   Par_GetParToText ("NumLines",UnsignedStr,10);
   NewNumLines = Rec_ConvertToNumLinesField (UnsignedStr);

   /* Get from the database the number of lines of the field */
   Rec_GetFieldByCod (Gbl.CurrentCrs.Records.Field.FieldCod,Gbl.CurrentCrs.Records.Field.Name,&Gbl.CurrentCrs.Records.Field.NumLines,&Gbl.CurrentCrs.Records.Field.Visibility);

   /***** Check if the old number of rows antiguo match the new one
          (this happens when return is pressed without changes) *****/
   if (Gbl.CurrentCrs.Records.Field.NumLines == NewNumLines)
     {
      sprintf (Gbl.Message,Txt_The_number_of_editing_lines_in_the_record_field_X_has_not_changed,
               Gbl.CurrentCrs.Records.Field.Name);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);
     }
   else
     {
      /***** Update of the table of fields changing the old maximum of students by the new one *****/
      sprintf (Query,"UPDATE crs_record_fields SET NumLines='%u' WHERE FieldCod='%ld'",
               NewNumLines,Gbl.CurrentCrs.Records.Field.FieldCod);
      DB_QueryUPDATE (Query,"can not update the number of lines of a field of record");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Message,Txt_From_now_on_the_number_of_editing_lines_of_the_field_X_is_Y,
	       Gbl.CurrentCrs.Records.Field.Name,NewNumLines);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Show the form again *****/
   Gbl.CurrentCrs.Records.Field.NumLines = NewNumLines;
   Rec_ReqEditRecordFields ();
  }

/*****************************************************************************/
/************ Change wisibility by students of a field of records ************/
/*****************************************************************************/

void Rec_ChangeVisibilityField (void)
  {
   extern const char *Txt_The_visibility_of_the_record_field_X_has_not_changed;
   extern const char *Txt_RECORD_FIELD_VISIBILITY_MSG[Rec_NUM_TYPES_VISIBILITY];
   char Query[1024];
   char UnsignedStr[10+1];
   unsigned Vis;
   Rec_VisibilityRecordFields_t NewVisibility;

   /***** Get parameters of the form *****/
   /* Get the code of field */
   if ((Gbl.CurrentCrs.Records.Field.FieldCod = Rec_GetFieldCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of field is missing.");

   /* Get the new visibility of the field */
   Par_GetParToText ("Visibility",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Vis) != 1)
      Lay_ShowErrorAndExit ("Visibility of field is missing.");
   if (Vis < Rec_NUM_TYPES_VISIBILITY)
      NewVisibility = (Rec_VisibilityRecordFields_t) Vis;
   else
      NewVisibility = Rec_HIDDEN_FIELD;

   /* Get from the database the visibility of the field */
   Rec_GetFieldByCod (Gbl.CurrentCrs.Records.Field.FieldCod,Gbl.CurrentCrs.Records.Field.Name,&Gbl.CurrentCrs.Records.Field.NumLines,&Gbl.CurrentCrs.Records.Field.Visibility);

   /***** Check if the old visibility matches the new one
          (this happens whe return is pressed without changes in the form) *****/
   if (Gbl.CurrentCrs.Records.Field.Visibility == NewVisibility)
     {
      sprintf (Gbl.Message,Txt_The_visibility_of_the_record_field_X_has_not_changed,
               Gbl.CurrentCrs.Records.Field.Name);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);
     }
   else
     {
      /***** Update of the table of fields changing the old visibility by the new *****/
      sprintf (Query,"UPDATE crs_record_fields SET Visibility='%u' WHERE FieldCod='%ld'",
               (unsigned) NewVisibility,Gbl.CurrentCrs.Records.Field.FieldCod);
      DB_QueryUPDATE (Query,"can not update the visibility of a field of record");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Message,Txt_RECORD_FIELD_VISIBILITY_MSG[NewVisibility],
	       Gbl.CurrentCrs.Records.Field.Name);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Show the form again *****/
   Gbl.CurrentCrs.Records.Field.Visibility = NewVisibility;
   Rec_ReqEditRecordFields ();
  }

/*****************************************************************************/
/********************** Liberar list of fields of records ********************/
/*****************************************************************************/

void Rec_FreeListFields (void)
  {
   if (Gbl.CurrentCrs.Records.LstFields.NestedCalls > 0)
      if (--Gbl.CurrentCrs.Records.LstFields.NestedCalls == 0)
         if (Gbl.CurrentCrs.Records.LstFields.Lst)
           {
            free ((void *) Gbl.CurrentCrs.Records.LstFields.Lst);
            Gbl.CurrentCrs.Records.LstFields.Lst = NULL;
            Gbl.CurrentCrs.Records.LstFields.Num = 0;
           }
  }

/*****************************************************************************/
/******************* Put a link to list official students ********************/
/*****************************************************************************/

void Rec_PutLinkToEditRecordFields (void)
  {
   extern const char *Txt_Edit_record_fields;

   /***** Link to edit record fields *****/
   Act_PutContextualLink (ActEdiRecFie,NULL,"edit",Txt_Edit_record_fields);
  }

/*****************************************************************************/
/*********************** Draw records of several guests **********************/
/*****************************************************************************/

void Rec_ListRecordsInvs (void)
  {
   extern const char *Txt_You_must_select_one_ore_more_users;
   unsigned NumUsrs = 0;
   const char *Ptr;
   Rec_RecordViewType_t TypeOfView = (Gbl.CurrentAct == ActSeeRecSevInv) ? Rec_RECORD_LIST :
                                                                           Rec_RECORD_PRINT;
   struct UsrData UsrDat;

   /***** Assign users listing type depending on current action *****/
   Gbl.Usrs.Listing.RecsUsrs = Rec_RECORD_USERS_GUESTS;

   /***** Get parameter with number of user records per page (only for printing) *****/
   if (Gbl.CurrentAct == ActPrnRecSevInv)
      Rec_GetParamRecordsPerPage ();

   /***** Get list of selected users *****/
   Usr_GetListSelectedUsrs ();

   /* Check the number of students to show */
   if (!Usr_CountNumUsrsInEncryptedList ())	// If no students selected...
     {						// ...write warning notice
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_select_one_ore_more_users);
      Usr_SeeGuests ();			// ...show again the form
      return;
     }

   if (Gbl.CurrentAct == ActSeeRecSevInv)
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

      /* Link to print view */
      Act_FormStart (ActPrnRecSevInv);
      Usr_PutHiddenParUsrCodAll (ActPrnRecSevInv,Gbl.Usrs.Select.All);
      Rec_ShowLinkToPrintPreviewOfRecords ();
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);
   UsrDat.Accepted = false;	// Guests have no courses,...
				// ...so they have not accepted...
				// ...inscription in any course

   /***** List the records *****/
   Ptr = Gbl.Usrs.Select.All;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EncryptedUsrCod,Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))                // Get from the database the data of the student
	{
	 fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\""
	                    " style=\"margin-bottom:10px;");
	 if (Gbl.CurrentAct == ActPrnRecSevInv &&
	     NumUsrs != 0 &&
	     (NumUsrs % Gbl.Usrs.Listing.RecsPerPag) == 0)
	    fprintf (Gbl.F.Out,"page-break-before:always;");
	 fprintf (Gbl.F.Out,"\">");

	 /* Common record */
	 Rec_ShowSharedUsrRecord (TypeOfView,&UsrDat);

	 fprintf (Gbl.F.Out,"</div>");

	 NumUsrs++;
	}
     }
   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);

   /***** Free list of fields of records *****/
   if (Gbl.Usrs.Listing.RecsUsrs == Rec_RECORD_USERS_STUDENTS)
      Rec_FreeListFields ();

   /***** Free memory used for by the list of users *****/
   Usr_FreeListsEncryptedUsrCods ();
  }

/*****************************************************************************/
/********** Get user's data and draw record of one unique student ************/
/*****************************************************************************/

void Rec_GetUsrAndShowRecordOneStdCrs (void)
  {
   /***** Get the selected student *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))	// Get from the database the data of the student
      if ((Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB =
	   Rol_GetRoleInCrs (Gbl.CurrentCrs.Crs.CrsCod,
	                     Gbl.Usrs.Other.UsrDat.UsrCod)) == Rol_STUDENT)
	 Rec_ShowRecordOneStdCrs ();
  }

/*****************************************************************************/
/******************** Draw record of one unique student **********************/
/*****************************************************************************/

static void Rec_ShowRecordOneStdCrs (void)
  {
   bool ItsMe;

   /***** Get if student has accepted enrollment in current course *****/
   Gbl.Usrs.Other.UsrDat.Accepted = Usr_GetIfUserHasAcceptedEnrollmentInCurrentCrs (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Asign users listing type depending on current action *****/
   Gbl.Usrs.Listing.RecsUsrs = Rec_RECORD_USERS_STUDENTS;

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

   /***** Link to edit record fields *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER)
      Rec_PutLinkToEditRecordFields ();

   /***** Link to print view *****/
   Act_FormStart (ActPrnRecSevStd);
   Usr_PutHiddenParUsrCodAll (ActPrnRecSevStd,Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
   Rec_ShowLinkToPrintPreviewOfRecords ();
   Act_FormEnd ();

   fprintf (Gbl.F.Out,"</div>");

   /***** Common record *****/
   Rec_ShowSharedUsrRecord (Rec_RECORD_LIST,&Gbl.Usrs.Other.UsrDat);

   /***** Record of the student in the course *****/
   /* Get list of fields of records in current course */
   Rec_GetListRecordFieldsInCurrentCrs ();

   if (Gbl.CurrentCrs.Records.LstFields.Num)	// There are fields in the record
     {
      if (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER ||
	  Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
	 Rec_ShowCrsRecord (Rec_RECORD_LIST,&Gbl.Usrs.Other.UsrDat);
      else
	{
	 ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod ==
	          Gbl.Usrs.Other.UsrDat.UsrCod);	// It's me
	 if (ItsMe && Gbl.Usrs.Me.LoggedRole == Rol_STUDENT)
	    Rec_ShowCrsRecord (Rec_FORM_MY_COURSE_RECORD_AS_STUDENT,&Gbl.Usrs.Other.UsrDat);
	}
     }

   /* Free list of fields of records */
   Rec_FreeListFields ();
  }

/*****************************************************************************/
/******************** Draw records of several students ***********************/
/*****************************************************************************/

void Rec_ListRecordsStdsCrs (void)
  {
   extern const char *Txt_You_must_select_one_ore_more_students;
   unsigned NumUsrs = 0;
   const char *Ptr;
   bool ItsMe;
   Rec_RecordViewType_t TypeOfView = (Gbl.CurrentAct == ActSeeRecSevStd) ? Rec_RECORD_LIST :
                                                                           Rec_RECORD_PRINT;
   struct UsrData UsrDat;

   /***** Asign users listing type depending on current action *****/
   Gbl.Usrs.Listing.RecsUsrs = Rec_RECORD_USERS_STUDENTS;

   /***** Get parameter with number of user records per page (only for printing) *****/
   if (Gbl.CurrentAct == ActPrnRecSevStd)
      Rec_GetParamRecordsPerPage ();

   /***** Get list of selected students *****/
   Usr_GetListSelectedUsrs ();

   /* Check the number of students to show */
   if (!Usr_CountNumUsrsInEncryptedList ())	// If no students selected...
     {						// ...write warning notice
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_select_one_ore_more_students);
      Usr_SeeStudents ();			// ...show again the form
      return;
     }

   /***** Get list of fields of records in current course *****/
   Rec_GetListRecordFieldsInCurrentCrs ();

   if (Gbl.CurrentAct == ActSeeRecSevStd)
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

      /* Link to edit record fields */
      if (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER)
         Rec_PutLinkToEditRecordFields ();

      /* Link to print view */
      Act_FormStart (ActPrnRecSevStd);
      Usr_PutHiddenParUsrCodAll (ActPrnRecSevStd,Gbl.Usrs.Select.All);
      Rec_ShowLinkToPrintPreviewOfRecords ();
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** List the records *****/
   Ptr = Gbl.Usrs.Select.All;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EncryptedUsrCod,Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))                // Get from the database the data of the student
         if (Usr_CheckIfUsrBelongsToCrs (UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod))
           {
            UsrDat.Accepted = Usr_GetIfUserHasAcceptedEnrollmentInCurrentCrs (UsrDat.UsrCod);

            fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\""
        	               " style=\"margin-bottom:10px;");
            if (Gbl.CurrentAct == ActPrnRecSevStd &&
                NumUsrs != 0 &&
                (NumUsrs % Gbl.Usrs.Listing.RecsPerPag) == 0)
               fprintf (Gbl.F.Out,"page-break-before:always;");
            fprintf (Gbl.F.Out,"\">");

            /* Common record */
            Rec_ShowSharedUsrRecord (TypeOfView,&UsrDat);

            /* Record of the student in the course */
            if (Gbl.CurrentCrs.Records.LstFields.Num)	// There are fields in the record
	      {
               ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == UsrDat.UsrCod);	// It's me
	       if (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER ||
		   Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||
		   (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT && ItsMe))	// I am student in this course and it's me
		  Rec_ShowCrsRecord (TypeOfView,&UsrDat);
	      }

            fprintf (Gbl.F.Out,"</div>");

            NumUsrs++;
           }
     }
   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);

   /***** Free list of fields of records *****/
   if (Gbl.Usrs.Listing.RecsUsrs == Rec_RECORD_USERS_STUDENTS)
      Rec_FreeListFields ();

   /***** Free memory used for by the list of users *****/
   Usr_FreeListsEncryptedUsrCods ();
  }

/*****************************************************************************/
/********** Get user's data and draw record of one unique teacher ************/
/*****************************************************************************/

void Rec_GetUsrAndShowRecordOneTchCrs (void)
  {
   /***** Get the selected teacher *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Show the record *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))	// Get from the database the data of the teacher
      if ((Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB =
	   Rol_GetRoleInCrs (Gbl.CurrentCrs.Crs.CrsCod,
	                     Gbl.Usrs.Other.UsrDat.UsrCod)) == Rol_TEACHER)
	 Rec_ShowRecordOneTchCrs ();
  }

/*****************************************************************************/
/******************** Draw record of one unique teacher **********************/
/*****************************************************************************/

static void Rec_ShowRecordOneTchCrs (void)
  {
   extern const char *Txt_TIMETABLE_TYPES[TT_NUM_TIMETABLE_TYPES];

   /***** Get if teacher has accepted enrollment in current course *****/
   Gbl.Usrs.Other.UsrDat.Accepted = Usr_GetIfUserHasAcceptedEnrollmentInCurrentCrs (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Asign users listing type depending on current action *****/
   Gbl.Usrs.Listing.RecsUsrs = Rec_RECORD_USERS_TEACHERS;

   /***** Show contextual menu *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

   /* Show office hours? */
   Rec_WriteFormShowOfficeHours (true,Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);

   /* Link to print view */
   Act_FormStart (ActPrnRecSevTch);
   Usr_PutHiddenParUsrCodAll (ActPrnRecSevTch,Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
   Par_PutHiddenParamChar ("ParamOfficeHours",'Y');
   Par_PutHiddenParamChar ("ShowOfficeHours",'Y');
   Rec_ShowLinkToPrintPreviewOfRecords ();
   Act_FormEnd ();

   fprintf (Gbl.F.Out,"</div>");

   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\""
	              " style=\"margin-bottom:10px;\">");

   /* Common record */
   Rec_ShowSharedUsrRecord (Rec_RECORD_LIST,&Gbl.Usrs.Other.UsrDat);

   /* Office hours */
   Lay_StartRoundFrame (NULL,Txt_TIMETABLE_TYPES[TT_TUTOR_TIMETABLE]);
   TT_ShowTimeTable (TT_TUTOR_TIMETABLE,Gbl.Usrs.Other.UsrDat.UsrCod);
   Lay_EndRoundFrame ();

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******************** Draw records of several teachers ***********************/
/*****************************************************************************/

void Rec_ListRecordsTchsCrs (void)
  {
   extern const char *Txt_You_must_select_one_ore_more_teachers;
   extern const char *Txt_TIMETABLE_TYPES[TT_NUM_TIMETABLE_TYPES];
   unsigned NumUsrs = 0;
   const char *Ptr;
   Rec_RecordViewType_t TypeOfView = (Gbl.CurrentAct == ActSeeRecSevTch) ? Rec_RECORD_LIST :
                                                                           Rec_RECORD_PRINT;
   struct UsrData UsrDat;
   bool ShowOfficeHours;

   /***** Asign users listing type depending on current action *****/
   Gbl.Usrs.Listing.RecsUsrs = Rec_RECORD_USERS_TEACHERS;

   /***** Get if I want to see teachers' office hours in teachers' records *****/
   ShowOfficeHours = Rec_GetParamShowOfficeHours ();

   /***** Get parameter with number of user records per page (only for printing) *****/
   if (Gbl.CurrentAct == ActPrnRecSevTch)
      Rec_GetParamRecordsPerPage ();

   /***** Get list of selected teachers *****/
   Usr_GetListSelectedUsrs ();

   /* Check the number of teachers to show */
   if (!Usr_CountNumUsrsInEncryptedList ())	// If no teachers selected...
     {						// ...write warning notice
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_select_one_ore_more_teachers);
      Usr_SeeTeachers ();			// ...show again the form
      return;
     }

   if (Gbl.CurrentAct == ActSeeRecSevTch)
     {
      /***** Show contextual menu *****/
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

      /* Show office hours? */
      Rec_WriteFormShowOfficeHours (ShowOfficeHours,Gbl.Usrs.Select.All);

      /* Link to print view */
      Act_FormStart (ActPrnRecSevTch);
      Usr_PutHiddenParUsrCodAll (ActPrnRecSevTch,Gbl.Usrs.Select.All);
      Par_PutHiddenParamChar ("ParamOfficeHours",'Y');
      Par_PutHiddenParamChar ("ShowOfficeHours",
                              ShowOfficeHours ? 'Y' :
                        	                'N');
      Rec_ShowLinkToPrintPreviewOfRecords ();
      Act_FormEnd ();

      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** List the records *****/
   Ptr = Gbl.Usrs.Select.All;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EncryptedUsrCod,Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))                // Get from the database the data of the student
         if (Usr_CheckIfUsrBelongsToCrs (UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod))
           {
            UsrDat.Accepted = Usr_GetIfUserHasAcceptedEnrollmentInCurrentCrs (UsrDat.UsrCod);

            fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\""
        	               " style=\"margin-bottom:10px;");
            if (Gbl.CurrentAct == ActPrnRecSevTch &&
                NumUsrs != 0 &&
                (NumUsrs % Gbl.Usrs.Listing.RecsPerPag) == 0)
               fprintf (Gbl.F.Out,"page-break-before:always;");
            fprintf (Gbl.F.Out,"\">");

            /* Common record */
            Rec_ShowSharedUsrRecord (TypeOfView,&UsrDat);

            /* Office hours */
            if (ShowOfficeHours)
              {
	       Lay_StartRoundFrame (NULL,Txt_TIMETABLE_TYPES[TT_TUTOR_TIMETABLE]);
	       TT_ShowTimeTable (TT_TUTOR_TIMETABLE,UsrDat.UsrCod);
	       Lay_EndRoundFrame ();
              }

            fprintf (Gbl.F.Out,"</div>");

            NumUsrs++;
           }
     }
   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);

   /***** Free memory used for by the list of users *****/
   Usr_FreeListsEncryptedUsrCods ();
  }

/*****************************************************************************/
/*************** Show a link to print preview of users' records **************/
/*****************************************************************************/

void Rec_ShowLinkToPrintPreviewOfRecords (void)
  {
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Print;
   extern const char *Txt_record_cards_per_page;
   unsigned i;

   Act_LinkFormSubmit (Txt_Print,The_ClassFormBold[Gbl.Prefs.Theme]);
   Lay_PutIconWithText ("print",Txt_Print,Txt_Print);
   fprintf (Gbl.F.Out,"<span class=\"%s\">(</span>"
	              "<select name=\"RecsPerPag\">",
	    The_ClassForm[Gbl.Prefs.Theme]);

   for (i = 1;
        i <= 10;
        i++)
     {
      fprintf (Gbl.F.Out,"<option");
      if (i == Gbl.Usrs.Listing.RecsPerPag)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%u</option>",i);
     }
   fprintf (Gbl.F.Out,"</select>"
	              "<span class=\"%s\"> %s)</span>",
            The_ClassForm[Gbl.Prefs.Theme],Txt_record_cards_per_page);
  }

/*****************************************************************************/
/** Get parameter with number of user records per page (only for printing) ***/
/*****************************************************************************/

static void Rec_GetParamRecordsPerPage (void)
  {
   char UnsignedStr[10+1];

   Par_GetParToText ("RecsPerPag",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Gbl.Usrs.Listing.RecsPerPag) != 1)
      Lay_ShowErrorAndExit ("Number of rows per page is missing.");
  }

/*****************************************************************************/
/************** Write a form to select whether show full tree ****************/
/*****************************************************************************/

static void Rec_WriteFormShowOfficeHours (bool ShowOfficeHours,const char *ListUsrCods)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Show_office_hours;

   /***** Start form *****/
   Act_FormStart (ActSeeRecSevTch);
   Usr_PutHiddenParUsrCodAll (ActSeeRecSevTch,ListUsrCods);
   Par_PutHiddenParamChar ("ParamOfficeHours",'Y');

   /***** End form *****/
   fprintf (Gbl.F.Out,"<div style=\"margin:0 5px; display:inline;\">"
                      "<input type=\"checkbox\" name=\"ShowOfficeHours\" value=\"Y\"");
   if (ShowOfficeHours)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   fprintf (Gbl.F.Out," class=\"LEFT_MIDDLE\""
	              " onclick=\"javascript:document.getElementById('%s').submit();\" />"
                      "<img src=\"%s/clock16x16.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICON16x16\" />"
                      "<span class=\"%s\">&nbsp;%s</span>"
                      "</div>",
            Gbl.FormId,
            Gbl.Prefs.IconsURL,
            Txt_Show_office_hours,
            Txt_Show_office_hours,
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Show_office_hours);
   Act_FormEnd ();
  }

/*****************************************************************************/
/********** Get parameter to show (or not) teachers' office hours ************/
/*****************************************************************************/
// Returns true if office hours must be shown

static bool Rec_GetParamShowOfficeHours (void)
  {
   char YN[1+1];

   Par_GetParToText ("ParamOfficeHours",YN,1);
   if (Str_ConvertToUpperLetter (YN[0]) == 'Y')
     {
      Par_GetParToText ("ShowOfficeHours",YN,1);
      return (Str_ConvertToUpperLetter (YN[0]) == 'Y');
     }
   return Rec_SHOW_OFFICE_HOURS_DEFAULT;
  }

/*****************************************************************************/
/*************** Update my record in the course and show it ******************/
/*****************************************************************************/

void Rec_UpdateAndShowMyCrsRecord (void)
  {
   /***** Get list of fields of records in current course *****/
   Rec_GetListRecordFieldsInCurrentCrs ();

   /***** Allocate memory for the texts of the fields *****/
   Rec_AllocMemFieldsRecordsCrs ();

   /***** Get data of the record from the form *****/
   Rec_GetFieldsCrsRecordFromForm ();

   /***** Update the record *****/
   Rec_UpdateCrsRecord (Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Show updated record *****/
   Rec_ShowMyCrsRecordUpdated ();

   /***** Free memory used for some fields *****/
   Rec_FreeMemFieldsRecordsCrs ();
  }

/*****************************************************************************/
/***** Update record in the course of another user (student) and show it *****/
/*****************************************************************************/

void Rec_UpdateAndShowOtherCrsRecord (void)
  {
   /***** Get the user's code of the student whose record we want to modify *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Get data of the student, because we need the name *****/
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat);

   /***** Get list of fields of records in current course *****/
   Rec_GetListRecordFieldsInCurrentCrs ();

   /***** Allocate memory for the texts of the fields *****/
   Rec_AllocMemFieldsRecordsCrs ();

   /***** Get data of the record from the form *****/
   Rec_GetFieldsCrsRecordFromForm ();

   /***** Update the record *****/
   Rec_UpdateCrsRecord (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show updated user's record *****/
   Rec_ShowOtherCrsRecordUpdated ();

   /***** Free memory used for some fields *****/
   Rec_FreeMemFieldsRecordsCrs ();
  }

/*****************************************************************************/
/**************************** Show record common *****************************/
/*****************************************************************************/
// Show form or only data depending on TypeOfView

void Rec_ShowCrsRecord (Rec_RecordViewType_t TypeOfView,struct UsrData *UsrDat)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_You_dont_have_permission_to_perform_this_action;
   extern const char *Txt_RECORD_FIELD_VISIBILITY_RECORD[Rec_NUM_TYPES_VISIBILITY];
   extern const char *Txt_Save;
   unsigned RecordWidth = Rec_RECORD_WIDTH_WIDE;
   char StrRecordWidth[10+1];
   unsigned FrameWidth = 10;
   unsigned Col1Width = 140;
   unsigned Col2Width;
   char *ClassHead;
   char *ClassData;
   bool ItsMe;
   bool DataForm = false;
   unsigned NumField;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row = NULL; // Initialized to avoid warning
   bool ShowField;
   bool ThisFieldHasText;
   bool ICanEdit;
   char Text[Cns_MAX_BYTES_TEXT+1];

   ClassHead = "HEAD_REC";
   ClassData = "DAT_REC";

   if (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT)	// I am a student
     {
      ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == UsrDat->UsrCod);	// It's me
      if (ItsMe)	// It's me
	 switch (TypeOfView)
	   {
	    case Rec_RECORD_LIST:
	       // When listing several records,
	       // only my record in course in allowed
	       // ==> show form to edit my record as student
 	       TypeOfView = Rec_FORM_MY_COURSE_RECORD_AS_STUDENT;
 	       break;
	    case Rec_FORM_MY_COURSE_RECORD_AS_STUDENT:
	    case Rec_CHECK_MY_COURSE_RECORD_AS_STUDENT:
	    case Rec_RECORD_PRINT:
	       break;
	    default:
	       Lay_ShowErrorAndExit (Txt_You_dont_have_permission_to_perform_this_action);
	       break;
	  }
      else	// It's not me ==> i am a student trying to do something forbidden
	 Lay_ShowErrorAndExit (Txt_You_dont_have_permission_to_perform_this_action);
     }

   switch (TypeOfView)
     {
      case Rec_FORM_MY_COURSE_RECORD_AS_STUDENT:
         for (NumField = 0;
              NumField < Gbl.CurrentCrs.Records.LstFields.Num;
              NumField++)
            if (Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Visibility == Rec_EDITABLE_FIELD)
              {
               DataForm = true;
  	       Act_FormStart (ActRcvRecCrs);
               break;
              }
	 FrameWidth = 10;
	 ClassHead = "HEAD_REC_SMALL";
	 ClassData = "DAT_REC";
	 break;
      case Rec_CHECK_MY_COURSE_RECORD_AS_STUDENT:
      case Rec_CHECK_OTHER_USR_COURSE_RECORD:
	 FrameWidth = 10;
	 ClassHead = "HEAD_REC_SMALL";
	 ClassData = "DAT_REC_SMALL_BOLD";
         break;
      case Rec_RECORD_LIST:
         DataForm = true;
	 Act_FormStart (ActRcvRecOthUsr);
         Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 FrameWidth = 10;
	 ClassHead = "HEAD_REC_SMALL";
	 ClassData = "DAT_REC_SMALL_BOLD";
	 break;
      case Rec_RECORD_PRINT:
	 FrameWidth = 1;
	 ClassHead = "HEAD_REC_SMALL";
	 ClassData = "DAT_REC_SMALL_BOLD";
         break;
      default:
         break;
    }

   Col2Width = RecordWidth - FrameWidth * 2 - Col1Width;

   /***** Start frame *****/
   sprintf (StrRecordWidth,"%upx",RecordWidth);
   Lay_StartRoundFrameTable (StrRecordWidth,2,NULL);

   /***** Header *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"LEFT_TOP\">"
	              "<table style=\"width:100%%;\">"
                      "<tr>"
                      "<td class=\"LEFT_MIDDLE\" style=\"width:%upx;\">",
            Rec_DEGREE_LOGO_SIZE);
   Log_DrawLogo (Sco_SCOPE_DEG,Gbl.CurrentDeg.Deg.DegCod,
                 Gbl.CurrentDeg.Deg.ShortName,Rec_DEGREE_LOGO_SIZE,NULL,true);
   fprintf (Gbl.F.Out,"</td>"
                      "<td class=\"%s CENTER_MIDDLE\">"
                      "%s<br />%s<br />%s"
                      "</td>"
                      "</tr>"
                      "</table>"
                      "</td>"
                      "</tr>",
            ClassHead,
            Gbl.CurrentDeg.Deg.FullName,Gbl.CurrentCrs.Crs.FullName,
            UsrDat->FullName);

   /***** Fields of the record that depends on the course *****/
   for (NumField = 0, Gbl.RowEvenOdd = 0;
	NumField < Gbl.CurrentCrs.Records.LstFields.Num;
	NumField++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
     {
      ShowField = !(TypeOfView == Rec_FORM_MY_COURSE_RECORD_AS_STUDENT ||
	            TypeOfView == Rec_CHECK_MY_COURSE_RECORD_AS_STUDENT) ||
                  Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Visibility != Rec_HIDDEN_FIELD;
      // If the field must be shown...
      if (ShowField)
        {
         ICanEdit = TypeOfView == Rec_RECORD_LIST ||
                     (TypeOfView == Rec_FORM_MY_COURSE_RECORD_AS_STUDENT &&
                      Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Visibility == Rec_EDITABLE_FIELD);

         /* Name of the field */
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"%s LEFT_TOP COLOR%u\""
                            " style=\"width:%upx;\">"
                            "%s:",
                  ICanEdit ? The_ClassForm[Gbl.Prefs.Theme] :
                	     "DAT_REC_SMALL",
                  Gbl.RowEvenOdd,Col1Width,
                  Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Name);
         if (TypeOfView == Rec_RECORD_LIST)
            fprintf (Gbl.F.Out,"<span class=\"DAT_SMALL\"> (%s)</span>",
                     Txt_RECORD_FIELD_VISIBILITY_RECORD[Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Visibility]);
         fprintf (Gbl.F.Out,"</td>");

         /***** Get the text of the field *****/
         if (Rec_GetFieldFromCrsRecord (UsrDat->UsrCod,Gbl.CurrentCrs.Records.LstFields.Lst[NumField].FieldCod,&mysql_res))
           {
            ThisFieldHasText = true;
            row = mysql_fetch_row (mysql_res);
           }
         else
            ThisFieldHasText = false;

         /***** Write form, text, or nothing depending on the user's role and the visibility of the field... */
         fprintf (Gbl.F.Out,"<td class=\"%s LEFT_TOP COLOR%u\""
                            " style=\"width:%upx;\">",
                  ClassData,Gbl.RowEvenOdd,Col2Width);
         if (ICanEdit)	// Show with form
           {
            fprintf (Gbl.F.Out,"<textarea name=\"Field%ld\" rows=\"%u\""
        	               " style=\"width:360px;\">",
                     Gbl.CurrentCrs.Records.LstFields.Lst[NumField].FieldCod,
                     Gbl.CurrentCrs.Records.LstFields.Lst[NumField].NumLines);
            if (ThisFieldHasText)
               fprintf (Gbl.F.Out,"%s",row[0]);
            fprintf (Gbl.F.Out,"</textarea>");
           }
         else		// Show without form
           {
            if (ThisFieldHasText)
              {
               strcpy (Text,row[0]);
               Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                                 Text,Cns_MAX_BYTES_TEXT,false);
               fprintf (Gbl.F.Out,"%s",Text);
              }
            else
               fprintf (Gbl.F.Out,"-");
           }
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");

         /***** Free structure that stores the query result *****/
         DB_FreeMySQLResult (&mysql_res);
        }
     }

   /***** Button to save changes and end frame *****/
   if (DataForm)
     {
      Lay_EndRoundFrameTableWithButton (Lay_CONFIRM_BUTTON,Txt_Save);
      Act_FormEnd ();
     }
   else
      Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/************** Get the text of a field of a record of course ****************/
/*****************************************************************************/

unsigned long Rec_GetFieldFromCrsRecord (long UsrCod,long FieldCod,MYSQL_RES **mysql_res)
  {
   char Query[512];

   /***** Get the text of a field of a record from database *****/
   sprintf (Query,"SELECT Txt FROM crs_records"
	          " WHERE FieldCod='%ld' AND UsrCod='%ld'",
            FieldCod,UsrCod);
   return DB_QuerySELECT (Query,mysql_res,"can not get the text of a field of a record.");
  }

/*****************************************************************************/
/****************** Get the fields of the record from form *******************/
/*****************************************************************************/

void Rec_GetFieldsCrsRecordFromForm (void)
  {
   unsigned NumField;
   char FieldParamName[5+10+1];

   for (NumField = 0;
	NumField < Gbl.CurrentCrs.Records.LstFields.Num;
	NumField++)
      if (Gbl.Usrs.Me.LoggedRole > Rol_STUDENT ||
          Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Visibility == Rec_EDITABLE_FIELD)
        {
         /* Get text of the form */
         sprintf (FieldParamName,"Field%ld",Gbl.CurrentCrs.Records.LstFields.Lst[NumField].FieldCod);
         Par_GetParToHTML (FieldParamName,Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Text,Cns_MAX_BYTES_TEXT);
        }
  }

/*****************************************************************************/
/*************************** Update record of a user *************************/
/*****************************************************************************/

void Rec_UpdateCrsRecord (long UsrCod)
  {
   unsigned NumField;
   char Query[256+Cns_MAX_BYTES_TEXT];
   MYSQL_RES *mysql_res;
   bool FieldAlreadyExists;

   for (NumField = 0;
	NumField < Gbl.CurrentCrs.Records.LstFields.Num;
	NumField++)
      if (Gbl.Usrs.Me.LoggedRole > Rol_STUDENT ||
          Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Visibility == Rec_EDITABLE_FIELD)
        {
         /***** Check if already exists this field for this user in database *****/
         FieldAlreadyExists = (Rec_GetFieldFromCrsRecord (UsrCod,Gbl.CurrentCrs.Records.LstFields.Lst[NumField].FieldCod,&mysql_res) != 0);
         DB_FreeMySQLResult (&mysql_res);
         if (FieldAlreadyExists)
           {
            if (Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Text[0])
              {
               /***** Update text of the field of record course *****/
               sprintf (Query,"UPDATE crs_records SET Txt='%s'"
        	              " WHERE UsrCod='%ld' AND FieldCod='%ld'",
                        Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Text,UsrCod,Gbl.CurrentCrs.Records.LstFields.Lst[NumField].FieldCod);
               DB_QueryUPDATE (Query,"can not update field of record");
              }
            else
              {
               /***** Remove text of the field of record course *****/
               sprintf (Query,"DELETE FROM crs_records"
        	              " WHERE UsrCod='%ld' AND FieldCod='%ld'",
                        UsrCod,Gbl.CurrentCrs.Records.LstFields.Lst[NumField].FieldCod);
               DB_QueryDELETE (Query,"can not remove field of record");
              }
           }
         else if (Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Text[0])
	   {
	    /***** Insert text field of record course *****/
	    sprintf (Query,"INSERT INTO crs_records (FieldCod,UsrCod,Txt)"
		           " VALUES ('%ld','%ld','%s')",
		     Gbl.CurrentCrs.Records.LstFields.Lst[NumField].FieldCod,
		     UsrCod,
		     Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Text);
	    DB_QueryINSERT (Query,"can not create field of record");
	   }
       }
  }

/*****************************************************************************/
/************ Remove fields of record of a user in current course ************/
/*****************************************************************************/

void Rec_RemoveFieldsCrsRecordInCrs (long UsrCod,struct Course *Crs,Cns_QuietOrVerbose_t QuietOrVerbose)
  {
   extern const char *Txt_User_record_card_in_the_course_X_has_been_removed;
   char Query[512];

   /***** Remove text of the field of record course *****/
   sprintf (Query,"DELETE FROM crs_records WHERE UsrCod='%ld' AND FieldCod IN"
                  " (SELECT FieldCod FROM crs_record_fields WHERE CrsCod='%ld')",
            UsrCod,Crs->CrsCod);
   DB_QueryDELETE (Query,"can not remove user's record in a course");

   /***** Write mensaje *****/
   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_User_record_card_in_the_course_X_has_been_removed,
               Crs->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
  }

/*****************************************************************************/
/************* Remove fields of record of a user in all courses **************/
/*****************************************************************************/

void Rec_RemoveFieldsCrsRecordAll (long UsrCod,Cns_QuietOrVerbose_t QuietOrVerbose)
  {
   extern const char *Txt_User_record_cards_in_all_courses_have_been_removed;
   char Query[128];

   /***** Remove text of the field of record course *****/
   sprintf (Query,"DELETE FROM crs_records WHERE UsrCod='%ld'",UsrCod);
   DB_QueryDELETE (Query,"can not remove user's records in all courses");

   /***** Write mensaje *****/
   if (QuietOrVerbose == Cns_VERBOSE)
      Lay_ShowAlert (Lay_SUCCESS,Txt_User_record_cards_in_all_courses_have_been_removed);
  }

/*****************************************************************************/
/*************** Show my record in the course already updated ****************/
/*****************************************************************************/

static void Rec_ShowMyCrsRecordUpdated (void)
  {
   extern const char *Txt_Your_record_card_in_this_course_has_been_updated;

   /***** Write mensaje of success *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_Your_record_card_in_this_course_has_been_updated);

   /***** Common record *****/
   Rec_ShowSharedUsrRecord (Rec_RECORD_LIST,&Gbl.Usrs.Me.UsrDat);

   /***** Show updated user's record *****/
   Rec_ShowCrsRecord (Rec_CHECK_MY_COURSE_RECORD_AS_STUDENT,&Gbl.Usrs.Me.UsrDat);
  }

/*****************************************************************************/
/**************** Show updated user's record in the course *******************/
/*****************************************************************************/

static void Rec_ShowOtherCrsRecordUpdated (void)
  {
   extern const char *Txt_Student_record_card_in_this_course_has_been_updated;

   /***** Write mensaje of success *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_Student_record_card_in_this_course_has_been_updated);

   /***** Common record *****/
   Rec_ShowSharedUsrRecord (Rec_RECORD_LIST,&Gbl.Usrs.Other.UsrDat);

   /***** Show updated user's record *****/
   Rec_ShowCrsRecord (Rec_CHECK_OTHER_USR_COURSE_RECORD,&Gbl.Usrs.Other.UsrDat);
  }

/*****************************************************************************/
/***** Allocate memory for the text of the field of the record in course *****/
/*****************************************************************************/

void Rec_AllocMemFieldsRecordsCrs (void)
  {
   unsigned NumField;

   for (NumField = 0;
	NumField < Gbl.CurrentCrs.Records.LstFields.Num;
	NumField++)
      if (Gbl.Usrs.Me.LoggedRole > Rol_STUDENT ||
          Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Visibility == Rec_EDITABLE_FIELD)
         /* Allocate memory for the texts of the fields */
         if ((Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Text = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
            Lay_ShowErrorAndExit ("Not enough memory to store records of the course.");
  }

/*****************************************************************************/
/**** Free memory used by the texts of the field of the record in course *****/
/*****************************************************************************/

void Rec_FreeMemFieldsRecordsCrs (void)
  {
   unsigned NumField;

   for (NumField = 0;
	NumField < Gbl.CurrentCrs.Records.LstFields.Num;
	NumField++)
      if (Gbl.Usrs.Me.LoggedRole > Rol_STUDENT ||
          Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Visibility == Rec_EDITABLE_FIELD)
         /* Free memory of the text of the field */
         if (Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Text)
           {
            free ((void *) Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Text);
            Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Text = NULL;
           }
  }

/*****************************************************************************/
/************ Show form to sign up and edit my common record *****************/
/*****************************************************************************/

void Rec_ShowFormSignUpWithMyCommonRecord (void)
  {
   extern const char *Txt_Sign_up;

   /***** Show the form *****/
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
   Act_FormStart (ActSignUp);
   Rec_ShowSharedUsrRecord (Rec_FORM_SIGN_UP,&Gbl.Usrs.Me.UsrDat);
   Lay_PutConfirmButton (Txt_Sign_up);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******************* Show form to edit my common record **********************/
/*****************************************************************************/

void Rec_ShowFormMyCommRecord (void)
  {
   extern const char *Txt_Please_fill_in_your_record_card_including_your_country_nationality;
   extern const char *Txt_Please_fill_in_your_record_card_including_your_sex;
   extern const char *Txt_Please_fill_in_your_record_card_including_your_name;

   /***** If user has no sex, name and surname... *****/
   if (Gbl.Usrs.Me.UsrDat.CtyCod < 0)
      Lay_ShowAlert (Lay_WARNING,Txt_Please_fill_in_your_record_card_including_your_country_nationality);
   else if (Gbl.Usrs.Me.UsrDat.Sex == Usr_SEX_UNKNOWN)
      Lay_ShowAlert (Lay_WARNING,Txt_Please_fill_in_your_record_card_including_your_sex);
   else if (!Gbl.Usrs.Me.UsrDat.FirstName[0] ||
	    !Gbl.Usrs.Me.UsrDat.Surname1[0])
      Lay_ShowAlert (Lay_WARNING,Txt_Please_fill_in_your_record_card_including_your_name);

   /***** Buttons for edition *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Rec_PutLinkToChangeMyInsCtrDpt ();			// Put link (form) to change my institution, centre, department...
   Rec_PutLinkToChangeMySocialNetworks ();		// Put link (form) to change my social networks
   Pho_PutLinkToChangeMyPhoto ();			// Put link (form) to change my photo
   Pri_PutLinkToChangeMyPrivacy ();			// Put link (form) to change my privacy
   fprintf (Gbl.F.Out,"</div>");

   /***** My record *****/
   Rec_ShowSharedUsrRecord (Rec_FORM_MY_COMMON_RECORD,&Gbl.Usrs.Me.UsrDat);
   Rec_WriteLinkToDataProtectionClause ();
  }

/*****************************************************************************/
/*************** Show form to edit the record of a new user ******************/
/*****************************************************************************/

void Rec_ShowFormOtherNewCommonRecord (struct UsrData *UsrDat)
  {
   /***** Show the form *****/
   Rec_ShowSharedUsrRecord (Rec_FORM_NEW_RECORD_OTHER_NEW_USR,UsrDat);
  }

/*****************************************************************************/
/*********************** Show my record after update *************************/
/*****************************************************************************/

void Rec_ShowMyCommonRecordUpd (void)
  {
   extern const char *Txt_Your_personal_data_have_been_updated;

   /***** Write alert *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_Your_personal_data_have_been_updated);

   /***** Show my record for checking *****/
   Rec_ShowSharedUsrRecord (Rec_MY_COMMON_RECORD_CHECK,&Gbl.Usrs.Me.UsrDat);
  }

/*****************************************************************************/
/********************** Show user's record for check *************************/
/*****************************************************************************/

void Rec_ShowCommonRecordUnmodifiable (struct UsrData *UsrDat)
  {
   /***** Get password, user type and user's data from database *****/
   Usr_GetAllUsrDataFromUsrCod (UsrDat);
   UsrDat->Accepted = Usr_GetIfUserHasAcceptedEnrollmentInCurrentCrs (UsrDat->UsrCod);

   /***** Show user's record *****/
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
   Rec_ShowSharedUsrRecord (Rec_CHECK_OTHER_USR_COMMON_RECORD,UsrDat);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/**************************** Show record common *****************************/
/*****************************************************************************/
// Show form or only data depending on TypeOfView

void Rec_ShowSharedUsrRecord (Rec_RecordViewType_t TypeOfView,
                              struct UsrData *UsrDat)
  {
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Edit_my_personal_data;
   extern const char *Txt_Edit;
   extern const char *Txt_View_record_for_this_course;
   extern const char *Txt_Admin_user;
   extern const char *Txt_ID;
   extern const char *Txt_Write_a_message;
   extern const char *Txt_View_works;
   extern const char *Txt_See_exams;
   extern const char *Txt_Attendance;
   extern const char *Txt_Unfollow;
   extern const char *Txt_Follow;
   extern const char *Txt_View_public_profile;
   extern const char *Txt_Country;
   extern const char *Txt_Email;
   extern const char *Txt_Sex;
   extern const char *Txt_Role;
   extern const char *Txt_SEX_SINGULAR_Abc[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Surname_1;
   extern const char *Txt_Surname_2;
   extern const char *Txt_First_name;
   extern const char *Txt_Country;
   extern const char *Txt_Another_country;
   extern const char *Txt_Place_of_origin;
   extern const char *Txt_Date_of_birth;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Department;
   extern const char *Txt_Office;
   extern const char *Txt_Phone;
   extern const char *Txt_Local_address;
   extern const char *Txt_Family_address;
   extern const char *Txt_USER_comments;
   extern const char *Txt_Save_changes;
   extern const char *Txt_Register;
   extern const char *Txt_Confirm;
   unsigned RecordWidth;
   unsigned TopC1Width;
   unsigned TopC2Width;
   unsigned TopC3Width;
   char StrRecordWidth[10+1];
   const char *ClassHead;
   const char *ClassForm;
   const char *ClassData;
   char PhotoURL[PATH_MAX+1];
   bool ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == UsrDat->UsrCod);
   bool IAmLoggedAsStudent = (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT);	// My current role is student
   bool IAmLoggedAsTeacher = (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER);	// My current role is teacher
   bool IAmLoggedAsSysAdm  = (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);	// My current role is superuser
   bool HeIsTeacherInAnyCourse = (UsrDat->Roles & (1 << Rol_TEACHER));	// He/she already is a teacher in any course
   bool HeBelongsToCurrentCrs = (UsrDat->RoleInCurrentCrsDB == Rol_STUDENT ||
	                         UsrDat->RoleInCurrentCrsDB == Rol_TEACHER);
   bool CountryForm = (TypeOfView == Rec_FORM_MY_COMMON_RECORD ||
                       TypeOfView == Rec_FORM_NEW_RECORD_OTHER_NEW_USR);
   bool RoleForm = (Gbl.CurrentCrs.Crs.CrsCod > 0 &&
	            (TypeOfView == Rec_FORM_SIGN_UP ||
                     TypeOfView == Rec_FORM_NEW_RECORD_OTHER_NEW_USR ||
                     TypeOfView == Rec_FORM_MODIFY_RECORD_OTHER_EXISTING_USR));
   bool SexForm = TypeOfView == Rec_FORM_MY_COMMON_RECORD;
   bool DataForm = (TypeOfView == Rec_FORM_MY_COMMON_RECORD ||
                    TypeOfView == Rec_FORM_NEW_RECORD_OTHER_NEW_USR ||
                    (TypeOfView == Rec_FORM_MODIFY_RECORD_OTHER_EXISTING_USR &&
                     Gbl.Usrs.Me.LoggedRole > Rol_TEACHER));
   bool PutFormLinks;	// Put links (forms) inside record card
   bool ShowEmail = (ItsMe ||
	             Gbl.Usrs.Me.LoggedRole >= Rol_DEG_ADM ||
	             DataForm ||
	             TypeOfView == Rec_FORM_MY_COMMON_RECORD  ||
		     TypeOfView == Rec_MY_COMMON_RECORD_CHECK ||
		     TypeOfView == Rec_FORM_MY_COURSE_RECORD_AS_STUDENT  ||
		     TypeOfView == Rec_CHECK_MY_COURSE_RECORD_AS_STUDENT ||
                     (UsrDat->Accepted &&
		      (TypeOfView == Rec_CHECK_OTHER_USR_COMMON_RECORD ||
		       ((TypeOfView == Rec_RECORD_LIST ||
			 TypeOfView == Rec_RECORD_PRINT) &&
		        (IAmLoggedAsTeacher || Gbl.Usrs.Listing.RecsUsrs == Rec_RECORD_USERS_TEACHERS)))));
   bool ShowID = (ItsMe ||
	          Gbl.Usrs.Me.LoggedRole >= Rol_DEG_ADM ||
	          DataForm ||
	          TypeOfView == Rec_FORM_MY_COMMON_RECORD  ||
		  TypeOfView == Rec_MY_COMMON_RECORD_CHECK ||
		  TypeOfView == Rec_FORM_MY_COURSE_RECORD_AS_STUDENT  ||
		  TypeOfView == Rec_CHECK_MY_COURSE_RECORD_AS_STUDENT ||
                  (UsrDat->Accepted &&
		   ((TypeOfView == Rec_CHECK_OTHER_USR_COMMON_RECORD &&
                     !(IAmLoggedAsTeacher && HeIsTeacherInAnyCourse)) ||	// A teacher can not see another teacher's ID
		    ((TypeOfView == Rec_RECORD_LIST ||
	              TypeOfView == Rec_RECORD_PRINT) &&
		     IAmLoggedAsTeacher && Gbl.Usrs.Listing.RecsUsrs == Rec_RECORD_USERS_STUDENTS))));
   bool ShowData = ItsMe ||
	           Gbl.Usrs.Me.LoggedRole >= Rol_DEG_ADM ||
	           UsrDat->Accepted;
   bool ShowIDRows = (TypeOfView != Rec_RECORD_PUBLIC);
   bool ShowAddressRows = (TypeOfView == Rec_FORM_MY_COMMON_RECORD  ||
                           TypeOfView == Rec_MY_COMMON_RECORD_CHECK ||
			   TypeOfView == Rec_FORM_MY_COURSE_RECORD_AS_STUDENT  ||
			   TypeOfView == Rec_CHECK_MY_COURSE_RECORD_AS_STUDENT ||
			   ((TypeOfView == Rec_RECORD_LIST          ||
			     TypeOfView == Rec_RECORD_PRINT) &&
			    (IAmLoggedAsTeacher || IAmLoggedAsSysAdm) &&
			    UsrDat->RoleInCurrentCrsDB == Rol_STUDENT));
   bool ShowTeacherRows = (((TypeOfView == Rec_FORM_MY_COMMON_RECORD  ||
			     TypeOfView == Rec_MY_COMMON_RECORD_CHECK ||
			     TypeOfView == Rec_FORM_MY_COURSE_RECORD_AS_STUDENT  ||
			     TypeOfView == Rec_CHECK_MY_COURSE_RECORD_AS_STUDENT) &&
			    (UsrDat->Roles & (1 << Rol_TEACHER))) ||	// He/she (me, really) is a teacher in any course
			   ((TypeOfView == Rec_RECORD_LIST ||
			     TypeOfView == Rec_RECORD_PRINT) &&
			    UsrDat->RoleInCurrentCrsDB == Rol_TEACHER));	// He/she is a teacher in the current course
   Usr_Sex_t Sex;
   Rol_Role_t Role;
   Rol_Role_t DefaultRoleInCurrentCrs;
   bool ShowPhoto;
   char CtyName[Cty_MAX_LENGTH_COUNTRY_NAME+1];
   unsigned NumCty;
   struct Institution Ins;
   struct Centre Ctr;
   struct Department Dpt;

   ClassHead = "HEAD_REC";
   ClassForm = The_ClassForm[Gbl.Prefs.Theme];
   ClassData = "DAT_REC";
   switch (TypeOfView)
     {
      case Rec_FORM_SIGN_UP:
      case Rec_FORM_MY_COMMON_RECORD:
      case Rec_FORM_MY_COURSE_RECORD_AS_STUDENT:
      case Rec_FORM_NEW_RECORD_OTHER_NEW_USR:
      case Rec_FORM_MODIFY_RECORD_OTHER_EXISTING_USR:
	 ClassHead = "HEAD_REC";
         ClassForm = The_ClassForm[Gbl.Prefs.Theme];
	 ClassData = "DAT_REC";
	 break;
      case Rec_MY_COMMON_RECORD_CHECK:
      case Rec_CHECK_MY_COURSE_RECORD_AS_STUDENT:
      case Rec_CHECK_OTHER_USR_COMMON_RECORD:
      case Rec_CHECK_OTHER_USR_COURSE_RECORD:
      case Rec_RECORD_LIST:
      case Rec_RECORD_PUBLIC:
      case Rec_RECORD_PRINT:
	 ClassHead = "HEAD_REC_SMALL";
	 ClassForm = "DAT_REC_SMALL";
	 ClassData = "DAT_REC_SMALL_BOLD";
         break;
     }

   switch (TypeOfView)
     {
      case Rec_RECORD_PUBLIC:
	 RecordWidth = Rec_RECORD_WIDTH_NARROW;
         TopC1Width = Rec_C1_TOP_NARROW;
         TopC2Width = Rec_C2_TOP_NARROW;
         TopC3Width = Rec_C3_TOP_NARROW;
	 break;
      default:
	 RecordWidth = Rec_RECORD_WIDTH_WIDE;
         TopC1Width = Rec_C1_TOP_WIDE;
         TopC2Width = Rec_C2_TOP_WIDE;
         TopC3Width = Rec_C3_TOP_WIDE;
	 break;
    }

   PutFormLinks = !Gbl.InsideForm &&						// Only if not inside another form
                  Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW;	// Only in main window

   /***** Start frame *****/
   sprintf (StrRecordWidth,"%upx",RecordWidth);
   Lay_StartRoundFrameTable (StrRecordWidth,2,NULL);

   /***** Institution *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"CENTER_MIDDLE\""
                      " style=\"width:%upx; height:%upx;\">",
	    TopC1Width,TopC1Width);
   if (UsrDat->InsCod > 0)
     {
      Ins.InsCod = UsrDat->InsCod;
      Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);

      /* Form to go to the institution */
      if (PutFormLinks)
	{
	 Act_FormGoToStart (ActSeeInsInf);
	 Ins_PutParamInsCod (Ins.InsCod);
	 Act_LinkFormSubmit (Ins.FullName,NULL);
	}
      Log_DrawLogo (Sco_SCOPE_INS,Ins.InsCod,Ins.ShortName,
                    Rec_INSTITUTION_LOGO_SIZE,NULL,true);
      if (PutFormLinks)
	{
         fprintf (Gbl.F.Out,"</a>");
	 Act_FormEnd ();
	}
     }
   fprintf (Gbl.F.Out,"</td>"
		      "<td class=\"%s LEFT_MIDDLE\""
		      " style=\"width:%upx; height:%upx;\">",
	    ClassHead,TopC2Width,TopC1Width);
   if (UsrDat->InsCod > 0)
     {
      /* Form to go to the institution */
      if (PutFormLinks)
	{
	 Act_FormGoToStart (ActSeeInsInf);
	 Ins_PutParamInsCod (Ins.InsCod);
	 Act_LinkFormSubmit (Ins.FullName,ClassHead);
	}
      fprintf (Gbl.F.Out,"%s",Ins.FullName);
      if (PutFormLinks)
	{
         fprintf (Gbl.F.Out,"</a>");
	 Act_FormEnd ();
	}
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Photo *****/
   ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
   fprintf (Gbl.F.Out,"<td rowspan=\"3\" class=\"CENTER_TOP\""
	              " style=\"width:%upx;\">",
	    TopC3Width);
   Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                	                NULL,
		     "PHOTO150x200",Pho_NO_ZOOM);
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /***** Commands *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td rowspan=\"3\" class=\"LEFT_TOP\""
	              " style=\"width:%upx;\">",
	    TopC1Width);

   if (PutFormLinks && Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<div style=\"width:20px; margin:6px auto;\">");

      /***** Button to edit my record card *****/
      if (ItsMe)
        {
	 Act_FormStart (ActReqEdiRecCom);
	 Act_LinkFormSubmit (Txt_Edit_my_personal_data,NULL);
	 fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\""
	                    " style=\"display:inline;\" >"
			    "<img src=\"%s/edit16x16.gif\""
			    " alt=\"%s\" title=\"%s\""
			    " style=\"width:16px; height:16px;"
			    " padding:0 2px;\" />"
			    "</div>"
			    "</a>",
		  Gbl.Prefs.IconsURL,
		  Txt_Edit,
		  Txt_Edit);
	 Act_FormEnd ();
        }

      /***** Button to view user's record card in course when:
             - a course is selected && the user belongs to it &&
             - I can view user's record card in course *****/
      if (HeBelongsToCurrentCrs &&
	  (IAmLoggedAsStudent ||
	   IAmLoggedAsTeacher ||
	   IAmLoggedAsSysAdm))
	{
	 Act_FormStart ((UsrDat->RoleInCurrentCrsDB == Rol_STUDENT) ? ActSeeRecOneStd :
									   ActSeeRecOneTch);
	 Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 Act_LinkFormSubmit (Txt_View_record_for_this_course,NULL);
	 fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\""
	                    " style=\"display:inline;\" >"
			    "<img src=\"%s/card16x16.gif\""
			    " alt=\"%s\" title=\"%s\""
			    " style=\"width:16px; height:16px;"
			    " padding:0 2px;\" />"
			    "</div>"
			    "</a>",
		  Gbl.Prefs.IconsURL,
		  Txt_View_record_for_this_course,
		  Txt_View_record_for_this_course);
	 Act_FormEnd ();
	}

      /***** Button to admin user *****/
      if (ItsMe ||
	  (Gbl.CurrentCrs.Crs.CrsCod > 0 && Gbl.Usrs.Me.LoggedRole == Rol_TEACHER) ||
	  (Gbl.CurrentDeg.Deg.DegCod > 0 && Gbl.Usrs.Me.LoggedRole == Rol_DEG_ADM) ||
	  (Gbl.CurrentCtr.Ctr.CtrCod > 0 && Gbl.Usrs.Me.LoggedRole == Rol_CTR_ADM) ||
	  (Gbl.CurrentIns.Ins.InsCod > 0 && Gbl.Usrs.Me.LoggedRole == Rol_INS_ADM) ||
	  Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
	{
	 Act_FormStart (ActReqMdfUsr);
	 Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 Act_LinkFormSubmit (Txt_Admin_user,NULL);
	 fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\""
	                    " style=\"display:inline;\" >"
			    "<img src=\"%s/config16x16.gif\""
			    " alt=\"%s\" title=\"%s\""
			    " style=\"width:16px; height:16px;"
			    " padding:0 2px;\" />"
			    "</div>"
			    "</a>",
		  Gbl.Prefs.IconsURL,
		  Txt_Admin_user,
		  Txt_Admin_user);
	 Act_FormEnd ();
	}

      if (Gbl.CurrentCrs.Crs.CrsCod > 0 &&			// A course is selected
	  UsrDat->RoleInCurrentCrsDB == Rol_STUDENT &&	// He/she is a student in the current course
	  (ItsMe || IAmLoggedAsTeacher || IAmLoggedAsSysAdm))		// I can view
	{
	 /***** Button to view user's assignments and works *****/
	 if (ItsMe)	// I am a student
	    Act_FormStart (ActAdmAsgWrkUsr);
	 else		// I am a teacher or superuser
	   {
	    Act_FormStart (ActAdmAsgWrkCrs);
	    Par_PutHiddenParamString ("UsrCodStd",UsrDat->EncryptedUsrCod);
	   }
	 Grp_PutParamAllGroups ();
	 Par_PutHiddenParamChar ("FullTree",'Y');	// By default, show all files
	 Act_LinkFormSubmit (Txt_View_works,ClassData);
	 fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\""
	                    " style=\"display:inline;\" >"
			    "<img src=\"%s/folder16x16.gif\""
			    " alt=\"%s\" title=\"%s\""
			    " style=\"width:16px; height:16px;"
			    " padding:0 2px;\" />"
			    "</div>"
			    "</a>",
		  Gbl.Prefs.IconsURL,
		  Txt_View_works,
		  Txt_View_works);
	 Act_FormEnd ();

	 /***** Button to view user's test exams *****/
	 if (ItsMe)
	    Act_FormStart (ActSeeMyTstExa);
	 else
	   {
	    Act_FormStart (ActSeeUsrTstExa);
	    Par_PutHiddenParamString ("UsrCodStd",UsrDat->EncryptedUsrCod);
	   }
	 Grp_PutParamAllGroups ();
	 Act_LinkFormSubmit (Txt_See_exams,ClassData);
	 fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\""
	                    " style=\"display:inline;\" >"
			    "<img src=\"%s/file16x16.gif\""
			    " alt=\"%s\" title=\"%s\""
			    " style=\"width:16px; height:16px;"
			    " padding:0 2px;\" />"
			    "</div>"
			    "</a>",
		  Gbl.Prefs.IconsURL,
		  Txt_See_exams,
		  Txt_See_exams);
	 Act_FormEnd ();

	 /***** Button to view user's attendance *****/
	 // TODO: A student should see her/his attendance
	 if (IAmLoggedAsTeacher || IAmLoggedAsSysAdm)
	   {
	    Act_FormStart (ActSeeLstAttStd);
	    Par_PutHiddenParamString ("UsrCodStd",UsrDat->EncryptedUsrCod);
	    Grp_PutParamAllGroups ();
	    Act_LinkFormSubmit (Txt_Attendance,ClassData);
	    fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\""
		               " style=\"display:inline;\" >"
			       "<img src=\"%s/rollcall16x16.gif\""
			       " alt=\"%s\" title=\"%s\""
			       " style=\"width:16px; height:16px;"
			       " padding:0 2px;\" />"
			       "</div>"
			       "</a>",
		     Gbl.Prefs.IconsURL,
		     Txt_Attendance,
		     Txt_Attendance);
	    Act_FormEnd ();
	   }
	}

      /***** Button to send a message *****/
      Act_FormStart (ActReqMsgUsr);
      Grp_PutParamAllGroups ();
      if (HeBelongsToCurrentCrs)
         Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      else
	 Msg_PutHiddenParamAnotherRecipient (UsrDat);
      Act_LinkFormSubmit (Txt_Write_a_message,ClassData);
      fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\""
	                 " style=\"display:inline;\" >"
			 "<img src=\"%s/msg16x16.gif\""
			 " alt=\"%s\" title=\"%s\""
			 " style=\"width:16px; height:16px;"
			 " padding:0 2px;\" />"
			 "</div>"
			 "</a>",
	       Gbl.Prefs.IconsURL,
	       Txt_Write_a_message,
	       Txt_Write_a_message);
      Act_FormEnd ();

      /***** Button to follow / unfollow *****/
      if (TypeOfView == Rec_RECORD_PUBLIC &&
	  !ItsMe)
	{
	 if (Fol_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,UsrDat->UsrCod))
	   {
	    Act_FormStart (ActUnfUsr);
	    Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	    Act_LinkFormSubmit (Txt_Unfollow,ClassData);
	    fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\""
		               " style=\"display:inline;\" >"
			       "<img src=\"%s/unfollow16x16.gif\""
			       " alt=\"%s\" title=\"%s\""
			       " style=\"width:16px; height:16px;"
			       " padding:0 2px;\" />"
			       "</div>"
			       "</a>",
		     Gbl.Prefs.IconsURL,
		     Txt_Unfollow,
		     Txt_Unfollow);
	    Act_FormEnd ();
	   }
	 else
	   {
	    Act_FormStart (ActFolUsr);
	    Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	    Act_LinkFormSubmit (Txt_Follow,ClassData);
	    fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\""
		               " style=\"display:inline;\" >"
			       "<img src=\"%s/follow16x16.gif\""
			       " alt=\"%s\" title=\"%s\""
			       " style=\"width:16px; height:16px;"
			       " padding:0 2px;\" />"
			       "</div>"
			       "</a>",
		     Gbl.Prefs.IconsURL,
		     Txt_Follow,
		     Txt_Follow);
	    Act_FormEnd ();
	   }
	}

      fprintf (Gbl.F.Out,"</div>");
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Full name *****/
   fprintf (Gbl.F.Out,"<td class=\"REC_NAME LEFT_TOP\""
	              " style=\"width:%upx;\">"
	              "%s<br />%s<br />%s"
	              "</td>"
	              "</tr>",
	    TopC2Width,
	    UsrDat->FirstName ,
	    UsrDat->Surname1,
	    UsrDat->Surname2);

   /***** User's nickname *****/
   fprintf (Gbl.F.Out,"<td class=\"REC_NAME LEFT_BOTTOM\""
	              " style=\"width:%upx;\">"
	              "<div class=\"REC_NICK\">",
	    TopC2Width);
   if (UsrDat->Nickname[0])
     {
      if (PutFormLinks)
	{
	 /* Put form to go to public profile */
	 Act_FormStart (ActSeePubPrf);
         Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 Act_LinkFormSubmit (Txt_View_public_profile,"REC_NICK");
	}
      fprintf (Gbl.F.Out,"@%s",UsrDat->Nickname);
      if (PutFormLinks)
	{
	 fprintf (Gbl.F.Out,"</a>");
	 Act_FormEnd ();

         /* Link to QR code */
	 QR_PutLinkToPrintQRCode (UsrDat,false);
	}
     }
   fprintf (Gbl.F.Out,"</div>"
	              "</td>"
	              "</tr>");

   /***** Country *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s LEFT_TOP\" style=\"width:%upx;\">",
	    ClassData,TopC2Width);
   if (ShowData && UsrDat->CtyCod > 0)
     {
      Cty_GetCountryName (UsrDat->CtyCod,CtyName);
      fprintf (Gbl.F.Out,"%s",CtyName);
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** User's web and social networks *****/
   fprintf (Gbl.F.Out,"<td style=\"width:%upx;\">"
	              "<div class=\"CENTER_TOP\" style=\"margin:0 auto;\">",
	    TopC3Width);
   Net_ShowWebsAndSocialNets (UsrDat);
   fprintf (Gbl.F.Out,"</div>"
	              "</td>"
		      "</tr>");

   if (ShowIDRows ||
       ShowAddressRows ||
       ShowTeacherRows)
     {
      fprintf (Gbl.F.Out,"<tr>"
			 "<td colspan=\"3\">");

      switch (TypeOfView)
        {
	 case Rec_FORM_MY_COMMON_RECORD:
	    Act_FormStart (ActChgMyData);
            break;
	 case Rec_FORM_NEW_RECORD_OTHER_NEW_USR:
	    Act_FormStart (ActCreOthUsrDat);
	    ID_PutParamOtherUsrIDPlain ();				// New user
	    break;
         case Rec_FORM_MODIFY_RECORD_OTHER_EXISTING_USR:
	    Act_FormStart (ActUpdOthUsrDat);
	    Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);	// Existing user
	    break;
         default:
            break;
        }

      fprintf (Gbl.F.Out,"<table style=\"width:100%%\">");

      if (ShowIDRows)
	{
	 /***** User's e-mail *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Email,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (UsrDat->Email[0])
	   {
	    if (ShowEmail)
	      {
	       fprintf (Gbl.F.Out,"<a href=\"mailto:%s\"",
			UsrDat->Email);
	       Str_LimitLengthHTMLStr (UsrDat->Email,36);
	       fprintf (Gbl.F.Out," class=\"%s\">%s</a>",
			ClassData,UsrDat->Email);
	      }
	    else
	       fprintf (Gbl.F.Out,"********");
	   }
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /***** User's ID *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_TOP\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_TOP\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_ID,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 ID_WriteUsrIDs (UsrDat,ShowID);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /***** User's role or sex *****/
	 if (RoleForm)
	   {
	    fprintf (Gbl.F.Out,"<tr>"
			       "<td class=\"%s RIGHT_MIDDLE\""
			       " style=\"width:%upx;\">"
			       "%s:</td>"
			       "<td class=\"%s LEFT_MIDDLE\""
			       " style=\"width:%upx;\">",
		     ClassForm,Rec_C1_BOTTOM_WIDE,
		     Txt_Role,
		     ClassData,Rec_C2_BOTTOM_WIDE);
	    switch (TypeOfView)
	      {
	       case Rec_FORM_SIGN_UP:			// I want to apply for enrollment
		  DefaultRoleInCurrentCrs = (UsrDat->Roles & (1 << Rol_TEACHER)) ? Rol_TEACHER :
											Rol_STUDENT;
		  fprintf (Gbl.F.Out,"<select name=\"Role\">");
		  for (Role = Rol_STUDENT;
		       Role <= Rol_TEACHER;
		       Role++)
		    {
		     fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Role);
		     if (Role == DefaultRoleInCurrentCrs)
			fprintf (Gbl.F.Out," selected=\"selected\"");
		     fprintf (Gbl.F.Out,">%s</option>",
			      Txt_ROLES_SINGUL_Abc[Role][UsrDat->Sex]);
		    }
		  fprintf (Gbl.F.Out,"</select>");
		  break;
	       case Rec_FORM_MODIFY_RECORD_OTHER_EXISTING_USR:	// The other user already exists in the platform
		  if (UsrDat->RoleInCurrentCrsDB < Rol_STUDENT)	// The other user does not belong to current course
		    {
		     /* If there is a request of this user, default role is the requested role */
		     if ((DefaultRoleInCurrentCrs = Rol_GetRequestedRole (UsrDat->UsrCod)) == Rol_UNKNOWN)
			DefaultRoleInCurrentCrs = (UsrDat->Roles & (1 << Rol_TEACHER)) ? Rol_TEACHER :
											      Rol_STUDENT;
		    }
		  else
		     DefaultRoleInCurrentCrs = UsrDat->RoleInCurrentCrsDB;

		  fprintf (Gbl.F.Out,"<select name=\"Role\">");
		  switch (Gbl.Usrs.Me.LoggedRole)
		    {
		     case Rol__GUEST_:
		     case Rol_VISITOR:
		     case Rol_STUDENT:
			fprintf (Gbl.F.Out,"<option value=\"%u\" selected=\"selected\" disabled=\"disabled\">%s</option>",
				 (unsigned) Gbl.Usrs.Me.LoggedRole,
				 Txt_ROLES_SINGUL_Abc[Gbl.Usrs.Me.LoggedRole][UsrDat->Sex]);
			break;
		     case Rol_TEACHER:
			for (Role = Rol_STUDENT;
			     Role <= Rol_TEACHER;
			     Role++)
			   if (Role == Rol_STUDENT ||
			       (UsrDat->Roles & (1 << Role)))	// A teacher can not upgrade a student (in all other courses) to teacher
			     {
			      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Role);
			      if (Role == DefaultRoleInCurrentCrs)
				 fprintf (Gbl.F.Out," selected=\"selected\"");
			      fprintf (Gbl.F.Out,">%s</option>",
				       Txt_ROLES_SINGUL_Abc[Role][UsrDat->Sex]);
			     }
			break;
		     case Rol_DEG_ADM:
		     case Rol_SYS_ADM:
			for (Role = Rol_STUDENT;
			     Role <= Rol_TEACHER;
			     Role++)
			  {
			   fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Role);
			   if (Role == DefaultRoleInCurrentCrs)
			      fprintf (Gbl.F.Out," selected=\"selected\"");
			   fprintf (Gbl.F.Out,">%s</option>",
				    Txt_ROLES_SINGUL_Abc[Role][UsrDat->Sex]);
			  }
			break;
		     default: // The rest of users can not register other users
			break;
		    }
		  fprintf (Gbl.F.Out,"</select>");
		  break;
	       case Rec_FORM_NEW_RECORD_OTHER_NEW_USR:	// The other user does not exist in platform
		  fprintf (Gbl.F.Out,"<select name=\"Role\">");
		  switch (Gbl.Usrs.Me.LoggedRole)
		    {
		     case Rol_TEACHER:	// A teacher only can create students
			fprintf (Gbl.F.Out,"<option value=\"%u\" selected=\"selected\">%s</option>",
				 (unsigned) Rol_STUDENT,Txt_ROLES_SINGUL_Abc[Rol_STUDENT][Usr_SEX_UNKNOWN]);
			break;
		     case Rol_DEG_ADM:	// An administrator or a superuser can create students and teachers
		     case Rol_SYS_ADM:
			fprintf (Gbl.F.Out,"<option value=\"%u\" selected=\"selected\">%s</option>"
					   "<option value=\"%u\">%s</option>",
				 (unsigned) Rol_STUDENT,Txt_ROLES_SINGUL_Abc[Rol_STUDENT][Usr_SEX_UNKNOWN],
				 (unsigned) Rol_TEACHER,Txt_ROLES_SINGUL_Abc[Rol_TEACHER][Usr_SEX_UNKNOWN]);
			break;
		     default:	// The rest of users can not register other users
			break;
		    }
		  fprintf (Gbl.F.Out,"</select>");
		  break;
	       default:
		  break;
	      }
	    fprintf (Gbl.F.Out,"</td>"
			       "</tr>");
	   }
	 else if (SexForm)
	   {
	    fprintf (Gbl.F.Out,"<tr>"
			       "<td class=\"%s RIGHT_MIDDLE\""
			       " style=\"width:%upx;\">"
			       "%s*:</td>"
			       "<td class=\"%s LEFT_MIDDLE\""
			       " style=\"width:%upx;\">",
		     ClassForm,Rec_C1_BOTTOM_WIDE,
		     Txt_Sex,
		     ClassData,Rec_C2_BOTTOM_WIDE);
	    for (Sex = Usr_SEX_FEMALE;
		 Sex <= Usr_SEX_MALE;
		 Sex++)
	      {
	       fprintf (Gbl.F.Out,"<input type=\"radio\" name=\"Sex\" value=\"%u\"",(unsigned) Sex);
	       if (Sex == Gbl.Usrs.Me.UsrDat.Sex)
		  fprintf (Gbl.F.Out," checked=\"checked\"");
	       fprintf (Gbl.F.Out," />"
				  "<img src=\"%s/%s16x16.gif\""
				  " alt=\"%s\" title=\"%s\""
				  " class=\"ICON16x16\" />"
				  "%s",
			Gbl.Prefs.IconsURL,Usr_StringsSexDB[Sex],
			Txt_SEX_SINGULAR_Abc[Sex],
			Txt_SEX_SINGULAR_Abc[Sex],
			Txt_SEX_SINGULAR_Abc[Sex]);
	      }
	    fprintf (Gbl.F.Out,"</td>"
			       "</tr>");
	   }
	 else	// RoleForm == false, SexForm == false
	    fprintf (Gbl.F.Out,"<tr>"
			       "<td class=\"%s RIGHT_MIDDLE\""
			       " style=\"width:%upx;\">"
			       "%s:"
			       "</td>"
			       "<td class=\"%s LEFT_MIDDLE\""
			       " style=\"width:%upx;\">"
			       "%s"
			       "</td>"
			       "</tr>",
		     ClassForm,Rec_C1_BOTTOM_WIDE,
		     TypeOfView == Rec_MY_COMMON_RECORD_CHECK ? Txt_Sex :
								Txt_Role,
		     ClassData,Rec_C2_BOTTOM_WIDE,
		     TypeOfView == Rec_MY_COMMON_RECORD_CHECK ? Txt_SEX_SINGULAR_Abc[UsrDat->Sex] :
								Txt_ROLES_SINGUL_Abc[UsrDat->RoleInCurrentCrsDB][UsrDat->Sex]);

	 /***** Name *****/
	 /* Surname 1 */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Surname_1);
	 if (TypeOfView == Rec_FORM_MY_COMMON_RECORD)
	    fprintf (Gbl.F.Out,"*");
	 fprintf (Gbl.F.Out,":</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (DataForm)
	    fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Surname1\""
			       " style=\"width:%upx;\" maxlength=\"%u\" value=\"%s\" />",
		     Rec_C2_BOTTOM_WIDE - 60,
		     Usr_MAX_LENGTH_USR_NAME_OR_SURNAME,
		     UsrDat->Surname1);
	 else if (UsrDat->Surname1[0])
	    fprintf (Gbl.F.Out,"<strong>%s</strong>",UsrDat->Surname1);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Surname 2 */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,
		  Txt_Surname_2,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (DataForm)
	    fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Surname2\""
			       " style=\"width:%upx;\" maxlength=\"%u\" value=\"%s\" />",
		     Rec_C2_BOTTOM_WIDE - 60,
		     Usr_MAX_LENGTH_USR_NAME_OR_SURNAME,
		     UsrDat->Surname2);
	 else if (UsrDat->Surname2[0])
	    fprintf (Gbl.F.Out,"<strong>%s</strong>",
		     UsrDat->Surname2);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* First name */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_First_name);
	 if (TypeOfView == Rec_FORM_MY_COMMON_RECORD)
	    fprintf (Gbl.F.Out,"*");
	 fprintf (Gbl.F.Out,":</td>"
			    "<td colspan=\"2\" class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (DataForm)
	    fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FirstName\""
			       " style=\"width:%upx;\" maxlength=\"%u\" value=\"%s\" />",
		     Rec_C2_BOTTOM_WIDE - 60,
		     Usr_MAX_LENGTH_USR_NAME_OR_SURNAME,
		     UsrDat->FirstName);
	 else if (UsrDat->FirstName[0])
	    fprintf (Gbl.F.Out,"<strong>%s</strong>",UsrDat->FirstName);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Country */
	 if (CountryForm)
	   {
	    /* If list of countries is empty, try to get it */
	    if (!Gbl.Ctys.Num)
	      {
	       Gbl.Ctys.SelectedOrderType = Cty_ORDER_BY_COUNTRY;
	       Cty_GetListCountries (Cty_GET_ONLY_COUNTRIES);
	      }

	    fprintf (Gbl.F.Out,"<tr>"
			       "<td class=\"%s RIGHT_MIDDLE\""
			       " style=\"width:%upx;\">"
			       "%s",
		     ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Country);
	    if (TypeOfView == Rec_FORM_MY_COMMON_RECORD)
	       fprintf (Gbl.F.Out,"*");
	    fprintf (Gbl.F.Out,":</td>"
			       "<td colspan=\"2\" class=\"%s LEFT_MIDDLE\""
			       " style=\"width:%upx;\">",
		     ClassData,Rec_C2_BOTTOM_WIDE);

	    /* Selector of country */
	    fprintf (Gbl.F.Out,"<select name=\"OthCtyCod\" style=\"width:%upx;\">"
			       "<option value=\"-1\">%s</option>"
			       "<option value=\"0\"",
		     Rec_C2_BOTTOM_WIDE - 60,
		     Txt_Country);
	    if (UsrDat->CtyCod == 0)
	       fprintf (Gbl.F.Out," selected=\"selected\"");
	    fprintf (Gbl.F.Out,">%s</option>",Txt_Another_country);
	    for (NumCty = 0;
		 NumCty < Gbl.Ctys.Num;
		 NumCty++)
	      {
	       fprintf (Gbl.F.Out,"<option value=\"%ld\"",
			Gbl.Ctys.Lst[NumCty].CtyCod);
	       if (Gbl.Ctys.Lst[NumCty].CtyCod == UsrDat->CtyCod)
		  fprintf (Gbl.F.Out," selected=\"selected\"");
	       fprintf (Gbl.F.Out,">%s</option>",
			Gbl.Ctys.Lst[NumCty].Name[Gbl.Prefs.Language]);
	      }
	    fprintf (Gbl.F.Out,"</select>");

	    fprintf (Gbl.F.Out,"</td>"
			       "</tr>");
	   }
	}

      if (ShowAddressRows)
	{
	 /* Origin place */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Place_of_origin,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (ShowData)
	   {
	    if (DataForm)
	       fprintf (Gbl.F.Out,"<input type=\"text\" name=\"OriginPlace\""
				  " style=\"width:%upx;\" maxlength=\"%u\" value=\"%s\" />",
			Rec_C2_BOTTOM_WIDE - 60,
			Cns_MAX_LENGTH_STRING,
			UsrDat->OriginPlace);
	    else if (UsrDat->OriginPlace[0])
	       fprintf (Gbl.F.Out,"%s",UsrDat->OriginPlace);
	   }
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Date of birth */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Date_of_birth,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (ShowData)
	   {
	    if (DataForm)
	       Dat_WriteFormDate (Gbl.Now.Date.Year - 99,
				  Gbl.Now.Date.Year - 16,
				  "DiaNac","MesNac","AnoNac",
				  &(UsrDat->Birthday),
				  false,false);
	    else if (UsrDat->StrBirthday[0])
	       fprintf (Gbl.F.Out,"%s",UsrDat->StrBirthday);
	   }
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Local address */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Local_address,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (ShowData)
	   {
	    if (DataForm)
	       fprintf (Gbl.F.Out,"<input type=\"text\" name=\"LocalAddress\""
				  " style=\"width:%upx;\" maxlength=\"%u\" value=\"%s\" />",
			Rec_C2_BOTTOM_WIDE - 60,
			Cns_MAX_LENGTH_STRING,
			UsrDat->LocalAddress);
	    else if (UsrDat->LocalAddress[0])
	       fprintf (Gbl.F.Out,"%s",UsrDat->LocalAddress);
	   }
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Local phone */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Phone,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (ShowData)
	   {
	    if (DataForm)
	       fprintf (Gbl.F.Out,"<input type=\"text\" name=\"LocalPhone\""
				  " style=\"width:%upx;\" maxlength=\"%u\" value=\"%s\" />",
			Rec_C2_BOTTOM_WIDE - 60,
			Usr_MAX_LENGTH_PHONE,
			UsrDat->LocalPhone);
	    else if (UsrDat->LocalPhone[0])
	       fprintf (Gbl.F.Out,"%s",UsrDat->LocalPhone);
	   }
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Family address */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Family_address,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (ShowData)
	   {
	    if (DataForm)
	       fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FamilyAddress\""
				  " style=\"width:%upx;\" maxlength=\"%u\" value=\"%s\" />",
			Rec_C2_BOTTOM_WIDE - 60,
			Cns_MAX_LENGTH_STRING,
			UsrDat->FamilyAddress);
	    else if (UsrDat->FamilyAddress[0])
	       fprintf (Gbl.F.Out,"%s",UsrDat->FamilyAddress);
	   }
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Family phone */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Phone,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (ShowData)
	   {
	    if (DataForm)
	       fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FamilyPhone\""
				  " style=\"width:%upx;\" maxlength=\"%u\" value=\"%s\" />",
			Rec_C2_BOTTOM_WIDE - 60,
			Usr_MAX_LENGTH_PHONE,
			UsrDat->FamilyPhone);
	    else if (UsrDat->FamilyPhone[0])
	       fprintf (Gbl.F.Out,"%s",UsrDat->FamilyPhone);
	   }
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Common comments for all the courses */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_TOP\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_TOP\" style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_USER_comments,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (ShowData)
	   {
	    if (DataForm)
	       fprintf (Gbl.F.Out,"<textarea name=\"Comments\" rows=\"3\""
				  " style=\"width:%upx;\">"
				  "%s"
				  "</textarea>",
			Rec_C2_BOTTOM_WIDE - 60,
			UsrDat->Comments);
	    else if (UsrDat->Comments[0])
	      {
	       Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				 UsrDat->Comments,Cns_MAX_BYTES_TEXT,false);     // Convert from HTML to rigorous HTML
	       fprintf (Gbl.F.Out,"%s",UsrDat->Comments);
	      }
	   }
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");
	}

      /***** Institution, centre, department, office, etc. *****/
      if (ShowTeacherRows)
	{
	 /* Institution */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Institution,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (ShowData)
	   {
	    if (UsrDat->InsCod > 0)
	      {
	       if (Ins.WWW[0])
		  fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"%s\">",
			   Ins.WWW,ClassData);
	       fprintf (Gbl.F.Out,"%s",Ins.FullName);
	       if (Ins.WWW[0])
		  fprintf (Gbl.F.Out,"</a>");
	      }
	   }
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Centre */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Centre,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (ShowData)
	   {
	    if (UsrDat->Tch.CtrCod > 0)
	      {
	       Ctr.CtrCod = UsrDat->Tch.CtrCod;
	       Ctr_GetDataOfCentreByCod (&Ctr);
	       if (Ctr.WWW[0])
		  fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"%s\">",
			   Ctr.WWW,ClassData);
	       fprintf (Gbl.F.Out,"%s",Ctr.FullName);
	       if (Ctr.WWW[0])
		  fprintf (Gbl.F.Out,"</a>");
	      }
	   }
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Department */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Department,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (ShowData)
	   {
	    if (UsrDat->Tch.DptCod > 0)
	      {
	       Dpt.DptCod = UsrDat->Tch.DptCod;
	       Dpt_GetDataOfDepartmentByCod (&Dpt);
	       if (Dpt.WWW[0])
		  fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"%s\">",
			   Dpt.WWW,ClassData);
	       fprintf (Gbl.F.Out,"%s",Dpt.FullName);
	       if (Dpt.WWW[0])
		  fprintf (Gbl.F.Out,"</a>");
	      }
	   }
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Office */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Office,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (ShowData)
	    fprintf (Gbl.F.Out,"%s",UsrDat->Tch.Office);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Phone */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\""
			    " style=\"width:%upx;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"%s LEFT_MIDDLE\""
			    " style=\"width:%upx;\">",
		  ClassForm,Rec_C1_BOTTOM_WIDE,Txt_Phone,
		  ClassData,Rec_C2_BOTTOM_WIDE);
	 if (ShowData)
	    fprintf (Gbl.F.Out,"%s",UsrDat->Tch.OfficePhone);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");
	}
      fprintf (Gbl.F.Out,"</table>");

      switch (TypeOfView)
        {
	 case Rec_FORM_MY_COMMON_RECORD:
	    Lay_PutConfirmButton (Txt_Save_changes);
	    Act_FormEnd ();
	    break;
	 case Rec_FORM_NEW_RECORD_OTHER_NEW_USR:
	    if (Gbl.CurrentCrs.Grps.NumGrps) // This course has groups?
	       Grp_ShowLstGrpsToChgOtherUsrsGrps (UsrDat->UsrCod);
	    Lay_PutConfirmButton (Txt_Register);
	    Act_FormEnd ();
	    break;
	 case Rec_FORM_MODIFY_RECORD_OTHER_EXISTING_USR:
	    /***** Show list of groups to register/remove me/user *****/
	    if (Gbl.CurrentCrs.Grps.NumGrps) // This course has groups?
	      {
	       if (ItsMe)
		 {
		  // Don't show groups if I don't belong to course
		  if (Gbl.Usrs.Me.IBelongToCurrentCrs)
		     Grp_ShowLstGrpsToChgMyGrps ((Gbl.Usrs.Me.LoggedRole == Rol_STUDENT));
		 }
	       else
		  Grp_ShowLstGrpsToChgOtherUsrsGrps (UsrDat->UsrCod);
	      }

	    /***** Which action, register or removing? *****/
	    if (Enr_PutActionsRegRemOneUsr (ItsMe))
	       Lay_PutConfirmButton (Txt_Confirm);

	    Act_FormEnd ();
	    break;
	 default:
	    break;
        }

      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   /***** End frame *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/*********************** Write a link to netiquette rules ********************/
/*****************************************************************************/

static void Rec_WriteLinkToDataProtectionClause (void)
  {
   extern const char *Txt_DATA_PROTECTION_CLAUSE;
   char Title[1024];

   sprintf (Title,"<div class=\"CENTER_MIDDLE\">"
	          "<a href=\"%s/%s/\" target=\"_blank\">%s</a>"
	          "</div>",
            Cfg_HTTP_URL_SWAD_PUBLIC,Cfg_DATA_PROTECTION_FOLDER,
            Txt_DATA_PROTECTION_CLAUSE);
   Lay_WriteTitle (Title);
  }

/*****************************************************************************/
/**************** Update and show data from identified user ******************/
/*****************************************************************************/

void Rec_UpdateMyRecord (void)
  {
   /***** Get my data from record form *****/
   Rec_GetUsrNameFromRecordForm (&Gbl.Usrs.Me.UsrDat);
   Rec_GetUsrExtraDataFromRecordForm (&Gbl.Usrs.Me.UsrDat);

   /***** Update my data in database *****/
   Enr_UpdateUsrData (&Gbl.Usrs.Me.UsrDat);
  }

/*****************************************************************************/
/**** Get and check future user's role in current course from record form ****/
/*****************************************************************************/

Rol_Role_t Rec_GetRoleFromRecordForm (void)
  {
   char UnsignedStr[10+1];
   Rol_Role_t Role;

   /***** Get role as a parameter from form *****/
   Par_GetParToText ("Role",UnsignedStr,10);
   Role = Rol_ConvertUnsignedStrToRole (UnsignedStr);

   /***** Check if I can register a user
          with the received role in current course *****/
   /* Received role must be student or teacher */
   if (!(Role == Rol_STUDENT ||
         Role == Rol_TEACHER))
      Lay_ShowErrorAndExit ("Wrong user's role.");

   /* Check for other possible errors */
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_STUDENT:		// I am logged as student
         /* A student can only change his/her data, but not his/her role */
	 if (Role != Rol_STUDENT)
            Lay_ShowErrorAndExit ("Wrong user's role.");
	 break;
      case Rol_TEACHER:		// I am logged as teacher
	 /* A teacher can only register another user as teacher
	    if the other is already teacher in any course.
	    That is, a teacher can not upgrade a student
	    (in all other courses) to teacher */
         if (Role == Rol_TEACHER &&				// He/she will be a teacher in current course
             !(Gbl.Usrs.Other.UsrDat.Roles & (1 << Rol_TEACHER)))	// He/she was not a teacher in any course
            Lay_ShowErrorAndExit ("Wrong user's role.");
         break;
      default:
	 break;
     }
   return Role;
  }

/*****************************************************************************/
/*************** Get data fields of common record from form ******************/
/*****************************************************************************/

void Rec_GetUsrNameFromRecordForm (struct UsrData *UsrDat)
  {
   Par_GetParToText ("Surname1",UsrDat->Surname1,Usr_MAX_BYTES_NAME);
   Str_ConvertToTitleType (UsrDat->Surname1);

   Par_GetParToText ("Surname2",UsrDat->Surname2,Usr_MAX_BYTES_NAME);
   Str_ConvertToTitleType (UsrDat->Surname2);

   Par_GetParToText ("FirstName",UsrDat->FirstName,Usr_MAX_BYTES_NAME);
   Str_ConvertToTitleType (UsrDat->FirstName);

   Usr_BuildFullName (UsrDat);
  }

void Rec_GetUsrExtraDataFromRecordForm (struct UsrData *UsrDat)
  {
   char UnsignedStr[10+1];
   char LongStr[1+10+1];
   unsigned UnsignedNum;

   /***** Get sex from form *****/
   Par_GetParToText ("Sex",UnsignedStr,10);
   UsrDat->Sex = Usr_SEX_UNKNOWN;
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum <= Usr_SEX_MALE)
	 UsrDat->Sex = (Usr_Sex_t) UnsignedNum;

   /***** Get country code *****/
   Par_GetParToText ("OthCtyCod",LongStr,1+10);
   UsrDat->CtyCod = Str_ConvertStrCodToLongCod (LongStr);

   Par_GetParToText ("OriginPlace",UsrDat->OriginPlace,Cns_MAX_BYTES_STRING);
   Str_ConvertToTitleType (UsrDat->OriginPlace);

   Dat_GetDateFromForm ("DiaNac","MesNac","AnoNac",&(UsrDat->Birthday.Day),&(UsrDat->Birthday.Month),&(UsrDat->Birthday.Year));
   Dat_ConvDateToDateStr (&(UsrDat->Birthday),UsrDat->StrBirthday);

   Par_GetParToText ("LocalAddress",UsrDat->LocalAddress,Cns_MAX_BYTES_STRING);

   Par_GetParToText ("LocalPhone",UsrDat->LocalPhone,Usr_MAX_BYTES_PHONE);

   Par_GetParToText ("FamilyAddress",UsrDat->FamilyAddress,Cns_MAX_BYTES_STRING);

   Par_GetParToText ("FamilyPhone",UsrDat->FamilyPhone,Usr_MAX_BYTES_PHONE);

   Rec_GetUsrCommentsFromForm (UsrDat);
  }

/*****************************************************************************/
/********** Get the comments of the record of a user from the form ***********/
/*****************************************************************************/

static void Rec_GetUsrCommentsFromForm (struct UsrData *UsrDat)
  {
   /***** Check if memory is allocated for comments *****/
   if (!UsrDat->Comments)
      Lay_ShowErrorAndExit ("Can not read comments of a user.");

   /***** Get the parameter with the comments *****/
   Par_GetParToHTML ("Comments",UsrDat->Comments,Cns_MAX_BYTES_TEXT);
  }

/*****************************************************************************/
/*** Put a link to the action to edit my institution, centre, department... **/
/*****************************************************************************/

static void Rec_PutLinkToChangeMyInsCtrDpt (void)
  {
   extern const char *Txt_Edit_my_institution;

   /***** Link to edit my institution, centre, department... *****/
   Act_PutContextualLink (ActReqEdiMyIns,NULL,
                          "institution",Txt_Edit_my_institution);
  }

/*****************************************************************************/
/*********** Put a link to the action to edit my social networks *************/
/*****************************************************************************/

static void Rec_PutLinkToChangeMySocialNetworks (void)
  {
   extern const char *Txt_Edit_my_webs_networks;

   /***** Link to edit my social networks *****/
   Act_PutContextualLink (ActReqEdiMyNet,NULL,
                          "earth",Txt_Edit_my_webs_networks);
  }

/*****************************************************************************/
/********* Show form to edit my institution, centre and department ***********/
/*****************************************************************************/

#define COL2_WIDTH 400

void Rec_ShowFormMyInsCtrDpt (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Please_fill_in_your_institution;
   extern const char *Txt_Please_fill_in_your_centre_and_department;
   extern const char *Txt_Institution_centre_and_department;
   extern const char *Txt_Institution;
   extern const char *Txt_Country_of_institution;
   extern const char *Txt_Another_institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Another_centre;
   extern const char *Txt_Department;
   extern const char *Txt_Another_department;
   extern const char *Txt_Office;
   extern const char *Txt_Phone;
   const char *ClassForm = The_ClassForm[Gbl.Prefs.Theme];
   unsigned NumCty;
   unsigned NumIns;
   unsigned NumCtr;
   unsigned NumDpt;
   bool IAmTeacher = (Gbl.Usrs.Me.UsrDat.Roles & (1 << Rol_TEACHER));

   /***** If there is no institution, centre or department *****/
   if (Gbl.Usrs.Me.UsrDat.InsCtyCod < 0 ||
       Gbl.Usrs.Me.UsrDat.InsCod < 0)
      Lay_ShowAlert (Lay_WARNING,Txt_Please_fill_in_your_institution);
   else if ((Gbl.Usrs.Me.UsrDat.Roles & (1 << Rol_TEACHER)) &&
            (Gbl.Usrs.Me.UsrDat.Tch.CtrCod < 0 ||
             Gbl.Usrs.Me.UsrDat.Tch.DptCod < 0))
      Lay_ShowAlert (Lay_WARNING,Txt_Please_fill_in_your_centre_and_department);

   /***** Start table *****/
   Lay_StartRoundFrameTable ("560px",2,
                             IAmTeacher ? Txt_Institution_centre_and_department :
	                                  Txt_Institution);

   /***** Country *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s RIGHT_MIDDLE\">"
		      "%s:"
		      "</td>"
		      "<td class=\"LEFT_MIDDLE\" style=\"width:%upx;\">",
            ClassForm,Txt_Country_of_institution,
            COL2_WIDTH);

   /* If list of countries is empty, try to get it */
   if (!Gbl.Ctys.Num)
     {
      Gbl.Ctys.SelectedOrderType = Cty_ORDER_BY_COUNTRY;
      Cty_GetListCountries (Cty_GET_ONLY_COUNTRIES);
     }

   /* Start form to select the country of my institution */
   Act_FormGoToStart (ActChgCtyMyIns);
   fprintf (Gbl.F.Out,"<select name=\"OthCtyCod\" style=\"width:400px;\""
	              " onchange=\"javascript:document.getElementById('%s').submit();\">"
                      "<option value=\"-1\"",
	    Gbl.FormId);
   if (Gbl.Usrs.Me.UsrDat.InsCtyCod <= 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out," disabled=\"disabled\"></option>");
   for (NumCty = 0;
	NumCty < Gbl.Ctys.Num;
	NumCty++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%ld\"",
	       Gbl.Ctys.Lst[NumCty].CtyCod);
      if (Gbl.Ctys.Lst[NumCty].CtyCod == Gbl.Usrs.Me.UsrDat.InsCtyCod)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",
	       Gbl.Ctys.Lst[NumCty].Name[Gbl.Prefs.Language]);
     }
   fprintf (Gbl.F.Out,"</select>");
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /***** Institution *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s RIGHT_MIDDLE\">"
		      "%s:"
		      "</td>"
		      "<td class=\"LEFT_MIDDLE\" style=\"width:%upx;\">",
            ClassForm,Txt_Institution,
	    COL2_WIDTH);

   /* Get list of institutions in this country */
   Ins_FreeListInstitutions ();
   if (Gbl.Usrs.Me.UsrDat.InsCtyCod > 0)
      Ins_GetListInstitutions (Gbl.Usrs.Me.UsrDat.InsCtyCod,Ins_GET_MINIMAL_DATA);

   /* Start form to select institution */
   Act_FormGoToStart (ActChgMyIns);
   fprintf (Gbl.F.Out,"<select name=\"OthInsCod\" style=\"width:400px;\""
	              " onchange=\"javascript:document.getElementById('%s').submit();\">"
                      "<option value=\"-1\"",
	    Gbl.FormId);
   if (Gbl.Usrs.Me.UsrDat.InsCod < 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out," disabled=\"disabled\"></option>"
		      "<option value=\"0\"");
   if (Gbl.Usrs.Me.UsrDat.InsCod == 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",
	    Txt_Another_institution);
   for (NumIns = 0;
	NumIns < Gbl.Inss.Num;
	NumIns++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%ld\"",
	       Gbl.Inss.Lst[NumIns].InsCod);
      if (Gbl.Inss.Lst[NumIns].InsCod == Gbl.Usrs.Me.UsrDat.InsCod)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",
	       Gbl.Inss.Lst[NumIns].FullName);
     }
   fprintf (Gbl.F.Out,"</select>");
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   if (IAmTeacher)
     {
      /***** Centre *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
			 "%s:"
			 "</td>"
			 "<td class=\"LEFT_MIDDLE\" style=\"width:%upx;\">",
	       ClassForm,Txt_Centre,
	       COL2_WIDTH);

      /* Get list of centres in this institution */
      Ctr_FreeListCentres ();
      if (Gbl.Usrs.Me.UsrDat.InsCod > 0)
	 Ctr_GetListCentres (Gbl.Usrs.Me.UsrDat.InsCod);

      /* Start form to select centre */
      Act_FormGoToStart (ActChgMyCtr);
      fprintf (Gbl.F.Out,"<select name=\"OthCtrCod\" style=\"width:400px;\""
			 " onchange=\"javascript:document.getElementById('%s').submit();\">"
			 "<option value=\"-1\"",
	       Gbl.FormId);
      if (Gbl.Usrs.Me.UsrDat.Tch.CtrCod < 0)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out," disabled=\"disabled\"></option>"
			 "<option value=\"0\"");
      if (Gbl.Usrs.Me.UsrDat.Tch.CtrCod == 0)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",
	       Txt_Another_centre);
      for (NumCtr = 0;
	   NumCtr < Gbl.Ctrs.Num;
	   NumCtr++)
	{
	 fprintf (Gbl.F.Out,"<option value=\"%ld\"",
		  Gbl.Ctrs.Lst[NumCtr].CtrCod);
	 if (Gbl.Ctrs.Lst[NumCtr].CtrCod == Gbl.Usrs.Me.UsrDat.Tch.CtrCod)
	    fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%s</option>",
		  Gbl.Ctrs.Lst[NumCtr].FullName);
	}
      fprintf (Gbl.F.Out,"</select>");
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      /***** Department *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
			 "%s:"
			 "</td>"
			 "<td class=\"LEFT_MIDDLE\" style=\"width:%upx;\">",
	       ClassForm,Txt_Department,
	       COL2_WIDTH);

      /* Get list of departments in this institution */
      Dpt_FreeListDepartments ();
      if (Gbl.Usrs.Me.UsrDat.InsCod > 0)
	 Dpt_GetListDepartments (Gbl.Usrs.Me.UsrDat.InsCod);

      /* Start form to select department */
      Act_FormGoToStart (ActChgMyDpt);
      fprintf (Gbl.F.Out,"<select name=\"DptCod\" style=\"width:400px;\""
			 " onchange=\"javascript:document.getElementById('%s').submit();\">"
			 "<option value=\"-1\"",
	       Gbl.FormId);
      if (Gbl.Usrs.Me.UsrDat.Tch.DptCod < 0)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out," disabled=\"disabled\"></option>"
			 "<option value=\"0\"");
      if (Gbl.Usrs.Me.UsrDat.Tch.DptCod == 0)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",
	       Txt_Another_department);
      for (NumDpt = 0;
	   NumDpt < Gbl.Dpts.Num;
	   NumDpt++)
	{
	 fprintf (Gbl.F.Out,"<option value=\"%ld\"",
		  Gbl.Dpts.Lst[NumDpt].DptCod);
	 if (Gbl.Dpts.Lst[NumDpt].DptCod == Gbl.Usrs.Me.UsrDat.Tch.DptCod)
	    fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%s</option>",
		  Gbl.Dpts.Lst[NumDpt].FullName);
	}
      fprintf (Gbl.F.Out,"</select>");
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      /***** Office *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"%s RIGHT_MIDDLE\">"
	                 "%s:"
	                 "</td>"
                         "<td class=\"LEFT_MIDDLE\" style=\"width:%upx;\">",
               ClassForm,Txt_Office,
               COL2_WIDTH);
      Act_FormGoToStart (ActChgMyOff);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Office\""
			 " style=\"width:400px;\" maxlength=\"%u\" value=\"%s\""
			 " onchange=\"javascript:document.getElementById('%s').submit();\" />",
               Cns_MAX_LENGTH_STRING,
	       Gbl.Usrs.Me.UsrDat.Tch.Office,
	       Gbl.FormId);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      /***** Phone *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"%s RIGHT_MIDDLE\">"
	                 "%s:"
	                 "</td>"
	                 "<td class=\"LEFT_MIDDLE\" style=\"width:%upx;\">",
               ClassForm,Txt_Phone,
               COL2_WIDTH);
      Act_FormGoToStart (ActChgMyOffPho);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"OfficePhone\""
			 " style=\"width:400px;\" maxlength=\"%u\" value=\"%s\""
			 " onchange=\"javascript:document.getElementById('%s').submit();\" />",
	       Usr_MAX_LENGTH_PHONE,
	       Gbl.Usrs.Me.UsrDat.Tch.OfficePhone,
	       Gbl.FormId);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");
     }

   /***** End table *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/******** Receive form data to change the country of my institution **********/
/*****************************************************************************/

void Rec_ChgCountryOfMyInstitution (void)
  {
   /***** Get country of my institution *****/
   /* Get country code */
   if ((Gbl.Usrs.Me.UsrDat.InsCtyCod = Cty_GetParamOtherCtyCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of institution country is missing.");

   /***** Update institution, centre and department *****/
   // When country changes, the institution, centre and department must be reset
   Gbl.Usrs.Me.UsrDat.InsCod = -1L;
   Gbl.Usrs.Me.UsrDat.Tch.CtrCod = -1L;
   Gbl.Usrs.Me.UsrDat.Tch.DptCod = -1L;
   Enr_UpdateInstitutionCentreDepartment ();

   /***** Show form again *****/
   Rec_ShowFormMyInsCtrDpt ();
  }

/*****************************************************************************/
/**************** Receive form data to change my institution *****************/
/*****************************************************************************/

void Rec_UpdateMyInstitution (void)
  {
   struct Institution Ins;

   /***** Get my institution *****/
   /* Get institution code */
   if ((Ins.InsCod = Ins_GetParamOtherInsCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of institution is missing.");

   /* Get country of institution */
   if (Ins.InsCod > 0)
     {
      Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);
      if (Gbl.Usrs.Me.UsrDat.InsCtyCod != Ins.CtyCod)
	 Gbl.Usrs.Me.UsrDat.InsCtyCod = Ins.CtyCod;
     }

   /***** Update institution, centre and department *****/
   Gbl.Usrs.Me.UsrDat.InsCod = Ins.InsCod;
   Gbl.Usrs.Me.UsrDat.Tch.CtrCod = -1L;
   Gbl.Usrs.Me.UsrDat.Tch.DptCod = -1L;
   Enr_UpdateInstitutionCentreDepartment ();

   /***** Show form again *****/
   Rec_ShowFormMyInsCtrDpt ();
  }

/*****************************************************************************/
/******************* Receive form data to change my centre *******************/
/*****************************************************************************/

void Rec_UpdateMyCentre (void)
  {
   struct Centre Ctr;

   /***** Get my centre *****/
   /* Get centre code */
   if ((Ctr.CtrCod = Ctr_GetParamOtherCtrCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of centre is missing.");

   /* Get institution of centre */
   if (Ctr.CtrCod > 0)
     {
      Ctr_GetDataOfCentreByCod (&Ctr);
      if (Gbl.Usrs.Me.UsrDat.InsCod != Ctr.InsCod)
	{
	 Gbl.Usrs.Me.UsrDat.InsCod = Ctr.InsCod;
         Gbl.Usrs.Me.UsrDat.Tch.DptCod = -1L;
	}
     }

   /***** Update institution, centre and department *****/
   Gbl.Usrs.Me.UsrDat.Tch.CtrCod = Ctr.CtrCod;
   Enr_UpdateInstitutionCentreDepartment ();

   /***** Show form again *****/
   Rec_ShowFormMyInsCtrDpt ();
  }

/*****************************************************************************/
/***************** Receive form data to change my department *****************/
/*****************************************************************************/

void Rec_UpdateMyDepartment (void)
  {
   struct Department Dpt;

   /***** Get my department *****/
   /* Get department code */
   if ((Dpt.DptCod = Dpt_GetParamDptCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of department is missing.");

   /* Get institution of department */
   if (Dpt.DptCod > 0)
     {
      Dpt_GetDataOfDepartmentByCod (&Dpt);
      if (Gbl.Usrs.Me.UsrDat.InsCod != Dpt.InsCod)
	{
	 Gbl.Usrs.Me.UsrDat.InsCod = Dpt.InsCod;
         Gbl.Usrs.Me.UsrDat.Tch.CtrCod = -1L;
	}
     }

   /***** Update institution, centre and department *****/
   Gbl.Usrs.Me.UsrDat.Tch.DptCod = Dpt.DptCod;
   Enr_UpdateInstitutionCentreDepartment ();

   /***** Show form again *****/
   Rec_ShowFormMyInsCtrDpt ();
  }

/*****************************************************************************/
/******************* Receive form data to change my office *******************/
/*****************************************************************************/

void Rec_UpdateMyOffice (void)
  {
   char Query[128+Cns_MAX_BYTES_STRING];

   /***** Get my office *****/
   Par_GetParToText ("Office",Gbl.Usrs.Me.UsrDat.Tch.Office,Cns_MAX_BYTES_STRING);

   /***** Update office *****/
   sprintf (Query,"UPDATE usr_data"
	          " SET Office='%s'"
	          " WHERE UsrCod='%ld'",
	    Gbl.Usrs.Me.UsrDat.Tch.Office,
	    Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update office");

   /***** Show form again *****/
   Rec_ShowFormMyInsCtrDpt ();
  }

/*****************************************************************************/
/**************** Receive form data to change my office phone ****************/
/*****************************************************************************/

void Rec_UpdateMyOfficePhone (void)
  {
   char Query[128+Usr_MAX_BYTES_PHONE];

   /***** Get my office *****/
   Par_GetParToText ("OfficePhone",Gbl.Usrs.Me.UsrDat.Tch.OfficePhone,Usr_MAX_BYTES_PHONE);

   /***** Update office phone *****/
   sprintf (Query,"UPDATE usr_data"
	          " SET OfficePhone='%s'"
	          " WHERE UsrCod='%ld'",
	    Gbl.Usrs.Me.UsrDat.Tch.OfficePhone,
	    Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update office phone");

   /***** Show form again *****/
   Rec_ShowFormMyInsCtrDpt ();
  }
