// swad_assignment.c: assignments

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
#include "swad_role.h"
#include "swad_rubric_database.h"
#include "swad_rubric_type.h"
#include "swad_setting.h"
#include "swad_string.h"

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

typedef enum
  {
   Asg_ONE_ASSIGMENT,
   Asg_MULTIPLE_ASSIGMENTS
  } Asg_OneOrMultiple_t;

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Asg_PutHead (struct Asg_Assignments *Assignments,
                         Asg_OneOrMultiple_t OneOrMultiple,
                         Vie_ViewType_t ViewType);
static Usr_Can_t Asg_CheckIfICanCreateAssignments (void);
static void Asg_PutIconsListAssignments (void *Assignments);
static void Asg_PutIconToCreateNewAsg (void *Assignments);
static void Asg_ParsMyAllGrps (void *Assignments);
static void Asg_PutIconsOneAsg (void *Assignments);
static void Asg_ShowAssignmentRow (struct Asg_Assignments *Assignments,
                                   Asg_OneOrMultiple_t OneOrMultiple,
                                   Vie_ViewType_t ViewType);
static void Asg_WriteAsgAuthor (struct Asg_Assignment *Asg);
static void Asg_WriteAssignmentFolder (struct Asg_Assignment *Asg,
				       Vie_ViewType_t ViewType);

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

static void Asg_EditRubrics (long AsgRubCod);
static void Asg_ShowLstGrpsToEditAssignment (long AsgCod);

static void Asg_CreateAssignment (struct Asg_Assignment *Asg,const char *Txt);
static void Asg_UpdateAssignment (struct Asg_Assignment *Asg,const char *Txt);
static void Asg_CreateGroups (long AsgCod);
static void Asg_GetAndWriteNamesOfGrpsAssociatedToAsg (struct Asg_Assignment *Asg);
static Usr_Can_t Asg_CheckIfICanDoAsgBasedOnGroups (long AsgCod);

/*****************************************************************************/
/*************************** Reset assignments *******************************/
/*****************************************************************************/

void Asg_ResetAssignments (struct Asg_Assignments *Assignments)
  {
   Assignments->LstIsRead     = false;	// List is not read
   Assignments->Num           = 0;
   Assignments->LstAsgCods    = NULL;
   Assignments->SelectedOrder = Asg_ORDER_DEFAULT;
   Assignments->CurrentPage   = 0;
   Assignments->Asg.AsgCod    = -1L;	// Used as parameter in contextual links
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
   Gbl.Crs.Grps.MyAllGrps = Grp_GetParMyAllGrps ();
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
   Box_BoxBegin (Txt_Assignments,Asg_PutIconsListAssignments,Assignments,
                 Hlp_ASSESSMENT_Assignments,Box_NOT_CLOSABLE);

      /***** Select whether show only my groups or all groups *****/
      if (Gbl.Crs.Grps.NumGrps)
	{
	 Set_BeginSettingsHead ();
	    Grp_ShowFormToSelMyAllGrps (ActSeeAllAsg,
					Asg_ParsMyAllGrps,Assignments);
	 Set_EndSettingsHead ();
	}

      /***** Write links to pages *****/
      Pag_WriteLinksToPagesCentered (Pag_ASSIGNMENTS,&Pagination,
				     Assignments,-1L);

      if (Assignments->Num)
	{
	 /***** Begin table *****/
	 HTM_TABLE_Begin ("TBL_SCROLL");

	    /***** Table head *****/
	    Asg_PutHead (Assignments,Asg_MULTIPLE_ASSIGMENTS,Vie_VIEW);

	    /***** Write all assignments *****/
	    for (NumAsg  = Pagination.FirstItemVisible, The_ResetRowColor ();
		 NumAsg <= Pagination.LastItemVisible;
		 NumAsg++, The_ChangeRowColor ())
	      {
	       Assignments->Asg.AsgCod = Assignments->LstAsgCods[NumAsg - 1];
	       Asg_GetAssignmentDataByCod (&Assignments->Asg);
	       Asg_ShowAssignmentRow (Assignments,Asg_MULTIPLE_ASSIGMENTS,Vie_VIEW);
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
                         Asg_OneOrMultiple_t OneOrMultiple,
                         Vie_ViewType_t ViewType)
  {
   extern const char *Txt_START_END_TIME_HELP[Dat_NUM_START_END_TIME];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Assignment;
   extern const char *Txt_Folder;
   Dat_StartEndTime_t Order;
   Grp_MyAllGrps_t MyAllGrps;

   HTM_TR_Begin (NULL);

      if (OneOrMultiple == Asg_MULTIPLE_ASSIGMENTS)
         HTM_TH_Span (NULL,HTM_HEAD_CENTER,1,1,"CONTEXT_COL");	// Column for contextual icons

      for (Order  = (Dat_StartEndTime_t) 0;
	   Order <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   Order++)
	{
	 /* Begin head cell */
         HTM_TH_Begin (HTM_HEAD_LEFT);

	    if (ViewType == Vie_VIEW)
	      {
	       /* Begin form */
	       Frm_BeginForm (ActSeeAllAsg);
		  MyAllGrps = Grp_GetParMyAllGrps ();
		  Grp_PutParMyAllGrps (&MyAllGrps);
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

	    if (ViewType == Vie_VIEW)
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

static Usr_Can_t Asg_CheckIfICanCreateAssignments (void)
  {
   return (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
           Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM) ? Usr_CAN :
        					     Usr_CAN_NOT;
  }

/*****************************************************************************/
/*************** Put contextual icons in list of assignments *****************/
/*****************************************************************************/

static void Asg_PutIconsListAssignments (void *Assignments)
  {
   /***** Put icon to create a new assignment *****/
   if (Assignments)
     {
      if (Asg_CheckIfICanCreateAssignments () == Usr_CAN)
	 Asg_PutIconToCreateNewAsg (Assignments);

      /***** Link to get resource link *****/
      ((struct Asg_Assignments *) Assignments)->Asg.AsgCod = -1L;
      Ico_PutContextualIconToGetLink (ActReqLnkAsg,NULL,
				      Asg_PutPars,Assignments);
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

static void Asg_ParsMyAllGrps (void *Assignments)
  {
   if (Assignments)
     {
      Par_PutParOrder ((unsigned)
	               ((struct Asg_Assignments *) Assignments)->SelectedOrder);
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
   Grp_GetParMyAllGrps ();
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
   Lay_WriteHeaderClassPhoto (Vie_PRINT);

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (2);

      /***** Table head *****/
      Asg_PutHead (&Assignments,Asg_ONE_ASSIGMENT,Vie_PRINT);

      /***** Write assignment *****/
      Asg_ShowAssignmentRow (&Assignments,Asg_ONE_ASSIGMENT,Vie_PRINT);

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

   /***** Begin box *****/
   Box_BoxBegin (Assignments->Asg.Title[0] ? Assignments->Asg.Title :
					     Txt_Assignment,
		 Asg_PutIconsOneAsg,Assignments,
		 Hlp_ASSESSMENT_Assignments,Box_NOT_CLOSABLE);

      /***** Begin table *****/
      HTM_TABLE_Begin ("TBL_SCROLL");

	 /***** Table head *****/
	 Asg_PutHead (Assignments,Asg_ONE_ASSIGMENT,Vie_VIEW);

	 /***** Write assignment *****/
	 Asg_ShowAssignmentRow (Assignments,Asg_ONE_ASSIGMENT,Vie_VIEW);

      /***** End table *****/
      HTM_TABLE_End ();

   /***** End box *****/
   Box_BoxEnd ();
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
                                   Asg_OneOrMultiple_t OneOrMultiple,
                                   Vie_ViewType_t ViewType)
  {
   extern const char *CloOpe_Class[CloOpe_NUM_CLOSED_OPEN][HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_TitleClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];
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
      if (OneOrMultiple == Asg_MULTIPLE_ASSIGMENTS)
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
	 switch (ViewType)
	   {
	    case Vie_VIEW:
	       HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
			     Id,
			     CloOpe_Class[Assignments->Asg.ClosedOrOpen][Assignments->Asg.HiddenOrVisible],
			     The_GetSuffix (),
			     The_GetColorRows ());
	       break;
	    case Vie_PRINT:
	       HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s\"",
			     Id,
			     CloOpe_Class[Assignments->Asg.ClosedOrOpen][Assignments->Asg.HiddenOrVisible],
			     The_GetSuffix ());
	       break;
	    default:
	       Err_WrongTypeExit ();
	       break;
	   }
	 Dat_WriteLocalDateHMSFromUTC (Id,Assignments->Asg.TimeUTC[StartEndTime],
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       Dat_WRITE_TODAY |
				       Dat_WRITE_DATE_ON_SAME_DAY |
				       Dat_WRITE_WEEK_DAY |
				       Dat_WRITE_HOUR |
				       Dat_WRITE_MINUTE |
				       Dat_WRITE_SECOND);
	 HTM_TD_End ();
	 free (Id);
	}

      /* Assignment title */
      switch (ViewType)
	{
	 case Vie_VIEW:
	    HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
	    break;
	 case Vie_PRINT:
	    HTM_TD_Begin ("class=\"LT\"");
	    break;
	 default:
	    Err_WrongTypeExit ();
	    break;
	}

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
      switch (ViewType)
	{
	 case Vie_VIEW:
	    HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
			  The_GetSuffix (),The_GetColorRows ());
	    break;
	 case Vie_PRINT:
	    HTM_TD_Begin ("class=\"LT DAT_%s\"",
			  The_GetSuffix ());
	    break;
	 default:
	    Err_WrongTypeExit ();
	    break;
	}

      if (Assignments->Asg.SendWork == Asg_SEND_WORK)
	 Asg_WriteAssignmentFolder (&Assignments->Asg,ViewType);
      HTM_TD_End ();

   HTM_TR_End ();

   /***** Write second row of data of this assignment *****/
   HTM_TR_Begin (NULL);

      /* Author of the assignment */
      switch (ViewType)
	{
	 case Vie_VIEW:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",The_GetColorRows ());
	    break;
	 case Vie_PRINT:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
	    break;
	 default:
	    Err_WrongTypeExit ();
	    break;
	}
      Asg_WriteAsgAuthor (&Assignments->Asg);
      HTM_TD_End ();

      /* Text of the assignment */
      Asg_DB_GetAssignmentTxt (Assignments->Asg.AsgCod,Txt);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			Txt,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
      ALn_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
      switch (ViewType)
	{
	 case Vie_VIEW:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",The_GetColorRows ());
	    break;
	 case Vie_PRINT:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
	    break;
	 default:
	    Err_WrongTypeExit ();
	    break;
	}
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

static void Asg_WriteAssignmentFolder (struct Asg_Assignment *Asg,
				       Vie_ViewType_t ViewType)
  {
   extern const char *Txt_Folder;
   Act_Action_t NextAction;
   bool ICanSendFiles = Asg->HiddenOrVisible == HidVis_VISIBLE &&	// It's visible (not hidden)
                        Asg->ClosedOrOpen == CloOpe_OPEN &&		// It's open (inside dates)
                        Asg->ICanDo == Usr_CAN;			// I can do (I belong to course/group)

   /***** Folder icon *****/
   if (ViewType == Vie_VIEW &&	// Not print view
       ICanSendFiles)		// I can send files to this assignment folder
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
	 Ico_PutContextualIconToGetLink (ActReqLnkAsg,NULL,
					 Asg_PutPars,Assignments);
	 /* falls through */
	 /* no break */
      case Rol_STD:
      case Rol_NET:
	 /***** Icon to print assignment *****/
	 Ico_PutContextualIconToPrint (ActPrnOneAsg,Asg_PutPars,Assignments);
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
   Grp_MyAllGrps_t MyAllGrps;

   if (Assignments)
     {
      ParCod_PutPar (ParCod_Asg,((struct Asg_Assignments *) Assignments)->Asg.AsgCod);
      Par_PutParOrder ((unsigned) ((struct Asg_Assignments *) Assignments)->SelectedOrder);
      MyAllGrps = Grp_GetParMyAllGrps ();
      Grp_PutParMyAllGrps (&MyAllGrps);
      Pag_PutParPagNum (Pag_ASSIGNMENTS,((struct Asg_Assignments *) Assignments)->CurrentPage);
     }
  }

/*****************************************************************************/
/*************************** List all assignments ****************************/
/*****************************************************************************/

static void Asg_GetListAssignments (struct Asg_Assignments *Assignments)
  {
   extern unsigned (*Asg_DB_GetListAssignments[Grp_NUM_MY_ALL_GROUPS]) (MYSQL_RES **mysql_res,
                                                                       Dat_StartEndTime_t SelectedOrder);
   MYSQL_RES *mysql_res;
   unsigned NumAsg;

   if (Assignments->LstIsRead)
      Asg_FreeListAssignments (Assignments);

   /***** Get list of assignments from database *****/
   Assignments->Num = Asg_DB_GetListAssignments[Gbl.Crs.Grps.MyAllGrps] (&mysql_res,Assignments->SelectedOrder);
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
      Asg->HiddenOrVisible = HidVid_GetHiddenOrVisible (row[1][0]);

      /* Get author of the assignment (row[2]) */
      Asg->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get start and end dates (row[3] and row[4] hold the start UTC time) */
      Asg->TimeUTC[Dat_STR_TIME] = Dat_GetUNIXTimeFromStr (row[3]);
      Asg->TimeUTC[Dat_END_TIME] = Dat_GetUNIXTimeFromStr (row[4]);

      /* Get whether the assignment is open or closed (row(5)) */
      Asg->ClosedOrOpen = CloOpe_GetClosedOrOpenFrom01 (row[5][0]);

      /* Get the title (row[6]) and the folder (row[7]) of the assignment  */
      Str_Copy (Asg->Title ,row[6],sizeof (Asg->Title ) - 1);
      Str_Copy (Asg->Folder,row[7],sizeof (Asg->Folder) - 1);
      Asg->SendWork = (Asg->Folder[0] != '\0');

      /* Can I do this assignment? */
      Asg->ICanDo = Asg_CheckIfICanDoAsgBasedOnGroups (Asg->AsgCod);
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
   Asg->HiddenOrVisible	      = HidVis_VISIBLE;
   Asg->UsrCod		      = -1L;
   Asg->TimeUTC[Dat_STR_TIME] =
   Asg->TimeUTC[Dat_END_TIME] = (time_t) 0;
   Asg->ClosedOrOpen	      = CloOpe_CLOSED;
   Asg->Title[0]	      = '\0';
   Asg->SendWork	      = Asg_DONT_SEND_WORK;
   Asg->Folder[0]	      = '\0';
   Asg->RubCod		      = -1L;
   Asg->ICanDo		      = Usr_CAN_NOT;
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
                             long AsgCod,Ntf_GetContent_t GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Query database. Result should have a unique row *****/
   if (Asg_DB_GetAssignmentTitleAndTxt (&mysql_res,AsgCod) == 1)
     {
      /***** Get row *****/
      row = mysql_fetch_row (mysql_res);

      /***** Get summary *****/
      Str_Copy (SummaryStr,row[0],Ntf_MAX_BYTES_SUMMARY);

      /***** Get content *****/
      if (GetContent == Ntf_GET_CONTENT)
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
   Gbl.Crs.Grps.MyAllGrps = Grp_GetParMyAllGrps ();
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
   Gbl.Crs.Grps.MyAllGrps = Grp_GetParMyAllGrps ();
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
   Gbl.Crs.Grps.MyAllGrps = Grp_GetParMyAllGrps ();
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
   extern const char *Folder_to_upload_files;
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
   Grp_GetParMyAllGrps ();
   Assignments.CurrentPage = Pag_GetParPagNum (Pag_ASSIGNMENTS);

   /***** Get the code of the assignment *****/
   ItsANewAssignment = ((Assignments.Asg.AsgCod = ParCod_GetPar (ParCod_Asg)) <= 0);

   /***** Get from the database the data of the assignment *****/
   if (ItsANewAssignment)
     {
      /* Initialize to empty assignment */
      Assignments.Asg.AsgCod		    = -1L;
      Assignments.Asg.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
      Assignments.Asg.TimeUTC[Dat_END_TIME] = Assignments.Asg.TimeUTC[Dat_STR_TIME] + (2 * 60 * 60);	// +2 hours
      Assignments.Asg.ClosedOrOpen	    = CloOpe_OPEN;
      Assignments.Asg.Title[0]		    = '\0';
      Assignments.Asg.SendWork		    = Asg_DONT_SEND_WORK;
      Assignments.Asg.Folder[0]		    = '\0';
      Assignments.Asg.RubCod		    = -1L;
      Assignments.Asg.ICanDo		    = Usr_CAN_NOT;
     }
   else
     {
      /* Get data of the assignment from database */
      Asg_GetAssignmentDataByCod (&Assignments.Asg);

      /* Get text of the assignment from database */
      Asg_DB_GetAssignmentTxt (Assignments.Asg.AsgCod,Txt);

      /* Get rubric associated to the assignment from database */
      Assignments.Asg.RubCod = Asg_DB_GetAssignmentRubCod (Assignments.Asg.AsgCod);
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
      Box_BoxTableBegin (Assignments.Asg.Title[0] ? Assignments.Asg.Title :
						    Txt_Assignment,
			 NULL,NULL,
			 Hlp_ASSESSMENT_Assignments_edit_assignment,Box_NOT_CLOSABLE,2);

	 /***** Assignment title *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RM","Title",Txt_Title);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LM\"");
	       HTM_INPUT_TEXT ("Title",Asg_MAX_CHARS_ASSIGNMENT_TITLE,Assignments.Asg.Title,
			       HTM_REQUIRED,
			       "id=\"Title\" class=\"Frm_C2_INPUT INPUT_%s\"",
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
	    Frm_LabelColumn ("Frm_C1 RM","Folder",Folder_to_upload_files);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LM\"");
	       HTM_LABEL_Begin ("class=\"DAT_%s\"",The_GetSuffix ());
		  HTM_INPUT_TEXT ("Folder",Brw_MAX_CHARS_FOLDER,Assignments.Asg.Folder,
				  HTM_NO_ATTR,
				  "id=\"Folder\" class=\"Frm_C2_INPUT INPUT_%s\"",
				  The_GetSuffix ());
	       HTM_LABEL_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Assignment text *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","Txt",Txt_Description);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	       HTM_TEXTAREA_Begin (HTM_NO_ATTR,
				   "id=\"Txt\" name=\"Txt\" rows=\"10\""
				   " class=\"Frm_C2_INPUT INPUT_%s\"",
				   The_GetSuffix ());
		  if (!ItsANewAssignment)
		     HTM_Txt (Txt);
	       HTM_TEXTAREA_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Rubrics *****/
	 if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)	// TODO: Remove when finished
	    Asg_EditRubrics (Assignments.Asg.RubCod);

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
   HTM_BR ();
   Asg_ShowAllAssignments (&Assignments);
  }

/*****************************************************************************/
/***************** Edit rubrics associated to an assignment ******************/
/*****************************************************************************/

static void Asg_EditRubrics (long AsgRubCod)
  {
   extern const char *Par_CodeStr[Par_NUM_PAR_COD];
   extern const char *Txt_Rubric;
   extern const char *Txt_no_rubric;
   MYSQL_RES *mysql_res;
   unsigned NumRubrics;
   unsigned NumRub;
   long RubCod;
   char Title[Rub_MAX_BYTES_TITLE + 1];

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT","",Txt_Rubric);

      /* Rubrics */
      HTM_TD_Begin ("class=\"Frm_C2 LT\"");

	 /* Get rubrics for current course from database */
	 NumRubrics = Rub_DB_GetListRubrics (&mysql_res);

	 HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
			   "name=\"%s\" class=\"PrjCfg_RUBRIC_SEL\"",
			   Par_CodeStr[ParCod_Rub]);

	    /* First option to indicate that no rubric is selected */
	    HTM_OPTION (HTM_Type_STRING,"-1",
			HTM_NO_ATTR,
			"[%s]",Txt_no_rubric);

	    /* One selector for each rubric */
	    for (NumRub = 0;
		 NumRub < NumRubrics;
		 NumRub++)
	      {
	       RubCod = DB_GetNextCode (mysql_res);

	       /* One option for each rubric in this course */
	       Rub_DB_GetRubricTitle (RubCod,Title,Rub_MAX_BYTES_TITLE);
	       HTM_OPTION (HTM_Type_LONG,&RubCod,
			   (RubCod == AsgRubCod) ? HTM_SELECTED :
						   HTM_NO_ATTR,
			   "%s",Title);
	      }

	 HTM_SELECT_End ();

	 /* Free structure that stores the query result */
	 DB_FreeMySQLResult (&mysql_res);

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Show list of groups to edit and assignment *****************/
/*****************************************************************************/

static void Asg_ShowLstGrpsToEditAssignment (long AsgCod)
  {
   extern const char *Txt_Groups;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   if (Gbl.Crs.Grps.GrpTypes.NumGrpTypes)
     {
      /***** Begin row *****/
      HTM_TR_Begin (NULL);

         /* Label */
	 Frm_LabelColumn ("Frm_C1 RT","",Txt_Groups);

	 /* Groups */
	 HTM_TD_Begin ("class=\"Frm_C2 LT\"");

	    /***** First row: checkbox to select the whole course *****/
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_CHECKBOX ("WholeCrs",
				   Grp_DB_CheckIfAssociatedToGrps ("asg_groups",
								   "AsgCod",
								   AsgCod) ? HTM_NO_ATTR :
									     HTM_CHECKED,
				   "id=\"WholeCrs\" value=\"Y\""
				   " onclick=\"uncheckChildren(this,'GrpCods')\"");
	       Grp_WriteTheWholeCourse ();
	    HTM_LABEL_End ();

	    /***** List the groups for each group type *****/
	    Grp_ListGrpsToEditAsgAttSvyEvtMch (Grp_ASSIGNMENT,AsgCod);

	 HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/****************** Receive form to create a new assignment ******************/
/*****************************************************************************/

void Asg_ReceiveAssignment (void)
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
   long NewRubCod;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParAsgOrder ();
   Grp_GetParMyAllGrps ();
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
   Assignments.Asg.SendWork = Assignments.Asg.Folder[0] ? Asg_SEND_WORK :
							  Asg_DONT_SEND_WORK;

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
      /* Check if current rubric code must be changed */
      OldAsg.RubCod = Asg_DB_GetAssignmentRubCod (OldAsg.AsgCod);
      NewRubCod = ParCod_GetPar (ParCod_Rub);
      if (NewRubCod != Assignments.Asg.RubCod)
         Asg_DB_UpdateRubCod (OldAsg.AsgCod,NewRubCod);

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
	    HTM_TxtF ("%s %s",row[0],row[1]);

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
	 Grp_WriteTheWholeCourse ();

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
/*********************** Check if I can do an assignment *********************/
/*****************************************************************************/

static Usr_Can_t Asg_CheckIfICanDoAsgBasedOnGroups (long AsgCod)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 // Students and teachers can do assignments depending on groups
	 /***** Get if I can do an assignment from database *****/
	 return Asg_DB_CheckIfICanDoAsgBasedOnGroups (AsgCod);
      case Rol_SYS_ADM:
         return Usr_CAN;
      default:
         return Usr_CAN_NOT;
     }
  }

/*****************************************************************************/
/************ Write start and end dates of a folder of assignment ************/
/*****************************************************************************/

void Asg_WriteDatesAssignment (const struct Asg_Assignment *Asg)
  {
   extern const char *Txt_unknown_assignment;
   static const char *DateClass[CloOpe_NUM_CLOSED_OPEN] =
     {
      [CloOpe_CLOSED] = "ASG_LST_DATE_RED",
      [CloOpe_OPEN  ] = "ASG_LST_DATE_GREEN",
     };
   static unsigned UniqueId = 0;
   char *Id;

   /***** Begin table cell *****/
   HTM_TD_Begin ("colspan=\"2\" class=\"RM %s_%s %s\"",
	         DateClass[Asg->ClosedOrOpen],The_GetSuffix (),
	         The_GetColorRows ());

      if (Asg->AsgCod > 0)
	{
	 UniqueId++;

	 /***** Write start date *****/
	 if (asprintf (&Id,"asg_start_date_%u",UniqueId) < 0)
	    Err_NotEnoughMemoryExit ();
	 HTM_SPAN_Begin ("id=\"%s\"",Id);
	    Dat_WriteLocalDateHMSFromUTC (Id,Asg->TimeUTC[Dat_STR_TIME],
					  Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				          Dat_WRITE_TODAY |
				          Dat_WRITE_DATE_ON_SAME_DAY |
				          Dat_WRITE_HOUR |
				          Dat_WRITE_MINUTE |
				          Dat_WRITE_SECOND);
	 HTM_SPAN_End ();
	 free (Id);

	 /***** Arrow *****/
	 HTM_Txt ("&rarr;");

	 /***** Write end date *****/
	 if (asprintf (&Id,"asg_end_date_%u",UniqueId) < 0)
	    Err_NotEnoughMemoryExit ();
	 HTM_SPAN_Begin ("id=\"%s\"",Id);
	    Dat_WriteLocalDateHMSFromUTC (Id,Asg->TimeUTC[Dat_END_TIME],
					  Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
					  Dat_WRITE_TODAY |
					  Dat_WRITE_DATE_ON_SAME_DAY |
					  Dat_WRITE_HOUR |
					  Dat_WRITE_MINUTE |
					  Dat_WRITE_SECOND);
	 HTM_SPAN_End ();
	 free (Id);
	}
      else
	 HTM_TxtF ("&nbsp;(%s)",Txt_unknown_assignment);

   /***** End table cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/* Check if I have permission to create a file or folder into an assignment **/
/*****************************************************************************/

Usr_Can_t Asg_CheckIfICanCreateIntoAssigment (const struct Asg_Assignment *Asg)
  {
   static Usr_Can_t CloOpe_ICanCreate[CloOpe_NUM_CLOSED_OPEN] =
     {
      [CloOpe_CLOSED] = Usr_CAN_NOT,
      [CloOpe_OPEN  ] = Usr_CAN,
     };

   /***** Trivial check 1: assignment is valid *****/
   if (Asg->AsgCod <= 0)
      return Usr_CAN_NOT;

   /***** Check 2: Do not create anything in hidden assigments *****/
   if (Asg->HiddenOrVisible == HidVis_HIDDEN)
      return Usr_CAN_NOT;

   /***** Check 3: If I do not belong to course / groups of this assignment,
		   I can not create anything inside this assignment *****/
   if (Asg->ICanDo == Usr_CAN_NOT)
      return Usr_CAN_NOT;

   /***** Check 4: Depending on my role in this course... *****/
   switch (Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs)
     {
      case Rol_STD:		// Students...
      case Rol_NET:		// ...and non-editing teachers...
				// ...can create only inside open assignments
	 return CloOpe_ICanCreate[Asg->ClosedOrOpen];
      case Rol_TCH:		// Teachers...
	 return Usr_CAN;	// ...can create inside any assignment
      default:
	 return Usr_CAN_NOT;
     }
  }

/*****************************************************************************/
/*************************** Set assignment folder ***************************/
/*****************************************************************************/

void Asg_SetFolder (struct Asg_Assignment *Asg,unsigned Level)
  {
   const char *Ptr;
   unsigned i;

   if (Level == 1)
      // We are in this case: assignments/assignment-folder
      Str_Copy (Asg->Folder,Gbl.FileBrowser.FilFolLnk.Name,
		sizeof (Asg->Folder) - 1);
   else
     {
      // We are in this case: assignments/assignment-folder/rest-of-path
      for (Ptr = Gbl.FileBrowser.FilFolLnk.Path;
	   *Ptr && *Ptr != '/';
	   Ptr++);	// Go to first '/'
      if (*Ptr == '/')
	 Ptr++;	// Skip '/'
      for (i = 0;
	   i < Brw_MAX_BYTES_FOLDER && *Ptr && *Ptr != '/';
	   i++, Ptr++)
	 Asg->Folder[i] = *Ptr;	// Copy assignment folder
      Asg->Folder[i] = '\0';
     }
  }

/*****************************************************************************/
/************************ Get number of assignments **************************/
/*****************************************************************************/
// Returns the number of assignments
// in this location (all the platform, the current degree or the current course)

unsigned Asg_GetNumAssignments (Hie_Level_t Level,unsigned *NumNotif)
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
   unsigned NumCoursesWithAssignments;

   /***** Get the number of assignments from this location *****/
   if ((NumAssignments = Asg_GetNumAssignments (Gbl.Scope.Current,&NumNotif)))
      NumCoursesWithAssignments = Asg_DB_GetNumCoursesWithAssignments (Gbl.Scope.Current);
   else
      NumCoursesWithAssignments = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_ASSIGNMENTS],NULL,NULL,
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
	 HTM_TD_Unsigned (NumAssignments);
	 HTM_TD_Unsigned (NumCoursesWithAssignments);
	 HTM_TD_Ratio (NumAssignments,NumCoursesWithAssignments);
	 HTM_TD_Unsigned (NumNotif);
      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
