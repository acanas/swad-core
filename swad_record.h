// swad_record.h: users' record cards

#ifndef _SWAD_REC
#define _SWAD_REC
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Rec_WIDTH_SHARE_RECORD_BIG	580
#define Rec_WIDTH_SHARE_RECORD_SMALL	540
#define Rec_WIDTH_SHARE_RECORD_PRINT	540
#define Rec_WIDTH_COURSE_RECORD		Rec_WIDTH_SHARE_RECORD_SMALL
#define Rec_WIDTH_COURSE_RECORD_PRINT	Rec_WIDTH_SHARE_RECORD_PRINT

#define Rec_MAX_LENGTH_NAME_FIELD	255
#define Rec_MIN_LINES_IN_EDITION_FIELD	  1
#define Rec_MAX_LINES_IN_EDITION_FIELD	 50

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   Rec_FORM_SIGN_UP,
   Rec_FORM_MY_SHARE_RECORD,
   Rec_FORM_MODIFY_RECORD_OTHER_EXISTING_USR,
   Rec_FORM_NEW_RECORD_OTHER_NEW_USR,
   Rec_MY_SHARE_RECORD_CHECK,
   Rec_OTHER_USR_SHARE_RECORD_CHECK,
   Rec_FORM_MY_COURSE_RECORD,
   Rec_MY_COURSE_RECORD_CHECK,
   Rec_OTHER_USR_COURSE_RECORD_CHECK,
   Rec_RECORD_LIST,
   Rec_RECORD_PRINT,
  } Rec_RecordViewType_t;

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
void Rec_ListRecordsInvs (void);
void Rec_GetUsrAndShowRecordOneStdCrs (void);
void Rec_ListRecordsStdsCrs (void);
void Rec_GetUsrAndShowRecordOneTchCrs (void);
void Rec_ListRecordsTchsCrs (void);
void Rec_ShowLinkToPrintPreviewOfRecords (void);
void Rec_ShowFormMyCrsRecord (void);
void Rec_UpdateAndShowMyCrsRecord (void);
void Rec_UpdateAndShowOtherCrsRecord (void);
void Rec_ShowCrsRecord (Rec_RecordViewType_t TypeOfView,struct UsrData *UsrDat);
unsigned long Rec_GetFieldFromCrsRecord (long UsrCod,long FieldCod,MYSQL_RES **mysql_res);
void Rec_GetFieldsCrsRecordFromForm (void);
void Rec_UpdateCrsRecord (long UsrCod);
void Rec_RemoveFieldsCrsRecordInCrs (long UsrCod,struct Course *Crs,Cns_QuietOrVerbose_t QuietOrVerbose);
void Rec_RemoveFieldsCrsRecordAll (long UsrCod,Cns_QuietOrVerbose_t QuietOrVerbose);
void Rec_ShowMyCrsRecordUpdated (void);
void Rec_ShowOtherCrsRecordUpdated (void);
void Rec_AllocMemFieldsRecordsCrs (void);
void Rec_FreeMemFieldsRecordsCrs (void);

void Rec_ShowFormSignUpWithMyCommonRecord (void);
void Rec_ShowFormMyCommRecord (void);
void Rec_ShowFormOtherNewCommonRecord (struct UsrData *UsrDat);
void Rec_ShowMyCommonRecordUpd (void);
void Rec_ShowCommonRecordUnmodifiable (struct UsrData *UsrDat);
void Rec_ShowCommonRecord (Rec_RecordViewType_t TypeOfView,
                           struct UsrData *UsrDat);

void Rec_UpdateMyRecord (void);
Rol_Role_t Rec_GetRoleFromRecordForm (void);
void Rec_GetUsrNameFromRecordForm (struct UsrData *UsrDat);
void Rec_GetUsrExtraDataFromRecordForm (struct UsrData *UsrDat);

void Rec_ShowFormMyInsCtrDpt (void);
void Rec_ChgCountryOfMyInstitution (void);
void Rec_UpdateMyInstitution (void);
void Rec_UpdateMyCentre (void);
void Rec_UpdateMyDepartment (void);
void Rec_UpdateMyOffice (void);
void Rec_UpdateMyOfficePhone (void);

#endif
