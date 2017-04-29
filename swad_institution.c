// swad_institution.c: institutions

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

#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_config.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
#include "swad_institution.h"
#include "swad_logo.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_text.h"
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

static void Ins_ListInstitutions (void);
static bool Ins_CheckIfICanCreateInstitutions (void);
static void Ins_PutIconsListInstitutions (void);
static void Ins_PutIconToEditInstitutions (void);
static void Ins_ListOneInstitutionForSeeing (struct Instit *Ins,unsigned NumIns);
static void Ins_PutHeadInstitutionsForSeeing (bool OrderSelectable);
static void Ins_GetParamInsOrder (void);

static void Ins_PutIconToViewInstitutions (void);

static void Ins_GetFullNameAndCtyOfInstitutionByCod (struct Instit *Ins,
                                               char CtyName[Hie_MAX_BYTES_FULL_NAME + 1]);

static void Ins_ListInstitutionsForEdition (void);
static bool Ins_CheckIfICanEdit (struct Instit *Ins);
static Ins_StatusTxt_t Ins_GetStatusTxtFromStatusBits (Ins_Status_t Status);
static Ins_Status_t Ins_GetStatusBitsFromStatusTxt (Ins_StatusTxt_t StatusTxt);

static void Ins_PutParamOtherInsCod (long InsCod);
static long Ins_GetParamOtherInsCod (void);

static void Ins_RenameInstitution (struct Instit *Ins,Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Ins_CheckIfInsNameExistsInCty (const char *FieldName,const char *Name,long InsCod,long CtyCod);
static void Ins_UpdateInsNameDB (long InsCod,const char *FieldName,const char *NewInsName);

static void Ins_UpdateInsCtyDB (long InsCod,long CtyCod);
static void Ins_UpdateInsWWWDB (long InsCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1]);
static void Ins_PutButtonToGoToIns (struct Instit *Ins);

static void Ins_PutFormToCreateInstitution (void);
static void Ins_PutHeadInstitutionsForEdition (void);
static void Ins_RecFormRequestOrCreateIns (unsigned Status);
static void Ins_CreateInstitution (struct Instit *Ins,unsigned Status);

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
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumInss;
   unsigned NumIns;
   struct Instit Ins;
   const char *BgColor;

   /***** Get institutions with pending centres *****/
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_INS_ADM:
         sprintf (Query,"SELECT centres.InsCod,COUNT(*)"
                        " FROM centres,ins_admin,institutions"
                        " WHERE (centres.Status & %u)<>0"
                        " AND centres.InsCod=ins_admin.InsCod AND ins_admin.UsrCod=%ld"
                        " AND centres.InsCod=institutions.InsCod"
                        " GROUP BY centres.InsCod ORDER BY institutions.ShortName",
                  (unsigned) Ctr_STATUS_BIT_PENDING,Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Rol_SYS_ADM:
         sprintf (Query,"SELECT centres.InsCod,COUNT(*)"
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
   if ((NumInss = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get institutions with pending centres")))
     {
      /***** Write heading *****/
      Lay_StartRoundFrameTable (NULL,Txt_Institutions_with_pending_centres,
                                NULL,Hlp_SYSTEM_Hierarchy_pending,2);
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
         BgColor = (Ins.InsCod == Gbl.CurrentIns.Ins.InsCod) ? "LIGHT_BLUE" :
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

      /***** End table *****/
      Lay_EndRoundFrameTable ();
     }
   else
      Lay_ShowAlert (Lay_INFO,Txt_There_are_no_institutions_with_requests_for_centres_to_be_confirmed);

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
      Act_FormStart (ActSeeInsInf);
      Ins_PutParamInsCod (Ins->InsCod);
      Act_LinkFormSubmit (Ins->FullName,NULL,NULL);
     }
   Log_DrawLogo (Sco_SCOPE_INS,Ins->InsCod,Ins->FullName,
		 Size,NULL,true);
   if (PutLink)
     {
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
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
   Act_FormGoToStart (Action);
   Ins_PutParamInsCod (Ins->InsCod);

   /***** Link to action *****/
   sprintf (Gbl.Title,Txt_Go_to_X,Ins->FullName);
   Act_LinkFormSubmit (Gbl.Title,ClassLink,NULL);

   /***** Draw institution logo *****/
   Log_DrawLogo (Sco_SCOPE_INS,Ins->InsCod,Ins->ShrtName,20,ClassLogo,true);

   /***** End link *****/
   fprintf (Gbl.F.Out,"&nbsp;%s</a>",Ins->FullName);

   /***** End form *****/
   Act_FormEnd ();
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
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Short_name;
   extern const char *Txt_Web;
   extern const char *Txt_Shortcut;
   extern const char *Txt_STR_LANG_ID[1 + Txt_NUM_LANGUAGES];
   extern const char *Txt_QR_code;
   extern const char *Txt_Centres;
   extern const char *Txt_Centres_of_INSTITUTION_X;
   extern const char *Txt_Degrees;
   extern const char *Txt_Courses;
   extern const char *Txt_Departments;
   extern const char *Txt_Users_of_the_institution;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   unsigned NumCty;
   bool PutLink = !PrintView && Gbl.CurrentIns.Ins.WWW[0];

   if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      /***** Start frame *****/
      Lay_StartRoundFrame (NULL,NULL,
                           PrintView ? NULL :
	                               Ins_PutIconsToPrintAndUpload,
	                   PrintView ? NULL :
	                               Hlp_INSTITUTION_Information);

      /***** Title *****/
      fprintf (Gbl.F.Out,"<div class=\"FRAME_TITLE FRAME_TITLE_BIG\">");
      if (PutLink)
	 fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
	                    " class=\"FRAME_TITLE_BIG\" title=\"%s\">",
		  Gbl.CurrentIns.Ins.WWW,
		  Gbl.CurrentIns.Ins.FullName);
      Log_DrawLogo (Sco_SCOPE_INS,Gbl.CurrentIns.Ins.InsCod,
                    Gbl.CurrentIns.Ins.ShrtName,64,NULL,true);
      fprintf (Gbl.F.Out,"<br />%s",Gbl.CurrentIns.Ins.FullName);
      if (PutLink)
	 fprintf (Gbl.F.Out,"</a>");
      fprintf (Gbl.F.Out,"</div>");

      /***** Start table *****/
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE CELLS_PAD_2\">");

      /***** Country *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"RIGHT_MIDDLE\">"
	                 "<label for=\"OthCtyCod\" class=\"%s\">%s:</label>"
			 "</td>"
			 "<td class=\"DAT LEFT_MIDDLE\">",
	       The_ClassForm[Gbl.Prefs.Theme],
	       Txt_Country);

      if (!PrintView &&
	  Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
	 // Only system admins can move an institution to another country
	{
	 /* Get list of countries */
         Cty_GetListCountries (Cty_GET_BASIC_DATA);

	 /* Put form to select country */
	 Act_FormStart (ActChgInsCtyCfg);
	 fprintf (Gbl.F.Out,"<select id=\"OthCtyCod\" name=\"OthCtyCod\""
			    " class=\"INPUT_SHORT_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\">",
		  Gbl.Form.Id);
	 for (NumCty = 0;
	      NumCty < Gbl.Ctys.Num;
	      NumCty++)
	    fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
		     Gbl.Ctys.Lst[NumCty].CtyCod,
		     Gbl.Ctys.Lst[NumCty].CtyCod == Gbl.CurrentCty.Cty.CtyCod ? " selected=\"selected\"" :
										"",
		     Gbl.Ctys.Lst[NumCty].Name[Gbl.Prefs.Language]);
	 fprintf (Gbl.F.Out,"</select>");
	 Act_FormEnd ();

	 /* Free list of countries */
	 Cty_FreeListCountries ();
	}
      else	// I can not move institution to another country
	 fprintf (Gbl.F.Out,"%s",Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);

      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      /***** Institution full name *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"RIGHT_MIDDLE\">"
	                 "<label for=\"FullName\" class=\"%s\">%s:</label>"
	                 "</td>"
			 "<td class=\"DAT_N LEFT_MIDDLE\">",
	       The_ClassForm[Gbl.Prefs.Theme],
	       Txt_Institution);
      if (!PrintView &&
	  Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
	 // Only system admins can edit institution full name
	{
	 /* Form to change institution full name */
	 Act_FormStart (ActRenInsFulCfg);
	 fprintf (Gbl.F.Out,"<input type=\"text\""
	                    " id=\"FullName\" name=\"FullName\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_FULL_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Hie_MAX_CHARS_FULL_NAME,
		  Gbl.CurrentIns.Ins.FullName,
		  Gbl.Form.Id);
	 Act_FormEnd ();
	}
      else	// I can not edit institution full name
	 fprintf (Gbl.F.Out,"%s",Gbl.CurrentIns.Ins.FullName);
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      /***** Institution short name *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"RIGHT_MIDDLE\">"
	                 "<label for=\"ShortName\" class=\"%s\">%s:</label>"
	                 "</td>"
			 "<td class=\"DAT_N LEFT_MIDDLE\">",
	       The_ClassForm[Gbl.Prefs.Theme],
	       Txt_Short_name);
      if (!PrintView &&
	  Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
	 // Only system admins can edit institution short name
	{
	 /* Form to change institution short name */
	 Act_FormStart (ActRenInsShoCfg);
	 fprintf (Gbl.F.Out,"<input type=\"text\""
	                    " id=\"ShortName\" name=\"ShortName\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_SHORT_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Hie_MAX_CHARS_SHRT_NAME,
		  Gbl.CurrentIns.Ins.ShrtName,
		  Gbl.Form.Id);
	 Act_FormEnd ();
	}
      else	// I can not edit institution short name
	 fprintf (Gbl.F.Out,"%s",Gbl.CurrentIns.Ins.ShrtName);
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      /***** Institution WWW *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"RIGHT_MIDDLE\">"
	                 "<label for=\"WWW\" class=\"%s\">%s:</label>"
			 "</td>"
			 "<td class=\"DAT LEFT_MIDDLE\">",
	       The_ClassForm[Gbl.Prefs.Theme],
	       Txt_Web);
      if (!PrintView &&
	  Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM)
	 // Only institution admins and system admins
	 // can change institution WWW
	{
	 /* Form to change institution WWW */
	 Act_FormStart (ActChgInsWWWCfg);
	 fprintf (Gbl.F.Out,"<input type=\"url\" id=\"WWW\" name=\"WWW\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_WWW\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Cns_MAX_CHARS_WWW,
		  Gbl.CurrentIns.Ins.WWW,
		  Gbl.Form.Id);
	 Act_FormEnd ();
	}
      else	// I can not change institution WWW
	 fprintf (Gbl.F.Out,"<div class=\"EXTERNAL_WWW_LONG\">"
			    "<a href=\"%s\" target=\"_blank\" class=\"DAT\">"
	                    "%s"
			    "</a>"
			    "</div>",
		  Gbl.CurrentIns.Ins.WWW,
		  Gbl.CurrentIns.Ins.WWW);
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
	       The_ClassForm[Gbl.Prefs.Theme],
	       Txt_Shortcut,
	       Cfg_URL_SWAD_CGI,
	       Txt_STR_LANG_ID[Gbl.Prefs.Language],
	       Gbl.CurrentIns.Ins.InsCod,
	       Cfg_URL_SWAD_CGI,
	       Txt_STR_LANG_ID[Gbl.Prefs.Language],
	       Gbl.CurrentIns.Ins.InsCod);

      if (PrintView)
	{
	 /***** QR code with link to the institution *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"LEFT_MIDDLE\">",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_QR_code);
	 QR_LinkTo (250,"ins",Gbl.CurrentIns.Ins.InsCod);
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
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_Users_of_the_institution,
		  Usr_GetNumUsrsWhoClaimToBelongToIns (Gbl.CurrentIns.Ins.InsCod));

	 /***** Number of centres *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
	                    "%s:"
	                    "</td>"
			    "<td class=\"LEFT_MIDDLE\">",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_Centres);

	 /* Form to go to see centres of this institution */
	 Act_FormGoToStart (ActSeeCtr);
	 Ins_PutParamInsCod (Gbl.CurrentIns.Ins.InsCod);
	 sprintf (Gbl.Title,Txt_Centres_of_INSTITUTION_X,
	          Gbl.CurrentIns.Ins.ShrtName);
	 Act_LinkFormSubmit (Gbl.Title,"DAT",NULL);
	 fprintf (Gbl.F.Out,"%u</a>",
		  Ctr_GetNumCtrsInIns (Gbl.CurrentIns.Ins.InsCod));
	 Act_FormEnd ();

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
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_Degrees,
		  Deg_GetNumDegsInIns (Gbl.CurrentIns.Ins.InsCod));

	 /***** Number of courses *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_Courses,
		  Crs_GetNumCrssInIns (Gbl.CurrentIns.Ins.InsCod));

	 /***** Number of departments *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_Departments,
		  Dpt_GetNumDepartmentsInInstitution (Gbl.CurrentIns.Ins.InsCod));

	 /***** Number of teachers in courses of this institution *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_ROLES_PLURAL_Abc[Rol_TEACHER][Usr_SEX_UNKNOWN],
		  Usr_GetNumUsrsInCrssOfIns (Rol_TEACHER,Gbl.CurrentIns.Ins.InsCod));

	 /***** Number of students in courses of this institution *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_ROLES_PLURAL_Abc[Rol_STUDENT][Usr_SEX_UNKNOWN],
		  Usr_GetNumUsrsInCrssOfIns (Rol_STUDENT,Gbl.CurrentIns.Ins.InsCod));

	 /***** Number of users in courses of this institution *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s + %s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_ROLES_PLURAL_Abc[Rol_TEACHER][Usr_SEX_UNKNOWN],
		  Txt_ROLES_PLURAL_Abc[Rol_STUDENT][Usr_SEX_UNKNOWN],
		  Usr_GetNumUsrsInCrssOfIns (Rol_UNKNOWN,Gbl.CurrentIns.Ins.InsCod));
	}

      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>");

      /***** End frame *****/
      Lay_EndRoundFrame ();
     }
  }

/*****************************************************************************/
/********* Put contextual icons in configuration of an institution ***********/
/*****************************************************************************/

static void Ins_PutIconsToPrintAndUpload (void)
  {
   extern const char *Txt_Print;

   /***** Link to print info about institution *****/
   Lay_PutContextualLink (ActPrnInsInf,NULL,NULL,
                          "print64x64.png",
                          Txt_Print,NULL,
                          NULL);

   if (Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM)
      /***** Link to upload logo of institution *****/
      Log_PutIconToChangeLogo (Sco_SCOPE_INS);
  }

/*****************************************************************************/
/**************** List the institutions of the current country ***************/
/*****************************************************************************/

void Ins_ShowInssOfCurrentCty (void)
  {
   if (Gbl.CurrentCty.Cty.CtyCod > 0)
     {
      /***** Get parameter with the type of order in the list of institutions *****/
      Ins_GetParamInsOrder ();

      /***** Get list of institutions *****/
      Ins_GetListInstitutions (Gbl.CurrentCty.Cty.CtyCod,Ins_GET_EXTRA_DATA);

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

   /***** Start frame *****/
   sprintf (Gbl.Title,Txt_Institutions_of_COUNTRY_X,Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
   Lay_StartRoundFrame (NULL,Gbl.Title,
                        Ins_PutIconsListInstitutions,Hlp_COUNTRY_Institutions);

   if (Gbl.Inss.Num)	// There are institutions in the current country
     {
      /***** Start table *****/
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE_MARGIN CELLS_PAD_2\">");
      Ins_PutHeadInstitutionsForSeeing (true);	// Order selectable

      /***** Write all the institutions and their nuber of users *****/
      for (NumIns = 0;
	   NumIns < Gbl.Inss.Num;
	   NumIns++)
	 Ins_ListOneInstitutionForSeeing (&(Gbl.Inss.Lst[NumIns]),NumIns + 1);

      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>");
     }
   else	// No insrtitutions created in the current country
      Lay_ShowAlert (Lay_INFO,Txt_No_institutions);

   /***** Button to create institution *****/
   if (Ins_CheckIfICanCreateInstitutions ())
     {
      Act_FormStart (ActEdiIns);
      Lay_PutConfirmButton (Gbl.Inss.Num ? Txt_Create_another_institution :
	                                   Txt_Create_institution);
      Act_FormEnd ();
     }

   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/******************* Check if I can create institutions **********************/
/*****************************************************************************/

static bool Ins_CheckIfICanCreateInstitutions (void)
  {
   return (bool) (Gbl.Usrs.Me.LoggedRole >= Rol__GUEST_);
  }

/*****************************************************************************/
/*************** Put contextual icons in list of institutions ****************/
/*****************************************************************************/

static void Ins_PutIconsListInstitutions (void)
  {
   /***** Put icon to edit institutions *****/
   if (Ins_CheckIfICanCreateInstitutions ())
      Ins_PutIconToEditInstitutions ();

   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_INSTITS;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/******************** Put link (form) to edit institutions *******************/
/*****************************************************************************/

static void Ins_PutIconToEditInstitutions (void)
  {
   extern const char *Txt_Edit;

   Lay_PutContextualLink (ActEdiIns,NULL,NULL,
                          "edit64x64.png",
                          Txt_Edit,NULL,
                          NULL);
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
   BgColor = (Ins->InsCod == Gbl.CurrentIns.Ins.InsCod) ? "LIGHT_BLUE" :
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
	    TxtClassNormal,BgColor,Ins->NumCtrs);

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
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Students_ABBREVIATION;
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
	 Act_FormStart (ActSeeIns);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Act_LinkFormSubmit (Txt_INSTITUTIONS_HELP_ORDER[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Inss.SelectedOrder)
	    fprintf (Gbl.F.Out,"<u>");
	}
      fprintf (Gbl.F.Out,"%s",Txt_INSTITUTIONS_ORDER[Order]);
      if (OrderSelectable)
	{
	 if (Order == Gbl.Inss.SelectedOrder)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Act_FormEnd ();
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
            Txt_Teachers_ABBREVIATION,Txt_Students_ABBREVIATION);
   }

/*****************************************************************************/
/******* Get parameter with the type or order in list of institutions ********/
/*****************************************************************************/

static void Ins_GetParamInsOrder (void)
  {
   Gbl.Inss.SelectedOrder = (Ins_Order_t)
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
   Ins_GetListInstitutions (Gbl.CurrentCty.Cty.CtyCod,Ins_GET_EXTRA_DATA);

   /***** Start frame *****/
   sprintf (Gbl.Title,Txt_Institutions_of_COUNTRY_X,
            Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
   Lay_StartRoundFrame (NULL,Gbl.Title,Ins_PutIconToViewInstitutions,
                        Hlp_COUNTRY_Institutions);

   /***** Put a form to create a new institution *****/
   Ins_PutFormToCreateInstitution ();

   /***** Forms to edit current institutions *****/
   if (Gbl.Inss.Num)
      Ins_ListInstitutionsForEdition ();

   /***** End frame *****/
   Lay_EndRoundFrame ();

   /***** Free list of institutions *****/
   Ins_FreeListInstitutions ();
  }

/*****************************************************************************/
/***************** Put contextual icon to view institutions ******************/
/*****************************************************************************/

static void Ins_PutIconToViewInstitutions (void)
  {
   extern const char *Txt_View;

   /***** Put form to create a new type of group *****/
   Lay_PutContextualLink (ActSeeIns,NULL,NULL,
			  "eye-on64x64.png",
			  Txt_View,NULL,
                          NULL);
  }

/*****************************************************************************/
/********************** Get list of current institutions *********************/
/*****************************************************************************/
// If CtyCod <= 0, get all institutions

void Ins_GetListInstitutions (long CtyCod,Ins_GetExtraData_t GetExtraData)
  {
   char OrderBySubQuery[256];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumIns;
   struct Instit *Ins;

   /***** Get institutions from database *****/
   switch (GetExtraData)
     {
      case Ins_GET_BASIC_DATA:
	 sprintf (Query,"SELECT InsCod,CtyCod,Status,RequesterUsrCod,ShortName,FullName,WWW"
			" FROM institutions"
			" WHERE CtyCod=%ld"
			" ORDER BY FullName",
		  CtyCod);
         break;
      case Ins_GET_EXTRA_DATA:
         switch (Gbl.Inss.SelectedOrder)
           {
            case Ins_ORDER_BY_INSTITUTION:
               sprintf (OrderBySubQuery,"FullName");
               break;
            case Ins_ORDER_BY_NUM_USRS:
               sprintf (OrderBySubQuery,"NumUsrs DESC,FullName");
               break;
           }
	 sprintf (Query,"(SELECT institutions.InsCod,institutions.CtyCod,"
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
	       OrderBySubQuery);
         break;
     }
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get institutions");

   if (NumRows) // Institutions found...
     {
      // NumRows should be equal to Deg->NumCourses
      Gbl.Inss.Num = (unsigned) NumRows;

      /***** Create list with institutions *****/
      if ((Gbl.Inss.Lst = (struct Instit *) calloc (NumRows,sizeof (struct Instit))) == NULL)
          Lay_ShowErrorAndExit ("Not enough memory to store institutions.");

      /***** Get the institutions *****/
      for (NumIns = 0;
	   NumIns < Gbl.Inss.Num;
	   NumIns++)
        {
         Ins = &(Gbl.Inss.Lst[NumIns]);

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
               Ins->NumCtrs = Ins->NumDegs = Ins->NumCrss = Ins->NumDpts = 0;
               Ins->NumUsrs = 0;
               break;
            case Ins_GET_EXTRA_DATA:
               /* Get number of users who claim to belong to this institution (row[7]) */
               if (sscanf (row[7],"%u",&Ins->NumUsrsWhoClaimToBelongToIns) != 1)
        	  Ins->NumUsrsWhoClaimToBelongToIns = 0;

               /* Get number of centres in this institution */
               Ins->NumCtrs = Ctr_GetNumCtrsInIns (Ins->InsCod);

               /* Get number of degrees in this institution */
               Ins->NumDegs = Deg_GetNumDegsInIns (Ins->InsCod);

               /* Get number of degrees in this institution */
               Ins->NumCrss = Crs_GetNumCrssInIns (Ins->InsCod);

               /* Get number of departments in this institution */
               Ins->NumDpts = Dpt_GetNumberOfDepartmentsInInstitution (Ins->InsCod);

               /* Get number of users in courses */
	       Ins->NumUsrs = Usr_GetNumUsrsInCrssOfIns (Rol_UNKNOWN,Ins->InsCod);	// Here Rol_UNKNOWN means "all users"
               break;
           }
        }
     }
   else
      Gbl.Inss.Num = 0;

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

   /***** Get institution full name *****/
   Ins.InsCod = InsCod;
   Ins_GetFullNameAndCtyOfInstitutionByCod (&Ins,CtyName);

   /***** Write institution full name *****/
   fprintf (Gbl.F.Out,"%s<br />%s",Ins.FullName,CtyName);
  }

/*****************************************************************************/
/************************* Get data of an institution ************************/
/*****************************************************************************/

bool Ins_GetDataOfInstitutionByCod (struct Instit *Ins,
                                    Ins_GetExtraData_t GetExtraData)
  {
   char Query[256];
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
   Ins->NumCtrs = Ins->NumDegs = Ins->NumCrss = Ins->NumDpts = 0;
   Ins->NumUsrs = 0;

   /***** Check if institution code is correct *****/
   if (Ins->InsCod > 0)
     {
      /***** Get data of an institution from database *****/
      sprintf (Query,"SELECT CtyCod,Status,RequesterUsrCod,ShortName,FullName,WWW"
		     " FROM institutions WHERE InsCod=%ld",
	       Ins->InsCod);

      /***** Count number of rows in result *****/
      if (DB_QuerySELECT (Query,&mysql_res,"can not get data of an institution")) // Institution found...
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
	    Ins->NumCtrs = Ctr_GetNumCtrsInIns (Ins->InsCod);

	    /* Get number of departments in this institution */
	    Ins->NumDpts = Dpt_GetNumberOfDepartmentsInInstitution (Ins->InsCod);

	    /* Get number of degrees in this institution */
	    Ins->NumDegs = Deg_GetNumDegsInIns (Ins->InsCod);

	    /* Get number of users in courses of this institution */
	    Ins->NumUsrs = Usr_GetNumUsrsInCrssOfIns (Rol_UNKNOWN,Ins->InsCod);	// Here Rol_UNKNOWN means "all users"
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

void Ins_GetShortNameOfInstitutionByCod (struct Instit *Ins)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   static struct
     {
      long InsCod;
      char ShrtName[Hie_MAX_BYTES_SHRT_NAME + 1];
     } Cached =
     {
      -1L,
      {'\0'}
     };

   if (Ins->InsCod <= 0)
      Ins->ShrtName[0] = '\0';	// Empty name
   else
     {
      if (Ins->InsCod != Cached.InsCod)	// If not cached...
	{
	 /***** Get the short name of an institution from database *****/
	 sprintf (Query,"SELECT ShortName FROM institutions WHERE InsCod=%ld",
		  Ins->InsCod);
	 if (DB_QuerySELECT (Query,&mysql_res,"can not get the short name of an institution") == 1)
	   {
	    /***** Get the short name of this institution *****/
	    row = mysql_fetch_row (mysql_res);

	    Str_Copy (Cached.ShrtName,row[0],
		      Hie_MAX_BYTES_SHRT_NAME);
	   }
	 else
	    Cached.ShrtName[0] = '\0';

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);
	}

      Str_Copy (Ins->ShrtName,Cached.ShrtName,
		Hie_MAX_BYTES_SHRT_NAME);
     }
  }

/*****************************************************************************/
/************ Get the full name of an institution from its code **************/
/*****************************************************************************/

static void Ins_GetFullNameAndCtyOfInstitutionByCod (struct Instit *Ins,
                                               char CtyName[Hie_MAX_BYTES_FULL_NAME + 1])
  {
   extern const char *Txt_STR_LANG_ID[1 + Txt_NUM_LANGUAGES];
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   static struct
     {
      long InsCod;
      char FullName[Hie_MAX_BYTES_FULL_NAME + 1];
      char CtyName[Hie_MAX_BYTES_FULL_NAME + 1];
     } Cached =
     {
      -1L,
      {'\0'},
      {'\0'},
     };

   if (Ins->InsCod <= 0)
     {
      Ins->FullName[0] = '\0';	// Empty name
      CtyName[0] = '\0';	// Empty name
     }
   else
     {
      if (Ins->InsCod != Cached.InsCod)	// If not cached...
	{
	 /***** Get the short name of an institution from database *****/
	 sprintf (Query,"SELECT institutions.FullName,countries.Name_%s"
	                " FROM institutions,countries"
	                " WHERE institutions.InsCod=%ld"
	                " AND institutions.CtyCod=countries.CtyCod",
		  Txt_STR_LANG_ID[Gbl.Prefs.Language],Ins->InsCod);
	 if (DB_QuerySELECT (Query,&mysql_res,"can not get the full name of an institution") == 1)
	   {
	    /* Get row */
	    row = mysql_fetch_row (mysql_res);

	    /* Get the full name of this institution (row[0]) */
	    Str_Copy (Cached.FullName,row[0],
		      Hie_MAX_BYTES_FULL_NAME);

	    /* Get the name of the country (row[1]) */
	    Str_Copy (Cached.CtyName,row[1],
		      Hie_MAX_BYTES_FULL_NAME);
	   }
	 else
	   {
	    Cached.FullName[0] = '\0';
	    Cached.CtyName[0] = '\0';
	   }

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);
	}

      Str_Copy (Ins->FullName,Cached.FullName,
		Hie_MAX_BYTES_FULL_NAME);
      Str_Copy (CtyName,Cached.CtyName,
		Hie_MAX_BYTES_FULL_NAME);
     }
  }

/*****************************************************************************/
/************************* Free list of institutions *************************/
/*****************************************************************************/

void Ins_FreeListInstitutions (void)
  {
   if (Gbl.Inss.Lst)
     {
      /***** Free memory used by the list of institutions *****/
      free ((void *) Gbl.Inss.Lst);
      Gbl.Inss.Lst = NULL;
      Gbl.Inss.Num = 0;
     }
  }

/*****************************************************************************/
/************************ Write selector of institution **********************/
/*****************************************************************************/

void Ins_WriteSelectorOfInstitution (void)
  {
   extern const char *Txt_Institution;
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumInss;
   unsigned NumIns;
   long InsCod;

   /***** Start form *****/
   Act_FormGoToStart (ActSeeCtr);
   fprintf (Gbl.F.Out,"<select id=\"ins\" name=\"ins\" style=\"width:175px;\"");
   if (Gbl.CurrentCty.Cty.CtyCod > 0)
      fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
               Gbl.Form.Id);
   else
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,"><option value=\"\"");
   if (Gbl.CurrentIns.Ins.InsCod < 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out," disabled=\"disabled\">[%s]</option>",
            Txt_Institution);

   if (Gbl.CurrentCty.Cty.CtyCod > 0)
     {
      /***** Get institutions of selected country from database *****/
      sprintf (Query,"SELECT DISTINCT InsCod,ShortName FROM institutions"
                     " WHERE CtyCod=%ld"
                     " ORDER BY ShortName",
               Gbl.CurrentCty.Cty.CtyCod);
      NumInss = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get institutions");

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
         if (Gbl.CurrentIns.Ins.InsCod > 0 && InsCod == Gbl.CurrentIns.Ins.InsCod)
            fprintf (Gbl.F.Out," selected=\"selected\"");
         fprintf (Gbl.F.Out,">%s</option>",row[1]);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** End form *****/
   fprintf (Gbl.F.Out,"</select>");
   Act_FormEnd ();
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
   fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_WIDE CELLS_PAD_2\">");
   Ins_PutHeadInstitutionsForEdition ();

   /***** Write all the institutions *****/
   for (NumIns = 0;
	NumIns < Gbl.Inss.Num;
	NumIns++)
     {
      Ins = &Gbl.Inss.Lst[NumIns];

      ICanEdit = Ins_CheckIfICanEdit (Ins);

      /* Put icon to remove institution */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"BM\">");
      if (Ins->NumCtrs ||
	  Ins->NumUsrsWhoClaimToBelongToIns ||
	  Ins->NumUsrs ||	// Institution has centres or users ==> deletion forbidden
          !ICanEdit)
         Lay_PutIconRemovalNotAllowed ();
      else
        {
         Act_FormStart (ActRemIns);
         Ins_PutParamOtherInsCod (Ins->InsCod);
         Lay_PutIconRemove ();
         Act_FormEnd ();
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
      Log_DrawLogo (Sco_SCOPE_INS,Ins->InsCod,Ins->ShrtName,20,NULL,true);
      fprintf (Gbl.F.Out,"</td>");

      /* Institution short name */
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (ICanEdit)
	{
	 Act_FormStart (ActRenInsSho);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_SHORT_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Hie_MAX_CHARS_SHRT_NAME,Ins->ShrtName,
		  Gbl.Form.Id);
	 Act_FormEnd ();
	}
      else
	 fprintf (Gbl.F.Out,"%s",Ins->ShrtName);
      fprintf (Gbl.F.Out,"</td>");

      /* Institution full name */
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (ICanEdit)
	{
	 Act_FormStart (ActRenInsFul);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_FULL_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Hie_MAX_CHARS_FULL_NAME,
		  Ins->FullName,
		  Gbl.Form.Id);
	 Act_FormEnd ();
	}
      else
	 fprintf (Gbl.F.Out,"%s",Ins->FullName);
      fprintf (Gbl.F.Out,"</td>");

      /* Institution WWW */
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (ICanEdit)
	{
	 Act_FormStart (ActChgInsWWW);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 fprintf (Gbl.F.Out,"<input type=\"url\" name=\"WWW\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_WWW\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Cns_MAX_CHARS_WWW,
		  Ins->WWW,
		  Gbl.Form.Id);
	 Act_FormEnd ();
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
               Ins->NumCtrs);

      /* Number of users in courses of this institution */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%u"
	                 "</td>",
               Ins->NumUsrs);

      /* Institution requester */
      UsrDat.UsrCod = Ins->RequesterUsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);
      fprintf (Gbl.F.Out,"<td class=\"INPUT_REQUESTER LEFT_TOP\">"
			 "<table class=\"INPUT_REQUESTER CELLS_PAD_2\">"
			 "<tr>");
      Msg_WriteMsgAuthor (&UsrDat,"DAT",true,NULL);
      fprintf (Gbl.F.Out,"</tr>"
			 "</table>"
			 "</td>");

      /* Institution status */
      StatusTxt = Ins_GetStatusTxtFromStatusBits (Ins->Status);
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM &&
	  StatusTxt == Ins_STATUS_PENDING)
	{
	 Act_FormStart (ActChgInsSta);
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
	 Act_FormEnd ();
	}
      else if (StatusTxt != Ins_STATUS_ACTIVE)	// If active ==> do not show anything
	 fprintf (Gbl.F.Out,"%s",Txt_INSTITUTION_STATUS[StatusTxt]);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   /***** End table *****/
   fprintf (Gbl.F.Out,"</table>");

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************ Check if I can edit, remove, etc. an institution ***************/
/*****************************************************************************/

static bool Ins_CheckIfICanEdit (struct Instit *Ins)
  {
   return (bool) (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ||		// I am a superuser
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

long Ins_GetAndCheckParamOtherInsCod (void)
  {
   long InsCod;

   /***** Get and check parameter with code of institution *****/
   if ((InsCod = Ins_GetParamOtherInsCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of institution is missing.");

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
   char Query[128];
   struct Instit Ins;
   char PathIns[PATH_MAX + 1];

   /***** Get institution code *****/
   Ins.InsCod = Ins_GetAndCheckParamOtherInsCod ();

   /***** Get data of the institution from database *****/
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_EXTRA_DATA);

   /***** Check if this institution has users *****/
   if (!Ins_CheckIfICanEdit (&Ins))
      Lay_ShowErrorAndExit ("You don't have permission to remove institution.");
   else if (Ins.NumCtrs ||
            Ins.NumUsrsWhoClaimToBelongToIns ||
            Ins.NumUsrs)	// Institution has centres or users ==> don't remove
      Lay_ShowAlert (Lay_WARNING,Txt_To_remove_an_institution_you_must_first_remove_all_centres_and_users_in_the_institution);
   else	// Institution has no users ==> remove it
     {
      /***** Remove all the threads and posts in forums of the institution *****/
      For_RemoveForums (Sco_SCOPE_INS,Ins.InsCod);

      /***** Remove surveys of the institution *****/
      Svy_RemoveSurveys (Sco_SCOPE_INS,Ins.InsCod);

      /***** Remove information related to files in institution *****/
      Brw_RemoveInsFilesFromDB (Ins.InsCod);

      /***** Remove directories of the institution *****/
      sprintf (PathIns,"%s/%s/%02u/%u",
	       Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_INS,
	       (unsigned) (Ins.InsCod % 100),
	       (unsigned) Ins.InsCod);
      Fil_RemoveTree (PathIns);

      /***** Remove institution *****/
      sprintf (Query,"DELETE FROM institutions WHERE InsCod=%ld",
               Ins.InsCod);
      DB_QueryDELETE (Query,"can not remove an institution");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Message,Txt_Institution_X_removed,
               Ins.FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Show the form again *****/
   Ins_EditInstitutions ();
  }

/*****************************************************************************/
/***************** Change the short name of an institution *******************/
/*****************************************************************************/

void Ins_RenameInsShort (void)
  {
   Gbl.Inss.EditingIns.InsCod = Ins_GetAndCheckParamOtherInsCod ();
   Ins_RenameInstitution (&Gbl.Inss.EditingIns,Cns_SHRT_NAME);
  }

void Ins_RenameInsShortInConfig (void)
  {
   Ins_RenameInstitution (&Gbl.CurrentIns.Ins,Cns_SHRT_NAME);
  }

/*****************************************************************************/
/***************** Change the full name of an institution ********************/
/*****************************************************************************/

void Ins_RenameInsFull (void)
  {
   Gbl.Inss.EditingIns.InsCod = Ins_GetAndCheckParamOtherInsCod ();
   Ins_RenameInstitution (&Gbl.Inss.EditingIns,Cns_FULL_NAME);
  }

void Ins_RenameInsFullInConfig (void)
  {
   Ins_RenameInstitution (&Gbl.CurrentIns.Ins,Cns_FULL_NAME);
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
     {
      Gbl.AlertType = Lay_WARNING;
      sprintf (Gbl.Message,Txt_You_can_not_leave_the_name_of_the_institution_X_empty,
               CurrentInsName);
     }
   else
     {
      /***** Check if old and new names are the same (this happens when user press enter with no changes in the form) *****/
      if (strcmp (CurrentInsName,NewInsName))	// Different names
        {
         /***** If institution was in database... *****/
         if (Ins_CheckIfInsNameExistsInCty (ParamName,NewInsName,Ins->InsCod,Gbl.CurrentCty.Cty.CtyCod))
           {
            Gbl.AlertType = Lay_WARNING;
            sprintf (Gbl.Message,Txt_The_institution_X_already_exists,
                     NewInsName);
           }
         else
           {
            /* Update the table changing old name by new name */
            Ins_UpdateInsNameDB (Ins->InsCod,FieldName,NewInsName);

            /* Create message to show the change made */
            Gbl.AlertType = Lay_SUCCESS;
            sprintf (Gbl.Message,Txt_The_institution_X_has_been_renamed_as_Y,
                     CurrentInsName,NewInsName);

	    /* Change current institution name in order to display it properly */
	    Str_Copy (CurrentInsName,NewInsName,
	              MaxBytes);
           }
        }
      else	// The same name
	{
         Gbl.AlertType = Lay_INFO;
         sprintf (Gbl.Message,Txt_The_name_of_the_institution_X_has_not_changed,
                  CurrentInsName);
	}
     }
  }

/*****************************************************************************/
/****** Check if the name of institution exists in the current country *******/
/*****************************************************************************/

static bool Ins_CheckIfInsNameExistsInCty (const char *FieldName,const char *Name,long InsCod,long CtyCod)
  {
   char Query[256 + Hie_MAX_BYTES_FULL_NAME];

   /***** Get number of institutions in current country with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM institutions"
                  " WHERE CtyCod=%ld AND %s='%s' AND InsCod<>%ld",
            CtyCod,FieldName,Name,InsCod);
   return (DB_QueryCOUNT (Query,"can not check if the name of an institution already existed") != 0);
  }

/*****************************************************************************/
/************ Update institution name in table of institutions ***************/
/*****************************************************************************/

static void Ins_UpdateInsNameDB (long InsCod,const char *FieldName,const char *NewInsName)
  {
   char Query[128 + Hie_MAX_BYTES_FULL_NAME];

   /***** Update institution changing old name by new name */
   sprintf (Query,"UPDATE institutions SET %s='%s' WHERE InsCod=%ld",
	    FieldName,NewInsName,InsCod);
   DB_QueryUPDATE (Query,"can not update the name of an institution");
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
   NewCty.CtyCod = Cty_GetAndCheckParamOtherCtyCod ();

   /***** Check if country has changed *****/
   if (NewCty.CtyCod != Gbl.CurrentIns.Ins.CtyCod)
     {
      /***** Get data of the country from database *****/
      Cty_GetDataOfCountryByCod (&NewCty,Cty_GET_BASIC_DATA);

      /***** Check if it already exists an institution with the same name in the new country *****/
      if (Ins_CheckIfInsNameExistsInCty ("ShortName",Gbl.CurrentIns.Ins.ShrtName,-1L,NewCty.CtyCod))
	{
         Gbl.AlertType = Lay_WARNING;
	 sprintf (Gbl.Message,Txt_The_institution_X_already_exists,
		  Gbl.CurrentIns.Ins.ShrtName);
	}
      else if (Ins_CheckIfInsNameExistsInCty ("FullName",Gbl.CurrentIns.Ins.FullName,-1L,NewCty.CtyCod))
	{
         Gbl.AlertType = Lay_WARNING;
	 sprintf (Gbl.Message,Txt_The_institution_X_already_exists,
		  Gbl.CurrentIns.Ins.FullName);
	}
      else
	{
	 /***** Update the table changing the country of the institution *****/
	 Ins_UpdateInsCtyDB (Gbl.CurrentIns.Ins.InsCod,NewCty.CtyCod);
         Gbl.CurrentIns.Ins.CtyCod =
         Gbl.CurrentCty.Cty.CtyCod = NewCty.CtyCod;

	 /***** Initialize again current course, degree, centre... *****/
	 Hie_InitHierarchy ();

	 /***** Write message to show the change made *****/
         Gbl.AlertType = Lay_SUCCESS;
	 sprintf (Gbl.Message,Txt_The_country_of_the_institution_X_has_changed_to_Y,
		  Gbl.CurrentIns.Ins.FullName,NewCty.Name[Gbl.Prefs.Language]);
	}
     }
  }

/*****************************************************************************/
/*** Show msg. of success after changing an institution in instit. config. ***/
/*****************************************************************************/

void Ins_ContEditAfterChgInsInConfig (void)
  {
   /***** Write success / warning message *****/
   Lay_ShowAlert (Gbl.AlertType,Gbl.Message);

   /***** Show the form again *****/
   Ins_ShowConfiguration ();
  }

/*****************************************************************************/
/****************** Update country in table of institutions ******************/
/*****************************************************************************/

static void Ins_UpdateInsCtyDB (long InsCod,long CtyCod)
  {
   char Query[128];

   /***** Update country in table of institutions *****/
   sprintf (Query,"UPDATE institutions SET CtyCod=%ld WHERE InsCod=%ld",
            CtyCod,InsCod);
   DB_QueryUPDATE (Query,"can not update the country of an institution");
  }

/*****************************************************************************/
/********************** Change the URL of a institution **********************/
/*****************************************************************************/

void Ins_ChangeInsWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   extern const char *Txt_You_can_not_leave_the_web_address_empty;
   struct Instit *Ins;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   Ins = &Gbl.Inss.EditingIns;

   /***** Get parameters from form *****/
   /* Get the code of the institution */
   Ins->InsCod = Ins_GetAndCheckParamOtherInsCod ();

   /* Get the new WWW for the institution */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get data of institution *****/
   Ins_GetDataOfInstitutionByCod (Ins,Ins_GET_BASIC_DATA);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ins_UpdateInsWWWDB (Ins->InsCod,NewWWW);
      Str_Copy (Ins->WWW,NewWWW,
                Cns_MAX_BYTES_WWW);

      /***** Write message to show the change made *****/
      sprintf (Gbl.Message,Txt_The_new_web_address_is_X,NewWWW);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

      /***** Put button to go to institution changed *****/
      Ins_PutButtonToGoToIns (Ins);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_leave_the_web_address_empty);

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
      Ins_UpdateInsWWWDB (Gbl.CurrentIns.Ins.InsCod,NewWWW);
      Str_Copy (Gbl.CurrentIns.Ins.WWW,NewWWW,
                Cns_MAX_BYTES_WWW);

      /***** Write message to show the change made *****/
      sprintf (Gbl.Message,Txt_The_new_web_address_is_X,NewWWW);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_leave_the_web_address_empty);

   /***** Show the form again *****/
   Ins_ShowConfiguration ();
  }

/*****************************************************************************/
/**************** Update database changing old WWW by new WWW ****************/
/*****************************************************************************/

static void Ins_UpdateInsWWWDB (long InsCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1])
  {
   char Query[128 + Cns_MAX_BYTES_WWW];

   /***** Update database changing old WWW by new WWW *****/
   sprintf (Query,"UPDATE institutions SET WWW='%s' WHERE InsCod=%ld",
	    NewWWW,InsCod);
   DB_QueryUPDATE (Query,"can not update the web of an institution");
  }

/*****************************************************************************/
/******************** Change the status of an institution ********************/
/*****************************************************************************/

void Ins_ChangeInsStatus (void)
  {
   extern const char *Txt_The_status_of_the_institution_X_has_changed;
   struct Instit *Ins;
   char Query[128];
   Ins_Status_t Status;
   Ins_StatusTxt_t StatusTxt;

   Ins = &Gbl.Inss.EditingIns;

   /***** Get parameters from form *****/
   /* Get institution code */
   Ins->InsCod = Ins_GetAndCheckParamOtherInsCod ();

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
   Ins_GetDataOfInstitutionByCod (Ins,Ins_GET_BASIC_DATA);

   /***** Update status in table of institutions *****/
   sprintf (Query,"UPDATE institutions SET Status=%u WHERE InsCod=%ld",
            (unsigned) Status,Ins->InsCod);
   DB_QueryUPDATE (Query,"can not update the status of an institution");

   Ins->Status = Status;

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_The_status_of_the_institution_X_has_changed,
            Ins->ShrtName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Put button to go to institution changed *****/
   Ins_PutButtonToGoToIns (Ins);

   /***** Show the form again *****/
   Ins_EditInstitutions ();
  }

/*****************************************************************************/
/*********** Show message of success after changing an institution ***********/
/*****************************************************************************/

void Ins_ContEditAfterChgIns (void)
  {
   /***** Write success / warning message *****/
   Lay_ShowAlert (Gbl.AlertType,Gbl.Message);

   if (Gbl.AlertType == Lay_SUCCESS)
      /***** Put button to go to institution changed *****/
      Ins_PutButtonToGoToIns (&Gbl.Inss.EditingIns);

   /***** Show the form again *****/
   Ins_EditInstitutions ();
  }

/*****************************************************************************/
/********************* Put button to go to institution ***********************/
/*****************************************************************************/

static void Ins_PutButtonToGoToIns (struct Instit *Ins)
  {
   extern const char *Txt_Go_to_X;

   // If the institution is different to the current one...
   if (Ins->InsCod != Gbl.CurrentIns.Ins.InsCod)
     {
      fprintf (Gbl.F.Out,"<div class=\"BUTTONS_AFTER_ALERT\">");
      Act_FormStart (ActSeeCtr);
      Ins_PutParamInsCod (Ins->InsCod);
      sprintf (Gbl.Title,Txt_Go_to_X,Ins->ShrtName);
      Lay_PutConfirmButtonInline (Gbl.Title);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");
     }
  }

/*****************************************************************************/
/******** Show a form for sending a logo of the current institution **********/
/*****************************************************************************/

void Ins_RequestLogo (void)
  {
   Log_RequestLogo (Sco_SCOPE_INS);
  }

/*****************************************************************************/
/************** Receive the logo of the current institution ******************/
/*****************************************************************************/

void Ins_ReceiveLogo (void)
  {
   Log_ReceiveLogo (Sco_SCOPE_INS);
  }

/*****************************************************************************/
/*************** Remove the logo of the current institution ******************/
/*****************************************************************************/

void Ins_RemoveLogo (void)
  {
   Log_RemoveLogo (Sco_SCOPE_INS);
  }

/*****************************************************************************/
/****************** Put a form to create a new institution *******************/
/*****************************************************************************/

static void Ins_PutFormToCreateInstitution (void)
  {
   extern const char *Txt_New_institution;
   extern const char *Txt_Create_institution;
   struct Instit *Ins;

   Ins = &Gbl.Inss.EditingIns;

   /***** Start form *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
      Act_FormStart (ActNewIns);
   else if (Gbl.Usrs.Me.MaxRole >= Rol__GUEST_)
      Act_FormStart (ActReqIns);
   else
      Lay_ShowErrorAndExit ("You can not edit institutions.");

   /***** Start of frame *****/
   Lay_StartRoundFrameTable (NULL,Txt_New_institution,NULL,NULL,2);

   /***** Write heading *****/
   Ins_PutHeadInstitutionsForEdition ();

   /***** Column to remove institution, disabled here *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"BM\"></td>");

   /***** Institution code *****/
   fprintf (Gbl.F.Out,"<td class=\"CODE\"></td>");

   /***** Institution logo *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\" style=\"width:25px;\">");
   Log_DrawLogo (Sco_SCOPE_INS,-1L,"",20,NULL,true);
   fprintf (Gbl.F.Out,"</td>");

   /***** Institution short name *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" name=\"ShortName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_SHORT_NAME\""
                      " required=\"required\" />"
                      "</td>",
            Hie_MAX_CHARS_SHRT_NAME,Ins->ShrtName);

   /***** Institution full name *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" name=\"FullName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_FULL_NAME\""
                      " required=\"required\" />"
                      "</td>",
            Hie_MAX_CHARS_FULL_NAME,Ins->FullName);

   /***** Institution WWW *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"url\" name=\"WWW\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_WWW\""
                      " required=\"required\" />"
                      "</td>",
            Cns_MAX_CHARS_WWW,Ins->WWW);

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
   fprintf (Gbl.F.Out,"<td class=\"INPUT_REQUESTER LEFT_TOP\">"
		      "<table class=\"INPUT_REQUESTER CELLS_PAD_2\">"
		      "<tr>");
   Msg_WriteMsgAuthor (&Gbl.Usrs.Me.UsrDat,"DAT",true,NULL);
   fprintf (Gbl.F.Out,"</tr>"
		      "</table>"
		      "</td>");

   /***** Institution status *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">"
	              "</td>"
		      "</tr>");

   /***** Send button and end of frame *****/
   Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_institution);

   /***** End of form *****/
   Act_FormEnd ();
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
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Students_ABBREVIATION;
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
            Txt_Teachers_ABBREVIATION,Txt_Students_ABBREVIATION,
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
   struct Instit *Ins;

   Ins = &Gbl.Inss.EditingIns;

   /***** Get parameters from form *****/
   /* Set institution country */
   Ins->CtyCod = Gbl.CurrentCty.Cty.CtyCod;

   /* Get institution short name */
   Par_GetParToText ("ShortName",Ins->ShrtName,Hie_MAX_BYTES_SHRT_NAME);

   /* Get institution full name */
   Par_GetParToText ("FullName",Ins->FullName,Hie_MAX_BYTES_FULL_NAME);

   /* Get institution WWW */
   Par_GetParToText ("WWW",Ins->WWW,Cns_MAX_BYTES_WWW);

   if (Ins->ShrtName[0] && Ins->FullName[0])	// If there's a institution name
     {
      if (Ins->WWW[0])
        {
         /***** If name of institution was in database... *****/
         if (Ins_CheckIfInsNameExistsInCty ("ShortName",Ins->ShrtName,-1L,Gbl.CurrentCty.Cty.CtyCod))
           {
            sprintf (Gbl.Message,Txt_The_institution_X_already_exists,
                     Ins->ShrtName);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else if (Ins_CheckIfInsNameExistsInCty ("FullName",Ins->FullName,-1L,Gbl.CurrentCty.Cty.CtyCod))
           {
            sprintf (Gbl.Message,Txt_The_institution_X_already_exists,
                     Ins->FullName);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else	// Add new institution to database
            Ins_CreateInstitution (Ins,Status);
        }
      else	// If there is not a web
         Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_web_address_of_the_new_institution);
     }
   else	// If there is not a institution name
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_institution);

   /***** Show the form again *****/
   Ins_EditInstitutions ();
  }

/*****************************************************************************/
/************************** Create a new institution *************************/
/*****************************************************************************/

static void Ins_CreateInstitution (struct Instit *Ins,unsigned Status)
  {
   extern const char *Txt_Created_new_institution_X;
   char Query[512 +
              Hie_MAX_BYTES_SHRT_NAME +
              Hie_MAX_BYTES_FULL_NAME +
              Cns_MAX_BYTES_WWW];

   /***** Create a new institution *****/
   sprintf (Query,"INSERT INTO institutions"
	          " (CtyCod,Status,RequesterUsrCod,ShortName,FullName,WWW)"
                  " VALUES"
                  " (%ld,%u,%ld,'%s','%s','%s')",
            Ins->CtyCod,
            Status,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Ins->ShrtName,Ins->FullName,Ins->WWW);
   Ins->InsCod = DB_QueryINSERTandReturnCode (Query,"can not create institution");

   /***** Write success message *****/
   sprintf (Gbl.Message,Txt_Created_new_institution_X,
            Ins->FullName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Put button to go to institution created *****/
   Ins_PutButtonToGoToIns (Ins);
  }

/*****************************************************************************/
/********************* Get total number of institutions **********************/
/*****************************************************************************/

unsigned Ins_GetNumInssTotal (void)
  {
   char Query[128];

   /***** Get total number of degrees from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM institutions");
   return (unsigned) DB_QueryCOUNT (Query,"can not get the total number of institutions");
  }

/*****************************************************************************/
/**************** Get number of institutions in a country ********************/
/*****************************************************************************/

unsigned Ins_GetNumInssInCty (long CtyCod)
  {
   char Query[128];

   /***** Get number of degrees of a place from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM institutions WHERE CtyCod=%ld",
            CtyCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of institutions in a country");
  }

/*****************************************************************************/
/***************** Get number of institutions with centres *******************/
/*****************************************************************************/

unsigned Ins_GetNumInssWithCtrs (const char *SubQuery)
  {
   char Query[512];

   /***** Get number of institutions with centres from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT institutions.InsCod)"
                  " FROM institutions,centres"
                  " WHERE %sinstitutions.InsCod=centres.InsCod",
            SubQuery);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of institutions with centres");
  }

/*****************************************************************************/
/****************** Get number of institutions with degrees ******************/
/*****************************************************************************/

unsigned Ins_GetNumInssWithDegs (const char *SubQuery)
  {
   char Query[512];

   /***** Get number of institutions with degrees from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT institutions.InsCod)"
                  " FROM institutions,centres,degrees"
                  " WHERE %sinstitutions.InsCod=centres.InsCod"
                  " AND centres.CtrCod=degrees.CtrCod",
            SubQuery);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of institutions with degrees");
  }

/*****************************************************************************/
/****************** Get number of institutions with courses ******************/
/*****************************************************************************/

unsigned Ins_GetNumInssWithCrss (const char *SubQuery)
  {
   char Query[512];

   /***** Get number of institutions with courses from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT institutions.InsCod)"
                  " FROM institutions,centres,degrees,courses"
                  " WHERE %sinstitutions.InsCod=centres.InsCod"
                  " AND centres.CtrCod=degrees.CtrCod"
                  " AND degrees.DegCod=courses.DegCod",
            SubQuery);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of institutions with courses");
  }

/*****************************************************************************/
/****************** Get number of institutions with users ********************/
/*****************************************************************************/

unsigned Ins_GetNumInssWithUsrs (Rol_Role_t Role,const char *SubQuery)
  {
   char Query[1024];

   /***** Get number of institutions with users from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT institutions.InsCod)"
                  " FROM institutions,centres,degrees,courses,crs_usr"
                  " WHERE %sinstitutions.InsCod=centres.InsCod"
                  " AND centres.CtrCod=degrees.CtrCod"
                  " AND degrees.DegCod=courses.DegCod"
                  " AND courses.CrsCod=crs_usr.CrsCod"
                  " AND crs_usr.Role=%u",
            SubQuery,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of institutions with users");
  }

/*****************************************************************************/
/*************************** List institutions found *************************/
/*****************************************************************************/
// Returns number of institutions found

unsigned Ins_ListInssFound (const char *Query)
  {
   extern const char *Txt_institution;
   extern const char *Txt_institutions;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumInss;
   unsigned NumIns;
   struct Instit Ins;

   /***** Query database *****/
   if ((NumInss = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get institutions")))
     {
      /***** Write heading *****/
      /* Number of institutions found */
      sprintf (Gbl.Title,"%u %s",
               NumInss,NumInss == 1 ? Txt_institution :
				      Txt_institutions);
      Lay_StartRoundFrameTable (NULL,Gbl.Title,NULL,NULL,2);
      Ins_PutHeadInstitutionsForSeeing (false);	// Order not selectable

      /***** List the institutions (one row per institution) *****/
      for (NumIns = 1;
	   NumIns <= NumInss;
	   NumIns++)
	{
	 /* Get next institution */
	 row = mysql_fetch_row (mysql_res);

	 /* Get institution code (row[0]) */
	 Ins.InsCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get data of institution */
	 Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_EXTRA_DATA);

	 /* Write data of this institution */
	 Ins_ListOneInstitutionForSeeing (&Ins,NumIns);
	}

      /***** End table *****/
      Lay_EndRoundFrameTable ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumInss;
  }
