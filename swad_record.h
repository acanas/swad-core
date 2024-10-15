// swad_record.h: users' record cards

#ifndef _SWAD_REC
#define _SWAD_REC
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Rec_MAX_CHARS_NAME_FIELD	(128 - 1)	// 127
#define Rec_MAX_BYTES_NAME_FIELD	((Rec_MAX_CHARS_NAME_FIELD + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Rec_MIN_LINES_IN_EDITION_FIELD	  1
#define Rec_MAX_LINES_IN_EDITION_FIELD	 50
#define Rec_DEF_LINES_IN_EDITION_FIELD Rec_MIN_LINES_IN_EDITION_FIELD

#define Rec_MIN_RECORDS_PER_PAGE  1
#define Rec_MAX_RECORDS_PER_PAGE 10
#define Rec_DEF_RECORDS_PER_PAGE Rec_MIN_RECORDS_PER_PAGE

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

#define Rec_SHARED_NUM_VIEW_TYPES 8
typedef enum
  {
   Rec_SHA_SIGN_UP_IN_CRS_FORM,

   Rec_SHA_MY_RECORD_FORM,

   Rec_SHA_OTHER_EXISTING_USR_FORM,
   Rec_SHA_OTHER_NEW_USR_FORM,
   Rec_SHA_OTHER_USR_CHECK,

   Rec_SHA_RECORD_LIST,
   Rec_SHA_RECORD_PRINT,
   Rec_SHA_RECORD_PUBLIC,
  } Rec_SharedRecordViewType_t;

#define Rec_COURSE_NUM_VIEW_TYPES 6
typedef enum
  {
   Rec_CRS_MY_RECORD_AS_STUDENT_FORM,
   Rec_CRS_MY_RECORD_AS_STUDENT_CHECK,
   Rec_CRS_LIST_ONE_RECORD,
   Rec_CRS_LIST_SEVERAL_RECORDS,
   Rec_CRS_PRINT_ONE_RECORD,
   Rec_CRS_PRINT_SEVERAL_RECORDS,
  } Rec_CourseRecordViewType_t;

typedef enum {
   Rec_RECORD_USERS_UNKNOWN,
   Rec_RECORD_USERS_GUESTS,
   Rec_RECORD_USERS_STUDENTS,
   Rec_RECORD_USERS_TEACHERS,
  } Rec_UsrsType_t;

// Related with user's index card
#define Rec_NUM_TYPES_VISIBILITY 3
typedef enum
  {
   Rec_HIDDEN_FIELD   = 0,
   Rec_VISIBLE_FIELD  = 1,
   Rec_EDITABLE_FIELD = 2,
  } Rec_VisibilityRecordFields_t;
#define Rec_VISIBILITY_DEFAULT Rec_HIDDEN_FIELD

struct RecordField
  {
   long FieldCod;		// FieldCode
   char Name[Rec_MAX_BYTES_NAME_FIELD + 1];	// Field name
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
void Rec_ShowFormCreateRecordField (void);
void Rec_ReceiveField (void);
unsigned Rec_ConvertToNumLinesField (const char *StrNumLines);
bool Rec_CheckIfRecordFieldIsRepeated (const char *FldName);
void Rec_CreateRecordField (void);
void Rec_ReqRemField (void);
void Rec_RemoveFieldFromDB (void);
void Rec_RemoveField (void);
void Rec_RenameField (void);
void Rec_ChangeLinesField (void);
void Rec_ChangeVisibilityField (void);
void Rec_FreeListFields (void);

void Rec_PutLinkToEditRecordFields (void);

void Rec_ListRecordsGstsShow (void);
void Rec_ListRecordsGstsPrint (void);

void Rec_GetUsrAndShowRecOneStdCrs (void);
void Rec_ListRecordsStdsShow (void);
void Rec_ListRecordsStdsPrint (void);

void Rec_GetUsrAndShowRecOneTchCrs (void);
void Rec_ShowRecordOneTchCrs (void);
void Rec_ListRecordsTchsShow (void);
void Rec_ListRecordsTchsPrint (void);

void Rec_UpdateAndShowMyCrsRecord (void);
void Rec_UpdateAndShowOtherCrsRecord (void);
void Rec_GetFieldsCrsRecordFromForm (void);
void Rec_UpdateCrsRecord (long UsrCod);
void Rec_AllocMemFieldsRecordsCrs (void);
void Rec_FreeMemFieldsRecordsCrs (void);

void Rec_ShowFormSignUpInCrsWithMySharedRecord (void);
void Rec_ShowFormOtherNewSharedRecord (struct Usr_Data *UsrDat,Rol_Role_t DefaultRole);
void Rec_ShowOtherSharedRecordEditable (void);
void Rec_ShowMySharedRecordUpd (void);
void Rec_ShowSharedRecordUnmodifiable (struct Usr_Data *UsrDat);
void Rec_ShowPublicSharedRecordOtherUsr (void);
void Rec_ShowSharedUsrRecord (Rec_SharedRecordViewType_t TypeOfView,
                              struct Usr_Data *UsrDat,const char *Anchor);
void Rec_PutParUsrCodEncrypted (__attribute__((unused)) void *Args);

void Rec_UpdateMyRecord (void);
Rol_Role_t Rec_GetRoleFromRecordForm (void);
void Rec_GetUsrNameFromRecordForm (struct Usr_Data *UsrDat);

void Rec_ShowMySharedRecordAndMore (void);
void Rec_ChgCountryOfMyInstitution (void);
void Rec_UpdateMyInstitution (void);
void Rec_UpdateMyCenter (void);
void Rec_UpdateMyDepartment (void);
void Rec_UpdateMyOffice (void);
void Rec_UpdateMyOfficePhone (void);

#endif
