// swad_program.c: course program

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
#include "swad_program.h"
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

static void Prg_ShowAllPrgItems (void);
static void Prg_PutHeadForSeeing (bool PrintView);
static bool Prg_CheckIfICanCreatePrgItems (void);
static void Prg_PutIconsListPrgItems (void);
static void Prg_PutIconToCreateNewPrgItem (void);
static void Prg_PutButtonToCreateNewPrgItem (void);
static void Prg_ParamsWhichGroupsToShow (void);
static void Prg_ShowOnePrgItem (long PrgIteCod,bool PrintView);
static void Prg_WritePrgItemAuthor (struct ProgramItem *PrgItem);
static void Prg_GetParamPrgOrder (void);

static void Prg_PutFormsToRemEditOnePrgItem (const struct ProgramItem *PrgItem,
                                             const char *Anchor);
static void Prg_PutParams (void);
static void Prg_GetDataOfPrgItem (struct ProgramItem *PrgItem,
                                  MYSQL_RES **mysql_res,
				  unsigned long NumRows);
static void Prg_ResetPrgItem (struct ProgramItem *PrgItem);
static void Prg_GetPrgItemTxtFromDB (long PrgIteCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);
static void Prg_PutParamPrgItemCod (long PrgIteCod);
static bool Prg_CheckIfSimilarPrgItemExists (const char *Field,const char *Value,long PrgIteCod);
static void Prg_ShowLstGrpsToEditPrgItem (long PrgIteCod);
static void Prg_CreatePrgItem (struct ProgramItem *PrgItem,const char *Txt);
static void Prg_UpdatePrgItem (struct ProgramItem *PrgItem,const char *Txt);
static bool Prg_CheckIfPrgItemIsAssociatedToGrps (long PrgIteCod);
static void Prg_RemoveAllTheGrpsAssociatedToAPrgItem (long PrgIteCod);
static void Prg_CreateGrps (long PrgIteCod);
static void Prg_GetAndWriteNamesOfGrpsAssociatedToPrgItem (struct ProgramItem *PrgItem);
static bool Prg_CheckIfIBelongToCrsOrGrpsThisPrgItem (long PrgIteCod);

/*****************************************************************************/
/************************ List all the program items *************************/
/*****************************************************************************/

void Prg_SeeCourseProgram (void)
  {
   /***** Get parameters *****/
   Prg_GetParamPrgOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Prg.CurrentPage = Pag_GetParamPagNum (Pag_COURSE_PROGRAM);

   /***** Show all the program items *****/
   Prg_ShowAllPrgItems ();
  }

/*****************************************************************************/
/*********************** Show all the program items **************************/
/*****************************************************************************/

static void Prg_ShowAllPrgItems (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_Course_program;
   extern const char *Txt_No_items;
   struct Pagination Pagination;
   unsigned NumAsg;

   /***** Get list of program items *****/
   Prg_GetListPrgItems ();

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Gbl.Prg.Num;
   Pagination.CurrentPage = (int) Gbl.Prg.CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Gbl.Prg.CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Begin box *****/
   Box_BoxBegin ("100%",Txt_Course_program,Prg_PutIconsListPrgItems,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

   /***** Select whether show only my groups or all groups *****/
   if (Gbl.Crs.Grps.NumGrps)
     {
      Set_StartSettingsHead ();
      Grp_ShowFormToSelWhichGrps (ActSeePrg,Prg_ParamsWhichGroupsToShow);
      Set_EndSettingsHead ();
     }

   /***** Write links to pages *****/
   Pag_WriteLinksToPagesCentered (Pag_COURSE_PROGRAM,
				  &Pagination,
				  0);

   if (Gbl.Prg.Num)
     {
      /***** Table head *****/
      HTM_TABLE_BeginWideMarginPadding (2);
      Prg_PutHeadForSeeing (false);	// Not print view

      /***** Write all the program items *****/
      for (NumAsg = Pagination.FirstItemVisible;
	   NumAsg <= Pagination.LastItemVisible;
	   NumAsg++)
	 Prg_ShowOnePrgItem (Gbl.Prg.LstPrgIteCods[NumAsg - 1],
	                        false);	// Not print view

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No program items created
      Ale_ShowAlert (Ale_INFO,Txt_No_items);

   /***** Write again links to pages *****/
   Pag_WriteLinksToPagesCentered (Pag_COURSE_PROGRAM,
				  &Pagination,
				  0);

   /***** Button to create a new program item *****/
   if (Prg_CheckIfICanCreatePrgItems ())
      Prg_PutButtonToCreateNewPrgItem ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of program items *****/
   Prg_FreeListPrgItems ();
  }

/*****************************************************************************/
/***************** Write header with fields of a program item ****************/
/*****************************************************************************/

static void Prg_PutHeadForSeeing (bool PrintView)
  {
   extern const char *Txt_START_END_TIME_HELP[Dat_NUM_START_END_TIME];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Item;
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
	 Frm_StartForm (ActSeePrg);
	 Grp_PutParamWhichGrps ();
	 Pag_PutHiddenParamPagNum (Pag_COURSE_PROGRAM,Gbl.Prg.CurrentPage);
	 Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
	 HTM_BUTTON_SUBMIT_Begin (Txt_START_END_TIME_HELP[Order],"BT_LINK TIT_TBL",NULL);
	 if (Order == Gbl.Prg.SelectedOrder)
	    HTM_U_Begin ();
	}
      HTM_Txt (Txt_START_END_TIME[Order]);
      if (!PrintView)
	{
	 if (Order == Gbl.Prg.SelectedOrder)
	    HTM_U_End ();
	 HTM_BUTTON_End ();
	 Frm_EndForm ();
	}

      HTM_TH_End ();
     }
   HTM_TH (1,1,"LM",Txt_Item);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Check if I can create program items *********************/
/*****************************************************************************/

static bool Prg_CheckIfICanCreatePrgItems (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
                  Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
  }

/*****************************************************************************/
/************** Put contextual icons in list of program items ****************/
/*****************************************************************************/

static void Prg_PutIconsListPrgItems (void)
  {
   /***** Put icon to create a new program item *****/
   if (Prg_CheckIfICanCreatePrgItems ())
      Prg_PutIconToCreateNewPrgItem ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_COURSE_PROGRAM;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/****************** Put icon to create a new program item ********************/
/*****************************************************************************/

static void Prg_PutIconToCreateNewPrgItem (void)
  {
   extern const char *Txt_New_item;

   /***** Put form to create a new program item *****/
   Gbl.Prg.PrgIteCodToEdit = -1L;
   Ico_PutContextualIconToAdd (ActFrmNewPrgIte,NULL,Prg_PutParams,
			       Txt_New_item);
  }

/*****************************************************************************/
/***************** Put button to create a new program item *******************/
/*****************************************************************************/

static void Prg_PutButtonToCreateNewPrgItem (void)
  {
   extern const char *Txt_New_item;

   Gbl.Prg.PrgIteCodToEdit = -1L;
   Frm_StartForm (ActFrmNewPrgIte);
   Prg_PutParams ();
   Btn_PutConfirmButton (Txt_New_item);
   Frm_EndForm ();
  }

/*****************************************************************************/
/**************** Put params to select which groups to show ******************/
/*****************************************************************************/

static void Prg_ParamsWhichGroupsToShow (void)
  {
   Prg_PutHiddenParamPrgOrder ();
   Pag_PutHiddenParamPagNum (Pag_COURSE_PROGRAM,Gbl.Prg.CurrentPage);
  }

/*****************************************************************************/
/******************* Show print view of one program item *********************/
/*****************************************************************************/

void Prg_PrintOnePrgItem (void)
  {
   long PrgIteCod;

   /***** Get the code of the program item *****/
   PrgIteCod = Prg_GetParamPrgItemCod ();

   /***** Write header *****/
   Lay_WriteHeaderClassPhoto (true,false,
			      Gbl.Hierarchy.Ins.InsCod,
			      Gbl.Hierarchy.Deg.DegCod,
			      Gbl.Hierarchy.Crs.CrsCod);

   /***** Table head *****/
   HTM_TABLE_BeginWideMarginPadding (2);
   Prg_PutHeadForSeeing (true);		// Print view

   /***** Write program item *****/
   Prg_ShowOnePrgItem (PrgIteCod,
                       true);		// Print view

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************************** Show one program item ****************************/
/*****************************************************************************/

static void Prg_ShowOnePrgItem (long PrgIteCod,bool PrintView)
  {
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   char *Id;
   struct ProgramItem PrgItem;
   Dat_StartEndTime_t StartEndTime;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get data of this program item *****/
   PrgItem.PrgIteCod = PrgIteCod;
   Prg_GetDataOfPrgItemByCod (&PrgItem);

   /***** Set anchor string *****/
   Frm_SetAnchorStr (PrgItem.PrgIteCod,&Anchor);

   /***** Write first row of data of this program item *****/
   HTM_TR_Begin (NULL);

   /* Forms to remove/edit this program item */
   if (PrintView)
      HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL\"");
   else
     {
      HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL COLOR%u\"",Gbl.RowEvenOdd);
      Prg_PutFormsToRemEditOnePrgItem (&PrgItem,Anchor);
     }
   HTM_TD_End ();

   /* Start/end date/time */
   UniqueId++;

   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      if (asprintf (&Id,"scd_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      if (PrintView)
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LB\"",
		       Id,
		       PrgItem.Hidden ? (PrgItem.Open ? "DATE_GREEN_LIGHT" :
					                "DATE_RED_LIGHT") :
				        (PrgItem.Open ? "DATE_GREEN" :
					                "DATE_RED"));
      else
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LB COLOR%u\"",
		       Id,
		       PrgItem.Hidden ? (PrgItem.Open ? "DATE_GREEN_LIGHT" :
					                "DATE_RED_LIGHT") :
				        (PrgItem.Open ? "DATE_GREEN" :
					                "DATE_RED"),
		       Gbl.RowEvenOdd);
      Dat_WriteLocalDateHMSFromUTC (Id,PrgItem.TimeUTC[StartEndTime],
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				    true,true,true,0x7);
      HTM_TD_End ();
      free (Id);
     }

   /* Schedule item title */
   if (PrintView)
      HTM_TD_Begin ("class=\"%s LT\"",
		    PrgItem.Hidden ? "ASG_TITLE_LIGHT" :
				     "ASG_TITLE");
   else
      HTM_TD_Begin ("class=\"%s LT COLOR%u\"",
		    PrgItem.Hidden ? "ASG_TITLE_LIGHT" :
				     "ASG_TITLE",
		    Gbl.RowEvenOdd);
   HTM_ARTICLE_Begin (Anchor);
   HTM_Txt (PrgItem.Title);
   HTM_ARTICLE_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Write second row of data of this program item *****/
   HTM_TR_Begin (NULL);

   /* Author of the program item */
   if (PrintView)
      HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
   else
      HTM_TD_Begin ("colspan=\"2\" class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   Prg_WritePrgItemAuthor (&PrgItem);
   HTM_TD_End ();

   /* Text of the program item */
   Prg_GetPrgItemTxtFromDB (PrgItem.PrgIteCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   if (PrintView)
      HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
   else
      HTM_TD_Begin ("colspan=\"2\" class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   if (Gbl.Crs.Grps.NumGrps)
      Prg_GetAndWriteNamesOfGrpsAssociatedToPrgItem (&PrgItem);
   HTM_DIV_Begin ("class=\"PAR %s\"",PrgItem.Hidden ? "DAT_LIGHT" :
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
	                PrgIteCod,Gbl.Hierarchy.Crs.CrsCod,
	                Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************* Write the author of a program item ********************/
/*****************************************************************************/

static void Prg_WritePrgItemAuthor (struct ProgramItem *PrgItem)
  {
   Usr_WriteAuthor1Line (PrgItem->UsrCod,PrgItem->Hidden);
  }

/*****************************************************************************/
/******* Get parameter with the type or order in list of program items *******/
/*****************************************************************************/

static void Prg_GetParamPrgOrder (void)
  {
   Gbl.Prg.SelectedOrder = (Dat_StartEndTime_t)
	                    Par_GetParToUnsignedLong ("Order",
                                                      0,
                                                      Dat_NUM_START_END_TIME - 1,
                                                      (unsigned long) Prg_ORDER_DEFAULT);
  }

/*****************************************************************************/
/** Put a hidden parameter with the type of order in list of program items ***/
/*****************************************************************************/

void Prg_PutHiddenParamPrgOrder (void)
  {
   Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Gbl.Prg.SelectedOrder);
  }

/*****************************************************************************/
/**************** Put a link (form) to edit one program item *****************/
/*****************************************************************************/

static void Prg_PutFormsToRemEditOnePrgItem (const struct ProgramItem *PrgItem,
                                             const char *Anchor)
  {
   Gbl.Prg.PrgIteCodToEdit = PrgItem->PrgIteCod;	// Used as parameter in contextual links

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Put form to remove program item *****/
	 Ico_PutContextualIconToRemove (ActReqRemAsg,Prg_PutParams);

	 /***** Put form to hide/show program item *****/
	 if (PrgItem->Hidden)
	    Ico_PutContextualIconToUnhide (ActShoAsg,Anchor,Prg_PutParams);
	 else
	    Ico_PutContextualIconToHide (ActHidAsg,Anchor,Prg_PutParams);

	 /***** Put form to edit program item *****/
	 Ico_PutContextualIconToEdit (ActEdiOneAsg,Prg_PutParams);
	 /* falls through */
	 /* no break */
      case Rol_STD:
      case Rol_NET:
	 /***** Put form to print program item *****/
	 Ico_PutContextualIconToPrint (ActPrnOneAsg,Prg_PutParams);
	 break;
      default:
         break;
     }
  }

/*****************************************************************************/
/******************** Params used to edit a program item *********************/
/*****************************************************************************/

static void Prg_PutParams (void)
  {
   if (Gbl.Prg.PrgIteCodToEdit > 0)
      Prg_PutParamPrgItemCod (Gbl.Prg.PrgIteCodToEdit);
   Prg_PutHiddenParamPrgOrder ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Pag_COURSE_PROGRAM,Gbl.Prg.CurrentPage);
  }

/*****************************************************************************/
/*********************** List all the program items **************************/
/*****************************************************************************/

void Prg_GetListPrgItems (void)
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

   if (Gbl.Prg.LstIsRead)
      Prg_FreeListPrgItems ();

   /***** Get list of program items from database *****/
   if (Gbl.Crs.Grps.WhichGrps == Grp_MY_GROUPS)
      NumRows = DB_QuerySELECT (&mysql_res,"can not get program items",
	                        "SELECT PrgIteCod"
				" FROM prg_items"
				" WHERE CrsCod=%ld%s"
				" AND (PrgIteCod NOT IN (SELECT PrgIteCod FROM prg_grp) OR"
				" PrgIteCod IN (SELECT prg_grp.PrgIteCod FROM prg_grp,crs_grp_usr"
				" WHERE crs_grp_usr.UsrCod=%ld AND prg_grp.GrpCod=crs_grp_usr.GrpCod))"
				" ORDER BY %s",
				Gbl.Hierarchy.Crs.CrsCod,
				HiddenSubQuery[Gbl.Usrs.Me.Role.Logged],
				Gbl.Usrs.Me.UsrDat.UsrCod,
				OrderBySubQuery[Gbl.Prg.SelectedOrder]);
   else	// Gbl.Crs.Grps.WhichGrps == Grp_ALL_GROUPS
      NumRows = DB_QuerySELECT (&mysql_res,"can not get program items",
	                        "SELECT PrgIteCod"
				" FROM prg_items"
				" WHERE CrsCod=%ld%s"
				" ORDER BY %s",
				Gbl.Hierarchy.Crs.CrsCod,
				HiddenSubQuery[Gbl.Usrs.Me.Role.Logged],
				OrderBySubQuery[Gbl.Prg.SelectedOrder]);

   if (NumRows) // Assignments found...
     {
      Gbl.Prg.Num = (unsigned) NumRows;

      /***** Create list of program items *****/
      if ((Gbl.Prg.LstPrgIteCods = (long *) calloc (NumRows,sizeof (long))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the program items codes *****/
      for (NumAsg = 0;
	   NumAsg < Gbl.Prg.Num;
	   NumAsg++)
        {
         /* Get next program item code */
         row = mysql_fetch_row (mysql_res);
         if ((Gbl.Prg.LstPrgIteCods[NumAsg] = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Error: wrong program item code.");
        }
     }
   else
      Gbl.Prg.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.Prg.LstIsRead = true;
  }

/*****************************************************************************/
/****************** Get program item data using its code *********************/
/*****************************************************************************/

void Prg_GetDataOfPrgItemByCod (struct ProgramItem *PrgItem)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumRows;

   if (PrgItem->PrgIteCod > 0)
     {
      /***** Build query *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get program item data",
				"SELECT PrgIteCod,Hidden,UsrCod,"
				"UNIX_TIMESTAMP(StartTime),"
				"UNIX_TIMESTAMP(EndTime),"
				"NOW() BETWEEN StartTime AND EndTime,"
				"Title"
				" FROM prg_items"
				" WHERE PrgIteCod=%ld AND CrsCod=%ld",
				PrgItem->PrgIteCod,Gbl.Hierarchy.Crs.CrsCod);

      /***** Get data of program item *****/
      Prg_GetDataOfPrgItem (PrgItem,&mysql_res,NumRows);
     }
   else
     {
      /***** Clear all program item data *****/
      PrgItem->PrgIteCod = -1L;
      Prg_ResetPrgItem (PrgItem);
     }
  }

/*****************************************************************************/
/************************* Get program item data *****************************/
/*****************************************************************************/

static void Prg_GetDataOfPrgItem (struct ProgramItem *PrgItem,
                                  MYSQL_RES **mysql_res,
				  unsigned long NumRows)
  {
   MYSQL_ROW row;

   /***** Clear all program item data *****/
   Prg_ResetPrgItem (PrgItem);

   /***** Get data of program item from database *****/
   if (NumRows) // Schedule item found...
     {
      /* Get row */
      row = mysql_fetch_row (*mysql_res);

      /* Get code of the program item (row[0]) */
      PrgItem->PrgIteCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get whether the program item is hidden or not (row[1]) */
      PrgItem->Hidden = (row[1][0] == 'Y');

      /* Get author of the program item (row[2]) */
      PrgItem->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get start date (row[3] holds the start UTC time) */
      PrgItem->TimeUTC[Dat_START_TIME] = Dat_GetUNIXTimeFromStr (row[3]);

      /* Get end date   (row[4] holds the end   UTC time) */
      PrgItem->TimeUTC[Dat_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[4]);

      /* Get whether the program item is open or closed (row(5)) */
      PrgItem->Open = (row[5][0] == '1');

      /* Get the title of the program item (row[6]) */
      Str_Copy (PrgItem->Title,row[6],
                Prg_MAX_BYTES_PROGRAM_ITEM_TITLE);

      /* Can I do this program item? */
      PrgItem->IBelongToCrsOrGrps = Prg_CheckIfIBelongToCrsOrGrpsThisPrgItem (PrgItem->PrgIteCod);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************************ Clear all program item data ************************/
/*****************************************************************************/

static void Prg_ResetPrgItem (struct ProgramItem *PrgItem)
  {
   if (PrgItem->PrgIteCod <= 0)	// If > 0 ==> keep value
      PrgItem->PrgIteCod = -1L;
   PrgItem->PrgIteCod = -1L;
   PrgItem->Hidden = false;
   PrgItem->UsrCod = -1L;
   PrgItem->TimeUTC[Dat_START_TIME] =
   PrgItem->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
   PrgItem->Open = false;
   PrgItem->Title[0] = '\0';
   PrgItem->IBelongToCrsOrGrps = false;
  }

/*****************************************************************************/
/************************ Free list of program items *************************/
/*****************************************************************************/

void Prg_FreeListPrgItems (void)
  {
   if (Gbl.Prg.LstIsRead && Gbl.Prg.LstPrgIteCods)
     {
      /***** Free memory used by the list of program items *****/
      free (Gbl.Prg.LstPrgIteCods);
      Gbl.Prg.LstPrgIteCods = NULL;
      Gbl.Prg.Num = 0;
      Gbl.Prg.LstIsRead = false;
     }
  }

/*****************************************************************************/
/******************* Get program item text from database *********************/
/*****************************************************************************/

static void Prg_GetPrgItemTxtFromDB (long PrgIteCod,char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get text of program item from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get program item text",
	                     "SELECT Txt FROM prg_items"
			     " WHERE PrgIteCod=%ld AND CrsCod=%ld",
			     PrgIteCod,Gbl.Hierarchy.Crs.CrsCod);

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
      Lay_ShowErrorAndExit ("Error when getting program item text.");
  }

/*****************************************************************************/
/***************** Get summary and content of a program item  ****************/
/*****************************************************************************/
// This function may be called inside a web service

void Prg_GetNotifPrgItem (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                          char **ContentStr,
                          long PrgIteCod,bool GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   size_t Length;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Build query *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get program item title and text",
	                     "SELECT Title,Txt FROM prg_items"
	                     " WHERE PrgIteCod=%ld",
			     PrgIteCod);

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
/**************** Write parameter with code of program item ******************/
/*****************************************************************************/

static void Prg_PutParamPrgItemCod (long PrgIteCod)
  {
   Par_PutHiddenParamLong (NULL,"PrgIteCod",PrgIteCod);
  }

/*****************************************************************************/
/***************** Get parameter with code of program item *******************/
/*****************************************************************************/

long Prg_GetParamPrgItemCod (void)
  {
   /***** Get code of program item *****/
   return Par_GetParToLong ("PrgIteCod");
  }

/*****************************************************************************/
/************* Ask for confirmation of removing a program item ***************/
/*****************************************************************************/

void Prg_ReqRemPrgItem (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_item_X;
   extern const char *Txt_Remove_item;
   struct ProgramItem PrgItem;

   /***** Get parameters *****/
   Prg_GetParamPrgOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Prg.CurrentPage = Pag_GetParamPagNum (Pag_COURSE_PROGRAM);

   /***** Get program item code *****/
   if ((PrgItem.PrgIteCod = Prg_GetParamPrgItemCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of program item is missing.");

   /***** Get data of the program item from database *****/
   Prg_GetDataOfPrgItemByCod (&PrgItem);

   /***** Show question and button to remove the program item *****/
   Gbl.Prg.PrgIteCodToEdit = PrgItem.PrgIteCod;
   Ale_ShowAlertAndButton (ActRemAsg,NULL,NULL,Prg_PutParams,
                           Btn_REMOVE_BUTTON,Txt_Remove_item,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_item_X,
                           PrgItem.Title);

   /***** Show program items again *****/
   Prg_SeeCourseProgram ();
  }

/*****************************************************************************/
/*************************** Remove a program item ***************************/
/*****************************************************************************/

void Prg_RemovePrgItem (void)
  {
   extern const char *Txt_Assignment_X_removed;
   struct ProgramItem PrgItem;

   /***** Get program item code *****/
   if ((PrgItem.PrgIteCod = Prg_GetParamPrgItemCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of program item is missing.");

   /***** Get data of the program item from database *****/
   Prg_GetDataOfPrgItemByCod (&PrgItem);	// Inside this function, the course is checked to be the current one

   /***** Remove all the groups of this program item *****/
   Prg_RemoveAllTheGrpsAssociatedToAPrgItem (PrgItem.PrgIteCod);

   /***** Remove program item *****/
   DB_QueryDELETE ("can not remove program item",
		   "DELETE FROM prg_items WHERE PrgIteCod=%ld AND CrsCod=%ld",
                   PrgItem.PrgIteCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_ASSIGNMENT,PrgItem.PrgIteCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Assignment_X_removed,
                  PrgItem.Title);

   /***** Show program items again *****/
   Prg_SeeCourseProgram ();
  }

/*****************************************************************************/
/***************************** Hide a program item ***************************/
/*****************************************************************************/

void Prg_HidePrgItem (void)
  {
   struct ProgramItem PrgItem;

   /***** Get program item code *****/
   if ((PrgItem.PrgIteCod = Prg_GetParamPrgItemCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of program item is missing.");

   /***** Get data of the program item from database *****/
   Prg_GetDataOfPrgItemByCod (&PrgItem);

   /***** Hide program item *****/
   DB_QueryUPDATE ("can not hide program item",
		   "UPDATE prg_items SET Hidden='Y'"
		   " WHERE PrgIteCod=%ld AND CrsCod=%ld",
                   PrgItem.PrgIteCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Show program items again *****/
   Prg_SeeCourseProgram ();
  }

/*****************************************************************************/
/***************************** Show a program item ***************************/
/*****************************************************************************/

void Prg_ShowPrgItem (void)
  {
   struct ProgramItem PrgItem;

   /***** Get program item code *****/
   if ((PrgItem.PrgIteCod = Prg_GetParamPrgItemCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of program item is missing.");

   /***** Get data of the program item from database *****/
   Prg_GetDataOfPrgItemByCod (&PrgItem);

   /***** Hide program item *****/
   DB_QueryUPDATE ("can not show program item",
		   "UPDATE prg_items SET Hidden='N'"
		   " WHERE PrgIteCod=%ld AND CrsCod=%ld",
                   PrgItem.PrgIteCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Show program items again *****/
   Prg_SeeCourseProgram ();
  }

/*****************************************************************************/
/*************** Check if the title of a program item exists *****************/
/*****************************************************************************/

static bool Prg_CheckIfSimilarPrgItemExists (const char *Field,const char *Value,long PrgIteCod)
  {
   /***** Get number of program items with a field value from database *****/
   return (DB_QueryCOUNT ("can not get similar program items",
			  "SELECT COUNT(*) FROM prg_items"
			  " WHERE CrsCod=%ld"
			  " AND %s='%s' AND PrgIteCod<>%ld",
			  Gbl.Hierarchy.Crs.CrsCod,
			  Field,Value,PrgIteCod) != 0);
  }

/*****************************************************************************/
/***************** Put a form to create a new program item *******************/
/*****************************************************************************/

void Prg_RequestCreatOrEditPrgItem (void)
  {
   extern const char *Hlp_COURSE_Program_new_item;
   extern const char *Hlp_COURSE_Program_edit_item;
   extern const char *Txt_New_item;
   extern const char *Txt_Edit_item;
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_Create_item;
   extern const char *Txt_Save_changes;
   struct ProgramItem PrgItem;
   bool ItsANewPrgItem;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get parameters *****/
   Prg_GetParamPrgOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Prg.CurrentPage = Pag_GetParamPagNum (Pag_COURSE_PROGRAM);

   /***** Get the code of the program item *****/
   ItsANewPrgItem = ((PrgItem.PrgIteCod = Prg_GetParamPrgItemCod ()) == -1L);

   /***** Get from the database the data of the program item *****/
   if (ItsANewPrgItem)
     {
      /* Initialize to empty program item */
      PrgItem.PrgIteCod = -1L;
      PrgItem.TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;
      PrgItem.TimeUTC[Dat_END_TIME  ] = Gbl.StartExecutionTimeUTC + (2 * 60 * 60);	// +2 hours
      PrgItem.Open = true;
      PrgItem.Title[0] = '\0';
      PrgItem.IBelongToCrsOrGrps = false;
     }
   else
     {
      /* Get data of the program item from database */
      Prg_GetDataOfPrgItemByCod (&PrgItem);

      /* Get text of the program item from database */
      Prg_GetPrgItemTxtFromDB (PrgItem.PrgIteCod,Txt);
     }

   /***** Begin form *****/
   if (ItsANewPrgItem)
     {
      Frm_StartForm (ActNewPrgIte);
      Gbl.Prg.PrgIteCodToEdit = -1L;
     }
   else
     {
      Frm_StartForm (ActChgPrgIte);
      Gbl.Prg.PrgIteCodToEdit = PrgItem.PrgIteCod;
     }
   Prg_PutParams ();

   /***** Begin box and table *****/
   if (ItsANewPrgItem)
      Box_BoxTableBegin (NULL,Txt_New_item,NULL,
			 Hlp_COURSE_Program_new_item,Box_NOT_CLOSABLE,2);
   else
      Box_BoxTableBegin (NULL,
                         PrgItem.Title[0] ? PrgItem.Title :
                	                    Txt_Edit_item,
                         NULL,
			 Hlp_COURSE_Program_edit_item,Box_NOT_CLOSABLE,2);


   /***** Schedule item title *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RM","Title",Txt_Title);

   /* Data */
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("Title",Prg_MAX_CHARS_PROGRAM_ITEM_TITLE,PrgItem.Title,false,
		   "id=\"Title\" required=\"required\""
		   " class=\"TITLE_DESCRIPTION_WIDTH\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Schedule item start and end dates *****/
   Dat_PutFormStartEndClientLocalDateTimes (PrgItem.TimeUTC,Dat_FORM_SECONDS_ON);

   /***** Schedule item text *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","Txt",Txt_Description);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_TEXTAREA_Begin ("id=\"Txt\" name=\"Txt\" rows=\"10\""
	               " class=\"TITLE_DESCRIPTION_WIDTH\"");
   if (!ItsANewPrgItem)
      HTM_Txt (Txt);
   HTM_TEXTAREA_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Groups *****/
   Prg_ShowLstGrpsToEditPrgItem (PrgItem.PrgIteCod);

   /***** End table, send button and end box *****/
   if (ItsANewPrgItem)
      Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_item);
   else
      Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** Show current program items, if any *****/
   Prg_ShowAllPrgItems ();
  }

/*****************************************************************************/
/*************** Show list of groups to edit and program item ****************/
/*****************************************************************************/

static void Prg_ShowLstGrpsToEditPrgItem (long PrgIteCod)
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
			  Prg_CheckIfPrgItemIsAssociatedToGrps (PrgIteCod) ? "" :
				                                             " checked=\"checked\"");
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
                                            PrgIteCod,Grp_ASSIGNMENT);

      /***** End table and box *****/
      Box_BoxTableEnd ();
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/***************** Receive form to create a new program item *****************/
/*****************************************************************************/

void Prg_RecFormPrgItem (void)
  {
   extern const char *Txt_Already_existed_an_item_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_item;
   extern const char *Txt_Created_new_item_X;
   extern const char *Txt_The_item_has_been_modified;
   struct ProgramItem OldPrgItem;	// Current program item data in database
   struct ProgramItem NewPrgItem;	// Schedule item data received from form
   bool ItsANewPrgItem;
   bool NewPrgItemIsCorrect = true;
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Get the code of the program item *****/
   NewPrgItem.PrgIteCod = Prg_GetParamPrgItemCod ();
   ItsANewPrgItem = (NewPrgItem.PrgIteCod < 0);

   if (ItsANewPrgItem)
     {
      /***** Reset old (current, not existing) program item data *****/
      OldPrgItem.PrgIteCod = -1L;
      Prg_ResetPrgItem (&OldPrgItem);
     }
   else
     {
      /***** Get data of the old (current) program item from database *****/
      OldPrgItem.PrgIteCod = NewPrgItem.PrgIteCod;
      Prg_GetDataOfPrgItemByCod (&OldPrgItem);
     }

   /***** Get start/end date-times *****/
   NewPrgItem.TimeUTC[Dat_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   NewPrgItem.TimeUTC[Dat_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /***** Get program item title *****/
   Par_GetParToText ("Title",NewPrgItem.Title,Prg_MAX_BYTES_PROGRAM_ITEM_TITLE);

   /***** Get program item text *****/
   Par_GetParToHTML ("Txt",Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewPrgItem.TimeUTC[Dat_START_TIME] == 0)
      NewPrgItem.TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;
   if (NewPrgItem.TimeUTC[Dat_END_TIME] == 0)
      NewPrgItem.TimeUTC[Dat_END_TIME] = NewPrgItem.TimeUTC[Dat_START_TIME] + 2 * 60 * 60;	// +2 hours

   /***** Check if title is correct *****/
   if (NewPrgItem.Title[0])	// If there's a program item title
     {
      /* If title of program item was in database... */
      if (Prg_CheckIfSimilarPrgItemExists ("Title",NewPrgItem.Title,NewPrgItem.PrgIteCod))
        {
         NewPrgItemIsCorrect = false;

	 Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_an_item_with_the_title_X,
                        NewPrgItem.Title);
        }
     }
   else	// If there is not a program item title
     {
      NewPrgItemIsCorrect = false;
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_title_of_the_item);
     }

   /***** Create a new program item or update an existing one *****/
   if (NewPrgItemIsCorrect)
     {
      /* Get groups for this program items */
      Grp_GetParCodsSeveralGrps ();

      if (ItsANewPrgItem)
	{
         Prg_CreatePrgItem (&NewPrgItem,Description);	// Add new program item to database

	 /***** Write success message *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_item_X,
		        NewPrgItem.Title);
	}
      else
        {
	 Prg_UpdatePrgItem (&NewPrgItem,Description);

	 /***** Write success message *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_The_item_has_been_modified);
        }

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();

      /***** Show program items again *****/
      Prg_SeeCourseProgram ();
     }
   else
      // TODO: The form should be filled with partial data, now is always empty
      Prg_RequestCreatOrEditPrgItem ();
  }

/*****************************************************************************/
/************************ Create a new program item **************************/
/*****************************************************************************/

static void Prg_CreatePrgItem (struct ProgramItem *PrgItem,const char *Txt)
  {
   /***** Create a new program item *****/
   PrgItem->PrgIteCod =
   DB_QueryINSERTandReturnCode ("can not create new program item",
				"INSERT INTO prg_items"
				" (CrsCod,UsrCod,StartTime,EndTime,Title,Txt)"
				" VALUES"
				" (%ld,%ld,FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				"'%s','%s')",
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				PrgItem->TimeUTC[Dat_START_TIME],
				PrgItem->TimeUTC[Dat_END_TIME  ],
				PrgItem->Title,
				Txt);

   /***** Create groups *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Prg_CreateGrps (PrgItem->PrgIteCod);
  }

/*****************************************************************************/
/******************** Update an existing program item ************************/
/*****************************************************************************/

static void Prg_UpdatePrgItem (struct ProgramItem *PrgItem,const char *Txt)
  {
   /***** Update the data of the program item *****/
   DB_QueryUPDATE ("can not update program item",
		   "UPDATE prg_items SET "
		   "StartTime=FROM_UNIXTIME(%ld),"
		   "EndTime=FROM_UNIXTIME(%ld),"
		   "Title='%s',Txt='%s'"
		   " WHERE PrgIteCod=%ld AND CrsCod=%ld",
                   PrgItem->TimeUTC[Dat_START_TIME],
                   PrgItem->TimeUTC[Dat_END_TIME  ],
                   PrgItem->Title,
                   Txt,
                   PrgItem->PrgIteCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Update groups *****/
   /* Remove old groups */
   Prg_RemoveAllTheGrpsAssociatedToAPrgItem (PrgItem->PrgIteCod);

   /* Create new groups */
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Prg_CreateGrps (PrgItem->PrgIteCod);
  }

/*****************************************************************************/
/*********** Check if a program item is associated to any group **************/
/*****************************************************************************/

static bool Prg_CheckIfPrgItemIsAssociatedToGrps (long PrgIteCod)
  {
   /***** Get if a program item is associated to a group from database *****/
   return (DB_QueryCOUNT ("can not check if a program item"
			  " is associated to groups",
			  "SELECT COUNT(*) FROM prg_grp WHERE PrgIteCod=%ld",
			  PrgIteCod) != 0);
  }

/*****************************************************************************/
/************ Check if a program item is associated to a group ***************/
/*****************************************************************************/

bool Prg_CheckIfPrgItemIsAssociatedToGrp (long PrgIteCod,long GrpCod)
  {
   /***** Get if a program item is associated to a group from database *****/
   return (DB_QueryCOUNT ("can not check if a program item"
			  " is associated to a group",
			  "SELECT COUNT(*) FROM prg_grp"
			  " WHERE PrgIteCod=%ld AND GrpCod=%ld",
		  	  PrgIteCod,GrpCod) != 0);
  }

/*****************************************************************************/
/********************* Remove groups of a program item ***********************/
/*****************************************************************************/

static void Prg_RemoveAllTheGrpsAssociatedToAPrgItem (long PrgIteCod)
  {
   /***** Remove groups of the program item *****/
   DB_QueryDELETE ("can not remove the groups associated to a program item",
		   "DELETE FROM prg_grp WHERE PrgIteCod=%ld",
		   PrgIteCod);
  }

/*****************************************************************************/
/*************** Remove one group from all the program items *****************/
/*****************************************************************************/

void Prg_RemoveGroup (long GrpCod)
  {
   /***** Remove group from all the program items *****/
   DB_QueryDELETE ("can not remove group from the associations"
	           " between program items and groups",
		   "DELETE FROM prg_grp WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/********** Remove groups of one type from all the program items *************/
/*****************************************************************************/

void Prg_RemoveGroupsOfType (long GrpTypCod)
  {
   /***** Remove group from all the program items *****/
   DB_QueryDELETE ("can not remove groups of a type from the associations"
	           " between program items and groups",
		   "DELETE FROM prg_grp USING crs_grp,prg_grp"
		   " WHERE crs_grp.GrpTypCod=%ld"
		   " AND crs_grp.GrpCod=prg_grp.GrpCod",
                   GrpTypCod);
  }

/*****************************************************************************/
/********************* Create groups of a program item ***********************/
/*****************************************************************************/

static void Prg_CreateGrps (long PrgIteCod)
  {
   unsigned NumGrpSel;

   /***** Create groups of the program item *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
      /* Create group */
      DB_QueryINSERT ("can not associate a group to a program item",
		      "INSERT INTO prg_grp"
		      " (PrgIteCod,GrpCod)"
		      " VALUES"
		      " (%ld,%ld)",
                      PrgIteCod,
		      Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
  }

/*****************************************************************************/
/********* Get and write the names of the groups of a program item ***********/
/*****************************************************************************/

static void Prg_GetAndWriteNamesOfGrpsAssociatedToPrgItem (struct ProgramItem *PrgItem)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_and;
   extern const char *Txt_The_whole_course;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;

   /***** Get groups associated to a program item from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get groups of a program item",
	                     "SELECT crs_grp_types.GrpTypName,crs_grp.GrpName"
			     " FROM prg_grp,crs_grp,crs_grp_types"
			     " WHERE prg_grp.PrgIteCod=%ld"
			     " AND prg_grp.GrpCod=crs_grp.GrpCod"
			     " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
			     " ORDER BY crs_grp_types.GrpTypName,crs_grp.GrpName",
			     PrgItem->PrgIteCod);

   /***** Write heading *****/
   HTM_DIV_Begin ("class=\"%s\"",PrgItem->Hidden ? "ASG_GRP_LIGHT" :
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
/***************** Remove all the program items of a course ******************/
/*****************************************************************************/

void Prg_RemoveCrsPrgItems (long CrsCod)
  {
   /***** Remove groups *****/
   DB_QueryDELETE ("can not remove all the groups associated"
		   " to program items of a course",
		   "DELETE FROM prg_grp USING prg_items,prg_grp"
		   " WHERE prg_items.CrsCod=%ld"
		   " AND prg_items.PrgIteCod=prg_grp.PrgIteCod",
                   CrsCod);

   /***** Remove program items *****/
   DB_QueryDELETE ("can not remove all the program items of a course",
		   "DELETE FROM prg_items WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/******** Check if I belong to any of the groups of a program item ***********/
/*****************************************************************************/

static bool Prg_CheckIfIBelongToCrsOrGrpsThisPrgItem (long PrgIteCod)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 // Students and teachers can do program items depending on groups
	 /***** Get if I can do a program item from database *****/
	 return (DB_QueryCOUNT ("can not check if I can do a program item",
			        "SELECT COUNT(*) FROM prg_items"
				" WHERE PrgIteCod=%ld"
				" AND "
				"("
				// Schedule item is for the whole course
				"PrgIteCod NOT IN (SELECT PrgIteCod FROM prg_grp)"
				" OR "
				// Schedule item is for specific groups
				"PrgIteCod IN"
				" (SELECT prg_grp.PrgIteCod FROM prg_grp,crs_grp_usr"
				" WHERE crs_grp_usr.UsrCod=%ld"
				" AND prg_grp.GrpCod=crs_grp_usr.GrpCod)"
				")",
				PrgIteCod,Gbl.Usrs.Me.UsrDat.UsrCod) != 0);
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
  }

/*****************************************************************************/
/***************** Get number of program items in a course *******************/
/*****************************************************************************/

unsigned Prg_GetNumPrgItemsInCrs (long CrsCod)
  {
   /***** Get number of program items in a course from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of program items in course",
			     "SELECT COUNT(*) FROM prg_items"
			     " WHERE CrsCod=%ld",
			     CrsCod);
  }

/*****************************************************************************/
/****************** Get number of courses with program items *****************/
/*****************************************************************************/
// Returns the number of courses with program items
// in this location (all the platform, current degree or current course)

unsigned Prg_GetNumCoursesWithPrgItems (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with program items from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM prg_items"
			 " WHERE CrsCod>0");
         break;
       case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT prg_items.CrsCod)"
			 " FROM institutions,centres,degrees,courses,prg_items"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Cty.CtyCod);
         break;
       case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT prg_items.CrsCod)"
			 " FROM centres,degrees,courses,prg_items"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT prg_items.CrsCod)"
			 " FROM degrees,courses,prg_items"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT prg_items.CrsCod)"
			 " FROM courses,prg_items"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM prg_items"
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
      Lay_ShowErrorAndExit ("Error when getting number of courses with program items.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCourses;
  }

/*****************************************************************************/
/************************ Get number of program items ************************/
/*****************************************************************************/
// Returns the number of program items
// in this location (all the platform, current degree or current course)

unsigned Prg_GetNumPrgItems (Hie_Level_t Scope,unsigned *NumNotif)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumPrgItems;

   /***** Get number of program items from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of program items",
                         "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM prg_items"
			 " WHERE CrsCod>0");
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of program items",
                         "SELECT COUNT(*),SUM(prg_items.NumNotif)"
			 " FROM institutions,centres,degrees,courses,prg_items"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Cty.CtyCod);
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of program items",
                         "SELECT COUNT(*),SUM(prg_items.NumNotif)"
			 " FROM centres,degrees,courses,prg_items"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of program items",
                         "SELECT COUNT(*),SUM(prg_items.NumNotif)"
			 " FROM degrees,courses,prg_items"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of program items",
                         "SELECT COUNT(*),SUM(prg_items.NumNotif)"
			 " FROM courses,prg_items"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of program items",
                         "SELECT COUNT(*),SUM(NumNotif)"
			 " FROM prg_items"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of program items *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumPrgItems) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of program items.");

   /***** Get number of notifications by email *****/
   if (row[1])
     {
      if (sscanf (row[1],"%u",NumNotif) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of notifications of program items.");
     }
   else
      *NumNotif = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumPrgItems;
  }
