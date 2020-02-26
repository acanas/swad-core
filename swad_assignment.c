// swad_assignment.c: assignments

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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
#include "swad_box.h"
#include "swad_database.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_HTML.h"
#include "swad_notification.h"
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
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Asg_ShowAllAssignments (void);
static void Asg_PutHeadForSeeing (bool PrintView);
static bool Asg_CheckIfICanCreateAssignments (void);
static void Asg_PutIconsListAssignments (void);
static void Asg_PutIconToCreateNewAsg (void);
static void Asg_PutButtonToCreateNewAsg (void);
static void Asg_ParamsWhichGroupsToShow (void);
static void Asg_ShowOneAssignment (long AsgCod,bool PrintView);
static void Asg_WriteAsgAuthor (struct Assignment *Asg);
static void Asg_WriteAssignmentFolder (struct Assignment *Asg,bool PrintView);
static void Asg_GetParamAsgOrder (void);

static void Asg_PutFormsToRemEditOneAsg (const struct Assignment *Asg,
                                         const char *Anchor);
static void Asg_PutParams (void);
static void Asg_GetDataOfAssignment (struct Assignment *Asg,
                                     MYSQL_RES **mysql_res,
				     unsigned long NumRows);
static void Asg_ResetAssignment (struct Assignment *Asg);
static void Asg_GetAssignmentTxtFromDB (long AsgCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);
static void Asg_PutParamAsgCod (long AsgCod);
static bool Asg_CheckIfSimilarAssignmentExists (const char *Field,const char *Value,long AsgCod);
static void Asg_ShowLstGrpsToEditAssignment (long AsgCod);
static void Asg_UpdateNumUsrsNotifiedByEMailAboutAssignment (long AsgCod,unsigned NumUsrsToBeNotifiedByEMail);
static void Asg_CreateAssignment (struct Assignment *Asg,const char *Txt);
static void Asg_UpdateAssignment (struct Assignment *Asg,const char *Txt);
static bool Asg_CheckIfAsgIsAssociatedToGrps (long AsgCod);
static void Asg_RemoveAllTheGrpsAssociatedToAnAssignment (long AsgCod);
static void Asg_CreateGrps (long AsgCod);
static void Asg_GetAndWriteNamesOfGrpsAssociatedToAsg (struct Assignment *Asg);
static bool Asg_CheckIfIBelongToCrsOrGrpsThisAssignment (long AsgCod);

/*****************************************************************************/
/************************ List all the assignments ***************************/
/*****************************************************************************/

void Asg_SeeAssignments (void)
  {
   /***** Get parameters *****/
   Asg_GetParamAsgOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Asgs.CurrentPage = Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Show all the assignments *****/
   Asg_ShowAllAssignments ();
  }

/*****************************************************************************/
/************************ Show all the assignments ***************************/
/*****************************************************************************/

static void Asg_ShowAllAssignments (void)
  {
   extern const char *Hlp_ASSESSMENT_Assignments;
   extern const char *Txt_Assignments;
   extern const char *Txt_No_assignments;
   struct Pagination Pagination;
   unsigned NumAsg;

   /***** Get list of assignments *****/
   Asg_GetListAssignments ();

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Gbl.Asgs.Num;
   Pagination.CurrentPage = (int) Gbl.Asgs.CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Gbl.Asgs.CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Begin box *****/
   Box_BoxBegin ("100%",Txt_Assignments,Asg_PutIconsListAssignments,
                 Hlp_ASSESSMENT_Assignments,Box_NOT_CLOSABLE);

   /***** Select whether show only my groups or all groups *****/
   if (Gbl.Crs.Grps.NumGrps)
     {
      Set_StartSettingsHead ();
      Grp_ShowFormToSelWhichGrps (ActSeeAsg,Asg_ParamsWhichGroupsToShow);
      Set_EndSettingsHead ();
     }

   /***** Write links to pages *****/
   Pag_WriteLinksToPagesCentered (Pag_ASSIGNMENTS,
				  &Pagination,
				  0);

   if (Gbl.Asgs.Num)
     {
      /***** Table head *****/
      HTM_TABLE_BeginWideMarginPadding (2);
      Asg_PutHeadForSeeing (false);	// Not print view

      /***** Write all the assignments *****/
      for (NumAsg = Pagination.FirstItemVisible;
	   NumAsg <= Pagination.LastItemVisible;
	   NumAsg++)
	 Asg_ShowOneAssignment (Gbl.Asgs.LstAsgCods[NumAsg - 1],
	                        false);	// Not print view

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No assignments created
      Ale_ShowAlert (Ale_INFO,Txt_No_assignments);

   /***** Write again links to pages *****/
   Pag_WriteLinksToPagesCentered (Pag_ASSIGNMENTS,
				  &Pagination,
				  0);

   /***** Button to create a new assignment *****/
   if (Asg_CheckIfICanCreateAssignments ())
      Asg_PutButtonToCreateNewAsg ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of assignments *****/
   Asg_FreeListAssignments ();
  }

/*****************************************************************************/
/***************** Write header with fields of an assignment *****************/
/*****************************************************************************/

static void Asg_PutHeadForSeeing (bool PrintView)
  {
   extern const char *Txt_START_END_TIME_HELP[Dat_NUM_START_END_TIME];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Assignment;
   extern const char *Txt_Folder;
   Dat_StartEndTime_t Order;

   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"CONTEXT_COL",NULL);	// Column for contextual icons
   for (Order = Dat_START_TIME;
	Order <= Dat_END_TIME;
	Order++)
     {
      HTM_TH_Begin (1,1,"LM");

      if (!PrintView)
	{
	 Frm_StartForm (ActSeeAsg);
	 Grp_PutParamWhichGrps ();
	 Pag_PutHiddenParamPagNum (Pag_ASSIGNMENTS,Gbl.Asgs.CurrentPage);
	 Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
	 HTM_BUTTON_SUBMIT_Begin (Txt_START_END_TIME_HELP[Order],"BT_LINK TIT_TBL",NULL);
	 if (Order == Gbl.Asgs.SelectedOrder)
	    HTM_U_Begin ();
	}
      HTM_Txt (Txt_START_END_TIME[Order]);
      if (!PrintView)
	{
	 if (Order == Gbl.Asgs.SelectedOrder)
	    HTM_U_End ();
	 HTM_BUTTON_End ();
	 Frm_EndForm ();
	}

      HTM_TH_End ();
     }
   HTM_TH (1,1,"LM",Txt_Assignment);
   HTM_TH (1,1,"LM",Txt_Folder);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Check if I can create assignments **********************/
/*****************************************************************************/

static bool Asg_CheckIfICanCreateAssignments (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
                  Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
  }

/*****************************************************************************/
/*************** Put contextual icons in list of assignments *****************/
/*****************************************************************************/

static void Asg_PutIconsListAssignments (void)
  {
   /***** Put icon to create a new assignment *****/
   if (Asg_CheckIfICanCreateAssignments ())
      Asg_PutIconToCreateNewAsg ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_ASSIGNMENTS;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/******************* Put icon to create a new assignment *********************/
/*****************************************************************************/

static void Asg_PutIconToCreateNewAsg (void)
  {
   extern const char *Txt_New_assignment;

   /***** Put form to create a new assignment *****/
   Gbl.Asgs.AsgCodToEdit = -1L;
   Ico_PutContextualIconToAdd (ActFrmNewAsg,NULL,Asg_PutParams,
			       Txt_New_assignment);
  }

/*****************************************************************************/
/****************** Put button to create a new assignment ********************/
/*****************************************************************************/

static void Asg_PutButtonToCreateNewAsg (void)
  {
   extern const char *Txt_New_assignment;

   Gbl.Asgs.AsgCodToEdit = -1L;
   Frm_StartForm (ActFrmNewAsg);
   Asg_PutParams ();
   Btn_PutConfirmButton (Txt_New_assignment);
   Frm_EndForm ();
  }

/*****************************************************************************/
/**************** Put params to select which groups to show ******************/
/*****************************************************************************/

static void Asg_ParamsWhichGroupsToShow (void)
  {
   Asg_PutHiddenParamAsgOrder ();
   Pag_PutHiddenParamPagNum (Pag_ASSIGNMENTS,Gbl.Asgs.CurrentPage);
  }

/*****************************************************************************/
/******************** Show print view of one assignment **********************/
/*****************************************************************************/

void Asg_PrintOneAssignment (void)
  {
   long AsgCod;

   /***** Get the code of the assignment *****/
   AsgCod = Asg_GetParamAsgCod ();

   /***** Write header *****/
   Lay_WriteHeaderClassPhoto (true,false,
			      Gbl.Hierarchy.Ins.InsCod,
			      Gbl.Hierarchy.Deg.DegCod,
			      Gbl.Hierarchy.Crs.CrsCod);

   /***** Table head *****/
   HTM_TABLE_BeginWideMarginPadding (2);
   Asg_PutHeadForSeeing (true);		// Print view

   /***** Write assignment *****/
   Asg_ShowOneAssignment (AsgCod,
                          true);	// Print view

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/*************************** Show one assignment *****************************/
/*****************************************************************************/

static void Asg_ShowOneAssignment (long AsgCod,bool PrintView)
  {
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   char *Id;
   struct Assignment Asg;
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
      HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL COLOR%u\"",Gbl.RowEvenOdd);
      Asg_PutFormsToRemEditOneAsg (&Asg,Anchor);
     }
   HTM_TD_End ();

   /* Start/end date/time */
   UniqueId++;

   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      if (asprintf (&Id,"asg_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      if (PrintView)
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LB\"",
		       Id,
		       Asg.Hidden ? (Asg.Open ? "DATE_GREEN_LIGHT" :
					        "DATE_RED_LIGHT") :
				    (Asg.Open ? "DATE_GREEN" :
					        "DATE_RED"));
      else
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LB COLOR%u\"",
		       Id,
		       Asg.Hidden ? (Asg.Open ? "DATE_GREEN_LIGHT" :
					        "DATE_RED_LIGHT") :
				    (Asg.Open ? "DATE_GREEN" :
					        "DATE_RED"),
		       Gbl.RowEvenOdd);
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
      HTM_TD_Begin ("class=\"%s LT COLOR%u\"",
		    Asg.Hidden ? "ASG_TITLE_LIGHT" :
				 "ASG_TITLE",
		    Gbl.RowEvenOdd);
   HTM_ARTICLE_Begin (Anchor);
   HTM_Txt (Asg.Title);
   HTM_ARTICLE_End ();
   HTM_TD_End ();

   /* Assignment folder */
   if (PrintView)
      HTM_TD_Begin ("class=\"DAT LT\"");
   else
      HTM_TD_Begin ("class=\"DAT LT COLOR%u\"",Gbl.RowEvenOdd);
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
      HTM_TD_Begin ("colspan=\"2\" class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   Asg_WriteAsgAuthor (&Asg);
   HTM_TD_End ();

   /* Text of the assignment */
   Asg_GetAssignmentTxtFromDB (Asg.AsgCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   if (PrintView)
      HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
   else
      HTM_TD_Begin ("colspan=\"2\" class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   if (Gbl.Crs.Grps.NumGrps)
      Asg_GetAndWriteNamesOfGrpsAssociatedToAsg (&Asg);
   HTM_DIV_Begin ("class=\"PAR %s\"",Asg.Hidden ? "DAT_LIGHT" :
        	                                  "DAT");
   HTM_Txt (Txt);
   HTM_DIV_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

   /***** Mark possible notification as seen *****/
   Ntf_MarkNotifAsSeen (Ntf_EVENT_ASSIGNMENT,
	                AsgCod,Gbl.Hierarchy.Crs.CrsCod,
	                Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************* Write the author of an assignment *********************/
/*****************************************************************************/

static void Asg_WriteAsgAuthor (struct Assignment *Asg)
  {
   Usr_WriteAuthor1Line (Asg->UsrCod,Asg->Hidden);
  }

/*****************************************************************************/
/********************* Write the folder of an assignment *********************/
/*****************************************************************************/

static void Asg_WriteAssignmentFolder (struct Assignment *Asg,bool PrintView)
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
	    Frm_StartForm (ActFrmCreAsgUsr);
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    Gbl.FileBrowser.Type = Brw_ADMI_ASG_CRS;	// Course assignments
	    Str_Copy (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod,Gbl.Usrs.Me.UsrDat.EncryptedUsrCod,
		      Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
	    Usr_CreateListSelectedUsrsCodsAndFillWithOtherUsr (&Gbl.Usrs.Selected);
	    Frm_StartForm (ActFrmCreAsgCrs);
	    break;
	 default:
            Rol_WrongRoleExit ();
	    break;
        }
      Str_Copy (Gbl.FileBrowser.FilFolLnk.Path,Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
   	        PATH_MAX);
      Str_Copy (Gbl.FileBrowser.FilFolLnk.Name,Asg->Folder,
   	        NAME_MAX);
      Gbl.FileBrowser.FilFolLnk.Type = Brw_IS_FOLDER;
      Brw_PutImplicitParamsFileBrowser ();
      Ico_PutIconLink ("folder-open-yellow-plus.png",
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
      Ico_PutIconOff (ICanSendFiles ? "folder-open-green.svg" :
				      "folder-red.svg",
		      Txt_Folder);

   /***** Folder name *****/
   HTM_Txt (Asg->Folder);
  }

/*****************************************************************************/
/******* Get parameter with the type or order in list of assignments *********/
/*****************************************************************************/

static void Asg_GetParamAsgOrder (void)
  {
   Gbl.Asgs.SelectedOrder = (Dat_StartEndTime_t)
	                    Par_GetParToUnsignedLong ("Order",
                                                      0,
                                                      Dat_NUM_START_END_TIME - 1,
                                                      (unsigned long) Asg_ORDER_DEFAULT);
  }

/*****************************************************************************/
/*** Put a hidden parameter with the type of order in list of assignments ****/
/*****************************************************************************/

void Asg_PutHiddenParamAsgOrder (void)
  {
   Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Gbl.Asgs.SelectedOrder);
  }

/*****************************************************************************/
/***************** Put a link (form) to edit one assignment ******************/
/*****************************************************************************/

static void Asg_PutFormsToRemEditOneAsg (const struct Assignment *Asg,
                                         const char *Anchor)
  {
   Gbl.Asgs.AsgCodToEdit = Asg->AsgCod;	// Used as parameter in contextual links

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Put form to remove assignment *****/
	 Ico_PutContextualIconToRemove (ActReqRemAsg,Asg_PutParams);

	 /***** Put form to hide/show assignment *****/
	 if (Asg->Hidden)
	    Ico_PutContextualIconToUnhide (ActShoAsg,Anchor,Asg_PutParams);
	 else
	    Ico_PutContextualIconToHide (ActHidAsg,Anchor,Asg_PutParams);

	 /***** Put form to edit assignment *****/
	 Ico_PutContextualIconToEdit (ActEdiOneAsg,Asg_PutParams);
	 /* falls through */
	 /* no break */
      case Rol_STD:
      case Rol_NET:
	 /***** Put form to print assignment *****/
	 Ico_PutContextualIconToPrint (ActPrnOneAsg,Asg_PutParams);
	 break;
      default:
         break;
     }
  }

/*****************************************************************************/
/******************** Params used to edit an assignment **********************/
/*****************************************************************************/

static void Asg_PutParams (void)
  {
   if (Gbl.Asgs.AsgCodToEdit > 0)
      Asg_PutParamAsgCod (Gbl.Asgs.AsgCodToEdit);
   Asg_PutHiddenParamAsgOrder ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Pag_ASSIGNMENTS,Gbl.Asgs.CurrentPage);
  }

/*****************************************************************************/
/************************ List all the assignments ***************************/
/*****************************************************************************/

void Asg_GetListAssignments (void)
  {
   static const char *HiddenSubQuery[Rol_NUM_ROLES] =
     {
      [Rol_UNK    ] = " AND Hidden='N'",
      [Rol_GST    ] = " AND Hidden='N'",
      [Rol_USR    ] = " AND Hidden='N'",
      [Rol_STD    ] = " AND Hidden='N'",
      [Rol_NET    ] = " AND Hidden='N'",
      [Rol_TCH    ] = "",
      [Rol_DEG_ADM] = " AND Hidden='N'",
      [Rol_CTR_ADM] = " AND Hidden='N'",
      [Rol_INS_ADM] = " AND Hidden='N'",
      [Rol_SYS_ADM] = "",
     };
   static const char *OrderBySubQuery[Dat_NUM_START_END_TIME] =
     {
      [Dat_START_TIME] = "StartTime DESC,EndTime DESC,Title DESC",
      [Dat_END_TIME  ] = "EndTime DESC,StartTime DESC,Title DESC",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumAsg;

   if (Gbl.Asgs.LstIsRead)
      Asg_FreeListAssignments ();

   /***** Get list of assignments from database *****/
   if (Gbl.Crs.Grps.WhichGrps == Grp_MY_GROUPS)
      NumRows = DB_QuerySELECT (&mysql_res,"can not get assignments",
	                        "SELECT AsgCod"
				" FROM assignments"
				" WHERE CrsCod=%ld%s"
				" AND (AsgCod NOT IN (SELECT AsgCod FROM asg_grp) OR"
				" AsgCod IN (SELECT asg_grp.AsgCod FROM asg_grp,crs_grp_usr"
				" WHERE crs_grp_usr.UsrCod=%ld AND asg_grp.GrpCod=crs_grp_usr.GrpCod))"
				" ORDER BY %s",
				Gbl.Hierarchy.Crs.CrsCod,
				HiddenSubQuery[Gbl.Usrs.Me.Role.Logged],
				Gbl.Usrs.Me.UsrDat.UsrCod,
				OrderBySubQuery[Gbl.Asgs.SelectedOrder]);
   else	// Gbl.Crs.Grps.WhichGrps == Grp_ALL_GROUPS
      NumRows = DB_QuerySELECT (&mysql_res,"can not get assignments",
	                        "SELECT AsgCod"
				" FROM assignments"
				" WHERE CrsCod=%ld%s"
				" ORDER BY %s",
				Gbl.Hierarchy.Crs.CrsCod,
				HiddenSubQuery[Gbl.Usrs.Me.Role.Logged],
				OrderBySubQuery[Gbl.Asgs.SelectedOrder]);

   if (NumRows) // Assignments found...
     {
      Gbl.Asgs.Num = (unsigned) NumRows;

      /***** Create list of assignments *****/
      if ((Gbl.Asgs.LstAsgCods = (long *) calloc (NumRows,sizeof (long))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the assignments codes *****/
      for (NumAsg = 0;
	   NumAsg < Gbl.Asgs.Num;
	   NumAsg++)
        {
         /* Get next assignment code */
         row = mysql_fetch_row (mysql_res);
         if ((Gbl.Asgs.LstAsgCods[NumAsg] = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Error: wrong assignment code.");
        }
     }
   else
      Gbl.Asgs.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.Asgs.LstIsRead = true;
  }

/*****************************************************************************/
/******************* Get assignment data using its code **********************/
/*****************************************************************************/

void Asg_GetDataOfAssignmentByCod (struct Assignment *Asg)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumRows;

   if (Asg->AsgCod > 0)
     {
      /***** Build query *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get assignment data",
				"SELECT AsgCod,Hidden,UsrCod,"
				"UNIX_TIMESTAMP(StartTime),"
				"UNIX_TIMESTAMP(EndTime),"
				"NOW() BETWEEN StartTime AND EndTime,"
				"Title,Folder"
				" FROM assignments"
				" WHERE AsgCod=%ld AND CrsCod=%ld",
				Asg->AsgCod,Gbl.Hierarchy.Crs.CrsCod);

      /***** Get data of assignment *****/
      Asg_GetDataOfAssignment (Asg,&mysql_res,NumRows);
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

void Asg_GetDataOfAssignmentByFolder (struct Assignment *Asg)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumRows;

   if (Asg->Folder[0])
     {
      /***** Query database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get assignment data",
	                        "SELECT AsgCod,Hidden,UsrCod,"
				"UNIX_TIMESTAMP(StartTime),"
				"UNIX_TIMESTAMP(EndTime),"
				"NOW() BETWEEN StartTime AND EndTime,"
				"Title,Folder"
				" FROM assignments"
				" WHERE CrsCod=%ld AND Folder='%s'",
				Gbl.Hierarchy.Crs.CrsCod,Asg->Folder);

      /***** Get data of assignment *****/
      Asg_GetDataOfAssignment (Asg,&mysql_res,NumRows);
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

static void Asg_GetDataOfAssignment (struct Assignment *Asg,
                                     MYSQL_RES **mysql_res,
				     unsigned long NumRows)
  {
   MYSQL_ROW row;

   /***** Clear all assignment data *****/
   Asg_ResetAssignment (Asg);

   /***** Get data of assignment from database *****/
   if (NumRows) // Assignment found...
     {
      /* Get row */
      row = mysql_fetch_row (*mysql_res);

      /* Get code of the assignment (row[0]) */
      Asg->AsgCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get whether the assignment is hidden or not (row[1]) */
      Asg->Hidden = (row[1][0] == 'Y');

      /* Get author of the assignment (row[2]) */
      Asg->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get start date (row[3] holds the start UTC time) */
      Asg->TimeUTC[Dat_START_TIME] = Dat_GetUNIXTimeFromStr (row[3]);

      /* Get end date   (row[4] holds the end   UTC time) */
      Asg->TimeUTC[Dat_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[4]);

      /* Get whether the assignment is open or closed (row(5)) */
      Asg->Open = (row[5][0] == '1');

      /* Get the title of the assignment (row[6]) */
      Str_Copy (Asg->Title,row[6],
                Asg_MAX_BYTES_ASSIGNMENT_TITLE);

      /* Get the folder for the assignment files (row[7]) */
      Str_Copy (Asg->Folder,row[7],
                Brw_MAX_BYTES_FOLDER);
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

static void Asg_ResetAssignment (struct Assignment *Asg)
  {
   if (Asg->AsgCod <= 0)	// If > 0 ==> keep value
      Asg->AsgCod = -1L;
   Asg->AsgCod = -1L;
   Asg->Hidden = false;
   Asg->UsrCod = -1L;
   Asg->TimeUTC[Dat_START_TIME] =
   Asg->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
   Asg->Open = false;
   Asg->Title[0] = '\0';
   Asg->SendWork = Asg_DO_NOT_SEND_WORK;
   Asg->Folder[0] = '\0';
   Asg->IBelongToCrsOrGrps = false;
  }

/*****************************************************************************/
/************************* Free list of assignments **************************/
/*****************************************************************************/

void Asg_FreeListAssignments (void)
  {
   if (Gbl.Asgs.LstIsRead && Gbl.Asgs.LstAsgCods)
     {
      /***** Free memory used by the list of assignments *****/
      free (Gbl.Asgs.LstAsgCods);
      Gbl.Asgs.LstAsgCods = NULL;
      Gbl.Asgs.Num = 0;
      Gbl.Asgs.LstIsRead = false;
     }
  }

/*****************************************************************************/
/******************** Get assignment text from database **********************/
/*****************************************************************************/

static void Asg_GetAssignmentTxtFromDB (long AsgCod,char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get text of assignment from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get assignment text",
	                     "SELECT Txt FROM assignments"
			     " WHERE AsgCod=%ld AND CrsCod=%ld",
			     AsgCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** The result of the query must have one row or none *****/
   if (NumRows == 1)
     {
      /* Get info text */
      row = mysql_fetch_row (mysql_res);
      Str_Copy (Txt,row[0],
                Cns_MAX_BYTES_TEXT);
     }
   else
      Txt[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumRows > 1)
      Lay_ShowErrorAndExit ("Error when getting assignment text.");
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
   unsigned long NumRows;
   size_t Length;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Build query *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get assignment title and text",
	                     "SELECT Title,Txt FROM assignments"
	                     " WHERE AsgCod=%ld",
			     AsgCod);

   /***** Result should have a unique row *****/
   if (NumRows == 1)
     {
      /***** Get row *****/
      row = mysql_fetch_row (mysql_res);

      /***** Get summary *****/
      Str_Copy (SummaryStr,row[0],
		Ntf_MAX_BYTES_SUMMARY);

      /***** Get content *****/
      if (GetContent)
	{
	 Length = strlen (row[1]);
	 if ((*ContentStr = (char *) malloc (Length + 1)) == NULL)
	    Lay_ShowErrorAndExit ("Error allocating memory for notification content.");
	 Str_Copy (*ContentStr,row[1],
		   Length);
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
   struct Assignment Asg;

   /***** Get parameters *****/
   Asg_GetParamAsgOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Asgs.CurrentPage = Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Get assignment code *****/
   if ((Asg.AsgCod = Asg_GetParamAsgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of assignment is missing.");

   /***** Get data of the assignment from database *****/
   Asg_GetDataOfAssignmentByCod (&Asg);

   /***** Show question and button to remove the assignment *****/
   Gbl.Asgs.AsgCodToEdit = Asg.AsgCod;
   Ale_ShowAlertAndButton (ActRemAsg,NULL,NULL,Asg_PutParams,
                           Btn_REMOVE_BUTTON,Txt_Remove_assignment,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_assignment_X,
                           Asg.Title);

   /***** Show assignments again *****/
   Asg_SeeAssignments ();
  }

/*****************************************************************************/
/*************************** Remove an assignment ****************************/
/*****************************************************************************/

void Asg_RemoveAssignment (void)
  {
   extern const char *Txt_Assignment_X_removed;
   struct Assignment Asg;

   /***** Get assignment code *****/
   if ((Asg.AsgCod = Asg_GetParamAsgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of assignment is missing.");

   /***** Get data of the assignment from database *****/
   Asg_GetDataOfAssignmentByCod (&Asg);	// Inside this function, the course is checked to be the current one

   /***** Remove all the folders associated to this assignment *****/
   if (Asg.Folder[0])
      Brw_RemoveFoldersAssignmentsIfExistForAllUsrs (Asg.Folder);

   /***** Remove all the groups of this assignment *****/
   Asg_RemoveAllTheGrpsAssociatedToAnAssignment (Asg.AsgCod);

   /***** Remove assignment *****/
   DB_QueryDELETE ("can not remove assignment",
		   "DELETE FROM assignments WHERE AsgCod=%ld AND CrsCod=%ld",
                   Asg.AsgCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_ASSIGNMENT,Asg.AsgCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Assignment_X_removed,
                  Asg.Title);

   /***** Show assignments again *****/
   Asg_SeeAssignments ();
  }

/*****************************************************************************/
/**************************** Hide an assignment *****************************/
/*****************************************************************************/

void Asg_HideAssignment (void)
  {
   struct Assignment Asg;

   /***** Get assignment code *****/
   if ((Asg.AsgCod = Asg_GetParamAsgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of assignment is missing.");

   /***** Get data of the assignment from database *****/
   Asg_GetDataOfAssignmentByCod (&Asg);

   /***** Hide assignment *****/
   DB_QueryUPDATE ("can not hide assignment",
		   "UPDATE assignments SET Hidden='Y'"
		   " WHERE AsgCod=%ld AND CrsCod=%ld",
                   Asg.AsgCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Show assignments again *****/
   Asg_SeeAssignments ();
  }

/*****************************************************************************/
/**************************** Show an assignment *****************************/
/*****************************************************************************/

void Asg_ShowAssignment (void)
  {
   struct Assignment Asg;

   /***** Get assignment code *****/
   if ((Asg.AsgCod = Asg_GetParamAsgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of assignment is missing.");

   /***** Get data of the assignment from database *****/
   Asg_GetDataOfAssignmentByCod (&Asg);

   /***** Hide assignment *****/
   DB_QueryUPDATE ("can not show assignment",
		   "UPDATE assignments SET Hidden='N'"
		   " WHERE AsgCod=%ld AND CrsCod=%ld",
                   Asg.AsgCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Show assignments again *****/
   Asg_SeeAssignments ();
  }

/*****************************************************************************/
/******** Check if the title or the folder of an assignment exists ***********/
/*****************************************************************************/

static bool Asg_CheckIfSimilarAssignmentExists (const char *Field,const char *Value,long AsgCod)
  {
   /***** Get number of assignments with a field value from database *****/
   return (DB_QueryCOUNT ("can not get similar assignments",
			  "SELECT COUNT(*) FROM assignments"
			  " WHERE CrsCod=%ld"
			  " AND %s='%s' AND AsgCod<>%ld",
			  Gbl.Hierarchy.Crs.CrsCod,
			  Field,Value,AsgCod) != 0);
  }

/*****************************************************************************/
/****************** Put a form to create a new assignment ********************/
/*****************************************************************************/

void Asg_RequestCreatOrEditAsg (void)
  {
   extern const char *Hlp_ASSESSMENT_Assignments_new_assignment;
   extern const char *Hlp_ASSESSMENT_Assignments_edit_assignment;
   extern const char *Txt_New_assignment;
   extern const char *Txt_Edit_assignment;
   extern const char *Txt_Title;
   extern const char *Txt_Upload_files_QUESTION;
   extern const char *Txt_Folder;
   extern const char *Txt_Description;
   extern const char *Txt_Create_assignment;
   extern const char *Txt_Save_changes;
   struct Assignment Asg;
   bool ItsANewAssignment;
   char Txt[Cns_MAX_BYTES_TEXT + 1];
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      Dat_HMS_TO_000000,
      Dat_HMS_TO_235959
     };

   /***** Get parameters *****/
   Asg_GetParamAsgOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Asgs.CurrentPage = Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Get the code of the assignment *****/
   ItsANewAssignment = ((Asg.AsgCod = Asg_GetParamAsgCod ()) == -1L);

   /***** Get from the database the data of the assignment *****/
   if (ItsANewAssignment)
     {
      /* Initialize to empty assignment */
      Asg.AsgCod = -1L;
      Asg.TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;
      Asg.TimeUTC[Dat_END_TIME  ] = Gbl.StartExecutionTimeUTC + (2 * 60 * 60);	// +2 hours
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
      Asg_GetAssignmentTxtFromDB (Asg.AsgCod,Txt);
     }

   /***** Begin form *****/
   if (ItsANewAssignment)
     {
      Frm_StartForm (ActNewAsg);
      Gbl.Asgs.AsgCodToEdit = -1L;
     }
   else
     {
      Frm_StartForm (ActChgAsg);
      Gbl.Asgs.AsgCodToEdit = Asg.AsgCod;
     }
   Asg_PutParams ();

   /***** Begin box and table *****/
   if (ItsANewAssignment)
      Box_BoxTableBegin (NULL,Txt_New_assignment,NULL,
			 Hlp_ASSESSMENT_Assignments_new_assignment,Box_NOT_CLOSABLE,2);
   else
      Box_BoxTableBegin (NULL,
                         Asg.Title[0] ? Asg.Title :
                	                Txt_Edit_assignment,
                         NULL,
			 Hlp_ASSESSMENT_Assignments_edit_assignment,Box_NOT_CLOSABLE,2);


   /***** Assignment title *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RM","Title",Txt_Title);

   /* Data */
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("Title",Asg_MAX_CHARS_ASSIGNMENT_TITLE,Asg.Title,false,
		   "id=\"Title\" required=\"required\""
		   " class=\"TITLE_DESCRIPTION_WIDTH\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Assignment start and end dates *****/
   Dat_PutFormStartEndClientLocalDateTimes (Asg.TimeUTC,
					    Dat_FORM_SECONDS_ON,
					    SetHMS);

   /***** Send work? *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RM","Folder",Txt_Upload_files_QUESTION);

   /* Data */
   HTM_TD_Begin ("class=\"LM\"");
   HTM_LABEL_Begin ("class=\"DAT\"");
   HTM_TxtF ("%s:",Txt_Folder);
   HTM_INPUT_TEXT ("Folder",Brw_MAX_CHARS_FOLDER,Asg.Folder,false,
		   "id=\"Folder\" size=\"30\"");
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
	               " class=\"TITLE_DESCRIPTION_WIDTH\"");
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
   Asg_ShowAllAssignments ();
  }

/*****************************************************************************/
/**************** Show list of groups to edit and assignment *****************/
/*****************************************************************************/

static void Asg_ShowLstGrpsToEditAssignment (long AsgCod)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Groups;
   extern const char *Txt_The_whole_course;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   if (Gbl.Crs.Grps.GrpTypes.Num)
     {
      /***** Begin box and table *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_TxtF ("%s:",Txt_Groups);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LT\"");
      Box_BoxTableBegin ("100%",NULL,NULL,
                         Hlp_USERS_Groups,Box_NOT_CLOSABLE,0);

      /***** First row: checkbox to select the whole course *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("colspan=\"7\" class=\"DAT LM\"");
      HTM_LABEL_Begin (NULL);
      HTM_INPUT_CHECKBOX ("WholeCrs",false,
		          "id=\"WholeCrs\" value=\"Y\"%s"
		          " onclick=\"uncheckChildren(this,'GrpCods')\"",
			  Asg_CheckIfAsgIsAssociatedToGrps (AsgCod) ? "" : " checked=\"checked\"");
      HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,Gbl.Hierarchy.Crs.ShrtName);
      HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TR_End ();

      /***** List the groups for each group type *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
         if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
            Grp_ListGrpsToEditAsgAttSvyMch (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],
                                            AsgCod,Grp_ASSIGNMENT);

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

void Asg_RecFormAssignment (void)
  {
   extern const char *Txt_Already_existed_an_assignment_with_the_title_X;
   extern const char *Txt_Already_existed_an_assignment_with_the_folder_X;
   extern const char *Txt_You_must_specify_the_title_of_the_assignment;
   extern const char *Txt_Created_new_assignment_X;
   extern const char *Txt_The_assignment_has_been_modified;
   extern const char *Txt_You_can_not_disable_file_uploading_once_folders_have_been_created;
   struct Assignment OldAsg;	// Current assigment data in database
   struct Assignment NewAsg;	// Assignment data received from form
   bool ItsANewAssignment;
   bool NewAssignmentIsCorrect = true;
   unsigned NumUsrsToBeNotifiedByEMail;
   char Description[Cns_MAX_BYTES_TEXT + 1];

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
   NewAsg.TimeUTC[Dat_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   NewAsg.TimeUTC[Dat_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /***** Get assignment title *****/
   Par_GetParToText ("Title",NewAsg.Title,Asg_MAX_BYTES_ASSIGNMENT_TITLE);

   /***** Get folder name where to send works of the assignment *****/
   Par_GetParToText ("Folder",NewAsg.Folder,Brw_MAX_BYTES_FOLDER);
   NewAsg.SendWork = (NewAsg.Folder[0]) ? Asg_SEND_WORK :
	                                  Asg_DO_NOT_SEND_WORK;

   /***** Get assignment text *****/
   Par_GetParToHTML ("Txt",Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewAsg.TimeUTC[Dat_START_TIME] == 0)
      NewAsg.TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;
   if (NewAsg.TimeUTC[Dat_END_TIME] == 0)
      NewAsg.TimeUTC[Dat_END_TIME] = NewAsg.TimeUTC[Dat_START_TIME] + 2 * 60 * 60;	// +2 hours

   /***** Check if title is correct *****/
   if (NewAsg.Title[0])	// If there's an assignment title
     {
      /* If title of assignment was in database... */
      if (Asg_CheckIfSimilarAssignmentExists ("Title",NewAsg.Title,NewAsg.AsgCod))
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
               if (Asg_CheckIfSimilarAssignmentExists ("Folder",NewAsg.Folder,NewAsg.AsgCod))	// If folder of assignment was in database...
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
	 Asg_UpdateNumUsrsNotifiedByEMailAboutAssignment (NewAsg.AsgCod,NumUsrsToBeNotifiedByEMail);

      /***** Show assignments again *****/
      Asg_SeeAssignments ();
     }
   else
      // TODO: The form should be filled with partial data, now is always empty
      Asg_RequestCreatOrEditAsg ();
  }

/*****************************************************************************/
/******** Update number of users notified in table of assignments ************/
/*****************************************************************************/

static void Asg_UpdateNumUsrsNotifiedByEMailAboutAssignment (long AsgCod,unsigned NumUsrsToBeNotifiedByEMail)
  {
   /***** Update number of users notified *****/
   DB_QueryUPDATE ("can not update the number of notifications"
		   " of an assignment",
		   "UPDATE assignments SET NumNotif=NumNotif+%u"
		   " WHERE AsgCod=%ld",
                   NumUsrsToBeNotifiedByEMail,AsgCod);
  }

/*****************************************************************************/
/************************ Create a new assignment ****************************/
/*****************************************************************************/

static void Asg_CreateAssignment (struct Assignment *Asg,const char *Txt)
  {
   /***** Create a new assignment *****/
   Asg->AsgCod =
   DB_QueryINSERTandReturnCode ("can not create new assignment",
				"INSERT INTO assignments"
				" (CrsCod,UsrCod,StartTime,EndTime,Title,Folder,Txt)"
				" VALUES"
				" (%ld,%ld,FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				"'%s','%s','%s')",
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Asg->TimeUTC[Dat_START_TIME],
				Asg->TimeUTC[Dat_END_TIME  ],
				Asg->Title,
				Asg->Folder,
				Txt);

   /***** Create groups *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Asg_CreateGrps (Asg->AsgCod);
  }

/*****************************************************************************/
/********************* Update an existing assignment *************************/
/*****************************************************************************/

static void Asg_UpdateAssignment (struct Assignment *Asg,const char *Txt)
  {
   /***** Update the data of the assignment *****/
   DB_QueryUPDATE ("can not update assignment",
		   "UPDATE assignments SET "
		   "StartTime=FROM_UNIXTIME(%ld),"
		   "EndTime=FROM_UNIXTIME(%ld),"
		   "Title='%s',Folder='%s',Txt='%s'"
		   " WHERE AsgCod=%ld AND CrsCod=%ld",
                   Asg->TimeUTC[Dat_START_TIME],
                   Asg->TimeUTC[Dat_END_TIME  ],
                   Asg->Title,
                   Asg->Folder,
                   Txt,
                   Asg->AsgCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Update groups *****/
   /* Remove old groups */
   Asg_RemoveAllTheGrpsAssociatedToAnAssignment (Asg->AsgCod);

   /* Create new groups */
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Asg_CreateGrps (Asg->AsgCod);
  }

/*****************************************************************************/
/*********** Check if an assignment is associated to any group ***************/
/*****************************************************************************/

static bool Asg_CheckIfAsgIsAssociatedToGrps (long AsgCod)
  {
   /***** Get if an assignment is associated to a group from database *****/
   return (DB_QueryCOUNT ("can not check if an assignment"
			  " is associated to groups",
			  "SELECT COUNT(*) FROM asg_grp WHERE AsgCod=%ld",
			  AsgCod) != 0);
  }

/*****************************************************************************/
/************ Check if an assignment is associated to a group ****************/
/*****************************************************************************/

bool Asg_CheckIfAsgIsAssociatedToGrp (long AsgCod,long GrpCod)
  {
   /***** Get if an assignment is associated to a group from database *****/
   return (DB_QueryCOUNT ("can not check if an assignment"
			  " is associated to a group",
			  "SELECT COUNT(*) FROM asg_grp"
			  " WHERE AsgCod=%ld AND GrpCod=%ld",
		  	  AsgCod,GrpCod) != 0);
  }

/*****************************************************************************/
/********************* Remove groups of an assignment ************************/
/*****************************************************************************/

static void Asg_RemoveAllTheGrpsAssociatedToAnAssignment (long AsgCod)
  {
   /***** Remove groups of the assignment *****/
   DB_QueryDELETE ("can not remove the groups associated to an assignment",
		   "DELETE FROM asg_grp WHERE AsgCod=%ld",
		   AsgCod);
  }

/*****************************************************************************/
/**************** Remove one group from all the assignments ******************/
/*****************************************************************************/

void Asg_RemoveGroup (long GrpCod)
  {
   /***** Remove group from all the assignments *****/
   DB_QueryDELETE ("can not remove group from the associations"
	           " between assignments and groups",
		   "DELETE FROM asg_grp WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/*********** Remove groups of one type from all the assignments **************/
/*****************************************************************************/

void Asg_RemoveGroupsOfType (long GrpTypCod)
  {
   /***** Remove group from all the assignments *****/
   DB_QueryDELETE ("can not remove groups of a type from the associations"
	           " between assignments and groups",
		   "DELETE FROM asg_grp USING crs_grp,asg_grp"
		   " WHERE crs_grp.GrpTypCod=%ld"
		   " AND crs_grp.GrpCod=asg_grp.GrpCod",
                   GrpTypCod);
  }

/*****************************************************************************/
/********************* Create groups of an assignment ************************/
/*****************************************************************************/

static void Asg_CreateGrps (long AsgCod)
  {
   unsigned NumGrpSel;

   /***** Create groups of the assignment *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
      /* Create group */
      DB_QueryINSERT ("can not associate a group to an assignment",
		      "INSERT INTO asg_grp"
		      " (AsgCod,GrpCod)"
		      " VALUES"
		      " (%ld,%ld)",
                      AsgCod,
		      Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
  }

/*****************************************************************************/
/********* Get and write the names of the groups of an assignment ************/
/*****************************************************************************/

static void Asg_GetAndWriteNamesOfGrpsAssociatedToAsg (struct Assignment *Asg)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_and;
   extern const char *Txt_The_whole_course;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;

   /***** Get groups associated to an assignment from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get groups of an assignment",
	                     "SELECT crs_grp_types.GrpTypName,crs_grp.GrpName"
			     " FROM asg_grp,crs_grp,crs_grp_types"
			     " WHERE asg_grp.AsgCod=%ld"
			     " AND asg_grp.GrpCod=crs_grp.GrpCod"
			     " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
			     " ORDER BY crs_grp_types.GrpTypName,crs_grp.GrpName",
			     Asg->AsgCod);

   /***** Write heading *****/
   HTM_DIV_Begin ("class=\"%s\"",Asg->Hidden ? "ASG_GRP_LIGHT" :
        	                               "ASG_GRP");
   HTM_TxtColonNBSP (NumRows == 1 ? Txt_Group  :
                                    Txt_Groups);

   /***** Write groups *****/
   if (NumRows) // Groups found...
     {
      /* Get and write the group types and names */
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
        {
         /* Get next group */
         row = mysql_fetch_row (mysql_res);

         /* Write group type name and group name */
         HTM_TxtF ("%s&nbsp;%s",row[0],row[1]);

         if (NumRows >= 2)
           {
            if (NumRow == NumRows-2)
               HTM_TxtF (" %s ",Txt_and);
            if (NumRows >= 3)
              if (NumRow < NumRows-2)
                  HTM_Txt (", ");
           }
        }
     }
   else
      HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,Gbl.Hierarchy.Crs.ShrtName);

   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Remove all the assignments of a course *******************/
/*****************************************************************************/

void Asg_RemoveCrsAssignments (long CrsCod)
  {
   /***** Remove groups *****/
   DB_QueryDELETE ("can not remove all the groups associated"
		   " to assignments of a course",
		   "DELETE FROM asg_grp USING assignments,asg_grp"
		   " WHERE assignments.CrsCod=%ld"
		   " AND assignments.AsgCod=asg_grp.AsgCod",
                   CrsCod);

   /***** Remove assignments *****/
   DB_QueryDELETE ("can not remove all the assignments of a course",
		   "DELETE FROM assignments WHERE CrsCod=%ld",
		   CrsCod);
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
	 return (DB_QueryCOUNT ("can not check if I can do an assignment",
			        "SELECT COUNT(*) FROM assignments"
				" WHERE AsgCod=%ld"
				" AND "
				"("
				// Assignment is for the whole course
				"AsgCod NOT IN (SELECT AsgCod FROM asg_grp)"
				" OR "
				// Assignment is for specific groups
				"AsgCod IN"
				" (SELECT asg_grp.AsgCod FROM asg_grp,crs_grp_usr"
				" WHERE crs_grp_usr.UsrCod=%ld"
				" AND asg_grp.GrpCod=crs_grp_usr.GrpCod)"
				")",
				AsgCod,Gbl.Usrs.Me.UsrDat.UsrCod) != 0);
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
  }

/*****************************************************************************/
/****************** Get number of assignments in a course ********************/
/*****************************************************************************/

unsigned Asg_GetNumAssignmentsInCrs (long CrsCod)
  {
   /***** Get number of assignments in a course from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of assignments in course",
			     "SELECT COUNT(*) FROM assignments"
			     " WHERE CrsCod=%ld",
			     CrsCod);
  }

/*****************************************************************************/
/****************** Get number of courses with assignments *******************/
/*****************************************************************************/
// Returns the number of courses with assignments
// in this location (all the platform, current degree or current course)

unsigned Asg_GetNumCoursesWithAssignments (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with assignments from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with assignments",
                         "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM assignments"
			 " WHERE CrsCod>0");
         break;
       case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with assignments",
                         "SELECT COUNT(DISTINCT assignments.CrsCod)"
			 " FROM institutions,centres,degrees,courses,assignments"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=assignments.CrsCod",
                         Gbl.Hierarchy.Cty.CtyCod);
         break;
       case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with assignments",
                         "SELECT COUNT(DISTINCT assignments.CrsCod)"
			 " FROM centres,degrees,courses,assignments"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=assignments.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with assignments",
                         "SELECT COUNT(DISTINCT assignments.CrsCod)"
			 " FROM degrees,courses,assignments"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=assignments.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with assignments",
                         "SELECT COUNT(DISTINCT assignments.CrsCod)"
			 " FROM courses,assignments"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=assignments.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with assignments",
                         "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM assignments"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of courses *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumCourses) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of courses with assignments.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCourses;
  }

/*****************************************************************************/
/************************ Get number of assignments **************************/
/*****************************************************************************/
// Returns the number of assignments
// in this location (all the platform, current degree or current course)

unsigned Asg_GetNumAssignments (Hie_Level_t Scope,unsigned *NumNotif)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAssignments;

   /***** Get number of assignments from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of assignments",
                         "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM assignments"
			 " WHERE CrsCod>0");
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of assignments",
                         "SELECT COUNT(*),SUM(assignments.NumNotif)"
			 " FROM institutions,centres,degrees,courses,assignments"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=assignments.CrsCod",
                         Gbl.Hierarchy.Cty.CtyCod);
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of assignments",
                         "SELECT COUNT(*),SUM(assignments.NumNotif)"
			 " FROM centres,degrees,courses,assignments"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=assignments.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of assignments",
                         "SELECT COUNT(*),SUM(assignments.NumNotif)"
			 " FROM degrees,courses,assignments"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=assignments.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of assignments",
                         "SELECT COUNT(*),SUM(assignments.NumNotif)"
			 " FROM courses,assignments"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=assignments.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of assignments",
                         "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM assignments"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of assignments *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumAssignments) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of assignments.");

   /***** Get number of notifications by email *****/
   if (row[1])
     {
      if (sscanf (row[1],"%u",NumNotif) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of notifications of assignments.");
     }
   else
      *NumNotif = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumAssignments;
  }
