// swad_assignment.c: assignments

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_assignment.h"
#include "swad_assignment_database.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_group_database.h"
#include "swad_hidden_visible.h"
#include "swad_HTML.h"
#include "swad_notification.h"
#include "swad_notification_database.h"
#include "swad_pagination.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_resource.h"
#include "swad_role.h"
#include "swad_setting.h"
#include "swad_string.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Asg_PutHead (struct Asg_Assignments *Assignments,
                         bool OnlyOneAssignment,
                         bool PrintView);
static bool Asg_CheckIfICanCreateAssignments (void);
static void Asg_PutIconsListAssignments (void *Assignments);
static void Asg_PutIconToCreateNewAsg (void *Assignments);
static void Asg_ParsWhichGroupsToShow (void *Assignments);
static void Asg_PutIconsOneAsg (void *Assignments);
static void Asg_ShowAssignmentRow (struct Asg_Assignments *Assignments,
                                   bool OnlyOneAssignment,
                                   bool PrintView);
static void Asg_WriteAsgAuthor (struct Asg_Assignment *Asg);
static void Asg_WriteAssignmentFolder (struct Asg_Assignment *Asg,bool PrintView);

static void Asg_PutIconsToRemEditOneAsg (struct Asg_Assignments *Assignments,
                                         const char *Anchor);
static void Asg_PutPars (void *Assignments);
static void Asg_GetListAssignments (struct Asg_Assignments *Assignments);
static void Asg_GetAssignmentDataFromRow (MYSQL_RES **mysql_res,
                                          struct Asg_Assignment *Asg,
                                          unsigned NumAsgs);
static void Asg_ResetAssignment (struct Asg_Assignment *Asg);
static void Asg_FreeListAssignments (struct Asg_Assignments *Assignments);
static void Asg_HideUnhideAssignment (HidVis_HiddenOrVisible_t HiddenOrVisible);
static void Asg_ShowLstGrpsToEditAssignment (long AsgCod);
static void Asg_CreateAssignment (struct Asg_Assignment *Asg,const char *Txt);
static void Asg_UpdateAssignment (struct Asg_Assignment *Asg,const char *Txt);
static void Asg_CreateGroups (long AsgCod);
static void Asg_GetAndWriteNamesOfGrpsAssociatedToAsg (struct Asg_Assignment *Asg);
static bool Asg_CheckIfIBelongToCrsOrGrpsThisAssignment (long AsgCod);

/*****************************************************************************/
/*************************** Reset assignments *******************************/
/*****************************************************************************/

void Asg_ResetAssignments (struct Asg_Assignments *Assignments)
  {
   Assignments->LstIsRead     = false;	// List is not read
   Assignments->Num           = 0;
   Assignments->LstAsgCods    = NULL;
   Assignments->SelectedOrder = Asg_ORDER_DEFAULT;
   // Assignments->AsgCod  = -1L;	// Used as parameter in contextual links
   Assignments->CurrentPage   = 0;
   Asg_ResetAssignment (&Assignments->Asg);
  }

/*****************************************************************************/
/**************************** List all assignments ***************************/
/*****************************************************************************/

void Asg_SeeAssignments (void)
  {
   struct Asg_Assignments Assignments;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParAsgOrder ();
   Gbl.Crs.Grps.WhichGrps = Grp_GetParWhichGroups ();
   Assignments.CurrentPage = Pag_GetParPagNum (Pag_ASSIGNMENTS);

   /***** Show all assignments *****/
   Asg_ShowAllAssignments (&Assignments);
  }

/*****************************************************************************/
/**************************** Show all assignments ***************************/
/*****************************************************************************/

void Asg_ShowAllAssignments (struct Asg_Assignments *Assignments)
  {
   extern const char *Hlp_ASSESSMENT_Assignments;
   extern const char *Txt_Assignments;
   extern const char *Txt_No_assignments;
   struct Pag_Pagination Pagination;
   unsigned NumAsg;

   /***** Get list of assignments *****/
   Asg_GetListAssignments (Assignments);

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Assignments->Num;
   Pagination.CurrentPage = (int) Assignments->CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Assignments->CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Begin box *****/
   Box_BoxBegin ("100%",Txt_Assignments,
                 Asg_PutIconsListAssignments,Assignments,
                 Hlp_ASSESSMENT_Assignments,Box_NOT_CLOSABLE);

      /***** Select whether show only my groups or all groups *****/
      if (Gbl.Crs.Grps.NumGrps)
	{
	 Set_BeginSettingsHead ();
	 Grp_ShowFormToSelWhichGrps (ActSeeAllAsg,
				     Asg_ParsWhichGroupsToShow,Assignments);
	 Set_EndSettingsHead ();
	}

      /***** Write links to pages *****/
      Pag_WriteLinksToPagesCentered (Pag_ASSIGNMENTS,&Pagination,
				     Assignments,-1L);

      if (Assignments->Num)
	{
	 /***** Begin table *****/
	 HTM_TABLE_BeginWideMarginPadding (2);

	    /***** Table head *****/
	    Asg_PutHead (Assignments,
	                 false,		// Not only this assignment in table
	                 false);	// Not print view

	    /***** Write all assignments *****/
	    for (NumAsg  = Pagination.FirstItemVisible, The_ResetRowColor ();
		 NumAsg <= Pagination.LastItemVisible;
		 NumAsg++, The_ChangeRowColor ())
	      {
	       Assignments->Asg.AsgCod = Assignments->LstAsgCods[NumAsg - 1];
	       Asg_GetAssignmentDataByCod (&Assignments->Asg);
	       Asg_ShowAssignmentRow (Assignments,
	                              false,	// Not only this assignment in table
				      false);	// Not print view
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();
	}
      else	// No assignments created
	 Ale_ShowAlert (Ale_INFO,Txt_No_assignments);

      /***** Write again links to pages *****/
      Pag_WriteLinksToPagesCentered (Pag_ASSIGNMENTS,&Pagination,
				     Assignments,-1L);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of assignments *****/
   Asg_FreeListAssignments (Assignments);
  }

/*****************************************************************************/
/***************** Write header with fields of an assignment *****************/
/*****************************************************************************/

static void Asg_PutHead (struct Asg_Assignments *Assignments,
                         bool OnlyOneAssignment,
                         bool PrintView)
  {
   extern const char *Txt_START_END_TIME_HELP[Dat_NUM_START_END_TIME];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Assignment;
   extern const char *Txt_Folder;
   Dat_StartEndTime_t Order;
   Grp_WhichGroups_t WhichGroups;

   HTM_TR_Begin (NULL);

      if (!OnlyOneAssignment)
         HTM_TH_Span (NULL,HTM_HEAD_CENTER,1,1,"CONTEXT_COL");	// Column for contextual icons

      for (Order  = (Dat_StartEndTime_t) 0;
	   Order <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   Order++)
	{
	 /* Begin head cell */
         HTM_TH_Begin (HTM_HEAD_LEFT);

	    if (!PrintView)
	      {
	       /* Begin form */
	       Frm_BeginForm (ActSeeAllAsg);
		  WhichGroups = Grp_GetParWhichGroups ();
		  Grp_PutParWhichGroups (&WhichGroups);
		  Pag_PutParPagNum (Pag_ASSIGNMENTS,Assignments->CurrentPage);
		  Par_PutParOrder ((unsigned) Order);

	          /* Begin link to select order */
		  HTM_BUTTON_Submit_Begin (Txt_START_END_TIME_HELP[Order],
		                           "class=\"BT_LINK\"");
		  if (Order == Assignments->SelectedOrder)
		     HTM_U_Begin ();
	      }

	    /* Start / end text */
	    HTM_Txt (Txt_START_END_TIME[Order]);

	    if (!PrintView)
	      {
	          /* End link to select order */
		  if (Order == Assignments->SelectedOrder)
		     HTM_U_End ();
		  HTM_BUTTON_End ();

	       /* End form */
	       Frm_EndForm ();
	      }

	 /* End head cell */
	 HTM_TH_End ();
	}

      HTM_TH (Txt_Assignment,HTM_HEAD_LEFT);
      HTM_TH (Txt_Folder    ,HTM_HEAD_LEFT);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Check if I can create assignments **********************/
/*****************************************************************************/

static bool Asg_CheckIfICanCreateAssignments (void)
  {
   return Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
          Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM;
  }

/*****************************************************************************/
/*************** Put contextual icons in list of assignments *****************/
/*****************************************************************************/

static void Asg_PutIconsListAssignments (void *Assignments)
  {
   /***** Put icon to create a new assignment *****/
   if (Assignments)
     {
      if (Asg_CheckIfICanCreateAssignments ())
	 Asg_PutIconToCreateNewAsg (Assignments);

      /***** Link to get resource link *****/
      if (Rsc_CheckIfICanGetLink ())
	{
         ((struct Asg_Assignments *) Assignments)->Asg.AsgCod = -1L;
	 Ico_PutContextualIconToGetLink (ActReqLnkAsg,NULL,
					 Asg_PutPars,Assignments);
	}
     }

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_ASSIGNMENTS);
  }

/*****************************************************************************/
/******************* Put icon to create a new assignment *********************/
/*****************************************************************************/

static void Asg_PutIconToCreateNewAsg (void *Assignments)
  {
   if (Assignments)
     {
      ((struct Asg_Assignments *) Assignments)->Asg.AsgCod = -1L;
      Ico_PutContextualIconToAdd (ActFrmNewAsg,NULL,Asg_PutPars,Assignments);
     }
  }

/*****************************************************************************/
/**************** Put params to select which groups to show ******************/
/*****************************************************************************/

static void Asg_ParsWhichGroupsToShow (void *Assignments)
  {
   if (Assignments)
     {
      Par_PutParOrder ((unsigned) ((struct Asg_Assignments *) Assignments)->SelectedOrder);
      Pag_PutParPagNum (Pag_ASSIGNMENTS,
                                ((struct Asg_Assignments *) Assignments)->CurrentPage);
     }
  }

/*****************************************************************************/
/**************************** Show one assignment ****************************/
/*****************************************************************************/

void Asg_SeeOneAssignment (void)
  {
   extern const char *Hlp_ASSESSMENT_Assignments;
   struct Asg_Assignments Assignments;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParAsgOrder ();
   Grp_GetParWhichGroups ();
   Assignments.CurrentPage = Pag_GetParPagNum (Pag_ASSIGNMENTS);

   /***** Get the code of the assignment *****/
   Assignments.Asg.AsgCod = ParCod_GetAndCheckPar (ParCod_Asg);

   /***** Get data of this assignment *****/
   Asg_GetAssignmentDataByCod (&Assignments.Asg);

   /***** Show selected assignment in a box *****/
   Asg_ShowOneAssignmentInBox (&Assignments);

   /***** Show current assignments, if any *****/
   // TODO: The page should be that corresponding to the selected assignment.
   Asg_ShowAllAssignments (&Assignments);
  }

/*****************************************************************************/
/******************** Show print view of one assignment **********************/
/*****************************************************************************/

void Asg_PrintOneAssignment (void)
  {
   struct Asg_Assignments Assignments;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get the code of the assignment *****/
   Assignments.Asg.AsgCod = ParCod_GetAndCheckPar (ParCod_Asg);

   /***** Get data of this assignment *****/
   Asg_GetAssignmentDataByCod (&Assignments.Asg);

   /***** Write header *****/
   Lay_WriteHeaderClassPhoto (true,false,
			      Gbl.Hierarchy.Ins.InsCod,
			      Gbl.Hierarchy.Deg.DegCod,
			      Gbl.Hierarchy.Crs.CrsCod);

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (2);

      /***** Table head *****/
      Asg_PutHead (&Assignments,
	           true,	// Only this assignment in table
		   true);	// Print view

      /***** Write assignment *****/
      Asg_ShowAssignmentRow (&Assignments,
                             true,	// Only this assignment in table
			     true);	// Print view

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************************* Show an assignment in a box ***********************/
/*****************************************************************************/

void Asg_ShowOneAssignmentInBox (struct Asg_Assignments *Assignments)
  {
   extern const char *Hlp_ASSESSMENT_Assignments;
   extern const char *Txt_Assignment;

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Assignments->Asg.Title[0] ? Assignments->Asg.Title :
						       Txt_Assignment,
                      Asg_PutIconsOneAsg,Assignments,
		      Hlp_ASSESSMENT_Assignments,Box_NOT_CLOSABLE,2);

      /***** Table head *****/
      Asg_PutHead (Assignments,
                   true,	// Only this assignment in table
		   false);	// Not print view

      /***** Write assignment *****/
      Asg_ShowAssignmentRow (Assignments,
                             true,	// Only this assignment in table
			     false);	// Not print view

   /***** End table and end box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/****************** Put contextual icons in an assignment ********************/
/*****************************************************************************/

static void Asg_PutIconsOneAsg (void *Assignments)
  {
   char *Anchor = NULL;

   if (Assignments)
     {
      /***** Set anchor string *****/
      Frm_SetAnchorStr (((struct Asg_Assignments *) Assignments)->Asg.AsgCod,&Anchor);

      /***** Icons to remove/edit this assignment *****/
      Asg_PutIconsToRemEditOneAsg (Assignments,Anchor);

      /***** Free anchor string *****/
      Frm_FreeAnchorStr (&Anchor);
     }
  }

/*****************************************************************************/
/********************* Show assignment row in a table ************************/
/*****************************************************************************/

static void Asg_ShowAssignmentRow (struct Asg_Assignments *Assignments,
                                   bool OnlyOneAssignment,
                                   bool PrintView)
  {
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];
   extern const char *HidVis_DateGreenClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_DateRedClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_TitleClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   char *Id;
   Dat_StartEndTime_t StartEndTime;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Assignments->Asg.AsgCod,&Anchor);

   /***** Write first row of data of this assignment *****/
   HTM_TR_Begin (NULL);

      /* Forms to remove/edit this assignment */
      if (!OnlyOneAssignment)
	{
	 HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL %s\"",
	               The_GetColorRows ());
	    Asg_PutIconsToRemEditOneAsg (Assignments,Anchor);
         HTM_TD_End ();
	}

      /* Start/end date/time */
      UniqueId++;

      for (StartEndTime  = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
	{
	 if (asprintf (&Id,"asg_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	    Err_NotEnoughMemoryExit ();
	 if (PrintView)
	    HTM_TD_Begin ("id=\"%s\" class=\"LB %s_%s\"",
			  Id,
			  Assignments->Asg.Open == HidVis_HIDDEN ? HidVis_DateGreenClass[Assignments->Asg.HiddenOrVisible] :
						                   HidVis_DateRedClass[Assignments->Asg.HiddenOrVisible],
			  The_GetSuffix ());
	 else
	    HTM_TD_Begin ("id=\"%s\" class=\"LB %s_%s %s\"",
			  Id,
			  Assignments->Asg.Open == HidVis_HIDDEN ? HidVis_DateGreenClass[Assignments->Asg.HiddenOrVisible] :
						                   HidVis_DateRedClass[Assignments->Asg.HiddenOrVisible],
			  The_GetSuffix (),
			  The_GetColorRows ());
	 Dat_WriteLocalDateHMSFromUTC (Id,Assignments->Asg.TimeUTC[StartEndTime],
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       true,true,true,0x7);
	 HTM_TD_End ();
	 free (Id);
	}

      /* Assignment title */
      if (PrintView)
	 HTM_TD_Begin ("class=\"LT\"");
      else
	 HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());

      HTM_ARTICLE_Begin (Anchor);
	 Frm_BeginForm (ActSeeOneAsg);
	    Asg_PutPars (Assignments);
	    HTM_BUTTON_Submit_Begin (Txt_Actions[ActSeeOneAsg],
	                             "class=\"LT BT_LINK %s_%s\"",
				     HidVis_TitleClass[Assignments->Asg.HiddenOrVisible],
				     The_GetSuffix ());
	       HTM_Txt (Assignments->Asg.Title);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
      HTM_ARTICLE_End ();

      HTM_TD_End ();

      /* Assignment folder */
      if (PrintView)
	 HTM_TD_Begin ("class=\"LT DAT_%s\"",
	               The_GetSuffix ());
      else
	 HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	               The_GetSuffix (),
	               The_GetColorRows ());
      if (Assignments->Asg.SendWork == Asg_SEND_WORK)
	 Asg_WriteAssignmentFolder (&Assignments->Asg,PrintView);
      HTM_TD_End ();

   HTM_TR_End ();

   /***** Write second row of data of this assignment *****/
   HTM_TR_Begin (NULL);

      /* Author of the assignment */
      if (PrintView)
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
      else
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",
	               The_GetColorRows ());
      Asg_WriteAsgAuthor (&Assignments->Asg);
      HTM_TD_End ();

      /* Text of the assignment */
      Asg_DB_GetAssignmentTxtByCod (Assignments->Asg.AsgCod,Txt);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			Txt,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
      ALn_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
      if (PrintView)
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
      else
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",The_GetColorRows ());
      if (Gbl.Crs.Grps.NumGrps)
	 Asg_GetAndWriteNamesOfGrpsAssociatedToAsg (&Assignments->Asg);

      HTM_DIV_Begin ("class=\"PAR %s_%s\"",
                     HidVis_DataClass[Assignments->Asg.HiddenOrVisible],
		     The_GetSuffix ());
	 HTM_Txt (Txt);
      HTM_DIV_End ();

      HTM_TD_End ();

   HTM_TR_End ();

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (&Anchor);

   /***** Mark possible notification as seen *****/
   Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_ASSIGNMENT,Assignments->Asg.AsgCod);
  }

/*****************************************************************************/
/********************* Write the author of an assignment *********************/
/*****************************************************************************/

static void Asg_WriteAsgAuthor (struct Asg_Assignment *Asg)
  {
   Usr_WriteAuthor1Line (Asg->UsrCod,Asg->HiddenOrVisible);
  }

/*****************************************************************************/
/********************* Write the folder of an assignment *********************/
/*****************************************************************************/

static void Asg_WriteAssignmentFolder (struct Asg_Assignment *Asg,bool PrintView)
  {
   extern const char *Txt_Folder;
   Act_Action_t NextAction;
   bool ICanSendFiles = Asg->HiddenOrVisible == HidVis_VISIBLE &&	// It's visible (not hidden)
                        Asg->Open &&				// It's open (inside dates)
                        Asg->IBelongToCrsOrGrps;		// I belong to course or groups

   /***** Folder icon *****/
   if (!PrintView &&	// Not print view
       ICanSendFiles)	// I can send files to this assignment folder
     {
      /* Form to create a new file or folder */
      Gbl.FileBrowser.FullTree = true;	// By default, show all files
      switch (Gbl.Usrs.Me.Role.Logged)
        {
	 case Rol_STD:
	    Gbl.FileBrowser.Type = Brw_ADMI_ASG_USR;	// User assignments
	    NextAction = ActFrmCreAsgUsr;
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    Gbl.FileBrowser.Type = Brw_ADMI_ASG_CRS;	// Course assignments
	    Str_Copy (Gbl.Usrs.Other.UsrDat.EnUsrCod,Gbl.Usrs.Me.UsrDat.EnUsrCod,
		      sizeof (Gbl.Usrs.Other.UsrDat.EnUsrCod) - 1);
	    Usr_CreateListSelectedUsrsCodsAndFillWithOtherUsr (&Gbl.Usrs.Selected);
	    NextAction = ActFrmCreAsgCrs;
	    break;
	 default:
            Err_WrongRoleExit ();
            NextAction = ActUnk;
	    break;
        }
      Frm_BeginForm (NextAction);

	 Str_Copy (Gbl.FileBrowser.FilFolLnk.Path,Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
		   sizeof (Gbl.FileBrowser.FilFolLnk.Path) - 1);
	 Str_Copy (Gbl.FileBrowser.FilFolLnk.Name,Asg->Folder,
		   sizeof (Gbl.FileBrowser.FilFolLnk.Name) - 1);
	 Gbl.FileBrowser.FilFolLnk.Type = Brw_IS_FOLDER;
	 Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);
	 Ico_PutIconLink ("folder-open-yellow-plus.png",Ico_UNCHANGED,NextAction);

      Frm_EndForm ();

      switch (Gbl.Usrs.Me.Role.Logged)
        {
	 case Rol_STD:
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
	    break;
	 default:
	    break;
        }
     }
   else			// Sending of files disabled
     {
      if (ICanSendFiles)
	 Ico_PutIconOff ("folder-open.svg",Ico_GREEN,Txt_Folder);
      else
	 Ico_PutIconOff ("folder.svg"     ,Ico_RED  ,Txt_Folder);
     }

   /***** Folder name *****/
   HTM_Txt (Asg->Folder);
  }

/*****************************************************************************/
/******* Get parameter with the type or order in list of assignments *********/
/*****************************************************************************/

Dat_StartEndTime_t Asg_GetParAsgOrder (void)
  {
   return (Dat_StartEndTime_t)
   Par_GetParUnsignedLong ("Order",
			   0,
			   Dat_NUM_START_END_TIME - 1,
			   (unsigned long) Asg_ORDER_DEFAULT);
  }

/*****************************************************************************/
/******************** Put icons to edit one assignment ***********************/
/*****************************************************************************/

static void Asg_PutIconsToRemEditOneAsg (struct Asg_Assignments *Assignments,
                                         const char *Anchor)
  {
   static Act_Action_t ActionHideUnhide[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = ActUnhAsg,	// Hidden ==> action to unhide
      [HidVis_VISIBLE] = ActHidAsg,	// Visible ==> action to hide
     };

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Icon to remove assignment *****/
	 Ico_PutContextualIconToRemove (ActReqRemAsg,NULL,
	                                Asg_PutPars,Assignments);

	 /***** Icon to hide/unhide assignment *****/
	 Ico_PutContextualIconToHideUnhide (ActionHideUnhide,Anchor,
					    Asg_PutPars,Assignments,
					    Assignments->Asg.HiddenOrVisible);

	 /***** Icon to edit assignment *****/
	 Ico_PutContextualIconToEdit (ActEdiOneAsg,NULL,
	                              Asg_PutPars,Assignments);

	 /***** Link to get resource link *****/
	 if (Rsc_CheckIfICanGetLink ())
	    Ico_PutContextualIconToGetLink (ActReqLnkAsg,NULL,
					    Asg_PutPars,Assignments);
	 /* falls through */
	 /* no break */
      case Rol_STD:
      case Rol_NET:
	 /***** Icon to print assignment *****/
	 Ico_PutContextualIconToPrint (ActPrnOneAsg,
	                               Asg_PutPars,Assignments);
	 break;
      default:
         break;
     }
  }

/*****************************************************************************/
/******************** Params used to edit an assignment **********************/
/*****************************************************************************/

static void Asg_PutPars (void *Assignments)
  {
   Grp_WhichGroups_t WhichGroups;

   if (Assignments)
     {
      ParCod_PutPar (ParCod_Asg,((struct Asg_Assignments *) Assignments)->Asg.AsgCod);
      Par_PutParOrder ((unsigned) ((struct Asg_Assignments *) Assignments)->SelectedOrder);
      WhichGroups = Grp_GetParWhichGroups ();
      Grp_PutParWhichGroups (&WhichGroups);
      Pag_PutParPagNum (Pag_ASSIGNMENTS,((struct Asg_Assignments *) Assignments)->CurrentPage);
     }
  }

/*****************************************************************************/
/*************************** List all assignments ****************************/
/*****************************************************************************/

static void Asg_GetListAssignments (struct Asg_Assignments *Assignments)
  {
   extern unsigned (*Asg_DB_GetListAssignments[Grp_NUM_WHICH_GROUPS]) (MYSQL_RES **mysql_res,
                                                                       Dat_StartEndTime_t SelectedOrder);
   MYSQL_RES *mysql_res;
   unsigned NumAsg;

   if (Assignments->LstIsRead)
      Asg_FreeListAssignments (Assignments);

   /***** Get list of assignments from database *****/
   Assignments->Num = Asg_DB_GetListAssignments[Gbl.Crs.Grps.WhichGrps] (&mysql_res,Assignments->SelectedOrder);
   if (Assignments->Num) // Assignments found...
     {
      /***** Create list of assignments *****/
      if ((Assignments->LstAsgCods = calloc ((size_t) Assignments->Num,
                                             sizeof (*Assignments->LstAsgCods))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the assignments codes *****/
      for (NumAsg = 0;
	   NumAsg < Assignments->Num;
	   NumAsg++)
         /* Get next assignment code */
         if ((Assignments->LstAsgCods[NumAsg] = DB_GetNextCode (mysql_res)) <= 0)
            Err_WrongAssignmentExit ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Assignments->LstIsRead = true;
  }

/*****************************************************************************/
/******************* Get assignment data using its code **********************/
/*****************************************************************************/

void Asg_GetAssignmentDataByCod (struct Asg_Assignment *Asg)
  {
   MYSQL_RES *mysql_res;
   unsigned NumAsgs;

   if (Asg->AsgCod > 0)
     {
      /***** Build query *****/
      NumAsgs = Asg_DB_GetAssignmentDataByCod (&mysql_res,Asg->AsgCod);

      /***** Get data of assignment *****/
      Asg_GetAssignmentDataFromRow (&mysql_res,Asg,NumAsgs);
     }
   else
     {
      /***** Clear all assignment data *****/
      Asg->AsgCod = -1L;
      Asg_ResetAssignment (Asg);
     }
  }

/*****************************************************************************/
/*************** Get assignment data using its folder name *******************/
/*****************************************************************************/

void Asg_GetAssignmentDataByFolder (struct Asg_Assignment *Asg)
  {
   MYSQL_RES *mysql_res;
   unsigned NumAsgs;

   if (Asg->Folder[0])
     {
      /***** Query database *****/
      NumAsgs = Asg_DB_GetAssignmentDataByFolder (&mysql_res,Asg->Folder);

      /***** Get data of assignment *****/
      Asg_GetAssignmentDataFromRow (&mysql_res,Asg,NumAsgs);
     }
   else
     {
      /***** Clear all assignment data *****/
      Asg->AsgCod = -1L;
      Asg_ResetAssignment (Asg);
     }
  }

/*****************************************************************************/
/************************* Get assignment data *******************************/
/*****************************************************************************/

static void Asg_GetAssignmentDataFromRow (MYSQL_RES **mysql_res,
                                          struct Asg_Assignment *Asg,
                                          unsigned NumAsgs)
  {
   MYSQL_ROW row;

   /***** Clear all assignment data *****/
   Asg_ResetAssignment (Asg);

   /***** Get data of assignment from database *****/
   if (NumAsgs) // Assignment found...
     {
      /* Get next row from result */
      row = mysql_fetch_row (*mysql_res);
      /*
      row[0]	AsgCod
      row[1]	Hidden
      row[2]	UsrCod
      row[3]	UNIX_TIMESTAMP(StartTime)
      row[4]	UNIX_TIMESTAMP(EndTime)
      row[5]	NOW() BETWEEN StartTime AND EndTime
      row[6]	Title
      row[7]	Folder
      */

      /* Get code of the assignment (row[0]) */
      Asg->AsgCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get whether the assignment is hidden or not (row[1]) */
      Asg->HiddenOrVisible = (row[1][0] == 'Y') ? HidVis_HIDDEN :
						  HidVis_VISIBLE;

      /* Get author of the assignment (row[2]) */
      Asg->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get start and end dates (row[3] and row[4] hold the start UTC time) */
      Asg->TimeUTC[Dat_STR_TIME] = Dat_GetUNIXTimeFromStr (row[3]);
      Asg->TimeUTC[Dat_END_TIME] = Dat_GetUNIXTimeFromStr (row[4]);

      /* Get whether the assignment is open or closed (row(5)) */
      Asg->Open = (row[5][0] == '1');

      /* Get the title (row[6]) and the folder (row[7]) of the assignment  */
      Str_Copy (Asg->Title ,row[6],sizeof (Asg->Title ) - 1);
      Str_Copy (Asg->Folder,row[7],sizeof (Asg->Folder) - 1);
      Asg->SendWork = (Asg->Folder[0] != '\0');

      /* Can I do this assignment? */
      Asg->IBelongToCrsOrGrps = Asg_CheckIfIBelongToCrsOrGrpsThisAssignment (Asg->AsgCod);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************************* Clear all assignment data **************************/
/*****************************************************************************/

static void Asg_ResetAssignment (struct Asg_Assignment *Asg)
  {
   if (Asg->AsgCod <= 0)	// If > 0 ==> keep value
      Asg->AsgCod = -1L;
   Asg->AsgCod = -1L;
   Asg->HiddenOrVisible = HidVis_VISIBLE;
   Asg->UsrCod = -1L;
   Asg->TimeUTC[Dat_STR_TIME] =
   Asg->TimeUTC[Dat_END_TIME] = (time_t) 0;
   Asg->Open = false;
   Asg->Title[0] = '\0';
   Asg->SendWork = Asg_DO_NOT_SEND_WORK;
   Asg->Folder[0] = '\0';
   Asg->IBelongToCrsOrGrps = false;
  }

/*****************************************************************************/
/************************* Free list of assignments **************************/
/*****************************************************************************/

static void Asg_FreeListAssignments (struct Asg_Assignments *Assignments)
  {
   if (Assignments->LstIsRead &&
       Assignments->LstAsgCods)
     {
      /***** Free memory used by the list of assignments *****/
      free (Assignments->LstAsgCods);
      Assignments->LstAsgCods = NULL;
      Assignments->Num        = 0;
      Assignments->LstIsRead  = false;
     }
  }

/*****************************************************************************/
/***************** Get summary and content of an assignment  *****************/
/*****************************************************************************/
// This function may be called inside a web service

void Asg_GetNotifAssignment (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                             char **ContentStr,
                             long AsgCod,bool GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Query database. Result should have a unique row *****/
   if (Asg_DB_GetAssignmentTitleAndTxtByCod (&mysql_res,AsgCod) == 1)
     {
      /***** Get row *****/
      row = mysql_fetch_row (mysql_res);

      /***** Get summary *****/
      Str_Copy (SummaryStr,row[0],Ntf_MAX_BYTES_SUMMARY);

      /***** Get content *****/
      if (GetContent)
	{
	 Length = strlen (row[1]);
	 if ((*ContentStr = malloc (Length + 1)) == NULL)
            Err_NotEnoughMemoryExit ();
	 Str_Copy (*ContentStr,row[1],Length);
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Ask for confirmation of removing an assignment ****************/
/*****************************************************************************/

void Asg_ReqRemAssignment (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_assignment_X;
   struct Asg_Assignments Assignments;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParAsgOrder ();
   Gbl.Crs.Grps.WhichGrps = Grp_GetParWhichGroups ();
   Assignments.CurrentPage = Pag_GetParPagNum (Pag_ASSIGNMENTS);

   /***** Get assignment code *****/
   Assignments.Asg.AsgCod = ParCod_GetAndCheckPar (ParCod_Asg);

   /***** Get data of the assignment from database *****/
   Asg_GetAssignmentDataByCod (&Assignments.Asg);

   /***** Show question and button to remove the assignment *****/
   Ale_ShowAlertRemove (ActRemAsg,NULL,
                        Asg_PutPars,&Assignments,
			Txt_Do_you_really_want_to_remove_the_assignment_X,
			Assignments.Asg.Title);

   /***** Show all assignments again *****/
   Asg_ShowAllAssignments (&Assignments);
  }

/*****************************************************************************/
/*************************** Remove an assignment ****************************/
/*****************************************************************************/

void Asg_RemoveAssignment (void)
  {
   extern const char *Txt_Assignment_X_removed;
   struct Asg_Assignments Assignments;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParAsgOrder ();
   Gbl.Crs.Grps.WhichGrps = Grp_GetParWhichGroups ();
   Assignments.CurrentPage = Pag_GetParPagNum (Pag_ASSIGNMENTS);

   /***** Get assignment code *****/
   Assignments.Asg.AsgCod = ParCod_GetAndCheckPar (ParCod_Asg);

   /***** Get data of the assignment from database *****/
   Asg_GetAssignmentDataByCod (&Assignments.Asg);	// Inside this function, the course is checked to be the current one

   /***** Remove all folders associated to this assignment *****/
   if (Assignments.Asg.Folder[0])
      Brw_RemoveFoldersAssignmentsIfExistForAllUsrs (Assignments.Asg.Folder);

   /***** Remove all groups of this assignment *****/
   Asg_DB_RemoveGrpsAssociatedToAnAssignment (Assignments.Asg.AsgCod);

   /***** Remove assignment *****/
   Asg_DB_RemoveAssignment (Assignments.Asg.AsgCod);

   /***** Mark possible notifications as removed *****/
   Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_ASSIGNMENT,Assignments.Asg.AsgCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Assignment_X_removed,
                  Assignments.Asg.Title);

   /***** Show all assignments again *****/
   Asg_ShowAllAssignments (&Assignments);
  }

/*****************************************************************************/
/************************* Hide/unhide an assignment *************************/
/*****************************************************************************/

void Asg_HideAssignment (void)
  {
   Asg_HideUnhideAssignment (HidVis_HIDDEN);
  }

void Asg_UnhideAssignment (void)
  {
   Asg_HideUnhideAssignment (HidVis_VISIBLE);
  }

static void Asg_HideUnhideAssignment (HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   struct Asg_Assignments Assignments;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParAsgOrder ();
   Gbl.Crs.Grps.WhichGrps = Grp_GetParWhichGroups ();
   Assignments.CurrentPage = Pag_GetParPagNum (Pag_ASSIGNMENTS);

   /***** Get assignment code *****/
   Assignments.Asg.AsgCod = ParCod_GetAndCheckPar (ParCod_Asg);

   /***** Get data of the assignment from database *****/
   Asg_GetAssignmentDataByCod (&Assignments.Asg);

   /***** Unhide assignment *****/
   Asg_DB_HideOrUnhideAssignment (Assignments.Asg.AsgCod,HiddenOrVisible);

   /***** Show all assignments again *****/
   Asg_ShowAllAssignments (&Assignments);
  }

/*****************************************************************************/
/****************** Put a form to create a new assignment ********************/
/*****************************************************************************/

void Asg_ReqCreatOrEditAsg (void)
  {
   extern const char *Hlp_ASSESSMENT_Assignments_edit_assignment;
   extern const char *Txt_Assignment;
   extern const char *Txt_Title;
   extern const char *Txt_Upload_files_QUESTION;
   extern const char *Txt_Folder;
   extern const char *Txt_Description;
   extern const char *Txt_Create;
   extern const char *Txt_Save_changes;
   struct Asg_Assignments Assignments;
   bool ItsANewAssignment;
   char Txt[Cns_MAX_BYTES_TEXT + 1];
   static const Dat_SetHMS SetHMSDontSet[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME] = Dat_HMS_DO_NOT_SET
     };
   static const Dat_SetHMS SetHMSAllDay[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_TO_000000,
      [Dat_END_TIME] = Dat_HMS_TO_235959
     };

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParAsgOrder ();
   Grp_GetParWhichGroups ();
   Assignments.CurrentPage = Pag_GetParPagNum (Pag_ASSIGNMENTS);

   /***** Get the code of the assignment *****/
   ItsANewAssignment = ((Assignments.Asg.AsgCod = ParCod_GetPar (ParCod_Asg)) <= 0);

   /***** Get from the database the data of the assignment *****/
   if (ItsANewAssignment)
     {
      /* Initialize to empty assignment */
      Assignments.Asg.AsgCod = -1L;
      Assignments.Asg.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
      Assignments.Asg.TimeUTC[Dat_END_TIME] = Assignments.Asg.TimeUTC[Dat_STR_TIME] + (2 * 60 * 60);	// +2 hours
      Assignments.Asg.Open = true;
      Assignments.Asg.Title[0] = '\0';
      Assignments.Asg.SendWork = Asg_DO_NOT_SEND_WORK;
      Assignments.Asg.Folder[0] = '\0';
      Assignments.Asg.IBelongToCrsOrGrps = false;
     }
   else
     {
      /* Get data of the assignment from database */
      Asg_GetAssignmentDataByCod (&Assignments.Asg);

      /* Get text of the assignment from database */
      Asg_DB_GetAssignmentTxtByCod (Assignments.Asg.AsgCod,Txt);
     }

   /***** Begin form *****/
   if (ItsANewAssignment)
     {
      Frm_BeginForm (ActNewAsg);
      Assignments.Asg.AsgCod = -1L;
     }
   else
     {
      Frm_BeginForm (ActChgAsg);
      // Assignments.Asg.AsgCod = Asg.AsgCod;
     }
   Asg_PutPars (&Assignments);

      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,
			 Assignments.Asg.Title[0] ? Assignments.Asg.Title :
						    Txt_Assignment,
			 NULL,NULL,
			 Hlp_ASSESSMENT_Assignments_edit_assignment,Box_NOT_CLOSABLE,2);

	 /***** Assignment title *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RM","Title",Txt_Title);

	    /* Data */
	    HTM_TD_Begin ("class=\"LM\"");
	       HTM_INPUT_TEXT ("Title",Asg_MAX_CHARS_ASSIGNMENT_TITLE,Assignments.Asg.Title,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "id=\"Title\""
			       " class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\""
			       " required=\"required\"",
			       The_GetSuffix ());
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Assignment start and end dates *****/
	 Dat_PutFormStartEndClientLocalDateTimes (Assignments.Asg.TimeUTC,
						  Dat_FORM_SECONDS_ON,
						  Gbl.Action.Act == ActFrmNewAsg ? SetHMSAllDay :
										   SetHMSDontSet);

	 /***** Send work? *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RM","Folder",Txt_Upload_files_QUESTION);

	    /* Data */
	    HTM_TD_Begin ("class=\"LM\"");
	       HTM_LABEL_Begin ("class=\"DAT_%s\"",The_GetSuffix ());
		  HTM_TxtColon (Txt_Folder);
		  HTM_INPUT_TEXT ("Folder",Brw_MAX_CHARS_FOLDER,Assignments.Asg.Folder,
				  HTM_DONT_SUBMIT_ON_CHANGE,
				  "id=\"Folder\" size=\"30\" class=\"INPUT_%s\"",
				  The_GetSuffix ());
	       HTM_LABEL_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Assignment text *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","Txt",Txt_Description);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_TEXTAREA_Begin ("id=\"Txt\" name=\"Txt\" rows=\"10\""
				   " class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\"",
				   The_GetSuffix ());
		  if (!ItsANewAssignment)
		     HTM_Txt (Txt);
	       HTM_TEXTAREA_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Groups *****/
	 Asg_ShowLstGrpsToEditAssignment (Assignments.Asg.AsgCod);

      /***** End table, send button and end box *****/
      if (ItsANewAssignment)
	 Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create);
      else
	 Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** Show current assignments, if any *****/
   Asg_ShowAllAssignments (&Assignments);
  }

/*****************************************************************************/
/**************** Show list of groups to edit and assignment *****************/
/*****************************************************************************/

static void Asg_ShowLstGrpsToEditAssignment (long AsgCod)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *Txt_Groups;
   extern const char *Txt_The_whole_course;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   if (Gbl.Crs.Grps.GrpTypes.NumGrpTypes)
     {
      /***** Begin box and table *****/
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"RT FORM_IN_%s\"",The_GetSuffix ());
	    HTM_TxtColon (Txt_Groups);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"LT\"");

	    Box_BoxTableBegin ("100%",NULL,
			       NULL,NULL,
			       Hlp_USERS_Groups,Box_NOT_CLOSABLE,0);

	       /***** First row: checkbox to select the whole course *****/
	       HTM_TR_Begin (NULL);
		  HTM_TD_Begin ("colspan=\"7\" class=\"LM DAT_%s\"",
		                The_GetSuffix ());
		     HTM_LABEL_Begin (NULL);
			HTM_INPUT_CHECKBOX ("WholeCrs",HTM_DONT_SUBMIT_ON_CHANGE,
					    "id=\"WholeCrs\" value=\"Y\"%s"
					    " onclick=\"uncheckChildren(this,'GrpCods')\"",
					    Grp_DB_CheckIfAssociatedToGrps ("asg_groups",
					                                    "AsgCod",
					                                    AsgCod) ? "" :
										      " checked=\"checked\"");
			HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,Gbl.Hierarchy.Crs.ShrtName);
		     HTM_LABEL_End ();
		  HTM_TD_End ();
	       HTM_TR_End ();

	       /***** List the groups for each group type *****/
	       for (NumGrpTyp = 0;
		    NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
		    NumGrpTyp++)
		  if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
		     Grp_ListGrpsToEditAsgAttSvyEvtMch (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],
							Grp_ASSIGNMENT,AsgCod);

	    /***** End table and box *****/
	    Box_BoxTableEnd ();

	 HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/****************** Receive form to create a new assignment ******************/
/*****************************************************************************/

void Asg_ReceiveFormAssignment (void)
  {
   extern const char *Txt_Already_existed_an_assignment_with_the_title_X;
   extern const char *Txt_Already_existed_an_assignment_with_the_folder_X;
   extern const char *Txt_Created_new_assignment_X;
   extern const char *Txt_The_assignment_has_been_modified;
   extern const char *Txt_You_can_not_disable_file_uploading_once_folders_have_been_created;
   struct Asg_Assignments Assignments;
   struct Asg_Assignment OldAsg;	// Current assigment data in database
   bool ItsANewAssignment;
   bool NewAssignmentIsCorrect = true;
   unsigned NumUsrsToBeNotifiedByEMail;
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParAsgOrder ();
   Grp_GetParWhichGroups ();
   Assignments.CurrentPage = Pag_GetParPagNum (Pag_ASSIGNMENTS);

   /***** Get the code of the assignment *****/
   ItsANewAssignment = ((Assignments.Asg.AsgCod = ParCod_GetPar (ParCod_Asg)) <= 0);

   if (ItsANewAssignment)
     {
      /***** Reset old (current, not existing) assignment data *****/
      OldAsg.AsgCod = -1L;
      Asg_ResetAssignment (&OldAsg);
     }
   else
     {
      /***** Get data of the old (current) assignment from database *****/
      OldAsg.AsgCod = Assignments.Asg.AsgCod;
      Asg_GetAssignmentDataByCod (&OldAsg);
     }

   /***** Get start/end date-times *****/
   Assignments.Asg.TimeUTC[Dat_STR_TIME] = Dat_GetTimeUTCFromForm (Dat_STR_TIME);
   Assignments.Asg.TimeUTC[Dat_END_TIME] = Dat_GetTimeUTCFromForm (Dat_END_TIME);

   /***** Get assignment title *****/
   Par_GetParText ("Title",Assignments.Asg.Title,Asg_MAX_BYTES_ASSIGNMENT_TITLE);

   /***** Get folder name where to send works of the assignment *****/
   Par_GetParText ("Folder",Assignments.Asg.Folder,Brw_MAX_BYTES_FOLDER);
   Assignments.Asg.SendWork = (Assignments.Asg.Folder[0]) ? Asg_SEND_WORK :
							    Asg_DO_NOT_SEND_WORK;

   /***** Get assignment text *****/
   Par_GetParHTML ("Txt",Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (Assignments.Asg.TimeUTC[Dat_STR_TIME] == 0)
      Assignments.Asg.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
   if (Assignments.Asg.TimeUTC[Dat_END_TIME] == 0)
      Assignments.Asg.TimeUTC[Dat_END_TIME] = Assignments.Asg.TimeUTC[Dat_STR_TIME] + 2 * 60 * 60;	// +2 hours

   /***** Check if title is correct *****/
   if (Assignments.Asg.Title[0])	// If there's an assignment title
     {
      /* If title of assignment was in database... */
      if (Asg_DB_CheckIfSimilarAssignmentExists ("Title",Assignments.Asg.Title,Assignments.Asg.AsgCod))
        {
         NewAssignmentIsCorrect = false;

	 Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_an_assignment_with_the_title_X,
                        Assignments.Asg.Title);
        }
      else	// Title is correct
        {
         if (Assignments.Asg.SendWork == Asg_SEND_WORK)
           {
            if (Str_ConvertFilFolLnkNameToValid (Assignments.Asg.Folder))	// If folder name is valid...
              {
               if (Asg_DB_CheckIfSimilarAssignmentExists ("Folder",Assignments.Asg.Folder,Assignments.Asg.AsgCod))	// If folder of assignment was in database...
                 {
                  NewAssignmentIsCorrect = false;

		  Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_an_assignment_with_the_folder_X,
                                 Assignments.Asg.Folder);
                 }
              }
            else	// Folder name not valid
              {
               NewAssignmentIsCorrect = false;
               Ale_ShowAlerts (NULL);
              }
           }
         else	// NewAsg.SendWork == Asg_DO_NOT_SEND_WORK
           {
            if (OldAsg.SendWork == Asg_SEND_WORK)
              {
               if (Brw_CheckIfExistsFolderAssigmentForAnyUsr (OldAsg.Folder))
                 {
                  NewAssignmentIsCorrect = false;
                  Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_disable_file_uploading_once_folders_have_been_created);
                 }
              }
           }
        }
     }
   else	// If there is not an assignment title
     {
      NewAssignmentIsCorrect = false;
      Ale_CreateAlertYouMustSpecifyTheTitle ();
     }

   /***** Create a new assignment or update an existing one *****/
   if (NewAssignmentIsCorrect)
     {
      /* Get groups for this assignments */
      Grp_GetParCodsSeveralGrps ();

      if (ItsANewAssignment)
	{
         Asg_CreateAssignment (&Assignments.Asg,Description);	// Add new assignment to database

	 /***** Write success message *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_assignment_X,
		        Assignments.Asg.Title);
	}
      else
        {
         if (OldAsg.Folder[0] && Assignments.Asg.Folder[0])
            if (strcmp (OldAsg.Folder,Assignments.Asg.Folder))	// Folder name has changed
               NewAssignmentIsCorrect = Brw_UpdateFoldersAssigmentsIfExistForAllUsrs (OldAsg.Folder,
                                                                                      Assignments.Asg.Folder);
         if (NewAssignmentIsCorrect)
           {
            Asg_UpdateAssignment (&Assignments.Asg,Description);

	    /***** Write success message *****/
	    Ale_ShowAlert (Ale_SUCCESS,Txt_The_assignment_has_been_modified);
           }
        }

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();

      /***** Notify by email about the new assignment *****/
      if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_ASSIGNMENT,Assignments.Asg.AsgCod)))
	 Asg_DB_UpdateNumUsrsNotifiedByEMailAboutAssignment (Assignments.Asg.AsgCod,
	                                                     NumUsrsToBeNotifiedByEMail);

      /***** Show all assignments again *****/
      Asg_ShowAllAssignments (&Assignments);
     }
   else
      // TODO: The form should be filled with partial data, now is always empty
      Asg_ReqCreatOrEditAsg ();
  }

/*****************************************************************************/
/************************ Create a new assignment ****************************/
/*****************************************************************************/

static void Asg_CreateAssignment (struct Asg_Assignment *Asg,const char *Txt)
  {
   /***** Create a new assignment *****/
   Asg->AsgCod = Asg_DB_CreateAssignment (Asg,Txt);

   /***** Create groups *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Asg_CreateGroups (Asg->AsgCod);
  }

/*****************************************************************************/
/********************* Update an existing assignment *************************/
/*****************************************************************************/

static void Asg_UpdateAssignment (struct Asg_Assignment *Asg,const char *Txt)
  {
   /***** Update the data of the assignment *****/
   Asg_DB_UpdateAssignment (Asg,Txt);

   /***** Update groups *****/
   /* Remove old groups */
   Asg_DB_RemoveGrpsAssociatedToAnAssignment (Asg->AsgCod);

   /* Create new groups */
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Asg_CreateGroups (Asg->AsgCod);
  }

/*****************************************************************************/
/********************* Create groups of an assignment ************************/
/*****************************************************************************/

static void Asg_CreateGroups (long AsgCod)
  {
   unsigned NumGrp;

   /***** Create groups of the assignment *****/
   for (NumGrp = 0;
	NumGrp < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrp++)
      Asg_DB_CreateGroup (AsgCod,Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrp]);
  }

/*****************************************************************************/
/********* Get and write the names of the groups of an assignment ************/
/*****************************************************************************/

static void Asg_GetAndWriteNamesOfGrpsAssociatedToAsg (struct Asg_Assignment *Asg)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_and;
   extern const char *Txt_The_whole_course;
   extern const char *HidVis_GroupClass[HidVis_NUM_HIDDEN_VISIBLE];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrps;
   unsigned NumGrp;

   /***** Get groups associated to an assignment from database *****/
   NumGrps = Asg_DB_GetGrps (&mysql_res,Asg->AsgCod);

   /***** Write heading *****/
   HTM_DIV_Begin ("class=\"%s_%s\"",
                  HidVis_GroupClass[Asg->HiddenOrVisible],The_GetSuffix ());

      HTM_TxtColonNBSP (NumGrps == 1 ? Txt_Group  :
				       Txt_Groups);

      /***** Write groups *****/
      if (NumGrps) // Groups found...
	 /* Get and write the group types and names */
	 for (NumGrp = 0;
	      NumGrp < NumGrps;
	      NumGrp++)
	   {
	    /* Get next group */
	    row = mysql_fetch_row (mysql_res);

	    /* Write group type name and group name */
	    HTM_TxtF ("%s&nbsp;%s",row[0],row[1]);

	    if (NumGrps >= 2)
	      {
	       if (NumGrp == NumGrps - 2)
		  HTM_TxtF (" %s ",Txt_and);
	       if (NumGrps >= 3)
		 if (NumGrp < NumGrps - 2)
		     HTM_Txt (", ");
	      }
	   }
      else
	 HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,Gbl.Hierarchy.Crs.ShrtName);

   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Remove all assignments from a course *******************/
/*****************************************************************************/

void Asg_RemoveCrsAssignments (long CrsCod)
  {
   /***** Remove groups associated to assignments in course *****/
   Asg_DB_RemoveGrpsAssociatedToAsgsInCrs (CrsCod);

   /***** Remove assignments *****/
   Asg_DB_RemoveCrsAssignments (CrsCod);
  }

/*****************************************************************************/
/********* Check if I belong to any of the groups of an assignment ***********/
/*****************************************************************************/

static bool Asg_CheckIfIBelongToCrsOrGrpsThisAssignment (long AsgCod)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 // Students and teachers can do assignments depending on groups
	 /***** Get if I can do an assignment from database *****/
	 return Asg_DB_CheckIfICanDoAssignment (AsgCod);
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
  }

/*****************************************************************************/
/************************ Get number of assignments **************************/
/*****************************************************************************/
// Returns the number of assignments
// in this location (all the platform, the current degree or the current course)

unsigned Asg_GetNumAssignments (HieLvl_Level_t Level,unsigned *NumNotif)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAssignments;

   /***** Default values *****/
   NumAssignments = 0;
   *NumNotif = 0;

   /***** Get number of assignments from database *****/
   if (Asg_DB_GetNumAssignments (&mysql_res,Level))
     {
      /***** Get number of assignments *****/
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",&NumAssignments) != 1)
	 Err_ShowErrorAndExit ("Error when getting number of assignments.");

      /***** Get number of notifications by email *****/
      if (row[1])
	 if (sscanf (row[1],"%u",NumNotif) != 1)
	    Err_ShowErrorAndExit ("Error when getting number of notifications of assignments.");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumAssignments;
  }

/*****************************************************************************/
/************************ Show stats about assignments ***********************/
/*****************************************************************************/

void Asg_GetAndShowAssignmentsStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_assignments;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Number_of_BR_assignments;
   extern const char *Txt_Number_of_BR_courses_with_BR_assignments;
   extern const char *Txt_Average_number_BR_of_ASSIG_BR_per_course;
   extern const char *Txt_Number_of_BR_notifications;
   unsigned NumAssignments;
   unsigned NumNotif;
   unsigned NumCoursesWithAssignments = 0;
   double NumAssignmentsPerCourse = 0.0;

   /***** Get the number of assignments from this location *****/
   if ((NumAssignments = Asg_GetNumAssignments (Gbl.Scope.Current,&NumNotif)))
      if ((NumCoursesWithAssignments = Asg_DB_GetNumCoursesWithAssignments (Gbl.Scope.Current)) != 0)
         NumAssignmentsPerCourse = (double) NumAssignments /
	                           (double) NumCoursesWithAssignments;

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_FIGURE_TYPES[Fig_ASSIGNMENTS],
                      NULL,NULL,
                      Hlp_ANALYTICS_Figures_assignments,Box_NOT_CLOSABLE,2);

      /***** Write table heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Number_of_BR_assignments                ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_BR_courses_with_BR_assignments,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Average_number_BR_of_ASSIG_BR_per_course,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_BR_notifications              ,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Write number of assignments *****/
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (NumAssignments);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (NumCoursesWithAssignments);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Double2Decimals (NumAssignmentsPerCourse);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (NumNotif);
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
