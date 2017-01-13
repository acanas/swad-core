// swad_record.h: users' record cards

#ifndef _SWAD_REC
#define _SWAD_REC
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_user.h"

/*****************************************************************************/
/*********************** Public constants and types **************************/
/*****************************************************************************/

#define Rec_RECORD_WIDTH	560

#define Rec_MAX_LENGTH_NAME_FIELD	255
#define Rec_MIN_LINES_IN_EDITION_FIELD	  1
#define Rec_MAX_LINES_IN_EDITION_FIELD	 50

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

#define Rec_SHARED_NUM_VIEW_TYPES 9
typedef enum
  {
   Rec_SHA_SIGN_UP_FORM,

   Rec_SHA_MY_RECORD_FORM,
   Rec_SHA_MY_RECORD_CHECK,

   Rec_SHA_OTHER_EXISTING_USR_FORM,
   Rec_SHA_OTHER_NEW_USR_FORM,
   Rec_SHA_OTHER_USR_CHECK,

   Rec_SHA_RECORD_LIST,
   Rec_SHA_RECORD_PRINT,
   Rec_SHA_RECORD_PUBLIC,
  } Rec_SharedRecordViewType_t;

#define Rec_COURSE_NUM_VIEW_TYPES 4
typedef enum
  {
   Rec_CRS_MY_RECORD_AS_STUDENT_FORM,
   Rec_CRS_MY_RECORD_AS_STUDENT_CHECK,
   Rec_CRS_RECORD_LIST,
   Rec_CRS_RECORD_PRINT,
  } Rec_CourseRecordViewType_t;

typedef enum {
   Rec_RECORD_USERS_UNKNOWN,
   Rec_RECORD_USERS_GUESTS,
   Rec_RECORD_USERS_STUDENTS,
   Rec_RECORD_USERS_TEACHERS,
  } Rec_UsrsType_t;

// Related with user's index card
#define Rec_NUM_TYPES_VISIBILITY	  3
typedef enum
  {
   Rec_HIDDEN_FIELD   = 0,
   Rec_VISIBLE_FIELD  = 1,
   Rec_EDITABLE_FIELD = 2,
  } Rec_VisibilityRecordFields_t;
struct RecordField
  {
   long FieldCod;		// FieldCode
   char Name[Rec_MAX_LENGTH_NAME_FIELD+1];	// Field name
   unsigned NumLines;		// Number of rows of form used to edit the filed (always >= 2)
   Rec_VisibilityRecordFields_t Visibility;		// Hidden, visible or editable by students?
   char *Text;			// Filed text
  };
struct LstRecordFields
  {
   struct RecordField *Lst;	// List of card fields
   unsigned Num;		// Number of fields
   int NestedCalls;		// Number of nested calls to the function that allocates memory for the list
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Rec_ReqEditRecordFields (void);
void Rec_GetListRecordFieldsInCurrentCrs (void);
void Rec_ListFieldsRecordsForEdition (void);
void Rec_ShowFormCreateRecordField (void);
void Rec_ReceiveFormField (void);
unsigned Rec_ConvertToNumLinesField (const char *StrNumLines);
bool Rec_CheckIfRecordFieldIsRepeated (const char *FieldName);
unsigned long Rec_GetAllFieldsInCurrCrs (MYSQL_RES **mysql_res);
void Rec_CreateRecordField (void);
void Rec_ReqRemField (void);
long Rec_GetFieldCod (void);
unsigned Rec_CountNumRecordsInCurrCrsWithField (long FieldCod);
void Rec_AskConfirmRemFieldWithRecords (unsigned NumRecords);
void Rec_RemoveFieldFromDB (void);
void Rec_RemoveField (void);
void Rec_RenameField (void);
void Rec_ChangeLinesField (void);
void Rec_ChangeVisibilityField (void);
void Rec_FreeListFields (void);

void Rec_PutLinkToEditRecordFields (void);
void Rec_ListRecordsGsts (void);
void Rec_GetUsrAndShowRecordOneStdCrs (void);
void Rec_ListRecordsStdsForEdit (void);
void Rec_ListRecordsStdsForPrint (void);
void Rec_GetUsrAndShowRecordOneTchCrs (void);
void Rec_ListRecordsTchs (void);
void Rec_UpdateAndShowMyCrsRecord (void);
void Rec_UpdateAndShowOtherCrsRecord (void);
unsigned long Rec_GetFieldFromCrsRecord (long UsrCod,long FieldCod,MYSQL_RES **mysql_res);
void Rec_GetFieldsCrsRecordFromForm (void);
void Rec_UpdateCrsRecord (long UsrCod);
void Rec_RemoveFieldsCrsRecordInCrs (long UsrCod,struct Course *Crs,Cns_QuietOrVerbose_t QuietOrVerbose);
void Rec_RemoveFieldsCrsRecordAll (long UsrCod,Cns_QuietOrVerbose_t QuietOrVerbose);
void Rec_AllocMemFieldsRecordsCrs (void);
void Rec_FreeMemFieldsRecordsCrs (void);

void Rec_ShowFormSignUpWithMySharedRecord (void);
void Rec_ShowFormMySharedRecord (void);
void Rec_ShowFormOtherNewSharedRecord (struct UsrData *UsrDat);
void Rec_ShowMySharedRecordUpd (void);
void Rec_ShowSharedRecordUnmodifiable (struct UsrData *UsrDat);
void Rec_ShowSharedUsrRecord (Rec_SharedRecordViewType_t TypeOfView,
                              struct UsrData *UsrDat);

void Rec_UpdateMyRecord (void);
Rol_Role_t Rec_GetRoleFromRecordForm (void);
void Rec_GetUsrNameFromRecordForm (struct UsrData *UsrDat);

void Rec_PutLinkToChangeMyInsCtrDpt (void);
void Rec_ShowFormMyInsCtrDpt (void);
void Rec_ChgCountryOfMyInstitution (void);
void Rec_UpdateMyInstitution (void);
void Rec_UpdateMyCentre (void);
void Rec_UpdateMyDepartment (void);
void Rec_UpdateMyOffice (void);
void Rec_UpdateMyOfficePhone (void);

#endif
