// swad_search.c: search for courses and teachers

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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
#include <string.h>	// For strcat...

#include "swad_database.h"
#include "swad_global.h"
#include "swad_layout.h"
#include "swad_parameter.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal constants ****************************/
/*****************************************************************************/

#define Sch_MAX_WORDS_IN_SEARCH		 10
#define Sch_MAX_LENGTH_SEARCH_WORD	255
#define Sch_MIN_LENGTH_LONGEST_WORD	  3
#define Sch_MAX_LENGTH_SEARCH_QUERY	(10*Sch_MAX_LENGTH_STRING_TO_FIND)

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

static void Sch_PutFormToSearchWithWhatToSearchAndScope (Act_Action_t Action,Sco_Scope_t DefaultScope);
static bool Sch_CheckIfIHavePermissionToSearch (Sch_WhatToSearch_t WhatToSearch);
static void Sch_GetParamSearch (char *SearchStr,size_t MaxLength);
static void Sch_SearchInDB (void);
static unsigned Sch_SearchInstitutionsInDB (const char *RangeQuery);
static unsigned Sch_SearchCentresInDB (const char *RangeQuery);
static unsigned Sch_SearchDegreesInDB (const char *RangeQuery);
static unsigned Sch_SearchCoursesInDB (const char *RangeQuery);
static unsigned Sch_SearchUsrsInDB (Rol_Role_t Role);
static unsigned Sch_SearchOpenDocumentsInDB (const char *RangeQuery);
static unsigned Sch_SearchDocumentsInMyCoursesInDB (const char *RangeQuery);
static unsigned Sch_SearchMyDocumentsInDB (const char *RangeQuery);
static bool Sch_BuildSearchQuery (char *SearchQuery,const char *FieldName,
                                  const char *CharSet,const char *Collate);

static void Sch_SaveLastSearchIntoSession (void);

/*****************************************************************************/
/*********************** Request search in system tab ************************/
/*****************************************************************************/

void Sch_ReqSysSearch (void)
  {
   /***** Select one of my courses *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<div align=\"center\">");
      Usr_PutFormToSelectMyCourses ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Search courses, teachers, documents... *****/
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActSysSch,Sco_SCOPE_PLATFORM);
  }

/*****************************************************************************/
/********************** Request search in country tab ************************/
/*****************************************************************************/

void Sch_ReqCtySearch (void)
  {
   /***** Select one of my courses *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<div align=\"center\">");
      Usr_PutFormToSelectMyCourses ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Search courses, teachers, documents... *****/
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActCtySch,Sco_SCOPE_COUNTRY);
  }

/*****************************************************************************/
/******************** Request search in institution tab **********************/
/*****************************************************************************/

void Sch_ReqInsSearch (void)
  {
   /***** Select one of my courses *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<div align=\"center\">");
      Usr_PutFormToSelectMyCourses ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Search courses, teachers, documents... *****/
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActInsSch,Sco_SCOPE_INSTITUTION);
  }

/*****************************************************************************/
/*********************** Request search in centre tab ************************/
/*****************************************************************************/

void Sch_ReqCtrSearch (void)
  {
   /***** Select one of my courses *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<div align=\"center\">");
      Usr_PutFormToSelectMyCourses ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Search courses, teachers, documents... *****/
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActCtrSch,Sco_SCOPE_CENTRE);
  }

/*****************************************************************************/
/*********************** Request search in degree tab ************************/
/*****************************************************************************/

void Sch_ReqDegSearch (void)
  {
   /***** Select one of my courses *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<div align=\"center\">");
      Usr_PutFormToSelectMyCourses ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Search courses, teachers, documents... *****/
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActDegSch,Sco_SCOPE_DEGREE);
  }

/*****************************************************************************/
/*********************** Request search in course tab ************************/
/*****************************************************************************/

void Sch_ReqCrsSearch (void)
  {
   /***** Select one of my courses *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<div align=\"center\">");
      Usr_PutFormToSelectMyCourses ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Search courses, teachers, documents... *****/
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActCrsSch,Sco_SCOPE_COURSE);
  }

/*****************************************************************************/
/****************** Put a form to search, including scope ********************/
/*****************************************************************************/

static void Sch_PutFormToSearchWithWhatToSearchAndScope (Act_Action_t Action,Sco_Scope_t DefaultScope)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Scope;
   extern const char *Txt_All;
   extern const char *Txt_Institutions;
   extern const char *Txt_Centres;
   extern const char *Txt_Degrees;
   extern const char *Txt_Courses;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_My_documents;
   extern const char *Txt_Documents_in_my_courses;
   extern const char *Txt_Open_documents;
   extern const char *Txt_Search;
   const char *Titles[Sch_NUM_WHAT_TO_SEARCH] =
     {
	Txt_All,						// Sch_SEARCH_ALL
	Txt_Institutions,					// Sch_SEARCH_INSTITUTIONS
	Txt_Centres,						// Sch_SEARCH_CENTRES
	Txt_Degrees,						// Sch_SEARCH_DEGREES
	Txt_Courses,						// Sch_SEARCH_COURSES
	Txt_ROLES_PLURAL_Abc[Rol_ROLE_TEACHER][Usr_SEX_UNKNOWN],// Sch_SEARCH_TEACHERS
	Txt_ROLES_PLURAL_Abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN],// Sch_SEARCH_STUDENTS
        Txt_ROLES_PLURAL_Abc[Rol_ROLE_GUEST  ][Usr_SEX_UNKNOWN],// Sch_SEARCH_GUESTS
	Txt_Open_documents,					// Sch_SEARCH_OPEN_DOCUMENTS
	Txt_Documents_in_my_courses,				// Sch_SEARCH_DOCUMENTS_IN_MY_COURSES
	Txt_My_documents,					// Sch_SEARCH_MY_DOCUMENTS
     };
   Sch_WhatToSearch_t WhatToSearch;

   /***** Form start *****/
   fprintf (Gbl.F.Out,"<div align=\"center\">");
   Act_FormStart (Action);
   Lay_StartRoundFrameTable10 (NULL,2,Txt_Search);
   fprintf (Gbl.F.Out,"<tr>"
		      "<td align=\"center\">");

   /***** Scope (whole platform, current centre, current degree or current course) *****/
   fprintf (Gbl.F.Out,"<div align=\"center\" class=\"%s\">%s: ",
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Scope);
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_PLATFORM    |
	               1 << Sco_SCOPE_COUNTRY     |
		       1 << Sco_SCOPE_INSTITUTION |
		       1 << Sco_SCOPE_CENTRE      |
		       1 << Sco_SCOPE_DEGREE      |
		       1 << Sco_SCOPE_COURSE;
   Gbl.Scope.Default = DefaultScope;
   Sco_GetScope ();
   Sco_PutSelectorScope (false);
   fprintf (Gbl.F.Out,"</div>");

   /***** What to search? *****/
   fprintf (Gbl.F.Out,"<select name=\"WhatToSearch\" style=\"width:150px;\">");
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
   fprintf (Gbl.F.Out,"</select>");

   /***** String to find *****/
   Sch_PutFormToSearchOnHead (Gbl.Prefs.IconsURL);

   /***** Send button *****/
   fprintf (Gbl.F.Out,"<br />"
                      "<input type=\"submit\" value=\"%s\" />",
            Txt_Search);

   /***** Form end *****/
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
   Lay_EndRoundFrameTable10 ();
   fprintf (Gbl.F.Out,"</form>"
	              "</div>");
  }

/*****************************************************************************/
/************* Check if I have permission to execute an action ***************/
/*****************************************************************************/

static bool Sch_CheckIfIHavePermissionToSearch (Sch_WhatToSearch_t WhatToSearch)
  {
   unsigned Permissions[Sch_NUM_WHAT_TO_SEARCH] =
     {
      0x1FF,	// Sch_SEARCH_ALL
      0x1FF,	// Sch_SEARCH_INSTITUTIONS
      0x1FF,	// Sch_SEARCH_CENTRES
      0x1FF,	// Sch_SEARCH_DEGREES
      0x1FF,	// Sch_SEARCH_COURSES
      0x1FF,	// Sch_SEARCH_TEACHERS
      0x100,	// Sch_SEARCH_STUDENTS			Only for superusers
      0x100,	// Sch_SEARCH_GUESTS			Only for superusers
      0x1FF,	// Sch_SEARCH_OPEN_DOCUMENTS
      0x1FE,	// Sch_SEARCH_DOCUMENTS_IN_MY_COURSES	Only if I am logged
      0x1FE,	// Sch_SEARCH_MY_DOCUMENTS		Only if I am logged
     };

   return (Permissions[WhatToSearch] & (1 << Gbl.Usrs.Me.LoggedRole));
  }

/*****************************************************************************/
/*********** Put form to search courses, teachers, documents... **************/
/*****************************************************************************/

void Sch_PutFormToSearchOnHead (const char *IconURL)
  {
   extern const char *Txt_Search;

   /***** String to find *****/
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Search\" size=\"30\" maxlength=\"%u\" value=\"%s\"",
	    Sch_MAX_LENGTH_STRING_TO_FIND,
            Gbl.Search.Str);
   if (!Gbl.Search.Str[0])
      fprintf (Gbl.F.Out," placeholder=\"%s...\"",
	       Txt_Search);
   fprintf (Gbl.F.Out," />");

   /***** Send button *****/
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/search16x16.gif\""
	              " alt=\"%s\" title=\"%s\""
	              " class=\"ICON16x16\" style=\"padding-top:3px;vertical-align:top;valign:top;\" />",
            IconURL,
            Txt_Search,
            Txt_Search);
  }

/*****************************************************************************/
/************* Get parameter "what to search" from search form ***************/
/*****************************************************************************/

static Sch_WhatToSearch_t Sch_GetParamWhatToSearch (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   /* Get what to search from form */
   Par_GetParToText ("WhatToSearch",UnsignedStr,10);

   if (sscanf (UnsignedStr,"%u",&UnsignedNum) != 1)
      return Sch_SEARCH_ALL;

   if (UnsignedNum < Sch_NUM_WHAT_TO_SEARCH)
      return (Sch_WhatToSearch_t) UnsignedNum;

   return Sch_SEARCH_ALL;
  }

/*****************************************************************************/
/*********************** Get string from search form *************************/
/*****************************************************************************/

static void Sch_GetParamSearch (char *SearchStr,size_t MaxLength)
  {
   /***** Get string to search *****/
   Par_GetParToText ("Search",SearchStr,MaxLength);
  }

/*****************************************************************************/
/************************* Get parameters to search **************************/
/*****************************************************************************/

void Sch_GetParamsSearch (void)
  {
   /***** What to search? *****/
   Gbl.Search.WhatToSearch = Sch_GetParamWhatToSearch ();

   /***** Get search string *****/
   Sch_GetParamSearch (Gbl.Search.Str,sizeof (Gbl.Search.Str) - 1);

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
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActSysSch,Sco_SCOPE_PLATFORM);

      /***** Show results of search *****/
      Sch_SearchInDB ();
     }
   else
      /***** Show search form and selectors *****/
      Sch_ReqSysSearch ();
  }

/*****************************************************************************/
/****************** Search courses, teachers, documents... *******************/
/*****************************************************************************/

void Sch_CtySearch (void)
  {
   if (Gbl.Search.Str[0])
     {
      /***** Show search form again *****/
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActCtySch,Sco_SCOPE_COUNTRY);

      /***** Show results of search *****/
      Sch_SearchInDB ();
     }
   else
      /***** Show search form and selectors *****/
      Sch_ReqCtySearch ();
  }

/*****************************************************************************/
/****************** Search courses, teachers, documents... *******************/
/*****************************************************************************/

void Sch_InsSearch (void)
  {
   if (Gbl.Search.Str[0])
     {
      /***** Show search form again *****/
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActInsSch,Sco_SCOPE_INSTITUTION);

      /***** Show results of search *****/
      Sch_SearchInDB ();
     }
   else
      /***** Show search form and selectors *****/
      Sch_ReqInsSearch ();
  }

/*****************************************************************************/
/****************** Search courses, teachers, documents... *******************/
/*****************************************************************************/

void Sch_CtrSearch (void)
  {
   if (Gbl.Search.Str[0])
     {
      /***** Show search form again *****/
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActCtrSch,Sco_SCOPE_CENTRE);

      /***** Show results of search *****/
      Sch_SearchInDB ();
     }
   else
      /***** Show search form and selectors *****/
      Sch_ReqCtrSearch ();
  }


/*****************************************************************************/
/****************** Search courses, teachers, documents... *******************/
/*****************************************************************************/

void Sch_DegSearch (void)
  {
   if (Gbl.Search.Str[0])
     {
      /***** Show search form again *****/
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActDegSch,Sco_SCOPE_DEGREE);

      /***** Show results of search *****/
      Sch_SearchInDB ();
     }
   else
      /***** Show search form and selectors *****/
      Sch_ReqDegSearch ();
  }

/*****************************************************************************/
/****************** Search courses, teachers, documents... *******************/
/*****************************************************************************/

void Sch_CrsSearch (void)
  {
   if (Gbl.Search.Str[0])
     {
      /***** Show search form again *****/
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActCrsSch,Sco_SCOPE_COURSE);

      /***** Show results of search *****/
      Sch_SearchInDB ();
     }
   else
      /***** Show search form and selectors *****/
      Sch_ReqCrsSearch ();
  }

/*****************************************************************************/
/**** Search institutions, centres, degrees, courses, teachers, documents ****/
/*****************************************************************************/

static void Sch_SearchInDB (void)
  {
   extern const char *Txt_No_results;
   char RangeQuery[256];
   unsigned NumResults = 0;	// Initialized to avoid warning

   /***** Select courses in all the degrees or in current degree *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_NONE:
	 // Not aplicable
      case Sco_SCOPE_PLATFORM:
         RangeQuery[0] = '\0';
         break;
      case Sco_SCOPE_COUNTRY:
         sprintf (RangeQuery," AND institutions.CtyCod='%ld'",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INSTITUTION:
         sprintf (RangeQuery," AND institutions.InsCod='%ld'",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CENTRE:
         sprintf (RangeQuery," AND centres.CtrCod='%ld'",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEGREE:
         sprintf (RangeQuery," AND degrees.DegCod='%ld'",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_COURSE:
         sprintf (RangeQuery," AND courses.CrsCod='%ld'",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
     }

   switch (Gbl.Search.WhatToSearch)
     {
      case Sch_SEARCH_ALL:
	 NumResults  = Sch_SearchInstitutionsInDB (RangeQuery);
	 NumResults += Sch_SearchCentresInDB (RangeQuery);
	 NumResults += Sch_SearchDegreesInDB (RangeQuery);
	 NumResults += Sch_SearchCoursesInDB (RangeQuery);
	 NumResults += Sch_SearchUsrsInDB (Rol_ROLE_TEACHER);
	 NumResults += Sch_SearchUsrsInDB (Rol_ROLE_STUDENT);
	 NumResults += Sch_SearchUsrsInDB (Rol_ROLE_GUEST);
	 NumResults += Sch_SearchOpenDocumentsInDB (RangeQuery);
	 NumResults += Sch_SearchDocumentsInMyCoursesInDB (RangeQuery);
	 NumResults += Sch_SearchMyDocumentsInDB (RangeQuery);
	 break;
      case Sch_SEARCH_INSTITUTIONS:
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
      case Sch_SEARCH_TEACHERS:
	 NumResults = Sch_SearchUsrsInDB (Rol_ROLE_TEACHER);
	 break;
      case Sch_SEARCH_STUDENTS:
	 NumResults = Sch_SearchUsrsInDB (Rol_ROLE_STUDENT);
	 break;
      case Sch_SEARCH_GUESTS:
	 NumResults = Sch_SearchUsrsInDB (Rol_ROLE_GUEST);
	 break;
      case Sch_SEARCH_OPEN_DOCUMENTS:
	 NumResults = Sch_SearchOpenDocumentsInDB (RangeQuery);
	 break;
      case Sch_SEARCH_DOCUMENTS_IN_MY_COURSES:
	 NumResults = Sch_SearchDocumentsInMyCoursesInDB (RangeQuery);
	 break;
      case Sch_SEARCH_MY_DOCUMENTS:
	 NumResults = Sch_SearchMyDocumentsInDB (RangeQuery);
	 break;
     }

   if (NumResults == 0)
      Lay_ShowAlert (Lay_INFO,Txt_No_results);
  }

/*****************************************************************************/
/********************** Search institutions in database **********************/
/*****************************************************************************/
// Returns number of institutions found

static unsigned Sch_SearchInstitutionsInDB (const char *RangeQuery)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   char SearchQuery[Sch_MAX_LENGTH_SEARCH_QUERY+1];
   char Query[1024+Sch_MAX_LENGTH_SEARCH_QUERY*2];

   /***** Check scope *****/
   if (Gbl.Scope.Current != Sco_SCOPE_CENTRE &&
       Gbl.Scope.Current != Sco_SCOPE_DEGREE &&
       Gbl.Scope.Current != Sco_SCOPE_COURSE)
      /***** Check user's permission *****/
      if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_INSTITUTIONS))
	 /***** Split institutions string into words *****/
	 if (Sch_BuildSearchQuery (SearchQuery,"institutions.FullName",NULL,NULL))
	   {
	    /***** Query database and list institutions found *****/
	    sprintf (Query,"SELECT institutions.InsCod"
			   " FROM institutions,countries"
			   " WHERE %s"
			   " AND institutions.CtyCod=countries.CtyCod"
			   "%s"
			   " ORDER BY institutions.FullName,countries.Name_%s",
		     SearchQuery,RangeQuery,
		     Txt_STR_LANG_ID[Gbl.Prefs.Language]);
	    return Ins_ListInssFound (Query);
	   }

   return 0;
  }

/*****************************************************************************/
/************************* Search centres in database ************************/
/*****************************************************************************/
// Returns number of centres found

static unsigned Sch_SearchCentresInDB (const char *RangeQuery)
  {
   char SearchQuery[Sch_MAX_LENGTH_SEARCH_QUERY+1];
   char Query[1024+Sch_MAX_LENGTH_SEARCH_QUERY*2];

   /***** Check scope *****/
   if (Gbl.Scope.Current != Sco_SCOPE_DEGREE &&
       Gbl.Scope.Current != Sco_SCOPE_COURSE)
      /***** Check user's permission *****/
      if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_CENTRES))
	 /***** Split centre string into words *****/
	 if (Sch_BuildSearchQuery (SearchQuery,"centres.FullName",NULL,NULL))
	   {
	    /***** Query database and list centres found *****/
	    sprintf (Query,"SELECT centres.CtrCod"
			   " FROM centres,institutions,countries"
			   " WHERE %s"
			   " AND centres.InsCod=institutions.InsCod"
			   " AND institutions.CtyCod=countries.CtyCod"
			   "%s"
			   " ORDER BY centres.FullName,institutions.FullName",
		     SearchQuery,RangeQuery);
	    return Ctr_ListCtrsFound (Query);
	   }

   return 0;
  }

/*****************************************************************************/
/************************* Search degrees in database ************************/
/*****************************************************************************/
// Returns number of degrees found

static unsigned Sch_SearchDegreesInDB (const char *RangeQuery)
  {
   char SearchQuery[Sch_MAX_LENGTH_SEARCH_QUERY+1];
   char Query[1024+Sch_MAX_LENGTH_SEARCH_QUERY*2];

   /***** Check scope *****/
   if (Gbl.Scope.Current != Sco_SCOPE_COURSE)
      /***** Check user's permission *****/
      if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_DEGREES))
	 /***** Split degree string into words *****/
	 if (Sch_BuildSearchQuery (SearchQuery,"degrees.FullName",NULL,NULL))
	   {
	    /***** Query database and list degrees found *****/
	    sprintf (Query,"SELECT degrees.DegCod"
			   " FROM degrees,centres,institutions,countries"
			   " WHERE %s"
			   " AND degrees.CtrCod=centres.CtrCod"
			   " AND centres.InsCod=institutions.InsCod"
			   " AND institutions.CtyCod=countries.CtyCod"
			   "%s"
			   " ORDER BY degrees.FullName,institutions.FullName",
		     SearchQuery,RangeQuery);
	    return Deg_ListDegsFound (Query);
	   }

   return 0;
  }

/*****************************************************************************/
/************************* Search courses in database ************************/
/*****************************************************************************/
// Returns number of courses found

static unsigned Sch_SearchCoursesInDB (const char *RangeQuery)
  {
   char SearchQuery[Sch_MAX_LENGTH_SEARCH_QUERY+1];
   char Query[1024+Sch_MAX_LENGTH_SEARCH_QUERY*2];

   /***** Check user's permission *****/
   if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_COURSES))
      /***** Split course string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,"courses.FullName",NULL,NULL))
	{
	 /***** Query database and list courses found *****/
	 sprintf (Query,"SELECT degrees.DegCod,courses.CrsCod,degrees.Logo,degrees.ShortName,degrees.FullName,"
			"courses.Year,courses.Semester,courses.FullName,centres.ShortName"
			" FROM courses,degrees,centres,institutions,countries"
			" WHERE %s"
			" AND courses.DegCod=degrees.DegCod"
			" AND degrees.CtrCod=centres.CtrCod"
			" AND centres.InsCod=institutions.InsCod"
			" AND institutions.CtyCod=countries.CtyCod"
			"%s"
			" ORDER BY courses.FullName,institutions.FullName,degrees.FullName,courses.Year,courses.Semester",
		  SearchQuery,RangeQuery);
	 return Crs_ListCrssFound (Query);
	}

   return 0;
  }

/*****************************************************************************/
/************************* Search teachers in database ***********************/
/*****************************************************************************/
// Returns number of teachers found

static unsigned Sch_SearchUsrsInDB (Rol_Role_t Role)
  {
   char SearchQuery[Sch_MAX_LENGTH_SEARCH_QUERY+1];

   /***** Check user's permission *****/
   if (Sch_CheckIfIHavePermissionToSearch ( Role == Rol_ROLE_TEACHER ? Sch_SEARCH_TEACHERS :
		                           (Role == Rol_ROLE_STUDENT ? Sch_SEARCH_STUDENTS :
		                        		               Sch_SEARCH_GUESTS)))
      /***** Split user string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,
				"CONCAT_WS(' ',usr_data.FirstName,usr_data.Surname1,usr_data.Surname2)",
				NULL,NULL))
	 /***** Query database and list users found *****/
	 return Usr_ListUsrsFound (Role,SearchQuery);

   return 0;
  }

/*****************************************************************************/
/********************** Search open documents in database ********************/
/*****************************************************************************/
// Returns number of documents found

static unsigned Sch_SearchOpenDocumentsInDB (const char *RangeQuery)
  {
   extern const char *Txt_Open_documents;
   char SearchQuery[Sch_MAX_LENGTH_SEARCH_QUERY+1];
   char Query[1024+Sch_MAX_LENGTH_SEARCH_QUERY*2];

   /***** Check user's permission *****/
   if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_OPEN_DOCUMENTS))
      /***** Split document string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,"SUBSTRING_INDEX(files.Path,'/',-1)",
				"_latin1 "," COLLATE latin1_general_ci"))
	{
	 /***** Build the query *****/
	 sprintf (Query,"SELECT files.FilCod,"
			"SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
			"degrees.DegCod,degrees.Logo,degrees.ShortName,"
			"centres.ShortName,courses.ShortName"
			" FROM files,courses,degrees,centres,institutions,countries"
			" WHERE files.Public='Y' AND %s"
			" AND files.CrsCod=courses.CrsCod"
			" AND courses.DegCod=degrees.DegCod"
			" AND degrees.CtrCod=centres.CtrCod"
			" AND centres.InsCod=institutions.InsCod"
			" AND institutions.CtyCod=countries.CtyCod"
			"%s"
			" HAVING PathFromRoot<>''"
			" ORDER BY degrees.ShortName,courses.ShortName,PathFromRoot",
		  SearchQuery,
		  RangeQuery);

	 /***** Query database and list documents found *****/
	 /* if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
	    Lay_ShowAlert (Lay_INFO,Query); */
	 return Brw_ListDocsFound (Query,Txt_Open_documents);
	}

   return 0;
  }

/*****************************************************************************/
/**************** Search documents in my courses in database *****************/
/*****************************************************************************/

static unsigned Sch_SearchDocumentsInMyCoursesInDB (const char *RangeQuery)
  {
   extern const char *Txt_Documents_in_my_courses;
   char SearchQuery[Sch_MAX_LENGTH_SEARCH_QUERY+1];
   char Query[1024+Sch_MAX_LENGTH_SEARCH_QUERY*2];
   unsigned NumDocs;

   /***** Check user's permission *****/
   if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_DOCUMENTS_IN_MY_COURSES))
      /***** Split document string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,"SUBSTRING_INDEX(files.Path,'/',-1)",
				"_latin1 "," COLLATE latin1_general_ci"))
	{
	 /***** Create temporary table with codes of files in documents and shared areas accessible by me.
		It is necessary to speed up the second query *****/
	 sprintf (Query,"CREATE TEMPORARY TABLE my_files (FilCod INT NOT NULL,UNIQUE INDEX(FilCod)) ENGINE=MEMORY"
			" SELECT files.FilCod FROM crs_usr,files"
			" WHERE crs_usr.UsrCod='%ld' AND crs_usr.CrsCod=files.CrsCod"
			" AND files.FileBrowser IN ('%u','%u','%u')"
			" UNION"
			" SELECT files.FilCod FROM crs_grp_usr,files"
			" WHERE crs_grp_usr.UsrCod='%ld' AND crs_grp_usr.GrpCod=files.GrpCod"
			" AND files.FileBrowser IN ('%u','%u','%u')",
		  Gbl.Usrs.Me.UsrDat.UsrCod,
		  (unsigned) Brw_FILE_BRW_ADMIN_DOCUMENTS_CRS,
		  (unsigned) Brw_FILE_BRW_COMMON_CRS,
		  (unsigned) Brw_FILE_BRW_ADMIN_MARKS_CRS,
		  Gbl.Usrs.Me.UsrDat.UsrCod,
		  (unsigned) Brw_FILE_BRW_ADMIN_DOCUMENTS_GRP,
		  (unsigned) Brw_FILE_BRW_COMMON_GRP,
		  (unsigned) Brw_FILE_BRW_ADMIN_MARKS_GRP);
	 /* if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
	    Lay_ShowAlert (Lay_INFO,Query); */
	 if (mysql_query (&Gbl.mysql,Query))
	    DB_ExitOnMySQLError ("can not create temporary table");

	 /***** Build the query *****/
	 sprintf (Query,"SELECT files.FilCod,"
			"SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
			"degrees.DegCod,degrees.Logo,degrees.ShortName,"
			"centres.ShortName,courses.ShortName"
			" FROM files,courses,degrees,centres,institutions,countries"
			" WHERE files.FilCod IN (SELECT FilCod FROM my_files) AND %s"
			" AND files.CrsCod=courses.CrsCod"
			" AND courses.DegCod=degrees.DegCod"
			" AND degrees.CtrCod=centres.CtrCod"
			" AND centres.InsCod=institutions.InsCod"
			" AND institutions.CtyCod=countries.CtyCod"
			"%s"
			" HAVING PathFromRoot<>''"
			" ORDER BY degrees.ShortName,courses.ShortName,PathFromRoot",
		  SearchQuery,
		  RangeQuery);

	 /***** Query database and list documents found *****/
	 /* if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
	    Lay_ShowAlert (Lay_INFO,Query); */
	 NumDocs = Brw_ListDocsFound (Query,Txt_Documents_in_my_courses);

	 /***** Drop temporary table *****/
	 sprintf (Query,"DROP TABLE IF EXISTS my_files");
	 if (mysql_query (&Gbl.mysql,Query))
	    DB_ExitOnMySQLError ("can not remove temporary table");

	 return NumDocs;
	}

   return 0;
  }

/*****************************************************************************/
/********************** Search my documents in database **********************/
/*****************************************************************************/

static unsigned Sch_SearchMyDocumentsInDB (const char *RangeQuery)
  {
   extern const char *Txt_My_documents;
   char SearchQuery[Sch_MAX_LENGTH_SEARCH_QUERY+1];
   char Query[1024+Sch_MAX_LENGTH_SEARCH_QUERY*2];

   /***** Check user's permission *****/
   if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_MY_DOCUMENTS))
      /***** Split document string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,"SUBSTRING_INDEX(files.Path,'/',-1)",
				"_latin1 "," COLLATE latin1_general_ci"))
	{
	 /***** Build the query *****/
	 if (Gbl.Scope.Current == Sco_SCOPE_PLATFORM)
	    /* Show also documents in private zone */
	    sprintf (Query,"SELECT * FROM "
			   "("
			   "SELECT files.FilCod,"
			   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
			   "degrees.DegCod,degrees.Logo,degrees.ShortName AS DegShortName,"
			   "centres.ShortName,courses.ShortName AS CrsShortName"
			   " FROM files,courses,degrees,centres,institutions,countries"
			   " WHERE files.PublisherUsrCod='%ld' AND %s"
			   " AND files.CrsCod=courses.CrsCod"
			   " AND courses.DegCod=degrees.DegCod"
			   " AND degrees.CtrCod=centres.CtrCod"
			   " AND centres.InsCod=institutions.InsCod"
			   " AND institutions.CtyCod=countries.CtyCod"
			   "%s"
			   " UNION "
			   "SELECT files.FilCod,"
			   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
			   "'-1','','' AS DegShortName,'','' AS CrsShortName"
			   " FROM files"
			   " WHERE files.PublisherUsrCod='%ld' AND files.FileBrowser='%u' AND %s"
			   ") AS my_files"
			   " WHERE PathFromRoot<>''"
			   " ORDER BY DegShortName,CrsShortName,PathFromRoot",
		     Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,RangeQuery,
		     Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) Brw_FILE_BRW_BRIEFCASE_USR,SearchQuery);
	 else
	    sprintf (Query,"SELECT files.FilCod,"
			   "SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
			   "degrees.DegCod,degrees.Logo,degrees.ShortName,"
			   "centres.ShortName,courses.ShortName"
			   " FROM files,courses,degrees,centres,institutions,countries"
			   " WHERE files.PublisherUsrCod='%ld' AND %s"
			   " AND files.CrsCod=courses.CrsCod"
			   " AND courses.DegCod=degrees.DegCod"
			   " AND degrees.CtrCod=centres.CtrCod"
			   " AND centres.InsCod=institutions.InsCod"
			   " AND institutions.CtyCod=countries.CtyCod"
			   "%s"
			   " HAVING PathFromRoot<>''"
			   " ORDER BY degrees.ShortName,courses.ShortName,PathFromRoot",
		     Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,
		     RangeQuery);

	 /***** Query database and list documents found *****/
	 /* if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
	    Lay_ShowAlert (Lay_INFO,Query); */
	 return Brw_ListDocsFound (Query,Txt_My_documents);
	}

   return 0;
  }

/*****************************************************************************/
/****** Build a search query by splitting a string to search into words ******/
/*****************************************************************************/
// Returns true if a valid search query is biult
// Returns false when no valid search query

static bool Sch_BuildSearchQuery (char *SearchQuery,const char *FieldName,
                                  const char *CharSet,const char *Collate)
  {
   extern const char *Txt_The_search_term_must_be_longer;
   static bool WarningMessageWritten = false;
   const char *Ptr;
   unsigned NumWords;
   size_t LengthWord;
   size_t MaxLengthWord = 0;
   char SearchWord[Sch_MAX_LENGTH_SEARCH_WORD+1];
   bool SearchWordIsValid = true;

   SearchQuery[0] = '\0';
   Ptr = Gbl.Search.Str;
   for (NumWords = 0;
	NumWords < Sch_MAX_WORDS_IN_SEARCH && *Ptr;
	NumWords++)
     {
      /* Get next word */
      Str_GetNextStringUntilSpace (&Ptr,SearchWord,Sch_MAX_LENGTH_SEARCH_WORD);

      /* Is this word valid? */
      switch (Gbl.Search.WhatToSearch)
        {
	 case Sch_SEARCH_OPEN_DOCUMENTS:
	 case Sch_SEARCH_DOCUMENTS_IN_MY_COURSES:
	 case Sch_SEARCH_MY_DOCUMENTS:
	    SearchWordIsValid = Str_ConvertFilFolLnkNameToValid (SearchWord);
	    break;
	 default:
	    SearchWordIsValid = true;
	    break;
        }

      /* Concatenate word to search string */
      if (SearchWordIsValid)
	{
	 LengthWord = strlen (SearchWord);
	 if (LengthWord > MaxLengthWord)
	    MaxLengthWord = LengthWord;
	 if (strlen (SearchQuery) + LengthWord + 512 > Sch_MAX_LENGTH_SEARCH_QUERY)	// Prevent string overflow
	    break;
	 if (NumWords)
	    strcat (SearchQuery," AND ");
	 strcat (SearchQuery,FieldName);
	 strcat (SearchQuery," LIKE ");
	 if (CharSet)
	    if (CharSet[0])
	       strcat (SearchQuery,CharSet);
	 strcat (SearchQuery,"'%");
	 strcat (SearchQuery,SearchWord);
	 strcat (SearchQuery,"%'");
	 if (Collate)
	    if (Collate[0])
	       strcat (SearchQuery,Collate);
	}
     }

   /***** If search string valid? *****/
   if (MaxLengthWord >= Sch_MIN_LENGTH_LONGEST_WORD)
      return true;
   else	// Too short
     {
      if (!WarningMessageWritten)	// To avoid repetitions
	{
         Lay_ShowAlert (Lay_WARNING,Txt_The_search_term_must_be_longer);
         WarningMessageWritten = true;
	}
      return false;
     }
  }

/*****************************************************************************/
/********************** Save last search into session ************************/
/*****************************************************************************/

static void Sch_SaveLastSearchIntoSession (void)
  {
   char Query[512];

   if (Gbl.Usrs.Me.Logged)
     {
      /***** Save last search in session *****/
      sprintf (Query,"UPDATE sessions SET WhatToSearch='%u',SearchString='%s'"
		     " WHERE SessionId='%s'",
	       (unsigned) Gbl.Search.WhatToSearch,
	       Gbl.Search.Str,
	       Gbl.Session.Id);
      DB_QueryUPDATE (Query,"can not update last search in session");

      /***** Update my last type of search *****/
      // WhatToSearch is stored in usr_last for next time I log in
      // In other existing sessions distinct to this, WhatToSearch will remain unchanged
      sprintf (Query,"UPDATE usr_last SET WhatToSearch='%u'"
		     " WHERE UsrCod='%ld'",
	       (unsigned) Gbl.Search.WhatToSearch,
	       Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE (Query,"can not update type of search in user's last data");
     }
  }
