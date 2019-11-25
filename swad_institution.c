// swad_institution.c: institutions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_config.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
#include "swad_HTML.h"
#include "swad_institution.h"
#include "swad_language.h"
#include "swad_logo.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_user.h"

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

static struct Instit *Ins_EditingIns = NULL;	// Static variable to keep the institution being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ins_Configuration (bool PrintView);
static void Ins_PutIconsToPrintAndUpload (void);
static void Ins_ShowNumUsrsInCrssOfIns (Rol_Role_t Role);

static void Ins_ListInstitutions (void);
static bool Ins_CheckIfICanCreateInstitutions (void);
static void Ins_PutIconsListingInstitutions (void);
static void Ins_PutIconToEditInstitutions (void);
static void Ins_ListOneInstitutionForSeeing (struct Instit *Ins,unsigned NumIns);
static void Ins_PutHeadInstitutionsForSeeing (bool OrderSelectable);
static void Ins_GetParamInsOrder (void);

static void Ins_EditInstitutionsInternal (void);
static void Ins_PutIconsEditingInstitutions (void);
static void Ins_PutIconToViewInstitutions (void);

static void Ins_GetShrtNameAndCtyOfInstitution (struct Instit *Ins,
                                                char CtyName[Hie_MAX_BYTES_FULL_NAME + 1]);

static void Ins_ListInstitutionsForEdition (void);
static bool Ins_CheckIfICanEdit (struct Instit *Ins);
static Ins_StatusTxt_t Ins_GetStatusTxtFromStatusBits (Ins_Status_t Status);
static Ins_Status_t Ins_GetStatusBitsFromStatusTxt (Ins_StatusTxt_t StatusTxt);

static void Ins_PutParamOtherInsCod (long InsCod);
static long Ins_GetParamOtherInsCod (void);

static void Ins_RenameInstitution (struct Instit *Ins,Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Ins_CheckIfInsNameExistsInCty (const char *FieldName,
                                           const char *Name,
					   long InsCod,
					   long CtyCod);
static void Ins_UpdateInsNameDB (long InsCod,const char *FieldName,const char *NewInsName);

static void Ins_UpdateInsCtyDB (long InsCod,long CtyCod);
static void Ins_UpdateInsWWWDB (long InsCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1]);
static void Ins_ShowAlertAndButtonToGoToIns (void);
static void Ins_PutParamGoToIns (void);

static void Ins_PutFormToCreateInstitution (void);
static void Ins_PutHeadInstitutionsForEdition (void);
static void Ins_RecFormRequestOrCreateIns (unsigned Status);
static void Ins_CreateInstitution (unsigned Status);

static void Ins_EditingInstitutionConstructor ();
static void Ins_EditingInstitutionDestructor ();

/*****************************************************************************/
/***************** List institutions with pending centres ********************/
/*****************************************************************************/

void Ins_SeeInsWithPendingCtrs (void)
  {
   extern const char *Hlp_SYSTEM_Hierarchy_pending;
   extern const char *Txt_Institutions_with_pending_centres;
   extern const char *Txt_Institution;
   extern const char *Txt_Centres_ABBREVIATION;
   extern const char *Txt_There_are_no_institutions_with_requests_for_centres_to_be_confirmed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumInss;
   unsigned NumIns;
   struct Instit Ins;
   const char *BgColor;

   /***** Get institutions with pending centres *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_INS_ADM:
         NumInss =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions"
					       " with pending centres",
				    "SELECT centres.InsCod,COUNT(*)"
				    " FROM centres,ins_admin,institutions"
				    " WHERE (centres.Status & %u)<>0"
				    " AND centres.InsCod=ins_admin.InsCod AND ins_admin.UsrCod=%ld"
				    " AND centres.InsCod=institutions.InsCod"
				    " GROUP BY centres.InsCod ORDER BY institutions.ShortName",
				    (unsigned) Ctr_STATUS_BIT_PENDING,
				    Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Rol_SYS_ADM:
         NumInss =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions"
					       " with pending centres",
				    "SELECT centres.InsCod,COUNT(*)"
				    " FROM centres,institutions"
				    " WHERE (centres.Status & %u)<>0"
				    " AND centres.InsCod=institutions.InsCod"
				    " GROUP BY centres.InsCod ORDER BY institutions.ShortName",
				    (unsigned) Ctr_STATUS_BIT_PENDING);
         break;
      default:	// Forbidden for other users
	 return;
     }

   /***** Get institutions *****/
   if (NumInss)
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_Institutions_with_pending_centres,NULL,
                         Hlp_SYSTEM_Hierarchy_pending,Box_NOT_CLOSABLE,2);

      /***** Write heading *****/
      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"LM",Txt_Institution);
      HTM_TH (1,1,"RM",Txt_Centres_ABBREVIATION);

      HTM_TR_End ();

      /***** List the institutions *****/
      for (NumIns = 0;
	   NumIns < NumInss;
	   NumIns++)
        {
         /* Get next centre */
         row = mysql_fetch_row (mysql_res);

         /* Get institution code (row[0]) */
         Ins.InsCod = Str_ConvertStrCodToLongCod (row[0]);
         BgColor = (Ins.InsCod == Gbl.Hierarchy.Ins.InsCod) ? "LIGHT_BLUE" :
                                                               Gbl.ColorRows[Gbl.RowEvenOdd];

         /* Get data of institution */
         Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);

         /* Institution logo and name */
         HTM_TR_Begin (NULL);
         HTM_TD_Begin ("class=\"LM %s\"",BgColor);
         Ins_DrawInstitutionLogoAndNameWithLink (&Ins,ActSeeCtr,
                                                 "BT_LINK DAT_NOBR","CM");
         HTM_TD_End ();

         /* Number of pending centres (row[1]) */
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
      Ale_ShowAlert (Ale_INFO,Txt_There_are_no_institutions_with_requests_for_centres_to_be_confirmed);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Draw institution logo with link **********************/
/*****************************************************************************/

void Ins_DrawInstitutionLogoWithLink (struct Instit *Ins,unsigned Size)
  {
   bool PutLink = !Gbl.Form.Inside;	// Don't put link to institution if already inside a form

   if (PutLink)
     {
      Frm_StartForm (ActSeeInsInf);
      Ins_PutParamInsCod (Ins->InsCod);
      HTM_BUTTON_SUBMIT_Begin (Ins->FullName,"BT_LINK",NULL);
     }
   Log_DrawLogo (Hie_INS,Ins->InsCod,Ins->FullName,
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

void Ins_DrawInstitutionLogoAndNameWithLink (struct Instit *Ins,Act_Action_t Action,
                                             const char *ClassLink,const char *ClassLogo)
  {
   extern const char *Txt_Go_to_X;

   /***** Begin form *****/
   Frm_StartFormGoTo (Action);
   Ins_PutParamInsCod (Ins->InsCod);

   /***** Link to action *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Go_to_X,
	     Ins->FullName);
   HTM_BUTTON_SUBMIT_Begin (Gbl.Title,ClassLink,NULL);

   /***** Institution logo and name *****/
   Log_DrawLogo (Hie_INS,Ins->InsCod,Ins->ShrtName,16,ClassLogo,true);
   HTM_TxtF ("&nbsp;%s",Ins->FullName);

   /***** End link *****/
   HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************** Show information of the current institution *****************/
/*****************************************************************************/

void Ins_ShowConfiguration (void)
  {
   Ins_Configuration (false);

   /***** Show help to enrol me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/*************** Print information of the current institution ****************/
/*****************************************************************************/

void Ins_PrintConfiguration (void)
  {
   Ins_Configuration (true);
  }

/*****************************************************************************/
/***************** Information of the current institution ********************/
/*****************************************************************************/

static void Ins_Configuration (bool PrintView)
  {
   extern const char *Hlp_INSTITUTION_Information;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Short_name;
   extern const char *Txt_Web;
   extern const char *Txt_Shortcut;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_QR_code;
   extern const char *Txt_Centres;
   extern const char *Txt_Centres_of_INSTITUTION_X;
   extern const char *Txt_Degrees;
   extern const char *Txt_Courses;
   extern const char *Txt_Departments;
   extern const char *Txt_Users_of_the_institution;
   unsigned NumCty;
   bool PutLink;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Ins.InsCod <= 0)		// No institution selected
      return;

   /***** Begin box *****/
   if (PrintView)
      Box_BoxBegin (NULL,NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,NULL,Ins_PutIconsToPrintAndUpload,
		    Hlp_INSTITUTION_Information,Box_NOT_CLOSABLE);

   /***** Title *****/
   PutLink = !PrintView && Gbl.Hierarchy.Ins.WWW[0];
   HTM_DIV_Begin ("class=\"FRAME_TITLE FRAME_TITLE_BIG\"");
   if (PutLink)
      HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"FRAME_TITLE_BIG\" title=\"%s\"",
	           Gbl.Hierarchy.Ins.WWW,
	           Gbl.Hierarchy.Ins.FullName);
   Log_DrawLogo (Hie_INS,Gbl.Hierarchy.Ins.InsCod,
		 Gbl.Hierarchy.Ins.ShrtName,64,NULL,true);
   HTM_BR ();
   HTM_Txt (Gbl.Hierarchy.Ins.FullName);
   if (PutLink)
      HTM_A_End ();
   HTM_DIV_End ();

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

   /***** Country *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   HTM_LABEL_Begin ("for=\"OthCtyCod\" class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Country);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LM\"");
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      // Only system admins can move an institution to another country
     {
      /* Get list of countries */
      Cty_GetListCountries (Cty_GET_BASIC_DATA);

      /* Put form to select country */
      Frm_StartForm (ActChgInsCtyCfg);
      HTM_SELECT_Begin (true,
			"id=\"OthCtyCod\" name=\"OthCtyCod\""
		        " class=\"INPUT_SHORT_NAME\"");
      for (NumCty = 0;
	   NumCty < Gbl.Hierarchy.Sys.Ctys.Num;
	   NumCty++)
	 HTM_OPTION (HTM_Type_LONG,&Gbl.Hierarchy.Sys.Ctys.Lst[NumCty].CtyCod,
		     Gbl.Hierarchy.Sys.Ctys.Lst[NumCty].CtyCod == Gbl.Hierarchy.Cty.CtyCod,false,
		     "%s",Gbl.Hierarchy.Sys.Ctys.Lst[NumCty].Name[Gbl.Prefs.Language]);
      HTM_SELECT_End ();
      Frm_EndForm ();

      /* Free list of countries */
      Cty_FreeListCountries ();
     }
   else	// I can not move institution to another country
      HTM_Txt (Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Institution full name *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   HTM_LABEL_Begin ("for=\"FullName\" class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Institution);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT_N LM\"");
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      // Only system admins can edit institution full name
     {
      /* Form to change institution full name */
      Frm_StartForm (ActRenInsFulCfg);
      HTM_INPUT_TEXT ("FullName",Hie_MAX_CHARS_FULL_NAME,Gbl.Hierarchy.Ins.FullName,true,
		      "id=\"FullName\" class=\"INPUT_FULL_NAME\"");
      Frm_EndForm ();
     }
   else	// I can not edit institution full name
      HTM_Txt (Gbl.Hierarchy.Ins.FullName);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Institution short name *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   HTM_LABEL_Begin ("for=\"ShortName\" class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Short_name);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT_N LM\"");
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      // Only system admins can edit institution short name
     {
      /* Form to change institution short name */
      Frm_StartForm (ActRenInsShoCfg);
      HTM_INPUT_TEXT ("ShortName",Hie_MAX_CHARS_SHRT_NAME,Gbl.Hierarchy.Ins.ShrtName,true,
		      "id=\"ShortName\" class=\"INPUT_SHORT_NAME\"");
      Frm_EndForm ();
     }
   else	// I can not edit institution short name
      HTM_Txt (Gbl.Hierarchy.Ins.ShrtName);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Institution WWW *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   HTM_LABEL_Begin ("for=\"WWW\" class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Web);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LM\"");
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      // Only institution admins and system admins
      // can change institution WWW
     {
      /* Form to change institution WWW */
      Frm_StartForm (ActChgInsWWWCfg);
      HTM_INPUT_URL ("WWW",Gbl.Hierarchy.Ins.WWW,true,
		     "class=\"INPUT_WWW\"");
      Frm_EndForm ();
     }
   else	// I can not change institution WWW
     {
      HTM_DIV_Begin ("class=\"EXTERNAL_WWW_LONG\"");
      HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"DAT\">",
	           Gbl.Hierarchy.Ins.WWW);
      HTM_Txt (Gbl.Hierarchy.Ins.WWW);
      HTM_A_End ();
      HTM_DIV_End ();
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Shortcut to the institution *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Shortcut);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LM\"");
   HTM_A_Begin ("href=\"%s/%s?ins=%ld\" class=\"DAT\" target=\"_blank\"",
	       Cfg_URL_SWAD_CGI,
	       Lan_STR_LANG_ID[Gbl.Prefs.Language],
	       Gbl.Hierarchy.Ins.InsCod);
   HTM_TxtF ("%s/%s?ins=%ld",
	     Cfg_URL_SWAD_CGI,
	     Lan_STR_LANG_ID[Gbl.Prefs.Language],
	     Gbl.Hierarchy.Ins.InsCod);
   HTM_A_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   if (PrintView)
     {
      /***** QR code with link to the institution *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_TxtF ("%s:",Txt_QR_code);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LM\"");
      QR_LinkTo (250,"ins",Gbl.Hierarchy.Ins.InsCod);
      HTM_TD_End ();

      HTM_TR_End ();
     }
   else
     {
      /***** Number of users who claim to belong to this institution *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_TxtF ("%s:",Txt_Users_of_the_institution);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LM\"");
      HTM_Unsigned (Usr_GetNumUsrsWhoClaimToBelongToIns (Gbl.Hierarchy.Ins.InsCod));
      HTM_TD_End ();

      HTM_TR_End ();
      HTM_TR_Begin (NULL);

      /***** Number of centres *****/
      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_TxtF ("%s:",Txt_Centres);
      HTM_TD_End ();

      /* Form to go to see centres of this institution */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartFormGoTo (ActSeeCtr);
      Ins_PutParamInsCod (Gbl.Hierarchy.Ins.InsCod);
      snprintf (Gbl.Title,sizeof (Gbl.Title),
		Txt_Centres_of_INSTITUTION_X,
		Gbl.Hierarchy.Ins.ShrtName);
      HTM_BUTTON_SUBMIT_Begin (Gbl.Title,"BT_LINK DAT",NULL);
      HTM_Unsigned (Ctr_GetNumCtrsInIns (Gbl.Hierarchy.Ins.InsCod));
      HTM_BUTTON_End ();
      Frm_EndForm ();
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Number of degrees *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_TxtF ("%s:",Txt_Degrees);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LM\"");
      HTM_Unsigned (Deg_GetNumDegsInIns (Gbl.Hierarchy.Ins.InsCod));
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Number of courses *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_TxtF ("%s:",Txt_Courses);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LM\"");
      HTM_Unsigned (Crs_GetNumCrssInIns (Gbl.Hierarchy.Ins.InsCod));
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Number of departments *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_TxtF ("%s:",Txt_Departments);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LM\"");
      HTM_Unsigned (Dpt_GetNumDepartmentsInInstitution (Gbl.Hierarchy.Ins.InsCod));
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Number of users in courses of this institution *****/
      Ins_ShowNumUsrsInCrssOfIns (Rol_TCH);
      Ins_ShowNumUsrsInCrssOfIns (Rol_NET);
      Ins_ShowNumUsrsInCrssOfIns (Rol_STD);
      Ins_ShowNumUsrsInCrssOfIns (Rol_UNK);
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********* Put contextual icons in configuration of an institution ***********/
/*****************************************************************************/

static void Ins_PutIconsToPrintAndUpload (void)
  {
   /***** Icon to print info about institution *****/
   Ico_PutContextualIconToPrint (ActPrnInsInf,NULL);

   if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      /***** Icon to upload logo of institution *****/
      Log_PutIconToChangeLogo (Hie_INS);

   /***** Put icon to view places *****/
   Plc_PutIconToViewPlaces ();
  }

/*****************************************************************************/
/************** Number of users in courses of this institution ***************/
/*****************************************************************************/

static void Ins_ShowNumUsrsInCrssOfIns (Rol_Role_t Role)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Users_in_courses;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Role == Rol_UNK ? Txt_Users_in_courses :
		                     Txt_ROLES_PLURAL_Abc[Role][Usr_SEX_UNKNOWN]);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_Unsigned (Usr_GetNumUsrsInCrssOfIns (Role,Gbl.Hierarchy.Ins.InsCod));
   HTM_TD_End ();

   HTM_TR_End ();
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
      Ins_GetListInstitutions (Gbl.Hierarchy.Cty.CtyCod,Ins_GET_EXTRA_DATA);

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
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Institutions_of_COUNTRY_X,
	     Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);
   Box_BoxBegin (NULL,Gbl.Title,Ins_PutIconsListingInstitutions,
                 Hlp_COUNTRY_Institutions,Box_NOT_CLOSABLE);

   if (Gbl.Hierarchy.Cty.Inss.Num)	// There are institutions in the current country
     {
      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (2);
      Ins_PutHeadInstitutionsForSeeing (true);	// Order selectable

      /***** Write all the institutions and their nuber of users *****/
      for (NumIns = 0;
	   NumIns < Gbl.Hierarchy.Cty.Inss.Num;
	   NumIns++)
	 Ins_ListOneInstitutionForSeeing (&(Gbl.Hierarchy.Cty.Inss.Lst[NumIns]),NumIns + 1);

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No insrtitutions created in the current country
      Ale_ShowAlert (Ale_INFO,Txt_No_institutions);

   /***** Button to create institution *****/
   if (Ins_CheckIfICanCreateInstitutions ())
     {
      Frm_StartForm (ActEdiIns);
      Btn_PutConfirmButton (Gbl.Hierarchy.Cty.Inss.Num ? Txt_Create_another_institution :
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

static void Ins_PutIconsListingInstitutions (void)
  {
   /***** Put icon to edit institutions *****/
   if (Ins_CheckIfICanCreateInstitutions ())
      Ins_PutIconToEditInstitutions ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_INSTITS;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/******************** Put link (form) to edit institutions *******************/
/*****************************************************************************/

static void Ins_PutIconToEditInstitutions (void)
  {
   Ico_PutContextualIconToEdit (ActEdiIns,NULL);
  }

/*****************************************************************************/
/********************** List one institution for seeing **********************/
/*****************************************************************************/

static void Ins_ListOneInstitutionForSeeing (struct Instit *Ins,unsigned NumIns)
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

   /***** Stats *****/
   /* Number of users who claim to belong to this institution */
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Ins->NumUsrsWhoClaimToBelongToIns);
   HTM_TD_End ();

   /* Number of centres in this institution */
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Ins->Ctrs.Num);
   HTM_TD_End ();

   /* Number of degrees in this institution */
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Ins->NumDegs);
   HTM_TD_End ();

   /* Number of courses in this institution */
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Ins->NumCrss);
   HTM_TD_End ();

   /* Number of departments in this institution */
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Ins->NumDpts);
   HTM_TD_End ();

   /* Number of users in courses of this institution */
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Ins->NumUsrs);
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
   extern const char *Txt_Centres_ABBREVIATION;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_Courses_ABBREVIATION;
   extern const char *Txt_Departments_ABBREVIATION;
   Ins_Order_t Order;

   HTM_TR_Begin (NULL);
   HTM_TH_Empty (1);
   for (Order = Ins_ORDER_BY_INSTITUTION;
	Order <= Ins_ORDER_BY_NUM_USRS;
	Order++)
     {
      HTM_TH_Begin (1,1,Order == Ins_ORDER_BY_INSTITUTION ? "LM" :
						            "RM");
      if (OrderSelectable)
	{
	 Frm_StartForm (ActSeeIns);
	 Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
	 HTM_BUTTON_SUBMIT_Begin (Txt_INSTITUTIONS_HELP_ORDER[Order],"BT_LINK TIT_TBL",NULL);
	 if (Order == Gbl.Hierarchy.Cty.Inss.SelectedOrder)
	    HTM_U_Begin ();
	}
      HTM_Txt (Txt_INSTITUTIONS_ORDER[Order]);
      if (OrderSelectable)
	{
	 if (Order == Gbl.Hierarchy.Cty.Inss.SelectedOrder)
	    HTM_U_End ();
	 HTM_BUTTON_End ();
	 Frm_EndForm ();
	}
      HTM_TH_End ();
     }

   HTM_TH (1,1,"RM",Txt_Centres_ABBREVIATION);
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
   Gbl.Hierarchy.Cty.Inss.SelectedOrder = (Ins_Order_t)
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
   Ins_GetListInstitutions (Gbl.Hierarchy.Cty.CtyCod,Ins_GET_EXTRA_DATA);

   /***** Write menu to select country *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Institutions_of_COUNTRY_X,
             Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);
   Box_BoxBegin (NULL,Gbl.Title,Ins_PutIconsEditingInstitutions,
                 Hlp_COUNTRY_Institutions,Box_NOT_CLOSABLE);

   /***** Put a form to create a new institution *****/
   Ins_PutFormToCreateInstitution ();

   /***** Forms to edit current institutions *****/
   if (Gbl.Hierarchy.Cty.Inss.Num)
      Ins_ListInstitutionsForEdition ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of institutions *****/
   Ins_FreeListInstitutions ();
  }

/*****************************************************************************/
/************ Put contextual icons in edition of institutions ****************/
/*****************************************************************************/

static void Ins_PutIconsEditingInstitutions (void)
  {
   /***** Put icon to view institutions *****/
   Ins_PutIconToViewInstitutions ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_INSTITS;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/*********************** Put icon to view institutions ***********************/
/*****************************************************************************/

static void Ins_PutIconToViewInstitutions (void)
  {
   extern const char *Txt_Institutions;

   Lay_PutContextualLinkOnlyIcon (ActSeeIns,NULL,NULL,
				  "university.svg",
				  Txt_Institutions);
  }

/*****************************************************************************/
/********************** Get list of current institutions *********************/
/*****************************************************************************/
// If CtyCod <= 0, get all institutions

void Ins_GetListInstitutions (long CtyCod,Ins_GetExtraData_t GetExtraData)
  {
   static const char *OrderBySubQuery[Ins_NUM_ORDERS] =
     {
      [Ins_ORDER_BY_INSTITUTION] = "FullName",
      [Ins_ORDER_BY_NUM_USRS   ] = "NumUsrs DESC,FullName",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows = 0;	// Initialized to avoid warning
   unsigned NumIns;
   struct Instit *Ins;

   /***** Get institutions from database *****/
   switch (GetExtraData)
     {
      case Ins_GET_BASIC_DATA:
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get institutions",
				   "SELECT InsCod,CtyCod,Status,RequesterUsrCod,ShortName,FullName,WWW"
				   " FROM institutions"
				   " WHERE CtyCod=%ld"
				   " ORDER BY FullName",
				   CtyCod);
         break;
      case Ins_GET_EXTRA_DATA:
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get institutions",
				   "(SELECT institutions.InsCod,institutions.CtyCod,"
				   "institutions.Status,institutions.RequesterUsrCod,"
				   "institutions.ShortName,institutions.FullName,"
				   "institutions.WWW,COUNT(*) AS NumUsrs"
				   " FROM institutions,usr_data"
				   " WHERE institutions.CtyCod=%ld"
				   " AND institutions.InsCod=usr_data.InsCod"
				   " GROUP BY institutions.InsCod)"
				   " UNION "
				   "(SELECT InsCod,CtyCod,Status,RequesterUsrCod,ShortName,FullName,WWW,0 AS NumUsrs"
				   " FROM institutions"
				   " WHERE CtyCod=%ld"
				   " AND InsCod NOT IN"
				   " (SELECT DISTINCT InsCod FROM usr_data))"
				   " ORDER BY %s",
				   CtyCod,CtyCod,
				   OrderBySubQuery[Gbl.Hierarchy.Cty.Inss.SelectedOrder]);
         break;
     }

   if (NumRows) // Institutions found...
     {
      // NumRows should be equal to Deg->NumCourses
      Gbl.Hierarchy.Cty.Inss.Num = (unsigned) NumRows;

      /***** Create list with institutions *****/
      if ((Gbl.Hierarchy.Cty.Inss.Lst = (struct Instit *) calloc (NumRows,sizeof (struct Instit))) == NULL)
          Lay_NotEnoughMemoryExit ();

      /***** Get the institutions *****/
      for (NumIns = 0;
	   NumIns < Gbl.Hierarchy.Cty.Inss.Num;
	   NumIns++)
        {
         Ins = &(Gbl.Hierarchy.Cty.Inss.Lst[NumIns]);

         /* Get next institution */
         row = mysql_fetch_row (mysql_res);

         /* Get institution code (row[0]) */
         if ((Ins->InsCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of institution.");

         /* Get country code (row[1]) */
         Ins->CtyCod = Str_ConvertStrCodToLongCod (row[1]);

	 /* Get institution status (row[2]) */
	 if (sscanf (row[2],"%u",&(Ins->Status)) != 1)
	    Lay_ShowErrorAndExit ("Wrong institution status.");

	 /* Get requester user's code (row[3]) */
	 Ins->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[3]);

         /* Get the short name of the institution (row[4]) */
         Str_Copy (Ins->ShrtName,row[4],
                   Hie_MAX_BYTES_SHRT_NAME);

         /* Get the full name of the institution (row[5]) */
         Str_Copy (Ins->FullName,row[5],
                   Hie_MAX_BYTES_FULL_NAME);

         /* Get the URL of the institution (row[6]) */
         Str_Copy (Ins->WWW,row[6],
                   Cns_MAX_BYTES_WWW);

         /* Get extra data */
         switch (GetExtraData)
           {
            case Ins_GET_BASIC_DATA:
               Ins->NumUsrsWhoClaimToBelongToIns = 0;
               Ins->Ctrs.Num = Ins->NumDegs = Ins->NumCrss = Ins->NumDpts = 0;
               Ins->NumUsrs = 0;
               break;
            case Ins_GET_EXTRA_DATA:
               /* Get number of users who claim to belong to this institution (row[7]) */
               if (sscanf (row[7],"%u",&Ins->NumUsrsWhoClaimToBelongToIns) != 1)
        	  Ins->NumUsrsWhoClaimToBelongToIns = 0;

               /* Get number of centres in this institution */
               Ins->Ctrs.Num = Ctr_GetNumCtrsInIns (Ins->InsCod);

               /* Get number of degrees in this institution */
               Ins->NumDegs = Deg_GetNumDegsInIns (Ins->InsCod);

               /* Get number of degrees in this institution */
               Ins->NumCrss = Crs_GetNumCrssInIns (Ins->InsCod);

               /* Get number of departments in this institution */
               Ins->NumDpts = Dpt_GetNumDptsInIns (Ins->InsCod);

               /* Get number of users in courses */
	       Ins->NumUsrs = Usr_GetNumUsrsInCrssOfIns (Rol_UNK,Ins->InsCod);	// Here Rol_UNK means "all users"
               break;
           }
        }
     }
   else
      Gbl.Hierarchy.Cty.Inss.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Write institution full name and country *******************/
/*****************************************************************************/
// If ClassLink == NULL ==> do not put link

void Ins_WriteInstitutionNameAndCty (long InsCod)
  {
   struct Instit Ins;
   char CtyName[Hie_MAX_BYTES_FULL_NAME + 1];

   /***** Get institution short name and country name *****/
   Ins.InsCod = InsCod;
   Ins_GetShrtNameAndCtyOfInstitution (&Ins,CtyName);

   /***** Write institution short name and country name *****/
   HTM_TxtF ("%s&nbsp;(%s)",Ins.ShrtName,CtyName);
  }

/*****************************************************************************/
/************************* Get data of an institution ************************/
/*****************************************************************************/

bool Ins_GetDataOfInstitutionByCod (struct Instit *Ins,
                                    Ins_GetExtraData_t GetExtraData)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool InsFound = false;

   /***** Clear data *****/
   Ins->CtyCod = -1L;
   Ins->Status = (Ins_Status_t) 0;
   Ins->RequesterUsrCod = -1L;
   Ins->ShrtName[0] =
   Ins->FullName[0] =
   Ins->WWW[0] = '\0';
   Ins->NumUsrsWhoClaimToBelongToIns = 0;
   Ins->Ctrs.Num = Ins->NumDegs = Ins->NumCrss = Ins->NumDpts = 0;
   Ins->NumUsrs = 0;

   /***** Check if institution code is correct *****/
   if (Ins->InsCod > 0)
     {
      /***** Get data of an institution from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get data of an institution",
			  "SELECT CtyCod,Status,RequesterUsrCod,ShortName,FullName,WWW"
			  " FROM institutions WHERE InsCod=%ld",
			  Ins->InsCod))	// Institution found...
	{
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get country code (row[0]) */
	 Ins->CtyCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get centre status (row[1]) */
	 if (sscanf (row[1],"%u",&(Ins->Status)) != 1)
	    Lay_ShowErrorAndExit ("Wrong institution status.");

	 /* Get requester user's code (row[2]) */
	 Ins->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[2]);

	 /* Get the short name of the institution (row[3]) */
	 Str_Copy (Ins->ShrtName,row[3],
	           Hie_MAX_BYTES_SHRT_NAME);

	 /* Get the full name of the institution (row[4]) */
	 Str_Copy (Ins->FullName,row[4],
	           Hie_MAX_BYTES_FULL_NAME);

	 /* Get the URL of the institution (row[5]) */
	 Str_Copy (Ins->WWW,row[5],
	           Cns_MAX_BYTES_WWW);

	 /* Get extra data */
	 if (GetExtraData == Ins_GET_EXTRA_DATA)
	   {
	    /* Get number of centres in this institution */
	    Ins->Ctrs.Num = Ctr_GetNumCtrsInIns (Ins->InsCod);

	    /* Get number of departments in this institution */
	    Ins->NumDpts = Dpt_GetNumDptsInIns (Ins->InsCod);

	    /* Get number of degrees in this institution */
	    Ins->NumDegs = Deg_GetNumDegsInIns (Ins->InsCod);

	    /* Get number of users in courses of this institution */
	    Ins->NumUsrs = Usr_GetNumUsrsInCrssOfIns (Rol_UNK,Ins->InsCod);	// Here Rol_UNK means "all users"
	   }

         /* Set return value */
	 InsFound = true;
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return InsFound;
  }

/*****************************************************************************/
/*********** Get the short name of an institution from its code **************/
/*****************************************************************************/

void Ins_FlushCacheShortNameOfInstitution (void)
  {
   Gbl.Cache.InstitutionShrtName.InsCod = -1L;
   Gbl.Cache.InstitutionShrtName.ShrtName[0] = '\0';
  }

void Ins_GetShortNameOfInstitution (struct Instit *Ins)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** 1. Fast check: Trivial case *****/
   if (Ins->InsCod <= 0)
     {
      Ins->ShrtName[0] = '\0';	// Empty name
      return;
     }

   /***** 2. Fast check: If cached... *****/
   if (Ins->InsCod == Gbl.Cache.InstitutionShrtName.InsCod)
     {
      Str_Copy (Ins->ShrtName,Gbl.Cache.InstitutionShrtName.ShrtName,
		Hie_MAX_BYTES_SHRT_NAME);
      return;
     }

   /***** 3. Slow: get short name of institution from database *****/
   Gbl.Cache.InstitutionShrtName.InsCod = Ins->InsCod;

   if (DB_QuerySELECT (&mysql_res,"can not get the short name"
				  " of an institution",
	               "SELECT ShortName FROM institutions"
	               " WHERE InsCod=%ld",
	               Ins->InsCod) == 1)
     {
      /* Get the short name of this institution */
      row = mysql_fetch_row (mysql_res);

      Str_Copy (Gbl.Cache.InstitutionShrtName.ShrtName,row[0],
		Hie_MAX_BYTES_SHRT_NAME);
     }
   else
      Gbl.Cache.InstitutionShrtName.ShrtName[0] = '\0';

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   Str_Copy (Ins->ShrtName,Gbl.Cache.InstitutionShrtName.ShrtName,
	     Hie_MAX_BYTES_SHRT_NAME);
  }

/*****************************************************************************/
/************ Get the full name of an institution from its code **************/
/*****************************************************************************/

void Ins_FlushCacheFullNameAndCtyOfInstitution (void)
  {
   Gbl.Cache.InstitutionShrtNameAndCty.InsCod = -1L;
   Gbl.Cache.InstitutionShrtNameAndCty.ShrtName[0] = '\0';
   Gbl.Cache.InstitutionShrtNameAndCty.CtyName[0] = '\0';
  }

static void Ins_GetShrtNameAndCtyOfInstitution (struct Instit *Ins,
                                                char CtyName[Hie_MAX_BYTES_FULL_NAME + 1])
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
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
		Hie_MAX_BYTES_SHRT_NAME);
      Str_Copy (CtyName,Gbl.Cache.InstitutionShrtNameAndCty.CtyName,
		Hie_MAX_BYTES_FULL_NAME);
      return;
     }

   /***** 3. Slow: get short name and country of institution from database *****/
   Gbl.Cache.InstitutionShrtNameAndCty.InsCod = Ins->InsCod;

   if (DB_QuerySELECT (&mysql_res,"can not get short name and country"
				  " of an institution",
		       "SELECT institutions.ShortName,countries.Name_%s"
		       " FROM institutions,countries"
		       " WHERE institutions.InsCod=%ld"
		       " AND institutions.CtyCod=countries.CtyCod",
		       Lan_STR_LANG_ID[Gbl.Prefs.Language],Ins->InsCod) == 1)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get the short name of this institution (row[0]) */
      Str_Copy (Gbl.Cache.InstitutionShrtNameAndCty.ShrtName,row[0],
		Hie_MAX_BYTES_SHRT_NAME);

      /* Get the name of the country (row[1]) */
      Str_Copy (Gbl.Cache.InstitutionShrtNameAndCty.CtyName,row[1],
		Hie_MAX_BYTES_FULL_NAME);
     }
   else
     {
      Gbl.Cache.InstitutionShrtNameAndCty.ShrtName[0] = '\0';
      Gbl.Cache.InstitutionShrtNameAndCty.CtyName[0] = '\0';
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   Str_Copy (Ins->ShrtName,Gbl.Cache.InstitutionShrtNameAndCty.ShrtName,
	     Hie_MAX_BYTES_SHRT_NAME);
   Str_Copy (CtyName,Gbl.Cache.InstitutionShrtNameAndCty.CtyName,
	     Hie_MAX_BYTES_FULL_NAME);
  }

/*****************************************************************************/
/************************* Free list of institutions *************************/
/*****************************************************************************/

void Ins_FreeListInstitutions (void)
  {
   if (Gbl.Hierarchy.Cty.Inss.Lst)
     {
      /***** Free memory used by the list of institutions *****/
      free (Gbl.Hierarchy.Cty.Inss.Lst);
      Gbl.Hierarchy.Cty.Inss.Lst = NULL;
      Gbl.Hierarchy.Cty.Inss.Num = 0;
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
   Frm_StartFormGoTo (ActSeeCtr);
   if (Gbl.Hierarchy.Cty.CtyCod > 0)
      HTM_SELECT_Begin (true,
			"id=\"ins\" name=\"ins\" class=\"HIE_SEL\"");
   else
      HTM_SELECT_Begin (false,
			"id=\"ins\" name=\"ins\" class=\"HIE_SEL\""
			" disabled=\"disabled\"");
   HTM_OPTION (HTM_Type_STRING,"",
	       Gbl.Hierarchy.Ins.InsCod < 0,true,
	       "[%s]",Txt_Institution);

   if (Gbl.Hierarchy.Cty.CtyCod > 0)
     {
      /***** Get institutions of selected country from database *****/
      NumInss =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
	                         "SELECT DISTINCT InsCod,ShortName"
	                         " FROM institutions"
				 " WHERE CtyCod=%ld"
				 " ORDER BY ShortName",
				 Gbl.Hierarchy.Cty.CtyCod);

      /***** List institutions *****/
      for (NumIns = 0;
	   NumIns < NumInss;
	   NumIns++)
        {
         /* Get next institution */
         row = mysql_fetch_row (mysql_res);

         /* Get institution code (row[0]) */
         if ((InsCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of institution.");

         /* Write option */
	 HTM_OPTION (HTM_Type_LONG,&InsCod,
		     Gbl.Hierarchy.Ins.InsCod > 0 &&
		     InsCod == Gbl.Hierarchy.Ins.InsCod,false,
		     "%s",row[1]);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** End form *****/
   HTM_SELECT_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************* List all the institutions *************************/
/*****************************************************************************/

static void Ins_ListInstitutionsForEdition (void)
  {
   extern const char *Txt_INSTITUTION_STATUS[Ins_NUM_STATUS_TXT];
   unsigned NumIns;
   struct Instit *Ins;
   char WWW[Cns_MAX_BYTES_WWW + 1];
   struct UsrData UsrDat;
   bool ICanEdit;
   Ins_StatusTxt_t StatusTxt;
   unsigned StatusUnsigned;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Write heading *****/
   HTM_TABLE_BeginWidePadding (2);
   Ins_PutHeadInstitutionsForEdition ();

   /***** Write all the institutions *****/
   for (NumIns = 0;
	NumIns < Gbl.Hierarchy.Cty.Inss.Num;
	NumIns++)
     {
      Ins = &Gbl.Hierarchy.Cty.Inss.Lst[NumIns];

      ICanEdit = Ins_CheckIfICanEdit (Ins);

      HTM_TR_Begin (NULL);

      /* Put icon to remove institution */
      HTM_TD_Begin ("class=\"BM\"");
      if (Ins->Ctrs.Num ||
	  Ins->NumUsrsWhoClaimToBelongToIns ||
	  Ins->NumUsrs ||	// Institution has centres or users ==> deletion forbidden
          !ICanEdit)
         Ico_PutIconRemovalNotAllowed ();
      else
        {
         Frm_StartForm (ActRemIns);
         Ins_PutParamOtherInsCod (Ins->InsCod);
         Ico_PutIconRemove ();
         Frm_EndForm ();
        }
      HTM_TD_End ();

      /* Institution code */
      HTM_TD_Begin ("class=\"DAT CODE\"");
      HTM_Long (Ins->InsCod);
      HTM_TD_End ();

      /* Institution logo */
      HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Ins->FullName);
      Log_DrawLogo (Hie_INS,Ins->InsCod,Ins->ShrtName,20,NULL,true);
      HTM_TD_End ();

      /* Institution short name */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_StartForm (ActRenInsSho);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 HTM_INPUT_TEXT ("ShortName",Hie_MAX_CHARS_SHRT_NAME,Ins->ShrtName,true,
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
	 Frm_StartForm (ActRenInsFul);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 HTM_INPUT_TEXT ("FullName",Hie_MAX_CHARS_FULL_NAME,Ins->FullName,true,
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
	 Frm_StartForm (ActChgInsWWW);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 HTM_INPUT_URL ("WWW",Ins->WWW,true,
			"class=\"INPUT_WWW\"");
	 Frm_EndForm ();
	 HTM_TD_End ();
	}
      else
	{
         Str_Copy (WWW,Ins->WWW,
                   Cns_MAX_BYTES_WWW);
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
      HTM_Unsigned (Ins->NumUsrsWhoClaimToBelongToIns);
      HTM_TD_End ();

      /* Number of centres */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Unsigned (Ins->Ctrs.Num);
      HTM_TD_End ();

      /* Number of users in courses of this institution */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Unsigned (Ins->NumUsrs);
      HTM_TD_End ();

      /* Institution requester */
      UsrDat.UsrCod = Ins->RequesterUsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);
      HTM_TD_Begin ("class=\"DAT INPUT_REQUESTER LT\"");
      Msg_WriteMsgAuthor (&UsrDat,true,NULL);
      HTM_TD_End ();

      /* Institution status */
      StatusTxt = Ins_GetStatusTxtFromStatusBits (Ins->Status);
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM &&
	  StatusTxt == Ins_STATUS_PENDING)
	{
	 Frm_StartForm (ActChgInsSta);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 HTM_SELECT_Begin (true,
			   "name=\"Status\" class=\"INPUT_STATUS\"");
	 StatusUnsigned = (unsigned) Ins_GetStatusBitsFromStatusTxt (Ins_STATUS_PENDING);
	 HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,true,false,
		     "%s",Txt_INSTITUTION_STATUS[Ins_STATUS_PENDING]);
	 StatusUnsigned = (unsigned) Ins_GetStatusBitsFromStatusTxt (Ins_STATUS_ACTIVE);
	 HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,true,false,
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

static bool Ins_CheckIfICanEdit (struct Instit *Ins)
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
/***************** Write parameter with code of institution ******************/
/*****************************************************************************/

void Ins_PutParamInsCod (long InsCod)
  {
   Par_PutHiddenParamLong (NULL,"ins",InsCod);
  }

/*****************************************************************************/
/***************** Write parameter with code of institution ******************/
/*****************************************************************************/

static void Ins_PutParamOtherInsCod (long InsCod)
  {
   Par_PutHiddenParamLong (NULL,"OthInsCod",InsCod);
  }

/*****************************************************************************/
/******************* Get parameter with code of institution ******************/
/*****************************************************************************/

long Ins_GetAndCheckParamOtherInsCod (long MinCodAllowed)
  {
   long InsCod;

   /***** Get and check parameter with code of institution *****/
   if ((InsCod = Ins_GetParamOtherInsCod ()) < MinCodAllowed)
      Lay_ShowErrorAndExit ("Code of institution is missing or invalid.");

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
   extern const char *Txt_To_remove_an_institution_you_must_first_remove_all_centres_and_users_in_the_institution;
   extern const char *Txt_Institution_X_removed;
   char PathIns[PATH_MAX + 1];

   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Get institution code *****/
   Ins_EditingIns->InsCod = Ins_GetAndCheckParamOtherInsCod (1);

   /***** Get data of the institution from database *****/
   Ins_GetDataOfInstitutionByCod (Ins_EditingIns,Ins_GET_EXTRA_DATA);

   /***** Check if this institution has users *****/
   if (!Ins_CheckIfICanEdit (Ins_EditingIns))
      Lay_NoPermissionExit ();
   else if (Ins_EditingIns->Ctrs.Num ||
            Ins_EditingIns->NumUsrsWhoClaimToBelongToIns ||
            Ins_EditingIns->NumUsrs)	// Institution has centres or users ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_an_institution_you_must_first_remove_all_centres_and_users_in_the_institution);
   else	// Institution has no users ==> remove it
     {
      /***** Remove all the threads and posts in forums of the institution *****/
      For_RemoveForums (Hie_INS,Ins_EditingIns->InsCod);

      /***** Remove surveys of the institution *****/
      Svy_RemoveSurveys (Hie_INS,Ins_EditingIns->InsCod);

      /***** Remove information related to files in institution *****/
      Brw_RemoveInsFilesFromDB (Ins_EditingIns->InsCod);

      /***** Remove directories of the institution *****/
      snprintf (PathIns,sizeof (PathIns),
	        "%s/%02u/%u",
	        Cfg_PATH_INS_PUBLIC,
	        (unsigned) (Ins_EditingIns->InsCod % 100),
	        (unsigned) Ins_EditingIns->InsCod);
      Fil_RemoveTree (PathIns);

      /***** Remove institution *****/
      DB_QueryDELETE ("can not remove an institution",
		      "DELETE FROM institutions WHERE InsCod=%ld",
		      Ins_EditingIns->InsCod);

      /***** Flush caches *****/
      Ins_FlushCacheShortNameOfInstitution ();
      Ins_FlushCacheFullNameAndCtyOfInstitution ();

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
/************ Change the name of an institution in configuration *************/
/*****************************************************************************/

void Ins_RenameInsShortInConfig (void)
  {
   /***** Rename institution *****/
   Ins_RenameInstitution (&Gbl.Hierarchy.Ins,Cns_SHRT_NAME);
  }

void Ins_RenameInsFullInConfig (void)
  {
   /***** Rename institution *****/
   Ins_RenameInstitution (&Gbl.Hierarchy.Ins,Cns_FULL_NAME);
  }

/*****************************************************************************/
/******************** Change the name of an institution **********************/
/*****************************************************************************/

static void Ins_RenameInstitution (struct Instit *Ins,Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_institution_X_empty;
   extern const char *Txt_The_institution_X_already_exists;
   extern const char *Txt_The_institution_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_institution_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentInsName = NULL;		// Initialized to avoid warning
   char NewInsName[Hie_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Hie_MAX_BYTES_SHRT_NAME;
         CurrentInsName = Ins->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Hie_MAX_BYTES_FULL_NAME;
         CurrentInsName = Ins->FullName;
         break;
     }

   /***** Get the new name for the institution from form *****/
   Par_GetParToText (ParamName,NewInsName,MaxBytes);

   /***** Get from the database the old names of the institution *****/
   Ins_GetDataOfInstitutionByCod (Ins,Ins_GET_BASIC_DATA);

   /***** Check if new name is empty *****/
   if (!NewInsName[0])
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_leave_the_name_of_the_institution_X_empty,
                       CurrentInsName);
   else
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentInsName,NewInsName))	// Different names
        {
         /***** If institution was in database... *****/
         if (Ins_CheckIfInsNameExistsInCty (ParamName,NewInsName,Ins->InsCod,
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
	    Str_Copy (CurrentInsName,NewInsName,
	              MaxBytes);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_of_the_institution_X_has_not_changed,
                          CurrentInsName);
     }
  }

/*****************************************************************************/
/****** Check if the name of institution exists in the current country *******/
/*****************************************************************************/

static bool Ins_CheckIfInsNameExistsInCty (const char *FieldName,
                                           const char *Name,
					   long InsCod,
					   long CtyCod)
  {
   /***** Get number of institutions in current country with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of an institution"
			  " already existed",
			  "SELECT COUNT(*) FROM institutions"
			  " WHERE CtyCod=%ld AND %s='%s' AND InsCod<>%ld",
			  CtyCod,FieldName,Name,InsCod) != 0);
  }

/*****************************************************************************/
/************ Update institution name in table of institutions ***************/
/*****************************************************************************/

static void Ins_UpdateInsNameDB (long InsCod,const char *FieldName,const char *NewInsName)
  {
   /***** Update institution changing old name by new name */
   DB_QueryUPDATE ("can not update the name of an institution",
		   "UPDATE institutions SET %s='%s' WHERE InsCod=%ld",
	           FieldName,NewInsName,InsCod);

   /***** Flush caches *****/
   Ins_FlushCacheShortNameOfInstitution ();
   Ins_FlushCacheFullNameAndCtyOfInstitution ();
  }

/*****************************************************************************/
/******************* Change the country of a institution *********************/
/*****************************************************************************/

void Ins_ChangeInsCtyInConfig (void)
  {
   extern const char *Txt_The_institution_X_already_exists;
   extern const char *Txt_The_country_of_the_institution_X_has_changed_to_Y;
   struct Country NewCty;

   /***** Get the new country code for the institution *****/
   NewCty.CtyCod = Cty_GetAndCheckParamOtherCtyCod (0);

   /***** Check if country has changed *****/
   if (NewCty.CtyCod != Gbl.Hierarchy.Ins.CtyCod)
     {
      /***** Get data of the country from database *****/
      Cty_GetDataOfCountryByCod (&NewCty,Cty_GET_BASIC_DATA);

      /***** Check if it already exists an institution with the same name in the new country *****/
      if (Ins_CheckIfInsNameExistsInCty ("ShortName",Gbl.Hierarchy.Ins.ShrtName,-1L,NewCty.CtyCod))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_institution_X_already_exists,
		          Gbl.Hierarchy.Ins.ShrtName);
      else if (Ins_CheckIfInsNameExistsInCty ("FullName",Gbl.Hierarchy.Ins.FullName,-1L,NewCty.CtyCod))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_institution_X_already_exists,
		          Gbl.Hierarchy.Ins.FullName);
      else
	{
	 /***** Update the table changing the country of the institution *****/
	 Ins_UpdateInsCtyDB (Gbl.Hierarchy.Ins.InsCod,NewCty.CtyCod);
         Gbl.Hierarchy.Ins.CtyCod =
         Gbl.Hierarchy.Cty.CtyCod = NewCty.CtyCod;

	 /***** Initialize again current course, degree, centre... *****/
	 Hie_InitHierarchy ();

	 /***** Write message to show the change made *****/
         Ale_CreateAlert (Ale_SUCCESS,NULL,
                          Txt_The_country_of_the_institution_X_has_changed_to_Y,
		          Gbl.Hierarchy.Ins.FullName,NewCty.Name[Gbl.Prefs.Language]);
	}
     }
  }

/*****************************************************************************/
/*** Show msg. of success after changing an institution in instit. config. ***/
/*****************************************************************************/

void Ins_ContEditAfterChgInsInConfig (void)
  {
   /***** Write success / warning message *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Ins_ShowConfiguration ();
  }

/*****************************************************************************/
/****************** Update country in table of institutions ******************/
/*****************************************************************************/

static void Ins_UpdateInsCtyDB (long InsCod,long CtyCod)
  {
   /***** Update country in table of institutions *****/
   DB_QueryUPDATE ("can not update the country of an institution",
		   "UPDATE institutions SET CtyCod=%ld WHERE InsCod=%ld",
                   CtyCod,InsCod);
  }

/*****************************************************************************/
/********************** Change the URL of a institution **********************/
/*****************************************************************************/

void Ins_ChangeInsWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   extern const char *Txt_You_can_not_leave_the_web_address_empty;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the institution */
   Ins_EditingIns->InsCod = Ins_GetAndCheckParamOtherInsCod (1);

   /* Get the new WWW for the institution */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get data of institution *****/
   Ins_GetDataOfInstitutionByCod (Ins_EditingIns,Ins_GET_BASIC_DATA);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ins_UpdateInsWWWDB (Ins_EditingIns->InsCod,NewWWW);
      Str_Copy (Ins_EditingIns->WWW,NewWWW,
                Cns_MAX_BYTES_WWW);

      /***** Write message to show the change made
	     and put button to go to institution changed *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
		       Txt_The_new_web_address_is_X,
		       NewWWW);
     }
   else
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_leave_the_web_address_empty);
  }

void Ins_ChangeInsWWWInConfig (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   extern const char *Txt_You_can_not_leave_the_web_address_empty;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Get parameters from form *****/
   /* Get the new WWW for the institution */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ins_UpdateInsWWWDB (Gbl.Hierarchy.Ins.InsCod,NewWWW);
      Str_Copy (Gbl.Hierarchy.Ins.WWW,NewWWW,
                Cns_MAX_BYTES_WWW);

      /***** Write message to show the change made *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_new_web_address_is_X,
		     NewWWW);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_the_web_address_empty);

   /***** Show the form again *****/
   Ins_ShowConfiguration ();
  }

/*****************************************************************************/
/**************** Update database changing old WWW by new WWW ****************/
/*****************************************************************************/

static void Ins_UpdateInsWWWDB (long InsCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1])
  {
   /***** Update database changing old WWW by new WWW *****/
   DB_QueryUPDATE ("can not update the web of an institution",
		   "UPDATE institutions SET WWW='%s' WHERE InsCod=%ld",
	           NewWWW,InsCod);
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
      Lay_ShowErrorAndExit ("Wrong status.");
   StatusTxt = Ins_GetStatusTxtFromStatusBits (Status);
   Status = Ins_GetStatusBitsFromStatusTxt (StatusTxt);	// New status

   /***** Get data of institution *****/
   Ins_GetDataOfInstitutionByCod (Ins_EditingIns,Ins_GET_BASIC_DATA);

   /***** Update status in table of institutions *****/
   DB_QueryUPDATE ("can not update the status of an institution",
		   "UPDATE institutions SET Status=%u WHERE InsCod=%ld",
                   (unsigned) Status,Ins_EditingIns->InsCod);
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
   extern const char *Txt_Go_to_X;

   // If the institution being edited is different to the current one...
   if (Ins_EditingIns->InsCod != Gbl.Hierarchy.Ins.InsCod)
     {
      /***** Alert with button to go to institution *****/
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        Txt_Go_to_X,
		Ins_EditingIns->ShrtName);
      Ale_ShowLastAlertAndButton (ActSeeCtr,NULL,NULL,Ins_PutParamGoToIns,
                                  Btn_CONFIRM_BUTTON,Gbl.Title);
     }
   else
      /***** Alert *****/
      Ale_ShowAlerts (NULL);
  }

static void Ins_PutParamGoToIns (void)
  {
   /***** Put parameter *****/
   Ins_PutParamInsCod (Ins_EditingIns->InsCod);
  }

/*****************************************************************************/
/******** Show a form for sending a logo of the current institution **********/
/*****************************************************************************/

void Ins_RequestLogo (void)
  {
   Log_RequestLogo (Hie_INS);
  }

/*****************************************************************************/
/************** Receive the logo of the current institution ******************/
/*****************************************************************************/

void Ins_ReceiveLogo (void)
  {
   Log_ReceiveLogo (Hie_INS);
  }

/*****************************************************************************/
/*************** Remove the logo of the current institution ******************/
/*****************************************************************************/

void Ins_RemoveLogo (void)
  {
   Log_RemoveLogo (Hie_INS);
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
      Frm_StartForm (ActNewIns);
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      Frm_StartForm (ActReqIns);
   else
      Lay_NoPermissionExit ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_institution,NULL,
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
   Log_DrawLogo (Hie_INS,-1L,"",20,NULL,true);
   HTM_TD_End ();

   /***** Institution short name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("ShortName",Hie_MAX_CHARS_SHRT_NAME,Ins_EditingIns->ShrtName,false,
		   "class=\"INPUT_SHORT_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Institution full name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("FullName",Hie_MAX_CHARS_FULL_NAME,Ins_EditingIns->FullName,false,
		   "class=\"INPUT_FULL_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Institution WWW *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_URL ("WWW",Ins_EditingIns->WWW,false,
		  "class=\"INPUT_WWW\" required=\"required\"");
   HTM_TD_End ();

   /***** Number of users who claim to belong to this institution ****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (0);
   HTM_TD_End ();

   /***** Number of centres *****/
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
   extern const char *Txt_Centres_ABBREVIATION;
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
   HTM_TH (1,1,"RM",Txt_Centres_ABBREVIATION);
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

void Ins_RecFormReqIns (void)
  {
   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Receive form to request a new institution *****/
   Ins_RecFormRequestOrCreateIns ((unsigned) Ins_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/***************** Receive form to create a new institution ******************/
/*****************************************************************************/

void Ins_RecFormNewIns (void)
  {
   /***** Institution constructor *****/
   Ins_EditingInstitutionConstructor ();

   /***** Receive form to create a new institution *****/
   Ins_RecFormRequestOrCreateIns (0);
  }

/*****************************************************************************/
/*********** Receive form to request or create a new institution *************/
/*****************************************************************************/

static void Ins_RecFormRequestOrCreateIns (unsigned Status)
  {
   extern const char *Txt_The_institution_X_already_exists;
   extern const char *Txt_Created_new_institution_X;
   extern const char *Txt_You_must_specify_the_web_address_of_the_new_institution;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_institution;

   /***** Get parameters from form *****/
   /* Set institution country */
   Ins_EditingIns->CtyCod = Gbl.Hierarchy.Cty.CtyCod;

   /* Get institution short name */
   Par_GetParToText ("ShortName",Ins_EditingIns->ShrtName,Hie_MAX_BYTES_SHRT_NAME);

   /* Get institution full name */
   Par_GetParToText ("FullName",Ins_EditingIns->FullName,Hie_MAX_BYTES_FULL_NAME);

   /* Get institution WWW */
   Par_GetParToText ("WWW",Ins_EditingIns->WWW,Cns_MAX_BYTES_WWW);

   if (Ins_EditingIns->ShrtName[0] &&
       Ins_EditingIns->FullName[0])	// If there's a institution name
     {
      if (Ins_EditingIns->WWW[0])
        {
         /***** If name of institution was in database... *****/
         if (Ins_CheckIfInsNameExistsInCty ("ShortName",Ins_EditingIns->ShrtName,
                                            -1L,Gbl.Hierarchy.Cty.CtyCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_institution_X_already_exists,
                             Ins_EditingIns->ShrtName);
         else if (Ins_CheckIfInsNameExistsInCty ("FullName",Ins_EditingIns->FullName,
                                                 -1L,Gbl.Hierarchy.Cty.CtyCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_institution_X_already_exists,
                             Ins_EditingIns->FullName);
         else	// Add new institution to database
           {
            Ins_CreateInstitution (Status);
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
/************************** Create a new institution *************************/
/*****************************************************************************/

static void Ins_CreateInstitution (unsigned Status)
  {
   /***** Create a new institution *****/
   Ins_EditingIns->InsCod =
   DB_QueryINSERTandReturnCode ("can not create institution",
				"INSERT INTO institutions"
				" (CtyCod,Status,RequesterUsrCod,ShortName,FullName,WWW)"
				" VALUES"
				" (%ld,%u,%ld,'%s','%s','%s')",
				Ins_EditingIns->CtyCod,
				Status,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Ins_EditingIns->ShrtName,
				Ins_EditingIns->FullName,
				Ins_EditingIns->WWW);
  }

/*****************************************************************************/
/********************* Get total number of institutions **********************/
/*****************************************************************************/

unsigned Ins_GetNumInssTotal (void)
  {
   /***** Get total number of degrees from database *****/
   return (unsigned) DB_GetNumRowsTable ("institutions");
  }

/*****************************************************************************/
/**************** Get number of institutions in a country ********************/
/*****************************************************************************/

unsigned Ins_GetNumInssInCty (long CtyCod)
  {
   /***** Get number of degrees of a place from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get the number of institutions"
			     " in a country",
			     "SELECT COUNT(*) FROM institutions"
			     " WHERE CtyCod=%ld",
			     CtyCod);
  }

/*****************************************************************************/
/***************** Get number of institutions with centres *******************/
/*****************************************************************************/

unsigned Ins_GetNumInssWithCtrs (const char *SubQuery)
  {
   /***** Get number of institutions with centres from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of institutions with centres",
			     "SELECT COUNT(DISTINCT institutions.InsCod)"
			     " FROM institutions,centres"
			     " WHERE %sinstitutions.InsCod=centres.InsCod",
			     SubQuery);
  }

/*****************************************************************************/
/****************** Get number of institutions with degrees ******************/
/*****************************************************************************/

unsigned Ins_GetNumInssWithDegs (const char *SubQuery)
  {
   /***** Get number of institutions with degrees from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of institutions with degrees",
			     "SELECT COUNT(DISTINCT institutions.InsCod)"
			     " FROM institutions,centres,degrees"
			     " WHERE %sinstitutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod",
			     SubQuery);
  }

/*****************************************************************************/
/****************** Get number of institutions with courses ******************/
/*****************************************************************************/

unsigned Ins_GetNumInssWithCrss (const char *SubQuery)
  {
   /***** Get number of institutions with courses from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of institutions with courses",
			     "SELECT COUNT(DISTINCT institutions.InsCod)"
			     " FROM institutions,centres,degrees,courses"
			     " WHERE %sinstitutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod"
			     " AND degrees.DegCod=courses.DegCod",
			     SubQuery);
  }

/*****************************************************************************/
/****************** Get number of institutions with users ********************/
/*****************************************************************************/

unsigned Ins_GetNumInssWithUsrs (Rol_Role_t Role,const char *SubQuery)
  {
   /***** Get number of institutions with users from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of institutions with users",
			     "SELECT COUNT(DISTINCT institutions.InsCod)"
			     " FROM institutions,centres,degrees,courses,crs_usr"
			     " WHERE %sinstitutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod"
			     " AND degrees.DegCod=courses.DegCod"
			     " AND courses.CrsCod=crs_usr.CrsCod"
			     " AND crs_usr.Role=%u",
			     SubQuery,(unsigned) Role);
  }

/*****************************************************************************/
/*************************** List institutions found *************************/
/*****************************************************************************/

void Ins_ListInssFound (MYSQL_RES **mysql_res,unsigned NumInss)
  {
   extern const char *Txt_institution;
   extern const char *Txt_institutions;
   MYSQL_ROW row;
   unsigned NumIns;
   struct Instit Ins;

   /***** List the institutions (one row per institution) *****/
   if (NumInss)
     {
      /***** Begin box and table *****/
      /* Number of institutions found */
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        "%u %s",
                NumInss,NumInss == 1 ? Txt_institution :
				       Txt_institutions);
      Box_BoxTableBegin (NULL,Gbl.Title,NULL,
                         NULL,Box_NOT_CLOSABLE,2);

      /***** Write heading *****/
      Ins_PutHeadInstitutionsForSeeing (false);	// Order not selectable

      /***** List the institutions (one row per institution) *****/
      for (NumIns = 1;
	   NumIns <= NumInss;
	   NumIns++)
	{
	 /* Get next institution */
	 row = mysql_fetch_row (*mysql_res);

	 /* Get institution code (row[0]) */
	 Ins.InsCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get data of institution */
	 Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_EXTRA_DATA);

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
      Lay_ShowErrorAndExit ("Error initializing institution.");

   /***** Allocate memory for institution *****/
   if ((Ins_EditingIns = (struct Instit *) malloc (sizeof (struct Instit))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for institution.");

   /***** Reset institution *****/
   Ins_EditingIns->InsCod             = -1L;
   Ins_EditingIns->CtyCod             = -1L;
   Ins_EditingIns->ShrtName[0]        = '\0';
   Ins_EditingIns->FullName[0]        = '\0';
   Ins_EditingIns->WWW[0]             = '\0';
   Ins_EditingIns->Ctrs.Num           = 0;
   Ins_EditingIns->Ctrs.Lst           = NULL;
   Ins_EditingIns->Ctrs.SelectedOrder = Ctr_ORDER_DEFAULT;
   Ins_EditingIns->NumDpts            = 0;
   Ins_EditingIns->NumDegs            = 0;
   Ins_EditingIns->NumUsrs            = 0;
   Ins_EditingIns->NumUsrsWhoClaimToBelongToIns = 0;
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
