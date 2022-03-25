// swad_assignment.c: assignments

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

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
#include "swad_HTML.h"
#include "swad_notification.h"
#include "swad_notification_database.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_photo.h"
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

static void Asg_ResetAssignments (struct Asg_Assignments *Assignments);

static void Asg_ShowAllAssignments (struct Asg_Assignments *Assignments);
static void Asg_PutHeadForSeeing (struct Asg_Assignments *Assignments,
                                  bool PrintView);
static bool Asg_CheckIfICanCreateAssignments (void);
static void Asg_PutIconsListAssignments (void *Assignments);
static void Asg_PutIconToCreateNewAsg (void *Assignments);
static void Asg_PutButtonToCreateNewAsg (void *Assignments);
static void Asg_ParamsWhichGroupsToShow (void *Assignments);
static void Asg_ShowOneAssignment (struct Asg_Assignments *Assignments,
                                   long AsgCod,bool PrintView);
static void Asg_WriteAsgAuthor (struct Asg_Assignment *Asg);
static void Asg_WriteAssignmentFolder (struct Asg_Assignment *Asg,bool PrintView);
static Dat_StartEndTime_t Asg_GetParamAsgOrder (void);

static void Asg_PutFormsToRemEditOneAsg (struct Asg_Assignments *Assignments,
                                         const struct Asg_Assignment *Asg,
                                         const char *Anchor);
static void Asg_PutParams (void *Assignments);
static void Asg_GetListAssignments (struct Asg_Assignments *Assignments);
static void Asg_GetDataOfAssignment (struct Asg_Assignment *Asg,
                                     MYSQL_RES **mysql_res,
				     unsigned NumAsgs);
static void Asg_ResetAssignment (struct Asg_Assignment *Asg);
static void Asg_FreeListAssignments (struct Asg_Assignments *Assignments);
static void Asg_PutParamAsgCod (long AsgCod);
static void Asg_ShowLstGrpsToEditAssignment (long AsgCod);
static void Asg_CreateAssignment (struct Asg_Assignment *Asg,const char *Txt);
static void Asg_UpdateAssignment (struct Asg_Assignment *Asg,const char *Txt);
static void Asg_CreateGroups (long AsgCod);
static void Asg_GetAndWriteNamesOfGrpsAssociatedToAsg (struct Asg_Assignment *Asg);
static bool Asg_CheckIfIBelongToCrsOrGrpsThisAssignment (long AsgCod);

/*****************************************************************************/
/*************************** Reset assignments *******************************/
/*****************************************************************************/

static void Asg_ResetAssignments (struct Asg_Assignments *Assignments)
  {
   Assignments->LstIsRead     = false;	// List is not read
   Assignments->Num           = 0;
   Assignments->LstAsgCods    = NULL;
   Assignments->SelectedOrder = Asg_ORDER_DEFAULT;
   Assignments->AsgCodToEdit  = -1L;	// Used as parameter in contextual links
   Assignments->CurrentPage   = 0;
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
   Assignments.SelectedOrder = Asg_GetParamAsgOrder ();
   Gbl.Crs.Grps.WhichGrps = Grp_GetParamWhichGroups ();
   Assignments.CurrentPage = Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Show all assignments *****/
   Asg_ShowAllAssignments (&Assignments);
  }

/*****************************************************************************/
/**************************** Show all assignments ***************************/
/*****************************************************************************/

static void Asg_ShowAllAssignments (struct Asg_Assignments *Assignments)
  {
   extern const char *Hlp_ASSESSMENT_Assignments;
   extern const char *Txt_Assignments;
   extern const char *Txt_No_assignments;
   struct Pagination Pagination;
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
      Grp_ShowFormToSelWhichGrps (ActSeeAsg,
                                  Asg_ParamsWhichGroupsToShow,Assignments);
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
	 Asg_PutHeadForSeeing (Assignments,false);	// Not print view

	 /***** Write all assignments *****/
	 for (NumAsg  = Pagination.FirstItemVisible;
	      NumAsg <= Pagination.LastItemVisible;
	      NumAsg++)
	    Asg_ShowOneAssignment (Assignments,
				   Assignments->LstAsgCods[NumAsg - 1],
				   false);	// Not print view

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No assignments created
      Ale_ShowAlert (Ale_INFO,Txt_No_assignments);

   /***** Write again links to pages *****/
   Pag_WriteLinksToPagesCentered (Pag_ASSIGNMENTS,&Pagination,
				  Assignments,-1L);

   /***** Button to create a new assignment *****/
   if (Asg_CheckIfICanCreateAssignments ())
      Asg_PutButtonToCreateNewAsg (Assignments);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of assignments *****/
   Asg_FreeListAssignments (Assignments);
  }

/*****************************************************************************/
/***************** Write header with fields of an assignment *****************/
/*****************************************************************************/

static void Asg_PutHeadForSeeing (struct Asg_Assignments *Assignments,
                                  bool PrintView)
  {
   extern const char *Txt_START_END_TIME_HELP[Dat_NUM_START_END_TIME];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Assignment;
   extern const char *Txt_Folder;
   Dat_StartEndTime_t Order;
   Grp_WhichGroups_t WhichGroups;

   HTM_TR_Begin (NULL);

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
	       Frm_BeginForm (ActSeeAsg);
		  WhichGroups = Grp_GetParamWhichGroups ();
		  Grp_PutParamWhichGroups (&WhichGroups);
		  Pag_PutHiddenParamPagNum (Pag_ASSIGNMENTS,Assignments->CurrentPage);
		  Dat_PutHiddenParamOrder (Order);

	          /* Begin link to select order */
		  HTM_BUTTON_OnSubmit_Begin (Txt_START_END_TIME_HELP[Order],
		                             "BT_LINK",NULL);
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
      if (Asg_CheckIfICanCreateAssignments ())
	 Asg_PutIconToCreateNewAsg (Assignments);

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_ASSIGNMENTS);
  }

/*****************************************************************************/
/******************* Put icon to create a new assignment *********************/
/*****************************************************************************/

static void Asg_PutIconToCreateNewAsg (void *Assignments)
  {
   extern const char *Txt_New_assignment;

   if (Assignments)
     {
      /***** Put form to create a new assignment *****/
      ((struct Asg_Assignments *) Assignments)->AsgCodToEdit = -1L;
      Ico_PutContextualIconToAdd (ActFrmNewAsg,NULL,
				  Asg_PutParams,Assignments,
				  Txt_New_assignment);
     }
  }

/*****************************************************************************/
/****************** Put button to create a new assignment ********************/
/*****************************************************************************/

static void Asg_PutButtonToCreateNewAsg (void *Assignments)
  {
   extern const char *Txt_New_assignment;

   if (Assignments)
     {
      /* Begin form */
      Frm_BeginForm (ActFrmNewAsg);
	 ((struct Asg_Assignments *) Assignments)->AsgCodToEdit = -1L;
	 Asg_PutParams (Assignments);

         /* Button to create new assignment */
	 Btn_PutConfirmButton (Txt_New_assignment);

      /* End form */
      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/**************** Put params to select which groups to show ******************/
/*****************************************************************************/

static void Asg_ParamsWhichGroupsToShow (void *Assignments)
  {
   if (Assignments)
     {
      Dat_PutHiddenParamOrder (((struct Asg_Assignments *) Assignments)->SelectedOrder);
      Pag_PutHiddenParamPagNum (Pag_ASSIGNMENTS,
                                ((struct Asg_Assignments *) Assignments)->CurrentPage);
     }
  }

/*****************************************************************************/
/******************** Show print view of one assignment **********************/
/*****************************************************************************/

void Asg_PrintOneAssignment (void)
  {
   struct Asg_Assignments Assignments;
   long AsgCod;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get the code of the assignment *****/
   AsgCod = Asg_GetParamAsgCod ();

   /***** Write header *****/
   Lay_WriteHeaderClassPhoto (true,false,
			      Gbl.Hierarchy.Ins.InsCod,
			      Gbl.Hierarchy.Deg.DegCod,
			      Gbl.Hierarchy.Crs.CrsCod);

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (2);

      /***** Table head *****/
      Asg_PutHeadForSeeing (&Assignments,
			    true);		// Print view

      /***** Write assignment *****/
      Asg_ShowOneAssignment (&Assignments,
			     AsgCod,
			     true);	// Print view

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/*************************** Show one assignment *****************************/
/*****************************************************************************/

static void Asg_ShowOneAssignment (struct Asg_Assignments *Assignments,
                                   long AsgCod,bool PrintView)
  {
   extern const char *The_Colors[The_NUM_THEMES];
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   char *Id;
   struct Asg_Assignment Asg;
   Dat_StartEndTime_t StartEndTime;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get data of this assignment *****/
   Asg.AsgCod = AsgCod;
   Asg_GetDataOfAssignmentByCod (&Asg);

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Asg.AsgCod,&Anchor);

   /***** Write first row of data of this assignment *****/
   HTM_TR_Begin (NULL);

      /* Forms to remove/edit this assignment */
      if (PrintView)
	 HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL\"");
      else
	{
	 HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL %s\"",
	               The_GetColorRows ());
	 Asg_PutFormsToRemEditOneAsg (Assignments,&Asg,Anchor);
	}
      HTM_TD_End ();

      /* Start/end date/time */
      UniqueId++;

      for (StartEndTime  = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
	{
	 if (asprintf (&Id,"asg_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	    Err_NotEnoughMemoryExit ();
	 if (PrintView)
	    HTM_TD_Begin ("id=\"%s\" class=\"%s LB\"",
			  Id,
			  Asg.Hidden ? (Asg.Open ? "DATE_GREEN_LIGHT" :
						   "DATE_RED_LIGHT") :
				       (Asg.Open ? "DATE_GREEN" :
						   "DATE_RED"));
	 else
	    HTM_TD_Begin ("id=\"%s\" class=\"%s LB %s\"",
			  Id,
			  Asg.Hidden ? (Asg.Open ? "DATE_GREEN_LIGHT" :
						   "DATE_RED_LIGHT") :
				       (Asg.Open ? "DATE_GREEN" :
						   "DATE_RED"),
			  The_GetColorRows ());
	 Dat_WriteLocalDateHMSFromUTC (Id,Asg.TimeUTC[StartEndTime],
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       true,true,true,0x7);
	 HTM_TD_End ();
	 free (Id);
	}

      /* Assignment title */
      if (PrintView)
	 HTM_TD_Begin ("class=\"%s LT\"",
		       Asg.Hidden ? "ASG_TITLE_LIGHT" :
				    "ASG_TITLE");
      else
	 HTM_TD_Begin ("class=\"%s LT %s\"",
		       Asg.Hidden ? "ASG_TITLE_LIGHT" :
				    "ASG_TITLE",
		       The_GetColorRows ());
      HTM_ARTICLE_Begin (Anchor);
      HTM_Txt (Asg.Title);
      HTM_ARTICLE_End ();
      HTM_TD_End ();

      /* Assignment folder */
      if (PrintView)
	 HTM_TD_Begin ("class=\"LT DAT_%s\"",
	               The_Colors[Gbl.Prefs.Theme]);
      else
	 HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	               The_Colors[Gbl.Prefs.Theme],
	               The_GetColorRows ());
      if (Asg.SendWork == Asg_SEND_WORK)
	 Asg_WriteAssignmentFolder (&Asg,PrintView);
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
      Asg_WriteAsgAuthor (&Asg);
      HTM_TD_End ();

      /* Text of the assignment */
      Asg_DB_GetAssignmentTxtByCod (Asg.AsgCod,Txt);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
      ALn_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
      if (PrintView)
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
      else
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",The_GetColorRows ());
      if (Gbl.Crs.Grps.NumGrps)
	 Asg_GetAndWriteNamesOfGrpsAssociatedToAsg (&Asg);

      HTM_DIV_Begin ("class=\"PAR %s_%s\"",
                     Asg.Hidden ? "DAT_LIGHT" :
				  "DAT",
		     The_Colors[Gbl.Prefs.Theme]);
	 HTM_Txt (Txt);
      HTM_DIV_End ();

      HTM_TD_End ();

   HTM_TR_End ();

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);

   The_ChangeRowColor ();

   /***** Mark possible notification as seen *****/
   Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_ASSIGNMENT,AsgCod);
  }

/*****************************************************************************/
/********************* Write the author of an assignment *********************/
/*****************************************************************************/

static void Asg_WriteAsgAuthor (struct Asg_Assignment *Asg)
  {
   Usr_WriteAuthor1Line (Asg->UsrCod,Asg->Hidden);
  }

/*****************************************************************************/
/********************* Write the folder of an assignment *********************/
/*****************************************************************************/

static void Asg_WriteAssignmentFolder (struct Asg_Assignment *Asg,bool PrintView)
  {
   extern const char *Txt_Upload_file_or_create_folder;
   extern const char *Txt_Folder;
   bool ICanSendFiles = !Asg->Hidden &&			// It's visible (not hidden)
                        Asg->Open &&			// It's open (inside dates)
                        Asg->IBelongToCrsOrGrps;	// I belong to course or groups

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
	    Frm_BeginForm (ActFrmCreAsgUsr);
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    Gbl.FileBrowser.Type = Brw_ADMI_ASG_CRS;	// Course assignments
	    Str_Copy (Gbl.Usrs.Other.UsrDat.EnUsrCod,Gbl.Usrs.Me.UsrDat.EnUsrCod,
		      sizeof (Gbl.Usrs.Other.UsrDat.EnUsrCod) - 1);
	    Usr_CreateListSelectedUsrsCodsAndFillWithOtherUsr (&Gbl.Usrs.Selected);
	    Frm_BeginForm (ActFrmCreAsgCrs);
	    break;
	 default:
            Err_WrongRoleExit ();
	    break;
        }

	 Str_Copy (Gbl.FileBrowser.FilFolLnk.Path,Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
		   sizeof (Gbl.FileBrowser.FilFolLnk.Path) - 1);
	 Str_Copy (Gbl.FileBrowser.FilFolLnk.Name,Asg->Folder,
		   sizeof (Gbl.FileBrowser.FilFolLnk.Name) - 1);
	 Gbl.FileBrowser.FilFolLnk.Type = Brw_IS_FOLDER;
	 Brw_PutImplicitParamsFileBrowser (&Gbl.FileBrowser.FilFolLnk);
	 Ico_PutIconLink ("folder-open-yellow-plus.png",Ico_UNCHANGED,
			  Txt_Upload_file_or_create_folder);

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

static Dat_StartEndTime_t Asg_GetParamAsgOrder (void)
  {
   return (Dat_StartEndTime_t)
   Par_GetParToUnsignedLong ("Order",
			     0,
			     Dat_NUM_START_END_TIME - 1,
			     (unsigned long) Asg_ORDER_DEFAULT);
  }

/*****************************************************************************/
/***************** Put a link (form) to edit one assignment ******************/
/*****************************************************************************/

static void Asg_PutFormsToRemEditOneAsg (struct Asg_Assignments *Assignments,
                                         const struct Asg_Assignment *Asg,
                                         const char *Anchor)
  {
   /***** Set assigment to edit
          (used as parameter in contextual links) *****/
   Assignments->AsgCodToEdit = Asg->AsgCod;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Put form to remove assignment *****/
	 Ico_PutContextualIconToRemove (ActReqRemAsg,NULL,
	                                Asg_PutParams,Assignments);

	 /***** Put form to hide/show assignment *****/
	 if (Asg->Hidden)
	    Ico_PutContextualIconToUnhide (ActShoAsg,Anchor,
	                                   Asg_PutParams,Assignments);
	 else
	    Ico_PutContextualIconToHide (ActHidAsg,Anchor,
	                                 Asg_PutParams,Assignments);

	 /***** Put form to edit assignment *****/
	 Ico_PutContextualIconToEdit (ActEdiOneAsg,NULL,
	                              Asg_PutParams,Assignments);
	 /* falls through */
	 /* no break */
      case Rol_STD:
      case Rol_NET:
	 /***** Put form to print assignment *****/
	 Ico_PutContextualIconToPrint (ActPrnOneAsg,
	                               Asg_PutParams,Assignments);
	 break;
      default:
         break;
     }
  }

/*****************************************************************************/
/******************** Params used to edit an assignment **********************/
/*****************************************************************************/

static void Asg_PutParams (void *Assignments)
  {
   Grp_WhichGroups_t WhichGroups;

   if (Assignments)
     {
      if (((struct Asg_Assignments *) Assignments)->AsgCodToEdit > 0)
	 Asg_PutParamAsgCod (((struct Asg_Assignments *) Assignments)->AsgCodToEdit);
      Dat_PutHiddenParamOrder (((struct Asg_Assignments *) Assignments)->SelectedOrder);
      WhichGroups = Grp_GetParamWhichGroups ();
      Grp_PutParamWhichGroups (&WhichGroups);
      Pag_PutHiddenParamPagNum (Pag_ASSIGNMENTS,((struct Asg_Assignments *) Assignments)->CurrentPage);
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

void Asg_GetDataOfAssignmentByCod (struct Asg_Assignment *Asg)
  {
   MYSQL_RES *mysql_res;
   unsigned NumAsgs;

   if (Asg->AsgCod > 0)
     {
      /***** Build query *****/
      NumAsgs = Asg_DB_GetDataOfAssignmentByCod (&mysql_res,Asg->AsgCod);

      /***** Get data of assignment *****/
      Asg_GetDataOfAssignment (Asg,&mysql_res,NumAsgs);
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

void Asg_GetDataOfAssignmentByFolder (struct Asg_Assignment *Asg)
  {
   MYSQL_RES *mysql_res;
   unsigned NumAsgs;

   if (Asg->Folder[0])
     {
      /***** Query database *****/
      NumAsgs = Asg_DB_GetDataOfAssignmentByFolder (&mysql_res,Asg->Folder);

      /***** Get data of assignment *****/
      Asg_GetDataOfAssignment (Asg,&mysql_res,NumAsgs);
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

static void Asg_GetDataOfAssignment (struct Asg_Assignment *Asg,
                                     MYSQL_RES **mysql_res,
				     unsigned NumAsgs)
  {
   MYSQL_ROW row;

   /***** Clear all assignment data *****/
   Asg_ResetAssignment (Asg);

   /***** Get data of assignment from database *****/
   if (NumAsgs) // Assignment found...
     {
      /* Get row */
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
      Asg->Hidden = (row[1][0] == 'Y');

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
   Asg->Hidden = false;
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
/***************** Write parameter with code of assignment *******************/
/*****************************************************************************/

static void Asg_PutParamAsgCod (long AsgCod)
  {
   Par_PutHiddenParamLong (NULL,"AsgCod",AsgCod);
  }

/*****************************************************************************/
/****************** Get parameter with code of assignment ********************/
/*****************************************************************************/

long Asg_GetParamAsgCod (void)
  {
   /***** Get code of assignment *****/
   return Par_GetParToLong ("AsgCod");
  }

/*****************************************************************************/
/************* Ask for confirmation of removing an assignment ****************/
/*****************************************************************************/

void Asg_ReqRemAssignment (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_assignment_X;
   extern const char *Txt_Remove_assignment;
   struct Asg_Assignments Assignments;
   struct Asg_Assignment Asg;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParamAsgOrder ();
   Gbl.Crs.Grps.WhichGrps = Grp_GetParamWhichGroups ();
   Assignments.CurrentPage = Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Get assignment code *****/
   if ((Asg.AsgCod = Asg_GetParamAsgCod ()) <= 0)
      Err_WrongAssignmentExit ();

   /***** Get data of the assignment from database *****/
   Asg_GetDataOfAssignmentByCod (&Asg);

   /***** Show question and button to remove the assignment *****/
   Assignments.AsgCodToEdit = Asg.AsgCod;
   Ale_ShowAlertAndButton (ActRemAsg,NULL,NULL,
                           Asg_PutParams,&Assignments,
                           Btn_REMOVE_BUTTON,Txt_Remove_assignment,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_assignment_X,
                           Asg.Title);

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
   struct Asg_Assignment Asg;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParamAsgOrder ();
   Gbl.Crs.Grps.WhichGrps = Grp_GetParamWhichGroups ();
   Assignments.CurrentPage = Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Get assignment code *****/
   if ((Asg.AsgCod = Asg_GetParamAsgCod ()) <= 0)
      Err_WrongAssignmentExit ();

   /***** Get data of the assignment from database *****/
   Asg_GetDataOfAssignmentByCod (&Asg);	// Inside this function, the course is checked to be the current one

   /***** Remove all folders associated to this assignment *****/
   if (Asg.Folder[0])
      Brw_RemoveFoldersAssignmentsIfExistForAllUsrs (Asg.Folder);

   /***** Remove all groups of this assignment *****/
   Asg_DB_RemoveGrpsAssociatedToAnAssignment (Asg.AsgCod);

   /***** Remove assignment *****/
   Asg_DB_RemoveAssignment (Asg.AsgCod);

   /***** Mark possible notifications as removed *****/
   Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_ASSIGNMENT,Asg.AsgCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Assignment_X_removed,
                  Asg.Title);

   /***** Show all assignments again *****/
   Asg_ShowAllAssignments (&Assignments);
  }

/*****************************************************************************/
/**************************** Hide an assignment *****************************/
/*****************************************************************************/

void Asg_HideAssignment (void)
  {
   struct Asg_Assignments Assignments;
   struct Asg_Assignment Asg;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParamAsgOrder ();
   Gbl.Crs.Grps.WhichGrps = Grp_GetParamWhichGroups ();
   Assignments.CurrentPage = Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Get assignment code *****/
   if ((Asg.AsgCod = Asg_GetParamAsgCod ()) <= 0)
      Err_WrongAssignmentExit ();

   /***** Get data of the assignment from database *****/
   Asg_GetDataOfAssignmentByCod (&Asg);

   /***** Hide assignment *****/
   Asg_DB_HideOrUnhideAssignment (Asg.AsgCod,true);

   /***** Show all assignments again *****/
   Asg_ShowAllAssignments (&Assignments);
  }

/*****************************************************************************/
/**************************** Unhide an assignment ***************************/
/*****************************************************************************/

void Asg_UnhideAssignment (void)
  {
   struct Asg_Assignments Assignments;
   struct Asg_Assignment Asg;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParamAsgOrder ();
   Gbl.Crs.Grps.WhichGrps = Grp_GetParamWhichGroups ();
   Assignments.CurrentPage = Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Get assignment code *****/
   if ((Asg.AsgCod = Asg_GetParamAsgCod ()) <= 0)
      Err_WrongAssignmentExit ();

   /***** Get data of the assignment from database *****/
   Asg_GetDataOfAssignmentByCod (&Asg);

   /***** Unhide assignment *****/
   Asg_DB_HideOrUnhideAssignment (Asg.AsgCod,false);

   /***** Show all assignments again *****/
   Asg_ShowAllAssignments (&Assignments);
  }

/*****************************************************************************/
/****************** Put a form to create a new assignment ********************/
/*****************************************************************************/

void Asg_RequestCreatOrEditAsg (void)
  {
   extern const char *Hlp_ASSESSMENT_Assignments_new_assignment;
   extern const char *Hlp_ASSESSMENT_Assignments_edit_assignment;
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *The_ClassInput[The_NUM_THEMES];
   extern const char *Txt_New_assignment;
   extern const char *Txt_Edit_assignment;
   extern const char *Txt_Title;
   extern const char *Txt_Upload_files_QUESTION;
   extern const char *Txt_Folder;
   extern const char *Txt_Description;
   extern const char *Txt_Create_assignment;
   extern const char *Txt_Save_changes;
   struct Asg_Assignments Assignments;
   struct Asg_Assignment Asg;
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
   Assignments.SelectedOrder = Asg_GetParamAsgOrder ();
   Grp_GetParamWhichGroups ();
   Assignments.CurrentPage = Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Get the code of the assignment *****/
   ItsANewAssignment = ((Asg.AsgCod = Asg_GetParamAsgCod ()) <= 0);

   /***** Get from the database the data of the assignment *****/
   if (ItsANewAssignment)
     {
      /* Initialize to empty assignment */
      Asg.AsgCod = -1L;
      Asg.TimeUTC[Dat_STR_TIME] = Gbl.StartExecutionTimeUTC;
      Asg.TimeUTC[Dat_END_TIME] = Gbl.StartExecutionTimeUTC + (2 * 60 * 60);	// +2 hours
      Asg.Open = true;
      Asg.Title[0] = '\0';
      Asg.SendWork = Asg_DO_NOT_SEND_WORK;
      Asg.Folder[0] = '\0';
      Asg.IBelongToCrsOrGrps = false;
     }
   else
     {
      /* Get data of the assignment from database */
      Asg_GetDataOfAssignmentByCod (&Asg);

      /* Get text of the assignment from database */
      Asg_DB_GetAssignmentTxtByCod (Asg.AsgCod,Txt);
     }

   /***** Begin form *****/
   if (ItsANewAssignment)
     {
      Frm_BeginForm (ActNewAsg);
      Assignments.AsgCodToEdit = -1L;
     }
   else
     {
      Frm_BeginForm (ActChgAsg);
      Assignments.AsgCodToEdit = Asg.AsgCod;
     }
   Asg_PutParams (&Assignments);

      /***** Begin box and table *****/
      if (ItsANewAssignment)
	 Box_BoxTableBegin (NULL,Txt_New_assignment,
			    NULL,NULL,
			    Hlp_ASSESSMENT_Assignments_new_assignment,Box_NOT_CLOSABLE,2);
      else
	 Box_BoxTableBegin (NULL,
			    Asg.Title[0] ? Asg.Title :
					   Txt_Edit_assignment,
			    NULL,NULL,
			    Hlp_ASSESSMENT_Assignments_edit_assignment,Box_NOT_CLOSABLE,2);


      /***** Assignment title *****/
      HTM_TR_Begin (NULL);

	 /* Label */
	 Frm_LabelColumn ("RM","Title",Txt_Title);

	 /* Data */
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("Title",Asg_MAX_CHARS_ASSIGNMENT_TITLE,Asg.Title,
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    "id=\"Title\""
			    " class=\"TITLE_DESCRIPTION_WIDTH %s\""
			    " required=\"required\"",
			    The_ClassInput[Gbl.Prefs.Theme]);
	 HTM_TD_End ();

      HTM_TR_End ();

      /***** Assignment start and end dates *****/
      Dat_PutFormStartEndClientLocalDateTimes (Asg.TimeUTC,
					       Dat_FORM_SECONDS_ON,
					       Gbl.Action.Act == ActFrmNewAsg ? SetHMSAllDay :
										SetHMSDontSet);

      /***** Send work? *****/
      HTM_TR_Begin (NULL);

	 /* Label */
	 Frm_LabelColumn ("RM","Folder",Txt_Upload_files_QUESTION);

	 /* Data */
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_LABEL_Begin ("class=\"%s\"",The_ClassDat[Gbl.Prefs.Theme]);
	       HTM_TxtColon (Txt_Folder);
	       HTM_INPUT_TEXT ("Folder",Brw_MAX_CHARS_FOLDER,Asg.Folder,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "id=\"Folder\" size=\"30\" class=\"%s\"",
			       The_ClassInput[Gbl.Prefs.Theme]);
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
				" class=\"TITLE_DESCRIPTION_WIDTH %s\"",
				The_ClassInput[Gbl.Prefs.Theme]);
	       if (!ItsANewAssignment)
		  HTM_Txt (Txt);
	    HTM_TEXTAREA_End ();
	 HTM_TD_End ();

      HTM_TR_End ();

      /***** Groups *****/
      Asg_ShowLstGrpsToEditAssignment (Asg.AsgCod);

      /***** End table, send button and end box *****/
      if (ItsANewAssignment)
	 Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_assignment);
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
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *Txt_Groups;
   extern const char *Txt_The_whole_course;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   if (Gbl.Crs.Grps.GrpTypes.NumGrpTypes)
     {
      /***** Begin box and table *****/
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
	    HTM_TxtColon (Txt_Groups);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"LT\"");

	    Box_BoxTableBegin ("100%",NULL,
			       NULL,NULL,
			       Hlp_USERS_Groups,Box_NOT_CLOSABLE,0);

	       /***** First row: checkbox to select the whole course *****/
	       HTM_TR_Begin (NULL);
		  HTM_TD_Begin ("colspan=\"7\" class=\"%s LM\"",
		                The_ClassDat[Gbl.Prefs.Theme]);
		     HTM_LABEL_Begin (NULL);
			HTM_INPUT_CHECKBOX ("WholeCrs",HTM_DONT_SUBMIT_ON_CHANGE,
					    "id=\"WholeCrs\" value=\"Y\"%s"
					    " onclick=\"uncheckChildren(this,'GrpCods')\"",
					    Grp_DB_CheckIfAssociatedToGrps ("asg_groups","AsgCod",AsgCod) ? "" :
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
   extern const char *Txt_You_must_specify_the_title_of_the_assignment;
   extern const char *Txt_Created_new_assignment_X;
   extern const char *Txt_The_assignment_has_been_modified;
   extern const char *Txt_You_can_not_disable_file_uploading_once_folders_have_been_created;
   struct Asg_Assignments Assignments;
   struct Asg_Assignment OldAsg;	// Current assigment data in database
   struct Asg_Assignment NewAsg;	// Assignment data received from form
   bool ItsANewAssignment;
   bool NewAssignmentIsCorrect = true;
   unsigned NumUsrsToBeNotifiedByEMail;
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParamAsgOrder ();
   Grp_GetParamWhichGroups ();
   Assignments.CurrentPage = Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Get the code of the assignment *****/
   NewAsg.AsgCod = Asg_GetParamAsgCod ();
   ItsANewAssignment = (NewAsg.AsgCod < 0);

   if (ItsANewAssignment)
     {
      /***** Reset old (current, not existing) assignment data *****/
      OldAsg.AsgCod = -1L;
      Asg_ResetAssignment (&OldAsg);
     }
   else
     {
      /***** Get data of the old (current) assignment from database *****/
      OldAsg.AsgCod = NewAsg.AsgCod;
      Asg_GetDataOfAssignmentByCod (&OldAsg);
     }

   /***** Get start/end date-times *****/
   NewAsg.TimeUTC[Dat_STR_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   NewAsg.TimeUTC[Dat_END_TIME] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /***** Get assignment title *****/
   Par_GetParToText ("Title",NewAsg.Title,Asg_MAX_BYTES_ASSIGNMENT_TITLE);

   /***** Get folder name where to send works of the assignment *****/
   Par_GetParToText ("Folder",NewAsg.Folder,Brw_MAX_BYTES_FOLDER);
   NewAsg.SendWork = (NewAsg.Folder[0]) ? Asg_SEND_WORK :
	                                  Asg_DO_NOT_SEND_WORK;

   /***** Get assignment text *****/
   Par_GetParToHTML ("Txt",Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewAsg.TimeUTC[Dat_STR_TIME] == 0)
      NewAsg.TimeUTC[Dat_STR_TIME] = Gbl.StartExecutionTimeUTC;
   if (NewAsg.TimeUTC[Dat_END_TIME] == 0)
      NewAsg.TimeUTC[Dat_END_TIME] = NewAsg.TimeUTC[Dat_STR_TIME] + 2 * 60 * 60;	// +2 hours

   /***** Check if title is correct *****/
   if (NewAsg.Title[0])	// If there's an assignment title
     {
      /* If title of assignment was in database... */
      if (Asg_DB_CheckIfSimilarAssignmentExists ("Title",NewAsg.Title,NewAsg.AsgCod))
        {
         NewAssignmentIsCorrect = false;

	 Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_an_assignment_with_the_title_X,
                        NewAsg.Title);
        }
      else	// Title is correct
        {
         if (NewAsg.SendWork == Asg_SEND_WORK)
           {
            if (Str_ConvertFilFolLnkNameToValid (NewAsg.Folder))	// If folder name is valid...
              {
               if (Asg_DB_CheckIfSimilarAssignmentExists ("Folder",NewAsg.Folder,NewAsg.AsgCod))	// If folder of assignment was in database...
                 {
                  NewAssignmentIsCorrect = false;

		  Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_an_assignment_with_the_folder_X,
                                 NewAsg.Folder);
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
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_title_of_the_assignment);
     }

   /***** Create a new assignment or update an existing one *****/
   if (NewAssignmentIsCorrect)
     {
      /* Get groups for this assignments */
      Grp_GetParCodsSeveralGrps ();

      if (ItsANewAssignment)
	{
         Asg_CreateAssignment (&NewAsg,Description);	// Add new assignment to database

	 /***** Write success message *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_assignment_X,
		        NewAsg.Title);
	}
      else
        {
         if (OldAsg.Folder[0] && NewAsg.Folder[0])
            if (strcmp (OldAsg.Folder,NewAsg.Folder))	// Folder name has changed
               NewAssignmentIsCorrect = Brw_UpdateFoldersAssigmentsIfExistForAllUsrs (OldAsg.Folder,NewAsg.Folder);
         if (NewAssignmentIsCorrect)
           {
            Asg_UpdateAssignment (&NewAsg,Description);

	    /***** Write success message *****/
	    Ale_ShowAlert (Ale_SUCCESS,Txt_The_assignment_has_been_modified);
           }
        }

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();

      /***** Notify by email about the new assignment *****/
      if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_ASSIGNMENT,NewAsg.AsgCod)))
	 Asg_DB_UpdateNumUsrsNotifiedByEMailAboutAssignment (NewAsg.AsgCod,
	                                                     NumUsrsToBeNotifiedByEMail);

      /***** Show all assignments again *****/
      Asg_ShowAllAssignments (&Assignments);
     }
   else
      // TODO: The form should be filled with partial data, now is always empty
      Asg_RequestCreatOrEditAsg ();
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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrps;
   unsigned NumGrp;

   /***** Get groups associated to an assignment from database *****/
   NumGrps = Asg_DB_GetGrps (&mysql_res,Asg->AsgCod);

   /***** Write heading *****/
   HTM_DIV_Begin ("class=\"%s\"",Asg->Hidden ? "ASG_GRP_LIGHT" :
        	                               "ASG_GRP");

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

unsigned Asg_GetNumAssignments (HieLvl_Level_t Scope,unsigned *NumNotif)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAssignments;

   /***** Default values *****/
   NumAssignments = 0;
   *NumNotif = 0;

   /***** Get number of assignments from database *****/
   if (Asg_DB_GetNumAssignments (&mysql_res,Scope))
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
   extern const char *The_ClassDat[The_NUM_THEMES];
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

	 HTM_TD_Begin ("class=\"%s RM\"",The_ClassDat[Gbl.Prefs.Theme]);
	    HTM_Unsigned (NumAssignments);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"%s RM\"",The_ClassDat[Gbl.Prefs.Theme]);
	    HTM_Unsigned (NumCoursesWithAssignments);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"%s RM\"",The_ClassDat[Gbl.Prefs.Theme]);
	    HTM_Double2Decimals (NumAssignmentsPerCourse);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"%s RM\"",The_ClassDat[Gbl.Prefs.Theme]);
	    HTM_Unsigned (NumNotif);
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
