// swad_search.c: search for courses and teachers

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <stdio.h>	// For fprintf...
#include <string.h>	// For string functions...

#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_layout.h"
#include "swad_parameter.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal constants ****************************/
/*****************************************************************************/

#define Sch_MIN_LENGTH_LONGEST_WORD	  3
#define Sch_MIN_LENGTH_TOTAL		  3	// "A An" is not valid; "A An Ann" is valid

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

static void Sch_PutFormToSearchWithWhatToSearchAndScope (Act_Action_t Action,Hie_Level_t DefaultScope);
static bool Sch_CheckIfIHavePermissionToSearch (Sch_WhatToSearch_t WhatToSearch);
static void Sch_GetParamSearch (void);
static void Sch_SearchInDB (void);
static unsigned Sch_SearchCountriesInDB (const char *RangeQuery);
static unsigned Sch_SearchInstitutionsInDB (const char *RangeQuery);
static unsigned Sch_SearchCentresInDB (const char *RangeQuery);
static unsigned Sch_SearchDegreesInDB (const char *RangeQuery);
static unsigned Sch_SearchCoursesInDB (const char *RangeQuery);
static unsigned Sch_SearchUsrsInDB (Rol_Role_t Role);
static unsigned Sch_SearchOpenDocumentsInDB (const char *RangeQuery);
static unsigned Sch_SearchDocumentsInMyCoursesInDB (const char *RangeQuery);
static unsigned Sch_SearchMyDocumentsInDB (const char *RangeQuery);

static void Sch_SaveLastSearchIntoSession (void);

/*****************************************************************************/
/*********************** Request search in system tab ************************/
/*****************************************************************************/

void Sch_ReqSysSearch (void)
  {
   /***** Search courses, teachers, documents... *****/
   Sch_GetParamWhatToSearch ();
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActSch,Hie_SYS);
  }

/*****************************************************************************/
/****************** Put a form to search, including scope ********************/
/*****************************************************************************/

static void Sch_PutFormToSearchWithWhatToSearchAndScope (Act_Action_t Action,Hie_Level_t DefaultScope)
  {
   extern const char *Hlp_START_Search;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Scope;
   extern const char *Txt_SEARCH_X_in_Y;
   extern const char *Txt_all;
   extern const char *Txt_countries;
   extern const char *Txt_institutions;
   extern const char *Txt_centres;
   extern const char *Txt_degrees;
   extern const char *Txt_courses;
   extern const char *Txt_users[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_my_documents;
   extern const char *Txt_documents_in_my_courses;
   extern const char *Txt_open_documents;
   extern const char *Txt_Search;
   const char *Titles[Sch_NUM_WHAT_TO_SEARCH] =
     {
	"",						// Sch_SEARCH_UNKNOWN
	Txt_all,					// Sch_SEARCH_ALL
	Txt_countries,					// Sch_SEARCH_COUNTRIES
	Txt_institutions,				// Sch_SEARCH_INSTITS
	Txt_centres,					// Sch_SEARCH_CENTRES
	Txt_degrees,					// Sch_SEARCH_DEGREES
	Txt_courses,					// Sch_SEARCH_COURSES
	Txt_users[Usr_SEX_UNKNOWN],			// Sch_SEARCH_USERS
	Txt_ROLES_PLURAL_abc[Rol_TCH][Usr_SEX_UNKNOWN],	// Sch_SEARCH_TEACHERS
	Txt_ROLES_PLURAL_abc[Rol_STD][Usr_SEX_UNKNOWN],	// Sch_SEARCH_STUDENTS
        Txt_ROLES_PLURAL_abc[Rol_GST][Usr_SEX_UNKNOWN],	// Sch_SEARCH_GUESTS
	Txt_open_documents,				// Sch_SEARCH_OPEN_DOCUMENTS
	Txt_documents_in_my_courses,			// Sch_SEARCH_DOCUM_IN_MY_COURSES
	Txt_my_documents,				// Sch_SEARCH_MY_DOCUMENTS
     };
   Sch_WhatToSearch_t WhatToSearch;

   /***** Start box *****/
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
   Box_StartBox (NULL,Txt_Search,Crs_PutIconToSelectMyCourses,
                 Hlp_START_Search,Box_NOT_CLOSABLE);

   /***** Start form *****/
   Frm_StartForm (Action);

   /***** Scope (whole platform, current country, current institution,
                 current centre, current degree or current course) *****/
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">"
	              "<label class=\"%s\">%s:&nbsp;",
            The_ClassFormInBox[Gbl.Prefs.Theme],Txt_Scope);
   Gbl.Scope.Allowed = 1 << Hie_SYS |
	               1 << Hie_CTY |
		       1 << Hie_INS |
		       1 << Hie_CTR |
		       1 << Hie_DEG |
		       1 << Hie_CRS;
   Gbl.Scope.Default = DefaultScope;
   Sco_GetScope ("ScopeSch");
   Sco_PutSelectorScope ("ScopeSch",false);
   fprintf (Gbl.F.Out,"</label>"
	              "</div>");

   /***** String to find *****/
   Sch_PutInputStringToSearch (NULL);

   /***** What to search? *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">"
	              " %s&nbsp;",
            The_ClassFormInBox[Gbl.Prefs.Theme],Txt_SEARCH_X_in_Y);
   fprintf (Gbl.F.Out,"<select name=\"WhatToSearch\" style=\"width:186px;\">");
   for (WhatToSearch = (Sch_WhatToSearch_t) 0;
        WhatToSearch < Sch_NUM_WHAT_TO_SEARCH;
        WhatToSearch++)
      if (Sch_CheckIfIHavePermissionToSearch (WhatToSearch))
	{
	 fprintf (Gbl.F.Out,"<option value=\"%u\"",
		  (unsigned) WhatToSearch);
	 if (Gbl.Search.WhatToSearch == WhatToSearch)
	    fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%s</option>",
		  Titles[WhatToSearch]);
        }
   fprintf (Gbl.F.Out,"</select>"
	              "</label>");

   /***** Magnifying glass icon *****/
   Sch_PutMagnifyingGlassButton ("search.svg");

   /***** Send button *****/
   Btn_PutButton (Btn_CONFIRM_BUTTON,Txt_Search);

   /***** End form *****/
   Frm_EndForm ();

   /***** End box *****/
   Box_EndBox ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************* Check if I have permission to execute an action ***************/
/*****************************************************************************/

static bool Sch_CheckIfIHavePermissionToSearch (Sch_WhatToSearch_t WhatToSearch)
  {
   unsigned Permissions[Sch_NUM_WHAT_TO_SEARCH] =
     {
      0x000,	// Sch_SEARCH_UNKNOWN
      0x3FF,	// Sch_SEARCH_ALL
      0x3FF,	// Sch_SEARCH_COUNTRIES
      0x3FF,	// Sch_SEARCH_INSTITS
      0x3FF,	// Sch_SEARCH_CENTRES
      0x3FF,	// Sch_SEARCH_DEGREES
      0x3FF,	// Sch_SEARCH_COURSES
      0x3FF,	// Sch_SEARCH_USERS
      0x3FF,	// Sch_SEARCH_TEACHERS
      0x3FF,	// Sch_SEARCH_STUDENTS
      0x3FF,	// Sch_SEARCH_GUESTS
      0x3FF,	// Sch_SEARCH_OPEN_DOCUMENTS
      0x3FE,	// Sch_SEARCH_DOCUM_IN_MY_COURSES	Only if I am logged
      0x3FE,	// Sch_SEARCH_MY_DOCUMENTS		Only if I am logged
     };

   return (Permissions[WhatToSearch] & (1 << Gbl.Usrs.Me.Role.Logged));
  }

/*****************************************************************************/
/**************** Put a form to search in page top heading *******************/
/*****************************************************************************/

void Sch_PutFormToSearchInPageTopHeading (void)
  {
   /***** Put form *****/
   fprintf (Gbl.F.Out,"<div id=\"head_row_1_search\">");
   Frm_StartForm (ActSch);
   Sco_PutParamScope ("ScopeSch",Hie_SYS);
   Sch_PutInputStringToSearch ("head_search_text");
   Sch_PutMagnifyingGlassButton ("search-white.svg");
   Frm_EndForm ();
   fprintf (Gbl.F.Out,"</div>");	// head_row_1_search
  }

/*****************************************************************************/
/********************* Put string to search inside form **********************/
/*****************************************************************************/

void Sch_PutInputStringToSearch (const char *IdInputText)
  {
   extern const char *Txt_Search;

   /***** String to find *****/
   // Input field not required, because it can be hidden (display:none)
   fprintf (Gbl.F.Out,"<input");
   if (IdInputText)
      fprintf (Gbl.F.Out," id=\"%s\"",IdInputText);
   fprintf (Gbl.F.Out," type=\"search\" name=\"Search\""
	              " size=\"18\" maxlength=\"%u\" value=\"%s\"",
	    Sch_MAX_CHARS_STRING_TO_FIND,
            Gbl.Search.Str);
   if (!Gbl.Search.Str[0])
      fprintf (Gbl.F.Out," placeholder=\"%s&hellip;\"",
	       Txt_Search);
   fprintf (Gbl.F.Out," style=\"margin:0;\" />");
  }

/*****************************************************************************/
/******************* Put magnifying glass button inside form *****************/
/*****************************************************************************/

void Sch_PutMagnifyingGlassButton (const char *Icon)
  {
   extern const char *Txt_Search;

   /***** Send button *****/
   Ico_PutIconLink (Icon,Txt_Search);
  }

/*****************************************************************************/
/************* Get parameter "what to search" from search form ***************/
/*****************************************************************************/

void Sch_GetParamWhatToSearch (void)
  {
   Sch_WhatToSearch_t WhatToSearch;

   /***** Get what to search from form *****/
   WhatToSearch = (Sch_WhatToSearch_t)
	          Par_GetParToUnsignedLong ("WhatToSearch",
	                                    0,
	                                    Sch_NUM_WHAT_TO_SEARCH - 1,
	                                    (unsigned long) Sch_SEARCH_UNKNOWN);

   /***** If parameter WhatToSearch is not present,
          use parameter from session *****/
   if (WhatToSearch != Sch_SEARCH_UNKNOWN)
      Gbl.Search.WhatToSearch = WhatToSearch;
  }

/*****************************************************************************/
/*********************** Get string from search form *************************/
/*****************************************************************************/

static void Sch_GetParamSearch (void)
  {
   /***** Get string to search *****/
   Par_GetParToText ("Search",Gbl.Search.Str,Sch_MAX_BYTES_STRING_TO_FIND);
  }

/*****************************************************************************/
/************************* Get parameters to search **************************/
/*****************************************************************************/

void Sch_GetParamsSearch (void)
  {
   /***** What to search? *****/
   Sch_GetParamWhatToSearch ();

   /***** Get search string *****/
   Sch_GetParamSearch ();

   /***** Save my search in order to show it in current session *****/
   if (Gbl.Usrs.Me.Logged)
      Sch_SaveLastSearchIntoSession ();
  }

/*****************************************************************************/
/****************** Search courses, teachers, documents... *******************/
/*****************************************************************************/

void Sch_SysSearch (void)
  {
   if (Gbl.Search.Str[0])
     {
      /***** Show search form again *****/
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActSch,Hie_SYS);

      /***** Show results of search *****/
      Sch_SearchInDB ();
     }
   else
      /***** Show search form and selectors *****/
      Sch_ReqSysSearch ();
  }

/*****************************************************************************/
/**** Search institutions, centres, degrees, courses, teachers, documents ****/
/*****************************************************************************/

static void Sch_SearchInDB (void)
  {
   extern const char *Txt_No_results;
   char RangeQuery[256];
   unsigned NumResults = 0;	// Initialized to avoid warning

   Gbl.Search.LogSearch = true;

   /***** Select courses in all the degrees or in current degree *****/
   switch (Gbl.Scope.Current)
     {
      case Hie_UNK:
	 // Not aplicable
      case Hie_SYS:
         RangeQuery[0] = '\0';
         break;
      case Hie_CTY:
         sprintf (RangeQuery," AND countries.CtyCod=%ld",
                  Gbl.Hierarchy.Cty.CtyCod);
         break;
      case Hie_INS:
         sprintf (RangeQuery," AND institutions.InsCod=%ld",
                  Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         sprintf (RangeQuery," AND centres.CtrCod=%ld",
                  Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         sprintf (RangeQuery," AND degrees.DegCod=%ld",
                  Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         sprintf (RangeQuery," AND courses.CrsCod=%ld",
                  Gbl.Hierarchy.Crs.Crs.CrsCod);
         break;
     }

   if (Gbl.Search.WhatToSearch == Sch_SEARCH_UNKNOWN)
      Gbl.Search.WhatToSearch = Sch_WHAT_TO_SEARCH_DEFAULT;
   switch (Gbl.Search.WhatToSearch)
     {
      case Sch_SEARCH_ALL:
	 NumResults  = Sch_SearchCountriesInDB (RangeQuery);
	 NumResults += Sch_SearchInstitutionsInDB (RangeQuery);
	 NumResults += Sch_SearchCentresInDB (RangeQuery);
	 NumResults += Sch_SearchDegreesInDB (RangeQuery);
	 NumResults += Sch_SearchCoursesInDB (RangeQuery);
	 NumResults += Sch_SearchUsrsInDB (Rol_TCH);	// Non-editing teachers and teachers
	 NumResults += Sch_SearchUsrsInDB (Rol_STD);
	 NumResults += Sch_SearchUsrsInDB (Rol_GST);
	 NumResults += Sch_SearchOpenDocumentsInDB (RangeQuery);
	 NumResults += Sch_SearchDocumentsInMyCoursesInDB (RangeQuery);
	 NumResults += Sch_SearchMyDocumentsInDB (RangeQuery);
	 break;
      case Sch_SEARCH_COUNTRIES:
	 NumResults = Sch_SearchCountriesInDB (RangeQuery);
	 break;
      case Sch_SEARCH_INSTITS:
	 NumResults = Sch_SearchInstitutionsInDB (RangeQuery);
	 break;
      case Sch_SEARCH_CENTRES:
	 NumResults = Sch_SearchCentresInDB (RangeQuery);
	 break;
      case Sch_SEARCH_DEGREES:
	 NumResults = Sch_SearchDegreesInDB (RangeQuery);
	 break;
      case Sch_SEARCH_COURSES:
	 NumResults = Sch_SearchCoursesInDB (RangeQuery);
	 break;
      case Sch_SEARCH_USERS:
	 NumResults = Sch_SearchUsrsInDB (Rol_UNK);	// Here Rol_UNK means any user
	 break;
      case Sch_SEARCH_TEACHERS:
	 NumResults = Sch_SearchUsrsInDB (Rol_TCH);	// Non-editing teachers and teachers
	 break;
      case Sch_SEARCH_STUDENTS:
	 NumResults = Sch_SearchUsrsInDB (Rol_STD);
	 break;
      case Sch_SEARCH_GUESTS:
	 NumResults = Sch_SearchUsrsInDB (Rol_GST);
	 break;
      case Sch_SEARCH_OPEN_DOCUMENTS:
	 NumResults = Sch_SearchOpenDocumentsInDB (RangeQuery);
	 break;
      case Sch_SEARCH_DOCUM_IN_MY_COURSES:
	 NumResults = Sch_SearchDocumentsInMyCoursesInDB (RangeQuery);
	 break;
      case Sch_SEARCH_MY_DOCUMENTS:
	 NumResults = Sch_SearchMyDocumentsInDB (RangeQuery);
	 break;
      default:
	 NumResults = 0;
	 break;
     }

   if (NumResults == 0)
      Ale_ShowAlert (Ale_INFO,Txt_No_results);
  }

/*****************************************************************************/
/************************ Search countries in database ***********************/
/*****************************************************************************/
// Returns number of countries found

static unsigned Sch_SearchCountriesInDB (const char *RangeQuery)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   char FieldName[4+1+2+1];	// Example: Name_en
   MYSQL_RES *mysql_res;
   unsigned NumCtys;

   /***** Check scope *****/
   if (Gbl.Scope.Current != Hie_INS &&
       Gbl.Scope.Current != Hie_CTR &&
       Gbl.Scope.Current != Hie_DEG &&
       Gbl.Scope.Current != Hie_CRS)
      /***** Check user's permission *****/
      if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_COUNTRIES))
	{
	 /***** Split countries string into words *****/
	 snprintf (FieldName,sizeof (FieldName),
	           "Name_%s",
		   Lan_STR_LANG_ID[Gbl.Prefs.Language]);
	 if (Sch_BuildSearchQuery (SearchQuery,FieldName,NULL,NULL))
	   {
	    /***** Query database and list institutions found *****/
	    NumCtys = (unsigned) DB_QuerySELECT (&mysql_res,"can not get countries",
						 "SELECT CtyCod"
						 " FROM countries"
						 " WHERE %s%s"
						 " ORDER BY Name_%s",
						 SearchQuery,RangeQuery,
						 Lan_STR_LANG_ID[Gbl.Prefs.Language]);
	    Cty_ListCtysFound (&mysql_res,NumCtys);
	    return NumCtys;
	   }
	}

   return 0;
  }

/*****************************************************************************/
/********************** Search institutions in database **********************/
/*****************************************************************************/
// Returns number of institutions found

static unsigned Sch_SearchInstitutionsInDB (const char *RangeQuery)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   MYSQL_RES *mysql_res;
   unsigned NumInss;

   /***** Check scope *****/
   if (Gbl.Scope.Current != Hie_CTR &&
       Gbl.Scope.Current != Hie_DEG &&
       Gbl.Scope.Current != Hie_CRS)
      /***** Check user's permission *****/
      if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_INSTITS))
	 /***** Split institutions string into words *****/
	 if (Sch_BuildSearchQuery (SearchQuery,"institutions.FullName",NULL,NULL))
	   {
	    /***** Query database and list institutions found *****/
	    NumInss = (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions",
				                 "SELECT institutions.InsCod"
						 " FROM institutions,countries"
						 " WHERE %s"
						 " AND institutions.CtyCod=countries.CtyCod"
						 "%s"
						 " ORDER BY institutions.FullName,countries.Name_%s",
						 SearchQuery,RangeQuery,
						 Lan_STR_LANG_ID[Gbl.Prefs.Language]);
	    Ins_ListInssFound (&mysql_res,NumInss);
	    return NumInss;
	   }

   return 0;
  }

/*****************************************************************************/
/************************* Search centres in database ************************/
/*****************************************************************************/
// Returns number of centres found

static unsigned Sch_SearchCentresInDB (const char *RangeQuery)
  {
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   MYSQL_RES *mysql_res;
   unsigned NumCtrs;

   /***** Check scope *****/
   if (Gbl.Scope.Current != Hie_DEG &&
       Gbl.Scope.Current != Hie_CRS)
      /***** Check user's permission *****/
      if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_CENTRES))
	 /***** Split centre string into words *****/
	 if (Sch_BuildSearchQuery (SearchQuery,"centres.FullName",NULL,NULL))
	   {
	    /***** Query database and list centres found *****/
	    NumCtrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get centres",
						 "SELECT centres.CtrCod"
						 " FROM centres,institutions,countries"
						 " WHERE %s"
						 " AND centres.InsCod=institutions.InsCod"
						 " AND institutions.CtyCod=countries.CtyCod"
						 "%s"
						 " ORDER BY centres.FullName,institutions.FullName",
						 SearchQuery,RangeQuery);
	    Ctr_ListCtrsFound (&mysql_res,NumCtrs);
	    return NumCtrs;
	   }

   return 0;
  }

/*****************************************************************************/
/************************* Search degrees in database ************************/
/*****************************************************************************/
// Returns number of degrees found

static unsigned Sch_SearchDegreesInDB (const char *RangeQuery)
  {
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   MYSQL_RES *mysql_res;
   unsigned NumDegs;

   /***** Check scope *****/
   if (Gbl.Scope.Current != Hie_CRS)
      /***** Check user's permission *****/
      if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_DEGREES))
	 /***** Split degree string into words *****/
	 if (Sch_BuildSearchQuery (SearchQuery,"degrees.FullName",NULL,NULL))
	   {
	    /***** Query database and list degrees found *****/
	    NumDegs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get degrees",
						 "SELECT degrees.DegCod"
						 " FROM degrees,centres,institutions,countries"
						 " WHERE %s"
						 " AND degrees.CtrCod=centres.CtrCod"
						 " AND centres.InsCod=institutions.InsCod"
						 " AND institutions.CtyCod=countries.CtyCod"
						 "%s"
						 " ORDER BY degrees.FullName,institutions.FullName",
						 SearchQuery,RangeQuery);
	    Deg_ListDegsFound (&mysql_res,NumDegs);
	    return NumDegs;
	   }

   return 0;
  }

/*****************************************************************************/
/************************* Search courses in database ************************/
/*****************************************************************************/
// Returns number of courses found

static unsigned Sch_SearchCoursesInDB (const char *RangeQuery)
  {
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   MYSQL_RES *mysql_res;
   unsigned NumCrss;

   /***** Check user's permission *****/
   if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_COURSES))
      /***** Split course string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,"courses.FullName",NULL,NULL))
	{
	 /***** Query database and list courses found *****/
	 NumCrss = (unsigned) DB_QuerySELECT (&mysql_res,"can not get courses",
					      "SELECT degrees.DegCod,courses.CrsCod,degrees.ShortName,degrees.FullName,"
					      "courses.Year,courses.FullName,centres.ShortName"
					      " FROM courses,degrees,centres,institutions,countries"
					      " WHERE %s"
					      " AND courses.DegCod=degrees.DegCod"
					      " AND degrees.CtrCod=centres.CtrCod"
					      " AND centres.InsCod=institutions.InsCod"
					      " AND institutions.CtyCod=countries.CtyCod"
					      "%s"
					      " ORDER BY courses.FullName,institutions.FullName,degrees.FullName,courses.Year",
					      SearchQuery,RangeQuery);
	 Crs_ListCrssFound (&mysql_res,NumCrss);
	 return NumCrss;
	}

   return 0;
  }

/*****************************************************************************/
/*************************** Search users in database ************************/
/*****************************************************************************/
// Returns number of users found

static unsigned Sch_SearchUsrsInDB (Rol_Role_t Role)
  {
   extern const char *Txt_The_search_text_must_be_longer;
   static bool WarningMessageWritten = false;
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];

   /***** Split user string into words *****/
   if (Sch_BuildSearchQuery (SearchQuery,
			     "CONCAT_WS(' ',FirstName,Surname1,Surname2)",
			     NULL,NULL))
      /***** Query database and list users found *****/
      return Usr_ListUsrsFound (Role,SearchQuery);
   else
      // Too short
      if (!WarningMessageWritten)	// To avoid repetitions
	{
         Ale_ShowAlert (Ale_WARNING,Txt_The_search_text_must_be_longer);
         WarningMessageWritten = true;
	}

   return 0;
  }

/*****************************************************************************/
/********************** Search open documents in database ********************/
/*****************************************************************************/
// Returns number of documents found

static unsigned Sch_SearchOpenDocumentsInDB (const char *RangeQuery)
  {
   extern const char *Txt_open_document;
   extern const char *Txt_open_documents;
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   MYSQL_RES *mysql_res;
   unsigned long NumDocs;

   /***** Check user's permission *****/
   if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_OPEN_DOCUMENTS))
      /***** Split document string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,"SUBSTRING_INDEX(files.Path,'/',-1)",
				"_latin1 "," COLLATE latin1_general_ci"))
	{
	 /***** Query database *****/
	 NumDocs = DB_QuerySELECT (&mysql_res,"can not get files",
				   "SELECT * FROM "
				   "("
				   "SELECT files.FilCod,"	// Institution
				   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
				   "institutions.InsCod,institutions.ShortName AS InsShortName,"
				   "-1 AS CtrCod,'' AS CtrShortName,"
				   "-1 AS DegCod,'' AS DegShortName,"
				   "-1 AS CrsCod,'' AS CrsShortName,"
				   "-1 AS GrpCod"
				   " FROM files,courses,degrees,centres,institutions,countries"
				   " WHERE files.Public='Y' AND %s"
				   " AND files.FileBrowser IN (%u,%u)"
				   " AND files.Cod=institutions.InsCod"
				   " AND institutions.CtyCod=countries.CtyCod"
				   "%s"
				   " UNION "
				   "SELECT files.FilCod,"	// Centre
				   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
				   "institutions.InsCod,institutions.ShortName AS InsShortName,"
				   "centres.CtrCod,centres.ShortName AS CtrShortName,"
				   "-1 AS DegCod,'' AS DegShortName,"
				   "-1 AS CrsCod,'' AS CrsShortName,"
				   "-1 AS GrpCod"
				   " FROM files,courses,degrees,centres,institutions,countries"
				   " WHERE files.Public='Y' AND %s"
				   " AND files.FileBrowser IN (%u,%u)"
				   " AND files.Cod=centres.CtrCod"
				   " AND centres.InsCod=institutions.InsCod"
				   " AND institutions.CtyCod=countries.CtyCod"
				   "%s"
				   " UNION "
				   "SELECT files.FilCod,"	// Degree
				   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
				   "institutions.InsCod,institutions.ShortName AS InsShortName,"
				   "centres.CtrCod,centres.ShortName AS CtrShortName,"
				   "degrees.DegCod,degrees.ShortName AS DegShortName,"
				   "-1,'' AS CrsShortName,"
				   "-1"
				   " FROM files,courses,degrees,centres,institutions,countries"
				   " WHERE files.Public='Y' AND %s"
				   " AND files.FileBrowser IN (%u,%u)"
				   " AND files.Cod=degrees.DegCod"
				   " AND degrees.CtrCod=centres.CtrCod"
				   " AND centres.InsCod=institutions.InsCod"
				   " AND institutions.CtyCod=countries.CtyCod"
				   "%s"
				   " UNION "
				   "SELECT files.FilCod,"	// Course
				   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
				   "institutions.InsCod,institutions.ShortName AS InsShortName,"
				   "centres.CtrCod,centres.ShortName AS CtrShortName,"
				   "degrees.DegCod,degrees.ShortName AS DegShortName,"
				   "courses.CrsCod,courses.ShortName AS CrsShortName,"
				   "-1"
				   " FROM files,courses,degrees,centres,institutions,countries"
				   " WHERE files.Public='Y' AND %s"
				   " AND files.FileBrowser IN (%u,%u)"
				   " AND files.Cod=courses.CrsCod"
				   " AND courses.DegCod=degrees.DegCod"
				   " AND degrees.CtrCod=centres.CtrCod"
				   " AND centres.InsCod=institutions.InsCod"
				   " AND institutions.CtyCod=countries.CtyCod"
				   "%s"
				   ") AS selected_files"
				   " WHERE PathFromRoot<>''"
				   " ORDER BY InsShortName,CtrShortName,DegShortName,CrsShortName,PathFromRoot",
				   SearchQuery,
				   (unsigned) Brw_ADMI_DOC_INS,
				   (unsigned) Brw_ADMI_SHR_INS,
				   RangeQuery,
				   SearchQuery,
				   (unsigned) Brw_ADMI_DOC_CTR,
				   (unsigned) Brw_ADMI_SHR_CTR,
				   RangeQuery,
				   SearchQuery,
				   (unsigned) Brw_ADMI_DOC_DEG,
				   (unsigned) Brw_ADMI_SHR_DEG,
				   RangeQuery,
				   SearchQuery,
				   (unsigned) Brw_ADMI_DOC_CRS,
				   (unsigned) Brw_ADMI_SHR_CRS,
				   RangeQuery);

	 /***** List documents found *****/
	 Brw_ListDocsFound (&mysql_res,NumDocs,
			    Txt_open_document,
	                    Txt_open_documents);

	 return (unsigned) NumDocs;
	}

   return 0;
  }

/*****************************************************************************/
/**************** Search documents in my courses in database *****************/
/*****************************************************************************/

static unsigned Sch_SearchDocumentsInMyCoursesInDB (const char *RangeQuery)
  {
   extern const char *Txt_document_in_my_courses;
   extern const char *Txt_documents_in_my_courses;
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   MYSQL_RES *mysql_res;
   unsigned long NumDocs;

   /***** Check user's permission *****/
   if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_DOCUM_IN_MY_COURSES))
      /***** Split document string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,"SUBSTRING_INDEX(files.Path,'/',-1)",
				"_latin1 "," COLLATE latin1_general_ci"))
	{
	 /***** Create temporary table with codes of files in documents and shared areas accessible by me.
		It is necessary to speed up the second query *****/
	 DB_Query ("can not remove temporary table",
	           "DROP TEMPORARY TABLE IF EXISTS my_files_crs,my_files_grp");

	 DB_Query ("can not create temporary table",
		   "CREATE TEMPORARY TABLE my_files_crs"
		   " (FilCod INT NOT NULL,UNIQUE INDEX(FilCod))"
		   " ENGINE=MEMORY"
		   " SELECT files.FilCod FROM crs_usr,files"
		   " WHERE crs_usr.UsrCod=%ld"
		   " AND crs_usr.CrsCod=files.Cod"
		   " AND files.FileBrowser IN (%u,%u,%u,%u)",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Brw_ADMI_DOC_CRS,
		   (unsigned) Brw_ADMI_TCH_CRS,
		   (unsigned) Brw_ADMI_SHR_CRS,
		   (unsigned) Brw_ADMI_MRK_CRS);

	 DB_Query ("can not create temporary table",
	           "CREATE TEMPORARY TABLE my_files_grp"
		   " (FilCod INT NOT NULL,UNIQUE INDEX(FilCod))"
		   " ENGINE=MEMORY"
		   " SELECT files.FilCod FROM crs_grp_usr,files"
		   " WHERE crs_grp_usr.UsrCod=%ld"
		   " AND crs_grp_usr.GrpCod=files.Cod"
		   " AND files.FileBrowser IN (%u,%u,%u,%u)",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Brw_ADMI_DOC_GRP,
		   (unsigned) Brw_ADMI_TCH_GRP,
		   (unsigned) Brw_ADMI_SHR_GRP,
		   (unsigned) Brw_ADMI_MRK_GRP);

	 /***** Build the query *****/
	 NumDocs = DB_QuerySELECT (&mysql_res,"can not get files",
				   "SELECT * FROM "
				   "("
				   "SELECT files.FilCod,"
				   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
				   "institutions.InsCod,institutions.ShortName AS InsShortName,"
				   "centres.CtrCod,centres.ShortName AS CtrShortName,"
				   "degrees.DegCod,degrees.ShortName AS DegShortName,"
				   "courses.CrsCod,courses.ShortName AS CrsShortName,"
				   "-1 AS GrpCod"
				   " FROM files,courses,degrees,centres,institutions,countries"
				   " WHERE files.FilCod IN (SELECT FilCod FROM my_files_crs) AND %s"
				   " AND files.FileBrowser IN (%u,%u,%u,%u)"
				   " AND files.Cod=courses.CrsCod"
				   " AND courses.DegCod=degrees.DegCod"
				   " AND degrees.CtrCod=centres.CtrCod"
				   " AND centres.InsCod=institutions.InsCod"
				   " AND institutions.CtyCod=countries.CtyCod"
				   "%s"
				   " UNION "
				   "SELECT files.FilCod,"
				   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
				   "institutions.InsCod,institutions.ShortName AS InsShortName,"
				   "centres.CtrCod,centres.ShortName AS CtrShortName,"
				   "degrees.DegCod,degrees.ShortName AS DegShortName,"
				   "courses.CrsCod,courses.ShortName AS CrsShortName,"
				   "crs_grp.GrpCod"
				   " FROM files,crs_grp,crs_grp_types,courses,degrees,centres,institutions,countries"
				   " WHERE files.FilCod IN (SELECT FilCod FROM my_files_grp) AND %s"
				   " AND files.FileBrowser IN (%u,%u,%u,%u)"
				   " AND files.Cod=crs_grp.GrpCod"
				   " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
				   " AND crs_grp_types.CrsCod=courses.CrsCod"
				   " AND courses.DegCod=degrees.DegCod"
				   " AND degrees.CtrCod=centres.CtrCod"
				   " AND centres.InsCod=institutions.InsCod"
				   " AND institutions.CtyCod=countries.CtyCod"
				   "%s"
				   ") AS selected_files"
				   " WHERE PathFromRoot<>''"
				   " ORDER BY InsShortName,CtrShortName,DegShortName,CrsShortName,PathFromRoot",
				   SearchQuery,
				   (unsigned) Brw_ADMI_DOC_CRS,
				   (unsigned) Brw_ADMI_TCH_CRS,
				   (unsigned) Brw_ADMI_SHR_CRS,
				   (unsigned) Brw_ADMI_MRK_CRS,
				   RangeQuery,
				   SearchQuery,
				   (unsigned) Brw_ADMI_DOC_GRP,
				   (unsigned) Brw_ADMI_TCH_GRP,
				   (unsigned) Brw_ADMI_SHR_GRP,
				   (unsigned) Brw_ADMI_MRK_GRP,
				   RangeQuery);

	 /***** List documents found *****/
	 Brw_ListDocsFound (&mysql_res,NumDocs,
	                    Txt_document_in_my_courses,
	                    Txt_documents_in_my_courses);

	 /***** Drop temporary table *****/
	 DB_Query ("can not remove temporary table",
		   "DROP TEMPORARY TABLE IF EXISTS my_files_crs,my_files_grp");

	 return (unsigned) NumDocs;
	}

   return 0;
  }

/*****************************************************************************/
/********************** Search my documents in database **********************/
/*****************************************************************************/

static unsigned Sch_SearchMyDocumentsInDB (const char *RangeQuery)
  {
   extern const char *Txt_document_from_me;
   extern const char *Txt_documents_from_me;
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   MYSQL_RES *mysql_res;
   unsigned long NumDocs;

   /***** Check user's permission *****/
   if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_MY_DOCUMENTS))
      /***** Split document string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,"SUBSTRING_INDEX(files.Path,'/',-1)",
				"_latin1 "," COLLATE latin1_general_ci"))
	{
	 /***** Build the query *****/
	 NumDocs = DB_QuerySELECT (&mysql_res,"can not get files",
				   "SELECT * FROM "
				   "("
				   "SELECT files.FilCod,"	// Institution
				   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
				   "institutions.InsCod,institutions.ShortName AS InsShortName,"
				   "-1 AS CtrCod,'' AS CtrShortName,"
				   "-1 AS DegCod,'' AS DegShortName,"
				   "-1 AS CrsCod,'' AS CrsShortName,"
				   "-1 AS GrpCod"
				   " FROM files,courses,degrees,centres,institutions,countries"
				   " WHERE files.PublisherUsrCod=%ld AND %s"
				   " AND files.FileBrowser IN (%u,%u)"
				   " AND files.Cod=institutions.InsCod"
				   " AND institutions.CtyCod=countries.CtyCod"
				   "%s"
				   " UNION "
				   "SELECT files.FilCod,"	// Centre
				   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
				   "institutions.InsCod,institutions.ShortName AS InsShortName,"
				   "centres.CtrCod,centres.ShortName AS CtrShortName,"
				   "-1 AS DegCod,'' AS DegShortName,"
				   "-1 AS CrsCod,'' AS CrsShortName,"
				   "-1 AS GrpCod"
				   " FROM files,courses,degrees,centres,institutions,countries"
				   " WHERE files.PublisherUsrCod=%ld AND %s"
				   " AND files.FileBrowser IN (%u,%u)"
				   " AND files.Cod=centres.CtrCod"
				   " AND centres.InsCod=institutions.InsCod"
				   " AND institutions.CtyCod=countries.CtyCod"
				   "%s"
				   " UNION "
				   "SELECT files.FilCod,"	// Degree
				   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
				   "institutions.InsCod,institutions.ShortName AS InsShortName,"
				   "centres.CtrCod,centres.ShortName AS CtrShortName,"
				   "degrees.DegCod,degrees.ShortName AS DegShortName,"
				   "-1 AS CrsCod,'' AS CrsShortName,"
				   "-1 AS GrpCod"
				   " FROM files,courses,degrees,centres,institutions,countries"
				   " WHERE files.PublisherUsrCod=%ld AND %s"
				   " AND files.FileBrowser IN (%u,%u)"
				   " AND files.Cod=degrees.DegCod"
				   " AND degrees.CtrCod=centres.CtrCod"
				   " AND centres.InsCod=institutions.InsCod"
				   " AND institutions.CtyCod=countries.CtyCod"
				   "%s"
				   " UNION "
				   "SELECT files.FilCod,"	// Course
				   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
				   "institutions.InsCod,institutions.ShortName AS InsShortName,"
				   "centres.CtrCod,centres.ShortName AS CtrShortName,"
				   "degrees.DegCod,degrees.ShortName AS DegShortName,"
				   "courses.CrsCod,courses.ShortName AS CrsShortName,"
				   "-1 AS GrpCod"
				   " FROM files,courses,degrees,centres,institutions,countries"
				   " WHERE files.PublisherUsrCod=%ld AND %s"
				   " AND files.FileBrowser IN (%u,%u,%u,%u)"
				   " AND files.Cod=courses.CrsCod"
				   " AND courses.DegCod=degrees.DegCod"
				   " AND degrees.CtrCod=centres.CtrCod"
				   " AND centres.InsCod=institutions.InsCod"
				   " AND institutions.CtyCod=countries.CtyCod"
				   "%s"
				   " UNION "
				   "SELECT files.FilCod,"	// Group
				   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
				   "institutions.InsCod,institutions.ShortName AS InsShortName,"
				   "centres.CtrCod,centres.ShortName AS CtrShortName,"
				   "degrees.DegCod,degrees.ShortName AS DegShortName,"
				   "courses.CrsCod,courses.ShortName AS CrsShortName,"
				   "crs_grp.GrpCod"
				   " FROM files,crs_grp,crs_grp_types,courses,degrees,centres,institutions,countries"
				   " WHERE files.PublisherUsrCod=%ld AND %s"
				   " AND files.FileBrowser IN (%u,%u,%u,%u)"
				   " AND files.Cod=crs_grp.GrpCod"
				   " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
				   " AND crs_grp_types.CrsCod=courses.CrsCod"
				   " AND courses.DegCod=degrees.DegCod"
				   " AND degrees.CtrCod=centres.CtrCod"
				   " AND centres.InsCod=institutions.InsCod"
				   " AND institutions.CtyCod=countries.CtyCod"
				   "%s"
				   " UNION "
				   "SELECT files.FilCod,"	// Briefcase
				   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
				   "-1 AS InsCod,'' AS InsShortName,"
				   "-1 AS CtrCod,'' AS CtrShortName,"
				   "-1 AS DegCod,'' AS DegShortName,"
				   "-1 AS CrsCod,'' AS CrsShortName,"
				   "-1 AS GrpCod"
				   " FROM files"
				   " WHERE files.PublisherUsrCod=%ld AND %s"
				   " AND files.FileBrowser=%u"
				   ") AS selected_files"
				   " WHERE PathFromRoot<>''"
				   " ORDER BY InsShortName,CtrShortName,DegShortName,CrsShortName,PathFromRoot",
				   Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,
				   (unsigned) Brw_ADMI_DOC_INS,
				   (unsigned) Brw_ADMI_SHR_INS,
				   RangeQuery,
				   Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,
				   (unsigned) Brw_ADMI_DOC_CTR,
				   (unsigned) Brw_ADMI_SHR_CTR,
				   RangeQuery,
				   Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,
				   (unsigned) Brw_ADMI_DOC_DEG,
				   (unsigned) Brw_ADMI_SHR_DEG,
				   RangeQuery,
				   Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,
				   (unsigned) Brw_ADMI_DOC_CRS,
				   (unsigned) Brw_ADMI_TCH_CRS,
				   (unsigned) Brw_ADMI_SHR_CRS,
				   (unsigned) Brw_ADMI_MRK_CRS,
				   RangeQuery,
				   Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,
				   (unsigned) Brw_ADMI_DOC_GRP,
				   (unsigned) Brw_ADMI_TCH_GRP,
				   (unsigned) Brw_ADMI_SHR_GRP,
				   (unsigned) Brw_ADMI_MRK_GRP,
				   RangeQuery,
				   Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,
				   (unsigned) Brw_ADMI_BRF_USR);

	 /***** List documents found *****/
	 Brw_ListDocsFound (&mysql_res,NumDocs,
			    Txt_document_from_me,
	                    Txt_documents_from_me);

	 return (unsigned) NumDocs;
	}

   return 0;
  }

/*****************************************************************************/
/****** Build a search query by splitting a string to search into words ******/
/*****************************************************************************/
// Returns true if a valid search query is built
// Returns false when no valid search query

bool Sch_BuildSearchQuery (char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1],
                           const char *FieldName,
                           const char *CharSet,const char *Collate)
  {
   const char *Ptr;
   unsigned NumWords;
   unsigned NumWord;
   size_t LengthWord;
   size_t LengthTotal = 0;
   size_t MaxLengthWord = 0;
   char SearchWords[Sch_MAX_WORDS_IN_SEARCH][Sch_MAX_BYTES_SEARCH_WORD + 1];
   bool SearchWordIsValid = true;

   if (Gbl.Search.Str[0])
     {
      SearchQuery[0] = '\0';
      Ptr = Gbl.Search.Str;
      for (NumWords = 0;
	   NumWords < Sch_MAX_WORDS_IN_SEARCH && *Ptr;
	   NumWords++)
	{
	 /* Get next word */
	 Str_GetNextStringUntilSpace (&Ptr,SearchWords[NumWords],Sch_MAX_BYTES_SEARCH_WORD);

	 /* Is this word valid? */
	 switch (Gbl.Search.WhatToSearch)
	   {
	    case Sch_SEARCH_OPEN_DOCUMENTS:
	    case Sch_SEARCH_DOCUM_IN_MY_COURSES:
	    case Sch_SEARCH_MY_DOCUMENTS:
	       SearchWordIsValid = Str_ConvertFilFolLnkNameToValid (SearchWords[NumWords]);
	       break;
	    default:
	       SearchWordIsValid = true;
	       break;
	   }

	 /* Check if this word is repeated (case insensitive) */
	 for (NumWord = 0;
	      SearchWordIsValid && NumWord < NumWords;
	      NumWord++)
	    if (!strcasecmp (SearchWords[NumWord],SearchWords[NumWords]))
	       SearchWordIsValid = false;

	 /* Concatenate word to search string */
	 if (SearchWordIsValid)
	   {
	    LengthWord = strlen (SearchWords[NumWords]);
	    LengthTotal += LengthWord;
	    if (LengthWord > MaxLengthWord)
	       MaxLengthWord = LengthWord;
	    if (strlen (SearchQuery) + 128 + LengthWord >
	        Sch_MAX_BYTES_SEARCH_QUERY)	// Prevent string overflow
	       break;
	    if (NumWords)
	       Str_Concat (SearchQuery," AND ",
	                   Sch_MAX_BYTES_SEARCH_QUERY);
	    Str_Concat (SearchQuery,FieldName,
	                Sch_MAX_BYTES_SEARCH_QUERY);
	    Str_Concat (SearchQuery," LIKE ",
	                Sch_MAX_BYTES_SEARCH_QUERY);
	    if (CharSet)
	       if (CharSet[0])
		  Str_Concat (SearchQuery,CharSet,
		              Sch_MAX_BYTES_SEARCH_QUERY);
	    Str_Concat (SearchQuery,"'%",
	                Sch_MAX_BYTES_SEARCH_QUERY);
	    Str_Concat (SearchQuery,SearchWords[NumWords],
	                Sch_MAX_BYTES_SEARCH_QUERY);
	    Str_Concat (SearchQuery,"%'",
	                Sch_MAX_BYTES_SEARCH_QUERY);
	    if (Collate)
	       if (Collate[0])
		  Str_Concat (SearchQuery,Collate,
		              Sch_MAX_BYTES_SEARCH_QUERY);
	   }
	}

      /***** If search string valid? *****/
      if (LengthTotal < Sch_MIN_LENGTH_TOTAL ||
	  MaxLengthWord < Sch_MIN_LENGTH_LONGEST_WORD)
	 return false;

      return true;
     }

   return false;
  }

/*****************************************************************************/
/********************** Save last search into session ************************/
/*****************************************************************************/

static void Sch_SaveLastSearchIntoSession (void)
  {
   if (Gbl.Usrs.Me.Logged)
     {
      if (Gbl.Search.WhatToSearch == Sch_SEARCH_UNKNOWN)
	 Gbl.Search.WhatToSearch = Sch_WHAT_TO_SEARCH_DEFAULT;

      /***** Save last search in session *****/
      DB_QueryUPDATE ("can not update last search in session",
		      "UPDATE sessions SET WhatToSearch=%u,SearchStr='%s'"
		      " WHERE SessionId='%s'",
		      (unsigned) Gbl.Search.WhatToSearch,
		      Gbl.Search.Str,
		      Gbl.Session.Id);

      /***** Update my last type of search *****/
      // WhatToSearch is stored in usr_last for next time I log in
      // In other existing sessions distinct to this, WhatToSearch will remain unchanged
      DB_QueryUPDATE ("can not update type of search in user's last data",
		      "UPDATE usr_last SET WhatToSearch=%u"
		      " WHERE UsrCod=%ld",
		      (unsigned) Gbl.Search.WhatToSearch,
		      Gbl.Usrs.Me.UsrDat.UsrCod);
     }
  }
