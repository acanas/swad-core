// swad_institution.c: institutions

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
#include "swad_hierarchy_level.h"
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

static struct Ins_Instit *Ins_EditingIns = NULL;	// Static variable to keep the institution being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ins_ListInstitutions (void);
static void Ins_PutIconsListingInstitutions (__attribute__((unused)) void *Args);
static void Ins_PutIconToEditInstitutions (void);
static void Ins_ListOneInstitutionForSeeing (struct Ins_Instit *Ins,unsigned NumIns);
static void Ins_PutHeadInstitutionsForSeeing (bool OrderSelectable);
static void Ins_GetParInsOrder (void);

static void Ins_EditInstitutionsInternal (void);
static void Ins_PutIconsEditingInstitutions (__attribute__((unused)) void *Args);

static void Ins_GetInstitDataFromRow (MYSQL_RES *mysql_res,
                                      struct Ins_Instit *Ins,
                                      bool GetNumUsrsWhoClaimToBelongToIns);

static void Ins_ListInstitutionsForEdition (void);
static bool Ins_CheckIfICanEdit (struct Ins_Instit *Ins);

static void Ins_UpdateInsNameDB (long InsCod,const char *FldName,const char *NewInsName);

static void Ins_ShowAlertAndButtonToGoToIns (void);

static void Ins_PutFormToCreateInstitution (void);
static void Ins_PutHeadInstitutionsForEdition (void);
static void Ins_ReceiveFormRequestOrCreateIns (Hie_Status_t Status);

static void Ins_EditingInstitutionConstructor ();
static void Ins_EditingInstitutionDestructor ();

static void Ins_FormToGoToMap (struct Ins_Instit *Ins);

static void Ins_GetAndShowInssOrderedByNumCtrs (void);
static void Ins_GetAndShowInssOrderedByNumDegs (void);
static void Ins_GetAndShowInssOrderedByNumCrss (void);
static void Ins_GetAndShowInssOrderedByNumUsrsInCrss (void);
static void Ins_GetAndShowInssOrderedByNumUsrsWhoClaimToBelongToThem (void);
static void Ins_ShowInss (MYSQL_RES **mysql_res,unsigned NumInss,
		          const char *TxtFigure);
static unsigned Ins_GetInsAndStat (struct Ins_Instit *Ins,MYSQL_RES *mysql_res);

/*****************************************************************************/
/***************** List institutions with pending centers ********************/
/*****************************************************************************/

void Ins_SeeInsWithPendingCtrs (void)
  {
   extern const char *Hlp_SYSTEM_Pending;
   extern const char *Txt_Institutions_with_pending_centers;
   extern const char *Txt_Institution;
   extern const char *Txt_Centers_ABBREVIATION;
   extern const char *Txt_There_are_no_institutions_with_requests_for_centers_to_be_confirmed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumInss = 0;
   unsigned NumIns;
   struct Ins_Instit Ins;
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
      Box_BoxTableBegin (NULL,Txt_Institutions_with_pending_centers,
                         NULL,NULL,
                         Hlp_SYSTEM_Pending,Box_NOT_CLOSABLE,2);

	 /***** Write heading *****/
	 HTM_TR_Begin (NULL);
	    HTM_TH (Txt_Institution         ,HTM_HEAD_LEFT );
	    HTM_TH (Txt_Centers_ABBREVIATION,HTM_HEAD_RIGHT);
	 HTM_TR_End ();

	 /***** List the institutions *****/
	 for (NumIns = 0, The_ResetRowColor ();
	      NumIns < NumInss;
	      NumIns++, The_ChangeRowColor ())
	   {
	    /* Get next center */
	    row = mysql_fetch_row (mysql_res);

	    /* Get institution code (row[0]) */
	    Ins.Cod = Str_ConvertStrCodToLongCod (row[0]);
	    BgColor = (Ins.Cod == Gbl.Hierarchy.Ins.Cod) ? "BG_HIGHLIGHT" :
								 The_GetColorRows ();

	    /* Get data of institution */
	    Ins_GetInstitDataByCod (&Ins);

	    /* Institution logo and name */
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"LM DAT_%s NOWRAP %s\"",
	                     The_GetSuffix (),BgColor);
		   Ins_DrawInstitLogoAndNameWithLink (&Ins,ActSeeCtr,"CM");
	       HTM_TD_End ();

	       /* Number of pending centers (row[1]) */
	       HTM_TD_Begin ("class=\"RM DAT_%s %s\"",
	                     The_GetSuffix (),BgColor);
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

void Ins_DrawInstitutionLogoWithLink (struct Ins_Instit *Ins,unsigned Size)
  {
   bool PutLink = !Frm_CheckIfInside ();	// Don't put link to institution if already inside a form

   if (PutLink)
     {
      Frm_BeginForm (ActSeeInsInf);
	 ParCod_PutPar (ParCod_Ins,Ins->Cod);
	 HTM_BUTTON_Submit_Begin (Ins->FullName,"class=\"BT_LINK\"");
     }
   Lgo_DrawLogo (HieLvl_INS,Ins->Cod,Ins->FullName,Size,NULL);
   if (PutLink)
     {
	 HTM_BUTTON_End ();
      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/****************** Draw institution logo and name with link *****************/
/*****************************************************************************/

void Ins_DrawInstitLogoAndNameWithLink (struct Ins_Instit *Ins,Act_Action_t Action,
                                        const char *ClassLogo)
  {
   /***** Begin form *****/
   Frm_BeginFormGoTo (Action);
      ParCod_PutPar (ParCod_Ins,Ins->Cod);

      /***** Link to action *****/
      HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Ins->FullName),
                               "class=\"BT_LINK LT\"");
      Str_FreeGoToTitle ();

	 /***** Institution logo and name *****/
	 Lgo_DrawLogo (HieLvl_INS,Ins->Cod,Ins->ShrtName,16,ClassLogo);
	 HTM_TxtF ("&nbsp;%s",Ins->FullName);

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
   if (Gbl.Hierarchy.Cty.Cod > 0)
     {
      /***** Get parameter with the type of order in the list of institutions *****/
      Ins_GetParInsOrder ();

      /***** Get list of institutions *****/
      Ins_GetFullListOfInstitutions (Gbl.Hierarchy.Cty.Cod);

      /***** Write menu to select country *****/
      Hie_WriteMenuHierarchy ();

      /***** List institutions *****/
      Ins_ListInstitutions ();

      /***** Free list of institutions *****/
      Ins_FreeListInstitutions ();
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
                 Gbl.Hierarchy.Cty.FullName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (NULL,Title,Ins_PutIconsListingInstitutions,NULL,
                 Hlp_COUNTRY_Institutions,Box_NOT_CLOSABLE);
   free (Title);

      if (Gbl.Hierarchy.Inss.Num)	// There are institutions in the current country
	{
	 /***** Begin table *****/
	 HTM_TABLE_BeginWideMarginPadding (2);
	 Ins_PutHeadInstitutionsForSeeing (true);	// Order selectable

	    /***** Write all institutions and their nuber of users *****/
	    for (NumIns = 0, The_ResetRowColor ();
		 NumIns < Gbl.Hierarchy.Inss.Num;
		 NumIns++, The_ChangeRowColor ())
	       Ins_ListOneInstitutionForSeeing (&(Gbl.Hierarchy.Inss.Lst[NumIns]),
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
   if (Hie_CheckIfICanEdit ())
      Ins_PutIconToEditInstitutions ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_INSTITS);
  }

/*****************************************************************************/
/******************** Put link (form) to edit institutions *******************/
/*****************************************************************************/

static void Ins_PutIconToEditInstitutions (void)
  {
   Ico_PutContextualIconToEdit (ActEdiIns,NULL,
                                NULL,NULL);
  }

/*****************************************************************************/
/********************** List one institution for seeing **********************/
/*****************************************************************************/

static void Ins_ListOneInstitutionForSeeing (struct Ins_Instit *Ins,unsigned NumIns)
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
   BgColor = (Ins->Cod == Gbl.Hierarchy.Ins.Cod) ? "BG_HIGHLIGHT" :
                                                         The_GetColorRows ();

   HTM_TR_Begin (NULL);

      /***** Number of institution in this list *****/
      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (NumIns);
      HTM_TD_End ();

      /***** Institution logo and name *****/
      HTM_TD_Begin ("class=\"LM %s_%s %s\"",
                    TxtClassStrong,The_GetSuffix (),BgColor);
	 Ins_DrawInstitLogoAndNameWithLink (Ins,ActSeeCtr,"CM");
      HTM_TD_End ();

      /***** Number of users who claim to belong to this institution *****/
      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Ins_GetCachedNumUsrsWhoClaimToBelongToIns (Ins));
      HTM_TD_End ();

      /***** Other stats *****/
      /* Number of centers in this institution */
      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Ctr_GetCachedNumCtrsInIns (Ins->Cod));
      HTM_TD_End ();

      /* Number of degrees in this institution */
      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Deg_GetCachedNumDegsInIns (Ins->Cod));
      HTM_TD_End ();

      /* Number of courses in this institution */
      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Crs_GetCachedNumCrssInIns (Ins->Cod));
      HTM_TD_End ();

      /* Number of departments in this institution */
      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Dpt_GetNumDptsInIns (Ins->Cod));
      HTM_TD_End ();

      /* Number of users in courses of this institution */
      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Enr_GetCachedNumUsrsInCrss (HieLvl_INS,Ins->Cod,
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
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Centers_ABBREVIATION;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_Courses_ABBREVIATION;
   extern const char *Txt_Departments_ABBREVIATION;
   Ins_Order_t Order;
   static HTM_HeadAlign Align[Ins_NUM_ORDERS] =
     {
      [Ins_ORDER_BY_INSTITUTION] = HTM_HEAD_LEFT,
      [Ins_ORDER_BY_NUM_USRS   ] = HTM_HEAD_RIGHT
     };

   HTM_TR_Begin (NULL);

      HTM_TH_Empty (1);
      for (Order  = (Ins_Order_t) 0;
	   Order <= (Ins_Order_t) (Ins_NUM_ORDERS - 1);
	   Order++)
	{
         HTM_TH_Begin (Align[Order]);
	    if (OrderSelectable)
	      {
	       Frm_BeginForm (ActSeeIns);
		  Par_PutParUnsigned (NULL,"Order",(unsigned) Order);
		  HTM_BUTTON_Submit_Begin (Txt_INSTITUTIONS_HELP_ORDER[Order],
		                           "class=\"BT_LINK\"");
		     if (Order == Gbl.Hierarchy.Inss.SelectedOrder)
			HTM_U_Begin ();
	      }
	    HTM_Txt (Txt_INSTITUTIONS_ORDER[Order]);
	    if (OrderSelectable)
	      {
		     if (Order == Gbl.Hierarchy.Inss.SelectedOrder)
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
      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 HTM_TxtF ("%s+",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
	 HTM_BR ();
	 HTM_Txt (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
      HTM_TH_End ();
      HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******* Get parameter with the type or order in list of institutions ********/
/*****************************************************************************/

static void Ins_GetParInsOrder (void)
  {
   Gbl.Hierarchy.Inss.SelectedOrder = (Ins_Order_t)
				      Par_GetParUnsignedLong ("Order",
							      0,
							      Ins_NUM_ORDERS - 1,
							      (unsigned long) Ins_ORDER_DEFAULT);
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
   Ins_GetFullListOfInstitutions (Gbl.Hierarchy.Cty.Cod);

   /***** Write menu to select country *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   if (asprintf (&Title,Txt_Institutions_of_COUNTRY_X,
                 Gbl.Hierarchy.Cty.FullName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (NULL,Title,Ins_PutIconsEditingInstitutions,NULL,
                 Hlp_COUNTRY_Institutions,Box_NOT_CLOSABLE);
   free (Title);

      /***** Put a form to create a new institution *****/
      Ins_PutFormToCreateInstitution ();

      /***** Forms to edit current institutions *****/
      if (Gbl.Hierarchy.Inss.Num)
	 Ins_ListInstitutionsForEdition ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of institutions *****/
   Ins_FreeListInstitutions ();
  }

/*****************************************************************************/
/************ Put contextual icons in edition of institutions ****************/
/*****************************************************************************/

static void Ins_PutIconsEditingInstitutions (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view institutions *****/
   Ico_PutContextualIconToView (ActSeeIns,NULL,
				NULL,NULL);

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
   if ((Gbl.Hierarchy.Inss.Num = Ins_DB_GetInssInCtyOrderedByFullName (&mysql_res,CtyCod))) // Institutions found...
     {
      /***** Create list with institutions *****/
      if ((Gbl.Hierarchy.Inss.Lst = calloc ((size_t) Gbl.Hierarchy.Inss.Num,
                                            sizeof (*Gbl.Hierarchy.Inss.Lst))) == NULL)
          Err_NotEnoughMemoryExit ();

      /***** Get the institutions *****/
      for (NumIns = 0;
	   NumIns < Gbl.Hierarchy.Inss.Num;
	   NumIns++)
         /* Get institution data */
         Ins_GetInstitDataFromRow (mysql_res,&Gbl.Hierarchy.Inss.Lst[NumIns],
                                   false);	// Don't get number of users who claim to belong to this institution
     }
   else
      Gbl.Hierarchy.Inss.Lst = NULL;

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
   if ((Gbl.Hierarchy.Inss.Num = Ins_DB_GetFullListOfInssInCty (&mysql_res,CtyCod))) // Institutions found...
     {
      /***** Create list with institutions *****/
      if ((Gbl.Hierarchy.Inss.Lst = calloc ((size_t) Gbl.Hierarchy.Inss.Num,
                                            sizeof (*Gbl.Hierarchy.Inss.Lst))) == NULL)
          Err_NotEnoughMemoryExit ();

      /***** Get the institutions *****/
      for (NumIns = 0;
	   NumIns < Gbl.Hierarchy.Inss.Num;
	   NumIns++)
         /* Get institution data */
         Ins_GetInstitDataFromRow (mysql_res,&Gbl.Hierarchy.Inss.Lst[NumIns],
                                   true);	// Get number of users who claim to belong to this institution
     }
   else
      Gbl.Hierarchy.Inss.Lst = NULL;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Write institution full name and country *******************/
/*****************************************************************************/
// If ClassLink == NULL ==> do not put link

void Ins_WriteInstitutionNameAndCty (long InsCod)
  {
   struct Ins_Instit Ins;
   char CtyName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1];

   /***** Get institution short name and country name *****/
   Ins.Cod = InsCod;
   Ins_GetShrtNameAndCtyOfInstitution (&Ins,CtyName);

   /***** Write institution short name and country name *****/
   HTM_TxtF ("%s&nbsp;(%s)",Ins.ShrtName,CtyName);
  }

/*****************************************************************************/
/************************* Get data of an institution ************************/
/*****************************************************************************/

bool Ins_GetInstitDataByCod (struct Ins_Instit *Ins)
  {
   MYSQL_RES *mysql_res;
   bool InsFound = false;

   /***** Clear data *****/
   Ins->PrtCod          = -1L;
   Ins->Status          = (Hie_Status_t) 0;
   Ins->RequesterUsrCod = -1L;
   Ins->ShrtName[0]     =
   Ins->FullName[0]     =
   Ins->WWW[0]          = '\0';
   Ins->NumUsrsWhoClaimToBelong.Valid = false;

   /***** Check if institution code is correct *****/
   if (Ins->Cod > 0)
     {
      /***** Get data of an institution from database *****/
      if (Ins_DB_GetInsDataByCod (&mysql_res,Ins->Cod))	// Institution found...
	{
         /* Get institution data */
         Ins_GetInstitDataFromRow (mysql_res,Ins,
                                   false);	// Don't get number of users who claim to belong to this institution

         /* Set return value */
	 InsFound = true;
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return InsFound;
  }

/*****************************************************************************/
/********** Get data of a center from a row resulting of a query *************/
/*****************************************************************************/

static void Ins_GetInstitDataFromRow (MYSQL_RES *mysql_res,
                                      struct Ins_Instit *Ins,
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
   if ((Ins->Cod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
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
   Gbl.Cache.InstitutionShrtNameAndCty.InsCod      = -1L;
   Gbl.Cache.InstitutionShrtNameAndCty.ShrtName[0] = '\0';
   Gbl.Cache.InstitutionShrtNameAndCty.CtyName[0]  = '\0';
  }

void Ins_GetShrtNameAndCtyOfInstitution (struct Ins_Instit *Ins,
                                         char CtyName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** 1. Fast check: Trivial case *****/
   if (Ins->Cod <= 0)
     {
      Ins->ShrtName[0] = '\0';	// Empty name
      CtyName[0] = '\0';	// Empty name
      return;
     }

   /***** 2. Fast check: If cached... *****/
   if (Ins->Cod == Gbl.Cache.InstitutionShrtNameAndCty.InsCod)
     {
      Str_Copy (Ins->ShrtName,Gbl.Cache.InstitutionShrtNameAndCty.ShrtName,
		sizeof (Ins->ShrtName) - 1);
      Str_Copy (CtyName,Gbl.Cache.InstitutionShrtNameAndCty.CtyName,
		Cns_HIERARCHY_MAX_BYTES_FULL_NAME);
      return;
     }

   /***** 3. Slow: get short name and country of institution from database *****/
   Gbl.Cache.InstitutionShrtNameAndCty.InsCod = Ins->Cod;

   if (Ins_DB_GetInsShrtNameAndCty (&mysql_res,Ins->Cod) == 1)
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
	     Cns_HIERARCHY_MAX_BYTES_FULL_NAME);
  }

/*****************************************************************************/
/************************* Free list of institutions *************************/
/*****************************************************************************/

void Ins_FreeListInstitutions (void)
  {
   if (Gbl.Hierarchy.Inss.Lst)
     {
      /***** Free memory used by the list of institutions *****/
      free (Gbl.Hierarchy.Inss.Lst);
      Gbl.Hierarchy.Inss.Num = 0;
      Gbl.Hierarchy.Inss.Lst = NULL;
     }
  }

/*****************************************************************************/
/************************ Write selector of institution **********************/
/*****************************************************************************/

void Ins_WriteSelectorOfInstitution (void)
  {
   extern const char *Txt_Institution;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumInss;
   unsigned NumIns;
   long InsCod;

   /***** Begin form *****/
   Frm_BeginFormGoTo (ActSeeCtr);

      /***** Begin selector *****/
      if (Gbl.Hierarchy.Cty.Cod > 0)
	 HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
			   "id=\"ins\" name=\"ins\" class=\"HIE_SEL INPUT_%s\"",
			   The_GetSuffix ());
      else
	 HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,NULL,
			   "id=\"ins\" name=\"ins\" class=\"HIE_SEL INPUT_%s\""
			   " disabled=\"disabled\"",
			   The_GetSuffix ());

      HTM_OPTION (HTM_Type_STRING,"",
		  Gbl.Hierarchy.Ins.Cod < 0 ? HTM_OPTION_SELECTED :
						 HTM_OPTION_UNSELECTED,
		  HTM_OPTION_DISABLED,
		  "[%s]",Txt_Institution);

      if (Gbl.Hierarchy.Cty.Cod > 0)
	{
	 /***** Get institutions of current country *****/
	 NumInss = Ins_DB_GetInssInCtyOrderedByShrtName (&mysql_res,Gbl.Hierarchy.Cty.Cod);

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
			Gbl.Hierarchy.Ins.Cod > 0 &&
			InsCod == Gbl.Hierarchy.Ins.Cod ? HTM_OPTION_SELECTED :
							     HTM_OPTION_UNSELECTED,
			HTM_OPTION_ENABLED,
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
   unsigned NumIns;
   struct Ins_Instit *Ins;
   char WWW[Cns_MAX_BYTES_WWW + 1];
   struct Usr_Data UsrDat;
   bool ICanEdit;
   unsigned NumCtrs;
   unsigned NumUsrsIns;
   unsigned NumUsrsInCrssOfIns;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

      /***** Write heading *****/
      Ins_PutHeadInstitutionsForEdition ();

      /***** Write all institutions *****/
      for (NumIns = 0;
	   NumIns < Gbl.Hierarchy.Inss.Num;
	   NumIns++)
	{
	 Ins = &Gbl.Hierarchy.Inss.Lst[NumIns];

	 ICanEdit = Ins_CheckIfICanEdit (Ins);
	 NumCtrs = Ctr_GetNumCtrsInIns (Ins->Cod);
	 NumUsrsIns = Ins_GetNumUsrsWhoClaimToBelongToIns (Ins);
	 NumUsrsInCrssOfIns = Enr_GetNumUsrsInCrss (HieLvl_INS,Ins->Cod,
						    1 << Rol_STD |
						    1 << Rol_NET |
						    1 << Rol_TCH);	// Any user

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove institution */
	    HTM_TD_Begin ("class=\"BM\"");
	       if (!ICanEdit ||
		   NumCtrs ||		// Institution has centers
		   NumUsrsIns ||		// Institution has users
		   NumUsrsInCrssOfIns)	// Institution has users
		  // Institution has centers or users ==> deletion forbidden
		  Ico_PutIconRemovalNotAllowed ();
	       else
		  Ico_PutContextualIconToRemove (ActRemIns,NULL,
						 Hie_PutParOtherHieCod,&Ins->Cod);
	    HTM_TD_End ();

	    /* Institution code */
	    HTM_TD_Begin ("class=\"DAT_%s CODE\"",The_GetSuffix ());
	       HTM_Long (Ins->Cod);
	    HTM_TD_End ();

	    /* Institution logo */
	    HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Ins->FullName);
	       Lgo_DrawLogo (HieLvl_INS,Ins->Cod,Ins->ShrtName,20,NULL);
	    HTM_TD_End ();

	    /* Institution short name */
	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	       if (ICanEdit)
		 {
		  Frm_BeginForm (ActRenInsSho);
		     ParCod_PutPar (ParCod_OthHie,Ins->Cod);
		     HTM_INPUT_TEXT ("ShortName",Cns_HIERARCHY_MAX_CHARS_SHRT_NAME,Ins->ShrtName,
				     HTM_SUBMIT_ON_CHANGE,
				     "class=\"INPUT_SHORT_NAME INPUT_%s\"",
				     The_GetSuffix ());
		  Frm_EndForm ();
		 }
	       else
		  HTM_Txt (Ins->ShrtName);
	    HTM_TD_End ();

	    /* Institution full name */
	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	    if (ICanEdit)
	      {
	       Frm_BeginForm (ActRenInsFul);
		  ParCod_PutPar (ParCod_OthHie,Ins->Cod);
		  HTM_INPUT_TEXT ("FullName",Cns_HIERARCHY_MAX_CHARS_FULL_NAME,Ins->FullName,
				  HTM_SUBMIT_ON_CHANGE,
				  "class=\"INPUT_FULL_NAME INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	      }
	    else
	       HTM_Txt (Ins->FullName);
	    HTM_TD_End ();

	    /* Institution WWW */
	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	       if (ICanEdit)
		 {
		  Frm_BeginForm (ActChgInsWWW);
		     ParCod_PutPar (ParCod_OthHie,Ins->Cod);
		     HTM_INPUT_URL ("WWW",Ins->WWW,HTM_SUBMIT_ON_CHANGE,
				    "class=\"INPUT_WWW_NARROW INPUT_%s\""
				    " required=\"required\"",
				    The_GetSuffix ());
		  Frm_EndForm ();
		 }
	       else
		 {
		  Str_Copy (WWW,Ins->WWW,sizeof (WWW) - 1);
		  HTM_DIV_Begin ("class=\"EXTERNAL_WWW_SHORT\"");
		     HTM_A_Begin ("href=\"%s\" target=\"_blank\" title=\"%s\""
			          " class=\"DAT_%s\"",
				  Ins->WWW,
				  The_GetSuffix (),
				  Ins->WWW);
			HTM_Txt (WWW);
		     HTM_A_End ();
		  HTM_DIV_End ();
		 }
	    HTM_TD_End ();

	    /* Number of users who claim to belong to this institution */
	    HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	       HTM_Unsigned (NumUsrsIns);
	    HTM_TD_End ();

	    /* Number of centers */
	    HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	       HTM_Unsigned (NumCtrs);
	    HTM_TD_End ();

	    /* Number of users in courses of this institution */
	    HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	       HTM_Unsigned (NumUsrsInCrssOfIns);
	    HTM_TD_End ();

	    /* Institution requester */
	    HTM_TD_Begin ("class=\"LT DAT_%s INPUT_REQUESTER\"",
	                  The_GetSuffix ());
	       UsrDat.UsrCod = Ins->RequesterUsrCod;
	       Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							Usr_DONT_GET_PREFS,
							Usr_DONT_GET_ROLE_IN_CURRENT_CRS);
	       Usr_WriteAuthor (&UsrDat,Cns_ENABLED);
	    HTM_TD_End ();

	    /* Institution status */
	    Hie_WriteStatusCellEditable (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM,
	                                 Ins->Status,ActChgInsSta,Ins->Cod,
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

static bool Ins_CheckIfICanEdit (struct Ins_Instit *Ins)
  {
   return Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||		// I am a superuser
          ((Ins->Status & Hie_STATUS_BIT_PENDING) != 0 &&	// Institution is not yet activated
          Gbl.Usrs.Me.UsrDat.UsrCod == Ins->RequesterUsrCod);	// I am the requester
  }

/*****************************************************************************/
/**************************** Remove a institution ***************************/
/*****************************************************************************/

void Ins_RemoveInstitution (void)
  {
   extern const char *Txt_To_remove_an_institution_you_must_first_remove_all_centers_and_users_in_the_institution;
   extern const char *Txt_Institution_X_removed;
   char PathIns[PATH_MAX + 1];

   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Get institution code *****/
   Ins_EditingIns->Cod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /***** Get data of the institution from database *****/
   Ins_GetInstitDataByCod (Ins_EditingIns);

   /***** Check if this institution has users *****/
   if (!Ins_CheckIfICanEdit (Ins_EditingIns))
      Err_NoPermissionExit ();
   else if (Ctr_GetNumCtrsInIns (Ins_EditingIns->Cod))
      // Institution has centers ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_an_institution_you_must_first_remove_all_centers_and_users_in_the_institution);
   else if (Ins_GetNumUsrsWhoClaimToBelongToIns (Ins_EditingIns))
      // Institution has users ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_an_institution_you_must_first_remove_all_centers_and_users_in_the_institution);
   else if (Enr_GetNumUsrsInCrss (HieLvl_INS,Ins_EditingIns->Cod,
				  1 << Rol_STD |
				  1 << Rol_NET |
				  1 << Rol_TCH))	// Any user
      // Institution has users ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_an_institution_you_must_first_remove_all_centers_and_users_in_the_institution);
   else	// Institution has no users ==> remove it
     {
      /***** Remove all threads and posts in forums of the institution *****/
      For_DB_RemoveForums (HieLvl_INS,Ins_EditingIns->Cod);

      /***** Remove surveys of the institution *****/
      Svy_RemoveSurveys (HieLvl_INS,Ins_EditingIns->Cod);

      /***** Remove information related to files in institution *****/
      Brw_DB_RemoveInsFiles (Ins_EditingIns->Cod);

      /***** Remove directories of the institution *****/
      snprintf (PathIns,sizeof (PathIns),"%s/%02u/%u",
	        Cfg_PATH_INS_PUBLIC,
	        (unsigned) (Ins_EditingIns->Cod % 100),
	        (unsigned)  Ins_EditingIns->Cod);
      Fil_RemoveTree (PathIns);

      /***** Remove administrators of this institution *****/
      Adm_DB_RemAdmins (HieLvl_INS,Ins_EditingIns->Cod);

      /***** Remove institution *****/
      Ins_DB_RemoveInstitution (Ins_EditingIns->Cod);

      /***** Flush caches *****/
      Ins_FlushCacheFullNameAndCtyOfInstitution ();
      Dpt_FlushCacheNumDptsInIns ();
      Ctr_FlushCacheNumCtrsInIns ();
      Deg_FlushCacheNumDegsInIns ();
      Crs_FlushCacheNumCrssInIns ();
      Ins_FlushCacheNumUsrsWhoClaimToBelongToIns ();

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Institution_X_removed,
                       Ins_EditingIns->FullName);

      Ins_EditingIns->Cod = -1L;	// To not showing button to go to institution
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
   Ins_EditingIns->Cod = ParCod_GetAndCheckPar (ParCod_OthHie);
   Ins_RenameInstitution (Ins_EditingIns,Cns_SHRT_NAME);
  }

void Ins_RenameInsFull (void)
  {
   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Rename institution *****/
   Ins_EditingIns->Cod = ParCod_GetAndCheckPar (ParCod_OthHie);
   Ins_RenameInstitution (Ins_EditingIns,Cns_FULL_NAME);
  }

/*****************************************************************************/
/******************** Change the name of an institution **********************/
/*****************************************************************************/

void Ins_RenameInstitution (struct Ins_Instit *Ins,Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_institution_X_already_exists;
   extern const char *Txt_The_institution_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   const char *ParName = NULL;	// Initialized to avoid warning
   const char *FldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;	// Initialized to avoid warning
   char *CurrentInsName = NULL;	// Initialized to avoid warning
   char NewInsName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParName = "ShortName";
         FldName = "ShortName";
         MaxBytes = Cns_HIERARCHY_MAX_BYTES_SHRT_NAME;
         CurrentInsName = Ins->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParName = "FullName";
         FldName = "FullName";
         MaxBytes = Cns_HIERARCHY_MAX_BYTES_FULL_NAME;
         CurrentInsName = Ins->FullName;
         break;
     }

   /***** Get the new name for the institution from form *****/
   Par_GetParText (ParName,NewInsName,MaxBytes);

   /***** Get from the database the old names of the institution *****/
   Ins_GetInstitDataByCod (Ins);

   /***** Check if new name is empty *****/
   if (NewInsName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentInsName,NewInsName))	// Different names
        {
         /***** If institution was in database... *****/
         if (Ins_DB_CheckIfInsNameExistsInCty (ParName,NewInsName,Ins->Cod,
                                            Gbl.Hierarchy.Cty.Cod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_institution_X_already_exists,
                             NewInsName);
         else
           {
            /* Update the table changing old name by new name */
            Ins_UpdateInsNameDB (Ins->Cod,FldName,NewInsName);

            /* Create message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_institution_X_has_been_renamed_as_Y,
                             CurrentInsName,NewInsName);

	    /* Change current institution name in order to display it properly */
	    Str_Copy (CurrentInsName,NewInsName,MaxBytes);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_X_has_not_changed,CurrentInsName);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
  }

/*****************************************************************************/
/************ Update institution name in table of institutions ***************/
/*****************************************************************************/

static void Ins_UpdateInsNameDB (long InsCod,const char *FldName,const char *NewInsName)
  {
   /***** Update institution changing old name by new name */
   Ins_DB_UpdateInsName (InsCod,FldName,NewInsName);

   /***** Flush caches *****/
   Ins_FlushCacheFullNameAndCtyOfInstitution ();
  }

/*****************************************************************************/
/********************** Change the URL of a institution **********************/
/*****************************************************************************/

void Ins_ChangeInsWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the institution */
   Ins_EditingIns->Cod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /* Get the new WWW for the institution */
   Par_GetParText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get data of institution *****/
   Ins_GetInstitDataByCod (Ins_EditingIns);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ins_DB_UpdateInsWWW (Ins_EditingIns->Cod,NewWWW);
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
   extern const char *Txt_The_status_of_the_institution_X_has_changed;
   Hie_Status_t Status;

   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Get parameters from form *****/
   /* Get institution code */
   Ins_EditingIns->Cod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /* Get parameter with status */
   Status = Hie_GetParStatus ();	// New status

   /***** Get data of institution *****/
   Ins_GetInstitDataByCod (Ins_EditingIns);

   /***** Update status *****/
   Ins_DB_UpdateInsStatus (Ins_EditingIns->Cod,Status);
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
   if (Ins_EditingIns->Cod != Gbl.Hierarchy.Ins.Cod)
     {
      /***** Alert with button to go to institution *****/
      Ale_ShowLastAlertAndButton (ActSeeCtr,NULL,NULL,
                                  Ins_PutParInsCod,&Ins_EditingIns->Cod,
                                  Btn_CONFIRM_BUTTON,
				  Str_BuildGoToTitle (Ins_EditingIns->ShrtName));
      Str_FreeGoToTitle ();
     }
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

   /***** Set action depending on role *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      NextAction = ActNewIns;
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      NextAction = ActReqIns;
   else
      Err_NoPermissionExit ();

   /***** Begin form to create *****/
   Frm_BeginFormTable (NextAction,NULL,NULL,NULL);

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
	    Lgo_DrawLogo (HieLvl_INS,-1L,"",20,NULL);
	 HTM_TD_End ();

	 /***** Institution short name *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("ShortName",Cns_HIERARCHY_MAX_CHARS_SHRT_NAME,Ins_EditingIns->ShrtName,
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_SHORT_NAME INPUT_%s\""
			    " required=\"required\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Institution full name *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("FullName",Cns_HIERARCHY_MAX_CHARS_FULL_NAME,Ins_EditingIns->FullName,
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_FULL_NAME INPUT_%s\""
			    " required=\"required\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Institution WWW *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_URL ("WWW",Ins_EditingIns->WWW,HTM_DONT_SUBMIT_ON_CHANGE,
			   "class=\"INPUT_WWW_NARROW INPUT_%s\""
			   " required=\"required\"",
			   The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Number of users who claim to belong to this institution ****/
	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (0);
	 HTM_TD_End ();

	 /***** Number of centers *****/
	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (0);
	 HTM_TD_End ();

	 /***** Number of users in courses of this institution ****/
	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (0);
	 HTM_TD_End ();

	 /***** Institution requester *****/
	 HTM_TD_Begin ("class=\"LT DAT_%s INPUT_REQUESTER\"",
		       The_GetSuffix ());
	    Usr_WriteAuthor (&Gbl.Usrs.Me.UsrDat,Cns_ENABLED);
	 HTM_TD_End ();

	 /***** Institution status *****/
	 HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE_BUTTON);
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
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
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
      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 HTM_TxtF ("%s+",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
	 HTM_BR ();
	 HTM_Txt (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
      HTM_TH_End ();
      HTM_TH (Txt_Requester                    ,HTM_HEAD_LEFT );
      HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/*************** Receive form to request a new institution *******************/
/*****************************************************************************/

void Ins_ReceiveFormReqIns (void)
  {
   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Receive form to request a new institution *****/
   Ins_ReceiveFormRequestOrCreateIns ((Hie_Status_t) Hie_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/***************** Receive form to create a new institution ******************/
/*****************************************************************************/

void Ins_ReceiveFormNewIns (void)
  {
   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Receive form to create a new institution *****/
   Ins_ReceiveFormRequestOrCreateIns ((Hie_Status_t) 0);
  }

/*****************************************************************************/
/*********** Receive form to request or create a new institution *************/
/*****************************************************************************/

static void Ins_ReceiveFormRequestOrCreateIns (Hie_Status_t Status)
  {
   extern const char *Txt_The_institution_X_already_exists;
   extern const char *Txt_Created_new_institution_X;

   /***** Get parameters from form *****/
   /* Set institution country */
   Ins_EditingIns->PrtCod = Gbl.Hierarchy.Cty.Cod;

   /* Get institution short name */
   Par_GetParText ("ShortName",Ins_EditingIns->ShrtName,Cns_HIERARCHY_MAX_BYTES_SHRT_NAME);

   /* Get institution full name */
   Par_GetParText ("FullName",Ins_EditingIns->FullName,Cns_HIERARCHY_MAX_BYTES_FULL_NAME);

   /* Get institution WWW */
   Par_GetParText ("WWW",Ins_EditingIns->WWW,Cns_MAX_BYTES_WWW);

   if (Ins_EditingIns->ShrtName[0] &&
       Ins_EditingIns->FullName[0])	// If there's a institution name
     {
      if (Ins_EditingIns->WWW[0])
        {
         /***** If name of institution was in database... *****/
         if (Ins_DB_CheckIfInsNameExistsInCty ("ShortName",Ins_EditingIns->ShrtName,
                                            -1L,Gbl.Hierarchy.Cty.Cod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_institution_X_already_exists,
                             Ins_EditingIns->ShrtName);
         else if (Ins_DB_CheckIfInsNameExistsInCty ("FullName",Ins_EditingIns->FullName,
                                                 -1L,Gbl.Hierarchy.Cty.Cod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_institution_X_already_exists,
                             Ins_EditingIns->FullName);
         else	// Add new institution to database
           {
            Ins_EditingIns->Cod = Ins_DB_CreateInstitution (Ins_EditingIns,Status);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
			     Txt_Created_new_institution_X,
			     Ins_EditingIns->FullName);
           }
        }
      else	// If there is not a web
         Ale_CreateAlertYouMustSpecifyTheWebAddress ();
     }
   else	// If there is not a institution name
      Ale_CreateAlertYouMustSpecifyTheShortNameAndTheFullName ();
  }

/*****************************************************************************/
/********************* Get total number of institutions **********************/
/*****************************************************************************/

unsigned Ins_GetCachedNumInssInSys (void)
  {
   unsigned NumInss;

   /***** Get number of institutions from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_INSS,HieLvl_SYS,-1L,
                                   FigCch_UNSIGNED,&NumInss))
     {
      /***** Get current number of institutions from database and update cache *****/
      NumInss = (unsigned) DB_GetNumRowsTable ("ins_instits");
      FigCch_UpdateFigureIntoCache (FigCch_NUM_INSS,HieLvl_SYS,-1L,
                                    FigCch_UNSIGNED,&NumInss);
     }

   return NumInss;
  }

/*****************************************************************************/
/**************** Get number of institutions in a country ********************/
/*****************************************************************************/

void Ins_FlushCacheNumInssInCty (void)
  {
   Gbl.Cache.NumInssInCty.Valid = false;
  }

unsigned Ins_GetNumInssInCty (long CtyCod)
  {
   /***** 1. Fast check: If cached... *****/
   if (Gbl.Cache.NumInssInCty.Valid &&
       CtyCod == Gbl.Cache.NumInssInCty.CtyCod)
      return Gbl.Cache.NumInssInCty.NumInss;

   /***** 2. Slow: number of institutions in a country from database *****/
   Gbl.Cache.NumInssInCty.CtyCod  = CtyCod;
   Gbl.Cache.NumInssInCty.NumInss = Ins_DB_GetNumInssInCty (CtyCod);
   Gbl.Cache.NumInssInCty.Valid   = true;
   FigCch_UpdateFigureIntoCache (FigCch_NUM_INSS,HieLvl_CTY,Gbl.Cache.NumInssInCty.CtyCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumInssInCty.NumInss);
   return Gbl.Cache.NumInssInCty.NumInss;
  }

unsigned Ins_GetCachedNumInssInCty (long CtyCod)
  {
   unsigned NumInss;

   /***** Get number of institutions from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_INSS,HieLvl_CTY,CtyCod,
                                   FigCch_UNSIGNED,&NumInss))
      /***** Get current number of institutions from database and update cache *****/
      NumInss = Ins_GetNumInssInCty (CtyCod);

   return NumInss;
  }

/*****************************************************************************/
/***************** Get number of institutions with centers *******************/
/*****************************************************************************/

unsigned Ins_GetCachedNumInssWithCtrs (void)
  {
   unsigned NumInssWithCtrs;
   long Cod = Hie_GetCurrentCod ();

   /***** Get number of institutions with centers from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_INSS_WITH_CTRS,Gbl.Scope.Current,Cod,
				   FigCch_UNSIGNED,&NumInssWithCtrs))
     {
      /***** Get current number of institutions with centers from database and update cache *****/
      NumInssWithCtrs = Ins_DB_GetNumInssWithCtrs (Gbl.Scope.Current,Cod);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_INSS_WITH_CTRS,Gbl.Scope.Current,Cod,
				    FigCch_UNSIGNED,&NumInssWithCtrs);
     }

   return NumInssWithCtrs;
  }

/*****************************************************************************/
/****************** Get number of institutions with degrees ******************/
/*****************************************************************************/

unsigned Ins_GetCachedNumInssWithDegs (void)
  {
   unsigned NumInssWithDegs;
   long Cod = Hie_GetCurrentCod ();

   /***** Get number of institutions with degrees from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_INSS_WITH_DEGS,Gbl.Scope.Current,Cod,
				   FigCch_UNSIGNED,&NumInssWithDegs))
     {
      /***** Get current number of institutions with degrees from database and update cache *****/
      NumInssWithDegs = Ins_DB_GetNumInssWithDegs (Gbl.Scope.Current,Cod);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_INSS_WITH_DEGS,Gbl.Scope.Current,Cod,
				    FigCch_UNSIGNED,&NumInssWithDegs);
     }

   return NumInssWithDegs;
  }

/*****************************************************************************/
/****************** Get number of institutions with courses ******************/
/*****************************************************************************/

unsigned Ins_GetCachedNumInssWithCrss (void)
  {
   unsigned NumInssWithCrss;
   long Cod = Hie_GetCurrentCod ();

   /***** Get number of institutions with courses from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_INSS_WITH_CRSS,Gbl.Scope.Current,Cod,
				   FigCch_UNSIGNED,&NumInssWithCrss))
     {
      /***** Get current number of institutions with courses from database and update cache *****/
      NumInssWithCrss = Ins_DB_GetNumInssWithCrss (Gbl.Scope.Current,Cod);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_INSS_WITH_CRSS,Gbl.Scope.Current,Cod,
				    FigCch_UNSIGNED,&NumInssWithCrss);
     }

   return NumInssWithCrss;
  }

/*****************************************************************************/
/****************** Get number of institutions with users ********************/
/*****************************************************************************/

unsigned Ins_GetCachedNumInssWithUsrs (Rol_Role_t Role)
  {
   static const FigCch_FigureCached_t FigureInss[Rol_NUM_ROLES] =
     {
      [Rol_STD] = FigCch_NUM_INSS_WITH_STDS,	// Students
      [Rol_NET] = FigCch_NUM_INSS_WITH_NETS,	// Non-editing teachers
      [Rol_TCH] = FigCch_NUM_INSS_WITH_TCHS,	// Teachers
     };
   unsigned NumInssWithUsrs;
   long Cod = Hie_GetCurrentCod ();

   /***** Get number of institutions with users from cache *****/
   if (!FigCch_GetFigureFromCache (FigureInss[Role],Gbl.Scope.Current,Cod,
				   FigCch_UNSIGNED,&NumInssWithUsrs))
     {
      /***** Get current number of institutions with users from database and update cache *****/
      NumInssWithUsrs = Ins_DB_GetNumInnsWithUsrs (Role,Gbl.Scope.Current,Cod);
      FigCch_UpdateFigureIntoCache (FigureInss[Role],Gbl.Scope.Current,Cod,
				    FigCch_UNSIGNED,&NumInssWithUsrs);
     }

   return NumInssWithUsrs;
  }

/*****************************************************************************/
/*************************** List institutions found *************************/
/*****************************************************************************/

void Ins_ListInssFound (MYSQL_RES **mysql_res,unsigned NumInss)
  {
   extern const char *Txt_institution;
   extern const char *Txt_institutions;
   char *Title;
   unsigned NumIns;
   struct Ins_Instit Ins;

   /***** List the institutions (one row per institution) *****/
   if (NumInss)
     {
      /***** Begin box and table *****/
      /* Number of institutions found */
      if (asprintf (&Title,"%u %s",NumInss,
                                   NumInss == 1 ? Txt_institution :
						  Txt_institutions) < 0)
	 Err_NotEnoughMemoryExit ();
      Box_BoxTableBegin (NULL,Title,NULL,NULL,NULL,Box_NOT_CLOSABLE,2);
      free (Title);

      /***** Write heading *****/
      Ins_PutHeadInstitutionsForSeeing (false);	// Order not selectable

      /***** List the institutions (one row per institution) *****/
      for (NumIns  = 1, The_ResetRowColor ();
	   NumIns <= NumInss;
	   NumIns++, The_ChangeRowColor ())
	{
	 /* Get next institution */
	 Ins.Cod = DB_GetNextCode (*mysql_res);

	 /* Get data of institution */
	 Ins_GetInstitDataByCod (&Ins);

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
   Ins_EditingIns->Cod         = -1L;
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

static void Ins_FormToGoToMap (struct Ins_Instit *Ins)
  {
   if (Ctr_DB_CheckIfMapIsAvailableInIns (Ins->Cod))
     {
      Ins_EditingIns = Ins;	// Used to pass parameter with the code of the institution
      Lay_PutContextualLinkOnlyIcon (ActSeeInsInf,NULL,
                                     Ins_PutParInsCod,&Ins_EditingIns->Cod,
				     "map-marker-alt.svg",Ico_BLACK);
     }
  }

/*****************************************************************************/
/** Get all my institutions (those of my courses) and store them in a list ***/
/*****************************************************************************/

void Ins_GetMyInstits (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumIns;
   unsigned NumInss;
   long InsCod;

   /***** If my institutions are yet filled, there's nothing to do *****/
   if (!Gbl.Usrs.Me.MyInss.Filled)
     {
      Gbl.Usrs.Me.MyInss.Num = 0;

      /***** Get my institutions from database *****/
      NumInss = Ins_DB_GetInssFromUsr (&mysql_res,
                                       Gbl.Usrs.Me.UsrDat.UsrCod,-1L);
      for (NumIns = 0;
	   NumIns < NumInss;
	   NumIns++)
	{
	 /* Get next institution */
	 row = mysql_fetch_row (mysql_res);

	 /* Get institution code */
	 if ((InsCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
	   {
	    if (Gbl.Usrs.Me.MyInss.Num == Ins_MAX_INSTITS_PER_USR)
	       Err_ShowErrorAndExit ("Maximum number of institutions of a user exceeded.");

	    Gbl.Usrs.Me.MyInss.Inss[Gbl.Usrs.Me.MyInss.Num].InsCod  = InsCod;
	    Gbl.Usrs.Me.MyInss.Inss[Gbl.Usrs.Me.MyInss.Num].MaxRole = Rol_ConvertUnsignedStrToRole (row[1]);

	    Gbl.Usrs.Me.MyInss.Num++;
	   }
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Set boolean that indicates that my institutions are yet filled *****/
      Gbl.Usrs.Me.MyInss.Filled = true;
     }
  }

/*****************************************************************************/
/********************* Free the list of my institutions **********************/
/*****************************************************************************/

void Ins_FreeMyInstits (void)
  {
   if (Gbl.Usrs.Me.MyInss.Filled)
     {
      /***** Reset list *****/
      Gbl.Usrs.Me.MyInss.Filled = false;
      Gbl.Usrs.Me.MyInss.Num    = 0;
     }
  }

/*****************************************************************************/
/******************** Check if I belong to an institution ********************/
/*****************************************************************************/

bool Ins_CheckIfIBelongToIns (long InsCod)
  {
   unsigned NumMyIns;

   /***** Fill the list with the institutions I belong to *****/
   Ins_GetMyInstits ();

   /***** Check if the institution passed as parameter is any of my institutions *****/
   for (NumMyIns = 0;
        NumMyIns < Gbl.Usrs.Me.MyInss.Num;
        NumMyIns++)
      if (Gbl.Usrs.Me.MyInss.Inss[NumMyIns].InsCod == InsCod)
         return true;
   return false;
  }

/*****************************************************************************/
/**************** Check if a user belongs to an institution ******************/
/*****************************************************************************/

void Ins_FlushCacheUsrBelongsToIns (void)
  {
   Gbl.Cache.UsrBelongsToIns.UsrCod = -1L;
   Gbl.Cache.UsrBelongsToIns.InsCod = -1L;
   Gbl.Cache.UsrBelongsToIns.Belongs = false;
  }

bool Ins_CheckIfUsrBelongsToIns (long UsrCod,long InsCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (UsrCod <= 0 ||
       InsCod <= 0)
      return false;

   /***** 2. Fast check: If cached... *****/
   if (UsrCod == Gbl.Cache.UsrBelongsToIns.UsrCod &&
       InsCod != Gbl.Cache.UsrBelongsToIns.InsCod)
      return Gbl.Cache.UsrBelongsToIns.Belongs;

   /***** 3. Slow check: Get is user belongs to institution from database *****/
   Gbl.Cache.UsrBelongsToIns.UsrCod = UsrCod;
   Gbl.Cache.UsrBelongsToIns.InsCod = InsCod;
   Gbl.Cache.UsrBelongsToIns.Belongs = Ins_DB_CheckIfUsrBelongsToIns (UsrCod,InsCod);
   return Gbl.Cache.UsrBelongsToIns.Belongs;
  }

/*****************************************************************************/
/******** Get number of users who claim to belong to an institution **********/
/*****************************************************************************/

void Ins_FlushCacheNumUsrsWhoClaimToBelongToIns (void)
  {
   Gbl.Cache.NumUsrsWhoClaimToBelongToIns.InsCod  = -1L;
   Gbl.Cache.NumUsrsWhoClaimToBelongToIns.NumUsrs = 0;
  }

unsigned Ins_GetNumUsrsWhoClaimToBelongToIns (struct Ins_Instit *Ins)
  {
   /***** 1. Fast check: Trivial case *****/
   if (Ins->Cod <= 0)
      return 0;

   /***** 2. Fast check: If already got... *****/
   if (Ins->NumUsrsWhoClaimToBelong.Valid)
      return Ins->NumUsrsWhoClaimToBelong.NumUsrs;

   /***** 3. Fast check: If cached... *****/
   if (Ins->Cod == Gbl.Cache.NumUsrsWhoClaimToBelongToIns.InsCod)
     {
      Ins->NumUsrsWhoClaimToBelong.NumUsrs = Gbl.Cache.NumUsrsWhoClaimToBelongToIns.NumUsrs;
      Ins->NumUsrsWhoClaimToBelong.Valid = true;
      return Ins->NumUsrsWhoClaimToBelong.NumUsrs;
     }

   /***** 4. Slow: number of users who claim to belong to an institution
                   from database *****/
   Gbl.Cache.NumUsrsWhoClaimToBelongToIns.InsCod  = Ins->Cod;
   Gbl.Cache.NumUsrsWhoClaimToBelongToIns.NumUsrs =
   Ins->NumUsrsWhoClaimToBelong.NumUsrs = Ins_DB_GetNumUsrsWhoClaimToBelongToIns (Ins->Cod);
   Ins->NumUsrsWhoClaimToBelong.Valid = true;
   FigCch_UpdateFigureIntoCache (FigCch_NUM_USRS_BELONG_INS,HieLvl_INS,Gbl.Cache.NumUsrsWhoClaimToBelongToIns.InsCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumUsrsWhoClaimToBelongToIns.NumUsrs);
   return Ins->NumUsrsWhoClaimToBelong.NumUsrs;
  }

unsigned Ins_GetCachedNumUsrsWhoClaimToBelongToIns (struct Ins_Instit *Ins)
  {
   unsigned NumUsrsIns;

   /***** Get number of users who claim to belong to institution from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_USRS_BELONG_INS,HieLvl_INS,Ins->Cod,
                                   FigCch_UNSIGNED,&NumUsrsIns))
      /***** Get current number of users who claim to belong to institution from database and update cache *****/
      NumUsrsIns = Ins_GetNumUsrsWhoClaimToBelongToIns (Ins);

   return NumUsrsIns;
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

void Ins_GetAndShowInstitutionsStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_institutions;
   extern const char *Txt_Institutions;
   struct Fig_Figures Figures;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Institutions,
                 NULL,NULL,
                 Hlp_ANALYTICS_Figures_institutions,Box_NOT_CLOSABLE);

      /***** Form to select type of list used to display degree photos *****/
      Set_GetAndUpdatePrefsAboutUsrList ();
      Figures.Level      = Gbl.Scope.Current;
      Figures.FigureType = Fig_INSTITS;
      Usr_ShowFormsToSelectUsrListType (Fig_PutParsFigures,&Figures);

      /***** Institutions ordered by number of centers *****/
      Ins_GetAndShowInssOrderedByNumCtrs ();

      /***** Institutions ordered by number of degrees *****/
      Ins_GetAndShowInssOrderedByNumDegs ();

      /***** Institutions ordered by number of courses *****/
      Ins_GetAndShowInssOrderedByNumCrss ();

      /***** Institutions ordered by number of users in courses *****/
      Ins_GetAndShowInssOrderedByNumUsrsInCrss ();

      /***** Institutions ordered by number of users who claim to belong to them *****/
      Ins_GetAndShowInssOrderedByNumUsrsWhoClaimToBelongToThem ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/**** Get and show stats about institutions ordered by number of centers *****/
/*****************************************************************************/

static void Ins_GetAndShowInssOrderedByNumCtrs (void)
  {
   extern const char *Txt_Institutions_by_number_of_centers;
   extern const char *Txt_Centers;
   MYSQL_RES *mysql_res;
   unsigned NumInss;

   /***** Begin box and table *****/
   Box_BoxTableBegin ("100%",Txt_Institutions_by_number_of_centers,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

      /***** Get institutions ordered by number of centers *****/
      NumInss = Ins_DB_GetInssOrderedByNumCtrs (&mysql_res);

      /***** Show institutions *****/
      Ins_ShowInss (&mysql_res,NumInss,Txt_Centers);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/**** Get and show stats about institutions ordered by number of degrees *****/
/*****************************************************************************/

static void Ins_GetAndShowInssOrderedByNumDegs (void)
  {
   extern const char *Txt_Institutions_by_number_of_degrees;
   extern const char *Txt_Degrees;
   MYSQL_RES *mysql_res;
   unsigned NumInss;

   /***** Begin box and table *****/
   Box_BoxTableBegin ("100%",Txt_Institutions_by_number_of_degrees,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

      /***** Get institutions ordered by number of degrees *****/
      NumInss = Ins_DB_GetInssOrderedByNumDegs (&mysql_res);

      /***** Show institutions *****/
      Ins_ShowInss (&mysql_res,NumInss,Txt_Degrees);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/**** Get and show stats about institutions ordered by number of courses *****/
/*****************************************************************************/

static void Ins_GetAndShowInssOrderedByNumCrss (void)
  {
   extern const char *Txt_Institutions_by_number_of_courses;
   extern const char *Txt_Courses;
   MYSQL_RES *mysql_res;
   unsigned NumInss;

   /***** Begin box and table *****/
   Box_BoxTableBegin ("100%",Txt_Institutions_by_number_of_courses,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

      /***** Get institutions ordered by number of courses *****/
      NumInss = Ins_DB_GetInssOrderedByNumCrss (&mysql_res);

      /***** Show institutions *****/
      Ins_ShowInss (&mysql_res,NumInss,Txt_Courses);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/***** Get and show stats about institutions ordered by users in courses *****/
/*****************************************************************************/

static void Ins_GetAndShowInssOrderedByNumUsrsInCrss (void)
  {
   extern const char *Txt_Institutions_by_number_of_users_in_courses;
   extern const char *Txt_Users;
   MYSQL_RES *mysql_res;
   unsigned NumInss;

   /***** Begin box and table *****/
   Box_BoxTableBegin ("100%",Txt_Institutions_by_number_of_users_in_courses,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

      /***** Get institutions ordered by number of users in courses *****/
      NumInss = Ins_DB_GetInssOrderedByNumUsrsInCrss (&mysql_res);

      /***** Show institutions *****/
      Ins_ShowInss (&mysql_res,NumInss,Txt_Users);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/************* Get and show stats about institutions ordered by **************/
/************* number of users who claim to belong to them      **************/
/*****************************************************************************/

static void Ins_GetAndShowInssOrderedByNumUsrsWhoClaimToBelongToThem (void)
  {
   extern const char *Txt_Institutions_by_number_of_users_who_claim_to_belong_to_them;
   extern const char *Txt_Users;
   MYSQL_RES *mysql_res;
   unsigned NumInss;

   /***** Begin box and table *****/
   Box_BoxTableBegin ("100%",Txt_Institutions_by_number_of_users_who_claim_to_belong_to_them,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

      /***** Get institutions ordered by number of users who claim to belong to them *****/
      NumInss = Ins_DB_GetInssOrderedByNumUsrsWhoClaimToBelongToThem (&mysql_res);

      /***** Show institutions *****/
      Ins_ShowInss (&mysql_res,NumInss,Txt_Users);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/****************** Get and show stats about institutions ********************/
/*****************************************************************************/

static void Ins_ShowInss (MYSQL_RES **mysql_res,unsigned NumInss,
		          const char *TxtFigure)
  {
   extern const char *Txt_Institution;
   unsigned NumIns;
   unsigned NumOrder;
   unsigned NumberLastRow;
   unsigned NumberThisRow;
   struct Ins_Instit Ins;
   bool TRIsOpen = false;

   /***** Query database *****/
   if (NumInss)
     {
      /* Draw the classphoto/list */
      switch (Gbl.Usrs.Me.ListType)
	{
	 case Set_USR_LIST_AS_CLASS_PHOTO:
	    /***** Draw institutions as a class photo *****/
	    for (NumIns = 0;
		 NumIns < NumInss;)
	      {
	       if ((NumIns % Gbl.Usrs.ClassPhoto.Cols) == 0)
		 {
		  HTM_TR_Begin (NULL);
		  TRIsOpen = true;
		 }

	       /***** Get institution data and statistic *****/
	       NumberThisRow = Ins_GetInsAndStat (&Ins,*mysql_res);

	       /***** Write link to institution *****/
	       HTM_TD_Begin ("class=\"CM FORM_IN_%s\"",
	                     The_GetSuffix ());
		  Ins_DrawInstitutionLogoWithLink (&Ins,40);
		  HTM_BR ();
		  HTM_Unsigned (NumberThisRow);
               HTM_TD_End ();

	       if ((++NumIns % Gbl.Usrs.ClassPhoto.Cols) == 0)
		 {
		  HTM_TR_End ();
		  TRIsOpen = false;
		 }
	      }
	    if (TRIsOpen)
	       HTM_TR_End ();

	    break;
	 case Set_USR_LIST_AS_LISTING:
	    /***** Draw institutions as a list *****/
	    HTM_TR_Begin (NULL);
	       HTM_TH_Empty (1);
	       HTM_TH (Txt_Institution,HTM_HEAD_LEFT);
	       HTM_TH (TxtFigure      ,HTM_HEAD_RIGHT);
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
		  HTM_TD_Begin ("class=\"RM DAT_%s\"",
		                The_GetSuffix ());
		     HTM_Unsigned (NumOrder);
		  HTM_TD_End ();

		  /***** Write link to institution *****/
		  HTM_TD_Begin ("class=\"LM FORM_IN_%s\"",
				The_GetSuffix ());
		     /* Icon and name of this institution */
		     Frm_BeginForm (ActSeeInsInf);
			ParCod_PutPar (ParCod_Ins,Ins.Cod);
			HTM_BUTTON_Submit_Begin (Ins.ShrtName,
			                         "class=\"LM BT_LINK\"");
			   if (Gbl.Usrs.Listing.WithPhotos)
			     {
			      Lgo_DrawLogo (HieLvl_INS,Ins.Cod,Ins.ShrtName,
					    40,NULL);
			      HTM_NBSP ();
			     }
			   HTM_Txt (Ins.FullName);
			HTM_BUTTON_End ();
		     Frm_EndForm ();
		  HTM_TD_End ();

		  /***** Write statistic *****/
		  HTM_TD_Begin ("class=\"RM DAT_%s\"",
		                The_GetSuffix ());
		     HTM_Unsigned (NumberThisRow);
		  HTM_TD_End ();

	       HTM_TR_End ();

	       NumberLastRow = NumberThisRow;
	      }
	    break;
	 default:
	    break;
	}
     }
  }

/*****************************************************************************/
/******************** Get institution data and statistic *********************/
/*****************************************************************************/

static unsigned Ins_GetInsAndStat (struct Ins_Instit *Ins,MYSQL_RES *mysql_res)
  {
   MYSQL_ROW row;
   unsigned NumberThisRow;

   /***** Get next institution *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get data of this institution (row[0]) *****/
   Ins->Cod = Str_ConvertStrCodToLongCod (row[0]);
   if (!Ins_GetInstitDataByCod (Ins))
      Err_WrongInstitExit ();

   /***** Get statistic (row[1]) *****/
   if (sscanf (row[1],"%u",&NumberThisRow) != 1)
      Err_ShowErrorAndExit ("Error in statistic");

   return NumberThisRow;
  }
