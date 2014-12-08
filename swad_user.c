// swad_user.c: users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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

#include "swad_announcement.h"
#include "swad_config.h"
#include "swad_connected.h"
#include "swad_course.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_ID.h"
#include "swad_nickname.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_password.h"
#include "swad_preference.h"
#include "swad_record.h"
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

/* Special codes for global administrator and superuser deg_admin table */
#define Usr_SPECIAL_CODE_GLOBAL_ADMIN	-1L
#define Usr_SPECIAL_CODE_SUPERUSER	-2L

#define Usr_NUM_MAIN_FIELDS_DATA_USR	 9
#define Usr_NUM_MAIN_FIELDS_DATA_ADM	 8
#define Usr_NUM_ALL_FIELDS_DATA_INV	17
#define Usr_NUM_ALL_FIELDS_DATA_STD	13
#define Usr_NUM_ALL_FIELDS_DATA_TCH	11
const char *Usr_UsrDatMainFieldNames[Usr_NUM_MAIN_FIELDS_DATA_USR];

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

#define Usr_NUM_ACTIONS_REG_REM_ONE_USR 5
typedef enum
  {
   Usr_REGISTER_MODIFY_ONE_USR_IN_CRS  = 0,
   Usr_REGISTER_ONE_DEGREE_ADMIN       = 1,
   Usr_REMOVE_ONE_USR_FROM_CRS         = 2,
   Usr_REMOVE_ONE_DEGREE_ADMIN         = 3,
   Usr_ELIMINATE_ONE_USR_FROM_PLATFORM = 4,
  } Usr_RegRemOneUsrAction_t;

#define Usr_NUM_ACTIONS_REG_REM_USRS 5
typedef enum
  {
   Usr_REGISTER_SPECIFIED_USRS_IN_CRS     = 0,
   Usr_REMOVE_SPECIFIED_USRS_FROM_CRS     = 1,
   Usr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS = 2,
   Usr_UPDATE_USRS_IN_CRS                 = 3,
   Usr_ELIMINATE_USRS_FROM_PLATFORM       = 4,
  } Usr_RegRemUsrsAction_t;

typedef enum
  {
   Usr_DO_NOT_REMOVE_WORKS,
   Usr_REMOVE_WORKS,
  } Usr_RemoveUsrWorks_t;

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;
extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
const char *Txt_RECORD_FIELD_VISIBILITY_RECORD[Rec_NUM_TYPES_VISIBILITY];

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Usr_GetMyLastData (void);
static void Usr_GetUsrCommentsFromString (char *Str,struct UsrData *UsrDat);
static Usr_Sex_t Usr_GetSexFromStr (const char *Str);
static Rol_Role_t Usr_GetRoleInCrs (long CrsCod,long UsrCod);
static void Usr_ModifyRoleInCurrentCrs (struct UsrData *UsrDat,
                                        Rol_Role_t NewRole,
                                        Cns_QuietOrVerbose_t QuietOrVerbose,
                                        Usr_KeepOrSetAccepted_t KeepOrSetAccepted);

static void Usr_PutFormToRemOldUsrs (void);

static bool Usr_ChkIfEncryptedUsrCodExists (const char *EncryptedUsrCod);

static bool Usr_CheckIfMyBirthdayHasNotBeenCongratulated (void);
static void Usr_InsertMyBirthday (void);
static unsigned Usr_GetNumAvailableRoles (void);
static void Usr_PutFormToChangeMyRole (bool FormInHead);

static void Usr_GetParamOtherUsrIDNickOrEMail (void);
static unsigned Usr_GetParamOtherUsrIDNickOrEMailAndGetUsrCods (struct ListUsrCods *ListUsrCods);

static bool Usr_ChkUsrAndGetUsrDataFromDirectLogin (void);
static bool Usr_ChkUsrAndGetUsrDataFromExternalLogin (void);
static bool Usr_ChkUsrAndGetUsrDataFromSession (void);

static bool Usr_CreateNewAccountAndLogIn (void);
static void Usr_SetUsrRoleAndPrefs (void);
static Rol_Role_t Usr_GetMaxRole (unsigned Roles);

static void Usr_ShowFormRequestNewAccountWithParams (const char *NewNicknameWithoutArroba,
                                                     const char *NewEmail);
static bool Usr_GetParamsNewAccount (char *NewNicknameWithoutArroba,
                                     char *NewEmail,
                                     char *NewEncryptedPassword);
static void Usr_PutLinkToRemoveMyAccount (void);
static void Usr_PrintAccountSeparator (void);

static void Usr_InsertMyLastData (void);

static void Usr_CreateNewEncryptedUsrCod (struct UsrData *UsrDat);
static void Usr_FilterUsrDat (struct UsrData *UsrDat);
static void Usr_RegisterAdminInCurrentDeg (struct UsrData *UsrDat);
static void Usr_EffectivelyRemUsrFromCrs (struct UsrData *UsrDat,struct Course *Crs,
                                          Usr_RemoveUsrWorks_t RemoveUsrWorks,Cns_QuietOrVerbose_t QuietOrVerbose);
static void Usr_EffectivelyRemAdmFromDeg (struct UsrData *UsrDat);
static void Usr_DeleteUsrFromPlatform (struct UsrData *UsrDat,
                                       Cns_QuietOrVerbose_t QuietOrVerbose);
static void Usr_RemoveUsr (struct UsrData *UsrDat);

static void Usr_WriteRowGstMainData (unsigned NumUsr,struct UsrData *UsrDat);
static void Usr_WriteRowTchMainData (unsigned NumUsr,struct UsrData *UsrDat,bool PutCheckboxToSelectUsr);
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

static void Usr_PutAreaToEnterUsrsIDs (void);
static bool Usr_PutActionsRegRemOneUsr (bool ItsMe);
static bool Usr_CheckIfICanEliminateAccount (bool ItsMe);
static void Usr_PutActionsRegRemSeveralUsrs (void);
static void Usr_PutAllRolesRegRemUsrsCrs (void);
static void Usr_PutOneRoleRegRemUsrsCrs (Rol_Role_t Role,bool Checked);

static void Usr_RegisterUsr (struct UsrData *UsrDat,Rol_Role_t RegRemRole,
                             struct ListCodGrps *LstGrps,unsigned *NumUsrsRegistered);
static void Usr_MarkOfficialStdsAsRemovable (long ImpGrpCod,bool RemoveSpecifiedUsrs);

static void Usr_PutFormToRemAllStdsThisCrs (void);
static void Usr_RemoveEnrollmentRequest (long CrsCod,long UsrCod);

static void Usr_ReqAnotherUsrIDToRegisterRemove (void);
static void Usr_AskIfRegRemMe (void);
static void Usr_AskIfRegRemUsr (struct ListUsrCods *ListUsrCods);

static void Usr_ShowFormToEditOtherUsr (void);

static void Usr_ReqRemAdmOfDeg (void);
static void Usr_ReqRemOrRemAdm (Usr_ReqDelOrDelUsr_t ReqDelOrDelUsr);

static void Usr_AskIfRemoveUsrFromCrs (struct UsrData *UsrDat,bool ItsMe);
static void Usr_AskIfCompletelyEliminateUsrFromPlatform (bool ItsMe);
static void Usr_ReqAddAdmOfDeg (void);

static void Usr_ConstructPathUsr (long UsrCod,char *PathUsr);

static unsigned Usr_GetNumUsrsNotBelongingToAnyCrs (void);
static unsigned Usr_GetNumUsrsBelongingToAnyCrs (Rol_Role_t Role);
static float Usr_GetNumCrssPerUsr (Rol_Role_t Role);
static float Usr_GetNumUsrsPerCrs (Rol_Role_t Role);

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
      if (Gbl.Usrs.Me.NumAccWithoutPhoto >= Usr_MAX_CLICKS_WITHOUT_PHOTO)
         Lay_ShowAlert (Lay_WARNING,Txt_You_must_send_your_photo_because_);
      else if (Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW)
        {
         sprintf (Message,Txt_You_can_only_perform_X_further_actions_,
                  Usr_MAX_CLICKS_WITHOUT_PHOTO-Gbl.Usrs.Me.NumAccWithoutPhoto);
         Lay_ShowAlert (Lay_WARNING,Message);

	 fprintf (Gbl.F.Out,"<div align=\"center\">");
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
   UsrDat->PublicPhoto = false;

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
   UsrDat->Prefs.SideCols = 3;
   UsrDat->Prefs.IconSet = Ico_ICON_SET_DEFAULT;
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
/********************** Get role from unsigned string ************************/
/*****************************************************************************/

Rol_Role_t Usr_ConvertUnsignedStrToRole (const char *UnsignedStr)
  {
   unsigned UnsignedNum;

   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      return (UnsignedNum >= Rol_NUM_ROLES) ? Rol_ROLE_UNKNOWN :
                                              (Rol_Role_t) UnsignedNum;
   return Rol_ROLE_UNKNOWN;
  }

/*****************************************************************************/
/****** Get roles (several bits can be activated) from unsigned string *******/
/*****************************************************************************/

unsigned Usr_ConvertUnsignedStrToRoles (const char *UnsignedStr)
  {
   unsigned UnsignedNum;

   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      return UnsignedNum;
   return 0;
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
   char YN[1+1];
   char StrBirthday[4+1+2+1+2+1];
   unsigned UnsignedNum;

   /***** Get user's data from database *****/
   sprintf (Query,"SELECT EncryptedUsrCod,Password,Surname1,Surname2,FirstName,Sex,"
                  "Layout,Theme,IconSet,Language,Photo,PublicPhoto,"
                  "CtyCod,InsCtyCod,InsCod,DptCod,CtrCod,Office,OfficePhone,"
                  "LocalAddress,LocalPhone,FamilyAddress,FamilyPhone,OriginPlace,Birthday,Comments,"
                  "SideCols,NotifNtfEvents,EmailNtfEvents"
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
   UsrDat->RoleInCurrentCrsDB = Usr_GetRoleInCrs (Gbl.CurrentCrs.Crs.CrsCod,UsrDat->UsrCod);
   UsrDat->Roles = Usr_GetRolesInAllCrss (UsrDat->UsrCod);
   if (UsrDat->RoleInCurrentCrsDB == Rol_ROLE_UNKNOWN)
      UsrDat->RoleInCurrentCrsDB = (UsrDat->Roles < (1 << Rol_ROLE_STUDENT)) ?
	                           Rol_ROLE_GUEST :	// User does not belong to any course
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
   strncpy (UsrDat->Photo            ,row[10],sizeof (UsrDat->Photo            )-1);
   strncpy (YN                       ,row[11],1);
   UsrDat->PublicPhoto = (Str_ConvertToUpperLetter (YN[0]) == 'Y');
   UsrDat->CtyCod = Str_ConvertStrCodToLongCod (row[12]);
   UsrDat->InsCtyCod = Str_ConvertStrCodToLongCod (row[13]);
   UsrDat->InsCod = Str_ConvertStrCodToLongCod (row[14]);

   UsrDat->Tch.DptCod = Str_ConvertStrCodToLongCod (row[15]);
   UsrDat->Tch.CtrCod = Str_ConvertStrCodToLongCod (row[16]);
   strncpy (UsrDat->Tch.Office       ,row[17],sizeof (UsrDat->Tch.Office       )-1);
   strncpy (UsrDat->Tch.OfficePhone  ,row[18],sizeof (UsrDat->Tch.OfficePhone  )-1);

   strncpy (UsrDat->LocalAddress ,row[19],sizeof (UsrDat->LocalAddress )-1);
   strncpy (UsrDat->LocalPhone   ,row[20],sizeof (UsrDat->LocalPhone   )-1);
   strncpy (UsrDat->FamilyAddress,row[21],sizeof (UsrDat->FamilyAddress)-1);
   strncpy (UsrDat->FamilyPhone  ,row[22],sizeof (UsrDat->FamilyPhone  )-1);
   strncpy (UsrDat->OriginPlace  ,row[23],sizeof (UsrDat->OriginPlace  )-1);
   strcpy (StrBirthday,
           row[24] ? row[24] :
	             "0000-00-00");
   Usr_GetUsrCommentsFromString (row[25] ? row[25] :
	                                   "",
	                         UsrDat);        // Get the comments comunes a todas the courses

   /* Get if user wants to show side columns */
   if (sscanf (row[26],"%u",&UsrDat->Prefs.SideCols) == 1)
     {
      if (UsrDat->Prefs.SideCols > 3)
         UsrDat->Prefs.SideCols = 3;	// Show both side columns
     }
   else
      UsrDat->Prefs.SideCols = 3;	// Show both side columns

   /* Get on which events I want to be notified by e-mail */
   if (sscanf (row[27],"%u",&UsrDat->Prefs.NotifNtfEvents) != 1)
      UsrDat->Prefs.NotifNtfEvents = (unsigned) -1;	// 0xFF..FF

   if (sscanf (row[28],"%u",&UsrDat->Prefs.EmailNtfEvents) != 1)
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
         if (UnsignedNum >= 1 || UnsignedNum <= Act_NUM_TABS)
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
/******************* Get my maximum role in a institution ********************/
/*****************************************************************************/

Rol_Role_t Usr_GetMyMaxRoleInIns (long InsCod)
  {
   unsigned NumMyIns;

   if (InsCod > 0)
     {
      /***** Fill the list with the institutions I belong to (if not already filled) *****/
      Usr_GetMyInstitutions ();

      /***** Check if the institution passed as parameter is any of my institutions *****/
      for (NumMyIns = 0;
           NumMyIns < Gbl.Usrs.Me.MyInstitutions.Num;
           NumMyIns++)
         if (Gbl.Usrs.Me.MyInstitutions.Inss[NumMyIns].InsCod == InsCod)
            return Gbl.Usrs.Me.MyInstitutions.Inss[NumMyIns].MaxRole;
      return Rol_ROLE_GUEST;
     }
   return Rol_ROLE_UNKNOWN;   // No degree
  }

/*****************************************************************************/
/********************** Get my maximum role in a centre **********************/
/*****************************************************************************/

Rol_Role_t Usr_GetMyMaxRoleInCtr (long CtrCod)
  {
   unsigned NumMyCtr;

   if (CtrCod > 0)
     {
      /***** Fill the list with the centres I belong to (if not already filled) *****/
      Usr_GetMyCentres ();

      /***** Check if the centre passed as parameter is any of my centres *****/
      for (NumMyCtr = 0;
           NumMyCtr < Gbl.Usrs.Me.MyCentres.Num;
           NumMyCtr++)
         if (Gbl.Usrs.Me.MyCentres.Ctrs[NumMyCtr].CtrCod == CtrCod)
            return Gbl.Usrs.Me.MyCentres.Ctrs[NumMyCtr].MaxRole;
      return Rol_ROLE_GUEST;
     }
   return Rol_ROLE_UNKNOWN;   // No centre
  }

/*****************************************************************************/
/********************** Get my maximum role in a degree **********************/
/*****************************************************************************/

Rol_Role_t Usr_GetMyMaxRoleInDeg (long DegCod)
  {
   unsigned NumMyDeg;

   if (DegCod > 0)
     {
      /***** Fill the list with the degrees I belong to (if not already filled) *****/
      Usr_GetMyDegrees ();

      /***** Check if the degree passed as parameter is any of my degrees *****/
      for (NumMyDeg = 0;
           NumMyDeg < Gbl.Usrs.Me.MyDegrees.Num;
           NumMyDeg++)
         if (Gbl.Usrs.Me.MyDegrees.Degs[NumMyDeg].DegCod == DegCod)
            return Gbl.Usrs.Me.MyDegrees.Degs[NumMyDeg].MaxRole;
      return Rol_ROLE_GUEST;
     }
   return Rol_ROLE_UNKNOWN;   // No degree
  }

/*****************************************************************************/
/*************************** Get my role in a course *************************/
/*****************************************************************************/

Rol_Role_t Usr_GetMyRoleInCrs (long CrsCod)
  {
   unsigned NumMyCrs;

   if (CrsCod > 0)
     {
      /***** Fill the list with the courses I belong to (if not already filled) *****/
      Usr_GetMyCourses ();

      /***** Check if the course passed as parameter is any of my courses *****/
      for (NumMyCrs = 0;
           NumMyCrs < Gbl.Usrs.Me.MyCourses.Num;
           NumMyCrs++)
         if (Gbl.Usrs.Me.MyCourses.Crss[NumMyCrs].CrsCod == CrsCod)
            return Gbl.Usrs.Me.MyCourses.Crss[NumMyCrs].Role;
      return Rol_ROLE_GUEST;
     }
   return Rol_ROLE_UNKNOWN;   // No course
  }

/*****************************************************************************/
/********************** Get role of a user in a course ***********************/
/*****************************************************************************/

static Rol_Role_t Usr_GetRoleInCrs (long CrsCod,long UsrCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Rol_Role_t Role;

   if (CrsCod > 0)
     {
      /***** Get rol of a user in a course from database.
             The result of the query will have one row or none *****/
      sprintf (Query,"SELECT Role FROM crs_usr"
                     " WHERE CrsCod='%ld' AND UsrCod='%ld'",
               CrsCod,UsrCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get the role of a user in a course") == 1)        // User belongs to the course
        {
         row = mysql_fetch_row (mysql_res);
         Role = Usr_ConvertUnsignedStrToRole (row[0]);
        }
      else                // User does not belong to the course
         Role = Rol_ROLE_UNKNOWN;

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else        // No course
      Role = Rol_ROLE_UNKNOWN;

   return Role;
  }

/*****************************************************************************/
/**************** Get roles of a user in all his/her courses *****************/
/*****************************************************************************/

unsigned Usr_GetRolesInAllCrss (long UsrCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRole;
   unsigned NumRoles;
   Rol_Role_t Role;
   unsigned Roles = 0;

   /***** Get distinct roles in all the courses of the user from database *****/
   sprintf (Query,"SELECT DISTINCT(Role) FROM crs_usr"
                  " WHERE UsrCod='%ld'",
            UsrCod);
   NumRoles = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get the roles of a user in all his/her courses");
   for (NumRole = 0;
        NumRole < NumRoles;
        NumRole++)
     {
      row = mysql_fetch_row (mysql_res);
      if ((Role = Usr_ConvertUnsignedStrToRole (row[0])) != Rol_ROLE_UNKNOWN)
         Roles |= (1 << Role);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Roles;
  }

/*****************************************************************************/
/***************** Modify the role of a user in a course *********************/
/*****************************************************************************/

static void Usr_ModifyRoleInCurrentCrs (struct UsrData *UsrDat,
                                        Rol_Role_t NewRole,
                                        Cns_QuietOrVerbose_t QuietOrVerbose,
                                        Usr_KeepOrSetAccepted_t KeepOrSetAccepted)
  {
   extern const char *Txt_The_role_of_THE_USER_X_in_the_course_Y_has_changed_from_A_to_B;
   extern const char *Txt_ROLES_SINGULAR_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char Query[256];
   Rol_Role_t OldRole;

   /***** Change user's role if different *****/
   OldRole = Usr_GetRoleInCrs (Gbl.CurrentCrs.Crs.CrsCod,UsrDat->UsrCod);
   if (NewRole != OldRole)        // The role must be updated
     {
      /***** Check if user's role is allowed *****/
      if (!(NewRole == Rol_ROLE_STUDENT ||
	    NewRole == Rol_ROLE_TEACHER))
	 Lay_ShowErrorAndExit ("Wrong role.");

      /***** Update the role of a user in a course *****/
      sprintf (Query,"UPDATE crs_usr SET Role='%u'"
		     " WHERE CrsCod='%ld' AND UsrCod='%ld'",
	       (unsigned) NewRole,Gbl.CurrentCrs.Crs.CrsCod,UsrDat->UsrCod);
      DB_QueryUPDATE (Query,"can not modify user's role in course");

      /***** Show info message *****/
      if (QuietOrVerbose == Cns_VERBOSE)
	{
	 sprintf (Gbl.Message,Txt_The_role_of_THE_USER_X_in_the_course_Y_has_changed_from_A_to_B,
		  UsrDat->FullName,Gbl.CurrentCrs.Crs.FullName,
		  Txt_ROLES_SINGULAR_abc[OldRole][UsrDat->Sex],
		  Txt_ROLES_SINGULAR_abc[NewRole][UsrDat->Sex]);
	 Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
	}

      UsrDat->RoleInCurrentCrsDB = NewRole;
      UsrDat->Roles |= (1 << NewRole);
     }

   /***** Set user's acceptation to true in the current course *****/
   if (KeepOrSetAccepted == Usr_SET_ACCEPTED_TO_TRUE)
      Usr_AcceptUsrInCrs (UsrDat->UsrCod);      // Confirm user enrollment
  }

/*****************************************************************************/
/********* Set a user's acceptation to true in the current course ************/
/*****************************************************************************/

void Usr_AcceptUsrInCrs (long UsrCod)
  {
   char Query[512];

   /***** Set enrollment of a user to "accepted" in the current course *****/
   sprintf (Query,"UPDATE crs_usr SET Accepted='Y'"
                  " WHERE CrsCod='%ld' AND UsrCod='%ld'",
            Gbl.CurrentCrs.Crs.CrsCod,UsrCod);
   DB_QueryUPDATE (Query,"can not confirm user's enrollment");
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

   /***** Replace tildes, ñ, etc. by codes, because some browsers (i.e., IE5) don't show correctly tildes with AJAX *****/
   Str_ReplaceSpecialCharByCodes (FirstName,Usr_MAX_BYTES_NAME_SPEC_CHAR);
   Str_ReplaceSpecialCharByCodes (Surnames,2*Usr_MAX_BYTES_NAME_SPEC_CHAR+1);

   /***** Write shorted firstname, then return, then shorted surnames *****/
   fprintf (Gbl.F.Out,"%s<br />%s",FirstName,Surnames);
  }

/*****************************************************************************/
/********** Check if a user is an administrator of current degree ************/
/*****************************************************************************/

bool Usr_CheckIfUsrIsAdmOfDeg (long UsrCod,long DegCod)
  {
   char Query[512];

   /***** Get if a user is administrator of a degree from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM deg_admin"
                  " WHERE UsrCod='%ld' AND DegCod='%ld'",
            UsrCod,DegCod);
   return (DB_QueryCOUNT (Query,"can not check if a user is administrator of a degree") != 0);
  }

/*****************************************************************************/
/********** Check if a user is an administrator of current degree ************/
/*****************************************************************************/

bool Usr_CheckIfUsrIsAdmOfAllDegs (long UsrCod)
  {
   char Query[512];

   /***** Get if a user is administrator of all degrees from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM deg_admin"
                  " WHERE UsrCod='%ld' AND DegCod='%ld'",
            UsrCod,Usr_SPECIAL_CODE_GLOBAL_ADMIN);
   return (DB_QueryCOUNT (Query,"can not check if a user is administrator of all degrees") != 0);
  }

/*****************************************************************************/
/********************* Check if a user is a superuser ************************/
/*****************************************************************************/

bool Usr_CheckIfUsrIsSuperuser (long UsrCod)
  {
   char Query[512];

   /***** Get if a user is superuser from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM deg_admin"
                  " WHERE UsrCod='%ld' AND DegCod='%ld'",
            UsrCod,Usr_SPECIAL_CODE_SUPERUSER);
   return (DB_QueryCOUNT (Query,"can not check if a user is superuser") != 0);
  }

/*****************************************************************************/
/*************** Check if a user belongs to any of my courses ****************/
/*****************************************************************************/

bool Usr_CheckIfUsrSharesAnyOfMyCrs (long UsrCod)
  {
   char Query[512];

   /***** Get if a user shares any course with me from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM crs_usr WHERE UsrCod='%ld'"
                  " AND CrsCod IN (SELECT CrsCod FROM crs_usr WHERE UsrCod='%ld')",
            UsrCod,Gbl.Usrs.Me.UsrDat.UsrCod);
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
      if ((NumInss = (unsigned) Usr_GetInssFromUsr (Gbl.Usrs.Me.UsrDat.UsrCod,&mysql_res)) > 0) // Institutions found
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
               Gbl.Usrs.Me.MyInstitutions.Inss[Gbl.Usrs.Me.MyInstitutions.Num].MaxRole = Usr_ConvertUnsignedStrToRole (row[1]);

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
               Gbl.Usrs.Me.MyCentres.Ctrs[Gbl.Usrs.Me.MyCentres.Num].MaxRole = Usr_ConvertUnsignedStrToRole (row[1]);

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
               Gbl.Usrs.Me.MyDegrees.Degs[Gbl.Usrs.Me.MyDegrees.Num].MaxRole = Usr_ConvertUnsignedStrToRole (row[1]);

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
               Gbl.Usrs.Me.MyCourses.Crss[Gbl.Usrs.Me.MyCourses.Num].Role   = Usr_ConvertUnsignedStrToRole (row[1]);
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
/************** Get the institutions of a user from database *****************/
/*****************************************************************************/
// Returns the number of rows of the result

unsigned long Usr_GetInssFromUsr (long UsrCod,MYSQL_RES **mysql_res)
  {
   char Query[512];

   /***** Get the institutions a user belongs to from database *****/
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
/******************** Put a link (form) to remove old users ******************/
/*****************************************************************************/

static void Usr_PutFormToRemOldUsrs (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Remove_old_users;

   /***** Put form to remove old users *****/
   Act_FormStart (ActReqRemOldUsr);
   Act_LinkFormSubmit (Txt_Remove_old_users,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("delon",Txt_Remove_old_users,Txt_Remove_old_users);
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/*********************** Write form to remove old users **********************/
/*****************************************************************************/

void Usr_AskRemoveOldUsrs (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Eliminate_all_users_who_are_not_enrolled_on_any_courses_PART_1_OF_2;
   extern const char *Txt_Eliminate_all_users_who_are_not_enrolled_on_any_courses_PART_2_OF_2;
   extern const char *Txt_Eliminate;
   unsigned MonthsWithoutAccess = Usr_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS;
   unsigned i;

   /***** Form to request number of months without clicks *****/
   fprintf (Gbl.F.Out,"<div align=\"center\">");
   Act_FormStart (ActRemOldUsr);
   fprintf (Gbl.F.Out,"<span class=\"%s\">%s </span>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Eliminate_all_users_who_are_not_enrolled_on_any_courses_PART_1_OF_2);
   fprintf (Gbl.F.Out,"<select name=\"Months\">");
   for (i  = Usr_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS;
        i <= Usr_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS;
        i++)
     {
      fprintf (Gbl.F.Out,"<option");
      if (i == MonthsWithoutAccess)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%u</option>",i);
     }
   fprintf (Gbl.F.Out,"</select>"
                      "<span class=\"%s\"> ",
            The_ClassFormul[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,Txt_Eliminate_all_users_who_are_not_enrolled_on_any_courses_PART_2_OF_2,
            Cfg_PLATFORM_SHORT_NAME);
   fprintf (Gbl.F.Out,"</span>");

   /***** Send button*****/
   Lay_PutSendButton (Txt_Eliminate);
   fprintf (Gbl.F.Out,"</form>"
                      "</div>");
  }

/*****************************************************************************/
/****************************** Remove old users *****************************/
/*****************************************************************************/

void Usr_RemoveOldUsrs (void)
  {
   extern const char *Txt_Eliminating_X_users_who_were_not_enrolled_in_any_course_and_with_more_than_Y_months_without_access_to_Z;
   extern const char *Txt_X_users_have_been_eliminated;
   char UnsignedStr[10+1];
   unsigned MonthsWithoutAccess;
   unsigned long SecondsWithoutAccess;
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumUsr;
   unsigned long NumUsrs;
   unsigned NumUsrsEliminated = 0;
   struct UsrData UsrDat;

   /***** Get parameter with number of months without access *****/
   Par_GetParToText ("Months",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&MonthsWithoutAccess) != 1)
      Lay_ShowErrorAndExit ("Number of months without clicks is missing.");
   if (MonthsWithoutAccess < Usr_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS ||
       MonthsWithoutAccess > Usr_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS)
      Lay_ShowErrorAndExit ("Wrong number of months without clicks.");
   SecondsWithoutAccess = (unsigned long) MonthsWithoutAccess * Dat_SECONDS_IN_ONE_MONTH;

   /***** Get old users from database *****/
   sprintf (Query,"SELECT UsrCod FROM"
                  "("
                  "SELECT UsrCod FROM usr_last WHERE"
                  " UNIX_TIMESTAMP(LastTime) < UNIX_TIMESTAMP()-%lu"
                  " UNION "
                  "SELECT UsrCod FROM usr_data WHERE"
                  " UsrCod NOT IN (SELECT UsrCod FROM usr_last)"
                  ") AS candidate_usrs"
                  " WHERE UsrCod NOT IN (SELECT DISTINCT UsrCod FROM crs_usr)",
            SecondsWithoutAccess);
   if ((NumUsrs = DB_QuerySELECT (Query,&mysql_res,"can not get old users")))
     {
      sprintf (Gbl.Message,Txt_Eliminating_X_users_who_were_not_enrolled_in_any_course_and_with_more_than_Y_months_without_access_to_Z,
               NumUsrs,
               MonthsWithoutAccess,
               Cfg_PLATFORM_SHORT_NAME);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Remove users *****/
      for (NumUsr = 0;
           NumUsr < NumUsrs;
           NumUsr++)
        {
         row = mysql_fetch_row (mysql_res);
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
           {
            Usr_DeleteUsrFromPlatform (&UsrDat,Cns_QUIET);
            NumUsrsEliminated++;
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Move unused contents of messages to table of deleted contents of messages *****/
      Msg_MoveUnusedMsgsContentToDeleted ();
     }

   /***** Write end message *****/
   sprintf (Gbl.Message,Txt_X_users_have_been_eliminated,
            NumUsrsEliminated);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/******** Check if a user exists with a given encrypted user's code **********/
/*****************************************************************************/

static bool Usr_ChkIfEncryptedUsrCodExists (const char *EncryptedUsrCod)
  {
   char Query[512];

   /***** Get if an encrypted user's code already existed in database *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_data WHERE EncryptedUsrCod='%s'",
            EncryptedUsrCod);
   return (DB_QueryCOUNT (Query,"can not check if an encrypted user's code already existed") != 0);
  }

/*****************************************************************************/
/****** Request acceptation / refusion of register in current course *********/
/*****************************************************************************/

void Usr_ReqAcceptRegisterInCrs (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_A_teacher_or_administrator_has_enrolled_you_into_the_course_;
   extern const char *Txt_Confirm_my_enrollment;
   extern const char *Txt_Remove_me_from_this_course;

   /***** Show message *****/
   sprintf (Gbl.Message,Txt_A_teacher_or_administrator_has_enrolled_you_into_the_course_,
            Gbl.CurrentCrs.Crs.FullName);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   /***** Send button to accept register in the current course *****/
   fprintf (Gbl.F.Out,"<div align=\"center\">");
   Act_FormStart (ActAccEnrCrs);
   Act_LinkFormSubmit (Txt_Confirm_my_enrollment,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("ok_green",Txt_Confirm_my_enrollment,Txt_Confirm_my_enrollment);
   fprintf (Gbl.F.Out,"</form>");

   /***** Send button to refuse register in the current course *****/
   Act_FormStart (ActRemMeCrs);
   Act_LinkFormSubmit (Txt_Remove_me_from_this_course,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("delon",Txt_Remove_me_from_this_course,Txt_Remove_me_from_this_course);
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");

   /***** Mark possible notification as seen *****/
   Ntf_SetNotifAsSeen (Ntf_EVENT_ENROLLMENT,
                       -1L,
                       Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*************** Accept my enrollment in the current course ******************/
/*****************************************************************************/

void Usr_AcceptRegisterMeInCrs (void)
  {
   extern const char *Txt_You_have_confirmed_your_enrollment_in_the_course_X;

   /***** Confirm my enrollment *****/
   Usr_AcceptUsrInCrs (Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Confirmation message *****/
   sprintf (Gbl.Message,Txt_You_have_confirmed_your_enrollment_in_the_course_X,
            Gbl.CurrentCrs.Crs.FullName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
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
   fprintf (Gbl.F.Out,"<div align=\"center\" style=\"margin-bottom:20px;\">");
   Act_FormStart (ActFrmUsrAcc);
   sprintf (Gbl.Title,Txt_New_on_PLATFORM_Sign_up,Cfg_PLATFORM_SHORT_NAME);
   Act_LinkFormSubmit (Gbl.Title,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("arroba",Txt_Create_account,Gbl.Title);
   fprintf (Gbl.F.Out,"</form>");

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
   fprintf (Gbl.F.Out,"<div align=\"center\">");
   Act_FormStart (ActAutUsrInt);
   Lay_StartRoundFrameTable10 (NULL,2,Txt_Log_in);

   /***** User's ID/nickname and password *****/
   fprintf (Gbl.F.Out,"<tr>"
			 "<td align=\"right\" valign=\"middle\" class=\"%s\">"
			    "%s:"
			 "</td>"
			 "<td align=\"left\" valign=\"middle\">"
			    "<input type=\"text\" id=\"UsrId\" name=\"UsrId\""
			    " size=\"16\" maxlength=\"%u\" value=\"%s\" />"
			 "</td>"
		      "</tr>"
		      "<tr>"
			 "<td align=\"right\" valign=\"middle\" class=\"%s\">"
			    "%s:"
			 "</td>"
			 "<td align=\"left\" valign=\"middle\">"
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
	              "<td colspan=\"2\" align=\"center\">"
                      "<input type=\"submit\" value=\"%s\" />"
                      "</td>"
	              "</tr>",
	              Txt_Log_in);
   Lay_EndRoundFrameTable10 ();
   fprintf (Gbl.F.Out,"</form>");

   /***** Link used for sending a new password *****/
   Act_FormStart (ActReqSndNewPwd);
   Par_PutHiddenParamString ("UsrId",Gbl.Usrs.Me.UsrIdLogin);
   Act_LinkFormSubmit (Txt_I_forgot_my_password,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("key",Txt_I_forgot_my_password,Txt_I_forgot_my_password);
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");
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
	 fprintf (Gbl.F.Out,"<div align=\"center\" style=\"margin:10px;\">");

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
                                     " alt=\"\" width=\"128\" height=\"128\" />",
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
                      " %s</a>"
                      "</form>",
            Gbl.Prefs.PathTheme,Txt_Log_in,Txt_Log_in);
  }

/*****************************************************************************/
/******************* Write type and name of logged user **********************/
/*****************************************************************************/

void Usr_WriteLoggedUsrHead (void)
  {
   extern const char *The_ClassUsr[The_NUM_THEMES];
   extern const char *The_ClassHead[The_NUM_THEMES];
   extern const char *Txt_ROLES_SINGULAR_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   static const unsigned NumCharsName[Lay_NUM_LAYOUTS] =
     {
      16,        // Lay_LAYOUT_DESKTOP
      10,        // Lay_LAYOUT_MOBILE
     };
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];
   char UsrFullName[(Usr_MAX_BYTES_NAME+1)*3];

   /***** User's type *****/
   if (Usr_GetNumAvailableRoles () == 1)
      fprintf (Gbl.F.Out,"<span class=\"%s\">%s:&nbsp;</span>",
               The_ClassUsr[Gbl.Prefs.Theme],
               Txt_ROLES_SINGULAR_Abc[Gbl.Usrs.Me.LoggedRole][Gbl.Usrs.Me.UsrDat.Sex]);
   else
     {
      Usr_PutFormToChangeMyRole (true);
      fprintf (Gbl.F.Out,"<span class=\"%s\">&nbsp;</span>",
               The_ClassUsr[Gbl.Prefs.Theme]);
     }

   /***** Show photo *****/
   ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&Gbl.Usrs.Me.UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&Gbl.Usrs.Me.UsrDat,
                     ShowPhoto ? PhotoURL :
                	         NULL,
                     15,20,true);
   fprintf (Gbl.F.Out,"<span class=\"%s\">&nbsp;</span>",
            The_ClassUsr[Gbl.Prefs.Theme]);

   /***** User's name *****/
   fprintf (Gbl.F.Out,"<span class=\"%s\">",
            The_ClassUsr[Gbl.Prefs.Theme]);

   /* Name */
   if (Gbl.Usrs.Me.UsrDat.FullName[0])
     {
      strcpy (UsrFullName,Gbl.Usrs.Me.UsrDat.FullName);
      Str_LimitLengthHTMLStr (UsrFullName,NumCharsName[Gbl.Prefs.Layout]);
      fprintf (Gbl.F.Out,"%s",UsrFullName);
     }
   else
      fprintf (Gbl.F.Out,"&nbsp;");

   fprintf (Gbl.F.Out,"</span>");
  }

/*****************************************************************************/
/****************** Get number of available roles for me *********************/
/*****************************************************************************/

static unsigned Usr_GetNumAvailableRoles (void)
  {
   Rol_Role_t Role;
   unsigned NumAvailableRoles = 0;

   for (Role = Rol_ROLE_GUEST;
        Role < Rol_NUM_ROLES;
        Role++)
      if (Gbl.Usrs.Me.AvailableRoles & (1 << Role))
         NumAvailableRoles++;

   return NumAvailableRoles;
  }

/*****************************************************************************/
/*********************** Put a form to change my role ************************/
/*****************************************************************************/

static void Usr_PutFormToChangeMyRole (bool FormInHead)
  {
   extern const char *Txt_ROLES_SINGULAR_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   static const unsigned SelectorWidth[Lay_NUM_LAYOUTS] =
     {
      90,        // Lay_LAYOUT_DESKTOP
      90,        // Lay_LAYOUT_MOBILE
     };
   Rol_Role_t Role;

   Act_FormStart (ActChgMyRol);
   fprintf (Gbl.F.Out,"<select name=\"UsrTyp\"");
   if (FormInHead)
      fprintf (Gbl.F.Out," style=\"width:%upx;\"",SelectorWidth[Gbl.Prefs.Layout]);
   fprintf (Gbl.F.Out," onchange=\"javascript:document.getElementById('%s').submit();\">",
            Gbl.FormId);
   for (Role = Rol_ROLE_GUEST;
        Role < Rol_NUM_ROLES;
        Role++)
     if (Gbl.Usrs.Me.AvailableRoles & (1 << Role))
        {
         fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Role);
         if (Role == Gbl.Usrs.Me.LoggedRole)
            fprintf (Gbl.F.Out," selected=\"selected\"");
         fprintf (Gbl.F.Out,">%s</option>",
                  Txt_ROLES_SINGULAR_Abc[Role][Gbl.Usrs.Me.UsrDat.Sex]);
        }
   fprintf (Gbl.F.Out,"</select>"
	              "</form>");
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
                      " %s</a>"
                      "</form>",
            Gbl.Prefs.PathTheme,Txt_Log_out,Txt_Log_out);
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

static unsigned Usr_GetParamOtherUsrIDNickOrEMailAndGetUsrCods (struct ListUsrCods *ListUsrCods)
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
	 Lay_SetCurrentTab ();
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
	 if (Usr_CreateNewAccountAndLogIn ())			// User logged in
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
	    Lay_DisableIncompatibleTabs ();
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
   extern const char *Txt_There_is_no_external_service_for_authentication_and_official_lists;
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
   if (Cfg_EXTERNAL_LOGIN_CLIENT_COMMAND[0] == '\0')
      Lay_ShowAlert (Lay_WARNING,Txt_There_is_no_external_service_for_authentication_and_official_lists);
   else if (Gbl.Imported.ExternalUsrId[0] &&
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
/*************** Create new user account with an ID and login ****************/
/*****************************************************************************/
// Return true if no error and user can be logged in
// Return false on error

static bool Usr_CreateNewAccountAndLogIn (void)
  {
   char NewNicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA+1];
   char NewEmail[Cns_MAX_BYTES_STRING+1];
   char NewEncryptedPassword[Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64+1];

   if (Usr_GetParamsNewAccount (NewNicknameWithoutArroba,NewEmail,NewEncryptedPassword))
     {
      /***** User's has no ID *****/
      Gbl.Usrs.Me.UsrDat.IDs.Num = 0;
      Gbl.Usrs.Me.UsrDat.IDs.List = NULL;

      /***** Set password to the password typed by the user *****/
      strcpy (Gbl.Usrs.Me.UsrDat.Password,NewEncryptedPassword);

      /***** User does not exist in the platform, so create him/her! *****/
      Usr_CreateNewUsr (&Gbl.Usrs.Me.UsrDat);

      /***** Save nickname *****/
      Nck_UpdateMyNick (NewNicknameWithoutArroba);
      strcpy (Gbl.Usrs.Me.UsrDat.Nickname,NewNicknameWithoutArroba);

      /***** Save e-mail *****/
      if (Mai_UpdateEmailInDB (&Gbl.Usrs.Me.UsrDat,NewEmail))
	{
	 /* E-mail updated sucessfully */
	 strcpy (Gbl.Usrs.Me.UsrDat.Email,NewEmail);
	 Gbl.Usrs.Me.UsrDat.EmailConfirmed = false;
	}

      return true;
     }
   else
     {
      /***** Show form again ******/
      Usr_ShowFormRequestNewAccountWithParams (NewNicknameWithoutArroba,NewEmail);
      return false;
     }
  }

/*****************************************************************************/
/**** Check if users exists, if his password is correct, get his data... *****/
/*****************************************************************************/

static void Usr_SetUsrRoleAndPrefs (void)
  {
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   bool ICanBeAdmin = false;

   // User is logged

   /***** Set preferences from my preferences *****/
   Gbl.Prefs.Theme = Gbl.Usrs.Me.UsrDat.Prefs.Theme;
   sprintf (Gbl.Prefs.PathTheme,"%s/%s/%s",
	    Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_THEMES,
	    The_ThemeId[Gbl.Prefs.Theme]);

   Gbl.Prefs.IconSet = Gbl.Usrs.Me.UsrDat.Prefs.IconSet;
   sprintf (Gbl.Prefs.PathIconSet,"%s/%s/%s",
	    Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_ICON_SETS,
	    Ico_IconSetId[Gbl.Prefs.IconSet]);

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
      	 Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB = Usr_GetRoleInCrs (Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);
      	}
     }

   /***** Set the user's role I am logged *****/
   Gbl.Usrs.Me.MaxRole = Usr_GetMaxRole (Gbl.Usrs.Me.UsrDat.Roles);
   Gbl.Usrs.Me.LoggedRole = (Gbl.Usrs.Me.RoleFromSession == Rol_ROLE_UNKNOWN) ?	// If no logged role retrieved from session...
                            Gbl.Usrs.Me.MaxRole :				// ...set current logged role to maximum role in database
                            Gbl.Usrs.Me.RoleFromSession;			// Get logged role from session

   /***** Construct the path to my directory *****/
   Usr_ConstructPathUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Me.PathDir);

   /***** Check if my photo exists and create a link to it ****/
   Gbl.Usrs.Me.MyPhotoExists = Pho_BuildLinkToPhoto (&Gbl.Usrs.Me.UsrDat,Gbl.Usrs.Me.PhotoURL,true);

   /***** Check if I belong to current degree and if I am administrator of current degree *****/
   if (Gbl.CurrentDeg.Deg.DegCod > 0)
      /* Check if I am and administrator of current degree */
      ICanBeAdmin = Usr_CheckIfUsrIsAdmOfDeg (Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.CurrentDeg.Deg.DegCod);
   if (!ICanBeAdmin)
      ICanBeAdmin = Usr_CheckIfUsrIsAdmOfAllDegs (Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Check if I belong to current course *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
      Gbl.Usrs.Me.IBelongToCurrentCrs = Usr_CheckIfIBelongToCrs (Gbl.CurrentCrs.Crs.CrsCod);

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
         Gbl.Usrs.Me.AvailableRoles = (1 << Rol_ROLE_GUEST);
     }
   else        // No course selected
      Gbl.Usrs.Me.AvailableRoles = (1 << Gbl.Usrs.Me.MaxRole);
   if (ICanBeAdmin)
      Gbl.Usrs.Me.AvailableRoles |= (1 << Rol_ROLE_DEG_ADMIN);
   if (Usr_CheckIfUsrIsSuperuser (Gbl.Usrs.Me.UsrDat.UsrCod))
      Gbl.Usrs.Me.AvailableRoles |= (1 << Rol_ROLE_SUPERUSER);

   /***** Check if the role I am logged is now available for me *****/
   if (!(Gbl.Usrs.Me.AvailableRoles & (1 << Gbl.Usrs.Me.LoggedRole)))        // Current type I am logged is not available for me
      /* Set the lowest role available for me */
      for (Gbl.Usrs.Me.LoggedRole = Rol_ROLE_UNKNOWN;
           Gbl.Usrs.Me.LoggedRole < Rol_NUM_ROLES;
           Gbl.Usrs.Me.LoggedRole++)
         if (Gbl.Usrs.Me.AvailableRoles & (1 << Gbl.Usrs.Me.LoggedRole))
            break;

   /***** Set other variables related with my user's type *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      Gbl.Usrs.Me.IHaveAccessToCurrentCrs = ((Gbl.Usrs.Me.IBelongToCurrentCrs && (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_STUDENT ||
                                                                                  Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER)) ||
                                              Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER);
      if (Gbl.Usrs.Me.IBelongToCurrentCrs)
         Gbl.Usrs.Me.UsrDat.Accepted = Usr_GetIfUserHasAcceptedEnrollmentInCurrentCrs (Gbl.Usrs.Me.UsrDat.UsrCod);
     }
  }

/*****************************************************************************/
/************ Get maximum role of a user in all his/her courses **************/
/*****************************************************************************/

static Rol_Role_t Usr_GetMaxRole (unsigned Roles)
  {
   if (Roles & (1 << Rol_ROLE_TEACHER))
      return Rol_ROLE_TEACHER;
   if (Roles & (1 << Rol_ROLE_STUDENT))
      return Rol_ROLE_STUDENT;
   return Rol_ROLE_GUEST;
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
/****************************** Change my role *******************************/
/*****************************************************************************/

void Usr_ChangeMyRole (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   /***** Get parameter with the new logged role ******/
   Par_GetParToText ("UsrTyp",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
     {
      /* Check if new role is a correct type *****/
      if (UnsignedNum >= Rol_NUM_ROLES)
         return;

      /* Check if new role is allowed for me */
      if (!(Gbl.Usrs.Me.AvailableRoles & (1 << UnsignedNum)))
         return;

      /* New role is correct and is allowed for me, so change my logged user type */
      Gbl.Usrs.Me.LoggedRole = (Rol_Role_t) UnsignedNum;

      /* Update logged role in session */
      Ses_UpdateSessionDataInDB ();
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
   extern const char *Txt_ROLES_SINGULAR_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Role;

   /***** Link to log out *****/
   fprintf (Gbl.F.Out,"<div align=\"center\" style=\"margin-bottom:20px;\">");
   Act_FormStart (ActLogOut);
   Act_LinkFormSubmit (Txt_Log_out,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("logout",Txt_Log_out,Txt_Log_out);
   fprintf (Gbl.F.Out,"</form>"
                      "</div>");

   /***** Write message with my new logged role *****/
   sprintf (Gbl.Message,Txt_You_are_LOGGED_as_X,
            Txt_logged[Gbl.Usrs.Me.UsrDat.Sex],
            Txt_ROLES_SINGULAR_abc[Gbl.Usrs.Me.LoggedRole][Gbl.Usrs.Me.UsrDat.Sex]);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   /***** Put a form to change my role *****/
   if (Usr_GetNumAvailableRoles () > 1)
     {
      fprintf (Gbl.F.Out,"<div align=\"center\" class=\"%s\">%s: ",
               The_ClassFormul[Gbl.Prefs.Theme],Txt_Role);
      Usr_PutFormToChangeMyRole (false);
      fprintf (Gbl.F.Out,"</div>");
     }
  }

/*****************************************************************************/
/******************* Update number of clicks without photo *******************/
/*****************************************************************************/

unsigned Usr_UpdateMyClicksWithoutPhoto (void)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumClicks;

   /***** Get number of clicks without photo from database *****/
   sprintf (Query,"SELECT NumClicks FROM clicks_without_photo"
                  " WHERE UsrCod='%ld'",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get number of clicks without photo");

   /***** Update the list of clicks without photo *****/
   if (NumRows)        // The user exists ==> update number of clicks without photo
     {
      /* Get current number of clicks */
      row = mysql_fetch_row (mysql_res);
      sscanf (row[0],"%u",&NumClicks);

      /* Update number of clicks */
      if (NumClicks <= Usr_MAX_CLICKS_WITHOUT_PHOTO)
        {
         sprintf (Query,"UPDATE clicks_without_photo SET NumClicks=NumClicks+1 WHERE UsrCod='%ld'",
                  Gbl.Usrs.Me.UsrDat.UsrCod);
         DB_QueryUPDATE (Query,"can not update number of clicks without photo");
         NumClicks++;
        }
     }
   else                                        // The user does not exist ==> add him/her
     {
      /* Add the user, with one access */
      sprintf (Query,"INSERT INTO clicks_without_photo (UsrCod,NumClicks) VALUES ('%ld',1)",
               Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryINSERT (Query,"can not create number of clicks without photo");
      NumClicks = 1;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Return the number of rows of the result *****/
   return NumClicks;
  }

/*****************************************************************************/
/******** Remove user from table with number of clicks without photo *********/
/*****************************************************************************/

void Usr_RemoveUsrFromTableClicksWithoutPhoto (long UsrCod)
  {
   char Query[512];

   sprintf (Query,"DELETE FROM clicks_without_photo WHERE UsrCod='%ld'",UsrCod);
   DB_QueryDELETE (Query,"can not remove a user from the list of users without photo");
  }

/*****************************************************************************/
/****** Write a form to request another user's ID, @nickname or e-mail *******/
/*****************************************************************************/

void Usr_WriteFormToReqAnotherUsrID (Act_Action_t NextAction)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_nick_email_or_ID;
   extern const char *Txt_Continue;

   /***** Form to request user's ID, @nickname or e-mail address *****/
   Act_FormStart (NextAction);
   fprintf (Gbl.F.Out,"<div align=\"center\" class=\"%s\">"
                      "%s: "
                      "<input type=\"text\" name=\"OtherUsrIDNickOrEMail\""
                      " size=\"20\" maxlength=\"%u\" />"
                      "</div>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_nick_email_or_ID,
            Usr_MAX_BYTES_USR_LOGIN);

   /***** Send button*****/
   Lay_PutSendButton (Txt_Continue);
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/********************* Show form to create a new account *********************/
/*****************************************************************************/

static void Usr_ShowFormRequestNewAccountWithParams (const char *NewNicknameWithoutArroba,
                                                     const char *NewEmail)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Log_in;
   extern const char *Txt_Nickname;
   extern const char *Txt_Email;
   // extern const char *Txt_ID;
   extern const char *Txt_New_on_PLATFORM_Sign_up;
   extern const char *Txt_Create_account;

   /***** Link to log in *****/
   fprintf (Gbl.F.Out,"<div align=\"center\" style=\"margin-bottom:20px;\">");
   Act_FormStart (ActFrmLogIn);
   Act_LinkFormSubmit (Txt_Log_in,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("login",Txt_Log_in,Txt_Log_in);
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");

   /***** Form to enter the ID of the new user *****/
   fprintf (Gbl.F.Out,"<div align=\"center\">");
   Act_FormStart (ActCreUsrAcc);
   sprintf (Gbl.Title,Txt_New_on_PLATFORM_Sign_up,Cfg_PLATFORM_SHORT_NAME);
   Lay_StartRoundFrameTable10 (NULL,2,Gbl.Title);

   /***** Nickname *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"right\" class=\"%s\">"
                      "%s: "
                      "</td>"
	              "<td align=\"left\">"
                      "<input type=\"text\" name=\"NewNick\""
                      " size=\"16\" maxlength=\"%u\" value=\"@%s\" />"
                      "</td>"
                      "</tr>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Nickname,
            1+Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA,
            NewNicknameWithoutArroba);

   /***** E-mail *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"right\" class=\"%s\">"
                      "%s: "
                      "</td>"
	              "<td align=\"left\">"
                      "<input type=\"text\" name=\"NewEmail\""
                      " size=\"16\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>"
                      "</tr>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Email,
            Cns_MAX_BYTES_STRING,
            NewEmail);

   /***** Password *****/
   Pwd_PutFormToGetNewPasswordTwice ();

   /***** Send button and form end *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" align=\"center\">"
                      "<input type=\"submit\" value=\"%s\" />"
                      "</td>"
	              "</tr>",
	              Txt_Create_account);
   Lay_EndRoundFrameTable10 ();
   fprintf (Gbl.F.Out,"</form>"
                      "</div>");
  }

/*****************************************************************************/
/************* Get parameters for the creation of a new account **************/
/*****************************************************************************/
// Return false on error

static bool Usr_GetParamsNewAccount (char *NewNicknameWithoutArroba,
                                     char *NewEmail,
                                     char *NewEncryptedPassword)
  {
   extern const char *Txt_The_nickname_X_had_been_registered_by_another_user;
   extern const char *Txt_The_nickname_entered_X_is_not_valid_;
   extern const char *Txt_The_email_address_X_had_been_registered_by_another_user;
   extern const char *Txt_The_email_address_entered_X_is_not_valid;
   extern const char *Txt_You_have_not_written_twice_the_same_new_password;
   char Query[1024];
   char NewNicknameWithArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA+1];
   char NewPlainPassword[2][Pwd_MAX_LENGTH_PLAIN_PASSWORD+1];
   bool Error = false;

   /***** Step 1/3: Get new nickname from form *****/
   Par_GetParToText ("NewNick",NewNicknameWithArroba,Nck_MAX_BYTES_NICKNAME_WITH_ARROBA);

   if (Nck_CheckIfNickWithArrobaIsValid (NewNicknameWithArroba))        // If new nickname is valid
     {
      /***** Remove arrobas at the beginning *****/
      strncpy (NewNicknameWithoutArroba,NewNicknameWithArroba,Nck_MAX_BYTES_NICKNAME_WITH_ARROBA);
      NewNicknameWithoutArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA] = '\0';
      Str_RemoveLeadingArrobas (NewNicknameWithoutArroba);

      /***** Check if the new nickname matches any of the nicknames of other users *****/
      sprintf (Query,"SELECT COUNT(*) FROM usr_nicknames"
		     " WHERE Nickname='%s' AND UsrCod<>'%ld'",
	       NewNicknameWithoutArroba,Gbl.Usrs.Me.UsrDat.UsrCod);
      if (DB_QueryCOUNT (Query,"can not check if nickname already existed"))        // A nickname of another user is the same that this nickname
	{
	 Error = true;
	 sprintf (Gbl.Message,Txt_The_nickname_X_had_been_registered_by_another_user,
		  NewNicknameWithoutArroba);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	}
     }
   else        // New nickname is not valid
     {
      Error = true;
      sprintf (Gbl.Message,Txt_The_nickname_entered_X_is_not_valid_,
               NewNicknameWithArroba,
               Nck_MIN_LENGTH_NICKNAME_WITHOUT_ARROBA,
               Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }

   /***** Step 2/3: Get new e-mail from form *****/
   Par_GetParToText ("NewEmail",NewEmail,Cns_MAX_BYTES_STRING);

   if (Mai_CheckIfEmailIsValid (NewEmail))	// New e-mail is valid
     {
      /***** Check if the new e-mail matches any of the confirmed e-mails of other users *****/
      sprintf (Query,"SELECT COUNT(*) FROM usr_emails"
		     " WHERE E_mail='%s' AND Confirmed='Y'",
	       NewEmail);
      if (DB_QueryCOUNT (Query,"can not check if e-mail already existed"))	// An e-mail of another user is the same that my e-mail
	{
	 Error = true;
	 sprintf (Gbl.Message,Txt_The_email_address_X_had_been_registered_by_another_user,
		  NewEmail);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	}
     }
   else	// New e-mail is not valid
     {
      Error = true;
      sprintf (Gbl.Message,Txt_The_email_address_entered_X_is_not_valid,
               NewEmail);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }

   /***** Step 3/3: Get new user's ID from form *****/
   Par_GetParToText ("Paswd1",NewPlainPassword[0],Pwd_MAX_LENGTH_PLAIN_PASSWORD);
   Par_GetParToText ("Paswd2",NewPlainPassword[1],Pwd_MAX_LENGTH_PLAIN_PASSWORD);
   if (strcmp (NewPlainPassword[0],NewPlainPassword[1]))
     {
      // Passwords don't match
      Error = true;
      Lay_ShowAlert (Lay_WARNING,Txt_You_have_not_written_twice_the_same_new_password);
     }
   else
     {
      Str_ChangeFormat (Str_FROM_FORM,Str_TO_TEXT,
                        NewPlainPassword[0],Pwd_MAX_LENGTH_PLAIN_PASSWORD,true);
      Cry_EncryptSHA512Base64 (NewPlainPassword[0],NewEncryptedPassword);
      if (!Pwd_SlowCheckIfPasswordIsGood (NewPlainPassword[0],NewEncryptedPassword,-1L))        // New password is good?
	{
	 Error = true;
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);	// Error message is set in Usr_SlowCheckIfPasswordIsGood
	}
     }

   return !Error;
  }

/*****************************************************************************/
/***************** Message after creation of a new account *******************/
/*****************************************************************************/

void Usr_AfterCreationNewAccount (void)
  {
   extern const char *Txt_New_account_created;

   if (Gbl.Usrs.Me.Logged)	// If account has been created without problem, I am logged
     {
      /***** Show message of success *****/
      Lay_ShowAlert (Lay_SUCCESS,Txt_New_account_created);

      /***** Show form with account data *****/
      Usr_ShowFormChangeMyAccount ();
     }
  }

/*****************************************************************************/
/******** Show form to change my account or to create a new account **********/
/*****************************************************************************/

void Usr_ShowFormAccount (void)
  {
   if (Gbl.Usrs.Me.Logged)
      Usr_ShowFormChangeMyAccount ();
   else
      Usr_ShowFormRequestNewAccountWithParams ("","");
  }

/*****************************************************************************/
/*********************** Show form to change my account **********************/
/*****************************************************************************/

void Usr_ShowFormChangeMyAccount (void)
  {
   extern const char *Txt_Before_going_to_any_other_option_you_must_fill_your_nickname;
   extern const char *Txt_Before_going_to_any_other_option_you_must_fill_your_email_address;
   extern const char *Txt_Before_going_to_any_other_option_you_must_fill_your_ID;
   extern const char *Txt_User_account;

   /***** Get current user's nickname and e-mail address
          It's necessary because current nickname or e-mail could be just updated *****/
   Nck_GetNicknameFromUsrCod (Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Me.UsrDat.Nickname);
   Mai_GetEmailFromUsrCod (Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Me.UsrDat.Email,&(Gbl.Usrs.Me.UsrDat.EmailConfirmed));

   /***** Check nickname and e-mail *****/
   if (!Gbl.Usrs.Me.UsrDat.Nickname[0])
      Lay_ShowAlert (Lay_WARNING,Txt_Before_going_to_any_other_option_you_must_fill_your_nickname);
   else if (!Gbl.Usrs.Me.UsrDat.Email[0])
      Lay_ShowAlert (Lay_WARNING,Txt_Before_going_to_any_other_option_you_must_fill_your_email_address);
   else if (!Gbl.Usrs.Me.UsrDat.IDs.Num)
      Lay_ShowAlert (Lay_WARNING,Txt_Before_going_to_any_other_option_you_must_fill_your_ID);

   /***** Put links to change my password and to remove my account*****/
   fprintf (Gbl.F.Out,"<div align=\"center\">");
   Pwd_PutLinkToChangeUsrPassword (&Gbl.Usrs.Me.UsrDat);
   if (Usr_CheckIfICanEliminateAccount (true))	// ItsMe = true
      Usr_PutLinkToRemoveMyAccount ();
   fprintf (Gbl.F.Out,"</div>");

   /***** Start table *****/
   Lay_StartRoundFrameTable10 ("80%",2,Txt_User_account);

   /***** Nickname *****/
   Nck_ShowFormChangeUsrNickname ();

   /***** Separator *****/
   Usr_PrintAccountSeparator ();

   /***** E-mail *****/
   Mai_ShowFormChangeUsrEmail ();

   /***** Separator *****/
   Usr_PrintAccountSeparator ();

   /***** User's ID *****/
   ID_ShowFormChangeUsrID (&Gbl.Usrs.Me.UsrDat,true);

   /***** End of table *****/
   Lay_EndRoundFrameTable10 ();
  }

static void Usr_PrintAccountSeparator (void)
  {
   extern const char *The_ClassSeparator[The_NUM_THEMES];

   /***** Separator *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td colspan=\"2\" align=\"center\" valign=\"middle\">"
		      "<hr class=\"%s\" />"
		      "</td>"
		      "</tr>",
	    The_ClassSeparator[Gbl.Prefs.Theme]);
  }

/*****************************************************************************/
/******** Put a link to the action used to request user's password ***********/
/*****************************************************************************/

static void Usr_PutLinkToRemoveMyAccount (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];

   Act_FormStart (ActUpdOthUsrDat);
   Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Me.UsrDat.EncryptedUsrCod);
   Par_PutHiddenParamUnsigned ("RegRemAction",(unsigned) Usr_ELIMINATE_ONE_USR_FROM_PLATFORM);
   Act_LinkFormSubmit ("Eliminar cuenta",The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("delon","Eliminar cuenta","Eliminar cuenta");	// Need translation!!!!
   fprintf (Gbl.F.Out,"</form>");
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
      // WhatToSearch, LastAccNotif, LastAccBriefcase remain unchanged
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
   // WhatToSearch, LastAccNotif, LastAccBriefcase are set to default (0)
   sprintf (Query,"INSERT INTO usr_last (UsrCod,LastCrs,LastTab,LastTime)"
                  " VALUES ('%ld','%ld','%u',NOW())",
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Gbl.CurrentCrs.Crs.CrsCod,
            (unsigned) Gbl.CurrentTab);
   DB_QueryINSERT (Query,"can not insert last user's data");
  }

/*****************************************************************************/
/******************* Create and show data from other user ********************/
/*****************************************************************************/

void Usr_CreatAndShowNewUsrRecordAndRegInCrs (void)
  {
   extern const char *Txt_The_ID_X_is_not_valid;
   Rol_Role_t NewRole;

   /***** Get user's ID from form *****/
   ID_GetParamOtherUsrIDPlain ();	// User's ID was already modified and passed as a hidden parameter

   if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID))        // User's ID valid
     {
      Gbl.Usrs.Other.UsrDat.UsrCod = -1L;

      /***** Get new role *****/
      NewRole = Rec_GetRoleFromRecordForm ();
      if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER &&
	  NewRole != Rol_ROLE_STUDENT)
	 Lay_ShowErrorAndExit ("A teacher only can create new users as students.");

      /***** Get user's name from form *****/
      Rec_GetUsrNameFromRecordForm (&Gbl.Usrs.Other.UsrDat);

      /***** Create user *****/
      Gbl.Usrs.Other.UsrDat.IDs.List[0].Confirmed = true;	// User's ID will be stored as confirmed
      Usr_CreateNewUsr (&Gbl.Usrs.Other.UsrDat);

      /***** Register user in current course in database *****/
      if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
	{
	 if (Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod))      // User does belong to current course, modify his/her role
	    Usr_ModifyRoleInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole,
			                Cns_VERBOSE,Usr_SET_ACCEPTED_TO_FALSE);
	 else
	    Usr_RegisterUsrInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole,
					 Cns_VERBOSE,Usr_SET_ACCEPTED_TO_FALSE);

	 /***** Change user's groups *****/
	 if (Gbl.CurrentCrs.Grps.NumGrps) // This course has groups?
	    Grp_ChangeOtherUsrGrps ();
	}

      /***** Show user's record *****/
      Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
     }
   else        // User's ID not valid
     {
      /***** Write message *****/
      sprintf (Gbl.Message,Txt_The_ID_X_is_not_valid,
               Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);
      Lay_ShowAlert (Lay_ERROR,Gbl.Message);
     }
  }

/*****************************************************************************/
/**** Modify other user's data and register her/him in course and groups *****/
/*****************************************************************************/

void Usr_ModifAndShowUsrCardAndRegInCrsAndGrps (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char UnsignedStr[10+1];
   unsigned UnsignedNum;
   bool ItsMe;
   bool IAmTeacher;
   bool HeIsTeacher;
   bool ICanChangeUsrName;
   Usr_RegRemOneUsrAction_t RegRemAction;
   Rol_Role_t NewRole;
   bool Error = false;

   /***** Get user from form *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      ItsMe = (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);

      /***** Get the action to do *****/
      Par_GetParToText ("RegRemAction",UnsignedStr,1);
      if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Usr_NUM_ACTIONS_REG_REM_ONE_USR)
	    switch ((RegRemAction = (Usr_RegRemOneUsrAction_t) UnsignedNum))
	      {
	       case Usr_REGISTER_MODIFY_ONE_USR_IN_CRS:
		  if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER || ItsMe)
		    {
		     IAmTeacher = (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER);			// I am logged as teacher
		     HeIsTeacher = (Gbl.Usrs.Other.UsrDat.Roles & (1 << Rol_ROLE_TEACHER));	// He/she is already a teacher in any course
		     ICanChangeUsrName = ItsMe ||
			                 !(IAmTeacher && HeIsTeacher);	// A teacher can not change another teacher's name

		     /***** Get user's name from record form *****/
		     if (ICanChangeUsrName)
			Rec_GetUsrNameFromRecordForm (&Gbl.Usrs.Other.UsrDat);

		     /***** Update user's data in database *****/
		     Usr_UpdateUsrData (&Gbl.Usrs.Other.UsrDat);

		     if (Gbl.CurrentCrs.Crs.CrsCod > 0)
		       {
			/***** Get new role from record form *****/
			NewRole = Rec_GetRoleFromRecordForm ();

			/***** Register user in current course in database *****/
			if (Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod))      // User does belong to current course, modify his/her role
			   Usr_ModifyRoleInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole,
					               Cns_VERBOSE,Usr_SET_ACCEPTED_TO_FALSE);
			else
			   Usr_RegisterUsrInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole,
							Cns_VERBOSE,Usr_SET_ACCEPTED_TO_FALSE);

			/***** Change user's groups *****/
			if (Gbl.CurrentCrs.Grps.NumGrps)	// This course has groups?
			  {
			   if (ItsMe)
			      Grp_ChangeMyGrps ();
			   else
			      Grp_ChangeOtherUsrGrps ();
			  }
		       }

		     /***** Show user's record *****/
		     Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
		    }
		  else
		     Error = true;
		  break;
	       case Usr_REGISTER_ONE_DEGREE_ADMIN:
		  if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
		     Usr_ReqAddAdmOfDeg ();
		  else
		     Error = true;
		  break;
	       case Usr_REMOVE_ONE_USR_FROM_CRS:
		  if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER || ItsMe)
		     Usr_ReqRemUsrFromCrs ();
		  else
		     Error = true;
		  break;
	       case Usr_REMOVE_ONE_DEGREE_ADMIN:
		  if ((Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN && ItsMe) ||
		      Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
		     Usr_ReqRemAdmOfDeg ();
		  else
		     Error = true;
		  break;
	       case Usr_ELIMINATE_ONE_USR_FROM_PLATFORM:
		  if (Usr_CheckIfICanEliminateAccount (ItsMe))
		     Usr_ReqRemUsrGbl ();
		  else
		     Error = true;
		  break;
	       default:
		  Error = true;
		  break;
	      }
	 else
	    Error = true;
      else
	 Error = true;
     }
   else
      Error = true;

   if (Error)
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/****************** Put an enrollment into a notification ********************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void Usr_GetNotifEnrollment (char *SummaryStr,
                             long CrsCod,long UsrCod,
                             unsigned MaxChars)
  {
   extern const char *Txt_ROLES_SINGULAR_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct UsrData UsrDat;
   Rol_Role_t Role;

   SummaryStr[0] = '\0';        // Return nothing on error

   /***** Get user's role in course from database *****/
   sprintf (Query,"SELECT Role"
                  " FROM crs_usr"
                  " WHERE CrsCod='%ld' AND UsrCod='%ld'",
            CrsCod,UsrCod);

   if (!mysql_query (&Gbl.mysql,Query))
      if ((mysql_res = mysql_store_result (&Gbl.mysql)) != NULL)
        {
         /***** Result should have a unique row *****/
         if (mysql_num_rows (mysql_res) == 1)
           {
            /***** Get user's role in course *****/
            row = mysql_fetch_row (mysql_res);

            /* Initialize structure with user's data */
            Usr_UsrDataConstructor (&UsrDat);

            /* Get user's data */
            UsrDat.UsrCod = UsrCod;
            Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);

            /* Role (row[0]) */
            Role = Usr_ConvertUnsignedStrToRole (row[0]);
            strcpy (SummaryStr,Txt_ROLES_SINGULAR_Abc[Role][UsrDat.Sex]);
            if (MaxChars)
               Str_LimitLengthHTMLStr (SummaryStr,MaxChars);

            /* Free memory used for user's data */
            Usr_UsrDataDestructor (&UsrDat);
           }

         mysql_free_result (mysql_res);
        }
  }

/*****************************************************************************/
/****************************** Create new user ******************************/
/*****************************************************************************/
// UsrDat->UsrCod must be <= 0
// UsrDat->UsrDat.IDs must contain a list of IDs for the new user

void Usr_CreateNewUsr (struct UsrData *UsrDat)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   char Query[2048];
   char PathRelUsr[PATH_MAX+1];
   unsigned NumID;

   /***** Check if user's code is initialized *****/
   if (UsrDat->UsrCod > 0)
      Lay_ShowErrorAndExit ("Can not create new user.");

   /***** Create encrypted user's code *****/
   Usr_CreateNewEncryptedUsrCod (UsrDat);

   /***** Filter some user's data before inserting */
   Usr_FilterUsrDat (UsrDat);

   /***** Insert new user in database *****/
   /* Insert user's data */
   sprintf (Query,"INSERT INTO usr_data (EncryptedUsrCod,Password,Surname1,Surname2,FirstName,Sex,"
		  "Layout,Theme,IconSet,Language,PublicPhoto,"
		  "CtyCod,"
		  "LocalAddress,LocalPhone,FamilyAddress,FamilyPhone,OriginPlace,Birthday,Comments,"
		  "SideCols,NotifNtfEvents,EmailNtfEvents)"
		  " VALUES ('%s','%s','%s','%s','%s','%s',"
		  "'%u','%s','%s','%s','%c',"
		  "'%ld',"
		  "'%s','%s','%s','%s','%s','%04u-%02u-%02u','%s',"
		  "'3','-1','0')",
	    UsrDat->EncryptedUsrCod,
	    UsrDat->Password,
	    UsrDat->Surname1,UsrDat->Surname2,UsrDat->FirstName,
	    Usr_StringsSexDB[UsrDat->Sex],
	    (unsigned) UsrDat->Prefs.Layout,
	    The_ThemeId[UsrDat->Prefs.Theme],
	    Ico_IconSetId[UsrDat->Prefs.IconSet],
	    Txt_STR_LANG_ID[UsrDat->Prefs.Language],
	    UsrDat->PublicPhoto ? 'Y' :
		                  'N',
	    UsrDat->CtyCod,
	    UsrDat->LocalAddress ,UsrDat->LocalPhone,
	    UsrDat->FamilyAddress,UsrDat->FamilyPhone,
	    UsrDat->OriginPlace,
	    UsrDat->Birthday.Year,UsrDat->Birthday.Month,UsrDat->Birthday.Day,
	    UsrDat->Comments ? UsrDat->Comments :
		               "");
   UsrDat->UsrCod = DB_QueryINSERTandReturnCode (Query,"can not create user");

   /* Insert user's IDs as confirmed */
   for (NumID = 0;
	NumID < UsrDat->IDs.Num;
	NumID++)
     {
      Str_ConvertToUpperText (UsrDat->IDs.List[NumID].ID);
      sprintf (Query,"INSERT INTO usr_IDs (UsrCod,UsrID,CreatTime,Confirmed)"
		     " VALUES ('%ld','%s',NOW(),'%c')",
	       UsrDat->UsrCod,
	       UsrDat->IDs.List[NumID].ID,
	       UsrDat->IDs.List[NumID].Confirmed ? 'Y' :
		                                   'N');
      DB_QueryINSERT (Query,"can not store user's ID when creating user");
     }

   /***** Create directory for the user, if not exists *****/
   Usr_ConstructPathUsr (UsrDat->UsrCod,PathRelUsr);
   Fil_CreateDirIfNotExists (PathRelUsr);
  }

/*****************************************************************************/
/***************************** Update user's data ****************************/
/*****************************************************************************/
// UsrDat->UsrCod must be > 0

void Usr_UpdateUsrData (struct UsrData *UsrDat)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   char Query[2048];

   /***** Check if user's code is initialized *****/
   if (UsrDat->UsrCod <= 0)
      Lay_ShowErrorAndExit ("Can not update user's data. Wrong user's code.");

   /***** Filter some user's data before updating */
   Usr_FilterUsrDat (UsrDat);

   /***** Update user's common data *****/
   sprintf (Query,"UPDATE usr_data"
		  " SET Password='%s',"
		  "Surname1='%s',Surname2='%s',FirstName='%s',Sex='%s',"
		  "Layout='%u',Theme='%s',IconSet='%s',Language='%s',PublicPhoto='%c',"
		  "CtyCod='%ld',"
		  "LocalAddress='%s',LocalPhone='%s',FamilyAddress='%s',FamilyPhone='%s',OriginPlace='%s',Birthday='%04u-%02u-%02u',Comments='%s'"
		  " WHERE UsrCod='%ld'",
	    UsrDat->Password,
	    UsrDat->Surname1,UsrDat->Surname2,UsrDat->FirstName,
	    Usr_StringsSexDB[UsrDat->Sex],
	    (unsigned) UsrDat->Prefs.Layout,
	    The_ThemeId[UsrDat->Prefs.Theme],
	    Ico_IconSetId[UsrDat->Prefs.IconSet],
	    Txt_STR_LANG_ID[UsrDat->Prefs.Language],
	    UsrDat->PublicPhoto ? 'Y' :
		                  'N',
	    UsrDat->CtyCod,
	    UsrDat->LocalAddress,UsrDat->LocalPhone,
	    UsrDat->FamilyAddress,UsrDat->FamilyPhone,UsrDat->OriginPlace,
	    UsrDat->Birthday.Year,UsrDat->Birthday.Month,UsrDat->Birthday.Day,
	    UsrDat->Comments ? UsrDat->Comments :
		               "",
	    UsrDat->UsrCod);
   DB_QueryUPDATE (Query,"can not update user's data");
  }

/*****************************************************************************/
/**************** Update institution, centre and department ******************/
/*****************************************************************************/

void Usr_UpdateInstitutionCentreDepartment (void)
  {
   char Query[256];

   sprintf (Query,"UPDATE usr_data"
	          " SET InsCtyCod='%ld',InsCod='%ld',CtrCod='%ld',DptCod='%ld'"
	          " WHERE UsrCod='%ld'",
	    Gbl.Usrs.Me.UsrDat.InsCtyCod,
	    Gbl.Usrs.Me.UsrDat.InsCod,
            Gbl.Usrs.Me.UsrDat.Tch.CtrCod,
            Gbl.Usrs.Me.UsrDat.Tch.DptCod,
	    Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update institution, centre and department");
  }

/*****************************************************************************/
/******************** Create a new encrypted user's code *********************/
/*****************************************************************************/

#define LENGTH_RANDOM_STR 32
#define MAX_TRY 10

static void Usr_CreateNewEncryptedUsrCod (struct UsrData *UsrDat)
  {
   char RandomStr[LENGTH_RANDOM_STR+1];
   unsigned NumTry;

   for (NumTry = 0;
        NumTry < MAX_TRY;
        NumTry++)
     {
      Str_CreateRandomAlphanumStr (RandomStr,LENGTH_RANDOM_STR);
      Cry_EncryptSHA256Base64 (RandomStr,UsrDat->EncryptedUsrCod);
      if (!Usr_ChkIfEncryptedUsrCodExists (UsrDat->EncryptedUsrCod))
          break;
     }
   if (NumTry == MAX_TRY)
      Lay_ShowErrorAndExit ("Can not create a new encrypted user's code.");
   }

/*****************************************************************************/
/************************* Filter some user's data ***************************/
/*****************************************************************************/

static void Usr_FilterUsrDat (struct UsrData *UsrDat)
  {
   /***** Fix birthday *****/
   if (UsrDat->Birthday.Year < Gbl.Now.Date.Year-99 ||
       UsrDat->Birthday.Year > Gbl.Now.Date.Year-16)
      UsrDat->Birthday.Year =
      UsrDat->Birthday.Month =
      UsrDat->Birthday.Day = 0;
  }

/*****************************************************************************/
/*********************** Register user in current course *********************/
/*****************************************************************************/
// Before calling this function, you must be sure that
// the user does not belong to the current course

void Usr_RegisterUsrInCurrentCrs (struct UsrData *UsrDat,Rol_Role_t NewRole,
                                  Cns_QuietOrVerbose_t QuietOrVerbose,
                                  Usr_KeepOrSetAccepted_t KeepOrSetAccepted)
  {
   extern const char *Txt_THE_USER_X_has_been_enrolled_in_the_course_Y;
   char Query[1024];
   bool CreateNotif;
   bool NotifyByEmail;

   /***** Check if user's role is allowed *****/
   if (!(NewRole == Rol_ROLE_STUDENT ||
	 NewRole == Rol_ROLE_TEACHER))
      Lay_ShowErrorAndExit ("Wrong role.");

   /***** Register user in current course in database *****/
   sprintf (Query,"INSERT INTO crs_usr (CrsCod,UsrCod,Role,Accepted,"
		  "LastAccDownloadCrs,LastAccCommonCrs,LastAccMyWorks,LastAccCrsWorks,LastAccMarksCrs,"
		  "LastDowGrpCod,LastComGrpCod,LastAssGrpCod,NumAccTst,LastAccTst,NumQstsLastTst,"
		  "UsrListType,ColsClassPhoto,ListWithPhotos)"
		  " VALUES ('%ld','%ld','%u','%c',"
		  "'0000-00-00','0000-00-00','0000-00-00','0000-00-00','0000-00-00',"
		  "'-1','-1','-1','0','0000-00-00','0',"
		  "'%s','%u','%c')",
	    Gbl.CurrentCrs.Crs.CrsCod,UsrDat->UsrCod,(unsigned) NewRole,
	    KeepOrSetAccepted == Usr_SET_ACCEPTED_TO_TRUE ? 'Y' :
		                                            'N',
	    Usr_StringsUsrListTypeInDB[Usr_SHOW_USRS_TYPE_DEFAULT],
	    Usr_CLASS_PHOTO_COLS_DEF,
	    Usr_LIST_WITH_PHOTOS_DEF ? 'Y' :
		                       'N');
   DB_QueryINSERT (Query,"can not register user in course");
   UsrDat->RoleInCurrentCrsDB = NewRole;
   UsrDat->Roles |= NewRole;

   /***** Remove possible inscription request ******/
   Usr_RemoveEnrollmentRequest (Gbl.CurrentCrs.Crs.CrsCod,UsrDat->UsrCod);

   /***** Create notification for this user.
	  If this user wants to receive notifications by e-mail,
	  activate the sending of a notification *****/
   CreateNotif = (UsrDat->Prefs.NotifNtfEvents & (1 << Ntf_EVENT_ENROLLMENT));
   NotifyByEmail = CreateNotif &&
		   (UsrDat->UsrCod != Gbl.Usrs.Me.UsrDat.UsrCod) &&
		   (UsrDat->Prefs.EmailNtfEvents & (1 << Ntf_EVENT_ENROLLMENT));
   if (CreateNotif)
      Ntf_StoreNotifyEventToOneUser (Ntf_EVENT_ENROLLMENT,UsrDat,-1L,
				     (Ntf_Status_t) (NotifyByEmail ? Ntf_STATUS_BIT_EMAIL :
					                             0));

   /***** Show info message *****/
   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_THE_USER_X_has_been_enrolled_in_the_course_Y,
	       UsrDat->FullName,Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
  }

/*****************************************************************************/
/******************* Register administrator in current degree ****************/
/*****************************************************************************/

static void Usr_RegisterAdminInCurrentDeg (struct UsrData *UsrDat)
  {
   extern const char *Txt_THE_USER_X_is_already_an_administrator_of_the_degree_Y;
   extern const char *Txt_THE_USER_X_has_been_enrolled_as_administrator_of_the_degree_Y;
   char Query[512];

   /***** Check if user was and administrator of current degree *****/
   if (Usr_CheckIfUsrIsAdmOfDeg (UsrDat->UsrCod,Gbl.CurrentDeg.Deg.DegCod))
      sprintf (Gbl.Message,Txt_THE_USER_X_is_already_an_administrator_of_the_degree_Y,
               UsrDat->FullName,Gbl.CurrentDeg.Deg.FullName);
   else        // User was not administrator of current degree
     {
      /***** Insert or replace administrator in current degree *****/
      sprintf (Query,"REPLACE INTO deg_admin (UsrCod,DegCod)"
                     " VALUES ('%ld','%ld')",
               UsrDat->UsrCod,Gbl.CurrentDeg.Deg.DegCod);
      DB_QueryREPLACE (Query,"can not create administrator of degree");

      sprintf (Gbl.Message,Txt_THE_USER_X_has_been_enrolled_as_administrator_of_the_degree_Y,
               UsrDat->FullName,Gbl.CurrentDeg.Deg.FullName);
     }
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/************************ Remove a user from a course ************************/
/*****************************************************************************/

static void Usr_EffectivelyRemUsrFromCrs (struct UsrData *UsrDat,struct Course *Crs,
                                          Usr_RemoveUsrWorks_t RemoveUsrWorks,Cns_QuietOrVerbose_t QuietOrVerbose)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_from_the_course_Y;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char Query[1024];

   if (Usr_CheckIfUsrBelongsToCrs (UsrDat->UsrCod,Crs->CrsCod))        // User belongs to the course
     {
      /***** Remove user from all the attendance events in course *****/
      Att_RemoveUsrFromCrsAttEvents (UsrDat->UsrCod,Crs->CrsCod);

      /***** Remove user from all the groups in course *****/
      Grp_RemUsrFromAllGrpsInCrs (UsrDat,Crs,QuietOrVerbose);

      /***** Remove user's status about reading of course information *****/
      Inf_RemoveUsrFromCrsInfoRead (UsrDat->UsrCod,Crs->CrsCod);

      /***** Remove works zone of this user in course *****/
      if (RemoveUsrWorks == Usr_REMOVE_WORKS)
         Brw_RemoveUsrWorksInCrs (UsrDat,Crs,QuietOrVerbose);

      /***** Remove fields of this user in its course record *****/
      Rec_RemoveFieldsCrsRecordInCrs (UsrDat->UsrCod,Crs,QuietOrVerbose);

      /***** Remove user's clipboard in course *****/
      Brw_RemoveUsrClipboardInCrs (UsrDat->UsrCod,Crs->CrsCod);

      /***** Remove user's expanded folders in course *****/
      Brw_RemoveUsrExpandedFoldersInCrs (UsrDat->UsrCod,Crs->CrsCod);

      /***** Remove exams made by user in course *****/
      Tst_RemoveExamsMadeByUsrInCrs (UsrDat->UsrCod,Crs->CrsCod);

      /***** Set all the notifications for this user in this course as removed,
             except notifications about new messages *****/
      Ntf_SetNotifInCrsAsRemoved (Crs->CrsCod,UsrDat->UsrCod);

      /***** Remove user from the table of courses-users *****/
      sprintf (Query,"DELETE FROM crs_usr"
                     " WHERE CrsCod='%ld' AND UsrCod='%ld'",
               Crs->CrsCod,UsrDat->UsrCod);
      DB_QueryDELETE (Query,"can not remove a user from a course");

      if (QuietOrVerbose == Cns_VERBOSE)
        {
         sprintf (Gbl.Message,Txt_THE_USER_X_has_been_removed_from_the_course_Y,
                  UsrDat->FullName,Crs->FullName);
         Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
        }
     }
   else        // User does not belong to course
      if (QuietOrVerbose == Cns_VERBOSE)
         Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/************* Remove an administrator from current degree *******************/
/*****************************************************************************/

static void Usr_EffectivelyRemAdmFromDeg (struct UsrData *UsrDat)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_as_administrator_of_the_degree_Y;
   extern const char *Txt_THE_USER_X_is_not_an_administrator_of_the_degree_Y;
   char Query[1024];

   if (Usr_CheckIfUsrIsAdmOfDeg (UsrDat->UsrCod,Gbl.CurrentDeg.Deg.DegCod))        // User is administrator of current degree
     {
      /***** Remove user from the table of courses-users *****/
      sprintf (Query,"DELETE FROM deg_admin"
                     " WHERE UsrCod='%ld' AND DegCod='%ld'",
               UsrDat->UsrCod,Gbl.CurrentDeg.Deg.DegCod);
      DB_QueryDELETE (Query,"can not remove an administrator from a degree");

      sprintf (Gbl.Message,Txt_THE_USER_X_has_been_removed_as_administrator_of_the_degree_Y,
               UsrDat->FullName,Gbl.CurrentDeg.Deg.FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
   else        // User is not an administrator of the current degree
     {
      sprintf (Gbl.Message,Txt_THE_USER_X_is_not_an_administrator_of_the_degree_Y,
               UsrDat->FullName,Gbl.CurrentDeg.Deg.FullName);
      Lay_ShowAlert (Lay_ERROR,Gbl.Message);
     }
  }

/*****************************************************************************/
/************* Remove completely a user from the whole platform **************/
/*****************************************************************************/

static void Usr_DeleteUsrFromPlatform (struct UsrData *UsrDat,
                                       Cns_QuietOrVerbose_t QuietOrVerbose)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_from_all_his_her_courses;
   extern const char *Txt_THE_USER_X_has_been_removed_as_administrator;
   extern const char *Txt_Messages_of_THE_USER_X_have_been_deleted;
   extern const char *Txt_Virtual_pendrive_of_THE_USER_X_has_been_removed;
   extern const char *Txt_Photo_of_THE_USER_X_has_been_removed;
   extern const char *Txt_Record_card_of_THE_USER_X_has_been_removed;
   char Query[1024];
   bool PhotoRemoved = false;

   /***** Remove the works zones of the user in all courses *****/
   Brw_RemoveUsrWorksInAllCrss (UsrDat,QuietOrVerbose);        // Make this before of removing the user from the courses

   /***** Remove the fields of course record in all courses *****/
   Rec_RemoveFieldsCrsRecordAll (UsrDat->UsrCod,QuietOrVerbose);

   /***** Remove user from all the attendance events *****/
   Att_RemoveUsrFromAllAttEvents (UsrDat->UsrCod);

   /***** Remove user from all the groups of all courses *****/
   Grp_RemUsrFromAllGrps (UsrDat,QuietOrVerbose);

   /***** Remove user's requests for inscription *****/
   sprintf (Query,"DELETE FROM crs_usr_requests"
                  " WHERE UsrCod='%ld'",
            UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's requests for inscription");

   /***** Remove user from the table of courses and users *****/
   sprintf (Query,"DELETE FROM crs_usr"
                  " WHERE UsrCod='%ld'",
            UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove a user from all courses");

   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_THE_USER_X_has_been_removed_from_all_his_her_courses,
               UsrDat->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Remove user as administrator of any degree *****/
   sprintf (Query,"DELETE FROM deg_admin"
                  " WHERE UsrCod='%ld'",
            UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove a user as administrator");

   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_THE_USER_X_has_been_removed_as_administrator,
               UsrDat->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Remove user's clipboards *****/
   Brw_RemoveUsrClipboard (UsrDat->UsrCod);
   For_RemoveUsrFromThrClipboard (UsrDat->UsrCod);

   /***** Remove user's expanded folders *****/
   Brw_RemoveUsrExpandedFolders (UsrDat->UsrCod);

   /***** Remove exams made by user in all courses *****/
   Tst_RemoveExamsMadeByUsrInAllCrss (UsrDat->UsrCod);

   /***** Remove user's notifications *****/
   Ntf_RemoveUsrNtfs (UsrDat->UsrCod);

   /***** Delete user's messages sent and received *****/
   Gbl.Msg.FilterContent[0] = '\0';
   Msg_DelAllRecAndSntMsgsUsr (UsrDat->UsrCod);
   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_Messages_of_THE_USER_X_have_been_deleted,
               UsrDat->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Remove user from table of banned users *****/
   Msg_RemoveUsrFromBanned (UsrDat->UsrCod);

   /***** Delete thread read status for this user *****/
   For_RemoveUsrFromReadThrs (UsrDat->UsrCod);

   /***** Remove user from table of seen announcements *****/
   Ann_RemoveUsrFromSeenAnnouncements (UsrDat->UsrCod);

   /***** Remove user from table of connected users *****/
   sprintf (Query,"DELETE FROM connected WHERE UsrCod='%ld'",
            UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove a user from table of connected users");

   /***** Remove all sessions of this user *****/
   sprintf (Query,"DELETE FROM sessions WHERE UsrCod='%ld'",
            UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove sessions of a user");

   /***** Remove the user from the list of users without photo *****/
   Usr_RemoveUsrFromTableClicksWithoutPhoto (UsrDat->UsrCod);

   /***** Remove the file tree of a user *****/
   Usr_RemoveUsrBriefcase (UsrDat);
   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_Virtual_pendrive_of_THE_USER_X_has_been_removed,
               UsrDat->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Remove user's photo *****/
   PhotoRemoved = Pho_RemovePhoto (UsrDat);
   if (PhotoRemoved && QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_Photo_of_THE_USER_X_has_been_removed,
               UsrDat->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Remove user *****/
   Usr_RemoveUsr (UsrDat);
   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_Record_card_of_THE_USER_X_has_been_removed,
               UsrDat->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
  }

/*****************************************************************************/
/********************** Remove the briefcase of a user ***********************/
/*****************************************************************************/

void Usr_RemoveUsrBriefcase (struct UsrData *UsrDat)
  {
   char PathRelUsr[PATH_MAX+1];

   /***** Remove the briefcase of the user *****/
   Usr_ConstructPathUsr (UsrDat->UsrCod,PathRelUsr);
   Brw_RemoveTree (PathRelUsr);

   /***** Remove files in the course from database *****/
   Brw_RemoveFilesFromDB (-1L,-1L,UsrDat->UsrCod);

   /***** Remove size of the briefcase of the user from database *****/
   Brw_RemoveSizeOfFileTreeFromDB (-1L,-1L,UsrDat->UsrCod);
  }

/*****************************************************************************/
/************************ Remove a user from database ************************/
/*****************************************************************************/

static void Usr_RemoveUsr (struct UsrData *UsrDat)
  {
   char Query[128];

   /***** Remove user's webs / social networks *****/
   sprintf (Query,"DELETE FROM usr_webs WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's webs / social networks");

   /***** Remove user's nicknames *****/
   sprintf (Query,"DELETE FROM usr_nicknames WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's nicknames");

   /***** Remove user's e-mails *****/
   sprintf (Query,"DELETE FROM pending_emails WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove pending user's e-mails");

   sprintf (Query,"DELETE FROM usr_emails WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's e-mails");

   /***** Remove user's IDs *****/
   sprintf (Query,"DELETE FROM usr_IDs WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's IDs");

   /***** Remove user's last data *****/
   sprintf (Query,"DELETE FROM usr_last WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's last data");

   /***** Remove user's data  *****/
   sprintf (Query,"DELETE FROM usr_data WHERE UsrCod='%ld'",
	    UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove user's data");
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
   fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\" bgcolor=\"%s\">",BgColor);
   Usr_PutCheckboxToSelectUser (Rol_ROLE_GUEST,UsrDat->EncryptedUsrCod,false);
   fprintf (Gbl.F.Out,"</td>");

   /***** Student has accepted enrollment in current course? *****/
   fprintf (Gbl.F.Out,"<td class=\"BM%d\">"
	              "<img src=\"%s/tr16x16.gif\" alt=\"\""
	              " class=\"ICON16x16\" />"
	              "</td>",
            Gbl.RowEvenOdd,
            Gbl.Prefs.IconsURL);

   /***** Write number of user in the list *****/
   fprintf (Gbl.F.Out,"<td align=\"right\" bgcolor=\"%s\" class=\"DAT_SMALL\">&nbsp;%u&nbsp;</td>",
            BgColor,NumUsr);

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show student's photo *****/
      fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\">",BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,
                        ShowPhoto ? PhotoURL :
                                    NULL,
                        18,24,
	                Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Prepare data for brief presentation *****/
   Usr_RestrictLengthMainData (true,UsrDat,MailLink);

   /****** Write user's IDs ******/
   fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"%s\">",
            BgColor,
            UsrDat->Accepted ? "DAT_SMALL_N" :
                               "DAT_SMALL");
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

void Usr_WriteRowStdMainData (unsigned NumUsr,struct UsrData *UsrDat,bool PutCheckboxToSelectUsr)
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
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN ||
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER;

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<tr>");

   /***** Checkbox to select user *****/
   if (PutCheckboxToSelectUsr)
      UsrIsTheMsgSender = (UsrDat->UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);
   // Two colors are used alternatively to better distinguish the rows
   BgColor = UsrIsTheMsgSender ? LIGHT_GREEN :
	                         Gbl.ColorRows[Gbl.RowEvenOdd];
   if (PutCheckboxToSelectUsr)
     {
      fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\" bgcolor=\"%s\">",BgColor);
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
   fprintf (Gbl.F.Out,"<td align=\"right\" bgcolor=\"%s\" class=\"%s\">&nbsp;%u&nbsp;</td>",
            BgColor,
            UsrDat->Accepted ? "DAT_SMALL_N" :
        	               "DAT_SMALL",
            NumUsr);

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show student's photo *****/
      fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\">",BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,
                        ShowPhoto ? PhotoURL :
                                    NULL,
                        18,24,
	                Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Prepare data for brief presentation *****/
   Usr_RestrictLengthMainData (ShowEmail,UsrDat,MailLink);

   /****** Write user's ID ******/
   fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"%s\">",
            BgColor,
            UsrDat->Accepted ? "DAT_SMALL_N" :
                               "DAT_SMALL");
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
      fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\">",BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,
                        ShowPhoto ? PhotoURL :
                                    NULL,
                        18,24,
                        Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW);
      fprintf (Gbl.F.Out,"</td>");
     }

   /****** Write user's ID ******/
   fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"DAT_SMALL\">",
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
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN ||
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER;

   /***** Start row *****/
   BgColor = Gbl.ColorRows[Gbl.RowEvenOdd];   // Two colors are used alternatively to better distinguish the rows
   fprintf (Gbl.F.Out,"<tr>");

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show student's photo *****/
      fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\">",BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,
                        ShowPhoto ? PhotoURL :
                                    NULL,
                        18,24,
                        Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW);
      fprintf (Gbl.F.Out,"</td>");
     }

   /****** Write user's ID ******/
   fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"%s\">",
            BgColor,
            UsrDat->Accepted ? "DAT_SMALL_N" :
        	               "DAT_SMALL");
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

   if (Gbl.Scope.Current == Sco_SCOPE_COURSE)
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

static void Usr_WriteRowTchMainData (unsigned NumUsr,struct UsrData *UsrDat,bool PutCheckboxToSelectUsr)
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
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN ||
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER;

   /***** Start row *****/
   fprintf (Gbl.F.Out,"<tr>");

   /***** Checkbox to select user *****/
   if (PutCheckboxToSelectUsr)
      UsrIsTheMsgSender = (UsrDat->UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);
   BgColor = UsrIsTheMsgSender ? LIGHT_GREEN :
	                         Gbl.ColorRows[Gbl.RowEvenOdd];   // Two colors are used alternatively to better distinguish the rows
   if (PutCheckboxToSelectUsr)
     {
      fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"middle\" bgcolor=\"%s\">",BgColor);
      Usr_PutCheckboxToSelectUser (Rol_ROLE_TEACHER,UsrDat->EncryptedUsrCod,UsrIsTheMsgSender);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Teacher has accepted enrollment in current course/in any course in degree/in any course? *****/
   fprintf (Gbl.F.Out,"<td class=\"");
   if (UsrIsTheMsgSender)
      fprintf (Gbl.F.Out,"BM_SEL");
   else
      fprintf (Gbl.F.Out,"BM%d",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\"><img src=\"%s/%s16x16.gif\""
	              " alt=\"\" title=\"%s\" class=\"ICON16x16\" />"
	              "</td>",
            Gbl.Prefs.IconsURL,
            UsrDat->Accepted ? "ok_on" :
        	               "tr",
            UsrDat->Accepted ? Txt_Enrollment_confirmed :
                               Txt_Enrollment_not_confirmed);

   /***** Write number of user *****/
   fprintf (Gbl.F.Out,"<td align=\"right\" bgcolor=\"%s\" class=\"DAT_SMALL_N\">&nbsp;%u&nbsp;</td>",
            BgColor,NumUsr);

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show teacher's photo *****/
      fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\">",BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,
                        ShowPhoto ? PhotoURL :
                                    NULL,
                        18,24,
	                Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Prepare data for brief presentation *****/
   Usr_RestrictLengthMainData (ShowEmail,UsrDat,MailLink);

   /****** Write the user's ID ******/
   fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"%s\">",
            BgColor,
            UsrDat->Accepted ? "DAT_SMALL_N" :
                               "DAT_SMALL");
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
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN ||
                    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER);
   struct Centre Ctr;
   struct Department Dpt;

   /***** Start row *****/
   BgColor = Gbl.ColorRows[Gbl.RowEvenOdd];   // Two colors are used alternatively to better distinguish the rows
   fprintf (Gbl.F.Out,"<tr>");
   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show teacher's photo *****/
      fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\">",BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,
                        ShowPhoto ? PhotoURL :
                                    NULL,
                        18,24,
	                Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW);
      fprintf (Gbl.F.Out,"</td>");
     }

   /****** Write the user's ID ******/
   fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"%s\">",
            BgColor,
            UsrDat->Accepted ? "DAT_SMALL_N" :
                               "DAT_SMALL");
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
   fprintf (Gbl.F.Out,"<td align=\"right\" bgcolor=\"%s\" class=\"DAT_SMALL_N\">&nbsp;%u&nbsp;</td>",BgColor,NumUsr);

   if (Gbl.Usrs.Listing.WithPhotos)
     {
      /***** Show administrator's photo *****/
      fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\">",BgColor);
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,
                        ShowPhoto ? PhotoURL :
                                    NULL,
                        18,24,
	                Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW);
      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Prepare data for brief presentation *****/
   Usr_RestrictLengthMainData (true,UsrDat,MailLink);

   /****** Write the user's ID ******/
   fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"%s\">",
            BgColor,
            UsrDat->Accepted ? "DAT_SMALL_N" :
                               "DAT_SMALL");
   ID_WriteUsrIDs (UsrDat,(Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER));
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
   fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"%s\">",BgColor,
            Accepted ? (NonBreak ? "DAT_SMALL_NOBR_N" :
        	                   "DAT_SMALL_N") :
                       (NonBreak ? "DAT_SMALL_NOBR" :
                	           "DAT_SMALL"));
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
   if (Search && Role == Rol_ROLE_GUEST)	// Special case
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
	 case Sco_SCOPE_PLATFORM:
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
	 case Sco_SCOPE_COUNTRY:
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
	 case Sco_SCOPE_INSTITUTION:
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
	 case Sco_SCOPE_CENTRE:
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
	 case Sco_SCOPE_DEGREE:
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
	 case Sco_SCOPE_COURSE:
	    Usr_BuildQueryToGetUsrsLstCrs (Role,UsrQuery,Search,Query);
	    break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong scope.");
	    break;
        }
/*
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
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
      case Sco_SCOPE_PLATFORM:
         strcpy (Query,"SELECT DISTINCT deg_admin.UsrCod,'Y',usr_data.Sex"
                       " FROM deg_admin,usr_data"
                       " WHERE deg_admin.UsrCod=usr_data.UsrCod "
                       " ORDER BY usr_data.Surname1,usr_data.Surname2,"
                       "usr_data.FirstName,usr_data.UsrCod");
         break;
      case Sco_SCOPE_INSTITUTION:
         sprintf (Query,"SELECT DISTINCT deg_admin.UsrCod,'Y',usr_data.Sex"
                        " FROM centres,degrees,deg_admin,usr_data"
                        " WHERE ((centres.InsCod='%ld' AND centres.CtrCod=degrees.CtrCod AND degrees.DegCod=deg_admin.DegCod) OR deg_admin.DegCod='%ld')"
                        " AND deg_admin.UsrCod=usr_data.UsrCod "
                        " ORDER BY usr_data.Surname1,usr_data.Surname2,"
                        "usr_data.FirstName,usr_data.UsrCod",
                  Gbl.CurrentIns.Ins.InsCod,
                  Usr_SPECIAL_CODE_GLOBAL_ADMIN);
         break;
      case Sco_SCOPE_CENTRE:
         sprintf (Query,"SELECT DISTINCT deg_admin.UsrCod,'Y',usr_data.Sex"
                        " FROM degrees,deg_admin,usr_data"
                        " WHERE ((degrees.CtrCod='%ld' AND degrees.DegCod=deg_admin.DegCod) OR deg_admin.DegCod='%ld')"
                        " AND deg_admin.UsrCod=usr_data.UsrCod "
                        " ORDER BY usr_data.Surname1,usr_data.Surname2,"
                        "usr_data.FirstName,usr_data.UsrCod",
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  Usr_SPECIAL_CODE_GLOBAL_ADMIN);
         break;
      case Sco_SCOPE_DEGREE:
         sprintf (Query,"SELECT DISTINCT deg_admin.UsrCod,'Y',usr_data.Sex"
                        " FROM deg_admin,usr_data"
                        " WHERE (deg_admin.DegCod='%ld' OR deg_admin.DegCod='%ld')"
                        " AND deg_admin.UsrCod=usr_data.UsrCod "
                        " ORDER BY usr_data.Surname1,usr_data.Surname2,"
                        "usr_data.FirstName,usr_data.UsrCod",
                  Gbl.CurrentDeg.Deg.DegCod,
                  Usr_SPECIAL_CODE_GLOBAL_ADMIN);
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
      case Sco_SCOPE_PLATFORM:
         strcpy (Query,"SELECT DISTINCT UsrCod,'N',Sex"
                       " FROM usr_data"
                       " WHERE UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
                       " ORDER BY Surname1,Surname2,FirstName,UsrCod");
         break;
      case Sco_SCOPE_COUNTRY:
         sprintf (Query,"SELECT DISTINCT UsrCod,'Y',Sex"
                        " FROM usr_data"
                        " WHERE (CtyCod='%ld' OR InsCtyCod='%ld')"
                        " AND UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
                        " ORDER BY Surname1,Surname2,FirstName,UsrCod",
                  Gbl.CurrentCty.Cty.CtyCod,
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INSTITUTION:
         sprintf (Query,"SELECT DISTINCT UsrCod,'Y',Sex"
                        " FROM usr_data"
                        " WHERE InsCod='%ld'"
                        " AND UsrCod NOT IN (SELECT UsrCod FROM crs_usr)"
                        " ORDER BY Surname1,Surname2,FirstName,UsrCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CENTRE:
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
if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
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

   fprintf (Gbl.F.Out,"<div align=\"center\">");

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
   fprintf (Gbl.F.Out,"</form>"
                      "</div>");
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
	              "<td align=\"left\" valign=\"middle\" class=\"%s\">",
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
   fprintf (Gbl.F.Out,"</form>"
	              "</td>"
	              "</tr>");

   /***** 2nd row *****/
   /* Put a button to select Usr_LIST */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"left\" valign=\"middle\" class=\"%s\">",
            Gbl.Usrs.Me.ListType == Usr_LIST ? "USR_LIST_TYPE_ON" :
        	                               "USR_LIST_TYPE_OFF");
   Usr_FormToSelectUsrListType (NextAction,Usr_LIST);

   /* See the photos in list? */
   Act_FormStart (NextAction);
   Grp_PutParamsCodGrps ();
   Usr_PutParamUsrListType (Usr_LIST);
   Usr_PutExtraParamsUsrList (NextAction);
   Usr_PutCheckboxListWithPhotos ();
   fprintf (Gbl.F.Out,"</form>"
	              "</td>"
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
                      " %s&nbsp;</a>"
                      "</form>",
            Gbl.Prefs.IconsURL,
            Usr_IconsClassPhotoOrList[ListType],
            Txt_USR_LIST_TYPES[ListType],
            Txt_USR_LIST_TYPES[ListType]);
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
   extern const char *Txt_ROLES_SINGULAR_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   struct ListUsers *LstUsrs;
   Usr_Sex_t Sex;

   fprintf (Gbl.F.Out,"<tr>"
	              "<td align=\"left\" colspan=\"%u\" bgcolor=\"%s\" class=\"TIT_TBL\">",
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
	    LstUsrs->NumUsrs == 1 ? Txt_ROLES_SINGULAR_Abc[Role][Sex] :
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
	 fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">&nbsp;</td>",
		  VERY_LIGHT_BLUE);

      /* Columns for the data */
      for (NumCol = 0;
           NumCol < Usr_NUM_MAIN_FIELDS_DATA_USR;
           NumCol++)
         if (NumCol != 2 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
            fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">%s&nbsp;</td>",
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
      Usr_ShowWarningNoUsersFound (Rol_ROLE_GUEST);

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
                            "<td colspan=\"%u\" align=\"center\" class=\"TIT\">",
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
	 fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">&nbsp;</td>",
		  VERY_LIGHT_BLUE);

      /* Columns for the data */
      for (NumCol = 0;
           NumCol < Usr_NUM_MAIN_FIELDS_DATA_USR;
           NumCol++)
         if (NumCol != 2 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
            fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">%s&nbsp;</td>",
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
	 fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">&nbsp;</td>",
		  VERY_LIGHT_BLUE);

      /* Columns for the data */
      for (NumCol = 0;
           NumCol < NumColumns;
           NumCol++)
         if (NumCol != 2 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
            fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">%s&nbsp;</td>",
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
      fprintf (Gbl.F.Out,"<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%%\">"
                         "<tr>");

      /* Columns for the data */
      for (NumCol = (Gbl.Usrs.Listing.WithPhotos ? 0 :
	                                           1);
           NumCol < NumColumnsCommonCard;
           NumCol++)
         fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">%s&nbsp;</td>",
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
      Usr_ShowWarningNoUsersFound (Rol_ROLE_GUEST);

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
      if (Gbl.Scope.Current == Sco_SCOPE_COURSE)
         /***** Get list of record fields in current course *****/
         Rec_GetListRecordFieldsInCurrentCrs ();

      /***** Set number of columns *****/
      NumColumnsCommonCard = Usr_NUM_ALL_FIELDS_DATA_STD;
      if (Gbl.Scope.Current == Sco_SCOPE_COURSE)
        {
         NumColumnsCardAndGroups = NumColumnsCommonCard + Gbl.CurrentCrs.Grps.GrpTypes.Num;
         NumColumnsTotal = NumColumnsCardAndGroups + Gbl.CurrentCrs.Records.LstFields.Num;
        }
      else
         NumColumnsTotal = NumColumnsCardAndGroups = NumColumnsCommonCard;

      /***** Allocate memory for the string with the list of group names where student belongs to *****/
      if (Gbl.Scope.Current == Sco_SCOPE_COURSE)
         if ((GroupNames = (char *) malloc ((MAX_LENGTH_GROUP_NAME+3)*Gbl.CurrentCrs.Grps.GrpTypes.NumGrpsTotal)) == NULL)
            Lay_ShowErrorAndExit ("Not enough memory to store names of groups.");

      /***** Start table with list of students *****/
      fprintf (Gbl.F.Out,"<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%%\">");
      if (!Gbl.Usrs.ClassPhoto.AllGroups)
        {
         fprintf (Gbl.F.Out,"<tr>"
	                    "<td colspan=\"%u\" align=\"center\" class=\"TIT\">",
                  NumColumnsTotal);
         Grp_WriteNamesOfSelectedGrps ();
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");
        }

      /***** Heading row with column names *****/
      /* Start row */
      fprintf (Gbl.F.Out,"<tr>");

      /* 1. Columns fot the data */
      for (NumCol = (Gbl.Usrs.Listing.WithPhotos ? 0 :
	                                           1);
           NumCol < NumColumnsCommonCard;
           NumCol++)
         fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">%s&nbsp;</td>",
                  VERY_LIGHT_BLUE,FieldNames[NumCol]);

      /* 2. Columns for the groups */
      if (Gbl.Scope.Current == Sco_SCOPE_COURSE)
        {
         if (Gbl.CurrentCrs.Grps.GrpTypes.Num)
            for (NumGrpTyp = 0;
                 NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
                 NumGrpTyp++)
               if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)         // If current course tiene groups of este type
                  fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">%s %s&nbsp;</td>",
                           VERY_LIGHT_BLUE,Txt_Group,
                           Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName);

         if (Gbl.CurrentCrs.Records.LstFields.Num)
           {
            /* 3. Names of record fields that depend on the course */
            for (NumField = 0;
                 NumField < Gbl.CurrentCrs.Records.LstFields.Num;
                 NumField++)
               fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">%s&nbsp;</td>",
                        VERY_LIGHT_BLUE,Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Name);
            /* 4. Visibility type for the record fields that depend on the course, in other row */
            fprintf (Gbl.F.Out,"</tr><tr>");
            for (NumCol = 0;
                 NumCol < NumColumnsCardAndGroups;
                 NumCol++)
               if (NumCol != 1 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want it in listing
                  fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\"></td>",LIGHTEST_BLUE);
            for (NumField = 0;
                 NumField < Gbl.CurrentCrs.Records.LstFields.Num;
                 NumField++)
               fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">(%s)&nbsp;</td>",
                        LIGHTEST_BLUE,Txt_RECORD_FIELD_VISIBILITY_RECORD[Gbl.CurrentCrs.Records.LstFields.Lst[NumField].Visibility]);
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
      if (Gbl.Scope.Current == Sco_SCOPE_COURSE)
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
                      "<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">&nbsp;</td>",
            VERY_LIGHT_BLUE);
   /* Columns for the data */
   for (NumCol = 0;
        NumCol < Usr_NUM_MAIN_FIELDS_DATA_USR;
        NumCol++)
      if (NumCol != 2 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
         fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">%s&nbsp;</td>",
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
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_PLATFORM    |
	               1 << Sco_SCOPE_COUNTRY     |
                       1 << Sco_SCOPE_INSTITUTION |
                       1 << Sco_SCOPE_CENTRE      |
                       1 << Sco_SCOPE_DEGREE      |
                       1 << Sco_SCOPE_COURSE;
   Gbl.Scope.Default = Sco_SCOPE_COURSE;
   Sco_GetScope ();

   /***** Get and order list of teachers *****/
   Usr_GetUsrsLst (Rol_ROLE_TEACHER,Gbl.Scope.Current,NULL,false);

   if (Gbl.Usrs.LstTchs.NumUsrs)
     {
      /***** Initialize number of columns *****/
      NumColumns = Usr_NUM_ALL_FIELDS_DATA_TCH;

      /***** Start table with list of teachers *****/
      /* Start row */
      fprintf (Gbl.F.Out,"<table cellspacing=\"0\" cellpadding=\"0\" width=\"100%%\">"
                         "<tr>");
      for (NumCol = (Gbl.Usrs.Listing.WithPhotos ? 0 :
	                                           1);
           NumCol < NumColumns;
           NumCol++)
         fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">%s&nbsp;</td>",
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
   extern const char *Txt_ROLES_SINGULAR_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
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
			 "<td align=\"center\" class=\"TIT_TBL\" colspan=\"%u\">",
	       Usr_NUM_MAIN_FIELDS_DATA_USR);
      if (NumUsrs == 1)
	 fprintf (Gbl.F.Out,"1 %s",
		  Txt_ROLES_SINGULAR_abc[Role][Sex]);
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
         fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">"
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
            if (Role != Rol_ROLE_GUEST)
              {
	       fprintf (Gbl.F.Out,"<tr>"
				  "<td align=\"left\" colspan=\"3\"></td>"
				  "<td align=\"left\" colspan=\"%u\">",
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
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_PLATFORM    |
	               1 << Sco_SCOPE_COUNTRY     |
                       1 << Sco_SCOPE_INSTITUTION |
                       1 << Sco_SCOPE_CENTRE      |
                       1 << Sco_SCOPE_DEGREE;
   Gbl.Scope.Default = Sco_SCOPE_DEGREE;
   Sco_GetScope ();

   /***** Form to select range of administrators *****/
   fprintf (Gbl.F.Out,"<div align=\"center\" class=\"%s\">"
	              "%s: ",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Scope);
   Act_FormStart (ActLstAdm);
   Sco_PutSelectorScope (true);
   Usr_PutParamListWithPhotos ();
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");

   /***** Get and order list of administrators *****/
   Usr_GetAdmsLst (Gbl.Scope.Current);

   if (Gbl.Usrs.LstAdms.NumUsrs)
     {
      /****** See the photos? *****/
      fprintf (Gbl.F.Out,"<div align=\"center\">");
      Act_FormStart (ActLstAdm);
      Sco_PutParamScope (Gbl.Scope.Current);
      Usr_PutCheckboxListWithPhotos ();
      fprintf (Gbl.F.Out,"</form>"
                         "</div>");

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
            fprintf (Gbl.F.Out,"<td align=\"left\" bgcolor=\"%s\" class=\"TIT_TBL\">%s&nbsp;</td>",
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
      Lay_ShowAlert (Lay_INFO,Txt_No_users_found[Rol_ROLE_DEG_ADMIN]);

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
      case Rol_ROLE_CTR_ADMIN:
      case Rol_ROLE_INS_ADMIN:
      case Rol_ROLE_SUPERUSER:
         /***** Form to select range of guests *****/
         fprintf (Gbl.F.Out,"<div align=\"center\" class=\"%s\">",
                  The_ClassFormul[Gbl.Prefs.Theme]);
         Act_FormStart (ActLstInv);
         Usr_PutParamUsrListType (Gbl.Usrs.Me.ListType);
         Usr_PutParamColsClassPhoto ();
         Usr_PutParamListWithPhotos ();
         fprintf (Gbl.F.Out,"%s: ",Txt_Scope);
         Sco_PutSelectorScope (true);
         fprintf (Gbl.F.Out,"</form>"
                            "</div>");
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
	       fprintf (Gbl.F.Out,"<div align=\"center\">");
	       Lay_PutLinkToPrintView1 (ActPrnInvPho);
	       Lay_PutLinkToPrintView2 ();
	       fprintf (Gbl.F.Out,"</div>");
	       break;
	    case Usr_LIST:
	       /****** Link to show all the data ******/
	       fprintf (Gbl.F.Out,"<div align=\"center\">");
	       Act_FormStart (ActLstInvAll);
	       Usr_PutParamListWithPhotos ();
	       Usr_PutExtraParamsUsrList (ActLstInvAll);
	       Act_LinkFormSubmit (Txt_Show_all_data,The_ClassFormul[Gbl.Prefs.Theme]);
	       Lay_PutSendIcon ("table",Txt_Show_all_data,Txt_Show_all_data);
	       fprintf (Gbl.F.Out,"</form>"
				  "</div>");
	       break;
           }

         /***** Draw a class photo with students of the course *****/
         fprintf (Gbl.F.Out,"<div align=\"center\">");

         /* Form start */
	 Act_FormStart (ActSeeRecSevInv);
	 Grp_PutParamsCodGrps ();

         /* Header */
         Lay_StartRoundFrameTable10 (NULL,0,NULL);

         if (Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO)
	    Lay_WriteHeaderClassPhoto (Gbl.Usrs.ClassPhoto.Cols,false,true,
				       (Gbl.Scope.Current == Sco_SCOPE_CENTRE ||
					Gbl.Scope.Current == Sco_SCOPE_INSTITUTION) ? Gbl.CurrentIns.Ins.InsCod :
					                                              -1L,
				       -1L,
				       -1L);

	 /* Put a row to select all users */
         Usr_PutCheckboxToSelectAllTheUsers (Rol_ROLE_GUEST);

         /* Draw the classphoto/list */
         switch (Gbl.Usrs.Me.ListType)
           {
            case Usr_CLASS_PHOTO:
               Usr_DrawClassPhoto (Usr_CLASS_PHOTO_SEL_SEE,
        	                   Rol_ROLE_GUEST);
               break;
            case Usr_LIST:
               Usr_ListMainDataGsts (true);
               break;
           }

         Lay_EndRoundFrameTable10 ();

         /* Send button */
         Lay_PutSendButton (Txt_Show_records);
         fprintf (Gbl.F.Out,"</form>"
                            "</div>");
	}
     }
   else
      Usr_ShowWarningNoUsersFound (Rol_ROLE_GUEST);

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
        Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER))	// ...or superuser
     {
      fprintf (Gbl.F.Out,"<div align=\"center\" style=\"margin-bottom:10px;\">");

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
   ICanViewRecords = (Gbl.Scope.Current == Sco_SCOPE_COURSE &&
	              (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_STUDENT ||
                       Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER ||
                       Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN ||
	               Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER));

   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_DEG_ADMIN:
      case Rol_ROLE_SUPERUSER:
         /***** Form to select range of students *****/
         fprintf (Gbl.F.Out,"<div align=\"center\" class=\"%s\">",
                  The_ClassFormul[Gbl.Prefs.Theme]);
         Act_FormStart (ActLstStd);
         Usr_PutParamUsrListType (Gbl.Usrs.Me.ListType);
         Usr_PutParamColsClassPhoto ();
         Usr_PutParamListWithPhotos ();
         fprintf (Gbl.F.Out,"%s: ",Txt_Scope);
         Sco_PutSelectorScope (true);
         fprintf (Gbl.F.Out,"</form>"
                            "</div>");
         break;
      default:
         break;
     }

   /***** Form to select groups *****/
   if (Gbl.Scope.Current == Sco_SCOPE_COURSE)
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
	       fprintf (Gbl.F.Out,"<div align=\"center\">");
	       Lay_PutLinkToPrintView1 (ActPrnStdPho);
	       Grp_PutParamsCodGrps ();
	       Lay_PutLinkToPrintView2 ();
	       fprintf (Gbl.F.Out,"</div>");
	       break;
	    case Usr_LIST:
	       if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER)
		 {
		  /****** Link to show all the data ******/
		  fprintf (Gbl.F.Out,"<div align=\"center\">");
		  Act_FormStart (ActLstStdAll);
		  Grp_PutParamsCodGrps ();
		  Usr_PutParamListWithPhotos ();
		  Usr_PutExtraParamsUsrList (ActLstStdAll);
		  Act_LinkFormSubmit (Txt_Show_all_data,The_ClassFormul[Gbl.Prefs.Theme]);
		  Lay_PutSendIcon ("table",Txt_Show_all_data,Txt_Show_all_data);
		  fprintf (Gbl.F.Out,"</form>"
				     "</div>");
		 }
	       break;
           }

         /***** Draw a class photo with students of the course *****/
         fprintf (Gbl.F.Out,"<div align=\"center\">");

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
				       (Gbl.Scope.Current == Sco_SCOPE_COURSE ||
					Gbl.Scope.Current == Sco_SCOPE_DEGREE ||
					Gbl.Scope.Current == Sco_SCOPE_CENTRE ||
					Gbl.Scope.Current == Sco_SCOPE_INSTITUTION) ? Gbl.CurrentIns.Ins.InsCod :
					                                              -1L,
				       (Gbl.Scope.Current == Sco_SCOPE_COURSE ||
					Gbl.Scope.Current == Sco_SCOPE_DEGREE) ? Gbl.CurrentDeg.Deg.DegCod :
					                                         -1L,
					Gbl.Scope.Current == Sco_SCOPE_COURSE ? Gbl.CurrentCrs.Crs.CrsCod :
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
            fprintf (Gbl.F.Out,"</form>");
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
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_PLATFORM    |
	               1 << Sco_SCOPE_COUNTRY     |
                       1 << Sco_SCOPE_INSTITUTION |
                       1 << Sco_SCOPE_CENTRE      |
                       1 << Sco_SCOPE_DEGREE      |
                       1 << Sco_SCOPE_COURSE;
   Gbl.Scope.Default = Sco_SCOPE_COURSE;
   Sco_GetScope ();
   ICanViewRecords = (Gbl.Scope.Current == Sco_SCOPE_COURSE);

   /***** Form to select scope *****/
   fprintf (Gbl.F.Out,"<div align=\"center\" class=\"%s\">",
	    The_ClassFormul[Gbl.Prefs.Theme]);
   Act_FormStart (ActLstTch);
   Usr_PutParamUsrListType (Gbl.Usrs.Me.ListType);
   Usr_PutParamColsClassPhoto ();
   Usr_PutParamListWithPhotos ();
   fprintf (Gbl.F.Out,"%s: ",Txt_Scope);
   Sco_PutSelectorScope (true);
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");

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
               fprintf (Gbl.F.Out,"<div align=\"center\">");
	       Lay_PutLinkToPrintView1 (ActPrnTchPho);
	       Sco_PutParamScope (Gbl.Scope.Current);
	       Lay_PutLinkToPrintView2 ();
	       fprintf (Gbl.F.Out,"</div>");
	       break;
            case Usr_LIST:
	       if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_TEACHER)
		 {
		  /****** Link to show all the data ******/
		  fprintf (Gbl.F.Out,"<div align=\"center\">");
		  Act_FormStart (ActLstTchAll);
		  Sco_PutParamScope (Gbl.Scope.Current);
		  Usr_PutParamListWithPhotos ();
		  Act_LinkFormSubmit (Txt_Show_all_data,The_ClassFormul[Gbl.Prefs.Theme]);
		  Lay_PutSendIcon ("table",Txt_Show_all_data,Txt_Show_all_data);
		  fprintf (Gbl.F.Out,"</form>"
				     "</div>");
		 }
               break;
           }

         /***** Draw a class photo with teachers of the course *****/
         fprintf (Gbl.F.Out,"<div align=\"center\">");

         /* Form start */
         if (ICanViewRecords)
            Act_FormStart (ActSeeRecSevTch);

         /* Header */
         Lay_StartRoundFrameTable10 (NULL,0,NULL);

         if (Gbl.Usrs.Me.ListType == Usr_CLASS_PHOTO)
	    Lay_WriteHeaderClassPhoto (Gbl.Usrs.ClassPhoto.Cols,false,true,
				       (Gbl.Scope.Current == Sco_SCOPE_COURSE ||
					Gbl.Scope.Current == Sco_SCOPE_DEGREE ||
					Gbl.Scope.Current == Sco_SCOPE_CENTRE ||
					Gbl.Scope.Current == Sco_SCOPE_INSTITUTION) ? Gbl.CurrentIns.Ins.InsCod :
					                                              -1L,
				       (Gbl.Scope.Current == Sco_SCOPE_COURSE ||
					Gbl.Scope.Current == Sco_SCOPE_DEGREE) ? Gbl.CurrentDeg.Deg.DegCod :
					                                         -1L,
					Gbl.Scope.Current == Sco_SCOPE_COURSE ? Gbl.CurrentCrs.Crs.CrsCod :
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
            fprintf (Gbl.F.Out,"</form>");
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

   if (Cfg_EXTERNAL_LOGIN_CLIENT_COMMAND[0] &&		// There is an external service for authentication and official lists
       Gbl.Imported.ExternalUsrId[0] &&			// I was authenticated from external service...
       Gbl.Imported.ExternalSesId[0] &&
       Gbl.Imported.ExternalRole == Rol_ROLE_TEACHER)	// ...as a teacher
     {
      /***** Link to list official students *****/
      Act_FormStart (ActGetExtLstStd);
      Act_LinkFormSubmit (Txt_Official_students,The_ClassFormul[Gbl.Prefs.Theme]);
      Lay_PutSendIcon ("list",Txt_Official_students,Txt_Official_students);
      fprintf (Gbl.F.Out,"</form>");
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
				 (Gbl.Scope.Current == Sco_SCOPE_CENTRE ||
				  Gbl.Scope.Current == Sco_SCOPE_INSTITUTION) ? Gbl.CurrentIns.Ins.InsCod :
                                                                                -1L,
				 -1L,-1L);
      Usr_DrawClassPhoto (Usr_CLASS_PHOTO_PRN,Rol_ROLE_GUEST);
      Lay_EndRoundFrameTable10 ();
     }
   else
      Usr_ShowWarningNoUsersFound (Rol_ROLE_GUEST);

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
				 (Gbl.Scope.Current == Sco_SCOPE_COURSE ||
				  Gbl.Scope.Current == Sco_SCOPE_DEGREE ||
				  Gbl.Scope.Current == Sco_SCOPE_CENTRE ||
				  Gbl.Scope.Current == Sco_SCOPE_INSTITUTION) ? Gbl.CurrentIns.Ins.InsCod :
					                                        -1L,
				 (Gbl.Scope.Current == Sco_SCOPE_COURSE ||
				  Gbl.Scope.Current == Sco_SCOPE_DEGREE) ? Gbl.CurrentDeg.Deg.DegCod :
					                                   -1L,
				  Gbl.Scope.Current == Sco_SCOPE_COURSE ? Gbl.CurrentCrs.Crs.CrsCod :
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
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_PLATFORM    |
	               1 << Sco_SCOPE_COUNTRY     |
                       1 << Sco_SCOPE_INSTITUTION |
                       1 << Sco_SCOPE_CENTRE      |
                       1 << Sco_SCOPE_DEGREE      |
                       1 << Sco_SCOPE_COURSE;
   Gbl.Scope.Default = Sco_SCOPE_COURSE;
   Sco_GetScope ();

   /***** Get and order list of teachers *****/
   Usr_GetUsrsLst (Rol_ROLE_TEACHER,Gbl.Scope.Current,NULL,false);

   if (Gbl.Usrs.LstTchs.NumUsrs)
     {
      /***** Draw the teachers' class photo *****/
      Lay_StartRoundFrameTable10 (NULL,0,NULL);
      Lay_WriteHeaderClassPhoto (Gbl.Usrs.ClassPhoto.Cols,true,true,
				 (Gbl.Scope.Current == Sco_SCOPE_COURSE ||
				  Gbl.Scope.Current == Sco_SCOPE_DEGREE ||
				  Gbl.Scope.Current == Sco_SCOPE_CENTRE ||
				  Gbl.Scope.Current == Sco_SCOPE_INSTITUTION) ? Gbl.CurrentIns.Ins.InsCod :
					                                        -1L,
				 (Gbl.Scope.Current == Sco_SCOPE_COURSE ||
				  Gbl.Scope.Current == Sco_SCOPE_DEGREE) ? Gbl.CurrentDeg.Deg.DegCod :
					                                   -1L,
				  Gbl.Scope.Current == Sco_SCOPE_COURSE ? Gbl.CurrentCrs.Crs.CrsCod :
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
   extern const char *Txt_to_see_photos_of_others_you_have_to_allow_others_to_see_your_photo_you_can_do_it_in_X;
   extern const char *Txt_to_see_photos_of_others_you_have_to_send_your_photo_you_can_do_it_in_X;
   struct ListUsers *LstUsrs;
   unsigned NumUsr;
   bool TRIsOpen = false;
   bool IAmLoggedAsTeacherOrAbove = (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER   ||
                                     Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN ||
                                     Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER);
   bool ICanSeePhotos = (IAmLoggedAsTeacherOrAbove ||
                         (Gbl.Usrs.Me.MyPhotoExists &&
                          Gbl.Usrs.Me.UsrDat.PublicPhoto));
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
   int PhotoWidth = 36;		// Default photo width
   int PhotoHeight = 48;	// Default photo height
   int LengthUsrData = 10;	// Maximum number of characters of user data
   char PhotoURL[PATH_MAX+1];
   char BreadcrumbStr[512];
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
         PhotoWidth = 18; PhotoHeight = 24; LengthUsrData = 10;
         break;
      case Usr_CLASS_PHOTO_SEL_SEE:
      case Usr_CLASS_PHOTO_SEE:
         PhotoWidth = 36; PhotoHeight = 48; LengthUsrData = 10;
         break;
      case Usr_CLASS_PHOTO_PRN:
         PhotoWidth = 36; PhotoHeight = 48; LengthUsrData = 15;
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
                     Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN ||
                     Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER);

         /***** Begin user's cell *****/
         fprintf (Gbl.F.Out,"<td align=\"center\" valign=\"bottom\" class=\"CLASSPHOTO\"");
         if (ClassPhotoType == Usr_CLASS_PHOTO_SEL &&
             UsrDat.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod)
           {
            UsrIsTheMsgSender = true;
            fprintf (Gbl.F.Out," bgcolor=\"%s\"",LIGHT_GREEN);
           }
         else
            UsrIsTheMsgSender = false;
         fprintf (Gbl.F.Out,">");

         /***** Checkbox to select this user *****/
         if (PutCheckBoxToSelectUsr)
            Usr_PutCheckboxToSelectUser (RoleInClassPhoto,UsrDat.EncryptedUsrCod,UsrIsTheMsgSender);

         /***** Show photo *****/
         ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
         Pho_ShowUsrPhoto (&UsrDat,
                           ShowPhoto ? PhotoURL :
                        	       NULL,
                           PhotoWidth,PhotoHeight,
                           Act_Actions[Gbl.CurrentAct].BrowserWindow == Act_MAIN_WINDOW);

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

   /***** Write message in case of the user can not view the photos of others *****/
   if (!ICanSeePhotos && RoleInClassPhoto == Rol_ROLE_STUDENT &&
       (ClassPhotoType == Usr_CLASS_PHOTO_SEE ||
        ClassPhotoType == Usr_CLASS_PHOTO_SEL))
     {
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td colspan=\"%d\" align=\"center\" class=\"MSJ_AVISO\">(",
               Gbl.Usrs.ClassPhoto.Cols);
      if (Gbl.Usrs.Me.MyPhotoExists)
        {
         Act_GetBreadcrumbStrForAction (ActEdiPrf,true,BreadcrumbStr);
         fprintf (Gbl.F.Out,Txt_to_see_photos_of_others_you_have_to_allow_others_to_see_your_photo_you_can_do_it_in_X,
                  BreadcrumbStr);
        }
      else
        {
         Act_GetBreadcrumbStrForAction (ActReqMyPho,true,BreadcrumbStr);
         fprintf (Gbl.F.Out,Txt_to_see_photos_of_others_you_have_to_send_your_photo_you_can_do_it_in_X,
                  BreadcrumbStr);
        }
      fprintf (Gbl.F.Out,")</td>"
	                 "</tr>");
     }
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
/***** Register/remove users (taken from a list) in/from current course ******/
/*****************************************************************************/

void Usr_ShowFormRegRemSeveralUsrs (void)
  {
   extern const char *The_ClassTitle[The_NUM_THEMES];
   extern const char *Txt_Step_1_Provide_a_list_of_users;
   extern const char *Txt_Option_a_Import_students_from_the_official_lists;
   extern const char *Txt_There_is_no_external_service_for_authentication_and_official_lists;
   extern const char *Txt_Select_the_groups_of_students_you_want_to_register_in_remove_from_this_course;
   extern const char *Txt_Option_b_Type_or_paste_a_list_of_users;
   extern const char *Txt_Type_or_paste_a_list_of_IDs_nicks_or_emails_;
   extern const char *Txt_Step_2_Select_the_type_of_user_to_register_remove;
   extern const char *Txt_Step_3_Select_the_desired_action;
   extern const char *Txt_Step_4_Optionally_select_groups;
   extern const char *Txt_Select_the_groups_in_from_which_you_want_to_register_remove_users_;
   extern const char *Txt_No_groups_have_been_created_in_the_course_X_Therefore_;
   extern const char *Txt_Step_5_Confirm_the_enrollment_removing;
   extern const char *Txt_Confirm;

   fprintf (Gbl.F.Out,"<div align=\"center\">");

   /***** Put link to remove all the students in the current course *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
      Usr_PutFormToRemAllStdsThisCrs ();

   /***** Put link to remove old users *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
      Usr_PutFormToRemOldUsrs ();

   /***** Form to send students to be enrolled / removed *****/
   Act_FormStart (ActRcvFrmMdfUsrCrs);

   /***** Step 1: List of students to be enrolled / removed *****/
   fprintf (Gbl.F.Out,"<div align=\"left\" class=\"%s\">"
                      "<br />%s</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Step_1_Provide_a_list_of_users);

   /* Option a: get students from official lists */
   fprintf (Gbl.F.Out,"<div align=\"left\" class=\"%s\">"
                      "<br />%s<br />&nbsp;</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Option_a_Import_students_from_the_official_lists);
   if (Cfg_EXTERNAL_LOGIN_CLIENT_COMMAND[0] == '\0')
      Lay_ShowAlert (Lay_INFO,Txt_There_is_no_external_service_for_authentication_and_official_lists);
   else
     {
      Lay_ShowAlert (Lay_INFO,Txt_Select_the_groups_of_students_you_want_to_register_in_remove_from_this_course);
      Imp_ListMyImpGrpsAndStdsForm ();
     }

   /* Option b: get students' IDs from pasted text */
   fprintf (Gbl.F.Out,"<div align=\"left\" class=\"%s\">"
                      "<br />%s<br />&nbsp;</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Option_b_Type_or_paste_a_list_of_users);
   Lay_ShowAlert (Lay_INFO,Txt_Type_or_paste_a_list_of_IDs_nicks_or_emails_);
   Usr_PutAreaToEnterUsrsIDs ();

   /***** Step 2: Select type of user to register/remove to/from current course *****/
   fprintf (Gbl.F.Out,"<div align=\"left\" class=\"%s\">"
                      "<br />%s<br />&nbsp;</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Step_2_Select_the_type_of_user_to_register_remove);
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
      Usr_PutAllRolesRegRemUsrsCrs ();

   /***** Step 3: Put different actions to register/remove students to/from current course *****/
   fprintf (Gbl.F.Out,"<div align=\"left\" class=\"%s\">"
                      "<br />%s<br />&nbsp;</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Step_3_Select_the_desired_action);
   Usr_PutActionsRegRemSeveralUsrs ();

   /***** Step 4: Select groups in which register / remove students *****/
   fprintf (Gbl.F.Out,"<div align=\"left\" class=\"%s\">"
                      "<br />%s<br />&nbsp;</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Step_4_Optionally_select_groups);
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
     {
      if (Gbl.CurrentCrs.Grps.NumGrps)	// This course has groups?
	{
	 Lay_ShowAlert (Lay_INFO,Txt_Select_the_groups_in_from_which_you_want_to_register_remove_users_);
	 Grp_ShowLstGrpsToChgOtherUsrsGrps (-1L);
	}
      else
	{
	 /* Write help message */
	 sprintf (Gbl.Message,Txt_No_groups_have_been_created_in_the_course_X_Therefore_,
		  Gbl.CurrentCrs.Crs.FullName);
	 Lay_ShowAlert (Lay_INFO,Gbl.Message);
	}
     }

   /***** Step 5: Button to register / remove students *****/
   fprintf (Gbl.F.Out,"<div align=\"left\" class=\"%s\">"
                      "<br />%s<br />&nbsp;</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Step_5_Confirm_the_enrollment_removing);
   Pwd_AskForConfirmationOnDangerousAction ();
   Lay_PutSendButton (Txt_Confirm);

   /***** End of form *****/
   fprintf (Gbl.F.Out,"</form>"
                      "</div>");
  }

/*****************************************************************************/
/***** Put text area to enter/paste IDs of users to be enrolled/removed ******/
/*****************************************************************************/

static void Usr_PutAreaToEnterUsrsIDs (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_List_of_nicks_emails_or_IDs;

   /***** Start of text area *****/
   fprintf (Gbl.F.Out,"<table>"
                      "<tr>"
                      "<td align=\"right\" valign=\"top\" class=\"%s\">%s: </td>"
                      "<td align=\"left\">"
                      "<textarea name=\"UsrsIDs\" cols=\"50\" rows=\"10\">",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_List_of_nicks_emails_or_IDs);

   /***** End of text area *****/
   fprintf (Gbl.F.Out,"</textarea>"
                      "</td>"
                      "</tr>"
                      "</table>");
  }

/*****************************************************************************/
/*** Put different actions to register/remove users to/from current course ***/
/*****************************************************************************/
// Returns true if at least one action can be shown

static bool Usr_PutActionsRegRemOneUsr (bool ItsMe)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Modify_me_in_the_course_X;
   extern const char *Txt_Modify_user_in_the_course_X;
   extern const char *Txt_Register_me_in_the_course_X;
   extern const char *Txt_Register_user_in_the_course_X;
   extern const char *Txt_Register_user_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Remove_me_from_the_course_X;
   extern const char *Txt_Remove_user_from_the_course_X;
   extern const char *Txt_Remove_me_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Remove_user_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Completely_eliminate_me;
   extern const char *Txt_Completely_eliminate_user;
   unsigned NumOptionsShown = 0;
   bool UsrBelongsToCrs = false;
   bool UsrIsDegAdmin = false;
   bool OptionChecked = false;

   /***** Check if the other user belongs to the current course *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
      UsrBelongsToCrs = Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod);

   /***** Check if the other user is administrator of the current degree *****/
   if (Gbl.CurrentDeg.Deg.DegCod > 0 &&
      ((Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN && ItsMe) ||
        Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER))
      UsrIsDegAdmin = Usr_CheckIfUsrIsAdmOfDeg (Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.CurrentDeg.Deg.DegCod);

   /***** Start list of options *****/
   fprintf (Gbl.F.Out,"<div style=\"display:inline-block; margin:0 auto;\">"
                      "<ul style=\"list-style-type:none; text-align:left;\" class=\"%s\">",
            The_ClassFormul[Gbl.Prefs.Theme]);

   /***** Register user in course / Modify user's data *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      sprintf (Gbl.Message,UsrBelongsToCrs ? (ItsMe ? Txt_Modify_me_in_the_course_X :
		                                      Txt_Modify_user_in_the_course_X) :
	                                     (ItsMe ? Txt_Register_me_in_the_course_X :
		                                      Txt_Register_user_in_the_course_X),
	       Gbl.CurrentCrs.Crs.ShortName);
      fprintf (Gbl.F.Out,"<li>"
			 "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\"",
               (unsigned) Usr_REGISTER_MODIFY_ONE_USR_IN_CRS);
      if (!OptionChecked)
	{
	 fprintf (Gbl.F.Out," checked=\"checked\"");
         OptionChecked = true;
	}
      fprintf (Gbl.F.Out," />%s</li>",Gbl.Message);

      NumOptionsShown++;
     }

   /***** Register user as administrator of degree *****/
   if (Gbl.CurrentDeg.Deg.DegCod > 0 &&
       !UsrIsDegAdmin &&
       Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
     {
      sprintf (Gbl.Message,Txt_Register_user_as_an_administrator_of_the_degree_X,
	       Gbl.CurrentDeg.Deg.ShortName);
      fprintf (Gbl.F.Out,"<li>"
			 "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\"",
	       (unsigned) Usr_REGISTER_ONE_DEGREE_ADMIN);
      if (!OptionChecked)
	{
	 fprintf (Gbl.F.Out," checked=\"checked\"");
         OptionChecked = true;
	}
      fprintf (Gbl.F.Out," />%s</li>",Gbl.Message);

      NumOptionsShown++;
     }

   /***** Remove user from the course *****/
   if (UsrBelongsToCrs)
     {
      sprintf (Gbl.Message,
	       ItsMe ? Txt_Remove_me_from_the_course_X :
		       Txt_Remove_user_from_the_course_X,
	       Gbl.CurrentCrs.Crs.ShortName);
      fprintf (Gbl.F.Out,"<li>"
			 "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\"",
	       (unsigned) Usr_REMOVE_ONE_USR_FROM_CRS);
      if (!OptionChecked)
	{
	 fprintf (Gbl.F.Out," checked=\"checked\"");
	 OptionChecked = true;
	}
      fprintf (Gbl.F.Out," />%s</li>",Gbl.Message);

      NumOptionsShown++;
     }

   /***** Remove user as an administrator of the degree *****/
   if (Gbl.CurrentDeg.Deg.DegCod > 0 &&
       UsrIsDegAdmin &&
       ((Gbl.Usrs.Me.LoggedRole == Rol_ROLE_DEG_ADMIN && ItsMe) ||
         Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER))
     {
      sprintf (Gbl.Message,
               ItsMe ? Txt_Remove_me_as_an_administrator_of_the_degree_X :
		       Txt_Remove_user_as_an_administrator_of_the_degree_X,
	       Gbl.CurrentDeg.Deg.ShortName);
      fprintf (Gbl.F.Out,"<li>"
			 "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\"",
	       (unsigned) Usr_REMOVE_ONE_DEGREE_ADMIN);
      if (!OptionChecked)
	{
	 fprintf (Gbl.F.Out," checked=\"checked\"");
         OptionChecked = true;
	}
      fprintf (Gbl.F.Out," />%s</li>",Gbl.Message);

      NumOptionsShown++;
     }

   /***** Eliminate user completely from platform *****/
   if (Usr_CheckIfICanEliminateAccount (ItsMe))
     {
      fprintf (Gbl.F.Out,"<li>"
                         "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\"",
               (unsigned) Usr_ELIMINATE_ONE_USR_FROM_PLATFORM);
      if (!OptionChecked)
	 fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," />%s</li>",
               ItsMe ? Txt_Completely_eliminate_me :
        	       Txt_Completely_eliminate_user);

      NumOptionsShown++;
     }

   /***** End list of options *****/
   fprintf (Gbl.F.Out,"</ul>"
                      "</div>");

   return (NumOptionsShown ? true :
	                     false);
  }

/*****************************************************************************/
/******** Check if I can eliminate completely another user's account *********/
/*****************************************************************************/

static bool Usr_CheckIfICanEliminateAccount (bool ItsMe)
  {
   // A user logged as superuser can eliminate any user except her/him
   // Other users only can eliminate themselves
   return (( ItsMe &&								// It's me
	    (Gbl.Usrs.Me.AvailableRoles & (1 << Rol_ROLE_SUPERUSER)) == 0)	// I can not be superuser
	   ||
           (!ItsMe &&								// It's not me
             Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER));			// I am logged as superuser
  }

/*****************************************************************************/
/*** Put different actions to register/remove users to/from current course ***/
/*****************************************************************************/

static void Usr_PutActionsRegRemSeveralUsrs (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Register_the_users_indicated_in_step_1;
   extern const char *Txt_Remove_the_users_indicated_in_step_1;
   extern const char *Txt_Remove_the_users_not_indicated_in_step_1;
   extern const char *Txt_Register_the_users_indicated_in_step_1_and_remove_the_users_not_indicated;
   extern const char *Txt_Eliminate_from_the_platform_the_users_indicated_in_step_1;

   /***** Start list of options *****/
   fprintf (Gbl.F.Out,"<div style=\"display:inline-block; margin:0 auto;\">"
                      "<ul style=\"list-style-type:none; text-align:left;\" class=\"%s\">",
            The_ClassFormul[Gbl.Prefs.Theme]);

   /***** Register / remove users listed or not listed *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
      fprintf (Gbl.F.Out,"<li>"
			 "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\" checked=\"checked\" />"
			 "%s</li>"
			 "<li>"
			 "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\" />"
			 "%s</li>"
			 "<li>"
			 "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\" />"
			 "%s</li>"
			 "<li>"
			 "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\" />"
			 "%s</li>",
	       (unsigned) Usr_REGISTER_SPECIFIED_USRS_IN_CRS,
	       Txt_Register_the_users_indicated_in_step_1,
	       (unsigned) Usr_REMOVE_SPECIFIED_USRS_FROM_CRS,
	       Txt_Remove_the_users_indicated_in_step_1,
	       (unsigned) Usr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS,
	       Txt_Remove_the_users_not_indicated_in_step_1,
	       (unsigned) Usr_UPDATE_USRS_IN_CRS,
	       Txt_Register_the_users_indicated_in_step_1_and_remove_the_users_not_indicated);

   /***** Only for superusers *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
      fprintf (Gbl.F.Out,"<li>"
                         "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\" />"
                         "%s</li>",
               (unsigned) Usr_ELIMINATE_USRS_FROM_PLATFORM,
               Txt_Eliminate_from_the_platform_the_users_indicated_in_step_1);

   /***** End list of options *****/
   fprintf (Gbl.F.Out,"</ul>"
                      "</div>");
  }

/*****************************************************************************/
/*** Select types of user to register/remove users to/from current course ****/
/*****************************************************************************/

static void Usr_PutAllRolesRegRemUsrsCrs (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];

   /***** Students or teachers *****/
   fprintf (Gbl.F.Out,"<div style=\"display:inline-block; margin:0 auto;\">"
                      "<ul style=\"list-style-type:none; text-align:left;\" class=\"%s\">",
            The_ClassFormul[Gbl.Prefs.Theme]);
   Usr_PutOneRoleRegRemUsrsCrs (Rol_ROLE_STUDENT,true);
   Usr_PutOneRoleRegRemUsrsCrs (Rol_ROLE_TEACHER,false);
   fprintf (Gbl.F.Out,"</ul>"
                      "</div>");
  }

/*****************************************************************************/
/*** Select types of user to register/remove users to/from current course ****/
/*****************************************************************************/

static void Usr_PutOneRoleRegRemUsrsCrs (Rol_Role_t Role,bool Checked)
  {
   extern const char *Txt_ROLES_SINGULAR_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   fprintf (Gbl.F.Out,"<li>"
	              "<input type=\"radio\" name=\"RegRemRole\" value=\"%u\"",
            (unsigned) Role);
   if (Checked)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   switch (Gbl.Usrs.Me.LoggedRole)      // Can I select type of user?
     {
      case Rol_ROLE_DEG_ADMIN:
      case Rol_ROLE_SUPERUSER:                // Yes, I can
         break;
      default:                                // No, I can not
         fprintf (Gbl.F.Out," disabled=\"disabled\"");
         break;
     }
   fprintf (Gbl.F.Out," />%s</li>",
            Txt_ROLES_SINGULAR_Abc[Role][Usr_SEX_UNKNOWN]);
  }

/*****************************************************************************/
/******* Receive the list of users of the course to register/remove **********/
/*****************************************************************************/

void Usr_ReceiveFormUsrsCrs (void)
  {
   extern const char *Txt_You_must_specify_in_step_3_the_action_to_perform;
   extern const char *Txt_In_a_type_of_group_with_single_enrollment_students_can_not_be_registered_in_more_than_one_group;
   extern const char *Txt_No_user_has_been_eliminated;
   extern const char *Txt_One_user_has_been_eliminated;
   extern const char *Txt_No_user_has_been_removed;
   extern const char *Txt_One_user_has_been_removed;
   extern const char *Txt_X_users_have_been_eliminated;
   extern const char *Txt_X_users_have_been_removed;
   extern const char *Txt_No_user_has_been_enrolled;
   extern const char *Txt_One_user_has_been_enrolled;
   extern const char *Txt_X_users_have_been_enrolled_including_possible_repetitions;
   char ListExternalGrpCods[Imp_MAX_BYTES_LIST_EXTERNAL_GRP_CODS+1];
   char ExternalStr[1+10+1+Crs_LENGTH_INSTITUTIONAL_CRS_COD+1];
   char ExternalCrsCod[Crs_LENGTH_INSTITUTIONAL_CRS_COD+1];
   char UnsignedStr[10+1];
   unsigned UnsignedNum;
   long LongNum;
   Rol_Role_t RegRemRole = Rol_ROLE_STUDENT;
   struct
     {
      bool RemoveUsrs;
      bool RemoveSpecifiedUsrs;
      bool EliminateUsrs;
      bool RegisterUsrs;
     } WhatToDo;
   char *ListUsrsIDs;
   struct ListUsrCods ListUsrCods;	// List with users' codes for a given user's ID
   unsigned NumUsrFound;
   const char *Ptr;
   unsigned NumCurrentUsr;
   long GrpCod;
   unsigned NumUsrsRegistered = 0;
   unsigned NumUsrsRemoved = 0;
   unsigned NumUsrsEliminated = 0;
   struct ListUsers *LstCurrentUsrs;
   struct ListCodGrps LstGrps;
   struct UsrData UsrDat;
   bool ItLooksLikeAUsrID;
   Usr_RegRemUsrsAction_t RegRemUsrsAction;
   bool ErrorInForm = false;

   /***** Get confirmation *****/
   if (!Pwd_GetConfirmationOnDangerousAction ())
      return;

   /***** Get the type of user to register / remove *****/
   Par_GetParToText ("RegRemRole",UnsignedStr,1);
   if (UnsignedStr[0])
      switch ((RegRemRole = Usr_ConvertUnsignedStrToRole (UnsignedStr)))
        {
         case Rol_ROLE_STUDENT:
            break;
         case Rol_ROLE_TEACHER:
            switch (Gbl.Usrs.Me.LoggedRole)        // Can I register/remove teachers?
              {
               case Rol_ROLE_DEG_ADMIN:
               case Rol_ROLE_SUPERUSER:                // Yes, I can
                  break;
               default:                                // No, I can not (TODO: teachers should be able to register/remove existing teachers)
                  Lay_ShowErrorAndExit ("You are not allowed to perform this action.");	// If user manipulated the form
                  break;
              }
            break;
         default:
            Lay_ShowErrorAndExit ("Wrong role.");				// If user manipulated the form
            break;
        }

   /***** Get the action to do *****/
   WhatToDo.RemoveUsrs = false;
   WhatToDo.RemoveSpecifiedUsrs = false;
   WhatToDo.EliminateUsrs = false;
   WhatToDo.RegisterUsrs = false;

   Par_GetParToText ("RegRemAction",UnsignedStr,1);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Usr_NUM_ACTIONS_REG_REM_USRS)
	 switch (RegRemUsrsAction = (Usr_RegRemUsrsAction_t) UnsignedNum)
	   {
	    case Usr_REGISTER_SPECIFIED_USRS_IN_CRS:
	       WhatToDo.RemoveUsrs = false;
	       WhatToDo.RemoveSpecifiedUsrs = false;	// Ignored
	       WhatToDo.EliminateUsrs = false;		// Ignored
	       WhatToDo.RegisterUsrs = true;
	       break;
	    case Usr_REMOVE_SPECIFIED_USRS_FROM_CRS:
	       WhatToDo.RemoveUsrs = true;
	       WhatToDo.RemoveSpecifiedUsrs = true;
	       WhatToDo.EliminateUsrs = false;
	       WhatToDo.RegisterUsrs = false;
	       break;
	    case Usr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS:
	       WhatToDo.RemoveUsrs = true;
	       WhatToDo.RemoveSpecifiedUsrs = false;
	       WhatToDo.EliminateUsrs = false;
	       WhatToDo.RegisterUsrs = false;
	       break;
	    case Usr_UPDATE_USRS_IN_CRS:
	       WhatToDo.RemoveUsrs = true;
	       WhatToDo.RemoveSpecifiedUsrs = false;
	       WhatToDo.EliminateUsrs = false;
	       WhatToDo.RegisterUsrs = true;
	       break;
	    case Usr_ELIMINATE_USRS_FROM_PLATFORM:
	       if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
		 {
		  WhatToDo.RemoveUsrs = true;
		  WhatToDo.RemoveSpecifiedUsrs = true;
		  WhatToDo.EliminateUsrs = true;
		  WhatToDo.RegisterUsrs = false;
		 }
	       else
		  Lay_ShowErrorAndExit ("You are not allowed to perform this action.");
	       break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong registering / removing specification.");
	       break;
	   }
      else
	 Lay_ShowErrorAndExit ("Wrong registering / removing specification.");
   else
     {
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_in_step_3_the_action_to_perform);

      /* Show form again */
      Usr_ShowFormRegRemSeveralUsrs ();
      ErrorInForm = true;
     }

   if (!ErrorInForm)
     {
      /***** Get external groups of students *****/
      Par_GetParMultiToText ("ImpGrpCod",ListExternalGrpCods,Imp_MAX_BYTES_LIST_EXTERNAL_GRP_CODS);

      /***** Get groups to which register/remove users *****/
      LstGrps.NumGrps = 0;
      if (Gbl.CurrentCrs.Grps.NumGrps) // This course has groups?
	{
	 /***** Get list of groups types and groups in current course *****/
	 Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

	 /***** Get the list of groups to which register/remove students *****/
	 Grp_GetLstCodsGrpWanted (&LstGrps);

	 /***** A student can't belong to more than one group when the type of group only allows to register in one group *****/
	 if (WhatToDo.RegisterUsrs &&
	     RegRemRole == Rol_ROLE_STUDENT &&
	     LstGrps.NumGrps >= 2)
	    /* Check if I have selected more than one group of single enrollment */
	    if (!Grp_CheckIfSelectionGrpsIsValid (&LstGrps))
	      {
	       /* Show warning message and exit */
	       Lay_ShowAlert (Lay_WARNING,Txt_In_a_type_of_group_with_single_enrollment_students_can_not_be_registered_in_more_than_one_group);

	       /* Free memory used by lists of groups and abort */
	       Grp_FreeListCodGrp (&LstGrps);
	       Grp_FreeListGrpTypesAndGrps ();
	       return;
	      }
	}

      /***** Get list of users' IDs *****/
      if ((ListUsrsIDs = (char *) malloc (ID_MAX_BYTES_LIST_USRS_IDS+1)) == NULL)
	 Lay_ShowErrorAndExit ("Not enough memory to store users' IDs.");
      Par_GetParToText ("UsrsIDs",ListUsrsIDs,ID_MAX_BYTES_LIST_USRS_IDS);

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Remove users *****/
      if (WhatToDo.RemoveUsrs)
	{
	 /***** Get list of users in current course *****/
	 switch (RegRemRole)
	   {
	    case Rol_ROLE_STUDENT:
	       Usr_GetUsrsLst (Rol_ROLE_STUDENT,Sco_SCOPE_COURSE,NULL,false);
	       LstCurrentUsrs = &Gbl.Usrs.LstStds;
	       break;
	    case Rol_ROLE_TEACHER:
	       Usr_GetUsrsLst (Rol_ROLE_TEACHER,Sco_SCOPE_COURSE,NULL,false);
	       LstCurrentUsrs = &Gbl.Usrs.LstTchs;
	       break;
	    default:
	       LstCurrentUsrs = NULL;        // To avoid warning
	       Lay_ShowErrorAndExit ("Wrong role.");        // If user manipulated the form
	       break;
	   }

	 if (LstCurrentUsrs->NumUsrs)
	   {
	    /***** Initialize list of users to remove *****/
	    for (NumCurrentUsr = 0;
		 NumCurrentUsr < LstCurrentUsrs->NumUsrs;
		 NumCurrentUsr++)
	       LstCurrentUsrs->Lst[NumCurrentUsr].Remove = !WhatToDo.RemoveSpecifiedUsrs;

	    /***** Loop 1: go through form list setting if a student must be removed *****/
	    /* Step a: Get students from a list of official groups */
	    if (RegRemRole == Rol_ROLE_STUDENT)
	      {
	       Ptr = ListExternalGrpCods;
	       while (*Ptr)
		 {
		  /* Find next external group code and course code in list */
		  Str_GetNextStringUntilSeparator (&Ptr,ExternalStr,1+10+1+Crs_LENGTH_INSTITUTIONAL_CRS_COD);
		  if (sscanf (ExternalStr,"%ld_%s",&LongNum,ExternalCrsCod) == 2)
		    {
		     GrpCod = LongNum;

		     /* Mark users belonging to this official group as removable */
		     Usr_MarkOfficialStdsAsRemovable (GrpCod,WhatToDo.RemoveSpecifiedUsrs);
		    }
		 }
	      }

	    /* Step b: Get users from a list of users' IDs */
	    Ptr = ListUsrsIDs;
	    while (*Ptr)
	      {
	       /* Reset user */
	       UsrDat.UsrCod = -1L;

	       /* Find next string in text */
	       Str_GetNextStringUntilSeparator (&Ptr,UsrDat.UsrIDNickOrEmail,Usr_MAX_BYTES_USR_LOGIN);

	       /* Reset default list of users' codes */
	       ListUsrCods.NumUsrs = 0;
	       ListUsrCods.Lst = NULL;

	       /* Check if string is a user's ID, user's nickname or user's e-mail address */
	       if (Nck_CheckIfNickWithArrobaIsValid (UsrDat.UsrIDNickOrEmail))	// 1: It's a nickname
		 {
		  if ((UsrDat.UsrCod = Nck_GetUsrCodFromNickname (UsrDat.UsrIDNickOrEmail)) > 0)
		    {
		     ListUsrCods.NumUsrs = 1;
		     Usr_AllocateListUsrCods (&ListUsrCods);
		     ListUsrCods.Lst[0] = UsrDat.UsrCod;
		    }
		 }
	       else if (Mai_CheckIfEmailIsValid (UsrDat.UsrIDNickOrEmail))	// 2: It's an e-mail
		 {
		  if ((UsrDat.UsrCod = Mai_GetUsrCodFromEmail (UsrDat.UsrIDNickOrEmail)) > 0)
		    {
		     ListUsrCods.NumUsrs = 1;
		     Usr_AllocateListUsrCods (&ListUsrCods);
		     ListUsrCods.Lst[0] = UsrDat.UsrCod;
		    }
		 }
	       else								// 3: It looks like a user's ID
		 {
		  // Users' IDs are always stored internally in capitals and without leading zeros
		  Str_RemoveLeadingZeros (UsrDat.UsrIDNickOrEmail);
		  Str_ConvertToUpperText (UsrDat.UsrIDNickOrEmail);
		  if (ID_CheckIfUsrIDSeemsAValidID (UsrDat.UsrIDNickOrEmail))
		    {
		     /***** Find users for this user's ID *****/
		     ID_ReallocateListIDs (&UsrDat,1);	// Only one user's ID
		     strcpy (UsrDat.IDs.List[0].ID,UsrDat.UsrIDNickOrEmail);
		     ID_GetListUsrCodsFromUsrID (&UsrDat,NULL,&ListUsrCods,false);
		    }
		 }

	       if (WhatToDo.RemoveSpecifiedUsrs)	// Remove the specified users (of the role)
		 {
	          if (ListUsrCods.NumUsrs == 1)		// If more than one user found ==> do not remove
		     for (NumCurrentUsr = 0;
			  NumCurrentUsr < LstCurrentUsrs->NumUsrs;
			  NumCurrentUsr++)
			if (LstCurrentUsrs->Lst[NumCurrentUsr].UsrCod == ListUsrCods.Lst[0])	// User found
			   LstCurrentUsrs->Lst[NumCurrentUsr].Remove = true;	// Mark as removable
		 }
	       else	// Remove all the users (of the role) except these specified
		 {
		  for (NumCurrentUsr = 0;
		       NumCurrentUsr < LstCurrentUsrs->NumUsrs;
		       NumCurrentUsr++)
		     for (NumUsrFound = 0;
			  NumUsrFound < ListUsrCods.NumUsrs;
			  NumUsrFound++)
			if (LstCurrentUsrs->Lst[NumCurrentUsr].UsrCod == ListUsrCods.Lst[NumUsrFound])	// User found
			   LstCurrentUsrs->Lst[NumCurrentUsr].Remove = false;	// Mark as not removable
		 }

	       /* Free memory used for list of users' codes found for this ID */
	       Usr_FreeListUsrCods (&ListUsrCods);
	      }

	    /***** Loop 2: go through users list removing users *****/
	    for (NumCurrentUsr = 0;
		 NumCurrentUsr < LstCurrentUsrs->NumUsrs;
		 NumCurrentUsr++)
	       if (LstCurrentUsrs->Lst[NumCurrentUsr].Remove)        // If this student must be removed
		 {
		  UsrDat.UsrCod = LstCurrentUsrs->Lst[NumCurrentUsr].UsrCod;
		  if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
		    {
		     if (WhatToDo.EliminateUsrs)                // Eliminate user completely from the platform
		       {
			Usr_DeleteUsrFromPlatform (&UsrDat,Cns_QUIET);                // Remove definitely the user from the platform
			NumUsrsEliminated++;
		       }
		     else
		       {
			if (Gbl.CurrentCrs.Grps.NumGrps)        // If there are groups in the course
			  {
			   if (LstGrps.NumGrps)        // If the teacher has selected groups
			     {
			      if (Grp_RemoveUsrFromGroups (&UsrDat,&LstGrps))                // Remove user from the selected groups, not from the whole course
				 NumUsrsRemoved++;
			     }
			   else        // The teacher has not selected groups
			     {
			      Usr_EffectivelyRemUsrFromCrs (&UsrDat,&Gbl.CurrentCrs.Crs,
			                                    Usr_DO_NOT_REMOVE_WORKS,Cns_QUIET);        // Remove user from the course
			      NumUsrsRemoved++;
			     }
			  }
			else        // No groups
			  {
			   Usr_EffectivelyRemUsrFromCrs (&UsrDat,&Gbl.CurrentCrs.Crs,
			                                 Usr_DO_NOT_REMOVE_WORKS,Cns_QUIET);        // Remove user from the course
			   NumUsrsRemoved++;
			  }
		       }
		    }
		 }
	   }

	 /***** Free memory for users list *****/
	 Usr_FreeUsrsList (LstCurrentUsrs);
	}

      /***** Register users *****/
      if (WhatToDo.RegisterUsrs)	// TODO: !!!!! NO CAMBIAR EL ROL DE LOS USUARIOS QUE YA ESTÉN EN LA ASIGNATURA SI HAY MÁS DE UN USUARIO ENCONTRADO PARA EL MISMO DNI !!!!!!
	{
	 /***** Step a: Get users from a list of official groups *****/
	 if (RegRemRole == Rol_ROLE_STUDENT)
	   {
	    Ptr = ListExternalGrpCods;
	    while (*Ptr)
	      {
	       /* Find next official group code in list */
	       Str_GetNextStringUntilSeparator (&Ptr,ExternalStr,1+10+1+Crs_LENGTH_INSTITUTIONAL_CRS_COD);
	       if (sscanf (ExternalStr,"%ld_%s",&LongNum,ExternalCrsCod) == 2)
		 {
		  GrpCod = LongNum;

		  /* Import official group of users */
		  Imp_ImportStdsFromAnImpGrp (GrpCod,&LstGrps,&NumUsrsRegistered);

		  /* Update the institutional course code to the external course code */
		  Crs_UpdateCurrentInstitutionalCrsCod (&Gbl.CurrentCrs.Crs,ExternalCrsCod);
		 }
	      }
	   }

	 /***** Step b: Get users from a list of users' IDs ******/
	 Ptr = ListUsrsIDs;
	 while (*Ptr)
	   {
	    /* Reset user */
	    UsrDat.UsrCod = -1L;
	    ItLooksLikeAUsrID = false;

	    /* Find next string in text */
	    Str_GetNextStringUntilSeparator (&Ptr,UsrDat.UsrIDNickOrEmail,Usr_MAX_BYTES_USR_LOGIN);

	    /* Reset default list of users' codes */
	    ListUsrCods.NumUsrs = 0;
	    ListUsrCods.Lst = NULL;

	    /* Check if the string is a user's ID, a user's nickname or a user's e-mail address */
	    if (Nck_CheckIfNickWithArrobaIsValid (UsrDat.UsrIDNickOrEmail))	// 1: It's a nickname
	      {
	       if ((UsrDat.UsrCod = Nck_GetUsrCodFromNickname (UsrDat.UsrIDNickOrEmail)) > 0)
		 {
		  ListUsrCods.NumUsrs = 1;
		  Usr_AllocateListUsrCods (&ListUsrCods);
		  ListUsrCods.Lst[0] = UsrDat.UsrCod;
		 }
	      }
	    else if (Mai_CheckIfEmailIsValid (UsrDat.UsrIDNickOrEmail))		// 2: It's an e-mail
	      {
	       if ((UsrDat.UsrCod = Mai_GetUsrCodFromEmail (UsrDat.UsrIDNickOrEmail)) > 0)
		 {
		  ListUsrCods.NumUsrs = 1;
		  Usr_AllocateListUsrCods (&ListUsrCods);
		  ListUsrCods.Lst[0] = UsrDat.UsrCod;
		 }
	      }
	    else								// 3: It looks like a user's ID
	      {
	       // Users' IDs are always stored internally in capitals and without leading zeros
	       Str_RemoveLeadingZeros (UsrDat.UsrIDNickOrEmail);
	       Str_ConvertToUpperText (UsrDat.UsrIDNickOrEmail);
	       if (ID_CheckIfUsrIDSeemsAValidID (UsrDat.UsrIDNickOrEmail))
		 {
		  ItLooksLikeAUsrID = true;

		  /* Find users for this user's ID */
		  ID_ReallocateListIDs (&UsrDat,1);	// Only one user's ID
		  strcpy (UsrDat.IDs.List[0].ID,UsrDat.UsrIDNickOrEmail);
		  ID_GetListUsrCodsFromUsrID (&UsrDat,NULL,&ListUsrCods,false);
		 }
	      }

	    /* Register user(s) */
	    if (ListUsrCods.NumUsrs)	// User(s) found
	       for (NumUsrFound = 0;
		    NumUsrFound < ListUsrCods.NumUsrs;
		    NumUsrFound++)
		 {
		  UsrDat.UsrCod = ListUsrCods.Lst[NumUsrFound];
		  Usr_RegisterUsr (&UsrDat,RegRemRole,&LstGrps,&NumUsrsRegistered);
		 }
	    else if (ItLooksLikeAUsrID)	// User not found. He/she is a new user. Register him/her using ID
	       Usr_RegisterUsr (&UsrDat,RegRemRole,&LstGrps,&NumUsrsRegistered);

	    /* Free memory used for list of users' codes found for this ID */
	    Usr_FreeListUsrCods (&ListUsrCods);
	   }
	}

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      if (NumUsrsEliminated)
	 /***** Move unused contents of messages to table of deleted contents of messages *****/
	 Msg_MoveUnusedMsgsContentToDeleted ();

      /***** Write messages with the number of users enrolled/removed *****/
      if (WhatToDo.RemoveUsrs)
	{
	 if (WhatToDo.EliminateUsrs)        // Eliminate completely from the platform
	    switch (NumUsrsEliminated)
	      {
	       case 0:
		  Lay_ShowAlert (Lay_INFO,Txt_No_user_has_been_eliminated);
		  break;
	       case 1:
		  Lay_ShowAlert (Lay_SUCCESS,Txt_One_user_has_been_eliminated);
		  break;
	       default:
		  sprintf (Gbl.Message,Txt_X_users_have_been_eliminated,
			   NumUsrsEliminated);
		  Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
		  break;
	      }
	 else                        // Only remove from course / groups
	    switch (NumUsrsRemoved)
	      {
	       case 0:
		  Lay_ShowAlert (Lay_INFO,Txt_No_user_has_been_removed);
		  break;
	       case 1:
		  Lay_ShowAlert (Lay_SUCCESS,Txt_One_user_has_been_removed);
		  break;
	       default:
		  sprintf (Gbl.Message,Txt_X_users_have_been_removed,
			   NumUsrsRemoved);
		  Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
		  break;
	      }
	}
      if (WhatToDo.RegisterUsrs)
	 switch (NumUsrsRegistered)
	   {
	    case 0:
	       Lay_ShowAlert (Lay_INFO,Txt_No_user_has_been_enrolled);
	       break;
	    case 1:
	       Lay_ShowAlert (Lay_SUCCESS,Txt_One_user_has_been_enrolled);
	       break;
	    default:
	       sprintf (Gbl.Message,Txt_X_users_have_been_enrolled_including_possible_repetitions,
			NumUsrsRegistered);
	       Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
	       break;
	   }

      /***** Free memory used by the list of user's IDs *****/
      free (ListUsrsIDs);

      /***** Free memory with the list of groups to/from which register/remove users *****/
      Grp_FreeListCodGrp (&LstGrps);

      /***** Free list of groups types and groups in current course *****/
      Grp_FreeListGrpTypesAndGrps ();
     }
  }

/*****************************************************************************/
/********************** Register a user using his/her ID *********************/
/*****************************************************************************/
// If user does not exists, UsrDat->IDs must hold the user's ID

static void Usr_RegisterUsr (struct UsrData *UsrDat,Rol_Role_t RegRemRole,
                             struct ListCodGrps *LstGrps,unsigned *NumUsrsRegistered)
  {
   /***** Check if I can register this user *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_TEACHER &&
       RegRemRole != Rol_ROLE_STUDENT)
      Lay_ShowErrorAndExit ("A teacher only can register new users if they are students.");

   /***** Check if the record of the user exists and get the type of user *****/
   if (UsrDat->UsrCod > 0)	// User exists in database
      Usr_GetAllUsrDataFromUsrCod (UsrDat);	// Get user's data
   else				// User does not exist in database, create it using his/her ID!
     {
      // Reset user's data
      Usr_ResetUsrDataExceptUsrCodAndIDs (UsrDat);	// It's necessary, because the same struct UsrDat was used for former user

      // User does not exist in database;list of IDs is initialized
      UsrDat->IDs.List[0].Confirmed = true;	// If he/she is a new user ==> his/her ID will be stored as confirmed in database
      Usr_CreateNewUsr (UsrDat);
     }

   /***** Register user in current course in database *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
     {
      if (Usr_CheckIfUsrBelongsToCrs (UsrDat->UsrCod,Gbl.CurrentCrs.Crs.CrsCod))      // User does belong to current course, modify his/her role
	 Usr_ModifyRoleInCurrentCrs (UsrDat,RegRemRole,
	                             Cns_QUIET,Usr_SET_ACCEPTED_TO_FALSE);
      else
	 Usr_RegisterUsrInCurrentCrs (UsrDat,RegRemRole,
	                              Cns_QUIET,Usr_SET_ACCEPTED_TO_FALSE);

      /***** Register user in the selected groups *****/
      if (Gbl.CurrentCrs.Grps.NumGrps)	// If there are groups in the course
	 Grp_RegisterUsrIntoGroups (UsrDat,LstGrps);
     }

   (*NumUsrsRegistered)++;
  }

/*****************************************************************************/
/****** Mark the students belonging to an official group as removable ********/
/*****************************************************************************/

static void Usr_MarkOfficialStdsAsRemovable (long ImpGrpCod,bool RemoveSpecifiedUsrs)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumStds;
   unsigned NumStd;
   unsigned NumUsr;
   struct ListUsrCods ListUsrCods;	// List with users' codes for a given user's ID
   unsigned NumUsrFound;

   /***** Get imported students belonging to this group from database *****/
   sprintf (Query,"SELECT UsrID FROM imported_students WHERE GrpCod='%ld'",
            ImpGrpCod);
   NumStds = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get imported students");

   /***** Mark the students from this official group as removable *****/
   for (NumStd = 0;
        NumStd < NumStds;
        NumStd++)        // For each student inside the official group...
     {
      /* Get user's ID of the student from official list */
      row = mysql_fetch_row (mysql_res);

      /***** Allocate space for the list *****/
      ID_ReallocateListIDs (&Gbl.Usrs.Other.UsrDat,1);

      strncpy (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID,row[0],ID_MAX_LENGTH_USR_ID);
      Gbl.Usrs.Other.UsrDat.IDs.List[0].ID[ID_MAX_LENGTH_USR_ID] = '\0';
      // Users' IDs are always stored internally in capitals and without leading zeros
      Str_RemoveLeadingZeros (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);
      Str_ConvertToUpperText (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);

      /* Check if this official user's ID is in the list of students */
      if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID))
         if (ID_GetListUsrCodsFromUsrID (&Gbl.Usrs.Other.UsrDat,NULL,&ListUsrCods,false))	// User(s) found
           {
	    for (NumUsr = 0;
		 NumUsr < Gbl.Usrs.LstStds.NumUsrs;
		 NumUsr++)
	       for (NumUsrFound = 0;
		    NumUsrFound < ListUsrCods.NumUsrs;
		    NumUsrFound++)
		  if (Gbl.Usrs.LstStds.Lst[NumUsr].UsrCod == ListUsrCods.Lst[NumUsrFound])	// User found
		     Gbl.Usrs.LstStds.Lst[NumUsr].Remove = RemoveSpecifiedUsrs;

	    /* Free memory used for list of users' codes found for this ID */
	    Usr_FreeListUsrCods (&ListUsrCods);
           }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**** Put a link (form) to remove all the students in the current course *****/
/*****************************************************************************/

static void Usr_PutFormToRemAllStdsThisCrs (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Remove_all_students;

   /***** Put form to remove all the students in the current course *****/
   Act_FormStart (ActReqRemAllStdCrs);
   Act_LinkFormSubmit (Txt_Remove_all_students,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("delon",Txt_Remove_all_students,Txt_Remove_all_students);
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/********** Ask for removing all the students from current course ************/
/*****************************************************************************/

void Usr_AskRemAllStdsThisCrs (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_X_students_from_the_course_Y_;
   extern const char *Txt_Remove_students;
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   unsigned NumStds;

   if ((NumStds = Usr_GetNumUsrsInCrs (Rol_ROLE_STUDENT,Gbl.CurrentCrs.Crs.CrsCod)))
     {
      /***** Write message to confirm the removing *****/
      sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_X_students_from_the_course_Y_,
               NumStds,Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);

      /***** Form to remove all the students in current course *****/
      Act_FormStart (ActRemAllStdCrs);
      Grp_PutParamAllGroups ();
      Pwd_AskForConfirmationOnDangerousAction ();
      Lay_PutSendButton (Txt_Remove_students);
      fprintf (Gbl.F.Out,"</form>");
     }
   else
     {
      sprintf (Gbl.Message,Txt_No_users_found[Rol_ROLE_STUDENT],
               Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
  }

/*****************************************************************************/
/***************** Remove all the students from current course ***************/
/*****************************************************************************/

void Usr_RemAllStdsThisCrs (void)
  {
   extern const char *Txt_The_X_students_who_belonged_to_the_course_Y_have_been_removed_from_it;
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   unsigned NumStdsInCrs;

   if (Pwd_GetConfirmationOnDangerousAction ())
     {
      if ((NumStdsInCrs = Usr_RemAllStdsInCrs (&Gbl.CurrentCrs.Crs)))
	{
	 sprintf (Gbl.Message,Txt_The_X_students_who_belonged_to_the_course_Y_have_been_removed_from_it,
		  NumStdsInCrs,Gbl.CurrentCrs.Crs.FullName);
	 Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
	}
      else
	{
	 sprintf (Gbl.Message,Txt_No_users_found[Rol_ROLE_STUDENT],
		  Gbl.CurrentCrs.Crs.FullName);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	}
     }
  }

/*****************************************************************************/
/***************** Remove all the students from a given course ***************/
/*****************************************************************************/
// Returns the numbers of students in the course before deletion

unsigned Usr_RemAllStdsInCrs (struct Course *Crs)
  {
   unsigned NumStdsInCrs;
   unsigned NumUsr;

   /***** Get list of students in current course *****/
   Gbl.Usrs.ClassPhoto.AllGroups = true;        // Get all the students of the current course
   Usr_GetUsrsLst (Rol_ROLE_STUDENT,Sco_SCOPE_COURSE,NULL,false);
   NumStdsInCrs = Gbl.Usrs.LstStds.NumUsrs;

   /***** Remove all the students *****/
   for (NumUsr = 0;
	NumUsr < NumStdsInCrs;
	NumUsr++)
     {
      Gbl.Usrs.Other.UsrDat.UsrCod = Gbl.Usrs.LstStds.Lst[NumUsr].UsrCod;
      Usr_EffectivelyRemUsrFromCrs (&Gbl.Usrs.Other.UsrDat,Crs,
				    Usr_REMOVE_WORKS,Cns_QUIET);
     }

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstStds);

   return NumStdsInCrs;
  }

/*****************************************************************************/
/************* Form to request sign up in the current course *****************/
/*****************************************************************************/

void Usr_ReqSignUpInCrs (void)
  {
   extern const char *Txt_You_were_already_enrolled_as_X_in_the_course_Y;
   extern const char *Txt_ROLES_SINGULAR_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   /***** Check if I already belong to course *****/
   if (Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB >= Rol_ROLE_STUDENT)
     {
      sprintf (Gbl.Message,Txt_You_were_already_enrolled_as_X_in_the_course_Y,
               Txt_ROLES_SINGULAR_abc[Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB][Gbl.Usrs.Me.UsrDat.Sex],
               Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
   else if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_GUEST ||
	    Gbl.Usrs.Me.LoggedRole == Rol_ROLE_VISITOR)
      /***** Show form to modify only the user's role or the user's data *****/
      Rec_ShowFormSignUpWithMyCommonRecord ();
   else
      Lay_ShowErrorAndExit ("You must be logged to sign up in a course.");        // This never should happen
  }

/*****************************************************************************/
/*********************** Sign up in the current course ***********************/
/*****************************************************************************/

void Usr_SignUpInCrs (void)
  {
   extern const char *Txt_You_were_already_enrolled_as_X_in_the_course_Y;
   extern const char *Txt_ROLES_SINGULAR_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Your_request_for_enrollment_as_X_in_the_course_Y_has_been_accepted_for_processing;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char UnsignedStr[10+1];
   Rol_Role_t RoleFromForm;
   long ReqCod = -1L;
   unsigned NumUsrsToBeNotifiedByEMail;

   /***** Check if I already belong to course *****/
   if (Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB >= Rol_ROLE_STUDENT)
     {
      sprintf (Gbl.Message,Txt_You_were_already_enrolled_as_X_in_the_course_Y,
               Txt_ROLES_SINGULAR_abc[Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB][Gbl.Usrs.Me.UsrDat.Sex],
               Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
   else
     {
      /***** Get new role from record form *****/
      Par_GetParToText ("Role",UnsignedStr,10);
      RoleFromForm = Usr_ConvertUnsignedStrToRole (UnsignedStr);

      /* Check if role is correct */
      if (!(RoleFromForm == Rol_ROLE_STUDENT ||
            RoleFromForm == Rol_ROLE_TEACHER))
         Lay_ShowErrorAndExit ("Wrong role.");

      /***** Try to get and old request of the same user in the same course from database *****/
      sprintf (Query,"SELECT ReqCod FROM crs_usr_requests"
                     " WHERE CrsCod='%ld' AND UsrCod='%ld'",
               Gbl.CurrentCrs.Crs.CrsCod,
               Gbl.Usrs.Me.UsrDat.UsrCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get enrollment request"))
        {
         row = mysql_fetch_row (mysql_res);
         /* Get request code (row[0]) */
         ReqCod = Str_ConvertStrCodToLongCod (row[0]);
        }
      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

      /***** Request user in current course in database *****/
      if (ReqCod > 0)        // Old request exists in database
        {
         sprintf (Query,"UPDATE crs_usr_requests SET Role='%u',RequestTime=NOW()"
                        " WHERE ReqCod='%ld' AND CrsCod='%ld' AND UsrCod='%ld'",
                  (unsigned) RoleFromForm,
                  ReqCod,
                  Gbl.CurrentCrs.Crs.CrsCod,
                  Gbl.Usrs.Me.UsrDat.UsrCod);
         DB_QueryUPDATE (Query,"can not update enrollment request");
        }
      else                // No request in database for this user in this course
        {
         sprintf (Query,"INSERT INTO crs_usr_requests (CrsCod,UsrCod,Role,RequestTime)"
                        " VALUES ('%ld','%ld','%u',NOW())",
                  Gbl.CurrentCrs.Crs.CrsCod,
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  (unsigned) RoleFromForm);
         ReqCod = DB_QueryINSERTandReturnCode (Query,"can not save enrollment request");
        }

      /***** Show confirmation message *****/
      sprintf (Gbl.Message,Txt_Your_request_for_enrollment_as_X_in_the_course_Y_has_been_accepted_for_processing,
               Txt_ROLES_SINGULAR_abc[RoleFromForm][Gbl.Usrs.Me.UsrDat.Sex],
               Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

      /***** Notify teachers or admins by e-mail about the new enrollment request *****/
      // If this course has teachers ==> send notification to teachers
      // If this course has no teachers and I want to be a teacher ==> send notification to administrators or superusers
      if (Gbl.CurrentCrs.Crs.NumTchs || RoleFromForm == Rol_ROLE_TEACHER)
	{
         NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_ENROLLMENT_REQUEST,ReqCod);
         Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);
	}
     }
  }

/*****************************************************************************/
/************** Put an enrollment request into a notification *****************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void Usr_GetNotifEnrollmentRequest (char *SummaryStr,char **ContentStr,
                                    long ReqCod,unsigned MaxChars,bool GetContent)
  {
   extern const char *Txt_ROLES_SINGULAR_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct UsrData UsrDat;
   Rol_Role_t DesiredRole;

   SummaryStr[0] = '\0';        // Return nothing on error

   /***** Get user and requested role from database *****/
   sprintf (Query,"SELECT UsrCod,Role,"
                  "DATE_FORMAT(RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                  " FROM crs_usr_requests"
                  " WHERE ReqCod='%ld'",
            ReqCod);

   if (!mysql_query (&Gbl.mysql,Query))
      if ((mysql_res = mysql_store_result (&Gbl.mysql)) != NULL)
        {
         /***** Result should have a unique row *****/
         if (mysql_num_rows (mysql_res) == 1)
           {
            /***** Get user and requested role *****/
            row = mysql_fetch_row (mysql_res);

            /* Initialize structure with user's data */
            Usr_UsrDataConstructor (&UsrDat);

            /* User's code (row[0]) */
            UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
            Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);

            /* Role (row[1]) */
            DesiredRole = Usr_ConvertUnsignedStrToRole (row[1]);
            strcpy (SummaryStr,Txt_ROLES_SINGULAR_Abc[DesiredRole][UsrDat.Sex]);
            if (MaxChars)
               Str_LimitLengthHTMLStr (SummaryStr,MaxChars);

            if (GetContent)
               if ((*ContentStr = (char *) malloc (16+1)))
                  /* Write date (row[2]) into content */
                  sprintf (*ContentStr,"%c%c/%c%c/%c%c%c%c %c%c:%c%c",
                           row[2][ 6],row[2][ 7],
                           row[2][ 4],row[2][ 5],
                           row[2][ 0],row[2][ 1],row[2][ 2],row[2][ 3],
                           row[2][ 8],row[2][ 9],
                           row[2][10],row[2][11]);

            /* Free memory used for user's data */
            Usr_UsrDataDestructor (&UsrDat);
           }

         mysql_free_result (mysql_res);
        }
  }

/*****************************************************************************/
/****** Ask if reject the request for enrollment of a user in a course *******/
/*****************************************************************************/

void Usr_AskIfRejectSignUp (void)
  {
   extern const char *Txt_THE_USER_X_is_already_enrolled_in_the_course_Y;
   extern const char *Txt_Do_you_really_want_to_reject_the_enrollment_request_;
   extern const char *Txt_ROLES_SINGULAR_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Reject;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   Rol_Role_t Role;

   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))        // If user's data exist...
     {
      if (Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod))
        {
         /* User already belongs to this course */
         sprintf (Gbl.Message,Txt_THE_USER_X_is_already_enrolled_in_the_course_Y,
                  Gbl.Usrs.Other.UsrDat.FullName,Gbl.CurrentCrs.Crs.FullName);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
         Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

         /* Remove inscription request because it has not sense */
         Usr_RemoveEnrollmentRequest (Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Other.UsrDat.UsrCod);
        }
      else        // User does not belong to this course
        {
         Role = Usr_GetRequestedRole (Gbl.Usrs.Other.UsrDat.UsrCod);
         if (Role == Rol_ROLE_STUDENT ||
             Role == Rol_ROLE_TEACHER)
           {
            /* Ask if reject */
            sprintf (Gbl.Message,Txt_Do_you_really_want_to_reject_the_enrollment_request_,
                     Gbl.Usrs.Other.UsrDat.FullName,
                     Txt_ROLES_SINGULAR_abc[Role][Gbl.Usrs.Other.UsrDat.Sex],
                     Gbl.CurrentCrs.Crs.FullName);
            Lay_ShowAlert (Lay_INFO,Gbl.Message);
            Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

            /* Button to confirm rejection */
            Act_FormStart (ActRejSignUp);
            Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);

            Lay_PutSendButton (Txt_Reject);
            fprintf (Gbl.F.Out,"</form>");
           }
         else
            Lay_ShowErrorAndExit ("Wrong role.");
        }
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/********* Reject the request for enrollment of a user in a course ***********/
/*****************************************************************************/

void Usr_RejectSignUp (void)
  {
   extern const char *Txt_THE_USER_X_is_already_enrolled_in_the_course_Y;
   extern const char *Txt_Enrollment_of_X_rejected;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))        // If user's data exist...
     {
      if (Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod))
        {
         /* User already belongs to this course */
         sprintf (Gbl.Message,Txt_THE_USER_X_is_already_enrolled_in_the_course_Y,
                  Gbl.Usrs.Other.UsrDat.FullName,Gbl.CurrentCrs.Crs.FullName);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
         Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
        }

      /* Remove inscription request */
      Usr_RemoveEnrollmentRequest (Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Other.UsrDat.UsrCod);

      /* Confirmation message */
      sprintf (Gbl.Message,Txt_Enrollment_of_X_rejected,
               Gbl.Usrs.Other.UsrDat.FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);

   /* Show again the rest of registrarion requests */
   Usr_ShowEnrollmentRequests ();
  }

/*****************************************************************************/
/******** Show pending requests for enrollment in the current course *********/
/*****************************************************************************/

void Usr_ShowEnrollmentRequests (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Scope;
   extern const char *Txt_Users;
   extern const char *Txt_Update;
   extern const char *Txt_Course;
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Requester;
   extern const char *Txt_Role;
   extern const char *Txt_Date;
   extern const char *Txt_Go_to_X;
   extern const char *Txt_ROLES_SINGULAR_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Register;
   extern const char *Txt_Reject;
   extern const char *Txt_No_enrollment_requests;
   unsigned Roles;
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumReq;
   unsigned NumRequests;
   long ReqCod;
   struct Degree Deg;
   struct Course Crs;
   struct UsrData UsrDat;
   bool UsrExists;
   bool UsrBelongsToCrs;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX+1];
   Rol_Role_t DesiredRole;

   /***** Selection of scope and roles *****/
   /* Start form */
   Act_FormStart (ActSeeSignUpReq);
   fprintf (Gbl.F.Out,"<div align=\"center\">"
                      "<table cellspacing=\"4\" cellpadding=\"0\">");

   /* Scope (whole platform, current centre, current degree or current course) */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" valign=\"middle\" class=\"%s\">%s:</td>"
                      "<td align=\"left\" valign=\"middle\">",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Scope);
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_PLATFORM    |
	               1 << Sco_SCOPE_COUNTRY     |
                       1 << Sco_SCOPE_INSTITUTION |
                       1 << Sco_SCOPE_CENTRE      |
                       1 << Sco_SCOPE_DEGREE      |
                       1 << Sco_SCOPE_COURSE;
   Gbl.Scope.Default = Sco_SCOPE_COURSE;
   Sco_GetScope ();
   Sco_PutSelectorScope (false);
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");

   /* Users' roles in listing */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" valign=\"top\" class=\"%s\">%s:</td>"
                      "<td align=\"left\" valign=\"middle\" class=\"DAT\">",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Users);
   Usr_GetSelectedRoles (&Roles);
   if (!Roles)
      /* Set default roles */
      switch (Gbl.Usrs.Me.LoggedRole)
        {
         case Rol_ROLE_TEACHER:
            Roles = (1 << Rol_ROLE_STUDENT) | (1 << Rol_ROLE_TEACHER);
            break;
         case Rol_ROLE_DEG_ADMIN:
         case Rol_ROLE_SUPERUSER:
            Roles = (1 << Rol_ROLE_TEACHER);
            break;
         default:
            Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
            break;
        }
   Usr_WriteSelectorRoles (Roles);
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");
   /* Form end */
   fprintf (Gbl.F.Out,"</table>"
                      "</div>");
   Lay_PutSendButton (Txt_Update);
   fprintf (Gbl.F.Out,"</form>");

   /***** Build query *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_PLATFORM:                // Show requesters for the whole platform
         switch (Gbl.Usrs.Me.LoggedRole)
           {
            case Rol_ROLE_TEACHER:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM crs_usr,crs_usr_requests"
                              " WHERE crs_usr.UsrCod='%ld'"
                              " AND crs_usr.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.Usrs.Me.UsrDat.UsrCod,
                        Roles);
               break;
            case Rol_ROLE_DEG_ADMIN:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM deg_admin,courses,crs_usr_requests"
                              " WHERE deg_admin.UsrCod='%ld'"
                              " AND deg_admin.DegCod=courses.DegCod"
                              " AND courses.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.Usrs.Me.UsrDat.UsrCod,
                        Roles);
               break;
           case Rol_ROLE_SUPERUSER:
               sprintf (Query,"SELECT ReqCod,CrsCod,UsrCod,Role,DATE_FORMAT(RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM crs_usr_requests WHERE ((1<<Role)&%u)<>0"
                              " ORDER BY RequestTime DESC",
                        Roles);
               break;
            default:
               Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
               break;
           }
         break;
      case Sco_SCOPE_INSTITUTION:                // Show requesters for the current institution
         switch (Gbl.Usrs.Me.LoggedRole)
           {
            case Rol_ROLE_TEACHER:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM crs_usr,centres,degrees,courses,crs_usr_requests"
                              " WHERE crs_usr.UsrCod='%ld'"
                              " AND centres.InsCod='%ld'"
                              " AND crs_usr.CrsCod=courses.CrsCod"
                              " AND degrees.DegCod=courses.DegCod"
                              " AND degrees.CtrCod=centres.CtrCod"
                              " AND courses.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.Usrs.Me.UsrDat.UsrCod,
                        Gbl.CurrentIns.Ins.InsCod,
                        Roles);
               break;
            case Rol_ROLE_DEG_ADMIN:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM deg_admin,centres,degrees,courses,crs_usr_requests"
                              " WHERE deg_admin.UsrCod='%ld'"
                              " AND centres.InsCod='%ld'"
                              " AND deg_admin.DegCod=degrees.DegCod"
                              " AND deg_admin.DegCod=courses.DegCod"
                              " AND degrees.CtrCod=centres.CtrCod"
                              " AND courses.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.Usrs.Me.UsrDat.UsrCod,
                        Gbl.CurrentIns.Ins.InsCod,
                        Roles);
               break;
           case Rol_ROLE_SUPERUSER:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM centres,degrees,courses,crs_usr_requests"
                              " WHERE centres.InsCod='%ld'"
                              " AND centres.CtrCod=degrees.CtrCod"
                              " AND degrees.DegCod=courses.DegCod"
                              " AND courses.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.CurrentIns.Ins.InsCod,
                        Roles);
               break;
            default:
               Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
               break;
           }
         break;
      case Sco_SCOPE_CENTRE:                // Show requesters for the current centre
         switch (Gbl.Usrs.Me.LoggedRole)
           {
            case Rol_ROLE_TEACHER:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM crs_usr,degrees,courses,crs_usr_requests"
                              " WHERE crs_usr.UsrCod='%ld'"
                              " AND degrees.CtrCod='%ld'"
                              " AND crs_usr.CrsCod=courses.CrsCod"
                              " AND degrees.DegCod=courses.DegCod"
                              " AND courses.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.Usrs.Me.UsrDat.UsrCod,
                        Gbl.CurrentCtr.Ctr.CtrCod,
                        Roles);
               break;
            case Rol_ROLE_DEG_ADMIN:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM deg_admin,degrees,courses,crs_usr_requests"
                              " WHERE deg_admin.UsrCod='%ld'"
                              " AND degrees.CtrCod='%ld'"
                              " AND deg_admin.DegCod=degrees.DegCod"
                              " AND deg_admin.DegCod=courses.DegCod"
                              " AND courses.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.Usrs.Me.UsrDat.UsrCod,
                        Gbl.CurrentCtr.Ctr.CtrCod,
                        Roles);
               break;
           case Rol_ROLE_SUPERUSER:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM degrees,courses,crs_usr_requests"
                              " WHERE degrees.CtrCod='%ld'"
                              " AND degrees.DegCod=courses.DegCod"
                              " AND courses.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.CurrentCtr.Ctr.CtrCod,
                        Roles);
               break;
            default:
               Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
               break;
           }
         break;
      case Sco_SCOPE_DEGREE:        // Show requesters for the current degree
         switch (Gbl.Usrs.Me.LoggedRole)
           {
            case Rol_ROLE_TEACHER:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM crs_usr,courses,crs_usr_requests"
                              " WHERE crs_usr.UsrCod='%ld'"
                              " AND crs_usr.CrsCod=courses.CrsCod"
                              " AND courses.DegCod='%ld'"
                              " AND courses.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.Usrs.Me.UsrDat.UsrCod,
                        Gbl.CurrentDeg.Deg.DegCod,
                        Roles);
               break;
            case Rol_ROLE_DEG_ADMIN:
            case Rol_ROLE_SUPERUSER:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM courses,crs_usr_requests"
                              " WHERE courses.DegCod='%ld'"
                              " AND courses.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.CurrentDeg.Deg.DegCod,
                        Roles);
               break;
            default:
               Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
               break;
           }
         break;
      case Sco_SCOPE_COURSE:        // Show requesters for the current course
         switch (Gbl.Usrs.Me.LoggedRole)
           {
            case Rol_ROLE_TEACHER:
            case Rol_ROLE_DEG_ADMIN:
            case Rol_ROLE_SUPERUSER:
               sprintf (Query,"SELECT ReqCod,CrsCod,UsrCod,Role,DATE_FORMAT(RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM crs_usr_requests"
                              " WHERE CrsCod='%ld'"
                              " AND ((1<<Role)&%u)<>0"
                              " ORDER BY RequestTime DESC",
                        Gbl.CurrentCrs.Crs.CrsCod,Roles);
               break;
            default:
               Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
               break;
           }
         break;
      default:
         Lay_ShowErrorAndExit ("Wrong scope.");
         break;
     }

   NumRequests = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get requests for enrollment");

   /***** List requests *****/
   if (NumRequests)
      {
      /* Initialize structure with user's data */
      Usr_UsrDataConstructor (&UsrDat);

      /* Start table */
      Lay_StartRoundFrameTable10 (NULL,2,NULL);
      fprintf (Gbl.F.Out,"<tr>"
                         "<th align=\"right\" valign=\"top\" class=\"TIT_TBL\"></th>"
                         "<th align=\"left\" valign=\"top\" class=\"TIT_TBL\">%s</th>"
                         "<th align=\"right\" valign=\"top\" class=\"TIT_TBL\">%s</th>"
                         "<th align=\"left\" valign=\"top\" colspan=\"2\" class=\"TIT_TBL\">%s</th>"
                         "<th align=\"left\" valign=\"top\" class=\"TIT_TBL\">%s</th>"
                         "<th align=\"center\" valign=\"top\" class=\"TIT_TBL\">%s</th>"
                         "<th align=\"center\" valign=\"top\" class=\"TIT_TBL\"></th>"
                         "<th align=\"center\" valign=\"top\" class=\"TIT_TBL\"></th>"
                         "</tr>",
               Txt_Course,
               Txt_Teachers_ABBREVIATION,
               Txt_Requester,
               Txt_Role,
               Txt_Date);

      /* List requests */
      for (NumReq = 0;
           NumReq < NumRequests;
           NumReq++)
        {
         row = mysql_fetch_row (mysql_res);

         /* Get request code (row[0]) */
         ReqCod = Str_ConvertStrCodToLongCod (row[0]);

         /* Get course code (row[1]) */
         Crs.CrsCod = Str_ConvertStrCodToLongCod (row[1]);

         /* Get user code (row[2]) */
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[2]);

         UsrExists = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);

         if (UsrExists)
            UsrBelongsToCrs = Usr_CheckIfUsrBelongsToCrs (UsrDat.UsrCod,Crs.CrsCod);
         else
            UsrBelongsToCrs = false;

         if (UsrExists & !UsrBelongsToCrs)
           {
            /***** Number *****/
            fprintf (Gbl.F.Out,"<tr>"
                               "<td align=\"right\" valign=\"top\" class=\"DAT\">%u</td>",
                     NumRequests - NumReq);

            /***** Link to course *****/
            Crs_GetDataOfCourseByCod (&Crs);
            Deg.DegCod = Crs.DegCod;
            Deg_GetDataOfDegreeByCod (&Deg);
            fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\" class=\"DAT\">");
            Act_FormGoToStart (ActSeeCrsInf);
            Crs_PutParamCrsCod (Crs.CrsCod);
            sprintf (Gbl.Title,Txt_Go_to_X,Crs.FullName);
            Act_LinkFormSubmit (Gbl.Title,"DAT");
            fprintf (Gbl.F.Out,"%s &gt; %s</a>"
        	               "</form>"
        	               "</td>",
                     Deg.ShortName,Crs.ShortName);

            /***** Number of teachers in the course *****/
            fprintf (Gbl.F.Out,"<td align=\"right\" valign=\"top\" class=\"DAT\">"
                               "%u"
                               "</td>",
                     Crs.NumTchs);

            /***** User photo *****/
            fprintf (Gbl.F.Out,"<td width=\"18\" align=\"center\" valign=\"top\" class=\"DAT\">");
            ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
            Pho_ShowUsrPhoto (&UsrDat,
                              ShowPhoto ? PhotoURL :
                        	          NULL,
                              18,24,true);
            fprintf (Gbl.F.Out,"</td>");

            /***** User name *****/
            fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\" class=\"DAT\">");
            Usr_RestrictLengthAndWriteName (&UsrDat,20);

            /***** Requested role (row[3]) *****/
            DesiredRole = Usr_ConvertUnsignedStrToRole (row[3]);
            fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\" class=\"DAT\">%s</td>",
                     Txt_ROLES_SINGULAR_abc[DesiredRole][UsrDat.Sex]);

            /***** Request time (row[4]) *****/
            Msg_WriteMsgDate (row[4],"DAT");

            /***** Button to confirm the request *****/
            fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\" class=\"DAT\">");
            Act_FormStart (ActReqMdfUsr);
            Crs_PutParamCrsCod (Crs.CrsCod);
            Usr_PutParamOtherUsrCodEncrypted (UsrDat.EncryptedUsrCod);
            Lay_PutSendButton (Txt_Register);
            fprintf (Gbl.F.Out,"</form></td>");

            /***** Button to reject the request *****/
            fprintf (Gbl.F.Out,"<td align=\"left\" valign=\"top\" class=\"DAT\">");
            Act_FormStart (ActReqRejSignUp);
            Crs_PutParamCrsCod (Crs.CrsCod);
            Usr_PutParamOtherUsrCodEncrypted (UsrDat.EncryptedUsrCod);
            Lay_PutSendButton (Txt_Reject);
            fprintf (Gbl.F.Out,"</form></td>"
                               "</tr>");

            /***** Mark possible notification as seen *****/
            Ntf_SetNotifAsSeen (Ntf_EVENT_ENROLLMENT_REQUEST,
                                ReqCod,
                                Gbl.Usrs.Me.UsrDat.UsrCod);
           }
         else        // User does not exists or user already belongs to course ==> remove pair from crs_usr_requests table
            Usr_RemoveEnrollmentRequest (Crs.CrsCod,UsrDat.UsrCod);
        }

      /* End of table */
      Lay_EndRoundFrameTable10 ();

      /* Free memory used for user's data */
      Usr_UsrDataDestructor (&UsrDat);
     }
   else        // There are no requests
      Lay_ShowAlert (Lay_INFO,Txt_No_enrollment_requests);
  }

/*****************************************************************************/
/********************* Write selector of users' roles ************************/
/*****************************************************************************/

void Usr_WriteSelectorRoles (unsigned Roles)
  {
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   Rol_Role_t Role;

   for (Role = Rol_ROLE_STUDENT;
        Role <= Rol_ROLE_TEACHER;
        Role++)
     {
      fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"Roles\" value=\"%u\"",
               (unsigned) Role);
      if ((Roles & (1 << Role)))
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," />%s<br />",
               Txt_ROLES_PLURAL_abc[Role][Usr_SEX_UNKNOWN]);
     }
  }

/*****************************************************************************/
/************************* Get selected users' roles *************************/
/*****************************************************************************/

void Usr_GetSelectedRoles (unsigned *Roles)
  {
   char StrRoles[(10+1)*2];
   const char *Ptr;
   char UnsignedStr[10+1];
   Rol_Role_t Role;

   Par_GetParMultiToText ("Roles",StrRoles,(10+1)*2);
   *Roles = 0;
   for (Ptr = StrRoles;
        *Ptr;)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,10);
      if (sscanf (UnsignedStr,"%u",&Role) != 1)
         Lay_ShowErrorAndExit ("can not get user's role");
      *Roles |= (1 << Role);
     }
  }

/*****************************************************************************/
/******************** Remove a request for inscription ***********************/
/*****************************************************************************/

static void Usr_RemoveEnrollmentRequest (long CrsCod,long UsrCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long ReqCod;

   /***** Mark possible notifications as removed
          Important: do this before removing the request *****/
   /* Request request code (returns 0 or 1 rows) */
   sprintf (Query,"SELECT ReqCod FROM crs_usr_requests"
                  " WHERE CrsCod='%ld' AND UsrCod='%ld'",
            CrsCod,UsrCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get request code"))        // Request exists
     {
      /* Get request code */
      row = mysql_fetch_row (mysql_res);
      ReqCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Mark possible notifications as removed */
      Ntf_SetNotifAsRemoved (Ntf_EVENT_ENROLLMENT_REQUEST,ReqCod);
     }
   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Remove enrollment request *****/
   sprintf (Query,"DELETE FROM crs_usr_requests"
                  " WHERE CrsCod='%ld' AND UsrCod='%ld'",
            CrsCod,UsrCod);
   DB_QueryDELETE (Query,"can not remove a request for enrollment");
  }

/*****************************************************************************/
/************ Get requested role of a user in current course *****************/
/*****************************************************************************/

Rol_Role_t Usr_GetRequestedRole (long UsrCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Rol_Role_t Role = Rol_ROLE_UNKNOWN;

   /***** Get requested role from database *****/
   sprintf (Query,"SELECT Role FROM crs_usr_requests"
                  " WHERE CrsCod='%ld' AND UsrCod='%ld'",
            Gbl.CurrentCrs.Crs.CrsCod,UsrCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get requested role"))
     {
      /***** Get role *****/
      row = mysql_fetch_row (mysql_res);
      Role = Usr_ConvertUnsignedStrToRole (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Role;
  }

/*****************************************************************************/
/************** Form to request the user's ID of another user ****************/
/*****************************************************************************/

void Usr_ReqRegRemUsr (void)
  {
   extern const char *Txt_You_dont_have_permission_to_perform_this_action;

   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_GUEST:
      case Rol_ROLE_VISITOR:
      case Rol_ROLE_STUDENT:
	 Usr_AskIfRegRemMe ();
	 break;
      case Rol_ROLE_TEACHER:
	 if (Gbl.CurrentCrs.Crs.CrsCod > 0)
	    Usr_ReqAnotherUsrIDToRegisterRemove ();
	 else
	    Usr_AskIfRegRemMe ();
	 break;
      case Rol_ROLE_DEG_ADMIN:
	 if (Gbl.CurrentDeg.Deg.DegCod > 0)
	    Usr_ReqAnotherUsrIDToRegisterRemove ();
	 else
	    Usr_AskIfRegRemMe ();
	 break;
      case Rol_ROLE_CTR_ADMIN:
	 if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
	    Usr_ReqAnotherUsrIDToRegisterRemove ();
	 else
	    Usr_AskIfRegRemMe ();
	 break;
      case Rol_ROLE_INS_ADMIN:
	 if (Gbl.CurrentIns.Ins.InsCod > 0)
	    Usr_ReqAnotherUsrIDToRegisterRemove ();
	 else
	    Usr_AskIfRegRemMe ();
	 break;
      case Rol_ROLE_SUPERUSER:
	 Usr_ReqAnotherUsrIDToRegisterRemove ();
	 break;
      default:
	 Lay_ShowAlert (Lay_ERROR,Txt_You_dont_have_permission_to_perform_this_action);
	 break;
     }
  }

/*****************************************************************************/
/****** Write a form to request another user's ID, @nickname or e-mail *******/
/*****************************************************************************/

static void Usr_ReqAnotherUsrIDToRegisterRemove (void)
  {
   extern const char *Txt_Enter_the_ID_of_the_user_you_want_to_register_remove_;

   Lay_ShowAlert (Lay_INFO,Txt_Enter_the_ID_of_the_user_you_want_to_register_remove_);

   Usr_WriteFormToReqAnotherUsrID (ActReqMdfUsr);
  }

/*****************************************************************************/
/********************** Ask me for register/remove me ************************/
/*****************************************************************************/

static void Usr_AskIfRegRemMe (void)
  {
   struct ListUsrCods ListUsrCods;

   /***** I only can admin me *****/
   Gbl.Usrs.Other.UsrDat.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   ListUsrCods.NumUsrs = 1;
   Usr_AllocateListUsrCods (&ListUsrCods);
   ListUsrCods.Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;

   Usr_AskIfRegRemUsr (&ListUsrCods);
  }

/*****************************************************************************/
/****************** Ask me for register/remove another user ******************/
/*****************************************************************************/

void Usr_AskIfRegRemAnotherUsr (void)
  {
   struct ListUsrCods ListUsrCods;

   /***** Check if UsrCod is present in parameters *****/
   Usr_GetParamOtherUsrCodEncrypted ();
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      /***** If UsrCod is present in parameters,
	     use this parameter to identify the user to be enrolled / removed *****/
      ListUsrCods.NumUsrs = 1;
      Usr_AllocateListUsrCods (&ListUsrCods);
      ListUsrCods.Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;
     }
   else        // Parameter with user code not present
      /***** If UsrCod is not present in parameters from form,
	     use user's ID to identify the user to be enrolled /removed *****/
      Usr_GetParamOtherUsrIDNickOrEMailAndGetUsrCods (&ListUsrCods);

   Usr_AskIfRegRemUsr (&ListUsrCods);
  }

/*****************************************************************************/
/********************** Ask me for register/remove user **********************/
/*****************************************************************************/

static void Usr_AskIfRegRemUsr (struct ListUsrCods *ListUsrCods)
  {
   extern const char *Txt_There_are_X_users_with_the_ID_Y;
   extern const char *Txt_THE_USER_X_is_already_enrolled_in_the_course_Y;
   extern const char *Txt_THE_USER_X_is_already_in_the_course_Y_but_has_not_yet_accepted_the_enrollment;
   extern const char *Txt_THE_USER_X_already_exists_in_Y_but_is_not_yet_enrolled_in_the_course_Z;
   extern const char *Txt_THE_USER_X_already_exists_in_Y;
   extern const char *Txt_The_user_is_new_does_not_exists_yet_in_X;
   extern const char *Txt_If_this_is_a_new_user_in_X_you_should_indicate_her_his_ID;
   unsigned NumUsr;
   bool NewUsrIDValid;

   if (ListUsrCods->NumUsrs)	// User(s) found with the ID
     {
      /***** Warning if more than one user found *****/
      if (ListUsrCods->NumUsrs > 1)
	{
	 sprintf (Gbl.Message,Txt_There_are_X_users_with_the_ID_Y,
		  ListUsrCods->NumUsrs,Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail);
	 Lay_ShowAlert (Lay_INFO,Gbl.Message);
	}

      /***** For each user found... *****/
      for (NumUsr = 0;
	   NumUsr < ListUsrCods->NumUsrs;
	   NumUsr++)
	{
	 /* Get user's data */
         Gbl.Usrs.Other.UsrDat.UsrCod = ListUsrCods->Lst[NumUsr];
         Usr_GetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat);

	 if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
	   {
	    /* Check if this user belongs to the current course */
	    if (Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod))
	      {
	       Gbl.Usrs.Other.UsrDat.Accepted = Usr_GetIfUserHasAcceptedEnrollmentInCurrentCrs (Gbl.Usrs.Other.UsrDat.UsrCod);
	       if (Gbl.Usrs.Other.UsrDat.Accepted)
		  sprintf (Gbl.Message,Txt_THE_USER_X_is_already_enrolled_in_the_course_Y,
			   Gbl.Usrs.Other.UsrDat.FullName,Gbl.CurrentCrs.Crs.FullName);
	       else        // Enrollment not yet accepted
		  sprintf (Gbl.Message,Txt_THE_USER_X_is_already_in_the_course_Y_but_has_not_yet_accepted_the_enrollment,
			   Gbl.Usrs.Other.UsrDat.FullName,Gbl.CurrentCrs.Crs.FullName);
	       Lay_ShowAlert (Lay_INFO,Gbl.Message);

	       Usr_ShowFormToEditOtherUsr ();
	      }
	    else        // User does not belong to the current course
	      {
	       sprintf (Gbl.Message,Txt_THE_USER_X_already_exists_in_Y_but_is_not_yet_enrolled_in_the_course_Z,
			Gbl.Usrs.Other.UsrDat.FullName,Cfg_PLATFORM_SHORT_NAME,Gbl.CurrentCrs.Crs.FullName);
	       Lay_ShowAlert (Lay_INFO,Gbl.Message);

	       Usr_ShowFormToEditOtherUsr ();
	      }
	   }
	 else	// No course selected
	   {
	    sprintf (Gbl.Message,Txt_THE_USER_X_already_exists_in_Y,
		     Gbl.Usrs.Other.UsrDat.FullName,Cfg_PLATFORM_SHORT_NAME);
	    Lay_ShowAlert (Lay_INFO,Gbl.Message);

	    Usr_ShowFormToEditOtherUsr ();
	   }
	}

      /***** Free list of users' codes *****/
      Usr_FreeListUsrCods (ListUsrCods);
     }
   else	// No users found, he/she is a new user
     {
      /***** If UsrCod is not present in parameters from form,
	     use user's ID to identify the user to be enrolled *****/
      if (Gbl.Usrs.Other.UsrDat.IDs.List)
         NewUsrIDValid = ID_CheckIfUsrIDIsValid (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);	// Check the first element of the list
      else
	 NewUsrIDValid = false;

      if (NewUsrIDValid)
	{
	 /* Initialize some data of this new user */
	 Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB = Rol_ROLE_STUDENT;
	 Gbl.Usrs.Other.UsrDat.Roles = (1 << Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB);

	 /***** Show form to enter the data of a new user *****/
	 sprintf (Gbl.Message,Txt_The_user_is_new_does_not_exists_yet_in_X,
		  Cfg_PLATFORM_SHORT_NAME);
	 Lay_ShowAlert (Lay_INFO,Gbl.Message);
	 Rec_ShowFormOtherNewCommonRecord (&Gbl.Usrs.Other.UsrDat);
	}
      else        // User's ID is not valid
	{
	 /* Write message and request a new user's ID */
	 sprintf (Gbl.Message,Txt_If_this_is_a_new_user_in_X_you_should_indicate_her_his_ID,
		  Cfg_PLATFORM_SHORT_NAME);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	 Usr_ReqRegRemUsr ();
	}
     }
  }

/*****************************************************************************/
/************ Show form to edit the record of an existing user ***************/
/*****************************************************************************/

static void Usr_ShowFormToEditOtherUsr (void)
  {
   extern const char *Txt_Confirm;
   bool ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);

   fprintf (Gbl.F.Out,"<div align=\"center\" style=\"margin-bottom:20px;\">");

   /***** Buttons for edition *****/
   if (Pwd_CheckIfICanChangeOtherUsrPassword (Gbl.Usrs.Other.UsrDat.UsrCod))
     {
      ID_PutLinkToChangeUsrIDs (&Gbl.Usrs.Other.UsrDat);	// Put link (form) to change user's IDs
      Pwd_PutLinkToChangeUsrPassword (&Gbl.Usrs.Other.UsrDat);	// Put link (form) to change user's password
     }
   if (Pho_CheckIfICanChangeOtherUsrPhoto (Gbl.Usrs.Other.UsrDat.UsrCod))
      Pho_PutLinkToChangeUsrPhoto (&Gbl.Usrs.Other.UsrDat);	// Put link (form) to change user's photo

   /***** User's record *****/
   Rec_ShowCommonRecord (Rec_FORM_MODIFY_RECORD_OTHER_EXISTING_USR,&Gbl.Usrs.Other.UsrDat);

   /***** Show list of groups to register/remove me/user *****/
   if (Gbl.CurrentCrs.Grps.NumGrps) // This course has groups?
     {
      if (ItsMe)
         Grp_ShowLstGrpsToChgMyGrps ((Gbl.Usrs.Me.LoggedRole == Rol_ROLE_STUDENT));
      else
         Grp_ShowLstGrpsToChgOtherUsrsGrps (Gbl.Usrs.Other.UsrDat.UsrCod);
     }

   /***** Which action, register or removing? *****/
   if (Usr_PutActionsRegRemOneUsr (ItsMe))
      Lay_PutSendButton (Txt_Confirm);

   fprintf (Gbl.F.Out,"</form>"
	              "</div>");
  }

/*****************************************************************************/
/******************* Add an administrator to current degree ******************/
/*****************************************************************************/

void Usr_AddAdmToDeg (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   if (Gbl.CurrentDeg.Deg.DegCod > 0)
     {
      /***** Get plain user's ID of the user to add/modify *****/
      if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
        {
         /* Check if it's allowed to register this administrator in degree */
         if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
           {
            /***** Register administrator in current degree in database *****/
            Usr_RegisterAdminInCurrentDeg (&Gbl.Usrs.Other.UsrDat);

            /***** Show user's record *****/
            Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
           }
         else
            Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
        }
      else
         Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
  }

/*****************************************************************************/
/****************** Ask for remove me from current course ********************/
/*****************************************************************************/

void Usr_ReqRemMeFromCrs (void)
  {
   Usr_AskIfRemoveUsrFromCrs (&Gbl.Usrs.Me.UsrDat,true);
  }

/*****************************************************************************/
/************** Ask for remove of a user from current course *****************/
/*****************************************************************************/

void Usr_ReqRemUsrFromCrs (void)
  {
   Usr_ReqRemOrRemUsrFromCrs (Usr_REQUEST_REMOVE_USR);
  }

/*****************************************************************************/
/********************* Remove a user from current course *********************/
/*****************************************************************************/

void Usr_RemUsrFromCrs (void)
  {
   if (Pwd_GetConfirmationOnDangerousAction ())
      Usr_ReqRemOrRemUsrFromCrs (Usr_REMOVE_USR);
  }

/*****************************************************************************/
/******************** Remove of a user from current course *******************/
/*****************************************************************************/

void Usr_ReqRemOrRemUsrFromCrs (Usr_ReqDelOrDelUsr_t ReqDelOrDelUsr)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   bool ItsMe;
   bool ICanRemove;

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      /* Check if it's forbidden remove that user */
      // A teacher can remove a student or himself
      // An administrator can remove anyone
      ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);
      ICanRemove = (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_STUDENT ? ItsMe :
                                                                 (Gbl.Usrs.Me.LoggedRole >= Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB));
      if (ICanRemove)
	 switch (ReqDelOrDelUsr)
	   {
	    case Usr_REQUEST_REMOVE_USR:        // Ask if remove user from current course
	       Usr_AskIfRemoveUsrFromCrs (&Gbl.Usrs.Other.UsrDat,ItsMe);
	       break;
	    case Usr_REMOVE_USR:                // Remove user from current course
	       Usr_EffectivelyRemUsrFromCrs (&Gbl.Usrs.Other.UsrDat,&Gbl.CurrentCrs.Crs,
	                                     Usr_REMOVE_WORKS,Cns_VERBOSE);
	       break;
	   }
      else
         Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/********* Ask for remove of an administrator from current degree ************/
/*****************************************************************************/

static void Usr_ReqRemAdmOfDeg (void)
  {
   Usr_ReqRemOrRemAdm (Usr_REQUEST_REMOVE_USR);
  }

/*****************************************************************************/
/********************* Remove a user from current course *********************/
/*****************************************************************************/

void Usr_RemAdm (void)
  {
   Usr_ReqRemOrRemAdm (Usr_REMOVE_USR);
  }

/*****************************************************************************/
/******************** Remove of a user from current course *******************/
/*****************************************************************************/

static void Usr_ReqRemOrRemAdm (Usr_ReqDelOrDelUsr_t ReqDelOrDelUsr)
  {
   extern const char *Txt_THE_USER_X_is_not_an_administrator_of_the_degree_Y;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   bool ItsMe;
   bool ICanRemove;

   if (Gbl.CurrentDeg.Deg.DegCod > 0)
     {
      /***** Get user to be removed *****/
      if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
        {
         /* Check if it's forbidden remove that administrator */
         // A superuser can remove any administrator
         // An administrator only can remove itself
         ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);
         ICanRemove = (ItsMe || Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER);
         if (ICanRemove)
           {
            /* Check if the other user belong to current course */
            if (Usr_CheckIfUsrIsAdmOfDeg (Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.CurrentDeg.Deg.DegCod))
              {                // The other user is an administrator of current degree ==> ask for removing or remove him
               switch (ReqDelOrDelUsr)
                 {
                  case Usr_REQUEST_REMOVE_USR:     // Ask if remove administrator from current degree
                     Usr_AskIfRemAdmFromDeg (ItsMe);
                     break;
                  case Usr_REMOVE_USR:             // Remove administrator from current degree
                     Usr_EffectivelyRemAdmFromDeg (&Gbl.Usrs.Other.UsrDat);
                     break;
                 }
              }
            else        // The other user is not an administrator of current degree
              {
               sprintf (Gbl.Message,Txt_THE_USER_X_is_not_an_administrator_of_the_degree_Y,
                        Gbl.Usrs.Other.UsrDat.FullName,Gbl.CurrentDeg.Deg.FullName);
               Lay_ShowAlert (Lay_WARNING,Gbl.Message);
              }
           }
         else
            Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
        }
      else
         Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
  }

/*****************************************************************************/
/********* Request definite removing of a user from the platform *************/
/*****************************************************************************/

void Usr_ReqRemUsrGbl (void)
  {
   Usr_ReqDelOrDelUsrGbl (Usr_REQUEST_REMOVE_USR);
  }

/*****************************************************************************/
/************** Definite removing of a user from the platform ****************/
/*****************************************************************************/

void Usr_RemUsrGbl (void)
  {
   Usr_ReqDelOrDelUsrGbl (Usr_REMOVE_USR);
  }

/*****************************************************************************/
/**************************** Removing of a user *****************************/
/*****************************************************************************/

void Usr_ReqDelOrDelUsrGbl (Usr_ReqDelOrDelUsr_t ReqDelOrDelUsr)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   bool ItsMe;

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);
      if (Usr_CheckIfICanEliminateAccount (ItsMe))
         switch (ReqDelOrDelUsr)
           {
            case Usr_REQUEST_REMOVE_USR:	// Ask if eliminate completely the user from the platform
               Usr_AskIfCompletelyEliminateUsrFromPlatform (ItsMe);
               break;
            case Usr_REMOVE_USR:		// Eliminate completely the user from the platform
               if (Pwd_GetConfirmationOnDangerousAction ())
        	 {
                  Usr_DeleteUsrFromPlatform (&Gbl.Usrs.Other.UsrDat,Cns_VERBOSE);

                  /***** Move unused contents of messages to table of deleted contents of messages *****/
                  Msg_MoveUnusedMsgsContentToDeleted ();
        	 }
               break;
           }
      else
         Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/******************* Ask if really wanted to remove a user *******************/
/*****************************************************************************/

static void Usr_AskIfRemoveUsrFromCrs (struct UsrData *UsrDat,bool ItsMe)
  {
   extern const char *Txt_Do_you_really_want_to_be_removed_from_the_course_X;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_user_from_the_course_X;
   extern const char *Txt_Remove_me_from_this_course;
   extern const char *Txt_Remove_user_from_this_course;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   if (Usr_CheckIfUsrBelongsToCrs (UsrDat->UsrCod,Gbl.CurrentCrs.Crs.CrsCod))
     {
      sprintf (Gbl.Message,
               ItsMe ? Txt_Do_you_really_want_to_be_removed_from_the_course_X :
		       Txt_Do_you_really_want_to_remove_the_following_user_from_the_course_X,
	       Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);

      Rec_ShowCommonRecordUnmodifiable (UsrDat);

      fprintf (Gbl.F.Out,"<div align=\"center\">");
      Act_FormStart (ActRemUsrCrs);
      Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Pwd_AskForConfirmationOnDangerousAction ();
      fprintf (Gbl.F.Out,"<input type=\"submit\" value=\"%s\" />",
               ItsMe ? Txt_Remove_me_from_this_course :
                       Txt_Remove_user_from_this_course);
      fprintf (Gbl.F.Out,"</form>"
	                 "</div>");
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/*********** Ask if really wanted to eliminate completely a user *************/
/*****************************************************************************/

static void Usr_AskIfCompletelyEliminateUsrFromPlatform (bool ItsMe)
  {
   extern const char *Txt_Do_you_really_want_to_completely_eliminate_your_user_account;
   extern const char *Txt_Do_you_really_want_to_completely_eliminate_the_following_user;
   extern const char *Txt_Completely_eliminate_me;
   extern const char *Txt_Completely_eliminate_user;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   if (Usr_ChkIfUsrCodExists (Gbl.Usrs.Other.UsrDat.UsrCod))
     {
      Lay_ShowAlert (Lay_WARNING,ItsMe ? Txt_Do_you_really_want_to_completely_eliminate_your_user_account :
				         Txt_Do_you_really_want_to_completely_eliminate_the_following_user);

      Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

      Act_FormStart (ActRemUsrGbl);
      Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);

      /* Ask for consent on dangerous actions */
      Pwd_AskForConfirmationOnDangerousAction ();

      fprintf (Gbl.F.Out,"<div align=\"center\">"
	                 "<input type=\"submit\" value=\"%s\" />"
	                 "</div>"
	                 "</form>",
               ItsMe ? Txt_Completely_eliminate_me :
                       Txt_Completely_eliminate_user);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/****** Ask if really wanted to add an administrator to current degree *******/
/*****************************************************************************/

static void Usr_ReqAddAdmOfDeg (void)
  {
   extern const char *Txt_THE_USER_X_is_already_an_administrator_of_the_degree_Y;
   extern const char *Txt_Do_you_really_want_to_register_the_following_user_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Register_user_IN_A_COURSE_OR_DEGREE;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   if (Gbl.CurrentDeg.Deg.DegCod > 0)
     {
      /***** Get user's identificator of the user to register as admin *****/
      if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
        {
         /* Check if it's allowed to register this administrator in degree */
         if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
           {
            if (Usr_CheckIfUsrIsAdmOfDeg (Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.CurrentDeg.Deg.DegCod))        // User is yet an administrator of current degree
              {
               sprintf (Gbl.Message,Txt_THE_USER_X_is_already_an_administrator_of_the_degree_Y,
                        Gbl.Usrs.Other.UsrDat.FullName,Gbl.CurrentDeg.Deg.FullName);
               Lay_ShowAlert (Lay_INFO,Gbl.Message);
               Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
              }
            else
              {
               sprintf (Gbl.Message,Txt_Do_you_really_want_to_register_the_following_user_as_an_administrator_of_the_degree_X,
                        Gbl.CurrentDeg.Deg.FullName);
               Lay_ShowAlert (Lay_INFO,Gbl.Message);
               Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

               Act_FormStart (ActNewAdm);
               Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
               fprintf (Gbl.F.Out,"<div align=\"center\">"
        	                  "<input type=\"submit\" value=\"%s\" />"
        	                  "</div>"
        	                  "</form>",
                        Txt_Register_user_IN_A_COURSE_OR_DEGREE);
              }
           }
         else
            Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
        }
      else
         Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
  }

/*****************************************************************************/
/**** Ask if really wanted to remove an administrator from current degree ****/
/*****************************************************************************/

void Usr_AskIfRemAdmFromDeg (bool ItsMe)
  {
   extern const char *Txt_Do_you_really_want_to_be_removed_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_user_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Remove_me_as_an_administrator;
   extern const char *Txt_Remove_user_as_an_administrator;

   if (Usr_ChkIfUsrCodExists (Gbl.Usrs.Other.UsrDat.UsrCod))
     {
      sprintf (Gbl.Message,
               ItsMe ? Txt_Do_you_really_want_to_be_removed_as_an_administrator_of_the_degree_X :
                       Txt_Do_you_really_want_to_remove_the_following_user_as_an_administrator_of_the_degree_X,
               Gbl.CurrentDeg.Deg.FullName);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);

      Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

      Act_FormStart (ActRemAdm);
      Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
      fprintf (Gbl.F.Out,"<div align=\"center\">"
	                 "<input type=\"submit\" value=\"%s\" />"
	                 "</div>"
	                 "</form>",
               ItsMe ? Txt_Remove_me_as_an_administrator :
                       Txt_Remove_user_as_an_administrator);
     }
   else
      Lay_ShowErrorAndExit ("User doen't exist.");
  }

/*****************************************************************************/
/********** Build the relative path of a user from his user's code ***********/
/*****************************************************************************/

static void Usr_ConstructPathUsr (long UsrCod,char *PathUsr)
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
   if (Role == Rol_ROLE_GUEST)	// Users not beloging to any course
      NumUsrs = Usr_GetNumUsrsNotBelongingToAnyCrs ();
   else
      NumUsrs = Usr_GetNumUsrsBelongingToAnyCrs (Role);

   /***** Get average number of courses per user *****/
   NumCrssPerUsr = (Role == Rol_ROLE_GUEST) ? 0 :
	                                      Usr_GetNumCrssPerUsr (Role);

   /***** Query the number of users per course *****/
   NumUsrsPerCrs = (Role == Rol_ROLE_GUEST) ? 0 :
	                                      Usr_GetNumUsrsPerCrs (Role);

   /***** Write the total number of users *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td align=\"right\" valign=\"bottom\" class=\"DAT\">%s</td>"
                      "<td align=\"right\" valign=\"bottom\" class=\"DAT\">%u</td>"
                      "<td align=\"right\" valign=\"bottom\" class=\"DAT\">%.2f</td>"
                      "<td align=\"right\" valign=\"bottom\" class=\"DAT\">%.2f</td>"
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
      case Sco_SCOPE_PLATFORM:
         sprintf (Query,"SELECT COUNT(DISTINCT UsrCod)"
                        " FROM crs_usr"
                        " WHERE Role='%u'",
                  (unsigned) Role);
         break;
      case Sco_SCOPE_INSTITUTION:
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
      case Sco_SCOPE_CENTRE:
         sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
                        " FROM degrees,courses,crs_usr"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.Role='%u'",
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  (unsigned) Role);
         break;
      case Sco_SCOPE_DEGREE:
         sprintf (Query,"SELECT COUNT(DISTINCT crs_usr.UsrCod)"
                        " FROM courses,crs_usr"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.Role='%u'",
                  Gbl.CurrentDeg.Deg.DegCod,
                  (unsigned) Role);
         break;
      case Sco_SCOPE_COURSE:
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
      case Sco_SCOPE_PLATFORM:
         sprintf (Query,"SELECT AVG(NumCrss) FROM "
                        "(SELECT COUNT(CrsCod) AS NumCrss"
                        " FROM crs_usr"
                        " WHERE Role='%u' GROUP BY UsrCod) AS NumCrssTable",
                  (unsigned) Role);
         break;
      case Sco_SCOPE_INSTITUTION:
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
      case Sco_SCOPE_CENTRE:
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
      case Sco_SCOPE_DEGREE:
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
      case Sco_SCOPE_COURSE:
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
      case Sco_SCOPE_PLATFORM:
         sprintf (Query,"SELECT AVG(NumUsrs) FROM "
                        "(SELECT COUNT(UsrCod) AS NumUsrs"
                        " FROM crs_usr"
                        " WHERE Role='%u' GROUP BY CrsCod) AS NumUsrsTable",
                  (unsigned) Role);
         break;
      case Sco_SCOPE_INSTITUTION:
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
      case Sco_SCOPE_CENTRE:
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
      case Sco_SCOPE_DEGREE:
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
      case Sco_SCOPE_COURSE:
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
