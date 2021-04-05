// swad_search.c: search for courses and teachers

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include <string.h>	// For string functions...

#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_layout.h"
#include "swad_parameter.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

#define Sch_MIN_LENGTH_LONGEST_WORD	  3
#define Sch_MIN_LENGTH_TOTAL		  6	// "An Ann" is not valid; "A An Ann" is valid

/*****************************************************************************/
/****************************** Private types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Sch_PutFormToSearchWithWhatToSearchAndScope (Act_Action_t Action,Hie_Lvl_Level_t DefaultScope);
static bool Sch_CheckIfIHavePermissionToSearch (Sch_WhatToSearch_t WhatToSearch);
static void Sch_GetParamSearch (void);
static void Sch_SearchInDB (void);
static unsigned Sch_SearchCountriesInDB (const char *RangeQuery);
static unsigned Sch_SearchInstitutionsInDB (const char *RangeQuery);
static unsigned Sch_SearchCentersInDB (const char *RangeQuery);
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
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActSch,Hie_Lvl_SYS);
  }

/*****************************************************************************/
/****************** Put a form to search, including scope ********************/
/*****************************************************************************/

static void Sch_PutFormToSearchWithWhatToSearchAndScope (Act_Action_t Action,Hie_Lvl_Level_t DefaultScope)
  {
   extern const char *Hlp_START_Search;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Scope;
   extern const char *Txt_SEARCH_X_in_Y;
   extern const char *Txt_all;
   extern const char *Txt_countries;
   extern const char *Txt_institutions;
   extern const char *Txt_centers;
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
      [Sch_SEARCH_UNKNOWN            ] = "",
      [Sch_SEARCH_ALL                ] = Txt_all,
      [Sch_SEARCH_COUNTRIES          ] = Txt_countries,
      [Sch_SEARCH_INSTITS            ] = Txt_institutions,
      [Sch_SEARCH_CENTERS            ] = Txt_centers,
      [Sch_SEARCH_DEGREES            ] = Txt_degrees,
      [Sch_SEARCH_COURSES            ] = Txt_courses,
      [Sch_SEARCH_USERS              ] = Txt_users[Usr_SEX_UNKNOWN],
      [Sch_SEARCH_TEACHERS           ] = Txt_ROLES_PLURAL_abc[Rol_TCH][Usr_SEX_UNKNOWN],
      [Sch_SEARCH_STUDENTS           ] = Txt_ROLES_PLURAL_abc[Rol_STD][Usr_SEX_UNKNOWN],
      [Sch_SEARCH_GUESTS             ] = Txt_ROLES_PLURAL_abc[Rol_GST][Usr_SEX_UNKNOWN],
      [Sch_SEARCH_OPEN_DOCUMENTS     ] = Txt_open_documents,
      [Sch_SEARCH_DOCUM_IN_MY_COURSES] = Txt_documents_in_my_courses,
      [Sch_SEARCH_MY_DOCUMENTS       ] = Txt_my_documents,
     };
   Sch_WhatToSearch_t WhatToSearch;
   unsigned WTS;

   /***** Begin box *****/
   HTM_DIV_Begin ("class=\"CM\"");
   Box_BoxBegin (NULL,Txt_Search,
                 Crs_PutIconToSelectMyCourses,NULL,
                 Hlp_START_Search,Box_NOT_CLOSABLE);

   /***** Begin form *****/
   Frm_BeginForm (Action);

   /***** Scope (whole platform, current country, current institution,
                 current center, current degree or current course) *****/
   HTM_DIV_Begin ("class=\"CM\"");
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtColonNBSP (Txt_Scope);
   Gbl.Scope.Allowed = 1 << Hie_Lvl_SYS |
	               1 << Hie_Lvl_CTY |
		       1 << Hie_Lvl_INS |
		       1 << Hie_Lvl_CTR |
		       1 << Hie_Lvl_DEG |
		       1 << Hie_Lvl_CRS;
   Gbl.Scope.Default = DefaultScope;
   Sco_GetScope ("ScopeSch");
   Sco_PutSelectorScope ("ScopeSch",HTM_DONT_SUBMIT_ON_CHANGE);
   HTM_LABEL_End ();
   HTM_DIV_End ();

   /***** String to find *****/
   Sch_PutInputStringToSearch ("");

   /***** What to search? *****/
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF (" %s&nbsp;",Txt_SEARCH_X_in_Y);
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "name=\"WhatToSearch\" class=\"WHAT_TO_SEARCH\"");
   for (WhatToSearch  = (Sch_WhatToSearch_t) 0;
        WhatToSearch <= (Sch_WhatToSearch_t) (Sch_NUM_WHAT_TO_SEARCH - 1);
        WhatToSearch++)
      if (Sch_CheckIfIHavePermissionToSearch (WhatToSearch))
	{
	 WTS = (unsigned) WhatToSearch;
	 HTM_OPTION (HTM_Type_UNSIGNED,&WTS,
		     Gbl.Search.WhatToSearch == WhatToSearch,false,
		     "%s",Titles[WhatToSearch]);
        }
   HTM_SELECT_End ();
   HTM_LABEL_End ();

   /***** Magnifying glass icon *****/
   Sch_PutMagnifyingGlassButton ("search.svg");

   /***** Send button *****/
   Btn_PutButton (Btn_CONFIRM_BUTTON,Txt_Search);

   /***** End form *****/
   Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************* Check if I have permission to execute an action ***************/
/*****************************************************************************/

static bool Sch_CheckIfIHavePermissionToSearch (Sch_WhatToSearch_t WhatToSearch)
  {
   static unsigned Permissions[Sch_NUM_WHAT_TO_SEARCH] =
     {
      [Sch_SEARCH_UNKNOWN            ] = 0x000,
      [Sch_SEARCH_ALL                ] = 0x3FF,
      [Sch_SEARCH_COUNTRIES          ] = 0x3FF,
      [Sch_SEARCH_INSTITS            ] = 0x3FF,
      [Sch_SEARCH_CENTERS            ] = 0x3FF,
      [Sch_SEARCH_DEGREES            ] = 0x3FF,
      [Sch_SEARCH_COURSES            ] = 0x3FF,
      [Sch_SEARCH_USERS              ] = 0x3FF,
      [Sch_SEARCH_TEACHERS           ] = 0x3FF,
      [Sch_SEARCH_STUDENTS           ] = 0x3FF,
      [Sch_SEARCH_GUESTS             ] = 0x3FF,
      [Sch_SEARCH_OPEN_DOCUMENTS     ] = 0x3FF,
      [Sch_SEARCH_DOCUM_IN_MY_COURSES] = 0x3FE,	// Only if I am logged
      [Sch_SEARCH_MY_DOCUMENTS       ] = 0x3FE,	// Only if I am logged
     };

   return (Permissions[WhatToSearch] & (1 << Gbl.Usrs.Me.Role.Logged));
  }

/*****************************************************************************/
/**************** Put a form to search in page top heading *******************/
/*****************************************************************************/

void Sch_PutFormToSearchInPageTopHeading (void)
  {
   HTM_DIV_Begin ("id=\"head_row_1_search\"");

   /***** Put form *****/
   Frm_BeginForm (ActSch);
   Sco_PutParamScope ("ScopeSch",Hie_Lvl_SYS);
   Sch_PutInputStringToSearch ("head_search_text");
   Sch_PutMagnifyingGlassButton ("search-white.svg");
   Frm_EndForm ();

   HTM_DIV_End ();	// head_row_1_search
  }

/*****************************************************************************/
/********************* Put string to search inside form **********************/
/*****************************************************************************/

void Sch_PutInputStringToSearch (const char *IdInputText)
  {
   extern const char *Txt_Search;

   /***** String to find *****/
   // Input field not required, because it can be hidden (display:none)
   HTM_INPUT_SEARCH ("Search",Sch_MAX_CHARS_STRING_TO_FIND,Gbl.Search.Str,
	             "id=\"%s\" size=\"18\" placeholder=\"%s&hellip;\"",
		     IdInputText,Txt_Search);
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
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActSch,Hie_Lvl_SYS);

      /***** Show results of search *****/
      Sch_SearchInDB ();
     }
   else
      /***** Show search form and selectors *****/
      Sch_ReqSysSearch ();
  }

/*****************************************************************************/
/**** Search institutions, centers, degrees, courses, teachers, documents ****/
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
      case Hie_Lvl_UNK:
	 // Not aplicable
      case Hie_Lvl_SYS:
         RangeQuery[0] = '\0';
         break;
      case Hie_Lvl_CTY:
         sprintf (RangeQuery," AND cty_countrs.CtyCod=%ld",
                  Gbl.Hierarchy.Cty.CtyCod);
         break;
      case Hie_Lvl_INS:
         sprintf (RangeQuery," AND ins_instits.InsCod=%ld",
                  Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_Lvl_CTR:
         sprintf (RangeQuery," AND ctr_centers.CtrCod=%ld",
                  Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_Lvl_DEG:
         sprintf (RangeQuery," AND deg_degrees.DegCod=%ld",
                  Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_Lvl_CRS:
         sprintf (RangeQuery," AND crs_courses.CrsCod=%ld",
                  Gbl.Hierarchy.Crs.CrsCod);
         break;
     }

   if (Gbl.Search.WhatToSearch == Sch_SEARCH_UNKNOWN)
      Gbl.Search.WhatToSearch = Sch_WHAT_TO_SEARCH_DEFAULT;
   switch (Gbl.Search.WhatToSearch)
     {
      case Sch_SEARCH_ALL:
	 NumResults  = Sch_SearchCountriesInDB (RangeQuery);
	 NumResults += Sch_SearchInstitutionsInDB (RangeQuery);
	 NumResults += Sch_SearchCentersInDB (RangeQuery);
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
      case Sch_SEARCH_CENTERS:
	 NumResults = Sch_SearchCentersInDB (RangeQuery);
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
   if (Gbl.Scope.Current != Hie_Lvl_INS &&
       Gbl.Scope.Current != Hie_Lvl_CTR &&
       Gbl.Scope.Current != Hie_Lvl_DEG &&
       Gbl.Scope.Current != Hie_Lvl_CRS)
      /***** Check user's permission *****/
      if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_COUNTRIES))
	{
	 /***** Split countries string into words *****/
	 snprintf (FieldName,sizeof (FieldName),"Name_%s",
		   Lan_STR_LANG_ID[Gbl.Prefs.Language]);
	 if (Sch_BuildSearchQuery (SearchQuery,FieldName,NULL,NULL))
	   {
	    /***** Query database and list institutions found *****/
	    NumCtys = (unsigned)
	    DB_QuerySELECT (&mysql_res,"can not get countries",
			    "SELECT CtyCod"
			     " FROM cty_countrs"
			    " WHERE %s"
			       "%s"
			    " ORDER BY Name_%s",
			    SearchQuery,
			    RangeQuery,
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
   if (Gbl.Scope.Current != Hie_Lvl_CTR &&
       Gbl.Scope.Current != Hie_Lvl_DEG &&
       Gbl.Scope.Current != Hie_Lvl_CRS)
      /***** Check user's permission *****/
      if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_INSTITS))
	 /***** Split institutions string into words *****/
	 if (Sch_BuildSearchQuery (SearchQuery,"ins_instits.FullName",NULL,NULL))
	   {
	    /***** Query database and list institutions found *****/
	    NumInss = (unsigned)
	    DB_QuerySELECT (&mysql_res,"can not get institutions",
			    "SELECT ins_instits.InsCod"
			     " FROM ins_instits,"
				   "cty_countrs"
			    " WHERE %s"
			      " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			      "%s"
			    " ORDER BY ins_instits.FullName,"
			              "cty_countrs.Name_%s",
			    SearchQuery,
			    RangeQuery,
			    Lan_STR_LANG_ID[Gbl.Prefs.Language]);
	    Ins_ListInssFound (&mysql_res,NumInss);
	    return NumInss;
	   }

   return 0;
  }

/*****************************************************************************/
/************************* Search centers in database ************************/
/*****************************************************************************/
// Returns number of centers found

static unsigned Sch_SearchCentersInDB (const char *RangeQuery)
  {
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   MYSQL_RES *mysql_res;
   unsigned NumCtrs;

   /***** Check scope *****/
   if (Gbl.Scope.Current != Hie_Lvl_DEG &&
       Gbl.Scope.Current != Hie_Lvl_CRS)
      /***** Check user's permission *****/
      if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_CENTERS))
	 /***** Split center string into words *****/
	 if (Sch_BuildSearchQuery (SearchQuery,"ctr_centers.FullName",NULL,NULL))
	   {
	    /***** Query database and list centers found *****/
	    NumCtrs = (unsigned)
	    DB_QuerySELECT (&mysql_res,"can not get centers",
			    "SELECT ctr_centers.CtrCod"
			     " FROM ctr_centers,"
			           "ins_instits,"
			           "cty_countrs"
			    " WHERE %s"
			      " AND ctr_centers.InsCod=ins_instits.InsCod"
			      " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			      "%s"
			    " ORDER BY ctr_centers.FullName,"
			              "ins_instits.FullName",
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
   if (Gbl.Scope.Current != Hie_Lvl_CRS)
      /***** Check user's permission *****/
      if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_DEGREES))
	 /***** Split degree string into words *****/
	 if (Sch_BuildSearchQuery (SearchQuery,"deg_degrees.FullName",NULL,NULL))
	   {
	    /***** Query database and list degrees found *****/
	    NumDegs = (unsigned)
	    DB_QuerySELECT (&mysql_res,"can not get degrees",
			    "SELECT deg_degrees.DegCod"
			     " FROM deg_degrees,"
			           "ctr_centers,"
			           "ins_instits,"
			           "cty_countrs"
			    " WHERE %s"
			      " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			      " AND ctr_centers.InsCod=ins_instits.InsCod"
			      " AND ins_instits.CtyCod=cty_countrs.CtyCod"
			      "%s"
			    " ORDER BY deg_degrees.FullName,"
				      "ins_instits.FullName",
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
      if (Sch_BuildSearchQuery (SearchQuery,"crs_courses.FullName",NULL,NULL))
	{
	 /***** Query database and list courses found *****/
	 NumCrss = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get courses",
		         "SELECT deg_degrees.DegCod,"		// row[0]
			        "crs_courses.CrsCod,"		// row[1]
			        "deg_degrees.ShortName,"	// row[2]
			        "deg_degrees.FullName,"		// row[3]
			        "crs_courses.Year,"		// row[4]
			        "crs_courses.FullName,"		// row[5]
			        "ctr_centers.ShortName"		// row[6]
		          " FROM crs_courses,"
		                "deg_degrees,"
		                "ctr_centers,"
		                "ins_instits,"
		                "cty_countrs"
		         " WHERE %s"
		           " AND crs_courses.DegCod=deg_degrees.DegCod"
		           " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		           " AND ctr_centers.InsCod=ins_instits.InsCod"
		           " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		           "%s"
		         " ORDER BY crs_courses.FullName,"
				   "ins_instits.FullName,"
				   "degrees.FullName,"
				   "crs_courses.Year",
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
	 NumDocs =
	 DB_QuerySELECT (&mysql_res,"can not get files",
		         "SELECT * FROM "
		         "("
		         "SELECT brw_files.FilCod,"	// Institution
			        "SUBSTRING(brw_files.Path,LOCATE('/',brw_files.Path)) AS PathFromRoot,"
			        "ins_instits.InsCod,"
			        "ins_instits.ShortName AS InsShortName,"
			        "-1 AS CtrCod,"
			        "'' AS CtrShortName,"
			        "-1 AS DegCod,"
			        "'' AS DegShortName,"
			        "-1 AS CrsCod,"
			        "'' AS CrsShortName,"
			        "-1 AS GrpCod"
		          " FROM brw_files,"
		                "crs_courses,"
		                "deg_degrees,"
		                "ctr_centers,"
		                "ins_instits,"
		                "cty_countrs"
		         " WHERE brw_files.Public='Y'"
		           " AND %s"
		           " AND brw_files.FileBrowser IN (%u,%u)"
		           " AND brw_files.Cod=ins_instits.InsCod"
		           " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		           "%s"
		         " UNION "
		         "SELECT brw_files.FilCod,"	// Center
			        "SUBSTRING(brw_files.Path,LOCATE('/',brw_files.Path)) AS PathFromRoot,"
			        "ins_instits.InsCod,"
			        "ins_instits.ShortName AS InsShortName,"
			        "ctr_centers.CtrCod,"
			        "ctr_centers.ShortName AS CtrShortName,"
			        "-1 AS DegCod,"
			        "'' AS DegShortName,"
			        "-1 AS CrsCod,"
			        "'' AS CrsShortName,"
			        "-1 AS GrpCod"
		          " FROM brw_files,"
		                "crs_courses,"
		                "deg_degrees,"
		                "ctr_centers,"
		                "ins_instits,"
		                "cty_countrs"
		         " WHERE brw_files.Public='Y' AND %s"
		           " AND brw_files.FileBrowser IN (%u,%u)"
		           " AND brw_files.Cod=ctr_centers.CtrCod"
		           " AND ctr_centers.InsCod=ins_instits.InsCod"
		           " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		           "%s"
		         " UNION "
		         "SELECT brw_files.FilCod,"	// Degree
			        "SUBSTRING(brw_files.Path,LOCATE('/',brw_files.Path)) AS PathFromRoot,"
			        "ins_instits.InsCod,"
			        "ins_instits.ShortName AS InsShortName,"
			        "ctr_centers.CtrCod,"
			        "ctr_centers.ShortName AS CtrShortName,"
			        "deg_degrees.DegCod,"
			        "deg_degrees.ShortName AS DegShortName,"
			        "-1,'' AS CrsShortName,"
			        "-1"
		          " FROM brw_files,"
		                "crs_courses,"
		                "deg_degrees,"
		                "ctr_centers,"
		                "ins_instits,"
		                "cty_countrs"
		         " WHERE brw_files.Public='Y' AND %s"
		           " AND brw_files.FileBrowser IN (%u,%u)"
		           " AND brw_files.Cod=deg_degrees.DegCod"
		           " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		             " AND ctr_centers.InsCod=ins_instits.InsCod"
		         " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		           "%s"
		         " UNION "
		         "SELECT brw_files.FilCod,"	// Course
			        "SUBSTRING(brw_files.Path,LOCATE('/',brw_files.Path)) AS PathFromRoot,"
			        "ins_instits.InsCod,"
			        "ins_instits.ShortName AS InsShortName,"
			        "ctr_centers.CtrCod,"
			        "ctr_centers.ShortName AS CtrShortName,"
			        "deg_degrees.DegCod,"
			        "deg_degrees.ShortName AS DegShortName,"
			        "crs_courses.CrsCod,"
			        "crs_courses.ShortName AS CrsShortName,"
			        "-1"
		          " FROM brw_files,"
		                "crs_courses,"
		                "deg_degrees,"
		                "ctr_centers,"
		                "ins_instits,"
		                "cty_countrs"
		         " WHERE brw_files.Public='Y' AND %s"
		           " AND brw_files.FileBrowser IN (%u,%u)"
		           " AND brw_files.Cod=crs_courses.CrsCod"
		           " AND crs_courses.DegCod=deg_degrees.DegCod"
		           " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		           " AND ctr_centers.InsCod=ins_instits.InsCod"
		           " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		           "%s"
		         ") AS selected_files"
		         " WHERE PathFromRoot<>''"
		         " ORDER BY InsShortName,"
				   "CtrShortName,"
				   "DegShortName,"
				   "CrsShortName,"
				   "PathFromRoot",
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
      if (Sch_BuildSearchQuery (SearchQuery,"SUBSTRING_INDEX(brw_files.Path,'/',-1)",
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
		   " SELECT brw_files.FilCod"
		     " FROM crs_users,"
		           "brw_files"
		    " WHERE crs_users.UsrCod=%ld"
		      " AND crs_users.CrsCod=brw_files.Cod"
		      " AND brw_files.FileBrowser IN (%u,%u,%u,%u)",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Brw_ADMI_DOC_CRS,
		   (unsigned) Brw_ADMI_TCH_CRS,
		   (unsigned) Brw_ADMI_SHR_CRS,
		   (unsigned) Brw_ADMI_MRK_CRS);

	 DB_Query ("can not create temporary table",
	           "CREATE TEMPORARY TABLE my_files_grp"
		   " (FilCod INT NOT NULL,UNIQUE INDEX(FilCod))"
		   " ENGINE=MEMORY"
		   " SELECT brw_files.FilCod"
		     " FROM grp_users,"
		           "brw_files"
		    " WHERE grp_users.UsrCod=%ld"
		      " AND grp_users.GrpCod=brw_files.Cod"
		      " AND brw_files.FileBrowser IN (%u,%u,%u,%u)",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Brw_ADMI_DOC_GRP,
		   (unsigned) Brw_ADMI_TCH_GRP,
		   (unsigned) Brw_ADMI_SHR_GRP,
		   (unsigned) Brw_ADMI_MRK_GRP);

	 /***** Build the query *****/
	 NumDocs =
	 DB_QuerySELECT (&mysql_res,"can not get files",
		         "SELECT * FROM "
		         "("
		         "SELECT brw_files.FilCod,"
			        "SUBSTRING(brw_files.Path,LOCATE('/',brw_files.Path)) AS PathFromRoot,"
			        "ins_instits.InsCod,"
			        "ins_instits.ShortName AS InsShortName,"
			        "ctr_centers.CtrCod,"
			        "ctr_centers.ShortName AS CtrShortName,"
			        "deg_degrees.DegCod,"
			        "deg_degrees.ShortName AS DegShortName,"
			        "crs_courses.CrsCod,"
			        "crs_courses.ShortName AS CrsShortName,"
			        "-1 AS GrpCod"
		          " FROM brw_files,"
		                "crs_courses,"
		                "deg_degrees,"
		                "ctr_centers,"
		                "ins_instits,"
		                "cty_countrs"
		         " WHERE brw_files.FilCod IN"
		               " (SELECT FilCod"
		                  " FROM my_files_crs)"
		           " AND %s"
		           " AND brw_files.FileBrowser IN (%u,%u,%u,%u)"
		           " AND brw_files.Cod=crs_courses.CrsCod"
		           " AND crs_courses.DegCod=deg_degrees.DegCod"
		           " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		           " AND ctr_centers.InsCod=ins_instits.InsCod"
		           " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		           "%s"
		         " UNION "
		         "SELECT brw_files.FilCod,"
			        "SUBSTRING(brw_files.Path,LOCATE('/',brw_files.Path)) AS PathFromRoot,"
			        "ins_instits.InsCod,"
			        "ins_instits.ShortName AS InsShortName,"
			        "ctr_centers.CtrCod,"
			        "ctr_centers.ShortName AS CtrShortName,"
			        "deg_degrees.DegCod,"
			        "deg_degrees.ShortName AS DegShortName,"
			        "crs_courses.CrsCod,"
			        "crs_courses.ShortName AS CrsShortName,"
			        "grp_groups.GrpCod"
		          " FROM brw_files,"
		                "grp_groups,"
		                "grp_types,"
		                "crs_courses,"
		                "deg_degrees,"
		                "ctr_centers,"
		                "ins_instits,"
		                "cty_countrs"
		         " WHERE brw_files.FilCod IN"
		               " (SELECT FilCod"
		                  " FROM my_files_grp)"
		           " AND %s"
		           " AND brw_files.FileBrowser IN (%u,%u,%u,%u)"
		           " AND brw_files.Cod=grp_groups.GrpCod"
		           " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
		           " AND grp_types.CrsCod=crs_courses.CrsCod"
		           " AND crs_courses.DegCod=deg_degrees.DegCod"
		           " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		           " AND ctr_centers.InsCod=ins_instits.InsCod"
		           " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		           "%s"
		         ") AS selected_files"
		         " WHERE PathFromRoot<>''"
		         " ORDER BY InsShortName,"
				   "CtrShortName,"
				   "DegShortName,"
				   "CrsShortName,"
				   "PathFromRoot",
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
      if (Sch_BuildSearchQuery (SearchQuery,"SUBSTRING_INDEX(brw_files.Path,'/',-1)",
				"_latin1 "," COLLATE latin1_general_ci"))
	{
	 /***** Build the query *****/
	 NumDocs =
	 DB_QuerySELECT (&mysql_res,"can not get files",
		         "SELECT * FROM "
		         "("
		         "SELECT brw_files.FilCod,"	// Institution
			        "SUBSTRING(brw_files.Path,LOCATE('/',brw_files.Path)) AS PathFromRoot,"
			        "ins_instits.InsCod,"
			        "ins_instits.ShortName AS InsShortName,"
			        "-1 AS CtrCod,"
			        "'' AS CtrShortName,"
			        "-1 AS DegCod,"
			        "'' AS DegShortName,"
			        "-1 AS CrsCod,"
			        "'' AS CrsShortName,"
			        "-1 AS GrpCod"
		          " FROM brw_files,"
		                "crs_courses,"
		                "deg_degrees,"
		                "ctr_centers,"
		                "ins_instits,"
		                "cty_countrs"
		         " WHERE brw_files.PublisherUsrCod=%ld"
		           " AND %s"
		           " AND brw_files.FileBrowser IN (%u,%u)"
		           " AND brw_files.Cod=ins_instits.InsCod"
		           " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		           "%s"
		         " UNION "
		         "SELECT brw_files.FilCod,"	// Center
			        "SUBSTRING(brw_files.Path,LOCATE('/',brw_files.Path)) AS PathFromRoot,"
			        "ins_instits.InsCod,"
			        "ins_instits.ShortName AS InsShortName,"
			        "ctr_centers.CtrCod,"
			        "ctr_centers.ShortName AS CtrShortName,"
			        "-1 AS DegCod,"
			        "'' AS DegShortName,"
			        "-1 AS CrsCod,"
			        "'' AS CrsShortName,"
			        "-1 AS GrpCod"
		          " FROM brw_files,"
		                "crs_courses,"
		                "deg_degrees,"
		                "ctr_centers,"
		                "ins_instits,"
		                "cty_countrs"
		         " WHERE brw_files.PublisherUsrCod=%ld AND %s"
		           " AND brw_files.FileBrowser IN (%u,%u)"
		           " AND brw_files.Cod=ctr_centers.CtrCod"
		           " AND ctr_centers.InsCod=ins_instits.InsCod"
		           " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		           "%s"
		         " UNION "
		         "SELECT brw_files.FilCod,"	// Degree
			        "SUBSTRING(brw_files.Path,LOCATE('/',brw_files.Path)) AS PathFromRoot,"
			        "ins_instits.InsCod,"
			        "ins_instits.ShortName AS InsShortName,"
			        "ctr_centers.CtrCod,"
			        "ctr_centers.ShortName AS CtrShortName,"
			        "deg_degrees.DegCod,"
			        "deg_degrees.ShortName AS DegShortName,"
			        "-1 AS CrsCod,"
			        "'' AS CrsShortName,"
			        "-1 AS GrpCod"
		          " FROM brw_files,"
		                "crs_courses,"
		                "deg_degrees,"
		                "ctr_centers,"
		                "ins_instits,"
		                "cty_countrs"
		         " WHERE brw_files.PublisherUsrCod=%ld"
		           " AND %s"
		           " AND brw_files.FileBrowser IN (%u,%u)"
		           " AND brw_files.Cod=deg_degrees.DegCod"
		           " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		           " AND ctr_centers.InsCod=ins_instits.InsCod"
		           " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		           "%s"
		         " UNION "
		         "SELECT brw_files.FilCod,"	// Course
			        "SUBSTRING(brw_files.Path,LOCATE('/',brw_files.Path)) AS PathFromRoot,"
			        "ins_instits.InsCod,"
			        "ins_instits.ShortName AS InsShortName,"
			        "ctr_centers.CtrCod,"
			        "ctr_centers.ShortName AS CtrShortName,"
			        "deg_degrees.DegCod,"
			        "deg_degrees.ShortName AS DegShortName,"
			        "crs_courses.CrsCod,"
			        "crs_courses.ShortName AS CrsShortName,"
			        "-1 AS GrpCod"
		          " FROM brw_files,"
		                "crs_courses,"
		                "deg_degrees,"
		                "ctr_centers,"
		                "ins_instits,"
		                "cty_countrs"
		         " WHERE brw_files.PublisherUsrCod=%ld"
		           " AND %s"
		           " AND brw_files.FileBrowser IN (%u,%u,%u,%u)"
		           " AND brw_files.Cod=crs_courses.CrsCod"
		           " AND crs_courses.DegCod=deg_degrees.DegCod"
		           " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		           " AND ctr_centers.InsCod=ins_instits.InsCod"
		           " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		           "%s"
		         " UNION "
		         "SELECT brw_files.FilCod,"	// Group
			        "SUBSTRING(brw_files.Path,LOCATE('/',brw_files.Path)) AS PathFromRoot,"
			        "ins_instits.InsCod,"
			        "ins_instits.ShortName AS InsShortName,"
			        "ctr_centers.CtrCod,"
			        "ctr_centers.ShortName AS CtrShortName,"
			        "deg_degrees.DegCod,"
			        "deg_degrees.ShortName AS DegShortName,"
			        "crs_courses.CrsCod,"
			        "crs_courses.ShortName AS CrsShortName,"
			        "grp_groups.GrpCod"
		          " FROM brw_files,"
		                "grp_groups,"
		                "grp_types,"
		                "crs_courses,"
		                "deg_degrees,"
		                "ctr_centers,"
		                "ins_instits,"
		                "cty_countrs"
		         " WHERE brw_files.PublisherUsrCod=%ld"
		           " AND %s"
		           " AND brw_files.FileBrowser IN (%u,%u,%u,%u)"
		           " AND brw_files.Cod=grp_groups.GrpCod"
		           " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
		           " AND grp_types.CrsCod=crs_courses.CrsCod"
		           " AND crs_courses.DegCod=deg_degrees.DegCod"
		           " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		           " AND ctr_centers.InsCod=ins_instits.InsCod"
		           " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		           "%s"
		         " UNION "
		         "SELECT brw_files.FilCod,"	// Briefcase
			        "SUBSTRING(brw_files.Path,LOCATE('/',brw_files.Path)) AS PathFromRoot,"
			        "-1 AS InsCod,"
			        "'' AS InsShortName,"
			        "-1 AS CtrCod,"
			        "'' AS CtrShortName,"
			        "-1 AS DegCod,"
			        "'' AS DegShortName,"
			        "-1 AS CrsCod,"
			        "'' AS CrsShortName,"
			        "-1 AS GrpCod"
		          " FROM brw_files"
		         " WHERE brw_files.PublisherUsrCod=%ld"
		           " AND %s"
		           " AND brw_files.FileBrowser=%u"
		         ") AS selected_files"
		         " WHERE PathFromRoot<>''"
		         " ORDER BY InsShortName,"
		                   "CtrShortName,"
		                   "DegShortName,"
		                   "CrsShortName,"
		                   "PathFromRoot",
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
	       Str_Concat (SearchQuery," AND ",Sch_MAX_BYTES_SEARCH_QUERY);
	    Str_Concat (SearchQuery,FieldName,Sch_MAX_BYTES_SEARCH_QUERY);
	    Str_Concat (SearchQuery," LIKE ",Sch_MAX_BYTES_SEARCH_QUERY);
	    if (CharSet)
	       if (CharSet[0])
		  Str_Concat (SearchQuery,CharSet,Sch_MAX_BYTES_SEARCH_QUERY);
	    Str_Concat (SearchQuery,"'%",Sch_MAX_BYTES_SEARCH_QUERY);
	    Str_Concat (SearchQuery,SearchWords[NumWords],Sch_MAX_BYTES_SEARCH_QUERY);
	    Str_Concat (SearchQuery,"%'",Sch_MAX_BYTES_SEARCH_QUERY);
	    if (Collate)
	       if (Collate[0])
		  Str_Concat (SearchQuery,Collate,Sch_MAX_BYTES_SEARCH_QUERY);
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
		      "UPDATE ses_sessions"
		        " SET WhatToSearch=%u,"
		             "SearchStr='%s'"
		      " WHERE SessionId='%s'",
		      (unsigned) Gbl.Search.WhatToSearch,
		      Gbl.Search.Str,
		      Gbl.Session.Id);

      /***** Update my last type of search *****/
      // WhatToSearch is stored in usr_last for next time I log in
      // In other existing sessions distinct to this, WhatToSearch will remain unchanged
      DB_QueryUPDATE ("can not update type of search in user's last data",
		      "UPDATE usr_last"
		        " SET WhatToSearch=%u"
		      " WHERE UsrCod=%ld",
		      (unsigned) Gbl.Search.WhatToSearch,
		      Gbl.Usrs.Me.UsrDat.UsrCod);
     }
  }

/*****************************************************************************/
/*************** Write parameters for link to search courses *****************/
/*****************************************************************************/

void Sch_PutLinkToSearchCoursesParams (__attribute__((unused)) void *Args)
  {
   Sco_PutParamScope ("ScopeSch",Hie_Lvl_SYS);
   Par_PutHiddenParamUnsigned (NULL,"WhatToSearch",(unsigned) Sch_SEARCH_COURSES);
  }

