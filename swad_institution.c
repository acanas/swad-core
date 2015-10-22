// swad_institution.c: institutions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

static void Ins_ListInstitutions (void);
static void Ins_ListInstitutionsForSeeing (void);
static void Ins_ListOneInstitutionForSeeing (struct Institution *Ins,unsigned NumIns);
static void Ins_PutHeadInstitutionsForSeeing (bool OrderSelectable);
static void Ins_GetParamInsOrderType (void);
static unsigned Ins_GetNumUsrsInInstitution (long InsCod);
static void Ins_ListInstitutionsForEdition (void);
static bool Ins_CheckIfICanEdit (struct Institution *Ins);
static Ins_StatusTxt_t Ins_GetStatusTxtFromStatusBits (Ins_Status_t Status);
static Ins_Status_t Ins_GetStatusBitsFromStatusTxt (Ins_StatusTxt_t StatusTxt);
static void Ins_PutParamOtherInsCod (long InsCod);
static void Ins_RenameInstitution (Cns_ShortOrFullName_t ShortOrFullName);
static bool Ins_CheckIfInsNameExistsInCty (const char *FieldName,const char *Name,long InsCod,long CtyCod);
static void Ins_PutFormToCreateInstitution (void);
static void Ins_PutHeadInstitutionsForEdition (void);
static void Ins_RecFormRequestOrCreateIns (unsigned Status);
static void Ins_CreateInstitution (struct Institution *Ins,unsigned Status);

/*****************************************************************************/
/***************** List institutions with pending centres ********************/
/*****************************************************************************/

void Ins_SeeInsWithPendingCtrs (void)
  {
   extern const char *Txt_Institutions_with_pending_centres;
   extern const char *Txt_Institution;
   extern const char *Txt_Centres_ABBREVIATION;
   extern const char *Txt_Go_to_X;
   extern const char *Txt_There_are_no_institutions_with_requests_for_centres_to_be_confirmed;
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumInss;
   unsigned NumIns;
   struct Institution Ins;
   const char *BgColor;

   /***** Get institutions with pending centres *****/
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_INS_ADM:
         sprintf (Query,"SELECT centres.InsCod,COUNT(*)"
                        " FROM centres,ins_admin,institutions"
                        " WHERE (centres.Status & %u)<>0"
                        " AND centres.InsCod=ins_admin.InsCod AND ins_admin.UsrCod='%ld'"
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
      Lay_StartRoundFrameTable (NULL,2,Txt_Institutions_with_pending_centres);
      fprintf (Gbl.F.Out,"<tr>"
                         "<th></th>"
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
         Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);

         /* Institution logo */
         fprintf (Gbl.F.Out,"<tr>"
	                    "<td class=\"CENTER_MIDDLE %s\">"
                            "<a href=\"%s\" title=\"%s\" class=\"DAT\" target=\"_blank\">",
                  BgColor,Ins.WWW,Ins.FullName);
         Log_DrawLogo (Sco_SCOPE_INS,Ins.InsCod,Ins.ShortName,
                       16,"CENTER_MIDDLE",true);
         fprintf (Gbl.F.Out,"</a>"
                            "</td>");

         /* Institution full name */
         fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE %s\">",
                  BgColor);
         Act_FormGoToStart (ActSeeCtr);
         Ins_PutParamInsCod (Ins.InsCod);
         sprintf (Gbl.Title,Txt_Go_to_X,Ins.FullName);
         Act_LinkFormSubmit (Gbl.Title,"DAT");
         fprintf (Gbl.F.Out,"%s</a>",
	          Ins.FullName);
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</td>");

         /* Number of pending centres (row[1]) */
         fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE %s\">"
                            "%s"
                            "</td>"
	                    "</tr>",
                  BgColor,row[1]);

         Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
        }

      Lay_EndRoundFrameTable ();
     }
   else
      Lay_ShowAlert (Lay_INFO,Txt_There_are_no_institutions_with_requests_for_centres_to_be_confirmed);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************** Show information of the current institution *****************/
/*****************************************************************************/

void Ins_ShowConfiguration (void)
  {
   Ins_Configuration (false);

   /***** Show help to enroll me *****/
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
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Centres;
   extern const char *Txt_Print;
   extern const char *Txt_Institution;
   extern const char *Txt_Short_name;
   extern const char *Txt_Web;
   extern const char *Txt_Shortcut;
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Txt_QR_code;
   extern const char *Txt_Degrees;
   extern const char *Txt_Courses;
   extern const char *Txt_Departments;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   bool PutLink = !PrintView && Gbl.CurrentIns.Ins.WWW[0];

   if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      /***** Links to show centres, to print view and to upload logo *****/
      if (!PrintView)
	{
         fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

	 /* Link to show centres */
         Act_PutContextualLink (ActSeeCtr,NULL,"ctr",Txt_Centres);

	 /* Link to print view */
         Act_PutContextualLink (ActPrnInsInf,NULL,"print",Txt_Print);

	 /* Link to upload logo */
	 if (Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM)
	    Log_PutFormToChangeLogo (Sco_SCOPE_INS);

	 fprintf (Gbl.F.Out,"</div>");
	}

      /***** Start frame *****/
      Lay_StartRoundFrameTable (NULL,2,NULL);

      /***** Title *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td colspan=\"2\" class=\"TITLE_LOCATION\">");
      if (PutLink)
	 fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
	                    " class=\"TITLE_LOCATION\" title=\"%s\">",
		  Gbl.CurrentIns.Ins.WWW,
		  Gbl.CurrentIns.Ins.FullName);
      Log_DrawLogo (Sco_SCOPE_INS,Gbl.CurrentIns.Ins.InsCod,
                    Gbl.CurrentIns.Ins.ShortName,64,NULL,true);
      fprintf (Gbl.F.Out,"<br />%s",Gbl.CurrentIns.Ins.FullName);
      if (PutLink)
	 fprintf (Gbl.F.Out,"</a>");
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      /***** Institution full name *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
	                 "%s:"
	                 "</td>"
			 "<td class=\"DAT_N LEFT_MIDDLE\">",
	       The_ClassForm[Gbl.Prefs.Theme],
	       Txt_Institution);
      if (PutLink)
	 fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
	                    " class=\"DAT_N\" title=\"%s\">",
		  Gbl.CurrentIns.Ins.WWW,
		  Gbl.CurrentIns.Ins.FullName);
      fprintf (Gbl.F.Out,"%s",
	       Gbl.CurrentIns.Ins.FullName);
      if (PutLink)
	 fprintf (Gbl.F.Out,"</a>");
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      /***** Institution short name *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
			 "%s:"
			 "</td>"
			 "<td class=\"DAT LEFT_MIDDLE\">"
			 "%s"
			 "</td>"
			 "</tr>",
	       The_ClassForm[Gbl.Prefs.Theme],
	       Txt_Short_name,
	       Gbl.CurrentIns.Ins.ShortName);

      /***** Institution WWW *****/
      if (Gbl.CurrentIns.Ins.WWW[0])
	{
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "<a href=\"%s\" target=\"_blank\" class=\"DAT\">",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_Web,
		  Gbl.CurrentIns.Ins.WWW);
	 Str_LimitLengthHTMLStr (Gbl.CurrentIns.Ins.WWW,20);
	 fprintf (Gbl.F.Out,"%s"
			    "</a>"
			    "</td>"
			    "</tr>",
		  Gbl.CurrentIns.Ins.WWW);
	}

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
	       Cfg_HTTPS_URL_SWAD_CGI,Txt_STR_LANG_ID[Gbl.Prefs.Language],Gbl.CurrentIns.Ins.InsCod,
	       Cfg_HTTPS_URL_SWAD_CGI,Txt_STR_LANG_ID[Gbl.Prefs.Language],Gbl.CurrentIns.Ins.InsCod);

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
	 /***** Number of centres *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_Centres,
		  Ctr_GetNumCtrsInIns (Gbl.CurrentIns.Ins.InsCod));

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

	 /***** Number of teachers *****/
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

	 /***** Number of students *****/
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
	}

      /***** End of the frame *****/
      Lay_EndRoundFrameTable ();
     }
  }

/*****************************************************************************/
/**************** List the institutions of the current country ***************/
/*****************************************************************************/

void Ins_ShowInssOfCurrentCty (void)
  {
   if (Gbl.CurrentCty.Cty.CtyCod > 0)
     {
      /***** Get parameter with the type of order in the list of institutions *****/
      Ins_GetParamInsOrderType ();

      /***** Get list of institutions *****/
      Ins_GetListInstitutions (Gbl.CurrentCty.Cty.CtyCod,Ins_GET_EXTRA_DATA);

      /***** Write menu to select country *****/
      Deg_WriteMenuAllCourses ();

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
   extern const char *Txt_No_institutions_have_been_created_in_this_country;
   extern const char *Txt_Create_institution;
   bool ICanEdit = (Gbl.Usrs.Me.LoggedRole >= Rol__GUEST_);

   if (Gbl.Inss.Num)	// There are institutions in the current country
     {
      if (ICanEdit)
	 Lay_PutFormToEdit (ActEdiIns);
      Ins_ListInstitutionsForSeeing ();
     }
   else			// No institutions created in the current country
     {
      Lay_ShowAlert (Lay_INFO,Txt_No_institutions_have_been_created_in_this_country);
      if (ICanEdit)
	{
	 fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
	 Act_PutContextualLink (ActEdiIns,NULL,"edit",Txt_Create_institution);
	 fprintf (Gbl.F.Out,"</div>");
	}
     }
  }

/*****************************************************************************/
/*************** List the institutions of the current country ****************/
/*****************************************************************************/

static void Ins_ListInstitutionsForSeeing (void)
  {
   extern const char *Txt_Institutions_of_COUNTRY_X;
   unsigned NumIns;

   /***** Table head *****/
   sprintf (Gbl.Title,Txt_Institutions_of_COUNTRY_X,
            Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
   Lay_StartRoundFrameTable (NULL,2,Gbl.Title);
   Ins_PutHeadInstitutionsForSeeing (true);	// Order selectable

   /***** Write all the institutions and their nuber of users *****/
   for (NumIns = 0;
	NumIns < Gbl.Inss.Num;
	NumIns++)
      Ins_ListOneInstitutionForSeeing (&(Gbl.Inss.Lst[NumIns]),NumIns + 1);

   /***** Table end *****/
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/********************** List one institution for seeing **********************/
/*****************************************************************************/

static void Ins_ListOneInstitutionForSeeing (struct Institution *Ins,unsigned NumIns)
  {
   extern const char *Txt_Go_to_X;
   extern const char *Txt_INSTITUTION_STATUS[Ins_NUM_STATUS_TXT];
   const char *TxtClass;
   const char *BgColor;
   Crs_StatusTxt_t StatusTxt;

   TxtClass = (Ins->Status & Ins_STATUS_BIT_PENDING) ? "DAT_LIGHT" :
	                                               "DAT";
   BgColor = (Ins->InsCod == Gbl.CurrentIns.Ins.InsCod) ? "LIGHT_BLUE" :
                                                          Gbl.ColorRows[Gbl.RowEvenOdd];

   /***** Number of institution in this list *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s RIGHT_MIDDLE %s\">"
                      "%u"
                      "</td>",
	    TxtClass,BgColor,
            NumIns);

   /***** Icon *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE %s\""
	              " style=\"width:25px;\">"
		      "<a href=\"%s\" target=\"_blank\" title=\"%s\">",
	    BgColor,
	    Ins->WWW,Ins->FullName);
   Log_DrawLogo (Sco_SCOPE_INS,Ins->InsCod,Ins->ShortName,
                 16,NULL,true);
   fprintf (Gbl.F.Out,"</a>"
		      "</td>");

   /***** Name and link to go to this institution *****/
   fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE %s\">",
	    TxtClass,BgColor);
   Act_FormGoToStart (ActSeeInsInf);
   Ins_PutParamInsCod (Ins->InsCod);
   sprintf (Gbl.Title,Txt_Go_to_X,Ins->FullName);
   Act_LinkFormSubmit (Gbl.Title,TxtClass);
   fprintf (Gbl.F.Out,"%s</a>",
	    Ins->FullName);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>");

   /***** Stats *****/
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
	              "%u"
	              "</td>",
	    TxtClass,BgColor,Ins->NumUsrs);
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
	              "%u"
	              "</td>",
	    TxtClass,BgColor,Ins->NumStds);
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
	              "%u"
	              "</td>",
	    TxtClass,BgColor,Ins->NumTchs);
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
	              "%u"
	              "</td>",
	    TxtClass,BgColor,Ins->NumCtrs);
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
	              "%u"
	              "</td>",
	    TxtClass,BgColor,Ins->NumDegs);
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
	              "%u"
	              "</td>",
	    TxtClass,BgColor,Ins->NumDpts);

   /***** Institution status *****/
   StatusTxt = Ins_GetStatusTxtFromStatusBits (Ins->Status);
   fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE %s\">"
	              "%s"
	              "</td>"
		      "</tr>",
	    TxtClass,BgColor,Txt_INSTITUTION_STATUS[StatusTxt]);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/**************** Write header with fields of an institution *****************/
/*****************************************************************************/

static void Ins_PutHeadInstitutionsForSeeing (bool OrderSelectable)
  {
   extern const char *Txt_INSTITUTIONS_HELP_ORDER[2];
   extern const char *Txt_INSTITUTIONS_ORDER[2];
   extern const char *Txt_Students_ABBREVIATION;
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Centres;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_Departments_ABBREVIATION;
   extern const char *Txt_Status;
   Ins_InssOrderType_t Order;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>"
                      "<th style=\"width:25px;\"></th>");
   for (Order = Ins_ORDER_BY_INSTITUTION;
	Order <= Ins_ORDER_BY_NUM_USRS;
	Order++)
     {
      fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">");
      if (OrderSelectable)
	{
	 Act_FormStart (ActSeeIns);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Act_LinkFormSubmit (Txt_INSTITUTIONS_HELP_ORDER[Order],"TIT_TBL");
	 if (Order == Gbl.Inss.SelectedOrderType)
	    fprintf (Gbl.F.Out,"<u>");
	}
      fprintf (Gbl.F.Out,"%s",Txt_INSTITUTIONS_ORDER[Order]);
      if (OrderSelectable)
	{
	 if (Order == Gbl.Inss.SelectedOrderType)
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
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Students_ABBREVIATION,
            Txt_Teachers_ABBREVIATION,
            Txt_Centres,
            Txt_Degrees_ABBREVIATION,
            Txt_Departments_ABBREVIATION,
            Txt_Status);
   }

/*****************************************************************************/
/******* Get parameter with the type or order in list of institutions ********/
/*****************************************************************************/

static void Ins_GetParamInsOrderType (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("Order",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      Gbl.Inss.SelectedOrderType = (Ins_InssOrderType_t) UnsignedNum;
   else
      Gbl.Inss.SelectedOrderType = Ins_DEFAULT_ORDER_TYPE;
  }

/*****************************************************************************/
/************************ Put forms to edit institutions *********************/
/*****************************************************************************/

void Ins_EditInstitutions (void)
  {
   /***** Get list of institutions *****/
   Ins_GetListInstitutions (Gbl.CurrentCty.Cty.CtyCod,Ins_GET_EXTRA_DATA);

   if (Gbl.Inss.Num)
      /***** Put link (form) to view institutions *****/
      Lay_PutFormToView (ActSeeIns);

   /***** Put a form to create a new institution *****/
   Ins_PutFormToCreateInstitution ();

   /***** Forms to edit current institutions *****/
   if (Gbl.Inss.Num)
      Ins_ListInstitutionsForEdition ();

   /***** Free list of institutions *****/
   Ins_FreeListInstitutions ();
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
   struct Institution *Ins;

   /***** Get institutions from database *****/
   switch (GetExtraData)
     {
      case Ins_GET_MINIMAL_DATA:
         if (CtyCod <= 0)	// Get all the institutions, belonging to any country
            sprintf (Query,"SELECT InsCod,CtyCod,Status,RequesterUsrCod,ShortName,FullName,WWW"
                           " FROM institutions"
                           " ORDER BY FullName");
         else			// Get only the institutions belonging to the country specified by CtyCod
            sprintf (Query,"SELECT InsCod,CtyCod,Status,RequesterUsrCod,ShortName,FullName,WWW"
                           " FROM institutions"
                           " WHERE CtyCod='%ld'"
                           " ORDER BY FullName",
                     CtyCod);
         break;
      case Ins_GET_EXTRA_DATA:
         switch (Gbl.Inss.SelectedOrderType)
           {
            case Ins_ORDER_BY_INSTITUTION:
               sprintf (OrderBySubQuery,"FullName");
               break;
            case Ins_ORDER_BY_NUM_USRS:
               sprintf (OrderBySubQuery,"NumUsrs DESC,FullName");
               break;
           }
         if (CtyCod <= 0)	// Get all the institutions, belonging to any country
            sprintf (Query,"(SELECT institutions.InsCod,institutions.CtyCod,"
                           "institutions.Status,institutions.RequesterUsrCod,"
                           "institutions.ShortName,institutions.FullName,"
                           "institutions.WWW,COUNT(*) AS NumUsrs"
                           " FROM institutions,usr_data"
                           " WHERE institutions.InsCod=usr_data.InsCod"
                           " GROUP BY institutions.InsCod)"
                           " UNION "
                           "(SELECT InsCod,CtyCod,Status,RequesterUsrCod,ShortName,FullName,WWW,0 AS NumUsrs"
                           " FROM institutions"
                           " WHERE InsCod NOT IN (SELECT DISTINCT InsCod FROM usr_data))"
                           " ORDER BY %s",
                  OrderBySubQuery);
         else			// Get only the institutions belonging to the country specified by CtyCod
            sprintf (Query,"(SELECT institutions.InsCod,institutions.CtyCod,"
                           "institutions.Status,institutions.RequesterUsrCod,"
                           "institutions.ShortName,institutions.FullName,"
                           "institutions.WWW,COUNT(*) AS NumUsrs"
                           " FROM institutions,usr_data"
                           " WHERE institutions.CtyCod='%ld' AND institutions.InsCod=usr_data.InsCod"
                           " GROUP BY institutions.InsCod)"
                           " UNION "
                           "(SELECT InsCod,CtyCod,Status,RequesterUsrCod,ShortName,FullName,WWW,0 AS NumUsrs"
                           " FROM institutions"
                           " WHERE CtyCod='%ld' AND InsCod NOT IN (SELECT DISTINCT InsCod FROM usr_data))"
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
      if ((Gbl.Inss.Lst = (struct Institution *) calloc (NumRows,sizeof (struct Institution))) == NULL)
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
         strcpy (Ins->ShortName,row[4]);

         /* Get the full name of the institution (row[5]) */
         strncpy (Ins->FullName,row[5],Ins_MAX_LENGTH_INSTITUTION_FULL_NAME);
         Ins->FullName[Ins_MAX_LENGTH_INSTITUTION_FULL_NAME] = '\0';

         /* Get the URL of the institution (row[6]) */
         strcpy (Ins->WWW,row[6]);

         /* Get extra data */
         switch (GetExtraData)
           {
            case Ins_GET_MINIMAL_DATA:
               Ins->NumStds = Ins->NumTchs = Ins->NumUsrs =
               Ins->NumCtrs = Ins->NumDpts = Ins->NumDegs = 0;
               break;
            case Ins_GET_EXTRA_DATA:
               /* Get number of users in this institution (row[7]) */
               if (sscanf (row[7],"%u",&Ins->NumUsrs) == 1)
                 {
                  if (Ins->NumUsrs)
                    {
                     Ins->NumStds = Usr_GetNumberOfUsersInInstitution (Ins->InsCod,Rol_STUDENT);	// Slow query
                     Ins->NumTchs = Usr_GetNumberOfUsersInInstitution (Ins->InsCod,Rol_TEACHER);	// Slow query
                     Ins->NumUsrs = Ins->NumStds + Ins->NumTchs;
                    }
                 }
               else
                  Ins->NumStds = Ins->NumTchs = Ins->NumUsrs = 0;

               /* Get number of centres in this institution */
               Ins->NumCtrs = Ctr_GetNumCtrsInIns (Ins->InsCod);

               /* Get number of departments in this institution */
               Ins->NumDpts = Dpt_GetNumberOfDepartmentsInInstitution (Ins->InsCod);

               /* Get number of degrees in this institution */
               Ins->NumDegs = Deg_GetNumDegsInIns (Ins->InsCod);
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
/************************* Get data of an institution ************************/
/*****************************************************************************/

bool Ins_GetDataOfInstitutionByCod (struct Institution *Ins,
                                    Ins_GetExtraData_t GetExtraData)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool InsFound;

   /***** Clear data *****/
   Ins->CtyCod = -1L;
   Ins->Status = (Ins_Status_t) 0;
   Ins->RequesterUsrCod = -1L;
   Ins->ShortName[0] =
   Ins->FullName[0]  =
   Ins->WWW[0]       = '\0';
   Ins->NumStds = Ins->NumTchs = Ins->NumUsrs =
   Ins->NumCtrs = Ins->NumDpts = Ins->NumDegs = 0;

   /***** Check if institution code is correct *****/
   if (Ins->InsCod <= 0)
      return false;
   // Ins->InsCod > 0

   /***** Get data of an institution from database *****/
   sprintf (Query,"SELECT CtyCod,Status,RequesterUsrCod,ShortName,FullName,WWW"
                  " FROM institutions WHERE InsCod='%ld'",
            Ins->InsCod);

   /***** Count number of rows in result *****/
   if (DB_QuerySELECT (Query,&mysql_res,"can not get data of an institution")) // Institution found...
     {
      InsFound = true;

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
      strcpy (Ins->ShortName,row[3]);

      /* Get the full name of the institution (row[4]) */
      strncpy (Ins->FullName,row[4],Ins_MAX_LENGTH_INSTITUTION_FULL_NAME);
      Ins->FullName[Ins_MAX_LENGTH_INSTITUTION_FULL_NAME] = '\0';

      /* Get the URL of the institution (row[5]) */
      strcpy (Ins->WWW,row[5]);

      /* Get extra data */
      if (GetExtraData == Ins_GET_EXTRA_DATA)
	{
	 /* Get number of users in this institution */
	 Ins->NumStds = Usr_GetNumberOfUsersInInstitution (Ins->InsCod,Rol_STUDENT);	// Slow query
	 Ins->NumTchs = Usr_GetNumberOfUsersInInstitution (Ins->InsCod,Rol_TEACHER);	// Slow query
	 Ins->NumUsrs = Ins->NumStds + Ins->NumTchs;

	 /* Get number of centres in this institution */
	 Ins->NumCtrs = Ctr_GetNumCtrsInIns (Ins->InsCod);

	 /* Get number of departments in this institution */
	 Ins->NumDpts = Dpt_GetNumberOfDepartmentsInInstitution (Ins->InsCod);

	 /* Get number of degrees in this institution */
	 Ins->NumDegs = Deg_GetNumDegsInIns (Ins->InsCod);
	}
     }
   else
     {
      Ins->InsCod = -1L;
      InsFound = false;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return InsFound;
  }

/*****************************************************************************/
/*********** Get the short name of an institution from its code **************/
/*****************************************************************************/

void Ins_GetShortNameOfInstitutionByCod (struct Institution *Ins)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   Ins->ShortName[0] = '\0';
   if (Ins->InsCod > 0)
     {
      /***** Get the short name of an institution from database *****/
      sprintf (Query,"SELECT ShortName FROM institutions"
		     " WHERE InsCod ='%ld'",
	       Ins->InsCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get the short name of an institution") == 1)
	{
	 /***** Get the short name of this institution *****/
	 row = mysql_fetch_row (mysql_res);
	 strcpy (Ins->ShortName,row[0]);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/****************** Get number of users in an institution ********************/
/*****************************************************************************/

static unsigned Ins_GetNumUsrsInInstitution (long InsCod)
  {
   char Query[256];

   /***** Get number of users in an institution from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_data WHERE InsCod='%ld'",
            InsCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not check number of users in an institution");
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
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumInss;
   unsigned NumIns;
   long InsCod;

   /***** Start form *****/
   Act_FormGoToStart (ActSeeCtr);
   fprintf (Gbl.F.Out,"<select name=\"ins\" style=\"width:175px;\"");
   if (Gbl.CurrentCty.Cty.CtyCod > 0)
      fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
               Gbl.FormId);
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
                     " WHERE CtyCod='%ld'"
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

#define Ins_MAX_LENGTH_WWW_ON_SCREEN 10

static void Ins_ListInstitutionsForEdition (void)
  {
   extern const char *Txt_Institutions_of_COUNTRY_X;
   extern const char *Txt_Another_country;
   extern const char *Txt_INSTITUTION_STATUS[Ins_NUM_STATUS_TXT];
   unsigned NumIns;
   struct Institution *Ins;
   unsigned NumCty;
   char WWW[Ins_MAX_LENGTH_WWW_ON_SCREEN+1];
   struct UsrData UsrDat;
   bool ICanEdit;
   Ins_StatusTxt_t StatusTxt;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Get list of countries *****/
   Gbl.Ctys.SelectedOrderType = Cty_ORDER_BY_COUNTRY;
   Cty_GetListCountries (Cty_GET_ONLY_COUNTRIES);

   /***** Write heading *****/
   sprintf (Gbl.Title,Txt_Institutions_of_COUNTRY_X,
            Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
   Lay_StartRoundFrameTable (NULL,2,Gbl.Title);
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
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%ld"
	                 "</td>",
               Ins->InsCod);

      /* Institution logo */
      fprintf (Gbl.F.Out,"<td title=\"%s\" class=\"LEFT_MIDDLE\""
	                 " style=\"width:25px;\">",
               Ins->FullName);
      Log_DrawLogo (Sco_SCOPE_INS,Ins->InsCod,Ins->ShortName,
                    16,NULL,true);
      fprintf (Gbl.F.Out,"</td>");

      /* Country */
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
	{
	 Act_FormStart (ActChgInsCty);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 fprintf (Gbl.F.Out,"<select name=\"OthCtyCod\" style=\"width:100px;\""
			    " onchange=\"document.getElementById('%s').submit();\" />"
			    "<option value=\"0\"",
		  Gbl.FormId);
	 if (Ins->CtyCod == 0)
	    fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%s</option>",Txt_Another_country);
	 for (NumCty = 0;
	      NumCty < Gbl.Ctys.Num;
	      NumCty++)
	   {
	    fprintf (Gbl.F.Out,"<option value=\"%ld\"",Gbl.Ctys.Lst[NumCty].CtyCod);
	    if (Ins->CtyCod == Gbl.Ctys.Lst[NumCty].CtyCod)
	       fprintf (Gbl.F.Out," selected=\"selected\"");
	    fprintf (Gbl.F.Out,">%s</option>",
		     Gbl.Ctys.Lst[NumCty].Name[Gbl.Prefs.Language]);
	   }
	 fprintf (Gbl.F.Out,"</select>");
	 Act_FormEnd ();
	}
      else
	 fprintf (Gbl.F.Out,"%s",Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
      fprintf (Gbl.F.Out,"</td>");

      /* Institution short name */
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (ICanEdit)
	{
	 Act_FormStart (ActRenInsSho);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
	                    " size=\"10\" maxlength=\"%u\" value=\"%s\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Ins_MAX_LENGTH_INSTITUTION_SHORT_NAME,Ins->ShortName,
		  Gbl.FormId);
	 Act_FormEnd ();
	}
      else
	 fprintf (Gbl.F.Out,"%s",Ins->ShortName);
      fprintf (Gbl.F.Out,"</td>");

      /* Institution full name */
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (ICanEdit)
	{
	 Act_FormStart (ActRenInsFul);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
	                    " size=\"30\" maxlength=\"%u\" value=\"%s\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Ins_MAX_LENGTH_INSTITUTION_FULL_NAME,Ins->FullName,
		  Gbl.FormId);
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
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"WWW\""
	                    " size=\"10\" maxlength=\"%u\" value=\"%s\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Cns_MAX_LENGTH_WWW,Ins->WWW,
		  Gbl.FormId);
	 Act_FormEnd ();
	 fprintf (Gbl.F.Out,"</td>");
	}
      else
	{
         strncpy (WWW,Ins->WWW,Ins_MAX_LENGTH_WWW_ON_SCREEN);
         WWW[Ins_MAX_LENGTH_WWW_ON_SCREEN] = '\0';
         fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"DAT\" title=\"%s\">%s",
                  Ins->WWW,Ins->WWW,WWW);
         if (strlen (Ins->WWW) > Ins_MAX_LENGTH_WWW_ON_SCREEN)
            fprintf (Gbl.F.Out,"...");
         fprintf (Gbl.F.Out,"</a>");
	}
      fprintf (Gbl.F.Out,"</td>");

      /* Number of users */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%u"
	                 "</td>",
               Ins->NumUsrs);

      /* Number of centres */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%u"
	                 "</td>",
               Ins->NumCtrs);

      /* Degree status */
      StatusTxt = Ins_GetStatusTxtFromStatusBits (Ins->Status);
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM &&
	  StatusTxt == Ins_STATUS_PENDING)
	{
	 Act_FormStart (ActChgInsSta);
	 Ins_PutParamOtherInsCod (Ins->InsCod);
	 fprintf (Gbl.F.Out,"<select name=\"Status\" style=\"width:100px;\""
			    " onchange=\"document.getElementById('%s').submit();\">"
			    "<option value=\"%u\" selected=\"selected\">%s</option>"
			    "<option value=\"%u\">%s</option>"
			    "</select>",
		  Gbl.FormId,
		  (unsigned) Ins_GetStatusBitsFromStatusTxt (Ins_STATUS_PENDING),
		  Txt_INSTITUTION_STATUS[Ins_STATUS_PENDING],
		  (unsigned) Ins_GetStatusBitsFromStatusTxt (Ins_STATUS_ACTIVE),
		  Txt_INSTITUTION_STATUS[Ins_STATUS_ACTIVE]);
	 Act_FormEnd ();
	}
      else
	 fprintf (Gbl.F.Out,"%s",Txt_INSTITUTION_STATUS[StatusTxt]);
      fprintf (Gbl.F.Out,"</td>");

      /* Degree requester */
      UsrDat.UsrCod = Ins->RequesterUsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);
      fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\" style=\"width:125px;\">"
			 "<table class=\"CELLS_PAD_2\" style=\"width:125px;\">"
			 "<tr>");
      Msg_WriteMsgAuthor (&UsrDat,100,10,"DAT",true,NULL);
      fprintf (Gbl.F.Out,"</tr>"
			 "</table>"
			 "</td>"
			 "</tr>");
     }


   /***** End table *****/
   Lay_EndRoundFrameTable ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }


/*****************************************************************************/
/************ Check if I can edit, remove, etc. an institution ***************/
/*****************************************************************************/

static bool Ins_CheckIfICanEdit (struct Institution *Ins)
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

long Ins_GetParamOtherInsCod (void)
  {
   char LongStr[1+10+1];

   /***** Get parameter with code of institution *****/
   Par_GetParToText ("OthInsCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/**************************** Remove a institution ***************************/
/*****************************************************************************/

void Ins_RemoveInstitution (void)
  {
   extern const char *Txt_To_remove_an_institution_you_must_first_remove_all_centres_and_users_in_the_institution;
   extern const char *Txt_Institution_X_removed;
   char Query[512];
   struct Institution Ins;
   char PathIns[PATH_MAX+1];

   /***** Get institution code *****/
   if ((Ins.InsCod = Ins_GetParamOtherInsCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of institution is missing.");

   /***** Get data of the institution from database *****/
   Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);

   /***** Check if this institution has users *****/
   if (Ctr_GetNumCtrsInIns (Ins.InsCod) ||
       Ins_GetNumUsrsInInstitution (Ins.InsCod))	// Institution has centres or users ==> don't remove
      Lay_ShowAlert (Lay_WARNING,Txt_To_remove_an_institution_you_must_first_remove_all_centres_and_users_in_the_institution);
   else	// Institution has no users ==> remove it
     {
      /***** Remove information related to files in institution *****/
      Brw_RemoveInsFilesFromDB (Ins.InsCod);

      /***** Remove directories of the institution *****/
      sprintf (PathIns,"%s/%s/%02u/%u",
	       Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_INS,
	       (unsigned) (Ins.InsCod % 100),
	       (unsigned) Ins.InsCod);
      Brw_RemoveTree (PathIns);

      /***** Remove institution *****/
      sprintf (Query,"DELETE FROM institutions WHERE InsCod='%ld'",
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
   Ins_RenameInstitution (Cns_SHORT_NAME);
  }

/*****************************************************************************/
/***************** Change the full name of an institution ********************/
/*****************************************************************************/

void Ins_RenameInsFull (void)
  {
   Ins_RenameInstitution (Cns_FULL_NAME);
  }

/*****************************************************************************/
/******************** Change the name of an institution **********************/
/*****************************************************************************/

static void Ins_RenameInstitution (Cns_ShortOrFullName_t ShortOrFullName)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_institution_X_empty;
   extern const char *Txt_The_institution_X_already_exists;
   extern const char *Txt_The_name_of_the_institution_X_has_changed_to_Y;
   extern const char *Txt_The_name_of_the_institution_X_has_not_changed;
   char Query[512];
   struct Institution *Ins;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxLength = 0;		// Initialized to avoid warning
   char *CurrentInsName = NULL;		// Initialized to avoid warning
   char NewInsName[Ins_MAX_LENGTH_INSTITUTION_FULL_NAME+1];

   Ins = &Gbl.Inss.EditingIns;
   switch (ShortOrFullName)
     {
      case Cns_SHORT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxLength = Ins_MAX_LENGTH_INSTITUTION_SHORT_NAME;
         CurrentInsName = Ins->ShortName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxLength = Ins_MAX_LENGTH_INSTITUTION_FULL_NAME;
         CurrentInsName = Ins->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the institution */
   if ((Ins->InsCod = Ins_GetParamOtherInsCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of institution is missing.");

   /* Get the new name for the institution */
   Par_GetParToText (ParamName,NewInsName,MaxLength);

   /***** Get from the database the old names of the institution *****/
   Ins_GetDataOfInstitutionByCod (Ins,Ins_GET_MINIMAL_DATA);

   /***** Check if new name is empty *****/
   if (!NewInsName[0])
     {
      sprintf (Gbl.Message,Txt_You_can_not_leave_the_name_of_the_institution_X_empty,
               CurrentInsName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
   else
     {
      /***** Check if old and new names are the same (this happens when user press enter with no changes in the form) *****/
      if (strcmp (CurrentInsName,NewInsName))	// Different names
        {
         /***** If institution was in database... *****/
         if (Ins_CheckIfInsNameExistsInCty (ParamName,NewInsName,Ins->InsCod,Gbl.CurrentCty.Cty.CtyCod))
           {
            sprintf (Gbl.Message,Txt_The_institution_X_already_exists,
                     NewInsName);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else
           {
            /* Update the table changing old name by new name */
            sprintf (Query,"UPDATE institutions SET %s='%s' WHERE InsCod='%ld'",
                     FieldName,NewInsName,Ins->InsCod);
            DB_QueryUPDATE (Query,"can not update the name of an institution");

            /***** Write message to show the change made *****/
            sprintf (Gbl.Message,Txt_The_name_of_the_institution_X_has_changed_to_Y,
                     CurrentInsName,NewInsName);
            Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
           }
        }
      else	// The same name
        {
         sprintf (Gbl.Message,Txt_The_name_of_the_institution_X_has_not_changed,
                  CurrentInsName);
         Lay_ShowAlert (Lay_INFO,Gbl.Message);
        }
     }

   /***** Show the form again *****/
   strcpy (CurrentInsName,NewInsName);
   Ins_EditInstitutions ();
  }

/*****************************************************************************/
/****** Check if the name of institution exists in the current country *******/
/*****************************************************************************/

static bool Ins_CheckIfInsNameExistsInCty (const char *FieldName,const char *Name,long InsCod,long CtyCod)
  {
   char Query[512];

   /***** Get number of institutions in current country with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM institutions"
                  " WHERE CtyCod='%ld' AND %s='%s' AND InsCod<>'%ld'",
            CtyCod,FieldName,Name,InsCod);
   return (DB_QueryCOUNT (Query,"can not check if the name of an institution already existed") != 0);
  }

/*****************************************************************************/
/******************* Change the country of a institution *********************/
/*****************************************************************************/

void Ins_ChangeInsCountry (void)
  {
   extern const char *Txt_The_institution_X_already_exists;
   extern const char *Txt_The_country_of_the_institution_X_has_changed_Y;
   struct Institution *Ins;
   struct Country NewCty;
   char Query[256];

   Ins = &Gbl.Inss.EditingIns;

   /***** Get parameters from form *****/
   /* Get the code of the institution */
   if ((Ins->InsCod = Ins_GetParamOtherInsCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of institution is missing.");

   /* Get the new country code for the institution */
   if ((NewCty.CtyCod = Cty_GetParamOtherCtyCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of country is missing.");

   /***** Get data of the institution from database *****/
   Ins_GetDataOfInstitutionByCod (Ins,Ins_GET_MINIMAL_DATA);

   /***** Get data of the country from database *****/
   Cty_GetDataOfCountryByCod (&NewCty);

   /***** Check if country has changed *****/
   if (NewCty.CtyCod != Ins->CtyCod)
     {
      /***** Check if it already exists an institution with the same name in the new country *****/
      if (Ins_CheckIfInsNameExistsInCty ("ShortName",Ins->ShortName,-1L,NewCty.CtyCod))
	{
	 sprintf (Gbl.Message,Txt_The_institution_X_already_exists,
		  Ins->ShortName);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	}
      else if (Ins_CheckIfInsNameExistsInCty ("FullName",Ins->FullName,-1L,NewCty.CtyCod))
	{
	 sprintf (Gbl.Message,Txt_The_institution_X_already_exists,
		  Ins->FullName);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	}
      else
	{
	 /***** Update the table changing the country of the institution *****/
	 sprintf (Query,"UPDATE institutions SET CtyCod='%ld'"
	                " WHERE InsCod='%ld'",
		  NewCty.CtyCod,Ins->InsCod);
	 DB_QueryUPDATE (Query,"can not update the country of an institution");

	 /***** Write message to show the change made *****/
	 sprintf (Gbl.Message,Txt_The_country_of_the_institution_X_has_changed_Y,
		  Ins->FullName,NewCty.Name);
	 Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

         Ins->CtyCod = NewCty.CtyCod;
	}
     }

   /***** Show the form again *****/
   Ins_EditInstitutions ();
  }

/*****************************************************************************/
/********************** Change the URL of a institution **********************/
/*****************************************************************************/

void Ins_ChangeInsWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   extern const char *Txt_You_can_not_leave_the_web_address_empty;
   struct Institution *Ins;
   char Query[256+Cns_MAX_LENGTH_WWW];
   char NewWWW[Cns_MAX_LENGTH_WWW+1];

   Ins = &Gbl.Inss.EditingIns;

   /***** Get parameters from form *****/
   /* Get the code of the institution */
   if ((Ins->InsCod = Ins_GetParamOtherInsCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of institution is missing.");

   /* Get the new WWW for the institution */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_LENGTH_WWW);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /* Update the table changing old WWW by new WWW */
      sprintf (Query,"UPDATE institutions SET WWW='%s' WHERE InsCod='%ld'",
               NewWWW,Ins->InsCod);
      DB_QueryUPDATE (Query,"can not update the web of an institution");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Message,Txt_The_new_web_address_is_X,
               NewWWW);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_leave_the_web_address_empty);

   /***** Show the form again *****/
   strcpy (Ins->WWW,NewWWW);
   Ins_EditInstitutions ();
  }

/*****************************************************************************/
/******************** Change the status of an institution ********************/
/*****************************************************************************/

void Ins_ChangeInsStatus (void)
  {
   extern const char *Txt_The_status_of_the_institution_X_has_changed;
   struct Institution *Ins;
   char Query[256];
   char UnsignedNum[10+1];
   Ins_Status_t Status;
   Ins_StatusTxt_t StatusTxt;

   Ins = &Gbl.Inss.EditingIns;

   /***** Get parameters from form *****/
   /* Get institution code */
   if ((Ins->InsCod = Ins_GetParamOtherInsCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of institution is missing.");

   /* Get parameter with status */
   Par_GetParToText ("Status",UnsignedNum,1);
   if (sscanf (UnsignedNum,"%u",&Status) != 1)
      Lay_ShowErrorAndExit ("Wrong status.");
   StatusTxt = Ins_GetStatusTxtFromStatusBits (Status);
   Status = Ins_GetStatusBitsFromStatusTxt (StatusTxt);	// New status

   /***** Get data of institution *****/
   Ins_GetDataOfInstitutionByCod (Ins,Ins_GET_MINIMAL_DATA);

   /***** Update status in table of institutions *****/
   sprintf (Query,"UPDATE institutions SET Status='%u' WHERE InsCod='%ld'",
            (unsigned) Status,Ins->InsCod);
   DB_QueryUPDATE (Query,"can not update the status of an institution");

   Ins->Status = Status;

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_The_status_of_the_institution_X_has_changed,
            Ins->ShortName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the form again *****/
   Ins_EditInstitutions ();
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
   extern const char *Txt_New_institution_of_COUNTRY_X;
   extern const char *Txt_INSTITUTION_STATUS[Ins_NUM_STATUS_TXT];
   extern const char *Txt_Create_institution;
   struct Institution *Ins;

   Ins = &Gbl.Inss.EditingIns;

   /***** Get list of countries *****/
   Gbl.Ctys.SelectedOrderType = Cty_ORDER_BY_COUNTRY;
   Cty_GetListCountries (Cty_GET_ONLY_COUNTRIES);

   /***** Start form *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
      Act_FormStart (ActNewIns);
   else if (Gbl.Usrs.Me.MaxRole >= Rol__GUEST_)
      Act_FormStart (ActReqIns);
   else
      Lay_ShowErrorAndExit ("You can not edit institutions.");

   /***** Start of frame *****/
   sprintf (Gbl.Title,Txt_New_institution_of_COUNTRY_X,
            Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
   Lay_StartRoundFrameTable (NULL,2,Gbl.Title);

   /***** Write heading *****/
   Ins_PutHeadInstitutionsForEdition ();

   /***** Put icon to remove institution *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"BM\">");
   Lay_PutIconRemovalNotAllowed ();
   fprintf (Gbl.F.Out,"</td>");

   /***** Institution code *****/
   fprintf (Gbl.F.Out,"<td></td>");

   /***** Institution logo *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\" style=\"width:25px;\">");
   Log_DrawLogo (Sco_SCOPE_INS,-1L,"",16,NULL,true);
   fprintf (Gbl.F.Out,"</td>");

   /***** Country *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<select name=\"OthCtyCod\""
                      " style=\"width:100px;\" disabled=\"disabled\">"
                      "<option value=\"%ld\" selected=\"selected\">"
                      "%s"
                      "</option>"
                      "</select>"
                      "</td>",
            Gbl.CurrentCty.Cty.CtyCod,
            Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);

   /***** Institution short name *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"ShortName\""
                      " size=\"10\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            Ins_MAX_LENGTH_INSTITUTION_SHORT_NAME,Ins->ShortName);

   /***** Institution full name *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"FullName\""
                      " size=\"30\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            Ins_MAX_LENGTH_INSTITUTION_FULL_NAME,Ins->FullName);

   /***** Institution WWW *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"WWW\""
                      " size=\"10\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            Cns_MAX_LENGTH_WWW,Ins->WWW);

   /***** Number of users ****/
   fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	              "0"
	              "</td>");

   /***** Number of centres *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	              "0"
	              "</td>");

   /***** Centre status *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">"
	              "%s"
	              "</td>",
            Txt_INSTITUTION_STATUS[Ins_STATUS_PENDING]);

   /***** Centre requester *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\" style=\"width:125px;\">"
		      "<table class=\"CELLS_PAD_2\" style=\"width:125px;\">"
		      "<tr>");
   Msg_WriteMsgAuthor (&Gbl.Usrs.Me.UsrDat,100,10,"DAT",true,NULL);
   fprintf (Gbl.F.Out,"</tr>"
		      "</table>"
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
   extern const char *Txt_Country;
   extern const char *Txt_Short_name;
   extern const char *Txt_Full_name;
   extern const char *Txt_WWW;
   extern const char *Txt_Users;
   extern const char *Txt_Centres_ABBREVIATION;
   extern const char *Txt_Status;
   extern const char *Txt_Requester;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th style=\"width:25px;\"></th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
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
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Code,
            Txt_Country,
            Txt_Short_name,
            Txt_Full_name,
            Txt_WWW,
            Txt_Users,
            Txt_Centres_ABBREVIATION,
            Txt_Status,
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
   struct Institution *Ins;

   Ins = &Gbl.Inss.EditingIns;

   /***** Get parameters from form *****/
   /* Set institution country */
   Ins->CtyCod = Gbl.CurrentCty.Cty.CtyCod;

   /* Get institution short name */
   Par_GetParToText ("ShortName",Ins->ShortName,Ins_MAX_LENGTH_INSTITUTION_SHORT_NAME);

   /* Get institution full name */
   Par_GetParToText ("FullName",Ins->FullName,Ins_MAX_LENGTH_INSTITUTION_FULL_NAME);

   /* Get institution WWW */
   Par_GetParToText ("WWW",Ins->WWW,Cns_MAX_LENGTH_WWW);

   if (Ins->ShortName[0] && Ins->FullName[0])	// If there's a institution name
     {
      if (Ins->WWW[0])
        {
         /***** If name of institution was in database... *****/
         if (Ins_CheckIfInsNameExistsInCty ("ShortName",Ins->ShortName,-1L,Gbl.CurrentCty.Cty.CtyCod))
           {
            sprintf (Gbl.Message,Txt_The_institution_X_already_exists,
                     Ins->ShortName);
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

static void Ins_CreateInstitution (struct Institution *Ins,unsigned Status)
  {
   extern const char *Txt_Created_new_institution_X;
   char Query[2048];

   /***** Create a new institution *****/
   sprintf (Query,"INSERT INTO institutions (CtyCod,Status,RequesterUsrCod,"
                  "ShortName,FullName,WWW)"
                  " VALUES ('%ld','%u','%ld',"
                  "'%s','%s','%s')",
            Ins->CtyCod,
            Status,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Ins->ShortName,Ins->FullName,Ins->WWW);
   DB_QueryINSERT (Query,"can not create institution");

   /***** Write success message *****/
   sprintf (Gbl.Message,Txt_Created_new_institution_X,
            Ins->FullName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/********************* Get total number of institutions **********************/
/*****************************************************************************/

unsigned Ins_GetNumInssTotal (void)
  {
   char Query[256];

   /***** Get total number of degrees from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM institutions");
   return (unsigned) DB_QueryCOUNT (Query,"can not get the total number of institutions");
  }

/*****************************************************************************/
/**************** Get number of institutions in a country ********************/
/*****************************************************************************/

unsigned Ins_GetNumInssInCty (long CtyCod)
  {
   char Query[256];

   /***** Get number of degrees of a place from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM institutions"
                  " WHERE CtyCod='%ld'",CtyCod);
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
   char Query[512];

   /***** Get number of institutions with users from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT institutions.InsCod)"
                  " FROM institutions,centres,degrees,courses,crs_usr"
                  " WHERE %sinstitutions.InsCod=centres.InsCod"
                  " AND centres.CtrCod=degrees.CtrCod"
                  " AND degrees.DegCod=courses.DegCod"
                  " AND courses.CrsCod=crs_usr.CrsCod"
                  " AND crs_usr.Role='%u'",
            SubQuery,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of institutions with users");
  }

/*****************************************************************************/
/*************************** List institutions found *************************/
/*****************************************************************************/
// Returns number of institutions found

unsigned Ins_ListInssFound (const char *Query)
  {
   extern const char *Txt_Institutions;
   extern const char *Txt_institution;
   extern const char *Txt_institutions;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumInss;
   unsigned NumIns;
   struct Institution Ins;

   /***** Query database *****/
   if ((NumInss = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get institutions")))
     {
      /***** Write heading *****/
      Lay_StartRoundFrameTable (NULL,2,Txt_Institutions);

      /* Number of institutions found */
      fprintf (Gbl.F.Out,"<tr>"
			 "<th colspan=\"10\" class=\"CENTER_MIDDLE\">");
      if (NumInss == 1)
	 fprintf (Gbl.F.Out,"1 %s",Txt_institution);
      else
	 fprintf (Gbl.F.Out,"%u %s",NumInss,Txt_institutions);
      fprintf (Gbl.F.Out,"</th>"
			 "</tr>");

      Ins_PutHeadInstitutionsForSeeing (false);	// Order not selectable

      /***** List the institutions (one row per centre) *****/
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

	 /* Write data of this centre */
	 Ins_ListOneInstitutionForSeeing (&Ins,NumIns);
	}

      /***** Table end *****/
      Lay_EndRoundFrameTable ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumInss;
  }
