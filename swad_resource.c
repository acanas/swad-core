// swad_resource.c: resources for course program and rubrics

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Ca�as Vargas

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

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_assignment_resource.h"
#include "swad_attendance_resource.h"
#include "swad_browser_resource.h"
#include "swad_call_for_exam_resource.h"
#include "swad_database.h"
#include "swad_exam_resource.h"
#include "swad_forum_resource.h"
#include "swad_game_resource.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_project_resource.h"
#include "swad_resource.h"
#include "swad_resource_database.h"
#include "swad_role.h"
#include "swad_rubric_resource.h"
#include "swad_survey_resource.h"
#include "swad_theme.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

const char *Rsc_ResourceTypesDB[Rsc_NUM_TYPES] =
  {
   [Rsc_NONE            ] = "non",
   // gui TEACHING_GUIDE	// Link to teaching guide
   // bib BIBLIOGRAPHY		// Link to bibliography
   // faq FAQ			// Link to FAQ
   // lnk LINKS			// Link to links
   // tmt TIMETABLE		// Link to timetable
   [Rsc_ASSIGNMENT      ] = "asg",
   [Rsc_PROJECT         ] = "prj",
   [Rsc_CALL_FOR_EXAM   ] = "cfe",
   // tst TEST			// User selects tags, teacher should select
   [Rsc_EXAM            ] = "exa",
   [Rsc_GAME            ] = "gam",
   [Rsc_RUBRIC          ] = "rub",
   [Rsc_DOCUMENT        ] = "doc",
   [Rsc_MARKS           ] = "mrk",
   // grp GROUPS		// ??? User select groups
   [Rsc_ATTENDANCE_EVENT] = "att",
   [Rsc_FORUM_THREAD    ] = "for",
   [Rsc_SURVEY          ] = "svy",
  };

const char *Rsc_ResourceTypesIcons[Rsc_NUM_TYPES] =
  {
   [Rsc_NONE            ] = "link-slash.svg",
   // gui TEACHING_GUIDE	// Link to teaching guide
   // bib BIBLIOGRAPHY		// Link to bibliography
   // faq FAQ			// Link to FAQ
   // lnk LINKS			// Link to links
   // tmt TIMETABLE		// Link to timetable
   [Rsc_ASSIGNMENT      ] = "edit.svg",
   [Rsc_PROJECT         ] = "file-invoice.svg",
   [Rsc_CALL_FOR_EXAM   ] = "bullhorn.svg",
   // tst TEST			// User selects tags, teacher should select
   [Rsc_EXAM            ] = "file-signature.svg",
   [Rsc_GAME            ] = "gamepad.svg",
   [Rsc_RUBRIC          ] = "tasks.svg",
   [Rsc_DOCUMENT        ] = "file.svg",
   [Rsc_MARKS           ] = "list-alt.svg",
   // grp GROUPS		// ??? User select groups
   [Rsc_ATTENDANCE_EVENT] = "calendar-check.svg",
   [Rsc_FORUM_THREAD    ] = "comments.svg",
   [Rsc_SURVEY          ] = "poll.svg",
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Rsc_GetResourceEmptyTitle (__attribute__((unused)) long Cod,
                                       char *Title,size_t TitleSize);

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
	    Rsc_WriteLinkName (&Link,Frm_PUT_FORM_TO_GO);
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
				HTM_DONT_SUBMIT_ON_CLICK,
				true);	// Checked

	 /***** Row with empty link to remove the current link *****/
	 if (CurrentLink->Type != Rsc_NONE)
	    Rsc_WriteRowClipboard (&EmptyLink,
				   HTM_SUBMIT_ON_CLICK,
				   false);	// Not checked
	}
      else		// Inside form to create a new element
	 /***** Row with empty link *****/
	 Rsc_WriteRowClipboard (&EmptyLink,
				HTM_DONT_SUBMIT_ON_CLICK,
				true);	// Checked

      /***** Get links in clipboard from database and write them *****/
      NumLinks = Rsc_DB_GetClipboard (&mysql_res);
      for (NumLink  = 1;
	   NumLink <= NumLinks;
	   NumLink++)
	{
	 Rsc_GetLinkDataFromRow (mysql_res,&Link);
	 Rsc_WriteRowClipboard (&Link,
				HTM_SUBMIT_ON_CLICK,
				false);	// Checked
	}
      DB_FreeMySQLResult (&mysql_res);

   /***** End list *****/
   HTM_UL_End ();
  }

/*****************************************************************************/
/************************ Show one link from clipboard ***********************/
/*****************************************************************************/

void Rsc_WriteRowClipboard (const struct Rsc_Link *Link,
                            HTM_SubmitOnClick_t SubmitOnClick,bool Checked)
  {
   /***** Begin list row *****/
   HTM_LI_Begin ("class=\"PRG_RSC_%s\"",The_GetSuffix ());
      HTM_LABEL_Begin (NULL);

         /***** Radio selector *****/
	 HTM_INPUT_RADIO ("Link",SubmitOnClick,
			  "value=\"%s_%ld\"%s",
			  Rsc_ResourceTypesDB[Link->Type],Link->Cod,
			  Checked ? " checked=\"checked\"" :
				    "");

	 /***** Name *****/
         Rsc_WriteLinkName (Link,Frm_DONT_PUT_FORM_TO_GO);

      HTM_LABEL_End ();

   /***** End list row *****/
   HTM_LI_End ();
  }

/*****************************************************************************/
/************* Write link name (filename, assignment title...) ***************/
/*****************************************************************************/

void Rsc_WriteLinkName (const struct Rsc_Link *Link,Frm_PutFormToGo_t PutFormToGo)
  {
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];
   extern const char *Rsc_ResourceTypesIcons[Rsc_NUM_TYPES];
   extern const char *Txt_RESOURCE_TYPES[Rsc_NUM_TYPES];
   static void (*WriteLinkName[Rsc_NUM_TYPES]) (long Cod,Frm_PutFormToGo_t PutFormToGo,
						const char *Icon,
						const char *IconTitle) =
     {
      [Rsc_NONE            ] = Rsc_WriteResourceEmpty,
      [Rsc_ASSIGNMENT      ] = AsgRsc_WriteResourceAssignment,
      [Rsc_PROJECT         ] = PrjRsc_WriteResourceProject,
      [Rsc_CALL_FOR_EXAM   ] = CfeRsc_WriteResourceCallForExam,
      [Rsc_EXAM            ] = ExaRsc_WriteResourceExam,
      [Rsc_GAME            ] = GamRsc_WriteResourceGame,
      [Rsc_RUBRIC          ] = RubRsc_WriteResourceRubric,
      [Rsc_DOCUMENT        ] = BrwRsc_WriteResourceDocument,
      [Rsc_MARKS           ] = BrwRsc_WriteResourceMarksFile,
      [Rsc_ATTENDANCE_EVENT] = AttRsc_WriteResourceEvent,
      [Rsc_FORUM_THREAD    ] = ForRsc_WriteResourceThread,
      [Rsc_SURVEY          ] = SvyRsc_WriteResourceSurvey,
     };
   static void (*GetResourceTitle[Rsc_NUM_TYPES]) (long Cod,char *Title,size_t TitleSize) =
     {
      [Rsc_NONE            ] = Rsc_GetResourceEmptyTitle,
      [Rsc_ASSIGNMENT      ] = AsgRsc_GetTitleFromAsgCod,
      [Rsc_PROJECT         ] = PrjRsc_GetTitleFromPrjCod,
      [Rsc_CALL_FOR_EXAM   ] = CfeRsc_GetTitleFromExaCod,
      [Rsc_EXAM            ] = ExaRsc_GetTitleFromExaCod,
      [Rsc_GAME            ] = GamRsc_GetTitleFromGamCod,
      [Rsc_RUBRIC          ] = RubRsc_GetTitleFromRubCod,
      [Rsc_DOCUMENT        ] = BrwRsc_GetTitleFromDocFilCod,
      [Rsc_MARKS           ] = BrwRsc_GetTitleFromMrkFilCod,
      [Rsc_ATTENDANCE_EVENT] = AttRsc_GetTitleFromAttCod,
      [Rsc_FORUM_THREAD    ] = ForRsc_GetTitleFromThrCod,
      [Rsc_SURVEY          ] = SvyRsc_GetTitleFromSvyCod,
     };
   static struct
     {
      void (*Set) (long Cod,char **Anchor);
      void (*Free) (char **Anchor);
     } FuncAnchor[Rsc_NUM_TYPES] =
     {
      [Rsc_NONE            ] = {NULL,NULL},
      [Rsc_ASSIGNMENT      ] = {NULL,NULL},
      [Rsc_PROJECT         ] = {NULL,NULL},
      [Rsc_CALL_FOR_EXAM   ] = {Frm_SetAnchorStr,Frm_FreeAnchorStr},
      [Rsc_EXAM            ] = {NULL,NULL},
      [Rsc_GAME            ] = {NULL,NULL},
      [Rsc_RUBRIC          ] = {NULL,NULL},
      [Rsc_DOCUMENT        ] = {NULL,NULL},
      [Rsc_MARKS           ] = {NULL,NULL},
      [Rsc_ATTENDANCE_EVENT] = {NULL,NULL},
      [Rsc_FORUM_THREAD    ] = {ForRsc_SetAnchorStr,ForRsc_FreeAnchorStr},
      [Rsc_SURVEY          ] = {NULL,NULL},
     };
   static struct
     {
      Act_Action_t IfCod;
      Act_Action_t IfNotCod;
     } NextActions[Rsc_NUM_TYPES] =
     {
      [Rsc_NONE            ] = {ActUnk			,ActUnk			},
      [Rsc_ASSIGNMENT      ] = {ActSeeOneAsg		,ActSeeAsg		},
      [Rsc_PROJECT         ] = {ActSeeOnePrj		,ActSeePrj		},
      [Rsc_CALL_FOR_EXAM   ] = {ActSeeOneCfe		,ActSeeAllCfe		},
      [Rsc_EXAM            ] = {ActSeeExa		,ActSeeAllExa		},
      [Rsc_GAME            ] = {ActSeeGam		,ActSeeAllGam		},
      [Rsc_RUBRIC          ] = {ActSeeRub		,ActSeeAllRub		},
      [Rsc_DOCUMENT        ] = {ActReqDatSeeDocCrs	,ActSeeAdmDocCrsGrp	},
      [Rsc_MARKS           ] = {ActReqDatSeeMrkCrs	,ActSeeAdmMrk		},
      [Rsc_ATTENDANCE_EVENT] = {ActSeeOneAtt		,ActSeeAtt		},
      [Rsc_FORUM_THREAD    ] = {ActSeePstForCrsUsr	,ActSeeForCrsUsr	},
      [Rsc_SURVEY          ] = {ActSeeSvy		,ActSeeAllSvy		},
     };
   static ParCod_Param_t ParCod[Rsc_NUM_TYPES] =
     {
      [Rsc_NONE            ] = ParCod_None,
      [Rsc_ASSIGNMENT      ] = ParCod_Asg,
      [Rsc_PROJECT         ] = ParCod_Prj,
      [Rsc_CALL_FOR_EXAM   ] = ParCod_Exa,
      [Rsc_EXAM            ] = ParCod_Exa,
      [Rsc_GAME            ] = ParCod_Gam,
      [Rsc_RUBRIC          ] = ParCod_Rub,
      [Rsc_DOCUMENT        ] = ParCod_Fil,
      [Rsc_MARKS           ] = ParCod_Fil,
      [Rsc_ATTENDANCE_EVENT] = ParCod_Att,
      [Rsc_FORUM_THREAD    ] = ParCod_Thr,
      [Rsc_SURVEY          ] = ParCod_Svy,
     };
   Act_Action_t NextAction;
   char Title[Cns_MAX_BYTES_SUBJECT + 1];
   char *Anchor;

   /***** Write link name *****/
   if (WriteLinkName[Link->Type])
     {
      GetResourceTitle[Link->Type] (Link->Cod,Title,sizeof (Title) - 1);

      /***** Begin form to go to resource *****/
      if (PutFormToGo == Frm_PUT_FORM_TO_GO)
	{
	 NextAction = (Link->Cod > 0) ? NextActions[Link->Type].IfCod :		// Resource specified
				        NextActions[Link->Type].IfNotCod;	// All resources of this type
         if (FuncAnchor[Link->Type].Set)
           {
            FuncAnchor[Link->Type].Set (Link->Cod,&Anchor);
	    Frm_BeginFormAnchor (NextAction,Anchor);
            FuncAnchor[Link->Type].Free (&Anchor);
           }
         else
            Frm_BeginForm (NextAction);

	 if (Link->Cod > 0)
	    ParCod_PutPar (ParCod[Link->Type],Link->Cod);

	 HTM_BUTTON_Submit_Begin (Txt_Actions[NextAction],
				  "class=\"LM BT_LINK PRG_LNK_%s\"",
				  The_GetSuffix ());
	}

      /***** Icon depending on type ******/
      switch (PutFormToGo)
	{
	 case Frm_DONT_PUT_FORM_TO_GO:
	    Ico_PutIconOn (Rsc_ResourceTypesIcons[Link->Type],Ico_BLACK,Txt_RESOURCE_TYPES[Link->Type]);
	    break;
	 case Frm_PUT_FORM_TO_GO:
	    Ico_PutIconLink (Rsc_ResourceTypesIcons[Link->Type],Ico_BLACK,NextAction);
	    break;
	}

      /***** Write title of resource*****/
      HTM_Txt (Title);

      /***** End form to download file *****/
      if (PutFormToGo == Frm_PUT_FORM_TO_GO)
	{
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
	}
     }
   else
      Ale_ShowAlert (Ale_ERROR,"Not implemented!");
  }

/*****************************************************************************/
/******************** Write empty resource as resource ***********************/
/*****************************************************************************/

void Rsc_WriteResourceEmpty (__attribute__((unused)) long Cod,
                             __attribute__((unused)) Frm_PutFormToGo_t PutFormToGo,
                             const char *Icon,const char *IconTitle)
  {
   extern const char *Txt_RESOURCE_TYPES[Rsc_NUM_TYPES];

   /***** Icon depending on type ******/
   Ico_PutIconOn (Icon,Ico_BLACK,IconTitle);

   /***** Write text *****/
   HTM_Txt (Txt_RESOURCE_TYPES[Rsc_NONE]);
  }

/*****************************************************************************/
/******************** Write empty resource as resource ***********************/
/*****************************************************************************/
// The trailing null character is not counted in TitleSize

static void Rsc_GetResourceEmptyTitle (__attribute__((unused)) long Cod,
                                       char *Title,size_t TitleSize)
  {
   extern const char *Txt_RESOURCE_TYPES[Rsc_NUM_TYPES];

   Str_Copy (Title,Txt_RESOURCE_TYPES[Rsc_NONE],TitleSize);
  }

/*****************************************************************************/
/************* Get the title for a new resource from link title **************/
/*****************************************************************************/

void Rsc_GetResourceTitleFromLink (struct Rsc_Link *Link,
                                   char Title[Rsc_MAX_BYTES_RESOURCE_TITLE + 1])
  {
   static void (*GetTitle[Rsc_NUM_TYPES]) (long Cod,char *Title,size_t TitleSize) =
     {
      [Rsc_NONE            ] = NULL,
      [Rsc_ASSIGNMENT      ] = AsgRsc_GetTitleFromAsgCod,
      [Rsc_PROJECT         ] = PrjRsc_GetTitleFromPrjCod,
      [Rsc_CALL_FOR_EXAM   ] = CfeRsc_GetTitleFromExaCod,
      [Rsc_EXAM            ] = ExaRsc_GetTitleFromExaCod,
      [Rsc_GAME            ] = GamRsc_GetTitleFromGamCod,
      [Rsc_RUBRIC          ] = RubRsc_GetTitleFromRubCod,
      [Rsc_DOCUMENT        ] = BrwRsc_GetTitleFromDocFilCod,
      [Rsc_MARKS           ] = BrwRsc_GetTitleFromMrkFilCod,
      [Rsc_ATTENDANCE_EVENT] = AttRsc_GetTitleFromAttCod,
      [Rsc_FORUM_THREAD    ] = ForRsc_GetTitleFromThrCod,
      [Rsc_SURVEY          ] = SvyRsc_GetTitleFromSvyCod,
     };

   /***** Reset title *****/
   Title[0] = '\0';

   /***** Get title *****/
   if (GetTitle[Link->Type])
      GetTitle[Link->Type] (Link->Cod,Title,Rsc_MAX_BYTES_RESOURCE_TITLE);
   else
      Ale_ShowAlert (Ale_ERROR,"Not implemented!");
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
   char TypeCod[3 + 1 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];
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

bool Rsc_CheckIfICanGetLink (void)
  {
   static const bool ICanGetLink[Rol_NUM_ROLES] =
     {
      [Rol_TCH    ] = true,
      [Rol_SYS_ADM] = true,
     };

   return ICanGetLink[Gbl.Usrs.Me.Role.Logged];
  }
