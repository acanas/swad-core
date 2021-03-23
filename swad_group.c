// swad_group.c: types of groups and groups

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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
#include "swad_attendance.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_exam_session.h"
#include "swad_form.h"
#include "swad_game.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_HTML.h"
#include "swad_match.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_program.h"
#include "swad_project.h"
#include "swad_setting.h"
#include "swad_survey.h"

/*****************************************************************************/
/*************************** Private constants *******************************/
/*****************************************************************************/

#define Grp_GROUP_TYPES_SECTION_ID	"grp_types"
#define Grp_NEW_GROUP_TYPE_SECTION_ID	"new_grp_type"
#define Grp_GROUPS_SECTION_ID		"grps"
#define Grp_NEW_GROUP_SECTION_ID	"new_grp"

static const bool Grp_ICanChangeGrps[Rol_NUM_ROLES] =
  {
   [Rol_UNK    ] = false,
   [Rol_GST    ] = false,
   [Rol_USR    ] = false,
   [Rol_STD    ] = true,
   [Rol_NET    ] = false,
   [Rol_TCH    ] = true,
   [Rol_DEG_ADM] = false,
   [Rol_CTR_ADM] = false,
   [Rol_INS_ADM] = false,
   [Rol_SYS_ADM] = true,
  };

/*****************************************************************************/
/****************************** Private types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/************* External global variables from others modules *****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************** Private global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Grp_ReqEditGroupsInternal (Ale_AlertType_t AlertTypeGroupTypes,const char *AlertTextGroupTypes,
                                       Ale_AlertType_t AlertTypeGroups,const char *AlertTextGroups);
static void Grp_ReqEditGroupsInternal0 (void);
static void Grp_ReqEditGroupsInternal1 (Ale_AlertType_t AlertTypeGroupTypes,
                                        const char *AlertTextGroupTypes);
static void Grp_ReqEditGroupsInternal2 (Ale_AlertType_t AlertTypeGroups,
                                        const char *AlertTextGroups);

static void Grp_EditGroupTypes (void);
static void Grp_EditGroups (const struct Roo_Rooms *Rooms);
static void Grp_PutIconsEditingGroups (__attribute__((unused)) void *Args);
static void Grp_PutIconToCreateNewGroup (void);

static void Grp_PutCheckboxAllGrps (Grp_WhichGroups_t GroupsSelectableByStdsOrNETs);

static void Grp_LockTables (void);
static void Grp_UnlockTables (void);

static void Grp_ConstructorListGrpAlreadySelec (struct ListGrpsAlreadySelec **AlreadyExistsGroupOfType);
static void Grp_DestructorListGrpAlreadySelec (struct ListGrpsAlreadySelec **AlreadyExistsGroupOfType);
static void Grp_RemoveUsrFromGroup (long UsrCod,long GrpCod);
static void Grp_AddUsrToGroup (struct UsrData *UsrDat,long GrpCod);

static void Grp_ListGroupTypesForEdition (void);
static void Grp_PutIconsEditingGroupTypes (__attribute__((unused)) void *Args);
static void Grp_PutIconToViewGroups (void);
static void Grp_PutIconToCreateNewGroupType (void);
static void Grp_WriteHeadingGroupTypes (void);

static void Grp_ListGroupsForEdition (const struct Roo_Rooms *Rooms);
static void Grp_WriteHeadingGroups (void);
static bool Grp_CheckIfAssociatedToGrp (const char *Table,const char *Field,
                                        long Cod,long GrpCod);
static void Grp_PutIconToEditGroups (__attribute__((unused)) void *Args);

static void Grp_ShowWarningToStdsToChangeGrps (void);
static bool Grp_ListGrpsForChangeMySelection (struct GroupType *GrpTyp,
                                              unsigned *NumGrpsThisTypeIBelong);
static void Grp_ListGrpsToAddOrRemUsrs (struct GroupType *GrpTyp,long UsrCod);
static void Grp_ListGrpsForMultipleSelection (struct GroupType *GrpTyp,
                                              Grp_WhichGroups_t GroupsSelectableByStdsOrNETs);
static void Grp_WriteGrpHead (struct GroupType *GrpTyp);
static void Grp_WriteRowGrp (struct Group *Grp,bool Highlight);
static void Grp_PutFormToCreateGroupType (void);
static void Grp_PutFormToCreateGroup (const struct Roo_Rooms *Rooms);
static unsigned Grp_CountNumGrpsInThisCrsOfType (long GrpTypCod);
static void Grp_GetDataOfGroupTypeByCod (struct GroupType *GrpTyp);
static bool Grp_GetMultipleEnrolmentOfAGroupType (long GrpTypCod);
static long Grp_GetTypeOfGroupOfAGroup (long GrpCod);
static unsigned long Grp_CountNumUsrsInNoGrpsOfType (Rol_Role_t Role,long GrpTypCod);
static bool Grp_CheckIfIBelongToGrpsOfType (long GrpTypCod);
static void Grp_GetLstCodGrpsUsrBelongs (long CrsCod,long GrpTypCod,long UsrCod,
                                         struct ListCodGrps *LstGrps);
static bool Grp_CheckIfGrpIsInList (long GrpCod,struct ListCodGrps *LstGrps);
static bool Grp_CheckIfOpenTimeInTheFuture (time_t OpenTimeUTC);
static bool Grp_CheckIfGroupTypeNameExists (const char *GrpTypName,long GrpTypCod);
static bool Grp_CheckIfGroupNameExists (long GrpTypCod,const char *GrpName,long GrpCod);
static void Grp_CreateGroupType (void);
static void Grp_CreateGroup (void);

static void Grp_AskConfirmRemGrpTypWithGrps (unsigned NumGrps);
static void Grp_AskConfirmRemGrp (void);
static void Grp_RemoveGroupTypeCompletely (void);
static void Grp_RemoveGroupCompletely (void);

static void Grp_WriteMaxStds (char Str[Cns_MAX_DECIMAL_DIGITS_UINT + 1],unsigned MaxStudents);
static long Grp_GetParamGrpTypCod (void);
static long Grp_GetParamGrpCod (void);
static void Grp_PutParamGrpTypCod (void *GrpTypCod);

/*****************************************************************************/
/******************* Write the names of the selected groups ******************/
/*****************************************************************************/

void Grp_WriteNamesOfSelectedGrps (void)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_users_with_no_group;
   extern const char *Txt_and;
   long GrpCod;
   unsigned NumGrpSel;
   struct GroupData GrpDat;

   /***** Show the selected groups *****/
   HTM_TxtColonNBSP (Gbl.Crs.Grps.LstGrpsSel.NumGrps == 1 ? Txt_Group  :
                                                            Txt_Groups);
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
     {
      if ((GrpCod = Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]) >= 0)
        {
         GrpDat.GrpCod = GrpCod;
         Grp_GetDataOfGroupByCod (&GrpDat);
         HTM_TxtF ("%s&nbsp;%s",GrpDat.GrpTypName,GrpDat.GrpName);
        }
      else	// GrpCod < 0 ==> students not belonging to any group of type (-GrpCod)
        {
         Gbl.Crs.Grps.GrpTyp.GrpTypCod = -GrpCod;
         Grp_GetDataOfGroupTypeByCod (&Gbl.Crs.Grps.GrpTyp);
         HTM_TxtF ("%s&nbsp;(%s)",Gbl.Crs.Grps.GrpTyp.GrpTypName,
                                  Txt_users_with_no_group);
        }

      if (Gbl.Crs.Grps.LstGrpsSel.NumGrps >= 2)
        {
         if (NumGrpSel == Gbl.Crs.Grps.LstGrpsSel.NumGrps-2)
            HTM_TxtF (" %s ",Txt_and);
         if (Gbl.Crs.Grps.LstGrpsSel.NumGrps >= 3)
            if (NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps-2)
               HTM_Txt (", ");
        }
     }
  }

/*****************************************************************************/
/************************** Put forms to edit groups *************************/
/*****************************************************************************/

void Grp_ReqEditGroups (void)
  {
   Grp_ReqEditGroupsInternal (Ale_INFO,NULL,
                              Ale_INFO,NULL);
  }

static void Grp_ReqEditGroupsInternal (Ale_AlertType_t AlertTypeGroupTypes,
                                       const char *AlertTextGroupTypes,
                                       Ale_AlertType_t AlertTypeGroups,
				       const char *AlertTextGroups)
  {
   Grp_ReqEditGroupsInternal0 ();
   Grp_ReqEditGroupsInternal1 (AlertTypeGroupTypes,AlertTextGroupTypes);
   Grp_ReqEditGroupsInternal2 (AlertTypeGroups,AlertTextGroups);
  }

static void Grp_ReqEditGroupsInternal0 (void)
  {
   /***** Start groups types section *****/
   HTM_SECTION_Begin (Grp_GROUP_TYPES_SECTION_ID);
  }

static void Grp_ReqEditGroupsInternal1 (Ale_AlertType_t AlertTypeGroupTypes,
                                        const char *AlertTextGroupTypes)
  {
   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ALL_GROUP_TYPES);

   /***** Show optional alert *****/
   if (AlertTextGroupTypes)
      if (AlertTextGroupTypes[0])
         Ale_ShowAlert (AlertTypeGroupTypes,AlertTextGroupTypes);

   /***** Put form to edit group types *****/
   Grp_EditGroupTypes ();

   /***** End groups types section *****/
   HTM_SECTION_End ();

   /***** Start groups section *****/
   HTM_SECTION_Begin (Grp_GROUPS_SECTION_ID);
  }

static void Grp_ReqEditGroupsInternal2 (Ale_AlertType_t AlertTypeGroups,
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
   if (Gbl.Crs.Grps.GrpTypes.Num) // If there are group types...
      Grp_EditGroups (&Rooms);

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

static void Grp_EditGroupTypes (void)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *Txt_Types_of_group;
   extern const char *Txt_There_are_no_types_of_group_in_the_course_X;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Types_of_group,
                 Grp_PutIconsEditingGroupTypes,NULL,
                 Hlp_USERS_Groups,Box_NOT_CLOSABLE);

   /***** Put a form to create a new group type *****/
   Grp_PutFormToCreateGroupType ();

   /***** Forms to edit current group types *****/
   if (Gbl.Crs.Grps.GrpTypes.Num)	// Group types found...
      Grp_ListGroupTypesForEdition ();
   else	// No group types found in this course
      Ale_ShowAlert (Ale_INFO,Txt_There_are_no_types_of_group_in_the_course_X,
                     Gbl.Hierarchy.Crs.ShrtName);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/**************************** Put forms to edit groups ***********************/
/*****************************************************************************/

static void Grp_EditGroups (const struct Roo_Rooms *Rooms)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *Txt_Groups;
   extern const char *Txt_No_groups_have_been_created_in_the_course_X;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Groups,
                 Grp_PutIconsEditingGroups,NULL,
                 Hlp_USERS_Groups,Box_NOT_CLOSABLE);

   /***** Put a form to create a new group *****/
   Grp_PutFormToCreateGroup (Rooms);

   /***** Forms to edit current groups *****/
   if (Gbl.Crs.Grps.GrpTypes.NumGrpsTotal)	// If there are groups...
      Grp_ListGroupsForEdition (Rooms);
   else	// There are group types, but there aren't groups
      Ale_ShowAlert (Ale_INFO,Txt_No_groups_have_been_created_in_the_course_X,
                     Gbl.Hierarchy.Crs.ShrtName);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of groups ******************/
/*****************************************************************************/

static void Grp_PutIconsEditingGroups (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view groups *****/
   Grp_PutIconToViewGroups ();

   /***** Put icon to create a new group *****/
   Grp_PutIconToCreateNewGroup ();
  }

static void Grp_PutIconToCreateNewGroup (void)
  {
   extern const char *Txt_New_group;

   /***** Put form to create a new group *****/
   Ico_PutContextualIconToAdd (ActReqEdiGrp,Grp_NEW_GROUP_SECTION_ID,
                               NULL,NULL,
			       Txt_New_group);
  }

/*****************************************************************************/
/*************** Show form to select one or several groups *******************/
/*****************************************************************************/

void Grp_ShowFormToSelectSeveralGroups (void (*FuncParams) (void *Args),void *Args,
                                        Grp_WhichGroups_t GroupsSelectableByStdsOrNETs)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *The_ClassFormLinkInBoxBold[The_NUM_THEMES];
   extern const char *Txt_Groups;
   extern const char *Txt_Update_users;
   unsigned NumGrpTyp;
   bool ICanEdit;

   /***** Trivial check: if no groups ==> nothing to do *****/
   if (!Gbl.Crs.Grps.NumGrps)
      return;

   /***** Begin box *****/
   ICanEdit = !Gbl.Form.Inside &&
	      (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
	       Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
   if (ICanEdit)
      Box_BoxBegin (NULL,Txt_Groups,
		    Grp_PutIconToEditGroups,NULL,
		    Hlp_USERS_Groups,Box_CLOSABLE);
   else
      Box_BoxBegin (NULL,Txt_Groups,
		    NULL,NULL,
		    Hlp_USERS_Groups,Box_CLOSABLE);

   /***** Begin form to update the students listed
	  depending on the groups selected *****/
   Frm_StartFormAnchor (Gbl.Action.Act,			// Repeat current action
			Usr_USER_LIST_SECTION_ID);
   Usr_PutParamsPrefsAboutUsrList ();
   if (FuncParams)
      FuncParams (Args);

   /***** Select all groups *****/
   Grp_PutCheckboxAllGrps (GroupsSelectableByStdsOrNETs);

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   /***** List the groups for each group type *****/
   HTM_TABLE_BeginWidePadding (2);
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	NumGrpTyp++)
      if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
	 Grp_ListGrpsForMultipleSelection (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],
					   GroupsSelectableByStdsOrNETs);
   HTM_TABLE_End ();

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();

   /***** Submit button *****/
   HTM_DIV_Begin ("class=\"CM\" style=\"padding-top:12px;\"");
   HTM_BUTTON_Animated_Begin (Txt_Update_users,
			      The_ClassFormLinkInBoxBold[Gbl.Prefs.Theme],
			      Gbl.Action.Act == ActReqMsgUsr ? "CopyMessageToHiddenFields();" :
                                                               NULL);
   Ico_PutCalculateIconWithText (Txt_Update_users);
   HTM_BUTTON_End ();
   HTM_DIV_End ();

   /***** End form *****/
   Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************* Put checkbox to select all groups ***********************/
/*****************************************************************************/

static void Grp_PutCheckboxAllGrps (Grp_WhichGroups_t GroupsSelectableByStdsOrNETs)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_All_groups;
   bool ICanSelUnselGroup;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
	 ICanSelUnselGroup = (GroupsSelectableByStdsOrNETs == Grp_ALL_GROUPS);
	 break;
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 ICanSelUnselGroup = true;
	 break;
      default:
	 ICanSelUnselGroup = false;
	 break;
     }

   HTM_DIV_Begin ("class=\"CONTEXT_OPT\"");
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_INPUT_CHECKBOX ("AllGroups",HTM_DONT_SUBMIT_ON_CHANGE,
		       "value=\"Y\"%s",
		       ICanSelUnselGroup ? (Gbl.Usrs.ClassPhoto.AllGroups ? " checked=\"checked\""
			                                                    " onclick=\"togglecheckChildren(this,'GrpCods')\"" :
			                                                    " onclick=\"togglecheckChildren(this,'GrpCods')\"") :
			                   " disabled=\"disabled\"");
   HTM_TxtF ("&nbsp;%s",Txt_All_groups);
   HTM_LABEL_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************ Put parameters with the groups of students selected ************/
/*****************************************************************************/

void Grp_PutParamsCodGrps (void)
  {
   extern const char *Par_SEPARATOR_PARAM_MULTIPLE;
   unsigned NumGrpSel;
   size_t MaxLengthGrpCods;
   char *GrpCods;
   char GrpCod[Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Write the boolean parameter that indicates if all the groups must be listed *****/
   Par_PutHiddenParamChar ("AllGroups",
                           Gbl.Usrs.ClassPhoto.AllGroups ? 'Y' :
                        	                           'N');

   /***** Write the parameter with the list of group codes to show *****/
   if (!Gbl.Usrs.ClassPhoto.AllGroups &&
       Gbl.Crs.Grps.LstGrpsSel.NumGrps)
     {
      MaxLengthGrpCods = Gbl.Crs.Grps.LstGrpsSel.NumGrps * (Cns_MAX_DECIMAL_DIGITS_LONG + 1) - 1;
      if ((GrpCods = malloc (MaxLengthGrpCods + 1)) == NULL)
	 Lay_NotEnoughMemoryExit ();
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

      Par_PutHiddenParamString (NULL,"GrpCods",GrpCods);
      free (GrpCods);
     }
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
   Gbl.Usrs.ClassPhoto.AllGroups = Par_GetParToBool ("AllGroups");

   /***** Get parameter with list of groups selected *****/
   Grp_GetParCodsSeveralGrps ();

   if (Gbl.Crs.Grps.NumGrps &&			// This course has groups and...
       !Gbl.Crs.Grps.LstGrpsSel.NumGrps)	// ...I haven't selected any group
     {
      /***** I I haven't selected any group, show by default the groups I belong to *****/
      /* Get list of groups of all types in current course I belong to */
      Grp_GetLstCodGrpsUsrBelongs (Gbl.Hierarchy.Crs.CrsCod,-1L,
				   Gbl.Usrs.Me.UsrDat.UsrCod,&LstGrpsIBelong);

      if (LstGrpsIBelong.NumGrps)
	{
	 /* Allocate space for list of selected groups */
	 if ((Gbl.Crs.Grps.LstGrpsSel.GrpCods = calloc (LstGrpsIBelong.NumGrps,
	                                                sizeof (*Gbl.Crs.Grps.LstGrpsSel.GrpCods))) == NULL)
	    Lay_NotEnoughMemoryExit ();

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
      Gbl.Usrs.ClassPhoto.AllGroups = true;
  }

/*****************************************************************************/
/**************** Get parameter with list of groups selected *****************/
/*****************************************************************************/

void Grp_GetParCodsSeveralGrps (void)
  {
   char *ParamLstCodGrps;
   const char *Ptr;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   unsigned NumGrp;
   unsigned long MaxSizeLstGrpCods = ((Cns_MAX_DECIMAL_DIGITS_LONG + 1) * Gbl.Crs.Grps.NumGrps) - 1;

   /***** Reset number of groups selected *****/
   Gbl.Crs.Grps.LstGrpsSel.NumGrps = 0;

   if (Gbl.Crs.Grps.NumGrps)	// If course has groups
     {
      /***** Allocate memory for the list of group codes selected *****/
      if ((ParamLstCodGrps = malloc (MaxSizeLstGrpCods + 1)) == NULL)
	 Lay_NotEnoughMemoryExit ();

      /***** Get parameter with list of groups to list *****/
      Par_GetParMultiToText ("GrpCods",ParamLstCodGrps,MaxSizeLstGrpCods);

      if (ParamLstCodGrps[0])
	{
	 /***** Count number of groups selected from LstCodGrps *****/
	 for (Ptr = ParamLstCodGrps, NumGrp = 0;
	      *Ptr;
	      NumGrp++)
	    Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
	 Gbl.Crs.Grps.LstGrpsSel.NumGrps = NumGrp;

	 if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)	// If I have selected groups...
	   {
	    /***** Create a list of groups selected from LstCodGrps *****/
	    if ((Gbl.Crs.Grps.LstGrpsSel.GrpCods = calloc (Gbl.Crs.Grps.LstGrpsSel.NumGrps,
	                                                   sizeof (*Gbl.Crs.Grps.LstGrpsSel.GrpCods))) == NULL)
	       Lay_NotEnoughMemoryExit ();
	    for (Ptr = ParamLstCodGrps, NumGrp = 0;
		 *Ptr;
		 NumGrp++)
	      {
	       Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
	       Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrp] = Str_ConvertStrCodToLongCod (LongStr);
	      }
	   }
	}

      /***** Free memory used for the list of groups to show *****/
      free (ParamLstCodGrps);
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
   Grp_ChangeMyGrps (Cns_VERBOSE);

   /***** Show again the table of selection of groups with the changes already made *****/
   Grp_ReqRegisterInGrps ();
  }

/*****************************************************************************/
/****************************** Change my groups *****************************/
/*****************************************************************************/

void Grp_ChangeMyGrps (Cns_QuietOrVerbose_t QuietOrVerbose)
  {
   extern const char *Txt_The_requested_group_changes_were_successful;
   extern const char *Txt_There_has_been_no_change_in_groups;
   extern const char *Txt_In_a_type_of_group_with_single_enrolment_students_can_not_be_registered_in_more_than_one_group;
   struct ListCodGrps LstGrpsIWant;
   bool MySelectionIsValid;
   bool ChangesMade;

   /***** Can I change my groups? *****/
   if (Grp_ICanChangeGrps[Gbl.Usrs.Me.Role.Logged])
     {
      /***** Get list of groups types and groups in this course *****/
      Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

      /***** Get the group codes which I want to join to *****/
      LstGrpsIWant.GrpCods = NULL;	// Initialized to avoid bug reported by Coverity
      LstGrpsIWant.NumGrps = 0;		// Initialized to avoid bug reported by Coverity
      Grp_GetLstCodsGrpWanted (&LstGrpsIWant);

      /***** A student can not be enroled in more than one group
	     if the type of group is of single enrolment *****/
      // As the form to register in groups of single enrolment...
      // ...is a radio-based form and not a checkbox-based form...
      // ...this check is made only to avoid problems...
      // ...if the student manipulates the form
      MySelectionIsValid = Grp_CheckIfSelectionGrpsSingleEnrolmentIsValid (Gbl.Usrs.Me.Role.Logged,&LstGrpsIWant);

      /***** Free list of groups types and groups in this course *****/
      // The lists of group types and groups need to be freed here...
      // ...in order to get them again when changing my groups atomically
      Grp_FreeListGrpTypesAndGrps ();

      /***** Change my groups *****/
      if (MySelectionIsValid)
	{
	 ChangesMade = Grp_ChangeMyGrpsAtomically (&LstGrpsIWant);
	 if (QuietOrVerbose == Cns_VERBOSE)
	   {
	    if (ChangesMade)
	       Ale_CreateAlert (Ale_SUCCESS,NULL,
		                Txt_The_requested_group_changes_were_successful);
	    else
	       Ale_CreateAlert (Ale_WARNING,NULL,
		                Txt_There_has_been_no_change_in_groups);
	   }
	}
      else if (QuietOrVerbose == Cns_VERBOSE)
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_In_a_type_of_group_with_single_enrolment_students_can_not_be_registered_in_more_than_one_group);

      /***** Free memory with the list of groups which I want to belong to *****/
      Grp_FreeListCodGrp (&LstGrpsIWant);
     }
  }

/*****************************************************************************/
/********************** Change groups of another user ************************/
/*****************************************************************************/

void Grp_ChangeOtherUsrGrps (void)
  {
   extern const char *Txt_The_requested_group_changes_were_successful;
   extern const char *Txt_There_has_been_no_change_in_groups;
   extern const char *Txt_In_a_type_of_group_with_single_enrolment_students_can_not_be_registered_in_more_than_one_group;
   struct ListCodGrps LstGrpsUsrWants;
   bool SelectionIsValid;

   /***** Can I change another user's groups? *****/
   if (Grp_ICanChangeGrps[Gbl.Usrs.Me.Role.Logged])
     {
      /***** Get list of groups types and groups in current course *****/
      Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

      /***** Get the list of groups to which register this user *****/
      LstGrpsUsrWants.GrpCods = NULL;	// Initialized to avoid bug reported by Coverity
      LstGrpsUsrWants.NumGrps = 0;	// Initialized to avoid bug reported by Coverity
      Grp_GetLstCodsGrpWanted (&LstGrpsUsrWants);

      /***** A student can not be enroled in more than one group
	     if the type of group is of single enrolment *****/
      SelectionIsValid = Grp_CheckIfSelectionGrpsSingleEnrolmentIsValid (Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role,&LstGrpsUsrWants);

      /***** Free list of groups types and groups in this course *****/
      // The lists of group types and groups need to be freed here...
      // ...in order to get them again when changing groups atomically
      Grp_FreeListGrpTypesAndGrps ();

      /***** Register user in the selected groups *****/
      if (SelectionIsValid)
	 Grp_ChangeGrpsOtherUsrAtomically (&LstGrpsUsrWants);
      else
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_In_a_type_of_group_with_single_enrolment_students_can_not_be_registered_in_more_than_one_group);

      /***** Free memory with the list of groups to/from which register/remove users *****/
      Grp_FreeListCodGrp (&LstGrpsUsrWants);
     }
  }

/*****************************************************************************/
/********************** Change my groups atomically **************************/
/*****************************************************************************/
// Return true if desired changes are made

bool Grp_ChangeMyGrpsAtomically (struct ListCodGrps *LstGrpsIWant)
  {
   struct ListCodGrps LstGrpsIBelong;
   unsigned NumGrpTyp;
   unsigned NumGrpIBelong;
   unsigned NumGrpIWant;
   unsigned NumGrpThisType;
   struct GroupType *GrpTyp;
   bool ITryToLeaveAClosedGroup      = false;
   bool ITryToRegisterInAClosedGroup = false;
   bool ITryToRegisterInFullGroup    = false;
   bool RemoveMeFromThisGrp;
   bool RegisterMeInThisGrp;
   bool ChangesMade = false;

   /***** Lock tables to make the inscription atomic *****/
   Grp_LockTables ();

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   /***** Query in the database the group codes which I belong to *****/
   Grp_GetLstCodGrpsUsrBelongs (Gbl.Hierarchy.Crs.CrsCod,-1L,
				Gbl.Usrs.Me.UsrDat.UsrCod,&LstGrpsIBelong);

   if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
     {
      /***** Go across the list of groups which I belong to and check if I try to leave a closed group *****/
      for (NumGrpIBelong = 0;
	   NumGrpIBelong < LstGrpsIBelong.NumGrps && !ITryToLeaveAClosedGroup;
	   NumGrpIBelong++)
	{
	 for (NumGrpIWant = 0, RemoveMeFromThisGrp = true;
	      NumGrpIWant < LstGrpsIWant->NumGrps && RemoveMeFromThisGrp;
	      NumGrpIWant++)
	    if (LstGrpsIBelong.GrpCods[NumGrpIBelong] == LstGrpsIWant->GrpCods[NumGrpIWant])
	       RemoveMeFromThisGrp = false;
	 if (RemoveMeFromThisGrp)
	    /* Check if the group is closed */
	    for (NumGrpTyp = 0;
		 NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num && !ITryToLeaveAClosedGroup;
		 NumGrpTyp++)
	      {
	       GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];
	       for (NumGrpThisType = 0;
		    NumGrpThisType < GrpTyp->NumGrps && !ITryToLeaveAClosedGroup;
		    NumGrpThisType++)
		  if ((GrpTyp->LstGrps[NumGrpThisType]).GrpCod == LstGrpsIBelong.GrpCods[NumGrpIBelong])
		     if (!((GrpTyp->LstGrps[NumGrpThisType]).Open))
			ITryToLeaveAClosedGroup = true;
	      }
	}

      if (!ITryToLeaveAClosedGroup)
	 /***** Go across the list of groups which I want to belong
		and check that they are not closed or full *****/
	 for (NumGrpIWant = 0;
	      NumGrpIWant < LstGrpsIWant->NumGrps &&
			    !ITryToRegisterInAClosedGroup &&
			    !ITryToRegisterInFullGroup;
	      NumGrpIWant++)
	   {
	    for (NumGrpIBelong = 0, RegisterMeInThisGrp = true;
		 NumGrpIBelong < LstGrpsIBelong.NumGrps && RegisterMeInThisGrp;
		 NumGrpIBelong++)
	       if (LstGrpsIWant->GrpCods[NumGrpIWant] == LstGrpsIBelong.GrpCods[NumGrpIBelong])
		  RegisterMeInThisGrp = false;
	    if (RegisterMeInThisGrp)
	       /* Check if the group is closed or full */
	       for (NumGrpTyp = 0;
		    NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num &&
				!ITryToRegisterInAClosedGroup &&
				!ITryToRegisterInFullGroup;
		    NumGrpTyp++)
		 {
		  GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];
		  for (NumGrpThisType = 0;
		       NumGrpThisType < GrpTyp->NumGrps &&
					!ITryToRegisterInAClosedGroup &&
					!ITryToRegisterInFullGroup;
		       NumGrpThisType++)
		     if ((GrpTyp->LstGrps[NumGrpThisType]).GrpCod == LstGrpsIWant->GrpCods[NumGrpIWant])
		       {
			/* Check if the group is closed */
			if (!((GrpTyp->LstGrps[NumGrpThisType]).Open))
			   ITryToRegisterInAClosedGroup = true;
			/* Check if the group is full */
			else if ((GrpTyp->LstGrps[NumGrpThisType]).NumUsrs[Rol_STD] >=
				 (GrpTyp->LstGrps[NumGrpThisType]).MaxStudents)
			   ITryToRegisterInFullGroup = true;
		       }
		 }
	   }
     }

   if (!ITryToLeaveAClosedGroup &&
       !ITryToRegisterInAClosedGroup &&
       !ITryToRegisterInFullGroup)
     {
      /***** Go across the list of groups I belong to, removing those groups that are not present in the list of groups I want to belong to *****/
      for (NumGrpIBelong = 0;
	   NumGrpIBelong < LstGrpsIBelong.NumGrps;
	   NumGrpIBelong++)
	{
	 for (NumGrpIWant = 0, RemoveMeFromThisGrp = true;
	      NumGrpIWant < LstGrpsIWant->NumGrps && RemoveMeFromThisGrp;
	      NumGrpIWant++)
	    if (LstGrpsIBelong.GrpCods[NumGrpIBelong] == LstGrpsIWant->GrpCods[NumGrpIWant])
	       RemoveMeFromThisGrp = false;
	 if (RemoveMeFromThisGrp)
	    Grp_RemoveUsrFromGroup (Gbl.Usrs.Me.UsrDat.UsrCod,LstGrpsIBelong.GrpCods[NumGrpIBelong]);
	}

      /***** Go across the list of groups that I want to register in, adding those groups that are not present in the list of groups I belong to *****/
      for (NumGrpIWant = 0;
	   NumGrpIWant < LstGrpsIWant->NumGrps;
	   NumGrpIWant++)
	{
	 for (NumGrpIBelong = 0, RegisterMeInThisGrp = true;
	      NumGrpIBelong < LstGrpsIBelong.NumGrps && RegisterMeInThisGrp;
	      NumGrpIBelong++)
	    if (LstGrpsIWant->GrpCods[NumGrpIWant] == LstGrpsIBelong.GrpCods[NumGrpIBelong])
	       RegisterMeInThisGrp = false;
	 if (RegisterMeInThisGrp)
	    Grp_AddUsrToGroup (&Gbl.Usrs.Me.UsrDat,LstGrpsIWant->GrpCods[NumGrpIWant]);
	}

      ChangesMade = true;
     }

   /***** Free memory with the list of groups which I belonged to *****/
   Grp_FreeListCodGrp (&LstGrpsIBelong);

   /***** Unlock tables after changes in my groups *****/
   Grp_UnlockTables ();

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();

   return ChangesMade;
  }

/*****************************************************************************/
/********************** Change my groups atomically **************************/
/*****************************************************************************/

void Grp_ChangeGrpsOtherUsrAtomically (struct ListCodGrps *LstGrpsUsrWants)
  {
   struct ListCodGrps LstGrpsUsrBelongs;
   unsigned NumGrpUsrBelongs;
   unsigned NumGrpUsrWants;
   bool RemoveUsrFromThisGrp;
   bool RegisterUsrInThisGrp;

   /***** Lock tables to make the inscription atomic *****/
   if (Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role == Rol_STD)
      Grp_LockTables ();

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   /***** Query in the database the group codes which user belongs to *****/
   Grp_GetLstCodGrpsUsrBelongs (Gbl.Hierarchy.Crs.CrsCod,-1L,
				Gbl.Usrs.Other.UsrDat.UsrCod,&LstGrpsUsrBelongs);

   /***** Go across the list of groups user belongs to, removing those groups that are not present in the list of groups user wants to belong to *****/
   for (NumGrpUsrBelongs = 0;
	NumGrpUsrBelongs < LstGrpsUsrBelongs.NumGrps;
	NumGrpUsrBelongs++)
     {
      for (NumGrpUsrWants = 0, RemoveUsrFromThisGrp = true;
	   NumGrpUsrWants < LstGrpsUsrWants->NumGrps && RemoveUsrFromThisGrp;
	   NumGrpUsrWants++)
	 if (LstGrpsUsrBelongs.GrpCods[NumGrpUsrBelongs] == LstGrpsUsrWants->GrpCods[NumGrpUsrWants])
	    RemoveUsrFromThisGrp = false;
      if (RemoveUsrFromThisGrp)
	 Grp_RemoveUsrFromGroup (Gbl.Usrs.Other.UsrDat.UsrCod,LstGrpsUsrBelongs.GrpCods[NumGrpUsrBelongs]);
     }

   /***** Go across the list of groups that user wants to register in, adding those groups that are not present in the list of groups user belongs to *****/
   for (NumGrpUsrWants = 0;
	NumGrpUsrWants < LstGrpsUsrWants->NumGrps;
	NumGrpUsrWants++)
     {
      for (NumGrpUsrBelongs = 0, RegisterUsrInThisGrp = true;
	   NumGrpUsrBelongs < LstGrpsUsrBelongs.NumGrps && RegisterUsrInThisGrp;
	   NumGrpUsrBelongs++)
	 if (LstGrpsUsrWants->GrpCods[NumGrpUsrWants] == LstGrpsUsrBelongs.GrpCods[NumGrpUsrBelongs])
	    RegisterUsrInThisGrp = false;
      if (RegisterUsrInThisGrp)
	 Grp_AddUsrToGroup (&Gbl.Usrs.Other.UsrDat,LstGrpsUsrWants->GrpCods[NumGrpUsrWants]);
     }

   /***** Free memory with the list of groups which user belonged to *****/
   Grp_FreeListCodGrp (&LstGrpsUsrBelongs);

   /***** Unlock tables after changes in groups *****/
   if (Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role == Rol_STD)
      Grp_UnlockTables ();

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/*********** Lock tables to make the registration in groups atomic ***********/
/*****************************************************************************/

static void Grp_LockTables (void)
  {
   DB_Query ("can not lock tables to change user's groups",
	     "LOCK TABLES "
	          "crs_grp_types WRITE,"
	          "grp_groups WRITE,"
	          "crs_grp_usr WRITE,"
	          "crs_users READ,"
	          "crs_usr_last READ,"
	          "roo_rooms READ");
   Gbl.DB.LockedTables = true;
  }

/*****************************************************************************/
/*********** Unlock tables after changes in registration in groups ***********/
/*****************************************************************************/

static void Grp_UnlockTables (void)
  {
   Gbl.DB.LockedTables = false;	// Set to false before the following unlock...
				// ...to not retry the unlock if error in unlocking
   DB_Query ("can not unlock tables after changing user's groups",
	     "UNLOCK TABLES");
  }

/*****************************************************************************/
/******* Check if not selected more than a group of single enrolment *********/
/*****************************************************************************/

bool Grp_CheckIfSelectionGrpsSingleEnrolmentIsValid (Rol_Role_t Role,struct ListCodGrps *LstGrps)
  {
   struct ListGrpsAlreadySelec *AlreadyExistsGroupOfType;
   unsigned NumCodGrp;
   unsigned NumGrpTyp;
   long GrpTypCod;
   bool MultipleEnrolment;
   bool SelectionValid;

   switch (Role)
     {
      case Rol_STD:
	 if (LstGrps->NumGrps <= 1)
	    return true;

	 /***** Create and initialize list of groups already selected *****/
	 Grp_ConstructorListGrpAlreadySelec (&AlreadyExistsGroupOfType);

	 /***** Go across the list of groups selected
	        checking if a group of the same type is already selected *****/
         SelectionValid = true;
	 for (NumCodGrp = 0;
	      SelectionValid && NumCodGrp < LstGrps->NumGrps;
	      NumCodGrp++)
	   {
	    GrpTypCod = Grp_GetTypeOfGroupOfAGroup (LstGrps->GrpCods[NumCodGrp]);
	    MultipleEnrolment = Grp_GetMultipleEnrolmentOfAGroupType (GrpTypCod);

	    if (!MultipleEnrolment)
	       for (NumGrpTyp = 0;
		    NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
		    NumGrpTyp++)
		  if (GrpTypCod == AlreadyExistsGroupOfType[NumGrpTyp].GrpTypCod)
		    {
		     if (AlreadyExistsGroupOfType[NumGrpTyp].AlreadySelected)
			SelectionValid = false;
		     else
			AlreadyExistsGroupOfType[NumGrpTyp].AlreadySelected = true;
		     break;
		    }
	   }

	 /***** Free memory of the list of booleanos that indicates
		if a group of each type has been selected *****/
	 Grp_DestructorListGrpAlreadySelec (&AlreadyExistsGroupOfType);
         return SelectionValid; // Return true if the selection of groups is correct
      case Rol_NET:
      case Rol_TCH:
	 return true;
      default:
         return false;
     }
  }

/*****************************************************************************/
/*********** Create e inicializar list of groups already selected ************/
/*****************************************************************************/

static void Grp_ConstructorListGrpAlreadySelec (struct ListGrpsAlreadySelec **AlreadyExistsGroupOfType)
  {
   unsigned NumGrpTyp;

   /***** Allocate memory to a list of booleanos that indica if already se ha selected a group of cada type *****/
   if ((*AlreadyExistsGroupOfType = calloc (Gbl.Crs.Grps.GrpTypes.Num,
                                            sizeof (**AlreadyExistsGroupOfType))) == NULL)
      Lay_NotEnoughMemoryExit ();

   /***** Initialize the list *****/
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
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
/******************* Register user in the groups of a list *******************/
/*****************************************************************************/

void Grp_RegisterUsrIntoGroups (struct UsrData *UsrDat,struct ListCodGrps *LstGrps)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_from_the_group_of_type_Y_to_which_it_belonged;
   extern const char *Txt_THE_USER_X_has_been_enroled_in_the_group_of_type_Y_Z;
   struct ListCodGrps LstGrpsHeBelongs;
   unsigned NumGrpTyp;
   unsigned NumGrpSel;
   unsigned NumGrpThisType;
   unsigned NumGrpHeBelongs;
   bool MultipleEnrolment;
   bool AlreadyRegisteredInGrp;

   /***** For each existing type of group in the course... *****/
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	NumGrpTyp++)
     {
      MultipleEnrolment = Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MultipleEnrolment;

      /***** Query in the database the group codes of any group of this type the student belongs to *****/
      LstGrpsHeBelongs.NumGrps = 0;	// Initialized to avoid bug reported by Coverity
      LstGrpsHeBelongs.GrpCods = NULL;	// Initialized to avoid bug reported by Coverity
      Grp_GetLstCodGrpsUsrBelongs (Gbl.Hierarchy.Crs.CrsCod,Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod,
	                           UsrDat->UsrCod,&LstGrpsHeBelongs);

      /***** For each group selected by me... *****/
      for (NumGrpSel = 0;
	   NumGrpSel < LstGrps->NumGrps;
	   NumGrpSel++)
        {
         /* Check if the selected group is of this type */
         for (NumGrpThisType = 0;
              NumGrpThisType < Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps;
              NumGrpThisType++)
            if (LstGrps->GrpCods[NumGrpSel] == Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].LstGrps[NumGrpThisType].GrpCod)
              {	// The selected group is of this type
               AlreadyRegisteredInGrp = false;

               /* For each group of this type to which the user belongs... */
               for (NumGrpHeBelongs = 0;
        	    NumGrpHeBelongs < LstGrpsHeBelongs.NumGrps;
        	    NumGrpHeBelongs++)
                  if (LstGrps->GrpCods[NumGrpSel] == LstGrpsHeBelongs.GrpCods[NumGrpHeBelongs])
                     AlreadyRegisteredInGrp = true;
                  else if (!MultipleEnrolment)	// If the type of group is of single enrolment
                    {
                     /* If the enrolment is single and the group to which the user belongs is different from the selected ==>
                        remove user from the group to which he belongs */
                     Grp_RemoveUsrFromGroup (UsrDat->UsrCod,LstGrpsHeBelongs.GrpCods[NumGrpHeBelongs]);
                     Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_removed_from_the_group_of_type_Y_to_which_it_belonged,
			            UsrDat->FullName,Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName);
                    }

               if (!AlreadyRegisteredInGrp)	// If the user does not belong to the selected group
                 {
                  Grp_AddUsrToGroup (UsrDat,LstGrps->GrpCods[NumGrpSel]);
                  Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_enroled_in_the_group_of_type_Y_Z,
		                 UsrDat->FullName,Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName,
                                 Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].LstGrps[NumGrpThisType].GrpName);
                 }

               break;	// Once we know the type of a selected group, it's not necessary to check the rest of types
              }
        }

      /***** Free the list of groups of this type to which the user belonged *****/
      Grp_FreeListCodGrp (&LstGrpsHeBelongs);
     }
  }

/*****************************************************************************/
/**************** Remove user of the groups indicados in a list **************/
/*****************************************************************************/
// Returns NumGrpsHeIsRemoved

unsigned Grp_RemoveUsrFromGroups (struct UsrData *UsrDat,struct ListCodGrps *LstGrps)
  {
   extern const char *Txt_THE_USER_X_has_not_been_removed_from_any_group;
   extern const char *Txt_THE_USER_X_has_been_removed_from_one_group;
   extern const char *Txt_THE_USER_X_has_been_removed_from_Y_groups;
   struct ListCodGrps LstGrpsHeBelongs;
   unsigned NumGrpSel;
   unsigned NumGrpHeBelongs;
   unsigned NumGrpsHeIsRemoved = 0;

   /***** Query in the database the group codes of any group the user belongs to *****/
   Grp_GetLstCodGrpsUsrBelongs (Gbl.Hierarchy.Crs.CrsCod,-1L,
	                        UsrDat->UsrCod,&LstGrpsHeBelongs);

   /***** For each group selected by me... *****/
   for (NumGrpSel = 0;
	NumGrpSel < LstGrps->NumGrps;
	NumGrpSel++)
      /* For each group to which the user belongs... */
      for (NumGrpHeBelongs = 0;
	   NumGrpHeBelongs < LstGrpsHeBelongs.NumGrps;
	   NumGrpHeBelongs++)
         /* If the user belongs to a selected group from which he must be removed */
         if (LstGrpsHeBelongs.GrpCods[NumGrpHeBelongs] == LstGrps->GrpCods[NumGrpSel])
           {
            Grp_RemoveUsrFromGroup (UsrDat->UsrCod,LstGrpsHeBelongs.GrpCods[NumGrpHeBelongs]);
            NumGrpsHeIsRemoved++;
           }

   /***** Write message to inform about how many groups the student has been removed from *****/
   if (NumGrpsHeIsRemoved == 0)
      Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_not_been_removed_from_any_group,
                     UsrDat->FullName);
   else if (NumGrpsHeIsRemoved == 1)
      Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_removed_from_one_group,
                     UsrDat->FullName);
   else	// NumGrpsHeIsRemoved > 1
      Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_removed_from_Y_groups,
                     UsrDat->FullName,NumGrpsHeIsRemoved);

   /***** Free the list of groups of this type to which the user belonged *****/
   Grp_FreeListCodGrp (&LstGrpsHeBelongs);

   return NumGrpsHeIsRemoved;
  }

/*****************************************************************************/
/*************** Remove a user of all the groups of a course *****************/
/*****************************************************************************/

void Grp_RemUsrFromAllGrpsInCrs (long UsrCod,long CrsCod)
  {
   bool ItsMe = Usr_ItsMe (UsrCod);

   /***** Remove user from all the groups of the course *****/
   DB_QueryDELETE ("can not remove a user from all groups of a course",
		   "DELETE FROM crs_grp_usr"
		   " WHERE UsrCod=%ld"
		     " AND GrpCod IN"
		         " (SELECT grp_groups.GrpCod"
		            " FROM crs_grp_types,"
		                  "grp_groups"
		           " WHERE crs_grp_types.CrsCod=%ld"
		             " AND crs_grp_types.GrpTypCod=grp_groups.GrpTypCod)",
                   UsrCod,CrsCod);

   /***** Flush caches *****/
   Grp_FlushCacheUsrSharesAnyOfMyGrpsInCurrentCrs ();
   if (ItsMe)
      Grp_FlushCacheIBelongToGrp ();
  }

/*****************************************************************************/
/******* Remove a user from all the groups of all the user's courses *********/
/*****************************************************************************/

void Grp_RemUsrFromAllGrps (long UsrCod)
  {
   bool ItsMe = Usr_ItsMe (UsrCod);

   /***** Remove user from all groups *****/
   DB_QueryDELETE ("can not remove a user from the groups he/she belongs to",
		   "DELETE FROM crs_grp_usr WHERE UsrCod=%ld",
		   UsrCod);

   /***** Flush caches *****/
   Grp_FlushCacheUsrSharesAnyOfMyGrpsInCurrentCrs ();
   if (ItsMe)
      Grp_FlushCacheIBelongToGrp ();
  }

/*****************************************************************************/
/************************* Remove a user from a group ************************/
/*****************************************************************************/

static void Grp_RemoveUsrFromGroup (long UsrCod,long GrpCod)
  {
   bool ItsMe = Usr_ItsMe (UsrCod);

   /***** Remove user from group *****/
   DB_QueryDELETE ("can not remove a user from a group",
		   "DELETE FROM crs_grp_usr"
		   " WHERE GrpCod=%ld AND UsrCod=%ld",
                   GrpCod,UsrCod);

   /***** Flush caches *****/
   Grp_FlushCacheUsrSharesAnyOfMyGrpsInCurrentCrs ();
   if (ItsMe)
      Grp_FlushCacheIBelongToGrp ();
  }

/*****************************************************************************/
/*********************** Register a user in a group **************************/
/*****************************************************************************/

static void Grp_AddUsrToGroup (struct UsrData *UsrDat,long GrpCod)
  {
   /***** Register in group *****/
   DB_QueryINSERT ("can not add a user to a group",
		   "INSERT INTO crs_grp_usr"
		   " (GrpCod,UsrCod)"
		   " VALUES"
		   " (%ld,%ld)",
                   GrpCod,UsrDat->UsrCod);
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
   unsigned UniqueId;
   char Id[32];

   /***** Write heading *****/
   HTM_TABLE_BeginWidePadding (2);
   Grp_WriteHeadingGroupTypes ();

   /***** List group types with forms for edition *****/
   for (NumGrpTyp = 0, UniqueId=1;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	NumGrpTyp++, UniqueId++)
     {
      HTM_TR_Begin (NULL);

      /* Put icon to remove group type */
      HTM_TD_Begin ("class=\"BM\"");
      Ico_PutContextualIconToRemove (ActReqRemGrpTyp,Grp_GROUP_TYPES_SECTION_ID,
				     Grp_PutParamGrpTypCod,&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
      HTM_TD_End ();

      /* Name of group type */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartFormAnchor (ActRenGrpTyp,Grp_GROUP_TYPES_SECTION_ID);
      Grp_PutParamGrpTypCod (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
      HTM_INPUT_TEXT ("GrpTypName",Grp_MAX_CHARS_GROUP_TYPE_NAME,
		      Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName,
		      HTM_SUBMIT_ON_CHANGE,
		      "size=\"12\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Is it mandatory to register in any group? */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartFormAnchor (ActChgMdtGrpTyp,Grp_GROUP_TYPES_SECTION_ID);
      Grp_PutParamGrpTypCod (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
      HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			"name=\"MandatoryEnrolment\""
	                " style=\"width:150px;\"");
      HTM_OPTION (HTM_Type_STRING,"N",
		  !Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MandatoryEnrolment,false,
		  "%s",Txt_It_is_optional_to_choose_a_group);
      HTM_OPTION (HTM_Type_STRING,"Y",
		  Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MandatoryEnrolment,false,
		  "%s",Txt_It_is_mandatory_to_choose_a_group);
      HTM_SELECT_End ();
      Frm_EndForm ();
      HTM_TD_End ();

      /* Is it possible to register in multiple groups? */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartFormAnchor (ActChgMulGrpTyp,Grp_GROUP_TYPES_SECTION_ID);
      Grp_PutParamGrpTypCod (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
      HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			"name=\"MultipleEnrolment\""
	                " style=\"width:150px;\"");
      HTM_OPTION (HTM_Type_STRING,"N",
		  !Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MultipleEnrolment,false,
		  "%s",Txt_A_student_can_only_belong_to_one_group);
      HTM_OPTION (HTM_Type_STRING,"Y",
		  Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MultipleEnrolment,false,
		  "%s",Txt_A_student_can_belong_to_several_groups);
      HTM_SELECT_End ();
      Frm_EndForm ();
      HTM_TD_End ();

      /* Open time */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartFormAnchor (ActChgTimGrpTyp,Grp_GROUP_TYPES_SECTION_ID);
      Grp_PutParamGrpTypCod (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
      HTM_TABLE_BeginCenterPadding (2);
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"LM\" style=\"width:16px;\"");
      Ico_PutIcon ("clock.svg",
		   Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MustBeOpened ? Txt_The_groups_will_automatically_open :
        	                                                               Txt_The_groups_will_not_automatically_open,
		   Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MustBeOpened ? "CONTEXT_ICO_16x16" :
        	                                                               "ICO_HIDDEN CONTEXT_ICO_16x16");
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LM\"");
      snprintf (Id,sizeof (Id),"open_time_%u",UniqueId);
      Dat_WriteFormClientLocalDateTimeFromTimeUTC (Id,
                                                   "Open",
						   Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].OpenTimeUTC,
						   Gbl.Now.Date.Year,
						   Gbl.Now.Date.Year + 1,
				                   Dat_FORM_SECONDS_ON,
				                   Dat_HMS_DO_NOT_SET,	// Don't set hour, minute and second
				                   true);		// Submit on change
      HTM_TD_End ();

      HTM_TR_End ();
      HTM_TABLE_End ();
      Frm_EndForm ();
      HTM_TD_End ();

      /* Number of groups of this type */
      HTM_TD_Begin ("class=\"DAT CM\"");
      HTM_Unsigned (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps);
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

   /***** Put icon to create a new type of group *****/
   Grp_PutIconToCreateNewGroupType ();
  }

static void Grp_PutIconToViewGroups (void)
  {
   Ico_PutContextualIconToView (ActReqSelGrp,
                                NULL,NULL);
  }

static void Grp_PutIconToCreateNewGroupType (void)
  {
   extern const char *Txt_New_type_of_group;

   /***** Put form to create a new type of group *****/
   Ico_PutContextualIconToAdd (ActReqEdiGrp,Grp_NEW_GROUP_TYPE_SECTION_ID,
                               NULL,NULL,
			       Txt_New_type_of_group);
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

   HTM_TH (1,1,"BM",NULL);
   HTM_TH_Begin (1,1,"CM");
   HTM_Txt (Txt_Type_of_group);
   HTM_BR ();
   HTM_TxtF ("(%s)",Txt_eg_Lectures_Practicals);
   HTM_TH_End ();
   HTM_TH (1,1,"CM",Txt_Mandatory_enrolment);
   HTM_TH (1,1,"CM",Txt_Multiple_enrolment);
   HTM_TH (1,1,"CM",Txt_Opening_of_groups);
   HTM_TH (1,1,"CM",Txt_Number_of_BR_groups);

   HTM_TR_End ();
  }

/*****************************************************************************/
/********************** List current groups for edition **********************/
/*****************************************************************************/

static void Grp_ListGroupsForEdition (const struct Roo_Rooms *Rooms)
  {
   extern const char *Txt_Group_X_open_click_to_close_it;
   extern const char *Txt_Group_X_closed_click_to_open_it;
   extern const char *Txt_File_zones_of_the_group_X_enabled_click_to_disable_them;
   extern const char *Txt_File_zones_of_the_group_X_disabled_click_to_enable_them;
   extern const char *Txt_No_assigned_room;
   extern const char *Txt_Another_room;
   unsigned NumGrpTyp;
   unsigned NumTipGrpAux;
   unsigned NumGrpThisType;
   unsigned NumRoo;
   struct GroupType *GrpTyp;
   struct GroupType *GrpTypAux;
   struct Group *Grp;
   Rol_Role_t Role;
   char StrMaxStudents[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Write heading *****/
   HTM_TABLE_BeginWidePadding (2);
   Grp_WriteHeadingGroups ();

   /***** List the groups *****/
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	NumGrpTyp++)
     {
      GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];
      for (NumGrpThisType = 0;
	   NumGrpThisType < GrpTyp->NumGrps;
	   NumGrpThisType++)
        {
         Grp = &(GrpTyp->LstGrps[NumGrpThisType]);

         HTM_TR_Begin (NULL);

         /***** Icon to remove group *****/
         HTM_TD_Begin ("class=\"BM\"");
	 Ico_PutContextualIconToRemove (ActReqRemGrp,Grp_GROUPS_SECTION_ID,
					Grp_PutParamGrpCod,&Grp->GrpCod);
         HTM_TD_End ();

         /***** Icon to open/close group *****/
         HTM_TD_Begin ("class=\"BM\"");
         Frm_StartFormAnchor (Grp->Open ? ActCloGrp :
                                          ActOpeGrp,
                              Grp_GROUPS_SECTION_ID);
         Grp_PutParamGrpCod (&Grp->GrpCod);
	 Ico_PutIconLink (Grp->Open ? "unlock.svg" :
                	              "lock.svg",
                          Str_BuildStringStr (Grp->Open ? Txt_Group_X_open_click_to_close_it :
                        			          Txt_Group_X_closed_click_to_open_it,
					      Grp->GrpName));
	 Str_FreeString ();
         Frm_EndForm ();
         HTM_TD_End ();

         /***** Icon to activate file zones for this group *****/
         HTM_TD_Begin ("class=\"BM\"");
         Frm_StartFormAnchor (Grp->FileZones ? ActDisFilZonGrp :
                                               ActEnaFilZonGrp,
                              Grp_GROUPS_SECTION_ID);
         Grp_PutParamGrpCod (&Grp->GrpCod);
	 Ico_PutIconLink (Grp->FileZones ? "folder-open-green.svg" :
                	                   "folder-red.svg",
                          Str_BuildStringStr (Grp->FileZones ? Txt_File_zones_of_the_group_X_enabled_click_to_disable_them :
							       Txt_File_zones_of_the_group_X_disabled_click_to_enable_them,
					      Grp->GrpName));
	 Str_FreeString ();
         Frm_EndForm ();
         HTM_TD_End ();

         /***** Group type *****/
         /* Start selector */
         HTM_TD_Begin ("class=\"CM\"");
         Frm_StartFormAnchor (ActChgGrpTyp,Grp_GROUPS_SECTION_ID);
         Grp_PutParamGrpCod (&Grp->GrpCod);
         HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			   "name=\"GrpTypCod\" style=\"width:100px;\"");

         /* Options for group types */
         for (NumTipGrpAux = 0;
              NumTipGrpAux < Gbl.Crs.Grps.GrpTypes.Num;
              NumTipGrpAux++)
           {
            GrpTypAux = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumTipGrpAux];
	    HTM_OPTION (HTM_Type_LONG,&GrpTypAux->GrpTypCod,
			GrpTypAux->GrpTypCod == GrpTyp->GrpTypCod,false,
			"%s",GrpTypAux->GrpTypName);
           }

         /* End selector */
         HTM_SELECT_End ();
         Frm_EndForm ();
         HTM_TD_End ();

         /***** Group name *****/
         HTM_TD_Begin ("class=\"CM\"");
         Frm_StartFormAnchor (ActRenGrp,Grp_GROUPS_SECTION_ID);
         Grp_PutParamGrpCod (&Grp->GrpCod);
	 HTM_INPUT_TEXT ("GrpName",Grp_MAX_CHARS_GROUP_NAME,Grp->GrpName,
	                 HTM_SUBMIT_ON_CHANGE,
			 "size=\"20\"");
         Frm_EndForm ();
         HTM_TD_End ();

	 /***** Room *****/
	 /* Start selector */
	 HTM_TD_Begin ("class=\"CM\"");
         Frm_StartFormAnchor (ActChgGrpRoo,Grp_GROUPS_SECTION_ID);
         Grp_PutParamGrpCod (&Grp->GrpCod);
         HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			   "name=\"RooCod\" style=\"width:100px;\"");

	 /* Option for no assigned room */
	 HTM_OPTION (HTM_Type_STRING,"-1",
		     Grp->Room.RooCod < 0,false,
		     "%s",Txt_No_assigned_room);

	 /* Option for another room */
	 HTM_OPTION (HTM_Type_STRING,"0",
		     Grp->Room.RooCod == 0,false,
		     "%s",Txt_Another_room);

	 /* Options for rooms */
	 for (NumRoo = 0;
	      NumRoo < Rooms->Num;
	      NumRoo++)
	    HTM_OPTION (HTM_Type_LONG,&Rooms->Lst[NumRoo].RooCod,
			Rooms->Lst[NumRoo].RooCod == Grp->Room.RooCod,false,
			"%s",Rooms->Lst[NumRoo].ShrtName);

	 /* End selector */
	 HTM_SELECT_End ();
         Frm_EndForm ();
	 HTM_TD_End ();

         /***** Current number of users in this group *****/
         for (Role = Rol_TCH;
              Role >= Rol_STD;
              Role--)
           {
	    HTM_TD_Begin ("class=\"DAT CM\"");
	    HTM_Int (Grp->NumUsrs[Role]);
	    HTM_TD_End ();
           }

         /***** Maximum number of students of the group (row[3]) *****/
         HTM_TD_Begin ("class=\"CM\"");
         Frm_StartFormAnchor (ActChgMaxStdGrp,Grp_GROUPS_SECTION_ID);
         Grp_PutParamGrpCod (&Grp->GrpCod);
         Grp_WriteMaxStds (StrMaxStudents,Grp->MaxStudents);
	 HTM_INPUT_TEXT ("MaxStudents",Cns_MAX_DECIMAL_DIGITS_UINT,StrMaxStudents,
	                 HTM_SUBMIT_ON_CHANGE,
			 "size=\"3\"");
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

   HTM_TH (1,1,"BM",NULL);
   HTM_TH (1,1,"BM",NULL);
   HTM_TH (1,1,"BM",NULL);
   HTM_TH (1,1,"CM",Txt_Type_BR_of_group);
   HTM_TH_Begin (1,1,"CM");
   HTM_Txt (Txt_Group_name);
   HTM_BR ();
   HTM_TxtF ("(%s)",Txt_eg_A_B);
   HTM_TH_End ();
   HTM_TH (1,1,"CM",Txt_Room);
   for (Role = Rol_TCH;
	Role >= Rol_STD;
	Role--)
      HTM_TH (1,1,"CM",Txt_ROLES_PLURAL_BRIEF_Abc[Role]);
   HTM_TH (1,1,"CM",Txt_Max_BR_students);

   HTM_TR_End ();
  }

/*****************************************************************************/
/****** List groups of a type to edit                                   ******/
/****** assignments, attendance events, surveys, exam events or matches ******/
/*****************************************************************************/

void Grp_ListGrpsToEditAsgAttSvyEvtMch (struct GroupType *GrpTyp,
                                        Grp_WhichIsAssociatedToGrp_t WhichIsAssociatedToGrp,
                                        long Cod)	// Assignment, attendance event, survey, exam event or match
  {
   static const struct
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
   bool IBelongToThisGroup;
   struct Group *Grp;
   bool AssociatedToGrp;

   /***** Write heading *****/
   Grp_WriteGrpHead (GrpTyp);

   /***** Query from the database the groups of this type which I belong to *****/
   Grp_GetLstCodGrpsUsrBelongs (Gbl.Hierarchy.Crs.CrsCod,GrpTyp->GrpTypCod,
	                        Gbl.Usrs.Me.UsrDat.UsrCod,&LstGrpsIBelong);

   /***** List the groups *****/
   for (NumGrpThisType = 0;
	NumGrpThisType < GrpTyp->NumGrps;
	NumGrpThisType++)
     {
      Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
      IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsIBelong);

      if (Cod > 0)	// Cod == -1L means new item, assignment, event, survey, exam event or match
	 AssociatedToGrp = Grp_CheckIfAssociatedToGrp (AssociationsToGrps[WhichIsAssociatedToGrp].Table,
	                                               AssociationsToGrps[WhichIsAssociatedToGrp].Field,
	                                               Cod,Grp->GrpCod);
      else
         AssociatedToGrp = false;

      /* Put checkbox to select the group */
      HTM_TR_Begin (NULL);

      if (IBelongToThisGroup)
	 HTM_TD_Begin ("class=\"LM LIGHT_BLUE\"");
      else
	 HTM_TD_Begin ("class=\"LM\"");
      HTM_INPUT_CHECKBOX ("GrpCods",HTM_DONT_SUBMIT_ON_CHANGE,
		          "id=\"Grp%ld\" value=\"%ld\"%s%s"
		          " onclick=\"uncheckParent(this,'WholeCrs')\"",
			  Grp->GrpCod,Grp->GrpCod,
			  AssociatedToGrp ? " checked=\"checked\"" :
				            "",
			  (IBelongToThisGroup ||
                           Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM) ? "" :
                        	                                     " disabled=\"disabled\"");
      HTM_TD_End ();

      Grp_WriteRowGrp (Grp,IBelongToThisGroup);

      HTM_TR_End ();
     }

   /***** Free memory with the list of groups which I belongs to *****/
   Grp_FreeListCodGrp (&LstGrpsIBelong);
  }


/*****************************************************************************/
/************ Check if an assignment is associated to a group ****************/
/*****************************************************************************/

static bool Grp_CheckIfAssociatedToGrp (const char *Table,const char *Field,
                                        long Cod,long GrpCod)
  {
   /***** Get if an assignment, attendance event, survey, exam event or match
          is associated to a given group from database *****/
   return (DB_QueryCOUNT ("can not check if associated to a group",
			  "SELECT COUNT(*) FROM %s"
			  " WHERE %s=%ld AND GrpCod=%ld",
		  	  Table,Field,Cod,GrpCod) != 0);
  }


/*****************************************************************************/
/*** Check if an assignment, attendance event, survey, exam event or match ***/
/*** is associated to any group                                            ***/
/*****************************************************************************/

bool Grp_CheckIfAssociatedToGrps (const char *Table,const char *Field,long Cod)
  {
   /***** Trivial check *****/
   if (Cod <= 0)	// Assignment, attendance event, survey, exam event or match code
      return false;

   /***** Get if an assignment, attendance event, survey, exam event or match
          is associated to any group from database *****/
   return (DB_QueryCOUNT ("can not check if associated to groups",
			  "SELECT COUNT(*) FROM %s"
			  " WHERE %s=%ld",
			  Table,Field,Cod) != 0);
  }

/*****************************************************************************/
/***************** Show list of groups to register/remove me *****************/
/*****************************************************************************/

void Grp_ReqRegisterInGrps (void)
  {
   /***** Show list of groups to register/remove me *****/
   Grp_ShowLstGrpsToChgMyGrps ();
  }

/*****************************************************************************/
/***************** Show list of groups to register/remove me *****************/
/*****************************************************************************/

void Grp_ShowLstGrpsToChgMyGrps (void)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *Txt_My_groups;
   extern const char *Txt_Change_my_groups;
   extern const char *Txt_Enrol_in_groups;
   extern const char *Txt_No_groups_have_been_created_in_the_course_X;
   extern const char *Txt_Create_group;
   unsigned NumGrpTyp;
   unsigned NumGrpsThisTypeIBelong;
   unsigned NumGrpsIBelong = 0;
   bool PutFormToChangeGrps = !Gbl.Form.Inside;	// Not inside another form (record card)
   bool ICanEdit = !Gbl.Form.Inside &&
	           (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
                    Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
   bool ICanChangeMyGrps = false;

   if (Gbl.Crs.Grps.NumGrps) // This course has groups
     {
      /***** Get list of groups types and groups in this course *****/
      Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

      /***** Show warnings to students *****/
      // Students are required to join groups with mandatory enrolment; teachers don't
      if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
	 Grp_ShowWarningToStdsToChangeGrps ();
     }

   /***** Begin box *****/
   if (ICanEdit)
      Box_BoxBegin (NULL,Txt_My_groups,
		    Grp_PutIconToEditGroups,NULL,
		    Hlp_USERS_Groups,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,Txt_My_groups,
		    NULL,NULL,
		    Hlp_USERS_Groups,Box_NOT_CLOSABLE);

   if (Gbl.Crs.Grps.NumGrps) // This course has groups
     {
      /***** Begin form *****/
      if (PutFormToChangeGrps)
	 Frm_BeginForm (ActChgGrp);

      /***** List the groups the user belongs to for change *****/
      HTM_TABLE_BeginWidePadding (2);
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
	 if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)	 // If there are groups of this type
	   {
	    ICanChangeMyGrps |= Grp_ListGrpsForChangeMySelection (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],
	                                                          &NumGrpsThisTypeIBelong);
	    NumGrpsIBelong += NumGrpsThisTypeIBelong;
	   }
      HTM_TABLE_End ();

      /***** End form *****/
      if (PutFormToChangeGrps)
	{
	 if (ICanChangeMyGrps)
	    Btn_PutConfirmButton (NumGrpsIBelong ? Txt_Change_my_groups :
						   Txt_Enrol_in_groups);
	 Frm_EndForm ();
	}
     }
   else	// This course has no groups
     {
      Ale_ShowAlert (Ale_INFO,Txt_No_groups_have_been_created_in_the_course_X,
                     Gbl.Hierarchy.Crs.FullName);

      /***** Button to create group *****/
      if (ICanEdit)
	{
	 Frm_BeginForm (ActReqEdiGrp);
	 Btn_PutConfirmButton (Txt_Create_group);
	 Frm_EndForm ();
	}
     }

   /***** End box *****/
   Box_BoxEnd ();

   if (Gbl.Crs.Grps.NumGrps) // This course has groups
      /***** Free list of groups types and groups in this course *****/
      Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/*************************** Put icon to edit groups *************************/
/*****************************************************************************/

static void Grp_PutIconToEditGroups (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToEdit (ActReqEdiGrp,NULL,
				NULL,NULL);
  }

/*****************************************************************************/
/*********** Show warnings to students before form to change groups **********/
/*****************************************************************************/

static void Grp_ShowWarningToStdsToChangeGrps (void)
  {
   extern const char *Txt_You_have_to_register_compulsorily_at_least_in_one_group_of_type_X;
   extern const char *Txt_You_have_to_register_compulsorily_in_one_group_of_type_X;
   extern const char *Txt_You_can_register_voluntarily_in_one_or_more_groups_of_type_X;
   extern const char *Txt_You_can_register_voluntarily_in_one_group_of_type_X;
   unsigned NumGrpTyp;
   struct GroupType *GrpTyp;

   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	NumGrpTyp++)
     {
      GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];
      // If there are groups of this type...
      if (GrpTyp->NumGrps)
	 // If I don't belong to any group
	 if (!Grp_CheckIfIBelongToGrpsOfType (GrpTyp->GrpTypCod))		// Fast check (not necesary, but avoid slow check)
	    // If there is any group of this type available
	    if (Grp_GetIfAvailableGrpTyp (GrpTyp->GrpTypCod))	// Slow check
	      {
	       if (GrpTyp->MandatoryEnrolment)
		  Ale_ShowAlert (Ale_WARNING,GrpTyp->MultipleEnrolment ? Txt_You_have_to_register_compulsorily_at_least_in_one_group_of_type_X :
						                         Txt_You_have_to_register_compulsorily_in_one_group_of_type_X,
			         GrpTyp->GrpTypName);
	       else
		  Ale_ShowAlert (Ale_INFO,GrpTyp->MultipleEnrolment ? Txt_You_can_register_voluntarily_in_one_or_more_groups_of_type_X :
						                      Txt_You_can_register_voluntarily_in_one_group_of_type_X,
			         GrpTyp->GrpTypName);
	      }
     }
  }

/*****************************************************************************/
/*************** List the groups of a type to register in ********************/
/*****************************************************************************/
// Returns true if I can change my selection

static bool Grp_ListGrpsForChangeMySelection (struct GroupType *GrpTyp,
                                              unsigned *NumGrpsThisTypeIBelong)
  {
   struct ListCodGrps LstGrpsIBelong;
   unsigned NumGrpThisType;
   struct Group *Grp;
   bool IBelongToThisGroup;
   bool IBelongToAClosedGroup;
   bool ICanChangeMySelectionForThisGrpTyp;
   bool ICanChangeMySelectionForThisGrp;
   char StrGrpCod[32];

   /***** Write heading *****/
   Grp_WriteGrpHead (GrpTyp);

   /***** Query in the database the group of this type that I belong to *****/
   Grp_GetLstCodGrpsUsrBelongs (Gbl.Hierarchy.Crs.CrsCod,GrpTyp->GrpTypCod,
	                        Gbl.Usrs.Me.UsrDat.UsrCod,&LstGrpsIBelong);
   *NumGrpsThisTypeIBelong = LstGrpsIBelong.NumGrps;

   /***** Check if I can change my selection *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 if (GrpTyp->MultipleEnrolment)	// Enrolment is multiple
	   {
	    for (NumGrpThisType = 0, ICanChangeMySelectionForThisGrpTyp = false;
		 NumGrpThisType < GrpTyp->NumGrps && !ICanChangeMySelectionForThisGrpTyp;
		 NumGrpThisType++)
	      {
	       Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
	       if (Grp->Open)				// If group is open
		 {
	          IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsIBelong);
	          if (IBelongToThisGroup)		// I belong to this group
	             ICanChangeMySelectionForThisGrpTyp = true;	// I can unregister from group
	          else					// I don't belong
	             if (Grp->NumUsrs[Rol_STD] < Grp->MaxStudents)	// Group is not full
	                ICanChangeMySelectionForThisGrpTyp = true;	// I can register into group
		 }
	      }
	   }
	 else				// Enrolment is single
	   {
	    /* Step 1: Check if I belong to a closed group */
	    for (NumGrpThisType = 0, IBelongToAClosedGroup = false;
		 NumGrpThisType < GrpTyp->NumGrps && !IBelongToAClosedGroup;
		 NumGrpThisType++)
	      {
	       Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
	       if (!Grp->Open)				// If group is closed
		 {
	          IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsIBelong);
	          if (IBelongToThisGroup)		// I belong to this group
	             IBelongToAClosedGroup = true;	// I belong to a closed group
		 }
	      }

	    if (IBelongToAClosedGroup)			// I belong to a closed group
	       ICanChangeMySelectionForThisGrpTyp = false;		// I can not unregister
	    else					// I don't belong to a closed group
	       /* Step 2: Check if I can register in at least one group to which I don't belong */
	       for (NumGrpThisType = 0, ICanChangeMySelectionForThisGrpTyp = false;
		    NumGrpThisType < GrpTyp->NumGrps && !ICanChangeMySelectionForThisGrpTyp;
		    NumGrpThisType++)
		 {
		  Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
		  if (Grp->Open &&					// If group is open...
		      Grp->NumUsrs[Rol_STD] < Grp->MaxStudents)		// ...and not full
		    {
		     IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsIBelong);
		     if (!IBelongToThisGroup)		// I don't belong to this group
			ICanChangeMySelectionForThisGrpTyp = true;	// I can register into this group
		    }
		 }
	   }
	 break;
      case Rol_TCH:
      case Rol_SYS_ADM:
	 ICanChangeMySelectionForThisGrpTyp = true;			// I can not register/unregister
	 break;
      default:
	 ICanChangeMySelectionForThisGrpTyp = false;			// I can not register/unregister
	 break;
     }

   /***** List the groups *****/
   for (NumGrpThisType = 0;
	NumGrpThisType < GrpTyp->NumGrps;
	NumGrpThisType++)
     {
      Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
      IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsIBelong);

      /* Selection disabled? */
      if (ICanChangeMySelectionForThisGrpTyp)	// I can change my selection for this group type
	{
	 ICanChangeMySelectionForThisGrp = true;
	 if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
	   {
	    if (Grp->Open)					// If group is open
	      {
	       if (!IBelongToThisGroup &&			// I don't belong to group
		   Grp->NumUsrs[Rol_STD] >= Grp->MaxStudents)	// Group is full
		  ICanChangeMySelectionForThisGrp = false;
	      }
	    else						// If group is closed
	       ICanChangeMySelectionForThisGrp = false;
	   }
        }
      else					// I can not change my selection for this group type
	 ICanChangeMySelectionForThisGrp = false;

      /* Put radio item or checkbox to select the group */
      HTM_TR_Begin (NULL);

      if (IBelongToThisGroup)
	 HTM_TD_Begin ("class=\"LM LIGHT_BLUE\"");
      else
	 HTM_TD_Begin ("class=\"LM\"");

      snprintf (StrGrpCod,sizeof (StrGrpCod),"GrpCod%ld",GrpTyp->GrpTypCod);
      if (Gbl.Usrs.Me.Role.Logged == Rol_STD &&	// If I am a student
          !GrpTyp->MultipleEnrolment &&		// ...and the enrolment is single
          GrpTyp->NumGrps > 1)			// ...and there are more than one group
	{
	 /* Put a radio item */
         if (GrpTyp->MandatoryEnrolment)
	    HTM_INPUT_RADIO (StrGrpCod,false,
			     "id=\"Grp%ld\" value=\"%ld\"%s%s",
			     Grp->GrpCod,Grp->GrpCod,
			     IBelongToThisGroup ? " checked=\"checked\"" : "", // Group selected?
         	             ICanChangeMySelectionForThisGrp ? "" :
	                                                       IBelongToThisGroup ? " readonly" :		// I can not unregister (disabled does not work because the value is not submitted)
						                                    " disabled=\"disabled\"");	// I can not register
         else	// If the enrolment is not mandatory, I can select no groups
	    HTM_INPUT_RADIO (StrGrpCod,false,
			     "id=\"Grp%ld\" value=\"%ld\"%s%s"
			     " onclick=\"selectUnselectRadio(this,this.form.GrpCod%ld,%u)\"",
			     Grp->GrpCod,Grp->GrpCod,
			     IBelongToThisGroup ? " checked=\"checked\"" : "", // Group selected?
         	             ICanChangeMySelectionForThisGrp ? "" :
	                                                       IBelongToThisGroup ? " readonly" :		// I can not unregister (disabled does not work because the value is not submitted)
						                                    " disabled=\"disabled\"",	// I can not register
                             GrpTyp->GrpTypCod,GrpTyp->NumGrps);
	}
      else
	 /* Put a checkbox item */
	 HTM_INPUT_CHECKBOX (StrGrpCod,HTM_DONT_SUBMIT_ON_CHANGE,
			     "id=\"Grp%ld\" value=\"%ld\"%s%s",
			     Grp->GrpCod,Grp->GrpCod,
			     IBelongToThisGroup ? " checked=\"checked\"" : "",
			     ICanChangeMySelectionForThisGrp ? "" :
				                               IBelongToThisGroup ? " readonly" :		// I can not unregister (disabled does not work because the value is not submitted)
				                        	                    " disabled=\"disabled\"");	// I can not register

      HTM_TD_End ();

      Grp_WriteRowGrp (Grp,IBelongToThisGroup);

      HTM_TR_End ();
     }

   /***** Free memory with the list of groups a the that belongs the user *****/
   Grp_FreeListCodGrp (&LstGrpsIBelong);

   return ICanChangeMySelectionForThisGrpTyp;
  }

/*****************************************************************************/
/*************** Show list of groups to register/remove users ****************/
/*****************************************************************************/
// If UsrCod  > 0 ==> mark her/his groups as checked
// If UsrCod <= 0 ==> do not mark any group as checked

void Grp_ShowLstGrpsToChgOtherUsrsGrps (long UsrCod)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *Txt_Groups;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in current course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_Groups,
                      NULL,NULL,
                      Hlp_USERS_Groups,Box_NOT_CLOSABLE,0);

   /***** List to select the groups the user belongs to *****/
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	NumGrpTyp++)
      if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
	 Grp_ListGrpsToAddOrRemUsrs (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],UsrCod);

   /***** End table and box *****/
   Box_BoxTableEnd ();

   /***** Free list of groups types and groups in current course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/*************** List groups of a type to add or remove users ****************/
/*****************************************************************************/
// If UsrCod  > 0 ==> mark her/his groups as checked
// If UsrCod <= 0 ==> do not mark any group as checked

static void Grp_ListGrpsToAddOrRemUsrs (struct GroupType *GrpTyp,long UsrCod)
  {
   struct ListCodGrps LstGrpsUsrBelongs;
   unsigned NumGrpThisType;
   bool UsrBelongsToThisGroup;
   struct Group *Grp;
   char StrGrpCod[32];

   /***** Write heading *****/
   Grp_WriteGrpHead (GrpTyp);

   /***** Query the groups of this type which the user belongs to *****/
   if (UsrCod > 0)
      Grp_GetLstCodGrpsUsrBelongs (Gbl.Hierarchy.Crs.CrsCod,GrpTyp->GrpTypCod,
				   Gbl.Usrs.Other.UsrDat.UsrCod,&LstGrpsUsrBelongs);

   /***** List the groups *****/
   for (NumGrpThisType = 0;
	NumGrpThisType < GrpTyp->NumGrps;
	NumGrpThisType++)
     {
      Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
      UsrBelongsToThisGroup = (UsrCod > 0) ? Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsUsrBelongs) :
	                                     false;

      /* Start row */
      HTM_TR_Begin (NULL);

      /* Start cell for checkbox */
      if (UsrBelongsToThisGroup)
	 HTM_TD_Begin ("class=\"LM LIGHT_BLUE\"");
      else
	 HTM_TD_Begin ("class=\"LM\"");

      /* Put checkbox to select the group */
      // Always checkbox, not radio, because the role in the form may be teacher,
      // so if he/she is registered as teacher, he/she can belong to several groups
      snprintf (StrGrpCod,sizeof (StrGrpCod),"GrpCod%ld",GrpTyp->GrpTypCod);
      HTM_INPUT_CHECKBOX (StrGrpCod,HTM_DONT_SUBMIT_ON_CHANGE,
			  "id=\"Grp%ld\" value=\"%ld\"%s",
			  Grp->GrpCod,Grp->GrpCod,
			  UsrBelongsToThisGroup ? " checked=\"checked\"" : "");	// I can not register

      /* End cell for checkbox */
      HTM_TD_End ();

      /* Write cell for group */
      Grp_WriteRowGrp (Grp,UsrBelongsToThisGroup);

      /* End row */
      HTM_TR_End ();
     }

   /***** Free memory with the lists of groups *****/
   if (UsrCod > 0)
      Grp_FreeListCodGrp (&LstGrpsUsrBelongs);
  }

/*****************************************************************************/
/******* Write a list of groups as checkbox form for unique selection ********/
/*****************************************************************************/

static void Grp_ListGrpsForMultipleSelection (struct GroupType *GrpTyp,
                                              Grp_WhichGroups_t GroupsSelectableByStdsOrNETs)
  {
   extern const char *Txt_users_with_no_group;
   unsigned NumGrpThisType;
   unsigned NumGrpSel;
   struct ListCodGrps LstGrpsIBelong;
   bool IBelongToThisGroup;
   bool ICanSelUnselGroup;
   bool Checked;
   struct Group *Grp;
   Rol_Role_t Role;

   /***** Write heading *****/
   Grp_WriteGrpHead (GrpTyp);

   /***** Query from the database the groups of this type which I belong to *****/
   Grp_GetLstCodGrpsUsrBelongs (Gbl.Hierarchy.Crs.CrsCod,GrpTyp->GrpTypCod,
	                        Gbl.Usrs.Me.UsrDat.UsrCod,&LstGrpsIBelong);

   /***** List the groups of this type *****/
   for (NumGrpThisType = 0;
	NumGrpThisType < GrpTyp->NumGrps;
	NumGrpThisType++)
     {
      /* Pointer to group */
      Grp = &(GrpTyp->LstGrps[NumGrpThisType]);

      /* Check if I belong to his group */
      IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsIBelong);

      /* Check if I can select / unselect this group */
      if (IBelongToThisGroup)
	 ICanSelUnselGroup = true;
      else
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_STD:
	    case Rol_NET:
	       ICanSelUnselGroup = (GroupsSelectableByStdsOrNETs == Grp_ALL_GROUPS) ||
	                           IBelongToThisGroup;
	       break;
	    case Rol_TCH:
	    case Rol_DEG_ADM:
	    case Rol_CTR_ADM:
	    case Rol_INS_ADM:
	    case Rol_SYS_ADM:
	       ICanSelUnselGroup = true;	// GroupsSelectable is ignored
	       break;
	    default:
	       ICanSelUnselGroup = false;	// GroupsSelectable is ignored
	       break;
	   }

      /* This group should be checked? */
      if (Gbl.Usrs.ClassPhoto.AllGroups)
         Checked = true;
      else
         for (NumGrpSel = 0, Checked = false;
              NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
              NumGrpSel++)
            if (Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel] == Grp->GrpCod)
              {
               Checked = true;
               break;
              }

      /* Put checkbox to select the group */
      HTM_TR_Begin (NULL);

      if (IBelongToThisGroup)
	 HTM_TD_Begin ("class=\"LM LIGHT_BLUE\"");
      else
	 HTM_TD_Begin ("class=\"LM\"");
      HTM_INPUT_CHECKBOX ("GrpCods",HTM_DONT_SUBMIT_ON_CHANGE,
			  "id=\"Grp%ld\" value=\"%ld\"%s%s",
			  Grp->GrpCod,Grp->GrpCod,
			  Checked ? " checked=\"checked\"" : "",
			  ICanSelUnselGroup ? " onclick=\"checkParent(this,'AllGroups')\"" :
				              " disabled=\"disabled\"");
      HTM_TD_End ();

      Grp_WriteRowGrp (Grp,IBelongToThisGroup);

      HTM_TR_End ();
     }

   /***** Free memory with the list of groups which I belongs to *****/
   Grp_FreeListCodGrp (&LstGrpsIBelong);

   /***** Write rows to select the students who don't belong to any group *****/
   /* To get the students who don't belong to a type of group, use group code -(GrpTyp->GrpTypCod) */
   /* Write checkbox to select the group */
   ICanSelUnselGroup = (Gbl.Usrs.Me.Role.Logged >= Rol_STD);
   if (ICanSelUnselGroup)
     {
      if (Gbl.Usrs.ClassPhoto.AllGroups)
	 Checked = true;
      else
	 for (NumGrpSel = 0, Checked = false;
	      NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	      NumGrpSel++)
	    if (Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel] == -(GrpTyp->GrpTypCod))
	      {
	       Checked = true;
	       break;
	      }
     }
   else
      Checked = false;

   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_CHECKBOX ("GrpCods",HTM_DONT_SUBMIT_ON_CHANGE,
		       "id=\"Grp%ld\" value=\"%ld\"%s"
		       " onclick=\"checkParent(this,'AllGroups')\"",
		       -GrpTyp->GrpTypCod,-GrpTyp->GrpTypCod,
		       ICanSelUnselGroup ? (Checked ? " checked=\"checked\"" : "") :
			                              " disabled=\"disabled\"");
   HTM_TD_End ();

   /* Column closed/open */
   HTM_TD_Begin ("class=\"LM\"");
   HTM_TD_End ();

   /* Group name = students with no group */
   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_LABEL_Begin ("for=\"Grp%ld\"",-GrpTyp->GrpTypCod);
   HTM_Txt (Txt_users_with_no_group);
   HTM_LABEL_End ();
   HTM_TD_End ();

   /* Room */
   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_TD_End ();

   /* Number of students who don't belong to any group of this type */
   for (Role  = Rol_TCH;
	Role >= Rol_STD;
	Role--)
     {
      HTM_TD_Begin ("class=\"DAT CM\"");
      HTM_UnsignedLong (Grp_CountNumUsrsInNoGrpsOfType (Role,GrpTyp->GrpTypCod));
      HTM_TD_End ();
     }

   /* Last empty columns for max. students and vacants */
   HTM_TD_Empty (2);

   HTM_TR_End ();
  }

/*****************************************************************************/
/************** Write a row with the head for list of groups *****************/
/*****************************************************************************/

static void Grp_WriteGrpHead (struct GroupType *GrpTyp)
  {
   extern const char *Txt_Opening_of_groups;
   extern const char *Txt_Group;
   extern const char *Txt_Room;
   extern const char *Txt_Max_BR_students;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Vacants;
   static unsigned UniqueId = 0;
   char *Id;
   Rol_Role_t Role;

   /***** Name of group type *****/
   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("colspan=\"9\" class=\"GRP_TITLE LM\"");
   HTM_BR ();
   HTM_Txt (GrpTyp->GrpTypName);
   if (GrpTyp->MustBeOpened)
     {
      UniqueId++;
      if (asprintf (&Id,"open_time_%u",UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      HTM_BR ();
      HTM_TxtColonNBSP (Txt_Opening_of_groups);
      HTM_SPAN_Begin ("id=\"%s\"",Id);
      HTM_SPAN_End ();
      Dat_WriteLocalDateHMSFromUTC (Id,GrpTyp->OpenTimeUTC,
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				    true,true,true,0x7);
      free (Id);
     }
   HTM_TD_End ();
   HTM_TR_End ();

   /***** Head row with title of each column *****/
   HTM_TR_Begin (NULL);

   HTM_TH_Empty (2);
   HTM_TH (1,1,"LM",Txt_Group);
   HTM_TH (1,1,"LM",Txt_Room);
   for (Role = Rol_TCH;
	Role >= Rol_STD;
	Role--)
      HTM_TH (1,1,"CM",Txt_ROLES_PLURAL_BRIEF_Abc[Role]);
   HTM_TH (1,1,"CM",Txt_Max_BR_students);
   HTM_TH (1,1,"CM",Txt_Vacants);

   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Write a row with the data of a group *********************/
/*****************************************************************************/

static void Grp_WriteRowGrp (struct Group *Grp,bool Highlight)
  {
   extern const char *Txt_Group_X_open;
   extern const char *Txt_Group_X_closed;
   int Vacant;
   Rol_Role_t Role;
   char StrMaxStudents[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Write icon to show if group is open or closed *****/
   if (Highlight)
      HTM_TD_Begin ("class=\"BM LIGHT_BLUE\"");
   else
      HTM_TD_Begin ("class=\"BM\"");
   Ico_PutIconOff (Grp->Open ? "unlock.svg" :
        	               "lock.svg",
	           Str_BuildStringStr (Grp->Open ? Txt_Group_X_open :
						   Txt_Group_X_closed,
				       Grp->GrpName));
   Str_FreeString ();
   HTM_TD_End ();

   /***** Group name *****/
   if (Highlight)
      HTM_TD_Begin ("class=\"LM LIGHT_BLUE\"");
   else
      HTM_TD_Begin ("class=\"LM\"");
   HTM_LABEL_Begin ("for=\"Grp%ld\" class=\"DAT\"",Grp->GrpCod);
   HTM_Txt (Grp->GrpName);
   HTM_LABEL_End ();
   HTM_TD_End ();

   /***** Room *****/
   if (Highlight)
      HTM_TD_Begin ("class=\"DAT LM LIGHT_BLUE\"");
   else
      HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_Txt (Grp->Room.ShrtName);
   HTM_TD_End ();

   /***** Current number of users in this group *****/
   for (Role = Rol_TCH;
	Role >= Rol_STD;
	Role--)
     {
      if (Highlight)
	 HTM_TD_Begin ("class=\"DAT CM LIGHT_BLUE\"");
      else
	 HTM_TD_Begin ("class=\"DAT CM\"");
      HTM_Int (Grp->NumUsrs[Role]);
      HTM_TD_End ();
     }

   /***** Max. number of students in this group *****/
   if (Highlight)
      HTM_TD_Begin ("class=\"DAT CM LIGHT_BLUE\"");
   else
      HTM_TD_Begin ("class=\"DAT CM\"");
   Grp_WriteMaxStds (StrMaxStudents,Grp->MaxStudents);
   HTM_TxtF ("%s&nbsp;",StrMaxStudents);
   HTM_TD_End ();

   /***** Vacants in this group *****/
   if (Highlight)
      HTM_TD_Begin ("class=\"DAT CM LIGHT_BLUE\"");
   else
      HTM_TD_Begin ("class=\"DAT CM\"");
   if (Grp->MaxStudents <= Grp_MAX_STUDENTS_IN_A_GROUP)
     {
      Vacant = (int) Grp->MaxStudents - (int) Grp->NumUsrs[Rol_STD];
      HTM_Unsigned (Vacant > 0 ? (unsigned) Vacant :
        	                 0);
     }
   HTM_TD_End ();
  }

/*****************************************************************************/
/********************* Put a form to create a new group type *****************/
/*****************************************************************************/

static void Grp_PutFormToCreateGroupType (void)
  {
   extern const char *Txt_New_type_of_group;
   extern const char *Txt_It_is_optional_to_choose_a_group;
   extern const char *Txt_It_is_mandatory_to_choose_a_group;
   extern const char *Txt_A_student_can_belong_to_several_groups;
   extern const char *Txt_A_student_can_only_belong_to_one_group;
   extern const char *Txt_The_groups_will_automatically_open;
   extern const char *Txt_The_groups_will_not_automatically_open;
   extern const char *Txt_Create_type_of_group;

   /***** Begin form *****/
   HTM_SECTION_Begin (Grp_NEW_GROUP_TYPE_SECTION_ID);
   Frm_StartFormAnchor (ActNewGrpTyp,Grp_GROUP_TYPES_SECTION_ID);

   /***** Begin box *****/
   Box_BoxTableBegin (NULL,Txt_New_type_of_group,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Grp_WriteHeadingGroupTypes ();

   HTM_TR_Begin (NULL);

   /***** Column to remove group type, disabled here *****/
   HTM_TD_Begin ("class=\"BM\"");
   HTM_TD_End ();

   /***** Name of group type *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("GrpTypName",Grp_MAX_CHARS_GROUP_TYPE_NAME,
		   Gbl.Crs.Grps.GrpTyp.GrpTypName,HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"12\" required=\"required\"");
   HTM_TD_End ();

   /***** Is it mandatory to register in any groups of this type? *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "name=\"MandatoryEnrolment\" style=\"width:150px;\"");
   HTM_OPTION (HTM_Type_STRING,"N",
	       !Gbl.Crs.Grps.GrpTyp.MandatoryEnrolment,false,
	       "%s",Txt_It_is_optional_to_choose_a_group);
   HTM_OPTION (HTM_Type_STRING,"Y",
	       Gbl.Crs.Grps.GrpTyp.MandatoryEnrolment,false,
	       "%s",Txt_It_is_mandatory_to_choose_a_group);
   HTM_SELECT_End ();
   HTM_TD_End ();

   /***** Is it possible to register in multiple groups of this type? *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "name=\"MultipleEnrolment\" style=\"width:150px;\"");
   HTM_OPTION (HTM_Type_STRING,"N",
	       !Gbl.Crs.Grps.GrpTyp.MultipleEnrolment,false,
	       "%s",Txt_A_student_can_only_belong_to_one_group);
   HTM_OPTION (HTM_Type_STRING,"Y",
	       Gbl.Crs.Grps.GrpTyp.MultipleEnrolment,false,
	       "%s",Txt_A_student_can_belong_to_several_groups);
   HTM_SELECT_End ();
   HTM_TD_End ();

   /***** Open time *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"LM\" style=\"width:20px;\"");
   Ico_PutIcon ("clock.svg",
		Gbl.Crs.Grps.GrpTyp.MustBeOpened ? Txt_The_groups_will_automatically_open :
        	                                   Txt_The_groups_will_not_automatically_open,
		Gbl.Crs.Grps.GrpTyp.MustBeOpened ? "CONTEXT_ICO_16x16" :
        	                                   "ICO_HIDDEN CONTEXT_ICO_16x16");
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LM\"");
   Dat_WriteFormClientLocalDateTimeFromTimeUTC ("open_time",
                                                "Open",
                                                Gbl.Crs.Grps.GrpTyp.OpenTimeUTC,
                                                Gbl.Now.Date.Year,
                                                Gbl.Now.Date.Year + 1,
				                Dat_FORM_SECONDS_ON,
				                Dat_HMS_DO_NOT_SET,	// Don't set hour, minute and second
				                false);			// Don't submit on change
   HTM_TD_End ();

   HTM_TR_End ();
   HTM_TABLE_End ();
   HTM_TD_End ();

   /***** Number of groups of this type *****/
   HTM_TD_Begin ("class=\"DAT CM\"");
   HTM_Unsigned (0);	// It's a new group type ==> 0 groups
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_type_of_group);

   /***** End form *****/
   Frm_EndForm ();
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/*********************** Put a form to create a new group ********************/
/*****************************************************************************/

static void Grp_PutFormToCreateGroup (const struct Roo_Rooms *Rooms)
  {
   extern const char *Txt_New_group;
   extern const char *Txt_Group_closed;
   extern const char *Txt_File_zones_disabled;
   extern const char *Txt_No_assigned_room;
   extern const char *Txt_Another_room;
   extern const char *Txt_Create_group;
   unsigned NumGrpTyp;
   unsigned NumRoo;
   Rol_Role_t Role;
   char StrMaxStudents[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Begin form *****/
   HTM_SECTION_Begin (Grp_NEW_GROUP_SECTION_ID);
   Frm_StartFormAnchor (ActNewGrp,Grp_GROUPS_SECTION_ID);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_group,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Grp_WriteHeadingGroups ();

   HTM_TR_Begin (NULL);

   /***** Empty column to remove *****/
   HTM_TD_Begin ("class=\"BM\"");
   HTM_TD_End ();

   /***** Disabled icon to open group *****/
   HTM_TD_Begin ("class=\"BM\"");
   Ico_PutIconOff ("lock.svg",Txt_Group_closed);
   HTM_TD_End ();

   /***** Disabled icon for archive zone *****/
   HTM_TD_Begin ("class=\"BM\"");
   Ico_PutIconOff ("folder-red.svg",Txt_File_zones_disabled);
   HTM_TD_End ();

   /***** Group type *****/
   /* Start selector */
   HTM_TD_Begin ("class=\"CM\"");
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "name=\"GrpTypCod\" style=\"width:100px;\"");

   /* Options for group types */
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	NumGrpTyp++)
      HTM_OPTION (HTM_Type_LONG,&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod,
		  Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod ==
	          Gbl.Crs.Grps.GrpTyp.GrpTypCod,false,
		  "%s",Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName);

   /* End selector */
   HTM_SELECT_End ();
   HTM_TD_End ();

   /***** Group name *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_INPUT_TEXT ("GrpName",Grp_MAX_CHARS_GROUP_NAME,Gbl.Crs.Grps.GrpName,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"20\" required=\"required\"");
   HTM_TD_End ();

   /***** Room *****/
   /* Start selector */
   HTM_TD_Begin ("class=\"CM\"");
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "name=\"RooCod\" style=\"width:100px;\"");

   /* Option for no assigned room */
   HTM_OPTION (HTM_Type_STRING,"-1",Gbl.Crs.Grps.RooCod < 0,false,
	       "%s",Txt_No_assigned_room);

   /* Option for another room */
   HTM_OPTION (HTM_Type_STRING,"0",Gbl.Crs.Grps.RooCod == 0,false,
	       "%s",Txt_Another_room);

   /* Options for rooms */
   for (NumRoo = 0;
	NumRoo < Rooms->Num;
	NumRoo++)
      HTM_OPTION (HTM_Type_LONG,&Rooms->Lst[NumRoo].RooCod,
		  Rooms->Lst[NumRoo].RooCod == Gbl.Crs.Grps.RooCod,false,
		  "%s",Rooms->Lst[NumRoo].ShrtName);

   /* End selector */
   HTM_SELECT_End ();
   HTM_TD_End ();

   /***** Current number of users in this group *****/
   for (Role = Rol_TCH;
	Role >= Rol_STD;
	Role--)
     {
      HTM_TD_Begin ("class=\"DAT CM\"");
      HTM_Unsigned (0);
      HTM_TD_End ();
     }

   /***** Maximum number of students *****/
   HTM_TD_Begin ("class=\"CM\"");
   Grp_WriteMaxStds (StrMaxStudents,Gbl.Crs.Grps.MaxStudents);
   HTM_INPUT_TEXT ("MaxStudents",Cns_MAX_DECIMAL_DIGITS_UINT,StrMaxStudents,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"3\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_group);

   /***** End form *****/
   Frm_EndForm ();
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/*********** Create a list with current group types in this course ***********/
/*****************************************************************************/

void Grp_GetListGrpTypesInThisCrs (Grp_WhichGroupTypes_t WhichGroupTypes)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumGrpTyp;

   if (++Gbl.Crs.Grps.GrpTypes.NestedCalls > 1) // If list is created yet, there's nothing to do
      return;

   /***** Open groups of this course that must be opened
          if open time is in the past *****/
   Grp_OpenGroupsAutomatically ();

   /***** Get group types with groups + groups types without groups from database *****/
   // The tables in the second part of the UNION requires ALIAS in order to LOCK TABLES when registering in groups
   switch (WhichGroupTypes)
     {
      case Grp_ONLY_GROUP_TYPES_WITH_GROUPS:
	 Gbl.Crs.Grps.GrpTypes.Num =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get types of group"
					       " of a course",
				    "SELECT crs_grp_types.GrpTypCod,"
				           "crs_grp_types.GrpTypName,"
				           "crs_grp_types.Mandatory,"
				           "crs_grp_types.Multiple,"
				           "crs_grp_types.MustBeOpened,"
				           "UNIX_TIMESTAMP(crs_grp_types.OpenTime),"
				           "COUNT(grp_groups.GrpCod)"
				     " FROM crs_grp_types,"
				           "grp_groups"
				    " WHERE crs_grp_types.CrsCod=%ld"
				      " AND crs_grp_types.GrpTypCod=grp_groups.GrpTypCod"
				    " GROUP BY crs_grp_types.GrpTypCod"
				    " ORDER BY crs_grp_types.GrpTypName",
				    Gbl.Hierarchy.Crs.CrsCod);
	 break;
      case Grp_ALL_GROUP_TYPES:
	 Gbl.Crs.Grps.GrpTypes.Num =
	 (unsigned) DB_QuerySELECT (&mysql_res,"can not get types of group"
					       " of a course",
				    "(SELECT crs_grp_types.GrpTypCod,"
				            "crs_grp_types.GrpTypName AS GrpTypName,"
				            "crs_grp_types.Mandatory,"
				            "crs_grp_types.Multiple,"
				            "crs_grp_types.MustBeOpened,"
				            "UNIX_TIMESTAMP(crs_grp_types.OpenTime),"
				            "COUNT(grp_groups.GrpCod)"
				      " FROM crs_grp_types,"
				            "grp_groups"
				     " WHERE crs_grp_types.CrsCod=%ld"
				       " AND crs_grp_types.GrpTypCod=grp_groups.GrpTypCod"
				     " GROUP BY crs_grp_types.GrpTypCod)"
				    " UNION "
				    "(SELECT GrpTypCod,"
				            "GrpTypName,"
				            "Mandatory,"
				            "Multiple,"
				            "MustBeOpened,"
				            "UNIX_TIMESTAMP(OpenTime),"
				            "0"
				      " FROM crs_grp_types"
				     " WHERE CrsCod=%ld"
				       " AND GrpTypCod NOT IN"
				           " (SELECT GrpTypCod"
				              " FROM grp_groups))"
				    " ORDER BY GrpTypName",
				    Gbl.Hierarchy.Crs.CrsCod,
				    Gbl.Hierarchy.Crs.CrsCod);
	 break;
     }

   /***** Get group types *****/
   Gbl.Crs.Grps.GrpTypes.NumGrpsTotal = 0;

   if (Gbl.Crs.Grps.GrpTypes.Num)
     {
      /***** Create a list of group types *****/
      if ((Gbl.Crs.Grps.GrpTypes.LstGrpTypes = calloc (Gbl.Crs.Grps.GrpTypes.Num,
                                                       sizeof (*Gbl.Crs.Grps.GrpTypes.LstGrpTypes))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get group types *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
        {
         /* Get next group type */
         row = mysql_fetch_row (mysql_res);

         /* Get group type code (row[0]) */
         if ((Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong type of group.");

         /* Get group type name (row[1]) */
         Str_Copy (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName,row[1],
                   sizeof (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName) - 1);

         /* Is it mandatory to register in any groups of this type? (row[2]) */
         Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MandatoryEnrolment = (row[2][0] == 'Y');

         /* Is it possible to register in multiple groups of this type? (row[3]) */
         Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MultipleEnrolment = (row[3][0] == 'Y');

         /* Groups of this type must be opened? (row[4]) */
         Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MustBeOpened = (row[4][0] == 'Y');

         /* Get open time (row[5] holds the open time UTC) */
         Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].OpenTimeUTC = Dat_GetUNIXTimeFromStr (row[5]);
         Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MustBeOpened &= Grp_CheckIfOpenTimeInTheFuture (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].OpenTimeUTC);

         /* Number of groups of this type (row[6]) */
         if (sscanf (row[6],"%u",&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps) != 1)
            Lay_ShowErrorAndExit ("Wrong number of groups of a type.");

         /* Add number of groups to total number of groups */
         Gbl.Crs.Grps.GrpTypes.NumGrpsTotal += Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps;

	 /* Initialize pointer to the list of groups of this type */
         Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].LstGrps = NULL;

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
   MYSQL_ROW row;
   unsigned NumGrpTypes;
   unsigned NumGrpTyp;
   long GrpTypCod;

   /***** Find group types to be opened *****/
   NumGrpTypes =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get the types of group"
					 " to be opened",
			      "SELECT GrpTypCod FROM crs_grp_types"
			      " WHERE CrsCod=%ld AND MustBeOpened='Y'"
			      " AND OpenTime<=NOW()",
			      Gbl.Hierarchy.Crs.CrsCod);

   for (NumGrpTyp = 0;
        NumGrpTyp < NumGrpTypes;
        NumGrpTyp++)
     {
      /* Get next group TYPE */
      row = mysql_fetch_row (mysql_res);

      if ((GrpTypCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
        {
         /***** Open all the closed groups in this course the must be opened
                and with open time in the past ****/
         DB_QueryUPDATE ("can not open groups",
			 "UPDATE grp_groups"
			   " SET Open='Y'"
		         " WHERE GrpTypCod=%ld"
		           " AND Open='N'",
	                 GrpTypCod);

         /***** To not try to open groups again, set MustBeOpened to false *****/
         DB_QueryUPDATE ("can not update the opening of a type of group",
			 "UPDATE crs_grp_types"
			   " SET MustBeOpened='N'"
		         " WHERE GrpTypCod=%ld",
	                 GrpTypCod);
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********* Create a list with group types and groups in this course **********/
/*****************************************************************************/

void Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_WhichGroupTypes_t WhichGroupTypes)
  {
   unsigned NumGrpTyp;
   unsigned NumGrp;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   struct GroupType *GrpTyp;
   struct Group *Grp;
   Rol_Role_t Role;

   /***** First we get the list of group types *****/
   Grp_GetListGrpTypesInThisCrs (WhichGroupTypes);

   /***** Then we get the list of groups for each group type *****/
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	NumGrpTyp++)
     {
      GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];
      if (GrpTyp->NumGrps)	 // If there are groups of this type...
        {
         /***** Query database *****/
         if ((NumRows = Grp_GetGrpsOfType (GrpTyp->GrpTypCod,&mysql_res)) > 0) // Groups found...
           {
	    // NumRows should be equal to GrpTyp->NumGrps
            GrpTyp->NumGrps = (unsigned) NumRows;

            /***** Create list with groups of this type *****/
            if ((GrpTyp->LstGrps = calloc (GrpTyp->NumGrps,
                                           sizeof (*GrpTyp->LstGrps))) == NULL)
               Lay_NotEnoughMemoryExit ();

            /***** Get the groups of this type *****/
            for (NumGrp = 0;
        	 NumGrp < GrpTyp->NumGrps;
        	 NumGrp++)
              {
               Grp = &(GrpTyp->LstGrps[NumGrp]);

               /* Get next group */
               row = mysql_fetch_row (mysql_res);

               /* Get group code (row[0]) */
               if ((Grp->GrpCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
                  Lay_ShowErrorAndExit ("Wrong code of group.");

               /* Get group name (row[1]) */
               Str_Copy (Grp->GrpName,row[1],sizeof (Grp->GrpName) - 1);

               /* Get room code (row[2]) */
               Grp->Room.RooCod = Str_ConvertStrCodToLongCod (row[2]);

               /* Get room short name (row[3]) */
               if (row[3])	// May be NULL because of LEFT JOIN
		  Str_Copy (Grp->Room.ShrtName,row[3],
			    sizeof (Grp->Room.ShrtName) - 1);
               else		// NULL
        	  Grp->Room.ShrtName[0] = '\0';

               /* Get number of current users in group */
	       for (Role = Rol_TCH;
		    Role >= Rol_STD;
		    Role--)
                  Grp->NumUsrs[Role] = Grp_CountNumUsrsInGrp (Role,Grp->GrpCod);

               /* Get maximum number of students in group (row[4]) */
               Grp->MaxStudents = Grp_ConvertToNumMaxStdsGrp (row[4]);

               /* Get whether group is open ('Y') or closed ('N') (row[5]) */
               Grp->Open = (row[5][0] == 'Y');

               /* Get whether group have file zones ('Y') or not ('N') (row[6]) */
               Grp->FileZones = (row[6][0] == 'Y');
              }
           }
         else	// Error: groups should be found, but really they haven't be found.
		// This never should happen.
            GrpTyp->NumGrps = 0;

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
	    /***** Free memory used for each list of groups (one list for each group type) *****/
	    for (NumGrpTyp = 0;
		 NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
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
            Gbl.Crs.Grps.GrpTypes.Num = 0;
           }
  }

/*****************************************************************************/
/*********** Query the number of groups that hay in this course **************/
/*****************************************************************************/

unsigned Grp_CountNumGrpsInCurrentCrs (void)
  {
   /***** Get number of group in current course from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of groups in this course",
			     "SELECT COUNT(*)"
			      " FROM crs_grp_types,"
			            "grp_groups"
			     " WHERE crs_grp_types.CrsCod=%ld"
			       " AND crs_grp_types.GrpTypCod=grp_groups.GrpTypCod",
			     Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/****************** Count number of groups in a group type *******************/
/*****************************************************************************/

static unsigned Grp_CountNumGrpsInThisCrsOfType (long GrpTypCod)
  {
   /***** Get number of groups of a type from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of groups of a type",
			     "SELECT COUNT(*)"
			      " FROM grp_groups"
			     " WHERE GrpTypCod=%ld",
			     GrpTypCod);
  }

/*****************************************************************************/
/******************** Get groups of a type in this course ********************/
/*****************************************************************************/

unsigned long Grp_GetGrpsOfType (long GrpTypCod,MYSQL_RES **mysql_res)
  {
   /***** Get groups of a type from database *****/
   // Don't use INNER JOIN because there are groups without assigned room
   return DB_QuerySELECT (mysql_res,"can not get groups of a type",
			  "SELECT grp_groups.GrpCod,"
			         "grp_groups.GrpName,"
			         "grp_groups.RooCod,"
			         "roo_rooms.ShortName,"
			         "grp_groups.MaxStudents,"
			         "grp_groups.Open,"
			         "grp_groups.FileZones"
			   " FROM grp_groups"
			   " LEFT JOIN roo_rooms"
			     " ON grp_groups.RooCod=roo_rooms.RooCod"
			  " WHERE grp_groups.GrpTypCod=%ld"
			  " ORDER BY grp_groups.GrpName",
			  GrpTypCod);
  }

/*****************************************************************************/
/******************* Get data of a group type from its code ******************/
/*****************************************************************************/
// GrpTyp->GrpTypCod must have the code of the type of group

static void Grp_GetDataOfGroupTypeByCod (struct GroupType *GrpTyp)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get data of a type of group from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get type of group",
			     "SELECT GrpTypName,"		// row[0]
				    "Mandatory,"		// row[1]
				    "Multiple,"			// row[2]
				    "MustBeOpened,"		// row[3]
				    "UNIX_TIMESTAMP(OpenTime)"	// row[4]
			     " FROM crs_grp_types"
			     " WHERE CrsCod=%ld AND GrpTypCod=%ld",
			     Gbl.Hierarchy.Crs.CrsCod,GrpTyp->GrpTypCod) != 1)
      Lay_ShowErrorAndExit ("Error when getting type of group.");

   /***** Get some data of group type *****/
   row = mysql_fetch_row (mysql_res);
   Str_Copy (GrpTyp->GrpTypName,row[0],sizeof (GrpTyp->GrpTypName) - 1);
   GrpTyp->MandatoryEnrolment = (row[1][0] == 'Y');
   GrpTyp->MultipleEnrolment  = (row[2][0] == 'Y');
   GrpTyp->MustBeOpened       = (row[3][0] == 'Y');
   GrpTyp->OpenTimeUTC = Dat_GetUNIXTimeFromStr (row[4]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Check if a group type has multiple enrolment *****************/
/*****************************************************************************/

static bool Grp_GetMultipleEnrolmentOfAGroupType (long GrpTypCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool MultipleEnrolment;

   /***** Get data of a type of group from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get if type of group"
				  " has multiple enrolment",
		       "SELECT Multiple FROM crs_grp_types"
		       " WHERE GrpTypCod=%ld",
                       GrpTypCod) != 1)
      Lay_ShowErrorAndExit ("Error when getting type of enrolment.");

   /***** Get multiple enrolment *****/
   row = mysql_fetch_row (mysql_res);
   MultipleEnrolment = (row[0][0] == 'Y');

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return MultipleEnrolment;
  }

/*****************************************************************************/
/********************** Get data of a group from its code ********************/
/*****************************************************************************/

void Grp_GetDataOfGroupByCod (struct GroupData *GrpDat)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Reset values *****/
   GrpDat->GrpTypCod             = -1L;
   GrpDat->CrsCod                = -1L;
   GrpDat->GrpTypName[0]         = '\0';
   GrpDat->GrpName[0]            = '\0';
   GrpDat->Room.RooCod      = -1L;
   GrpDat->Room.ShrtName[0] = '\0';
   GrpDat->MaxStudents           = 0;
   GrpDat->Vacant                = 0;
   GrpDat->Open                  = false;
   GrpDat->FileZones             = false;
   GrpDat->MultipleEnrolment     = false;

   if (GrpDat->GrpCod > 0)
     {
      /***** Get data of a group from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a group",
				"SELECT grp_groups.GrpTypCod,"		// row[0]
				       "crs_grp_types.CrsCod,"		// row[1]
				       "crs_grp_types.GrpTypName,"	// row[2]
				       "crs_grp_types.Multiple,"	// row[3]
				       "grp_groups.GrpName,"		// row[4]
				       "grp_groups.RooCod,"		// row[5]
				       "roo_rooms.ShortName,"		// row[6]
				       "grp_groups.MaxStudents,"	// row[7]
				       "grp_groups.Open,"		// row[8]
				       "grp_groups.FileZones"		// row[9]
				 " FROM (grp_groups,"
				        "crs_grp_types)"
				 " LEFT JOIN roo_rooms"
				   " ON grp_groups.RooCod=roo_rooms.RooCod"
				" WHERE grp_groups.GrpCod=%ld"
				  " AND grp_groups.GrpTypCod=crs_grp_types.GrpTypCod",
				GrpDat->GrpCod);

      if (NumRows == 1)
	{
	 /***** Get data of group *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get the code of the group type (row[0]) */
	 if ((GrpDat->GrpTypCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
	    Lay_ShowErrorAndExit ("Wrong code of type of group.");

	 /* Get the code of the course (row[1]) */
	 if ((GrpDat->CrsCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
	    Lay_ShowErrorAndExit ("Wrong code of course.");

	 /* Get the name of the group type (row[2]) */
	 Str_Copy (GrpDat->GrpTypName,row[2],sizeof (GrpDat->GrpTypName) - 1);

	 /* Get whether a student may be in one or multiple groups (row[3]) */
	 GrpDat->MultipleEnrolment = (row[3][0] == 'Y');

	 /* Get the name of the group (row[4]) */
	 Str_Copy (GrpDat->GrpName,row[4],sizeof (GrpDat->GrpName) - 1);

	 /* Get the code of the course (row[5]) */
	 GrpDat->Room.RooCod = Str_ConvertStrCodToLongCod (row[5]);

	 /* Get the name of the room (row[6]) */
	 if (row[6])	// May be NULL because of LEFT JOIN
	    Str_Copy (GrpDat->Room.ShrtName,row[6],
	              sizeof (GrpDat->Room.ShrtName) - 1);
	 else		// NULL
	    GrpDat->Room.ShrtName[0] = '\0';

	 /* Get maximum number of students (row[7]) */
	 GrpDat->MaxStudents = Grp_ConvertToNumMaxStdsGrp (row[7]);

	 /* Get whether group is open or closed (row[8]) */
	 GrpDat->Open = (row[8][0] == 'Y');

	 /* Get whether group has file zones (row[9]) */
	 GrpDat->FileZones = (row[9][0] == 'Y');
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/********************** Get the type of group of a group *********************/
/*****************************************************************************/

static long Grp_GetTypeOfGroupOfAGroup (long GrpCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long GrpTypCod;

   /***** Get data of a group from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get the type of a group",
		       "SELECT GrpTypCod"
		        " FROM grp_groups"
		       " WHERE GrpCod=%ld",
		       GrpCod) != 1)
      Lay_ShowErrorAndExit ("Error when getting group.");

   /***** Get data of group *****/
   row = mysql_fetch_row (mysql_res);
   /* Get the code of the group type (row[0]) */
   if ((GrpTypCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
      Lay_ShowErrorAndExit ("Wrong code of type of group.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return GrpTypCod;
  }

/*****************************************************************************/
/******************** Check if a group exists in database ********************/
/*****************************************************************************/

bool Grp_CheckIfGroupExists (long GrpCod)
  {
   /***** Get if a group exists from database *****/
   return (DB_QueryCOUNT ("can not check if a group exists",
			  "SELECT COUNT(*)"
			   " FROM grp_groups"
			  " WHERE GrpCod=%ld",
			  GrpCod) != 0);
  }

/*****************************************************************************/
/******************* Check if a group belongs to a course ********************/
/*****************************************************************************/

bool Grp_CheckIfGroupBelongsToCourse (long GrpCod,long CrsCod)
  {
   /***** Get if a group exists from database *****/
   return (DB_QueryCOUNT ("can not check if a group belongs to a course",
			  "SELECT COUNT(*)"
			   " FROM grp_groups,"
			         "crs_grp_types"
			  " WHERE grp_groups.GrpCod=%ld"
			    " AND grp_groups.GrpTypCod=crs_grp_types.GrpTypCod"
			    " AND crs_grp_types.CrsCod=%ld",
			  GrpCod,CrsCod) != 0);
  }

/*****************************************************************************/
/********************* Count number of users in a group **********************/
/*****************************************************************************/

unsigned Grp_CountNumUsrsInGrp (Rol_Role_t Role,long GrpCod)
  {
   /***** Get number of students in a group from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of users in a group",
			     "SELECT COUNT(*)"
			      " FROM crs_grp_usr,"
			            "grp_groups,"
			            "crs_grp_types,"
			            "crs_users"
			     " WHERE crs_grp_usr.GrpCod=%ld"
			       " AND crs_grp_usr.GrpCod=grp_groups.GrpCod"
			       " AND grp_groups.GrpTypCod=crs_grp_types.GrpTypCod"
			       " AND crs_grp_types.CrsCod=crs_users.CrsCod"
			       " AND crs_grp_usr.UsrCod=crs_users.UsrCod"
			       " AND crs_users.Role=%u",
			     GrpCod,(unsigned) Role);
  }

/*****************************************************************************/
/*** Count # of users of current course not belonging to groups of a type ****/
/*****************************************************************************/

static unsigned long Grp_CountNumUsrsInNoGrpsOfType (Rol_Role_t Role,long GrpTypCod)
  {
   /***** Get number of users not belonging to groups of a type ******/
   return DB_QueryCOUNT ("can not get the number of users"
	                 " not belonging to groups of a type",
			 "SELECT COUNT(UsrCod)"
			  " FROM crs_users"
			 " WHERE CrsCod=%ld"
			   " AND Role=%u"
			   " AND UsrCod NOT IN"
			       " (SELECT DISTINCT crs_grp_usr.UsrCod"
			          " FROM grp_groups,"
			                "crs_grp_usr"
			         " WHERE grp_groups.GrpTypCod=%ld"
			           " AND grp_groups.GrpCod=crs_grp_usr.GrpCod)",
			 Gbl.Hierarchy.Crs.CrsCod,
			 (unsigned) Role,GrpTypCod);
  }

/*****************************************************************************/
/********* Check if I belong to any groups of a given type I belong **********/
/*****************************************************************************/

static bool Grp_CheckIfIBelongToGrpsOfType (long GrpTypCod)
  {
   unsigned long NumGrps;

   /***** Get a group which I belong to from database *****/
   NumGrps = DB_QueryCOUNT ("can not check if you belong to a group type",
			    "SELECT COUNT(grp_groups.GrpCod)"
			     " FROM grp_groups,crs_grp_usr"
			    " WHERE grp_groups.GrpTypCod=%ld"
			      " AND grp_groups.GrpCod=crs_grp_usr.GrpCod"
			      " AND crs_grp_usr.UsrCod=%ld",	// I belong
			    GrpTypCod,Gbl.Usrs.Me.UsrDat.UsrCod);

   return (NumGrps != 0);
  }

/*****************************************************************************/
/************************ Check if I belong to a group ***********************/
/*****************************************************************************/
// Return true if I belong to group with code GrpCod

void Grp_FlushCacheIBelongToGrp (void)
  {
   Gbl.Cache.IBelongToGrp.GrpCod = -1L;
   Gbl.Cache.IBelongToGrp.IBelong = false;
  }

bool Grp_GetIfIBelongToGrp (long GrpCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (GrpCod <= 0)
      return false;

   /***** 2. Fast check: Is already calculated if I belong to group? *****/
   if (GrpCod == Gbl.Cache.IBelongToGrp.GrpCod)
      return Gbl.Cache.IBelongToGrp.IBelong;

   /***** 3. Slow check: Get if I belong to a group from database *****/
   Gbl.Cache.IBelongToGrp.GrpCod = GrpCod;
   Gbl.Cache.IBelongToGrp.IBelong =
      (DB_QueryCOUNT ("can not check if you belong to a group",
		      "SELECT COUNT(*) FROM crs_grp_usr"
		      " WHERE GrpCod=%ld AND UsrCod=%ld",
		      GrpCod,Gbl.Usrs.Me.UsrDat.UsrCod) != 0);
   return Gbl.Cache.IBelongToGrp.IBelong;
  }

/*****************************************************************************/
/*************** Check if a user belongs to any of my courses ****************/
/*****************************************************************************/

void Grp_FlushCacheUsrSharesAnyOfMyGrpsInCurrentCrs (void)
  {
   Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.UsrCod = -1L;
   Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Shares = false;
  }

bool Grp_CheckIfUsrSharesAnyOfMyGrpsInCurrentCrs (const struct UsrData *UsrDat)
  {
   bool ItsMe;

   /***** 1. Fast check: Am I logged? *****/
   if (!Gbl.Usrs.Me.Logged)
      return false;

   /***** 2. Fast check: Is it a valid user code? *****/
   if (UsrDat->UsrCod <= 0)
      return false;

   /***** 3. Fast check: Is it a course selected? *****/
   if (Gbl.Hierarchy.Crs.CrsCod <= 0)
      return false;

   /***** 4. Fast check: Do I belong to the current course? *****/
   if (!Gbl.Usrs.Me.IBelongToCurrentCrs)
      return false;

   /***** 5. Fast check: It's me? *****/
   ItsMe = Usr_ItsMe (UsrDat->UsrCod);
   if (ItsMe)
      return true;

   /***** 6. Fast check: Is already calculated if user shares
                         any group in the current course with me? *****/
   if (UsrDat->UsrCod == Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.UsrCod)
      return Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Shares;

   /***** 7. Fast / slow check: Does he/she belong to the current course? *****/
   if (!Usr_CheckIfUsrBelongsToCurrentCrs (UsrDat))
     {
      Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.UsrCod = UsrDat->UsrCod;
      Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Shares = false;
      return false;
     }

   /***** 8. Fast check: Course has groups? *****/
   if (!Gbl.Crs.Grps.NumGrps)
     {
      Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.UsrCod = UsrDat->UsrCod;
      Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Shares = true;
      return true;
     }

   // Course has groups

   /***** 9. Slow check: Get if user shares any group in this course with me from database *****/
   /* Check if user shares any group with me */
   Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.UsrCod = UsrDat->UsrCod;
   Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Shares =
      (DB_QueryCOUNT ("can not check if a user shares any group"
		      " in the current course with you",
		      "SELECT COUNT(*)"
		       " FROM crs_grp_usr"
		      " WHERE UsrCod=%ld"
		        " AND GrpCod IN"
		            " (SELECT crs_grp_usr.GrpCod"
		               " FROM crs_grp_usr,"
		                     "grp_groups,"
		                     "crs_grp_types"
		              " WHERE crs_grp_usr.UsrCod=%ld"
		                " AND crs_grp_usr.GrpCod=grp_groups.GrpCod"
		                " AND grp_groups.GrpTypCod=crs_grp_types.GrpTypCod"
		                " AND crs_grp_types.CrsCod=%ld)",
		      UsrDat->UsrCod,
		      Gbl.Usrs.Me.UsrDat.UsrCod,
		      Gbl.Hierarchy.Crs.CrsCod) != 0);
   return Gbl.Cache.UsrSharesAnyOfMyGrpsInCurrentCrs.Shares;
  }

/*****************************************************************************/
/**** Get if any group in group-type/this-course is open and has vacants *****/
/*****************************************************************************/
// If GrpTypCod >  0 ==> restrict to the given group type, mandatory or not
// If GrpTypCod <= 0 ==> all mandatory group types in the current course

bool Grp_GetIfAvailableGrpTyp (long GrpTypCod)
  {
   unsigned NumGrpTypes;
   char *SubQueryGrpTypes;

   if (GrpTypCod > 0)	// restrict to the given group type, mandatory or not
     {
      if (asprintf (&SubQueryGrpTypes,"crs_grp_types.GrpTypCod=%ld",
	            GrpTypCod) < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   else			// all mandatory group types in the current course
     {
      if (asprintf (&SubQueryGrpTypes,"crs_grp_types.CrsCod=%ld"
	                              " AND crs_grp_types.Mandatory='Y'",
	            Gbl.Hierarchy.Crs.CrsCod) < 0)
	 Lay_NotEnoughMemoryExit ();
     }

   /***** Get the number of types of group in this course
          with one or more open groups with vacants, from database *****/
   NumGrpTypes =
   (unsigned) DB_QueryCOUNT ("can not check if there has available mandatory group types",
			     "SELECT COUNT(GrpTypCod) FROM "
			     "("

			     // Available mandatory groups with students
	                     "SELECT GrpTypCod FROM"
			     " ("
			     "SELECT crs_grp_types.GrpTypCod AS GrpTypCod,"
			            "COUNT(*) AS NumStudents,"
			            "grp_groups.MaxStudents as MaxStudents"
			      " FROM crs_grp_types,"
			            "grp_groups,"
			            "crs_grp_usr,"
			            "crs_users"
			     " WHERE %s"				// Which group types?
			       " AND crs_grp_types.GrpTypCod=grp_groups.GrpTypCod"
			       " AND grp_groups.Open='Y'"		// Open
			       " AND grp_groups.MaxStudents>0"		// Admits students
			       " AND crs_grp_types.CrsCod=crs_users.CrsCod"
			       " AND grp_groups.GrpCod=crs_grp_usr.GrpCod"
			       " AND crs_grp_usr.UsrCod=crs_users.UsrCod"
			       " AND crs_users.Role=%u"			// Student
			     " GROUP BY grp_groups.GrpCod"
			     " HAVING NumStudents<MaxStudents"		// Not full
			     ") AS available_grp_types_with_stds"

			     " UNION "

			     // Available mandatory groups...
			     "SELECT crs_grp_types.GrpTypCod AS GrpTypCod"
			      " FROM crs_grp_types,"
			            "grp_groups"
			     " WHERE %s"				// Which group types?
			       " AND crs_grp_types.GrpTypCod=grp_groups.GrpTypCod"
			       " AND grp_groups.Open='Y'"			// Open
			       " AND grp_groups.MaxStudents>0"		// Admits students
			     // ...without students
			       " AND grp_groups.GrpCod NOT IN"
			           " (SELECT crs_grp_usr.GrpCod"
			              " FROM crs_users,"
			                    "crs_grp_usr"
			             " WHERE crs_users.CrsCod=%ld"
			               " AND crs_users.Role=%u"		// Student
			               " AND crs_users.UsrCod=crs_grp_usr.UsrCod)"

			     ") AS available_grp_types"

			     // ...to which I don't belong
			     " WHERE GrpTypCod NOT IN"
			           " (SELECT crs_grp_types.GrpTypCod"
			              " FROM crs_grp_types,"
			                    "grp_groups,"
			                    "crs_grp_usr"
			             " WHERE %s"				// Which group types?
			               " AND crs_grp_types.GrpTypCod=grp_groups.GrpTypCod"
			               " AND grp_groups.Open='Y'"		// Open
			               " AND grp_groups.MaxStudents>0"		// Admits students
			               " AND grp_groups.GrpCod=crs_grp_usr.GrpCod"
			               " AND crs_grp_usr.UsrCod=%ld)",		// I belong

			     SubQueryGrpTypes,(unsigned) Rol_STD,
			     SubQueryGrpTypes,
			     Gbl.Hierarchy.Crs.CrsCod,(unsigned) Rol_STD,
			     SubQueryGrpTypes,Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Free allocated memory for subquery *****/
   free (SubQueryGrpTypes);

   return (NumGrpTypes != 0);
  }

/*****************************************************************************/
/****** Query list of group codes of a type to which a user belongs to *******/
/*****************************************************************************/
// If CrsCod    < 0 ==> get the groups from all the user's courses
// If GrpTypCod < 0 ==> get the groups of any type

static void Grp_GetLstCodGrpsUsrBelongs (long CrsCod,long GrpTypCod,
                                         long UsrCod,struct ListCodGrps *LstGrps)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrp;

   /***** Get groups which a user belong to from database *****/
   if (CrsCod < 0)		// Query the groups from all the user's courses
      LstGrps->NumGrps =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get the groups"
					    " which a user belongs to",
				 "SELECT GrpCod"
				 " FROM crs_grp_usr"
				 " WHERE UsrCod=%ld",	// Groups will be unordered
				 UsrCod);
   else if (GrpTypCod < 0)	// Query the groups of any type in the course
      LstGrps->NumGrps =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get the groups"
					    " which a user belongs to",
				 "SELECT grp_groups.GrpCod"
				  " FROM crs_grp_types,"
				        "grp_groups,"
				        "crs_grp_usr"
				 " WHERE crs_grp_types.CrsCod=%ld"
				   " AND crs_grp_types.GrpTypCod=grp_groups.GrpTypCod"
				   " AND grp_groups.GrpCod=crs_grp_usr.GrpCod"
				   " AND crs_grp_usr.UsrCod=%ld"
				 " ORDER BY crs_grp_types.GrpTypName,"
				           "grp_groups.GrpName",
				 Gbl.Hierarchy.Crs.CrsCod,
				 UsrCod);
   else				// Query only the groups of specified type in the course
      LstGrps->NumGrps =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get the groups"
					    " which a user belongs to",
				 "SELECT grp_groups.GrpCod"
				  " FROM crs_grp_types,"
				        "grp_groups,"
				        "crs_grp_usr"
				 " WHERE crs_grp_types.CrsCod=%ld"
				   " AND crs_grp_types.GrpTypCod=%ld"
				   " AND crs_grp_types.GrpTypCod=grp_groups.GrpTypCod"
				   " AND grp_groups.GrpCod=crs_grp_usr.GrpCod"
				   " AND crs_grp_usr.UsrCod=%ld"
				 " ORDER BY grp_groups.GrpName",
				 Gbl.Hierarchy.Crs.CrsCod,
				 GrpTypCod,
				 UsrCod);

   /***** Get the groups *****/
   if (LstGrps->NumGrps)
     {
      /***** Create a list of groups the user belongs to *****/
      if ((LstGrps->GrpCods = calloc (LstGrps->NumGrps,
                                      sizeof (*LstGrps->GrpCods))) == NULL)
         Lay_NotEnoughMemoryExit ();
      for (NumGrp = 0;
	   NumGrp < LstGrps->NumGrps;
	   NumGrp++)
        {
         row = mysql_fetch_row (mysql_res);

         /* Get the code of group (row[0]) */
         if ((LstGrps->GrpCods[NumGrp] = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of group.");
        }
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
   MYSQL_ROW row;
   unsigned NumGrp;

   /***** Get groups which I belong to from database *****/
   LstGrps->NumGrps =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get the groups"
					 " which you belong to",
			      "SELECT grp_groups.GrpCod"
			       " FROM crs_grp_types,"
			             "grp_groups,"
			             "crs_grp_usr"
			      " WHERE crs_grp_types.CrsCod=%ld"
			        " AND crs_grp_types.GrpTypCod=grp_groups.GrpTypCod"
			        " AND grp_groups.FileZones='Y'"
			        " AND grp_groups.GrpCod=crs_grp_usr.GrpCod"
			        " AND crs_grp_usr.UsrCod=%ld"
			      " ORDER BY crs_grp_types.GrpTypName,"
			                "grp_groups.GrpName",
			      Gbl.Hierarchy.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Get the groups *****/
   if (LstGrps->NumGrps)
     {
      /***** Create a list of groups I belong to *****/
      if ((LstGrps->GrpCods = calloc (LstGrps->NumGrps,
                                      sizeof (*LstGrps->GrpCods))) == NULL)
         Lay_NotEnoughMemoryExit ();
      for (NumGrp = 0;
	   NumGrp < LstGrps->NumGrps;
	   NumGrp++)
        {
         row = mysql_fetch_row (mysql_res);

         /* Get the code of group (row[0]) */
         if ((LstGrps->GrpCods[NumGrp] = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of group.");
        }
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

void Grp_GetNamesGrpsStdBelongsTo (long GrpTypCod,long UsrCod,char *GroupNames)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;
   size_t MaxLength = (Grp_MAX_BYTES_GROUP_NAME + 2) *
		      Gbl.Crs.Grps.GrpTypes.NumGrpsTotal;

   /***** Get the names of groups which a user belongs to, from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get the names of groups"
				        " a user belongs to",
			     "SELECT grp_groups.GrpName"
			      " FROM grp_groups,crs_grp_usr"
			     " WHERE grp_groups.GrpTypCod=%ld"
			       " AND grp_groups.GrpCod=crs_grp_usr.GrpCod"
			       " AND crs_grp_usr.UsrCod=%ld"
			     " ORDER BY grp_groups.GrpName",
			     GrpTypCod,UsrCod);

   /***** Get the groups *****/
   GroupNames[0] = '\0';
   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      /* Get next group */
      row = mysql_fetch_row (mysql_res);

      /* El group name in row[0] */
      if (NumRow)
         Str_Concat (GroupNames,", ",MaxLength);
      Str_Concat (GroupNames,row[0],MaxLength);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Receive form to create a new group type ******************/
/*****************************************************************************/

void Grp_ReceiveFormNewGrpTyp (void)
  {
   extern const char *Txt_The_type_of_group_X_already_exists;
   extern const char *Txt_Created_new_type_of_group_X;
   extern const char *Txt_You_must_specify_the_name_of_the_new_type_of_group;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_TYPE_NAME];

   /***** Get parameters from form *****/
   /* Get the name of group type */
   Par_GetParToText ("GrpTypName",Gbl.Crs.Grps.GrpTyp.GrpTypName,
                     Grp_MAX_BYTES_GROUP_TYPE_NAME);

   /* Get whether it is mandatory to regisrer in any group of this type */
   Gbl.Crs.Grps.GrpTyp.MandatoryEnrolment = Par_GetParToBool ("MandatoryEnrolment");

   /* Get whether it is possible to register in multiple groups of this type */
   Gbl.Crs.Grps.GrpTyp.MultipleEnrolment = Par_GetParToBool ("MultipleEnrolment");

   /* Get open time */
   Gbl.Crs.Grps.GrpTyp.OpenTimeUTC = Dat_GetTimeUTCFromForm ("OpenTimeUTC");
   Gbl.Crs.Grps.GrpTyp.MustBeOpened = Grp_CheckIfOpenTimeInTheFuture (Gbl.Crs.Grps.GrpTyp.OpenTimeUTC);

   if (Gbl.Crs.Grps.GrpTyp.GrpTypName[0])	// If there's a group type name
     {
      /***** If name of group type was in database... *****/
      if (Grp_CheckIfGroupTypeNameExists (Gbl.Crs.Grps.GrpTyp.GrpTypName,-1L))
        {
         AlertType = Ale_WARNING;
         snprintf (AlertTxt,sizeof (AlertTxt),
                   Txt_The_type_of_group_X_already_exists,
                   Gbl.Crs.Grps.GrpTyp.GrpTypName);
        }
      else	// Add new group type to database
	{
         Grp_CreateGroupType ();

         AlertType = Ale_SUCCESS;
	 snprintf (AlertTxt,sizeof (AlertTxt),
	           Txt_Created_new_type_of_group_X,
		   Gbl.Crs.Grps.GrpTyp.GrpTypName);
	}
     }
   else	// If there is not a group type name
     {
      AlertType = Ale_WARNING;
      Str_Copy (AlertTxt,Txt_You_must_specify_the_name_of_the_new_type_of_group,
		sizeof (AlertTxt) - 1);
     }

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (AlertType,AlertTxt,
                              Ale_INFO,NULL);
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
   return (OpenTimeUTC > Gbl.StartExecutionTimeUTC);
  }

/*****************************************************************************/
/******************** Receive form to create a new group *********************/
/*****************************************************************************/

void Grp_ReceiveFormNewGrp (void)
  {
   extern const char *Txt_The_group_X_already_exists;
   extern const char *Txt_Created_new_group_X;
   extern const char *Txt_You_must_specify_the_name_of_the_new_group;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get parameters from form *****/
   if ((Gbl.Crs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) > 0) // Group type valid
     {
      /* Get group name */
      Par_GetParToText ("GrpName",Gbl.Crs.Grps.GrpName,
                        Grp_MAX_BYTES_GROUP_NAME);

      /* Get room */
      Gbl.Crs.Grps.RooCod = Roo_GetParamRooCod ();

      /* Get maximum number of students */
      Gbl.Crs.Grps.MaxStudents = (unsigned)
	                                Par_GetParToUnsignedLong ("MaxStudents",
                                                                  0,
                                                                  Grp_MAX_STUDENTS_IN_A_GROUP,
                                                                  Grp_NUM_STUDENTS_NOT_LIMITED);

      if (Gbl.Crs.Grps.GrpName[0])	// If there's a group name
        {
         /***** If name of group was in database... *****/
         if (Grp_CheckIfGroupNameExists (Gbl.Crs.Grps.GrpTyp.GrpTypCod,Gbl.Crs.Grps.GrpName,-1L))
           {
            AlertType = Ale_WARNING;
            snprintf (AlertTxt,sizeof (AlertTxt),
	              Txt_The_group_X_already_exists,
                      Gbl.Crs.Grps.GrpName);
           }
         else	// Add new group to database
           {
            Grp_CreateGroup ();

	    /* Write success message */
            AlertType = Ale_SUCCESS;
	    snprintf (AlertTxt,sizeof (AlertTxt),
	              Txt_Created_new_group_X,
		      Gbl.Crs.Grps.GrpName);
           }
        }
      else	// If there is not a group name
        {
         AlertType = Ale_ERROR;
	 Str_Copy (AlertTxt,Txt_You_must_specify_the_name_of_the_new_group,
		   sizeof (AlertTxt) - 1);
        }
     }
   else	// Invalid group type
     {
      AlertType = Ale_ERROR;
      Str_Copy (AlertTxt,"Wrong type of group.",
		sizeof (AlertTxt) - 1);
     }

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (Ale_INFO,NULL,
                              AlertType,AlertTxt);
  }

/*****************************************************************************/
/******************* Check if name of group type exists **********************/
/*****************************************************************************/

static bool Grp_CheckIfGroupTypeNameExists (const char *GrpTypName,long GrpTypCod)
  {
   /***** Get number of group types with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of type of group"
			  " already existed",
			  "SELECT COUNT(*) FROM crs_grp_types"
			  " WHERE CrsCod=%ld AND GrpTypName='%s'"
			  " AND GrpTypCod<>%ld",
			  Gbl.Hierarchy.Crs.CrsCod,GrpTypName,GrpTypCod) != 0);
  }

/*****************************************************************************/
/************************ Check if name of group exists **********************/
/*****************************************************************************/

static bool Grp_CheckIfGroupNameExists (long GrpTypCod,const char *GrpName,long GrpCod)
  {
   /***** Get number of groups with a type and a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of group already existed",
			  "SELECT COUNT(*)"
			   " FROM grp_groups"
			  " WHERE GrpTypCod=%ld"
			    " AND GrpName='%s'"
			    " AND GrpCod<>%ld",
			  GrpTypCod,GrpName,GrpCod) != 0);
  }

/*****************************************************************************/
/************************** Create a new group type **************************/
/*****************************************************************************/

static void Grp_CreateGroupType (void)
  {
   /***** Create a new group type *****/
   Gbl.Crs.Grps.GrpTyp.GrpTypCod =
   DB_QueryINSERTandReturnCode ("can not create type of group",
				"INSERT INTO crs_grp_types"
				" (CrsCod,GrpTypName,Mandatory,Multiple,MustBeOpened,OpenTime)"
				" VALUES"
				" (%ld,'%s','%c','%c','%c',FROM_UNIXTIME(%ld))",
				Gbl.Hierarchy.Crs.CrsCod,Gbl.Crs.Grps.GrpTyp.GrpTypName,
				Gbl.Crs.Grps.GrpTyp.MandatoryEnrolment ? 'Y' :
										'N',
				Gbl.Crs.Grps.GrpTyp.MultipleEnrolment ? 'Y' :
									       'N',
				Gbl.Crs.Grps.GrpTyp.MustBeOpened ? 'Y' :
									  'N',
				(long) Gbl.Crs.Grps.GrpTyp.OpenTimeUTC);
  }

/*****************************************************************************/
/***************************** Create a new group ****************************/
/*****************************************************************************/

static void Grp_CreateGroup (void)
  {
   /***** Create a new group *****/
   DB_QueryINSERT ("can not create group",
		   "INSERT INTO grp_groups"
		   " (GrpTypCod,GrpName,RooCod,MaxStudents,Open,FileZones)"
		   " VALUES"
		   " (%ld,'%s',%ld,%u,'N','N')",
	           Gbl.Crs.Grps.GrpTyp.GrpTypCod,
	           Gbl.Crs.Grps.GrpName,
	           Gbl.Crs.Grps.RooCod,
	           Gbl.Crs.Grps.MaxStudents);
  }

/*****************************************************************************/
/********************* Request removing of a group type **********************/
/*****************************************************************************/

void Grp_ReqRemGroupType (void)
  {
   unsigned NumGrps;

   /***** Get the code of the group type *****/
   if ((Gbl.Crs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /***** Check if this group type has groups *****/
   if ((NumGrps = Grp_CountNumGrpsInThisCrsOfType (Gbl.Crs.Grps.GrpTyp.GrpTypCod)))	// Group type has groups ==> Ask for confirmation
      Grp_AskConfirmRemGrpTypWithGrps (NumGrps);
   else	// Group type has no groups ==> remove directly
      Grp_RemoveGroupTypeCompletely ();
  }

/*****************************************************************************/
/************************* Request removal of a group ************************/
/*****************************************************************************/

void Grp_ReqRemGroup (void)
  {
   /***** Get group code *****/
   if ((Gbl.Crs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /***** Confirm removing *****/
   Grp_AskConfirmRemGrp ();
  }

/*****************************************************************************/
/********** Ask for confirmation to remove a group type with groups **********/
/*****************************************************************************/

static void Grp_AskConfirmRemGrpTypWithGrps (unsigned NumGrps)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_type_of_group_X_1_group_;
   extern const char *Txt_Do_you_really_want_to_remove_the_type_of_group_X_Y_groups_;
   extern const char *Txt_Remove_type_of_group;

   /***** Get data of the group type from database *****/
   Grp_GetDataOfGroupTypeByCod (&Gbl.Crs.Grps.GrpTyp);

   /***** Begin section to edit group types *****/
   Grp_ReqEditGroupsInternal0 ();

   /***** Show question and button to remove type of group *****/
   if (NumGrps == 1)
      Ale_ShowAlertAndButton (ActRemGrpTyp,Grp_GROUP_TYPES_SECTION_ID,NULL,
			      Grp_PutParamGrpTypCod,&Gbl.Crs.Grps.GrpTyp.GrpTypCod,
			      Btn_REMOVE_BUTTON,Txt_Remove_type_of_group,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_type_of_group_X_1_group_,
                              Gbl.Crs.Grps.GrpTyp.GrpTypName);
   else
      Ale_ShowAlertAndButton (ActRemGrpTyp,Grp_GROUP_TYPES_SECTION_ID,NULL,
			      Grp_PutParamGrpTypCod,&Gbl.Crs.Grps.GrpTyp.GrpTypCod,
			      Btn_REMOVE_BUTTON,Txt_Remove_type_of_group,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_type_of_group_X_Y_groups_,
                              Gbl.Crs.Grps.GrpTyp.GrpTypName,NumGrps);

   /***** Show the form to edit group types and groups again *****/
   Grp_ReqEditGroupsInternal1 (Ale_INFO,NULL);
   Grp_ReqEditGroupsInternal2 (Ale_INFO,NULL);
  }

/*****************************************************************************/
/******************* Ask for confirmation to remove a group ******************/
/*****************************************************************************/

static void Grp_AskConfirmRemGrp (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_group_X;
   extern const char *Txt_Do_you_really_want_to_remove_the_group_X_1_student_;
   extern const char *Txt_Do_you_really_want_to_remove_the_group_X_Y_students_;
   extern const char *Txt_Remove_group;
   struct GroupData GrpDat;
   unsigned NumStds;

   /***** Get name of the group from database *****/
   GrpDat.GrpCod = Gbl.Crs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Count number of students in group *****/
   NumStds = Grp_CountNumUsrsInGrp (Rol_STD,Gbl.Crs.Grps.GrpCod);

   /***** Show the form to edit group types again *****/
   Grp_ReqEditGroupsInternal0 ();
   Grp_ReqEditGroupsInternal1 (Ale_INFO,NULL);

   /***** Show question and button to remove group *****/
   if (NumStds == 0)
      Ale_ShowAlertAndButton (ActRemGrp,Grp_GROUPS_SECTION_ID,NULL,
			      Grp_PutParamGrpCod,&Gbl.Crs.Grps.GrpCod,
			      Btn_REMOVE_BUTTON,Txt_Remove_group,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_group_X,
                              GrpDat.GrpName);
   else if (NumStds == 1)
      Ale_ShowAlertAndButton (ActRemGrp,Grp_GROUPS_SECTION_ID,NULL,
			      Grp_PutParamGrpCod,&Gbl.Crs.Grps.GrpCod,
			      Btn_REMOVE_BUTTON,Txt_Remove_group,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_group_X_1_student_,
                              GrpDat.GrpName);
   else
      Ale_ShowAlertAndButton (ActRemGrp,Grp_GROUPS_SECTION_ID,NULL,
			      Grp_PutParamGrpCod,&Gbl.Crs.Grps.GrpCod,
			      Btn_REMOVE_BUTTON,Txt_Remove_group,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_group_X_Y_students_,
                              GrpDat.GrpName,NumStds);

   /***** Show the form to edit groups again *****/
   Grp_ReqEditGroupsInternal2 (Ale_INFO,NULL);
  }

/*****************************************************************************/
/**************************** Remove a group type ****************************/
/*****************************************************************************/

void Grp_RemoveGroupType (void)
  {
   /***** Get param with code of group type *****/
   if ((Gbl.Crs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of type of group is missing.");

   /***** Remove group type and its groups *****/
   Grp_RemoveGroupTypeCompletely ();
  }

/*****************************************************************************/
/******************************* Remove a group ******************************/
/*****************************************************************************/

void Grp_RemoveGroup (void)
  {
   /***** Get param with group code *****/
   if ((Gbl.Crs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /***** Remove group *****/
   Grp_RemoveGroupCompletely ();
  }

/*****************************************************************************/
/********************* Remove a group type from database *********************/
/*****************************************************************************/

static void Grp_RemoveGroupTypeCompletely (void)
  {
   extern const char *Txt_Type_of_group_X_removed;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_TYPE_NAME];

   /***** Get name and type of the group from database *****/
   Grp_GetDataOfGroupTypeByCod (&Gbl.Crs.Grps.GrpTyp);

   /***** Remove file zones of all the groups of this type *****/
   Brw_RemoveZonesOfGroupsOfType (Gbl.Crs.Grps.GrpTyp.GrpTypCod);

   /***** Remove the associations of assignments to groups of this type *****/
   Asg_RemoveGroupsOfType (Gbl.Crs.Grps.GrpTyp.GrpTypCod);

   /***** Remove the associations of attendance events to groups of this type *****/
   Att_RemoveGroupsOfType (Gbl.Crs.Grps.GrpTyp.GrpTypCod);

   /***** Remove the associations of matches to groups of this type *****/
   Mch_RemoveGroupsOfType (Gbl.Crs.Grps.GrpTyp.GrpTypCod);

   /***** Remove the associations of surveys to groups of this type *****/
   Svy_RemoveGroupsOfType (Gbl.Crs.Grps.GrpTyp.GrpTypCod);

   /***** Change all groups of this type in course timetable *****/
   DB_QueryUPDATE ("can not update all groups of a type in course timetable",
		   "UPDATE tmt_courses"
		     " SET GrpCod=-1"
		   " WHERE GrpCod IN"
			 " (SELECT GrpCod"
			   " FROM grp_groups"
			  " WHERE GrpTypCod=%ld)",
                   Gbl.Crs.Grps.GrpTyp.GrpTypCod);

   /***** Remove all the students in groups of this type *****/
   DB_QueryDELETE ("can not remove users from all groups of a type",
		   "DELETE FROM crs_grp_usr"
		   " WHERE GrpCod IN"
		         " (SELECT GrpCod"
		            " FROM grp_groups"
		           " WHERE GrpTypCod=%ld)",
                   Gbl.Crs.Grps.GrpTyp.GrpTypCod);

   /***** Remove all the groups of this type *****/
   DB_QueryDELETE ("can not remove groups of a type",
		   "DELETE FROM grp_groups"
		   " WHERE GrpTypCod=%ld",
                   Gbl.Crs.Grps.GrpTyp.GrpTypCod);

   /***** Remove the group type *****/
   DB_QueryDELETE ("can not remove a type of group",
		   "DELETE FROM crs_grp_types"
		   " WHERE GrpTypCod=%ld",
                   Gbl.Crs.Grps.GrpTyp.GrpTypCod);

   /***** Create message to show the change made *****/
   snprintf (AlertTxt,sizeof (AlertTxt),Txt_Type_of_group_X_removed,
             Gbl.Crs.Grps.GrpTyp.GrpTypName);

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (Ale_SUCCESS,AlertTxt,
                              Ale_INFO,NULL);
  }

/*****************************************************************************/
/******* Remove a group from data base and remove group common zone **********/
/*****************************************************************************/

static void Grp_RemoveGroupCompletely (void)
  {
   extern const char *Txt_Group_X_removed;
   struct GroupData GrpDat;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get name and type of the group from database *****/
   GrpDat.GrpCod = Gbl.Crs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Remove file zones of this group *****/
   Brw_RemoveGrpZones (Gbl.Hierarchy.Crs.CrsCod,GrpDat.GrpCod);

   /***** Remove this group from all assignments *****/
   Asg_RemoveGroup (GrpDat.GrpCod);

   /***** Remove this group from all attendance events *****/
   Att_RemoveGroup (GrpDat.GrpCod);

   /***** Remove this group from all matches *****/
   Mch_RemoveGroup (GrpDat.GrpCod);

   /***** Remove this group from all surveys *****/
   Svy_RemoveGroup (GrpDat.GrpCod);

   /***** Change this group in course timetable *****/
   DB_QueryUPDATE ("can not update a group in course timetable",
		   "UPDATE tmt_courses"
		     " SET GrpCod=-1"
		   " WHERE GrpCod=%ld",
                   Gbl.Crs.Grps.GrpCod);

   /***** Remove all the students in this group *****/
   DB_QueryDELETE ("can not remove users from a group",
		   "DELETE FROM crs_grp_usr WHERE GrpCod=%ld",
                   Gbl.Crs.Grps.GrpCod);

   /***** Remove the group *****/
   DB_QueryDELETE ("can not remove a group",
		   "DELETE FROM grp_groups"
		   " WHERE GrpCod=%ld",
                   Gbl.Crs.Grps.GrpCod);

   /***** Create message to show the change made *****/
   snprintf (AlertTxt,sizeof (AlertTxt),Txt_Group_X_removed,
	     GrpDat.GrpName);

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (Ale_INFO,NULL,
                              Ale_SUCCESS,AlertTxt);
  }

/*****************************************************************************/
/******************************* Open a group ********************************/
/*****************************************************************************/

void Grp_OpenGroup (void)
  {
   extern const char *Txt_The_group_X_is_now_open;
   struct GroupData GrpDat;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get group code *****/
   if ((Gbl.Crs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /***** Get group data from database *****/
   GrpDat.GrpCod = Gbl.Crs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Update the table of groups changing open/close status *****/
   DB_QueryUPDATE ("can not open a group",
		   "UPDATE grp_groups"
		     " SET Open='Y'"
		   " WHERE GrpCod=%ld",
                   Gbl.Crs.Grps.GrpCod);

   /***** Create message to show the change made *****/
   snprintf (AlertTxt,sizeof (AlertTxt),Txt_The_group_X_is_now_open,
	     GrpDat.GrpName);

   /***** Show the form again *****/
   Gbl.Crs.Grps.Open = true;
   Grp_ReqEditGroupsInternal (Ale_INFO,NULL,
                              Ale_SUCCESS,AlertTxt);
  }

/*****************************************************************************/
/******************************* Close a group *******************************/
/*****************************************************************************/

void Grp_CloseGroup (void)
  {
   extern const char *Txt_The_group_X_is_now_closed;
   struct GroupData GrpDat;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get group code *****/
   if ((Gbl.Crs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /***** Get group data from database *****/
   GrpDat.GrpCod = Gbl.Crs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Update the table of groups changing open/close status *****/
   DB_QueryUPDATE ("can not close a group",
		   "UPDATE grp_groups"
		     " SET Open='N'"
		   " WHERE GrpCod=%ld",
                   Gbl.Crs.Grps.GrpCod);

   /***** Create message to show the change made *****/
   snprintf (AlertTxt,sizeof (AlertTxt),Txt_The_group_X_is_now_closed,
	     GrpDat.GrpName);

   /***** Show the form again *****/
   Gbl.Crs.Grps.Open = false;
   Grp_ReqEditGroupsInternal (Ale_INFO,NULL,
                              Ale_SUCCESS,AlertTxt);
  }

/*****************************************************************************/
/************************ Enable file zones of a group ***********************/
/*****************************************************************************/

void Grp_EnableFileZonesGrp (void)
  {
   extern const char *Txt_File_zones_of_the_group_X_are_now_enabled;
   struct GroupData GrpDat;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get group code *****/
   if ((Gbl.Crs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /***** Get group data from database *****/
   GrpDat.GrpCod = Gbl.Crs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Update the table of groups changing file zones status *****/
   DB_QueryUPDATE ("can not enable file zones of a group",
		   "UPDATE grp_groups"
		     " SET FileZones='Y'"
		   " WHERE GrpCod=%ld",
                   Gbl.Crs.Grps.GrpCod);

   /***** Create message to show the change made *****/
   snprintf (AlertTxt,sizeof (AlertTxt),
             Txt_File_zones_of_the_group_X_are_now_enabled,
             GrpDat.GrpName);

   /***** Show the form again *****/
   Gbl.Crs.Grps.FileZones = true;
   Grp_ReqEditGroupsInternal (Ale_INFO,NULL,
                              Ale_SUCCESS,AlertTxt);
  }

/*****************************************************************************/
/*********************** Disable file zones of a group ***********************/
/*****************************************************************************/

void Grp_DisableFileZonesGrp (void)
  {
   extern const char *Txt_File_zones_of_the_group_X_are_now_disabled;
   struct GroupData GrpDat;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get group code *****/
   if ((Gbl.Crs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /***** Get group data from database *****/
   GrpDat.GrpCod = Gbl.Crs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Update the table of groups changing file zones status *****/
   DB_QueryUPDATE ("can not disable file zones of a group",
		   "UPDATE grp_groups"
		     " SET FileZones='N'"
		   " WHERE GrpCod=%ld",
                   Gbl.Crs.Grps.GrpCod);

   /***** Create message to show the change made *****/
   snprintf (AlertTxt,sizeof (AlertTxt),
	     Txt_File_zones_of_the_group_X_are_now_disabled,
             GrpDat.GrpName);

   /***** Show the form again *****/
   Gbl.Crs.Grps.FileZones = false;
   Grp_ReqEditGroupsInternal (Ale_INFO,NULL,
                              Ale_SUCCESS,AlertTxt);
  }

/*****************************************************************************/
/*********************** Change the group type of a group ********************/
/*****************************************************************************/

void Grp_ChangeGroupType (void)
  {
   extern const char *Txt_The_group_X_already_exists;
   extern const char *Txt_The_type_of_group_of_the_group_X_has_changed;
   long NewGrpTypCod;
   struct GroupData GrpDat;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get parameters from form *****/
   /* Get group code */
   if ((Gbl.Crs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /* Get the new group type */
   NewGrpTypCod = Grp_GetParamGrpTypCod ();

   /* Get from the database the type and the name of the group */
   GrpDat.GrpCod = Gbl.Crs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** If group was in database... *****/
   if (Grp_CheckIfGroupNameExists (NewGrpTypCod,GrpDat.GrpName,-1L))
     {
      /* Create warning message */
      AlertType = Ale_WARNING;
      snprintf (AlertTxt,sizeof (AlertTxt),Txt_The_group_X_already_exists,
		GrpDat.GrpName);
     }
   else	// Group is not in database
     {
      /* Update the table of groups changing old type by new type */
      DB_QueryUPDATE ("can not update the type of a group",
		      "UPDATE grp_groups"
		        " SET GrpTypCod=%ld"
		      " WHERE GrpCod=%ld",
                      NewGrpTypCod,Gbl.Crs.Grps.GrpCod);

      /* Create message to show the change made */
      AlertType = Ale_SUCCESS;
      snprintf (AlertTxt,sizeof (AlertTxt),
	        Txt_The_type_of_group_of_the_group_X_has_changed,
                GrpDat.GrpName);
     }

   /***** Show the form again *****/
   Gbl.Crs.Grps.GrpTyp.GrpTypCod = NewGrpTypCod;
   Grp_ReqEditGroupsInternal (Ale_INFO,NULL,
                              AlertType,AlertTxt);
  }

/*****************************************************************************/
/************************* Change the room of a group ************************/
/*****************************************************************************/

void Grp_ChangeGroupRoom (void)
  {
   extern const char *Txt_The_room_assigned_to_the_group_X_has_changed;
   long NewRooCod;
   struct GroupData GrpDat;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get parameters from form *****/
   /* Get group code */
   if ((Gbl.Crs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /* Get the new room */
   NewRooCod = Roo_GetParamRooCod ();

   /* Get from the database the name of the group */
   GrpDat.GrpCod = Gbl.Crs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Update the table of groups changing old room by new room *****/
   DB_QueryUPDATE ("can not update the room of a group",
		   "UPDATE grp_groups"
		     " SET RooCod=%ld"
		   " WHERE GrpCod=%ld",
		   NewRooCod,Gbl.Crs.Grps.GrpCod);

   /* Create message to show the change made */
   AlertType = Ale_SUCCESS;
   snprintf (AlertTxt,sizeof (AlertTxt),
	     Txt_The_room_assigned_to_the_group_X_has_changed,
	     GrpDat.GrpName);

   /***** Show the form again *****/
   Gbl.Crs.Grps.RooCod = NewRooCod;
   Grp_ReqEditGroupsInternal (Ale_INFO,NULL,
                              AlertType,AlertTxt);
  }

/*****************************************************************************/
/************ Change mandatory registration to a group of a type *************/
/*****************************************************************************/

void Grp_ChangeMandatGrpTyp (void)
  {
   extern const char *Txt_The_type_of_enrolment_of_the_type_of_group_X_has_not_changed;
   extern const char *Txt_The_enrolment_of_students_into_groups_of_type_X_is_now_mandatory;
   extern const char *Txt_The_enrolment_of_students_into_groups_of_type_X_is_now_voluntary;
   bool NewMandatoryEnrolment;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_TYPE_NAME];

   /***** Get parameters of the form *****/
   /* Get the código of type of group */
   if ((Gbl.Crs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of type of group is missing.");

   /* Get the new type of enrolment (mandatory or voluntaria) of this type of group */
   NewMandatoryEnrolment = Par_GetParToBool ("MandatoryEnrolment");

   /* Get from the database the name of the type and the old type of enrolment */
   Grp_GetDataOfGroupTypeByCod (&Gbl.Crs.Grps.GrpTyp);

   /***** Check if the old type of enrolment match the new
          (this happens when return is pressed without changes) *****/
   if (Gbl.Crs.Grps.GrpTyp.MandatoryEnrolment == NewMandatoryEnrolment)
     {
      AlertType = Ale_INFO;
      snprintf (AlertTxt,sizeof (AlertTxt),
	        Txt_The_type_of_enrolment_of_the_type_of_group_X_has_not_changed,
                Gbl.Crs.Grps.GrpTyp.GrpTypName);
     }
   else
     {
      /***** Update of the table of types of group changing the old type of enrolment by the new *****/
      DB_QueryUPDATE ("can not update enrolment type of a type of group",
		      "UPDATE crs_grp_types SET Mandatory='%c'"
		      " WHERE GrpTypCod=%ld",
                      NewMandatoryEnrolment ? 'Y' :
        	                              'N',
                      Gbl.Crs.Grps.GrpTyp.GrpTypCod);

      /***** Write message to show the change made *****/
      AlertType = Ale_SUCCESS;
      snprintf (AlertTxt,sizeof (AlertTxt),
	        NewMandatoryEnrolment ? Txt_The_enrolment_of_students_into_groups_of_type_X_is_now_mandatory :
                                        Txt_The_enrolment_of_students_into_groups_of_type_X_is_now_voluntary,
                Gbl.Crs.Grps.GrpTyp.GrpTypName);
     }

   /***** Show the form again *****/
   Gbl.Crs.Grps.GrpTyp.MandatoryEnrolment = NewMandatoryEnrolment;
   Grp_ReqEditGroupsInternal (AlertType,AlertTxt,
                              Ale_INFO,NULL);
  }

/*****************************************************************************/
/******** Change multiple enrolment to one or more groups of a type *********/
/*****************************************************************************/

void Grp_ChangeMultiGrpTyp (void)
  {
   extern const char *Txt_The_type_of_enrolment_of_the_type_of_group_X_has_not_changed;
   extern const char *Txt_Now_each_student_can_belong_to_multiple_groups_of_type_X;
   extern const char *Txt_Now_each_student_can_only_belong_to_a_group_of_type_X;
   bool NewMultipleEnrolment;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_TYPE_NAME];

   /***** Get parameters from the form *****/
   /* Get the code of type of group */
   if ((Gbl.Crs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of type of group is missing.");

   /* Get the new type of enrolment (single or multiple) of this type of group */
   NewMultipleEnrolment = Par_GetParToBool ("MultipleEnrolment");

   /* Get from the database the name of the type and the old type of enrolment */
   Grp_GetDataOfGroupTypeByCod (&Gbl.Crs.Grps.GrpTyp);

   /***** Check if the old type of enrolment match the new one
   	  (this happends when return is pressed without changes) *****/
   if (Gbl.Crs.Grps.GrpTyp.MultipleEnrolment == NewMultipleEnrolment)
     {
      AlertType = Ale_INFO;
      snprintf (AlertTxt,sizeof (AlertTxt),
	        Txt_The_type_of_enrolment_of_the_type_of_group_X_has_not_changed,
                Gbl.Crs.Grps.GrpTyp.GrpTypName);
     }
   else
     {
      /***** Update of the table of types of group changing the old type of enrolment by the new *****/
      DB_QueryUPDATE ("can not update enrolment type of a type of group",
		      "UPDATE crs_grp_types SET Multiple='%c'"
		      " WHERE GrpTypCod=%ld",
                      NewMultipleEnrolment ? 'Y' :
        	                             'N',
                      Gbl.Crs.Grps.GrpTyp.GrpTypCod);

      /***** Write message to show the change made *****/
      AlertType = Ale_SUCCESS;
      snprintf (AlertTxt,sizeof (AlertTxt),
	        NewMultipleEnrolment ? Txt_Now_each_student_can_belong_to_multiple_groups_of_type_X :
                                       Txt_Now_each_student_can_only_belong_to_a_group_of_type_X,
                Gbl.Crs.Grps.GrpTyp.GrpTypName);
     }

   /***** Show the form again *****/
   Gbl.Crs.Grps.GrpTyp.MultipleEnrolment = NewMultipleEnrolment;
   Grp_ReqEditGroupsInternal (AlertType,AlertTxt,
                              Ale_INFO,NULL);
  }

/*****************************************************************************/
/****************** Change open time for a type of group *********************/
/*****************************************************************************/

void Grp_ChangeOpenTimeGrpTyp (void)
  {
   extern const char *Txt_The_date_time_of_opening_of_groups_has_changed;

   /***** Get the code of type of group *****/
   if ((Gbl.Crs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of type of group is missing.");

   /***** Get from the database the data of this type of group *****/
   Grp_GetDataOfGroupTypeByCod (&Gbl.Crs.Grps.GrpTyp);

   /***** Get open time *****/
   Gbl.Crs.Grps.GrpTyp.OpenTimeUTC = Dat_GetTimeUTCFromForm ("OpenTimeUTC");
   Gbl.Crs.Grps.GrpTyp.MustBeOpened = Grp_CheckIfOpenTimeInTheFuture (Gbl.Crs.Grps.GrpTyp.OpenTimeUTC);

   /***** Update the table of types of group
          changing the old open time of enrolment by the new *****/
   DB_QueryUPDATE ("can not update enrolment type of a type of group",
		   "UPDATE crs_grp_types"
		   " SET MustBeOpened='%c',OpenTime=FROM_UNIXTIME(%ld)"
		   " WHERE GrpTypCod=%ld",
                   Gbl.Crs.Grps.GrpTyp.MustBeOpened ? 'Y' :
        	                                             'N',
                   (long) Gbl.Crs.Grps.GrpTyp.OpenTimeUTC,
                   Gbl.Crs.Grps.GrpTyp.GrpTypCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_date_time_of_opening_of_groups_has_changed);

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (Ale_SUCCESS,NULL,
                              Ale_INFO,NULL);
  }

/*****************************************************************************/
/***************** Change maximum of students in a group *********************/
/*****************************************************************************/

void Grp_ChangeMaxStdsGrp (void)
  {
   extern const char *Txt_The_maximum_number_of_students_in_group_X_has_not_changed;
   extern const char *Txt_The_group_X_does_not_have_a_student_limit_now;
   extern const char *Txt_The_maximum_number_of_students_in_group_X_is_now_Y;
   struct GroupData GrpDat;
   unsigned NewMaxStds;
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get parameters of the form *****/
   /* Get group code */
   if ((Gbl.Crs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /* Get the new maximum number of students of the group */
   NewMaxStds = (unsigned)
	        Par_GetParToUnsignedLong ("MaxStudents",
                                          0,
                                          Grp_MAX_STUDENTS_IN_A_GROUP,
                                          Grp_NUM_STUDENTS_NOT_LIMITED);

   /* Get from the database the type, name, and old maximum of students of the group */
   GrpDat.GrpCod = Gbl.Crs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Check if the old maximum of students equals the new one
             (this happens when return is pressed without changes) *****/
   if (GrpDat.MaxStudents == NewMaxStds)
     {
      AlertType = Ale_INFO;
      snprintf (AlertTxt,sizeof (AlertTxt),
		Txt_The_maximum_number_of_students_in_group_X_has_not_changed,
		GrpDat.GrpName);
     }
   else
     {
      /***** Update the table of groups changing the old maximum of students to the new *****/
      DB_QueryUPDATE ("can not update the maximum number of students"
		      " in a group",
		      "UPDATE grp_groups"
		        " SET MaxStudents=%u"
		      " WHERE GrpCod=%ld",
                      NewMaxStds,Gbl.Crs.Grps.GrpCod);

      /***** Write message to show the change made *****/
      AlertType = Ale_SUCCESS;
      if (NewMaxStds > Grp_MAX_STUDENTS_IN_A_GROUP)
         snprintf (AlertTxt,sizeof (AlertTxt),
	           Txt_The_group_X_does_not_have_a_student_limit_now,
                   GrpDat.GrpName);
      else
         snprintf (AlertTxt,sizeof (AlertTxt),
	           Txt_The_maximum_number_of_students_in_group_X_is_now_Y,
                   GrpDat.GrpName,NewMaxStds);
     }

   /***** Show the form again *****/
   Gbl.Crs.Grps.MaxStudents = NewMaxStds;
   Grp_ReqEditGroupsInternal (Ale_INFO,NULL,
                              AlertType,AlertTxt);
  }

/*****************************************************************************/
/************* Write the maximum number of students in a group ***************/
/*****************************************************************************/

static void Grp_WriteMaxStds (char Str[Cns_MAX_DECIMAL_DIGITS_UINT + 1],unsigned MaxStudents)
  {
   if (MaxStudents <= Grp_MAX_STUDENTS_IN_A_GROUP)
      snprintf (Str,Cns_MAX_DECIMAL_DIGITS_UINT + 1,"%u",MaxStudents);
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
   extern const char *Txt_The_name_of_the_type_of_group_X_has_not_changed;
   char NewNameGrpTyp[Grp_MAX_BYTES_GROUP_TYPE_NAME + 1];
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_TYPE_NAME];

   /***** Get parameters from form *****/
   /* Get the code of the group type */
   if ((Gbl.Crs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of type of group is missing.");

   /* Get the new name for the group type */
   Par_GetParToText ("GrpTypName",NewNameGrpTyp,Grp_MAX_BYTES_GROUP_TYPE_NAME);

   /***** Get from the database the old name of the group type *****/
   Grp_GetDataOfGroupTypeByCod (&Gbl.Crs.Grps.GrpTyp);

   /***** Check if new name is empty *****/
   if (NewNameGrpTyp[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (Gbl.Crs.Grps.GrpTyp.GrpTypName,NewNameGrpTyp))	// Different names
        {
         /***** If group type was in database... *****/
         if (Grp_CheckIfGroupTypeNameExists (NewNameGrpTyp,Gbl.Crs.Grps.GrpTyp.GrpTypCod))
           {
	    AlertType = Ale_WARNING;
            snprintf (AlertTxt,sizeof (AlertTxt),
	              Txt_The_type_of_group_X_already_exists,NewNameGrpTyp);
           }
         else
           {
            /* Update the table changing old name by new name */
            DB_QueryUPDATE ("can not update the type of a group",
        		    "UPDATE crs_grp_types SET GrpTypName='%s'"
			    " WHERE GrpTypCod=%ld",
                            NewNameGrpTyp,
                            Gbl.Crs.Grps.GrpTyp.GrpTypCod);

            /***** Write message to show the change made *****/
	    AlertType = Ale_SUCCESS;
            snprintf (AlertTxt,sizeof (AlertTxt),
	              Txt_The_type_of_group_X_has_been_renamed_as_Y,
                      Gbl.Crs.Grps.GrpTyp.GrpTypName,NewNameGrpTyp);
           }
        }
      else	// The same name
        {
	 AlertType = Ale_INFO;
         snprintf (AlertTxt,sizeof (AlertTxt),
	           Txt_The_name_of_the_type_of_group_X_has_not_changed,
                   NewNameGrpTyp);
        }
     }
   else
     {
      AlertType = Ale_WARNING;
      Str_Copy (AlertTxt,Txt_You_can_not_leave_the_field_empty,
		sizeof (AlertTxt) - 1);
     }

   /***** Show the form again *****/
   Str_Copy (Gbl.Crs.Grps.GrpTyp.GrpTypName,NewNameGrpTyp,
             sizeof (Gbl.Crs.Grps.GrpTyp.GrpTypName) - 1);
   Grp_ReqEditGroupsInternal (AlertType,AlertTxt,
                              Ale_INFO,NULL);
  }

/*****************************************************************************/
/******************************* Rename a group ******************************/
/*****************************************************************************/

void Grp_RenameGroup (void)
  {
   extern const char *Txt_You_can_not_leave_the_field_empty;
   extern const char *Txt_The_group_X_already_exists;
   extern const char *Txt_The_group_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_group_X_has_not_changed;
   struct GroupData GrpDat;
   char NewNameGrp[Grp_MAX_BYTES_GROUP_NAME + 1];
   Ale_AlertType_t AlertType;
   char AlertTxt[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get parameters from form *****/
   /* Get the code of the group */
   if ((Gbl.Crs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /* Get the new name for the group */
   Par_GetParToText ("GrpName",NewNameGrp,Grp_MAX_BYTES_GROUP_NAME);

   /***** Get from the database the type and the old name of the group *****/
   GrpDat.GrpCod = Gbl.Crs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Check if new name is empty *****/
   if (!NewNameGrp[0])
     {
      AlertType = Ale_WARNING;
      Str_Copy (AlertTxt,Txt_You_can_not_leave_the_field_empty,
		sizeof (AlertTxt) - 1);
     }
   else
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (GrpDat.GrpName,NewNameGrp))	// Different names
        {
         /***** If group was in database... *****/
         if (Grp_CheckIfGroupNameExists (GrpDat.GrpTypCod,NewNameGrp,Gbl.Crs.Grps.GrpCod))
           {
	    AlertType = Ale_WARNING;
            snprintf (AlertTxt,sizeof (AlertTxt),
	              Txt_The_group_X_already_exists,NewNameGrp);
           }
         else
           {
            /* Update the table changing old name by new name */
            DB_QueryUPDATE ("can not update the name of a group",
        		    "UPDATE grp_groups"
        		      " SET GrpName='%s'"
        		    " WHERE GrpCod=%ld",
                            NewNameGrp,Gbl.Crs.Grps.GrpCod);

            /***** Write message to show the change made *****/
	    AlertType = Ale_SUCCESS;
            snprintf (AlertTxt,sizeof (AlertTxt),
	              Txt_The_group_X_has_been_renamed_as_Y,
                      GrpDat.GrpName,NewNameGrp);
           }
        }
      else	// The same name
        {
	 AlertType = Ale_INFO;
         snprintf (AlertTxt,sizeof (AlertTxt),
	           Txt_The_name_of_the_group_X_has_not_changed,NewNameGrp);
        }
     }

   /***** Show the form again *****/
   Str_Copy (Gbl.Crs.Grps.GrpName,NewNameGrp,sizeof (Gbl.Crs.Grps.GrpName) - 1);
   Grp_ReqEditGroupsInternal (Ale_INFO,NULL,
                              AlertType,AlertTxt);
  }

/*****************************************************************************/
/******************* Get parameter with code of group type *******************/
/*****************************************************************************/

static long Grp_GetParamGrpTypCod (void)
  {
   /***** Get code of group type *****/
   return Par_GetParToLong ("GrpTypCod");
  }

/*****************************************************************************/
/*********************** Get parameter with group code ***********************/
/*****************************************************************************/

static long Grp_GetParamGrpCod (void)
  {
   /***** Get group code *****/
   return Par_GetParToLong ("GrpCod");
  }

/*****************************************************************************/
/****************** Write parameter with code of group type ******************/
/*****************************************************************************/

static void Grp_PutParamGrpTypCod (void *GrpTypCod)
  {
   if (GrpTypCod)
      Par_PutHiddenParamLong (NULL,"GrpTypCod",*((long *) GrpTypCod));
  }

/*****************************************************************************/
/********************* Write parameter with code of group ********************/
/*****************************************************************************/

void Grp_PutParamGrpCod (void *GrpCod)
  {
   if (GrpCod)
      Par_PutHiddenParamLong (NULL,"GrpCod",*((long *) GrpCod));
  }

/*****************************************************************************/
/************************ Get list of group codes selected *******************/
/*****************************************************************************/

void Grp_GetLstCodsGrpWanted (struct ListCodGrps *LstGrpsWanted)
  {
   unsigned NumGrpTyp;
   char Param[6 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   char LongStr[1 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   char **LstStrCodGrps;
   const char *Ptr;
   unsigned NumGrpWanted;

   /***** Allocate memory for the strings with group codes in each type *****/
   if ((LstStrCodGrps = calloc (Gbl.Crs.Grps.GrpTypes.Num,
                                sizeof (*LstStrCodGrps))) == NULL)
      Lay_NotEnoughMemoryExit ();

   /***** Get lists with the groups that I want in each type
          in order to count the total number of groups selected *****/
   for (NumGrpTyp = 0, LstGrpsWanted->NumGrps = 0;
	NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	NumGrpTyp++)
     {
      /***** Allocate memory for the list of group codes of this type *****/
      if ((LstStrCodGrps[NumGrpTyp] = malloc (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps *
                                              (Cns_MAX_DECIMAL_DIGITS_LONG + 1))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the multiple parameter code of group of this type *****/
      snprintf (Param,sizeof (Param),"GrpCod%ld",
                Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
      Par_GetParMultiToText (Param,LstStrCodGrps[NumGrpTyp],
                             ((Cns_MAX_DECIMAL_DIGITS_LONG + 1) * Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps) - 1);
      if (LstStrCodGrps[NumGrpTyp][0])
        {
         /***** Count the number of groups selected of this type of LstCodGrps[NumGrpTyp] *****/
         for (Ptr = LstStrCodGrps[NumGrpTyp], NumGrpWanted = 0;
              *Ptr;
              NumGrpWanted++)
            Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);

         /***** Add the number of groups selected of this type to the number of groups selected total *****/
         LstGrpsWanted->NumGrps += NumGrpWanted;
        }
      }

   /***** Create the list (argument passed to this function)
          with all the groups selected (of all the types) *****/
   if (LstGrpsWanted->NumGrps)
     {
      if ((LstGrpsWanted->GrpCods = calloc (LstGrpsWanted->NumGrps,
                                            sizeof (*LstGrpsWanted->GrpCods))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the groups *****/
      for (NumGrpTyp = 0, NumGrpWanted = 0;
	   NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
        {
         /* Add the groups selected of this type to the complete list of groups selected */
         for (Ptr = LstStrCodGrps[NumGrpTyp];
              *Ptr;
              NumGrpWanted++)
           {
            Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
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

void Grp_PutParamAllGroups (void)
  {
   Par_PutHiddenParamChar ("AllGroups",'Y');
  }

/*****************************************************************************/
/************* Parameter to show only my groups or all groups ****************/
/*****************************************************************************/

void Grp_PutParamWhichGroups (void *WhichGrps)
  {
   if (WhichGrps)
      Par_PutHiddenParamUnsigned (NULL,"WhichGrps",
				  (unsigned) *((Grp_WhichGroups_t *) WhichGrps));
  }

void Grp_PutParamWhichGrpsOnlyMyGrps (void)
  {
   Par_PutHiddenParamUnsigned (NULL,"WhichGrps",(unsigned) Grp_MY_GROUPS);
  }

void Grp_PutParamWhichGrpsAllGrps (void)
  {
   Par_PutHiddenParamUnsigned (NULL,"WhichGrps",(unsigned) Grp_ALL_GROUPS);
  }

/*****************************************************************************/
/***** Show form to choice whether to show only my groups or all groups ******/
/*****************************************************************************/

void Grp_ShowFormToSelWhichGrps (Act_Action_t Action,
                                 void (*FuncParams) (void *Args),void *Args)
  {
   extern const char *Txt_GROUP_WHICH_GROUPS[2];
   Grp_WhichGroups_t WhichGrps;

   /***** Start setting selector *****/
   Set_BeginOneSettingSelector ();

   /***** Put icons to select which groups *****/
   for (WhichGrps  = Grp_MY_GROUPS;
	WhichGrps <= Grp_ALL_GROUPS;
	WhichGrps++)
     {
      HTM_DIV_Begin ("class=\"%s\"",
		      WhichGrps == Gbl.Crs.Grps.WhichGrps ? "PREF_ON" :
							    "PREF_OFF");
      Frm_BeginForm (Action);
      Par_PutHiddenParamUnsigned (NULL,"WhichGrps",(unsigned) WhichGrps);
      if (FuncParams)	// Extra parameters depending on the action
	 FuncParams (Args);
      Ico_PutSettingIconLink (WhichGrps == Grp_MY_GROUPS ? "mysitemap.png" :
							   "sitemap.svg",
			      Txt_GROUP_WHICH_GROUPS[WhichGrps]);
      Frm_EndForm ();
      HTM_DIV_End ();
     }

   /***** End setting selector *****/
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/************* Get whether to show only my groups or all groups **************/
/*****************************************************************************/

Grp_WhichGroups_t Grp_GetParamWhichGroups (void)
  {
   static bool AlreadyGot = false;
   Grp_WhichGroups_t WhichGroupsDefault;

   if (!AlreadyGot)
     {
      /***** Get which groups (my groups or all groups) *****/
      /* Set default */
      switch (Gbl.Action.Act)
	{
	 case ActSeeCrsTT:	// Show course timetable
	 case ActPrnCrsTT:	// Print course timetable
	 case ActChgCrsTT1stDay:// Change first day of week in course timetable
	 case ActSeeAsg:	// List assignments
	 case ActSeeAllExa:	// List exams
	 case ActSeeAllGam:	// List games
	 case ActSeeAllSvy:	// List surveys
	 case ActSeeAtt:	// List attendance
	    /*
	    If I belong       to this course ==> see only my groups
	    If I don't belong to this course ==> see all groups
	    */
	    WhichGroupsDefault = Gbl.Usrs.Me.IBelongToCurrentCrs ? Grp_MY_GROUPS :
							           Grp_ALL_GROUPS;
	    break;
	 case ActSeeMyTT:	// Show my timetable
	 case ActPrnMyTT:	// Print my timetable
	 case ActChgMyTT1stDay:	// Change first day of week in my timetable
	    /* By default, show only my groups */
	    WhichGroupsDefault = Grp_MY_GROUPS;
	    break;
	 default:			// Control never should enter here
	    WhichGroupsDefault = Grp_WHICH_GROUPS_DEFAULT;
	    break;
	}

      /* Get parameter */
      Gbl.Crs.Grps.WhichGrps = (Grp_WhichGroups_t)
	                       Par_GetParToUnsignedLong ("WhichGrps",
	                                                 0,
	                                                 Grp_NUM_WHICH_GROUPS - 1,
	                                                 (unsigned long) WhichGroupsDefault);

      AlreadyGot = true;
     }

   return Gbl.Crs.Grps.WhichGrps;
  }
