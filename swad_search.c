// swad_search.c: search for courses and teachers

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_browser_database.h"
#include "swad_center_database.h"
#include "swad_country_database.h"
#include "swad_course_database.h"
#include "swad_database.h"
#include "swad_degree_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_institution_database.h"
#include "swad_layout.h"
#include "swad_log.h"
#include "swad_parameter.h"
#include "swad_session_database.h"
#include "swad_user.h"
#include "swad_user_database.h"

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

#define Sch_MIN_LENGTH_LONGEST_WORD	  3
#define Sch_MIN_LENGTH_TOTAL		  6	// "An Ann" is not valid; "A An Ann" is valid

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static struct Sch_Search Sch_Search =
  {
   .WhatToSearch = Sch_WHAT_TO_SEARCH_DEFAULT,
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static Hie_Level_t Sch_PutFormToSearchWithWhatToSearchAndScope (Hie_Level_t DefaultHieLvl);
static Usr_Can_t Sch_CheckIfICanSearch (Sch_WhatToSearch_t WhatToSearch);
static void Sch_GetParSearchStr (void);
static void Sch_SearchInDB (Hie_Level_t HieLvl);

static unsigned Sch_SearchCountrsInDB (Hie_Level_t HieLvl,const char *RangeQuery);
static unsigned Sch_SearchInstitsInDB (Hie_Level_t HieLvl,const char *RangeQuery);
static unsigned Sch_SearchCentersInDB (Hie_Level_t HieLvl,const char *RangeQuery);
static unsigned Sch_SearchDegreesInDB (Hie_Level_t HieLvl,const char *RangeQuery);
static unsigned Sch_SearchCoursesInDB (const char *RangeQuery);

static unsigned Sch_SearchUsrsInDB (Hie_Level_t HieLvl,Rol_Role_t Role);

static unsigned Sch_SearchOpenDocumentsInDB (const char *RangeQuery);
static unsigned Sch_SearchDocumentsInMyCoursesInDB (const char *RangeQuery);
static unsigned Sch_SearchMyDocumentsInDB (const char *RangeQuery);

static void Sch_SaveLastSearchIntoSession (void);


/*****************************************************************************/
/*********************** Get pointer to global search ************************/
/*****************************************************************************/

struct Sch_Search *Sch_GetSearch (void)
  {
   return &Sch_Search;
  }

/*****************************************************************************/
/*********************** Request search in system tab ************************/
/*****************************************************************************/

void Sch_ReqSysSearch (void)
  {
   /***** Search courses, teachers, documents... *****/
   Sch_GetParWhatToSearch ();
   Sch_PutFormToSearchWithWhatToSearchAndScope (Hie_SYS);
  }

/*****************************************************************************/
/****************** Put a form to search, including scope ********************/
/*****************************************************************************/

static Hie_Level_t Sch_PutFormToSearchWithWhatToSearchAndScope (Hie_Level_t DefaultHieLvl)
  {
   extern const char *Hlp_START_Search;
   extern const char *Txt_Scope;
   extern const char *Txt_SEARCH_X_in_Y;
   extern const char *Txt_all;
   extern const char *Txt_HIERARCHY_PLURAL_abc[Hie_NUM_LEVELS];
   extern const char *Txt_users[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_my_documents;
   extern const char *Txt_documents_in_my_courses;
   extern const char *Txt_open_documents;
   extern const char *Txt_Search;
   static const char **Titles[Sch_NUM_WHAT_TO_SEARCH] =
     {
      [Sch_SEARCH_UNKNOWN            ] = NULL,
      [Sch_SEARCH_ALL                ] = &Txt_all,
      [Sch_SEARCH_COUNTRIES          ] = &Txt_HIERARCHY_PLURAL_abc[Hie_CTY],
      [Sch_SEARCH_INSTITS            ] = &Txt_HIERARCHY_PLURAL_abc[Hie_INS],
      [Sch_SEARCH_CENTERS            ] = &Txt_HIERARCHY_PLURAL_abc[Hie_CTR],
      [Sch_SEARCH_DEGREES            ] = &Txt_HIERARCHY_PLURAL_abc[Hie_DEG],
      [Sch_SEARCH_COURSES            ] = &Txt_HIERARCHY_PLURAL_abc[Hie_CRS],
      [Sch_SEARCH_USERS              ] = &Txt_users[0],
      [Sch_SEARCH_TEACHERS           ] = &Txt_ROLES_PLURAL_abc[Rol_TCH][Usr_SEX_UNKNOWN],
      [Sch_SEARCH_STUDENTS           ] = &Txt_ROLES_PLURAL_abc[Rol_STD][Usr_SEX_UNKNOWN],
      [Sch_SEARCH_GUESTS             ] = &Txt_ROLES_PLURAL_abc[Rol_GST][Usr_SEX_UNKNOWN],
      [Sch_SEARCH_OPEN_DOCUMENTS     ] = &Txt_open_documents,
      [Sch_SEARCH_DOCUM_IN_MY_COURSES] = &Txt_documents_in_my_courses,
      [Sch_SEARCH_MY_DOCUMENTS       ] = &Txt_my_documents,
     };
   unsigned AllowedLvls;
   Hie_Level_t HieLvl;
   Sch_WhatToSearch_t WhatToSearch;
   unsigned WTS;
   const struct Sch_Search *Search = Sch_GetSearch ();

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"CM\"");

      /***** Begin box *****/
      Box_BoxBegin (Txt_Search,Hie_PutIconToSelectMyHierarchy,NULL,
		    Hlp_START_Search,Box_NOT_CLOSABLE);

	 /***** Begin form *****/
	 Frm_BeginForm (ActSch);

	    /***** Scope (whole platform, current country, current institution,
			  current center, current degree or current course) *****/
	    HTM_DIV_Begin ("class=\"CM\"");
	       HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
		  HTM_TxtColonNBSP (Txt_Scope);
		  AllowedLvls = 1 << Hie_SYS |
			        1 << Hie_CTY |
			        1 << Hie_INS |
			        1 << Hie_CTR |
			        1 << Hie_DEG |
			        1 << Hie_CRS;
		  HieLvl = Sco_GetScope ("ScopeSch",DefaultHieLvl,AllowedLvls);
		  Sco_PutSelectorScope ("ScopeSch",HTM_NO_ATTR,
					HieLvl,AllowedLvls);
	       HTM_LABEL_End ();
	    HTM_DIV_End ();

	    /***** String to find *****/
	    Sch_PutInputStringToSearch ("");

	    /***** What to search? *****/
	    HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	       HTM_SPTxt (Txt_SEARCH_X_in_Y);
	       HTM_NBSP ();
	       HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
				 "name=\"WhatToSearch\""
				 " class=\"WHAT_TO_SEARCH INPUT_%s\"",
				 The_GetSuffix ());
		  for (WhatToSearch  = (Sch_WhatToSearch_t) 0;
		       WhatToSearch <= (Sch_WhatToSearch_t) (Sch_NUM_WHAT_TO_SEARCH - 1);
		       WhatToSearch++)
		     if (Sch_CheckIfICanSearch (WhatToSearch) == Usr_CAN)
		       {
			WTS = (unsigned) WhatToSearch;
			HTM_OPTION (HTM_Type_UNSIGNED,&WTS,
				    (WhatToSearch == Search->WhatToSearch) ? HTM_SELECTED :
									     HTM_NO_ATTR,
				    "%s",*Titles[WhatToSearch]);
		       }
	       HTM_SELECT_End ();
	    HTM_LABEL_End ();

	    /***** Magnifying glass icon *****/
	    Sch_PutMagnifyingGlassButton (Ico_BLACK);

	    /***** Send button *****/
	    Btn_PutButton (Btn_SEARCH);

	 /***** End form *****/
	 Frm_EndForm ();

      /***** End box *****/
      Box_BoxEnd ();

   /***** End container *****/
   HTM_DIV_End ();

   return HieLvl;
  }

/*****************************************************************************/
/************* Check if I have permission to execute an action ***************/
/*****************************************************************************/

static Usr_Can_t Sch_CheckIfICanSearch (Sch_WhatToSearch_t WhatToSearch)
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

   return (Permissions[WhatToSearch] & (1 << Gbl.Usrs.Me.Role.Logged)) ? Usr_CAN :
									 Usr_CAN_NOT;
  }

/*****************************************************************************/
/**************** Put a form to search in page top heading *******************/
/*****************************************************************************/

void Sch_PutFormToSearchInPageTopHeading (void)
  {
   HTM_DIV_Begin ("id=\"head_row_1_search\"");

      /***** Put form *****/
      Frm_BeginForm (ActSch);
	 Sco_PutParScope ("ScopeSch",Hie_SYS);
	 Sch_PutInputStringToSearch ("head_search_text");
	 Sch_PutMagnifyingGlassButton (Ico_WHITE);
      Frm_EndForm ();

   HTM_DIV_End ();	// head_row_1_search
  }

/*****************************************************************************/
/********************* Put string to search inside form **********************/
/*****************************************************************************/

void Sch_PutInputStringToSearch (const char *IdInputText)
  {
   extern const char *Txt_Search;
   const struct Sch_Search *Search = Sch_GetSearch ();

   /***** String to find *****/
   // Input field not required, because it can be hidden (display:none)
   HTM_INPUT_SEARCH ("Search",Sch_MAX_CHARS_STRING_TO_FIND,Search->Str,
	             "id=\"%s\" size=\"16\" class=\"INPUT_%s\""
	             " placeholder=\"%s&hellip;\"",
		     IdInputText,The_GetSuffix (),Txt_Search);
  }

/*****************************************************************************/
/******************* Put magnifying glass button inside form *****************/
/*****************************************************************************/

void Sch_PutMagnifyingGlassButton (Ico_Color_t Color)
  {
   Ico_PutIconLink ("search.svg",Color,ActSch);
  }

/*****************************************************************************/
/************* Get parameter "what to search" from search form ***************/
/*****************************************************************************/

void Sch_GetParWhatToSearch (void)
  {
   Sch_WhatToSearch_t WhatToSearch;
   struct Sch_Search *Search = Sch_GetSearch ();

   /***** Get what to search from form *****/
   WhatToSearch = (Sch_WhatToSearch_t)
	          Par_GetParUnsignedLong ("WhatToSearch",
	                                  0,
	                                  Sch_NUM_WHAT_TO_SEARCH - 1,
	                                  (unsigned long) Sch_SEARCH_UNKNOWN);

   /***** If parameter WhatToSearch is not present,
          use parameter from session *****/
   if (WhatToSearch != Sch_SEARCH_UNKNOWN)
      Search->WhatToSearch = WhatToSearch;
  }

/*****************************************************************************/
/*********************** Get string from search form *************************/
/*****************************************************************************/

static void Sch_GetParSearchStr (void)
  {
   struct Sch_Search *Search = Sch_GetSearch ();

   /***** Get string to search *****/
   Par_GetParText ("Search",Search->Str,Sch_MAX_BYTES_STRING_TO_FIND);
  }

/*****************************************************************************/
/************************* Get parameters to search **************************/
/*****************************************************************************/

void Sch_GetParsSearch (void)
  {
   /***** What to search? *****/
   Sch_GetParWhatToSearch ();

   /***** Get search string *****/
   Sch_GetParSearchStr ();

   /***** Save my search in order to show it in current session *****/
   if (Gbl.Usrs.Me.Logged)
      Sch_SaveLastSearchIntoSession ();
  }

/*****************************************************************************/
/****************** Search courses, teachers, documents... *******************/
/*****************************************************************************/

void Sch_SysSearch (void)
  {
   Hie_Level_t HieLvl;
   const struct Sch_Search *Search = Sch_GetSearch ();

   if (Search->Str[0])
     {
      /***** Activate logging search in database *****/
      Log_SetLogSearch (true);

      /***** Show search form again *****/
      HieLvl = Sch_PutFormToSearchWithWhatToSearchAndScope (Hie_SYS);

      /***** Show results of search *****/
      Sch_SearchInDB (HieLvl);
     }
   else
      /***** Show search form and selectors *****/
      Sch_ReqSysSearch ();
  }

/*****************************************************************************/
/**** Search institutions, centers, degrees, courses, teachers, documents ****/
/*****************************************************************************/

static void Sch_SearchInDB (Hie_Level_t HieLvl)
  {
   extern const char *Txt_No_results;
   char RangeQuery[256];
   unsigned NumResults = 0;	// Initialized to avoid warning
   struct Sch_Search *Search = Sch_GetSearch ();

   /***** Select courses in all degrees or in current degree *****/
   switch (HieLvl)
     {
      case Hie_UNK:
	 // Not aplicable
      case Hie_SYS:
         RangeQuery[0] = '\0';
         break;
      case Hie_CTY:
         sprintf (RangeQuery," AND cty_countrs.CtyCod=%ld",
                  Gbl.Hierarchy.Node[Hie_CTY].HieCod);
         break;
      case Hie_INS:
         sprintf (RangeQuery," AND ins_instits.InsCod=%ld",
                  Gbl.Hierarchy.Node[Hie_INS].HieCod);
         break;
      case Hie_CTR:
         sprintf (RangeQuery," AND ctr_centers.CtrCod=%ld",
                  Gbl.Hierarchy.Node[Hie_CTR].HieCod);
         break;
      case Hie_DEG:
         sprintf (RangeQuery," AND deg_degrees.DegCod=%ld",
                  Gbl.Hierarchy.Node[Hie_DEG].HieCod);
         break;
      case Hie_CRS:
         sprintf (RangeQuery," AND crs_courses.CrsCod=%ld",
                  Gbl.Hierarchy.Node[Hie_CRS].HieCod);
         break;
     }

   if (Search->WhatToSearch == Sch_SEARCH_UNKNOWN)
      Search->WhatToSearch = Sch_WHAT_TO_SEARCH_DEFAULT;
   switch (Search->WhatToSearch)
     {
      case Sch_SEARCH_ALL:
	 NumResults  = Sch_SearchCountrsInDB (HieLvl,RangeQuery);
	 NumResults += Sch_SearchInstitsInDB (HieLvl,RangeQuery);
	 NumResults += Sch_SearchCentersInDB (HieLvl,RangeQuery);
	 NumResults += Sch_SearchDegreesInDB (HieLvl,RangeQuery);
	 NumResults += Sch_SearchCoursesInDB (RangeQuery);
	 NumResults += Sch_SearchUsrsInDB (HieLvl,Rol_TCH);	// Non-editing teachers and teachers
	 NumResults += Sch_SearchUsrsInDB (HieLvl,Rol_STD);
	 NumResults += Sch_SearchUsrsInDB (HieLvl,Rol_GST);
	 NumResults += Sch_SearchOpenDocumentsInDB (RangeQuery);
	 NumResults += Sch_SearchDocumentsInMyCoursesInDB (RangeQuery);
	 NumResults += Sch_SearchMyDocumentsInDB (RangeQuery);
	 break;
      case Sch_SEARCH_COUNTRIES:
	 NumResults = Sch_SearchCountrsInDB (HieLvl,RangeQuery);
	 break;
      case Sch_SEARCH_INSTITS:
	 NumResults = Sch_SearchInstitsInDB (HieLvl,RangeQuery);
	 break;
      case Sch_SEARCH_CENTERS:
	 NumResults = Sch_SearchCentersInDB (HieLvl,RangeQuery);
	 break;
      case Sch_SEARCH_DEGREES:
	 NumResults = Sch_SearchDegreesInDB (HieLvl,RangeQuery);
	 break;
      case Sch_SEARCH_COURSES:
	 NumResults = Sch_SearchCoursesInDB (RangeQuery);
	 break;
      case Sch_SEARCH_USERS:
	 NumResults = Sch_SearchUsrsInDB (HieLvl,Rol_UNK);	// Here Rol_UNK means any user
	 break;
      case Sch_SEARCH_TEACHERS:
	 NumResults = Sch_SearchUsrsInDB (HieLvl,Rol_TCH);	// Non-editing teachers and teachers
	 break;
      case Sch_SEARCH_STUDENTS:
	 NumResults = Sch_SearchUsrsInDB (HieLvl,Rol_STD);
	 break;
      case Sch_SEARCH_GUESTS:
	 NumResults = Sch_SearchUsrsInDB (HieLvl,Rol_GST);
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

static unsigned Sch_SearchCountrsInDB (Hie_Level_t HieLvl,const char *RangeQuery)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   char FldName[4+1+2+1];	// Example: Name_en
   MYSQL_RES *mysql_res;
   unsigned NumCtys;

   /***** Check scope *****/
   if (HieLvl != Hie_INS &&
       HieLvl != Hie_CTR &&
       HieLvl != Hie_DEG &&
       HieLvl != Hie_CRS)
      /***** Check user's permission *****/
      if (Sch_CheckIfICanSearch (Sch_SEARCH_COUNTRIES) == Usr_CAN)
	{
	 /***** Split countries string into words *****/
	 snprintf (FldName,sizeof (FldName),"Name_%s",
		   Lan_STR_LANG_ID[Gbl.Prefs.Language]);
	 if (Sch_BuildSearchQuery (SearchQuery,Sch_GetSearch (),
	                           FldName,NULL,NULL))
	   {
	    /***** Query database and list countries found *****/
	    NumCtys = Cty_DB_SearchCtys (&mysql_res,SearchQuery,RangeQuery);
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

static unsigned Sch_SearchInstitsInDB (Hie_Level_t HieLvl,const char *RangeQuery)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   MYSQL_RES *mysql_res;
   unsigned NumInss;

   /***** Check scope *****/
   if (HieLvl != Hie_CTR &&
       HieLvl != Hie_DEG &&
       HieLvl != Hie_CRS)
      /***** Check user's permission *****/
      if (Sch_CheckIfICanSearch (Sch_SEARCH_INSTITS) == Usr_CAN)
	 /***** Split institutions string into words *****/
	 if (Sch_BuildSearchQuery (SearchQuery,Sch_GetSearch (),
	                           "ins_instits.FullName",NULL,NULL))
	   {
	    /***** Query database and list institutions found *****/
	    NumInss = Ins_DB_SearchInss (&mysql_res,SearchQuery,RangeQuery);
	    Ins_ListInssFound (&mysql_res,NumInss);
	    return NumInss;
	   }

   return 0;
  }

/*****************************************************************************/
/************************* Search centers in database ************************/
/*****************************************************************************/
// Returns number of centers found

static unsigned Sch_SearchCentersInDB (Hie_Level_t HieLvl,const char *RangeQuery)
  {
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   MYSQL_RES *mysql_res;
   unsigned NumCtrs;

   /***** Check scope *****/
   if (HieLvl != Hie_DEG &&
       HieLvl != Hie_CRS)
      /***** Check user's permission *****/
      if (Sch_CheckIfICanSearch (Sch_SEARCH_CENTERS) == Usr_CAN)
	 /***** Split center string into words *****/
	 if (Sch_BuildSearchQuery (SearchQuery,Sch_GetSearch (),
	                           "ctr_centers.FullName",NULL,NULL))
	   {
	    /***** Query database and list centers found *****/
	    NumCtrs = Ctr_DB_SearchCtrs (&mysql_res,SearchQuery,RangeQuery);
	    Ctr_ListCtrsFound (&mysql_res,NumCtrs);
	    return NumCtrs;
	   }

   return 0;
  }

/*****************************************************************************/
/************************* Search degrees in database ************************/
/*****************************************************************************/
// Returns number of degrees found

static unsigned Sch_SearchDegreesInDB (Hie_Level_t HieLvl,const char *RangeQuery)
  {
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   MYSQL_RES *mysql_res;
   unsigned NumDegs;

   /***** Check scope *****/
   if (HieLvl != Hie_CRS)
      /***** Check user's permission *****/
      if (Sch_CheckIfICanSearch (Sch_SEARCH_DEGREES) == Usr_CAN)
	 /***** Split degree string into words *****/
	 if (Sch_BuildSearchQuery (SearchQuery,Sch_GetSearch (),
	                           "deg_degrees.FullName",NULL,NULL))
	   {
	    /***** Query database and list degrees found *****/
	    NumDegs = Deg_DB_SearchDegs (&mysql_res,SearchQuery,RangeQuery);
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
   if (Sch_CheckIfICanSearch (Sch_SEARCH_COURSES) == Usr_CAN)
      /***** Split course string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,Sch_GetSearch (),
                                "crs_courses.FullName",NULL,NULL))
	{
	 /***** Query database and list courses found *****/
	 NumCrss = Crs_DB_SearchCrss (&mysql_res,SearchQuery,RangeQuery);
	 Crs_ListCrssFound (&mysql_res,NumCrss);
	 return NumCrss;
	}

   return 0;
  }

/*****************************************************************************/
/*************************** Search users in database ************************/
/*****************************************************************************/
// Returns number of users found

static unsigned Sch_SearchUsrsInDB (Hie_Level_t HieLvl,Rol_Role_t Role)
  {
   extern const char *Txt_The_search_text_must_be_longer;
   static bool WarningMessageWritten = false;
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];

   /***** Split user string into words *****/
   if (Sch_BuildSearchQuery (SearchQuery,Sch_GetSearch (),
			     "CONCAT_WS(' ',FirstName,Surname1,Surname2)",
			     NULL,NULL))
      /***** Query database and list users found *****/
      return Usr_ListUsrsFound (HieLvl,Role,SearchQuery);
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
   unsigned NumDocs;

   /***** Check user's permission *****/
   if (Sch_CheckIfICanSearch (Sch_SEARCH_OPEN_DOCUMENTS) == Usr_CAN)
      /***** Split document string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,Sch_GetSearch (),
                                "SUBSTRING_INDEX(brw_files.Path,'/',-1)",
				"_latin1 "," COLLATE latin1_general_ci"))
	{
	 /***** Query database *****/
	 NumDocs = Brw_DB_SearchPublicFiles (&mysql_res,RangeQuery,SearchQuery);

	 /***** List documents found *****/
	 Brw_ListDocsFound (&mysql_res,NumDocs,
			    Txt_open_document,
	                    Txt_open_documents);

	 return NumDocs;
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
   unsigned NumDocs;

   /***** Check user's permission *****/
   if (Sch_CheckIfICanSearch (Sch_SEARCH_DOCUM_IN_MY_COURSES) == Usr_CAN)
      /***** Split document string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,Sch_GetSearch (),
                                "SUBSTRING_INDEX(brw_files.Path,'/',-1)",
				"_latin1 "," COLLATE latin1_general_ci"))
	{
	 /***** Query database *****/
	 NumDocs = Brw_DB_SearchFilesInMyCrss (&mysql_res,RangeQuery,SearchQuery);

	 /***** List documents found *****/
	 Brw_ListDocsFound (&mysql_res,NumDocs,
	                    Txt_document_in_my_courses,
	                    Txt_documents_in_my_courses);

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
   char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1];
   MYSQL_RES *mysql_res;
   unsigned NumDocs;

   /***** Check user's permission *****/
   if (Sch_CheckIfICanSearch (Sch_SEARCH_MY_DOCUMENTS) == Usr_CAN)
      /***** Split document string into words *****/
      if (Sch_BuildSearchQuery (SearchQuery,Sch_GetSearch (),
                                "SUBSTRING_INDEX(brw_files.Path,'/',-1)",
				"_latin1 "," COLLATE latin1_general_ci"))
	{
	 /***** Query database *****/
	 NumDocs = Brw_DB_SearchMyFiles (&mysql_res,RangeQuery,SearchQuery);

	 /***** List documents found *****/
	 Brw_ListDocsFound (&mysql_res,NumDocs,
			    Txt_document_from_me,
	                    Txt_documents_from_me);

	 return NumDocs;
	}

   return 0;
  }

/*****************************************************************************/
/****** Build a search query by splitting a string to search into words ******/
/*****************************************************************************/
// Returns true if a valid search query is built
// Returns false when no valid search query

bool Sch_BuildSearchQuery (char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1],
                           const struct Sch_Search *Search,
                           const char *FldName,
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

   if (Search->Str[0])
     {
      SearchQuery[0] = '\0';
      Ptr = Search->Str;
      for (NumWords = 0;
	   NumWords < Sch_MAX_WORDS_IN_SEARCH && *Ptr;
	   NumWords++)
	{
	 /* Get next word */
	 Str_GetNextStringUntilSpace (&Ptr,SearchWords[NumWords],Sch_MAX_BYTES_SEARCH_WORD);

	 /* Is this word valid? */
	 switch (Search->WhatToSearch)
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
	    Str_Concat (SearchQuery,FldName,Sch_MAX_BYTES_SEARCH_QUERY);
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
   struct Sch_Search *Search;

   if (Gbl.Usrs.Me.Logged)
     {
      Search = Sch_GetSearch ();
      if (Search->WhatToSearch == Sch_SEARCH_UNKNOWN)
	 Search->WhatToSearch = Sch_WHAT_TO_SEARCH_DEFAULT;

      /***** Save last search in session *****/
      Ses_DB_SaveLastSearchIntoSession (Search);

      /***** Update my last type of search *****/
      // WhatToSearch is stored in usr_last for next time I log in
      // In other existing sessions distinct to this, WhatToSearch will remain unchanged
      Usr_DB_UpdateMyLastWhatToSearch (Search->WhatToSearch);
     }
  }

/*****************************************************************************/
/*************** Write parameters for link to search courses *****************/
/*****************************************************************************/

void Sch_PutLinkToSearchCoursesPars (__attribute__((unused)) void *Args)
  {
   Sco_PutParScope ("ScopeSch",Hie_SYS);
   Par_PutParUnsigned (NULL,"WhatToSearch",(unsigned) Sch_SEARCH_COURSES);
  }
