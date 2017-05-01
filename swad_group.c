// swad_group.c: types of groups and groups

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For exit, system, malloc, free, rand, etc.
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_notification.h"
#include "swad_parameter.h"

/*****************************************************************************/
/*************************** Internal constants ******************************/
/*****************************************************************************/

#define Grp_SECTION_GROUP_TYPES		"grp_types"
#define Grp_SECTION_NEW_GROUP_TYPE	"new_grp_type"
#define Grp_SECTION_GROUPS		"grps"
#define Grp_SECTION_NEW_GROUP		"new_grp"

/*****************************************************************************/
/***************************** Internal types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/************* External global variables from others modules *****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

static void Grp_ReqEditGroupsInternal (Lay_AlertType_t AlertTypeGroupTypes,const char *MessageGroupTypes,
                                       Lay_AlertType_t AlertTypeGroups,const char *MessageGroups);
static void Grp_ReqEditGroupsInternal0 (void);
static void Grp_ReqEditGroupsInternal1 (Lay_AlertType_t AlertTypeGroupTypes,const char *MessageGroupTypes);
static void Grp_ReqEditGroupsInternal2 (Lay_AlertType_t AlertTypeGroups,const char *MessageGroups);

static void Grp_EditGroupTypes (void);
static void Grp_EditGroups (void);
static void Grp_PutIconsEditingGroups (void);
static void Grp_PutIconToCreateNewGroup (void);

static void Grp_ConstructorListGrpAlreadySelec (struct ListGrpsAlreadySelec **AlreadyExistsGroupOfType);
static void Grp_DestructorListGrpAlreadySelec (struct ListGrpsAlreadySelec **AlreadyExistsGroupOfType);
static void Grp_RemoveUsrFromGroup (long UsrCod,long GrpCod);
static void Grp_AddUsrToGroup (struct UsrData *UsrDat,long GrpCod);

static void Grp_ListGroupTypesForEdition (void);
static void Grp_PutIconsEditingGroupTypes (void);
static void Grp_PutIconToViewGroups (void);
static void Grp_PutIconToCreateNewGroupType (void);
static void Grp_WriteHeadingGroupTypes (void);

static void Grp_ListGroupsForEdition (void);
static void Grp_WriteHeadingGroups (void);
static void Grp_PutIconToEditGroups (void);

static void Grp_ShowWarningToStdsToChangeGrps (void);
static unsigned Grp_ListGrpsForChange (struct GroupType *GrpTyp);
static void Grp_ListGrpsToAddOrRemUsrs (struct GroupType *GrpTyp,long UsrCod);
static void Grp_ListGrpsForMultipleSelection (struct GroupType *GrpTyp);
static void Grp_WriteGrpHead (struct GroupType *GrpTyp);
static void Grp_WriteRowGrp (struct Group *Grp,bool Highlight);
static void Grp_PutFormToCreateGroupType (void);
static void Grp_PutFormToCreateGroup (void);
static unsigned Grp_CountNumGrpsInThisCrsOfType (long GrpTypCod);
static void Grp_GetDataOfGroupTypeByCod (struct GroupType *GrpTyp);
static bool Grp_GetMultipleEnrolmentOfAGroupType (long GrpTypCod);
static long Grp_GetTypeOfGroupOfAGroup (long GrpCod);
static unsigned Grp_CountNumStdsInNoGrpsOfType (long GrpTypCod);
static long Grp_GetFirstCodGrpStdBelongsTo (long GrpTypCod,long UsrCod);
static bool Grp_GetIfGrpIsAvailable (long GrpTypCod);
static void Grp_GetLstCodGrpsUsrBelongs (long CrsCod,long GrpTypCod,long UsrCod,
                                         struct ListCodGrps *LstGrps);
static bool Grp_CheckIfGrpIsInList (long GrpCod,struct ListCodGrps *LstGrps);
static bool Grp_CheckIfOpenTimeInTheFuture (time_t OpenTimeUTC);
static bool Grp_CheckIfGroupTypeNameExists (const char *GrpTypName,long GrpTypCod);
static bool Grp_CheckIfGroupNameExists (long GrpTypCod,const char *GrpName,long GrpCod);
static void Grp_CreateGroupType (void);
static void Grp_CreateGroup (void);

static void Grp_AskConfirmRemGrpTypWithGrps (unsigned NumGrps);
static void Grp_PutParamRemGrpTyp (void);
static void Grp_AskConfirmRemGrp (void);
static void Grp_PutParamRemGrp (void);
static void Grp_RemoveGroupTypeCompletely (void);
static void Grp_RemoveGroupCompletely (void);

static void Grp_WriteMaxStdsGrp (unsigned MaxStudents);
static long Grp_GetParamGrpTypCod (void);
static long Grp_GetParamGrpCod (void);
static void Grp_PutParamGrpTypCod (long GrpTypCod);

/*****************************************************************************/
/******************* Write the names of the selected groups ******************/
/*****************************************************************************/

void Grp_WriteNamesOfSelectedGrps (void)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_students_with_no_group;
   extern const char *Txt_and;
   long GrpCod;
   unsigned NumGrpSel;
   struct GroupData GrpDat;

   /***** Show the selected groups *****/
   fprintf (Gbl.F.Out,"%s: ",
            (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps == 1) ?
            Txt_Group  :
            Txt_Groups);
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
     {
      if ((GrpCod = Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCods[NumGrpSel]) >= 0)
        {
         GrpDat.GrpCod = GrpCod;
         Grp_GetDataOfGroupByCod (&GrpDat);
         fprintf (Gbl.F.Out,"%s %s",
                  GrpDat.GrpTypName,GrpDat.GrpName);
        }
      else	// GrpCod < 0 ==> students not belonging to any group of type (-GrpCod)
        {
         Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod = -GrpCod;
         Grp_GetDataOfGroupTypeByCod (&Gbl.CurrentCrs.Grps.GrpTyp);
         fprintf (Gbl.F.Out,"%s (%s)",
                  Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName,
                  Txt_students_with_no_group);
        }

      if (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps >= 2)
        {
         if (NumGrpSel == Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps-2)
            fprintf (Gbl.F.Out," %s ",Txt_and);
         if (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps >= 3)
            if (NumGrpSel < Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps-2)
               fprintf (Gbl.F.Out,", ");
        }
     }
  }

/*****************************************************************************/
/************************** Put forms to edit groups *************************/
/*****************************************************************************/

void Grp_ReqEditGroups (void)
  {
   Grp_ReqEditGroupsInternal (Lay_INFO,NULL,
                              Lay_INFO,NULL);
  }

static void Grp_ReqEditGroupsInternal (Lay_AlertType_t AlertTypeGroupTypes,const char *MessageGroupTypes,
                                       Lay_AlertType_t AlertTypeGroups,const char *MessageGroups)
  {
   Grp_ReqEditGroupsInternal0 ();
   Grp_ReqEditGroupsInternal1 (AlertTypeGroupTypes,MessageGroupTypes);
   Grp_ReqEditGroupsInternal2 (AlertTypeGroups,MessageGroups);
  }

static void Grp_ReqEditGroupsInternal0 (void)
  {
   /***** Start groups types section *****/
   fprintf (Gbl.F.Out,"<section id=\"%s\">",Grp_SECTION_GROUP_TYPES);
  }

static void Grp_ReqEditGroupsInternal1 (Lay_AlertType_t AlertTypeGroupTypes,const char *MessageGroupTypes)
  {
   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ALL_GROUP_TYPES);

   /***** Show optional alert *****/
   if (MessageGroupTypes)
      if (MessageGroupTypes[0])
         Lay_ShowAlert (AlertTypeGroupTypes,MessageGroupTypes);

   /***** Put form to edit group types *****/
   Grp_EditGroupTypes ();

   /***** End groups types section *****/
   fprintf (Gbl.F.Out,"</section>");

   /***** Start groups section *****/
   fprintf (Gbl.F.Out,"<section id=\"%s\">",Grp_SECTION_GROUPS);
  }

static void Grp_ReqEditGroupsInternal2 (Lay_AlertType_t AlertTypeGroups,const char *MessageGroups)
  {
   /***** Show optional alert *****/
   if (MessageGroups)
      if (MessageGroups[0])
         Lay_ShowAlert (AlertTypeGroups,MessageGroups);

   /***** Put form to edit groups *****/
   if (Gbl.CurrentCrs.Grps.GrpTypes.Num) // If there are group types...
      Grp_EditGroups ();

   /***** End groups section *****/
   fprintf (Gbl.F.Out,"</section>");

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

   /***** Start frame *****/
   Lay_StartRoundFrame (NULL,Txt_Types_of_group,
                        Grp_PutIconsEditingGroupTypes,
                        Hlp_USERS_Groups);

   /***** Forms to edit current group types *****/
   if (Gbl.CurrentCrs.Grps.GrpTypes.Num)	// Group types found...
      Grp_ListGroupTypesForEdition ();
   else	// No group types found in this course
     {
      sprintf (Gbl.Message,Txt_There_are_no_types_of_group_in_the_course_X,
               Gbl.CurrentCrs.Crs.ShrtName);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);
     }

   /***** Put a form to create a new group type *****/
   Grp_PutFormToCreateGroupType ();

   /***** End frame *****/
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/**************************** Put forms to edit groups ***********************/
/*****************************************************************************/

static void Grp_EditGroups (void)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *Txt_Groups;
   extern const char *Txt_No_groups_have_been_created_in_the_course_X;

   /***** Start frame *****/
   Lay_StartRoundFrame (NULL,Txt_Groups,Grp_PutIconsEditingGroups,
                        Hlp_USERS_Groups);

   /***** Forms to edit current groups *****/
   if (Gbl.CurrentCrs.Grps.GrpTypes.NumGrpsTotal)	// If there are groups...
      Grp_ListGroupsForEdition ();
   else	// There are group types, but there aren't groups
     {
      sprintf (Gbl.Message,Txt_No_groups_have_been_created_in_the_course_X,
               Gbl.CurrentCrs.Crs.ShrtName);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);
     }

   /***** Put a form to create a new group *****/
   Grp_PutFormToCreateGroup ();

   /***** End frame *****/
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of groups ******************/
/*****************************************************************************/

static void Grp_PutIconsEditingGroups (void)
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
   Lay_PutContextualLink (ActReqEdiGrp,Grp_SECTION_NEW_GROUP,NULL,
                          "plus64x64.png",
                          Txt_New_group,NULL,
                          NULL);
  }

/*****************************************************************************/
/*************** Show form to select one or several groups *******************/
/*****************************************************************************/

void Grp_ShowFormToSelectSeveralGroups (Act_Action_t NextAction)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_Groups;
   extern const char *Txt_All_groups;
   extern const char *Txt_Update_students;
   extern const char *Txt_Update_students_according_to_selected_groups;
   unsigned NumGrpTyp;
   bool ICanEdit;

   if (Gbl.CurrentCrs.Grps.NumGrps)
     {
      ICanEdit = !Gbl.Form.Inside &&
	         (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER ||
                  Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);

      /***** Start frame *****/
      Lay_StartRoundFrame (NULL,Txt_Groups,
			   ICanEdit ? Grp_PutIconToEditGroups :
				      NULL,
			   Hlp_USERS_Groups);

      /***** Start form to update the students listed
	     depending on the groups selected *****/
      Act_FormStart (NextAction);
      Usr_PutParamsPrefsAboutUsrList ();

      /***** Put parameters needed depending on the action *****/
      Usr_PutExtraParamsUsrList (NextAction);

      /***** Select all groups *****/
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_OPT\">"
                         "<label class=\"%s\">"
			 "<input type=\"checkbox\""
			 " id=\"AllGroups\" name=\"AllGroups\" value=\"Y\"",
	       The_ClassForm[Gbl.Prefs.Theme]);
      if (Gbl.Usrs.ClassPhoto.AllGroups)
	 fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"togglecheckChildren(this,'GrpCods')\" />"
			 "&nbsp;%s"
                         "</label>"
			 "</div>",
	       Txt_All_groups);

      /***** Get list of groups types and groups in this course *****/
      Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

      /***** List the groups for each group type *****/
      Lay_StartTableWide (2);
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
	 if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
	    Grp_ListGrpsForMultipleSelection (&Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp]);
      Lay_EndTable ();

      /***** Free list of groups types and groups in this course *****/
      Grp_FreeListGrpTypesAndGrps ();

      /***** Submit button *****/
      fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\""
			 " style=\"padding-top:12px;\">");
      Act_LinkFormSubmitAnimated (Txt_Update_students_according_to_selected_groups,
				  The_ClassFormBold[Gbl.Prefs.Theme],
				  "CopyMessageToHiddenFields()");
      Lay_PutCalculateIconWithText (Txt_Update_students_according_to_selected_groups,
				    Txt_Update_students);
      fprintf (Gbl.F.Out,"</div>");

      /***** End form *****/
      Act_FormEnd ();

      /***** End frame *****/
      Lay_EndRoundFrame ();
     }
  }

/*****************************************************************************/
/************ Put parameters with the groups of students selected ************/
/*****************************************************************************/

void Grp_PutParamsCodGrps (void)
  {
   unsigned NumGrpSel;

   /***** Write the boolean parameter that indicates if all the groups must be listed *****/
   Par_PutHiddenParamChar ("AllGroups",
                           Gbl.Usrs.ClassPhoto.AllGroups ? 'Y' :
                        	                           'N');

   /***** Write the parameter with the list of group codes to show *****/
   if (!Gbl.Usrs.ClassPhoto.AllGroups &&
       Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps)
     {
      fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"GrpCods\" value=\"");
      for (NumGrpSel = 0;
	   NumGrpSel < Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps;
	   NumGrpSel++)
        {
         if (NumGrpSel)
            fprintf (Gbl.F.Out,"%c",Par_SEPARATOR_PARAM_MULTIPLE);
         fprintf (Gbl.F.Out,"%ld",Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
        }
      fprintf (Gbl.F.Out,"\" />");
     }
  }

/*****************************************************************************/
/**************** Get parameters related to groups selected ******************/
/*****************************************************************************/

void Grp_GetParCodsSeveralGrpsToShowUsrs (void)
  {
   struct ListCodGrps LstGrpsIBelong;
   unsigned NumGrp;

   if (++Gbl.CurrentCrs.Grps.LstGrpsSel.NestedCalls > 1) // If list is created yet, there's nothing to do
      return;

   /***** Get boolean parameter that indicates if all groups must be listed *****/
   Gbl.Usrs.ClassPhoto.AllGroups = Par_GetParToBool ("AllGroups");

   /***** Get parameter with list of groups selected *****/
   Grp_GetParCodsSeveralGrps ();

   if (Gbl.CurrentCrs.Grps.NumGrps &&		// This course has groups and...
       !Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps)	// ...I haven't selected any group
     {
      /***** I I haven't selected any group, show by default the groups I belong to *****/
      /* Get list of groups of all types in current course I belong to */
      Grp_GetLstCodGrpsUsrBelongs (Gbl.CurrentCrs.Crs.CrsCod,-1L,
				   Gbl.Usrs.Me.UsrDat.UsrCod,&LstGrpsIBelong);

      if (LstGrpsIBelong.NumGrps)
	{
	 /* Allocate space for list of selected groups */
	 if ((Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCods = (long *) calloc (LstGrpsIBelong.NumGrps,sizeof (long))) == NULL)
	    Lay_ShowErrorAndExit ("Not enough memory to store the codes of the selected groups.");

	 /* Fill list of selected groups with list of groups I belong to */
	 for (NumGrp = 0;
	      NumGrp < LstGrpsIBelong.NumGrps;
	      NumGrp++)
	    Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCods[NumGrp] = LstGrpsIBelong.GrpCods[NumGrp];
	 Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps = LstGrpsIBelong.NumGrps;
	}

      /* Free list of groups I belong to */
      Grp_FreeListCodGrp (&LstGrpsIBelong);
     }

   /***** If no groups selected ==> show all groups *****/
   if (!Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps)
      Gbl.Usrs.ClassPhoto.AllGroups = true;
  }

/*****************************************************************************/
/**************** Get parameter with list of groups selected *****************/
/*****************************************************************************/

void Grp_GetParCodsSeveralGrps (void)
  {
   char *ParamLstCodGrps;
   const char *Ptr;
   char LongStr[1 + 10 + 1];
   unsigned NumGrp;
   unsigned long MaxSizeLstGrpCods = ((1 + 10 + 1) * Gbl.CurrentCrs.Grps.NumGrps) - 1;

   /***** Reset number of groups selected *****/
   Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps = 0;

   if (Gbl.CurrentCrs.Grps.NumGrps)	// If course has groups
     {
      /***** Allocate memory for the list of group codes selected *****/
      if ((ParamLstCodGrps = (char *) malloc (MaxSizeLstGrpCods + 1)) == NULL)
	 Lay_ShowErrorAndExit ("Not enough memory to store the codes of the selected groups.");

      /***** Get parameter with list of groups to list *****/
      Par_GetParMultiToText ("GrpCods",ParamLstCodGrps,MaxSizeLstGrpCods);

      if (ParamLstCodGrps[0])
	{
	 /***** Count number of groups selected from LstCodGrps *****/
	 for (Ptr = ParamLstCodGrps, NumGrp = 0;
	      *Ptr;
	      NumGrp++)
	    Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,1 + 10);
	 Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps = NumGrp;

	 if (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps)	// If I have selected groups...
	   {
	    /***** Create a list of groups selected from LstCodGrps *****/
	    if ((Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCods = (long *) calloc (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps,sizeof (long))) == NULL)
	       Lay_ShowErrorAndExit ("Not enough memory to store the codes of the selected groups.");
	    for (Ptr = ParamLstCodGrps, NumGrp = 0;
		 *Ptr;
		 NumGrp++)
	      {
	       Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,1 + 10);
	       Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCods[NumGrp] = Str_ConvertStrCodToLongCod (LongStr);
	      }
	   }
	}

      /***** Free memory used for the list of groups to show *****/
      free ((void *) ParamLstCodGrps);
     }
  }

/*****************************************************************************/
/********* Free memory used for the list of group codes selected *************/
/*****************************************************************************/

void Grp_FreeListCodSelectedGrps (void)
  {
   if (Gbl.CurrentCrs.Grps.LstGrpsSel.NestedCalls > 0)
      if (--Gbl.CurrentCrs.Grps.LstGrpsSel.NestedCalls == 0)
         if (Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCods)
           {
            free ((void *) Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCods);
            Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCods = NULL;
            Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps = 0;
           }
  }

/*****************************************************************************/
/******************* Change my groups and show form again ********************/
/*****************************************************************************/

void Grp_ChangeMyGrpsAndShowChanges (void)
  {
   /***** Change my groups *****/
   Grp_ChangeMyGrps ();

   /***** Show again the table of selection of groups with the changes already made *****/
   Grp_ReqRegisterInGrps ();
  }

/*****************************************************************************/
/****************************** Change my groups *****************************/
/*****************************************************************************/

void Grp_ChangeMyGrps (void)
  {
   extern const char *Txt_The_requested_group_changes_were_successful;
   extern const char *Txt_There_has_been_no_change_in_groups;
   extern const char *Txt_In_a_type_of_group_with_single_enrolment_students_can_not_be_registered_in_more_than_one_group;
   struct ListCodGrps LstGrpsIWant;
   bool MySelectionIsValid = true;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   /***** Get the group codes which I want to join to *****/
   LstGrpsIWant.GrpCods = NULL;	// Initialized to avoid bug reported by Coverity
   LstGrpsIWant.NumGrps = 0;	// Initialized to avoid bug reported by Coverity
   Grp_GetLstCodsGrpWanted (&LstGrpsIWant);

   /***** A student can not be enroled in more than one group
          if the type of group is of single enrolment *****/
   // As the form to register in groups of single enrolment...
   // ...is a radio-based form and not a checkbox-based form...
   // ...this check is made only to avoid problems...
   // ...if the student manipulates the form
   if (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT &&
       LstGrpsIWant.NumGrps >= 2)
      MySelectionIsValid = Grp_CheckIfSelectionGrpsIsValid (&LstGrpsIWant);

   /***** Free list of groups types and groups in this course *****/
   // The lists of group types and groups need to be freed here...
   // ...in order to get them again when changing my groups atomically
   Grp_FreeListGrpTypesAndGrps ();

   /***** Change my groups *****/
   if (MySelectionIsValid)
     {
      if (Grp_ChangeMyGrpsAtomically (&LstGrpsIWant))
	 Lay_ShowAlert (Lay_SUCCESS,Txt_The_requested_group_changes_were_successful);
      else
	 Lay_ShowAlert (Lay_WARNING,Txt_There_has_been_no_change_in_groups);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_In_a_type_of_group_with_single_enrolment_students_can_not_be_registered_in_more_than_one_group);

   /***** Free memory with the list of groups which I want to belong to *****/
   Grp_FreeListCodGrp (&LstGrpsIWant);
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
   bool SelectionIsValid = true;

   /***** Get list of groups types and groups in current course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   /***** Get the list of groups to which register this user *****/
   LstGrpsUsrWants.GrpCods = NULL;	// Initialized to avoid bug reported by Coverity
   LstGrpsUsrWants.NumGrps = 0;		// Initialized to avoid bug reported by Coverity
   Grp_GetLstCodsGrpWanted (&LstGrpsUsrWants);

   /***** A student can not be enroled in more than one group
          if the type of group is of single enrolment *****/
   if (Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB == Rol_STUDENT &&
       LstGrpsUsrWants.NumGrps >= 2)
      SelectionIsValid = Grp_CheckIfSelectionGrpsIsValid (&LstGrpsUsrWants);

   /***** Free list of groups types and groups in this course *****/
   // The lists of group types and groups need to be freed here...
   // ...in order to get them again when changing groups atomically
   Grp_FreeListGrpTypesAndGrps ();

   /***** Register user in the selected groups *****/
   if (SelectionIsValid)
     {
      if (Grp_ChangeGrpsOtherUsrAtomically (&LstGrpsUsrWants))
	 Lay_ShowAlert (Lay_SUCCESS,Txt_The_requested_group_changes_were_successful);
      else
	 Lay_ShowAlert (Lay_WARNING,Txt_There_has_been_no_change_in_groups);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_In_a_type_of_group_with_single_enrolment_students_can_not_be_registered_in_more_than_one_group);

   /***** Free memory with the list of groups to/from which register/remove users *****/
   Grp_FreeListCodGrp (&LstGrpsUsrWants);
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
   DB_Query ("LOCK TABLES crs_grp_types WRITE,crs_grp WRITE,"
	     "crs_grp_usr WRITE,crs_usr READ",
	     "Can not lock tables to change user's groups");
   Gbl.DB.LockedTables = true;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   /***** Query in the database the group codes which I belong to *****/
   Grp_GetLstCodGrpsUsrBelongs (Gbl.CurrentCrs.Crs.CrsCod,-1L,
				Gbl.Usrs.Me.UsrDat.UsrCod,&LstGrpsIBelong);

   if (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT)
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
		 NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num && !ITryToLeaveAClosedGroup;
		 NumGrpTyp++)
	      {
	       GrpTyp = &Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];
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
		    NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num &&
				!ITryToRegisterInAClosedGroup &&
				!ITryToRegisterInFullGroup;
		    NumGrpTyp++)
		 {
		  GrpTyp = &Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];
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
			else if ((GrpTyp->LstGrps[NumGrpThisType]).NumStudents >=
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
   Gbl.DB.LockedTables = false;	// Set to false before the following unlock...
				     // ...to not retry the unlock if error in unlocking
   DB_Query ("UNLOCK TABLES",
	     "Can not unlock tables after changes in user's groups");

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();

   return ChangesMade;
  }

/*****************************************************************************/
/********************** Change my groups atomically **************************/
/*****************************************************************************/
// Return true if desired changes are made

bool Grp_ChangeGrpsOtherUsrAtomically (struct ListCodGrps *LstGrpsUsrWants)
  {
   struct ListCodGrps LstGrpsUsrBelongs;
   unsigned NumGrpUsrBelongs;
   unsigned NumGrpUsrWants;
   bool RemoveUsrFromThisGrp;
   bool RegisterUsrInThisGrp;
   bool ChangesMade = false;

   if (Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB == Rol_STUDENT)
     {
      /***** Lock tables to make the inscription atomic *****/
      DB_Query ("LOCK TABLES crs_grp_types WRITE,crs_grp WRITE,"
		"crs_grp_usr WRITE,crs_usr READ",
		"Can not lock tables to change user's groups");
      Gbl.DB.LockedTables = true;
     }

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   /***** Query in the database the group codes which user belongs to *****/
   Grp_GetLstCodGrpsUsrBelongs (Gbl.CurrentCrs.Crs.CrsCod,-1L,
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

   ChangesMade = true;

   /***** Free memory with the list of groups which I belonged to *****/
   Grp_FreeListCodGrp (&LstGrpsUsrBelongs);

   /***** Unlock tables after changes in my groups *****/
   if (Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB == Rol_STUDENT)
     {
      Gbl.DB.LockedTables = false;	// Set to false before the following unlock...
				     // ...to not retry the unlock if error in unlocking
      DB_Query ("UNLOCK TABLES",
		"Can not unlock tables after changes in user's groups");
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();

   return ChangesMade;
  }

/*****************************************************************************/
/***** Check if no se ha selected más of a group of single enrolment ********/
/*****************************************************************************/

bool Grp_CheckIfSelectionGrpsIsValid (struct ListCodGrps *LstGrps)
  {
   struct ListGrpsAlreadySelec *AlreadyExistsGroupOfType;
   unsigned NumCodGrp;
   unsigned NumGrpTyp;
   long GrpTypCod;
   bool SelectionValid = true;

   /***** Create and initialize list of groups already selected *****/
   Grp_ConstructorListGrpAlreadySelec (&AlreadyExistsGroupOfType);

   /***** Go across the list of groups selected checking if a group of the same type
          is already selected *****/
   for (NumCodGrp = 0;
	SelectionValid && NumCodGrp < LstGrps->NumGrps;
	NumCodGrp++)
     {
      GrpTypCod = Grp_GetTypeOfGroupOfAGroup (LstGrps->GrpCods[NumCodGrp]);
      if (!Grp_GetMultipleEnrolmentOfAGroupType (GrpTypCod))
         for (NumGrpTyp = 0;
	      NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
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
  }

/*****************************************************************************/
/*********** Create e inicializar list of groups already selected ************/
/*****************************************************************************/

static void Grp_ConstructorListGrpAlreadySelec (struct ListGrpsAlreadySelec **AlreadyExistsGroupOfType)
  {
   unsigned NumGrpTyp;

   /***** Allocate memory to a list of booleanos that indica if already se ha selected a group of cada type *****/
   if ((*AlreadyExistsGroupOfType = (struct ListGrpsAlreadySelec *) calloc (Gbl.CurrentCrs.Grps.GrpTypes.Num,sizeof (struct ListGrpsAlreadySelec))) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store type of group.");

   /***** Initialize the list *****/
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	NumGrpTyp++)
     {
      (*AlreadyExistsGroupOfType)[NumGrpTyp].GrpTypCod = Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod;
      (*AlreadyExistsGroupOfType)[NumGrpTyp].AlreadySelected = false;
     }
  }

/*****************************************************************************/
/***************** Liberar list of groups already selected *******************/
/*****************************************************************************/

static void Grp_DestructorListGrpAlreadySelec (struct ListGrpsAlreadySelec **AlreadyExistsGroupOfType)
  {
   free ((void *) *AlreadyExistsGroupOfType);
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
	NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	NumGrpTyp++)
     {
      MultipleEnrolment = Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MultipleEnrolment;

      /***** Query in the database the group codes of any group of this type the student belongs to *****/
      LstGrpsHeBelongs.NumGrps = 0;	// Initialized to avoid bug reported by Coverity
      LstGrpsHeBelongs.GrpCods = NULL;	// Initialized to avoid bug reported by Coverity
      Grp_GetLstCodGrpsUsrBelongs (Gbl.CurrentCrs.Crs.CrsCod,Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod,
	                           UsrDat->UsrCod,&LstGrpsHeBelongs);

      /***** For each group selected by me... *****/
      for (NumGrpSel = 0;
	   NumGrpSel < LstGrps->NumGrps;
	   NumGrpSel++)
        {
         /* Check if the selected group is of this type */
         for (NumGrpThisType = 0;
              NumGrpThisType < Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps;
              NumGrpThisType++)
            if (LstGrps->GrpCods[NumGrpSel] == Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].LstGrps[NumGrpThisType].GrpCod)
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
                     sprintf (Gbl.Message,Txt_THE_USER_X_has_been_removed_from_the_group_of_type_Y_to_which_it_belonged,
			      UsrDat->FullName,Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName);
                     Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
                    }

               if (!AlreadyRegisteredInGrp)	// If the user does not belong to the selected group
                 {
                  Grp_AddUsrToGroup (UsrDat,LstGrps->GrpCods[NumGrpSel]);
                  sprintf (Gbl.Message,Txt_THE_USER_X_has_been_enroled_in_the_group_of_type_Y_Z,
		           UsrDat->FullName,Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName,
                           Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].LstGrps[NumGrpThisType].GrpName);
                  Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
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
   unsigned NumGrpSel,NumGrpHeBelongs,NumGrpsHeIsRemoved = 0;

   /***** Query in the database the group codes of any group the user belongs to *****/
   Grp_GetLstCodGrpsUsrBelongs (Gbl.CurrentCrs.Crs.CrsCod,-1L,
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
      sprintf (Gbl.Message,Txt_THE_USER_X_has_not_been_removed_from_any_group,
               UsrDat->FullName);
   else if (NumGrpsHeIsRemoved == 1)
      sprintf (Gbl.Message,Txt_THE_USER_X_has_been_removed_from_one_group,
               UsrDat->FullName);
   else	// NumGrpsHeIsRemoved > 1
      sprintf (Gbl.Message,Txt_THE_USER_X_has_been_removed_from_Y_groups,
               UsrDat->FullName,NumGrpsHeIsRemoved);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Free the list of groups of this type to which the user belonged *****/
   Grp_FreeListCodGrp (&LstGrpsHeBelongs);

   return NumGrpsHeIsRemoved;
  }

/*****************************************************************************/
/*************** Remove a user of all the groups of a course *****************/
/*****************************************************************************/

void Grp_RemUsrFromAllGrpsInCrs (struct UsrData *UsrDat,struct Course *Crs,Cns_QuietOrVerbose_t QuietOrVerbose)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_from_all_groups_of_the_course_Y;
   char Query[512];

   /***** Remove user from all the groups of the course *****/
   sprintf (Query,"DELETE FROM crs_grp_usr"
	          " WHERE UsrCod=%ld AND GrpCod IN"
                  " (SELECT crs_grp.GrpCod FROM crs_grp_types,crs_grp"
                  " WHERE crs_grp_types.CrsCod=%ld"
                  " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod)",
            UsrDat->UsrCod,Crs->CrsCod);
   DB_QueryDELETE (Query,"can not remove a user from all groups of a course");

   /***** Write message to show the change made *****/
   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_THE_USER_X_has_been_removed_from_all_groups_of_the_course_Y,
               UsrDat->FullName,Crs->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
  }

/*****************************************************************************/
/******* Remove a user from all the groups of all the user's courses *********/
/*****************************************************************************/

void Grp_RemUsrFromAllGrps (struct UsrData *UsrDat,Cns_QuietOrVerbose_t QuietOrVerbose)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_from_all_groups_in_all_courses;
   char Query[128];

   /***** Remove user from all groups *****/
   sprintf (Query,"DELETE FROM crs_grp_usr WHERE UsrCod=%ld",
            UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove a user from the groups he/she belongs to");

   /***** Write message to show the change made *****/
   if (QuietOrVerbose == Cns_VERBOSE)
     {
      sprintf (Gbl.Message,Txt_THE_USER_X_has_been_removed_from_all_groups_in_all_courses,
               UsrDat->FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
  }

/*****************************************************************************/
/************************* Remove a user from a group ************************/
/*****************************************************************************/

static void Grp_RemoveUsrFromGroup (long UsrCod,long GrpCod)
  {
   char Query[256];

   /***** Remove user from group *****/
   sprintf (Query,"DELETE FROM crs_grp_usr"
	          " WHERE GrpCod=%ld AND UsrCod=%ld",
            GrpCod,UsrCod);
   DB_QueryDELETE (Query,"can not remove a user from a group");
  }

/*****************************************************************************/
/*********************** Register a user in a group **************************/
/*****************************************************************************/

static void Grp_AddUsrToGroup (struct UsrData *UsrDat,long GrpCod)
  {
   char Query[256];

   /***** Register in group *****/
   sprintf (Query,"INSERT INTO crs_grp_usr"
	          " (GrpCod,UsrCod)"
                  " VALUES"
                  " (%ld,%ld)",
            GrpCod,UsrDat->UsrCod);
   DB_QueryINSERT (Query,"can not add a user to a group");
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
   Lay_StartTableWide (2);
   Grp_WriteHeadingGroupTypes ();

   /***** List group types with forms for edition *****/
   for (NumGrpTyp = 0, UniqueId=1;
	NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	NumGrpTyp++, UniqueId++)
     {
      /* Put icon to remove group type */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"BM\">");
      Act_FormStartAnchor (ActReqRemGrpTyp,Grp_SECTION_GROUP_TYPES);
      Grp_PutParamGrpTypCod (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
      Lay_PutIconRemove ();
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Name of group type */
      fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">");
      Act_FormStartAnchor (ActRenGrpTyp,Grp_SECTION_GROUP_TYPES);
      Grp_PutParamGrpTypCod (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"GrpTypName\""
	                 " size=\"12\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Grp_MAX_CHARS_GROUP_TYPE_NAME,
               Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName,
               Gbl.Form.Id);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Is it mandatory to register in any group? */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStartAnchor (ActChgMdtGrpTyp,Grp_SECTION_GROUP_TYPES);
      Grp_PutParamGrpTypCod (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
      fprintf (Gbl.F.Out,"<select name=\"MandatoryEnrolment\""
	                 " style=\"width:150px;\""
	                 " onchange=\"document.getElementById('%s').submit();\">"
                         "<option value=\"N\"",
               Gbl.Form.Id);
      if (!Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MandatoryEnrolment)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>"
	                 "<option value=\"Y\"",
               Txt_It_is_optional_to_choose_a_group);
      if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MandatoryEnrolment)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>"
	                 "</select>",
               Txt_It_is_mandatory_to_choose_a_group);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Is it possible to register in multiple groups? */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStartAnchor (ActChgMulGrpTyp,Grp_SECTION_GROUP_TYPES);
      Grp_PutParamGrpTypCod (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
      fprintf (Gbl.F.Out,"<select name=\"MultipleEnrolment\""
	                 " style=\"width:150px;\""
	                 " onchange=\"document.getElementById('%s').submit();\">"
                         "<option value=\"N\"",
               Gbl.Form.Id);
      if (!Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MultipleEnrolment)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>"
	                 "<option value=\"Y\"",
               Txt_A_student_can_only_belong_to_one_group);
      if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MultipleEnrolment)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>"
	                 "</select>",
               Txt_A_student_can_belong_to_several_groups);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Open time */
      fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">");
      Act_FormStartAnchor (ActChgTimGrpTyp,Grp_SECTION_GROUP_TYPES);
      Grp_PutParamGrpTypCod (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
      Lay_StartTableCenter (2);
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"LEFT_MIDDLE\" style=\"width:20px;\">"
                         "<img src=\"%s/%s16x16.gif\""
                         " alt=\"%s\" title=\"%s\""
                         " class=\"ICO20x20\" />"
                         "</td>"
	                 "<td class=\"LEFT_MIDDLE\">",
               Gbl.Prefs.IconsURL,
               Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MustBeOpened ? "time" :
        	                                                                  "time-off",
               Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MustBeOpened ? Txt_The_groups_will_automatically_open :
        	                                                                  Txt_The_groups_will_not_automatically_open,
               Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].MustBeOpened ? Txt_The_groups_will_automatically_open :
        	                                                                  Txt_The_groups_will_not_automatically_open);
      sprintf (Id,"open_time_%u",UniqueId);
      Dat_WriteFormClientLocalDateTimeFromTimeUTC (Id,
                                                   "Open",
						   Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].OpenTimeUTC,
						   Gbl.Now.Date.Year,
						   Gbl.Now.Date.Year + 1,
				                   Dat_FORM_SECONDS_ON,
				                   Dat_HMS_DO_NOT_SET,	// Don't set hour, minute and second
				                   true);		// Submit on change
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");
      Lay_EndTable ();
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Number of groups of this type */
      fprintf (Gbl.F.Out,"<td class=\"DAT CENTER_MIDDLE\">"
	                 "%u"
	                 "</td>"
	                 "</tr>",
               Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps);
     }

   /***** End table *****/
   Lay_EndTable ();
  }

/*****************************************************************************/
/************ Put contextual icons in edition of types of group **************/
/*****************************************************************************/

static void Grp_PutIconsEditingGroupTypes (void)
  {
   /***** Put icon to view groups *****/
   Grp_PutIconToViewGroups ();

   /***** Put icon to create a new type of group *****/
   Grp_PutIconToCreateNewGroupType ();
  }

static void Grp_PutIconToViewGroups (void)
  {
   Lay_PutContextualIconToView (ActReqSelGrp,NULL);
  }

static void Grp_PutIconToCreateNewGroupType (void)
  {
   extern const char *Txt_New_type_of_group;

   /***** Put form to create a new type of group *****/
   Lay_PutContextualLink (ActReqEdiGrp,Grp_SECTION_NEW_GROUP_TYPE,NULL,
                          "plus64x64.png",
                          Txt_New_type_of_group,NULL,
                          NULL);
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
   extern const char *Txt_No_of_BR_groups;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"BM\"></th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s<br />(%s)"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Type_of_group,Txt_eg_Lectures_Practicals,
            Txt_Mandatory_enrolment,
            Txt_Multiple_enrolment,
            Txt_Opening_of_groups,
            Txt_No_of_BR_groups);
  }

/*****************************************************************************/
/********************** List current groups for edition **********************/
/*****************************************************************************/

static void Grp_ListGroupsForEdition (void)
  {
   extern const char *Txt_Group_X_open_click_to_close_it;
   extern const char *Txt_Group_X_closed_click_to_open_it;
   extern const char *Txt_File_zones_of_the_group_X_enabled_click_to_disable_them;
   extern const char *Txt_File_zones_of_the_group_X_disabled_click_to_enable_them;
   unsigned NumGrpTyp;
   unsigned NumTipGrpAux;
   unsigned NumGrpThisType;
   struct GroupType *GrpTyp;
   struct GroupType *GrpTypAux;
   struct Group *Grp;

   /***** Write heading *****/
   Lay_StartTableWide (2);
   Grp_WriteHeadingGroups ();

   /***** List the groups *****/
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	NumGrpTyp++)
     {
      GrpTyp = &Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];
      for (NumGrpThisType = 0;
	   NumGrpThisType < GrpTyp->NumGrps;
	   NumGrpThisType++)
        {
         Grp = &(GrpTyp->LstGrps[NumGrpThisType]);

         /* Write icon to remove group */
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"BM\">");
         Act_FormStartAnchor (ActReqRemGrp,Grp_SECTION_GROUPS);
         Grp_PutParamGrpCod (Grp->GrpCod);
         Lay_PutIconRemove ();
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</td>");

         /* Write icon to open/close group */
         fprintf (Gbl.F.Out,"<td class=\"BM\">");
         Act_FormStartAnchor (Grp->Open ? ActCloGrp :
                                          ActOpeGrp,
                              Grp_SECTION_GROUPS);
         Grp_PutParamGrpCod (Grp->GrpCod);
         sprintf (Gbl.Title,
                  Grp->Open ? Txt_Group_X_open_click_to_close_it :
                              Txt_Group_X_closed_click_to_open_it,
                  Grp->GrpName);
         fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s-on64x64.png\""
                            " alt=\"%s\" title=\"%s\""
                            " class=\"ICO20x20\" />",
                  Gbl.Prefs.IconsURL,
                  Grp->Open ? "unlock" :
                	      "lock",
                  Gbl.Title,
                  Gbl.Title);
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</td>");

         /* Write icon to activate file zones for this group */
         fprintf (Gbl.F.Out,"<td class=\"BM\">");
         Act_FormStartAnchor (Grp->FileZones ? ActDisFilZonGrp :
                                               ActEnaFilZonGrp,
                              Grp_SECTION_GROUPS);
         Grp_PutParamGrpCod (Grp->GrpCod);
         sprintf (Gbl.Title,
                  Grp->FileZones ? Txt_File_zones_of_the_group_X_enabled_click_to_disable_them :
                                   Txt_File_zones_of_the_group_X_disabled_click_to_enable_them,
                  Grp->GrpName);
         fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s16x16.gif\""
                            " alt=\"%s\" title=\"%s\""
                            " class=\"ICO20x20\" />",
                  Gbl.Prefs.IconsURL,
                  Grp->FileZones ? "folder-yes" :
                	           "folder-no",
                  Gbl.Title,
                  Gbl.Title);
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</td>");

         /* Group type */
         fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
         Act_FormStartAnchor (ActChgGrpTyp,Grp_SECTION_GROUPS);
         Grp_PutParamGrpCod (Grp->GrpCod);
         fprintf (Gbl.F.Out,"<select name=\"GrpTypCod\" style=\"width:150px;\""
                            " onchange=\"document.getElementById('%s').submit();\">",
                  Gbl.Form.Id);
         for (NumTipGrpAux = 0;
              NumTipGrpAux < Gbl.CurrentCrs.Grps.GrpTypes.Num;
              NumTipGrpAux++)
           {
            GrpTypAux = &Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumTipGrpAux];
            fprintf (Gbl.F.Out,"<option value=\"%ld\"",GrpTypAux->GrpTypCod);
            if (GrpTypAux->GrpTypCod == GrpTyp->GrpTypCod)
	       fprintf (Gbl.F.Out," selected=\"selected\"");
            fprintf (Gbl.F.Out,">%s</option>",GrpTypAux->GrpTypName);
           }
         fprintf (Gbl.F.Out,"</select>");
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</td>");

         /* Group name */
         fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
         Act_FormStartAnchor (ActRenGrp,Grp_SECTION_GROUPS);
         Grp_PutParamGrpCod (Grp->GrpCod);
         fprintf (Gbl.F.Out,"<input type=\"text\" name=\"GrpName\""
                            " size=\"40\" maxlength=\"%u\" value=\"%s\""
                            " onchange=\"document.getElementById('%s').submit();\" />",
                  Grp_MAX_CHARS_GROUP_NAME,Grp->GrpName,Gbl.Form.Id);
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</td>");

         /* Maximum number of students of the group (row[3]) */
         fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
         Act_FormStartAnchor (ActChgMaxStdGrp,Grp_SECTION_GROUPS);
         Grp_PutParamGrpCod (Grp->GrpCod);
         fprintf (Gbl.F.Out,"<input type=\"text\" name=\"MaxStudents\""
                            " size=\"3\" maxlength=\"10\" value=\"");
         Grp_WriteMaxStdsGrp (Grp->MaxStudents);
         fprintf (Gbl.F.Out,"\" onchange=\"document.getElementById('%s').submit();\" />",
                  Gbl.Form.Id);
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</td>");

         /* Current number of students in this group */
         fprintf (Gbl.F.Out,"<td class=\"DAT CENTER_MIDDLE\">"
                            "%d"
                            "</td>"
                            "</tr>",
                  Grp->NumStudents);
        }
     }

   /***** End table *****/
   Lay_EndTable ();
  }

/*****************************************************************************/
/************************** Write heading of groups **************************/
/*****************************************************************************/

static void Grp_WriteHeadingGroups (void)
  {
   extern const char *Txt_Type_BR_of_group;
   extern const char *Txt_Group_name;
   extern const char *Txt_eg_A_B;
   extern const char *Txt_Max_BR_students;
   extern const char *Txt_Students_ABBREVIATION;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"BM\"></th>"
                      "<th class=\"BM\"></th>"
                      "<th class=\"BM\"></th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s<br />(%s)"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Type_BR_of_group,
            Txt_Group_name,Txt_eg_A_B,
            Txt_Max_BR_students,
            Txt_Students_ABBREVIATION);
  }

/*****************************************************************************/
/* List groups of a type to edit assignments, attendance events, or surveys **/
/*****************************************************************************/

void Grp_ListGrpsToEditAsgAttOrSvy (struct GroupType *GrpTyp,long Cod,Grp_AsgOrSvy_t Grp_AsgAttOrSvy)
  {
   struct ListCodGrps LstGrpsIBelong;
   unsigned NumGrpThisType;
   bool IBelongToThisGroup;
   struct Group *Grp;
   bool AssociatedToGrp = false;

   /***** Write heading *****/
   Grp_WriteGrpHead (GrpTyp);

   /***** Query from the database the groups of this type which I belong to *****/
   Grp_GetLstCodGrpsUsrBelongs (Gbl.CurrentCrs.Crs.CrsCod,GrpTyp->GrpTypCod,
	                        Gbl.Usrs.Me.UsrDat.UsrCod,&LstGrpsIBelong);

   /***** List the groups *****/
   for (NumGrpThisType = 0;
	NumGrpThisType < GrpTyp->NumGrps;
	NumGrpThisType++)
     {
      Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
      IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsIBelong);

      /* Put checkbox to select the group */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LEFT_MIDDLE");
      if (IBelongToThisGroup)
	 fprintf (Gbl.F.Out," LIGHT_BLUE");
      fprintf (Gbl.F.Out,"\">"
	                 "<input type=\"checkbox\""
	                 " id=\"Grp%ld\" name=\"GrpCods\" value=\"%ld\"",
               Grp->GrpCod,
               Grp->GrpCod);
      if (Cod > 0)	// Cod == -1L means new assignment or survey
        {
         switch (Grp_AsgAttOrSvy)
           {
            case Grp_ASSIGNMENT:
               AssociatedToGrp = Asg_CheckIfAsgIsAssociatedToGrp (Cod,Grp->GrpCod);
               break;
            case Grp_ATT_EVENT:
               AssociatedToGrp = Att_CheckIfAttEventIsAssociatedToGrp (Cod,Grp->GrpCod);
               break;
            case Grp_SURVEY:
               AssociatedToGrp = Svy_CheckIfSvyIsAssociatedToGrp (Cod,Grp->GrpCod);
               break;
           }
         if (AssociatedToGrp)
            fprintf (Gbl.F.Out," checked=\"checked\"");
        }
      if (!(IBelongToThisGroup ||
            Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM))
         fprintf (Gbl.F.Out," disabled=\"disabled\"");
      fprintf (Gbl.F.Out," onclick=\"uncheckParent(this,'WholeCrs')\" />"
	                 "</td>");

      Grp_WriteRowGrp (Grp,IBelongToThisGroup);

      fprintf (Gbl.F.Out,"</tr>");
     }

   /***** Free memory with the list of groups which I belongs to *****/
   Grp_FreeListCodGrp (&LstGrpsIBelong);
  }

/*****************************************************************************/
/***************** Show list of groups to register/remove me *****************/
/*****************************************************************************/

void Grp_ReqRegisterInGrps (void)
  {
   /***** Show list of groups to register/remove me *****/
   Grp_ShowLstGrpsToChgMyGrps ((Gbl.Usrs.Me.LoggedRole == Rol_STUDENT));
  }

/*****************************************************************************/
/***************** Show list of groups to register/remove me *****************/
/*****************************************************************************/

void Grp_ShowLstGrpsToChgMyGrps (bool ShowWarningsToStudents)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *Txt_My_groups;
   extern const char *Txt_Change_my_groups;
   extern const char *Txt_Enrol_in_groups;
   extern const char *Txt_No_groups_have_been_created_in_the_course_X;
   extern const char *Txt_Create_group;
   unsigned NumGrpTyp;
   unsigned NumGrpsIBelong = 0;
   bool PutFormToChangeGrps = !Gbl.Form.Inside;	// Not inside another form (record card)
   bool ICanEdit = !Gbl.Form.Inside &&
	           (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER ||
                    Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);

   if (Gbl.CurrentCrs.Grps.NumGrps) // This course has groups
     {
      /***** Get list of groups types and groups in this course *****/
      Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

      /***** Show warnings to students *****/
      // Students are required to join groups with mandatory enrolment; teachers don't
      if (ShowWarningsToStudents)
	 Grp_ShowWarningToStdsToChangeGrps ();
     }

   /***** Start frame *****/
   Lay_StartRoundFrame (NULL,Txt_My_groups,
                        ICanEdit ? Grp_PutIconToEditGroups :
                                   NULL,
                        Hlp_USERS_Groups);

   if (Gbl.CurrentCrs.Grps.NumGrps) // This course has groups
     {
      /***** Start form *****/
      if (PutFormToChangeGrps)
	 Act_FormStart (ActChgGrp);

      /***** List the groups the user belongs to for change *****/
      Lay_StartTableWide (2);
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
	 if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)	 // If there are groups of this type
	    NumGrpsIBelong += Grp_ListGrpsForChange (&Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp]);
      Lay_EndTable ();

      /***** End form *****/
      if (PutFormToChangeGrps)
	{
	 Lay_PutConfirmButton (NumGrpsIBelong ? Txt_Change_my_groups :
						Txt_Enrol_in_groups);
	 Act_FormEnd ();
	}
     }
   else	// This course has no groups
     {
      sprintf (Gbl.Message,Txt_No_groups_have_been_created_in_the_course_X,
               Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);

      /***** Button to create group *****/
      if (ICanEdit)
	{
	 Act_FormStart (ActReqEdiGrp);
	 Lay_PutConfirmButton (Txt_Create_group);
	 Act_FormEnd ();
	}
     }

   /***** End frame *****/
   Lay_EndRoundFrame ();

   if (Gbl.CurrentCrs.Grps.NumGrps) // This course has groups
      /***** Free list of groups types and groups in this course *****/
      Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/*************************** Put icon to edit groups *************************/
/*****************************************************************************/

static void Grp_PutIconToEditGroups (void)
  {
   Lay_PutContextualIconToEdit (ActReqEdiGrp,NULL);
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
	NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	NumGrpTyp++)
     {
      GrpTyp = &Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];
      if (GrpTyp->NumGrps)	 // If there are groups of this type
	 if (Grp_GetFirstCodGrpStdBelongsTo (GrpTyp->GrpTypCod,Gbl.Usrs.Me.UsrDat.UsrCod) < 0)  // If the student does not belong to any group
	    if (Grp_GetIfGrpIsAvailable (GrpTyp->GrpTypCod))	// If there is any group of this type available
	      {
	       if (GrpTyp->MandatoryEnrolment)
		 {
		  sprintf (Gbl.Message,
			   GrpTyp->MultipleEnrolment ? Txt_You_have_to_register_compulsorily_at_least_in_one_group_of_type_X :
						       Txt_You_have_to_register_compulsorily_in_one_group_of_type_X,
			   GrpTyp->GrpTypName);
		  Lay_ShowAlert (Lay_WARNING,Gbl.Message);
		 }
	       else
		 {
		  sprintf (Gbl.Message,
			   GrpTyp->MultipleEnrolment ? Txt_You_can_register_voluntarily_in_one_or_more_groups_of_type_X :
						       Txt_You_can_register_voluntarily_in_one_group_of_type_X,
			   GrpTyp->GrpTypName);
		  Lay_ShowAlert (Lay_INFO,Gbl.Message);
		 }
	      }
     }
  }

/*****************************************************************************/
/*************** List the groups of a type to register in ********************/
/*****************************************************************************/
// Returns the number of groups of this type I belong to

static unsigned Grp_ListGrpsForChange (struct GroupType *GrpTyp)
  {
   struct ListCodGrps LstGrpsIBelong;
   unsigned NumGrpThisType;
   struct Group *Grp;
   bool IBelongToThisGroup;
   unsigned NumGrpsIBelong;

   /***** Write heading *****/
   Grp_WriteGrpHead (GrpTyp);

   /***** Query in the database the group of this type that I belong to *****/
   Grp_GetLstCodGrpsUsrBelongs (Gbl.CurrentCrs.Crs.CrsCod,GrpTyp->GrpTypCod,
	                        Gbl.Usrs.Me.UsrDat.UsrCod,&LstGrpsIBelong);
   NumGrpsIBelong = LstGrpsIBelong.NumGrps;

   /***** List the groups *****/
   for (NumGrpThisType = 0;
	NumGrpThisType < GrpTyp->NumGrps;
	NumGrpThisType++)
     {
      Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
      IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsIBelong);

      /* Put icon to select the group */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LEFT_MIDDLE");
      if (IBelongToThisGroup)
         fprintf (Gbl.F.Out," LIGHT_BLUE");
      fprintf (Gbl.F.Out,"\">"
	                 "<input type=\"");

      // If user is a student and the enrolment is single
      // and there are more than a group, put a radio item
      if (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT &&
          !GrpTyp->MultipleEnrolment &&
          GrpTyp->NumGrps > 1)
	{
         fprintf (Gbl.F.Out,"radio\" id=\"Grp%ld\" name=\"GrpCod%ld\""
                            " value=\"%ld\"",
                  Grp->GrpCod,GrpTyp->GrpTypCod,
                  Grp->GrpCod);
         if (!GrpTyp->MandatoryEnrolment)	// If the enrolment is not mandatory, I can select no groups
            fprintf (Gbl.F.Out," onclick=\"selectUnselectRadio(this,this.form.GrpCod%ld,%u)\"",
                     GrpTyp->GrpTypCod,GrpTyp->NumGrps);
	}
      else // Put a checkbox item
         fprintf (Gbl.F.Out,"checkbox\" id=\"Grp%ld\" name=\"GrpCod%ld\""
                            " value=\"%ld\"",
                  Grp->GrpCod,GrpTyp->GrpTypCod,
                  Grp->GrpCod);

      if (IBelongToThisGroup)
	 fprintf (Gbl.F.Out," checked=\"checked\"");
      else if ((Gbl.Usrs.Me.LoggedRole == Rol_STUDENT) &&
               ((!Grp->Open) || (Grp->NumStudents >= Grp->MaxStudents)))
         fprintf (Gbl.F.Out," disabled=\"disabled\"");
      fprintf (Gbl.F.Out," /></td>");

      Grp_WriteRowGrp (Grp,IBelongToThisGroup);

      fprintf (Gbl.F.Out,"</tr>");
     }

   /***** Free memory with the list of groups a the that belongs the user *****/
   Grp_FreeListCodGrp (&LstGrpsIBelong);

   return NumGrpsIBelong;
  }

/*****************************************************************************/
/*************** Show list of groups to register/remove users ****************/
/*****************************************************************************/
// If UsrCod > 0 ==> mark her/his groups as checked
// If UsrCod <= 0 ==> do not mark any group as checked

void Grp_ShowLstGrpsToChgOtherUsrsGrps (long UsrCod)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *Txt_Groups;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in current course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   /***** Start table *****/
   Lay_StartRoundFrameTable (NULL,Txt_Groups,NULL,Hlp_USERS_Groups,0);

   /***** List to select the groups the user belongs to *****/
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	NumGrpTyp++)
      if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
	 Grp_ListGrpsToAddOrRemUsrs (&Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],UsrCod);

   /***** End table *****/
   Lay_EndRoundFrameTable ();

   /***** Free list of groups types and groups in current course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/*************** List groups of a type to add or remove users ****************/
/*****************************************************************************/
// If UsrCod > 0 ==> mark her/his groups as checked
// If UsrCod <= 0 ==> do not mark any group as checked

static void Grp_ListGrpsToAddOrRemUsrs (struct GroupType *GrpTyp,long UsrCod)
  {
   struct ListCodGrps LstGrpsIBelong;
   struct ListCodGrps LstGrpsUsrBelongs;
   unsigned NumGrpThisType;
   bool IBelongToThisGroup;
   bool UsrBelongsToThisGroup;
   struct Group *Grp;

   /***** Write heading *****/
   Grp_WriteGrpHead (GrpTyp);

   /***** Query from the database the groups of this type which I belong to *****/
   Grp_GetLstCodGrpsUsrBelongs (Gbl.CurrentCrs.Crs.CrsCod,GrpTyp->GrpTypCod,
	                        Gbl.Usrs.Me.UsrDat.UsrCod,&LstGrpsIBelong);

   /***** Query from the database the groups of this type which I belong to *****/
   if (UsrCod > 0)
      Grp_GetLstCodGrpsUsrBelongs (Gbl.CurrentCrs.Crs.CrsCod,GrpTyp->GrpTypCod,
				   Gbl.Usrs.Other.UsrDat.UsrCod,&LstGrpsUsrBelongs);

   /***** List the groups *****/
   for (NumGrpThisType = 0;
	NumGrpThisType < GrpTyp->NumGrps;
	NumGrpThisType++)
     {
      Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
      IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsIBelong);
      UsrBelongsToThisGroup = (UsrCod > 0) ? Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsUsrBelongs) :
	                                     false;

      /* Put checkbox to select the group */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LEFT_MIDDLE");
      if (UsrBelongsToThisGroup)
	 fprintf (Gbl.F.Out," LIGHT_BLUE");
      fprintf (Gbl.F.Out,"\">"
	                 "<input type=\"checkbox\""
	                 " id=\"Grp%ld\" name=\"GrpCod%ld\" value=\"%ld\"",
               Grp->GrpCod,GrpTyp->GrpTypCod,
               Grp->GrpCod);
      if (UsrBelongsToThisGroup)
      	 fprintf (Gbl.F.Out," checked=\"checked\"");
      if (!(IBelongToThisGroup ||
            Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM))
         fprintf (Gbl.F.Out," disabled=\"disabled\"");
      fprintf (Gbl.F.Out," /></td>");

      Grp_WriteRowGrp (Grp,UsrBelongsToThisGroup);

      fprintf (Gbl.F.Out,"</tr>");
     }

   /***** Free memory with the lists of groups *****/
   if (UsrCod > 0)
      Grp_FreeListCodGrp (&LstGrpsUsrBelongs);
   Grp_FreeListCodGrp (&LstGrpsIBelong);
  }

/*****************************************************************************/
/******* Write a list of groups as checkbox form for unique selection ********/
/*****************************************************************************/

static void Grp_ListGrpsForMultipleSelection (struct GroupType *GrpTyp)
  {
   extern const char *Txt_students_with_no_group;
   unsigned NumGrpThisType;
   unsigned NumGrpSel;
   struct ListCodGrps LstGrpsIBelong;
   bool IBelongToThisGroup;
   struct Group *Grp;

   /***** Write heading *****/
   Grp_WriteGrpHead (GrpTyp);

   /***** Query from the database the groups of this type which I belong to *****/
   Grp_GetLstCodGrpsUsrBelongs (Gbl.CurrentCrs.Crs.CrsCod,GrpTyp->GrpTypCod,
	                        Gbl.Usrs.Me.UsrDat.UsrCod,&LstGrpsIBelong);

   /***** List the groups *****/
   for (NumGrpThisType = 0;
	NumGrpThisType < GrpTyp->NumGrps;
	NumGrpThisType++)
     {
      Grp = &(GrpTyp->LstGrps[NumGrpThisType]);
      IBelongToThisGroup = Grp_CheckIfGrpIsInList (Grp->GrpCod,&LstGrpsIBelong);

      /* Put checkbox to select the group */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LEFT_MIDDLE");
      if (IBelongToThisGroup)
         fprintf (Gbl.F.Out," LIGHT_BLUE");
      fprintf (Gbl.F.Out,"\">"
	                 "<input type=\"checkbox\""
	                 " id=\"Grp%ld\" name=\"GrpCods\" value=\"%ld\"",
               Grp->GrpCod,
               Grp->GrpCod);
      if (Gbl.Usrs.ClassPhoto.AllGroups)
         fprintf (Gbl.F.Out," checked=\"checked\"");
      else
         for (NumGrpSel = 0;
              NumGrpSel < Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps;
              NumGrpSel++)
            if (Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCods[NumGrpSel] == Grp->GrpCod)
              {
               fprintf (Gbl.F.Out," checked=\"checked\"");
               break;
              }
      fprintf (Gbl.F.Out," onclick=\"checkParent(this,'AllGroups')\" /></td>");

      Grp_WriteRowGrp (Grp,IBelongToThisGroup);

      fprintf (Gbl.F.Out,"</tr>");
     }

   /***** Free memory with the list of groups which I belongs to *****/
   Grp_FreeListCodGrp (&LstGrpsIBelong);

   /***** Write rows to select the students who don't belong to any group *****/
   /* To get the students who don't belong to a type of group, use group code -(GrpTyp->GrpTypCod) */
   /* Write checkbox to select the group */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"checkbox\" id=\"Grp%ld\" name=\"GrpCods\""
                      " value=\"%ld\"",
            -(GrpTyp->GrpTypCod),
            -(GrpTyp->GrpTypCod));
   if (Gbl.Usrs.ClassPhoto.AllGroups)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   else
      for (NumGrpSel = 0;
	   NumGrpSel < Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps;
	   NumGrpSel++)
         if (Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCods[NumGrpSel] == -(GrpTyp->GrpTypCod))
           {
            fprintf (Gbl.F.Out," checked=\"checked\"");
            break;
           }
   fprintf (Gbl.F.Out," onclick=\"checkParent(this,'AllGroups')\" />"
	              "</td>");

   /* Column closed/open */
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
	              "</td>");

   /* Group name = students with no group */
   fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"DAT LEFT_MIDDLE\">"
	              "<label for=\"Grp%ld\">%s</label>"
	              "</td>",
            -(GrpTyp->GrpTypCod),Txt_students_with_no_group);

   /* Number of students who don't belong to any group of this type */
   fprintf (Gbl.F.Out,"<td class=\"DAT CENTER_MIDDLE\">"
	              "%u"
	              "</td>",
            Grp_CountNumStdsInNoGrpsOfType (GrpTyp->GrpTypCod));

   /* Last column */
   fprintf (Gbl.F.Out,"<td></td>"
	              "</tr>");
  }

/*****************************************************************************/
/************** Write a row with the head for list of groups *****************/
/*****************************************************************************/

static void Grp_WriteGrpHead (struct GroupType *GrpTyp)
  {
   extern const char *Txt_Opening_of_groups;
   extern const char *Txt_Today;
   extern const char *Txt_Group;
   extern const char *Txt_Max_BR_students;
   extern const char *Txt_Students_ABBREVIATION;
   extern const char *Txt_Vacants;
   static unsigned UniqueId = 0;

   /***** Name of group type *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"6\" class=\"GRP_TITLE LEFT_MIDDLE\">"
	              "<br />%s",
	    GrpTyp->GrpTypName);
   if (GrpTyp->MustBeOpened)
     {
      UniqueId++;
      fprintf (Gbl.F.Out,"<br />%s: "
                         "<span id=\"open_time_%u\"></span>"
                         "<script type=\"text/javascript\">"
                         "writeLocalDateHMSFromUTC('open_time_%u',"
                         "%ld,',&nbsp;','%s',true,true,true);"
                         "</script>",
               Txt_Opening_of_groups,
               UniqueId,
               UniqueId,(long) GrpTyp->OpenTimeUTC,Txt_Today);
     }
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");

   /***** Head row with title of each column *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th colspan=\"2\"></th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Group,
            Txt_Max_BR_students,
            Txt_Students_ABBREVIATION,
            Txt_Vacants);
  }

/*****************************************************************************/
/****************** Write a row with the data of a group *********************/
/*****************************************************************************/

static void Grp_WriteRowGrp (struct Group *Grp,bool Highlight)
  {
   extern const char *Txt_Group_X_open;
   extern const char *Txt_Group_X_closed;
   int Vacant;

   /***** Write icon to show if group is open or closed *****/
   sprintf (Gbl.Title,Grp->Open ? Txt_Group_X_open :
	                          Txt_Group_X_closed,
            Grp->GrpName);
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE");
   if (Highlight)
      fprintf (Gbl.F.Out," LIGHT_BLUE");
   fprintf (Gbl.F.Out,"\" style=\"width:15px;\">"
	              "<img src=\"%s/%s-off64x64.png\""
	              " alt=\"%s\" title=\"%s\""
	              " class=\"ICO20x20\" />"
	              "</td>",
            Gbl.Prefs.IconsURL,
            Grp->Open ? "unlock" :
        	        "lock",
	    Gbl.Title,Gbl.Title);

   /***** Group name *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE");
   if (Highlight)
      fprintf (Gbl.F.Out," LIGHT_BLUE");
   fprintf (Gbl.F.Out,"\">"
                      "<label for=\"Grp%ld\" class=\"DAT\">"
	              "%s"
	              "</label>"
	              "</td>",
	    Grp->GrpCod,
	    Grp->GrpName);

   /***** Max. number of students in this group *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT CENTER_MIDDLE");
   if (Highlight)
      fprintf (Gbl.F.Out," LIGHT_BLUE");
   fprintf (Gbl.F.Out,"\">");
   Grp_WriteMaxStdsGrp (Grp->MaxStudents);
   fprintf (Gbl.F.Out,"&nbsp;"
	              "</td>");

   /***** Current number of students in this group *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT CENTER_MIDDLE");
   if (Highlight)
      fprintf (Gbl.F.Out," LIGHT_BLUE");
   fprintf (Gbl.F.Out,"\">"
	              "%d"
	              "</td>",
	    Grp->NumStudents);

   /***** Vacants in this group *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT CENTER_MIDDLE");
   if (Highlight)
      fprintf (Gbl.F.Out," LIGHT_BLUE");
   fprintf (Gbl.F.Out,"\">");
   if (Grp->MaxStudents > Grp_MAX_STUDENTS_IN_A_GROUP)
      fprintf (Gbl.F.Out,"-");
   else
     {
      Vacant = (int) Grp->MaxStudents - (int) Grp->NumStudents;
      fprintf (Gbl.F.Out,"%u",
               Vacant > 0 ? (unsigned) Vacant :
        	                       0);
     }
   fprintf (Gbl.F.Out,"</td>");
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

   /***** Start form *****/
   fprintf (Gbl.F.Out,"<section id=\"%s\">",Grp_SECTION_NEW_GROUP_TYPE);
   Act_FormStartAnchor (ActNewGrpTyp,Grp_SECTION_GROUP_TYPES);

   /***** Start of frame *****/
   Lay_StartRoundFrameTable (NULL,Txt_New_type_of_group,
                             NULL,NULL,2);

   /***** Write heading *****/
   Grp_WriteHeadingGroupTypes ();

   /***** Column to remove group type, disabled here *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"BM\"></td>");

   /***** Name of group type *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" name=\"GrpTypName\""
                      " size=\"12\" maxlength=\"%u\" value=\"%s\""
	              " required=\"required\" />"
                      "</td>",
            Grp_MAX_CHARS_GROUP_TYPE_NAME,Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName);

   /***** Is it mandatory to register in any groups of this type? *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<select name=\"MandatoryEnrolment\""
                      " style=\"width:150px;\">"
                      "<option value=\"N\"");
   if (!Gbl.CurrentCrs.Grps.GrpTyp.MandatoryEnrolment)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>"
	              "<option value=\"Y\"",
            Txt_It_is_optional_to_choose_a_group);
   if (Gbl.CurrentCrs.Grps.GrpTyp.MandatoryEnrolment)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>"
	              "</select>"
	              "</td>",
            Txt_It_is_mandatory_to_choose_a_group);

   /***** Is it possible to register in multiple groups of this type? *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<select name=\"MultipleEnrolment\""
                      " style=\"width:150px;\">"
                      "<option value=\"N\"");
   if (!Gbl.CurrentCrs.Grps.GrpTyp.MultipleEnrolment)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>"
	              "<option value=\"Y\"",
            Txt_A_student_can_only_belong_to_one_group);
   if (Gbl.CurrentCrs.Grps.GrpTyp.MultipleEnrolment)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>"
	              "</select>"
	              "</td>",
            Txt_A_student_can_belong_to_several_groups);

   /***** Open time *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">");
   Lay_StartTable (2);
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"LEFT_MIDDLE\" style=\"width:20px;\">"
                      "<img src=\"%s/%s16x16.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />"
                      "</td>"
	              "<td class=\"LEFT_MIDDLE\">",
            Gbl.Prefs.IconsURL,
            Gbl.CurrentCrs.Grps.GrpTyp.MustBeOpened ? "time" :
        	                                      "time-off",
            Gbl.CurrentCrs.Grps.GrpTyp.MustBeOpened ? Txt_The_groups_will_automatically_open :
        	                                      Txt_The_groups_will_not_automatically_open,
            Gbl.CurrentCrs.Grps.GrpTyp.MustBeOpened ? Txt_The_groups_will_automatically_open :
        	                                      Txt_The_groups_will_not_automatically_open);
   Dat_WriteFormClientLocalDateTimeFromTimeUTC ("open_time",
                                                "Open",
                                                Gbl.CurrentCrs.Grps.GrpTyp.OpenTimeUTC,
                                                Gbl.Now.Date.Year,
                                                Gbl.Now.Date.Year + 1,
				                Dat_FORM_SECONDS_ON,
				                Dat_HMS_DO_NOT_SET,	// Don't set hour, minute and second
				                false);			// Don't submit on change
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");
   Lay_EndTable ();
   fprintf (Gbl.F.Out,"</td>");

   /***** Number of groups of this type *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT CENTER_MIDDLE\">"
		      "0"	// It's a new group type ==> 0 groups
		      "</td>"
		      "</tr>");

   /***** Send button and end frame *****/
   Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_type_of_group);

   /***** End form *****/
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</section>");
  }

/*****************************************************************************/
/*********************** Put a form to create a new group ********************/
/*****************************************************************************/

static void Grp_PutFormToCreateGroup (void)
  {
   extern const char *Txt_New_group;
   extern const char *Txt_Group_closed;
   extern const char *Txt_File_zones_disabled;
   extern const char *Txt_Create_group;
   unsigned NumGrpTyp;

   /***** Start form *****/
   fprintf (Gbl.F.Out,"<section id=\"%s\">",Grp_SECTION_NEW_GROUP);
   Act_FormStartAnchor (ActNewGrp,Grp_SECTION_GROUPS);

   /***** Start of frame *****/
   Lay_StartRoundFrameTable (NULL,Txt_New_group,NULL,NULL,2);

   /***** Write heading *****/
   Grp_WriteHeadingGroups ();

   /***** Put disabled icons to open group and archive zone *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"BM\"></td>"
                      "<td class=\"BM\">"
                      "<img src=\"%s/lock-off64x64.png\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />"
                      "</td>"
                      "<td class=\"BM\">"
                      "<img src=\"%s/folder-no_off16x16.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />"
                      "</td>",
            Gbl.Prefs.IconsURL,
            Txt_Group_closed,
            Txt_Group_closed,
            Gbl.Prefs.IconsURL,
            Txt_File_zones_disabled,
            Txt_File_zones_disabled);

   /***** Group type *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<select name=\"GrpTypCod\" style=\"width:150px;\">");
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	NumGrpTyp++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%ld\"",
	       Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
      if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod == Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",
	       Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName);
     }
   fprintf (Gbl.F.Out,"</select>"
	              "</td>");

   /***** Group name *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"GrpName\""
                      " size=\"40\" maxlength=\"%u\" value=\"%s\""
	              " required=\"required\" />"
	              "</td>",
            Grp_MAX_CHARS_GROUP_NAME,Gbl.CurrentCrs.Grps.GrpName);

   /***** Maximum number of students *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
	              "<input type=\"text\" name=\"MaxStudents\""
	              " size=\"3\" maxlength=\"10\" value=\"");
   Grp_WriteMaxStdsGrp (Gbl.CurrentCrs.Grps.MaxStudents);
   fprintf (Gbl.F.Out,"\" />"
	              "</td>");

   /***** Current number of students in this group *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT CENTER_MIDDLE\">"
		      "0"	// It's a new group ==> 0 students
		      "</td>"
		      "</tr>");

   /***** Send button and end frame *****/
   Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_group);

   /***** End of form *****/
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</section>");
  }

/*****************************************************************************/
/*********** Create a list with current group types in this course ***********/
/*****************************************************************************/

void Grp_GetListGrpTypesInThisCrs (Grp_WhichGroupTypes_t WhichGroupTypes)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;

   if (++Gbl.CurrentCrs.Grps.GrpTypes.NestedCalls > 1) // If list is created yet, there's nothing to do
      return;

   /***** Open groups of this course that must be opened
          if open time is in the past *****/
   Grp_OpenGroupsAutomatically ();

   /***** Get group types with groups + groups types without groups from database *****/
   // The tables in the second part of the UNION requires ALIAS in order to LOCK TABLES when registering in groups
   switch (WhichGroupTypes)
     {
      case Grp_ONLY_GROUP_TYPES_WITH_GROUPS:
	 sprintf (Query,"SELECT crs_grp_types.GrpTypCod,crs_grp_types.GrpTypName,"
			"crs_grp_types.Mandatory,crs_grp_types.Multiple,"
			"crs_grp_types.MustBeOpened,"
			"UNIX_TIMESTAMP(crs_grp_types.OpenTime),"
			"COUNT(crs_grp.GrpCod)"
			" FROM crs_grp_types,crs_grp"
			" WHERE crs_grp_types.CrsCod=%ld"
			" AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			" GROUP BY crs_grp_types.GrpTypCod"
			" ORDER BY crs_grp_types.GrpTypName",
		  Gbl.CurrentCrs.Crs.CrsCod);
	 break;
      case Grp_ALL_GROUP_TYPES:
	 sprintf (Query,"(SELECT crs_grp_types.GrpTypCod,crs_grp_types.GrpTypName AS GrpTypName,"
			"crs_grp_types.Mandatory,crs_grp_types.Multiple,"
			"crs_grp_types.MustBeOpened,"
			"UNIX_TIMESTAMP(crs_grp_types.OpenTime),"
			"COUNT(crs_grp.GrpCod)"
			" FROM crs_grp_types,crs_grp"
			" WHERE crs_grp_types.CrsCod=%ld"
			" AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
			" GROUP BY crs_grp_types.GrpTypCod)"
			" UNION "
			"(SELECT GrpTypCod,GrpTypName,"
			"Mandatory,Multiple,"
			"MustBeOpened,"
			"UNIX_TIMESTAMP(OpenTime),"
			"0"
			" FROM crs_grp_types WHERE CrsCod=%ld"
			" AND GrpTypCod NOT IN (SELECT GrpTypCod FROM crs_grp))"
			" ORDER BY GrpTypName",
		  Gbl.CurrentCrs.Crs.CrsCod,
		  Gbl.CurrentCrs.Crs.CrsCod);
	 break;
     }
   Gbl.CurrentCrs.Grps.GrpTypes.Num = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get types of group of a course");

   /***** Get group types *****/
   Gbl.CurrentCrs.Grps.GrpTypes.NumGrpsTotal = 0;

   if (Gbl.CurrentCrs.Grps.GrpTypes.Num)
     {
      /***** Create a list of group types *****/
      if ((Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes = (struct GroupType *) calloc (Gbl.CurrentCrs.Grps.GrpTypes.Num,sizeof (struct GroupType))) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store types of group.");

      /***** Get group types *****/
      for (NumRow = 0;
	   NumRow < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	   NumRow++)
        {
         /* Get next group type */
         row = mysql_fetch_row (mysql_res);

         /* Get group type code (row[0]) */
         if ((Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumRow].GrpTypCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong type of group.");

         /* Get group type name (row[1]) */
         Str_Copy (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumRow].GrpTypName,row[1],
                   Grp_MAX_BYTES_GROUP_TYPE_NAME);

         /* Is it mandatory to register in any groups of this type? (row[2]) */
         Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumRow].MandatoryEnrolment = (row[2][0] == 'Y');

         /* Is it possible to register in multiple groups of this type? (row[3]) */
         Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumRow].MultipleEnrolment = (row[3][0] == 'Y');

         /* Groups of this type must be opened? (row[4]) */
         Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumRow].MustBeOpened = (row[4][0] == 'Y');

         /* Get open time (row[5] holds the open time UTC) */
         Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumRow].OpenTimeUTC = Dat_GetUNIXTimeFromStr (row[5]);
         Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumRow].MustBeOpened &= Grp_CheckIfOpenTimeInTheFuture (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumRow].OpenTimeUTC);

         /* Number of groups of this type (row[6]) */
         if (sscanf (row[6],"%u",&Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumRow].NumGrps) != 1)
            Lay_ShowErrorAndExit ("Wrong number of groups of a type.");

         /* Add number of groups to total number of groups */
         Gbl.CurrentCrs.Grps.GrpTypes.NumGrpsTotal += Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumRow].NumGrps;

	 /* Initialize pointer to the list of groups of this type */
         Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumRow].LstGrps = NULL;

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
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrpTypes;
   unsigned NumGrpTyp;
   long GrpTypCod;

   /***** Find group types to be opened *****/
   sprintf (Query,"SELECT GrpTypCod FROM crs_grp_types"
	          " WHERE CrsCod=%ld AND MustBeOpened='Y'"
	          " AND OpenTime<=NOW()",
	    Gbl.CurrentCrs.Crs.CrsCod);
   NumGrpTypes = (unsigned) DB_QuerySELECT (Query,&mysql_res,
	                                    "can not get the types of group to be opened");
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
         sprintf (Query,"UPDATE crs_grp SET Open='Y'"
                        " WHERE GrpTypCod=%ld AND Open='N'",
	          GrpTypCod);
         DB_QueryUPDATE (Query,"can not open groups");

         /***** To not try to open groups again, set MustBeOpened to false *****/
         sprintf (Query,"UPDATE crs_grp_types SET MustBeOpened='N'"
	                " WHERE GrpTypCod=%ld",
	          GrpTypCod);
         DB_QueryUPDATE (Query,"can not update the opening of a type of group");
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

   /***** First we get the list of group types *****/
   Grp_GetListGrpTypesInThisCrs (WhichGroupTypes);

   /***** Then we get the list of groups for each group type *****/
   for (NumGrpTyp = 0;
	NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	NumGrpTyp++)
     {
      GrpTyp = &Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];
      if (GrpTyp->NumGrps)	 // If there are groups of this type...
        {
         /***** Query database *****/
         if ((NumRows = Grp_GetGrpsOfType (GrpTyp->GrpTypCod,&mysql_res)) > 0) // Groups found...
           {
	    // NumRows should be equal to GrpTyp->NumGrps
            GrpTyp->NumGrps = (unsigned) NumRows;

            /***** Create list with groups of this type *****/
            if ((GrpTyp->LstGrps = (struct Group *) calloc (GrpTyp->NumGrps,sizeof (struct Group))) == NULL)
               Lay_ShowErrorAndExit ("Not enough memory to store groups of a type.");

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
               Str_Copy (Grp->GrpName,row[1],
                         Grp_MAX_BYTES_GROUP_NAME);

               /* Get max number of students of group (row[2]) and number of current students */
               Grp->MaxStudents = Grp_ConvertToNumMaxStdsGrp (row[2]);
               Grp->NumStudents = Grp_CountNumStdsInGrp (Grp->GrpCod);

               /* Get whether group is open ('Y') or closed ('N') (row[3]) */
               Grp->Open = (row[3][0] == 'Y');

               /* Get whether group have file zones ('Y') or not ('N') (row[4]) */
               Grp->FileZones = (row[4][0] == 'Y');
              }
           }
         else	// Error: groups should be found, but really they haven't be found. This never should happen.
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

   if (Gbl.CurrentCrs.Grps.GrpTypes.NestedCalls > 0)
      if (--Gbl.CurrentCrs.Grps.GrpTypes.NestedCalls == 0)
         if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes)
           {
	    /***** Free memory used for each list of groups (one list for each group type) *****/
	    for (NumGrpTyp = 0;
		 NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
		 NumGrpTyp++)
              {
               GrpTyp = &Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];
               if (GrpTyp->LstGrps)
                 {
                  free ((void *) GrpTyp->LstGrps);
		  GrpTyp->LstGrps = NULL;
		  GrpTyp->NumGrps = 0;
                 }
              }

	    /***** Free memory used by the list of group types *****/
            free ((void *) Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes);
            Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes = NULL;
            Gbl.CurrentCrs.Grps.GrpTypes.Num = 0;
           }
  }

/*****************************************************************************/
/*********** Query the number of groups that hay in this course **************/
/*****************************************************************************/

unsigned Grp_CountNumGrpsInCurrentCrs (void)
  {
   char Query[512];

   /***** Get number of group in current course from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM crs_grp_types,crs_grp"
                  " WHERE crs_grp_types.CrsCod=%ld"
                  " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod",
            Gbl.CurrentCrs.Crs.CrsCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of groups in this course");
  }

/*****************************************************************************/
/****************** Count number of groups in a group type *******************/
/*****************************************************************************/

static unsigned Grp_CountNumGrpsInThisCrsOfType (long GrpTypCod)
  {
   char Query[128];

   /***** Get number of groups of a type from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM crs_grp WHERE GrpTypCod=%ld",
            GrpTypCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of groups of a type");
  }

/*****************************************************************************/
/***************** Get current groups of a type in this course ***************/
/*****************************************************************************/

unsigned long Grp_GetGrpsOfType (long GrpTypCod,MYSQL_RES **mysql_res)
  {
   char Query[512];

   /***** Get groups of a type from database *****/
   sprintf (Query,"SELECT GrpCod,GrpName,MaxStudents,Open,FileZones"
                  " FROM crs_grp"
                  " WHERE GrpTypCod=%ld"
                  " ORDER BY GrpName",
            GrpTypCod);
   return DB_QuerySELECT (Query,mysql_res,"can not get groups of a type");
  }

/*****************************************************************************/
/******************* Get data of a group type from its code ******************/
/*****************************************************************************/
// GrpTyp->GrpTypCod must have the code of the type of group

static void Grp_GetDataOfGroupTypeByCod (struct GroupType *GrpTyp)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get data of a type of group from database *****/
   sprintf (Query,"SELECT GrpTypName,Mandatory,Multiple,MustBeOpened,UNIX_TIMESTAMP(OpenTime)"
	          " FROM crs_grp_types"
                  " WHERE CrsCod=%ld AND GrpTypCod=%ld",
            Gbl.CurrentCrs.Crs.CrsCod,GrpTyp->GrpTypCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get type of group");

   /***** Count number of rows in result *****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Error when getting type of group.");

   /***** Get some data of group type *****/
   row = mysql_fetch_row (mysql_res);
   Str_Copy (GrpTyp->GrpTypName,row[0],
             Grp_MAX_BYTES_GROUP_TYPE_NAME);
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
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool MultipleEnrolment;

   /***** Get data of a type of group from database *****/
   sprintf (Query,"SELECT Multiple FROM crs_grp_types WHERE GrpTypCod=%ld",
            GrpTypCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get if type of group has multiple enrolment") != 1)
      Lay_ShowErrorAndExit ("Error when getting type of group.");

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
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Reset values *****/
   GrpDat->GrpTypCod = -1L;
   GrpDat->CrsCod    = -1L;
   GrpDat->GrpTypName[0] = '\0';
   GrpDat->GrpName[0]    = '\0';
   GrpDat->MaxStudents = 0;
   GrpDat->Vacant      = 0;
   GrpDat->Open              = false;
   GrpDat->FileZones         = false;
   GrpDat->MultipleEnrolment = false;

   if (GrpDat->GrpCod > 0)
     {
      /***** Get data of a group from database *****/
      sprintf (Query,"SELECT crs_grp_types.GrpTypCod,crs_grp_types.CrsCod,"
		     "crs_grp_types.GrpTypName,crs_grp_types.Multiple,"
		     "crs_grp.GrpName,crs_grp.MaxStudents,"
		     "crs_grp.Open,crs_grp.FileZones"
		     " FROM crs_grp,crs_grp_types"
		     " WHERE crs_grp.GrpCod=%ld"
		     " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod",
	       GrpDat->GrpCod);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get data of a group");

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
	 Str_Copy (GrpDat->GrpTypName,row[2],
	           Grp_MAX_BYTES_GROUP_TYPE_NAME);

	 /* Get whether a student may be in one or multiple groups (row[3]) */
	 GrpDat->MultipleEnrolment = (row[3][0] == 'Y');

	 /* Get the name of the group (row[4]) */
	 Str_Copy (GrpDat->GrpName,row[4],
	           Grp_MAX_BYTES_GROUP_NAME);

	 /* Get maximum number of students (row[5]) */
	 GrpDat->MaxStudents = Grp_ConvertToNumMaxStdsGrp (row[5]);

	 /* Get whether group is open or closed (row[6]) */
	 GrpDat->Open = (row[6][0] == 'Y');

	 /* Get whether group has file zones (row[7]) */
	 GrpDat->FileZones = (row[7][0] == 'Y');
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
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long GrpTypCod;

   /***** Get data of a group from database *****/
   sprintf (Query,"SELECT GrpTypCod FROM crs_grp WHERE GrpCod=%ld",
            GrpCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get the type of a group") != 1)
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
   char Query[128];

   /***** Get if a group exists from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM crs_grp WHERE GrpCod=%ld",GrpCod);
   return (DB_QueryCOUNT (Query,"can not check if a group exists") != 0);
  }

/*****************************************************************************/
/******************* Check if a group belongs to a course ********************/
/*****************************************************************************/

bool Grp_CheckIfGroupBelongsToCourse (long GrpCod,long CrsCod)
  {
   char Query[256];

   /***** Get if a group exists from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM crs_grp,crs_grp_types"
                  " WHERE crs_grp.GrpCod=%ld"
                  " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
                  " AND crs_grp_types.CrsCod=%ld",
            GrpCod,CrsCod);
   return (DB_QueryCOUNT (Query,"can not check if a group belongs to a course") != 0);
  }

/*****************************************************************************/
/******************** Count number of students in a group ********************/
/*****************************************************************************/

unsigned Grp_CountNumStdsInGrp (long GrpCod)
  {
   char Query[512];

   /***** Get number of students in a group from database *****/
   sprintf (Query,"SELECT COUNT(*)"
	          " FROM crs_grp_usr,crs_grp,crs_grp_types,crs_usr"
                  " WHERE crs_grp_usr.GrpCod=%ld"
                  " AND crs_grp_usr.GrpCod=crs_grp.GrpCod"
                  " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
                  " AND crs_grp_types.CrsCod=crs_usr.CrsCod"
                  " AND crs_grp_usr.UsrCod=crs_usr.UsrCod"
                  " AND crs_usr.Role=%u",
            GrpCod,(unsigned) Rol_STUDENT);
   return (unsigned) DB_QueryCOUNT (Query,
	                            "can not get number of students in a group");
  }

/*****************************************************************************/
/** Count # of students of current course not belonging to groups of a type **/
/*****************************************************************************/

static unsigned Grp_CountNumStdsInNoGrpsOfType (long GrpTypCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumStds;

   /***** Get number of students not belonging to groups of a type from database ******/
   sprintf (Query,"SELECT COUNT(UsrCod) FROM crs_usr"
                  " WHERE CrsCod=%ld AND Role=%u"
                  " AND UsrCod NOT IN"
                  " (SELECT DISTINCT crs_grp_usr.UsrCod"
                  " FROM crs_grp,crs_grp_usr"
                  " WHERE crs_grp.GrpTypCod=%ld"
                  " AND crs_grp.GrpCod=crs_grp_usr.GrpCod)",
            Gbl.CurrentCrs.Crs.CrsCod,(unsigned) Rol_STUDENT,GrpTypCod);
   DB_QuerySELECT (Query,&mysql_res,"can not get the number of students not belonging to groups of a type");

   /***** Get the number of students (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumStds) != 1)
      Lay_ShowErrorAndExit ("Error when getting the number of students not belonging to groups of a type.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumStds;
  }

/*****************************************************************************/
/**** Get the first code of group of cierto type al that pert. a student *****/
/*****************************************************************************/
// Return -GrpTypCod if the student does not belongs to any group of type GrpTypCod

static long Grp_GetFirstCodGrpStdBelongsTo (long GrpTypCod,long UsrCod)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   long CodGrpIBelong;

   /***** Get a group which a user belong to from database *****/
   sprintf (Query,"SELECT crs_grp.GrpCod FROM crs_grp,crs_grp_usr"
	          " WHERE crs_grp.GrpTypCod=%ld"
                  " AND crs_grp.GrpCod=crs_grp_usr.GrpCod"
                  " AND crs_grp_usr.UsrCod=%ld",
            GrpTypCod,UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the group which a user belongs to");

   /***** Get the group *****/
   if (NumRows == 0)
      CodGrpIBelong = -GrpTypCod;
   else	// If there are more than a group, only get the first one
     {
      row = mysql_fetch_row (mysql_res);

      /* Get the code of group (row[0]) */
      if ((CodGrpIBelong = Str_ConvertStrCodToLongCod (row[0])) < 0)
         Lay_ShowErrorAndExit ("Wrong code of group.");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return CodGrpIBelong;
  }

/*****************************************************************************/
/********************* Check if a user belongs to a group ********************/
/*****************************************************************************/
// Return true if the user identificado belongs al group with código GrpCod

bool Grp_GetIfIBelongToGrp (long GrpCod)
  {
   char Query[256];

   /***** Get if I belong to a group from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM crs_grp_usr"
                  " WHERE GrpCod=%ld AND UsrCod=%ld",
            GrpCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if you belong to a group") != 0);
  }

/*****************************************************************************/
/**** Get the number of types of group with mandatory enrolment          *****/
/**** that have any group open and with any vacant                       *****/
/**** and I don't belong to any of these groups as student               *****/
/*****************************************************************************/

unsigned Grp_NumGrpTypesMandatIDontBelong (void)
  {
   char Query[4096];
   unsigned NumGrpTypes;

   /***** Get the number of types of groups with mandatory enrolment which I don't belong to, from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT GrpTypCod) FROM"
                  " (SELECT crs_grp_types.GrpTypCod AS GrpTypCod,"
                  "COUNT(*) AS NumStudents,"
                  "crs_grp.MaxStudents as MaxStudents"
                  " FROM crs_grp_types,crs_grp,crs_grp_usr,crs_usr"
                  " WHERE crs_grp_types.CrsCod=%ld"
                  " AND crs_grp_types.Mandatory='Y'"
                  " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
                  " AND crs_grp.Open='Y'"
                  " AND crs_grp_types.CrsCod=crs_usr.CrsCod"
                  " AND crs_grp.GrpCod=crs_grp_usr.GrpCod"
                  " AND crs_grp_usr.UsrCod=crs_usr.UsrCod"
                  " AND crs_usr.Role=%u"
                  " GROUP BY crs_grp.GrpCod"
                  " HAVING NumStudents<MaxStudents) AS grp_types_open_not_full"
                  " WHERE GrpTypCod NOT IN"
                  " (SELECT DISTINCT crs_grp_types.GrpTypCod"
                  " FROM crs_grp_types,crs_grp,crs_grp_usr"
                  " WHERE crs_grp_types.CrsCod=%ld"
                  " AND crs_grp_types.Mandatory='Y'"
                  " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
                  " AND crs_grp.GrpCod=crs_grp_usr.GrpCod"
                  " AND crs_grp_usr.UsrCod=%ld)",
            Gbl.CurrentCrs.Crs.CrsCod,
            (unsigned) Rol_STUDENT,
            Gbl.CurrentCrs.Crs.CrsCod,
            Gbl.Usrs.Me.UsrDat.UsrCod);
   NumGrpTypes = DB_QueryCOUNT (Query,"can not get the number of types of group of mandatory registration to which you don't belong to");

   return NumGrpTypes;
  }

/*****************************************************************************/
/********** Query if any group of a type is open and has vacants *************/
/*****************************************************************************/

static bool Grp_GetIfGrpIsAvailable (long GrpTypCod)
  {
   char Query[2048];
   unsigned NumGrpTypes;

   /***** Get the number of types of group (0 or 1) of a type
          with one or more open groups with vacants, from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT GrpTypCod) FROM"
                  " (SELECT crs_grp_types.GrpTypCod AS GrpTypCod,"
                  "COUNT(*) AS NumStudents,"
                  "crs_grp.MaxStudents as MaxStudents"
                  " FROM crs_grp_types,crs_grp,crs_grp_usr,crs_usr"
                  " WHERE crs_grp_types.GrpTypCod=%ld"
                  " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
                  " AND crs_grp.Open='Y'"
                  " AND crs_grp_types.CrsCod=crs_usr.CrsCod"
                  " AND crs_grp.GrpCod=crs_grp_usr.GrpCod"
                  " AND crs_grp_usr.UsrCod=crs_usr.UsrCod"
                  " AND crs_usr.Role=%u"
                  " GROUP BY crs_grp.GrpCod"
                  " HAVING NumStudents<MaxStudents) AS available_grp_types",
            GrpTypCod,(unsigned) Rol_STUDENT);
   NumGrpTypes = DB_QueryCOUNT (Query,"can not check if a type of group has available groups");

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
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrp;

   /***** Get groups which a user belong to from database *****/
   if (CrsCod < 0)		// Query the groups from all the user's courses
      sprintf (Query,"SELECT GrpCod"
	             " FROM crs_grp_usr"
                     " WHERE UsrCod=%ld",	// Groups will be unordered
               UsrCod);
   else if (GrpTypCod < 0)	// Query the groups of any type in the course
      sprintf (Query,"SELECT crs_grp.GrpCod"
	             " FROM crs_grp_types,crs_grp,crs_grp_usr"
                     " WHERE crs_grp_types.CrsCod=%ld"
                     " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
                     " AND crs_grp.GrpCod=crs_grp_usr.GrpCod"
                     " AND crs_grp_usr.UsrCod=%ld"
                     " ORDER BY crs_grp_types.GrpTypName,crs_grp.GrpName",
               Gbl.CurrentCrs.Crs.CrsCod,UsrCod);
   else				// Query only the groups of specified type in the course
      sprintf (Query,"SELECT crs_grp.GrpCod"
	             " FROM crs_grp_types,crs_grp,crs_grp_usr"
                     " WHERE crs_grp_types.CrsCod=%ld"
                     " AND crs_grp_types.GrpTypCod=%ld"
                     " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
                     " AND crs_grp.GrpCod=crs_grp_usr.GrpCod"
                     " AND crs_grp_usr.UsrCod=%ld"
                     " ORDER BY crs_grp.GrpName",
               Gbl.CurrentCrs.Crs.CrsCod,GrpTypCod,UsrCod);
   LstGrps->NumGrps = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get the groups which a user belongs to");

   /***** Get the groups *****/
   if (LstGrps->NumGrps)
     {
      /***** Create a list of groups the user belongs to *****/
      if ((LstGrps->GrpCods = (long *) calloc (LstGrps->NumGrps,sizeof (long))) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store codes of groups a user belongs to.");
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
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrp;

   /***** Get groups which I belong to from database *****/
   sprintf (Query,"SELECT crs_grp.GrpCod"
	          " FROM crs_grp_types,crs_grp,crs_grp_usr"
                  " WHERE crs_grp_types.CrsCod=%ld"
                  " AND crs_grp_types.GrpTypCod=crs_grp.GrpTypCod"
                  " AND crs_grp.FileZones='Y'"
                  " AND crs_grp.GrpCod=crs_grp_usr.GrpCod"
                  " AND crs_grp_usr.UsrCod=%ld"
                  " ORDER BY crs_grp_types.GrpTypName,crs_grp.GrpName",
            Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   LstGrps->NumGrps = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get the groups which I belong to");

   /***** Get the groups *****/
   if (LstGrps->NumGrps)
     {
      /***** Create a list of groups I belong to *****/
      if ((LstGrps->GrpCods = (long *) calloc (LstGrps->NumGrps,sizeof (long))) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store codes of groups I belongs to.");
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
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;
   size_t MaxLength = (Grp_MAX_BYTES_GROUP_NAME + 2) * Gbl.CurrentCrs.Grps.GrpTypes.NumGrpsTotal;

   /***** Get the names of groups which a user belongs to, from database *****/
   sprintf (Query,"SELECT crs_grp.GrpName FROM crs_grp,crs_grp_usr"
                  " WHERE crs_grp.GrpTypCod=%ld"
                  " AND crs_grp.GrpCod=crs_grp_usr.GrpCod"
                  " AND crs_grp_usr.UsrCod=%ld"
                  " ORDER BY crs_grp.GrpName",
            GrpTypCod,UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the names of groups a user belongs to");

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
         Str_Concat (GroupNames,", ",
                     MaxLength);
      Str_Concat (GroupNames,row[0],
                  MaxLength);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Receive form to create a new group type ******************/
/*****************************************************************************/

void Grp_RecFormNewGrpTyp (void)
  {
   extern const char *Txt_The_type_of_group_X_already_exists;
   extern const char *Txt_Created_new_type_of_group_X;
   extern const char *Txt_You_must_specify_the_name_of_the_new_type_of_group;
   Lay_AlertType_t AlertType;

   /***** Get parameters from form *****/
   /* Get the name of group type */
   Par_GetParToText ("GrpTypName",Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName,
                     Grp_MAX_BYTES_GROUP_TYPE_NAME);

   /* Get whether it is mandatory to regisrer in any group of this type */
   Gbl.CurrentCrs.Grps.GrpTyp.MandatoryEnrolment = Par_GetParToBool ("MandatoryEnrolment");

   /* Get whether it is possible to register in multiple groups of this type */
   Gbl.CurrentCrs.Grps.GrpTyp.MultipleEnrolment = Par_GetParToBool ("MultipleEnrolment");

   /* Get open time */
   Gbl.CurrentCrs.Grps.GrpTyp.OpenTimeUTC = Dat_GetTimeUTCFromForm ("OpenTimeUTC");
   Gbl.CurrentCrs.Grps.GrpTyp.MustBeOpened = Grp_CheckIfOpenTimeInTheFuture (Gbl.CurrentCrs.Grps.GrpTyp.OpenTimeUTC);

   if (Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName[0])	// If there's a group type name
     {
      /***** If name of group type was in database... *****/
      if (Grp_CheckIfGroupTypeNameExists (Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName,-1L))
        {
         AlertType = Lay_WARNING;
         sprintf (Gbl.Message,Txt_The_type_of_group_X_already_exists,
                  Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName);
        }
      else	// Add new group type to database
	{
         Grp_CreateGroupType ();

         AlertType = Lay_SUCCESS;
	 sprintf (Gbl.Message,Txt_Created_new_type_of_group_X,
		  Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName);
	}
     }
   else	// If there is not a group type name
     {
      AlertType = Lay_WARNING;
      sprintf (Gbl.Message,"%s",Txt_You_must_specify_the_name_of_the_new_type_of_group);
     }

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (AlertType,Gbl.Message,
                              Lay_INFO,NULL);
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

void Grp_RecFormNewGrp (void)
  {
   extern const char *Txt_The_group_X_already_exists;
   extern const char *Txt_Created_new_group_X;
   extern const char *Txt_You_must_specify_the_name_of_the_new_group;
   Lay_AlertType_t AlertType;

   /***** Get parameters from form *****/
   if ((Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) > 0) // Group type valid
     {
      /* Get group name */
      Par_GetParToText ("GrpName",Gbl.CurrentCrs.Grps.GrpName,
                        Grp_MAX_BYTES_GROUP_NAME);

      /* Get maximum number of students */
      Gbl.CurrentCrs.Grps.MaxStudents = (unsigned)
	                                Par_GetParToUnsignedLong ("MaxStudents",
                                                                  0,
                                                                  Grp_MAX_STUDENTS_IN_A_GROUP,
                                                                  Grp_NUM_STUDENTS_NOT_LIMITED);

      if (Gbl.CurrentCrs.Grps.GrpName[0])	// If there's a group name
        {
         /***** If name of group was in database... *****/
         if (Grp_CheckIfGroupNameExists (Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod,Gbl.CurrentCrs.Grps.GrpName,-1L))
           {
            AlertType = Lay_WARNING;
            sprintf (Gbl.Message,Txt_The_group_X_already_exists,
                     Gbl.CurrentCrs.Grps.GrpName);
           }
         else	// Add new group to database
           {
            Grp_CreateGroup ();

	    /* Write success message */
            AlertType = Lay_SUCCESS;
	    sprintf (Gbl.Message,Txt_Created_new_group_X,
		     Gbl.CurrentCrs.Grps.GrpName);
           }
        }
      else	// If there is not a group name
        {
         AlertType = Lay_ERROR;
         sprintf (Gbl.Message,"%s",Txt_You_must_specify_the_name_of_the_new_group);
        }
     }
   else	// Invalid group type
     {
      AlertType = Lay_ERROR;
      sprintf (Gbl.Message,"%s","Wrong type of group.");
     }

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (Lay_INFO,NULL,
                              AlertType,Gbl.Message);
  }

/*****************************************************************************/
/******************* Check if name of group type exists **********************/
/*****************************************************************************/

static bool Grp_CheckIfGroupTypeNameExists (const char *GrpTypName,long GrpTypCod)
  {
   char Query[256 + Grp_MAX_BYTES_GROUP_TYPE_NAME];

   /***** Get number of group types with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM crs_grp_types"
                  " WHERE CrsCod=%ld AND GrpTypName='%s'"
                  " AND GrpTypCod<>%ld",
            Gbl.CurrentCrs.Crs.CrsCod,GrpTypName,GrpTypCod);
   return (DB_QueryCOUNT (Query,"can not check if the name of type of group already existed") != 0);
  }

/*****************************************************************************/
/************************ Check if name of group exists **********************/
/*****************************************************************************/

static bool Grp_CheckIfGroupNameExists (long GrpTypCod,const char *GrpName,long GrpCod)
  {
   char Query[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Get number of groups with a type and a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM crs_grp"
                  " WHERE GrpTypCod=%ld AND GrpName='%s' AND GrpCod<>%ld",
            GrpTypCod,GrpName,GrpCod);
   return (DB_QueryCOUNT (Query,"can not check if the name of group already existed") != 0);
  }

/*****************************************************************************/
/************************** Create a new group type **************************/
/*****************************************************************************/

static void Grp_CreateGroupType (void)
  {
   char Query[1024];

   /***** Create a new group type *****/
   sprintf (Query,"INSERT INTO crs_grp_types"
	          " (CrsCod,GrpTypName,Mandatory,Multiple,MustBeOpened,OpenTime)"
                  " VALUES"
                  " (%ld,'%s','%c','%c','%c',FROM_UNIXTIME(%ld))",
            Gbl.CurrentCrs.Crs.CrsCod,Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName,
            Gbl.CurrentCrs.Grps.GrpTyp.MandatoryEnrolment ? 'Y' :
        	                                            'N',
            Gbl.CurrentCrs.Grps.GrpTyp.MultipleEnrolment ? 'Y' :
        	                                           'N',
            Gbl.CurrentCrs.Grps.GrpTyp.MustBeOpened ? 'Y' :
        	                                      'N',
            (long) Gbl.CurrentCrs.Grps.GrpTyp.OpenTimeUTC);
   Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod = DB_QueryINSERTandReturnCode (Query,"can not create type of group");
  }

/*****************************************************************************/
/***************************** Create a new group ****************************/
/*****************************************************************************/

static void Grp_CreateGroup (void)
  {
   char Query[256 + Grp_MAX_BYTES_GROUP_NAME];

   /***** Create a new group *****/
   sprintf (Query,"INSERT INTO crs_grp"
	          " (GrpTypCod,GrpName,MaxStudents,Open,FileZones)"
                  " VALUES"
                  " (%ld,'%s',%u,'N','N')",
            Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod,
            Gbl.CurrentCrs.Grps.GrpName,
            Gbl.CurrentCrs.Grps.MaxStudents);
   DB_QueryINSERT (Query,"can not create group");
  }

/*****************************************************************************/
/********************* Request removing of a group type **********************/
/*****************************************************************************/

void Grp_ReqRemGroupType (void)
  {
   unsigned NumGrps;

   /***** Get the code of the group type *****/
   if ((Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /***** Check if this group type has groups *****/
   if ((NumGrps = Grp_CountNumGrpsInThisCrsOfType (Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod)))	// Group type has groups ==> Ask for confirmation
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
   if ((Gbl.CurrentCrs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1L)
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
   Grp_GetDataOfGroupTypeByCod (&Gbl.CurrentCrs.Grps.GrpTyp);

   /***** Start section to edit group types *****/
   Grp_ReqEditGroupsInternal0 ();

   /***** Show question and button to remove type of group *****/
   /* Start alert */
   if (NumGrps == 1)
      sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_type_of_group_X_1_group_,
               Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName);
   else
      sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_type_of_group_X_Y_groups_,
               Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName,NumGrps);
   Lay_ShowAlertAndButton1 (Lay_QUESTION,Gbl.Message);

   /* End alert */
   Lay_ShowAlertAndButton2 (ActRemGrpTyp,Grp_SECTION_GROUP_TYPES,
                            Grp_PutParamRemGrpTyp,
			    Lay_REMOVE_BUTTON,Txt_Remove_type_of_group);

   /***** Show the form to edit group types and groups again *****/
   Grp_ReqEditGroupsInternal1 (Lay_INFO,NULL);
   Grp_ReqEditGroupsInternal2 (Lay_INFO,NULL);
  }

/*****************************************************************************/
/**************** Put parameter to remove a type of group ********************/
/*****************************************************************************/

static void Grp_PutParamRemGrpTyp (void)
  {
   Grp_PutParamGrpTypCod (Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod);
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
   GrpDat.GrpCod = Gbl.CurrentCrs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Count number of students in group *****/
   NumStds = Grp_CountNumStdsInGrp (Gbl.CurrentCrs.Grps.GrpCod);

   /***** Show the form to edit group types again *****/
   Grp_ReqEditGroupsInternal0 ();
   Grp_ReqEditGroupsInternal1 (Lay_INFO,NULL);

   /***** Show question and button to remove group *****/
   /* Start alert */
   if (NumStds == 0)
      sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_group_X,
               GrpDat.GrpName);
   else if (NumStds == 1)
      sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_group_X_1_student_,
               GrpDat.GrpName);
   else
      sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_group_X_Y_students_,
               GrpDat.GrpName,NumStds);
   Lay_ShowAlertAndButton1 (Lay_QUESTION,Gbl.Message);

   /* End alert */
   Lay_ShowAlertAndButton2 (ActRemGrp,Grp_SECTION_GROUPS,Grp_PutParamRemGrp,
			    Lay_REMOVE_BUTTON,Txt_Remove_group);

   /***** Show the form to edit groups again *****/
   Grp_ReqEditGroupsInternal2 (Lay_INFO,NULL);
  }

/*****************************************************************************/
/*********************** Put parameter to remove a group *********************/
/*****************************************************************************/

static void Grp_PutParamRemGrp (void)
  {
   Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
  }

/*****************************************************************************/
/**************************** Remove a group type ****************************/
/*****************************************************************************/

void Grp_RemoveGroupType (void)
  {
   /***** Get param with code of group type *****/
   if ((Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) < 0)
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
   if ((Gbl.CurrentCrs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1L)
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
   char Query[512];

   /***** Get name and type of the group from database *****/
   Grp_GetDataOfGroupTypeByCod (&Gbl.CurrentCrs.Grps.GrpTyp);

   /***** Remove file zones of all the groups of this type *****/
   Brw_RemoveZonesOfGroupsOfType (Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod);

   /***** Remove the associations of assignments to groups of this type *****/
   Asg_RemoveGroupsOfType (Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod);

   /***** Remove the associations of attendance events to groups of this type *****/
   Att_RemoveGroupsOfType (Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod);

   /***** Remove the associations of surveys to groups of this type *****/
   Svy_RemoveGroupsOfType (Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod);

   /***** Change all groups of this type in course timetable *****/
   sprintf (Query,"UPDATE timetable_crs SET GrpCod=-1"
                  " WHERE GrpCod IN"
                  " (SELECT GrpCod FROM crs_grp WHERE GrpTypCod=%ld)",
            Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod);
   DB_QueryUPDATE (Query,"can not update all groups of a type in course timetable");

   /***** Remove all the students in groups of this type *****/
   sprintf (Query,"DELETE FROM crs_grp_usr WHERE GrpCod IN"
                  " (SELECT GrpCod FROM crs_grp WHERE GrpTypCod=%ld)",
            Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod);
   DB_QueryDELETE (Query,"can not remove users from all groups of a type");

   /***** Remove all the groups of this type *****/
   sprintf (Query,"DELETE FROM crs_grp WHERE GrpTypCod=%ld",
            Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod);
   DB_QueryDELETE (Query,"can not remove groups of a type");

   /***** Remove the group type *****/
   sprintf (Query,"DELETE FROM crs_grp_types WHERE GrpTypCod=%ld",
            Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod);
   DB_QueryDELETE (Query,"can not remove a type of group");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Type_of_group_X_removed,
            Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName);

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (Lay_SUCCESS,Gbl.Message,
                              Lay_INFO,NULL);
  }

/*****************************************************************************/
/******* Remove a group from data base and remove group common zone **********/
/*****************************************************************************/

static void Grp_RemoveGroupCompletely (void)
  {
   extern const char *Txt_Group_X_removed;
   struct GroupData GrpDat;
   char Query[512];

   /***** Get name and type of the group from database *****/
   GrpDat.GrpCod = Gbl.CurrentCrs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Remove file zones of this group *****/
   Brw_RemoveGrpZones (Gbl.CurrentCrs.Crs.CrsCod,GrpDat.GrpCod);

   /***** Remove this group from all the assignments *****/
   Asg_RemoveGroup (GrpDat.GrpCod);

   /***** Remove this group from all the attendance events *****/
   Att_RemoveGroup (GrpDat.GrpCod);

   /***** Remove this group from all the surveys *****/
   Svy_RemoveGroup (GrpDat.GrpCod);

   /***** Change this group in course timetable *****/
   sprintf (Query,"UPDATE timetable_crs SET GrpCod=-1 WHERE GrpCod=%ld",
            Gbl.CurrentCrs.Grps.GrpCod);
   DB_QueryUPDATE (Query,"can not update a group in course timetable");

   /***** Remove all the students in this group *****/
   sprintf (Query,"DELETE FROM crs_grp_usr WHERE GrpCod=%ld",
            Gbl.CurrentCrs.Grps.GrpCod);
   DB_QueryDELETE (Query,"can not remove users from a group");

   /***** Remove the group *****/
   sprintf (Query,"DELETE FROM crs_grp WHERE GrpCod=%ld",
            Gbl.CurrentCrs.Grps.GrpCod);
   DB_QueryDELETE (Query,"can not remove a group");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Group_X_removed,GrpDat.GrpName);

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (Lay_INFO,NULL,
                              Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/******************************* Open a group ********************************/
/*****************************************************************************/

void Grp_OpenGroup (void)
  {
   extern const char *Txt_The_group_X_is_now_open;
   struct GroupData GrpDat;
   char Query[512];

   /***** Get group code *****/
   if ((Gbl.CurrentCrs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /***** Get group data from database *****/
   GrpDat.GrpCod = Gbl.CurrentCrs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Update the table of groups changing open/close status *****/
   sprintf (Query,"UPDATE crs_grp SET Open='Y' WHERE GrpCod=%ld",
            Gbl.CurrentCrs.Grps.GrpCod);
   DB_QueryUPDATE (Query,"can not open a group");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_The_group_X_is_now_open,GrpDat.GrpName);

   /***** Show the form again *****/
   Gbl.CurrentCrs.Grps.Open = true;
   Grp_ReqEditGroupsInternal (Lay_INFO,NULL,
                              Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/******************************* Close a group *******************************/
/*****************************************************************************/

void Grp_CloseGroup (void)
  {
   extern const char *Txt_The_group_X_is_now_closed;
   struct GroupData GrpDat;
   char Query[512];

   /***** Get group code *****/
   if ((Gbl.CurrentCrs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /***** Get group data from database *****/
   GrpDat.GrpCod = Gbl.CurrentCrs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Update the table of groups changing open/close status *****/
   sprintf (Query,"UPDATE crs_grp SET Open='N' WHERE GrpCod=%ld",
            Gbl.CurrentCrs.Grps.GrpCod);
   DB_QueryUPDATE (Query,"can not close a group");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_The_group_X_is_now_closed,GrpDat.GrpName);

   /***** Show the form again *****/
   Gbl.CurrentCrs.Grps.Open = false;
   Grp_ReqEditGroupsInternal (Lay_INFO,NULL,
                              Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/************************ Enable file zones of a group ***********************/
/*****************************************************************************/

void Grp_EnableFileZonesGrp (void)
  {
   extern const char *Txt_File_zones_of_the_group_X_are_now_enabled;
   struct GroupData GrpDat;
   char Query[512];

   /***** Get group code *****/
   if ((Gbl.CurrentCrs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /***** Get group data from database *****/
   GrpDat.GrpCod = Gbl.CurrentCrs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Update the table of groups changing file zones status *****/
   sprintf (Query,"UPDATE crs_grp SET FileZones='Y' WHERE GrpCod=%ld",
            Gbl.CurrentCrs.Grps.GrpCod);
   DB_QueryUPDATE (Query,"can not enable file zones of a group");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_File_zones_of_the_group_X_are_now_enabled,
            GrpDat.GrpName);

   /***** Show the form again *****/
   Gbl.CurrentCrs.Grps.FileZones = true;
   Grp_ReqEditGroupsInternal (Lay_INFO,NULL,
                              Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/*********************** Disable file zones of a group ***********************/
/*****************************************************************************/

void Grp_DisableFileZonesGrp (void)
  {
   extern const char *Txt_File_zones_of_the_group_X_are_now_disabled;
   struct GroupData GrpDat;
   char Query[512];

   /***** Get group code *****/
   if ((Gbl.CurrentCrs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /***** Get group data from database *****/
   GrpDat.GrpCod = Gbl.CurrentCrs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Update the table of groups changing file zones status *****/
   sprintf (Query,"UPDATE crs_grp SET FileZones='N' WHERE GrpCod=%ld",
            Gbl.CurrentCrs.Grps.GrpCod);
   DB_QueryUPDATE (Query,"can not disable file zones of a group");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_File_zones_of_the_group_X_are_now_disabled,
            GrpDat.GrpName);

   /***** Show the form again *****/
   Gbl.CurrentCrs.Grps.FileZones = false;
   Grp_ReqEditGroupsInternal (Lay_INFO,NULL,
                              Lay_SUCCESS,Gbl.Message);
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
   char Query[512];
   Lay_AlertType_t AlertType;

   /***** Get parameters from form *****/
   /* Get group code */
   if ((Gbl.CurrentCrs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /* Get the new group type */
   NewGrpTypCod = Grp_GetParamGrpTypCod ();

   /* Get from the database the type and the name of the group */
   GrpDat.GrpCod = Gbl.CurrentCrs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** If group was in database... *****/
   if (Grp_CheckIfGroupNameExists (NewGrpTypCod,GrpDat.GrpName,-1L))
     {
      AlertType = Lay_WARNING;
      sprintf (Gbl.Message,Txt_The_group_X_already_exists,GrpDat.GrpName);
     }
   else
     {
      /* Update the table of groups changing old type by new type */
      sprintf (Query,"UPDATE crs_grp SET GrpTypCod=%ld WHERE GrpCod=%ld",
               NewGrpTypCod,Gbl.CurrentCrs.Grps.GrpCod);
      DB_QueryUPDATE (Query,"can not update the type of a group");

      /***** Write message to show the change made *****/
      AlertType = Lay_SUCCESS;
      sprintf (Gbl.Message,Txt_The_type_of_group_of_the_group_X_has_changed,
               GrpDat.GrpName);
     }

   /***** Show the form again *****/
   Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod = NewGrpTypCod;
   Grp_ReqEditGroupsInternal (Lay_INFO,NULL,
                              AlertType,Gbl.Message);
  }

/*****************************************************************************/
/************ Change mandatory registration to a group of a type *************/
/*****************************************************************************/

void Grp_ChangeMandatGrpTyp (void)
  {
   extern const char *Txt_The_type_of_enrolment_of_the_type_of_group_X_has_not_changed;
   extern const char *Txt_The_enrolment_of_students_into_groups_of_type_X_is_now_mandatory;
   extern const char *Txt_The_enrolment_of_students_into_groups_of_type_X_is_now_voluntary;
   char Query[1024];
   bool NewMandatoryEnrolment;
   Lay_AlertType_t AlertType;

   /***** Get parameters of the form *****/
   /* Get the código of type of group */
   if ((Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of type of group is missing.");

   /* Get the new type of enrolment (mandatory or voluntaria) of this type of group */
   NewMandatoryEnrolment = Par_GetParToBool ("MandatoryEnrolment");

   /* Get from the database the name of the type and the old type of enrolment */
   Grp_GetDataOfGroupTypeByCod (&Gbl.CurrentCrs.Grps.GrpTyp);

   /***** Check if the old type of enrolment match the new
          (this happens when return is pressed without changes in the form) *****/
   if (Gbl.CurrentCrs.Grps.GrpTyp.MandatoryEnrolment == NewMandatoryEnrolment)
     {
      AlertType = Lay_INFO;
      sprintf (Gbl.Message,Txt_The_type_of_enrolment_of_the_type_of_group_X_has_not_changed,
               Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName);
     }
   else
     {
      /***** Update of the table of types of group changing the old type of enrolment by the new *****/
      sprintf (Query,"UPDATE crs_grp_types SET Mandatory='%c' WHERE GrpTypCod=%ld",
               NewMandatoryEnrolment ? 'Y' :
        	                        'N',
               Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod);
      DB_QueryUPDATE (Query,"can not update enrolment type of a type of group");

      /***** Write message to show the change made *****/
      AlertType = Lay_SUCCESS;
      sprintf (Gbl.Message,
               NewMandatoryEnrolment ? Txt_The_enrolment_of_students_into_groups_of_type_X_is_now_mandatory :
                                       Txt_The_enrolment_of_students_into_groups_of_type_X_is_now_voluntary,
               Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName);
     }

   /***** Show the form again *****/
   Gbl.CurrentCrs.Grps.GrpTyp.MandatoryEnrolment = NewMandatoryEnrolment;
   Grp_ReqEditGroupsInternal (AlertType,Gbl.Message,
                              Lay_INFO,NULL);
  }

/*****************************************************************************/
/******** Change multiple enrolment to one or more groups of a type *********/
/*****************************************************************************/

void Grp_ChangeMultiGrpTyp (void)
  {
   extern const char *Txt_The_type_of_enrolment_of_the_type_of_group_X_has_not_changed;
   extern const char *Txt_Now_each_student_can_belong_to_multiple_groups_of_type_X;
   extern const char *Txt_Now_each_student_can_only_belong_to_a_group_of_type_X;
   char Query[1024];
   bool NewMultipleEnrolment;
   Lay_AlertType_t AlertType;

   /***** Get parameters from the form *****/
   /* Get the code of type of group */
   if ((Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of type of group is missing.");

   /* Get the new type of enrolment (single or multiple) of this type of group */
   NewMultipleEnrolment = Par_GetParToBool ("MultipleEnrolment");

   /* Get from the database the name of the type and the old type of enrolment */
   Grp_GetDataOfGroupTypeByCod (&Gbl.CurrentCrs.Grps.GrpTyp);

   /***** Check if the old type of enrolment match the new one
   	  (this happends when return is pressed without changes) *****/
   if (Gbl.CurrentCrs.Grps.GrpTyp.MultipleEnrolment == NewMultipleEnrolment)
     {
      AlertType = Lay_INFO;
      sprintf (Gbl.Message,Txt_The_type_of_enrolment_of_the_type_of_group_X_has_not_changed,
               Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName);
     }
   else
     {
      /***** Update of the table of types of group changing the old type of enrolment by the new *****/
      sprintf (Query,"UPDATE crs_grp_types SET Multiple='%c'"
	             " WHERE GrpTypCod=%ld",
               NewMultipleEnrolment ? 'Y' :
        	                      'N',
               Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod);
      DB_QueryUPDATE (Query,"can not update enrolment type of a type of group");

      /***** Write message to show the change made *****/
      AlertType = Lay_SUCCESS;
      sprintf (Gbl.Message,
               NewMultipleEnrolment ? Txt_Now_each_student_can_belong_to_multiple_groups_of_type_X :
                                      Txt_Now_each_student_can_only_belong_to_a_group_of_type_X,
               Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName);
     }

   /***** Show the form again *****/
   Gbl.CurrentCrs.Grps.GrpTyp.MultipleEnrolment = NewMultipleEnrolment;
   Grp_ReqEditGroupsInternal (AlertType,Gbl.Message,
                              Lay_INFO,NULL);
  }

/*****************************************************************************/
/****************** Change open time for a type of group *********************/
/*****************************************************************************/

void Grp_ChangeOpenTimeGrpTyp (void)
  {
   extern const char *Txt_The_date_time_of_opening_of_groups_has_changed;
   char Query[512];

   /***** Get the code of type of group *****/
   if ((Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of type of group is missing.");

   /***** Get from the database the data of this type of group *****/
   Grp_GetDataOfGroupTypeByCod (&Gbl.CurrentCrs.Grps.GrpTyp);

   /***** Get open time *****/
   Gbl.CurrentCrs.Grps.GrpTyp.OpenTimeUTC = Dat_GetTimeUTCFromForm ("OpenTimeUTC");
   Gbl.CurrentCrs.Grps.GrpTyp.MustBeOpened = Grp_CheckIfOpenTimeInTheFuture (Gbl.CurrentCrs.Grps.GrpTyp.OpenTimeUTC);

   /***** Update the table of types of group
          changing the old open time of enrolment by the new *****/
   sprintf (Query,"UPDATE crs_grp_types"
	          " SET MustBeOpened='%c',OpenTime=FROM_UNIXTIME(%ld)"
                  " WHERE GrpTypCod=%ld",
            Gbl.CurrentCrs.Grps.GrpTyp.MustBeOpened ? 'Y' :
        	                                      'N',
            (long) Gbl.CurrentCrs.Grps.GrpTyp.OpenTimeUTC,
            Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod);
   DB_QueryUPDATE (Query,"can not update enrolment type of a type of group");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,"%s",Txt_The_date_time_of_opening_of_groups_has_changed);

   /***** Show the form again *****/
   Grp_ReqEditGroupsInternal (Lay_SUCCESS,Gbl.Message,
                              Lay_INFO,NULL);
  }

/*****************************************************************************/
/***************** Change maximum of students in a group *********************/
/*****************************************************************************/

void Grp_ChangeMaxStdsGrp (void)
  {
   extern const char *Txt_The_maximum_number_of_students_in_the_group_X_has_not_changed;
   extern const char *Txt_The_group_X_now_has_no_limit_of_students;
   extern const char *Txt_The_maximum_number_of_students_in_the_group_X_is_now_Y;
   struct GroupData GrpDat;
   char Query[1024];
   unsigned NewMaxStds;
   Lay_AlertType_t AlertType;

   /***** Get parameters of the form *****/
   /* Get group code */
   if ((Gbl.CurrentCrs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /* Get the new maximum number of students of the group */
   NewMaxStds = (unsigned)
	        Par_GetParToUnsignedLong ("MaxStudents",
                                          0,
                                          Grp_MAX_STUDENTS_IN_A_GROUP,
                                          Grp_NUM_STUDENTS_NOT_LIMITED);

   /* Get from the database the type, name, and antiguo maximum of students of the group */
   GrpDat.GrpCod = Gbl.CurrentCrs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Check if the old maximum of students equals the new one (this happens when user press return without change the form) *****/
   if (GrpDat.MaxStudents == NewMaxStds)
     {
      AlertType = Lay_INFO;
      sprintf (Gbl.Message,Txt_The_maximum_number_of_students_in_the_group_X_has_not_changed,
               GrpDat.GrpName);
     }
   else
     {
      /***** Update the table of groups changing the old maximum of students to the new *****/
      sprintf (Query,"UPDATE crs_grp SET MaxStudents=%u WHERE GrpCod=%ld",
               NewMaxStds,Gbl.CurrentCrs.Grps.GrpCod);
      DB_QueryUPDATE (Query,"can not update the maximum number of students in a group");

      /***** Write message to show the change made *****/
      AlertType = Lay_SUCCESS;
      if (NewMaxStds > Grp_MAX_STUDENTS_IN_A_GROUP)
         sprintf (Gbl.Message,Txt_The_group_X_now_has_no_limit_of_students,
                  GrpDat.GrpName);
      else
         sprintf (Gbl.Message,Txt_The_maximum_number_of_students_in_the_group_X_is_now_Y,
                  GrpDat.GrpName,NewMaxStds);
     }

   /***** Show the form again *****/
   Gbl.CurrentCrs.Grps.MaxStudents = NewMaxStds;
   Grp_ReqEditGroupsInternal (Lay_INFO,NULL,
                              AlertType,Gbl.Message);
  }

/*****************************************************************************/
/************ Write the number maximum of students in a group ***************/
/*****************************************************************************/

static void Grp_WriteMaxStdsGrp (unsigned MaxStudents)
  {
   if (MaxStudents > Grp_MAX_STUDENTS_IN_A_GROUP)
      fprintf (Gbl.F.Out,"-");
   else
      fprintf (Gbl.F.Out,"%u",MaxStudents);
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
   extern const char *Txt_You_can_not_leave_the_name_of_the_type_of_group_X_empty;
   extern const char *Txt_The_type_of_group_X_already_exists;
   extern const char *Txt_The_type_of_group_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_type_of_group_X_has_not_changed;
   char Query[128 + Grp_MAX_BYTES_GROUP_TYPE_NAME];
   char NewNameGrpTyp[Grp_MAX_BYTES_GROUP_TYPE_NAME + 1];
   Lay_AlertType_t AlertType;

   /***** Get parameters from form *****/
   /* Get the code of the group type */
   if ((Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod = Grp_GetParamGrpTypCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of type of group is missing.");

   /* Get the new name for the group type */
   Par_GetParToText ("GrpTypName",NewNameGrpTyp,Grp_MAX_BYTES_GROUP_TYPE_NAME);

   /***** Get from the database the old name of the group type *****/
   Grp_GetDataOfGroupTypeByCod (&Gbl.CurrentCrs.Grps.GrpTyp);

   /***** Check if new name is empty *****/
   if (!NewNameGrpTyp[0])
     {
      AlertType = Lay_WARNING;
      sprintf (Gbl.Message,Txt_You_can_not_leave_the_name_of_the_type_of_group_X_empty,
               Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName);
     }
   else
     {
      /***** Check if old and new names are the same (this happens when user press enter with no changes in the form) *****/
      if (strcmp (Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName,NewNameGrpTyp))	// Different names
        {
         /***** If group type was in database... *****/
         if (Grp_CheckIfGroupTypeNameExists (NewNameGrpTyp,Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod))
           {
	    AlertType = Lay_WARNING;
            sprintf (Gbl.Message,Txt_The_type_of_group_X_already_exists,
                     NewNameGrpTyp);
           }
         else
           {
            /* Update the table changing old name by new name */
            sprintf (Query,"UPDATE crs_grp_types SET GrpTypName='%s'"
        	           " WHERE GrpTypCod=%ld",
                     NewNameGrpTyp,
                     Gbl.CurrentCrs.Grps.GrpTyp.GrpTypCod);
            DB_QueryUPDATE (Query,"can not update the type of a group");

            /***** Write message to show the change made *****/
	    AlertType = Lay_SUCCESS;
            sprintf (Gbl.Message,Txt_The_type_of_group_X_has_been_renamed_as_Y,
                     Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName,NewNameGrpTyp);
           }
        }
      else	// The same name
        {
	 AlertType = Lay_INFO;
         sprintf (Gbl.Message,Txt_The_name_of_the_type_of_group_X_has_not_changed,
                  NewNameGrpTyp);
        }
     }

   /***** Show the form again *****/
   Str_Copy (Gbl.CurrentCrs.Grps.GrpTyp.GrpTypName,NewNameGrpTyp,
             Grp_MAX_BYTES_GROUP_TYPE_NAME);
   Grp_ReqEditGroupsInternal (AlertType,Gbl.Message,
                              Lay_INFO,NULL);
  }

/*****************************************************************************/
/******************************* Rename a group ******************************/
/*****************************************************************************/

void Grp_RenameGroup (void)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_group_X_empty;
   extern const char *Txt_The_group_X_already_exists;
   extern const char *Txt_The_group_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_group_X_has_not_changed;
   struct GroupData GrpDat;
   char Query[128 + Grp_MAX_BYTES_GROUP_NAME];
   char NewNameGrp[Grp_MAX_BYTES_GROUP_NAME + 1];
   Lay_AlertType_t AlertType;

   /***** Get parameters from form *****/
   /* Get the code of the group */
   if ((Gbl.CurrentCrs.Grps.GrpCod = Grp_GetParamGrpCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of group is missing.");

   /* Get the new name for the group */
   Par_GetParToText ("GrpName",NewNameGrp,Grp_MAX_BYTES_GROUP_NAME);

   /***** Get from the database the type and the old name of the group *****/
   GrpDat.GrpCod = Gbl.CurrentCrs.Grps.GrpCod;
   Grp_GetDataOfGroupByCod (&GrpDat);

   /***** Check if new name is empty *****/
   if (!NewNameGrp[0])
     {
      AlertType = Lay_WARNING;
      sprintf (Gbl.Message,Txt_You_can_not_leave_the_name_of_the_group_X_empty,
               GrpDat.GrpName);
     }
   else
     {
      /***** Check if old and new names are the same (this happens when user press enter with no changes in the form) *****/
      if (strcmp (GrpDat.GrpName,NewNameGrp))	// Different names
        {
         /***** If group was in database... *****/
         if (Grp_CheckIfGroupNameExists (GrpDat.GrpTypCod,NewNameGrp,Gbl.CurrentCrs.Grps.GrpCod))
           {
	    AlertType = Lay_WARNING;
            sprintf (Gbl.Message,Txt_The_group_X_already_exists,NewNameGrp);
           }
         else
           {
            /* Update the table changing old name by new name */
            sprintf (Query,"UPDATE crs_grp SET GrpName='%s' WHERE GrpCod=%ld",
                     NewNameGrp,Gbl.CurrentCrs.Grps.GrpCod);
            DB_QueryUPDATE (Query,"can not update the name of a group");

            /***** Write message to show the change made *****/
	    AlertType = Lay_SUCCESS;
            sprintf (Gbl.Message,Txt_The_group_X_has_been_renamed_as_Y,
                     GrpDat.GrpName,NewNameGrp);
           }
        }
      else	// The same name
        {
	 AlertType = Lay_INFO;
         sprintf (Gbl.Message,Txt_The_name_of_the_group_X_has_not_changed,
                  NewNameGrp);
        }
     }

   /***** Show the form again *****/
   Str_Copy (Gbl.CurrentCrs.Grps.GrpName,NewNameGrp,
             Grp_MAX_BYTES_GROUP_NAME);
   Grp_ReqEditGroupsInternal (Lay_INFO,NULL,
                              AlertType,Gbl.Message);
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

static void Grp_PutParamGrpTypCod (long GrpTypCod)
  {
   Par_PutHiddenParamLong ("GrpTypCod",GrpTypCod);
  }

/*****************************************************************************/
/********************* Write parameter with code of group ********************/
/*****************************************************************************/

void Grp_PutParamGrpCod (long GrpCod)
  {
   Par_PutHiddenParamLong ("GrpCod",GrpCod);
  }

/*****************************************************************************/
/************************ Get list of group codes selected *******************/
/*****************************************************************************/

void Grp_GetLstCodsGrpWanted (struct ListCodGrps *LstGrpsWanted)
  {
   unsigned NumGrpTyp;
   char Param[8 + 10 + 1];
   char LongStr[1 + 10 + 1];
   char **LstStrCodGrps;
   const char *Ptr;
   unsigned NumGrpWanted;

   /***** Allocate memory for the strings with group codes in each type *****/
   if ((LstStrCodGrps = (char **) calloc (Gbl.CurrentCrs.Grps.GrpTypes.Num,sizeof (char *))) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store codes of groups in which a user wants to be enroled.");

   /***** Get lists with the groups that I want in each type
          in order to count the total number of groups selected *****/
   for (NumGrpTyp = 0, LstGrpsWanted->NumGrps = 0;
	NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	NumGrpTyp++)
     {
      /***** Allocate memory for the list of group codes of this type *****/
      if ((LstStrCodGrps[NumGrpTyp] = (char *) malloc ((size_t) ((1 + 10 + 1) *
                                                                 Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps))) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store codes of groups in which a user wants to be enroled.");

      /***** Get the multiple parameter code of group of this type *****/
      sprintf (Param,"GrpCod%ld",Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
      Par_GetParMultiToText (Param,LstStrCodGrps[NumGrpTyp],
                             ((1 + 10 + 1) * Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps) - 1);
      if (LstStrCodGrps[NumGrpTyp][0])
        {
         /***** Count the number of groups selected of this type of LstCodGrps[NumGrpTyp] *****/
         for (Ptr = LstStrCodGrps[NumGrpTyp], NumGrpWanted = 0;
              *Ptr;
              NumGrpWanted++)
            Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,1 + 10);

         /***** Add the number of groups selected of this type to the number of groups selected total *****/
         LstGrpsWanted->NumGrps += NumGrpWanted;
        }
      }

   /***** Create the list (argument passed to this function)
          with all the groups selected (of all the types) *****/
   if (LstGrpsWanted->NumGrps)
     {
      if ((LstGrpsWanted->GrpCods = (long *) calloc (LstGrpsWanted->NumGrps,sizeof (long))) == NULL)
         Lay_ShowErrorAndExit ("Not enoguh memory to store codes of groups in which a user wants to be enroled.");

      /***** Get the groups *****/
      for (NumGrpTyp = 0, NumGrpWanted = 0;
	   NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
        {
         /* Add the groups selected of this type to the complete list of groups selected */
         for (Ptr = LstStrCodGrps[NumGrpTyp];
              *Ptr;
              NumGrpWanted++)
           {
            Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,1 + 10);
            LstGrpsWanted->GrpCods[NumGrpWanted] = Str_ConvertStrCodToLongCod (LongStr);
           }
         /* Free memory used by the list of group codes of this type */
         free ((void *) LstStrCodGrps[NumGrpTyp]);
        }
     }

   /***** Free memory used by the lists of group codes of each type *****/
   free ((void *) LstStrCodGrps);
  }

/*****************************************************************************/
/************************** Free list of group codes *************************/
/*****************************************************************************/

void Grp_FreeListCodGrp (struct ListCodGrps *LstGrps)
  {
   if (LstGrps->NumGrps && LstGrps->GrpCods)
      free ((void *) LstGrps->GrpCods);
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
/****** Parameter to show only my groups or all groups or in timetable *******/
/*****************************************************************************/

void Grp_PutParamWhichGrps (void)
  {
   Grp_GetParamWhichGrps ();

   Par_PutHiddenParamUnsigned ("WhichGrps",(unsigned) Gbl.CurrentCrs.Grps.WhichGrps);
  }

void Grp_PutParamWhichGrpsOnlyMyGrps (void)
  {
   Par_PutHiddenParamUnsigned ("WhichGrps",(unsigned) Grp_ONLY_MY_GROUPS);
  }

void Grp_PutParamWhichGrpsAllGrps (void)
  {
   Par_PutHiddenParamUnsigned ("WhichGrps",(unsigned) Grp_ALL_GROUPS);
  }

/*****************************************************************************/
/***** Show form to choice whether to show only my groups or all groups ******/
/*****************************************************************************/

void Grp_ShowFormToSelWhichGrps (Act_Action_t Action,void (*FuncParams) ())
  {
   extern const char *Txt_GROUP_WHICH_GROUPS[2];
   Grp_WhichGroups_t WhichGrps;

   fprintf (Gbl.F.Out,"<div class=\"PREF_CONTAINER\">");
   for (WhichGrps = Grp_ONLY_MY_GROUPS;
	WhichGrps <= Grp_ALL_GROUPS;
	WhichGrps++)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\">",
	       WhichGrps == Gbl.CurrentCrs.Grps.WhichGrps ? "PREF_ON" :
							    "PREF_OFF");
      Act_FormStart (Action);
      Par_PutHiddenParamUnsigned ("WhichGrps",(unsigned) WhichGrps);
      if (FuncParams)	// Extra parameters depending on the action
	 FuncParams ();
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s\""
			 " alt=\"%s\" title=\"%s\" class=\"ICO25x25\""
			 " style=\"margin:0 auto;\" />",
	       Gbl.Prefs.IconsURL,
	       WhichGrps == Grp_ONLY_MY_GROUPS ? "myhierarchy64x64.png" :
		                                 "hierarchy64x64.png",
	       Txt_GROUP_WHICH_GROUPS[WhichGrps],
	       Txt_GROUP_WHICH_GROUPS[WhichGrps]);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");
     }
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************* Get whether to show only my groups or all groups **************/
/*****************************************************************************/

void Grp_GetParamWhichGrps (void)
  {
   static bool AlreadyGot = false;
   Grp_WhichGroups_t WhichGroupsDefault;

   if (!AlreadyGot)
     {
      /***** Get which grous (my groups or all groups) *****/
      /* Set default */
      switch (Gbl.Action.Act)
	{
	 case ActSeeCrsTT:
	 case ActPrnCrsTT:
	 case ActChgCrsTT1stDay:
	 case ActSeeAsg:
	 case ActSeeAllSvy:
	 case ActSeeAtt:
	    WhichGroupsDefault = Gbl.Usrs.Me.IBelongToCurrentCrs ? Grp_ONLY_MY_GROUPS :	// If I belong to this course ==> see only my groups
							           Grp_ALL_GROUPS;	// If I don't belong to this course ==> see all groups
	    break;
	 case ActSeeMyTT:
	 case ActPrnMyTT:
	 case ActChgMyTT1stDay:
	    WhichGroupsDefault = Grp_ONLY_MY_GROUPS;	// By default, see only my groups
	    break;
	 default:	// Control never should enter here
	    WhichGroupsDefault = Grp_WHICH_GROUPS_DEFAULT;
	    break;
	}

      /* Get parameter */
      Gbl.CurrentCrs.Grps.WhichGrps = (Grp_WhichGroups_t)
	                              Par_GetParToUnsignedLong ("WhichGrps",
	                                                        0,
	                                                        Grp_NUM_WHICH_GROUPS - 1,
	                                                        (unsigned long) WhichGroupsDefault);

      AlreadyGot = true;
     }
  }
