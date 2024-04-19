// swad_user_type.h: definition of types for users

#ifndef _SWAD_USR_TYP
#define _SWAD_USR_TYP
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

#define Usr_BIRTHDAY_STR_DB_LENGTH (1 + 4 + 1 + 2 + 1 + 2 + 1)	// "'%04u-%02u-%02u'"

#define Usr_MAX_CHARS_ADDRESS	(128 - 1)	// 127
#define Usr_MAX_BYTES_ADDRESS	((Usr_MAX_CHARS_ADDRESS + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Usr_MAX_CHARS_PHONE	16
#define Usr_MAX_BYTES_PHONE	Usr_MAX_CHARS_PHONE

#define Usr_CLASS_PHOTO_COLS_DEF	 10	// Default number of columns in a class photo
#define Usr_CLASS_PHOTO_COLS_MAX	100	// Maximum number of columns in a class photo

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

#define Usr_LIST_USRS_NUM_OPTIONS 8
typedef enum
  {
   Usr_OPTION_UNKNOWN		= 0,
   Usr_OPTION_RECORDS		= 1,
   Usr_OPTION_HOMEWORK		= 2,
   Usr_OPTION_ATTENDANCE	= 3,
   Usr_OPTION_MESSAGE		= 4,
   Usr_OPTION_EMAIL		= 5,
   Usr_OPTION_FOLLOW		= 6,
   Usr_OPTION_UNFOLLOW		= 7,
  } Usr_ListUsrsOption_t;
#define Usr_LIST_USRS_DEFAULT_OPTION Usr_OPTION_RECORDS

#define Usr_NUM_CAN 2
typedef enum
  {
   Usr_CAN_NOT,
   Usr_CAN,
  } Usr_Can_t;

#define Usr_NUM_BELONG 2
typedef enum
  {
   Usr_DONT_BELONG,
   Usr_BELONG,
  } Usr_Belong_t;

#define Usr_NUM_ME_OR_OTHER 2
typedef enum
  {
   Usr_ME,
   Usr_OTHER,
  } Usr_MeOrOther_t;

#define Usr_NUM_WHO 5
typedef enum
  {
   Usr_WHO_UNKNOWN,
   Usr_WHO_ME,
   Usr_WHO_SELECTED,
   Usr_WHO_FOLLOWED,
   Usr_WHO_ALL,
  } Usr_Who_t;
#define Usr_WHO_DEFAULT Usr_WHO_ALL

#endif
