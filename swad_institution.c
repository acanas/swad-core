// swad_institution.c: institutions

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_admin_database.h"
#include "swad_box.h"
#include "swad_browser_database.h"
#include "swad_center_database.h"
#include "swad_database.h"
#include "swad_department.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_forum_database.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_config.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_institution.h"
#include "swad_institution_database.h"
#include "swad_logo.h"
#include "swad_message.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_place.h"
#include "swad_survey.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct Hie_Node *Ins_EditingIns = NULL;	// Static variable to keep the institution being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ins_ListInstitutions (void);
static void Ins_PutIconsListingInstitutions (__attribute__((unused)) void *Args);
static void Ins_PutIconToEditInstitutions (void);
static void Ins_ListOneInstitutionForSeeing (struct Hie_Node *Ins,unsigned NumIns);
static void Ins_PutHeadInstitutionsForSeeing (bool OrderSelectable);

static void Ins_EditInstitutionsInternal (void);
static void Ins_PutIconsEditingInstitutions (__attribute__((unused)) void *Args);

static void Ins_GetInstitDataFromRow (MYSQL_RES *mysql_res,
                                      struct Hie_Node *Ins,
                                      bool GetNumUsrsWhoClaimToBelongToIns);

static void Ins_ListInstitutionsForEdition (void);
static Usr_Can_t Ins_CheckIfICanEdit (struct Hie_Node *Ins);

static void Ins_UpdateInsNameDB (long InsCod,const char *FldName,const char *NewName);

static void Ins_ShowAlertAndButtonToGoToIns (void);

static void Ins_PutFormToCreateInstitution (void);
static void Ins_PutHeadInstitutionsForEdition (void);
static void Ins_ReceiveRequestOrCreateIns (Hie_Status_t Status);

static void Ins_EditingInstitutionConstructor ();
static void Ins_EditingInstitutionDestructor ();

static void Ins_FormToGoToMap (struct Hie_Node *Ins);

static void Ins_GetAndShowInssOrderedByNumCtrs (Hie_Level_t HieLvl,
						Pho_ShowPhotos_t ShowPhotos);
static void Ins_GetAndShowInssOrderedByNumDegs (Hie_Level_t HieLvl,
						Pho_ShowPhotos_t ShowPhotos);
static void Ins_GetAndShowInssOrderedByNumCrss (Hie_Level_t HieLvl,
						Pho_ShowPhotos_t ShowPhotos);
static void Ins_GetAndShowInssOrderedByNumUsrsInCrss (Hie_Level_t HieLvl,
						      Pho_ShowPhotos_t ShowPhotos);
static void Ins_GetAndShowInssOrderedByNumUsrsWhoClaimToBelongToThem (Hie_Level_t HieLvl,
								      Pho_ShowPhotos_t ShowPhotos);
static void Ins_ShowInss (MYSQL_RES **mysql_res,unsigned NumInss,
		          const char *TxtFigure,Pho_ShowPhotos_t ShowPhotos);
static unsigned Ins_GetInsAndStat (struct Hie_Node *Ins,MYSQL_RES *mysql_res);

/*****************************************************************************/
/***************** List institutions with pending centers ********************/
/*****************************************************************************/

void Ins_SeeInsWithPendingCtrs (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Hlp_SYSTEM_Pending;
   extern const char *Txt_Institutions_with_pending_centers;
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Centers_ABBREVIATION;
   extern const char *Txt_There_are_no_institutions_with_requests_for_centers_to_be_confirmed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumInss = 0;
   unsigned NumIns;
   struct Hie_Node Ins;
   const char *BgColor;

   /***** Get institutions with pending centers *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_INS_ADM:
         NumInss = Ins_DB_GetInsWithPendingCtrsAdminByMe (&mysql_res);
         break;
      case Rol_SYS_ADM:
         NumInss = Ins_DB_GetAllInsWithPendingCtr (&mysql_res);
         break;
      default:	// Forbidden for other users
         Err_WrongRoleExit ();
         break;	// Not reached
     }

   /***** Get institutions *****/
   if (NumInss)
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin (Txt_Institutions_with_pending_centers,NULL,NULL,
                         Hlp_SYSTEM_Pending,Box_NOT_CLOSABLE,2);

	 /***** Write heading *****/
	 HTM_TR_Begin (NULL);
	    HTM_TH (Txt_HIERARCHY_SINGUL_Abc[Hie_INS],HTM_HEAD_LEFT );
	    HTM_TH (Txt_Centers_ABBREVIATION  ,HTM_HEAD_RIGHT);
	 HTM_TR_End ();

	 /***** List the institutions *****/
	 for (NumIns = 0, The_ResetRowColor ();
	      NumIns < NumInss;
	      NumIns++, The_ChangeRowColor ())
	   {
	    /* Get next center */
	    row = mysql_fetch_row (mysql_res);

	    /* Get institution code (row[0]) */
	    Ins.HieCod = Str_ConvertStrCodToLongCod (row[0]);
	    BgColor = (Ins.HieCod == Gbl.Hierarchy.Node[Hie_INS].HieCod) ? "BG_HIGHLIGHT" :
								           The_GetColorRows ();

	    /* Get data of institution */
	    Hie_GetDataByCod[Hie_INS] (&Ins);

	    /* Institution logo and name */
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"LM DAT_%s NOWRAP %s\"",The_GetSuffix (),BgColor);
		   Ins_DrawInstitLogoAndNameWithLink (&Ins,ActSeeCtr,"CM ICO16x16");
	       HTM_TD_End ();

	       /* Number of pending centers (row[1]) */
	       HTM_TD_Begin ("class=\"RM DAT_%s %s\"",The_GetSuffix (),BgColor);
		  HTM_Txt (row[1]);
	       HTM_TD_End ();

	    HTM_TR_End ();
	   }

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }
   else
      Ale_ShowAlert (Ale_INFO,Txt_There_are_no_institutions_with_requests_for_centers_to_be_confirmed);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Draw institution logo with link **********************/
/*****************************************************************************/

void Ins_DrawInstitutionLogoWithLink (struct Hie_Node *Ins,
				      Pho_ShowPhotos_t ShowPhotos,
				      const char *IconClass)
  {
   Hie_PutLink_t PutLink = Frm_CheckIfInside () ? Hie_DONT_PUT_LINK :	// Don't put link to institution if already inside a form
						  Hie_PUT_LINK;

   if (PutLink == Hie_PUT_LINK)
     {
      Frm_BeginForm (ActSeeInsInf);
	 ParCod_PutPar (ParCod_Ins,Ins->HieCod);
	 HTM_BUTTON_Submit_Begin (Ins->FullName,NULL,
				  "class=\"CLASSPHOTO_CAPTION BT_LINK\"");
     }
   if (ShowPhotos == Pho_PHOTOS_SHOW)
      Lgo_DrawLogo (Hie_INS,Ins,IconClass);
   else
      HTM_Txt (Ins->ShrtName);
   if (PutLink == Hie_PUT_LINK)
     {
	 HTM_BUTTON_End ();
      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/****************** Draw institution logo and name with link *****************/
/*****************************************************************************/

void Ins_DrawInstitLogoAndNameWithLink (struct Hie_Node *Ins,Act_Action_t Action,
                                        const char *IconClass)
  {
   /***** Begin form *****/
   Frm_BeginFormGoTo (Action);
      ParCod_PutPar (ParCod_Ins,Ins->HieCod);

      /***** Link to action *****/
      HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Ins->FullName),NULL,
                               "class=\"BT_LINK LT\"");
      Str_FreeGoToTitle ();

	 /***** Institution logo and name *****/
	 Lgo_DrawLogo (Hie_INS,Ins,IconClass);
	 HTM_NBSP ();
         HTM_Txt (Ins->FullName);

      /***** End link *****/
      HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();

   /***** Map *****/
   Ins_FormToGoToMap (Ins);
  }

/*****************************************************************************/
/**************** List the institutions of the current country ***************/
/*****************************************************************************/

void Ins_ShowInssOfCurrentCty (void)
  {
   if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)
     {
      /***** Get parameter with the type of order in the list of institutions *****/
      Gbl.Hierarchy.List[Hie_INS].SelectedOrder = Hie_GetParHieOrder ();

      /***** Get list of institutions *****/
      Ins_GetFullListOfInstitutions (Gbl.Hierarchy.Node[Hie_CTY].HieCod);

      /***** Write menu to select country *****/
      Hie_WriteMenuHierarchy ();

      /***** List institutions *****/
      Ins_ListInstitutions ();

      /***** Free list of institutions *****/
      Hie_FreeList (Hie_CTY);
     }
  }

/*****************************************************************************/
/*************** List the institutions of the current country ****************/
/*****************************************************************************/

static void Ins_ListInstitutions (void)
  {
   extern const char *Hlp_COUNTRY_Institutions;
   extern const char *Txt_Institutions_of_COUNTRY_X;
   extern const char *Txt_No_institutions;
   char *Title;
   unsigned NumIns;

   /***** Begin box *****/
   if (asprintf (&Title,Txt_Institutions_of_COUNTRY_X,
                 Gbl.Hierarchy.Node[Hie_CTY].FullName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (Title,Ins_PutIconsListingInstitutions,NULL,
                 Hlp_COUNTRY_Institutions,Box_NOT_CLOSABLE);
   free (Title);

      if (Gbl.Hierarchy.List[Hie_CTY].Num)	// There are institutions in the current country
	{
	 /***** Begin table *****/
	 HTM_TABLE_Begin ("TBL_SCROLL");

	    /***** Heading *****/
	    Ins_PutHeadInstitutionsForSeeing (true);	// Order selectable

	    /***** Write all institutions and their nuber of users *****/
	    for (NumIns = 0, The_ResetRowColor ();
		 NumIns < Gbl.Hierarchy.List[Hie_CTY].Num;
		 NumIns++, The_ChangeRowColor ())
	       Ins_ListOneInstitutionForSeeing (&(Gbl.Hierarchy.List[Hie_CTY].Lst[NumIns]),
	                                        NumIns + 1);

	 /***** End table *****/
	 HTM_TABLE_End ();
	}
      else	// No insrtitutions created in the current country
	 Ale_ShowAlert (Ale_INFO,Txt_No_institutions);

   Box_BoxEnd ();
  }

/*****************************************************************************/
/*************** Put contextual icons in list of institutions ****************/
/*****************************************************************************/

static void Ins_PutIconsListingInstitutions (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit institutions *****/
   if (Hie_CheckIfICanEdit () == Usr_CAN)
      Ins_PutIconToEditInstitutions ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_INSTITS);
  }

/*****************************************************************************/
/******************** Put link (form) to edit institutions *******************/
/*****************************************************************************/

static void Ins_PutIconToEditInstitutions (void)
  {
   Ico_PutContextualIconToEdit (ActEdiIns,NULL,NULL,NULL);
  }

/*****************************************************************************/
/********************** List one institution for seeing **********************/
/*****************************************************************************/

static void Ins_ListOneInstitutionForSeeing (struct Hie_Node *Ins,unsigned NumIns)
  {
   extern const char *Txt_INSTITUTION_STATUS[Hie_NUM_STATUS_TXT];
   const char *TxtClassNormal;
   const char *TxtClassStrong;
   const char *BgColor;

   if (Ins->Status & Hie_STATUS_BIT_PENDING)
     {
      TxtClassNormal =
      TxtClassStrong = "DAT_LIGHT";
     }
   else
     {
      TxtClassNormal = "DAT";
      TxtClassStrong = "DAT_STRONG";
     }
   BgColor = (Ins->HieCod == Gbl.Hierarchy.Node[Hie_INS].HieCod) ? "BG_HIGHLIGHT" :
							           The_GetColorRows ();

   HTM_TR_Begin (NULL);

      /***** Number of institution in this list *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (NumIns);
      HTM_TD_End ();

      /***** Institution logo and name *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",TxtClassStrong,The_GetSuffix (),BgColor);
	 Ins_DrawInstitLogoAndNameWithLink (Ins,ActSeeCtr,"CT ICO16x16");
      HTM_TD_End ();

      /***** Number of users who claim to belong to this institution *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Hie_GetCachedNumUsrsWhoClaimToBelongTo (Hie_INS,Ins));
      HTM_TD_End ();

      /***** Other stats *****/
      /* Number of centers in this institution */
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CTR,	// Number of centers...
						      Hie_INS,	// ...in institution
						      Ins->HieCod));
      HTM_TD_End ();

      /* Number of degrees in this institution */
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_DEG,	// Number of degrees...
						      Hie_INS,	// ...in institution
						      Ins->HieCod));
      HTM_TD_End ();

      /* Number of courses in this institution */
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CRS,	// Number of courses...
						      Hie_INS,	// ...in institution
						      Ins->HieCod));
      HTM_TD_End ();

      /* Number of departments in this institution */
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Dpt_GetNumDptsInIns (Ins->HieCod));
      HTM_TD_End ();

      /* Number of users in courses of this institution */
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Enr_GetCachedNumUsrsInCrss (Hie_INS,Ins->HieCod,
						   1 << Rol_STD |
						   1 << Rol_NET |
						   1 << Rol_TCH));	// Any user);
      HTM_TD_End ();

      /***** Institution status *****/
      Hie_WriteStatusCell (Ins->Status,TxtClassNormal,BgColor,Txt_INSTITUTION_STATUS);

   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Write header with fields of an institution *****************/
/*****************************************************************************/

static void Ins_PutHeadInstitutionsForSeeing (bool OrderSelectable)
  {
   extern const char *Txt_INSTITUTIONS_HELP_ORDER[2];
   extern const char *Txt_INSTITUTIONS_ORDER[2];
   extern const char *Txt_Centers_ABBREVIATION;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_Courses_ABBREVIATION;
   extern const char *Txt_Departments_ABBREVIATION;
   Hie_Order_t Order;
   static HTM_HeadAlign Align[Hie_NUM_ORDERS] =
     {
      [Hie_ORDER_BY_NAME    ] = HTM_HEAD_LEFT,
      [Hie_ORDER_BY_NUM_USRS] = HTM_HEAD_RIGHT
     };

   HTM_TR_Begin (NULL);

      HTM_TH_Empty (1);
      for (Order  = (Hie_Order_t) 0;
	   Order <= (Hie_Order_t) (Hie_NUM_ORDERS - 1);
	   Order++)
	{
         HTM_TH_Begin (Align[Order]);
	    if (OrderSelectable)
	      {
	       Frm_BeginForm (ActSeeIns);
		  Par_PutParUnsigned (NULL,"Order",(unsigned) Order);
		  HTM_BUTTON_Submit_Begin (Txt_INSTITUTIONS_HELP_ORDER[Order],NULL,
		                           "class=\"BT_LINK RT\"");
		     if (Order == Gbl.Hierarchy.List[Hie_CTY].SelectedOrder)
			HTM_U_Begin ();
	      }
	    HTM_Txt (Txt_INSTITUTIONS_ORDER[Order]);
	    if (OrderSelectable)
	      {
		     if (Order == Gbl.Hierarchy.List[Hie_CTY].SelectedOrder)
			HTM_U_End ();
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	      }
	 HTM_TH_End ();
	}
      HTM_TH (Txt_Centers_ABBREVIATION    ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Degrees_ABBREVIATION    ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Courses_ABBREVIATION    ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Departments_ABBREVIATION,HTM_HEAD_RIGHT);
      Usr_THTchsPlusStds ();
      HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/************************ Put forms to edit institutions *********************/
/*****************************************************************************/

void Ins_EditInstitutions (void)
  {
   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Edit institutions *****/
   Ins_EditInstitutionsInternal ();

   /***** Institution destructor *****/
   Ins_EditingInstitutionDestructor ();
  }

static void Ins_EditInstitutionsInternal (void)
  {
   extern const char *Hlp_COUNTRY_Institutions;
   extern const char *Txt_Institutions_of_COUNTRY_X;
   char *Title;

   /***** Get list of institutions *****/
   Ins_GetFullListOfInstitutions (Gbl.Hierarchy.Node[Hie_CTY].HieCod);

   /***** Write menu to select country *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   if (asprintf (&Title,Txt_Institutions_of_COUNTRY_X,
                 Gbl.Hierarchy.Node[Hie_CTY].FullName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (Title,Ins_PutIconsEditingInstitutions,NULL,
                 Hlp_COUNTRY_Institutions,Box_NOT_CLOSABLE);
   free (Title);

      /***** Put a form to create a new institution *****/
      Ins_PutFormToCreateInstitution ();

      /***** Forms to edit current institutions *****/
      if (Gbl.Hierarchy.List[Hie_CTY].Num)
	 Ins_ListInstitutionsForEdition ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of institutions *****/
   Hie_FreeList (Hie_CTY);
  }

/*****************************************************************************/
/************ Put contextual icons in edition of institutions ****************/
/*****************************************************************************/

static void Ins_PutIconsEditingInstitutions (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view institutions *****/
   Ico_PutContextualIconToView (ActSeeIns,NULL,NULL,NULL);

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_INSTITS);
  }

/*****************************************************************************/
/******* Get basic list of institutions ordered by name of institution *******/
/*****************************************************************************/

void Ins_GetBasicListOfInstitutions (long CtyCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumIns;

   /***** Get institutions from database *****/
   if ((Gbl.Hierarchy.List[Hie_CTY].Num = Ins_DB_GetInssInCtyOrderedByFullName (&mysql_res,CtyCod))) // Institutions found...
     {
      /***** Create list with institutions *****/
      if ((Gbl.Hierarchy.List[Hie_CTY].Lst = calloc ((size_t) Gbl.Hierarchy.List[Hie_CTY].Num,
						     sizeof (*Gbl.Hierarchy.List[Hie_CTY].Lst))) == NULL)
          Err_NotEnoughMemoryExit ();

      /***** Get the institutions *****/
      for (NumIns = 0;
	   NumIns < Gbl.Hierarchy.List[Hie_CTY].Num;
	   NumIns++)
         /* Get institution data */
         Ins_GetInstitDataFromRow (mysql_res,&Gbl.Hierarchy.List[Hie_CTY].Lst[NumIns],
                                   false);	// Don't get number of users who claim to belong to this institution
     }
   else
      Gbl.Hierarchy.List[Hie_CTY].Lst = NULL;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get full list of institutions                    **************/
/************* with number of users who claim to belong to them **************/
/*****************************************************************************/

void Ins_GetFullListOfInstitutions (long CtyCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumIns;

   /***** Get institutions from database *****/
   if ((Gbl.Hierarchy.List[Hie_CTY].Num = Ins_DB_GetFullListOfInssInCty (&mysql_res,CtyCod))) // Institutions found...
     {
      /***** Create list with institutions *****/
      if ((Gbl.Hierarchy.List[Hie_CTY].Lst = calloc ((size_t) Gbl.Hierarchy.List[Hie_CTY].Num,
						     sizeof (*Gbl.Hierarchy.List[Hie_CTY].Lst))) == NULL)
          Err_NotEnoughMemoryExit ();

      /***** Get the institutions *****/
      for (NumIns = 0;
	   NumIns < Gbl.Hierarchy.List[Hie_CTY].Num;
	   NumIns++)
         /* Get institution data */
         Ins_GetInstitDataFromRow (mysql_res,&Gbl.Hierarchy.List[Hie_CTY].Lst[NumIns],
                                   true);	// Get number of users who claim to belong to this institution
     }
   else
      Gbl.Hierarchy.List[Hie_CTY].Lst = NULL;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Write institution full name and country *******************/
/*****************************************************************************/
// If ClassLink == NULL ==> do not put link

void Ins_WriteInstitutionNameAndCty (long InsCod)
  {
   struct Hie_Node Ins;
   char CtyName[Nam_MAX_BYTES_FULL_NAME + 1];

   /***** Get institution short name and country name *****/
   Ins.HieCod = InsCod;
   Ins_GetShrtNameAndCtyOfInstitution (&Ins,CtyName);

   /***** Write institution short name and country name *****/
   HTM_Txt (Ins.ShrtName);
   HTM_NBSP ();
   HTM_ParTxtPar (CtyName);
  }

/*****************************************************************************/
/************************* Get data of an institution ************************/
/*****************************************************************************/

bool Ins_GetInstitDataByCod (struct Hie_Node *Node)
  {
   MYSQL_RES *mysql_res;
   bool Found = false;

   /***** Clear data *****/
   Node->PrtCod          = -1L;
   Node->Status          = (Hie_Status_t) 0;
   Node->RequesterUsrCod = -1L;
   Node->ShrtName[0]     =
   Node->FullName[0]     =
   Node->WWW[0]          = '\0';
   Node->NumUsrsWhoClaimToBelong.Valid = false;

   /***** Check if institution code is correct *****/
   if (Node->HieCod > 0)
     {
      /***** Get data of an institution from database *****/
      if (Ins_DB_GetInsDataByCod (&mysql_res,Node->HieCod))	// Institution found...
	{
         /* Get institution data */
         Ins_GetInstitDataFromRow (mysql_res,Node,
                                   false);	// Don't get number of users who claim to belong to this institution

         /* Set return value */
	 Found = true;
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return Found;
  }

/*****************************************************************************/
/********** Get data of a center from a row resulting of a query *************/
/*****************************************************************************/

static void Ins_GetInstitDataFromRow (MYSQL_RES *mysql_res,
                                      struct Hie_Node *Ins,
                                      bool GetNumUsrsWhoClaimToBelongToIns)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]: InsCod
   row[1]: CtyCod
   row[2]: Status
   row[3]: RequesterUsrCod
   row[4]: ShortName
   row[5]: FullName
   row[6]: WWW
   row[7]: number of users who claim to belong to this institution
   */
   /***** Get institution code (row[0]) *****/
   if ((Ins->HieCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongInstitExit ();

   /***** Get country code (row[1]) *****/
   Ins->PrtCod = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get institution status (row[2]) *****/
   if (sscanf (row[2],"%u",&(Ins->Status)) != 1)
      Err_WrongStatusExit ();

   /***** Get requester user's code (row[3]) *****/
   Ins->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[3]);

   /***** Get short name (row[4]), full name (row[5])
          and URL (row[6]) of the institution *****/
   Str_Copy (Ins->ShrtName,row[4],sizeof (Ins->ShrtName) - 1);
   Str_Copy (Ins->FullName,row[5],sizeof (Ins->FullName) - 1);
   Str_Copy (Ins->WWW     ,row[6],sizeof (Ins->WWW     ) - 1);

   /***** Get number of users who claim to belong to this institution (row[7]) *****/
   Ins->NumUsrsWhoClaimToBelong.Valid = false;
   if (GetNumUsrsWhoClaimToBelongToIns)
      if (sscanf (row[7],"%u",&(Ins->NumUsrsWhoClaimToBelong.NumUsrs)) == 1)
	 Ins->NumUsrsWhoClaimToBelong.Valid = true;
  }

/*****************************************************************************/
/******** Get short name and country of an institution from its code *********/
/*****************************************************************************/

void Ins_FlushCacheFullNameAndCtyOfInstitution (void)
  {
   Gbl.Cache.InstitutionShrtNameAndCty.Valid = false;
  }

void Ins_GetShrtNameAndCtyOfInstitution (struct Hie_Node *Ins,
                                         char CtyName[Nam_MAX_BYTES_FULL_NAME + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** 1. Fast check: Trivial case *****/
   if (Ins->HieCod <= 0)
     {
      Ins->ShrtName[0] = '\0';	// Empty name
      CtyName[0] = '\0';	// Empty name
      return;
     }

   /***** 2. Fast check: If cached... *****/
   if (Gbl.Cache.InstitutionShrtNameAndCty.Valid &&
       Ins->HieCod == Gbl.Cache.InstitutionShrtNameAndCty.HieCod)
     {
      Str_Copy (Ins->ShrtName,Gbl.Cache.InstitutionShrtNameAndCty.ShrtName,
		sizeof (Ins->ShrtName) - 1);
      Str_Copy (CtyName,Gbl.Cache.InstitutionShrtNameAndCty.CtyName,
		Nam_MAX_BYTES_FULL_NAME);
      return;
     }

   /***** 3. Slow: get short name and country of institution from database *****/
   Gbl.Cache.InstitutionShrtNameAndCty.HieCod = Ins->HieCod;

   if (Ins_DB_GetInsShrtNameAndCty (&mysql_res,Ins->HieCod) == 1)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get short name (row[0]) and country name (row[1]) of this institution */
      Str_Copy (Gbl.Cache.InstitutionShrtNameAndCty.ShrtName,row[0],
		sizeof (Gbl.Cache.InstitutionShrtNameAndCty.ShrtName) - 1);
      Str_Copy (Gbl.Cache.InstitutionShrtNameAndCty.CtyName ,row[1],
		sizeof (Gbl.Cache.InstitutionShrtNameAndCty.CtyName ) - 1);
     }
   else
     {
      Gbl.Cache.InstitutionShrtNameAndCty.ShrtName[0] = '\0';
      Gbl.Cache.InstitutionShrtNameAndCty.CtyName [0] = '\0';
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   Str_Copy (Ins->ShrtName,Gbl.Cache.InstitutionShrtNameAndCty.ShrtName,
	     sizeof (Ins->ShrtName) - 1);
   Str_Copy (CtyName      ,Gbl.Cache.InstitutionShrtNameAndCty.CtyName ,
	     Nam_MAX_BYTES_FULL_NAME);
   Gbl.Cache.InstitutionShrtNameAndCty.Valid = true;
  }

/*****************************************************************************/
/************************ Write selector of institution **********************/
/*****************************************************************************/

void Ins_WriteSelectorOfInstitution (void)
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumInss;
   unsigned NumIns;
   long InsCod;

   /***** Begin form *****/
   Frm_BeginFormGoTo (ActSeeCtr);

      /***** Begin selector *****/
      HTM_SELECT_Begin ((Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0) ? HTM_SUBMIT_ON_CHANGE :
								   HTM_DISABLED,
			NULL,
			"id=\"ins\" name=\"ins\" class=\"HIE_SEL INPUT_%s\"",
			The_GetSuffix ());

	 /***** Initial disabled option *****/
	 HTM_OPTION (HTM_Type_STRING,"",
		     ((Gbl.Hierarchy.Node[Hie_INS].HieCod < 0) ? HTM_SELECTED :
							         HTM_NO_ATTR) | HTM_DISABLED,
		     "[%s]",Txt_HIERARCHY_SINGUL_Abc[Hie_INS]);

	 if (Gbl.Hierarchy.Node[Hie_CTY].HieCod > 0)
	   {
	    /***** Get institutions of current country *****/
	    NumInss = Ins_DB_GetInssInCtyOrderedByShrtName (&mysql_res,Gbl.Hierarchy.Node[Hie_CTY].HieCod);

	    /***** List institutions *****/
	    for (NumIns = 0;
		 NumIns < NumInss;
		 NumIns++)
	      {
	       /* Get next institution */
	       row = mysql_fetch_row (mysql_res);

	       /* Get institution code (row[0]) */
	       if ((InsCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
		  Err_WrongInstitExit ();

	       /* Write option */
	       HTM_OPTION (HTM_Type_LONG,&InsCod,
			   (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0 &&
			   InsCod == Gbl.Hierarchy.Node[Hie_INS].HieCod) ? HTM_SELECTED :
									   HTM_NO_ATTR,
			   "%s",row[1]);
	      }

	    /***** Free structure that stores the query result *****/
	    DB_FreeMySQLResult (&mysql_res);
	   }

      /***** End selector *****/
      HTM_SELECT_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************************** List all institutions ***************************/
/*****************************************************************************/

static void Ins_ListInstitutionsForEdition (void)
  {
   extern const char *Txt_INSTITUTION_STATUS[Hie_NUM_STATUS_TXT];
   static Act_Action_t ActionRename[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = ActRenInsSho,
      [Nam_FULL_NAME] = ActRenInsFul,
     };
   unsigned NumIns;
   struct Hie_Node *Ins;
   char WWW[WWW_MAX_BYTES_WWW + 1];
   struct Usr_Data UsrDat;
   Usr_Can_t ICanEdit;
   unsigned NumCtrs;
   unsigned NumUsrsIns;
   unsigned NumUsrsInCrssOfIns;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Begin table *****/
   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Write heading *****/
      Ins_PutHeadInstitutionsForEdition ();

      /***** Write all institutions *****/
      for (NumIns = 0;
	   NumIns < Gbl.Hierarchy.List[Hie_CTY].Num;
	   NumIns++)
	{
	 Ins = &Gbl.Hierarchy.List[Hie_CTY].Lst[NumIns];

	 ICanEdit = Ins_CheckIfICanEdit (Ins);
	 NumCtrs = Hie_GetNumNodesInHieLvl (Hie_CTR,	// Number of centers...
					    Hie_INS,	// ...in institution
					    Ins->HieCod);
	 NumUsrsIns = Hie_GetNumUsrsWhoClaimToBelongTo (Hie_INS,Ins);
	 NumUsrsInCrssOfIns = Enr_GetNumUsrsInCrss (Hie_INS,Ins->HieCod,
						    1 << Rol_STD |
						    1 << Rol_NET |
						    1 << Rol_TCH);	// Any user

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove institution */
	    HTM_TD_Begin ("class=\"BT\"");
	       if (ICanEdit == Usr_CAN_NOT ||
		   NumCtrs ||		// Institution has centers
		   NumUsrsIns ||		// Institution has users
		   NumUsrsInCrssOfIns)	// Institution has users
		  // Institution has centers or users ==> deletion forbidden
		  Ico_PutIconRemovalNotAllowed ();
	       else
		  Ico_PutContextualIconToRemove (ActRemIns,NULL,
						 Hie_PutParOtherHieCod,&Ins->HieCod);
	    HTM_TD_End ();

	    /* Institution code */
	    HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
	       HTM_Long (Ins->HieCod);
	    HTM_TD_End ();

	    /* Institution logo */
	    HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Ins->FullName);
	       Lgo_DrawLogo (Hie_INS,Ins,"ICO20x20");
	    HTM_TD_End ();

	    /* Institution short name and full name */
	    Names[Nam_SHRT_NAME] = Ins->ShrtName;
	    Names[Nam_FULL_NAME] = Ins->FullName;
	    Nam_ExistingShortAndFullNames (ActionRename,
				           ParCod_OthHie,Ins->HieCod,
				           Names,
				           ICanEdit == Usr_CAN ? Frm_PUT_FORM :
				        			   Frm_DONT_PUT_FORM);

	    /* Institution WWW */
	    HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
	       switch (ICanEdit)
		 {
		  case Usr_CAN:
		     Frm_BeginForm (ActChgInsWWW);
			ParCod_PutPar (ParCod_OthHie,Ins->HieCod);
			HTM_INPUT_URL ("WWW",Ins->WWW,
				       HTM_REQUIRED | HTM_SUBMIT_ON_CHANGE,
				       "class=\"INPUT_WWW INPUT_%s\"",
				       The_GetSuffix ());
		     Frm_EndForm ();
		     break;
		  case Usr_CAN_NOT:
		  default:
		     Str_Copy (WWW,Ins->WWW,sizeof (WWW) - 1);
		     HTM_DIV_Begin ("class=\"EXTERNAL_WWW_SHRT\"");
			HTM_A_Begin ("href=\"%s\" target=\"_blank\" title=\"%s\""
				     " class=\"DAT_%s\"",
				     Ins->WWW,
				     The_GetSuffix (),
				     Ins->WWW);
			   HTM_Txt (WWW);
			HTM_A_End ();
		     HTM_DIV_End ();
		     break;
		 }
	    HTM_TD_End ();

	    /* Number of users who claim to belong to this institution */
	    HTM_TD_Unsigned (NumUsrsIns);

	    /* Number of centers */
	    HTM_TD_Unsigned (NumCtrs);

	    /* Number of users in courses of this institution */
	    HTM_TD_Unsigned (NumUsrsInCrssOfIns);

	    /* Institution requester */
	    HTM_TD_Begin ("class=\"LT DAT_%s INPUT_REQUESTER\"",The_GetSuffix ());
	       UsrDat.UsrCod = Ins->RequesterUsrCod;
	       Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							Usr_DONT_GET_PREFS,
							Usr_DONT_GET_ROLE_IN_CRS);
	       Usr_WriteAuthor (&UsrDat,For_ENABLED);
	    HTM_TD_End ();

	    /* Institution status */
	    Hie_WriteStatusCellEditable (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ? Usr_CAN :
										  Usr_CAN_NOT,
	                                 Ins->Status,ActChgInsSta,Ins->HieCod,
	                                 Txt_INSTITUTION_STATUS);

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************ Check if I can edit, remove, etc. an institution ***************/
/*****************************************************************************/

static Usr_Can_t Ins_CheckIfICanEdit (struct Hie_Node *Ins)
  {
   return (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||		// I am a superuser
           ((Ins->Status & Hie_STATUS_BIT_PENDING) &&	// Institution is not yet activated
           Gbl.Usrs.Me.UsrDat.UsrCod == Ins->RequesterUsrCod)) ? Usr_CAN :	// I am the requester
        							 Usr_CAN_NOT;
  }

/*****************************************************************************/
/**************************** Remove a institution ***************************/
/*****************************************************************************/

void Ins_RemoveInstitution (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_To_remove_an_institution_you_must_first_remove_all_centers_and_users_in_the_institution;
   extern const char *Txt_Institution_X_removed;
   char PathIns[PATH_MAX + 1];

   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Get institution code *****/
   Ins_EditingIns->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /***** Get data of the institution from database *****/
   Hie_GetDataByCod[Hie_INS] (Ins_EditingIns);

   /***** Check if I can edit this institution *****/
   if (Ins_CheckIfICanEdit (Ins_EditingIns) == Usr_CAN_NOT)
      Err_NoPermissionExit ();

   /***** Check if this institution has centers or users *****/
   if (Hie_GetNumNodesInHieLvl (Hie_CTR,	// Number of centers...
				Hie_INS,	// ...in institution
				Ins_EditingIns->HieCod))
      // Institution has centers ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_an_institution_you_must_first_remove_all_centers_and_users_in_the_institution);
   else if (Hie_GetNumUsrsWhoClaimToBelongTo (Hie_INS,Ins_EditingIns))
      // Institution has users ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_an_institution_you_must_first_remove_all_centers_and_users_in_the_institution);
   else if (Enr_GetNumUsrsInCrss (Hie_INS,Ins_EditingIns->HieCod,
				  1 << Rol_STD |
				  1 << Rol_NET |
				  1 << Rol_TCH))	// Any user
      // Institution has users ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_an_institution_you_must_first_remove_all_centers_and_users_in_the_institution);
   else	// Institution has no users ==> remove it
     {
      /***** Remove all threads and posts in forums of the institution *****/
      For_DB_RemoveForums (Hie_INS,Ins_EditingIns->HieCod);

      /***** Remove surveys of the institution *****/
      Svy_RemoveSurveys (Hie_INS,Ins_EditingIns->HieCod);

      /***** Remove information related to files in institution *****/
      Brw_DB_RemoveInsFiles (Ins_EditingIns->HieCod);

      /***** Remove directories of the institution *****/
      snprintf (PathIns,sizeof (PathIns),"%s/%02u/%u",
	        Cfg_PATH_INS_PUBLIC,
	        (unsigned) (Ins_EditingIns->HieCod % 100),
	        (unsigned)  Ins_EditingIns->HieCod);
      Fil_RemoveTree (PathIns);

      /***** Remove administrators of this institution *****/
      Adm_DB_RemAdmins (Hie_INS,Ins_EditingIns->HieCod);

      /***** Remove institution *****/
      Ins_DB_RemoveInstitution (Ins_EditingIns->HieCod);

      /***** Flush caches *****/
      Ins_FlushCacheFullNameAndCtyOfInstitution ();
      Dpt_FlushCacheNumDptsInIns ();
      Hie_FlushCachedNumNodesInHieLvl (Hie_CTR,Hie_INS);	// Number of centers in institution
      Hie_FlushCachedNumNodesInHieLvl (Hie_DEG,Hie_INS);	// Number of degrees in institution
      Hie_FlushCachedNumNodesInHieLvl (Hie_CRS,Hie_INS);	// Number of courses in institution
      Hie_FlushCacheNumUsrsWhoClaimToBelongTo (Hie_INS);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Institution_X_removed,
                       Ins_EditingIns->FullName);

      Ins_EditingIns->HieCod = -1L;	// To not showing button to go to institution
     }
  }

/*****************************************************************************/
/********************* Change the name of an institution *********************/
/*****************************************************************************/

void Ins_RenameInsShort (void)
  {
   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Rename institution *****/
   Ins_EditingIns->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);
   Ins_RenameInstitution (Ins_EditingIns,Nam_SHRT_NAME);
  }

void Ins_RenameInsFull (void)
  {
   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Rename institution *****/
   Ins_EditingIns->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);
   Ins_RenameInstitution (Ins_EditingIns,Nam_FULL_NAME);
  }

/*****************************************************************************/
/******************** Change the name of an institution **********************/
/*****************************************************************************/

void Ins_RenameInstitution (struct Hie_Node *Ins,Nam_ShrtOrFullName_t ShrtOrFull)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Nam_Fields[Nam_NUM_SHRT_FULL_NAMES];
   extern unsigned Nam_MaxBytes[Nam_NUM_SHRT_FULL_NAMES];
   extern const char *Txt_X_already_exists;
   extern const char *Txt_The_institution_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   char *CurrentName[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = Ins->ShrtName,
      [Nam_FULL_NAME] = Ins->FullName,
     };
   char NewName[Nam_MAX_BYTES_FULL_NAME + 1];

   /***** Get the new name for the institution from form *****/
   Nam_GetParShrtOrFullName (ShrtOrFull,NewName);

   /***** Get from the database the old names of the institution *****/
   Hie_GetDataByCod[Hie_INS] (Ins);

   /***** Check if new name is empty *****/
   if (NewName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentName[ShrtOrFull],NewName))	// Different names
         /***** If institution was not in database... *****/
         switch (Ins_DB_CheckIfInsNameExistsInCty (Nam_Fields[ShrtOrFull],
						   NewName,Ins->HieCod,
						   Gbl.Hierarchy.Node[Hie_CTY].HieCod,
						   0))	// Unused
	   {
	    case Exi_EXISTS:
	       Ale_CreateAlert (Ale_WARNING,NULL,Txt_X_already_exists,NewName);
	       break;
	    case Exi_DOES_NOT_EXIST:
	    default:
	       /* Update the table changing old name by new name */
	       Ins_UpdateInsNameDB (Ins->HieCod,
				    Nam_Fields[ShrtOrFull],NewName);

	       /* Create message to show the change made */
	       Ale_CreateAlert (Ale_SUCCESS,NULL,
				Txt_The_institution_X_has_been_renamed_as_Y,
				CurrentName[ShrtOrFull],NewName);

	       /* Change current institution name in order to display it properly */
	       Str_Copy (CurrentName[ShrtOrFull],NewName,Nam_MaxBytes[ShrtOrFull]);
	       break;
	   }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,Txt_The_name_X_has_not_changed,
                          CurrentName[ShrtOrFull]);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
  }

/*****************************************************************************/
/************ Update institution name in table of institutions ***************/
/*****************************************************************************/

static void Ins_UpdateInsNameDB (long InsCod,const char *FldName,const char *NewName)
  {
   /***** Update institution changing old name by new name */
   Ins_DB_UpdateInsName (InsCod,FldName,NewName);

   /***** Flush caches *****/
   Ins_FlushCacheFullNameAndCtyOfInstitution ();
  }

/*****************************************************************************/
/********************** Change the URL of a institution **********************/
/*****************************************************************************/

void Ins_ChangeInsWWW (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[WWW_MAX_BYTES_WWW + 1];

   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the institution */
   Ins_EditingIns->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /* Get the new WWW for the institution */
   Par_GetParText ("WWW",NewWWW,WWW_MAX_BYTES_WWW);

   /***** Get data of institution *****/
   Hie_GetDataByCod[Hie_INS] (Ins_EditingIns);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ins_DB_UpdateInsWWW (Ins_EditingIns->HieCod,NewWWW);
      Str_Copy (Ins_EditingIns->WWW,NewWWW,sizeof (Ins_EditingIns->WWW) - 1);

      /***** Write message to show the change made
	     and put button to go to institution changed *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
		       Txt_The_new_web_address_is_X,
		       NewWWW);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
  }

/*****************************************************************************/
/******************** Change the status of an institution ********************/
/*****************************************************************************/

void Ins_ChangeInsStatus (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_status_of_the_institution_X_has_changed;
   Hie_Status_t Status;

   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Get parameters from form *****/
   /* Get institution code */
   Ins_EditingIns->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /* Get parameter with status */
   Status = Hie_GetParStatus ();	// New status

   /***** Get data of institution *****/
   Hie_GetDataByCod[Hie_INS] (Ins_EditingIns);

   /***** Update status *****/
   Ins_DB_UpdateInsStatus (Ins_EditingIns->HieCod,Status);
   Ins_EditingIns->Status = Status;

   /***** Create message to show the change made
	  and put button to go to institution changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
		    Txt_The_status_of_the_institution_X_has_changed,
		    Ins_EditingIns->ShrtName);
  }

/*****************************************************************************/
/****** Show alerts after changing an institution and continue editing *******/
/*****************************************************************************/

void Ins_ContEditAfterChgIns (void)
  {
   /***** Write message to show the change made
	  and put button to go to institution changed *****/
   Ins_ShowAlertAndButtonToGoToIns ();

   /***** Show the form again *****/
   Ins_EditInstitutionsInternal ();

   /***** Institution destructor *****/
   Ins_EditingInstitutionDestructor ();
  }

/*****************************************************************************/
/*************** Write message to show the change made       *****************/
/*************** and put button to go to institution changed *****************/
/*****************************************************************************/

static void Ins_ShowAlertAndButtonToGoToIns (void)
  {
   // If the institution being edited is different to the current one...
   if (Ins_EditingIns->HieCod != Gbl.Hierarchy.Node[Hie_INS].HieCod)
      /***** Alert with button to go to institution *****/
      Ale_ShowLastAlertAndButtonGo (ActSeeCtr,
                                    Ins_PutParInsCod,&Ins_EditingIns->HieCod,
				    Ins_EditingIns->ShrtName);
   else
      /***** Alert *****/
      Ale_ShowAlerts (NULL);
  }

/*****************************************************************************/
/****************** Put a form to create a new institution *******************/
/*****************************************************************************/

static void Ins_PutFormToCreateInstitution (void)
  {
   Act_Action_t NextAction = ActUnk;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];
   struct Hie_Node Node;

   /***** Set action depending on role *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      NextAction = ActNewIns;
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      NextAction = ActReqIns;
   else
      Err_NoPermissionExit ();

   /***** Begin form to create *****/
   Frm_BeginFormTable (NextAction,NULL,NULL,NULL,"TBL_SCROLL");

      /***** Write heading *****/
      Ins_PutHeadInstitutionsForEdition ();

      HTM_TR_Begin (NULL);

	 /***** Column to remove institution, disabled here *****/
	 HTM_TD_Begin ("class=\"BM\"");
	 HTM_TD_End ();

	 /***** Institution code *****/
	 HTM_TD_Begin ("class=\"CODE\"");
	 HTM_TD_End ();

	 /***** Institution logo *****/
	 HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Ins_EditingIns->FullName);
	    Node.HieCod = -1L;
	    Node.ShrtName[0] = '\0';
	    Lgo_DrawLogo (Hie_INS,&Node,"ICO20x20");
	 HTM_TD_End ();

	 /***** Institution short name and full name *****/
	 Names[Nam_SHRT_NAME] = Ins_EditingIns->ShrtName;
	 Names[Nam_FULL_NAME] = Ins_EditingIns->FullName;
	 Nam_NewShortAndFullNames (Names);

	 /***** Institution WWW *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_URL ("WWW",Ins_EditingIns->WWW,
			   HTM_REQUIRED,
			   "class=\"INPUT_WWW INPUT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Number of users who claim to belong to this institution ****/
	 HTM_TD_Unsigned (0);

	 /***** Number of centers *****/
	 HTM_TD_Unsigned (0);

	 /***** Number of users in courses of this institution ****/
	 HTM_TD_Unsigned (0);

	 /***** Institution requester *****/
	 HTM_TD_Begin ("class=\"LT DAT_%s INPUT_REQUESTER\"",The_GetSuffix ());
	    Usr_WriteAuthor (&Gbl.Usrs.Me.UsrDat,For_ENABLED);
	 HTM_TD_End ();

	 /***** Institution status *****/
	 HTM_TD_Empty (1);

      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE);
  }

/*****************************************************************************/
/**************** Write header with fields of an institution *****************/
/*****************************************************************************/

static void Ins_PutHeadInstitutionsForEdition (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Short_name_of_the_institution;
   extern const char *Txt_Full_name_of_the_institution;
   extern const char *Txt_WWW;
   extern const char *Txt_Users;
   extern const char *Txt_Centers_ABBREVIATION;
   extern const char *Txt_Requester;

   HTM_TR_Begin (NULL);

      HTM_TH_Empty (1);
      HTM_TH (Txt_Code                         ,HTM_HEAD_RIGHT);
      HTM_TH_Empty (1);
      HTM_TH (Txt_Short_name_of_the_institution,HTM_HEAD_LEFT );
      HTM_TH (Txt_Full_name_of_the_institution ,HTM_HEAD_LEFT );
      HTM_TH (Txt_WWW                          ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Users                        ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Centers_ABBREVIATION         ,HTM_HEAD_RIGHT);
      Usr_THTchsPlusStds ();
      HTM_TH (Txt_Requester                    ,HTM_HEAD_LEFT );
      HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/*************** Receive form to request a new institution *******************/
/*****************************************************************************/

void Ins_ReceiveReqIns (void)
  {
   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Receive form to request a new institution *****/
   Ins_ReceiveRequestOrCreateIns ((Hie_Status_t) Hie_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/***************** Receive form to create a new institution ******************/
/*****************************************************************************/

void Ins_ReceiveNewIns (void)
  {
   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Receive form to create a new institution *****/
   Ins_ReceiveRequestOrCreateIns ((Hie_Status_t) 0);
  }

/*****************************************************************************/
/*********** Receive form to request or create a new institution *************/
/*****************************************************************************/

static void Ins_ReceiveRequestOrCreateIns (Hie_Status_t Status)
  {
   extern const char *Txt_Created_new_institution_X;
   char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Get parameters from form *****/
   /* Set institution country */
   Ins_EditingIns->PrtCod = Gbl.Hierarchy.Node[Hie_CTY].HieCod;

   /* Get institution short name and full name */
   Names[Nam_SHRT_NAME] = Ins_EditingIns->ShrtName;
   Names[Nam_FULL_NAME] = Ins_EditingIns->FullName;
   Nam_GetParsShrtAndFullName (Names);

   /* Get institution URL */
   Par_GetParText ("WWW",Ins_EditingIns->WWW,WWW_MAX_BYTES_WWW);

   if (Ins_EditingIns->ShrtName[0] &&
       Ins_EditingIns->FullName[0])	// If there's a institution name
     {
      if (Ins_EditingIns->WWW[0])
        {
         /***** If name of institution was not in database... *****/
	 if (Nam_CheckIfNameExists (Ins_DB_CheckIfInsNameExistsInCty,
				    (const char **) Names,
				    -1L,
				    Gbl.Hierarchy.Node[Hie_CTY].HieCod,
				    0) == Exi_DOES_NOT_EXIST)	// Unused
           {
            Ins_EditingIns->HieCod = Ins_DB_CreateInstitution (Ins_EditingIns,Status);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_Created_new_institution_X,
			     Names[Nam_FULL_NAME]);
           }
        }
      else	// If there is not a web
         Ale_CreateAlertYouMustSpecifyTheWebAddress ();
     }
   else	// If there is not a institution name
      Ale_CreateAlertYouMustSpecifyShrtNameAndFullName ();
  }

/*****************************************************************************/
/****************** Get number of institutions with users ********************/
/*****************************************************************************/

unsigned Ins_GetCachedNumInssWithUsrs (Hie_Level_t HieLvl,Rol_Role_t Role)
  {
   static FigCch_FigureCached_t FigureInss[Rol_NUM_ROLES] =
     {
      [Rol_STD] = FigCch_NUM_INSS_WITH_STDS,	// Students
      [Rol_NET] = FigCch_NUM_INSS_WITH_NETS,	// Non-editing teachers
      [Rol_TCH] = FigCch_NUM_INSS_WITH_TCHS,	// Teachers
     };
   unsigned NumInssWithUsrs;
   long HieCod = Hie_GetHieCod (HieLvl);

   /***** Get number of institutions with users from cache *****/
   if (!FigCch_GetFigureFromCache (FigureInss[Role],HieLvl,HieCod,
				   FigCch_UNSIGNED,&NumInssWithUsrs))
     {
      /***** Get current number of institutions with users from database and update cache *****/
      NumInssWithUsrs = Ins_DB_GetNumInnsWithUsrs (HieLvl,HieCod,Role);
      FigCch_UpdateFigureIntoCache (FigureInss[Role],HieLvl,HieCod,
				    FigCch_UNSIGNED,&NumInssWithUsrs);
     }

   return NumInssWithUsrs;
  }

/*****************************************************************************/
/*************************** List institutions found *************************/
/*****************************************************************************/

void Ins_ListInssFound (MYSQL_RES **mysql_res,unsigned NumInss)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_HIERARCHY_SINGUL_abc[Hie_NUM_LEVELS];
   extern const char *Txt_HIERARCHY_PLURAL_abc[Hie_NUM_LEVELS];
   char *Title;
   unsigned NumIns;
   struct Hie_Node Ins;

   /***** List the institutions (one row per institution) *****/
   if (NumInss)
     {
      /***** Begin box and table *****/
      /* Number of institutions found */
      if (asprintf (&Title,"%u %s",NumInss,
                                   NumInss == 1 ? Txt_HIERARCHY_SINGUL_abc[Hie_INS] :
						  Txt_HIERARCHY_PLURAL_abc[Hie_INS]) < 0)
	 Err_NotEnoughMemoryExit ();
      Box_BoxTableBegin (Title,NULL,NULL,NULL,Box_NOT_CLOSABLE,2);
      free (Title);

      /***** Write heading *****/
      Ins_PutHeadInstitutionsForSeeing (false);	// Order not selectable

      /***** List the institutions (one row per institution) *****/
      for (NumIns  = 1, The_ResetRowColor ();
	   NumIns <= NumInss;
	   NumIns++, The_ChangeRowColor ())
	{
	 /* Get next institution */
	 Ins.HieCod = DB_GetNextCode (*mysql_res);

	 /* Get data of institution */
	 Hie_GetDataByCod[Hie_INS] (&Ins);

	 /* Write data of this institution */
	 Ins_ListOneInstitutionForSeeing (&Ins,NumIns);
	}

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/********************** Institution constructor/destructor *******************/
/*****************************************************************************/

static void Ins_EditingInstitutionConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Ins_EditingIns != NULL)
      Err_WrongInstitExit ();

   /***** Allocate memory for institution *****/
   if ((Ins_EditingIns = malloc (sizeof (*Ins_EditingIns))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Reset institution *****/
   Ins_EditingIns->HieCod         = -1L;
   Ins_EditingIns->PrtCod      = -1L;
   Ins_EditingIns->ShrtName[0] = '\0';
   Ins_EditingIns->FullName[0] = '\0';
   Ins_EditingIns->WWW[0]      = '\0';
  }

static void Ins_EditingInstitutionDestructor (void)
  {
   /***** Free memory used for institution *****/
   if (Ins_EditingIns != NULL)
     {
      free (Ins_EditingIns);
      Ins_EditingIns = NULL;
     }
  }

/*****************************************************************************/
/********************* Form to go to institution map *************************/
/*****************************************************************************/

static void Ins_FormToGoToMap (struct Hie_Node *Ins)
  {
   if (Ctr_DB_CheckIfMapExistsInIns (Ins->HieCod) == Exi_EXISTS)
     {
      Ins_EditingIns = Ins;	// Used to pass parameter with the code of the institution
      Lay_PutContextualLinkOnlyIcon (ActSeeInsInf,NULL,
                                     Ins_PutParInsCod,&Ins_EditingIns->HieCod,
				     "map-marker-alt.svg",Ico_BLACK);
     }
  }

/*****************************************************************************/
/***************** Write parameter with code of institution ******************/
/*****************************************************************************/

void Ins_PutParInsCod (void *InsCod)
  {
   if (InsCod)
      ParCod_PutPar (ParCod_Ins,*((long *) InsCod));
  }

/*****************************************************************************/
/****************** Get and show stats about institutions ********************/
/*****************************************************************************/

void Ins_GetAndShowInstitutionsStats (Hie_Level_t HieLvl)
  {
   extern const char *Hlp_ANALYTICS_Figures_institutions;
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   Pho_ShowPhotos_t ShowPhotos;
   struct Fig_Figures Figures;

   /***** Begin box *****/
   Box_BoxBegin (Txt_HIERARCHY_PLURAL_Abc[Hie_INS],NULL,NULL,
                 Hlp_ANALYTICS_Figures_institutions,Box_NOT_CLOSABLE);

      /***** Form to select type of list used to display degree photos *****/
      Set_GetAndUpdatePrefsAboutUsrList (&ShowPhotos);
      Figures.HieLvl     = HieLvl;
      Figures.FigureType = Fig_INSTITS;
      Usr_ShowFormsToSelectUsrListType (Fig_PutParsFigures,&Figures,
				        NULL,ShowPhotos);

      /***** Institutions ordered by number of centers *****/
      Ins_GetAndShowInssOrderedByNumCtrs (HieLvl,ShowPhotos);

      /***** Institutions ordered by number of degrees *****/
      HTM_BR ();
      Ins_GetAndShowInssOrderedByNumDegs (HieLvl,ShowPhotos);

      /***** Institutions ordered by number of courses *****/
      HTM_BR ();
      Ins_GetAndShowInssOrderedByNumCrss (HieLvl,ShowPhotos);

      /***** Institutions ordered by number of users in courses *****/
      HTM_BR ();
      Ins_GetAndShowInssOrderedByNumUsrsInCrss (HieLvl,ShowPhotos);

      /***** Institutions ordered by number of users who claim to belong to them *****/
      HTM_BR ();
      Ins_GetAndShowInssOrderedByNumUsrsWhoClaimToBelongToThem (HieLvl,ShowPhotos);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/**** Get and show stats about institutions ordered by number of centers *****/
/*****************************************************************************/

static void Ins_GetAndShowInssOrderedByNumCtrs (Hie_Level_t HieLvl,
						Pho_ShowPhotos_t ShowPhotos)
  {
   extern const char *Txt_Institutions_by_number_of_centers;
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   MYSQL_RES *mysql_res;
   unsigned NumInss;

   Box_BoxBegin (Txt_Institutions_by_number_of_centers,NULL,NULL,
                 NULL,Box_NOT_CLOSABLE);

	 /***** Get institutions ordered by number of centers *****/
	 NumInss = Ins_DB_GetInssOrderedByNumCtrs (&mysql_res,HieLvl);

	 /***** Show institutions *****/
	 Ins_ShowInss (&mysql_res,NumInss,Txt_HIERARCHY_PLURAL_Abc[Hie_CTR],ShowPhotos);

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);

   Box_BoxEnd ();
  }

/*****************************************************************************/
/**** Get and show stats about institutions ordered by number of degrees *****/
/*****************************************************************************/

static void Ins_GetAndShowInssOrderedByNumDegs (Hie_Level_t HieLvl,
						Pho_ShowPhotos_t ShowPhotos)
  {
   extern const char *Txt_Institutions_by_number_of_degrees;
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   MYSQL_RES *mysql_res;
   unsigned NumInss;

   Box_BoxBegin (Txt_Institutions_by_number_of_degrees,NULL,NULL,
                 NULL,Box_NOT_CLOSABLE);
      HTM_TABLE_Begin ("TBL_SCROLL");

	 /***** Get institutions ordered by number of degrees *****/
	 NumInss = Ins_DB_GetInssOrderedByNumDegs (&mysql_res,HieLvl);

	 /***** Show institutions *****/
	 Ins_ShowInss (&mysql_res,NumInss,Txt_HIERARCHY_PLURAL_Abc[Hie_DEG],ShowPhotos);

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);

      HTM_TABLE_End ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/**** Get and show stats about institutions ordered by number of courses *****/
/*****************************************************************************/

static void Ins_GetAndShowInssOrderedByNumCrss (Hie_Level_t HieLvl,
						Pho_ShowPhotos_t ShowPhotos)
  {
   extern const char *Txt_Institutions_by_number_of_courses;
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   MYSQL_RES *mysql_res;
   unsigned NumInss;

   Box_BoxBegin (Txt_Institutions_by_number_of_courses,NULL,NULL,
                 NULL,Box_NOT_CLOSABLE);
      HTM_TABLE_Begin ("TBL_SCROLL");

	 /***** Get institutions ordered by number of courses *****/
	 NumInss = Ins_DB_GetInssOrderedByNumCrss (&mysql_res,HieLvl);

	 /***** Show institutions *****/
	 Ins_ShowInss (&mysql_res,NumInss,Txt_HIERARCHY_PLURAL_Abc[Hie_CRS],ShowPhotos);

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);

      HTM_TABLE_End ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/***** Get and show stats about institutions ordered by users in courses *****/
/*****************************************************************************/

static void Ins_GetAndShowInssOrderedByNumUsrsInCrss (Hie_Level_t HieLvl,
						      Pho_ShowPhotos_t ShowPhotos)
  {
   extern const char *Txt_Institutions_by_number_of_users_in_courses;
   extern const char *Txt_Users;
   MYSQL_RES *mysql_res;
   unsigned NumInss;

   Box_BoxBegin (Txt_Institutions_by_number_of_users_in_courses,NULL,NULL,
                 NULL,Box_NOT_CLOSABLE);
      HTM_TABLE_Begin ("TBL_SCROLL");

	 /***** Get institutions ordered by number of users in courses *****/
	 NumInss = Ins_DB_GetInssOrderedByNumUsrsInCrss (&mysql_res,HieLvl);

	 /***** Show institutions *****/
	 Ins_ShowInss (&mysql_res,NumInss,Txt_Users,ShowPhotos);

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);

      HTM_TABLE_End ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************* Get and show stats about institutions ordered by **************/
/************* number of users who claim to belong to them      **************/
/*****************************************************************************/

static void Ins_GetAndShowInssOrderedByNumUsrsWhoClaimToBelongToThem (Hie_Level_t HieLvl,
								      Pho_ShowPhotos_t ShowPhotos)
  {
   extern const char *Txt_Institutions_by_number_of_users_who_claim_to_belong_to_them;
   extern const char *Txt_Users;
   MYSQL_RES *mysql_res;
   unsigned NumInss;

   Box_BoxBegin (Txt_Institutions_by_number_of_users_who_claim_to_belong_to_them,
                 NULL,NULL,
                 NULL,Box_NOT_CLOSABLE);
      HTM_TABLE_Begin ("TBL_SCROLL");

	 /***** Get institutions ordered by number of users who claim to belong to them *****/
	 NumInss = Ins_DB_GetInssOrderedByNumUsrsWhoClaimToBelongToThem (&mysql_res,HieLvl);

	 /***** Show institutions *****/
	 Ins_ShowInss (&mysql_res,NumInss,Txt_Users,ShowPhotos);

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);

      HTM_TABLE_End ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/****************** Get and show stats about institutions ********************/
/*****************************************************************************/

static void Ins_ShowInss (MYSQL_RES **mysql_res,unsigned NumInss,
		          const char *TxtFigure,Pho_ShowPhotos_t ShowPhotos)
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   unsigned NumIns;
   unsigned NumOrder;
   unsigned NumberLastRow;
   unsigned NumberThisRow;
   struct Hie_Node Ins;

   /***** Query database *****/
   if (NumInss)
     {
      /* Draw the classphoto/list */
      switch (Gbl.Usrs.Me.ListType)
	{
	 case Set_USR_LIST_AS_CLASS_PHOTO:
	    /***** Draw institutions as a class photo *****/
	    HTM_DIV_Begin ("class=\"CLASSPHOTO_CONT\"");

	       for (NumIns = 0;
		    NumIns < NumInss;
		    NumIns++)
		 {
		  HTM_DIV_Begin ("class=\"CLASSPHOTO FORM_IN_%s CB\"",
				 The_GetSuffix ());

		     /***** Get institution data and statistic *****/
		     NumberThisRow = Ins_GetInsAndStat (&Ins,*mysql_res);

		     /***** Write link to institution *****/
		     Ins_DrawInstitutionLogoWithLink (&Ins,ShowPhotos,"ICO40x40");
		     HTM_Unsigned (NumberThisRow);

	          HTM_DIV_End ();
		 }

	    HTM_DIV_End ();
	    break;
	 case Set_USR_LIST_AS_LISTING:
	    /***** Draw institutions as a list *****/
	    HTM_TABLE_Begin ("TBL_SCROLL");

	       HTM_TR_Begin (NULL);
		  HTM_TH_Empty (1);
		  HTM_TH (Txt_HIERARCHY_SINGUL_Abc[Hie_INS],HTM_HEAD_LEFT );
		  HTM_TH (TxtFigure      		   ,HTM_HEAD_RIGHT);
	       HTM_TR_End ();

	       for (NumIns  = 1, NumOrder = 1, NumberLastRow = 0;
		    NumIns <= NumInss;
		    NumIns++)
		 {
		  /***** Get institution data and statistic *****/
		  NumberThisRow = Ins_GetInsAndStat (&Ins,*mysql_res);

		  HTM_TR_Begin (NULL);

		     /***** Number of order *****/
		     if (NumberThisRow != NumberLastRow)
			NumOrder = NumIns;
		     HTM_TD_Unsigned (NumOrder);

		     /***** Write link to institution *****/
		     HTM_TD_Begin ("class=\"LM FORM_IN_%s\"",The_GetSuffix ());
			/* Icon and name of this institution */
			Frm_BeginForm (ActSeeInsInf);
			   ParCod_PutPar (ParCod_Ins,Ins.HieCod);
			   HTM_BUTTON_Submit_Begin (Ins.ShrtName,NULL,
						    "class=\"LM BT_LINK\"");
			      if (ShowPhotos == Pho_PHOTOS_SHOW)
				{
				 Lgo_DrawLogo (Hie_INS,&Ins,"ICO40x40");
				 HTM_NBSP ();
				}
			      HTM_Txt (Ins.FullName);
			   HTM_BUTTON_End ();
			Frm_EndForm ();
		     HTM_TD_End ();

		     /***** Write statistic *****/
		     HTM_TD_Unsigned (NumberThisRow);

		  HTM_TR_End ();

		  NumberLastRow = NumberThisRow;
		 }

	    HTM_TABLE_End ();
	    break;
	 default:
	    break;
	}
     }
  }

/*****************************************************************************/
/******************** Get institution data and statistic *********************/
/*****************************************************************************/

static unsigned Ins_GetInsAndStat (struct Hie_Node *Ins,MYSQL_RES *mysql_res)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   MYSQL_ROW row;
   unsigned NumberThisRow;

   /***** Get next institution *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get data of this institution (row[0]) *****/
   Ins->HieCod = Str_ConvertStrCodToLongCod (row[0]);
   if (!Hie_GetDataByCod[Hie_INS] (Ins))
      Err_WrongInstitExit ();

   /***** Get statistic (row[1]) *****/
   if (sscanf (row[1],"%u",&NumberThisRow) != 1)
      Err_ShowErrorAndExit ("Error in statistic");

   return NumberThisRow;
  }
