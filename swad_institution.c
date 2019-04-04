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
#include "swad_institution.h"
#include "swad_language.h"
#include "swad_logo.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_table.h"
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
      /***** Start box and table *****/
      Box_StartBoxTable (NULL,Txt_Institutions_with_pending_centres,NULL,
                         Hlp_SYSTEM_Hierarchy_pending,Box_NOT_CLOSABLE,2);

      /***** Write heading *****/
      fprintf (Gbl.F.Out,"<tr>"
                         "<th class=\"LEFT_MIDDLE\">"
                         "%s"
                         "</th>"
                         "<th class=\"RIGHT_MIDDLE\">"
                         "%s"
                         "</th>"
                         "</tr>",
               Txt_Institution,
               Txt_Centres_ABBREVIATION);

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
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"LEFT_MIDDLE %s\">",
                  BgColor);
         Ins_DrawInstitutionLogoAndNameWithLink (&Ins,ActSeeCtr,
                                                 "DAT_NOBR","CENTER_MIDDLE");
         fprintf (Gbl.F.Out,"</td>");

         /* Number of pending centres (row[1]) */
         fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE %s\">"
                            "%s"
                            "</td>"
	                    "</tr>",
                  BgColor,row[1]);

         Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
        }

      /***** End table and box *****/
      Box_EndBoxTable ();
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
      Frm_LinkFormSubmit (Ins->FullName,NULL,NULL);
     }
   Log_DrawLogo (Hie_INS,Ins->InsCod,Ins->FullName,
		 Size,NULL,true);
   if (PutLink)
     {
      fprintf (Gbl.F.Out,"</a>");
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

   /***** Start form *****/
   Frm_StartFormGoTo (Action);
   Ins_PutParamInsCod (Ins->InsCod);

   /***** Link to action *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Go_to_X,
	     Ins->FullName);
   Frm_LinkFormSubmit (Gbl.Title,ClassLink,NULL);

   /***** Draw institution logo *****/
   Log_DrawLogo (Hie_INS,Ins->InsCod,Ins->ShrtName,16,ClassLogo,true);

   /***** End link *****/
   fprintf (Gbl.F.Out,"&nbsp;%s</a>",Ins->FullName);

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

   /***** Start box *****/
   if (PrintView)
      Box_StartBox (NULL,NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);
   else
      Box_StartBox (NULL,NULL,Ins_PutIconsToPrintAndUpload,
		    Hlp_INSTITUTION_Information,Box_NOT_CLOSABLE);

   /***** Title *****/
   PutLink = !PrintView && Gbl.Hierarchy.Ins.WWW[0];
   fprintf (Gbl.F.Out,"<div class=\"FRAME_TITLE FRAME_TITLE_BIG\">");
   if (PutLink)
      fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
			 " class=\"FRAME_TITLE_BIG\" title=\"%s\">",
	       Gbl.Hierarchy.Ins.WWW,
	       Gbl.Hierarchy.Ins.FullName);
   Log_DrawLogo (Hie_INS,Gbl.Hierarchy.Ins.InsCod,
		 Gbl.Hierarchy.Ins.ShrtName,64,NULL,true);
   fprintf (Gbl.F.Out,"<br />%s",Gbl.Hierarchy.Ins.FullName);
   if (PutLink)
      fprintf (Gbl.F.Out,"</a>");
   fprintf (Gbl.F.Out,"</div>");

   /***** Start table *****/
   Tbl_StartTableWide (2);

   /***** Country *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"RIGHT_MIDDLE\">"
		      "<label for=\"OthCtyCod\" class=\"%s\">%s:</label>"
		      "</td>"
		      "<td class=\"DAT LEFT_MIDDLE\">",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_Country);

   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      // Only system admins can move an institution to another country
     {
      /* Get list of countries */
      Cty_GetListCountries (Cty_GET_BASIC_DATA);

      /* Put form to select country */
      Frm_StartForm (ActChgInsCtyCfg);
      fprintf (Gbl.F.Out,"<select id=\"OthCtyCod\" name=\"OthCtyCod\""
			 " class=\"INPUT_SHORT_NAME\""
			 " onchange=\"document.getElementById('%s').submit();\">",
	       Gbl.Form.Id);
      for (NumCty = 0;
	   NumCty < Gbl.Hierarchy.Sys.Ctys.Num;
	   NumCty++)
	 fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
		  Gbl.Hierarchy.Sys.Ctys.Lst[NumCty].CtyCod,
		  Gbl.Hierarchy.Sys.Ctys.Lst[NumCty].CtyCod == Gbl.Hierarchy.Cty.CtyCod ? " selected=\"selected\"" :
									     "",
		  Gbl.Hierarchy.Sys.Ctys.Lst[NumCty].Name[Gbl.Prefs.Language]);
      fprintf (Gbl.F.Out,"</select>");
      Frm_EndForm ();

      /* Free list of countries */
      Cty_FreeListCountries ();
     }
   else	// I can not move institution to another country
      fprintf (Gbl.F.Out,"%s",Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);

   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /***** Institution full name *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"RIGHT_MIDDLE\">"
		      "<label for=\"FullName\" class=\"%s\">%s:</label>"
		      "</td>"
		      "<td class=\"DAT_N LEFT_MIDDLE\">",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_Institution);
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      // Only system admins can edit institution full name
     {
      /* Form to change institution full name */
      Frm_StartForm (ActRenInsFulCfg);
      fprintf (Gbl.F.Out,"<input type=\"text\""
			 " id=\"FullName\" name=\"FullName\""
			 " maxlength=\"%u\" value=\"%s\""
			 " class=\"INPUT_FULL_NAME\""
			 " onchange=\"document.getElementById('%s').submit();\" />",
	       Hie_MAX_CHARS_FULL_NAME,
	       Gbl.Hierarchy.Ins.FullName,
	       Gbl.Form.Id);
      Frm_EndForm ();
     }
   else	// I can not edit institution full name
      fprintf (Gbl.F.Out,"%s",Gbl.Hierarchy.Ins.FullName);
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /***** Institution short name *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"RIGHT_MIDDLE\">"
		      "<label for=\"ShortName\" class=\"%s\">%s:</label>"
		      "</td>"
		      "<td class=\"DAT_N LEFT_MIDDLE\">",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_Short_name);
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      // Only system admins can edit institution short name
     {
      /* Form to change institution short name */
      Frm_StartForm (ActRenInsShoCfg);
      fprintf (Gbl.F.Out,"<input type=\"text\""
			 " id=\"ShortName\" name=\"ShortName\""
			 " maxlength=\"%u\" value=\"%s\""
			 " class=\"INPUT_SHORT_NAME\""
			 " onchange=\"document.getElementById('%s').submit();\" />",
	       Hie_MAX_CHARS_SHRT_NAME,
	       Gbl.Hierarchy.Ins.ShrtName,
	       Gbl.Form.Id);
      Frm_EndForm ();
     }
   else	// I can not edit institution short name
      fprintf (Gbl.F.Out,"%s",Gbl.Hierarchy.Ins.ShrtName);
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /***** Institution WWW *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"RIGHT_MIDDLE\">"
		      "<label for=\"WWW\" class=\"%s\">%s:</label>"
		      "</td>"
		      "<td class=\"DAT LEFT_MIDDLE\">",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_Web);
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      // Only institution admins and system admins
      // can change institution WWW
     {
      /* Form to change institution WWW */
      Frm_StartForm (ActChgInsWWWCfg);
      fprintf (Gbl.F.Out,"<input type=\"url\" id=\"WWW\" name=\"WWW\""
			 " maxlength=\"%u\" value=\"%s\""
			 " class=\"INPUT_WWW\""
			 " onchange=\"document.getElementById('%s').submit();\" />",
	       Cns_MAX_CHARS_WWW,
	       Gbl.Hierarchy.Ins.WWW,
	       Gbl.Form.Id);
      Frm_EndForm ();
     }
   else	// I can not change institution WWW
      fprintf (Gbl.F.Out,"<div class=\"EXTERNAL_WWW_LONG\">"
			 "<a href=\"%s\" target=\"_blank\" class=\"DAT\">"
			 "%s"
			 "</a>"
			 "</div>",
	       Gbl.Hierarchy.Ins.WWW,
	       Gbl.Hierarchy.Ins.WWW);
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /***** Shortcut to the institution *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s RIGHT_MIDDLE\">"
		      "%s:"
		      "</td>"
		      "<td class=\"LEFT_MIDDLE\">"
		      "<a href=\"%s/%s?ins=%ld\" class=\"DAT\" target=\"_blank\">"
		      "%s/%s?ins=%ld"
		      "</a>"
		      "</td>"
		      "</tr>",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_Shortcut,
	    Cfg_URL_SWAD_CGI,
	    Lan_STR_LANG_ID[Gbl.Prefs.Language],
	    Gbl.Hierarchy.Ins.InsCod,
	    Cfg_URL_SWAD_CGI,
	    Lan_STR_LANG_ID[Gbl.Prefs.Language],
	    Gbl.Hierarchy.Ins.InsCod);

   if (PrintView)
     {
      /***** QR code with link to the institution *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
			 "%s:"
			 "</td>"
			 "<td class=\"LEFT_MIDDLE\">",
	       The_ClassFormInBox[Gbl.Prefs.Theme],
	       Txt_QR_code);
      QR_LinkTo (250,"ins",Gbl.Hierarchy.Ins.InsCod);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }
   else
     {
      /***** Number of users who claim to belong to this institution *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
			 "%s:"
			 "</td>"
			 "<td class=\"DAT LEFT_MIDDLE\">"
			 "%u"
			 "</td>"
			 "</tr>",
	       The_ClassFormInBox[Gbl.Prefs.Theme],
	       Txt_Users_of_the_institution,
	       Usr_GetNumUsrsWhoClaimToBelongToIns (Gbl.Hierarchy.Ins.InsCod));

      /***** Number of centres *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
			 "%s:"
			 "</td>"
			 "<td class=\"LEFT_MIDDLE\">",
	       The_ClassFormInBox[Gbl.Prefs.Theme],
	       Txt_Centres);

      /* Form to go to see centres of this institution */
      Frm_StartFormGoTo (ActSeeCtr);
      Ins_PutParamInsCod (Gbl.Hierarchy.Ins.InsCod);
      snprintf (Gbl.Title,sizeof (Gbl.Title),
		Txt_Centres_of_INSTITUTION_X,
		Gbl.Hierarchy.Ins.ShrtName);
      Frm_LinkFormSubmit (Gbl.Title,"DAT",NULL);
      fprintf (Gbl.F.Out,"%u</a>",
	       Ctr_GetNumCtrsInIns (Gbl.Hierarchy.Ins.InsCod));
      Frm_EndForm ();

      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      /***** Number of degrees *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
			 "%s:"
			 "</td>"
			 "<td class=\"DAT LEFT_MIDDLE\">"
			 "%u"
			 "</td>"
			 "</tr>",
	       The_ClassFormInBox[Gbl.Prefs.Theme],
	       Txt_Degrees,
	       Deg_GetNumDegsInIns (Gbl.Hierarchy.Ins.InsCod));

      /***** Number of courses *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
			 "%s:"
			 "</td>"
			 "<td class=\"DAT LEFT_MIDDLE\">"
			 "%u"
			 "</td>"
			 "</tr>",
	       The_ClassFormInBox[Gbl.Prefs.Theme],
	       Txt_Courses,
	       Crs_GetNumCrssInIns (Gbl.Hierarchy.Ins.InsCod));

      /***** Number of departments *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
			 "%s:"
			 "</td>"
			 "<td class=\"DAT LEFT_MIDDLE\">"
			 "%u"
			 "</td>"
			 "</tr>",
	       The_ClassFormInBox[Gbl.Prefs.Theme],
	       Txt_Departments,
	       Dpt_GetNumDepartmentsInInstitution (Gbl.Hierarchy.Ins.InsCod));

      /***** Number of users in courses of this institution *****/
      Ins_ShowNumUsrsInCrssOfIns (Rol_TCH);
      Ins_ShowNumUsrsInCrssOfIns (Rol_NET);
      Ins_ShowNumUsrsInCrssOfIns (Rol_STD);
      Ins_ShowNumUsrsInCrssOfIns (Rol_UNK);
     }

   /***** End table *****/
   Tbl_EndTable ();

   /***** End box *****/
   Box_EndBox ();
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

   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s RIGHT_MIDDLE\">"
		      "%s:"
		      "</td>"
		      "<td class=\"DAT LEFT_MIDDLE\">"
		      "%u"
		      "</td>"
		      "</tr>",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    (Role == Rol_UNK) ? Txt_Users_in_courses :
		                Txt_ROLES_PLURAL_Abc[Role][Usr_SEX_UNKNOWN],
            Usr_GetNumUsrsInCrssOfIns (Role,Gbl.Hierarchy.Ins.InsCod));
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

   /***** Start box *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Institutions_of_COUNTRY_X,
	     Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);
   Box_StartBox (NULL,Gbl.Title,Ins_PutIconsListingInstitutions,
                 Hlp_COUNTRY_Institutions,Box_NOT_CLOSABLE);

   if (Gbl.Hierarchy.Cty.Inss.Num)	// There are institutions in the current country
     {
      /***** Start table *****/
      Tbl_StartTableWideMargin (2);
      Ins_PutHeadInstitutionsForSeeing (true);	// Order selectable

      /***** Write all the institutions and their nuber of users *****/
      for (NumIns = 0;
	   NumIns < Gbl.Hierarchy.Cty.Inss.Num;
	   NumIns++)
	 Ins_ListOneInstitutionForSeeing (&(Gbl.Hierarchy.Cty.Inss.Lst[NumIns]),NumIns + 1);

      /***** End table *****/
      Tbl_EndTable ();
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

   Box_EndBox ();
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
      TxtClassStrong = "DAT_LIGHT";
     }
   else
     {
      TxtClassNormal = "DAT";
      TxtClassStrong = "DAT_N";
     }
   BgColor = (Ins->InsCod == Gbl.Hierarchy.Ins.InsCod) ? "LIGHT_BLUE" :
                                                          Gbl.ColorRows[Gbl.RowEvenOdd];

   /***** Number of institution in this list *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s RIGHT_MIDDLE %s\">"
                      "%u"
                      "</td>",
	    TxtClassNormal,BgColor,
            NumIns);

   /***** Institution logo and name *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE %s\">",BgColor);
   Ins_DrawInstitutionLogoAndNameWithLink (Ins,ActSeeCtr,
                                           TxtClassStrong,"CENTER_MIDDLE");
   fprintf (Gbl.F.Out,"</td>");

   /***** Stats *****/
   /* Number of users who claim to belong to this institution */
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
	              "%u"
	              "</td>",
	    TxtClassNormal,BgColor,Ins->NumUsrsWhoClaimToBelongToIns);

   /* Number of centres in this institution */
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
	              "%u"
	              "</td>",
	    TxtClassNormal,BgColor,Ins->Ctrs.Num);

   /* Number of degrees in this institution */
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
	              "%u"
	              "</td>",
	    TxtClassNormal,BgColor,Ins->NumDegs);

   /* Number of courses in this institution */
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
	              "%u"
	              "</td>",
	    TxtClassNormal,BgColor,Ins->NumCrss);

   /* Number of departments in this institution */
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
	              "%u"
	              "</td>",
	    TxtClassNormal,BgColor,Ins->NumDpts);

   /* Number of users in courses of this institution */
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
	              "%u"
	              "</td>",
	    TxtClassNormal,BgColor,Ins->NumUsrs);

   /***** Institution status *****/
   StatusTxt = Ins_GetStatusTxtFromStatusBits (Ins->Status);
   fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE %s\">",
	    TxtClassNormal,BgColor);
   if (StatusTxt != Ins_STATUS_ACTIVE) // If active ==> do not show anything
      fprintf (Gbl.F.Out,"%s",Txt_INSTITUTION_STATUS[StatusTxt]);
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

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

   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>");
   for (Order = Ins_ORDER_BY_INSTITUTION;
	Order <= Ins_ORDER_BY_NUM_USRS;
	Order++)
     {
      fprintf (Gbl.F.Out,"<th class=\"%s\">",
               Order == Ins_ORDER_BY_INSTITUTION ? "LEFT_MIDDLE" :
        	                                   "RIGHT_MIDDLE");
      if (OrderSelectable)
	{
	 Frm_StartForm (ActSeeIns);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Frm_LinkFormSubmit (Txt_INSTITUTIONS_HELP_ORDER[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Hierarchy.Cty.Inss.SelectedOrder)
	    fprintf (Gbl.F.Out,"<u>");
	}
      fprintf (Gbl.F.Out,"%s",Txt_INSTITUTIONS_ORDER[Order]);
      if (OrderSelectable)
	{
	 if (Order == Gbl.Hierarchy.Cty.Inss.SelectedOrder)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Frm_EndForm ();
	}
      fprintf (Gbl.F.Out,"</th>");
     }
   fprintf (Gbl.F.Out,"<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s+<br />%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "</th>"
                      "</tr>",
            Txt_Centres_ABBREVIATION,
            Txt_Degrees_ABBREVIATION,
            Txt_Courses_ABBREVIATION,
            Txt_Departments_ABBREVIATION,
            Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH],
            Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
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
   extern const char *Hlp_COUNTRY_Institutions;
   extern const char *Txt_Institutions_of_COUNTRY_X;

   /***** Get list of institutions *****/
   Ins_GetListInstitutions (Gbl.Hierarchy.Cty.CtyCod,Ins_GET_EXTRA_DATA);

   /***** Write menu to select country *****/
   Hie_WriteMenuHierarchy ();

   /***** Start box *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Institutions_of_COUNTRY_X,
             Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);
   Box_StartBox (NULL,Gbl.Title,Ins_PutIconsEditingInstitutions,
                 Hlp_COUNTRY_Institutions,Box_NOT_CLOSABLE);

   /***** Put a form to create a new institution *****/
   Ins_PutFormToCreateInstitution ();

   /***** Forms to edit current institutions *****/
   if (Gbl.Hierarchy.Cty.Inss.Num)
      Ins_ListInstitutionsForEdition ();

   /***** End box *****/
   Box_EndBox ();

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
      "FullName",		// Ins_ORDER_BY_INSTITUTION
      "NumUsrs DESC,FullName",	// Ins_ORDER_BY_NUM_USRS
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
   fprintf (Gbl.F.Out,"%s (%s)",Ins.ShrtName,CtyName);
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
      free ((void *) Gbl.Hierarchy.Cty.Inss.Lst);
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

   /***** Start form *****/
   Frm_StartFormGoTo (ActSeeCtr);
   fprintf (Gbl.F.Out,"<select id=\"ins\" name=\"ins\" style=\"width:175px;\"");
   if (Gbl.Hierarchy.Cty.CtyCod > 0)
      fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
               Gbl.Form.Id);
   else
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,"><option value=\"\"");
   if (Gbl.Hierarchy.Ins.InsCod < 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out," disabled=\"disabled\">[%s]</option>",
            Txt_Institution);

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
         fprintf (Gbl.F.Out,"<option value=\"%ld\"",InsCod);
         if (Gbl.Hierarchy.Ins.InsCod > 0 && InsCod == Gbl.Hierarchy.Ins.InsCod)
            fprintf (Gbl.F.Out," selected=\"selected\"");
         fprintf (Gbl.F.Out,">%s</option>",row[1]);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** End form *****/
   fprintf (Gbl.F.Out,"</select>");
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

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Write heading *****/
   Tbl_StartTableWide (2);
   Ins_PutHeadInstitutionsForEdition ();

   /***** Write all the institutions *****/
   for (NumIns = 0;
	NumIns < Gbl.Hierarchy.Cty.Inss.Num;
	NumIns++)
     {
      Ins = &Gbl.Hierarchy.Cty.Inss.Lst[NumIns];

      ICanEdit = Ins_CheckIfICanEdit (Ins);

      /* Put icon to remove institution */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"BM\">");
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
      fprintf (Gbl.F.Out,"</td>");

      /* Institution code */
      fprintf (Gbl.F.Out,"<td class=\"DAT CODE\">"
	                 "%ld"
	                 "</td>",
               Ins->InsCod);

      /* Institution logo */
      fprintf (Gbl.F.Out,"<td title=\"%s\" class=\"LEFT_MIDDLE\""
	                 " style=\"width:25px;\">",
               Ins->FullName);
      Log_DrawLogo (Hie_INS,Ins->InsCod,Ins->ShrtName,20,NULL,true);
      fprintf (Gbl.F.Out,"</td>");

      /* Institution short name */
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (ICanEdit)
	{
	 Frm_StartForm (ActRenInsSho);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_SHORT_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Hie_MAX_CHARS_SHRT_NAME,Ins->ShrtName,
		  Gbl.Form.Id);
	 Frm_EndForm ();
	}
      else
	 fprintf (Gbl.F.Out,"%s",Ins->ShrtName);
      fprintf (Gbl.F.Out,"</td>");

      /* Institution full name */
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (ICanEdit)
	{
	 Frm_StartForm (ActRenInsFul);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_FULL_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Hie_MAX_CHARS_FULL_NAME,
		  Ins->FullName,
		  Gbl.Form.Id);
	 Frm_EndForm ();
	}
      else
	 fprintf (Gbl.F.Out,"%s",Ins->FullName);
      fprintf (Gbl.F.Out,"</td>");

      /* Institution WWW */
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (ICanEdit)
	{
	 Frm_StartForm (ActChgInsWWW);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 fprintf (Gbl.F.Out,"<input type=\"url\" name=\"WWW\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_WWW\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Cns_MAX_CHARS_WWW,
		  Ins->WWW,
		  Gbl.Form.Id);
	 Frm_EndForm ();
	 fprintf (Gbl.F.Out,"</td>");
	}
      else
	{
         Str_Copy (WWW,Ins->WWW,
                   Cns_MAX_BYTES_WWW);
         fprintf (Gbl.F.Out,"<div class=\"EXTERNAL_WWW_SHORT\">"
                            "<a href=\"%s\" target=\"_blank\""
                            " class=\"DAT\" title=\"%s\">"
                            "%s"
                            "</a>"
                            "</div>",
                  Ins->WWW,Ins->WWW,WWW);
	}
      fprintf (Gbl.F.Out,"</td>");

      /* Number of users who claim to belong to this institution */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%u"
	                 "</td>",
               Ins->NumUsrsWhoClaimToBelongToIns);

      /* Number of centres */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%u"
	                 "</td>",
               Ins->Ctrs.Num);

      /* Number of users in courses of this institution */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%u"
	                 "</td>",
               Ins->NumUsrs);

      /* Institution requester */
      UsrDat.UsrCod = Ins->RequesterUsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);
      fprintf (Gbl.F.Out,"<td class=\"DAT INPUT_REQUESTER LEFT_TOP\">");
      Msg_WriteMsgAuthor (&UsrDat,true,NULL);
      fprintf (Gbl.F.Out,"</td>");

      /* Institution status */
      StatusTxt = Ins_GetStatusTxtFromStatusBits (Ins->Status);
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM &&
	  StatusTxt == Ins_STATUS_PENDING)
	{
	 Frm_StartForm (ActChgInsSta);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 fprintf (Gbl.F.Out,"<select name=\"Status\" class=\"INPUT_STATUS\""
			    " onchange=\"document.getElementById('%s').submit();\">"
			    "<option value=\"%u\" selected=\"selected\">%s</option>"
			    "<option value=\"%u\">%s</option>"
			    "</select>",
		  Gbl.Form.Id,
		  (unsigned) Ins_GetStatusBitsFromStatusTxt (Ins_STATUS_PENDING),
		  Txt_INSTITUTION_STATUS[Ins_STATUS_PENDING],
		  (unsigned) Ins_GetStatusBitsFromStatusTxt (Ins_STATUS_ACTIVE),
		  Txt_INSTITUTION_STATUS[Ins_STATUS_ACTIVE]);
	 Frm_EndForm ();
	}
      else if (StatusTxt != Ins_STATUS_ACTIVE)	// If active ==> do not show anything
	 fprintf (Gbl.F.Out,"%s",Txt_INSTITUTION_STATUS[StatusTxt]);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   /***** End table *****/
   Tbl_EndTable ();

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
   Par_PutHiddenParamLong ("ins",InsCod);
  }

/*****************************************************************************/
/***************** Write parameter with code of institution ******************/
/*****************************************************************************/

static void Ins_PutParamOtherInsCod (long InsCod)
  {
   Par_PutHiddenParamLong ("OthInsCod",InsCod);
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
   struct Instit Ins;
   char PathIns[PATH_MAX + 1];

   /***** Get institution code *****/
   Ins.InsCod = Ins_GetAndCheckParamOtherInsCod (1);

   /***** Get data of the institution from database *****/
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_EXTRA_DATA);

   /***** Check if this institution has users *****/
   if (!Ins_CheckIfICanEdit (&Ins))
      Lay_NoPermissionExit ();
   else if (Ins.Ctrs.Num ||
            Ins.NumUsrsWhoClaimToBelongToIns ||
            Ins.NumUsrs)	// Institution has centres or users ==> don't remove
      Ale_ShowAlert (Ale_WARNING,Txt_To_remove_an_institution_you_must_first_remove_all_centres_and_users_in_the_institution);
   else	// Institution has no users ==> remove it
     {
      /***** Remove all the threads and posts in forums of the institution *****/
      For_RemoveForums (Hie_INS,Ins.InsCod);

      /***** Remove surveys of the institution *****/
      Svy_RemoveSurveys (Hie_INS,Ins.InsCod);

      /***** Remove information related to files in institution *****/
      Brw_RemoveInsFilesFromDB (Ins.InsCod);

      /***** Remove directories of the institution *****/
      snprintf (PathIns,sizeof (PathIns),
	        "%s/%02u/%u",
	        Cfg_PATH_INS_PUBLIC,
	        (unsigned) (Ins.InsCod % 100),
	        (unsigned) Ins.InsCod);
      Fil_RemoveTree (PathIns);

      /***** Remove institution *****/
      DB_QueryDELETE ("can not remove an institution",
		      "DELETE FROM institutions WHERE InsCod=%ld",
		      Ins.InsCod);

      /***** Flush caches *****/
      Ins_FlushCacheShortNameOfInstitution ();
      Ins_FlushCacheFullNameAndCtyOfInstitution ();

      /***** Write message to show the change made *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_Institution_X_removed,
                     Ins.FullName);
     }

   /***** Show the form again *****/
   Ins_EditInstitutions ();
  }

/*****************************************************************************/
/***************** Change the short name of an institution *******************/
/*****************************************************************************/

void Ins_RenameInsShort (void)
  {
   Gbl.Inss.EditingIns.InsCod = Ins_GetAndCheckParamOtherInsCod (1);
   Ins_RenameInstitution (&Gbl.Inss.EditingIns,Cns_SHRT_NAME);
  }

void Ins_RenameInsShortInConfig (void)
  {
   Ins_RenameInstitution (&Gbl.Hierarchy.Ins,Cns_SHRT_NAME);
  }

/*****************************************************************************/
/***************** Change the full name of an institution ********************/
/*****************************************************************************/

void Ins_RenameInsFull (void)
  {
   Gbl.Inss.EditingIns.InsCod = Ins_GetAndCheckParamOtherInsCod (1);
   Ins_RenameInstitution (&Gbl.Inss.EditingIns,Cns_FULL_NAME);
  }

void Ins_RenameInsFullInConfig (void)
  {
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

   /***** Get parameters from form *****/
   /* Get the code of the institution */
   Gbl.Inss.EditingIns.InsCod = Ins_GetAndCheckParamOtherInsCod (1);

   /* Get the new WWW for the institution */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get data of institution *****/
   Ins_GetDataOfInstitutionByCod (&Gbl.Inss.EditingIns,Ins_GET_BASIC_DATA);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ins_UpdateInsWWWDB (Gbl.Inss.EditingIns.InsCod,NewWWW);
      Str_Copy (Gbl.Inss.EditingIns.WWW,NewWWW,
                Cns_MAX_BYTES_WWW);

      /***** Write message to show the change made
	     and put button to go to institution changed *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
		       Txt_The_new_web_address_is_X,
		       NewWWW);
      Ins_ShowAlertAndButtonToGoToIns ();
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_the_web_address_empty);

   /***** Show the form again *****/
   Ins_EditInstitutions ();
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

   /***** Get parameters from form *****/
   /* Get institution code */
   Gbl.Inss.EditingIns.InsCod = Ins_GetAndCheckParamOtherInsCod (1);

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
   Ins_GetDataOfInstitutionByCod (&Gbl.Inss.EditingIns,Ins_GET_BASIC_DATA);

   /***** Update status in table of institutions *****/
   DB_QueryUPDATE ("can not update the status of an institution",
		   "UPDATE institutions SET Status=%u WHERE InsCod=%ld",
                   (unsigned) Status,Gbl.Inss.EditingIns.InsCod);
   Gbl.Inss.EditingIns.Status = Status;

   /***** Write message to show the change made
	  and put button to go to institution changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
		    Txt_The_status_of_the_institution_X_has_changed,
		    Gbl.Inss.EditingIns.ShrtName);
   Ins_ShowAlertAndButtonToGoToIns ();

   /***** Show the form again *****/
   Ins_EditInstitutions ();
  }

/*****************************************************************************/
/*********** Show message of success after changing an institution ***********/
/*****************************************************************************/

void Ins_ContEditAfterChgIns (void)
  {
   /***** Write message to show the change made
	  and put button to go to institution changed *****/
   Ins_ShowAlertAndButtonToGoToIns ();

   /***** Show the form again *****/
   Ins_EditInstitutions ();
  }

/*****************************************************************************/
/*************** Write message to show the change made       *****************/
/*************** and put button to go to institution changed *****************/
/*****************************************************************************/
// Gbl.Degs.EditingDeg is the degree that is beeing edited
// Gbl.Hierarchy.Deg is the current degree

static void Ins_ShowAlertAndButtonToGoToIns (void)
  {
   extern const char *Txt_Go_to_X;

   // If the institution beeing edited is different to the current one...
   if (Gbl.Inss.EditingIns.InsCod != Gbl.Hierarchy.Ins.InsCod)
      /***** Alert with button to go to degree *****/
      Ale_ShowLastAlertAndButton (ActSeeCtr,NULL,NULL,Ins_PutParamGoToIns,
                                  Btn_CONFIRM_BUTTON,Gbl.Title);
   else
      /***** Alert *****/
      Ale_ShowAlerts (NULL);
  }

static void Ins_PutParamGoToIns (void)
  {
   Ins_PutParamInsCod (Gbl.Inss.EditingIns.InsCod);
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

   /***** Start form *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      Frm_StartForm (ActNewIns);
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      Frm_StartForm (ActReqIns);
   else
      Lay_ShowErrorAndExit ("You can not edit institutions.");

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_New_institution,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Ins_PutHeadInstitutionsForEdition ();

   /***** Column to remove institution, disabled here *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"BM\"></td>");

   /***** Institution code *****/
   fprintf (Gbl.F.Out,"<td class=\"CODE\"></td>");

   /***** Institution logo *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\" style=\"width:25px;\">");
   Log_DrawLogo (Hie_INS,-1L,"",20,NULL,true);
   fprintf (Gbl.F.Out,"</td>");

   /***** Institution short name *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" name=\"ShortName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_SHORT_NAME\""
                      " required=\"required\" />"
                      "</td>",
            Hie_MAX_CHARS_SHRT_NAME,Gbl.Inss.EditingIns.ShrtName);

   /***** Institution full name *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" name=\"FullName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_FULL_NAME\""
                      " required=\"required\" />"
                      "</td>",
            Hie_MAX_CHARS_FULL_NAME,Gbl.Inss.EditingIns.FullName);

   /***** Institution WWW *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"url\" name=\"WWW\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_WWW\""
                      " required=\"required\" />"
                      "</td>",
            Cns_MAX_CHARS_WWW,Gbl.Inss.EditingIns.WWW);

   /***** Number of users who claim to belong to this institution ****/
   fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	              "0"
	              "</td>");

   /***** Number of centres *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	              "0"
	              "</td>");

   /***** Number of users in courses of this institution ****/
   fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	              "0"
	              "</td>");

   /***** Institution requester *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT INPUT_REQUESTER LEFT_TOP\">");
   Msg_WriteMsgAuthor (&Gbl.Usrs.Me.UsrDat,true,NULL);
   fprintf (Gbl.F.Out,"</td>");

   /***** Institution status *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">"
	              "</td>"
		      "</tr>");

   /***** End table, send button and end box *****/
   Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_institution);

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

   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th></th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s+<br />%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "</th>"
                      "</tr>",
            Txt_Code,
            Txt_Short_name_of_the_institution,
            Txt_Full_name_of_the_institution,
            Txt_WWW,
            Txt_Users,
            Txt_Centres_ABBREVIATION,
            Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH],
            Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD],
            Txt_Requester);
  }

/*****************************************************************************/
/*************** Receive form to request a new institution *******************/
/*****************************************************************************/

void Ins_RecFormReqIns (void)
  {
   Ins_RecFormRequestOrCreateIns ((unsigned) Ins_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/***************** Receive form to create a new institution ******************/
/*****************************************************************************/

void Ins_RecFormNewIns (void)
  {
   Ins_RecFormRequestOrCreateIns (0);
  }

/*****************************************************************************/
/*********** Receive form to request or create a new institution *************/
/*****************************************************************************/

static void Ins_RecFormRequestOrCreateIns (unsigned Status)
  {
   extern const char *Txt_The_institution_X_already_exists;
   extern const char *Txt_You_must_specify_the_web_address_of_the_new_institution;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_institution;

   /***** Get parameters from form *****/
   /* Set institution country */
   Gbl.Inss.EditingIns.CtyCod = Gbl.Hierarchy.Cty.CtyCod;

   /* Get institution short name */
   Par_GetParToText ("ShortName",Gbl.Inss.EditingIns.ShrtName,Hie_MAX_BYTES_SHRT_NAME);

   /* Get institution full name */
   Par_GetParToText ("FullName",Gbl.Inss.EditingIns.FullName,Hie_MAX_BYTES_FULL_NAME);

   /* Get institution WWW */
   Par_GetParToText ("WWW",Gbl.Inss.EditingIns.WWW,Cns_MAX_BYTES_WWW);

   if (Gbl.Inss.EditingIns.ShrtName[0] &&
       Gbl.Inss.EditingIns.FullName[0])	// If there's a institution name
     {
      if (Gbl.Inss.EditingIns.WWW[0])
        {
         /***** If name of institution was in database... *****/
         if (Ins_CheckIfInsNameExistsInCty ("ShortName",Gbl.Inss.EditingIns.ShrtName,-1L,Gbl.Hierarchy.Cty.CtyCod))
            Ale_ShowAlert (Ale_WARNING,Txt_The_institution_X_already_exists,
                           Gbl.Inss.EditingIns.ShrtName);
         else if (Ins_CheckIfInsNameExistsInCty ("FullName",Gbl.Inss.EditingIns.FullName,-1L,Gbl.Hierarchy.Cty.CtyCod))
            Ale_ShowAlert (Ale_WARNING,Txt_The_institution_X_already_exists,
                           Gbl.Inss.EditingIns.FullName);
         else	// Add new institution to database
            Ins_CreateInstitution (Status);
        }
      else	// If there is not a web
         Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_web_address_of_the_new_institution);
     }
   else	// If there is not a institution name
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_institution);

   /***** Show the form again *****/
   Ins_EditInstitutions ();
  }

/*****************************************************************************/
/************************** Create a new institution *************************/
/*****************************************************************************/
// Gbl.Inss.EditingIns must hold the institution beeing edited

static void Ins_CreateInstitution (unsigned Status)
  {
   extern const char *Txt_Created_new_institution_X;

   /***** Create a new institution *****/
   Gbl.Inss.EditingIns.InsCod =
   DB_QueryINSERTandReturnCode ("can not create institution",
				"INSERT INTO institutions"
				" (CtyCod,Status,RequesterUsrCod,ShortName,FullName,WWW)"
				" VALUES"
				" (%ld,%u,%ld,'%s','%s','%s')",
				Gbl.Inss.EditingIns.CtyCod,
				Status,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Gbl.Inss.EditingIns.ShrtName,
				Gbl.Inss.EditingIns.FullName,
				Gbl.Inss.EditingIns.WWW);

   /***** Write message to show the change made
	  and put button to go to institution created *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
		    Txt_Created_new_institution_X,
		    Gbl.Inss.EditingIns.FullName);
   Ins_ShowAlertAndButtonToGoToIns ();
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
      /***** Start box and table *****/
      /* Number of institutions found */
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        "%u %s",
                NumInss,NumInss == 1 ? Txt_institution :
				       Txt_institutions);
      Box_StartBoxTable (NULL,Gbl.Title,NULL,
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
      Box_EndBoxTable ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }
