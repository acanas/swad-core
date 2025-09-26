// swad_enrolment.c: enrolment or removing of users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include "swad_user_clipboard.h"
#include "swad_user_database.h"

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

#define Enr_NUM_ENR_REM_USRS_ACTIONS 6
typedef enum
  {
   Enr_ENR_REM_USRS_UNKNOWN_ACTION     	  = 0,
   Enr_ENROL_SPECIFIED_USRS_IN_CRS        = 1,
   Enr_REMOVE_SPECIFIED_USRS_FROM_CRS     = 2,
   Enr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS = 3,
   Enr_UPDATE_USRS_IN_CRS                 = 4,
   Enr_ELIMINATE_USRS_FROM_PLATFORM       = 5,
  } Enr_EnrRemUsrsAction_t;

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
/************************* Private global variables **************************/
/*****************************************************************************/

static Enr_EnrRemOneUsrAction_t Enr_EnrRemAction;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Enr_NotifyAfterEnrolment (const struct Usr_Data *UsrDat,
                                      Rol_Role_t NewRole);

static void Enr_ReqAdminUsrs (Rol_Role_t Role);
static void Enr_ShowFormEnrRemSeveralUsrs (Rol_Role_t Role);
static void Enr_PutAreaToEnterUsrsIDs (void);
static void Enr_PutUsrsClipboard (void);
static void Enr_PutActionsEnrRemSeveralUsrs (void);

static void Enr_ReceiveUsrsCrs (Rol_Role_t Role);
static void Enr_InitializeLstUsrsToBeRemoved (Rol_Role_t Role,
					      bool RemoveSpecifiedUsrs);
static void Enr_UpdateLstUsrsToBeRemovedUsingTextarea (Rol_Role_t Role,
						       bool RemoveSpecifiedUsrs,
						       struct Usr_Data *UsrDat,
						       char *ListUsrsIDs);
static void Enr_UpdateLstUsrsToBeRemovedUsingSelectedUsrs (Rol_Role_t Role,
							   bool RemoveSpecifiedUsrs,
							   struct Usr_Data *UsrDat);
static void Enr_RemoveUsrsMarkedToBeRemoved (Rol_Role_t Role,
					     bool EliminateUsrs,
					     struct Usr_Data *UsrDat,
					     struct ListCodGrps *LstGrps,
					     unsigned *NumUsrsRemoved);
static void Enr_EnrolUsrsFoundInTextarea (Rol_Role_t Role,
					  char *ListUsrsIDs,
					  struct Usr_Data *UsrDat,
					  struct ListCodGrps *LstGrps,
					  unsigned *NumUsrsEnroled);
static void Enr_EnrolSelectedUsrs (Rol_Role_t Role,
				   struct Usr_Data *UsrDat,
				   struct ListCodGrps *LstGrps,
				   unsigned *NumUsrsEnroled);
static void Enr_ShowMessageRemoved (unsigned NumUsrsRemoved,bool EliminateUsrs);
static void Enr_ShowMessageEnroled (unsigned NumUsrsEnroled);

static void Enr_PutActionModifyOneUsr (HTM_Attributes_t *Attributes,
                                       Usr_Belong_t UsrBelongsToCrs,
                                       Usr_MeOrOther_t MeOrOther);
static void Enr_PutActionEnrOneDegAdm (HTM_Attributes_t *Attributes);
static void Enr_PutActionEnrOneCtrAdm (HTM_Attributes_t *Attributes);
static void Enr_PutActionEnrOneInsAdm (HTM_Attributes_t *Attributes);
static void Enr_PutActionAddToClipboard (Enr_EnrRemOneUsrAction_t EnrRemOneUsrAction,
					 HTM_Attributes_t *Attributes);
static void Enr_PutActionOverwriteClipboard (Enr_EnrRemOneUsrAction_t EnrRemOneUsrAction,
					     HTM_Attributes_t *Attributes);
static void Enr_PutActionRepUsrAsDup (HTM_Attributes_t *Attributes);
static void Enr_PutActionRemUsrFromCrs (HTM_Attributes_t *Attributes,
					Usr_MeOrOther_t MeOrOther);
static void Enr_PutActionRemUsrAsDegAdm (HTM_Attributes_t *Attributes,
					 Usr_MeOrOther_t MeOrOther);
static void Enr_PutActionRemUsrAsCtrAdm (HTM_Attributes_t *Attributes,
					 Usr_MeOrOther_t MeOrOther);
static void Enr_PutActionRemUsrAsInsAdm (HTM_Attributes_t *Attributes,
					 Usr_MeOrOther_t MeOrOther);
static void Enr_PutActionRemUsrAcc (HTM_Attributes_t *Attributes,
				    Usr_MeOrOther_t MeOrOther);
static void Enr_EnrRemOneUsrActionBegin (Enr_EnrRemOneUsrAction_t EnrRemOneUsrAction,
                                         HTM_Attributes_t *Attributes);
static void Enr_EnrRemOneUsrActionEnd (void);

static void Enr_EnrolUsr (struct Usr_Data *UsrDat,Rol_Role_t Role,
                          struct ListCodGrps *LstGrps,unsigned *NumUsrsEnroled);

static void Enr_PutLinkToRemAllStdsThisCrs (void);

static void Enr_ShowEnrolmentRequestsGivenRoles (unsigned RolesSelected);

static void Enr_RemUsrEnrolmentRequestInCrs (long UsrCod,long CrsCod);

static void Enr_ReqEnrRemUsr (Rol_Role_t Role);
static void Enr_ReqAnotherUsrIDToEnrolRemove (Rol_Role_t Role);
static void Enr_AskIfEnrRemMe (Rol_Role_t Role);
static void Enr_AskIfEnrRemAnotherUsr (Rol_Role_t Role);
static void Enr_AskIfEnrRemUsr (struct Usr_ListUsrCods *ListUsrCods,Rol_Role_t Role);

static void Enr_ShowFormToEditOtherUsr (void);

static Usr_Can_t Enr_CheckIfICanRemUsrFromCrs (void);

static void Enr_SetEnrRemAction (Enr_EnrRemOneUsrAction_t EnrRemAction);
static Enr_EnrRemOneUsrAction_t Enr_GetEnrRemAction (void);

static void Enr_AskIfRemoveUsrFromCrs (struct Usr_Data *UsrDat);
static void Enr_EffectivelyRemUsrFromCrs (struct Usr_Data *UsrDat,
					  struct Hie_Node *Crs,
                                          Enr_RemoveUsrProduction_t RemoveUsrWorks,
					  Cns_Verbose_t Verbose);

static FigCch_FigureCached_t Enr_GetFigureNumUsrsInCrss (unsigned Roles);

/*****************************************************************************/
/** Check if current course has students and show warning no students found **/
/*****************************************************************************/

void Enr_CheckStdsAndPutButtonToEnrolStdsInCurrentCrs (void)
  {
   /***** Put link to enrol students *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_TCH)	// Course selected and I am logged as teacher
      if (!Enr_GetNumUsrsInCrss (Hie_CRS,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				 1 << Rol_STD))	// No students in course
         Usr_ShowWarningNoUsersFound (Rol_STD);
  }

/*****************************************************************************/
/******************** Put inline button to enrol students ********************/
/*****************************************************************************/

void Enr_PutButtonInlineToEnrolStds (long CrsCod,
				     unsigned Level,const Lay_Last_t *IsLastItemInLevel,
				     Lay_Highlight_t Highlight)
  {
   extern const char *Lay_HighlightClass[Lay_NUM_HIGHLIGHT];

   if (Rol_GetMyRoleInCrs (CrsCod) == Rol_TCH)	// I am a teacher in the given course
      if (!Enr_GetNumUsrsInCrss (Hie_CRS,CrsCod,
				 1 << Rol_STD))	// No students in course
	{
	 HTM_LI_Begin (Lay_HighlightClass[Highlight]);
	    Lay_IndentDependingOnLevel (Level,IsLastItemInLevel,
					Lay_NO_HORIZONTAL_LINE_AT_RIGHT);
	    Frm_BeginForm (ActReqEnrSevStd);
	       ParCod_PutPar (ParCod_Crs,CrsCod);
	       Btn_PutButtonInline (Btn_ENROL);
	    Frm_EndForm ();
	 HTM_LI_End ();
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
/************************* Enrol user in current course **********************/
/*****************************************************************************/
// Before calling this function, you must be sure that
// the user does not belong to the current course

void Enr_EnrolUsrInCurrentCrs (struct Usr_Data *UsrDat,Rol_Role_t NewRole,
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

   /***** Enrol user in current course in database *****/
   Enr_DB_InsertUsrInCurrentCrs (UsrDat->UsrCod,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
                                 NewRole,KeepOrSetAccepted);

   /***** Create last prefs in current course in database *****/
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
/********* Create notification after enrolling user in current course ********/
/*****************************************************************************/

static void Enr_NotifyAfterEnrolment (const struct Usr_Data *UsrDat,
                                      Rol_Role_t NewRole)
  {
   static Ntf_NotifyEvent_t NotifyEvent[Rol_NUM_ROLES] =
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

   /***** Form to request user's ID, @nickname or email address *****/
   Frm_BeginForm (NextAction);
      if (FuncPars)
	 FuncPars ();

      /***** Label *****/
      HTM_LABEL_Begin ("for=\"OtherUsrIDNickOrEMail\" class=\"RM FORM_IN_%s\"",
		       The_GetSuffix ());
	 HTM_Txt (Txt_nick_email_or_ID); HTM_Colon (); HTM_NBSP ();
      HTM_LABEL_End ();

      /***** Input box to enter user *****/
      HTM_INPUT_TEXT ("OtherUsrIDNickOrEMail",Cns_MAX_CHARS_EMAIL_ADDRESS,"",
		      HTM_REQUIRED,
		      "id=\"OtherUsrIDNickOrEMail\" size=\"16\""
		      " class=\"INPUT_%s\"",The_GetSuffix ());

      /***** Send button*****/
      Btn_PutButton (Btn_CONTINUE,NULL);

   Frm_EndForm ();
  }

/*****************************************************************************/
/****** Request acceptation / refusion of enrolment in current course ********/
/*****************************************************************************/

void Enr_ReqAcceptEnrolmentInCrs (void)
  {
   extern const char *Hlp_USERS_SignUp_confirm_enrolment;
   extern const char *Txt_Enrolment;
   extern const char *Txt_A_teacher_or_administrator_has_enroled_you_as_X_into_the_course_Y;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   static struct
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
   Box_BoxBegin (Txt_Enrolment,NULL,NULL,
                 Hlp_USERS_SignUp_confirm_enrolment,Box_NOT_CLOSABLE);

      /***** Show message *****/
      Ale_ShowAlert (Ale_INFO,Txt_A_teacher_or_administrator_has_enroled_you_as_X_into_the_course_Y,
		     Txt_ROLES_SINGUL_abc[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs][Gbl.Usrs.Me.UsrDat.Sex],
		     Gbl.Hierarchy.Node[Hie_CRS].FullName);

      /***** Send button to accept enrolment in the current course *****/
      if (!WhatToDo[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs].NextAction.Accept)
	 Err_WrongRoleExit ();
      Frm_BeginForm (WhatToDo[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs].NextAction.Accept);
	 Btn_PutButtonInline (Btn_CONFIRM);
      Frm_EndForm ();

      /***** Send button to refuse enrolment in the current course *****/
      if (!WhatToDo[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs].NextAction.Refuse)
	 Err_WrongRoleExit ();
      Frm_BeginForm (WhatToDo[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs].NextAction.Refuse);
	 Btn_PutButtonInline (Btn_REMOVE);
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
   __attribute__((unused)) Exi_Exist_t UsrExists;

   /***** Get user's role in course from database *****/
   Role = Rol_GetRoleUsrInCrs (UsrCod,CrsCod);

   /***** Set summary string *****/
   /* Initialize structure with user's data */
   Usr_UsrDataConstructor (&UsrDat);

      /* Get user's data */
      UsrDat.UsrCod = UsrCod;
      UsrExists = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							   Usr_DONT_GET_PREFS,
							   Usr_DONT_GET_ROLE_IN_CRS);

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

void Enr_ReqAdminNETs (void)
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
	 Enr_AskIfEnrRemMe (Role);
	 break;
      case Rol_TCH:
	 if (Gbl.Hierarchy.HieLvl == Hie_CRS &&
	     (Role == Rol_STD ||	// As a teacher, I can admin students...
	      Role == Rol_NET))		// ...or non-editing teachers
	    Enr_ShowFormEnrRemSeveralUsrs (Role);
	 else
	    Enr_AskIfEnrRemMe (Rol_TCH);
	 break;
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 if (Gbl.Hierarchy.HieLvl == Hie_CRS)
	    Enr_ShowFormEnrRemSeveralUsrs (Role);
	 else
	    Enr_ReqAnotherUsrIDToEnrolRemove (Role);
	 break;
      default:
	 Err_NoPermissionExit ();
	 break;
     }
  }

/*****************************************************************************/
/******* Enrol/remove users (taken from a list) in/from current course *******/
/*****************************************************************************/

static void Enr_ShowFormEnrRemSeveralUsrs (Rol_Role_t Role)
  {
   extern const char *Hlp_USERS_Administration_administer_multiple_users;
   extern const char *Txt_Administer_multiple_students;
   extern const char *Txt_Administer_multiple_non_editing_teachers;
   extern const char *Txt_Administer_multiple_teachers;
   extern const char *Txt_Step_1_Provide_a_list_of_users;
   extern const char *Txt_Step_2_Select_an_action;
   extern const char *Txt_Step_3_Optionally_select_groups;
   extern const char *Txt_Select_the_groups_in_from_which_you_want_to_enrol_remove_users_;
   extern const char *Txt_No_groups_have_been_created_in_the_course_X_Therefore_;
   extern const char *Txt_Step_4_Confirm_the_enrolment_removing;
   static struct
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
   if (Gbl.Hierarchy.HieLvl == Hie_CRS)	 	// Course selected
     {
      Mnu_ContextMenuBegin ();

	 switch (Role)
	   {
	    case Rol_STD:
	       /* Put link to go to admin student */
	       Enr_PutLinkToAdminOneUsr (ActReqID_MdfStd);

	       /* Put link to remove all students in the current course */
	       if (Enr_GetNumUsrsInCrss (Hie_CRS,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
					 1 << Rol_STD))	// This course has students
		  Enr_PutLinkToRemAllStdsThisCrs ();
	       break;
	    case Rol_NET:
	       /* Put link to go to admin teacher */
	       Enr_PutLinkToAdminOneUsr (ActReqID_MdfTch);
	       break;
	    case Rol_TCH:
	       /* Put link to go to admin teacher */
	       Enr_PutLinkToAdminOneUsr (ActReqID_MdfTch);
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
      Box_BoxBegin (*WhatToDo[Role].Title,NULL,NULL,
		    Hlp_USERS_Administration_administer_multiple_users,
		    Box_NOT_CLOSABLE);

	 /***** Step 1: List of students to be enroled / removed *****/
	 HTM_FIELDSET_Begin (NULL);
	    HTM_LEGEND (Txt_Step_1_Provide_a_list_of_users);
	       HTM_TABLE_BeginCenterPadding (2);
		  Enr_PutAreaToEnterUsrsIDs ();
		  Enr_PutUsrsClipboard ();
	       HTM_TABLE_End ();
	 HTM_FIELDSET_End ();

	 /***** Step 2: Put different actions to enrol/remove users to/from current course *****/
	 HTM_FIELDSET_Begin (NULL);
	    HTM_LEGEND (Txt_Step_2_Select_an_action);
	    Enr_PutActionsEnrRemSeveralUsrs ();
	 HTM_FIELDSET_End ();

	 /***** Step 3: Select groups in which enrol/remove users *****/
	 HTM_FIELDSET_Begin (NULL);
	    HTM_LEGEND (Txt_Step_3_Optionally_select_groups);
	    if (Gbl.Hierarchy.HieLvl == Hie_CRS)	// Course selected
	      {
	       if (Gbl.Crs.Grps.NumGrps)	// This course has groups?
		 {
		  Ale_ShowAlert (Ale_INFO,Txt_Select_the_groups_in_from_which_you_want_to_enrol_remove_users_);
		  Grp_ShowLstGrpsToChgOtherUsrsGrps (-1L);
		 }
	       else
		  /* Write help message */
		  Ale_ShowAlert (Ale_INFO,Txt_No_groups_have_been_created_in_the_course_X_Therefore_,
				 Gbl.Hierarchy.Node[Hie_CRS].FullName);
	      }
	 HTM_FIELDSET_End ();

	 /***** Step 4: Confirm enrol / remove students *****/
	 HTM_FIELDSET_Begin (NULL);
	    HTM_LEGEND (Txt_Step_4_Confirm_the_enrolment_removing);
	    Pwd_AskForConfirmationOnDangerousAction ();
	 HTM_FIELDSET_End ();

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_CONFIRM);

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
   unsigned Months;

   /***** Begin form *****/
   Frm_BeginForm (ActRemOldUsr);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Eliminate_old_users,NULL,NULL,NULL,Box_NOT_CLOSABLE);

	 /***** Form to request number of months without clicks *****/
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_Txt (Txt_Eliminate_all_users_who_are_not_enroled_on_any_courses_PART_1_OF_2);
	    HTM_NBSP ();
	    HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
			      "name=\"Months\" class=\"INPUT_%s\"",
			      The_GetSuffix ());
	       for (Months  = Usr_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS;
		    Months <= Usr_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS;
		    Months++)
		  HTM_OPTION (HTM_Type_UNSIGNED,&Months,
			      (Months == Usr_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS) ? HTM_SELECTED :
											     HTM_NO_ATTR,
			      "%u",Months);
	    HTM_SELECT_End ();
	    HTM_NBSP ();
	    HTM_TxtF (Txt_Eliminate_all_users_who_are_not_enroled_on_any_courses_PART_2_OF_2,
		      Cfg_PLATFORM_SHORT_NAME);
	 HTM_LABEL_End ();

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_ELIMINATE);

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
							 Usr_DONT_GET_ROLE_IN_CRS) == Exi_EXISTS)
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
   extern const char *Txt_The_nicks_emails_or_IDs_can_be_separated_;

   /***** Text area for users' IDs *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT","UsrsIDs",Txt_List_of_nicks_emails_or_IDs);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	 HTM_TEXTAREA_Begin (HTM_NO_ATTR,
			     "id=\"UsrsIDs\" name=\"UsrsIDs\""
			     " placeholder=\"%s\" rows=\"10\""
			     " class=\"Frm_C2_INPUT INPUT_%s\"",
			     Txt_The_nicks_emails_or_IDs_can_be_separated_,
			     The_GetSuffix ());
	 HTM_TEXTAREA_End ();
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************************ Put current users' clipboard ***********************/
/*****************************************************************************/

static void Enr_PutUsrsClipboard (void)
  {
   extern const char *Txt_Clipboard;

   /***** Users' clipboard *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT","",Txt_Clipboard);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB\"");
	 UsrClp_ListUsrsInMyClipboard (Frm_PUT_FORM,
				       true);	// Show even if empty
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/**** Put different actions to enrol/remove users to/from current course *****/
/*****************************************************************************/

static void Enr_PutActionsEnrRemSeveralUsrs (void)
  {
   extern const char *Txt_Enrol_the_users_indicated_in_step_1;
   extern const char *Txt_Remove_the_users_indicated_in_step_1;
   extern const char *Txt_Remove_the_users_not_indicated_in_step_1;
   extern const char *Txt_Enrol_the_users_indicated_in_step_1_and_remove_the_users_not_indicated;
   extern const char *Txt_Eliminate_from_the_platform_the_users_indicated_in_step_1;

   /***** Begin list of options *****/
   HTM_UL_Begin ("class=\"LIST_LEFT FORM_IN_%s\"",The_GetSuffix ());

      /***** Enrol / remove users listed or not listed *****/
      if (Gbl.Hierarchy.HieLvl == Hie_CRS)	// Course selected
	{
	 HTM_LI_Begin (NULL);
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_RADIO ("EnrRemAction",
			        HTM_CHECKED,
				" value=\"%u\"",
				(unsigned) Enr_ENROL_SPECIFIED_USRS_IN_CRS);
	       HTM_Txt (Txt_Enrol_the_users_indicated_in_step_1);
	    HTM_LABEL_End ();
	 HTM_LI_End ();

	 HTM_LI_Begin (NULL);
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_RADIO ("EnrRemAction",
				HTM_NO_ATTR,
				" value=\"%u\"",
				(unsigned) Enr_REMOVE_SPECIFIED_USRS_FROM_CRS);
	       HTM_Txt (Txt_Remove_the_users_indicated_in_step_1);
	    HTM_LABEL_End ();
	 HTM_LI_End ();

	 HTM_LI_Begin (NULL);
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_RADIO ("EnrRemAction",
				HTM_NO_ATTR,
				" value=\"%u\"",
				(unsigned) Enr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS);
	       HTM_Txt (Txt_Remove_the_users_not_indicated_in_step_1);
	    HTM_LABEL_End ();
	 HTM_LI_End ();

	 HTM_LI_Begin (NULL);
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_RADIO ("EnrRemAction",
				HTM_NO_ATTR,
				" value=\"%u\"",
				(unsigned) Enr_UPDATE_USRS_IN_CRS);
	       HTM_Txt (Txt_Enrol_the_users_indicated_in_step_1_and_remove_the_users_not_indicated);
	    HTM_LABEL_End ();
	 HTM_LI_End ();
	}

      /***** Only for superusers *****/
      if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	{
	 HTM_LI_Begin (NULL);
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_RADIO ("EnrRemAction",
				HTM_NO_ATTR,
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
/********** Receive the list of users of the course to enrol/remove **********/
/*****************************************************************************/

void Enr_ReceiveAdminStds (void)
  {
   Enr_ReceiveUsrsCrs (Rol_STD);
  }

void Enr_ReceiveAdminNETs (void)
  {
   Enr_ReceiveUsrsCrs (Rol_NET);
  }

void Enr_ReceiveAdminTchs (void)
  {
   Enr_ReceiveUsrsCrs (Rol_TCH);
  }

static void Enr_ReceiveUsrsCrs (Rol_Role_t Role)
  {
   extern const char *Txt_In_a_type_of_group_with_single_enrolment_students_can_not_be_enroled_in_more_than_one_group;
   struct
     {
      bool RemoveUsrs;
      bool RemoveSpecifiedUsrs;
      bool EliminateUsrs;
      bool EnrolUsrs;
     } WhatToDo;
   char *ListUsrsIDs;
   unsigned NumUsrsEnroled = 0;
   unsigned NumUsrsRemoved = 0;
   struct ListCodGrps LstGrps;
   struct Usr_Data UsrDat;
   Enr_EnrRemUsrsAction_t EnrRemUsrsAction;
   Err_SuccessOrError_t SelectionIsValid = Err_SUCCESS;

   /***** Check the role of users to enrol/remove *****/
   switch (Role)
     {
      case Rol_STD:
      case Rol_NET:
	 if (Gbl.Usrs.Me.Role.Logged < Rol_TCH)		// Can I enrol/remove students?
	    // No, I can not
	    Err_NoPermissionExit ();
	 break;
      case Rol_TCH:
	 if (Gbl.Usrs.Me.Role.Logged < Rol_DEG_ADM)	// Can I enrol/remove teachers?
	    // No, I can not
	    Err_NoPermissionExit ();
	 break;
      default:
	 Err_WrongRoleExit ();
	 break;
     }

   /***** Get confirmation *****/
   if (Pwd_GetConfirmationOnDangerousAction () == Err_ERROR)
      return;

   /***** Get the action to do *****/
   WhatToDo.RemoveUsrs		=
   WhatToDo.RemoveSpecifiedUsrs	=
   WhatToDo.EliminateUsrs	=
   WhatToDo.EnrolUsrs		= false;

   EnrRemUsrsAction = (Enr_EnrRemUsrsAction_t)
	              Par_GetParUnsignedLong ("EnrRemAction",
                                              0,
                                              Enr_NUM_ENR_REM_USRS_ACTIONS - 1,
                                              (unsigned long) Enr_ENR_REM_USRS_UNKNOWN_ACTION);
   switch (EnrRemUsrsAction)
     {
      case Enr_ENROL_SPECIFIED_USRS_IN_CRS:
	 WhatToDo.RemoveUsrs		= false;
	 WhatToDo.RemoveSpecifiedUsrs	= false;	// Ignored
	 WhatToDo.EliminateUsrs		= false;	// Ignored
	 WhatToDo.EnrolUsrs		= true;
	 break;
      case Enr_REMOVE_SPECIFIED_USRS_FROM_CRS:
	 WhatToDo.RemoveUsrs		= true;
	 WhatToDo.RemoveSpecifiedUsrs	= true;
	 WhatToDo.EliminateUsrs		= false;
	 WhatToDo.EnrolUsrs		= false;
	 break;
      case Enr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS:
	 WhatToDo.RemoveUsrs		= true;
	 WhatToDo.RemoveSpecifiedUsrs	= false;
	 WhatToDo.EliminateUsrs		= false;
	 WhatToDo.EnrolUsrs		= false;
	 break;
      case Enr_UPDATE_USRS_IN_CRS:
	 WhatToDo.RemoveUsrs		= true;
	 WhatToDo.RemoveSpecifiedUsrs	= false;
	 WhatToDo.EliminateUsrs		= false;
	 WhatToDo.EnrolUsrs		= true;
	 break;
      case Enr_ELIMINATE_USRS_FROM_PLATFORM:
	 if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	   {
	    WhatToDo.RemoveUsrs		= true;
	    WhatToDo.RemoveSpecifiedUsrs= true;
	    WhatToDo.EliminateUsrs	= true;
	    WhatToDo.EnrolUsrs		= false;
	   }
	 else
	    Err_NoPermissionExit ();
	 break;
      default:
	 Err_ShowErrorAndExit ("Wrong enrolment / removing specification.");
	 break;
     }

   /***** Get groups to which remove/enrol users *****/
   LstGrps.NumGrps = 0;
   if (Gbl.Crs.Grps.NumGrps) // This course has groups?
     {
      /***** Get list of groups types and groups in current course *****/
      Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_GRP_TYPES_WITH_GROUPS);

      /***** Get the list of groups to which enrol/remove students *****/
      LstGrps.GrpCods = NULL;	// Initialized to avoid bug reported by Coverity
      LstGrps.NumGrps = 0;	// Initialized to avoid bug reported by Coverity
      Grp_GetLstCodsGrpWanted (&LstGrps);

      /***** A student can't belong to more than one group
             when the type of group only allows to enrol in one group *****/
      if (WhatToDo.EnrolUsrs)
        {
	 switch (Role)
	   {
	    case Rol_STD:
	       /* Check if I have selected more than one group of single enrolment */
	       SelectionIsValid = Grp_CheckIfAtMostOneSingleEnrolmentGrpIsSelected (&LstGrps,
							                            false);	// Don't check closed groups
	       break;
	    case Rol_NET:
	    case Rol_TCH:
	       break;
	    default:
	       Err_WrongRoleExit ();
	       break;
	   }
        }
     }

   switch (SelectionIsValid)
     {
      case Err_SUCCESS:
	 /***** Get list of users' IDs *****/
	 if ((ListUsrsIDs = malloc (ID_MAX_BYTES_LIST_USRS_IDS + 1)) == NULL)
	    Err_NotEnoughMemoryExit ();
	 Par_GetParText ("UsrsIDs",ListUsrsIDs,ID_MAX_BYTES_LIST_USRS_IDS);

	 /***** Get list of selected users if not already got *****/
	 Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected,
						Usr_GET_LIST_ALL_USRS);

	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&UsrDat);

	 /***** Remove users *****/
	 if (WhatToDo.RemoveUsrs)
	   {
	    /* Get list of users in current course */
	    Usr_GetListUsrs (Hie_CRS,Role);

	    if (Gbl.Usrs.LstUsrs[Role].NumUsrs)
	      {
	       /* Loop 1: Initialize list of users to remove */
	       Enr_InitializeLstUsrsToBeRemoved (Role,WhatToDo.RemoveSpecifiedUsrs);

	       /* Loop 2: Go through form list setting if a user must be removed */
	       /* 2.1: Update list of users to be removed
		       using the form with IDs, nicks and emails */
	       Enr_UpdateLstUsrsToBeRemovedUsingTextarea (Role,WhatToDo.RemoveSpecifiedUsrs,
							  &UsrDat,ListUsrsIDs);

	       /* 2.2: Update list of users to be removed
		       using list of users selected from clipboard */
	       Enr_UpdateLstUsrsToBeRemovedUsingSelectedUsrs (Role,WhatToDo.RemoveSpecifiedUsrs,
							      &UsrDat);

	       /* Loop 3: Go through list removing users */
	       Enr_RemoveUsrsMarkedToBeRemoved (Role,WhatToDo.EliminateUsrs,
						&UsrDat,&LstGrps,&NumUsrsRemoved);
	      }

	    /* Free memory for users list */
	    Usr_FreeUsrsList (Role);

	    /* Write messages */
	    Enr_ShowMessageRemoved (NumUsrsRemoved,WhatToDo.EliminateUsrs);
	   }

	 /***** Enrol users *****/
	 if (WhatToDo.EnrolUsrs)	// TODO: !!!!! NO CAMBIAR EL ROL DE LOS USUARIOS QUE YA ESTÉN EN LA ASIGNATURA SI HAY MÁS DE UN USUARIO ENCONTRADO PARA EL MISMO DNI !!!!!!
	   {
	    /* Enrol users found in the form with IDs, nicks and emails */
	    Enr_EnrolUsrsFoundInTextarea (Role,ListUsrsIDs,&UsrDat,&LstGrps,&NumUsrsEnroled);

	    /* Enrol users selected from clipboard */
	    Enr_EnrolSelectedUsrs (Role,&UsrDat,&LstGrps,&NumUsrsEnroled);

	    /* Write messages */
	    Enr_ShowMessageEnroled (NumUsrsEnroled);
	   }

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);

	 /***** Free memory used by list of selected users' codes *****/
	 Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

	 /***** Free memory used by the list of user's IDs *****/
	 free (ListUsrsIDs);
	 break;
      case Err_ERROR:	// Selection of groups not valid
      default:
	 Ale_ShowAlert (Ale_WARNING,
			Txt_In_a_type_of_group_with_single_enrolment_students_can_not_be_enroled_in_more_than_one_group);
	 break;
     }

   /***** Free memory with the list of groups to/from which remove/enrol users *****/
   Grp_FreeListCodGrp (&LstGrps);

   /***** Free list of groups types and groups in current course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/******************** Initialize list of users to remove *********************/
/*****************************************************************************/

static void Enr_InitializeLstUsrsToBeRemoved (Rol_Role_t Role,
					      bool RemoveSpecifiedUsrs)
  {
   unsigned NumUsr;

   for (NumUsr = 0;
	NumUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs;
	NumUsr++)
      Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Remove = !RemoveSpecifiedUsrs;
  }

/*****************************************************************************/
/*************** Update list of users to be removed        *******************/
/*************** using the form with IDs, nicks and emails *******************/
/*****************************************************************************/

static void Enr_UpdateLstUsrsToBeRemovedUsingTextarea (Rol_Role_t Role,
						       bool RemoveSpecifiedUsrs,
						       struct Usr_Data *UsrDat,
						       char *ListUsrsIDs)
  {
   const char *Ptr;
   unsigned NumUsr;
   unsigned NumUsrFound;
   struct Usr_ListUsrCods ListUsrCods;	// List with users' codes for a given user's ID

   for (Ptr = ListUsrsIDs;
        *Ptr;
       )
     {
      /* Reset user */
      UsrDat->UsrCod = -1L;

      /* Find next string in text */
      Str_GetNextStringUntilSeparator (&Ptr,UsrDat->UsrIDNickOrEmail,
				       sizeof (UsrDat->UsrIDNickOrEmail) - 1);

      /* Reset default list of users' codes */
      ListUsrCods.NumUsrs = 0;
      ListUsrCods.Lst = NULL;

      /* Check if string is a user's ID, user's nickname or user's email address */
      switch (Nck_CheckIfNickWithArrIsValid (UsrDat->UsrIDNickOrEmail))
	{
	 case Err_SUCCESS:		// 1: It's a nickname
	    if ((UsrDat->UsrCod = Nck_GetUsrCodFromNickname (UsrDat->UsrIDNickOrEmail)) > 0)
	      {
	       ListUsrCods.NumUsrs = 1;
	       Usr_AllocateListUsrCods (&ListUsrCods);
	       ListUsrCods.Lst[0] = UsrDat->UsrCod;
	      }
	    break;
	 case Err_ERROR:
	 default:
	    switch (Mai_CheckIfEmailIsValid (UsrDat->UsrIDNickOrEmail))
	      {
	       case Err_SUCCESS:	// 2: It's an email
		  if ((UsrDat->UsrCod = Mai_DB_GetUsrCodFromEmail (UsrDat->UsrIDNickOrEmail)) > 0)
		    {
		     ListUsrCods.NumUsrs = 1;
		     Usr_AllocateListUsrCods (&ListUsrCods);
		     ListUsrCods.Lst[0] = UsrDat->UsrCod;
		    }
		  break;
	       case Err_ERROR:	// 3: It looks like a user's ID
	       default:
		  // Users' IDs are always stored internally in capitals and without leading zeros
		  Str_RemoveLeadingZeros (UsrDat->UsrIDNickOrEmail);
		  if (ID_CheckIfUsrIDSeemsAValidID (UsrDat->UsrIDNickOrEmail) == Err_SUCCESS)
		    {
		     /***** Find users for this user's ID *****/
		     ID_ReallocateListIDs (UsrDat,1);	// Only one user's ID
		     Str_Copy (UsrDat->IDs.List[0].ID,UsrDat->UsrIDNickOrEmail,
			       sizeof (UsrDat->IDs.List[0].ID) - 1);
		     Str_ConvertToUpperText (UsrDat->IDs.List[0].ID);
		     ID_GetListUsrCodsFromUsrID (UsrDat,NULL,&ListUsrCods,false);
		    }
		  break;
	      }
	    break;
	}

      if (RemoveSpecifiedUsrs)	// Remove the specified users (of the role)
	{
	 if (ListUsrCods.NumUsrs == 1)		// If more than one user found ==> do not remove
	    for (NumUsr = 0;
		 NumUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs;
		 NumUsr++)
	       if (Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].UsrCod == ListUsrCods.Lst[0])	// User found
		  Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Remove = true;	// Mark as removable
	}
      else	// Remove all users (of the role) except these specified
	{
	 for (NumUsr = 0;
	      NumUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs;
	      NumUsr++)
	    for (NumUsrFound = 0;
		 NumUsrFound < ListUsrCods.NumUsrs;
		 NumUsrFound++)
	       if (Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].UsrCod == ListUsrCods.Lst[NumUsrFound])	// User found
		  Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Remove = false;	// Mark as not removable
	}

      /* Free memory used for list of users' codes found for this ID */
      Usr_FreeListUsrCods (&ListUsrCods);
     }
  }

/*****************************************************************************/
/************** Update list of users to be removed          ******************/
/************** using list of users selected from clipboard ******************/
/*****************************************************************************/

static void Enr_UpdateLstUsrsToBeRemovedUsingSelectedUsrs (Rol_Role_t Role,
							   bool RemoveSpecifiedUsrs,
							   struct Usr_Data *UsrDat)
  {
   const char *Ptr;
   unsigned NumUsr;

   for (Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
        *Ptr;
       )
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,UsrDat->EnUsrCod,
				       Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (UsrDat);
      if (UsrDat->UsrCod > 0)
	 for (NumUsr = 0;
	      NumUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs;
	      NumUsr++)
	    if (Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].UsrCod == UsrDat->UsrCod)	// User found
	       Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Remove = RemoveSpecifiedUsrs;
     }
  }

/*****************************************************************************/
/****** Go through users list removing those users marked to be removed ******/
/*****************************************************************************/

static void Enr_RemoveUsrsMarkedToBeRemoved (Rol_Role_t Role,
					     bool EliminateUsrs,
					     struct Usr_Data *UsrDat,
					     struct ListCodGrps *LstGrps,
					     unsigned *NumUsrsRemoved)
  {
   unsigned NumUsr;

   /***** Loop 3: go through users list removing users *****/
   for (NumUsr = 0;
	NumUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs;
	NumUsr++)
      if (Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Remove)        // If this user is marked to be removed/eliminated
	{
	 UsrDat->UsrCod = Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].UsrCod;
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (UsrDat,
						      Usr_DONT_GET_PREFS,
						      Usr_DONT_GET_ROLE_IN_CRS) == Exi_EXISTS)
	   {	// User's data exist...
	    if (EliminateUsrs)			// Eliminate user completely from the platform
	      {
	       Acc_CompletelyEliminateAccount (UsrDat,Cns_QUIET);	// Remove definitely the user from the platform
	       (*NumUsrsRemoved)++;
	      }
	    else
	      {
	       if (Gbl.Crs.Grps.NumGrps)	// If there are groups in the course
		 {
		  if (LstGrps->NumGrps)		// If the teacher has selected groups
		    {
		     if (Grp_RemoveUsrFromGroups (UsrDat,LstGrps))	// Remove user from the selected groups, not from the whole course
			(*NumUsrsRemoved)++;
		    }
		  else		// The teacher has not selected groups
		    {
		     Enr_EffectivelyRemUsrFromCrs (UsrDat,&Gbl.Hierarchy.Node[Hie_CRS],
						   Enr_DO_NOT_REMOVE_USR_PRODUCTION,
						   Cns_QUIET);		// Remove user from the course
		     (*NumUsrsRemoved)++;
		    }
		 }
	       else		// No groups
		 {
		  Enr_EffectivelyRemUsrFromCrs (UsrDat,&Gbl.Hierarchy.Node[Hie_CRS],
						Enr_DO_NOT_REMOVE_USR_PRODUCTION,
						Cns_QUIET);		// Remove user from the course
		  (*NumUsrsRemoved)++;
		 }
	      }
	   }
	}
  }

/*****************************************************************************/
/********* Enrol users found in the form with IDs, nicks and emails **********/
/*****************************************************************************/

static void Enr_EnrolUsrsFoundInTextarea (Rol_Role_t Role,
					  char *ListUsrsIDs,
					  struct Usr_Data *UsrDat,
					  struct ListCodGrps *LstGrps,
					  unsigned *NumUsrsEnroled)
  {
   const char *Ptr;
   bool ItLooksLikeAUsrID;
   struct Usr_ListUsrCods ListUsrCods;	// List with users' codes for a given user's ID
   unsigned NumUsrFound;

   /***** Get users from a list of users' IDs ******/
   for (Ptr = ListUsrsIDs;
        *Ptr;
       )
     {
      /* Reset user */
      UsrDat->UsrCod = -1L;
      ItLooksLikeAUsrID = false;

      /* Find next string in text */
      Str_GetNextStringUntilSeparator (&Ptr,UsrDat->UsrIDNickOrEmail,
				       sizeof (UsrDat->UsrIDNickOrEmail) - 1);

      /* Reset default list of users' codes */
      ListUsrCods.NumUsrs = 0;
      ListUsrCods.Lst = NULL;

      /* Check if the string is a user's ID, a user's nickname or a user's email address */
      switch (Nck_CheckIfNickWithArrIsValid (UsrDat->UsrIDNickOrEmail))
	{
	 case Err_SUCCESS:		// 1: It's a nickname
	    if ((UsrDat->UsrCod = Nck_GetUsrCodFromNickname (UsrDat->UsrIDNickOrEmail)) > 0)
	      {
	       ListUsrCods.NumUsrs = 1;
	       Usr_AllocateListUsrCods (&ListUsrCods);
	       ListUsrCods.Lst[0] = UsrDat->UsrCod;
	      }
	    break;
	 case Err_ERROR:
	 default:
	    switch (Mai_CheckIfEmailIsValid (UsrDat->UsrIDNickOrEmail))
	      {
	       case Err_SUCCESS:	// 2: It's an email
		  if ((UsrDat->UsrCod = Mai_DB_GetUsrCodFromEmail (UsrDat->UsrIDNickOrEmail)) > 0)
		    {
		     ListUsrCods.NumUsrs = 1;
		     Usr_AllocateListUsrCods (&ListUsrCods);
		     ListUsrCods.Lst[0] = UsrDat->UsrCod;
		    }
		  break;
	       case Err_ERROR:		// 3: It looks like a user's ID
	       default:
		  // Users' IDs are always stored internally in capitals and without leading zeros
		  Str_RemoveLeadingZeros (UsrDat->UsrIDNickOrEmail);
		  if (ID_CheckIfUsrIDSeemsAValidID (UsrDat->UsrIDNickOrEmail) == Err_SUCCESS)
		    {
		     ItLooksLikeAUsrID = true;

		     /* Find users for this user's ID */
		     ID_ReallocateListIDs (UsrDat,1);	// Only one user's ID
		     Str_Copy (UsrDat->IDs.List[0].ID,UsrDat->UsrIDNickOrEmail,
			       sizeof (UsrDat->IDs.List[0].ID) - 1);
		     Str_ConvertToUpperText (UsrDat->IDs.List[0].ID);
		     ID_GetListUsrCodsFromUsrID (UsrDat,NULL,&ListUsrCods,false);
		    }
		  break;
	      }
	    break;
	}

      /* Enrol user(s) */
      if (ListUsrCods.NumUsrs)	// User(s) found
	 for (NumUsrFound = 0;
	      NumUsrFound < ListUsrCods.NumUsrs;
	      NumUsrFound++)
	   {
	    UsrDat->UsrCod = ListUsrCods.Lst[NumUsrFound];
	    Enr_EnrolUsr (UsrDat,Role,LstGrps,NumUsrsEnroled);
	   }
      else if (ItLooksLikeAUsrID)	// User not found. He/she is a new user. Enrol him/her using ID
	 Enr_EnrolUsr (UsrDat,Role,LstGrps,NumUsrsEnroled);

      /* Free memory used for list of users' codes found for this ID */
      Usr_FreeListUsrCods (&ListUsrCods);
     }
  }

/*****************************************************************************/
/******************** Enrol users selected from clipboard ********************/
/*****************************************************************************/

static void Enr_EnrolSelectedUsrs (Rol_Role_t Role,
				   struct Usr_Data *UsrDat,
				   struct ListCodGrps *LstGrps,
				   unsigned *NumUsrsEnroled)
  {
   const char *Ptr;

   for (Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
        *Ptr;
       )
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,UsrDat->EnUsrCod,
				       Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (UsrDat);
      if (UsrDat->UsrCod > 0)
	 Enr_EnrolUsr (UsrDat,Role,LstGrps,NumUsrsEnroled);
     }
  }

/*****************************************************************************/
/*********************** Write number of users removed ***********************/
/*****************************************************************************/

static void Enr_ShowMessageRemoved (unsigned NumUsrsRemoved,bool EliminateUsrs)
  {
   extern const char *Txt_No_user_has_been_eliminated;
   extern const char *Txt_No_user_has_been_removed;
   extern const char *Txt_One_user_has_been_eliminated;
   extern const char *Txt_One_user_has_been_removed;
   extern const char *Txt_X_users_have_been_eliminated;
   extern const char *Txt_X_users_have_been_removed;

   switch (NumUsrsRemoved)
     {
      case 0:
	 Ale_ShowAlert (Ale_INFO,EliminateUsrs ? Txt_No_user_has_been_eliminated :
						 Txt_No_user_has_been_removed);
	 break;
      case 1:
	 Ale_ShowAlert (Ale_SUCCESS,EliminateUsrs ? Txt_One_user_has_been_eliminated :
						    Txt_One_user_has_been_removed);
	 break;
      default:
	 Ale_ShowAlert (Ale_SUCCESS,EliminateUsrs ? Txt_X_users_have_been_eliminated :
						    Txt_X_users_have_been_removed,
			NumUsrsRemoved);
	 break;
     }
  }

/*****************************************************************************/
/*********************** Write number of users enroled ***********************/
/*****************************************************************************/

static void Enr_ShowMessageEnroled (unsigned NumUsrsEnroled)
  {
   extern const char *Txt_No_user_has_been_enroled;
   extern const char *Txt_One_user_has_been_enroled;
   extern const char *Txt_X_users_have_been_enroled_including_possible_repetitions;

   switch (NumUsrsEnroled)
     {
      case 0:
	 Ale_ShowAlert (Ale_INFO,Txt_No_user_has_been_enroled);
	 break;
      case 1:
	 Ale_ShowAlert (Ale_SUCCESS,Txt_One_user_has_been_enroled);
	 break;
      default:
	 Ale_ShowAlert (Ale_SUCCESS,Txt_X_users_have_been_enroled_including_possible_repetitions,
			NumUsrsEnroled);
	 break;
     }
  }

/*****************************************************************************/
/***** Put different actions to enrol/remove users to/from current course ****/
/*****************************************************************************/
// Returns true if at least one action can be shown

bool Enr_PutActionsEnrRemOneUsr (Usr_MeOrOther_t MeOrOther)
  {
   bool OptionsShown = false;
   Usr_Belong_t UsrBelongsToCrs = Usr_DONT_BELONG;
   bool UsrIsDegAdmin = false;
   bool UsrIsCtrAdmin = false;
   bool UsrIsInsAdmin = false;
   HTM_Attributes_t Attributes = HTM_NO_ATTR;

   /***** Check if the other user belongs to the current course *****/
   if (Gbl.Hierarchy.HieLvl == Hie_CRS)
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

      /***** Enrol user in course / Modify user's data *****/
      if (Gbl.Hierarchy.HieLvl == Hie_CRS && Gbl.Usrs.Me.Role.Logged >= Rol_STD)
	{
	 Enr_PutActionModifyOneUsr (&Attributes,UsrBelongsToCrs,MeOrOther);
	 OptionsShown = true;
	}

      if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
	{
	 if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)
	   {
	    if (Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0)
	       /***** Enrol user as administrator of degree *****/
	       if (!UsrIsDegAdmin && Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
		 {
		  Enr_PutActionEnrOneDegAdm (&Attributes);
		  OptionsShown = true;
		 }

	    /***** Enrol user as administrator of center *****/
	    if (!UsrIsCtrAdmin && Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
	      {
	       Enr_PutActionEnrOneCtrAdm (&Attributes);
	       OptionsShown = true;
	      }
	   }

	 /***** Enrol user as administrator of institution *****/
	 if (!UsrIsInsAdmin && Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	   {
	    Enr_PutActionEnrOneInsAdm (&Attributes);
	    OptionsShown = true;
	   }
	}

      /***** Copy user to clipboard *****/
      if (Gbl.Usrs.Me.Role.Logged >= Rol_GST)
        {
	 switch (Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs)
	   {
	    case Rol_STD:
	       Enr_PutActionAddToClipboard     (Enr_ADD_TO_CLIPBOARD_STD   ,&Attributes);
	       Enr_PutActionOverwriteClipboard (Enr_OVERWRITE_CLIPBOARD_OTH,&Attributes);
	       break;
	    case Rol_NET:
	    case Rol_TCH:
	       Enr_PutActionAddToClipboard     (Enr_ADD_TO_CLIPBOARD_TCH   ,&Attributes);
	       Enr_PutActionOverwriteClipboard (Enr_OVERWRITE_CLIPBOARD_TCH,&Attributes);
	       break;
	    default:
	       Enr_PutActionAddToClipboard     (Enr_ADD_TO_CLIPBOARD_OTH   ,&Attributes);
	       Enr_PutActionOverwriteClipboard (Enr_OVERWRITE_CLIPBOARD_OTH,&Attributes);
	       break;
	   }
	 OptionsShown = true;
	}

      /***** Report user as possible duplicate *****/
      if (MeOrOther == Usr_OTHER && Gbl.Usrs.Me.Role.Logged >= Rol_TCH)
	{
	 Enr_PutActionRepUsrAsDup (&Attributes);
	 OptionsShown = true;
	}

      /***** Remove user from the course *****/
      if (UsrBelongsToCrs == Usr_BELONG)
	{
	 Enr_PutActionRemUsrFromCrs (&Attributes,MeOrOther);
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
		  Enr_PutActionRemUsrAsDegAdm (&Attributes,MeOrOther);
		  OptionsShown = true;
		 }

	    /***** Remove user as an administrator of the center *****/
	    if (UsrIsCtrAdmin &&
		(MeOrOther == Usr_ME || Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM))
	      {
	       Enr_PutActionRemUsrAsCtrAdm (&Attributes,MeOrOther);
	       OptionsShown = true;
	      }
	   }

	 /***** Remove user as an administrator of the institution *****/
	 if (UsrIsInsAdmin &&
	     (MeOrOther == Usr_ME || Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM))
	   {
	    Enr_PutActionRemUsrAsInsAdm (&Attributes,MeOrOther);
	    OptionsShown = true;
	   }
	}

      /***** Eliminate user completely from platform *****/
      if (Acc_CheckIfICanEliminateAccount (Gbl.Usrs.Other.UsrDat.UsrCod) == Usr_CAN)
	{
	 Enr_PutActionRemUsrAcc (&Attributes,MeOrOther);
	 OptionsShown = true;
	}

   /***** End list of options *****/
   HTM_UL_End ();

   return OptionsShown;
  }

/*****************************************************************************/
/**************** Put action to modify user in current course ****************/
/*****************************************************************************/

static void Enr_PutActionModifyOneUsr (HTM_Attributes_t *Attributes,
                                       Usr_Belong_t UsrBelongsToCrs,
                                       Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Enrol_me;
   extern const char *Txt_Enrol_USER;
   extern const char *Txt_Modify_me;
   extern const char *Txt_Modify_user;
   const char *Txt[Usr_NUM_BELONG][Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_DONT_BELONG][Usr_ME   ] = Txt_Enrol_me,
      [Usr_DONT_BELONG][Usr_OTHER] = Txt_Enrol_USER,
      [Usr_BELONG     ][Usr_ME   ] = Txt_Modify_me,
      [Usr_BELONG     ][Usr_OTHER] = Txt_Modify_user,
     };

   Enr_EnrRemOneUsrActionBegin (Enr_ENROL_MODIFY_ONE_USR_IN_CRS,Attributes);
      HTM_Txt (Txt[UsrBelongsToCrs][MeOrOther]);
   Enr_EnrRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/****************** Put action to enrol user as degree admin *****************/
/*****************************************************************************/

static void Enr_PutActionEnrOneDegAdm (HTM_Attributes_t *Attributes)
  {
   extern const char *Txt_Enrol_USER_as_an_administrator_of_the_degree_X;

   Enr_EnrRemOneUsrActionBegin (Enr_ENROL_ONE_DEG_ADMIN,Attributes);
      HTM_TxtF (Txt_Enrol_USER_as_an_administrator_of_the_degree_X,
		Gbl.Hierarchy.Node[Hie_DEG].ShrtName);
   Enr_EnrRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/****************** Put action to enrol user as center admin *****************/
/*****************************************************************************/

static void Enr_PutActionEnrOneCtrAdm (HTM_Attributes_t *Attributes)
  {
   extern const char *Txt_Enrol_USER_as_an_administrator_of_the_center_X;

   Enr_EnrRemOneUsrActionBegin (Enr_ENROL_ONE_CTR_ADMIN,Attributes);
      HTM_TxtF (Txt_Enrol_USER_as_an_administrator_of_the_center_X,
		Gbl.Hierarchy.Node[Hie_CTR].ShrtName);
   Enr_EnrRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/*************** Put action to enrol user as institution admin ***************/
/*****************************************************************************/

static void Enr_PutActionEnrOneInsAdm (HTM_Attributes_t *Attributes)
  {
   extern const char *Txt_Enrol_USER_as_an_administrator_of_the_institution_X;

   Enr_EnrRemOneUsrActionBegin (Enr_ENROL_ONE_INS_ADMIN,Attributes);
      HTM_TxtF (Txt_Enrol_USER_as_an_administrator_of_the_institution_X,
		Gbl.Hierarchy.Node[Hie_INS].ShrtName);
   Enr_EnrRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/****************** Put action to add user to clipboard **********************/
/*****************************************************************************/

static void Enr_PutActionAddToClipboard (Enr_EnrRemOneUsrAction_t EnrRemOneUsrAction,
					 HTM_Attributes_t *Attributes)
  {
   extern const char *Txt_Add_to_clipboard;

   Enr_EnrRemOneUsrActionBegin (EnrRemOneUsrAction,Attributes);
      HTM_Txt (Txt_Add_to_clipboard);
   Enr_EnrRemOneUsrActionEnd ();
  }

static void Enr_PutActionOverwriteClipboard (Enr_EnrRemOneUsrAction_t EnrRemOneUsrAction,
					     HTM_Attributes_t *Attributes)
  {
   extern const char *Txt_Overwrite_clipboard;

   Enr_EnrRemOneUsrActionBegin (EnrRemOneUsrAction,Attributes);
      HTM_Txt (Txt_Overwrite_clipboard);
   Enr_EnrRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/****************** Put action to report user as duplicate *******************/
/*****************************************************************************/

static void Enr_PutActionRepUsrAsDup (HTM_Attributes_t *Attributes)
  {
   extern const char *Txt_Report_possible_duplicate_user;

   Enr_EnrRemOneUsrActionBegin (Enr_REPORT_USR_AS_POSSIBLE_DUPLICATE,Attributes);
      HTM_Txt (Txt_Report_possible_duplicate_user);
   Enr_EnrRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/****************** Put action to remove user from course ********************/
/*****************************************************************************/

static void Enr_PutActionRemUsrFromCrs (HTM_Attributes_t *Attributes,
					Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Remove_me_from_THE_COURSE_X;
   extern const char *Txt_Remove_USER_from_THE_COURSE_X;
   const char *Txt[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Remove_me_from_THE_COURSE_X,
      [Usr_OTHER] = Txt_Remove_USER_from_THE_COURSE_X,
     };

   Enr_EnrRemOneUsrActionBegin (Enr_REMOVE_ONE_USR_FROM_CRS,Attributes);
      HTM_TxtF (Txt[MeOrOther],Gbl.Hierarchy.Node[Hie_CRS].ShrtName);
   Enr_EnrRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/***************** Put action to remove user as degree admin *****************/
/*****************************************************************************/

static void Enr_PutActionRemUsrAsDegAdm (HTM_Attributes_t *Attributes,
					 Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Remove_me_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Remove_USER_as_an_administrator_of_the_degree_X;
   const char *Txt[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Remove_me_as_an_administrator_of_the_degree_X,
      [Usr_OTHER] = Txt_Remove_USER_as_an_administrator_of_the_degree_X,
     };

   Enr_EnrRemOneUsrActionBegin (Enr_REMOVE_ONE_DEG_ADMIN,Attributes);
      HTM_TxtF (Txt[MeOrOther],Gbl.Hierarchy.Node[Hie_DEG].ShrtName);
   Enr_EnrRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/***************** Put action to remove user as center admin *****************/
/*****************************************************************************/

static void Enr_PutActionRemUsrAsCtrAdm (HTM_Attributes_t *Attributes,
					 Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Remove_me_as_an_administrator_of_the_center_X;
   extern const char *Txt_Remove_USER_as_an_administrator_of_the_center_X;
   const char *Txt[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Remove_me_as_an_administrator_of_the_center_X,
      [Usr_OTHER] = Txt_Remove_USER_as_an_administrator_of_the_center_X,
     };

   Enr_EnrRemOneUsrActionBegin (Enr_REMOVE_ONE_CTR_ADMIN,Attributes);
      HTM_TxtF (Txt[MeOrOther],Gbl.Hierarchy.Node[Hie_CTR].ShrtName);
   Enr_EnrRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/************** Put action to remove user as institution admin ***************/
/*****************************************************************************/

static void Enr_PutActionRemUsrAsInsAdm (HTM_Attributes_t *Attributes,
					 Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Remove_me_as_an_administrator_of_the_institution_X;
   extern const char *Txt_Remove_USER_as_an_administrator_of_the_institution_X;
   const char *Txt[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Remove_me_as_an_administrator_of_the_institution_X,
      [Usr_OTHER] = Txt_Remove_USER_as_an_administrator_of_the_institution_X,
     };

   Enr_EnrRemOneUsrActionBegin (Enr_REMOVE_ONE_INS_ADMIN,Attributes);
      HTM_TxtF (Txt[MeOrOther],Gbl.Hierarchy.Node[Hie_INS].ShrtName);
   Enr_EnrRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/********************* Put action to remove user account *********************/
/*****************************************************************************/

static void Enr_PutActionRemUsrAcc (HTM_Attributes_t *Attributes,
				    Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Eliminate_my_user_account;
   extern const char *Txt_Eliminate_user_account;
   const char *Txt[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Eliminate_my_user_account,
      [Usr_OTHER] = Txt_Eliminate_user_account,
     };

   Enr_EnrRemOneUsrActionBegin (Enr_ELIMINATE_ONE_USR_FROM_PLATFORM,Attributes);
      HTM_Txt (Txt[MeOrOther]);
   Enr_EnrRemOneUsrActionEnd ();
  }

/*****************************************************************************/
/************** Put start/end of action to enrol/remove one user *************/
/*****************************************************************************/

static void Enr_EnrRemOneUsrActionBegin (Enr_EnrRemOneUsrAction_t EnrRemOneUsrAction,
                                         HTM_Attributes_t *Attributes)
  {
   HTM_LI_Begin (NULL);
      HTM_LABEL_Begin (NULL);
	 HTM_INPUT_RADIO ("EnrRemAction",
			  (*Attributes & HTM_CHECKED) ? HTM_NO_ATTR :
							HTM_CHECKED,
			  "value=\"%u\"",(unsigned) EnrRemOneUsrAction);
	    *Attributes = HTM_CHECKED;
  }

static void Enr_EnrRemOneUsrActionEnd (void)
  {
      HTM_LABEL_End ();
   HTM_LI_End ();
  }

/*****************************************************************************/
/************************ Enrol a user using his/her ID **********************/
/*****************************************************************************/
// If user does not exists, UsrDat->IDs must hold the user's ID

static void Enr_EnrolUsr (struct Usr_Data *UsrDat,Rol_Role_t Role,
                          struct ListCodGrps *LstGrps,unsigned *NumUsrsEnroled)
  {
   /***** Check if I can enrol this user *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_TCH &&	// A teacher only can enrol several users...
       !(Role == Rol_STD ||			// ...as students...
	 Role == Rol_NET))			// ...or non-editing teachers
      Err_NoPermissionExit ();

   /***** Check if the record of the user exists and get the type of user *****/
   if (UsrDat->UsrCod > 0)	// User exists in database
      /* Get user's data */
      Usr_GetAllUsrDataFromUsrCod (UsrDat,
                                   Usr_DONT_GET_PREFS,
                                   Usr_GET_ROLE_IN_CRS);
   else				// User does not exist in database, create it using his/her ID!
     {
      /* Reset user's data */
      Usr_ResetUsrDataExceptUsrCodAndIDs (UsrDat);	// It's necessary, because the same struct UsrDat was used for former user

      /* User does not exist in database; list of IDs is initialized */
      UsrDat->IDs.List[0].Confirmed = ID_CONFIRMED;	// If he/she is a new user ==> his/her ID will be stored as confirmed in database
      Acc_CreateNewUsr (UsrDat,Usr_OTHER);
     }

   /***** Enrol user in current course in database *****/
   if (Gbl.Hierarchy.HieLvl == Hie_CRS)	// Course selected
     {
      switch (Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat))
	{
         case Usr_BELONG:
	    if (Role != UsrDat->Roles.InCurrentCrs)	// The role must be updated
	       /* Modify role */
	       Enr_ModifyRoleInCurrentCrs (UsrDat,Role);
	    break;
         case Usr_DONT_BELONG:	// User does not belong to this course
         default:
	    /* Enrol user */
	    Enr_EnrolUsrInCurrentCrs (UsrDat,Role,
				      Enr_SET_ACCEPTED_TO_FALSE);
	    break;
	}

      /***** Enrol user in the selected groups *****/
      if (Gbl.Crs.Grps.NumGrps)	// If there are groups in the course
	 Grp_EnrolUsrIntoGroups (UsrDat,Role,LstGrps);
     }

   (*NumUsrsEnroled)++;
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
   Box_BoxBegin (Txt_Remove_all_students,NULL,NULL,
                 Hlp_USERS_Administration_remove_all_students,Box_NOT_CLOSABLE);

      if (NumStds)
	{
	 /***** Show question and button to remove students *****/
	 /* Begin alert */
	 Ale_ShowAlertAndButtonBegin (Ale_QUESTION,
				      Txt_Do_you_really_want_to_remove_the_X_students_from_the_course_Y_,
				      NumStds,Gbl.Hierarchy.Node[Hie_CRS].FullName);

	 /* Show form to request confirmation */
	 Frm_BeginForm (ActRemAllStdCrs);
	    Grp_PutParAllGroups ();
	    Pwd_AskForConfirmationOnDangerousAction ();
	    Btn_PutButton (Btn_REMOVE,NULL);
	 Frm_EndForm ();

	 /* End alert */
	 Ale_ShowAlertAndButtonEnd (ActUnk,NULL,NULL,NULL,NULL,Btn_NO_BUTTON);
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

   if (Pwd_GetConfirmationOnDangerousAction () == Err_SUCCESS)
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
   Gbl.Crs.Grps.AllGrpsSel = true;        // Get all students of the current course
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
                                   long ReqCod,Ntf_GetContent_t GetContent)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct Usr_Data UsrDat;
   __attribute__((unused)) Exi_Exist_t UsrExists;
   Rol_Role_t DesiredRole;

   SummaryStr[0] = '\0';        // Return nothing on error

   /***** Get user and requested role from database *****/
   if (Enr_DB_GetEnrolmentRequestByCod (&mysql_res,ReqCod) == Exi_EXISTS)
     {
      /***** Get user and requested role *****/
      row = mysql_fetch_row (mysql_res);

      /* Initialize structure with user's data */
      Usr_UsrDataConstructor (&UsrDat);

      /* User's code (row[0]) */
      UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
      UsrExists = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							   Usr_DONT_GET_PREFS,
							   Usr_DONT_GET_ROLE_IN_CRS);

      /* Role (row[1]) */
      DesiredRole = Rol_ConvertUnsignedStrToRole (row[1]);
      Str_Copy (SummaryStr,Txt_ROLES_SINGUL_Abc[DesiredRole][UsrDat.Sex],
		Ntf_MAX_BYTES_SUMMARY);

      if (GetContent == Ntf_GET_CONTENT)
	 /* Write desired role into content */
	 if (asprintf (ContentStr,"%s",	// TODO: Write more info in this content
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
   Rol_Role_t Role;

   /***** Get user's code *****/
   Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();

   switch (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                Usr_DONT_GET_PREFS,
                                                Usr_DONT_GET_ROLE_IN_CRS))
     {
      case Exi_EXISTS:
	 // User's data exist...
	 switch (Enr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
	   {
	    case Usr_BELONG:
	       /* User already belongs to this course */
	       Ale_ShowAlert (Ale_WARNING,Txt_THE_USER_X_is_already_enroled_in_the_course_Y,
			      Gbl.Usrs.Other.UsrDat.FullName,
			      Gbl.Hierarchy.Node[Hie_CRS].FullName);
	       Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

	       /* Remove inscription request because it has not sense */
	       Enr_RemUsrEnrolmentRequestInCrs (Gbl.Usrs.Other.UsrDat.UsrCod,
						Gbl.Hierarchy.Node[Hie_CRS].HieCod);
	       break;
	    case Usr_DONT_BELONG:        // User does not belong to this course
	    default:
	       Role = Rol_DB_GetRequestedRole (Gbl.Hierarchy.Node[Hie_CRS].HieCod,
					       Gbl.Usrs.Other.UsrDat.UsrCod);
	       if (Role == Rol_STD ||
		   Role == Rol_NET ||
		   Role == Rol_TCH)
		 {
		  /***** Show question and button to reject user's enrolment request *****/
		  /* Begin alert */
		  Ale_ShowAlertAndButtonBegin (Ale_QUESTION,Txt_Do_you_really_want_to_reject_the_enrolment_request_,
					       Gbl.Usrs.Other.UsrDat.FullName,
					       Txt_ROLES_SINGUL_abc[Role][Gbl.Usrs.Other.UsrDat.Sex],
					       Gbl.Hierarchy.Node[Hie_CRS].FullName);

		     /* Show user's record */
		     Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

		  /* End alert */
		  Ale_ShowAlertAndButtonEnd (ActRejSignUp,NULL,NULL,
					     Usr_PutParOtherUsrCodEncrypted,Gbl.Usrs.Other.UsrDat.EnUsrCod,
					     Btn_REJECT);
		 }
	       else
		  Err_WrongRoleExit ();
	       break;
	   }
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
     }
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

   switch (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                    Usr_DONT_GET_PREFS,
                                                    Usr_DONT_GET_ROLE_IN_CRS))
     {
      case Exi_EXISTS:
	 // User's data exist...
	 if (Enr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat) == Usr_BELONG)
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
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
     }

   /* Show again the rest of enrolment requests */
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
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Hlp_USERS_Requests;
   extern const char *Txt_Enrolment_requests;
   extern const char *Txt_Scope;
   extern const char *Txt_Users;
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Requester;
   extern const char *Txt_Role;
   extern const char *Txt_Date;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
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
   unsigned AllowedLvls;
   Hie_Level_t HieLvl;
   unsigned NumReqs;
   unsigned NumReq;
   long ReqCod;
   struct Hie_Node Deg;
   struct Hie_Node Crs;
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;
   struct Usr_Data UsrDat;
   Exi_Exist_t UsrExists;
   Usr_Belong_t UsrBelongsToCrs;
   Rol_Role_t DesiredRole;
   static Act_Action_t NextAction[Rol_NUM_ROLES] =
     {
      [Rol_STD] = ActReqMdfStd,
      [Rol_NET] = ActReqMdfNET,
      [Rol_TCH] = ActReqMdfTch,
     };

   /***** Remove expired enrolment requests *****/
   Enr_DB_RemoveExpiredEnrolmentRequests ();

   /***** Get scope *****/
   AllowedLvls = 1 << Hie_SYS |
		 1 << Hie_CTY |
		 1 << Hie_INS |
		 1 << Hie_CTR |
		 1 << Hie_DEG |
		 1 << Hie_CRS;
   HieLvl = Sco_GetScope ("ScopeEnr",Hie_CRS,AllowedLvls);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Enrolment_requests,NULL,NULL,
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
		  Sco_PutSelectorScope ("ScopeEnr",HTM_SUBMIT_ON_CHANGE,
					HieLvl,AllowedLvls);
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
					  HTM_SUBMIT_ON_CHANGE);
	       HTM_TD_End ();

	    HTM_TR_End ();

	 /* End table and form */
	 HTM_TABLE_End ();
      Frm_EndForm ();

      /***** List requests *****/
      if ((NumReqs = Enr_DB_GetEnrolmentRequests (&mysql_res,HieLvl,RolesSelected)))
	 {
	 /* Initialize structure with user's data */
	 Usr_UsrDataConstructor (&UsrDat);

	 /* Begin table */
	 HTM_TABLE_BeginCenterPadding (2);

	    /* Table heading */
	    HTM_TR_Begin (NULL);
	       HTM_TH_Empty (1);
	       HTM_TH      (Txt_HIERARCHY_SINGUL_Abc[Hie_CRS]         ,HTM_HEAD_LEFT  );
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
								    Usr_DONT_GET_ROLE_IN_CRS);

	       /***** Get requested role (row[3]) *****/
	       DesiredRole = Rol_ConvertUnsignedStrToRole (row[3]);

	       switch (UsrExists)
		 {
		  case Exi_EXISTS:
		     UsrBelongsToCrs = Hie_CheckIfUsrBelongsTo (Hie_CRS,
								UsrDat.UsrCod,
								Crs.HieCod,
								false);
		     break;
		  case Exi_DOES_NOT_EXIST:
		  default:
		     UsrBelongsToCrs = Usr_DONT_BELONG;
		     break;
		 }

	       if (UsrExists == Exi_EXISTS &&
		   UsrBelongsToCrs == Usr_DONT_BELONG &&
		   (DesiredRole == Rol_STD ||
		    DesiredRole == Rol_NET ||
		    DesiredRole == Rol_TCH))
		 {
		  HTM_TR_Begin (NULL);

		     /***** Number *****/
		     HTM_TD_Begin ("class=\"RT DAT_%s\"",The_GetSuffix ());
			HTM_Unsigned (NumReqs - NumReq);
		     HTM_TD_End ();

		     /***** Link to course *****/
		     HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());

			SuccessOrError = Hie_GetDataByCod[Hie_CRS] (&Crs);
			Deg.HieCod = Crs.PrtCod;
			SuccessOrError = Hie_GetDataByCod[Hie_DEG] (&Deg);

			Frm_BeginFormGoTo (ActSeeCrsInf);
			   ParCod_PutPar (ParCod_Crs,Crs.HieCod);
			   HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Crs.FullName),NULL,
						    "class=\"LT BT_LINK\"");
			   Str_FreeGoToTitle ();
			      HTM_Txt (Deg.ShrtName);
			      HTM_SP (); HTM_GT (); HTM_SP ();
			      HTM_Txt (Crs.ShrtName);
			   HTM_BUTTON_End ();
			Frm_EndForm ();

		     HTM_TD_End ();

		     /***** Number of teachers in the course *****/
		     HTM_TD_Begin ("class=\"RT DAT_%s\"",The_GetSuffix ());
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
		     HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
			HTM_DIV_Begin ("class=\"REQUESTER_NAME\"");	// Limited width
			   Usr_WriteFirstNameBRSurnames (&UsrDat);
			HTM_DIV_End ();
		     HTM_TD_End ();

		     /***** Requested role (row[3]) *****/
		     HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
			HTM_Txt (Txt_ROLES_SINGUL_abc[DesiredRole][UsrDat.Sex]);
		     HTM_TD_End ();

		     /***** Request time (row[4]) *****/
		     Msg_WriteMsgDate (Dat_GetUNIXTimeFromStr (row[4]),
		                       "DAT",The_GetColorRows ());

		     /***** Button to confirm the request *****/
		     HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
			if (!NextAction[DesiredRole])
			   Err_WrongRoleExit ();
			Frm_BeginForm (NextAction[DesiredRole]);
			   ParCod_PutPar (ParCod_Crs,Crs.HieCod);
			   Usr_PutParUsrCodEncrypted (UsrDat.EnUsrCod);
			   Btn_PutButtonInline (Btn_ENROL);
			Frm_EndForm ();
		     HTM_TD_End ();

		     /***** Button to reject the request *****/
		     HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
			Frm_BeginForm (ActReqRejSignUp);
			   ParCod_PutPar (ParCod_Crs,Crs.HieCod);
			   Usr_PutParUsrCodEncrypted (UsrDat.EnUsrCod);
			   Btn_PutButtonInline (Btn_REJECT);
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
   extern const char *Txt_Administer_me;
   extern const char *Txt_Administer_one_user;
   static const char **TitleText[] =
     {
      [Usr_CAN_NOT] = &Txt_Administer_me,
      [Usr_CAN    ] = &Txt_Administer_one_user,
     };

   Lay_PutContextualLinkIconText (NextAction,NULL,
                                  NULL,NULL,
				  "user-gear.svg",Ico_BLACK,
				  *TitleText[Adm_CheckIfICanAdminOtherUsrs ()],NULL);
  }

/*****************************************************************************/
/******************* Write a form to admin several users *********************/
/*****************************************************************************/

void Enr_PutLinkToAdminSeveralUsrs (Rol_Role_t Role)
  {
   extern const char *Txt_Administer_multiple_students;
   extern const char *Txt_Administer_multiple_non_editing_teachers;
   extern const char *Txt_Administer_multiple_teachers;
   static struct
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
				  "users-gear.svg",Ico_BLACK,
				  *WhatToDo[Role].Title,NULL);
  }

/*****************************************************************************/
/************** Form to request the user's ID of another user ****************/
/*****************************************************************************/

void Enr_ReqEnrRemOth (void)
  {
   /***** Form to request user to be administered *****/
   Enr_ReqEnrRemUsr (Rol_GST);
  }

void Enr_ReqEnrRemStd (void)
  {
   /***** Contextual menu *****/
   if (Adm_CheckIfICanAdminOtherUsrs () == Usr_CAN)
     {
      Mnu_ContextMenuBegin ();
	 Enr_PutLinkToAdminSeveralUsrs (Rol_STD);	// Admin several students
      Mnu_ContextMenuEnd ();
     }

   /***** Form to request user to be administered *****/
   Enr_ReqEnrRemUsr (Rol_STD);
  }

void Enr_ReqEnrRemTch (void)
  {
   /***** Form to request user to be administered *****/
   Enr_ReqEnrRemUsr (Rol_TCH);
  }

static void Enr_ReqEnrRemUsr (Rol_Role_t Role)
  {
   switch (Adm_CheckIfICanAdminOtherUsrs ())
     {
      case Usr_CAN:
	 /***** Form to request the user's ID of another user *****/
	 Enr_ReqAnotherUsrIDToEnrolRemove (Role);
	 break;
      case Usr_CAN_NOT:
      default:
	 /***** Form to request if enrol/remove me *****/
	 Enr_AskIfEnrRemMe (Role);
	 break;
     }
  }

/*****************************************************************************/
/****** Write a form to request another user's ID, @nickname or email ********/
/*****************************************************************************/

static void Enr_ReqAnotherUsrIDToEnrolRemove (Rol_Role_t Role)
  {
   extern const char *Hlp_USERS_Administration_administer_one_user;
   extern const char *Txt_Administer_one_user;
   static Act_Action_t NextAction[Rol_NUM_ROLES] =
     {
      [Rol_GST] = ActReqMdfOth,
      [Rol_STD] = ActReqMdfStd,
      [Rol_NET] = ActReqMdfNET,
      [Rol_TCH] = ActReqMdfTch,
     };

   /***** Begin box *****/
   Box_BoxBegin (Txt_Administer_one_user,NULL,NULL,
                 Hlp_USERS_Administration_administer_one_user,Box_NOT_CLOSABLE);

      /***** Write form to request another user's ID *****/
      if (!NextAction[Role])
	 Err_WrongRoleExit ();
      Enr_WriteFormToReqAnotherUsrID (NextAction[Role],NULL);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************************ Ask me for enrol/remove me *************************/
/*****************************************************************************/

static void Enr_AskIfEnrRemMe (Rol_Role_t Role)
  {
   struct Usr_ListUsrCods ListUsrCods;

   /***** I only can admin me *****/
   Gbl.Usrs.Other.UsrDat.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   ListUsrCods.NumUsrs = 1;
   Usr_AllocateListUsrCods (&ListUsrCods);
   ListUsrCods.Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;

   Enr_AskIfEnrRemUsr (&ListUsrCods,Role);
  }

/*****************************************************************************/
/******************** Ask me for enrol/remove another user *******************/
/*****************************************************************************/

void Enr_AskIfEnrRemAnotherOth (void)
  {
   Enr_AskIfEnrRemAnotherUsr (Rol_GST);
  }

void Enr_AskIfEnrRemAnotherStd (void)
  {
   Enr_AskIfEnrRemAnotherUsr (Rol_STD);
  }

void Enr_AskIfEnrRemAnotherTch (void)
  {
   Enr_AskIfEnrRemAnotherUsr (Rol_TCH);
  }

static void Enr_AskIfEnrRemAnotherUsr (Rol_Role_t Role)
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

   Enr_AskIfEnrRemUsr (&ListUsrCods,Role);
  }

/*****************************************************************************/
/************************ Ask me for enrol/remove user ***********************/
/*****************************************************************************/

static void Enr_AskIfEnrRemUsr (struct Usr_ListUsrCods *ListUsrCods,Rol_Role_t Role)
  {
   extern const char *Txt_There_are_X_users_with_the_ID_Y;
   extern const char *Txt_The_user_is_new_not_yet_in_X;
   extern const char *Txt_If_this_is_a_new_user_in_X_you_should_indicate_her_his_ID;
   unsigned NumUsr;
   bool UsrIDValid;

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
                                      Usr_GET_ROLE_IN_CRS);

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
         UsrIDValid = (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID) == Err_SUCCESS);	// Check the first element of the list
      else
	 UsrIDValid = false;

      if (UsrIDValid)
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
	 Enr_ReqEnrRemUsr (Role);
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
   static const char **Usr_AcceptedTxt[Usr_NUM_ACCEPTED] =
     {
      [Usr_HAS_NOT_ACCEPTED] = &Txt_THE_USER_X_is_in_the_course_Y_but_has_not_yet_accepted_the_enrolment,
      [Usr_HAS_ACCEPTED    ] = &Txt_THE_USER_X_is_already_enroled_in_the_course_Y,
     };

   /***** If user exists... *****/
   switch (Usr_DB_ChkIfUsrCodExists (Gbl.Usrs.Other.UsrDat.UsrCod))
     {
      case Exi_EXISTS:
	 /***** Show form to edit user *****/
	 if (Gbl.Hierarchy.HieLvl == Hie_CRS)	// Course selected
	    /* Check if this user belongs to the current course */
	    switch (Enr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
	      {
	       case Usr_BELONG:
		  Gbl.Usrs.Other.UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
		  Ale_ShowAlert (Ale_INFO,*Usr_AcceptedTxt[Gbl.Usrs.Other.UsrDat.Accepted],
				 Gbl.Usrs.Other.UsrDat.FullName,
				 Gbl.Hierarchy.Node[Hie_CRS].FullName);
		  break;
	       case Usr_DONT_BELONG:	// User does not belong to the current course
	       default:
		  Ale_ShowAlert (Ale_INFO,Txt_THE_USER_X_exists_in_Y_but_is_not_enroled_in_the_course_Z,
				 Gbl.Usrs.Other.UsrDat.FullName,
				 Cfg_PLATFORM_SHORT_NAME,
				 Gbl.Hierarchy.Node[Hie_CRS].FullName);
		  break;
	      }
	 else	// No course selected
	    Ale_ShowAlert (Ale_INFO,Txt_THE_USER_X_already_exists_in_Y,
			   Gbl.Usrs.Other.UsrDat.FullName,Cfg_PLATFORM_SHORT_NAME);
	 Rec_ShowOtherSharedRecordEditable ();
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
     }
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
   switch (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
     {
      case Exi_EXISTS:
	 switch (Enr_CheckIfICanRemUsrFromCrs ())
	   {
	    case Usr_CAN:
	       Enr_AskIfRemoveUsrFromCrs (&Gbl.Usrs.Other.UsrDat);
	       break;
	    case Usr_CAN_NOT:
	    default:
	       Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	       break;
	   }
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
     }
  }

/*****************************************************************************/
/********************* Remove a user from current course *********************/
/*****************************************************************************/

void Enr_RemUsrFromCrs1 (void)
  {
   if (Pwd_GetConfirmationOnDangerousAction () == Err_SUCCESS)
      /***** Get user to be removed *****/
      switch (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
	{
	 case Exi_EXISTS:
	    switch (Enr_CheckIfICanRemUsrFromCrs ())
	      {
	       case Usr_CAN:
		  Enr_EffectivelyRemUsrFromCrs (&Gbl.Usrs.Other.UsrDat,
						&Gbl.Hierarchy.Node[Hie_CRS],
						Enr_DO_NOT_REMOVE_USR_PRODUCTION,
						Cns_VERBOSE);
		  break;
	       case Usr_CAN_NOT:
	       default:
		  Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
		  break;
	      }
	    break;
	 case Exi_DOES_NOT_EXIST:
	 default:
	    Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
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

static Usr_Can_t Enr_CheckIfICanRemUsrFromCrs (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
	 // A student or non-editing teacher can remove herself/himself
	 return (Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod) == Usr_ME) ? Usr_CAN :
								       Usr_CAN_NOT;
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 // A teacher or administrator can remove anyone
	 return Usr_CAN;
      default:
	 return Usr_CAN_NOT;
     }
  }

/*****************************************************************************/
/*************** Accept my enrolment in the current course ******************/
/*****************************************************************************/

void Enr_AcceptEnrolMeInCrs (void)
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
   static Act_Action_t Action[Rol_NUM_ROLES] =
     {
      [Rol_GST] = ActCreOth,
      [Rol_STD] = ActCreStd,
      [Rol_NET] = ActCreNET,
      [Rol_TCH] = ActCreTch,
     };

   /***** Get user's ID from form *****/
   ID_GetParOtherUsrIDPlain ();	// User's ID was already modified and passed as a hidden parameter

   switch (ID_CheckIfUsrIDIsValid (Gbl.Usrs.Other.UsrDat.IDs.List[0].ID))
     {
      case Err_SUCCESS:	// User's ID valid
	 Gbl.Usrs.Other.UsrDat.UsrCod = -1L;

	 /***** Get new role *****/
	 NewRole = Rec_GetRoleFromRecordForm ();

	 /***** Get user's name from form *****/
	 Rec_GetUsrNameFromRecordForm (&Gbl.Usrs.Other.UsrDat);

	 /***** Create user *****/
	 Gbl.Usrs.Other.UsrDat.IDs.List[0].Confirmed = ID_CONFIRMED;	// User's ID will be stored as confirmed
	 Acc_CreateNewUsr (&Gbl.Usrs.Other.UsrDat,Usr_OTHER);

	 /***** Enrol user in current course in database *****/
	 if (Gbl.Hierarchy.HieLvl == Hie_CRS)	// Course selected
	   {
	    switch (Enr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
	      {
	       case Usr_BELONG:
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
		  break;
	       case Usr_DONT_BELONG:	// User does not belong to current course
	       default:
		  /* Enrol user */
		  Enr_EnrolUsrInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole,
					    Enr_SET_ACCEPTED_TO_FALSE);

		  /* Success message */
		  Ale_CreateAlert (Ale_SUCCESS,NULL,
				   Txt_THE_USER_X_has_been_enroled_in_the_course_Y,
				   Gbl.Usrs.Other.UsrDat.FullName,
				   Gbl.Hierarchy.Node[Hie_CRS].FullName);
		  break;
	      }

	    /***** Change user's groups *****/
	    if (Gbl.Crs.Grps.NumGrps)	// This course has groups?
	       Grp_ChangeUsrGrps (Usr_OTHER,Cns_QUIET);
	   }

	 /***** Change current action *****/
	 if (!Action[NewRole])
	    Err_WrongRoleExit ();
	 Gbl.Action.Act = Action[NewRole];
	 Tab_SetCurrentTab ();
	 break;
      case Err_ERROR:	// User's ID not valid
      default:
	 /***** Error message *****/
	 Ale_CreateAlert (Ale_ERROR,NULL,
			  Txt_The_ID_X_is_not_valid,
			  Gbl.Usrs.Other.UsrDat.IDs.List[0].ID);
	 break;
     }
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
/****** Modify other user's data and enrol her/him in course and groups ******/
/*****************************************************************************/

static void Enr_SetEnrRemAction (Enr_EnrRemOneUsrAction_t EnrRemAction)
  {
   Enr_EnrRemAction = EnrRemAction;
  }

static Enr_EnrRemOneUsrAction_t Enr_GetEnrRemAction (void)
  {
   return Enr_EnrRemAction;
  }

void Enr_ModifyUsr1 (void)
  {
   extern const char *Txt_The_role_of_THE_USER_X_in_the_course_Y_has_changed_from_A_to_B;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_THE_USER_X_has_been_enroled_in_the_course_Y;
   static Act_Action_t Action[Rol_NUM_ROLES] =
     {
      [Rol_GST] = ActUpdOth,
      [Rol_STD] = ActUpdStd,
      [Rol_NET] = ActUpdNET,
      [Rol_TCH] = ActUpdTch,
     };
   Usr_MeOrOther_t MeOrOther;
   Enr_EnrRemOneUsrAction_t EnrRemAction;
   Rol_Role_t OldRole;
   Rol_Role_t NewRole;

   /***** Get user from form *****/
   switch (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
     {
      case Exi_EXISTS:
	 MeOrOther = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);

	 /***** Get the action to do *****/
	 EnrRemAction = (Enr_EnrRemOneUsrAction_t)
			Par_GetParUnsignedLong ("EnrRemAction",
						0,
						Enr_ENR_REM_ONE_USR_NUM_ACTIONS - 1,
						(unsigned long) Enr_ENR_REM_ONE_USR_UNKNOWN_ACTION);
	 Enr_SetEnrRemAction (EnrRemAction);
	 switch (EnrRemAction)
	   {
	    case Enr_ENROL_MODIFY_ONE_USR_IN_CRS:
	       if (MeOrOther == Usr_ME || Gbl.Usrs.Me.Role.Logged >= Rol_TCH)
		 {
		  /***** Get user's name from record form *****/
		  if (Usr_ICanChangeOtherUsrData (&Gbl.Usrs.Other.UsrDat) == Usr_CAN)
		     Rec_GetUsrNameFromRecordForm (&Gbl.Usrs.Other.UsrDat);

		  /***** Update user's data in database *****/
		  Enr_UpdateUsrData (&Gbl.Usrs.Other.UsrDat);

		  if (Gbl.Hierarchy.HieLvl == Hie_CRS)	// Course selected
		    {
		     /***** Get new role from record form *****/
		     NewRole = Rec_GetRoleFromRecordForm ();

		     /***** Enrol user in current course in database *****/
		     switch (Enr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
		       {
			case Usr_BELONG:
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
			   break;
			case Usr_DONT_BELONG:	// User does not belong to current course
			default:
			   /* Enrol user */
			   Enr_EnrolUsrInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole,
						     Enr_SET_ACCEPTED_TO_FALSE);

			   /* Set success message */
			   Ale_CreateAlert (Ale_SUCCESS,NULL,
					    Txt_THE_USER_X_has_been_enroled_in_the_course_Y,
					    Gbl.Usrs.Other.UsrDat.FullName,
					    Gbl.Hierarchy.Node[Hie_CRS].FullName);
			   break;
		       }

		     /***** Change user's groups *****/
		     if (Gbl.Crs.Grps.NumGrps)	// This course has groups?
			Grp_ChangeUsrGrps (MeOrOther,Cns_VERBOSE);

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
	    case Enr_ENROL_ONE_DEG_ADMIN:
	       if (Gbl.Usrs.Me.Role.Logged < Rol_CTR_ADM)
		  Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	       break;
	    case Enr_ENROL_ONE_CTR_ADMIN:
	       if (Gbl.Usrs.Me.Role.Logged < Rol_INS_ADM)
		  Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	       break;
	    case Enr_ENROL_ONE_INS_ADMIN:
	       if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
		  Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	       break;
	    case Enr_ADD_TO_CLIPBOARD_OTH:
	    case Enr_ADD_TO_CLIPBOARD_STD:
	    case Enr_ADD_TO_CLIPBOARD_TCH:
	    case Enr_OVERWRITE_CLIPBOARD_OTH:
	    case Enr_OVERWRITE_CLIPBOARD_STD:
	    case Enr_OVERWRITE_CLIPBOARD_TCH:
	       if (Gbl.Usrs.Me.Role.Logged < Rol_GST)
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
	       if (Acc_CheckIfICanEliminateAccount (Gbl.Usrs.Other.UsrDat.UsrCod) == Usr_CAN_NOT)
		  Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	       break;
	    default:
	       Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	       break;
	   }
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
     }
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
      switch (Enr_GetEnrRemAction ())
	{
	 case Enr_ENROL_MODIFY_ONE_USR_IN_CRS:
            /***** Show possible alerts *****/
            Ale_ShowAlerts (NULL);

            /***** Show form to edit user again *****/
	    Enr_ShowFormToEditOtherUsr ();
	    break;
	 case Enr_ENROL_ONE_DEG_ADMIN:
	    Adm_ReqAddAdm (Hie_DEG);
	    break;
	 case Enr_ENROL_ONE_CTR_ADMIN:
	    Adm_ReqAddAdm (Hie_CTR);
	    break;
	 case Enr_ENROL_ONE_INS_ADMIN:
	    Adm_ReqAddAdm (Hie_INS);
	    break;
	 case Enr_ADD_TO_CLIPBOARD_OTH:
	    UsrClp_AddOthToClipboard ();
	    break;
	 case Enr_ADD_TO_CLIPBOARD_STD:
	    UsrClp_AddStdToClipboard ();
	    break;
	 case Enr_ADD_TO_CLIPBOARD_TCH:
	    UsrClp_AddTchToClipboard ();
	    break;
	 case Enr_OVERWRITE_CLIPBOARD_OTH:
	    UsrClp_OverwriteOthClipboard ();
	    break;
	 case Enr_OVERWRITE_CLIPBOARD_STD:
	    UsrClp_OverwriteStdClipboard ();
	    break;
	 case Enr_OVERWRITE_CLIPBOARD_TCH:
	    UsrClp_OverwriteTchClipboard ();
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
   Usr_MeOrOther_t MeOrOther;
   static Act_Action_t NextAction[Rol_NUM_ROLES] =
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

   switch (Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat))
     {
      case Usr_BELONG:
	 MeOrOther = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);

	 /***** Show question and button to remove user as administrator *****/
	 /* Begin alert */
	 Ale_ShowAlertAndButtonBegin (Ale_QUESTION,Question[MeOrOther],
				      Gbl.Hierarchy.Node[Hie_CRS].FullName);

	 /* Show user's record */
	 Rec_ShowSharedRecordUnmodifiable (UsrDat);

	 /* Show form to request confirmation */
	 if (!NextAction[UsrDat->Roles.InCurrentCrs])
	    Err_WrongRoleExit ();
	 Frm_BeginForm (NextAction[UsrDat->Roles.InCurrentCrs]);
	    Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
	    Pwd_AskForConfirmationOnDangerousAction ();
	    Btn_PutButton (Btn_REMOVE,NULL);
	 Frm_EndForm ();

	 /* End alert */
	 Ale_ShowAlertAndButtonEnd (ActUnk,NULL,NULL,NULL,NULL,Btn_NO_BUTTON);
	 break;
      case Usr_DONT_BELONG:	// User does not belong to current course
      default:
	 Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
     }
  }

/*****************************************************************************/
/************************ Remove a user from a course ************************/
/*****************************************************************************/

static void Enr_EffectivelyRemUsrFromCrs (struct Usr_Data *UsrDat,
					  struct Hie_Node *Crs,
                                          Enr_RemoveUsrProduction_t RemoveUsrWorks,
					  Cns_Verbose_t Verbose)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_from_the_course_Y;

   switch (Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat))
     {
      case Usr_BELONG:
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
	       Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] = Usr_DONT_BELONG;
	       Gbl.Usrs.Me.UsrDat.Accepted           = Usr_HAS_NOT_ACCEPTED;

	       /* Fill the list with the courses I belong to */
	       Gbl.Usrs.Me.Hierarchy[Hie_CRS].Filled = false;
	       Hie_GetMyHierarchy (Hie_CRS);

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
	       /* Now he/she does not belong to current course */
	       UsrDat->Accepted           = Usr_HAS_NOT_ACCEPTED;
	       UsrDat->Roles.InCurrentCrs = Rol_USR;
	       break;
	   }

	 if (Verbose == Cns_VERBOSE)
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
			     Txt_THE_USER_X_has_been_removed_from_the_course_Y,
			     UsrDat->FullName,Crs->FullName);
	 break;
      case Usr_DONT_BELONG:	// User does not belong to course
      default:
	 if (Verbose == Cns_VERBOSE)
	    Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
     }
  }

/*****************************************************************************/
/***** Check if user belongs (no matter if he/she has accepted or not) *******/
/***** to the current course                                           *******/
/*****************************************************************************/

void Enr_FlushCacheUsrBelongsToCurrentCrs (void)
  {
   Gbl.Cache.UsrBelongsToCurrentCrs.Valid = false;
  }

Usr_Belong_t Enr_CheckIfUsrBelongsToCurrentCrs (const struct Usr_Data *UsrDat)
  {
   /***** 1. Fast check: Trivial cases *****/
   if (UsrDat->UsrCod <= 0 ||
       Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)
      return Usr_DONT_BELONG;

   /***** 2. Fast check: If cached... *****/
   if (Gbl.Cache.UsrBelongsToCurrentCrs.Valid &&
       UsrDat->UsrCod == Gbl.Cache.UsrBelongsToCurrentCrs.UsrCod)
      return Gbl.Cache.UsrBelongsToCurrentCrs.Belongs;

   /***** 3. Fast check: If we know role of user in the current course *****/
   if (UsrDat->Roles.InCurrentCrs != Rol_UNK)
     {
      Gbl.Cache.UsrBelongsToCurrentCrs.UsrCod  = UsrDat->UsrCod;
      Gbl.Cache.UsrBelongsToCurrentCrs.Belongs = (UsrDat->Roles.InCurrentCrs == Rol_STD ||
	                                          UsrDat->Roles.InCurrentCrs == Rol_NET ||
	                                          UsrDat->Roles.InCurrentCrs == Rol_TCH) ? Usr_BELONG :
	                                        					   Usr_DONT_BELONG;
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

Usr_Accepted_t Enr_CheckIfUsrHasAcceptedInCurrentCrs (const struct Usr_Data *UsrDat)
  {
   /***** 1. Fast check: Trivial cases *****/
   if (UsrDat->UsrCod <= 0 ||
       Gbl.Hierarchy.Node[Hie_CRS].HieCod <= 0)
      return Usr_HAS_NOT_ACCEPTED;

   /***** 2. Fast check: If cached... *****/
   if (Gbl.Cache.UsrHasAcceptedInCurrentCrs.Valid &&
       UsrDat->UsrCod == Gbl.Cache.UsrHasAcceptedInCurrentCrs.UsrCod)
      return Gbl.Cache.UsrHasAcceptedInCurrentCrs.Accepted;

   /***** 3. Fast / slow check: Get if user belongs to current course
                                and has accepted *****/
   Gbl.Cache.UsrHasAcceptedInCurrentCrs.UsrCod = UsrDat->UsrCod;
   Gbl.Cache.UsrHasAcceptedInCurrentCrs.Accepted = (Hie_CheckIfUsrBelongsTo (Hie_CRS,UsrDat->UsrCod,
						                             Gbl.Hierarchy.Node[Hie_CRS].HieCod,
						                             true) == Usr_BELONG) ? Usr_HAS_ACCEPTED :
												    Usr_HAS_NOT_ACCEPTED;
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
   if (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] == Usr_BELONG)
      if (Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat) == Usr_BELONG)	// Course selected and we both belong to it
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

unsigned Enr_GetNumUsrsInCrss (Hie_Level_t HieLvl,long HieCod,unsigned Roles)
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
   NumUsrs = Enr_DB_GetNumUsrsInCrss (HieLvl,HieCod,Roles,AnyUserInCourses);

   FigCch_UpdateFigureIntoCache (Enr_GetFigureNumUsrsInCrss (Roles),HieLvl,HieCod,
				 FigCch_UNSIGNED,&NumUsrs);
   return NumUsrs;
  }

unsigned Enr_GetCachedNumUsrsInCrss (Hie_Level_t HieLvl,long HieCod,unsigned Roles)
  {
   unsigned NumUsrsInCrss;

   /***** Get number of users in courses from cache *****/
   if (FigCch_GetFigureFromCache (Enr_GetFigureNumUsrsInCrss (Roles),HieLvl,HieCod,
                                  FigCch_UNSIGNED,&NumUsrsInCrss) == Exi_DOES_NOT_EXIST)
      /***** Get current number of users in courses from database and update cache *****/
      NumUsrsInCrss = Enr_GetNumUsrsInCrss (HieLvl,HieCod,Roles);

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
   if (FigCch_GetFigureFromCache (FigCch_NUM_GSTS,Hie_SYS,-1L,
                                  FigCch_UNSIGNED,&NumGsts) == Exi_DOES_NOT_EXIST)
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

double Enr_GetCachedAverageNumUsrsPerCrs (Hie_Level_t HieLvl,long HieCod,Rol_Role_t Role)
  {
   static FigCch_FigureCached_t FigureNumUsrsPerCrs[Rol_NUM_ROLES] =
     {
      [Rol_UNK] = FigCch_NUM_USRS_PER_CRS,	// Number of users per course
      [Rol_STD] = FigCch_NUM_STDS_PER_CRS,	// Number of students per course
      [Rol_NET] = FigCch_NUM_NETS_PER_CRS,	// Number of non-editing teachers per course
      [Rol_TCH] = FigCch_NUM_TCHS_PER_CRS,	// Number of teachers per course
     };
   double AverageNumUsrsPerCrs;

   /***** Get number of users per course from cache *****/
   if (FigCch_GetFigureFromCache (FigureNumUsrsPerCrs[Role],HieLvl,HieCod,
                                  FigCch_DOUBLE,&AverageNumUsrsPerCrs) == Exi_DOES_NOT_EXIST)
     {
      /***** Get current number of users per course from database and update cache *****/
      AverageNumUsrsPerCrs = Enr_DB_GetAverageNumUsrsPerCrs (HieLvl,HieCod,Role);
      FigCch_UpdateFigureIntoCache (FigureNumUsrsPerCrs[Role],HieLvl,HieCod,
                                    FigCch_DOUBLE,&AverageNumUsrsPerCrs);
     }

   return AverageNumUsrsPerCrs;
  }

/*****************************************************************************/
/************ Get average number of courses with users of a role *************/
/*****************************************************************************/

double Enr_GetCachedAverageNumCrssPerUsr (Hie_Level_t HieLvl,long HieCod,Rol_Role_t Role)
  {
   static FigCch_FigureCached_t FigureNumCrssPerUsr[Rol_NUM_ROLES] =
     {
      [Rol_UNK] = FigCch_NUM_CRSS_PER_USR,	// Number of courses per user
      [Rol_STD] = FigCch_NUM_CRSS_PER_STD,	// Number of courses per student
      [Rol_NET] = FigCch_NUM_CRSS_PER_NET,	// Number of courses per non-editing teacher
      [Rol_TCH] = FigCch_NUM_CRSS_PER_TCH,	// Number of courses per teacher
     };
   double AverageNumCrssPerUsr;

   /***** Get number of courses per user from cache *****/
   if (FigCch_GetFigureFromCache (FigureNumCrssPerUsr[Role],HieLvl,HieCod,
                                  FigCch_DOUBLE,&AverageNumCrssPerUsr) == Exi_DOES_NOT_EXIST)
     {
      /***** Get current number of courses per user from database and update cache *****/
      AverageNumCrssPerUsr = Enr_DB_GetAverageNumCrssPerUsr (HieLvl,HieCod,Role);
      FigCch_UpdateFigureIntoCache (FigureNumCrssPerUsr[Role],HieLvl,HieCod,
                                    FigCch_DOUBLE,&AverageNumCrssPerUsr);
     }

   return AverageNumCrssPerUsr;
  }
