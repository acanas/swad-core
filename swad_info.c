// swad_info.c: info about course

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include <limits.h>		// For maximum values
#include <linux/limits.h>	// For PATH_MAX, NAME_MAX
#include <stddef.h>		// For NULL
#include <stdlib.h>		// For getenv, etc
#include <stdsoap2.h>		// For SOAP_OK and soap functions
#include <string.h>		// For string functions
#include <unistd.h>		// For unlink

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_info.h"
#include "swad_info_database.h"
#include "swad_parameter.h"
#include "swad_string.h"
#include "swad_tree.h"
#include "swad_tree_database.h"
#include "swad_tree_specific.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

Inf_Type_t Inf_Types[Inf_NUM_TYPES] =
  {
   [Inf_UNKNOWN_TYPE	] = Inf_UNKNOWN_TYPE,
   [Inf_INFORMATION	] = Inf_INFORMATION,
   [Inf_PROGRAM		] = Inf_PROGRAM,
   [Inf_TEACH_GUIDE	] = Inf_TEACH_GUIDE,
   [Inf_SYLLABUS_LEC	] = Inf_SYLLABUS_LEC,
   [Inf_SYLLABUS_PRA	] = Inf_SYLLABUS_PRA,
   [Inf_BIBLIOGRAPHY	] = Inf_BIBLIOGRAPHY,
   [Inf_FAQ		] = Inf_FAQ,
   [Inf_LINKS		] = Inf_LINKS,
   [Inf_ASSESSMENT	] = Inf_ASSESSMENT,
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *Inf_FileNamesForInfoType[Inf_NUM_TYPES] =
  {
   [Inf_UNKNOWN_TYPE	] = NULL,
   [Inf_INFORMATION	] = Cfg_CRS_INFO_INFORMATION,
   [Inf_PROGRAM		] = NULL,	// Not used
   [Inf_TEACH_GUIDE	] = Cfg_CRS_INFO_TEACHING_GUIDE,
   [Inf_SYLLABUS_LEC	] = Cfg_CRS_INFO_LECTURES,
   [Inf_SYLLABUS_PRA	] = Cfg_CRS_INFO_PRACTICALS,
   [Inf_BIBLIOGRAPHY	] = Cfg_CRS_INFO_BIBLIOGRAPHY,
   [Inf_FAQ		] = Cfg_CRS_INFO_FAQ,
   [Inf_LINKS		] = Cfg_CRS_INFO_LINKS,
   [Inf_ASSESSMENT	] = Cfg_CRS_INFO_ASSESSMENT,
  };

static Act_Action_t Inf_ActionsInfo[Inf_NUM_SOURCES][Inf_NUM_TYPES] =
  {
   [Inf_SRC_NONE	][Inf_UNKNOWN_TYPE	] = ActUnk,
   [Inf_SRC_NONE	][Inf_INFORMATION	] = ActUnk,
   [Inf_SRC_NONE	][Inf_PROGRAM		] = ActUnk,
   [Inf_SRC_NONE	][Inf_TEACH_GUIDE	] = ActUnk,
   [Inf_SRC_NONE	][Inf_SYLLABUS_LEC	] = ActUnk,
   [Inf_SRC_NONE	][Inf_SYLLABUS_PRA	] = ActUnk,
   [Inf_SRC_NONE	][Inf_BIBLIOGRAPHY	] = ActUnk,
   [Inf_SRC_NONE	][Inf_FAQ		] = ActUnk,
   [Inf_SRC_NONE	][Inf_LINKS		] = ActUnk,
   [Inf_SRC_NONE	][Inf_ASSESSMENT	] = ActUnk,

   [Inf_EDITOR		][Inf_UNKNOWN_TYPE	] = ActUnk,
   [Inf_EDITOR		][Inf_INFORMATION	] = ActEdiTreInf,
   [Inf_EDITOR		][Inf_PROGRAM		] = ActEdiTrePrg,
   [Inf_EDITOR		][Inf_TEACH_GUIDE	] = ActEdiTreGui,
   [Inf_EDITOR		][Inf_SYLLABUS_LEC	] = ActEdiTreSyl,
   [Inf_EDITOR		][Inf_SYLLABUS_PRA	] = ActEdiTreSyl,
   [Inf_EDITOR		][Inf_BIBLIOGRAPHY	] = ActEdiTreBib,
   [Inf_EDITOR		][Inf_FAQ		] = ActEdiTreFAQ,
   [Inf_EDITOR		][Inf_LINKS		] = ActEdiTreLnk,
   [Inf_EDITOR		][Inf_ASSESSMENT	] = ActEdiTreAss,

   [Inf_PLAIN_TEXT	][Inf_UNKNOWN_TYPE	] = ActUnk,
   [Inf_PLAIN_TEXT	][Inf_INFORMATION	] = ActEdiPlaTxtCrsInf,
   [Inf_PLAIN_TEXT	][Inf_PROGRAM		] = ActUnk,	// Not used
   [Inf_PLAIN_TEXT	][Inf_TEACH_GUIDE	] = ActEdiPlaTxtTchGui,
   [Inf_PLAIN_TEXT	][Inf_SYLLABUS_LEC	] = ActEdiPlaTxtSyl,
   [Inf_PLAIN_TEXT	][Inf_SYLLABUS_PRA	] = ActEdiPlaTxtSyl,
   [Inf_PLAIN_TEXT	][Inf_BIBLIOGRAPHY	] = ActEdiPlaTxtBib,
   [Inf_PLAIN_TEXT	][Inf_FAQ		] = ActEdiPlaTxtFAQ,
   [Inf_PLAIN_TEXT	][Inf_LINKS		] = ActEdiPlaTxtCrsLnk,
   [Inf_PLAIN_TEXT	][Inf_ASSESSMENT	] = ActEdiPlaTxtAss,

   [Inf_RICH_TEXT	][Inf_UNKNOWN_TYPE	] = ActUnk,
   [Inf_RICH_TEXT	][Inf_INFORMATION	] = ActEdiRchTxtCrsInf,
   [Inf_RICH_TEXT	][Inf_PROGRAM		] = ActUnk,	// Not used
   [Inf_RICH_TEXT	][Inf_TEACH_GUIDE	] = ActEdiRchTxtTchGui,
   [Inf_RICH_TEXT	][Inf_SYLLABUS_LEC	] = ActEdiRchTxtSyl,
   [Inf_RICH_TEXT	][Inf_SYLLABUS_PRA	] = ActEdiRchTxtSyl,
   [Inf_RICH_TEXT	][Inf_BIBLIOGRAPHY	] = ActEdiRchTxtBib,
   [Inf_RICH_TEXT	][Inf_FAQ		] = ActEdiRchTxtFAQ,
   [Inf_RICH_TEXT	][Inf_LINKS		] = ActEdiRchTxtCrsLnk,
   [Inf_RICH_TEXT	][Inf_ASSESSMENT	] = ActEdiRchTxtAss,

   [Inf_PAGE		][Inf_UNKNOWN_TYPE	] = ActUnk,
   [Inf_PAGE		][Inf_INFORMATION	] = ActEdiPagCrsInf,
   [Inf_PAGE		][Inf_PROGRAM		] = ActUnk,	// Not used
   [Inf_PAGE		][Inf_TEACH_GUIDE	] = ActEdiPagTchGui,
   [Inf_PAGE		][Inf_SYLLABUS_LEC	] = ActEdiPagSyl,
   [Inf_PAGE		][Inf_SYLLABUS_PRA	] = ActEdiPagSyl,
   [Inf_PAGE		][Inf_BIBLIOGRAPHY	] = ActEdiPagBib,
   [Inf_PAGE		][Inf_FAQ		] = ActEdiPagFAQ,
   [Inf_PAGE		][Inf_LINKS		] = ActEdiPagCrsLnk,
   [Inf_PAGE		][Inf_ASSESSMENT	] = ActEdiPagAss,

   [Inf_URL		][Inf_UNKNOWN_TYPE	] = ActUnk,
   [Inf_URL		][Inf_INFORMATION	] = ActEdiURLCrsInf,
   [Inf_URL		][Inf_PROGRAM		] = ActUnk,	// Not used
   [Inf_URL		][Inf_TEACH_GUIDE	] = ActEdiURLTchGui,
   [Inf_URL		][Inf_SYLLABUS_LEC	] = ActEdiURLSyl,
   [Inf_URL		][Inf_SYLLABUS_PRA	] = ActEdiURLSyl,
   [Inf_URL		][Inf_BIBLIOGRAPHY	] = ActEdiURLBib,
   [Inf_URL		][Inf_FAQ		] = ActEdiURLFAQ,
   [Inf_URL		][Inf_LINKS		] = ActEdiURLCrsLnk,
   [Inf_URL		][Inf_ASSESSMENT	] = ActEdiURLAss,
  };

static Act_Action_t Inf_ActionsCfg[Inf_NUM_TYPES] =
  {
   [Inf_UNKNOWN_TYPE	] = ActUnk,
   [Inf_INFORMATION	] = ActCfgCrsInf,
   [Inf_PROGRAM		] = ActUnk,	// Not used
   [Inf_TEACH_GUIDE	] = ActCfgTchGui,
   [Inf_SYLLABUS_LEC	] = ActCfgSyl,
   [Inf_SYLLABUS_PRA	] = ActCfgSyl,
   [Inf_BIBLIOGRAPHY	] = ActCfgBib,
   [Inf_FAQ		] = ActCfgFAQ,
   [Inf_LINKS		] = ActCfgCrsLnk,
   [Inf_ASSESSMENT	] = ActCfgAss,
  };

static Act_Action_t Inf_ActionsReqLnk[Inf_NUM_TYPES] =
  {
   [Inf_UNKNOWN_TYPE	] = ActUnk,
   [Inf_INFORMATION	] = ActReqLnkCrsInf,
   [Inf_PROGRAM		] = ActUnk,	// Not used
   [Inf_TEACH_GUIDE	] = ActReqLnkTchGui,
   [Inf_SYLLABUS_LEC	] = ActReqLnkSyl,
   [Inf_SYLLABUS_PRA	] = ActReqLnkSyl,
   [Inf_BIBLIOGRAPHY	] = ActReqLnkBib,
   [Inf_FAQ		] = ActReqLnkFAQ,
   [Inf_LINKS		] = ActReqLnkCrsLnk,
   [Inf_ASSESSMENT	] = ActReqLnkAss,
  };

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static struct
  {
   Inf_MustBeRead_t MustBeRead[Inf_NUM_TYPES];	// Students must read info?
   Lay_Show_t ShowMsgMustBeRead;
  } Inf_InfoMustBeRead;

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Inf_BeforeTree (struct Inf_Info *Info,Vie_ViewType_t ViewType,Inf_Src_t InfoSrc);
static void Inf_AfterTree (void);

static void Inf_PutFormWhichSyllabus (Inf_Type_t InfoType,Vie_ViewType_t ViewType);

static void Inf_PutIconsWhenViewing (void *Info);
static void Inf_PutIconsWhenEditing (void *Info);
static void Inf_PutIconsWhenConfiguring (void *Info);

static Exi_Exist_t Inf_CheckPage (Inf_Type_t InfoType);
static Exi_Exist_t Inf_CheckAndShowPage (Inf_Type_t InfoType);

static Exi_Exist_t Inf_CheckURL (Inf_Type_t InfoType);
static Exi_Exist_t Inf_CheckAndShowURL (Inf_Type_t InfoType);
static void Inf_BuildPathURL (long HieCod,Inf_Type_t InfoType,
                              char PathFile[PATH_MAX + 1]);

static void Inf_ShowPage (const char *URL);

//---------------------- Force students to read info? -------------------------
static void Inf_ConfigInfoReading (const struct Inf_Info *Info);
static void Inf_PutCheckboxForceStdsToReadInfo (const struct Inf_Info *Info,
						HTM_Attributes_t Attributes);
static void Inf_PutCheckboxConfirmIHaveReadInfo (Inf_Type_t InfoType);
static Inf_MustBeRead_t Inf_GetMustBeReadFromForm (void);
static Inf_IHaveRead_t Inf_GetIfIHaveReadFromForm (void);

//-----------------------------------------------------------------------------
static void Inf_ConfigInfoSource (struct Inf_Info *Info);

static Exi_Exist_t Inf_CheckIfInfoAvailable (Inf_Type_t InfoType,Inf_Src_t InfoSrc);

static void Inf_FormToEnterEditor (Inf_Type_t InfoType,Inf_Src_t InfoSrc);
static void Inf_FormToEnterPageUploader (Inf_Type_t InfoType,Inf_Src_t InfoSrc);

static Exi_Exist_t Inf_CheckPlainTxt (Inf_Type_t InfoType);
static Exi_Exist_t Inf_CheckAndShowPlainTxt (Inf_Type_t InfoType);

static Exi_Exist_t Inf_CheckRichTxt (Inf_Type_t InfoType);
static Exi_Exist_t Inf_CheckAndShowRichTxt (Inf_Type_t InfoType);

/*****************************************************************************/
/*************************** Before and after tree ***************************/
/*****************************************************************************/

static void Inf_BeforeTree (struct Inf_Info *Info,Vie_ViewType_t ViewType,Inf_Src_t InfoSrc)
  {
   extern const char *Hlp_COURSE_Information_textual_information;
   extern const char *Hlp_COURSE_Program;
   extern const char *Hlp_COURSE_Guide;
   extern const char *Hlp_COURSE_Syllabus;
   extern const char *Hlp_COURSE_Bibliography;
   extern const char *Hlp_COURSE_FAQ;
   extern const char *Hlp_COURSE_Links;
   extern const char *Hlp_COURSE_Assessment;
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   static void (*FunctionToDrawContextualIcons[Vie_NUM_VIEW_TYPES]) (void *Args) =
     {
      [Vie_VIEW		] = Inf_PutIconsWhenViewing,
      [Vie_EDIT		] = Inf_PutIconsWhenEditing,
      [Vie_CONFIG	] = Inf_PutIconsWhenConfiguring,
      [Vie_PRINT	] = NULL,
     };
   static const char **Help[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = &Hlp_COURSE_Information_textual_information,
      [Inf_PROGRAM	] = &Hlp_COURSE_Program,
      [Inf_TEACH_GUIDE	] = &Hlp_COURSE_Guide,
      [Inf_SYLLABUS_LEC	] = &Hlp_COURSE_Syllabus,
      [Inf_SYLLABUS_PRA	] = &Hlp_COURSE_Syllabus,
      [Inf_BIBLIOGRAPHY	] = &Hlp_COURSE_Bibliography,
      [Inf_FAQ		] = &Hlp_COURSE_FAQ,
      [Inf_LINKS	] = &Hlp_COURSE_Links,
      [Inf_ASSESSMENT	] = &Hlp_COURSE_Assessment,
     };

   /***** Set info type *****/
   Info->Type = Inf_AsignInfoType ();

   /***** Set info source in database *****/
   if (InfoSrc != Inf_SRC_NONE)
      Inf_DB_SetInfoSrc (Info->Type,InfoSrc);

   /***** Get info source and check if info must be read from database *****/
   Inf_GetAndCheckInfoSrcFromDB (Info);

   /***** Begin box *****/
   Box_BoxBegin (Txt_INFO_TITLE[Info->Type],
		 FunctionToDrawContextualIcons[ViewType],Info,
		 *Help[Info->Type],Box_NOT_CLOSABLE);

      /***** Form to select syllabus *****/
      Inf_PutFormWhichSyllabus (Info->Type,ViewType);

      /***** Only for students: Have I read this information? ******/
      if (Info->FromDB.MustBeRead == Inf_MUST_BE_READ &&
	  Gbl.Usrs.Me.Role.Logged == Rol_STD)
        {
	 Mnu_ContextMenuBegin ();
	    Inf_PutCheckboxConfirmIHaveReadInfo (Info->Type);	// Checkbox to confirm that...
								   // ...I have read this couse info
	 Mnu_ContextMenuEnd ();
        }

  }

static void Inf_AfterTree (void)
  {
   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************************ Write form to select syllabus **********************/
/*****************************************************************************/

static void Inf_PutFormWhichSyllabus (Inf_Type_t InfoType,Vie_ViewType_t ViewType)
  {
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   static Act_Action_t Actions[Vie_NUM_VIEW_TYPES] =
     {
      [Vie_VIEW		] = ActSeeSyl,
      [Vie_EDIT		] = ActEdiTreSyl,
      [Vie_CONFIG	] = ActCfgSyl,
      [Vie_PRINT	] = ActUnk,
     };
   Inf_Type_t Type;

   /***** If no syllabus ==> nothing to do *****/
   switch (InfoType)
     {
      case Inf_SYLLABUS_LEC:
      case Inf_SYLLABUS_PRA:
	 break;
      default:	// Nothing to do
	 return;
     }

   /***** Form to select which syllabus I want to see (lectures/practicals) *****/
   Frm_BeginForm (Actions[ViewType]);
      HTM_DIV_Begin ("class=\"SEL_BELOW_TITLE DAT_%s\"",The_GetSuffix ());
	 HTM_UL_Begin (NULL);

	    for (Type  = Inf_SYLLABUS_LEC;
		 Type <= Inf_SYLLABUS_PRA;
		 Type++)
	      {
	       HTM_LI_Begin (NULL);
		  HTM_LABEL_Begin (NULL);
		     HTM_INPUT_RADIO ("WhichSyllabus",
				      (Type == InfoType ? HTM_CHECKED :
							  HTM_NO_ATTR) |
				      HTM_SUBMIT_ON_CLICK,
				      "value=\"%u\"",(unsigned) Type);
		     HTM_Txt (Txt_INFO_TITLE[Type]);
		  HTM_LABEL_End ();
	       HTM_LI_End ();
	      }

	 HTM_UL_End ();
      HTM_DIV_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/******** Show course info (theory, practices, bibliography, etc.) ***********/
/*****************************************************************************/

void Inf_ShowInfo (void)
  {
   extern const char *Txt_No_information;
   static Lay_Show_t Show[Exi_NUM_EXIST] =
     {
      [Exi_DOES_NOT_EXIST] = Lay_SHOW,
      [Exi_EXISTS        ] = Lay_DONT_SHOW,
     };
   static Exi_Exist_t (*Inf_CheckAndShow[Inf_NUM_SOURCES]) (Inf_Type_t InfoType) =
     {
      [Inf_SRC_NONE  ] = NULL,
      [Inf_EDITOR    ] = NULL,
      [Inf_PLAIN_TEXT] = Inf_CheckAndShowPlainTxt,
      [Inf_RICH_TEXT ] = Inf_CheckAndShowRichTxt,
      [Inf_PAGE      ] = Inf_CheckAndShowPage,
      [Inf_URL       ] = Inf_CheckAndShowURL,
     };
   struct Inf_Info Info;
   Lay_Show_t ShowWarningNoInfo = Lay_DONT_SHOW;

   /***** Begin box *****/
   Inf_BeforeTree (&Info,Vie_VIEW,Inf_SRC_NONE);

      switch (Info.FromDB.Src)
	{
	 case Inf_EDITOR:
	    switch (Info.Type)
	      {
	       case Inf_INFORMATION:
	       case Inf_TEACH_GUIDE:
	       case Inf_SYLLABUS_LEC:
	       case Inf_SYLLABUS_PRA:
	       case Inf_BIBLIOGRAPHY:
	       case Inf_FAQ:
	       case Inf_LINKS:
	       case Inf_ASSESSMENT:
		  ShowWarningNoInfo = Tre_ShowTree (Info.Type) == 0 ? Lay_SHOW :
								      Lay_DONT_SHOW;
		  break;
	       case Inf_UNKNOWN_TYPE:
	       case Inf_PROGRAM:
	       default:
		  Err_WrongTypeExit ();
		  break;
	      }
	    break;
	 case Inf_PLAIN_TEXT:
	 case Inf_RICH_TEXT:
	 case Inf_PAGE:
	 case Inf_URL:
	    ShowWarningNoInfo = Show[Inf_CheckAndShow[Info.FromDB.Src] (Info.Type)];
	    break;
	 case Inf_SRC_NONE:
	 default:
	    ShowWarningNoInfo = Lay_SHOW;
	    break;
	}

      if (ShowWarningNoInfo == Lay_SHOW)
	 Ale_ShowAlert (Ale_INFO,Txt_No_information);

   /***** End box *****/
   Inf_AfterTree ();
  }

/*****************************************************************************/
/******** Edit course info (theory, practices, bibliography, etc.) ***********/
/*****************************************************************************/

void Inf_EditTree (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_EditTree (Info.Type);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/********* List tree nodes when click on view a node after edition ***********/
/*****************************************************************************/

void Inf_ViewNodeAfterEdit (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_ViewNodeAfterEdit (Info.Type);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/*********** List tree nodes with a form to change/create a node *************/
/*****************************************************************************/

void Inf_ReqChangeNode (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_ReqChangeNode (Info.Type);
   Inf_AfterTree ();
  }

void Inf_ReqCreateNode (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_ReqCreateNode (Info.Type);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/*************** Receive form to change/create a tree node *******************/
/*****************************************************************************/

void Inf_ReceiveChgNode (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_ReceiveChgNode (Info.Type);
   Inf_AfterTree ();
  }

void Inf_ReceiveNewNode (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_ReceiveNewNode (Info.Type);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/******************** Remove a tree node and its children ********************/
/*****************************************************************************/

void Inf_ReqRemNode (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_ReqRemNode (Info.Type);
   Inf_AfterTree ();
  }

void Inf_RemoveNode (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_RemoveNode (Info.Type);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/*************************** Hide/unhide a tree node *************************/
/*****************************************************************************/

void Inf_HideNode (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_HideOrUnhideNode (Info.Type,HidVis_HIDDEN);
   Inf_AfterTree ();
  }

void Inf_UnhideNode (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_HideOrUnhideNode (Info.Type,HidVis_VISIBLE);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/*********** Move up/down position of a subtree in a course tree *************/
/*****************************************************************************/

void Inf_MoveUpNode (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_MoveUpDownNode (Info.Type,Tre_MOVE_UP);
   Inf_AfterTree ();
  }

void Inf_MoveDownNode (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_MoveUpDownNode (Info.Type,Tre_MOVE_DOWN);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/************** Move a subtree to left/right in a course program *************/
/*****************************************************************************/

void Inf_MoveLeftNode (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_MoveLeftRightNode (Info.Type,Tre_MOVE_LEFT);
   Inf_AfterTree ();
  }

void Inf_MoveRightNode (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_MoveLeftRightNode (Info.Type,Tre_MOVE_RIGHT);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/****************** Expand/contract a node in a course tree ******************/
/*****************************************************************************/

void Inf_ExpandNodeSeeing (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_VIEW,Inf_EDITOR);
      Tre_ExpandContractNode (Info.Type,Tre_EXPAND,Tre_VIEW);
   Inf_AfterTree ();
  }

void Inf_ContractNodeSeeing (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_VIEW,Inf_EDITOR);
      Tre_ExpandContractNode (Info.Type,Tre_CONTRACT,Tre_VIEW);
   Inf_AfterTree ();
  }

void Inf_ExpandNodeEditing (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_ExpandContractNode (Info.Type,Tre_EXPAND,Tre_EDIT_NODES);
   Inf_AfterTree ();
  }

void Inf_ContractNodeEditing (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      Tre_ExpandContractNode (Info.Type,Tre_CONTRACT,Tre_EDIT_NODES);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/************** View specific list of items after editing them ***************/
/*****************************************************************************/

void Inf_ViewListItemsAfterEdit (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      TreSpc_ViewListItemsAfterEdit (Info.Type);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/************************ Edit specific list of items ************************/
/*****************************************************************************/

void Inf_EditListItems (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      TreSpc_EditListItems (Info.Type);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/************************ Create new specific list item **********************/
/*****************************************************************************/

void Inf_CreateItem (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      TreSpc_ChangeItem (Info.Type);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/********** Ask for confirmation of removing an specific list item ***********/
/*****************************************************************************/

void Inf_ReqRemItem (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      TreSpc_ReqRemItem (Info.Type);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/*********************** Remove an specific list item ************************/
/*****************************************************************************/

void Inf_RemoveItem (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      TreSpc_RemoveItem (Info.Type);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/********************** Hide/unhide specific list item ***********************/
/*****************************************************************************/

void Inf_HideItem (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      TreSpc_HideOrUnhideItem (Info.Type,HidVis_HIDDEN);
   Inf_AfterTree ();
  }

void Inf_UnhideItem (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      TreSpc_HideOrUnhideItem (Info.Type,HidVis_VISIBLE);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/*********************** Move up/down specific list item *********************/
/*****************************************************************************/

void Inf_MoveUpItem (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      TreSpc_MoveUpDownItem (Info.Type,TreSpc_UP);
   Inf_AfterTree ();
  }

void Inf_MoveDownItem (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      TreSpc_MoveUpDownItem (Info.Type,TreSpc_DOWN);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/************* Edit tree with form to change specific list item **************/
/*****************************************************************************/

void Inf_EditTreeWithFormItem (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      TreSpc_EditTreeWithFormItem (Info.Type);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/************************ Change specific list item **************************/
/*****************************************************************************/

void Inf_ChangeItem (void)
  {
   struct Inf_Info Info;

   Inf_BeforeTree (&Info,Vie_EDIT,Inf_EDITOR);
      TreSpc_ChangeItem (Info.Type);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/******************** Put icons to view/edit course info *********************/
/*****************************************************************************/

static void Inf_PutIconsWhenViewing (void *Info)
  {
   struct Tre_Node Node;
   Act_Action_t ActionEdit;

   if (Info)
      if (Tre_CheckIfICanEditTree () == Usr_CAN)
        {
	 Node.InfoType = ((struct Inf_Info *) Info)->Type;
	 Tre_ResetNode (&Node);

	 /***** Icon to edit *****/
	 ActionEdit = Inf_ActionsInfo[((struct Inf_Info *) Info)->FromDB.Src][((struct Inf_Info *) Info)->Type];
	 if (ActionEdit != ActUnk)
	    Ico_PutContextualIconToEdit (ActionEdit,NULL,Tre_PutPars,&Node);

	 /***** Icon to configure *****/
	 Ico_PutContextualIconToConfigure (Inf_ActionsCfg[((struct Inf_Info *) Info)->Type],NULL,
					   Tre_PutPars,&Node);

	 /***** Icon to get resource link *****/
	 Ico_PutContextualIconToGetLink (Inf_ActionsReqLnk[((struct Inf_Info *) Info)->Type],NULL,
					 Tre_PutPars,&Node);
        }
  }

static void Inf_PutIconsWhenEditing (void *Info)
  {
   struct Tre_Node Node;

   if (Info)
     {
      Node.InfoType = ((struct Inf_Info *) Info)->Type;
      Tre_ResetNode (&Node);

      /***** Put icon to view program *****/
      Tre_PutIconToViewTree (&Node);

      if (Tre_CheckIfICanEditTree () == Usr_CAN)
        {
	 /***** Put icon to create a new tree node *****/
	 if (((struct Inf_Info *) Info)->FromDB.Src == Inf_EDITOR)
	    Tre_PutIconToCreateNewNode (&Node);

	 /***** Icon to configure *****/
	 Ico_PutContextualIconToConfigure (Inf_ActionsCfg[((struct Inf_Info *) Info)->Type],NULL,
					   Tre_PutPars,&Node);

	 /***** Icon to get resource link *****/
	 Ico_PutContextualIconToGetLink (Inf_ActionsReqLnk[((struct Inf_Info *) Info)->Type],NULL,
					 Tre_PutPars,&Node);
        }
     }
  }

static void Inf_PutIconsWhenConfiguring (void *Info)
  {
   struct Tre_Node Node;
   Act_Action_t ActionEdit;

   if (Info)
     {
      Node.InfoType = ((struct Inf_Info *) Info)->Type;
      Tre_ResetNode (&Node);

      /***** Put icon to view program *****/
      Tre_PutIconToViewTree (&Node);

      if (Tre_CheckIfICanEditTree () == Usr_CAN)
        {
	 /***** Icon to edit *****/
	 ActionEdit = Inf_ActionsInfo[((struct Inf_Info *) Info)->FromDB.Src][((struct Inf_Info *) Info)->Type];
	 if (ActionEdit != ActUnk)
	    Ico_PutContextualIconToEdit (ActionEdit,NULL,Tre_PutPars,&Node);

	 /***** Icon to get resource link *****/
	 Ico_PutContextualIconToGetLink (Inf_ActionsReqLnk[((struct Inf_Info *) Info)->Type],NULL,
					 Tre_PutPars,&Node);
        }
     }
  }

/*****************************************************************************/
/************************** Check if exists a page ***************************/
/*****************************************************************************/
// Return Exi_EXISTS if info available

static Exi_Exist_t Inf_CheckPage (Inf_Type_t InfoType)
  {
   char PathRelDirHTML[PATH_MAX + 1];
   char PathRelFileHTML[PATH_MAX + 1 + 10 + 1];

   // TODO !!!!!!!!!!!! If the page is hosted in server ==> it should be created a temporary public directory
   //                                                       and host the page in a private directory !!!!!!!!!!!!!!!!!

   /***** Build path of directory containing web page *****/
   Inf_BuildPathPage (Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		      InfoType,PathRelDirHTML);

   /***** Open file with web page *****/
   /* 1. Check if index.html exists */
   snprintf (PathRelFileHTML,sizeof (PathRelFileHTML),"%s/index.html",
	     PathRelDirHTML);
   if (Fil_CheckIfPathExists (PathRelFileHTML) == Exi_EXISTS)	// TODO: Check if not empty?
      return Exi_EXISTS;

   /* 2. If index.html does not exist, try index.htm */
   snprintf (PathRelFileHTML,sizeof (PathRelFileHTML),"%s/index.htm",
	     PathRelDirHTML);
   if (Fil_CheckIfPathExists (PathRelFileHTML) == Exi_EXISTS)	// TODO: Check if not empty?
      return Exi_EXISTS;

   return Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/**************** Check if exists and show link to a page ********************/
/*****************************************************************************/
// Return Exi_EXISTS if info available

static Exi_Exist_t Inf_CheckAndShowPage (Inf_Type_t InfoType)
  {
   char PathRelDirHTML[PATH_MAX + 1];
   char PathRelFileHTML[PATH_MAX + 1 + 10 + 1];
   char URL[PATH_MAX + 1];

   // TODO !!!!!!!!!!!! If the page is hosted in server ==> it should be created a temporary public directory
   //                                                       and host the page in a private directory !!!!!!!!!!!!!!!!!

   /***** Build path of directory containing web page *****/
   Inf_BuildPathPage (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,
		      PathRelDirHTML);

   /***** Open file with web page *****/
   /* 1. Check if index.html exists */
   snprintf (PathRelFileHTML,sizeof (PathRelFileHTML),"%s/index.html",
	     PathRelDirHTML);
   if (Fil_CheckIfPathExists (PathRelFileHTML) == Exi_EXISTS)	// TODO: Check if not empty?
     {
      snprintf (URL,sizeof (URL),"%s/%ld/%s/index.html",
	        Cfg_URL_CRS_PUBLIC,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
	        Inf_FileNamesForInfoType[InfoType]);
      Inf_ShowPage (URL);

      return Exi_EXISTS;
     }

   /* 2. If index.html does not exist, try index.htm */
   snprintf (PathRelFileHTML,sizeof (PathRelFileHTML),"%s/index.htm",
	     PathRelDirHTML);
   if (Fil_CheckIfPathExists (PathRelFileHTML) == Exi_EXISTS)	// TODO: Check if not empty?
     {
      snprintf (URL,sizeof (URL),"%s/%ld/%s/index.htm",
	        Cfg_URL_CRS_PUBLIC,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
	        Inf_FileNamesForInfoType[InfoType]);
      Inf_ShowPage (URL);

      return Exi_EXISTS;
     }

   return Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/* Build path inside a course for a given a info type to store web page file */
/*****************************************************************************/

void Inf_BuildPathPage (long HieCod,Inf_Type_t InfoType,char PathDir[PATH_MAX + 1])
  {
   snprintf (PathDir,PATH_MAX + 1,"%s/%ld/%s",
             Cfg_PATH_CRS_PUBLIC,HieCod,Inf_FileNamesForInfoType[InfoType]);
  }

/*****************************************************************************/
/********************* Check if exists link to a page ************************/
/*****************************************************************************/
// Return Exi_EXISTS if info available

static Exi_Exist_t Inf_CheckURL (Inf_Type_t InfoType)
  {
   char URL[WWW_MAX_BYTES_WWW + 1];
   char PathFile[PATH_MAX + 1];
   FILE *FileURL;

   /***** Build path to file containing URL *****/
   Inf_BuildPathURL (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,PathFile);

   /***** Check if file with URL exists *****/
   if ((FileURL = fopen (PathFile,"rb")))
     {
      if (fgets (URL,WWW_MAX_BYTES_WWW,FileURL) == NULL)
	 URL[0] = '\0';
      /* File is not longer needed  ==> close it */
      fclose (FileURL);

      if (URL[0])
         return Exi_EXISTS;
     }

   return Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/**************** Check if exists and show link to a page ********************/
/*****************************************************************************/
// Return Exi_EXISTS if info available

static Exi_Exist_t Inf_CheckAndShowURL (Inf_Type_t InfoType)
  {
   char URL[WWW_MAX_BYTES_WWW + 1];
   char PathFile[PATH_MAX + 1];
   FILE *FileURL;

   /***** Build path to file containing URL *****/
   Inf_BuildPathURL (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,PathFile);

   /***** Check if file with URL exists *****/
   if ((FileURL = fopen (PathFile,"rb")))
     {
      if (fgets (URL,WWW_MAX_BYTES_WWW,FileURL) == NULL)
	 URL[0] = '\0';
      /* File is not longer needed  ==> close it */
      fclose (FileURL);

      if (URL[0])
	{
	 Inf_ShowPage (URL);
         return Exi_EXISTS;
	}
     }

   return Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/*** Build path inside a course for a given a info type to store URL file ****/
/*****************************************************************************/

static void Inf_BuildPathURL (long HieCod,Inf_Type_t InfoType,
                              char PathFile[PATH_MAX + 1])
  {
   snprintf (PathFile,PATH_MAX + 1,"%s/%ld/%s.url",
	     Cfg_PATH_CRS_PRIVATE,HieCod,Inf_FileNamesForInfoType[InfoType]);
  }

/*****************************************************************************/
/************* Check if exists and write URL into text buffer ****************/
/*****************************************************************************/
// This function is called only from web service

void Inf_WriteURLIntoTxtBuffer (Inf_Type_t InfoType,
				char TxtBuffer[WWW_MAX_BYTES_WWW + 1])
  {
   char PathFile[PATH_MAX + 1];
   FILE *FileURL;

   /***** Initialize buffer *****/
   TxtBuffer[0] = '\0';

   /***** Build path to file containing URL *****/
   Inf_BuildPathURL (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,
		     PathFile);

   /***** Check if file with URL exists *****/
   if ((FileURL = fopen (PathFile,"rb")))
     {
      if (fgets (TxtBuffer,WWW_MAX_BYTES_WWW,FileURL) == NULL)
	 TxtBuffer[0] = '\0';
      /* File is not longer needed  ==> close it */
      fclose (FileURL);
     }
  }

/*****************************************************************************/
/*************** Show link to a internal or external a page ******************/
/*****************************************************************************/

static void Inf_ShowPage (const char *URL)
  {
   extern const char *Txt_View_in_a_new_window;

   /***** Link to view in a new window *****/
   HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"FORM_OUT_%s BOLD\"",
		URL,The_GetSuffix ());
      Ico_PutIconTextLink ("expand-arrows-alt.svg",Ico_BLACK,
			   Txt_View_in_a_new_window);
   HTM_A_End ();
  }

/*****************************************************************************/
/************************** Set course info source ***************************/
/*****************************************************************************/

void Inf_SetInfoSrc (void)
  {
   struct Inf_Info Info;
   Inf_Src_t InfoSrcSelected;

   /***** Set info type *****/
   Info.Type = Inf_AsignInfoType ();

   /***** Set info source into database *****/
   InfoSrcSelected = Inf_GetInfoSrcFromForm ();
   Inf_DB_SetInfoSrc (Info.Type,InfoSrcSelected);

   /***** Show the selected info *****/
   Inf_ShowInfo ();
  }

/*****************************************************************************/
/************** Configure course info (reading and info source) **************/
/*****************************************************************************/

void Inf_ConfigInfo (void)
  {
   struct Inf_Info Info;

   /***** Begin box *****/
   Inf_BeforeTree (&Info,Vie_CONFIG,Inf_SRC_NONE);

      /***** Force students to read info? *****/
      Inf_ConfigInfoReading (&Info);

      /***** Information source *****/
      Inf_ConfigInfoSource (&Info);

   /***** End box *****/
   Inf_AfterTree ();
  }

/*****************************************************************************/
/********** Configure info reading (force students to read info?) ************/
/*****************************************************************************/

static void Inf_ConfigInfoReading (const struct Inf_Info *Info)
  {
   extern const char *Txt_Reading;

   HTM_FIELDSET_Begin (NULL);
      HTM_LEGEND (Txt_Reading);
	 // Checkbox to force students to read this couse info
	 Mnu_ContextMenuBegin ();
	    // Non-editing teachers can not change the status of checkbox
	    Inf_PutCheckboxForceStdsToReadInfo (Info,
						Gbl.Usrs.Me.Role.Logged == Rol_NET ? HTM_DISABLED :
										     HTM_NO_ATTR);
	 Mnu_ContextMenuEnd ();
   HTM_FIELDSET_End ();
  }

/*****************************************************************************/
/********** Put a form (checkbox) to force students to read info *************/
/*****************************************************************************/

static void Inf_PutCheckboxForceStdsToReadInfo (const struct Inf_Info *Info,
						HTM_Attributes_t Attributes)
  {
   extern const char *Txt_Force_students_to_read_this_information;
   static struct Act_ActionFunc Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = {ActUnk		,NULL,NULL},
      [Inf_INFORMATION	] = {ActChgFrcReaCrsInf	,NULL,NULL},
      [Inf_PROGRAM	] = {ActUnk		,NULL,NULL},	// Not used
      [Inf_TEACH_GUIDE	] = {ActChgFrcReaTchGui	,NULL,NULL},
      [Inf_SYLLABUS_LEC	] = {ActChgFrcReaSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_LEC]},
      [Inf_SYLLABUS_PRA	] = {ActChgFrcReaSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_PRA]},
      [Inf_BIBLIOGRAPHY	] = {ActChgFrcReaBib	,NULL,NULL},
      [Inf_FAQ		] = {ActChgFrcReaFAQ	,NULL,NULL},
      [Inf_LINKS	] = {ActChgFrcReaCrsLnk	,NULL,NULL},
      [Inf_ASSESSMENT	] = {ActChgFrcReaAss	,NULL,NULL},
     };
   static HTM_Attributes_t AttributesRead[Inf_NUM_MUST_BE_READ] =
     {
      [Inf_DONT_MUST_BE_READ] = HTM_NO_ATTR,
      [Inf_MUST_BE_READ     ] = HTM_CHECKED,
     };

   Lay_PutContextualCheckbox (Actions[Info->Type].NextAction,
                              Actions[Info->Type].FuncPars,
                              Actions[Info->Type].Args,
                              "MustBeRead",
                              Attributes |
                              AttributesRead[Info->FromDB.MustBeRead] |
                              HTM_SUBMIT_ON_CHANGE,
                              Txt_Force_students_to_read_this_information,
                              Txt_Force_students_to_read_this_information);
  }

/*****************************************************************************/
/********** Put a form (checkbox) to force students to read info *************/
/*****************************************************************************/

static void Inf_PutCheckboxConfirmIHaveReadInfo (Inf_Type_t InfoType)
  {
   extern const char *Txt_I_have_read_this_information;
   static struct Act_ActionFunc Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = {ActUnk		,NULL,NULL},
      [Inf_INFORMATION	] = {ActChgHavReaCrsInf	,NULL,NULL},
      [Inf_PROGRAM	] = {ActUnk		,NULL,NULL},	// Not used
      [Inf_TEACH_GUIDE	] = {ActChgHavReaTchGui	,NULL,NULL},
      [Inf_SYLLABUS_LEC	] = {ActChgHavReaSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_LEC]},
      [Inf_SYLLABUS_PRA	] = {ActChgHavReaSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_PRA]},
      [Inf_BIBLIOGRAPHY	] = {ActChgHavReaBib	,NULL,NULL},
      [Inf_FAQ		] = {ActChgHavReaFAQ	,NULL,NULL},
      [Inf_LINKS	] = {ActChgHavReaCrsLnk	,NULL,NULL},
      [Inf_ASSESSMENT	] = {ActChgHavReaAss	,NULL,NULL},
     };
   static HTM_Attributes_t AttributesRead[Inf_NUM_I_HAVE_READ] =
     {
      [Inf_I_DONT_HAVE_READ] = HTM_NO_ATTR,
      [Inf_I_HAVE_READ     ] = HTM_CHECKED,
     };
   Inf_IHaveRead_t IHaveRead = Inf_DB_CheckIfIHaveReadInfo (InfoType);

   Lay_PutContextualCheckbox (Actions[InfoType].NextAction,
			      Actions[InfoType].FuncPars,
			      Actions[InfoType].Args,
			      "IHaveRead",
			      AttributesRead[IHaveRead] |
			      HTM_SUBMIT_ON_CHANGE,
			      Txt_I_have_read_this_information,
			      Txt_I_have_read_this_information);
  }

/*****************************************************************************/
/********* Get if students must read any info about current course ***********/
/*****************************************************************************/

void Inf_GetIfIMustReadAnyCrsInfoInThisCrs (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumInfos;
   unsigned NumInfo;
   Inf_Type_t InfoType;

   /***** Reset must-be-read to false for all info types *****/
   for (InfoType  = (Inf_Type_t) 0;
	InfoType <= (Inf_Type_t) (Inf_NUM_TYPES - 1);
	InfoType++)
      Inf_InfoMustBeRead.MustBeRead[InfoType] = Inf_DONT_MUST_BE_READ;

   /***** Get info types where students must read info *****/
   NumInfos = Inf_DB_GetInfoTypesfIMustReadInfo (&mysql_res);

   /***** Set must-be-read for each rown in result *****/
   for (NumInfo = 0;
	NumInfo < NumInfos;
	NumInfo++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get info type (row[0]) */
      InfoType = Inf_DB_ConvertFromStrDBToInfoType (row[0]);

      Inf_InfoMustBeRead.MustBeRead[InfoType] = Inf_MUST_BE_READ;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Inf_InfoMustBeRead.ShowMsgMustBeRead = NumInfos != 0 ? Lay_SHOW :
							  Lay_DONT_SHOW;
  }

/*****************************************************************************/
/***** Write message if students must read any info about current course *****/
/*****************************************************************************/

void Inf_WriteMsgYouMustReadInfo (void)
  {
   extern const char *Txt_Required_reading;
   extern const char *Txt_You_should_read_the_following_information;
   static struct Act_ActionFunc Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = {ActUnk		,NULL,NULL},
      [Inf_INFORMATION	] = {ActSeeCrsInf	,NULL,NULL},
      [Inf_PROGRAM	] = {ActUnk		,NULL,NULL},	// Not used
      [Inf_TEACH_GUIDE	] = {ActSeeTchGui	,NULL,NULL},
      [Inf_SYLLABUS_LEC	] = {ActSeeSyl		,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_LEC]},
      [Inf_SYLLABUS_PRA	] = {ActSeeSyl		,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_PRA]},
      [Inf_BIBLIOGRAPHY	] = {ActSeeBib		,NULL,NULL},
      [Inf_FAQ		] = {ActSeeFAQ		,NULL,NULL},
      [Inf_LINKS	] = {ActSeeCrsLnk	,NULL,NULL},
      [Inf_ASSESSMENT	] = {ActSeeAss		,NULL,NULL},
     };
   Inf_Type_t InfoType;
   const char *TitleAction;

   if (Inf_InfoMustBeRead.ShowMsgMustBeRead == Lay_SHOW)
     {
      /***** Begin box *****/
      Box_BoxBegin (Txt_Required_reading,NULL,NULL,NULL,Box_CLOSABLE);

	 /***** Write message *****/
	 Ale_ShowAlert (Ale_WARNING,Txt_You_should_read_the_following_information);

	 /***** Write every information I must read *****/
	 HTM_DIV_Begin ("class=\"CM\"");
	    HTM_UL_Begin ("class=\"LIST_I_MUST_READ\"");
	       for (InfoType  = (Inf_Type_t) 0;
		    InfoType <= (Inf_Type_t) (Inf_NUM_TYPES - 1);
		    InfoType++)
		  if (Inf_InfoMustBeRead.MustBeRead[InfoType] == Inf_MUST_BE_READ)
		    {
		     HTM_LI_Begin (NULL);
			Frm_BeginForm (Actions[InfoType].NextAction);
			   if (Actions[InfoType].FuncPars)
			      Actions[InfoType].FuncPars (Actions[InfoType].Args);
			   TitleAction = Act_GetTitleAction (Actions[InfoType].NextAction);
			   HTM_BUTTON_Submit_Begin (TitleAction,NULL,
						    "class=\"BT_LINK FORM_IN_%s\"",
						    The_GetSuffix ());
			      HTM_Txt (TitleAction);
			   HTM_BUTTON_End ();
			Frm_EndForm ();
		     HTM_LI_End ();
		    }
	    HTM_UL_End ();
	 HTM_DIV_End ();

      /***** End box *****/
      Box_BoxEnd ();
     }
  }

/*****************************************************************************/
/****** Change teacher's preference about force students to read info ********/
/*****************************************************************************/

void Inf_ChangeForceReadInfo (void)
  {
   extern const char *Txt_Students_are_no_longer_obliged_to_read_this_information;
   extern const char *Txt_Students_now_are_required_to_read_this_information;
   static const char **AlertTxt[Inf_NUM_MUST_BE_READ] =
     {
      [Inf_DONT_MUST_BE_READ] = &Txt_Students_are_no_longer_obliged_to_read_this_information,
      [Inf_MUST_BE_READ     ] = &Txt_Students_now_are_required_to_read_this_information,
     };
   Inf_Type_t InfoType;
   Inf_MustBeRead_t MustBeRead;

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();

   /***** Set whether info must be read by students *****/
   MustBeRead = Inf_GetMustBeReadFromForm ();

   /***** Set status (if info must be read or not) into database *****/
   Inf_DB_SetForceRead (InfoType,MustBeRead);

   /***** Write message of success *****/
   Ale_ShowAlert (Ale_SUCCESS,*AlertTxt[MustBeRead]);

   /***** Show the selected info *****/
   Inf_ShowInfo ();
  }

/*****************************************************************************/
/************** Change confirmation of I have read course info ***************/
/*****************************************************************************/

void Inf_ChangeIHaveReadInfo (void)
  {
   extern const char *Txt_You_have_eliminated_the_confirmation_that_you_have_read_this_information;
   extern const char *Txt_You_have_confirmed_that_you_have_read_this_information;
   static const char **AlertTxt[Inf_NUM_I_HAVE_READ] =
     {
      [Inf_I_DONT_HAVE_READ] = &Txt_You_have_eliminated_the_confirmation_that_you_have_read_this_information,
      [Inf_I_HAVE_READ     ] = &Txt_You_have_confirmed_that_you_have_read_this_information,
     };
   Inf_Type_t InfoType;
   Inf_IHaveRead_t IHaveRead;

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();

   /***** Set whether I have read information *****/
   IHaveRead = Inf_GetIfIHaveReadFromForm ();

   /***** Set status (if I have read or not a information) into database *****/
   Inf_DB_SetIHaveRead (InfoType,IHaveRead);

   /***** Write message of success *****/
   Ale_ShowAlert (Ale_SUCCESS,*AlertTxt[IHaveRead]);

   /***** Show the selected info *****/
   Inf_ShowInfo ();
  }

/*****************************************************************************/
/************* Get if info must be read by students from form ****************/
/*****************************************************************************/

static Inf_MustBeRead_t Inf_GetMustBeReadFromForm (void)
  {
   return Par_GetParBool ("MustBeRead") ? Inf_MUST_BE_READ :
					  Inf_DONT_MUST_BE_READ;
  }

/*****************************************************************************/
/************* Get if info must be read by students from form ****************/
/*****************************************************************************/

static Inf_IHaveRead_t Inf_GetIfIHaveReadFromForm (void)
  {
   return Par_GetParBool ("IHaveRead") ? Inf_I_HAVE_READ :
					 Inf_I_DONT_HAVE_READ;
  }

/*****************************************************************************/
/************************** Configure info source ****************************/
/*****************************************************************************/

static void Inf_ConfigInfoSource (struct Inf_Info *Info)
  {
   extern const char *Txt_Source_of_information;
   extern const char *Txt_INFO_SRC_FULL_TEXT[Inf_NUM_SOURCES];
   extern const char *Txt_INFO_SRC_HELP[Inf_NUM_SOURCES];
   Inf_Src_t InfoSrc;
   Exi_Exist_t InfoAvailable[Inf_NUM_SOURCES];
   static Act_Action_t Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActSelInfSrcCrsInf,
      [Inf_PROGRAM	] = ActUnk,	// Not used
      [Inf_TEACH_GUIDE	] = ActSelInfSrcTchGui,
      [Inf_SYLLABUS_LEC	] = ActSelInfSrcSyl,
      [Inf_SYLLABUS_PRA	] = ActSelInfSrcSyl,
      [Inf_BIBLIOGRAPHY	] = ActSelInfSrcBib,
      [Inf_FAQ		] = ActSelInfSrcFAQ,
      [Inf_LINKS	] = ActSelInfSrcCrsLnk,
      [Inf_ASSESSMENT	] = ActSelInfSrcAss,
     };
   /* Functions to write forms in course edition (FAQ, links, etc.) */
   static void (*Inf_FormsForEditionTypes[Inf_NUM_SOURCES])(Inf_Type_t InfoType,Inf_Src_t InfoSrc) =
     {
      [Inf_SRC_NONE	] = NULL,
      [Inf_EDITOR	] = Inf_FormToEnterEditor,
      [Inf_PLAIN_TEXT	] = Inf_FormToEnterEditor,
      [Inf_RICH_TEXT	] = Inf_FormToEnterEditor,
      [Inf_PAGE		] = Inf_FormToEnterPageUploader,
      [Inf_URL		] = Inf_FormToEnterEditor,
     };

   /***** Check if info available *****/
   for (InfoSrc  = (Inf_Src_t) 0;
	InfoSrc <= (Inf_Src_t) (Inf_NUM_SOURCES - 1);
	InfoSrc++)
      InfoAvailable[InfoSrc] = Inf_CheckIfInfoAvailable (Info->Type,InfoSrc);

   /***** Set info source to none
	  when no info available for the current source *****/
   if (Info->FromDB.Src != Inf_SRC_NONE &&
       InfoAvailable[Info->FromDB.Src] == Exi_DOES_NOT_EXIST)
     {
      Info->FromDB.Src = Inf_SRC_NONE;
      Inf_DB_SetInfoSrc (Info->Type,Inf_SRC_NONE);
     }

   HTM_FIELDSET_Begin (NULL);
      HTM_LEGEND (Txt_Source_of_information);
      HTM_TABLE_BeginPadding (2);

	 /* Options */
	 for (InfoSrc  = (Inf_Src_t) 0;
	      InfoSrc <= (Inf_Src_t) (Inf_NUM_SOURCES - 1);
	      InfoSrc++)
	   {
	    HTM_TR_Begin (NULL);

	       /* Select info source */
	       HTM_TD_Begin ("class=\"LT DAT_%s%s\"",The_GetSuffix (),
			     InfoSrc == Info->FromDB.Src ? " BG_HIGHLIGHT" :
							   "");
		  Frm_BeginForm (Actions[Info->Type]);
		     Inf_PutParInfoType (&Info->Type);
		     HTM_INPUT_RADIO ("InfoSrc",
				      (InfoSrc == Info->FromDB.Src ? HTM_CHECKED :
								     HTM_NO_ATTR) |
				      (InfoSrc == Inf_SRC_NONE ||
				       InfoAvailable[InfoSrc] == Exi_EXISTS ? HTM_NO_ATTR :
									      HTM_DISABLED) |
				      (InfoSrc != Info->FromDB.Src &&
				       (InfoSrc == Inf_SRC_NONE ||
				        InfoAvailable[InfoSrc] == Exi_EXISTS) ? HTM_SUBMIT_ON_CLICK :
									        HTM_NO_ATTR),
				      "id=\"InfoSrc%u\" value=\"%u\"",
				      (unsigned) InfoSrc,(unsigned) InfoSrc);
		  Frm_EndForm ();
	       HTM_TD_End ();

	       /* Form for this info source */
	       HTM_TD_Begin ("class=\"LT%s\"",
			     InfoSrc == Info->FromDB.Src ? " BG_HIGHLIGHT" :
							   "");
		  HTM_LABEL_Begin ("for=\"InfoSrc%u\" class=\"FORM_IN_%s\"",
				   (unsigned) InfoSrc,The_GetSuffix ());
		     HTM_Txt (Txt_INFO_SRC_FULL_TEXT[InfoSrc]);
		  HTM_LABEL_End ();
		  if (Txt_INFO_SRC_HELP[InfoSrc])
		    {
		     HTM_SPAN_Begin ("class=\"DAT_%s\"",The_GetSuffix ());
			HTM_BR ();
			HTM_Txt (Txt_INFO_SRC_HELP[InfoSrc]);
		     HTM_SPAN_End ();
		    }
		  if (Inf_FormsForEditionTypes[InfoSrc])
		     Inf_FormsForEditionTypes[InfoSrc] (Info->Type,InfoSrc);
	       HTM_TD_End ();

	    HTM_TR_End ();
	   }

     HTM_TABLE_End ();
   HTM_FIELDSET_End ();
  }

/*****************************************************************************/
/* Check if there is info available for current info type and a given source */
/*****************************************************************************/

static Exi_Exist_t Inf_CheckIfInfoAvailable (Inf_Type_t InfoType,Inf_Src_t InfoSrc)
  {
   switch (InfoSrc)
     {
      case Inf_SRC_NONE:
	 return Exi_DOES_NOT_EXIST;
      case Inf_EDITOR:
         return Tre_DB_GetNumNodes (InfoType,Hie_CRS) != 0 ? Exi_EXISTS :
							     Exi_DOES_NOT_EXIST;
      case Inf_PLAIN_TEXT:
         return Inf_CheckPlainTxt (InfoType);
      case Inf_RICH_TEXT:
         return Inf_CheckRichTxt (InfoType);
      case Inf_PAGE:
	 return Inf_CheckPage (InfoType);
      case Inf_URL:
	 return Inf_CheckURL (InfoType);
     }

   return Exi_DOES_NOT_EXIST;	// Not reached
  }

/*****************************************************************************/
/****************** Form to enter in plain text editor ***********************/
/*****************************************************************************/

static void Inf_FormToEnterEditor (Inf_Type_t InfoType,Inf_Src_t InfoSrc)
  {
   Frm_BeginForm (Inf_ActionsInfo[InfoSrc][InfoType]);
      Inf_PutParInfoType (&InfoType);
      Btn_PutButtonInline (Btn_EDIT);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************* Form to upload a file with a page ***********************/
/*****************************************************************************/

static void Inf_FormToEnterPageUploader (Inf_Type_t InfoType,Inf_Src_t InfoSrc)
  {
   Frm_BeginForm (Inf_ActionsInfo[InfoSrc][InfoType]);
      Inf_PutParInfoType (&InfoType);
      Btn_PutButtonInline (Btn_UPLOAD);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******** Returns bibliography, assessment, etc. from Gbl.Action.Act *********/
/*****************************************************************************/

Inf_Type_t Inf_AsignInfoType (void)
  {
   switch (Act_GetSuperAction (Gbl.Action.Act))
     {
      case ActSeeCrsInf:
         return Inf_INFORMATION;
      case ActSeePrg:
         return Inf_PROGRAM;
      case ActSeeTchGui:
         return Inf_TEACH_GUIDE;
      case ActSeeSyl:
	 return Inf_GetParInfoType ();
      case ActSeeBib:
         return Inf_BIBLIOGRAPHY;
      case ActSeeFAQ:
         return Inf_FAQ;
      case ActSeeCrsLnk:
         return Inf_LINKS;
      case ActSeeAss:
         return Inf_ASSESSMENT;
         break;
      default:
	 Err_WrongActionExit ();
	 return Inf_UNKNOWN_TYPE;	// Not reached
     }
  }

/*****************************************************************************/
/************ Get parameter about which information I want to see ************/
/*****************************************************************************/

Inf_Type_t Inf_GetParInfoType (void)
  {
   static Inf_Type_t InfoTypeCached = Inf_UNKNOWN_TYPE;

   /***** If already got ==> don't search parameter again *****/
   if (InfoTypeCached != Inf_UNKNOWN_TYPE)
      return InfoTypeCached;

   /***** If not yet got ==> search parameter *****/
   return InfoTypeCached = (Inf_Type_t)
	  Par_GetParUnsignedLong ("WhichSyllabus",
				  (unsigned long) Inf_SYLLABUS_LEC,
				  (unsigned long) Inf_SYLLABUS_PRA,
				  (unsigned long) Inf_DEFAULT_WHICH_SYLLABUS);
  }

/*****************************************************************************/
/****************** Put parameter with type of syllabus **********************/
/*****************************************************************************/

void Inf_PutParInfoType (void *InfoType)
  {
   if (InfoType)
      switch (*((Inf_Type_t *) InfoType))
	{
	 case Inf_SYLLABUS_LEC:
	 case Inf_SYLLABUS_PRA:
	    Par_PutParUnsigned (NULL,"WhichSyllabus",*((Inf_Type_t *) InfoType));
	    break;
	 default:
	    break;
	}
  }

/*****************************************************************************/
/********** Get info source for bibliography, FAQ, etc. from form ************/
/*****************************************************************************/

Inf_Src_t Inf_GetInfoSrcFromForm (void)
  {
   /***** Get info source for a specific type of course information
          (introduction, teaching guide, bibliography, FAQ, links or assessment) *****/
   return (Inf_Src_t)
	  Par_GetParUnsignedLong ("InfoSrc",
                                  0,
                                  Inf_NUM_SOURCES - 1,
                                  (unsigned long) Inf_SRC_NONE);
  }

/*****************************************************************************/
/***** Get and check info source for a type of course info from database *****/
/*****************************************************************************/

Inf_Src_t Inf_GetInfoSrcFromDB (long HieCod,Inf_Type_t InfoType)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Inf_Src_t InfoSrc;

   /***** Get info source for a specific type of info from database *****/
   switch (Inf_DB_GetInfoSrc (&mysql_res,HieCod,InfoType))
     {
      case Exi_EXISTS:
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get info source (row[0]) */
	 InfoSrc = Inf_DB_ConvertFromStrDBToInfoSrc (row[0]);
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 InfoSrc = Inf_SRC_NONE;
	 break;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return InfoSrc;
  }

/*****************************************************************************/
/***** Get and check info source for a type of course info from database *****/
/*****************************************************************************/

void Inf_GetAndCheckInfoSrcFromDB (struct Inf_Info *Info)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Set default values *****/
   Info->FromDB.Src        = Inf_SRC_NONE;
   Info->FromDB.MustBeRead = Inf_DONT_MUST_BE_READ;

   /***** Get info source for a specific type of info from database *****/
   if (Inf_DB_GetInfoSrcAndMustBeRead (&mysql_res,
				       Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				       Info->Type) == Exi_EXISTS)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get info source (row[0]) and if students must read info (row[1]) */
      Info->FromDB.Src = Inf_DB_ConvertFromStrDBToInfoSrc (row[0]);
      Info->FromDB.MustBeRead = row[1][0] == 'Y' ? Inf_MUST_BE_READ :
						   Inf_DONT_MUST_BE_READ;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Get info text for a type of course info from database ************/
/*****************************************************************************/

void Inf_GetInfoTxtFromDB (long HieCod,Inf_Type_t InfoType,
                           char InfoTxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1],
                           char InfoTxtMD  [Cns_MAX_BYTES_LONG_TEXT + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get info source for a specific type of course information
          (bibliography, FAQ, links or assessment) from database *****/
   switch (Inf_DB_GetInfoTxt (&mysql_res,HieCod,InfoType))
     {
      case Exi_EXISTS:
	 /* Get info text */
	 row = mysql_fetch_row (mysql_res);

	 /* Get text in HTML format (not rigorous) */
	 if (InfoTxtHTML)
	    Str_Copy (InfoTxtHTML,row[0],Cns_MAX_BYTES_LONG_TEXT);

	 /* Get text in Markdown format */
	 if (InfoTxtMD)
	    Str_Copy (InfoTxtMD  ,row[1],Cns_MAX_BYTES_LONG_TEXT);
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 if (InfoTxtHTML)
	    InfoTxtHTML[0] = '\0';
	 if (InfoTxtMD)
	    InfoTxtMD  [0] = '\0';
	 break;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Check information about the course ********************/
/*****************************************************************************/
// Return Exi_EXISTS if info available

static Exi_Exist_t Inf_CheckPlainTxt (Inf_Type_t InfoType)
  {
   char TxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Get info text from database *****/
   Inf_GetInfoTxtFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,TxtHTML,NULL);

   return TxtHTML[0] != '\0' ? Exi_EXISTS :
			       Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/********************* Show information about the course *********************/
/*****************************************************************************/
// Return Exi_EXISTS if info available

static Exi_Exist_t Inf_CheckAndShowPlainTxt (Inf_Type_t InfoType)
  {
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   char TxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Get info text from database *****/
   Inf_GetInfoTxtFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,
                         TxtHTML,NULL);

   if (TxtHTML[0])
     {
      switch (InfoType)
	{
	 case Inf_INFORMATION:
	 case Inf_TEACH_GUIDE:
	    Lay_WriteHeaderClassPhoto (Hie_CRS,Vie_VIEW);
	    break;
	 default:
	    break;
	}

      HTM_DIV_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());

	 /***** Convert to respectful HTML and insert links *****/
	 Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			   TxtHTML,Cns_MAX_BYTES_LONG_TEXT,
			   Str_DONT_REMOVE_SPACES);
	 ALn_InsertLinks (TxtHTML,Cns_MAX_BYTES_LONG_TEXT,60);	// Insert links

	 /***** Write text *****/
	 HTM_Txt (TxtHTML);

      HTM_DIV_End ();

      return Exi_EXISTS;
     }

   return Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/********************* Show information about the course *********************/
/*****************************************************************************/
// Return Exi_EXISTS if info available

static Exi_Exist_t Inf_CheckRichTxt (Inf_Type_t InfoType)
  {
   char TxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1];
   char TxtMD[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Get info text from database *****/
   Inf_GetInfoTxtFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,
                         TxtHTML,TxtMD);

   return TxtMD[0] != '\0' ? Exi_EXISTS :
			     Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/********************* Show information about the course *********************/
/*****************************************************************************/
// Return Exi_EXISTS if info available

static Exi_Exist_t Inf_CheckAndShowRichTxt (Inf_Type_t InfoType)
  {
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   char TxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1];
   char TxtMD[Cns_MAX_BYTES_LONG_TEXT + 1];
   char PathFileMD[PATH_MAX + 1];
   char PathFileHTML[PATH_MAX + 1];
   const char *UniqueNameEncrypted;
   FILE *FileMD;		// Temporary Markdown file
   FILE *FileHTML;		// Temporary HTML file
   char MathJaxURL[PATH_MAX + 1];
   char Command[512 + PATH_MAX * 3]; // Command to call the program of preprocessing of photos
   int ReturnCode;

   /***** Get info text from database *****/
   Inf_GetInfoTxtFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,
                         TxtHTML,TxtMD);

   if (TxtMD[0])
     {
      switch (InfoType)
	{
	 case Inf_INFORMATION:
	 case Inf_TEACH_GUIDE:
	    Lay_WriteHeaderClassPhoto (Hie_CRS,Vie_VIEW);
	    break;
	 default:
	    break;
	}

      HTM_DIV_Begin ("id=\"crs_info\" class=\"LM CRS_INFO_%s\"",The_GetSuffix ());

	 /***** Store text into a temporary .md file in HTML output directory *****/
	 // TODO: change to another directory?
	 /* Create a unique name for the .md file */
	 UniqueNameEncrypted = Cry_GetUniqueNameEncrypted ();
	 snprintf (PathFileMD,sizeof (PathFileMD),"%s/%s.md",
		   Cfg_PATH_OUT_PRIVATE,UniqueNameEncrypted);
	 snprintf (PathFileHTML,sizeof (PathFileHTML),"%s/%s.md.html",	// Do not use only .html because that is the output temporary file
		   Cfg_PATH_OUT_PRIVATE,UniqueNameEncrypted);

	 /* Open Markdown file for writing */
	 if ((FileMD = fopen (PathFileMD,"wb")) == NULL)
	    Err_ShowErrorAndExit ("Can not create temporary Markdown file.");

	 /* Write text into Markdown file */
	 fprintf (FileMD,"%s",TxtMD);

	 /* Close Markdown file */
	 fclose (FileMD);

	 /***** Convert from Markdown to HTML *****/
	 /* MathJax 2.5.1
#ifdef Cfg_MATHJAX_LOCAL
	 // Use the local copy of MathJax
	 snprintf (MathJaxURL,sizeof (MathJaxURL),
		   "=%s/MathJax/MathJax.js?config=TeX-AMS-MML_HTMLorMML",
		   Cfg_URL_SWAD_PUBLIC);
#else
	 // Use the MathJax Content Delivery Network (CDN)
	 MathJaxURL[0] = '\0';
#endif
	 */
	 /* MathJax 3.0.1
	 // https://pandoc.org/MANUAL.html#math-rendering-in-html
	 // https://docs.mathjax.org/en/latest/input/tex/
	 //
#ifdef Cfg_MATHJAX_LOCAL
	 // Use the local copy of MathJax
	 snprintf (MathJaxURL,sizeof (MathJaxURL),"=%s/mathjax/tex-chtml.js",
		   Cfg_URL_SWAD_PUBLIC);
#else
	 // Use the MathJax Content Delivery Network (CDN)
	 MathJaxURL[0] = '\0';
#endif
	 // --ascii uses only ascii characters in output
	 //         (uses numerical entities instead of UTF-8)
	 //         is mandatory in order to convert (with iconv) the UTF-8 output of pandoc to WINDOWS-1252
	 snprintf (Command,sizeof (Command),
		   "iconv -f WINDOWS-1252 -t UTF-8 %s"
		   " | "
		   "pandoc --ascii --mathjax%s -f markdown_github+tex_math_dollars -t html5"
		   " | "
		   "iconv -f UTF-8 -t WINDOWS-1252 -o %s",
		   PathFileMD,
		   MathJaxURL,
		   PathFileHTML);
	 */
	 /* MathJax 4.1.0 */
	 // https://pandoc.org/MANUAL.html#math-rendering-in-html
	 // https://docs.mathjax.org/en/latest/input/tex/
	 //
#ifdef Cfg_MATHJAX_LOCAL
	 // Use the local copy of MathJax
	 snprintf (MathJaxURL,sizeof (MathJaxURL),"=%s/mathjax/tex-chtml.js",
		   Cfg_URL_SWAD_PUBLIC);
#else
	 // Use the MathJax Content Delivery Network (CDN)
	 MathJaxURL[0] = '\0';
#endif
	 // --ascii uses only ascii characters in output
	 //         (uses numerical entities instead of UTF-8)
	 //         is mandatory in order to convert (with iconv) the UTF-8 output of pandoc to WINDOWS-1252
	 snprintf (Command,sizeof (Command),
		   "iconv -f WINDOWS-1252 -t UTF-8 %s"
		   " | "
		   // "pandoc --ascii --mathjax%s -f markdown+tex_math_dollars+tex_math_single_backslash -t html5"
		   "pandoc --ascii --mathjax%s -f markdown+tex_math_single_backslash -t html5"
		   " | "
		   "iconv -f UTF-8 -t WINDOWS-1252 -o %s",
		   PathFileMD,
		   MathJaxURL,
		   PathFileHTML);

	 ReturnCode = system (Command);
	 if (ReturnCode == -1)
	    Err_ShowErrorAndExit ("Error when running command to convert from Markdown to HTML.");

	 /***** Remove Markdown file *****/
	 unlink (PathFileMD);

	 /***** Copy HTML file just created to HTML output *****/
	 /* Open temporary HTML file for reading */
	 if ((FileHTML = fopen (PathFileHTML,"rb")) == NULL)
	    Err_ShowErrorAndExit ("Can not open temporary HTML file.");

	 /* Copy from temporary HTML file to output file */
	 Fil_FastCopyOfOpenFiles (FileHTML,Fil_GetOutputFile ());

	 /* Close and remove temporary HTML file */
	 fclose (FileHTML);
	 unlink (PathFileHTML);

      HTM_DIV_End ();

      return Exi_EXISTS;
     }

   return Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/************** Edit plain text information about the course *****************/
/*****************************************************************************/

void Inf_EditPlainTxtInfo (void)
  {
   extern const char *Hlp_COURSE_Information_edit;
   extern const char *Hlp_COURSE_Program_edit;
   extern const char *Hlp_COURSE_Guide_edit;
   extern const char *Hlp_COURSE_Syllabus_edit;
   extern const char *Hlp_COURSE_Bibliography_edit;
   extern const char *Hlp_COURSE_FAQ_edit;
   extern const char *Hlp_COURSE_Links_edit;
   extern const char *Hlp_COURSE_Assessment_edit;
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   static struct Act_ActionFunc Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = {ActUnk		,NULL,NULL},
      [Inf_INFORMATION	] = {ActRcvPlaTxtCrsInf	,NULL,NULL},
      [Inf_PROGRAM	] = {ActUnk		,NULL,NULL},	// Not used
      [Inf_TEACH_GUIDE	] = {ActRcvPlaTxtTchGui	,NULL,NULL},
      [Inf_SYLLABUS_LEC	] = {ActRcvPlaTxtSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_LEC]},
      [Inf_SYLLABUS_PRA	] = {ActRcvPlaTxtSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_PRA]},
      [Inf_BIBLIOGRAPHY	] = {ActRcvPlaTxtBib	,NULL,NULL},
      [Inf_FAQ		] = {ActRcvPlaTxtFAQ	,NULL,NULL},
      [Inf_LINKS	] = {ActRcvPlaTxtCrsLnk	,NULL,NULL},
      [Inf_ASSESSMENT	] = {ActRcvPlaTxtAss	,NULL,NULL},
     };
   static const char **HelpEdit[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = &Hlp_COURSE_Information_edit,
      [Inf_PROGRAM	] = &Hlp_COURSE_Program_edit,	// Not used
      [Inf_TEACH_GUIDE	] = &Hlp_COURSE_Guide_edit,
      [Inf_SYLLABUS_LEC	] = &Hlp_COURSE_Syllabus_edit,
      [Inf_SYLLABUS_PRA	] = &Hlp_COURSE_Syllabus_edit,
      [Inf_BIBLIOGRAPHY	] = &Hlp_COURSE_Bibliography_edit,
      [Inf_FAQ		] = &Hlp_COURSE_FAQ_edit,
      [Inf_LINKS	] = &Hlp_COURSE_Links_edit,
      [Inf_ASSESSMENT	] = &Hlp_COURSE_Assessment_edit,
     };
   Inf_Type_t InfoType;
   char TxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();

   /***** Begin form and box *****/
   Frm_BeginForm (Actions[InfoType].NextAction);
      if (Actions[InfoType].FuncPars)
	 Actions[InfoType].FuncPars (Actions[InfoType].Args);
      Box_BoxBegin (Txt_INFO_TITLE[InfoType],NULL,NULL,
		    *HelpEdit[InfoType],Box_NOT_CLOSABLE);

	 switch (InfoType)
	   {
	    case Inf_INFORMATION:
	    case Inf_TEACH_GUIDE:
	       Lay_WriteHeaderClassPhoto (Hie_CRS,Vie_VIEW);
	       break;
	    default:
	       break;
	   }

	 /***** Get info text from database *****/
	 Inf_GetInfoTxtFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,
			       TxtHTML,NULL);

	 /***** Edition area *****/
	 HTM_DIV_Begin ("class=\"CM\"");
	    Lay_HelpPlainEditor ();
	    HTM_TEXTAREA_Begin (HTM_NO_ATTR,
				"name=\"Txt\" cols=\"80\" rows=\"20\""
		                " class=\"INPUT_%s\"",The_GetSuffix ());
	       HTM_Txt (TxtHTML);
	    HTM_TEXTAREA_End ();
	 HTM_DIV_End ();

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_SAVE_CHANGES);
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************** Edit rich text information about the course *****************/
/*****************************************************************************/

void Inf_EditRichTxtInfo (void)
  {
   extern const char *Hlp_COURSE_Information_edit;
   extern const char *Hlp_COURSE_Program_edit;
   extern const char *Hlp_COURSE_Guide_edit;
   extern const char *Hlp_COURSE_Syllabus_edit;
   extern const char *Hlp_COURSE_Bibliography_edit;
   extern const char *Hlp_COURSE_FAQ_edit;
   extern const char *Hlp_COURSE_Links_edit;
   extern const char *Hlp_COURSE_Assessment_edit;
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   static struct Act_ActionFunc Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = {ActUnk		,NULL,NULL},
      [Inf_INFORMATION	] = {ActRcvRchTxtCrsInf	,NULL,NULL},
      [Inf_PROGRAM	] = {ActUnk		,NULL,NULL},	// Not used
      [Inf_TEACH_GUIDE	] = {ActRcvRchTxtTchGui	,NULL,NULL},
      [Inf_SYLLABUS_LEC	] = {ActRcvRchTxtSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_LEC]},
      [Inf_SYLLABUS_PRA	] = {ActRcvRchTxtSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_PRA]},
      [Inf_BIBLIOGRAPHY	] = {ActRcvRchTxtBib	,NULL,NULL},
      [Inf_FAQ		] = {ActRcvRchTxtFAQ	,NULL,NULL},
      [Inf_LINKS	] = {ActRcvRchTxtCrsLnk	,NULL,NULL},
      [Inf_ASSESSMENT	] = {ActRcvRchTxtAss	,NULL,NULL},
     };
   static const char **HelpEdit[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = &Hlp_COURSE_Information_edit,
      [Inf_PROGRAM	] = &Hlp_COURSE_Program_edit,	// Not used
      [Inf_TEACH_GUIDE	] = &Hlp_COURSE_Guide_edit,
      [Inf_SYLLABUS_LEC	] = &Hlp_COURSE_Syllabus_edit,
      [Inf_SYLLABUS_PRA	] = &Hlp_COURSE_Syllabus_edit,
      [Inf_BIBLIOGRAPHY	] = &Hlp_COURSE_Bibliography_edit,
      [Inf_FAQ		] = &Hlp_COURSE_FAQ_edit,
      [Inf_LINKS	] = &Hlp_COURSE_Links_edit,
      [Inf_ASSESSMENT	] = &Hlp_COURSE_Assessment_edit,
     };
   Inf_Type_t InfoType;
   char TxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();

   /***** Begin form and box *****/
   Frm_BeginForm (Actions[InfoType].NextAction);
      if (Actions[InfoType].FuncPars)
	 Actions[InfoType].FuncPars (Actions[InfoType].Args);
      Box_BoxBegin (Txt_INFO_TITLE[InfoType],NULL,NULL,
		    *HelpEdit[InfoType],Box_NOT_CLOSABLE);

      switch (InfoType)
	{
	 case Inf_INFORMATION:
	 case Inf_TEACH_GUIDE:
	    Lay_WriteHeaderClassPhoto (Hie_CRS,Vie_VIEW);
	    break;
	 default:
	    break;
	}

      /***** Get info text from database *****/
      Inf_GetInfoTxtFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,
			    TxtHTML,NULL);

      /***** Edition area *****/
      HTM_DIV_Begin ("class=\"CM\"");
	 Lay_HelpRichEditor ();
	 HTM_TEXTAREA_Begin (HTM_NO_ATTR,
			     "name=\"Txt\" cols=\"80\" rows=\"20\""
		             " class=\"INPUT_%s\"",The_GetSuffix ());
	    HTM_Txt (TxtHTML);
	 HTM_TEXTAREA_End ();
      HTM_DIV_End ();

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_SAVE_CHANGES);
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************** Receive and change plain text of course info ****************/
/*****************************************************************************/

void Inf_ReceivePlainTxtInfo (void)
  {
   Inf_Type_t InfoType;
   char Txt_HTMLFormat    [Cns_MAX_BYTES_LONG_TEXT + 1];
   char Txt_MarkdownFormat[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();

   /***** Get text with course information from form *****/
   Par_GetPar (Par_PARAM_SINGLE,"Txt",Txt_HTMLFormat,
	       Cns_MAX_BYTES_LONG_TEXT,NULL);
   Str_Copy (Txt_MarkdownFormat,Txt_HTMLFormat,sizeof (Txt_MarkdownFormat) - 1);
   Str_ChangeFormat (Str_FROM_FORM,Str_TO_HTML,
                     Txt_HTMLFormat,Cns_MAX_BYTES_LONG_TEXT,Str_REMOVE_SPACES);
   Str_ChangeFormat (Str_FROM_FORM,Str_TO_MARKDOWN,
                     Txt_MarkdownFormat,Cns_MAX_BYTES_LONG_TEXT,Str_REMOVE_SPACES);	// Store a copy in Markdown format

   /***** Update text of course info in database *****/
   Inf_DB_SetInfoTxt (InfoType,Txt_HTMLFormat,Txt_MarkdownFormat);

   /***** Change info source to "plain text" in database *****/
   Inf_DB_SetInfoSrc (InfoType,Txt_HTMLFormat[0] ? Inf_PLAIN_TEXT :
						   Inf_SRC_NONE);
   if (Txt_HTMLFormat[0])
      /***** Show the updated info *****/
      Inf_ShowInfo ();
   else
      /***** Show again the form to select and send course info *****/
      Inf_ConfigInfo ();
  }

/*****************************************************************************/
/*************** Receive and change rich text of course info *****************/
/*****************************************************************************/

void Inf_ReceiveRichTxtInfo (void)
  {
   Inf_Type_t InfoType;
   char Txt_HTMLFormat    [Cns_MAX_BYTES_LONG_TEXT + 1];
   char Txt_MarkdownFormat[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();

   /***** Get text with course information from form *****/
   Par_GetPar (Par_PARAM_SINGLE,"Txt",Txt_HTMLFormat,
               Cns_MAX_BYTES_LONG_TEXT,NULL);
   Str_Copy (Txt_MarkdownFormat,Txt_HTMLFormat,sizeof (Txt_MarkdownFormat) - 1);
   Str_ChangeFormat (Str_FROM_FORM,Str_TO_HTML,
                     Txt_HTMLFormat,Cns_MAX_BYTES_LONG_TEXT,Str_REMOVE_SPACES);
   Str_ChangeFormat (Str_FROM_FORM,Str_TO_MARKDOWN,
                     Txt_MarkdownFormat,Cns_MAX_BYTES_LONG_TEXT,Str_REMOVE_SPACES);	// Store a copy in Markdown format

   /***** Update text of course info in database *****/
   Inf_DB_SetInfoTxt (InfoType,Txt_HTMLFormat,Txt_MarkdownFormat);

   /***** Change info source to "rich text" in database *****/
   Inf_DB_SetInfoSrc (InfoType,Txt_HTMLFormat[0] ? Inf_RICH_TEXT :
						   Inf_SRC_NONE);
   if (Txt_HTMLFormat[0])
      /***** Show the updated info *****/
      Inf_ShowInfo ();
   else
      /***** Show again the form to select and send course info *****/
      Inf_ConfigInfo ();
  }

/*****************************************************************************/
/************** Receive a page of syllabus, bibliography, etc. ***************/
/*****************************************************************************/

void Inf_EditPagInfo (void)
  {
   extern const char *Txt_INFO_SRC_HELP[Inf_NUM_SOURCES];
   extern const char *Txt_File;
   static Act_Action_t Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActRcvPagCrsInf,
      [Inf_PROGRAM	] = ActUnk,	// Not used
      [Inf_TEACH_GUIDE	] = ActRcvPagTchGui,
      [Inf_SYLLABUS_LEC	] = ActRcvPagSyl,
      [Inf_SYLLABUS_PRA	] = ActRcvPagSyl,
      [Inf_BIBLIOGRAPHY	] = ActRcvPagBib,
      [Inf_FAQ		] = ActRcvPagFAQ,
      [Inf_LINKS	] = ActRcvPagCrsLnk,
      [Inf_ASSESSMENT	] = ActRcvPagAss,
     };
   struct Inf_Info Info;

   /***** Begin box *****/
   Inf_BeforeTree (&Info,Vie_EDIT,Inf_PAGE);

      /***** Begin form *****/
      Frm_BeginForm (Actions[Info.Type]);
	 Inf_PutParInfoType (&Info.Type);

	 /***** Help *****/
	 Ale_ShowAlert (Ale_INFO,Txt_INFO_SRC_HELP[Inf_PAGE]);

	 /***** File *****/
	 HTM_DIV_Begin ("class=\"CM\"");
	    HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	       HTM_Txt (Txt_File); HTM_Colon (); HTM_NBSP ();
	       HTM_INPUT_FILE (Fil_NAME_OF_PARAM_FILENAME_ORG,".htm,.html,.pdf,.zip",
			       HTM_SUBMIT_ON_CHANGE,
			       NULL);
	    HTM_LABEL_End ();
	 HTM_DIV_End ();

      /***** End form *****/
      Frm_EndForm ();

   /***** End box *****/
   Inf_AfterTree ();
  }

void Inf_ReceivePagInfo (void)
  {
   extern const char *Txt_The_file_type_is_X_and_should_be_HTML_or_ZIP;
   extern const char *Txt_The_HTML_file_has_been_received_successfully;
   extern const char *Txt_The_ZIP_file_has_been_received_successfully;
   extern const char *Txt_The_ZIP_file_has_been_unzipped_successfully;
   extern const char *Txt_Found_an_index_html_file;
   extern const char *Txt_No_file_index_html_found_within_the_ZIP_file;
   extern const char *Txt_The_file_type_should_be_HTML_or_ZIP;
   Inf_Type_t InfoType;
   struct Par_Param *Par;
   char SourceFileName[PATH_MAX + 1];
   char PathRelDirHTML[PATH_MAX + 1];
   char PathRelFileHTML[PATH_MAX + 1 + 10 + 1];
   char PathRelFileZIP[PATH_MAX + 1 + NAME_MAX + 1];
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   char StrUnzip[128 + PATH_MAX + 1 + NAME_MAX + 1 + PATH_MAX + 1];
   Err_SuccessOrError_t SuccessOrError;
   Err_SuccessOrError_t FileIsOK = Err_ERROR;

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();

   /***** First of all, store in disk the file received *****/
   Par = Fil_StartReceptionOfFile (Fil_NAME_OF_PARAM_FILENAME_ORG,
                                   SourceFileName,MIMEType);

   /***** Check that MIME type is HTML or ZIP *****/
   SuccessOrError = Err_SUCCESS;
   if (strcmp (MIMEType,"text/html"))
      if (strcmp (MIMEType,"text/plain"))
         if (strcmp (MIMEType,"application/x-zip-compressed"))
            if (strcmp (MIMEType,"application/zip"))
               if (strcmp (MIMEType,"application/x-download"))
		  if (strcmp (MIMEType,"application/octet-stream"))
	             if (strcmp (MIMEType,"application/octetstream"))
	                if (strcmp (MIMEType,"application/octet"))
                           SuccessOrError = Err_ERROR;
   switch (SuccessOrError)
     {
      case Err_SUCCESS:
	 /***** Build path of directory containing web page *****/
	 Inf_BuildPathPage (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,
			    PathRelDirHTML);

	 /***** End the reception of the data *****/
	 if (Str_FileIs (SourceFileName,"html") ||
	     Str_FileIs (SourceFileName,"htm" )) // .html or .htm file
	   {
	    Fil_RemoveTree (PathRelDirHTML);
	    Fil_CreateDirIfNotExists (PathRelDirHTML);
	    snprintf (PathRelFileHTML,sizeof (PathRelFileHTML),"%s/index.html",
		      PathRelDirHTML);
	    switch (Fil_EndReceptionOfFile (PathRelFileHTML,Par))
	      {
	       case Err_SUCCESS:
		  Ale_ShowAlert (Ale_SUCCESS,
				 Txt_The_HTML_file_has_been_received_successfully);
		  FileIsOK = Err_SUCCESS;
		  break;
	       case Err_ERROR:
	       default:
		  Ale_ShowAlert (Ale_ERROR,"Error uploading file.");
		  break;
	      }
	   }
	 else if (Str_FileIs (SourceFileName,"zip")) // .zip file
	   {
	    Fil_RemoveTree (PathRelDirHTML);
	    Fil_CreateDirIfNotExists (PathRelDirHTML);
	    snprintf (PathRelFileZIP,sizeof (PathRelFileZIP),"%s/%s.zip",
		      Gbl.Crs.Path.AbsPriv,
		      Inf_FileNamesForInfoType[InfoType]);

	    switch (Fil_EndReceptionOfFile (PathRelFileZIP,Par))
	      {
	       case Err_SUCCESS:
		  Ale_ShowAlert (Ale_SUCCESS,
				 Txt_The_ZIP_file_has_been_received_successfully);

		  /* Uncompress ZIP */
		  snprintf (StrUnzip,sizeof (StrUnzip),"unzip -qq -o %s -d %s",
			    PathRelFileZIP,PathRelDirHTML);
		  if (system (StrUnzip) == 0)
		    {
		     /* Check if uploaded file is index.html or index.htm */
		     snprintf (PathRelFileHTML,sizeof (PathRelFileHTML),
			       "%s/index.html",PathRelDirHTML);
		     switch (Fil_CheckIfPathExists (PathRelFileHTML))
		       {
			case Exi_EXISTS:
			   Ale_ShowAlert (Ale_SUCCESS,
					  Txt_The_ZIP_file_has_been_unzipped_successfully);
			   Ale_ShowAlert (Ale_SUCCESS,Txt_Found_an_index_html_file);
			   FileIsOK = Err_SUCCESS;
			   break;
			case Exi_DOES_NOT_EXIST:
			default:
			   snprintf (PathRelFileHTML,sizeof (PathRelFileHTML),
				     "%s/index.htm",PathRelDirHTML);
			   switch (Fil_CheckIfPathExists (PathRelFileHTML))
			     {
			      case Exi_EXISTS:
				 Ale_ShowAlert (Ale_SUCCESS,
						Txt_The_ZIP_file_has_been_unzipped_successfully);
				 Ale_ShowAlert (Ale_SUCCESS,
						Txt_Found_an_index_html_file);
				 FileIsOK = Err_SUCCESS;
				 break;
			      case Exi_DOES_NOT_EXIST:
			      default:
				 Ale_ShowAlert (Ale_WARNING,
						Txt_No_file_index_html_found_within_the_ZIP_file);
				 break;
			     }
			   break;
		       }
		    }
		  else
		     Err_ShowErrorAndExit ("Can not unzip file.");
		  break;
	       case Err_ERROR:
	       default:
		  Ale_ShowAlert (Ale_ERROR,"Error uploading file.");
		  break;
	      }
	   }
	 else
	    Ale_ShowAlert (Ale_WARNING,Txt_The_file_type_should_be_HTML_or_ZIP);
         break;
      case Err_ERROR:
      default:
	 Ale_ShowAlert (Ale_INFO,Txt_The_file_type_is_X_and_should_be_HTML_or_ZIP,
			MIMEType);
	 break;
     }

   switch (FileIsOK)
     {
      case Err_SUCCESS:
	 /***** Change info source to page in database *****/
	 Inf_DB_SetInfoSrc (InfoType,Inf_PAGE);

	 /***** Show the updated info *****/
	 Inf_ShowInfo ();
	 break;
      case Err_ERROR:
      default:
	 /***** Change info source to none in database *****/
	 Inf_DB_SetInfoSrc (InfoType,Inf_SRC_NONE);

	 /***** Show again the form to select and send course info *****/
	 Inf_ConfigInfo ();
	 break;
     }
  }

/*****************************************************************************/
/********* Receive a link to a page of syllabus, bibliography, etc. **********/
/*****************************************************************************/

void Inf_EditURLInfo (void)
  {
   extern const char *Txt_URL;
   char URL[WWW_MAX_BYTES_WWW + 1];
   char PathFile[PATH_MAX + 1];
   FILE *FileURL;
   static Act_Action_t Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActRcvURLCrsInf,
      [Inf_PROGRAM	] = ActUnk,	// Not used
      [Inf_TEACH_GUIDE	] = ActRcvURLTchGui,
      [Inf_SYLLABUS_LEC	] = ActRcvURLSyl,
      [Inf_SYLLABUS_PRA	] = ActRcvURLSyl,
      [Inf_BIBLIOGRAPHY	] = ActRcvURLBib,
      [Inf_FAQ		] = ActRcvURLFAQ,
      [Inf_LINKS	] = ActRcvURLCrsLnk,
      [Inf_ASSESSMENT	] = ActRcvURLAss,
     };
   struct Inf_Info Info;

   /***** Begin box *****/
   Inf_BeforeTree (&Info,Vie_EDIT,Inf_URL);

      /***** Build path to file containing URL *****/
      Inf_BuildPathURL (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Info.Type,
			PathFile);

      /***** Begin form *****/
      Frm_BeginForm (Actions[Info.Type]);
	 Inf_PutParInfoType (&Info.Type);

	 /***** Link *****/
	 if ((FileURL = fopen (PathFile,"rb")) != NULL)
	   {
	    if (fgets (URL,WWW_MAX_BYTES_WWW,FileURL) == NULL)
	       URL[0] = '\0';
	    /* File is not longer needed. Close it */
	    fclose (FileURL);
	   }
	 else
	    URL[0] = '\0';

	 HTM_DIV_Begin ("class=\"CM\"");
	    HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	       HTM_Txt (Txt_URL); HTM_Colon (); HTM_NBSP ();
	       HTM_INPUT_URL ("InfoSrcURL",URL,
			      HTM_NO_ATTR,	// TODO: Required?
			      "size=\"50\" class=\"INPUT_%s\"",The_GetSuffix ());
	    HTM_LABEL_End ();
	 HTM_DIV_End ();

	 /***** Send button *****/
	 Btn_PutButton (Btn_SAVE_CHANGES,NULL);

      /***** End form *****/
      Frm_EndForm ();

   /***** End box *****/
   Inf_AfterTree ();
  }

void Inf_ReceiveURLInfo (void)
  {
   extern const char *Txt_The_URL_X_has_been_updated;
   struct Inf_Info Info;
   char URL[WWW_MAX_BYTES_WWW + 1];
   char PathFile[PATH_MAX + 1];
   FILE *FileURL;
   Err_SuccessOrError_t URLIsOK = Err_ERROR;

   /***** Set info type *****/
   Info.Type = Inf_AsignInfoType ();

   /***** Get parameter with URL *****/
   Par_GetParText ("InfoSrcURL",URL,WWW_MAX_BYTES_WWW);

   /***** Build path to file containing URL *****/
   Inf_BuildPathURL (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Info.Type,
		     PathFile);

   /***** Open file with URL *****/
   if ((FileURL = fopen (PathFile,"wb")) != NULL)
     {
      /***** Write URL *****/
      fprintf (FileURL,"%s",URL);

      /***** Close file *****/
      fclose (FileURL);

      /***** Write message *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_URL_X_has_been_updated,URL);
      URLIsOK = Err_SUCCESS;
     }
   else
      Ale_ShowAlert (Ale_ERROR,"Error when saving URL to file.");

   switch (URLIsOK)
     {
      case Err_SUCCESS:
	 /***** Change info source to URL in database *****/
	 Inf_DB_SetInfoSrc (Info.Type,Inf_URL);

	 /***** Show the updated info *****/
	 Inf_ShowInfo ();
	 break;
      case Err_ERROR:
      default:
	 /***** Change info source to none in database *****/
	 Inf_DB_SetInfoSrc (Info.Type,Inf_SRC_NONE);

	 /***** Show again the form to select and send course info *****/
	 Inf_ConfigInfo ();
	 break;
     }
  }
