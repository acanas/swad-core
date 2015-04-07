// swad_enrollment.c: enrollment (registration) or removing of users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include <stdlib.h>		// For exit, system, malloc, free, rand, etc.
#include <string.h>		// For string functions

#include "swad_account.h"
#include "swad_announcement.h"
#include "swad_database.h"
#include "swad_enrollment.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

#define Enr_NUM_ACTIONS_REG_REM_USRS 5
typedef enum
  {
   Enr_REGISTER_SPECIFIED_USRS_IN_CRS     = 0,
   Enr_REMOVE_SPECIFIED_USRS_FROM_CRS     = 1,
   Enr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS = 2,
   Enr_UPDATE_USRS_IN_CRS                 = 3,
   Enr_ELIMINATE_USRS_FROM_PLATFORM       = 4,
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

static void Enr_ShowFormRegRemSeveralUsrs (void);

static void Enr_PutLinkToRemOldUsrs (void);
static void Enr_PutAreaToEnterUsrsIDs (void);
static void Enr_PutActionsRegRemSeveralUsrs (void);

static void Enr_RegisterUsr (struct UsrData *UsrDat,Rol_Role_t RegRemRole,
                             struct ListCodGrps *LstGrps,unsigned *NumUsrsRegistered);
static void Enr_MarkOfficialStdsAsRemovable (long ImpGrpCod,bool RemoveSpecifiedUsrs);

static void Enr_PutLinkToRemAllStdsThisCrs (void);
static void Enr_RemoveEnrollmentRequest (long CrsCod,long UsrCod);
static void Enr_PutLinkToAdminOneUsr (void);
static void Enr_PutLinkToAdminSeveralUsrs (void);

static void Enr_ReqAnotherUsrIDToRegisterRemove (void);
static void Enr_AskIfRegRemMe (void);
static void Enr_AskIfRegRemUsr (struct ListUsrCods *ListUsrCods);

static void Enr_ShowFormToEditOtherUsr (void);

static void Enr_AddAdm (Sco_Scope_t Scope,long Cod,const char *InsCtrDegName);
static void Enr_RegisterAdmin (struct UsrData *UsrDat,Sco_Scope_t Scope,
                               long Cod,const char *InsCtrDegName);
static void Enr_ReqRemOrRemUsrFromCrs (Enr_ReqDelOrDelUsr_t ReqDelOrDelUsr);
static void Enr_ReqRemAdmOfDeg (void);
static void Enr_ReqRemOrRemAdm (Enr_ReqDelOrDelUsr_t ReqDelOrDelUsr,Sco_Scope_t Scope,
                                long Cod,const char *InsCtrDegName);

static void Enr_ReqAddAdm (Sco_Scope_t Scope,long Cod,const char *InsCtrDegName);
static void Enr_AskIfRemoveUsrFromCrs (struct UsrData *UsrDat,bool ItsMe);
static void Enr_EffectivelyRemUsrFromCrs (struct UsrData *UsrDat,struct Course *Crs,
                                          Enr_RemoveUsrWorks_t RemoveUsrWorks,Cns_QuietOrVerbose_t QuietOrVerbose);

static void Enr_AskIfRemAdm (bool ItsMe,Sco_Scope_t Scope,const char *InsCtrDegName);
static void Enr_EffectivelyRemAdm (struct UsrData *UsrDat,Sco_Scope_t Scope,
                                   long Cod,const char *InsCtrDegName);

/*****************************************************************************/
/************ Show form to request sign up in the current course *************/
/*****************************************************************************/

void Enr_PutLinkToRequestSignUp (void)
  {
   extern const char *Txt_Sign_up;

   /***** Show the form *****/
   Act_PutContextualLink (ActReqSignUp,NULL,"signup",Txt_Sign_up);
  }

/*****************************************************************************/
/***************** Modify the role of a user in a course *********************/
/*****************************************************************************/

void Enr_ModifyRoleInCurrentCrs (struct UsrData *UsrDat,
                                 Rol_Role_t NewRole,
                                 Cns_QuietOrVerbose_t QuietOrVerbose)
  {
   extern const char *Txt_The_role_of_THE_USER_X_in_the_course_Y_has_changed_from_A_to_B;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char Query[256];
   Rol_Role_t OldRole;

   /***** Change user's role if different *****/
   OldRole = Rol_GetRoleInCrs (Gbl.CurrentCrs.Crs.CrsCod,UsrDat->UsrCod);
   if (NewRole != OldRole)        // The role must be updated
     {
      /***** Check if user's role is allowed *****/
      if (!(NewRole == Rol_STUDENT ||
	    NewRole == Rol_TEACHER))
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
		  Txt_ROLES_SINGUL_abc[OldRole][UsrDat->Sex],
		  Txt_ROLES_SINGUL_abc[NewRole][UsrDat->Sex]);
	 Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
	}

      UsrDat->RoleInCurrentCrsDB = NewRole;
      UsrDat->Roles |= (1 << NewRole);
     }
  }

/*****************************************************************************/
/*********************** Register user in current course *********************/
/*****************************************************************************/
// Before calling this function, you must be sure that
// the user does not belong to the current course

void Enr_RegisterUsrInCurrentCrs (struct UsrData *UsrDat,Rol_Role_t NewRole,
                                  Cns_QuietOrVerbose_t QuietOrVerbose,
                                  Enr_KeepOrSetAccepted_t KeepOrSetAccepted)
  {
   extern const char *Usr_StringsUsrListTypeInDB[Usr_NUM_USR_LIST_TYPES];
   extern const char *Txt_THE_USER_X_has_been_enrolled_in_the_course_Y;
   char Query[1024];
   bool CreateNotif;
   bool NotifyByEmail;

   /***** Check if user's role is allowed *****/
   if (!(NewRole == Rol_STUDENT ||
	 NewRole == Rol_TEACHER))
      Lay_ShowErrorAndExit ("Wrong role.");

   /***** Register user in current course in database *****/
   sprintf (Query,"INSERT INTO crs_usr (CrsCod,UsrCod,Role,Accepted,"
		  "LastDowGrpCod,LastComGrpCod,LastAssGrpCod,NumAccTst,LastAccTst,NumQstsLastTst,"
		  "UsrListType,ColsClassPhoto,ListWithPhotos)"
		  " VALUES ('%ld','%ld','%u','%c',"
		  "'-1','-1','-1','0','0000-00-00','0',"
		  "'%s','%u','%c')",
	    Gbl.CurrentCrs.Crs.CrsCod,UsrDat->UsrCod,(unsigned) NewRole,
	    KeepOrSetAccepted == Enr_SET_ACCEPTED_TO_TRUE ? 'Y' :
		                                            'N',
	    Usr_StringsUsrListTypeInDB[Usr_SHOW_USRS_TYPE_DEFAULT],
	    Usr_CLASS_PHOTO_COLS_DEF,
	    Usr_LIST_WITH_PHOTOS_DEF ? 'Y' :
		                       'N');
   DB_QueryINSERT (Query,"can not register user in course");
   UsrDat->RoleInCurrentCrsDB = NewRole;
   UsrDat->Roles |= NewRole;

   /***** Remove possible inscription request ******/
   Enr_RemoveEnrollmentRequest (Gbl.CurrentCrs.Crs.CrsCod,UsrDat->UsrCod);

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
/****** Write a form to request another user's ID, @nickname or e-mail *******/
/*****************************************************************************/

void Enr_WriteFormToReqAnotherUsrID (Act_Action_t NextAction)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_nick_email_or_ID;
   extern const char *Txt_Continue;

   /***** Form to request user's ID, @nickname or e-mail address *****/
   Act_FormStart (NextAction);
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:center;\">"
                      "%s: "
                      "<input type=\"text\" name=\"OtherUsrIDNickOrEMail\""
                      " size=\"20\" maxlength=\"%u\" />"
                      "</div>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_nick_email_or_ID,
            Usr_MAX_BYTES_USR_LOGIN);

   /***** Send button*****/
   Lay_PutConfirmButton (Txt_Continue);
   Act_FormEnd ();
  }

/*****************************************************************************/
/****** Request acceptation / refusion of register in current course *********/
/*****************************************************************************/

void Enr_ReqAcceptRegisterInCrs (void)
  {
   extern const char *Txt_A_teacher_or_administrator_has_enrolled_you_into_the_course_;
   extern const char *Txt_Confirm_my_enrollment;
   extern const char *Txt_Remove_me_from_this_course;

   /***** Show message *****/
   sprintf (Gbl.Message,Txt_A_teacher_or_administrator_has_enrolled_you_into_the_course_,
            Gbl.CurrentCrs.Crs.FullName);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

   /***** Send button to accept register in the current course *****/
   Act_PutContextualLink (ActAccEnrCrs,NULL,
                          "ok_green",Txt_Confirm_my_enrollment);

   /***** Send button to refuse register in the current course *****/
   Act_PutContextualLink (ActRemMeCrs,NULL,
                          "delon",Txt_Remove_me_from_this_course);

   fprintf (Gbl.F.Out,"</div>");

   /***** Mark possible notification as seen *****/
   Ntf_SetNotifAsSeen (Ntf_EVENT_ENROLLMENT,
                       -1L,
                       Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/****************** Put an enrollment into a notification ********************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void Enr_GetNotifEnrollment (char *SummaryStr,
                             long CrsCod,long UsrCod,
                             unsigned MaxChars)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
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
            Role = Rol_ConvertUnsignedStrToRole (row[0]);
            strcpy (SummaryStr,Txt_ROLES_SINGUL_Abc[Role][UsrDat.Sex]);
            if (MaxChars)
               Str_LimitLengthHTMLStr (SummaryStr,MaxChars);

            /* Free memory used for user's data */
            Usr_UsrDataDestructor (&UsrDat);
           }

         mysql_free_result (mysql_res);
        }
  }

/*****************************************************************************/
/***************************** Update user's data ****************************/
/*****************************************************************************/
// UsrDat->UsrCod must be > 0

void Enr_UpdateUsrData (struct UsrData *UsrDat)
  {
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];
   char Query[2048];

   /***** Check if user's code is initialized *****/
   if (UsrDat->UsrCod <= 0)
      Lay_ShowErrorAndExit ("Can not update user's data. Wrong user's code.");

   /***** Filter some user's data before updating */
   Enr_FilterUsrDat (UsrDat);

   /***** Update user's common data *****/
   sprintf (Query,"UPDATE usr_data"
		  " SET Password='%s',"
		  "Surname1='%s',Surname2='%s',FirstName='%s',Sex='%s',"
		  "Layout='%u',Theme='%s',IconSet='%s',Language='%s',PhotoVisibility='%s',ProfileVisibility='%s',"
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
            Pri_VisibilityDB[UsrDat->PhotoVisibility],
            Pri_VisibilityDB[UsrDat->ProfileVisibility],
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
/************************* Filter some user's data ***************************/
/*****************************************************************************/

void Enr_FilterUsrDat (struct UsrData *UsrDat)
  {
   /***** Fix birthday *****/
   if (UsrDat->Birthday.Year < Gbl.Now.Date.Year-99 ||
       UsrDat->Birthday.Year > Gbl.Now.Date.Year-16)
      UsrDat->Birthday.Year =
      UsrDat->Birthday.Month =
      UsrDat->Birthday.Day = 0;
  }

/*****************************************************************************/
/**************** Update institution, centre and department ******************/
/*****************************************************************************/

void Enr_UpdateInstitutionCentreDepartment (void)
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
/************** Form to request the user's ID of another user ****************/
/*****************************************************************************/

void Enr_ReqAdminUsrs (void)
  {
   extern const char *Txt_You_dont_have_permission_to_perform_this_action;

   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol__GUEST_:
      case Rol_VISITOR:
      case Rol_STUDENT:
	 Enr_AskIfRegRemMe ();
	 break;
      case Rol_TEACHER:
	 if (Gbl.CurrentCrs.Crs.CrsCod > 0)
	    Enr_ShowFormRegRemSeveralUsrs ();
	 else
	    Enr_AskIfRegRemMe ();
	 break;
      case Rol_DEG_ADM:
	 if (Gbl.CurrentDeg.Deg.DegCod > 0)
	    Enr_ReqAnotherUsrIDToRegisterRemove ();
	 else
	    Enr_AskIfRegRemMe ();
	 break;
      case Rol_CTR_ADM:
	 if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
	    Enr_ReqAnotherUsrIDToRegisterRemove ();
	 else
	    Enr_AskIfRegRemMe ();
	 break;
      case Rol_INS_ADM:
	 if (Gbl.CurrentIns.Ins.InsCod > 0)
	    Enr_ReqAnotherUsrIDToRegisterRemove ();
	 else
	    Enr_AskIfRegRemMe ();
	 break;
      case Rol_SYS_ADM:
	 if (Gbl.CurrentCrs.Crs.CrsCod > 0)
	    Enr_ShowFormRegRemSeveralUsrs ();
	 else
	    Enr_ReqAnotherUsrIDToRegisterRemove ();
	 break;
      default:
	 Lay_ShowAlert (Lay_ERROR,Txt_You_dont_have_permission_to_perform_this_action);
	 break;
     }
  }

/*****************************************************************************/
/***** Register/remove users (taken from a list) in/from current course ******/
/*****************************************************************************/

static void Enr_ShowFormRegRemSeveralUsrs (void)
  {
   extern const char *The_ClassTitle[The_NUM_THEMES];
   extern const char *Txt_Admin_several_users;
   extern const char *Txt_Step_1_Provide_a_list_of_users;
   extern const char *Txt_Option_a_Import_students_from_the_official_lists;
   extern const char *Txt_Select_the_groups_of_students_you_want_to_register_in_remove_from_this_course;
   extern const char *Txt_Option_b_Type_or_paste_a_list_of_users;
   extern const char *Txt_Type_or_paste_a_list_of_IDs_nicks_or_emails_;
   extern const char *Txt_Step_2_Select_the_type_of_users_to_register_remove;
   extern const char *Txt_Step_3_Select_the_desired_action;
   extern const char *Txt_Step_4_Optionally_select_groups;
   extern const char *Txt_Select_the_groups_in_from_which_you_want_to_register_remove_users_;
   extern const char *Txt_No_groups_have_been_created_in_the_course_X_Therefore_;
   extern const char *Txt_Step_5_Confirm_the_enrollment_removing;
   extern const char *Txt_Confirm;
   bool ExternalUsrsServiceAvailable = (Cfg_EXTERNAL_LOGIN_CLIENT_COMMAND[0] != '\0');

   /***** Put contextual links *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

   /* Put link to go to admin one user */
   Enr_PutLinkToAdminOneUsr ();

   /* Put link to remove all the students in the current course */
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
      Enr_PutLinkToRemAllStdsThisCrs ();

   /* Put link to remove old users */
   if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
      Enr_PutLinkToRemOldUsrs ();

   fprintf (Gbl.F.Out,"</div>");

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,2,Txt_Admin_several_users);
   fprintf (Gbl.F.Out,"<tr>"
                      "<td>");

   /***** Form to send students to be enrolled / removed *****/
   Act_FormStart (ActRcvFrmMdfUsrCrs);

   /***** Step 1: List of students to be enrolled / removed *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:left;\">"
                      "%s"
                      "</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Step_1_Provide_a_list_of_users);

   if (ExternalUsrsServiceAvailable)
     {
      /* Option a: get students from official lists */
      fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:left;\">"
			 "%s"
			 "</div>",
	       The_ClassTitle[Gbl.Prefs.Theme],
	       Txt_Option_a_Import_students_from_the_official_lists);
      Lay_ShowAlert (Lay_INFO,Txt_Select_the_groups_of_students_you_want_to_register_in_remove_from_this_course);
      Imp_ListMyImpGrpsAndStdsForm ();

      /* Option b: get students' IDs from pasted text */
      fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:left;\">"
			 "%s"
			 "</div>",
	       The_ClassTitle[Gbl.Prefs.Theme],
	       Txt_Option_b_Type_or_paste_a_list_of_users);
     }

   Lay_ShowAlert (Lay_INFO,Txt_Type_or_paste_a_list_of_IDs_nicks_or_emails_);
   Enr_PutAreaToEnterUsrsIDs ();

   /***** Step 2: Select type of user to register/remove to/from current course *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:left;\">"
                      "%s"
                      "</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Step_2_Select_the_type_of_users_to_register_remove);
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
      Rol_PutAllRolesRegRemUsrsCrs ();

   /***** Step 3: Put different actions to register/remove students to/from current course *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:left;\">"
                      "%s"
                      "</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Step_3_Select_the_desired_action);
   Enr_PutActionsRegRemSeveralUsrs ();

   /***** Step 4: Select groups in which register / remove students *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:left;\">"
                      "%s"
                      "</div>",
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
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:left;\">"
                      "%s"
                      "</div>",
            The_ClassTitle[Gbl.Prefs.Theme],
            Txt_Step_5_Confirm_the_enrollment_removing);
   Pwd_AskForConfirmationOnDangerousAction ();
   Lay_PutConfirmButton (Txt_Confirm);

   /***** End of form *****/
   Act_FormEnd ();

   /***** End frame *****/
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/******************** Put a link (form) to remove old users ******************/
/*****************************************************************************/

static void Enr_PutLinkToRemOldUsrs (void)
  {
   extern const char *Txt_Remove_old_users;

   /***** Put form to remove old users *****/
   Act_PutContextualLink (ActReqRemOldUsr,NULL,"delon",Txt_Remove_old_users);
  }

/*****************************************************************************/
/*********************** Write form to remove old users **********************/
/*****************************************************************************/

void Enr_AskRemoveOldUsrs (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Eliminate_all_users_who_are_not_enrolled_on_any_courses_PART_1_OF_2;
   extern const char *Txt_Eliminate_all_users_who_are_not_enrolled_on_any_courses_PART_2_OF_2;
   extern const char *Txt_Eliminate;
   unsigned MonthsWithoutAccess = Usr_DEF_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS;
   unsigned Months;

   /***** Form to request number of months without clicks *****/
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
   Act_FormStart (ActRemOldUsr);
   fprintf (Gbl.F.Out,"<span class=\"%s\">%s </span>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Eliminate_all_users_who_are_not_enrolled_on_any_courses_PART_1_OF_2);
   fprintf (Gbl.F.Out,"<select name=\"Months\">");
   for (Months  = Usr_MIN_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS;
        Months <= Usr_MAX_MONTHS_WITHOUT_ACCESS_TO_REMOVE_OLD_USRS;
        Months++)
     {
      fprintf (Gbl.F.Out,"<option");
      if (Months == MonthsWithoutAccess)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%u</option>",Months);
     }
   fprintf (Gbl.F.Out,"</select>"
                      "<span class=\"%s\"> ",
            The_ClassFormul[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,Txt_Eliminate_all_users_who_are_not_enrolled_on_any_courses_PART_2_OF_2,
            Cfg_PLATFORM_SHORT_NAME);
   fprintf (Gbl.F.Out,"</span>");

   /***** Send button*****/
   Lay_PutRemoveButton (Txt_Eliminate);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******************************* Remove old users ****************************/
/*****************************************************************************/

void Enr_RemoveOldUsrs (void)
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
   sprintf (Gbl.Message,Txt_X_users_have_been_eliminated,
            NumUsrsEliminated);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/***** Put text area to enter/paste IDs of users to be enrolled/removed ******/
/*****************************************************************************/

static void Enr_PutAreaToEnterUsrsIDs (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_List_of_nicks_emails_or_IDs;

   /***** Start of text area *****/
   fprintf (Gbl.F.Out,"<table style=\"margin:0 auto;\">"
                      "<tr>"
                      "<td class=\"%s\""
                      " style=\"text-align:right; vertical-align:top;\">"
                      "%s: "
                      "</td>"
                      "<td style=\"text-align:left;\">"
                      "<textarea name=\"UsrsIDs\" cols=\"90\" rows=\"15\">",
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

bool Enr_PutActionsRegRemOneUsr (bool ItsMe)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Modify_me_in_the_course_X;
   extern const char *Txt_Modify_user_in_the_course_X;
   extern const char *Txt_Register_me_in_the_course_X;
   extern const char *Txt_Register_user_in_the_course_X;
   extern const char *Txt_Register_user_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Register_user_as_an_administrator_of_the_centre_X;
   extern const char *Txt_Register_user_as_an_administrator_of_the_institution_X;
   extern const char *Txt_Remove_me_from_the_course_X;
   extern const char *Txt_Remove_user_from_the_course_X;
   extern const char *Txt_Remove_me_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Remove_user_as_an_administrator_of_the_degree_X;
   extern const char *Txt_Remove_me_as_an_administrator_of_the_centre_X;
   extern const char *Txt_Remove_user_as_an_administrator_of_the_centre_X;
   extern const char *Txt_Remove_me_as_an_administrator_of_the_institution_X;
   extern const char *Txt_Remove_user_as_an_administrator_of_the_institution_X;
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
      UsrBelongsToCrs = Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod);

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
   fprintf (Gbl.F.Out,"<div style=\"display:inline-block; margin:0 auto;\">"
                      "<ul class=\"%s\" style=\"list-style-type:none;"
                      " text-align:left;\">",
            The_ClassFormul[Gbl.Prefs.Theme]);

   /***** Register user in course / Modify user's data *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0 &&
       Gbl.Usrs.Me.LoggedRole >= Rol_TEACHER)
     {
      sprintf (Gbl.Message,UsrBelongsToCrs ? (ItsMe ? Txt_Modify_me_in_the_course_X :
		                                      Txt_Modify_user_in_the_course_X) :
	                                     (ItsMe ? Txt_Register_me_in_the_course_X :
		                                      Txt_Register_user_in_the_course_X),
	       Gbl.CurrentCrs.Crs.ShortName);
      fprintf (Gbl.F.Out,"<li>"
			 "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\"",
               (unsigned) Enr_REGISTER_MODIFY_ONE_USR_IN_CRS);
      if (!OptionChecked)
	{
	 fprintf (Gbl.F.Out," checked=\"checked\"");
         OptionChecked = true;
	}
      fprintf (Gbl.F.Out," />%s</li>",Gbl.Message);

      NumOptionsShown++;
     }

   if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
	{
	 if (Gbl.CurrentDeg.Deg.DegCod > 0)
	    /***** Register user as administrator of degree *****/
	    if (!UsrIsDegAdmin &&
		Gbl.Usrs.Me.LoggedRole >= Rol_CTR_ADM)
	      {
	       sprintf (Gbl.Message,Txt_Register_user_as_an_administrator_of_the_degree_X,
			Gbl.CurrentDeg.Deg.ShortName);
	       fprintf (Gbl.F.Out,"<li>"
				  "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\"",
			(unsigned) Enr_REGISTER_ONE_DEGREE_ADMIN);
	       if (!OptionChecked)
		 {
		  fprintf (Gbl.F.Out," checked=\"checked\"");
		  OptionChecked = true;
		 }
	       fprintf (Gbl.F.Out," />%s</li>",Gbl.Message);

	       NumOptionsShown++;
	      }

	 /***** Register user as administrator of centre *****/
	 if (!UsrIsCtrAdmin &&
	     Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM)
	   {
	    sprintf (Gbl.Message,Txt_Register_user_as_an_administrator_of_the_centre_X,
		     Gbl.CurrentCtr.Ctr.ShortName);
	    fprintf (Gbl.F.Out,"<li>"
			       "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\"",
		     (unsigned) Enr_REGISTER_ONE_CENTRE_ADMIN);
	    if (!OptionChecked)
	      {
	       fprintf (Gbl.F.Out," checked=\"checked\"");
	       OptionChecked = true;
	      }
	    fprintf (Gbl.F.Out," />%s</li>",Gbl.Message);

	    NumOptionsShown++;
	   }
	}

      /***** Register user as administrator of institution *****/
      if (!UsrIsInsAdmin &&
	  Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
	{
	 sprintf (Gbl.Message,Txt_Register_user_as_an_administrator_of_the_institution_X,
		  Gbl.CurrentIns.Ins.ShortName);
	 fprintf (Gbl.F.Out,"<li>"
			    "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\"",
		  (unsigned) Enr_REGISTER_ONE_INSTITUTION_ADMIN);
	 if (!OptionChecked)
	   {
	    fprintf (Gbl.F.Out," checked=\"checked\"");
	    OptionChecked = true;
	   }
	 fprintf (Gbl.F.Out," />%s</li>",Gbl.Message);

	 NumOptionsShown++;
	}
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
	       (unsigned) Enr_REMOVE_ONE_USR_FROM_CRS);
      if (!OptionChecked)
	{
	 fprintf (Gbl.F.Out," checked=\"checked\"");
	 OptionChecked = true;
	}
      fprintf (Gbl.F.Out," />%s</li>",Gbl.Message);

      NumOptionsShown++;
     }

   if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
	{
	 if (Gbl.CurrentIns.Ins.InsCod > 0)
	    /***** Remove user as an administrator of the degree *****/
	    if (UsrIsDegAdmin &&
		(ItsMe || Gbl.Usrs.Me.LoggedRole >= Rol_CTR_ADM))
	      {
	       sprintf (Gbl.Message,
			ItsMe ? Txt_Remove_me_as_an_administrator_of_the_degree_X :
				Txt_Remove_user_as_an_administrator_of_the_degree_X,
			Gbl.CurrentDeg.Deg.ShortName);
	       fprintf (Gbl.F.Out,"<li>"
				  "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\"",
			(unsigned) Enr_REMOVE_ONE_DEGREE_ADMIN);
	       if (!OptionChecked)
		 {
		  fprintf (Gbl.F.Out," checked=\"checked\"");
		  OptionChecked = true;
		 }
	       fprintf (Gbl.F.Out," />%s</li>",Gbl.Message);

	       NumOptionsShown++;
	      }

          /***** Remove user as an administrator of the centre *****/
	  if (UsrIsCtrAdmin &&
	     (ItsMe || Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM))
	   {
	    sprintf (Gbl.Message,
		     ItsMe ? Txt_Remove_me_as_an_administrator_of_the_centre_X :
			     Txt_Remove_user_as_an_administrator_of_the_centre_X,
		     Gbl.CurrentCtr.Ctr.ShortName);
	    fprintf (Gbl.F.Out,"<li>"
			       "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\"",
		     (unsigned) Enr_REMOVE_ONE_CENTRE_ADMIN);
	    if (!OptionChecked)
	      {
	       fprintf (Gbl.F.Out," checked=\"checked\"");
	       OptionChecked = true;
	      }
	    fprintf (Gbl.F.Out," />%s</li>",Gbl.Message);

	    NumOptionsShown++;
	   }
	}

      /***** Remove user as an administrator of the institution *****/
      if (UsrIsInsAdmin &&
	  (ItsMe || Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM))
	{
	 sprintf (Gbl.Message,
		  ItsMe ? Txt_Remove_me_as_an_administrator_of_the_institution_X :
			  Txt_Remove_user_as_an_administrator_of_the_institution_X,
		  Gbl.CurrentIns.Ins.ShortName);
	 fprintf (Gbl.F.Out,"<li>"
			    "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\"",
		  (unsigned) Enr_REMOVE_ONE_INSTITUTION_ADMIN);
	 if (!OptionChecked)
	   {
	    fprintf (Gbl.F.Out," checked=\"checked\"");
	    OptionChecked = true;
	   }
	 fprintf (Gbl.F.Out," />%s</li>",Gbl.Message);

	 NumOptionsShown++;
	}
     }

   /***** Eliminate user completely from platform *****/
   if (Acc_CheckIfICanEliminateAccount (ItsMe))
     {
      fprintf (Gbl.F.Out,"<li>"
                         "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\"",
               (unsigned) Enr_ELIMINATE_ONE_USR_FROM_PLATFORM);
      if (!OptionChecked)
	 fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," />%s</li>",
               ItsMe ? Txt_Eliminate_my_user_account :
        	       Txt_Eliminate_user_account);

      NumOptionsShown++;
     }

   /***** End list of options *****/
   fprintf (Gbl.F.Out,"</ul>"
                      "</div>");

   return (NumOptionsShown ? true :
	                     false);
  }

/*****************************************************************************/
/*** Put different actions to register/remove users to/from current course ***/
/*****************************************************************************/

static void Enr_PutActionsRegRemSeveralUsrs (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Register_the_users_indicated_in_step_1;
   extern const char *Txt_Remove_the_users_indicated_in_step_1;
   extern const char *Txt_Remove_the_users_not_indicated_in_step_1;
   extern const char *Txt_Register_the_users_indicated_in_step_1_and_remove_the_users_not_indicated;
   extern const char *Txt_Eliminate_from_the_platform_the_users_indicated_in_step_1;

   /***** Start list of options *****/
   fprintf (Gbl.F.Out,"<div style=\"display:inline-block; margin:0 auto;\">"
                      "<ul class=\"%s\" style=\"list-style-type:none;"
                      " text-align:left;\">",
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
	       (unsigned) Enr_REGISTER_SPECIFIED_USRS_IN_CRS,
	       Txt_Register_the_users_indicated_in_step_1,
	       (unsigned) Enr_REMOVE_SPECIFIED_USRS_FROM_CRS,
	       Txt_Remove_the_users_indicated_in_step_1,
	       (unsigned) Enr_REMOVE_NOT_SPECIFIED_USRS_FROM_CRS,
	       Txt_Remove_the_users_not_indicated_in_step_1,
	       (unsigned) Enr_UPDATE_USRS_IN_CRS,
	       Txt_Register_the_users_indicated_in_step_1_and_remove_the_users_not_indicated);

   /***** Only for superusers *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
      fprintf (Gbl.F.Out,"<li>"
                         "<input type=\"radio\" name=\"RegRemAction\" value=\"%u\" />"
                         "%s</li>",
               (unsigned) Enr_ELIMINATE_USRS_FROM_PLATFORM,
               Txt_Eliminate_from_the_platform_the_users_indicated_in_step_1);

   /***** End list of options *****/
   fprintf (Gbl.F.Out,"</ul>"
                      "</div>");
  }

/*****************************************************************************/
/******* Receive the list of users of the course to register/remove **********/
/*****************************************************************************/

void Enr_ReceiveFormUsrsCrs (void)
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
   Rol_Role_t RegRemRole = Rol_STUDENT;
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
   Enr_RegRemUsrsAction_t RegRemUsrsAction;
   bool ErrorInForm = false;

   /***** Get confirmation *****/
   if (!Pwd_GetConfirmationOnDangerousAction ())
      return;

   /***** Get the type of user to register / remove *****/
   Par_GetParToText ("RegRemRole",UnsignedStr,1);
   if (UnsignedStr[0])
      switch ((RegRemRole = Rol_ConvertUnsignedStrToRole (UnsignedStr)))
        {
         case Rol_STUDENT:
            break;
         case Rol_TEACHER:
            switch (Gbl.Usrs.Me.LoggedRole)        // Can I register/remove teachers?
              {
               case Rol_DEG_ADM:
               case Rol_SYS_ADM:                // Yes, I can
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
      if (UnsignedNum < Enr_NUM_ACTIONS_REG_REM_USRS)
	 switch (RegRemUsrsAction = (Enr_RegRemUsrsAction_t) UnsignedNum)
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
	       if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
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
      Enr_ShowFormRegRemSeveralUsrs ();
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
	     RegRemRole == Rol_STUDENT &&
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
	    case Rol_STUDENT:
	       Usr_GetUsrsLst (Rol_STUDENT,Sco_SCOPE_CRS,NULL,false);
	       LstCurrentUsrs = &Gbl.Usrs.LstStds;
	       break;
	    case Rol_TEACHER:
	       Usr_GetUsrsLst (Rol_TEACHER,Sco_SCOPE_CRS,NULL,false);
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
	    if (RegRemRole == Rol_STUDENT)
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
		     Enr_MarkOfficialStdsAsRemovable (GrpCod,WhatToDo.RemoveSpecifiedUsrs);
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
	 Usr_FreeUsrsList (LstCurrentUsrs);
	}

      /***** Register users *****/
      if (WhatToDo.RegisterUsrs)	// TODO: !!!!! NO CAMBIAR EL ROL DE LOS USUARIOS QUE YA ESTÉN EN LA ASIGNATURA SI HAY MÁS DE UN USUARIO ENCONTRADO PARA EL MISMO DNI !!!!!!
	{
	 /***** Step a: Get users from a list of official groups *****/
	 if (RegRemRole == Rol_STUDENT)
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
		  Enr_RegisterUsr (&UsrDat,RegRemRole,&LstGrps,&NumUsrsRegistered);
		 }
	    else if (ItLooksLikeAUsrID)	// User not found. He/she is a new user. Register him/her using ID
	       Enr_RegisterUsr (&UsrDat,RegRemRole,&LstGrps,&NumUsrsRegistered);

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

static void Enr_RegisterUsr (struct UsrData *UsrDat,Rol_Role_t RegRemRole,
                             struct ListCodGrps *LstGrps,unsigned *NumUsrsRegistered)
  {
   /***** Check if I can register this user *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER &&
       RegRemRole != Rol_STUDENT)
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
      Acc_CreateNewUsr (UsrDat);
     }

   /***** Register user in current course in database *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
     {
      if (Usr_CheckIfUsrBelongsToCrs (UsrDat->UsrCod,Gbl.CurrentCrs.Crs.CrsCod))      // User does belong to current course, modify his/her role
	 Enr_ModifyRoleInCurrentCrs (UsrDat,RegRemRole,Cns_QUIET);
      else
	 Enr_RegisterUsrInCurrentCrs (UsrDat,RegRemRole,Cns_QUIET,
	                              Enr_SET_ACCEPTED_TO_FALSE);

      /***** Register user in the selected groups *****/
      if (Gbl.CurrentCrs.Grps.NumGrps)	// If there are groups in the course
	 Grp_RegisterUsrIntoGroups (UsrDat,LstGrps);
     }

   (*NumUsrsRegistered)++;
  }

/*****************************************************************************/
/****** Mark the students belonging to an official group as removable ********/
/*****************************************************************************/

static void Enr_MarkOfficialStdsAsRemovable (long ImpGrpCod,bool RemoveSpecifiedUsrs)
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

static void Enr_PutLinkToRemAllStdsThisCrs (void)
  {
   extern const char *Txt_Remove_all_students;

   /***** Put form to remove all the students in the current course *****/
   Act_PutContextualLink (ActReqRemAllStdCrs,NULL,
                          "delon",Txt_Remove_all_students);
  }

/*****************************************************************************/
/********** Ask for removing all the students from current course ************/
/*****************************************************************************/

void Enr_AskRemAllStdsThisCrs (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_X_students_from_the_course_Y_;
   extern const char *Txt_Remove_students;
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   unsigned NumStds;

   if ((NumStds = Usr_GetNumUsrsInCrs (Rol_STUDENT,Gbl.CurrentCrs.Crs.CrsCod)))
     {
      /***** Write message to confirm the removing *****/
      sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_X_students_from_the_course_Y_,
               NumStds,Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);

      /***** Form to remove all the students in current course *****/
      Act_FormStart (ActRemAllStdCrs);
      Grp_PutParamAllGroups ();
      Pwd_AskForConfirmationOnDangerousAction ();
      Lay_PutRemoveButton (Txt_Remove_students);
      Act_FormEnd ();
     }
   else
     {
      sprintf (Gbl.Message,Txt_No_users_found[Rol_STUDENT],
               Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
  }

/*****************************************************************************/
/***************** Remove all the students from current course ***************/
/*****************************************************************************/

void Enr_RemAllStdsThisCrs (void)
  {
   extern const char *Txt_The_X_students_who_belonged_to_the_course_Y_have_been_removed_from_it;
   extern const char *Txt_No_users_found[Rol_NUM_ROLES];
   unsigned NumStdsInCrs;

   if (Pwd_GetConfirmationOnDangerousAction ())
     {
      if ((NumStdsInCrs = Enr_RemAllStdsInCrs (&Gbl.CurrentCrs.Crs)))
	{
	 sprintf (Gbl.Message,Txt_The_X_students_who_belonged_to_the_course_Y_have_been_removed_from_it,
		  NumStdsInCrs,Gbl.CurrentCrs.Crs.FullName);
	 Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
	}
      else
	{
	 sprintf (Gbl.Message,Txt_No_users_found[Rol_STUDENT],
		  Gbl.CurrentCrs.Crs.FullName);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	}
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
   Usr_GetUsrsLst (Rol_STUDENT,Sco_SCOPE_CRS,NULL,false);
   NumStdsInCrs = Gbl.Usrs.LstStds.NumUsrs;

   /***** Remove all the students *****/
   for (NumUsr = 0;
	NumUsr < NumStdsInCrs;
	NumUsr++)
     {
      Gbl.Usrs.Other.UsrDat.UsrCod = Gbl.Usrs.LstStds.Lst[NumUsr].UsrCod;
      Enr_EffectivelyRemUsrFromCrs (&Gbl.Usrs.Other.UsrDat,Crs,
				    Enr_REMOVE_WORKS,Cns_QUIET);
     }

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstStds);

   return NumStdsInCrs;
  }

/*****************************************************************************/
/************* Form to request sign up in the current course *****************/
/*****************************************************************************/

void Enr_ReqSignUpInCrs (void)
  {
   extern const char *Txt_You_were_already_enrolled_as_X_in_the_course_Y;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   /***** Check if I already belong to course *****/
   if (Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB >= Rol_STUDENT)
     {
      sprintf (Gbl.Message,Txt_You_were_already_enrolled_as_X_in_the_course_Y,
               Txt_ROLES_SINGUL_abc[Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB][Gbl.Usrs.Me.UsrDat.Sex],
               Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
   else if (Gbl.Usrs.Me.LoggedRole == Rol__GUEST_ ||
	    Gbl.Usrs.Me.LoggedRole == Rol_VISITOR)
      /***** Show form to modify only the user's role or the user's data *****/
      Rec_ShowFormSignUpWithMyCommonRecord ();
   else
      Lay_ShowErrorAndExit ("You must be logged to sign up in a course.");        // This never should happen
  }

/*****************************************************************************/
/*********************** Sign up in the current course ***********************/
/*****************************************************************************/

void Enr_SignUpInCrs (void)
  {
   extern const char *Txt_You_were_already_enrolled_as_X_in_the_course_Y;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Your_request_for_enrollment_as_X_in_the_course_Y_has_been_accepted_for_processing;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char UnsignedStr[10+1];
   Rol_Role_t RoleFromForm;
   long ReqCod = -1L;
   unsigned NumUsrsToBeNotifiedByEMail;

   /***** Check if I already belong to course *****/
   if (Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB >= Rol_STUDENT)
     {
      sprintf (Gbl.Message,Txt_You_were_already_enrolled_as_X_in_the_course_Y,
               Txt_ROLES_SINGUL_abc[Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB][Gbl.Usrs.Me.UsrDat.Sex],
               Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
   else
     {
      /***** Get new role from record form *****/
      Par_GetParToText ("Role",UnsignedStr,10);
      RoleFromForm = Rol_ConvertUnsignedStrToRole (UnsignedStr);

      /* Check if role is correct */
      if (!(RoleFromForm == Rol_STUDENT ||
            RoleFromForm == Rol_TEACHER))
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
               Txt_ROLES_SINGUL_abc[RoleFromForm][Gbl.Usrs.Me.UsrDat.Sex],
               Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

      /***** Notify teachers or admins by e-mail about the new enrollment request *****/
      // If this course has teachers ==> send notification to teachers
      // If this course has no teachers and I want to be a teacher ==> send notification to administrators or superusers
      if (Gbl.CurrentCrs.Crs.NumTchs || RoleFromForm == Rol_TEACHER)
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

void Enr_GetNotifEnrollmentRequest (char *SummaryStr,char **ContentStr,
                                    long ReqCod,unsigned MaxChars,bool GetContent)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
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
            DesiredRole = Rol_ConvertUnsignedStrToRole (row[1]);
            strcpy (SummaryStr,Txt_ROLES_SINGUL_Abc[DesiredRole][UsrDat.Sex]);
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

void Enr_AskIfRejectSignUp (void)
  {
   extern const char *Txt_THE_USER_X_is_already_enrolled_in_the_course_Y;
   extern const char *Txt_Do_you_really_want_to_reject_the_enrollment_request_;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
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
         Enr_RemoveEnrollmentRequest (Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Other.UsrDat.UsrCod);
        }
      else        // User does not belong to this course
        {
         Role = Rol_GetRequestedRole (Gbl.Usrs.Other.UsrDat.UsrCod);
         if (Role == Rol_STUDENT ||
             Role == Rol_TEACHER)
           {
            /* Ask if reject */
            sprintf (Gbl.Message,Txt_Do_you_really_want_to_reject_the_enrollment_request_,
                     Gbl.Usrs.Other.UsrDat.FullName,
                     Txt_ROLES_SINGUL_abc[Role][Gbl.Usrs.Other.UsrDat.Sex],
                     Gbl.CurrentCrs.Crs.FullName);
            Lay_ShowAlert (Lay_INFO,Gbl.Message);
            Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

            /* Button to confirm rejection */
            Act_FormStart (ActRejSignUp);
            Usr_PutParamOtherUsrCodEncrypted ();

            Lay_PutConfirmButton (Txt_Reject);
            Act_FormEnd ();
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

void Enr_RejectSignUp (void)
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
      Enr_RemoveEnrollmentRequest (Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Other.UsrDat.UsrCod);

      /* Confirmation message */
      sprintf (Gbl.Message,Txt_Enrollment_of_X_rejected,
               Gbl.Usrs.Other.UsrDat.FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);

   /* Show again the rest of registrarion requests */
   Enr_ShowEnrollmentRequests ();
  }

/*****************************************************************************/
/******** Show pending requests for enrollment in the current course *********/
/*****************************************************************************/

void Enr_ShowEnrollmentRequests (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *The_ClassFormulB[The_NUM_THEMES];
   extern const char *Txt_Scope;
   extern const char *Txt_Users;
   extern const char *Txt_Update;
   extern const char *Txt_Course;
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Requester;
   extern const char *Txt_Role;
   extern const char *Txt_Date;
   extern const char *Txt_Go_to_X;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
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
   fprintf (Gbl.F.Out,"<table style=\"margin:0 auto; border-spacing:4px;\">");

   /* Scope (whole platform, current centre, current degree or current course) */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s\""
                      " style=\"text-align:right; vertical-align:middle;\">"
                      "%s:"
                      "</td>"
                      "<td style=\"text-align:left; vertical-align:middle;\">",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Scope);
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	               1 << Sco_SCOPE_CTY |
                       1 << Sco_SCOPE_INS |
                       1 << Sco_SCOPE_CTR |
                       1 << Sco_SCOPE_DEG |
                       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = Sco_SCOPE_CRS;
   Sco_GetScope ();
   Sco_PutSelectorScope (false);
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");

   /* Users' roles in listing */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s\""
                      " style=\"text-align:right; vertical-align:top;\">"
                      "%s:"
                      "</td>"
                      "<td class=\"DAT\""
                      " style=\"text-align:left; vertical-align:middle;\">",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Users);
   Rol_GetSelectedRoles (&Roles);
   if (!Roles)
      /* Set default roles */
      switch (Gbl.Usrs.Me.LoggedRole)
        {
         case Rol_TEACHER:
            Roles = (1 << Rol_STUDENT) | (1 << Rol_TEACHER);
            break;
         case Rol_DEG_ADM:
         case Rol_SYS_ADM:
            Roles = (1 << Rol_TEACHER);
            break;
         default:
            Lay_ShowErrorAndExit ("You don't have permission to list requesters.");
            break;
        }
   Rol_WriteSelectorRoles (Roles);
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>"
                      "</table>");

   /* Send button */
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Act_LinkFormSubmitAnimated (Txt_Update,The_ClassFormulB[Gbl.Prefs.Theme]);
   Lay_PutCalculateIcon (Txt_Update,Txt_Update);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");

   /***** Build query *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:                // Show requesters for the whole platform
         switch (Gbl.Usrs.Me.LoggedRole)
           {
            case Rol_TEACHER:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM crs_usr,crs_usr_requests"
                              " WHERE crs_usr.UsrCod='%ld'"
                              " AND crs_usr.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.Usrs.Me.UsrDat.UsrCod,
                        Roles);
               break;
            case Rol_DEG_ADM:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM admin,courses,crs_usr_requests"
                              " WHERE admin.UsrCod='%ld' AND admin.Scope='Deg'"
                              " AND admin.Cod=courses.DegCod"
                              " AND courses.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.Usrs.Me.UsrDat.UsrCod,
                        Roles);
               break;
           case Rol_SYS_ADM:
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
      case Sco_SCOPE_INS:                // Show requesters for the current institution
         switch (Gbl.Usrs.Me.LoggedRole)
           {
            case Rol_TEACHER:
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
            case Rol_DEG_ADM:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM admin,centres,degrees,courses,crs_usr_requests"
                              " WHERE admin.UsrCod='%ld' AND admin.Scope='Deg'"
                              " AND centres.InsCod='%ld'"
                              " AND admin.Cod=degrees.DegCod"
                              " AND admin.Cod=courses.DegCod"
                              " AND degrees.CtrCod=centres.CtrCod"
                              " AND courses.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.Usrs.Me.UsrDat.UsrCod,
                        Gbl.CurrentIns.Ins.InsCod,
                        Roles);
               break;
           case Rol_SYS_ADM:
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
      case Sco_SCOPE_CTR:                // Show requesters for the current centre
         switch (Gbl.Usrs.Me.LoggedRole)
           {
            case Rol_TEACHER:
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
            case Rol_DEG_ADM:
               sprintf (Query,"SELECT crs_usr_requests.ReqCod,crs_usr_requests.CrsCod,crs_usr_requests.UsrCod,crs_usr_requests.Role,DATE_FORMAT(crs_usr_requests.RequestTime,'%%Y%%m%%d%%H%%i%%S')"
                              " FROM admin,degrees,courses,crs_usr_requests"
                              " WHERE admin.UsrCod='%ld' AND admin.Scope='Deg'"
                              " AND degrees.CtrCod='%ld'"
                              " AND admin.Cod=degrees.DegCod"
                              " AND admin.Cod=courses.DegCod"
                              " AND courses.CrsCod=crs_usr_requests.CrsCod"
                              " AND ((1<<crs_usr_requests.Role)&%u)<>0"
                              " ORDER BY crs_usr_requests.RequestTime DESC",
                        Gbl.Usrs.Me.UsrDat.UsrCod,
                        Gbl.CurrentCtr.Ctr.CtrCod,
                        Roles);
               break;
           case Rol_SYS_ADM:
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
      case Sco_SCOPE_DEG:        // Show requesters for the current degree
         switch (Gbl.Usrs.Me.LoggedRole)
           {
            case Rol_TEACHER:
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
            case Rol_DEG_ADM:
            case Rol_SYS_ADM:
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
      case Sco_SCOPE_CRS:        // Show requesters for the current course
         switch (Gbl.Usrs.Me.LoggedRole)
           {
            case Rol_TEACHER:
            case Rol_DEG_ADM:
            case Rol_SYS_ADM:
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
                         "<th></th>"
                         "<th class=\"TIT_TBL\""
                         " style=\"text-align:left; vertical-align:top;\">"
                         "%s"
                         "</th>"
                         "<th class=\"TIT_TBL\""
                         " style=\"text-align:right; vertical-align:top;\">"
                         "%s"
                         "</th>"
                         "<th colspan=\"2\" class=\"TIT_TBL\""
                         " style=\"text-align:left; vertical-align:top;\">"
                         "%s"
                         "</th>"
                         "<th class=\"TIT_TBL\""
                         " style=\"text-align:left; vertical-align:top;\">"
                         "%s"
                         "</th>"
                         "<th class=\"TIT_TBL\""
                         " style=\"text-align:center; vertical-align:top;\">"
                         "%s"
                         "</th>"
                         "<th></th>"
                         "<th></th>"
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
                               "<td class=\"DAT\" style=\"text-align:right;"
                               " vertical-align:top;\">"
                               "%u"
                               "</td>",
                     NumRequests - NumReq);

            /***** Link to course *****/
            Crs_GetDataOfCourseByCod (&Crs);
            Deg.DegCod = Crs.DegCod;
            Deg_GetDataOfDegreeByCod (&Deg);
            fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:left;"
        	               " vertical-align:top;\">");
            Act_FormGoToStart (ActSeeCrsInf);
            Crs_PutParamCrsCod (Crs.CrsCod);
            sprintf (Gbl.Title,Txt_Go_to_X,Crs.FullName);
            Act_LinkFormSubmit (Gbl.Title,"DAT");
            fprintf (Gbl.F.Out,"%s &gt; %s"
        	               "</a>",
                     Deg.ShortName,Crs.ShortName);
            Act_FormEnd ();
            fprintf (Gbl.F.Out,"</td>");

            /***** Number of teachers in the course *****/
            fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:right;"
        	               " vertical-align:top;\">"
                               "%u"
                               "</td>",
                     Crs.NumTchs);

            /***** User photo *****/
            fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"width:18px;"
        	               " text-align:center; vertical-align:top;\">");
            ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
            Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                        	                  NULL,
                              "PHOTO18x24",Pho_ZOOM);
            fprintf (Gbl.F.Out,"</td>");

            /***** User name *****/
            fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:left;"
        	               " vertical-align:top;\">");
            Usr_RestrictLengthAndWriteName (&UsrDat,20);

            /***** Requested role (row[3]) *****/
            DesiredRole = Rol_ConvertUnsignedStrToRole (row[3]);
            fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:left;"
        	               " vertical-align:top;\">"
        	               "%s"
        	               "</td>",
                     Txt_ROLES_SINGUL_abc[DesiredRole][UsrDat.Sex]);

            /***** Request time (row[4]) *****/
            Msg_WriteMsgDate (row[4],"DAT");

            /***** Button to confirm the request *****/
            fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:left;"
        	               " vertical-align:top;\">");
            Act_FormStart (ActReqMdfUsr);
            Crs_PutParamCrsCod (Crs.CrsCod);
            Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
            Lay_PutCreateButton (Txt_Register);
            Act_FormEnd ();
            fprintf (Gbl.F.Out,"</td>");

            /***** Button to reject the request *****/
            fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:left;"
        	               " vertical-align:top;\">");
            Act_FormStart (ActReqRejSignUp);
            Crs_PutParamCrsCod (Crs.CrsCod);
            Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
            Lay_PutConfirmButton (Txt_Reject);
            Act_FormEnd ();
            fprintf (Gbl.F.Out,"</td>"
                               "</tr>");

            /***** Mark possible notification as seen *****/
            Ntf_SetNotifAsSeen (Ntf_EVENT_ENROLLMENT_REQUEST,
                                ReqCod,
                                Gbl.Usrs.Me.UsrDat.UsrCod);
           }
         else        // User does not exists or user already belongs to course ==> remove pair from crs_usr_requests table
            Enr_RemoveEnrollmentRequest (Crs.CrsCod,UsrDat.UsrCod);
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
/******************** Remove a request for inscription ***********************/
/*****************************************************************************/

static void Enr_RemoveEnrollmentRequest (long CrsCod,long UsrCod)
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
/********************** Write a form to admin one user ***********************/
/*****************************************************************************/

static void Enr_PutLinkToAdminOneUsr (void)
  {
   extern const char *Txt_Admin_one_user;

   Act_PutContextualLink (ActReqMdfOneUsr,NULL,
                          "configtest",Txt_Admin_one_user);
  }

/*****************************************************************************/
/******************* Write a form to admin several users *********************/
/*****************************************************************************/

static void Enr_PutLinkToAdminSeveralUsrs (void)
  {
   extern const char *Txt_Admin_several_users;

   Act_PutContextualLink (ActReqMdfSevUsr,NULL,
                          "configtest",Txt_Admin_several_users);
  }

/*****************************************************************************/
/************** Form to request the user's ID of another user ****************/
/*****************************************************************************/

void Enr_ReqRegRemUsr (void)
  {
   extern const char *Txt_You_dont_have_permission_to_perform_this_action;

   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol__GUEST_:
      case Rol_VISITOR:
      case Rol_STUDENT:
	 Enr_AskIfRegRemMe ();
	 break;
      case Rol_TEACHER:
	 if (Gbl.CurrentCrs.Crs.CrsCod > 0)
	    Enr_ReqAnotherUsrIDToRegisterRemove ();
	 else
	    Enr_AskIfRegRemMe ();
	 break;
      case Rol_DEG_ADM:
	 if (Gbl.CurrentDeg.Deg.DegCod > 0)
	    Enr_ReqAnotherUsrIDToRegisterRemove ();
	 else
	    Enr_AskIfRegRemMe ();
	 break;
      case Rol_CTR_ADM:
	 if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
	    Enr_ReqAnotherUsrIDToRegisterRemove ();
	 else
	    Enr_AskIfRegRemMe ();
	 break;
      case Rol_INS_ADM:
	 if (Gbl.CurrentIns.Ins.InsCod > 0)
	    Enr_ReqAnotherUsrIDToRegisterRemove ();
	 else
	    Enr_AskIfRegRemMe ();
	 break;
      case Rol_SYS_ADM:
	 Enr_ReqAnotherUsrIDToRegisterRemove ();
	 break;
      default:
	 Lay_ShowAlert (Lay_ERROR,Txt_You_dont_have_permission_to_perform_this_action);
	 break;
     }
  }

/*****************************************************************************/
/****** Write a form to request another user's ID, @nickname or e-mail *******/
/*****************************************************************************/

static void Enr_ReqAnotherUsrIDToRegisterRemove (void)
  {
   extern const char *Txt_Admin_one_user;

   /***** Put contextual links *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0 ||
       Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

      if (Gbl.CurrentCrs.Crs.CrsCod > 0)
         /* Put link to go to admin several users */
	 Enr_PutLinkToAdminSeveralUsrs ();
      else if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
         /* Put link to remove old users */
	 Enr_PutLinkToRemOldUsrs ();

      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,2,Txt_Admin_one_user);
   fprintf (Gbl.F.Out,"<tr>"
                      "<td>");

   /***** Write form to request another user's ID *****/
   Enr_WriteFormToReqAnotherUsrID (ActReqMdfUsr);

   /***** End frame *****/
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/********************** Ask me for register/remove me ************************/
/*****************************************************************************/

static void Enr_AskIfRegRemMe (void)
  {
   struct ListUsrCods ListUsrCods;

   /***** I only can admin me *****/
   Gbl.Usrs.Other.UsrDat.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   ListUsrCods.NumUsrs = 1;
   Usr_AllocateListUsrCods (&ListUsrCods);
   ListUsrCods.Lst[0] = Gbl.Usrs.Other.UsrDat.UsrCod;

   Enr_AskIfRegRemUsr (&ListUsrCods);
  }

/*****************************************************************************/
/****************** Ask me for register/remove another user ******************/
/*****************************************************************************/

void Enr_AskIfRegRemAnotherUsr (void)
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

   Enr_AskIfRegRemUsr (&ListUsrCods);
  }

/*****************************************************************************/
/********************** Ask me for register/remove user **********************/
/*****************************************************************************/

static void Enr_AskIfRegRemUsr (struct ListUsrCods *ListUsrCods)
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

	       Enr_ShowFormToEditOtherUsr ();
	      }
	    else        // User does not belong to the current course
	      {
	       sprintf (Gbl.Message,Txt_THE_USER_X_already_exists_in_Y_but_is_not_yet_enrolled_in_the_course_Z,
			Gbl.Usrs.Other.UsrDat.FullName,Cfg_PLATFORM_SHORT_NAME,Gbl.CurrentCrs.Crs.FullName);
	       Lay_ShowAlert (Lay_INFO,Gbl.Message);

	       Enr_ShowFormToEditOtherUsr ();
	      }
	   }
	 else	// No course selected
	   {
	    sprintf (Gbl.Message,Txt_THE_USER_X_already_exists_in_Y,
		     Gbl.Usrs.Other.UsrDat.FullName,Cfg_PLATFORM_SHORT_NAME);
	    Lay_ShowAlert (Lay_INFO,Gbl.Message);

	    Enr_ShowFormToEditOtherUsr ();
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
	 Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB = Rol_STUDENT;
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
	 Enr_ReqRegRemUsr ();
	}
     }
  }

/*****************************************************************************/
/************ Show form to edit the record of an existing user ***************/
/*****************************************************************************/

static void Enr_ShowFormToEditOtherUsr (void)
  {
   /***** Buttons for edition *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   if (Pwd_CheckIfICanChangeOtherUsrPassword (Gbl.Usrs.Other.UsrDat.UsrCod))
     {
      ID_PutLinkToChangeUsrIDs ();		// Put link (form) to change user's IDs
      Pwd_PutLinkToChangeOtherUsrPassword ();	// Put link (form) to change user's password
     }
   Pho_PutLinkToChangeOtherUsrPhoto ();		// Put link (form) to change user's photo
   fprintf (Gbl.F.Out,"</div>");

   /***** User's record *****/
   Rec_ShowSharedUsrRecord (Rec_FORM_MODIFY_RECORD_OTHER_EXISTING_USR,&Gbl.Usrs.Other.UsrDat);
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
	 ICanRegister = ((Scope == Sco_SCOPE_DEG && Gbl.Usrs.Me.LoggedRole >= Rol_CTR_ADM) ||
                         (Scope == Sco_SCOPE_CTR && Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM) ||
                         (Scope == Sco_SCOPE_INS && Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM));
         if (ICanRegister)
           {
            /***** Register administrator in current institution/centre/degree in database *****/
            Enr_RegisterAdmin (&Gbl.Usrs.Other.UsrDat,Scope,
                               Cod,InsCtrDegName);

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
/**************** Register administrator in current institution **************/
/*****************************************************************************/

static void Enr_RegisterAdmin (struct UsrData *UsrDat,Sco_Scope_t Scope,long Cod,const char *InsCtrDegName)
  {
   extern const char *Sco_ScopeAdminDB[Sco_NUM_SCOPES];
   extern const char *Txt_THE_USER_X_is_already_an_administrator_of_Y;
   extern const char *Txt_THE_USER_X_has_been_enrolled_as_administrator_of_Y;
   char Query[512];

   /***** Check if user was and administrator of current institution/centre/degree *****/
   if (Usr_CheckIfUsrIsAdm (UsrDat->UsrCod,Scope,Cod))
      sprintf (Gbl.Message,Txt_THE_USER_X_is_already_an_administrator_of_Y,
               UsrDat->FullName,InsCtrDegName);
   else        // User was not administrator of current institution/centre/degree
     {
      /***** Insert or replace administrator in current institution/centre/degree *****/
      sprintf (Query,"REPLACE INTO admin (UsrCod,Scope,Cod)"
                     " VALUES ('%ld','%s','%ld')",
               UsrDat->UsrCod,Sco_ScopeAdminDB[Scope],Cod);
      DB_QueryREPLACE (Query,"can not create administrator");

      sprintf (Gbl.Message,Txt_THE_USER_X_has_been_enrolled_as_administrator_of_Y,
               UsrDat->FullName,InsCtrDegName);
     }
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/****************** Ask for remove me from current course ********************/
/*****************************************************************************/

void Enr_ReqRemMeFromCrs (void)
  {
   Enr_AskIfRemoveUsrFromCrs (&Gbl.Usrs.Me.UsrDat,true);
  }

/*****************************************************************************/
/************** Ask for remove of a user from current course *****************/
/*****************************************************************************/

void Enr_ReqRemUsrFromCrs (void)
  {
   Enr_ReqRemOrRemUsrFromCrs (Enr_REQUEST_REMOVE_USR);
  }

/*****************************************************************************/
/********************* Remove a user from current course *********************/
/*****************************************************************************/

void Enr_RemUsrFromCrs (void)
  {
   if (Pwd_GetConfirmationOnDangerousAction ())
      Enr_ReqRemOrRemUsrFromCrs (Enr_REMOVE_USR);
  }

/*****************************************************************************/
/******************** Remove of a user from current course *******************/
/*****************************************************************************/

static void Enr_ReqRemOrRemUsrFromCrs (Enr_ReqDelOrDelUsr_t ReqDelOrDelUsr)
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
      ICanRemove = (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT ? ItsMe :
                                                                 (Gbl.Usrs.Me.LoggedRole >= Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB));
      if (ICanRemove)
	 switch (ReqDelOrDelUsr)
	   {
	    case Enr_REQUEST_REMOVE_USR:        // Ask if remove user from current course
	       Enr_AskIfRemoveUsrFromCrs (&Gbl.Usrs.Other.UsrDat,ItsMe);
	       break;
	    case Enr_REMOVE_USR:                // Remove user from current course
	       Enr_EffectivelyRemUsrFromCrs (&Gbl.Usrs.Other.UsrDat,&Gbl.CurrentCrs.Crs,
	                                     Enr_REMOVE_WORKS,Cns_VERBOSE);
	       break;
	   }
      else
         Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
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
         ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);
         ICanRemove = (ItsMe ||
                       (Scope == Sco_SCOPE_DEG && Gbl.Usrs.Me.LoggedRole >= Rol_CTR_ADM) ||
                       (Scope == Sco_SCOPE_CTR && Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM) ||
                       (Scope == Sco_SCOPE_INS && Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM));
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
               sprintf (Gbl.Message,Txt_THE_USER_X_is_not_an_administrator_of_Y,
                        Gbl.Usrs.Other.UsrDat.FullName,InsCtrDegName);
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
	 ICanRegister = ((Scope == Sco_SCOPE_DEG && Gbl.Usrs.Me.LoggedRole >= Rol_CTR_ADM) ||
                         (Scope == Sco_SCOPE_CTR && Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM) ||
                         (Scope == Sco_SCOPE_INS && Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM));
         if (ICanRegister)
           {
            if (Usr_CheckIfUsrIsAdm (Gbl.Usrs.Other.UsrDat.UsrCod,Scope,Cod))        // User is yet an administrator of current institution/centre/degree
              {
               sprintf (Gbl.Message,Txt_THE_USER_X_is_already_an_administrator_of_Y,
                        Gbl.Usrs.Other.UsrDat.FullName,InsCtrDegName);
               Lay_ShowAlert (Lay_INFO,Gbl.Message);
               Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);
              }
            else
              {
               sprintf (Gbl.Message,Txt_Do_you_really_want_to_register_the_following_user_as_an_administrator_of_X,
                        InsCtrDegName);
               Lay_ShowAlert (Lay_INFO,Gbl.Message);
               Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

               Act_FormStart (Enr_ActNewAdm[Scope]);
               Usr_PutParamOtherUsrCodEncrypted ();
               Lay_PutConfirmButton (Txt_Register_user_IN_A_COURSE_OR_DEGREE);
               Act_FormEnd ();
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
/*************** Accept my enrollment in the current course ******************/
/*****************************************************************************/

void Enr_AcceptRegisterMeInCrs (void)
  {
   extern const char *Txt_You_have_confirmed_your_enrollment_in_the_course_X;

   /***** Confirm my enrollment *****/
   Enr_AcceptUsrInCrs (Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Confirmation message *****/
   sprintf (Gbl.Message,Txt_You_have_confirmed_your_enrollment_in_the_course_X,
            Gbl.CurrentCrs.Crs.FullName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/******************* Create and show data from other user ********************/
/*****************************************************************************/

void Enr_CreatAndShowNewUsrRecordAndRegInCrs (void)
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
      if (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER &&
	  NewRole != Rol_STUDENT)
	 Lay_ShowErrorAndExit ("A teacher only can create new users as students.");

      /***** Get user's name from form *****/
      Rec_GetUsrNameFromRecordForm (&Gbl.Usrs.Other.UsrDat);

      /***** Create user *****/
      Gbl.Usrs.Other.UsrDat.IDs.List[0].Confirmed = true;	// User's ID will be stored as confirmed
      Acc_CreateNewUsr (&Gbl.Usrs.Other.UsrDat);

      /***** Register user in current course in database *****/
      if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
	{
	 if (Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod))      // User does belong to current course, modify his/her role
	    Enr_ModifyRoleInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole,Cns_VERBOSE);
	 else
	    Enr_RegisterUsrInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole,Cns_VERBOSE,
	                                 Enr_SET_ACCEPTED_TO_FALSE);

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

void Enr_ModifAndShowUsrCardAndRegInCrsAndGrps (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char UnsignedStr[10+1];
   unsigned UnsignedNum;
   bool ItsMe;
   bool IAmTeacher;
   bool HeIsTeacher;
   bool ICanChangeUsrName;
   Enr_RegRemOneUsrAction_t RegRemAction;
   Rol_Role_t NewRole;
   bool Error = false;

   /***** Get user from form *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      ItsMe = (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);

      /***** Get the action to do *****/
      Par_GetParToText ("RegRemAction",UnsignedStr,1);
      if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Enr_NUM_ACTIONS_REG_REM_ONE_USR)
	    switch ((RegRemAction = (Enr_RegRemOneUsrAction_t) UnsignedNum))
	      {
	       case Enr_REGISTER_MODIFY_ONE_USR_IN_CRS:
		  if (ItsMe || Gbl.Usrs.Me.LoggedRole >= Rol_TEACHER)
		    {
		     IAmTeacher = (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER);			// I am logged as teacher
		     HeIsTeacher = (Gbl.Usrs.Other.UsrDat.Roles & (1 << Rol_TEACHER));	// He/she is already a teacher in any course
		     ICanChangeUsrName = ItsMe ||
			                 !(IAmTeacher && HeIsTeacher);	// A teacher can not change another teacher's name

		     /***** Get user's name from record form *****/
		     if (ICanChangeUsrName)
			Rec_GetUsrNameFromRecordForm (&Gbl.Usrs.Other.UsrDat);

		     /***** Update user's data in database *****/
		     Enr_UpdateUsrData (&Gbl.Usrs.Other.UsrDat);

		     if (Gbl.CurrentCrs.Crs.CrsCod > 0)
		       {
			/***** Get new role from record form *****/
			NewRole = Rec_GetRoleFromRecordForm ();

			/***** Register user in current course in database *****/
			if (Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.CurrentCrs.Crs.CrsCod))      // User does belong to current course, modify his/her role
			   Enr_ModifyRoleInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole,Cns_VERBOSE);
			else
			   Enr_RegisterUsrInCurrentCrs (&Gbl.Usrs.Other.UsrDat,NewRole,Cns_VERBOSE,
			                                Enr_SET_ACCEPTED_TO_FALSE);

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
	       case Enr_REGISTER_ONE_DEGREE_ADMIN:
		  if (Gbl.Usrs.Me.LoggedRole >= Rol_CTR_ADM)
		     Enr_ReqAddAdm (Sco_SCOPE_DEG,Gbl.CurrentDeg.Deg.DegCod,
		                    Gbl.CurrentDeg.Deg.FullName);
		  else
		     Error = true;
		  break;
	       case Enr_REGISTER_ONE_CENTRE_ADMIN:
		  if (Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM)
		     Enr_ReqAddAdm (Sco_SCOPE_CTR,Gbl.CurrentCtr.Ctr.CtrCod,
		                    Gbl.CurrentCtr.Ctr.FullName);
		  else
		     Error = true;
		  break;
	       case Enr_REGISTER_ONE_INSTITUTION_ADMIN:
		  if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
		     Enr_ReqAddAdm (Sco_SCOPE_INS,Gbl.CurrentIns.Ins.InsCod,
		                    Gbl.CurrentIns.Ins.FullName);
		  else
		     Error = true;
		  break;
	       case Enr_REMOVE_ONE_USR_FROM_CRS:
		  if (ItsMe || Gbl.Usrs.Me.LoggedRole >= Rol_TEACHER)
		     Enr_ReqRemUsrFromCrs ();
		  else
		     Error = true;
		  break;
	       case Enr_REMOVE_ONE_DEGREE_ADMIN:
		  if (ItsMe || Gbl.Usrs.Me.LoggedRole >= Rol_CTR_ADM)
		     Enr_ReqRemAdmOfDeg ();
		  else
		     Error = true;
		  break;
	       case Enr_REMOVE_ONE_CENTRE_ADMIN:
		  if (ItsMe || Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM)
		     Enr_ReqRemAdmOfCtr ();
		  else
		     Error = true;
		  break;
	       case Enr_REMOVE_ONE_INSTITUTION_ADMIN:
		  if (ItsMe || Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
		     Enr_ReqRemAdmOfIns ();
		  else
		     Error = true;
		  break;
	       case Enr_ELIMINATE_ONE_USR_FROM_PLATFORM:
		  if (Acc_CheckIfICanEliminateAccount (ItsMe))
		     Acc_ReqRemUsrGbl ();
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
/********* Set a user's acceptation to true in the current course ************/
/*****************************************************************************/

void Enr_AcceptUsrInCrs (long UsrCod)
  {
   char Query[512];

   /***** Set enrollment of a user to "accepted" in the current course *****/
   sprintf (Query,"UPDATE crs_usr SET Accepted='Y'"
                  " WHERE CrsCod='%ld' AND UsrCod='%ld'",
            Gbl.CurrentCrs.Crs.CrsCod,UsrCod);
   DB_QueryUPDATE (Query,"can not confirm user's enrollment");
  }

/*****************************************************************************/
/******************* Ask if really wanted to remove a user *******************/
/*****************************************************************************/

static void Enr_AskIfRemoveUsrFromCrs (struct UsrData *UsrDat,bool ItsMe)
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

      fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
      Act_FormStart (ActRemUsrCrs);
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Pwd_AskForConfirmationOnDangerousAction ();
      Lay_PutRemoveButton (ItsMe ? Txt_Remove_me_from_this_course :
                                   Txt_Remove_user_from_this_course);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/************************ Remove a user from a course ************************/
/*****************************************************************************/

static void Enr_EffectivelyRemUsrFromCrs (struct UsrData *UsrDat,struct Course *Crs,
                                          Enr_RemoveUsrWorks_t RemoveUsrWorks,Cns_QuietOrVerbose_t QuietOrVerbose)
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
      if (RemoveUsrWorks == Enr_REMOVE_WORKS)
         Brw_RemoveUsrWorksInCrs (UsrDat,Crs,QuietOrVerbose);

      /***** Remove fields of this user in its course record *****/
      Rec_RemoveFieldsCrsRecordInCrs (UsrDat->UsrCod,Crs,QuietOrVerbose);

      /***** Remove some information about files in course and groups *****/
      Brw_RemoveSomeInfoAboutCrsUsrFilesFromDB (UsrDat->UsrCod,Crs->CrsCod);

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
/** Ask if really wanted to remove an administrator from current institution */
/*****************************************************************************/

static void Enr_AskIfRemAdm (bool ItsMe,Sco_Scope_t Scope,const char *InsCtrDegName)
  {
   extern const char *Txt_Do_you_really_want_to_be_removed_as_an_administrator_of_X;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_user_as_an_administrator_of_X;
   extern const char *Txt_Remove_me_as_an_administrator;
   extern const char *Txt_Remove_user_as_an_administrator;
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
      sprintf (Gbl.Message,
               ItsMe ? Txt_Do_you_really_want_to_be_removed_as_an_administrator_of_X :
                       Txt_Do_you_really_want_to_remove_the_following_user_as_an_administrator_of_X,
               InsCtrDegName);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);

      Rec_ShowCommonRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

      Act_FormStart (Enr_ActRemAdm[Scope]);
      Usr_PutParamOtherUsrCodEncrypted ();
      Lay_PutRemoveButton (ItsMe ? Txt_Remove_me_as_an_administrator :
                                   Txt_Remove_user_as_an_administrator);
      Act_FormEnd ();
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
   extern const char *Sco_ScopeAdminDB[Sco_NUM_SCOPES];
   extern const char *Txt_THE_USER_X_has_been_removed_as_administrator_of_Y;
   extern const char *Txt_THE_USER_X_is_not_an_administrator_of_Y;
   char Query[1024];

   if (Usr_CheckIfUsrIsAdm (UsrDat->UsrCod,Scope,Cod))        // User is administrator of current institution/centre/degree
     {
      /***** Remove user from the table of admins *****/
      sprintf (Query,"DELETE FROM admin"
                     " WHERE UsrCod='%ld' AND Scope='%s' AND Cod='%ld'",
               UsrDat->UsrCod,Sco_ScopeAdminDB[Scope],Cod);
      DB_QueryDELETE (Query,"can not remove an administrator");

      sprintf (Gbl.Message,Txt_THE_USER_X_has_been_removed_as_administrator_of_Y,
               UsrDat->FullName,InsCtrDegName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
   else        // User is not an administrator of the current institution/centre/degree
     {
      sprintf (Gbl.Message,Txt_THE_USER_X_is_not_an_administrator_of_Y,
               UsrDat->FullName,InsCtrDegName);
      Lay_ShowAlert (Lay_ERROR,Gbl.Message);
     }
  }
