// swad_group.c: types of groups and groups

/*
    SWAD (Shared Workspace At a Distance),
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For exit, system, malloc, free, rand, etc.
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_assignment_database.h"
#include "swad_attendance_database.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_exam_database.h"
#include "swad_form.h"
#include "swad_game.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_group_database.h"
#include "swad_HTML.h"
#include "swad_match_database.h"
#include "swad_notification.h"
#include "swad_parameter_code.h"
#include "swad_program.h"
#include "swad_project.h"
#include "swad_setting.h"
#include "swad_survey.h"
#include "swad_survey_database.h"
#include "swad_timetable.h"
#include "swad_timetable_database.h"

/*****************************************************************************/
/*************************** Private constants *******************************/
/*****************************************************************************/

#define Grp_GROUP_TYPES_SECTION_ID	"grp_types"
#define Grp_NEW_GROUP_TYPE_SECTION_ID	"new_grp_type"
#define Grp_GROUPS_SECTION_ID		"grps"
#define Grp_NEW_GROUP_SECTION_ID	"new_grp"

/*****************************************************************************/
/************* External global variables from others modules *****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static Usr_Can_t Grp_CheckIfICanChangeGrps (void);

static void Grp_ReqEditGroupsInternal (const struct GroupType *GrpTyp,
				       const struct Group *Grp,
				       Ale_AlertType_t AlertTypeGroupTypes,
                                       const char *AlertTextGroupTypes,
                                       Ale_AlertType_t AlertTypeGroups,
				       const char *AlertTextGroups);
static void Grp_ReqEditGroupsInternal0 (void);
static void Grp_ReqEditGroupsInternal1 (const struct GroupType *GrpTyp,
					Ale_AlertType_t AlertTypeGroupTypes,
                                        const char *AlertTextGroupTypes);
static void Grp_ReqEditGroupsInternal2 (const struct GroupType *GrpTyp,
					const struct Group *Grp,
					Ale_AlertType_t AlertTypeGroups,
                                        const char *AlertTextGroups);

static void Grp_EditGroupTypes (const struct GroupType *GrpTyp);
static void Grp_EditGroups (const struct GroupType *GrpTyp,
			    const struct Group *Grp,
			    const struct Roo_Rooms *Rooms);
static void Grp_PutIconsEditingGroups (__attribute__((unused)) void *Args);

static void Grp_PutCheckboxAllGrps (void);

static Err_SuccessOrError_t Grp_CheckIfNotClosedOrFull (struct ListCodGrps *LstGrpsWant,
						        struct ListCodGrps *LstGrpsBelong);
static void Grp_RemoveUsrFromGrps (Usr_MeOrOther_t MeOrOther,
				   struct ListCodGrps *LstGrpsWant,
				   struct ListCodGrps *LstGrpsBelong);
static void Grp_EnrolUsrInGrps (Usr_MeOrOther_t MeOrOther,
				struct ListCodGrps *LstGrpsWant,
				struct ListCodGrps *LstGrpsBelong);

static void Grp_ConstructorListGrpAlreadySelec (struct ListGrpsAlreadySelec **AlreadyExistsGroupOfType);
static void Grp_DestructorListGrpAlreadySelec (struct ListGrpsAlreadySelec **AlreadyExistsGroupOfType);
static void Grp_RemoveUsrFromGroup (long UsrCod,long GrpCod);

static void Grp_ListGroupTypesForEdition (void);
static void Grp_PutIconsEditingGroupTypes (__attribute__((unused)) void *Args);
static void Grp_PutIconToViewGroups (void);
static void Grp_WriteHeadingGroupTypes (void);

static void Grp_ListGroupsForEdition (const struct Roo_Rooms *Rooms);
static void Grp_WriteHeadingGroups (void);

static void Grp_ListGrpsOfATypeToEditAsgAttSvyEvtMch (Grp_WhichIsAssociatedToGrp_t WhichIsAssociatedToGrp,
						      long Cod,
						      struct GroupType *GrpTyp);

static void Grp_PutIconsMyGroups (__attribute__((unused)) void *Args);

static void Grp_ShowWarningToStdsToChangeGrps (void);
static Usr_Can_t Grp_ListGrpsForChangeMySelection (const struct GroupType *GrpTyp,
						   long SelectedGrpTypCod);
static void Grp_ListGrpsToAddOrRemUsrs (const struct GroupType *GrpTyp,long UsrCod);

static void Grp_ListGrpsForMultipleSelection (const struct GroupType *GrpTyp);
static HTM_Attributes_t Grp_Checked (long GrpCod);

static void Grp_WriteGrpTypOpening (const struct GroupType *GrpTyp);
static void Grp_WriteGrpHead (void);
static void Grp_WriteRowGrp (const struct Group *Grp,Lay_Highlight_t Highlight);
static void Grp_PutFormToCreateGroupType (const struct GroupType *GrpTyp);
static void Grp_PutFormToCreateGroup (const struct GroupType *CurrentGrpTyp,
				      const struct Group *Grp,
				      const struct Roo_Rooms *Rooms);

static Grp_SingleOrMultiple_t Grp_GetSingleMultiple (long GrpTypCod);

static Grp_OptionalOrMandatory_t Grp_GetMandatoryFromYN (char Ch);
static Grp_SingleOrMultiple_t Grp_MultipleFromYN (char Ch);
static Grp_MustBeOpened_t Grp_MustBeOpenedFromYN (char Ch);
static Grp_HasFileZones_t Grp_GetHasFileZonesFromYN (char Ch);

static void Grp_GetLstCodGrpsUsrBelongs (long UsrCod,long GrpTypCod,
                                         struct ListCodGrps *LstGrps,
                                         Grp_ClosedOpenGrps_t ClosedOpenGroups);
static bool Grp_CheckIfGrpIsInList (long GrpCod,struct ListCodGrps *LstGrps);
static bool Grp_CheckIfOpenTimeInTheFuture (time_t OpenTimeUTC);

static void Grp_AskConfirmRemGrp (long GrpCod);
static void Grp_RemoveGroupTypeCompletely (long GrpTypCod);
static void Grp_RemoveGroupCompletely (long GrpCod);

static void Grp_WriteMaxStds (char Str[Cns_MAX_DIGITS_UINT + 1],unsigned MaxStudents);
static void Grp_PutParGrpTypCod (void *GrpTypCod);
static void Grp_PutParGrpCod (void *GrpCod);

/*****************************************************************************/
/**************** Reset data of a group type to default values ***************/
/*****************************************************************************/

void Grp_ResetGroupType (struct GroupType *GrpTyp)
  {
   GrpTyp->GrpTypCod = -1L;
   GrpTyp->Name[0] = '\0';
   GrpTyp->Enrolment.OptionalMandatory = Grp_MANDATORY;
   GrpTyp->Enrolment.SingleMultiple    = Grp_SINGLE;
   GrpTyp->MustBeOpened = Grp_MUST_NOT_BE_OPENED;
   GrpTyp->OpenTimeUTC = (time_t) 0;
  }

/*****************************************************************************/
/**************** Reset data of a group type to default values ***************/
/*****************************************************************************/

void Grp_ResetGroup (struct Group *Grp)
  {
   Rol_Role_t Role;

   Grp->GrpCod = -1L;
   Grp->Name[0] = '\0';
   Grp->Room.RooCod = -1L;
   Grp->Room.ShrtName[0] = '\0';
   for (Role  = (Rol_Role_t) 0;
	Role <= (Rol_Role_t) (Rol_NUM_ROLES - 1);
	Role++)
      Grp->NumUsrs[Role] = 0;
   Grp->MaxStds = Grp_NUM_STUDENTS_NOT_LIMITED;
   Grp->Open = CloOpe_CLOSED;
   Grp->HasFileZones = Grp_HAS_NOT_FILEZONES;
  }

/*****************************************************************************/
/************************ Check if I can change groups ***********************/
/*****************************************************************************/

static Usr_Can_t Grp_CheckIfICanChangeGrps (void)
  {
   static Usr_Can_t Grp_ICanChangeGrps[Rol_NUM_ROLES] =
     {
      [Rol_UNK    ] = Usr_CAN_NOT,
      [Rol_GST    ] = Usr_CAN_NOT,
      [Rol_USR    ] = Usr_CAN_NOT,
      [Rol_STD    ] = Usr_CAN,
      [Rol_NET    ] = Usr_CAN_NOT,
      [Rol_TCH    ] = Usr_CAN,
      [Rol_DEG_ADM] = Usr_CAN_NOT,
      [Rol_CTR_ADM] = Usr_CAN_NOT,
      [Rol_INS_ADM] = Usr_CAN_NOT,
      [Rol_SYS_ADM] = Usr_CAN,
     };

   return Grp_ICanChangeGrps[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/******************* Write the names of the selected groups ******************/
/*****************************************************************************/

void Grp_WriteNamesOfSelectedGrps (void)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_users_with_no_group;
   long GrpCod;
   unsigned NumGrpSel;
   long CrsCod;
   struct GroupType GrpTyp;
   struct Group Grp;

   /***** Show the selected groups *****/
   HTM_Txt (Gbl.Crs.Grps.LstGrpsSel.NumGrps == 1 ? Txt_Group  :
                                                   Txt_Groups);
   HTM_Colon (); HTM_NBSP ();
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
     {
      if ((GrpCod = Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]) >= 0)
        {
         Grp.GrpCod = GrpCod;
         Grp_GetGroupDataByCod (&CrsCod,&GrpTyp.GrpTypCod,&Grp);
	 Grp_GetGroupTypeDataByCod (&GrpTyp);
         HTM_Txt (GrpTyp.Name);
	 HTM_SP ();
	 HTM_Txt (Grp.Name);
        }
      else	// GrpCod < 0 ==> students not belonging to any group of type (-GrpCod)
        {
         GrpTyp.GrpTypCod = -GrpCod;
         Grp_GetGroupTypeDataByCod (&GrpTyp);
	 HTM_Txt (GrpTyp.Name);
	 HTM_NBSP ();
	 HTM_ParTxtPar (Txt_users_with_no_group);
        }

      /* Write separator */
      HTM_ListSeparator (NumGrpSel,Gbl.Crs.Grps.LstGrpsSel.NumGrps);
     }
  }

/*****************************************************************************/
/************************** Put forms to edit groups *************************/
/*****************************************************************************/

void Grp_ReqEditGroups (void)
  {
   struct GroupType GrpTyp;
   struct Group Grp;

   Grp_ResetGroupType (&GrpTyp);
   Grp_ResetGroup (&Grp);
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,Ale_INFO,NULL,Ale_INFO,NULL);
  }

static void Grp_ReqEditGroupsInternal (const struct GroupType *GrpTyp,
				       const struct Group *Grp,
				       Ale_AlertType_t AlertTypeGroupTypes,
                                       const char *AlertTextGroupTypes,
                                       Ale_AlertType_t AlertTypeGroups,
				       const char *AlertTextGroups)
  {
   Grp_ReqEditGroupsInternal0 ();
   Grp_ReqEditGroupsInternal1 (GrpTyp,AlertTypeGroupTypes,AlertTextGroupTypes);
   Grp_ReqEditGroupsInternal2 (GrpTyp,Grp,AlertTypeGroups,AlertTextGroups);
  }

static void Grp_ReqEditGroupsInternal0 (void)
  {
   /***** Begin groups types section *****/
   HTM_SECTION_Begin (Grp_GROUP_TYPES_SECTION_ID);
  }

static void Grp_ReqEditGroupsInternal1 (const struct GroupType *GrpTyp,
					Ale_AlertType_t AlertTypeGroupTypes,
                                        const char *AlertTextGroupTypes)
  {
      /***** Get list of groups types and groups in this course *****/
      Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ALL_GROUP_TYPES);

      /***** Show optional alert *****/
      if (AlertTextGroupTypes)
	 if (AlertTextGroupTypes[0])
	    Ale_ShowAlert (AlertTypeGroupTypes,AlertTextGroupTypes);

      /***** Put form to edit group types *****/
      Grp_EditGroupTypes (GrpTyp);

   /***** End groups types section *****/
   HTM_SECTION_End ();

   /***** Begin groups section *****/
   HTM_SECTION_Begin (Grp_GROUPS_SECTION_ID);
  }

static void Grp_ReqEditGroupsInternal2 (const struct GroupType *GrpTyp,
					const struct Group *Grp,
					Ale_AlertType_t AlertTypeGroups,
                                        const char *AlertTextGroups)
  {
   struct Roo_Rooms Rooms;

      /***** Reset rooms context *****/
      Roo_ResetRooms (&Rooms);

      /***** Show optional alert *****/
      if (AlertTextGroups)
	 if (AlertTextGroups[0])
	    Ale_ShowAlert (AlertTypeGroups,AlertTextGroups);

      /***** Get list of rooms in this center *****/
      Roo_GetListRooms (&Rooms,Roo_ONLY_SHRT_NAME);

      /***** Put form to edit groups *****/
      if (Gbl.Crs.Grps.GrpTypes.NumGrpTypes) // If there are group types...
	 Grp_EditGroups (GrpTyp,Grp,&Rooms);

   /***** End groups section *****/
   HTM_SECTION_End ();

   /***** Free list of rooms in this center *****/
   Roo_FreeListRooms (&Rooms);

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/************************* Put forms to edit group types *********************/
/*****************************************************************************/

static void Grp_EditGroupTypes (const struct GroupType *GrpTyp)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *Txt_Types_of_group;
   extern const char *Txt_There_are_no_types_of_group_in_the_course_X;

   /***** Begin box *****/
   Box_BoxBegin (Txt_Types_of_group,Grp_PutIconsEditingGroupTypes,NULL,
                 Hlp_USERS_Groups,Box_NOT_CLOSABLE);

      /***** Put a form to create a new group type *****/
      Grp_PutFormToCreateGroupType (GrpTyp);

      /***** Forms to edit current group types *****/
      if (Gbl.Crs.Grps.GrpTypes.NumGrpTypes)	// Group types found...
	 Grp_ListGroupTypesForEdition ();
      else	// No group types found in this course
	 Ale_ShowAlert (Ale_INFO,Txt_There_are_no_types_of_group_in_the_course_X,
			Gbl.Hierarchy.Node[Hie_CRS].ShrtName);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/**************************** Put forms to edit groups ***********************/
/*****************************************************************************/

static void Grp_EditGroups (const struct GroupType *GrpTyp,
			    const struct Group *Grp,
			    const struct Roo_Rooms *Rooms)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *Txt_Groups;
   extern const char *Txt_No_groups_have_been_created_in_the_course_X;

   /***** Begin box *****/
   Box_BoxBegin (Txt_Groups,Grp_PutIconsEditingGroups,NULL,
                 Hlp_USERS_Groups,Box_NOT_CLOSABLE);

      /***** Put a form to create a new group *****/
      Grp_PutFormToCreateGroup (GrpTyp,Grp,Rooms);

      /***** Forms to edit current groups *****/
      if (Gbl.Crs.Grps.GrpTypes.NumGrpsTotal)	// If there are groups...
	 Grp_ListGroupsForEdition (Rooms);
      else	// There are group types, but there aren't groups
	 Ale_ShowAlert (Ale_INFO,Txt_No_groups_have_been_created_in_the_course_X,
			Gbl.Hierarchy.Node[Hie_CRS].ShrtName);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of groups ******************/
/*****************************************************************************/

static void Grp_PutIconsEditingGroups (__attribute__((unused)) void *Args)
  {
   Grp_PutIconToViewGroups ();
  }

/*****************************************************************************/
/*************** Show form to select one or several groups *******************/
/*****************************************************************************/

void Grp_ShowFormToSelectSeveralGroups (void (*FuncPars) (void *Args),void *Args,
				        const char *OnSubmit)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *Txt_Groups;
   extern const char *Txt_Update_users;
   unsigned NumGrpTyp;
   struct GroupType *GrpTyp;

   /***** Trivial check: if no groups ==> nothing to do *****/
   if (!Gbl.Crs.Grps.NumGrps)
      return;

   /***** Begin box *****/
   Box_BoxBegin (Txt_Groups,
		 Grp_PutIconsMyGroups,NULL,
		 Hlp_USERS_Groups,Box_CLOSABLE);

      /***** Begin form to update the students listed
	     depending on the groups selected *****/
      Frm_BeginFormAnchorOnSubmit (Gbl.Action.Act,Usr_USER_LIST_SECTION_ID,OnSubmit);
	 Set_PutParsPrefsAboutUsrList ();
	 if (FuncPars)
	    FuncPars (Args);

	 /***** Select all groups *****/
	 Grp_PutCheckboxAllGrps ();

	 /***** Get list of groups types and groups in this course *****/
	 Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_GRP_TYPES_WITH_GROUPS);

	 /***** List the groups for each group type *****/
	 for (NumGrpTyp = 0;
	      NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	      NumGrpTyp++)
	   {
	    GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

	    if (GrpTyp->NumGrps)
	       Grp_ListGrpsForMultipleSelection (GrpTyp);
	   }

	 /***** Free list of groups types and groups in this course *****/
	 Grp_FreeListGrpTypesAndGrps ();

	 /***** Submit button *****/
	 Lay_WriteLinkToUpdate (Txt_Update_users,NULL);

      /***** End form *****/
      Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************* Put checkbox to select all groups ***********************/
/*****************************************************************************/

static void Grp_PutCheckboxAllGrps (void)
  {
   extern const char *Txt_All_groups;
   Usr_Can_t ICanSelUnselGroup;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 ICanSelUnselGroup = Usr_CAN;
	 break;
      default:
	 ICanSelUnselGroup = Usr_CAN_NOT;
	 break;
     }

   HTM_DIV_Begin ("class=\"CONTEXT_OPT\"");
      HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	 HTM_INPUT_CHECKBOX ("AllGroups",
			     ICanSelUnselGroup == Usr_CAN ? (Gbl.Crs.Grps.AllGrpsSel ? HTM_CHECKED :
										       HTM_NO_ATTR) :
							    HTM_DISABLED,
			     "value=\"Y\"%s",
			     ICanSelUnselGroup == Usr_CAN ? " onclick=\"togglecheckChildren(this,'GrpCods')\"" :
							    "");
	 HTM_Txt (Txt_All_groups);
      HTM_LABEL_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/****************** Put parameters with the groups selected ******************/
/*****************************************************************************/

void Grp_PutParsCodGrps (void)
  {
   extern const char *Par_SEPARATOR_PARAM_MULTIPLE;
   unsigned NumGrpSel;
   size_t MaxLengthGrpCods;
   char *GrpCods;
   char GrpCod[Cns_MAX_DIGITS_LONG + 1];

   /***** Write the boolean parameter that indicates if all groups must be listed *****/
   Par_PutParChar ("AllGroups",
		   Gbl.Crs.Grps.AllGrpsSel ? 'Y' :
					     'N');

   /***** Write the parameter with the list of group codes to show *****/
   if (!Gbl.Crs.Grps.AllGrpsSel &&
        Gbl.Crs.Grps.LstGrpsSel.NumGrps)
     {
      MaxLengthGrpCods = Gbl.Crs.Grps.LstGrpsSel.NumGrps * (Cns_MAX_DIGITS_LONG + 1) - 1;
      if ((GrpCods = malloc (MaxLengthGrpCods + 1)) == NULL)
	 Err_NotEnoughMemoryExit ();
      GrpCods[0] = '\0';

      for (NumGrpSel = 0;
	   NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	   NumGrpSel++)
        {
         /* Append group code to list */
         if (NumGrpSel)
            Str_Concat (GrpCods,Par_SEPARATOR_PARAM_MULTIPLE,MaxLengthGrpCods);
         snprintf (GrpCod,sizeof (GrpCod),"%ld",
		   Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
         Str_Concat (GrpCods,GrpCod,MaxLengthGrpCods);
        }

      Par_PutParString (NULL,"GrpCods",GrpCods);
      free (GrpCods);
     }
  }

/*****************************************************************************/
/************** Put parameters with the default groups         ***************/
/************** associated to exam session or attendance event ***************/
/*****************************************************************************/

void Grp_PutParsCodGrpsAssociated (Grp_Association_t Association,long Cod)
  {
   extern const char *Par_SEPARATOR_PARAM_MULTIPLE;
   static unsigned (*GetGrpCodsAssociatedTo[Grp_NUM_ASSOCIATIONS]) (MYSQL_RES **mysql_res,
								    long Cod) =
     {
      [Grp_EXAM_SESSION    ] = Exa_DB_GetGrpCodsAssociatedToSes,
      [Grp_ATTENDANCE_EVENT] = Att_DB_GetGrpCodsAssociatedToEvent,
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrp;
   unsigned NumGrps;
   size_t MaxLengthGrpCods;
   char *GrpCods;

   /***** Get groups associated from database *****/
   if (Gbl.Crs.Grps.NumGrps)
      NumGrps = GetGrpCodsAssociatedTo[Association] (&mysql_res,Cod);
   else
      NumGrps = 0;

   /***** Get groups *****/
   if (NumGrps) // Groups found...
     {
      MaxLengthGrpCods = NumGrps * (1 + 20) - 1;
      if ((GrpCods = malloc (MaxLengthGrpCods + 1)) == NULL)
	 Err_NotEnoughMemoryExit ();
      GrpCods[0] = '\0';

      /* Get groups */
      for (NumGrp = 0;
	   NumGrp < NumGrps;
	   NumGrp++)
        {
         /* Get next group */
         row = mysql_fetch_row (mysql_res);

         /* Append group code to list */
         if (NumGrp)
            Str_Concat (GrpCods,Par_SEPARATOR_PARAM_MULTIPLE,MaxLengthGrpCods);
         Str_Concat (GrpCods,row[0],MaxLengthGrpCods);
        }

      Par_PutParString (NULL,"GrpCods",GrpCods);
      free (GrpCods);
     }
   else
      /***** Write the boolean parameter that indicates
             if all groups should be listed *****/
      Par_PutParChar ("AllGroups",'Y');

   /***** Free structure that stores the query result *****/
   if (Gbl.Crs.Grps.NumGrps)
      DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************** Get parameters related to groups selected ******************/
/*****************************************************************************/

void Grp_GetParCodsSeveralGrpsToShowUsrs (void)
  {
   struct ListCodGrps LstGrpsIBelong;
   unsigned NumGrp;

   if (++Gbl.Crs.Grps.LstGrpsSel.NestedCalls > 1) // If list is created yet, there's nothing to do
      return;

   /***** Get boolean parameter that indicates if all groups must be listed *****/
   Gbl.Crs.Grps.AllGrpsSel = Par_GetParBool ("AllGroups");

   /***** Get parameter with list of groups selected *****/
   Grp_GetParCodsSeveralGrps ();

   if ( Gbl.Crs.Grps.NumGrps &&			// This course has groups and...
       !Gbl.Crs.Grps.LstGrpsSel.NumGrps)	// ...I haven't selected any group
     {
      /***** I I haven't selected any group, show by default the groups I belong to *****/
      /* Get list of groups of all types in current course I belong to */
      Grp_GetLstCodGrpsUsrBelongs (Gbl.Usrs.Me.UsrDat.UsrCod,-1L,
                                   &LstGrpsIBelong,
                                   Grp_CLOSED_AND_OPEN_GROUPS);

      if (LstGrpsIBelong.NumGrps)
	{
	 /* Allocate space for list of selected groups */
	 if ((Gbl.Crs.Grps.LstGrpsSel.GrpCods = calloc (LstGrpsIBelong.NumGrps,
	                                                sizeof (*Gbl.Crs.Grps.LstGrpsSel.GrpCods))) == NULL)
	    Err_NotEnoughMemoryExit ();

	 /* Fill list of selected groups with list of groups I belong to */
	 for (NumGrp = 0;
	      NumGrp < LstGrpsIBelong.NumGrps;
	      NumGrp++)
	    Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrp] = LstGrpsIBelong.GrpCods[NumGrp];
	 Gbl.Crs.Grps.LstGrpsSel.NumGrps = LstGrpsIBelong.NumGrps;
	}

      /* Free list of groups I belong to */
      Grp_FreeListCodGrp (&LstGrpsIBelong);
     }

   /***** If no groups selected ==> show all groups *****/
   if (!Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Gbl.Crs.Grps.AllGrpsSel = true;
  }

/*****************************************************************************/
/**************** Get parameter with list of groups selected *****************/
/*****************************************************************************/

void Grp_GetParCodsSeveralGrps (void)
  {
   char *ParLstCodGrps;
   const char *Ptr;
   char LongStr[Cns_MAX_DIGITS_LONG + 1];
   unsigned NumGrp;
   unsigned long MaxSizeLstGrpCods;

   /***** Reset number of groups selected *****/
   Gbl.Crs.Grps.LstGrpsSel.NumGrps = 0;

   if (Gbl.Crs.Grps.NumGrps)	// If course has groups
     {
      MaxSizeLstGrpCods = ((Cns_MAX_DIGITS_LONG + 1) * Gbl.Crs.Grps.NumGrps) - 1;

      /***** Allocate memory for the list of group codes selected *****/
      if ((ParLstCodGrps = malloc (MaxSizeLstGrpCods + 1)) == NULL)
	 Err_NotEnoughMemoryExit ();

      /***** Get parameter with list of groups to list *****/
      Par_GetParMultiToText ("GrpCods",ParLstCodGrps,MaxSizeLstGrpCods);

      if (ParLstCodGrps[0])
	{
	 /***** Count number of groups selected from LstCodGrps *****/
	 for (Ptr = ParLstCodGrps, NumGrp = 0;
	      *Ptr;
	      NumGrp++)
	    Par_GetNextStrUntilSeparParMult (&Ptr,LongStr,Cns_MAX_DIGITS_LONG);
	 Gbl.Crs.Grps.LstGrpsSel.NumGrps = NumGrp;

	 /***** If I have selected any group... *****/
	 if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
	   {
	    /***** ...create a list of groups selected *****/
	    if ((Gbl.Crs.Grps.LstGrpsSel.GrpCods = calloc (Gbl.Crs.Grps.LstGrpsSel.NumGrps,
	                                                   sizeof (*Gbl.Crs.Grps.LstGrpsSel.GrpCods))) == NULL)
	       Err_NotEnoughMemoryExit ();
	    for (Ptr = ParLstCodGrps, NumGrp = 0;
		 *Ptr;
		 NumGrp++)
	      {
	       Par_GetNextStrUntilSeparParMult (&Ptr,LongStr,Cns_MAX_DIGITS_LONG);
	       Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrp] = Str_ConvertStrCodToLongCod (LongStr);
	      }
	   }
	}

      /***** Free memory used for the list of groups to show *****/
      free (ParLstCodGrps);
     }
  }

/*****************************************************************************/
/********* Free memory used for the list of group codes selected *************/
/*****************************************************************************/

void Grp_FreeListCodSelectedGrps (void)
  {
   if (Gbl.Crs.Grps.LstGrpsSel.NestedCalls > 0)
      if (--Gbl.Crs.Grps.LstGrpsSel.NestedCalls == 0)
         if (Gbl.Crs.Grps.LstGrpsSel.GrpCods)
           {
            free (Gbl.Crs.Grps.LstGrpsSel.GrpCods);
            Gbl.Crs.Grps.LstGrpsSel.GrpCods = NULL;
            Gbl.Crs.Grps.LstGrpsSel.NumGrps = 0;
           }
  }

/*****************************************************************************/
/******************* Change my groups and show form again ********************/
/*****************************************************************************/

void Grp_ChangeMyGrpsAndShowChanges (void)
  {
   /***** Change my groups *****/
   Grp_ChangeUsrGrps (Usr_ME,Cns_VERBOSE);

   /***** Show possible alerts *****/
   Ale_ShowAlerts (NULL);

   /***** Show again the table of selection of groups with the changes already made *****/
   Grp_ShowLstGrpsToChgMyGrps ();
  }

/*****************************************************************************/
/********************** Change groups of another user ************************/
/*****************************************************************************/

void Grp_ChangeUsrGrps (Usr_MeOrOther_t MeOrOther,Cns_Verbose_t Verbose)
  {
   extern const char *Txt_The_requested_group_changes_were_successful;
   extern const char *Txt_There_has_been_no_change_in_groups;
   extern const char *Txt_In_a_type_of_group_with_single_enrolment_students_can_not_be_enroled_in_more_than_one_group;
   extern struct Usr_Data *Usr_UsrDat[Usr_NUM_ME_OR_OTHER];
   struct ListCodGrps LstGrpsUsrWants;
   Err_SuccessOrError_t SelectionIsValid = Err_SUCCESS;
   Err_SuccessOrError_t ChangesMade;

   /***** Can I change another user's groups? *****/
   if (Grp_CheckIfICanChangeGrps () == Usr_CAN_NOT)
      return;

   /***** Get list of groups types and groups in current course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_GRP_TYPES_WITH_GROUPS);

   /***** Get the list of groups to which enrol this user *****/
   LstGrpsUsrWants.GrpCods = NULL;	// Initialized to avoid bug reported by Coverity
   LstGrpsUsrWants.NumGrps = 0;	// Initialized to avoid bug reported by Coverity
   Grp_GetLstCodsGrpWanted (&LstGrpsUsrWants);

   /***** A student can not be enroled in more than one group
	  if the type of group is of single enrolment *****/
   switch (Usr_UsrDat[MeOrOther]->Roles.InCurrentCrs)
     {
      case Rol_STD:
	 SelectionIsValid = Grp_CheckIfAtMostOneSingleEnrolmentGrpIsSelected (&LstGrpsUsrWants,
									      MeOrOther == Usr_ME);	// Check closed groups?
         break;
      case Rol_NET:
      case Rol_TCH:
         break;
      default:
         Err_WrongRoleExit ();
         break;
     }

   /***** Free list of groups types and groups in this course *****/
   // The lists of group types and groups need to be freed here...
   // ...in order to get them again when changing groups atomically
   Grp_FreeListGrpTypesAndGrps ();

   /***** Enrol user in the selected groups *****/
   switch (SelectionIsValid)
     {
      case Err_SUCCESS:
	 ChangesMade = Grp_ChangeGrpsAtomically (MeOrOther,&LstGrpsUsrWants);
	 if (Verbose == Cns_VERBOSE)
	    switch (ChangesMade)
	      {
	       case Err_SUCCESS:
		  Ale_CreateAlert (Ale_SUCCESS,NULL,
				   Txt_The_requested_group_changes_were_successful);
		  break;
	       case Err_ERROR:
	       default:
		  Ale_CreateAlert (Ale_WARNING,NULL,
				   Txt_There_has_been_no_change_in_groups);
		  break;
	      }
	 break;
      case Err_ERROR:
      default:
         if (Verbose == Cns_VERBOSE)
	    Ale_CreateAlert (Ale_WARNING,NULL,
			     Txt_In_a_type_of_group_with_single_enrolment_students_can_not_be_enroled_in_more_than_one_group);
         break;
     }

   /***** Free memory with the list of groups to/from which enrol/remove users *****/
   Grp_FreeListCodGrp (&LstGrpsUsrWants);
  }

/*****************************************************************************/
/***************** Change another user's groups atomically *******************/
/*****************************************************************************/
// Return Err_SUCCESS if desired changes are made

Err_SuccessOrError_t Grp_ChangeGrpsAtomically (Usr_MeOrOther_t MeOrOther,
					       struct ListCodGrps *LstGrpsUsrWants)
  {
   extern struct Usr_Data *Usr_UsrDat[Usr_NUM_ME_OR_OTHER];
   struct ListCodGrps LstGrpsUsrBelongs;
   Err_SuccessOrError_t SelectionIsValid;

   /***** Lock tables to make the enrolment atomic *****/
   if (Usr_UsrDat[MeOrOther]->Roles.InCurrentCrs == Rol_STD)
      Grp_DB_LockTables ();

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_GRP_TYPES_WITH_GROUPS);

   /***** Query in the database the group codes which user belongs to *****/
   Grp_GetLstCodGrpsUsrBelongs (Usr_UsrDat[MeOrOther]->UsrCod,-1L,
				&LstGrpsUsrBelongs,
                                Grp_CLOSED_AND_OPEN_GROUPS);

   /***** Check if selection of groups is valid *****/
   SelectionIsValid = Err_SUCCESS;
   if (MeOrOther == Usr_ME && Gbl.Usrs.Me.Role.Logged == Rol_STD)	// It's me, a student, trying to changing my groups
      /* Go across the received list of groups which I want to belong
	 and check that they are not closed or full */
      SelectionIsValid = Grp_CheckIfNotClosedOrFull (LstGrpsUsrWants,&LstGrpsUsrBelongs);

   if (SelectionIsValid == Err_SUCCESS)
     {
      /***** Go across the list of groups user belongs to,
	     removing her/him from those groups not wanted *****/
      Grp_RemoveUsrFromGrps (MeOrOther,LstGrpsUsrWants,&LstGrpsUsrBelongs);

      /***** Go across the list of groups wanted,
	     adding user to those groups to which user doesn't belong to *****/
      Grp_EnrolUsrInGrps (MeOrOther,LstGrpsUsrWants,&LstGrpsUsrBelongs);
     }

   /***** Unlock tables after changes in groups *****/
   if (Usr_UsrDat[MeOrOther]->Roles.InCurrentCrs == Rol_STD)
      DB_UnlockTables ();

   /***** Free memory with the list of groups which user belonged to *****/
   Grp_FreeListCodGrp (&LstGrpsUsrBelongs);

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();

   return SelectionIsValid;
  }

/*****************************************************************************/
/********* Go across the list of groups I belong to,               ***********/
/********* removing me from those open groups that are not present ***********/
/********* in the received list of groups I want to enrol in       ***********/
/*****************************************************************************/
// Return Err_SUCCESS is selection is valid

static Err_SuccessOrError_t Grp_CheckIfNotClosedOrFull (struct ListCodGrps *LstGrpsWant,
						        struct ListCodGrps *LstGrpsBelong)
  {
   unsigned NumGrpWant;
   long GrpCodWant;
   unsigned NumGrpTyp;
   unsigned NumGrpThisType;
   struct GroupType *GrpTyp;
   struct Group *Grp;

   /***** Go across the received list of groups which I want to belong
	  and check that they are not closed or full *****/
   for (NumGrpWant = 0;
	NumGrpWant < LstGrpsWant->NumGrps;
	NumGrpWant++)
     {
      GrpCodWant = LstGrpsWant->GrpCods[NumGrpWant];

      if (!Grp_CheckIfGrpIsInList (GrpCodWant,LstGrpsBelong))	// I want this group but I don't belong to it
	 /* Check if the group is closed or full */
	 for (NumGrpTyp = 0;
	      NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	      NumGrpTyp++)
	   {
	    GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

	    for (NumGrpThisType = 0;
		 NumGrpThisType < GrpTyp->NumGrps;
		 NumGrpThisType++)
	      {
	       Grp = &GrpTyp->LstGrps[NumGrpThisType];

	       if (GrpCodWant == Grp->GrpCod)
		 {
		  /* Check if the group is closed */
		  if (Grp->Open == CloOpe_CLOSED)
		     return Err_ERROR;	// Selection is not valid

		  /* Check if the group is full */
		  if (Grp->NumUsrs[Rol_STD] >= Grp->MaxStds)
		     return Err_ERROR;	// Selection is not valid
		 }
	      }
	   }
     }

   return Err_SUCCESS;	// Selection is valid
  }

/*****************************************************************************/
/************** Go across the list of groups users belong to, ****************/
/************** removing user from those groups not wanted    ****************/
/*****************************************************************************/

static void Grp_RemoveUsrFromGrps (Usr_MeOrOther_t MeOrOther,
				   struct ListCodGrps *LstGrpsWant,
				   struct ListCodGrps *LstGrpsBelong)
  {
   extern struct Usr_Data *Usr_UsrDat[Usr_NUM_ME_OR_OTHER];
   unsigned NumGrpBelong;
   long GrpCodBelong;
   bool RemoveUsrFromThisGrp;
   unsigned NumGrpTyp;
   unsigned NumGrpThisType;
   struct GroupType *GrpTyp;
   struct Group *Grp;

   for (NumGrpBelong = 0;
	NumGrpBelong < LstGrpsBelong->NumGrps;
	NumGrpBelong++)
     {
      GrpCodBelong = LstGrpsBelong->GrpCods[NumGrpBelong];

      /* Step 1: if user checked this group ==> don't remove */
      RemoveUsrFromThisGrp = !Grp_CheckIfGrpIsInList (GrpCodBelong,LstGrpsWant);	// User belong to this group but he/she don't want it

      /* Step 2 (only for a student changing its groups):
         cancel removing if the group is closed */
      if (RemoveUsrFromThisGrp && MeOrOther == Usr_ME && Gbl.Usrs.Me.Role.Logged == Rol_STD)
	 /* Find if this group is closed.
	    - If the group is closed,
	      it was not sent in the received list of groups I want
	      because it was disabled ==> don't remove me.
	    - If the group is open,
	      it was not sent in the received list of groups I want
	      because I unchecked it ==> remove me. */
	 for (NumGrpTyp = 0;
	      NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes && RemoveUsrFromThisGrp;
	      NumGrpTyp++)
	   {
	    GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

	    for (NumGrpThisType = 0;
		 NumGrpThisType < GrpTyp->NumGrps && RemoveUsrFromThisGrp;
		 NumGrpThisType++)
	      {
	       Grp = &GrpTyp->LstGrps[NumGrpThisType];

	       if (GrpCodBelong == Grp->GrpCod && Grp->Open == CloOpe_CLOSED)
		  RemoveUsrFromThisGrp = false;
	      }
	   }

      if (RemoveUsrFromThisGrp)
	 Grp_RemoveUsrFromGroup (Usr_UsrDat[MeOrOther]->UsrCod,GrpCodBelong);
     }
  }

/*****************************************************************************/
/****** Go across the received list of groups that I want to enrol in, *******/
/****** adding me to those groups that I don't belong to               *******/
/*****************************************************************************/

static void Grp_EnrolUsrInGrps (Usr_MeOrOther_t MeOrOther,
				   struct ListCodGrps *LstGrpsWant,
				   struct ListCodGrps *LstGrpsBelong)
  {
   extern struct Usr_Data *Usr_UsrDat[Usr_NUM_ME_OR_OTHER];
   unsigned NumGrpWant;
   long GrpCodWant;

   for (NumGrpWant = 0;
	NumGrpWant < LstGrpsWant->NumGrps;
	NumGrpWant++)
     {
      GrpCodWant = LstGrpsWant->GrpCods[NumGrpWant];

      if (!Grp_CheckIfGrpIsInList (GrpCodWant,LstGrpsBelong))	// User wants this group but he/she don't belong to it
	 Grp_DB_AddUsrToGrp (Usr_UsrDat[MeOrOther]->UsrCod,GrpCodWant);
     }
  }

/*****************************************************************************/
/******* Check if at most one single-enrolment group has been selected *******/
/*****************************************************************************/
 // Return Err_SUCCESS if the selection of groups is valid

Err_SuccessOrError_t Grp_CheckIfAtMostOneSingleEnrolmentGrpIsSelected (struct ListCodGrps *LstGrps,
								       bool CheckClosedGroupsIBelong)
  {
   struct ListCodGrps LstClosedGrpsIBelong;
   struct ListGrpsAlreadySelec *AlreadyExistsGroupOfType;
   unsigned NumCodGrp;
   unsigned NumGrpTyp;
   long GrpTypCod;
   Err_SuccessOrError_t SelectionIsValid = Err_SUCCESS;

   /***** Create and initialize list of groups already selected *****/
   Grp_ConstructorListGrpAlreadySelec (&AlreadyExistsGroupOfType);

   /***** Go across the list of closed groups to which i belong
	  checking if a group of the same type is already selected *****/
   /* Step 1 (only if I am a student selecting groups):
      check the closed groups I belong
      because these groups are disabled in form, an so not received */
   if (CheckClosedGroupsIBelong)
     {
      /* Query in the database the closed group codes which I belong to */
      Grp_GetLstCodGrpsUsrBelongs (Gbl.Usrs.Me.UsrDat.UsrCod,-1L,
				   &LstClosedGrpsIBelong,
				   Grp_ONLY_CLOSED_GROUPS);
      for (NumCodGrp = 0;
	   NumCodGrp < LstClosedGrpsIBelong.NumGrps && SelectionIsValid == Err_SUCCESS;
	   NumCodGrp++)
	{
	 GrpTypCod = Grp_DB_GetGrpTypeFromGrp (LstClosedGrpsIBelong.GrpCods[NumCodGrp]);

	 if (Grp_GetSingleMultiple (GrpTypCod) == Grp_SINGLE)
	    for (NumGrpTyp = 0;
		 NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
		 NumGrpTyp++)
	       if (GrpTypCod == AlreadyExistsGroupOfType[NumGrpTyp].GrpTypCod)
		 {
		  if (AlreadyExistsGroupOfType[NumGrpTyp].AlreadySelected)
		     SelectionIsValid = Err_ERROR;
		  else
		     AlreadyExistsGroupOfType[NumGrpTyp].AlreadySelected = true;
		  break;
		 }
	}

      /* Free memory with the list of closed groups which I belongs to */
      Grp_FreeListCodGrp (&LstClosedGrpsIBelong);
     }

   /* Step 2: Check the list of groups selected received from form */
   for (NumCodGrp = 0;
	NumCodGrp < LstGrps->NumGrps && SelectionIsValid == Err_SUCCESS;
	NumCodGrp++)
     {
      GrpTypCod = Grp_DB_GetGrpTypeFromGrp (LstGrps->GrpCods[NumCodGrp]);

      if (Grp_GetSingleMultiple (GrpTypCod) == Grp_SINGLE)
	 for (NumGrpTyp = 0;
	      NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	      NumGrpTyp++)
	    if (GrpTypCod == AlreadyExistsGroupOfType[NumGrpTyp].GrpTypCod)
	      {
	       if (AlreadyExistsGroupOfType[NumGrpTyp].AlreadySelected)
		  SelectionIsValid = Err_ERROR;
	       else
		  AlreadyExistsGroupOfType[NumGrpTyp].AlreadySelected = true;
	       break;
	      }
     }

   /***** Free memory of the list of booleanos that indicates
	  if a group of each type has been selected *****/
   Grp_DestructorListGrpAlreadySelec (&AlreadyExistsGroupOfType);

   return SelectionIsValid;
  }

/*****************************************************************************/
/*********** Create e inicializar list of groups already selected ************/
/*****************************************************************************/

static void Grp_ConstructorListGrpAlreadySelec (struct ListGrpsAlreadySelec **AlreadyExistsGroupOfType)
  {
   unsigned NumGrpTyp;

   /***** Allocate memory for a list of booleans
          that indicates if already a group of each type has been selected *****/
   if ((*AlreadyExistsGroupOfType = calloc (Gbl.Crs.Grps.GrpTypes.NumGrpTypes,
                                            sizeof (**AlreadyExistsGroupOfType))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Initialize the list *****/
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	NumGrpTyp++)
     {
      (*AlreadyExistsGroupOfType)[NumGrpTyp].GrpTypCod = Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod;
      (*AlreadyExistsGroupOfType)[NumGrpTyp].AlreadySelected = false;
     }
  }

/*****************************************************************************/
/***************** Liberar list of groups already selected *******************/
/*****************************************************************************/

static void Grp_DestructorListGrpAlreadySelec (struct ListGrpsAlreadySelec **AlreadyExistsGroupOfType)
  {
   free (*AlreadyExistsGroupOfType);
   *AlreadyExistsGroupOfType = NULL;
  }

/*****************************************************************************/
/********************* Enrol user in the groups of a list ********************/
/*****************************************************************************/

void Grp_EnrolUsrIntoGroups (struct Usr_Data *UsrDat,Rol_Role_t Role,
			     struct ListCodGrps *LstGrps)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_from_the_group_of_type_Y_to_which_it_belonged;
   extern const char *Txt_THE_USER_X_has_been_enroled_in_the_group_of_type_Y_Z;
   struct ListCodGrps LstGrpsBelong;
   unsigned NumGrpTyp;
   unsigned NumGrpSel;
   unsigned NumGrpThisType;
   unsigned NumGrpBelong;
   struct GroupType *GrpTyp;
   struct Group *Grp;
   long GrpCodSel;
   Grp_SingleOrMultiple_t SingleMultiple;
   bool AlreadyEnroledInGrp;

   /***** For each existing type of group in the course... *****/
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	NumGrpTyp++)
     {
      GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];
      SingleMultiple = GrpTyp->Enrolment.SingleMultiple;

      /***** Query in the database the group codes of any group of this type the student belongs to *****/
      LstGrpsBelong.NumGrps = 0;	// Initialized to avoid bug reported by Coverity
      LstGrpsBelong.GrpCods = NULL;	// Initialized to avoid bug reported by Coverity
      Grp_GetLstCodGrpsUsrBelongs (UsrDat->UsrCod,GrpTyp->GrpTypCod,
	                           &LstGrpsBelong,
                                   Grp_CLOSED_AND_OPEN_GROUPS);

      /***** For each group selected by me... *****/
      for (NumGrpSel = 0;
	   NumGrpSel < LstGrps->NumGrps;
	   NumGrpSel++)
        {
	 GrpCodSel = LstGrps->GrpCods[NumGrpSel];

         /* Check if the selected group is of this type */
         for (NumGrpThisType = 0;
              NumGrpThisType < GrpTyp->NumGrps;
              NumGrpThisType++)
           {
            Grp = &GrpTyp->LstGrps[NumGrpThisType];

            if (GrpCodSel == Grp->GrpCod)	// The selected group is of this type
              {
               /* Search if user is already enroled in this group */
               AlreadyEnroledInGrp = false;
               if (SingleMultiple == Grp_SINGLE &&	// If the type of group is of single enrolment ==>
        	   Role == Rol_STD)			// ==> a student can only belong to one group
                 {
		  for (NumGrpBelong = 0;
		       NumGrpBelong < LstGrpsBelong.NumGrps;
		       NumGrpBelong++)
		     if (GrpCodSel == LstGrpsBelong.GrpCods[NumGrpBelong])
			AlreadyEnroledInGrp = true;
		     else
		       {
			/* If the enrolment is single
			   and the group to which the user belongs is different from the selected ==>
			   remove user from the group to which he/she belongs */
			Grp_RemoveUsrFromGroup (UsrDat->UsrCod,LstGrpsBelong.GrpCods[NumGrpBelong]);
			Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_removed_from_the_group_of_type_Y_to_which_it_belonged,
				       UsrDat->FullName,GrpTyp->Name);
		       }
                 }
	       else
		  for (NumGrpBelong = 0;
		       NumGrpBelong < LstGrpsBelong.NumGrps;
		       NumGrpBelong++)
		     if (GrpCodSel == LstGrpsBelong.GrpCods[NumGrpBelong])
		       {
			AlreadyEnroledInGrp = true;
			break;
		       }

               if (!AlreadyEnroledInGrp)	// If the user does not belong to the selected group
                 {
                  Grp_DB_AddUsrToGrp (UsrDat->UsrCod,GrpCodSel);
                  Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_enroled_in_the_group_of_type_Y_Z,
		                 UsrDat->FullName,GrpTyp->Name,Grp->Name);
                 }

               break;	// Once we know the type of a selected group, it's not necessary to check the rest of types
              }
           }
        }

      /***** Free the list of groups of this type to which the user belonged *****/
      Grp_FreeListCodGrp (&LstGrpsBelong);
     }
  }

/*****************************************************************************/
/**************** Remove user of the groups indicados in a list **************/
/*****************************************************************************/
// Returns NumGrpsHeIsRemoved

unsigned Grp_RemoveUsrFromGroups (struct Usr_Data *UsrDat,struct ListCodGrps *LstGrps)
  {
   extern const char *Txt_THE_USER_X_has_not_been_removed_from_any_group;
   extern const char *Txt_THE_USER_X_has_been_removed_from_one_group;
   extern const char *Txt_THE_USER_X_has_been_removed_from_Y_groups;
   struct ListCodGrps LstGrpsBelong;
   unsigned NumGrpSel;
   unsigned NumGrpsUsrIsRemoved = 0;
   long GrpCodSel;

   /***** Query in the database the group codes of any group the user belongs to *****/
   Grp_GetLstCodGrpsUsrBelongs (UsrDat->UsrCod,-1L,
	                        &LstGrpsBelong,
                                Grp_CLOSED_AND_OPEN_GROUPS);

   /***** For each group selected by me... *****/
   for (NumGrpSel = 0;
	NumGrpSel < LstGrps->NumGrps;
	NumGrpSel++)
     {
      GrpCodSel = LstGrps->GrpCods[NumGrpSel];

      /* For each group to which the user belongs... */
      if (Grp_CheckIfGrpIsInList (GrpCodSel,&LstGrpsBelong))
	{
         // If the user belongs to a selected group from which he/she must be removed
	 Grp_RemoveUsrFromGroup (UsrDat->UsrCod,GrpCodSel);
	 NumGrpsUsrIsRemoved++;
	}
     }

   /***** Write message to inform about how many groups the student has been removed from *****/
   if (NumGrpsUsrIsRemoved == 0)
      Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_not_been_removed_from_any_group,
                     UsrDat->FullName);
   else if (NumGrpsUsrIsRemoved == 1)
      Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_removed_from_one_group,
                     UsrDat->FullName);
   else	// NumGrpsHeIsRemoved > 1
      Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_removed_from_Y_groups,
                     UsrDat->FullName,NumGrpsUsrIsRemoved);

   /***** Free the list of groups of this type to which the user belonged *****/
   Grp_FreeListCodGrp (&LstGrpsBelong);

   return NumGrpsUsrIsRemoved;
  }

/*****************************************************************************/
/***************** Remove a user of all groups of a course *******************/
/*****************************************************************************/

void Grp_RemUsrFromAllGrpsInCrs (long UsrCod,long CrsCod)
  {
   /***** Remove user from all groups in the given course *****/
   Grp_DB_RemUsrFromAllGrpsInCrs (UsrCod,CrsCod);

   /***** Flush caches *****/
   Grp_FlushCacheUsrSharesAnyOfMyGrpsInCurrentCrs ();
   if (Usr_ItsMe (UsrCod) == Usr_ME)
      Grp_FlushCacheIBelongToGrp ();
  }

/*****************************************************************************/
/********* Remove a user from all groups of all the user's courses ***********/
/*****************************************************************************/

void Grp_RemUsrFromAllGrps (long UsrCod)
  {
   /***** Remove user from all groups *****/
   Grp_DB_RemUsrFromAllGrps (UsrCod);

   /***** Flush caches *****/
   Grp_FlushCacheUsrSharesAnyOfMyGrpsInCurrentCrs ();
   if (Usr_ItsMe (UsrCod) == Usr_ME)
      Grp_FlushCacheIBelongToGrp ();
  }

/*****************************************************************************/
/************************* Remove a user from a group ************************/
/*****************************************************************************/

static void Grp_RemoveUsrFromGroup (long UsrCod,long GrpCod)
  {
   /***** Remove user from group *****/
   Grp_DB_RemoveUsrFromGrp (UsrCod,GrpCod);

   /***** Flush caches *****/
   Grp_FlushCacheUsrSharesAnyOfMyGrpsInCurrentCrs ();
   if (Usr_ItsMe (UsrCod) == Usr_ME)
      Grp_FlushCacheIBelongToGrp ();
  }

/*****************************************************************************/
/******************** List current group types for edition *******************/
/*****************************************************************************/

static void Grp_ListGroupTypesForEdition (void)
  {
   extern const char *Txt_It_is_optional_to_choose_a_group;
   extern const char *Txt_It_is_mandatory_to_choose_a_group;
   extern const char *Txt_A_student_can_belong_to_several_groups;
   extern const char *Txt_A_student_can_only_belong_to_one_group;
   extern const char *Txt_The_groups_will_automatically_open;
   extern const char *Txt_The_groups_will_not_automatically_open;
   unsigned NumGrpTyp;
   struct GroupType *GrpTyp;
   unsigned CurrentYear = Dat_GetCurrentYear ();
   unsigned UniqueId;
   char Id[32];

   /***** Begin table *****/
   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Write heading *****/
      Grp_WriteHeadingGroupTypes ();

      /***** List group types with forms for edition *****/
      for (NumGrpTyp = 0, UniqueId=1;
	   NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	   NumGrpTyp++, UniqueId++)
	{
	 GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove group type */
	    HTM_TD_Begin ("class=\"BM\"");
	       Ico_PutContextualIconToRemove (ActReqRemGrpTyp,Grp_GROUP_TYPES_SECTION_ID,
					      Grp_PutParGrpTypCod,
					      &GrpTyp->GrpTypCod);
	    HTM_TD_End ();

	    /* Link to get resource link */
	    HTM_TD_Begin ("class=\"BM\"");
	       Ico_PutContextualIconToGetLink (ActReqLnkGrpTyp,NULL,
					       Grp_PutParGrpTypCod,&GrpTyp->GrpTypCod);
	    HTM_TD_End ();

	    /* Name of group type */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginFormAnchor (ActRenGrpTyp,Grp_GROUP_TYPES_SECTION_ID);
		  ParCod_PutPar (ParCod_GrpTyp,GrpTyp->GrpTypCod);
		  HTM_INPUT_TEXT ("GrpTypName",Grp_MAX_CHARS_GROUP_TYPE_NAME,
				  GrpTyp->Name,
				  HTM_SUBMIT_ON_CHANGE,
				  "size=\"12\" class=\"INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Is it mandatory to enrol in any group? */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginFormAnchor (ActChgMdtGrpTyp,Grp_GROUP_TYPES_SECTION_ID);
		  ParCod_PutPar (ParCod_GrpTyp,GrpTyp->GrpTypCod);
		  HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
				    "name=\"OptionalMandatory\""
		                    " class=\"INPUT_%s\" style=\"width:150px;\"",
		                    The_GetSuffix ());
		     HTM_OPTION (HTM_Type_STRING,"N",
				 GrpTyp->Enrolment.OptionalMandatory == Grp_OPTIONAL  ? HTM_SELECTED :
										        HTM_NO_ATTR,
				 "%s",Txt_It_is_optional_to_choose_a_group);
		     HTM_OPTION (HTM_Type_STRING,"Y",
				 GrpTyp->Enrolment.OptionalMandatory == Grp_MANDATORY ? HTM_SELECTED :
											HTM_NO_ATTR,
				 "%s",Txt_It_is_mandatory_to_choose_a_group);
		  HTM_SELECT_End ();
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Is it possible to enrol in multiple groups? */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginFormAnchor (ActChgMulGrpTyp,Grp_GROUP_TYPES_SECTION_ID);
		  ParCod_PutPar (ParCod_GrpTyp,GrpTyp->GrpTypCod);
		  HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
				    "name=\"SingleMultiple\""
				    " class=\"INPUT_%s\" style=\"width:150px;\"",
				    The_GetSuffix ());
		     HTM_OPTION (HTM_Type_STRING,"N",
				 GrpTyp->Enrolment.SingleMultiple == Grp_SINGLE   ? HTM_SELECTED :
										    HTM_NO_ATTR,
				 "%s",Txt_A_student_can_only_belong_to_one_group);
		     HTM_OPTION (HTM_Type_STRING,"Y",
				 GrpTyp->Enrolment.SingleMultiple == Grp_MULTIPLE ? HTM_SELECTED :
										    HTM_NO_ATTR,
				 "%s",Txt_A_student_can_belong_to_several_groups);
		  HTM_SELECT_End ();
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Open time */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginFormAnchor (ActChgTimGrpTyp,Grp_GROUP_TYPES_SECTION_ID);
		  ParCod_PutPar (ParCod_GrpTyp,GrpTyp->GrpTypCod);
		  HTM_TABLE_BeginCenterPadding (2);
		     HTM_TR_Begin (NULL);

			HTM_TD_Begin ("class=\"LM\" style=\"width:16px;\"");
			   switch (GrpTyp->MustBeOpened)
			     {
			      case Grp_MUST_BE_OPENED:
				 Ico_PutIconOn ("clock.svg",Ico_BLACK,
						Txt_The_groups_will_automatically_open);
				 break;
			      case Grp_MUST_NOT_BE_OPENED:
			      default:
				 Ico_PutIconOff ("clock.svg",Ico_BLACK,
						 Txt_The_groups_will_not_automatically_open);
				 break;
			     }
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM\"");
			   snprintf (Id,sizeof (Id),"open_time_%u",UniqueId);
			   Dat_WriteFormClientLocalDateTimeFromTimeUTC (Id,
									Dat_STR_TIME,
									GrpTyp->OpenTimeUTC,
									CurrentYear,
									CurrentYear + 1,
									Dat_FORM_SECONDS_ON,
									Dat_HMS_DO_NOT_SET,	// Don't set hour, minute and second
									HTM_SUBMIT_ON_CHANGE);
			HTM_TD_End ();

		     HTM_TR_End ();
		  HTM_TABLE_End ();
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Number of groups of this type */
	    HTM_TD_Begin ("class=\"CM DAT_%s\"",The_GetSuffix ());
	       HTM_Unsigned (GrpTyp->NumGrps);
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************ Put contextual icons in edition of types of group **************/
/*****************************************************************************/

static void Grp_PutIconsEditingGroupTypes (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view groups *****/
   Grp_PutIconToViewGroups ();
  }

static void Grp_PutIconToViewGroups (void)
  {
   Ico_PutContextualIconToView (ActReqSelAllGrp,NULL,NULL,NULL);
  }

/*****************************************************************************/
/*********************** Write heading of group types ************************/
/*****************************************************************************/

static void Grp_WriteHeadingGroupTypes (void)
  {
   extern const char *Txt_Type_of_group;
   extern const char *Txt_eg_Lectures_Practicals;
   extern const char *Txt_Mandatory_enrolment;
   extern const char *Txt_Multiple_enrolment;
   extern const char *Txt_Opening_of_groups;
   extern const char *Txt_Number_of_BR_groups;

   HTM_TR_Begin (NULL);

      HTM_TH_Span (NULL              ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH_Span (NULL              ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH_Begin (HTM_HEAD_CENTER);
	 HTM_Txt (Txt_Type_of_group);
	 HTM_BR ();
	 HTM_ParTxtPar (Txt_eg_Lectures_Practicals);
      HTM_TH_End ();
      HTM_TH (Txt_Mandatory_enrolment,HTM_HEAD_CENTER);
      HTM_TH (Txt_Multiple_enrolment ,HTM_HEAD_CENTER);
      HTM_TH (Txt_Opening_of_groups  ,HTM_HEAD_CENTER);
      HTM_TH (Txt_Number_of_BR_groups,HTM_HEAD_CENTER);

   HTM_TR_End ();
  }

/*****************************************************************************/
/********************** List current groups for edition **********************/
/*****************************************************************************/

static void Grp_ListGroupsForEdition (const struct Roo_Rooms *Rooms)
  {
   extern const char *Txt_No_assigned_room;
   extern const char *Txt_Another_room;

   static struct
     {
      Act_Action_t NextAction;
      const char *Icon;
      Ico_Color_t Color;
     } ClosedOpen[CloOpe_NUM_CLOSED_OPEN] =
     {
      [CloOpe_CLOSED] = {ActOpeGrp,"lock.svg"  ,Ico_RED  },	// Group is now closed
      [CloOpe_OPEN  ] = {ActCloGrp,"unlock.svg",Ico_GREEN},	// Group is now open
     },
       FileZones[Grp_NUM_FILEZONES] =
     {
      [Grp_HAS_NOT_FILEZONES] = {ActEnaFilZonGrp,"folder.svg"     ,Ico_RED  },	// Group has not filezones
      [Grp_HAS_FILEZONES    ] = {ActDisFilZonGrp,"folder-open.svg",Ico_GREEN},	// Group has filezones
     };
   unsigned NumGrpTyp1;
   const struct GroupType *GrpTyp1;
   unsigned NumGrpTyp2;
   const struct GroupType *GrpTyp2;
   unsigned NumGrpThisType;
   struct Group *Grp;
   unsigned NumRoo;
   const struct Roo_Room *Roo;
   Rol_Role_t Role;
   char StrMaxStudents[Cns_MAX_DIGITS_UINT + 1];

   /***** Begin table *****/
   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Write heading *****/
      Grp_WriteHeadingGroups ();

      /***** List the groups *****/
      for (NumGrpTyp1 = 0;
	   NumGrpTyp1 < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	   NumGrpTyp1++)
	{
	 GrpTyp1 = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp1];
	 for (NumGrpThisType = 0;
	      NumGrpThisType < GrpTyp1->NumGrps;
	      NumGrpThisType++)
	   {
	    Grp = &GrpTyp1->LstGrps[NumGrpThisType];

	    HTM_TR_Begin (NULL);

	       /***** Icon to remove group *****/
	       HTM_TD_Begin ("class=\"BM\"");
		  Ico_PutContextualIconToRemove (ActReqRemGrp,Grp_GROUPS_SECTION_ID,
						 Grp_PutParGrpCod,&Grp->GrpCod);
	       HTM_TD_End ();

	       /***** Icon to open/close group *****/
	       HTM_TD_Begin ("class=\"BM\"");
		  Frm_BeginFormAnchor (ClosedOpen[Grp->Open].NextAction,
				       Grp_GROUPS_SECTION_ID);
		     ParCod_PutPar (ParCod_Grp,Grp->GrpCod);
		     Ico_PutIconLink (ClosedOpen[Grp->Open].Icon,
			              ClosedOpen[Grp->Open].Color,
			              ClosedOpen[Grp->Open].NextAction);
		  Frm_EndForm ();
	       HTM_TD_End ();

	       /***** Icon to activate file zones for this group *****/
	       HTM_TD_Begin ("class=\"BM\"");
		  Frm_BeginFormAnchor (FileZones[Grp->HasFileZones].NextAction,
				       Grp_GROUPS_SECTION_ID);
		     ParCod_PutPar (ParCod_Grp,Grp->GrpCod);
		     Ico_PutIconLink (FileZones[Grp->HasFileZones].Icon,
			              FileZones[Grp->HasFileZones].Color,
			              FileZones[Grp->HasFileZones].NextAction);
		  Frm_EndForm ();
	       HTM_TD_End ();

	       /***** Group type *****/
	       /* Begin selector */
	       HTM_TD_Begin ("class=\"CM\"");
		  Frm_BeginFormAnchor (ActChgGrpTyp,Grp_GROUPS_SECTION_ID);
		     ParCod_PutPar (ParCod_Grp,Grp->GrpCod);
		     HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
				       "name=\"GrpTypCod\""
				       " class=\"INPUT_%s\" style=\"width:100px;\"",
				       The_GetSuffix ());

			/* Options for group types */
			for (NumGrpTyp2 = 0;
			     NumGrpTyp2 < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
			     NumGrpTyp2++)
			  {
			   GrpTyp2 = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp2];
			   HTM_OPTION (HTM_Type_LONG,&GrpTyp2->GrpTypCod,
				       GrpTyp2->GrpTypCod == GrpTyp1->GrpTypCod ? HTM_SELECTED :
										  HTM_NO_ATTR,
				       "%s",GrpTyp2->Name);
			  }

		     /* End selector */
		     HTM_SELECT_End ();
		  Frm_EndForm ();
	       HTM_TD_End ();

	       /***** Group name *****/
	       HTM_TD_Begin ("class=\"CM\"");
		  Frm_BeginFormAnchor (ActRenGrp,Grp_GROUPS_SECTION_ID);
		     ParCod_PutPar (ParCod_Grp,Grp->GrpCod);
		     HTM_INPUT_TEXT ("GrpName",Grp_MAX_CHARS_GROUP_NAME,Grp->Name,
				     HTM_SUBMIT_ON_CHANGE,
				     "size=\"20\" class=\"INPUT_%s\"",
				     The_GetSuffix ());
		  Frm_EndForm ();
	       HTM_TD_End ();

	       /***** Room *****/
	       /* Begin selector */
	       HTM_TD_Begin ("class=\"CM\"");
		  Frm_BeginFormAnchor (ActChgGrpRoo,Grp_GROUPS_SECTION_ID);
		     ParCod_PutPar (ParCod_Grp,Grp->GrpCod);
		     HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
				       "name=\"RooCod\""
				       " class=\"INPUT_%s\" style=\"width:100px;\"",
				       The_GetSuffix ());

			/* Option for no assigned room */
			HTM_OPTION (HTM_Type_STRING,"-1",
				    Grp->Room.RooCod < 0 ? HTM_SELECTED :
							   HTM_NO_ATTR,
				    "%s",Txt_No_assigned_room);

			/* Option for another room */
			HTM_OPTION (HTM_Type_STRING,"0",
				    Grp->Room.RooCod == 0 ? HTM_SELECTED :
							    HTM_NO_ATTR,
				    "%s",Txt_Another_room);

			/* Options for rooms */
			for (NumRoo = 0;
			     NumRoo < Rooms->Num;
			     NumRoo++)
			  {
			   Roo = &Rooms->Lst[NumRoo];
			   HTM_OPTION (HTM_Type_LONG,&Roo->RooCod,
				       Roo->RooCod == Grp->Room.RooCod ? HTM_SELECTED :
									 HTM_NO_ATTR,
				       "%s",Roo->ShrtName);
			  }

		     /* End selector */
		     HTM_SELECT_End ();
		  Frm_EndForm ();
	       HTM_TD_End ();

	       /***** Current number of users in this group *****/
	       for (Role = Rol_TCH;
		    Role >= Rol_STD;
		    Role--)
		 {
		  HTM_TD_Begin ("class=\"CM DAT_%s\"",The_GetSuffix ());
		     HTM_Int (Grp->NumUsrs[Role]);
		  HTM_TD_End ();
		 }

	       /***** Maximum number of students of the group (row[3]) *****/
	       HTM_TD_Begin ("class=\"CM\"");
		  Frm_BeginFormAnchor (ActChgMaxStdGrp,Grp_GROUPS_SECTION_ID);
		     ParCod_PutPar (ParCod_Grp,Grp->GrpCod);
		     Grp_WriteMaxStds (StrMaxStudents,Grp->MaxStds);
		     HTM_INPUT_TEXT ("MaxStudents",Cns_MAX_DIGITS_UINT,StrMaxStudents,
				     HTM_SUBMIT_ON_CHANGE,
				     "size=\"3\" class=\"INPUT_%s\"",
				     The_GetSuffix ());
		  Frm_EndForm ();
	       HTM_TD_End ();

	    HTM_TR_End ();
	   }
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************************** Write heading of groups **************************/
/*****************************************************************************/

static void Grp_WriteHeadingGroups (void)
  {
   extern const char *Txt_Type_BR_of_group;
   extern const char *Txt_Group_name;
   extern const char *Txt_eg_A_B;
   extern const char *Txt_Room;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Max_BR_students;
   Rol_Role_t Role;

   HTM_TR_Begin (NULL);

      HTM_TH_Span (NULL           ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH_Span (NULL           ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH_Span (NULL           ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH (Txt_Type_BR_of_group,HTM_HEAD_CENTER);
      HTM_TH_Begin (HTM_HEAD_CENTER);
	 HTM_Txt (Txt_Group_name);
	 HTM_BR ();
	 HTM_ParTxtPar (Txt_eg_A_B);
      HTM_TH_End ();
      HTM_TH (Txt_Room,HTM_HEAD_CENTER);
      for (Role  = Rol_TCH;
	   Role >= Rol_STD;
	   Role--)
         HTM_TH (Txt_ROLES_PLURAL_BRIEF_Abc[Role],HTM_HEAD_CENTER);
      HTM_TH (Txt_Max_BR_students ,HTM_HEAD_CENTER);

   HTM_TR_End ();
  }

/*********************** Write text "the whole course" ***********************/

void Grp_WriteTheWholeCourse (void)
  {
   extern const char *Txt_The_whole_course;

   HTM_Txt (Txt_The_whole_course);
   HTM_SP ();
   HTM_Txt (Gbl.Hierarchy.Node[Hie_CRS].ShrtName);
  }

/*****************************************************************************/
/***** List groups of a type to edit                                     *****/
/***** assignments, attendance events, exam sessions, matches or surveys *****/
/*****************************************************************************/

void Grp_ListGrpsToEditAsgAttSvyEvtMch (Grp_WhichIsAssociatedToGrp_t WhichIsAssociatedToGrp,
                                        long Cod)	// Assignment, attendance event, survey, exam event or match
  {
   unsigned NumGrpTyp;
   struct GroupType *GrpTyp;

   /***** List the groups for each group type *****/
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	NumGrpTyp++)
     {
      GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

      if (GrpTyp->NumGrps)
	 Grp_ListGrpsOfATypeToEditAsgAttSvyEvtMch (WhichIsAssociatedToGrp,
						   Cod,
						   GrpTyp);
     }
  }

/*****************************************************************************/
/***** List groups of a type to edit                                     *****/
/***** assignments, attendance events, exam sessions, matches or surveys *****/
/*****************************************************************************/

static void Grp_ListGrpsOfATypeToEditAsgAttSvyEvtMch (Grp_WhichIsAssociatedToGrp_t WhichIsAssociatedToGrp,
						      long Cod,	// Assignment, attendance event, survey, exam event or match
						      struct GroupType *GrpTyp)
  {
   static struct
     {
      const char *Table;
      const char *Field;
     } AssociationsToGrps[Grp_NUM_ASSOCIATIONS_TO_GROUPS] =
     {
      [Grp_ASSIGNMENT] = {"asg_groups","AsgCod"},
      [Grp_ATT_EVENT ] = {"att_groups","AttCod"},
      [Grp_SURVEY    ] = {"svy_groups","SvyCod"},
      [Grp_EXA_EVENT ] = {"exa_groups","SesCod"},
      [Grp_MATCH     ] = {"mch_groups","MchCod"},
     };
   struct ListCodGrps LstGrpsIBelong;
   unsigned NumGrpThisType;
   Usr_Belong_t IBelongToThisGroup;
   struct Group *Grp;

   /***** Write heading *****/
   HTM_FIELDSET_Begin (NULL);
      HTM_LEGEND (GrpTyp->Name);

      /***** Begin table *****/
      HTM_TABLE_Begin (NULL);

	 /***** Write heading *****/
	 Grp_WriteGrpHead ();

	 /***** Query from the database the groups of this type which I belong to *****/
	 Grp_GetLstCodGrpsUsrBelongs (Gbl.Usrs.Me.UsrDat.UsrCod,GrpTyp->GrpTypCod,
				      &LstGrpsIBelong,
				      Grp_CLOSED_AND_OPEN_GROUPS);

	 /***** List the groups *****/
	 for (NumGrpThisType = 0;
	      NumGrpThisType < GrpTyp->NumGrps;
	      NumGrpThisType++)
	   {
	    Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
	    IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsIBelong) ? Usr_BELONG :
											Usr_DONT_BELONG;

	    /* Put checkbox to select the group */
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin (IBelongToThisGroup == Usr_BELONG ? "class=\"LM BG_HIGHLIGHT\"" :
								"class=\"LM\"");
		  HTM_INPUT_CHECKBOX ("GrpCods",
				      (Grp_DB_CheckIfAssociatedToGrp (AssociationsToGrps[WhichIsAssociatedToGrp].Table,
								      AssociationsToGrps[WhichIsAssociatedToGrp].Field,
								      Cod,Grp->GrpCod) ? HTM_CHECKED :
											 HTM_NO_ATTR) |
				      (IBelongToThisGroup == Usr_BELONG ||
				       Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ? HTM_NO_ATTR :
										HTM_DISABLED),
				      "id=\"Grp%ld\" value=\"%ld\""
				      " onclick=\"uncheckParent(this,'WholeCrs')\"",
				      Grp->GrpCod,Grp->GrpCod);
	       HTM_TD_End ();

	       Grp_WriteRowGrp (Grp,IBelongToThisGroup == Usr_BELONG ? Lay_HIGHLIGHT :
								       Lay_DONT_HIGHLIGHT);

	    HTM_TR_End ();
	   }

	 /***** Free memory with the list of groups which I belongs to *****/
	 Grp_FreeListCodGrp (&LstGrpsIBelong);

      HTM_TABLE_End ();

   HTM_FIELDSET_End ();
  }

/*****************************************************************************/
/****************** Show list of groups to enrol/remove me *******************/
/*****************************************************************************/

void Grp_ShowLstGrpsToChgMyGrps (void)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *Txt_My_groups;
   extern const char *Txt_No_groups_have_been_created_in_the_course_X;
   unsigned NumGrpTyp;
   struct GroupType *GrpTyp;
   Frm_PutForm_t PutFormToChangeGrps = Frm_CheckIfInside () == Frm_INSIDE_FORM ? Frm_DONT_PUT_FORM :	// Inside another form (record card)?
										 Frm_PUT_FORM;
   Usr_Can_t ICanChangeMyGrps = Usr_CAN_NOT;
   long SelectedGrpTypCod = ParCod_GetPar (ParCod_GrpTyp);

   if (Gbl.Crs.Grps.NumGrps) // This course has groups
     {
      /***** Get list of groups types and groups in this course *****/
      Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_GRP_TYPES_WITH_GROUPS);

      /***** Show warnings to students *****/
      // Students are required to join groups with mandatory enrolment; teachers don't
      if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
	 Grp_ShowWarningToStdsToChangeGrps ();
     }

   /***** Begin box *****/
   Box_BoxBegin (Txt_My_groups,
		 Grp_PutIconsMyGroups,NULL,
		 Hlp_USERS_Groups,Box_NOT_CLOSABLE);

      if (Gbl.Crs.Grps.NumGrps) // This course has groups
	{
	 /***** Begin form *****/
	 if (PutFormToChangeGrps)
	    Frm_BeginForm (ActChgGrp);

	 /***** List the groups the user belongs to for change *****/
	 for (NumGrpTyp = 0;
	      NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	      NumGrpTyp++)
	   {
	    GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

	    if (GrpTyp->NumGrps)	 // If there are groups of this type
	      {
	       if (Grp_ListGrpsForChangeMySelection (GrpTyp,SelectedGrpTypCod) == Usr_CAN)
		  ICanChangeMyGrps = Usr_CAN;
	      }
	   }

	 /***** End form *****/
	 if (PutFormToChangeGrps)
	   {
	       if (ICanChangeMyGrps == Usr_CAN)
		  Btn_PutButton (Btn_SAVE_CHANGES,NULL);
	    Frm_EndForm ();
	   }
	}
      else	// This course has no groups
	 Ale_ShowAlert (Ale_INFO,Txt_No_groups_have_been_created_in_the_course_X,
			Gbl.Hierarchy.Node[Hie_CRS].FullName);

   /***** End box *****/
   Box_BoxEnd ();

   if (Gbl.Crs.Grps.NumGrps) // This course has groups
      /***** Free list of groups types and groups in this course *****/
      Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/*************************** Put icon to edit groups *************************/
/*****************************************************************************/

static void Grp_PutIconsMyGroups (__attribute__((unused)) void *Args)
  {
   Usr_Can_t ICanEdit = (Frm_CheckIfInside () == Frm_OUTSIDE_FORM &&
			 (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
			  Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)) ? Usr_CAN :
								     Usr_CAN_NOT;

   /***** Icon to edit groups *****/
   if (ICanEdit == Usr_CAN)
      Ico_PutContextualIconToEdit (ActReqEdiGrp,NULL,NULL,NULL);

   /***** Link to get resource link *****/
   switch (Gbl.Action.Act)
     {
      case ActReqSelAllGrp:
      case ActReqSelOneGrpTyp:
      case ActChgGrp:
      case ActReqLnkAllGrp:
         Ico_PutContextualIconToGetLink (ActReqLnkAllGrp,NULL,NULL,NULL);
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/*********** Show warnings to students before form to change groups **********/
/*****************************************************************************/

static void Grp_ShowWarningToStdsToChangeGrps (void)
  {
   extern const char *Txt_You_have_to_enrol_compulsorily_at_least_in_one_group_of_type_X;
   extern const char *Txt_You_have_to_enrol_compulsorily_in_one_group_of_type_X;
   extern const char *Txt_You_can_enrol_voluntarily_in_one_or_more_groups_of_type_X;
   extern const char *Txt_You_can_enrol_voluntarily_in_one_group_of_type_X;
   unsigned NumGrpTyp;
   struct GroupType *GrpTyp;
   static Ale_AlertType_t AlertType[Grp_NUM_OPTIONAL_MANDATORY] =
     {
      [Grp_OPTIONAL ] = Ale_INFO,	// Optional enrolment
      [Grp_MANDATORY] = Ale_WARNING,	// Mandatory enrolment
     };
   static const char **Format[Grp_NUM_OPTIONAL_MANDATORY][Grp_NUM_SINGLE_MULTIPLE] =
     {
      [Grp_OPTIONAL ][Grp_SINGLE  ] = &Txt_You_can_enrol_voluntarily_in_one_group_of_type_X,			// Optional, single enrolment
      [Grp_OPTIONAL ][Grp_MULTIPLE] = &Txt_You_can_enrol_voluntarily_in_one_or_more_groups_of_type_X,	// Optional, multiple enrolment
      [Grp_MANDATORY][Grp_SINGLE  ] = &Txt_You_have_to_enrol_compulsorily_in_one_group_of_type_X,		// Mandatory, single enrolment
      [Grp_MANDATORY][Grp_MULTIPLE] = &Txt_You_have_to_enrol_compulsorily_at_least_in_one_group_of_type_X,	// Mandatory, multiple enrolment
     };

   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	NumGrpTyp++)
     {
      GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

      if (GrpTyp->NumGrps)	 // If there are groups of this type...
	 // If I don't belong to any group
	 if (Grp_DB_CheckIfIBelongToGrpsOfType (GrpTyp->GrpTypCod) == Usr_DONT_BELONG)	// Fast check (not necessary, but avoid slow check)
	    // If there is any group of this type available
	    if (Grp_DB_CheckIfAvailableGrpTyp (GrpTyp->GrpTypCod))	// Slow check
	       Ale_ShowAlert (AlertType[GrpTyp->Enrolment.OptionalMandatory],
			      *Format[GrpTyp->Enrolment.OptionalMandatory][GrpTyp->Enrolment.SingleMultiple],
			      GrpTyp->Name);
     }
  }

/*****************************************************************************/
/***************** List the groups of a type to enrol in *********************/
/*****************************************************************************/
// Returns true if I can change my selection

static Usr_Can_t Grp_ListGrpsForChangeMySelection (const struct GroupType *GrpTyp,
						   long SelectedGrpTypCod)
  {
   struct ListCodGrps LstGrpsIBelong;
   unsigned NumGrpThisType;
   struct Group *Grp;
   Usr_Belong_t IBelongToThisGroup;
   Usr_Belong_t IBelongToAClosedGroup;
   Usr_Can_t ICanChangeMySelectionForThisGrpTyp;
   Usr_Can_t ICanChangeMySelectionForThisGrp;
   HTM_Attributes_t Attributes;
   char StrGrpCod[32];
   Lay_Highlight_t Highlight;
   char *Anchor = NULL;

   if (SelectedGrpTypCod > 0)	// One group type will be highlighted
     {
      /***** Build anchor string *****/
      Frm_SetAnchorStr (GrpTyp->GrpTypCod,&Anchor);

      /***** Begin article *****/
      HTM_ARTICLE_Begin (Anchor);

      /***** Highlight this group type? *****/
      Highlight = GrpTyp->GrpTypCod == SelectedGrpTypCod ? Lay_HIGHLIGHT :
							   Lay_DONT_HIGHLIGHT;
     }
   else
      Highlight = Lay_DONT_HIGHLIGHT;

   /***** Begin fieldset *****/
   switch (Highlight)
     {
      case Lay_HIGHLIGHT:
	 HTM_FIELDSET_Begin ("class=\"HIGHLIGHT_%s\"",The_GetSuffix ());
	 break;
      case Lay_DONT_HIGHLIGHT:
      default:
	 HTM_FIELDSET_Begin (NULL);
	 break;
     }
   HTM_LEGEND (GrpTyp->Name);

      /***** Alert with groups opening date *****/
      if (GrpTyp->MustBeOpened == Grp_MUST_BE_OPENED)
	 Grp_WriteGrpTypOpening (GrpTyp);

      /***** Begin table *****/
      HTM_TABLE_BeginWidePadding (2);

	 /***** Write heading *****/
	 Grp_WriteGrpHead ();

	 /***** Query in the database the group of this type that I belong to *****/
	 Grp_GetLstCodGrpsUsrBelongs (Gbl.Usrs.Me.UsrDat.UsrCod,GrpTyp->GrpTypCod,
				      &LstGrpsIBelong,
				      Grp_CLOSED_AND_OPEN_GROUPS);

	 /***** Check if I can change my selection *****/
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_STD:
	       switch (GrpTyp->Enrolment.SingleMultiple)
		 {
		  case Grp_MULTIPLE:
		     for (NumGrpThisType = 0, ICanChangeMySelectionForThisGrpTyp = Usr_CAN_NOT;
			  NumGrpThisType < GrpTyp->NumGrps &&
			  ICanChangeMySelectionForThisGrpTyp == Usr_CAN_NOT;
			  NumGrpThisType++)
		       {
			Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
			if (Grp->Open == CloOpe_OPEN)	// If group is open
			  {
			   IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,
									&LstGrpsIBelong) ? Usr_BELONG :
											   Usr_DONT_BELONG;
			   switch (IBelongToThisGroup)
			     {
			      case Usr_BELONG:
				 ICanChangeMySelectionForThisGrpTyp = Usr_CAN;		// I can leave group
				 break;
			      case Usr_DONT_BELONG:
			      default:
				 if (Grp->NumUsrs[Rol_STD] < Grp->MaxStds)		// Group is not full
				    ICanChangeMySelectionForThisGrpTyp = Usr_CAN;	// I can enrol into group
				 break;
			     }
			  }
		       }
		     break;
		  case Grp_SINGLE:
		  default:
		     /* Check if I belong to a closed group */
		     for (NumGrpThisType = 0, IBelongToAClosedGroup = Usr_DONT_BELONG;
			  NumGrpThisType < GrpTyp->NumGrps && IBelongToAClosedGroup == Usr_DONT_BELONG;
			  NumGrpThisType++)
		       {
			Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
			if (Grp->Open == CloOpe_CLOSED)	// If group is closed
			  {
			   IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,
									&LstGrpsIBelong) ? Usr_BELONG :
											   Usr_DONT_BELONG;
			   if (IBelongToThisGroup == Usr_BELONG)
			      IBelongToAClosedGroup = Usr_BELONG;	// I belong to a closed group
			  }
		       }

		     switch (IBelongToAClosedGroup)
		       {
			case Usr_BELONG:
			   ICanChangeMySelectionForThisGrpTyp = Usr_CAN_NOT;	// I can not leave
			   break;
			case Usr_DONT_BELONG:
			default:
			   switch (GrpTyp->Enrolment.OptionalMandatory)
			     {
			      case Grp_MANDATORY:	// Check if I can enrol in at least one group to which I don't belong
				 for (NumGrpThisType = 0, ICanChangeMySelectionForThisGrpTyp = Usr_CAN_NOT;
				      NumGrpThisType < GrpTyp->NumGrps &&
				      ICanChangeMySelectionForThisGrpTyp == Usr_CAN_NOT;
				      NumGrpThisType++)
				   {
				    Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
				    if (Grp->Open == CloOpe_OPEN &&		// If group is open...
					Grp->NumUsrs[Rol_STD] < Grp->MaxStds)	// ...and not full
				      {
				       IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,
										    &LstGrpsIBelong) ? Usr_BELONG :
												       Usr_DONT_BELONG;
				       if (IBelongToThisGroup == Usr_DONT_BELONG)
					  ICanChangeMySelectionForThisGrpTyp = Usr_CAN;// I can enrol into this group
				      }
				   }
				 break;
			      case Grp_OPTIONAL:	// If enrolment is optional, I can leave
			      default:
				 ICanChangeMySelectionForThisGrpTyp = Usr_CAN;	// I can leave group
				 break;
			     }
			   break;
		       }
		     break;
		 }
	       break;
	    case Rol_TCH:
	    case Rol_SYS_ADM:
	       ICanChangeMySelectionForThisGrpTyp = Usr_CAN;			// I can not enrol/leave
	       break;
	    default:
	       ICanChangeMySelectionForThisGrpTyp = Usr_CAN_NOT;		// I can not enrol/leave
	       break;
	   }

	 /***** List the groups *****/
	 for (NumGrpThisType = 0;
	      NumGrpThisType < GrpTyp->NumGrps;
	      NumGrpThisType++)
	   {
	    Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
	    IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,
							 &LstGrpsIBelong) ? Usr_BELONG :
									    Usr_DONT_BELONG;

	    /* Selection disabled? */
	    switch (ICanChangeMySelectionForThisGrpTyp)	// I can change my selection for this group type
	      {
	       case Usr_CAN:
		  ICanChangeMySelectionForThisGrp = Usr_CAN;
		  if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
		     switch (Grp->Open)
		       {
			case CloOpe_OPEN:		// If group is open
			   if (IBelongToThisGroup == Usr_DONT_BELONG &&
			       Grp->NumUsrs[Rol_STD] >= Grp->MaxStds)	// Group is full
			      ICanChangeMySelectionForThisGrp = Usr_CAN_NOT;
			   break;
			case CloOpe_CLOSED:		// If group is closed
			default:
			   ICanChangeMySelectionForThisGrp = Usr_CAN_NOT;
			   break;
		       }
		  break;
	       case Usr_CAN_NOT:	// I can not change my selection for this group type
	       default:
		  ICanChangeMySelectionForThisGrp = Usr_CAN_NOT;
		  break;
	      }

	    /* Put radio item or checkbox to select the group */
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin (IBelongToThisGroup == Usr_BELONG ? "class=\"LM BG_HIGHLIGHT\"" :
								"class=\"LM\"");
		  Attributes = (IBelongToThisGroup == Usr_BELONG ? HTM_CHECKED :
								   HTM_NO_ATTR) |
			       (ICanChangeMySelectionForThisGrp == Usr_CAN ? HTM_NO_ATTR :
									     HTM_DISABLED);
		  snprintf (StrGrpCod,sizeof (StrGrpCod),"GrpCod%ld",GrpTyp->GrpTypCod);
		  if (Gbl.Usrs.Me.Role.Logged == Rol_STD &&			// If I am a student
		      GrpTyp->Enrolment.SingleMultiple == Grp_SINGLE &&	// ...and the enrolment is single
		      GrpTyp->NumGrps > 1)					// ...and there are more than one group
		    {
		     /* Put a radio item */
		     switch (GrpTyp->Enrolment.OptionalMandatory)
		       {
			case Grp_MANDATORY:
			   HTM_INPUT_RADIO (StrGrpCod,
					    Attributes,
					    "id=\"Grp%ld\" value=\"%ld\"",
					    Grp->GrpCod,Grp->GrpCod);
			   break;
			case Grp_OPTIONAL:	// If the enrolment is optional, I can select no groups
			default:
			   HTM_INPUT_RADIO (StrGrpCod,
					    Attributes,
					    "id=\"Grp%ld\" value=\"%ld\""
					    " onclick=\"selectUnselectRadio(this,%s,this.form.GrpCod%ld,%u)\"",
					    Grp->GrpCod,Grp->GrpCod,
					    IBelongToThisGroup == Usr_BELONG ? "true" :	// initially checked
									       "false",	// initially unchecked
					    GrpTyp->GrpTypCod,GrpTyp->NumGrps);
			   break;
		       }
		    }
		  else
		     /* Put a checkbox item */
		     HTM_INPUT_CHECKBOX (StrGrpCod,
					 Attributes,
					 "id=\"Grp%ld\" value=\"%ld\"",
					 Grp->GrpCod,Grp->GrpCod);

	       HTM_TD_End ();

	       Grp_WriteRowGrp (Grp,IBelongToThisGroup == Usr_BELONG ? Lay_HIGHLIGHT :
								       Lay_DONT_HIGHLIGHT);

	    HTM_TR_End ();
	   }

	 /***** Free memory with the list of groups a the that belongs the user *****/
	 Grp_FreeListCodGrp (&LstGrpsIBelong);

      /***** End table *****/
      HTM_TABLE_End ();

   /***** End fieldset *****/
   HTM_FIELDSET_End ();

   if (SelectedGrpTypCod > 0)	// One group type will be highlighted
     {
      /***** End article *****/
      HTM_ARTICLE_End ();

      /***** Free anchor string *****/
      Frm_FreeAnchorStr (&Anchor);
     }

   return ICanChangeMySelectionForThisGrpTyp;
  }

/*****************************************************************************/
/**************** Show list of groups to enrol/remove users ******************/
/*****************************************************************************/
// If UsrCod  > 0 ==> mark her/his groups as checked
// If UsrCod <= 0 ==> do not mark any group as checked

void Grp_ShowLstGrpsToChgOtherUsrsGrps (long UsrCod)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *Txt_Groups;
   unsigned NumGrpTyp;
   struct GroupType *GrpTyp;

   /***** Get list of groups types and groups in current course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_GRP_TYPES_WITH_GROUPS);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Groups,NULL,NULL,
                 Hlp_USERS_Groups,Box_NOT_CLOSABLE);

      /***** List to select the groups the user belongs to *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	   NumGrpTyp++)
	{
	 GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

	 if (GrpTyp->NumGrps)
	    Grp_ListGrpsToAddOrRemUsrs (GrpTyp,UsrCod);
	}

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of groups types and groups in current course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/*************** List groups of a type to add or remove users ****************/
/*****************************************************************************/
// If UsrCod  > 0 ==> mark her/his groups as checked
// If UsrCod <= 0 ==> do not mark any group as checked

static void Grp_ListGrpsToAddOrRemUsrs (const struct GroupType *GrpTyp,long UsrCod)
  {
   struct ListCodGrps LstGrpsUsrBelongs;
   unsigned NumGrpThisType;
   Usr_Belong_t UsrBelongsToThisGroup;
   struct Group *Grp;
   char StrGrpCod[32];

   /***** Write heading *****/
   HTM_FIELDSET_Begin (NULL);
      HTM_LEGEND (GrpTyp->Name);

      /***** Begin table *****/
      HTM_TABLE_BeginWidePadding (2);

	 /***** Write heading *****/
	 Grp_WriteGrpHead ();

	 /***** Query the groups of this type which the user belongs to *****/
	 if (UsrCod > 0)
	    Grp_GetLstCodGrpsUsrBelongs (Gbl.Usrs.Other.UsrDat.UsrCod,GrpTyp->GrpTypCod,
					 &LstGrpsUsrBelongs,
					 Grp_CLOSED_AND_OPEN_GROUPS);

	 /***** List the groups *****/
	 for (NumGrpThisType = 0;
	      NumGrpThisType < GrpTyp->NumGrps;
	      NumGrpThisType++)
	   {
	    Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
	    UsrBelongsToThisGroup = UsrCod > 0 ? (Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsUsrBelongs) ? Usr_BELONG :
													    Usr_DONT_BELONG) :
						 Usr_DONT_BELONG;

	    /* Begin row */
	    HTM_TR_Begin (NULL);

	       /* Begin cell for checkbox */
	       HTM_TD_Begin (UsrBelongsToThisGroup == Usr_BELONG ? "class=\"LM BG_HIGHLIGHT\"" :
								   "class=\"LM\"");

		  /* Put checkbox to select the group */
		  // Always checkbox, not radio, because the role in the form may be teacher,
		  // so if he/she is enroled as teacher, he/she can belong to several groups
		  snprintf (StrGrpCod,sizeof (StrGrpCod),"GrpCod%ld",GrpTyp->GrpTypCod);
		  HTM_INPUT_CHECKBOX (StrGrpCod,
				      UsrBelongsToThisGroup == Usr_BELONG ? HTM_CHECKED :
									    HTM_NO_ATTR,
				      "id=\"Grp%ld\" value=\"%ld\"",
				      Grp->GrpCod,Grp->GrpCod);

	       /* End cell for checkbox */
	       HTM_TD_End ();

	       /* Write cell for group */
	       Grp_WriteRowGrp (Grp,UsrBelongsToThisGroup == Usr_BELONG ? Lay_HIGHLIGHT :
									  Lay_DONT_HIGHLIGHT);

	    /* End row */
	    HTM_TR_End ();
	   }

	 /***** Free memory with the lists of groups *****/
	 if (UsrCod > 0)
	    Grp_FreeListCodGrp (&LstGrpsUsrBelongs);

      /***** End table *****/
      HTM_TABLE_End ();

   HTM_FIELDSET_End ();
  }

/*****************************************************************************/
/******* Write a list of groups as checkbox form for unique selection ********/
/*****************************************************************************/

static void Grp_ListGrpsForMultipleSelection (const struct GroupType *GrpTyp)
  {
   extern const char *Txt_users_with_no_group;
   unsigned NumGrpThisType;
   struct ListCodGrps LstGrpsIBelong;
   Usr_Belong_t IBelongToThisGroup;
   Usr_Can_t ICanSelUnselGroup;
   struct Group *Grp;
   Rol_Role_t Role;

   HTM_FIELDSET_Begin (NULL);
      HTM_LEGEND (GrpTyp->Name);

      /***** Begin table *****/
      HTM_TABLE_Begin ("TBL_SCROLL");

	 /***** Write heading *****/
	 Grp_WriteGrpHead ();

	 /***** Query from the database the groups of this type which I belong to *****/
	 Grp_GetLstCodGrpsUsrBelongs (Gbl.Usrs.Me.UsrDat.UsrCod,GrpTyp->GrpTypCod,
				      &LstGrpsIBelong,
				      Grp_CLOSED_AND_OPEN_GROUPS);

	 /***** List the groups of this type *****/
	 for (NumGrpThisType = 0;
	      NumGrpThisType < GrpTyp->NumGrps;
	      NumGrpThisType++)
	   {
	    /* Pointer to group */
	    Grp = &(GrpTyp->LstGrps[NumGrpThisType]);

	    /* Check if I belong to his group */
	    IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,
							 &LstGrpsIBelong) ? Usr_BELONG :
									    Usr_DONT_BELONG;

	    /* Check if I can select / unselect this group */
	    switch (IBelongToThisGroup)
	      {
	       case Usr_BELONG:
		  ICanSelUnselGroup = Usr_CAN;
		  break;
	       case Usr_DONT_BELONG:
	       default:
		  switch (Gbl.Usrs.Me.Role.Logged)
		    {
		     case Rol_TCH:
		     case Rol_DEG_ADM:
		     case Rol_CTR_ADM:
		     case Rol_INS_ADM:
		     case Rol_SYS_ADM:
			ICanSelUnselGroup = Usr_CAN;
			break;
		     default:
			ICanSelUnselGroup = Usr_CAN_NOT;
			break;
		    }
		  break;
	      }

	    /* Put checkbox to select the group */
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin (IBelongToThisGroup == Usr_BELONG ? "class=\"LM BG_HIGHLIGHT\"" :
								"class=\"LM\"");
		  HTM_INPUT_CHECKBOX ("GrpCods",
				      Grp_Checked (Grp->GrpCod) |
				      (ICanSelUnselGroup == Usr_CAN ? HTM_NO_ATTR :
								      HTM_DISABLED),
				      "id=\"Grp%ld\" value=\"%ld\"%s",
				      Grp->GrpCod,Grp->GrpCod,
				      ICanSelUnselGroup == Usr_CAN ? " onclick=\"checkParent(this,'AllGroups')\"" :
								     "");
	       HTM_TD_End ();

	       Grp_WriteRowGrp (Grp,IBelongToThisGroup == Usr_BELONG ? Lay_HIGHLIGHT :
								       Lay_DONT_HIGHLIGHT);

	    HTM_TR_End ();
	   }

	 /***** Free memory with the list of groups which I belongs to *****/
	 Grp_FreeListCodGrp (&LstGrpsIBelong);

	 /***** Write row to select the users who don't belong to any group *****/
	 HTM_TR_Begin (NULL);

	    /* Write checkbox to select the group */
	    HTM_TD_Begin ("class=\"LM\"");
	       // To get the users who don't belong to a type of group,
	       // use group code -(GrpTyp->GrpTypCod)
	       HTM_INPUT_CHECKBOX ("GrpCods",
				   Grp_Checked (-GrpTyp->GrpTypCod),
				   "id=\"Grp%ld\" value=\"%ld\""
				   " onclick=\"checkParent(this,'AllGroups')\"",
				   -GrpTyp->GrpTypCod,-GrpTyp->GrpTypCod);
	    HTM_TD_End ();

	    /* Column closed/open */
	    HTM_TD_Begin ("class=\"LM\"");
	    HTM_TD_End ();

	    /* Group name = students with no group */
	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	       HTM_LABEL_Begin ("for=\"Grp%ld\"",-GrpTyp->GrpTypCod);
		  HTM_Txt (Txt_users_with_no_group);
	       HTM_LABEL_End ();
	    HTM_TD_End ();

	    /* Room */
	    HTM_TD_Empty (1);

	    /* Number of students who don't belong to any group of this type */
	    for (Role  = Rol_TCH;
		 Role >= Rol_STD;
		 Role--)
	      {
	       HTM_TD_Begin ("class=\"CM DAT_%s\"",The_GetSuffix ());
		  HTM_Unsigned (Grp_DB_CountNumUsrsInNoGrpsOfType (Role,GrpTyp->GrpTypCod));
	       HTM_TD_End ();
	      }

	    /* Last empty columns for max. students and vacants */
	    HTM_TD_Empty (2);

	 HTM_TR_End ();

      /***** End table *****/
      HTM_TABLE_End ();
   HTM_FIELDSET_End ();
  }

/*****************************************************************************/
/******** The checkbox associated to a given group should be checked? ********/
/*****************************************************************************/

static HTM_Attributes_t Grp_Checked (long GrpCod)
  {
   unsigned NumGrpSel;

   if (Gbl.Crs.Grps.AllGrpsSel)
      return HTM_CHECKED;

   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
      if (Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel] == GrpCod)
	 return HTM_CHECKED;

   return HTM_NO_ATTR;
  }

/*****************************************************************************/
/*********************** Alert with groups opening date **********************/
/*****************************************************************************/

static void Grp_WriteGrpTypOpening (const struct GroupType *GrpTyp)
  {
   extern const char *Txt_Opening_of_groups;
   static unsigned UniqueId = 0;
   char *Id;

   /***** Begin alert *****/
   Ale_ShowAlertAndButtonBegin (Ale_INFO,Txt_Opening_of_groups);

      /***** Opening date *****/
      UniqueId++;
      if (asprintf (&Id,"open_time_%u",UniqueId) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_SPAN_Begin ("id=\"%s\" class=\"ALERT_TXT_%s\"",Id,The_GetSuffix ());
      HTM_SPAN_End ();
      Dat_WriteLocalDateHMSFromUTC (Id,GrpTyp->OpenTimeUTC,
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				    Dat_WRITE_TODAY |
				    Dat_WRITE_DATE_ON_SAME_DAY |
				    Dat_WRITE_WEEK_DAY |
				    Dat_WRITE_HOUR |
				    Dat_WRITE_MINUTE |
				    Dat_WRITE_SECOND);
      free (Id);

   /***** End alert *****/
   Ale_ShowAlertAndButtonEnd (ActUnk,NULL,NULL,NULL,NULL,Btn_NO_BUTTON);
  }

/*****************************************************************************/
/************** Write a row with the head for list of groups *****************/
/*****************************************************************************/

static void Grp_WriteGrpHead (void)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Room;
   extern const char *Txt_Max_BR_students;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Vacants;
   Rol_Role_t Role;

   /***** Head row with title of each column *****/
   HTM_TR_Begin (NULL);

      HTM_TH_Empty (2);
      HTM_TH (Txt_Group          ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Room           ,HTM_HEAD_LEFT  );
      for (Role  = Rol_TCH;
	   Role >= Rol_STD;
	   Role--)
         HTM_TH (Txt_ROLES_PLURAL_BRIEF_Abc[Role],HTM_HEAD_CENTER);
      HTM_TH (Txt_Max_BR_students,HTM_HEAD_CENTER);
      HTM_TH (Txt_Vacants        ,HTM_HEAD_CENTER);

   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Write a row with the data of a group *********************/
/*****************************************************************************/

static void Grp_WriteRowGrp (const struct Group *Grp,Lay_Highlight_t Highlight)
  {
   extern const char *Txt_Group_X_open;
   extern const char *Txt_Group_X_closed;
   char *Title;
   int Vacant;
   Rol_Role_t Role;
   char StrMaxStudents[Cns_MAX_DIGITS_UINT + 1];
   static const char *HighlightClass[Lay_NUM_HIGHLIGHT] =
     {
      [Lay_DONT_HIGHLIGHT] = "",
      [Lay_HIGHLIGHT   ] = " BG_HIGHLIGHT",
     };
   static const char **TitleFormat[CloOpe_NUM_CLOSED_OPEN] =
     {
      [CloOpe_CLOSED] = &Txt_Group_X_closed,
      [CloOpe_OPEN  ] = &Txt_Group_X_open,
     };
   static const char *Icon[CloOpe_NUM_CLOSED_OPEN] =
     {
      [CloOpe_CLOSED] = "lock.svg",
      [CloOpe_OPEN  ] = "unlock.svg",
     };
   static Ico_Color_t Color[CloOpe_NUM_CLOSED_OPEN] =
     {
      [CloOpe_CLOSED] = Ico_RED,
      [CloOpe_OPEN  ] = Ico_GREEN,
     };


   /***** Write icon to show if group is open or closed *****/
   HTM_TD_Begin ("class=\"BM%s\"",HighlightClass[Highlight]);
      if (asprintf (&Title,*TitleFormat[Grp->Open],Grp->Name) < 0)
	 Err_NotEnoughMemoryExit ();
      Ico_PutIconOff (Icon[Grp->Open],Color[Grp->Open],Title);
      free (Title);
   HTM_TD_End ();

   /***** Group name *****/
   HTM_TD_Begin ("class=\"LM%s\"",HighlightClass[Highlight]);
      HTM_LABEL_Begin ("for=\"Grp%ld\" class=\"DAT_%s\"",
		       Grp->GrpCod,The_GetSuffix ());
	 HTM_Txt (Grp->Name);
      HTM_LABEL_End ();
   HTM_TD_End ();

   /***** Room *****/
   HTM_TD_Begin ("class=\"LM DAT_%s%s\"",
		 The_GetSuffix (),HighlightClass[Highlight]);
      HTM_Txt (Grp->Room.ShrtName);
   HTM_TD_End ();

   /***** Current number of users in this group *****/
   for (Role  = Rol_TCH;
	Role >= Rol_STD;
	Role--)
     {
      HTM_TD_Begin ("class=\"CM DAT_%s%s\"",
                    The_GetSuffix (),HighlightClass[Highlight]);
	 HTM_Int (Grp->NumUsrs[Role]);
      HTM_TD_End ();
     }

   /***** Max. number of students in this group *****/
   HTM_TD_Begin ("class=\"CM DAT_%s%s\"",
                 The_GetSuffix (),HighlightClass[Highlight]);
      Grp_WriteMaxStds (StrMaxStudents,Grp->MaxStds);
      HTM_Txt (StrMaxStudents);
      HTM_NBSP ();
   HTM_TD_End ();

   /***** Vacants in this group *****/
   HTM_TD_Begin ("class=\"CM DAT_%s%s\"",
                 The_GetSuffix (),HighlightClass[Highlight]);
      if (Grp->MaxStds <= Grp_MAX_STUDENTS_IN_A_GROUP)
	{
	 Vacant = (int) Grp->MaxStds - (int) Grp->NumUsrs[Rol_STD];
	 HTM_Unsigned (Vacant > 0 ? (unsigned) Vacant :
				    0);
	}
   HTM_TD_End ();
  }

/*****************************************************************************/
/********************* Put a form to create a new group type *****************/
/*****************************************************************************/

static void Grp_PutFormToCreateGroupType (const struct GroupType *GrpTyp)
  {
   extern const char *Txt_Type_of_group;
   extern const char *Txt_It_is_optional_to_choose_a_group;
   extern const char *Txt_It_is_mandatory_to_choose_a_group;
   extern const char *Txt_A_student_can_belong_to_several_groups;
   extern const char *Txt_A_student_can_only_belong_to_one_group;
   extern const char *Txt_The_groups_will_automatically_open;
   extern const char *Txt_The_groups_will_not_automatically_open;
   unsigned CurrentYear = Dat_GetCurrentYear ();

   /***** Begin section *****/
   HTM_SECTION_Begin (Grp_NEW_GROUP_TYPE_SECTION_ID);

      /***** Begin form to create *****/
      Frm_BeginFormTable (ActNewGrpTyp,Grp_NEW_GROUP_TYPE_SECTION_ID,
                          NULL,NULL,"TBL_SCROLL");

	 /***** Write heading *****/
	 Grp_WriteHeadingGroupTypes ();

	 HTM_TR_Begin (NULL);

	    /***** Column to remove group type, disabled here *****/
	    HTM_TD_Begin ("class=\"BM\"");
	    HTM_TD_End ();

	    /***** Column to get resource link, disabled here *****/
	    HTM_TD_Begin ("class=\"BM\"");
	    HTM_TD_End ();

	    /***** Name of group type *****/
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_INPUT_TEXT ("GrpTypName",Grp_MAX_CHARS_GROUP_TYPE_NAME,
			       GrpTyp->Name,
			       HTM_REQUIRED,
			       "size=\"12\" class=\"INPUT_%s\"",The_GetSuffix ());
	    HTM_TD_End ();

	    /***** Is it mandatory to enrol in any groups of this type? *****/
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
				 "name=\"OptionalMandatory\""
				 " class=\"INPUT_%s\" style=\"width:150px;\"",
				 The_GetSuffix ());
		  HTM_OPTION (HTM_Type_STRING,"N",
			      GrpTyp->Enrolment.OptionalMandatory == Grp_OPTIONAL  ? HTM_SELECTED :
										     HTM_NO_ATTR,
			      "%s",Txt_It_is_optional_to_choose_a_group);
		  HTM_OPTION (HTM_Type_STRING,"Y",
			      GrpTyp->Enrolment.OptionalMandatory == Grp_MANDATORY ? HTM_SELECTED :
										     HTM_NO_ATTR,
			      "%s",Txt_It_is_mandatory_to_choose_a_group);
	       HTM_SELECT_End ();
	    HTM_TD_End ();

	    /***** Is it possible to enrol in multiple groups of this type? *****/
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
				 "name=\"SingleMultiple\""
				 " class=\"INPUT_%s\" style=\"width:150px;\"",
				 The_GetSuffix ());
		  HTM_OPTION (HTM_Type_STRING,"N",
			      GrpTyp->Enrolment.SingleMultiple == Grp_SINGLE   ? HTM_SELECTED :
										 HTM_NO_ATTR,
			      "%s",Txt_A_student_can_only_belong_to_one_group);
		  HTM_OPTION (HTM_Type_STRING,"Y",
			      GrpTyp->Enrolment.SingleMultiple == Grp_MULTIPLE ? HTM_SELECTED :
										 HTM_NO_ATTR,
			      "%s",Txt_A_student_can_belong_to_several_groups);
	       HTM_SELECT_End ();
	    HTM_TD_End ();

	    /***** Open time *****/
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_TABLE_BeginCenterPadding (2);
		  HTM_TR_Begin (NULL);

		     HTM_TD_Begin ("class=\"LM\" style=\"width:20px;\"");
			switch (GrpTyp->MustBeOpened)
			  {
			   case Grp_MUST_BE_OPENED:
			      Ico_PutIconOn ("clock.svg",Ico_BLACK,
					     Txt_The_groups_will_automatically_open);
			      break;
			   case Grp_MUST_NOT_BE_OPENED:
			   default:
			      Ico_PutIconOff ("clock.svg",Ico_BLACK,
					      Txt_The_groups_will_not_automatically_open);
			      break;
			  }
		     HTM_TD_End ();

		     HTM_TD_Begin ("class=\"LM\"");
			Dat_WriteFormClientLocalDateTimeFromTimeUTC ("open_time",
								     Dat_STR_TIME,
								     GrpTyp->OpenTimeUTC,
								     CurrentYear,
								     CurrentYear + 1,
								     Dat_FORM_SECONDS_ON,
								     Dat_HMS_DO_NOT_SET,	// Don't set hour, minute and second
								     HTM_NO_ATTR);
		     HTM_TD_End ();

		  HTM_TR_End ();
	       HTM_TABLE_End ();
	    HTM_TD_End ();

	    /***** Number of groups of this type *****/
	    HTM_TD_Begin ("class=\"CM DAT_%s\"",The_GetSuffix ());
	       HTM_0 ();	// It's a new group type ==> 0 groups
	    HTM_TD_End ();

	 HTM_TR_End ();

      /***** End form to create *****/
      Frm_EndFormTable (Btn_CREATE);

   /***** End section *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/*********************** Put a form to create a new group ********************/
/*****************************************************************************/

static void Grp_PutFormToCreateGroup (const struct GroupType *CurrentGrpTyp,
				      const struct Group *Grp,
				      const struct Roo_Rooms *Rooms)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Group_closed;
   extern const char *Txt_File_zones_disabled;
   extern const char *Txt_No_assigned_room;
   extern const char *Txt_Another_room;
   unsigned NumGrpTyp;
   const struct GroupType *GrpTyp;
   unsigned NumRoo;
   const struct Roo_Room *Roo;
   Rol_Role_t Role;
   char StrMaxStudents[Cns_MAX_DIGITS_UINT + 1];

   /***** Begin section *****/
   HTM_SECTION_Begin (Grp_NEW_GROUP_SECTION_ID);

      /***** Begin form to create *****/
      Frm_BeginFormTable (ActNewGrp,Grp_GROUPS_SECTION_ID,NULL,NULL,"TBL_SCROLL");

	 /***** Write heading *****/
	 Grp_WriteHeadingGroups ();

	 HTM_TR_Begin (NULL);

	    /***** Empty column to remove *****/
	    HTM_TD_Begin ("class=\"BM\"");
	    HTM_TD_End ();

	    /***** Disabled icon to open group *****/
	    HTM_TD_Begin ("class=\"BM\"");
	       Ico_PutIconOff ("lock.svg",Ico_RED,Txt_Group_closed);
	    HTM_TD_End ();

	    /***** Disabled icon for archive zone *****/
	    HTM_TD_Begin ("class=\"BM\"");
	       Ico_PutIconOff ("folder.svg",Ico_RED,Txt_File_zones_disabled);
	    HTM_TD_End ();

	    /***** Group type *****/
	    /* Begin selector */
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
				 "name=\"GrpTypCod\""
				 " class=\"INPUT_%s\" style=\"width:100px;\"",
				 The_GetSuffix ());

		  /* Options for group types */
		  for (NumGrpTyp = 0;
		       NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
		       NumGrpTyp++)
		    {
		     GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

		     HTM_OPTION (HTM_Type_LONG,&GrpTyp->GrpTypCod,
				 GrpTyp->GrpTypCod == CurrentGrpTyp->GrpTypCod ? HTM_SELECTED :
									         HTM_NO_ATTR,
				 "%s",GrpTyp->Name);
		    }

	       /* End selector */
	       HTM_SELECT_End ();
	    HTM_TD_End ();

	    /***** Group name *****/
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_INPUT_TEXT ("GrpName",Grp_MAX_CHARS_GROUP_NAME,Grp->Name,
			       HTM_REQUIRED,
			       "size=\"20\" class=\"INPUT_%s\"",The_GetSuffix ());
	    HTM_TD_End ();

	    /***** Room *****/
	    /* Begin selector */
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
				 "name=\"RooCod\""
				 " class=\"INPUT_%s\" style=\"width:100px;\"",
				 The_GetSuffix ());

		  /* Option for no assigned room */
		  HTM_OPTION (HTM_Type_STRING,"-1",
			      Grp->Room.RooCod < 0 ? HTM_SELECTED :
					             HTM_NO_ATTR,
			      "%s",Txt_No_assigned_room);

		  /* Option for another room */
		  HTM_OPTION (HTM_Type_STRING,"0",
			      Grp->Room.RooCod == 0 ? HTM_SELECTED :
						      HTM_NO_ATTR,
			      "%s",Txt_Another_room);

		  /* Options for rooms */
		  for (NumRoo = 0;
		       NumRoo < Rooms->Num;
		       NumRoo++)
		    {
		     Roo = &Rooms->Lst[NumRoo];
		     HTM_OPTION (HTM_Type_LONG,&Roo->RooCod,
				 Roo->RooCod == Grp->Room.RooCod ? HTM_SELECTED :
								   HTM_NO_ATTR,
				 "%s",Roo->ShrtName);
		    }

	       /* End selector */
	       HTM_SELECT_End ();
	    HTM_TD_End ();

	    /***** Current number of users in this group *****/
	    for (Role  = Rol_TCH;
		 Role >= Rol_STD;
		 Role--)
	      {
	       HTM_TD_Begin ("class=\"CM DAT_%s\"",The_GetSuffix ());
		  HTM_0 ();
	       HTM_TD_End ();
	      }

	    /***** Maximum number of students *****/
	    HTM_TD_Begin ("class=\"CM\"");
	       Grp_WriteMaxStds (StrMaxStudents,Grp->MaxStds);
	       HTM_INPUT_TEXT ("MaxStudents",Cns_MAX_DIGITS_UINT,StrMaxStudents,
			       HTM_NO_ATTR,
			       "size=\"3\" class=\"INPUT_%s\"",The_GetSuffix ());
	    HTM_TD_End ();

	 HTM_TR_End ();

      /***** End form to create *****/
      Frm_EndFormTable (Btn_CREATE);

   /***** End section *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/*********** Create a list with current group types in this course ***********/
/*****************************************************************************/

void Grp_GetListGrpTypesInCurrentCrs (Grp_WhichGrpTypes_t WhichGrpTypes)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumGrpTyp;
   struct GroupType *GrpTyp;
   static unsigned (*Grp_DB_GetGrpTypesInCurrentCrs[Grp_NUM_WHICH_GROUP_TYPES]) (MYSQL_RES **mysql_res,
										 long HieCod) =
    {
     [Grp_GRP_TYPES_WITH_GROUPS] = Grp_DB_GetGrpTypesWithGrpsInCrs,
     [Grp_ALL_GROUP_TYPES      ] = Grp_DB_GetAllGrpTypesInCrs,
    };

   if (++Gbl.Crs.Grps.GrpTypes.NestedCalls > 1) // If list is created yet, there's nothing to do
      return;

   /***** Open groups of this course that must be opened
          if open time is in the past *****/
   Grp_OpenGroupsAutomatically ();

   /***** Get group types from database *****/
   Gbl.Crs.Grps.GrpTypes.NumGrpTypes =
   Grp_DB_GetGrpTypesInCurrentCrs[WhichGrpTypes] (&mysql_res,
						  Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   /***** Get group types *****/
   Gbl.Crs.Grps.GrpTypes.NumGrpsTotal = 0;

   if (Gbl.Crs.Grps.GrpTypes.NumGrpTypes)
     {
      /***** Create a list of group types *****/
      if ((Gbl.Crs.Grps.GrpTypes.LstGrpTypes = calloc (Gbl.Crs.Grps.GrpTypes.NumGrpTypes,
                                                       sizeof (*Gbl.Crs.Grps.GrpTypes.LstGrpTypes))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get group types *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	   NumGrpTyp++)
        {
	 GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

         /* Get next group type */
         row = mysql_fetch_row (mysql_res);
         /*
	 row[0]: grp_types.GrpTypCod
	 row[1]: grp_types.GrpTypName
	 row[2]: grp_types.Mandatory
	 row[3]: grp_types.Multiple
	 row[4]: grp_types.MustBeOpened
	 row[5]: UNIX_TIMESTAMP(grp_types.OpenTime)
	 row[6]: COUNT(grp_groups.GrpCod
	*/

         /* Get group type code (row[0]) */
         if ((GrpTyp->GrpTypCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Err_WrongGrpTypExit ();

         /* Get group type name (row[1]) */
         Str_Copy (GrpTyp->Name,row[1],sizeof (GrpTyp->Name) - 1);

         /* Is it mandatory to enrol in any groups of this type? (row[2]) */
         GrpTyp->Enrolment.OptionalMandatory = Grp_GetMandatoryFromYN (row[2][0]);

         /* Is it possible to enrol in multiple groups of this type? (row[3]) */
         GrpTyp->Enrolment.SingleMultiple = Grp_MultipleFromYN (row[3][0]);

         /* Groups of this type must be opened? (row[4]) */
         GrpTyp->MustBeOpened = Grp_MustBeOpenedFromYN (row[4][0]);

         /* Get open time (row[5] holds the open time UTC) */
         GrpTyp->OpenTimeUTC = Dat_GetUNIXTimeFromStr (row[5]);
         if (GrpTyp->MustBeOpened == Grp_MUST_BE_OPENED)
            GrpTyp->MustBeOpened = Grp_CheckIfOpenTimeInTheFuture (GrpTyp->OpenTimeUTC) ? Grp_MUST_BE_OPENED :
											  Grp_MUST_NOT_BE_OPENED;

         /* Number of groups of this type (row[6]) */
         if (sscanf (row[6],"%u",&GrpTyp->NumGrps) != 1)
            Err_ShowErrorAndExit ("Wrong number of groups of a type.");

         /* Add number of groups to total number of groups */
         Gbl.Crs.Grps.GrpTypes.NumGrpsTotal += GrpTyp->NumGrps;

	 /* Initialize pointer to the list of groups of this type */
         GrpTyp->LstGrps = NULL;
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Open automatically groups in this course ******************/
/*****************************************************************************/

void Grp_OpenGroupsAutomatically (void)
  {
   MYSQL_RES *mysql_res;
   unsigned NumGrpTypes;
   unsigned NumGrpTyp;
   long GrpTypCod;

   /***** Find group types to be opened *****/
   NumGrpTypes = Grp_DB_GetGrpTypesInCurrentCrsToBeOpened (&mysql_res);

   for (NumGrpTyp = 0;
        NumGrpTyp < NumGrpTypes;
        NumGrpTyp++)
     {
      /* Get next group type */
      if ((GrpTypCod = DB_GetNextCode (mysql_res)) > 0)
        {
         /***** Open all closed groups of this type that are closed ****/
         Grp_DB_OpenGrpsOfType (GrpTypCod);

         /***** To not try to open groups again, set MustBeOpened to false *****/
         Grp_DB_ClearMustBeOpened (GrpTypCod);
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********* Create a list with group types and groups in this course **********/
/*****************************************************************************/

void Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_WhichGrpTypes_t WhichGrpTypes)
  {
   unsigned NumGrpTyp;
   unsigned NumGrp;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct GroupType *GrpTyp;
   struct Group *Grp;
   Rol_Role_t Role;

   /***** First we get the list of group types *****/
   Grp_GetListGrpTypesInCurrentCrs (WhichGrpTypes);

   /***** Then we get the list of groups for each group type *****/
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	NumGrpTyp++)
     {
      GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

      if (GrpTyp->NumGrps)	 // If there are groups of this type...
        {
         /***** Query database *****/
	 GrpTyp->NumGrps = Grp_DB_GetGrpsOfType (&mysql_res,GrpTyp->GrpTypCod);
         if (GrpTyp->NumGrps > 0) // Groups found...
           {
            /***** Create list with groups of this type *****/
            if ((GrpTyp->LstGrps = calloc ((size_t) GrpTyp->NumGrps,
                                           sizeof (*GrpTyp->LstGrps))) == NULL)
               Err_NotEnoughMemoryExit ();

            /***** Get the groups of this type *****/
            for (NumGrp = 0;
        	 NumGrp < GrpTyp->NumGrps;
        	 NumGrp++)
              {
               Grp = &(GrpTyp->LstGrps[NumGrp]);

               /* Get next group */
               row = mysql_fetch_row (mysql_res);

               /* Get group code (row[0]) */
               if ((Grp->GrpCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
                  Err_WrongGroupExit ();

               /* Get group name (row[1]) */
               Str_Copy (Grp->Name,row[1],sizeof (Grp->Name) - 1);

               /* Get room code (row[2]) */
               Grp->Room.RooCod = Str_ConvertStrCodToLongCod (row[2]);

               /* Get room short name (row[3]) */
	       Str_Copy (Grp->Room.ShrtName,row[3],
		         sizeof (Grp->Room.ShrtName) - 1);

               /* Get number of current users in group */
	       for (Role  = Rol_TCH;
		    Role >= Rol_STD;
		    Role--)
                  Grp->NumUsrs[Role] = Grp_DB_CountNumUsrsInGrp (Role,Grp->GrpCod);

               /* Get maximum number of students in group (row[4]) */
               Grp->MaxStds = Grp_ConvertToNumMaxStdsGrp (row[4]);

               /* Get whether group is open ('Y') or closed ('N') (row[5]),
                  and whether group have file zones ('Y') or not ('N') (row[6]) */
               Grp->Open = CloOpe_GetOpenFromYN (row[5][0]);
               Grp->HasFileZones = Grp_GetHasFileZonesFromYN (row[6][0]);
              }
           }

         /***** Free structure that stores the query result *****/
         DB_FreeMySQLResult (&mysql_res);
        }
     }
  }

/*****************************************************************************/
/********* Free list of groups types and list of groups of each type *********/
/*****************************************************************************/

void Grp_FreeListGrpTypesAndGrps (void)
  {
   unsigned NumGrpTyp;
   struct GroupType *GrpTyp;

   if (Gbl.Crs.Grps.GrpTypes.NestedCalls > 0)
      if (--Gbl.Crs.Grps.GrpTypes.NestedCalls == 0)
         if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes)
           {
	    /***** Free memory used for each list of groups
	           (one list for each group type) *****/
	    for (NumGrpTyp = 0;
		 NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
		 NumGrpTyp++)
              {
               GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

               if (GrpTyp->LstGrps)
                 {
                  free (GrpTyp->LstGrps);
		  GrpTyp->LstGrps = NULL;
		  GrpTyp->NumGrps = 0;
                 }
              }

	    /***** Free memory used by the list of group types *****/
            free (Gbl.Crs.Grps.GrpTypes.LstGrpTypes);
            Gbl.Crs.Grps.GrpTypes.LstGrpTypes = NULL;
            Gbl.Crs.Grps.GrpTypes.NumGrpTypes = 0;
           }
  }

/*****************************************************************************/
/******************* Get data of a group type from its code ******************/
/*****************************************************************************/
// GrpTyp->GrpTypCod must have the code of the type of group

void Grp_GetGroupTypeDataByCod (struct GroupType *GrpTyp)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (GrpTyp->GrpTypCod > 0)
     {
      /***** Get data of a type of group from database *****/
      if (Grp_DB_GetGroupTypeDataFromGrpTypCod (&mysql_res,
						GrpTyp->GrpTypCod) == Exi_DOES_NOT_EXIST)
	 Err_WrongGrpTypExit ();

      /***** Get some data of group type *****/
      row = mysql_fetch_row (mysql_res);
      /*
      row[0]: GrpTypName
      row[1]: Mandatory
      row[2]: Multiple
      row[3]: MustBeOpened
      row[4]: UNIX_TIMESTAMP(OpenTime)
      */
      Str_Copy (GrpTyp->Name,row[0],sizeof (GrpTyp->Name) - 1);
      GrpTyp->Enrolment.OptionalMandatory = Grp_GetMandatoryFromYN (row[1][0]);
      GrpTyp->Enrolment.SingleMultiple = Grp_MultipleFromYN (row[2][0]);
      GrpTyp->MustBeOpened = Grp_MustBeOpenedFromYN (row[3][0]);
      GrpTyp->OpenTimeUTC = Dat_GetUNIXTimeFromStr (row[4]);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      Grp_ResetGroupType (GrpTyp);
  }

/*****************************************************************************/
/************* Check if a group type has multiple enrolment *****************/
/*****************************************************************************/

static Grp_SingleOrMultiple_t Grp_GetSingleMultiple (long GrpTypCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Grp_SingleOrMultiple_t SingleMultiple;

   /***** Get data of a type of group from database *****/
   if (Grp_DB_GetSingleMultiple (&mysql_res,GrpTypCod) == Exi_DOES_NOT_EXIST)
      Err_ShowErrorAndExit ("Error when getting type of enrolment.");

   /***** Get multiple enrolment *****/
   row = mysql_fetch_row (mysql_res);
   SingleMultiple = Grp_MultipleFromYN (row[0][0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return SingleMultiple;
  }

/*****************************************************************************/
/****************** Check if a group type has file zones *********************/
/*****************************************************************************/

Grp_HasFileZones_t Grp_GetFileZones (long GrpCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Grp_HasFileZones_t HasFileZones;

   /***** Get data of a group from database *****/
   if (Grp_DB_GetFileZones (&mysql_res,GrpCod) == Exi_DOES_NOT_EXIST)
      Err_ShowErrorAndExit ("Error when getting file zones.");

   /***** Get file zones *****/
   row = mysql_fetch_row (mysql_res);
   HasFileZones = Grp_GetHasFileZonesFromYN (row[0][0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return HasFileZones;
  }

/*****************************************************************************/
/********************** Get data of a group from its code ********************/
/*****************************************************************************/

void Grp_GetGroupDataByCod (long *CrsCod,long *GrpTypCod,struct Group *Grp)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Reset values *****/
   *CrsCod               = -1L;
   *GrpTypCod            = -1L;
   Grp->Name[0]          = '\0';
   Grp->Room.RooCod      = -1L;
   Grp->Room.ShrtName[0] = '\0';
   Grp->MaxStds          = 0;
   Grp->Open             = CloOpe_CLOSED;
   Grp->HasFileZones     = Grp_HAS_NOT_FILEZONES;

   if (Grp->GrpCod > 0)
     {
      /***** Get data of a group from database *****/
      if (Grp_DB_GetGroupDataByCod (&mysql_res,Grp->GrpCod) == Exi_EXISTS)
	{
	 /***** Get data of group *****/
	 row = mysql_fetch_row (mysql_res);
	 // row[0]: grp_groups.GrpTypCod
	 // row[1]: grp_types.CrsCod
	 // row[2]: grp_groups.GrpName
	 // row[3]: grp_groups.RooCod
	 // row[4]: grp_groups.MaxStudents
	 // row[5]: grp_groups.Open
	 // row[6]: grp_groups.FileZones
	 // row[7]: roo_rooms.ShortName

	 /* Get the code of the group type (row[0]) */
	 if ((*GrpTypCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
	    Err_WrongGrpTypExit ();

	 /* Get the code of the course (row[1]) */
	 if ((*CrsCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
	    Err_WrongCourseExit ();

	 /* Get the name of the group (row[2]) */
	 Str_Copy (Grp->Name,row[2],sizeof (Grp->Name) - 1);

	 /* Get the code of the room (row[3]) */
	 Grp->Room.RooCod = Str_ConvertStrCodToLongCod (row[3]);

	 /* Get maximum number of students (row[4]) */
	 Grp->MaxStds = Grp_ConvertToNumMaxStdsGrp (row[4]);

	 /* Get whether group is open or closed (row[5]),
	    and whether group has file zones (row[6]) */
	 Grp->Open = CloOpe_GetOpenFromYN (row[5][0]);
	 Grp->HasFileZones = Grp_GetHasFileZonesFromYN (row[6][0]);

	 /* Get the name of the room (row[7]) */
         Str_Copy (Grp->Room.ShrtName,row[7],sizeof (Grp->Room.ShrtName) - 1);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/********** Get if optional or mandatory from a 'Y'/'N' character ************/
/*****************************************************************************/

static Grp_OptionalOrMandatory_t Grp_GetMandatoryFromYN (char Ch)
  {
   return Ch == 'Y' ? Grp_MANDATORY :
		      Grp_OPTIONAL;
  }

/*****************************************************************************/
/************ Get if single or multiple from a 'Y'/'N' character *************/
/*****************************************************************************/

static Grp_SingleOrMultiple_t Grp_MultipleFromYN (char Ch)
  {
   return Ch == 'Y' ? Grp_MULTIPLE :
		      Grp_SINGLE;
  }

/*****************************************************************************/
/************** Get if must be opened from a 'Y'/'N' character ***************/
/*****************************************************************************/

static Grp_MustBeOpened_t Grp_MustBeOpenedFromYN (char Ch)
  {
   return Ch == 'Y' ? Grp_MUST_BE_OPENED :
		      Grp_MUST_NOT_BE_OPENED;
  }

/*****************************************************************************/
/***************** Get if file zones from a 'Y'/'N' character ****************/
/*****************************************************************************/

static Grp_HasFileZones_t Grp_GetHasFileZonesFromYN (char Ch)
  {
   return Ch == 'Y' ? Grp_HAS_FILEZONES :
		      Grp_HAS_NOT_FILEZONES;
  }

/*****************************************************************************/
/************************ Check if I belong to a group ***********************/
/*****************************************************************************/
// Return true if I belong to group with code GrpCod

void Grp_FlushCacheIBelongToGrp (void)
  {
   Gbl.Cache.IBelongToGrp.Status = Cac_INVALID;
  }

Usr_Belong_t Grp_GetIfIBelongToGrp (long GrpCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (GrpCod <= 0)
      return Usr_DONT_BELONG;

   /***** 2. Fast check: Is already calculated if I belong to group? *****/
   if (Gbl.Cache.IBelongToGrp.Status == Cac_VALID &&
       GrpCod == Gbl.Cache.IBelongToGrp.GrpCod)
      return Gbl.Cache.IBelongToGrp.IBelong;

   /***** 3. Slow check: Get if I belong to a group from database *****/
   Gbl.Cache.IBelongToGrp.GrpCod = GrpCod;
   Gbl.Cache.IBelongToGrp.IBelong = Grp_DB_CheckIfIBelongToGrp (GrpCod);
   Gbl.Cache.IBelongToGrp.Status = Cac_VALID;
   return Gbl.Cache.IBelongToGrp.IBelong;
  }

/*****************************************************************************/
/***** Check if a user belongs to any of my groups in the current course *****/
/*****************************************************************************/

void Grp_FlushCacheUsrSharesAnyOfMyGrpsInCurrentCrs (void)
  {
   Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Status = Cac_INVALID;
  }

bool Grp_CheckIfUsrSharesAnyOfMyGrpsInCurrentCrs (const struct Usr_Data *UsrDat)
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

   /***** 4. Fast check: Do I belong to the current course? *****/
   if (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] == Usr_DONT_BELONG)
      return false;

   /***** 5. Fast check: It's me? *****/
   if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)
      return true;

   /***** 6. Fast check: Is already calculated if user shares
                         any group in the current course with me? *****/
   if (Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Status == Cac_VALID &&
       UsrDat->UsrCod == Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.UsrCod)
      return Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Shares;

   /***** 7. Fast / slow check: Does he/she belong to the current course? *****/
   if (Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat) == Usr_DONT_BELONG)
     {
      Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.UsrCod = UsrDat->UsrCod;
      Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Shares = false;
      Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Status = Cac_VALID;
      return false;
     }

   /***** 8. Fast check: Course has groups? *****/
   if (!Gbl.Crs.Grps.NumGrps)
     {
      Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.UsrCod = UsrDat->UsrCod;
      Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Shares = true;
      Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Status = Cac_VALID;
      return true;
     }

   // Course has groups

   /***** 9. Slow check: Get if user shares any group in this course with me from database *****/
   /* Check if user shares any group with me */
   Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.UsrCod = UsrDat->UsrCod;
   Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Shares = Grp_DB_CheckIfUsrSharesAnyOfMyGrpsInCurrentCrs (UsrDat->UsrCod);
   Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Status = Cac_VALID;
   return Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Shares;
  }

/*****************************************************************************/
/****** Query list of group codes of a type to which a user belongs to *******/
/*****************************************************************************/
// If GrpTypCod < 0 ==> get the groups of any type

static void Grp_GetLstCodGrpsUsrBelongs (long UsrCod,long GrpTypCod,
                                         struct ListCodGrps *LstGrps,
                                         Grp_ClosedOpenGrps_t ClosedOpenGroups)
  {
   MYSQL_RES *mysql_res;
   unsigned NumGrp;

   /***** Get groups which a user belong to from database *****/
   if (GrpTypCod < 0)		// Query the groups of any type in the current course
      LstGrps->NumGrps = Grp_DB_GetLstCodGrpsOfAnyTypeInCurrentCrsUsrBelongs (&mysql_res,UsrCod,
									      ClosedOpenGroups);
   else				// Query only the groups of specified type in the current course
      LstGrps->NumGrps = Grp_DB_GetLstCodGrpsOfATypeInCurrentCrsUsrBelongs (&mysql_res,UsrCod,GrpTypCod);

   /***** Get the groups *****/
   if (LstGrps->NumGrps)
     {
      /***** Create a list of groups the user belongs to *****/
      if ((LstGrps->GrpCods = calloc (LstGrps->NumGrps,
                                      sizeof (*LstGrps->GrpCods))) == NULL)
         Err_NotEnoughMemoryExit ();
      for (NumGrp = 0;
	   NumGrp < LstGrps->NumGrps;
	   NumGrp++)
         /* Get the code of group (row[0]) */
         if ((LstGrps->GrpCods[NumGrp] = DB_GetNextCode (mysql_res)) <= 0)
            Err_WrongGroupExit ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Query list of group codes with file zones I belong to ************/
/*****************************************************************************/

void Grp_GetLstCodGrpsWithFileZonesIBelong (struct ListCodGrps *LstGrps)
  {
   MYSQL_RES *mysql_res;
   unsigned NumGrp;

   /***** Get groups with file zones which I belong to from database *****/
   LstGrps->NumGrps = Grp_DB_GetLstCodGrpsWithFileZonesInCurrentCrsIBelong (&mysql_res);

   /***** Get the groups *****/
   if (LstGrps->NumGrps)
     {
      /***** Create a list of groups I belong to *****/
      if ((LstGrps->GrpCods = calloc (LstGrps->NumGrps,
                                      sizeof (*LstGrps->GrpCods))) == NULL)
         Err_NotEnoughMemoryExit ();
      for (NumGrp = 0;
	   NumGrp < LstGrps->NumGrps;
	   NumGrp++)
         /* Get the code of group */
         if ((LstGrps->GrpCods[NumGrp] = DB_GetNextCode (mysql_res)) <= 0)
            Err_WrongGroupExit ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******** Check if a group is in a list of groups which I belong to **********/
/*****************************************************************************/

static bool Grp_CheckIfGrpIsInList (long GrpCod,struct ListCodGrps *LstGrps)
  {
   unsigned NumGrp;

   for (NumGrp = 0;
	NumGrp < LstGrps->NumGrps;
	NumGrp++)
      if (GrpCod == LstGrps->GrpCods[NumGrp])
         return true;

   return false;
  }

/*****************************************************************************/
/********** Query names of groups of a type which user belongs to ************/
/*****************************************************************************/

void Grp_GetNamesGrpsUsrBelongsTo (long UsrCod,long GrpTypCod,char *GroupNames)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrps;
   unsigned NumGrp;
   size_t MaxLength = (Grp_MAX_BYTES_GROUP_NAME + 2) *
		      Gbl.Crs.Grps.GrpTypes.NumGrpsTotal;

   /***** Get the names of groups which a user belongs to, from database *****/
   NumGrps = Grp_DB_GetNamesGrpsUsrBelongsTo (&mysql_res,UsrCod,GrpTypCod);

   /***** Get the groups *****/
   GroupNames[0] = '\0';
   for (NumGrp = 0;
	NumGrp < NumGrps;
	NumGrp++)
     {
      /* Get next group */
      row = mysql_fetch_row (mysql_res);

      /* Group name is in row[0] */
      if (NumGrp)
         Str_Concat (GroupNames,", ",MaxLength);
      Str_Concat (GroupNames,row[0],MaxLength);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Receive form to create a new group type ******************/
/*****************************************************************************/

void Grp_ReceiveNewGrpTyp (void)
  {
   extern const char *Txt_The_type_of_group_X_already_exists;
   extern const char *Txt_Created_new_type_of_group_X;
   extern const char *Txt_You_must_specify_the_name;
   struct GroupType GrpTyp;
   struct Group Grp;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_TYPE_NAME];

   /***** Get parameters from form *****/
   /* Get the name of group type */
   Par_GetParText ("GrpTypName",GrpTyp.Name,Grp_MAX_BYTES_GROUP_TYPE_NAME);

   /* Get whether it is mandatory to enrol in any group of this type
      and whether it is possible to enrol in multiple groups of this type */
   GrpTyp.Enrolment.OptionalMandatory = Par_GetParBool ("OptionalMandatory") ? Grp_MANDATORY :
									       Grp_OPTIONAL;
   GrpTyp.Enrolment.SingleMultiple    = Par_GetParBool ("SingleMultiple"   ) ? Grp_MULTIPLE :
									       Grp_SINGLE;

   /* Get open time */
   GrpTyp.OpenTimeUTC = Dat_GetTimeUTCFromForm (Dat_STR_TIME);
   GrpTyp.MustBeOpened = Grp_CheckIfOpenTimeInTheFuture (GrpTyp.OpenTimeUTC) ? Grp_MUST_BE_OPENED :
									       Grp_MUST_NOT_BE_OPENED;

   if (GrpTyp.Name[0])	// If there's a group type name
      /***** If name of group type was in database... *****/
      switch (Grp_DB_CheckIfGrpTypNameExistsInCurrentCrs (GrpTyp.Name,-1L))
        {
         case Exi_EXISTS:
	    AlertType = Ale_WARNING;
	    snprintf (AlertTxt,sizeof (AlertTxt),
		      Txt_The_type_of_group_X_already_exists,GrpTyp.Name);
	    break;
         case Exi_DOES_NOT_EXIST:
         default:
            /* Add new group type to database */
	    Grp_DB_CreateGroupType (&GrpTyp);
	    AlertType = Ale_SUCCESS;
	    snprintf (AlertTxt,sizeof (AlertTxt),
		      Txt_Created_new_type_of_group_X,GrpTyp.Name);
            break;
        }
   else	// If there is not a group type name
     {
      AlertType = Ale_WARNING;
      Str_Copy (AlertTxt,Txt_You_must_specify_the_name,sizeof (AlertTxt) - 1);
     }

   /***** Show the form again *****/
   Grp_ResetGroup (&Grp);
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,AlertType,AlertTxt,Ale_INFO,NULL);
  }

/*****************************************************************************/
/**************** Check if the open time if in the future ********************/
/*****************************************************************************/

static bool Grp_CheckIfOpenTimeInTheFuture (time_t OpenTimeUTC)
  {
   /***** If open time is 0 ==> groups must no be opened *****/
   if (OpenTimeUTC == (time_t) 0)
      return false;

   /***** Is open time in the future? *****/
   return (OpenTimeUTC > Dat_GetStartExecutionTimeUTC ());
  }

/*****************************************************************************/
/******************** Receive form to create a new group *********************/
/*****************************************************************************/

void Grp_ReceiveNewGrp (void)
  {
   extern const char *Txt_The_group_X_already_exists;
   extern const char *Txt_Created_new_group_X;
   extern const char *Txt_You_must_specify_the_name;
   struct GroupType GrpTyp;
   struct Group Grp;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get parameters from form *****/
   if ((GrpTyp.GrpTypCod = ParCod_GetPar (ParCod_GrpTyp)) > 0) // Group type valid
     {
      /* Get data of type of group from database */
      Grp_GetGroupTypeDataByCod (&GrpTyp);

      /* Get group name */
      Par_GetParText ("GrpName",Grp.Name,Grp_MAX_BYTES_GROUP_NAME);

      /* Get room */
      Grp.Room.RooCod = ParCod_GetPar (ParCod_Roo);

      /* Get maximum number of students */
      Grp.MaxStds = (unsigned)
		    Par_GetParUnsignedLong ("MaxStudents",
					    0,
					    Grp_MAX_STUDENTS_IN_A_GROUP,
					    Grp_NUM_STUDENTS_NOT_LIMITED);

      if (Grp.Name[0])	// If there's a group name
         /***** If name of group was in database... *****/
         switch (Grp_DB_CheckIfGrpNameExistsForGrpTyp (GrpTyp.GrpTypCod,
						       Grp.Name,-1L))
	   {
	    case Exi_EXISTS:
	       AlertType = Ale_WARNING;
	       snprintf (AlertTxt,sizeof (AlertTxt),
			 Txt_The_group_X_already_exists,Grp.Name);
	       break;
	    case Exi_DOES_NOT_EXIST:
	    default:
	       /* Add new group to database */
	       Grp_DB_CreateGroup (GrpTyp.GrpTypCod,&Grp);

	       /* Write success message */
	       AlertType = Ale_SUCCESS;
	       snprintf (AlertTxt,sizeof (AlertTxt),
			 Txt_Created_new_group_X,Grp.Name);
	       break;
	   }
      else	// If there is not a group name
        {
         AlertType = Ale_ERROR;
	 Str_Copy (AlertTxt,Txt_You_must_specify_the_name,sizeof (AlertTxt) - 1);
        }
     }
   else	// Invalid group type
     {
      GrpTyp.GrpTypCod = -1L;
      AlertType = Ale_ERROR;
      Str_Copy (AlertTxt,"Wrong type of group.",sizeof (AlertTxt) - 1);
     }

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,Ale_INFO,NULL,AlertType,AlertTxt);
  }

/*****************************************************************************/
/********************* Request removing of a group type **********************/
/*****************************************************************************/

void Grp_ReqRemGroupType (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_type_of_group_X;
   struct GroupType GrpTyp;
   struct Group Grp;

   /***** Get the code of the group type *****/
   GrpTyp.GrpTypCod = ParCod_GetAndCheckPar (ParCod_GrpTyp);

   /***** Get data of the group type from database *****/
   Grp_GetGroupTypeDataByCod (&GrpTyp);
   Grp_ResetGroup (&Grp);

   /***** Begin section to edit group types *****/
   Grp_ReqEditGroupsInternal0 ();

   /***** Show question and button to remove type of group *****/
   Ale_ShowAlertRemove (ActRemGrpTyp,Grp_GROUP_TYPES_SECTION_ID,
			Grp_PutParGrpTypCod,&GrpTyp.GrpTypCod,
			Txt_Do_you_really_want_to_remove_the_type_of_group_X,
			GrpTyp.Name);

   /***** Show the form to edit group types and groups again *****/
   Grp_ReqEditGroupsInternal1 (&GrpTyp,Ale_INFO,NULL);
   Grp_ReqEditGroupsInternal2 (&GrpTyp,&Grp,Ale_INFO,NULL);
  }

/*****************************************************************************/
/************************* Request removal of a group ************************/
/*****************************************************************************/

void Grp_ReqRemGroup (void)
  {
   long GrpCod;

   /***** Get group code *****/
   GrpCod = ParCod_GetAndCheckPar (ParCod_Grp);

   /***** Confirm removing *****/
   Grp_AskConfirmRemGrp (GrpCod);
  }

/*****************************************************************************/
/******************* Ask for confirmation to remove a group ******************/
/*****************************************************************************/

static void Grp_AskConfirmRemGrp (long GrpCod)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_group_X;
   long CrsCod;
   struct GroupType GrpTyp;
   struct Group Grp;

   /***** Get name of the group from database *****/
   Grp.GrpCod = GrpCod;
   Grp_GetGroupDataByCod (&CrsCod,&GrpTyp.GrpTypCod,&Grp);
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Show the form to edit group types again *****/
   Grp_ReqEditGroupsInternal0 ();
   Grp_ReqEditGroupsInternal1 (&GrpTyp,Ale_INFO,NULL);

   /***** Show question and button to remove group *****/
   Ale_ShowAlertRemove (ActRemGrp,Grp_GROUPS_SECTION_ID,
			Grp_PutParGrpCod,&Grp.GrpCod,
			Txt_Do_you_really_want_to_remove_the_group_X,Grp.Name);

   /***** Show the form to edit groups again *****/
   Grp_ReqEditGroupsInternal2 (&GrpTyp,&Grp,Ale_INFO,NULL);
  }

/*****************************************************************************/
/**************************** Remove a group type ****************************/
/*****************************************************************************/

void Grp_RemoveGroupType (void)
  {
   extern const char *Txt_Type_of_group_X_removed;
   struct GroupType GrpTyp;
   struct Group Grp;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_TYPE_NAME];

   /***** Get param with code of group type *****/
   GrpTyp.GrpTypCod = ParCod_GetAndCheckPar (ParCod_GrpTyp);

   /***** Get data of type of group from database *****/
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Remove group type and its groups *****/
   Grp_RemoveGroupTypeCompletely (GrpTyp.GrpTypCod);

   /***** Create message to show the change made *****/
   snprintf (AlertTxt,sizeof (AlertTxt),Txt_Type_of_group_X_removed,GrpTyp.Name);

   /***** Show the form again *****/
   Grp_ResetGroup (&Grp);
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,Ale_SUCCESS,AlertTxt,Ale_INFO,NULL);
  }

/*****************************************************************************/
/******************************* Remove a group ******************************/
/*****************************************************************************/

void Grp_RemoveGroup (void)
  {
   extern const char *Txt_Group_X_removed;
   long CrsCod;
   struct GroupType GrpTyp;
   struct Group Grp;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get param with group code *****/
   Grp.GrpCod = ParCod_GetAndCheckPar (ParCod_Grp);

   /***** Get name and type of the group from database *****/
   Grp_GetGroupDataByCod (&CrsCod,&GrpTyp.GrpTypCod,&Grp);
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Remove group *****/
   Grp_RemoveGroupCompletely (Grp.GrpCod);

   /***** Create message to show the change made *****/
   snprintf (AlertTxt,sizeof (AlertTxt),Txt_Group_X_removed,Grp.Name);

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,Ale_INFO,NULL,Ale_SUCCESS,AlertTxt);
  }

/*****************************************************************************/
/********************* Remove a group type from database *********************/
/*****************************************************************************/

static void Grp_RemoveGroupTypeCompletely (long GrpTypCod)
  {
   /***** Remove file zones of all groups of this type *****/
   Brw_RemoveZonesOfGroupsOfType (GrpTypCod);

   /***** Remove the associations of assignments to groups of this type *****/
   Asg_DB_RemoveGroupsOfType (GrpTypCod);

   /***** Remove the associations of attendance events to groups of this type *****/
   Att_DB_RemoveGroupsOfType (GrpTypCod);

   /***** Remove the associations of exam sessions to groups of this type *****/
   Exa_DB_RemoveAllGrpsOfType (GrpTypCod);

   /***** Remove the associations of matches to groups of this type *****/
   Mch_DB_RemoveGroupsOfType (GrpTypCod);

   /***** Remove the associations of surveys to groups of this type *****/
   Svy_DB_RemoveGroupsOfType (GrpTypCod);

   /***** Orphan all groups of this type in course timetable *****/
   Tmt_DB_OrphanAllGrpsOfATypeInCrsTimeTable (GrpTypCod);

   /***** Remove all users from groups of this type *****/
   Grp_DB_RemoveUsrsFromGrpsOfType (GrpTypCod);

   /***** Remove all groups of this type *****/
   Grp_DB_RemoveGrpsOfType (GrpTypCod);

   /***** Remove the group type *****/
   Grp_DB_RemoveGrpType (GrpTypCod);
  }

/*****************************************************************************/
/******* Remove a group from data base and remove group common zone **********/
/*****************************************************************************/

static void Grp_RemoveGroupCompletely (long GrpCod)
  {
   /***** Remove file zones of this group *****/
   Brw_RemoveGrpZones (Gbl.Hierarchy.Node[Hie_CRS].HieCod,GrpCod);

   /***** Remove this group from all assignments *****/
   Asg_DB_RemoveGroup (GrpCod);

   /***** Remove this group from all attendance events *****/
   Att_DB_RemoveGroup (GrpCod);

   /***** Remove this group from all matches *****/
   Mch_DB_RemoveGroup (GrpCod);

   /***** Remove this group from all exam sessions *****/
   Exa_DB_RemoveGroup (GrpCod);

   /***** Remove this group from all surveys *****/
   Svy_DB_RemoveGroup (GrpCod);

   /***** Orphan this group in course timetable *****/
   Tmt_DB_OrphanGrpInCrsTimeTable (GrpCod);

   /***** Remove all users in this group *****/
   Grp_DB_RemoveUsrsFromGrp (GrpCod);

   /***** Remove the group *****/
   Grp_DB_RemoveGrp (GrpCod);
  }

/*****************************************************************************/
/******************************* Open a group ********************************/
/*****************************************************************************/

void Grp_OpenGroup (void)
  {
   extern const char *Txt_The_group_X_is_now_open;
   long CrsCod;
   struct GroupType GrpTyp;
   struct Group Grp;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get param with group code *****/
   Grp.GrpCod = ParCod_GetAndCheckPar (ParCod_Grp);

   /***** Get name and type of the group from database *****/
   Grp_GetGroupDataByCod (&CrsCod,&GrpTyp.GrpTypCod,&Grp);
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Update the table of groups changing open/close status *****/
   Grp_DB_OpenGrp (Grp.GrpCod);

   /***** Create message to show the change made *****/
   snprintf (AlertTxt,sizeof (AlertTxt),Txt_The_group_X_is_now_open,Grp.Name);

   /***** Show the form again *****/
   Grp.Open = CloOpe_OPEN;
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,Ale_INFO,NULL,Ale_SUCCESS,AlertTxt);
  }

/*****************************************************************************/
/******************************* Close a group *******************************/
/*****************************************************************************/

void Grp_CloseGroup (void)
  {
   extern const char *Txt_The_group_X_is_now_closed;
   long CrsCod;
   struct GroupType GrpTyp;
   struct Group Grp;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get param with group code *****/
   Grp.GrpCod = ParCod_GetAndCheckPar (ParCod_Grp);

   /***** Get name and type of the group from database *****/
   Grp_GetGroupDataByCod (&CrsCod,&GrpTyp.GrpTypCod,&Grp);
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Update the table of groups changing open/close status *****/
   Grp_DB_CloseGrp (Grp.GrpCod);

   /***** Create message to show the change made *****/
   snprintf (AlertTxt,sizeof (AlertTxt),Txt_The_group_X_is_now_closed,Grp.Name);

   /***** Show the form again *****/
   Grp.Open = CloOpe_CLOSED;
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,Ale_INFO,NULL,Ale_SUCCESS,AlertTxt);
  }

/*****************************************************************************/
/************************ Enable file zones of a group ***********************/
/*****************************************************************************/

void Grp_EnableFileZonesGrp (void)
  {
   extern const char *Txt_File_zones_of_the_group_X_are_now_enabled;
   long CrsCod;
   struct GroupType GrpTyp;
   struct Group Grp;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get param with group code *****/
   Grp.GrpCod = ParCod_GetAndCheckPar (ParCod_Grp);

   /***** Get name and type of the group from database *****/
   Grp_GetGroupDataByCod (&CrsCod,&GrpTyp.GrpTypCod,&Grp);
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Update the table of groups changing file zones status *****/
   Grp_DB_EnableFileZonesGrp (Grp.GrpCod);

   /***** Create message to show the change made *****/
   snprintf (AlertTxt,sizeof (AlertTxt),
             Txt_File_zones_of_the_group_X_are_now_enabled,Grp.Name);

   /***** Show the form again *****/
   Grp.HasFileZones = Grp_HAS_FILEZONES;
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,Ale_INFO,NULL,Ale_SUCCESS,AlertTxt);
  }

/*****************************************************************************/
/*********************** Disable file zones of a group ***********************/
/*****************************************************************************/

void Grp_DisableFileZonesGrp (void)
  {
   extern const char *Txt_File_zones_of_the_group_X_are_now_disabled;
   long CrsCod;
   struct GroupType GrpTyp;
   struct Group Grp;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get param with group code *****/
   Grp.GrpCod = ParCod_GetAndCheckPar (ParCod_Grp);

   /***** Get name and type of the group from database *****/
   Grp_GetGroupDataByCod (&CrsCod,&GrpTyp.GrpTypCod,&Grp);
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Update the table of groups changing file zones status *****/
   Grp_DB_DisableFileZonesGrp (Grp.GrpCod);

   /***** Create message to show the change made *****/
   snprintf (AlertTxt,sizeof (AlertTxt),
	     Txt_File_zones_of_the_group_X_are_now_disabled,Grp.Name);

   /***** Show the form again *****/
   Grp.HasFileZones = Grp_HAS_NOT_FILEZONES;
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,Ale_INFO,NULL,Ale_SUCCESS,AlertTxt);
  }

/*****************************************************************************/
/*********************** Change the group type of a group ********************/
/*****************************************************************************/

void Grp_ChangeGroupType (void)
  {
   extern const char *Txt_The_group_X_already_exists;
   extern const char *Txt_The_type_of_group_of_the_group_X_has_changed;
   long CrsCod;
   struct GroupType GrpTyp;
   struct Group Grp;
   long NewGrpTypCod;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get parameters from form *****/
   /* Get group code */
   Grp.GrpCod = ParCod_GetAndCheckPar (ParCod_Grp);

   /* Get the new group type */
   NewGrpTypCod = ParCod_GetAndCheckPar (ParCod_GrpTyp);

   /* Get from the database the type and the name of the group */
   Grp_GetGroupDataByCod (&CrsCod,&GrpTyp.GrpTypCod,&Grp);
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** If group was in database... *****/
   switch (Grp_DB_CheckIfGrpNameExistsForGrpTyp (NewGrpTypCod,Grp.Name,-1L))
     {
      case Exi_EXISTS:
	 /* Create warning message */
	 AlertType = Ale_WARNING;
	 snprintf (AlertTxt,sizeof (AlertTxt),
		   Txt_The_group_X_already_exists,Grp.Name);
	 break;
      case Exi_DOES_NOT_EXIST:	// Group is not in database
      default:
	 /* Update the table of groups changing old type by new type */
	 GrpTyp.GrpTypCod = NewGrpTypCod;
	 Grp_DB_ChangeGrpTypOfGrp (Grp.GrpCod,NewGrpTypCod);

	 /* Create message to show the change made */
	 AlertType = Ale_SUCCESS;
	 snprintf (AlertTxt,sizeof (AlertTxt),
		   Txt_The_type_of_group_of_the_group_X_has_changed,Grp.Name);
	 break;
     }

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,Ale_INFO,NULL,AlertType,AlertTxt);
  }

/*****************************************************************************/
/************************* Change the room of a group ************************/
/*****************************************************************************/

void Grp_ChangeGroupRoom (void)
  {
   extern const char *Txt_The_room_assigned_to_the_group_X_has_changed;
   long CrsCod;
   struct GroupType GrpTyp;
   struct Group Grp;
   long NewRooCod;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get parameters from form *****/
   /* Get group code */
   Grp.GrpCod = ParCod_GetAndCheckPar (ParCod_Grp);

   /* Get the new room */
   NewRooCod = ParCod_GetPar (ParCod_Roo);

   /* Get from the database the name of the group */
   Grp_GetGroupDataByCod (&CrsCod,&GrpTyp.GrpTypCod,&Grp);
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Update the table of groups changing old room by new room *****/
   Grp_DB_ChangeRoomOfGrp (Grp.GrpCod,NewRooCod);

   /* Create message to show the change made */
   AlertType = Ale_SUCCESS;
   snprintf (AlertTxt,sizeof (AlertTxt),
	     Txt_The_room_assigned_to_the_group_X_has_changed,Grp.Name);

   /***** Show the form again *****/
   Grp.Room.RooCod = NewRooCod;
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,Ale_INFO,NULL,AlertType,AlertTxt);
  }

/*****************************************************************************/
/************** Change mandatory enrolment to a group of a type **************/
/*****************************************************************************/

void Grp_ChangeMandatGrpTyp (void)
  {
   extern const char *Txt_The_type_of_enrolment_of_the_type_of_group_X_has_not_changed;
   extern const char *Txt_The_enrolment_of_students_into_groups_of_type_X_is_now_mandatory;
   extern const char *Txt_The_enrolment_of_students_into_groups_of_type_X_is_now_optional;
   struct GroupType GrpTyp;
   struct Group Grp;
   Grp_OptionalOrMandatory_t NewOptionalMandatory;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_TYPE_NAME];

   /***** Get parameters of the form *****/
   /* Get the code of type of group */
   GrpTyp.GrpTypCod = ParCod_GetAndCheckPar (ParCod_GrpTyp);

   /* Get the new type of enrolment (mandatory or optional) of this type of group */
   NewOptionalMandatory = Par_GetParBool ("OptionalMandatory") ? Grp_MANDATORY :
								 Grp_OPTIONAL;

   /* Get from the database the name of the type and the old type of enrolment */
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Check if the old type of enrolment match the new
          (this happens when return is pressed without changes) *****/
   if (GrpTyp.Enrolment.OptionalMandatory == NewOptionalMandatory)
     {
      AlertType = Ale_INFO;
      snprintf (AlertTxt,sizeof (AlertTxt),
	        Txt_The_type_of_enrolment_of_the_type_of_group_X_has_not_changed,
                GrpTyp.Name);
     }
   else
     {
      /***** Update of the table of types of group
             changing the old type of enrolment by the new *****/
      GrpTyp.Enrolment.OptionalMandatory = NewOptionalMandatory;
      Grp_DB_ChangeOptionalMandatory (&GrpTyp);

      /***** Write message to show the change made *****/
      AlertType = Ale_SUCCESS;
      snprintf (AlertTxt,sizeof (AlertTxt),
	        NewOptionalMandatory == Grp_MANDATORY ? Txt_The_enrolment_of_students_into_groups_of_type_X_is_now_mandatory :
							Txt_The_enrolment_of_students_into_groups_of_type_X_is_now_optional,
                GrpTyp.Name);
     }

   /***** Show the form again *****/
   Grp_ResetGroup (&Grp);
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,AlertType,AlertTxt,Ale_INFO,NULL);
  }

/*****************************************************************************/
/******** Change multiple enrolment to one or more groups of a type *********/
/*****************************************************************************/

void Grp_ChangeMultiGrpTyp (void)
  {
   extern const char *Txt_The_type_of_enrolment_of_the_type_of_group_X_has_not_changed;
   extern const char *Txt_Now_each_student_can_belong_to_multiple_groups_of_type_X;
   extern const char *Txt_Now_each_student_can_only_belong_to_a_group_of_type_X;
   struct GroupType GrpTyp;
   struct Group Grp;
   Grp_SingleOrMultiple_t NewSingleMultiple;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_TYPE_NAME];

   /***** Get parameters from the form *****/
   /* Get the code of type of group */
   GrpTyp.GrpTypCod = ParCod_GetAndCheckPar (ParCod_GrpTyp);

   /* Get the new type of enrolment (single or multiple) of this type of group */
   NewSingleMultiple = Par_GetParBool ("SingleMultiple") ? Grp_MULTIPLE :
							   Grp_SINGLE;;

   /* Get from the database the name of the type and the old type of enrolment */
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Check if the old type of enrolment match the new one
   	  (this happends when return is pressed without changes) *****/
   if (GrpTyp.Enrolment.SingleMultiple == NewSingleMultiple)
     {
      AlertType = Ale_INFO;
      snprintf (AlertTxt,sizeof (AlertTxt),
	        Txt_The_type_of_enrolment_of_the_type_of_group_X_has_not_changed,
                GrpTyp.Name);
     }
   else
     {
      /***** Update of the table of types of group changing the old type of enrolment by the new *****/
      GrpTyp.Enrolment.SingleMultiple = NewSingleMultiple;
      Grp_DB_ChangeSingleMultiple (&GrpTyp);

      /***** Write message to show the change made *****/
      AlertType = Ale_SUCCESS;
      snprintf (AlertTxt,sizeof (AlertTxt),
	        NewSingleMultiple == Grp_MULTIPLE ? Txt_Now_each_student_can_belong_to_multiple_groups_of_type_X :
						    Txt_Now_each_student_can_only_belong_to_a_group_of_type_X,
                GrpTyp.Name);
     }

   /***** Show the form again *****/
   Grp_ResetGroup (&Grp);
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,AlertType,AlertTxt,Ale_INFO,NULL);
  }

/*****************************************************************************/
/****************** Change open time for a type of group *********************/
/*****************************************************************************/

void Grp_ChangeOpenTimeGrpTyp (void)
  {
   extern const char *Txt_The_date_time_of_opening_of_groups_has_changed;
   struct GroupType GrpTyp;
   struct Group Grp;

   /***** Get the code of type of group *****/
   GrpTyp.GrpTypCod = ParCod_GetAndCheckPar (ParCod_GrpTyp);

   /***** Get from the database the data of this type of group *****/
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Get open time *****/
   GrpTyp.OpenTimeUTC = Dat_GetTimeUTCFromForm (Dat_STR_TIME);
   GrpTyp.MustBeOpened = Grp_CheckIfOpenTimeInTheFuture (GrpTyp.OpenTimeUTC) ? Grp_MUST_BE_OPENED :
									       Grp_MUST_NOT_BE_OPENED;

   /***** Update the table of types of group
          changing the old opening time of enrolment by the new *****/
   Grp_DB_ChangeOpeningTime (&GrpTyp);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_date_time_of_opening_of_groups_has_changed);

   /***** Show the form again *****/
   Grp_ResetGroup (&Grp);
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,Ale_SUCCESS,NULL,Ale_INFO,NULL);
  }

/*****************************************************************************/
/***************** Change maximum of students in a group *********************/
/*****************************************************************************/

void Grp_ChangeMaxStdsGrp (void)
  {
   extern const char *Txt_The_maximum_number_of_students_in_group_X_has_not_changed;
   extern const char *Txt_The_group_X_does_not_have_a_student_limit_now;
   extern const char *Txt_The_maximum_number_of_students_in_group_X_is_now_Y;
   long CrsCod;
   struct GroupType GrpTyp;
   struct Group Grp;
   unsigned NewMaxStds;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get parameters of the form *****/
   /* Get group code */
   Grp.GrpCod = ParCod_GetAndCheckPar (ParCod_Grp);

   /* Get the new maximum number of students of the group */
   NewMaxStds = (unsigned)
	        Par_GetParUnsignedLong ("MaxStudents",
                                        0,
                                        Grp_MAX_STUDENTS_IN_A_GROUP,
                                        Grp_NUM_STUDENTS_NOT_LIMITED);

   /* Get from the database the type, name, and old maximum of students of the group */
   Grp_GetGroupDataByCod (&CrsCod,&GrpTyp.GrpTypCod,&Grp);
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Check if the old maximum of students equals the new one
             (this happens when return is pressed without changes) *****/
   if (Grp.MaxStds == NewMaxStds)
     {
      AlertType = Ale_INFO;
      snprintf (AlertTxt,sizeof (AlertTxt),
		Txt_The_maximum_number_of_students_in_group_X_has_not_changed,Grp.Name);
     }
   else
     {
      /***** Update the table of groups changing the old maximum of students to the new *****/
      Grp_DB_ChangeMaxStdsOfGrp (Grp.GrpCod,NewMaxStds);

      /***** Write message to show the change made *****/
      AlertType = Ale_SUCCESS;
      if (NewMaxStds > Grp_MAX_STUDENTS_IN_A_GROUP)
         snprintf (AlertTxt,sizeof (AlertTxt),
	           Txt_The_group_X_does_not_have_a_student_limit_now,Grp.Name);
      else
         snprintf (AlertTxt,sizeof (AlertTxt),
	           Txt_The_maximum_number_of_students_in_group_X_is_now_Y,
	           Grp.Name,NewMaxStds);
     }

   /***** Show the form again *****/
   Grp.MaxStds = NewMaxStds;
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,Ale_INFO,NULL,AlertType,AlertTxt);
  }

/*****************************************************************************/
/************* Write the maximum number of students in a group ***************/
/*****************************************************************************/

static void Grp_WriteMaxStds (char Str[Cns_MAX_DIGITS_UINT + 1],unsigned MaxStudents)
  {
   if (MaxStudents <= Grp_MAX_STUDENTS_IN_A_GROUP)
      snprintf (Str,Cns_MAX_DIGITS_UINT + 1,"%u",MaxStudents);
   else
      Str[0] = '\0';
  }

/*****************************************************************************/
/********* Convert string to maximum number of students in a group ***********/
/*****************************************************************************/

unsigned Grp_ConvertToNumMaxStdsGrp (const char *StrMaxStudents)
  {
   unsigned MaxStudents;

   if (sscanf (StrMaxStudents,"%u",&MaxStudents) != 1)
      return Grp_NUM_STUDENTS_NOT_LIMITED;
   else if (MaxStudents > Grp_MAX_STUDENTS_IN_A_GROUP)
      return Grp_NUM_STUDENTS_NOT_LIMITED;
   return MaxStudents;
  }

/*****************************************************************************/
/***************************** Rename a group type ***************************/
/*****************************************************************************/

void Grp_RenameGroupType (void)
  {
   extern const char *Txt_You_can_not_leave_the_field_empty;
   extern const char *Txt_The_type_of_group_X_already_exists;
   extern const char *Txt_The_type_of_group_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   struct GroupType GrpTyp;
   struct Group Grp;
   char NewGrpTypName[Grp_MAX_BYTES_GROUP_TYPE_NAME + 1];
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_TYPE_NAME];

   /***** Get parameters from form *****/
   /* Get the code of the group type */
   GrpTyp.GrpTypCod = ParCod_GetAndCheckPar (ParCod_GrpTyp);

   /* Get the new name for the group type */
   Par_GetParText ("GrpTypName",NewGrpTypName,Grp_MAX_BYTES_GROUP_TYPE_NAME);

   /***** Get from the database the old name of the group type *****/
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Check if new name is empty *****/
   if (NewGrpTypName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (GrpTyp.Name,NewGrpTypName))	// Different names
         /***** If group type was in database... *****/
         switch (Grp_DB_CheckIfGrpTypNameExistsInCurrentCrs (NewGrpTypName,
							     GrpTyp.GrpTypCod))
	   {
	    case Exi_EXISTS:
	       AlertType = Ale_WARNING;
	       snprintf (AlertTxt,sizeof (AlertTxt),
			 Txt_The_type_of_group_X_already_exists,NewGrpTypName);
	       break;
	    case Exi_DOES_NOT_EXIST:
	    default:
	       /***** Update the table changing old name by new name *****/
	       Str_Copy (GrpTyp.Name,NewGrpTypName,sizeof (GrpTyp.Name) - 1);
	       Grp_DB_RenameGrpTyp (&GrpTyp);

	       /***** Write message to show the change made *****/
	       AlertType = Ale_SUCCESS;
	       snprintf (AlertTxt,sizeof (AlertTxt),
			 Txt_The_type_of_group_X_has_been_renamed_as_Y,
			 GrpTyp.Name,NewGrpTypName);
	       break;
	   }
      else	// The same name
        {
	 AlertType = Ale_INFO;
         snprintf (AlertTxt,sizeof (AlertTxt),
	           Txt_The_name_X_has_not_changed,NewGrpTypName);
        }
     }
   else
     {
      AlertType = Ale_WARNING;
      Str_Copy (AlertTxt,Txt_You_can_not_leave_the_field_empty,
		sizeof (AlertTxt) - 1);
     }

   /***** Show the form again *****/
   Grp_ResetGroup (&Grp);
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,AlertType,AlertTxt,Ale_INFO,NULL);
  }

/*****************************************************************************/
/******************************* Rename a group ******************************/
/*****************************************************************************/

void Grp_RenameGroup (void)
  {
   extern const char *Txt_You_can_not_leave_the_field_empty;
   extern const char *Txt_The_group_X_already_exists;
   extern const char *Txt_The_group_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   long CrsCod;
   struct GroupType GrpTyp;
   struct Group Grp;
   char NewGrpName[Grp_MAX_BYTES_GROUP_NAME + 1];
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get parameters from form *****/
   /* Get the code of the group */
   Grp.GrpCod = ParCod_GetAndCheckPar (ParCod_Grp);

   /* Get the new name for the group */
   Par_GetParText ("GrpName",NewGrpName,Grp_MAX_BYTES_GROUP_NAME);

   /***** Get from the database the type and the old name of the group *****/
   Grp_GetGroupDataByCod (&CrsCod,&GrpTyp.GrpTypCod,&Grp);
   Grp_GetGroupTypeDataByCod (&GrpTyp);

   /***** Check if new name is empty *****/
   if (!NewGrpName[0])
     {
      AlertType = Ale_WARNING;
      Str_Copy (AlertTxt,Txt_You_can_not_leave_the_field_empty,
		sizeof (AlertTxt) - 1);
     }
   else
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (Grp.Name,NewGrpName))	// Different names
         /***** If group was in database... *****/
         switch (Grp_DB_CheckIfGrpNameExistsForGrpTyp (GrpTyp.GrpTypCod,NewGrpName,
						       Grp.GrpCod))
	   {
	    case Exi_EXISTS:
	       AlertType = Ale_WARNING;
	       snprintf (AlertTxt,sizeof (AlertTxt),
			 Txt_The_group_X_already_exists,NewGrpName);
	       break;
	    case Exi_DOES_NOT_EXIST:
	    default:
	       /***** Update the table changing old name by new name *****/
	       Grp_DB_RenameGrp (Grp.GrpCod,NewGrpName);

	       /***** Write message to show the change made *****/
	       AlertType = Ale_SUCCESS;
	       snprintf (AlertTxt,sizeof (AlertTxt),
			 Txt_The_group_X_has_been_renamed_as_Y,Grp.Name,NewGrpName);
	       break;
	   }
      else	// The same name
        {
	 AlertType = Ale_INFO;
         snprintf (AlertTxt,sizeof (AlertTxt),
	           Txt_The_name_X_has_not_changed,NewGrpName);
        }
     }

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (&GrpTyp,&Grp,Ale_INFO,NULL,AlertType,AlertTxt);
  }

/*****************************************************************************/
/****************** Write parameter with code of group type ******************/
/*****************************************************************************/

static void Grp_PutParGrpTypCod (void *GrpTypCod)
  {
   if (GrpTypCod)
      ParCod_PutPar (ParCod_GrpTyp,*((long *) GrpTypCod));
  }

/*****************************************************************************/
/********************* Write parameter with code of group ********************/
/*****************************************************************************/

static void Grp_PutParGrpCod (void *GrpCod)
  {
   if (GrpCod)
      ParCod_PutPar (ParCod_Grp,*((long *) GrpCod));
  }

/*****************************************************************************/
/************************ Get list of group codes selected *******************/
/*****************************************************************************/

void Grp_GetLstCodsGrpWanted (struct ListCodGrps *LstGrpsWanted)
  {
   unsigned NumGrpTyp;
   char Par[6 + Cns_MAX_DIGITS_LONG + 1];
   char LongStr[1 + Cns_MAX_DIGITS_LONG + 1];
   char **LstStrCodGrps;
   const char *Ptr;
   unsigned NumGrpWanted;
   struct GroupType *GrpTyp;

   /***** Allocate memory for the strings with group codes in each type *****/
   if ((LstStrCodGrps = calloc (Gbl.Crs.Grps.GrpTypes.NumGrpTypes,
                                sizeof (*LstStrCodGrps))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Get lists with the groups that I want in each type
          in order to count the total number of groups selected *****/
   for (NumGrpTyp = 0, LstGrpsWanted->NumGrps = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	NumGrpTyp++)
     {
      GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

      /***** Allocate memory for the list of group codes of this type *****/
      if ((LstStrCodGrps[NumGrpTyp] = malloc (GrpTyp->NumGrps *
                                              (Cns_MAX_DIGITS_LONG + 1))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the multiple parameter code of group of this type *****/
      snprintf (Par,sizeof (Par),"GrpCod%ld",
                GrpTyp->GrpTypCod);
      Par_GetParMultiToText (Par,LstStrCodGrps[NumGrpTyp],
                             ((Cns_MAX_DIGITS_LONG + 1) * GrpTyp->NumGrps) - 1);
      if (LstStrCodGrps[NumGrpTyp][0])
        {
         /***** Count the number of groups selected of this type of LstCodGrps[NumGrpTyp] *****/
         for (Ptr = LstStrCodGrps[NumGrpTyp], NumGrpWanted = 0;
              *Ptr;
              NumGrpWanted++)
            Par_GetNextStrUntilSeparParMult (&Ptr,LongStr,Cns_MAX_DIGITS_LONG);

         /***** Add the number of groups selected of this type to the number of groups selected total *****/
         LstGrpsWanted->NumGrps += NumGrpWanted;
        }
      }

   /***** Create the list (argument passed to this function)
          with all groups selected (of all the types) *****/
   if (LstGrpsWanted->NumGrps)
     {
      if ((LstGrpsWanted->GrpCods = calloc (LstGrpsWanted->NumGrps,
                                            sizeof (*LstGrpsWanted->GrpCods))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the groups *****/
      for (NumGrpTyp = 0, NumGrpWanted = 0;
	   NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
	   NumGrpTyp++)
        {
	 /* Open groups of this type selected by user */
         for (Ptr = LstStrCodGrps[NumGrpTyp];
              *Ptr;
              NumGrpWanted++)
           {
            Par_GetNextStrUntilSeparParMult (&Ptr,LongStr,Cns_MAX_DIGITS_LONG);
            LstGrpsWanted->GrpCods[NumGrpWanted] = Str_ConvertStrCodToLongCod (LongStr);
           }

         /* Free memory used by the list of group codes of this type */
         free (LstStrCodGrps[NumGrpTyp]);
        }
     }

   /***** Free memory used by the lists of group codes of each type *****/
   free (LstStrCodGrps);
  }

/*****************************************************************************/
/************************** Free list of group codes *************************/
/*****************************************************************************/

void Grp_FreeListCodGrp (struct ListCodGrps *LstGrps)
  {
   if (LstGrps->NumGrps && LstGrps->GrpCods)
      free (LstGrps->GrpCods);
   LstGrps->GrpCods = NULL;
   LstGrps->NumGrps = 0;
  }

/*****************************************************************************/
/*********** Put parameter that indicates all groups selected ****************/
/*****************************************************************************/

void Grp_PutParAllGroups (void)
  {
   Par_PutParChar ("AllGroups",'Y');
  }

/*****************************************************************************/
/************* Parameter to show only my groups or all groups ****************/
/*****************************************************************************/

void Grp_PutParMyAllGrps (void *MyAllGrps)
  {
   if (MyAllGrps)
      Par_PutParUnsigned (NULL,"WhichGrps",
			  (unsigned) *((Grp_MyAllGrps_t *) MyAllGrps));
  }

void Grp_PutParWhichGrpsOnlyMyGrps (void)
  {
   Par_PutParUnsigned (NULL,"WhichGrps",(unsigned) Grp_MY_GROUPS);
  }

void Grp_PutParWhichGrpsAllGrps (void)
  {
   Par_PutParUnsigned (NULL,"WhichGrps",(unsigned) Grp_ALL_GROUPS);
  }

/*****************************************************************************/
/***** Show form to choice whether to show only my groups or all groups ******/
/*****************************************************************************/

void Grp_ShowFormToSelMyAllGrps (Act_Action_t Action,
                                 void (*FuncPars) (void *Args),void *Args)
  {
   extern const char *Txt_GROUP_WHICH_GROUPS[2];
   Grp_MyAllGrps_t MyAllGrps;

   Set_BeginOneSettingSelector ();
      for (MyAllGrps  = Grp_MY_GROUPS;
	   MyAllGrps <= Grp_ALL_GROUPS;
	   MyAllGrps++)
	{
	 Set_BeginPref (MyAllGrps == Gbl.Crs.Grps.MyAllGrps);
	    Frm_BeginForm (Action);
	       Par_PutParUnsigned (NULL,"WhichGrps",(unsigned) MyAllGrps);
	       if (FuncPars)	// Extra parameters depending on the action
		  FuncPars (Args);
	       Ico_PutSettingIconLink (MyAllGrps == Grp_MY_GROUPS ? "mysitemap.png" :
								    "sitemap.svg",
				       Ico_BLACK,Txt_GROUP_WHICH_GROUPS[MyAllGrps]);
	    Frm_EndForm ();
	 Set_EndPref ();
	}
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/************* Get whether to show only my groups or all groups **************/
/*****************************************************************************/

Grp_MyAllGrps_t Grp_GetParMyAllGrps (void)
  {
   static bool AlreadyGot = false;
   Grp_MyAllGrps_t MyAllGrpsDefault;

   if (!AlreadyGot)
     {
      /***** Get which groups (my groups or all groups) *****/
      /* Set default */
      switch (Gbl.Action.Act)
	{
	 case ActSeeCrsTT:	// Show course timetable
	 case ActPrnCrsTT:	// Print course timetable
	 case ActChgCrsTT1stDay:// Change first day of week in course timetable
	 case ActReqLnkCrsTT:	// Get link to resource course timetable
	 case ActSeeAllAsg:	// List assignments
	 case ActSeeAllExa:	// List exams
	 case ActSeeAllGam:	// List games
	 case ActSeeAllAtt:	// List attendance
	 case ActSeeAllSvy:	// List surveys
	    /*
	    If I belong       to this course ==> see only my groups
	    If I don't belong to this course ==> see all groups
	    */
	    MyAllGrpsDefault = Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] == Usr_BELONG ? Grp_MY_GROUPS :
										     Grp_ALL_GROUPS;
	    break;
	 case ActSeeMyTT:	// Show my timetable
	 case ActPrnMyTT:	// Print my timetable
	 case ActChgMyTT1stDay:	// Change first day of week in my timetable
	    /* By default, show only my groups */
	    MyAllGrpsDefault = Grp_MY_GROUPS;
	    break;
	 default:			// Control never should enter here
	    MyAllGrpsDefault = Grp_MY_ALL_GROUPS_DEFAULT;
	    break;
	}

      /* Get parameter */
      Gbl.Crs.Grps.MyAllGrps = (Grp_MyAllGrps_t)
	                       Par_GetParUnsignedLong ("WhichGrps",
	                                               0,
	                                               Grp_NUM_MY_ALL_GROUPS - 1,
	                                               (unsigned long) MyAllGrpsDefault);

      AlreadyGot = true;
     }

   return Gbl.Crs.Grps.MyAllGrps;
  }

/*****************************************************************************/
/************************** Remove groups in a course ************************/
/*****************************************************************************/

void Grp_DB_RemoveCrsGrps (long HieCod)
  {
   /***** Remove all users in groups in the course *****/
   Grp_DB_RemoveUsrsFromGrpsOfCrs (HieCod);

   /***** Remove all groups in the course *****/
   Grp_DB_RemoveGrpsInCrs (HieCod);

   /***** Remove all group types in the course *****/
   Grp_DB_RemoveGrpTypesInCrs (HieCod);
  }
