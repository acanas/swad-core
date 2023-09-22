// swad_enrolment.c: enrolment (registration) or removing of users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
#include "swad_account_database.h"
#include "swad_action_list.h"
#include "swad_admin.h"
#include "swad_admin_database.h"
#include "swad_alert.h"
#include "swad_announcement.h"
#include "swad_attendance_database.h"
#include "swad_box.h"
#include "swad_browser_database.h"
#include "swad_database.h"
#include "swad_duplicate.h"
#include "swad_enrolment.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_exam_database.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_info_database.h"
#include "swad_mail_database.h"
#include "swad_match.h"
#include "swad_message.h"
#include "swad_message_database.h"
#include "swad_notification.h"
#include "swad_notification_database.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_record_database.h"
#include "swad_role.h"
#include "swad_role_database.h"
#include "swad_setting.h"
#include "swad_setting_database.h"
#include "swad_test_print.h"
#include "swad_user.h"
#include "swad_user_database.h"

/*****************************************************************************/
/******************************* Private types *******************************/
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

/* Remove important user production (works and match results)? */
typedef enum
  {
   Enr_DO_NOT_REMOVE_USR_PRODUCTION,
   Enr_REMOVE_USR_PRODUCTION,
  } Enr_RemoveUsrProduction_t;

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Enr_NotifyAfterEnrolment (const struct Usr_Data *UsrDat,
                                      Rol_Role_t NewRole);

static void Enr_ReqAdminUsrs (Rol_Role_t Role);
static void Enr_ShowFormRegRemSeveralUsrs (Rol_Role_t Role);
static void Enr_PutAreaToEnterUsrsIDs (void);
static void Enr_PutActionsRegRemSeveralUsrs (void);

static void Enr_ReceiveFormUsrsCrs (Rol_Role_t Role);

static void Enr_PutActionModifyOneUsr (bool *OptionChecked,
                                       bool UsrBelongsToCrs,Usr_MeOrOther_t MeOrOther);
static void Enr_PutActionRegOneDegAdm (bool *OptionChecked);
static void Enr_PutActionRegOneCtrAdm (bool *OptionChecked);
static void Enr_PutActionRegOneInsAdm (bool *OptionChecked);
static void Enr_PutActionRepUsrAsDup (bool *OptionChecked);
static void Enr_PutActionRemUsrFromCrs (bool *OptionChecked,Usr_MeOrOther_t MeOrOther);
static void Enr_PutActionRemUsrAsDegAdm (bool *OptionChecked,Usr_MeOrOther_t MeOrOther);
static void Enr_PutActionRemUsrAsCtrAdm (bool *OptionChecked,Usr_MeOrOther_t MeOrOther);
static void Enr_PutActionRemUsrAsInsAdm (bool *OptionChecked,Usr_MeOrOther_t MeOrOther);
static void Enr_PutActionRemUsrAcc (bool *OptionChecked,Usr_MeOrOther_t MeOrOther);
static void Enr_RegRemOneUsrActionBegin (Enr_RegRemOneUsrAction_t RegRemOneUsrAction,
                                         bool *OptionChecked);
static void Enr_RegRemOneUsrActionEnd (void);

static void Enr_RegisterUsr (struct Usr_Data *UsrDat,Rol_Role_t RegRemRole,
                             struct ListCodGrps *LstGrps,unsigned *NumUsrsRegistered);

static void Enr_PutLinkToRemAllStdsThisCrs (void);

static void Enr_ShowEnrolmentRequestsGivenRoles (unsigned RolesSelected);

static void Enr_RemUsrEnrolmentRequestInCrs (long UsrCod,long CrsCod);

static void Enr_ReqRegRemUsr (Rol_Role_t Role);
static void Enr_ReqAnotherUsrIDToRegisterRemove (Rol_Role_t Role);
static void Enr_AskIfRegRemMe (Rol_Role_t Role);
static void Enr_AskIfRegRemAnotherUsr (Rol_Role_t Role);
static void Enr_AskIfRegRemUsr (struct Usr_ListUsrCods *ListUsrCods,Rol_Role_t Role);

static void Enr_ShowFormToEditOtherUsr (void);

static bool Enr_CheckIfICanRemUsrFromCrs (void);

static void Enr_AskIfRemoveUsrFromCrs (struct Usr_Data *UsrDat);
static void Enr_EffectivelyRemUsrFromCrs (struct Usr_Data *UsrDat,
					  struct Hie_Node *Crs,
                                          Enr_RemoveUsrProduction_t RemoveUsrWorks,
					  Cns_QuietOrVerbose_t QuietOrVerbose);

static FigCch_FigureCached_t Enr_GetFigureNumUsrsInCrss (unsigned Roles);

/*****************************************************************************/
/** Check if current course has students and show warning no students found **/
/*****************************************************************************/

void Enr_CheckStdsAndPutButtonToRegisterStdsInCurrentCrs (void)
  {
   /***** Put link to register students *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_TCH)	// Course selected and I am logged as teacher
      if (!Enr_GetNumUsrsInCrss (Hie_CRS,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				 1 << Rol_STD))	// No students in course
         Usr_ShowWarningNoUsersFound (Rol_STD);
  }

/*****************************************************************************/
/****************** Put inline button to register students *******************/
/*****************************************************************************/

void Enr_PutButtonInlineToRegisterStds (long CrsCod)
  {
   extern const char *Txt_Register_students;

   if (Gbl.Usrs.Me.Role.Logged == Rol_TCH)	// Course selected and I am logged as teacher
      if (!Enr_GetNumUsrsInCrss (Hie_CRS,CrsCod,
				 1 << Rol_STD))	// No students in course
	{
	 Frm_BeginForm (ActReqEnrSevStd);
	    ParCod_PutPar (ParCod_Crs,CrsCod);
	    Btn_PutCreateButtonInline (Txt_Register_students);
	 Frm_EndForm ();
	}
  }

/*****************************************************************************/
/************ Show form to request sign up in the current course *************/
/*****************************************************************************/

void Enr_PutLinkToRequestSignUp (void)
  {
   extern const char *Txt_Sign_up;

   /***** Show the form *****/
   Lay_PutContextualLinkIconText (ActReqSignUp,NULL,
                                  NULL,NULL,
				  "hand-point-up.svg",Ico_BLACK,
				  Txt_Sign_up,NULL);
  }

/*****************************************************************************/
/***************** Modify the role of a user in a course *********************/
/*****************************************************************************/

void Enr_ModifyRoleInCurrentCrs (struct Usr_Data *UsrDat,Rol_Role_t NewRole)
  {
   /***** Trivial check 1: current course code should be > 0 *****/
   if (Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)
      Err_WrongCourseExit ();

   /***** Trivial check 2: check if user's role is allowed *****/
   switch (NewRole)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 break;
      default:
	 Err_WrongRoleExit ();
     }

   /***** Update the role of a user in a course *****/
   Rol_DB_UpdateUsrRoleInCrs (Gbl.Hierarchy.Node[Hie_CRS].HieCod,UsrDat->UsrCod,NewRole);

   /***** Flush caches *****/
   Usr_FlushCachesUsr ();

   /***** Set user's roles *****/
   UsrDat->Roles.InCurrentCrs = NewRole;
   UsrDat->Roles.InCrss = -1;	// Force roles to be got from database
   Rol_GetRolesInAllCrss (UsrDat);	// Get roles

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

void Enr_RegisterUsrInCurrentCrs (struct Usr_Data *UsrDat,Rol_Role_t NewRole,
                                  Enr_KeepOrSetAccepted_t KeepOrSetAccepted)
  {
   /***** Trivial check 1: current course code should be > 0 *****/
   if (Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)
      Err_WrongCourseExit ();

   /***** Trivial check 2: check if user's role is allowed *****/
   switch (NewRole)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 break;
      default:
         Err_WrongRoleExit ();
     }

   /***** Register user in current course in database *****/
   Enr_DB_InsertUsrInCurrentCrs (UsrDat->UsrCod,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
                                 NewRole,KeepOrSetAccepted);

   /***** Register last prefs in current course in database *****/
   Set_DB_InsertUsrInCrsSettings (UsrDat->UsrCod,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   /***** Flush caches *****/
   Usr_FlushCachesUsr ();

   /***** Set roles *****/
   UsrDat->Roles.InCurrentCrs = NewRole;
   UsrDat->Roles.InCrss = -1;	// Force roles to be got from database
   Rol_GetRolesInAllCrss (UsrDat);	// Get roles

   /***** Create notification for this user.
	  If this user wants to receive notifications by email,
	  activate the sending of a notification *****/
   Enr_NotifyAfterEnrolment (UsrDat,NewRole);
  }

/*****************************************************************************/
/********* Create notification after register user in current course *********/
/*****************************************************************************/

static void Enr_NotifyAfterEnrolment (const struct Usr_Data *UsrDat,
                                      Rol_Role_t NewRole)
  {
   static const Ntf_NotifyEvent_t NotifyEvent[Rol_NUM_ROLES] =
     {
      [Rol_STD] = Ntf_EVENT_ENROLMENT_STD,
      [Rol_NET] = Ntf_EVENT_ENROLMENT_NET,
      [Rol_TCH] = Ntf_EVENT_ENROLMENT_TCH,
     };
   bool CreateNotif;
   bool NotifyByEmail;

   /***** Check if user's role is allowed *****/
   if (!NotifyEvent[NewRole])
      Err_WrongRoleExit ();

   /***** Remove possible enrolment request ******/
   Enr_RemUsrEnrolmentRequestInCrs (UsrDat->UsrCod,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   /***** Remove old enrolment notifications before inserting the new one ******/
   Ntf_DB_MarkNotifToOneUsrAsRemoved (Ntf_EVENT_ENROLMENT_STD,-1,UsrDat->UsrCod);
   Ntf_DB_MarkNotifToOneUsrAsRemoved (Ntf_EVENT_ENROLMENT_NET,-1,UsrDat->UsrCod);
   Ntf_DB_MarkNotifToOneUsrAsRemoved (Ntf_EVENT_ENROLMENT_TCH,-1,UsrDat->UsrCod);

   /***** Create new notification ******/
   CreateNotif = (UsrDat->NtfEvents.CreateNotif & (1 << NotifyEvent[NewRole]));
   NotifyByEmail = CreateNotif && Usr_ItsMe (UsrDat->UsrCod) == Usr_OTHER &&
		   (UsrDat->NtfEvents.SendEmail & (1 << NotifyEvent[NewRole]));
   if (CreateNotif)
      Ntf_DB_StoreNotifyEventToUsr (NotifyEvent[NewRole],UsrDat->UsrCod,-1L,
				    (Ntf_Status_t) (NotifyByEmail ? Ntf_STATUS_BIT_EMAIL :
					                            0),
				    Gbl.Hierarchy.Node[Hie_INS].HieCod,
				    Gbl.Hierarchy.Node[Hie_CTR].HieCod,
				    Gbl.Hierarchy.Node[Hie_DEG].HieCod,
				    Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/****** Write a form to request another user's ID, @nickname or email ********/
/*****************************************************************************/

void Enr_WriteFormToReqAnotherUsrID (Act_Action_t NextAction,void (*FuncPars) (void))
  {
   extern const char *Txt_nick_email_or_ID;
   extern const char *Txt_Continue;

   /***** Form to request user's ID, @nickname or email address *****/
   Frm_BeginForm (NextAction);
      if (FuncPars)
	 FuncPars ();

      /***** Label *****/
      HTM_LABEL_Begin ("for=\"OtherUsrIDNickOrEMail\" class=\"RM FORM_IN_%s\"",
		       The_GetSuffix ());
	 HTM_TxtColonNBSP (Txt_nick_email_or_ID);
      HTM_LABEL_End ();

      /***** Input box to enter user *****/
      HTM_INPUT_TEXT ("OtherUsrIDNickOrEMail",Cns_MAX_CHARS_EMAIL_ADDRESS,"",
		      HTM_DONT_SUBMIT_ON_CHANGE,
		      "id=\"OtherUsrIDNickOrEMail\" size=\"18\""
		      " class=\"INPUT_%s\""
		      " required=\"required\"",
		      The_GetSuffix ());

      /***** Send button*****/
      Btn_PutConfirmButton (Txt_Continue);

   Frm_EndForm ();
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
   static const struct
     {
      struct
        {
	 Act_Action_t Accept;
	 Act_Action_t Refuse;
        } NextAction;
      Ntf_NotifyEvent_t NotifyEvent;
     } WhatToDo[Rol_NUM_ROLES] =
     {
      [Rol_STD] = {{ActAccEnrStd,ActRemMe_Std},Ntf_EVENT_ENROLMENT_STD},
      [Rol_NET] = {{ActAccEnrNET,ActRemMe_NET},Ntf_EVENT_ENROLMENT_NET},
      [Rol_TCH] = {{ActAccEnrTch,ActRemMe_Tch},Ntf_EVENT_ENROLMENT_TCH},
     };

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Enrolment,
                 NULL,NULL,
                 Hlp_USERS_SignUp_confirm_enrolment,Box_NOT_CLOSABLE);

      /***** Show message *****/
      Ale_ShowAlert (Ale_INFO,Txt_A_teacher_or_administrator_has_enroled_you_as_X_into_the_course_Y,
		     Txt_ROLES_SINGUL_abc[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs][Gbl.Usrs.Me.UsrDat.Sex],
		     Gbl.Hierarchy.Node[Hie_CRS].FullName);

      /***** Send button to accept register in the current course *****/
      if (!WhatToDo[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs].NextAction.Accept)
	 Err_WrongRoleExit ();
      Frm_BeginForm (WhatToDo[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs].NextAction.Accept);
	 Btn_PutCreateButtonInline (Txt_Confirm_my_enrolment);
      Frm_EndForm ();

      /***** Send button to refuse register in the current course *****/
      if (!WhatToDo[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs].NextAction.Refuse)
	 Err_WrongRoleExit ();
      Frm_BeginForm (WhatToDo[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs].NextAction.Refuse);
	 Btn_PutRemoveButtonInline (Txt_Remove_me_from_this_course);
      Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Mark possible notification as seen *****/
   if (!WhatToDo[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs].NotifyEvent)
      Err_WrongRoleExit ();
   Ntf_DB_MarkNotifsInCrsAsSeen (WhatToDo[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs].NotifyEvent);
  }

/*****************************************************************************/
/****************** Put an enrolment into a notification ********************/
/*****************************************************************************/

void Enr_GetNotifEnrolment (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                            long CrsCod,long UsrCod)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   struct Usr_Data UsrDat;
   Rol_Role_t Role;

   /***** Get user's role in course from database *****/
   Role = Rol_GetRoleUsrInCrs (UsrCod,CrsCod);

   /***** Set summary string *****/
   /* Initialize structure with user's data */
   Usr_UsrDataConstructor (&UsrDat);

   /* Get user's data */
   UsrDat.UsrCod = UsrCod;
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
					    Usr_DONT_GET_PREFS,
					    Usr_DONT_GET_ROLE_IN_CURRENT_CRS);

   /* Set summary string depending on role and sex */
   Str_Copy (SummaryStr,Txt_ROLES_SINGUL_Abc[Role][UsrDat.Sex],
	     Ntf_MAX_BYTES_SUMMARY);

   /* Free memory used for user's data */
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/***************************** Update user's data ****************************/
/*****************************************************************************/
// UsrDat->UsrCod must be > 0

void Enr_UpdateUsrData (struct Usr_Data *UsrDat)
  {
   /***** Check if user's code is initialized *****/
   if (UsrDat->UsrCod <= 0)
      Err_WrongUserExit ();

   /***** Filter some user's data before updating */
   Usr_FilterUsrBirthday (&UsrDat->Birthday);

   /***** Update user's common data *****/
   Acc_DB_UpdateUsrData (UsrDat);
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
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_GST:
      case Rol_STD:
      case Rol_NET:
	 Enr_AskIfRegRemMe (Role);
	 break;
      case Rol_TCH:
	 if (Gbl.Hierarchy.Level == Hie_CRS && Role == Rol_STD)
	    Enr_ShowFormRegRemSeveralUsrs (Rol_STD);
	 else
	    Enr_AskIfRegRemMe (Rol_TCH);
	 break;
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 if (Gbl.Hierarchy.Level == Hie_CRS)
	    Enr_ShowFormRegRemSeveralUsrs (Role);
	 else
	    Enr_ReqAnotherUsrIDToRegisterRemove (Role);
	 break;
      default:
	 Err_NoPermissionExit ();
	 break;
     }
  }

/*****************************************************************************/
/***** Register/remove users (taken from a list) in/from current course ******/
/*****************************************************************************/

static void Enr_ShowFormRegRemSeveralUsrs (Rol_Role_t Role)
  {
   extern const char *Hlp_USERS_Administration_administer_multiple_users;
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
   static const struct
     {
      Act_Action_t NextAction;
      const char **Title;
     } WhatToDo[Rol_NUM_ROLES] =
     {
      [Rol_STD] = {ActRcvFrmEnrSevStd,&Txt_Administer_multiple_students            },
      [Rol_NET] = {ActRcvFrmEnrSevNET,&Txt_Administer_multiple_non_editing_teachers},
      [Rol_TCH] = {ActRcvFrmEnrSevTch,&Txt_Administer_multiple_teachers            },
     };

   /***** Contextual menu *****/
   if (Gbl.Hierarchy.Level == Hie_CRS)	 	// Course selected
     {
      Mnu_ContextMenuBegin ();

	 switch (Role)
	   {
	    case Rol_STD:
	       /* Put link to go to admin student */
	       Enr_PutLinkToAdminOneUsr (ActReqMdfOneStd);

	       /* Put link to remove all students in the current course */
	       if (Enr_GetNumUsrsInCrss (Hie_CRS,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
					 1 << Rol_STD))	// This course has students
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
	       Err_WrongRoleExit ();
	       break;
	   }

      Mnu_ContextMenuEnd ();
     }

   /***** Form to send students to be enroled / removed *****/
   if (!WhatToDo[Role].NextAction)
      Err_WrongRoleExit ();
   Frm_BeginForm (WhatToDo[Role].NextAction);

      /***** Begin box *****/
      Box_BoxBegin (NULL,*WhatToDo[Role].Title,
		    NULL,NULL,
		    Hlp_USERS_Administration_administer_multiple_users,Box_NOT_CLOSABLE);

	 /***** Step 1: List of students to be enroled / removed *****/
	 HTM_DIV_Begin ("class=\"TITLE_%s LM\"",The_GetSuffix ());
	    HTM_Txt (Txt_Step_1_Provide_a_list_of_users);
	 HTM_DIV_End ();

	 Ale_ShowAlert (Ale_INFO,Txt_Type_or_paste_a_list_of_IDs_nicks_or_emails_);
	 Enr_PutAreaToEnterUsrsIDs ();

	 /***** Step 2: Put different actions to register/remove users to/from current course *****/
	 HTM_DIV_Begin ("class=\"TITLE_%s LM\"",The_GetSuffix ());
	    HTM_Txt (Txt_Step_2_Select_the_desired_action);
	 HTM_DIV_End ();
	 Enr_PutActionsRegRemSeveralUsrs ();

	 /***** Step 3: Select groups in which register / remove users *****/
	 HTM_DIV_Begin ("class=\"TITLE_%s LM\"",The_GetSuffix ());
	    HTM_Txt (Txt_Step_3_Optionally_select_groups);
	 HTM_DIV_End ();
	 if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
	   {
	    if (Gbl.Crs.Grps.NumGrps)	// This course has groups?
	      {
	       Ale_ShowAlert (Ale_INFO,Txt_Select_the_groups_in_from_which_you_want_to_register_remove_users_);
	       Grp_ShowLstGrpsToChgOtherUsrsGrps (-1L);
	      }
	    else
	       /* Write help message */
	       Ale_ShowAlert (Ale_INFO,Txt_No_groups_have_been_created_in_the_course_X_Therefore_,
			      Gbl.Hierarchy.Node[Hie_CRS].FullName);
	   }

	 /***** Step 4: Confirm register / remove students *****/
	 HTM_DIV_Begin ("class=\"TITLE_%s LM\"",The_GetSuffix ());
	    HTM_Txt (Txt_Step_4_Confirm_the_enrolment_removing);
	 HTM_DIV_End ();
	 Pwd_AskForConfirmationOnDangerousAction ();

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Confirm);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Put a link (form) to remove old users ******************/
/*****************************************************************************/

void Enr_PutLinkToRemOldUsrs (void)
  {
   extern const char *Txt_Eliminate_old_users;

   /***** Put form to remove old users *****/
   Lay_PutContextualLinkIconText (ActReqRemOldUsr,NULL,
                                  NULL,NULL,
				  "trash.svg",Ico_RED,
				  Txt_Eliminate_old_users,NULL);
  }

/*****************************************************************************/
/*********************** Write form to remove old users **********************/
/*****************************************************************************/

void Enr_AskRemoveOldUsrs (void)
  {
   extern const char *Txt_Eliminate_old_users;
   extern const char *Txt_Eliminate_all_users_who_are_not_enroled_on_any_courses_PART_1_OF_2;
   extern const char *Txt_Eliminate_all_users_who_are_not_enroled_on_any_courses_PART_2_OF_2;
   extern const char *Txt_Eliminate;
   unsigned Months;

   /***** Begin form *****/
   Frm_BeginForm (ActRemOldUsr);

      /***** Begin box *****/
      Box_BoxBegin (NULL,Txt_Eliminate_old_users,
		    NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);

	 /***** Form to request number of months without clicks *****/
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_TxtF ("%s&nbsp;",Txt_Eliminate_all_users_who_are_not_enroled_on_any_courses_PART_1_OF_2);
	    HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,NULL,
			      "name=\"Months\" class=\"INPUT_%s\"",
			      The_GetSuffix ());
	       for (Months  = Usr_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS;
		    Months <= Usr_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS;
		    Months++)
		  HTM_OPTION (HTM_Type_UNSIGNED,&Months,
			      Months == Usr_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS ? HTM_OPTION_SELECTED :
											   HTM_OPTION_UNSELECTED,
			      HTM_OPTION_ENABLED,
			      "%u",Months);
	    HTM_SELECT_End ();
	    HTM_NBSP ();
	    HTM_TxtF (Txt_Eliminate_all_users_who_are_not_enroled_on_any_courses_PART_2_OF_2,
		      Cfg_PLATFORM_SHORT_NAME);
	 HTM_LABEL_End ();

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_REMOVE_BUTTON,Txt_Eliminate);

   /***** End form *****/
   Frm_EndForm ();
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
   unsigned NumUsr;
   unsigned NumUsrs;
   unsigned NumUsrsEliminated = 0;
   struct Usr_Data UsrDat;

   /***** Get parameter with number of months without access *****/
   MonthsWithoutAccess = (unsigned)
	                 Par_GetParUnsignedLong ("Months",
                                                 Usr_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS,
                                                 Usr_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS,
                                                 (unsigned long) UINT_MAX);
   if (MonthsWithoutAccess == UINT_MAX)
      Err_ShowErrorAndExit ("Wrong number of months.");
   SecondsWithoutAccess = (time_t) MonthsWithoutAccess * Dat_SECONDS_IN_ONE_MONTH;

   /***** Get old users from database *****/
   if ((NumUsrs = Usr_DB_GetOldUsrs (&mysql_res,SecondsWithoutAccess)))
     {
      Ale_ShowAlert (Ale_INFO,Txt_Eliminating_X_users_who_were_not_enroled_in_any_course_and_with_more_than_Y_months_without_access_to_Z,
                     NumUsrs,
                     MonthsWithoutAccess,
                     Cfg_PLATFORM_SHORT_NAME);

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Remove users *****/
      for (NumUsr = 0;
           NumUsr < NumUsrs;
           NumUsr++)
        {
         UsrDat.UsrCod = DB_GetNextCode (mysql_res);
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
                                                      Usr_DONT_GET_PREFS,
                                                      Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
           {
            // User's data exist...
            Acc_CompletelyEliminateAccount (&UsrDat,Cns_QUIET);
            NumUsrsEliminated++;
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Move unused contents of messages to table of deleted contents of messages *****/
      Msg_DB_MoveUnusedMsgsContentToDeleted ();
     }

   /***** Write end message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_X_users_have_been_eliminated,
                  NumUsrsEliminated);
  }

/*****************************************************************************/
/***** Put text area to enter/paste IDs of users to be enroled/removed ******/
/*****************************************************************************/

static void Enr_PutAreaToEnterUsrsIDs (void)
  {
   extern const char *Txt_List_of_nicks_emails_or_IDs;

   /***** Text area for users' IDs *****/
   HTM_TABLE_BeginCenterPadding (2);
      HTM_TR_Begin (NULL);

	 /* Label */
	 Frm_LabelColumn ("RT","UsrsIDs",Txt_List_of_nicks_emails_or_IDs);

	 /* Data */
	 HTM_TD_Begin ("class=\"LT\"");
	    HTM_TEXTAREA_Begin ("id=\"UsrsIDs\" name=\"UsrsIDs\""
		                " cols=\"60\" rows=\"10\""
		                " class=\"INPUT_%s\"",
		                The_GetSuffix ());
	    HTM_TEXTAREA_End ();
	 HTM_TD_End ();

      HTM_TR_End ();
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/*** Put different actions to register/remove users to/from current course ***/
/*****************************************************************************/

static void Enr_PutActionsRegRemSeveralUsrs (void)
  {
   extern const char *Txt_Register_the_users_indicated_in_step_1;
   extern const char *Txt_Remove_the_users_indicated_in_step_1;
   extern const char *Txt_Remove_the_users_not_indicated_in_step_1;
   extern const char *Txt_Register_the_users_indicated_in_step_1_and_remove_the_users_not_indicated;
   extern const char *Txt_Eliminate_from_the_platform_the_users_indicated_in_step_1;

   /***** Begin list of options *****/
   HTM_UL_Begin ("class=\"LIST_LEFT FORM_IN_%s\"",The_GetSuffix ());

      /***** Register / remove users listed or not listed *****/
      if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
	{
	 HTM_LI_Begin (NULL);
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_RADIO ("RegRemAction",HTM_DONT_SUBMIT_ON_CLICK,
				" value=\"%u\" checked=\"checked\"",
				(unsigned) Enr_REGISTER_SPECIFIED_USRS_IN_CRS);
	       HTM_Txt (Txt_Register_the_users_indicated_in_step_1);
	    HTM_LABEL_End ();
	 HTM_LI_End ();

	 HTM_LI_Begin (NULL);
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_RADIO ("RegRemAction",HTM_DONT_SUBMIT_ON_CLICK,
				" value=\"%u\"",
				(unsigned) Enr_REMOVE_SPECIFIED_USRS_FROM_CRS);
	       HTM_Txt (Txt_Remove_the_users_indicated_in_step_1);
	    HTM_LABEL_End ();
	 HTM_LI_End ();

	 HTM_LI_Begin (NULL);
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_RADIO ("RegRemAction",HTM_DONT_SUBMIT_ON_CLICK,
				" value=\"%u\"",
				(unsigned) Enr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS);
	       HTM_Txt (Txt_Remove_the_users_not_indicated_in_step_1);
	    HTM_LABEL_End ();
	 HTM_LI_End ();

	 HTM_LI_Begin (NULL);
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_RADIO ("RegRemAction",HTM_DONT_SUBMIT_ON_CLICK,
				" value=\"%u\"",
				(unsigned) Enr_UPDATE_USRS_IN_CRS);
	       HTM_Txt (Txt_Register_the_users_indicated_in_step_1_and_remove_the_users_not_indicated);
	    HTM_LABEL_End ();
	 HTM_LI_End ();
	}

      /***** Only for superusers *****/
      if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	{
	 HTM_LI_Begin (NULL);
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_RADIO ("RegRemAction",HTM_DONT_SUBMIT_ON_CLICK,
				" value=\"%u\"",
				(unsigned) Enr_ELIMINATE_USRS_FROM_PLATFORM);
	       HTM_Txt (Txt_Eliminate_from_the_platform_the_users_indicated_in_step_1);
	    HTM_LABEL_End ();
	 HTM_LI_End ();
	}

   /***** End list of options *****/
   HTM_UL_End ();
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
   struct Usr_ListUsrCods ListUsrCods;	// List with users' codes for a given user's ID
   unsigned NumUsrFound;
   const char *Ptr;
   unsigned NumCurrentUsr;
   unsigned NumUsrsRegistered = 0;
   unsigned NumUsrsRemoved = 0;
   unsigned NumUsrsEliminated = 0;
   struct ListCodGrps LstGrps;
   struct Usr_Data UsrDat;
   bool ItLooksLikeAUsrID;
   Enr_RegRemUsrsAction_t RegRemUsrsAction;

   /***** Check the role of users to register / remove *****/
   switch (Role)
     {
      case Rol_STD:
	 if (Gbl.Usrs.Me.Role.Logged < Rol_TCH)		// Can I register/remove students?
	    // No, I can not
	    Err_NoPermissionExit ();
	 break;
      case Rol_NET:
      case Rol_TCH:
	 if (Gbl.Usrs.Me.Role.Logged < Rol_DEG_ADM)	// Can I register/remove teachers?
	    // No, I can not
	    Err_NoPermissionExit ();
	 break;
      default:
	 Err_WrongRoleExit ();
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
	              Par_GetParUnsignedLong ("RegRemAction",
                                              0,
                                              Enr_NUM_REG_REM_USRS_ACTIONS - 1,
                                              (unsigned long) Enr_REG_REM_USRS_UNKNOWN_ACTION);
   switch (RegRemUsrsAction)
     {
      case Enr_REGISTER_SPECIFIED_USRS_IN_CRS:
	 WhatToDo.RemoveUsrs = false;
	 WhatToDo.RemoveSpecifiedUsrs = false;	// Ignored
	 WhatToDo.EliminateUsrs = false;	// Ignored
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
	    Err_NoPermissionExit ();
	 break;
      default:
	 Err_ShowErrorAndExit ("Wrong registering / removing specification.");
	 break;
     }

   /***** Get groups to which register/remove users *****/
   LstGrps.NumGrps = 0;
   if (Gbl.Crs.Grps.NumGrps) // This course has groups?
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
   if ((ListUsrsIDs = malloc (ID_MAX_BYTES_LIST_USRS_IDS + 1)) == NULL)
      Err_NotEnoughMemoryExit ();
   Par_GetParText ("UsrsIDs",ListUsrsIDs,ID_MAX_BYTES_LIST_USRS_IDS);

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Remove users *****/
   if (WhatToDo.RemoveUsrs)
     {
      /***** Get list of users in current course *****/
      Usr_GetListUsrs (Hie_CRS,Role);

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
	                                     sizeof (UsrDat.UsrIDNickOrEmail) - 1);

	    /* Reset default list of users' codes */
	    ListUsrCods.NumUsrs = 0;
	    ListUsrCods.Lst = NULL;

	    /* Check if string is a user's ID, user's nickname or user's email address */
	    if (Nck_CheckIfNickWithArrIsValid (UsrDat.UsrIDNickOrEmail))	// 1: It's a nickname
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
	       if ((UsrDat.UsrCod = Mai_DB_GetUsrCodFromEmail (UsrDat.UsrIDNickOrEmail)) > 0)
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
			    sizeof (UsrDat.IDs.List[0].ID) - 1);
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
	    else	// Remove all users (of the role) except these specified
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
	       if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
	                                                    Usr_DONT_GET_PREFS,
	                                                    Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
		 {
		  // User's data exist...
		  if (WhatToDo.EliminateUsrs)                // Eliminate user completely from the platform
		    {
		     Acc_CompletelyEliminateAccount (&UsrDat,Cns_QUIET);                // Remove definitely the user from the platform
		     NumUsrsEliminated++;
		    }
		  else
		    {
		     if (Gbl.Crs.Grps.NumGrps)        // If there are groups in the course
		       {
			if (LstGrps.NumGrps)        // If the teacher has selected groups
			  {
			   if (Grp_RemoveUsrFromGroups (&UsrDat,&LstGrps))                // Remove user from the selected groups, not from the whole course
			      NumUsrsRemoved++;
			  }
			else        // The teacher has not selected groups
			  {
			   Enr_EffectivelyRemUsrFromCrs (&UsrDat,&Gbl.Hierarchy.Node[Hie_CRS],
							 Enr_DO_NOT_REMOVE_USR_PRODUCTION,
							 Cns_QUIET);        // Remove user from the course
			   NumUsrsRemoved++;
			  }
		       }
		     else        // No groups
		       {
			Enr_EffectivelyRemUsrFromCrs (&UsrDat,&Gbl.Hierarchy.Node[Hie_CRS],
						      Enr_DO_NOT_REMOVE_USR_PRODUCTION,
						      Cns_QUIET);        // Remove user from the course
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
	                                  sizeof (UsrDat.UsrIDNickOrEmail) - 1);

	 /* Reset default list of users' codes */
	 ListUsrCods.NumUsrs = 0;
	 ListUsrCods.Lst = NULL;

	 /* Check if the string is a user's ID, a user's nickname or a user's email address */
	 if (Nck_CheckIfNickWithArrIsValid (UsrDat.UsrIDNickOrEmail))	// 1: It's a nickname
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
	    if ((UsrDat.UsrCod = Mai_DB_GetUsrCodFromEmail (UsrDat.UsrIDNickOrEmail)) > 0)
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
			 sizeof (UsrDat.IDs.List[0].ID) - 1);
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
      Msg_DB_MoveUnusedMsgsContentToDeleted ();

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
	       Ale_ShowAlert (Ale_SUCCESS,Txt_X_users_have_been_eliminated,
			      NumUsrsEliminated);
	       break;
	   }
      else                       	 // Only remove from course / groups
	 switch (NumUsrsRemoved)
	   {
	    case 0:
	       Ale_ShowAlert (Ale_INFO,Txt_No_user_has_been_removed);
	       break;
	    case 1:
	       Ale_ShowAlert (Ale_SUCCESS,Txt_One_user_has_been_removed);
	       break;
	    default:
	       Ale_ShowAlert (Ale_SUCCESS,Txt_X_users_have_been_removed,
			      NumUsrsRemoved);
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
	    Ale_ShowAlert (Ale_SUCCESS,Txt_X_users_have_been_enroled_including_possible_repetitions,
		           NumUsrsRegistered);
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
/*** Put different actions to register/remove users to/from current course ***/
/*****************************************************************************/
// Returns true if at least one action can be shown

bool Enr_PutActionsRegRemOneUsr (Usr_MeOrOther_t MeOrOther)
  {
   bool OptionsShown = false;
   bool UsrBelongsToCrs = false;
   bool UsrIsDegAdmin = false;
   bool UsrIsCtrAdmin = false;
   bool UsrIsInsAdmin = false;
   bool OptionChecked = false;

   /***** Check if the other user belongs to the current course *****/
   if (Gbl.Hierarchy.Level == Hie_CRS)
      UsrBelongsToCrs = Enr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat);

   if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
     {
      /***** Check if the other user is administrator of the current institution *****/
      UsrIsInsAdmin = Adm_DB_CheckIfUsrIsAdm (Gbl.Usrs.Other.UsrDat.UsrCod,
					      Hie_INS);

      if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)
	{
	 /***** Check if the other user is administrator of the current center *****/
	 UsrIsCtrAdmin = Adm_DB_CheckIfUsrIsAdm (Gbl.Usrs.Other.UsrDat.UsrCod,
						 Hie_CTR);

	 if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)
	    /***** Check if the other user is administrator of the current degree *****/
	    UsrIsDegAdmin = Adm_DB_CheckIfUsrIsAdm (Gbl.Usrs.Other.UsrDat.UsrCod,
						    Hie_DEG);
	}
     }

   /***** Begin list of options *****/
   HTM_UL_Begin ("class=\"LIST_LEFT FORM_IN_%s\"",The_GetSuffix ());

      /***** Register user in course / Modify user's data *****/
      if (Gbl.Hierarchy.Level == Hie_CRS && Gbl.Usrs.Me.Role.Logged >= Rol_STD)
	{
	 Enr_PutActionModifyOneUsr (&OptionChecked,UsrBelongsToCrs,MeOrOther);
	 OptionsShown = true;
	}

      if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
	{
	 if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)
	   {
	    if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)
	       /***** Register user as administrator of degree *****/
	       if (!UsrIsDegAdmin && Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
		 {
		  Enr_PutActionRegOneDegAdm (&OptionChecked);
		  OptionsShown = true;
		 }

	    /***** Register user as administrator of center *****/
	    if (!UsrIsCtrAdmin && Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
	      {
	       Enr_PutActionRegOneCtrAdm (&OptionChecked);
	       OptionsShown = true;
	      }
	   }

	 /***** Register user as administrator of institution *****/
	 if (!UsrIsInsAdmin && Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	   {
	    Enr_PutActionRegOneInsAdm (&OptionChecked);
	    OptionsShown = true;
	   }
	}

      /***** Report user as possible duplicate *****/
      if (MeOrOther == Usr_OTHER && Gbl.Usrs.Me.Role.Logged >= Rol_TCH)
	{
	 Enr_PutActionRepUsrAsDup (&OptionChecked);
	 OptionsShown = true;
	}

      /***** Remove user from the course *****/
      if (UsrBelongsToCrs)
	{
	 Enr_PutActionRemUsrFromCrs (&OptionChecked,MeOrOther);
	 OptionsShown = true;
	}

      if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
	{
	 if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)
	   {
	    if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
	       /***** Remove user as an administrator of the degree *****/
	       if (UsrIsDegAdmin &&
	           (MeOrOther == Usr_ME || Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM))
		 {
		  Enr_PutActionRemUsrAsDegAdm (&OptionChecked,MeOrOther);
		  OptionsShown = true;
		 }

	    /***** Remove user as an administrator of the center *****/
	    if (UsrIsCtrAdmin &&
		(MeOrOther == Usr_ME || Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM))
	      {
	       Enr_PutActionRemUsrAsCtrAdm (&OptionChecked,MeOrOther);
	       OptionsShown = true;
	      }
	   }

	 /***** Remove user as an administrator of the institution *****/
	 if (UsrIsInsAdmin &&
	     (MeOrOther == Usr_ME || Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM))
	   {
	    Enr_PutActionRemUsrAsInsAdm (&OptionChecked,MeOrOther);
	    OptionsShown = true;
	   }
	}

      /***** Eliminate user completely from platform *****/
      if (Acc_CheckIfICanEliminateAccount (Gbl.Usrs.Other.UsrDat.UsrCod))
	{
	 Enr_PutActionRemUsrAcc (&OptionChecked,MeOrOther);
	 OptionsShown = true;
	}

   /***** End list of options *****/
   HTM_UL_End ();

   return OptionsShown;
  }

/*****************************************************************************/
/**************** Put action to modify user in current course ****************/
/*****************************************************************************/

static void Enr_PutActionModifyOneUsr (bool *OptionChecked,
                                       bool UsrBelongsToCrs,Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Register_me_in_X;
   extern const char *Txt_Register_USER_in_the_course_X;
   extern const char *Txt_Modify_me_in_the_course_X;
   extern const char *Txt_Modify_user_in_the_course_X;
   const char *Txt[2][Usr_NUM_ME_OR_OTHER] =
     {
      [false][Usr_ME   ] = Txt_Register_me_in_X,
      [false][Usr_OTHER] = Txt_Register_USER_in_the_course_X,
      [true ][Usr_ME   ] = Txt_Modify_me_in_the_course_X,
      [true ][Usr_OTHER] = Txt_Modify_user_in_the_course_X,
     };

   Enr_RegRemOneUsrActionBegin (Enr_REGISTER_MODIFY_ONE_USR_IN_CRS,OptionChecked);
      HTM_TxtF (Txt[UsrBelongsToCrs][MeOrOther],
		Gbl.Hierarchy.Node[Hie_CRS].ShrtName);
   Enr_RegRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/**************** Put action to register user as degree admin ****************/
/*****************************************************************************/

static void Enr_PutActionRegOneDegAdm (bool *OptionChecked)
  {
   extern const char *Txt_Register_USER_as_an_administrator_of_the_degree_X;

   Enr_RegRemOneUsrActionBegin (Enr_REGISTER_ONE_DEG_ADMIN,OptionChecked);
      HTM_TxtF (Txt_Register_USER_as_an_administrator_of_the_degree_X,
		Gbl.Hierarchy.Node[Hie_DEG].ShrtName);
   Enr_RegRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/**************** Put action to register user as center admin ****************/
/*****************************************************************************/

static void Enr_PutActionRegOneCtrAdm (bool *OptionChecked)
  {
   extern const char *Txt_Register_USER_as_an_administrator_of_the_center_X;

   Enr_RegRemOneUsrActionBegin (Enr_REGISTER_ONE_CTR_ADMIN,OptionChecked);
      HTM_TxtF (Txt_Register_USER_as_an_administrator_of_the_center_X,
		Gbl.Hierarchy.Node[Hie_CTR].ShrtName);
   Enr_RegRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/************* Put action to register user as institution admin **************/
/*****************************************************************************/

static void Enr_PutActionRegOneInsAdm (bool *OptionChecked)
  {
   extern const char *Txt_Register_USER_as_an_administrator_of_the_institution_X;

   Enr_RegRemOneUsrActionBegin (Enr_REGISTER_ONE_INS_ADMIN,OptionChecked);
      HTM_TxtF (Txt_Register_USER_as_an_administrator_of_the_institution_X,
		Gbl.Hierarchy.Node[Hie_INS].ShrtName);
   Enr_RegRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/****************** Put action to report user as duplicate *******************/
/*****************************************************************************/

static void Enr_PutActionRepUsrAsDup (bool *OptionChecked)
  {
   extern const char *Txt_Report_possible_duplicate_user;

   Enr_RegRemOneUsrActionBegin (Enr_REPORT_USR_AS_POSSIBLE_DUPLICATE,OptionChecked);
      HTM_Txt (Txt_Report_possible_duplicate_user);
   Enr_RegRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/****************** Put action to remove user from course ********************/
/*****************************************************************************/

static void Enr_PutActionRemUsrFromCrs (bool *OptionChecked,Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Remove_me_from_THE_COURSE_X;
   extern const char *Txt_Remove_USER_from_THE_COURSE_X;
   const char *Txt[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Remove_me_from_THE_COURSE_X,
      [Usr_OTHER] = Txt_Remove_USER_from_THE_COURSE_X,
     };

   Enr_RegRemOneUsrActionBegin (Enr_REMOVE_ONE_USR_FROM_CRS,OptionChecked);
      HTM_TxtF (Txt[MeOrOther],Gbl.Hierarchy.Node[Hie_CRS].ShrtName);
   Enr_RegRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/***************** Put action to remove user as degree admin *****************/
/*****************************************************************************/

static void Enr_PutActionRemUsrAsDegAdm (bool *OptionChecked,Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Remove_me_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Remove_USER_as_an_administrator_of_the_degree_X;
   const char *Txt[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Remove_me_as_an_administrator_of_the_degree_X,
      [Usr_OTHER] = Txt_Remove_USER_as_an_administrator_of_the_degree_X,
     };

   Enr_RegRemOneUsrActionBegin (Enr_REMOVE_ONE_DEG_ADMIN,OptionChecked);
      HTM_TxtF (Txt[MeOrOther],Gbl.Hierarchy.Node[Hie_DEG].ShrtName);
   Enr_RegRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/***************** Put action to remove user as center admin *****************/
/*****************************************************************************/

static void Enr_PutActionRemUsrAsCtrAdm (bool *OptionChecked,Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Remove_me_as_an_administrator_of_the_center_X;
   extern const char *Txt_Remove_USER_as_an_administrator_of_the_center_X;
   const char *Txt[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Remove_me_as_an_administrator_of_the_center_X,
      [Usr_OTHER] = Txt_Remove_USER_as_an_administrator_of_the_center_X,
     };

   Enr_RegRemOneUsrActionBegin (Enr_REMOVE_ONE_CTR_ADMIN,OptionChecked);
      HTM_TxtF (Txt[MeOrOther],Gbl.Hierarchy.Node[Hie_CTR].ShrtName);
   Enr_RegRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/************** Put action to remove user as institution admin ***************/
/*****************************************************************************/

static void Enr_PutActionRemUsrAsInsAdm (bool *OptionChecked,Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Remove_me_as_an_administrator_of_the_institution_X;
   extern const char *Txt_Remove_USER_as_an_administrator_of_the_institution_X;
   const char *Txt[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Remove_me_as_an_administrator_of_the_institution_X,
      [Usr_OTHER] = Txt_Remove_USER_as_an_administrator_of_the_institution_X,
     };

   Enr_RegRemOneUsrActionBegin (Enr_REMOVE_ONE_INS_ADMIN,OptionChecked);
      HTM_TxtF (Txt[MeOrOther],Gbl.Hierarchy.Node[Hie_INS].ShrtName);
   Enr_RegRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/********************* Put action to remove user account *********************/
/*****************************************************************************/

static void Enr_PutActionRemUsrAcc (bool *OptionChecked,Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Eliminate_my_user_account;
   extern const char *Txt_Eliminate_user_account;
   const char *Txt[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Eliminate_my_user_account,
      [Usr_OTHER] = Txt_Eliminate_user_account,
     };

   Enr_RegRemOneUsrActionBegin (Enr_ELIMINATE_ONE_USR_FROM_PLATFORM,OptionChecked);
      HTM_Txt (Txt[MeOrOther]);
   Enr_RegRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/************ Put start/end of action to register/remove one user ************/
/*****************************************************************************/

static void Enr_RegRemOneUsrActionBegin (Enr_RegRemOneUsrAction_t RegRemOneUsrAction,
                                         bool *OptionChecked)
  {
   HTM_LI_Begin (NULL);
      HTM_LABEL_Begin (NULL);
	 HTM_INPUT_RADIO ("RegRemAction",HTM_DONT_SUBMIT_ON_CLICK,
			  "value=\"%u\"%s",
			  (unsigned) RegRemOneUsrAction,
			  *OptionChecked ? "" : " checked=\"checked\"");

	    if (!*OptionChecked)
	       *OptionChecked = true;
  }

static void Enr_RegRemOneUsrActionEnd (void)
  {
      HTM_LABEL_End ();
   HTM_LI_End ();
  }

/*****************************************************************************/
/********************** Register a user using his/her ID *********************/
/*****************************************************************************/
// If user does not exists, UsrDat->IDs must hold the user's ID

static void Enr_RegisterUsr (struct Usr_Data *UsrDat,Rol_Role_t RegRemRole,
                             struct ListCodGrps *LstGrps,unsigned *NumUsrsRegistered)
  {
   /***** Check if I can register this user *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_TCH &&
       RegRemRole != Rol_STD)
      Err_ShowErrorAndExit ("A teacher only can register several users as students.");

   /***** Check if the record of the user exists and get the type of user *****/
   if (UsrDat->UsrCod > 0)	// User exists in database
      /* Get user's data */
      Usr_GetAllUsrDataFromUsrCod (UsrDat,
                                   Usr_DONT_GET_PREFS,
                                   Usr_GET_ROLE_IN_CURRENT_CRS);
   else				// User does not exist in database, create it using his/her ID!
     {
      /* Reset user's data */
      Usr_ResetUsrDataExceptUsrCodAndIDs (UsrDat);	// It's necessary, because the same struct UsrDat was used for former user

      /* User does not exist in database; list of IDs is initialized */
      UsrDat->IDs.List[0].Confirmed = true;	// If he/she is a new user ==> his/her ID will be stored as confirmed in database
      Acc_CreateNewUsr (UsrDat,Usr_OTHER);
     }

   /***** Register user in current course in database *****/
   if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
     {
      if (Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat))
	{
	 if (RegRemRole != UsrDat->Roles.InCurrentCrs)	// The role must be updated
	    /* Modify role */
	    Enr_ModifyRoleInCurrentCrs (UsrDat,RegRemRole);
	}
      else        // User does not belong to this course
	 /* Register user */
	 Enr_RegisterUsrInCurrentCrs (UsrDat,RegRemRole,
	                              Enr_SET_ACCEPTED_TO_FALSE);

      /***** Register user in the selected groups *****/
      if (Gbl.Crs.Grps.NumGrps)	// If there are groups in the course
	 Grp_RegisterUsrIntoGroups (UsrDat,LstGrps);
     }

   (*NumUsrsRegistered)++;
  }

/*****************************************************************************/
/****** Put a link (form) to remove all students in the current course *******/
/*****************************************************************************/

static void Enr_PutLinkToRemAllStdsThisCrs (void)
  {
   extern const char *Txt_Remove_all_students;

   /***** Put form to remove all students in the current course *****/
   Lay_PutContextualLinkIconText (ActReqRemAllStdCrs,NULL,
                                  NULL,NULL,
				  "trash.svg",Ico_RED,
				  Txt_Remove_all_students,NULL);
  }

/*****************************************************************************/
/************ Ask for removing all students from current course **************/
/*****************************************************************************/

void Enr_AskRemAllStdsThisCrs (void)
  {
   extern const char *Hlp_USERS_Administration_remove_all_students;
   extern const char *Txt_Remove_all_students;
   extern const char *Txt_Do_you_really_want_to_remove_the_X_students_from_the_course_Y_;
   unsigned NumStds = Enr_GetNumUsrsInCrss (Hie_CRS,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				            1 << Rol_STD);	// This course has students

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Remove_all_students,
                 NULL,NULL,
                 Hlp_USERS_Administration_remove_all_students,Box_NOT_CLOSABLE);

      if (NumStds)
	{
	 /***** Show question and button to remove students *****/
	 /* Begin alert */
	 Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_X_students_from_the_course_Y_,
				  NumStds,
				  Gbl.Hierarchy.Node[Hie_CRS].FullName);

	 /* Show form to request confirmation */
	 Frm_BeginForm (ActRemAllStdCrs);
	    Grp_PutParAllGroups ();
	    Pwd_AskForConfirmationOnDangerousAction ();
	    Btn_PutRemoveButton (Txt_Remove_all_students);
	 Frm_EndForm ();

	 /* End alert */
	 Ale_ShowAlertAndButton2 (ActUnk,NULL,NULL,
				  NULL,NULL,
				  Btn_NO_BUTTON,NULL);
	}
      else	// Gbl.Hierarchy.Node[Hie_CRS].NumUsrs[Rol_STD] == 0
	 /***** Show warning indicating no students found *****/
	 Usr_ShowWarningNoUsersFound (Rol_STD);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************* Remove all students from current course *****************/
/*****************************************************************************/

void Enr_RemAllStdsThisCrs (void)
  {
   extern const char *Txt_The_X_students_who_belonged_to_the_course_Y_have_been_removed_from_it;
   unsigned NumStdsInCrs;

   if (Pwd_GetConfirmationOnDangerousAction ())
     {
      if ((NumStdsInCrs = Enr_RemAllStdsInCrs (&Gbl.Hierarchy.Node[Hie_CRS])))
	 Ale_ShowAlert (Ale_SUCCESS,Txt_The_X_students_who_belonged_to_the_course_Y_have_been_removed_from_it,
		        NumStdsInCrs,Gbl.Hierarchy.Node[Hie_CRS].FullName);
      else	// NumStdsInCrs == 0
	 /***** Show warning indicating no students found *****/
	 Usr_ShowWarningNoUsersFound (Rol_STD);
     }
  }

/*****************************************************************************/
/******************* Remove all students from a given course *****************/
/*****************************************************************************/
// Returns the numbers of students in list

unsigned Enr_RemAllStdsInCrs (struct Hie_Node *Crs)
  {
   unsigned NumStdsInCrs;
   unsigned NumUsr;

   /***** Get list of students in current course *****/
   Gbl.Usrs.ClassPhoto.AllGroups = true;        // Get all students of the current course
   Usr_GetListUsrs (Hie_CRS,Rol_STD);
   NumStdsInCrs = Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs;

   /***** Remove all students *****/
   for (NumUsr = 0;
	NumUsr < NumStdsInCrs;
	NumUsr++)
     {
      Gbl.Usrs.Other.UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].UsrCod;
      Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs = Gbl.Usrs.LstUsrs[Rol_STD].Lst[NumUsr].RoleInCurrentCrsDB;
      Enr_EffectivelyRemUsrFromCrs (&Gbl.Usrs.Other.UsrDat,Crs,
				    Enr_REMOVE_USR_PRODUCTION,
				    Cns_QUIET);
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
   if (Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs >= Rol_STD)
      Ale_ShowAlert (Ale_WARNING,Txt_You_were_already_enroled_as_X_in_the_course_Y,
                     Txt_ROLES_SINGUL_abc[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs][Gbl.Usrs.Me.UsrDat.Sex],
                     Gbl.Hierarchy.Node[Hie_CRS].FullName);
   else if (Gbl.Usrs.Me.Role.Logged == Rol_GST ||
	    Gbl.Usrs.Me.Role.Logged == Rol_USR)
      /***** Show form to modify only the user's role or the user's data *****/
      Rec_ShowFormSignUpInCrsWithMySharedRecord ();
   else
      Err_ShowErrorAndExit ("You must be logged to sign up in a course.");        // This never should happen
  }

/*****************************************************************************/
/*********************** Sign up in the current course ***********************/
/*****************************************************************************/

void Enr_SignUpInCrs (void)
  {
   extern const char *Txt_You_were_already_enroled_as_X_in_the_course_Y;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Your_request_for_enrolment_as_X_in_the_course_Y_has_been_accepted_for_processing;
   Rol_Role_t RoleFromForm;
   bool Notify;
   long ReqCod = -1L;

   /***** Check if I already belong to course *****/
   if (Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs >= Rol_STD)
      Ale_ShowAlert (Ale_WARNING,Txt_You_were_already_enroled_as_X_in_the_course_Y,
                     Txt_ROLES_SINGUL_abc[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs][Gbl.Usrs.Me.UsrDat.Sex],
                     Gbl.Hierarchy.Node[Hie_CRS].FullName);
   else
     {
      /***** Get new role from record form *****/
      RoleFromForm = (Rol_Role_t)
	             Par_GetParUnsignedLong ("Role",
                                             0,
                                             Rol_NUM_ROLES - 1,
                                             (unsigned long) Rol_UNK);

      /* Check if role is correct */
      if (!(RoleFromForm == Rol_STD ||
	    RoleFromForm == Rol_NET ||
            RoleFromForm == Rol_TCH))
         Err_WrongRoleExit ();

      /***** Try to get and old request of the same user (me) in the current course *****/
      ReqCod = Enr_DB_GetUsrEnrolmentRequestInCrs (Gbl.Usrs.Me.UsrDat.UsrCod,
                                                   Gbl.Hierarchy.Node[Hie_CRS].HieCod);

      /***** Request user in current course in database *****/
      if (ReqCod > 0)	// Old request exists in database
	 Enr_DB_UpdateMyEnrolmentRequestInCurrentCrs (ReqCod,RoleFromForm);
      else		// No request in database for this user in this course
         ReqCod = Enr_DB_CreateMyEnrolmentRequestInCurrentCrs (RoleFromForm);

      /***** Show confirmation message *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_Your_request_for_enrolment_as_X_in_the_course_Y_has_been_accepted_for_processing,
                     Txt_ROLES_SINGUL_abc[RoleFromForm][Gbl.Usrs.Me.UsrDat.Sex],
                     Gbl.Hierarchy.Node[Hie_CRS].FullName);

      /***** Notify teachers or admins by email about the new enrolment request *****/
      // If I want to be a teacher ==> send notification to other teachers, administrators or superusers
      // If this course has teachers ==> send notification to teachers
      if (RoleFromForm == Rol_TCH)
         Notify = true;
      else
	 Notify = (Enr_GetNumUsrsInCrss (Hie_CRS,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				         1 << Rol_TCH) != 0);	// This course has teachers
      if (Notify)
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
   struct Usr_Data UsrDat;
   Rol_Role_t DesiredRole;

   SummaryStr[0] = '\0';        // Return nothing on error

   /***** Get user and requested role from database *****/
   if (Enr_DB_GetEnrolmentRequestByCod (&mysql_res,ReqCod) == 1)	// Result should have a unique row
     {
      /***** Get user and requested role *****/
      row = mysql_fetch_row (mysql_res);

      /* Initialize structure with user's data */
      Usr_UsrDataConstructor (&UsrDat);

      /* User's code (row[0]) */
      UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
                                               Usr_DONT_GET_PREFS,
                                               Usr_DONT_GET_ROLE_IN_CURRENT_CRS);

      /* Role (row[1]) */
      DesiredRole = Rol_ConvertUnsignedStrToRole (row[1]);
      Str_Copy (SummaryStr,Txt_ROLES_SINGUL_Abc[DesiredRole][UsrDat.Sex],
		Ntf_MAX_BYTES_SUMMARY);

      if (GetContent)
	 /* Write desired role into content */
	 if (asprintf (ContentStr,
		       "%s",	// TODO: Write more info in this content
		       Txt_ROLES_SINGUL_Abc[DesiredRole][UsrDat.Sex]) < 0)
	    Err_NotEnoughMemoryExit ();

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

   Rol_Role_t Role;

   /***** Get user's code *****/
   Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();

   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                Usr_DONT_GET_PREFS,
                                                Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
     {
      // User's data exist...
      if (Enr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
        {
         /* User already belongs to this course */
         Ale_ShowAlert (Ale_WARNING,Txt_THE_USER_X_is_already_enroled_in_the_course_Y,
                        Gbl.Usrs.Other.UsrDat.FullName,
                        Gbl.Hierarchy.Node[Hie_CRS].FullName);
         Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

         /* Remove inscription request because it has not sense */
         Enr_RemUsrEnrolmentRequestInCrs (Gbl.Usrs.Other.UsrDat.UsrCod,
					  Gbl.Hierarchy.Node[Hie_CRS].HieCod);
        }
      else        // User does not belong to this course
        {
         Role = Rol_DB_GetRequestedRole (Gbl.Hierarchy.Node[Hie_CRS].HieCod,
                                         Gbl.Usrs.Other.UsrDat.UsrCod);
         if (Role == Rol_STD ||
             Role == Rol_NET ||
             Role == Rol_TCH)
           {
	    /***** Show question and button to reject user's enrolment request *****/
	    /* Begin alert */
	    Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_reject_the_enrolment_request_,
				     Gbl.Usrs.Other.UsrDat.FullName,
				     Txt_ROLES_SINGUL_abc[Role][Gbl.Usrs.Other.UsrDat.Sex],
				     Gbl.Hierarchy.Node[Hie_CRS].FullName);

	    /* Show user's record */
            Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

	    /* End alert */
	    Ale_ShowAlertAndButton2 (ActRejSignUp,NULL,NULL,
	                             Usr_PutParOtherUsrCodEncrypted,Gbl.Usrs.Other.UsrDat.EnUsrCod,
				     Btn_REMOVE_BUTTON,Txt_Reject);
           }
         else
            Err_WrongRoleExit ();
        }
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/********* Reject the request for enrolment of a user in a course ***********/
/*****************************************************************************/

void Enr_RejectSignUp (void)
  {
   extern const char *Txt_THE_USER_X_is_already_enroled_in_the_course_Y;
   extern const char *Txt_Enrolment_of_X_rejected;

   /***** Get user's code *****/
   Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();

   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                Usr_DONT_GET_PREFS,
                                                Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
     {
      // User's data exist...
      if (Enr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
        {
         /* User already belongs to this course */
         Ale_ShowAlert (Ale_WARNING,Txt_THE_USER_X_is_already_enroled_in_the_course_Y,
                        Gbl.Usrs.Other.UsrDat.FullName,
                        Gbl.Hierarchy.Node[Hie_CRS].FullName);
         Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
        }

      /* Remove inscription request */
      Enr_RemUsrEnrolmentRequestInCrs (Gbl.Usrs.Other.UsrDat.UsrCod,
				       Gbl.Hierarchy.Node[Hie_CRS].HieCod);

      /* Confirmation message */
      Ale_ShowAlert (Ale_SUCCESS,Txt_Enrolment_of_X_rejected,
                     Gbl.Usrs.Other.UsrDat.FullName);
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();

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
	 Err_NoPermissionExit ();
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
   extern const char *Txt_Enrolment_requests;
   extern const char *Txt_Scope;
   extern const char *Txt_Users;
   extern const char *Txt_Course;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Requester;
   extern const char *Txt_Role;
   extern const char *Txt_Date;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Register;
   extern const char *Txt_Reject;
   extern const char *Txt_No_enrolment_requests;
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumReqs;
   unsigned NumReq;
   long ReqCod;
   struct Hie_Node Deg;
   struct Hie_Node Crs;
   struct Usr_Data UsrDat;
   bool UsrExists;
   bool UsrBelongsToCrs;
   Rol_Role_t DesiredRole;
   static const Act_Action_t NextAction[Rol_NUM_ROLES] =
     {
      [Rol_STD] = ActReqMdfStd,
      [Rol_NET] = ActReqMdfNET,
      [Rol_TCH] = ActReqMdfTch,
     };

   /***** Remove expired enrolment requests *****/
   Enr_DB_RemoveExpiredEnrolmentRequests ();

   /***** Get scope *****/
   Gbl.Scope.Allowed = 1 << Hie_SYS |
	               1 << Hie_CTY |
                       1 << Hie_INS |
                       1 << Hie_CTR |
                       1 << Hie_DEG |
                       1 << Hie_CRS;
   Gbl.Scope.Default = Hie_CRS;
   Sco_GetScope ("ScopeEnr");

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Enrolment_requests,
                 NULL,NULL,
                 Hlp_USERS_Requests,Box_NOT_CLOSABLE);

      /***** Selection of scope and roles *****/
      /* Begin form and table */
      Frm_BeginForm (ActUpdSignUpReq);
	 HTM_TABLE_BeginWideMarginPadding (2);

	    /* Scope (whole platform, current center, current degree or current course) */
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("RT","ScopeEnr",Txt_Scope);

	       /* Data */
	       HTM_TD_Begin ("class=\"LM\"");
		  Sco_PutSelectorScope ("ScopeEnr",HTM_SUBMIT_ON_CHANGE);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /* Users' roles in listing */
	    HTM_TR_Begin (NULL);

	       Frm_LabelColumn ("RT","Role",Txt_Users);

	       HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
		  Rol_WriteSelectorRoles (1 << Rol_STD |
					  1 << Rol_NET |
					  1 << Rol_TCH,
					  RolesSelected,
					  false,true);
	       HTM_TD_End ();

	    HTM_TR_End ();

	 /* End table and form */
	 HTM_TABLE_End ();
      Frm_EndForm ();

      /***** List requests *****/
      if ((NumReqs = Enr_DB_GetEnrolmentRequests (&mysql_res,RolesSelected)))
	 {
	 /* Initialize structure with user's data */
	 Usr_UsrDataConstructor (&UsrDat);

	 /* Begin table */
	 HTM_TABLE_BeginCenterPadding (2);

	    /* Table heading */
	    HTM_TR_Begin (NULL);
	       HTM_TH_Empty (1);
	       HTM_TH      (Txt_Course                         ,HTM_HEAD_LEFT  );
	       HTM_TH      (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH],HTM_HEAD_RIGHT );
	       HTM_TH_Span (Txt_Requester                      ,HTM_HEAD_LEFT  ,1,2,NULL);
	       HTM_TH      (Txt_Role                           ,HTM_HEAD_LEFT  );
	       HTM_TH      (Txt_Date                           ,HTM_HEAD_CENTER);
	       HTM_TH_Empty (2);
	    HTM_TR_End ();

	    /* List requests */
	    for (NumReq = 0;
		 NumReq < NumReqs;
		 NumReq++)
	      {
	       row = mysql_fetch_row (mysql_res);

	       /* Get request code (row[0]) */
	       ReqCod = Str_ConvertStrCodToLongCod (row[0]);

	       /* Get course code (row[1]) */
	       Crs.HieCod = Str_ConvertStrCodToLongCod (row[1]);

	       /* Get user code (row[2]) */
	       UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[2]);
	       UsrExists = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
								    Usr_DONT_GET_PREFS,
								    Usr_DONT_GET_ROLE_IN_CURRENT_CRS);

	       /***** Get requested role (row[3]) *****/
	       DesiredRole = Rol_ConvertUnsignedStrToRole (row[3]);

	       if (UsrExists)
		  UsrBelongsToCrs = Hie_CheckIfUsrBelongsTo (Hie_CRS,
							     UsrDat.UsrCod,
							     Crs.HieCod,
							     false);
	       else
		  UsrBelongsToCrs = false;

	       if (UsrExists &&
		   !UsrBelongsToCrs &&
		   (DesiredRole == Rol_STD ||
		    DesiredRole == Rol_NET ||
		    DesiredRole == Rol_TCH))
		 {
		  HTM_TR_Begin (NULL);

		     /***** Number *****/
		     HTM_TD_Begin ("class=\"RT DAT_%s\"",
		                   The_GetSuffix ());
			HTM_Unsigned (NumReqs - NumReq);
		     HTM_TD_End ();

		     /***** Link to course *****/
		     HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());

			Crs_GetCourseDataByCod (&Crs);
			Deg.HieCod = Crs.PrtCod;
			Deg_GetDegreeDataByCod (&Deg);

			Frm_BeginFormGoTo (ActSeeCrsInf);
			   ParCod_PutPar (ParCod_Crs,Crs.HieCod);
			   HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Crs.FullName),
						    "class=\"LT BT_LINK\"");
			   Str_FreeGoToTitle ();
			      HTM_TxtF ("%s &gt; %s",Deg.ShrtName,Crs.ShrtName);
			   HTM_BUTTON_End ();
			Frm_EndForm ();

		     HTM_TD_End ();

		     /***** Number of teachers in the course *****/
		     HTM_TD_Begin ("class=\"RT DAT_%s\"",
		                   The_GetSuffix ());
			HTM_Unsigned (Enr_GetNumUsrsInCrss (Hie_CRS,Crs.HieCod,
							    1 << Rol_TCH));
		     HTM_TD_End ();

		     /***** User photo *****/
		     HTM_TD_Begin ("class=\"CT DAT_%s\" style=\"width:22px;\"",
		                   The_GetSuffix ());
			Pho_ShowUsrPhotoIfAllowed (&UsrDat,
			                           ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
		     HTM_TD_End ();

		     /***** User name *****/
		     HTM_TD_Begin ("class=\"LT DAT_%s\"",
		                   The_GetSuffix ());
			HTM_DIV_Begin ("class=\"REQUESTER_NAME\"");	// Limited width
			   Usr_WriteFirstNameBRSurnames (&UsrDat);
			HTM_DIV_End ();
		     HTM_TD_End ();

		     /***** Requested role (row[3]) *****/
		     HTM_TD_Begin ("class=\"LT DAT_%s\"",
		                   The_GetSuffix ());
			HTM_Txt (Txt_ROLES_SINGUL_abc[DesiredRole][UsrDat.Sex]);
		     HTM_TD_End ();

		     /***** Request time (row[4]) *****/
		     Msg_WriteMsgDate (Dat_GetUNIXTimeFromStr (row[4]),
		                       "DAT",The_GetColorRows ());

		     /***** Button to confirm the request *****/
		     HTM_TD_Begin ("class=\"LT DAT_%s\"",
		                   The_GetSuffix ());
			if (!NextAction[DesiredRole])
			   Err_WrongRoleExit ();
			Frm_BeginForm (NextAction[DesiredRole]);
			   ParCod_PutPar (ParCod_Crs,Crs.HieCod);
			   Usr_PutParUsrCodEncrypted (UsrDat.EnUsrCod);
			   Btn_PutCreateButtonInline (Txt_Register);
			Frm_EndForm ();
		     HTM_TD_End ();

		     /***** Button to reject the request *****/
		     HTM_TD_Begin ("class=\"LT DAT_%s\"",
		                   The_GetSuffix ());
			Frm_BeginForm (ActReqRejSignUp);
			   ParCod_PutPar (ParCod_Crs,Crs.HieCod);
			   Usr_PutParUsrCodEncrypted (UsrDat.EnUsrCod);
			   Btn_PutRemoveButtonInline (Txt_Reject);
			Frm_EndForm ();
		     HTM_TD_End ();

		  HTM_TR_End ();

		  /***** Mark possible notification as seen *****/
		  Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_ENROLMENT_REQUEST,
		                                  ReqCod);
		 }
	       else        // User does not exists or user already belongs to course ==> remove pair from crs_requests table
		  Enr_RemUsrEnrolmentRequestInCrs (UsrDat.UsrCod,Crs.HieCod);
	      }

	 /* End table */
	 HTM_TABLE_End ();

	 /* Free memory used for user's data */
	 Usr_UsrDataDestructor (&UsrDat);
	}
      else	// There are no requests
	 Ale_ShowAlert (Ale_INFO,Txt_No_enrolment_requests);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* Remove a request for enrolment ***********************/
/*****************************************************************************/

static void Enr_RemUsrEnrolmentRequestInCrs (long UsrCod,long CrsCod)
  {
   long ReqCod;

   /***** Get request code *****/
   ReqCod = Enr_DB_GetUsrEnrolmentRequestInCrs (UsrCod,CrsCod);

   if (ReqCod > 0)
     {
      /***** Mark possible notifications as removed
	     Important: do this before removing the request *****/
      Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_ENROLMENT_REQUEST,ReqCod);

      /***** Remove enrolment request *****/
      Enr_DB_RemRequest (ReqCod);
     }
  }

/*****************************************************************************/
/********************** Write a form to admin one user ***********************/
/*****************************************************************************/

void Enr_PutLinkToAdminOneUsr (Act_Action_t NextAction)
  {
   extern const bool Adm_ICanAdminOtherUsrs[Rol_NUM_ROLES];
   extern const char *Txt_Administer_me;
   extern const char *Txt_Administer_one_user;
   const char *TitleText = Adm_ICanAdminOtherUsrs[Gbl.Usrs.Me.Role.Logged] ? Txt_Administer_one_user :
                        	                                             Txt_Administer_me;

   Lay_PutContextualLinkIconText (NextAction,NULL,
                                  NULL,NULL,
				  "user-cog.svg",Ico_BLACK,
				  TitleText,NULL);
  }

/*****************************************************************************/
/******************* Write a form to admin several users *********************/
/*****************************************************************************/

void Enr_PutLinkToAdminSeveralUsrs (Rol_Role_t Role)
  {
   extern const char *Txt_Administer_multiple_students;
   extern const char *Txt_Administer_multiple_non_editing_teachers;
   extern const char *Txt_Administer_multiple_teachers;
   static const struct
     {
      Act_Action_t NextAction;
      const char **Title;
     } WhatToDo[Rol_NUM_ROLES] =
     {
      [Rol_STD] = {ActReqEnrSevStd,&Txt_Administer_multiple_students            },
      [Rol_NET] = {ActReqEnrSevNET,&Txt_Administer_multiple_non_editing_teachers},
      [Rol_TCH] = {ActReqEnrSevTch,&Txt_Administer_multiple_teachers            },
     };

   if (!WhatToDo[Role].NextAction)
      Err_WrongRoleExit ();
   Lay_PutContextualLinkIconText (WhatToDo[Role].NextAction,NULL,
                                  NULL,NULL,
				  "users-cog.svg",Ico_BLACK,
				  *WhatToDo[Role].Title,NULL);
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
   extern const bool Adm_ICanAdminOtherUsrs[Rol_NUM_ROLES];

   /***** Contextual menu *****/
   if (Adm_ICanAdminOtherUsrs[Gbl.Usrs.Me.Role.Logged])
     {
      Mnu_ContextMenuBegin ();
	 Enr_PutLinkToAdminSeveralUsrs (Rol_STD);	// Admin several students
      Mnu_ContextMenuEnd ();
     }

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
   extern const bool Adm_ICanAdminOtherUsrs[Rol_NUM_ROLES];

   if (Adm_ICanAdminOtherUsrs[Gbl.Usrs.Me.Role.Logged])
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
   static const Act_Action_t NextAction[Rol_NUM_ROLES] =
     {
      [Rol_GST] = ActReqMdfOth,
      [Rol_STD] = ActReqMdfStd,
      [Rol_NET] = ActReqMdfNET,
      [Rol_TCH] = ActReqMdfTch,
     };

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Administer_one_user,
                 NULL,NULL,
                 Hlp_USERS_Administration_administer_one_user,Box_NOT_CLOSABLE);

      /***** Write form to request another user's ID *****/
      if (!NextAction[Role])
	 Err_WrongRoleExit ();
      Enr_WriteFormToReqAnotherUsrID (NextAction[Role],NULL);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************** Ask me for register/remove me ************************/
/*****************************************************************************/

static void Enr_AskIfRegRemMe (Rol_Role_t Role)
  {
   struct Usr_ListUsrCods ListUsrCods;

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
   struct Usr_ListUsrCods ListUsrCods;

   /***** Check if UsrCod is present in parameters *****/
   Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();
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
      Usr_GetParOtherUsrIDNickOrEMailAndGetUsrCods (&ListUsrCods);

   Enr_AskIfRegRemUsr (&ListUsrCods,Role);
  }

/*****************************************************************************/
/********************** Ask me for register/remove user **********************/
/*****************************************************************************/

static void Enr_AskIfRegRemUsr (struct Usr_ListUsrCods *ListUsrCods,Rol_Role_t Role)
  {
   extern const char *Txt_There_are_X_users_with_the_ID_Y;
   extern const char *Txt_The_user_is_new_not_yet_in_X;
   extern const char *Txt_If_this_is_a_new_user_in_X_you_should_indicate_her_his_ID;
   unsigned NumUsr;
   bool NewUsrIDValid;

   if (ListUsrCods->NumUsrs)	// User(s) found with the ID
     {
      /***** Warning if more than one user found *****/
      if (ListUsrCods->NumUsrs > 1)
	 Ale_ShowAlert (Ale_INFO,Txt_There_are_X_users_with_the_ID_Y,
		        ListUsrCods->NumUsrs,Gbl.Usrs.Other.UsrDat.UsrIDNickOrEmail);

      /***** For each user found... *****/
      for (NumUsr = 0;
	   NumUsr < ListUsrCods->NumUsrs;
	   NumUsr++)
	{
	 /* Get user's data */
         Gbl.Usrs.Other.UsrDat.UsrCod = ListUsrCods->Lst[NumUsr];
         Usr_GetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                      Usr_DONT_GET_PREFS,
                                      Usr_GET_ROLE_IN_CURRENT_CRS);

         /* Show form to edit user */
	 Enr_ShowFormToEditOtherUsr ();
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
	 Ale_ShowAlert (Ale_INFO,Txt_The_user_is_new_not_yet_in_X,
		        Cfg_PLATFORM_SHORT_NAME);
	 Rec_ShowFormOtherNewSharedRecord (&Gbl.Usrs.Other.UsrDat,Role);
	}
      else        // User's ID is not valid
	{
	 /* Write message and request a new user's ID */
	 Ale_ShowAlert (Ale_WARNING,Txt_If_this_is_a_new_user_in_X_you_should_indicate_her_his_ID,
		        Cfg_PLATFORM_SHORT_NAME);
	 Enr_ReqRegRemUsr (Role);
	}
     }
  }

/*****************************************************************************/
/********* Show other existing user's shared record to be edited *************/
/*****************************************************************************/

static void Enr_ShowFormToEditOtherUsr (void)
  {
   extern const char *Txt_THE_USER_X_is_already_enroled_in_the_course_Y;
   extern const char *Txt_THE_USER_X_is_in_the_course_Y_but_has_not_yet_accepted_the_enrolment;
   extern const char *Txt_THE_USER_X_exists_in_Y_but_is_not_enroled_in_the_course_Z;
   extern const char *Txt_THE_USER_X_already_exists_in_Y;

   /***** If user exists... *****/
   if (Usr_DB_ChkIfUsrCodExists (Gbl.Usrs.Other.UsrDat.UsrCod))
     {
      /***** Show form to edit user *****/
      if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
	{
	 /* Check if this user belongs to the current course */
	 if (Enr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
	   {
	    Gbl.Usrs.Other.UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	    if (Gbl.Usrs.Other.UsrDat.Accepted)
	       Ale_ShowAlert (Ale_INFO,Txt_THE_USER_X_is_already_enroled_in_the_course_Y,
			      Gbl.Usrs.Other.UsrDat.FullName,
			      Gbl.Hierarchy.Node[Hie_CRS].FullName);
	    else        // Enrolment not yet accepted
	       Ale_ShowAlert (Ale_INFO,Txt_THE_USER_X_is_in_the_course_Y_but_has_not_yet_accepted_the_enrolment,
			      Gbl.Usrs.Other.UsrDat.FullName,
			      Gbl.Hierarchy.Node[Hie_CRS].FullName);

	    Rec_ShowOtherSharedRecordEditable ();
	   }
	 else        // User does not belong to the current course
	   {
	    Ale_ShowAlert (Ale_INFO,Txt_THE_USER_X_exists_in_Y_but_is_not_enroled_in_the_course_Z,
			   Gbl.Usrs.Other.UsrDat.FullName,
			   Cfg_PLATFORM_SHORT_NAME,
			   Gbl.Hierarchy.Node[Hie_CRS].FullName);

	    Rec_ShowOtherSharedRecordEditable ();
	   }
	}
      else	// No course selected
	{
	 Ale_ShowAlert (Ale_INFO,Txt_THE_USER_X_already_exists_in_Y,
			Gbl.Usrs.Other.UsrDat.FullName,Cfg_PLATFORM_SHORT_NAME);

	 Rec_ShowOtherSharedRecordEditable ();
	}
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
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
   /***** Get user to be removed *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Enr_CheckIfICanRemUsrFromCrs ())
	 Enr_AskIfRemoveUsrFromCrs (&Gbl.Usrs.Other.UsrDat);
      else
         Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/********************* Remove a user from current course *********************/
/*****************************************************************************/

void Enr_RemUsrFromCrs1 (void)
  {
   if (Pwd_GetConfirmationOnDangerousAction ())
     {
      /***** Get user to be removed *****/
      if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
	{
	 if (Enr_CheckIfICanRemUsrFromCrs ())
	    Enr_EffectivelyRemUsrFromCrs (&Gbl.Usrs.Other.UsrDat,&Gbl.Hierarchy.Node[Hie_CRS],
					  Enr_DO_NOT_REMOVE_USR_PRODUCTION,
					  Cns_VERBOSE);
	 else
	    Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	}
      else
	 Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
     }
  }

void Enr_RemUsrFromCrs2 (void)
  {
   /* Show possible alerts */
   Ale_ShowAlerts (NULL);

   /* Show form to edit user again */
   Enr_ShowFormToEditOtherUsr ();
  }

/*****************************************************************************/
/*********** Check if I can remove another user in current course ************/
/*****************************************************************************/

static bool Enr_CheckIfICanRemUsrFromCrs (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
	 // A student or non-editing teacher can remove herself/himself
	 return Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod) == Usr_ME;
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 // A teacher or administrator can remove anyone
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/*************** Accept my enrolment in the current course ******************/
/*****************************************************************************/

void Enr_AcceptRegisterMeInCrs (void)
  {
   extern const char *Txt_You_have_confirmed_your_enrolment_in_the_course_X;

   /***** Confirm my enrolment *****/
   Enr_DB_AcceptUsrInCrs (Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   /***** Mark all notifications about enrolment (as student or as teacher)
          in current course as removed *****/
   Ntf_DB_MarkNotifToOneUsrAsRemoved (Ntf_EVENT_ENROLMENT_STD,-1L,
                                  Gbl.Usrs.Me.UsrDat.UsrCod);
   Ntf_DB_MarkNotifToOneUsrAsRemoved (Ntf_EVENT_ENROLMENT_TCH,-1L,
                                  Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Confirmation message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_You_have_confirmed_your_enrolment_in_the_course_X,
                  Gbl.Hierarchy.Node[Hie_CRS].FullName);
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
   static const Act_Action_t Action[Rol_NUM_ROLES] =
     {
      [Rol_GST] = ActCreOth,
      [Rol_STD] = ActCreStd,
      [Rol_NET] = ActCreNET,
      [Rol_TCH] = ActCreTch,
     };

   /***** Get user's ID from form *****/
   ID_GetParOtherUsrIDPlain ();	// User's ID was already modified and passed as a hidden parameter

   if (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID))        // User's ID valid
     {
      Gbl.Usrs.Other.UsrDat.UsrCod = -1L;

      /***** Get new role *****/
      NewRole = Rec_GetRoleFromRecordForm ();

      /***** Get user's name from form *****/
      Rec_GetUsrNameFromRecordForm (&Gbl.Usrs.Other.UsrDat);

      /***** Create user *****/
      Gbl.Usrs.Other.UsrDat.IDs.List[0].Confirmed = true;	// User's ID will be stored as confirmed
      Acc_CreateNewUsr (&Gbl.Usrs.Other.UsrDat,Usr_OTHER);

      /***** Register user in current course in database *****/
      if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
	{
	 if (Enr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
	   {
	    OldRole = Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs;	// Remember old role before changing it
	    if (NewRole != OldRole)	// The role must be updated
	      {
	       /* Modify role */
	       Enr_ModifyRoleInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole);

	       /* Success message */
               Ale_CreateAlert (Ale_SUCCESS,NULL,
        	                Txt_The_role_of_THE_USER_X_in_the_course_Y_has_changed_from_A_to_B,
				Gbl.Usrs.Other.UsrDat.FullName,
				Gbl.Hierarchy.Node[Hie_CRS].FullName,
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
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_THE_USER_X_has_been_enroled_in_the_course_Y,
		             Gbl.Usrs.Other.UsrDat.FullName,
		             Gbl.Hierarchy.Node[Hie_CRS].FullName);
	   }

	 /***** Change user's groups *****/
	 if (Gbl.Crs.Grps.NumGrps)	// This course has groups?
	    Grp_ChangeOtherUsrGrps ();
	}

      /***** Change current action *****/
      if (!Action[NewRole])
	 Err_WrongRoleExit ();
      Gbl.Action.Act = Action[NewRole];
      Tab_SetCurrentTab ();
     }
   else        // User's ID not valid
      /***** Error message *****/
      Ale_CreateAlert (Ale_ERROR,NULL,
	               Txt_The_ID_X_is_not_valid,
                       Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);
  }

void Enr_CreateNewUsr2 (void)
  {
   if (Ale_GetTypeOfLastAlert () == Ale_ERROR)	// User's ID not valid
      Ale_ShowAlerts (NULL);
   else						// User's ID valid
     {
      /***** Show possible alerts *****/
      Ale_ShowAlerts (NULL);

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
   Usr_MeOrOther_t MeOrOther;
   Rol_Role_t OldRole;
   Rol_Role_t NewRole;
   static const Act_Action_t Action[Rol_NUM_ROLES] =
     {
      [Rol_GST] = ActUpdOth,
      [Rol_STD] = ActUpdStd,
      [Rol_NET] = ActUpdNET,
      [Rol_TCH] = ActUpdTch,
     };

   /***** Get user from form *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
     {
      MeOrOther = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);

      /***** Get the action to do *****/
      Gbl.Usrs.RegRemAction = (Enr_RegRemOneUsrAction_t)
	                      Par_GetParUnsignedLong ("RegRemAction",
                                                      0,
                                                      Enr_REG_REM_ONE_USR_NUM_ACTIONS - 1,
                                                      (unsigned long) Enr_REG_REM_ONE_USR_UNKNOWN_ACTION);
      switch (Gbl.Usrs.RegRemAction)
	{
	 case Enr_REGISTER_MODIFY_ONE_USR_IN_CRS:
	    if (MeOrOther == Usr_ME || Gbl.Usrs.Me.Role.Logged >= Rol_TCH)
	      {
	       /***** Get user's name from record form *****/
	       if (Usr_ICanChangeOtherUsrData (&Gbl.Usrs.Other.UsrDat))
		  Rec_GetUsrNameFromRecordForm (&Gbl.Usrs.Other.UsrDat);

	       /***** Update user's data in database *****/
	       Enr_UpdateUsrData (&Gbl.Usrs.Other.UsrDat);

	       if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
		 {
		  /***** Get new role from record form *****/
		  NewRole = Rec_GetRoleFromRecordForm ();

		  /***** Register user in current course in database *****/
		  if (Enr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
		    {
		     OldRole = Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs;	// Remember old role before changing it
		     if (NewRole != OldRole)	// The role must be updated
		       {
			/* Modify role */
			Enr_ModifyRoleInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole);

			/* Set success message */
			Ale_CreateAlert (Ale_SUCCESS,NULL,
			                 Txt_The_role_of_THE_USER_X_in_the_course_Y_has_changed_from_A_to_B,
					 Gbl.Usrs.Other.UsrDat.FullName,
					 Gbl.Hierarchy.Node[Hie_CRS].FullName,
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
	             Ale_CreateAlert (Ale_SUCCESS,NULL,
	        	              Txt_THE_USER_X_has_been_enroled_in_the_course_Y,
			              Gbl.Usrs.Other.UsrDat.FullName,
				      Gbl.Hierarchy.Node[Hie_CRS].FullName);
		    }

		  /***** Change user's groups *****/
		  if (Gbl.Crs.Grps.NumGrps)	// This course has groups?
		     switch (MeOrOther)
		       {
			case Usr_ME:
			   Grp_ChangeMyGrps (Cns_QUIET);
			   break;
			case Usr_OTHER:
			default:
			   Grp_ChangeOtherUsrGrps ();
			   break;
		       }

		  /***** If it's me, change my roles *****/
		  if (MeOrOther == Usr_ME)
		    {
		     Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs = Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs;
                     Gbl.Usrs.Me.UsrDat.Roles.InCrss = Gbl.Usrs.Other.UsrDat.Roles.InCrss;
                     Rol_SetMyRoles ();
		    }

		  /***** Change current action *****/
		  if (!Action[NewRole])
		     Err_WrongRoleExit ();
		  Gbl.Action.Act = Action[NewRole];
		  Tab_SetCurrentTab ();
		 }
	      }
	    else
	       Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	 case Enr_REGISTER_ONE_DEG_ADMIN:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_CTR_ADM)
	       Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	 case Enr_REGISTER_ONE_CTR_ADMIN:
	    if (Gbl.Usrs.Me.Role.Logged < Rol_INS_ADM)
	       Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	 case Enr_REGISTER_ONE_INS_ADMIN:
	    if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
	       Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	 case Enr_REPORT_USR_AS_POSSIBLE_DUPLICATE:
	    if (MeOrOther == Usr_ME || Gbl.Usrs.Me.Role.Logged < Rol_TCH)
	       Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	 case Enr_REMOVE_ONE_USR_FROM_CRS:
	    if (MeOrOther == Usr_OTHER && Gbl.Usrs.Me.Role.Logged < Rol_TCH)
	       Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	 case Enr_REMOVE_ONE_DEG_ADMIN:
	    if (MeOrOther == Usr_OTHER && Gbl.Usrs.Me.Role.Logged < Rol_CTR_ADM)
	       Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	 case Enr_REMOVE_ONE_CTR_ADMIN:
	    if (MeOrOther == Usr_OTHER && Gbl.Usrs.Me.Role.Logged < Rol_INS_ADM)
	       Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	 case Enr_REMOVE_ONE_INS_ADMIN:
	    if (MeOrOther == Usr_OTHER && Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
	       Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	 case Enr_ELIMINATE_ONE_USR_FROM_PLATFORM:
	    if (!Acc_CheckIfICanEliminateAccount (Gbl.Usrs.Other.UsrDat.UsrCod))
	       Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	 default:
	    Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	}
     }
   else
      Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
  }

void Enr_ModifyUsr2 (void)
  {
   if (Ale_GetTypeOfLastAlert () == Ale_WARNING)
     {
      /***** Show possible alerts *****/
      Ale_ShowAlerts (NULL);

      /***** Show form to edit user again *****/
      Enr_ShowFormToEditOtherUsr ();
     }
   else // No error
      switch (Gbl.Usrs.RegRemAction)
	{
	 case Enr_REGISTER_MODIFY_ONE_USR_IN_CRS:
            /***** Show possible alerts *****/
            Ale_ShowAlerts (NULL);

            /***** Show form to edit user again *****/
	    Enr_ShowFormToEditOtherUsr ();
	    break;
	 case Enr_REGISTER_ONE_DEG_ADMIN:
	    Adm_ReqAddAdm (Hie_DEG);
	    break;
	 case Enr_REGISTER_ONE_CTR_ADMIN:
	    Adm_ReqAddAdm (Hie_CTR);
	    break;
	 case Enr_REGISTER_ONE_INS_ADMIN:
	    Adm_ReqAddAdm (Hie_INS);
	    break;
	 case Enr_REPORT_USR_AS_POSSIBLE_DUPLICATE:
	    Dup_ReportUsrAsPossibleDuplicate ();
	    break;
	 case Enr_REMOVE_ONE_USR_FROM_CRS:
            Enr_ReqRemUsrFromCrs ();
	    break;
	 case Enr_REMOVE_ONE_DEG_ADMIN:
            Adm_ReqRemAdmOfDeg ();
	    break;
	 case Enr_REMOVE_ONE_CTR_ADMIN:
            Adm_ReqRemAdmOfCtr ();
	    break;
	 case Enr_REMOVE_ONE_INS_ADMIN:
            Adm_ReqRemAdmOfIns ();
	    break;
	 case Enr_ELIMINATE_ONE_USR_FROM_PLATFORM:
	    Acc_ReqRemAccountOrRemAccount (Acc_REQUEST_REMOVE_USR);
	    break;
	 default:
	    break;
	}
  }

/*****************************************************************************/
/******************* Ask if really wanted to remove a user *******************/
/*****************************************************************************/

static void Enr_AskIfRemoveUsrFromCrs (struct Usr_Data *UsrDat)
  {
   extern const char *Txt_Do_you_really_want_to_be_removed_from_the_course_X;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_user_from_the_course_X;
   extern const char *Txt_Remove_me_from_this_course;
   extern const char *Txt_Remove_user_from_this_course;
   Usr_MeOrOther_t MeOrOther;
   static const Act_Action_t NextAction[Rol_NUM_ROLES] =
     {
      [Rol_STD] = ActRemStdCrs,
      [Rol_NET] = ActRemNETCrs,
      [Rol_TCH] = ActRemTchCrs,
     };
   const char *Question[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Do_you_really_want_to_be_removed_from_the_course_X,
      [Usr_OTHER] = Txt_Do_you_really_want_to_remove_the_following_user_from_the_course_X
     };
   const char *TxtButton[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Remove_me_from_this_course,
      [Usr_OTHER] = Txt_Remove_user_from_this_course
     };

   if (Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat))
     {
      MeOrOther = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);

      /***** Show question and button to remove user as administrator *****/
      /* Begin alert */
      Ale_ShowAlertAndButton1 (Ale_QUESTION,Question[MeOrOther],
			       Gbl.Hierarchy.Node[Hie_CRS].FullName);

      /* Show user's record */
      Rec_ShowSharedRecordUnmodifiable (UsrDat);

      /* Show form to request confirmation */
      if (!NextAction[UsrDat->Roles.InCurrentCrs])
	 Err_WrongRoleExit ();
      Frm_BeginForm (NextAction[UsrDat->Roles.InCurrentCrs]);
	 Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
	 Pwd_AskForConfirmationOnDangerousAction ();
	 Btn_PutRemoveButton (TxtButton[MeOrOther]);
      Frm_EndForm ();

      /* End alert */
      Ale_ShowAlertAndButton2 (ActUnk,NULL,NULL,
                               NULL,NULL,
                               Btn_NO_BUTTON,NULL);
     }
   else	      // User does not belong to current course
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/************************ Remove a user from a course ************************/
/*****************************************************************************/

static void Enr_EffectivelyRemUsrFromCrs (struct Usr_Data *UsrDat,
					  struct Hie_Node *Crs,
                                          Enr_RemoveUsrProduction_t RemoveUsrWorks,
					  Cns_QuietOrVerbose_t QuietOrVerbose)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_from_the_course_Y;

   if (Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat))
     {
      /***** Remove user from all attendance events in course *****/
      Att_DB_RemoveUsrFromCrsEvents (UsrDat->UsrCod,Crs->HieCod);

      /***** Remove user from all groups in course *****/
      Grp_RemUsrFromAllGrpsInCrs (UsrDat->UsrCod,Crs->HieCod);

      /***** Remove user's status about reading of course information *****/
      Inf_DB_RemoveUsrFromCrsInfoRead (UsrDat->UsrCod,Crs->HieCod);

      /***** Remove important production of this user in course *****/
      if (RemoveUsrWorks == Enr_REMOVE_USR_PRODUCTION)
	{
	 /* Remove works zone in course */
         Brw_RemoveUsrWorksInCrs (UsrDat,Crs);

	 /* Remove tests, exams and matches results made by user in course */
	 TstPrn_RemovePrintsMadeByUsrInCrs (UsrDat->UsrCod,Crs->HieCod);
	 Exa_DB_RemovePrintQstsMadeByUsrInCrs (UsrDat->UsrCod,Crs->HieCod);
	 Exa_DB_RemoveAllPrintsMadeByUsrInCrs (UsrDat->UsrCod,Crs->HieCod);
         Mch_RemoveMatchesMadeByUsrInCrs (UsrDat->UsrCod,Crs->HieCod);
	}

      /***** Remove fields of this user in its course record *****/
      Rec_DB_RemoveAllFieldContentsFromUsrRecordInCrs (UsrDat->UsrCod,Crs->HieCod);

      /***** Remove some information about files in course and groups *****/
      Brw_DB_RemoveSomeInfoAboutCrsUsrFiles (UsrDat->UsrCod,Crs->HieCod);

      /***** Set all notifications for this user in this course as removed,
             except notifications about new messages *****/
      Ntf_DB_MarkNotifInCrsAsRemoved (UsrDat->UsrCod,Crs->HieCod);

      /***** Remove user from the tables of courses-users *****/
      Set_DB_RemUsrFromCrsSettings (UsrDat->UsrCod,Crs->HieCod);
      Enr_DB_RemUsrFromCrs (UsrDat->UsrCod,Crs->HieCod);

      /***** Flush caches *****/
      Usr_FlushCachesUsr ();

      /***** If it's me, change my roles *****/
      switch (Usr_ItsMe (UsrDat->UsrCod))
	{
	 case Usr_ME:
	    /* Now I don't belong to current course */
	    Gbl.Usrs.Me.IBelongToCurrentCrs =
	    Gbl.Usrs.Me.UsrDat.Accepted     = false;

	    /* Fill the list with the courses I belong to */
	    Gbl.Usrs.Me.MyCrss.Filled = false;
	    Enr_GetMyCourses ();

	    /* Set my roles */
	    Gbl.Usrs.Me.Role.FromSession              =
	    Gbl.Usrs.Me.Role.Logged                   =
	    Gbl.Usrs.Me.Role.LoggedBeforeCloseSession =
	    Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs     =
	    UsrDat->Roles.InCurrentCrs                = Rol_UNK;

	    Gbl.Usrs.Me.UsrDat.Roles.InCrss =
	    UsrDat->Roles.InCrss            = -1;	// not yet filled/calculated

	    Rol_SetMyRoles ();
	    break;
	 case Usr_OTHER:
	 default:
	    /* Now he/she does not belong to current course */
	    UsrDat->Accepted           = false;
	    UsrDat->Roles.InCurrentCrs = Rol_USR;
	    break;
        }

      if (QuietOrVerbose == Cns_VERBOSE)
	 Ale_CreateAlert (Ale_SUCCESS,NULL,
	                  Txt_THE_USER_X_has_been_removed_from_the_course_Y,
                          UsrDat->FullName,Crs->FullName);
     }
   else        // User does not belong to course
      if (QuietOrVerbose == Cns_VERBOSE)
	 Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/*************** Get all my courses and store them in a list *****************/
/*****************************************************************************/

void Enr_GetMyCourses (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   long CrsCod;

   /***** Trivial check 1: if my courses are already filled, there's nothing to do *****/
   if (Gbl.Usrs.Me.MyCrss.Filled)
      return;

   /***** Trivial check 2: if user's code is not set, don't query database *****/
   if (Gbl.Usrs.Me.UsrDat.UsrCod <= 0)
      return;

   /***** Remove temporary table with my courses *****/
   Enr_DB_DropTmpTableMyCourses ();

   /***** Create temporary table with my courses *****/
   Enr_DB_CreateTmpTableMyCourses ();

   /***** Get my courses from database *****/
   NumCrss = Enr_DB_GetMyCourses (&mysql_res);
   for (NumCrs = 0;
	NumCrs < NumCrss;
	NumCrs++)
     {
      /* Get next course */
      row = mysql_fetch_row (mysql_res);

      /* Get course code (row[0]) */
      if ((CrsCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
	{
	 if (Gbl.Usrs.Me.MyCrss.Num == Crs_MAX_COURSES_PER_USR)
	    Err_ShowErrorAndExit ("Maximum number of courses of a user exceeded.");

	 Gbl.Usrs.Me.MyCrss.Crss[Gbl.Usrs.Me.MyCrss.Num].CrsCod = CrsCod;

	 /* Get role (row[1]) and degree code (row[2]) */
	 Gbl.Usrs.Me.MyCrss.Crss[Gbl.Usrs.Me.MyCrss.Num].Role   = Rol_ConvertUnsignedStrToRole (row[1]);
	 Gbl.Usrs.Me.MyCrss.Crss[Gbl.Usrs.Me.MyCrss.Num].DegCod = Str_ConvertStrCodToLongCod (row[2]);

	 Gbl.Usrs.Me.MyCrss.Num++;
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Set boolean that indicates that my courses are already filled *****/
   Gbl.Usrs.Me.MyCrss.Filled = true;
  }

/*****************************************************************************/
/************************ Free the list of my courses ************************/
/*****************************************************************************/

void Enr_FreeMyCourses (void)
  {
   if (Gbl.Usrs.Me.MyCrss.Filled)
     {
      /***** Reset list *****/
      Gbl.Usrs.Me.MyCrss.Filled = false;
      Gbl.Usrs.Me.MyCrss.Num    = 0;

      /***** Remove temporary table with my courses *****/
      Enr_DB_DropTmpTableMyCourses ();
     }
  }

/*****************************************************************************/
/*********************** Check if I belong to a course ***********************/
/*****************************************************************************/

bool Enr_CheckIfIBelongToCrs (long CrsCod)
  {
   unsigned NumMyCrs;

   /***** Fill the list with the courses I belong to *****/
   Enr_GetMyCourses ();

   /***** Check if the course passed as parameter is any of my courses *****/
   for (NumMyCrs = 0;
        NumMyCrs < Gbl.Usrs.Me.MyCrss.Num;
        NumMyCrs++)
      if (Gbl.Usrs.Me.MyCrss.Crss[NumMyCrs].CrsCod == CrsCod)
         return true;

   return false;
  }

/*****************************************************************************/
/***** Check if user belongs (no matter if he/she has accepted or not) *******/
/***** to the current course                                           *******/
/*****************************************************************************/

void Enr_FlushCacheUsrBelongsToCurrentCrs (void)
  {
   Gbl.Cache.UsrBelongsToCurrentCrs.Valid = false;
  }

bool Enr_CheckIfUsrBelongsToCurrentCrs (const struct Usr_Data *UsrDat)
  {
   /***** 1. Fast check: Trivial cases *****/
   if (UsrDat->UsrCod <= 0 ||
       Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)
      return false;

   /***** 2. Fast check: If cached... *****/
   if (Gbl.Cache.UsrBelongsToCurrentCrs.Valid &&
       UsrDat->UsrCod == Gbl.Cache.UsrBelongsToCurrentCrs.UsrCod)
      return Gbl.Cache.UsrBelongsToCurrentCrs.Belongs;

   /***** 3. Fast check: If we know role of user in the current course *****/
   if (UsrDat->Roles.InCurrentCrs != Rol_UNK)
     {
      Gbl.Cache.UsrBelongsToCurrentCrs.UsrCod  = UsrDat->UsrCod;
      Gbl.Cache.UsrBelongsToCurrentCrs.Belongs = UsrDat->Roles.InCurrentCrs == Rol_STD ||
	                                         UsrDat->Roles.InCurrentCrs == Rol_NET ||
	                                         UsrDat->Roles.InCurrentCrs == Rol_TCH;
      Gbl.Cache.UsrBelongsToCurrentCrs.Valid = true;
      return Gbl.Cache.UsrBelongsToCurrentCrs.Belongs;
     }

   /***** 4. Fast / slow check: Get if user belongs to current course *****/
   Gbl.Cache.UsrBelongsToCurrentCrs.UsrCod  = UsrDat->UsrCod;
   Gbl.Cache.UsrBelongsToCurrentCrs.Belongs = Hie_CheckIfUsrBelongsTo (Hie_CRS,UsrDat->UsrCod,
						                       Gbl.Hierarchy.Node[Hie_CRS].HieCod,
						                       false);
   Gbl.Cache.UsrBelongsToCurrentCrs.Valid = true;
   return Gbl.Cache.UsrBelongsToCurrentCrs.Belongs;
  }

/*****************************************************************************/
/***** Check if user belongs (no matter if he/she has accepted or not) *******/
/***** to the current course                                           *******/
/*****************************************************************************/

void Enr_FlushCacheUsrHasAcceptedInCurrentCrs (void)
  {
   Gbl.Cache.UsrHasAcceptedInCurrentCrs.Valid = false;
  }

bool Enr_CheckIfUsrHasAcceptedInCurrentCrs (const struct Usr_Data *UsrDat)
  {
   /***** 1. Fast check: Trivial cases *****/
   if (UsrDat->UsrCod <= 0 ||
       Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)
      return false;

   /***** 2. Fast check: If cached... *****/
   if (Gbl.Cache.UsrHasAcceptedInCurrentCrs.Valid &&
       UsrDat->UsrCod == Gbl.Cache.UsrHasAcceptedInCurrentCrs.UsrCod)
      return Gbl.Cache.UsrHasAcceptedInCurrentCrs.Accepted;

   /***** 3. Fast / slow check: Get if user belongs to current course
                                and has accepted *****/
   Gbl.Cache.UsrHasAcceptedInCurrentCrs.UsrCod = UsrDat->UsrCod;
   Gbl.Cache.UsrHasAcceptedInCurrentCrs.Accepted = Hie_CheckIfUsrBelongsTo (Hie_CRS,UsrDat->UsrCod,
						                            Gbl.Hierarchy.Node[Hie_CRS].HieCod,
						                            true);
   Gbl.Cache.UsrHasAcceptedInCurrentCrs.Valid = true;
   return Gbl.Cache.UsrHasAcceptedInCurrentCrs.Accepted;
  }

/*****************************************************************************/
/*************** Check if a user belongs to any of my courses ****************/
/*****************************************************************************/

void Enr_FlushCacheUsrSharesAnyOfMyCrs (void)
  {
   Gbl.Cache.UsrSharesAnyOfMyCrs.Valid = false;
  }

bool Enr_CheckIfUsrSharesAnyOfMyCrs (struct Usr_Data *UsrDat)
  {
   /***** 1. Fast check: Am I logged? *****/
   if (!Gbl.Usrs.Me.Logged)
      return false;

   /***** 2. Fast check: It is a valid user code? *****/
   if (UsrDat->UsrCod <= 0)
      return false;

   /***** 3. Fast check: It's me? *****/
   if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)
      return true;

   /***** 4. Fast check: Is already calculated if user shares any course with me? *****/
   if (Gbl.Cache.UsrSharesAnyOfMyCrs.Valid &&
       UsrDat->UsrCod == Gbl.Cache.UsrSharesAnyOfMyCrs.UsrCod)
      return Gbl.Cache.UsrSharesAnyOfMyCrs.SharesAnyOfMyCrs;

   /***** 5. Fast check: Is course selected and we both belong to it? *****/
   if (Gbl.Usrs.Me.IBelongToCurrentCrs)
      if (Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat))	// Course selected and we both belong to it
        {
         Gbl.Cache.UsrSharesAnyOfMyCrs.UsrCod = UsrDat->UsrCod;
         Gbl.Cache.UsrSharesAnyOfMyCrs.SharesAnyOfMyCrs = true;
         Gbl.Cache.UsrSharesAnyOfMyCrs.Valid = true;
         return true;
        }

   /***** 6. Fast/slow check: Does he/she belong to any course? *****/
   Rol_GetRolesInAllCrss (UsrDat);
   if (!(UsrDat->Roles.InCrss & ((1 << Rol_STD) |	// Any of his/her roles is student
	                         (1 << Rol_NET) |	// or non-editing teacher
			         (1 << Rol_TCH))))	// or teacher?
     {
      Gbl.Cache.UsrSharesAnyOfMyCrs.UsrCod = UsrDat->UsrCod;
      Gbl.Cache.UsrSharesAnyOfMyCrs.SharesAnyOfMyCrs = false;
      Gbl.Cache.UsrSharesAnyOfMyCrs.Valid = true;
      return false;
     }

   /***** 7. Slow check: Get if user shares any course with me from database *****/
   Gbl.Cache.UsrSharesAnyOfMyCrs.UsrCod = UsrDat->UsrCod;
   Gbl.Cache.UsrSharesAnyOfMyCrs.SharesAnyOfMyCrs = Enr_DB_CheckIfUsrSharesAnyOfMyCrs (UsrDat->UsrCod);
   return Gbl.Cache.UsrSharesAnyOfMyCrs.SharesAnyOfMyCrs;
  }

/*****************************************************************************/
/******* Get total number of users of one or several roles in courses ********/
/*****************************************************************************/

#define Enr_DB_MAX_BYTES_SUBQUERY_ROLES (Rol_NUM_ROLES * (10 + 1) - 1)

unsigned Enr_GetNumUsrsInCrss (Hie_Level_t Level,long Cod,unsigned Roles)
  {
   bool AnyUserInCourses;
   unsigned NumUsrs;

   /***** Reset roles that can not belong to courses.
          Only
          - students,
          - non-editing teachers,
          - teachers
          can belong to a course *****/
   Roles &= ((1 << Rol_STD) |
	     (1 << Rol_NET) |
	     (1 << Rol_TCH));

   /***** Check if no roles requested *****/
   if (Roles == 0)
      return 0;

   /***** Check if any user in courses is requested *****/
   AnyUserInCourses = (Roles == ((1 << Rol_STD) |
	                         (1 << Rol_NET) |
	                         (1 << Rol_TCH)));

   /***** Get number of users from database *****/
   NumUsrs = Enr_DB_GetNumUsrsInCrss (Level,Cod,Roles,AnyUserInCourses);

   FigCch_UpdateFigureIntoCache (Enr_GetFigureNumUsrsInCrss (Roles),Level,Cod,
				 FigCch_UNSIGNED,&NumUsrs);
   return NumUsrs;
  }

unsigned Enr_GetCachedNumUsrsInCrss (Hie_Level_t Level,long Cod,unsigned Roles)
  {
   unsigned NumUsrsInCrss;

   /***** Get number of users in courses from cache *****/
   if (!FigCch_GetFigureFromCache (Enr_GetFigureNumUsrsInCrss (Roles),Level,Cod,
                                   FigCch_UNSIGNED,&NumUsrsInCrss))
      /***** Get current number of users in courses from database and update cache *****/
      NumUsrsInCrss = Enr_GetNumUsrsInCrss (Level,Cod,Roles);

   return NumUsrsInCrss;
  }

static FigCch_FigureCached_t Enr_GetFigureNumUsrsInCrss (unsigned Roles)
  {
   switch (Roles)
     {
      case 1 << Rol_STD:	// Students
	 return FigCch_NUM_STDS_IN_CRSS;
      case 1 << Rol_NET:	// Non-editing teachers
	 return FigCch_NUM_NETS_IN_CRSS;
      case 1 << Rol_TCH:	// Teachers
	 return FigCch_NUM_TCHS_IN_CRSS;
      case 1 << Rol_NET |
	   1 << Rol_TCH:	// Any teacher in courses
	 return FigCch_NUM_ALLT_IN_CRSS;
      case 1 << Rol_STD |
	   1 << Rol_NET |
	   1 << Rol_TCH:	// Any user in courses
	 return FigCch_NUM_USRS_IN_CRSS;
      default:
	 Err_WrongRoleExit ();
	 return FigCch_UNKNOWN;	// Not reached
     }
  }

/*****************************************************************************/
/******** Get total number of users who do not belong to any course **********/
/*****************************************************************************/

unsigned Enr_GetCachedNumUsrsNotBelongingToAnyCrs (void)
  {
   unsigned NumGsts;

   /***** Get number of guests from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_GSTS,Hie_SYS,-1L,
                                   FigCch_UNSIGNED,&NumGsts))
     {
      /***** Get current number of guests from database and update cache *****/
      NumGsts = Enr_DB_GetNumUsrsNotBelongingToAnyCrs ();
      FigCch_UpdateFigureIntoCache (FigCch_NUM_GSTS,Hie_SYS,-1L,
                                    FigCch_UNSIGNED,&NumGsts);
     }

   return NumGsts;
  }

/*****************************************************************************/
/************ Get average number of courses with users of a type *************/
/*****************************************************************************/

double Enr_GetCachedAverageNumUsrsPerCrs (Hie_Level_t Level,long Cod,Rol_Role_t Role)
  {
   static const FigCch_FigureCached_t FigureNumUsrsPerCrs[Rol_NUM_ROLES] =
     {
      [Rol_UNK] = FigCch_NUM_USRS_PER_CRS,	// Number of users per course
      [Rol_STD] = FigCch_NUM_STDS_PER_CRS,	// Number of students per course
      [Rol_NET] = FigCch_NUM_NETS_PER_CRS,	// Number of non-editing teachers per course
      [Rol_TCH] = FigCch_NUM_TCHS_PER_CRS,	// Number of teachers per course
     };
   double AverageNumUsrsPerCrs;

   /***** Get number of users per course from cache *****/
   if (!FigCch_GetFigureFromCache (FigureNumUsrsPerCrs[Role],Level,Cod,
                                   FigCch_DOUBLE,&AverageNumUsrsPerCrs))
     {
      /***** Get current number of users per course from database and update cache *****/
      AverageNumUsrsPerCrs = Enr_DB_GetAverageNumUsrsPerCrs (Level,Cod,Role);
      FigCch_UpdateFigureIntoCache (FigureNumUsrsPerCrs[Role],Level,Cod,
                                    FigCch_DOUBLE,&AverageNumUsrsPerCrs);
     }

   return AverageNumUsrsPerCrs;
  }

/*****************************************************************************/
/************ Get average number of courses with users of a role *************/
/*****************************************************************************/

double Enr_GetCachedAverageNumCrssPerUsr (Hie_Level_t Level,long Cod,Rol_Role_t Role)
  {
   static const FigCch_FigureCached_t FigureNumCrssPerUsr[Rol_NUM_ROLES] =
     {
      [Rol_UNK] = FigCch_NUM_CRSS_PER_USR,	// Number of courses per user
      [Rol_STD] = FigCch_NUM_CRSS_PER_STD,	// Number of courses per student
      [Rol_NET] = FigCch_NUM_CRSS_PER_NET,	// Number of courses per non-editing teacher
      [Rol_TCH] = FigCch_NUM_CRSS_PER_TCH,	// Number of courses per teacher
     };
   double AverageNumCrssPerUsr;

   /***** Get number of courses per user from cache *****/
   if (!FigCch_GetFigureFromCache (FigureNumCrssPerUsr[Role],Level,Cod,
                                   FigCch_DOUBLE,&AverageNumCrssPerUsr))
     {
      /***** Get current number of courses per user from database and update cache *****/
      AverageNumCrssPerUsr = Enr_DB_GetAverageNumCrssPerUsr (Level,Cod,Role);
      FigCch_UpdateFigureIntoCache (FigureNumCrssPerUsr[Role],Level,Cod,
                                    FigCch_DOUBLE,&AverageNumCrssPerUsr);
     }

   return AverageNumCrssPerUsr;
  }
