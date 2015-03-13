// swad_user.c: users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Ca�as Vargas

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
#include "swad_config.h"
#include "swad_connected.h"
#include "swad_course.h"
#include "swad_database.h"
#include "swad_enrollment.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_ID.h"
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
   "classphoto",
   "list"
  };

#define Usr_NUM_MAIN_FIELDS_DATA_USR	 9
#define Usr_NUM_MAIN_FIELDS_DATA_ADM	 8
#define Usr_NUM_ALL_FIELDS_DATA_INV	17
#define Usr_NUM_ALL_FIELDS_DATA_STD	13
#define Usr_NUM_ALL_FIELDS_DATA_TCH	11
const char *Usr_UsrDatMainFieldNames[Usr_NUM_MAIN_FIELDS_DATA_USR];

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

struct UsrFigures
  {
   struct DateTime FirstClickTime;	//   0 ==> unknown first click time of user never logged
   int NumDays;				//  -1 ==> not applicable
   long NumClicks;			// -1L ==> unknown number of clicks
   long NumForPst;			// -1L ==> unknown number of forum posts
   long NumMsgSnt;			// -1L ==> unknown number of messages sent
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;
extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Usr_GetMyLastData (void);
static void Usr_GetUsrCommentsFromString (char *Str,struct UsrData *UsrDat);
static Usr_Sex_t Usr_GetSexFromStr (const char *Str);

static unsigned Usr_GetNumCrssOfUsrWithARole (long UsrCod,Rol_Role_t Role);
static unsigned Usr_GetNumUsrsInCrssOfAUsr (long UsrCod,Rol_Role_t UsrRole,Rol_Role_t OthersRole);

static bool Usr_CheckIfMyBirthdayHasNotBeenCongratulated (void);
static void Usr_InsertMyBirthday (void);

static void Usr_GetParamOtherUsrIDNickOrEMail (void);

static bool Usr_ChkUsrAndGetUsrDataFromDirectLogin (void);
static bool Usr_ChkUsrAndGetUsrDataFromExternalLogin (void);
static bool Usr_ChkUsrAndGetUsrDataFromSession (void);

static void Usr_SetUsrRoleAndPrefs (void);

static void Usr_InsertMyLastData (void);

static void Usr_WriteRowGstMainData (unsigned NumUsr,struct UsrData *UsrDat);
static void Usr_WriteRowTchMainData (unsigned NumUsr,struct UsrData *UsrDat,bool PutCheckBoxToSelectUsr);
static void Usr_WriteRowGstAllData (struct UsrData *UsrDat);
static void Usr_WriteMainUsrDataExceptUsrID (struct UsrData *UsrDat,const char *BgColor,bool ShowEmail,
                                             const char *MailLink,
                                             const char *InstitutionName,const char *InstitutionLink);
static void Usr_WriteUsrData (const char *BgColor,const char *Data,const char *Link,bool NonBreak,bool Accepted);

static void Usr_BuildQueryToGetUsrsLstCrs (Rol_Role_t Role,const char *UsrQuery,bool Search,char *Query);
static void Usr_GetAdmsLst (Sco_Scope_t Scope);
static void Usr_GetGstsLst (Sco_Scope_t Scope);
static void Usr_GetListUsrs (const char *Query,struct ListUsers *LstUsrs);
static void Usr_AllocateUsrsList (struct ListUsers *LstUsrs);
static void Usr_ShowWarningListIsTooBig (unsigned NumStds);
static void Usr_PutButtonToConfirmIWantToSeeBigList (unsigned NumUsrs);

static void Usr_AllocateListOtherRecipients (void);

static void Usr_FormToSelectUsrListType (Act_Action_t NextAction,Usr_ShowUsrsType_t ListType);

static Usr_Sex_t Usr_GetSexOfUsrsLst (struct ListUsers *LstUsrs);

static void Usr_SetUsrDatMainFieldNames (void);
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
static void Usr_PutLinkToListOfficialStudents (void);

static void Usr_DrawClassPhoto (Usr_ClassPhotoType_t ClassPhotoType,
                                Rol_Role_t RoleInClassPhoto);

static unsigned Usr_GetNumUsrsNotBelongingToAnyCrs (void);
static unsigned Usr_GetNumUsrsBelongingToAnyCrs (Rol_Role_t Role);
static float Usr_GetNumCrssPerUsr (Rol_Role_t Role);
static float Usr_GetNumUsrsPerCrs (Rol_Role_t Role);

static void Usr_ShowUserProfile (void);
static void Usr_GetUsrFigures (long UsrCod,struct UsrFigures *UsrFigures);
static unsigned long Usr_GetRankingNumClicks (long UsrCod);
static unsigned long Usr_GetNumUsrsWithNumClicks (void);
static unsigned long Usr_GetRankingNumClicksPerDay (long UsrCod);
static unsigned long Usr_GetNumUsrsWithNumClicksPerDay (void);
static void Usr_GetFirstClickFromLogAndStoreAsUsrFigure (long UsrCod);
static void Usr_GetNumClicksAndStoreAsUsrFigure (long UsrCod);
static void Usr_GetNumForPstAndStoreAsUsrFigure (long UsrCod);
static void Usr_GetNumMsgSntAndStoreAsUsrFigure (long UsrCod);
static void Usr_ResetUsrFigures (struct UsrFigures *UsrFigures);
static void Usr_CreateUsrFigures (long UsrCod,const struct UsrFigures *UsrFigures);
static bool Usr_CheckIfUsrFiguresExists (long UsrCod);

/*****************************************************************************/
/**** Show alert about number of clicks remaining before sending my photo ****/
/*****************************************************************************/

void Usr_InformAboutNumClicksBeforePhoto (void)
  {
   extern const char *Txt_You_must_send_your_photo_because_;
   extern const char *Txt_You_can_only_perform_X_further_actions_;
   char Message[512];        // Don't use Gbl.Message here, because it may be filled with another message and such message would be overwritten

   if (Gbl.Usrs.Me.NumAccWithoutPhoto)
     {
      if (Gbl.Usrs.Me.NumAccWithoutPhoto >= Pho_MAX_CLICKS_WITHOUT_PHOTO)
         Lay_ShowAlert (Lay_WARNING,Txt_You_must_send_your_photo_because_);
      else if (Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW)
        {
         sprintf (Message,Txt_You_can_only_perform_X_further_actions_,
                  Pho_MAX_CLICKS_WITHOUT_PHOTO-Gbl.Usrs.Me.NumAccWithoutPhoto);
         Lay_ShowAlert (Lay_WARNING,Message);

	 fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
	 Pho_PutLinkToChangeUsrPhoto (&Gbl.Usrs.Me.UsrDat);
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
   UsrDat->RoleInCurrentCrsDB = Rol_ROLE_UNKNOWN;
   UsrDat->Roles = 0;
   UsrDat->Accepted = true;

   UsrDat->Sex = Usr_SEX_UNKNOWN;
   UsrDat->Surname1[0] = '\0';
   UsrDat->Surname2[0] = '\0';
   UsrDat->FirstName[0] = '\0';
   UsrDat->FullName[0] = '\0';

   UsrDat->Email[0] = '\0';
   UsrDat->EmailConfirmed = false;

   UsrDat->Photo[0] = '\0';
   UsrDat->PhotoVisibility =
   UsrDat->ProfileVisibility = Pri_VISIBILITY_DEFAULT;

   UsrDat->CtyCod = -1L;
   UsrDat->OriginPlace[0] = '\0';
   UsrDat->StrBirthday[0] = '\0';
   UsrDat->Birthday.Day = 0;
   UsrDat->Birthday.Month = 0;
   UsrDat->Birthday.Year = 0;
   UsrDat->LocalAddress[0] = '\0';
   UsrDat->LocalPhone[0] = '\0';
   UsrDat->FamilyAddress[0] = '\0';
   UsrDat->FamilyPhone[0] = '\0';
   if (UsrDat->Comments)
      UsrDat->Comments[0] = '\0';

   UsrDat->InsCtyCod = -1L;
   UsrDat->InsCod = -1L;
   UsrDat->Tch.CtrCod = -1L;
   UsrDat->Tch.DptCod = -1L;
   UsrDat->Tch.Office[0] = '\0';
   UsrDat->Tch.OfficePhone[0] = '\0';

   UsrDat->Prefs.Language = Cfg_DEFAULT_LANGUAGE_FOR_NEW_USERS;
   UsrDat->Prefs.Layout = Lay_LAYOUT_DEFAULT;
   UsrDat->Prefs.Theme = The_THEME_DEFAULT;
   UsrDat->Prefs.IconSet = Ico_ICON_SET_DEFAULT;
   UsrDat->Prefs.Menu = Mnu_MENU_DEFAULT;
   UsrDat->Prefs.SideCols = Cfg_DEFAULT_COLUMNS;
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
/** Get user code from user's record in database using encrypted user code ***/
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
      sprintf (Query,"SELECT UsrCod FROM usr_data WHERE EncryptedUsrCod='%s'",
               UsrDat->EncryptedUsrCod);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get user's code");

      if (NumRows != 1)
         Lay_ShowErrorAndExit ("Error when getting user's code.");

      /***** Get user code *****/
      row = mysql_fetch_row (mysql_res);
      UsrDat->UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      UsrDat->UsrCod = -1L;
  }

/*****************************************************************************/
/************ Get user's data from database giving a user's code *************/
/*****************************************************************************/
// UsrDat->UsrCod must contain an existing user's code

void Usr_GetUsrDataFromUsrCod (struct UsrData *UsrDat)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
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
                  "Layout,Theme,IconSet,Language,Photo,PhotoVisibility,ProfileVisibility,"
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

   /* Get encrypted user code */
   strncpy (UsrDat->EncryptedUsrCod,row[0],sizeof (UsrDat->EncryptedUsrCod)-1);
   UsrDat->EncryptedUsrCod[sizeof (UsrDat->EncryptedUsrCod)-1] = '\0';

   /* Get encrypted password */
   strncpy (UsrDat->Password,row[1],sizeof (UsrDat->Password)-1);
   UsrDat->Password[sizeof (UsrDat->Password)-1] = '\0';

   /* Get roles */
   UsrDat->RoleInCurrentCrsDB = Rol_GetRoleInCrs (Gbl.CurrentCrs.Crs.CrsCod,UsrDat->UsrCod);
   UsrDat->Roles = Rol_GetRolesInAllCrss (UsrDat->UsrCod);
   if (UsrDat->RoleInCurrentCrsDB == Rol_ROLE_UNKNOWN)
      UsrDat->RoleInCurrentCrsDB = (UsrDat->Roles < (1 << Rol_ROLE_STUDENT)) ?
	                           Rol_ROLE_GUEST__ :	// User does not belong to any course
	                           Rol_ROLE_VISITOR;	// User belongs to some courses

   /* Get name */
   strncpy (UsrDat->Surname1 ,row[2],sizeof (UsrDat->Surname1 )-1);
   strncpy (UsrDat->Surname2 ,row[3],sizeof (UsrDat->Surname2 )-1);
   strncpy (UsrDat->FirstName,row[4],sizeof (UsrDat->FirstName)-1);

   /* Get sex */
   UsrDat->Sex = Usr_GetSexFromStr (row[5]);

   /* Get layout */
   UsrDat->Prefs.Layout = Lay_LAYOUT_DEFAULT;
   if (sscanf (row[6],"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Lay_NUM_LAYOUTS)
         UsrDat->Prefs.Layout = (Lay_Layout_t) UnsignedNum;

   /* Get theme */
   UsrDat->Prefs.Theme = The_THEME_DEFAULT;
   for (Theme = (The_Theme_t) 0;
        Theme < The_NUM_THEMES;
        Theme++)
      if (!strcasecmp (row[7],The_ThemeId[Theme]))
        {
         UsrDat->Prefs.Theme = Theme;
         break;
        }

   /* Get icon set */
   UsrDat->Prefs.IconSet = Ico_ICON_SET_DEFAULT;
   for (IconSet = (Ico_IconSet_t) 0;
        IconSet < Ico_NUM_ICON_SETS;
        IconSet++)
      if (!strcasecmp (row[8],Ico_IconSetId[IconSet]))
        {
         UsrDat->Prefs.IconSet = IconSet;
         break;
        }

   /* Get language */
   UsrDat->Prefs.Language = Cfg_DEFAULT_LANGUAGE_FOR_NEW_USERS;
   for (Lan = (Txt_Language_t) 0;
        Lan < Txt_NUM_LANGUAGES;
        Lan++)
      if (!strcasecmp (row[9],Txt_STR_LANG_ID[Lan]))
        {
         UsrDat->Prefs.Language = Lan;
         break;
        }

   /* Get rest of data */
   strncpy (UsrDat->Photo,row[10],sizeof (UsrDat->Photo)-1);
   UsrDat->PhotoVisibility   = Pri_GetVisibilityFromStr (row[11]);
   UsrDat->ProfileVisibility = Pri_GetVisibilityFromStr (row[12]);
   UsrDat->CtyCod = Str_ConvertStrCodToLongCod (row[13]);
   UsrDat->InsCtyCod = Str_ConvertStrCodToLongCod (row[14]);
   UsrDat->InsCod = Str_ConvertStrCodToLongCod (row[15]);

   UsrDat->Tch.DptCod = Str_ConvertStrCodToLongCod (row[16]);
   UsrDat->Tch.CtrCod = Str_ConvertStrCodToLongCod (row[17]);
   strncpy (UsrDat->Tch.Office     ,row[18],sizeof (UsrDat->Tch.Office     )-1);
   strncpy (UsrDat->Tch.OfficePhone,row[19],sizeof (UsrDat->Tch.OfficePhone)-1);

   strncpy (UsrDat->LocalAddress   ,row[20],sizeof (UsrDat->LocalAddress   )-1);
   strncpy (UsrDat->LocalPhone     ,row[21],sizeof (UsrDat->LocalPhone     )-1);
   strncpy (UsrDat->FamilyAddress  ,row[22],sizeof (UsrDat->FamilyAddress  )-1);
   strncpy (UsrDat->FamilyPhone    ,row[23],sizeof (UsrDat->FamilyPhone    )-1);
   strncpy (UsrDat->OriginPlace    ,row[24],sizeof (UsrDat->OriginPlace    )-1);
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

   /* Get on which events I want to be notified by e-mail */
   if (sscanf (row[29],"%u",&UsrDat->Prefs.NotifNtfEvents) != 1)
      UsrDat->Prefs.NotifNtfEvents = (unsigned) -1;	// 0xFF..FF

   if (sscanf (row[30],"%u",&UsrDat->Prefs.EmailNtfEvents) != 1)
      UsrDat->Prefs.EmailNtfEvents = 0;
   if (UsrDat->Prefs.EmailNtfEvents >= (1 << Ntf_NUM_NOTIFY_EVENTS))	// Maximum binary value for NotifyEvents is 000...0011...11
      UsrDat->Prefs.EmailNtfEvents = 0;

   UsrDat->Surname1[sizeof (UsrDat->Surname1)-1] = '\0';
   UsrDat->Surname2[sizeof (UsrDat->Surname2)-1] = '\0';
   UsrDat->FirstName[sizeof (UsrDat->FirstName)-1] = '\0';
   Str_ConvertToTitleType (UsrDat->Surname1 );
   Str_ConvertToTitleType (UsrDat->Surname2 );
   Str_ConvertToTitleType (UsrDat->FirstName);
   /* Create full name using FirstName, Surname1 and Surname2 */
   Usr_BuildFullName (UsrDat);

   UsrDat->Photo[sizeof (UsrDat->Photo)-1] = '\0';

   UsrDat->OriginPlace  [sizeof (UsrDat->OriginPlace  )-1] = '\0';
   if (sscanf (StrBirthday,"%u-%u-%u",
	       &(UsrDat->Birthday.Year),
	       &(UsrDat->Birthday.Month),
	       &(UsrDat->Birthday.Day)) != 3)
      Lay_ShowErrorAndExit ("Wrong date.");
   Dat_ConvDateToDateStr (&(UsrDat->Birthday),UsrDat->StrBirthday);
   UsrDat->LocalAddress [sizeof (UsrDat->LocalAddress )-1] = '\0';
   UsrDat->LocalPhone   [sizeof (UsrDat->LocalPhone   )-1] = '\0';
   UsrDat->FamilyAddress[sizeof (UsrDat->FamilyAddress)-1] = '\0';
   UsrDat->FamilyPhone  [sizeof (UsrDat->FamilyPhone  )-1] = '\0';

   UsrDat->Tch.Office     [sizeof (UsrDat->Tch.Office     )-1] = '\0';
   UsrDat->Tch.OfficePhone[sizeof (UsrDat->Tch.OfficePhone)-1] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get nickname *****/
   Nck_GetNicknameFromUsrCod (UsrDat->UsrCod,UsrDat->Nickname);

   /***** Get e-mail *****/
   Mai_GetEmailFromUsrCod (UsrDat->UsrCod,UsrDat->Email,&(UsrDat->EmailConfirmed));
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
         if (UnsignedNum >= 1 || UnsignedNum <= Tab_NUM_TABS)
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

void Usr_RestrictLengthAndWriteName (struct UsrData *UsrDat,unsigned MaxChars)
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

   /***** Replace tildes, �, etc. by codes, because some browsers (i.e., IE5) don't show correctly tildes with AJAX *****/
   Str_ReplaceSpecialCharByCodes (FirstName,Usr_MAX_BYTES_NAME_SPEC_CHAR);
   Str_ReplaceSpecialCharByCodes (Surnames,2*Usr_MAX_BYTES_NAME_SPEC_CHAR+1);

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
/********* Get number of courses in with a user have a given role ************/
/*****************************************************************************/

static unsigned Usr_GetNumCrssOfUsrWithARole (long UsrCod,Rol_Role_t Role)
  {
   char Query[128];

   /***** Get the number of teachers in a course from database ******/
   sprintf (Query,"SELECT COUNT(*) FROM crs_usr"
                  " WHERE UsrCod='%ld' AND Role='%u'",
            UsrCod,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of courses of a user with a role");
  }

/*****************************************************************************/
/******* Get number of users with different role in courses of a user ********/
/*****************************************************************************/

static unsigned Usr_GetNumUsrsInCrssOfAUsr (long UsrCod,Rol_Role_t UsrRole,Rol_Role_t OthersRole)
  {
   char Query[256];

   /***** Get the number of teachers in a course from database ******/
   sprintf (Query,"SELECT COUNT(DISTINCT UsrCod) FROM crs_usr"
                  " WHERE CrsCod IN"
                  " (SELECT CrsCod FROM crs_usr WHERE UsrCod='%ld' AND Role='%u')"
                  " AND Role='%u'",
            UsrCod,(unsigned) UsrRole,(unsigned) OthersRole);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of users");
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
                  " AND my_courses.Role!=usr_courses.Role",
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
/******************* Check if a user belongs to a degree *********************/
/*****************************************************************************/

bool Usr_CheckIfUsrBelongsToDeg (long UsrCod,long DegCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;

   /***** Get is a user belongs to a degree from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT courses.DegCod)"
	          " FROM crs_usr,courses"
                  " WHERE crs_usr.UsrCod='%ld' AND courses.DegCod='%ld'"
                  " AND crs_usr.CrsCod=courses.CrsCod",
            UsrCod,DegCod);
   DB_QuerySELECT (Query,&mysql_res,"can not check if a user belongs to a degree");

   /***** Get number of rows *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumRows) != 1)
      Lay_ShowErrorAndExit ("Error when checking if a user belongs to a degree.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return (NumRows > 0);
  }

/*****************************************************************************/
/******************** Check if a user belongs to a course ********************/
/*****************************************************************************/

bool Usr_CheckIfUsrBelongsToCrs (long UsrCod,long CrsCod)
  {
   char Query[256];

   /***** If user code or course code not valid... *****/
   if (UsrCod <= 0 ||
       CrsCod <= 0)
      return false;

   /***** Get if a user belongs to a course from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM crs_usr"
	          " WHERE CrsCod='%ld' AND UsrCod='%ld'",
            CrsCod,UsrCod);
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
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
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
   sprintf (Query,"SELECT COUNT(*) FROM usr_data WHERE EncryptedUsrCod='%s'",
            EncryptedUsrCod);
   return (DB_QueryCOUNT (Query,"can not check if an encrypted user's code already existed") != 0);
  }

/*****************************************************************************/
/****************** Write form for user log in or log out ********************/
/*****************************************************************************/

void Usr_WriteFormLoginLogout (void)
  {
   if (Gbl.Session.IsOpen)
      /***** Form to change my role *****/
      Usr_ShowFormsRoleAndLogout ();
   else
      /***** Form to log in *****/
      Usr_WriteFormLogin ();
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
/************************ Write form for user log in *************************/
/*****************************************************************************/

void Usr_WriteFormLogin (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_New_on_PLATFORM_Sign_up;
   extern const char *Txt_Create_account;
   extern const char *Txt_Enter_from_X;
   extern const char *Txt_nick_email_or_ID;
   extern const char *Txt_Password;
   extern const char *Txt_Log_in;
   extern const char *Txt_I_forgot_my_password;

   /***** Link to create a new account *****/
   fprintf (Gbl.F.Out,"<div style=\"text-align:center; margin-bottom:20px;\">");
   Act_FormStart (ActFrmUsrAcc);
   sprintf (Gbl.Title,Txt_New_on_PLATFORM_Sign_up,Cfg_PLATFORM_SHORT_NAME);
   Act_LinkFormSubmit (Gbl.Title,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("arroba",Txt_Create_account,Gbl.Title);
   Act_FormEnd ();

   /***** Link to enter from external site *****/
   if (Cfg_EXTERNAL_LOGIN_URL[0] &&
       Cfg_EXTERNAL_LOGIN_SERVICE_SHORT_NAME[0])
     {
      fprintf (Gbl.F.Out,"<a href=\"%s\" class=\"%s\" style=\"margin-left:10px;\">",
               Cfg_EXTERNAL_LOGIN_URL,
               The_ClassFormul[Gbl.Prefs.Theme]);
      sprintf (Gbl.Title,Txt_Enter_from_X,
               Cfg_EXTERNAL_LOGIN_SERVICE_SHORT_NAME);
      Lay_PutSendIcon ("login",Gbl.Title,Gbl.Title);
     }
   fprintf (Gbl.F.Out,"</div>");

   /***** Form start *****/
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
   Act_FormStart (ActAutUsrInt);
   Lay_StartRoundFrameTable10 (NULL,2,Txt_Log_in);

   /***** User's ID/nickname and password *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s\" style=\"text-align:right;"
	              " vertical-align:middle;\">"
                      "%s:"
                      "</td>"
                      "<td style=\"text-align:left; vertical-align:middle;\">"
                      "<input type=\"text\" id=\"UsrId\" name=\"UsrId\""
                      " size=\"16\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>"
		      "</tr>"
		      "<tr>"
		      "<td class=\"%s\" style=\"text-align:right;"
	              " vertical-align:middle;\">"
		      "%s:"
		      "</td>"
		      "<td style=\"text-align:left; vertical-align:middle;\">"
		      "<input type=\"password\" name=\"UsrPwd\""
		      " size=\"16\" maxlength=\"%u\" />"
		      "</td>"
		      "</tr>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_nick_email_or_ID,
            Usr_MAX_LENGTH_USR_LOGIN,
            Gbl.Usrs.Me.UsrIdLogin,
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Password,
            Pwd_MAX_LENGTH_PLAIN_PASSWORD);

   /***** Send button and form end *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" style=\"text-align:center;\">"
                      "<input type=\"submit\" value=\"%s\" />"
                      "</td>"
	              "</tr>",
	              Txt_Log_in);
   Lay_EndRoundFrameTable10 ();
   Act_FormEnd ();

   /***** Link used for sending a new password *****/
   Act_FormStart (ActReqSndNewPwd);
   Par_PutHiddenParamString ("UsrId",Gbl.Usrs.Me.UsrIdLogin);
   Act_LinkFormSubmit (Txt_I_forgot_my_password,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("key",Txt_I_forgot_my_password,Txt_I_forgot_my_password);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******************* Write type and name of logged user **********************/
/*****************************************************************************/

void Usr_WelcomeUsr (void)
  {
   extern const unsigned Txt_Current_CGI_SWAD_Language;
   extern const char *Txt_Welcome[Usr_NUM_SEXS];
   extern const char *Txt_Welcome_X[Usr_NUM_SEXS];
   extern const char *Txt_Welcome_X_and_happy_birthday[Usr_NUM_SEXS];
   extern const char *Txt_You_dont_have_photo;
   extern const char *Txt_Switching_to_LANGUAGE[Txt_NUM_LANGUAGES];
   bool CongratulateMyBirthday;

   if (Gbl.Usrs.Me.Logged)
     {
      if (Gbl.Usrs.Me.UsrDat.Prefs.Language == Txt_Current_CGI_SWAD_Language)
        {
	 fprintf (Gbl.F.Out,"<div style=\"text-align:center; margin:10px;\">");

         /***** User's first name *****/
         if (Gbl.Usrs.Me.UsrDat.FirstName[0])
           {
            CongratulateMyBirthday = false;
            if (Gbl.Usrs.Me.UsrDat.Birthday.Day   == Gbl.Now.Date.Day &&
                Gbl.Usrs.Me.UsrDat.Birthday.Month == Gbl.Now.Date.Month)
               if ((CongratulateMyBirthday = Usr_CheckIfMyBirthdayHasNotBeenCongratulated ()))
                 {
                  Usr_InsertMyBirthday ();
                  fprintf (Gbl.F.Out,"<img src=\"%s/%s/cake128x128.gif\" alt=\"\""
                	             " style=\"width:128px; height:128px;\" />",
                           Gbl.Prefs.PathIconSet,Cfg_ICON_128x128);
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

         if (Gbl.Usrs.Me.UsrDat.Password[0] &&
             Gbl.Usrs.Me.UsrDat.FirstName[0] &&
             Gbl.Usrs.Me.UsrDat.Surname1[0] &&
             !Gbl.Usrs.Me.MyPhotoExists)	// Check if I have no photo
	   {
	    Lay_ShowAlert (Lay_WARNING,Txt_You_dont_have_photo);
	    Pho_PutLinkToChangeUsrPhoto (&Gbl.Usrs.Me.UsrDat);
	   }

	 fprintf (Gbl.F.Out,"</div>");

         /***** Show the global announcements I have not seen *****/
         Ann_ShowMyAnnouncementsNotMarkedAsSeen ();
        }
      else        // The current language is not my preferred language ==> change automatically to my language
         Lay_ShowAlert (Lay_INFO,Txt_Switching_to_LANGUAGE[Gbl.Usrs.Me.UsrDat.Prefs.Language]);
     }
  }

/******************************************************************************/
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
   fprintf (Gbl.F.Out,"<img src=\"%s/login16x16.gif\""
                      " alt=\"%s\" class=\"ICON16x16\" />"
                      " %s</a>",
            Gbl.Prefs.PathTheme,Txt_Log_in,Txt_Log_in);
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
   static const unsigned NumCharsName[Lay_NUM_LAYOUTS] =
     {
      16,        // Lay_LAYOUT_DESKTOP
      10,        // Lay_LAYOUT_MOBILE
     };
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];
   char UsrFullName[(Usr_MAX_BYTES_NAME+1)*3];

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

   /***** Show photo *****/
   ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&Gbl.Usrs.Me.UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&Gbl.Usrs.Me.UsrDat,ShowPhoto ? PhotoURL :
                	                             NULL,
                     "PHOTO15x20",Pho_ZOOM);

   /***** User's name *****/
   fprintf (Gbl.F.Out,"<span class=\"%s\">&nbsp;",
            The_ClassUsr[Gbl.Prefs.Theme]);

   /* Name */
   if (Gbl.Usrs.Me.UsrDat.FullName[0])
     {
      strcpy (UsrFullName,Gbl.Usrs.Me.UsrDat.FullName);
      Str_LimitLengthHTMLStr (UsrFullName,NumCharsName[Gbl.Prefs.Layout]);
      fprintf (Gbl.F.Out,"%s",UsrFullName);
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
   fprintf (Gbl.F.Out,"<img src=\"%s/logout16x16.gif\" alt=\"%s\""
                      " class=\"ICON16x16\" style=\"vertical-align:middle;\" />"
                      " %s</a>",
            Gbl.Prefs.PathTheme,Txt_Log_out,Txt_Log_out);
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
   Str_ConvertToUpperText (Gbl.Usrs.Me.UsrIdLogin);
  }

/*****************************************************************************/
/******* Get parameter user's identificator of other user from a form ********/
/*****************************************************************************/

static void Usr_GetParamOtherUsrIDNickOrEMail (void)
  {
   /***** Get parameter with the plain user's ID, @nick or e-mail of another user *****/
   Par_GetParToText ("OtherUsrIDNickOrEMail",Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail,Usr_MAX_BYTES_USR_LOGIN);

   // If it's a user's ID (if does not contain '@')
   if (strchr (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail,(int) '@') != NULL)	// '@' found
     {
      // Users' IDs are always stored internally in capitals and without leading zeros
      Str_RemoveLeadingZeros (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail);
      Str_ConvertToUpperText (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail);
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

   /***** Reset default list of users' codes *****/
   ListUsrCods->NumUsrs = 0;
   ListUsrCods->Lst = NULL;

   /***** Get parameter with the plain user's ID, @nick or e-mail of another user *****/
   Usr_GetParamOtherUsrIDNickOrEMail ();

   /***** Check if it's an ID, a nickname or an e-mail address *****/
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
      Str_ConvertToUpperText (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail);
      if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail))
	{
	 if (strlen (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail) <= ID_MAX_LENGTH_USR_ID)
	   {
	    /* Allocate space for the list */
	    ID_ReallocateListIDs (&Gbl.Usrs.Other.UsrDat,1);

	    strncpy (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID,Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail,ID_MAX_LENGTH_USR_ID);
	    Gbl.Usrs.Other.UsrDat.IDs.List[0].ID[ID_MAX_LENGTH_USR_ID] = '\0';

	    /* Check if user's ID exists in database */
	    ID_GetListUsrCodsFromUsrID (&Gbl.Usrs.Other.UsrDat,NULL,ListUsrCods,false);
	   }
	}
      else
	{
	 /***** String is not a valid user's nickname, e-mail or ID *****/
	 sprintf (Gbl.Message,Txt_The_ID_nickname_or_email_X_is_not_valid,
		  Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	}
     }

   return ListUsrCods->NumUsrs;
  }

/*****************************************************************************/
/********* Put hidden parameter encrypted user's code of other user **********/
/*****************************************************************************/

void Usr_PutParamOtherUsrCodEncrypted (const char EncryptedUsrCod[Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64+1])
  {
   Par_PutHiddenParamString ("OtherUsrCod",EncryptedUsrCod);
  }

/*****************************************************************************/
/********* Get hidden parameter encrypted user's code of other user **********/
/*****************************************************************************/

void Usr_GetParamOtherUsrCodEncrypted (void)
  {
   Par_GetParToText ("OtherUsrCod",Gbl.Usrs.Other.UsrDat.EncryptedUsrCod,Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
   if (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod[0])        // If parameter exists...
     {
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
      if (Gbl.Usrs.Other.UsrDat.UsrCod < 0)        // Check is user's code is valid
         Lay_ShowErrorAndExit ("Wrong user's code.");
      ID_GetListIDsFromUsrCod (&Gbl.Usrs.Other.UsrDat);
     }
   else        // Parameter does not exist
     {
      Gbl.Usrs.Other.UsrDat.UsrCod = -1L;
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
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Check if user exists and get his data *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))        // Existing user
      return true;

   Lay_ShowAlert (Lay_ERROR,"User does not exist.");
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
      if (Gbl.CurrentAct != ActRefCon)
	{
	 Gbl.CurrentAct = ActLogOut;
	 Tab_SetCurrentTab ();
	 Lay_ShowAlert (Lay_WARNING,Txt_The_session_has_expired_due_to_inactivity);
	 PutFormLogin = true;
	}
     }
   else	// !Gbl.Session.HasBeenDisconnected
     {
      /***** Check user and get user's data *****/
      if (Gbl.CurrentAct == ActCreUsrAcc)
	{
	 /***** Create new account and login *****/
	 if (Acc_CreateNewAccountAndLogIn ())			// User logged in
	   {
	    Gbl.Usrs.Me.Logged = true;
	    Usr_SetUsrRoleAndPrefs ();

	    Act_AdjustCurrentAction ();
	    Ses_CreateSession ();

	    /* Set preferences from current IP */
	    Prf_SetPrefsFromIP ();

	    /* Send message via email to confirm the new email address */
	    Mai_SendMailMsgToConfirmEmail ();
	   }
	}
      else	// Gbl.CurrentAct != ActCreUsrAcc
	{
	 /***** Check user and get user's data *****/
	 if (Gbl.Session.IsOpen)
	   {
	    if (Usr_ChkUsrAndGetUsrDataFromSession ())		// User logged in
	      {
	       Gbl.Usrs.Me.Logged = true;
	       Usr_SetUsrRoleAndPrefs ();

	       if (Gbl.CurrentAct == ActRefCon)	// If refreshing connected users ==> don't refresh session
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
	 else if (Gbl.CurrentAct == ActAutUsrInt)
	   {
	    if (Usr_ChkUsrAndGetUsrDataFromDirectLogin ())		// User logged in
	      {
	       Gbl.Usrs.Me.Logged = true;
	       Usr_SetUsrRoleAndPrefs ();

	       Act_AdjustCurrentAction ();
	       Ses_CreateSession ();

	       Prf_SetPrefsFromIP ();	// Set preferences from current IP
	      }
	    else
	       PutFormLogin = true;
	   }
	 else if (Gbl.CurrentAct == ActAutUsrExt)
	   {
	    if (Usr_ChkUsrAndGetUsrDataFromExternalLogin ())	// User logged in
	      {
	       Gbl.Usrs.Me.Logged = true;
	       Usr_SetUsrRoleAndPrefs ();

	       Act_AdjustCurrentAction ();
	       Ses_CreateSession ();

	       Prf_SetPrefsFromIP ();	// Set preferences from current IP

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
   if (Gbl.CurrentAct != ActRefCon)
     {
      if (Gbl.Usrs.Me.Logged)
	{
	 /***** Set default tab when unknown *****/
	 if (Gbl.CurrentTab == TabUnk)
	   {
	    // Don't adjust Gbl.CurrentAct here
	    Gbl.CurrentTab = ((Gbl.Usrs.Me.UsrLast.LastTab == TabCrs) &&
			      (Gbl.CurrentCrs.Crs.CrsCod <= 0)) ? TabSys :
								  Gbl.Usrs.Me.UsrLast.LastTab;
	    Tab_DisableIncompatibleTabs ();
	   }
	 Usr_UpdateMyLastData ();
	 Crs_UpdateCrsLast ();
	}
      else if (Gbl.CurrentAct == ActUnk)	// No user logged and unknown action
	 Act_AdjustActionWhenNoUsrLogged ();

      /***** When I change to another tab, go to the first option allowed *****/
      if (Gbl.CurrentAct == ActMnu &&
	  Gbl.Prefs.Layout == Lay_LAYOUT_DESKTOP)
	{
	 if (Gbl.Usrs.Me.Logged)
	   {
	    if ((Action = Act_GetMyLastActionInCurrentTab ()) == ActUnk)
	       Action = Act_GetFirstActionAvailableInCurrentTab ();
	   }
	 else
	    Action = Act_GetFirstActionAvailableInCurrentTab ();
	 if (Action != ActUnk)
	    Gbl.CurrentAct = Action;
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
   extern const char *Txt_The_user_does_not_exist_or_password_is_incorrect;
   extern const char *Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email;
   struct ListUsrCods ListUsrCods;
   bool PasswordCorrect = false;

   /***** Check if user has typed his user's ID, his nickname or his e-mail address *****/
   if (Nck_CheckIfNickWithArrobaIsValid (Gbl.Usrs.Me.UsrIdLogin))	// 1: It's a nickname
     {
      // User is trying to log using his/her nickname
      if ((Gbl.Usrs.Me.UsrDat.UsrCod = Nck_GetUsrCodFromNickname (Gbl.Usrs.Me.UsrIdLogin)) <= 0)
	{
	 Lay_ShowAlert (Lay_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
	 return false;
	}
     }
   else if (Mai_CheckIfEmailIsValid (Gbl.Usrs.Me.UsrIdLogin))		// 2: It's an e-mail
     {
      // User is trying to log using his/her e-mail
      if ((Gbl.Usrs.Me.UsrDat.UsrCod = Mai_GetUsrCodFromEmail (Gbl.Usrs.Me.UsrIdLogin)) <= 0)
	{
	 Lay_ShowAlert (Lay_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
	 return false;
	}
     }
   else									// 3: It's not a nickname nor e-mail
     {
      // Users' IDs are always stored internally in capitals and without leading zeros
      Str_RemoveLeadingZeros (Gbl.Usrs.Me.UsrIdLogin);
      Str_ConvertToUpperText (Gbl.Usrs.Me.UsrIdLogin);
      if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Me.UsrIdLogin))
	{
	 // User is trying to log using his/her ID
	 if (strlen (Gbl.Usrs.Me.UsrIdLogin) > ID_MAX_LENGTH_USR_ID)        // User's ID too long
	   {
	    Lay_ShowAlert (Lay_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
	    return false;
	   }

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

	       sprintf (Gbl.Message,Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email,
			Gbl.Usrs.Me.UsrIdLogin);
	       Lay_ShowAlert (Lay_WARNING,Gbl.Message);

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
	 	  Lay_ShowAlert (Lay_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
	 	  return false;
	 	 }
	      }
	    else	// ListUsrCods.NumUsrs > 1
	      {
	       /* Free memory used for list of users' codes found for this ID */
	       Usr_FreeListUsrCods (&ListUsrCods);

	       sprintf (Gbl.Message,Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email,
		        Gbl.Usrs.Me.UsrIdLogin);
	       Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	       return false;
	      }
	   }
	 else	// No users found for this ID
	   {
	    Lay_ShowAlert (Lay_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
	    return false;
	   }
	}
      else	// String is not a valid user's nickname, e-mail or ID
	{
	 Lay_ShowAlert (Lay_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
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
	 Lay_ShowAlert (Lay_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
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
   extern const char *Txt_The_user_does_not_exist_or_password_is_incorrect;
   extern const char *Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email;
   extern const char *Txt_There_is_no_user_in_X_with_ID_Y_If_you_already_have_an_account_on_Z_;
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
   Str_ConvertToUpperText (Gbl.Usrs.Me.UsrIdLogin);

   /***** Check if user's ID is valid *****/
   if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Me.UsrIdLogin))
     {
      // User is trying to log using his/her ID
      if (strlen (Gbl.Usrs.Me.UsrIdLogin) > ID_MAX_LENGTH_USR_ID)        // User's ID too long
	{
	 Lay_ShowAlert (Lay_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
	 return false;
	}

      /***** Allocate space for the list *****/
      ID_ReallocateListIDs (&Gbl.Usrs.Me.UsrDat,1);

      strncpy (Gbl.Usrs.Me.UsrDat.IDs.List[0].ID,Gbl.Usrs.Me.UsrIdLogin,ID_MAX_LENGTH_USR_ID);
      Gbl.Usrs.Me.UsrDat.IDs.List[0].ID[ID_MAX_LENGTH_USR_ID] = '\0';

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

	    sprintf (Gbl.Message,Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email,
		     Gbl.Usrs.Me.UsrIdLogin);
	    Lay_ShowAlert (Lay_WARNING,Gbl.Message);

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
	    ID_ConfirmUsrID (Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Me.UsrIdLogin);

	    /* Get user's data */
	    Usr_GetAllUsrDataFromUsrCod (&Gbl.Usrs.Me.UsrDat);
	   }
	 else	// ListUsrCods.NumUsrs > 1
	   {
	    /* Free memory used for list of users' codes found for this ID */
	    Usr_FreeListUsrCods (&ListUsrCods);

	    sprintf (Gbl.Message,Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email,
		     Gbl.Usrs.Me.UsrIdLogin);
	    Lay_ShowAlert (Lay_WARNING,Gbl.Message);

	    return false;
	   }
	}
      else
	 ItSeemsANewUsrIsEnteringFromExternalSite = true;
     }
   else	// String is not a valid user's nickname, e-mail or ID
     {
      Lay_ShowAlert (Lay_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
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
	 case Rol_ROLE_STUDENT:	// Student
	 case Rol_ROLE_TEACHER:	// Teacher
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
	 sprintf (Gbl.Message,Txt_There_is_no_user_in_X_with_ID_Y_If_you_already_have_an_account_on_Z_,
	          Cfg_PLATFORM_SHORT_NAME,
	          Gbl.Usrs.Me.UsrIdLogin,
	          Cfg_PLATFORM_SHORT_NAME);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
         return false;
	}
     }
   else	// External user's ID or session are not valid
     {
      Lay_ShowAlert (Lay_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
      return false;
     }

   return true;	// User is now logged
  }

/*****************************************************************************/
/******** Check user and get user's data when the session is open ************/
/*****************************************************************************/

static bool Usr_ChkUsrAndGetUsrDataFromSession (void)
  {
   extern const char *Txt_The_user_does_not_exist_or_password_is_incorrect;

   /***** Session is open and user's code is get from session *****/
   Gbl.Usrs.Me.UsrDat.UsrCod = Gbl.Session.UsrCod;

   /* Check if user exists in database, and get his/her data */
   if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Me.UsrDat))
     {
      Lay_ShowAlert (Lay_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
      return false;
     }

   /* Check user's password:
      the encrypted password typed by user or coming from the session
      is the same as the stored in database? */
   if (!Pwd_CheckCurrentPassword ())	// If my password is not correct...
     {
      Lay_ShowAlert (Lay_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
      return false;
     }

   return true;
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

   /***** Set preferences from my preferences *****/
   Gbl.Prefs.Theme = Gbl.Usrs.Me.UsrDat.Prefs.Theme;
   sprintf (Gbl.Prefs.PathTheme,"%s/%s/%s",
	    Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_THEMES,
	    The_ThemeId[Gbl.Prefs.Theme]);

   Gbl.Prefs.IconSet = Gbl.Usrs.Me.UsrDat.Prefs.IconSet;
   sprintf (Gbl.Prefs.PathIconSet,"%s/%s/%s",
	    Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_ICON_SETS,
	    Ico_IconSetId[Gbl.Prefs.IconSet]);

   Gbl.Prefs.Menu = Gbl.Usrs.Me.UsrDat.Prefs.Menu;
   Gbl.Prefs.SideCols = Gbl.Usrs.Me.UsrDat.Prefs.SideCols;

   /***** Get my last data *****/
   Usr_GetMyLastData ();
   if (Gbl.CurrentAct == ActAutUsrInt ||
       Gbl.CurrentAct == ActAutUsrExt)	// If I just logged in...
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
   Gbl.Usrs.Me.LoggedRole = (Gbl.Usrs.Me.RoleFromSession == Rol_ROLE_UNKNOWN) ?	// If no logged role retrieved from session...
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
      Gbl.Usrs.Me.UsrDat.Accepted = Usr_GetIfUserHasAcceptedEnrollmentInCurrentCrs (Gbl.Usrs.Me.UsrDat.UsrCod);

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
        {
         if (Gbl.Imported.ExternalRole == Rol_ROLE_UNKNOWN &&				// I logged in directly, not from external service...
             Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB == Rol_ROLE_STUDENT &&		// ...and I am a student in the current course...
             !Gbl.CurrentDegTyp.DegTyp.AllowDirectLogIn &&				// ...but the current degree type...
             !Gbl.CurrentCrs.Crs.AllowDirectLogIn)					// ...and the current course do not allow to log in directly
            Gbl.Usrs.Me.AvailableRoles = (1 << Rol_ROLE_VISITOR);	// In this case, my role will be visitor, and an alert will be shown
         else
            Gbl.Usrs.Me.AvailableRoles = (1 << Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB);
        }
      else if (Gbl.Usrs.Me.MaxRole >= Rol_ROLE_STUDENT)
         Gbl.Usrs.Me.AvailableRoles = (1 << Rol_ROLE_VISITOR);
      else
         Gbl.Usrs.Me.AvailableRoles = (1 << Rol_ROLE_GUEST__);
     }
   else        // No course selected
      Gbl.Usrs.Me.AvailableRoles = (1 << Gbl.Usrs.Me.MaxRole);
   if (ICanBeInsAdm)
      Gbl.Usrs.Me.AvailableRoles |= (1 << Rol_ROLE_INS_ADM);
   if (ICanBeCtrAdm)
      Gbl.Usrs.Me.AvailableRoles |= (1 << Rol_ROLE_CTR_ADM);
   if (ICanBeDegAdm)
      Gbl.Usrs.Me.AvailableRoles |= (1 << Rol_ROLE_DEG_ADM);
   if (Usr_CheckIfUsrIsSuperuser (Gbl.Usrs.Me.UsrDat.UsrCod))
      Gbl.Usrs.Me.AvailableRoles |= (1 << Rol_ROLE_SYS_ADM);

   /***** Check if the role I am logged is now available for me *****/
   if (!(Gbl.Usrs.Me.AvailableRoles & (1 << Gbl.Usrs.Me.LoggedRole)))        // Current type I am logged is not available for me
      /* Set the lowest role available for me */
      for (Gbl.Usrs.Me.LoggedRole = Rol_ROLE_UNKNOWN;
           Gbl.Usrs.Me.LoggedRole < Rol_NUM_ROLES;
           Gbl.Usrs.Me.LoggedRole++)
         if (Gbl.Usrs.Me.AvailableRoles & (1 << Gbl.Usrs.Me.LoggedRole))
            break;
  }

/*****************************************************************************/
/******** Write warning when degree type does not allow direct login *********/
/*****************************************************************************/
/*
When is forbidden direct log in?
                                                      Gbl.CurrentCrs.Crs.AllowDirectLogIn
                                                      false                          true
                                           false      forbidden                    allowed
Gbl.CurrentDegTyp.DegTyp.AllowDirectLogIn
                                           true       allowed                      allowed
*/

void Usr_WarningWhenDegreeTypeDoesntAllowDirectLogin (void)
  {
   extern const char *Txt_This_course_requires_log_in_from_X_to_have_full_functionality_;

   if (Cfg_EXTERNAL_LOGIN_URL[0] && Cfg_EXTERNAL_LOGIN_SERVICE_SHORT_NAME[0])
      /* If I belong to current course but my role in current course is visitor, show alert */
      if (Gbl.Usrs.Me.IBelongToCurrentCrs &&
          Gbl.Imported.ExternalRole == Rol_ROLE_UNKNOWN &&		// I logged in directly, not from external service...
          Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB == Rol_ROLE_STUDENT &&	// ...and I am a student in the current course...
          !Gbl.CurrentDegTyp.DegTyp.AllowDirectLogIn &&			// ...but the current degree type...
          !Gbl.CurrentCrs.Crs.AllowDirectLogIn &&			// ...and the current course do not allow to log in directly
          (Gbl.CurrentAct == ActSeeCrsInf ||
           Gbl.CurrentAct == ActAutUsrInt ||
           Gbl.CurrentAct == ActHom))
         {
          sprintf (Gbl.Message,Txt_This_course_requires_log_in_from_X_to_have_full_functionality_,
                   Cfg_EXTERNAL_LOGIN_URL,Cfg_EXTERNAL_LOGIN_SERVICE_SHORT_NAME,
                   Cfg_EXTERNAL_LOGIN_URL,Cfg_EXTERNAL_LOGIN_SERVICE_SHORT_NAME);
          Lay_ShowAlert (Lay_WARNING,Gbl.Message);
         }
  }

/*****************************************************************************/
/************** Show forms to change my role and to log out ******************/
/*****************************************************************************/

void Usr_ShowFormsRoleAndLogout (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Log_out;
   extern const char *Txt_You_are_LOGGED_as_X;
   extern const char *Txt_logged[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Role;

   /***** Link to log out *****/
   fprintf (Gbl.F.Out,"<div style=\"text-align:center; margin-bottom:20px;\">");
   Act_FormStart (ActLogOut);
   Act_LinkFormSubmit (Txt_Log_out,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("logout",Txt_Log_out,Txt_Log_out);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");

   /***** Write message with my new logged role *****/
   sprintf (Gbl.Message,Txt_You_are_LOGGED_as_X,
            Txt_logged[Gbl.Usrs.Me.UsrDat.Sex],
            Txt_ROLES_SINGUL_abc[Gbl.Usrs.Me.LoggedRole][Gbl.Usrs.Me.UsrDat.Sex]);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   /***** Put a form to change my role *****/
   if (Rol_GetNumAvailableRoles () > 1)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:center;\">"
	                 "%s: ",
               The_ClassFormul[Gbl.Prefs.Theme],Txt_Role);
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
/********* Get if a user has accepted to belong to current course ************/
/*****************************************************************************/

bool Usr_GetIfUserHasAcceptedEnrollmentInCurrentCrs (long UsrCod)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool Accepted = false;

   /***** Get if a user has accepted belonging to current course *****/
   sprintf (Query,"SELECT Accepted FROM crs_usr"
                  " WHERE CrsCod='%ld' AND UsrCod='%ld'",
            Gbl.CurrentCrs.Crs.CrsCod,UsrCod);
   if (DB_QuerySELECT (Query,&mysql_res,
                       "can not check if a user has accepted"
                       " the enrollment in the current course") == 1)
     {
      /* Get if accepted */
      row = mysql_fetch_row (mysql_res);
      Accepted = (Str_ConvertToUpperLetter (row[0][0]) == 'Y');
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Accepted;
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
               (unsigned) Gbl.CurrentTab,
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
   // WhatToSearch, LastAccNotif are set to default (0)
   sprintf (Query,"INSERT INTO usr_last (UsrCod,LastCrs,LastTab,LastTime)"
                  " VALUES ('%ld','%ld','%u',NOW())",
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Gbl.CurrentCrs.Crs.CrsCod,
            (unsigned) Gbl.CurrentTab);
   DB_QueryINSERT (Query,"can not insert last user's data");
  }
/*****************************************************************************/
/************* Write a row of a table with the data of a guest ***************/
/*****************************************************************************/

static void Usr_WriteRowGstMainData (unsigned NumUsr,struct UsrData *UsrDat)
  {
   const char *BgColor;
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   char MailLink[7+Cns_MAX_BYTES_STRING+1];                // mailto:mail_address
   struct Institution Ins;

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<tr>");

   /***** Checkbox to select user *****/
   // Two colors are used alternatively to better distinguish the rows
   BgColor = Gbl.ColorRows[Gbl.RowEvenOdd];
   fprintf (Gbl.F.Out,"<td style=\"text-align:center; vertical-align:middle;"
	              " background-color:%s;\">",
            BgColor);
   Usr_PutCheckboxToSelectUser (Rol_ROLE_GUEST__,UsrDat->EncryptedUsrCod,false);
   fprintf (Gbl.F.Out,"</td>");

   /***** Student has accepted enrollment in current course? *****/
   fprintf (Gbl.F.Out,"<td class=\"BM%d\">"
	              "<img src=\"%s/tr16x16.gif\" alt=\"\""
	              " class=\"ICON16x16\" />"
	              "</td>",
            Gbl.RowEvenOdd,
            Gbl.Prefs.IconsURL);

   /***** Write number of user in the list *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL\" style=\"text-align:right;"
	              " background-color:%s;\">"
	              "&nbsp;%u&nbsp;"
	              "</td>",
            BgColor,NumUsr);

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show student's photo *****/
      fprintf (Gbl.F.Out,"<td style=\"text-align:left; background-color:%s;\">",
               BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO18x24",Pho_ZOOM);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Prepare data for brief presentation *****/
   Usr_RestrictLengthMainData (true,UsrDat,MailLink);

   /****** Write user's IDs ******/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
	              " background-color:%s;\">",
            UsrDat->Accepted ? "DAT_SMALL_N" :
                               "DAT_SMALL",
            BgColor);
   ID_WriteUsrIDs (UsrDat,(Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER));
   fprintf (Gbl.F.Out,"</td>");

   /***** Write rest of main student's data *****/
   Ins.InsCod = UsrDat->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);
   Usr_WriteMainUsrDataExceptUsrID (UsrDat,BgColor,true,
                                    UsrDat->Email[0]  ? MailLink :
                                	                NULL,
                                    Ins.ShortName,
                                    Ins.WWW[0] ? Ins.WWW  :
                                	         NULL);

   /***** End row *****/
   fprintf (Gbl.F.Out,"</tr>");

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/************ Write a row of a table with the data of a student **************/
/*****************************************************************************/

void Usr_WriteRowStdMainData (unsigned NumUsr,struct UsrData *UsrDat,bool PutCheckBoxToSelectUsr)
  {
   extern const char *Txt_Enrollment_confirmed;
   extern const char *Txt_Enrollment_not_confirmed;
   const char *BgColor;
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   bool UsrIsTheMsgSender = false;
   char MailLink[7+Cns_MAX_BYTES_STRING+1];                // mailto:mail_address
   struct Institution Ins;
   bool ShowEmail = (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER && UsrDat->Accepted) ||
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADM ||
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM;

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<tr>");

   /***** Checkbox to select user *****/
   if (PutCheckBoxToSelectUsr)
      UsrIsTheMsgSender = (UsrDat->UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);
   // Two colors are used alternatively to better distinguish the rows
   BgColor = UsrIsTheMsgSender ? LIGHT_GREEN :
	                         Gbl.ColorRows[Gbl.RowEvenOdd];
   if (PutCheckBoxToSelectUsr)
     {
      fprintf (Gbl.F.Out,"<td style=\"text-align:center; vertical-align:middle;"
	                 " background-color:%s;\">",
               BgColor);
      Usr_PutCheckboxToSelectUser (Rol_ROLE_STUDENT,UsrDat->EncryptedUsrCod,UsrIsTheMsgSender);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Student has accepted enrollment in current course? *****/
   fprintf (Gbl.F.Out,"<td class=\"");
   if (UsrIsTheMsgSender)
      fprintf (Gbl.F.Out,"BM_SEL");
   else
      fprintf (Gbl.F.Out,"BM%d",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">"
	              "<img src=\"%s/%s16x16.gif\" alt=\"\" title=\"%s\""
	              " class=\"ICON16x16\" />"
	              "</td>",
            Gbl.Prefs.IconsURL,
            UsrDat->Accepted ? "ok_on" :
        	               "tr",
            UsrDat->Accepted ? Txt_Enrollment_confirmed :
                               Txt_Enrollment_not_confirmed);

   /***** Write number of student in the list *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:right;"
	              " background-color:%s;\">"
	              "&nbsp;%u&nbsp;"
	              "</td>",
            UsrDat->Accepted ? "DAT_SMALL_N" :
        	               "DAT_SMALL",
            BgColor,
            NumUsr);

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show student's photo *****/
      fprintf (Gbl.F.Out,"<td style=\"text-align:left; background-color:%s;\">",
               BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO18x24",Pho_ZOOM);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Prepare data for brief presentation *****/
   Usr_RestrictLengthMainData (ShowEmail,UsrDat,MailLink);

   /****** Write user's ID ******/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
	              " background-color:%s;\">",
            UsrDat->Accepted ? "DAT_SMALL_N" :
                               "DAT_SMALL",
            BgColor);
   ID_WriteUsrIDs (UsrDat,(Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER));
   fprintf (Gbl.F.Out,"</td>");

   /***** Write rest of main student's data *****/
   Ins.InsCod = UsrDat->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);
   Usr_WriteMainUsrDataExceptUsrID (UsrDat,BgColor,ShowEmail,
                                    UsrDat->Email[0]  ? MailLink :
                                	                NULL,
                                    Ins.ShortName,
                                    Ins.WWW[0] ? Ins.WWW :
                                	         NULL);

   /***** End row *****/
   fprintf (Gbl.F.Out,"</tr>");

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/*************** Write a row of a table with the data of a guest *************/
/*****************************************************************************/

static void Usr_WriteRowGstAllData (struct UsrData *UsrDat)
  {
   const char *BgColor;
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   struct Institution Ins;
   struct Centre Ctr;
   struct Department Dpt;

   /***** Start row *****/
   BgColor = Gbl.ColorRows[Gbl.RowEvenOdd];   // Two colors are used alternatively to better distinguish the rows
   fprintf (Gbl.F.Out,"<tr>");

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show guest's photo *****/
      fprintf (Gbl.F.Out,"<td style=\"text-align:left; background-color:%s;\">",
               BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO18x24",Pho_NO_ZOOM);
      fprintf (Gbl.F.Out,"</td>");
     }

   /****** Write user's ID ******/
   fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL\" style=\"text-align:left;"
	              " background-color:%s;\">",
            BgColor);
   ID_WriteUsrIDs (UsrDat,true);
   fprintf (Gbl.F.Out,"&nbsp;</td>");

   /***** Write rest of guest's main data *****/
   Ins.InsCod = UsrDat->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);
   Usr_WriteMainUsrDataExceptUsrID (UsrDat,BgColor,true,NULL,Ins.ShortName,NULL);

   /***** Write the rest of the data of the guest *****/
   if (UsrDat->Tch.CtrCod > 0)
     {
      Ctr.CtrCod = UsrDat->Tch.CtrCod;
      Ctr_GetDataOfCentreByCod (&Ctr);
     }
   Usr_WriteUsrData (BgColor,
                     UsrDat->Tch.CtrCod > 0 ? Ctr.FullName :
	                                      "&nbsp;",
	             NULL,true,false);
   if (UsrDat->Tch.DptCod > 0)
     {
      Dpt.DptCod = UsrDat->Tch.DptCod;
      Dpt_GetDataOfDepartmentByCod (&Dpt);
     }
   Usr_WriteUsrData (BgColor,
                     UsrDat->Tch.DptCod > 0 ? Dpt.FullName :
	                                      "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (BgColor,
                     UsrDat->Tch.Office[0] ? UsrDat->Tch.Office :
	                                     "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (BgColor,
                     UsrDat->Tch.OfficePhone[0] ? UsrDat->Tch.OfficePhone :
	                                          "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (BgColor,
                     UsrDat->LocalAddress[0] ? UsrDat->LocalAddress :
	                                       "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (BgColor,
                     UsrDat->LocalPhone[0] ? UsrDat->LocalPhone :
	                                     "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (BgColor,
                     UsrDat->FamilyAddress[0] ? UsrDat->FamilyAddress :
	                                        "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (BgColor,
                     UsrDat->FamilyPhone[0] ? UsrDat->FamilyPhone :
	                                      "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (BgColor,
                     UsrDat->OriginPlace[0] ? UsrDat->OriginPlace :
	                                      "&nbsp;",
	             NULL,true,false);
   Usr_WriteUsrData (BgColor,
                     UsrDat->StrBirthday[0] ? UsrDat->StrBirthday :
	                                      "&nbsp;",
	             NULL,true,false);

   /***** End row *****/
   fprintf (Gbl.F.Out,"</tr>");

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/************ Write a row of a table with the data of a student **************/
/*****************************************************************************/

void Usr_WriteRowStdAllData (struct UsrData *UsrDat,char *GroupNames)
  {
   const char *BgColor;
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   unsigned NumGrpTyp,NumField;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char Text[Cns_MAX_BYTES_TEXT+1];
   struct Institution Ins;
   bool ShowData = (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER && UsrDat->Accepted) ||
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADM ||
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM;

   /***** Start row *****/
   BgColor = Gbl.ColorRows[Gbl.RowEvenOdd];   // Two colors are used alternatively to better distinguish the rows
   fprintf (Gbl.F.Out,"<tr>");

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show student's photo *****/
      fprintf (Gbl.F.Out,"<td style=\"text-align:left; background-color:%s;\">",
               BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO18x24",Pho_NO_ZOOM);
      fprintf (Gbl.F.Out,"</td>");
     }

   /****** Write user's ID ******/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
	              " background-color:%s;\">",
            UsrDat->Accepted ? "DAT_SMALL_N" :
        	               "DAT_SMALL",
            BgColor);
   ID_WriteUsrIDs (UsrDat,(Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER));
   fprintf (Gbl.F.Out,"&nbsp;</td>");

   /***** Write rest of main student's data *****/
   Ins.InsCod = UsrDat->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);
   Usr_WriteMainUsrDataExceptUsrID (UsrDat,BgColor,ShowData,NULL,Ins.ShortName,NULL);

   /***** Write the rest of the data of the student *****/
   Usr_WriteUsrData (BgColor,
                     UsrDat->LocalAddress[0] ? (ShowData ? UsrDat->LocalAddress  :
	                                                   "********") :
	                                       "&nbsp;",
	             NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (BgColor,
                     UsrDat->LocalPhone[0] ? (ShowData ? UsrDat->LocalPhone    :
                	                                 "********") :
                	                     "&nbsp;",
	             NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (BgColor,
                     UsrDat->FamilyAddress[0] ? (ShowData ? UsrDat->FamilyAddress :
                	                                    "********") :
                	                        "&nbsp;",
	             NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (BgColor,
                     UsrDat->FamilyPhone[0] ? (ShowData ? UsrDat->FamilyPhone   :
                	                                  "********") :
                	                      "&nbsp;",
	             NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (BgColor,
                     UsrDat->OriginPlace[0] ? (ShowData ? UsrDat->OriginPlace   :
                	                                  "********") :
                	                      "&nbsp;",
	             NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (BgColor,
                     UsrDat->StrBirthday[0] ? (ShowData ? UsrDat->StrBirthday   :
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
            Usr_WriteUsrData (BgColor,GroupNames,NULL,true,UsrDat->Accepted);
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
         Usr_WriteUsrData (BgColor,Text,NULL,false,UsrDat->Accepted);

         /* Free structure that stores the query result */
         DB_FreeMySQLResult (&mysql_res);
        }
     }

   /***** End row *****/
   fprintf (Gbl.F.Out,"</tr>");

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/************* Write a row of a table with the data of a teacher *************/
/*****************************************************************************/

static void Usr_WriteRowTchMainData (unsigned NumUsr,struct UsrData *UsrDat,bool PutCheckBoxToSelectUsr)
  {
   extern const char *Txt_Enrollment_confirmed;
   extern const char *Txt_Enrollment_not_confirmed;
   const char *BgColor;
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   bool UsrIsTheMsgSender = false;
   char MailLink[7+Cns_MAX_BYTES_STRING+1];                // mailto:mail_address
   struct Institution Ins;
   bool ShowEmail = UsrDat->Accepted ||
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADM ||
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM;

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<tr>");

   /***** Checkbox to select user *****/
   if (PutCheckBoxToSelectUsr)
      UsrIsTheMsgSender = (UsrDat->UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);
   BgColor = UsrIsTheMsgSender ? LIGHT_GREEN :
	                         Gbl.ColorRows[Gbl.RowEvenOdd];   // Two colors are used alternatively to better distinguish the rows
   if (PutCheckBoxToSelectUsr)
     {
      fprintf (Gbl.F.Out,"<td style=\"text-align:center; vertical-align:middle;"
	                 " background-color:%s;\">",
               BgColor);
      Usr_PutCheckboxToSelectUser (Rol_ROLE_TEACHER,UsrDat->EncryptedUsrCod,UsrIsTheMsgSender);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Teacher has accepted enrollment in current course/in any course in degree/in any course? *****/
   fprintf (Gbl.F.Out,"<td class=\"");
   if (UsrIsTheMsgSender)
      fprintf (Gbl.F.Out,"BM_SEL");
   else
      fprintf (Gbl.F.Out,"BM%d",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">"
	              "<img src=\"%s/%s16x16.gif\""
	              " alt=\"\" title=\"%s\" class=\"ICON16x16\" />"
	              "</td>",
            Gbl.Prefs.IconsURL,
            UsrDat->Accepted ? "ok_on" :
        	               "tr",
            UsrDat->Accepted ? Txt_Enrollment_confirmed :
                               Txt_Enrollment_not_confirmed);

   /***** Write number of user *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL_N\" style=\"text-align:right;"
	              " background-color:%s;\">"
	              "&nbsp;%u&nbsp;"
	              "</td>",
            BgColor,NumUsr);

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show teacher's photo *****/
      fprintf (Gbl.F.Out,"<td style=\"text-align:left; background-color:%s;\">",
               BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO18x24",Pho_ZOOM);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Prepare data for brief presentation *****/
   Usr_RestrictLengthMainData (ShowEmail,UsrDat,MailLink);

   /****** Write the user's ID ******/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
	              " background-color:%s;\">",
            UsrDat->Accepted ? "DAT_SMALL_N" :
                               "DAT_SMALL",
            BgColor);
   ID_WriteUsrIDs (UsrDat,ID_ICanSeeTeacherID (UsrDat));
   fprintf (Gbl.F.Out,"&nbsp;");
   fprintf (Gbl.F.Out,"</td>");

   /***** Write rest of main teacher's data *****/
   Ins.InsCod = UsrDat->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);
   Usr_WriteMainUsrDataExceptUsrID (UsrDat,BgColor,ShowEmail,
                                    UsrDat->Email[0] ? MailLink :
                                	               NULL,
                                    Ins.ShortName,
                                    Ins.WWW[0] ? Ins.WWW :
                                	         NULL);
   fprintf (Gbl.F.Out,"</tr>");

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/*** Write a row of a table with the data of a teacher or an administrator ***/
/*****************************************************************************/

void Usr_WriteRowTchAllData (struct UsrData *UsrDat)
  {
   const char *BgColor;
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   struct Institution Ins;
   bool ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == UsrDat->UsrCod);
   bool ShowData = (ItsMe || UsrDat->Accepted ||
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADM ||
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM);
   struct Centre Ctr;
   struct Department Dpt;

   /***** Start row *****/
   BgColor = Gbl.ColorRows[Gbl.RowEvenOdd];   // Two colors are used alternatively to better distinguish the rows
   fprintf (Gbl.F.Out,"<tr>");
   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show teacher's photo *****/
      fprintf (Gbl.F.Out,"<td style=\"text-align:left; background-color:%s;\">",
               BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO18x24",Pho_NO_ZOOM);
      fprintf (Gbl.F.Out,"</td>");
     }

   /****** Write the user's ID ******/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
	              " background-color:%s;\">",
            UsrDat->Accepted ? "DAT_SMALL_N" :
                               "DAT_SMALL",
            BgColor);
   ID_WriteUsrIDs (UsrDat,ID_ICanSeeTeacherID (UsrDat));
   fprintf (Gbl.F.Out,"&nbsp;</td>");

   /***** Write rest of main teacher's data *****/
   Ins.InsCod = UsrDat->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);
   Usr_WriteMainUsrDataExceptUsrID (UsrDat,BgColor,ShowData,NULL,Ins.ShortName,NULL);

   /***** Write the rest of teacher's data *****/
   if (ShowData && UsrDat->Tch.CtrCod > 0)
     {
      Ctr.CtrCod = UsrDat->Tch.CtrCod;
      Ctr_GetDataOfCentreByCod (&Ctr);
     }
   Usr_WriteUsrData (BgColor,
                     (ShowData && UsrDat->Tch.CtrCod > 0) ? Ctr.FullName :
                	                                    "&nbsp;",
                     NULL,true,UsrDat->Accepted);
   if (ShowData && UsrDat->Tch.DptCod > 0)
     {
      Dpt.DptCod = UsrDat->Tch.DptCod;
      Dpt_GetDataOfDepartmentByCod (&Dpt);
     }
   Usr_WriteUsrData (BgColor,
                     (ShowData && UsrDat->Tch.DptCod > 0) ? Dpt.FullName :
                	                                    "&nbsp;",
                     NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (BgColor,
                     (ShowData && UsrDat->Tch.Office[0]) ? UsrDat->Tch.Office :
                	                                   "&nbsp;",
                     NULL,true,UsrDat->Accepted);
   Usr_WriteUsrData (BgColor,
                     (ShowData && UsrDat->Tch.OfficePhone[0]) ? UsrDat->Tch.OfficePhone :
                	                                        "&nbsp;",
                     NULL,true,UsrDat->Accepted);

   fprintf (Gbl.F.Out,"</tr>");

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/*** Write a row of a table with the data of a teacher or an administrator ***/
/*****************************************************************************/

void Usr_WriteRowAdmData (unsigned NumUsr,struct UsrData *UsrDat)
  {
   const char *BgColor;
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;
   char MailLink[7+Cns_MAX_BYTES_STRING+1];                // mailto:mail_address
   struct Institution Ins;

   /***** Start row *****/
   BgColor = Gbl.ColorRows[Gbl.RowEvenOdd];   // Two colors are used alternatively to better distinguish the rows
   fprintf (Gbl.F.Out,"<tr>");

   /***** Write number of user *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL_N\" style=\"text-align:right;"
	              " background-color:%s;\">"
	              "&nbsp;%u&nbsp;"
	              "</td>",
            BgColor,NumUsr);

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show administrator's photo *****/
      fprintf (Gbl.F.Out,"<td style=\"text-align:left; background-color:%s;\">",
               BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO18x24",Pho_ZOOM);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Prepare data for brief presentation *****/
   Usr_RestrictLengthMainData (true,UsrDat,MailLink);

   /****** Write the user's ID ******/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
	              " background-color:%s;\">",
            UsrDat->Accepted ? "DAT_SMALL_N" :
                               "DAT_SMALL",
            BgColor);
   ID_WriteUsrIDs (UsrDat,(Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM));
   fprintf (Gbl.F.Out,"&nbsp;</td>");

   /***** Write rest of main administrator's data *****/
   Ins.InsCod = UsrDat->InsCod;
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);
   Usr_WriteMainUsrDataExceptUsrID (UsrDat,BgColor,true,
                                    UsrDat->Email[0] ? MailLink :
                                	               NULL,
                                    Ins.ShortName,
                                    Ins.WWW[0] ? Ins.WWW :
                                	         NULL);
   fprintf (Gbl.F.Out,"</tr>");

   /***** Write degrees which are administrated by this administrator *****/
   Deg_GetAndWriteDegreesAdminBy (UsrDat->UsrCod,
                                  Gbl.Usrs.Listing.WithPhotos ? Usr_NUM_MAIN_FIELDS_DATA_ADM :
                                	                        Usr_NUM_MAIN_FIELDS_DATA_ADM-1);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/************** Restrict the length of the main data of a user ***************/
/*****************************************************************************/

void Usr_RestrictLengthMainData (bool ShowData,struct UsrData *UsrDat,char *MailLink)
  {
   Str_LimitLengthHTMLStr (UsrDat->FirstName,10);
   Str_LimitLengthHTMLStr (UsrDat->Surname1,10);
   Str_LimitLengthHTMLStr (UsrDat->Surname2,10);
   if (ShowData && UsrDat->Email[0])
     {
      sprintf (MailLink,"mailto:%s",UsrDat->Email);
      Str_LimitLengthHTMLStr (UsrDat->Email,10);
     }
  }

/*****************************************************************************/
/************************* Write main data of a user *************************/
/*****************************************************************************/

static void Usr_WriteMainUsrDataExceptUsrID (struct UsrData *UsrDat,const char *BgColor,bool ShowEmail,
                                             const char *MailLink,
                                             const char *InstitutionName,const char *InstitutionLink)
  {
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

static void Usr_WriteUsrData (const char *BgColor,const char *Data,const char *Link,bool NonBreak,bool Accepted)
  {
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
	              " background-color:%s;\">",
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

   /***** Get the number of users in a degree from database ******/
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

unsigned Usr_GetNumUsrsInCrssOfCtr (Rol_Role_t Role,long CtrCod)
  {
   char Query[512];

   /***** Get the number of users in a degree from database ******/
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

unsigned Usr_GetNumUsrsInCrssOfIns (Rol_Role_t Role,long InsCod)
  {
   char Query[512];

   /***** Get the number of users in a degree from database ******/
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

unsigned Usr_GetNumUsrsInCrssOfCty (Rol_Role_t Role,long CtyCod)
  {
   char Query[512];

   /***** Get the number of users in a degree from database ******/
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
/*********************** Get number of users in a country ********************/
/*****************************************************************************/

unsigned Usr_GetNumUsrsInCountry (Rol_Role_t Role,long CtyCod)
  {
   char Query[256];

   /***** Get the number of users (with a role) in a country from database ******/
   if (Role == Rol_ROLE_UNKNOWN)        // Here Rol_ROLE_UNKNOWN means "all users"
      sprintf (Query,"SELECT COUNT(*) FROM usr_data"
                     " WHERE CtyCod='%ld'",CtyCod);
   else
      sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
	             " FROM usr_data,crs_usr"
                     " WHERE usr_data.CtyCod='%ld'"
                     " AND usr_data.UsrCod=crs_usr.UsrCod AND crs_usr.Role='%u'",
               CtyCod,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of users in a country");
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
            CrsCod,(unsigned) Rol_ROLE_STUDENT);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get a random student from the current course"))
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
            GrpCod,(unsigned) Rol_ROLE_STUDENT);
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
            Gbl.CurrentIns.Ins.InsCod,DptCod,(unsigned) Rol_ROLE_TEACHER);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of teachers in a department");
  }

/*****************************************************************************/
/******************* Get number of users in a institution ********************/
/*****************************************************************************/

unsigned Usr_GetNumberOfUsersInInstitution (long InsCod,Rol_Role_t Role)
  {
   char Query[256];

   /***** Get the number of users in an institution from database *****/
   // The following query is very slow, so call this function as minimum as possible
   sprintf (Query,"SELECT COUNT(DISTINCT usr_data.UsrCod)"
	          " FROM usr_data,crs_usr"
                  " WHERE usr_data.InsCod='%ld'"
                  " AND usr_data.UsrCod=crs_usr.UsrCod AND crs_usr.Role='%u'",
            InsCod,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of users in an institution");
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
            CtrCod,(unsigned) Rol_ROLE_TEACHER);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of teachers in a centre");
  }

/*****************************************************************************/
/******* Build query to get list with data of users in current course ********/
/*****************************************************************************/

static void Usr_BuildQueryToGetUsrsLstCrs (Rol_Role_t Role,const char *UsrQuery,bool Search,char *Query)
  {
   unsigned NumPositiveCods = 0;
   unsigned NumNegativeCods = 0;
   char LongStr[1+10+1];
   unsigned NumGrpSel;
   long GrpCod;
   unsigned NumGrpTyp;
   bool *AddStdsWithoutGroupOf;

   /***** If there are no groups selected, don't do anything *****/
   if (Role == Rol_ROLE_STUDENT &&
       (!Gbl.Usrs.ClassPhoto.AllGroups &&
        !Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps))
     {
      Query[0] = '\0';
      return;
     }

   /***** Create query for users in the course *****/
   if (Gbl.CurrentAct == ActReqMsgUsr)        // Selecting users to write a message
      sprintf (Query,"SELECT crs_usr.UsrCod,crs_usr.Accepted,usr_data.Sex"
                     " FROM crs_usr,usr_data"
                     " WHERE crs_usr.CrsCod='%ld'"
                     " AND crs_usr.Role='%u'"
                     " AND crs_usr.UsrCod NOT IN"
                     " (SELECT ToUsrCod FROM msg_banned WHERE FromUsrCod='%ld')"
      		     " AND crs_usr.UsrCod=usr_data.UsrCod",        // Do not get banned users
               Gbl.CurrentCrs.Crs.CrsCod,(unsigned) Role,
               Gbl.Usrs.Me.UsrDat.UsrCod);
   else if (Search)
      sprintf (Query,"SELECT crs_usr.UsrCod,crs_usr.Accepted,usr_data.Sex"
		     " FROM crs_usr,usr_data"
		     " WHERE crs_usr.CrsCod='%ld'"
		     " AND crs_usr.Role='%u'"
		     " AND crs_usr.UsrCod=usr_data.UsrCod"
		     " AND %s",
	       Gbl.CurrentCrs.Crs.CrsCod,(unsigned) Role,
	       UsrQuery);
   else
      sprintf (Query,"SELECT crs_usr.UsrCod,crs_usr.Accepted,usr_data.Sex"
                     " FROM crs_usr,usr_data"
                     " WHERE crs_usr.CrsCod='%ld'"
                     " AND crs_usr.Role='%u'"
		     " AND crs_usr.UsrCod=usr_data.UsrCod",
               Gbl.CurrentCrs.Crs.CrsCod,(unsigned) Role);

   /***** Select users in selected groups (only for students) *****/
   if (Role == Rol_ROLE_STUDENT && !Gbl.Usrs.ClassPhoto.AllGroups)
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
   strcat (Query," ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName,usr_data.UsrCod");
  }

/*****************************************************************************/
/************************* Get list with data of users ***********************/
/*****************************************************************************/

#define Usr_MAX_LENGTH_QUERY_LIST_USERS (16*1024)

void Usr_GetUsrsLst (Rol_Role_t Role,Sco_Scope_t Scope,const char *UsrQuery,bool Search)
  {
   char Query[Usr_MAX_LENGTH_QUERY_LIST_USERS+1];

   /***** Build query *****/
   if (Search && Role == Rol_ROLE_GUEST__)	// Special case
     {
      /* Select users with no courses */
      sprintf (Query,"SELECT UsrCod,'N',Sex"
                     " FROM usr_data"
		     " WHERE %s"
		     " AND UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
		     " ORDER BY Surname1,Surname2,FirstName,UsrCod",
	       UsrQuery);
     }
   else						// Rest of cases
      switch (Scope)
	{
	 case Sco_SCOPE_SYS:
	    if (Search)
	       /* Select users with any of their courses not accepted +
			 users with all their courses accepted */
	       sprintf (Query,"SELECT list_usrs.UsrCod,list_usrs.Accepted,usr_data.Sex FROM "
			      "(SELECT DISTINCT UsrCod,'N' AS Accepted"
			      " FROM crs_usr"
			      " WHERE Role='%u' AND Accepted='N'"
			      " AND UsrCod IN (SELECT UsrCod FROM usr_data WHERE %s)"
			      " UNION "
			      "SELECT DISTINCT UsrCod,'Y' AS Accepted"
			      " FROM crs_usr"
			      " WHERE Role='%u'"
			      " AND UsrCod IN "
			      "(SELECT UsrCod FROM usr_data WHERE %s)"
			      " AND UsrCod NOT IN "
			      "(SELECT DISTINCT UsrCod FROM crs_usr"
			      " WHERE Role='%u' AND Accepted='N'"
			      " AND UsrCod IN "
			      " (SELECT UsrCod FROM usr_data WHERE %s))"
			      ") AS list_usrs,usr_data"
			      " WHERE list_usrs.UsrCod=usr_data.UsrCod "
			      " ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName,usr_data.UsrCod",
			(unsigned) Role,UsrQuery,
			(unsigned) Role,UsrQuery,
			(unsigned) Role,UsrQuery);
	    else
	       /* Select users with any of their courses not accepted +
			 users with all their courses accepted */
	       sprintf (Query,"SELECT list_usrs.UsrCod,list_usrs.Accepted,usr_data.Sex FROM "
			      "(SELECT DISTINCT UsrCod,'N' AS Accepted"
			      " FROM crs_usr"
			      " WHERE Role='%u' AND Accepted='N'"
			      " UNION "
			      "SELECT DISTINCT UsrCod,'Y' AS Accepted"
			      " FROM crs_usr"
			      " WHERE Role='%u' AND UsrCod NOT IN "
			      "(SELECT DISTINCT UsrCod FROM crs_usr"
			      " WHERE Role='%u' AND Accepted='N')"
			      ") AS list_usrs,usr_data"
			      " WHERE list_usrs.UsrCod=usr_data.UsrCod "
			      " ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName,usr_data.UsrCod",
			(unsigned) Role,
			(unsigned) Role,
			(unsigned) Role);
	    break;
	 case Sco_SCOPE_CTY:
	    if (Search)
	       /* Select users of degrees in current country with any courses in those degrees not accepted +
			 users of degrees in current country with all their courses in those degrees accepted */
	       sprintf (Query,"SELECT list_usrs.UsrCod,list_usrs.Accepted,usr_data.Sex FROM "
			      "(SELECT DISTINCT crs_usr.UsrCod,'N' AS Accepted"
			      " FROM institutions,centres,degrees,courses,crs_usr,(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users"
			      " WHERE institutions.CtyCod='%ld'"
			      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=candidate_users.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N'"
			      " UNION "
			      "SELECT DISTINCT crs_usr.UsrCod,'Y' AS Accepted"
			      " FROM institutions,centres,degrees,courses,crs_usr,(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users"
			      " WHERE institutions.CtyCod='%ld'"
			      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=candidate_users.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod NOT IN "
			      "(SELECT DISTINCT crs_usr.UsrCod"
			      " FROM institutions,centres,degrees,courses,crs_usr,(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users"
			      " WHERE institutions.CtyCod='%ld'"
			      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=candidate_users.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N')"
			      ") AS list_usrs,usr_data"
			      " WHERE list_usrs.UsrCod=usr_data.UsrCod "
			      " ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName,usr_data.UsrCod",
			UsrQuery,Gbl.CurrentCty.Cty.CtyCod,(unsigned) Role,
			UsrQuery,Gbl.CurrentCty.Cty.CtyCod,(unsigned) Role,
			UsrQuery,Gbl.CurrentCty.Cty.CtyCod,(unsigned) Role);
	    else
	       /* Select users of degrees in current country with any courses in those degrees not accepted +
			 users of degrees in current country with all their courses in those degrees accepted */
	       sprintf (Query,"SELECT list_usrs.UsrCod,list_usrs.Accepted,usr_data.Sex FROM "
			      "(SELECT DISTINCT crs_usr.UsrCod,'N' AS Accepted"
			      " FROM institutions,centres,degrees,courses,crs_usr"
			      " WHERE institutions.CtyCod='%ld'"
			      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N'"
			      " UNION "
			      "SELECT DISTINCT crs_usr.UsrCod,'Y' AS Accepted"
			      " FROM institutions,centres,degrees,courses,crs_usr"
			      " WHERE institutions.CtyCod='%ld'"
			      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod NOT IN "
			      "(SELECT DISTINCT crs_usr.UsrCod"
			      " FROM institutions,centres,degrees,courses,crs_usr"
			      " WHERE institutions.CtyCod='%ld'"
			      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N')"
			      ") AS list_usrs,usr_data"
			      " WHERE list_usrs.UsrCod=usr_data.UsrCod "
			      " ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName,usr_data.UsrCod",
			Gbl.CurrentCty.Cty.CtyCod,(unsigned) Role,
			Gbl.CurrentCty.Cty.CtyCod,(unsigned) Role,
			Gbl.CurrentCty.Cty.CtyCod,(unsigned) Role);
	    break;
	 case Sco_SCOPE_INS:
	    if (Search)
	       /* Select users of degrees in current institution with any courses in those degrees not accepted +
			 users of degrees in current institution with all their courses in those degrees accepted */
	       sprintf (Query,"SELECT list_usrs.UsrCod,list_usrs.Accepted,usr_data.Sex FROM "
			      "(SELECT DISTINCT crs_usr.UsrCod,'N' AS Accepted"
			      " FROM centres,degrees,courses,crs_usr,(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users"
			      " WHERE centres.InsCod='%ld'"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=candidate_users.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N'"
			      " UNION "
			      "SELECT DISTINCT crs_usr.UsrCod,'Y' AS Accepted"
			      " FROM centres,degrees,courses,crs_usr,(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users"
			      " WHERE centres.InsCod='%ld'"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=candidate_users.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod NOT IN "
			      "(SELECT DISTINCT crs_usr.UsrCod"
			      " FROM centres,degrees,courses,crs_usr,(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users"
			      " WHERE centres.InsCod='%ld'"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=candidate_users.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N')"
			      ") AS list_usrs,usr_data"
			      " WHERE list_usrs.UsrCod=usr_data.UsrCod "
			      " ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName,usr_data.UsrCod",
			UsrQuery,Gbl.CurrentIns.Ins.InsCod,(unsigned) Role,
			UsrQuery,Gbl.CurrentIns.Ins.InsCod,(unsigned) Role,
			UsrQuery,Gbl.CurrentIns.Ins.InsCod,(unsigned) Role);
	    else
	       /* Select users of degrees in current institution with any courses in those degrees not accepted +
			 users of degrees in current institution with all their courses in those degrees accepted */
	       sprintf (Query,"SELECT list_usrs.UsrCod,list_usrs.Accepted,usr_data.Sex FROM "
			      "(SELECT DISTINCT crs_usr.UsrCod,'N' AS Accepted"
			      " FROM centres,degrees,courses,crs_usr"
			      " WHERE centres.InsCod='%ld'"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N'"
			      " UNION "
			      "SELECT DISTINCT crs_usr.UsrCod,'Y' AS Accepted"
			      " FROM centres,degrees,courses,crs_usr"
			      " WHERE centres.InsCod='%ld'"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod NOT IN "
			      "(SELECT DISTINCT crs_usr.UsrCod"
			      " FROM centres,degrees,courses,crs_usr"
			      " WHERE centres.InsCod='%ld'"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N')"
			      ") AS list_usrs,usr_data"
			      " WHERE list_usrs.UsrCod=usr_data.UsrCod "
			      " ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName,usr_data.UsrCod",
			Gbl.CurrentIns.Ins.InsCod,(unsigned) Role,
			Gbl.CurrentIns.Ins.InsCod,(unsigned) Role,
			Gbl.CurrentIns.Ins.InsCod,(unsigned) Role);
	    break;
	 case Sco_SCOPE_CTR:
	    if (Search)
	       /* Select users of degrees in current centre with any courses in those degrees not accepted +
			 users of degrees in current centre with all their courses in those degrees accepted */
	       sprintf (Query,"SELECT list_usrs.UsrCod,list_usrs.Accepted,usr_data.Sex FROM "
			      "(SELECT DISTINCT crs_usr.UsrCod,'N' AS Accepted"
			      " FROM degrees,courses,crs_usr,(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users"
			      " WHERE degrees.CtrCod='%ld'"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=candidate_users.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N'"
			      " UNION "
			      "SELECT DISTINCT crs_usr.UsrCod,'Y' AS Accepted"
			      " FROM degrees,courses,crs_usr,(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users"
			      " WHERE degrees.CtrCod='%ld'"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=candidate_users.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod NOT IN "
			      "(SELECT DISTINCT crs_usr.UsrCod"
			      " FROM degrees,courses,crs_usr,(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users"
			      " WHERE degrees.CtrCod='%ld'"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=candidate_users.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N')"
			      ") AS list_usrs,usr_data"
			      " WHERE list_usrs.UsrCod=usr_data.UsrCod "
			      " ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName,usr_data.UsrCod",
			UsrQuery,Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) Role,
			UsrQuery,Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) Role,
			UsrQuery,Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) Role);
	    else
	       /* Select users of degrees in current centre with any courses in those degrees not accepted +
			 users of degrees in current centre with all their courses in those degrees accepted */
	       sprintf (Query,"SELECT list_usrs.UsrCod,list_usrs.Accepted,usr_data.Sex FROM "
			      "(SELECT DISTINCT crs_usr.UsrCod,'N' AS Accepted"
			      " FROM degrees,courses,crs_usr"
			      " WHERE degrees.CtrCod='%ld'"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N'"
			      " UNION "
			      "SELECT DISTINCT crs_usr.UsrCod,'Y' AS Accepted"
			      " FROM degrees,courses,crs_usr"
			      " WHERE degrees.CtrCod='%ld'"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod NOT IN "
			      "(SELECT DISTINCT crs_usr.UsrCod"
			      " FROM degrees,courses,crs_usr"
			      " WHERE degrees.CtrCod='%ld'"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N')"
			      ") AS list_usrs,usr_data"
			      " WHERE list_usrs.UsrCod=usr_data.UsrCod "
			      " ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName,usr_data.UsrCod",
			Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) Role,
			Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) Role,
			Gbl.CurrentCtr.Ctr.CtrCod,(unsigned) Role);
	    break;
	 case Sco_SCOPE_DEG:
	    if (Search)
	       /* Select users of current degree with any courses in current degree not accepted +
			 users of current degree with all their courses in current degree accepted */
	       sprintf (Query,"SELECT list_usrs.UsrCod,list_usrs.Accepted,usr_data.Sex FROM "
			      "(SELECT DISTINCT crs_usr.UsrCod,'N' AS Accepted"
			      " FROM courses,crs_usr,(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users"
			      " WHERE courses.DegCod='%ld'"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=candidate_users.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N'"
			      " UNION "
			      "SELECT DISTINCT crs_usr.UsrCod,'Y' AS Accepted"
			      " FROM courses,crs_usr,(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users"
			      " WHERE courses.DegCod='%ld'"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=candidate_users.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod NOT IN "
			      "(SELECT DISTINCT crs_usr.UsrCod"
			      " FROM courses,crs_usr,(SELECT UsrCod FROM usr_data WHERE %s) AS candidate_users"
			      " WHERE courses.DegCod='%ld'"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.UsrCod=candidate_users.UsrCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N')"
			      ") AS list_usrs,usr_data"
			      " WHERE list_usrs.UsrCod=usr_data.UsrCod "
			      " ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName,usr_data.UsrCod",
			UsrQuery,Gbl.CurrentDeg.Deg.DegCod,(unsigned) Role,
			UsrQuery,Gbl.CurrentDeg.Deg.DegCod,(unsigned) Role,
			UsrQuery,Gbl.CurrentDeg.Deg.DegCod,(unsigned) Role);
	    else
	       /* Select users of current degree with any courses in current degree not accepted +
			 users of current degree with all their courses in current degree accepted */
	       sprintf (Query,"SELECT list_usrs.UsrCod,list_usrs.Accepted,usr_data.Sex FROM "
			      "(SELECT DISTINCT crs_usr.UsrCod,'N' AS Accepted FROM courses,crs_usr"
			      " WHERE courses.DegCod='%ld'"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N'"
			      " UNION "
			      "SELECT DISTINCT crs_usr.UsrCod,'Y' AS Accepted FROM courses,crs_usr"
			      " WHERE courses.DegCod='%ld'"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.UsrCod NOT IN "
			      "(SELECT DISTINCT crs_usr.UsrCod FROM courses,crs_usr"
			      " WHERE courses.DegCod='%ld'"
			      " AND courses.CrsCod=crs_usr.CrsCod"
			      " AND crs_usr.Role='%u'"
			      " AND crs_usr.Accepted='N')"
			      ") AS list_usrs,usr_data"
			      " WHERE list_usrs.UsrCod=usr_data.UsrCod "
			      " ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName,usr_data.UsrCod",
			Gbl.CurrentDeg.Deg.DegCod,(unsigned) Role,
			Gbl.CurrentDeg.Deg.DegCod,(unsigned) Role,
			Gbl.CurrentDeg.Deg.DegCod,(unsigned) Role);
	    break;
	 case Sco_SCOPE_CRS:
	    Usr_BuildQueryToGetUsrsLstCrs (Role,UsrQuery,Search,Query);
	    break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
        }
/*
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
      Lay_ShowAlert (Lay_INFO,Query);
*/
   /***** Get list of users from database *****/
   Usr_GetListUsrs (Query,
                    ( Role == Rol_ROLE_TEACHER ? &Gbl.Usrs.LstTchs :
		     (Role == Rol_ROLE_STUDENT ? &Gbl.Usrs.LstStds :
		                        	 &Gbl.Usrs.LstGsts)));
  }

/*****************************************************************************/
/******************** Get list with data of administrators *******************/
/*****************************************************************************/

static void Usr_GetAdmsLst (Sco_Scope_t Scope)
  {
   char Query[512];

   /***** Build query *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         strcpy (Query,"SELECT DISTINCT admin.UsrCod,'Y',usr_data.Sex"
                       " FROM admin,usr_data"
                       " WHERE (admin.Scope='Deg'"
                       " OR admin.Scope='Sys')"
                       " AND admin.UsrCod=usr_data.UsrCod "
                       " ORDER BY usr_data.Surname1,usr_data.Surname2,"
                       "usr_data.FirstName,usr_data.UsrCod");
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT DISTINCT admin.UsrCod,'Y',usr_data.Sex"
                        " FROM centres,degrees,admin,usr_data"
                        " WHERE ((centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=admin.Cod"
                        " AND admin.Scope='Deg')"
                        " OR admin.Scope='Sys')"
                        " AND admin.UsrCod=usr_data.UsrCod "
                        " ORDER BY usr_data.Surname1,usr_data.Surname2,"
                        "usr_data.FirstName,usr_data.UsrCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT DISTINCT admin.UsrCod,'Y',usr_data.Sex"
                        " FROM degrees,admin,usr_data"
                        " WHERE ((degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=admin.Cod"
                        " AND admin.Scope='Deg')"
                        " OR admin.Scope='Sys')"
                        " AND admin.UsrCod=usr_data.UsrCod "
                        " ORDER BY usr_data.Surname1,usr_data.Surname2,"
                        "usr_data.FirstName,usr_data.UsrCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT DISTINCT admin.UsrCod,'Y',usr_data.Sex"
                        " FROM admin,usr_data"
                        " WHERE ((admin.Scope='Deg' AND admin.Cod='%ld')"
                        " OR admin.Scope='Sys')"
                        " AND admin.UsrCod=usr_data.UsrCod "
                        " ORDER BY usr_data.Surname1,usr_data.Surname2,"
                        "usr_data.FirstName,usr_data.UsrCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      default:        // not aplicable
         return;
     }

   /***** Get list of students from database *****/
   Usr_GetListUsrs (Query,&Gbl.Usrs.LstAdms);
  }

/*****************************************************************************/
/************************ Get list with data of guests ***********************/
/*****************************************************************************/

static void Usr_GetGstsLst (Sco_Scope_t Scope)
  {
   char Query[512];

   /***** Build query *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         strcpy (Query,"SELECT DISTINCT UsrCod,'N',Sex"
                       " FROM usr_data"
                       " WHERE UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
                       " ORDER BY Surname1,Surname2,FirstName,UsrCod");
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT DISTINCT UsrCod,'Y',Sex"
                        " FROM usr_data"
                        " WHERE (CtyCod='%ld' OR InsCtyCod='%ld')"
                        " AND UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
                        " ORDER BY Surname1,Surname2,FirstName,UsrCod",
                  Gbl.CurrentCty.Cty.CtyCod,
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT DISTINCT UsrCod,'Y',Sex"
                        " FROM usr_data"
                        " WHERE InsCod='%ld'"
                        " AND UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
                        " ORDER BY Surname1,Surname2,FirstName,UsrCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT DISTINCT UsrCod,'Y',Sex"
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
   Usr_GetListUsrs (Query,&Gbl.Usrs.LstGsts);
  }

/*****************************************************************************/
/*********** Get the user's codes of all the students of a degree ************/
/*****************************************************************************/

void Usr_GetUnorderedStdsCodesInDeg (long DegCod)
  {
   char Query[512];

   Gbl.Usrs.LstStds.NumUsrs = 0;

   if (Usr_GetNumUsrsInCrssOfDeg (Rol_ROLE_STUDENT,DegCod))
     {
      /***** Get the students in a degree from database *****/
      sprintf (Query,"SELECT DISTINCT crs_usr.UsrCod,'N',usr_data.Sex"
	             " FROM courses,crs_usr,usr_data"
                     " WHERE courses.DegCod='%ld' AND courses.CrsCod=crs_usr.CrsCod AND crs_usr.Role='%u'"
                     " AND crs_usr.UsrCod=usr_data.UsrCod",
               DegCod,(unsigned) Rol_ROLE_STUDENT);

      /***** Get list of students from database *****/
      Usr_GetListUsrs (Query,&Gbl.Usrs.LstStds);
     }
  }

/*****************************************************************************/
/********************** Get list of users from database **********************/
/*****************************************************************************/

static void Usr_GetListUsrs (const char *Query,struct ListUsers *LstUsrs)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsr;
   bool Abort = false;

   if (!Query[0])
     {
      LstUsrs->NumUsrs = 0;
      return;
     }

   /***** Query database *****/
   if ((LstUsrs->NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get list of users")))
     {
      if (LstUsrs->NumUsrs > Cfg_MAX_USRS_IN_LIST)
        {
         Usr_ShowWarningListIsTooBig (LstUsrs->NumUsrs);
         Abort = true;
        }
      else
        {
         /***** Allocate memory for the list of users *****/
         Usr_AllocateUsrsList (LstUsrs);

         for (NumUsr = 0;
              NumUsr < LstUsrs->NumUsrs;
              NumUsr++)
           {
            /* Get next student */
            row = mysql_fetch_row (mysql_res);

            /* Get user code */
            LstUsrs->Lst[NumUsr].UsrCod = Str_ConvertStrCodToLongCod (row[0]);

            /* Get if user has accepted enrollment in current course */
            LstUsrs->Lst[NumUsr].Accepted = (Str_ConvertToUpperLetter (row[1][0]) == 'Y');

            /* Get user's sex */
            LstUsrs->Lst[NumUsr].Sex = Usr_GetSexFromStr (row[2]);

            /* By default, users are not removed */
            LstUsrs->Lst[NumUsr].Remove = false;
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

static void Usr_AllocateUsrsList (struct ListUsers *LstUsrs)
  {
/*
if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
   {
    sprintf (Gbl.Message,"Memory used by list = %lu",(long) sizeof (struct UsrInList) * NumUsrs);
    Lay_ShowAlert (Lay_INFO,Gbl.Message);
   }
*/
   if (LstUsrs->NumUsrs)
      if ((LstUsrs->Lst = (struct UsrInList *) calloc (LstUsrs->NumUsrs,sizeof (struct UsrInList))) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store users list.");
  }

/*****************************************************************************/
/********************* Free space used for list of users *********************/
/*****************************************************************************/

void Usr_FreeUsrsList (struct ListUsers *LstUsrs)
  {
   if (LstUsrs->NumUsrs)
     {
      /***** Free the list itself *****/
      if (LstUsrs->Lst)
        {
         free ((void *) LstUsrs->Lst);
         LstUsrs->Lst = NULL;
        }

      /***** Reset number of users *****/
      LstUsrs->NumUsrs = 0;
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

   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");

   /***** Show warning *****/
   sprintf (Gbl.Message,Txt_The_list_of_X_users_is_too_large_to_be_displayed,
            NumUsrs);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);

   /***** Put form to confirm that I want to see the big list *****/
   Act_FormStart (Gbl.CurrentAct);
   Grp_PutParamsCodGrps ();
   Usr_PutParamUsrListType (Gbl.Usrs.Me.ListType);
   Usr_PutParamColsClassPhoto ();
   Usr_PutParamListWithPhotos ();
   Usr_PutExtraParamsUsrList (Gbl.CurrentAct);
   Par_PutHiddenParamChar ("ShowBigList",'Y');

   /***** Send button *****/
   Lay_PutSendButton (Txt_Show_anyway);
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

void Usr_GetListSelectedUsrs (void)
  {
   unsigned Length;

   /***** Allocate memory for the lists of users *****/
   Usr_AllocateListEncryptedUsrCodAll ();
   Usr_AllocateListEncryptedUsrCodTch ();
   Usr_AllocateListEncryptedUsrCodStd ();

   /***** Get selected users *****/
   if (Gbl.Session.IsOpen)	// If the session is open, get parameter from DB
     {
      Ses_GetHiddenParFromDB (Gbl.CurrentAct,"UsrCodAll",
                              Gbl.Usrs.Select.All,Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);
      Str_ChangeFormat (Str_FROM_FORM,Str_TO_TEXT,
			Gbl.Usrs.Select.All,Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS,true);
     }
   else
      Par_GetParMultiToText ("UsrCodAll",Gbl.Usrs.Select.All,Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);

   Par_GetParMultiToText ("UsrCodTch",Gbl.Usrs.Select.Tch,Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);

   Par_GetParMultiToText ("UsrCodStd",Gbl.Usrs.Select.Std,Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);
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
   Usr_AllocateListEncryptedUsrCodAll ();

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
               if (!Usr_FindEncryptedUsrCodInList (UsrDat.EncryptedUsrCod))        // If not in list ==> add it
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

bool Usr_FindEncryptedUsrCodInList (const char *EncryptedUsrCodToFind)
  {
   const char *Ptr;
   char EncryptedUsrCod[Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64+1];

   Ptr = Gbl.Usrs.Select.All;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,EncryptedUsrCod,Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
      if (!strcmp (EncryptedUsrCodToFind,EncryptedUsrCod))
         return true;        // Found!
     }
   return false;        // Not found
  }

/*****************************************************************************/
/************ Count number of valid users' IDs in encrypted list *************/
/*****************************************************************************/

unsigned Usr_CountNumUsrsInEncryptedList (void)
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

void Usr_AllocateListEncryptedUsrCodAll (void)
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

void Usr_AllocateListEncryptedUsrCodStd (void)
  {
   if (!Gbl.Usrs.Select.Std)
     {
      if ((Gbl.Usrs.Select.Std = (char *) malloc (Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS+1)) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store list of students.");
      Gbl.Usrs.Select.Std[0] = '\0';
     }
  }

/*****************************************************************************/
/****************** Allocate memory for list of teachers *********************/
/*****************************************************************************/

void Usr_AllocateListEncryptedUsrCodTch (void)
  {
   if (!Gbl.Usrs.Select.Tch)
     {
      if ((Gbl.Usrs.Select.Tch = (char *) malloc (Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS+1)) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store list of teachers.");
      Gbl.Usrs.Select.Tch[0] = '\0';
     }
  }

/*****************************************************************************/
/********************** Free memory for lists of users ***********************/
/*****************************************************************************/

void Usr_FreeListsEncryptedUsrCods (void)
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
      if ((Gbl.Usrs.ListOtherRecipients = malloc (Nck_MAX_BYTES_LIST_NICKS+1)) == NULL)
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
/******************** List users to select some of them **********************/
/*****************************************************************************/

void Usr_ShowFormsToSelectUsrListType (Act_Action_t NextAction)
  {
   /***** Start table *****/
   Lay_StartRoundFrameTable10 (NULL,2,NULL);

   /***** 1st row *****/
   /* Put a button to select USR_CLASS_ROOM */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s\" style=\"text-align:left;"
	              " vertical-align:middle;\">",
            Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO ? "USR_LIST_TYPE_ON" :
        	                                      "USR_LIST_TYPE_OFF");
   Usr_FormToSelectUsrListType (NextAction,Usr_CLASS_PHOTO);

   /* Number of columns in the class photo */
   Act_FormStart (NextAction);
   Grp_PutParamsCodGrps ();
   Usr_PutParamUsrListType (Usr_CLASS_PHOTO);
   Usr_PutParamListWithPhotos ();
   Usr_PutExtraParamsUsrList (NextAction);
   Usr_PutSelectorNumColsClassPhoto ();
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** 2nd row *****/
   /* Put a button to select Usr_LIST */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s\" style=\"text-align:left;"
	              " vertical-align:middle;\">",
            Gbl.Usrs.Me.ListType == Usr_LIST ? "USR_LIST_TYPE_ON" :
        	                               "USR_LIST_TYPE_OFF");
   Usr_FormToSelectUsrListType (NextAction,Usr_LIST);

   /* See the photos in list? */
   Act_FormStart (NextAction);
   Grp_PutParamsCodGrps ();
   Usr_PutParamUsrListType (Usr_LIST);
   Usr_PutExtraParamsUsrList (NextAction);
   Usr_PutCheckboxListWithPhotos ();
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** End of table *****/
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/************* Put a radio element to select a users' list type **************/
/*****************************************************************************/

static void Usr_FormToSelectUsrListType (Act_Action_t NextAction,Usr_ShowUsrsType_t ListType)
  {
   extern const char *The_ClassFormulNB[The_NUM_THEMES];
   extern const char *Txt_USR_LIST_TYPES[Usr_NUM_USR_LIST_TYPES];

   Act_FormStart (NextAction);
   Grp_PutParamsCodGrps ();
   Usr_PutParamListWithPhotos ();
   Usr_PutParamUsrListType (ListType);
   Usr_PutExtraParamsUsrList (NextAction);
   Act_LinkFormSubmit (Txt_USR_LIST_TYPES[ListType],The_ClassFormulNB[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"<img src=\"%s/%s16x16.gif\""
                      " alt=\"%s\" class=\"ICON16x16\" />"
                      " %s&nbsp;</a>",
            Gbl.Prefs.IconsURL,
            Usr_IconsClassPhotoOrList[ListType],
            Txt_USR_LIST_TYPES[ListType],
            Txt_USR_LIST_TYPES[ListType]);
   Act_FormEnd ();
  }

/*****************************************************************************/
/***************** Put extra parameters for a list of users ******************/
/*****************************************************************************/

void Usr_PutExtraParamsUsrList (Act_Action_t NextAction)
  {
   switch (Gbl.CurrentAct)
     {
      case ActLstInv:
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
         if (Gbl.Msg.IsReply)
           {
            Par_PutHiddenParamChar ("IsReply",'Y');
            Msg_PutHiddenParamMsgCod (Gbl.Msg.RepliedMsgCod);
            Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
           }
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
   switch (Role)
     {
      case Rol_ROLE_STUDENT:
         if (!Gbl.Usrs.LstStds.NumUsrs)
            return;
         break;
      case Rol_ROLE_TEACHER:
         if (!Gbl.Usrs.LstTchs.NumUsrs)
            return;
         break;
      default:
         return;
     }

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
   struct ListUsers *LstUsrs;
   Usr_Sex_t Sex;

   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"%u\" class=\"TIT_TBL\""
	              " style=\"text-align:left; background-color:%s;\">",
            Usr_GetColumnsForSelectUsrs (),VERY_LIGHT_BLUE);
   if (Role == Rol_ROLE_STUDENT)
     {
      fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"SEL_UNSEL_STDS\" value=\"\" onclick=\"togglecheckChildren(this,'UsrCodStd')\" />");
      LstUsrs = &Gbl.Usrs.LstStds;
     }
   else
     {
      fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"SEL_UNSEL_TCHS\" value=\"\" onclick=\"togglecheckChildren(this,'UsrCodTch')\" />");
      LstUsrs = &Gbl.Usrs.LstTchs;
     }
   Sex = Usr_GetSexOfUsrsLst (LstUsrs);
   fprintf (Gbl.F.Out,"%s:</td>"
	              "</tr>",
	    LstUsrs->NumUsrs == 1 ? Txt_ROLES_SINGUL_Abc[Role][Sex] :
                                    Txt_ROLES_PLURAL_Abc  [Role][Sex]);
  }

/*****************************************************************************/
/************************** Get sex of a list of users ***********************/
/*****************************************************************************/

static Usr_Sex_t Usr_GetSexOfUsrsLst (struct ListUsers *LstUsrs)
  {
   Usr_Sex_t Sex;
   unsigned NumUsr;

   /***** If no users, sex is undefined *****/
   if (LstUsrs->NumUsrs == 0)
      return Usr_SEX_UNKNOWN;

   /***** Initialize sex to that of first user in list *****/
   Sex = LstUsrs->Lst[0].Sex;

   /***** Search if there is one user at least with different sex than the first one *****/
   for (NumUsr = 1;
        NumUsr < LstUsrs->NumUsrs;
        NumUsr++)
      if (LstUsrs->Lst[NumUsr].Sex != Sex)
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

void Usr_PutCheckboxToSelectUser (Rol_Role_t Role,const char *EncryptedUsrCod,bool UsrIsTheMsgSender)
  {
   fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"");
   if (Role == Rol_ROLE_STUDENT)
      fprintf (Gbl.F.Out,"UsrCodStd\" value=\"%s\" onclick=\"checkParent(this,'SEL_UNSEL_STDS')\"",EncryptedUsrCod);
   else
      fprintf (Gbl.F.Out,"UsrCodTch\" value=\"%s\" onclick=\"checkParent(this,'SEL_UNSEL_TCHS')\"",EncryptedUsrCod);
   if (UsrIsTheMsgSender)        // Reply to a user
      fprintf (Gbl.F.Out," checked=\"checked\"");
   fprintf (Gbl.F.Out," />");
  }

/*****************************************************************************/
/********* Put a checkbox to select whether list users with photos ***********/
/*****************************************************************************/

void Usr_PutCheckboxListWithPhotos (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Display_photos;

   Par_PutHiddenParamChar ("WithPhotosExists",'Y');

   /***** Put checkbox to select whether list users with photos *****/
   fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"WithPhotos\" value=\"Y\"");
   if (Gbl.Usrs.Listing.WithPhotos)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   fprintf (Gbl.F.Out," onclick=\"javascript:document.getElementById('%s').submit();\" />"
                      "<span class=\"%s\">%s</span>",
	    Gbl.FormId,
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Display_photos);
  }

/*****************************************************************************/
/*********************** Set field names of user's data **********************/
/*****************************************************************************/

static void Usr_SetUsrDatMainFieldNames (void)
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
/************************** List guests' main data ***************************/
/*****************************************************************************/

static void Usr_ListMainDataGsts (bool PutCheckBoxToSelectUsr)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   unsigned NumCol;
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Initialize field names *****/
   Usr_SetUsrDatMainFieldNames ();

   if (Gbl.Usrs.LstGsts.NumUsrs)
     {
      /***** Heading row with column names *****/
      /* Start row */
      fprintf (Gbl.F.Out,"<tr>");

      /* First column used for selection  */
      if (PutCheckBoxToSelectUsr)
	 fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:left;"
	                    " background-color:%s;\">"
	                    "&nbsp;"
	                    "</td>",
		  VERY_LIGHT_BLUE);

      /* Columns for the data */
      for (NumCol = 0;
           NumCol < Usr_NUM_MAIN_FIELDS_DATA_USR;
           NumCol++)
         if (NumCol != 2 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
            fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:left;"
	                       " background-color:%s;\">"
        	               "%s&nbsp;"
        	               "</td>",
                     VERY_LIGHT_BLUE,Usr_UsrDatMainFieldNames[NumCol]);

      /* End row */
      fprintf (Gbl.F.Out,"</tr>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List students' data *****/
      for (NumUsr = 0;
           NumUsr < Gbl.Usrs.LstGsts.NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstGsts.Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = false;	// Guests have no courses,...
                                        // ...so they have not accepted...
            	    	    	    	// ...inscription in any course
            Usr_WriteRowGstMainData (++NumUsr,&UsrDat);
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else        // Gbl.Usrs.LstGsts.NumUsrs == 0
      Usr_ShowWarningNoUsersFound (Rol_ROLE_GUEST__);

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstGsts);
  }

/*****************************************************************************/
/*************************** List main students' data ************************/
/*****************************************************************************/

static void Usr_ListMainDataStds (bool PutCheckBoxToSelectUsr)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   unsigned NumCol;
   unsigned NumUsr;
   char *GroupNames;
   struct UsrData UsrDat;

   /***** Initialize field names *****/
   Usr_SetUsrDatMainFieldNames ();

   GroupNames = NULL;        // To avoid warning

   if (Gbl.Usrs.LstStds.NumUsrs)
     {
      /***** Allocate memory for the string with the list of group names where student belongs to *****/
      if ((GroupNames = (char *) malloc ((MAX_LENGTH_GROUP_NAME+3)*Gbl.CurrentCrs.Grps.GrpTypes.NumGrpsTotal)) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store names of groups.");

      /***** Start table with list of students *****/
      if (!Gbl.Usrs.ClassPhoto.AllGroups)
        {
         fprintf (Gbl.F.Out,"<tr>"
                            "<td colspan=\"%u\" class=\"TIT\""
                            " style=\"text-align:center;\">",
                  1 + Usr_NUM_MAIN_FIELDS_DATA_USR);
         Grp_WriteNamesOfSelectedGrps ();
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");
        }

      /***** Heading row with column names *****/
      /* Start row */
      fprintf (Gbl.F.Out,"<tr>");

      /* First column used for selection  */
      if (PutCheckBoxToSelectUsr)
	 fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:left;"
	                    " background-color:%s;\">"
	                    "&nbsp;"
	                    "</td>",
		  VERY_LIGHT_BLUE);

      /* Columns for the data */
      for (NumCol = 0;
           NumCol < Usr_NUM_MAIN_FIELDS_DATA_USR;
           NumCol++)
         if (NumCol != 2 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
            fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:left;"
	                       " background-color:%s;\">"
        	               "%s&nbsp;"
        	               "</td>",
                     VERY_LIGHT_BLUE,Usr_UsrDatMainFieldNames[NumCol]);

      /* End row */
      fprintf (Gbl.F.Out,"</tr>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List students' data *****/
      for (NumUsr = 0;
           NumUsr < Gbl.Usrs.LstStds.NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstStds.Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = Gbl.Usrs.LstStds.Lst[NumUsr].Accepted;
            Usr_WriteRowStdMainData (++NumUsr,&UsrDat,PutCheckBoxToSelectUsr);
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** Free memory used by the string with the list of group names where student belongs to *****/
      free ((void *) GroupNames);
     }
   else        // Gbl.Usrs.LstStds.NumUsrs == 0
      Usr_ShowWarningNoUsersFound (Rol_ROLE_STUDENT);

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstStds);
  }

/*****************************************************************************/
/************************** List main teachers' data *************************/
/*****************************************************************************/

static void Usr_ListMainDataTchs (bool PutCheckBoxToSelectUsr)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   unsigned NumColumns;
   unsigned NumCol;
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Initialize field names *****/
   Usr_SetUsrDatMainFieldNames ();

   if (Gbl.Usrs.LstTchs.NumUsrs)
     {
      /***** Initialize number of columns *****/
      NumColumns = Usr_NUM_MAIN_FIELDS_DATA_USR;

      /***** Heading row with column names *****/
      /* Start row */
      fprintf (Gbl.F.Out,"<tr>");

      /* First column used for selection  */
      if (PutCheckBoxToSelectUsr)
	 fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:left;"
	                    " background-color:%s;\">"
	                    "&nbsp;"
	                    "</td>",
		  VERY_LIGHT_BLUE);

      /* Columns for the data */
      for (NumCol = 0;
           NumCol < NumColumns;
           NumCol++)
         if (NumCol != 2 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
            fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:left;"
	                       " background-color:%s;\">"
        	               "%s&nbsp;"
        	               "</td>",
                     VERY_LIGHT_BLUE,Usr_UsrDatMainFieldNames[NumCol]);

      /* End row */
      fprintf (Gbl.F.Out,"</tr>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List data of teachers *****/
      for (NumUsr = 0;
           NumUsr < Gbl.Usrs.LstTchs.NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstTchs.Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = Gbl.Usrs.LstTchs.Lst[NumUsr].Accepted;
            Usr_WriteRowTchMainData (++NumUsr,&UsrDat,PutCheckBoxToSelectUsr);
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else        // Gbl.Usrs.LstTchs.NumUsrs == 0
      Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_ROLE_TEACHER]);

   /***** Free memory for teachers list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstTchs);
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

   if (Gbl.Usrs.LstGsts.NumUsrs)
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
         fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:left;"
	                    " background-color:%s;\">"
                            "%s&nbsp;"
                            "</td>",
                  VERY_LIGHT_BLUE,FieldNames[NumCol]);

      /* End row */
      fprintf (Gbl.F.Out,"</tr>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List guests' data *****/
      for (NumUsr = 0;
           NumUsr < Gbl.Usrs.LstGsts.NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstGsts.Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = false;	// Guests have no courses,...
            	    	    	    	// ...so they have not accepted...
                                        // ...inscription in any course
            NumUsr++;
            Usr_WriteRowGstAllData (&UsrDat);
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** End of table *****/
      fprintf (Gbl.F.Out,"</table>");
     }
   else        // Gbl.Usrs.LstGsts.NumUsrs == 0
      Usr_ShowWarningNoUsersFound (Rol_ROLE_GUEST__);

   /***** Free memory for guests' list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstGsts);
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
   Usr_GetUsrsLst (Rol_ROLE_STUDENT,Gbl.Scope.Current,NULL,false);

   if (Gbl.Usrs.LstStds.NumUsrs)
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
	                    "<td colspan=\"%u\" class=\"TIT\""
	                    " style=\"text-align:center;\">",
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
         fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:left;"
	                    " background-color:%s;\">"
                            "%s&nbsp;"
                            "</td>",
                  VERY_LIGHT_BLUE,FieldNames[NumCol]);

      /* 2. Columns for the groups */
      if (Gbl.Scope.Current == Sco_SCOPE_CRS)
        {
         if (Gbl.CurrentCrs.Grps.GrpTypes.Num)
            for (NumGrpTyp = 0;
                 NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
                 NumGrpTyp++)
               if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)         // If current course tiene groups of este type
                  fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\""
                	             " style=\"text-align:left;"
	                             " background-color:%s;\">"
                	             "%s %s&nbsp;"
                	             "</td>",
                           VERY_LIGHT_BLUE,Txt_Group,
                           Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName);

         if (Gbl.CurrentCrs.Records.LstFields.Num)
           {
            /* 3. Names of record fields that depend on the course */
            for (NumField = 0;
                 NumField < Gbl.CurrentCrs.Records.LstFields.Num;
                 NumField++)
               fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\""
        	                  " style=\"text-align:left;"
	                          " background-color:%s;\">"
        	                  "%s&nbsp;"
        	                  "</td>",
                        VERY_LIGHT_BLUE,Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Name);
            /* 4. Visibility type for the record fields that depend on the course, in other row */
            fprintf (Gbl.F.Out,"</tr><tr>");
            for (NumCol = 0;
                 NumCol < NumColumnsCardAndGroups;
                 NumCol++)
               if (NumCol != 1 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want it in listing
                  fprintf (Gbl.F.Out,"<td style=\"background-color:%s;\"></td>",
                           LIGHTEST_BLUE);
            for (NumField = 0;
                 NumField < Gbl.CurrentCrs.Records.LstFields.Num;
                 NumField++)
               fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\""
        	                  " style=\"text-align:left;"
	                          " background-color:%s;\">"
        	                  "(%s)&nbsp;"
        	                  "</td>",
                        LIGHTEST_BLUE,
                        Txt_RECORD_FIELD_VISIBILITY_RECORD[Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Visibility]);
           }
        }

      /* End row */
      fprintf (Gbl.F.Out,"</tr>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List students' data *****/
      for (NumUsr = 0;
           NumUsr < Gbl.Usrs.LstStds.NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstStds.Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = Gbl.Usrs.LstStds.Lst[NumUsr].Accepted;
            NumUsr++;
            Usr_WriteRowStdAllData (&UsrDat,GroupNames);
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
   else        // Gbl.Usrs.LstStds.NumUsrs == 0
      Usr_ShowWarningNoUsersFound (Rol_ROLE_STUDENT);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstStds);

   /***** Free list of groups types and groups in current course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/*************** List users (of current course) for selection ****************/
/*****************************************************************************/

void Usr_ListUsrsForSelection (Rol_Role_t Role)
  {
   unsigned NumCol;
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Initialize field names *****/
   Usr_SetUsrDatMainFieldNames ();

   /***** Heading row with column names *****/
   /* Start row and first column used for selection */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;"
	              " background-color:%s;\">"
                      "&nbsp;"
                      "</td>",
            VERY_LIGHT_BLUE);
   /* Columns for the data */
   for (NumCol = 0;
        NumCol < Usr_NUM_MAIN_FIELDS_DATA_USR;
        NumCol++)
      if (NumCol != 2 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
         fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:left;"
	                    " background-color:%s;\">"
                            "%s&nbsp;"
                            "</td>",
                  VERY_LIGHT_BLUE,Usr_UsrDatMainFieldNames[NumCol]);
   /* End row */
   fprintf (Gbl.F.Out,"</tr>");

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** List users' data *****/
   switch (Role)
     {
      case Rol_ROLE_STUDENT:
         for (NumUsr = 0;
              NumUsr < Gbl.Usrs.LstStds.NumUsrs; )
           {
            UsrDat.UsrCod = Gbl.Usrs.LstStds.Lst[NumUsr].UsrCod;
            if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
              {
               UsrDat.Accepted = Gbl.Usrs.LstStds.Lst[NumUsr].Accepted;
               Usr_WriteRowStdMainData (++NumUsr,&UsrDat,true);
              }
           }
         break;
      case Rol_ROLE_TEACHER:
         for (NumUsr = 0;
              NumUsr < Gbl.Usrs.LstTchs.NumUsrs; )
           {
            UsrDat.UsrCod = Gbl.Usrs.LstTchs.Lst[NumUsr].UsrCod;
            if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
              {
               UsrDat.Accepted = Gbl.Usrs.LstTchs.Lst[NumUsr].Accepted;
               Usr_WriteRowTchMainData (++NumUsr,&UsrDat,true);
              }
           }
         break;
      default:
         break;
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
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS    |
	               1 << Sco_SCOPE_CTY     |
                       1 << Sco_SCOPE_INS |
                       1 << Sco_SCOPE_CTR      |
                       1 << Sco_SCOPE_DEG      |
                       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   Sco_GetScope ();

   /***** Get and order list of teachers *****/
   Usr_GetUsrsLst (Rol_ROLE_TEACHER,Gbl.Scope.Current,NULL,false);

   if (Gbl.Usrs.LstTchs.NumUsrs)
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
         fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:left;"
	                    " background-color:%s;\">"
                            "%s&nbsp;"
                            "</td>",
                  VERY_LIGHT_BLUE,FieldNames[NumCol]);

      /* End row */
      fprintf (Gbl.F.Out,"</tr>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List data of teachers *****/
      for (NumUsr = 0;
           NumUsr < Gbl.Usrs.LstTchs.NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstTchs.Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = Gbl.Usrs.LstTchs.Lst[NumUsr].Accepted;
            NumUsr++;
            Usr_WriteRowTchAllData (&UsrDat);
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** End of table *****/
      fprintf (Gbl.F.Out,"</table>");
     }
   else        // Gbl.Usrs.LstTchs.NumUsrs == 0
      Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_ROLE_TEACHER]);

   /***** Free memory for teachers list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstTchs);
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
   struct ListUsers *LstUsrs;
   Usr_Sex_t Sex;
   unsigned NumCol;
   unsigned NumUsr;
   struct UsrData UsrDat;
   unsigned NumUsrs;

   /***** Set list *****/
   LstUsrs = ( Role == Rol_ROLE_TEACHER ? &Gbl.Usrs.LstTchs :
	      (Role == Rol_ROLE_STUDENT ? &Gbl.Usrs.LstStds :
					  &Gbl.Usrs.LstGsts));

   /***** Initialize field names *****/
   Usr_SetUsrDatMainFieldNames ();

   /***** Search for teachers *****/
   Usr_GetUsrsLst (Role,Gbl.Scope.Current,UsrQuery,true);
   if ((NumUsrs = LstUsrs->NumUsrs))
     {
      /***** Write heading *****/
      Sex = Usr_GetSexOfUsrsLst (LstUsrs);
      Lay_StartRoundFrameTable10 (NULL,2,Txt_ROLES_PLURAL_Abc[Role][Sex]);

      /* Number of users found */
      fprintf (Gbl.F.Out,"<tr>"
			 "<td colspan=\"%u\" class=\"TIT_TBL\""
			 " style=\"text-align:center;\">",
	       Usr_NUM_MAIN_FIELDS_DATA_USR);
      if (NumUsrs == 1)
	 fprintf (Gbl.F.Out,"1 %s",
		  Txt_ROLES_SINGUL_abc[Role][Sex]);
      else
	 fprintf (Gbl.F.Out,"%u %s",
		  NumUsrs,
		  Txt_ROLES_PLURAL_abc[Role][Sex]);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      /***** Heading row with column names *****/
      /* Start row */
      fprintf (Gbl.F.Out,"<tr>");
      for (NumCol = 0;
           NumCol < Usr_NUM_MAIN_FIELDS_DATA_USR;
           NumCol++)
         fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:left;"
	                    " background-color:%s;\">"
                            "%s&nbsp;"
                            "</td>",
                  VERY_LIGHT_BLUE,
                  Usr_UsrDatMainFieldNames[NumCol]);
      /* End row */
      fprintf (Gbl.F.Out,"</tr>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List data of users *****/
      for (NumUsr = 0;
           NumUsr < NumUsrs;
           NumUsr++)
        {
         UsrDat.UsrCod = LstUsrs->Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = LstUsrs->Lst[NumUsr].Accepted;

            /* Write data of this user */
            if (Role == Rol_ROLE_STUDENT)
               Usr_WriteRowStdMainData (NumUsr + 1,&UsrDat,false);
            else	// Role == Rol_ROLE_TEACHER
               Usr_WriteRowTchMainData (NumUsr + 1,&UsrDat,false);

	    /* Write all the courses this user belongs to */
            if (Role != Rol_ROLE_GUEST__)
              {
	       fprintf (Gbl.F.Out,"<tr>"
				  "<td colspan=\"3\"></td>"
				  "<td colspan=\"%u\">",
			Usr_NUM_MAIN_FIELDS_DATA_USR-3);
	       Lay_StartRoundFrameTable10 (NULL,2,NULL);
	       Crs_GetAndWriteCrssOfAUsr (UsrDat.UsrCod,Role);
	       Lay_EndRoundFrameTable10 ();
	       fprintf (Gbl.F.Out,"</td>"
				  "</tr>");
              }
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** Table end *****/
      Lay_EndRoundFrameTable10 ();
     }

   /***** Free memory for teachers list *****/
   Usr_FreeUsrsList (LstUsrs);

   return NumUsrs;
  }

/*****************************************************************************/
/**************************** List administrators ****************************/
/*****************************************************************************/

void Usr_ListDataAdms (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Photo;
   extern const char *Txt_ID;
   extern const char *Txt_Surname_1;
   extern const char *Txt_Surname_2;
   extern const char *Txt_First_name;
   extern const char *Txt_Email;
   extern const char *Txt_Institution;
   extern const char *Txt_Scope;
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   unsigned NumColumns;
   unsigned NumCol;
   unsigned NumUsr;
   struct UsrData UsrDat;
   const char *FieldNames[Usr_NUM_MAIN_FIELDS_DATA_ADM];

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
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS    |
	               1 << Sco_SCOPE_CTY     |
                       1 << Sco_SCOPE_INS |
                       1 << Sco_SCOPE_CTR      |
                       1 << Sco_SCOPE_DEG;
   Gbl.Scope.Default = Sco_SCOPE_DEG;
   Sco_GetScope ();

   /***** Form to select range of administrators *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:center;\">"
	              "%s: ",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Scope);
   Act_FormStart (ActLstAdm);
   Sco_PutSelectorScope (true);
   Usr_PutParamListWithPhotos ();
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");

   /***** Get and order list of administrators *****/
   Usr_GetAdmsLst (Gbl.Scope.Current);

   if (Gbl.Usrs.LstAdms.NumUsrs)
     {
      /****** See the photos? *****/
      fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
      Act_FormStart (ActLstAdm);
      Sco_PutParamScope (Gbl.Scope.Current);
      Usr_PutCheckboxListWithPhotos ();
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");

      /***** Initialize number of columns *****/
      NumColumns = Usr_NUM_MAIN_FIELDS_DATA_ADM;

      /***** Start table with list of administrators *****/
      Lay_StartRoundFrameTable10 (NULL,0,NULL);

      /***** Heading row with column names *****/
      /* Start row */
      fprintf (Gbl.F.Out,"<tr>");
      for (NumCol = 0;
           NumCol < NumColumns;
           NumCol++)
         if (NumCol != 1 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
            fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:left;"
	                       " background-color:%s;\">"
        	               "%s&nbsp;"
        	               "</td>",
                     VERY_LIGHT_BLUE,FieldNames[NumCol]);

      /* End row */
      fprintf (Gbl.F.Out,"</tr>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List data of administrators *****/
      for (NumUsr = 0;
           NumUsr < Gbl.Usrs.LstAdms.NumUsrs; )
        {
         UsrDat.UsrCod = Gbl.Usrs.LstAdms.Lst[NumUsr].UsrCod;
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            UsrDat.Accepted = Gbl.Usrs.LstAdms.Lst[NumUsr].Accepted;
            Usr_WriteRowAdmData (++NumUsr,&UsrDat);
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** End of table *****/
      Lay_EndRoundFrameTable10 ();
     }
   else        // Gbl.Usrs.LstAdms.NumUsrs == 0
      Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_ROLE_DEG_ADM]);

   /***** Free memory for teachers list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstAdms);
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
   sprintf (Query,"SELECT UsrListType FROM crs_usr WHERE CrsCod='%ld' AND UsrCod='%ld'",
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
/********************* Show list or class photo of guests ********************/
/*****************************************************************************/

void Usr_SeeGuests (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Scope;
   extern const char *Txt_Show_all_data;
   extern const char *Txt_Show_records;

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Sco_SetScopesForListingGuests ();
   Sco_GetScope ();

   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_CTR_ADM:
      case Rol_ROLE_INS_ADM:
      case Rol_ROLE_SYS_ADM:
         /***** Form to select range of guests *****/
         fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:center;\">",
                  The_ClassFormul[Gbl.Prefs.Theme]);
         Act_FormStart (ActLstInv);
         Usr_PutParamUsrListType (Gbl.Usrs.Me.ListType);
         Usr_PutParamColsClassPhoto ();
         Usr_PutParamListWithPhotos ();
         fprintf (Gbl.F.Out,"%s: ",Txt_Scope);
         Sco_PutSelectorScope (true);
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</div>");
         break;
      default:
         break;
     }

   /***** Form to select type of list of users *****/
   Usr_ShowFormsToSelectUsrListType (ActLstInv);

   /***** Get and order list of students in current scope *****/
   Usr_GetGstsLst (Gbl.Scope.Current);

   if (Gbl.Usrs.LstGsts.NumUsrs)
     {
      if (Usr_GetIfShowBigList (Gbl.Usrs.LstGsts.NumUsrs))
	{
         /***** Get list of selected users *****/
         Usr_GetListSelectedUsrs ();

         switch (Gbl.Usrs.Me.ListType)
           {
            case Usr_CLASS_PHOTO:
                /***** Link to print view *****/
	       fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
	       Lay_PutLinkToPrintView1 (ActPrnInvPho);
	       Lay_PutLinkToPrintView2 ();
	       fprintf (Gbl.F.Out,"</div>");
	       break;
	    case Usr_LIST:
	       /****** Link to show all the data ******/
	       fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
	       Act_FormStart (ActLstInvAll);
	       Usr_PutParamListWithPhotos ();
	       Usr_PutExtraParamsUsrList (ActLstInvAll);
	       Act_LinkFormSubmit (Txt_Show_all_data,The_ClassFormul[Gbl.Prefs.Theme]);
	       Lay_PutSendIcon ("table",Txt_Show_all_data,Txt_Show_all_data);
	       Act_FormEnd ();
	       fprintf (Gbl.F.Out,"</div>");
	       break;
           }

         /***** Draw a class photo with students of the course *****/
         fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");

         /* Form start */
	 Act_FormStart (ActSeeRecSevInv);
	 Grp_PutParamsCodGrps ();

         /* Header */
         Lay_StartRoundFrameTable10 (NULL,0,NULL);

         if (Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO)
	    Lay_WriteHeaderClassPhoto (Gbl.Usrs.ClassPhoto.Cols,false,true,
				       (Gbl.Scope.Current == Sco_SCOPE_CTR ||
					Gbl.Scope.Current == Sco_SCOPE_INS) ? Gbl.CurrentIns.Ins.InsCod :
					                                              -1L,
				       -1L,
				       -1L);

	 /* Put a row to select all users */
         Usr_PutCheckboxToSelectAllTheUsers (Rol_ROLE_GUEST__);

         /* Draw the classphoto/list */
         switch (Gbl.Usrs.Me.ListType)
           {
            case Usr_CLASS_PHOTO:
               Usr_DrawClassPhoto (Usr_CLASS_PHOTO_SEL_SEE,
        	                   Rol_ROLE_GUEST__);
               break;
            case Usr_LIST:
               Usr_ListMainDataGsts (true);
               break;
           }

         Lay_EndRoundFrameTable10 ();

         /* Send button */
         Lay_PutSendButton (Txt_Show_records);
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</div>");
	}
     }
   else
      Usr_ShowWarningNoUsersFound (Rol_ROLE_GUEST__);

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstGsts);
  }

/*****************************************************************************/
/******************** Show list or class photo of students *******************/
/*****************************************************************************/

void Usr_SeeStudents (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Scope;
   extern const char *Txt_Show_all_data;
   extern const char *Txt_Show_records;
   bool ICanViewRecords;

   if (Gbl.CurrentCrs.Crs.CrsCod > 0 &&			// Course selected
       (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER ||	// My role in current course is teacher...
        Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM))	// ...or superuser
     {
      fprintf (Gbl.F.Out,"<div style=\"text-align:center;"
	                 " margin-bottom:10px;\">");

      /***** Link to list official students *****/
      Usr_PutLinkToListOfficialStudents ();

      /***** Link to edit record fields *****/
      Rec_PutLinkToEditRecordFields ();

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
	              (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_STUDENT ||
                       Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER ||
                       Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADM ||
	               Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM));

   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_DEG_ADM:
      case Rol_ROLE_SYS_ADM:
         /***** Form to select range of students *****/
         fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:center;\">",
                  The_ClassFormul[Gbl.Prefs.Theme]);
         Act_FormStart (ActLstStd);
         Usr_PutParamUsrListType (Gbl.Usrs.Me.ListType);
         Usr_PutParamColsClassPhoto ();
         Usr_PutParamListWithPhotos ();
         fprintf (Gbl.F.Out,"%s: ",Txt_Scope);
         Sco_PutSelectorScope (true);
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</div>");
         break;
      default:
         break;
     }

   /***** Form to select groups *****/
   if (Gbl.Scope.Current == Sco_SCOPE_CRS)
      Grp_ShowFormToSelectSeveralGroups (ActLstStd);

   /***** Form to select type of list of users *****/
   Usr_ShowFormsToSelectUsrListType (ActLstStd);

   /***** Get and order list of students *****/
   Usr_GetUsrsLst (Rol_ROLE_STUDENT,Gbl.Scope.Current,NULL,false);

   if (Gbl.Usrs.LstStds.NumUsrs)
     {
      if (Usr_GetIfShowBigList (Gbl.Usrs.LstStds.NumUsrs))
	{
         /***** Get list of selected users *****/
         Usr_GetListSelectedUsrs ();

         switch (Gbl.Usrs.Me.ListType)
           {
            case Usr_CLASS_PHOTO:
                /***** Link to print view *****/
	       fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
	       Lay_PutLinkToPrintView1 (ActPrnStdPho);
	       Grp_PutParamsCodGrps ();
	       Lay_PutLinkToPrintView2 ();
	       fprintf (Gbl.F.Out,"</div>");
	       break;
	    case Usr_LIST:
	       if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER)
		 {
		  /****** Link to show all the data ******/
		  fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
		  Act_FormStart (ActLstStdAll);
		  Grp_PutParamsCodGrps ();
		  Usr_PutParamListWithPhotos ();
		  Usr_PutExtraParamsUsrList (ActLstStdAll);
		  Act_LinkFormSubmit (Txt_Show_all_data,The_ClassFormul[Gbl.Prefs.Theme]);
		  Lay_PutSendIcon ("table",Txt_Show_all_data,Txt_Show_all_data);
		  Act_FormEnd ();
		  fprintf (Gbl.F.Out,"</div>");
		 }
	       break;
           }

         /***** Draw a class photo with students of the course *****/
         fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");

         /* Form start */
         if (ICanViewRecords)
           {
	    Act_FormStart (ActSeeRecSevStd);
	    Grp_PutParamsCodGrps ();
           }

         /* Header */
         Lay_StartRoundFrameTable10 (NULL,0,NULL);

         if (Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO)
	    Lay_WriteHeaderClassPhoto (Gbl.Usrs.ClassPhoto.Cols,false,true,
				       (Gbl.Scope.Current == Sco_SCOPE_CRS ||
					Gbl.Scope.Current == Sco_SCOPE_DEG ||
					Gbl.Scope.Current == Sco_SCOPE_CTR ||
					Gbl.Scope.Current == Sco_SCOPE_INS) ? Gbl.CurrentIns.Ins.InsCod :
					                                              -1L,
				       (Gbl.Scope.Current == Sco_SCOPE_CRS ||
					Gbl.Scope.Current == Sco_SCOPE_DEG) ? Gbl.CurrentDeg.Deg.DegCod :
					                                         -1L,
					Gbl.Scope.Current == Sco_SCOPE_CRS ? Gbl.CurrentCrs.Crs.CrsCod :
					                                        -1L);

	 /* Put a row to select all users */
         if (ICanViewRecords)
            Usr_PutCheckboxToSelectAllTheUsers (Rol_ROLE_STUDENT);

         /* Draw the classphoto/list */
         switch (Gbl.Usrs.Me.ListType)
           {
            case Usr_CLASS_PHOTO:
               Usr_DrawClassPhoto (ICanViewRecords ? Usr_CLASS_PHOTO_SEL_SEE :
        	                                     Usr_CLASS_PHOTO_SEE,
        	                   Rol_ROLE_STUDENT);
               break;
            case Usr_LIST:
               Usr_ListMainDataStds (ICanViewRecords);
               break;
           }

         Lay_EndRoundFrameTable10 ();

         /* Send button */
         if (ICanViewRecords)
           {
            Lay_PutSendButton (Txt_Show_records);
            Act_FormEnd ();
           }

         fprintf (Gbl.F.Out,"</div>");
	}
     }
   else
      Usr_ShowWarningNoUsersFound (Rol_ROLE_STUDENT);

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstStds);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/******************** Show list or class photo of teachers *******************/
/*****************************************************************************/

void Usr_SeeTeachers (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Scope;
   extern const char *Txt_Show_all_data;
   extern const char *Txt_Show_records;
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   bool ICanViewRecords;

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS    |
	               1 << Sco_SCOPE_CTY     |
                       1 << Sco_SCOPE_INS |
                       1 << Sco_SCOPE_CTR      |
                       1 << Sco_SCOPE_DEG      |
                       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   Sco_GetScope ();
   ICanViewRecords = (Gbl.Scope.Current == Sco_SCOPE_CRS);

   /***** Form to select scope *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:center;\">",
	    The_ClassFormul[Gbl.Prefs.Theme]);
   Act_FormStart (ActLstTch);
   Usr_PutParamUsrListType (Gbl.Usrs.Me.ListType);
   Usr_PutParamColsClassPhoto ();
   Usr_PutParamListWithPhotos ();
   fprintf (Gbl.F.Out,"%s: ",Txt_Scope);
   Sco_PutSelectorScope (true);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");

   /***** Form to select type of list of users *****/
   Usr_ShowFormsToSelectUsrListType (ActLstTch);

   /***** Get and order list of teachers *****/
   Usr_GetUsrsLst (Rol_ROLE_TEACHER,Gbl.Scope.Current,NULL,false);

   if (Gbl.Usrs.LstTchs.NumUsrs)
     {
      if (Usr_GetIfShowBigList (Gbl.Usrs.LstTchs.NumUsrs))
	{
	 /***** Link to print view *****/
         switch (Gbl.Usrs.Me.ListType)
           {
            case Usr_CLASS_PHOTO:
               fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
	       Lay_PutLinkToPrintView1 (ActPrnTchPho);
	       Sco_PutParamScope (Gbl.Scope.Current);
	       Lay_PutLinkToPrintView2 ();
	       fprintf (Gbl.F.Out,"</div>");
	       break;
            case Usr_LIST:
	       if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER)
		 {
		  /****** Link to show all the data ******/
		  fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
		  Act_FormStart (ActLstTchAll);
		  Sco_PutParamScope (Gbl.Scope.Current);
		  Usr_PutParamListWithPhotos ();
		  Act_LinkFormSubmit (Txt_Show_all_data,The_ClassFormul[Gbl.Prefs.Theme]);
		  Lay_PutSendIcon ("table",Txt_Show_all_data,Txt_Show_all_data);
		  Act_FormEnd ();
		  fprintf (Gbl.F.Out,"</div>");
		 }
               break;
           }

         /***** Draw a class photo with teachers of the course *****/
         fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");

         /* Form start */
         if (ICanViewRecords)
            Act_FormStart (ActSeeRecSevTch);

         /* Header */
         Lay_StartRoundFrameTable10 (NULL,0,NULL);

         if (Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO)
	    Lay_WriteHeaderClassPhoto (Gbl.Usrs.ClassPhoto.Cols,false,true,
				       (Gbl.Scope.Current == Sco_SCOPE_CRS ||
					Gbl.Scope.Current == Sco_SCOPE_DEG ||
					Gbl.Scope.Current == Sco_SCOPE_CTR ||
					Gbl.Scope.Current == Sco_SCOPE_INS) ? Gbl.CurrentIns.Ins.InsCod :
					                                              -1L,
				       (Gbl.Scope.Current == Sco_SCOPE_CRS ||
					Gbl.Scope.Current == Sco_SCOPE_DEG) ? Gbl.CurrentDeg.Deg.DegCod :
					                                         -1L,
					Gbl.Scope.Current == Sco_SCOPE_CRS ? Gbl.CurrentCrs.Crs.CrsCod :
					                                        -1L);

	 /* Put a row to select all users */
         if (ICanViewRecords)
	    Usr_PutCheckboxToSelectAllTheUsers (Rol_ROLE_TEACHER);

         /***** Draw the classphoto/list  *****/
         switch (Gbl.Usrs.Me.ListType)
           {
            case Usr_CLASS_PHOTO:
               Usr_DrawClassPhoto (ICanViewRecords ? Usr_CLASS_PHOTO_SEL_SEE :
        	                                     Usr_CLASS_PHOTO_SEE,
        	                   Rol_ROLE_TEACHER);
               break;
            case Usr_LIST:
               Usr_ListMainDataTchs (ICanViewRecords);
               break;
           }

         Lay_EndRoundFrameTable10 ();

         if (ICanViewRecords)
           {
            /* Send button */
            Lay_PutSendButton (Txt_Show_records);
            Act_FormEnd ();
           }

         fprintf (Gbl.F.Out,"</div>");
	}
     }
   else
      Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_ROLE_TEACHER]);

   /***** Free memory for teachers list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstTchs);
  }

/*****************************************************************************/
/******************* Put a link to list official students ********************/
/*****************************************************************************/

static void Usr_PutLinkToListOfficialStudents (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Official_students;
   bool ExternalUsrsServiceAvailable = (Cfg_EXTERNAL_LOGIN_CLIENT_COMMAND[0] != '\0');

   if (ExternalUsrsServiceAvailable &&				// There is an external service for authentication and official lists
       Gbl.Imported.ExternalUsrId[0] &&			// I was authenticated from external service...
       Gbl.Imported.ExternalSesId[0] &&
       Gbl.Imported.ExternalRole == Rol_ROLE_TEACHER)	// ...as a teacher
     {
      /***** Link to list official students *****/
      Act_FormStart (ActGetExtLstStd);
      Act_LinkFormSubmit (Txt_Official_students,The_ClassFormul[Gbl.Prefs.Theme]);
      Lay_PutSendIcon ("list",Txt_Official_students,Txt_Official_students);
      Act_FormEnd ();
     }
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

   if (Gbl.Usrs.LstGsts.NumUsrs)
     {
      /***** Draw the guests' class photo *****/
      Lay_StartRoundFrameTable10 (NULL,0,NULL);
      Lay_WriteHeaderClassPhoto (Gbl.Usrs.ClassPhoto.Cols,true,true,
				 (Gbl.Scope.Current == Sco_SCOPE_CTR ||
				  Gbl.Scope.Current == Sco_SCOPE_INS) ? Gbl.CurrentIns.Ins.InsCod :
                                                                                -1L,
				 -1L,-1L);
      Usr_DrawClassPhoto (Usr_CLASS_PHOTO_PRN,Rol_ROLE_GUEST__);
      Lay_EndRoundFrameTable10 ();
     }
   else
      Usr_ShowWarningNoUsersFound (Rol_ROLE_GUEST__);

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstGsts);
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
   Usr_GetUsrsLst (Rol_ROLE_STUDENT,Gbl.Scope.Current,NULL,false);

   if (Gbl.Usrs.LstStds.NumUsrs)
     {
      /***** Draw the students' class photo *****/
      Lay_StartRoundFrameTable10 (NULL,0,NULL);
      Lay_WriteHeaderClassPhoto (Gbl.Usrs.ClassPhoto.Cols,true,true,
				 (Gbl.Scope.Current == Sco_SCOPE_CRS ||
				  Gbl.Scope.Current == Sco_SCOPE_DEG ||
				  Gbl.Scope.Current == Sco_SCOPE_CTR ||
				  Gbl.Scope.Current == Sco_SCOPE_INS) ? Gbl.CurrentIns.Ins.InsCod :
					                                        -1L,
				 (Gbl.Scope.Current == Sco_SCOPE_CRS ||
				  Gbl.Scope.Current == Sco_SCOPE_DEG) ? Gbl.CurrentDeg.Deg.DegCod :
					                                   -1L,
				  Gbl.Scope.Current == Sco_SCOPE_CRS ? Gbl.CurrentCrs.Crs.CrsCod :
					                                  -1L);
      Usr_DrawClassPhoto (Usr_CLASS_PHOTO_PRN,Rol_ROLE_STUDENT);
      Lay_EndRoundFrameTable10 ();
     }
   else
      Usr_ShowWarningNoUsersFound (Rol_ROLE_STUDENT);

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstStds);

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
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS    |
	               1 << Sco_SCOPE_CTY     |
                       1 << Sco_SCOPE_INS |
                       1 << Sco_SCOPE_CTR      |
                       1 << Sco_SCOPE_DEG      |
                       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   Sco_GetScope ();

   /***** Get and order list of teachers *****/
   Usr_GetUsrsLst (Rol_ROLE_TEACHER,Gbl.Scope.Current,NULL,false);

   if (Gbl.Usrs.LstTchs.NumUsrs)
     {
      /***** Draw the teachers' class photo *****/
      Lay_StartRoundFrameTable10 (NULL,0,NULL);
      Lay_WriteHeaderClassPhoto (Gbl.Usrs.ClassPhoto.Cols,true,true,
				 (Gbl.Scope.Current == Sco_SCOPE_CRS ||
				  Gbl.Scope.Current == Sco_SCOPE_DEG ||
				  Gbl.Scope.Current == Sco_SCOPE_CTR ||
				  Gbl.Scope.Current == Sco_SCOPE_INS) ? Gbl.CurrentIns.Ins.InsCod :
					                                        -1L,
				 (Gbl.Scope.Current == Sco_SCOPE_CRS ||
				  Gbl.Scope.Current == Sco_SCOPE_DEG) ? Gbl.CurrentDeg.Deg.DegCod :
					                                   -1L,
				  Gbl.Scope.Current == Sco_SCOPE_CRS ? Gbl.CurrentCrs.Crs.CrsCod :
					                                  -1L);
      Usr_DrawClassPhoto (Usr_CLASS_PHOTO_PRN,Rol_ROLE_TEACHER);
      Lay_EndRoundFrameTable10 ();
     }
   else
      Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_ROLE_TEACHER]);

   /***** Free memory for teachers list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstTchs);
  }

/*****************************************************************************/
/****************** Draw class photo with students or teachers ***************/
/*****************************************************************************/

static void Usr_DrawClassPhoto (Usr_ClassPhotoType_t ClassPhotoType,
                                Rol_Role_t RoleInClassPhoto)
  {
   struct ListUsers *LstUsrs;
   unsigned NumUsr;
   bool TRIsOpen = false;
   bool PutCheckBoxToSelectUsr = (ClassPhotoType == Usr_CLASS_PHOTO_SEL ||
	                          ClassPhotoType == Usr_CLASS_PHOTO_SEL_SEE);
   bool PutOriginPlace = (RoleInClassPhoto == Rol_ROLE_STUDENT &&
                          (ClassPhotoType == Usr_CLASS_PHOTO_SEL_SEE ||
                           ClassPhotoType == Usr_CLASS_PHOTO_SEE ||
                           ClassPhotoType == Usr_CLASS_PHOTO_PRN));
   bool ItsMe;
   bool ShowPhoto;
   bool ShowData;
   bool UsrIsTheMsgSender;
   const char *ClassPhoto = "PHOTO18x24";	// Default photo size
   int LengthUsrData = 10;	// Maximum number of characters of user data
   char PhotoURL[PATH_MAX+1];
   struct UsrData UsrDat;

   /***** Show guests, students or teachers? *****/
   switch (RoleInClassPhoto)
     {
      case Rol_ROLE_STUDENT:
	 LstUsrs = &Gbl.Usrs.LstStds;
	 break;
      case Rol_ROLE_TEACHER:
	 LstUsrs = &Gbl.Usrs.LstTchs;
         break;
      default:
	 LstUsrs = &Gbl.Usrs.LstGsts;
         break;
     }

   /***** Set width and height of photos *****/
   switch (ClassPhotoType)
     {
      case Usr_CLASS_PHOTO_SEL:
	 ClassPhoto = "PHOTO18x24";
         LengthUsrData = 10;
         break;
      case Usr_CLASS_PHOTO_SEL_SEE:
      case Usr_CLASS_PHOTO_SEE:
	 ClassPhoto = "PHOTO36x48";
         LengthUsrData = 10;
         break;
      case Usr_CLASS_PHOTO_PRN:
	 ClassPhoto = "PHOTO36x48";
         LengthUsrData = 15;
         break;
     }

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Loop for showing users photos, names and place of birth *****/
   for (NumUsr = 0;
        NumUsr < LstUsrs->NumUsrs; )
     {
      if ((NumUsr % Gbl.Usrs.ClassPhoto.Cols) == 0)
        {
         fprintf (Gbl.F.Out,"<tr>");
         TRIsOpen = true;
        }

      UsrDat.UsrCod = LstUsrs->Lst[NumUsr].UsrCod;
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
        {
         UsrDat.Accepted = LstUsrs->Lst[NumUsr].Accepted;

         ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == UsrDat.UsrCod);
         ShowData = (ItsMe || UsrDat.Accepted ||
                     Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADM ||
                     Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM);

         /***** Begin user's cell *****/
         fprintf (Gbl.F.Out,"<td class=\"CLASSPHOTO\""
                            " style=\"text-align:center;"
                            " vertical-align:bottom;");
         if (ClassPhotoType == Usr_CLASS_PHOTO_SEL &&
             UsrDat.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod)
           {
            UsrIsTheMsgSender = true;
            fprintf (Gbl.F.Out," background-color:%s;",LIGHT_GREEN);
           }
         else
            UsrIsTheMsgSender = false;
         fprintf (Gbl.F.Out,"\">");

         /***** Checkbox to select this user *****/
         if (PutCheckBoxToSelectUsr)
            Usr_PutCheckboxToSelectUser (RoleInClassPhoto,UsrDat.EncryptedUsrCod,UsrIsTheMsgSender);

         /***** Show photo *****/
         ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
         Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                        	               NULL,
                           ClassPhoto,Pho_ZOOM);

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
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_columns;
   unsigned Cols;

   /***** Start selector *****/
   fprintf (Gbl.F.Out,"<select name=\"ColsClassPhoto\""
                      " onchange=\"javascript:document.getElementById('%s').submit();\">",
            Gbl.FormId);

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
            The_ClassFormul[Gbl.Prefs.Theme],Txt_columns);
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
   sprintf (Query,"SELECT COUNT(*) FROM usr_data WHERE UsrCod='%ld'",
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

   Lay_ShowAlert (Lay_WARNING,Txt_No_users_found[Role]);
  }

/*****************************************************************************/
/************************ See stats about the platform ***********************/
/*****************************************************************************/

void Usr_GetAndShowNumUsrsInPlatform (Rol_Role_t Role)
  {
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   unsigned NumUsrs;
   float NumCrssPerUsr;
   float NumUsrsPerCrs;

   /***** Get the number of users belonging to any course *****/
   if (Role == Rol_ROLE_GUEST__)	// Users not beloging to any course
      NumUsrs = Usr_GetNumUsrsNotBelongingToAnyCrs ();
   else
      NumUsrs = Usr_GetNumUsrsBelongingToAnyCrs (Role);

   /***** Get average number of courses per user *****/
   NumCrssPerUsr = (Role == Rol_ROLE_GUEST__) ? 0 :
	                                      Usr_GetNumCrssPerUsr (Role);

   /***** Query the number of users per course *****/
   NumUsrsPerCrs = (Role == Rol_ROLE_GUEST__) ? 0 :
	                                      Usr_GetNumUsrsPerCrs (Role);

   /***** Write the total number of users *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"DAT\" style=\"text-align:right;"
                      " vertical-align:bottom;\">"
                      "%s"
                      "</td>"
                      "<td class=\"DAT\" style=\"text-align:right;"
                      " vertical-align:bottom;\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT\" style=\"text-align:right;"
                      " vertical-align:bottom;\">"
                      "%.2f"
                      "</td>"
                      "<td class=\"DAT\" style=\"text-align:right;"
                      " vertical-align:bottom;\">"
                      "%.2f"
                      "</td>"
                      "</tr>",
            Txt_ROLES_PLURAL_Abc[Role][Usr_SEX_UNKNOWN],
            NumUsrs,
            NumCrssPerUsr,
            NumUsrsPerCrs);
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
/*************** Get number of courses with users of a type ******************/
/*****************************************************************************/

static unsigned Usr_GetNumUsrsBelongingToAnyCrs (Rol_Role_t Role)
  {
   char Query[1024];

   /***** Get number of users who belong to any course *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(DISTINCT UsrCod)"
                        " FROM crs_usr"
                        " WHERE Role='%u'",
                  (unsigned) Role);
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
                        " FROM institutions,centres,degrees,courses,crs_usr"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.Role='%u'",
                  Gbl.CurrentCty.Cty.CtyCod,
                  (unsigned) Role);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
                        " FROM centres,degrees,courses,crs_usr"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.Role='%u'",
                  Gbl.CurrentIns.Ins.InsCod,
                  (unsigned) Role);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
                        " FROM degrees,courses,crs_usr"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.Role='%u'",
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  (unsigned) Role);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
                        " FROM courses,crs_usr"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.Role='%u'",
                  Gbl.CurrentDeg.Deg.DegCod,
                  (unsigned) Role);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
                        " FROM crs_usr"
                        " WHERE crs_usr.CrsCod='%ld'"
                        " AND crs_usr.Role='%u'",
                  Gbl.CurrentCrs.Crs.CrsCod,
                  (unsigned) Role);
         break;
      default:
         Lay_ShowErrorAndExit ("Wrong scope.");
         break;
     }
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of users who belong to any course");
  }

/*****************************************************************************/
/************ Get average number of courses with users of a type *************/
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
         sprintf (Query,"SELECT AVG(NumCrss) FROM "
                        "(SELECT COUNT(CrsCod) AS NumCrss"
                        " FROM crs_usr"
                        " WHERE Role='%u' GROUP BY UsrCod) AS NumCrssTable",
                  (unsigned) Role);
         break;
      case Sco_SCOPE_CTY:
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
         sprintf (Query,"SELECT AVG(NumUsrs) FROM "
                        "(SELECT COUNT(UsrCod) AS NumUsrs"
                        " FROM crs_usr"
                        " WHERE Role='%u' GROUP BY CrsCod) AS NumUsrsTable",
                  (unsigned) Role);
         break;
      case Sco_SCOPE_CTY:
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
         return (float) ((Role == Rol_ROLE_TEACHER) ? Gbl.CurrentCrs.Crs.NumTchs :
                                                      Gbl.CurrentCrs.Crs.NumStds);
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
/************************** Request a user's profile *************************/
/*****************************************************************************/

void Usr_RequestUserProfile (void)
  {
   extern const char *Txt_View_public_profile;
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Nickname;
   extern const char *Txt_Continue;

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,2,Txt_View_public_profile);
   fprintf (Gbl.F.Out,"<tr>"
                      "<td>");

   /***** Form to request user's @nickname *****/
   Act_FormStart (ActSeePubPrf);
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:center;\">"
                      "%s: "
                      "<input type=\"text\" name=\"usr\""
                      " size=\"20\" maxlength=\"%u\" />"
                      "</div>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Nickname,
            Nck_MAX_BYTES_NICKNAME_WITH_ARROBA);

   /***** Send button*****/
   Lay_PutSendButton (Txt_Continue);
   Act_FormEnd ();

   /***** End frame *****/
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/**************** Get user's code and show a user's profile ******************/
/*****************************************************************************/

void Usr_GetUsrCodAndShowUserProfile (void)
  {
   char Nickname[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA + 1];
   long OtherUsrCod;

   /***** Get user from nickname *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod < 0)
     {
      Par_GetParToText ("usr",Nickname,Nck_MAX_BYTES_NICKNAME_WITH_ARROBA);
      if ((OtherUsrCod = Nck_GetUsrCodFromNickname (Nickname)) > 0)
	{
	 Gbl.Usrs.Other.UsrDat.UsrCod = OtherUsrCod;
	 Gbl.CurrentAct = ActSeePubPrf;
	}
      else
	 Usr_GetParamOtherUsrCodEncrypted ();
     }

   /***** Show user's profile *****/
   Usr_ShowUserProfile ();
  }

/*****************************************************************************/
/*************************** Show a user's profile ***************************/
/*****************************************************************************/

static void Usr_ShowUserProfile (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   bool Error = false;

   /***** Check if user exists and get his data *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))        // Existing user
     {
      /***** Check if I can see the public profile *****/
      if (Pri_ShowIsAllowed (Gbl.Usrs.Other.UsrDat.ProfileVisibility,Gbl.Usrs.Other.UsrDat.UsrCod))
	{
	 if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
	   {
	    /* Get user's role in current course */
	    Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB = Rol_GetRoleInCrs (Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Other.UsrDat.UsrCod);

	    /* Get if user has accepted enrollment in current course */
	    Gbl.Usrs.Other.UsrDat.Accepted = Usr_GetIfUserHasAcceptedEnrollmentInCurrentCrs (Gbl.Usrs.Other.UsrDat.UsrCod);
	   }

	 fprintf (Gbl.F.Out,"<div style=\"margin:0 auto;\">"
	                    "<table style=\"margin:0 auto;\">"
	                    "<tr>"
	                    "<td style=\"text-align:right; vertical-align:top;\">");

	 /***** Common record *****/
	 Rec_ShowSharedUsrRecord (Rec_RECORD_PUBLIC,&Gbl.Usrs.Other.UsrDat);

	 fprintf (Gbl.F.Out,"</td>"
	                    "<td style=\"text-align:left; vertical-align:top; padding-left:4px;\">");

	 /***** Show details of user's profile *****/
	 Usr_ShowDetailsUserProfile (&Gbl.Usrs.Other.UsrDat);

	 fprintf (Gbl.F.Out,"</td>"
	                    "</tr>"
	                    "</table>"
	                    "</div>");
	}
      else
	 Error = true;
     }
   else
      Error = true;

   if (Error)
     {
      /***** Show error message *****/
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);

      /***** Request nickname again *****/
      Usr_RequestUserProfile ();
     }
  }

/*****************************************************************************/
/******************** Change my public profile visibility ********************/
/*****************************************************************************/

void Usr_ChangeProfileVisibility (void)
  {
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];
   extern const char *Txt_The_visibility_of_your_public_profile_has_changed;
   char Query[128];

   /***** Get param with public/private photo *****/
   Gbl.Usrs.Me.UsrDat.ProfileVisibility = Pri_GetParamVisibility ();

   /***** Store public/private photo in database *****/
   sprintf (Query,"UPDATE usr_data SET ProfileVisibility='%s'"
	          " WHERE UsrCod='%ld'",
            Pri_VisibilityDB[Gbl.Usrs.Me.UsrDat.ProfileVisibility],
            Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update your preference about public profile visibility");

   /***** Show alert *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_The_visibility_of_your_public_profile_has_changed);

   /***** Show form again *****/
   Pri_EditMyPrivacy ();
  }

/*****************************************************************************/
/********************** Show details of user's profile ***********************/
/*****************************************************************************/

void Usr_ShowDetailsUserProfile (const struct UsrData *UsrDat)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   // extern const char *Txt_Figures;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_course;
   extern const char *Txt_courses;
   extern const char *Txt_From_TIME;
   extern const char *Txt_day;
   extern const char *Txt_days;
   extern const char *Txt_Calculate;
   extern const char *Txt_Clicks;
   extern const char *Txt_of_PART_OF_A_TOTAL;
   extern const char *Txt_clicks;
   extern const char *Txt_Forums;
   extern const char *Txt_post;
   extern const char *Txt_posts;
   extern const char *Txt_Messages;
   extern const char *Txt_message;
   extern const char *Txt_messages;
   extern const char *Txt_Files;
   extern const char *Txt_files;
   extern const char *Txt_public_FILES;
   struct UsrFigures UsrFigures;
   unsigned NumCrssUsrIsTeacher;
   unsigned NumCrssUsrIsStudent;
   unsigned NumStds;
   unsigned NumTchs;
   unsigned NumFiles;
   unsigned NumPublicFiles;

   /***** Start table *****/
   fprintf (Gbl.F.Out,"<table class=\"TABLE10 CELLS_PAD_2\">");

   /***** Number of courses in which the user is teacher or student *****/
   if ((NumCrssUsrIsTeacher = Usr_GetNumCrssOfUsrWithARole (UsrDat->UsrCod,Rol_ROLE_TEACHER)))
     {
      NumTchs = Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Rol_ROLE_TEACHER,Rol_ROLE_TEACHER);
      NumStds = Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Rol_ROLE_TEACHER,Rol_ROLE_STUDENT);
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s\""
			 " style=\"text-align:right; vertical-align:top;\">"
                         "%s"
			 "</td>"
			 "<td class=\"DAT\""
			 " style=\"text-align:left; vertical-align:top;\">"
			 "%u&nbsp;%s<br />"
			 "%u&nbsp;%s<br />"
			 "%u&nbsp;%s"
			 "</a>"
			 "</td>",
	       The_ClassFormul[Gbl.Prefs.Theme],
	       Txt_ROLES_SINGUL_Abc[Rol_ROLE_TEACHER][UsrDat->Sex],
	       NumCrssUsrIsTeacher,
	       (NumCrssUsrIsTeacher == 1) ? Txt_course :
		                            Txt_courses,
	       NumTchs,
	       (NumTchs == 1) ? Txt_ROLES_SINGUL_abc[Rol_ROLE_TEACHER][Usr_SEX_UNKNOWN] :
		                Txt_ROLES_PLURAL_abc[Rol_ROLE_TEACHER][Usr_SEX_UNKNOWN],
	       NumStds,
	       (NumStds == 1) ? Txt_ROLES_SINGUL_abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN] :
		                Txt_ROLES_PLURAL_abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN]);
     }
   if ((NumCrssUsrIsStudent = Usr_GetNumCrssOfUsrWithARole (UsrDat->UsrCod,Rol_ROLE_STUDENT)))
     {
      NumTchs = Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Rol_ROLE_STUDENT,Rol_ROLE_TEACHER);
      NumStds = Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Rol_ROLE_STUDENT,Rol_ROLE_STUDENT);
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s\""
			 " style=\"text-align:right; vertical-align:top;\">"
                         "%s"
			 "</td>"
			 "<td class=\"DAT\""
			 " style=\"text-align:left; vertical-align:top;\">"
			 "%u&nbsp;%s<br />"
			 "%u&nbsp;%s<br />"
			 "%u&nbsp;%s"
			 "</a>"
			 "</td>"
			 "</tr>",
	       The_ClassFormul[Gbl.Prefs.Theme],
	       Txt_ROLES_SINGUL_Abc[Rol_ROLE_STUDENT][UsrDat->Sex],
	       NumCrssUsrIsStudent,
	       (NumCrssUsrIsStudent == 1) ? Txt_course :
	                                    Txt_courses,
	       NumTchs,
	       (NumTchs == 1) ? Txt_ROLES_SINGUL_abc[Rol_ROLE_TEACHER][Usr_SEX_UNKNOWN] :
		                Txt_ROLES_PLURAL_abc[Rol_ROLE_TEACHER][Usr_SEX_UNKNOWN],
	       NumStds,
	       (NumStds == 1) ? Txt_ROLES_SINGUL_abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN] :
		                Txt_ROLES_PLURAL_abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN]);
     }

   /***** Get figures *****/
   Usr_GetUsrFigures (UsrDat->UsrCod,&UsrFigures);

   /* First click time */
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s\""
		      " style=\"text-align:right; vertical-align:top;\">"
		      "%s"
		      "</td>"
		      "<td class=\"DAT\""
		      " style=\"text-align:left; vertical-align:top;\">",
	    The_ClassFormul[Gbl.Prefs.Theme],
	    Txt_From_TIME);
   if (UsrFigures.FirstClickTime.Date.Year)
     {
      Dat_WriteDate (UsrFigures.FirstClickTime.Date.YYYYMMDD);
      if (UsrFigures.NumDays >= 0)
	 fprintf (Gbl.F.Out,"<br />"
	                    "%d&nbsp;%s",
		  UsrFigures.NumDays,
		  (UsrFigures.NumDays == 1) ? Txt_day :
					      Txt_days);
     }
   else	// First click time is unknown or user never logged
     {
      /***** Button to fetch and store first click time *****/
      Act_FormStart (ActCal1stClkTim);
      Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmitAnimated (Txt_Calculate,The_ClassFormul[Gbl.Prefs.Theme],
                                  "calculate1","calculating1");
      Lay_PutCalculateIcon (Txt_Calculate,Txt_Calculate,
                            "calculate1","calculating1");
      Act_FormEnd ();
     }
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /* Number of clicks */
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s\""
		      " style=\"text-align:right; vertical-align:top;\">"
		      "%s"
		      "</td>"
		      "<td class=\"DAT\""
		      " style=\"text-align:left; vertical-align:top;\">",
	    The_ClassFormul[Gbl.Prefs.Theme],
	    Txt_Clicks);
   if (UsrFigures.NumClicks >= 0)
     {
      fprintf (Gbl.F.Out,"%ld&nbsp;%s<br />"
	                 "<span class=\"RANK\">#%ld</span>"
	                 "&nbsp;%s&nbsp;%ld",
               UsrFigures.NumClicks,Txt_clicks,
               Usr_GetRankingNumClicks (UsrDat->UsrCod),
               Txt_of_PART_OF_A_TOTAL,
               Usr_GetNumUsrsWithNumClicks ());
      if (UsrFigures.NumDays >= 0)
	{
	 fprintf (Gbl.F.Out,"<br />");
         Str_WriteFloatNum ((float) UsrFigures.NumClicks /
		            (float) (UsrFigures.NumDays + 1));
	 fprintf (Gbl.F.Out,"&nbsp;/&nbsp;%s<br />"
	                    "<span class=\"RANK\">#%ld</span>"
	                    "&nbsp;%s&nbsp;%ld",
	          Txt_day,
		  Usr_GetRankingNumClicksPerDay (UsrDat->UsrCod),
		  Txt_of_PART_OF_A_TOTAL,
		  Usr_GetNumUsrsWithNumClicksPerDay ());
	}
     }
   else	// Number of clicks is unknown
     {
      /***** Button to fetch and store number of clicks *****/
      Act_FormStart (ActCalNumClk);
      Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmitAnimated (Txt_Calculate,The_ClassFormul[Gbl.Prefs.Theme],
                                  "calculate2","calculating2");
      Lay_PutCalculateIcon (Txt_Calculate,Txt_Calculate,
                            "calculate2","calculating2");
      Act_FormEnd ();
     }
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /***** Number of posts in forums *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s\""
		      " style=\"text-align:right; vertical-align:top;\">"
		      "%s"
		      "</td>"
		      "<td class=\"DAT\""
		      " style=\"text-align:left; vertical-align:top;\">",
	    The_ClassFormul[Gbl.Prefs.Theme],
	    Txt_Forums);
   if (UsrFigures.NumForPst >= 0)
     {
      fprintf (Gbl.F.Out,"%ld&nbsp;%s",
               UsrFigures.NumForPst,
               (UsrFigures.NumForPst == 1) ? Txt_post :
        	                             Txt_posts);
      if (UsrFigures.NumDays >= 0)
	{
	 fprintf (Gbl.F.Out,"<br />");
         Str_WriteFloatNum ((float) UsrFigures.NumForPst /
		            (float) (UsrFigures.NumDays + 1));
	 fprintf (Gbl.F.Out,"&nbsp;/&nbsp;%s",Txt_day);
	}
     }
   else	// Number of forum posts is unknown
     {
      /***** Button to fetch and store number of forum posts *****/
      Act_FormStart (ActCalNumForPst);
      Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmitAnimated (Txt_Calculate,The_ClassFormul[Gbl.Prefs.Theme],
                                  "calculate3","calculating3");
      Lay_PutCalculateIcon (Txt_Calculate,Txt_Calculate,
                            "calculate3","calculating3");
      Act_FormEnd ();
     }
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /***** Number of messages sent *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s\""
		      " style=\"text-align:right; vertical-align:top;\">"
		      "%s"
		      "</td>"
		      "<td class=\"DAT\""
		      " style=\"text-align:left; vertical-align:top;\">",
	    The_ClassFormul[Gbl.Prefs.Theme],
	    Txt_Messages);
   if (UsrFigures.NumMsgSnt >= 0)
     {
      fprintf (Gbl.F.Out,"%ld&nbsp;%s",
               UsrFigures.NumMsgSnt,
               (UsrFigures.NumMsgSnt == 1) ? Txt_message :
        	                             Txt_messages);
      if (UsrFigures.NumDays >= 0)
	{
	 fprintf (Gbl.F.Out,"<br />");
         Str_WriteFloatNum ((float) UsrFigures.NumMsgSnt /
		            (float) (UsrFigures.NumDays + 1));
	 fprintf (Gbl.F.Out,"&nbsp;/&nbsp;%s",Txt_day);
	}
     }
   else	// Number of clicks is unknown
     {
      /***** Button to fetch and store number of messages sent *****/
      Act_FormStart (ActCalNumMsgSnt);
      Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmitAnimated (Txt_Calculate,The_ClassFormul[Gbl.Prefs.Theme],
                                  "calculate4","calculating4");
      Lay_PutCalculateIcon (Txt_Calculate,Txt_Calculate,
                            "calculate4","calculating4");
      Act_FormEnd ();
     }
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /***** Number of files published *****/
   if ((NumFiles = Brw_GetNumFilesUsr (UsrDat->UsrCod)))
      NumPublicFiles = Brw_GetNumPublicFilesUsr (UsrDat->UsrCod);
   else
      NumPublicFiles = 0;
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s\""
		      " style=\"text-align:right; vertical-align:top;\">"
		      "%s"
		      "</td>"
		      "<td class=\"DAT\""
		      " style=\"text-align:left; vertical-align:top;\">"
		      "%u&nbsp;%s<br />"
		      "%u&nbsp;%s"
		      "</a>"
		      "</td>"
		      "</tr>",
	    The_ClassFormul[Gbl.Prefs.Theme],
	    Txt_Files,
	    NumFiles,Txt_files,
	    NumPublicFiles,Txt_public_FILES);

   /***** End of table *****/
   fprintf (Gbl.F.Out,"</table>");
  }

/*****************************************************************************/
/********************** Select values on user's figures **********************/
/*****************************************************************************/

static void Usr_GetUsrFigures (long UsrCod,struct UsrFigures *UsrFigures)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;

   /***** Get user's code from database *****/
   sprintf (Query,"SELECT DATE_FORMAT(FirstClickTime,'%%Y%%m%%d%%H%%i%%S'),"
	          "DATEDIFF(NOW(),FirstClickTime),"
	          "NumClicks,NumForPst,NumMsgSnt"
	          " FROM usr_figures WHERE UsrCod='%ld'",
	    UsrCod);
   if ((NumRows = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get user's figures")))
     {
      /***** Get user's figures *****/
      row = mysql_fetch_row (mysql_res);

      /* Get first click (row[0] holds the start date in YYYYMMDDHHMMSS format) */
      if (!(Dat_GetDateTimeFromYYYYMMDDHHMMSS (&(UsrFigures->FirstClickTime),row[0])))
	 Lay_ShowErrorAndExit ("Error when reading first click time.");

      /* Get number of days since first click (row[1]) */
      if (UsrFigures->FirstClickTime.Date.Year)
	{
	 if (sscanf (row[1],"%d",&UsrFigures->NumDays) != 1)
	    UsrFigures->NumDays = -1;
	}
      else
	 UsrFigures->NumDays = -1;

      /* Get number of clicks (row[2]) */
      if (sscanf (row[2],"%ld",&UsrFigures->NumClicks) != 1)
	 UsrFigures->NumClicks = -1L;

      /* Get number of forum posts (row[3]) */
      if (sscanf (row[3],"%ld",&UsrFigures->NumForPst) != 1)
	 UsrFigures->NumForPst = -1L;

      /* Get number of messages sent (row[4]) */
      if (sscanf (row[4],"%ld",&UsrFigures->NumMsgSnt) != 1)
	 UsrFigures->NumMsgSnt = -1L;
     }
   else
      /***** Return special user's figures indicating "not present" *****/
      Usr_ResetUsrFigures (UsrFigures);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Get ranking of a user according to the number of clicks **********/
/*****************************************************************************/

static unsigned long Usr_GetRankingNumClicks (long UsrCod)
  {
   char Query[128];

   /***** Select number of rows with number of clicks
          greater than the clicks of this user *****/
   sprintf (Query,"SELECT COUNT(*)+1 FROM usr_figures"
	          " WHERE UsrCod<>'%ld'"	// Really not necessary here
                  " AND NumClicks>"
	          "(SELECT NumClicks FROM usr_figures WHERE UsrCod='%ld')",
	    UsrCod,UsrCod);
   return DB_QueryCOUNT (Query,"can not get ranking using number of clicks");
  }

/*****************************************************************************/
/******************* Get number of users with number of clicks ***************/
/*****************************************************************************/

static unsigned long Usr_GetNumUsrsWithNumClicks (void)
  {
   char Query[128];

   /***** Select number of rows with values already calculated *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_figures WHERE NumClicks>='0'");
   return DB_QueryCOUNT (Query,"can not get number of users with number of clicks");
  }

/*****************************************************************************/
/****** Get ranking of a user according to the number of clicks per day ******/
/*****************************************************************************/

static unsigned long Usr_GetRankingNumClicksPerDay (long UsrCod)
  {
   char Query[512];

   /***** Select number of rows with number of clicks per day
          greater than the clicks per day of this user *****/
   sprintf (Query,"SELECT COUNT(*)+1 FROM"
                  " (SELECT NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1)"
                  " AS NumClicksPerDay"
                  " FROM usr_figures"
                  " WHERE UsrCod<>'%ld'"	// Necessary because the following comparison is not exact in floating point
                  " AND NumClicks>='0' AND UNIX_TIMESTAMP(FirstClickTime)>'0')"
                  " AS TableNumClicksPerDay"
                  " WHERE NumClicksPerDay>"
                  "(SELECT NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1)"
                  " FROM usr_figures"
                  " WHERE UsrCod='%ld'"
                  " AND NumClicks>='0' AND UNIX_TIMESTAMP(FirstClickTime)>'0')",
	    UsrCod,UsrCod);
   return DB_QueryCOUNT (Query,"can not get ranking using number of clicks per day");
  }

/*****************************************************************************/
/************** Get number of users with number of clicks per day ************/
/*****************************************************************************/

static unsigned long Usr_GetNumUsrsWithNumClicksPerDay (void)
  {
   char Query[128];

   /***** Select number of rows with values already calculated *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_figures"
	          " WHERE NumClicks>='0' AND UNIX_TIMESTAMP(FirstClickTime)>'0'");
   return DB_QueryCOUNT (Query,"can not get number of users with number of clicks per day");
  }

/*****************************************************************************/
/********* Calculate first click time and show user's profile again **********/
/*****************************************************************************/

void Usr_CalculateFirstClickTime (void)
  {
   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Get first click time from log and store as user's figure *****/
   Usr_GetFirstClickFromLogAndStoreAsUsrFigure (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show user's profile again *****/
   Usr_ShowUserProfile ();
  }

/*****************************************************************************/
/*** Get first click of a user from log table and store in user's figures ****/
/*****************************************************************************/

static void Usr_GetFirstClickFromLogAndStoreAsUsrFigure (long UsrCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Usr_ResetUsrFigures (&UsrFigures);

      /***** Get first click from log table *****/
      sprintf (Query,"SELECT DATE_FORMAT("
	             "(SELECT MIN(ClickTime) FROM log WHERE UsrCod='%ld')"
	             ",'%%Y%%m%%d%%H%%i%%S')",
	       UsrCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get user's first click"))
	{
	 /* Get first click */
	 row = mysql_fetch_row (mysql_res);

	 /* Get first click (row[0] holds the start date in YYYYMMDDHHMMSS format) */
	 if (row[0])	// It is NULL when user never logged
	    if (!(Dat_GetDateTimeFromYYYYMMDDHHMMSS (&(UsrFigures.FirstClickTime),row[0])))
	       Lay_ShowErrorAndExit ("Error when reading first click time.");
	}
      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

      /***** Update first click time in user's figures *****/
      if (Usr_CheckIfUsrFiguresExists (UsrCod))
	{
	 sprintf (Query,"UPDATE usr_figures SET FirstClickTime='%s'"
			" WHERE UsrCod='%ld'",
		  UsrFigures.FirstClickTime.YYYYMMDDHHMMSS,UsrCod);
	 DB_QueryUPDATE (Query,"can not update user's figures");
	}
      else			// User entry does not exist
	 Usr_CreateUsrFigures (UsrCod,&UsrFigures);
     }
  }

/*****************************************************************************/
/********* Calculate number of clicks and show user's profile again **********/
/*****************************************************************************/

void Usr_CalculateNumClicks (void)
  {
   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Get number of clicks and store as user's figure *****/
   Usr_GetNumClicksAndStoreAsUsrFigure (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show user's profile again *****/
   Usr_ShowUserProfile ();
  }

/*****************************************************************************/
/* Get number of clicks of a user from log table and store in user's figures */
/*****************************************************************************/

static void Usr_GetNumClicksAndStoreAsUsrFigure (long UsrCod)
  {
   char Query[256];
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Usr_ResetUsrFigures (&UsrFigures);

      /***** Get number of clicks from database *****/
      sprintf (Query,"SELECT COUNT(*) FROM log WHERE UsrCod='%ld'",
	       UsrCod);
      UsrFigures.NumClicks = (long) DB_QueryCOUNT (Query,"can not get number of clicks");

      /***** Update number of clicks in user's figures *****/
      if (Usr_CheckIfUsrFiguresExists (UsrCod))
	{
	 sprintf (Query,"UPDATE usr_figures SET NumClicks='%ld'"
			" WHERE UsrCod='%ld'",
		  UsrFigures.NumClicks,UsrCod);
	 DB_QueryUPDATE (Query,"can not update user's figures");
	}
      else			// User entry does not exist
	 Usr_CreateUsrFigures (UsrCod,&UsrFigures);
     }
   }

/*****************************************************************************/
/******* Calculate number of forum posts and show user's profile again *******/
/*****************************************************************************/

void Usr_CalculateNumForPst (void)
  {
   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Get number of forum posts and store as user's figure *****/
   Usr_GetNumForPstAndStoreAsUsrFigure (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show user's profile again *****/
   Usr_ShowUserProfile ();
  }

/*****************************************************************************/
/**** Get number of forum posts sent by a user and store in user's figures ***/
/*****************************************************************************/

static void Usr_GetNumForPstAndStoreAsUsrFigure (long UsrCod)
  {
   char Query[256];
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Usr_ResetUsrFigures (&UsrFigures);

      /***** Get number of forum posts from database *****/
      UsrFigures.NumForPst = For_GetNumPostsUsr (UsrCod);

      /***** Update number of forum posts in user's figures *****/
      if (Usr_CheckIfUsrFiguresExists (UsrCod))
	{
	 sprintf (Query,"UPDATE usr_figures SET NumForPst='%ld'"
			" WHERE UsrCod='%ld'",
		  UsrFigures.NumForPst,UsrCod);
	 DB_QueryUPDATE (Query,"can not update user's figures");
	}
      else			// User entry does not exist
	 Usr_CreateUsrFigures (UsrCod,&UsrFigures);
     }
   }

/*****************************************************************************/
/****** Calculate number of messages sent and show user's profile again ******/
/*****************************************************************************/

void Usr_CalculateNumMsgSnt (void)
  {
   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Get number of messages sent and store as user's figure *****/
   Usr_GetNumMsgSntAndStoreAsUsrFigure (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show user's profile again *****/
   Usr_ShowUserProfile ();
  }

/*****************************************************************************/
/***** Get number of messages sent by a user and store in user's figures *****/
/*****************************************************************************/

static void Usr_GetNumMsgSntAndStoreAsUsrFigure (long UsrCod)
  {
   char Query[256];
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Usr_ResetUsrFigures (&UsrFigures);

      /***** Get number of messages sent from database *****/
      UsrFigures.NumMsgSnt = Msg_GetNumMsgsSentByUsr (UsrCod);

      /***** Update number of messages sent in user's figures *****/
      if (Usr_CheckIfUsrFiguresExists (UsrCod))
	{
	 sprintf (Query,"UPDATE usr_figures SET NumMsgSnt='%ld'"
			" WHERE UsrCod='%ld'",
		  UsrFigures.NumMsgSnt,UsrCod);
	 DB_QueryUPDATE (Query,"can not update user's figures");
	}
      else			// User entry does not exist
	 Usr_CreateUsrFigures (UsrCod,&UsrFigures);
     }
  }

/*****************************************************************************/
/********************** Reset values of user's figures ***********************/
/*****************************************************************************/

static void Usr_ResetUsrFigures (struct UsrFigures *UsrFigures)
  {
   Dat_GetDateTimeFromYYYYMMDDHHMMSS (&(UsrFigures->FirstClickTime),"00000000000000");	//  unknown first click time or user never logged
   UsrFigures->NumDays   = -1;		// not applicable
   UsrFigures->NumClicks = -1L;		// unknown number of clicks
   UsrFigures->NumForPst = -1L;		// unknown number of forum posts
   UsrFigures->NumMsgSnt = -1L;		// unknown number of messages sent
  }

/*****************************************************************************/
/***** Get number of messages sent by a user and store in user's figures *****/
/*****************************************************************************/

static void Usr_CreateUsrFigures (long UsrCod,const struct UsrFigures *UsrFigures)
  {
   char Query[256];

   /***** Create user's figures *****/
   sprintf (Query,"INSERT INTO usr_figures (UsrCod,FirstClickTime,NumClicks,NumForPst,NumMsgSnt)"
		  " VALUES ('%ld','%s','%ld','%ld','%ld')",
	    UsrCod,
	    UsrFigures->FirstClickTime.YYYYMMDDHHMMSS,	//   0 ==> unknown first click time or user never logged
	    UsrFigures->NumClicks,			// -1L ==> unknown number of clicks
	    UsrFigures->NumForPst,			// -1L ==> unknown number of forum posts
	    UsrFigures->NumMsgSnt);			// -1L ==> unknown number of messages sent
   DB_QueryINSERT (Query,"can not create user's figures");
  }

/*****************************************************************************/
/**************************** Remove user's figures **************************/
/*****************************************************************************/

void Usr_RemoveUsrFigures (long UsrCod)
  {
   char Query[128];

   /***** Remove user's figures *****/
   sprintf (Query,"DELETE FROM usr_figures WHERE UsrCod='%ld'",
	    UsrCod);
   DB_QueryDELETE (Query,"can not delete user's figures");
  }

/*****************************************************************************/
/*** Check if it exists an entry for this user in table of user's figures ****/
/*****************************************************************************/

static bool Usr_CheckIfUsrFiguresExists (long UsrCod)
  {
   char Query[128];

   sprintf (Query,"SELECT COUNT(*) FROM usr_figures WHERE UsrCod='%ld'",
	    UsrCod);
   return (DB_QueryCOUNT (Query,"can not get user's first click") != 0);
  }

/*****************************************************************************/
/*************** Increment number of clicks made by a user *******************/
/*****************************************************************************/

void Usr_IncrementNumClicksUsr (long UsrCod)
  {
   char Query[256];

   /***** Increment number of clicks *****/
   // If NumClicks < 0 ==> not yet calculated, so do nothing
   sprintf (Query,"UPDATE IGNORE usr_figures SET NumClicks=NumClicks+1"
	          " WHERE UsrCod='%ld' AND NumClicks>=0",
	    UsrCod);
   DB_QueryINSERT (Query,"can not increment user's clicks");
  }

/*****************************************************************************/
/************* Increment number of forum posts sent by a user ****************/
/*****************************************************************************/

void Usr_IncrementNumForPstUsr (long UsrCod)
  {
   char Query[256];

   /***** Increment number of forum posts *****/
   // If NumForPst < 0 ==> not yet calculated, so do nothing
   sprintf (Query,"UPDATE IGNORE usr_figures SET NumForPst=NumForPst+1"
	          " WHERE UsrCod='%ld' AND NumForPst>=0",
	    UsrCod);
   DB_QueryINSERT (Query,"can not increment user's forum posts");
  }

/*****************************************************************************/
/*************** Increment number of messages sent by a user *****************/
/*****************************************************************************/

void Usr_IncrementNumMsgSntUsr (long UsrCod)
  {
   char Query[256];

   /***** Increment number of messages sent *****/
   // If NumMsgSnt < 0 ==> not yet calculated, so do nothing
   sprintf (Query,"UPDATE IGNORE usr_figures SET NumMsgSnt=NumMsgSnt+1"
	          " WHERE UsrCod='%ld' AND NumMsgSnt>=0",
	    UsrCod);
   DB_QueryINSERT (Query,"can not increment user's messages sent");
  }
