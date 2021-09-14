// swad_institution.c: institutions

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

#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_admin_database.h"
#include "swad_browser_database.h"
#include "swad_database.h"
#include "swad_department.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_institution.h"
#include "swad_institution_database.h"
#include "swad_logo.h"
#include "swad_message.h"
#include "swad_place.h"
#include "swad_survey.h"

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

static struct Ins_Instit *Ins_EditingIns = NULL;	// Static variable to keep the institution being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ins_ListInstitutions (void);
static bool Ins_CheckIfICanCreateInstitutions (void);
static void Ins_PutIconsListingInstitutions (__attribute__((unused)) void *Args);
static void Ins_PutIconToEditInstitutions (void);
static void Ins_ListOneInstitutionForSeeing (struct Ins_Instit *Ins,unsigned NumIns);
static void Ins_PutHeadInstitutionsForSeeing (bool OrderSelectable);
static void Ins_GetParamInsOrder (void);

static void Ins_EditInstitutionsInternal (void);
static void Ins_PutIconsEditingInstitutions (__attribute__((unused)) void *Args);
static void Ins_PutIconToViewInstitutions (void);

static void Ins_GetDataOfInstitFromRow (struct Ins_Instit *Ins,MYSQL_ROW row);

static void Ins_GetShrtNameAndCtyOfInstitution (struct Ins_Instit *Ins,
                                                char CtyName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1]);

static void Ins_ListInstitutionsForEdition (void);
static bool Ins_CheckIfICanEdit (struct Ins_Instit *Ins);
static Ins_StatusTxt_t Ins_GetStatusTxtFromStatusBits (Ins_Status_t Status);
static Ins_Status_t Ins_GetStatusBitsFromStatusTxt (Ins_StatusTxt_t StatusTxt);

static void Ins_PutParamOtherInsCod (void *InsCod);
static long Ins_GetParamOtherInsCod (void);

static void Ins_UpdateInsNameDB (long InsCod,const char *FieldName,const char *NewInsName);

static void Ins_ShowAlertAndButtonToGoToIns (void);
static void Ins_PutParamGoToIns (void *InsCod);

static void Ins_PutFormToCreateInstitution (void);
static void Ins_PutHeadInstitutionsForEdition (void);
static void Ins_ReceiveFormRequestOrCreateIns (unsigned Status);

static void Ins_EditingInstitutionConstructor ();
static void Ins_EditingInstitutionDestructor ();

static void Ins_FormToGoToMap (struct Ins_Instit *Ins);

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
	    HTM_TH (1,1,"LM",Txt_Institution);
	    HTM_TH (1,1,"RM",Txt_Centers_ABBREVIATION);
	 HTM_TR_End ();

	 /***** List the institutions *****/
	 for (NumIns = 0;
	      NumIns < NumInss;
	      NumIns++)
	   {
	    /* Get next center */
	    row = mysql_fetch_row (mysql_res);

	    /* Get institution code (row[0]) */
	    Ins.InsCod = Str_ConvertStrCodToLongCod (row[0]);
	    BgColor = (Ins.InsCod == Gbl.Hierarchy.Ins.InsCod) ? "LIGHT_BLUE" :
								  Gbl.ColorRows[Gbl.RowEvenOdd];

	    /* Get data of institution */
	    Ins_GetDataOfInstitutionByCod (&Ins);

	    /* Institution logo and name */
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"LM %s\"",BgColor);
		  Ins_DrawInstitutionLogoAndNameWithLink (&Ins,ActSeeCtr,
							  "BT_LINK DAT_NOBR","CM");
	       HTM_TD_End ();

	       /* Number of pending centers (row[1]) */
	       HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
		  HTM_Txt (row[1]);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
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
   bool PutLink = !Gbl.Form.Inside;	// Don't put link to institution if already inside a form

   if (PutLink)
     {
      Frm_BeginForm (ActSeeInsInf);
      Ins_PutParamInsCod (Ins->InsCod);
	 HTM_BUTTON_SUBMIT_Begin (Ins->FullName,"BT_LINK",NULL);
     }
   Lgo_DrawLogo (HieLvl_INS,Ins->InsCod,Ins->FullName,
		 Size,NULL,true);
   if (PutLink)
     {
	 HTM_BUTTON_End ();
      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/****************** Draw institution logo and name with link *****************/
/*****************************************************************************/

void Ins_DrawInstitutionLogoAndNameWithLink (struct Ins_Instit *Ins,Act_Action_t Action,
                                             const char *ClassLink,const char *ClassLogo)
  {
   /***** Begin form *****/
   Frm_BeginFormGoTo (Action);
   Ins_PutParamInsCod (Ins->InsCod);

      /***** Link to action *****/
      HTM_BUTTON_SUBMIT_Begin (Hie_BuildGoToMsg (Ins->FullName),ClassLink,NULL);
      Hie_FreeGoToMsg ();

	 /***** Institution logo and name *****/
	 Lgo_DrawLogo (HieLvl_INS,Ins->InsCod,Ins->ShrtName,16,ClassLogo,true);
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
   if (Gbl.Hierarchy.Cty.CtyCod > 0)
     {
      /***** Get parameter with the type of order in the list of institutions *****/
      Ins_GetParamInsOrder ();

      /***** Get list of institutions *****/
      Ins_GetFullListOfInstitutions (Gbl.Hierarchy.Cty.CtyCod);

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
   extern const char *Txt_Create_another_institution;
   extern const char *Txt_Create_institution;
   unsigned NumIns;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Str_BuildStringStr (Txt_Institutions_of_COUNTRY_X,
				          Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]),
		 Ins_PutIconsListingInstitutions,NULL,
                 Hlp_COUNTRY_Institutions,Box_NOT_CLOSABLE);
   Str_FreeString ();

      if (Gbl.Hierarchy.Inss.Num)	// There are institutions in the current country
	{
	 /***** Begin table *****/
	 HTM_TABLE_BeginWideMarginPadding (2);
	 Ins_PutHeadInstitutionsForSeeing (true);	// Order selectable

	    /***** Write all the institutions and their nuber of users *****/
	    for (NumIns = 0;
		 NumIns < Gbl.Hierarchy.Inss.Num;
		 NumIns++)
	       Ins_ListOneInstitutionForSeeing (&(Gbl.Hierarchy.Inss.Lst[NumIns]),
	                                        NumIns + 1);

	 /***** End table *****/
	 HTM_TABLE_End ();
	}
      else	// No insrtitutions created in the current country
	 Ale_ShowAlert (Ale_INFO,Txt_No_institutions);

      /***** Button to create institution *****/
      if (Ins_CheckIfICanCreateInstitutions ())
	{
	 Frm_BeginForm (ActEdiIns);
	    Btn_PutConfirmButton (Gbl.Hierarchy.Inss.Num ? Txt_Create_another_institution :
							   Txt_Create_institution);
	 Frm_EndForm ();
	}

   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************* Check if I can create institutions **********************/
/*****************************************************************************/

static bool Ins_CheckIfICanCreateInstitutions (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_GST);
  }

/*****************************************************************************/
/*************** Put contextual icons in list of institutions ****************/
/*****************************************************************************/

static void Ins_PutIconsListingInstitutions (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit institutions *****/
   if (Ins_CheckIfICanCreateInstitutions ())
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
   extern const char *Txt_INSTITUTION_STATUS[Ins_NUM_STATUS_TXT];
   const char *TxtClassNormal;
   const char *TxtClassStrong;
   const char *BgColor;
   Ins_StatusTxt_t StatusTxt;

   if (Ins->Status & Ins_STATUS_BIT_PENDING)
     {
      TxtClassNormal = "DAT_LIGHT";
      TxtClassStrong = "BT_LINK LT DAT_LIGHT";
     }
   else
     {
      TxtClassNormal = "DAT";
      TxtClassStrong = "BT_LINK LT DAT_N";
     }
   BgColor = (Ins->InsCod == Gbl.Hierarchy.Ins.InsCod) ? "LIGHT_BLUE" :
                                                          Gbl.ColorRows[Gbl.RowEvenOdd];

   HTM_TR_Begin (NULL);

      /***** Number of institution in this list *****/
      HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
	 HTM_Unsigned (NumIns);
      HTM_TD_End ();

      /***** Institution logo and name *****/
      HTM_TD_Begin ("class=\"LM %s\"",BgColor);
	 Ins_DrawInstitutionLogoAndNameWithLink (Ins,ActSeeCtr,
						 TxtClassStrong,"CM");
      HTM_TD_End ();

      /***** Number of users who claim to belong to this institution *****/
      HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
	 HTM_Unsigned (Usr_GetCachedNumUsrsWhoClaimToBelongToIns (Ins));
      HTM_TD_End ();

      /***** Other stats *****/
      /* Number of centers in this institution */
      HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
	 HTM_Unsigned (Ctr_GetCachedNumCtrsInIns (Ins->InsCod));
      HTM_TD_End ();

      /* Number of degrees in this institution */
      HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
	 HTM_Unsigned (Deg_GetCachedNumDegsInIns (Ins->InsCod));
      HTM_TD_End ();

      /* Number of courses in this institution */
      HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
	 HTM_Unsigned (Crs_GetCachedNumCrssInIns (Ins->InsCod));
      HTM_TD_End ();

      /* Number of departments in this institution */
      HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
	 HTM_Unsigned (Dpt_GetNumDptsInIns (Ins->InsCod));
      HTM_TD_End ();

      /* Number of users in courses of this institution */
      HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
	 HTM_Unsigned (Usr_GetCachedNumUsrsInCrss (HieLvl_INS,Ins->InsCod,
						   1 << Rol_STD |
						   1 << Rol_NET |
						   1 << Rol_TCH));	// Any user);
      HTM_TD_End ();

      /***** Institution status *****/
      StatusTxt = Ins_GetStatusTxtFromStatusBits (Ins->Status);
      HTM_TD_Begin ("class=\"%s LM %s\"",TxtClassNormal,BgColor);
	 if (StatusTxt != Ins_STATUS_ACTIVE) // If active ==> do not show anything
	    HTM_Txt (Txt_INSTITUTION_STATUS[StatusTxt]);
      HTM_TD_End ();

   HTM_TR_End ();

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
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
   static const char *ClassTH[Ins_NUM_ORDERS] =
     {
      [Ins_ORDER_BY_INSTITUTION] = "LM",
      [Ins_ORDER_BY_NUM_USRS   ] = "RM"
     };
   static const char *ClassButton[Ins_NUM_ORDERS] =
     {
      [Ins_ORDER_BY_INSTITUTION] = "BT_LINK LM TIT_TBL",
      [Ins_ORDER_BY_NUM_USRS   ] = "BT_LINK RM TIT_TBL"
     };

   HTM_TR_Begin (NULL);

      HTM_TH_Empty (1);
      for (Order  = (Ins_Order_t) 0;
	   Order <= (Ins_Order_t) (Ins_NUM_ORDERS - 1);
	   Order++)
	{
	 HTM_TH_Begin (1,1,ClassTH[Order]);
	    if (OrderSelectable)
	      {
	       Frm_BeginForm (ActSeeIns);
	       Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
		  HTM_BUTTON_SUBMIT_Begin (Txt_INSTITUTIONS_HELP_ORDER[Order],ClassButton[Order],NULL);
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
      HTM_TH (1,1,"RM",Txt_Centers_ABBREVIATION);
      HTM_TH (1,1,"RM",Txt_Degrees_ABBREVIATION);
      HTM_TH (1,1,"RM",Txt_Courses_ABBREVIATION);
      HTM_TH (1,1,"RM",Txt_Departments_ABBREVIATION);
      HTM_TH_Begin (1,1,"RM");
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

static void Ins_GetParamInsOrder (void)
  {
   Gbl.Hierarchy.Inss.SelectedOrder = (Ins_Order_t)
				      Par_GetParToUnsignedLong ("Order",
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

   /***** Get list of institutions *****/
   Ins_GetFullListOfInstitutions (Gbl.Hierarchy.Cty.CtyCod);

   /***** Write menu to select country *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Str_BuildStringStr (Txt_Institutions_of_COUNTRY_X,
				          Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]),
		 Ins_PutIconsEditingInstitutions,NULL,
                 Hlp_COUNTRY_Institutions,Box_NOT_CLOSABLE);
   Str_FreeString ();

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
   Ins_PutIconToViewInstitutions ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_INSTITS);
  }

/*****************************************************************************/
/*********************** Put icon to view institutions ***********************/
/*****************************************************************************/

static void Ins_PutIconToViewInstitutions (void)
  {
   extern const char *Txt_Institutions;

   Lay_PutContextualLinkOnlyIcon (ActSeeIns,NULL,
                                  NULL,NULL,
				  "university.svg",
				  Txt_Institutions);
  }

/*****************************************************************************/
/******* Get basic list of institutions ordered by name of institution *******/
/*****************************************************************************/

void Ins_GetBasicListOfInstitutions (long CtyCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumIns;
   struct Ins_Instit *Ins;

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
        {
         Ins = &(Gbl.Hierarchy.Inss.Lst[NumIns]);

         /* Get institution data */
         row = mysql_fetch_row (mysql_res);
         Ins_GetDataOfInstitFromRow (Ins,row);

	 /* Reset number of users who claim to belong to this institution */
         Ins->NumUsrsWhoClaimToBelongToIns.Valid = false;
        }
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
   MYSQL_ROW row;
   unsigned NumIns;
   struct Ins_Instit *Ins;

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
        {
         Ins = &(Gbl.Hierarchy.Inss.Lst[NumIns]);

         /* Get institution data */
         row = mysql_fetch_row (mysql_res);
         Ins_GetDataOfInstitFromRow (Ins,row);

	 /* Get number of users who claim to belong to this institution (row[7]) */
         Ins->NumUsrsWhoClaimToBelongToIns.Valid = false;
	 if (sscanf (row[7],"%u",&(Ins->NumUsrsWhoClaimToBelongToIns.NumUsrs)) == 1)
	    Ins->NumUsrsWhoClaimToBelongToIns.Valid = true;
        }
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
   Ins.InsCod = InsCod;
   Ins_GetShrtNameAndCtyOfInstitution (&Ins,CtyName);

   /***** Write institution short name and country name *****/
   HTM_TxtF ("%s&nbsp;(%s)",Ins.ShrtName,CtyName);
  }

/*****************************************************************************/
/************************* Get data of an institution ************************/
/*****************************************************************************/

bool Ins_GetDataOfInstitutionByCod (struct Ins_Instit *Ins)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool InsFound = false;

   /***** Clear data *****/
   Ins->CtyCod             = -1L;
   Ins->Status             = (Ins_Status_t) 0;
   Ins->RequesterUsrCod    = -1L;
   Ins->ShrtName[0]        =
   Ins->FullName[0]        =
   Ins->WWW[0]             = '\0';
   Ins->NumUsrsWhoClaimToBelongToIns.Valid = false;

   /***** Check if institution code is correct *****/
   if (Ins->InsCod > 0)
     {
      /***** Get data of an institution from database *****/
      if (Ins_DB_GetDataOfInstitutionByCod (&mysql_res,Ins->InsCod))	// Institution found...
	{
         /* Get institution data */
	 row = mysql_fetch_row (mysql_res);
         Ins_GetDataOfInstitFromRow (Ins,row);

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

static void Ins_GetDataOfInstitFromRow (struct Ins_Instit *Ins,MYSQL_ROW row)
  {
   /*
   row[0]: InsCod
   row[1]: CtyCod
   row[2]: Status
   row[3]: RequesterUsrCod
   row[4]: ShortName
   row[5]: FullName
   row[6]: WWW
   */
   /***** Get institution code (row[0]) *****/
   if ((Ins->InsCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongInstitExit ();

   /***** Get country code (row[1]) *****/
   Ins->CtyCod = Str_ConvertStrCodToLongCod (row[1]);

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

static void Ins_GetShrtNameAndCtyOfInstitution (struct Ins_Instit *Ins,
                                                char CtyName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** 1. Fast check: Trivial case *****/
   if (Ins->InsCod <= 0)
     {
      Ins->ShrtName[0] = '\0';	// Empty name
      CtyName[0] = '\0';	// Empty name
      return;
     }

   /***** 2. Fast check: If cached... *****/
   if (Ins->InsCod == Gbl.Cache.InstitutionShrtNameAndCty.InsCod)
     {
      Str_Copy (Ins->ShrtName,Gbl.Cache.InstitutionShrtNameAndCty.ShrtName,
		sizeof (Ins->ShrtName) - 1);
      Str_Copy (CtyName,Gbl.Cache.InstitutionShrtNameAndCty.CtyName,
		Cns_HIERARCHY_MAX_BYTES_FULL_NAME);
      return;
     }

   /***** 3. Slow: get short name and country of institution from database *****/
   Gbl.Cache.InstitutionShrtNameAndCty.InsCod = Ins->InsCod;

   if (Ins_DB_GetShrtNameAndCtyOfIns (&mysql_res,Ins->InsCod) == 1)
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
      if (Gbl.Hierarchy.Cty.CtyCod > 0)
	 HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			   "id=\"ins\" name=\"ins\" class=\"HIE_SEL\"");
      else
	 HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
			   "id=\"ins\" name=\"ins\" class=\"HIE_SEL\""
			   " disabled=\"disabled\"");

      HTM_OPTION (HTM_Type_STRING,"",
		  Gbl.Hierarchy.Ins.InsCod < 0,true,
		  "[%s]",Txt_Institution);

      if (Gbl.Hierarchy.Cty.CtyCod > 0)
	{
	 /***** Get institutions of current country *****/
	 NumInss = Ins_DB_GetInssInCtyOrderedByShrtName (&mysql_res,Gbl.Hierarchy.Cty.CtyCod);

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
			Gbl.Hierarchy.Ins.InsCod > 0 &&
			InsCod == Gbl.Hierarchy.Ins.InsCod,false,
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
/************************* List all the institutions *************************/
/*****************************************************************************/

static void Ins_ListInstitutionsForEdition (void)
  {
   extern const char *Txt_INSTITUTION_STATUS[Ins_NUM_STATUS_TXT];
   unsigned NumIns;
   struct Ins_Instit *Ins;
   char WWW[Cns_MAX_BYTES_WWW + 1];
   struct UsrData UsrDat;
   bool ICanEdit;
   unsigned NumCtrs;
   unsigned NumUsrsIns;
   unsigned NumUsrsInCrssOfIns;
   Ins_StatusTxt_t StatusTxt;
   unsigned StatusUnsigned;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

      /***** Write heading *****/
      Ins_PutHeadInstitutionsForEdition ();

      /***** Write all the institutions *****/
      for (NumIns = 0;
	   NumIns < Gbl.Hierarchy.Inss.Num;
	   NumIns++)
	{
	 Ins = &Gbl.Hierarchy.Inss.Lst[NumIns];

	 ICanEdit = Ins_CheckIfICanEdit (Ins);
	 NumCtrs = Ctr_GetNumCtrsInIns (Ins->InsCod);
	 NumUsrsIns = Usr_GetNumUsrsWhoClaimToBelongToIns (Ins);
	 NumUsrsInCrssOfIns = Usr_GetNumUsrsInCrss (HieLvl_INS,Ins->InsCod,
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
					      Ins_PutParamOtherInsCod,&Ins->InsCod);
	    HTM_TD_End ();

	    /* Institution code */
	    HTM_TD_Begin ("class=\"DAT CODE\"");
	    HTM_Long (Ins->InsCod);
	    HTM_TD_End ();

	    /* Institution logo */
	    HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Ins->FullName);
	    Lgo_DrawLogo (HieLvl_INS,Ins->InsCod,Ins->ShrtName,20,NULL,true);
	    HTM_TD_End ();

	    /* Institution short name */
	    HTM_TD_Begin ("class=\"DAT LM\"");
	    if (ICanEdit)
	      {
	       Frm_BeginForm (ActRenInsSho);
	       Ins_PutParamOtherInsCod (&Ins->InsCod);
	       HTM_INPUT_TEXT ("ShortName",Cns_HIERARCHY_MAX_CHARS_SHRT_NAME,Ins->ShrtName,
			       HTM_SUBMIT_ON_CHANGE,
			       "class=\"INPUT_SHORT_NAME\"");
	       Frm_EndForm ();
	      }
	    else
	       HTM_Txt (Ins->ShrtName);
	    HTM_TD_End ();

	    /* Institution full name */
	    HTM_TD_Begin ("class=\"DAT LM\"");
	    if (ICanEdit)
	      {
	       Frm_BeginForm (ActRenInsFul);
	       Ins_PutParamOtherInsCod (&Ins->InsCod);
	       HTM_INPUT_TEXT ("FullName",Cns_HIERARCHY_MAX_CHARS_FULL_NAME,Ins->FullName,
			       HTM_SUBMIT_ON_CHANGE,
			       "class=\"INPUT_FULL_NAME\"");
	       Frm_EndForm ();
	      }
	    else
	       HTM_Txt (Ins->FullName);
	    HTM_TD_End ();

	    /* Institution WWW */
	    HTM_TD_Begin ("class=\"DAT LM\"");
	       if (ICanEdit)
		 {
		  Frm_BeginForm (ActChgInsWWW);
		  Ins_PutParamOtherInsCod (&Ins->InsCod);
		  HTM_INPUT_URL ("WWW",Ins->WWW,HTM_SUBMIT_ON_CHANGE,
				 "class=\"INPUT_WWW_NARROW\" required=\"required\"");
		  Frm_EndForm ();
		 }
	       else
		 {
		  Str_Copy (WWW,Ins->WWW,sizeof (WWW) - 1);
		  HTM_DIV_Begin ("class=\"EXTERNAL_WWW_SHORT\"");
		  HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"DAT\" title=\"%s\"",
			       Ins->WWW,Ins->WWW);
		  HTM_Txt (WWW);
		  HTM_A_End ();
		  HTM_DIV_End ();
		 }
	    HTM_TD_End ();

	    /* Number of users who claim to belong to this institution */
	    HTM_TD_Begin ("class=\"DAT RM\"");
	       HTM_Unsigned (NumUsrsIns);
	    HTM_TD_End ();

	    /* Number of centers */
	    HTM_TD_Begin ("class=\"DAT RM\"");
	       HTM_Unsigned (NumCtrs);
	    HTM_TD_End ();

	    /* Number of users in courses of this institution */
	    HTM_TD_Begin ("class=\"DAT RM\"");
	       HTM_Unsigned (NumUsrsInCrssOfIns);
	    HTM_TD_End ();

	    /* Institution requester */
	    HTM_TD_Begin ("class=\"DAT INPUT_REQUESTER LT\"");
	       UsrDat.UsrCod = Ins->RequesterUsrCod;
	       Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							Usr_DONT_GET_PREFS,
							Usr_DONT_GET_ROLE_IN_CURRENT_CRS);
	       Msg_WriteMsgAuthor (&UsrDat,true,NULL);
	    HTM_TD_End ();

	    /* Institution status */
	    HTM_TD_Begin ("class=\"DAT LM\"");
	       StatusTxt = Ins_GetStatusTxtFromStatusBits (Ins->Status);
	       if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM &&
		   StatusTxt == Ins_STATUS_PENDING)
		 {
		  Frm_BeginForm (ActChgInsSta);
		  Ins_PutParamOtherInsCod (&Ins->InsCod);
		  HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
				    "name=\"Status\" class=\"INPUT_STATUS\"");
		     StatusUnsigned = (unsigned) Ins_GetStatusBitsFromStatusTxt (Ins_STATUS_PENDING);
		     HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,true,false,
				 "%s",Txt_INSTITUTION_STATUS[Ins_STATUS_PENDING]);
		     StatusUnsigned = (unsigned) Ins_GetStatusBitsFromStatusTxt (Ins_STATUS_ACTIVE);
		     HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,false,false,
				 "%s",Txt_INSTITUTION_STATUS[Ins_STATUS_ACTIVE]);
		  HTM_SELECT_End ();
		  Frm_EndForm ();
		 }
	       else if (StatusTxt != Ins_STATUS_ACTIVE)	// If active ==> do not show anything
		  HTM_Txt (Txt_INSTITUTION_STATUS[StatusTxt]);
	    HTM_TD_End ();

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
   return (bool) (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ||		// I am a superuser
                  ((Ins->Status & Ins_STATUS_BIT_PENDING) != 0 &&		// Institution is not yet activated
                   Gbl.Usrs.Me.UsrDat.UsrCod == Ins->RequesterUsrCod));		// I am the requester
  }

/*****************************************************************************/
/******************* Set StatusTxt depending on status bits ******************/
/*****************************************************************************/
// Ins_STATUS_UNKNOWN = 0	// Other
// Ins_STATUS_ACTIVE  = 1	// 00 (Status == 0)
// Ins_STATUS_PENDING = 2	// 01 (Status == Ins_STATUS_BIT_PENDING)
// Ins_STATUS_REMOVED = 3	// 1- (Status & Ins_STATUS_BIT_REMOVED)

static Ins_StatusTxt_t Ins_GetStatusTxtFromStatusBits (Ins_Status_t Status)
  {
   if (Status == 0)
      return Ins_STATUS_ACTIVE;
   if (Status == Ins_STATUS_BIT_PENDING)
      return Ins_STATUS_PENDING;
   if (Status & Ins_STATUS_BIT_REMOVED)
      return Ins_STATUS_REMOVED;
   return Ins_STATUS_UNKNOWN;
  }

/*****************************************************************************/
/******************* Set status bits depending on StatusTxt ******************/
/*****************************************************************************/
// Ins_STATUS_UNKNOWN = 0	// Other
// Ins_STATUS_ACTIVE  = 1	// 00 (Status == 0)
// Ins_STATUS_PENDING = 2	// 01 (Status == Ins_STATUS_BIT_PENDING)
// Ins_STATUS_REMOVED = 3	// 1- (Status & Ins_STATUS_BIT_REMOVED)

static Ins_Status_t Ins_GetStatusBitsFromStatusTxt (Ins_StatusTxt_t StatusTxt)
  {
   switch (StatusTxt)
     {
      case Ins_STATUS_UNKNOWN:
      case Ins_STATUS_ACTIVE:
	 return (Ins_Status_t) 0;
      case Ins_STATUS_PENDING:
	 return Ins_STATUS_BIT_PENDING;
      case Ins_STATUS_REMOVED:
	 return Ins_STATUS_BIT_REMOVED;
     }
   return (Ins_Status_t) 0;
  }

/*****************************************************************************/
/************* Write parameter with code of current institution **************/
/*****************************************************************************/

void Ins_PutParamCurrentInsCod (void *InsCod)
  {
   if (InsCod)
      Ins_PutParamInsCod (*((long *) InsCod));
  }

/*****************************************************************************/
/***************** Write parameter with code of institution ******************/
/*****************************************************************************/

void Ins_PutParamInsCod (long InsCod)
  {
   Par_PutHiddenParamLong (NULL,"ins",InsCod);
  }

/*****************************************************************************/
/***************** Write parameter with code of institution ******************/
/*****************************************************************************/

static void Ins_PutParamOtherInsCod (void *InsCod)
  {
   if (InsCod)
      Par_PutHiddenParamLong (NULL,"OthInsCod",*((long *) InsCod));
  }

/*****************************************************************************/
/******************* Get parameter with code of institution ******************/
/*****************************************************************************/

long Ins_GetAndCheckParamOtherInsCod (long MinCodAllowed)
  {
   long InsCod;

   /***** Get and check parameter with code of institution *****/
   if ((InsCod = Ins_GetParamOtherInsCod ()) < MinCodAllowed)
      Err_WrongInstitExit ();

   return InsCod;
  }

static long Ins_GetParamOtherInsCod (void)
  {
   /***** Get code of institution *****/
   return Par_GetParToLong ("OthInsCod");
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
   Ins_EditingIns->InsCod = Ins_GetAndCheckParamOtherInsCod (1);

   /***** Get data of the institution from database *****/
   Ins_GetDataOfInstitutionByCod (Ins_EditingIns);

   /***** Check if this institution has users *****/
   if (!Ins_CheckIfICanEdit (Ins_EditingIns))
      Err_NoPermissionExit ();
   else if (Ctr_GetNumCtrsInIns (Ins_EditingIns->InsCod))
      // Institution has centers ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_an_institution_you_must_first_remove_all_centers_and_users_in_the_institution);
   else if (Usr_GetNumUsrsWhoClaimToBelongToIns (Ins_EditingIns))
      // Institution has users ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_an_institution_you_must_first_remove_all_centers_and_users_in_the_institution);
   else if (Usr_GetNumUsrsInCrss (HieLvl_INS,Ins_EditingIns->InsCod,
				  1 << Rol_STD |
				  1 << Rol_NET |
				  1 << Rol_TCH))	// Any user
      // Institution has users ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_an_institution_you_must_first_remove_all_centers_and_users_in_the_institution);
   else	// Institution has no users ==> remove it
     {
      /***** Remove all the threads and posts in forums of the institution *****/
      For_RemoveForums (HieLvl_INS,Ins_EditingIns->InsCod);

      /***** Remove surveys of the institution *****/
      Svy_RemoveSurveys (HieLvl_INS,Ins_EditingIns->InsCod);

      /***** Remove information related to files in institution *****/
      Brw_DB_RemoveInsFiles (Ins_EditingIns->InsCod);

      /***** Remove directories of the institution *****/
      snprintf (PathIns,sizeof (PathIns),"%s/%02u/%u",
	        Cfg_PATH_INS_PUBLIC,
	        (unsigned) (Ins_EditingIns->InsCod % 100),
	        (unsigned)  Ins_EditingIns->InsCod);
      Fil_RemoveTree (PathIns);

      /***** Remove administrators of this institution *****/
      Adm_DB_RemAdmins (HieLvl_INS,Ins_EditingIns->InsCod);

      /***** Remove institution *****/
      Ins_DB_RemoveInstitution (Ins_EditingIns->InsCod);

      /***** Flush caches *****/
      Ins_FlushCacheFullNameAndCtyOfInstitution ();
      Dpt_FlushCacheNumDptsInIns ();
      Ctr_FlushCacheNumCtrsInIns ();
      Deg_FlushCacheNumDegsInIns ();
      Crs_FlushCacheNumCrssInIns ();
      Usr_FlushCacheNumUsrsWhoClaimToBelongToIns ();

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Institution_X_removed,
                       Ins_EditingIns->FullName);

      Ins_EditingIns->InsCod = -1L;	// To not showing button to go to institution
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
   Ins_EditingIns->InsCod = Ins_GetAndCheckParamOtherInsCod (1);
   Ins_RenameInstitution (Ins_EditingIns,Cns_SHRT_NAME);
  }

void Ins_RenameInsFull (void)
  {
   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Rename institution *****/
   Ins_EditingIns->InsCod = Ins_GetAndCheckParamOtherInsCod (1);
   Ins_RenameInstitution (Ins_EditingIns,Cns_FULL_NAME);
  }

/*****************************************************************************/
/******************** Change the name of an institution **********************/
/*****************************************************************************/

void Ins_RenameInstitution (struct Ins_Instit *Ins,Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_institution_X_already_exists;
   extern const char *Txt_The_institution_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_institution_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentInsName = NULL;		// Initialized to avoid warning
   char NewInsName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Cns_HIERARCHY_MAX_BYTES_SHRT_NAME;
         CurrentInsName = Ins->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Cns_HIERARCHY_MAX_BYTES_FULL_NAME;
         CurrentInsName = Ins->FullName;
         break;
     }

   /***** Get the new name for the institution from form *****/
   Par_GetParToText (ParamName,NewInsName,MaxBytes);

   /***** Get from the database the old names of the institution *****/
   Ins_GetDataOfInstitutionByCod (Ins);

   /***** Check if new name is empty *****/
   if (NewInsName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentInsName,NewInsName))	// Different names
        {
         /***** If institution was in database... *****/
         if (Ins_DB_CheckIfInsNameExistsInCty (ParamName,NewInsName,Ins->InsCod,
                                            Gbl.Hierarchy.Cty.CtyCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_institution_X_already_exists,
                             NewInsName);
         else
           {
            /* Update the table changing old name by new name */
            Ins_UpdateInsNameDB (Ins->InsCod,FieldName,NewInsName);

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
                          Txt_The_name_of_the_institution_X_has_not_changed,
                          CurrentInsName);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
  }

/*****************************************************************************/
/************ Update institution name in table of institutions ***************/
/*****************************************************************************/

static void Ins_UpdateInsNameDB (long InsCod,const char *FieldName,const char *NewInsName)
  {
   /***** Update institution changing old name by new name */
   Ins_DB_UpdateInsName (InsCod,FieldName,NewInsName);

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
   Ins_EditingIns->InsCod = Ins_GetAndCheckParamOtherInsCod (1);

   /* Get the new WWW for the institution */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get data of institution *****/
   Ins_GetDataOfInstitutionByCod (Ins_EditingIns);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ins_DB_UpdateInsWWW (Ins_EditingIns->InsCod,NewWWW);
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
   Ins_Status_t Status;
   Ins_StatusTxt_t StatusTxt;

   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Get parameters from form *****/
   /* Get institution code */
   Ins_EditingIns->InsCod = Ins_GetAndCheckParamOtherInsCod (1);

   /* Get parameter with status */
   Status = (Ins_Status_t)
	    Par_GetParToUnsignedLong ("Status",
	                              0,
	                              (unsigned long) Ins_MAX_STATUS,
                                      (unsigned long) Ins_WRONG_STATUS);
   if (Status == Ins_WRONG_STATUS)
      Err_WrongStatusExit ();
   StatusTxt = Ins_GetStatusTxtFromStatusBits (Status);
   Status = Ins_GetStatusBitsFromStatusTxt (StatusTxt);	// New status

   /***** Get data of institution *****/
   Ins_GetDataOfInstitutionByCod (Ins_EditingIns);

   /***** Update status in table of institutions *****/
   Ins_DB_UpdateInsStatus (Status,Ins_EditingIns->InsCod);
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
   if (Ins_EditingIns->InsCod != Gbl.Hierarchy.Ins.InsCod)
     {
      /***** Alert with button to go to institution *****/
      Ale_ShowLastAlertAndButton (ActSeeCtr,NULL,NULL,
                                  Ins_PutParamGoToIns,&Ins_EditingIns->InsCod,
                                  Btn_CONFIRM_BUTTON,
				  Hie_BuildGoToMsg (Ins_EditingIns->ShrtName));
      Hie_FreeGoToMsg ();
     }
   else
      /***** Alert *****/
      Ale_ShowAlerts (NULL);
  }

static void Ins_PutParamGoToIns (void *InsCod)
  {
   if (InsCod)
      /***** Put parameter *****/
      Ins_PutParamInsCod (*((long *) InsCod));
  }

/*****************************************************************************/
/****************** Put a form to create a new institution *******************/
/*****************************************************************************/

static void Ins_PutFormToCreateInstitution (void)
  {
   extern const char *Txt_New_institution;
   extern const char *Txt_Create_institution;

   /***** Begin form *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      Frm_BeginForm (ActNewIns);
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      Frm_BeginForm (ActReqIns);
   else
      Err_NoPermissionExit ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_institution,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

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
	    Lgo_DrawLogo (HieLvl_INS,-1L,"",20,NULL,true);
	 HTM_TD_End ();

	 /***** Institution short name *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("ShortName",Cns_HIERARCHY_MAX_CHARS_SHRT_NAME,Ins_EditingIns->ShrtName,
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_SHORT_NAME\" required=\"required\"");
	 HTM_TD_End ();

	 /***** Institution full name *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("FullName",Cns_HIERARCHY_MAX_CHARS_FULL_NAME,Ins_EditingIns->FullName,
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_FULL_NAME\" required=\"required\"");
	 HTM_TD_End ();

	 /***** Institution WWW *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_URL ("WWW",Ins_EditingIns->WWW,HTM_DONT_SUBMIT_ON_CHANGE,
			   "class=\"INPUT_WWW_NARROW\" required=\"required\"");
	 HTM_TD_End ();

	 /***** Number of users who claim to belong to this institution ****/
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (0);
	 HTM_TD_End ();

	 /***** Number of centers *****/
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (0);
	 HTM_TD_End ();

	 /***** Number of users in courses of this institution ****/
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (0);
	 HTM_TD_End ();

	 /***** Institution requester *****/
	 HTM_TD_Begin ("class=\"DAT INPUT_REQUESTER LT\"");
	    Msg_WriteMsgAuthor (&Gbl.Usrs.Me.UsrDat,true,NULL);
	 HTM_TD_End ();

	 /***** Institution status *****/
	 HTM_TD_Begin ("class=\"DAT LM\"");
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_institution);

   /***** End form *****/
   Frm_EndForm ();
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
      HTM_TH (1,1,"RM",Txt_Code);
      HTM_TH_Empty (1);
      HTM_TH (1,1,"LM",Txt_Short_name_of_the_institution);
      HTM_TH (1,1,"LM",Txt_Full_name_of_the_institution);
      HTM_TH (1,1,"LM",Txt_WWW);
      HTM_TH (1,1,"RM",Txt_Users);
      HTM_TH (1,1,"RM",Txt_Centers_ABBREVIATION);
      HTM_TH_Begin (1,1,"RM");
	 HTM_TxtF ("%s+",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
	 HTM_BR ();
	 HTM_Txt (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
      HTM_TH_End ();
      HTM_TH (1,1,"LM",Txt_Requester);
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
   Ins_ReceiveFormRequestOrCreateIns ((unsigned) Ins_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/***************** Receive form to create a new institution ******************/
/*****************************************************************************/

void Ins_ReceiveFormNewIns (void)
  {
   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Receive form to create a new institution *****/
   Ins_ReceiveFormRequestOrCreateIns (0);
  }

/*****************************************************************************/
/*********** Receive form to request or create a new institution *************/
/*****************************************************************************/

static void Ins_ReceiveFormRequestOrCreateIns (unsigned Status)
  {
   extern const char *Txt_The_institution_X_already_exists;
   extern const char *Txt_Created_new_institution_X;
   extern const char *Txt_You_must_specify_the_web_address_of_the_new_institution;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_institution;

   /***** Get parameters from form *****/
   /* Set institution country */
   Ins_EditingIns->CtyCod = Gbl.Hierarchy.Cty.CtyCod;

   /* Get institution short name */
   Par_GetParToText ("ShortName",Ins_EditingIns->ShrtName,Cns_HIERARCHY_MAX_BYTES_SHRT_NAME);

   /* Get institution full name */
   Par_GetParToText ("FullName",Ins_EditingIns->FullName,Cns_HIERARCHY_MAX_BYTES_FULL_NAME);

   /* Get institution WWW */
   Par_GetParToText ("WWW",Ins_EditingIns->WWW,Cns_MAX_BYTES_WWW);

   if (Ins_EditingIns->ShrtName[0] &&
       Ins_EditingIns->FullName[0])	// If there's a institution name
     {
      if (Ins_EditingIns->WWW[0])
        {
         /***** If name of institution was in database... *****/
         if (Ins_DB_CheckIfInsNameExistsInCty ("ShortName",Ins_EditingIns->ShrtName,
                                            -1L,Gbl.Hierarchy.Cty.CtyCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_institution_X_already_exists,
                             Ins_EditingIns->ShrtName);
         else if (Ins_DB_CheckIfInsNameExistsInCty ("FullName",Ins_EditingIns->FullName,
                                                 -1L,Gbl.Hierarchy.Cty.CtyCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_institution_X_already_exists,
                             Ins_EditingIns->FullName);
         else	// Add new institution to database
           {
            Ins_EditingIns->InsCod = Ins_DB_CreateInstitution (Ins_EditingIns,Status);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
			     Txt_Created_new_institution_X,
			     Ins_EditingIns->FullName);
           }
        }
      else	// If there is not a web
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_You_must_specify_the_web_address_of_the_new_institution);
     }
   else	// If there is not a institution name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_institution);
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
   long Cod = Sco_GetCurrentCod ();

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
   long Cod = Sco_GetCurrentCod ();

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
   long Cod = Sco_GetCurrentCod ();

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
   long Cod = Sco_GetCurrentCod ();

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
   unsigned NumIns;
   struct Ins_Instit Ins;

   /***** List the institutions (one row per institution) *****/
   if (NumInss)
     {
      /***** Begin box and table *****/
      /* Number of institutions found */
      Box_BoxTableBegin (NULL,Str_BuildStringLongStr ((long) NumInss,
						      NumInss == 1 ? Txt_institution :
								     Txt_institutions),
			 NULL,NULL,
			 NULL,Box_NOT_CLOSABLE,2);
      Str_FreeString ();

      /***** Write heading *****/
      Ins_PutHeadInstitutionsForSeeing (false);	// Order not selectable

      /***** List the institutions (one row per institution) *****/
      for (NumIns = 1;
	   NumIns <= NumInss;
	   NumIns++)
	{
	 /* Get next institution */
	 Ins.InsCod = DB_GetNextCode (*mysql_res);

	 /* Get data of institution */
	 Ins_GetDataOfInstitutionByCod (&Ins);

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
   Ins_EditingIns->InsCod             = -1L;
   Ins_EditingIns->CtyCod             = -1L;
   Ins_EditingIns->ShrtName[0]        = '\0';
   Ins_EditingIns->FullName[0]        = '\0';
   Ins_EditingIns->WWW[0]             = '\0';
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
   extern const char *Txt_Map;

   if (Ins_DB_GetIfMapIsAvailable (Ins->InsCod))
     {
      Ins_EditingIns = Ins;	// Used to pass parameter with the code of the institution
      Lay_PutContextualLinkOnlyIcon (ActSeeInsInf,NULL,
                                     Ins_PutParamGoToIns,&Ins_EditingIns->InsCod,
				     "map-marker-alt.svg",
				     Txt_Map);
     }
  }
