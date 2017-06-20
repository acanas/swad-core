// swad_user.h: users

#ifndef _SWAD_USR
#define _SWAD_USR
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_action.h"
#include "swad_constant.h"
#include "swad_cryptography.h"
#include "swad_date.h"
#include "swad_degree.h"
#include "swad_icon.h"
#include "swad_layout.h"
#include "swad_menu.h"
#include "swad_nickname.h"
#include "swad_password.h"
#include "swad_privacy_visibility_type.h"
#include "swad_role_type.h"
#include "swad_scope.h"
#include "swad_search.h"
#include "swad_string.h"
#include "swad_theme.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Usr_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS  6
#define Usr_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS 12
#define Usr_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS 60

#define Usr_MAX_CHARS_FIRSTNAME_OR_SURNAME	(32 - 1)	// 31
#define Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME	((Usr_MAX_CHARS_FIRSTNAME_OR_SURNAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 511

#define Usr_MAX_BYTES_SURNAMES			(Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1 + Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME)
						// Surname1                         +' '+ Surname2
#define Usr_MAX_BYTES_FULL_NAME			(Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1 + Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME +     6    + Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME)
						// Surname1                         +' '+ Surname2                           +  ','+' ' + FirstName
						// Surname1                         +' '+ Surname2                           + '<br />' + FirstName

#define Usr_BIRTHDAY_STR_DB_LENGTH (4 + 1 + 2 + 1 + 2)	// "'%04u-%02u-%02u'"

#define Usr_MAX_CHARS_ADDRESS	(128 - 1)	// 127
#define Usr_MAX_BYTES_ADDRESS	((Usr_MAX_CHARS_ADDRESS + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Usr_MAX_CHARS_PHONE	16
#define Usr_MAX_BYTES_PHONE	Usr_MAX_CHARS_PHONE

#define Usr_CLASS_PHOTO_COLS_DEF	10	// Default number of columns in a class photo
#define Usr_CLASS_PHOTO_COLS_MAX	50	// Maximum number of columns in a class photo

#define Usr_LIST_WITH_PHOTOS_DEF	true

#define Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS	(Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 * Cfg_MAX_USRS_IN_LIST)

#define Usr_NUM_MAIN_FIELDS_DATA_USR	 8

#define Usr_USER_LIST_SECTION_ID	"user_list"

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

// Related with user's sexs
#define Usr_NUM_SEXS 4	// Unknown, female, male, all
typedef enum
  {
   Usr_SEX_UNKNOWN = 0,
   Usr_SEX_FEMALE  = 1,
   Usr_SEX_MALE    = 2,
   Usr_SEX_ALL     = 3,	// Usr_SEX_ALL is intended for "all sexs"
  } Usr_Sex_t;
// Don't change these numbers! They are used for user's sex in database

// Related with class photograph
typedef enum
  {
   Usr_CLASS_PHOTO_SEL,		// Only for selection of users
   Usr_CLASS_PHOTO_SEL_SEE,	// Selection and seeing of users
   Usr_CLASS_PHOTO_SEE,		// Only seeing users
   Usr_CLASS_PHOTO_PRN,		// Only print users
  } Usr_ClassPhotoType_t;

// Related with type of list of users
#define Usr_NUM_USR_LIST_TYPES 3
typedef enum
  {
   Usr_LIST_UNKNOWN        = 0,
   Usr_LIST_AS_CLASS_PHOTO = 1,
   Usr_LIST_AS_LISTING     = 2,
  } Usr_ShowUsrsType_t;
#define Usr_SHOW_USRS_TYPE_DEFAULT Usr_LIST_AS_CLASS_PHOTO

// Related with user's data
struct UsrData
  {
   long UsrCod;
   char EncryptedUsrCod [Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1];
   char UsrIDNickOrEmail[Cns_MAX_BYTES_EMAIL_ADDRESS + 1];	// String to store the ID, nickname or email
   struct
     {
      struct ListIDs *List;
      unsigned Num;
     } IDs;
   char Nickname        [Nck_MAX_BYTES_NICKNAME_WITHOUT_ARROBA + 1];
   char Password        [Pwd_BYTES_ENCRYPTED_PASSWORD + 1];
   struct
     {
      struct
        {
	 long GotFromDBForUsrCod;	// Role was got from database for this user (used to not retrieve role if already retrieved)
	 Rol_Role_t Role;
        } InCurrentCrs;	// Role in current course (Rol_UNK is no course selected)
      int InCrss;	// Roles in all his/her courses
			// Check always if filled/calculated
			// >=0 ==> filled/calculated
			//  <0 ==> not yet filled/calculated
     } Roles;
   bool Accepted;	// User has accepted joining to current course?
   char Surname1	[Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];
   char Surname2	[Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];
   char FirstName	[Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];
   char FullName	[Usr_MAX_BYTES_FULL_NAME + 1];
   Usr_Sex_t Sex;
   char Email		[Cns_MAX_BYTES_EMAIL_ADDRESS + 1];
   bool EmailConfirmed;
   char Photo		[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1];	// Name of public link to photo
   Pri_Visibility_t PhotoVisibility;	// Who can see user's photo
   Pri_Visibility_t ProfileVisibility;	// Who can see user's public profile
   long CtyCod;		// Country
   char OriginPlace	[Usr_MAX_BYTES_ADDRESS + 1];
   struct Date Birthday;
   char StrBirthday	[Cns_MAX_BYTES_DATE + 1];
   char LocalAddress	[Usr_MAX_BYTES_ADDRESS + 1];
   char LocalPhone	[Usr_MAX_BYTES_PHONE + 1];
   char FamilyAddress	[Usr_MAX_BYTES_ADDRESS + 1];
   char FamilyPhone	[Usr_MAX_BYTES_PHONE + 1];
   char *Comments;
   long InsCtyCod;	// Country of the institution
   long InsCod;		// Institution
   struct
     {
      long CtrCod;	// Centre
      long DptCod;	// Department
      char Office	[Usr_MAX_BYTES_ADDRESS + 1];
      char OfficePhone	[Usr_MAX_BYTES_PHONE  + 1];
     } Tch;
   struct
     {
      Txt_Language_t Language;
      unsigned FirstDayOfWeek;
      Dat_Format_t DateFormat;
      The_Theme_t Theme;
      Ico_IconSet_t IconSet;
      Mnu_Menu_t Menu;
      unsigned SideCols;
      unsigned NotifNtfEvents;		// One bit activated for each type of event
      unsigned EmailNtfEvents;		// One bit activated for each type of event
     } Prefs;
  };

struct UsrLast
  {
   Sch_WhatToSearch_t WhatToSearch;	// Search courses, teachers, documents...?
   long LastCrs;
   Tab_Tab_t LastTab;
   long LastAccNotif;
  };

struct UsrInList
  {
   long UsrCod;
   char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1];
   char Password[Pwd_BYTES_ENCRYPTED_PASSWORD + 1];
   char Surname1 [Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];
   char Surname2 [Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];
   char FirstName[Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];
   Usr_Sex_t Sex;
   char Photo[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1];	// Name of public link to photo
   Pri_Visibility_t PhotoVisibility;				// Who can see user's photo
   long CtyCod;		// Country
   long InsCod;		// Institution
   Rol_Role_t RoleInCurrentCrsDB;	// Role in current course in database
   bool Accepted;	// User has accepted joining to one/all courses?
   bool Remove;		// A boolean associated with each user that indicates if it must be removed
  };

struct ListUsrs
  {
   struct UsrInList *Lst;	// List of users
   unsigned NumUsrs;		// Number of users in the list
  };

struct ListUsrCods
  {
   long *Lst;		// List of users' codes
   unsigned NumUsrs;	// Number of users in the list
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Usr_InformAboutNumClicksBeforePhoto (void);

void Usr_UsrDataConstructor (struct UsrData *UsrDat);
void Usr_ResetUsrDataExceptUsrCodAndIDs (struct UsrData *UsrDat);
void Usr_ResetMyLastData (void);
void Usr_UsrDataDestructor (struct UsrData *UsrDat);
void Usr_GetAllUsrDataFromUsrCod (struct UsrData *UsrDat);
void Usr_AllocateListUsrCods (struct ListUsrCods *ListUsrCods);
void Usr_FreeListUsrCods (struct ListUsrCods *ListUsrCods);
void Usr_GetUsrCodFromEncryptedUsrCod (struct UsrData *UsrDat);
void Usr_GetEncryptedUsrCodFromUsrCod (struct UsrData *UsrDat);	// TODO: Remove this funcion, it's not used
void Usr_GetUsrDataFromUsrCod (struct UsrData *UsrDat);

void Usr_BuildFullName (struct UsrData *UsrDat);

void Usr_WriteFirstNameBRSurnames (const struct UsrData *UsrDat);

bool Usr_CheckIfUsrIsAdm (long UsrCod,Sco_Scope_t Scope,long Cod);
void Usr_FlushCacheUsrIsSuperuser (void);
bool Usr_CheckIfUsrIsSuperuser (long UsrCod);

bool Usr_ICanChangeOtherUsrData (const struct UsrData *UsrDat);
bool Usr_ICanEditOtherUsr (const struct UsrData *UsrDat);

unsigned Usr_GetNumCrssOfUsr (long UsrCod);
unsigned Usr_GetNumCrssOfUsrNotAccepted (long UsrCod);
unsigned Usr_GetNumCrssOfUsrWithARole (long UsrCod,Rol_Role_t Role);
unsigned Usr_GetNumCrssOfUsrWithARoleNotAccepted (long UsrCod,Rol_Role_t Role);

unsigned Usr_GetNumUsrsInCrssOfAUsr (long UsrCod,Rol_Role_t UsrRole,
                                     Rol_Role_t OthersRole);

void Usr_FlushCacheUsrBelongsToCurrentCrs (void);
bool Usr_CheckIfUsrBelongsToCurrentCrs (const struct UsrData *UsrDat);
void Usr_FlushCacheUsrHasAcceptedInCurrentCrs (void);
bool Usr_CheckIfUsrHasAcceptedInCurrentCrs (const struct UsrData *UsrDat);
bool Usr_CheckIfICanViewRecordStd (const struct UsrData *UsrDat);
bool Usr_CheckIfICanViewRecordTch (struct UsrData *UsrDat);
bool Usr_CheckIfICanViewWrkTstAtt (const struct UsrData *UsrDat);
bool Usr_CheckIfICanViewUsrAgenda (struct UsrData *UsrDat);
void Usr_FlushCacheUsrSharesAnyOfMyCrs (void);
bool Usr_CheckIfUsrSharesAnyOfMyCrs (struct UsrData *UsrDat);
bool Usr_CheckIfUsrSharesAnyOfMyCrsWithDifferentRole (long UsrCod);

void Usr_GetMyCountrs (void);
void Usr_GetMyInstits (void);
void Usr_GetMyCentres (void);
void Usr_GetMyDegrees (void);
void Usr_GetMyCourses (void);

void Usr_FreeMyCountrs (void);
void Usr_FreeMyInstits (void);
void Usr_FreeMyCentres (void);
void Usr_FreeMyDegrees (void);
void Usr_FreeMyCourses (void);

void Usr_FlushCacheUsrBelongsToIns (void);
bool Usr_CheckIfUsrBelongsToIns (long UsrCod,long InsCod);
void Usr_FlushCacheUsrBelongsToCtr (void);
bool Usr_CheckIfUsrBelongsToCtr (long UsrCod,long CtrCod);
void Usr_FlushCacheUsrBelongsToDeg (void);
bool Usr_CheckIfUsrBelongsToDeg (long UsrCod,long DegCod);
void Usr_FlushCacheUsrBelongsToCrs (void);
bool Usr_CheckIfUsrBelongsToCrs (long UsrCod,long CrsCod,
                                 bool CountOnlyAcceptedCourses);

bool Usr_CheckIfIBelongToCty (long CtyCod);
bool Usr_CheckIfIBelongToIns (long InsCod);
bool Usr_CheckIfIBelongToCtr (long CtrCod);
bool Usr_CheckIfIBelongToDeg (long DegCod);
bool Usr_CheckIfIBelongToCrs (long CrsCod);

unsigned Usr_GetCtysFromUsr (long UsrCod,MYSQL_RES **mysql_res);
unsigned long Usr_GetInssFromUsr (long UsrCod,long CtyCod,MYSQL_RES **mysql_res);
unsigned long Usr_GetCtrsFromUsr (long UsrCod,long InsCod,MYSQL_RES **mysql_res);
unsigned long Usr_GetDegsFromUsr (long UsrCod,long CtrCod,MYSQL_RES **mysql_res);
unsigned long Usr_GetCrssFromUsr (long UsrCod,long DegCod,MYSQL_RES **mysql_res);

bool Usr_ChkIfEncryptedUsrCodExists (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64]);

void Usr_WriteLandingPage (void);
void Usr_WriteFormLogout (void);
void Usr_Logout (void);
void Usr_PutLinkToLogin (void);
void Usr_WriteFormLogin (Act_Action_t NextAction,void (*FuncParams) ());
void Usr_WelcomeUsr (void);

void Usr_CreateBirthdayStrDB (const struct UsrData *UsrDat,
                              char BirthdayStrDB[Usr_BIRTHDAY_STR_DB_LENGTH + 1]);

void Usr_PutFormLogIn (void);
void Usr_WriteLoggedUsrHead (void);
void Usr_PutFormLogOut (void);
void Usr_GetParamUsrIdLogin (void);
unsigned Usr_GetParamOtherUsrIDNickOrEMailAndGetUsrCods (struct ListUsrCods *ListUsrCods);

void Usr_PutParamMyUsrCodEncrypted (void);
void Usr_PutParamOtherUsrCodEncrypted (void);
void Usr_PutParamUsrCodEncrypted (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1]);
void Usr_GetParamOtherUsrCodEncrypted (struct UsrData *UsrDat);
void Usr_GetParamOtherUsrCodEncryptedAndGetListIDs (void);
bool Usr_GetParamOtherUsrCodEncryptedAndGetUsrData (void);

void Usr_ChkUsrAndGetUsrData (void);

void Usr_ShowFormsLogoutAndRole (void);

bool Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (struct UsrData *UsrDat);
void Usr_UpdateMyLastData (void);
void Usr_InsertMyLastCrsTabAndTime (void);

void Usr_WriteRowUsrMainData (unsigned NumUsr,struct UsrData *UsrDat,
                              bool PutCheckBoxToSelectUsr,Rol_Role_t Role);

unsigned Usr_GetNumUsrsInCrs (Rol_Role_t Role,long CrsCod);
unsigned Usr_GetNumUsrsInCrssOfDeg (Rol_Role_t Role,long DegCod);
unsigned Usr_GetNumUsrsInCrssOfCtr (Rol_Role_t Role,long CtrCod);
unsigned Usr_GetNumUsrsInCrssOfIns (Rol_Role_t Role,long InsCod);
unsigned Usr_GetNumUsrsInCrssOfCty (Rol_Role_t Role,long CtyCod);

long Usr_GetRamdomStdFromCrs (long CrsCod);
long Usr_GetRamdomStdFromGrp (long GrpCod);

unsigned Usr_GetNumTchsCurrentInsInDepartment (long DptCod);
unsigned Usr_GetNumUsrsWhoClaimToBelongToCty (long CtyCod);
unsigned Usr_GetNumUsrsWhoClaimToBelongToIns (long InsCod);
unsigned Usr_GetNumUsrsWhoClaimToBelongToCtr (long CtrCod);
unsigned Usr_GetNumberOfTeachersInCentre (long CtrCod);

void Usr_GetListUsrs (Sco_Scope_t Scope,Rol_Role_t Role);

void Usr_SearchListUsrs (Rol_Role_t Role);
void Usr_CreateTmpTableAndSearchCandidateUsrs (const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1]);
void Usr_DropTmpTableWithCandidateUsrs (void);

void Usr_GetUnorderedStdsCodesInDeg (long DegCod);

void Usr_CopyBasicUsrDataFromList (struct UsrData *UsrDat,const struct UsrInList *UsrInList);
void Usr_FreeUsrsList (Rol_Role_t Role);

bool Usr_GetIfShowBigList (unsigned NumUsrs,const char *OnSubmit);

void Usr_PutHiddenParUsrCodAll (Act_Action_t NextAction,const char *ListUsrCods);
void Usr_GetListsSelectedUsrsCods (void);
bool Usr_GetListMsgRecipientsWrittenExplicitelyBySender (bool WriteErrorMsgs);
bool Usr_FindUsrCodInListOfSelectedUsrs (const char *EncryptedUsrCodToFind);
unsigned Usr_CountNumUsrsInListOfSelectedUsrs (void);
void Usr_FreeListsSelectedUsrsCods (void);
void Usr_FreeListOtherRecipients (void);

void Usr_ShowFormsToSelectUsrListType (Act_Action_t NextAction);
void Usr_PutCheckboxToSelectAllUsers (Rol_Role_t Role);
unsigned Usr_GetColumnsForSelectUsrs (void);
void Usr_SetUsrDatMainFieldNames (void);
void Usr_WriteHeaderFieldsUsrDat (bool PutCheckBoxToSelectUsr);

void Usr_PutExtraParamsUsrList (Act_Action_t NextAction);
void Usr_ListUsersToSelect (Rol_Role_t Role);

void Usr_ListAllDataGsts (void);
void Usr_ListAllDataStds (void);
void Usr_ListAllDataTchs (void);
unsigned Usr_ListUsrsFound (Rol_Role_t Role,
                            const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY]);
void Usr_ListDataAdms (void);

void Usr_PutParamsPrefsAboutUsrList (void);
void Usr_GetAndUpdatePrefsAboutUsrList (void);
void Usr_PutParamUsrListType (Usr_ShowUsrsType_t ListType);
void Usr_GetAndUpdateColsClassPhoto (void);
void Usr_PutParamColsClassPhoto (void);
void Usr_PutParamListWithPhotos (void);
void Usr_GetMyPrefAboutListWithPhotosFromDB (void);

void Usr_SeeGuests (void);
void Usr_SeeStudents (void);
void Usr_SeeTeachers (void);
void Usr_SeeGstClassPhotoPrn (void);
void Usr_SeeStdClassPhotoPrn (void);
void Usr_SeeTchClassPhotoPrn (void);
void Usr_PutSelectorNumColsClassPhoto (void);

void Usr_ConstructPathUsr (long UsrCod,char *PathUsr);
bool Usr_ChkIfUsrCodExists (long UsrCod);

void Usr_ShowWarningNoUsersFound (Rol_Role_t Role);

unsigned Usr_GetTotalNumberOfUsersInPlatform (void);
unsigned Usr_GetTotalNumberOfUsersInCourses (Sco_Scope_t Scope,unsigned Roles);
unsigned Usr_GetNumUsrsNotBelongingToAnyCrs (void);
float Usr_GetNumCrssPerUsr (Rol_Role_t Role);
float Usr_GetNumUsrsPerCrs (Rol_Role_t Role);

bool Usr_CheckIfUsrBanned (long UsrCod);
void Usr_RemoveUsrFromUsrBanned (long UsrCod);

void Usr_PrintUsrQRCode (void);

void Usr_WriteAuthor1Line (long UsrCod,bool Hidden);

#endif
