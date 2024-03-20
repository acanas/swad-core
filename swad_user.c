// swad_user.c: users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <ctype.h>		// For isalnum, isdigit, etc.
#include <limits.h>		// For maximum values
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For exit, system, malloc, free, rand, etc.
#include <string.h>		// For string functions
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For access, lstat, getpid, chdir, symlink, unlink

#include "swad_account.h"
#include "swad_action_list.h"
#include "swad_admin.h"
#include "swad_admin_database.h"
#include "swad_agenda.h"
#include "swad_announcement.h"
#include "swad_box.h"
#include "swad_calendar.h"
#include "swad_center_database.h"
#include "swad_config.h"
#include "swad_constant.h"
#include "swad_connected_database.h"
#include "swad_country_database.h"
#include "swad_course.h"
#include "swad_database.h"
#include "swad_degree_database.h"
#include "swad_department.h"
#include "swad_duplicate.h"
#include "swad_enrolment.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_figure_cache.h"
#include "swad_follow.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_help.h"
#include "swad_hidden_visible.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_institution_database.h"
#include "swad_language.h"
#include "swad_mail_database.h"
#include "swad_message.h"
#include "swad_MFU.h"
#include "swad_nickname.h"
#include "swad_nickname_database.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_password.h"
#include "swad_photo.h"
#include "swad_privacy.h"
#include "swad_profile.h"
#include "swad_QR.h"
#include "swad_record.h"
#include "swad_record_database.h"
#include "swad_role.h"
#include "swad_session.h"
#include "swad_session_database.h"
#include "swad_setting.h"
#include "swad_tab.h"
#include "swad_user.h"
#include "swad_user_database.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *Usr_StringsSexIcons[Usr_NUM_SEXS] =
  {
   [Usr_SEX_UNKNOWN] = "?",
   [Usr_SEX_FEMALE ] = "&female;",
   [Usr_SEX_MALE   ] = "&male;",
   [Usr_SEX_ALL    ] = "*",
   };

const char *Usr_StringsSexDB[Usr_NUM_SEXS] =
  {
   [Usr_SEX_UNKNOWN] = "unknown",
   [Usr_SEX_FEMALE ] = "female",
   [Usr_SEX_MALE   ] = "male",
   [Usr_SEX_ALL    ] = "all",
   };

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *Usr_IconsClassPhotoOrList[Set_NUM_USR_LIST_TYPES] =
  {
   [Set_USR_LIST_UNKNOWN       ] = "",
   [Set_USR_LIST_AS_CLASS_PHOTO] = "th.svg",
   [Set_USR_LIST_AS_LISTING    ] = "list-ol.svg",
  };

static const char *Usr_NameSelUnsel[Rol_NUM_ROLES] =
  {
   [Rol_GST] = "SEL_UNSEL_GSTS",
   [Rol_STD] = "SEL_UNSEL_STDS",
   [Rol_NET] = "SEL_UNSEL_NETS",
   [Rol_TCH] = "SEL_UNSEL_TCHS",
  };
static const char *Usr_ParUsrCod[Rol_NUM_ROLES] =
  {
   [Rol_UNK] = "UsrCodAll",	//  here means all users
   [Rol_GST] = "UsrCodGst",
   [Rol_STD] = "UsrCodStd",
   [Rol_NET] = "UsrCodNET",
   [Rol_TCH] = "UsrCodTch",
  };

#define Usr_NUM_MAIN_FIELDS_DATA_ADM	 7
#define Usr_NUM_ALL_FIELDS_DATA_GST	14
#define Usr_NUM_ALL_FIELDS_DATA_STD	10
#define Usr_NUM_ALL_FIELDS_DATA_TCH	11
const char *Usr_UsrDatMainFieldNames[Usr_NUM_MAIN_FIELDS_DATA_USR];

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Private variables ****************************/
/*****************************************************************************/

static void (*Usr_FuncParsBigList) (void *Args);	// Used to pass pointer to function

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Usr_GetMyLastData (void);
static void Usr_GetUsrCommentsFromString (char *Str,struct Usr_Data *UsrDat);
static Usr_Sex_t Usr_GetSexFromStr (const char *Str);

static void Usr_GetParOtherUsrIDNickOrEMail (void);

static bool Usr_ChkUsrAndGetUsrDataFromDirectLogin (void);
static bool Usr_ChkUsrAndGetUsrDataFromSession (void);
static void Usr_ShowAlertUsrDoesNotExistsOrWrongPassword (void);
static void Usr_ShowAlertThereAreMoreThanOneUsr (void);

static void Usr_SetMyPrefsAndRoles (void);

static void Usr_PutLinkToLogOut (__attribute__((unused)) void *Args);

static void Usr_WriteRowGstAllData (struct Usr_Data *UsrDat);
static void Usr_WriteRowStdAllData (struct Usr_Data *UsrDat,char *GroupNames);
static void Usr_WriteRowTchAllData (struct Usr_Data *UsrDat);
static void Usr_WriteRowAdmData (unsigned NumUsr,struct Usr_Data *UsrDat);
static void Usr_WriteMainUsrDataExceptUsrID (struct Usr_Data *UsrDat,
                                             const char *BgColor);
static void Usr_WriteEmail (struct Usr_Data *UsrDat,const char *BgColor);
static void Usr_WriteUsrData (const char *BgColor,
                              const char *Data,const char *Link,
                              bool NonBreak,bool Accepted);

static void Usr_GetGstsLst (Hie_Level_t Level);
static void Usr_AllocateUsrsList (Rol_Role_t Role);

static void Usr_PutButtonToConfirmIWantToSeeBigList (unsigned NumUsrs,
						     Act_Action_t NextAction,
                                                     void (*FuncPars) (void *Args),void *Args,
                                                     const char *OnSubmit);
static void Usr_PutParsConfirmIWantToSeeBigList (void *Args);

static void Usr_BuildParName (char **ParName,
			      const char *ParRoot,
			      const char *ParSuffix);

static void Usr_AllocateListSelectedEncryptedUsrCods (struct Usr_SelectedUsrs *SelectedUsrs,
						      Rol_Role_t Role);
static void Usr_AllocateListOtherRecipients (void);

static void Set_FormToSelectUsrListType (Act_Action_t NextAction,
					 void (*FuncPars) (void *Args),void *Args,
					 const char *OnSubmit,
					 Set_ShowUsrsType_t ListType);
static void Usr_ListUsersByRoleToSelect (struct Usr_SelectedUsrs *SelectedUsrs,
					 Rol_Role_t Role);
static void Usr_ListUsrsForSelection (struct Usr_SelectedUsrs *SelectedUsrs,
				      Rol_Role_t Role);
static void Usr_PutCheckboxToSelectAllUsers (struct Usr_SelectedUsrs *SelectedUsrs,
					     Rol_Role_t Role);
static Usr_Sex_t Usr_GetSexOfUsrsLst (Rol_Role_t Role);
static void Usr_PutCheckboxToSelectUser (Rol_Role_t Role,
                                         const char *EncryptedUsrCod,
                                         bool UsrIsTheMsgSender,
					 struct Usr_SelectedUsrs *SelectedUsrs);
static void Usr_PutCheckboxListWithPhotos (void);

static void Usr_ListMainDataGsts (bool PutCheckBoxToSelectUsr);
static void Usr_ListMainDataStds (bool PutCheckBoxToSelectUsr);
static void Usr_ListMainDataTchs (Rol_Role_t Role,
				  bool PutCheckBoxToSelectUsr);
static void Usr_ListRowsAllDataTchs (Rol_Role_t Role,
                                     const char *FieldNames[Usr_NUM_ALL_FIELDS_DATA_TCH],
                                     unsigned NumColumns);

static void Usr_PutLinkToSeeAdmins (void);
static void Usr_PutLinkToSeeGuests (void);

static Frm_PutForm_t Usr_SetOptionsListUsrsAllowed (Rol_Role_t UsrsRole,
                                                    bool ICanChooseOption[Usr_LIST_USRS_NUM_OPTIONS]);
static void Usr_PutOptionsListUsrs (const bool ICanChooseOption[Usr_LIST_USRS_NUM_OPTIONS]);
static void Usr_ShowOneListUsrsOption (Usr_ListUsrsOption_t ListUsrsAction,
                                       const char *Label);
static Usr_ListUsrsOption_t Usr_GetListUsrsOption (Usr_ListUsrsOption_t DefaultAction);

static void Usr_PutIconsListGsts (__attribute__((unused)) void *Args);
static void Usr_PutIconsListStds (__attribute__((unused)) void *Args);
static void Usr_PutIconsListTchs (__attribute__((unused)) void *Args);

static void Usr_PutIconToPrintGsts (void);
static void Usr_PutIconToPrintStds (void);
static void Usr_PutIconToPrintTchs (void);
static void Usr_PutIconToShowGstsAllData (void);
static void Usr_PutIconToShowStdsAllData (void);
static void Usr_PutIconToShowTchsAllData (void);
static void Usr_ShowGstsAllDataPars (__attribute__((unused)) void *Args);
static void Usr_ShowStdsAllDataPars (__attribute__((unused)) void *Args);
static void Usr_ShowTchsAllDataPars (__attribute__((unused)) void *Args);

static void Usr_DrawClassPhoto (struct Usr_SelectedUsrs *SelectedUsrs,
                                Rol_Role_t Role,
				Usr_ClassPhotoType_t ClassPhotoType,
				bool PutCheckBoxToSelectUsr);

static void Usr_GetAndShowNumUsrsInCrss (Rol_Role_t Role);
static void Usr_GetAndShowNumUsrsNotBelongingToAnyCrs (void);

/*****************************************************************************/
/**** Show alert about number of clicks remaining before sending my photo ****/
/*****************************************************************************/

void Usr_InformAboutNumClicksBeforePhoto (void)
  {
   extern const char *Txt_You_must_send_your_photo_because_;
   extern const char *Txt_You_can_only_perform_X_further_actions_;
   extern const char *Txt_Upload_photo;

   if (Gbl.Usrs.Me.NumAccWithoutPhoto)
     {
      if (Gbl.Usrs.Me.NumAccWithoutPhoto >= Pho_MAX_CLICKS_WITHOUT_PHOTO)
         Ale_ShowAlert (Ale_WARNING,Txt_You_must_send_your_photo_because_);
      else if (Act_GetBrowserTab (Gbl.Action.Act) == Act_1ST_TAB)
         Ale_ShowAlertAndButton (ActReqMyPho,NULL,NULL,
                                 NULL,NULL,
                                 Btn_CONFIRM_BUTTON,Txt_Upload_photo,
				 Ale_WARNING,Txt_You_can_only_perform_X_further_actions_,
                                 Pho_MAX_CLICKS_WITHOUT_PHOTO - Gbl.Usrs.Me.NumAccWithoutPhoto);
     }
  }

/*****************************************************************************/
/************************** Create data for a user ***************************/
/*****************************************************************************/

void Usr_UsrDataConstructor (struct Usr_Data *UsrDat)
  {
   /***** Allocate memory for the comments *****/
   if ((UsrDat->Comments = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Initialize to zero the data of the user *****/
   Usr_ResetUsrDataExceptUsrCodAndIDs (UsrDat);
   UsrDat->IDs.Num = 0;
   UsrDat->IDs.List = NULL;
  }

/*****************************************************************************/
/****************** Reset user's data except UsrCod and ID *******************/
/*****************************************************************************/
// UsrCod and ID are not changed

void Usr_ResetUsrDataExceptUsrCodAndIDs (struct Usr_Data *UsrDat)
  {
   UsrDat->EnUsrCod[0] = '\0';
   UsrDat->Nickname[0] = '\0';
   UsrDat->Password[0] = '\0';
   UsrDat->Roles.InCurrentCrs = Rol_UNK;// not yet got from database
   UsrDat->Roles.InCrss = -1;		// not yet got from database
   UsrDat->Accepted = false;

   UsrDat->Sex = Usr_SEX_UNKNOWN;
   UsrDat->Surname1[0]  = '\0';
   UsrDat->Surname2[0]  = '\0';
   UsrDat->FrstName[0] = '\0';
   UsrDat->FullName[0]  = '\0';

   UsrDat->Email[0] = '\0';
   UsrDat->EmailConfirmed = false;

   UsrDat->Photo[0] = '\0';
   UsrDat->PhotoVisibility = Pri_PHOTO_VIS_DEFAULT;
   UsrDat->BaPrfVisibility = Pri_BASIC_PROFILE_VIS_DEFAULT;
   UsrDat->ExPrfVisibility = Pri_EXTENDED_PROFILE_VIS_DEFAULT;

   UsrDat->CtyCod = -1L;
   UsrDat->StrBirthday[0] = '\0';
   UsrDat->Birthday.Day   = 0;
   UsrDat->Birthday.Month = 0;
   UsrDat->Birthday.Year  = 0;
   UsrDat->Phone[0][0]    =
   UsrDat->Phone[1][0]    = '\0';
   if (UsrDat->Comments)
      UsrDat->Comments[0] = '\0';

   UsrDat->InsCtyCod  = -1L;
   UsrDat->InsCod     = -1L;
   UsrDat->Tch.CtrCod = -1L;
   UsrDat->Tch.DptCod = -1L;
   UsrDat->Tch.Office[0]      = '\0';
   UsrDat->Tch.OfficePhone[0] = '\0';

   UsrDat->Prefs.Language	= Lan_LANGUAGE_UNKNOWN;			// Language unknown
   UsrDat->Prefs.FirstDayOfWeek	= Cal_FIRST_DAY_OF_WEEK_DEFAULT;	// Default first day of week
   UsrDat->Prefs.DateFormat	= Dat_FORMAT_DEFAULT		;	// Default date format
   UsrDat->Prefs.Theme		= The_THEME_DEFAULT;
   UsrDat->Prefs.IconSet	= Ico_ICON_SET_DEFAULT;
   UsrDat->Prefs.Menu		= Mnu_MENU_DEFAULT;
   UsrDat->Prefs.SideCols	= Cfg_DEFAULT_COLUMNS;
   UsrDat->Prefs.PhotoShape	= PhoSha_SHAPE_DEFAULT;
   UsrDat->Prefs.RefuseAcceptCookies = Coo_REFUSE;	// By default, don't accept third party cookies
   UsrDat->NtfEvents.SendEmail	= 0;       		// By default, don't notify anything
  }

/*****************************************************************************/
/**************************** Reset my last data *****************************/
/*****************************************************************************/

void Usr_ResetMyLastData (void)
  {
   Gbl.Usrs.Me.UsrLast.WhatToSearch  = Sch_WHAT_TO_SEARCH_DEFAULT;
   Gbl.Usrs.Me.UsrLast.LastHie.Level = Hie_UNK;
   Gbl.Usrs.Me.UsrLast.LastHie.HieCod   = -1L;
   Gbl.Usrs.Me.UsrLast.LastAct       = ActUnk;
   Gbl.Usrs.Me.UsrLast.LastRole      = Rol_UNK;
   Gbl.Usrs.Me.UsrLast.LastTime      = 0;
   Gbl.Usrs.Me.UsrLast.LastAccNotif  = 0;
  }

/*****************************************************************************/
/**************** Free memory used to store the data of a user ***************/
/*****************************************************************************/

void Usr_UsrDataDestructor (struct Usr_Data *UsrDat)
  {
   /***** Free memory allocated for comments *****/
   if (UsrDat->Comments)
     {
      free (UsrDat->Comments);
      UsrDat->Comments = NULL;
     }

   /***** Free memory allocated for list of IDs *****/
   ID_FreeListIDs (UsrDat);
  }

/*****************************************************************************/
/*************** Get all user's data from a given user's code ****************/
/*****************************************************************************/
// Input: UsrDat->UsrCod must hold user's code

void Usr_GetAllUsrDataFromUsrCod (struct Usr_Data *UsrDat,
                                  Usr_GetPrefs_t GetPrefs,
                                  Usr_GetRoleInCurrentCrs_t GetRoleInCurrentCrs)
  {
   ID_GetListIDsFromUsrCod (UsrDat);
   Usr_GetUsrDataFromUsrCod (UsrDat,GetPrefs,GetRoleInCurrentCrs);
  }

/*****************************************************************************/
/**************** Allocate memory for the list of users' codes ***************/
/*****************************************************************************/

void Usr_AllocateListUsrCods (struct Usr_ListUsrCods *ListUsrCods)
  {
   if ((ListUsrCods->Lst = malloc (ListUsrCods->NumUsrs *
                                   sizeof (*ListUsrCods->Lst))) == NULL)
      Err_NotEnoughMemoryExit ();
  }

/*****************************************************************************/
/****************** Free memory for the list of users' codes *****************/
/*****************************************************************************/

void Usr_FreeListUsrCods (struct Usr_ListUsrCods *ListUsrCods)
  {
   if (ListUsrCods->NumUsrs && ListUsrCods->Lst)
     {
      free (ListUsrCods->Lst);
      ListUsrCods->Lst = NULL;
      ListUsrCods->NumUsrs = 0;
     }
  }

/*****************************************************************************/
/************************ Check if I am a given user *************************/
/*****************************************************************************/

Usr_MeOrOther_t Usr_ItsMe (long UsrCod)
  {
   return (Gbl.Usrs.Me.Logged &&
	   UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod) ? Usr_ME :
		                                  Usr_OTHER;
  }

/*****************************************************************************/
/******** Get user's code from database using encrypted user's code **********/
/*****************************************************************************/
// Input: UsrDat->EncryptedUsrCod must hold user's encrypted code

void Usr_GetUsrCodFromEncryptedUsrCod (struct Usr_Data *UsrDat)
  {
   if (UsrDat->EnUsrCod[0])
      /***** Get user's code from database *****/
      UsrDat->UsrCod = Usr_DB_GetUsrCodFromEncryptedUsrCod (UsrDat->EnUsrCod);
   else
      UsrDat->UsrCod = -1L;
  }

/*****************************************************************************/
/************ Get user's data from database giving a user's code *************/
/*****************************************************************************/
// UsrDat->UsrCod must contain an existing user's code

void Usr_GetUsrDataFromUsrCod (struct Usr_Data *UsrDat,
                               Usr_GetPrefs_t GetPrefs,
                               Usr_GetRoleInCurrentCrs_t GetRoleInCurrentCrs)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get user's data from database *****/
   if (Usr_DB_GetUsrDataFromUsrCod (&mysql_res,UsrDat->UsrCod,GetPrefs))
     {
      /***** Read user's data *****/
      row = mysql_fetch_row (mysql_res);

      /* Get encrypted user's code (row[0])
         and encrypted password (row[1]) */
      Str_Copy (UsrDat->EnUsrCod,row[0],sizeof (UsrDat->EnUsrCod) - 1);
      Str_Copy (UsrDat->Password,row[1],sizeof (UsrDat->Password) - 1);

      /* Get roles */
      switch (GetRoleInCurrentCrs)
	{
	 case Usr_DONT_GET_ROLE_IN_CRS:
	    UsrDat->Roles.InCurrentCrs = Rol_UNK;
	    UsrDat->Roles.InCrss = -1;	// Force roles to be got from database
	    break;
	 case Usr_GET_ROLE_IN_CRS:
	    UsrDat->Roles.InCurrentCrs = Rol_GetRoleUsrInCrs (UsrDat->UsrCod,
							      Gbl.Hierarchy.Node[Hie_CRS].HieCod);
	    UsrDat->Roles.InCrss = -1;	// Force roles to be got from database
	    break;
	}

      /* Get name (row[2], row[3], row[4]) */
      Str_Copy (UsrDat->Surname1,row[2],sizeof (UsrDat->Surname1) - 1);
      Str_Copy (UsrDat->Surname2,row[3],sizeof (UsrDat->Surname2) - 1);
      Str_Copy (UsrDat->FrstName,row[4],sizeof (UsrDat->FrstName) - 1);
      Str_ConvertToTitleType (UsrDat->Surname1 );
      Str_ConvertToTitleType (UsrDat->Surname2 );
      Str_ConvertToTitleType (UsrDat->FrstName);
      Usr_BuildFullName (UsrDat);	// Create full name using FirstName, Surname1 and Surname2

      /* Get sex (row[5]) */
      UsrDat->Sex = Usr_GetSexFromStr (row[5]);

      /* Get photo (row[6]) */
      Str_Copy (UsrDat->Photo,row[6],sizeof (UsrDat->Photo) - 1);

      /* Get photo visibility (row[7]) */
      UsrDat->PhotoVisibility = Pri_GetVisibilityFromStr (row[7]);

      /* Get profile visibility (row[8], row[9]) */
      UsrDat->BaPrfVisibility = Pri_GetVisibilityFromStr (row[8]);
      UsrDat->ExPrfVisibility = Pri_GetVisibilityFromStr (row[9]);

      /* Get country (row[10]), institution country (row[11]),
             institution (row[12]), department (row[13]) and center (row[14]) */
      UsrDat->CtyCod     = Str_ConvertStrCodToLongCod (row[10]);
      UsrDat->InsCtyCod  = Str_ConvertStrCodToLongCod (row[11]);
      UsrDat->InsCod     = Str_ConvertStrCodToLongCod (row[12]);
      UsrDat->Tch.DptCod = Str_ConvertStrCodToLongCod (row[13]);
      UsrDat->Tch.CtrCod = Str_ConvertStrCodToLongCod (row[14]);

      /* Get office (row[15]) and office phone (row[16]) */
      Str_Copy (UsrDat->Tch.Office     ,row[15],sizeof (UsrDat->Tch.Office     ) - 1);
      Str_Copy (UsrDat->Tch.OfficePhone,row[16],sizeof (UsrDat->Tch.OfficePhone) - 1);

      /* Get phones (row[17]) and row[18] */
      Str_Copy (UsrDat->Phone[0],row[17],sizeof (UsrDat->Phone[0]) - 1);
      Str_Copy (UsrDat->Phone[1],row[18],sizeof (UsrDat->Phone[1]) - 1);

      /* Get birthday (row[19]) */
      Dat_GetDateFromYYYYMMDD (&(UsrDat->Birthday),row[19]);
      Dat_ConvDateToDateStr (&(UsrDat->Birthday),UsrDat->StrBirthday);

      /* Get comments (row[20]) */
      Usr_GetUsrCommentsFromString (row[20] ? row[20] :
					      "",
				    UsrDat);

      /* Get on which events the user wants to be notified inside the platform (row[21]) */
      if (sscanf (row[21],"%u",&UsrDat->NtfEvents.CreateNotif) != 1)
	 UsrDat->NtfEvents.CreateNotif = (unsigned) -1;	// 0xFF..FF

      /* Get on which events the user wants to be notified by email (row[22]) */
      if (sscanf (row[22],"%u",&UsrDat->NtfEvents.SendEmail) != 1)
	 UsrDat->NtfEvents.SendEmail = 0;
      if (UsrDat->NtfEvents.SendEmail >= (1 << Ntf_NUM_NOTIFY_EVENTS))	// Maximum binary value for NotifyEvents is 000...0011...11
	 UsrDat->NtfEvents.SendEmail = 0;

      /***** Get user's settings *****/
      if (GetPrefs == Usr_GET_PREFS)
	{
	 /* Get language (row[23]),
		first day of week (row[24]),
		date format (row[25]),
		theme (row[26]),
		icon set (row[27]),
		menu (row[28]),
		if user wants to show side columns (row[29]),
		user settings on user photo shape (row[30]),
	    	and if user accepts third party cookies (row[31]) */
	 UsrDat->Prefs.Language	      = Lan_GetLanguageFromStr (row[23]);
	 UsrDat->Prefs.FirstDayOfWeek = Cal_GetFirstDayOfWeekFromStr (row[24]);
	 UsrDat->Prefs.DateFormat     = Dat_GetDateFormatFromStr (row[25]);
	 UsrDat->Prefs.Theme          = The_GetThemeFromStr (row[26]);
	 UsrDat->Prefs.IconSet        = Ico_GetIconSetFromStr (row[27]);
	 UsrDat->Prefs.Menu           = Mnu_GetMenuFromStr (row[28]);
	 UsrDat->Prefs.SideCols       = Set_GetSideColsFromStr (row[29]);
	 UsrDat->Prefs.PhotoShape     = PhoSha_GetShapeFromStr (row[30]);
	 UsrDat->Prefs.RefuseAcceptCookies = (row[31][0] == 'Y') ? Coo_ACCEPT :
								   Coo_REFUSE;
	}
     }
   else
      Err_WrongUserExit ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get nickname and email *****/
   Nck_DB_GetNicknameFromUsrCod (UsrDat->UsrCod,UsrDat->Nickname);
   Mai_GetEmailFromUsrCod (UsrDat);
  }

/*****************************************************************************/
/********* Get the comments in the record of a user from a string ************/
/*****************************************************************************/

static void Usr_GetUsrCommentsFromString (char *Str,struct Usr_Data *UsrDat)
  {
   /***** Check that memory for comments is allocated *****/
   if (UsrDat->Comments)
      /***** Copy comments from Str to Comments *****/
      Str_Copy (UsrDat->Comments,Str,Cns_MAX_BYTES_TEXT - 1);
  }

/*****************************************************************************/
/********** Get user's last data from database giving a user's code **********/
/*****************************************************************************/

static void Usr_GetMyLastData (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;
   long ActCod;

   /***** Get user's last data from database *****/
   if (Usr_DB_GetMyLastData (&mysql_res))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get last type of search (row[0]) */
      Gbl.Usrs.Me.UsrLast.WhatToSearch = Sch_SEARCH_UNKNOWN;
      if (sscanf (row[0],"%u",&UnsignedNum) == 1)
         if (UnsignedNum < Sch_NUM_WHAT_TO_SEARCH)
            Gbl.Usrs.Me.UsrLast.WhatToSearch = (Sch_WhatToSearch_t) UnsignedNum;
      if (Gbl.Usrs.Me.UsrLast.WhatToSearch == Sch_SEARCH_UNKNOWN)
	 Gbl.Usrs.Me.UsrLast.WhatToSearch = Sch_WHAT_TO_SEARCH_DEFAULT;

      /* Get last hierarchy: scope (row[1]) and code (row[2]) */
      Gbl.Usrs.Me.UsrLast.LastHie.Level = Hie_GetLevelFromDBStr (row[1]);
      switch (Gbl.Usrs.Me.UsrLast.LastHie.Level)
        {
         case Hie_SYS:	// System
            Gbl.Usrs.Me.UsrLast.LastHie.HieCod = -1L;
            break;
         case Hie_CTY:	// Country
         case Hie_INS:	// Institution
         case Hie_CTR:	// Center
         case Hie_DEG:	// Degree
         case Hie_CRS:	// Course
            Gbl.Usrs.Me.UsrLast.LastHie.HieCod = Str_ConvertStrCodToLongCod (row[2]);
            if (Gbl.Usrs.Me.UsrLast.LastHie.HieCod <= 0)
              {
               Gbl.Usrs.Me.UsrLast.LastHie.Level = Hie_UNK;
               Gbl.Usrs.Me.UsrLast.LastHie.HieCod = -1L;
              }
            break;
         default:
            Gbl.Usrs.Me.UsrLast.LastHie.Level = Hie_UNK;
            Gbl.Usrs.Me.UsrLast.LastHie.HieCod = -1L;
            break;
        }

      /* Get last action (row[3]) */
      ActCod = Str_ConvertStrCodToLongCod (row[3]);
      Gbl.Usrs.Me.UsrLast.LastAct = Act_GetActionFromActCod (ActCod);

      /* Get last role (row[4]) */
      Gbl.Usrs.Me.UsrLast.LastRole = Rol_ConvertUnsignedStrToRole (row[4]);

      /* Get last access to platform (row[5]) */
      Gbl.Usrs.Me.UsrLast.LastTime = 0L;
      if (row[5])
         sscanf (row[5],"%ld",&(Gbl.Usrs.Me.UsrLast.LastTime));

      /* Get last access to notifications (row[6]) */
      Gbl.Usrs.Me.UsrLast.LastAccNotif = 0L;
      if (row[6])
         sscanf (row[6],"%ld",&(Gbl.Usrs.Me.UsrLast.LastAccNotif));
     }
   else	// No user's last data found
     {
      /***** Create entry for me in table of user's last data *****/
      Usr_ResetMyLastData ();
      Usr_DB_InsertMyLastData ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************************** Get sex from string **************************/
/*****************************************************************************/

static Usr_Sex_t Usr_GetSexFromStr (const char *Str)
  {
   Usr_Sex_t Sex;

   for (Sex  = (Usr_Sex_t) 0;
        Sex <= (Usr_Sex_t) (Usr_NUM_SEXS - 1);
        Sex++)
      if (!strcasecmp (Str,Usr_StringsSexDB[Sex]))
         return Sex;

   return Usr_SEX_UNKNOWN;
  }

/*****************************************************************************/
/********** Build full name using FirstName, Surname1 and Surname2 ***********/
/*****************************************************************************/

void Usr_BuildFullName (struct Usr_Data *UsrDat)
  {
   Str_Copy (UsrDat->FullName,UsrDat->FrstName,sizeof (UsrDat->FullName) - 1);
   if (UsrDat->Surname1[0])
     {
      Str_Concat (UsrDat->FullName," "             ,sizeof (UsrDat->FullName) - 1);
      Str_Concat (UsrDat->FullName,UsrDat->Surname1,sizeof (UsrDat->FullName) - 1);
     }
   if (UsrDat->Surname2[0])
     {
      Str_Concat (UsrDat->FullName," "             ,sizeof (UsrDat->FullName) - 1);
      Str_Concat (UsrDat->FullName,UsrDat->Surname2,sizeof (UsrDat->FullName) - 1);
     }
  }

/*****************************************************************************/
/********* Write user name in two lines. 1: first name, 2: surnames **********/
/*****************************************************************************/

void Usr_WriteFirstNameBRSurnames (const struct Usr_Data *UsrDat)
  {
   /***** Write first name and surname 1 *****/
   HTM_Txt (UsrDat->FrstName);
   HTM_BR ();
   HTM_Txt (UsrDat->Surname1);

   /***** Write surname2 if exists *****/
   if (UsrDat->Surname2[0])
      HTM_SPTxt (UsrDat->Surname2);
  }

/*****************************************************************************/
/********************* Flush all caches related to users *********************/
/*****************************************************************************/

void Usr_FlushCachesUsr (void)
  {
   Hie_FlushCacheUsrBelongsTo (Hie_INS);
   Hie_FlushCacheUsrBelongsTo (Hie_CTR);
   Hie_FlushCacheUsrBelongsTo (Hie_DEG);
   Hie_FlushCacheUsrBelongsTo (Hie_CRS);
   Enr_FlushCacheUsrBelongsToCurrentCrs ();
   Enr_FlushCacheUsrHasAcceptedInCurrentCrs ();
   Enr_FlushCacheUsrSharesAnyOfMyCrs ();
   Rol_FlushCacheMyRoleInCurrentCrs ();
   Rol_FlushCacheRoleUsrInCrs ();
   Grp_FlushCacheUsrSharesAnyOfMyGrpsInCurrentCrs ();
   Grp_FlushCacheIBelongToGrp ();
   Fol_FlushCacheFollow ();
  }

/*****************************************************************************/
/********************* Check if a user is a superuser ************************/
/*****************************************************************************/

void Usr_FlushCacheUsrIsSuperuser (void)
  {
   Gbl.Cache.UsrIsSuperuser.Valid = false;
  }

bool Usr_CheckIfUsrIsSuperuser (long UsrCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (UsrCod <= 0)
      return false;

   /***** 2. Fast check: If cached... *****/
   if (Gbl.Cache.UsrIsSuperuser.Valid &&
       UsrCod == Gbl.Cache.UsrIsSuperuser.UsrCod)
      return Gbl.Cache.UsrIsSuperuser.IsSuperuser;

   /***** 3. Slow check: If not cached, get if a user is superuser from database *****/
   Gbl.Cache.UsrIsSuperuser.UsrCod = UsrCod;
   Gbl.Cache.UsrIsSuperuser.IsSuperuser = Adm_DB_CheckIfUsrIsSuperuser (UsrCod);
   Gbl.Cache.UsrIsSuperuser.Valid = true;
   return Gbl.Cache.UsrIsSuperuser.IsSuperuser;
  }

/*****************************************************************************/
/**************** Check if I can change another user's data ******************/
/*****************************************************************************/

bool Usr_ICanChangeOtherUsrData (const struct Usr_Data *UsrDat)
  {
   /***** I can change my data *****/
   if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)
      return true;

   /***** Check if I have permission to see another user's IDs *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
	 /* Check 1: I can change data of users who do not exist in database */
         if (UsrDat->UsrCod <= 0)	// User does not exist (when creating a new user)
            return true;

         /* Check 2: I change data of users without password */
         if (!UsrDat->Password[0])	// User has no password (never logged)
            return true;

         return false;
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
         return Usr_ICanEditOtherUsr (UsrDat);
      default:
	 return false;
     }
  }

/*****************************************************************************/
/***************** Check if I can edit another user's data *******************/
/*****************************************************************************/

bool Usr_ICanEditOtherUsr (const struct Usr_Data *UsrDat)
  {
   /***** I can edit me *****/
   if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)
      return true;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_DEG_ADM:
	 /* If I am an administrator of current degree,
	    I only can edit users from current degree who have accepted */
	 if (Hie_CheckIfUsrBelongsTo (Hie_DEG,UsrDat->UsrCod,
				      Gbl.Hierarchy.Node[Hie_DEG].HieCod,
				      true))	// count only accepted courses
	    // Degree admins can't edit superusers' data
	    if (!Usr_CheckIfUsrIsSuperuser (UsrDat->UsrCod))
	       return true;
	 return false;
      case Rol_CTR_ADM:
	 /* If I am an administrator of current center,
	    I only can edit from current center who have accepted */
	 if (Hie_CheckIfUsrBelongsTo (Hie_CTR,UsrDat->UsrCod,
				      Gbl.Hierarchy.Node[Hie_CTR].HieCod,
				      true))	// count only accepted courses
	    // Center admins can't edit superusers' data
	    if (!Usr_CheckIfUsrIsSuperuser (UsrDat->UsrCod))
	       return true;
	 return false;
      case Rol_INS_ADM:
	 /* If I am an administrator of current institution,
	    I only can edit from current institution who have accepted */
	 if (Hie_CheckIfUsrBelongsTo (Hie_INS,UsrDat->UsrCod,
				      Gbl.Hierarchy.Node[Hie_INS].HieCod,
				      true))	// count only accepted courses
	    // Institution admins can't edit superusers' data
	    if (!Usr_CheckIfUsrIsSuperuser (UsrDat->UsrCod))
	       return true;
	 return false;
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/************ Check if I can view the record card of a student ***************/
/*****************************************************************************/

bool Usr_CheckIfICanViewRecordStd (const struct Usr_Data *UsrDat)
  {
   /***** 1. Fast check: Am I logged? *****/
   if (!Gbl.Usrs.Me.Logged)
      return false;

   /***** 2. Fast check: Is it a valid user code? *****/
   if (UsrDat->UsrCod <= 0)
      return false;

   /***** 3. Fast check: Is it a course selected? *****/
   if (Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)
      return false;

   /***** 4. Fast check: Is he/she a student? *****/
   if (UsrDat->Roles.InCurrentCrs != Rol_STD)
      return false;

   /***** 5. Fast check: Am I a system admin? *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      return true;

   /***** 6. Fast check: Do I belong to the current course? *****/
   if (!Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS])
      return false;

   /***** 7. Fast check: It's me? *****/
   if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)
      return true;

   /***** 8. Fast / slow check: Does he/she belong to the current course? *****/
   if (!Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat))
      return false;

   /***** 9. Fast / slow check depending on roles *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
	 return Grp_CheckIfUsrSharesAnyOfMyGrpsInCurrentCrs (UsrDat);
      case Rol_TCH:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/************ Check if I can view the record card of a teacher ***************/
/*****************************************************************************/
// Teacher here is intended as:
// - a non-editing teacher
// - or a teacher

bool Usr_CheckIfICanViewRecordTch (struct Usr_Data *UsrDat)
  {
   /***** 1. Fast check: Am I logged? *****/
   if (!Gbl.Usrs.Me.Logged)
      return false;

   /***** 2. Fast check: Is it a valid user code? *****/
   if (UsrDat->UsrCod <= 0)
      return false;

   /***** 3. Fast check: Is it a course selected? *****/
   if (Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)
      return false;

   /***** 4. Fast check: Is he/she a non-editing teacher or a teacher? *****/
   return (UsrDat->Roles.InCurrentCrs == Rol_NET ||
           UsrDat->Roles.InCurrentCrs == Rol_TCH);
  }

/*****************************************************************************/
/********* Check if I can view test/exam/match result of another user ********/
/*****************************************************************************/

bool Usr_CheckIfICanViewTstExaMchResult (const struct Usr_Data *UsrDat)
  {
   /***** 1. Fast check: Am I logged? *****/
   if (!Gbl.Usrs.Me.Logged)
      return false;

   /***** 2. Fast check: Is it a valid user code? *****/
   if (UsrDat->UsrCod <= 0)
      return false;

   /***** 3. Fast check: Is it a course selected? *****/
   if (Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)
      return false;

   /***** 4. Fast check: Am I a system admin? *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      return true;

   /***** 5. Fast check: Do I belong to the current course? *****/
   if (!Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS])
      return false;

   /***** 6. Fast check: It's me? *****/
   if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)
      return true;

   /***** 7. Fast check: Does he/she belong to the current course? *****/
   if (!Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat))
      return false;

   /***** 8. Fast / slow check depending on roles *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
	 return Grp_CheckIfUsrSharesAnyOfMyGrpsInCurrentCrs (UsrDat);
      case Rol_TCH:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/********** Check if I can view assigments / works of another user ***********/
/*****************************************************************************/

bool Usr_CheckIfICanViewAsgWrk (const struct Usr_Data *UsrDat)
  {
   /***** 1. Fast check: Am I logged? *****/
   if (!Gbl.Usrs.Me.Logged)
      return false;

   /***** 2. Fast check: Is it a valid user code? *****/
   if (UsrDat->UsrCod <= 0)
      return false;

   /***** 3. Fast check: Is it a course selected? *****/
   if (Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)
      return false;

   /***** 4. Fast check: Does he/she belong to the current course? *****/
   // Only users beloging to course can have files in assignments/works
   if (!Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat))
      return false;

   /***** 5. Fast check: Am I a system admin? *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      return true;

   /***** 6. Fast check: Do I belong to the current course? *****/
   if (!Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS])
      return false;

   /***** 7. Fast check: It's me? *****/
   if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)
      return true;

   /***** 8. Fast / slow check depending on roles *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
	 return Grp_CheckIfUsrSharesAnyOfMyGrpsInCurrentCrs (UsrDat);
      case Rol_TCH:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/************** Check if I can view attendance of another user ***************/
/*****************************************************************************/

bool Usr_CheckIfICanViewAtt (const struct Usr_Data *UsrDat)
  {
   /***** 1. Fast check: Am I logged? *****/
   if (!Gbl.Usrs.Me.Logged)
      return false;

   /***** 2. Fast check: Is it a valid user code? *****/
   if (UsrDat->UsrCod <= 0)
      return false;

   /***** 3. Fast check: Is it a course selected? *****/
   if (Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)
      return false;

   /***** 4. Fast check: Am I a system admin? *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      return true;

   /***** 5. Fast check: Do I belong to the current course? *****/
   if (!Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS])
      return false;

   /***** 6. Fast check: It's me? *****/
   if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)
      return true;

   /***** 7. Fast / slow check depending on roles *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
	 return Grp_CheckIfUsrSharesAnyOfMyGrpsInCurrentCrs (UsrDat);
      case Rol_TCH:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/******************* Check if I can view a user's agenda *********************/
/*****************************************************************************/

bool Usr_CheckIfICanViewUsrAgenda (struct Usr_Data *UsrDat)
  {
   /***** 1. Fast check: Am I logged? *****/
   if (!Gbl.Usrs.Me.Logged)
      return false;

   /***** 2. Fast check: It's me? *****/
   if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)
      return true;

   /***** 3. Fast check: Am I logged as system admin? *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      return true;

   /***** 4. Slow check: Get if user shares any course with me from database *****/
   return Enr_CheckIfUsrSharesAnyOfMyCrs (UsrDat);
  }

/*****************************************************************************/
/***************************** Write landing page ****************************/
/*****************************************************************************/

void Usr_WriteLandingPage (void)
  {
   /***** Form to log in *****/
   Usr_WriteFormLogin (ActLogIn,NULL);

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

   /***** Show help to enrol me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/********************* Message and form shown after log out ******************/
/*****************************************************************************/

void Usr_Logout (void)
  {
   // extern const char *Txt_The_session_has_been_closed;

   /***** Confirmation message *****/
   // Ale_ShowFixedAlert (Ale_INFO,Txt_The_session_has_been_closed);

   /***** Form to log in *****/
   Usr_WriteFormLogin (ActLogIn,NULL);

   /***** Advertisement about mobile app *****/
   Lay_AdvertisementMobile ();
  }

/*****************************************************************************/
/*************************** Put link to log in ******************************/
/*****************************************************************************/

void Usr_PutLinkToLogin (void)
  {
   extern const char *Txt_Log_in;

   Lay_PutContextualLinkIconText (ActFrmLogIn,NULL,
                                  NULL,NULL,
				  "sign-in-alt.svg",Ico_GREEN,
				  Txt_Log_in,NULL);
  }

/*****************************************************************************/
/************************ Write form for user log in *************************/
/*****************************************************************************/

void Usr_WriteFormLogin (Act_Action_t NextAction,void (*FuncPars) (void))
  {
   extern const char *Hlp_PROFILE_LogIn;
   extern const char *Txt_Log_in;
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_nick_email_or_ID;
   extern const char *Txt_Password;
   extern const char *Txt_password;

   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
      Acc_PutLinkToCreateAccount ();	// Create account
      Pwd_PutLinkToSendNewPasswd ();	// Send new password
      Lan_PutLinkToChangeLanguage ();	// Change language
   Mnu_ContextMenuEnd ();

   HTM_DIV_Begin ("class=\"CM\"");

      /***** Begin form *****/
      Frm_BeginForm (NextAction);
	 if (FuncPars)
	    FuncPars ();

	 /***** Begin box and table *****/
	 Box_BoxTableBegin (Txt_Log_in,NULL,NULL,
			    Hlp_PROFILE_LogIn,Box_NOT_CLOSABLE,2);

	    /***** User's ID/nickname *****/
	    HTM_DIV_Begin ("class=\"LM\"");
	       HTM_LABEL_Begin ("for=\"UsrId\"");
		  Ico_PutIcon ("user.svg",Ico_BLACK,
		               Txt_User[Usr_SEX_UNKNOWN],"CONTEXT_ICO16x16");
	       HTM_LABEL_End ();
	       HTM_INPUT_TEXT ("UsrId",Cns_MAX_CHARS_EMAIL_ADDRESS,Gbl.Usrs.Me.UsrIdLogin,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "id=\"UsrId\" size=\"16\" placeholder=\"%s\""
			       " class=\"INPUT_%s\" autofocus=\"autofocus\""
			       " required=\"required\"",
			       Txt_nick_email_or_ID,
			       The_GetSuffix ());
	    HTM_DIV_End ();

	    /***** User's password *****/
	    HTM_DIV_Begin ("class=\"LM\"");
	       HTM_LABEL_Begin ("for=\"UsrPwd\"");
		  Ico_PutIcon ("key.svg",Ico_BLACK,
		               Txt_Password,"CONTEXT_ICO16x16");
	       HTM_LABEL_End ();
	       HTM_INPUT_PASSWORD ("UsrPwd",Txt_password,NULL,
				   false,	// Not required
				   "id=\"UsrPwd\" class=\"INPUT_%s\"",
				   The_GetSuffix ());
	    HTM_DIV_End ();

	 /***** End table, send button and end box *****/
	 Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Log_in);

      /***** End form *****/
      Frm_EndForm ();

   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************* Write type and name of logged user **********************/
/*****************************************************************************/

void Usr_WelcomeUsr (void)
  {
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   extern unsigned Txt_Current_CGI_SWAD_Language;
   extern const char *Txt_NEW_YEAR_GREETING;
   extern const char *Txt_Happy_birthday_X;
   extern const char *Txt_Please_check_your_email_address;
   extern const char *Txt_Check;
   extern const char *Txt_Switching_to_LANGUAGE[1 + Lan_NUM_LANGUAGES];
   char URLIconSet[PATH_MAX + 1];
   unsigned CurrentDay   = Dat_GetCurrentDay ();
   unsigned CurrentMonth = Dat_GetCurrentMonth ();
   unsigned CurrentYear  = Dat_GetCurrentYear ();

   if (Gbl.Usrs.Me.Logged)
     {
      if (Gbl.Usrs.Me.UsrDat.Prefs.Language == Txt_Current_CGI_SWAD_Language)
        {
         if (Gbl.Usrs.Me.UsrDat.FrstName[0])
           {
	    /***** New year greeting *****/
	    if (CurrentDay == 1 && CurrentMonth == 1)
	       Ale_ShowAlert (Ale_INFO,Txt_NEW_YEAR_GREETING,
			      CurrentYear);

            /***** Birthday congratulation *****/
            if (Gbl.Usrs.Me.UsrDat.Birthday.Day   == CurrentDay &&
                Gbl.Usrs.Me.UsrDat.Birthday.Month == CurrentMonth)
               if (Usr_DB_CheckIfMyBirthdayHasNotBeenCongratulated ())
                 {
                  /* Mark my birthday as already congratulated */
        	  Usr_DB_DeleteOldBirthdays ();
                  Usr_DB_MarkMyBirthdayAsCongratulated ();

		  /* Begin alert */
		  Ale_ShowAlertAndButtonBegin (Ale_INFO,Txt_Happy_birthday_X,
			                   Gbl.Usrs.Me.UsrDat.FrstName);

		  /* Show cake icon */
		  snprintf (URLIconSet,sizeof (URLIconSet),"%s/%s",
			    Cfg_URL_ICON_SETS_PUBLIC,Ico_IconSetId[Gbl.Prefs.IconSet]);
		  HTM_IMG (URLIconSet,"birthday-cake.svg",NULL,
			   "class=\"ICO160x160\"");

		  /* End alert */
		  Ale_ShowAlertAndButtonEnd (ActUnk,NULL,NULL,
		                           NULL,NULL,
		                           Btn_NO_BUTTON,NULL);
                 }

	    /***** Alert with button to check email address *****/
	    if ( Gbl.Usrs.Me.UsrDat.Email[0] &&
		!Gbl.Usrs.Me.UsrDat.EmailConfirmed)	// Email needs to be confirmed
	       Ale_ShowAlertAndButton (ActFrmMyAcc,NULL,NULL,
	                               NULL,NULL,
				       Btn_CONFIRM_BUTTON,Txt_Check,
				       Ale_WARNING,Txt_Please_check_your_email_address);
           }

         /***** Games tool *****/
         /*
         Ale_ShowAlert (Ale_INFO,
			"Herramienta <a href=\"https://github.com/acanas/swad-core/wiki/ASSESSMENT.Games.es\" target=\"_blank\">"
			"Evaluaci&oacute;n &gt; Juegos</a><br />"
			"<br />"
			"Imagen de la izquierda: pantalla proyectada en el aula por el profesor.<br />"
			"Imagen de la derecha: pantalla del estudiante en su m&oacute;vil."
			"<br />"
			"<br />"
	                "<img src=\"/img/juego.png\" alt=\"Juegos\" style=\"width:100%%\">");
	 */

         /***** Institutional video *****/
         /*
         Ale_ShowAlert (Ale_INFO,
			"<a href=\"https://abierta.ugr.es/creative_commons/\" target=\"_blank\">"
			"Curso MOOC LICENCIAS CREATIVE COMMONS Y OER</a><br />"
			"Reconocimiento de 1 cr&eacute;dito por actividades universitarias<br />"
			"&iexcl;&Uacute;ltimos d&iacute;as!<br />"
			"<br />"
			"<video style=\"width:480px; height:270px;\""
			" poster=\"/img/abierta-ugr-creative-commons1280x720.jpg\""
			" controls>"
			"<source src=\"https://abierta.ugr.es/creative_commons/promo_cc.mp4\""
			" type=\"video/mp4\">"
	                "<img src=\"/img/abierta-ugr-creative-commons1280x720.jpg\""
	                " class=\"img-responsive\""
	                " alt=\"Responsive image\">"
	                "</video>");
	 */

         /***** Show the global announcements I have not seen *****/
         Ann_ShowMyAnnouncementsNotMarkedAsSeen ();
        }
      else
	 /* The current language is not my preferred language
	    ==> change automatically to my language */
         Ale_ShowAlert (Ale_INFO,Txt_Switching_to_LANGUAGE[Gbl.Usrs.Me.UsrDat.Prefs.Language]);
     }
  }

/*****************************************************************************/
/************ Write birthday string to insert or update database *************/
/*****************************************************************************/
// It can include start and ending apostrophes

void Usr_CreateBirthdayStrDB (const struct Usr_Data *UsrDat,
                              char BirthdayStrDB[Usr_BIRTHDAY_STR_DB_LENGTH + 1])
  {
   char BirthdayStrTmp[1 + Cns_MAX_DECIMAL_DIGITS_UINT +
                       1 + Cns_MAX_DECIMAL_DIGITS_UINT +
                       1 + Cns_MAX_DECIMAL_DIGITS_UINT +
                       1 + 1];

   if (UsrDat->Birthday.Year  == 0 ||
       UsrDat->Birthday.Month == 0 ||
       UsrDat->Birthday.Day   == 0)
      Str_Copy (BirthdayStrDB,"NULL",Usr_BIRTHDAY_STR_DB_LENGTH);	// Without apostrophes
   else
     {
      sprintf (BirthdayStrTmp,"'%04u-%02u-%02u'",	// With apostrophes
	       UsrDat->Birthday.Year,
	       UsrDat->Birthday.Month,
	       UsrDat->Birthday.Day);
      Str_Copy (BirthdayStrDB,BirthdayStrTmp,Usr_BIRTHDAY_STR_DB_LENGTH);
     }
  }

/*****************************************************************************/
/************************* Filter some user's data ***************************/
/*****************************************************************************/

void Usr_FilterUsrBirthday (struct Dat_Date *Birthday)
  {
   unsigned CurrentYear = Dat_GetCurrentYear ();

   /***** Fix birthday *****/
   if (Birthday->Year < CurrentYear - 99 ||
       Birthday->Year > CurrentYear - 16)
      Birthday->Year  =
      Birthday->Month =
      Birthday->Day   = 0;
  }

/*****************************************************************************/
/************************ Write form for user log in *************************/
/*****************************************************************************/

void Usr_PutFormLogIn (void)
  {
   Frm_BeginForm (ActFrmLogIn);
      Ico_PutIconLink ("sign-in-alt.svg",Ico_WHITE,ActFrmLogIn);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************* Write type and name of logged user **********************/
/*****************************************************************************/

void Usr_WriteLoggedUsrHead (void)
  {
   extern const char *Txt_Role;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC18x24",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE18x24",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO18x24",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR18x24",
     };
   unsigned NumAvailableRoles = Rol_GetNumAvailableRoles ();
   char *ClassSelect;

   HTM_DIV_Begin ("class=\"HEAD_USR USR_%s\"",The_GetSuffix ());

      /***** User's role *****/
      if (NumAvailableRoles == 1)
	{
	 Frm_BeginForm (ActFrmRolSes);
	    HTM_BUTTON_Submit_Begin (Txt_Role,"class=\"BT_LINK\"");
	       HTM_Txt (Txt_ROLES_SINGUL_Abc[Gbl.Usrs.Me.Role.Logged][Gbl.Usrs.Me.UsrDat.Sex]);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

	 HTM_Colon ();
	}
      else
	{
	 if (asprintf (&ClassSelect,"SEL_ROLE INPUT_%s",
	               The_GetSuffix ()) < 0)
	    Err_NotEnoughMemoryExit ();
	 Rol_PutFormToChangeMyRole (ClassSelect);
	 free (ClassSelect);
	}
      HTM_NBSP ();

      /***** Show my photo *****/
      Pho_ShowUsrPhotoIfAllowed (&Gbl.Usrs.Me.UsrDat,
                                 ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);

      /***** User's name *****/
      if (Gbl.Usrs.Me.UsrDat.FrstName[0])
	 HTM_TxtF ("&nbsp;%s",Gbl.Usrs.Me.UsrDat.FrstName);

   HTM_DIV_End ();
  }

/*****************************************************************************/
/*************** Put a form to close current session (log out) ***************/
/*****************************************************************************/

void Usr_PutFormLogOut (void)
  {
   Frm_BeginForm (ActLogOut);
      Ico_PutIconLink ("sign-out-alt.svg",Ico_WHITE,ActLogOut);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******* Get parameter with my plain user's ID or nickname from a form *******/
/*****************************************************************************/

void Usr_GetParUsrIdLogin (void)
  {
   Par_GetParText ("UsrId",Gbl.Usrs.Me.UsrIdLogin,sizeof (Gbl.Usrs.Me.UsrIdLogin) - 1);
   // Users' IDs are always stored internally without leading zeros
   Str_RemoveLeadingZeros (Gbl.Usrs.Me.UsrIdLogin);
  }

/*****************************************************************************/
/******* Get parameter user's identificator of other user from a form ********/
/*****************************************************************************/

static void Usr_GetParOtherUsrIDNickOrEMail (void)
  {
   /***** Get parameter with the plain user's ID, @nick or email of another user *****/
   Par_GetParText ("OtherUsrIDNickOrEMail",
                     Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail,
                     sizeof (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail) - 1);

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
// Returns the number of users for a given ID, @nick or email

unsigned Usr_GetParOtherUsrIDNickOrEMailAndGetUsrCods (struct Usr_ListUsrCods *ListUsrCods)
  {
   extern const char *Txt_The_ID_nickname_or_email_X_is_not_valid;
   bool Wrong = false;

   /***** Reset default list of users' codes *****/
   ListUsrCods->NumUsrs = 0;
   ListUsrCods->Lst = NULL;

   /***** Get parameter with the plain user's ID, @nick or email of another user *****/
   Usr_GetParOtherUsrIDNickOrEMail ();

   /***** Check if it's an ID, a nickname or an email address *****/
   if (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail[0])
     {
      if (Nck_CheckIfNickWithArrIsValid (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail))	// 1: It's a nickname
	{
	 if ((Gbl.Usrs.Other.UsrDat.UsrCod = Nck_GetUsrCodFromNickname (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail)) > 0)
	   {
	    ListUsrCods->NumUsrs = 1;	// One user found
	    Usr_AllocateListUsrCods (ListUsrCods);
	    ListUsrCods->Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;
	   }
	}
      else if (Mai_CheckIfEmailIsValid (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail))		// 2: It's an email
	{
	 if ((Gbl.Usrs.Other.UsrDat.UsrCod = Mai_DB_GetUsrCodFromEmail (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail)) > 0)
	   {
	    ListUsrCods->NumUsrs = 1;	// One user found
	    Usr_AllocateListUsrCods (ListUsrCods);
	    ListUsrCods->Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;
	   }
	}
      else											// 3: It's not a nickname nor email
	{
	 // Users' IDs are always stored internally in capitals and without leading zeros
	 Str_RemoveLeadingZeros (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail);
	 if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail))
	   {
	    /* Allocate space for the list */
	    ID_ReallocateListIDs (&Gbl.Usrs.Other.UsrDat,1);

	    Str_Copy (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID,
	              Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail,
	              sizeof (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID) - 1);
	    Str_ConvertToUpperText (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);

	    /* Check if user's ID exists in database */
	    ID_GetListUsrCodsFromUsrID (&Gbl.Usrs.Other.UsrDat,NULL,ListUsrCods,false);
	   }
	 else	// Not a valid user's nickname, email or ID
	    Wrong = true;
	}
     }
   else	// Empty string
      Wrong = true;

   if (Wrong)
      /***** String is not a valid user's nickname, email or ID *****/
      Ale_ShowAlert (Ale_WARNING,Txt_The_ID_nickname_or_email_X_is_not_valid,
	             Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail);

   return ListUsrCods->NumUsrs;
  }

/*****************************************************************************/
/********* Put hidden parameter encrypted user's code of other user **********/
/*****************************************************************************/

void Usr_PutParMyUsrCodEncrypted (void *EncryptedUsrCod)
  {
   Usr_PutParUsrCodEncrypted ((const char *) EncryptedUsrCod);
  }

void Usr_PutParOtherUsrCodEncrypted (void *EncryptedUsrCod)
  {
   Usr_PutParUsrCodEncrypted ((const char *) EncryptedUsrCod);
  }

void Usr_PutParUsrCodEncrypted (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1])
  {
   Par_PutParString (NULL,"OtherUsrCod",EncryptedUsrCod);
  }

/*****************************************************************************/
/********* Get hidden parameter encrypted user's code of other user **********/
/*****************************************************************************/

void Usr_GetParOtherUsrCodEncrypted (struct Usr_Data *UsrDat)
  {
   Par_GetParText ("OtherUsrCod",UsrDat->EnUsrCod,
                     Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
   if (UsrDat->EnUsrCod[0])	// If parameter exists...
     {
      Usr_GetUsrCodFromEncryptedUsrCod (UsrDat);
      if (UsrDat->UsrCod <= 0)	// Check is user's code is valid
         Err_WrongUserExit ();
     }
   else
      UsrDat->UsrCod = -1L;
  }

/*****************************************************************************/
/********* Get hidden parameter encrypted user's code of other user **********/
/*****************************************************************************/

void Usr_GetParOtherUsrCodEncryptedAndGetListIDs (void)
  {
   Usr_GetParOtherUsrCodEncrypted (&Gbl.Usrs.Other.UsrDat);
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)	// If parameter exists...
      ID_GetListIDsFromUsrCod (&Gbl.Usrs.Other.UsrDat);
   else       					// Parameter does not exist
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

bool Usr_GetParOtherUsrCodEncryptedAndGetUsrData (void)
  {
   /***** Get parameter with encrypted user's code *****/
   Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();

   /***** Check if user exists and get her/his data *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                Usr_DONT_GET_PREFS,
                                                Usr_GET_ROLE_IN_CRS))
      // Existing user
      return true;

   return false;
  }

/*****************************************************************************/
/** Check and get user data from session, from internal or external login... */
/*****************************************************************************/

void Usr_ChkUsrAndGetUsrData (void)
  {
   extern const char *Txt_The_session_has_expired;
   struct
     {
      Frm_PutForm_t PutForm;
      Act_Action_t Action;
      void (*FuncPars) (void);
     } FormLogin =
     {
      Frm_DONT_PUT_FORM,
      ActLogIn,
      NULL
     };
   Act_Action_t Action;

   if (Gbl.Session.HasBeenDisconnected)
     {
      if (!Gbl.Action.UsesAJAX)
	{
	 Gbl.Action.Act = ActLogOut;
	 Tab_SetCurrentTab ();
	 Ale_ShowAlert (Ale_WARNING,Txt_The_session_has_expired);
	 FormLogin.PutForm = Frm_PUT_FORM;
	}
     }
   else	// !Gbl.Session.HasBeenDisconnected
     {
      /***** Check user and get user's data *****/
      if (Gbl.Action.Act == ActCreUsrAcc)
	{
	 /***** Create my new account and login *****/
	 if (Acc_CreateMyNewAccountAndLogIn ())		// User logged in
	   {
	    Gbl.Usrs.Me.Logged = true;
	    Usr_SetMyPrefsAndRoles ();

	    Act_AdjustCurrentAction ();
	    Ses_CreateSession ();

	    /* Set settings from current IP */
	    Set_SetSettingsFromIP ();

	    /* Send message via email to confirm the new email address */
	    Mai_SendMailMsgToConfirmEmail ();
	    Ale_ShowAlerts (NULL);	// Show alert after sending email confirmation
	   }
	}
      else	// Gbl.Action.Act != ActCreUsrAcc
	{
	 /***** Check user and get user's data *****/
	 if (Gbl.Session.IsOpen)
	   {
	    if (Usr_ChkUsrAndGetUsrDataFromSession ())	// User logged in
	      {
	       Gbl.Usrs.Me.Logged = true;

	       Usr_SetMyPrefsAndRoles ();

	       if (Gbl.Action.IsAJAXAutoRefresh)	// If refreshing ==> don't refresh LastTime in session
		  Ses_DB_UpdateSessionLastRefresh ();
	       else
		 {
		  Act_AdjustCurrentAction ();
		  Ses_DB_UpdateSession ();
		  Con_DB_UpdateMeInConnectedList ();
		 }
	      }
	    else
	       FormLogin.PutForm = Frm_PUT_FORM;
	   }
	 else if (Gbl.Action.Act == ActLogIn ||
	          Gbl.Action.Act == ActLogInUsrAgd)	// Login using @nickname, email or ID from form
	   {
	    if (Usr_ChkUsrAndGetUsrDataFromDirectLogin ())	// User logged in
	      {
	       Gbl.Usrs.Me.Logged = true;
	       Usr_SetMyPrefsAndRoles ();

	       Act_AdjustCurrentAction ();
	       Ses_CreateSession ();

	       Set_SetSettingsFromIP ();	// Set settings from current IP
	      }
	    else
	      {
	       FormLogin.PutForm = Frm_PUT_FORM;
	       if (Gbl.Action.Act == ActLogInUsrAgd)
		 {
	          FormLogin.Action = ActLogInUsrAgd;
	          FormLogin.FuncPars = Agd_PutParAgd;
		 }
	      }
	   }
	 else if (Gbl.Action.Act == ActLogInNew)	// Empty account without password, login using encrypted user's code
	   {
	    /***** Get user's data *****/
	    Usr_GetParOtherUsrCodEncrypted (&Gbl.Usrs.Me.UsrDat);
            Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Me.UsrDat);
            if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Me.UsrDat,
                                                         Usr_GET_PREFS,
                                                         Usr_GET_ROLE_IN_CRS))
	      {
               // User logged in
	       Gbl.Usrs.Me.Logged = true;
	       Usr_SetMyPrefsAndRoles ();

	       Act_AdjustCurrentAction ();
	       Ses_CreateSession ();

	       Set_SetSettingsFromIP ();	// Set settings from current IP
	      }
	    else
	       FormLogin.PutForm = Frm_PUT_FORM;
	   }
	}
     }

   /***** If session disconnected or error in login, show form to login *****/
   if (FormLogin.PutForm == Frm_PUT_FORM)
     {
      Usr_WriteFormLogin (FormLogin.Action,FormLogin.FuncPars);
      Err_ShowErrorAndExit (NULL);
     }

   /***** Adjust tab and action *****/
   if (!Gbl.Action.UsesAJAX)
     {
      if (!Gbl.Usrs.Me.Logged &&	// No user logged...
	  Gbl.Action.Act == ActUnk)	// ...and unknown action
	 Act_AdjustActionWhenNoUsrLogged ();

      /***** When I change to another tab, go to:
             - my last action in that tab if it is known, or
             - the first option allowed *****/
      if (Gbl.Action.Act == ActMnu)
	{
	 /* Get my last action in current tab */
	 Action = (Gbl.Usrs.Me.Logged) ? MFU_GetMyLastActionInCurrentTab () :
	                                 ActUnk;
	 if (Action == ActUnk)
	    /* Get the first option allowed */
	    Action = Mnu_GetFirstActionAvailableInCurrentTab ();

	 Gbl.Action.Act = (Action == ActUnk) ? ((Gbl.Usrs.Me.Logged) ? ActSeeGblTL :	// Default action if logged
								       ActFrmLogIn) :	// Default action if not logged
					       Action;
	 Tab_SetCurrentTab ();
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
   struct Usr_ListUsrCods ListUsrCods;
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

   /***** Check if user has typed his user's ID, his nickname or his email address *****/
   if (Nck_CheckIfNickWithArrIsValid (Gbl.Usrs.Me.UsrIdLogin))	// 1: It's a nickname
     {
      // User is trying to log using his/her nickname
      if ((Gbl.Usrs.Me.UsrDat.UsrCod = Nck_GetUsrCodFromNickname (Gbl.Usrs.Me.UsrIdLogin)) <= 0)
	{
	 Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
	 return false;
	}
     }
   else if (Mai_CheckIfEmailIsValid (Gbl.Usrs.Me.UsrIdLogin))		// 2: It's an email
     {
      // User is trying to log using his/her email
      if ((Gbl.Usrs.Me.UsrDat.UsrCod = Mai_DB_GetUsrCodFromEmail (Gbl.Usrs.Me.UsrIdLogin)) <= 0)
	{
	 Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
	 return false;
	}
     }
   else									// 3: It's not a nickname nor email
     {
      // Users' IDs are always stored internally in capitals and without leading zeros
      Str_RemoveLeadingZeros (Gbl.Usrs.Me.UsrIdLogin);
      if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Me.UsrIdLogin))
	{
	 /***** Allocate space for the list *****/
	 ID_ReallocateListIDs (&Gbl.Usrs.Me.UsrDat,1);

	 Str_Copy (Gbl.Usrs.Me.UsrDat.IDs.List[0].ID,Gbl.Usrs.Me.UsrIdLogin,
	           sizeof (Gbl.Usrs.Me.UsrDat.IDs.List[0].ID) - 1);
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
      else	// String is not a valid user's nickname, email or ID
	{
	 Usr_ShowAlertUsrDoesNotExistsOrWrongPassword ();
	 return false;
	}
     }

   /***** Get user's data *****/
   Usr_GetAllUsrDataFromUsrCod (&Gbl.Usrs.Me.UsrDat,
                                Usr_GET_PREFS,
                                Usr_GET_ROLE_IN_CRS);

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
/******** Check user and get user's data when the session is open ************/
/*****************************************************************************/

static bool Usr_ChkUsrAndGetUsrDataFromSession (void)
  {
   /***** Session is open and user's code is get from session *****/
   Gbl.Usrs.Me.UsrDat.UsrCod = Gbl.Session.UsrCod;

   /* Check if user exists in database, and get his/her data */
   if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Me.UsrDat,
                                                 Usr_GET_PREFS,
                                                 Usr_GET_ROLE_IN_CRS))
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

   Ale_ShowAlert (Ale_WARNING,Txt_The_user_does_not_exist_or_password_is_incorrect);
  }

/*****************************************************************************/
/****** Show alert indicating that more than one user share the same ID ******/
/*****************************************************************************/

static void Usr_ShowAlertThereAreMoreThanOneUsr (void)
  {
   extern const char *Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email;

   Gbl.Action.Act = ActFrmLogIn;
   Tab_SetCurrentTab ();
   Ale_ShowAlert (Ale_WARNING,Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email,
	          Gbl.Usrs.Me.UsrIdLogin);
  }

/*****************************************************************************/
/********************** Set my settings and my roles *************************/
/*****************************************************************************/

static void Usr_SetMyPrefsAndRoles (void)
  {
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   struct Sch_Search *Search;
   bool GetRoleAndActionFromLastData;
   Act_Action_t LastSuperAction;
   bool JustAfterLogin = Gbl.Action.Act == ActLogIn    ||
	                 Gbl.Action.Act == ActLogInLan ||
	                 Gbl.Action.Act == ActLogInNew ||
			 Gbl.Action.Act == ActAnnSee;

   // In this point I am logged

   /***** Set my language if unknown *****/
   if (Gbl.Usrs.Me.UsrDat.Prefs.Language == Lan_LANGUAGE_UNKNOWN)		// I have not chosen language
      Lan_UpdateMyLanguageToCurrentLanguage ();	// Update my language in database

   /***** Set settings from my settings *****/
   Gbl.Prefs.FirstDayOfWeek = Gbl.Usrs.Me.UsrDat.Prefs.FirstDayOfWeek;
   Gbl.Prefs.DateFormat     = Gbl.Usrs.Me.UsrDat.Prefs.DateFormat;
   Gbl.Prefs.IconSet        = Gbl.Usrs.Me.UsrDat.Prefs.IconSet;
   Gbl.Prefs.Menu           = Gbl.Usrs.Me.UsrDat.Prefs.Menu;
   Gbl.Prefs.Theme          = Gbl.Usrs.Me.UsrDat.Prefs.Theme;
   Gbl.Prefs.SideCols       = Gbl.Usrs.Me.UsrDat.Prefs.SideCols;
   Gbl.Prefs.PhotoShape     = Gbl.Usrs.Me.UsrDat.Prefs.PhotoShape;

   /***** Construct the path to my directory *****/
   Usr_ConstructPathUsr (Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Usrs.Me.PathDir);

   /***** Check if my photo exists and create a link to it ****/
   Gbl.Usrs.Me.MyPhotoExists = Pho_BuildLinkToPhoto (&Gbl.Usrs.Me.UsrDat,Gbl.Usrs.Me.PhotoURL);

   /***** Get my last data *****/
   Usr_GetMyLastData ();
   if (JustAfterLogin)	// If I just logged in...
     {
      /***** WhatToSearch is stored in session,
             but in login it is got from user's last data *****/
      Search = Sch_GetSearch ();
      Search->WhatToSearch = Gbl.Usrs.Me.UsrLast.WhatToSearch;

      /***** Location in hierarchy and role are stored in session,
             but in login the are got from user's last data *****/
      if (Gbl.Hierarchy.Level == Hie_SYS)	// No country selected
        {
	 /***** Copy last hierarchy to current hierarchy *****/
	 Hie_SetHierarchyFromUsrLastHierarchy ();

	 /* Course may have changed ==> get my role in current course again */
	 if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
	    Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs = Rol_GetMyRoleInCrs (Gbl.Hierarchy.Node[Hie_CRS].HieCod);

	 // role and action will be got from last data
         GetRoleAndActionFromLastData = true;
        }
      else	// Country (and may be institution, center, degree or course) selected
	 // Role and action will be got from last data
	 // only if I am in the same hierarchy location that the stored one
	 GetRoleAndActionFromLastData =
	    (Gbl.Hierarchy.Level == Gbl.Usrs.Me.UsrLast.LastHie.Level &&	// The same scope...
	     Gbl.Hierarchy.Node[Gbl.Hierarchy.Level].HieCod ==
	     Gbl.Usrs.Me.UsrLast.LastHie.HieCod);				// ...and code in hierarchy

      /***** Get role and action from last data *****/
      if (GetRoleAndActionFromLastData)
        {
	 /* Get role from last data */
	 Gbl.Usrs.Me.Role.Logged = Gbl.Usrs.Me.UsrLast.LastRole;

	 /* Last action is really got only if last access is recent */
	 if (Gbl.Usrs.Me.UsrLast.LastTime >= Dat_GetStartExecutionTimeUTC () -
	                                     Cfg_MAX_TIME_TO_REMEMBER_LAST_ACTION_ON_LOGIN)
	   {
	    /* Get action from last data */
	    LastSuperAction = Act_GetSuperAction (Gbl.Usrs.Me.UsrLast.LastAct);
	    if (LastSuperAction != ActUnk)
	      {
	       Gbl.Action.Act = LastSuperAction;
	       Tab_SetCurrentTab ();
	      }
	   }
	 /* If action is not set to last action,
	    it will be set later to a default action */
        }
     }

   /***** Set my roles *****/
   Rol_SetMyRoles ();
  }

/*****************************************************************************/
/************** Show forms to log out and to change my role ******************/
/*****************************************************************************/

void Usr_ShowFormsLogoutAndRole (void)
  {
   extern const char *Hlp_PROFILE_Session_role;
   extern const char *Txt_Session;
   extern const char *Txt_Role;
   extern const char *Txt_You_are_now_LOGGED_IN_as_X;
   extern const char *Txt_logged[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char *ClassSelect;

   /***** Write message with my new logged role *****/
   if (Gbl.Usrs.Me.Role.HasChanged)
      Ale_ShowAlert (Ale_SUCCESS,Txt_You_are_now_LOGGED_IN_as_X,
	             Txt_logged[Gbl.Usrs.Me.UsrDat.Sex],
	             Txt_ROLES_SINGUL_abc[Gbl.Usrs.Me.Role.Logged][Gbl.Usrs.Me.UsrDat.Sex]);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Session,Usr_PutLinkToLogOut,NULL,
                 Hlp_PROFILE_Session_role,Box_NOT_CLOSABLE);

      /***** Put a form to change my role *****/
      if (Rol_GetNumAvailableRoles () == 1)
	{
	 HTM_SPAN_Begin ("class=\"DAT_%s\"",The_GetSuffix ());
	    HTM_TxtColonNBSP (Txt_Role);
	 HTM_SPAN_End ();

	 HTM_SPAN_Begin ("class=\"DAT_STRONG_%s BOLD\"",
	                 The_GetSuffix ());
	    HTM_Txt (Txt_ROLES_SINGUL_Abc[Gbl.Usrs.Me.Role.Logged][Gbl.Usrs.Me.UsrDat.Sex]);
	 HTM_SPAN_End ();
	}
      else
	{
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_TxtColonNBSP (Txt_Role);
	    if (asprintf (&ClassSelect,"INPUT_%s",
			  The_GetSuffix ()) < 0)
	       Err_NotEnoughMemoryExit ();
	    Rol_PutFormToChangeMyRole (ClassSelect);
	    free (ClassSelect);
	 HTM_LABEL_End ();
	}

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************** Put an icon (form) to close the current session **************/
/*****************************************************************************/

static void Usr_PutLinkToLogOut (__attribute__((unused)) void *Args)
  {
   Lay_PutContextualLinkOnlyIcon (ActLogOut,NULL,
				  NULL,NULL,
				  "sign-out-alt.svg",Ico_RED);
  }

/*****************************************************************************/
/******* Check a user's code and get all user's data from user's code ********/
/*****************************************************************************/
// Input: UsrDat->UsrCod must hold a valid user code
// Output: When true ==> UsrDat will hold all user's data
//         When false ==> UsrDat is reset, except user's code

bool Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (struct Usr_Data *UsrDat,
                                              Usr_GetPrefs_t GetPrefs,
                                              Usr_GetRoleInCurrentCrs_t GetRoleInCurrentCrs)
  {
   /***** Check if a user exists having this user's code *****/
   if (Usr_DB_ChkIfUsrCodExists (UsrDat->UsrCod))
     {
      /* Get user's data */
      Usr_GetAllUsrDataFromUsrCod (UsrDat,GetPrefs,GetRoleInCurrentCrs);
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
   /***** Check if it exists an entry for me *****/
   if (Usr_DB_CheckMyLastData ())
      /***** Update my last accessed course, tab and time of click in database *****/
      // WhatToSearch, LastAccNotif remain unchanged
      Usr_DB_UpdateMyLastData ();
   else
      Usr_DB_InsertMyLastData ();
  }

/*****************************************************************************/
/*********** Write a row of a table with the main data of a user *************/
/*****************************************************************************/

#define Usr_MAX_BYTES_BG_COLOR (16 - 1)

void Usr_WriteRowUsrMainData (unsigned NumUsr,struct Usr_Data *UsrDat,
                              bool PutCheckBoxToSelectUsr,Rol_Role_t Role,
			      struct Usr_SelectedUsrs *SelectedUsrs)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_Enrolment_confirmed;
   extern const char *Txt_Enrolment_not_confirmed;
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   char BgColor[Usr_MAX_BYTES_BG_COLOR + 1];
   bool UsrIsTheMsgSender = PutCheckBoxToSelectUsr &&
	                    (UsrDat->UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);
   struct Hie_Node Ins;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Checkbox to select user *****/
      // Two colors are used alternatively to better distinguish the rows
      if (UsrIsTheMsgSender)
	 Str_Copy (BgColor,"LIGHT_GREEN",sizeof (BgColor) - 1);
      else
	 snprintf (BgColor,sizeof (BgColor),"%s",The_GetColorRows ());

      if (PutCheckBoxToSelectUsr)
	{
	 HTM_TD_Begin ("class=\"CM %s\"",BgColor);
	    Usr_PutCheckboxToSelectUser (Role,UsrDat->EnUsrCod,UsrIsTheMsgSender,
					 SelectedUsrs);
	 HTM_TD_End ();
	}

      /***** User has accepted enrolment? *****/
      if (UsrIsTheMsgSender)
	 HTM_TD_Begin ("class=\"BM_SEL %s_%s\" title=\"%s\"",
		       UsrDat->Accepted ? "USR_LIST_NUM_N" :
					  "USR_LIST_NUM",
		       The_GetSuffix (),
		       UsrDat->Accepted ? Txt_Enrolment_confirmed :
					  Txt_Enrolment_not_confirmed);
      else
	 HTM_TD_Begin ("class=\"BM %s_%s %s\" title=\"%s\"",
		       UsrDat->Accepted ? "USR_LIST_NUM_N" :
					  "USR_LIST_NUM",
		       The_GetSuffix (),
		       BgColor,
		       UsrDat->Accepted ? Txt_Enrolment_confirmed :
					  Txt_Enrolment_not_confirmed);
      HTM_Txt (UsrDat->Accepted ? "&check;" :
				  "&cross;");
      HTM_TD_End ();

      /***** Write number of user in the list *****/
      HTM_TD_Begin ("class=\"%s_%s RM %s\"",
		    UsrDat->Accepted ? "USR_LIST_NUM_N" :
				       "USR_LIST_NUM",
		    The_GetSuffix (),
		    BgColor);
	 HTM_Unsigned (NumUsr);
      HTM_TD_End ();

      /***** Show user's photo *****/
      if (Gbl.Usrs.Listing.WithPhotos)
	{
	 HTM_TD_Begin ("class=\"CM %s\"",BgColor);
	    Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                               ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
	 HTM_TD_End ();
	}

      /****** Write user's IDs ******/
      HTM_TD_Begin ("class=\"LM %s_%s %s\"",
		    UsrDat->Accepted ? "DAT_SMALL_STRONG" :
			               "DAT_SMALL",
		    The_GetSuffix (),
		    BgColor);
	 ID_WriteUsrIDs (UsrDat,NULL);
      HTM_TD_End ();

      /***** Write rest of main user's data *****/
      Ins.HieCod = UsrDat->InsCod;
      Hie_GetDataByCod[Hie_INS] (&Ins);
      Usr_WriteMainUsrDataExceptUsrID (UsrDat,BgColor);

      HTM_TD_Begin ("class=\"LM %s\"",BgColor);
	 Ins_DrawInstitutionLogoWithLink (&Ins,"ICO25x25");
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*************** Write a row of a table with the data of a guest *************/
/*****************************************************************************/

static void Usr_WriteRowGstAllData (struct Usr_Data *UsrDat)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   struct Hie_Node Ins;
   struct Hie_Node Ctr;
   struct Dpt_Department Dpt;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      if (Gbl.Usrs.Listing.WithPhotos)
	{
	 /***** Show guest's photo *****/
	 HTM_TD_Begin ("class=\"%s LM\"",The_GetColorRows ());
	    Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                               ClassPhoto[Gbl.Prefs.PhotoShape],Pho_NO_ZOOM);
	 HTM_TD_End ();
	}

      /****** Write user's ID ******/
      HTM_TD_Begin ("class=\"LM DAT_SMALL_%s %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 ID_WriteUsrIDs (UsrDat,NULL);
	 HTM_NBSP ();
      HTM_TD_End ();

      /***** Write rest of guest's main data *****/
      Ins.HieCod = UsrDat->InsCod;
      Hie_GetDataByCod[Hie_INS] (&Ins);
      Usr_WriteMainUsrDataExceptUsrID (UsrDat,The_GetColorRows ());
      Usr_WriteEmail (UsrDat,The_GetColorRows ());
      Usr_WriteUsrData (The_GetColorRows (),
			Ins.FullName,
			NULL,true,false);

      /***** Write the rest of the data of the guest *****/
      if (UsrDat->Tch.CtrCod > 0)
	{
	 Ctr.HieCod = UsrDat->Tch.CtrCod;
	 Hie_GetDataByCod[Hie_CTR] (&Ctr);
	}
      Usr_WriteUsrData (The_GetColorRows (),
			UsrDat->Tch.CtrCod > 0 ? Ctr.FullName :
						 "&nbsp;",
			NULL,true,false);
      if (UsrDat->Tch.DptCod > 0)
	{
	 Dpt.DptCod = UsrDat->Tch.DptCod;
	 Dpt_GetDepartmentDataByCod (&Dpt);
	}
      Usr_WriteUsrData (The_GetColorRows (),
			UsrDat->Tch.DptCod > 0 ? Dpt.FullName :
						 "&nbsp;",
			NULL,true,false);
      Usr_WriteUsrData (The_GetColorRows (),
			UsrDat->Tch.Office[0] ? UsrDat->Tch.Office :
						"&nbsp;",
			NULL,true,false);
      Usr_WriteUsrData (The_GetColorRows (),
			UsrDat->Tch.OfficePhone[0] ? UsrDat->Tch.OfficePhone :
						     "&nbsp;",
			NULL,true,false);
      Usr_WriteUsrData (The_GetColorRows (),
			UsrDat->Phone[0][0] ? UsrDat->Phone[0] :
					      "&nbsp;",
			NULL,true,false);
      Usr_WriteUsrData (The_GetColorRows (),
			UsrDat->Phone[1][0] ? UsrDat->Phone[1] :
					      "&nbsp;",
			NULL,true,false);
      Usr_WriteUsrData (The_GetColorRows (),
			UsrDat->StrBirthday[0] ? UsrDat->StrBirthday :
						 "&nbsp;",
			NULL,true,false);

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/************ Write a row of a table with the data of a student **************/
/*****************************************************************************/

static void Usr_WriteRowStdAllData (struct Usr_Data *UsrDat,char *GroupNames)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   unsigned NumGrpTyp,NumField;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char Text[Cns_MAX_BYTES_TEXT + 1];
   struct Hie_Node Ins;
   bool ShowData = (Gbl.Usrs.Me.Role.Logged == Rol_TCH && UsrDat->Accepted) ||
                    Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      if (Gbl.Usrs.Listing.WithPhotos)
	{
	 /***** Show student's photo *****/
	 HTM_TD_Begin ("class=\"LM %s\"",The_GetColorRows ());
	    Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                               ClassPhoto[Gbl.Prefs.PhotoShape],Pho_NO_ZOOM);
	 HTM_TD_End ();
	}

      /****** Write user's ID ******/
      HTM_TD_Begin ("class=\"LM %s_%s %s\"",
		    UsrDat->Accepted ? "DAT_SMALL_STRONG" :
			               "DAT_SMALL",
		    The_GetSuffix (),
		    The_GetColorRows ());
	 ID_WriteUsrIDs (UsrDat,NULL);
	 HTM_NBSP ();
      HTM_TD_End ();

      /***** Write rest of main student's data *****/
      Ins.HieCod = UsrDat->InsCod;
      Hie_GetDataByCod[Hie_INS] (&Ins);
      Usr_WriteMainUsrDataExceptUsrID (UsrDat,The_GetColorRows ());
      Usr_WriteEmail (UsrDat,The_GetColorRows ());
      Usr_WriteUsrData (The_GetColorRows (),
			Ins.FullName,
			NULL,true,UsrDat->Accepted);

      /***** Write the rest of the data of the student *****/
      Usr_WriteUsrData (The_GetColorRows (),
			UsrDat->Phone[0][0] ? (ShowData ? UsrDat->Phone[0] :
							  "********") :
					      "&nbsp;",
			NULL,true,UsrDat->Accepted);
      Usr_WriteUsrData (The_GetColorRows (),
			UsrDat->Phone[1][0] ? (ShowData ? UsrDat->Phone[1] :
							  "********") :
					      "&nbsp;",
			NULL,true,UsrDat->Accepted);
      Usr_WriteUsrData (The_GetColorRows (),
			UsrDat->StrBirthday[0] ? (ShowData ? UsrDat->StrBirthday :
							     "********") :
						 "&nbsp;",
			NULL,true,UsrDat->Accepted);

      if (Gbl.Scope.Current == Hie_CRS)
	{
	 /***** Write the groups a the que pertenece the student *****/
	 for (NumGrpTyp = 0;
	      NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	      NumGrpTyp++)
	    if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)         // If current course tiene groups of este type
	      {
	       Grp_GetNamesGrpsUsrBelongsTo (UsrDat->UsrCod,
					     Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod,
					     GroupNames);
	       Usr_WriteUsrData (The_GetColorRows (),GroupNames,NULL,true,UsrDat->Accepted);
	      }

	 /***** Fields of the record dependientes of the course *****/
	 for (NumField = 0;
	      NumField < Gbl.Crs.Records.LstFields.Num;
	      NumField++)
	   {
	    /* Get the text of the field */
	    if (Rec_DB_GetFieldTxtFromUsrRecord (&mysql_res,
					         Gbl.Crs.Records.LstFields.Lst[NumField].FieldCod,
					         UsrDat->UsrCod))
	      {
	       row = mysql_fetch_row (mysql_res);
	       Str_Copy (Text,row[0],sizeof (Text) - 1);
	       Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				 Text,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
	      }
	    else
	       Text[0] = '\0';
	    Usr_WriteUsrData (The_GetColorRows (),Text,NULL,false,UsrDat->Accepted);

	    /* Free structure that stores the query result */
	    DB_FreeMySQLResult (&mysql_res);
	   }
	}

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*** Write a row of a table with the data of a teacher or an administrator ***/
/*****************************************************************************/

static void Usr_WriteRowTchAllData (struct Usr_Data *UsrDat)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   struct Hie_Node Ins;
   struct Hie_Node Ctr;
   struct Dpt_Department Dpt;
   bool ShowData = (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME || UsrDat->Accepted ||
                    Gbl.Usrs.Me.Role.Logged == Rol_DEG_ADM ||
                    Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);

   /***** Begin row *****/
   HTM_TR_Begin (NULL);
      if (Gbl.Usrs.Listing.WithPhotos)
	{
	 /***** Show teacher's photo *****/
	 HTM_TD_Begin ("class=\"LM %s\"",The_GetColorRows ());
	    Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                               ClassPhoto[Gbl.Prefs.PhotoShape],Pho_NO_ZOOM);
	 HTM_TD_End ();
	}

      /****** Write the user's ID ******/
      HTM_TD_Begin ("class=\"LM %s_%s %s\"",
		    UsrDat->Accepted ? "DAT_SMALL_STRONG" :
			               "DAT_SMALL",
		    The_GetSuffix (),
		    The_GetColorRows ());
	 ID_WriteUsrIDs (UsrDat,NULL);
	 HTM_NBSP ();
      HTM_TD_End ();

      /***** Write rest of main teacher's data *****/
      Ins.HieCod = UsrDat->InsCod;
      Hie_GetDataByCod[Hie_INS] (&Ins);
      Usr_WriteMainUsrDataExceptUsrID (UsrDat,The_GetColorRows ());
      Usr_WriteEmail (UsrDat,The_GetColorRows ());
      Usr_WriteUsrData (The_GetColorRows (),
			Ins.FullName,
			NULL,true,UsrDat->Accepted);

      /***** Write the rest of teacher's data *****/
      if (ShowData && UsrDat->Tch.CtrCod > 0)
	{
	 Ctr.HieCod = UsrDat->Tch.CtrCod;
	 Hie_GetDataByCod[Hie_CTR] (&Ctr);
	}
      Usr_WriteUsrData (The_GetColorRows (),
			(ShowData && UsrDat->Tch.CtrCod > 0) ? Ctr.FullName :
							       "&nbsp;",
			NULL,true,UsrDat->Accepted);
      if (ShowData && UsrDat->Tch.DptCod > 0)
	{
	 Dpt.DptCod = UsrDat->Tch.DptCod;
	 Dpt_GetDepartmentDataByCod (&Dpt);
	}
      Usr_WriteUsrData (The_GetColorRows (),
			(ShowData && UsrDat->Tch.DptCod > 0) ? Dpt.FullName :
							       "&nbsp;",
			NULL,true,UsrDat->Accepted);
      Usr_WriteUsrData (The_GetColorRows (),
			(ShowData && UsrDat->Tch.Office[0]) ? UsrDat->Tch.Office :
							      "&nbsp;",
			NULL,true,UsrDat->Accepted);
      Usr_WriteUsrData (The_GetColorRows (),
			(ShowData && UsrDat->Tch.OfficePhone[0]) ? UsrDat->Tch.OfficePhone :
								   "&nbsp;",
			NULL,true,UsrDat->Accepted);

   HTM_TR_End ();
  }

/*****************************************************************************/
/********** Write a row of a table with the data of an administrator *********/
/*****************************************************************************/

static void Usr_WriteRowAdmData (unsigned NumUsr,struct Usr_Data *UsrDat)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   struct Hie_Node Ins;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Write number of user *****/
      HTM_TD_Begin ("class=\"CM USR_LIST_NUM_N_%s %s\"",
		    The_GetSuffix (),
                    The_GetColorRows ());
	 HTM_Unsigned (NumUsr);
      HTM_TD_End ();

      if (Gbl.Usrs.Listing.WithPhotos)
	{
	 /***** Show administrator's photo *****/
	 HTM_TD_Begin ("class=\"LM %s\"",The_GetColorRows ());
	    Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                               ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
	 HTM_TD_End ();
	}

      /****** Write the user's ID ******/
      HTM_TD_Begin ("class=\"LM %s_%s %s\"",
		    UsrDat->Accepted ? "DAT_SMALL_STRONG" :
			               "DAT_SMALL",
		    The_GetSuffix (),
		    The_GetColorRows ());
	 ID_WriteUsrIDs (UsrDat,NULL);
	 HTM_NBSP ();
      HTM_TD_End ();

      /***** Write rest of main administrator's data *****/
      Ins.HieCod = UsrDat->InsCod;
      Hie_GetDataByCod[Hie_INS] (&Ins);
      Usr_WriteMainUsrDataExceptUsrID (UsrDat,The_GetColorRows ());

      HTM_TD_Begin ("class=\"LM %s\"",The_GetColorRows ());
	 Ins_DrawInstitutionLogoWithLink (&Ins,"ICO25x25");
      HTM_TD_End ();

   HTM_TR_End ();

   /***** Write degrees which are administrated by this administrator *****/
   Hie_GetAndWriteInsCtrDegAdminBy (UsrDat->UsrCod,
                                    Gbl.Usrs.Listing.WithPhotos ? Usr_NUM_MAIN_FIELDS_DATA_ADM :
                                	                          Usr_NUM_MAIN_FIELDS_DATA_ADM-1);
  }

/*****************************************************************************/
/************************* Write main data of a user *************************/
/*****************************************************************************/

static void Usr_WriteMainUsrDataExceptUsrID (struct Usr_Data *UsrDat,
                                             const char *BgColor)
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
                     UsrDat->FrstName[0] ? UsrDat->FrstName :
                	                    "&nbsp;",
                     NULL,true,UsrDat->Accepted);
  }

/*****************************************************************************/
/**************************** Write user's email *****************************/
/*****************************************************************************/

static void Usr_WriteEmail (struct Usr_Data *UsrDat,const char *BgColor)
  {
   bool ShowEmail;
   char MailLink[7 + Cns_MAX_BYTES_EMAIL_ADDRESS + 1];	// mailto:mail_address

   if (UsrDat->Email[0])
     {
      ShowEmail = Mai_ICanSeeOtherUsrEmail (UsrDat);
      if (ShowEmail)
         snprintf (MailLink,sizeof (MailLink),"mailto:%s",UsrDat->Email);
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
  }

/*****************************************************************************/
/********************* Write a cell with data of a user **********************/
/*****************************************************************************/

static void Usr_WriteUsrData (const char *BgColor,
                              const char *Data,const char *Link,
                              bool NonBreak,bool Accepted)
  {
   /***** Begin table cell *****/
   HTM_TD_Begin ("class=\"LM %s_%s %s\"",
		 Accepted ? (NonBreak ? "DAT_SMALL_NOBR_STRONG" :
				        "DAT_SMALL_STRONG") :
			    (NonBreak ? "DAT_SMALL_NOBR" :
				        "DAT_SMALL"),
		 The_GetSuffix (),
		 BgColor);

      /***** Container to limit length *****/
      HTM_DIV_Begin ("class=\"USR_DAT\"");

	 /***** Begin link *****/
	 if (Link)
	    HTM_A_Begin ("href=\"%s\" class=\"%s_%s\" target=\"_blank\"",
			 Link,Accepted ? "DAT_SMALL_NOBR_STRONG" :
					 "DAT_SMALL_NOBR",
			 The_GetSuffix ());

	 /***** Write data *****/
	 HTM_Txt (Data);
	 if (NonBreak)
	    HTM_NBSP ();

	 /***** End link *****/
	 if (Link)
	    HTM_A_End ();

      /***** End container *****/
      HTM_DIV_End ();

   /***** End table cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/*********** Get list of users with a given role in a given scope ************/
/*****************************************************************************/
// Role can be:
// - Rol_STD	Student
// - Rol_NET	Non-editing teacher
// - Rol_TCH	Teacher

void Usr_GetListUsrs (Hie_Level_t Level,Rol_Role_t Role)
  {
   char *Query = NULL;

   /***** Build query *****/
   Usr_DB_BuildQueryToGetUsrsLst (Level,Role,&Query);

   /***** Get list of users from database given a query *****/
   Usr_GetListUsrsFromQuery (Query,Role,Level);

   /***** Free query string *****/
   free (Query);
  }

/*****************************************************************************/
/*********** Search for users with a given role in current scope *************/
/*****************************************************************************/

void Usr_SearchListUsrs (Rol_Role_t Role)
  {
   char *Query = NULL;

   /***** Build query *****/
   Usr_DB_BuildQueryToSearchListUsrs (Role,&Query);

   /***** Get list of users from database given a query *****/
   Usr_GetListUsrsFromQuery (Query,Role,Gbl.Scope.Current);

   /***** Free query string *****/
   free (Query);
  }

/*****************************************************************************/
/************************ Get list with data of guests ***********************/
/*****************************************************************************/

static void Usr_GetGstsLst (Hie_Level_t Level)
  {
   char *Query = NULL;

   /***** Build query *****/
   Usr_DB_BuildQueryToGetGstsLst (Level,&Query);

   /***** Get list of students from database *****/
   Usr_GetListUsrsFromQuery (Query,Rol_GST,Level);

   /***** Free query string *****/
   free (Query);
  }

/*****************************************************************************/
/************** Get the user's codes of all students of a degree *************/
/*****************************************************************************/

void Usr_GetUnorderedStdsCodesInDeg (long DegCod)
  {
   char *Query = NULL;

   /***** Build query string *****/
   Usr_DB_BuildQueryToGetUnorderedStdsCodesInDeg (DegCod,&Query);

   /***** Get list of students *****/
   Usr_GetListUsrsFromQuery (Query,Rol_STD,Hie_DEG);

   /***** Free query string *****/
   free (Query);
  }

/*****************************************************************************/
/********************** Get list of users from database **********************/
/*****************************************************************************/

void Usr_GetListUsrsFromQuery (char *Query,Rol_Role_t Role,Hie_Level_t Level)
  {
   extern const char *Txt_The_list_of_X_users_is_too_large_to_be_displayed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsr;
   struct Usr_InList *UsrInList;
   bool Abort = false;

   if (Query == NULL)
     {
      Gbl.Usrs.LstUsrs[Role].NumUsrs = 0;
      return;
     }

   if (!Query[0])
     {
      Gbl.Usrs.LstUsrs[Role].NumUsrs = 0;
      return;
     }

   /***** Query database *****/
   if ((Gbl.Usrs.LstUsrs[Role].NumUsrs = (unsigned)
       DB_QuerySELECT (&mysql_res,"can not get list of users",
		       "%s",
		       Query)))
     {
      if (Gbl.Usrs.LstUsrs[Role].NumUsrs > Cfg_MAX_USRS_IN_LIST)
        {
	 Ale_ShowAlert (Ale_WARNING,Txt_The_list_of_X_users_is_too_large_to_be_displayed,
		        Gbl.Usrs.LstUsrs[Role].NumUsrs);
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
            /*
            row[ 0]: usr_data.UsrCod
            row[ 1]: usr_data.EncryptedUsrCod
            row[ 2]: usr_data.Password (used to check if a teacher can edit user's data)
	    row[ 3]: usr_data.Surname1
	    row[ 4]: usr_data.Surname2
	    row[ 5]: usr_data.FirstName
            row[ 6]: usr_data.Sex
            row[ 7]: usr_data.Photo
            row[ 8]: usr_data.PhotoVisibility
	    row[ 9]: usr_data.CtyCod
	    row[10]: usr_data.InsCod
	    row[11]: crs_users.Role	(only if Scope == Hie_CRS)
	    row[12]: crs_users.Accepted	(only if Scope == Hie_CRS)
	    */
            UsrInList = &Gbl.Usrs.LstUsrs[Role].Lst[NumUsr];

            /* Get user's code (row[0]) */
            UsrInList->UsrCod = Str_ConvertStrCodToLongCod (row[0]);

            /* Get encrypted user's code (row[1]), encrypted password (row[2]),
               surname 1 (row[3]), surname 2 (row[4]), first name (row[5]), */
	    Str_Copy (UsrInList->EnUsrCod,row[1],sizeof (UsrInList->EnUsrCod) - 1);
	    Str_Copy (UsrInList->Password,row[2],sizeof (UsrInList->Password) - 1);
	    Str_Copy (UsrInList->Surname1,row[3],sizeof (UsrInList->Surname1) - 1);
	    Str_Copy (UsrInList->Surname2,row[4],sizeof (UsrInList->Surname2) - 1);
	    Str_Copy (UsrInList->FrstName,row[5],sizeof (UsrInList->FrstName) - 1);

            /* Get user's sex (row[6]) */
            UsrInList->Sex = Usr_GetSexFromStr (row[6]);

            /* Get user's photo (row[7]) */
	    Str_Copy (UsrInList->Photo   ,row[7],sizeof (UsrInList->Photo   ) - 1);

            /* Get user's photo visibility (row[8]) */
            UsrInList->PhotoVisibility = Pri_GetVisibilityFromStr (row[8]);

            /* Get user's country code (row[9])
               and user's institution code (row[10]) */
	    UsrInList->CtyCod = Str_ConvertStrCodToLongCod (row[ 9]);
	    UsrInList->InsCod = Str_ConvertStrCodToLongCod (row[10]);

            /* Get user's role and acceptance of enrolment in course(s)
               (row[11], row[12] if Scope == Hie_CRS) */
            switch (Role)
              {
               case Rol_UNK:	// Here Rol_UNK means any user
		  switch (Level)
		    {
		     case Hie_UNK:	// Unknown
			Err_WrongHierarchyLevelExit ();
			break;
		     case Hie_SYS:	// System
			// Query result has not a column with the acceptation
			UsrInList->RoleInCurrentCrsDB = Rol_UNK;
			if (Enr_DB_GetNumCrssOfUsr (UsrInList->UsrCod))
			   UsrInList->Accepted = (Enr_DB_GetNumCrssOfUsrNotAccepted (UsrInList->UsrCod) == 0);
			else
			   UsrInList->Accepted = false;
			break;
		     case Hie_CTY:	// Country
		     case Hie_INS:	// Institution
		     case Hie_CTR:	// Center
		     case Hie_DEG:	// Degree
			// Query result has not a column with the acceptation
			UsrInList->RoleInCurrentCrsDB = Rol_UNK;
			UsrInList->Accepted = (Enr_DB_GetNumCrssOfUsrNotAccepted (UsrInList->UsrCod) == 0);
			break;
		     case Hie_CRS:	// Course
			// Query result has a column with the acceptation
			UsrInList->RoleInCurrentCrsDB = Rol_ConvertUnsignedStrToRole (row[11]);
			UsrInList->Accepted = (row[12][0] == 'Y');
			break;
		    }
        	  break;
               case Rol_GST:		// Guests have no courses,...
					// ...so they have not accepted...
					// ...inscription in any course
               case Rol_DEG_ADM:	// Any admin (degree, center, institution or system)
	          UsrInList->RoleInCurrentCrsDB = Rol_UNK;
	          UsrInList->Accepted = false;
	          break;
               case Rol_STD:
               case Rol_NET:
               case Rol_TCH:
		  switch (Level)
		    {
		     case Hie_UNK:	// Unknown
			Err_WrongHierarchyLevelExit ();
			break;
		     case Hie_SYS:	// System
		     case Hie_CTY:	// Country
		     case Hie_INS:	// Institution
		     case Hie_CTR:	// Center
		     case Hie_DEG:	// Degree
			// Query result has not a column with the acceptation
	                UsrInList->RoleInCurrentCrsDB = Rol_UNK;
			UsrInList->Accepted = (Enr_DB_GetNumCrssOfUsrWithARoleNotAccepted (UsrInList->UsrCod,Role) == 0);
			break;
		     case Hie_CRS:	// Course
			// Query result has a column with the acceptation
			UsrInList->RoleInCurrentCrsDB = Rol_ConvertUnsignedStrToRole (row[11]);
			UsrInList->Accepted = (row[12][0] == 'Y');
			break;
		    }
        	  break;
               default:
		  Err_WrongRoleExit ();
        	  break;
	      }

            /* By default, users are not removed */
            UsrInList->Remove = false;
           }
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (Abort)
      Err_ShowErrorAndExit (NULL);
  }

/*****************************************************************************/
/********************** Copy user's basic data from list *********************/
/*****************************************************************************/

void Usr_CopyBasicUsrDataFromList (struct Usr_Data *UsrDat,
                                   const struct Usr_InList *UsrInList)
  {
   UsrDat->UsrCod             = UsrInList->UsrCod;
   Str_Copy (UsrDat->EnUsrCod,UsrInList->EnUsrCod,sizeof (UsrDat->EnUsrCod) - 1);
   Str_Copy (UsrDat->Surname1,UsrInList->Surname1,sizeof (UsrDat->Surname1) - 1);
   Str_Copy (UsrDat->Surname2,UsrInList->Surname2,sizeof (UsrDat->Surname2) - 1);
   Str_Copy (UsrDat->FrstName,UsrInList->FrstName,sizeof (UsrDat->FrstName) - 1);
   UsrDat->Sex                = UsrInList->Sex;
   Str_Copy (UsrDat->Photo   ,UsrInList->Photo   ,sizeof (UsrDat->Photo   ) - 1);
   UsrDat->PhotoVisibility    = UsrInList->PhotoVisibility;
   UsrDat->CtyCod             = UsrInList->CtyCod;
   UsrDat->InsCod             = UsrInList->InsCod;
   UsrDat->Roles.InCurrentCrs = UsrInList->RoleInCurrentCrsDB;
   UsrDat->Accepted           = UsrInList->Accepted;
  }

/*****************************************************************************/
/********************** Allocate space for list of users *********************/
/*****************************************************************************/

static void Usr_AllocateUsrsList (Rol_Role_t Role)
  {
   if (Gbl.Usrs.LstUsrs[Role].NumUsrs)
      if ((Gbl.Usrs.LstUsrs[Role].Lst = calloc (Gbl.Usrs.LstUsrs[Role].NumUsrs,
                                                sizeof (*Gbl.Usrs.LstUsrs[Role].Lst))) == NULL)
         Err_NotEnoughMemoryExit ();
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
         free (Gbl.Usrs.LstUsrs[Role].Lst);
         Gbl.Usrs.LstUsrs[Role].Lst = NULL;
        }

      /***** Reset number of users *****/
      Gbl.Usrs.LstUsrs[Role].NumUsrs = 0;
     }
  }

/*****************************************************************************/
/******** Show form to confirm that I want to see a big list of users ********/
/*****************************************************************************/

bool Usr_GetIfShowBigList (unsigned NumUsrs,
                           Act_Action_t NextAction,
                           void (*FuncPars) (void *Args),void *Args,
                           const char *OnSubmit)
  {
   bool ShowBigList;

   /***** If list of users is too big... *****/
   if (NumUsrs > Cfg_MIN_NUM_USERS_TO_CONFIRM_SHOW_BIG_LIST)
     {
      /***** Get parameter with user's confirmation
             to see a big list of users *****/
      if (!(ShowBigList = Par_GetParBool ("ShowBigList")))
	 Usr_PutButtonToConfirmIWantToSeeBigList (NumUsrs,
	                                          NextAction,FuncPars,Args,
	                                          OnSubmit);

      return ShowBigList;
     }
   else
      return true;        // List is not too big ==> show it
  }

/*****************************************************************************/
/******** Show form to confirm that I want to see a big list of users ********/
/*****************************************************************************/

static void Usr_PutButtonToConfirmIWantToSeeBigList (unsigned NumUsrs,
						     Act_Action_t NextAction,
                                                     void (*FuncPars) (void *Args),void *Args,
                                                     const char *OnSubmit)
  {
   extern const char *Txt_The_list_of_X_users_is_too_large_to_be_displayed;
   extern const char *Txt_Show_anyway;

   /***** Show alert and button to confirm that I want to see the big list *****/
   Usr_FuncParsBigList = FuncPars;	// Used to pass pointer to function
   Ale_ShowAlertAndButton (NextAction,Usr_USER_LIST_SECTION_ID,OnSubmit,
                           Usr_PutParsConfirmIWantToSeeBigList,Args,
                           Btn_CONFIRM_BUTTON,Txt_Show_anyway,
			   Ale_WARNING,Txt_The_list_of_X_users_is_too_large_to_be_displayed,
                           NumUsrs);
  }

static void Usr_PutParsConfirmIWantToSeeBigList (void *Args)
  {
   Grp_PutParsCodGrps ();
   Set_PutParsPrefsAboutUsrList ();
   if (Usr_FuncParsBigList)
      Usr_FuncParsBigList (Args);
   Par_PutParChar ("ShowBigList",'Y');
  }

/*****************************************************************************/
/************ Create list of selected users with one given user **************/
/*****************************************************************************/

void Usr_CreateListSelectedUsrsCodsAndFillWithOtherUsr (struct Usr_SelectedUsrs *SelectedUsrs)
  {
   /***** Create list of user codes and put encrypted user code in it *****/
   if (!SelectedUsrs->List[Rol_UNK])
     {
      if ((SelectedUsrs->List[Rol_UNK] =
	   malloc (Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1)) == NULL)
         Err_NotEnoughMemoryExit ();
      Str_Copy (SelectedUsrs->List[Rol_UNK],Gbl.Usrs.Other.UsrDat.EnUsrCod,
		Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      SelectedUsrs->Filled = true;
     }
  }

/*****************************************************************************/
/************* Write parameter with the list of users selected ***************/
/*****************************************************************************/

void Usr_PutParSelectedUsrsCods (struct Usr_SelectedUsrs *SelectedUsrs)
  {
   char *ParName;

   /***** Put a parameter indicating that a list of several users is present *****/
   Par_PutParChar ("MultiUsrs",'Y');

   /***** Put a parameter with the encrypted user codes of several users *****/
   /* Build name of the parameter.
      Sometimes a unique action needs several distinct lists of users,
      so, it's necessary to use distinct names for the parameters. */
   Usr_BuildParName (&ParName,Usr_ParUsrCod[Rol_UNK],SelectedUsrs->ParSuffix);

   /* Put the parameter *****/
   if (Gbl.Session.IsOpen)
      Ses_InsertParInDB (ParName,SelectedUsrs->List[Rol_UNK]);
   else
      Par_PutParString (NULL,ParName,SelectedUsrs->List[Rol_UNK]);

   /***** Free allocated memory for parameter name *****/
   free (ParName);
  }

/*****************************************************************************/
/************************* Get list of selected users ************************/
/*****************************************************************************/

void Usr_GetListsSelectedEncryptedUsrsCods (struct Usr_SelectedUsrs *SelectedUsrs)
  {
   extern const char *Par_SEPARATOR_PARAM_MULTIPLE;
   char *ParName;
   unsigned Length;
   Rol_Role_t Role;

   if (!SelectedUsrs->Filled)	// Get list only if not already got
     {
      /***** Build name of the parameter.
	     Sometimes a unique action needs several distinct lists of users,
	     so, it's necessary to use distinct names for the parameters. *****/
      Usr_BuildParName (&ParName,Usr_ParUsrCod[Rol_UNK],SelectedUsrs->ParSuffix);

      /***** Get possible list of all selected users *****/
      Usr_AllocateListSelectedEncryptedUsrCods (SelectedUsrs,Rol_UNK);
      if (Gbl.Session.IsOpen)	// If the session is open, get parameter from DB
	{
	 Ses_DB_GetPar (ParName,SelectedUsrs->List[Rol_UNK],
			Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);
	 Str_ChangeFormat (Str_FROM_FORM,Str_TO_TEXT,SelectedUsrs->List[Rol_UNK],
			   Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS,
			   Str_REMOVE_SPACES);
	}
      else
	 Par_GetParMultiToText (ParName,SelectedUsrs->List[Rol_UNK],
				Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);

      /***** Free allocated memory for parameter name *****/
      free (ParName);

      /***** Get list of selected users for each possible role *****/
      for (Role  = Rol_TCH;	// From the highest possible role of selected users...
	   Role >= Rol_GST;	// ...downto the lowest possible role of selected users
	   Role--)
	 if (Usr_ParUsrCod[Role])
	   {
            /* Build name of the parameter */
	    Usr_BuildParName (&ParName,Usr_ParUsrCod[Role],SelectedUsrs->ParSuffix);

	    /* Get parameter with selected users with this role */
	    Usr_AllocateListSelectedEncryptedUsrCods (SelectedUsrs,Role);
	    Par_GetParMultiToText (ParName,SelectedUsrs->List[Role],
				   Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);

	    /* Free allocated memory for parameter name */
	    free (ParName);

	    /* Add selected users with this role
	       to the list with all selected users */
	    if (SelectedUsrs->List[Role][0])
	      {
	       if (SelectedUsrs->List[Rol_UNK][0])
		  if ((Length = strlen (SelectedUsrs->List[Rol_UNK])) <
		      Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS)
		    {
		     SelectedUsrs->List[Rol_UNK][Length    ] = Par_SEPARATOR_PARAM_MULTIPLE[0];
		     SelectedUsrs->List[Rol_UNK][Length + 1] = '\0';
		    }
	       Str_Concat (SelectedUsrs->List[Rol_UNK],SelectedUsrs->List[Role],
			   Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);
	      }
	   }

      /***** List is filled *****/
      SelectedUsrs->Filled = true;
     }
  }

static void Usr_BuildParName (char **ParName,
			      const char *ParRoot,
			      const char *ParSuffix)
  {
   /* Build name of the parameter.
      Sometimes a unique action needs several distinct lists of users,
      so, it's necessary to use distinct names for the parameters. */
   if (ParSuffix)
     {
      if (asprintf (ParName,"%s%s",ParRoot,ParSuffix) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else
     {
      if (asprintf (ParName,"%s",ParRoot) < 0)
	 Err_NotEnoughMemoryExit ();
     }
  }

/*****************************************************************************/
/*** Get list of recipients of a message written explicitely by the sender ***/
/*****************************************************************************/
// Returns true if no errors

bool Usr_GetListMsgRecipientsWrittenExplicitelyBySender (bool WriteErrorMsgs)
  {
   extern const char *Par_SEPARATOR_PARAM_MULTIPLE;
   extern const char *Txt_There_is_no_user_with_nickname_X;
   extern const char *Txt_There_is_no_user_with_email_X;
   extern const char *Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email;
   extern const char *Txt_There_is_no_user_with_ID_nick_or_email_X;
   extern const char *Txt_The_ID_nickname_or_email_X_is_not_valid;
   size_t LengthSelectedUsrsCods;
   size_t LengthUsrCod;
   const char *Ptr;
   char UsrIDNickOrEmail[Cns_MAX_BYTES_USR_LOGIN + 1];
   struct Usr_Data UsrDat;
   struct Usr_ListUsrCods ListUsrCods;
   bool Error = false;

   /***** Get list of selected encrypted users's codes if not already got.
          This list is necessary to add encrypted user's codes at the end. *****/
   Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
   LengthSelectedUsrsCods = strlen (Gbl.Usrs.Selected.List[Rol_UNK]);

   /***** Allocate memory for the lists of recipients written explicetely *****/
   Usr_AllocateListOtherRecipients ();

   /***** Get recipients written explicetely *****/
   Par_GetParText ("OtherRecipients",Gbl.Usrs.ListOtherRecipients,
                     Msg_MAX_BYTES_LIST_OTHER_RECIPIENTS);

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
         Str_GetNextStringUntilComma (&Ptr,UsrIDNickOrEmail,sizeof (UsrIDNickOrEmail) - 1);

         /* Check if string is plain user's ID or nickname and get encrypted user's ID */
         if (UsrIDNickOrEmail[0])
           {
	    /***** Reset default list of users' codes *****/
	    ListUsrCods.NumUsrs = 0;
	    ListUsrCods.Lst = NULL;

	    if (Nck_CheckIfNickWithArrIsValid (UsrIDNickOrEmail))	// 1: It's a nickname
	      {
	       if ((UsrDat.UsrCod = Nck_GetUsrCodFromNickname (UsrIDNickOrEmail)) > 0)
		 {
		  ListUsrCods.NumUsrs = 1;
		  Usr_AllocateListUsrCods (&ListUsrCods);
		  ListUsrCods.Lst[0] = UsrDat.UsrCod;
		 }
	       else
		 {
		  if (WriteErrorMsgs)
		     Ale_ShowAlert (Ale_WARNING,Txt_There_is_no_user_with_nickname_X,
			            UsrIDNickOrEmail);
		  Error = true;
		 }
	      }
	    else if (Mai_CheckIfEmailIsValid (UsrIDNickOrEmail))	// 2: It's an email
	      {
	       if ((UsrDat.UsrCod = Mai_DB_GetUsrCodFromEmail (UsrIDNickOrEmail)) > 0)
		 {
		  ListUsrCods.NumUsrs = 1;
		  Usr_AllocateListUsrCods (&ListUsrCods);
		  ListUsrCods.Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;
		 }
	       else
		 {
		  if (WriteErrorMsgs)
		     Ale_ShowAlert (Ale_WARNING,Txt_There_is_no_user_with_email_X,
			            UsrIDNickOrEmail);
		  Error = true;
		 }
	      }
            else							// 3: It's not a nickname nor email
              {
               // Users' IDs are always stored internally in capitals and without leading zeros
	       Str_RemoveLeadingZeros (UsrIDNickOrEmail);
	       Str_ConvertToUpperText (UsrIDNickOrEmail);
               if (ID_CheckIfUsrIDIsValid (UsrIDNickOrEmail))
		 {
		  // It seems a user's ID
		  /***** Allocate space for the list *****/
		  ID_ReallocateListIDs (&UsrDat,1);

		  Str_Copy (UsrDat.IDs.List[0].ID,UsrIDNickOrEmail,
		            sizeof (UsrDat.IDs.List[0].ID) - 1);

		  /***** Check if a user exists having this user's ID *****/
		  if (ID_GetListUsrCodsFromUsrID (&UsrDat,NULL,&ListUsrCods,false))
		    {
		     if (ListUsrCods.NumUsrs > 1)	// Two or more user share the same user's ID
		       {// TODO: Consider forbid IDs here
			if (WriteErrorMsgs)
			   Ale_ShowAlert (Ale_ERROR,Txt_There_are_more_than_one_user_with_the_ID_X_Please_type_a_nick_or_email,
				          UsrIDNickOrEmail);
			Error = true;
		       }
		    }
		  else	// No users found
		    {
		     if (WriteErrorMsgs)
			Ale_ShowAlert (Ale_ERROR,Txt_There_is_no_user_with_ID_nick_or_email_X,
				       UsrIDNickOrEmail);
		     Error = true;
		    }
		 }
	       else	// String is not a valid user's nickname, email or ID
		 {
		  if (WriteErrorMsgs)
		     Ale_ShowAlert (Ale_WARNING,Txt_The_ID_nickname_or_email_X_is_not_valid,
			            UsrIDNickOrEmail);
		  Error = true;
		 }
              }

            if (ListUsrCods.NumUsrs == 1)	// Only if user is valid
              {
               /* Get user's data */
	       Usr_GetUsrDataFromUsrCod (&UsrDat,	// Really only EncryptedUsrCod is needed
	                                 Usr_DONT_GET_PREFS,
	                                 Usr_DONT_GET_ROLE_IN_CRS);

               /* Find if encrypted user's code is already in list */
               if (!Usr_FindEncryptedUsrCodInListOfSelectedEncryptedUsrCods (UsrDat.EnUsrCod,&Gbl.Usrs.Selected))        // If not in list ==> add it
                 {
                  LengthUsrCod = strlen (UsrDat.EnUsrCod);

                  /* Add encrypted user's code to list of users */
                  if (LengthSelectedUsrsCods == 0)	// First user in list
                    {
                     if (LengthUsrCod < Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS)
                       {
                        /* Add user */
                        Str_Copy (Gbl.Usrs.Selected.List[Rol_UNK],
                                  UsrDat.EnUsrCod,
                                  Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);
                        LengthSelectedUsrsCods = LengthUsrCod;
                       }
                    }
                  else					// Not first user in list
                    {
                     if (LengthSelectedUsrsCods + (1 + LengthUsrCod) <
                	 Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS)
                       {
                        /* Add separator */
                        Gbl.Usrs.Selected.List[Rol_UNK][LengthSelectedUsrsCods] = Par_SEPARATOR_PARAM_MULTIPLE[0];
                        LengthSelectedUsrsCods++;

                        /* Add user */
                        Str_Copy (Gbl.Usrs.Selected.List[Rol_UNK] + LengthSelectedUsrsCods,
                                  UsrDat.EnUsrCod,
                                  Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS);
                        LengthSelectedUsrsCods += LengthUsrCod;
                       }
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
// Returns true if EncryptedUsrCodToFind is in list

bool Usr_FindEncryptedUsrCodInListOfSelectedEncryptedUsrCods (const char *EncryptedUsrCodToFind,
							      struct Usr_SelectedUsrs *SelectedUsrs)
  {
   const char *Ptr;
   char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1];

   if (SelectedUsrs->List[Rol_UNK])
     {
      Ptr = SelectedUsrs->List[Rol_UNK];
      while (*Ptr)
	{
	 Par_GetNextStrUntilSeparParMult (&Ptr,EncryptedUsrCod,
	                                    Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
	 if (!strcmp (EncryptedUsrCodToFind,EncryptedUsrCod))
	    return true;        // Found!
	}
     }
   return false;        // List not allocated or user not found
  }

/*****************************************************************************/
/******* Check if there are valid users in list of encrypted user codes ******/
/*****************************************************************************/

bool Usr_CheckIfThereAreUsrsInListOfSelectedEncryptedUsrCods (struct Usr_SelectedUsrs *SelectedUsrs)
  {
   const char *Ptr;
   struct Usr_Data UsrDat;

   /***** Loop over the list to check if there are valid users *****/
   Ptr = SelectedUsrs->List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,UsrDat.EnUsrCod,
                                         Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
      if (UsrDat.UsrCod > 0)
         return true;
     }
   return false;
  }

/*****************************************************************************/
/******** Count number of valid users in list of encrypted user codes ********/
/*****************************************************************************/

unsigned Usr_CountNumUsrsInListOfSelectedEncryptedUsrCods (struct Usr_SelectedUsrs *SelectedUsrs)
  {
   const char *Ptr;
   unsigned NumUsrs = 0;
   struct Usr_Data UsrDat;

   /***** Loop over the list to count the number of users *****/
   Ptr = SelectedUsrs->List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,UsrDat.EnUsrCod,
                                         Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
      if (UsrDat.UsrCod > 0)
         NumUsrs++;
     }
   return NumUsrs;
  }

/*****************************************************************************/
/****************** Allocate memory for list of students *********************/
/*****************************************************************************/
// Role = Rol_UNK here means all users

static void Usr_AllocateListSelectedEncryptedUsrCods (struct Usr_SelectedUsrs *SelectedUsrs,
						      Rol_Role_t Role)
  {
   if (!SelectedUsrs->List[Role])
     {
      if ((SelectedUsrs->List[Role] =
	   malloc (Usr_MAX_BYTES_LIST_ENCRYPTED_USR_CODS + 1)) == NULL)
         Err_NotEnoughMemoryExit ();
      SelectedUsrs->List[Role][0] = '\0';
     }
  }

/*****************************************************************************/
/******* Free memory used by lists of selected encrypted users' codes ********/
/*****************************************************************************/
// Role = Rol_UNK here means all users

void Usr_FreeListsSelectedEncryptedUsrsCods (struct Usr_SelectedUsrs *SelectedUsrs)
  {
   Rol_Role_t Role;

   if (SelectedUsrs->Filled)	// Only if lists are filled
     {
      /***** Free lists *****/
      for (Role  = (Rol_Role_t) 0;
	   Role <= (Rol_Role_t) (Rol_NUM_ROLES - 1);
	   Role++)
	 if (SelectedUsrs->List[Role])
	   {
	    free (SelectedUsrs->List[Role]);
	    SelectedUsrs->List[Role] = NULL;
	   }

      /***** Mark lists as empty *****/
      SelectedUsrs->Filled = false;
      // Lists of encrypted codes of users selected from form
      // are now marked as not filled
     }
  }

/*****************************************************************************/
/************* Get list of users selected to show their projects *************/
/*****************************************************************************/

void Usr_GetListSelectedUsrCods (struct Usr_SelectedUsrs *SelectedUsrs,
				 unsigned NumUsrsInList,
				 long **LstSelectedUsrCods)
  {
   unsigned NumUsr;
   const char *Ptr;
   struct Usr_Data UsrDat;

   /***** Create list of user codes *****/
   if ((*LstSelectedUsrCods = calloc (NumUsrsInList,
                                      sizeof (**LstSelectedUsrCods))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Loop over the list getting users' codes *****/
   for (NumUsr = 0, Ptr = SelectedUsrs->List[Rol_UNK];
	NumUsr < NumUsrsInList && *Ptr;
	NumUsr++)
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,UsrDat.EnUsrCod,
                                         Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
      (*LstSelectedUsrCods)[NumUsr] = UsrDat.UsrCod;
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

void Usr_FreeListSelectedUsrCods (long *LstSelectedUsrCods)
  {
   if (LstSelectedUsrCods)
      free (LstSelectedUsrCods);
  }

/*****************************************************************************/
/******** Create subquery string with users' codes separated by commas *******/
/******** from list of users' codes                                    *******/
/*****************************************************************************/

void Usr_CreateSubqueryUsrCods (long LstSelectedUsrCods[],
				unsigned NumUsrsInList,
				char **UsrsSubQuery)
  {
   char SubQueryOneUsr[1 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   unsigned NumUsr;
   size_t MaxLength;

   /***** Allocate space for subquery *****/
   MaxLength = (size_t) NumUsrsInList * (size_t) (1 + Cns_MAX_DECIMAL_DIGITS_LONG);
   if ((*UsrsSubQuery = malloc (MaxLength + 1)) == NULL)
      Err_NotEnoughMemoryExit ();
   (*UsrsSubQuery)[0] = '\0';

   /***** Build subquery *****/
   for (NumUsr = 0;
	NumUsr < NumUsrsInList;
	NumUsr++)
      if (NumUsr)
	{
	 snprintf (SubQueryOneUsr,sizeof (SubQueryOneUsr),",%ld",
		   LstSelectedUsrCods[NumUsr]);
	 Str_Concat (*UsrsSubQuery,SubQueryOneUsr,MaxLength);
	}
      else
	 snprintf (*UsrsSubQuery,sizeof (SubQueryOneUsr),"%ld",
		   LstSelectedUsrCods[NumUsr]);
  }

void Usr_FreeSubqueryUsrCods (char *SubQueryUsrs)
  {
   free (SubQueryUsrs);
  }

/*****************************************************************************/
/********** Allocate memory for list of users's IDs or nicknames *************/
/*****************************************************************************/

static void Usr_AllocateListOtherRecipients (void)
  {
   if (!Gbl.Usrs.ListOtherRecipients)
     {
      if ((Gbl.Usrs.ListOtherRecipients = malloc (Msg_MAX_BYTES_LIST_OTHER_RECIPIENTS + 1)) == NULL)
         Err_NotEnoughMemoryExit ();
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
      free (Gbl.Usrs.ListOtherRecipients);
      Gbl.Usrs.ListOtherRecipients = NULL;
     }
  }

/*****************************************************************************/
/*************************** Selection of list type **************************/
/*****************************************************************************/

void Usr_ShowFormsToSelectUsrListType (Act_Action_t NextAction,
				       void (*FuncPars) (void *Args),void *Args,
				       const char *OnSubmit)
  {
   Set_BeginSettingsHead ();
   Set_BeginOneSettingSelector ();

   /***** Select Set_USR_LIST_AS_CLASS_PHOTO *****/
   Set_BeginPref (Gbl.Usrs.Me.ListType == Set_USR_LIST_AS_CLASS_PHOTO);

      Set_FormToSelectUsrListType (NextAction,FuncPars,Args,OnSubmit,
				   Set_USR_LIST_AS_CLASS_PHOTO);

      /* Number of columns in the class photo */
      Frm_BeginFormAnchor (NextAction,Usr_USER_LIST_SECTION_ID);
	 Grp_PutParsCodGrps ();
	 Set_PutParUsrListType (Set_USR_LIST_AS_CLASS_PHOTO);
	 Set_PutParListWithPhotos ();
	 Usr_PutSelectorNumColsClassPhoto ();
	 if (FuncPars)
	    FuncPars (Args);
      Frm_EndForm ();

   Set_EndPref ();

   /***** Select Usr_LIST_AS_LISTING *****/
   Set_BeginPref (Gbl.Usrs.Me.ListType == Set_USR_LIST_AS_LISTING);

      Set_FormToSelectUsrListType (NextAction,FuncPars,Args,OnSubmit,
				   Set_USR_LIST_AS_LISTING);

      /* See the photos in list? */
      Frm_BeginFormAnchor (NextAction,Usr_USER_LIST_SECTION_ID);
	 Grp_PutParsCodGrps ();
	 Set_PutParUsrListType (Set_USR_LIST_AS_LISTING);
	 if (FuncPars)
	    FuncPars (Args);
	 Usr_PutCheckboxListWithPhotos ();
      Frm_EndForm ();

   Set_EndPref ();

   Set_EndOneSettingSelector ();
   Set_EndSettingsHead ();
  }

/*****************************************************************************/
/************* Put a radio element to select a users' list type **************/
/*****************************************************************************/

static void Set_FormToSelectUsrListType (Act_Action_t NextAction,
					 void (*FuncPars) (void *Args),void *Args,
					 const char *OnSubmit,
					 Set_ShowUsrsType_t ListType)
  {
   extern const char *Txt_USR_LIST_TYPES[Set_NUM_USR_LIST_TYPES];

   /***** Begin form *****/
   Frm_BeginFormAnchorOnSubmit (NextAction,Usr_USER_LIST_SECTION_ID,OnSubmit);
      Grp_PutParsCodGrps ();
      Set_PutParUsrListType (ListType);
      Set_PutParListWithPhotos ();
      if (FuncPars)
	 FuncPars (Args);

      /***** Link and image *****/
      if (OnSubmit)
	 HTM_BUTTON_Submit_Begin (Txt_USR_LIST_TYPES[ListType],
				  "class=\"BT_LINK FORM_IN_%s NOWRAP\" onsubmit=\"%s\"",
				  The_GetSuffix (),OnSubmit);
      else
	 HTM_BUTTON_Submit_Begin (Txt_USR_LIST_TYPES[ListType],
				  "class=\"BT_LINK FORM_IN_%s NOWRAP\"",
				  The_GetSuffix ());
      Ico_PutIcon (Usr_IconsClassPhotoOrList[ListType],Ico_BLACK,
		   Txt_USR_LIST_TYPES[ListType],"ICO20x20");
      HTM_NBSPTxt (Txt_USR_LIST_TYPES[ListType]);
      HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** List users to select some of them **********************/
/*****************************************************************************/

void Usr_PutFormToSelectUsrsToGoToAct (struct Usr_SelectedUsrs *SelectedUsrs,
				       Act_Action_t NextAction,
				       void (*FuncPars) (void *Args),void *Args,
				       const char *Title,
                                       const char *HelpLink,
                                       const char *TxtButton,
				       Frm_PutForm_t PutFormDateRange)
  {
   extern const char *Txt_Users;
   unsigned NumTotalUsrs;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME] = Dat_HMS_DO_NOT_SET
     };

   /***** Begin box *****/
   Box_BoxBegin (Title,NULL,NULL,HelpLink,Box_NOT_CLOSABLE);

      /***** Get and update type of list,
	     number of columns in class photo
	     and preference about view photos *****/
      Set_GetAndUpdatePrefsAboutUsrList ();

      /***** Get groups to show ******/
      Grp_GetParCodsSeveralGrpsToShowUsrs ();

      /***** Get and order lists of users from this course *****/
      Usr_GetListUsrs (Hie_CRS,Rol_STD);
      Usr_GetListUsrs (Hie_CRS,Rol_NET);
      Usr_GetListUsrs (Hie_CRS,Rol_TCH);
      NumTotalUsrs = Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs +
		     Gbl.Usrs.LstUsrs[Rol_NET].NumUsrs +
		     Gbl.Usrs.LstUsrs[Rol_TCH].NumUsrs;

      /***** Show form to select the groups *****/
      Grp_ShowFormToSelectSeveralGroups (NextAction,FuncPars,Args,NULL);

      /***** Begin section with user list *****/
      HTM_SECTION_Begin (Usr_USER_LIST_SECTION_ID);

	 if (NumTotalUsrs)
	   {
	    if (Usr_GetIfShowBigList (NumTotalUsrs,
				      NextAction,FuncPars,Args,
				      NULL))
	      {
	       /***** Form to select type of list used for select several users *****/
	       Usr_ShowFormsToSelectUsrListType (NextAction,FuncPars,Args,NULL);

	       /***** Link to register students *****/
	       Enr_CheckStdsAndPutButtonToRegisterStdsInCurrentCrs ();

	       /***** Form to select users and select date range ****/
	       /* Begin form */
	       Frm_BeginForm (NextAction);

		  /* Hidden parameters */
		  Grp_PutParsCodGrps ();
		  if (NextAction == ActAdmAsgWrkCrs)
		    {
		     Gbl.FileBrowser.FullTree = true;	// By default, show all files
		     Brw_PutParFullTreeIfSelected (&Gbl.FileBrowser.FullTree);
		    }
		  if (FuncPars)
		     FuncPars (Args);

		  HTM_TABLE_BeginCenterPadding (2);

		     /* Put list of users to select some of them */
		     HTM_TR_Begin (NULL);
			/* Label */
			Frm_LabelColumn ("Frm_C1 RT","Txt",Txt_Users);

			/* Data */
			HTM_TD_Begin ("class=\"Frm_C2 LT\"");
		           HTM_TABLE_Begin ("TBL_SCROLL_C2");
			      Usr_ListUsersToSelect (SelectedUsrs);
			   HTM_TABLE_End ();
			HTM_TD_End ();
		     HTM_TR_End ();

		     /* Starting and ending dates in the search */
		     if (PutFormDateRange == Frm_PUT_FORM)
			Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (SetHMS);

		  HTM_TABLE_End ();

		  /***** Send button *****/
		  Btn_PutConfirmButton (TxtButton);

	       /***** End form *****/
	       Frm_EndForm ();
	      }
	   }
	 else	// NumTotalUsrs == 0
	    /***** Show warning indicating no users found *****/
	    Usr_ShowWarningNoUsersFound (Rol_UNK);

      /***** End section with user list *****/
      HTM_SECTION_End ();

      /***** Free memory for users' list *****/
      Usr_FreeUsrsList (Rol_TCH);
      Usr_FreeUsrsList (Rol_NET);
      Usr_FreeUsrsList (Rol_STD);

      /***** Free memory for list of selected groups *****/
      Grp_FreeListCodSelectedGrps ();

   /***** End box *****/
   Box_BoxEnd ();
  }

void Usr_GetSelectedUsrsAndGoToAct (struct Usr_SelectedUsrs *SelectedUsrs,
				    void (*FuncWhenUsrsSelected) (void *ArgsSelected),void *ArgsSelected,
                                    void (*FuncWhenNoUsrsSelected) (void *ArgsNoSelected),void *ArgsNoSelected)
  {
   extern const char *Txt_You_must_select_one_ore_more_users;

   /***** Get lists of the selected users if not already got *****/
   Usr_GetListsSelectedEncryptedUsrsCods (SelectedUsrs);

   /***** Check number of users *****/
   if (Usr_CheckIfThereAreUsrsInListOfSelectedEncryptedUsrCods (SelectedUsrs))	// If some users are selected...
      FuncWhenUsrsSelected (ArgsSelected);
   else	// If no users are selected...
     {
      // ...write warning alert
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_one_ore_more_users);
      // ...and show again the form
      FuncWhenNoUsrsSelected (ArgsNoSelected);
     }

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (SelectedUsrs);
  }

/*****************************************************************************/
/********************** List users to select some of them ********************/
/*****************************************************************************/

void Usr_ListUsersToSelect (struct Usr_SelectedUsrs *SelectedUsrs)
  {
   Usr_ListUsersByRoleToSelect (SelectedUsrs,Rol_TCH);
   Usr_ListUsersByRoleToSelect (SelectedUsrs,Rol_NET);
   Usr_ListUsersByRoleToSelect (SelectedUsrs,Rol_STD);
  }

/*****************************************************************************/
/*********** List users with a given role to select some of them *************/
/*****************************************************************************/

static void Usr_ListUsersByRoleToSelect (struct Usr_SelectedUsrs *SelectedUsrs,
					 Rol_Role_t Role)
  {
   /***** If there are no users, don't list anything *****/
   if (!Gbl.Usrs.LstUsrs[Role].NumUsrs)
      return;

   /***** Draw the classphoto/list *****/
   switch (Gbl.Usrs.Me.ListType)
     {
      case Set_USR_LIST_AS_CLASS_PHOTO:
         Usr_DrawClassPhoto (SelectedUsrs,Role,Usr_CLASS_PHOTO_SEL,
			     true);	// Put checkbox to select user
         break;
      case Set_USR_LIST_AS_LISTING:
         Usr_ListUsrsForSelection (SelectedUsrs,Role);
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/*************** List users (of current course) for selection ****************/
/*****************************************************************************/

static void Usr_ListUsrsForSelection (struct Usr_SelectedUsrs *SelectedUsrs,
				      Rol_Role_t Role)
  {
   unsigned NumUsr;
   struct Usr_Data UsrDat;

   if (Gbl.Usrs.LstUsrs[Role].NumUsrs)
     {
      /***** Initialize field names *****/
      Usr_SetUsrDatMainFieldNames ();

      /***** Put a row to select all users *****/
      Usr_PutCheckboxToSelectAllUsers (SelectedUsrs,Role);

      /***** Heading row with column names *****/
      Usr_WriteHeaderFieldsUsrDat (true);	// Columns for the data

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List users' data *****/
      for (NumUsr = 0, The_ResetRowColor ();
	   NumUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs; )
	{
	 UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].UsrCod;
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
	                                              Usr_DONT_GET_PREFS,
	                                              Usr_DONT_GET_ROLE_IN_CRS))
	   {
	    UsrDat.Roles.InCurrentCrs = Role;	// We know the user's role.
						// It is not necessary to retrieve
						// his/her role from database.
	    UsrDat.Accepted = Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Accepted;
	    Usr_WriteRowUsrMainData (++NumUsr,&UsrDat,true,Role,SelectedUsrs);

	    The_ChangeRowColor ();
	   }
	}

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
  }

/*****************************************************************************/
/******** Put a row, in a classphoto or a list, to select all users **********/
/*****************************************************************************/

static void Usr_PutCheckboxToSelectAllUsers (struct Usr_SelectedUsrs *SelectedUsrs,
					     Rol_Role_t Role)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char *ParName;
   Usr_Sex_t Sex;

   HTM_TR_Begin (NULL);

      HTM_TH_Span_Begin (HTM_HEAD_LEFT,1,Usr_GetColumnsForSelectUsrs (),"BG_HIGHLIGHT");

	 HTM_LABEL_Begin (NULL);
	    if (Usr_NameSelUnsel[Role] && Usr_ParUsrCod[Role])
	      {
	       Usr_BuildParName (&ParName,Usr_ParUsrCod[Role],SelectedUsrs->ParSuffix);
	       HTM_INPUT_CHECKBOX (Usr_NameSelUnsel[Role],HTM_DONT_SUBMIT_ON_CHANGE,
				   "value=\"\" onclick=\"togglecheckChildren(this,'%s')\"",
				   ParName);
	       free (ParName);
	      }
	    else
	       Err_WrongRoleExit ();
	    Sex = Usr_GetSexOfUsrsLst (Role);
	    HTM_TxtColon (Gbl.Usrs.LstUsrs[Role].NumUsrs == 1 ? Txt_ROLES_SINGUL_Abc[Role][Sex] :
								Txt_ROLES_PLURAL_Abc[Role][Sex]);
	 HTM_LABEL_End ();

      HTM_TH_End ();

   HTM_TR_End ();
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
   return (Gbl.Usrs.Me.ListType == Set_USR_LIST_AS_CLASS_PHOTO) ? Gbl.Usrs.ClassPhoto.Cols :
                                                                 (Gbl.Usrs.Listing.WithPhotos ? 1 + Usr_NUM_MAIN_FIELDS_DATA_USR :
                                                                                                Usr_NUM_MAIN_FIELDS_DATA_USR);
  }

/*****************************************************************************/
/******* Put a checkbox, in a classphoto or a list, to select a user *********/
/*****************************************************************************/

static void Usr_PutCheckboxToSelectUser (Rol_Role_t Role,
                                         const char *EncryptedUsrCod,
                                         bool UsrIsTheMsgSender,
					 struct Usr_SelectedUsrs *SelectedUsrs)
  {
   bool CheckboxChecked;
   char *ParName;

   if (Usr_NameSelUnsel[Role] && Usr_ParUsrCod[Role])
     {
      /***** Check box must be checked? *****/
      if (UsrIsTheMsgSender)
	 CheckboxChecked = true;
      else
	 /* Check if user is in lists of selected users */
	 CheckboxChecked = Usr_FindEncryptedUsrCodInListOfSelectedEncryptedUsrCods (EncryptedUsrCod,SelectedUsrs);

      /***** Check box *****/
      Usr_BuildParName (&ParName,Usr_ParUsrCod[Role],SelectedUsrs->ParSuffix);
      HTM_INPUT_CHECKBOX (ParName,HTM_DONT_SUBMIT_ON_CHANGE,
			  "value=\"%s\"%s onclick=\"checkParent(this,'%s')\"",
			  EncryptedUsrCod,
			  CheckboxChecked ? " checked=\"checked\"" :
				            "",
			  Usr_NameSelUnsel[Role]);
      free (ParName);
     }
   else
      Err_WrongRoleExit ();
  }

/*****************************************************************************/
/********* Put a checkbox to select whether list users with photos ***********/
/*****************************************************************************/

static void Usr_PutCheckboxListWithPhotos (void)
  {
   extern const char *Txt_Display_photos;

   Par_PutParChar ("WithPhotosExists",'Y');

   /***** Put checkbox to select whether list users with photos *****/
   HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
      HTM_INPUT_CHECKBOX ("WithPhotos",HTM_SUBMIT_ON_CHANGE,
			  "value=\"Y\"%s",
			  Gbl.Usrs.Listing.WithPhotos ? " checked=\"checked\"" :
							"");
      HTM_Txt (Txt_Display_photos);
   HTM_LABEL_End ();
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
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];

   /***** Initialize field names *****/
   Usr_UsrDatMainFieldNames[0] = "&nbsp;";
   Usr_UsrDatMainFieldNames[1] = Txt_No_INDEX;
   Usr_UsrDatMainFieldNames[2] = Txt_Photo;
   Usr_UsrDatMainFieldNames[3] = Txt_ID;
   Usr_UsrDatMainFieldNames[4] = Txt_Surname_1;
   Usr_UsrDatMainFieldNames[5] = Txt_Surname_2;
   Usr_UsrDatMainFieldNames[6] = Txt_First_name;
   Usr_UsrDatMainFieldNames[7] = Txt_HIERARCHY_SINGUL_Abc[Hie_INS];
  }

/*****************************************************************************/
/************ Write header with main field names of user's data **************/
/*****************************************************************************/

void Usr_WriteHeaderFieldsUsrDat (bool PutCheckBoxToSelectUsr)
  {
   unsigned NumCol;

   HTM_TR_Begin (NULL);

      /***** First column used for selection *****/
      if (PutCheckBoxToSelectUsr)
	 HTM_TH_Span (NULL,HTM_HEAD_LEFT,1,1,"BG_HIGHLIGHT");

      /***** Columns for user's data fields *****/
      for (NumCol = 0;
	   NumCol < Usr_NUM_MAIN_FIELDS_DATA_USR;
	   NumCol++)
	 if (NumCol != 2 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
	    HTM_TH_Span (Usr_UsrDatMainFieldNames[NumCol],HTM_HEAD_LEFT,1,1,"BG_HIGHLIGHT");

   HTM_TR_End ();
  }

/*****************************************************************************/
/************************** List guests' main data ***************************/
/*****************************************************************************/

static void Usr_ListMainDataGsts (bool PutCheckBoxToSelectUsr)
  {
   unsigned NumUsr;
   struct Usr_Data UsrDat;

   if (Gbl.Usrs.LstUsrs[Rol_GST].NumUsrs)
     {
      /***** Initialize field names *****/
      Usr_SetUsrDatMainFieldNames ();

      /***** Put a row to select all users *****/
      if (PutCheckBoxToSelectUsr)
         Usr_PutCheckboxToSelectAllUsers (&Gbl.Usrs.Selected,Rol_GST);

      /***** Heading row with column names *****/
      Usr_WriteHeaderFieldsUsrDat (PutCheckBoxToSelectUsr);	// Columns for the data

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

	 /***** List guests' data *****/
	 for (NumUsr = 0, The_ResetRowColor ();
	      NumUsr < Gbl.Usrs.LstUsrs[Rol_GST].NumUsrs;
	      NumUsr++, The_ChangeRowColor ())
	   {
	    /* Copy user's basic data from list */
	    Usr_CopyBasicUsrDataFromList (&UsrDat,&Gbl.Usrs.LstUsrs[Rol_GST].Lst[NumUsr]);

	    /* Get list of user's IDs */
	    ID_GetListIDsFromUsrCod (&UsrDat);

	    /* Show row for this guest */
	    Usr_WriteRowUsrMainData (NumUsr + 1,&UsrDat,true,Rol_GST,
				     &Gbl.Usrs.Selected);
	   }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else        // Gbl.Usrs.LstUsrs[Rol_GST].NumUsrs == 0
      /***** Show warning indicating no guests found *****/
      Usr_ShowWarningNoUsersFound (Rol_GST);

   /***** Free memory for guests list *****/
   Usr_FreeUsrsList (Rol_GST);
  }

/*****************************************************************************/
/*************************** List main students' data ************************/
/*****************************************************************************/

static void Usr_ListMainDataStds (bool PutCheckBoxToSelectUsr)
  {
   unsigned NumUsr;
   char *GroupNames;
   struct Usr_Data UsrDat;

   /***** Initialize field names *****/
   Usr_SetUsrDatMainFieldNames ();

   GroupNames = NULL;        // To avoid warning

   if (Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs)
     {
      /***** Allocate memory for the string with the list of group names where student belongs to *****/
      if ((GroupNames = malloc (Gbl.Crs.Grps.GrpTypes.NumGrpsTotal *
                                (Grp_MAX_BYTES_GROUP_NAME + 3))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Begin table with list of students *****/
      if (!Gbl.Crs.Grps.AllGrps)
        {
         HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"%u\" class=\"TIT CM\"",
			  1 + Usr_NUM_MAIN_FIELDS_DATA_USR);
	       Grp_WriteNamesOfSelectedGrps ();
	    HTM_TD_End ();
         HTM_TR_End ();
        }

      /***** Put a row to select all users *****/
      if (PutCheckBoxToSelectUsr)
	 Usr_PutCheckboxToSelectAllUsers (&Gbl.Usrs.Selected,Rol_STD);

      /***** Heading row with column names *****/
      Usr_WriteHeaderFieldsUsrDat (PutCheckBoxToSelectUsr);	// Columns for the data

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List students' data *****/
      for (NumUsr = 0, The_ResetRowColor ();
           NumUsr < Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs;
           NumUsr++, The_ChangeRowColor ())
        {
	 /* Copy user's basic data from list */
         Usr_CopyBasicUsrDataFromList (&UsrDat,&Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr]);

	 /* Get list of user's IDs */
         ID_GetListIDsFromUsrCod (&UsrDat);

         /* Show row for this student */
         Usr_WriteRowUsrMainData (NumUsr + 1,&UsrDat,
                                  PutCheckBoxToSelectUsr,Rol_STD,
				  &Gbl.Usrs.Selected);
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** Free memory used by the string with the list of group names where student belongs to *****/
      free (GroupNames);
     }
   else        // Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs == 0
      /***** Show warning indicating no students found *****/
      Usr_ShowWarningNoUsersFound (Rol_STD);

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (Rol_STD);
  }

/*****************************************************************************/
/************************** List main teachers' data *************************/
/*****************************************************************************/
// Role can be:
// - Rol_NET
// - Rol_TCH

static void Usr_ListMainDataTchs (Rol_Role_t Role,
				  bool PutCheckBoxToSelectUsr)
  {
   unsigned NumCol;
   unsigned NumUsr;
   struct Usr_Data UsrDat;

   if (Gbl.Usrs.LstUsrs[Role].NumUsrs)
     {
      /***** Put a row to select all users *****/
      if (PutCheckBoxToSelectUsr)
	 Usr_PutCheckboxToSelectAllUsers (&Gbl.Usrs.Selected,Role);

      /***** Heading row with column names *****/
      /* Begin row */
      HTM_TR_Begin (NULL);

	 /* First column used for selection  */
	 if (PutCheckBoxToSelectUsr)
	    HTM_TH_Span (NULL,HTM_HEAD_LEFT,1,1,"BG_HIGHLIGHT");

	 /* Columns for the data */
	 for (NumCol = 0;
	      NumCol < Usr_NUM_MAIN_FIELDS_DATA_USR;
	      NumCol++)
	    if (NumCol != 2 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
	       HTM_TH_Span (Usr_UsrDatMainFieldNames[NumCol],HTM_HEAD_LEFT,1,1,"BG_HIGHLIGHT");

      /* End row */
      HTM_TR_End ();

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List teachers' data *****/
      for (NumUsr = 0, The_ResetRowColor ();
           NumUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs;
           NumUsr++, The_ChangeRowColor ())
        {
	 /* Copy user's basic data from list */
         Usr_CopyBasicUsrDataFromList (&UsrDat,&Gbl.Usrs.LstUsrs[Role].Lst[NumUsr]);

	 /* Get list of user's IDs */
         ID_GetListIDsFromUsrCod (&UsrDat);

         /* Show row for this teacher */
	 Usr_WriteRowUsrMainData (NumUsr + 1,&UsrDat,
	                          PutCheckBoxToSelectUsr,Role,
				  &Gbl.Usrs.Selected);
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
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
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Email;
   extern const char *Txt_Department;
   extern const char *Txt_Office;
   extern const char *Txt_Phone;
   extern const char *Txt_Date_of_birth;
   unsigned NumColumnsCommonCard;
   unsigned NumCol;
   unsigned NumUsr;
   struct Usr_Data UsrDat;
   const char *FieldNames[Usr_NUM_ALL_FIELDS_DATA_GST];

   /***** Initialize field names *****/
   FieldNames[ 0] = Txt_Photo;
   FieldNames[ 1] = Txt_ID;
   FieldNames[ 2] = Txt_Surname_1;
   FieldNames[ 3] = Txt_Surname_2;
   FieldNames[ 4] = Txt_First_name;
   FieldNames[ 5] = Txt_Email;
   FieldNames[ 6] = Txt_HIERARCHY_SINGUL_Abc[Hie_INS];
   FieldNames[ 7] = Txt_HIERARCHY_SINGUL_Abc[Hie_CTR];
   FieldNames[ 8] = Txt_Department;
   FieldNames[ 9] = Txt_Office;
   FieldNames[10] = Txt_Phone;
   FieldNames[11] = Txt_Phone;
   FieldNames[12] = Txt_Phone;
   FieldNames[13] = Txt_Date_of_birth;

   /***** Get and update type of list,
          number of columns in class photo
          and preference about viewing photos *****/
   Set_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Sco_SetAllowedScopesForListingGuests ();
   Sco_GetScope ("ScopeUsr",Hie_SYS);

   /****** Get list of guests ******/
   Usr_GetGstsLst (Gbl.Scope.Current);

   if (Gbl.Usrs.LstUsrs[Rol_GST].NumUsrs)
     {
      /***** Set number of columns *****/
      NumColumnsCommonCard = Usr_NUM_ALL_FIELDS_DATA_GST;

      /***** Begin table with list of guests *****/
      HTM_TABLE_BeginWide ();

	 /* Begin row */
	 HTM_TR_Begin (NULL);

	    /* Columns for the data */
	    for (NumCol = (Gbl.Usrs.Listing.WithPhotos ? 0 :
							 1);
		 NumCol < NumColumnsCommonCard;
		 NumCol++)
	       HTM_TH_Span (FieldNames[NumCol],HTM_HEAD_LEFT,1,1,"BG_HIGHLIGHT");

	 /* End row */
	 HTM_TR_End ();

	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&UsrDat);

	 /***** List guests' data *****/
	 for (NumUsr = 0, The_ResetRowColor ();
	      NumUsr < Gbl.Usrs.LstUsrs[Rol_GST].NumUsrs; )
	   {
	    UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Rol_GST].Lst[NumUsr].UsrCod;
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							 Usr_DONT_GET_PREFS,
							 Usr_DONT_GET_ROLE_IN_CRS))
	      {
	       UsrDat.Roles.InCurrentCrs = Rol_GST;	// We know the user's role.
							// It is not necessary to retrieve
							// his/her role from database.
	       UsrDat.Accepted = false;	// Guests have no courses,...
					// ...so they have not accepted...
					// ...inscription in any course
	       NumUsr++;
	       Usr_WriteRowGstAllData (&UsrDat);

	       The_ChangeRowColor ();
	      }
	   }

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else        // Gbl.Usrs.LstUsrs[Rol_GST].NumUsrs == 0
      /***** Show warning indicating no guests found *****/
      Usr_ShowWarningNoUsersFound (Rol_GST);

   /***** Free memory for guests list *****/
   Usr_FreeUsrsList (Rol_GST);
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
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Phone;
   extern const char *Txt_Date_of_birth;
   extern const char *Txt_Group;
   extern const char *Txt_RECORD_FIELD_VISIBILITY_RECORD[Rec_NUM_TYPES_VISIBILITY];
   unsigned NumColsCommonRecord;
   unsigned NumColsRecordAndGroups;
   unsigned NumColsTotal;
   unsigned NumCol;
   unsigned NumUsr;
   char *GroupNames;
   unsigned NumGrpTyp,NumField;
   struct Usr_Data UsrDat;
   const char *FieldNames[Usr_NUM_ALL_FIELDS_DATA_STD];
   size_t Length;

   /***** Initialize field names *****/
   FieldNames[0] = Txt_Photo;
   FieldNames[1] = Txt_ID;
   FieldNames[2] = Txt_Surname_1;
   FieldNames[3] = Txt_Surname_2;
   FieldNames[4] = Txt_First_name;
   FieldNames[5] = Txt_Email;
   FieldNames[6] = Txt_HIERARCHY_SINGUL_Abc[Hie_INS];
   FieldNames[7] = Txt_Phone;
   FieldNames[8] = Txt_Phone;
   FieldNames[9] = Txt_Date_of_birth;

   GroupNames = NULL;        // To avoid warning

   /***** Get and update type of list,
          number of columns in class photo
          and preference about viewing photos *****/
   Set_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Sco_SetAllowedScopesForListingStudents ();
   Sco_GetScope ("ScopeUsr",Hie_CRS);

   /***** If the scope is the current course... *****/
   if (Gbl.Scope.Current == Hie_CRS)
     {
      /* Get list of groups types and groups in current course
         This is necessary to show columns with group selection */
      Grp_GetListGrpTypesInCurrentCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

      /* Get groups to show */
      Grp_GetParCodsSeveralGrpsToShowUsrs ();
     }

   /****** Get list of students in current course ******/
   Usr_GetListUsrs (Gbl.Scope.Current,Rol_STD);

   if (Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs)
     {
      if (Gbl.Scope.Current == Hie_CRS)
         /***** Get list of record fields in current course *****/
         Rec_GetListRecordFieldsInCurrentCrs ();

      /***** Set number of columns *****/
      NumColsCommonRecord = Usr_NUM_ALL_FIELDS_DATA_STD;
      if (Gbl.Scope.Current == Hie_CRS)
        {
         NumColsRecordAndGroups = NumColsCommonRecord + Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
         NumColsTotal = NumColsRecordAndGroups + Gbl.Crs.Records.LstFields.Num;
        }
      else
         NumColsTotal = NumColsRecordAndGroups = NumColsCommonRecord;

      /***** Allocate memory for the string with the list of group names where student belongs to *****/
      if (Gbl.Scope.Current == Hie_CRS)
	{
	 Length = (size_t) (Grp_MAX_BYTES_GROUP_NAME + 2) *
		  (size_t) Gbl.Crs.Grps.GrpTypes.NumGrpsTotal;
         if ((GroupNames = malloc (Length + 1)) == NULL)
            Err_NotEnoughMemoryExit ();
	}

      /***** Begin table with list of students *****/
      HTM_TABLE_BeginWide ();

	 if (!Gbl.Crs.Grps.AllGrps)
	   {
	    HTM_TR_Begin (NULL);
	       HTM_TD_Begin ("colspan=\"%u\" class=\"TIT CM\"",NumColsTotal);
		  Grp_WriteNamesOfSelectedGrps ();
	       HTM_TD_End ();
	    HTM_TR_End ();
	   }

	 /***** Heading row with column names *****/
	 /* Begin row */
	 HTM_TR_Begin (NULL);

	    /* 1. Columns for the data */
	    for (NumCol = (Gbl.Usrs.Listing.WithPhotos ? 0 :
							 1);
		 NumCol < NumColsCommonRecord;
		 NumCol++)
	       HTM_TH_Span (FieldNames[NumCol],HTM_HEAD_LEFT,1,1,"BG_HIGHLIGHT");

	    /* 2. Columns for the groups */
	    if (Gbl.Scope.Current == Hie_CRS)
	      {
	       if (Gbl.Crs.Grps.GrpTypes.NumGrpTypes)
		  for (NumGrpTyp = 0;
		       NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
		       NumGrpTyp++)
		     if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)         // If current course tiene groups of este type
		       {
			HTM_TH_Span_Begin (HTM_HEAD_LEFT,1,1,"BG_HIGHLIGHT");
			   HTM_TxtF ("%s %s",
				     Txt_Group,
				     Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName);
			HTM_TH_End ();
		       }

	       if (Gbl.Crs.Records.LstFields.Num)
		 {
		  /* 3. Names of record fields that depend on the course */
		  for (NumField = 0;
		       NumField < Gbl.Crs.Records.LstFields.Num;
		       NumField++)
		     HTM_TH_Span (Gbl.Crs.Records.LstFields.Lst[NumField].Name,HTM_HEAD_LEFT,1,1,"BG_HIGHLIGHT");

		  /* 4. Visibility type for the record fields that depend on the course, in other row */
		  HTM_TR_End ();

		  HTM_TR_Begin (NULL);

		     for (NumCol = 0;
			  NumCol < NumColsRecordAndGroups;
			  NumCol++)
			if (NumCol != 1 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want it in listing
			  {
			   HTM_TD_Begin ("class=\"LM BG_HIGHLIGHT\"");
			   HTM_TD_End ();
			  }
		     for (NumField = 0;
			  NumField < Gbl.Crs.Records.LstFields.Num;
			  NumField++)
		       {
			HTM_TH_Span_Begin (HTM_HEAD_LEFT,1,1,"BG_HIGHLIGHT");
			   HTM_TxtF ("(%s)",
			             Txt_RECORD_FIELD_VISIBILITY_RECORD[Gbl.Crs.Records.LstFields.Lst[NumField].Visibility]);
			HTM_TH_End ();
		       }
		 }
	      }

	 /* End row */
	 HTM_TR_End ();

	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&UsrDat);

	 /***** List students' data *****/
	 for (NumUsr = 0, The_ResetRowColor ();
	      NumUsr < Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs; )
	   {
	    UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].UsrCod;
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							 Usr_DONT_GET_PREFS,
							 Usr_DONT_GET_ROLE_IN_CRS))
	      {
	       UsrDat.Roles.InCurrentCrs = Rol_STD;	// We know the user's role.
							// It is not necessary to retrieve
							// his/her role from database.
	       UsrDat.Accepted = Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].Accepted;
	       NumUsr++;
	       Usr_WriteRowStdAllData (&UsrDat,GroupNames);

	       The_ChangeRowColor ();
	      }
	   }

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Free memory used by the string with the list of group names where student belongs to *****/
      if (Gbl.Scope.Current == Hie_CRS)
         free (GroupNames);
     }
   else        // Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs == 0
      /***** Show warning indicating no students found *****/
      Usr_ShowWarningNoUsersFound (Rol_STD);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (Rol_STD);

   /***** Free list of groups types and groups in current course *****/
   Grp_FreeListGrpTypesAndGrps ();
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
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Email;
   extern const char *Txt_Department;
   extern const char *Txt_Office;
   extern const char *Txt_Phone;
   unsigned NumUsrs;
   unsigned NumColumns;
   const char *FieldNames[Usr_NUM_ALL_FIELDS_DATA_TCH];

   /***** Initialize field names *****/
   FieldNames[ 0] = Txt_Photo;
   FieldNames[ 1] = Txt_ID;
   FieldNames[ 2] = Txt_Surname_1;
   FieldNames[ 3] = Txt_Surname_2;
   FieldNames[ 4] = Txt_First_name;
   FieldNames[ 5] = Txt_Email;
   FieldNames[ 6] = Txt_HIERARCHY_SINGUL_Abc[Hie_INS];
   FieldNames[ 7] = Txt_HIERARCHY_SINGUL_Abc[Hie_CTR];
   FieldNames[ 8] = Txt_Department;
   FieldNames[ 9] = Txt_Office;
   FieldNames[10] = Txt_Phone;

   /***** Get and update type of list,
          number of columns in class photo
          and preference about viewing photos *****/
   Set_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Gbl.Scope.Allowed = 1 << Hie_SYS |
	               1 << Hie_CTY |
                       1 << Hie_INS |
                       1 << Hie_CTR |
                       1 << Hie_DEG |
                       1 << Hie_CRS;
   Sco_GetScope ("ScopeUsr",Hie_CRS);

   /***** Get list of teachers *****/
   Usr_GetListUsrs (Gbl.Scope.Current,Rol_NET);	// Non-editing teachers
   Usr_GetListUsrs (Gbl.Scope.Current,Rol_TCH);	// Teachers
   if (Gbl.Scope.Current == Hie_CRS)
      NumUsrs = Gbl.Usrs.LstUsrs[Rol_NET].NumUsrs +
		Gbl.Usrs.LstUsrs[Rol_TCH].NumUsrs;
   else
      NumUsrs = Enr_GetNumUsrsInCrss (Gbl.Scope.Current,
				     (Gbl.Scope.Current == Hie_CTY ? Gbl.Hierarchy.Node[Hie_CTY].HieCod :
				     (Gbl.Scope.Current == Hie_INS ? Gbl.Hierarchy.Node[Hie_INS].HieCod :
				     (Gbl.Scope.Current == Hie_CTR ? Gbl.Hierarchy.Node[Hie_CTR].HieCod :
				     (Gbl.Scope.Current == Hie_DEG ? Gbl.Hierarchy.Node[Hie_DEG].HieCod :
				     (Gbl.Scope.Current == Hie_CRS ? Gbl.Hierarchy.Node[Hie_CRS].HieCod :
								     -1L))))),
				      1 << Rol_NET |
				      1 << Rol_TCH);

   if (NumUsrs)
     {
      /***** Initialize number of columns *****/
      NumColumns = Usr_NUM_ALL_FIELDS_DATA_TCH;

      /***** Begin table with lists of teachers *****/
      HTM_TABLE_BeginWide ();

	 /***** List teachers and non-editing teachers *****/
	 Usr_ListRowsAllDataTchs (Rol_TCH,FieldNames,NumColumns);
	 Usr_ListRowsAllDataTchs (Rol_NET,FieldNames,NumColumns);

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else        // NumUsrs == 0
      /***** Show warning indicating no teachers found *****/
      Usr_ShowWarningNoUsersFound (Rol_TCH);

   /***** Free memory for teachers lists *****/
   Usr_FreeUsrsList (Rol_TCH);	// Teachers
   Usr_FreeUsrsList (Rol_NET);	// Non-editing teachers
  }

/*****************************************************************************/
/*********************** List all teachers' data rows ************************/
/*****************************************************************************/

static void Usr_ListRowsAllDataTchs (Rol_Role_t Role,
                                     const char *FieldNames[Usr_NUM_ALL_FIELDS_DATA_TCH],
                                     unsigned NumColumns)
  {
   unsigned NumCol;
   struct Usr_Data UsrDat;
   unsigned NumUsr;

   /***** Heading row *****/
   HTM_TR_Begin (NULL);

      for (NumCol = (Gbl.Usrs.Listing.WithPhotos ? 0 :
						   1);
	   NumCol < NumColumns;
	   NumCol++)
	 HTM_TH_Span (FieldNames[NumCol],HTM_HEAD_LEFT,1,1,"BG_HIGHLIGHT");

   HTM_TR_End ();

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** List data of teachers *****/
   for (NumUsr = 0, The_ResetRowColor ();
	NumUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs; )
     {
      UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].UsrCod;
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
                                                   Usr_DONT_GET_PREFS,
                                                   Usr_DONT_GET_ROLE_IN_CRS))
	{
	 UsrDat.Roles.InCurrentCrs = Role;	// We know the user's role.
						// It is not necessary to retrieve
						// his/her role from database.
	 UsrDat.Accepted = Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Accepted;
	 NumUsr++;
	 Usr_WriteRowTchAllData (&UsrDat);

	 The_ChangeRowColor ();
	}
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/****************************** List users found *****************************/
/*****************************************************************************/
// Returns number of users found

unsigned Usr_ListUsrsFound (Rol_Role_t Role,
                            const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1])
  {
   extern const char *Txt_user[Usr_NUM_SEXS];
   extern const char *Txt_users[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   unsigned NumUsrs;
   unsigned NumUsr;
   char *Title;
   struct Usr_Data UsrDat;
   Usr_Sex_t Sex;
   struct Usr_InList *UsrInList;

   /***** Initialize field names *****/
   Usr_SetUsrDatMainFieldNames ();

   /***** Create temporary table with candidate users *****/
   // Search is faster (aproximately x2) using temporary tables
   Usr_DB_CreateTmpTableAndSearchCandidateUsrs (SearchQuery);

   /***** Search for users *****/
   Usr_SearchListUsrs (Role);
   if ((NumUsrs = Gbl.Usrs.LstUsrs[Role].NumUsrs))
     {
      /***** Begin box with number of users found *****/
      Sex = Usr_GetSexOfUsrsLst (Role);
      if (asprintf (&Title,"%u %s",NumUsrs,
				   (Role == Rol_UNK) ? (NumUsrs == 1 ? Txt_user[Sex] :
								       Txt_users[Sex]) :
						       (NumUsrs == 1 ? Txt_ROLES_SINGUL_abc[Role][Sex] :
								       Txt_ROLES_PLURAL_abc[Role][Sex])) < 0)
	 Err_NotEnoughMemoryExit ();
      Box_BoxBegin (Title,NULL,NULL,NULL,Box_NOT_CLOSABLE);
      free (Title);

         HTM_TABLE_Begin ("TBL_SCROLL");

	    /***** Heading row with column names *****/
	    Gbl.Usrs.Listing.WithPhotos = true;
	    Usr_WriteHeaderFieldsUsrDat (false);	// Columns for the data

	    /***** Initialize structure with user's data *****/
	    Usr_UsrDataConstructor (&UsrDat);

	    /***** List data of users *****/
	    for (NumUsr = 0, The_ResetRowColor ();
		 NumUsr < NumUsrs;
		 NumUsr++, The_ChangeRowColor ())
	      {
	       UsrInList = &Gbl.Usrs.LstUsrs[Role].Lst[NumUsr];

	       /* Copy user's basic data from list */
	       Usr_CopyBasicUsrDataFromList (&UsrDat,UsrInList);

	       /* Get list of user's IDs */
	       ID_GetListIDsFromUsrCod (&UsrDat);

	       /* Write data of this user */
	       Usr_WriteRowUsrMainData (NumUsr + 1,&UsrDat,false,Role,
					&Gbl.Usrs.Selected);

	       /* Write all courses this user belongs to */
	       if (Role != Rol_GST &&				// Guests do not belong to any course
		   Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)		// Only admins can view the courses
		 {
		  HTM_TR_Begin (NULL);

		     HTM_TD_Begin ("colspan=\"2\" class=\"%s\"",
				   The_GetColorRows ());
		     HTM_TD_End ();

		     HTM_TD_Begin ("colspan=\"%u\" class=\"%s\"",
				   Usr_NUM_MAIN_FIELDS_DATA_USR-2,
				   The_GetColorRows ());
			if (Role == Rol_UNK)
			  {
			   Crs_GetAndWriteCrssOfAUsr (&UsrDat,Rol_TCH);
			   Crs_GetAndWriteCrssOfAUsr (&UsrDat,Rol_NET);
			   Crs_GetAndWriteCrssOfAUsr (&UsrDat,Rol_STD);
			  }
			else
			   Crs_GetAndWriteCrssOfAUsr (&UsrDat,Role);
		     HTM_TD_End ();

		  HTM_TR_End ();
		 }
	      }

	    /***** Free memory used for user's data *****/
	    Usr_UsrDataDestructor (&UsrDat);

	 HTM_TABLE_End ();
      Box_BoxEnd ();
     }

   /***** Free memory for teachers list *****/
   Usr_FreeUsrsList (Role);

   /***** Drop temporary table with candidate users *****/
   Usr_DB_DropTmpTableWithCandidateUsrs ();

   return NumUsrs;
  }

/*****************************************************************************/
/**************************** List administrators ****************************/
/*****************************************************************************/

void Usr_ListDataAdms (void)
  {
   extern const char *Hlp_USERS_Administrators;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Scope;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Photo;
   extern const char *Txt_ID;
   extern const char *Txt_Surname_1;
   extern const char *Txt_Surname_2;
   extern const char *Txt_First_name;
   extern const char *Txt_Email;
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   unsigned NumCol;
   unsigned NumUsr;
   struct Usr_Data UsrDat;
   const char *FieldNames[Usr_NUM_MAIN_FIELDS_DATA_ADM];

   /***** Put contextual links *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 /***** Contextual menu *****/
	 Mnu_ContextMenuBegin ();
	    if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	      {
	       Usr_PutLinkToSeeGuests ();		// List guests
	       Dup_PutLinkToListDupUsrs ();		// List possible duplicate users
	      }
	    Enr_PutLinkToAdminOneUsr (ActReqMdfOneOth);	// Admin one user
	    if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	       Enr_PutLinkToRemOldUsrs ();		// Remove old users
	 Mnu_ContextMenuEnd ();
	 break;
      default:
	 break;
     }

   /***** Initialize field names *****/
   FieldNames[0] = Txt_No_INDEX;
   FieldNames[1] = Txt_Photo;
   FieldNames[2] = Txt_ID;
   FieldNames[3] = Txt_Surname_1;
   FieldNames[4] = Txt_Surname_2;
   FieldNames[5] = Txt_First_name;
   FieldNames[6] = Txt_HIERARCHY_SINGUL_Abc[Hie_INS];

   /***** Get and update type of list,
          number of columns in class photo
          and preference about viewing photos *****/
   Set_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Gbl.Scope.Allowed = 1 << Hie_SYS |
	               1 << Hie_CTY |
                       1 << Hie_INS |
                       1 << Hie_CTR |
                       1 << Hie_DEG;
   Sco_GetScope ("ScopeUsr",Hie_DEG);

   /***** Get list of administrators *****/
   Adm_GetAdmsLst (Gbl.Scope.Current);

   /***** Begin box with list of administrators *****/
   Box_BoxBegin (Txt_ROLES_PLURAL_Abc[Rol_DEG_ADM][Usr_SEX_UNKNOWN],NULL,NULL,
                 Hlp_USERS_Administrators,Box_NOT_CLOSABLE);

      /***** Form to select scope *****/
      HTM_DIV_Begin ("class=\"CM\"");
	 Frm_BeginForm (ActLstOth);
	    Set_PutParListWithPhotos ();
	    HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	       HTM_TxtColonNBSP (Txt_Scope);
	       Sco_PutSelectorScope ("ScopeUsr",HTM_SUBMIT_ON_CHANGE);
	    HTM_LABEL_End ();
	 Frm_EndForm ();
      HTM_DIV_End ();

      if (Gbl.Usrs.LstUsrs[Rol_DEG_ADM].NumUsrs)
	{
	 /****** Show photos? *****/
	 HTM_DIV_Begin ("class=\"PREF_CONT\"");
	    HTM_DIV_Begin ("class=\"PREF_OFF\"");
	       Frm_BeginForm (ActLstOth);
		  Sco_PutParCurrentScope (&Gbl.Scope.Current);
		  Usr_PutCheckboxListWithPhotos ();
	       Frm_EndForm ();
	    HTM_DIV_End ();
	 HTM_DIV_End ();

	 /***** Heading row with column names *****/
	 HTM_TABLE_Begin ("TBL_SCROLL");
	    HTM_TR_Begin (NULL);

	       for (NumCol = 0;
		    NumCol < Usr_NUM_MAIN_FIELDS_DATA_ADM;
		    NumCol++)
		  if (NumCol != 1 || Gbl.Usrs.Listing.WithPhotos)        // Skip photo column if I don't want this column
		     HTM_TH_Span (FieldNames[NumCol],HTM_HEAD_LEFT,1,1,"BG_HIGHLIGHT");

	    HTM_TR_End ();

	    /***** Initialize structure with user's data *****/
	    Usr_UsrDataConstructor (&UsrDat);

	    /***** List data of administrators *****/
	    for (NumUsr = 0, The_ResetRowColor ();
		 NumUsr < Gbl.Usrs.LstUsrs[Rol_DEG_ADM].NumUsrs; )
	      {
	       UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Rol_DEG_ADM].Lst[NumUsr].UsrCod;
	       if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							    Usr_DONT_GET_PREFS,
							    Usr_DONT_GET_ROLE_IN_CRS))
		 {
		  UsrDat.Accepted = Gbl.Usrs.LstUsrs[Rol_DEG_ADM].Lst[NumUsr].Accepted;
		  Usr_WriteRowAdmData (++NumUsr,&UsrDat);

		  The_ChangeRowColor ();
		 }
	      }

	    /***** Free memory used for user's data *****/
	    Usr_UsrDataDestructor (&UsrDat);

	 /***** End table *****/
	 HTM_TABLE_End ();
	}
      else        // Gbl.Usrs.LstUsrs[Rol_DEG_ADM].NumUsrs == 0
	 /***** Show warning indicating no admins found *****/
	 Usr_ShowWarningNoUsersFound (Rol_DEG_ADM);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free memory for administrators list *****/
   Usr_FreeUsrsList (Rol_DEG_ADM);
  }

/*****************************************************************************/
/********** Put a link (form) to show list or class photo of guests **********/
/*****************************************************************************/

static void Usr_PutLinkToSeeAdmins (void)
  {
   extern const char *Rol_Icons[Rol_NUM_ROLES];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   /***** Put form to list admins *****/
   Lay_PutContextualLinkIconText (ActLstOth,NULL,
                                  NULL,NULL,
				  Rol_Icons[Rol_DEG_ADM],Ico_BLACK,
				  Txt_ROLES_PLURAL_Abc[Rol_DEG_ADM][Usr_SEX_UNKNOWN],NULL);
  }

/*****************************************************************************/
/********** Put a link (form) to show list or class photo of guests **********/
/*****************************************************************************/

static void Usr_PutLinkToSeeGuests (void)
  {
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   /***** Put form to list guests *****/
   Lay_PutContextualLinkIconText (ActLstGst,NULL,
                                  NULL,NULL,
				  "users.svg",Ico_BLACK,
				  Txt_ROLES_PLURAL_Abc[Rol_GST][Usr_SEX_UNKNOWN],NULL);
  }

/*****************************************************************************/
/********************* Show list or class photo of guests ********************/
/*****************************************************************************/

void Usr_SeeGuests (void)
  {
   extern const char *Hlp_USERS_Guests;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Scope;
   bool ICanChooseOption[Usr_LIST_USRS_NUM_OPTIONS];
   Frm_PutForm_t PutForm;

   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
      Usr_PutLinkToSeeAdmins ();			// List admins
      Enr_PutLinkToAdminOneUsr (ActReqMdfOneOth);	// Admin one user
      if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	 Enr_PutLinkToRemOldUsrs ();			// Remove old users
   Mnu_ContextMenuEnd ();

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Set_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Sco_SetAllowedScopesForListingGuests ();
   Sco_GetScope ("ScopeUsr",Hie_SYS);

   /***** Get list of guests in current scope *****/
   Usr_GetGstsLst (Gbl.Scope.Current);

   /***** Begin box *****/
   Box_BoxBegin (Txt_ROLES_PLURAL_Abc[Rol_GST][Usr_SEX_UNKNOWN],
                 Usr_PutIconsListGsts,NULL,
		 Hlp_USERS_Guests,Box_NOT_CLOSABLE);

      /***** Form to select scope *****/
      if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	{
	 HTM_DIV_Begin ("class=\"CM\"");
	    Frm_BeginForm (ActLstGst);
	       Set_PutParsPrefsAboutUsrList ();
	       HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",
	                        The_GetSuffix ());
		  HTM_TxtColonNBSP (Txt_Scope);
		  Sco_PutSelectorScope ("ScopeUsr",HTM_SUBMIT_ON_CHANGE);
	       HTM_LABEL_End ();
	    Frm_EndForm ();
	 HTM_DIV_End ();
	}

      /***** Begin section with user list *****/
      HTM_SECTION_Begin (Usr_USER_LIST_SECTION_ID);

	 if (Gbl.Usrs.LstUsrs[Rol_GST].NumUsrs)
	   {
	    if (Usr_GetIfShowBigList (Gbl.Usrs.LstUsrs[Rol_GST].NumUsrs,
				      ActLstGst,Sco_PutParCurrentScope,&Gbl.Scope.Current,
				      NULL))
	      {
	       /***** Form to select type of list of users *****/
	       Usr_ShowFormsToSelectUsrListType (ActLstGst,Sco_PutParCurrentScope,&Gbl.Scope.Current,
						 NULL);

	       /***** Draw a class photo with guests *****/
	       if (Gbl.Usrs.Me.ListType == Set_USR_LIST_AS_CLASS_PHOTO)
		  Lay_WriteHeaderClassPhoto (Vie_VIEW);

	       /* Set options allowed */
	       PutForm = Usr_SetOptionsListUsrsAllowed (Rol_GST,ICanChooseOption) ? Frm_PUT_FORM :
										    Frm_DONT_PUT_FORM;

	       /* Begin form */
	       if (PutForm == Frm_PUT_FORM)
		  Frm_BeginForm (ActDoActOnSevGst);

	       /* Begin table */
	       HTM_TABLE_Begin ("TBL_SCROLL");

		  /* Draw the classphoto/list */
		  switch (Gbl.Usrs.Me.ListType)
		    {
		     case Set_USR_LIST_AS_CLASS_PHOTO:
			Usr_DrawClassPhoto (&Gbl.Usrs.Selected,Rol_GST,
					    Usr_CLASS_PHOTO_SEL_SEE,
					    PutForm);	// Put checkbox to select user?
			break;
		     case Set_USR_LIST_AS_LISTING:
			Usr_ListMainDataGsts (PutForm);	// Put checkbox to select user?
			break;
		     default:
			break;
		    }

	       /* End table */
	       HTM_TABLE_End ();

	       /***** Which action, show records, follow...? *****/
	       if (PutForm == Frm_PUT_FORM)
		 {
		     Usr_PutOptionsListUsrs (ICanChooseOption);
		  Frm_EndForm ();
		 }
	      }
	   }
	 else	// Gbl.Usrs.LstUsrs[Rol_GST].NumUsrs == 0
	    /***** Show warning indicating no guests found *****/
	    Usr_ShowWarningNoUsersFound (Rol_GST);

      /***** End section with user list *****/
      HTM_SECTION_End ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free memory for guests list *****/
   Usr_FreeUsrsList (Rol_GST);
  }

/*****************************************************************************/
/******************** Show list or class photo of students *******************/
/*****************************************************************************/

void Usr_SeeStudents (void)
  {
   extern const char *Hlp_USERS_Students;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Scope;
   bool ICanChooseOption[Usr_LIST_USRS_NUM_OPTIONS];
   Frm_PutForm_t PutForm;

   /***** Put contextual links *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 /***** Contextual menu *****/
         Mnu_ContextMenuBegin ();
	    Enr_PutLinkToAdminOneUsr (ActReqMdfOneStd);	// Admin one student
	    if (Gbl.Hierarchy.Level == Hie_CRS &&	// Course selected
		Gbl.Usrs.Me.Role.Logged != Rol_STD)	// Teacher or admin
	      {
	       Enr_PutLinkToAdminSeveralUsrs (Rol_STD);	// Admin several students
	       Rec_PutLinkToEditRecordFields ();	// Edit record fields
	      }
         Mnu_ContextMenuEnd ();
	 break;
      default:
	 break;
     }

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Set_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Sco_SetAllowedScopesForListingStudents ();
   Sco_GetScope ("ScopeUsr",Hie_CRS);

   /***** Get groups to show ******/
   if (Gbl.Scope.Current == Hie_CRS)
      Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get list of students *****/
   Usr_GetListUsrs (Gbl.Scope.Current,Rol_STD);

   /***** Begin box *****/
   Box_BoxBegin (Txt_ROLES_PLURAL_Abc[Rol_STD][Usr_SEX_UNKNOWN],
                 Usr_PutIconsListStds,NULL,
		 Hlp_USERS_Students,Box_NOT_CLOSABLE);

      /***** Form to select scope *****/
      switch (Gbl.Usrs.Me.Role.Logged)
	{
	 case Rol_DEG_ADM:
	 case Rol_CTR_ADM:
	 case Rol_INS_ADM:
	 case Rol_SYS_ADM:
	    HTM_DIV_Begin ("class=\"CM\"");
	       Frm_BeginForm (ActLstStd);
		  Set_PutParsPrefsAboutUsrList ();
		  HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",
		                   The_GetSuffix ());
		     HTM_TxtColonNBSP (Txt_Scope);
		     Sco_PutSelectorScope ("ScopeUsr",HTM_SUBMIT_ON_CHANGE);
		  HTM_LABEL_End ();
	       Frm_EndForm ();
	    HTM_DIV_End ();
	    break;
	 default:
	    break;
	}

      /***** Form to select groups *****/
      if (Gbl.Scope.Current == Hie_CRS)
	 Grp_ShowFormToSelectSeveralGroups (ActLstStd,Sco_PutParCurrentScope,
					    &Gbl.Scope.Current,NULL);

      /***** Begin section with user list *****/
      HTM_SECTION_Begin (Usr_USER_LIST_SECTION_ID);

	 if (Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs)
	   {
	    if (Usr_GetIfShowBigList (Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs,
				      ActLstStd,Sco_PutParCurrentScope,&Gbl.Scope.Current,
				      NULL))
	      {
	       /***** Form to select type of list of users *****/
	       Usr_ShowFormsToSelectUsrListType (ActLstStd,Sco_PutParCurrentScope,&Gbl.Scope.Current,
						 NULL);

	       /***** Draw a class photo with students of the course *****/
	       if (Gbl.Usrs.Me.ListType == Set_USR_LIST_AS_CLASS_PHOTO)
		  Lay_WriteHeaderClassPhoto (Vie_VIEW);

	       /* Set options allowed */
	       PutForm = Usr_SetOptionsListUsrsAllowed (Rol_STD,ICanChooseOption) ? Frm_PUT_FORM :
										    Frm_DONT_PUT_FORM;

	       /* Begin form */
	       if (PutForm == Frm_PUT_FORM)
		 {
		  Frm_BeginForm (ActDoActOnSevStd);
		     Grp_PutParsCodGrps ();
		 }

	       /* Begin table */
	       HTM_TABLE_Begin ("TBL_SCROLL");

		  /* Draw the classphoto/list */
		  switch (Gbl.Usrs.Me.ListType)
		    {
		     case Set_USR_LIST_AS_CLASS_PHOTO:
			Usr_DrawClassPhoto (&Gbl.Usrs.Selected,Rol_STD,
					    Usr_CLASS_PHOTO_SEL_SEE,
					    PutForm);	// Put checkbox to select user?
			break;
		     case Set_USR_LIST_AS_LISTING:
			Usr_ListMainDataStds (PutForm);	// Put checkbox to select user?
			break;
		     default:
			break;
		    }

	       /* End table */
	       HTM_TABLE_End ();

	       /***** Which action, show records, follow...? *****/
	       if (PutForm == Frm_PUT_FORM)
		 {
		     Usr_PutOptionsListUsrs (ICanChooseOption);
		  Frm_EndForm ();
		 }
	      }
	   }
	 else	// Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs == 0
	    /***** Show warning indicating no students found *****/
	    Usr_ShowWarningNoUsersFound (Rol_STD);

      /***** End section with user list *****/
      HTM_SECTION_End ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (Rol_STD);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/******************** Show list or class photo of teachers *******************/
/*****************************************************************************/

void Usr_SeeTeachers (void)
  {
   extern const char *Hlp_USERS_Teachers;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Scope;
   unsigned NumUsrs;
   bool ICanChooseOption[Usr_LIST_USRS_NUM_OPTIONS];
   Frm_PutForm_t PutForm;

   /***** Put contextual links *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 /***** Contextual menu *****/
	 Mnu_ContextMenuBegin ();
	    Enr_PutLinkToAdminOneUsr (ActReqMdfOneTch);	// Admin one teacher
	    if (Gbl.Hierarchy.Level == Hie_CRS &&	// Course selected
		Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)	// I am logged as admin
	      {
	       Enr_PutLinkToAdminSeveralUsrs (Rol_NET);	// Admin several non-editing teachers
	       Enr_PutLinkToAdminSeveralUsrs (Rol_TCH);	// Admin several teachers
	      }
	 Mnu_ContextMenuEnd ();
         break;
      default:
	 break;
     }

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Set_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Gbl.Scope.Allowed = 1 << Hie_SYS |
	               1 << Hie_CTY |
                       1 << Hie_INS |
                       1 << Hie_CTR |
                       1 << Hie_DEG |
                       1 << Hie_CRS;
   Sco_GetScope ("ScopeUsr",Hie_CRS);

   /***** Get groups to show ******/
   if (Gbl.Scope.Current == Hie_CRS)
      Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get lists of teachers *****/
   Usr_GetListUsrs (Gbl.Scope.Current,Rol_NET);	// Non-editing teachers
   Usr_GetListUsrs (Gbl.Scope.Current,Rol_TCH);	// Teachers
   if (Gbl.Scope.Current == Hie_CRS)
      NumUsrs = Gbl.Usrs.LstUsrs[Rol_NET].NumUsrs +
		Gbl.Usrs.LstUsrs[Rol_TCH].NumUsrs;
   else
      NumUsrs = Enr_GetNumUsrsInCrss (Gbl.Scope.Current,
				     (Gbl.Scope.Current == Hie_CTY ? Gbl.Hierarchy.Node[Hie_CTY].HieCod :
				     (Gbl.Scope.Current == Hie_INS ? Gbl.Hierarchy.Node[Hie_INS].HieCod :
				     (Gbl.Scope.Current == Hie_CTR ? Gbl.Hierarchy.Node[Hie_CTR].HieCod :
				     (Gbl.Scope.Current == Hie_DEG ? Gbl.Hierarchy.Node[Hie_DEG].HieCod :
				     (Gbl.Scope.Current == Hie_CRS ? Gbl.Hierarchy.Node[Hie_CRS].HieCod :
								     -1L))))),
				      1 << Rol_NET |
				      1 << Rol_TCH);

   /***** Begin box *****/
   Box_BoxBegin (Txt_ROLES_PLURAL_Abc[Rol_TCH][Usr_SEX_UNKNOWN],
                 Usr_PutIconsListTchs,NULL,
		 Hlp_USERS_Teachers,Box_NOT_CLOSABLE);

      /***** Form to select scope *****/
      HTM_DIV_Begin ("class=\"CM\"");
	 Frm_BeginForm (ActLstTch);
	    Set_PutParsPrefsAboutUsrList ();
	    HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",
	                     The_GetSuffix ());
	       HTM_TxtColonNBSP (Txt_Scope);
	       Sco_PutSelectorScope ("ScopeUsr",HTM_SUBMIT_ON_CHANGE);
	    HTM_LABEL_End ();
	 Frm_EndForm ();
      HTM_DIV_End ();

      /***** Form to select groups *****/
      if (Gbl.Scope.Current == Hie_CRS)
	 Grp_ShowFormToSelectSeveralGroups (ActLstTch,Sco_PutParCurrentScope,
					    &Gbl.Scope.Current,NULL);

      /***** Begin section with user list *****/
      HTM_SECTION_Begin (Usr_USER_LIST_SECTION_ID);

	 if (NumUsrs)
	   {
	    if (Usr_GetIfShowBigList (NumUsrs,
				      ActLstTch,Sco_PutParCurrentScope,&Gbl.Scope.Current,
				      NULL))
	      {
	       /***** Form to select type of list of users *****/
	       Usr_ShowFormsToSelectUsrListType (ActLstTch,Sco_PutParCurrentScope,&Gbl.Scope.Current,
						 NULL);

	       /***** Draw a class photo with teachers of the course *****/
	       if (Gbl.Usrs.Me.ListType == Set_USR_LIST_AS_CLASS_PHOTO)
		  Lay_WriteHeaderClassPhoto (Vie_VIEW);

	       /* Set options allowed */
	       PutForm = Usr_SetOptionsListUsrsAllowed (Rol_TCH,ICanChooseOption) ? Frm_PUT_FORM :
										    Frm_DONT_PUT_FORM;

	       /* Begin form */
	       if (PutForm == Frm_PUT_FORM)
		 {
		  Frm_BeginForm (ActDoActOnSevTch);
		     Grp_PutParsCodGrps ();
		 }

	       /* Begin table */
	       HTM_TABLE_Begin ("TBL_SCROLL");

		  /***** Draw the classphoto/list  *****/
		  switch (Gbl.Usrs.Me.ListType)
		    {
		     case Set_USR_LIST_AS_CLASS_PHOTO:
			/* List teachers and non-editing teachers */
			Usr_DrawClassPhoto (&Gbl.Usrs.Selected,Rol_TCH,
					    Usr_CLASS_PHOTO_SEL_SEE,
					    PutForm);	// Put checkbox to select user?
			Usr_DrawClassPhoto (&Gbl.Usrs.Selected,Rol_NET,
					    Usr_CLASS_PHOTO_SEL_SEE,
					    PutForm);	// Put checkbox to select user?
			break;
		     case Set_USR_LIST_AS_LISTING:
			/* Initialize field names */
			Usr_SetUsrDatMainFieldNames ();

			/* List teachers and non-editing teachers */
			Usr_ListMainDataTchs (Rol_TCH,
					      PutForm);	// Put checkbox to select user?
			Usr_ListMainDataTchs (Rol_NET,
					      PutForm);	// Put checkbox to select user?
			break;
		     default:
			break;
		    }

	       /* End table */
	       HTM_TABLE_End ();

	       /***** Which action, show records, follow...? *****/
	       if (PutForm == Frm_PUT_FORM)
		 {
		     Usr_PutOptionsListUsrs (ICanChooseOption);
		  Frm_EndForm ();
		 }
	      }
	   }
	 else	// NumUsrs == 0
	    /***** Show warning indicating no teachers found *****/
	    Usr_ShowWarningNoUsersFound (Rol_TCH);

      /***** End section with user list *****/
      HTM_SECTION_End ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free memory for teachers lists *****/
   Usr_FreeUsrsList (Rol_TCH);	// Teachers
   Usr_FreeUsrsList (Rol_NET);	// Non-editing teachers

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/**************** Set allowed options to do with several users ***************/
/*****************************************************************************/
// Returns true if any option is allowed

static Frm_PutForm_t Usr_SetOptionsListUsrsAllowed (Rol_Role_t UsrsRole,
                                                    bool ICanChooseOption[Usr_LIST_USRS_NUM_OPTIONS])
  {
   Usr_ListUsrsOption_t Opt;

   /***** Check which options I can choose *****/
   /* Set default (I can not choose options) */
   for (Opt  = (Usr_ListUsrsOption_t) 1;	// Skip unknown option
	Opt <= (Usr_ListUsrsOption_t) (Usr_LIST_USRS_NUM_OPTIONS - 1);
	Opt++)
      ICanChooseOption[Opt] = false;

   /* Activate some options depending on users' role, on my role, etc. */
   switch (UsrsRole)
     {
      case Rol_GST:
	 ICanChooseOption[Usr_OPTION_RECORDS]    = (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
	 break;
      case Rol_STD:
	 ICanChooseOption[Usr_OPTION_RECORDS]    =
	 ICanChooseOption[Usr_OPTION_MESSAGE]    =
	 ICanChooseOption[Usr_OPTION_FOLLOW]     =
	 ICanChooseOption[Usr_OPTION_UNFOLLOW]   = (Gbl.Scope.Current == Hie_CRS &&
						    (Gbl.Usrs.Me.Role.Logged == Rol_STD ||
						     Gbl.Usrs.Me.Role.Logged == Rol_NET ||
						     Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
						     Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM));

         ICanChooseOption[Usr_OPTION_HOMEWORK]   =
         ICanChooseOption[Usr_OPTION_ATTENDANCE] =
         ICanChooseOption[Usr_OPTION_EMAIL]      = (Gbl.Scope.Current == Hie_CRS &&
						    (Gbl.Usrs.Me.Role.Logged == Rol_NET ||
						     Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
						     Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM));
	 break;
      case Rol_TCH:
	 ICanChooseOption[Usr_OPTION_RECORDS]    =
	 ICanChooseOption[Usr_OPTION_MESSAGE]    =
	 ICanChooseOption[Usr_OPTION_EMAIL]      =
	 ICanChooseOption[Usr_OPTION_FOLLOW]     =
	 ICanChooseOption[Usr_OPTION_UNFOLLOW]   = (Gbl.Scope.Current == Hie_CRS &&
						    (Gbl.Usrs.Me.Role.Logged == Rol_STD ||
						     Gbl.Usrs.Me.Role.Logged == Rol_NET ||
						     Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
						     Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM));
         ICanChooseOption[Usr_OPTION_HOMEWORK]   = (Gbl.Scope.Current == Hie_CRS &&
						    (Gbl.Usrs.Me.Role.Logged == Rol_NET ||
						     Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
						     Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM));
	 break;
      default:
	 return Frm_DONT_PUT_FORM;
     }

   /***** Count allowed options *****/
   for (Opt  = (Usr_ListUsrsOption_t) 1;	// Skip unknown option
	Opt <= (Usr_ListUsrsOption_t) (Usr_LIST_USRS_NUM_OPTIONS - 1);
	Opt++)
      if (ICanChooseOption[Opt])
	 return Frm_PUT_FORM;

   return Frm_DONT_PUT_FORM;
  }

/*****************************************************************************/
/*************** Put different options to do with several users **************/
/*****************************************************************************/
// Returns true if at least one action can be shown

static void Usr_PutOptionsListUsrs (const bool ICanChooseOption[Usr_LIST_USRS_NUM_OPTIONS])
  {
   extern const char *Txt_View_records;
   extern const char *Txt_View_homework;
   extern const char *Txt_View_attendance;
   extern const char *Txt_Send_message;
   extern const char *Txt_Create_email_message;
   extern const char *Txt_Follow;
   extern const char *Txt_Unfollow;
   extern const char *Txt_Continue;
   static const char **Label[Usr_LIST_USRS_NUM_OPTIONS] =
     {
      [Usr_OPTION_UNKNOWN   ] = NULL,
      [Usr_OPTION_RECORDS   ] = &Txt_View_records,
      [Usr_OPTION_HOMEWORK  ] = &Txt_View_homework,
      [Usr_OPTION_ATTENDANCE] = &Txt_View_attendance,
      [Usr_OPTION_MESSAGE   ] = &Txt_Send_message,
      [Usr_OPTION_EMAIL     ] = &Txt_Create_email_message,
      [Usr_OPTION_FOLLOW    ] = &Txt_Follow,
      [Usr_OPTION_UNFOLLOW  ] = &Txt_Unfollow,
     };
   Usr_ListUsrsOption_t Opt;

   /***** Get the selected option from form *****/
   Gbl.Usrs.Selected.Option = Usr_GetListUsrsOption (Usr_LIST_USRS_DEFAULT_OPTION);

   /***** Write list of options *****/
   /* Begin list of options */
   HTM_UL_Begin ("class=\"LIST_LEFT FORM_IN_%s\"",The_GetSuffix ());

      /* Show option items */
      for (Opt  = (Usr_ListUsrsOption_t) 1;	// Skip unknown option
	   Opt <= (Usr_ListUsrsOption_t) (Usr_LIST_USRS_NUM_OPTIONS - 1);
	   Opt++)
	 if (ICanChooseOption[Opt])
	    Usr_ShowOneListUsrsOption (Opt,*Label[Opt]);

   /* End list of options */
   HTM_UL_End ();

   /***** Put button to confirm *****/
   Btn_PutConfirmButton (Txt_Continue);
  }

/*****************************************************************************/
/************ Put start/end of action to register/remove one user ************/
/*****************************************************************************/

static void Usr_ShowOneListUsrsOption (Usr_ListUsrsOption_t ListUsrsAction,
                                       const char *Label)
  {
   HTM_LI_Begin (NULL);
      HTM_LABEL_Begin (NULL);
	 HTM_INPUT_RADIO ("ListUsrsAction",HTM_DONT_SUBMIT_ON_CLICK,
			  "value=\"%u\"%s",
			  (unsigned) ListUsrsAction,
			  ListUsrsAction == Gbl.Usrs.Selected.Option ? " checked=\"checked\"" :
				                                       "");
	 HTM_Txt (Label);
      HTM_LABEL_End ();
   HTM_LI_End ();
  }

/*****************************************************************************/
/********************** Do action on several students ************************/
/*****************************************************************************/

void Usr_DoActionOnSeveralUsrs1 (void)
  {
   extern const char *Txt_You_must_select_one_ore_more_users;

   /***** Get parameters from form *****/
   /* Get list of selected users */
   Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /* Check if there are selected users */
   if (Usr_CheckIfThereAreUsrsInListOfSelectedEncryptedUsrCods (&Gbl.Usrs.Selected))
     {
      /* Get the action to do */
      Gbl.Usrs.Selected.Option = Usr_GetListUsrsOption (Usr_OPTION_UNKNOWN);

      /***** Change action depending on my selection *****/
      Gbl.Action.Original = Gbl.Action.Act;	// To check if action changes

      switch (Gbl.Usrs.Selected.Option)
	{
	 case Usr_OPTION_RECORDS:
	    switch (Gbl.Action.Act)
	      {
	       case ActDoActOnSevGst:
		  Gbl.Action.Act = ActSeeRecSevGst;
		  break;
	       case ActDoActOnSevStd:
		  Gbl.Action.Act = ActSeeRecSevStd;
		  break;
	       case ActDoActOnSevTch:
		  Gbl.Action.Act = ActSeeRecSevTch;
		  break;
	       default:
		  break;
	      }
	    break;
	 case Usr_OPTION_HOMEWORK:
	    switch (Gbl.Action.Act)
	      {
	       case ActDoActOnSevStd:
	       case ActDoActOnSevTch:
		  Gbl.Action.Act = ActAdmAsgWrkCrs;
		  break;
	       default:
		  break;
	      }
	    break;
	 case Usr_OPTION_ATTENDANCE:
	    switch (Gbl.Action.Act)
	      {
	       case ActDoActOnSevStd:
		  Gbl.Action.Act = ActSeeLstUsrAtt;
		  break;
	       default:
		  break;
	      }
	    break;
	 case Usr_OPTION_MESSAGE:
	    switch (Gbl.Action.Act)
	      {
	       case ActDoActOnSevStd:
	       case ActDoActOnSevTch:
		  Gbl.Action.Act = ActReqMsgUsr;
		  break;
	       default:
		  break;
	      }
	    break;
	 case Usr_OPTION_EMAIL:
	    switch (Gbl.Action.Act)
	      {
	       case ActDoActOnSevStd:
		  Gbl.Action.Act = ActMaiUsr;
		  break;
	       default:
		  break;
	      }
	    break;
	 case Usr_OPTION_FOLLOW:
	    switch (Gbl.Action.Act)
	      {
	       case ActDoActOnSevStd:
		  Gbl.Action.Act = ActReqFolSevStd;
		  break;
	       case ActDoActOnSevTch:
		  Gbl.Action.Act = ActReqFolSevTch;
		  break;
	       default:
		  break;
	      }
	    break;
	 case Usr_OPTION_UNFOLLOW:
	    switch (Gbl.Action.Act)
	      {
	       case ActDoActOnSevStd:
		  Gbl.Action.Act = ActReqUnfSevStd;
		  break;
	       case ActDoActOnSevTch:
		  Gbl.Action.Act = ActReqUnfSevTch;
		  break;
	       default:
		  break;
	      }
	    break;
	 default:
	    break;
	}

      if (Gbl.Action.Act == Gbl.Action.Original)	// Fail, no change in action
	 Ale_CreateAlert (Ale_ERROR,NULL,"Wrong action.");
      else						// Success, action has changed
	 Tab_SetCurrentTab ();
     }
   else							// If no users selected...
      Ale_CreateAlert (Ale_WARNING,NULL,		// ...write warning notice
	               Txt_You_must_select_one_ore_more_users);
  }

void Usr_DoActionOnSeveralUsrs2 (void)
  {
   /***** Show possible alerts *****/
   Ale_ShowAlerts (NULL);

   /***** If success, action has changed.
          No change in action means an error in form has happened,
          so show again the form to selected users *****/
   switch (Gbl.Action.Act)
     {
      case ActDoActOnSevGst:
	 Usr_SeeGuests ();
	 break;
      case ActDoActOnSevStd:
	 Usr_SeeStudents ();
	 break;
      case ActDoActOnSevTch:
	 Usr_SeeTeachers ();
	 break;
      default:
	 break;
     }

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
  }

/*****************************************************************************/
/*************** Get action to do with list of selected users ****************/
/*****************************************************************************/

static Usr_ListUsrsOption_t Usr_GetListUsrsOption (Usr_ListUsrsOption_t DefaultAction)
  {
   return (Usr_ListUsrsOption_t) Par_GetParUnsignedLong ("ListUsrsAction",
							 0,
							 Usr_LIST_USRS_NUM_OPTIONS - 1,
							 (unsigned long) DefaultAction);
  }

/*****************************************************************************/
/***************** Put contextual icons in list of guests ********************/
/*****************************************************************************/

static void Usr_PutIconsListGsts (__attribute__((unused)) void *Args)
  {
   switch (Gbl.Usrs.Me.ListType)
     {
      case Set_USR_LIST_AS_CLASS_PHOTO:
	 if (Gbl.Usrs.LstUsrs[Rol_GST].NumUsrs)
	    /***** Put icon to print guests *****/
	    Usr_PutIconToPrintGsts ();
	 break;
      case Set_USR_LIST_AS_LISTING:
	 /***** Put icon to show all data of guests *****/
	 Usr_PutIconToShowGstsAllData ();
	 break;
      default:
	 break;
     }

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_USERS);
  }

/*****************************************************************************/
/**************** Put contextual icons in list of students *******************/
/*****************************************************************************/

static void Usr_PutIconsListStds (__attribute__((unused)) void *Args)
  {
   switch (Gbl.Usrs.Me.ListType)
     {
      case Set_USR_LIST_AS_CLASS_PHOTO:
	 if (Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs)
	    /***** Put icon to print students *****/
	    Usr_PutIconToPrintStds ();
	 break;
      case Set_USR_LIST_AS_LISTING:
	 /***** Put icon to show all data of students *****/
	 Usr_PutIconToShowStdsAllData ();
	 break;
      default:
	 break;
     }

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_USERS);
  }

/*****************************************************************************/
/**************** Put contextual icons in list of teachers *******************/
/*****************************************************************************/

static void Usr_PutIconsListTchs (__attribute__((unused)) void *Args)
  {
   switch (Gbl.Usrs.Me.ListType)
     {
      case Set_USR_LIST_AS_CLASS_PHOTO:
	 if (Gbl.Usrs.LstUsrs[Rol_TCH].NumUsrs)
	    /***** Put icon to print teachers *****/
	    Usr_PutIconToPrintTchs ();
	 break;
      case Set_USR_LIST_AS_LISTING:
	 /***** Put icon to show all data of teachers *****/
	 Usr_PutIconToShowTchsAllData ();
	 break;
      default:
	 break;
     }

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_USERS);
  }

/*****************************************************************************/
/***************** Functions used to print lists of users ********************/
/*****************************************************************************/

static void Usr_PutIconToPrintGsts (void)
  {
   Ico_PutContextualIconToPrint (ActPrnGstPho,Usr_ShowGstsAllDataPars,NULL);
  }

static void Usr_PutIconToPrintStds (void)
  {
   Ico_PutContextualIconToPrint (ActPrnStdPho,Usr_ShowStdsAllDataPars,NULL);
  }

static void Usr_PutIconToPrintTchs (void)
  {
   Ico_PutContextualIconToPrint (ActPrnTchPho,Usr_ShowTchsAllDataPars,NULL);
  }

/*****************************************************************************/
/**************** Functions used to list all data of users *******************/
/*****************************************************************************/

static void Usr_PutIconToShowGstsAllData (void)
  {
   Lay_PutContextualLinkOnlyIcon (ActLstGstAll,NULL,
                                  Usr_ShowGstsAllDataPars,NULL,
				  "table.svg",Ico_BLACK);
  }

static void Usr_PutIconToShowStdsAllData (void)
  {
   Lay_PutContextualLinkOnlyIcon (ActLstStdAll,NULL,
                                  Usr_ShowStdsAllDataPars,NULL,
			          "table.svg",Ico_BLACK);
  }

static void Usr_PutIconToShowTchsAllData (void)
  {
   Lay_PutContextualLinkOnlyIcon (ActLstTchAll,NULL,
                                  Usr_ShowTchsAllDataPars,NULL,
			          "table.svg",Ico_BLACK);
  }

static void Usr_ShowGstsAllDataPars (__attribute__((unused)) void *Args)
  {
   Set_PutParListWithPhotos ();
  }

static void Usr_ShowStdsAllDataPars (__attribute__((unused)) void *Args)
  {
   Grp_PutParsCodGrps ();
   Set_PutParListWithPhotos ();
  }

static void Usr_ShowTchsAllDataPars (__attribute__((unused)) void *Args)
  {
   Sco_PutParCurrentScope (&Gbl.Scope.Current);
   Set_PutParListWithPhotos ();
  }

/*****************************************************************************/
/************* Draw class photo with guests ready to be printed **************/
/*****************************************************************************/

void Usr_SeeGstClassPhotoPrn (void)
  {
   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Set_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   // Sco_SetScopesForListingGuests ();
   Sco_GetScope ("ScopeUsr",Hie_SYS);

   /***** Get list of guests *****/
   Usr_GetGstsLst (Gbl.Scope.Current);

   if (Gbl.Usrs.LstUsrs[Rol_GST].NumUsrs)
     {
      /***** Draw the guests' class photo *****/
      Lay_WriteHeaderClassPhoto (Vie_PRINT);
      HTM_TABLE_BeginWide ();
	 Usr_DrawClassPhoto (&Gbl.Usrs.Selected,Rol_GST,
			     Usr_CLASS_PHOTO_PRN,
			     false);	// Don't put checkbox to select user
      HTM_TABLE_End ();
     }
   else	// Gbl.Usrs.LstUsrs[Rol_GST].NumUsrs
      /***** Show warning indicating no guests found *****/
      Usr_ShowWarningNoUsersFound (Rol_GST);

   /***** Free memory for guests list *****/
   Usr_FreeUsrsList (Rol_GST);
  }

/*****************************************************************************/
/**************** Draw class photo with students ready to print **************/
/*****************************************************************************/

void Usr_SeeStdClassPhotoPrn (void)
  {
   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Set_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Sco_SetAllowedScopesForListingStudents ();
   Sco_GetScope ("ScopeUsr",Hie_CRS);

   /****** Get groups to show ******/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get list of students *****/
   Usr_GetListUsrs (Gbl.Scope.Current,Rol_STD);

   if (Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs)
     {
      /***** Draw the students' class photo *****/
      Lay_WriteHeaderClassPhoto (Vie_PRINT);
      HTM_TABLE_BeginWide ();
	 Usr_DrawClassPhoto (&Gbl.Usrs.Selected,Rol_STD,
			     Usr_CLASS_PHOTO_PRN,
			     false);	// Don't put checkbox to select user
      HTM_TABLE_End ();
     }
   else	// Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs == 0
      /***** Show warning indicating no students found *****/
      Usr_ShowWarningNoUsersFound (Rol_STD);

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (Rol_STD);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/**************** Draw class photo with teachers ready to print **************/
/*****************************************************************************/

void Usr_SeeTchClassPhotoPrn (void)
  {
   unsigned NumUsrs;

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Set_GetAndUpdatePrefsAboutUsrList ();

   /***** Get scope *****/
   Gbl.Scope.Allowed = 1 << Hie_SYS |
	               1 << Hie_CTY |
                       1 << Hie_INS |
                       1 << Hie_CTR |
                       1 << Hie_DEG |
                       1 << Hie_CRS;
   Sco_GetScope ("ScopeUsr",Hie_CRS);

   /****** Get groups to show ******/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get list of teachers *****/
   Usr_GetListUsrs (Gbl.Scope.Current,Rol_NET);	// Non-editing teachers
   Usr_GetListUsrs (Gbl.Scope.Current,Rol_TCH);	// Teachers
   if (Gbl.Scope.Current == Hie_CRS)
      NumUsrs = Gbl.Usrs.LstUsrs[Rol_NET].NumUsrs +
		Gbl.Usrs.LstUsrs[Rol_TCH].NumUsrs;
   else
      NumUsrs = Enr_GetNumUsrsInCrss (Gbl.Scope.Current,
				     (Gbl.Scope.Current == Hie_CTY ? Gbl.Hierarchy.Node[Hie_CTY].HieCod :
				     (Gbl.Scope.Current == Hie_INS ? Gbl.Hierarchy.Node[Hie_INS].HieCod :
				     (Gbl.Scope.Current == Hie_CTR ? Gbl.Hierarchy.Node[Hie_CTR].HieCod :
				     (Gbl.Scope.Current == Hie_DEG ? Gbl.Hierarchy.Node[Hie_DEG].HieCod :
				     (Gbl.Scope.Current == Hie_CRS ? Gbl.Hierarchy.Node[Hie_CRS].HieCod :
								     -1L))))),
				      1 << Rol_NET |
				      1 << Rol_TCH);

   if (NumUsrs)
     {
      /***** Draw the teachers' class photo *****/
      Lay_WriteHeaderClassPhoto (Vie_PRINT);
      HTM_TABLE_BeginWide ();

	 /* List teachers and non-editing teachers */
	 Usr_DrawClassPhoto (&Gbl.Usrs.Selected,Rol_TCH,
			     Usr_CLASS_PHOTO_PRN,
			     false);	// Don't put checkbox to select user
	 Usr_DrawClassPhoto (&Gbl.Usrs.Selected,Rol_NET,
			     Usr_CLASS_PHOTO_PRN,
			     false);	// Don't put checkbox to select user

      HTM_TABLE_End ();
     }
   else	// NumUsrs == 0
      /***** Show warning indicating no teachers found *****/
      Usr_ShowWarningNoUsersFound (Rol_TCH);

   /***** Free memory for teachers lists *****/
   Usr_FreeUsrsList (Rol_TCH);	// Teachers
   Usr_FreeUsrsList (Rol_NET);	// Non-editing teachers

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/****************** Draw class photo with students or teachers ***************/
/*****************************************************************************/

static void Usr_DrawClassPhoto (struct Usr_SelectedUsrs *SelectedUsrs,
                                Rol_Role_t Role,
				Usr_ClassPhotoType_t ClassPhotoType,
				bool PutCheckBoxToSelectUsr)
  {
   static const char *ClassPhoto[Usr_NUM_CLASS_PHOTO_TYPE][PhoSha_NUM_SHAPES] =
     {
      [Usr_CLASS_PHOTO_SEL    ][PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [Usr_CLASS_PHOTO_SEL    ][PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [Usr_CLASS_PHOTO_SEL    ][PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [Usr_CLASS_PHOTO_SEL    ][PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
      [Usr_CLASS_PHOTO_SEL_SEE][PhoSha_SHAPE_CIRCLE   ] = "PHOTOC45x60",
      [Usr_CLASS_PHOTO_SEL_SEE][PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE45x60",
      [Usr_CLASS_PHOTO_SEL_SEE][PhoSha_SHAPE_OVAL     ] = "PHOTOO45x60",
      [Usr_CLASS_PHOTO_SEL_SEE][PhoSha_SHAPE_RECTANGLE] = "PHOTOR45x60",
      [Usr_CLASS_PHOTO_PRN    ][PhoSha_SHAPE_CIRCLE   ] = "PHOTOC45x60",
      [Usr_CLASS_PHOTO_PRN    ][PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE45x60",
      [Usr_CLASS_PHOTO_PRN    ][PhoSha_SHAPE_OVAL     ] = "PHOTOO45x60",
      [Usr_CLASS_PHOTO_PRN    ][PhoSha_SHAPE_RECTANGLE] = "PHOTOR45x60",
     };
   unsigned NumUsr;
   bool TRIsOpen = false;
   bool UsrIsTheMsgSender;
   struct Usr_Data UsrDat;

   if (Gbl.Usrs.LstUsrs[Role].NumUsrs)
     {
      /***** Put a row to select all users *****/
      if (PutCheckBoxToSelectUsr)
	 Usr_PutCheckboxToSelectAllUsers (SelectedUsrs,Role);

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Loop for showing users photos, names and place of birth *****/
      for (NumUsr = 0;
	   NumUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs; )
	{
	 if ((NumUsr % Gbl.Usrs.ClassPhoto.Cols) == 0)
	   {
	    HTM_TR_Begin (NULL);
	    TRIsOpen = true;
	   }

	 /* Copy user's basic data from list */
	 Usr_CopyBasicUsrDataFromList (&UsrDat,&Gbl.Usrs.LstUsrs[Role].Lst[NumUsr]);

	 /* Get list of user's IDs */
	 ID_GetListIDsFromUsrCod (&UsrDat);

	 /***** Begin user's cell *****/
	 if (ClassPhotoType == Usr_CLASS_PHOTO_SEL &&
	     UsrDat.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod)
	   {
	    UsrIsTheMsgSender = true;
	    HTM_TD_Begin ("class=\"CLASSPHOTO CLASSPHOTO_%s CB LIGHT_GREEN\"",
	                  The_GetSuffix ());
	   }
	 else
	   {
	    UsrIsTheMsgSender = false;
	    HTM_TD_Begin ("class=\"CLASSPHOTO CLASSPHOTO_%s CB\"",
	                  The_GetSuffix ());
	   }

	 /***** Checkbox to select this user *****/
	 if (PutCheckBoxToSelectUsr)
	    Usr_PutCheckboxToSelectUser (Role,UsrDat.EnUsrCod,UsrIsTheMsgSender,
					 SelectedUsrs);

	 /***** Show photo *****/
	 Pho_ShowUsrPhotoIfAllowed (&UsrDat,
	                            ClassPhoto[ClassPhotoType][Gbl.Prefs.PhotoShape],Pho_ZOOM);

	 /***** Photo foot *****/
	 HTM_DIV_Begin ("class=\"CLASSPHOTO_CAPTION CLASSPHOTO_%s\"",
	                The_GetSuffix ());

	    /* Name */
	    if (UsrDat.FrstName[0])
	       HTM_Txt (UsrDat.FrstName);
	    else
	       HTM_NBSP ();
	    HTM_BR ();
	    if (UsrDat.Surname1[0])
	       HTM_Txt (UsrDat.Surname1);
	    else
	       HTM_NBSP ();
	    HTM_BR ();
	    if (UsrDat.Surname2[0])
	       HTM_Txt (UsrDat.Surname2);
	    else
	       HTM_NBSP ();

	 HTM_DIV_End ();

	 /***** End user's cell *****/
	 HTM_TD_End ();

	 if ((++NumUsr % Gbl.Usrs.ClassPhoto.Cols) == 0)
	   {
	    HTM_TR_End ();
	    TRIsOpen = false;
	   }
	}
      if (TRIsOpen)
	 HTM_TR_End ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
  }

/*****************************************************************************/
/***************** Write selector of columns in class photo ******************/
/*****************************************************************************/

void Usr_PutSelectorNumColsClassPhoto (void)
  {
   extern const char *Txt_columns;
   unsigned Cols;

   /***** Begin label *****/
   HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());

      /***** Begin selector *****/
      HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
			"name=\"ColsClassPhoto\" class=\"INPUT_%s\"",
			The_GetSuffix ());

	 /***** Put a row in selector for every number of columns *****/
	 for (Cols  = 1;
	      Cols <= Usr_CLASS_PHOTO_COLS_MAX;
	      Cols++)
	    HTM_OPTION (HTM_Type_UNSIGNED,&Cols,
			Cols == Gbl.Usrs.ClassPhoto.Cols ? HTM_OPTION_SELECTED :
	                				   HTM_OPTION_UNSELECTED,
			HTM_OPTION_ENABLED,
			"%u",Cols);

      /***** End selector *****/
      HTM_SELECT_End ();

      HTM_Txt (Txt_columns);

   /***** End label *****/
   HTM_LABEL_End ();
  }

/*****************************************************************************/
/********** Build the relative path of a user from his user's code ***********/
/*****************************************************************************/

void Usr_ConstructPathUsr (long UsrCod,char PathUsr[PATH_MAX + 1])
  {
   char PathAboveUsr[PATH_MAX + 1];
   char PathUsrTmp[PATH_MAX + 1 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Path for users *****/
   Fil_CreateDirIfNotExists (Cfg_PATH_USR_PRIVATE);

   /***** Path above user's ID *****/
   snprintf (PathAboveUsr,sizeof (PathAboveUsr),"%s/%02u",
	     Cfg_PATH_USR_PRIVATE,(unsigned) (UsrCod % 100));
   Fil_CreateDirIfNotExists (PathAboveUsr);

   /***** Path for user *****/
   snprintf (PathUsrTmp,sizeof (PathUsrTmp),"%s/%ld",PathAboveUsr,UsrCod);
   if (strlen (PathUsrTmp) <= PATH_MAX)
      Str_Copy (PathUsr,PathUsrTmp,PATH_MAX);
   else
      Err_PathTooLongExit ();
  }

/*****************************************************************************/
/********************** Show warning "no users found" ************************/
/*****************************************************************************/
// Use Rol_UNK type to display "no users found"

void Usr_ShowWarningNoUsersFound (Rol_Role_t Role)
  {
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   extern const char *Txt_Register_students;
   extern const char *Txt_Register_teacher;

   if (Gbl.Crs.Grps.AllGrps &&			// All groups selected
       Role == Rol_STD &&			// No students found
       Gbl.Usrs.Me.Role.Logged == Rol_TCH)	// Course selected and I am logged as teacher
      /***** Show alert and button to enrol students *****/
      Ale_ShowAlertAndButton (ActReqEnrSevStd,NULL,NULL,
                              NULL,NULL,
                              Btn_CREATE_BUTTON,Txt_Register_students,
			      Ale_WARNING,Txt_No_users_found[Rol_STD]);

   else if (Gbl.Crs.Grps.AllGrps &&		// All groups selected
            Role == Rol_TCH &&			// No teachers found
            Gbl.Hierarchy.Level == Hie_CRS &&	// Course selected
            Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)	// I am an administrator
      /***** Show alert and button to enrol students *****/
      Ale_ShowAlertAndButton (ActReqMdfOneTch,NULL,NULL,
                              NULL,NULL,
                              Btn_CREATE_BUTTON,Txt_Register_teacher,
			      Ale_WARNING,Txt_No_users_found[Rol_TCH]);
   else
      /***** Show alert *****/
      Ale_ShowAlert (Ale_INFO,Txt_No_users_found[Role]);
  }

/*****************************************************************************/
/****************** Get total number of users in platform ********************/
/*****************************************************************************/

unsigned Usr_GetTotalNumberOfUsers (void)
  {
   long Cod;
   unsigned Roles;

   /***** Get number of users with events from database *****/
   switch (Gbl.Scope.Current)
     {
      case Hie_SYS:
	 return (unsigned) DB_GetNumRowsTable ("usr_data");			// All users in platform
      case Hie_CTY:
      case Hie_INS:
      case Hie_CTR:
      case Hie_DEG:
      case Hie_CRS:
         Cod = (Gbl.Scope.Current == Hie_CTY ? Gbl.Hierarchy.Node[Hie_CTY].HieCod :
	       (Gbl.Scope.Current == Hie_INS ? Gbl.Hierarchy.Node[Hie_INS].HieCod :
	       (Gbl.Scope.Current == Hie_CTR ? Gbl.Hierarchy.Node[Hie_CTR].HieCod :
	       (Gbl.Scope.Current == Hie_DEG ? Gbl.Hierarchy.Node[Hie_DEG].HieCod :
	                                       Gbl.Hierarchy.Node[Hie_CRS].HieCod))));
         Roles = (1 << Rol_STD) |
	         (1 << Rol_NET) |
	         (1 << Rol_TCH);
         return Enr_GetCachedNumUsrsInCrss (Gbl.Scope.Current,Cod,Roles);	// All users in courses
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/******************** Write the author of a message/post/ ********************/
/*****************************************************************************/
// Input: UsrDat must hold user's data

void Usr_WriteAuthor (struct Usr_Data *UsrDat,
                      Cns_DisabledOrEnabled_t DisabledOrEnabled)
  {
   extern const char *Txt_Unknown_or_without_photo;
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC30x40",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE30x40",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO30x40",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR30x40",
     };
   bool WriteAuthor;

   /***** Write author name or don't write it? *****/
   WriteAuthor = false;
   if (DisabledOrEnabled == Cns_ENABLED)
      if (UsrDat->UsrCod > 0)
         WriteAuthor = true;

   /***** Begin table and row *****/
   HTM_TABLE_BeginPadding (2);
      HTM_TR_Begin (NULL);

	 /***** Begin first column with author's photo
		(if author has a web page, put a link to it) *****/
	 HTM_TD_Begin ("class=\"CT\" style=\"width:30px;\"");
	    if (WriteAuthor)
	       Pho_ShowUsrPhotoIfAllowed (UsrDat,
					  ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
	    else
	       Ico_PutIcon ("usr_bl.jpg",Ico_UNCHANGED,Txt_Unknown_or_without_photo,
			    ClassPhoto[Gbl.Prefs.PhotoShape]);
	 HTM_TD_End ();

	 /***** Second column with user name
	        (if author has a web page, put a link to it) *****/
	 if (WriteAuthor)
	   {
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_DIV_Begin ("class=\"AUTHOR_2_LINES\"");	// Limited width
		  Usr_WriteFirstNameBRSurnames (UsrDat);
	       HTM_DIV_End ();
	   }
	 else
	    HTM_TD_Begin ("class=\"LM\"");
	 HTM_TD_End ();

      /***** End row and table *****/
      HTM_TR_End ();
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********************* Write the author of an assignment *********************/
/*****************************************************************************/

void Usr_WriteAuthor1Line (long UsrCod,HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC15x20",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE15x20",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO15x20",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR15x20",
     };
   extern const char *HidVis_MsgClass[HidVis_NUM_HIDDEN_VISIBLE];
   bool ShowPhoto = false;
   char PhotoURL[Cns_MAX_BYTES_WWW + 1];
   struct Usr_Data UsrDat;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Get data of author *****/
   UsrDat.UsrCod = UsrCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
                                                Usr_DONT_GET_PREFS,
                                                Usr_DONT_GET_ROLE_IN_CRS))
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);

   /***** Show photo *****/
   Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                	                 NULL,
	             ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);

   /***** Write name *****/
   HTM_DIV_Begin ("class=\"AUTHOR_1_LINE %s_%s\"",
                  HidVis_MsgClass[HiddenOrVisible],The_GetSuffix ());
      HTM_Txt (UsrDat.FullName);
   HTM_DIV_End ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/*************** Show a table cell with the data of a user *******************/
/*****************************************************************************/

void Usr_ShowTableCellWithUsrData (struct Usr_Data *UsrDat,unsigned NumRows)
  {
   static Act_Action_t NextAction[Rol_NUM_ROLES] =
     {
      [Rol_STD] = ActSeeRecOneStd,
      [Rol_NET] = ActSeeRecOneTch,
      [Rol_TCH] = ActSeeRecOneTch,
     };
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC45x60",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE45x60",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO45x60",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR45x60",
     };

   /***** Show user's photo *****/
   if (NumRows)
      HTM_TD_Begin ("rowspan=\"%u\" class=\"LT LINE_BOTTOM %s\"",
	            NumRows + 1,The_GetColorRows ());
   else
      HTM_TD_Begin ("class=\"LT LINE_BOTTOM %s\"",The_GetColorRows ());
   Pho_ShowUsrPhotoIfAllowed (UsrDat,ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
   HTM_TD_End ();

   /***** User's IDs and name *****/
   /* Begin cell */
   if (NumRows)
      HTM_TD_Begin ("rowspan=\"%u\" class=\"LT LINE_BOTTOM %s\"",
	            NumRows + 1,The_GetColorRows ());
   else
      HTM_TD_Begin ("class=\"LT LINE_BOTTOM %s\"",The_GetColorRows ());

   /* Action to go to user's record depending on role in course */
   if (!NextAction[UsrDat->Roles.InCurrentCrs])
      /* Begin div */
      HTM_DIV_Begin ("class=\"MSG_AUT_%s\"",The_GetSuffix ());
   else
     {
      /* Begin form to go to user's record card */
      Frm_BeginForm (NextAction[UsrDat->Roles.InCurrentCrs]);
	 Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
	 HTM_BUTTON_Submit_Begin (UsrDat->FullName,
	                          "class=\"LT BT_LINK MSG_AUT_%s\"",
	                          The_GetSuffix ());
     }

   /* User's ID */
   ID_WriteUsrIDs (UsrDat,NULL);

   /* User's name */
   HTM_BR ();
   HTM_Txt (UsrDat->Surname1);
   if (UsrDat->Surname2[0])
      HTM_SPTxt (UsrDat->Surname2);
   if (UsrDat->FrstName[0])
     {
      HTM_Comma ();
      HTM_BR ();
      HTM_Txt (UsrDat->FrstName);
     }

   if (!NextAction[UsrDat->Roles.InCurrentCrs])
      /* End div */
      HTM_DIV_End ();
   else
     {
      /* End form */
	 HTM_BUTTON_End ();
      Frm_EndForm ();
     }

   /* End cell */
   HTM_TD_End ();
  }

/*****************************************************************************/
/********** Show a setting selector for me / selected users / all ************/
/*****************************************************************************/

void Usr_PutWhoIcon (Usr_Who_t Who)
  {
   extern const char *Txt_WHO[Usr_NUM_WHO];
   static const char *Icon[Usr_NUM_WHO] =
     {
      [Usr_WHO_UNKNOWN ] = NULL,
      [Usr_WHO_ME      ] = NULL,
      [Usr_WHO_SELECTED] = "search.svg",
      [Usr_WHO_FOLLOWED] = "user-check.svg",
      [Usr_WHO_ALL     ] = "users.svg",
     };

   switch (Who)
     {
      case Usr_WHO_UNKNOWN:
	 break;
      case Usr_WHO_ME:
         HTM_INPUT_IMAGE (Gbl.Usrs.Me.PhotoURL[0] ? Gbl.Usrs.Me.PhotoURL :
						    Cfg_URL_ICON_PUBLIC,
			  Gbl.Usrs.Me.PhotoURL[0] ? NULL :
						    "usr_bl.jpg",
		          Txt_WHO[Who],
	                  "class=\"ICO_HIGHLIGHT PHOTOR15x20\"");
	 break;
      case Usr_WHO_SELECTED:
      case Usr_WHO_FOLLOWED:
      case Usr_WHO_ALL:
         HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,Icon[Who],Txt_WHO[Who],
                          "class=\"ICO_HIGHLIGHT ICOx20 ICO_%s_%s\"",
                          Ico_GetPreffix (Ico_BLACK),The_GetSuffix ());
         break;
      default:
	 Err_WrongWhoExit ();
	 break;
     }
  }

/*****************************************************************************/
/*************** Put hidden param for which users are involved ***************/
/*****************************************************************************/

void Usr_PutParWho (Usr_Who_t Who)
  {
   Par_PutParUnsigned (NULL,"Who",(unsigned) Who);
  }

/*****************************************************************************/
/*************** Get hidden param for which users are involved ***************/
/*****************************************************************************/

Usr_Who_t Usr_GetParWho (void)
  {
   return (Usr_Who_t) Par_GetParUnsignedLong ("Who",
                                              1,
                                              Usr_NUM_WHO - 1,
                                              Usr_WHO_UNKNOWN);
  }

/*****************************************************************************/
/********************** Show stats about number of users *********************/
/*****************************************************************************/

void Usr_GetAndShowUsersStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_users;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Users;
   extern const char *Txt_Number_of_users;
   extern const char *Txt_Average_number_of_courses_to_which_a_user_belongs;
   extern const char *Txt_Average_number_of_users_belonging_to_a_course;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_USERS],NULL,NULL,
                      Hlp_ANALYTICS_Figures_users,Box_NOT_CLOSABLE,2);

      /***** Write heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Users                                            ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_users                                  ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Average_number_of_courses_to_which_a_user_belongs,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Average_number_of_users_belonging_to_a_course    ,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Figures *****/
      Usr_GetAndShowNumUsrsInCrss (Rol_STD);		// Students
      Usr_GetAndShowNumUsrsInCrss (Rol_NET);		// Non-editing teachers
      Usr_GetAndShowNumUsrsInCrss (Rol_TCH);		// Teachers
      Usr_GetAndShowNumUsrsInCrss (Rol_UNK);		// Any user in courses

      /***** Separator *****/
      HTM_TR_Begin (NULL);
	 HTM_TH_Span (NULL,HTM_HEAD_CENTER,1,4,"SEPAR_ROW");
      HTM_TR_End ();

      Usr_GetAndShowNumUsrsNotBelongingToAnyCrs ();	// Users not beloging to any course

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/**************** Get and show number of users in courses ********************/
/*****************************************************************************/
// Rol_UNK means any role in courses

static void Usr_GetAndShowNumUsrsInCrss (Rol_Role_t Role)
  {
   extern const char *Txt_Total;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   long Cod = Hie_GetCurrentCod ();
   const char *Class;
   unsigned Roles;

   /***** Initializations depending on role *****/
   if (Role == Rol_UNK)
     {
      Class = "RB LINE_TOP DAT_STRONG";
      Roles = (1 << Rol_STD) |
	      (1 << Rol_NET) |
	      (1 << Rol_TCH);
     }
   else
     {
      Class = "RB DAT";
      Roles = (1 << Role);
     }

   /***** Write the total number of users *****/
   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s_%s\"",Class,The_GetSuffix ());
	 HTM_Txt ((Role == Rol_UNK) ? Txt_Total :
				      Txt_ROLES_PLURAL_Abc[Role][Usr_SEX_UNKNOWN]);
      HTM_TD_End ();

      /* Number of users in courses */
      HTM_TD_Begin ("class=\"%s_%s\"",Class,The_GetSuffix ());
	 HTM_Unsigned (Enr_GetCachedNumUsrsInCrss (Gbl.Scope.Current,Cod,Roles));
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"%s_%s\"",Class,The_GetSuffix ());
	 HTM_Double2Decimals (Enr_GetCachedAverageNumCrssPerUsr (Gbl.Scope.Current,Cod,Role));
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"%s_%s\"",Class,The_GetSuffix ());
	 HTM_Double2Decimals (Enr_GetCachedAverageNumUsrsPerCrs (Gbl.Scope.Current,Cod,Role));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Get and show number of users in courses ********************/
/*****************************************************************************/

static void Usr_GetAndShowNumUsrsNotBelongingToAnyCrs (void)
  {
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char *Class;

   if (asprintf (&Class,"RB DAT_%s",The_GetSuffix ()) < 0)
      Err_NotEnoughMemoryExit ();

   /***** Write the total number of users not belonging to any course *****/
   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s\"",Class);
	 HTM_Txt (Txt_ROLES_PLURAL_Abc[Rol_GST][Usr_SEX_UNKNOWN]);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"%s\"",Class);
	 HTM_Unsigned (Enr_GetCachedNumUsrsNotBelongingToAnyCrs ());
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"%s\"",Class);
	 HTM_Double2Decimals (0.0);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"%s\"",Class);
	 HTM_Double2Decimals (0.0);
      HTM_TD_End ();

   HTM_TR_End ();

   free (Class);
  }

/*****************************************************************************/
/****************************** Show users' ranking **************************/
/*****************************************************************************/

void Usr_GetAndShowUsersRanking (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_ranking;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Clicks;
   extern const char *Txt_Clicks_per_day;
   extern const char *Txt_Timeline;
   extern const char *Txt_Downloads;
   extern const char *Txt_Forums;
   extern const char *Txt_Messages;
   extern const char *Txt_Followers;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_USERS_RANKING],NULL,NULL,
                      Hlp_ANALYTICS_Figures_ranking,Box_NOT_CLOSABLE,2);

      /***** Write heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Clicks        ,HTM_HEAD_CENTER);
	 HTM_TH (Txt_Clicks_per_day,HTM_HEAD_CENTER);
	 HTM_TH (Txt_Timeline      ,HTM_HEAD_CENTER);
	 HTM_TH (Txt_Followers     ,HTM_HEAD_CENTER);
	 HTM_TH (Txt_Downloads     ,HTM_HEAD_CENTER);
	 HTM_TH (Txt_Forums        ,HTM_HEAD_CENTER);
	 HTM_TH (Txt_Messages      ,HTM_HEAD_CENTER);
      HTM_TR_End ();

      /***** Rankings *****/
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"LT\"");
	    Prf_GetAndShowRankingClicks ();
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"LT\"");
	    Prf_GetAndShowRankingClicksPerDay ();
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"LT\"");
	    Prf_GetAndShowRankingTimelinePubs ();
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"LT\"");
	    Fol_GetAndShowRankingFollowers ();
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"LT\"");
	    Prf_GetAndShowRankingFileViews ();
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"LT\"");
	    Prf_GetAndShowRankingForPsts ();
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"LT\"");
	    Prf_GetAndShowRankingMsgsSnt ();
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
