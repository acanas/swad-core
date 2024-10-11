// swad_resource.c: resources for course program and rubrics

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include <stdbool.h>		// For boolean type
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_assignment_database.h"
#include "swad_attendance_database.h"
#include "swad_browser_resource.h"
#include "swad_call_for_exam_resource.h"
#include "swad_database.h"
#include "swad_exam_database.h"
#include "swad_forum_database.h"
#include "swad_forum_resource.h"
#include "swad_game_database.h"
#include "swad_global.h"
#include "swad_group_database.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_project_database.h"
#include "swad_resource.h"
#include "swad_resource_database.h"
#include "swad_role.h"
#include "swad_rubric_database.h"
#include "swad_survey_database.h"
#include "swad_tag_database.h"
#include "swad_theme.h"
#include "swad_timetable.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

const char *Rsc_ResourceTypesDB[Rsc_NUM_TYPES] =
  {
   [Rsc_NONE		] = "non",
   [Rsc_INFORMATION	] = "inf",
   [Rsc_TEACH_GUIDE	] = "gui",
   [Rsc_LECTURES	] = "lec",
   [Rsc_PRACTICALS	] = "pra",
   [Rsc_BIBLIOGRAPHY	] = "bib",
   [Rsc_FAQ		] = "faq",
   [Rsc_LINKS		] = "lnk",
   [Rsc_ASSESSMENT	] = "ass",
   [Rsc_TIMETABLE	] = "tmt",
   [Rsc_ASSIGNMENT	] = "asg",
   [Rsc_PROJECT		] = "prj",
   [Rsc_CALL_FOR_EXAM	] = "cfe",
   [Rsc_TEST		] = "tst",
   [Rsc_EXAM		] = "exa",
   [Rsc_GAME		] = "gam",
   [Rsc_RUBRIC		] = "rub",
   [Rsc_DOCUMENT	] = "doc",
   [Rsc_MARKS		] = "mrk",
   [Rsc_GROUPS		] = "grp",
   [Rsc_ATT_EVENT	] = "att",
   [Rsc_FORUM_THREAD	] = "for",
   [Rsc_SURVEY		] = "svy",
  };

const char *Rsc_ResourceTypesIcons[Rsc_NUM_TYPES] =
  {
   [Rsc_NONE		] = "link-slash.svg",
   [Rsc_INFORMATION	] = "info.svg",
   [Rsc_TEACH_GUIDE	] = "book-open.svg",
   [Rsc_LECTURES	] = "list-ol.svg",
   [Rsc_PRACTICALS	] = "list-ol.svg",
   [Rsc_BIBLIOGRAPHY	] = "book.svg",
   [Rsc_FAQ		] = "question.svg",
   [Rsc_LINKS		] = "up-right-from-square.svg",
   [Rsc_ASSESSMENT	] = "check.svg",
   [Rsc_TIMETABLE	] = "clock.svg",
   [Rsc_ASSIGNMENT	] = "edit.svg",
   [Rsc_PROJECT		] = "file-invoice.svg",
   [Rsc_CALL_FOR_EXAM	] = "bullhorn.svg",
   [Rsc_TEST		] = "check.svg",
   [Rsc_EXAM		] = "file-signature.svg",
   [Rsc_GAME		] = "gamepad.svg",
   [Rsc_RUBRIC		] = "tasks.svg",
   [Rsc_DOCUMENT	] = "file.svg",
   [Rsc_MARKS		] = "list-alt.svg",
   [Rsc_GROUPS		] = "sitemap.svg",
   [Rsc_ATT_EVENT	] = "calendar-check.svg",
   [Rsc_FORUM_THREAD	] = "comments.svg",
   [Rsc_SURVEY		] = "poll.svg",
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Rsc_WriteRowClipboard (const struct Rsc_Link *Link,
                                   HTM_Attributes_t Attributes);

/*****************************************************************************/
/************************* Show resources clipboard **************************/
/*****************************************************************************/

void Rsc_ShowClipboard (void)
  {
   MYSQL_RES *mysql_res;
   unsigned NumLink;
   unsigned NumLinks;
   struct Rsc_Link Link;

   /***** Begin list *****/
   HTM_UL_Begin ("class=\"RSC_CLIPBOARD\"");

      /***** Get links in clipboard from database and write them *****/
      NumLinks = Rsc_DB_GetClipboard (&mysql_res);
      for (NumLink  = 1;
	   NumLink <= NumLinks;
	   NumLink++)
	{
	 Rsc_GetLinkDataFromRow (mysql_res,&Link);
	 HTM_LI_Begin ("class=\"PRG_RSC_%s\"",The_GetSuffix ());
	    Rsc_WriteLinkName (&Link,Frm_PUT_FORM);
	 HTM_LI_End ();
	}
      DB_FreeMySQLResult (&mysql_res);

   /***** End list *****/
   HTM_UL_End ();
  }

/*****************************************************************************/
/***************** Show clipboard to change resource link ********************/
/*****************************************************************************/

void Rsc_ShowClipboardToChangeLink (const struct Rsc_Link *CurrentLink)
  {
   MYSQL_RES *mysql_res;
   unsigned NumLink;
   unsigned NumLinks;
   struct Rsc_Link Link;
   static const struct Rsc_Link EmptyLink =
     {
      .Type = Rsc_NONE,
      .Cod  = -1L,
     };

   /***** Begin list *****/
   HTM_UL_Begin ("class=\"RSC_CLIPBOARD\"");

      if (CurrentLink)	// Editing an existing element
	{
         /***** Current link (empty or not) *****/
	 Rsc_WriteRowClipboard (CurrentLink,
				HTM_CHECKED);

	 /***** Row with empty link to remove the current link *****/
	 if (CurrentLink->Type != Rsc_NONE)
	    Rsc_WriteRowClipboard (&EmptyLink,
				   HTM_SUBMIT_ON_CLICK);
	}
      else		// Inside form to create a new element
	 /***** Row with empty link *****/
	 Rsc_WriteRowClipboard (&EmptyLink,
				HTM_CHECKED);

      /***** Get links in clipboard from database and write them *****/
      NumLinks = Rsc_DB_GetClipboard (&mysql_res);
      for (NumLink  = 1;
	   NumLink <= NumLinks;
	   NumLink++)
	{
	 Rsc_GetLinkDataFromRow (mysql_res,&Link);
	 Rsc_WriteRowClipboard (&Link,
				CurrentLink ? HTM_SUBMIT_ON_CLICK :
					      HTM_NO_ATTR);
	}
      DB_FreeMySQLResult (&mysql_res);

   /***** End list *****/
   HTM_UL_End ();
  }

/*****************************************************************************/
/************************ Show one link from clipboard ***********************/
/*****************************************************************************/

static void Rsc_WriteRowClipboard (const struct Rsc_Link *Link,
                                   HTM_Attributes_t Attributes)
  {
   /***** Begin list row *****/
   HTM_LI_Begin ("class=\"PRG_RSC_%s\"",The_GetSuffix ());
      HTM_LABEL_Begin (NULL);

         /***** Radio selector *****/
	 HTM_INPUT_RADIO ("Link",
			  Attributes,
			  "value=\"%s_%ld\"",
			  Rsc_ResourceTypesDB[Link->Type],Link->Cod);

	 /***** Name *****/
         Rsc_WriteLinkName (Link,Frm_DONT_PUT_FORM);

      HTM_LABEL_End ();

   /***** End list row *****/
   HTM_LI_End ();
  }

/*****************************************************************************/
/************* Write link name (filename, assignment title...) ***************/
/*****************************************************************************/

void Rsc_WriteLinkName (const struct Rsc_Link *Link,Frm_PutForm_t PutFormToGo)
  {
   extern const char *Rsc_ResourceTypesIcons[Rsc_NUM_TYPES];
   extern Syl_WhichSyllabus_t Syl_WhichSyllabus[Syl_NUM_WHICH_SYLLABUS];
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];
   extern const char *Txt_RESOURCE_TYPES[Rsc_NUM_TYPES];
   static struct
     {
      void (*Set) (long Cod,char **Anchor);
      void (*Free) (char **Anchor);
     } FuncAnchor[Rsc_NUM_TYPES] =
     {
      [Rsc_NONE		] = {NULL		,NULL			},
      [Rsc_INFORMATION	] = {NULL		,NULL			},
      [Rsc_TEACH_GUIDE	] = {NULL		,NULL			},
      [Rsc_LECTURES	] = {NULL		,NULL			},
      [Rsc_PRACTICALS	] = {NULL		,NULL			},
      [Rsc_BIBLIOGRAPHY	] = {NULL		,NULL			},
      [Rsc_FAQ		] = {NULL		,NULL			},
      [Rsc_LINKS	] = {NULL		,NULL			},
      [Rsc_ASSESSMENT	] = {NULL		,NULL			},
      [Rsc_TIMETABLE	] = {NULL		,NULL			},
      [Rsc_ASSIGNMENT	] = {NULL		,NULL			},
      [Rsc_PROJECT	] = {NULL		,NULL			},
      [Rsc_CALL_FOR_EXAM] = {Frm_SetAnchorStr	,Frm_FreeAnchorStr	},
      [Rsc_TEST		] = {NULL		,NULL			},
      [Rsc_EXAM		] = {NULL		,NULL			},
      [Rsc_GAME		] = {NULL		,NULL			},
      [Rsc_RUBRIC	] = {NULL		,NULL			},
      [Rsc_DOCUMENT	] = {NULL		,NULL			},
      [Rsc_MARKS	] = {NULL		,NULL			},
      [Rsc_GROUPS	] = {Frm_SetAnchorStr	,Frm_FreeAnchorStr	},
      [Rsc_ATT_EVENT	] = {NULL		,NULL			},
      [Rsc_FORUM_THREAD	] = {ForRsc_SetAnchorStr,ForRsc_FreeAnchorStr	},
      [Rsc_SURVEY	] = {NULL		,NULL			},
     };
   static struct
     {
      struct Act_ActionFunc IfCod;
      struct Act_ActionFunc IfNotCod;
     } NextActions[Rsc_NUM_TYPES] =
     {
      [Rsc_NONE		] = {{ActUnk		,NULL,NULL},
			     {ActUnk		,NULL,NULL}},
      [Rsc_INFORMATION	] = {{ActUnk		,NULL,NULL},
			     {ActSeeCrsInf	,NULL,NULL}},
      [Rsc_TEACH_GUIDE	] = {{ActUnk		,NULL,NULL},
			     {ActSeeTchGui	,NULL,NULL}},
      [Rsc_LECTURES	] = {{ActUnk		,NULL,NULL},
			     {ActSeeSyl		,Syl_PutParWhichSyllabus,&Syl_WhichSyllabus[Syl_LECTURES  ]}},
      [Rsc_PRACTICALS	] = {{ActUnk		,NULL,NULL},
			     {ActSeeSyl		,Syl_PutParWhichSyllabus,&Syl_WhichSyllabus[Syl_PRACTICALS]}},
      [Rsc_BIBLIOGRAPHY	] = {{ActUnk		,NULL,NULL},
			     {ActSeeBib		,NULL,NULL}},
      [Rsc_FAQ		] = {{ActUnk		,NULL,NULL},
			     {ActSeeFAQ		,NULL,NULL}},
      [Rsc_LINKS	] = {{ActUnk		,NULL,NULL},
			     {ActSeeCrsLnk	,NULL,NULL}},
      [Rsc_ASSESSMENT	] = {{ActUnk		,NULL,NULL},
			     {ActSeeAss		,NULL,NULL}},
      [Rsc_TIMETABLE	] = {{ActUnk		,NULL,NULL},
			     {ActSeeCrsTT	,NULL,NULL}},
      [Rsc_ASSIGNMENT	] = {{ActSeeOneAsg	,NULL,NULL},
			     {ActSeeAllAsg	,NULL,NULL}},
      [Rsc_PROJECT	] = {{ActSeeOnePrj	,NULL,NULL},
			     {ActSeeAllPrj	,NULL,NULL}},
      [Rsc_CALL_FOR_EXAM] = {{ActSeeOneCfe	,NULL,NULL},
			     {ActSeeAllCfe	,NULL,NULL}},
      [Rsc_TEST		] = {{ActReqTstOneTag	,NULL,NULL},
			     {ActReqTstAnyTag	,NULL,NULL}},
      [Rsc_EXAM		] = {{ActSeeOneExa	,NULL,NULL},
			     {ActSeeAllExa	,NULL,NULL}},
      [Rsc_GAME		] = {{ActSeeOneGam	,NULL,NULL},
			     {ActSeeAllGam	,NULL,NULL}},
      [Rsc_RUBRIC	] = {{ActSeeOneRub	,NULL,NULL},
			     {ActSeeAllRub	,NULL,NULL}},
      [Rsc_DOCUMENT	] = {{ActReqDatSeeDocCrs,NULL,NULL},
			     {ActSeeAdmDocCrsGrp,NULL,NULL}},
      [Rsc_MARKS	] = {{ActReqDatSeeMrkCrs,NULL,NULL},
			     {ActSeeAdmMrk	,NULL,NULL}},
      [Rsc_GROUPS	] = {{ActReqSelOneGrpTyp,NULL,NULL},
			     {ActReqSelAllGrp	,NULL,NULL}},
      [Rsc_ATT_EVENT	] = {{ActSeeOneAtt	,NULL,NULL},
			     {ActSeeAllAtt	,NULL,NULL}},
      [Rsc_FORUM_THREAD	] = {{ActSeePstForCrsUsr,NULL,NULL},
			     {ActSeeForCrsUsr	,NULL,NULL}},
      [Rsc_SURVEY	] = {{ActSeeOneSvy	,NULL,NULL},
			     {ActSeeAllSvy	,NULL,NULL}},
     };
   static ParCod_Param_t ParCod[Rsc_NUM_TYPES] =
     {
      [Rsc_NONE		] = ParCod_None,
      [Rsc_INFORMATION	] = ParCod_None,
      [Rsc_TEACH_GUIDE	] = ParCod_None,
      [Rsc_LECTURES	] = ParCod_None,
      [Rsc_PRACTICALS	] = ParCod_None,
      [Rsc_BIBLIOGRAPHY	] = ParCod_None,
      [Rsc_FAQ		] = ParCod_None,
      [Rsc_LINKS	] = ParCod_None,
      [Rsc_ASSESSMENT	] = ParCod_None,
      [Rsc_TIMETABLE	] = ParCod_None,
      [Rsc_ASSIGNMENT	] = ParCod_Asg,
      [Rsc_PROJECT	] = ParCod_Prj,
      [Rsc_CALL_FOR_EXAM] = ParCod_Exa,
      [Rsc_TEST		] = ParCod_Tag,
      [Rsc_EXAM		] = ParCod_Exa,
      [Rsc_GAME		] = ParCod_Gam,
      [Rsc_RUBRIC	] = ParCod_Rub,
      [Rsc_DOCUMENT	] = ParCod_Fil,
      [Rsc_MARKS	] = ParCod_Fil,
      [Rsc_GROUPS	] = ParCod_GrpTyp,
      [Rsc_ATT_EVENT	] = ParCod_Att,
      [Rsc_FORUM_THREAD	] = ParCod_Thr,
      [Rsc_SURVEY	] = ParCod_Svy,
     };
   Act_Action_t NextAction;
   char Title[Cns_MAX_BYTES_SUBJECT + 1];
   char *Anchor;

   /***** Get resource title *****/
   Rsc_GetResourceTitleFromLink (Link,Title);

   /***** Put icon and title of resource *****/
   switch (PutFormToGo)
     {
      case Frm_DONT_PUT_FORM:
	 /* Icon and title of resource */
	 Ico_PutIconOn (Rsc_ResourceTypesIcons[Link->Type],Ico_BLACK,
	                Txt_RESOURCE_TYPES[Link->Type]);
	 HTM_Txt (Title);
	 break;
      case Frm_PUT_FORM:
	 /* Begin form to go to resource */
	 NextAction = (Link->Cod > 0) ? NextActions[Link->Type].IfCod.NextAction :	// Resource specified
					NextActions[Link->Type].IfNotCod.NextAction;	// All resources of this type
	 if (FuncAnchor[Link->Type].Set)
	   {
	    FuncAnchor[Link->Type].Set (Link->Cod,&Anchor);
	    Frm_BeginFormAnchor (NextAction,Anchor);
	    FuncAnchor[Link->Type].Free (&Anchor);
	   }
	 else
	    Frm_BeginForm (NextAction);

	 /* Form hidden parameters */
	 ParCod_PutPar (ParCod[Link->Type],Link->Cod);
	 if (Link->Cod > 0)
	   {
	    if (NextActions[Link->Type].IfCod.FuncPars)
	       NextActions[Link->Type].IfCod.FuncPars (NextActions[Link->Type].IfCod.Args);
	   }
	 else
	   {
	    if (NextActions[Link->Type].IfNotCod.FuncPars)
	       NextActions[Link->Type].IfNotCod.FuncPars (NextActions[Link->Type].IfNotCod.Args);
	   }

	 /* Begin link */
	 HTM_BUTTON_Submit_Begin (Txt_Actions[NextAction],
				  "class=\"LM BT_LINK PRG_LNK_%s\"",
				  The_GetSuffix ());

	    /* Icon and title of resource */
	    Ico_PutIconLink (Rsc_ResourceTypesIcons[Link->Type],Ico_BLACK,
	                     NextAction);
	    HTM_Txt (Title);

         /* End link */
	 HTM_BUTTON_End ();

	 Frm_EndForm ();
	 break;
     }
  }

/*****************************************************************************/
/************* Get the title for a new resource from link title **************/
/*****************************************************************************/

void Rsc_GetResourceTitleFromLink (const struct Rsc_Link *Link,
                                   char Title[Rsc_MAX_BYTES_RESOURCE_TITLE + 1])
  {
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   extern const char *Txt_TIMETABLE_TYPES[Tmt_NUM_TIMETABLE_TYPES];
   extern const char *Txt_Assignments;
   extern const char *Txt_Projects;
   extern const char *Txt_Calls_for_exams;
   extern const char *Txt_Test;
   extern const char *Txt_Exams;
   extern const char *Txt_Games;
   extern const char *Txt_Rubrics;
   extern const char *Txt_Documents;
   extern const char *Txt_Marks_area;
   extern const char *Txt_Groups;
   extern const char *Txt_Control_of_class_attendance;
   extern const char *Txt_Course_forum;
   extern const char *Txt_Surveys;
   static void (*GetTitle[Rsc_NUM_TYPES]) (long Cod,char *Title,size_t TitleSize) =
     {
      [Rsc_NONE		] = NULL,
      [Rsc_INFORMATION	] = NULL,
      [Rsc_TEACH_GUIDE	] = NULL,
      [Rsc_LECTURES	] = NULL,
      [Rsc_PRACTICALS	] = NULL,
      [Rsc_BIBLIOGRAPHY	] = NULL,
      [Rsc_FAQ		] = NULL,
      [Rsc_LINKS	] = NULL,
      [Rsc_ASSESSMENT	] = NULL,
      [Rsc_TIMETABLE	] = NULL,
      [Rsc_ASSIGNMENT	] = Asg_DB_GetAssignmentTitle,
      [Rsc_PROJECT	] = Prj_DB_GetProjectTitle,
      [Rsc_CALL_FOR_EXAM] = CfeRsc_GetCallForExamTitle,
      [Rsc_TEST		] = Tag_DB_GetTagTitle,
      [Rsc_EXAM		] = Exa_DB_GetExamTitle,
      [Rsc_GAME		] = Gam_DB_GetGameTitle,
      [Rsc_RUBRIC	] = Rub_DB_GetRubricTitle,
      [Rsc_DOCUMENT	] = BrwRsc_GetFileTitle,
      [Rsc_MARKS	] = BrwRsc_GetFileTitle,
      [Rsc_GROUPS	] = Grp_DB_GetGrpTypTitle,
      [Rsc_ATT_EVENT	] = Att_DB_GetEventTitle,
      [Rsc_FORUM_THREAD	] = For_DB_GetThreadTitle,
      [Rsc_SURVEY	] = Svy_DB_GetSurveyTitle,
     };
   static const char **GenericTitle[Rsc_NUM_TYPES] =
     {
      [Rsc_NONE		] = NULL,
      [Rsc_INFORMATION	] = &Txt_INFO_TITLE[Inf_INFORMATION	],
      [Rsc_TEACH_GUIDE	] = &Txt_INFO_TITLE[Inf_TEACH_GUIDE	],
      [Rsc_LECTURES	] = &Txt_INFO_TITLE[Inf_LECTURES	],
      [Rsc_PRACTICALS	] = &Txt_INFO_TITLE[Inf_PRACTICALS	],
      [Rsc_BIBLIOGRAPHY	] = &Txt_INFO_TITLE[Inf_BIBLIOGRAPHY	],
      [Rsc_FAQ		] = &Txt_INFO_TITLE[Inf_FAQ		],
      [Rsc_LINKS	] = &Txt_INFO_TITLE[Inf_LINKS		],
      [Rsc_ASSESSMENT	] = &Txt_INFO_TITLE[Inf_ASSESSMENT	],
      [Rsc_TIMETABLE	] = &Txt_TIMETABLE_TYPES[Tmt_COURSE_TIMETABLE],
      [Rsc_ASSIGNMENT	] = &Txt_Assignments,
      [Rsc_PROJECT	] = &Txt_Projects,
      [Rsc_CALL_FOR_EXAM] = &Txt_Calls_for_exams,
      [Rsc_TEST		] = &Txt_Test,
      [Rsc_EXAM		] = &Txt_Exams,
      [Rsc_GAME		] = &Txt_Games,
      [Rsc_RUBRIC	] = &Txt_Rubrics,
      [Rsc_DOCUMENT	] = &Txt_Documents,
      [Rsc_MARKS	] = &Txt_Marks_area,
      [Rsc_GROUPS	] = &Txt_Groups,
      [Rsc_ATT_EVENT	] = &Txt_Control_of_class_attendance,
      [Rsc_FORUM_THREAD	] = &Txt_Course_forum,
      [Rsc_SURVEY	] = &Txt_Surveys,
     };

   /***** Reset title *****/
   Title[0] = '\0';

   /***** Get title *****/
   if (Link->Type != Rsc_NONE)
     {
      if (Link->Cod > 0)
	{
	 if (GetTitle[Link->Type])
	    GetTitle[Link->Type] (Link->Cod,Title,Rsc_MAX_BYTES_RESOURCE_TITLE);
	 else
	    Ale_ShowAlert (Ale_ERROR,"Not implemented!");
	}
      else
	{
	 if (GenericTitle[Link->Type])
	    Str_Copy (Title,*(GenericTitle[Link->Type]),Rsc_MAX_BYTES_RESOURCE_TITLE);
	 else
	    Ale_ShowAlert (Ale_ERROR,"Not implemented!");
	}
     }
  }

/*****************************************************************************/
/********************** Get resource data from clipboard *********************/
/*****************************************************************************/

void Rsc_GetLinkDataFromRow (MYSQL_RES *mysql_res,struct Rsc_Link *Link)
  {
   MYSQL_ROW row;

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);
   /*
   Type	row[0]
   Cod	row[1]
   */
   /***** Get type (row[0]) *****/
   Link->Type = Rsc_GetTypeFromString (row[0]);

   /***** Get code (row[1]) *****/
   Link->Cod = Str_ConvertStrCodToLongCod (row[1]);
  }

/*****************************************************************************/
/********************** Convert from string to type **************************/
/*****************************************************************************/

Rsc_Type_t Rsc_GetTypeFromString (const char *Str)
  {
   Rsc_Type_t Type;

   /***** Compare string with all string types *****/
   for (Type  = (Rsc_Type_t) 0;
	Type <= (Rsc_Type_t) (Rsc_NUM_TYPES - 1);
	Type++)
      if (!strcmp (Rsc_ResourceTypesDB[Type],Str))
	 return Type;

   return Rsc_NONE;
  }

/*****************************************************************************/
/********************** Get resource data from clipboard *********************/
/*****************************************************************************/

bool Rsc_GetParLink (struct Rsc_Link *Link)
  {
   char TypeCod[3 + 1 + Cns_MAX_DIGITS_LONG + 1];
   char TypeStr[3 + 1];
   long Cod;

   /***** Get link type and code *****/
   Par_GetParText ("Link",TypeCod,sizeof (TypeCod) - 1);
   if (sscanf (TypeCod,"%3s_%ld",TypeStr,&Cod) == 2)
     {
      /* Correct link found */
      Link->Type = Rsc_GetTypeFromString (TypeStr);
      Link->Cod  = Cod;
      return true;
     }

   /* No link found */
   Link->Type = Rsc_NONE;
   Link->Cod  = -1L;
   return false;
  }

/*****************************************************************************/
/************************** Check if I can get link **************************/
/*****************************************************************************/

Usr_Can_t Rsc_CheckIfICanGetLink (void)
  {
   static Usr_Can_t ICanGetLink[Rol_NUM_ROLES] =
     {
      [Rol_TCH    ] = Usr_CAN,
      [Rol_SYS_ADM] = Usr_CAN,
     };

   return ICanGetLink[Gbl.Usrs.Me.Role.Logged];
  }
