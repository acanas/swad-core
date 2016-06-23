// swad_user.c: users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <ctype.h>		// For isalnum, isdigit, etc.
#include <limits.h>		// For maximum values
#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For exit, system, malloc, free, rand, etc.
#include <string.h>		// For string functions
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For access, lstat, getpid, chdir, symlink, unlink

#include "swad_account.h"
#include "swad_announcement.h"
#include "swad_calendar.h"
#include "swad_config.h"
#include "swad_connected.h"
#include "swad_course.h"
#include "swad_database.h"
#include "swad_duplicate.h"
#include "swad_enrollment.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_help.h"
#include "swad_ID.h"
#include "swad_MFU.h"
#include "swad_nickname.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_password.h"
#include "swad_preference.h"
#include "swad_record.h"
#include "swad_tab.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *Usr_StringsSexDB[Usr_NUM_SEXS] =
  {
   "unknown",
   "female",
   "male",
   "all",
   };

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

const char *Usr_StringsUsrListTypeInDB[Usr_NUM_USR_LIST_TYPES] =
  {
   "classphoto",
   "list"
  };
const char *Usr_IconsClassPhotoOrList[Usr_NUM_USR_LIST_TYPES] =
  {
   "classphoto16x16.gif",
   "list64x64.gif"
  };

#define Usr_NUM_MAIN_FIELDS_DATA_ADM	 8
#define Usr_NUM_ALL_FIELDS_DATA_INV	17
#define Usr_NUM_ALL_FIELDS_DATA_STD	13
#define Usr_NUM_ALL_FIELDS_DATA_TCH	11
const char *Usr_UsrDatMainFieldNames[Usr_NUM_MAIN_FIELDS_DATA_USR];

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Usr_GetMyLastData (void);
static void Usr_GetUsrCommentsFromString (char *Str,struct UsrData *UsrDat);
static Usr_Sex_t Usr_GetSexFromStr (const char *Str);

static bool Usr_CheckIfMyBirthdayHasNotBeenCongratulated (void);
static void Usr_InsertMyBirthday (void);

static void Usr_GetParamOtherUsrIDNickOrEMail (void);

static bool Usr_ChkUsrAndGetUsrDataFromDirectLogin (void);
static bool Usr_ChkUsrAndGetUsrDataFromExternalLogin (void);
static bool Usr_ChkUsrAndGetUsrDataFromSession (void);
static void Usr_ShowAlertUsrDoesNotExistsOrWrongPassword (void);
static void Usr_ShowAlertThereAreMoreThanOneUsr (void);
static void Usr_ShowAlertNoUsrWithThisID (void);

static void Usr_SetUsrRoleAndPrefs (void);

static void Usr_InsertMyLastData (void);

static void Usr_WriteRowGstAllData (struct UsrData *UsrDat);
static void Usr_WriteRowStdAllData (struct UsrData *UsrDat,char *GroupNames);
static void Usr_WriteRowTchAllData (struct UsrData *UsrDat);
static void Usr_WriteRowAdmData (unsigned NumUsr,struct UsrData *UsrDat);
static void Usr_RestrictLengthUsrName (struct UsrData *UsrDat);
static void Usr_WriteMainUsrDataExceptUsrID (struct UsrData *UsrDat,
                                             const char *BgColor,
                                             const char *InstitutionName,
                                             const char *InstitutionLink);
static void Usr_WriteUsrData (const char *BgColor,
                              const char *Data,const char *Link,
                              bool NonBreak,bool Accepted);

static void Usr_BuildQueryToGetUsrsLstCrs (Rol_Role_t Role,bool Search,const char *UsrQuery,char *Query);
static void Usr_SearchListUsrs (Rol_Role_t Role,const char *UsrQuery);
static void Usr_GetAdmsLst (Sco_Scope_t Scope);
static void Usr_GetGstsLst (Sco_Scope_t Scope);
static void Usr_GetListUsrsFromQuery (const char *Query,Rol_Role_t Role,Sco_Scope_t Scope);
static void Usr_AllocateUsrsList (Rol_Role_t Role);
static void Usr_ShowWarningListIsTooBig (unsigned NumStds);
static void Usr_PutButtonToConfirmIWantToSeeBigList (unsigned NumUsrs);

static void Usr_AllocateListOtherRecipients (void);

static void Usr_FormToSelectUsrListType (Act_Action_t NextAction,Usr_ShowUsrsType_t ListType);

static Usr_Sex_t Usr_GetSexOfUsrsLst (Rol_Role_t Role);

static void Usr_PutCheckboxToSelectUser (struct UsrData *UsrDat,bool UsrIsTheMsgSender);
static void Usr_PutCheckboxListWithPhotos (void);

static void Usr_ListMainDataGsts (bool PutCheckBoxToSelectUsr);
static void Usr_ListMainDataStds (bool PutCheckBoxToSelectUsr);
static void Usr_ListMainDataTchs (bool PutCheckBoxToSelectUsr);
static void Usr_GetAndUpdateUsrListType (void);
static bool Usr_GetUsrListTypeFromForm (void);
static void Usr_GetMyUsrListTypeFromDB (void);
static void Usr_UpdateMyUsrListTypeInDB (void);

static bool Usr_GetParamColsClassPhotoFromForm (void);
static void Usr_GetMyColsClassPhotoFromDB (void);
static void Usr_UpdateMyColsClassPhotoInDB (void);

static void Usr_GetAndUpdatePrefAboutListWithPhotos (void);
static bool Usr_GetParamListWithPhotosFromForm (void);
static void Usr_UpdateMyPrefAboutListWithPhotosPhotoInDB (void);

static void Usr_PutLinkToSeeAdmins (void);
static void Usr_PutLinkToSeeGuests (void);

static void Usr_PutIconToPrintGsts (void);
static void Usr_PutIconToPrintStds (void);
static void Usr_PutIconToPrintTchs (void);
static void Usr_PutIconToShowGstsAllData (void);
static void Usr_PutIconToShowStdsAllData (void);
static void Usr_PutIconToShowTchsAllData (void);
static void Usr_ShowGstsAllDataParams (void);
static void Usr_ShowStdsAllDataParams (void);
static void Usr_ShowTchsAllDataParams (void);

static void Usr_PutLinkToListOfficialStudents (void);

static void Usr_DrawClassPhoto (Usr_ClassPhotoType_t ClassPhotoType,
                                Rol_Role_t RoleInClassPhoto);

static unsigned Usr_GetNumUsrsNotBelongingToAnyCrs (void);
static float Usr_GetNumCrssPerUsr (Rol_Role_t Role);
static float Usr_GetNumUsrsPerCrs (Rol_Role_t Role);

/*****************************************************************************/
/**** Show alert about number of clicks remaining before sending my photo ****/
/*****************************************************************************/

void Usr_InformAboutNumClicksBeforePhoto (void)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   extern const char *Txt_You_must_send_your_photo_because_;
   extern const char *Txt_You_can_only_perform_X_further_actions_;
   char Message[512];        // Don't use Gbl.Message here, because it may be filled with another message and such message would be overwritten

   if (Gbl.Usrs.Me.NumAccWithoutPhoto)
     {
      if (Gbl.Usrs.Me.NumAccWithoutPhoto >= Pho_MAX_CLICKS_WITHOUT_PHOTO)
         Lay_ShowAlert (Lay_WARNING,Txt_You_must_send_your_photo_because_);
      else if (Act_Actions[Gbl.Action.Act].BrowserWindow == Act_MAIN_WINDOW)
        {
         sprintf (Message,Txt_You_can_only_perform_X_further_actions_,
                  Pho_MAX_CLICKS_WITHOUT_PHOTO-Gbl.Usrs.Me.NumAccWithoutPhoto);
         Lay_ShowAlert (Lay_WARNING,Message);

	 fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
	 Pho_PutLinkToChangeMyPhoto ();
	 fprintf (Gbl.F.Out,"</div>");
        }
     }
  }

/*****************************************************************************/
/************************** Create data for a user ***************************/
/*****************************************************************************/

void Usr_UsrDataConstructor (struct UsrData *UsrDat)
  {
   /***** Allocate memory for the comments *****/
   if ((UsrDat->Comments = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store user's data.");

   /***** Initialize to zero the data of the user *****/
   Usr_ResetUsrDataExceptUsrCodAndIDs (UsrDat);
   UsrDat->IDs.Num = 0;
   UsrDat->IDs.List = NULL;
  }

/*****************************************************************************/
/****************** Reset user's data except UsrCod and ID *******************/
/*****************************************************************************/
// UsrCod and ID are not changed

void Usr_ResetUsrDataExceptUsrCodAndIDs (struct UsrData *UsrDat)
  {
   UsrDat->EncryptedUsrCod[0] = '\0';
   UsrDat->Nickname[0] = '\0';
   UsrDat->Password[0] = '\0';
   UsrDat->RoleInCurrentCrsDB = Rol_UNKNOWN;
   UsrDat->Roles = 0;
   UsrDat->Accepted = true;

   UsrDat->Sex = Usr_SEX_UNKNOWN;
   UsrDat->Surname1[0]  = '\0';
   UsrDat->Surname2[0]  = '\0';
   UsrDat->FirstName[0] = '\0';
   UsrDat->FullName[0]  = '\0';

   UsrDat->Email[0] = '\0';
   UsrDat->EmailConfirmed = false;

   UsrDat->Photo[0] = '\0';
   UsrDat->PhotoVisibility   = Pri_PHOTO_VISIBILITY_DEFAULT;
   UsrDat->ProfileVisibility = Pri_PROFILE_VISIBILITY_DEFAULT;

   UsrDat->CtyCod = -1L;
   UsrDat->OriginPlace[0] = '\0';
   UsrDat->StrBirthday[0] = '\0';
   UsrDat->Birthday.Day   = 0;
   UsrDat->Birthday.Month = 0;
   UsrDat->Birthday.Year  = 0;
   UsrDat->LocalAddress[0]  = '\0';
   UsrDat->LocalPhone[0]    = '\0';
   UsrDat->FamilyAddress[0] = '\0';
   UsrDat->FamilyPhone[0]   = '\0';
   if (UsrDat->Comments)
      UsrDat->Comments[0] = '\0';

   UsrDat->InsCtyCod  = -1L;
   UsrDat->InsCod     = -1L;
   UsrDat->Tch.CtrCod = -1L;
   UsrDat->Tch.DptCod = -1L;
   UsrDat->Tch.Office[0]      = '\0';
   UsrDat->Tch.OfficePhone[0] = '\0';

   UsrDat->Prefs.Language       = Txt_LANGUAGE_UNKNOWN;			// Language unknown
   UsrDat->Prefs.FirstDayOfWeek = Cal_FIRST_DAY_OF_WEEK_DEFAULT;	// Default first day of week
   UsrDat->Prefs.Theme          = The_THEME_DEFAULT;
   UsrDat->Prefs.IconSet        = Ico_ICON_SET_DEFAULT;
   UsrDat->Prefs.Menu           = Mnu_MENU_DEFAULT;
   UsrDat->Prefs.SideCols       = Cfg_DEFAULT_COLUMNS;
   UsrDat->Prefs.EmailNtfEvents = 0;        // By default, don't notify anything
  }

/*****************************************************************************/
/**************************** Reset my last data *****************************/
/*****************************************************************************/

void Usr_ResetMyLastData (void)
  {
   Gbl.Usrs.Me.UsrLast.WhatToSearch = Sch_SEARCH_ALL;
   Gbl.Usrs.Me.UsrLast.LastCrs      = -1L;
   Gbl.Usrs.Me.UsrLast.LastTab      = TabUnk;
   Gbl.Usrs.Me.UsrLast.LastAccNotif = 0;
  }

/*****************************************************************************/
/**************** Free memory used to store the data of a user ***************/
/*****************************************************************************/

void Usr_UsrDataDestructor (struct UsrData *UsrDat)
  {
   /***** Free memory allocated for comments *****/
   if (UsrDat->Comments)
     {
      free ((void *) UsrDat->Comments);
      UsrDat->Comments = NULL;
     }

   /***** Free memory allocated for list of IDs *****/
   ID_FreeListIDs (UsrDat);
  }

/*****************************************************************************/
/************* Get all the user's data from a given user's code **************/
/*****************************************************************************/
// Input: UsrDat->UsrCod must hold user's code

void Usr_GetAllUsrDataFromUsrCod (struct UsrData *UsrDat)
  {
   ID_GetListIDsFromUsrCod (UsrDat);
   Usr_GetUsrDataFromUsrCod (UsrDat);
  }

/*****************************************************************************/
/**************** Allocate memory for the list of users' codes ***************/
/*****************************************************************************/

void Usr_AllocateListUsrCods (struct ListUsrCods *ListUsrCods)
  {
   if ((ListUsrCods->Lst = (long *) malloc (ListUsrCods->NumUsrs * sizeof (long))) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store list of users' codes.");
  }

/*****************************************************************************/
/****************** Free memory for the list of users' codes *****************/
/*****************************************************************************/

void Usr_FreeListUsrCods (struct ListUsrCods *ListUsrCods)
  {
   if (ListUsrCods->NumUsrs && ListUsrCods->Lst)
     {
      free ((void *) ListUsrCods->Lst);
      ListUsrCods->Lst = NULL;
      ListUsrCods->NumUsrs = 0;
     }
  }

/*****************************************************************************/
/******** Get user's code from database using encrypted user's code **********/
/*****************************************************************************/
// Input: UsrDat->EncryptedUsrCod must hold user's encrypted code

void Usr_GetUsrCodFromEncryptedUsrCod (struct UsrData *UsrDat)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   if (UsrDat->EncryptedUsrCod[0])
     {
      /***** Get user's code from database *****/
      sprintf (Query,"SELECT UsrCod FROM usr_data"
	             " WHERE EncryptedUsrCod='%s'",
               UsrDat->EncryptedUsrCod);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get user's code");

      if (NumRows != 1)
         Lay_ShowErrorAndExit ("Error when getting user's code.");

      /***** Get user's code *****/
      row = mysql_fetch_row (mysql_res);
      UsrDat->UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      UsrDat->UsrCod = -1L;
  }

/*****************************************************************************/
/********* Get encrypted user's code from database using user's code *********/
/*****************************************************************************/
// Input: UsrDat->UsrCod must hold user's code

void Usr_GetEncryptedUsrCodFromUsrCod (struct UsrData *UsrDat)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   if (UsrDat->UsrCod > 0)
     {
      /***** Get encrypted user's code from database *****/
      sprintf (Query,"SELECT EncryptedUsrCod FROM usr_data"
	             " WHERE UsrCod='%ld'",
               UsrDat->UsrCod);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get encrypted user's code");

      if (NumRows != 1)
         Lay_ShowErrorAndExit ("Error when getting encrypted user's code.");

      /***** Get encrypted user's code *****/
      row = mysql_fetch_row (mysql_res);
      strncpy (UsrDat->EncryptedUsrCod,row[0],sizeof (UsrDat->EncryptedUsrCod)-1);
      UsrDat->EncryptedUsrCod[sizeof (UsrDat->EncryptedUsrCod)-1] = '\0';

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      UsrDat->EncryptedUsrCod[0] = '\0';
  }

/*****************************************************************************/
/************ Get user's data from database giving a user's code *************/
/*****************************************************************************/
// UsrDat->UsrCod must contain an existing user's code

void Usr_GetUsrDataFromUsrCod (struct UsrData *UsrDat)
  {
   extern const bool Cal_DayIsValidAsFirstDayOfWeek[7];
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   The_Theme_t Theme;
   Ico_IconSet_t IconSet;
   Txt_Language_t Lan;
   unsigned UnsignedNum;
   char StrBirthday[4+1+2+1+2+1];

   /***** Get user's data from database *****/
   sprintf (Query,"SELECT EncryptedUsrCod,Password,Surname1,Surname2,FirstName,Sex,"
                  "Theme,IconSet,Language,FirstDayOfWeek,Photo,PhotoVisibility,ProfileVisibility,"
                  "CtyCod,InsCtyCod,InsCod,DptCod,CtrCod,Office,OfficePhone,"
                  "LocalAddress,LocalPhone,FamilyAddress,FamilyPhone,OriginPlace,Birthday,Comments,"
                  "Menu,SideCols,NotifNtfEvents,EmailNtfEvents"
                  " FROM usr_data WHERE UsrCod='%ld'",
            UsrDat->UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get user's data");

   /***** Check number of rows in result *****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Error when getting user's data.");

   /***** Read user's data *****/
   row = mysql_fetch_row (mysql_res);

   /* Get encrypted user's code */
   strncpy (UsrDat->EncryptedUsrCod,row[0],sizeof (UsrDat->EncryptedUsrCod)-1);
   UsrDat->EncryptedUsrCod[sizeof (UsrDat->EncryptedUsrCod)-1] = '\0';

   /* Get encrypted password */
   strncpy (UsrDat->Password,row[1],sizeof (UsrDat->Password)-1);
   UsrDat->Password[sizeof (UsrDat->Password)-1] = '\0';

   /* Get roles */
   UsrDat->RoleInCurrentCrsDB = Rol_GetRoleInCrs (Gbl.CurrentCrs.Crs.CrsCod,UsrDat->UsrCod);
   UsrDat->Roles = Rol_GetRolesInAllCrss (UsrDat->UsrCod);
   if (UsrDat->RoleInCurrentCrsDB == Rol_UNKNOWN)
      UsrDat->RoleInCurrentCrsDB = (UsrDat->Roles < (1 << Rol_STUDENT)) ?
	                           Rol__GUEST_ :	// User does not belong to any course
	                           Rol_VISITOR;		// User belongs to some courses

   /* Get name */
   strncpy (UsrDat->Surname1 ,row[2],sizeof (UsrDat->Surname1 ) - 1);
   UsrDat->Surname1 [sizeof (UsrDat->Surname1 ) - 1] = '\0';
   strncpy (UsrDat->Surname2 ,row[3],sizeof (UsrDat->Surname2 ) - 1);
   UsrDat->Surname2 [sizeof (UsrDat->Surname2 ) - 1] = '\0';
   strncpy (UsrDat->FirstName,row[4],sizeof (UsrDat->FirstName) - 1);
   UsrDat->FirstName[sizeof (UsrDat->FirstName) - 1] = '\0';

   /* Get sex */
   UsrDat->Sex = Usr_GetSexFromStr (row[5]);

   /* Get theme */
   UsrDat->Prefs.Theme = The_THEME_DEFAULT;
   for (Theme = (The_Theme_t) 0;
        Theme < The_NUM_THEMES;
        Theme++)
      if (!strcasecmp (row[6],The_ThemeId[Theme]))
        {
         UsrDat->Prefs.Theme = Theme;
         break;
        }

   /* Get icon set */
   UsrDat->Prefs.IconSet = Ico_ICON_SET_DEFAULT;
   for (IconSet = (Ico_IconSet_t) 0;
        IconSet < Ico_NUM_ICON_SETS;
        IconSet++)
      if (!strcasecmp (row[7],Ico_IconSetId[IconSet]))
        {
         UsrDat->Prefs.IconSet = IconSet;
         break;
        }

   /* Get language */
   UsrDat->Prefs.Language = Txt_LANGUAGE_UNKNOWN;	// Language unknown
   for (Lan = (Txt_Language_t) 1;
        Lan <= Txt_NUM_LANGUAGES;
        Lan++)
      if (!strcasecmp (row[8],Txt_STR_LANG_ID[Lan]))
        {
         UsrDat->Prefs.Language = Lan;
         break;
        }

   /* Get first day of week */
   UsrDat->Prefs.FirstDayOfWeek = Cal_FIRST_DAY_OF_WEEK_DEFAULT;
   if (sscanf (row[9],"%u",&UnsignedNum) == 1)
      if (Cal_DayIsValidAsFirstDayOfWeek[UnsignedNum])
         UsrDat->Prefs.FirstDayOfWeek = UnsignedNum;

   /* Get rest of data */
   strncpy (UsrDat->Photo,row[10],sizeof (UsrDat->Photo) - 1);
   UsrDat->Photo[sizeof (UsrDat->Photo) - 1] = '\0';
   UsrDat->PhotoVisibility   = Pri_GetVisibilityFromStr (row[11]);
   UsrDat->ProfileVisibility = Pri_GetVisibilityFromStr (row[12]);
   UsrDat->CtyCod    = Str_ConvertStrCodToLongCod (row[13]);
   UsrDat->InsCtyCod = Str_ConvertStrCodToLongCod (row[14]);
   UsrDat->InsCod    = Str_ConvertStrCodToLongCod (row[15]);

   UsrDat->Tch.DptCod = Str_ConvertStrCodToLongCod (row[16]);
   UsrDat->Tch.CtrCod = Str_ConvertStrCodToLongCod (row[17]);
   strncpy (UsrDat->Tch.Office     ,row[18],sizeof (UsrDat->Tch.Office     ) - 1);
   UsrDat->Tch.Office     [sizeof (UsrDat->Tch.Office     ) - 1] = '\0';
   strncpy (UsrDat->Tch.OfficePhone,row[19],sizeof (UsrDat->Tch.OfficePhone) - 1);
   UsrDat->Tch.OfficePhone[sizeof (UsrDat->Tch.OfficePhone) - 1] = '\0';

   strncpy (UsrDat->LocalAddress ,row[20],sizeof (UsrDat->LocalAddress ) - 1);
   UsrDat->LocalAddress [sizeof (UsrDat->LocalAddress ) - 1] = '\0';
   strncpy (UsrDat->LocalPhone   ,row[21],sizeof (UsrDat->LocalPhone   ) - 1);
   UsrDat->LocalPhone   [sizeof (UsrDat->LocalPhone   ) - 1] = '\0';
   strncpy (UsrDat->FamilyAddress,row[22],sizeof (UsrDat->FamilyAddress) - 1);
   UsrDat->FamilyAddress[sizeof (UsrDat->FamilyAddress) - 1] = '\0';
   strncpy (UsrDat->FamilyPhone  ,row[23],sizeof (UsrDat->FamilyPhone  ) - 1);
   UsrDat->FamilyPhone  [sizeof (UsrDat->FamilyPhone  ) - 1] = '\0';
   strncpy (UsrDat->OriginPlace  ,row[24],sizeof (UsrDat->OriginPlace  ) - 1);
   UsrDat->OriginPlace  [sizeof (UsrDat->OriginPlace  ) - 1] = '\0';
   strcpy (StrBirthday,
	   row[25] ? row[25] :
		     "0000-00-00");

   Usr_GetUsrCommentsFromString (row[26] ? row[26] :
	                                   "",
	                         UsrDat);        // Get the comments comunes a todas the courses

   /* Get menu */
   UsrDat->Prefs.Menu = Mnu_MENU_DEFAULT;
   if (sscanf (row[27],"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Mnu_NUM_MENUS)
         UsrDat->Prefs.Menu = (Mnu_Menu_t) UnsignedNum;

   /* Get if user wants to show side columns */
   if (sscanf (row[28],"%u",&UsrDat->Prefs.SideCols) == 1)
     {
      if (UsrDat->Prefs.SideCols > Lay_SHOW_BOTH_COLUMNS)
         UsrDat->Prefs.SideCols = Cfg_DEFAULT_COLUMNS;
     }
   else
      UsrDat->Prefs.SideCols = Cfg_DEFAULT_COLUMNS;

   /* Get on which events I want to be notified inside the platform */
   if (sscanf (row[29],"%u",&UsrDat->Prefs.NotifNtfEvents) != 1)
      UsrDat->Prefs.NotifNtfEvents = (unsigned) -1;	// 0xFF..FF

   /* Get on which events I want to be notified by e-mail */
   if (sscanf (row[30],"%u",&UsrDat->Prefs.EmailNtfEvents) != 1)
      UsrDat->Prefs.EmailNtfEvents = 0;
   if (UsrDat->Prefs.EmailNtfEvents >= (1 << Ntf_NUM_NOTIFY_EVENTS))	// Maximum binary value for NotifyEvents is 000...0011...11
      UsrDat->Prefs.EmailNtfEvents = 0;

   Str_ConvertToTitleType (UsrDat->Surname1 );
   Str_ConvertToTitleType (UsrDat->Surname2 );
   Str_ConvertToTitleType (UsrDat->FirstName);
   /* Create full name using FirstName, Surname1 and Surname2 */
   Usr_BuildFullName (UsrDat);

   if (sscanf (StrBirthday,"%u-%u-%u",
	       &(UsrDat->Birthday.Year),
	       &(UsrDat->Birthday.Month),
	       &(UsrDat->Birthday.Day)) != 3)
      Lay_ShowErrorAndExit ("Wrong date.");
   Dat_ConvDateToDateStr (&(UsrDat->Birthday),UsrDat->StrBirthday);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get nickname and e-mail *****/
   Nck_GetNicknameFromUsrCod (UsrDat->UsrCod,UsrDat->Nickname);
   Mai_GetEmailFromUsrCod (UsrDat);
  }

/*****************************************************************************/
/********* Get the comments in the record of a user from a string ************/
/*****************************************************************************/

static void Usr_GetUsrCommentsFromString (char *Str,struct UsrData *UsrDat)
  {
   /***** Check that memory for comments is allocated *****/
   if (UsrDat->Comments)
     {
      /***** Copy comments from Str to Comments *****/
      strncpy (UsrDat->Comments,Str,Cns_MAX_BYTES_TEXT);
      UsrDat->Comments[Cns_MAX_BYTES_TEXT] = '\0';
     }
  }

/*****************************************************************************/
/********** Get user's last data from database giving a user's code **********/
/*****************************************************************************/

static void Usr_GetMyLastData (void)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned UnsignedNum;

   /***** Get user's data from database *****/
   sprintf (Query,"SELECT WhatToSearch,LastCrs,LastTab,UNIX_TIMESTAMP(LastAccNotif)"
                  " FROM usr_last WHERE UsrCod='%ld'",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get user's last data");

   /***** Check number of rows in result *****/
   if (NumRows == 0)
     {
      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Create entry for me in table of user's last data *****/
      Usr_ResetMyLastData ();
      Usr_InsertMyLastData ();
     }
   else if (NumRows == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get last type of search */
      Gbl.Usrs.Me.UsrLast.WhatToSearch = Sch_SEARCH_ALL;
      if (sscanf (row[0],"%u",&UnsignedNum) == 1)
         if (UnsignedNum < Sch_NUM_WHAT_TO_SEARCH)
            Gbl.Usrs.Me.UsrLast.WhatToSearch = (Sch_WhatToSearch_t) UnsignedNum;

      /* Get last course */
      Gbl.Usrs.Me.UsrLast.LastCrs = Str_ConvertStrCodToLongCod (row[1]);

      /* Get last tab */
      Gbl.Usrs.Me.UsrLast.LastTab = TabPrf;        // By default, set last tab to the profile tab
      if (sscanf (row[2],"%u",&UnsignedNum) == 1)
         if (UnsignedNum >= 1 ||
             UnsignedNum <= Tab_NUM_TABS)
            Gbl.Usrs.Me.UsrLast.LastTab = (Act_Tab_t) UnsignedNum;

      /* Get last access to notifications */
      Gbl.Usrs.Me.UsrLast.LastAccNotif = 0L;
      if (row[3])
         sscanf (row[3],"%ld",&(Gbl.Usrs.Me.UsrLast.LastAccNotif));

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      Lay_ShowErrorAndExit ("Error when getting user's last data.");
  }

/*****************************************************************************/
/****************************** Get sex from string **************************/
/*****************************************************************************/

static Usr_Sex_t Usr_GetSexFromStr (const char *Str)
  {
   Usr_Sex_t Sex;

   for (Sex = (Usr_Sex_t) 0;
        Sex < Usr_NUM_SEXS;
        Sex++)
      if (!strcasecmp (Str,Usr_StringsSexDB[Sex]))
         return Sex;

   return Usr_SEX_UNKNOWN;
  }

/*****************************************************************************/
/********** Build full name using FirstName, Surname1 and Surname2 ***********/
/*****************************************************************************/

void Usr_BuildFullName (struct UsrData *UsrDat)
  {
   strcpy (UsrDat->FullName,UsrDat->FirstName);
   if (UsrDat->Surname1[0])
     {
      strcat (UsrDat->FullName," ");
      strcat (UsrDat->FullName,UsrDat->Surname1);
     }
   if (UsrDat->Surname2[0])
     {
      strcat (UsrDat->FullName," ");
      strcat (UsrDat->FullName,UsrDat->Surname2);
     }
  }

/*****************************************************************************/
/******** Restrict length of firstname and surnames and write then ***********/
/*****************************************************************************/

void Usr_RestrictLengthAndWriteName (const struct UsrData *UsrDat,unsigned MaxChars)
  {
   char FirstName[Usr_MAX_BYTES_NAME_SPEC_CHAR+1];
   char Surnames[2*(Usr_MAX_BYTES_NAME_SPEC_CHAR+1)];

   /***** Restrict length of firstname and surnames *****/
   strcpy (FirstName,UsrDat->FirstName);
   strcpy (Surnames,UsrDat->Surname1);
   if (UsrDat->Surname2[0])
     {
      strcat (Surnames," ");
      strcat (Surnames,UsrDat->Surname2);
     }
   Str_LimitLengthHTMLStr (FirstName,MaxChars);
   Str_LimitLengthHTMLStr (Surnames,MaxChars);

   /***** Write shorted firstname, then return, then shorted surnames *****/
   fprintf (Gbl.F.Out,"%s<br />%s",FirstName,Surnames);
  }

/*****************************************************************************/
/*********** Check if a user is an administrator of an institution ***********/
/*****************************************************************************/

bool Usr_CheckIfUsrIsAdm (long UsrCod,Sco_Scope_t Scope,long Cod)
  {
   extern const char *Sco_ScopeAdminDB[Sco_NUM_SCOPES];
   char Query[128];

   if (Sco_ScopeAdminDB[Scope])
     {
      /***** Get if a user is administrator of a degree from database *****/
      sprintf (Query,"SELECT COUNT(*) FROM admin"
		     " WHERE UsrCod='%ld' AND Scope='%s' AND Cod='%ld'",
	       UsrCod,Sco_ScopeAdminDB[Scope],Cod);
      return (DB_QueryCOUNT (Query,"can not check if a user is administrator") != 0);
     }
   return false;
  }

/*****************************************************************************/
/********************* Check if a user is a superuser ************************/
/*****************************************************************************/

bool Usr_CheckIfUsrIsSuperuser (long UsrCod)
  {
   char Query[128];

   /***** Get if a user is superuser from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM admin"
                  " WHERE UsrCod='%ld' AND Scope='Sys'",
            UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if a user is superuser") != 0);
  }

/*****************************************************************************/
/********************* Get number of courses of a user ***********************/
/*****************************************************************************/

unsigned Usr_GetNumCrssOfUsr (long UsrCod)
  {
   char Query[128];

   /***** Get the number of courses of a user from database ******/
   sprintf (Query,"SELECT COUNT(*) FROM crs_usr"
	          " WHERE UsrCod='%ld'",
            UsrCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of courses of a user");
  }

/*****************************************************************************/
/*************** Get number of courses of a user not accepted ****************/
/*****************************************************************************/

unsigned Usr_GetNumCrssOfUsrNotAccepted (long UsrCod)
  {
   char Query[256];

   /***** Get the number of courses of a user not accepted from database ******/
   sprintf (Query,"SELECT COUNT(*) FROM crs_usr"
	          " WHERE UsrCod='%ld' AND Accepted='N'",
            UsrCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of courses of a user");
  }

/*****************************************************************************/
/********* Get number of courses in with a user have a given role ************/
/*****************************************************************************/

unsigned Usr_GetNumCrssOfUsrWithARole (long UsrCod,Rol_Role_t Role)
  {
   char Query[128];

   /***** Get the number of courses of a user with a role from database ******/
   sprintf (Query,"SELECT COUNT(*) FROM crs_usr"
                  " WHERE UsrCod='%ld' AND Role='%u'",
            UsrCod,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of courses of a user with a role");
  }

/*****************************************************************************/
/********* Get number of courses in with a user have a given role ************/
/*****************************************************************************/

unsigned Usr_GetNumCrssOfUsrWithARoleNotAccepted (long UsrCod,Rol_Role_t Role)
  {
   char Query[128];

   /***** Get the number of courses of a user with a role from database ******/
   sprintf (Query,"SELECT COUNT(*) FROM crs_usr"
                  " WHERE UsrCod='%ld' AND Role='%u' AND Accepted='N'",
            UsrCod,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of courses of a user with a role");
  }

/*****************************************************************************/
/******* Get number of users with different role in courses of a user ********/
/*****************************************************************************/

unsigned Usr_GetNumUsrsInCrssOfAUsr (long UsrCod,Rol_Role_t UsrRole,
                                     Rol_Role_t OthersRole)
  {
   char Query[256];
   unsigned NumUsrs;
   // This query can be made in a unique, but slower, query
   // The temporary table achieves speedup from ~2s to few ms

   /***** Remove temporary table if exists *****/
   sprintf (Query,"DROP TEMPORARY TABLE IF EXISTS my_courses_tmp");
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not remove temporary tables");

   /***** Create temporary table with all my courses for a role *****/
   sprintf (Query,"CREATE TEMPORARY TABLE IF NOT EXISTS my_courses_tmp"
	          " (CrsCod INT NOT NULL,UNIQUE INDEX (CrsCod))"
	          " ENGINE=MEMORY"
	          " SELECT CrsCod FROM crs_usr"
	          " WHERE UsrCod='%ld' AND Role='%u'",
	    UsrCod,(unsigned) UsrRole);
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not create temporary table");

   /***** Get the number of teachers in a course from database ******/
   sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
	          " FROM crs_usr,my_courses_tmp"
                  " WHERE crs_usr.CrsCod=my_courses_tmp.CrsCod"
                  " AND crs_usr.Role='%u'",
            (unsigned) OthersRole);
   NumUsrs = (unsigned) DB_QueryCOUNT (Query,"can not get the number of users");

   /***** Remove temporary table *****/
   sprintf (Query,"DROP TEMPORARY TABLE IF EXISTS my_courses_tmp");
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not remove temporary tables");

   return NumUsrs;
  }

/*****************************************************************************/
/*************** Check if a user belongs to any of my courses ****************/
/*****************************************************************************/

bool Usr_CheckIfUsrSharesAnyOfMyCrs (long UsrCod)
  {
   char Query[512];

   /***** Get if a user shares any course with me from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM crs_usr WHERE UsrCod='%ld'"
                  " AND CrsCod IN"
                  " (SELECT CrsCod FROM crs_usr WHERE UsrCod='%ld')",
            UsrCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if a user shares any course with you") != 0);
  }

/*****************************************************************************/
/*** Check if a user belongs to any of my courses but has a different role ***/
/*****************************************************************************/

bool Usr_CheckIfUsrSharesAnyOfMyCrsWithDifferentRole (long UsrCod)
  {
   char Query[512];

   /***** Get if a user shares any course with me from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM"
                  "(SELECT CrsCod,Role FROM crs_usr WHERE UsrCod='%ld') AS my_courses,"
                  "(SELECT CrsCod,Role FROM crs_usr WHERE UsrCod='%ld') AS usr_courses"
                  " WHERE my_courses.CrsCod=usr_courses.CrsCod"
                  " AND my_courses.Role<>usr_courses.Role",
            Gbl.Usrs.Me.UsrDat.UsrCod,UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if a user shares any course with you") != 0);
  }

/*****************************************************************************/
/** Get all my institutions (those of my courses) and store them in a list ***/
/*****************************************************************************/

void Usr_GetMyInstitutions (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumIns;
   unsigned NumInss;
   long InsCod;

   /***** If my institutions are yet filled, there's nothing to do *****/
   if (!Gbl.Usrs.Me.MyInstitutions.Filled)
     {
      Gbl.Usrs.Me.MyInstitutions.Num = 0;

      /***** Get my institutions from database *****/
      if ((NumInss = (unsigned) Usr_GetInssFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,-1L,&mysql_res)) > 0) // Institutions found
         for (NumIns = 0;
              NumIns < NumInss;
              NumIns++)
           {
            /* Get next institution */
            row = mysql_fetch_row (mysql_res);

            /* Get institution code */
            if ((InsCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
              {
               if (Gbl.Usrs.Me.MyInstitutions.Num == Ins_MAX_INSTITUTIONS_PER_USR)
                  Lay_ShowErrorAndExit ("Maximum number of institutions of a user exceeded.");

               Gbl.Usrs.Me.MyInstitutions.Inss[Gbl.Usrs.Me.MyInstitutions.Num].InsCod  = InsCod;
               Gbl.Usrs.Me.MyInstitutions.Inss[Gbl.Usrs.Me.MyInstitutions.Num].MaxRole = Rol_ConvertUnsignedStrToRole (row[1]);

               Gbl.Usrs.Me.MyInstitutions.Num++;
              }
           }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Set boolean that indicates that my institutions are yet filled *****/
      Gbl.Usrs.Me.MyInstitutions.Filled = true;
     }
  }

/*****************************************************************************/
/***** Get all my centres (those of my courses) and store them in a list *****/
/*****************************************************************************/

void Usr_GetMyCentres (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtr;
   unsigned NumCtrs;
   long CtrCod;

   /***** If my centres are yet filled, there's nothing to do *****/
   if (!Gbl.Usrs.Me.MyCentres.Filled)
     {
      Gbl.Usrs.Me.MyCentres.Num = 0;

      /***** Get my centres from database *****/
      if ((NumCtrs = (unsigned) Usr_GetCtrsFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,-1L,&mysql_res)) > 0) // Centres found
         for (NumCtr = 0;
              NumCtr < NumCtrs;
              NumCtr++)
           {
            /* Get next centre */
            row = mysql_fetch_row (mysql_res);

            /* Get centre code */
            if ((CtrCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
              {
               if (Gbl.Usrs.Me.MyCentres.Num == Ctr_MAX_CENTRES_PER_USR)
                  Lay_ShowErrorAndExit ("Maximum number of centres of a user exceeded.");

               Gbl.Usrs.Me.MyCentres.Ctrs[Gbl.Usrs.Me.MyCentres.Num].CtrCod = CtrCod;
               Gbl.Usrs.Me.MyCentres.Ctrs[Gbl.Usrs.Me.MyCentres.Num].MaxRole = Rol_ConvertUnsignedStrToRole (row[1]);

               Gbl.Usrs.Me.MyCentres.Num++;
              }
           }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Set boolean that indicates that my centres are yet filled *****/
      Gbl.Usrs.Me.MyCentres.Filled = true;
     }
  }

/*****************************************************************************/
/***** Get all my degrees (those of my courses) and store them in a list *****/
/*****************************************************************************/

void Usr_GetMyDegrees (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumDeg;
   unsigned NumDegs;
   long DegCod;

   /***** If my degrees are yet filled, there's nothing to do *****/
   if (!Gbl.Usrs.Me.MyDegrees.Filled)
     {
      Gbl.Usrs.Me.MyDegrees.Num = 0;

      /***** Get my degrees from database *****/
      if ((NumDegs = (unsigned) Usr_GetDegsFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,-1L,&mysql_res)) > 0) // Degrees found
         for (NumDeg = 0;
              NumDeg < NumDegs;
              NumDeg++)
           {
            /* Get next degree */
            row = mysql_fetch_row (mysql_res);

            /* Get degree code */
            if ((DegCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
              {
               if (Gbl.Usrs.Me.MyDegrees.Num == Deg_MAX_DEGREES_PER_USR)
                  Lay_ShowErrorAndExit ("Maximum number of degrees of a user exceeded.");

               Gbl.Usrs.Me.MyDegrees.Degs[Gbl.Usrs.Me.MyDegrees.Num].DegCod  = DegCod;
               Gbl.Usrs.Me.MyDegrees.Degs[Gbl.Usrs.Me.MyDegrees.Num].MaxRole = Rol_ConvertUnsignedStrToRole (row[1]);

               Gbl.Usrs.Me.MyDegrees.Num++;
              }
           }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Set boolean that indicates that my degrees are yet filled *****/
      Gbl.Usrs.Me.MyDegrees.Filled = true;
     }
  }

/*****************************************************************************/
/*************** Get all my courses and store them in a list *****************/
/*****************************************************************************/

void Usr_GetMyCourses (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrs;
   unsigned NumCrss;
   long CrsCod;

   /***** If my courses are yet filled, there's nothing to do *****/
   if (!Gbl.Usrs.Me.MyCourses.Filled)
     {
      Gbl.Usrs.Me.MyCourses.Num = 0;

      /***** Get my courses from database *****/
      if ((NumCrss = (unsigned) Usr_GetCrssFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,-1L,&mysql_res)) > 0) // Courses found
         for (NumCrs = 0;
              NumCrs < NumCrss;
              NumCrs++)
           {
            /* Get next course */
            row = mysql_fetch_row (mysql_res);

            /* Get course code */
            if ((CrsCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
              {
               if (Gbl.Usrs.Me.MyCourses.Num == Crs_MAX_COURSES_PER_USR)
                  Lay_ShowErrorAndExit ("Maximum number of courses of a user exceeded.");

               Gbl.Usrs.Me.MyCourses.Crss[Gbl.Usrs.Me.MyCourses.Num].CrsCod = CrsCod;
               Gbl.Usrs.Me.MyCourses.Crss[Gbl.Usrs.Me.MyCourses.Num].Role   = Rol_ConvertUnsignedStrToRole (row[1]);
               Gbl.Usrs.Me.MyCourses.Crss[Gbl.Usrs.Me.MyCourses.Num].DegCod = Str_ConvertStrCodToLongCod (row[2]);
               Gbl.Usrs.Me.MyCourses.Num++;
              }
           }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Set boolean that indicates that my courses are yet filled *****/
      Gbl.Usrs.Me.MyCourses.Filled = true;
     }
  }

/*****************************************************************************/
/**************** Check if a user belongs to an institution ******************/
/*****************************************************************************/

bool Usr_CheckIfUsrBelongsToIns (long UsrCod,
                                 long InsCod,
                                 bool CountOnlyAcceptedCourses)
  {
   char Query[512];
   const char *SubQuery;

   /***** Trivial case *****/
   if (UsrCod <= 0 ||
       InsCod <= 0)
      return false;

   /***** Get is a user belongs to an institution from database *****/
   SubQuery = (CountOnlyAcceptedCourses ? " AND crs_usr.Accepted='Y'" :
	                                  "");
   sprintf (Query,"SELECT COUNT(DISTINCT centres.InsCod)"
		  " FROM crs_usr,courses,degrees,centres"
		  " WHERE crs_usr.UsrCod='%ld'%s"
		  " AND crs_usr.CrsCod=courses.CrsCod"
		  " AND courses.DegCod=degrees.DegCod"
		  " AND degrees.CtrCod=centres.CtrCod"
		  " AND centres.InsCod='%ld'",
	    UsrCod,SubQuery,InsCod);
   return (DB_QueryCOUNT (Query,"can not check if a user belongs to an institution") != 0);
  }

/*****************************************************************************/
/******************* Check if a user belongs to a centre *********************/
/*****************************************************************************/

bool Usr_CheckIfUsrBelongsToCtr (long UsrCod,
                                 long CtrCod,
                                 bool CountOnlyAcceptedCourses)
  {
   char Query[512];
   const char *SubQuery;

   /***** Trivial case *****/
   if (UsrCod <= 0 ||
       CtrCod <= 0)
      return false;

   /***** Get is a user belongs to a centre from database *****/
   SubQuery = (CountOnlyAcceptedCourses ? " AND crs_usr.Accepted='Y'" :
	                                  "");
   sprintf (Query,"SELECT COUNT(DISTINCT degrees.CtrCod)"
		  " FROM crs_usr,courses,degrees"
		  " WHERE crs_usr.UsrCod='%ld'%s"
		  " AND crs_usr.CrsCod=courses.CrsCod"
		  " AND courses.DegCod=degrees.DegCod"
		  " AND degrees.CtrCod='%ld'",
	    UsrCod,SubQuery,CtrCod);
   return (DB_QueryCOUNT (Query,"can not check if a user belongs to a centre") != 0);
  }

/*****************************************************************************/
/******************* Check if a user belongs to a degree *********************/
/*****************************************************************************/

bool Usr_CheckIfUsrBelongsToDeg (long UsrCod,
                                 long DegCod,
                                 bool CountOnlyAcceptedCourses)
  {
   char Query[512];
   const char *SubQuery;

   /***** Trivial case *****/
   if (UsrCod <= 0 ||
       DegCod <= 0)
      return false;

   /***** Get is a user belongs to a degree from database *****/
   SubQuery = (CountOnlyAcceptedCourses ? " AND crs_usr.Accepted='Y'" :
	                                  "");
   sprintf (Query,"SELECT COUNT(DISTINCT courses.DegCod)"
		  " FROM crs_usr,courses"
		  " WHERE crs_usr.UsrCod='%ld'%s"
		  " AND crs_usr.CrsCod=courses.CrsCod"
		  " AND courses.DegCod='%ld'",
	    UsrCod,SubQuery,DegCod);
   return (DB_QueryCOUNT (Query,"can not check if a user belongs to a degree") != 0);
  }

/*****************************************************************************/
/******************** Check if a user belongs to a course ********************/
/*****************************************************************************/

bool Usr_CheckIfUsrBelongsToCrs (long UsrCod,
                                 long CrsCod,
                                 bool CountOnlyAcceptedCourses)
  {
   char Query[512];
   const char *SubQuery;

   /***** Trivial case *****/
   if (UsrCod <= 0 ||
       CrsCod <= 0)
      return false;

   /***** Get if a user belongs to a course from database *****/
   SubQuery = (CountOnlyAcceptedCourses ? " AND crs_usr.Accepted='Y'" :
	                                  "");
   sprintf (Query,"SELECT COUNT(*) FROM crs_usr"
	          " WHERE CrsCod='%ld' AND UsrCod='%ld'%s",
            CrsCod,UsrCod,SubQuery);
   return (DB_QueryCOUNT (Query,"can not check if a user belongs to a course") != 0);
  }

/*****************************************************************************/
/******************** Check if I belong to an institution ********************/
/*****************************************************************************/
// The list of my institutions must be filled before calling this function

bool Usr_CheckIfIBelongToIns (long InsCod)
  {
   unsigned NumMyIns;

   /***** Fill the list with the institutions I belong to *****/
   Usr_GetMyInstitutions ();

   /***** Check if the institution passed as parameter is any of my institutions *****/
   for (NumMyIns = 0;
        NumMyIns < Gbl.Usrs.Me.MyInstitutions.Num;
        NumMyIns++)
      if (Gbl.Usrs.Me.MyInstitutions.Inss[NumMyIns].InsCod == InsCod)
         return true;
   return false;
  }

/*****************************************************************************/
/*********************** Check if I belong to a centre ***********************/
/*****************************************************************************/
// The list of my centres must be filled before calling this function

bool Usr_CheckIfIBelongToCtr (long CtrCod)
  {
   unsigned NumMyCtr;

   /***** Fill the list with the centres I belong to *****/
   Usr_GetMyCentres ();

   /***** Check if the centre passed as parameter is any of my centres *****/
   for (NumMyCtr = 0;
        NumMyCtr < Gbl.Usrs.Me.MyCentres.Num;
        NumMyCtr++)
      if (Gbl.Usrs.Me.MyCentres.Ctrs[NumMyCtr].CtrCod == CtrCod)
         return true;
   return false;
  }

/*****************************************************************************/
/*********************** Check if I belong to a degree ***********************/
/*****************************************************************************/
// The list of my degrees must be filled before calling this function

bool Usr_CheckIfIBelongToDeg (long DegCod)
  {
   unsigned NumMyDeg;

   /***** Fill the list with the degrees I belong to *****/
   Usr_GetMyDegrees ();

   /***** Check if the degree passed as parameter is any of my degrees *****/
   for (NumMyDeg = 0;
        NumMyDeg < Gbl.Usrs.Me.MyDegrees.Num;
        NumMyDeg++)
      if (Gbl.Usrs.Me.MyDegrees.Degs[NumMyDeg].DegCod == DegCod)
         return true;
   return false;
  }

/*****************************************************************************/
/*********************** Check if I belong to a course ***********************/
/*****************************************************************************/
// The list of my courses must be filled before calling this function

bool Usr_CheckIfIBelongToCrs (long CrsCod)
  {
   unsigned NumMyCrs;

   /***** Fill the list with the courses I belong to *****/
   Usr_GetMyCourses ();

   /***** Check if the course passed as parameter is any of my courses *****/
   for (NumMyCrs = 0;
        NumMyCrs < Gbl.Usrs.Me.MyCourses.Num;
        NumMyCrs++)
      if (Gbl.Usrs.Me.MyCourses.Crss[NumMyCrs].CrsCod == CrsCod)
         return true;

   return false;
  }

/*****************************************************************************/
/**************** Get the countries of a user from database ******************/
/*****************************************************************************/
// Returns the number of rows of the result

unsigned Usr_GetCtysFromUsr (long UsrCod,MYSQL_RES **mysql_res)
  {
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];
   char Query[512];

   /***** Get the institutions a user belongs to from database *****/
   sprintf (Query,"SELECT countries.CtyCod,MAX(crs_usr.Role)"
                  " FROM crs_usr,courses,degrees,centres,institutions,countries"
                  " WHERE crs_usr.UsrCod='%ld'"
                  " AND crs_usr.CrsCod=courses.CrsCod"
                  " AND courses.DegCod=degrees.DegCod"
                  " AND degrees.CtrCod=centres.CtrCod"
                  " AND centres.InsCod=institutions.InsCod"
                  " AND institutions.CtyCod=countries.CtyCod"
                  " GROUP BY countries.CtyCod"
                  " ORDER BY countries.Name_%s",
            UsrCod,Txt_STR_LANG_ID[Gbl.Prefs.Language]);
   return (unsigned) DB_QuerySELECT (Query,mysql_res,"can not get the countries a user belongs to");
  }

/*****************************************************************************/
/************** Get the institutions of a user from database *****************/
/*****************************************************************************/
// Returns the number of rows of the result

unsigned long Usr_GetInssFromUsr (long UsrCod,long CtyCod,MYSQL_RES **mysql_res)
  {
   char Query[512];

   /***** Get the institutions a user belongs to from database *****/
   if (CtyCod > 0)
      sprintf (Query,"SELECT institutions.InsCod,MAX(crs_usr.Role)"
		     " FROM crs_usr,courses,degrees,centres,institutions"
		     " WHERE crs_usr.UsrCod='%ld'"
		     " AND crs_usr.CrsCod=courses.CrsCod"
		     " AND courses.DegCod=degrees.DegCod"
		     " AND degrees.CtrCod=centres.CtrCod"
		     " AND centres.InsCod=institutions.InsCod"
		     " AND institutions.CtyCod='%ld'"
		     " GROUP BY institutions.InsCod"
		     " ORDER BY institutions.ShortName",
	       UsrCod,CtyCod);
   else
      sprintf (Query,"SELECT institutions.InsCod,MAX(crs_usr.Role)"
		     " FROM crs_usr,courses,degrees,centres,institutions"
		     " WHERE crs_usr.UsrCod='%ld'"
		     " AND crs_usr.CrsCod=courses.CrsCod"
		     " AND courses.DegCod=degrees.DegCod"
		     " AND degrees.CtrCod=centres.CtrCod"
		     " AND centres.InsCod=institutions.InsCod"
		     " GROUP BY institutions.InsCod"
		     " ORDER BY institutions.ShortName",
	       UsrCod);
   return DB_QuerySELECT (Query,mysql_res,"can not get the institutions a user belongs to");
  }

/*****************************************************************************/
/***************** Get the centres of a user from database *******************/
/*****************************************************************************/
// Returns the number of rows of the result

unsigned long Usr_GetCtrsFromUsr (long UsrCod,long InsCod,MYSQL_RES **mysql_res)
  {
   char Query[512];

   /***** Get from database the centres a user belongs to *****/
   if (InsCod > 0)
      sprintf (Query,"SELECT centres.CtrCod,MAX(crs_usr.Role)"
                     " FROM crs_usr,courses,degrees,centres"
                     " WHERE crs_usr.UsrCod='%ld'"
                     " AND crs_usr.CrsCod=courses.CrsCod"
                     " AND courses.DegCod=degrees.DegCod"
                     " AND degrees.CtrCod=centres.CtrCod"
                     " AND centres.InsCod='%ld'"
                     " GROUP BY centres.CtrCod ORDER BY centres.ShortName",
               UsrCod,InsCod);
   else
      sprintf (Query,"SELECT degrees.CtrCod,MAX(crs_usr.Role)"
                     " FROM crs_usr,courses,degrees,centres"
                     " WHERE crs_usr.UsrCod='%ld'"
                     " AND crs_usr.CrsCod=courses.CrsCod"
                     " AND courses.DegCod=degrees.DegCod"
                     " AND degrees.CtrCod=centres.CtrCod"
                     " GROUP BY centres.CtrCod ORDER BY centres.ShortName",
               UsrCod);
   return DB_QuerySELECT (Query,mysql_res,"can not check the centres a user belongs to");
  }

/*****************************************************************************/
/***************** Get the degrees of a user from database *******************/
/*****************************************************************************/
// Returns the number of rows of the result

unsigned long Usr_GetDegsFromUsr (long UsrCod,long CtrCod,MYSQL_RES **mysql_res)
  {
   char Query[512];

   /***** Get from database the degrees a user belongs to *****/
   if (CtrCod > 0)
      sprintf (Query,"SELECT degrees.DegCod,MAX(crs_usr.Role)"
                     " FROM crs_usr,courses,degrees"
                     " WHERE crs_usr.UsrCod='%ld' AND crs_usr.CrsCod=courses.CrsCod AND courses.DegCod=degrees.DegCod AND degrees.CtrCod='%ld'"
                     " GROUP BY degrees.DegCod ORDER BY degrees.ShortName",
               UsrCod,CtrCod);
   else
      sprintf (Query,"SELECT degrees.DegCod,MAX(crs_usr.Role)"
                     " FROM crs_usr,courses,degrees"
                     " WHERE crs_usr.UsrCod='%ld' AND crs_usr.CrsCod=courses.CrsCod AND courses.DegCod=degrees.DegCod"
                     " GROUP BY degrees.DegCod ORDER BY degrees.ShortName",
               UsrCod);
   return DB_QuerySELECT (Query,mysql_res,"can not check the degrees a user belongs to");
  }

/*****************************************************************************/
/************** Get all the courses of a user from database ******************/
/*****************************************************************************/
// Returns the number of rows of the result

unsigned long Usr_GetCrssFromUsr (long UsrCod,long DegCod,MYSQL_RES **mysql_res)
  {
   char Query[512];

   /***** Get from database the courses a user belongs to *****/
   if (DegCod > 0)                // Courses in a degree
      sprintf (Query,"SELECT crs_usr.CrsCod,crs_usr.Role,courses.DegCod"
                     " FROM crs_usr,courses"
                     " WHERE crs_usr.UsrCod='%ld' AND crs_usr.CrsCod=courses.CrsCod AND courses.DegCod='%ld'"
                     " ORDER BY courses.ShortName",
               UsrCod,DegCod);
   else                        // All the courses
      sprintf (Query,"SELECT crs_usr.CrsCod,crs_usr.Role,courses.DegCod"
                     " FROM crs_usr,courses,degrees"
                     " WHERE crs_usr.UsrCod='%ld' AND crs_usr.CrsCod=courses.CrsCod AND courses.DegCod=degrees.DegCod"
                     " ORDER BY degrees.ShortName,courses.ShortName",
               UsrCod);
   return DB_QuerySELECT (Query,mysql_res,"can not get the courses a user belongs to");
  }

/*****************************************************************************/
/******** Check if a user exists with a given encrypted user's code **********/
/*****************************************************************************/

bool Usr_ChkIfEncryptedUsrCodExists (const char *EncryptedUsrCod)
  {
   char Query[512];

   /***** Get if an encrypted user's code already existed in database *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_data"
	          " WHERE EncryptedUsrCod='%s'",
            EncryptedUsrCod);
   return (DB_QueryCOUNT (Query,"can not check if an encrypted user's code already existed") != 0);
  }

/*****************************************************************************/
/***************************** Write landing page ****************************/
/*****************************************************************************/

void Usr_WriteLandingPage (void)
  {
   /***** Form to log in *****/
   Usr_WriteFormLogin ();

   /***** Form to go to request the creation of a new account *****/
   Acc_ShowFormGoToRequestNewAccount ();
  }

/*****************************************************************************/
/************************ Write form for user log out ************************/
/*****************************************************************************/

void Usr_WriteFormLogout (void)
  {
   /***** Form to change my role *****/
   Usr_ShowFormsLogoutAndRole ();

   /***** Show help to enroll me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/********************* Message and form shown after log out ******************/
/*****************************************************************************/

void Usr_Logout (void)
  {
   extern const char *Txt_The_session_has_been_closed;

   /***** Confirmation message *****/
   Lay_ShowAlert (Lay_INFO,Txt_The_session_has_been_closed);

   /***** Form to log in *****/
   Usr_WriteFormLogin ();

   /***** Advertisement about mobile app *****/
   Lay_AdvertisementMobile ();
  }

/*****************************************************************************/
/*************************** Put link to log in ******************************/
/*****************************************************************************/

void Usr_PutLinkToLogin (void)
  {
   extern const char *Txt_Log_in;

   Lay_PutContextualLink (ActFrmLogIn,NULL,"login-green64x64.png",
                          Txt_Log_in,Txt_Log_in);
  }

/*****************************************************************************/
/************************ Write form for user log in *************************/
/*****************************************************************************/

void Usr_WriteFormLogin (void)
  {
   extern const char *Txt_Log_in;
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_nick_email_or_ID;
   extern const char *Txt_Password;
   extern const char *Txt_password;

   /***** Links to other actions *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

   Acc_PutLinkToCreateAccount ();
   Pwd_PutLinkToSendNewPasswd ();
   Pre_PutLinkToChangeLanguage ();

   fprintf (Gbl.F.Out,"</div>");

   /***** Start form *****/
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
   Act_FormStart (ActAutUsrInt);
   Lay_StartRoundFrameTable (NULL,2,Txt_Log_in);

   /***** User's ID/nickname and password *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"BM\">"
                      "<img src=\"%s/user64x64.gif\""
                      " alt=\"%s\" title=\"%s\""
	              " class=\"ICON20x20\" />"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" id=\"UsrId\" name=\"UsrId\""
                      " size=\"25\" maxlength=\"%u\""
                      " placeholder=\"%s\" value=\"%s\" />"
                      "</td>"
		      "</tr>"
		      "<tr>"
		      "<td class=\"BM\">"
                      "<img src=\"%s/key64x64.gif\""
                      " alt=\"%s\" title=\"%s\""
	              " class=\"ICON20x20\" />"
		      "</td>"
		      "<td class=\"LEFT_MIDDLE\">"
		      "<input type=\"password\" name=\"UsrPwd\""
		      " size=\"25\" maxlength=\"%u\" placeholder=\"%s\" />"
		      "</td>"
		      "</tr>",
            Gbl.Prefs.IconsURL,
            Txt_User[Usr_SEX_UNKNOWN],
            Txt_User[Usr_SEX_UNKNOWN],
            Usr_MAX_LENGTH_USR_LOGIN,
            Txt_nick_email_or_ID,
            Gbl.Usrs.Me.UsrIdLogin,
            Gbl.Prefs.IconsURL,
            Txt_Password,
            Txt_Password,
            Pwd_MAX_LENGTH_PLAIN_PASSWORD,
            Txt_password);

   /***** Send button and form end *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"CENTER_MIDDLE\">"
                      "<button type=\"submit\" class=\"BT_SUBMIT BT_CONFIRM\">"
                      "%s"
                      "</button>"
                      "</td>"
	              "</tr>",
	              Txt_Log_in);
   Lay_EndRoundFrameTable ();
   Act_FormEnd ();

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******************* Write type and name of logged user **********************/
/*****************************************************************************/

void Usr_WelcomeUsr (void)
  {
   extern const unsigned Txt_Current_CGI_SWAD_Language;
   extern const char *Txt_Happy_birthday;
   extern const char *Txt_Welcome[Usr_NUM_SEXS];
   extern const char *Txt_Welcome_X[Usr_NUM_SEXS];
   extern const char *Txt_Welcome_X_and_happy_birthday[Usr_NUM_SEXS];
   extern const char *Txt_Switching_to_LANGUAGE[1+Txt_NUM_LANGUAGES];
   bool CongratulateMyBirthday;

   if (Gbl.Usrs.Me.Logged)
     {
      if (Gbl.Usrs.Me.UsrDat.Prefs.Language == Txt_Current_CGI_SWAD_Language)
        {
	 fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\""
	                    " style=\"margin:12px;\">");

         /***** User's first name *****/
         if (Gbl.Usrs.Me.UsrDat.FirstName[0])
           {
            CongratulateMyBirthday = false;
            if (Gbl.Usrs.Me.UsrDat.Birthday.Day   == Gbl.Now.Date.Day &&
                Gbl.Usrs.Me.UsrDat.Birthday.Month == Gbl.Now.Date.Month)
               if ((CongratulateMyBirthday = Usr_CheckIfMyBirthdayHasNotBeenCongratulated ()))
                 {
                  Usr_InsertMyBirthday ();
                  fprintf (Gbl.F.Out,"<img src=\"%s/%s/cake128x128.gif\""
                	             " alt=\"%s\" title=\"%s\""
                                     " class=\"ICON160x160\" />",
                           Gbl.Prefs.PathIconSet,Cfg_ICON_128x128,
                           Txt_Happy_birthday,
                           Txt_Happy_birthday);
                  sprintf (Gbl.Message,Txt_Welcome_X_and_happy_birthday[Gbl.Usrs.Me.UsrDat.Sex],
                           Gbl.Usrs.Me.UsrDat.FirstName);
                 }
            if (!CongratulateMyBirthday)
               sprintf (Gbl.Message,Txt_Welcome_X[Gbl.Usrs.Me.UsrDat.Sex],
                        Gbl.Usrs.Me.UsrDat.FirstName);
            Lay_ShowAlert (Lay_INFO,Gbl.Message);
           }
         else
            Lay_ShowAlert (Lay_INFO,Txt_Welcome[Gbl.Usrs.Me.UsrDat.Sex]);

	 /***** Show help to enroll me *****/
	 Hlp_ShowHelpWhatWouldYouLikeToDo ();

	 fprintf (Gbl.F.Out,"</div>");

         /***** Show the global announcements I have not seen *****/
         Ann_ShowMyAnnouncementsNotMarkedAsSeen ();
        }
      else        // The current language is not my preferred language ==> change automatically to my language
         Lay_ShowAlert (Lay_INFO,Txt_Switching_to_LANGUAGE[Gbl.Usrs.Me.UsrDat.Prefs.Language]);
     }
  }

/*****************************************************************************/
/************ Write birthday string to insert or update database *************/
/*****************************************************************************/

void Usr_CreateBirthdayStrDB (const struct UsrData *UsrDat,
                              char BirthdayStrDB[Usr_BIRTHDAY_STR_DB_LENGTH+1])
  {
   if (UsrDat->Birthday.Year  == 0 ||
       UsrDat->Birthday.Month == 0 ||
       UsrDat->Birthday.Day   == 0)
      strcpy (BirthdayStrDB,"NULL");
   else
      sprintf (BirthdayStrDB,"'%04u-%02u-%02u'",
	       UsrDat->Birthday.Year,
	       UsrDat->Birthday.Month,
	       UsrDat->Birthday.Day);
  }

/*****************************************************************************/
/*************** Check if my birthday is already congratulated ***************/
/*****************************************************************************/

static bool Usr_CheckIfMyBirthdayHasNotBeenCongratulated (void)
  {
   char Query[256];

   /***** Delete old birthdays *****/
   sprintf (Query,"SELECT COUNT(*) FROM birthdays_today WHERE UsrCod='%ld'",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if my birthday has been congratulated") == 0);
  }

/*****************************************************************************/
/******** Insert my user's code in table of birthdays already wished *********/
/*****************************************************************************/

static void Usr_InsertMyBirthday (void)
  {
   char Query[256];

   /***** Delete old birthdays *****/
   sprintf (Query,"DELETE FROM birthdays_today WHERE Today<>CURDATE()");
   DB_QueryDELETE (Query,"can not delete old birthdays");

   /***** Insert new birthday *****/
   sprintf (Query,"INSERT INTO birthdays_today (UsrCod,Today) VALUES ('%ld',CURDATE())",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryINSERT (Query,"can not insert birthday");
  }

/*****************************************************************************/
/************************ Write form for user log in *************************/
/*****************************************************************************/

void Usr_PutFormLogIn (void)
  {
   extern const char *The_ClassHead[The_NUM_THEMES];
   extern const char *Txt_Log_in;

   /***** Link to log in form *****/
   Act_FormStart (ActFrmLogIn);
   Act_LinkFormSubmit (Txt_Log_in,The_ClassHead[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"<img src=\"%s/login-green64x64.png\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICON20x20\" />"
                      "<span id=\"login_txt\">&nbsp;%s</span>"
                      "</a>",
            Gbl.Prefs.IconsURL,
            Txt_Log_in,
            Txt_Log_in,
            Txt_Log_in);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************* Write type and name of logged user **********************/
/*****************************************************************************/

void Usr_WriteLoggedUsrHead (void)
  {
   extern const char *The_ClassUsr[The_NUM_THEMES];
   extern const char *The_ClassHead[The_NUM_THEMES];
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];
   char UsrName[Usr_MAX_BYTES_NAME+1];

   /***** User's type *****/
   if (Rol_GetNumAvailableRoles () == 1)
      fprintf (Gbl.F.Out,"<span class=\"%s\">%s:&nbsp;</span>",
               The_ClassUsr[Gbl.Prefs.Theme],
               Txt_ROLES_SINGUL_Abc[Gbl.Usrs.Me.LoggedRole][Gbl.Usrs.Me.UsrDat.Sex]);
   else
     {
      Rol_PutFormToChangeMyRole (true);
      fprintf (Gbl.F.Out,"<span class=\"%s\">&nbsp;</span>",
               The_ClassUsr[Gbl.Prefs.Theme]);
     }

   /***** Show my photo *****/
   ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&Gbl.Usrs.Me.UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&Gbl.Usrs.Me.UsrDat,ShowPhoto ? PhotoURL :
                	                             NULL,
                     "PHOTO18x24",Pho_ZOOM,false);

   /***** User's name *****/
   fprintf (Gbl.F.Out,"<span class=\"%s\">&nbsp;",
            The_ClassUsr[Gbl.Prefs.Theme]);

   /* Name */
   if (Gbl.Usrs.Me.UsrDat.FullName[0])
     {
      strcpy (UsrName,Gbl.Usrs.Me.UsrDat.FirstName);
      Str_LimitLengthHTMLStr (UsrName,12);
      fprintf (Gbl.F.Out,"%s",UsrName);
     }

   fprintf (Gbl.F.Out,"</span>");
  }

/*****************************************************************************/
/*************** Put a form to close current session (log out) ***************/
/*****************************************************************************/

void Usr_PutFormLogOut (void)
  {
   extern const char *The_ClassHead[The_NUM_THEMES];
   extern const char *Txt_Log_out;

   Act_FormStart (ActLogOut);
   Act_LinkFormSubmit (Txt_Log_out,The_ClassHead[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"<img src=\"%s/logout-red64x64.png\""
	              " alt=\"%s\" title=\"%s\""
                      " class=\"ICON20x20\" />"
                      "<span id=\"login_txt\">&nbsp;%s</span>"
                      "</a>",
            Gbl.Prefs.IconsURL,
            Txt_Log_out,
            Txt_Log_out,
            Txt_Log_out);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******* Get parameter with my plain user's ID or nickname from a form *******/
/*****************************************************************************/

void Usr_GetParamUsrIdLogin (void)
  {
   Par_GetParToText ("UsrId",Gbl.Usrs.Me.UsrIdLogin,Usr_MAX_BYTES_USR_LOGIN);
   // Users' IDs are always stored internally in capitals and without leading zeros
   Str_RemoveLeadingZeros (Gbl.Usrs.Me.UsrIdLogin);
   // Str_ConvertToUpperText (Gbl.Usrs.Me.UsrIdLogin);
  }

/*****************************************************************************/
/******* Get parameter user's identificator of other user from a form ********/
/*****************************************************************************/

static void Usr_GetParamOtherUsrIDNickOrEMail (void)
  {
   /***** Get parameter with the plain user's ID, @nick or e-mail of another user *****/
   Par_GetParToText ("OtherUsrIDNickOrEMail",Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail,Usr_MAX_BYTES_USR_LOGIN);

   // If it's a user's ID (if does not contain '@')
   if (strchr (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail,(int) '@') != NULL)	// '@' not found
     {
      // Users' IDs are always stored internally in capitals and without leading zeros
      Str_RemoveLeadingZeros (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail);
      // Str_ConvertToUpperText (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail);
     }
  }

/*****************************************************************************/
/******* Get parameter user's identificator of other user from a form ********/
/******* and get user's data                                          ********/
/*****************************************************************************/
// Returns the number of users for a given ID, @nick or e-mail

unsigned Usr_GetParamOtherUsrIDNickOrEMailAndGetUsrCods (struct ListUsrCods *ListUsrCods)
  {
   extern const char *Txt_The_ID_nickname_or_email_X_is_not_valid;
   bool Wrong = false;

   /***** Reset default list of users' codes *****/
   ListUsrCods->NumUsrs = 0;
   ListUsrCods->Lst = NULL;

   /***** Get parameter with the plain user's ID, @nick or e-mail of another user *****/
   Usr_GetParamOtherUsrIDNickOrEMail ();

   /***** Check if it's an ID, a nickname or an e-mail address *****/
   if (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail[0])
     {
      if (Nck_CheckIfNickWithArrobaIsValid (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail))	// 1: It's a nickname
	{
	 if ((Gbl.Usrs.Other.UsrDat.UsrCod = Nck_GetUsrCodFromNickname (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail)) > 0)
	   {
	    ListUsrCods->NumUsrs = 1;	// One user found
	    Usr_AllocateListUsrCods (ListUsrCods);
	    ListUsrCods->Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;
	   }
	}
      else if (Mai_CheckIfEmailIsValid (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail))		// 2: It's an e-mail
	{
	 if ((Gbl.Usrs.Other.UsrDat.UsrCod = Mai_GetUsrCodFromEmail (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail)) > 0)
	   {
	    ListUsrCods->NumUsrs = 1;	// One user found
	    Usr_AllocateListUsrCods (ListUsrCods);
	    ListUsrCods->Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;
	   }
	}
      else											// 3: It's not a nickname nor e-mail
	{
	 // Users' IDs are always stored internally in capitals and without leading zeros
	 Str_RemoveLeadingZeros (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail);
	 if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail))
	   {
	    /* Allocate space for the list */
	    ID_ReallocateListIDs (&Gbl.Usrs.Other.UsrDat,1);

	    strncpy (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID,Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail,ID_MAX_LENGTH_USR_ID);
	    Gbl.Usrs.Other.UsrDat.IDs.List[0].ID[ID_MAX_LENGTH_USR_ID] = '\0';
	    Str_ConvertToUpperText (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);

	    /* Check if user's ID exists in database */
	    ID_GetListUsrCodsFromUsrID (&Gbl.Usrs.Other.UsrDat,NULL,ListUsrCods,false);
	   }
	 else	// Not a valid user's nickname, e-mail or ID
	    Wrong = true;
	}
     }
   else	// Empty string
      Wrong = true;

   if (Wrong)
     {
      /***** String is not a valid user's nickname, e-mail or ID *****/
      sprintf (Gbl.Message,Txt_The_ID_nickname_or_email_X_is_not_valid,
	       Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }

   return ListUsrCods->NumUsrs;
  }

/*****************************************************************************/
/********* Put hidden parameter encrypted user's code of other user **********/
/*****************************************************************************/

void Usr_PutParamOtherUsrCodEncrypted (void)
  {
   Usr_PutParamUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
  }

void Usr_PutParamUsrCodEncrypted (const char EncryptedUsrCod[Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64+1])
  {
   Par_PutHiddenParamString ("OtherUsrCod",EncryptedUsrCod);
  }

/*****************************************************************************/
/********* Get hidden parameter encrypted user's code of other user **********/
/*****************************************************************************/

void Usr_GetParamOtherUsrCodEncrypted (struct UsrData *UsrDat)
  {
   Par_GetParToText ("OtherUsrCod",UsrDat->EncryptedUsrCod,
                     Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
   if (UsrDat->EncryptedUsrCod[0])        // If parameter exists...
     {
      Usr_GetUsrCodFromEncryptedUsrCod (UsrDat);
      if (UsrDat->UsrCod < 0)        // Check is user's code is valid
         Lay_ShowErrorAndExit ("Wrong user's code.");
     }
   else
      UsrDat->UsrCod = -1L;
  }

/*****************************************************************************/
/********* Get hidden parameter encrypted user's code of other user **********/
/*****************************************************************************/

void Usr_GetParamOtherUsrCodEncryptedAndGetListIDs (void)
  {
   Usr_GetParamOtherUsrCodEncrypted (&Gbl.Usrs.Other.UsrDat);
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)        // If parameter exists...
      ID_GetListIDsFromUsrCod (&Gbl.Usrs.Other.UsrDat);
   else       					 // Parameter does not exist
     {
      Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail[0] = '\0';
      Gbl.Usrs.Other.UsrDat.IDs.Num = 0;
      Gbl.Usrs.Other.UsrDat.IDs.List = NULL;
     }
  }

/*****************************************************************************/
/*** Get parameter encrypted user's code of other user and get user's data ***/
/*****************************************************************************/
// Return true if user exists

bool Usr_GetParamOtherUsrCodEncryptedAndGetUsrData (void)
  {
   /***** Get parameter with encrypted user's code *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   /***** Check if user exists and get her/his data *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))        // Existing user
      return true;

   return false;
  }

/*****************************************************************************/
/** Check and get user data from session, from internal or external login... */
/*****************************************************************************/

void Usr_ChkUsrAndGetUsrData (void)
  {
   extern const char *Txt_The_session_has_expired_due_to_inactivity;
   bool PutFormLogin = false;
   Act_Action_t Action;

   if (Gbl.Session.HasBeenDisconnected)
     {
      if (Gbl.Action.Act != ActRefCon)
	{
	 Gbl.Action.Act = ActLogOut;
	 Tab_SetCurrentTab ();
	 Lay_ShowAlert (Lay_WARNING,Txt_The_session_has_expired_due_to_inactivity);
	 PutFormLogin = true;
	}
     }
   else	// !Gbl.Session.HasBeenDisconnected
     {
      /***** Check user and get user's data *****/
      if (Gbl.Action.Act == ActCreUsrAcc)
	{
	 /***** Create new account and login *****/
	 if (Acc_CreateNewAccountAndLogIn ())			// User logged in
	   {
	    Gbl.Usrs.Me.Logged = true;
	    Usr_SetUsrRoleAndPrefs ();

	    Act_AdjustCurrentAction ();
	    Ses_CreateSession ();

	    /* Set preferences from current IP */
	    Pre_SetPrefsFromIP ();

	    /* Send message via email to confirm the new email address */
	    Mai_SendMailMsgToConfirmEmail ();
	   }
	}
      else	// Gbl.Action.Act != ActCreUsrAcc
	{
	 /***** Check user and get user's data *****/
	 if (Gbl.Session.IsOpen)
	   {
	    if (Usr_ChkUsrAndGetUsrDataFromSession ())		// User logged in
	      {
	       Gbl.Usrs.Me.Logged = true;
	       Usr_SetUsrRoleAndPrefs ();

	       if (Gbl.Action.Act == ActRefCon)	// If refreshing connected users ==> don't refresh session
		  Ses_UpdateSessionLastRefreshInDB ();
	       else
		 {
		  Act_AdjustCurrentAction ();
		  Ses_UpdateSessionDataInDB ();
		  Con_UpdateMeInConnectedList ();
		 }
	      }
	    else
	       PutFormLogin = true;
	   }
	 else if (Gbl.Action.Act == ActAutUsrInt)
	   {
	    if (Usr_ChkUsrAndGetUsrDataFromDirectLogin ())		// User logged in
	      {
	       Gbl.Usrs.Me.Logged = true;
	       Usr_SetUsrRoleAndPrefs ();

	       Act_AdjustCurrentAction ();
	       Ses_CreateSession ();

	       Pre_SetPrefsFromIP ();	// Set preferences from current IP
	      }
	    else
	       PutFormLogin = true;
	   }
	 else if (Gbl.Action.Act == ActAutUsrExt)
	   {
	    if (Usr_ChkUsrAndGetUsrDataFromExternalLogin ())	// User logged in
	      {
	       Gbl.Usrs.Me.Logged = true;
	       Usr_SetUsrRoleAndPrefs ();

	       Act_AdjustCurrentAction ();
	       Ses_CreateSession ();

	       Pre_SetPrefsFromIP ();	// Set preferences from current IP

	       Imp_InsertImpSessionInDB ();
	      }
	    else
	       PutFormLogin = true;
	   }
	}
     }

   /***** If session disconnected or error in login, show form to login *****/
   if (PutFormLogin)
     {
      Usr_WriteFormLogin ();
      Lay_ShowErrorAndExit (NULL);
     }

   /***** Adjust tab and action *****/
   if (Gbl.Action.Act != ActRefCon)
     {
      if (Gbl.Usrs.Me.Logged)
	{
	 /***** Set default tab when unknown *****/
	 if (Gbl.Action.Tab == TabUnk)
	   {
	    // Don't adjust Gbl.Action.Act here
	    Gbl.Action.Tab = ((Gbl.Usrs.Me.UsrLast.LastTab == TabCrs) &&
			      (Gbl.CurrentCrs.Crs.CrsCod <= 0)) ? TabSys :
								  Gbl.Usrs.Me.UsrLast.LastTab;
	    Tab_DisableIncompatibleTabs ();
	   }
	 Usr_UpdateMyLastData ();
	 Crs_UpdateCrsLast ();
	}
      else if (Gbl.Action.Act == ActUnk)	// No user logged and unknown action
	 Act_AdjustActionWhenNoUsrLogged ();

      /***** When I change to another tab, go to the first option allowed *****/
      if (Gbl.Action.Act == ActMnu)
	{
	 if (Gbl.Usrs.Me.Logged)
	   {
	    if ((Action = MFU_GetMyLastActionInCurrentTab ()) == ActUnk)
	       Action = Mnu_GetFirstActionAvailableInCurrentTab ();
	   }
	 else
	    Action = Mnu_GetFirstActionAvailableInCurrentTab ();
	 if (Action != ActUnk)
	    Gbl.Action.Act = Action;
	}
     }
  }

/*****************************************************************************/
/************** Check user and get user's data when direct login *************/
/*****************************************************************************/
// Returns true if user logged in successfully
// Returns false if user not logged in

static bool Usr_ChkUsrAndGetUsrDataFromDirectLogin (void)
  {
   struct ListUsrCods ListUsrCods;
   bool PasswordCorrect = false;

   /***** Check if user typed anything *****/
   if (!Gbl.Usrs.Me.UsrIdLogin)
     {
      Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
      return false;
     }
   if (!Gbl.Usrs.Me.UsrIdLogin[0])
     {
      Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
      return false;
     }

   /***** Check if user has typed his user's ID, his nickname or his e-mail address *****/
   if (Nck_CheckIfNickWithArrobaIsValid (Gbl.Usrs.Me.UsrIdLogin))	// 1: It's a nickname
     {
      // User is trying to log using his/her nickname
      if ((Gbl.Usrs.Me.UsrDat.UsrCod = Nck_GetUsrCodFromNickname (Gbl.Usrs.Me.UsrIdLogin)) <= 0)
	{
	 Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
	 return false;
	}
     }
   else if (Mai_CheckIfEmailIsValid (Gbl.Usrs.Me.UsrIdLogin))		// 2: It's an e-mail
     {
      // User is trying to log using his/her e-mail
      if ((Gbl.Usrs.Me.UsrDat.UsrCod = Mai_GetUsrCodFromEmail (Gbl.Usrs.Me.UsrIdLogin)) <= 0)
	{
	 Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
	 return false;
	}
     }
   else									// 3: It's not a nickname nor e-mail
     {
      // Users' IDs are always stored internally in capitals and without leading zeros
      Str_RemoveLeadingZeros (Gbl.Usrs.Me.UsrIdLogin);
      if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Me.UsrIdLogin))
	{
	 /***** Allocate space for the list *****/
	 ID_ReallocateListIDs (&Gbl.Usrs.Me.UsrDat,1);

	 strncpy (Gbl.Usrs.Me.UsrDat.IDs.List[0].ID,Gbl.Usrs.Me.UsrIdLogin,ID_MAX_LENGTH_USR_ID);
	 Gbl.Usrs.Me.UsrDat.IDs.List[0].ID[ID_MAX_LENGTH_USR_ID] = '\0';
	 Str_ConvertToUpperText (Gbl.Usrs.Me.UsrDat.IDs.List[0].ID);

	 /* Check if user's ID exists in database, and get user's data */
	 if (ID_GetListUsrCodsFromUsrID (&Gbl.Usrs.Me.UsrDat,
	                                 Gbl.Usrs.Me.LoginEncryptedPassword,	// Check password
	                                 &ListUsrCods,false))
	   {
	    if (ListUsrCods.NumUsrs == 1)
	      {
	       /* Free memory used for list of users' codes found for this ID */
	       Usr_FreeListUsrCods (&ListUsrCods);

	       PasswordCorrect = true;
	      }
	    else	// ListUsrCods.NumUsrs > 1
	      {
	       /* Free memory used for list of users' codes found for this ID */
	       Usr_FreeListUsrCods (&ListUsrCods);

	       Usr_ShowAlertThereAreMoreThanOneUsr ();
	       return false;
	      }
	   }
	 else if (ID_GetListUsrCodsFromUsrID (&Gbl.Usrs.Me.UsrDat,
	                                      NULL,				// Don't check password
	                                      &ListUsrCods,false))
	   {
	    if (ListUsrCods.NumUsrs == 1)
	      {
	       /* Free memory used for list of users' codes found for this ID */
	       Usr_FreeListUsrCods (&ListUsrCods);

	       if (Pwd_CheckPendingPassword ())
		 {
	 	  Pwd_AssignMyPendingPasswordToMyCurrentPassword ();
	       	  PasswordCorrect = true;
	         }
	       else
	 	 {
		  Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
	 	  return false;
	 	 }
	      }
	    else	// ListUsrCods.NumUsrs > 1
	      {
	       /* Free memory used for list of users' codes found for this ID */
	       Usr_FreeListUsrCods (&ListUsrCods);

	       Usr_ShowAlertThereAreMoreThanOneUsr ();
	       return false;
	      }
	   }
	 else	// No users found for this ID
	   {
	    Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
	    return false;
	   }
	}
      else	// String is not a valid user's nickname, e-mail or ID
	{
	 Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
	 return false;
	}
     }

   /***** Get user's data *****/
   Usr_GetAllUsrDataFromUsrCod (&Gbl.Usrs.Me.UsrDat);

   /***** Check password *****/
   /* Check user's password:
      is the encrypted password typed by user or coming from the session
      the same as that stored in database? */
   if (!PasswordCorrect)
      PasswordCorrect = Pwd_CheckCurrentPassword ();

   if (!PasswordCorrect)	// If my password is not correct...
     {
      if (Pwd_CheckPendingPassword ())
	 Pwd_AssignMyPendingPasswordToMyCurrentPassword ();
      else
	{
	 Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
	 return false;
	}
     }

   return true;
  }

/*****************************************************************************/
/************ Check user and get user's data when external login *************/
/*****************************************************************************/
// Returns true if user logged in successfully
// Returns false if user not logged in

static bool Usr_ChkUsrAndGetUsrDataFromExternalLogin (void)
  {
   struct ListUsrCods ListUsrCods;
   bool ItSeemsANewUsrIsEnteringFromExternalSite = false;
   char PathRelParamsToCommandsPriv[PATH_MAX+1];
   char FileNameParams[PATH_MAX+1];
   FILE *FileParams;
   char Command[2048];
   int ReturnCode;

   /* Login from external service using ID (not necessarily unique) is not secure.
      Example: Assume a first user who uses an account with private files, messages, etc.
               If he/she is the unique user with this ID confirmed
               or if he/she is the unique user with this ID,
               another second user with the same ID
               would enter from external service into the first user's account.
      The solution would be that external service should use e-mail, that is unique. */

   /***** Copy user's identifier received from external site *****/
   strncpy (Gbl.Usrs.Me.UsrIdLogin,Gbl.Imported.ExternalUsrId,Usr_MAX_BYTES_USR_LOGIN);
   Gbl.Usrs.Me.UsrIdLogin[Usr_MAX_BYTES_USR_LOGIN] = '\0';
   // Users' IDs are always stored internally in capitals and without leading zeros
   Str_RemoveLeadingZeros (Gbl.Usrs.Me.UsrIdLogin);

   /***** Check if user's ID is valid *****/
   if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Me.UsrIdLogin))
     {
      /***** Allocate space for the list *****/
      ID_ReallocateListIDs (&Gbl.Usrs.Me.UsrDat,1);

      strncpy (Gbl.Usrs.Me.UsrDat.IDs.List[0].ID,Gbl.Usrs.Me.UsrIdLogin,ID_MAX_LENGTH_USR_ID);
      Gbl.Usrs.Me.UsrDat.IDs.List[0].ID[ID_MAX_LENGTH_USR_ID] = '\0';
      Str_ConvertToUpperText (Gbl.Usrs.Me.UsrDat.IDs.List[0].ID);

      /* Check if user's ID exists in database, and get user's data */
      if (ID_GetListUsrCodsFromUsrID (&Gbl.Usrs.Me.UsrDat,NULL,&ListUsrCods,true))	// Try first only confirmed IDs
	{
	 if (ListUsrCods.NumUsrs == 1)	// Only one user with this ID confirmed
	   {
	    /* Free memory used for list of users' codes found for this ID */
  	    Usr_FreeListUsrCods (&ListUsrCods);

	    /* Get user's data */
	    Usr_GetAllUsrDataFromUsrCod (&Gbl.Usrs.Me.UsrDat);
	   }
	 else	// ListUsrCods.NumUsrs > 1 ==> more than one user with this ID confirmed
	   {
	    /* Free memory used for list of users' codes found for this ID */
	    Usr_FreeListUsrCods (&ListUsrCods);

	    Usr_ShowAlertThereAreMoreThanOneUsr ();
	    return false;
	   }
	}
      else if (ID_GetListUsrCodsFromUsrID (&Gbl.Usrs.Me.UsrDat,NULL,&ListUsrCods,false))	// If there are no users with this ID confirmed, try with all IDs, not only the confirmed ones
	{
	 if (ListUsrCods.NumUsrs == 1)	// Only one user with this ID
	   {
	    /* Free memory used for list of users' codes found for this ID */
	    Usr_FreeListUsrCods (&ListUsrCods);

	    /* Mark user's ID as confirmed */
	    ID_ConfirmUsrID (&Gbl.Usrs.Me.UsrDat,Gbl.Usrs.Me.UsrIdLogin);

	    /* Get user's data */
	    Usr_GetAllUsrDataFromUsrCod (&Gbl.Usrs.Me.UsrDat);
	   }
	 else	// ListUsrCods.NumUsrs > 1
	   {
	    /* Free memory used for list of users' codes found for this ID */
	    Usr_FreeListUsrCods (&ListUsrCods);

	    Usr_ShowAlertThereAreMoreThanOneUsr ();
	    return false;
	   }
	}
      else
	 ItSeemsANewUsrIsEnteringFromExternalSite = true;
     }
   else	// String is not a valid user's nickname, e-mail or ID
     {
      Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
      return false;
     }

   /***** Validate session:
          the call to SWAD is really coming from external site? *****/
   if (Gbl.Imported.ExternalUsrId[0] &&
       Gbl.Imported.ExternalSesId[0])
     {
      /***** Parameters to command used to import data are passed through a temporary file *****/
      /* If the private directory does not exist, create it */
      sprintf (PathRelParamsToCommandsPriv,"%s/%s",
	       Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PARAM);
      Fil_CreateDirIfNotExists (PathRelParamsToCommandsPriv);

      /* First of all, we remove the oldest temporary files.
	 Such temporary files have been created by me or by other users.
	 This is a bit sloppy, but they must be removed by someone.
	 Here "oldest" means more than x time from their creation */
      Fil_RemoveOldTmpFiles (PathRelParamsToCommandsPriv,Cfg_TIME_TO_DELETE_PARAMS_TO_COMMANDS,false);

      /* Create a new temporary file *****/
      sprintf (FileNameParams,"%s/%s",PathRelParamsToCommandsPriv,Gbl.UniqueNameEncrypted);
      if ((FileParams = fopen (FileNameParams,"wb")) == NULL)
	 Lay_ShowErrorAndExit ("Can not open file to send parameters to command.");
      fprintf (FileParams,"1\n0\n%s\n%s\n",
	       Gbl.Imported.ExternalUsrId,Gbl.Imported.ExternalSesId);
      fclose (FileParams);

      /***** Validate imported session *****/
      sprintf (Command,"%s %s",Cfg_EXTERNAL_LOGIN_CLIENT_COMMAND,FileNameParams);
      ReturnCode = system (Command);
      unlink (FileNameParams);        // File with parameters is no longer necessary
      if (ReturnCode == -1)
	 Lay_ShowErrorAndExit ("Error when running command to validate imported session.");
      ReturnCode = WEXITSTATUS(ReturnCode);
      switch (ReturnCode)
	{
	 case Rol_STUDENT:	// Student
	 case Rol_TEACHER:	// Teacher
	    Gbl.Imported.ExternalRole = (Rol_Role_t) ReturnCode;
	    break;
	 default:
	    sprintf (Gbl.Message,"Error %d while validating session.",ReturnCode);
	    Lay_ShowErrorAndExit (Gbl.Message);
	    break;
	}

      // Now we know than imported session (external login) is valid
      if (ItSeemsANewUsrIsEnteringFromExternalSite)
	{
	 /***** User does not exist in the platform *****/
	 Usr_ShowAlertNoUsrWithThisID ();
         return false;
	}
     }
   else	// External user's ID or session are not valid
     {
      Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
      return false;
     }

   return true;	// User is now logged
  }

/*****************************************************************************/
/******** Check user and get user's data when the session is open ************/
/*****************************************************************************/

static bool Usr_ChkUsrAndGetUsrDataFromSession (void)
  {
   /***** Session is open and user's code is get from session *****/
   Gbl.Usrs.Me.UsrDat.UsrCod = Gbl.Session.UsrCod;

   /* Check if user exists in database, and get his/her data */
   if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Me.UsrDat))
     {
      Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
      return false;
     }

   /* Check user's password:
      the encrypted password typed by user or coming from the session
      is the same as the stored in database? */
   if (!Pwd_CheckCurrentPassword ())	// If my password is not correct...
     {
      Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
      return false;
     }

   return true;
  }

/*****************************************************************************/
/** Show alert indicating that user does not exists or password is incorrect */
/*****************************************************************************/

static void Usr_ShowAlertUsrDoesNotExistsOrWrongPassword (void)
  {
   extern const char *Txt_The_user_does_not_exist_or_password_is_incorrect;

   Gbl.Action.Act = ActFrmLogIn;
   Tab_SetCurrentTab ();
   Lay_ShowAlert (Lay_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
  }

/*****************************************************************************/
/****** Show alert indicating that more than one user share the same ID ******/
/*****************************************************************************/

static void Usr_ShowAlertThereAreMoreThanOneUsr (void)
  {
   extern const char *Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email;

   Gbl.Action.Act = ActFrmLogIn;
   Tab_SetCurrentTab ();
   sprintf (Gbl.Message,Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email,
	    Gbl.Usrs.Me.UsrIdLogin);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);
  }

/*****************************************************************************/
/********** Show alert indicating that this ID does not exist ****************/
/*****************************************************************************/

static void Usr_ShowAlertNoUsrWithThisID (void)
  {
   extern const char *Txt_There_is_no_user_in_X_with_ID_Y_If_you_already_have_an_account_on_Z_;

   Gbl.Action.Act = ActFrmLogIn;
   Tab_SetCurrentTab ();
   sprintf (Gbl.Message,Txt_There_is_no_user_in_X_with_ID_Y_If_you_already_have_an_account_on_Z_,
	    Cfg_PLATFORM_SHORT_NAME,
	    Gbl.Usrs.Me.UsrIdLogin,
	    Cfg_PLATFORM_SHORT_NAME);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);
  }

/*****************************************************************************/
/**** Check if users exists, if his password is correct, get his data... *****/
/*****************************************************************************/

static void Usr_SetUsrRoleAndPrefs (void)
  {
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   bool ICanBeInsAdm = false;
   bool ICanBeCtrAdm = false;
   bool ICanBeDegAdm = false;

   // In this point I am logged

   /***** Set my language if unknown *****/
   if (Gbl.Usrs.Me.UsrDat.Prefs.Language == Txt_LANGUAGE_UNKNOWN)		// I have not chosen language
      Pre_UpdateMyLanguageToCurrentLanguage ();	// Update my language in database

   /***** Set preferences from my preferences *****/
   Gbl.Prefs.FirstDayOfWeek = Gbl.Usrs.Me.UsrDat.Prefs.FirstDayOfWeek;
   Gbl.Prefs.Menu           = Gbl.Usrs.Me.UsrDat.Prefs.Menu;
   Gbl.Prefs.SideCols       = Gbl.Usrs.Me.UsrDat.Prefs.SideCols;

   Gbl.Prefs.Theme = Gbl.Usrs.Me.UsrDat.Prefs.Theme;
   sprintf (Gbl.Prefs.PathTheme,"%s/%s/%s",
	    Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_THEMES,
	    The_ThemeId[Gbl.Prefs.Theme]);

   Gbl.Prefs.IconSet = Gbl.Usrs.Me.UsrDat.Prefs.IconSet;
   sprintf (Gbl.Prefs.PathIconSet,"%s/%s/%s",
	    Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_ICON_SETS,
	    Ico_IconSetId[Gbl.Prefs.IconSet]);

   /***** Get my last data *****/
   Usr_GetMyLastData ();
   if (Gbl.Action.Act == ActAutUsrInt ||
       Gbl.Action.Act == ActAutUsrExt)	// If I just logged in...
     {
      /***** WhatToSearch is stored in session,
             but in login it is got from user's last data *****/
      Gbl.Search.WhatToSearch = Gbl.Usrs.Me.UsrLast.WhatToSearch;

      /***** If no course selected, go to my last visited course *****/
      if (Gbl.CurrentCrs.Crs.CrsCod <= 0 &&
      	  Gbl.Usrs.Me.UsrLast.LastCrs > 0)
      	{
      	 Gbl.CurrentCrs.Crs.CrsCod = Gbl.Usrs.Me.UsrLast.LastCrs;

	 /* Initialize again current course, degree, centre... */
      	 Deg_InitCurrentCourse ();

	 /* Get again my role in this course */
      	 Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB = Rol_GetRoleInCrs (Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);
      	}
     }

   /***** Set the user's role I am logged *****/
   Gbl.Usrs.Me.MaxRole = Rol_GetMaxRole (Gbl.Usrs.Me.UsrDat.Roles);
   Gbl.Usrs.Me.LoggedRole = (Gbl.Usrs.Me.RoleFromSession == Rol_UNKNOWN) ?	// If no logged role retrieved from session...
                            Gbl.Usrs.Me.MaxRole :				// ...set current logged role to maximum role in database
                            Gbl.Usrs.Me.RoleFromSession;			// Get logged role from session

   /***** Construct the path to my directory *****/
   Usr_ConstructPathUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Me.PathDir);

   /***** Check if my photo exists and create a link to it ****/
   Gbl.Usrs.Me.MyPhotoExists = Pho_BuildLinkToPhoto (&Gbl.Usrs.Me.UsrDat,Gbl.Usrs.Me.PhotoURL,true);

   /***** Check if I am administrator of current institution/centre/degree *****/
   if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      /* Check if I am and administrator of current institution */
      ICanBeInsAdm = Usr_CheckIfUsrIsAdm (Gbl.Usrs.Me.UsrDat.UsrCod,
                                          Sco_SCOPE_INS,
                                          Gbl.CurrentIns.Ins.InsCod);
      if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
	{
	 /* Check if I am and administrator of current centre */
	 ICanBeCtrAdm = Usr_CheckIfUsrIsAdm (Gbl.Usrs.Me.UsrDat.UsrCod,
	                                     Sco_SCOPE_CTR,
	                                     Gbl.CurrentCtr.Ctr.CtrCod);
	 if (Gbl.CurrentDeg.Deg.DegCod > 0)
	    /* Check if I am and administrator of current degree */
	    ICanBeDegAdm = Usr_CheckIfUsrIsAdm (Gbl.Usrs.Me.UsrDat.UsrCod,
	                                        Sco_SCOPE_DEG,
	                                        Gbl.CurrentDeg.Deg.DegCod);
	}
     }

   /***** Check if I belong to current course *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
      Gbl.Usrs.Me.IBelongToCurrentCrs = Usr_CheckIfIBelongToCrs (Gbl.CurrentCrs.Crs.CrsCod);
   else
      Gbl.Usrs.Me.IBelongToCurrentCrs = false;
   if (Gbl.Usrs.Me.IBelongToCurrentCrs)
      Gbl.Usrs.Me.UsrDat.Accepted = Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Me.UsrDat.UsrCod,
                                                                Gbl.CurrentCrs.Crs.CrsCod,
                                                                true);
   else
      Gbl.Usrs.Me.UsrDat.Accepted = false;

   /***** Check if I belong to current degree *****/
   if (Gbl.CurrentDeg.Deg.DegCod > 0)
     {
      if (Gbl.Usrs.Me.IBelongToCurrentCrs)
	 Gbl.Usrs.Me.IBelongToCurrentDeg = true;
      else
	 Gbl.Usrs.Me.IBelongToCurrentDeg = Usr_CheckIfIBelongToDeg (Gbl.CurrentDeg.Deg.DegCod);
     }
   else
      Gbl.Usrs.Me.IBelongToCurrentDeg = false;

   /***** Check if I belong to current centre *****/
   if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
     {
      if (Gbl.Usrs.Me.IBelongToCurrentDeg)
         Gbl.Usrs.Me.IBelongToCurrentCtr = true;
      else
         Gbl.Usrs.Me.IBelongToCurrentCtr = Usr_CheckIfIBelongToCtr (Gbl.CurrentCtr.Ctr.CtrCod);
     }
   else
      Gbl.Usrs.Me.IBelongToCurrentCtr = false;

   /***** Check if I belong to current institution *****/
   if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      if (Gbl.Usrs.Me.IBelongToCurrentCtr)
	 Gbl.Usrs.Me.IBelongToCurrentIns = true;
      else
	 Gbl.Usrs.Me.IBelongToCurrentIns = Usr_CheckIfIBelongToIns (Gbl.CurrentIns.Ins.InsCod);
     }
   else
      Gbl.Usrs.Me.IBelongToCurrentIns = false;

   /***** Build my list of available roles for current course *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      if (Gbl.Usrs.Me.IBelongToCurrentCrs)
         Gbl.Usrs.Me.AvailableRoles = (1 << Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB);
      else if (Gbl.Usrs.Me.MaxRole >= Rol_STUDENT)
         Gbl.Usrs.Me.AvailableRoles = (1 << Rol_VISITOR);
      else
         Gbl.Usrs.Me.AvailableRoles = (1 << Rol__GUEST_);
     }
   else        // No course selected
      Gbl.Usrs.Me.AvailableRoles = (1 << Gbl.Usrs.Me.MaxRole);
   if (ICanBeInsAdm)
      Gbl.Usrs.Me.AvailableRoles |= (1 << Rol_INS_ADM);
   if (ICanBeCtrAdm)
      Gbl.Usrs.Me.AvailableRoles |= (1 << Rol_CTR_ADM);
   if (ICanBeDegAdm)
      Gbl.Usrs.Me.AvailableRoles |= (1 << Rol_DEG_ADM);
   if (Usr_CheckIfUsrIsSuperuser (Gbl.Usrs.Me.UsrDat.UsrCod))
      Gbl.Usrs.Me.AvailableRoles |= (1 << Rol_SYS_ADM);

   /***** Check if the role I am logged is now available for me *****/
   if (!(Gbl.Usrs.Me.AvailableRoles & (1 << Gbl.Usrs.Me.LoggedRole)))        // Current type I am logged is not available for me
      /* Set the lowest role available for me */
      for (Gbl.Usrs.Me.LoggedRole = Rol_UNKNOWN;
           Gbl.Usrs.Me.LoggedRole < Rol_NUM_ROLES;
           Gbl.Usrs.Me.LoggedRole++)
         if (Gbl.Usrs.Me.AvailableRoles & (1 << Gbl.Usrs.Me.LoggedRole))
            break;
  }

/*****************************************************************************/
/************** Show forms to log out and to change my role ******************/
/*****************************************************************************/

void Usr_ShowFormsLogoutAndRole (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Log_out;
   extern const char *Txt_You_are_LOGGED_as_X;
   extern const char *Txt_logged[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Role;

   /***** Link to log out *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Lay_PutContextualLink (ActLogOut,NULL,"logout-red64x64.png",
                          Txt_Log_out,Txt_Log_out);
   fprintf (Gbl.F.Out,"</div>");

   /***** Write message with my new logged role *****/
   sprintf (Gbl.Message,Txt_You_are_LOGGED_as_X,
            Txt_logged[Gbl.Usrs.Me.UsrDat.Sex],
            Txt_ROLES_SINGUL_abc[Gbl.Usrs.Me.LoggedRole][Gbl.Usrs.Me.UsrDat.Sex]);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   /***** Put a form to change my role *****/
   if (Rol_GetNumAvailableRoles () > 1)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s CENTER_MIDDLE\">"
	                 "%s: ",
               The_ClassForm[Gbl.Prefs.Theme],Txt_Role);
      Rol_PutFormToChangeMyRole (false);
      fprintf (Gbl.F.Out,"</div>");
     }
  }

/*****************************************************************************/
/******* Check a user's code and get all user's data from user's code ********/
/*****************************************************************************/
// Input: UsrDat->UsrCod must hold a valid user code
// Output: When true ==> UsrDat will hold all user's data
//         When false ==> UsrDat is reset, except user's code

bool Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (struct UsrData *UsrDat)
  {
   /***** Check if a user exists having this user's code *****/
   if (Usr_ChkIfUsrCodExists (UsrDat->UsrCod))
     {
      /* Get user's data */
      Usr_GetAllUsrDataFromUsrCod (UsrDat);
      return true;
     }

   /***** No user's code found *****/
   UsrDat->UsrIDNickOrEmail[0] = '\0';
   Usr_ResetUsrDataExceptUsrCodAndIDs (UsrDat);
   return false;
  }

/*****************************************************************************/
/********* Update my last accessed course, tab and time in database **********/
/*****************************************************************************/

void Usr_UpdateMyLastData (void)
  {
   char Query[256];

   /***** Check if it exists an entry for me *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_last WHERE UsrCod='%ld'",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   if (DB_QueryCOUNT (Query,"can not get last user's click"))
     {
      /***** Update my last accessed course, tab and time of click in database *****/
      // WhatToSearch, LastAccNotif remain unchanged
      sprintf (Query,"UPDATE usr_last SET LastCrs='%ld',LastTab='%u',LastTime=NOW()"
                     " WHERE UsrCod='%ld'",
               Gbl.CurrentCrs.Crs.CrsCod,
               (unsigned) Gbl.Action.Tab,
               Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE (Query,"can not update last user's data");
     }
   else
      Usr_InsertMyLastData ();
  }

/*****************************************************************************/
/*************** Create new entry for my last data in database ***************/
/*****************************************************************************/

static void Usr_InsertMyLastData (void)
  {
   char Query[256];

   /***** Insert my last accessed course, tab and time of click in database *****/
   sprintf (Query,"INSERT INTO usr_last"
	          " (UsrCod,WhatToSearch,LastCrs,LastTab,LastTime,LastAccNotif)"
                  " VALUES"
                  " ('%ld','%u','%ld','%u',NOW(),FROM_UNIXTIME('%ld'))",
            Gbl.Usrs.Me.UsrDat.UsrCod,
            (unsigned) Sch_SEARCH_ALL,
            Gbl.CurrentCrs.Crs.CrsCod,
            (unsigned) Gbl.Action.Tab,
            (long) (time_t) 0);	// The user never accessed to notifications
   DB_QueryINSERT (Query,"can not insert last user's data");
  }

/*****************************************************************************/
/*********** Write a row of a table with the main data of a user *************/
/*****************************************************************************/

void Usr_WriteRowUsrMainData (unsigned NumUsr,struct UsrData *UsrDat,
                              bool PutCheckBoxToSelectUsr)
  {
   extern const char *Txt_Enrollment_confirmed;
   extern const char *Txt_Enrollment_not_confirmed;
   char BgColor[16];
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   bool UsrIsTheMsgSender = PutCheckBoxToSelectUsr &&
	                    (UsrDat->UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);
   struct Institution Ins;

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<tr>");

   /***** Checkbox to select user *****/
   // Two colors are used alternatively to better distinguish the rows
   if (UsrIsTheMsgSender)
      strcpy (BgColor,"LIGHT_GREEN");
   else
      sprintf (BgColor,"COLOR%u",Gbl.RowEvenOdd);

   if (PutCheckBoxToSelectUsr)
     {
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE %s\">",BgColor);
      Usr_PutCheckboxToSelectUser (UsrDat,UsrIsTheMsgSender);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** User has accepted enrollment? *****/
   fprintf (Gbl.F.Out,"<td class=\"");
   if (UsrIsTheMsgSender)
      fprintf (Gbl.F.Out,"BM_SEL");
   else
      fprintf (Gbl.F.Out,"BM%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">"
	              "<img src=\"%s/%s16x16.gif\""
	              " alt=\"%s\" title=\"%s\""
	              " class=\"ICON20x20\" />"
	              "</td>",
            Gbl.Prefs.IconsURL,
            UsrDat->Accepted ? "ok_on" :
        	               "tr",
            UsrDat->Accepted ? Txt_Enrollment_confirmed :
                               Txt_Enrollment_not_confirmed,
            UsrDat->Accepted ? Txt_Enrollment_confirmed :
                               Txt_Enrollment_not_confirmed);

   /***** Write number of user in the list *****/
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
	              "%u"
	              "</td>",
            UsrDat->Accepted ? "USR_LIST_NUM_N" :
        	               "USR_LIST_NUM",
            BgColor,
            NumUsr);

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show user's photo *****/
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE %s\">",BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO21x28",Pho_ZOOM,false);
      fprintf (Gbl.F.Out,"</td>");
     }

   /****** Write user's IDs ******/
   fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE %s\">",
            UsrDat->Accepted ? "DAT_SMALL_N" :
                               "DAT_SMALL",
            BgColor);
   ID_WriteUsrIDs (UsrDat);
   fprintf (Gbl.F.Out,"</td>");

   /***** Write rest of main user's data *****/
   Ins.InsCod = UsrDat->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);
   Usr_RestrictLengthUsrName (UsrDat);
   Usr_WriteMainUsrDataExceptUsrID (UsrDat,BgColor,
                                    Ins.ShortName,Ins.WWW[0] ? Ins.WWW :
                                	                       NULL);

   /***** End row *****/
   fprintf (Gbl.F.Out,"</tr>");
  }

/*****************************************************************************/
/*************** Write a row of a table with the data of a guest *************/
/*****************************************************************************/

static void Usr_WriteRowGstAllData (struct UsrData *UsrDat)
  {
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   struct Institution Ins;
   struct Centre Ctr;
   struct Department Dpt;

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<tr>");

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show guest's photo *****/
      fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE COLOR%u\">",
               Gbl.RowEvenOdd);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO21x28",Pho_NO_ZOOM,false);
      fprintf (Gbl.F.Out,"</td>");
     }

   /****** Write user's ID ******/
   fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL LEFT_MIDDLE COLOR%u\">",
            Gbl.RowEvenOdd);
   ID_WriteUsrIDs (UsrDat);
   fprintf (Gbl.F.Out,"&nbsp;</td>");

   /***** Write rest of guest's main data *****/
   Ins.InsCod = UsrDat->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);
   Usr_WriteMainUsrDataExceptUsrID (UsrDat,Gbl.ColorRows[Gbl.RowEvenOdd],
                                    Ins.ShortName,NULL);

   /***** Write the rest of the data of the guest *****/
   if (UsrDat->Tch.CtrCod > 0)
     {
      Ctr.CtrCod = UsrDat->Tch.CtrCod;
      Ctr_GetDataOfCentreByCod (&Ctr);
     }
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->Tch.CtrCod > 0 ? Ctr.FullName :
	                                      "&nbsp;",
	             NULL,true,false);
   if (UsrDat->Tch.DptCod > 0)
     {
      Dpt.DptCod = UsrDat->Tch.DptCod;
      Dpt_GetDataOfDepartmentByCod (&Dpt);
     }
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->Tch.DptCod > 0 ? Dpt.FullName :
	                                      "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->Tch.Office[0] ? UsrDat->Tch.Office :
	                                     "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->Tch.OfficePhone[0] ? UsrDat->Tch.OfficePhone :
	                                          "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->LocalAddress[0] ? UsrDat->LocalAddress :
	                                       "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->LocalPhone[0] ? UsrDat->LocalPhone :
	                                     "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->FamilyAddress[0] ? UsrDat->FamilyAddress :
	                                        "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->FamilyPhone[0] ? UsrDat->FamilyPhone :
	                                      "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->OriginPlace[0] ? UsrDat->OriginPlace :
	                                      "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->StrBirthday[0] ? UsrDat->StrBirthday :
	                                      "&nbsp;",
	             NULL,true,false);

   /***** End row *****/
   fprintf (Gbl.F.Out,"</tr>");
  }

/*****************************************************************************/
/************ Write a row of a table with the data of a student **************/
/*****************************************************************************/

static void Usr_WriteRowStdAllData (struct UsrData *UsrDat,char *GroupNames)
  {
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   unsigned NumGrpTyp,NumField;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char Text[Cns_MAX_BYTES_TEXT+1];
   struct Institution Ins;
   bool ShowData = (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER && UsrDat->Accepted) ||
                    Gbl.Usrs.Me.LoggedRole >= Rol_DEG_ADM;

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<tr>");

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show student's photo *****/
      fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE COLOR%u\">",
               Gbl.RowEvenOdd);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO21x28",Pho_NO_ZOOM,false);
      fprintf (Gbl.F.Out,"</td>");
     }

   /****** Write user's ID ******/
   fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE COLOR%u\">",
            UsrDat->Accepted ? "DAT_SMALL_N" :
        	               "DAT_SMALL",
            Gbl.RowEvenOdd);
   ID_WriteUsrIDs (UsrDat);
   fprintf (Gbl.F.Out,"&nbsp;</td>");

   /***** Write rest of main student's data *****/
   Ins.InsCod = UsrDat->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);
   Usr_WriteMainUsrDataExceptUsrID (UsrDat,Gbl.ColorRows[Gbl.RowEvenOdd],
                                    Ins.ShortName,NULL);

   /***** Write the rest of the data of the student *****/
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->LocalAddress[0] ? (ShowData ? UsrDat->LocalAddress :
	                                                   "********") :
	                                       "&nbsp;",
	             NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->LocalPhone[0] ? (ShowData ? UsrDat->LocalPhone :
                	                                 "********") :
                	                     "&nbsp;",
	             NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->FamilyAddress[0] ? (ShowData ? UsrDat->FamilyAddress :
                	                                    "********") :
                	                        "&nbsp;",
	             NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->FamilyPhone[0] ? (ShowData ? UsrDat->FamilyPhone :
                	                                  "********") :
                	                      "&nbsp;",
	             NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->OriginPlace[0] ? (ShowData ? UsrDat->OriginPlace :
                	                                  "********") :
                	                      "&nbsp;",
	             NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     UsrDat->StrBirthday[0] ? (ShowData ? UsrDat->StrBirthday :
                	                                  "********") :
                	                      "&nbsp;",
	             NULL,true,UsrDat->Accepted);

   if (Gbl.Scope.Current == Sco_SCOPE_CRS)
     {
      /***** Write the groups a the que pertenece the student *****/
      for (NumGrpTyp = 0;
           NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
           NumGrpTyp++)
         if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)         // If current course tiene groups of este type
           {
            Grp_GetNamesGrpsStdBelongsTo (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod,UsrDat->UsrCod,GroupNames);
            Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],GroupNames,NULL,true,UsrDat->Accepted);
           }

      /***** Fields of the record dependientes of the course *****/
      for (NumField = 0;
           NumField < Gbl.CurrentCrs.Records.LstFields.Num;
           NumField++)
        {
         /* Get the text of the field */
         if (Rec_GetFieldFromCrsRecord (UsrDat->UsrCod,Gbl.CurrentCrs.Records.LstFields.Lst[NumField].FieldCod,&mysql_res))
           {
            row = mysql_fetch_row (mysql_res);
            strcpy (Text,row[0]);
            Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                              Text,Cns_MAX_BYTES_TEXT,false);        // Se convierte of HTML a HTML respetuoso
           }
         else
            Text[0] = '\0';
         Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],Text,NULL,false,UsrDat->Accepted);

         /* Free structure that stores the query result */
         DB_FreeMySQLResult (&mysql_res);
        }
     }

   /***** End row *****/
   fprintf (Gbl.F.Out,"</tr>");
  }

/*****************************************************************************/
/*** Write a row of a table with the data of a teacher or an administrator ***/
/*****************************************************************************/

static void Usr_WriteRowTchAllData (struct UsrData *UsrDat)
  {
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   struct Institution Ins;
   bool ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == UsrDat->UsrCod);
   bool ShowData = (ItsMe || UsrDat->Accepted ||
                    Gbl.Usrs.Me.LoggedRole == Rol_DEG_ADM ||
                    Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);
   struct Centre Ctr;
   struct Department Dpt;

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<tr>");
   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show teacher's photo *****/
      fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE COLOR%u\">",
               Gbl.RowEvenOdd);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO21x28",Pho_NO_ZOOM,false);
      fprintf (Gbl.F.Out,"</td>");
     }

   /****** Write the user's ID ******/
   fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE COLOR%u\">",
            UsrDat->Accepted ? "DAT_SMALL_N" :
                               "DAT_SMALL",
            Gbl.RowEvenOdd);
   ID_WriteUsrIDs (UsrDat);
   fprintf (Gbl.F.Out,"&nbsp;</td>");

   /***** Write rest of main teacher's data *****/
   Ins.InsCod = UsrDat->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);
   Usr_WriteMainUsrDataExceptUsrID (UsrDat,Gbl.ColorRows[Gbl.RowEvenOdd],
                                    Ins.ShortName,NULL);

   /***** Write the rest of teacher's data *****/
   if (ShowData && UsrDat->Tch.CtrCod > 0)
     {
      Ctr.CtrCod = UsrDat->Tch.CtrCod;
      Ctr_GetDataOfCentreByCod (&Ctr);
     }
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     (ShowData && UsrDat->Tch.CtrCod > 0) ? Ctr.FullName :
                	                                    "&nbsp;",
                     NULL,true,UsrDat->Accepted);
   if (ShowData && UsrDat->Tch.DptCod > 0)
     {
      Dpt.DptCod = UsrDat->Tch.DptCod;
      Dpt_GetDataOfDepartmentByCod (&Dpt);
     }
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     (ShowData && UsrDat->Tch.DptCod > 0) ? Dpt.FullName :
                	                                    "&nbsp;",
                     NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     (ShowData && UsrDat->Tch.Office[0]) ? UsrDat->Tch.Office :
                	                                   "&nbsp;",
                     NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (Gbl.ColorRows[Gbl.RowEvenOdd],
                     (ShowData && UsrDat->Tch.OfficePhone[0]) ? UsrDat->Tch.OfficePhone :
                	                                        "&nbsp;",
                     NULL,true,UsrDat->Accepted);

   fprintf (Gbl.F.Out,"</tr>");
  }

/*****************************************************************************/
/********** Write a row of a table with the data of an administrator *********/
/*****************************************************************************/

static void Usr_WriteRowAdmData (unsigned NumUsr,struct UsrData *UsrDat)
  {
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   struct Institution Ins;

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<tr>");

   /***** Write number of user *****/
   fprintf (Gbl.F.Out,"<td class=\"USR_LIST_NUM_N CENTER_MIDDLE COLOR%u\">"
	              "%u"
	              "</td>",
            Gbl.RowEvenOdd,NumUsr);

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show administrator's photo *****/
      fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE COLOR%u\">",
               Gbl.RowEvenOdd);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO21x28",Pho_ZOOM,false);
      fprintf (Gbl.F.Out,"</td>");
     }

   /****** Write the user's ID ******/
   fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE COLOR%u\">",
            UsrDat->Accepted ? "DAT_SMALL_N" :
                               "DAT_SMALL",
            Gbl.RowEvenOdd);
   ID_WriteUsrIDs (UsrDat);
   fprintf (Gbl.F.Out,"&nbsp;</td>");

   /***** Write rest of main administrator's data *****/
   Ins.InsCod = UsrDat->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);
   Usr_RestrictLengthUsrName (UsrDat);
   Usr_WriteMainUsrDataExceptUsrID (UsrDat,Gbl.ColorRows[Gbl.RowEvenOdd],
                                    Ins.ShortName,Ins.WWW[0] ? Ins.WWW :
                                	                       NULL);
   fprintf (Gbl.F.Out,"</tr>");

   /***** Write degrees which are administrated by this administrator *****/
   Deg_GetAndWriteInsCtrDegAdminBy (UsrDat->UsrCod,
                                    Gbl.Usrs.Listing.WithPhotos ? Usr_NUM_MAIN_FIELDS_DATA_ADM :
                                	                          Usr_NUM_MAIN_FIELDS_DATA_ADM-1);
  }

/*****************************************************************************/
/***************** Restrict the length of the user's name ********************/
/*****************************************************************************/

static void Usr_RestrictLengthUsrName (struct UsrData *UsrDat)
  {
   Str_LimitLengthHTMLStr (UsrDat->FirstName,10);
   Str_LimitLengthHTMLStr (UsrDat->Surname1,10);
   Str_LimitLengthHTMLStr (UsrDat->Surname2,10);
  }

/*****************************************************************************/
/************************* Write main data of a user *************************/
/*****************************************************************************/

static void Usr_WriteMainUsrDataExceptUsrID (struct UsrData *UsrDat,
                                             const char *BgColor,
                                             const char *InstitutionName,
                                             const char *InstitutionLink)
  {
   bool ShowEmail;
   char MailLink[7+Usr_MAX_BYTES_USR_EMAIL+1];	// mailto:mail_address

   Usr_WriteUsrData (BgColor,
                     UsrDat->Surname1[0] ? UsrDat->Surname1 :
                	                   "&nbsp;",
                     NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (BgColor,
                     UsrDat->Surname2[0] ? UsrDat->Surname2 :
                	                   "&nbsp;",
                     NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (BgColor,
                     UsrDat->FirstName[0] ? UsrDat->FirstName :
                	                    "&nbsp;",
                     NULL,true,UsrDat->Accepted);
   if (UsrDat->Email[0])
     {
      ShowEmail = Mai_ICanSeeEmail (UsrDat);
      sprintf (MailLink,"mailto:%s",UsrDat->Email);
      Str_LimitLengthHTMLStr (UsrDat->Email,10);
     }
   else
      ShowEmail = false;
   Usr_WriteUsrData (BgColor,
                     UsrDat->Email[0] ? (ShowEmail ? UsrDat->Email :
                	                             "********") :
                	                "&nbsp;",
                     ShowEmail ? MailLink :
                	         NULL,
                     true,UsrDat->Accepted);
   Usr_WriteUsrData (BgColor,
                     UsrDat->InsCod > 0 ? InstitutionName :
                	                  "&nbsp;",
                     InstitutionLink,true,UsrDat->Accepted);
  }

/*****************************************************************************/
/********************* Write a cell with data of a user **********************/
/*****************************************************************************/

static void Usr_WriteUsrData (const char *BgColor,
                              const char *Data,const char *Link,
                              bool NonBreak,bool Accepted)
  {
   fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE %s\">",
            Accepted ? (NonBreak ? "DAT_SMALL_NOBR_N" :
        	                   "DAT_SMALL_N") :
                       (NonBreak ? "DAT_SMALL_NOBR" :
                	           "DAT_SMALL"),
            BgColor);
   if (Link != NULL)
      fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\">",Link);
   fprintf (Gbl.F.Out,"%s",Data);
   if (NonBreak)
      fprintf (Gbl.F.Out,"&nbsp;");
   if (Link != NULL)
      fprintf (Gbl.F.Out,"</a>");
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/************* Get number of users with a role in a course *******************/
/*****************************************************************************/

unsigned Usr_GetNumUsrsInCrs (Rol_Role_t Role,long CrsCod)
  {
   char Query[512];

   /***** Get the number of teachers in a course from database ******/
   sprintf (Query,"SELECT COUNT(*) FROM crs_usr"
                  " WHERE CrsCod='%ld' AND Role='%u'",
            CrsCod,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of users in a course");
  }

/*****************************************************************************/
/*********** Count how many users with a role belong to a degree *************/
/*****************************************************************************/

unsigned Usr_GetNumUsrsInCrssOfDeg (Rol_Role_t Role,long DegCod)
  {
   char Query[512];

   /***** Get the number of users in courses of a degree from database ******/
   sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
	          " FROM courses,crs_usr"
                  " WHERE courses.DegCod='%ld'"
                  " AND courses.CrsCod=crs_usr.CrsCod"
                  " AND crs_usr.Role='%u'",
            DegCod,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of users in courses of a degree");
  }

/*****************************************************************************/
/************ Count how many users with a role belong to a centre ************/
/*****************************************************************************/
// Here Rol_UNKNOWN means students or teachers

unsigned Usr_GetNumUsrsInCrssOfCtr (Rol_Role_t Role,long CtrCod)
  {
   char Query[512];

   /***** Get the number of users in courses of a centre from database ******/
   if (Role == Rol_UNKNOWN)	// Any user
      sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
		     " FROM degrees,courses,crs_usr"
		     " WHERE degrees.CtrCod='%ld'"
		     " AND degrees.DegCod=courses.DegCod"
		     " AND courses.CrsCod=crs_usr.CrsCod",
	       CtrCod);
   else
      // This query is very slow.
      // It's a bad idea to get number of teachers or students for a big list of centres
      sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
		     " FROM degrees,courses,crs_usr"
		     " WHERE degrees.CtrCod='%ld'"
		     " AND degrees.DegCod=courses.DegCod"
		     " AND courses.CrsCod=crs_usr.CrsCod"
		     " AND crs_usr.Role='%u'",
	       CtrCod,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of users in courses of a centre");
  }

/*****************************************************************************/
/********* Count how many users with a role belong to an institution *********/
/*****************************************************************************/
// Here Rol_UNKNOWN means students or teachers

unsigned Usr_GetNumUsrsInCrssOfIns (Rol_Role_t Role,long InsCod)
  {
   char Query[512];

   /***** Get the number of users in courses of an institution from database ******/
   if (Role == Rol_UNKNOWN)	// Any user
      sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
		     " FROM centres,degrees,courses,crs_usr"
		     " WHERE centres.InsCod='%ld'"
		     " AND centres.CtrCod=degrees.CtrCod"
		     " AND degrees.DegCod=courses.DegCod"
		     " AND courses.CrsCod=crs_usr.CrsCod",
	       InsCod);
   else
      // This query is very slow.
      // It's a bad idea to get number of teachers or students for a big list of institutions
      sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
		     " FROM centres,degrees,courses,crs_usr"
		     " WHERE centres.InsCod='%ld'"
		     " AND centres.CtrCod=degrees.CtrCod"
		     " AND degrees.DegCod=courses.DegCod"
		     " AND courses.CrsCod=crs_usr.CrsCod"
		     " AND crs_usr.Role='%u'",
	       InsCod,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of users in courses of an institution");
  }

/*****************************************************************************/
/****** Count how many users with a role belong to courses of a country ******/
/*****************************************************************************/
// Here Rol_UNKNOWN means students or teachers

unsigned Usr_GetNumUsrsInCrssOfCty (Rol_Role_t Role,long CtyCod)
  {
   char Query[512];

   /***** Get the number of users in courses of a country from database ******/
   if (Role == Rol_UNKNOWN)	// Any user
      sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
		     " FROM institutions,centres,degrees,courses,crs_usr"
		     " WHERE institutions.CtyCod='%ld'"
		     " AND institutions.InsCod=centres.InsCod"
		     " AND centres.CtrCod=degrees.CtrCod"
		     " AND degrees.DegCod=courses.DegCod"
		     " AND courses.CrsCod=crs_usr.CrsCod",
	       CtyCod);
   else
      // This query is very slow.
      // It's a bad idea to get number of teachers or students for a big list of countries
      sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
		     " FROM institutions,centres,degrees,courses,crs_usr"
		     " WHERE institutions.CtyCod='%ld'"
		     " AND institutions.InsCod=centres.InsCod"
		     " AND centres.CtrCod=degrees.CtrCod"
		     " AND degrees.DegCod=courses.DegCod"
		     " AND courses.CrsCod=crs_usr.CrsCod"
		     " AND crs_usr.Role='%u'",
	       CtyCod,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of users in courses of a country");
  }

/*****************************************************************************/
/******** Get the user's code of a random student from current course ********/
/*****************************************************************************/
// Returns user's code or -1 if no user found

long Usr_GetRamdomStdFromCrs (long CrsCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long UsrCod = -1L;	// -1 means user not found

   /***** Get a random student from current course from database *****/
   sprintf (Query,"SELECT UsrCod FROM crs_usr"
                  " WHERE CrsCod='%ld' AND Role='%u'"
                  " ORDER BY RAND(NOW()) LIMIT 1",
            CrsCod,(unsigned) Rol_STUDENT);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get a random student from the current course"))
     {
      /***** Get user code *****/
      row = mysql_fetch_row (mysql_res);
      UsrCod = Str_ConvertStrCodToLongCod (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return UsrCod;
  }

/*****************************************************************************/
/*********** Get the user's code of a random student from a group ************/
/*****************************************************************************/
// Returns user's code or -1 if no user found

long Usr_GetRamdomStdFromGrp (long GrpCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long UsrCod = -1L;	// -1 means user not found

   /***** Get a random student from a group from database *****/
   sprintf (Query,"SELECT crs_grp_usr.UsrCod FROM crs_grp_usr,crs_usr"
                  " WHERE crs_grp_usr.GrpCod='%ld' AND crs_grp_usr.UsrCod=crs_usr.UsrCod"
                  " AND crs_usr.Role='%u' ORDER BY RAND(NOW()) LIMIT 1",
            GrpCod,(unsigned) Rol_STUDENT);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get a random student from a group"))
     {
      /***** Get user code *****/
      row = mysql_fetch_row (mysql_res);
      UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return UsrCod;
  }

/*****************************************************************************/
/**** Get number of teachers from the current institution in a department ****/
/*****************************************************************************/

unsigned Usr_GetNumTchsCurrentInsInDepartment (long DptCod)
  {
   char Query[256];

   /***** Get the number of teachers
          from the current institution in a department *****/
   sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
	          " FROM usr_data,crs_usr"
                  " WHERE usr_data.InsCod='%ld' AND usr_data.DptCod='%ld'"
                  " AND usr_data.UsrCod=crs_usr.UsrCod AND crs_usr.Role='%u'",
            Gbl.CurrentIns.Ins.InsCod,DptCod,(unsigned) Rol_TEACHER);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of teachers in a department");
  }

/*****************************************************************************/
/*********** Get number of users who claim to belong to a country ************/
/*****************************************************************************/

unsigned Usr_GetNumUsrsWhoClaimToBelongToCty (long CtyCod)
  {
   char Query[128];

   /***** Get the number of users in a country from database *****/
   sprintf (Query,"SELECT COUNT(UsrCod)"
	          " FROM usr_data"
                  " WHERE CtyCod='%ld'",
            CtyCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of users in a country");
  }

/*****************************************************************************/
/******** Get number of users who claim to belong to an institution **********/
/*****************************************************************************/

unsigned Usr_GetNumUsrsWhoClaimToBelongToIns (long InsCod)
  {
   char Query[128];

   /***** Get the number of users in an institution from database *****/
   sprintf (Query,"SELECT COUNT(UsrCod)"
	          " FROM usr_data"
                  " WHERE InsCod='%ld'",
            InsCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of users in an institution");
  }

/*****************************************************************************/
/*********** Get number of users who claim to belong to a centre *************/
/*****************************************************************************/

unsigned Usr_GetNumUsrsWhoClaimToBelongToCtr (long CtrCod)
  {
   char Query[128];

   /***** Get the number of users in a centre from database *****/
   sprintf (Query,"SELECT COUNT(UsrCod)"
	          " FROM usr_data"
                  " WHERE CtrCod='%ld'",
            CtrCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of users in a centre");
  }

/*****************************************************************************/
/******************* Get number of teachers in a centre **********************/
/*****************************************************************************/

unsigned Usr_GetNumberOfTeachersInCentre (long CtrCod)
  {
   char Query[256];

   /***** Get the number of teachers in a centre from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
	          " FROM usr_data,crs_usr"
                  " WHERE usr_data.CtrCod='%ld'"
                  " AND usr_data.UsrCod=crs_usr.UsrCod AND crs_usr.Role='%u'",
            CtrCod,(unsigned) Rol_TEACHER);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of teachers in a centre");
  }

/*****************************************************************************/
/******* Build query to get list with data of users in current course ********/
/*****************************************************************************/

static void Usr_BuildQueryToGetUsrsLstCrs (Rol_Role_t Role,bool Search,const char *UsrQuery,char *Query)
  {
   unsigned NumPositiveCods = 0;
   unsigned NumNegativeCods = 0;
   char LongStr[1+10+1];
   unsigned NumGrpSel;
   long GrpCod;
   unsigned NumGrpTyp;
   bool *AddStdsWithoutGroupOf;

   /***** If there are no groups selected, don't do anything *****/
   if (Role == Rol_STUDENT &&
       (!Gbl.Usrs.ClassPhoto.AllGroups &&
        !Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps))
     {
      Query[0] = '\0';
      return;
     }

   /***** Create query for users in the course *****/
   if (Gbl.Action.Act == ActReqMsgUsr)        // Selecting users to write a message
      sprintf (Query,"SELECT crs_usr.UsrCod,usr_data.Sex,crs_usr.Accepted"
                     " FROM crs_usr,usr_data"
                     " WHERE crs_usr.CrsCod='%ld'"
                     " AND crs_usr.Role='%u'"
                     " AND crs_usr.UsrCod NOT IN"
                     " (SELECT ToUsrCod FROM msg_banned WHERE FromUsrCod='%ld')"
      		     " AND crs_usr.UsrCod=usr_data.UsrCod",        // Do not get banned users
               Gbl.CurrentCrs.Crs.CrsCod,(unsigned) Role,
               Gbl.Usrs.Me.UsrDat.UsrCod);
   else if (Search)
      sprintf (Query,"SELECT crs_usr.UsrCod,usr_data.Sex,crs_usr.Accepted"
		     " FROM crs_usr,usr_data"
		     " WHERE crs_usr.CrsCod='%ld'"
		     " AND crs_usr.Role='%u'"
		     " AND crs_usr.UsrCod=usr_data.UsrCod"
		     " AND %s",
	       Gbl.CurrentCrs.Crs.CrsCod,(unsigned) Role,
	       UsrQuery);
   else
      sprintf (Query,"SELECT crs_usr.UsrCod,usr_data.Sex,crs_usr.Accepted"
                     " FROM crs_usr,usr_data"
                     " WHERE crs_usr.CrsCod='%ld'"
                     " AND crs_usr.Role='%u'"
		     " AND crs_usr.UsrCod=usr_data.UsrCod",
               Gbl.CurrentCrs.Crs.CrsCod,(unsigned) Role);

   /***** Select users in selected groups (only for students) *****/
   if (Role == Rol_STUDENT && !Gbl.Usrs.ClassPhoto.AllGroups)
     {
      /***** Get list of groups types in current course *****/
      Grp_GetListGrpTypesInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

      /***** Allocate memory for list of booleans AddStdsWithoutGroupOf *****/
      if ((AddStdsWithoutGroupOf = (bool *) calloc (Gbl.CurrentCrs.Grps.GrpTypes.Num,sizeof (bool))) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store types of group.");

      /***** Initialize vector of booleans that indicates whether it's necessary add to the list
             the students who don't belong to any group of each type *****/
      for (NumGrpTyp = 0;
           NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
           NumGrpTyp++)
         AddStdsWithoutGroupOf[NumGrpTyp] = false;

      /***** Create query with the students who belong to the groups selected *****/
      if (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps)        // If there are groups selected...
        {
         /* Check if there are positive and negative codes in the list */
         for (NumGrpSel = 0;
              NumGrpSel < Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps;
              NumGrpSel++)
            if ((GrpCod = Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCod[NumGrpSel]) > 0)
               NumPositiveCods++;
            else
               for (NumGrpTyp = 0;
                    NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
                    NumGrpTyp++)
                  if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod == -GrpCod)
                    {
                     AddStdsWithoutGroupOf[NumGrpTyp] = true;
                     break;
                    }
         /* If there are positive codes, add the students who belong to groups with those codes */
         if (NumPositiveCods)
           {
            strcat (Query," AND (crs_usr.UsrCod IN"
                          " (SELECT DISTINCT UsrCod FROM crs_grp_usr WHERE");
            NumPositiveCods = 0;
            for (NumGrpSel = 0;
                 NumGrpSel < Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps;
                 NumGrpSel++)
               if ((GrpCod = Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCod[NumGrpSel]) > 0)
                 {
                  strcat (Query,NumPositiveCods ? " OR GrpCod='" :
                	                          " GrpCod='");
                  sprintf (LongStr,"%ld",GrpCod);
                  strcat (Query,LongStr);
                  strcat (Query,"'");
                  NumPositiveCods++;
                 }
            strcat (Query,")");
           }
        }

      /***** Create a query with the students who don't belong to any group *****/
      for (NumGrpTyp = 0;
           NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
           NumGrpTyp++)
         if (AddStdsWithoutGroupOf[NumGrpTyp])
           {
            if (NumPositiveCods || NumNegativeCods)
               strcat (Query," OR ");
            else
               strcat (Query," AND (");
            /* Select all the students of the course who don't belong to any group of type GrpTypCod */
            strcat (Query,"crs_usr.UsrCod NOT IN"
                          " (SELECT DISTINCT crs_grp_usr.UsrCod"
                          " FROM crs_grp,crs_grp_usr"
                          " WHERE crs_grp.GrpTypCod='");
            sprintf (LongStr,"%ld",Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
            strcat (Query,LongStr);
            strcat (Query,"' AND crs_grp.GrpCod=crs_grp_usr.GrpCod)");
            NumNegativeCods++;
           }
      if (NumPositiveCods ||
          NumNegativeCods)
         strcat (Query,")");

      /***** Free memory used by the list of booleans AddStdsWithoutGroupOf *****/
      free ((void *) AddStdsWithoutGroupOf);

      /***** Free list of groups types in current course *****/
      Grp_FreeListGrpTypesAndGrps ();
     }

   /***** The last part of the query is for ordering the list *****/
   strcat (Query," ORDER BY "
	         "usr_data.Surname1,"
	         "usr_data.Surname2,"
	         "usr_data.FirstName,"
	         "usr_data.UsrCod");
  }

/*****************************************************************************/
/*********** Get list of users with a given role in a given scope ************/
/*****************************************************************************/

// Role can be:
// - Rol_STUDENT
// - Rol_TEACHER

void Usr_GetListUsrs (Rol_Role_t Role,Sco_Scope_t Scope)
  {
   char Query[512];

   /***** Build query *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
	 /* Get users in courses from the whole platform */
	 // 2 columns are retrieved: UsrCod, Sex
	 sprintf (Query,"SELECT DISTINCT usr_data.UsrCod,usr_data.Sex"
			" FROM usr_data,crs_usr"
			" WHERE usr_data.UsrCod=crs_usr.UsrCod"
			" AND crs_usr.Role='%u'"
			" ORDER BY "
			"usr_data.Surname1,"
			"usr_data.Surname2,"
			"usr_data.FirstName,"
			"usr_data.UsrCod",
		  (unsigned) Role);
	 break;
      case Sco_SCOPE_CTY:
	 /* Get users in courses from the current country */
	 // 2 columns are retrieved: UsrCod, Sex
	 sprintf (Query,"SELECT DISTINCT usr_data.UsrCod,usr_data.Sex"
			" FROM usr_data,crs_usr,courses,degrees,centres,institutions"
			" WHERE usr_data.UsrCod=crs_usr.UsrCod"
			" AND crs_usr.Role='%u'"
			" AND crs_usr.CrsCod=courses.CrsCod"
			" AND courses.DegCod=degrees.DegCod"
			" AND degrees.CtrCod=centres.CtrCod"
			" AND centres.InsCod=institutions.InsCod"
			" AND institutions.CtyCod='%ld'"
			" ORDER BY "
			"usr_data.Surname1,"
			"usr_data.Surname2,"
			"usr_data.FirstName,"
			"usr_data.UsrCod",
		  (unsigned) Role,
		  Gbl.CurrentCty.Cty.CtyCod);
	 break;
      case Sco_SCOPE_INS:
	 /* Get users in courses from the current institution */
	 // 2 columns are retrieved: UsrCod, Sex
	 sprintf (Query,"SELECT DISTINCT usr_data.UsrCod,usr_data.Sex"
			" FROM usr_data,crs_usr,courses,degrees,centres"
			" WHERE usr_data.UsrCod=crs_usr.UsrCod"
			" AND crs_usr.Role='%u'"
			" AND crs_usr.CrsCod=courses.CrsCod"
			" AND courses.DegCod=degrees.DegCod"
			" AND degrees.CtrCod=centres.CtrCod"
			" AND centres.InsCod='%ld'"
			" ORDER BY "
			"usr_data.Surname1,"
			"usr_data.Surname2,"
			"usr_data.FirstName,"
			"usr_data.UsrCod",
		  (unsigned) Role,
		  Gbl.CurrentIns.Ins.InsCod);
	 break;
      case Sco_SCOPE_CTR:
	 /* Get users in courses from the current centre */
	 // 2 columns are retrieved: UsrCod, Sex
	 sprintf (Query,"SELECT DISTINCT usr_data.UsrCod,usr_data.Sex"
			" FROM usr_data,crs_usr,courses,degrees"
			" WHERE usr_data.UsrCod=crs_usr.UsrCod"
			" AND crs_usr.Role='%u'"
			" AND crs_usr.CrsCod=courses.CrsCod"
			" AND courses.DegCod=degrees.DegCod"
			" AND degrees.CtrCod='%ld'"
			" ORDER BY "
			"usr_data.Surname1,"
			"usr_data.Surname2,"
			"usr_data.FirstName,"
			"usr_data.UsrCod",
		  (unsigned) Role,
		  Gbl.CurrentCtr.Ctr.CtrCod);
	 break;
      case Sco_SCOPE_DEG:
	 /* Get users in courses from the current degree */
	 // 2 columns are retrieved: UsrCod, Sex
	 sprintf (Query,"SELECT DISTINCT usr_data.UsrCod,usr_data.Sex"
			" FROM usr_data,crs_usr,courses"
			" WHERE usr_data.UsrCod=crs_usr.UsrCod"
			" AND crs_usr.Role='%u'"
			" AND crs_usr.CrsCod=courses.CrsCod"
			" AND courses.DegCod='%ld'"
			" ORDER BY "
			"usr_data.Surname1,"
			"usr_data.Surname2,"
			"usr_data.FirstName,"
			"usr_data.UsrCod",
		  (unsigned) Role,
		  Gbl.CurrentDeg.Deg.DegCod);
	 break;
      case Sco_SCOPE_CRS:
	 /* Get users from the current course */
	 // 3 columns are retrieved: UsrCod, Sex, Accepted
	 Usr_BuildQueryToGetUsrsLstCrs (Role,false,NULL,Query);
	 break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
/*
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
      Lay_ShowAlert (Lay_INFO,Query);
*/
   /***** Get list of users from database given a query *****/
   Usr_GetListUsrsFromQuery (Query,Role,Scope);
  }

/*****************************************************************************/
/********* Search list of users with a given role in current scope ***********/
/*****************************************************************************/

static void Usr_SearchListUsrs (Rol_Role_t Role,const char *UsrQuery)
  {
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];
   char Query[16*1024];

   /***** Build query *****/
   if (Role == Rol__GUEST_)	// Guests
      /* Search users with no courses */
      sprintf (Query,"SELECT UsrCod,Sex"
                     " FROM usr_data"
		     " WHERE %s"
		     " AND UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
		     " ORDER BY "
		     "Surname1,"
		     "Surname2,"
		     "FirstName,"
		     "UsrCod",
	       UsrQuery);
   else				// Students / Teachers
      /*
         To achieve maximum speed, it's important to do the things in this order:
         1) Search for user's name (UsrQuery) getting candidate users
         2) Filter the candidate users according to scope
      */
      switch (Gbl.Scope.Current)
	{
	 case Sco_SCOPE_SYS:
	    /* Search users in courses from the whole platform */
	    // 2 columns are retrieved: UsrCod, Sex
	    switch (Gbl.Usrs.Me.LoggedRole)
	      {
	       case Rol_SYS_ADM:
		  sprintf (Query,"SELECT list_usrs.UsrCod,usr_data.Sex FROM "
				 "(SELECT DISTINCT candidate_users.UsrCod"
				 " FROM (SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,crs_usr"
				 " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				 " AND crs_usr.Role='%u') AS list_usrs,usr_data"
				 " WHERE list_usrs.UsrCod=usr_data.UsrCod "
				 " ORDER BY "
				 "usr_data.Surname1,"
				 "usr_data.Surname2,"
				 "usr_data.FirstName,"
				 "usr_data.UsrCod",
			   UsrQuery,
			   (unsigned) Role);
		  break;
	       default:
		  sprintf (Query,"SELECT list_usrs.UsrCod,usr_data.Sex FROM "
				    "("
				    // Users whose privacy is
				    // Pri_VISIBILITY_SYSTEM or Pri_VISIBILITY_WORLD
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "crs_usr,"
				    "usr_data"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility IN ('%s','%s')"
		                    " UNION "
				    // Users who share any course with me
		                    // and whose privacy is Pri_VISIBILITY_COURSE
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "(SELECT CrsCod FROM crs_usr WHERE UsrCod='%ld') AS my_crs,"
				    "crs_usr,"
				    "usr_data"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
				    " AND crs_usr.CrsCod=my_crs.CrsCod"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility='%s'"
		                    " UNION "
				    // Users who share any course with me with another role
				    // and whose privacy is Pri_VISIBILITY_USER
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "(SELECT CrsCod,Role FROM crs_usr WHERE UsrCod='%ld') AS my_crs_role,"
				    "crs_usr,"
				    "usr_data"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
		                    " AND crs_usr.CrsCod=my_crs_role.CrsCod"
                                    " AND crs_usr.Role<>my_crs_role.Role"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility='%s'"
				    ") "
				 "AS list_usrs,usr_data"
				 " WHERE list_usrs.UsrCod=usr_data.UsrCod "
				 " ORDER BY "
				 "usr_data.Surname1,"
				 "usr_data.Surname2,"
				 "usr_data.FirstName,"
				 "usr_data.UsrCod",
			   UsrQuery,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
                           Pri_VisibilityDB[Pri_VISIBILITY_WORLD ],
			   UsrQuery,
			   Gbl.Usrs.Me.UsrDat.UsrCod,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_COURSE],
			   UsrQuery,
			   Gbl.Usrs.Me.UsrDat.UsrCod,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_USER  ]);
	          break;
	      }
	    break;
	 case Sco_SCOPE_CTY:
	    /* Search users in courses from the current country */
	    // 2 columns are retrieved: UsrCod, Sex
	    switch (Gbl.Usrs.Me.LoggedRole)
	      {
	       case Rol_SYS_ADM:
		  sprintf (Query,"SELECT list_usrs.UsrCod,usr_data.Sex FROM "
				 "(SELECT DISTINCT candidate_users.UsrCod"
				 " FROM (SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,crs_usr,courses,degrees,centres,institutions"
				 " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				 " AND crs_usr.Role='%u'"
				 " AND crs_usr.CrsCod=courses.CrsCod"
				 " AND courses.DegCod=degrees.DegCod"
				 " AND degrees.CtrCod=centres.CtrCod"
				 " AND centres.InsCod=institutions.InsCod"
				 " AND institutions.CtyCod='%ld') AS list_usrs,usr_data"
				 " WHERE list_usrs.UsrCod=usr_data.UsrCod "
				 " ORDER BY "
				 "usr_data.Surname1,"
				 "usr_data.Surname2,"
				 "usr_data.FirstName,"
				 "usr_data.UsrCod",
			   UsrQuery,
			   (unsigned) Role,
			   Gbl.CurrentCty.Cty.CtyCod);
		  break;
	       default:
		  sprintf (Query,"SELECT list_usrs.UsrCod,usr_data.Sex FROM "
				    "("
				    // Users whose privacy is
				    // Pri_VISIBILITY_SYSTEM or Pri_VISIBILITY_WORLD
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "crs_usr,"
				    "usr_data,"
				    "courses,degrees,centres,institutions"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility IN ('%s','%s')"
				    " AND crs_usr.CrsCod=courses.CrsCod"
				    " AND courses.DegCod=degrees.DegCod"
				    " AND degrees.CtrCod=centres.CtrCod"
				    " AND centres.InsCod=institutions.InsCod"
				    " AND institutions.CtyCod='%ld'"
		                    " UNION "
				    // Users who share any course with me
		                    // and whose privacy is Pri_VISIBILITY_COURSE
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "(SELECT CrsCod FROM crs_usr WHERE UsrCod='%ld') AS my_crs,"
				    "crs_usr,"
				    "usr_data,"
				    "courses,degrees,centres,institutions"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
				    " AND crs_usr.CrsCod=my_crs.CrsCod"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility='%s'"
				    " AND crs_usr.CrsCod=courses.CrsCod"
				    " AND courses.DegCod=degrees.DegCod"
				    " AND degrees.CtrCod=centres.CtrCod"
				    " AND centres.InsCod=institutions.InsCod"
				    " AND institutions.CtyCod='%ld'"
		                    " UNION "
				    // Users who share any course with me with another role
				    // and whose privacy is Pri_VISIBILITY_USER
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "(SELECT CrsCod,Role FROM crs_usr WHERE UsrCod='%ld') AS my_crs_role,"
				    "crs_usr,"
				    "usr_data,"
				    "courses,degrees,centres,institutions"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
		                    " AND crs_usr.CrsCod=my_crs_role.CrsCod"
                                    " AND crs_usr.Role<>my_crs_role.Role"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility='%s'"
				    " AND crs_usr.CrsCod=courses.CrsCod"
				    " AND courses.DegCod=degrees.DegCod"
				    " AND degrees.CtrCod=centres.CtrCod"
				    " AND centres.InsCod=institutions.InsCod"
				    " AND institutions.CtyCod='%ld'"
				    ") "
				 "AS list_usrs,usr_data"
				 " WHERE list_usrs.UsrCod=usr_data.UsrCod "
				 " ORDER BY "
				 "usr_data.Surname1,"
				 "usr_data.Surname2,"
				 "usr_data.FirstName,"
				 "usr_data.UsrCod",
			   UsrQuery,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
                           Pri_VisibilityDB[Pri_VISIBILITY_WORLD ],
			   Gbl.CurrentCty.Cty.CtyCod,
			   UsrQuery,
			   Gbl.Usrs.Me.UsrDat.UsrCod,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_COURSE],
			   Gbl.CurrentCty.Cty.CtyCod,
			   UsrQuery,
			   Gbl.Usrs.Me.UsrDat.UsrCod,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_USER  ],
			   Gbl.CurrentCty.Cty.CtyCod);
	          break;
	      }
	    break;
	 case Sco_SCOPE_INS:
	    /* Search users in courses from the current institution */
	    // 2 columns are retrieved: UsrCod, Sex
	    switch (Gbl.Usrs.Me.LoggedRole)
	      {
	       case Rol_INS_ADM:
	       case Rol_SYS_ADM:
		  sprintf (Query,"SELECT list_usrs.UsrCod,usr_data.Sex FROM "
				 "(SELECT DISTINCT candidate_users.UsrCod"
				 " FROM (SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,crs_usr,courses,degrees,centres"
				 " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				 " AND crs_usr.Role='%u'"
				 " AND crs_usr.CrsCod=courses.CrsCod"
				 " AND courses.DegCod=degrees.DegCod"
				 " AND degrees.CtrCod=centres.CtrCod"
				 " AND centres.InsCod='%ld') AS list_usrs,usr_data"
				 " WHERE list_usrs.UsrCod=usr_data.UsrCod "
				 " ORDER BY "
				 "usr_data.Surname1,"
				 "usr_data.Surname2,"
				 "usr_data.FirstName,"
				 "usr_data.UsrCod",
			   UsrQuery,
			   (unsigned) Role,
			   Gbl.CurrentIns.Ins.InsCod);
		  break;
	       default:
		  sprintf (Query,"SELECT list_usrs.UsrCod,usr_data.Sex FROM "
				    "("
				    // Users whose privacy is
				    // Pri_VISIBILITY_SYSTEM or Pri_VISIBILITY_WORLD
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "crs_usr,"
				    "usr_data,"
				    "courses,degrees,centres"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility IN ('%s','%s')"
				    " AND crs_usr.CrsCod=courses.CrsCod"
				    " AND courses.DegCod=degrees.DegCod"
				    " AND degrees.CtrCod=centres.CtrCod"
				    " AND centres.InsCod='%ld'"
		                    " UNION "
				    // Users who share any course with me
		                    // and whose privacy is Pri_VISIBILITY_COURSE
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "(SELECT CrsCod FROM crs_usr WHERE UsrCod='%ld') AS my_crs,"
				    "crs_usr,"
				    "usr_data,"
				    "courses,degrees,centres"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
				    " AND crs_usr.CrsCod=my_crs.CrsCod"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility='%s'"
				    " AND crs_usr.CrsCod=courses.CrsCod"
				    " AND courses.DegCod=degrees.DegCod"
				    " AND degrees.CtrCod=centres.CtrCod"
				    " AND centres.InsCod='%ld'"
		                    " UNION "
				    // Users who share any course with me with another role
				    // and whose privacy is Pri_VISIBILITY_USER
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "(SELECT CrsCod,Role FROM crs_usr WHERE UsrCod='%ld') AS my_crs_role,"
				    "crs_usr,"
				    "usr_data,"
				    "courses,degrees,centres"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
		                    " AND crs_usr.CrsCod=my_crs_role.CrsCod"
                                    " AND crs_usr.Role<>my_crs_role.Role"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility='%s'"
				    " AND crs_usr.CrsCod=courses.CrsCod"
				    " AND courses.DegCod=degrees.DegCod"
				    " AND degrees.CtrCod=centres.CtrCod"
				    " AND centres.InsCod='%ld'"
				    ") "
				 "AS list_usrs,usr_data"
				 " WHERE list_usrs.UsrCod=usr_data.UsrCod "
				 " ORDER BY "
				 "usr_data.Surname1,"
				 "usr_data.Surname2,"
				 "usr_data.FirstName,"
				 "usr_data.UsrCod",
			   UsrQuery,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
                           Pri_VisibilityDB[Pri_VISIBILITY_WORLD ],
			   Gbl.CurrentIns.Ins.InsCod,
			   UsrQuery,
			   Gbl.Usrs.Me.UsrDat.UsrCod,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_COURSE],
			   Gbl.CurrentIns.Ins.InsCod,
			   UsrQuery,
			   Gbl.Usrs.Me.UsrDat.UsrCod,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_USER  ],
			   Gbl.CurrentIns.Ins.InsCod);
	          break;
	      }
            break;
	 case Sco_SCOPE_CTR:
	    /* Search users in courses from the current centre */
	    // 2 columns are retrieved: UsrCod, Sex
	    switch (Gbl.Usrs.Me.LoggedRole)
	      {
	       case Rol_CTR_ADM:
	       case Rol_INS_ADM:
	       case Rol_SYS_ADM:
		  sprintf (Query,"SELECT list_usrs.UsrCod,usr_data.Sex FROM "
				 "(SELECT DISTINCT candidate_users.UsrCod"
				 " FROM (SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,crs_usr,courses,degrees"
				 " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				 " AND crs_usr.Role='%u'"
				 " AND crs_usr.CrsCod=courses.CrsCod"
				 " AND courses.DegCod=degrees.DegCod"
				 " AND degrees.CtrCod='%ld') AS list_usrs,usr_data"
				 " WHERE list_usrs.UsrCod=usr_data.UsrCod "
				 " ORDER BY "
				 "usr_data.Surname1,"
				 "usr_data.Surname2,"
				 "usr_data.FirstName,"
				 "usr_data.UsrCod",
			   UsrQuery,
			   (unsigned) Role,
			   Gbl.CurrentCtr.Ctr.CtrCod);
		  break;
	       default:
		  sprintf (Query,"SELECT list_usrs.UsrCod,usr_data.Sex FROM "
				    "("
				    // Users whose privacy is
				    // Pri_VISIBILITY_SYSTEM or Pri_VISIBILITY_WORLD
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "crs_usr,"
				    "usr_data,"
				    "courses,degrees"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility IN ('%s','%s')"
				    " AND crs_usr.CrsCod=courses.CrsCod"
				    " AND courses.DegCod=degrees.DegCod"
				    " AND degrees.CtrCod='%ld'"
		                    " UNION "
				    // Users who share any course with me
		                    // and whose privacy is Pri_VISIBILITY_COURSE
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "(SELECT CrsCod FROM crs_usr WHERE UsrCod='%ld') AS my_crs,"
				    "crs_usr,"
				    "usr_data,"
				    "courses,degrees"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
				    " AND crs_usr.CrsCod=my_crs.CrsCod"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility='%s'"
				    " AND crs_usr.CrsCod=courses.CrsCod"
				    " AND courses.DegCod=degrees.DegCod"
				    " AND degrees.CtrCod='%ld'"
		                    " UNION "
				    // Users who share any course with me with another role
				    // and whose privacy is Pri_VISIBILITY_USER
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "(SELECT CrsCod,Role FROM crs_usr WHERE UsrCod='%ld') AS my_crs_role,"
				    "crs_usr,"
				    "usr_data,"
				    "courses,degrees"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
		                    " AND crs_usr.CrsCod=my_crs_role.CrsCod"
                                    " AND crs_usr.Role<>my_crs_role.Role"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility='%s'"
				    " AND crs_usr.CrsCod=courses.CrsCod"
				    " AND courses.DegCod=degrees.DegCod"
				    " AND degrees.CtrCod='%ld'"
				    ") "
				 "AS list_usrs,usr_data"
				 " WHERE list_usrs.UsrCod=usr_data.UsrCod "
				 " ORDER BY "
				 "usr_data.Surname1,"
				 "usr_data.Surname2,"
				 "usr_data.FirstName,"
				 "usr_data.UsrCod",
			   UsrQuery,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
                           Pri_VisibilityDB[Pri_VISIBILITY_WORLD ],
			   Gbl.CurrentCtr.Ctr.CtrCod,
			   UsrQuery,
			   Gbl.Usrs.Me.UsrDat.UsrCod,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_COURSE],
			   Gbl.CurrentCtr.Ctr.CtrCod,
			   UsrQuery,
			   Gbl.Usrs.Me.UsrDat.UsrCod,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_USER  ],
			   Gbl.CurrentCtr.Ctr.CtrCod);
	          break;
	      }
	    break;
	 case Sco_SCOPE_DEG:
	    /* Search users in courses from the current degree */
	    // 2 columns are retrieved: UsrCod, Sex
	    switch (Gbl.Usrs.Me.LoggedRole)
	      {
	       case Rol_DEG_ADM:
	       case Rol_CTR_ADM:
	       case Rol_INS_ADM:
	       case Rol_SYS_ADM:
		  sprintf (Query,"SELECT list_usrs.UsrCod,usr_data.Sex FROM "
				    "(SELECT DISTINCT candidate_users.UsrCod"
				    " FROM (SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,crs_usr,courses"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
				    " AND crs_usr.CrsCod=courses.CrsCod"
				    " AND courses.DegCod='%ld') "
				 "AS list_usrs,usr_data"
				 " WHERE list_usrs.UsrCod=usr_data.UsrCod "
				 " ORDER BY "
				 "usr_data.Surname1,"
				 "usr_data.Surname2,"
				 "usr_data.FirstName,"
				 "usr_data.UsrCod",
			   UsrQuery,
			   (unsigned) Role,
			   Gbl.CurrentDeg.Deg.DegCod);
		  break;
	       default:
		  sprintf (Query,"SELECT list_usrs.UsrCod,usr_data.Sex FROM "
				    "("
				    // Users whose privacy is
				    // Pri_VISIBILITY_SYSTEM or Pri_VISIBILITY_WORLD
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "crs_usr,"
				    "usr_data,"
				    "courses"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility IN ('%s','%s')"
				    " AND crs_usr.CrsCod=courses.CrsCod"
				    " AND courses.DegCod='%ld'"
		                    " UNION "
				    // Users who share any course with me
		                    // and whose privacy is Pri_VISIBILITY_COURSE
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "(SELECT CrsCod FROM crs_usr WHERE UsrCod='%ld') AS my_crs,"
				    "crs_usr,"
				    "usr_data,"
				    "courses"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
				    " AND crs_usr.CrsCod=my_crs.CrsCod"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility='%s'"
				    " AND crs_usr.CrsCod=courses.CrsCod"
				    " AND courses.DegCod='%ld'"
		                    " UNION "
				    // Users who share any course with me with another role
				    // and whose privacy is Pri_VISIBILITY_USER
				    "SELECT DISTINCT crs_usr.UsrCod FROM "
				    "(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users,"
				    "(SELECT CrsCod,Role FROM crs_usr WHERE UsrCod='%ld') AS my_crs_role,"
				    "crs_usr,"
				    "usr_data,"
				    "courses"
				    " WHERE candidate_users.UsrCod=crs_usr.UsrCod"
				    " AND crs_usr.Role='%u'"
		                    " AND crs_usr.CrsCod=my_crs_role.CrsCod"
                                    " AND crs_usr.Role<>my_crs_role.Role"
				    " AND crs_usr.UsrCod=usr_data.UsrCod"
		                    " AND usr_data.ProfileVisibility='%s'"
				    " AND crs_usr.CrsCod=courses.CrsCod"
				    " AND courses.DegCod='%ld'"
				    ") "
				 "AS list_usrs,usr_data"
				 " WHERE list_usrs.UsrCod=usr_data.UsrCod "
				 " ORDER BY "
				 "usr_data.Surname1,"
				 "usr_data.Surname2,"
				 "usr_data.FirstName,"
				 "usr_data.UsrCod",
			   UsrQuery,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
                           Pri_VisibilityDB[Pri_VISIBILITY_WORLD ],
			   Gbl.CurrentDeg.Deg.DegCod,
			   UsrQuery,
			   Gbl.Usrs.Me.UsrDat.UsrCod,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_COURSE],
			   Gbl.CurrentDeg.Deg.DegCod,
			   UsrQuery,
			   Gbl.Usrs.Me.UsrDat.UsrCod,
			   (unsigned) Role,
			   Pri_VisibilityDB[Pri_VISIBILITY_USER  ],
			   Gbl.CurrentDeg.Deg.DegCod);
	          break;
	      }
	    break;
	 case Sco_SCOPE_CRS:
	    /* Search users in courses from the current course */
	    // 3 columns are retrieved: UsrCod, Sex, Accepted
	    Usr_BuildQueryToGetUsrsLstCrs (Role,true,UsrQuery,Query);
	    break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
        }

   // if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
      Lay_ShowAlert (Lay_INFO,Query);

   /***** Get list of users from database given a query *****/
   Usr_GetListUsrsFromQuery (Query,Role,Gbl.Scope.Current);
  }

/*****************************************************************************/
/******************** Get list with data of administrators *******************/
/*****************************************************************************/

static void Usr_GetAdmsLst (Sco_Scope_t Scope)
  {
   char Query[1024];

   /***** Build query *****/
   // 2 columns are retrieved: UsrCod, Sex
   // Important: it is better to use:
   // SELECT... WHERE UsrCod IN (SELECT...) OR UsrCod IN (SELECT...) <-- fast
   // instead of using or with different joins:
   // SELECT... WHERE (...) OR (...) <-- very slow
   switch (Scope)
     {
      case Sco_SCOPE_SYS:	// All admins
         strcpy (Query,"SELECT UsrCod,Sex FROM usr_data WHERE UsrCod IN "
                       "(SELECT DISTINCT UsrCod FROM admin)"
                       " ORDER BY Surname1,Surname2,FirstName,UsrCod");
         break;
      case Sco_SCOPE_CTY:	// System admins
				// and admins of the institutions, centres and degrees in the current country
         sprintf (Query,"SELECT UsrCod,Sex FROM usr_data"
                        " WHERE UsrCod IN "
                        "(SELECT UsrCod FROM admin"
                        " WHERE Scope='Sys')"
                        " OR UsrCod IN "
                        "(SELECT admin.UsrCod FROM admin,institutions"
                        " WHERE admin.Scope='Ins'"
                        " AND admin.Cod=institutions.InsCod"
                        " AND institutions.CtyCod='%ld')"
                        " OR UsrCod IN "
                        "(SELECT admin.UsrCod FROM admin,centres,institutions"
                        " WHERE admin.Scope='Ctr'"
                        " AND admin.Cod=centres.CtrCod"
                        " AND centres.InsCod=institutions.InsCod"
                        " AND institutions.CtyCod='%ld')"
                        " OR UsrCod IN "
                        "(SELECT admin.UsrCod FROM admin,degrees,centres,institutions"
                        " WHERE admin.Scope='Deg'"
                        " AND admin.Cod=degrees.DegCod"
                        " AND degrees.CtrCod=centres.CtrCod"
                        " AND centres.InsCod=institutions.InsCod"
                        " AND institutions.CtyCod='%ld')"
                        " ORDER BY Surname1,Surname2,FirstName,UsrCod",
                  Gbl.CurrentCty.Cty.CtyCod,
                  Gbl.CurrentCty.Cty.CtyCod,
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:	// System admins,
				// admins of the current institution,
				// and admins of the centres and degrees in the current institution
         sprintf (Query,"SELECT UsrCod,Sex FROM usr_data"
                        " WHERE UsrCod IN "
                        "(SELECT UsrCod FROM admin"
                        " WHERE Scope='Sys')"
                        " OR UsrCod IN "
                        "(SELECT UsrCod FROM admin"
                        " WHERE Scope='Ins' AND Cod='%ld')"
                        " OR UsrCod IN "
                        "(SELECT admin.UsrCod FROM admin,centres"
                        " WHERE admin.Scope='Ctr'"
                        " AND admin.Cod=centres.CtrCod"
                        " AND centres.InsCod='%ld')"
                        " OR UsrCod IN "
                        "(SELECT admin.UsrCod FROM admin,degrees,centres"
                        " WHERE admin.Scope='Deg'"
                        " AND admin.Cod=degrees.DegCod"
                        " AND degrees.CtrCod=centres.CtrCod"
                        " AND centres.InsCod='%ld')"
                        " ORDER BY Surname1,Surname2,FirstName,UsrCod",
                  Gbl.CurrentIns.Ins.InsCod,
                  Gbl.CurrentIns.Ins.InsCod,
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:	// System admins,
				// admins of the current institution,
				// admins and the current centre,
				// and admins of the degrees in the current centre
         sprintf (Query,"SELECT UsrCod,Sex FROM usr_data"
                        " WHERE UsrCod IN "
                        "(SELECT UsrCod FROM admin"
                        " WHERE Scope='Sys')"
                        " OR UsrCod IN "
                        "(SELECT UsrCod FROM admin"
                        " WHERE Scope='Ins' AND Cod='%ld')"
                        " OR UsrCod IN "
                        "(SELECT UsrCod FROM admin"
                        " WHERE Scope='Ctr' AND Cod='%ld')"
                        " OR UsrCod IN "
                        "(SELECT admin.UsrCod FROM admin,degrees"
                        " WHERE admin.Scope='Deg'"
                        " AND admin.Cod=degrees.DegCod"
                        " AND degrees.CtrCod='%ld')"
                        " ORDER BY Surname1,Surname2,FirstName,UsrCod",
                  Gbl.CurrentIns.Ins.InsCod,
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:	// System admins
				// and admins of the current institution, centre or degree
         sprintf (Query,"SELECT UsrCod,Sex FROM usr_data"
                        " WHERE UsrCod IN "
                        "(SELECT UsrCod FROM admin"
                        " WHERE Scope='Sys')"
                        " OR UsrCod IN "
                        "(SELECT UsrCod FROM admin"
                        " WHERE Scope='Ins' AND Cod='%ld')"
                        " OR UsrCod IN "
                        "(SELECT UsrCod FROM admin"
                        " WHERE Scope='Ctr' AND Cod='%ld')"
                        " OR UsrCod IN "
                        "(SELECT UsrCod FROM admin"
                        " WHERE Scope='Deg' AND Cod='%ld')"
                        " ORDER BY Surname1,Surname2,FirstName,UsrCod",
                  Gbl.CurrentIns.Ins.InsCod,
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      default:        // not aplicable
	 Lay_ShowErrorAndExit ("Wrong scope.");
         break;
     }

   /***** Get list of administrators from database *****/
   Usr_GetListUsrsFromQuery (Query,Rol_DEG_ADM,Scope);
  }

/*****************************************************************************/
/************************ Get list with data of guests ***********************/
/*****************************************************************************/

static void Usr_GetGstsLst (Sco_Scope_t Scope)
  {
   char Query[512];

   /***** Build query *****/
   // 2 columns are retrieved: UsrCod, Sex
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         strcpy (Query,"SELECT DISTINCT UsrCod,Sex"
                       " FROM usr_data"
                       " WHERE UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
                       " ORDER BY Surname1,Surname2,FirstName,UsrCod");
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT DISTINCT UsrCod,Sex"
                        " FROM usr_data"
                        " WHERE (CtyCod='%ld' OR InsCtyCod='%ld')"
                        " AND UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
                        " ORDER BY Surname1,Surname2,FirstName,UsrCod",
                  Gbl.CurrentCty.Cty.CtyCod,
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT DISTINCT UsrCod,Sex"
                        " FROM usr_data"
                        " WHERE InsCod='%ld'"
                        " AND UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
                        " ORDER BY Surname1,Surname2,FirstName,UsrCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT DISTINCT UsrCod,Sex"
                        " FROM usr_data"
                        " WHERE CtrCod='%ld'"
                        " AND UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
                        " ORDER BY Surname1,Surname2,FirstName,UsrCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      default:        // not aplicable
         return;
     }

   /***** Get list of students from database *****/
   Usr_GetListUsrsFromQuery (Query,Rol__GUEST_,Scope);
  }

/*****************************************************************************/
/*********** Get the user's codes of all the students of a degree ************/
/*****************************************************************************/

void Usr_GetUnorderedStdsCodesInDeg (long DegCod)
  {
   char Query[512];

   Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs = 0;

   if (Usr_GetNumUsrsInCrssOfDeg (Rol_STUDENT,DegCod))
     {
      /***** Get the students in a degree from database *****/
      // 2 columns are retrieved: UsrCod, Sex
      sprintf (Query,"SELECT DISTINCT usr_data.UsrCod,usr_data.Sex"
	             " FROM courses,crs_usr,usr_data"
                     " WHERE courses.DegCod='%ld'"
                     " AND courses.CrsCod=crs_usr.CrsCod"
                     " AND crs_usr.Role='%u'"
                     " AND crs_usr.UsrCod=usr_data.UsrCod",
               DegCod,(unsigned) Rol_STUDENT);

      /***** Get list of students from database *****/
      Usr_GetListUsrsFromQuery (Query,Rol_STUDENT,Sco_SCOPE_DEG);
     }
  }

/*****************************************************************************/
/********************** Get list of users from database **********************/
/*****************************************************************************/

static void Usr_GetListUsrsFromQuery (const char *Query,Rol_Role_t Role,Sco_Scope_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsr;
   bool Abort = false;

   if (!Query[0])
     {
      Gbl.Usrs.LstUsrs[Role].NumUsrs = 0;
      return;
     }

   /***** Query database *****/
   if ((Gbl.Usrs.LstUsrs[Role].NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get list of users")))
     {
      if (Gbl.Usrs.LstUsrs[Role].NumUsrs > Cfg_MAX_USRS_IN_LIST)
        {
         Usr_ShowWarningListIsTooBig (Gbl.Usrs.LstUsrs[Role].NumUsrs);
         Abort = true;
        }
      else
        {
         /***** Allocate memory for the list of users *****/
         Usr_AllocateUsrsList (Role);

         /***** Get list of users *****/
         for (NumUsr = 0;
              NumUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs;
              NumUsr++)
           {
            /* Get next user */
            row = mysql_fetch_row (mysql_res);

            /* Get user code */
            Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].UsrCod = Str_ConvertStrCodToLongCod (row[0]);

            /* Get user's sex */
            Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Sex = Usr_GetSexFromStr (row[1]);

            /* Get user's acceptance of enrollment in course(s) */
	    if (Role == Rol_STUDENT ||
		Role == Rol_TEACHER)
	      {
	       if (Scope == Sco_SCOPE_CRS)
		  // Query result has a third column with the acceptation
		  Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Accepted = (Str_ConvertToUpperLetter (row[2][0]) == 'Y');
	       else
		  // Query result has not a third column with the acceptation
		  Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Accepted = (Usr_GetNumCrssOfUsrWithARoleNotAccepted (Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].UsrCod,Role) == 0);
	      }
	    else
	       Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Accepted = false;

            /* By default, users are not removed */
            Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Remove = false;
           }
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (Abort)
      Lay_ShowErrorAndExit (NULL);
  }

/*****************************************************************************/
/********************** Allocate space for list of users *********************/
/*****************************************************************************/

static void Usr_AllocateUsrsList (Rol_Role_t Role)
  {
/*
if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
   {
    sprintf (Gbl.Message,"Memory used by list = %lu",(long) sizeof (struct UsrInList) * NumUsrs);
    Lay_ShowAlert (Lay_INFO,Gbl.Message);
   }
*/
   if (Gbl.Usrs.LstUsrs[Role].NumUsrs)
      if ((Gbl.Usrs.LstUsrs[Role].Lst = (struct UsrInList *) calloc (Gbl.Usrs.LstUsrs[Role].NumUsrs,sizeof (struct UsrInList))) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store users list.");
  }

/*****************************************************************************/
/********************* Free space used for list of users *********************/
/*****************************************************************************/

void Usr_FreeUsrsList (Rol_Role_t Role)
  {
   if (Gbl.Usrs.LstUsrs[Role].NumUsrs)
     {
      /***** Free the list itself *****/
      if (Gbl.Usrs.LstUsrs[Role].Lst)
        {
         free ((void *) Gbl.Usrs.LstUsrs[Role].Lst);
         Gbl.Usrs.LstUsrs[Role].Lst = NULL;
        }

      /***** Reset number of users *****/
      Gbl.Usrs.LstUsrs[Role].NumUsrs = 0;
     }
  }

/*****************************************************************************/
/******** Show form to confirm that I want to see a big list of users ********/
/*****************************************************************************/

static void Usr_ShowWarningListIsTooBig (unsigned NumStds)
  {
   extern const char *Txt_The_list_of_X_users_is_too_large_to_be_displayed;

   sprintf (Gbl.Message,Txt_The_list_of_X_users_is_too_large_to_be_displayed,
            NumStds);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);
  }

/*****************************************************************************/
/******** Show form to confirm that I want to see a big list of users ********/
/*****************************************************************************/

bool Usr_GetIfShowBigList (unsigned NumUsrs)
  {
   bool ShowBigList;
   char YN[1+1];

   /***** If list of users is too big... *****/
   if (NumUsrs <= Cfg_MIN_NUM_USERS_TO_CONFIRM_SHOW_BIG_LIST)
      return true;        // List is not too big ==> show it

   /***** Get parameter with user's confirmation to see a big list of users *****/
   Par_GetParToText ("ShowBigList",YN,1);
   if (!(ShowBigList = (Str_ConvertToUpperLetter (YN[0]) == 'Y')))
      Usr_PutButtonToConfirmIWantToSeeBigList (NumUsrs);

   return ShowBigList;
  }

/*****************************************************************************/
/******** Show form to confirm that I want to see a big list of users ********/
/*****************************************************************************/

static void Usr_PutButtonToConfirmIWantToSeeBigList (unsigned NumUsrs)
  {
   extern const char *Txt_The_list_of_X_users_is_too_large_to_be_displayed;
   extern const char *Txt_Show_anyway;

   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");

   /***** Show warning *****/
   sprintf (Gbl.Message,Txt_The_list_of_X_users_is_too_large_to_be_displayed,
            NumUsrs);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);

   /***** Put form to confirm that I want to see the big list *****/
   Act_FormStart (Gbl.Action.Act);
   Grp_PutParamsCodGrps ();
   Usr_PutParamUsrListType (Gbl.Usrs.Me.ListType);
   Usr_PutParamColsClassPhoto ();
   Usr_PutParamListWithPhotos ();
   Usr_PutExtraParamsUsrList (Gbl.Action.Act);
   Par_PutHiddenParamChar ("ShowBigList",'Y');

   /***** Send button *****/
   Lay_PutConfirmButton (Txt_Show_anyway);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************* Write parameter with the list of users selected ***************/
/*****************************************************************************/

void Usr_PutHiddenParUsrCodAll (Act_Action_t NextAction,const char *ListUsrCods)
  {
   if (Gbl.Session.IsOpen)
      Ses_InsertHiddenParInDB (NextAction,"UsrCodAll",ListUsrCods);
   else
      Par_PutHiddenParamString ("UsrCodAll",ListUsrCods);
  }

/*****************************************************************************/
/************************* Get list of selected users ************************/
/*****************************************************************************/

void Usr_GetListsSelectedUsrs (void)
  {
   unsigned Length;

   /***** Allocate memory for the lists of users *****/
   Usr_AllocateListSelectedUsrCodAll ();
   Usr_AllocateListSelectedUsrCodTch ();
   Usr_AllocateListSelectedUsrCodStd ();

   /***** Get selected users *****/
   if (Gbl.Session.IsOpen)	// If the session is open, get parameter from DB
     {
      Ses_GetHiddenParFromDB (Gbl.Action.Act,"UsrCodAll",
                              Gbl.Usrs.Select.All,Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);
      Str_ChangeFormat (Str_FROM_FORM,Str_TO_TEXT,
			Gbl.Usrs.Select.All,Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS,true);
     }
   else
      Par_GetParMultiToText ("UsrCodAll",Gbl.Usrs.Select.All,Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);

   Par_GetParMultiToText ("UsrCodTch",Gbl.Usrs.Select.Tch,Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);	// Teachers or guests

   Par_GetParMultiToText ("UsrCodStd",Gbl.Usrs.Select.Std,Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);	// Students
/*
sprintf (Gbl.Message,"UsrCodAll = %s / UsrCodTch = %s / UsrCodStd = %s",
         Gbl.Usrs.Select.All,Gbl.Usrs.Select.Tch,Gbl.Usrs.Select.Std);
Lay_ShowErrorAndExit (Gbl.Message);
*/
   /***** Add teachers to the list with all selected users *****/
   if (Gbl.Usrs.Select.Tch[0])
     {
      if (Gbl.Usrs.Select.All[0])
         if ((Length = strlen (Gbl.Usrs.Select.All)) < Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS)
           {
            Gbl.Usrs.Select.All[Length  ] = Par_SEPARATOR_PARAM_MULTIPLE;
            Gbl.Usrs.Select.All[Length+1] = '\0';
           }
      strncat (Gbl.Usrs.Select.All,Gbl.Usrs.Select.Tch,
               Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS - strlen (Gbl.Usrs.Select.All));
     }

   /***** Add students to the list with all selected users *****/
   if (Gbl.Usrs.Select.Std[0])
     {
      if (Gbl.Usrs.Select.All[0])
         if ((Length = strlen (Gbl.Usrs.Select.All)) < Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS)
           {
            Gbl.Usrs.Select.All[Length  ] = Par_SEPARATOR_PARAM_MULTIPLE;
            Gbl.Usrs.Select.All[Length+1] = '\0';
           }
      strncat (Gbl.Usrs.Select.All,Gbl.Usrs.Select.Std,
               Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS - strlen (Gbl.Usrs.Select.All));
     }
  }

/*****************************************************************************/
/*** Get list of recipients of a message written explicitely by the sender ***/
/*****************************************************************************/
// Returns true if no errors

bool Usr_GetListMsgRecipientsWrittenExplicitelyBySender (bool WriteErrorMsgs)
  {
   extern const char *Txt_There_is_no_user_with_nickname_X;
   extern const char *Txt_There_is_no_user_with_email_X;
   extern const char *Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email;
   extern const char *Txt_There_is_no_user_with_ID_nick_or_e_mail_X;
   extern const char *Txt_The_ID_nickname_or_email_X_is_not_valid;
   unsigned Length;
   const char *Ptr;
   char UsrIDNickOrEmail[1024+1];
   struct UsrData UsrDat;
   struct ListUsrCods ListUsrCods;
   bool Error = false;

   /***** Allocate memory for the lists of users's IDs *****/
   Usr_AllocateListSelectedUsrCodAll ();

   /***** Allocate memory for the lists of recipients written explicetely *****/
   Usr_AllocateListOtherRecipients ();

   /***** Get recipients written explicetely *****/
   Par_GetParToText ("OtherRecipients",Gbl.Usrs.ListOtherRecipients,Nck_MAX_BYTES_LIST_NICKS);

   /***** Add encrypted users' IDs to the list with all selected users *****/
   if (Gbl.Usrs.ListOtherRecipients[0])
     {
      /* Initialize structure with user's data */
      Usr_UsrDataConstructor (&UsrDat);

      /* Get next plain user's ID or nickname */
      Ptr = Gbl.Usrs.ListOtherRecipients;
      while (*Ptr)
        {
         /* Find next string in text until comma or semicolon (leading and trailing spaces are removed) */
         Str_GetNextStringUntilComma (&Ptr,UsrIDNickOrEmail,1024);

         /* Check if string is plain user's ID or nickname and get encrypted user's ID */
         if (UsrIDNickOrEmail[0])
           {
	    /***** Reset default list of users' codes *****/
	    ListUsrCods.NumUsrs = 0;
	    ListUsrCods.Lst = NULL;

	    if (Nck_CheckIfNickWithArrobaIsValid (UsrIDNickOrEmail))	// 1: It's a nickname
	      {
	       if ((UsrDat.UsrCod = Nck_GetUsrCodFromNickname (UsrIDNickOrEmail)) > 0)
		 {
		  ListUsrCods.NumUsrs = 1;
		  Usr_AllocateListUsrCods (&ListUsrCods);
		  ListUsrCods.Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;
		 }
	       else
		 {
		  if (WriteErrorMsgs)
		    {
		     sprintf (Gbl.Message,Txt_There_is_no_user_with_nickname_X,
			      UsrIDNickOrEmail);
		     Lay_ShowAlert (Lay_WARNING,Gbl.Message);
		    }
		  Error = true;
		 }
	      }
	    else if (Mai_CheckIfEmailIsValid (UsrIDNickOrEmail))	// 2: It's an e-mail
	      {
	       if ((UsrDat.UsrCod = Mai_GetUsrCodFromEmail (UsrIDNickOrEmail)) > 0)
		 {
		  ListUsrCods.NumUsrs = 1;
		  Usr_AllocateListUsrCods (&ListUsrCods);
		  ListUsrCods.Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;
		 }
	       else
		 {
		  if (WriteErrorMsgs)
		    {
		     sprintf (Gbl.Message,Txt_There_is_no_user_with_email_X,
			      UsrIDNickOrEmail);
		     Lay_ShowAlert (Lay_WARNING,Gbl.Message);
		    }
		  Error = true;
		 }
	      }
            else							// 3: It's not a nickname nor e-mail
              {
               // Users' IDs are always stored internally in capitals and without leading zeros
	       Str_RemoveLeadingZeros (UsrIDNickOrEmail);
	       Str_ConvertToUpperText (UsrIDNickOrEmail);
               if (ID_CheckIfUsrIDIsValid (UsrIDNickOrEmail))
		 {
		  // It seems a user's ID
		  /***** Allocate space for the list *****/
		  ID_ReallocateListIDs (&UsrDat,1);

		  strncpy (UsrDat.IDs.List[0].ID,UsrIDNickOrEmail,ID_MAX_LENGTH_USR_ID);
		  UsrDat.IDs.List[0].ID[ID_MAX_LENGTH_USR_ID] = '\0';

		  /***** Check if a user exists having this user's ID *****/
		  if (ID_GetListUsrCodsFromUsrID (&UsrDat,NULL,&ListUsrCods,false))
		    {
		     if (ListUsrCods.NumUsrs > 1)	// Two or more user share the same user's ID
		       {// TODO: Consider forbid IDs here
			if (WriteErrorMsgs)
			  {
			   sprintf (Gbl.Message,Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email,
				    UsrIDNickOrEmail);
			   Lay_ShowAlert (Lay_ERROR,Gbl.Message);
			  }
			Error = true;
		       }
		    }
		  else	// No users found
		    {
		     if (WriteErrorMsgs)
		       {
			sprintf (Gbl.Message,Txt_There_is_no_user_with_ID_nick_or_e_mail_X,
				 UsrIDNickOrEmail);
			Lay_ShowAlert (Lay_ERROR,Gbl.Message);
		       }
		     Error = true;
		    }
		 }
	       else	// String is not a valid user's nickname, e-mail or ID
		 {
		  if (WriteErrorMsgs)
		    {
		     sprintf (Gbl.Message,Txt_The_ID_nickname_or_email_X_is_not_valid,
			      UsrIDNickOrEmail);
		     Lay_ShowAlert (Lay_WARNING,Gbl.Message);
		    }
		  Error = true;
		 }
              }

            if (ListUsrCods.NumUsrs == 1)	// Only if user is valid
              {
               /* Get user's data */
	       Usr_GetUsrDataFromUsrCod (&UsrDat);	// Really only EncryptedUsrCod is needed

               /* Find if encrypted user's code is already in list */
               if (!Usr_FindUsrCodInListOfSelectedUsrs (UsrDat.EncryptedUsrCod))        // If not in list ==> add it
                 {
                  /* Add encrypted user's code to list of users */
                  if ((Length = strlen (Gbl.Usrs.Select.All)) == 0)        // First user in list
                    {
                     if (strlen (UsrDat.EncryptedUsrCod) < Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS)
                        strcpy (Gbl.Usrs.Select.All,UsrDat.EncryptedUsrCod);        // Add first user
                    }
                  else        // Not first user in list
                     if (Length < Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS + 1 + strlen (UsrDat.EncryptedUsrCod))
                       {
                        // Add another user
                        Gbl.Usrs.Select.All[Length] = Par_SEPARATOR_PARAM_MULTIPLE;
                        strcat (Gbl.Usrs.Select.All,UsrDat.EncryptedUsrCod);
                       }
                 }
              }

	    /***** Free list of users' codes *****/
	    Usr_FreeListUsrCods (&ListUsrCods);
           }
        }

      /* Free memory used for user's data */
      Usr_UsrDataDestructor (&UsrDat);
     }
   return Error;
  }

/*****************************************************************************/
/************** Find if encrypted user's code is yet in list *****************/
/*****************************************************************************/
// Returns true if EncryptedUsrCodToFind is in Gbl.Usrs.Select.All

bool Usr_FindUsrCodInListOfSelectedUsrs (const char *EncryptedUsrCodToFind)
  {
   const char *Ptr;
   char EncryptedUsrCod[Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64+1];

   if (Gbl.Usrs.Select.All)
     {
      Ptr = Gbl.Usrs.Select.All;
      while (*Ptr)
	{
	 Par_GetNextStrUntilSeparParamMult (&Ptr,EncryptedUsrCod,Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
	 if (!strcmp (EncryptedUsrCodToFind,EncryptedUsrCod))
	    return true;        // Found!
	}
     }
   return false;        // List not allocated or user not found
  }

/*****************************************************************************/
/************ Count number of valid users' IDs in encrypted list *************/
/*****************************************************************************/

unsigned Usr_CountNumUsrsInListOfSelectedUsrs (void)
  {
   const char *Ptr;
   unsigned NumUsrs = 0;
   struct UsrData UsrDat;

   /***** Loop over the list Gbl.Usrs.Select.All to count the number of users *****/
   Ptr = Gbl.Usrs.Select.All;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EncryptedUsrCod,Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
      if (UsrDat.UsrCod > 0)
         NumUsrs++;
     }
   return NumUsrs;
  }

/*****************************************************************************/
/********************* Allocate memory for list of users *********************/
/*****************************************************************************/

void Usr_AllocateListSelectedUsrCodAll (void)
  {
   if (!Gbl.Usrs.Select.All)
     {
      if ((Gbl.Usrs.Select.All = (char *) malloc (Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS+1)) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store list of users.");
      Gbl.Usrs.Select.All[0] = '\0';
     }
  }

/*****************************************************************************/
/****************** Allocate memory for list of students *********************/
/*****************************************************************************/

void Usr_AllocateListSelectedUsrCodStd (void)
  {
   if (!Gbl.Usrs.Select.Std)
     {
      if ((Gbl.Usrs.Select.Std = (char *) malloc (Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS + 1)) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store list of users.");
      Gbl.Usrs.Select.Std[0] = '\0';
     }
  }

/*****************************************************************************/
/************* Allocate memory for list of teachers or guests ****************/
/*****************************************************************************/

void Usr_AllocateListSelectedUsrCodTch (void)
  {
   if (!Gbl.Usrs.Select.Tch)
     {
      if ((Gbl.Usrs.Select.Tch = (char *) malloc (Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS + 1)) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store list of users.");
      Gbl.Usrs.Select.Tch[0] = '\0';
     }
  }

/*****************************************************************************/
/********************** Free memory for lists of users ***********************/
/*****************************************************************************/

void Usr_FreeListsSelectedUsrCods (void)
  {
   if (Gbl.Usrs.Select.All)
     {
      free ((void *) Gbl.Usrs.Select.All);
      Gbl.Usrs.Select.All = NULL;
     }
   if (Gbl.Usrs.Select.Tch)
     {
      free ((void *) Gbl.Usrs.Select.Tch);
      Gbl.Usrs.Select.Tch = NULL;
     }
   if (Gbl.Usrs.Select.Std)
     {
      free ((void *) Gbl.Usrs.Select.Std);
      Gbl.Usrs.Select.Std = NULL;
     }
  }

/*****************************************************************************/
/********** Allocate memory for list of users's IDs or nicknames *************/
/*****************************************************************************/

static void Usr_AllocateListOtherRecipients (void)
  {
   if (!Gbl.Usrs.ListOtherRecipients)
     {
      if ((Gbl.Usrs.ListOtherRecipients = malloc (Nck_MAX_BYTES_LIST_NICKS + 1)) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store list of recipients.");
      Gbl.Usrs.ListOtherRecipients[0] = '\0';
     }
  }

/*****************************************************************************/
/****************** Free memory for the list of nicknames ********************/
/*****************************************************************************/

void Usr_FreeListOtherRecipients (void)
  {
   if (Gbl.Usrs.ListOtherRecipients)
     {
      free ((void *) Gbl.Usrs.ListOtherRecipients);
      Gbl.Usrs.ListOtherRecipients = NULL;
     }
  }

/*****************************************************************************/
/*************************** Selection of list type **************************/
/*****************************************************************************/

void Usr_ShowFormsToSelectUsrListType (Act_Action_t NextAction)
  {
   /***** Select Usr_CLASS_PHOTO *****/
   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_5\" style=\"margin:4px auto;\">"
	              "<tr>"
	              "<td class=\"%s LEFT_MIDDLE\">",
            Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO ? "PREF_ON" :
        	                                      "PREF_OFF");
   Usr_FormToSelectUsrListType (NextAction,Usr_CLASS_PHOTO);

   /* Number of columns in the class photo */
   Act_FormStart (NextAction);
   Grp_PutParamsCodGrps ();
   Usr_PutParamUsrListType (Usr_CLASS_PHOTO);
   Usr_PutParamListWithPhotos ();
   Usr_PutExtraParamsUsrList (NextAction);
   Usr_PutSelectorNumColsClassPhoto ();
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>");

   /***** Select Usr_LIST *****/
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE\">",
            Gbl.Usrs.Me.ListType == Usr_LIST ? "PREF_ON" :
        	                               "PREF_OFF");
   Usr_FormToSelectUsrListType (NextAction,Usr_LIST);

   /* See the photos in list? */
   Act_FormStart (NextAction);
   Grp_PutParamsCodGrps ();
   Usr_PutParamUsrListType (Usr_LIST);
   Usr_PutExtraParamsUsrList (NextAction);
   Usr_PutCheckboxListWithPhotos ();
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>"
	              "</table>");
  }

/*****************************************************************************/
/************* Put a radio element to select a users' list type **************/
/*****************************************************************************/

static void Usr_FormToSelectUsrListType (Act_Action_t NextAction,Usr_ShowUsrsType_t ListType)
  {
   extern const char *The_ClassFormNoWrap[The_NUM_THEMES];
   extern const char *Txt_USR_LIST_TYPES[Usr_NUM_USR_LIST_TYPES];

   Act_FormStart (NextAction);
   Grp_PutParamsCodGrps ();
   Usr_PutParamListWithPhotos ();
   Usr_PutParamUsrListType (ListType);
   Usr_PutExtraParamsUsrList (NextAction);
   Act_LinkFormSubmit (Txt_USR_LIST_TYPES[ListType],The_ClassFormNoWrap[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"<img src=\"%s/%s\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICON20x20\" />"
                      " %s</a>",
            Gbl.Prefs.IconsURL,
            Usr_IconsClassPhotoOrList[ListType],
            Txt_USR_LIST_TYPES[ListType],
            Txt_USR_LIST_TYPES[ListType],
            Txt_USR_LIST_TYPES[ListType]);
   Act_FormEnd ();
  }

/*****************************************************************************/
/***************** Put extra parameters for a list of users ******************/
/*****************************************************************************/

void Usr_PutExtraParamsUsrList (Act_Action_t NextAction)
  {
   switch (Gbl.Action.Act)
     {
      case ActLstGst:
      case ActLstStd:
      case ActLstTch:
         Sco_PutParamScope (Gbl.Scope.Current);
         break;
      case ActSeeOneAtt:
      case ActRecAttStd:
         Att_PutParamAttCod (Gbl.AttEvents.AttCod);
         break;
      case ActReqMsgUsr:
         Usr_PutHiddenParUsrCodAll (NextAction,Gbl.Usrs.Select.All);
         Msg_PutHiddenParamOtherRecipients ();
         if (Gbl.Msg.Reply.IsReply)
           {
            Par_PutHiddenParamChar ("IsReply",'Y');
            Msg_PutHiddenParamMsgCod (Gbl.Msg.Reply.OriginalMsgCod);
           }
         if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
           {
            Usr_PutParamOtherUsrCodEncrypted ();
            if (Gbl.Msg.ShowOnlyOneRecipient)
               Par_PutHiddenParamChar ("ShowOnlyOneRecipient",'Y');
           }
         break;
      case ActSeeUseGbl:
         Pho_PutHiddenParamFigureType ();
         break;
      case ActSeePhoDeg:
         Pho_PutHiddenParamTypeOfAvg ();
         Pho_PutHiddenParamPhotoSize ();
         Pho_PutHiddenParamOrderDegrees ();
         break;
      default:
         break;
     }
  }

/*****************************************************************************/
/******************** List users to select some of them **********************/
/*****************************************************************************/

void Usr_ListUsersToSelect (Rol_Role_t Role)
  {
   /***** If there are no users, don't list anything *****/
   if (!Gbl.Usrs.LstUsrs[Role].NumUsrs)
      return;

   /***** Put a row to select all users *****/
   Usr_PutCheckboxToSelectAllTheUsers (Role);

   /***** Draw the classphoto/list *****/
   switch (Gbl.Usrs.Me.ListType)
     {
      case Usr_CLASS_PHOTO:
         Usr_DrawClassPhoto (Usr_CLASS_PHOTO_SEL,Role);
         break;
      case Usr_LIST:
         Usr_ListUsrsForSelection (Role);
         break;
     }
  }

/*****************************************************************************/
/****** Put a row, in a classphoto or a list, to select all the users ********/
/*****************************************************************************/

void Usr_PutCheckboxToSelectAllTheUsers (Rol_Role_t Role)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   Usr_Sex_t Sex;

   fprintf (Gbl.F.Out,"<tr>"
	              "<th colspan=\"%u\" class=\"LEFT_MIDDLE LIGHT_BLUE\">",
            Usr_GetColumnsForSelectUsrs ());
   if (Role == Rol_STUDENT)
      fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"SEL_UNSEL_STDS\" value=\"\""
	                 " onclick=\"togglecheckChildren(this,'UsrCodStd')\" />");
   else	// Role == Rol_TEACHER or Role == Rol__GUEST_
      fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"SEL_UNSEL_TCHS\" value=\"\""
	                 " onclick=\"togglecheckChildren(this,'UsrCodTch')\" />");
   Sex = Usr_GetSexOfUsrsLst (Role);
   fprintf (Gbl.F.Out,"%s:"
	              "</th>"
	              "</tr>",
	    Gbl.Usrs.LstUsrs[Role].NumUsrs == 1 ? Txt_ROLES_SINGUL_Abc[Role][Sex] :
                                                  Txt_ROLES_PLURAL_Abc[Role][Sex]);
  }

/*****************************************************************************/
/************************** Get sex of a list of users ***********************/
/*****************************************************************************/

static Usr_Sex_t Usr_GetSexOfUsrsLst (Rol_Role_t Role)
  {
   Usr_Sex_t Sex;
   unsigned NumUsr;

   /***** If no users, sex is undefined *****/
   if (Gbl.Usrs.LstUsrs[Role].NumUsrs == 0)
      return Usr_SEX_UNKNOWN;

   /***** Initialize sex to that of first user in list *****/
   Sex = Gbl.Usrs.LstUsrs[Role].Lst[0].Sex;

   /***** Search if there is one user at least with different sex than the first one *****/
   for (NumUsr = 1;
        NumUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs;
        NumUsr++)
      if (Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Sex != Sex)
         return Usr_SEX_UNKNOWN;

   return Sex;
  }

/*****************************************************************************/
/**** Get number of table columns, in classphoto or list, to select users ****/
/*****************************************************************************/

unsigned Usr_GetColumnsForSelectUsrs (void)
  {
   return (Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO) ? Gbl.Usrs.ClassPhoto.Cols :
                                                      (Gbl.Usrs.Listing.WithPhotos ? 1 + Usr_NUM_MAIN_FIELDS_DATA_USR :
                                                                                         Usr_NUM_MAIN_FIELDS_DATA_USR);
  }

/*****************************************************************************/
/******* Put a checkbox, in a classphoto or a list, to select a user *********/
/*****************************************************************************/

static void Usr_PutCheckboxToSelectUser (struct UsrData *UsrDat,bool UsrIsTheMsgSender)
  {
   bool CheckboxChecked;

   /***** Check box *****/
   fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"");
   if (UsrDat->RoleInCurrentCrsDB == Rol_STUDENT)
      fprintf (Gbl.F.Out,"UsrCodStd\" value=\"%s\""
	                 " onclick=\"checkParent(this,'SEL_UNSEL_STDS')\"",
	       UsrDat->EncryptedUsrCod);
   else	// Role == Rol_TEACHER or Role == Rol__GUEST_
      fprintf (Gbl.F.Out,"UsrCodTch\" value=\"%s\""
	                 " onclick=\"checkParent(this,'SEL_UNSEL_TCHS')\"",
	       UsrDat->EncryptedUsrCod);

   /***** Check box must be checked? *****/
   CheckboxChecked = false;
   if (UsrIsTheMsgSender)
      CheckboxChecked = true;
   else
      /* Check if user is in lists of selected users */
      CheckboxChecked = Usr_FindUsrCodInListOfSelectedUsrs (UsrDat->EncryptedUsrCod);
   if (CheckboxChecked)
      fprintf (Gbl.F.Out," checked=\"checked\"");

   fprintf (Gbl.F.Out," />");
  }

/*****************************************************************************/
/********* Put a checkbox to select whether list users with photos ***********/
/*****************************************************************************/

static void Usr_PutCheckboxListWithPhotos (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Display_photos;

   Par_PutHiddenParamChar ("WithPhotosExists",'Y');

   /***** Put checkbox to select whether list users with photos *****/
   fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"WithPhotos\" value=\"Y\"");
   if (Gbl.Usrs.Listing.WithPhotos)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   fprintf (Gbl.F.Out," onclick=\"document.getElementById('%s').submit();\" />"
                      "<span class=\"%s\">%s</span>",
	    Gbl.Form.Id,
            The_ClassForm[Gbl.Prefs.Theme],Txt_Display_photos);
  }

/*****************************************************************************/
/*********************** Set field names of user's data **********************/
/*****************************************************************************/

void Usr_SetUsrDatMainFieldNames (void)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Photo;
   extern const char *Txt_ID;
   extern const char *Txt_Surname_1;
   extern const char *Txt_Surname_2;
   extern const char *Txt_First_name;
   extern const char *Txt_Email;
   extern const char *Txt_Institution;

   /***** Initialize field names *****/
   Usr_UsrDatMainFieldNames[0] = "&nbsp;";
   Usr_UsrDatMainFieldNames[1] = Txt_No_INDEX;
   Usr_UsrDatMainFieldNames[2] = Txt_Photo;
   Usr_UsrDatMainFieldNames[3] = Txt_ID;
   Usr_UsrDatMainFieldNames[4] = Txt_Surname_1;
   Usr_UsrDatMainFieldNames[5] = Txt_Surname_2;
   Usr_UsrDatMainFieldNames[6] = Txt_First_name;
   Usr_UsrDatMainFieldNames[7] = Txt_Email;
   Usr_UsrDatMainFieldNames[8] = Txt_Institution;
  }

/*****************************************************************************/
/************ Write header with main field names of user's data **************/
/*****************************************************************************/

void Usr_WriteHeaderFieldsUsrDat (bool PutCheckBoxToSelectUsr)
  {
   unsigned NumCol;

   fprintf (Gbl.F.Out,"<tr>");

   /***** First column used for selection *****/
   if (PutCheckBoxToSelectUsr)
      fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE LIGHT_BLUE\">"
			 "</th>");

   /***** Columns for user's data fields *****/
   for (NumCol = 0;
        NumCol < Usr_NUM_MAIN_FIELDS_DATA_USR;
        NumCol++)
      if (NumCol != 2 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
         fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE LIGHT_BLUE\">"
                            "%s&nbsp;"
                            "</th>",
                  Usr_UsrDatMainFieldNames[NumCol]);

   fprintf (Gbl.F.Out,"</tr>");
  }

/*****************************************************************************/
/************************** List guests' main data ***************************/
/*****************************************************************************/

static void Usr_ListMainDataGsts (bool PutCheckBoxToSelectUsr)
  {
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Initialize field names *****/
   Usr_SetUsrDatMainFieldNames ();

   if (Gbl.Usrs.LstUsrs[Rol__GUEST_].NumUsrs)
     {
      /***** Heading row with column names *****/
      Usr_WriteHeaderFieldsUsrDat (PutCheckBoxToSelectUsr);	// Columns for the data

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List students' data *****/
      for (NumUsr = 0, Gbl.RowEvenOdd = 0;
           NumUsr < Gbl.Usrs.LstUsrs[Rol__GUEST_].NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Rol__GUEST_].Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = false;	// Guests have no courses,...
                                        // ...so they have not accepted...
            	    	    	    	// ...inscription in any course
            Usr_WriteRowUsrMainData (++NumUsr,&UsrDat,true);

            Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else        // Gbl.Usrs.LstUsrs[Rol__GUEST_].NumUsrs == 0
      Usr_ShowWarningNoUsersFound (Rol__GUEST_);

   /***** Free memory for guests list *****/
   Usr_FreeUsrsList (Rol__GUEST_);
  }

/*****************************************************************************/
/*************************** List main students' data ************************/
/*****************************************************************************/

static void Usr_ListMainDataStds (bool PutCheckBoxToSelectUsr)
  {
   unsigned NumUsr;
   char *GroupNames;
   struct UsrData UsrDat;

   /***** Initialize field names *****/
   Usr_SetUsrDatMainFieldNames ();

   GroupNames = NULL;        // To avoid warning

   if (Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs)
     {
      /***** Allocate memory for the string with the list of group names where student belongs to *****/
      if ((GroupNames = (char *) malloc ((MAX_LENGTH_GROUP_NAME+3)*Gbl.CurrentCrs.Grps.GrpTypes.NumGrpsTotal)) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store names of groups.");

      /***** Start table with list of students *****/
      if (!Gbl.Usrs.ClassPhoto.AllGroups)
        {
         fprintf (Gbl.F.Out,"<tr>"
                            "<td colspan=\"%u\" class=\"TIT CENTER_MIDDLE\">",
                  1 + Usr_NUM_MAIN_FIELDS_DATA_USR);
         Grp_WriteNamesOfSelectedGrps ();
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");
        }

      /***** Heading row with column names *****/
      Usr_WriteHeaderFieldsUsrDat (PutCheckBoxToSelectUsr);	// Columns for the data

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List students' data *****/
      for (NumUsr = 0, Gbl.RowEvenOdd = 0;
           NumUsr < Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Rol_STUDENT].Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = Gbl.Usrs.LstUsrs[Rol_STUDENT].Lst[NumUsr].Accepted;
            Usr_WriteRowUsrMainData (++NumUsr,&UsrDat,PutCheckBoxToSelectUsr);

            Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** Free memory used by the string with the list of group names where student belongs to *****/
      free ((void *) GroupNames);
     }
   else        // Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs == 0
      Usr_ShowWarningNoUsersFound (Rol_STUDENT);

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (Rol_STUDENT);
  }

/*****************************************************************************/
/************************** List main teachers' data *************************/
/*****************************************************************************/

static void Usr_ListMainDataTchs (bool PutCheckBoxToSelectUsr)
  {
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   unsigned NumColumns;
   unsigned NumCol;
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Initialize field names *****/
   Usr_SetUsrDatMainFieldNames ();

   if (Gbl.Usrs.LstUsrs[Rol_TEACHER].NumUsrs)
     {
      /***** Initialize number of columns *****/
      NumColumns = Usr_NUM_MAIN_FIELDS_DATA_USR;

      /***** Heading row with column names *****/
      /* Start row */
      fprintf (Gbl.F.Out,"<tr>");

      /* First column used for selection  */
      if (PutCheckBoxToSelectUsr)
	 fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE LIGHT_BLUE\">"
	                    "&nbsp;"
	                    "</th>");

      /* Columns for the data */
      for (NumCol = 0;
           NumCol < NumColumns;
           NumCol++)
         if (NumCol != 2 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
            fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE LIGHT_BLUE\">"
        	               "%s&nbsp;"
        	               "</th>",
                     Usr_UsrDatMainFieldNames[NumCol]);

      /* End row */
      fprintf (Gbl.F.Out,"</tr>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List data of teachers *****/
      for (NumUsr = 0, Gbl.RowEvenOdd = 0;
           NumUsr < Gbl.Usrs.LstUsrs[Rol_TEACHER].NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Rol_TEACHER].Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = Gbl.Usrs.LstUsrs[Rol_TEACHER].Lst[NumUsr].Accepted;
            Usr_WriteRowUsrMainData (++NumUsr,&UsrDat,PutCheckBoxToSelectUsr);

            Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else        // Gbl.Usrs.LstUsrs[Rol_TEACHER].NumUsrs == 0
      Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_TEACHER]);

   /***** Free memory for teachers list *****/
   Usr_FreeUsrsList (Rol_TEACHER);
  }

/*****************************************************************************/
/***************************** List guests' data *****************************/
/*****************************************************************************/

void Usr_ListAllDataGsts (void)
  {
   extern const char *Txt_Photo;
   extern const char *Txt_ID;
   extern const char *Txt_Surname_1;
   extern const char *Txt_Surname_2;
   extern const char *Txt_First_name;
   extern const char *Txt_Email;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Department;
   extern const char *Txt_Office;
   extern const char *Txt_Phone;
   extern const char *Txt_Local_address;
   extern const char *Txt_Family_address;
   extern const char *Txt_Place_of_origin;
   extern const char *Txt_Date_of_birth;
   unsigned NumColumnsCommonCard;
   unsigned NumCol;
   unsigned NumUsr;
   struct UsrData UsrDat;
   const char *FieldNames[Usr_NUM_ALL_FIELDS_DATA_INV];

   /***** Initialize field names *****/
   FieldNames[ 0] = Txt_Photo;
   FieldNames[ 1] = Txt_ID;
   FieldNames[ 2] = Txt_Surname_1;
   FieldNames[ 3] = Txt_Surname_2;
   FieldNames[ 4] = Txt_First_name;
   FieldNames[ 5] = Txt_Email;
   FieldNames[ 6] = Txt_Institution;
   FieldNames[ 7] = Txt_Centre;
   FieldNames[ 8] = Txt_Department;
   FieldNames[ 9] = Txt_Office;
   FieldNames[10] = Txt_Phone;
   FieldNames[11] = Txt_Local_address;
   FieldNames[12] = Txt_Phone;
   FieldNames[13] = Txt_Family_address;
   FieldNames[14] = Txt_Phone;
   FieldNames[15] = Txt_Place_of_origin;
   FieldNames[16] = Txt_Date_of_birth;

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Sco_SetScopesForListingGuests ();
   Sco_GetScope ();

   /****** Get and order list of guests ******/
   Usr_GetGstsLst (Gbl.Scope.Current);

   if (Gbl.Usrs.LstUsrs[Rol__GUEST_].NumUsrs)
     {
      /***** Set number of columns *****/
      NumColumnsCommonCard = Usr_NUM_ALL_FIELDS_DATA_INV;

      /***** Start table with list of guests *****/
      /* Start row */
      fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">"
                         "<tr>");

      /* Columns for the data */
      for (NumCol = (Gbl.Usrs.Listing.WithPhotos ? 0 :
	                                           1);
           NumCol < NumColumnsCommonCard;
           NumCol++)
         fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE LIGHT_BLUE\">"
                            "%s&nbsp;"
                            "</th>",
                  FieldNames[NumCol]);

      /* End row */
      fprintf (Gbl.F.Out,"</tr>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List guests' data *****/
      for (NumUsr = 0, Gbl.RowEvenOdd = 0;
           NumUsr < Gbl.Usrs.LstUsrs[Rol__GUEST_].NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Rol__GUEST_].Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = false;	// Guests have no courses,...
            	    	    	    	// ...so they have not accepted...
                                        // ...inscription in any course
            NumUsr++;
            Usr_WriteRowGstAllData (&UsrDat);

            Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** End of table *****/
      fprintf (Gbl.F.Out,"</table>");
     }
   else        // Gbl.Usrs.LstUsrs[Rol__GUEST_].NumUsrs == 0
      Usr_ShowWarningNoUsersFound (Rol__GUEST_);

   /***** Free memory for guests list *****/
   Usr_FreeUsrsList (Rol__GUEST_);
  }

/*****************************************************************************/
/***************************** List students' data ***************************/
/*****************************************************************************/

void Usr_ListAllDataStds (void)
  {
   extern const char *Txt_Photo;
   extern const char *Txt_ID;
   extern const char *Txt_Surname_1;
   extern const char *Txt_Surname_2;
   extern const char *Txt_First_name;
   extern const char *Txt_Email;
   extern const char *Txt_Institution;
   extern const char *Txt_Local_address;
   extern const char *Txt_Phone;
   extern const char *Txt_Family_address;
   extern const char *Txt_Place_of_origin;
   extern const char *Txt_Date_of_birth;
   extern const char *Txt_Group;
   extern const char *Txt_RECORD_FIELD_VISIBILITY_RECORD[Rec_NUM_TYPES_VISIBILITY];
   unsigned NumColumnsCommonCard;
   unsigned NumColumnsCardAndGroups;
   unsigned NumColumnsTotal;
   unsigned NumCol;
   unsigned NumUsr;
   char *GroupNames;
   unsigned NumGrpTyp,NumField;
   struct UsrData UsrDat;
   const char *FieldNames[Usr_NUM_ALL_FIELDS_DATA_STD];

   /***** Initialize field names *****/
   FieldNames[ 0] = Txt_Photo;
   FieldNames[ 1] = Txt_ID;
   FieldNames[ 2] = Txt_Surname_1;
   FieldNames[ 3] = Txt_Surname_2;
   FieldNames[ 4] = Txt_First_name;
   FieldNames[ 5] = Txt_Email;
   FieldNames[ 6] = Txt_Institution;
   FieldNames[ 7] = Txt_Local_address;
   FieldNames[ 8] = Txt_Phone;
   FieldNames[ 9] = Txt_Family_address;
   FieldNames[10] = Txt_Phone;
   FieldNames[11] = Txt_Place_of_origin;
   FieldNames[12] = Txt_Date_of_birth;

   GroupNames = NULL;        // To avoid warning

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get list of groups types and groups in current course *****/
   Grp_GetListGrpTypesInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   /***** Get groups to show *****/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get scope *****/
   Sco_SetScopesForListingStudents ();
   Sco_GetScope ();

   /****** Get and order list of students in current course ******/
   Usr_GetListUsrs (Rol_STUDENT,Gbl.Scope.Current);

   if (Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs)
     {
      if (Gbl.Scope.Current == Sco_SCOPE_CRS)
         /***** Get list of record fields in current course *****/
         Rec_GetListRecordFieldsInCurrentCrs ();

      /***** Set number of columns *****/
      NumColumnsCommonCard = Usr_NUM_ALL_FIELDS_DATA_STD;
      if (Gbl.Scope.Current == Sco_SCOPE_CRS)
        {
         NumColumnsCardAndGroups = NumColumnsCommonCard + Gbl.CurrentCrs.Grps.GrpTypes.Num;
         NumColumnsTotal = NumColumnsCardAndGroups + Gbl.CurrentCrs.Records.LstFields.Num;
        }
      else
         NumColumnsTotal = NumColumnsCardAndGroups = NumColumnsCommonCard;

      /***** Allocate memory for the string with the list of group names where student belongs to *****/
      if (Gbl.Scope.Current == Sco_SCOPE_CRS)
         if ((GroupNames = (char *) malloc ((MAX_LENGTH_GROUP_NAME+3)*Gbl.CurrentCrs.Grps.GrpTypes.NumGrpsTotal)) == NULL)
            Lay_ShowErrorAndExit ("Not enough memory to store names of groups.");

      /***** Start table with list of students *****/
      fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">");
      if (!Gbl.Usrs.ClassPhoto.AllGroups)
        {
         fprintf (Gbl.F.Out,"<tr>"
	                    "<td colspan=\"%u\" class=\"TIT CENTER_MIDDLE\">",
                  NumColumnsTotal);
         Grp_WriteNamesOfSelectedGrps ();
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");
        }

      /***** Heading row with column names *****/
      /* Start row */
      fprintf (Gbl.F.Out,"<tr>");

      /* 1. Columns for the data */
      for (NumCol = (Gbl.Usrs.Listing.WithPhotos ? 0 :
	                                           1);
           NumCol < NumColumnsCommonCard;
           NumCol++)
         fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE LIGHT_BLUE\">"
                            "%s&nbsp;"
                            "</th>",
                  FieldNames[NumCol]);

      /* 2. Columns for the groups */
      if (Gbl.Scope.Current == Sco_SCOPE_CRS)
        {
         if (Gbl.CurrentCrs.Grps.GrpTypes.Num)
            for (NumGrpTyp = 0;
                 NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
                 NumGrpTyp++)
               if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)         // If current course tiene groups of este type
                  fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE LIGHT_BLUE\">"
                	             "%s %s&nbsp;"
                	             "</th>",
                           Txt_Group,
                           Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName);

         if (Gbl.CurrentCrs.Records.LstFields.Num)
           {
            /* 3. Names of record fields that depend on the course */
            for (NumField = 0;
                 NumField < Gbl.CurrentCrs.Records.LstFields.Num;
                 NumField++)
               fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE LIGHT_BLUE\">"
        	                  "%s&nbsp;"
        	                  "</th>",
                        Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Name);

            /* 4. Visibility type for the record fields that depend on the course, in other row */
            fprintf (Gbl.F.Out,"</tr>"
        	               "<tr>");
            for (NumCol = 0;
                 NumCol < NumColumnsCardAndGroups;
                 NumCol++)
               if (NumCol != 1 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want it in listing
                  fprintf (Gbl.F.Out,"<td class=\"VERY_LIGHT_BLUE\">"
                	             "</td>");
            for (NumField = 0;
                 NumField < Gbl.CurrentCrs.Records.LstFields.Num;
                 NumField++)
               fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE VERY_LIGHT_BLUE\">"
        	                  "(%s)&nbsp;"
        	                  "</th>",
                        Txt_RECORD_FIELD_VISIBILITY_RECORD[Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Visibility]);
           }
        }

      /* End row */
      fprintf (Gbl.F.Out,"</tr>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List students' data *****/
      for (NumUsr = 0, Gbl.RowEvenOdd = 0;
           NumUsr < Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Rol_STUDENT].Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = Gbl.Usrs.LstUsrs[Rol_STUDENT].Lst[NumUsr].Accepted;
            NumUsr++;
            Usr_WriteRowStdAllData (&UsrDat,GroupNames);

            Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** End of table *****/
      fprintf (Gbl.F.Out,"</table>");

      /***** Free memory used by the string with the list of group names where student belongs to *****/
      if (Gbl.Scope.Current == Sco_SCOPE_CRS)
         free ((void *) GroupNames);
     }
   else        // Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs == 0
      Usr_ShowWarningNoUsersFound (Rol_STUDENT);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (Rol_STUDENT);

   /***** Free list of groups types and groups in current course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/*************** List users (of current course) for selection ****************/
/*****************************************************************************/

void Usr_ListUsrsForSelection (Rol_Role_t Role)
  {
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Initialize field names *****/
   Usr_SetUsrDatMainFieldNames ();

   /***** Heading row with column names *****/
   Usr_WriteHeaderFieldsUsrDat (true);	// Columns for the data

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** List users' data *****/
   for (NumUsr = 0, Gbl.RowEvenOdd = 0;
	NumUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs; )
     {
      UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].UsrCod;
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
	{
	 UsrDat.Accepted = Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Accepted;
	 Usr_WriteRowUsrMainData (++NumUsr,&UsrDat,true);

	 Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
	}
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************************** List all teachers' data **************************/
/*****************************************************************************/

void Usr_ListAllDataTchs (void)
  {
   extern const char *Txt_Photo;
   extern const char *Txt_ID;
   extern const char *Txt_Surname_1;
   extern const char *Txt_Surname_2;
   extern const char *Txt_First_name;
   extern const char *Txt_Email;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Department;
   extern const char *Txt_Office;
   extern const char *Txt_Phone;
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   unsigned NumColumns;
   unsigned NumCol;
   unsigned NumUsr;
   struct UsrData UsrDat;
   const char *FieldNames[Usr_NUM_ALL_FIELDS_DATA_TCH];

   /***** Initialize field names *****/
   FieldNames[ 0] = Txt_Photo;
   FieldNames[ 1] = Txt_ID;
   FieldNames[ 2] = Txt_Surname_1;
   FieldNames[ 3] = Txt_Surname_2;
   FieldNames[ 4] = Txt_First_name;
   FieldNames[ 5] = Txt_Email;
   FieldNames[ 6] = Txt_Institution;
   FieldNames[ 7] = Txt_Centre;
   FieldNames[ 8] = Txt_Department;
   FieldNames[ 9] = Txt_Office;
   FieldNames[10] = Txt_Phone;

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	               1 << Sco_SCOPE_CTY |
                       1 << Sco_SCOPE_INS |
                       1 << Sco_SCOPE_CTR |
                       1 << Sco_SCOPE_DEG |
                       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   Sco_GetScope ();

   /***** Get and order list of teachers *****/
   Usr_GetListUsrs (Rol_TEACHER,Gbl.Scope.Current);

   if (Gbl.Usrs.LstUsrs[Rol_TEACHER].NumUsrs)
     {
      /***** Initialize number of columns *****/
      NumColumns = Usr_NUM_ALL_FIELDS_DATA_TCH;

      /***** Start table with list of teachers *****/
      /* Start row */
      fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">"
                         "<tr>");
      for (NumCol = (Gbl.Usrs.Listing.WithPhotos ? 0 :
	                                           1);
           NumCol < NumColumns;
           NumCol++)
         fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE LIGHT_BLUE\">"
                            "%s&nbsp;"
                            "</th>",
                  FieldNames[NumCol]);

      /* End row */
      fprintf (Gbl.F.Out,"</tr>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List data of teachers *****/
      for (NumUsr = 0, Gbl.RowEvenOdd = 0;
           NumUsr < Gbl.Usrs.LstUsrs[Rol_TEACHER].NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Rol_TEACHER].Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = Gbl.Usrs.LstUsrs[Rol_TEACHER].Lst[NumUsr].Accepted;
            NumUsr++;
            Usr_WriteRowTchAllData (&UsrDat);

            Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** End of table *****/
      fprintf (Gbl.F.Out,"</table>");
     }
   else        // Gbl.Usrs.LstUsrs[Rol_TEACHER].NumUsrs == 0
      Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_TEACHER]);

   /***** Free memory for teachers list *****/
   Usr_FreeUsrsList (Rol_TEACHER);
  }

/*****************************************************************************/
/****************************** List users found *****************************/
/*****************************************************************************/
// Returns number of users found

unsigned Usr_ListUsrsFound (Rol_Role_t Role,const char *UsrQuery)
  {
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   Usr_Sex_t Sex;
   unsigned NumUsr;
   struct UsrData UsrDat;
   unsigned NumUsrs;

   /***** Initialize field names *****/
   Usr_SetUsrDatMainFieldNames ();

   /***** Search for users *****/
   Usr_SearchListUsrs (Role,UsrQuery);
   if ((NumUsrs = Gbl.Usrs.LstUsrs[Role].NumUsrs))
     {
      /***** Write heading *****/
      /* Number of users found */
      Sex = Usr_GetSexOfUsrsLst (Role);
      sprintf (Gbl.Title,"%u %s",
	       NumUsrs,(NumUsrs == 1) ? Txt_ROLES_SINGUL_abc[Role][Sex] :
		                        Txt_ROLES_PLURAL_abc[Role][Sex]);
      Lay_StartRoundFrameTable (NULL,2,Gbl.Title);

      /***** Heading row with column names *****/
      Gbl.Usrs.Listing.WithPhotos = true;
      Usr_WriteHeaderFieldsUsrDat (false);	// Columns for the data

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List data of users *****/
      for (NumUsr = 0, Gbl.RowEvenOdd = 0;
           NumUsr < NumUsrs;
           NumUsr++)
        {
         UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Accepted;

            /* Write data of this user */
            Usr_WriteRowUsrMainData (NumUsr + 1,&UsrDat,false);

	    /* Write all the courses this user belongs to */
            if (Role != Rol__GUEST_)
              {
	       fprintf (Gbl.F.Out,"<tr>"
			          "<td colspan=\"2\" class=\"COLOR%u\"></td>"
			          "<td colspan=\"%u\" class=\"COLOR%u\">",
		        Gbl.RowEvenOdd,
		        Usr_NUM_MAIN_FIELDS_DATA_USR-2,
		        Gbl.RowEvenOdd);
	       Crs_GetAndWriteCrssOfAUsr (&UsrDat,Role);
	       fprintf (Gbl.F.Out,"</td>"
				  "</tr>");
              }

            Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** End table *****/
      Lay_EndRoundFrameTable ();
     }

   /***** Free memory for teachers list *****/
   Usr_FreeUsrsList (Role);

   return NumUsrs;
  }

/*****************************************************************************/
/**************************** List administrators ****************************/
/*****************************************************************************/

void Usr_ListDataAdms (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Scope;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Photo;
   extern const char *Txt_ID;
   extern const char *Txt_Surname_1;
   extern const char *Txt_Surname_2;
   extern const char *Txt_First_name;
   extern const char *Txt_Email;
   extern const char *Txt_Institution;
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   unsigned NumCol;
   unsigned NumUsr;
   struct UsrData UsrDat;
   const char *FieldNames[Usr_NUM_MAIN_FIELDS_DATA_ADM];

   /***** Put contextual links *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

      /* Put link to remove old users */
      Usr_PutLinkToSeeGuests ();

      /* Put link to go to admin one user */
      Enr_PutLinkToAdminOneUsr (ActReqMdfOneOth);

      /* Put link to list possible duplicate users */
      Dup_PutLinkToListDupUsrs ();

      /* Put link to remove old users */
      Enr_PutLinkToRemOldUsrs ();

      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Initialize field names *****/
   FieldNames[0] = Txt_No_INDEX;
   FieldNames[1] = Txt_Photo;
   FieldNames[2] = Txt_ID;
   FieldNames[3] = Txt_Surname_1;
   FieldNames[4] = Txt_Surname_2;
   FieldNames[5] = Txt_First_name;
   FieldNames[6] = Txt_Email;
   FieldNames[7] = Txt_Institution;

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	               1 << Sco_SCOPE_CTY |
                       1 << Sco_SCOPE_INS |
                       1 << Sco_SCOPE_CTR |
                       1 << Sco_SCOPE_DEG;
   Gbl.Scope.Default = Sco_SCOPE_DEG;
   Sco_GetScope ();

   /***** Get and order list of administrators *****/
   Usr_GetAdmsLst (Gbl.Scope.Current);

   /***** Start frame with list of administrators *****/
   Lay_StartRoundFrame (NULL,Txt_ROLES_PLURAL_Abc[Rol_DEG_ADM][Usr_SEX_UNKNOWN],NULL);

   /***** Form to select range of administrators *****/
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">"
		      "<label class=\"%s\">%s: </label>",
	    The_ClassForm[Gbl.Prefs.Theme],Txt_Scope);
   Act_FormStart (ActLstOth);
   Sco_PutSelectorScope (true);
   Usr_PutParamListWithPhotos ();
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");

   if (Gbl.Usrs.LstUsrs[Rol_DEG_ADM].NumUsrs)
     {
      /****** Show photos? *****/
      fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\""
	                 " style=\"margin-bottom:8px;\">");
      Act_FormStart (ActLstOth);
      Sco_PutParamScope (Gbl.Scope.Current);
      Usr_PutCheckboxListWithPhotos ();
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");

      /***** Heading row with column names *****/
      fprintf (Gbl.F.Out,"<table>"
	                 "<tr>");
      for (NumCol = 0;
           NumCol < Usr_NUM_MAIN_FIELDS_DATA_ADM;
           NumCol++)
         if (NumCol != 1 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
            fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE LIGHT_BLUE\">"
        	               "%s&nbsp;"
        	               "</th>",
                     FieldNames[NumCol]);
      fprintf (Gbl.F.Out,"</tr>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List data of administrators *****/
      for (NumUsr = 0, Gbl.RowEvenOdd = 0;
           NumUsr < Gbl.Usrs.LstUsrs[Rol_DEG_ADM].NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Rol_DEG_ADM].Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = Gbl.Usrs.LstUsrs[Rol_DEG_ADM].Lst[NumUsr].Accepted;
            Usr_WriteRowAdmData (++NumUsr,&UsrDat);

            Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** End of table *****/
      fprintf (Gbl.F.Out,"</table>");
     }
   else        // Gbl.Usrs.LstUsrs[Rol_DEG_ADM].NumUsrs == 0
      Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_DEG_ADM]);

   /***** End frame *****/
   Lay_EndRoundFrame ();

   /***** Free memory for administrators list *****/
   Usr_FreeUsrsList (Rol_DEG_ADM);
  }

/*****************************************************************************/
/********************* Get and update type of list,     **********************/
/********************* number of columns in class photo **********************/
/********************* and preference about view photos **********************/
/*****************************************************************************/

void Usr_GetAndUpdatePrefsAboutUsrList (void)
  {
   /***** Get and update type of list *****/
   Usr_GetAndUpdateUsrListType ();

   /***** Get and update number of columns in class photo *****/
   Usr_GetAndUpdateColsClassPhoto ();

   /***** Get and update preference about view photos *****/
   Usr_GetAndUpdatePrefAboutListWithPhotos ();
  }

/*****************************************************************************/
/****************** Get from form the type of users' list ********************/
/*****************************************************************************/

static void Usr_GetAndUpdateUsrListType (void)
  {
   /***** Get type of list used to select users from form *****/
   if (Usr_GetUsrListTypeFromForm ())
      /* Save in the database the type of list preferred by me */
      Usr_UpdateMyUsrListTypeInDB ();
   else
      /* If parameter can't be retrieved from, get my preference from database */
      Usr_GetMyUsrListTypeFromDB ();
  }

/*****************************************************************************/
/************* Put a hidden parameter with the users' list type **************/
/*****************************************************************************/

void Usr_PutParamUsrListType (Usr_ShowUsrsType_t ListType)
  {
   Par_PutHiddenParamUnsigned ("UsrListType",(unsigned) ListType);
  }

/*****************************************************************************/
/****************** Get from form the type of users' list ********************/
/*****************************************************************************/

static bool Usr_GetUsrListTypeFromForm (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Gbl.Usrs.Me.ListType = Usr_SHOW_USRS_TYPE_DEFAULT;

   /***** Get param with number of users per row *****/
   Par_GetParToText ("UsrListType",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) != 1)
      return false;
   if (UnsignedNum >= Usr_NUM_USR_LIST_TYPES)
      return false;
   Gbl.Usrs.Me.ListType = UnsignedNum;
   return true;
  }

/*****************************************************************************/
/************** Get my preference about type of users' list ******************/
/*****************************************************************************/

static void Usr_GetMyUsrListTypeFromDB (void)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   Usr_ShowUsrsType_t ListType;

   /***** Get type of listing of users from database *****/
   sprintf (Query,"SELECT UsrListType FROM crs_usr"
	          " WHERE CrsCod='%ld' AND UsrCod='%ld'",
            Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get type of listing of users");

   if (NumRows == 1)                // Should be one only row
     {
      /* Get type of users' listing used to select some of them */
      Gbl.Usrs.Me.ListType = Usr_SHOW_USRS_TYPE_DEFAULT;
      row = mysql_fetch_row (mysql_res);
      if (row[0])
         for (ListType = (Usr_ShowUsrsType_t) 0;
              ListType < Usr_NUM_USR_LIST_TYPES;
              ListType++)
            if (!strcasecmp (row[0],Usr_StringsUsrListTypeInDB[ListType]))
              {
               Gbl.Usrs.Me.ListType = ListType;
               break;
              }
     }
   else if (NumRows == 0)        // If I am an administrator or superuser and I don't belong to current course, then the result will be the default
      Gbl.Usrs.Me.ListType = Usr_SHOW_USRS_TYPE_DEFAULT;
   else        // Error in databse: more than one row for a user in course
      Lay_ShowErrorAndExit ("Error when getting type of listing of users.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Save my preference about type of users' list **************/
/*****************************************************************************/

static void Usr_UpdateMyUsrListTypeInDB (void)
  {
   char Query[512];

   /***** Update type of users listing *****/
   sprintf (Query,"UPDATE crs_usr SET UsrListType='%s'"
                  " WHERE CrsCod='%ld' AND UsrCod='%ld'",
            Usr_StringsUsrListTypeInDB[Gbl.Usrs.Me.ListType],
            Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update type of listing");
  }

/*****************************************************************************/
/************* Get and update number of columns in class photo ***************/
/*****************************************************************************/

void Usr_GetAndUpdateColsClassPhoto (void)
  {
   /***** Get the number of columns in class photo from form *****/
   if (Usr_GetParamColsClassPhotoFromForm ())
      /***** Save the number of columns into the database  *****/
      Usr_UpdateMyColsClassPhotoInDB ();
   else
      /***** If parameter can't be retrieved from form, get my preference from database *****/
      Usr_GetMyColsClassPhotoFromDB ();
  }

/*****************************************************************************/
/****** Put a hidden parameter with the number of colums in class photo ******/
/*****************************************************************************/

void Usr_PutParamColsClassPhoto (void)
  {
   Par_PutHiddenParamUnsigned ("ColsClassPhoto",Gbl.Usrs.ClassPhoto.Cols);
  }

/*****************************************************************************/
/************* Get from form the number of colums in class photo *************/
/*****************************************************************************/

static bool Usr_GetParamColsClassPhotoFromForm (void)
  {
   char UnsignedStr[10+1];

   /***** Get parameter with number of users per row *****/
   Par_GetParToText ("ColsClassPhoto",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Gbl.Usrs.ClassPhoto.Cols) != 1)
     {
      Gbl.Usrs.ClassPhoto.Cols = Usr_CLASS_PHOTO_COLS_DEF;
      return false;
     }
   if (Gbl.Usrs.ClassPhoto.Cols < 1 ||
       Gbl.Usrs.ClassPhoto.Cols > Usr_CLASS_PHOTO_COLS_MAX)
      Gbl.Usrs.ClassPhoto.Cols = Usr_CLASS_PHOTO_COLS_DEF;
   return true;
  }

/*****************************************************************************/
/** Get my prefs. about number of colums in class photo for current course ***/
/*****************************************************************************/

static void Usr_GetMyColsClassPhotoFromDB (void)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   Gbl.Usrs.ClassPhoto.Cols = Usr_CLASS_PHOTO_COLS_DEF;

   /***** If user logged and course selected... *****/
   if (Gbl.Usrs.Me.Logged && Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      /***** Get number of columns in class photo from database *****/
      sprintf (Query,"SELECT ColsClassPhoto FROM crs_usr"
                     " WHERE CrsCod='%ld' AND UsrCod='%ld'",
               Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get number of columns in class photo");

      if (NumRows == 1)                // Should be one only row
        {
         /* Get number of columns in class photo */
         row = mysql_fetch_row (mysql_res);
         if (row[0])
            if (sscanf (row[0],"%u",&Gbl.Usrs.ClassPhoto.Cols) == 1)
               if (Gbl.Usrs.ClassPhoto.Cols < 1 ||
                   Gbl.Usrs.ClassPhoto.Cols > Usr_CLASS_PHOTO_COLS_MAX)
                  Gbl.Usrs.ClassPhoto.Cols = Usr_CLASS_PHOTO_COLS_DEF;
        }
      else if (NumRows > 1)        // Error in databse: more than one row for a user in course
         Lay_ShowErrorAndExit ("Error when getting number of columns in class photo.");

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/** Save my prefs. about number of colums in class photo for current course **/
/*****************************************************************************/

static void Usr_UpdateMyColsClassPhotoInDB (void)
  {
   char Query[256];

   if (Gbl.Usrs.Me.UsrDat.UsrCod > 0 && Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      /***** Update number of colums in class photo for current course *****/
      sprintf (Query,"UPDATE crs_usr SET ColsClassPhoto='%u'"
                     " WHERE CrsCod='%ld' AND UsrCod='%ld'",
               Gbl.Usrs.ClassPhoto.Cols,
               Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE (Query,"can not update number of columns in class photo");
     }
  }

/*****************************************************************************/
/********** Get and update preference about photos in users' list ************/
/*****************************************************************************/

static void Usr_GetAndUpdatePrefAboutListWithPhotos (void)
  {
   /***** Get he preference about photos in users' list from form *****/
   if (Usr_GetParamListWithPhotosFromForm ())
      /***** Save preference about photos in users' list into the database  *****/
      Usr_UpdateMyPrefAboutListWithPhotosPhotoInDB ();
   else
      /***** If parameter can't be retrieved from form, get my preference from database *****/
      Usr_GetMyPrefAboutListWithPhotosFromDB ();
  }

/*****************************************************************************/
/** Put a hidden parameter with the preference about photos in users' list ***/
/*****************************************************************************/

void Usr_PutParamListWithPhotos (void)
  {
   Par_PutHiddenParamChar ("WithPhotosExists",'Y');
   Par_PutHiddenParamChar ("WithPhotos",
                           Gbl.Usrs.Listing.WithPhotos ? 'Y' :
                        	                         'N');
  }

/*****************************************************************************/
/********* Get from form the preference about photos in users' list **********/
/*****************************************************************************/

static bool Usr_GetParamListWithPhotosFromForm (void)
  {
   char YN[1+1];

   /***** Get if exists parameter with preference about photos in users' list *****/
   Par_GetParToText ("WithPhotosExists",YN,1);
   if (Str_ConvertToUpperLetter (YN[0]) != 'Y')
     {
      Gbl.Usrs.Listing.WithPhotos = Usr_LIST_WITH_PHOTOS_DEF;
      return false;
     }

   /***** Parameter with preference about photos in users' list exists, so get it *****/
   Par_GetParToText ("WithPhotos",YN,1);
   Gbl.Usrs.Listing.WithPhotos = (Str_ConvertToUpperLetter (YN[0]) == 'Y');
   return true;
  }

/*****************************************************************************/
/***** Get my preference about photos in users' list for current course ******/
/*****************************************************************************/

void Usr_GetMyPrefAboutListWithPhotosFromDB (void)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   Gbl.Usrs.Listing.WithPhotos = Usr_LIST_WITH_PHOTOS_DEF;

   /***** If no user logged or not course selected... *****/
   if (Gbl.Usrs.Me.Logged && Gbl.CurrentCrs.Crs.CrsCod)
     {
      /***** Get if listing of users must show photos from database *****/
      sprintf (Query,"SELECT ListWithPhotos FROM crs_usr"
                     " WHERE CrsCod='%ld' AND UsrCod='%ld'",
               Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not check if listing of users must show photos");

      if (NumRows == 1)                // Should be one only row
        {
         /* Get number of columns in class photo */
         Gbl.Usrs.Listing.WithPhotos = Usr_LIST_WITH_PHOTOS_DEF;
         row = mysql_fetch_row (mysql_res);
         Gbl.Usrs.Listing.WithPhotos = (Str_ConvertToUpperLetter (row[0][0]) == 'Y');
        }
      else if (NumRows > 1)        // Error in databse: more than one row for a user in course
         Lay_ShowErrorAndExit ("Error when checking if listing of users must show photos.");

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/**** Save my preference about photos in users' list for current course ******/
/*****************************************************************************/

static void Usr_UpdateMyPrefAboutListWithPhotosPhotoInDB (void)
  {
   char Query[256];

   if (Gbl.Usrs.Me.UsrDat.UsrCod > 0 && Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      /***** Update number of colums in class photo for current course *****/
      sprintf (Query,"UPDATE crs_usr SET ListWithPhotos='%c'"
                     " WHERE CrsCod='%ld' AND UsrCod='%ld'",
               Gbl.Usrs.Listing.WithPhotos ? 'Y' :
        	                             'N',
               Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE (Query,"can not update your preference about photos in listing");
     }
  }

/*****************************************************************************/
/********** Put a link (form) to show list or class photo of guests **********/
/*****************************************************************************/

static void Usr_PutLinkToSeeAdmins (void)
  {
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   /***** Put form to list admins *****/
   Lay_PutContextualLink (ActLstOth,NULL,"adm16x16.gif",
                          Txt_ROLES_PLURAL_Abc[Rol_DEG_ADM][Usr_SEX_UNKNOWN],
                          Txt_ROLES_PLURAL_Abc[Rol_DEG_ADM][Usr_SEX_UNKNOWN]);
  }

/*****************************************************************************/
/********** Put a link (form) to show list or class photo of guests **********/
/*****************************************************************************/

static void Usr_PutLinkToSeeGuests (void)
  {
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   /***** Put form to list guests *****/
   Lay_PutContextualLink (ActLstGst,NULL,"usrs64x64.gif",
                          Txt_ROLES_PLURAL_Abc[Rol__GUEST_][Usr_SEX_UNKNOWN],
                          Txt_ROLES_PLURAL_Abc[Rol__GUEST_][Usr_SEX_UNKNOWN]);
  }

/*****************************************************************************/
/********************* Show list or class photo of guests ********************/
/*****************************************************************************/

void Usr_SeeGuests (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Scope;
   extern const char *Txt_Show_records;

   /***** Put contextual links *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

   /* Put link to remove old users */
   Usr_PutLinkToSeeAdmins ();

   /* Put link to go to admin one user */
   Enr_PutLinkToAdminOneUsr (ActReqMdfOneOth);

   if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
      /* Put link to remove old users */
      Enr_PutLinkToRemOldUsrs ();

   fprintf (Gbl.F.Out,"</div>");

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Sco_SetScopesForListingGuests ();
   Sco_GetScope ();

   /***** Get and order list of students in current scope *****/
   Usr_GetGstsLst (Gbl.Scope.Current);

   if (Usr_GetIfShowBigList (Gbl.Usrs.LstUsrs[Rol__GUEST_].NumUsrs))
     {
      /***** Get list of selected users *****/
      Usr_GetListsSelectedUsrs ();

      /***** Start frame *****/
      Lay_StartRoundFrame (NULL,Txt_ROLES_PLURAL_Abc[Rol__GUEST_][Usr_SEX_UNKNOWN],
			   (Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO) ? (Gbl.Usrs.LstUsrs[Rol__GUEST_].NumUsrs ? Usr_PutIconToPrintGsts :
				                                                                                NULL) :
								       Usr_PutIconToShowGstsAllData);

      /***** Form to select range of guests *****/
      switch (Gbl.Usrs.Me.LoggedRole)
	{
	 case Rol_CTR_ADM:
	 case Rol_INS_ADM:
	 case Rol_SYS_ADM:
	    fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
	    Act_FormStart (ActLstGst);
	    Usr_PutParamUsrListType (Gbl.Usrs.Me.ListType);
	    Usr_PutParamColsClassPhoto ();
	    Usr_PutParamListWithPhotos ();
	    fprintf (Gbl.F.Out,"<label class=\"%s\">%s: </label>",
		     The_ClassForm[Gbl.Prefs.Theme],Txt_Scope);
	    Sco_PutSelectorScope (true);
	    Act_FormEnd ();
	    fprintf (Gbl.F.Out,"</div>");
	    break;
	 default:
	    break;
	}

      if (Gbl.Usrs.LstUsrs[Rol__GUEST_].NumUsrs)
	{
	 /***** Form to select type of list of users *****/
	 Usr_ShowFormsToSelectUsrListType (ActLstGst);

         /***** Draw a class photo with students of the course *****/
         /* Start form */
	 Act_FormStart (ActSeeRecSevGst);
	 Grp_PutParamsCodGrps ();

         if (Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO)
	    Lay_WriteHeaderClassPhoto (false,true,
				       (Gbl.Scope.Current == Sco_SCOPE_CTR ||
					Gbl.Scope.Current == Sco_SCOPE_INS) ? Gbl.CurrentIns.Ins.InsCod :
					                                      -1L,
				       -1L,
				       -1L);

         /* Start table */
         fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">");

	 /* Put a row to select all users */
         Usr_PutCheckboxToSelectAllTheUsers (Rol__GUEST_);

         /* Draw the classphoto/list */
         switch (Gbl.Usrs.Me.ListType)
           {
            case Usr_CLASS_PHOTO:
               Usr_DrawClassPhoto (Usr_CLASS_PHOTO_SEL_SEE,
        	                   Rol__GUEST_);
               break;
            case Usr_LIST:
               Usr_ListMainDataGsts (true);
               break;
           }

         /* End table */
         fprintf (Gbl.F.Out,"</table>");

         /* Send button */
	 Lay_PutConfirmButton (Txt_Show_records);

         /* End form */
         Act_FormEnd ();
	}
      else
	 Usr_ShowWarningNoUsersFound (Rol__GUEST_);

      /***** End frame *****/
      Lay_EndRoundFrame ();
     }

   /***** Free memory for guests list *****/
   Usr_FreeUsrsList (Rol__GUEST_);
  }

/*****************************************************************************/
/******************** Show list or class photo of students *******************/
/*****************************************************************************/

void Usr_SeeStudents (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Scope;
   extern const char *Txt_Show_records;
   bool ICanViewRecords;

   /***** Put contextual links *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0 &&		// Course selected
       Gbl.Usrs.Me.LoggedRole >= Rol_TEACHER)	// I am logged as teacher or admin
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

      /* Put link to go to admin one user */
      Enr_PutLinkToAdminOneUsr (ActReqMdfOneStd);

      /* Put link to go to admin several users */
      Enr_PutLinkToAdminSeveralUsrs (Rol_STUDENT);

      /* Put link to list official students */
      Usr_PutLinkToListOfficialStudents ();

      /* Put link to edit record fields */
      Rec_PutLinkToEditRecordFields ();

      fprintf (Gbl.F.Out,"</div>");
     }
   else if (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT)
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

      /* Put link to go to admin one user */
      Enr_PutLinkToAdminOneUsr (ActReqMdfOneStd);

      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Sco_SetScopesForListingStudents ();
   Sco_GetScope ();
   ICanViewRecords = (Gbl.Scope.Current == Sco_SCOPE_CRS &&
	              (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT ||
                       Gbl.Usrs.Me.LoggedRole == Rol_TEACHER ||
                       Gbl.Usrs.Me.LoggedRole == Rol_DEG_ADM ||
	               Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM));

   /***** Form to select groups *****/
   if (Gbl.Scope.Current == Sco_SCOPE_CRS)
      Grp_ShowFormToSelectSeveralGroups (ActLstStd);

   /***** Get and order list of students *****/
   Usr_GetListUsrs (Rol_STUDENT,Gbl.Scope.Current);

   if (Usr_GetIfShowBigList (Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs))
     {
      /***** Get list of selected users *****/
      Usr_GetListsSelectedUsrs ();

      /***** Start frame *****/
      Lay_StartRoundFrame (NULL,Txt_ROLES_PLURAL_Abc[Rol_STUDENT][Usr_SEX_UNKNOWN],
			   (Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO) ? (Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs ? Usr_PutIconToPrintStds :
												   NULL) :
			    ((Gbl.Usrs.Me.LoggedRole >= Rol_TEACHER) ? Usr_PutIconToShowStdsAllData :
								       NULL));

      /***** Form to select range of students *****/
      switch (Gbl.Usrs.Me.LoggedRole)
	{
	 case Rol_DEG_ADM:
	 case Rol_SYS_ADM:
	    fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
	    Act_FormStart (ActLstStd);
	    Usr_PutParamUsrListType (Gbl.Usrs.Me.ListType);
	    Usr_PutParamColsClassPhoto ();
	    Usr_PutParamListWithPhotos ();
	    fprintf (Gbl.F.Out,"<label class=\"%s\">%s: </label>",
		     The_ClassForm[Gbl.Prefs.Theme],Txt_Scope);
	    Sco_PutSelectorScope (true);
	    Act_FormEnd ();
	    fprintf (Gbl.F.Out,"</div>");
	    break;
	 default:
	    break;
	}

      if (Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs)
	{
	 /***** Form to select type of list of users *****/
	 Usr_ShowFormsToSelectUsrListType (ActLstStd);

         /***** Draw a class photo with students of the course *****/
         /* Start form */
         if (ICanViewRecords)
           {
	    Act_FormStart (ActSeeRecSevStd);
	    Grp_PutParamsCodGrps ();
           }

         if (Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO)
	    Lay_WriteHeaderClassPhoto (false,true,
				       (Gbl.Scope.Current == Sco_SCOPE_CRS ||
					Gbl.Scope.Current == Sco_SCOPE_DEG ||
					Gbl.Scope.Current == Sco_SCOPE_CTR ||
					Gbl.Scope.Current == Sco_SCOPE_INS) ? Gbl.CurrentIns.Ins.InsCod :
					                                      -1L,
				       (Gbl.Scope.Current == Sco_SCOPE_CRS ||
					Gbl.Scope.Current == Sco_SCOPE_DEG) ? Gbl.CurrentDeg.Deg.DegCod :
					                                      -1L,
					Gbl.Scope.Current == Sco_SCOPE_CRS  ? Gbl.CurrentCrs.Crs.CrsCod :
					                                      -1L);

         /* Start table */
         fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">");

	 /* Put a row to select all users */
         if (ICanViewRecords)
            Usr_PutCheckboxToSelectAllTheUsers (Rol_STUDENT);

         /* Draw the classphoto/list */
         switch (Gbl.Usrs.Me.ListType)
           {
            case Usr_CLASS_PHOTO:
               Usr_DrawClassPhoto (ICanViewRecords ? Usr_CLASS_PHOTO_SEL_SEE :
        	                                     Usr_CLASS_PHOTO_SEE,
        	                   Rol_STUDENT);
               break;
            case Usr_LIST:
               Usr_ListMainDataStds (ICanViewRecords);
               break;
           }

         /* End table */
         fprintf (Gbl.F.Out,"</table>");

         if (ICanViewRecords)
           {
            /* Send button */
	    Lay_PutConfirmButton (Txt_Show_records);

	    /* End form */
	    Act_FormEnd ();
           }
	}
      else
	{
	 /***** Show warning indicating no students found *****/
	 Usr_ShowWarningNoUsersFound (Rol_STUDENT);

	 /***** Form to enroll several students *****/
	 if (Gbl.CurrentCrs.Crs.CrsCod > 0 &&				// Course selected
	     Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB == Rol_TEACHER)		// I am a teacher in current course
	    Enr_PutButtonToEnrollStudents ();
	}

      /***** End frame *****/
      Lay_EndRoundFrame ();
     }

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (Rol_STUDENT);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/******************** Show list or class photo of teachers *******************/
/*****************************************************************************/

void Usr_SeeTeachers (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Scope;
   extern const char *Txt_Show_records;
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   bool ICanViewRecords;

   /***** Put contextual links *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0 &&			// Course selected
       Gbl.Usrs.Me.LoggedRole >= Rol_TEACHER)		// ...or I am logged as teacher or admin
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

      /* Put link to go to admin one user */
      Enr_PutLinkToAdminOneUsr (ActReqMdfOneTch);

      /* Put link to go to admin several users */
      if (Gbl.Usrs.Me.LoggedRole >= Rol_DEG_ADM)	// I am logged as admin
         Enr_PutLinkToAdminSeveralUsrs (Rol_TEACHER);

      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	               1 << Sco_SCOPE_CTY |
                       1 << Sco_SCOPE_INS |
                       1 << Sco_SCOPE_CTR |
                       1 << Sco_SCOPE_DEG |
                       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   Sco_GetScope ();
   ICanViewRecords = (Gbl.Scope.Current == Sco_SCOPE_CRS);

   /***** Get and order list of teachers *****/
   Usr_GetListUsrs (Rol_TEACHER,Gbl.Scope.Current);

   if (Usr_GetIfShowBigList (Gbl.Usrs.LstUsrs[Rol_TEACHER].NumUsrs))
     {
      /***** Start frame *****/
      Lay_StartRoundFrame (NULL,Txt_ROLES_PLURAL_Abc[Rol_TEACHER][Usr_SEX_UNKNOWN],
			   (Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO) ? (Gbl.Usrs.LstUsrs[Rol_TEACHER].NumUsrs ? Usr_PutIconToPrintTchs :
				                                                                                NULL) :
			    ((Gbl.Usrs.Me.LoggedRole >= Rol_DEG_ADM) ? Usr_PutIconToShowTchsAllData :
								       NULL));

      /***** Form to select scope *****/
      fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActLstTch);
      Usr_PutParamUsrListType (Gbl.Usrs.Me.ListType);
      Usr_PutParamColsClassPhoto ();
      Usr_PutParamListWithPhotos ();
      fprintf (Gbl.F.Out,"<label class=\"%s\">%s: </label>",
	       The_ClassForm[Gbl.Prefs.Theme],Txt_Scope);
      Sco_PutSelectorScope (true);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");

      if (Gbl.Usrs.LstUsrs[Rol_TEACHER].NumUsrs)
	{
	 /***** Form to select type of list of users *****/
	 Usr_ShowFormsToSelectUsrListType (ActLstTch);

         /***** Draw a class photo with teachers of the course *****/
         /* Start form */
         if (ICanViewRecords)
            Act_FormStart (ActSeeRecSevTch);

         if (Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO)
	    Lay_WriteHeaderClassPhoto (false,true,
				       (Gbl.Scope.Current == Sco_SCOPE_CRS ||
					Gbl.Scope.Current == Sco_SCOPE_DEG ||
					Gbl.Scope.Current == Sco_SCOPE_CTR ||
					Gbl.Scope.Current == Sco_SCOPE_INS) ? Gbl.CurrentIns.Ins.InsCod :
					                                      -1L,
				       (Gbl.Scope.Current == Sco_SCOPE_CRS ||
					Gbl.Scope.Current == Sco_SCOPE_DEG) ? Gbl.CurrentDeg.Deg.DegCod :
					                                      -1L,
					Gbl.Scope.Current == Sco_SCOPE_CRS  ? Gbl.CurrentCrs.Crs.CrsCod :
					                                      -1L);

         /* Start table */
         fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">");

	 /* Put a row to select all users */
         if (ICanViewRecords)
	    Usr_PutCheckboxToSelectAllTheUsers (Rol_TEACHER);

         /***** Draw the classphoto/list  *****/
         switch (Gbl.Usrs.Me.ListType)
           {
            case Usr_CLASS_PHOTO:
               Usr_DrawClassPhoto (ICanViewRecords ? Usr_CLASS_PHOTO_SEL_SEE :
        	                                     Usr_CLASS_PHOTO_SEE,
        	                   Rol_TEACHER);
               break;
            case Usr_LIST:
               Usr_ListMainDataTchs (ICanViewRecords);
               break;
           }

         /* End table */
         fprintf (Gbl.F.Out,"</table>");

         if (ICanViewRecords)
           {
            /* Send button */
	    Lay_PutConfirmButton (Txt_Show_records);

	    /* End form */
	    Act_FormEnd ();
           }
	}
      else
	 Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_TEACHER]);

      /***** End frame *****/
      Lay_EndRoundFrame ();
     }

   /***** Free memory for teachers list *****/
   Usr_FreeUsrsList (Rol_TEACHER);
  }

/*****************************************************************************/
/***************** Functions used to print lists of users ********************/
/*****************************************************************************/

static void Usr_PutIconToPrintGsts (void)
  {
   extern const char *Txt_Print;

   Lay_PutContextualLink (ActPrnGstPho,Usr_ShowGstsAllDataParams,
			  "print64x64.png",Txt_Print,NULL);
  }

static void Usr_PutIconToPrintStds (void)
  {
   extern const char *Txt_Print;

   Lay_PutContextualLink (ActPrnStdPho,Usr_ShowStdsAllDataParams,
			  "print64x64.png",Txt_Print,NULL);
  }

static void Usr_PutIconToPrintTchs (void)
  {
   extern const char *Txt_Print;

   Lay_PutContextualLink (ActPrnTchPho,Usr_ShowTchsAllDataParams,
			  "print64x64.png",Txt_Print,NULL);
  }

static void Usr_PutIconToShowGstsAllData (void)
  {
   extern const char *Txt_Show_all_data;

   Lay_PutContextualLink (ActLstGstAll,Usr_ShowGstsAllDataParams,
			  "table64x64.gif",Txt_Show_all_data,NULL);
  }

static void Usr_PutIconToShowStdsAllData (void)
  {
   extern const char *Txt_Show_all_data;

   Lay_PutContextualLink (ActLstStdAll,Usr_ShowStdsAllDataParams,
			  "table64x64.gif",Txt_Show_all_data,NULL);
  }

static void Usr_PutIconToShowTchsAllData (void)
  {
   extern const char *Txt_Show_all_data;

   Lay_PutContextualLink (ActLstTchAll,Usr_ShowTchsAllDataParams,
			  "table64x64.gif",Txt_Show_all_data,NULL);
  }

static void Usr_ShowGstsAllDataParams (void)
  {
   Usr_PutParamListWithPhotos ();
   Usr_PutExtraParamsUsrList (ActLstGstAll);
  }

static void Usr_ShowStdsAllDataParams (void)
  {
   Grp_PutParamsCodGrps ();
   Usr_PutParamListWithPhotos ();
   Usr_PutExtraParamsUsrList (ActLstStdAll);
  }

static void Usr_ShowTchsAllDataParams (void)
  {
   Sco_PutParamScope (Gbl.Scope.Current);
   Usr_PutParamListWithPhotos ();
   Usr_PutExtraParamsUsrList (ActLstTchAll);
  }

/*****************************************************************************/
/******************* Put a link to list official students ********************/
/*****************************************************************************/

static void Usr_PutLinkToListOfficialStudents (void)
  {
   extern const char *Txt_Official_students;
   bool ExternalUsrsServiceAvailable = (Cfg_EXTERNAL_LOGIN_CLIENT_COMMAND[0] != '\0');

   if (ExternalUsrsServiceAvailable &&				// There is an external service for authentication and official lists
       Gbl.Imported.ExternalUsrId[0] &&			// I was authenticated from external service...
       Gbl.Imported.ExternalSesId[0] &&
       Gbl.Imported.ExternalRole == Rol_TEACHER)	// ...as a teacher
      /***** Link to list official students *****/
      Lay_PutContextualLink (ActGetExtLstStd,NULL,"list64x64.gif",
                             Txt_Official_students,Txt_Official_students);
  }

/*****************************************************************************/
/************* Draw class photo with guests ready to be printed **************/
/*****************************************************************************/

void Usr_SeeGstClassPhotoPrn (void)
  {
   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Sco_SetScopesForListingGuests ();
   Sco_GetScope ();

   /***** Get and order list of students *****/
   Usr_GetGstsLst (Gbl.Scope.Current);

   if (Gbl.Usrs.LstUsrs[Rol__GUEST_].NumUsrs)
     {
      /***** Draw the guests' class photo *****/
      Lay_StartRoundFrame (NULL,NULL,NULL);
      Lay_WriteHeaderClassPhoto (true,true,
				 (Gbl.Scope.Current == Sco_SCOPE_CTR ||
				  Gbl.Scope.Current == Sco_SCOPE_INS) ? Gbl.CurrentIns.Ins.InsCod :
                                                                        -1L,
				 -1L,-1L);
      fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">");
      Usr_DrawClassPhoto (Usr_CLASS_PHOTO_PRN,Rol__GUEST_);
      fprintf (Gbl.F.Out,"</table>");
      Lay_EndRoundFrame ();
     }
   else
      Usr_ShowWarningNoUsersFound (Rol__GUEST_);

   /***** Free memory for guests list *****/
   Usr_FreeUsrsList (Rol__GUEST_);
  }

/*****************************************************************************/
/**************** Draw class photo with students ready to print **************/
/*****************************************************************************/

void Usr_SeeStdClassPhotoPrn (void)
  {
   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Sco_SetScopesForListingStudents ();
   Sco_GetScope ();

   /****** Get groups to show ******/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get and order list of students *****/
   Usr_GetListUsrs (Rol_STUDENT,Gbl.Scope.Current);

   if (Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs)
     {
      /***** Draw the students' class photo *****/
      Lay_StartRoundFrame (NULL,NULL,NULL);
      Lay_WriteHeaderClassPhoto (true,true,
				 (Gbl.Scope.Current == Sco_SCOPE_CRS ||
				  Gbl.Scope.Current == Sco_SCOPE_DEG ||
				  Gbl.Scope.Current == Sco_SCOPE_CTR ||
				  Gbl.Scope.Current == Sco_SCOPE_INS) ? Gbl.CurrentIns.Ins.InsCod :
					                                -1L,
				 (Gbl.Scope.Current == Sco_SCOPE_CRS ||
				  Gbl.Scope.Current == Sco_SCOPE_DEG) ? Gbl.CurrentDeg.Deg.DegCod :
					                                -1L,
				  Gbl.Scope.Current == Sco_SCOPE_CRS  ? Gbl.CurrentCrs.Crs.CrsCod :
					                                -1L);
      fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">");
      Usr_DrawClassPhoto (Usr_CLASS_PHOTO_PRN,Rol_STUDENT);
      fprintf (Gbl.F.Out,"</table>");
      Lay_EndRoundFrame ();
     }
   else
      Usr_ShowWarningNoUsersFound (Rol_STUDENT);

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (Rol_STUDENT);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/**************** Draw class photo with teachers ready to print **************/
/*****************************************************************************/

void Usr_SeeTchClassPhotoPrn (void)
  {
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	               1 << Sco_SCOPE_CTY |
                       1 << Sco_SCOPE_INS |
                       1 << Sco_SCOPE_CTR |
                       1 << Sco_SCOPE_DEG |
                       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   Sco_GetScope ();

   /***** Get and order list of teachers *****/
   Usr_GetListUsrs (Rol_TEACHER,Gbl.Scope.Current);

   if (Gbl.Usrs.LstUsrs[Rol_TEACHER].NumUsrs)
     {
      /***** Draw the teachers' class photo *****/
      Lay_StartRoundFrame (NULL,NULL,NULL);
      Lay_WriteHeaderClassPhoto (true,true,
				 (Gbl.Scope.Current == Sco_SCOPE_CRS ||
				  Gbl.Scope.Current == Sco_SCOPE_DEG ||
				  Gbl.Scope.Current == Sco_SCOPE_CTR ||
				  Gbl.Scope.Current == Sco_SCOPE_INS) ? Gbl.CurrentIns.Ins.InsCod :
					                                -1L,
				 (Gbl.Scope.Current == Sco_SCOPE_CRS ||
				  Gbl.Scope.Current == Sco_SCOPE_DEG) ? Gbl.CurrentDeg.Deg.DegCod :
					                                -1L,
				  Gbl.Scope.Current == Sco_SCOPE_CRS  ? Gbl.CurrentCrs.Crs.CrsCod :
					                                -1L);
      fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">");
      Usr_DrawClassPhoto (Usr_CLASS_PHOTO_PRN,Rol_TEACHER);
      fprintf (Gbl.F.Out,"</table>");
      Lay_EndRoundFrame ();
     }
   else
      Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_TEACHER]);

   /***** Free memory for teachers list *****/
   Usr_FreeUsrsList (Rol_TEACHER);
  }

/*****************************************************************************/
/****************** Draw class photo with students or teachers ***************/
/*****************************************************************************/

static void Usr_DrawClassPhoto (Usr_ClassPhotoType_t ClassPhotoType,
                                Rol_Role_t RoleInClassPhoto)
  {
   unsigned NumUsr;
   bool TRIsOpen = false;
   bool PutCheckBoxToSelectUsr = (ClassPhotoType == Usr_CLASS_PHOTO_SEL ||
	                          ClassPhotoType == Usr_CLASS_PHOTO_SEL_SEE);
   bool PutOriginPlace = (RoleInClassPhoto == Rol_STUDENT &&
                          (ClassPhotoType == Usr_CLASS_PHOTO_SEL_SEE ||
                           ClassPhotoType == Usr_CLASS_PHOTO_SEE ||
                           ClassPhotoType == Usr_CLASS_PHOTO_PRN));
   bool ItsMe;
   bool ShowPhoto;
   bool ShowData;
   bool UsrIsTheMsgSender;
   const char *ClassPhoto = "PHOTO21x28";	// Default photo size
   int LengthUsrData = 10;	// Maximum number of characters of user data
   char PhotoURL[PATH_MAX+1];
   struct UsrData UsrDat;

   /***** Set width and height of photos *****/
   switch (ClassPhotoType)
     {
      case Usr_CLASS_PHOTO_SEL:
	 ClassPhoto = "PHOTO21x28";
         LengthUsrData = 10;
         break;
      case Usr_CLASS_PHOTO_SEL_SEE:
      case Usr_CLASS_PHOTO_SEE:
	 ClassPhoto = "PHOTO45x60";
         LengthUsrData = 10;
         break;
      case Usr_CLASS_PHOTO_PRN:
	 ClassPhoto = "PHOTO45x60";
         LengthUsrData = 15;
         break;
     }

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Loop for showing users photos, names and place of birth *****/
   for (NumUsr = 0;
        NumUsr < Gbl.Usrs.LstUsrs[RoleInClassPhoto].NumUsrs; )
     {
      if ((NumUsr % Gbl.Usrs.ClassPhoto.Cols) == 0)
        {
         fprintf (Gbl.F.Out,"<tr>");
         TRIsOpen = true;
        }

      UsrDat.UsrCod = Gbl.Usrs.LstUsrs[RoleInClassPhoto].Lst[NumUsr].UsrCod;
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
        {
         UsrDat.Accepted = Gbl.Usrs.LstUsrs[RoleInClassPhoto].Lst[NumUsr].Accepted;

         ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == UsrDat.UsrCod);
         ShowData = (ItsMe || UsrDat.Accepted ||
                     Gbl.Usrs.Me.LoggedRole == Rol_DEG_ADM ||
                     Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);

         /***** Begin user's cell *****/
         fprintf (Gbl.F.Out,"<td class=\"CLASSPHOTO CENTER_BOTTOM");
         if (ClassPhotoType == Usr_CLASS_PHOTO_SEL &&
             UsrDat.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod)
           {
            UsrIsTheMsgSender = true;
            fprintf (Gbl.F.Out," LIGHT_GREEN");
           }
         else
            UsrIsTheMsgSender = false;
         fprintf (Gbl.F.Out,"\">");

         /***** Checkbox to select this user *****/
         if (PutCheckBoxToSelectUsr)
            Usr_PutCheckboxToSelectUser (&UsrDat,UsrIsTheMsgSender);

         /***** Show photo *****/
         ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
         Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                        	               NULL,
                           ClassPhoto,Pho_ZOOM,false);

         /***** Photo foot *****/
         fprintf (Gbl.F.Out,"<br />");

         /* Name */
         if (UsrDat.FirstName[0])
           {
            Str_LimitLengthHTMLStr (UsrDat.FirstName,LengthUsrData);
            fprintf (Gbl.F.Out,"%s",UsrDat.FirstName);
           }
         else
            fprintf (Gbl.F.Out,"&nbsp;");
         fprintf (Gbl.F.Out,"<br />");
         if (UsrDat.Surname1[0])
           {
            Str_LimitLengthHTMLStr (UsrDat.Surname1,LengthUsrData);
            fprintf (Gbl.F.Out,"%s",UsrDat.Surname1);
           }
         else
            fprintf (Gbl.F.Out,"&nbsp;");
         fprintf (Gbl.F.Out,"<br />");
         if (UsrDat.Surname2[0])
           {
            Str_LimitLengthHTMLStr (UsrDat.Surname2,LengthUsrData);
            fprintf (Gbl.F.Out,"%s",UsrDat.Surname2);
           }
         else
            fprintf (Gbl.F.Out,"&nbsp;");

         /* Origin place */
         if (PutOriginPlace)
           {
            fprintf (Gbl.F.Out,"<br />");
            if (UsrDat.OriginPlace[0])
              {
               fprintf (Gbl.F.Out,"<em>");
               if (ShowData)
                 {
                  Str_LimitLengthHTMLStr (UsrDat.OriginPlace,LengthUsrData);
                  fprintf (Gbl.F.Out,"%s",UsrDat.OriginPlace);
                 }
               else
                  fprintf (Gbl.F.Out,"-");
               fprintf (Gbl.F.Out,"</em>");
              }
            else // There's no origin place
               fprintf (Gbl.F.Out,"&nbsp;");
           }

         /***** End of user's cell *****/
         fprintf (Gbl.F.Out,"</td>");

         if ((++NumUsr % Gbl.Usrs.ClassPhoto.Cols) == 0)
           {
            fprintf (Gbl.F.Out,"</tr>");
            TRIsOpen = false;
           }
        }
     }
   if (TRIsOpen)
      fprintf (Gbl.F.Out,"</tr>");

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/***************** Write selector of columns in class photo ******************/
/*****************************************************************************/

void Usr_PutSelectorNumColsClassPhoto (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_columns;
   unsigned Cols;

   /***** Start selector *****/
   fprintf (Gbl.F.Out,"<select name=\"ColsClassPhoto\""
                      " onchange=\"document.getElementById('%s').submit();\">",
            Gbl.Form.Id);

   /***** Put a row in selector for every number from 1 to Usr_CLASS_PHOTO_COLS_MAX *****/
   for (Cols = 1;
        Cols <= Usr_CLASS_PHOTO_COLS_MAX;
        Cols++)
     {
      fprintf (Gbl.F.Out,"<option");
      if (Cols == Gbl.Usrs.ClassPhoto.Cols)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%u</option>",Cols);
     }

   /***** End selector *****/
   fprintf (Gbl.F.Out,"</select>"
	              "<span class=\"%s\">%s</span>",
            The_ClassForm[Gbl.Prefs.Theme],Txt_columns);
  }

/*****************************************************************************/
/********** Build the relative path of a user from his user's code ***********/
/*****************************************************************************/

void Usr_ConstructPathUsr (long UsrCod,char *PathUsr)
  {
   char PathUsrs[PATH_MAX+1];
   char PathAboveUsr[PATH_MAX+1];

   /***** Path for users *****/
   sprintf (PathUsrs,"%s/%s",Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_USR);
   Fil_CreateDirIfNotExists (PathUsrs);

   /***** Path above user's ID *****/
   sprintf (PathAboveUsr,"%s/%02u",PathUsrs,(unsigned) (UsrCod % 100));
   Fil_CreateDirIfNotExists (PathAboveUsr);

   /***** Path for user *****/
   sprintf (PathUsr,"%s/%ld",PathAboveUsr,UsrCod);
  }

/*****************************************************************************/
/************** Check if a user exists with a given user's code **************/
/*****************************************************************************/

bool Usr_ChkIfUsrCodExists (long UsrCod)
  {
   char Query[256];

   if (UsrCod <= 0)	// Wrong user's code
      return false;

   /***** Get if a user exists in database *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_data"
	          " WHERE UsrCod='%ld'",
	    UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if a user exists") != 0);
  }

/*****************************************************************************/
/********************** Show warning "no users found" ************************/
/*****************************************************************************/
// Use Rol_ROLE_UNKNOWN type to display "no users found"

void Usr_ShowWarningNoUsersFound (Rol_Role_t Role)
  {
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];

   Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Role]);
  }

/*****************************************************************************/
/************************ See stats about the platform ***********************/
/*****************************************************************************/
// Here Rol_ROLE_UNKNOWN means "all users"

void Usr_GetAndShowNumUsrsInPlatform (Rol_Role_t Role)
  {
   extern const char *Txt_Total;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   unsigned NumUsrs;
   float NumCrssPerUsr;
   float NumUsrsPerCrs;
   char *Class = (Role == Rol_UNKNOWN) ? "DAT_N_LINE_TOP RIGHT_BOTTOM" :
	                                 "DAT RIGHT_BOTTOM";

   /***** Get the number of users belonging to any course *****/
   if (Role == Rol__GUEST_)	// Users not beloging to any course
      NumUsrs = Usr_GetNumUsrsNotBelongingToAnyCrs ();
   else
      NumUsrs = Sta_GetTotalNumberOfUsersInCourses (Gbl.Scope.Current,Role);

   /***** Get average number of courses per user *****/
   NumCrssPerUsr = (Role == Rol__GUEST_) ? 0 :
	                                   Usr_GetNumCrssPerUsr (Role);

   /***** Query the number of users per course *****/
   NumUsrsPerCrs = (Role == Rol__GUEST_) ? 0 :
	                                   Usr_GetNumUsrsPerCrs (Role);

   /***** Write the total number of users *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s\">"
                      "%s"
                      "</td>"
                      "<td class=\"%s\">"
                      "%u"
                      "</td>"
                      "<td class=\"%s\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"%s\">"
                      "%.2f"
                      "</td>"
                      "</tr>",
            Class,(Role == Rol_UNKNOWN) ? Txt_Total :
        	                          Txt_ROLES_PLURAL_Abc[Role][Usr_SEX_UNKNOWN],
            Class,NumUsrs,
            Class,NumCrssPerUsr,
            Class,NumUsrsPerCrs);
  }

/*****************************************************************************/
/******** Get total number of users who do not belong to any course **********/
/*****************************************************************************/

static unsigned Usr_GetNumUsrsNotBelongingToAnyCrs (void)
  {
   char Query[256];

   /***** Get number of users who are in table of users but not in table courses-users *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_data"
                  " WHERE UsrCod NOT IN"
                  " (SELECT DISTINCT(UsrCod) FROM crs_usr)");
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of users who do not belong to any course");
  }

/*****************************************************************************/
/************ Get average number of courses with users of a role *************/
/*****************************************************************************/

static float Usr_GetNumCrssPerUsr (Rol_Role_t Role)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   float NumCrssPerUsr;

   /***** Get number of courses per user from database *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 if (Role == Rol_UNKNOWN)	// Any user
	    sprintf (Query,"SELECT AVG(NumCrss) FROM "
			   "(SELECT COUNT(CrsCod) AS NumCrss"
			   " FROM crs_usr"
			   " GROUP BY UsrCod) AS NumCrssTable");
	 else
	    sprintf (Query,"SELECT AVG(NumCrss) FROM "
			   "(SELECT COUNT(CrsCod) AS NumCrss"
			   " FROM crs_usr"
			   " WHERE Role='%u' GROUP BY UsrCod) AS NumCrssTable",
		     (unsigned) Role);
         break;
      case Sco_SCOPE_CTY:
	 if (Role == Rol_UNKNOWN)	// Any user
	    sprintf (Query,"SELECT AVG(NumCrss) FROM "
			   "(SELECT COUNT(crs_usr.CrsCod) AS NumCrss"
			   " FROM institutions,centres,degrees,courses,crs_usr"
			   " WHERE institutions.CtyCod='%ld'"
			   " AND institutions.InsCod=centres.InsCod"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " GROUP BY crs_usr.UsrCod) AS NumCrssTable",
		     Gbl.CurrentCty.Cty.CtyCod);
	 else
	    sprintf (Query,"SELECT AVG(NumCrss) FROM "
			   "(SELECT COUNT(crs_usr.CrsCod) AS NumCrss"
			   " FROM institutions,centres,degrees,courses,crs_usr"
			   " WHERE institutions.CtyCod='%ld'"
			   " AND institutions.InsCod=centres.InsCod"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.Role='%u'"
			   " GROUP BY crs_usr.UsrCod) AS NumCrssTable",
		     Gbl.CurrentCty.Cty.CtyCod,
		     (unsigned) Role);
         break;
      case Sco_SCOPE_INS:
	 if (Role == Rol_UNKNOWN)	// Any user
	    sprintf (Query,"SELECT AVG(NumCrss) FROM "
			   "(SELECT COUNT(crs_usr.CrsCod) AS NumCrss"
			   " FROM centres,degrees,courses,crs_usr"
			   " WHERE centres.InsCod='%ld'"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " GROUP BY crs_usr.UsrCod) AS NumCrssTable",
		     Gbl.CurrentIns.Ins.InsCod);
	 else
	    sprintf (Query,"SELECT AVG(NumCrss) FROM "
			   "(SELECT COUNT(crs_usr.CrsCod) AS NumCrss"
			   " FROM centres,degrees,courses,crs_usr"
			   " WHERE centres.InsCod='%ld'"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.Role='%u'"
			   " GROUP BY crs_usr.UsrCod) AS NumCrssTable",
		     Gbl.CurrentIns.Ins.InsCod,
		     (unsigned) Role);
         break;
      case Sco_SCOPE_CTR:
	 if (Role == Rol_UNKNOWN)	// Any user
	    sprintf (Query,"SELECT AVG(NumCrss) FROM "
			   "(SELECT COUNT(crs_usr.CrsCod) AS NumCrss"
			   " FROM degrees,courses,crs_usr"
			   " WHERE degrees.CtrCod='%ld'"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " GROUP BY crs_usr.UsrCod) AS NumCrssTable",
		     Gbl.CurrentCtr.Ctr.CtrCod);
	 else
	    sprintf (Query,"SELECT AVG(NumCrss) FROM "
			   "(SELECT COUNT(crs_usr.CrsCod) AS NumCrss"
			   " FROM degrees,courses,crs_usr"
			   " WHERE degrees.CtrCod='%ld'"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.Role='%u'"
			   " GROUP BY crs_usr.UsrCod) AS NumCrssTable",
		     Gbl.CurrentCtr.Ctr.CtrCod,
		     (unsigned) Role);
         break;
      case Sco_SCOPE_DEG:
	 if (Role == Rol_UNKNOWN)	// Any user
	    sprintf (Query,"SELECT AVG(NumCrss) FROM "
			   "(SELECT COUNT(crs_usr.CrsCod) AS NumCrss"
			   " FROM courses,crs_usr"
			   " WHERE courses.DegCod='%ld'"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " GROUP BY crs_usr.UsrCod) AS NumCrssTable",
		     Gbl.CurrentDeg.Deg.DegCod);
	 else
	    sprintf (Query,"SELECT AVG(NumCrss) FROM "
			   "(SELECT COUNT(crs_usr.CrsCod) AS NumCrss"
			   " FROM courses,crs_usr"
			   " WHERE courses.DegCod='%ld'"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.Role='%u'"
			   " GROUP BY crs_usr.UsrCod) AS NumCrssTable",
		     Gbl.CurrentDeg.Deg.DegCod,
		     (unsigned) Role);
         break;
      case Sco_SCOPE_CRS:
         return 1.0;
      default:
         Lay_ShowErrorAndExit ("Wrong scope.");
         break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of courses per user");

   /***** Get number of courses *****/
   row = mysql_fetch_row (mysql_res);
   NumCrssPerUsr = Str_GetFloatNumFromStr (row[0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCrssPerUsr;
  }

/*****************************************************************************/
/************ Get average number of courses with users of a type *************/
/*****************************************************************************/

static float Usr_GetNumUsrsPerCrs (Rol_Role_t Role)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   float NumUsrsPerCrs;

   /***** Get number of users per course from database *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 if (Role == Rol_UNKNOWN)	// Any user
	    sprintf (Query,"SELECT AVG(NumUsrs) FROM "
			   "(SELECT COUNT(UsrCod) AS NumUsrs"
			   " FROM crs_usr"
			   " GROUP BY CrsCod) AS NumUsrsTable");
	 else
	    sprintf (Query,"SELECT AVG(NumUsrs) FROM "
			   "(SELECT COUNT(UsrCod) AS NumUsrs"
			   " FROM crs_usr"
			   " WHERE Role='%u' GROUP BY CrsCod) AS NumUsrsTable",
		     (unsigned) Role);
         break;
      case Sco_SCOPE_CTY:
	 if (Role == Rol_UNKNOWN)	// Any user
	    sprintf (Query,"SELECT AVG(NumUsrs) FROM "
			   "(SELECT COUNT(crs_usr.UsrCod) AS NumUsrs"
			   " FROM institutions,centres,degrees,courses,crs_usr"
			   " WHERE institutions.CtyCod='%ld'"
			   " AND institutions.InsCod=centres.InsCod"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " GROUP BY crs_usr.CrsCod) AS NumUsrsTable",
		     Gbl.CurrentCty.Cty.CtyCod);
	 else
	    sprintf (Query,"SELECT AVG(NumUsrs) FROM "
			   "(SELECT COUNT(crs_usr.UsrCod) AS NumUsrs"
			   " FROM institutions,centres,degrees,courses,crs_usr"
			   " WHERE institutions.CtyCod='%ld'"
			   " AND institutions.InsCod=centres.InsCod"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.Role='%u'"
			   " GROUP BY crs_usr.CrsCod) AS NumUsrsTable",
		     Gbl.CurrentCty.Cty.CtyCod,
		     (unsigned) Role);
         break;
      case Sco_SCOPE_INS:
	 if (Role == Rol_UNKNOWN)	// Any user
	    sprintf (Query,"SELECT AVG(NumUsrs) FROM "
			   "(SELECT COUNT(crs_usr.UsrCod) AS NumUsrs"
			   " FROM centres,degrees,courses,crs_usr"
			   " WHERE centres.InsCod='%ld'"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " GROUP BY crs_usr.CrsCod) AS NumUsrsTable",
		     Gbl.CurrentIns.Ins.InsCod);
	 else
	    sprintf (Query,"SELECT AVG(NumUsrs) FROM "
			   "(SELECT COUNT(crs_usr.UsrCod) AS NumUsrs"
			   " FROM centres,degrees,courses,crs_usr"
			   " WHERE centres.InsCod='%ld'"
			   " AND centres.CtrCod=degrees.CtrCod"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.Role='%u'"
			   " GROUP BY crs_usr.CrsCod) AS NumUsrsTable",
		     Gbl.CurrentIns.Ins.InsCod,
		     (unsigned) Role);
         break;
      case Sco_SCOPE_CTR:
	 if (Role == Rol_UNKNOWN)	// Any user
	    sprintf (Query,"SELECT AVG(NumUsrs) FROM "
			   "(SELECT COUNT(crs_usr.UsrCod) AS NumUsrs"
			   " FROM degrees,courses,crs_usr"
			   " WHERE degrees.CtrCod='%ld'"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " GROUP BY crs_usr.CrsCod) AS NumUsrsTable",
		     Gbl.CurrentCtr.Ctr.CtrCod);
	 else
	    sprintf (Query,"SELECT AVG(NumUsrs) FROM "
			   "(SELECT COUNT(crs_usr.UsrCod) AS NumUsrs"
			   " FROM degrees,courses,crs_usr"
			   " WHERE degrees.CtrCod='%ld'"
			   " AND degrees.DegCod=courses.DegCod"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.Role='%u'"
			   " GROUP BY crs_usr.CrsCod) AS NumUsrsTable",
		     Gbl.CurrentCtr.Ctr.CtrCod,
		     (unsigned) Role);
         break;
      case Sco_SCOPE_DEG:
	 if (Role == Rol_UNKNOWN)	// Any user
	    sprintf (Query,"SELECT AVG(NumUsrs) FROM "
			   "(SELECT COUNT(crs_usr.UsrCod) AS NumUsrs"
			   " FROM courses,crs_usr"
			   " WHERE courses.DegCod='%ld'"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " GROUP BY crs_usr.CrsCod) AS NumUsrsTable",
		     Gbl.CurrentDeg.Deg.DegCod);
	 else
	    sprintf (Query,"SELECT AVG(NumUsrs) FROM "
			   "(SELECT COUNT(crs_usr.UsrCod) AS NumUsrs"
			   " FROM courses,crs_usr"
			   " WHERE courses.DegCod='%ld'"
			   " AND courses.CrsCod=crs_usr.CrsCod"
			   " AND crs_usr.Role='%u'"
			   " GROUP BY crs_usr.CrsCod) AS NumUsrsTable",
		     Gbl.CurrentDeg.Deg.DegCod,
		     (unsigned) Role);
         break;
      case Sco_SCOPE_CRS:
         return (float) ( Role == Rol_UNKNOWN ? Gbl.CurrentCrs.Crs.NumUsrs :	// Any user
                         (Role == Rol_TEACHER ? Gbl.CurrentCrs.Crs.NumTchs :	// Teachers
                                                Gbl.CurrentCrs.Crs.NumStds));	// Students
      default:
         Lay_ShowErrorAndExit ("Wrong scope.");
         break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of users per course");

   /***** Get number of users *****/
   row = mysql_fetch_row (mysql_res);
   NumUsrsPerCrs = Str_GetFloatNumFromStr (row[0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumUsrsPerCrs;
  }

/*****************************************************************************/
/****************** Check if a user is banned in ranking *********************/
/*****************************************************************************/

bool Usr_CheckIfUsrBanned (long UsrCod)
  {
   char Query[128];

   sprintf (Query,"SELECT COUNT(*) FROM usr_banned WHERE UsrCod='%ld'",
	    UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if user is banned") != 0);
  }

/*****************************************************************************/
/**************** Remove user from banned users in ranking *******************/
/*****************************************************************************/

void Usr_RemoveUsrFromUsrBanned (long UsrCod)
  {
   char Query[128];

   sprintf (Query,"DELETE FROM usr_banned WHERE UsrCod='%ld'",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove user from users banned");
  }
