// swad_enrolment.c: enrolment (registration) or removing of users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For exit, system, malloc, free, rand, etc.
#include <string.h>		// For string functions

#include "swad_account.h"
#include "swad_announcement.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_duplicate.h"
#include "swad_enrolment.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_role.h"
#include "swad_table.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const bool Enr_ICanAdminOtherUsrs[Rol_NUM_ROLES] =
  {
   /* Users who can not admin */
   false,	// Rol_UNK
   false,	// Rol_GST
   false,	// Rol_USR
   false,	// Rol_STD
   false,	// Rol_NET

   /* Users who can admin */
   true,	// Rol_TCH
   true,	// Rol_DEG_ADM
   true,	// Rol_CTR_ADM
   true,	// Rol_INS_ADM
   true,	// Rol_SYS_ADM
  };

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

#define Enr_NUM_REG_REM_USRS_ACTIONS 6
typedef enum
  {
   Enr_REG_REM_USRS_UNKNOWN_ACTION     	  = 0,
   Enr_REGISTER_SPECIFIED_USRS_IN_CRS     = 1,
   Enr_REMOVE_SPECIFIED_USRS_FROM_CRS     = 2,
   Enr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS = 3,
   Enr_UPDATE_USRS_IN_CRS                 = 4,
   Enr_ELIMINATE_USRS_FROM_PLATFORM       = 5,
  } Enr_RegRemUsrsAction_t;

typedef enum
  {
   Enr_REQUEST_REMOVE_USR,
   Enr_REMOVE_USR,
  } Enr_ReqDelOrDelUsr_t;

typedef enum
  {
   Enr_DO_NOT_REMOVE_WORKS,
   Enr_REMOVE_WORKS,
  } Enr_RemoveUsrWorks_t;

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

static void Enr_NotifyAfterEnrolment (struct UsrData *UsrDat,Rol_Role_t NewRole);

static void Enr_ReqAdminUsrs (Rol_Role_t Role);
static void Enr_ShowFormRegRemSeveralUsrs (Rol_Role_t Role);

static void Enr_PutAreaToEnterUsrsIDs (void);
static void Enr_PutActionsRegRemSeveralUsrs (void);

static void Enr_ReceiveFormUsrsCrs (Rol_Role_t Role);

static void Enr_RegisterUsr (struct UsrData *UsrDat,Rol_Role_t RegRemRole,
                             struct ListCodGrps *LstGrps,unsigned *NumUsrsRegistered);

static void Enr_PutLinkToRemAllStdsThisCrs (void);

static void Enr_ShowEnrolmentRequestsGivenRoles (unsigned RolesSelected);

static void Enr_RemoveEnrolmentRequest (long CrsCod,long UsrCod);
static void Enr_RemoveExpiredEnrolmentRequests (void);

static void Enr_ReqRegRemUsr (Rol_Role_t Role);
static void Enr_ReqAnotherUsrIDToRegisterRemove (Rol_Role_t Role);
static void Enr_AskIfRegRemMe (Rol_Role_t Role);
static void Enr_AskIfRegRemAnotherUsr (Rol_Role_t Role);
static void Enr_AskIfRegRemUsr (struct ListUsrCods *ListUsrCods,Rol_Role_t Role);

static void Enr_ShowFormToEditOtherUsr (void);

static void Enr_AddAdm (Sco_Scope_t Scope,long Cod,const char *InsCtrDegName);
static void Enr_RegisterAdmin (struct UsrData *UsrDat,Sco_Scope_t Scope,
                               long Cod,const char *InsCtrDegName);

static bool Enr_CheckIfICanRemUsrFromCrs (void);

static void Enr_ReqRemAdmOfDeg (void);
static void Enr_ReqRemOrRemAdm (Enr_ReqDelOrDelUsr_t ReqDelOrDelUsr,Sco_Scope_t Scope,
                                long Cod,const char *InsCtrDegName);

static void Enr_ReqAddAdm (Sco_Scope_t Scope,long Cod,const char *InsCtrDegName);
static void Enr_AskIfRemoveUsrFromCrs (struct UsrData *UsrDat);
static void Enr_EffectivelyRemUsrFromCrs (struct UsrData *UsrDat,struct Course *Crs,
                                          Enr_RemoveUsrWorks_t RemoveUsrWorks,Cns_QuietOrVerbose_t QuietOrVerbose);

static void Enr_AskIfRemAdm (bool ItsMe,Sco_Scope_t Scope,
			     const char *InsCtrDegName);
static void Enr_EffectivelyRemAdm (struct UsrData *UsrDat,Sco_Scope_t Scope,
                                   long Cod,const char *InsCtrDegName);

/*****************************************************************************/
/** Check if current course has students and show warning no students found **/
/*****************************************************************************/

void Enr_CheckStdsAndPutButtonToRegisterStdsInCurrentCrs (void)
  {
   /***** Put link to register students *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_TCH)	// Course selected and I am logged as teacher
      if (!Gbl.CurrentCrs.Crs.NumUsrs[Rol_STD])	// No students in course
          Usr_ShowWarningNoUsersFound (Rol_STD);
  }

/*****************************************************************************/
/****************** Put inline button to register students *******************/
/*****************************************************************************/

void Enr_PutButtonInlineToRegisterStds (long CrsCod)
  {
   extern const char *Txt_Register_students;

   if (Rol_GetRoleUsrInCrs (Gbl.Usrs.Me.UsrDat.UsrCod,CrsCod) ==  Rol_TCH) // I am a teacher in course
      if (!Usr_GetNumUsrsInCrs (Rol_STD,CrsCod))			   // No students in course
	{
	 Act_StartForm (ActReqEnrSevStd);
	 Crs_PutParamCrsCod (CrsCod);
	 Btn_PutCreateButtonInline (Txt_Register_students);
	 Act_EndForm ();
	}
  }

/*****************************************************************************/
/************ Show form to request sign up in the current course *************/
/*****************************************************************************/

void Enr_PutLinkToRequestSignUp (void)
  {
   extern const char *Txt_Sign_up;

   /***** Show the form *****/
   Lay_PutContextualLink (ActReqSignUp,NULL,NULL,
                          "enrollmentrequest64x64.gif",
                          Txt_Sign_up,Txt_Sign_up,
                          NULL);
  }

/*****************************************************************************/
/***************** Modify the role of a user in a course *********************/
/*****************************************************************************/

void Enr_ModifyRoleInCurrentCrs (struct UsrData *UsrDat,Rol_Role_t NewRole)
  {
   char *Query;

   /***** Check if user's role is allowed *****/
   switch (NewRole)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 break;
      default:
	 Lay_ShowErrorAndExit ("Wrong role.");
     }

   /***** Update the role of a user in a course *****/
   if (asprintf (&Query,"UPDATE crs_usr SET Role=%u"
		        " WHERE CrsCod=%ld AND UsrCod=%ld",
	         (unsigned) NewRole,Gbl.CurrentCrs.Crs.CrsCod,UsrDat->UsrCod) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryUPDATE_free (Query,"can not modify user's role in course");

   /***** Flush caches *****/
   Usr_FlushCachesUsr ();

   /***** Set user's roles *****/
   UsrDat->Roles.InCurrentCrs.Role = NewRole;
   UsrDat->Roles.InCurrentCrs.Valid = true;
   UsrDat->Roles.InCrss = -1;	// Force roles to be got from database
   Rol_GetRolesInAllCrssIfNotYetGot (UsrDat);	// Get roles

   /***** Create notification for this user.
	  If this user wants to receive notifications by email,
	  activate the sending of a notification *****/
   Enr_NotifyAfterEnrolment (UsrDat,NewRole);
  }

/*****************************************************************************/
/*********************** Register user in current course *********************/
/*****************************************************************************/
// Before calling this function, you must be sure that
// the user does not belong to the current course

void Enr_RegisterUsrInCurrentCrs (struct UsrData *UsrDat,Rol_Role_t NewRole,
                                  Enr_KeepOrSetAccepted_t KeepOrSetAccepted)
  {
   extern const char *Usr_StringsUsrListTypeInDB[Usr_NUM_USR_LIST_TYPES];

   /***** Check if user's role is allowed *****/
   switch (NewRole)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 break;
      default:
         Lay_ShowErrorAndExit ("Wrong role.");
     }

   /***** Register user in current course in database *****/
   DB_BuildQuery ("INSERT INTO crs_usr"
		  " (CrsCod,UsrCod,Role,Accepted,"
		  "LastDowGrpCod,LastComGrpCod,LastAssGrpCod,"
		  "NumAccTst,LastAccTst,NumQstsLastTst,"
		  "UsrListType,ColsClassPhoto,ListWithPhotos)"
		  " VALUES"
		  " (%ld,%ld,%u,'%c',"
		  "-1,-1,-1,"
		  "0,FROM_UNIXTIME(%ld),0,"
		  "'%s',%u,'%c')",
	          Gbl.CurrentCrs.Crs.CrsCod,UsrDat->UsrCod,(unsigned) NewRole,
	          KeepOrSetAccepted == Enr_SET_ACCEPTED_TO_TRUE ? 'Y' :
							          'N',
	          (long) (time_t) 0,	// The user never accessed to tests in this course
	          Usr_StringsUsrListTypeInDB[Usr_SHOW_USRS_TYPE_DEFAULT],
	          Usr_CLASS_PHOTO_COLS_DEF,
	          Usr_LIST_WITH_PHOTOS_DEF ? 'Y' :
					     'N');
   DB_QueryINSERT_new ("can not register user in course");

   /***** Flush caches *****/
   Usr_FlushCachesUsr ();

   /***** Set roles *****/
   UsrDat->Roles.InCurrentCrs.Role = NewRole;
   UsrDat->Roles.InCurrentCrs.Valid = true;
   UsrDat->Roles.InCrss = -1;	// Force roles to be got from database
   Rol_GetRolesInAllCrssIfNotYetGot (UsrDat);	// Get roles

   /***** Create notification for this user.
	  If this user wants to receive notifications by email,
	  activate the sending of a notification *****/
   Enr_NotifyAfterEnrolment (UsrDat,NewRole);
  }

/*****************************************************************************/
/********* Create notification after register user in current course *********/
/*****************************************************************************/

static void Enr_NotifyAfterEnrolment (struct UsrData *UsrDat,Rol_Role_t NewRole)
  {
   bool CreateNotif;
   bool NotifyByEmail;
   Ntf_NotifyEvent_t NotifyEvent;
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);

   /***** Check if user's role is allowed *****/
   switch (NewRole)
     {
      case Rol_STD:
	 NotifyEvent = Ntf_EVENT_ENROLMENT_STD;
	 break;
      case Rol_NET:
	 NotifyEvent = Ntf_EVENT_ENROLMENT_NET;
	 break;
      case Rol_TCH:
	 NotifyEvent = Ntf_EVENT_ENROLMENT_TCH;
	 break;
      default:
	 NotifyEvent = Ntf_EVENT_UNKNOWN;
         Lay_ShowErrorAndExit ("Wrong role.");
     }

   /***** Remove possible enrolment request ******/
   Enr_RemoveEnrolmentRequest (Gbl.CurrentCrs.Crs.CrsCod,UsrDat->UsrCod);

   /***** Remove old enrolment notifications before inserting the new one ******/
   Ntf_MarkNotifToOneUsrAsRemoved (Ntf_EVENT_ENROLMENT_STD,-1,UsrDat->UsrCod);
   Ntf_MarkNotifToOneUsrAsRemoved (Ntf_EVENT_ENROLMENT_NET,-1,UsrDat->UsrCod);
   Ntf_MarkNotifToOneUsrAsRemoved (Ntf_EVENT_ENROLMENT_TCH,-1,UsrDat->UsrCod);

   /***** Create new notification ******/
   CreateNotif = (UsrDat->Prefs.NotifNtfEvents & (1 << NotifyEvent));
   NotifyByEmail = CreateNotif && !ItsMe &&
		   (UsrDat->Prefs.EmailNtfEvents & (1 << NotifyEvent));
   if (CreateNotif)
      Ntf_StoreNotifyEventToOneUser (NotifyEvent,UsrDat,-1L,
				     (Ntf_Status_t) (NotifyByEmail ? Ntf_STATUS_BIT_EMAIL :
					                             0));
  }

/*****************************************************************************/
/****** Write a form to request another user's ID, @nickname or email ********/
/*****************************************************************************/

void Enr_WriteFormToReqAnotherUsrID (Act_Action_t NextAction,void (*FuncParams) ())
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_nick_email_or_ID;
   extern const char *Txt_Continue;

   /***** Form to request user's ID, @nickname or email address *****/
   Act_StartForm (NextAction);
   if (FuncParams)
      FuncParams ();
   fprintf (Gbl.F.Out,"<label for=\"OtherUsrIDNickOrEMail\""
	              " class=\"%s RIGHT_MIDDLE\">"
                      "%s:&nbsp;"
                      "</label>"
                      "<input type=\"text\" id=\"OtherUsrIDNickOrEMail\""
                      " name=\"OtherUsrIDNickOrEMail\""
                      " size=\"18\" maxlength=\"%u\" required=\"required\" />",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_nick_email_or_ID,
            Cns_MAX_CHARS_EMAIL_ADDRESS);

   /***** Send button*****/
   Btn_PutConfirmButton (Txt_Continue);
   Act_EndForm ();
  }

/*****************************************************************************/
/****** Request acceptation / refusion of register in current course *********/
/*****************************************************************************/

void Enr_ReqAcceptRegisterInCrs (void)
  {
   extern const char *Hlp_USERS_SignUp_confirm_enrolment;
   extern const char *Txt_Enrolment;
   extern const char *Txt_A_teacher_or_administrator_has_enroled_you_as_X_into_the_course_Y;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Confirm_my_enrolment;
   extern const char *Txt_Remove_me_from_this_course;
   Ntf_NotifyEvent_t NotifyEvent;

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Enrolment,NULL,
                 Hlp_USERS_SignUp_confirm_enrolment,Box_NOT_CLOSABLE);

   /***** Show message *****/
   snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	     Txt_A_teacher_or_administrator_has_enroled_you_as_X_into_the_course_Y,
             Txt_ROLES_SINGUL_abc[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Role][Gbl.Usrs.Me.UsrDat.Sex],
             Gbl.CurrentCrs.Crs.FullName);
   Ale_ShowAlert (Ale_INFO,Gbl.Alert.Txt);

   /***** Send button to accept register in the current course *****/
   switch (Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Role)
     {
      case Rol_STD:
	 Act_StartForm (ActAccEnrStd);
	 break;
      case Rol_NET:
	 Act_StartForm (ActAccEnrNET);
	 break;
      case Rol_TCH:
	 Act_StartForm (ActAccEnrTch);
	 break;
      default:
	 Lay_ShowErrorAndExit ("Wrong role.");
     }
   Btn_PutCreateButtonInline (Txt_Confirm_my_enrolment);
   Act_EndForm ();

   /***** Send button to refuse register in the current course *****/
   switch (Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Role)
     {
      case Rol_STD:
	 Act_StartForm (ActRemMe_Std);
	 break;
      case Rol_NET:
	 Act_StartForm (ActRemMe_NET);
	 break;
      case Rol_TCH:
	 Act_StartForm (ActRemMe_Tch);
	 break;
      default:
	 Lay_ShowErrorAndExit ("Wrong role.");
     }
   Btn_PutRemoveButtonInline (Txt_Remove_me_from_this_course);
   Act_EndForm ();

   /***** End box *****/
   Box_EndBox ();

   /***** Mark possible notification as seen *****/
   switch (Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Role)
     {
      case Rol_STD:
	 NotifyEvent = Ntf_EVENT_ENROLMENT_STD;
	 break;
      case Rol_NET:
	 NotifyEvent = Ntf_EVENT_ENROLMENT_NET;
	 break;
      case Rol_TCH:
	 NotifyEvent = Ntf_EVENT_ENROLMENT_TCH;
	 break;
      default:
	 NotifyEvent = Ntf_EVENT_UNKNOWN;
	 Lay_ShowErrorAndExit ("Wrong role.");
	 break;
     }
   Ntf_MarkNotifAsSeen (NotifyEvent,-1L,Gbl.CurrentCrs.Crs.CrsCod,
                        Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/****************** Put an enrolment into a notification ********************/
/*****************************************************************************/

void Enr_GetNotifEnrolment (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                            long CrsCod,long UsrCod)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct UsrData UsrDat;
   Rol_Role_t Role;

   SummaryStr[0] = '\0';        // Return nothing on error
   // This function may be called inside a web service, so don't report error

   /***** Get user's role in course from database *****/
   DB_BuildQuery ("SELECT Role"
		  " FROM crs_usr"
		  " WHERE CrsCod=%ld AND UsrCod=%ld",
                  CrsCod,UsrCod);
   if (DB_QuerySELECT_new (&mysql_res,"can not get user's role"
	                              " in course") == 1)	// Result should have a unique row
     {
      /***** Get user's role in course *****/
      row = mysql_fetch_row (mysql_res);

      /* Initialize structure with user's data */
      Usr_UsrDataConstructor (&UsrDat);

      /* Get user's data */
      UsrDat.UsrCod = UsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);

      /* Role (row[0]) */
      Role = Rol_ConvertUnsignedStrToRole (row[0]);
      Str_Copy (SummaryStr,Txt_ROLES_SINGUL_Abc[Role][UsrDat.Sex],
		Ntf_MAX_BYTES_SUMMARY);

      /* Free memory used for user's data */
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************************** Update user's data ****************************/
/*****************************************************************************/
// UsrDat->UsrCod must be > 0

void Enr_UpdateUsrData (struct UsrData *UsrDat)
  {
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];
   char BirthdayStrDB[Usr_BIRTHDAY_STR_DB_LENGTH + 1];
   char *Query;

   /***** Check if user's code is initialized *****/
   if (UsrDat->UsrCod <= 0)
      Lay_ShowErrorAndExit ("Can not update user's data. Wrong user's code.");

   /***** Filter some user's data before updating */
   Enr_FilterUsrDat (UsrDat);

   /***** Update user's common data *****/
   Usr_CreateBirthdayStrDB (UsrDat,BirthdayStrDB);	// It can include start and ending apostrophes
   if (asprintf (&Query,"UPDATE usr_data"
			" SET Password='%s',"
			"Surname1='%s',Surname2='%s',FirstName='%s',Sex='%s',"
			"CtyCod=%ld,"
			"LocalAddress='%s',LocalPhone='%s',"
			"FamilyAddress='%s',FamilyPhone='%s',"
			"OriginPlace='%s',Birthday=%s,"
			"Comments='%s'"
			" WHERE UsrCod=%ld",
	         UsrDat->Password,
	         UsrDat->Surname1,UsrDat->Surname2,UsrDat->FirstName,
	         Usr_StringsSexDB[UsrDat->Sex],
	         UsrDat->CtyCod,
	         UsrDat->LocalAddress,UsrDat->LocalPhone,
	         UsrDat->FamilyAddress,UsrDat->FamilyPhone,
	         UsrDat->OriginPlace,
	         BirthdayStrDB,
	         UsrDat->Comments ? UsrDat->Comments :
				    "",
	         UsrDat->UsrCod) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryUPDATE_free (Query,"can not update user's data");
  }

/*****************************************************************************/
/************************* Filter some user's data ***************************/
/*****************************************************************************/

void Enr_FilterUsrDat (struct UsrData *UsrDat)
  {
   /***** Fix birthday *****/
   if (UsrDat->Birthday.Year < Gbl.Now.Date.Year-99 ||
       UsrDat->Birthday.Year > Gbl.Now.Date.Year-16)
      UsrDat->Birthday.Year  =
      UsrDat->Birthday.Month =
      UsrDat->Birthday.Day   = 0;
  }

/*****************************************************************************/
/**************** Update institution, centre and department ******************/
/*****************************************************************************/

void Enr_UpdateInstitutionCentreDepartment (void)
  {
   char *Query;

   if (asprintf (&Query,"UPDATE usr_data"
			" SET InsCtyCod=%ld,InsCod=%ld,CtrCod=%ld,DptCod=%ld"
			" WHERE UsrCod=%ld",
	         Gbl.Usrs.Me.UsrDat.InsCtyCod,
	         Gbl.Usrs.Me.UsrDat.InsCod,
	         Gbl.Usrs.Me.UsrDat.Tch.CtrCod,
	         Gbl.Usrs.Me.UsrDat.Tch.DptCod,
	         Gbl.Usrs.Me.UsrDat.UsrCod) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryUPDATE_free (Query,"can not update institution, centre and department");
  }

/*****************************************************************************/
/************** Form to request the user's ID of another user ****************/
/*****************************************************************************/

void Enr_ReqAdminStds (void)
  {
   Enr_ReqAdminUsrs (Rol_STD);
  }

void Enr_ReqAdminNonEditingTchs (void)
  {
   Enr_ReqAdminUsrs (Rol_NET);
  }

void Enr_ReqAdminTchs (void)
  {
   Enr_ReqAdminUsrs (Rol_TCH);
  }

static void Enr_ReqAdminUsrs (Rol_Role_t Role)
  {
   extern const char *Txt_You_dont_have_permission_to_perform_this_action;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_GST:
      case Rol_STD:
      case Rol_NET:
	 Enr_AskIfRegRemMe (Role);
	 break;
      case Rol_TCH:
	 if (Gbl.CurrentCrs.Crs.CrsCod > 0 &&
	     Role == Rol_STD)
	    Enr_ShowFormRegRemSeveralUsrs (Rol_STD);
	 else
	    Enr_AskIfRegRemMe (Rol_TCH);
	 break;
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 if (Gbl.CurrentCrs.Crs.CrsCod > 0)
	    Enr_ShowFormRegRemSeveralUsrs (Role);
	 else
	    Enr_ReqAnotherUsrIDToRegisterRemove (Role);
	 break;
      default:
	 Ale_ShowAlert (Ale_ERROR,Txt_You_dont_have_permission_to_perform_this_action);
	 break;
     }
  }

/*****************************************************************************/
/***** Register/remove users (taken from a list) in/from current course ******/
/*****************************************************************************/

static void Enr_ShowFormRegRemSeveralUsrs (Rol_Role_t Role)
  {
   extern const char *Hlp_USERS_Administration_administer_multiple_users;
   extern const char *The_ClassTitle[The_NUM_THEMES];
   extern const char *Txt_Administer_multiple_students;
   extern const char *Txt_Administer_multiple_non_editing_teachers;
   extern const char *Txt_Administer_multiple_teachers;
   extern const char *Txt_Step_1_Provide_a_list_of_users;
   extern const char *Txt_Type_or_paste_a_list_of_IDs_nicks_or_emails_;
   extern const char *Txt_Step_2_Select_the_desired_action;
   extern const char *Txt_Step_3_Optionally_select_groups;
   extern const char *Txt_Select_the_groups_in_from_which_you_want_to_register_remove_users_;
   extern const char *Txt_No_groups_have_been_created_in_the_course_X_Therefore_;
   extern const char *Txt_Step_4_Confirm_the_enrolment_removing;
   extern const char *Txt_Confirm;
   Act_Action_t NextAction;
   const char *Title;

   /***** Put contextual links *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	 	// Course selected
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

      switch (Role)
	{
	 case Rol_STD:
            /* Put link to go to admin student */
            Enr_PutLinkToAdminOneUsr (ActReqMdfOneStd);

            /* Put link to remove all the students in the current course */
            if (Gbl.CurrentCrs.Crs.NumUsrs[Rol_STD])	// This course has students
               Enr_PutLinkToRemAllStdsThisCrs ();
	    break;
	 case Rol_NET:
            /* Put link to go to admin teacher */
            Enr_PutLinkToAdminOneUsr (ActReqMdfOneTch);
	    break;
	 case Rol_TCH:
            /* Put link to go to admin teacher */
            Enr_PutLinkToAdminOneUsr (ActReqMdfOneTch);
	    break;
	 default:
	    NextAction = ActUnk;
	    Title = NULL;
	    Lay_ShowErrorAndExit ("Wrong role.");
	    break;
	}

      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Form to send students to be enroled / removed *****/
   switch (Role)
     {
      case Rol_STD:
	 NextAction = ActRcvFrmEnrSevStd;
	 Title = Txt_Administer_multiple_students;
	 break;
      case Rol_NET:
	 NextAction = ActRcvFrmEnrSevNET;
	 Title = Txt_Administer_multiple_non_editing_teachers;
	 break;
      case Rol_TCH:
	 NextAction = ActRcvFrmEnrSevTch;
	 Title = Txt_Administer_multiple_teachers;
	 break;
      default:
	 NextAction = ActUnk;
	 Title = NULL;
	 Lay_ShowErrorAndExit ("Wrong role.");
	 break;
     }
   Act_StartForm (NextAction);

   /***** Start box *****/
   Box_StartBox (NULL,Title,NULL,
	         Hlp_USERS_Administration_administer_multiple_users,Box_NOT_CLOSABLE);

   /***** Step 1: List of students to be enroled / removed *****/
   fprintf (Gbl.F.Out,"<div class=\"%s LEFT_MIDDLE\">"
                      "%s"
                      "</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Step_1_Provide_a_list_of_users);

   Ale_ShowAlert (Ale_INFO,Txt_Type_or_paste_a_list_of_IDs_nicks_or_emails_);
   Enr_PutAreaToEnterUsrsIDs ();

   /***** Step 2: Put different actions to register/remove users to/from current course *****/
   fprintf (Gbl.F.Out,"<div class=\"%s LEFT_MIDDLE\">"
                      "%s"
                      "</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Step_2_Select_the_desired_action);
   Enr_PutActionsRegRemSeveralUsrs ();

   /***** Step 3: Select groups in which register / remove users *****/
   fprintf (Gbl.F.Out,"<div class=\"%s LEFT_MIDDLE\">"
                      "%s"
                      "</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Step_3_Optionally_select_groups);
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
     {
      if (Gbl.CurrentCrs.Grps.NumGrps)	// This course has groups?
	{
	 Ale_ShowAlert (Ale_INFO,Txt_Select_the_groups_in_from_which_you_want_to_register_remove_users_);
	 Grp_ShowLstGrpsToChgOtherUsrsGrps (-1L);
	}
      else
	{
	 /* Write help message */
	 snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	           Txt_No_groups_have_been_created_in_the_course_X_Therefore_,
		   Gbl.CurrentCrs.Crs.FullName);
	 Ale_ShowAlert (Ale_INFO,Gbl.Alert.Txt);
	}
     }

   /***** Step 4: Confirm register / remove students *****/
   fprintf (Gbl.F.Out,"<div class=\"%s LEFT_MIDDLE\">"
                      "%s"
                      "</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Step_4_Confirm_the_enrolment_removing);
   Pwd_AskForConfirmationOnDangerousAction ();

   /***** Send button and end box *****/
   Box_EndBoxWithButton (Btn_CONFIRM_BUTTON,Txt_Confirm);

   /***** End form *****/
   Act_EndForm ();
  }

/*****************************************************************************/
/******************** Put a link (form) to remove old users ******************/
/*****************************************************************************/

void Enr_PutLinkToRemOldUsrs (void)
  {
   extern const char *Txt_Eliminate_old_users;

   /***** Put form to remove old users *****/
   Lay_PutContextualLink (ActReqRemOldUsr,NULL,NULL,
                          "remove-on64x64.png",
                          Txt_Eliminate_old_users,Txt_Eliminate_old_users,
                          NULL);
  }

/*****************************************************************************/
/*********************** Write form to remove old users **********************/
/*****************************************************************************/

void Enr_AskRemoveOldUsrs (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Eliminate_old_users;
   extern const char *Txt_Eliminate_all_users_who_are_not_enroled_on_any_courses_PART_1_OF_2;
   extern const char *Txt_Eliminate_all_users_who_are_not_enroled_on_any_courses_PART_2_OF_2;
   extern const char *Txt_Eliminate;
   unsigned Months;

   /***** Start form *****/
   Act_StartForm (ActRemOldUsr);

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Eliminate_old_users,NULL,
                 NULL,Box_NOT_CLOSABLE);

   /***** Form to request number of months without clicks *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">%s&nbsp;",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Eliminate_all_users_who_are_not_enroled_on_any_courses_PART_1_OF_2);
   fprintf (Gbl.F.Out,"<select name=\"Months\">");
   for (Months  = Usr_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS;
        Months <= Usr_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS;
        Months++)
     {
      fprintf (Gbl.F.Out,"<option");
      if (Months == Usr_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%u</option>",Months);
     }
   fprintf (Gbl.F.Out,"</select>&nbsp;");
   fprintf (Gbl.F.Out,Txt_Eliminate_all_users_who_are_not_enroled_on_any_courses_PART_2_OF_2,
            Cfg_PLATFORM_SHORT_NAME);
   fprintf (Gbl.F.Out,"</label>");

   /***** Send button and end box *****/
   Box_EndBoxWithButton (Btn_REMOVE_BUTTON,Txt_Eliminate);

   /***** End form *****/
   Act_EndForm ();
  }

/*****************************************************************************/
/******************************* Remove old users ****************************/
/*****************************************************************************/

void Enr_RemoveOldUsrs (void)
  {
   extern const char *Txt_Eliminating_X_users_who_were_not_enroled_in_any_course_and_with_more_than_Y_months_without_access_to_Z;
   extern const char *Txt_X_users_have_been_eliminated;
   unsigned MonthsWithoutAccess;
   time_t SecondsWithoutAccess;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumUsr;
   unsigned long NumUsrs;
   unsigned NumUsrsEliminated = 0;
   struct UsrData UsrDat;

   /***** Get parameter with number of months without access *****/
   MonthsWithoutAccess = (unsigned)
	                 Par_GetParToUnsignedLong ("Months",
                                                   Usr_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS,
                                                   Usr_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS,
                                                   (unsigned long) UINT_MAX);
   if (MonthsWithoutAccess == UINT_MAX)
      Lay_ShowErrorAndExit ("Wrong number of months.");
   SecondsWithoutAccess = (time_t) MonthsWithoutAccess * Dat_SECONDS_IN_ONE_MONTH;

   /***** Get old users from database *****/
   DB_BuildQuery ("SELECT UsrCod FROM"
		  "("
		  "SELECT UsrCod FROM usr_last WHERE"
		  " LastTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-'%lu')"
		  " UNION "
		  "SELECT UsrCod FROM usr_data WHERE"
		  " UsrCod NOT IN (SELECT UsrCod FROM usr_last)"
		  ") AS candidate_usrs"
		  " WHERE UsrCod NOT IN (SELECT DISTINCT UsrCod FROM crs_usr)",
                  (unsigned long) SecondsWithoutAccess);
   if ((NumUsrs = DB_QuerySELECT_new (&mysql_res,"can not get old users")))
     {
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        Txt_Eliminating_X_users_who_were_not_enroled_in_any_course_and_with_more_than_Y_months_without_access_to_Z,
                NumUsrs,
                MonthsWithoutAccess,
                Cfg_PLATFORM_SHORT_NAME);
      Ale_ShowAlert (Ale_INFO,Gbl.Alert.Txt);

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
            Acc_CompletelyEliminateAccount (&UsrDat,Cns_QUIET);
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
   snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	     Txt_X_users_have_been_eliminated,
             NumUsrsEliminated);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
  }

/*****************************************************************************/
/***** Put text area to enter/paste IDs of users to be enroled/removed ******/
/*****************************************************************************/

static void Enr_PutAreaToEnterUsrsIDs (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_List_of_nicks_emails_or_IDs;

   /***** Text area for users' IDs *****/
   Tbl_StartTableCenter (2);
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_TOP\">"
                      "<label for=\"UsrsIDs\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea id=\"UsrsIDs\" name=\"UsrsIDs\""
                      " cols=\"60\" rows=\"10\">"
                      "</textarea>"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_List_of_nicks_emails_or_IDs);
   Tbl_EndTable ();
  }

/*****************************************************************************/
/*** Put different actions to register/remove users to/from current course ***/
/*****************************************************************************/
// Returns true if at least one action can be shown

bool Enr_PutActionsRegRemOneUsr (bool ItsMe)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Modify_me_in_the_course_X;
   extern const char *Txt_Modify_user_in_the_course_X;
   extern const char *Txt_Register_me_in_X;
   extern const char *Txt_Register_USER_in_the_course_X;
   extern const char *Txt_Report_possible_duplicate_user;
   extern const char *Txt_Register_USER_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Register_USER_as_an_administrator_of_the_centre_X;
   extern const char *Txt_Register_USER_as_an_administrator_of_the_institution_X;
   extern const char *Txt_Remove_me_from_THE_COURSE_X;
   extern const char *Txt_Remove_USER_from_THE_COURSE_X;
   extern const char *Txt_Remove_me_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Remove_USER_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Remove_me_as_an_administrator_of_the_centre_X;
   extern const char *Txt_Remove_USER_as_an_administrator_of_the_centre_X;
   extern const char *Txt_Remove_me_as_an_administrator_of_the_institution_X;
   extern const char *Txt_Remove_USER_as_an_administrator_of_the_institution_X;
   extern const char *Txt_Eliminate_my_user_account;
   extern const char *Txt_Eliminate_user_account;
   unsigned NumOptionsShown = 0;
   bool UsrBelongsToCrs = false;
   bool UsrIsDegAdmin = false;
   bool UsrIsCtrAdmin = false;
   bool UsrIsInsAdmin = false;
   bool OptionChecked = false;

   /***** Check if the other user belongs to the current course *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
      UsrBelongsToCrs = Usr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat);

   if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      /***** Check if the other user is administrator of the current institution *****/
      UsrIsInsAdmin = Usr_CheckIfUsrIsAdm (Gbl.Usrs.Other.UsrDat.UsrCod,
					   Sco_SCOPE_INS,
					   Gbl.CurrentIns.Ins.InsCod);

      if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
	{
	 /***** Check if the other user is administrator of the current centre *****/
	 UsrIsCtrAdmin = Usr_CheckIfUsrIsAdm (Gbl.Usrs.Other.UsrDat.UsrCod,
					      Sco_SCOPE_CTR,
					      Gbl.CurrentCtr.Ctr.CtrCod);

	 if (Gbl.CurrentDeg.Deg.DegCod > 0)
	    /***** Check if the other user is administrator of the current degree *****/
	    UsrIsDegAdmin = Usr_CheckIfUsrIsAdm (Gbl.Usrs.Other.UsrDat.UsrCod,
						 Sco_SCOPE_DEG,
						 Gbl.CurrentDeg.Deg.DegCod);
	}
     }

   /***** Start list of options *****/
   fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT %s\" style=\"margin:12px;\">",
	    The_ClassForm[Gbl.Prefs.Theme]);

   /***** Register user in course / Modify user's data *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0 &&
       Gbl.Usrs.Me.Role.Logged >= Rol_STD)
     {
      fprintf (Gbl.F.Out,"<li>"
			 "<input type=\"radio\" id=\"RegRemAction%u\""
			 " name=\"RegRemAction\" value=\"%u\"",
               (unsigned) Enr_REGISTER_MODIFY_ONE_USR_IN_CRS,
               (unsigned) Enr_REGISTER_MODIFY_ONE_USR_IN_CRS);
      if (!OptionChecked)
	{
	 fprintf (Gbl.F.Out," checked=\"checked\"");
         OptionChecked = true;
	}
      fprintf (Gbl.F.Out," />"
                         "<label for=\"RegRemAction%u\">",
	       (unsigned) Enr_REGISTER_MODIFY_ONE_USR_IN_CRS);
      fprintf (Gbl.F.Out,
	       UsrBelongsToCrs ? (ItsMe ? Txt_Modify_me_in_the_course_X :
		                          Txt_Modify_user_in_the_course_X) :
	                         (ItsMe ? Txt_Register_me_in_X :
		                          Txt_Register_USER_in_the_course_X),
	       Gbl.CurrentCrs.Crs.ShrtName);
      fprintf (Gbl.F.Out,"</label>"
	                 "</li>");

      NumOptionsShown++;
     }

   if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
	{
	 if (Gbl.CurrentDeg.Deg.DegCod > 0)
	    /***** Register user as administrator of degree *****/
	    if (!UsrIsDegAdmin &&
		Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
	      {
	       fprintf (Gbl.F.Out,"<li>"
				  "<input type=\"radio\" id=\"RegRemAction%u\""
				  " name=\"RegRemAction\" value=\"%u\"",
			(unsigned) Enr_REGISTER_ONE_DEGREE_ADMIN,
			(unsigned) Enr_REGISTER_ONE_DEGREE_ADMIN);
	       if (!OptionChecked)
		 {
		  fprintf (Gbl.F.Out," checked=\"checked\"");
		  OptionChecked = true;
		 }
	       fprintf (Gbl.F.Out," />"
                                  "<label for=\"RegRemAction%u\">",
			(unsigned) Enr_REGISTER_ONE_DEGREE_ADMIN);
	       fprintf (Gbl.F.Out,
		        Txt_Register_USER_as_an_administrator_of_the_degree_X,
			Gbl.CurrentDeg.Deg.ShrtName);
	       fprintf (Gbl.F.Out,"</label>"
		                  "</li>");

	       NumOptionsShown++;
	      }

	 /***** Register user as administrator of centre *****/
	 if (!UsrIsCtrAdmin &&
	     Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
	   {
	    fprintf (Gbl.F.Out,"<li>"
			       "<input type=\"radio\" id=\"RegRemAction%u\""
			       " name=\"RegRemAction\" value=\"%u\"",
		     (unsigned) Enr_REGISTER_ONE_CENTRE_ADMIN,
		     (unsigned) Enr_REGISTER_ONE_CENTRE_ADMIN);
	    if (!OptionChecked)
	      {
	       fprintf (Gbl.F.Out," checked=\"checked\"");
	       OptionChecked = true;
	      }
	    fprintf (Gbl.F.Out," />"
	                       "<label for=\"RegRemAction%u\">",
		     (unsigned) Enr_REGISTER_ONE_CENTRE_ADMIN);
	    fprintf (Gbl.F.Out,
	             Txt_Register_USER_as_an_administrator_of_the_centre_X,
		     Gbl.CurrentCtr.Ctr.ShrtName);
	    fprintf (Gbl.F.Out,"</label>"
		               "</li>");

	    NumOptionsShown++;
	   }
	}

      /***** Register user as administrator of institution *****/
      if (!UsrIsInsAdmin &&
	  Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	{
	 fprintf (Gbl.F.Out,"<li>"
			    "<input type=\"radio\" id=\"RegRemAction%u\""
			    " name=\"RegRemAction\" value=\"%u\"",
		  (unsigned) Enr_REGISTER_ONE_INSTITUTION_ADMIN,
		  (unsigned) Enr_REGISTER_ONE_INSTITUTION_ADMIN);
	 if (!OptionChecked)
	   {
	    fprintf (Gbl.F.Out," checked=\"checked\"");
	    OptionChecked = true;
	   }
	 fprintf (Gbl.F.Out," />"
	                    "<label for=\"RegRemAction%u\">",
		  (unsigned) Enr_REGISTER_ONE_INSTITUTION_ADMIN);
	 fprintf (Gbl.F.Out,Txt_Register_USER_as_an_administrator_of_the_institution_X,
		  Gbl.CurrentIns.Ins.ShrtName);
	 fprintf (Gbl.F.Out,"</label>"
	                    "</li>");

	 NumOptionsShown++;
	}
     }

   /***** Report user as possible duplicate *****/
   if (!ItsMe && Gbl.Usrs.Me.Role.Logged >= Rol_TCH)
     {
      fprintf (Gbl.F.Out,"<li>"
			 "<input type=\"radio\" id=\"RegRemAction%u\""
			 " name=\"RegRemAction\" value=\"%u\"",
	       (unsigned) Enr_REPORT_USR_AS_POSSIBLE_DUPLICATE,
	       (unsigned) Enr_REPORT_USR_AS_POSSIBLE_DUPLICATE);
      if (!OptionChecked)
	{
	 fprintf (Gbl.F.Out," checked=\"checked\"");
	 OptionChecked = true;
	}
      fprintf (Gbl.F.Out," />"
                         "<label for=\"RegRemAction%u\">"
	                 "%s"
                         "</label>"
	                 "</li>",
	       (unsigned) Enr_REPORT_USR_AS_POSSIBLE_DUPLICATE,
	       Txt_Report_possible_duplicate_user);

      NumOptionsShown++;
     }

   /***** Remove user from the course *****/
   if (UsrBelongsToCrs)
     {
      fprintf (Gbl.F.Out,"<li>"
			 "<input type=\"radio\" id=\"RegRemAction%u\""
			 " name=\"RegRemAction\" value=\%u\"",
	       (unsigned) Enr_REMOVE_ONE_USR_FROM_CRS,
	       (unsigned) Enr_REMOVE_ONE_USR_FROM_CRS);
      if (!OptionChecked)
	{
	 fprintf (Gbl.F.Out," checked=\"checked\"");
	 OptionChecked = true;
	}
      fprintf (Gbl.F.Out," />"
                         "<label for=\"RegRemAction%u\">",
	       (unsigned) Enr_REMOVE_ONE_USR_FROM_CRS);
      fprintf (Gbl.F.Out,
	       ItsMe ? Txt_Remove_me_from_THE_COURSE_X :
		       Txt_Remove_USER_from_THE_COURSE_X,
	       Gbl.CurrentCrs.Crs.ShrtName);
      fprintf (Gbl.F.Out,"</label>"
	                 "</li>");

      NumOptionsShown++;
     }

   if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
	{
	 if (Gbl.CurrentIns.Ins.InsCod > 0)
	    /***** Remove user as an administrator of the degree *****/
	    if (UsrIsDegAdmin &&
		(ItsMe || Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM))
	      {
	       fprintf (Gbl.F.Out,"<li>"
				  "<input type=\"radio\" id=\"RegRemAction%u\" "
				  " name=\"RegRemAction\" value=\"%u\"",
			(unsigned) Enr_REMOVE_ONE_DEGREE_ADMIN,
			(unsigned) Enr_REMOVE_ONE_DEGREE_ADMIN);
	       if (!OptionChecked)
		 {
		  fprintf (Gbl.F.Out," checked=\"checked\"");
		  OptionChecked = true;
		 }
	       fprintf (Gbl.F.Out," />"
	                          "<label for=\"RegRemAction%u\">",
			(unsigned) Enr_REMOVE_ONE_DEGREE_ADMIN);
	       fprintf (Gbl.F.Out,
			ItsMe ? Txt_Remove_me_as_an_administrator_of_the_degree_X :
				Txt_Remove_USER_as_an_administrator_of_the_degree_X,
			Gbl.CurrentDeg.Deg.ShrtName);
	       fprintf (Gbl.F.Out,"</label>"
		                  "</li>");

	       NumOptionsShown++;
	      }

          /***** Remove user as an administrator of the centre *****/
	  if (UsrIsCtrAdmin &&
	     (ItsMe || Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM))
	   {
	    fprintf (Gbl.F.Out,"<li>"
			       "<input type=\"radio\" id=\"RegRemAction%u\""
			       " name=\"RegRemAction\" value=\"%u\"",
		     (unsigned) Enr_REMOVE_ONE_CENTRE_ADMIN,
		     (unsigned) Enr_REMOVE_ONE_CENTRE_ADMIN);
	    if (!OptionChecked)
	      {
	       fprintf (Gbl.F.Out," checked=\"checked\"");
	       OptionChecked = true;
	      }
	    fprintf (Gbl.F.Out," />"
	                       "<label for=\"RegRemAction%u\">",
		     (unsigned) Enr_REMOVE_ONE_CENTRE_ADMIN);
	    fprintf (Gbl.F.Out,
		     ItsMe ? Txt_Remove_me_as_an_administrator_of_the_centre_X :
			     Txt_Remove_USER_as_an_administrator_of_the_centre_X,
		     Gbl.CurrentCtr.Ctr.ShrtName);
	    fprintf (Gbl.F.Out,"</label>"
		               "</li>");

	    NumOptionsShown++;
	   }
	}

      /***** Remove user as an administrator of the institution *****/
      if (UsrIsInsAdmin &&
	  (ItsMe || Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM))
	{
	 fprintf (Gbl.F.Out,"<li>"
			    "<input type=\"radio\" id=\"RegRemAction%u\""
			    " name=\"RegRemAction\" value=\"%u\"",
		  (unsigned) Enr_REMOVE_ONE_INSTITUTION_ADMIN,
		  (unsigned) Enr_REMOVE_ONE_INSTITUTION_ADMIN);
	 if (!OptionChecked)
	   {
	    fprintf (Gbl.F.Out," checked=\"checked\"");
	    OptionChecked = true;
	   }
	 fprintf (Gbl.F.Out," />"
	                    "<label for=\"RegRemAction%u\">",
		  (unsigned) Enr_REMOVE_ONE_INSTITUTION_ADMIN);
	 fprintf (Gbl.F.Out,
		  ItsMe ? Txt_Remove_me_as_an_administrator_of_the_institution_X :
			  Txt_Remove_USER_as_an_administrator_of_the_institution_X,
		  Gbl.CurrentIns.Ins.ShrtName);
	 fprintf (Gbl.F.Out,"</label>"
	                    "</li>");

	 NumOptionsShown++;
	}
     }

   /***** Eliminate user completely from platform *****/
   if (Acc_CheckIfICanEliminateAccount (Gbl.Usrs.Other.UsrDat.UsrCod))
     {
      fprintf (Gbl.F.Out,"<li>"
                         "<input type=\"radio\" id=\"RegRemAction%u\""
                         " name=\"RegRemAction\" value=\"%u\"",
               (unsigned) Enr_ELIMINATE_ONE_USR_FROM_PLATFORM,
               (unsigned) Enr_ELIMINATE_ONE_USR_FROM_PLATFORM);
      if (!OptionChecked)
	 fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," />"
                         "<label for=\"RegRemAction%u\">"
	                 "%s"
                         "</label>"
	                 "</li>",
               (unsigned) Enr_ELIMINATE_ONE_USR_FROM_PLATFORM,
               ItsMe ? Txt_Eliminate_my_user_account :
        	       Txt_Eliminate_user_account);

      NumOptionsShown++;
     }

   /***** End list of options *****/
   fprintf (Gbl.F.Out,"</ul>");

   return (NumOptionsShown ? true :
	                     false);
  }

/*****************************************************************************/
/*** Put different actions to register/remove users to/from current course ***/
/*****************************************************************************/

static void Enr_PutActionsRegRemSeveralUsrs (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Register_the_users_indicated_in_step_1;
   extern const char *Txt_Remove_the_users_indicated_in_step_1;
   extern const char *Txt_Remove_the_users_not_indicated_in_step_1;
   extern const char *Txt_Register_the_users_indicated_in_step_1_and_remove_the_users_not_indicated;
   extern const char *Txt_Eliminate_from_the_platform_the_users_indicated_in_step_1;

   /***** Start list of options *****/
   fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT %s\" style=\"margin:12px;\">",
            The_ClassForm[Gbl.Prefs.Theme]);

   /***** Register / remove users listed or not listed *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
      fprintf (Gbl.F.Out,"<li>"
			 "<input type=\"radio\" id=\"RegRemAction%u\""
			 " name=\"RegRemAction\" value=\"%u\" checked=\"checked\" />"
			 "<label for=\"RegRemAction%u\">"
			 "%s"
			 "</label>"
			 "</li>"
			 "<li>"
			 "<input type=\"radio\" id=\"RegRemAction%u\""
			 " name=\"RegRemAction\" value=\"%u\" />"
			 "<label for=\"RegRemAction%u\">"
			 "%s"
			 "</label>"
			 "</li>"
			 "<li>"
			 "<input type=\"radio\" id=\"RegRemAction%u\""
			 " name=\"RegRemAction\" value=\"%u\" />"
			 "<label for=\"RegRemAction%u\">"
			 "%s"
			 "</label>"
			 "</li>"
			 "<li>"
			 "<input id=\"RegRemAction%u\" type=\"radio\""
			 " name=\"RegRemAction\" value=\"%u\" />"
			 "<label for=\"RegRemAction%u\">"
			 "%s"
			 "</label>"
			 "</li>",
	       (unsigned) Enr_REGISTER_SPECIFIED_USRS_IN_CRS,
	       (unsigned) Enr_REGISTER_SPECIFIED_USRS_IN_CRS,
	       (unsigned) Enr_REGISTER_SPECIFIED_USRS_IN_CRS,
	       Txt_Register_the_users_indicated_in_step_1,
	       (unsigned) Enr_REMOVE_SPECIFIED_USRS_FROM_CRS,
	       (unsigned) Enr_REMOVE_SPECIFIED_USRS_FROM_CRS,
	       (unsigned) Enr_REMOVE_SPECIFIED_USRS_FROM_CRS,
	       Txt_Remove_the_users_indicated_in_step_1,
	       (unsigned) Enr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS,
	       (unsigned) Enr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS,
	       (unsigned) Enr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS,
	       Txt_Remove_the_users_not_indicated_in_step_1,
	       (unsigned) Enr_UPDATE_USRS_IN_CRS,
	       (unsigned) Enr_UPDATE_USRS_IN_CRS,
	       (unsigned) Enr_UPDATE_USRS_IN_CRS,
	       Txt_Register_the_users_indicated_in_step_1_and_remove_the_users_not_indicated);

   /***** Only for superusers *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      fprintf (Gbl.F.Out,"<li>"
                         "<input type=\"radio\" id=\"RegRemAction%u\""
                         " name=\"RegRemAction\" value=\"%u\" />"
			 "<label for=\"RegRemAction%u\">"
                         "%s"
			 "</label>"
                         "</li>",
               (unsigned) Enr_ELIMINATE_USRS_FROM_PLATFORM,
               (unsigned) Enr_ELIMINATE_USRS_FROM_PLATFORM,
               (unsigned) Enr_ELIMINATE_USRS_FROM_PLATFORM,
               Txt_Eliminate_from_the_platform_the_users_indicated_in_step_1);

   /***** End list of options *****/
   fprintf (Gbl.F.Out,"</ul>");
  }

/*****************************************************************************/
/******* Receive the list of users of the course to register/remove **********/
/*****************************************************************************/

void Enr_ReceiveFormAdminStds (void)
  {
   Enr_ReceiveFormUsrsCrs (Rol_STD);
  }

void Enr_ReceiveFormAdminNonEditTchs (void)
  {
   Enr_ReceiveFormUsrsCrs (Rol_NET);
  }

void Enr_ReceiveFormAdminTchs (void)
  {
   Enr_ReceiveFormUsrsCrs (Rol_TCH);
  }

static void Enr_ReceiveFormUsrsCrs (Rol_Role_t Role)
  {
   extern const char *Txt_In_a_type_of_group_with_single_enrolment_students_can_not_be_registered_in_more_than_one_group;
   extern const char *Txt_No_user_has_been_eliminated;
   extern const char *Txt_One_user_has_been_eliminated;
   extern const char *Txt_No_user_has_been_removed;
   extern const char *Txt_One_user_has_been_removed;
   extern const char *Txt_X_users_have_been_eliminated;
   extern const char *Txt_X_users_have_been_removed;
   extern const char *Txt_No_user_has_been_enroled;
   extern const char *Txt_One_user_has_been_enroled;
   extern const char *Txt_X_users_have_been_enroled_including_possible_repetitions;
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
   unsigned NumUsrsRegistered = 0;
   unsigned NumUsrsRemoved = 0;
   unsigned NumUsrsEliminated = 0;
   struct ListCodGrps LstGrps;
   struct UsrData UsrDat;
   bool ItLooksLikeAUsrID;
   Enr_RegRemUsrsAction_t RegRemUsrsAction;

   /***** Check the role of users to register / remove *****/
   switch (Role)
     {
      case Rol_STD:
	 if (Gbl.Usrs.Me.Role.Logged < Rol_TCH)		// Can I register/remove students?
	    // No, I can not
	    Lay_ShowErrorAndExit ("You are not allowed to perform this action.");
	 break;
      case Rol_NET:
      case Rol_TCH:
	 if (Gbl.Usrs.Me.Role.Logged < Rol_DEG_ADM)	// Can I register/remove teachers?
	    // No, I can not
	    Lay_ShowErrorAndExit ("You are not allowed to perform this action.");
	 break;
      default:
	 Lay_ShowErrorAndExit ("Wrong role.");
	 break;
     }

   /***** Get confirmation *****/
   if (!Pwd_GetConfirmationOnDangerousAction ())
      return;

   /***** Get the action to do *****/
   WhatToDo.RemoveUsrs = false;
   WhatToDo.RemoveSpecifiedUsrs = false;
   WhatToDo.EliminateUsrs = false;
   WhatToDo.RegisterUsrs = false;

   RegRemUsrsAction = (Enr_RegRemUsrsAction_t)
	              Par_GetParToUnsignedLong ("RegRemAction",
                                                0,
                                                Enr_NUM_REG_REM_USRS_ACTIONS - 1,
                                                (unsigned long) Enr_REG_REM_USRS_UNKNOWN_ACTION);
   switch (RegRemUsrsAction)
     {
      case Enr_REGISTER_SPECIFIED_USRS_IN_CRS:
	 WhatToDo.RemoveUsrs = false;
	 WhatToDo.RemoveSpecifiedUsrs = false;	// Ignored
	 WhatToDo.EliminateUsrs = false;		// Ignored
	 WhatToDo.RegisterUsrs = true;
	 break;
      case Enr_REMOVE_SPECIFIED_USRS_FROM_CRS:
	 WhatToDo.RemoveUsrs = true;
	 WhatToDo.RemoveSpecifiedUsrs = true;
	 WhatToDo.EliminateUsrs = false;
	 WhatToDo.RegisterUsrs = false;
	 break;
      case Enr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS:
	 WhatToDo.RemoveUsrs = true;
	 WhatToDo.RemoveSpecifiedUsrs = false;
	 WhatToDo.EliminateUsrs = false;
	 WhatToDo.RegisterUsrs = false;
	 break;
      case Enr_UPDATE_USRS_IN_CRS:
	 WhatToDo.RemoveUsrs = true;
	 WhatToDo.RemoveSpecifiedUsrs = false;
	 WhatToDo.EliminateUsrs = false;
	 WhatToDo.RegisterUsrs = true;
	 break;
      case Enr_ELIMINATE_USRS_FROM_PLATFORM:
	 if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
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

   /***** Get groups to which register/remove users *****/
   LstGrps.NumGrps = 0;
   if (Gbl.CurrentCrs.Grps.NumGrps) // This course has groups?
     {
      /***** Get list of groups types and groups in current course *****/
      Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

      /***** Get the list of groups to which register/remove students *****/
      LstGrps.GrpCods = NULL;	// Initialized to avoid bug reported by Coverity
      LstGrps.NumGrps = 0;	// Initialized to avoid bug reported by Coverity
      Grp_GetLstCodsGrpWanted (&LstGrps);

      /***** A student can't belong to more than one group
             when the type of group only allows to register in one group *****/
      if (WhatToDo.RegisterUsrs)
	 /* Check if I have selected more than one group of single enrolment */
	 if (!Grp_CheckIfSelectionGrpsSingleEnrolmentIsValid (Role,&LstGrps))
	   {
	    /* Show warning message and exit */
	    Ale_ShowAlert (Ale_WARNING,Txt_In_a_type_of_group_with_single_enrolment_students_can_not_be_registered_in_more_than_one_group);

	    /* Free memory used by lists of groups and abort */
	    Grp_FreeListCodGrp (&LstGrps);
	    Grp_FreeListGrpTypesAndGrps ();
	    return;
	   }
     }

   /***** Get list of users' IDs *****/
   if ((ListUsrsIDs = (char *) malloc (ID_MAX_BYTES_LIST_USRS_IDS + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();
   Par_GetParToText ("UsrsIDs",ListUsrsIDs,ID_MAX_BYTES_LIST_USRS_IDS);

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Remove users *****/
   if (WhatToDo.RemoveUsrs)
     {
      /***** Get list of users in current course *****/
      Usr_GetListUsrs (Sco_SCOPE_CRS,Role);

      if (Gbl.Usrs.LstUsrs[Role].NumUsrs)
	{
	 /***** Initialize list of users to remove *****/
	 for (NumCurrentUsr = 0;
	      NumCurrentUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs;
	      NumCurrentUsr++)
	    Gbl.Usrs.LstUsrs[Role].Lst[NumCurrentUsr].Remove = !WhatToDo.RemoveSpecifiedUsrs;

	 /***** Loop 1: go through form list setting if a student must be removed *****/
	 /* Get users from a list of users' IDs */
	 Ptr = ListUsrsIDs;
	 while (*Ptr)
	   {
	    /* Reset user */
	    UsrDat.UsrCod = -1L;

	    /* Find next string in text */
	    Str_GetNextStringUntilSeparator (&Ptr,UsrDat.UsrIDNickOrEmail,
	                                     Cns_MAX_BYTES_EMAIL_ADDRESS);

	    /* Reset default list of users' codes */
	    ListUsrCods.NumUsrs = 0;
	    ListUsrCods.Lst = NULL;

	    /* Check if string is a user's ID, user's nickname or user's email address */
	    if (Nck_CheckIfNickWithArrobaIsValid (UsrDat.UsrIDNickOrEmail))	// 1: It's a nickname
	      {
	       if ((UsrDat.UsrCod = Nck_GetUsrCodFromNickname (UsrDat.UsrIDNickOrEmail)) > 0)
		 {
		  ListUsrCods.NumUsrs = 1;
		  Usr_AllocateListUsrCods (&ListUsrCods);
		  ListUsrCods.Lst[0] = UsrDat.UsrCod;
		 }
	      }
	    else if (Mai_CheckIfEmailIsValid (UsrDat.UsrIDNickOrEmail))	// 2: It's an email
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
	       if (ID_CheckIfUsrIDSeemsAValidID (UsrDat.UsrIDNickOrEmail))
		 {
		  /***** Find users for this user's ID *****/
		  ID_ReallocateListIDs (&UsrDat,1);	// Only one user's ID
		  Str_Copy (UsrDat.IDs.List[0].ID,UsrDat.UsrIDNickOrEmail,
			    ID_MAX_BYTES_USR_ID);
		  Str_ConvertToUpperText (UsrDat.IDs.List[0].ID);
		  ID_GetListUsrCodsFromUsrID (&UsrDat,NULL,&ListUsrCods,false);
		 }
	      }

	    if (WhatToDo.RemoveSpecifiedUsrs)	// Remove the specified users (of the role)
	      {
	       if (ListUsrCods.NumUsrs == 1)		// If more than one user found ==> do not remove
		  for (NumCurrentUsr = 0;
		       NumCurrentUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs;
		       NumCurrentUsr++)
		     if (Gbl.Usrs.LstUsrs[Role].Lst[NumCurrentUsr].UsrCod == ListUsrCods.Lst[0])	// User found
			Gbl.Usrs.LstUsrs[Role].Lst[NumCurrentUsr].Remove = true;	// Mark as removable
	      }
	    else	// Remove all the users (of the role) except these specified
	      {
	       for (NumCurrentUsr = 0;
		    NumCurrentUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs;
		    NumCurrentUsr++)
		  for (NumUsrFound = 0;
		       NumUsrFound < ListUsrCods.NumUsrs;
		       NumUsrFound++)
		     if (Gbl.Usrs.LstUsrs[Role].Lst[NumCurrentUsr].UsrCod == ListUsrCods.Lst[NumUsrFound])	// User found
			Gbl.Usrs.LstUsrs[Role].Lst[NumCurrentUsr].Remove = false;	// Mark as not removable
	      }

	    /* Free memory used for list of users' codes found for this ID */
	    Usr_FreeListUsrCods (&ListUsrCods);
	   }

	 /***** Loop 2: go through users list removing users *****/
	 for (NumCurrentUsr = 0;
	      NumCurrentUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs;
	      NumCurrentUsr++)
	    if (Gbl.Usrs.LstUsrs[Role].Lst[NumCurrentUsr].Remove)        // If this student must be removed
	      {
	       UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Role].Lst[NumCurrentUsr].UsrCod;
	       if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))        // If user's data exist...
		 {
		  if (WhatToDo.EliminateUsrs)                // Eliminate user completely from the platform
		    {
		     Acc_CompletelyEliminateAccount (&UsrDat,Cns_QUIET);                // Remove definitely the user from the platform
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
			   Enr_EffectivelyRemUsrFromCrs (&UsrDat,&Gbl.CurrentCrs.Crs,
							 Enr_DO_NOT_REMOVE_WORKS,Cns_QUIET);        // Remove user from the course
			   NumUsrsRemoved++;
			  }
		       }
		     else        // No groups
		       {
			Enr_EffectivelyRemUsrFromCrs (&UsrDat,&Gbl.CurrentCrs.Crs,
						      Enr_DO_NOT_REMOVE_WORKS,Cns_QUIET);        // Remove user from the course
			NumUsrsRemoved++;
		       }
		    }
		 }
	      }
	}

      /***** Free memory for users list *****/
      Usr_FreeUsrsList (Role);
     }

   /***** Register users *****/
   if (WhatToDo.RegisterUsrs)	// TODO: !!!!! NO CAMBIAR EL ROL DE LOS USUARIOS QUE YA ESTÉN EN LA ASIGNATURA SI HAY MÁS DE UN USUARIO ENCONTRADO PARA EL MISMO DNI !!!!!!
     {
      /***** Get users from a list of users' IDs ******/
      Ptr = ListUsrsIDs;
      while (*Ptr)
	{
	 /* Reset user */
	 UsrDat.UsrCod = -1L;
	 ItLooksLikeAUsrID = false;

	 /* Find next string in text */
	 Str_GetNextStringUntilSeparator (&Ptr,UsrDat.UsrIDNickOrEmail,
	                                  Cns_MAX_BYTES_EMAIL_ADDRESS);

	 /* Reset default list of users' codes */
	 ListUsrCods.NumUsrs = 0;
	 ListUsrCods.Lst = NULL;

	 /* Check if the string is a user's ID, a user's nickname or a user's email address */
	 if (Nck_CheckIfNickWithArrobaIsValid (UsrDat.UsrIDNickOrEmail))	// 1: It's a nickname
	   {
	    if ((UsrDat.UsrCod = Nck_GetUsrCodFromNickname (UsrDat.UsrIDNickOrEmail)) > 0)
	      {
	       ListUsrCods.NumUsrs = 1;
	       Usr_AllocateListUsrCods (&ListUsrCods);
	       ListUsrCods.Lst[0] = UsrDat.UsrCod;
	      }
	   }
	 else if (Mai_CheckIfEmailIsValid (UsrDat.UsrIDNickOrEmail))		// 2: It's an email
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
	    if (ID_CheckIfUsrIDSeemsAValidID (UsrDat.UsrIDNickOrEmail))
	      {
	       ItLooksLikeAUsrID = true;

	       /* Find users for this user's ID */
	       ID_ReallocateListIDs (&UsrDat,1);	// Only one user's ID
	       Str_Copy (UsrDat.IDs.List[0].ID,UsrDat.UsrIDNickOrEmail,
			 ID_MAX_BYTES_USR_ID);
	       Str_ConvertToUpperText (UsrDat.IDs.List[0].ID);
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
	       Enr_RegisterUsr (&UsrDat,Role,&LstGrps,&NumUsrsRegistered);
	      }
	 else if (ItLooksLikeAUsrID)	// User not found. He/she is a new user. Register him/her using ID
	    Enr_RegisterUsr (&UsrDat,Role,&LstGrps,&NumUsrsRegistered);

	 /* Free memory used for list of users' codes found for this ID */
	 Usr_FreeListUsrCods (&ListUsrCods);
	}
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);

   if (NumUsrsEliminated)
      /***** Move unused contents of messages to table of deleted contents of messages *****/
      Msg_MoveUnusedMsgsContentToDeleted ();

   /***** Write messages with the number of users enroled/removed *****/
   if (WhatToDo.RemoveUsrs)
     {
      if (WhatToDo.EliminateUsrs)        // Eliminate completely from the platform
	 switch (NumUsrsEliminated)
	   {
	    case 0:
	       Ale_ShowAlert (Ale_INFO,Txt_No_user_has_been_eliminated);
	       break;
	    case 1:
	       Ale_ShowAlert (Ale_SUCCESS,Txt_One_user_has_been_eliminated);
	       break;
	    default:
	       snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	                 Txt_X_users_have_been_eliminated,
			 NumUsrsEliminated);
	       Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
	       break;
	   }
      else                        // Only remove from course / groups
	 switch (NumUsrsRemoved)
	   {
	    case 0:
	       Ale_ShowAlert (Ale_INFO,Txt_No_user_has_been_removed);
	       break;
	    case 1:
	       Ale_ShowAlert (Ale_SUCCESS,Txt_One_user_has_been_removed);
	       break;
	    default:
	       snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	                 Txt_X_users_have_been_removed,
			 NumUsrsRemoved);
	       Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
	       break;
	   }
     }
   if (WhatToDo.RegisterUsrs)
      switch (NumUsrsRegistered)
	{
	 case 0:
	    Ale_ShowAlert (Ale_INFO,Txt_No_user_has_been_enroled);
	    break;
	 case 1:
	    Ale_ShowAlert (Ale_SUCCESS,Txt_One_user_has_been_enroled);
	    break;
	 default:
	    snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	              Txt_X_users_have_been_enroled_including_possible_repetitions,
		      NumUsrsRegistered);
	    Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
	    break;
	}

   /***** Free memory used by the list of user's IDs *****/
   free (ListUsrsIDs);

   /***** Free memory with the list of groups to/from which register/remove users *****/
   Grp_FreeListCodGrp (&LstGrps);

   /***** Free list of groups types and groups in current course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/********************** Register a user using his/her ID *********************/
/*****************************************************************************/
// If user does not exists, UsrDat->IDs must hold the user's ID

static void Enr_RegisterUsr (struct UsrData *UsrDat,Rol_Role_t RegRemRole,
                             struct ListCodGrps *LstGrps,unsigned *NumUsrsRegistered)
  {
   /***** Check if I can register this user *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_TCH &&
       RegRemRole != Rol_STD)
      Lay_ShowErrorAndExit ("A teacher only can register several users as students.");

   /***** Check if the record of the user exists and get the type of user *****/
   if (UsrDat->UsrCod > 0)	// User exists in database
      Usr_GetAllUsrDataFromUsrCod (UsrDat);	// Get user's data
   else				// User does not exist in database, create it using his/her ID!
     {
      // Reset user's data
      Usr_ResetUsrDataExceptUsrCodAndIDs (UsrDat);	// It's necessary, because the same struct UsrDat was used for former user

      // User does not exist in database; list of IDs is initialized
      UsrDat->IDs.List[0].Confirmed = true;	// If he/she is a new user ==> his/her ID will be stored as confirmed in database
      Acc_CreateNewUsr (UsrDat,
                        false);	// I am NOT creating my own account
     }

   /***** Register user in current course in database *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
     {
      if (Usr_CheckIfUsrBelongsToCurrentCrs (UsrDat))
	{
	 if (RegRemRole != UsrDat->Roles.InCurrentCrs.Role)	// The role must be updated
	    /* Modify role */
	    Enr_ModifyRoleInCurrentCrs (UsrDat,RegRemRole);
	}
      else        // User does not belong to this course
	 /* Register user */
	 Enr_RegisterUsrInCurrentCrs (UsrDat,RegRemRole,
	                              Enr_SET_ACCEPTED_TO_FALSE);

      /***** Register user in the selected groups *****/
      if (Gbl.CurrentCrs.Grps.NumGrps)	// If there are groups in the course
	 Grp_RegisterUsrIntoGroups (UsrDat,LstGrps);
     }

   (*NumUsrsRegistered)++;
  }

/*****************************************************************************/
/**** Put a link (form) to remove all the students in the current course *****/
/*****************************************************************************/

static void Enr_PutLinkToRemAllStdsThisCrs (void)
  {
   extern const char *Txt_Remove_all_students;

   /***** Put form to remove all the students in the current course *****/
   Lay_PutContextualLink (ActReqRemAllStdCrs,NULL,NULL,
                          "remove-on64x64.png",
                          Txt_Remove_all_students,Txt_Remove_all_students,
                          NULL);
  }

/*****************************************************************************/
/********** Ask for removing all the students from current course ************/
/*****************************************************************************/

void Enr_AskRemAllStdsThisCrs (void)
  {
   extern const char *Hlp_USERS_Administration_remove_all_students;
   extern const char *Txt_Remove_all_students;
   extern const char *Txt_Do_you_really_want_to_remove_the_X_students_from_the_course_Y_;

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Remove_all_students,NULL,
                 Hlp_USERS_Administration_remove_all_students,Box_NOT_CLOSABLE);

   if (Gbl.CurrentCrs.Crs.NumUsrs[Rol_STD])
     {
      /***** Show question and button to remove students *****/
      /* Start alert */
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        Txt_Do_you_really_want_to_remove_the_X_students_from_the_course_Y_,
                Gbl.CurrentCrs.Crs.NumUsrs[Rol_STD],
                Gbl.CurrentCrs.Crs.FullName);
      Ale_ShowAlertAndButton1 (Ale_QUESTION,Gbl.Alert.Txt);

      /* Show form to request confirmation */
      Act_StartForm (ActRemAllStdCrs);
      Grp_PutParamAllGroups ();
      Pwd_AskForConfirmationOnDangerousAction ();
      Btn_PutRemoveButton (Txt_Remove_all_students);
      Act_EndForm ();

      /* End alert */
      Ale_ShowAlertAndButton2 (ActUnk,NULL,NULL,NULL,Btn_NO_BUTTON,NULL);
     }
   else	// Gbl.CurrentCrs.Crs.NumUsrs[Rol_STD] == 0
      /***** Show warning indicating no students found *****/
      Usr_ShowWarningNoUsersFound (Rol_STD);

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/***************** Remove all the students from current course ***************/
/*****************************************************************************/

void Enr_RemAllStdsThisCrs (void)
  {
   extern const char *Txt_The_X_students_who_belonged_to_the_course_Y_have_been_removed_from_it;
   unsigned NumStdsInCrs;

   if (Pwd_GetConfirmationOnDangerousAction ())
     {
      if ((NumStdsInCrs = Enr_RemAllStdsInCrs (&Gbl.CurrentCrs.Crs)))
	{
	 snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	           Txt_The_X_students_who_belonged_to_the_course_Y_have_been_removed_from_it,
		   NumStdsInCrs,Gbl.CurrentCrs.Crs.FullName);
	 Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
	}
      else	// NumStdsInCrs == 0
	 /***** Show warning indicating no students found *****/
	 Usr_ShowWarningNoUsersFound (Rol_STD);
     }
  }

/*****************************************************************************/
/***************** Remove all the students from a given course ***************/
/*****************************************************************************/
// Returns the numbers of students in the course before deletion

unsigned Enr_RemAllStdsInCrs (struct Course *Crs)
  {
   unsigned NumStdsInCrs;
   unsigned NumUsr;

   /***** Get list of students in current course *****/
   Gbl.Usrs.ClassPhoto.AllGroups = true;        // Get all the students of the current course
   Usr_GetListUsrs (Sco_SCOPE_CRS,Rol_STD);
   NumStdsInCrs = Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs;

   /***** Remove all the students *****/
   for (NumUsr = 0;
	NumUsr < NumStdsInCrs;
	NumUsr++)
     {
      Gbl.Usrs.Other.UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].UsrCod;
      Enr_EffectivelyRemUsrFromCrs (&Gbl.Usrs.Other.UsrDat,Crs,
				    Enr_REMOVE_WORKS,Cns_QUIET);
     }

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (Rol_STD);

   return NumStdsInCrs;
  }

/*****************************************************************************/
/************* Form to request sign up in the current course *****************/
/*****************************************************************************/

void Enr_ReqSignUpInCrs (void)
  {
   extern const char *Txt_You_were_already_enroled_as_X_in_the_course_Y;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   /***** Check if I already belong to course *****/
   if (Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Role >= Rol_STD)
     {
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        Txt_You_were_already_enroled_as_X_in_the_course_Y,
                Txt_ROLES_SINGUL_abc[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Role][Gbl.Usrs.Me.UsrDat.Sex],
                Gbl.CurrentCrs.Crs.FullName);
      Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
     }
   else if (Gbl.Usrs.Me.Role.Logged == Rol_GST ||
	    Gbl.Usrs.Me.Role.Logged == Rol_USR)
      /***** Show form to modify only the user's role or the user's data *****/
      Rec_ShowFormSignUpInCrsWithMySharedRecord ();
   else
      Lay_ShowErrorAndExit ("You must be logged to sign up in a course.");        // This never should happen
  }

/*****************************************************************************/
/*********************** Sign up in the current course ***********************/
/*****************************************************************************/

void Enr_SignUpInCrs (void)
  {
   extern const char *Txt_You_were_already_enroled_as_X_in_the_course_Y;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Your_request_for_enrolment_as_X_in_the_course_Y_has_been_accepted_for_processing;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Rol_Role_t RoleFromForm;
   long ReqCod = -1L;

   /***** Check if I already belong to course *****/
   if (Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Role >= Rol_STD)
     {
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        Txt_You_were_already_enroled_as_X_in_the_course_Y,
                Txt_ROLES_SINGUL_abc[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Role][Gbl.Usrs.Me.UsrDat.Sex],
                Gbl.CurrentCrs.Crs.FullName);
      Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
     }
   else
     {
      /***** Get new role from record form *****/
      RoleFromForm = (Rol_Role_t)
	             Par_GetParToUnsignedLong ("Role",
                                               0,
                                               Rol_NUM_ROLES - 1,
                                               (unsigned long) Rol_UNK);

      /* Check if role is correct */
      if (!(RoleFromForm == Rol_STD ||
	    RoleFromForm == Rol_NET ||
            RoleFromForm == Rol_TCH))
         Lay_ShowErrorAndExit ("Wrong role.");

      /***** Try to get and old request of the same user in the same course from database *****/
      DB_BuildQuery ("SELECT ReqCod FROM crs_usr_requests"
		     " WHERE CrsCod=%ld AND UsrCod=%ld",
		     Gbl.CurrentCrs.Crs.CrsCod,
		     Gbl.Usrs.Me.UsrDat.UsrCod);
      if (DB_QuerySELECT_new (&mysql_res,"can not get enrolment request"))
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
         DB_BuildQuery ("UPDATE crs_usr_requests SET Role=%u,RequestTime=NOW()"
			" WHERE ReqCod=%ld AND CrsCod=%ld AND UsrCod=%ld",
		        (unsigned) RoleFromForm,
		        ReqCod,
		        Gbl.CurrentCrs.Crs.CrsCod,
		        Gbl.Usrs.Me.UsrDat.UsrCod);
         DB_QueryUPDATE_new ("can not update enrolment request");
        }
      else                // No request in database for this user in this course
        {
         DB_BuildQuery ("INSERT INTO crs_usr_requests"
			" (CrsCod,UsrCod,Role,RequestTime)"
			" VALUES"
			" (%ld,%ld,%u,NOW())",
		        Gbl.CurrentCrs.Crs.CrsCod,
		        Gbl.Usrs.Me.UsrDat.UsrCod,
		        (unsigned) RoleFromForm);
         ReqCod = DB_QueryINSERTandReturnCode_new ("can not save enrolment request");
        }

      /***** Show confirmation message *****/
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        Txt_Your_request_for_enrolment_as_X_in_the_course_Y_has_been_accepted_for_processing,
                Txt_ROLES_SINGUL_abc[RoleFromForm][Gbl.Usrs.Me.UsrDat.Sex],
                Gbl.CurrentCrs.Crs.FullName);
      Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);

      /***** Notify teachers or admins by email about the new enrolment request *****/
      // If this course has teachers ==> send notification to teachers
      // If this course has no teachers and I want to be a teacher ==> send notification to administrators or superusers
      if (Gbl.CurrentCrs.Crs.NumUsrs[Rol_TCH] ||
	  RoleFromForm == Rol_TCH)
         Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_ENROLMENT_REQUEST,ReqCod);
     }
  }

/*****************************************************************************/
/************** Put an enrolment request into a notification *****************/
/*****************************************************************************/

void Enr_GetNotifEnrolmentRequest (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                   char **ContentStr,
                                   long ReqCod,bool GetContent)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct UsrData UsrDat;
   Rol_Role_t DesiredRole;

   SummaryStr[0] = '\0';        // Return nothing on error
   // This function may be called inside a web service, so don't report error

   /***** Get user and requested role from database *****/
   DB_BuildQuery ("SELECT UsrCod,Role"
		  " FROM crs_usr_requests"
		  " WHERE ReqCod=%ld",
                  ReqCod);
   if (DB_QuerySELECT_new (&mysql_res,"can not get enrolment request") == 1)	// Result should have a unique row
     {
      /***** Get user and requested role *****/
      row = mysql_fetch_row (mysql_res);

      /* Initialize structure with user's data */
      Usr_UsrDataConstructor (&UsrDat);

      /* User's code (row[0]) */
      UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);

      /* Role (row[1]) */
      DesiredRole = Rol_ConvertUnsignedStrToRole (row[1]);
      Str_Copy (SummaryStr,Txt_ROLES_SINGUL_Abc[DesiredRole][UsrDat.Sex],
		Ntf_MAX_BYTES_SUMMARY);

      if (GetContent)
	 /* Write desired role into content */
	 if (asprintf (ContentStr,
		       "%s",	// TODO: Write more info in this content
		       Txt_ROLES_SINGUL_Abc[DesiredRole][UsrDat.Sex]) < 0)
	    Lay_NotEnoughMemoryExit ();

      /* Free memory used for user's data */
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******* Ask if reject the request for enrolment of a user in a course *******/
/*****************************************************************************/

void Enr_AskIfRejectSignUp (void)
  {
   extern const char *Txt_THE_USER_X_is_already_enroled_in_the_course_Y;
   extern const char *Txt_Do_you_really_want_to_reject_the_enrolment_request_;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Reject;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   Rol_Role_t Role;

   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))        // If user's data exist...
     {
      if (Usr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
        {
         /* User already belongs to this course */
         snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	           Txt_THE_USER_X_is_already_enroled_in_the_course_Y,
                   Gbl.Usrs.Other.UsrDat.FullName,Gbl.CurrentCrs.Crs.FullName);
         Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
         Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

         /* Remove inscription request because it has not sense */
         Enr_RemoveEnrolmentRequest (Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Other.UsrDat.UsrCod);
        }
      else        // User does not belong to this course
        {
         Role = Rol_GetRequestedRole (Gbl.Usrs.Other.UsrDat.UsrCod);
         if (Role == Rol_STD ||
             Role == Rol_NET ||
             Role == Rol_TCH)
           {
	    /***** Show question and button to reject user's enrolment request *****/
	    /* Start alert */
            snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	              Txt_Do_you_really_want_to_reject_the_enrolment_request_,
                      Gbl.Usrs.Other.UsrDat.FullName,
                      Txt_ROLES_SINGUL_abc[Role][Gbl.Usrs.Other.UsrDat.Sex],
                      Gbl.CurrentCrs.Crs.FullName);
	    Ale_ShowAlertAndButton1 (Ale_QUESTION,Gbl.Alert.Txt);

	    /* Show user's record */
            Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

	    /* End alert */
	    Ale_ShowAlertAndButton2 (ActRejSignUp,NULL,NULL,
	                             Usr_PutParamOtherUsrCodEncrypted,
				     Btn_REMOVE_BUTTON,Txt_Reject);
           }
         else
            Lay_ShowErrorAndExit ("Wrong role.");
        }
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/********* Reject the request for enrolment of a user in a course ***********/
/*****************************************************************************/

void Enr_RejectSignUp (void)
  {
   extern const char *Txt_THE_USER_X_is_already_enroled_in_the_course_Y;
   extern const char *Txt_Enrolment_of_X_rejected;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))        // If user's data exist...
     {
      if (Usr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
        {
         /* User already belongs to this course */
         snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	           Txt_THE_USER_X_is_already_enroled_in_the_course_Y,
                   Gbl.Usrs.Other.UsrDat.FullName,Gbl.CurrentCrs.Crs.FullName);
         Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
         Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
        }

      /* Remove inscription request */
      Enr_RemoveEnrolmentRequest (Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Other.UsrDat.UsrCod);

      /* Confirmation message */
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        Txt_Enrolment_of_X_rejected,
                Gbl.Usrs.Other.UsrDat.FullName);
      Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);

   /* Show again the rest of registrarion requests */
   Enr_ShowEnrolmentRequests ();
  }

/*****************************************************************************/
/******** Show pending requests for enrolment in the current course *********/
/*****************************************************************************/

void Enr_ShowEnrolmentRequests (void)
  {
   /***** Show enrolment request (default roles depend on my logged role) *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
	 Enr_ShowEnrolmentRequestsGivenRoles ((1 << Rol_STD) |
	                                      (1 << Rol_NET) |
			                      (1 << Rol_TCH));
	 break;
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 Enr_ShowEnrolmentRequestsGivenRoles ((1 << Rol_NET) |
			                      (1 << Rol_TCH));
	 break;
      default:
	 Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
	 break;
     }
  }

/*****************************************************************************/
/******* Update pending requests for enrolment in the current course ********/
/*****************************************************************************/

void Enr_UpdateEnrolmentRequests (void)
  {
   unsigned RolesSelected;

   /***** Get selected roles *****/
   RolesSelected = Rol_GetSelectedRoles ();

   /***** Update enrolment requests *****/
   Enr_ShowEnrolmentRequestsGivenRoles (RolesSelected);
  }

/*****************************************************************************/
/************* Show pending requests for enrolment given roles **************/
/*****************************************************************************/

static void Enr_ShowEnrolmentRequestsGivenRoles (unsigned RolesSelected)
  {
   extern const char *Hlp_USERS_Requests;
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Enrolment_requests;
   extern const char *Txt_Scope;
   extern const char *Txt_Users;
   extern const char *Txt_Course;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Requester;
   extern const char *Txt_Role;
   extern const char *Txt_Date;
   extern const char *Txt_Go_to_X;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Register;
   extern const char *Txt_Reject;
   extern const char *Txt_No_enrolment_requests;
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
   char PhotoURL[PATH_MAX + 1];
   Rol_Role_t DesiredRole;
   Act_Action_t NextAction;

   /***** Remove expired enrolment requests *****/
   Enr_RemoveExpiredEnrolmentRequests ();

   /***** Get scope *****/
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	               1 << Sco_SCOPE_CTY |
                       1 << Sco_SCOPE_INS |
                       1 << Sco_SCOPE_CTR |
                       1 << Sco_SCOPE_DEG |
                       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   Sco_GetScope ("ScopeEnr");

   /***** Start box *****/
   Box_StartBox ("100%",Txt_Enrolment_requests,NULL,
                 Hlp_USERS_Requests,Box_NOT_CLOSABLE);

   /***** Selection of scope and roles *****/
   /* Start form and table */
   Act_StartForm (ActUpdSignUpReq);
   Tbl_StartTableWideMargin (2);

   /* Scope (whole platform, current centre, current degree or current course) */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label for=\"ScopeEnr\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Scope);
   Sco_PutSelectorScope ("ScopeEnr",true);
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");

   /* Users' roles in listing */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_TOP\">"
                      "<label class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"DAT LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Users);
   Rol_WriteSelectorRoles (1 << Rol_STD |
                           1 << Rol_NET |
                           1 << Rol_TCH,
                           RolesSelected,
                           false,true);
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");

   /* End table and form */
   Tbl_EndTable ();
   Act_EndForm ();

   /***** Build query *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:                // Show requesters for the whole platform
         switch (Gbl.Usrs.Me.Role.Logged)
           {
            case Rol_TCH:
               // Requests in all courses in which I am teacher
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM crs_usr,crs_usr_requests"
			      " WHERE crs_usr.UsrCod=%ld"
			      " AND crs_usr.Role=%u"
			      " AND crs_usr.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
			      Gbl.Usrs.Me.UsrDat.UsrCod,
			      (unsigned) Rol_TCH,
			      RolesSelected);
               break;
            case Rol_DEG_ADM:
               // Requests in all degrees administrated by me
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM admin,courses,crs_usr_requests"
			      " WHERE admin.UsrCod=%ld AND admin.Scope='%s'"
			      " AND admin.Cod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
                              Gbl.Usrs.Me.UsrDat.UsrCod,Sco_ScopeDB[Sco_SCOPE_DEG],
                              RolesSelected);
               break;
            case Rol_CTR_ADM:
               // Requests in all centres administrated by me
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM admin,degrees,courses,crs_usr_requests"
			      " WHERE admin.UsrCod=%ld AND admin.Scope='%s'"
			      " AND admin.Cod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
                              Gbl.Usrs.Me.UsrDat.UsrCod,Sco_ScopeDB[Sco_SCOPE_CTR],
                              RolesSelected);
               break;
            case Rol_INS_ADM:
               // Requests in all institutions administrated by me
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM admin,centres,degrees,courses,crs_usr_requests"
			      " WHERE admin.UsrCod=%ld AND admin.Scope='%s'"
			      " AND admin.Cod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
			      Gbl.Usrs.Me.UsrDat.UsrCod,Sco_ScopeDB[Sco_SCOPE_INS],
			      RolesSelected);
               break;
           case Rol_SYS_ADM:
               // All requests
               DB_BuildQuery ("SELECT ReqCod,"
			      "CrsCod,"
			      "UsrCod,"
			      "Role,"
			      "UNIX_TIMESTAMP(RequestTime)"
			      " FROM crs_usr_requests"
			      " WHERE ((1<<Role)&%u)<>0"
			      " ORDER BY RequestTime DESC",
                              RolesSelected);
               break;
            default:
               Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
               break;
           }
         break;
      case Sco_SCOPE_CTY:                // Show requesters for the current country
         switch (Gbl.Usrs.Me.Role.Logged)
           {
            case Rol_TCH:
               // Requests in courses of this country in which I am teacher
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM crs_usr,institutions,centres,degrees,courses,crs_usr_requests"
			      " WHERE crs_usr.UsrCod=%ld"
			      " AND crs_usr.Role=%u"
			      " AND crs_usr.CrsCod=courses.CrsCod"
			      " AND courses.DegCod=degrees.DegCod"
			      " AND degrees.CtrCod=centres.CtrCod"
			      " AND centres.InsCod=institutions.InsCod"
			      " AND institutions.CtyCod=%ld"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
			      Gbl.Usrs.Me.UsrDat.UsrCod,
			      (unsigned) Rol_TCH,
			      Gbl.CurrentCty.Cty.CtyCod,
			      RolesSelected);
               break;
            case Rol_DEG_ADM:
               // Requests in degrees of this country administrated by me
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM admin,institutions,centres,degrees,courses,crs_usr_requests"
			      " WHERE admin.UsrCod=%ld AND admin.Scope='%s'"
			      " AND admin.Cod=degrees.DegCod"
			      " AND degrees.CtrCod=centres.CtrCod"
			      " AND centres.InsCod=institutions.InsCod"
			      " AND institutions.CtyCod=%ld"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
			      Gbl.Usrs.Me.UsrDat.UsrCod,Sco_ScopeDB[Sco_SCOPE_DEG],
			      Gbl.CurrentCty.Cty.CtyCod,
			      RolesSelected);
               break;
            case Rol_CTR_ADM:
               // Requests in centres of this country administrated by me
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM admin,institutions,centres,degrees,courses,crs_usr_requests"
			      " WHERE admin.UsrCod=%ld AND admin.Scope='%s'"
			      " AND admin.Cod=centres.CtrCod"
			      " AND centres.InsCod=institutions.InsCod"
			      " AND institutions.CtyCod=%ld"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
                              Gbl.Usrs.Me.UsrDat.UsrCod,Sco_ScopeDB[Sco_SCOPE_CTR],
                              Gbl.CurrentCty.Cty.CtyCod,
                              RolesSelected);
               break;
            case Rol_INS_ADM:
               // Requests in institutions of this country administrated by me
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM admin,institutions,centres,degrees,courses,crs_usr_requests"
			      " WHERE admin.UsrCod=%ld AND admin.Scope='%s'"
			      " AND admin.Cod=institutions.InsCod"
			      " AND institutions.CtyCod=%ld"
			      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
			      Gbl.Usrs.Me.UsrDat.UsrCod,Sco_ScopeDB[Sco_SCOPE_INS],
			      Gbl.CurrentCty.Cty.CtyCod,
			      RolesSelected);
               break;
            case Rol_SYS_ADM:
               // Requests in any course of this country
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM institutions,centres,degrees,courses,crs_usr_requests"
			      " WHERE institutions.CtyCod=%ld"
			      " AND institutions.InsCod=centres.InsCod"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
                              Gbl.CurrentCty.Cty.CtyCod,
                              RolesSelected);
               break;
            default:
               Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
               break;
           }
         break;
      case Sco_SCOPE_INS:                // Show requesters for the current institution
         switch (Gbl.Usrs.Me.Role.Logged)
           {
            case Rol_TCH:
               // Requests in courses of this institution in which I am teacher
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM crs_usr,centres,degrees,courses,crs_usr_requests"
			      " WHERE crs_usr.UsrCod=%ld"
			      " AND crs_usr.Role=%u"
			      " AND crs_usr.CrsCod=courses.CrsCod"
			      " AND courses.DegCod=degrees.DegCod"
			      " AND degrees.CtrCod=centres.CtrCod"
			      " AND centres.InsCod=%ld"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
			      Gbl.Usrs.Me.UsrDat.UsrCod,
			      (unsigned) Rol_TCH,
			      Gbl.CurrentIns.Ins.InsCod,
			      RolesSelected);
               break;
            case Rol_DEG_ADM:
               // Requests in degrees of this institution administrated by me
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM admin,centres,degrees,courses,crs_usr_requests"
			      " WHERE admin.UsrCod=%ld AND admin.Scope='%s'"
			      " AND admin.Cod=degrees.DegCod"
			      " AND degrees.CtrCod=centres.CtrCod"
			      " AND centres.InsCod=%ld"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
			     Gbl.Usrs.Me.UsrDat.UsrCod,Sco_ScopeDB[Sco_SCOPE_DEG],
			     Gbl.CurrentIns.Ins.InsCod,
			     RolesSelected);
               break;
            case Rol_CTR_ADM:
               // Requests in centres of this institution administrated by me
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM admin,centres,degrees,courses,crs_usr_requests"
			      " WHERE admin.UsrCod=%ld AND admin.Scope='%s'"
			      " AND admin.Cod=centres.CtrCod"
			      " AND centres.InsCod=%ld"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
			     Gbl.Usrs.Me.UsrDat.UsrCod,Sco_ScopeDB[Sco_SCOPE_CTR],
			     Gbl.CurrentIns.Ins.InsCod,
			     RolesSelected);
               break;
            case Rol_INS_ADM:	// If I am logged as admin of this institution, I can view all the requesters from this institution
            case Rol_SYS_ADM:
               // Requests in any course of this institution
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM centres,degrees,courses,crs_usr_requests"
			      " WHERE centres.InsCod=%ld"
			      " AND centres.CtrCod=degrees.CtrCod"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
			      Gbl.CurrentIns.Ins.InsCod,
			      RolesSelected);
               break;
            default:
               Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
               break;
           }
         break;
      case Sco_SCOPE_CTR:                // Show requesters for the current centre
         switch (Gbl.Usrs.Me.Role.Logged)
           {
            case Rol_TCH:
               // Requests in courses of this centre in which I am teacher
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM crs_usr,degrees,courses,crs_usr_requests"
			      " WHERE crs_usr.UsrCod=%ld"
			      " AND crs_usr.Role=%u"
			      " AND crs_usr.CrsCod=courses.CrsCod"
			      " AND courses.DegCod=degrees.DegCod"
			      " AND degrees.CtrCod=%ld"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
			     Gbl.Usrs.Me.UsrDat.UsrCod,
			     (unsigned) Rol_TCH,
			     Gbl.CurrentCtr.Ctr.CtrCod,
			     RolesSelected);
               break;
            case Rol_DEG_ADM:
               // Requests in degrees of this centre administrated by me
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM admin,degrees,courses,crs_usr_requests"
			      " WHERE admin.UsrCod=%ld AND admin.Scope='%s'"
			      " AND admin.Cod=degrees.DegCod"
			      " AND degrees.CtrCod=%ld"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
			      Gbl.Usrs.Me.UsrDat.UsrCod,Sco_ScopeDB[Sco_SCOPE_DEG],
			      Gbl.CurrentCtr.Ctr.CtrCod,
			      RolesSelected);
               break;
            case Rol_CTR_ADM:	// If I am logged as admin of this centre     , I can view all the requesters from this centre
            case Rol_INS_ADM:	// If I am logged as admin of this institution, I can view all the requesters from this centre
            case Rol_SYS_ADM:
               // Request in any course of this centre
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM degrees,courses,crs_usr_requests"
			      " WHERE degrees.CtrCod=%ld"
			      " AND degrees.DegCod=courses.DegCod"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
                              Gbl.CurrentCtr.Ctr.CtrCod,
                              RolesSelected);
               break;
            default:
               Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
               break;
           }
         break;
      case Sco_SCOPE_DEG:        // Show requesters for the current degree
         switch (Gbl.Usrs.Me.Role.Logged)
           {
            case Rol_TCH:
               // Requests in courses of this degree in which I am teacher
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM crs_usr,courses,crs_usr_requests"
			      " WHERE crs_usr.UsrCod=%ld"
			      " AND crs_usr.Role=%u"
			      " AND crs_usr.CrsCod=courses.CrsCod"
			      " AND courses.DegCod=%ld"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
			     Gbl.Usrs.Me.UsrDat.UsrCod,
			     (unsigned) Rol_TCH,
			     Gbl.CurrentDeg.Deg.DegCod,
			     RolesSelected);
               break;
            case Rol_DEG_ADM:	// If I am logged as admin of this degree     , I can view all the requesters from this degree
            case Rol_CTR_ADM:	// If I am logged as admin of this centre     , I can view all the requesters from this degree
            case Rol_INS_ADM:	// If I am logged as admin of this institution, I can view all the requesters from this degree
            case Rol_SYS_ADM:
               // Requests in any course of this degree
               DB_BuildQuery ("SELECT crs_usr_requests.ReqCod,"
			      "crs_usr_requests.CrsCod,"
			      "crs_usr_requests.UsrCod,"
			      "crs_usr_requests.Role,"
			      "UNIX_TIMESTAMP(crs_usr_requests.RequestTime)"
			      " FROM courses,crs_usr_requests"
			      " WHERE courses.DegCod=%ld"
			      " AND courses.CrsCod=crs_usr_requests.CrsCod"
			      " AND ((1<<crs_usr_requests.Role)&%u)<>0"
			      " ORDER BY crs_usr_requests.RequestTime DESC",
			      Gbl.CurrentDeg.Deg.DegCod,
			      RolesSelected);
               break;
            default:
               Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
               break;
           }
         break;
      case Sco_SCOPE_CRS:        // Show requesters for the current course
         switch (Gbl.Usrs.Me.Role.Logged)
           {
            case Rol_TCH:	// If I am logged as teacher of this course   , I can view all the requesters from this course
            case Rol_DEG_ADM:	// If I am logged as admin of this degree     , I can view all the requesters from this course
            case Rol_CTR_ADM:	// If I am logged as admin of this centre     , I can view all the requesters from this course
            case Rol_INS_ADM:	// If I am logged as admin of this institution, I can view all the requesters from this course
            case Rol_SYS_ADM:
               // Requests in this course
               DB_BuildQuery ("SELECT ReqCod,CrsCod,UsrCod,Role,"
			      "UNIX_TIMESTAMP(RequestTime)"
			      " FROM crs_usr_requests"
			      " WHERE CrsCod=%ld"
			      " AND ((1<<Role)&%u)<>0"
			      " ORDER BY RequestTime DESC",
			      Gbl.CurrentCrs.Crs.CrsCod,
			      RolesSelected);
               break;
            default:
               Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
               break;
           }
         break;
      default:
         Lay_WrongScopeExit ();
         break;
     }

   NumRequests = (unsigned) DB_QuerySELECT_new (&mysql_res,"can not get requests for enrolment");

   /***** List requests *****/
   if (NumRequests)
      {
      /* Initialize structure with user's data */
      Usr_UsrDataConstructor (&UsrDat);

      /* Start table */
      Tbl_StartTableCenter (2);
      fprintf (Gbl.F.Out,"<th></th>"
                         "<th class=\"LEFT_TOP\">"
                         "%s"
                         "</th>"
                         "<th class=\"RIGHT_TOP\">"
                         "%s"
                         "</th>"
                         "<th colspan=\"2\" class=\"LEFT_TOP\">"
                         "%s"
                         "</th>"
                         "<th class=\"LEFT_TOP\">"
                         "%s"
                         "</th>"
                         "<th class=\"CENTER_TOP\">"
                         "%s"
                         "</th>"
                         "<th></th>"
                         "<th></th>"
                         "</tr>",
               Txt_Course,
               Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH],
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

	 /***** Get requested role (row[3]) *****/
	 DesiredRole = Rol_ConvertUnsignedStrToRole (row[3]);

         if (UsrExists)
            UsrBelongsToCrs = Usr_CheckIfUsrBelongsToCrs (UsrDat.UsrCod,
                                                          Crs.CrsCod,
                                                          false);
         else
            UsrBelongsToCrs = false;

         if (UsrExists &&
             !UsrBelongsToCrs &&
             (DesiredRole == Rol_STD ||
              DesiredRole == Rol_NET ||
              DesiredRole == Rol_TCH))
           {
            /***** Number *****/
            fprintf (Gbl.F.Out,"<tr>"
                               "<td class=\"DAT RIGHT_TOP\">"
                               "%u"
                               "</td>",
                     NumRequests - NumReq);

            /***** Link to course *****/
            Crs_GetDataOfCourseByCod (&Crs);
            Deg.DegCod = Crs.DegCod;
            Deg_GetDataOfDegreeByCod (&Deg);
            fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP\">");
            Act_StartFormGoTo (ActSeeCrsInf);
            Crs_PutParamCrsCod (Crs.CrsCod);
            snprintf (Gbl.Title,sizeof (Gbl.Title),
        	      Txt_Go_to_X,
		      Crs.FullName);
            Act_LinkFormSubmit (Gbl.Title,"DAT",NULL);
            fprintf (Gbl.F.Out,"%s &gt; %s"
        	               "</a>",
                     Deg.ShrtName,Crs.ShrtName);
            Act_EndForm ();
            fprintf (Gbl.F.Out,"</td>");

            /***** Number of teachers in the course *****/
            fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_TOP\">"
                               "%u"
                               "</td>",
                     Crs.NumUsrs[Rol_TCH]);

            /***** User photo *****/
            fprintf (Gbl.F.Out,"<td class=\"DAT CENTER_TOP\""
        	               " style=\"width:22px;\">");
            ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);
            Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                        	                  NULL,
                              "PHOTO21x28",Pho_ZOOM,false);
            fprintf (Gbl.F.Out,"</td>");

            /***** User name *****/
            fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP\">"
        	               "<div class=\"REQUESTER_NAME\">");	// Limited width
            Usr_WriteFirstNameBRSurnames (&UsrDat);
            fprintf (Gbl.F.Out,"</div>"
        	               "</td>");

            /***** Requested role (row[3]) *****/
            fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP\">"
        	               "%s"
        	               "</td>",
                     Txt_ROLES_SINGUL_abc[DesiredRole][UsrDat.Sex]);

            /***** Request time (row[4]) *****/
            Msg_WriteMsgDate (Dat_GetUNIXTimeFromStr (row[4]),"DAT");

            /***** Button to confirm the request *****/
            fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP\">");
            switch (DesiredRole)
              {
               case Rol_STD:
        	  NextAction = ActReqMdfStd;
        	  break;
               case Rol_NET:
        	  NextAction = ActReqMdfNET;
        	  break;
               case Rol_TCH:
        	  NextAction = ActReqMdfTch;
        	  break;
               default:
        	  NextAction = ActUnk;
        	  Lay_ShowErrorAndExit ("Wrong role.");
        	  break;
              }
            Act_StartForm (NextAction);
            Crs_PutParamCrsCod (Crs.CrsCod);
            Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
            Btn_PutCreateButtonInline (Txt_Register);
            Act_EndForm ();
            fprintf (Gbl.F.Out,"</td>");

            /***** Button to reject the request *****/
            fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP\">");
            Act_StartForm (ActReqRejSignUp);
            Crs_PutParamCrsCod (Crs.CrsCod);
            Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
            Btn_PutRemoveButtonInline (Txt_Reject);
            Act_EndForm ();
            fprintf (Gbl.F.Out,"</td>"
                               "</tr>");

            /***** Mark possible notification as seen *****/
            Ntf_MarkNotifAsSeen (Ntf_EVENT_ENROLMENT_REQUEST,
                                ReqCod,Gbl.CurrentCrs.Crs.CrsCod,
                                Gbl.Usrs.Me.UsrDat.UsrCod);
           }
         else        // User does not exists or user already belongs to course ==> remove pair from crs_usr_requests table
            Enr_RemoveEnrolmentRequest (Crs.CrsCod,UsrDat.UsrCod);
        }

      /* End table */
      Tbl_EndTable ();

      /* Free memory used for user's data */
      Usr_UsrDataDestructor (&UsrDat);
     }
   else	// There are no requests
      Ale_ShowAlert (Ale_INFO,Txt_No_enrolment_requests);

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/********************* Remove a request for enrolment ***********************/
/*****************************************************************************/

static void Enr_RemoveEnrolmentRequest (long CrsCod,long UsrCod)
  {
   char *Query;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long ReqCod;

   /***** Mark possible notifications as removed
          Important: do this before removing the request *****/
   /* Request request code (returns 0 or 1 rows) */
   DB_BuildQuery ("SELECT ReqCod FROM crs_usr_requests"
		  " WHERE CrsCod=%ld AND UsrCod=%ld",
                  CrsCod,UsrCod);
   if (DB_QuerySELECT_new (&mysql_res,"can not get request code"))        // Request exists
     {
      /* Get request code */
      row = mysql_fetch_row (mysql_res);
      ReqCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Mark possible notifications as removed */
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_ENROLMENT_REQUEST,ReqCod);
     }
   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Remove enrolment request *****/
   if (asprintf (&Query,"DELETE FROM crs_usr_requests"
                        " WHERE CrsCod=%ld AND UsrCod=%ld",
                 CrsCod,UsrCod) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryDELETE_free (Query,"can not remove a request for enrolment");
  }

/*****************************************************************************/
/******************* Remove expired requests for enrolment ******************/
/*****************************************************************************/

static void Enr_RemoveExpiredEnrolmentRequests (void)
  {
   char *Query;

   /***** Mark possible notifications as removed
          Important: do this before removing the request *****/
   if (asprintf (&Query,"UPDATE notif,crs_usr_requests"
			" SET notif.Status=(notif.Status | %u)"
			" WHERE notif.NotifyEvent=%u"
			" AND notif.Cod=crs_usr_requests.ReqCod"
			" AND crs_usr_requests.RequestTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-'%lu')",
	         (unsigned) Ntf_STATUS_BIT_REMOVED,
	         (unsigned) Ntf_EVENT_ENROLMENT_REQUEST,
	         Cfg_TIME_TO_DELETE_ENROLMENT_REQUESTS) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryUPDATE_free (Query,"can not set notification(s) as removed");

   /***** Remove expired requests for enrolment *****/
   if (asprintf (&Query,"DELETE FROM crs_usr_requests"
                        " WHERE RequestTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-'%lu')",
                 Cfg_TIME_TO_DELETE_ENROLMENT_REQUESTS) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryDELETE_free (Query,"can not remove expired requests for enrolment");
  }

/*****************************************************************************/
/********************** Write a form to admin one user ***********************/
/*****************************************************************************/

void Enr_PutLinkToAdminOneUsr (Act_Action_t NextAction)
  {
   extern const char *Txt_Administer_me;
   extern const char *Txt_Administer_one_user;
   const char *TitleText = Enr_ICanAdminOtherUsrs[Gbl.Usrs.Me.Role.Logged] ? Txt_Administer_one_user :
                        	                                             Txt_Administer_me;

   Lay_PutContextualLink (NextAction,NULL,NULL,
                          "config64x64.gif",
                          TitleText,TitleText,
                          NULL);
  }

/*****************************************************************************/
/******************* Write a form to admin several users *********************/
/*****************************************************************************/

void Enr_PutLinkToAdminSeveralUsrs (Rol_Role_t Role)
  {
   extern const char *Txt_Administer_multiple_students;
   extern const char *Txt_Administer_multiple_non_editing_teachers;
   extern const char *Txt_Administer_multiple_teachers;
   Act_Action_t NextAction;
   const char *TitleText;

   switch (Role)
     {
      case Rol_STD:
	 NextAction = ActReqEnrSevStd;
	 TitleText = Txt_Administer_multiple_students;
	 break;
      case Rol_NET:
	 NextAction = ActReqEnrSevNET;
	 TitleText = Txt_Administer_multiple_non_editing_teachers;
	 break;
      case Rol_TCH:
	 NextAction = ActReqEnrSevTch;
	 TitleText = Txt_Administer_multiple_teachers;
	 break;
      default:
	 NextAction = ActUnk;
	 TitleText = NULL;
	 Lay_ShowErrorAndExit ("Wrong role.");
     }
   Lay_PutContextualLink (NextAction,NULL,NULL,
	                  "config64x64.gif",
	                  TitleText,TitleText,
                          NULL);
  }

/*****************************************************************************/
/************** Form to request the user's ID of another user ****************/
/*****************************************************************************/

void Enr_ReqRegRemOth (void)
  {
   /***** Form to request user to be administered *****/
   Enr_ReqRegRemUsr (Rol_GST);
  }

void Enr_ReqRegRemStd (void)
  {
   /***** Put contextual links *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

   /* Put link to go to admin several students */
   Enr_PutLinkToAdminSeveralUsrs (Rol_STD);

   fprintf (Gbl.F.Out,"</div>");

   /***** Form to request user to be administered *****/
   Enr_ReqRegRemUsr (Rol_STD);
  }

void Enr_ReqRegRemTch (void)
  {
   /***** Form to request user to be administered *****/
   Enr_ReqRegRemUsr (Rol_TCH);
  }

static void Enr_ReqRegRemUsr (Rol_Role_t Role)
  {
   if (Enr_ICanAdminOtherUsrs[Gbl.Usrs.Me.Role.Logged])
      /***** Form to request the user's ID of another user *****/
      Enr_ReqAnotherUsrIDToRegisterRemove (Role);
   else
      /***** Form to request if register/remove me *****/
      Enr_AskIfRegRemMe (Role);
  }

/*****************************************************************************/
/****** Write a form to request another user's ID, @nickname or email ********/
/*****************************************************************************/

static void Enr_ReqAnotherUsrIDToRegisterRemove (Rol_Role_t Role)
  {
   extern const char *Hlp_USERS_Administration_administer_one_user;
   extern const char *Txt_Administer_one_user;
   Act_Action_t NextAction;

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Administer_one_user,NULL,
                 Hlp_USERS_Administration_administer_one_user,Box_NOT_CLOSABLE);

   /***** Write form to request another user's ID *****/
   switch (Role)
     {
      case Rol_GST:
	 NextAction = ActReqMdfOth;
         break;
      case Rol_STD:
	 NextAction = ActReqMdfStd;
	 break;
      case Rol_NET:
	 NextAction = ActReqMdfNET;
	 break;
      case Rol_TCH:
	 NextAction = ActReqMdfTch;
	 break;
      default:
	 NextAction = ActUnk;
         Lay_ShowErrorAndExit ("Wrong role.");
	 break;
     }
   Enr_WriteFormToReqAnotherUsrID (NextAction,NULL);

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/********************** Ask me for register/remove me ************************/
/*****************************************************************************/

static void Enr_AskIfRegRemMe (Rol_Role_t Role)
  {
   struct ListUsrCods ListUsrCods;

   /***** I only can admin me *****/
   Gbl.Usrs.Other.UsrDat.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   ListUsrCods.NumUsrs = 1;
   Usr_AllocateListUsrCods (&ListUsrCods);
   ListUsrCods.Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;

   Enr_AskIfRegRemUsr (&ListUsrCods,Role);
  }

/*****************************************************************************/
/****************** Ask me for register/remove another user ******************/
/*****************************************************************************/

void Enr_AskIfRegRemAnotherOth (void)
  {
   Enr_AskIfRegRemAnotherUsr (Rol_GST);
  }

void Enr_AskIfRegRemAnotherStd (void)
  {
   Enr_AskIfRegRemAnotherUsr (Rol_STD);
  }

void Enr_AskIfRegRemAnotherTch (void)
  {
   Enr_AskIfRegRemAnotherUsr (Rol_TCH);
  }

static void Enr_AskIfRegRemAnotherUsr (Rol_Role_t Role)
  {
   struct ListUsrCods ListUsrCods;

   /***** Check if UsrCod is present in parameters *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      /***** If UsrCod is present in parameters,
	     use this parameter to identify the user to be enroled / removed *****/
      ListUsrCods.NumUsrs = 1;
      Usr_AllocateListUsrCods (&ListUsrCods);
      ListUsrCods.Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;
     }
   else        // Parameter with user code not present
      /***** If UsrCod is not present in parameters from form,
	     use user's ID to identify the user to be enroled /removed *****/
      Usr_GetParamOtherUsrIDNickOrEMailAndGetUsrCods (&ListUsrCods);

   Enr_AskIfRegRemUsr (&ListUsrCods,Role);
  }

/*****************************************************************************/
/********************** Ask me for register/remove user **********************/
/*****************************************************************************/

static void Enr_AskIfRegRemUsr (struct ListUsrCods *ListUsrCods,Rol_Role_t Role)
  {
   extern const char *Txt_There_are_X_users_with_the_ID_Y;
   extern const char *Txt_THE_USER_X_is_already_enroled_in_the_course_Y;
   extern const char *Txt_THE_USER_X_is_already_in_the_course_Y_but_has_not_yet_accepted_the_enrolment;
   extern const char *Txt_THE_USER_X_already_exists_in_Y_but_is_not_yet_enroled_in_the_course_Z;
   extern const char *Txt_THE_USER_X_already_exists_in_Y;
   extern const char *Txt_The_user_is_new_not_yet_in_X;
   extern const char *Txt_If_this_is_a_new_user_in_X_you_should_indicate_her_his_ID;
   unsigned NumUsr;
   bool NewUsrIDValid;

   if (ListUsrCods->NumUsrs)	// User(s) found with the ID
     {
      /***** Warning if more than one user found *****/
      if (ListUsrCods->NumUsrs > 1)
	{
	 snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	           Txt_There_are_X_users_with_the_ID_Y,
		   ListUsrCods->NumUsrs,Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail);
	 Ale_ShowAlert (Ale_INFO,Gbl.Alert.Txt);
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
	    if (Usr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
	      {
	       Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	       if (Gbl.Usrs.Other.UsrDat.Accepted)
		  snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	                    Txt_THE_USER_X_is_already_enroled_in_the_course_Y,
			    Gbl.Usrs.Other.UsrDat.FullName,Gbl.CurrentCrs.Crs.FullName);
	       else        // Enrolment not yet accepted
		  snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	                    Txt_THE_USER_X_is_already_in_the_course_Y_but_has_not_yet_accepted_the_enrolment,
			    Gbl.Usrs.Other.UsrDat.FullName,Gbl.CurrentCrs.Crs.FullName);
	       Ale_ShowAlert (Ale_INFO,Gbl.Alert.Txt);

	       Enr_ShowFormToEditOtherUsr ();
	      }
	    else        // User does not belong to the current course
	      {
	       snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	                 Txt_THE_USER_X_already_exists_in_Y_but_is_not_yet_enroled_in_the_course_Z,
			 Gbl.Usrs.Other.UsrDat.FullName,
			 Cfg_PLATFORM_SHORT_NAME,Gbl.CurrentCrs.Crs.FullName);
	       Ale_ShowAlert (Ale_INFO,Gbl.Alert.Txt);

	       Enr_ShowFormToEditOtherUsr ();
	      }
	   }
	 else	// No course selected
	   {
	    snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	              Txt_THE_USER_X_already_exists_in_Y,
		      Gbl.Usrs.Other.UsrDat.FullName,Cfg_PLATFORM_SHORT_NAME);
	    Ale_ShowAlert (Ale_INFO,Gbl.Alert.Txt);

	    Enr_ShowFormToEditOtherUsr ();
	   }
	}

      /***** Free list of users' codes *****/
      Usr_FreeListUsrCods (ListUsrCods);
     }
   else	// No users found, he/she is a new user
     {
      /***** If UsrCod is not present in parameters from form,
	     use user's ID to identify the user to be enroled *****/
      if (Gbl.Usrs.Other.UsrDat.IDs.List)
         NewUsrIDValid = ID_CheckIfUsrIDIsValid (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);	// Check the first element of the list
      else
	 NewUsrIDValid = false;

      if (NewUsrIDValid)
	{
	 /***** Show form to enter the data of a new user *****/
	 snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	           Txt_The_user_is_new_not_yet_in_X,
		   Cfg_PLATFORM_SHORT_NAME);
	 Ale_ShowAlert (Ale_INFO,Gbl.Alert.Txt);
	 Rec_ShowFormOtherNewSharedRecord (&Gbl.Usrs.Other.UsrDat,Role);
	}
      else        // User's ID is not valid
	{
	 /* Write message and request a new user's ID */
	 snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	           Txt_If_this_is_a_new_user_in_X_you_should_indicate_her_his_ID,
		   Cfg_PLATFORM_SHORT_NAME);
	 Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
	 Enr_ReqRegRemUsr (Role);
	}
     }
  }

/*****************************************************************************/
/************ Show form to edit the record of an existing user ***************/
/*****************************************************************************/

static void Enr_ShowFormToEditOtherUsr (void)
  {
   /***** User's record *****/
   Rec_ShowSharedUsrRecord (Rec_SHA_OTHER_EXISTING_USR_FORM,
                            &Gbl.Usrs.Other.UsrDat,NULL);
  }

/*****************************************************************************/
/*************** Add an administrator to current institution *****************/
/*****************************************************************************/

void Enr_AddAdmToIns (void)
  {
   Enr_AddAdm (Sco_SCOPE_INS,Gbl.CurrentIns.Ins.InsCod,Gbl.CurrentIns.Ins.FullName);
  }

/*****************************************************************************/
/******************* Add an administrator to current centre ******************/
/*****************************************************************************/

void Enr_AddAdmToCtr (void)
  {
   Enr_AddAdm (Sco_SCOPE_CTR,Gbl.CurrentCtr.Ctr.CtrCod,Gbl.CurrentCtr.Ctr.FullName);
  }

/*****************************************************************************/
/******************* Add an administrator to current degree ******************/
/*****************************************************************************/

void Enr_AddAdmToDeg (void)
  {
   Enr_AddAdm (Sco_SCOPE_DEG,Gbl.CurrentDeg.Deg.DegCod,Gbl.CurrentDeg.Deg.FullName);
  }

/*****************************************************************************/
/******************* Add an administrator to current degree ******************/
/*****************************************************************************/

static void Enr_AddAdm (Sco_Scope_t Scope,long Cod,const char *InsCtrDegName)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   bool ICanRegister;

   if (Cod > 0)
     {
      /***** Get plain user's ID of the user to add/modify *****/
      if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
        {
         /* Check if I am allowed to register user as administrator in institution/centre/degree */
	 ICanRegister = ((Scope == Sco_SCOPE_DEG && Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM) ||
                         (Scope == Sco_SCOPE_CTR && Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM) ||
                         (Scope == Sco_SCOPE_INS && Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM));
         if (ICanRegister)
           {
            /***** Register administrator in current institution/centre/degree in database *****/
            Enr_RegisterAdmin (&Gbl.Usrs.Other.UsrDat,Scope,
                               Cod,InsCtrDegName);

            /***** Show user's record *****/
            Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
           }
         else
            Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
        }
      else
         Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
  }

/*****************************************************************************/
/**************** Register administrator in current institution **************/
/*****************************************************************************/

static void Enr_RegisterAdmin (struct UsrData *UsrDat,Sco_Scope_t Scope,long Cod,const char *InsCtrDegName)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   extern const char *Txt_THE_USER_X_is_already_an_administrator_of_Y;
   extern const char *Txt_THE_USER_X_has_been_enroled_as_administrator_of_Y;
   char *Query;

   /***** Check if user was and administrator of current institution/centre/degree *****/
   if (Usr_CheckIfUsrIsAdm (UsrDat->UsrCod,Scope,Cod))
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        Txt_THE_USER_X_is_already_an_administrator_of_Y,
                UsrDat->FullName,InsCtrDegName);
   else        // User was not administrator of current institution/centre/degree
     {
      /***** Insert or replace administrator in current institution/centre/degree *****/
      if (asprintf (&Query,"REPLACE INTO admin"
	                   " (UsrCod,Scope,Cod)"
                           " VALUES"
                           " (%ld,'%s',%ld)",
                    UsrDat->UsrCod,Sco_ScopeDB[Scope],Cod) < 0)
         Lay_NotEnoughMemoryExit ();
      DB_QueryREPLACE_free (Query,"can not create administrator");

      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        Txt_THE_USER_X_has_been_enroled_as_administrator_of_Y,
                UsrDat->FullName,InsCtrDegName);
     }
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
  }

/*****************************************************************************/
/****************** Ask for remove me from current course ********************/
/*****************************************************************************/

void Enr_ReqRemMeFromCrs (void)
  {
   Enr_AskIfRemoveUsrFromCrs (&Gbl.Usrs.Me.UsrDat);
  }

/*****************************************************************************/
/************** Ask for remove of a user from current course *****************/
/*****************************************************************************/

void Enr_ReqRemUsrFromCrs (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Enr_CheckIfICanRemUsrFromCrs ())
	 Enr_AskIfRemoveUsrFromCrs (&Gbl.Usrs.Other.UsrDat);
      else
         Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/********************* Remove a user from current course *********************/
/*****************************************************************************/

void Enr_RemUsrFromCrs1 (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   if (Pwd_GetConfirmationOnDangerousAction ())
     {
      /***** Get user to be removed *****/
      if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
	{
	 if (Enr_CheckIfICanRemUsrFromCrs ())
	    Enr_EffectivelyRemUsrFromCrs (&Gbl.Usrs.Other.UsrDat,&Gbl.CurrentCrs.Crs,
					  Enr_REMOVE_WORKS,Cns_VERBOSE);
	 else
	   {
	    Gbl.Alert.Type = Ale_WARNING;
	    Str_Copy (Gbl.Alert.Txt,Txt_User_not_found_or_you_do_not_have_permission_,
		      Ale_MAX_BYTES_ALERT);
	   }
	}
      else
	{
	 Gbl.Alert.Type = Ale_WARNING;
	 Str_Copy (Gbl.Alert.Txt,Txt_User_not_found_or_you_do_not_have_permission_,
		   Ale_MAX_BYTES_ALERT);
	}
     }
  }

void Enr_RemUsrFromCrs2 (void)
  {
   Ale_ShowAlert (Gbl.Alert.Type,Gbl.Alert.Txt);
  }

/*****************************************************************************/
/*********** Check if I can remove another user in current course ************/
/*****************************************************************************/

static bool Enr_CheckIfICanRemUsrFromCrs (void)
  {
   bool ItsMe;

   /* Check if I can remove another user from current course */
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
	 ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);
	 return ItsMe;	// A student or non-editing teacher can remove herself/himself
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 return true;	// A teacher or administrator can remove anyone
      default:
	 return false;
     }
   return false;
  }

/*****************************************************************************/
/******* Ask for remove of an administrator from current institution *********/
/*****************************************************************************/

static void Enr_ReqRemAdmOfIns (void)
  {
   Enr_ReqRemOrRemAdm (Enr_REQUEST_REMOVE_USR,Sco_SCOPE_INS,
                       Gbl.CurrentIns.Ins.InsCod,Gbl.CurrentIns.Ins.FullName);
  }

/*****************************************************************************/
/********* Ask for remove of an administrator from current centre ************/
/*****************************************************************************/

static void Enr_ReqRemAdmOfCtr (void)
  {
   Enr_ReqRemOrRemAdm (Enr_REQUEST_REMOVE_USR,Sco_SCOPE_CTR,
                       Gbl.CurrentCtr.Ctr.CtrCod,Gbl.CurrentCtr.Ctr.FullName);
  }

/*****************************************************************************/
/********* Ask for remove of an administrator from current degree ************/
/*****************************************************************************/

static void Enr_ReqRemAdmOfDeg (void)
  {
   Enr_ReqRemOrRemAdm (Enr_REQUEST_REMOVE_USR,Sco_SCOPE_DEG,
                       Gbl.CurrentDeg.Deg.DegCod,Gbl.CurrentDeg.Deg.FullName);
  }

/*****************************************************************************/
/************ Remove an administrator from current institution ***************/
/*****************************************************************************/

void Enr_RemAdmIns (void)
  {
   Enr_ReqRemOrRemAdm (Enr_REMOVE_USR,Sco_SCOPE_INS,
                       Gbl.CurrentIns.Ins.InsCod,Gbl.CurrentIns.Ins.FullName);
  }

/*****************************************************************************/
/*************** Remove an administrator from current centre *****************/
/*****************************************************************************/

void Enr_RemAdmCtr (void)
  {
   Enr_ReqRemOrRemAdm (Enr_REMOVE_USR,Sco_SCOPE_CTR,
                       Gbl.CurrentCtr.Ctr.CtrCod,Gbl.CurrentCtr.Ctr.FullName);
  }

/*****************************************************************************/
/*************** Remove an administrator from current degree *****************/
/*****************************************************************************/

void Enr_RemAdmDeg (void)
  {
   Enr_ReqRemOrRemAdm (Enr_REMOVE_USR,Sco_SCOPE_DEG,
                       Gbl.CurrentDeg.Deg.DegCod,Gbl.CurrentDeg.Deg.FullName);
  }

/*****************************************************************************/
/***************** Remove an admin from current institution ******************/
/*****************************************************************************/

static void Enr_ReqRemOrRemAdm (Enr_ReqDelOrDelUsr_t ReqDelOrDelUsr,Sco_Scope_t Scope,
                                long Cod,const char *InsCtrDegName)
  {
   extern const char *Txt_THE_USER_X_is_not_an_administrator_of_Y;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   bool ItsMe;
   bool ICanRemove;

   if (Cod > 0)
     {
      /***** Get user to be removed *****/
      if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
        {
         /* Check if it's forbidden to remove an administrator */
         ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);
         ICanRemove = (ItsMe ||
                       (Scope == Sco_SCOPE_DEG && Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM) ||
                       (Scope == Sco_SCOPE_CTR && Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM) ||
                       (Scope == Sco_SCOPE_INS && Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM));
         if (ICanRemove)
           {
            /* Check if the other user is an admin of the current institution/centre/degree */
            if (Usr_CheckIfUsrIsAdm (Gbl.Usrs.Other.UsrDat.UsrCod,Scope,Cod))
              {                // The other user is an administrator of current institution/centre/degree ==> ask for removing or remove her/him
               switch (ReqDelOrDelUsr)
                 {
                  case Enr_REQUEST_REMOVE_USR:     // Ask if remove administrator from current institution
                     Enr_AskIfRemAdm (ItsMe,Scope,InsCtrDegName);
                     break;
                  case Enr_REMOVE_USR:             // Remove administrator from current institution
                     Enr_EffectivelyRemAdm (&Gbl.Usrs.Other.UsrDat,Scope,
                                            Cod,InsCtrDegName);
                     break;
                 }
              }
            else        // The other user is not an administrator of current institution
              {
               snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	                 Txt_THE_USER_X_is_not_an_administrator_of_Y,
                         Gbl.Usrs.Other.UsrDat.FullName,InsCtrDegName);
               Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
              }
           }
         else
            Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
        }
      else
         Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
  }

/*****************************************************************************/
/**** Ask if really wanted to add an administrator to current institution ****/
/*****************************************************************************/

static void Enr_ReqAddAdm (Sco_Scope_t Scope,long Cod,const char *InsCtrDegName)
  {
   extern const char *Txt_THE_USER_X_is_already_an_administrator_of_Y;
   extern const char *Txt_Do_you_really_want_to_register_the_following_user_as_an_administrator_of_X;
   extern const char *Txt_Register_user_IN_A_COURSE_OR_DEGREE;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   static const Act_Action_t Enr_ActNewAdm[Sco_NUM_SCOPES] =
     {
      ActUnk,		// Sco_SCOPE_UNK
      ActUnk,		// Sco_SCOPE_SYS,
      ActUnk,		// Sco_SCOPE_CTY,
      ActNewAdmIns,	// Sco_SCOPE_INS,
      ActNewAdmCtr,	// Sco_SCOPE_CTR,
      ActNewAdmDeg,	// Sco_SCOPE_DEG,
      ActUnk,		// Sco_SCOPE_CRS,
     };
   bool ICanRegister;

   if (Cod > 0)
     {
      /***** Get user's identificator of the user to register as admin *****/
      if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
        {
         /* Check if I am allowed to register user as administrator in institution/centre/degree */
	 ICanRegister = ((Scope == Sco_SCOPE_DEG && Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM) ||
                         (Scope == Sco_SCOPE_CTR && Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM) ||
                         (Scope == Sco_SCOPE_INS && Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM));
         if (ICanRegister)
           {
            if (Usr_CheckIfUsrIsAdm (Gbl.Usrs.Other.UsrDat.UsrCod,Scope,Cod))        // User is already an administrator of current institution/centre/degree
              {
               snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	                 Txt_THE_USER_X_is_already_an_administrator_of_Y,
                         Gbl.Usrs.Other.UsrDat.FullName,InsCtrDegName);
               Ale_ShowAlert (Ale_INFO,Gbl.Alert.Txt);
               Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
              }
            else
              {
	       /***** Show question and button to register user as administrator *****/
	       /* Start alert */
               snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	                 Txt_Do_you_really_want_to_register_the_following_user_as_an_administrator_of_X,
                         InsCtrDegName);
	       Ale_ShowAlertAndButton1 (Ale_QUESTION,Gbl.Alert.Txt);

	       /* Show user's record */
               Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

	       /* End alert */
	       Ale_ShowAlertAndButton2 (Enr_ActNewAdm[Scope],NULL,NULL,
	                                Usr_PutParamOtherUsrCodEncrypted,
	                                Btn_CREATE_BUTTON,Txt_Register_user_IN_A_COURSE_OR_DEGREE);
              }
           }
         else
            Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
        }
      else
         Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
  }

/*****************************************************************************/
/*************** Accept my enrolment in the current course ******************/
/*****************************************************************************/

void Enr_AcceptRegisterMeInCrs (void)
  {
   extern const char *Txt_You_have_confirmed_your_enrolment_in_the_course_X;

   /***** Confirm my enrolment *****/
   Enr_AcceptUsrInCrs (Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Mark all notifications about enrolment (as student or as teacher)
          in current course as removed *****/
   Ntf_MarkNotifToOneUsrAsRemoved (Ntf_EVENT_ENROLMENT_STD,-1L,
                                  Gbl.Usrs.Me.UsrDat.UsrCod);
   Ntf_MarkNotifToOneUsrAsRemoved (Ntf_EVENT_ENROLMENT_TCH,-1L,
                                  Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Confirmation message *****/
   snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	     Txt_You_have_confirmed_your_enrolment_in_the_course_X,
             Gbl.CurrentCrs.Crs.FullName);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
  }

/*****************************************************************************/
/******************* Create and show data from other user ********************/
/*****************************************************************************/

void Enr_CreateNewUsr1 (void)
  {
   extern const char *Txt_The_role_of_THE_USER_X_in_the_course_Y_has_changed_from_A_to_B;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_THE_USER_X_has_been_enroled_in_the_course_Y;
   extern const char *Txt_The_ID_X_is_not_valid;
   Rol_Role_t OldRole;
   Rol_Role_t NewRole;

   /***** Get user's ID from form *****/
   ID_GetParamOtherUsrIDPlain ();	// User's ID was already modified and passed as a hidden parameter

   /***** Initialize alert type and message *****/
   Gbl.Alert.Type = Ale_NONE;	// Do not show alert

   if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID))        // User's ID valid
     {
      Gbl.Usrs.Other.UsrDat.UsrCod = -1L;

      /***** Get new role *****/
      NewRole = Rec_GetRoleFromRecordForm ();

      /***** Get user's name from form *****/
      Rec_GetUsrNameFromRecordForm (&Gbl.Usrs.Other.UsrDat);

      /***** Create user *****/
      Gbl.Usrs.Other.UsrDat.IDs.List[0].Confirmed = true;	// User's ID will be stored as confirmed
      Acc_CreateNewUsr (&Gbl.Usrs.Other.UsrDat,
                        false);	// I am NOT creating my own account

      /***** Register user in current course in database *****/
      if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
	{
	 if (Usr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
	   {
	    OldRole = Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role;	// Remember old role before changing it
	    if (NewRole != OldRole)	// The role must be updated
	      {
	       /* Modify role */
	       Enr_ModifyRoleInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole);

	       /* Success message */
               Gbl.Alert.Type = Ale_SUCCESS;
	       snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	                 Txt_The_role_of_THE_USER_X_in_the_course_Y_has_changed_from_A_to_B,
			 Gbl.Usrs.Other.UsrDat.FullName,
			 Gbl.CurrentCrs.Crs.FullName,
			 Txt_ROLES_SINGUL_abc[OldRole][Gbl.Usrs.Other.UsrDat.Sex],
			 Txt_ROLES_SINGUL_abc[NewRole][Gbl.Usrs.Other.UsrDat.Sex]);
	      }
	   }
	 else      // User does not belong to current course
	   {
	    /* Register user */
	    Enr_RegisterUsrInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole,
	                                 Enr_SET_ACCEPTED_TO_FALSE);

	    /* Success message */
            Gbl.Alert.Type = Ale_SUCCESS;
	    snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	              Txt_THE_USER_X_has_been_enroled_in_the_course_Y,
		      Gbl.Usrs.Other.UsrDat.FullName,
		      Gbl.CurrentCrs.Crs.FullName);
	   }

	 /***** Change user's groups *****/
	 if (Gbl.CurrentCrs.Grps.NumGrps)	// This course has groups?
	    Grp_ChangeOtherUsrGrps ();
	}

      /***** Change current action *****/
      switch (NewRole)
        {
	 case Rol_GST:
	    Gbl.Action.Act = ActCreOth;
	    break;
	 case Rol_STD:
	    Gbl.Action.Act = ActCreStd;
	    break;
	 case Rol_NET:
	    Gbl.Action.Act = ActCreNET;
	    break;
	 case Rol_TCH:
	    Gbl.Action.Act = ActCreTch;
	    break;
	 default:
	    Lay_ShowErrorAndExit ("Wrong role.");
	    break;
        }
      Tab_SetCurrentTab ();
     }
   else        // User's ID not valid
     {
      /***** Error message *****/
      Gbl.Alert.Type = Ale_ERROR;
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        Txt_The_ID_X_is_not_valid,
                Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);
     }
  }

void Enr_CreateNewUsr2 (void)
  {
   if (Gbl.Alert.Type == Ale_ERROR)	// User's ID not valid
      Ale_ShowPendingAlert ();
   else					// User's ID valid
     {
      if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
	 /***** Show optional alert *****/
         Ale_ShowPendingAlert ();

      /***** Show user's record *****/
      Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
     }
  }

/*****************************************************************************/
/**** Modify other user's data and register her/him in course and groups *****/
/*****************************************************************************/

void Enr_ModifyUsr1 (void)
  {
   extern const char *Txt_The_role_of_THE_USER_X_in_the_course_Y_has_changed_from_A_to_B;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_THE_USER_X_has_been_enroled_in_the_course_Y;
   bool ItsMe;
   Rol_Role_t OldRole;
   Rol_Role_t NewRole;

   /***** Initialize alert type and message *****/
   Gbl.Alert.Type = Ale_NONE;	// Do not show alert

   /***** Get user from form *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);

      /***** Get the action to do *****/
      Gbl.Usrs.RegRemAction = (Enr_RegRemOneUsrAction_t)
	                      Par_GetParToUnsignedLong ("RegRemAction",
                                                        0,
                                                        Enr_REG_REM_ONE_USR_NUM_ACTIONS - 1,
                                                        (unsigned long) Enr_REG_REM_ONE_USR_UNKNOWN_ACTION);
      switch (Gbl.Usrs.RegRemAction)
	{
	 case Enr_REGISTER_MODIFY_ONE_USR_IN_CRS:
	    if (ItsMe || Gbl.Usrs.Me.Role.Logged >= Rol_TCH)
	      {
	       /***** Get user's name from record form *****/
	       if (Usr_ICanChangeOtherUsrData (&Gbl.Usrs.Other.UsrDat))
		  Rec_GetUsrNameFromRecordForm (&Gbl.Usrs.Other.UsrDat);

	       /***** Update user's data in database *****/
	       Enr_UpdateUsrData (&Gbl.Usrs.Other.UsrDat);

	       if (Gbl.CurrentCrs.Crs.CrsCod > 0)
		 {
		  /***** Get new role from record form *****/
		  NewRole = Rec_GetRoleFromRecordForm ();

		  /***** Register user in current course in database *****/
		  if (Usr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
		    {
		     OldRole = Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role;	// Remember old role before changing it
		     if (NewRole != OldRole)	// The role must be updated
		       {
			/* Modify role */
			Enr_ModifyRoleInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole);

			/* Set success message */
			Gbl.Alert.Type = Ale_SUCCESS;
			snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	                          Txt_The_role_of_THE_USER_X_in_the_course_Y_has_changed_from_A_to_B,
				  Gbl.Usrs.Other.UsrDat.FullName,
				  Gbl.CurrentCrs.Crs.FullName,
				  Txt_ROLES_SINGUL_abc[OldRole][Gbl.Usrs.Other.UsrDat.Sex],
				  Txt_ROLES_SINGUL_abc[NewRole][Gbl.Usrs.Other.UsrDat.Sex]);
		       }
		    }
		  else	      // User does not belong to current course
		    {
		     /* Register user */
		     Enr_RegisterUsrInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole,
						  Enr_SET_ACCEPTED_TO_FALSE);

		     /* Set success message */
	             Gbl.Alert.Type = Ale_SUCCESS;
		     snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	                       Txt_THE_USER_X_has_been_enroled_in_the_course_Y,
			       Gbl.Usrs.Other.UsrDat.FullName,Gbl.CurrentCrs.Crs.FullName);
		    }

		  /***** Change user's groups *****/
		  if (Gbl.CurrentCrs.Grps.NumGrps)	// This course has groups?
		    {
		     if (ItsMe)
			Grp_ChangeMyGrps (Cns_QUIET);
		     else
			Grp_ChangeOtherUsrGrps ();
		    }

		  /***** If it's me, change my roles *****/
		  if (ItsMe)
		    {
		     Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Role = Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role;
                     Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Valid = true;
                     Gbl.Usrs.Me.UsrDat.Roles.InCrss = Gbl.Usrs.Other.UsrDat.Roles.InCrss;
                     Rol_SetMyRoles ();
		    }

		  /***** Change current action *****/
		  switch (NewRole)
		    {
		     case Rol_GST:
			Gbl.Action.Act = ActUpdOth;
			break;
		     case Rol_STD:
			Gbl.Action.Act = ActUpdStd;
			break;
		     case Rol_NET:
			Gbl.Action.Act = ActUpdNET;
			break;
		     case Rol_TCH:
			Gbl.Action.Act = ActUpdTch;
			break;
		     default:
			Lay_ShowErrorAndExit ("Wrong role.");
			break;
		    }
		  Tab_SetCurrentTab ();
		 }
	      }
	    else
	       Gbl.Alert.Type = Ale_WARNING;
	    break;
	 case Enr_REGISTER_ONE_DEGREE_ADMIN:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_CTR_ADM)
	       Gbl.Alert.Type = Ale_WARNING;
	    break;
	 case Enr_REGISTER_ONE_CENTRE_ADMIN:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_INS_ADM)
	       Gbl.Alert.Type = Ale_WARNING;
	    break;
	 case Enr_REGISTER_ONE_INSTITUTION_ADMIN:
	    if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
	       Gbl.Alert.Type = Ale_WARNING;
	    break;
	 case Enr_REPORT_USR_AS_POSSIBLE_DUPLICATE:
	    if (ItsMe || Gbl.Usrs.Me.Role.Logged < Rol_TCH)
	       Gbl.Alert.Type = Ale_WARNING;
	    break;
	 case Enr_REMOVE_ONE_USR_FROM_CRS:
	    if (!ItsMe && Gbl.Usrs.Me.Role.Logged < Rol_TCH)
	       Gbl.Alert.Type = Ale_WARNING;
	    break;
	 case Enr_REMOVE_ONE_DEGREE_ADMIN:
	    if (!ItsMe && Gbl.Usrs.Me.Role.Logged < Rol_CTR_ADM)
	       Gbl.Alert.Type = Ale_WARNING;
	    break;
	 case Enr_REMOVE_ONE_CENTRE_ADMIN:
	    if (!ItsMe && Gbl.Usrs.Me.Role.Logged < Rol_INS_ADM)
	       Gbl.Alert.Type = Ale_WARNING;
	    break;
	 case Enr_REMOVE_ONE_INSTITUTION_ADMIN:
	    if (!ItsMe && Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
	       Gbl.Alert.Type = Ale_WARNING;
	    break;
	 case Enr_ELIMINATE_ONE_USR_FROM_PLATFORM:
	    if (!Acc_CheckIfICanEliminateAccount (Gbl.Usrs.Other.UsrDat.UsrCod))
	       Gbl.Alert.Type = Ale_WARNING;
	    break;
	 default:
	    Gbl.Alert.Type = Ale_WARNING;
	    break;
	}
     }
   else
      Gbl.Alert.Type = Ale_WARNING;
  }

void Enr_ModifyUsr2 (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   if (Gbl.Alert.Type == Ale_WARNING)
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
   else // No error
      switch (Gbl.Usrs.RegRemAction)
	{
	 case Enr_REGISTER_MODIFY_ONE_USR_IN_CRS:
	    if (Gbl.CurrentCrs.Crs.CrsCod > 0)
               /***** Show optional alert *****/
               Ale_ShowPendingAlert ();

	    /***** Show user's record *****/
	    Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
	    break;
	 case Enr_REGISTER_ONE_DEGREE_ADMIN:
	    Enr_ReqAddAdm (Sco_SCOPE_DEG,Gbl.CurrentDeg.Deg.DegCod,
			   Gbl.CurrentDeg.Deg.FullName);
	    break;
	 case Enr_REGISTER_ONE_CENTRE_ADMIN:
	    Enr_ReqAddAdm (Sco_SCOPE_CTR,Gbl.CurrentCtr.Ctr.CtrCod,
			   Gbl.CurrentCtr.Ctr.FullName);
	    break;
	 case Enr_REGISTER_ONE_INSTITUTION_ADMIN:
	    Enr_ReqAddAdm (Sco_SCOPE_INS,Gbl.CurrentIns.Ins.InsCod,
			   Gbl.CurrentIns.Ins.FullName);
	    break;
	 case Enr_REPORT_USR_AS_POSSIBLE_DUPLICATE:
	    Dup_ReportUsrAsPossibleDuplicate ();
	    break;
	 case Enr_REMOVE_ONE_USR_FROM_CRS:
            Enr_ReqRemUsrFromCrs ();
	    break;
	 case Enr_REMOVE_ONE_DEGREE_ADMIN:
            Enr_ReqRemAdmOfDeg ();
	    break;
	 case Enr_REMOVE_ONE_CENTRE_ADMIN:
            Enr_ReqRemAdmOfCtr ();
	    break;
	 case Enr_REMOVE_ONE_INSTITUTION_ADMIN:
            Enr_ReqRemAdmOfIns ();
	    break;
	 case Enr_ELIMINATE_ONE_USR_FROM_PLATFORM:
	    Acc_ReqRemAccountOrRemAccount (Acc_REQUEST_REMOVE_USR);
	    break;
	 default:
	    break;
	}
  }

/*****************************************************************************/
/********* Set a user's acceptation to true in the current course ************/
/*****************************************************************************/

void Enr_AcceptUsrInCrs (long UsrCod)
  {
   char *Query;

   /***** Set enrolment of a user to "accepted" in the current course *****/
   if (asprintf (&Query,"UPDATE crs_usr SET Accepted='Y'"
                        " WHERE CrsCod=%ld AND UsrCod=%ld",
                 Gbl.CurrentCrs.Crs.CrsCod,UsrCod) < 0)
      Lay_NotEnoughMemoryExit ();
   DB_QueryUPDATE_free (Query,"can not confirm user's enrolment");
  }

/*****************************************************************************/
/******************* Ask if really wanted to remove a user *******************/
/*****************************************************************************/

static void Enr_AskIfRemoveUsrFromCrs (struct UsrData *UsrDat)
  {
   extern const char *Txt_Do_you_really_want_to_be_removed_from_the_course_X;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_user_from_the_course_X;
   extern const char *Txt_Remove_me_from_this_course;
   extern const char *Txt_Remove_user_from_this_course;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   bool ItsMe;
   Act_Action_t NextAction;

   if (Usr_CheckIfUsrBelongsToCurrentCrs (UsrDat))
     {
      ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);

      /***** Show question and button to remove user as administrator *****/
      /* Start alert */
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        ItsMe ? Txt_Do_you_really_want_to_be_removed_from_the_course_X :
		        Txt_Do_you_really_want_to_remove_the_following_user_from_the_course_X,
	        Gbl.CurrentCrs.Crs.FullName);
      Ale_ShowAlertAndButton1 (Ale_QUESTION,Gbl.Alert.Txt);

      /* Show user's record */
      Rec_ShowSharedRecordUnmodifiable (UsrDat);

      /* Show form to request confirmation */
      switch (UsrDat->Roles.InCurrentCrs.Role)
        {
	 case Rol_STD:
	    NextAction = ActRemStdCrs;
	    break;
	 case Rol_NET:
	    NextAction = ActRemNETCrs;
	    break;
	 case Rol_TCH:
	    NextAction = ActRemTchCrs;
	    break;
	 default:
	    NextAction = ActUnk;
	    Lay_ShowErrorAndExit ("Wrong role.");
	    break;
        }
      Act_StartForm (NextAction);
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Pwd_AskForConfirmationOnDangerousAction ();
      Btn_PutRemoveButton (ItsMe ? Txt_Remove_me_from_this_course :
                                   Txt_Remove_user_from_this_course);
      Act_EndForm ();

      /* End alert */
      Ale_ShowAlertAndButton2 (ActUnk,NULL,NULL,NULL,Btn_NO_BUTTON,NULL);
     }
   else	      // User does not belong to current course
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/************************ Remove a user from a course ************************/
/*****************************************************************************/

static void Enr_EffectivelyRemUsrFromCrs (struct UsrData *UsrDat,struct Course *Crs,
                                          Enr_RemoveUsrWorks_t RemoveUsrWorks,Cns_QuietOrVerbose_t QuietOrVerbose)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_from_the_course_Y;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char *Query;
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);

   if (Usr_CheckIfUsrBelongsToCurrentCrs (UsrDat))
     {
      /***** Remove user from all the attendance events in course *****/
      Att_RemoveUsrFromCrsAttEvents (UsrDat->UsrCod,Crs->CrsCod);

      /***** Remove user from all the groups in course *****/
      Grp_RemUsrFromAllGrpsInCrs (UsrDat->UsrCod,Crs->CrsCod);

      /***** Remove user's status about reading of course information *****/
      Inf_RemoveUsrFromCrsInfoRead (UsrDat->UsrCod,Crs->CrsCod);

      /***** Remove works zone of this user in course *****/
      if (RemoveUsrWorks == Enr_REMOVE_WORKS)
         Brw_RemoveUsrWorksInCrs (UsrDat,Crs);

      /***** Remove fields of this user in its course record *****/
      Rec_RemoveFieldsCrsRecordInCrs (UsrDat->UsrCod,Crs);

      /***** Remove some information about files in course and groups *****/
      Brw_RemoveSomeInfoAboutCrsUsrFilesFromDB (UsrDat->UsrCod,Crs->CrsCod);

      /***** Remove test results made by user in course *****/
      Tst_RemoveTestResultsMadeByUsrInCrs (UsrDat->UsrCod,Crs->CrsCod);

      /***** Set all the notifications for this user in this course as removed,
             except notifications about new messages *****/
      Ntf_MarkNotifInCrsAsRemoved (UsrDat->UsrCod,Crs->CrsCod);

      /***** Remove user from the table of courses-users *****/
      if (asprintf (&Query,"DELETE FROM crs_usr"
                           " WHERE CrsCod=%ld AND UsrCod=%ld",
                    Crs->CrsCod,UsrDat->UsrCod) < 0)
         Lay_NotEnoughMemoryExit ();
      DB_QueryDELETE_free (Query,"can not remove a user from a course");

      /***** Flush caches *****/
      Usr_FlushCachesUsr ();

      /***** If it's me, change my roles *****/
      if (ItsMe)
	{
	 /* Now I don't belong to current course */
	 Gbl.Usrs.Me.IBelongToCurrentCrs =
         Gbl.Usrs.Me.UsrDat.Accepted     = false;

         /* Fill the list with the courses I belong to */
         Gbl.Usrs.Me.MyCrss.Filled = false;
         Usr_GetMyCourses ();

         /* Set my roles */
	 Gbl.Usrs.Me.Role.FromSession               =
	 Gbl.Usrs.Me.Role.Logged                    =
	 Gbl.Usrs.Me.Role.LoggedBeforeCloseSession  =
	 Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Role =
	 UsrDat->Roles.InCurrentCrs.Role            = Rol_UNK;

	 Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs.Valid =
	 UsrDat->Roles.InCurrentCrs.Valid            = true;

	 Gbl.Usrs.Me.UsrDat.Roles.InCrss =
	 UsrDat->Roles.InCrss            = -1;	// not yet filled/calculated

	 Rol_SetMyRoles ();
	}

      if (QuietOrVerbose == Cns_VERBOSE)
        {
	 Gbl.Alert.Type = Ale_SUCCESS;
         snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	           Txt_THE_USER_X_has_been_removed_from_the_course_Y,
                   UsrDat->FullName,Crs->FullName);
        }
     }
   else        // User does not belong to course
      if (QuietOrVerbose == Cns_VERBOSE)
	{
	 Gbl.Alert.Type = Ale_WARNING;
	 Str_Copy (Gbl.Alert.Txt,Txt_User_not_found_or_you_do_not_have_permission_,
		   Ale_MAX_BYTES_ALERT);
	}
  }

/*****************************************************************************/
/** Ask if really wanted to remove an administrator from current institution */
/*****************************************************************************/

static void Enr_AskIfRemAdm (bool ItsMe,Sco_Scope_t Scope,
			     const char *InsCtrDegName)
  {
   extern const char *Txt_Do_you_really_want_to_be_removed_as_an_administrator_of_X;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_user_as_an_administrator_of_X;
   extern const char *Txt_Remove_me_as_an_administrator;
   extern const char *Txt_Remove_USER_as_an_administrator;
   static const Act_Action_t Enr_ActRemAdm[Sco_NUM_SCOPES] =
     {
      ActUnk,		// Sco_SCOPE_UNK
      ActUnk,		// Sco_SCOPE_SYS,
      ActUnk,		// Sco_SCOPE_CTY,
      ActRemAdmIns,	// Sco_SCOPE_INS,
      ActRemAdmCtr,	// Sco_SCOPE_CTR,
      ActRemAdmDeg,	// Sco_SCOPE_DEG,
      ActUnk,		// Sco_SCOPE_CRS,
     };

   if (Usr_ChkIfUsrCodExists (Gbl.Usrs.Other.UsrDat.UsrCod))
     {
      /***** Show question and button to remove user as administrator *****/
      /* Start alert */
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        ItsMe ? Txt_Do_you_really_want_to_be_removed_as_an_administrator_of_X :
                        Txt_Do_you_really_want_to_remove_the_following_user_as_an_administrator_of_X,
                InsCtrDegName);
      Ale_ShowAlertAndButton1 (Ale_QUESTION,Gbl.Alert.Txt);

      /* Show user's record */
      Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

      /* End alert */
      Ale_ShowAlertAndButton2 (Enr_ActRemAdm[Scope],NULL,NULL,
                               Usr_PutParamOtherUsrCodEncrypted,
                               Btn_REMOVE_BUTTON,
                               ItsMe ? Txt_Remove_me_as_an_administrator :
                                       Txt_Remove_USER_as_an_administrator);
     }
   else
      Lay_ShowErrorAndExit ("User doesn't exist.");
  }

/*****************************************************************************/
/**** Remove an administrator from current institution, centre or degree *****/
/*****************************************************************************/

static void Enr_EffectivelyRemAdm (struct UsrData *UsrDat,Sco_Scope_t Scope,
                                   long Cod,const char *InsCtrDegName)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   extern const char *Txt_THE_USER_X_has_been_removed_as_administrator_of_Y;
   extern const char *Txt_THE_USER_X_is_not_an_administrator_of_Y;
   char *Query;

   if (Usr_CheckIfUsrIsAdm (UsrDat->UsrCod,Scope,Cod))        // User is administrator of current institution/centre/degree
     {
      /***** Remove user from the table of admins *****/
      if (asprintf (&Query,"DELETE FROM admin"
                           " WHERE UsrCod=%ld AND Scope='%s' AND Cod=%ld",
                    UsrDat->UsrCod,Sco_ScopeDB[Scope],Cod) < 0)
         Lay_NotEnoughMemoryExit ();
      DB_QueryDELETE_free (Query,"can not remove an administrator");

      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        Txt_THE_USER_X_has_been_removed_as_administrator_of_Y,
                UsrDat->FullName,InsCtrDegName);
      Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
     }
   else        // User is not an administrator of the current institution/centre/degree
     {
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        Txt_THE_USER_X_is_not_an_administrator_of_Y,
                UsrDat->FullName,InsCtrDegName);
      Ale_ShowAlert (Ale_ERROR,Gbl.Alert.Txt);
     }
  }
