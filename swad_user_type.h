// swad_user_type.h: Definition of types for users

#ifndef _SWAD_USR_TYP
#define _SWAD_USR_TYP
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_cookie.h"
#include "swad_cryptography.h"
#include "swad_date.h"
#include "swad_icon.h"
#include "swad_language.h"
#include "swad_mail_type.h"
#include "swad_menu.h"
#include "swad_nickname.h"
#include "swad_password.h"
#include "swad_photo_shape.h"
#include "swad_photo_type.h"
#include "swad_privacy_visibility_type.h"
#include "swad_role_type.h"
#include "swad_search.h"
#include "swad_theme.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Usr_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS  6
#define Usr_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS 12
#define Usr_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS 60

#define Usr_MAX_CHARS_FIRSTNAME_OR_SURNAME	(32 - 1)	// 31
#define Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME	((Usr_MAX_CHARS_FIRSTNAME_OR_SURNAME + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 511

#define Usr_MAX_BYTES_SURNAMES			(Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1 + Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME)
						// Surname1                         +' '+ Surname2
#define Usr_MAX_BYTES_FULL_NAME			(Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1 + Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME +     6    + Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME)
						// Surname1                         +' '+ Surname2                           +  ','+' ' + FirstName
						// Surname1                         +' '+ Surname2                           + '<br />' + FirstName

#define Usr_BIRTHDAY_STR_DB_LENGTH (1 + 4 + 1 + 2 + 1 + 2 + 1)	// "'%04u-%02u-%02u'"

#define Usr_MAX_CHARS_ADDRESS	(128 - 1)	// 127
#define Usr_MAX_BYTES_ADDRESS	((Usr_MAX_CHARS_ADDRESS + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Usr_MAX_CHARS_PHONE	16
#define Usr_MAX_BYTES_PHONE	Usr_MAX_CHARS_PHONE

#define Usr_LIST_WITH_PHOTOS_DEF	true

#define Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS	(Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 * Cfg_MAX_USRS_IN_LIST)

#define Usr_NUM_MAIN_FIELDS_DATA_USR	 8

#define Usr_USER_LIST_SECTION_ID	"user_list"

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

// Get user's data with or without personal settings
typedef enum
  {
   Usr_DONT_GET_PREFS = 0,
   Usr_GET_PREFS      = 1,
  } Usr_GetPrefs_t;

typedef enum
  {
   Usr_DONT_GET_ROLE_IN_CRS = 0,
   Usr_GET_ROLE_IN_CRS      = 1,
  } Usr_GetRoleInCurrentCrs_t;

// Related with user's sexes
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
#define Usr_NUM_CLASS_PHOTO_TYPE 3
typedef enum
  {
   Usr_CLASS_PHOTO_SEL,		// Only for selection of users
   Usr_CLASS_PHOTO_SEL_SEE,	// Selecting and seeing users
   Usr_CLASS_PHOTO_PRN,		// Only print users
  } Usr_ClassPhotoType_t;

#define Usr_LIST_USRS_NUM_OPTIONS 13
typedef enum
  {
   Usr_ACT_UNKNOWN			= 0,

   // Listing users ------------------------
   Usr_ACT_RECORDS			= 1,
   Usr_ACT_ADD_TO_CLIPBOARD		= 2,
   Usr_ACT_OVERWRITE_CLIPBOARD		= 3,
   Usr_ACT_HOMEWORK			= 4,
   Usr_ACT_ATTENDANCE			= 5,
   Usr_ACT_MESSAGE			= 6,
   Usr_ACT_EMAIL			= 7,
   Usr_ACT_FOLLOW			= 8,
   Usr_ACT_UNFOLLOW			= 9,

   // Exam sessions ------------------------
   Usr_ACT_EXAMS_QST_SHEETS		= 10,
   Usr_ACT_BLANK_EXAMS_ANS_SHEETS	= 11,
   Usr_ACT_SOLVD_EXAMS_ANS_SHEETS	= 12,

  } Usr_ListUsrsAction_t;

#define Usr_NUM_CAN 2
typedef enum
  {
   Usr_CAN_NOT = 0,
   Usr_CAN     = 1,
  } Usr_Can_t;

#define Usr_NUM_BELONG 2
typedef enum
  {
   Usr_DONT_BELONG = 0,
   Usr_BELONG      = 1,
  } Usr_Belong_t;

#define Usr_NUM_ACCEPTED 2
typedef enum
  {
   Usr_HAS_NOT_ACCEPTED,
   Usr_HAS_ACCEPTED,
  } Usr_Accepted_t;

#define Usr_NUM_ME_OR_OTHER 2
typedef enum
  {
   Usr_ME,
   Usr_OTHER,
  } Usr_MeOrOther_t;

// #define Usr_NUM_PUT_CHECKBOX 2
typedef enum
  {
   Usr_DONT_PUT_CHECKBOX,
   Usr_PUT_CHECKBOX,
   } Usr_PutCheckBox_t;

#define Usr_NUM_WHO 6
typedef enum
  {
   Usr_WHO_UNKNOWN,
   Usr_WHO_NONE,
   Usr_WHO_ME,
   Usr_WHO_SELECTED,
   Usr_WHO_FOLLOWED,
   Usr_WHO_ALL,
  } Usr_Who_t;
#define Usr_WHO_DEFAULT Usr_WHO_ALL

typedef enum
  {
   Usr_DONT_GET_LIST_ALL_USRS,	// Don't get parameter with list of all users
   Usr_GET_LIST_ALL_USRS,	// Get parameter with list of all users
  } Usr_GetListAllUsrs_t;

// Related with user's data
struct Usr_Data
  {
   long UsrCod;
   char EnUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1];
   char UsrIDNickOrEmail[Cns_MAX_BYTES_USR_LOGIN + 1];	// String to store the ID, nickname or email
   struct
     {
      struct ListIDs *List;
      unsigned Num;
     } IDs;
   char Nickname[Nck_MAX_BYTES_NICK_WITHOUT_ARROBA + 1];
   char Password[Pwd_BYTES_ENCRYPTED_PASSWORD + 1];
   struct
     {
      Rol_Role_t InCurrentCrs;	// Role in current course (Rol_UNK is not filled/calculated or no course selected)
      int InCrss;	// Roles in all his/her courses
			// Check always if filled/calculated
			// >=0 ==> filled/calculated
			//  <0 ==> not yet filled/calculated
     } Roles;
   Usr_Accepted_t Accepted;	// User has accepted joining to current course?
   char Surname1[Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];
   char Surname2[Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];
   char FrstName[Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];
   char FullName[Usr_MAX_BYTES_FULL_NAME + 1];
   Usr_Sex_t Sex;
   char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1];
   Mai_Confirmed_t EmailConfirmed;
   char Photo[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1];	// Name of public link to photo
   Pri_Visibility_t PhotoVisibility;	// Who can see user's photo
   Pri_Visibility_t BaPrfVisibility;	// Who can see user's basic public profile (minimal record card)
   Pri_Visibility_t ExPrfVisibility;	// Who can see user's extended public profile (figures, follow)
   long CtyCod;		// Country
   struct Dat_Date Birthday;
   char StrBirthday[Cns_MAX_BYTES_DATE + 1];
   char Phone[2][Usr_MAX_BYTES_PHONE + 1];
   char *Comments;
   long InsCtyCod;	// Country of the institution
   long InsCod;		// Institution
   struct
     {
      long CtrCod;	// Center
      long DptCod;	// Department
      char Office[Usr_MAX_BYTES_ADDRESS + 1];
      char OfficePhone[Usr_MAX_BYTES_PHONE  + 1];
     } Tch;
   struct
     {
      unsigned CreateNotif;	// One bit activated for each type of event
      unsigned SendEmail;	// One bit activated for each type of event
     } NtfEvents;
   struct
     {
      Lan_Language_t Language;
      unsigned FirstDayOfWeek;
      Dat_Format_t DateFormat;
      The_Theme_t Theme;
      Ico_IconSet_t IconSet;
      Mnu_Menu_t Menu;
      unsigned SideCols;
      PhoSha_Shape_t PhotoShape;
      Coo_RefuseAccept_t AcceptCookies;	// User has accepted third party cookies
     } Prefs;
  };

struct Usr_Last
  {
   Sch_WhatToSearch_t WhatToSearch;	// Search courses, teachers, documents...?
   struct
     {
      Hie_Level_t HieLvl;	// Course, degree, center, etc.
      long HieCod;		// Course code, degree code, center code, etc.
     } LastHie;
   Act_Action_t LastAct;
   Rol_Role_t LastRole;
   long LastTime;
   long LastAccNotif;
  };

struct Usr_InList
  {
   long UsrCod;
   char EnUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1];
   char Password[Pwd_BYTES_ENCRYPTED_PASSWORD + 1];
   char Surname1[Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];
   char Surname2[Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];
   char FrstName[Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];
   Usr_Sex_t Sex;
   char Photo[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1];	// Name of public link to photo
   Pri_Visibility_t PhotoVisibility;				// Who can see user's photo
   long CtyCod;		// Country
   long InsCod;		// Institution
   Rol_Role_t RoleInCurrentCrsDB;	// Role in current course in database
   Usr_Accepted_t Accepted;	// User has accepted joining to one/all courses?
   bool Remove;		// A boolean associated with each user that indicates if he/she must be removed
  };

struct Usr_ListUsrs
  {
   struct Usr_InList *Lst;	// List of users
   unsigned NumUsrs;		// Number of users in the list
  };

typedef enum
  {
   Usr_DONT_USE_LIST_SELECTED_USERS,
   Usr_USE_LIST_SELECTED_USERS,
  } Usr_UseListSelectedUsrs;

struct Usr_SelectedUsrs
  {
   char *List[Rol_NUM_ROLES];	// Lists of encrypted codes of users selected from a form
   bool Filled;			// If lists are already filled/readed
   char *ParSuffix;
   Usr_ListUsrsAction_t Action;	// What action I have selected to do with these selected users
  };

struct Usr_ListUsrCods
  {
   long *Lst;		// List of users' codes
   unsigned NumUsrs;	// Number of users in the list
  };

#endif
