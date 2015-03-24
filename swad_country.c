// swad_country.c: countries

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
#include <math.h>		// For log10, ceil, pow...
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_constant.h"
#include "swad_country.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_institution.h"
#include "swad_parameter.h"
#include "swad_preference.h"
#include "swad_QR.h"
#include "swad_text.h"

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

static void Cty_Configuration (bool PrintView);

static void Cty_GetParamCtyOrderType (void);
static void Cty_GetMapAttribution (long CtyCod,char **MapAttribution);
static void Cty_FreeMapAttribution (char **MapAttribution);
static void Cty_ListCountriesForEdition (void);
static void Cty_PutParamOtherCtyCod (long CtyCod);
static bool Cty_CheckIfNumericCountryCodeExists (long CtyCod);
static bool Cty_CheckIfAlpha2CountryCodeExists (const char Alpha2[2+1]);
static bool Cty_CheckIfCountryNameExists (Txt_Language_t Language,const char *Name,long CtyCod);
static void Cty_PutFormToCreateCountry (void);
static void Cty_PutHeadCountries (void);
static void Cty_CreateCountry (struct Country *Cty);

/*****************************************************************************/
/***************** List countries with pending institutions ******************/
/*****************************************************************************/

void Cty_SeeCtyWithPendingInss (void)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Txt_Countries_with_pending_institutions;
   extern const char *Txt_Country;
   extern const char *Txt_Institutions_ABBREVIATION;
   extern const char *Txt_Go_to_X;
   extern const char *Txt_There_are_no_countries_with_requests_for_institutions_to_be_confirmed;
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtys;
   unsigned NumCty;
   struct Country Cty;
   const char *BgColor;

   /***** Get countries with pending institutions *****/
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_SYS_ADM:
         sprintf (Query,"SELECT institutions.CtyCod,COUNT(*)"
                        " FROM institutions,countries"
                        " WHERE (institutions.Status & %u)<>0"
                        " AND institutions.CtyCod=countries.CtyCod"
                        " GROUP BY institutions.CtyCod ORDER BY countries.Name_%s",
                  (unsigned) Ins_STATUS_BIT_PENDING,
                  Txt_STR_LANG_ID[Gbl.Prefs.Language]);
         break;
      default:	// Forbidden for other users
	 return;
     }

   /***** Get countries *****/
   if ((NumCtys = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get countries with pending institutions")))
     {
      /***** Write heading *****/
      Lay_StartRoundFrameTable10 (NULL,2,Txt_Countries_with_pending_institutions);
      fprintf (Gbl.F.Out,"<tr>"
                         "<th></th>"
                         "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                         "%s"
                         "</th>"
                         "<th class=\"TIT_TBL\" style=\"text-align:right;\">"
                         "%s"
                         "</th>"
                         "</tr>",
               Txt_Country,
               Txt_Institutions_ABBREVIATION);

      /***** List the countries *****/
      for (NumCty = 0;
	   NumCty < NumCtys;
	   NumCty++)
        {
         /* Get next country */
         row = mysql_fetch_row (mysql_res);

         /* Get country code (row[0]) */
         Cty.CtyCod = Str_ConvertStrCodToLongCod (row[0]);
         BgColor = (Cty.CtyCod == Gbl.CurrentCty.Cty.CtyCod) ? VERY_LIGHT_BLUE :
	                                                       Gbl.ColorRows[Gbl.RowEvenOdd];

         /* Get data of country */
         Cty_GetDataOfCountryByCod (&Cty);

         /* Country map */
         fprintf (Gbl.F.Out,"<tr>"
	                    "<td style=\"text-align:center;"
	                    " vertical-align:middle; background-color:%s;\">",
                  BgColor);
         if (Cty_CheckIfCountryMapExists (&Cty))
           {
            fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\">",
                     Cty.WWW[Gbl.Prefs.Language]);
            Cty_DrawCountryMap (&Cty,"COUNTRY_MAP_SMALL");
            fprintf (Gbl.F.Out,"</a>");
           }
         fprintf (Gbl.F.Out,"</td>");

         /* Country name */
         fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:left;"
	                    " vertical-align:middle; background-color:%s;\">",
                  BgColor);
         Act_FormGoToStart (ActSeeIns);
         Cty_PutParamCtyCod (Cty.CtyCod);
         sprintf (Gbl.Title,Txt_Go_to_X,Cty.Name[Gbl.Prefs.Language]);
         Act_LinkFormSubmit (Gbl.Title,"DAT");
         fprintf (Gbl.F.Out,"%s</a>",
	          Cty.Name[Gbl.Prefs.Language]);
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</td>");

         /* Number of pending institutions (row[1]) */
         fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:right;"
	                    " vertical-align:middle; background-color:%s;\">"
	                    "%s"
	                    "</td>"
	                    "</tr>",
                  BgColor,row[1]);

         Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
        }

      Lay_EndRoundFrameTable10 ();
     }
   else
      Lay_ShowAlert (Lay_INFO,Txt_There_are_no_countries_with_requests_for_institutions_to_be_confirmed);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Show information of the current country *******************/
/*****************************************************************************/

void Cty_ShowConfiguration (void)
  {
   Cty_Configuration (false);
  }

/*****************************************************************************/
/***************** Print information of the current country ******************/
/*****************************************************************************/

void Cty_PrintConfiguration (void)
  {
   Cty_Configuration (true);
  }

/*****************************************************************************/
/******************** Information of the current country *********************/
/*****************************************************************************/

static void Cty_Configuration (bool PrintView)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Institutions;
   extern const char *Txt_Country;
   extern const char *Txt_Shortcut;
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Txt_QR_code;
   extern const char *Txt_Centres;
   extern const char *Txt_Degrees;
   extern const char *Txt_Courses;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char *MapAttribution = NULL;
   bool PutLink = !PrintView && Gbl.CurrentCty.Cty.WWW[Gbl.Prefs.Language][0];

   if (Gbl.CurrentCty.Cty.CtyCod > 0)
     {
      /***** Links to show institutions and to print view  *****/
      fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");

      /* Link to show institutions */
      Act_FormStart (ActSeeIns);
      Act_LinkFormSubmit (Txt_Institutions,The_ClassFormul[Gbl.Prefs.Theme]);
      Lay_PutSendIcon ("ins",Txt_Institutions,Txt_Institutions);
      Act_FormEnd ();

      /* Link to print view */
      if (!PrintView)
	{
	 Lay_PutLinkToPrintView1 (ActPrnCtyInf);
	 Lay_PutLinkToPrintView2 ();
	}

      fprintf (Gbl.F.Out,"</div>");

      /***** Start frame *****/
      Lay_StartRoundFrameTable10 (NULL,2,NULL);

      /***** Title *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td colspan=\"2\" class=\"TITLE_LOCATION\""
	                 " style=\"text-align:center;\">");
      if (PutLink)
	 fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
	                    " class=\"TITLE_LOCATION\" title=\"%s\">",
		  Gbl.CurrentCty.Cty.WWW[Gbl.Prefs.Language],
		  Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
      fprintf (Gbl.F.Out,"%s",Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
      if (PutLink)
	 fprintf (Gbl.F.Out,"</a>");
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      /***** Country map (and link to WWW if exists) *****/
      if (Cty_CheckIfCountryMapExists (&Gbl.CurrentCty.Cty))
	{
	 /* Get map attribution */
	 Cty_GetMapAttribution (Gbl.CurrentCty.Cty.CtyCod,&MapAttribution);

	 /* Map image */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td colspan=\"2\" class=\"DAT_SMALL\""
			    " style=\"text-align:center;\">");
	 if (PutLink)
	    fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\">",
		     Gbl.CurrentCty.Cty.WWW[Gbl.Prefs.Language]);
	 Cty_DrawCountryMap (&Gbl.CurrentCty.Cty,PrintView ? "COUNTRY_MAP_PRINT" :
			                                     "COUNTRY_MAP_SHOW");
	 if (PutLink)
	    fprintf (Gbl.F.Out,"</a>");
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Map attribution */
	 if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM && !PrintView)
	   {
	    fprintf (Gbl.F.Out,"<tr>"
			       "<td colspan=\"2\""
			       " style=\"text-align:center;\">");
	    Act_FormStart (ActChgCtyMapAtt);
	    fprintf (Gbl.F.Out,"<textarea name=\"Attribution\" cols=\"50\" rows=\"2\""
			       " onchange=\"javascript:document.getElementById('%s').submit();\">",
		     Gbl.FormId);
            if (MapAttribution)
	       fprintf (Gbl.F.Out,"%s",MapAttribution);
	    fprintf (Gbl.F.Out,"</textarea>");
	    Act_FormEnd ();
	    fprintf (Gbl.F.Out,"</td>"
		               "</tr>");
           }
	 else if (MapAttribution)
	    fprintf (Gbl.F.Out,"<tr>"
			       "<td colspan=\"2\" class=\"ATTRIBUTION\""
			       " style=\"text-align:center;\">"
			       "%s"
                               "</td>"
			       "</tr>",
	             MapAttribution);

	 /* Free memory used for map attribution */
	 Cty_FreeMapAttribution (&MapAttribution);
	}

      /***** Country name (an link to WWW if exists) *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s\""
			 " style=\"text-align:right; vertical-align:middle;\">"
			 "%s:"
			 "</td>"
			 "<td class=\"DAT_N\""
			 " style=\"text-align:left; vertical-align:middle;\">",
	       The_ClassFormul[Gbl.Prefs.Theme],
	       Txt_Country);
      if (!PrintView && Gbl.CurrentCty.Cty.WWW[Gbl.Prefs.Language][0])
	 fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"DAT_N\">",
		  Gbl.CurrentCty.Cty.WWW[Gbl.Prefs.Language]);
      fprintf (Gbl.F.Out,"%s",Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
      if (!PrintView && Gbl.CurrentCty.Cty.WWW[Gbl.Prefs.Language][0])
	 fprintf (Gbl.F.Out,"</a>");
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      /***** Link to the country inside platform *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s\""
			 " style=\"text-align:right; vertical-align:middle;\">"
			 "%s:"
			 "</td>"
			 "<td class=\"DAT\""
			 " style=\"text-align:left; vertical-align:middle;\">"
			 "<a href=\"%s/%s?cty=%ld\" class=\"DAT\" target=\"_blank\">"
			 "%s/%s?cty=%ld</a>"
			 "</td>"
			 "</tr>",
	       The_ClassFormul[Gbl.Prefs.Theme],
	       Txt_Shortcut,
	       Cfg_HTTPS_URL_SWAD_CGI,Txt_STR_LANG_ID[Gbl.Prefs.Language],Gbl.CurrentCty.Cty.CtyCod,
	       Cfg_HTTPS_URL_SWAD_CGI,Txt_STR_LANG_ID[Gbl.Prefs.Language],Gbl.CurrentCty.Cty.CtyCod);

      if (PrintView)
	{
	 /***** QR code with link to the country *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">",
		  The_ClassFormul[Gbl.Prefs.Theme],
		  Txt_QR_code);
	 QR_LinkTo (200,"cty",Gbl.CurrentCty.Cty.CtyCod);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");
	}
      else
	{
	 /***** Number of institutions *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassFormul[Gbl.Prefs.Theme],
		  Txt_Institutions,
		  Ins_GetNumInssInCty (Gbl.CurrentCty.Cty.CtyCod));

	 /***** Number of centres *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassFormul[Gbl.Prefs.Theme],
		  Txt_Centres,
		  Ctr_GetNumCtrsInCty (Gbl.CurrentCty.Cty.CtyCod));

	 /***** Number of degrees *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassFormul[Gbl.Prefs.Theme],
		  Txt_Degrees,
		  Deg_GetNumDegsInCty (Gbl.CurrentCty.Cty.CtyCod));

	 /***** Number of courses *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassFormul[Gbl.Prefs.Theme],
		  Txt_Courses,
		  Crs_GetNumCrssInCty (Gbl.CurrentCty.Cty.CtyCod));

	 /***** Number of teachers *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassFormul[Gbl.Prefs.Theme],
		  Txt_ROLES_PLURAL_Abc[Rol_ROLE_TEACHER][Usr_SEX_UNKNOWN],
		  Usr_GetNumUsrsInCrssOfCty (Rol_ROLE_TEACHER,Gbl.CurrentCty.Cty.CtyCod));

	 /***** Number of students *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassFormul[Gbl.Prefs.Theme],
		  Txt_ROLES_PLURAL_Abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN],
		  Usr_GetNumUsrsInCrssOfCty (Rol_ROLE_STUDENT,Gbl.CurrentCty.Cty.CtyCod));
	}

      /***** End frame *****/
      Lay_EndRoundFrameTable10 ();
     }
  }

/*****************************************************************************/
/*************************** List all the countries **************************/
/*****************************************************************************/

void Cty_ListCountries (void)
  {
   Cty_ListCountries1 ();
   Cty_ListCountries2 ();
  }

/*****************************************************************************/
/*************************** List all the countries **************************/
/*****************************************************************************/

void Cty_ListCountries1 (void)
  {
   /***** Get parameter with the type of order in the list of countries *****/
   Cty_GetParamCtyOrderType ();

   /***** Get list of countries *****/
   Cty_GetListCountries (Cty_GET_EXTRA_DATA);
  }

void Cty_ListCountries2 (void)
  {
   extern const char *Txt_Countries;
   extern const char *Txt_COUNTRIES_HELP_ORDER[2];
   extern const char *Txt_COUNTRIES_ORDER[2];
   extern const char *Txt_Students_ABBREVIATION;
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Institutions_ABBREVIATION;
   extern const char *Txt_Go_to_X;
   extern const char *Txt_Other_countries;
   extern const char *Txt_Country_unspecified;
   Cty_CtysOrderType_t Order;
   unsigned NumCty;
   unsigned NumUsrs;
   unsigned NumStds;
   unsigned NumTchs;
   unsigned NumUsrsWithCountry = 0;
   unsigned NumStdsWithCountry = 0;
   unsigned NumTchsWithCountry = 0;
   unsigned NumInssWithCountry = 0;
   unsigned NumUsrsInOtherCtys;
   unsigned NumInssInOtherCtys;
   const char *BgColor;

   /***** Put link (form) to edit countries *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
      Lay_PutFormToEdit (ActEdiCty);

   /***** Table head *****/
   Lay_StartRoundFrameTable10 (NULL,2,Txt_Countries);
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"COUNTRY_MAP_SMALL\"></th>");
   for (Order = Cty_ORDER_BY_COUNTRY;
	Order <= Cty_ORDER_BY_NUM_USRS;
	Order++)
     {
      fprintf (Gbl.F.Out,"<th class=\"TIT_TBL\" style=\"text-align:left;\">");
      Act_FormStart (ActSeeCty);
      Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
      Act_LinkFormSubmit (Txt_COUNTRIES_HELP_ORDER[Order],"TIT_TBL");
      if (Order == Gbl.Ctys.SelectedOrderType)
         fprintf (Gbl.F.Out,"<u>");
      fprintf (Gbl.F.Out,"%s",Txt_COUNTRIES_ORDER[Order]);
      if (Order == Gbl.Ctys.SelectedOrderType)
         fprintf (Gbl.F.Out,"</u>");
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</th>");
     }
   fprintf (Gbl.F.Out,"<th class=\"TIT_TBL\" style=\"text-align:right;\">"
	              "%s"
	              "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:right;\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:right;\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Students_ABBREVIATION,
            Txt_Teachers_ABBREVIATION,
            Txt_Institutions_ABBREVIATION);

   /***** Write all the countries and their number of users and institutions *****/
   for (NumCty = 0;
	NumCty < Gbl.Ctys.Num;
	NumCty++)
     {
      BgColor = (Gbl.Ctys.Lst[NumCty].CtyCod ==
	         Gbl.CurrentCty.Cty.CtyCod) ? VERY_LIGHT_BLUE :
                                              Gbl.ColorRows[Gbl.RowEvenOdd];

      /***** Country map (and link to WWW if exists) *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"COUNTRY_MAP_SMALL\""
			 " style=\"text-align:center; background-color:%s;\">",
	       BgColor);
      if (Cty_CheckIfCountryMapExists (&Gbl.Ctys.Lst[NumCty]))
	{
	 /* Map image */
	 Act_FormGoToStart (ActSeeCtyInf);
	 Cty_PutParamCtyCod (Gbl.Ctys.Lst[NumCty].CtyCod);
	 sprintf (Gbl.Title,Txt_Go_to_X,
		  Gbl.Ctys.Lst[NumCty].Name[Gbl.Prefs.Language]);
	 Act_LinkFormSubmit (Gbl.Title,NULL);
	 Cty_DrawCountryMap (&Gbl.Ctys.Lst[NumCty],"COUNTRY_MAP_SMALL");
         fprintf (Gbl.F.Out,"</a>");
         Act_FormEnd ();
	}
      fprintf (Gbl.F.Out,"</td>");

      /* Name and link to go to this country */
      fprintf (Gbl.F.Out,"<td class=\"DAT\""
	                 " style=\"text-align:left; background-color:%s;\">",
	       BgColor);
      Act_FormGoToStart (ActSeeCtyInf);
      Cty_PutParamCtyCod (Gbl.Ctys.Lst[NumCty].CtyCod);
      sprintf (Gbl.Title,Txt_Go_to_X,
	       Gbl.Ctys.Lst[NumCty].Name[Gbl.Prefs.Language]);
      Act_LinkFormSubmit (Gbl.Title,"DAT");
      fprintf (Gbl.F.Out,"%s (%s)</a>",
	       Gbl.Ctys.Lst[NumCty].Name[Gbl.Prefs.Language],
	       Gbl.Ctys.Lst[NumCty].Alpha2);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Write stats of this country */
      NumStds = Usr_GetNumUsrsInCountry (Rol_ROLE_STUDENT,Gbl.Ctys.Lst[NumCty].CtyCod);
      NumTchs = Usr_GetNumUsrsInCountry (Rol_ROLE_TEACHER,Gbl.Ctys.Lst[NumCty].CtyCod);
      fprintf (Gbl.F.Out,"<td class=\"DAT\""
	                 " style=\"text-align:right; background-color:%s;\">"
	                 "%u"
	                 "</td>"
                         "<td class=\"DAT\""
                         " style=\"text-align:right; background-color:%s;\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT\""
                         " style=\"text-align:right; background-color:%s;\">"
                         "%u"
                         "</td>"
                         "<td class=\"DAT\""
                         " style=\"text-align:right; background-color:%s;\">"
                         "%u"
                         "</td>"
			 "</tr>",
	       BgColor,Gbl.Ctys.Lst[NumCty].NumUsrs,
	       BgColor,NumStds,
	       BgColor,NumTchs,
	       BgColor,Gbl.Ctys.Lst[NumCty].NumInss);
      NumUsrsWithCountry += Gbl.Ctys.Lst[NumCty].NumUsrs;
      NumInssWithCountry += Gbl.Ctys.Lst[NumCty].NumInss;
      NumStdsWithCountry += NumStds;
      NumTchsWithCountry += NumTchs;

      Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
     }

   /***** Separation row *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td colspan=\"6\" class=\"DAT\""
                      " style=\"text-align:center;\">"
                      "&nbsp;"
                      "</td>"
                      "</tr>");

   /***** Write institutions and users with other country *****/
   NumUsrsInOtherCtys = Usr_GetNumUsrsInCountry (Rol_ROLE_UNKNOWN,0);	// Here Rol_ROLE_UNKNOWN means "all users"
   NumStds            = Usr_GetNumUsrsInCountry (Rol_ROLE_STUDENT,0);
   NumTchs            = Usr_GetNumUsrsInCountry (Rol_ROLE_TEACHER,0);
   NumInssInOtherCtys = Ins_GetNumInssInCty (0);
   fprintf (Gbl.F.Out,"<tr>"
                      "<td></td>"
                      "<td class=\"DAT\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "<td class=\"DAT\" style=\"text-align:right;\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT\" style=\"text-align:right;\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT\" style=\"text-align:right;\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT\" style=\"text-align:right;\">"
                      "%u"
                      "</td>"
                      "</tr>",
            Txt_Other_countries,
            NumUsrsInOtherCtys,NumStds,NumTchs,NumInssInOtherCtys);
   NumUsrsWithCountry += NumUsrsInOtherCtys;
   NumStdsWithCountry += NumStds;
   NumTchsWithCountry += NumTchs;
   NumInssWithCountry += NumInssInOtherCtys;

   /***** Write institutions with no country *****/
   NumStds = Sta_GetTotalNumberOfUsers (Sco_SCOPE_SYS,Rol_ROLE_STUDENT);
   NumTchs = Sta_GetTotalNumberOfUsers (Sco_SCOPE_SYS,Rol_ROLE_TEACHER);
   NumUsrs = Sta_GetTotalNumberOfUsers (Sco_SCOPE_SYS,Rol_ROLE_UNKNOWN);	// NumUsrs >= NumStds + NumTchs
   fprintf (Gbl.F.Out,"<tr>"
                      "<td></td>"
                      "<td class=\"DAT\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "<td class=\"DAT\" style=\"text-align:right;\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT\" style=\"text-align:right;\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT\" style=\"text-align:right;\">"
                      "%u"
                      "</td>"
                      "<td class=\"DAT\" style=\"text-align:right;\">"
                      "%u"
                      "</td>"
                      "</tr>",
            Txt_Country_unspecified,
            NumUsrs - NumUsrsWithCountry,
            NumStds - NumStdsWithCountry,
            NumTchs - NumTchsWithCountry,
            Ins_GetNumInssTotal () - NumInssWithCountry);

   /***** Table end *****/
   Lay_EndRoundFrameTable10 ();

   /***** Div for Google Geochart *****/
   if (Gbl.CurrentAct == ActSeeCty)
     {
      fprintf (Gbl.F.Out,"<div id='chart_div' style=\"width:500px;"
	                 " margin:10px auto;\">"
                         "</div>");
     }

   /***** Free list of countries *****/
   Cty_FreeListCountries ();
  }

/*****************************************************************************/
/*********************** Check if country map exists *************************/
/*****************************************************************************/

bool Cty_CheckIfCountryMapExists (struct Country *Cty)
  {
   char PathMap[PATH_MAX+1];

   sprintf (PathMap,"%s/%s/%s/%s/%s.png",
	    Cfg_PATH_SWAD_PUBLIC,
	    Cfg_FOLDER_PUBLIC_ICON,
	    Cfg_ICON_FOLDER_COUNTRIES,
	    Cty->Alpha2,
	    Cty->Alpha2);
   return Fil_CheckIfPathExists (PathMap);
  }

/*****************************************************************************/
/***************************** Draw country map ******************************/
/*****************************************************************************/

void Cty_DrawCountryMap (struct Country *Cty,const char *Class)
  {
   /***** Draw country map *****/
   fprintf (Gbl.F.Out,"<img src=\"%s/%s/%s/%s.png\" alt=\"%s\" title=\"%s\""
		      " class=\"%s\" />",
	    Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_COUNTRIES,
	    Cty->Alpha2,
	    Cty->Alpha2,
	    Cty->Alpha2,
	    Cty->Name[Gbl.Prefs.Language],
	    Class);
  }

/*****************************************************************************/
/********************** Write script for Google Geochart *********************/
/*****************************************************************************/

void Cty_WriteScriptGoogleGeochart (void)
  {
   extern const char *Txt_Country_NO_HTML;
   extern const char *Txt_Users_NO_HTML;
   extern const char *Txt_Institutions_NO_HTML;
   extern const char *The_TabOnBgColors[The_NUM_THEMES];
   unsigned NumCty;
   unsigned NumUsrsWithCountry = 0;
   unsigned NumCtysWithUsrs = 0;

   /***** Write start of the script *****/
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>\n"
                      "<script type=\"text/javascript\">\n"
                      "	google.load('visualization', '1', {'packages': ['geochart']});\n"
                      "	google.setOnLoadCallback(drawRegionsMap);\n"
                      "	function drawRegionsMap() {\n"
                      "	var data = new google.visualization.DataTable();\n"
                      "	data.addColumn('string', '%s');\n"
                      "	data.addColumn('number', '%s');\n"
                      "	data.addColumn('number', '%s');\n"
                      "	data.addRows([\n",
            Txt_Country_NO_HTML,
            Txt_Users_NO_HTML,
            Txt_Institutions_NO_HTML);

   /***** Write all the countries and their number of users and institutions *****/
   for (NumCty = 0;
	NumCty < Gbl.Ctys.Num;
	NumCty++)
      if (Gbl.Ctys.Lst[NumCty].NumUsrs)
        {
         /* Write data of this country */
         fprintf (Gbl.F.Out,"	['%s', %u, %u],\n",
                  Gbl.Ctys.Lst[NumCty].Alpha2,
                  Gbl.Ctys.Lst[NumCty].NumUsrs,
                  Gbl.Ctys.Lst[NumCty].NumInss);
         NumUsrsWithCountry += Gbl.Ctys.Lst[NumCty].NumUsrs;
         NumCtysWithUsrs++;
        }

   /***** Write end of the script *****/
   fprintf (Gbl.F.Out,"	]);\n"
                      "	var options = {\n"
                      "		width:500,\n"
                      "		height:300,\n"
                      "		backgroundColor:'%s',\n"
                      "		datalessRegionColor:'white',\n"
                      "		colorAxis:{colors:['%s','black'],minValue:0,maxValue:%u}};\n"
                      "	var chart = new google.visualization.GeoChart(document.getElementById('chart_div'));\n"
                      "	chart.draw(data, options);\n"
                      "	};\n"
                      "</script>\n",
            The_TabOnBgColors[Gbl.Prefs.Theme],
            The_TabOnBgColors[Gbl.Prefs.Theme],
            NumCtysWithUsrs ? (unsigned) pow (10.0,ceil (log10 (2.0 * (double) NumUsrsWithCountry /
                                                                      (double) NumCtysWithUsrs))) :
        	              0);	// colorAxis.maxValue = 2*Average_number_of_users
  }

/*****************************************************************************/
/******** Get parameter with the type or order in list of countries **********/
/*****************************************************************************/

static void Cty_GetParamCtyOrderType (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("Order",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      Gbl.Ctys.SelectedOrderType = (Cty_CtysOrderType_t) UnsignedNum;
   else
      Gbl.Ctys.SelectedOrderType = Cty_DEFAULT_ORDER_TYPE;
  }

/*****************************************************************************/
/******************** Put forms to edit institution types ********************/
/*****************************************************************************/

void Cty_EditCountries (void)
  {
   extern const char *Txt_No_countries_have_been_created;

   /***** Get list of countries *****/
   Gbl.Ctys.SelectedOrderType = Cty_ORDER_BY_COUNTRY;
   Cty_GetListCountries (Cty_GET_EXTRA_DATA);

   if (Gbl.Ctys.Num)
      /***** Put link (form) to view countries *****/
      Lay_PutFormToView (ActSeeCty);
   else
      /***** Help message *****/
      Lay_ShowAlert (Lay_INFO,Txt_No_countries_have_been_created);

   /***** Put a form to create a new country *****/
   Cty_PutFormToCreateCountry ();

   /***** Forms to edit current countries *****/
   if (Gbl.Ctys.Num)
      Cty_ListCountriesForEdition ();

   /***** Free list of countries *****/
   Cty_FreeListCountries ();
  }

/*****************************************************************************/
/************************** List all the countries ***************************/
/*****************************************************************************/

void Cty_GetListCountries (Cty_GetExtraData_t GetExtraData)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   char StrField[32];
   char SubQueryNam1[Txt_NUM_LANGUAGES*32];
   char SubQueryNam2[Txt_NUM_LANGUAGES*32];
   char SubQueryWWW1[Txt_NUM_LANGUAGES*32];
   char SubQueryWWW2[Txt_NUM_LANGUAGES*32];
   char OrderBySubQuery[256];
   char Query[1024+Txt_NUM_LANGUAGES*32*4];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumCty;
   struct Country *Cty;
   Txt_Language_t Lan;

   /***** Get countries from database *****/
   switch (GetExtraData)
     {
      case Cty_GET_ONLY_COUNTRIES:
         sprintf (Query,"SELECT CtyCod,Alpha2,Name_%s"
                        " FROM countries ORDER BY Name_%s",
                  Txt_STR_LANG_ID[Gbl.Prefs.Language],
                  Txt_STR_LANG_ID[Gbl.Prefs.Language]);
         break;
      case Cty_GET_EXTRA_DATA:
         SubQueryNam1[0] = '\0';
         SubQueryNam2[0] = '\0';
         SubQueryWWW1[0] = '\0';
         SubQueryWWW2[0] = '\0';
         for (Lan = (Txt_Language_t) 0;
              Lan < Txt_NUM_LANGUAGES;
              Lan++)
           {
            sprintf (StrField,"countries.Name_%s,",
        	     Txt_STR_LANG_ID[Lan]);
            strcat (SubQueryNam1,StrField);
            sprintf (StrField,"Name_%s,",
        	     Txt_STR_LANG_ID[Lan]);
            strcat (SubQueryNam2,StrField);

            sprintf (StrField,"countries.WWW_%s,",
        	     Txt_STR_LANG_ID[Lan]);
            strcat (SubQueryWWW1,StrField);
            sprintf (StrField,"WWW_%s,",
        	     Txt_STR_LANG_ID[Lan]);
            strcat (SubQueryWWW2,StrField);
           }

         switch (Gbl.Ctys.SelectedOrderType)
           {
            case Cty_ORDER_BY_COUNTRY:
               sprintf (OrderBySubQuery,"Name_%s",
        	        Txt_STR_LANG_ID[Gbl.Prefs.Language]);
               break;
            case Cty_ORDER_BY_NUM_USRS:
               sprintf (OrderBySubQuery,"NumUsrs DESC,Name_%s",
        	        Txt_STR_LANG_ID[Gbl.Prefs.Language]);
               break;
           }
         sprintf (Query,"(SELECT countries.CtyCod,countries.Alpha2,%s%sCOUNT(*) AS NumUsrs"
                        " FROM countries,usr_data"
                        " WHERE countries.CtyCod=usr_data.CtyCod"
                        " GROUP BY countries.CtyCod)"
                        " UNION "
                        "(SELECT CtyCod,Alpha2,%s%s0 AS NumUsrs"
                        " FROM countries"
                        " WHERE CtyCod NOT IN (SELECT DISTINCT CtyCod FROM usr_data))"
                        " ORDER BY %s",
                  SubQueryNam1,SubQueryWWW1,
                  SubQueryNam2,SubQueryWWW2,OrderBySubQuery);
         break;
     }

   /***** Count number of rows in result *****/
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get countries");
   if (NumRows) // Countries found...
     {
      Gbl.Ctys.Num = (unsigned) NumRows;

      /***** Create list with countries *****/
      if ((Gbl.Ctys.Lst = (struct Country *) calloc (NumRows,sizeof (struct Country))) == NULL)
          Lay_ShowErrorAndExit ("Not enough memory to store countries.");

      /***** Get the countries *****/
      for (NumCty = 0;
	   NumCty < Gbl.Ctys.Num;
	   NumCty++)
        {
         Cty = &(Gbl.Ctys.Lst[NumCty]);

         /* Get next country */
         row = mysql_fetch_row (mysql_res);

         /* Get numerical country code (row[0]) */
         if ((Cty->CtyCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of country.");

         /* Get Alpha-2 country code (row[1]) */
         strncpy (Cty->Alpha2,row[1],2);
         Cty->Alpha2[2] = '\0';

         switch (GetExtraData)
           {
            case Cty_GET_ONLY_COUNTRIES:
               /* Get the name of the country in current language */
               strcpy (Cty->Name[Gbl.Prefs.Language],row[2]);
               break;
            case Cty_GET_EXTRA_DATA:
               /* Get the name of the country in several languages */
               for (Lan = (Txt_Language_t) 0;
        	    Lan < Txt_NUM_LANGUAGES;
        	    Lan++)
        	 {
                  strcpy (Cty->Name[Lan],row[2+Lan]);
                  strcpy (Cty->WWW[Lan],row[2+Txt_NUM_LANGUAGES+Lan]);
        	 }

               /* Get number of users in this country */
               if (sscanf (row[2+Txt_NUM_LANGUAGES*2],"%u",&Cty->NumUsrs) != 1)
                  Cty->NumUsrs = 0;

               /* Get number of institutions in this country */
               Cty->NumInss = Ins_GetNumInssInCty (Cty->CtyCod);
               break;
           }
        }
     }
   else
      Gbl.Ctys.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************** Write selector of country ************************/
/*****************************************************************************/

void Cty_WriteSelectorOfCountry (Act_Action_t NextAction)
  {
   extern const char *Txt_Country;
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtys;
   unsigned NumCty;
   long CtyCod;

   /***** Start form *****/
   Act_FormGoToStart (NextAction);
   fprintf (Gbl.F.Out,"<select name=\"CtyCod\" style=\"width:140px;\""
                      " onchange=\"javascript:document.getElementById('%s').submit();\">"
                      "<option value=\"\"",
	    Gbl.FormId);
   if (Gbl.CurrentCty.Cty.CtyCod < 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out," disabled=\"disabled\">[%s]</option>",
            Txt_Country);

   /***** Get countries from database *****/
   sprintf (Query,"SELECT DISTINCT CtyCod,Name_%s"
	          " FROM countries"
                  " ORDER BY countries.Name_%s",
            Txt_STR_LANG_ID[Gbl.Prefs.Language],
            Txt_STR_LANG_ID[Gbl.Prefs.Language]);
   NumCtys = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get countries");

   /***** List countries *****/
   for (NumCty = 0;
	NumCty < NumCtys;
	NumCty++)
     {
      /* Get next country */
      row = mysql_fetch_row (mysql_res);

      /* Get country code (row[0]) */
      if ((CtyCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
         Lay_ShowErrorAndExit ("Wrong code of country.");

      /* Write option */
      fprintf (Gbl.F.Out,"<option value=\"%ld\"",CtyCod);
      if (CtyCod == Gbl.CurrentCty.Cty.CtyCod)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",row[1]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End form *****/
   fprintf (Gbl.F.Out,"</select>");
   Act_FormEnd ();
  }

/*****************************************************************************/
/**************************** Get country full name **************************/
/*****************************************************************************/

bool Cty_GetDataOfCountryByCod (struct Country *Cty)
  {
   extern const char *Txt_Another_country;
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   char StrField[32];
   char SubQueryNam1[Txt_NUM_LANGUAGES*32];
   char SubQueryNam2[Txt_NUM_LANGUAGES*32];
   char SubQueryWWW1[Txt_NUM_LANGUAGES*32];
   char SubQueryWWW2[Txt_NUM_LANGUAGES*32];
   char Query[1024+Txt_NUM_LANGUAGES*32*4];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   Txt_Language_t Lan;
   bool CtyFound;

   if (Cty->CtyCod < 0)
      return false;

   /***** Clear data *****/
   for (Lan = (Txt_Language_t) 0;
	Lan < Txt_NUM_LANGUAGES;
	Lan++)
     {
      Cty->Name[Lan][0] = '\0';
      Cty->WWW[Lan][0] = '\0';
     }
   Cty->NumUsrs = 0;
   Cty->NumInss = 0;

   /***** Check if country code is correct *****/
   if (Cty->CtyCod == 0)
     {
      for (Lan = (Txt_Language_t) 0;
	   Lan < Txt_NUM_LANGUAGES;
	   Lan++)
         if (Lan == Gbl.Prefs.Language)
            strcpy (Cty->Name[Lan],Txt_Another_country);
         else
            Cty->Name[Lan][0] = '\0';
      return false;
     }

   // Cty->CtyCod > 0
   /***** Get data of a country from database *****/
   SubQueryNam1[0] = '\0';
   SubQueryNam2[0] = '\0';
   SubQueryWWW1[0] = '\0';
   SubQueryWWW2[0] = '\0';
   for (Lan = (Txt_Language_t) 0;
	Lan < Txt_NUM_LANGUAGES;
	Lan++)
     {
      sprintf (StrField,"countries.Name_%s,",Txt_STR_LANG_ID[Lan]);
      strcat (SubQueryNam1,StrField);
      sprintf (StrField,"Name_%s,",Txt_STR_LANG_ID[Lan]);
      strcat (SubQueryNam2,StrField);

      sprintf (StrField,"countries.WWW_%s,",Txt_STR_LANG_ID[Lan]);
      strcat (SubQueryWWW1,StrField);
      sprintf (StrField,"WWW_%s,",Txt_STR_LANG_ID[Lan]);
      strcat (SubQueryWWW2,StrField);
     }
   sprintf (Query,"(SELECT Alpha2,%s%sCOUNT(*)"
                  " FROM countries,institutions"
                  " WHERE countries.CtyCod='%03ld' AND countries.CtyCod=institutions.CtyCod"
                  " GROUP BY countries.CtyCod)"
                  " UNION "
                  "(SELECT Alpha2,%s%s0"
                  " FROM countries"
                  " WHERE CtyCod='%03ld'"
                  " AND CtyCod NOT IN (SELECT DISTINCT CtyCod FROM institutions))",
            SubQueryNam1,SubQueryWWW1,Cty->CtyCod,
            SubQueryNam2,SubQueryWWW2,Cty->CtyCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get data of a country");

   /***** Count number of rows in result *****/
   if (NumRows) // Country found...
     {
      CtyFound = true;

      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get Alpha-2 country code (row[0]) */
      strncpy (Cty->Alpha2,row[0],2);
      Cty->Alpha2[2] = '\0';

      /* Get the name of the country in several languages */
      for (Lan = (Txt_Language_t) 0;
	   Lan < Txt_NUM_LANGUAGES;
	   Lan++)
	{
         strcpy (Cty->Name[Lan],row[1+Lan]);
         strcpy (Cty->WWW[Lan],row[1+Txt_NUM_LANGUAGES+Lan]);
	}

      /* Get number of institutions in this country */
      if (sscanf (row[1+Txt_NUM_LANGUAGES*2],"%u",&(Cty->NumInss)) != 1)
         Cty->NumInss = 0;
     }
   else
      CtyFound = false;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return CtyFound;
  }

/*****************************************************************************/
/***************************** Get country name ******************************/
/*****************************************************************************/

void Cty_GetCountryName (long CtyCod,char *CtyName)
  {
   extern const char *Txt_Another_country;
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Check if country code is correct *****/
   if (CtyCod <= 0)
      strcpy (CtyName,Txt_Another_country);
   else
     {
      /***** Get name of the country from database *****/
      sprintf (Query,"SELECT Name_%s"
                     " FROM countries"
                     " WHERE CtyCod='%03ld'",
               Txt_STR_LANG_ID[Gbl.Prefs.Language],CtyCod);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the name of a country");

      /***** Count number of rows in result *****/
      if (NumRows) // Country found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get the name of the country */
         strcpy (CtyName,row[0]);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/******************** Get map attribution from database **********************/
/*****************************************************************************/

static void Cty_GetMapAttribution (long CtyCod,char **MapAttribution)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Free possible former map attribution *****/
   Cty_FreeMapAttribution (MapAttribution);

   /***** Get photo attribution from database *****/
   sprintf (Query,"SELECT MapAttribution FROM countries WHERE CtyCod='%ld'",
	    CtyCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get photo attribution"))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get the attribution of the map of the country (row[0]) */
      if (row[0])
	 if (row[0][0])
	   {
	    if (((*MapAttribution) = (char *) malloc (strlen (row[0])+1)) == NULL)
	       Lay_ShowErrorAndExit ("Error allocating memory for map attribution.");
	    strcpy (*MapAttribution,row[0]);
	   }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Free memory used for map attribution ********************/
/*****************************************************************************/

static void Cty_FreeMapAttribution (char **MapAttribution)
  {
   if (*MapAttribution)
     {
      free ((void *) *MapAttribution);
      *MapAttribution = NULL;
     }
  }

/*****************************************************************************/
/*************************** Free list of countries **************************/
/*****************************************************************************/

void Cty_FreeListCountries (void)
  {
   if (Gbl.Ctys.Lst)
     {
      /***** Free memory used by the list of courses in institution *****/
      free ((void *) Gbl.Ctys.Lst);
      Gbl.Ctys.Lst = NULL;
      Gbl.Ctys.Num = 0;
     }
  }

/*****************************************************************************/
/*************************** List all the countries **************************/
/*****************************************************************************/

static void Cty_ListCountriesForEdition (void)
  {
   extern const char *Txt_Countries;
   extern const char *Txt_STR_LANG_NAME[Txt_NUM_LANGUAGES];
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Txt_Remove_country;
   unsigned NumCty;
   struct Country *Cty;
   Txt_Language_t Lan;

   Lay_StartRoundFrameTable10 (NULL,2,Txt_Countries);

   /***** Table head *****/
   Cty_PutHeadCountries ();

   /***** Write all the countries *****/
   for (NumCty = 0;
	NumCty < Gbl.Ctys.Num;
	NumCty++)
     {
      Cty = &Gbl.Ctys.Lst[NumCty];

      /* Put icon to remove country */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td rowspan=\"%u\" class=\"BT\">",
	       1 + Txt_NUM_LANGUAGES);
      if (Cty->NumInss ||
	  Cty->NumUsrs)	// Country has institutions or users ==> deletion forbidden
         fprintf (Gbl.F.Out,"<img src=\"%s/deloff16x16.gif\""
                            " alt=\"\" class=\"ICON16x16\" />",
                  Gbl.Prefs.IconsURL);
      else
        {
         Act_FormStart (ActRemCty);
         Cty_PutParamOtherCtyCod (Cty->CtyCod);
         fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/delon16x16.gif\""
                            " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />",
                  Gbl.Prefs.IconsURL,
                  Txt_Remove_country,
                  Txt_Remove_country);
         Act_FormEnd ();
        }
      fprintf (Gbl.F.Out,"</td>");

      /* Numerical country code (ISO 3166-1) */
      fprintf (Gbl.F.Out,"<td rowspan=\"%u\" class=\"DAT\""
	                 " style=\"text-align:right; vertical-align:top;\">"
	                 "%03ld"
	                 "</td>",
               1 + Txt_NUM_LANGUAGES,Cty->CtyCod);

      /* Alphabetic country code with 2 letters (ISO 3166-1) */
      fprintf (Gbl.F.Out,"<td rowspan=\"%u\" class=\"DAT\""
	                 " style=\"text-align:right; vertical-align:top;\">"
	                 "%s"
	                 "</td>",
               1 + Txt_NUM_LANGUAGES,Cty->Alpha2);

      fprintf (Gbl.F.Out,"<td></td>"
	                 "<td></td>"
	                 "<td></td>");

      /* Number of users */
      fprintf (Gbl.F.Out,"<td rowspan=\"%u\" class=\"DAT\""
	                 " style=\"text-align:right; vertical-align:top;\">"
	                 "%u"
	                 "</td>",
               1 + Txt_NUM_LANGUAGES,Cty->NumUsrs);

      /* Number of institutions */
      fprintf (Gbl.F.Out,"<td rowspan=\"%u\" class=\"DAT\""
	                 " style=\"text-align:right; vertical-align:top;\">"
	                 "%u"
	                 "</td>"
	                 "</tr>",
               1 + Txt_NUM_LANGUAGES,Cty->NumInss);

      /* Country name in several languages */
      for (Lan = (Txt_Language_t) 0;
	   Lan < Txt_NUM_LANGUAGES;
	   Lan++)
        {
	 /* Language */
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"DAT\" style=\"text-align:right;"
                            " vertical-align:middle;\">"
                            "%s:"
                            "</td>",
                  Txt_STR_LANG_NAME[Lan]);

         /* Name */
         fprintf (Gbl.F.Out,"<td style=\"text-align:left;"
                            " vertical-align:top;\">");
         Act_FormStart (ActRenCty);
         Cty_PutParamOtherCtyCod (Cty->CtyCod);
         Par_PutHiddenParamUnsigned ("Lan",(unsigned) Lan);
         fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Name\" size=\"15\" maxlength=\"%u\" value=\"%s\""
                            " onchange=\"javascript:document.getElementById('%s').submit();\" />",
                  Cty_MAX_LENGTH_COUNTRY_NAME,
                  Cty->Name[Lan],Gbl.FormId);
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</td>");

         /* WWW */
         fprintf (Gbl.F.Out,"<td style=\"text-align:left;"
                            " vertical-align:top;\">");
         Act_FormStart (ActChgCtyWWW);
         Cty_PutParamOtherCtyCod (Cty->CtyCod);
         Par_PutHiddenParamUnsigned ("Lan",(unsigned) Lan);
         fprintf (Gbl.F.Out,"<input type=\"text\" name=\"WWW\" size=\"15\" maxlength=\"%u\" value=\"%s\""
                            " onchange=\"javascript:document.getElementById('%s').submit();\" />",
                  Cty_MAX_LENGTH_COUNTRY_WWW,
                  Cty->WWW[Lan],Gbl.FormId);
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");
        }
     }

   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/******************** Write parameter with code of country *******************/
/*****************************************************************************/

void Cty_PutParamCtyCod (long CtyCod)
  {
   Par_PutHiddenParamLong ("cty",CtyCod);
  }

/*****************************************************************************/
/******************** Write parameter with code of country *******************/
/*****************************************************************************/

static void Cty_PutParamOtherCtyCod (long CtyCod)
  {
   Par_PutHiddenParamLong ("OthCtyCod",CtyCod);
  }

/*****************************************************************************/
/******************* Get parameter with code of country **********************/
/*****************************************************************************/

long Cty_GetParamOtherCtyCod (void)
  {
   char LongStr[1+10+1];

   /***** Get parameter with code of country *****/
   Par_GetParToText ("OthCtyCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/****************************** Remove a country *****************************/
/*****************************************************************************/

void Cty_RemoveCountry (void)
  {
   extern const char *Txt_You_can_not_remove_a_country_with_institutions_or_users;
   extern const char *Txt_Country_X_removed;
   char Query[512];
   struct Country Cty;

   /***** Get country code *****/
   if ((Cty.CtyCod = Cty_GetParamOtherCtyCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of country is missing.");

   /***** Get data of the country from database *****/
   Cty_GetDataOfCountryByCod (&Cty);

   /***** Check if this country has users *****/
   if (Cty.NumInss || Cty.NumUsrs)	// Country has institutions or users ==> don't remove
      Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_remove_a_country_with_institutions_or_users);
   else	// Country has no users ==> remove it
     {
      /***** Remove country *****/
      sprintf (Query,"DELETE FROM countries WHERE CtyCod='%03ld'",
               Cty.CtyCod);
      DB_QueryDELETE (Query,"can not remove a country");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Message,Txt_Country_X_removed,
               Cty.Name);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Show the form again *****/
   Cty_EditCountries ();
  }

/*****************************************************************************/
/************************ Change the name of a country ***********************/
/*****************************************************************************/

void Cty_RenameCountry (void)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_country_X_empty;
   extern const char *Txt_The_country_X_already_exists;
   extern const char *Txt_The_country_X_has_been_renamed_as_Y;
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Txt_The_name_of_the_country_X_has_not_changed;
   char Query[512];
   struct Country *Cty;
   char NewCtyName[Cty_MAX_LENGTH_COUNTRY_NAME+1];
   Txt_Language_t Language;

   Cty = &Gbl.Ctys.EditingCty;

   /***** Get parameters from form *****/
   /* Get the code of the country */
   if ((Cty->CtyCod = Cty_GetParamOtherCtyCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of country is missing.");

   /* Get the lenguage */
   Language = Pre_GetParamLanguage ();

   /* Get the new name for the country */
   Par_GetParToText ("Name",NewCtyName,Cty_MAX_LENGTH_COUNTRY_NAME);

   /***** Get from the database the data of the country *****/
   Cty_GetDataOfCountryByCod (Cty);

   /***** Check if new name is empty *****/
   if (!NewCtyName[0])
     {
      sprintf (Gbl.Message,Txt_You_can_not_leave_the_name_of_the_country_X_empty,
               Cty->Name[Language]);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
   else
     {
      /***** Check if old and new names are the same (this happens when user press enter with no changes in the form) *****/
      if (strcmp (Cty->Name[Language],NewCtyName))	// Different names
        {
         /***** If country was in database... *****/
         if (Cty_CheckIfCountryNameExists (Language,NewCtyName,Cty->CtyCod))
           {
            sprintf (Gbl.Message,Txt_The_country_X_already_exists,
                     NewCtyName);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else
           {
            /* Update the table changing old name by new name */
            sprintf (Query,"UPDATE countries SET Name_%s='%s'"
                           " WHERE CtyCod='%03ld'",
                     Txt_STR_LANG_ID[Language],NewCtyName,Cty->CtyCod);
            DB_QueryUPDATE (Query,"can not update the name of a country");

            /***** Write message to show the change made *****/
            sprintf (Gbl.Message,Txt_The_country_X_has_been_renamed_as_Y,
                     Cty->Name[Language],NewCtyName);
            Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
           }
        }
      else	// The same name
        {
         sprintf (Gbl.Message,Txt_The_name_of_the_country_X_has_not_changed,
                  Cty->Name[Language]);
         Lay_ShowAlert (Lay_INFO,Gbl.Message);
        }
     }

   /***** Show the form again *****/
   strcpy (Cty->Name[Language],NewCtyName);
   Cty_EditCountries ();
  }

/*****************************************************************************/
/******************* Check if a numeric country code exists ******************/
/*****************************************************************************/

static bool Cty_CheckIfNumericCountryCodeExists (long CtyCod)
  {
   char Query[256];

   /***** Get number of countries with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM countries WHERE CtyCod='%03ld'",
            CtyCod);
   return (DB_QueryCOUNT (Query,"can not check if the numeric code of a country already existed") != 0);
  }

/*****************************************************************************/
/*************** Check if an alphabetic country code exists ******************/
/*****************************************************************************/

static bool Cty_CheckIfAlpha2CountryCodeExists (const char Alpha2[2+1])
  {
   char Query[512];

   /***** Get number of countries with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM countries WHERE Alpha2='%s'",
            Alpha2);
   return (DB_QueryCOUNT (Query,"can not check if the alphabetic code of a country already existed") != 0);
  }

/*****************************************************************************/
/******************** Check if the name of country exists ********************/
/*****************************************************************************/

static bool Cty_CheckIfCountryNameExists (Txt_Language_t Language,const char *Name,long CtyCod)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   char Query[512];

   /***** Get number of countries with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM countries WHERE Name_%s='%s' AND CtyCod<>'%03ld'",
            Txt_STR_LANG_ID[Language],Name,CtyCod);
   return (DB_QueryCOUNT (Query,"can not check if the name of a country already existed") != 0);
  }

/*****************************************************************************/
/************************ Change the URL of a country ************************/
/*****************************************************************************/

void Cty_ChangeCtyWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   char Query[256+Cty_MAX_LENGTH_COUNTRY_WWW];
   struct Country *Cty;
   char NewWWW[Cty_MAX_LENGTH_COUNTRY_WWW+1];
   Txt_Language_t Language;

   Cty = &Gbl.Ctys.EditingCty;

   /***** Get parameters from form *****/
   /* Get the code of the country */
   if ((Cty->CtyCod = Cty_GetParamOtherCtyCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of country is missing.");

   /* Get the lenguage */
   Language = Pre_GetParamLanguage ();

   /* Get the new WWW for the country */
   Par_GetParToText ("WWW",NewWWW,Cty_MAX_LENGTH_COUNTRY_NAME);

   /***** Get from the database the data of the country *****/
   Cty_GetDataOfCountryByCod (Cty);

   /***** Update the table changing old WWW by new WWW *****/
   sprintf (Query,"UPDATE countries SET WWW_%s='%s'"
		  " WHERE CtyCod='%03ld'",
	    Txt_STR_LANG_ID[Language],NewWWW,Cty->CtyCod);
   DB_QueryUPDATE (Query,"can not update the web of a country");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_The_new_web_address_is_X,NewWWW);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the form again *****/
   strcpy (Cty->WWW[Language],NewWWW);
   Cty_EditCountries ();
  }

/*****************************************************************************/
/*********** Change the attribution of the map of current country ************/
/*****************************************************************************/

void Cty_ChangeCtyMapAttribution (void)
  {
   char Query[256+Cty_MAX_LENGTH_MAP_ATTRIBUTION];
   char NewMapAttribution[Cty_MAX_LENGTH_MAP_ATTRIBUTION+1];

   /***** Get parameters from form *****/
   /* Get the new map attribution for the country */
   Par_GetParToText ("Attribution",NewMapAttribution,Cty_MAX_LENGTH_MAP_ATTRIBUTION);

   /***** Update the table changing old attribution by new attribution *****/
   sprintf (Query,"UPDATE countries SET MapAttribution='%s'"
		  " WHERE CtyCod='%03ld'",
	    NewMapAttribution,Gbl.CurrentCty.Cty.CtyCod);
   DB_QueryUPDATE (Query,"can not update the map attribution of a country");

   /***** Show the country information again *****/
   Cty_ShowConfiguration ();
  }

/*****************************************************************************/
/********************* Put a form to create a new country ********************/
/*****************************************************************************/

static void Cty_PutFormToCreateCountry (void)
  {
   extern const char *Txt_New_country;
   extern const char *Txt_STR_LANG_NAME[Txt_NUM_LANGUAGES];
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Txt_Create_country;
   struct Country *Cty;
   Txt_Language_t Lan;

   Cty = &Gbl.Ctys.EditingCty;

   /***** Start form *****/
   Act_FormStart (ActNewCty);

   /***** Start of frame *****/
   Lay_StartRoundFrameTable10 (NULL,2,Txt_New_country);

   /***** Write heading *****/
   Cty_PutHeadCountries ();

   /***** Firts columns for CtyCod *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td rowspan=\"%u\" class=\"BT\"></td>",
            1 + Txt_NUM_LANGUAGES);

   /***** Numerical country code (ISO 3166-1) *****/
   fprintf (Gbl.F.Out,"<td rowspan=\"%u\""
	              " style=\"text-align:right; vertical-align:top;\">"
                      "<input type=\"text\" name=\"OthCtyCod\" size=\"3\" maxlength=\"10\" value=\"",
            1 + Txt_NUM_LANGUAGES);
   if (Cty->CtyCod > 0)
      fprintf (Gbl.F.Out,"%03ld",Cty->CtyCod);
   fprintf (Gbl.F.Out,"\" /></td>");

   /***** Alphabetic country code with 2 letters (ISO 3166-1) *****/
   fprintf (Gbl.F.Out,"<td rowspan=\"%u\""
	              " style=\"text-align:right; vertical-align:top;\">"
                      "<input type=\"text\" name=\"Alpha2\" size=\"2\" maxlength=\"2\" value=\"%s\" />"
                      "</td>",
            1 + Txt_NUM_LANGUAGES,Cty->Alpha2);

   fprintf (Gbl.F.Out,"<td></td>"
		      "<td></td>"
		      "<td></td>");

   /* Number of users */
   fprintf (Gbl.F.Out,"<td rowspan=\"%u\" class=\"DAT\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "0"
	              "</td>",
	    1 + Txt_NUM_LANGUAGES);

   /* Number of institutions */
   fprintf (Gbl.F.Out,"<td rowspan=\"%u\" class=\"DAT\""
	              " style=\"text-align:right; vertical-align:top;\">"
	              "0"
	              "</td>"
		      "</tr>",
	    1 + Txt_NUM_LANGUAGES);

   /***** Country name in several languages *****/
   for (Lan = (Txt_Language_t) 0;
	Lan < Txt_NUM_LANGUAGES;
	Lan++)
     {
      /* Language */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"DAT\""
	                 " style=\"text-align:right; vertical-align:top;\">"
	                 "%s"
	                 "</td>",
               Txt_STR_LANG_NAME[Lan]);

      /* Name */
      fprintf (Gbl.F.Out,"<td style=\"text-align:left;"
	                 " vertical-align:middle;\">"
                         "<input type=\"text\" name=\"Name_%s\" size=\"15\" maxlength=\"%u\" value=\"%s\" />"
                         "</td>",
               Txt_STR_LANG_ID[Lan],Cty_MAX_LENGTH_COUNTRY_NAME,Cty->Name[Lan]);

      /* WWW */
      fprintf (Gbl.F.Out,"<td style=\"text-align:left;"
	                 " vertical-align:middle;\">"
                         "<input type=\"text\" name=\"WWW_%s\" size=\"15\" maxlength=\"%u\" value=\"%s\" />"
			 "</td>"
			 "</tr>",
	       Txt_STR_LANG_ID[Lan],Cty_MAX_LENGTH_COUNTRY_WWW,Cty->WWW[Lan]);
     }

   /***** Send button *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"8\" style=\"text-align:center;\">");
   Lay_PutCreateButton (Txt_Create_country);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** End of frame *****/
   Lay_EndRoundFrameTable10 ();

   /***** End of form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************* Write header with fields of a country *******************/
/*****************************************************************************/

static void Cty_PutHeadCountries (void)
  {
   extern const char *Txt_Numeric_BR_code_BR_ISO_3166_1;
   extern const char *Txt_Alphabetic_BR_code_BR_ISO_3166_1;
   extern const char *Txt_Name;
   extern const char *Txt_WWW;
   extern const char *Txt_Users;
   extern const char *Txt_Institutions_ABBREVIATION;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"BM\"></th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:right;\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:right;\">"
                      "%s"
                      "</th>"
                      "<th></th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:right;\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:right;\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Numeric_BR_code_BR_ISO_3166_1,
            Txt_Alphabetic_BR_code_BR_ISO_3166_1,
            Txt_Name,
            Txt_WWW,
            Txt_Users,
            Txt_Institutions_ABBREVIATION);
  }

/*****************************************************************************/
/******************* Receive form to create a new country ********************/
/*****************************************************************************/

void Cty_RecFormNewCountry (void)
  {
   extern const char *Txt_You_must_specify_the_numerical_code_of_the_new_country;
   extern const char *Txt_The_numerical_code_X_already_exists;
   extern const char *Txt_The_alphabetical_code_X_is_not_correct;
   extern const char *Txt_The_alphabetical_code_X_already_exists;
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Txt_The_country_X_already_exists;
   extern const char *Txt_You_must_specify_the_name_of_the_new_country_in_all_languages;
   char ParamName[32];
   struct Country *Cty;
   bool CreateCountry = true;
   Txt_Language_t Lan;
   unsigned i;

   Cty = &Gbl.Ctys.EditingCty;

   /***** Get parameters from form *****/
   /* Get numeric country code */
   if ((Cty->CtyCod = Cty_GetParamOtherCtyCod ()) < 0)
     {
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_numerical_code_of_the_new_country);
      CreateCountry = false;
     }
   else if (Cty_CheckIfNumericCountryCodeExists (Cty->CtyCod))
     {
      sprintf (Gbl.Message,Txt_The_numerical_code_X_already_exists,
               Cty->CtyCod);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
      CreateCountry = false;
     }
   else	// Numeric code correct
     {
      /* Get alphabetic-2 country code */
      Par_GetParToText ("Alpha2",Cty->Alpha2,2);
      Str_ConvertToUpperText (Cty->Alpha2);
      for (i = 0;
	   i < 2 && CreateCountry;
	   i++)
         if (Cty->Alpha2[i] < 'A' ||
             Cty->Alpha2[i] > 'Z')
           {
            sprintf (Gbl.Message,Txt_The_alphabetical_code_X_is_not_correct,
                     Cty->Alpha2);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
            CreateCountry = false;
           }
      if (CreateCountry)
        {
         if (Cty_CheckIfAlpha2CountryCodeExists (Cty->Alpha2))
           {
            sprintf (Gbl.Message,Txt_The_alphabetical_code_X_already_exists,
                     Cty->Alpha2);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
            CreateCountry = false;
           }
         else	// Alphabetic code correct
           {
            /* Get country name and WWW in different languages */
            for (Lan = (Txt_Language_t) 0;
        	 Lan < Txt_NUM_LANGUAGES;
        	 Lan++)
              {
               sprintf (ParamName,"Name_%s",Txt_STR_LANG_ID[Lan]);
               Par_GetParToText (ParamName,Cty->Name[Lan],Cty_MAX_LENGTH_COUNTRY_NAME);

               if (Cty->Name[Lan][0])	// If there's a country name
                 {
                  /***** If name of country was in database... *****/
                  if (Cty_CheckIfCountryNameExists (Lan,Cty->Name[Lan],-1L))
                    {
                     sprintf (Gbl.Message,Txt_The_country_X_already_exists,
                              Cty->Name[Lan]);
                     Lay_ShowAlert (Lay_WARNING,Gbl.Message);
                     CreateCountry = false;
                     break;
                    }
                 }
               else	// If there is not a country name
                 {
                  Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_name_of_the_new_country_in_all_languages);
                  CreateCountry = false;
                  break;
                 }

               sprintf (ParamName,"WWW_%s",Txt_STR_LANG_ID[Lan]);
               Par_GetParToText (ParamName,Cty->WWW[Lan],Cty_MAX_LENGTH_COUNTRY_WWW);
              }
           }
        }
     }

   if (CreateCountry)
      Cty_CreateCountry (Cty);	// Add new country to database

   /***** Show the form again *****/
   Cty_EditCountries ();
  }

/*****************************************************************************/
/**************************** Create a new country ***************************/
/*****************************************************************************/

static void Cty_CreateCountry (struct Country *Cty)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Txt_Created_new_country_X;
   Txt_Language_t Lan;
   char StrField[32];
   char SubQueryNam1[Txt_NUM_LANGUAGES*32];
   char SubQueryNam2[Txt_NUM_LANGUAGES*Cty_MAX_LENGTH_COUNTRY_NAME];
   char SubQueryWWW1[Txt_NUM_LANGUAGES*32];
   char SubQueryWWW2[Txt_NUM_LANGUAGES*Cty_MAX_LENGTH_COUNTRY_WWW];
   char Query[1024+Txt_NUM_LANGUAGES*(32+Cty_MAX_LENGTH_COUNTRY_NAME+32+Cty_MAX_LENGTH_COUNTRY_WWW)];

   /***** Create a new country *****/
   SubQueryNam1[0] = '\0';
   SubQueryNam2[0] = '\0';
   SubQueryWWW1[0] = '\0';
   SubQueryWWW2[0] = '\0';
   for (Lan = (Txt_Language_t) 0;
	Lan < Txt_NUM_LANGUAGES;
	Lan++)
     {
      sprintf (StrField,",Name_%s",Txt_STR_LANG_ID[Lan]);
      strcat (SubQueryNam1,StrField);

      strcat (SubQueryNam2,",'");
      strcat (SubQueryNam2,Cty->Name[Lan]);
      strcat (SubQueryNam2,"'");

      sprintf (StrField,",WWW_%s",Txt_STR_LANG_ID[Lan]);
      strcat (SubQueryWWW1,StrField);

      strcat (SubQueryWWW2,",'");
      strcat (SubQueryWWW2,Cty->WWW[Lan]);
      strcat (SubQueryWWW2,"'");
     }
   sprintf (Query,"INSERT INTO countries (CtyCod,Alpha2%s%s)"
	          " VALUES ('%03ld','%s'%s%s)",
            SubQueryNam1,SubQueryWWW1,
            Cty->CtyCod,Cty->Alpha2,SubQueryNam2,SubQueryWWW2);
   DB_QueryINSERT (Query,"can not create country");

   /***** Write success message *****/
   sprintf (Gbl.Message,Txt_Created_new_country_X,
            Cty->Name);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/*********************** Get total number of countries ***********************/
/*****************************************************************************/

unsigned Cty_GetNumCtysTotal (void)
  {
   char Query[256];

   /***** Get total number of degrees from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM countries");
   return (unsigned) DB_QueryCOUNT (Query,"can not get the total number of countries");
  }

/*****************************************************************************/
/***************** Get number of countries with institutions *****************/
/*****************************************************************************/

unsigned Cty_GetNumCtysWithInss (const char *SubQuery)
  {
   char Query[512];

   /***** Get number of countries with institutions from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT countries.CtyCod)"
                  " FROM countries,institutions"
                  " WHERE %scountries.CtyCod=institutions.CtyCod",
            SubQuery);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of countries with institutions");
  }

/*****************************************************************************/
/******************* Get number of countries with centres ********************/
/*****************************************************************************/

unsigned Cty_GetNumCtysWithCtrs (const char *SubQuery)
  {
   char Query[512];

   /***** Get number of countries with centres from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT countries.CtyCod)"
                  " FROM countries,institutions,centres"
                  " WHERE %scountries.CtyCod=institutions.CtyCod"
                  " AND institutions.InsCod=centres.InsCod",
            SubQuery);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of countries with centres");
  }

/*****************************************************************************/
/******************* Get number of countries with degrees ********************/
/*****************************************************************************/

unsigned Cty_GetNumCtysWithDegs (const char *SubQuery)
  {
   char Query[512];

   /***** Get number of countries with degrees from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT countries.CtyCod)"
                  " FROM countries,institutions,centres,degrees"
                  " WHERE %scountries.CtyCod=institutions.CtyCod"
                  " AND institutions.InsCod=centres.InsCod"
                  " AND centres.CtrCod=degrees.CtrCod",
            SubQuery);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of countries with degrees");
  }

/*****************************************************************************/
/******************* Get number of countries with courses ********************/
/*****************************************************************************/

unsigned Cty_GetNumCtysWithCrss (const char *SubQuery)
  {
   char Query[512];

   /***** Get number of countries with courses from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT countries.CtyCod)"
                  " FROM countries,institutions,centres,degrees,courses"
                  " WHERE %scountries.CtyCod=institutions.CtyCod"
                  " AND institutions.InsCod=centres.InsCod"
                  " AND centres.CtrCod=degrees.CtrCod"
                  " AND degrees.DegCod=courses.DegCod",
            SubQuery);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of countries with courses");
  }

/*****************************************************************************/
/******************* Get number of countries with users **********************/
/*****************************************************************************/

unsigned Cty_GetNumCtysWithUsrs (Rol_Role_t Role,const char *SubQuery)
  {
   char Query[512];

   /***** Get number of countries with users from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT countries.CtyCod)"
                  " FROM countries,institutions,centres,degrees,courses,crs_usr"
                  " WHERE %scountries.CtyCod=institutions.CtyCod"
                  " AND institutions.InsCod=centres.InsCod"
                  " AND centres.CtrCod=degrees.CtrCod"
                  " AND degrees.DegCod=courses.DegCod"
                  " AND courses.CrsCod=crs_usr.CrsCod"
                  " AND crs_usr.Role='%u'",
            SubQuery,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of countries with users");
  }
