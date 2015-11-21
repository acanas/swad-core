// swad_user.h: users

#ifndef _SWAD_USR
#define _SWAD_USR
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
#include "swad_privacy.h"
#include "swad_role.h"
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

#define Usr_MAX_BYTES_USR_EMAIL			127
#define Usr_MAX_LENGTH_USR_LOGIN		127	// @nick, e-mail or ID
#define Usr_MAX_BYTES_USR_LOGIN			127

#define Usr_MAX_LENGTH_USR_NAME_OR_SURNAME	 32
#define Usr_MAX_BYTES_NAME			 32
#define Usr_MAX_BYTES_NAME_SPEC_CHAR		(Usr_MAX_BYTES_NAME*Str_MAX_LENGTH_SPEC_CHAR_HTML)

#define Usr_MAX_LENGTH_PHONE	16
#define Usr_MAX_BYTES_PHONE	16

#define Usr_CLASS_PHOTO_COLS_DEF	10	// Default number of columns in a class photo
#define Usr_CLASS_PHOTO_COLS_MAX	50	// Maximum number of columns in a class photo

#define Usr_LIST_WITH_PHOTOS_DEF	false

#define Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS	(Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64*Cfg_MAX_USRS_IN_LIST)

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
#define Usr_NUM_USR_LIST_TYPES 2
typedef enum
  {
   Usr_CLASS_PHOTO,
   Usr_LIST,
  } Usr_ShowUsrsType_t;
#define Usr_SHOW_USRS_TYPE_DEFAULT Usr_CLASS_PHOTO

// Related with user's data
struct UsrData
  {
   long UsrCod;
   char EncryptedUsrCod [Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64+1];
   char UsrIDNickOrEmail[Usr_MAX_BYTES_USR_LOGIN+1];	// String to store the ID, nickname or e-mail
   struct
     {
      struct ListIDs *List;
      unsigned Num;
     } IDs;
   char Nickname        [Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA+1];
   char Password        [Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64+1];
   Rol_Role_t RoleInCurrentCrsDB;
   unsigned Roles;
   bool Accepted;	// User has accepted joining to current course?
   char Surname1	[Usr_MAX_BYTES_NAME+1];
   char Surname2	[Usr_MAX_BYTES_NAME+1];
   char FirstName	[Usr_MAX_BYTES_NAME+1];
   char FullName	[(Usr_MAX_BYTES_NAME+1)*3];
   Usr_Sex_t Sex;
   char Email		[Usr_MAX_BYTES_USR_EMAIL+1];
   bool EmailConfirmed;
   char Photo		[Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64+1];	// Name of public link to photo
   Pri_Visibility_t PhotoVisibility;	// Who can see user's photo
   Pri_Visibility_t ProfileVisibility;	// Who can see user's public profile
   long CtyCod;		// Country
   char OriginPlace	[Cns_MAX_BYTES_STRING+1];
   struct Date Birthday;
   char StrBirthday	[Cns_MAX_LENGTH_DATE +1];
   char LocalAddress	[Cns_MAX_BYTES_STRING+1];
   char LocalPhone	[Usr_MAX_BYTES_PHONE +1];
   char FamilyAddress	[Cns_MAX_BYTES_STRING+1];
   char FamilyPhone	[Usr_MAX_BYTES_PHONE +1];
   char *Comments;
   long InsCtyCod;	// Country of the institution
   long InsCod;		// Institution
   struct
     {
      long CtrCod;	// Centre
      long DptCod;	// Department
      char Office	[Cns_MAX_BYTES_STRING+1];
      char OfficePhone	[Usr_MAX_BYTES_PHONE +1];
     } Tch;
   struct
     {
      Txt_Language_t Language;
      unsigned FirstDayOfWeek;
      Lay_Layout_t Layout;
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
   Act_Tab_t LastTab;
   long LastAccNotif;
  };

struct UsrInList
  {
   long UsrCod;
   Usr_Sex_t Sex;
   bool Accepted;	// User has accepted joining to one/all courses?
   bool Remove;		// A boolean associated with each user that indicates if it must be removed
  };

struct ListUsers
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
void Usr_GetUsrDataFromUsrCod (struct UsrData *UsrDat);

void Usr_BuildFullName (struct UsrData *UsrDat);

void Usr_RestrictLengthAndWriteName (const struct UsrData *UsrDat,unsigned MaxChars);

bool Usr_CheckIfUsrIsAdm (long UsrCod,Sco_Scope_t Scope,long Cod);
bool Usr_CheckIfUsrIsSuperuser (long UsrCod);
unsigned Usr_GetNumCrssOfUsrWithARole (long UsrCod,Rol_Role_t Role);
unsigned Usr_GetNumUsrsInCrssOfAUsr (long UsrCod,Rol_Role_t UsrRole,
                                     Rol_Role_t OthersRole);
bool Usr_CheckIfUsrSharesAnyOfMyCrs (long UsrCod);
bool Usr_CheckIfUsrSharesAnyOfMyCrsWithDifferentRole (long UsrCod);
void Usr_GetMyInstitutions (void);
void Usr_GetMyCentres (void);
void Usr_GetMyDegrees (void);
void Usr_GetMyCourses (void);
bool Usr_CheckIfUsrBelongsToIns (long UsrCod,
                                 long InsCod,
                                 bool CountOnlyAcceptedCourses);
bool Usr_CheckIfUsrBelongsToCtr (long UsrCod,
                                 long CtrCod,
                                 bool CountOnlyAcceptedCourses);
bool Usr_CheckIfUsrBelongsToDeg (long UsrCod,
                                 long DegCod,
                                 bool CountOnlyAcceptedCourses);
bool Usr_CheckIfUsrBelongsToCrs (long UsrCod,
                                 long CrsCod,
                                 bool CountOnlyAcceptedCourses);
bool Usr_CheckIfIBelongToIns (long InsCod);
bool Usr_CheckIfIBelongToCtr (long CtrCod);
bool Usr_CheckIfIBelongToDeg (long DegCod);
bool Usr_CheckIfIBelongToCrs (long CrsCod);
unsigned Usr_GetCtysFromUsr (long UsrCod,MYSQL_RES **mysql_res);
unsigned long Usr_GetInssFromUsr (long UsrCod,long CtyCod,MYSQL_RES **mysql_res);
unsigned long Usr_GetCtrsFromUsr (long UsrCod,long InsCod,MYSQL_RES **mysql_res);
unsigned long Usr_GetDegsFromUsr (long UsrCod,long CtrCod,MYSQL_RES **mysql_res);
unsigned long Usr_GetCrssFromUsr (long UsrCod,long DegCod,MYSQL_RES **mysql_res);

bool Usr_ChkIfEncryptedUsrCodExists (const char *EncryptedUsrCod);

void Usr_WriteFormLoginLogout (void);
void Usr_Logout (void);
void Usr_WriteFormLogin (void);
void Usr_WelcomeUsr (void);
void Usr_PutFormLogIn (void);
void Usr_WriteLoggedUsrHead (void);
void Usr_PutFormLogOut (void);
void Usr_GetParamUsrIdLogin (void);
unsigned Usr_GetParamOtherUsrIDNickOrEMailAndGetUsrCods (struct ListUsrCods *ListUsrCods);

void Usr_PutParamOtherUsrCodEncrypted (void);
void Usr_PutParamUsrCodEncrypted (const char EncryptedUsrCod[Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64+1]);
void Usr_GetParamOtherUsrCodEncrypted (void);
bool Usr_GetParamOtherUsrCodEncryptedAndGetUsrData (void);

void Usr_ChkUsrAndGetUsrData (void);

void Usr_WarningWhenDegreeTypeDoesntAllowDirectLogin (void);
void Usr_ShowFormsLogoutAndRole (void);

bool Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (struct UsrData *UsrDat);
void Usr_UpdateMyLastData (void);
void Usr_InsertMyLastCrsTabAndTime (void);

void Usr_WriteRowStdMainData (unsigned NumUsr,struct UsrData *UsrDat,bool PutCheckboxToSelectUsr);
void Usr_WriteRowStdAllData (struct UsrData *UsrDat,char *GrpNames);
void Usr_WriteRowTchAllData (struct UsrData *UsrDat);
void Usr_RestrictLengthMainData (bool ShowData,struct UsrData *UsrDat,char *MailLink);

unsigned Usr_GetNumUsrsInCrs (Rol_Role_t Role,long CrsCod);
unsigned Usr_GetNumUsrsInCrssOfDeg (Rol_Role_t Role,long DegCod);
unsigned Usr_GetNumUsrsInCrssOfCtr (Rol_Role_t Role,long CtrCod);
unsigned Usr_GetNumUsrsInCrssOfIns (Rol_Role_t Role,long InsCod);
unsigned Usr_GetNumUsrsInCrssOfCty (Rol_Role_t Role,long CtyCod);

long Usr_GetRamdomStdFromCrs (long CrsCod);
long Usr_GetRamdomStdFromGrp (long GrpCod);

unsigned Usr_GetNumTchsCurrentInsInDepartment (long DptCod);
unsigned Usr_GetNumberOfUsersInInstitution (long InsCod,Rol_Role_t Role);
unsigned Usr_GetNumberOfTeachersInCentre (long CtrCod);

void Usr_GetUsrsLst (Rol_Role_t Role,Sco_Scope_t ListUsrsRange,const char *TchQuery,bool Search);
void Usr_GetUnorderedStdsCodesInDeg (long DegCod);
void Usr_FreeUsrsList (struct ListUsers *LstUsrs);
bool Usr_GetIfShowBigList (unsigned NumUsrs);
void Usr_PutHiddenParUsrCodAll (Act_Action_t NextAction,const char *ListUsrCods);
void Usr_GetListsSelectedUsrs (void);
bool Usr_GetListMsgRecipientsWrittenExplicitelyBySender (bool WriteErrorMsgs);
bool Usr_FindUsrCodInListOfSelectedUsrs (const char *EncryptedUsrCodToFind);
unsigned Usr_CountNumUsrsInListOfSelectedUsrs (void);
void Usr_AllocateListSelectedUsrCodAll (void);
void Usr_AllocateListSelectedUsrCodStd (void);
void Usr_AllocateListSelectedUsrCodTch (void);
void Usr_FreeListsSelectedUsrCods (void);
void Usr_FreeListOtherRecipients (void);

void Usr_ShowFormsToSelectUsrListType (Act_Action_t NextAction);
void Usr_PutCheckboxToSelectAllTheUsers (Rol_Role_t Role);
unsigned Usr_GetColumnsForSelectUsrs (void);
void Usr_PutExtraParamsUsrList (Act_Action_t NextAction);
void Usr_ListUsersToSelect (Rol_Role_t Role);

void Usr_ListAllDataGsts (void);
void Usr_ListAllDataStds (void);
void Usr_ListUsrsForSelection (Rol_Role_t Role);
void Usr_ListAllDataTchs (void);
unsigned Usr_ListUsrsFound (Rol_Role_t Role,const char *UsrQuery);
void Usr_ListDataAdms (void);

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

void Usr_GetAndShowNumUsrsInPlatform (Rol_Role_t Role);

bool Usr_CheckIfUsrBanned (long UsrCod);
void Usr_RemoveUsrFromUsrBanned (long UsrCod);

#endif
