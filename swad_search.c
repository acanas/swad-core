// swad_search.c: search for courses and teachers

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Crs_PutFormToSelectMyCourses ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Search courses, teachers, documents... *****/
   Sch_GetParamWhatToSearch ();
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActSysSch,Sco_SCOPE_SYS);
  }

/*****************************************************************************/
/********************** Request search in country tab ************************/
/*****************************************************************************/

void Sch_ReqCtySearch (void)
  {
   /***** Select one of my courses *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Crs_PutFormToSelectMyCourses ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Search courses, teachers, documents... *****/
   Sch_GetParamWhatToSearch ();
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActCtySch,Sco_SCOPE_CTY);
  }

/*****************************************************************************/
/******************** Request search in institution tab **********************/
/*****************************************************************************/

void Sch_ReqInsSearch (void)
  {
   /***** Select one of my courses *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Crs_PutFormToSelectMyCourses ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Search courses, teachers, documents... *****/
   Sch_GetParamWhatToSearch ();
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActInsSch,Sco_SCOPE_INS);
  }

/*****************************************************************************/
/*********************** Request search in centre tab ************************/
/*****************************************************************************/

void Sch_ReqCtrSearch (void)
  {
   /***** Select one of my courses *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Crs_PutFormToSelectMyCourses ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Search courses, teachers, documents... *****/
   Sch_GetParamWhatToSearch ();
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActCtrSch,Sco_SCOPE_CTR);
  }

/*****************************************************************************/
/*********************** Request search in degree tab ************************/
/*****************************************************************************/

void Sch_ReqDegSearch (void)
  {
   /***** Select one of my courses *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Crs_PutFormToSelectMyCourses ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Search courses, teachers, documents... *****/
   Sch_GetParamWhatToSearch ();
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActDegSch,Sco_SCOPE_DEG);
  }

/*****************************************************************************/
/*********************** Request search in course tab ************************/
/*****************************************************************************/

void Sch_ReqCrsSearch (void)
  {
   /***** Select one of my courses *****/
   if (Gbl.Usrs.Me.Logged)
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Crs_PutFormToSelectMyCourses ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Search courses, teachers, documents... *****/
   Sch_GetParamWhatToSearch ();
   Sch_PutFormToSearchWithWhatToSearchAndScope (ActCrsSch,Sco_SCOPE_CRS);
  }

/*****************************************************************************/
/****************** Put a form to search, including scope ********************/
/*****************************************************************************/

static void Sch_PutFormToSearchWithWhatToSearchAndScope (Act_Action_t Action,Sco_Scope_t DefaultScope)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Scope;
   extern const char *Txt_SEARCH_X_in_Y;
   extern const char *Txt_all;
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
	Txt_all,						// Sch_SEARCH_ALL
	Txt_institutions,					// Sch_SEARCH_INSTITUTIONS
	Txt_centres,						// Sch_SEARCH_CENTRES
	Txt_degrees,						// Sch_SEARCH_DEGREES
	Txt_courses,						// Sch_SEARCH_COURSES
	Txt_users[Usr_SEX_UNKNOWN],				// Sch_SEARCH_USERS
	Txt_ROLES_PLURAL_abc[Rol_TEACHER][Usr_SEX_UNKNOWN],	// Sch_SEARCH_TEACHERS
	Txt_ROLES_PLURAL_abc[Rol_STUDENT][Usr_SEX_UNKNOWN],	// Sch_SEARCH_STUDENTS
        Txt_ROLES_PLURAL_abc[Rol__GUEST_][Usr_SEX_UNKNOWN],	// Sch_SEARCH_GUESTS
	Txt_open_documents,					// Sch_SEARCH_OPEN_DOCUMENTS
	Txt_documents_in_my_courses,				// Sch_SEARCH_DOCUM_IN_MY_COURSES
	Txt_my_documents,					// Sch_SEARCH_MY_DOCUMENTS
     };
   Sch_WhatToSearch_t WhatToSearch;

   /***** Start form *****/
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
   Act_FormStart (Action);
   Lay_StartRoundFrame (NULL,Txt_Search,NULL);

   /***** Scope (whole platform, current country, current institution,
                 current centre, current degree or current course) *****/
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">"
	              "<label class=\"%s\">%s: </label>",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Scope);
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	               1 << Sco_SCOPE_CTY |
		       1 << Sco_SCOPE_INS |
		       1 << Sco_SCOPE_CTR |
		       1 << Sco_SCOPE_DEG |
		       1 << Sco_SCOPE_CRS;
   Gbl.Scope.Default = DefaultScope;
   Sco_GetScope ("ScopeSch");
   Sco_PutSelectorScope ("ScopeSch",false);
   fprintf (Gbl.F.Out,"</div>");

   /***** String to find *****/
   Sch_PutInputStringToSearch (NULL);

   /***** What to search? *****/
   fprintf (Gbl.F.Out,"<span class=\"%s\"> %s </span>",
            The_ClassForm[Gbl.Prefs.Theme],Txt_SEARCH_X_in_Y);
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
   fprintf (Gbl.F.Out,"</select>");

   /***** Magnifying glass icon *****/
   Sch_PutMagnifyingGlassButton ("search64x64.png");

   /***** Send button and end frame *****/
   Lay_EndRoundFrameWithButton (Lay_CONFIRM_BUTTON,Txt_Search);

   /***** End form *****/
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
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
      0x1FF,	// Sch_SEARCH_USERS
      0x1FF,	// Sch_SEARCH_TEACHERS
      0x1FF,	// Sch_SEARCH_STUDENTS
      0x1FF,	// Sch_SEARCH_GUESTS
      0x1FF,	// Sch_SEARCH_OPEN_DOCUMENTS
      0x1FE,	// Sch_SEARCH_DOCUM_IN_MY_COURSES	Only if I am logged
      0x1FE,	// Sch_SEARCH_MY_DOCUMENTS		Only if I am logged
     };

   return (Permissions[WhatToSearch] & (1 << Gbl.Usrs.Me.LoggedRole));
  }

/*****************************************************************************/
/**************** Put a form to search in page top heading *******************/
/*****************************************************************************/

void Sch_PutFormToSearchInPageTopHeading (void)
  {
   Act_Action_t ActionSearch;

   fprintf (Gbl.F.Out,"<div id=\"head_row_1_search\">");
   Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	               1 << Sco_SCOPE_CTY |
		       1 << Sco_SCOPE_INS |
		       1 << Sco_SCOPE_CTR |
		       1 << Sco_SCOPE_DEG |
		       1 << Sco_SCOPE_CRS;
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      ActionSearch = ActCrsSch;
      Gbl.Scope.Default = Sco_SCOPE_CRS;
     }
   else if (Gbl.CurrentDeg.Deg.DegCod > 0)
     {
      ActionSearch = ActDegSch;
      Gbl.Scope.Default = Sco_SCOPE_DEG;
     }
   else if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
     {
      ActionSearch = ActCtrSch;
      Gbl.Scope.Default = Sco_SCOPE_CTR;
     }
   else if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      ActionSearch = ActInsSch;
      Gbl.Scope.Default = Sco_SCOPE_INS;
     }
   else if (Gbl.CurrentCty.Cty.CtyCod > 0)
     {
      ActionSearch = ActCtySch;
      Gbl.Scope.Default = Sco_SCOPE_CTY;
     }
   else
     {
      ActionSearch = ActSysSch;
      Gbl.Scope.Default = Sco_SCOPE_SYS;
     }
   Act_FormStart (ActionSearch);
   Sco_GetScope ("ScopeSch");
   Sco_PutParamScope ("ScopeSch",Gbl.Scope.Current);
   Sch_PutInputStringToSearch ("head_search_text");
   Sch_PutMagnifyingGlassButton ("search-white64x64.png");
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");	// head_row_1_search
  }

/*****************************************************************************/
/********************* Put string to search inside form **********************/
/*****************************************************************************/

void Sch_PutInputStringToSearch (const char *IdInputText)
  {
   extern const char *Txt_Search;

   /***** String to find *****/
   fprintf (Gbl.F.Out,"<input");
   if (IdInputText)
      fprintf (Gbl.F.Out," id=\"%s\"",IdInputText);
   fprintf (Gbl.F.Out," type=\"text\" name=\"Search\""
	              " size=\"20\" maxlength=\"%u\" value=\"%s\"",
	    Sch_MAX_LENGTH_STRING_TO_FIND,
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
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s\""
	              " alt=\"%s\" title=\"%s\""
	              " class=\"ICON20x20\" />",
            Gbl.Prefs.IconsURL,
	    Icon,
            Txt_Search,
            Txt_Search);
  }

/*****************************************************************************/
/************* Get parameter "what to search" from search form ***************/
/*****************************************************************************/

void Sch_GetParamWhatToSearch (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   /***** Get what to search from form *****/
   Par_GetParToText ("WhatToSearch",UnsignedStr,10);

   // If parameter WhatToSearch is not present, use parameter from session
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Sch_NUM_WHAT_TO_SEARCH)
         Gbl.Search.WhatToSearch = (Sch_WhatToSearch_t) UnsignedNum;
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
   Sch_GetParamWhatToSearch ();

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
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActSysSch,Sco_SCOPE_SYS);

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
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActCtySch,Sco_SCOPE_CTY);

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
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActInsSch,Sco_SCOPE_INS);

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
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActCtrSch,Sco_SCOPE_CTR);

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
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActDegSch,Sco_SCOPE_DEG);

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
      Sch_PutFormToSearchWithWhatToSearchAndScope (ActCrsSch,Sco_SCOPE_CRS);

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
      case Sco_SCOPE_UNK:
	 // Not aplicable
      case Sco_SCOPE_SYS:
         RangeQuery[0] = '\0';
         break;
      case Sco_SCOPE_CTY:
         sprintf (RangeQuery," AND institutions.CtyCod='%ld'",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         sprintf (RangeQuery," AND institutions.InsCod='%ld'",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (RangeQuery," AND centres.CtrCod='%ld'",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (RangeQuery," AND degrees.DegCod='%ld'",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
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
	 NumResults += Sch_SearchUsrsInDB (Rol_TEACHER);
	 NumResults += Sch_SearchUsrsInDB (Rol_STUDENT);
	 NumResults += Sch_SearchUsrsInDB (Rol__GUEST_);
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
      case Sch_SEARCH_USERS:
	 NumResults = Sch_SearchUsrsInDB (Rol_UNKNOWN);	// Here Rol_UNKNOWN means any user
	 break;
      case Sch_SEARCH_TEACHERS:
	 NumResults = Sch_SearchUsrsInDB (Rol_TEACHER);
	 break;
      case Sch_SEARCH_STUDENTS:
	 NumResults = Sch_SearchUsrsInDB (Rol_STUDENT);
	 break;
      case Sch_SEARCH_GUESTS:
	 NumResults = Sch_SearchUsrsInDB (Rol__GUEST_);
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
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];
   char SearchQuery[Sch_MAX_LENGTH_SEARCH_QUERY+1];
   char Query[1024+Sch_MAX_LENGTH_SEARCH_QUERY*2];

   /***** Check scope *****/
   if (Gbl.Scope.Current != Sco_SCOPE_CTR &&
       Gbl.Scope.Current != Sco_SCOPE_DEG &&
       Gbl.Scope.Current != Sco_SCOPE_CRS)
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
   if (Gbl.Scope.Current != Sco_SCOPE_DEG &&
       Gbl.Scope.Current != Sco_SCOPE_CRS)
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
   if (Gbl.Scope.Current != Sco_SCOPE_CRS)
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
	 sprintf (Query,"SELECT degrees.DegCod,courses.CrsCod,degrees.ShortName,degrees.FullName,"
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
   if (Sch_CheckIfIHavePermissionToSearch ( Role == Rol_TEACHER ? Sch_SEARCH_TEACHERS :
		                           (Role == Rol_STUDENT ? Sch_SEARCH_STUDENTS :
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
   extern const char *Txt_open_document;
   extern const char *Txt_open_documents;
   char SearchQuery[Sch_MAX_LENGTH_SEARCH_QUERY+1];
   char Query[(512+Sch_MAX_LENGTH_SEARCH_QUERY)*4];

   /***** Check user's permission *****/
   if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_OPEN_DOCUMENTS))
      /***** Split document string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,"SUBSTRING_INDEX(files.Path,'/',-1)",
				"_latin1 "," COLLATE latin1_general_ci"))
	{
	 /***** Build the query *****/
	 sprintf (Query,"SELECT * FROM "
			"("
			"SELECT files.FilCod,"	// Institution
			"SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
			"institutions.InsCod,institutions.ShortName AS InsShortName,"
			"'-1' AS CtrCod,'' AS CtrShortName,"
			"'-1' AS DegCod,'' AS DegShortName,"
			"'-1' AS CrsCod,'' AS CrsShortName,"
	                "'-1' AS GrpCod"
			" FROM files,courses,degrees,centres,institutions,countries"
			" WHERE files.Public='Y' AND %s"
			" AND files.FileBrowser IN ('%u','%u')"
			" AND files.Cod=institutions.InsCod"
			" AND institutions.CtyCod=countries.CtyCod"
			"%s"
			" UNION "
			"SELECT files.FilCod,"	// Centre
			"SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
			"institutions.InsCod,institutions.ShortName AS InsShortName,"
			"centres.CtrCod,centres.ShortName AS CtrShortName,"
			"'-1' AS DegCod,'' AS DegShortName,"
			"'-1' AS CrsCod,'' AS CrsShortName,"
	                "'-1' AS GrpCod"
			" FROM files,courses,degrees,centres,institutions,countries"
			" WHERE files.Public='Y' AND %s"
			" AND files.FileBrowser IN ('%u','%u')"
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
			"'-1','' AS CrsShortName,"
	                "'-1'"
			" FROM files,courses,degrees,centres,institutions,countries"
			" WHERE files.Public='Y' AND %s"
			" AND files.FileBrowser IN ('%u','%u')"
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
	                "'-1'"
			" FROM files,courses,degrees,centres,institutions,countries"
			" WHERE files.Public='Y' AND %s"
	                " AND files.FileBrowser IN ('%u','%u')"
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
		  (unsigned) Brw_ADMI_DOCUM_INS,
		  (unsigned) Brw_ADMI_SHARE_INS,
		  RangeQuery,
		  SearchQuery,
		  (unsigned) Brw_ADMI_DOCUM_CTR,
		  (unsigned) Brw_ADMI_SHARE_CTR,
		  RangeQuery,
		  SearchQuery,
		  (unsigned) Brw_ADMI_DOCUM_DEG,
		  (unsigned) Brw_ADMI_SHARE_DEG,
		  RangeQuery,
		  SearchQuery,
		  (unsigned) Brw_ADMI_DOCUM_CRS,
		  (unsigned) Brw_ADMI_SHARE_CRS,
		  RangeQuery);

	 /***** Query database and list documents found *****/
	 /* if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
	    Lay_ShowAlert (Lay_INFO,Query); */
	 return Brw_ListDocsFound (Query,
	                           Txt_open_document,
	                           Txt_open_documents);
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
   char SearchQuery[Sch_MAX_LENGTH_SEARCH_QUERY+1];
   char Query[(1024+Sch_MAX_LENGTH_SEARCH_QUERY)*2];
   unsigned NumDocs;

   /***** Check user's permission *****/
   if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_DOCUM_IN_MY_COURSES))
      /***** Split document string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,"SUBSTRING_INDEX(files.Path,'/',-1)",
				"_latin1 "," COLLATE latin1_general_ci"))
	{
	 /***** Create temporary table with codes of files in documents and shared areas accessible by me.
		It is necessary to speed up the second query *****/
	 sprintf (Query,"DROP TEMPORARY TABLE IF EXISTS my_files_crs,my_files_grp");
	 if (mysql_query (&Gbl.mysql,Query))
	    DB_ExitOnMySQLError ("can not remove temporary table");

	 sprintf (Query,"CREATE TEMPORARY TABLE my_files_crs"
	                " (FilCod INT NOT NULL,UNIQUE INDEX(FilCod))"
	                " ENGINE=MEMORY"
			" SELECT files.FilCod FROM crs_usr,files"
			" WHERE crs_usr.UsrCod='%ld'"
			" AND crs_usr.CrsCod=files.Cod"
			" AND files.FileBrowser IN ('%u','%u','%u','%u')",
		  Gbl.Usrs.Me.UsrDat.UsrCod,
		  (unsigned) Brw_ADMI_DOCUM_CRS,
		  (unsigned) Brw_ADMI_TEACH_CRS,
		  (unsigned) Brw_ADMI_SHARE_CRS,
		  (unsigned) Brw_ADMI_MARKS_CRS);
	 if (mysql_query (&Gbl.mysql,Query))
	    DB_ExitOnMySQLError ("can not create temporary table");

	 sprintf (Query,"CREATE TEMPORARY TABLE my_files_grp"
	                " (FilCod INT NOT NULL,UNIQUE INDEX(FilCod))"
	                " ENGINE=MEMORY"
			" SELECT files.FilCod FROM crs_grp_usr,files"
			" WHERE crs_grp_usr.UsrCod='%ld'"
			" AND crs_grp_usr.GrpCod=files.Cod"
			" AND files.FileBrowser IN ('%u','%u','%u','%u')",
		  Gbl.Usrs.Me.UsrDat.UsrCod,
		  (unsigned) Brw_ADMI_DOCUM_GRP,
		  (unsigned) Brw_ADMI_TEACH_GRP,
		  (unsigned) Brw_ADMI_SHARE_GRP,
		  (unsigned) Brw_ADMI_MARKS_GRP);
	 /* if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
	    Lay_ShowAlert (Lay_INFO,Query); */
	 if (mysql_query (&Gbl.mysql,Query))
	    DB_ExitOnMySQLError ("can not create temporary table");

	 /***** Build the query *****/
	 sprintf (Query,"SELECT * FROM "
			"("
	                "SELECT files.FilCod,"
			"SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
			"institutions.InsCod,institutions.ShortName AS InsShortName,"
			"centres.CtrCod,centres.ShortName AS CtrShortName,"
			"degrees.DegCod,degrees.ShortName AS DegShortName,"
			"courses.CrsCod,courses.ShortName AS CrsShortName,"
	                "'-1' AS GrpCod"
			" FROM files,courses,degrees,centres,institutions,countries"
			" WHERE files.FilCod IN (SELECT FilCod FROM my_files_crs) AND %s"
	                " AND files.FileBrowser IN ('%u','%u','%u','%u')"
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
	                " AND files.FileBrowser IN ('%u','%u','%u','%u')"
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
		  (unsigned) Brw_ADMI_DOCUM_CRS,
		  (unsigned) Brw_ADMI_TEACH_CRS,
		  (unsigned) Brw_ADMI_SHARE_CRS,
		  (unsigned) Brw_ADMI_MARKS_CRS,
		  RangeQuery,
		  SearchQuery,
		  (unsigned) Brw_ADMI_DOCUM_GRP,
		  (unsigned) Brw_ADMI_TEACH_GRP,
		  (unsigned) Brw_ADMI_SHARE_GRP,
		  (unsigned) Brw_ADMI_MARKS_GRP,
		  RangeQuery);

	 /***** Query database and list documents found *****/
	 /* if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
	    Lay_ShowAlert (Lay_INFO,Query); */
	 NumDocs = Brw_ListDocsFound (Query,
	                              Txt_document_in_my_courses,
	                              Txt_documents_in_my_courses);

	 /***** Drop temporary table *****/
	 sprintf (Query,"DROP TEMPORARY TABLE IF EXISTS my_files_crs,my_files_grp");
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
   extern const char *Txt_document_from_me;
   extern const char *Txt_documents_from_me;
   char SearchQuery[Sch_MAX_LENGTH_SEARCH_QUERY+1];
   char Query[(512+Sch_MAX_LENGTH_SEARCH_QUERY)*5];

   /***** Check user's permission *****/
   if (Sch_CheckIfIHavePermissionToSearch (Sch_SEARCH_MY_DOCUMENTS))
      /***** Split document string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,"SUBSTRING_INDEX(files.Path,'/',-1)",
				"_latin1 "," COLLATE latin1_general_ci"))
	{
	 /***** Build the query *****/
	 sprintf (Query,"SELECT * FROM "
			"("
			"SELECT files.FilCod,"	// Institution
			"SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
			"institutions.InsCod,institutions.ShortName AS InsShortName,"
			"'-1' AS CtrCod,'' AS CtrShortName,"
			"'-1' AS DegCod,'' AS DegShortName,"
			"'-1' AS CrsCod,'' AS CrsShortName,"
	                "'-1' AS GrpCod"
			" FROM files,courses,degrees,centres,institutions,countries"
			" WHERE files.PublisherUsrCod='%ld' AND %s"
			" AND files.FileBrowser IN ('%u','%u')"
			" AND files.Cod=institutions.InsCod"
			" AND institutions.CtyCod=countries.CtyCod"
			"%s"
			" UNION "
			"SELECT files.FilCod,"	// Centre
			"SUBSTRING(files.Path,LOCATE('/',files.Path)) AS PathFromRoot,"
			"institutions.InsCod,institutions.ShortName AS InsShortName,"
			"centres.CtrCod,centres.ShortName AS CtrShortName,"
			"'-1' AS DegCod,'' AS DegShortName,"
			"'-1' AS CrsCod,'' AS CrsShortName,"
	                "'-1' AS GrpCod"
			" FROM files,courses,degrees,centres,institutions,countries"
			" WHERE files.PublisherUsrCod='%ld' AND %s"
			" AND files.FileBrowser IN ('%u','%u')"
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
			"'-1' AS CrsCod,'' AS CrsShortName,"
	                "'-1' AS GrpCod"
			" FROM files,courses,degrees,centres,institutions,countries"
			" WHERE files.PublisherUsrCod='%ld' AND %s"
			" AND files.FileBrowser IN ('%u','%u')"
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
	                "'-1' AS GrpCod"
			" FROM files,courses,degrees,centres,institutions,countries"
			" WHERE files.PublisherUsrCod='%ld' AND %s"
			" AND files.FileBrowser IN ('%u','%u','%u','%u')"
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
			" WHERE files.PublisherUsrCod='%ld' AND %s"
			" AND files.FileBrowser IN ('%u','%u','%u','%u')"
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
			"'-1' AS InsCod,'' AS InsShortName,"
			"'-1' AS CtrCod,'' AS CtrShortName,"
			"'-1' AS DegCod,'' AS DegShortName,"
			"'-1' AS CrsCod,'' AS CrsShortName,"
	                "'-1' AS GrpCod"
			" FROM files"
			" WHERE files.PublisherUsrCod='%ld' AND %s"
			" AND files.FileBrowser='%u'"
			") AS selected_files"
			" WHERE PathFromRoot<>''"
			" ORDER BY InsShortName,CtrShortName,DegShortName,CrsShortName,PathFromRoot",
		  Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,
		  (unsigned) Brw_ADMI_DOCUM_INS,
		  (unsigned) Brw_ADMI_SHARE_INS,
		  RangeQuery,
		  Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,
		  (unsigned) Brw_ADMI_DOCUM_CTR,
		  (unsigned) Brw_ADMI_SHARE_CTR,
		  RangeQuery,
		  Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,
		  (unsigned) Brw_ADMI_DOCUM_DEG,
		  (unsigned) Brw_ADMI_SHARE_DEG,
		  RangeQuery,
		  Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,
		  (unsigned) Brw_ADMI_DOCUM_CRS,
		  (unsigned) Brw_ADMI_TEACH_CRS,
		  (unsigned) Brw_ADMI_SHARE_CRS,
		  (unsigned) Brw_ADMI_MARKS_CRS,
		  RangeQuery,
		  Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,
		  (unsigned) Brw_ADMI_DOCUM_GRP,
		  (unsigned) Brw_ADMI_TEACH_GRP,
		  (unsigned) Brw_ADMI_SHARE_GRP,
		  (unsigned) Brw_ADMI_MARKS_GRP,
		  RangeQuery,
		  Gbl.Usrs.Me.UsrDat.UsrCod,SearchQuery,
		  (unsigned) Brw_ADMI_BRIEF_USR);

	 /***** Query database and list documents found *****/
	 /* if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
	    Lay_ShowAlert (Lay_INFO,Query); */
	 return Brw_ListDocsFound (Query,
	                           Txt_document_from_me,
	                           Txt_documents_from_me);
	}

   return 0;
  }

/*****************************************************************************/
/****** Build a search query by splitting a string to search into words ******/
/*****************************************************************************/
// Returns true if a valid search query is built
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
	 case Sch_SEARCH_DOCUM_IN_MY_COURSES:
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
