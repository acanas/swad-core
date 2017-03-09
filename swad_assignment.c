// swad_assignment.c: assignments

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_assignment.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_notification.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_photo.h"
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
static bool Asg_CheckIfICanCreateAssignments (void);
static void Asg_PutIconsListAssignments (void);
static void Asg_PutIconToCreateNewAsg (void);
static void Asg_PutButtonToCreateNewAsg (void);
static void Asg_PutParamsToCreateNewAsg (void);
static void Asg_PutFormToSelectWhichGroupsToShow (void);
static void Asg_ParamsWhichGroupsToShow (void);
static void Asg_ShowOneAssignment (long AsgCod);
static void Asg_WriteAsgAuthor (struct Assignment *Asg);
static void Asg_WriteAssignmentFolder (struct Assignment *Asg);
static void Asg_GetParamAsgOrder (void);

static void Asg_PutFormsToRemEditOneAsg (long AsgCod,bool Hidden);
static void Asg_PutParams (void);
static void Asg_GetDataOfAssignment (struct Assignment *Asg,const char *Query);
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
   Pag_GetParamPagNum (Pag_ASSIGNMENTS);

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
   extern const char *Txt_START_END_TIME_HELP[Dat_NUM_START_END_TIME];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Assignment;
   extern const char *Txt_Upload_files_QUESTION;
   extern const char *Txt_Folder;
   extern const char *Txt_No_assignments;
   Dat_StartEndTime_t Order;
   struct Pagination Pagination;
   unsigned NumAsg;

   /***** Get list of assignments *****/
   Asg_GetListAssignments ();

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Gbl.Asgs.Num;
   Pagination.CurrentPage = (int) Gbl.Pag.CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Gbl.Pag.CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Write links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (Pag_ASSIGNMENTS,0,&Pagination);

   /***** Start frame *****/
   Lay_StartRoundFrame ("100%",Txt_Assignments,
                        Asg_PutIconsListAssignments,Hlp_ASSESSMENT_Assignments);

   /***** Select whether show only my groups or all groups *****/
   if (Gbl.CurrentCrs.Grps.NumGrps)
      Asg_PutFormToSelectWhichGroupsToShow ();

   if (Gbl.Asgs.Num)
     {
      /***** Table head *****/
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_MARGIN CELLS_PAD_2\">"
                         "<tr>");
      for (Order = Dat_START_TIME;
	   Order <= Dat_END_TIME;
	   Order++)
	{
	 fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">");
	 Act_FormStart (ActSeeAsg);
	 Grp_PutParamWhichGrps ();
	 Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Act_LinkFormSubmit (Txt_START_END_TIME_HELP[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Asgs.SelectedOrder)
	    fprintf (Gbl.F.Out,"<u>");
	 fprintf (Gbl.F.Out,"%s",Txt_START_END_TIME[Order]);
	 if (Order == Gbl.Asgs.SelectedOrder)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Act_FormEnd ();
	 fprintf (Gbl.F.Out,"</th>");
	}
      fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">"
			 "%s"
			 "</th>"
			 "<th class=\"CENTER_MIDDLE\">"
			 "%s"
			 "</th>"
			 "<th class=\"LEFT_MIDDLE\">"
			 "%s"
			 "</th>"
			 "</tr>",
	       Txt_Assignment,
	       Txt_Upload_files_QUESTION,
	       Txt_Folder);

      /***** Write all the assignments *****/
      for (NumAsg = Pagination.FirstItemVisible;
	   NumAsg <= Pagination.LastItemVisible;
	   NumAsg++)
	 Asg_ShowOneAssignment (Gbl.Asgs.LstAsgCods[NumAsg - 1]);

      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>");
     }
   else	// No assignments created
      Lay_ShowAlert (Lay_INFO,Txt_No_assignments);

   /***** Button to create a new assignment *****/
   if (Asg_CheckIfICanCreateAssignments ())
      Asg_PutButtonToCreateNewAsg ();

   /***** End frame *****/
   Lay_EndRoundFrame ();

   /***** Write again links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (Pag_ASSIGNMENTS,0,&Pagination);

   /***** Free list of assignments *****/
   Asg_FreeListAssignments ();
  }

/*****************************************************************************/
/******************** Check if I can create assignments **********************/
/*****************************************************************************/

static bool Asg_CheckIfICanCreateAssignments (void)
  {
   return (bool) (Gbl.Usrs.Me.LoggedRole == Rol_TEACHER ||
                  Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);
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
   Gbl.Stat.FigureType = Sta_ASSIGNMENTS;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/******************* Put icon to create a new assignment *********************/
/*****************************************************************************/

static void Asg_PutIconToCreateNewAsg (void)
  {
   extern const char *Txt_New_assignment;

   /***** Put form to create a new assignment *****/
   Lay_PutContextualLink (ActFrmNewAsg,Asg_PutParamsToCreateNewAsg,
                          "plus64x64.png",
                          Txt_New_assignment,NULL,
                          NULL);
  }

/*****************************************************************************/
/****************** Put button to create a new assignment ********************/
/*****************************************************************************/

static void Asg_PutButtonToCreateNewAsg (void)
  {
   extern const char *Txt_New_assignment;

   Act_FormStart (ActFrmNewAsg);
   Asg_PutParamsToCreateNewAsg ();
   Lay_PutConfirmButton (Txt_New_assignment);
   Act_FormEnd ();
  }

/*****************************************************************************/
/***************** Put parameters to create a new assignment *****************/
/*****************************************************************************/

static void Asg_PutParamsToCreateNewAsg (void)
  {
   Asg_PutHiddenParamAsgOrder ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
  }

/*****************************************************************************/
/***************** Put form to select which groups to show *******************/
/*****************************************************************************/

static void Asg_PutFormToSelectWhichGroupsToShow (void)
  {
   fprintf (Gbl.F.Out,"<div style=\"display:table; margin:0 auto;\">");
   Grp_ShowFormToSelWhichGrps (ActSeeAsg,Asg_ParamsWhichGroupsToShow);
   fprintf (Gbl.F.Out,"</div>");
  }

static void Asg_ParamsWhichGroupsToShow (void)
  {
   Asg_PutHiddenParamAsgOrder ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
  }

/*****************************************************************************/
/*************************** Show one assignment *****************************/
/*****************************************************************************/

static void Asg_ShowOneAssignment (long AsgCod)
  {
   extern const char *Txt_Today;
   extern const char *Txt_ASSIGNMENT_TYPES[Asg_NUM_TYPES_SEND_WORK];
   extern const char *Txt_Yes;
   extern const char *Txt_No;
   static unsigned UniqueId = 0;
   struct Assignment Asg;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get data of this assignment *****/
   Asg.AsgCod = AsgCod;
   Asg_GetDataOfAssignmentByCod (&Asg);

   /***** Write first row of data of this assignment *****/
   /* Start date/time */
   UniqueId++;
   fprintf (Gbl.F.Out,"<tr>"
	              "<td id=\"asg_date_start_%u\" class=\"%s LEFT_BOTTOM COLOR%u\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('asg_date_start_%u',"
                      "%ld,'<br />','%s',true,true,true);"
                      "</script>"
	              "</td>",
	    UniqueId,
            Asg.Hidden ? (Asg.Open ? "DATE_GREEN_LIGHT" :
        	                     "DATE_RED_LIGHT") :
                         (Asg.Open ? "DATE_GREEN" :
                                     "DATE_RED"),
            Gbl.RowEvenOdd,
            UniqueId,Asg.TimeUTC[Dat_START_TIME],Txt_Today);

   /* End date/time */
   UniqueId++;
   fprintf (Gbl.F.Out,"<td id=\"asg_date_end_%u\" class=\"%s LEFT_BOTTOM COLOR%u\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('asg_date_end_%u',"
                      "%ld,'<br />','%s',false,true,true);"
                      "</script>"
	              "</td>",
	    UniqueId,
            Asg.Hidden ? (Asg.Open ? "DATE_GREEN_LIGHT" :
        	                     "DATE_RED_LIGHT") :
                         (Asg.Open ? "DATE_GREEN" :
                                     "DATE_RED"),
            Gbl.RowEvenOdd,
            UniqueId,Asg.TimeUTC[Dat_END_TIME],Txt_Today);

   /* Assignment title */
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">"
                      "<div class=\"%s\">%s</div>",
            Gbl.RowEvenOdd,
            Asg.Hidden ? "ASG_TITLE_LIGHT" :
        	         "ASG_TITLE",
            Asg.Title);
   fprintf (Gbl.F.Out,"</td>");

   /* Send work? */
   fprintf (Gbl.F.Out,"<td rowspan=\"2\" class=\"%s CENTER_TOP COLOR%u\">"
                      "<img src=\"%s/%s16x16.gif\""
                      " alt=\"%s\" title=\"%s\" class=\"ICO20x20\" />"
                      "<br />%s"
                      "</td>",
            (Asg.SendWork == Asg_SEND_WORK) ? "DAT_N" :
        	                              "DAT",
            Gbl.RowEvenOdd,
            Gbl.Prefs.IconsURL,
            (Asg.SendWork == Asg_SEND_WORK) ? "file_on" :
        	                              "file_off",
            Txt_ASSIGNMENT_TYPES[Asg.SendWork],
            Txt_ASSIGNMENT_TYPES[Asg.SendWork],
            (Asg.SendWork == Asg_SEND_WORK) ? Txt_Yes :
        	                              Txt_No);

   /* Assignment folder */
   fprintf (Gbl.F.Out,"<td rowspan=\"2\" class=\"DAT LEFT_TOP COLOR%u\">",
            Gbl.RowEvenOdd);
   Asg_WriteAssignmentFolder (&Asg);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Write second row of data of this assignment *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"LEFT_TOP COLOR%u\">",
            Gbl.RowEvenOdd);

   /* Author of the assignment */
   Asg_WriteAsgAuthor (&Asg);

   /* Forms to remove/edit this assignment */
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_TEACHER:
      case Rol_SYS_ADM:
         Asg_PutFormsToRemEditOneAsg (Asg.AsgCod,Asg.Hidden);
         break;
      default:
         break;
     }
   fprintf (Gbl.F.Out,"</td>");

   /* Text of the assignment */
   Asg_GetAssignmentTxtFromDB (Asg.AsgCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">",
            Gbl.RowEvenOdd);

   if (Gbl.CurrentCrs.Grps.NumGrps)
      Asg_GetAndWriteNamesOfGrpsAssociatedToAsg (&Asg);

   fprintf (Gbl.F.Out,"<p class=\"%s\">"
                      "%s"
                      "</p>"
                      "</td>"
                      "</tr>",
            Asg.Hidden ? "DAT_LIGHT" :
        	         "DAT",
            Txt);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

   /***** Mark possible notification as seen *****/
   Ntf_MarkNotifAsSeen (Ntf_EVENT_ASSIGNMENT,
	               AsgCod,Gbl.CurrentCrs.Crs.CrsCod,
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

static void Asg_WriteAssignmentFolder (struct Assignment *Asg)
  {
   extern const char *Txt_Upload_file_or_create_folder_in_FOLDER;
   extern const char *Txt_Folder;

   if (Asg->SendWork == Asg_SEND_WORK)
     {
      /***** Folder icon *****/
      if (!Asg->Hidden &&				// It's visible (not hidden)
	  Asg->Open &&					// It's open (inside dates)
	  Asg->IBelongToCrsOrGrps &&			// I belong to course or groups
	  Gbl.Usrs.Me.LoggedRole == Rol_STUDENT)	// I am a student
	 // I can send files to this assignment folder
        {
         /* Form to create a new file or folder */
         Act_FormStart (ActFrmCreAsgUsr);
         Brw_PutParamsFileBrowser (ActUnk,
                                   Brw_INTERNAL_NAME_ROOT_FOLDER_ASSIGNMENTS,
                                   Asg->Folder,
                                   Brw_IS_FOLDER,-1L);
         sprintf (Gbl.Title,Txt_Upload_file_or_create_folder_in_FOLDER,
                  Asg->Folder);
         fprintf (Gbl.F.Out,"<input type=\"image\""
                            " src=\"%s/folder-open-plus16x16.gif\""
                            " alt=\"%s\" title=\"%s\" class=\"ICO20x20\" />",
                  Gbl.Prefs.IconsURL,
                  Gbl.Title,
                  Gbl.Title);
         Act_FormEnd ();
        }
      else				// I can't send files to this assignment folder
         fprintf (Gbl.F.Out,"<img src=\"%s/folder-closed16x16.gif\""
                            " alt=\"%s\" title=\"%s\" class=\"ICO20x20\" />",
                  Gbl.Prefs.IconsURL,
                  Txt_Folder,Txt_Folder);

      /***** Folder name *****/
      fprintf (Gbl.F.Out,"%s",Asg->Folder);
     }
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
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.Asgs.SelectedOrder);
  }

/*****************************************************************************/
/***************** Put a link (form) to edit one assignment ******************/
/*****************************************************************************/

static void Asg_PutFormsToRemEditOneAsg (long AsgCod,bool Hidden)
  {
   extern const char *Txt_Remove;
   extern const char *Txt_Show;
   extern const char *Txt_Hide;
   extern const char *Txt_Edit;

   fprintf (Gbl.F.Out,"<div>");

   Gbl.Asgs.AsgCodToEdit = AsgCod;	// Used as parameter in contextual links

   /***** Put form to remove assignment *****/
   Lay_PutContextualLink (ActReqRemAsg,Asg_PutParams,
                          "remove-on64x64.png",
                          Txt_Remove,NULL,
                          NULL);

   /***** Put form to hide/show assignment *****/
   if (Hidden)
      Lay_PutContextualLink (ActShoAsg,Asg_PutParams,
                             "eye-slash-on64x64.png",
			     Txt_Show,NULL,
                             NULL);
   else
      Lay_PutContextualLink (ActHidAsg,Asg_PutParams,
                             "eye-on64x64.png",
			     Txt_Hide,NULL,
                             NULL);

   /***** Put form to edit assignment *****/
   Lay_PutContextualLink (ActEdiOneAsg,Asg_PutParams,
                          "edit64x64.png",
                          Txt_Edit,NULL,
                          NULL);

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******************** Params used to edit an assignment **********************/
/*****************************************************************************/

static void Asg_PutParams (void)
  {
   Asg_PutParamAsgCod (Gbl.Asgs.AsgCodToEdit);
   Asg_PutHiddenParamAsgOrder ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
  }

/*****************************************************************************/
/************************ List all the assignments ***************************/
/*****************************************************************************/

void Asg_GetListAssignments (void)
  {
   char HiddenSubQuery[256];
   char OrderBySubQuery[256];
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumAsg;

   if (Gbl.Asgs.LstIsRead)
      Asg_FreeListAssignments ();

   /***** Get list of assignments from database *****/
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_TEACHER:
      case Rol_SYS_ADM:
         HiddenSubQuery[0] = '\0';
         break;
      default:
         sprintf (HiddenSubQuery,"AND Hidden='N'");
         break;
     }
   switch (Gbl.Asgs.SelectedOrder)
     {
      case Dat_START_TIME:
         sprintf (OrderBySubQuery,"StartTime DESC,EndTime DESC,Title DESC");
         break;
      case Dat_END_TIME:
         sprintf (OrderBySubQuery,"EndTime DESC,StartTime DESC,Title DESC");
         break;
     }
   if (Gbl.CurrentCrs.Grps.WhichGrps == Grp_ONLY_MY_GROUPS)
      sprintf (Query,"SELECT AsgCod"
                     " FROM assignments"
                     " WHERE CrsCod='%ld'%s"
                     " AND (AsgCod NOT IN (SELECT AsgCod FROM asg_grp) OR"
                     " AsgCod IN (SELECT asg_grp.AsgCod FROM asg_grp,crs_grp_usr"
                     " WHERE crs_grp_usr.UsrCod='%ld' AND asg_grp.GrpCod=crs_grp_usr.GrpCod))"
                     " ORDER BY %s",
               Gbl.CurrentCrs.Crs.CrsCod,
               HiddenSubQuery,
               Gbl.Usrs.Me.UsrDat.UsrCod,
               OrderBySubQuery);
   else	// Gbl.CurrentCrs.Grps.WhichGrps == Grp_ALL_GROUPS
      sprintf (Query,"SELECT AsgCod"
                     " FROM assignments"
                     " WHERE CrsCod='%ld'%s"
                     " ORDER BY %s",
               Gbl.CurrentCrs.Crs.CrsCod,HiddenSubQuery,OrderBySubQuery);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get assignments");

   if (NumRows) // Assignments found...
     {
      Gbl.Asgs.Num = (unsigned) NumRows;

      /***** Create list of assignments *****/
      if ((Gbl.Asgs.LstAsgCods = (long *) calloc (NumRows,sizeof (long))) == NULL)
          Lay_ShowErrorAndExit ("Not enough memory to store list of assignments.");

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
   char Query[1024];

   if (Asg->AsgCod > 0)
     {
      /***** Build query *****/
      sprintf (Query,"SELECT AsgCod,Hidden,UsrCod,"
		     "UNIX_TIMESTAMP(StartTime),"
		     "UNIX_TIMESTAMP(EndTime),"
		     "NOW() BETWEEN StartTime AND EndTime,"
		     "Title,Folder"
		     " FROM assignments"
		     " WHERE AsgCod='%ld' AND CrsCod='%ld'",
	       Asg->AsgCod,Gbl.CurrentCrs.Crs.CrsCod);

      /***** Get data of assignment *****/
      Asg_GetDataOfAssignment (Asg,Query);
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
   char Query[1024 + Brw_MAX_BYTES_FOLDER];

   if (Asg->Folder[0])
     {
      /***** Query database *****/
      sprintf (Query,"SELECT AsgCod,Hidden,UsrCod,"
		     "UNIX_TIMESTAMP(StartTime),"
		     "UNIX_TIMESTAMP(EndTime),"
		     "NOW() BETWEEN StartTime AND EndTime,"
		     "Title,Folder"
		     " FROM assignments"
		     " WHERE CrsCod='%ld' AND Folder='%s'",
	       Gbl.CurrentCrs.Crs.CrsCod,Asg->Folder);

      /***** Get data of assignment *****/
      Asg_GetDataOfAssignment (Asg,Query);
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

static void Asg_GetDataOfAssignment (struct Assignment *Asg,const char *Query)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Clear all assignment data *****/
   Asg_ResetAssignment (Asg);

   /***** Get data of assignment from database *****/
   if (DB_QuerySELECT (Query,&mysql_res,"can not get assignment data")) // Assignment found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

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
   DB_FreeMySQLResult (&mysql_res);
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
   Asg->SendWork = false;
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
      free ((void *) Gbl.Asgs.LstAsgCods);
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
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get text of assignment from database *****/
   sprintf (Query,"SELECT Txt FROM assignments"
	          " WHERE AsgCod='%ld' AND CrsCod='%ld'",
            AsgCod,Gbl.CurrentCrs.Crs.CrsCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get assignment text");

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
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Build query *****/
   sprintf (Query,"SELECT Title,Txt FROM assignments WHERE AsgCod='%ld'",
            AsgCod);
   if (!mysql_query (&Gbl.mysql,Query))
      if ((mysql_res = mysql_store_result (&Gbl.mysql)) != NULL)
        {
         /***** Result should have a unique row *****/
         if (mysql_num_rows (mysql_res) == 1)
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
         mysql_free_result (mysql_res);
        }
  }

/*****************************************************************************/
/***************** Write parameter with code of assignment *******************/
/*****************************************************************************/

static void Asg_PutParamAsgCod (long AsgCod)
  {
   Par_PutHiddenParamLong ("AsgCod",AsgCod);
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
   Pag_GetParamPagNum (Pag_ASSIGNMENTS);

   /***** Get assignment code *****/
   if ((Asg.AsgCod = Asg_GetParamAsgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of assignment is missing.");

   /***** Get data of the assignment from database *****/
   Asg_GetDataOfAssignmentByCod (&Asg);

   /***** Button of confirmation of removing *****/
   Act_FormStart (ActRemAsg);
   Asg_PutParamAsgCod (Asg.AsgCod);
   Asg_PutHiddenParamAsgOrder ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);

   /***** Ask for confirmation of removing *****/
   sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_assignment_X,
            Asg.Title);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);
   Lay_PutRemoveButton (Txt_Remove_assignment);
   Act_FormEnd ();

   /***** Show assignments again *****/
   Asg_SeeAssignments ();
  }

/*****************************************************************************/
/*************************** Remove an assignment ****************************/
/*****************************************************************************/

void Asg_RemoveAssignment (void)
  {
   extern const char *Txt_Assignment_X_removed;
   char Query[512];
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
   sprintf (Query,"DELETE FROM assignments"
                  " WHERE AsgCod='%ld' AND CrsCod='%ld'",
            Asg.AsgCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryDELETE (Query,"can not remove assignment");

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_ASSIGNMENT,Asg.AsgCod);

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Assignment_X_removed,
            Asg.Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show assignments again *****/
   Asg_SeeAssignments ();
  }

/*****************************************************************************/
/**************************** Hide an assignment *****************************/
/*****************************************************************************/

void Asg_HideAssignment (void)
  {
   extern const char *Txt_Assignment_X_is_now_hidden;
   char Query[512];
   struct Assignment Asg;

   /***** Get assignment code *****/
   if ((Asg.AsgCod = Asg_GetParamAsgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of assignment is missing.");

   /***** Get data of the assignment from database *****/
   Asg_GetDataOfAssignmentByCod (&Asg);

   /***** Hide assignment *****/
   sprintf (Query,"UPDATE assignments SET Hidden='Y'"
                  " WHERE AsgCod='%ld' AND CrsCod='%ld'",
            Asg.AsgCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not hide assignment");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Assignment_X_is_now_hidden,
            Asg.Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show assignments again *****/
   Asg_SeeAssignments ();
  }

/*****************************************************************************/
/**************************** Show an assignment *****************************/
/*****************************************************************************/

void Asg_ShowAssignment (void)
  {
   extern const char *Txt_Assignment_X_is_now_visible;
   char Query[512];
   struct Assignment Asg;

   /***** Get assignment code *****/
   if ((Asg.AsgCod = Asg_GetParamAsgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of assignment is missing.");

   /***** Get data of the assignment from database *****/
   Asg_GetDataOfAssignmentByCod (&Asg);

   /***** Hide assignment *****/
   sprintf (Query,"UPDATE assignments SET Hidden='N'"
                  " WHERE AsgCod='%ld' AND CrsCod='%ld'",
            Asg.AsgCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not show assignment");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Assignment_X_is_now_visible,
            Asg.Title);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show assignments again *****/
   Asg_SeeAssignments ();
  }

/*****************************************************************************/
/******** Check if the title or the folder of an assignment exists ***********/
/*****************************************************************************/

static bool Asg_CheckIfSimilarAssignmentExists (const char *Field,const char *Value,long AsgCod)
  {
   char Query[256 + Asg_MAX_BYTES_ASSIGNMENT_TITLE];

   /***** Get number of assignments with a field value from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM assignments"
	          " WHERE CrsCod='%ld' AND %s='%s' AND AsgCod<>'%ld'",
            Gbl.CurrentCrs.Crs.CrsCod,Field,Value,AsgCod);
   return (DB_QueryCOUNT (Query,"can not get similar assignments") != 0);
  }

/*****************************************************************************/
/****************** Put a form to create a new assignment ********************/
/*****************************************************************************/

void Asg_RequestCreatOrEditAsg (void)
  {
   extern const char *Hlp_ASSESSMENT_Assignments_new_assignment;
   extern const char *Hlp_ASSESSMENT_Assignments_edit_assignment;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_New_assignment;
   extern const char *Txt_Edit_assignment;
   extern const char *Txt_Title;
   extern const char *Txt_Upload_files_QUESTION;
   extern const char *Txt_Folder;
   extern const char *Txt_Description;
   extern const char *Txt_Create_assignment;
   extern const char *Txt_Save;
   struct Assignment Asg;
   bool ItsANewAssignment;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get parameters *****/
   Asg_GetParamAsgOrder ();
   Grp_GetParamWhichGrps ();
   Pag_GetParamPagNum (Pag_ASSIGNMENTS);

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
      Asg.SendWork = false;
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

   /***** Start form *****/
   if (ItsANewAssignment)
      Act_FormStart (ActNewAsg);
   else
     {
      Act_FormStart (ActChgAsg);
      Asg_PutParamAsgCod (Asg.AsgCod);
     }
   Asg_PutHiddenParamAsgOrder ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);

   /***** Table start *****/
   Lay_StartRoundFrameTable (NULL,
                             ItsANewAssignment ? Txt_New_assignment :
                                                 Txt_Edit_assignment,
                             NULL,
                             ItsANewAssignment ? Hlp_ASSESSMENT_Assignments_new_assignment :
                        	                 Hlp_ASSESSMENT_Assignments_edit_assignment,
                             2);

   /***** Assignment title *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_MIDDLE\">"
	              "<label for=\"Title\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" id=\"Title\" name=\"Title\""
                      " size=\"45\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Title,
            Asg_MAX_CHARS_ASSIGNMENT_TITLE,Asg.Title);

   /***** Assignment start and end dates *****/
   Dat_PutFormStartEndClientLocalDateTimes (Asg.TimeUTC,Dat_FORM_SECONDS_ON);

   /***** Send work? *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_MIDDLE\">"
	              "%s:"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
	              "<label class=\"DAT\">%s:"
                      "<input type=\"text\" name=\"Folder\""
                      " size=\"30\" maxlength=\"%u\" value=\"%s\" />"
                      "</label>"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Upload_files_QUESTION,
            Txt_Folder,
            Brw_MAX_CHARS_FOLDER,Asg.Folder);

   /***** Assignment text *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP\">"
	              "<label for=\"Txt\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea id=\"Txt\" name=\"Txt\""
                      " cols=\"60\" rows=\"10\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Description);
   if (!ItsANewAssignment)
      fprintf (Gbl.F.Out,"%s",Txt);
   fprintf (Gbl.F.Out,"</textarea>"
                      "</td>"
                      "</tr>");

   /***** Groups *****/
   Asg_ShowLstGrpsToEditAssignment (Asg.AsgCod);

   /***** New assignment *****/
   if (ItsANewAssignment)
      Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_assignment);
   else
      Lay_EndRoundFrameTableWithButton (Lay_CONFIRM_BUTTON,Txt_Save);
   Act_FormEnd ();

   /***** Show current assignments, if any *****/
   Asg_ShowAllAssignments ();
  }

/*****************************************************************************/
/**************** Show list of groups to edit and assignment *****************/
/*****************************************************************************/

static void Asg_ShowLstGrpsToEditAssignment (long AsgCod)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Groups;
   extern const char *Txt_The_whole_course;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   if (Gbl.CurrentCrs.Grps.GrpTypes.Num)
     {
      /***** Start table *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"%s RIGHT_TOP\">"
	                 "%s:"
	                 "</td>"
                         "<td class=\"LEFT_TOP\">",
               The_ClassForm[Gbl.Prefs.Theme],
               Txt_Groups);
      Lay_StartRoundFrameTable ("100%",NULL,NULL,Hlp_USERS_Groups,0);

      /***** First row: checkbox to select the whole course *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td colspan=\"7\" class=\"DAT LEFT_MIDDLE\">"
                         "<label>"
                         "<input type=\"checkbox\" id=\"WholeCrs\" name=\"WholeCrs\" value=\"Y\"");
      if (!Asg_CheckIfAsgIsAssociatedToGrps (AsgCod))
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"uncheckChildren(this,'GrpCods')\" />"
	                 "%s %s"
                         "</label>"
	                 "</td>"
	                 "</tr>",
               Txt_The_whole_course,Gbl.CurrentCrs.Crs.ShrtName);

      /***** List the groups for each group type *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
         if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
            Grp_ListGrpsToEditAsgAttOrSvy (&Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],AsgCod,Grp_ASSIGNMENT);

      /***** End table *****/
      Lay_EndRoundFrameTable ();
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");
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
   char Txt[Cns_MAX_BYTES_TEXT + 1];

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
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

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
         sprintf (Gbl.Message,Txt_Already_existed_an_assignment_with_the_title_X,
                  NewAsg.Title);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
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
                  sprintf (Gbl.Message,Txt_Already_existed_an_assignment_with_the_folder_X,
                           NewAsg.Folder);
                  Lay_ShowAlert (Lay_WARNING,Gbl.Message);
                 }
              }
            else	// Folder name not valid
              {
               NewAssignmentIsCorrect = false;
               Lay_ShowAlert (Lay_WARNING,Gbl.Message);
              }
           }
         else	// NewAsg.SendWork == Asg_DO_NOT_SEND_WORK
           {
            if (OldAsg.SendWork == Asg_SEND_WORK)
              {
               if (Brw_CheckIfExistsFolderAssigmentForAnyUsr (OldAsg.Folder))
                 {
                  NewAssignmentIsCorrect = false;
                  Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_disable_file_uploading_once_folders_have_been_created);
                 }
              }
           }
        }
     }
   else	// If there is not an assignment title
     {
      NewAssignmentIsCorrect = false;
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_title_of_the_assignment);
     }

   /***** Create a new assignment or update an existing one *****/
   if (NewAssignmentIsCorrect)
     {
      /* Get groups for this assignments */
      Grp_GetParCodsSeveralGrps ();

      if (ItsANewAssignment)
	{
         Asg_CreateAssignment (&NewAsg,Txt);	// Add new assignment to database

	 /***** Write success message *****/
	 sprintf (Gbl.Message,Txt_Created_new_assignment_X,NewAsg.Title);
	 Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
	}
      else
        {
         if (OldAsg.Folder[0] && NewAsg.Folder[0])
            if (strcmp (OldAsg.Folder,NewAsg.Folder))	// Folder name has changed
               NewAssignmentIsCorrect = Brw_UpdateFoldersAssigmentsIfExistForAllUsrs (OldAsg.Folder,NewAsg.Folder);
         if (NewAssignmentIsCorrect)
           {
            Asg_UpdateAssignment (&NewAsg,Txt);

	    /***** Write success message *****/
	    Lay_ShowAlert (Lay_SUCCESS,Txt_The_assignment_has_been_modified);
           }
        }

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();

      /***** Notify by email about the new assignment *****/
      if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_ASSIGNMENT,NewAsg.AsgCod)))
	 Asg_UpdateNumUsrsNotifiedByEMailAboutAssignment (NewAsg.AsgCod,NumUsrsToBeNotifiedByEMail);
      Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumUsrsToBeNotifiedByEMail);

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
   char Query[512];

   /***** Update number of users notified *****/
   sprintf (Query,"UPDATE assignments SET NumNotif=NumNotif+'%u'"
                  " WHERE AsgCod='%ld'",
            NumUsrsToBeNotifiedByEMail,AsgCod);
   DB_QueryUPDATE (Query,"can not update the number of notifications of an assignment");
  }

/*****************************************************************************/
/************************ Create a new assignment ****************************/
/*****************************************************************************/

static void Asg_CreateAssignment (struct Assignment *Asg,const char *Txt)
  {
   char Query[1024 +
              Asg_MAX_BYTES_ASSIGNMENT_TITLE +
              Cns_MAX_BYTES_TEXT];

   /***** Create a new assignment *****/
   sprintf (Query,"INSERT INTO assignments"
	          " (CrsCod,UsrCod,StartTime,EndTime,Title,Folder,Txt)"
                  " VALUES"
                  " ('%ld','%ld',FROM_UNIXTIME('%ld'),FROM_UNIXTIME('%ld'),"
                  "'%s','%s','%s')",
            Gbl.CurrentCrs.Crs.CrsCod,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Asg->TimeUTC[Dat_START_TIME],
            Asg->TimeUTC[Dat_END_TIME  ],
            Asg->Title,
            Asg->Folder,
            Txt);
   Asg->AsgCod = DB_QueryINSERTandReturnCode (Query,"can not create new assignment");

   /***** Create groups *****/
   if (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps)
      Asg_CreateGrps (Asg->AsgCod);
  }

/*****************************************************************************/
/********************* Update an existing assignment *************************/
/*****************************************************************************/

static void Asg_UpdateAssignment (struct Assignment *Asg,const char *Txt)
  {
   char Query[1024 +
              Asg_MAX_BYTES_ASSIGNMENT_TITLE +
              Cns_MAX_BYTES_TEXT];

   /***** Update the data of the assignment *****/
   sprintf (Query,"UPDATE assignments SET "
	          "StartTime=FROM_UNIXTIME('%ld'),"
	          "EndTime=FROM_UNIXTIME('%ld'),"
                  "Title='%s',Folder='%s',Txt='%s'"
                  " WHERE AsgCod='%ld' AND CrsCod='%ld'",
            Asg->TimeUTC[Dat_START_TIME],
            Asg->TimeUTC[Dat_END_TIME  ],
            Asg->Title,
            Asg->Folder,
            Txt,
            Asg->AsgCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not update assignment");

   /***** Update groups *****/
   /* Remove old groups */
   Asg_RemoveAllTheGrpsAssociatedToAnAssignment (Asg->AsgCod);

   /* Create new groups */
   if (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps)
      Asg_CreateGrps (Asg->AsgCod);
  }

/*****************************************************************************/
/*********** Check if an assignment is associated to any group ***************/
/*****************************************************************************/

static bool Asg_CheckIfAsgIsAssociatedToGrps (long AsgCod)
  {
   char Query[256];

   /***** Get if an assignment is associated to a group from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM asg_grp WHERE AsgCod='%ld'",
            AsgCod);
   return (DB_QueryCOUNT (Query,"can not check if an assignment is associated to groups") != 0);
  }

/*****************************************************************************/
/************ Check if an assignment is associated to a group ****************/
/*****************************************************************************/

bool Asg_CheckIfAsgIsAssociatedToGrp (long AsgCod,long GrpCod)
  {
   char Query[256];

   /***** Get if an assignment is associated to a group from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM asg_grp"
	          " WHERE AsgCod='%ld' AND GrpCod='%ld'",
            AsgCod,GrpCod);
   return (DB_QueryCOUNT (Query,"can not check if an assignment is associated to a group") != 0);
  }

/*****************************************************************************/
/********************* Remove groups of an assignment ************************/
/*****************************************************************************/

static void Asg_RemoveAllTheGrpsAssociatedToAnAssignment (long AsgCod)
  {
   char Query[256];

   /***** Remove groups of the assignment *****/
   sprintf (Query,"DELETE FROM asg_grp WHERE AsgCod='%ld'",AsgCod);
   DB_QueryDELETE (Query,"can not remove the groups associated to an assignment");
  }

/*****************************************************************************/
/**************** Remove one group from all the assignments ******************/
/*****************************************************************************/

void Asg_RemoveGroup (long GrpCod)
  {
   char Query[256];

   /***** Remove group from all the assignments *****/
   sprintf (Query,"DELETE FROM asg_grp WHERE GrpCod='%ld'",GrpCod);
   DB_QueryDELETE (Query,"can not remove group from the associations between assignments and groups");
  }

/*****************************************************************************/
/*********** Remove groups of one type from all the assignments **************/
/*****************************************************************************/

void Asg_RemoveGroupsOfType (long GrpTypCod)
  {
   char Query[256];

   /***** Remove group from all the assignments *****/
   sprintf (Query,"DELETE FROM asg_grp USING crs_grp,asg_grp"
                  " WHERE crs_grp.GrpTypCod='%ld'"
                  " AND crs_grp.GrpCod=asg_grp.GrpCod",
            GrpTypCod);
   DB_QueryDELETE (Query,"can not remove groups of a type from the associations between assignments and groups");
  }

/*****************************************************************************/
/********************* Create groups of an assignment ************************/
/*****************************************************************************/

static void Asg_CreateGrps (long AsgCod)
  {
   unsigned NumGrpSel;
   char Query[256];

   /***** Create groups of the assignment *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
     {
      /* Create group */
      sprintf (Query,"INSERT INTO asg_grp (AsgCod,GrpCod)"
	             " VALUES ('%ld','%ld')",
               AsgCod,Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
      DB_QueryINSERT (Query,"can not associate a group to an assignment");
     }
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
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;

   /***** Get groups associated to an assignment from database *****/
   sprintf (Query,"SELECT crs_grp_types.GrpTypName,crs_grp.GrpName"
	          " FROM asg_grp,crs_grp,crs_grp_types"
                  " WHERE asg_grp.AsgCod='%ld'"
                  " AND asg_grp.GrpCod=crs_grp.GrpCod"
                  " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
                  " ORDER BY crs_grp_types.GrpTypName,crs_grp.GrpName",
            Asg->AsgCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get groups of an assignment");

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s: ",
            Asg->Hidden ? "ASG_GRP_LIGHT" :
        	          "ASG_GRP",
            (NumRows == 1) ? Txt_Group  :
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
         fprintf (Gbl.F.Out,"%s %s",row[0],row[1]);

         if (NumRows >= 2)
           {
            if (NumRow == NumRows-2)
               fprintf (Gbl.F.Out," %s ",Txt_and);
            if (NumRows >= 3)
              if (NumRow < NumRows-2)
                  fprintf (Gbl.F.Out,", ");
           }
        }
     }
   else
      fprintf (Gbl.F.Out,"%s %s",
               Txt_The_whole_course,Gbl.CurrentCrs.Crs.ShrtName);

   fprintf (Gbl.F.Out,"</div>");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Remove all the assignments of a course *******************/
/*****************************************************************************/

void Asg_RemoveCrsAssignments (long CrsCod)
  {
   char Query[512];

   /***** Remove groups *****/
   sprintf (Query,"DELETE FROM asg_grp USING assignments,asg_grp"
                  " WHERE assignments.CrsCod='%ld'"
                  " AND assignments.AsgCod=asg_grp.AsgCod",
            CrsCod);
   DB_QueryDELETE (Query,"can not remove all the groups associated to assignments of a course");

   /***** Remove assignments *****/
   sprintf (Query,"DELETE FROM assignments WHERE CrsCod='%ld'",CrsCod);
   DB_QueryDELETE (Query,"can not remove all the assignments of a course");
  }

/*****************************************************************************/
/********* Check if I belong to any of the groups of an assignment ***********/
/*****************************************************************************/

static bool Asg_CheckIfIBelongToCrsOrGrpsThisAssignment (long AsgCod)
  {
   char Query[512];

   if (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT ||
       Gbl.Usrs.Me.LoggedRole == Rol_TEACHER)
     {
      // Students and teachers can edit assignments depending on groups
      /***** Get if I can edit an assignment from database *****/
      sprintf (Query,"SELECT COUNT(*) FROM assignments"
		     " WHERE AsgCod='%ld'"
		     " AND "
		     "("
		     "AsgCod NOT IN (SELECT AsgCod FROM asg_grp)"	// Assignment is for the whole course
		     " OR "
		     "AsgCod IN"					// Assignment is for specific groups
		     " (SELECT asg_grp.AsgCod FROM asg_grp,crs_grp_usr"
		     " WHERE crs_grp_usr.UsrCod='%ld'"
		     " AND asg_grp.GrpCod=crs_grp_usr.GrpCod)"
		     ")",
	       AsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);
      return (DB_QueryCOUNT (Query,"can not check if I can do an assignment") != 0);
     }
   else
      return (Gbl.Usrs.Me.LoggedRole > Rol_TEACHER);	// Admins can edit assignments
  }

/*****************************************************************************/
/****************** Get number of assignments in a course ********************/
/*****************************************************************************/

unsigned Asg_GetNumAssignmentsInCrs (long CrsCod)
  {
   char Query[256];

   /***** Get number of assignments in a course from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM assignments WHERE CrsCod='%ld'",
            CrsCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of assignments in course");
  }

/*****************************************************************************/
/****************** Get number of courses with assignments *******************/
/*****************************************************************************/
// Returns the number of courses with assignments
// in this location (all the platform, current degree or current course)

unsigned Asg_GetNumCoursesWithAssignments (Sco_Scope_t Scope)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with assignments from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(DISTINCT CrsCod)"
                        " FROM assignments"
                        " WHERE CrsCod>'0'");
         break;
       case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(DISTINCT assignments.CrsCod)"
                        " FROM institutions,centres,degrees,courses,assignments"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=assignments.CrsCod",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
       case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(DISTINCT assignments.CrsCod)"
                        " FROM centres,degrees,courses,assignments"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=assignments.CrsCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(DISTINCT assignments.CrsCod)"
                        " FROM degrees,courses,assignments"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=assignments.CrsCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(DISTINCT assignments.CrsCod)"
                        " FROM courses,assignments"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=assignments.CrsCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(DISTINCT CrsCod)"
                        " FROM assignments"
                        " WHERE CrsCod='%ld'",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of courses with assignments");

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

unsigned Asg_GetNumAssignments (Sco_Scope_t Scope,unsigned *NumNotif)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAssignments;

   /***** Get number of assignments from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM assignments"
                        " WHERE CrsCod>'0'");
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(*),SUM(assignments.NumNotif)"
                        " FROM institutions,centres,degrees,courses,assignments"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=assignments.CrsCod",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(*),SUM(assignments.NumNotif)"
                        " FROM centres,degrees,courses,assignments"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=assignments.CrsCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(*),SUM(assignments.NumNotif)"
                        " FROM degrees,courses,assignments"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=assignments.CrsCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(*),SUM(assignments.NumNotif)"
                        " FROM courses,assignments"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=assignments.CrsCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM assignments"
                        " WHERE CrsCod='%ld'",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of assignments");

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
