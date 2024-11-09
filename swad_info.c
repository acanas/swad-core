// swad_info.c: info about course

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Ca�as Vargas

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
#include "swad_syllabus.h"
#include "swad_tree.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

Inf_Type_t Inf_Types[Inf_NUM_TYPES] =
  {
   [Inf_UNKNOWN_TYPE	] = Inf_UNKNOWN_TYPE,
   [Inf_INFORMATION	] = Inf_INFORMATION,
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

static Tre_TreeType_t Inf_TreeTypes[Inf_NUM_TYPES] =
  {
   [Inf_UNKNOWN_TYPE	] = Tre_UNKNOWN,
   [Inf_INFORMATION	] = Tre_UNKNOWN,
   [Inf_TEACH_GUIDE	] = Tre_GUIDE,
   [Inf_SYLLABUS_LEC	] = Tre_SYLLABUS_LEC,
   [Inf_SYLLABUS_PRA	] = Tre_SYLLABUS_PRA,
   [Inf_BIBLIOGRAPHY	] = Tre_BIBLIOGRAPHY,
   [Inf_FAQ		] = Tre_FAQ,
   [Inf_LINKS		] = Tre_LINKS,
   [Inf_ASSESSMENT	] = Tre_ASSESSMENT,
  };

static const char *Inf_FileNamesForInfoType[Inf_NUM_TYPES] =
  {
   [Inf_UNKNOWN_TYPE	] = NULL,
   [Inf_INFORMATION	] = Cfg_CRS_INFO_INFORMATION,
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
   [Inf_SRC_NONE	][Inf_TEACH_GUIDE	] = ActUnk,
   [Inf_SRC_NONE	][Inf_SYLLABUS_LEC	] = ActUnk,
   [Inf_SRC_NONE	][Inf_SYLLABUS_PRA	] = ActUnk,
   [Inf_SRC_NONE	][Inf_BIBLIOGRAPHY	] = ActUnk,
   [Inf_SRC_NONE	][Inf_FAQ		] = ActUnk,
   [Inf_SRC_NONE	][Inf_LINKS		] = ActUnk,
   [Inf_SRC_NONE	][Inf_ASSESSMENT	] = ActUnk,

   [Inf_EDITOR		][Inf_UNKNOWN_TYPE	] = ActUnk,
   [Inf_EDITOR		][Inf_INFORMATION	] = ActEditorCrsInf,
   [Inf_EDITOR		][Inf_TEACH_GUIDE	] = ActEditorTchGui,
   [Inf_EDITOR		][Inf_SYLLABUS_LEC	] = ActEdiTreSyl,
   [Inf_EDITOR		][Inf_SYLLABUS_PRA	] = ActEdiTreSyl,
   [Inf_EDITOR		][Inf_BIBLIOGRAPHY	] = ActEditorBib,
   [Inf_EDITOR		][Inf_FAQ		] = ActEditorFAQ,
   [Inf_EDITOR		][Inf_LINKS		] = ActEditorCrsLnk,
   [Inf_EDITOR		][Inf_ASSESSMENT	] = ActEditorAss,

   [Inf_PLAIN_TEXT	][Inf_UNKNOWN_TYPE	] = ActUnk,
   [Inf_PLAIN_TEXT	][Inf_INFORMATION	] = ActEdiPlaTxtCrsInf,
   [Inf_PLAIN_TEXT	][Inf_TEACH_GUIDE	] = ActEdiPlaTxtTchGui,
   [Inf_PLAIN_TEXT	][Inf_SYLLABUS_LEC	] = ActEdiPlaTxtSyl,
   [Inf_PLAIN_TEXT	][Inf_SYLLABUS_PRA	] = ActEdiPlaTxtSyl,
   [Inf_PLAIN_TEXT	][Inf_BIBLIOGRAPHY	] = ActEdiPlaTxtBib,
   [Inf_PLAIN_TEXT	][Inf_FAQ		] = ActEdiPlaTxtFAQ,
   [Inf_PLAIN_TEXT	][Inf_LINKS		] = ActEdiPlaTxtCrsLnk,
   [Inf_PLAIN_TEXT	][Inf_ASSESSMENT	] = ActEdiPlaTxtAss,

   [Inf_RICH_TEXT	][Inf_UNKNOWN_TYPE	] = ActUnk,
   [Inf_RICH_TEXT	][Inf_INFORMATION	] = ActEdiRchTxtCrsInf,
   [Inf_RICH_TEXT	][Inf_TEACH_GUIDE	] = ActEdiRchTxtTchGui,
   [Inf_RICH_TEXT	][Inf_SYLLABUS_LEC	] = ActEdiRchTxtSyl,
   [Inf_RICH_TEXT	][Inf_SYLLABUS_PRA	] = ActEdiRchTxtSyl,
   [Inf_RICH_TEXT	][Inf_BIBLIOGRAPHY	] = ActEdiRchTxtBib,
   [Inf_RICH_TEXT	][Inf_FAQ		] = ActEdiRchTxtFAQ,
   [Inf_RICH_TEXT	][Inf_LINKS		] = ActEdiRchTxtCrsLnk,
   [Inf_RICH_TEXT	][Inf_ASSESSMENT	] = ActEdiRchTxtAss,

   [Inf_PAGE		][Inf_UNKNOWN_TYPE	] = ActUnk,
   [Inf_PAGE		][Inf_INFORMATION	] = ActEdiPagCrsInf,
   [Inf_PAGE		][Inf_TEACH_GUIDE	] = ActEdiPagTchGui,
   [Inf_PAGE		][Inf_SYLLABUS_LEC	] = ActEdiPagSyl,
   [Inf_PAGE		][Inf_SYLLABUS_PRA	] = ActEdiPagSyl,
   [Inf_PAGE		][Inf_BIBLIOGRAPHY	] = ActEdiPagBib,
   [Inf_PAGE		][Inf_FAQ		] = ActEdiPagFAQ,
   [Inf_PAGE		][Inf_LINKS		] = ActEdiPagCrsLnk,
   [Inf_PAGE		][Inf_ASSESSMENT	] = ActEdiPagAss,

   [Inf_URL		][Inf_UNKNOWN_TYPE	] = ActUnk,
   [Inf_URL		][Inf_INFORMATION	] = ActEdiURLCrsInf,
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
   [Inf_TEACH_GUIDE	] = ActReqLnkTchGui,
   [Inf_SYLLABUS_LEC	] = ActReqLnkSyl,
   [Inf_SYLLABUS_PRA	] = ActReqLnkSyl,
   [Inf_BIBLIOGRAPHY	] = ActReqLnkBib,
   [Inf_FAQ		] = ActReqLnkFAQ,
   [Inf_LINKS		] = ActReqLnkCrsLnk,
   [Inf_ASSESSMENT	] = ActReqLnkAss,
  };

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Inf_BeforeTree (Vie_ViewType_t ViewType,Inf_Src_t InfoSrc);
static void Inf_AfterTree (void);

static void Inf_PutIconsWhenViewing (void *Info);
static void Inf_PutIconsWhenEditing (void *Info);
static void Inf_PutIconsWhenConfiguring (void *Info);

static bool Inf_CheckPage (Inf_Type_t InfoType);
static bool Inf_CheckAndShowPage (void);

static bool Inf_CheckURL (Inf_Type_t InfoType);
static bool Inf_CheckAndShowURL (void);
static void Inf_BuildPathURL (long CrsCod,Inf_Type_t InfoType,
                              char PathFile[PATH_MAX + 1]);

static void Inf_ShowPage (const char *URL);

//---------------------- Force students to read info? -------------------------
static void Inf_ConfigInfoReading (void);
static void Inf_PutCheckboxForceStdsToReadInfo (bool MustBeRead,
						HTM_Attributes_t Attributes);
static void Inf_PutCheckboxConfirmIHaveReadInfo (void);
static bool Inf_GetMustBeReadFromForm (void);
static bool Inf_GetIfIHaveReadFromForm (void);

//-----------------------------------------------------------------------------
static void Inf_ConfigInfoSource (void);

static bool Inf_CheckIfInfoAvailable (Inf_Src_t InfoSrc);

static void Inf_FormToEnterIntegratedEditor (Inf_Src_t InfoSrc);
static void Inf_FormToEnterPlainTextEditor (Inf_Src_t InfoSrc);
static void Inf_FormToEnterRichTextEditor (Inf_Src_t InfoSrc);
static void Inf_FormToEnterSendingPage (Inf_Src_t InfoSrc);
static void Inf_FormToEnterSendingURL (Inf_Src_t InfoSrc);

static bool Inf_CheckPlainTxt (Inf_Type_t InfoType);
static bool Inf_CheckAndShowPlainTxt (void);

static bool Inf_CheckRichTxt (Inf_Type_t InfoType);
static bool Inf_CheckAndShowRichTxt (void);

/*****************************************************************************/
/*************************** Before and after tree ***************************/
/*****************************************************************************/

static void Inf_BeforeTree (Vie_ViewType_t ViewType,Inf_Src_t InfoSrc)
  {
   extern const char *Hlp_COURSE_Information_textual_information;
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
      [Inf_TEACH_GUIDE	] = &Hlp_COURSE_Guide,
      [Inf_SYLLABUS_LEC	] = &Hlp_COURSE_Syllabus,
      [Inf_SYLLABUS_PRA	] = &Hlp_COURSE_Syllabus,
      [Inf_BIBLIOGRAPHY	] = &Hlp_COURSE_Bibliography,
      [Inf_FAQ		] = &Hlp_COURSE_FAQ,
      [Inf_LINKS	] = &Hlp_COURSE_Links,
      [Inf_ASSESSMENT	] = &Hlp_COURSE_Assessment,
     };

   /***** Set info type *****/
   Inf_AsignInfoType (&Gbl.Crs.Info);

   /***** Set info source in database *****/
   if (InfoSrc != Inf_SRC_NONE)
      Inf_DB_SetInfoSrc (Gbl.Crs.Info.FromDB.Src = InfoSrc);

   /***** Get info source and check if info must be read from database *****/
   Inf_GetAndCheckInfoSrcFromDB (&Gbl.Crs.Info);

   /***** Begin box *****/
   Box_BoxBegin (Txt_INFO_TITLE[Gbl.Crs.Info.Type],
		 FunctionToDrawContextualIcons[ViewType],&Gbl.Crs.Info,
		 *Help[Gbl.Crs.Info.Type],Box_NOT_CLOSABLE);

      /***** Form to select syllabus *****/
      Syl_PutFormWhichSyllabus (ViewType);

      /***** Only for students: Have I read this information? ******/
      if (Gbl.Crs.Info.FromDB.MustBeRead && Gbl.Usrs.Me.Role.Logged == Rol_STD)
	 Inf_PutCheckboxConfirmIHaveReadInfo ();	// Checkbox to confirm that...
							// ...I have read this couse info

  }

static void Inf_AfterTree (void)
  {
   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******** Show course info (theory, practices, bibliography, etc.) ***********/
/*****************************************************************************/

void Inf_ShowInfo (void)
  {
   extern const char *Txt_No_information;
   bool ShowWarningNoInfo = false;

   /***** Begin box *****/
   Inf_BeforeTree (Vie_VIEW,Inf_SRC_NONE);

      switch (Gbl.Crs.Info.FromDB.Src)
	{
	 case Inf_SRC_NONE:
	    ShowWarningNoInfo = true;
	    break;
	 case Inf_EDITOR:
	    switch (Gbl.Crs.Info.Type)
	      {
	       case Inf_SYLLABUS_LEC:
	       case Inf_SYLLABUS_PRA:
		  ShowWarningNoInfo = !Tre_ShowTree ();
		  break;
	       case Inf_INFORMATION:
	       case Inf_TEACH_GUIDE:
	       case Inf_BIBLIOGRAPHY:
	       case Inf_FAQ:
	       case Inf_LINKS:
	       case Inf_ASSESSMENT:
		  ShowWarningNoInfo = true;
		  break;
	       case Inf_UNKNOWN_TYPE:
	       default:
		  Err_WrongTypeExit ();
		  break;
	      }
	    break;
	 case Inf_PLAIN_TEXT:
	    ShowWarningNoInfo = !Inf_CheckAndShowPlainTxt ();
	    break;
	 case Inf_RICH_TEXT:
	    ShowWarningNoInfo = !Inf_CheckAndShowRichTxt ();
	    break;
	 case Inf_PAGE:
	    /***** Open file with web page *****/
	    ShowWarningNoInfo = !Inf_CheckAndShowPage ();
	    break;
	 case Inf_URL:
	    /***** Check if file with URL exists *****/
	    ShowWarningNoInfo = !Inf_CheckAndShowURL ();
	    break;
	}

      if (ShowWarningNoInfo)
	 Ale_ShowAlert (Ale_INFO,Txt_No_information);

   /***** End box *****/
   Inf_AfterTree ();
  }

/*****************************************************************************/
/******** Edit course info (theory, practices, bibliography, etc.) ***********/
/*****************************************************************************/

void Inf_EditTree (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_EditTree ();
   Inf_AfterTree ();
  }

/*****************************************************************************/
/********* List tree nodes when click on view a node after edition ***********/
/*****************************************************************************/

void Inf_ViewNodeAfterEdit (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_ViewNodeAfterEdit ();
   Inf_AfterTree ();
  }

/*****************************************************************************/
/*********** List tree nodes with a form to change/create a node *************/
/*****************************************************************************/

void Inf_ReqChangeNode (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_ReqChangeNode ();
   Inf_AfterTree ();
  }

void Inf_ReqCreateNode (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_ReqCreateNode ();
   Inf_AfterTree ();
  }

/*****************************************************************************/
/*************** Receive form to change/create a tree node *******************/
/*****************************************************************************/

void Inf_ReceiveChgNode (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_ReceiveChgNode ();
   Inf_AfterTree ();
  }

void Inf_ReceiveNewNode (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_ReceiveNewNode ();
   Inf_AfterTree ();
  }

/*****************************************************************************/
/******************** Remove a tree node and its children ********************/
/*****************************************************************************/

void Inf_ReqRemNode (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_ReqRemNode ();
   Inf_AfterTree ();
  }

void Inf_RemoveNode (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_RemoveNode ();
   Inf_AfterTree ();
  }

/*****************************************************************************/
/*************************** Hide/unhide a tree node *************************/
/*****************************************************************************/

void Inf_HideNode (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_HideOrUnhideNode (HidVis_HIDDEN);
   Inf_AfterTree ();
  }

void Inf_UnhideNode (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_HideOrUnhideNode (HidVis_VISIBLE);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/*********** Move up/down position of a subtree in a course tree *************/
/*****************************************************************************/

void Inf_MoveUpNode (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_MoveUpDownNode (Tre_MOVE_UP);
   Inf_AfterTree ();
  }

void Inf_MoveDownNode (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_MoveUpDownNode (Tre_MOVE_DOWN);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/************** Move a subtree to left/right in a course program *************/
/*****************************************************************************/

void Inf_MoveLeftNode (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_MoveLeftRightNode (Tre_MOVE_LEFT);
   Inf_AfterTree ();
  }

void Inf_MoveRightNode (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_MoveLeftRightNode (Tre_MOVE_RIGHT);
   Inf_AfterTree ();
  }

/*****************************************************************************/
/****************** Expand/contract a node in a course tree ******************/
/*****************************************************************************/

void Inf_ExpandNodeSeeing (void)
  {
   Inf_BeforeTree (Vie_VIEW,Inf_EDITOR);
      Tre_ExpandContractNode (Tre_EXPAND,Tre_VIEW);
   Inf_AfterTree ();
  }

void Inf_ContractNodeSeeing (void)
  {
   Inf_BeforeTree (Vie_VIEW,Inf_EDITOR);
      Tre_ExpandContractNode (Tre_CONTRACT,Tre_VIEW);
   Inf_AfterTree ();
  }

void Inf_ExpandNodeEditing (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_ExpandContractNode (Tre_EXPAND,Tre_EDIT_NODES);
   Inf_AfterTree ();
  }

void Inf_ContractNodeEditing (void)
  {
   Inf_BeforeTree (Vie_EDIT,Inf_EDITOR);
      Tre_ExpandContractNode (Tre_CONTRACT,Tre_EDIT_NODES);
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
	 Node.TreeType = Inf_TreeTypes[((struct Inf_Info *) Info)->Type];
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
      Node.TreeType = Inf_TreeTypes[((struct Inf_Info *) Info)->Type];
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
      Node.TreeType = Inf_TreeTypes[((struct Inf_Info *) Info)->Type];
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
// Return true if info available

static bool Inf_CheckPage (Inf_Type_t InfoType)
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
   if (Fil_CheckIfPathExists (PathRelFileHTML))	// TODO: Check if not empty?
      return true;

   /* 2. If index.html does not exist, try index.htm */
   snprintf (PathRelFileHTML,sizeof (PathRelFileHTML),"%s/index.htm",
	     PathRelDirHTML);
   if (Fil_CheckIfPathExists (PathRelFileHTML))	// TODO: Check if not empty?
      return true;

   return false;
  }

/*****************************************************************************/
/**************** Check if exists and show link to a page ********************/
/*****************************************************************************/
// Return true if info available

static bool Inf_CheckAndShowPage (void)
  {
   char PathRelDirHTML[PATH_MAX + 1];
   char PathRelFileHTML[PATH_MAX + 1 + 10 + 1];
   char URL[PATH_MAX + 1];

   // TODO !!!!!!!!!!!! If the page is hosted in server ==> it should be created a temporary public directory
   //                                                       and host the page in a private directory !!!!!!!!!!!!!!!!!

   /***** Build path of directory containing web page *****/
   Inf_BuildPathPage (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Gbl.Crs.Info.Type,
		      PathRelDirHTML);

   /***** Open file with web page *****/
   /* 1. Check if index.html exists */
   snprintf (PathRelFileHTML,sizeof (PathRelFileHTML),"%s/index.html",
	     PathRelDirHTML);
   if (Fil_CheckIfPathExists (PathRelFileHTML))	// TODO: Check if not empty?
     {
      snprintf (URL,sizeof (URL),"%s/%ld/%s/index.html",
	        Cfg_URL_CRS_PUBLIC,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
	        Inf_FileNamesForInfoType[Gbl.Crs.Info.Type]);
      Inf_ShowPage (URL);

      return true;
     }

   /* 2. If index.html does not exist, try index.htm */
   snprintf (PathRelFileHTML,sizeof (PathRelFileHTML),"%s/index.htm",
	     PathRelDirHTML);
   if (Fil_CheckIfPathExists (PathRelFileHTML))	// TODO: Check if not empty?
     {
      snprintf (URL,sizeof (URL),"%s/%ld/%s/index.htm",
	        Cfg_URL_CRS_PUBLIC,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
	        Inf_FileNamesForInfoType[Gbl.Crs.Info.Type]);
      Inf_ShowPage (URL);

      return true;
     }

   return false;
  }

/*****************************************************************************/
/* Build path inside a course for a given a info type to store web page file */
/*****************************************************************************/

void Inf_BuildPathPage (long CrsCod,Inf_Type_t InfoType,char PathDir[PATH_MAX + 1])
  {
   snprintf (PathDir,PATH_MAX + 1,"%s/%ld/%s",
             Cfg_PATH_CRS_PUBLIC,CrsCod,Inf_FileNamesForInfoType[InfoType]);
  }

/*****************************************************************************/
/********************* Check if exists link to a page ************************/
/*****************************************************************************/
// Return true if info available

static bool Inf_CheckURL (Inf_Type_t InfoType)
  {
   char PathFile[PATH_MAX + 1];
   FILE *FileURL;

   /***** Build path to file containing URL *****/
   Inf_BuildPathURL (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,PathFile);

   /***** Check if file with URL exists *****/
   if ((FileURL = fopen (PathFile,"rb")))
     {
      if (fgets (Gbl.Crs.Info.URL,WWW_MAX_BYTES_WWW,FileURL) == NULL)
	 Gbl.Crs.Info.URL[0] = '\0';
      /* File is not longer needed  ==> close it */
      fclose (FileURL);

      if (Gbl.Crs.Info.URL[0])
         return true;
     }

   return false;
  }

/*****************************************************************************/
/**************** Check if exists and show link to a page ********************/
/*****************************************************************************/
// Return true if info available

static bool Inf_CheckAndShowURL (void)
  {
   char PathFile[PATH_MAX + 1];
   FILE *FileURL;

   /***** Build path to file containing URL *****/
   Inf_BuildPathURL (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Gbl.Crs.Info.Type,
		     PathFile);

   /***** Check if file with URL exists *****/
   if ((FileURL = fopen (PathFile,"rb")))
     {
      if (fgets (Gbl.Crs.Info.URL,WWW_MAX_BYTES_WWW,FileURL) == NULL)
	 Gbl.Crs.Info.URL[0] = '\0';
      /* File is not longer needed  ==> close it */
      fclose (FileURL);

      if (Gbl.Crs.Info.URL[0])
	{
	 Inf_ShowPage (Gbl.Crs.Info.URL);
         return true;
	}
     }

   return false;
  }

/*****************************************************************************/
/*** Build path inside a course for a given a info type to store URL file ****/
/*****************************************************************************/

static void Inf_BuildPathURL (long CrsCod,Inf_Type_t InfoType,
                              char PathFile[PATH_MAX + 1])
  {
   snprintf (PathFile,PATH_MAX + 1,"%s/%ld/%s.url",
	     Cfg_PATH_CRS_PRIVATE,CrsCod,Inf_FileNamesForInfoType[InfoType]);
  }

/*****************************************************************************/
/************* Check if exists and write URL into text buffer ****************/
/*****************************************************************************/
// This function is called only from web service

void Inf_WriteURLIntoTxtBuffer (char TxtBuffer[WWW_MAX_BYTES_WWW + 1])
  {
   char PathFile[PATH_MAX + 1];
   FILE *FileURL;

   /***** Initialize buffer *****/
   TxtBuffer[0] = '\0';

   /***** Build path to file containing URL *****/
   Inf_BuildPathURL (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Gbl.Crs.Info.Type,
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
   Inf_Src_t InfoSrcSelected;

   /***** Set info type *****/
   Inf_AsignInfoType (&Gbl.Crs.Info);

   /***** Set info source into database *****/
   InfoSrcSelected = Inf_GetInfoSrcFromForm ();
   Inf_DB_SetInfoSrc (InfoSrcSelected);

   /***** Show the selected info *****/
   Inf_ShowInfo ();
  }

/*****************************************************************************/
/************** Configure course info (reading and info source) **************/
/*****************************************************************************/

void Inf_ConfigInfo (void)
  {
   /***** Begin box *****/
   Inf_BeforeTree (Vie_CONFIG,Inf_SRC_NONE);

      /***** Force students to read info? *****/
      Inf_ConfigInfoReading ();

      /***** Information source *****/
      Inf_ConfigInfoSource ();

   /***** End box *****/
   Inf_AfterTree ();
  }

/*****************************************************************************/
/********** Configure info reading (force students to read info?) ************/
/*****************************************************************************/

static void Inf_ConfigInfoReading (void)
  {
   extern const char *Txt_Reading;

   HTM_FIELDSET_Begin (NULL);
      HTM_LEGEND (Txt_Reading);
	 // Checkbox to force students to read this couse info
	 Mnu_ContextMenuBegin ();
	    // Non-editing teachers can not change the status of checkbox
	    Inf_PutCheckboxForceStdsToReadInfo (Gbl.Crs.Info.FromDB.MustBeRead,
						(Gbl.Usrs.Me.Role.Logged == Rol_NET) ? HTM_DISABLED :
										       HTM_NO_ATTR);
	 Mnu_ContextMenuEnd ();
   HTM_FIELDSET_End ();
  }

/*****************************************************************************/
/********** Put a form (checkbox) to force students to read info *************/
/*****************************************************************************/

static void Inf_PutCheckboxForceStdsToReadInfo (bool MustBeRead,
						HTM_Attributes_t Attributes)
  {
   extern const char *Txt_Force_students_to_read_this_information;
   static struct Act_ActionFunc Inf_Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = {ActUnk		,NULL,NULL},
      [Inf_INFORMATION	] = {ActChgFrcReaCrsInf	,NULL,NULL},
      [Inf_TEACH_GUIDE	] = {ActChgFrcReaTchGui	,NULL,NULL},
      [Inf_SYLLABUS_LEC	] = {ActChgFrcReaSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_LEC  ]},
      [Inf_SYLLABUS_PRA	] = {ActChgFrcReaSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_PRA]},
      [Inf_BIBLIOGRAPHY	] = {ActChgFrcReaBib	,NULL,NULL},
      [Inf_FAQ		] = {ActChgFrcReaFAQ	,NULL,NULL},
      [Inf_LINKS	] = {ActChgFrcReaCrsLnk	,NULL,NULL},
      [Inf_ASSESSMENT	] = {ActChgFrcReaAss	,NULL,NULL},
     };

   Lay_PutContextualCheckbox (Inf_Actions[Gbl.Crs.Info.Type].NextAction,
                              Inf_Actions[Gbl.Crs.Info.Type].FuncPars,
                              Inf_Actions[Gbl.Crs.Info.Type].Args,
                              "MustBeRead",
                              Attributes |
                              (MustBeRead ? HTM_CHECKED :
                        		    HTM_NO_ATTR) |
                              HTM_SUBMIT_ON_CHANGE,
                              Txt_Force_students_to_read_this_information,
                              Txt_Force_students_to_read_this_information);
  }

/*****************************************************************************/
/********** Put a form (checkbox) to force students to read info *************/
/*****************************************************************************/

static void Inf_PutCheckboxConfirmIHaveReadInfo (void)
  {
   extern const char *Txt_I_have_read_this_information;
   static struct Act_ActionFunc Inf_Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = {ActUnk		,NULL,NULL},
      [Inf_INFORMATION	] = {ActChgHavReaCrsInf	,NULL,NULL},
      [Inf_TEACH_GUIDE	] = {ActChgHavReaTchGui	,NULL,NULL},
      [Inf_SYLLABUS_LEC	] = {ActChgHavReaSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_LEC  ]},
      [Inf_SYLLABUS_PRA	] = {ActChgHavReaSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_PRA]},
      [Inf_BIBLIOGRAPHY	] = {ActChgHavReaBib	,NULL,NULL},
      [Inf_FAQ		] = {ActChgHavReaFAQ	,NULL,NULL},
      [Inf_LINKS	] = {ActChgHavReaCrsLnk	,NULL,NULL},
      [Inf_ASSESSMENT	] = {ActChgHavReaAss	,NULL,NULL},
     };
   bool IHaveRead = Inf_DB_CheckIfIHaveReadInfo ();

   Mnu_ContextMenuBegin ();
      Lay_PutContextualCheckbox (Inf_Actions[Gbl.Crs.Info.Type].NextAction,
				 Inf_Actions[Gbl.Crs.Info.Type].FuncPars,
				 Inf_Actions[Gbl.Crs.Info.Type].Args,
				 "IHaveRead",
				 (IHaveRead ? HTM_CHECKED :
					      HTM_NO_ATTR) | HTM_SUBMIT_ON_CHANGE,
				 Txt_I_have_read_this_information,
				 Txt_I_have_read_this_information);
   Mnu_ContextMenuEnd ();
  }

/*****************************************************************************/
/********* Get if students must read any info about current course ***********/
/*****************************************************************************/

bool Inf_GetIfIMustReadAnyCrsInfoInThisCrs (void)
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
      Gbl.Crs.Info.MustBeRead[InfoType] = false;

   /***** Get info types where students must read info *****/
   NumInfos = Inf_DB_GetInfoTypesfIMustReadInfo (&mysql_res);

   /***** Set must-be-read to true for each rown in result *****/
   for (NumInfo = 0;
	NumInfo < NumInfos;
	NumInfo++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get info type (row[0]) */
      InfoType = Inf_DB_ConvertFromStrDBToInfoType (row[0]);

      Gbl.Crs.Info.MustBeRead[InfoType] = true;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return (NumInfos != 0);
  }

/*****************************************************************************/
/***** Write message if students must read any info about current course *****/
/*****************************************************************************/

void Inf_WriteMsgYouMustReadInfo (void)
  {
   extern const char *Txt_Required_reading;
   extern const char *Txt_You_should_read_the_following_information;
   static struct Act_ActionFunc Inf_Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = {ActUnk		,NULL,NULL},
      [Inf_INFORMATION	] = {ActSeeCrsInf	,NULL,NULL},
      [Inf_TEACH_GUIDE	] = {ActSeeTchGui	,NULL,NULL},
      [Inf_SYLLABUS_LEC	] = {ActSeeSyl		,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_LEC  ]},
      [Inf_SYLLABUS_PRA	] = {ActSeeSyl		,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_PRA]},
      [Inf_BIBLIOGRAPHY	] = {ActSeeBib		,NULL,NULL},
      [Inf_FAQ		] = {ActSeeFAQ		,NULL,NULL},
      [Inf_LINKS	] = {ActSeeCrsLnk	,NULL,NULL},
      [Inf_ASSESSMENT	] = {ActSeeAss		,NULL,NULL},
     };
   Inf_Type_t InfoType;
   const char *TitleAction;

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
	       if (Gbl.Crs.Info.MustBeRead[InfoType])
		 {
		  HTM_LI_Begin (NULL);
		     Frm_BeginForm (Inf_Actions[InfoType].NextAction);
			if (Inf_Actions[InfoType].FuncPars)
			   Inf_Actions[InfoType].FuncPars (Inf_Actions[InfoType].Args);
		        TitleAction = Act_GetTitleAction (Inf_Actions[InfoType].NextAction);
			HTM_BUTTON_Submit_Begin (TitleAction,
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

/*****************************************************************************/
/****** Change teacher's preference about force students to read info ********/
/*****************************************************************************/

void Inf_ChangeForceReadInfo (void)
  {
   extern const char *Txt_Students_now_are_required_to_read_this_information;
   extern const char *Txt_Students_are_no_longer_obliged_to_read_this_information;
   bool MustBeRead = Inf_GetMustBeReadFromForm ();

   /***** Set status (if info must be read or not) into database *****/
   Inf_DB_SetForceRead (MustBeRead);

   /***** Write message of success *****/
   Ale_ShowAlert (Ale_SUCCESS,
                  MustBeRead ? Txt_Students_now_are_required_to_read_this_information :
                               Txt_Students_are_no_longer_obliged_to_read_this_information);

   /***** Show the selected info *****/
   Inf_ShowInfo ();
  }

/*****************************************************************************/
/************** Change confirmation of I have read course info ***************/
/*****************************************************************************/

void Inf_ChangeIHaveReadInfo (void)
  {
   extern const char *Txt_You_have_confirmed_that_you_have_read_this_information;
   extern const char *Txt_You_have_eliminated_the_confirmation_that_you_have_read_this_information;
   bool IHaveRead = Inf_GetIfIHaveReadFromForm ();

   /***** Set status (if I have read or not a information) into database *****/
   Inf_DB_SetIHaveRead (IHaveRead);

   /***** Write message of success *****/
   Ale_ShowAlert (Ale_SUCCESS,
                  IHaveRead ? Txt_You_have_confirmed_that_you_have_read_this_information :
                              Txt_You_have_eliminated_the_confirmation_that_you_have_read_this_information);

   /***** Show the selected info *****/
   Inf_ShowInfo ();
  }

/*****************************************************************************/
/************* Get if info must be read by students from form ****************/
/*****************************************************************************/

static bool Inf_GetMustBeReadFromForm (void)
  {
   return Par_GetParBool ("MustBeRead");
  }

/*****************************************************************************/
/************* Get if info must be read by students from form ****************/
/*****************************************************************************/

static bool Inf_GetIfIHaveReadFromForm (void)
  {
   return Par_GetParBool ("IHaveRead");
  }

/*****************************************************************************/
/************************** Configure info source ****************************/
/*****************************************************************************/

static void Inf_ConfigInfoSource (void)
  {
   extern const char *Txt_Source_of_information;
   extern const char *Txt_INFO_SRC_FULL_TEXT[Inf_NUM_SOURCES];
   extern const char *Txt_INFO_SRC_HELP[Inf_NUM_SOURCES];
   Inf_Src_t InfoSrc;
   bool InfoAvailable[Inf_NUM_SOURCES];
   static Act_Action_t Inf_ActionsSelecInfoSrc[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActSelInfSrcCrsInf,
      [Inf_TEACH_GUIDE	] = ActSelInfSrcTchGui,
      [Inf_SYLLABUS_LEC	] = ActSelInfSrcSyl,
      [Inf_SYLLABUS_PRA	] = ActSelInfSrcSyl,
      [Inf_BIBLIOGRAPHY	] = ActSelInfSrcBib,
      [Inf_FAQ		] = ActSelInfSrcFAQ,
      [Inf_LINKS	] = ActSelInfSrcCrsLnk,
      [Inf_ASSESSMENT	] = ActSelInfSrcAss,
     };
   /* Functions to write forms in course edition (FAQ, links, etc.) */
   static void (*Inf_FormsForEditionTypes[Inf_NUM_SOURCES])(Inf_Src_t InfoSrc) =
     {
      [Inf_SRC_NONE	] = NULL,
      [Inf_EDITOR	] = Inf_FormToEnterIntegratedEditor,
      [Inf_PLAIN_TEXT	] = Inf_FormToEnterPlainTextEditor,
      [Inf_RICH_TEXT	] = Inf_FormToEnterRichTextEditor,
      [Inf_PAGE		] = Inf_FormToEnterSendingPage,
      [Inf_URL		] = Inf_FormToEnterSendingURL,
     };

   /***** Check if info available *****/
   for (InfoSrc  = (Inf_Src_t) 0;
	InfoSrc <= (Inf_Src_t) (Inf_NUM_SOURCES - 1);
	InfoSrc++)
      InfoAvailable[InfoSrc] = Inf_CheckIfInfoAvailable (InfoSrc);

   /***** Set info source to none
	  when no info available for the current source *****/
   if (Gbl.Crs.Info.FromDB.Src != Inf_SRC_NONE &&
       !InfoAvailable[Gbl.Crs.Info.FromDB.Src])
     {
      Gbl.Crs.Info.FromDB.Src = Inf_SRC_NONE;
      Inf_DB_SetInfoSrc (Inf_SRC_NONE);
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
			     InfoSrc == Gbl.Crs.Info.FromDB.Src ? " BG_HIGHLIGHT" :
								  "");
		  Frm_BeginForm (Inf_ActionsSelecInfoSrc[Gbl.Crs.Info.Type]);
		     Inf_PutParInfoType (&Gbl.Crs.Info.Type);
		     HTM_INPUT_RADIO ("InfoSrc",
				      ((InfoSrc == Gbl.Crs.Info.FromDB.Src) ? HTM_CHECKED :
									      HTM_NO_ATTR) |
				      ((InfoSrc == Inf_SRC_NONE ||
				       InfoAvailable[InfoSrc]) ? HTM_NO_ATTR :
								 HTM_DISABLED) |
				      ((InfoSrc != Gbl.Crs.Info.FromDB.Src &&
				       (InfoSrc == Inf_SRC_NONE ||
				       InfoAvailable[InfoSrc])) ? HTM_SUBMIT_ON_CLICK :
								  HTM_NO_ATTR),
				      "id=\"InfoSrc%u\" value=\"%u\"",
				      (unsigned) InfoSrc,(unsigned) InfoSrc);
		  Frm_EndForm ();
	       HTM_TD_End ();

	       /* Form for this info source */
	       HTM_TD_Begin ("class=\"LT%s\"",
			     InfoSrc == Gbl.Crs.Info.FromDB.Src ? " BG_HIGHLIGHT" :
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
		     Inf_FormsForEditionTypes[InfoSrc] (InfoSrc);
	       HTM_TD_End ();

	    HTM_TR_End ();
	   }

     HTM_TABLE_End ();
   HTM_FIELDSET_End ();
  }

/*****************************************************************************/
/* Check if there is info available for current info type and a given source */
/*****************************************************************************/

static bool Inf_CheckIfInfoAvailable (Inf_Src_t InfoSrc)
  {
   switch (InfoSrc)
     {
      case Inf_SRC_NONE:
	 return false;
      case Inf_EDITOR:
         switch (Gbl.Crs.Info.Type)
           {
            case Inf_SYLLABUS_LEC:
               return Syl_CheckSyllabus (Tre_SYLLABUS_LEC);
            case Inf_SYLLABUS_PRA:
               return Syl_CheckSyllabus (Tre_SYLLABUS_PRA);
            default:
               return false;
           }
         return false;	// Not reached
      case Inf_PLAIN_TEXT:
         return Inf_CheckPlainTxt (Gbl.Crs.Info.Type);
      case Inf_RICH_TEXT:
         return Inf_CheckRichTxt (Gbl.Crs.Info.Type);
      case Inf_PAGE:
	 return Inf_CheckPage (Gbl.Crs.Info.Type);
      case Inf_URL:
	 return Inf_CheckURL (Gbl.Crs.Info.Type);
     }

   return false;	// Not reached
  }

/*****************************************************************************/
/****************** Form to enter in integrated editor ***********************/
/*****************************************************************************/

static void Inf_FormToEnterIntegratedEditor (Inf_Src_t InfoSrc)
  {
   Frm_BeginForm (Inf_ActionsInfo[InfoSrc][Gbl.Crs.Info.Type]);
      Inf_PutParInfoType (&Gbl.Crs.Info.Type);
      Btn_PutConfirmButton (Act_GetActionText (Inf_ActionsInfo[InfoSrc][Gbl.Crs.Info.Type]));
   Frm_EndForm ();
  }

/*****************************************************************************/
/****************** Form to enter in plain text editor ***********************/
/*****************************************************************************/

static void Inf_FormToEnterPlainTextEditor (Inf_Src_t InfoSrc)
  {
   extern const char *Txt_Edit_plain_text;

   Frm_BeginForm (Inf_ActionsInfo[InfoSrc][Gbl.Crs.Info.Type]);
      Inf_PutParInfoType (&Gbl.Crs.Info.Type);
      Btn_PutConfirmButton (Txt_Edit_plain_text);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************* Form to enter in rich text editor ***********************/
/*****************************************************************************/

static void Inf_FormToEnterRichTextEditor (Inf_Src_t InfoSrc)
  {
   extern const char *Txt_Edit_rich_text;

   Frm_BeginForm (Inf_ActionsInfo[InfoSrc][Gbl.Crs.Info.Type]);
      Inf_PutParInfoType (&Gbl.Crs.Info.Type);
      Btn_PutConfirmButton (Txt_Edit_rich_text);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************* Form to upload a file with a page ***********************/
/*****************************************************************************/

static void Inf_FormToEnterSendingPage (Inf_Src_t InfoSrc)
  {
   extern const char *Txt_Upload_file;

   Frm_BeginForm (Inf_ActionsInfo[InfoSrc][Gbl.Crs.Info.Type]);
      Inf_PutParInfoType (&Gbl.Crs.Info.Type);
      Btn_PutConfirmButton (Txt_Upload_file);
   Frm_EndForm ();
  }

/*****************************************************************************/
/********************* Form to send a link to a web page *********************/
/*****************************************************************************/

static void Inf_FormToEnterSendingURL (Inf_Src_t InfoSrc)
  {
   extern const char *Txt_Send_URL;

   Frm_BeginForm (Inf_ActionsInfo[InfoSrc][Gbl.Crs.Info.Type]);
      Inf_PutParInfoType (&Gbl.Crs.Info.Type);
      Btn_PutConfirmButton (Txt_Send_URL);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******** Returns bibliography, assessment, etc. from Gbl.Action.Act *********/
/*****************************************************************************/

void Inf_AsignInfoType (struct Inf_Info *Info)
  {
   switch (Act_GetSuperAction (Gbl.Action.Act))
     {
      case ActSeeCrsInf:
         Info->Type = Inf_INFORMATION;
         break;
      case ActSeeTchGui:
         Info->Type = Inf_TEACH_GUIDE;
         break;
      case ActSeeSyl:
	 Info->Type = Inf_GetParInfoType ();
	 break;
      case ActSeeBib:
         Info->Type = Inf_BIBLIOGRAPHY;
         break;
      case ActSeeFAQ:
         Info->Type = Inf_FAQ;
         break;
      case ActSeeCrsLnk:
         Info->Type = Inf_LINKS;
         break;
      case ActSeeAss:
         Info->Type = Inf_ASSESSMENT;
         break;
      default:
	 Err_WrongActionExit ();
	 break;
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

Inf_Src_t Inf_GetInfoSrcFromDB (long CrsCod,Inf_Type_t InfoType)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Inf_Src_t InfoSrc;

   /***** Get info source for a specific type of info from database *****/
   if (Inf_DB_GetInfoSrc (&mysql_res,CrsCod,InfoType))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get info source (row[0]) */
      InfoSrc = Inf_DB_ConvertFromStrDBToInfoSrc (row[0]);
     }
   else
      InfoSrc = Inf_SRC_NONE;

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
   Info->FromDB.MustBeRead = false;

   /***** Get info source for a specific type of info from database *****/
   if (Inf_DB_GetInfoSrcAndMustBeRead (&mysql_res,Gbl.Hierarchy.Node[Hie_CRS].HieCod,Info->Type) == 1)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get info source (row[0]) and if students must read info (row[1]) */
      Info->FromDB.Src        = Inf_DB_ConvertFromStrDBToInfoSrc (row[0]);
      Info->FromDB.MustBeRead = (row[1][0] == 'Y');
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** If info is empty, return Inf_NONE *****/
   switch (Info->FromDB.Src)
     {
      case Inf_SRC_NONE:
         break;
      case Inf_EDITOR:
         switch (Info->Type)
           {
            case Inf_SYLLABUS_LEC:
               if (!Syl_CheckSyllabus (Tre_SYLLABUS_LEC))
                  Info->FromDB.Src = Inf_SRC_NONE;
               break;
            case Inf_SYLLABUS_PRA:
               if (!Syl_CheckSyllabus (Tre_SYLLABUS_PRA))
                  Info->FromDB.Src = Inf_SRC_NONE;
               break;
            case Inf_UNKNOWN_TYPE:
            case Inf_INFORMATION:
            case Inf_TEACH_GUIDE:
            case Inf_BIBLIOGRAPHY:
            case Inf_FAQ:
            case Inf_LINKS:
            case Inf_ASSESSMENT:
               Info->FromDB.Src = Inf_SRC_NONE;
	       break;	// Internal editor is not yet available
           }
         break;
      case Inf_PLAIN_TEXT:
	 if (!Inf_CheckPlainTxt (Info->Type))
            Info->FromDB.Src = Inf_SRC_NONE;
         break;
      case Inf_RICH_TEXT:
	 if (!Inf_CheckRichTxt (Info->Type))
            Info->FromDB.Src = Inf_SRC_NONE;
         break;
      case Inf_PAGE:
	 if (!Inf_CheckPage (Info->Type))
	    Info->FromDB.Src = Inf_SRC_NONE;
         break;
      case Inf_URL:
	 if (!Inf_CheckURL (Info->Type))
	    Info->FromDB.Src = Inf_SRC_NONE;
         break;
     }

   if (Info->FromDB.Src == Inf_SRC_NONE)
      Info->FromDB.MustBeRead = false;
  }

/*****************************************************************************/
/********** Get info text for a type of course info from database ************/
/*****************************************************************************/

void Inf_GetInfoTxtFromDB (long CrsCod,Inf_Type_t InfoType,
                           char InfoTxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1],
                           char InfoTxtMD  [Cns_MAX_BYTES_LONG_TEXT + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get info source for a specific type of course information
          (bibliography, FAQ, links or assessment) from database *****/
   if (Inf_DB_GetInfoTxt (&mysql_res,CrsCod,InfoType) == 1)
     {
      /* Get info text */
      row = mysql_fetch_row (mysql_res);

      /* Get text in HTML format (not rigorous) */
      if (InfoTxtHTML)
	 Str_Copy (InfoTxtHTML,row[0],Cns_MAX_BYTES_LONG_TEXT);

      /* Get text in Markdown format */
      if (InfoTxtMD)
	 Str_Copy (InfoTxtMD  ,row[1],Cns_MAX_BYTES_LONG_TEXT);
     }
   else
     {
      if (InfoTxtHTML)
         InfoTxtHTML[0] = '\0';
      if (InfoTxtMD)
         InfoTxtMD  [0] = '\0';
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Check information about the course ********************/
/*****************************************************************************/
// Return true if info available

static bool Inf_CheckPlainTxt (Inf_Type_t InfoType)
  {
   char TxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Get info text from database *****/
   Inf_GetInfoTxtFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,TxtHTML,NULL);

   return (TxtHTML[0] != '\0');
  }

/*****************************************************************************/
/********************* Show information about the course *********************/
/*****************************************************************************/
// Return true if info available

static bool Inf_CheckAndShowPlainTxt (void)
  {
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   char TxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Get info text from database *****/
   Inf_GetInfoTxtFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Gbl.Crs.Info.Type,
                         TxtHTML,NULL);

   if (TxtHTML[0])
     {
      switch (Gbl.Crs.Info.Type)
	{
	 case Inf_INFORMATION:
	 case Inf_TEACH_GUIDE:
	    Lay_WriteHeaderClassPhoto (Vie_VIEW);
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

      return true;
     }

   return false;
  }

/*****************************************************************************/
/********************* Show information about the course *********************/
/*****************************************************************************/
// Return true if info available

static bool Inf_CheckRichTxt (Inf_Type_t InfoType)
  {
   char TxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1];
   char TxtMD[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Get info text from database *****/
   Inf_GetInfoTxtFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,InfoType,
                         TxtHTML,TxtMD);

   return (TxtMD[0] != '\0');
  }

/*****************************************************************************/
/********************* Show information about the course *********************/
/*****************************************************************************/
// Return true if info available

static bool Inf_CheckAndShowRichTxt (void)
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
   Inf_GetInfoTxtFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Gbl.Crs.Info.Type,
                         TxtHTML,TxtMD);

   if (TxtMD[0])
     {
      switch (Gbl.Crs.Info.Type)
	{
	 case Inf_INFORMATION:
	 case Inf_TEACH_GUIDE:
	    Lay_WriteHeaderClassPhoto (Vie_VIEW);
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
	 /* MathJax 3.0.1 */
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

      return true;
     }

   return false;
  }

/*****************************************************************************/
/************** Edit plain text information about the course *****************/
/*****************************************************************************/

void Inf_EditPlainTxtInfo (void)
  {
   extern const char *Hlp_COURSE_Information_edit;
   extern const char *Hlp_COURSE_Guide_edit;
   extern const char *Hlp_COURSE_Syllabus_edit;
   extern const char *Hlp_COURSE_Bibliography_edit;
   extern const char *Hlp_COURSE_FAQ_edit;
   extern const char *Hlp_COURSE_Links_edit;
   extern const char *Hlp_COURSE_Assessment_edit;
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   extern const char *Txt_Save_changes;
   static struct Act_ActionFunc Inf_Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = {ActUnk		,NULL,NULL},
      [Inf_INFORMATION	] = {ActRcvPlaTxtCrsInf	,NULL,NULL},
      [Inf_TEACH_GUIDE	] = {ActRcvPlaTxtTchGui	,NULL,NULL},
      [Inf_SYLLABUS_LEC	] = {ActRcvPlaTxtSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_LEC  ]},
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
      [Inf_TEACH_GUIDE	] = &Hlp_COURSE_Guide_edit,
      [Inf_SYLLABUS_LEC	] = &Hlp_COURSE_Syllabus_edit,
      [Inf_SYLLABUS_PRA	] = &Hlp_COURSE_Syllabus_edit,
      [Inf_BIBLIOGRAPHY	] = &Hlp_COURSE_Bibliography_edit,
      [Inf_FAQ		] = &Hlp_COURSE_FAQ_edit,
      [Inf_LINKS	] = &Hlp_COURSE_Links_edit,
      [Inf_ASSESSMENT	] = &Hlp_COURSE_Assessment_edit,
     };
   char TxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Set info type *****/
   Inf_AsignInfoType (&Gbl.Crs.Info);

   /***** Begin form and box *****/
   Frm_BeginForm (Inf_Actions[Gbl.Crs.Info.Type].NextAction);
      if (Inf_Actions[Gbl.Crs.Info.Type].FuncPars)
	 Inf_Actions[Gbl.Crs.Info.Type].FuncPars (Inf_Actions[Gbl.Crs.Info.Type].Args);
      Box_BoxBegin (Txt_INFO_TITLE[Gbl.Crs.Info.Type],NULL,NULL,
		    *HelpEdit[Gbl.Crs.Info.Type],Box_NOT_CLOSABLE);

	 switch (Gbl.Crs.Info.Type)
	   {
	    case Inf_INFORMATION:
	    case Inf_TEACH_GUIDE:
	       Lay_WriteHeaderClassPhoto (Vie_VIEW);
	       break;
	    default:
	       break;
	   }

	 /***** Get info text from database *****/
	 Inf_GetInfoTxtFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Gbl.Crs.Info.Type,
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
      Box_BoxWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************** Edit rich text information about the course *****************/
/*****************************************************************************/

void Inf_EditRichTxtInfo (void)
  {
   extern const char *Hlp_COURSE_Information_edit;
   extern const char *Hlp_COURSE_Guide_edit;
   extern const char *Hlp_COURSE_Syllabus_edit;
   extern const char *Hlp_COURSE_Bibliography_edit;
   extern const char *Hlp_COURSE_FAQ_edit;
   extern const char *Hlp_COURSE_Links_edit;
   extern const char *Hlp_COURSE_Assessment_edit;
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   extern const char *Txt_Save_changes;
   char TxtHTML[Cns_MAX_BYTES_LONG_TEXT + 1];
   static struct Act_ActionFunc Inf_Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = {ActUnk		,NULL,NULL},
      [Inf_INFORMATION	] = {ActRcvRchTxtCrsInf	,NULL,NULL},
      [Inf_TEACH_GUIDE	] = {ActRcvRchTxtTchGui	,NULL,NULL},
      [Inf_SYLLABUS_LEC	] = {ActRcvRchTxtSyl	,Inf_PutParInfoType,&Inf_Types[Inf_SYLLABUS_LEC  ]},
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
      [Inf_TEACH_GUIDE	] = &Hlp_COURSE_Guide_edit,
      [Inf_SYLLABUS_LEC	] = &Hlp_COURSE_Syllabus_edit,
      [Inf_SYLLABUS_PRA	] = &Hlp_COURSE_Syllabus_edit,
      [Inf_BIBLIOGRAPHY	] = &Hlp_COURSE_Bibliography_edit,
      [Inf_FAQ		] = &Hlp_COURSE_FAQ_edit,
      [Inf_LINKS	] = &Hlp_COURSE_Links_edit,
      [Inf_ASSESSMENT	] = &Hlp_COURSE_Assessment_edit,
     };

   /***** Begin form and box *****/
   Frm_BeginForm (Inf_Actions[Gbl.Crs.Info.Type].NextAction);
      if (Inf_Actions[Gbl.Crs.Info.Type].FuncPars)
	 Inf_Actions[Gbl.Crs.Info.Type].FuncPars (Inf_Actions[Gbl.Crs.Info.Type].Args);
      Box_BoxBegin (Txt_INFO_TITLE[Gbl.Crs.Info.Type],NULL,NULL,
		    *HelpEdit[Gbl.Crs.Info.Type],Box_NOT_CLOSABLE);

      switch (Gbl.Crs.Info.Type)
	{
	 case Inf_INFORMATION:
	 case Inf_TEACH_GUIDE:
	    Lay_WriteHeaderClassPhoto (Vie_VIEW);
	    break;
	 default:
	    break;
	}

      /***** Get info text from database *****/
      Inf_GetInfoTxtFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Gbl.Crs.Info.Type,
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
      Box_BoxWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************** Receive and change plain text of course info ****************/
/*****************************************************************************/

void Inf_ReceivePlainTxtInfo (void)
  {
   char Txt_HTMLFormat    [Cns_MAX_BYTES_LONG_TEXT + 1];
   char Txt_MarkdownFormat[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Set info type *****/
   Inf_AsignInfoType (&Gbl.Crs.Info);

   /***** Get text with course information from form *****/
   Par_GetPar (Par_PARAM_SINGLE,"Txt",Txt_HTMLFormat,
	       Cns_MAX_BYTES_LONG_TEXT,NULL);
   Str_Copy (Txt_MarkdownFormat,Txt_HTMLFormat,sizeof (Txt_MarkdownFormat) - 1);
   Str_ChangeFormat (Str_FROM_FORM,Str_TO_HTML,
                     Txt_HTMLFormat,Cns_MAX_BYTES_LONG_TEXT,Str_REMOVE_SPACES);
   Str_ChangeFormat (Str_FROM_FORM,Str_TO_MARKDOWN,
                     Txt_MarkdownFormat,Cns_MAX_BYTES_LONG_TEXT,Str_REMOVE_SPACES);	// Store a copy in Markdown format

   /***** Update text of course info in database *****/
   Inf_DB_SetInfoTxt (Txt_HTMLFormat,Txt_MarkdownFormat);

   /***** Change info source to "plain text" in database *****/
   Inf_DB_SetInfoSrc (Txt_HTMLFormat[0] ? Inf_PLAIN_TEXT :
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
   char Txt_HTMLFormat    [Cns_MAX_BYTES_LONG_TEXT + 1];
   char Txt_MarkdownFormat[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Set info type *****/
   Inf_AsignInfoType (&Gbl.Crs.Info);

   /***** Get text with course information from form *****/
   Par_GetPar (Par_PARAM_SINGLE,"Txt",Txt_HTMLFormat,
               Cns_MAX_BYTES_LONG_TEXT,NULL);
   Str_Copy (Txt_MarkdownFormat,Txt_HTMLFormat,sizeof (Txt_MarkdownFormat) - 1);
   Str_ChangeFormat (Str_FROM_FORM,Str_TO_HTML,
                     Txt_HTMLFormat,Cns_MAX_BYTES_LONG_TEXT,Str_REMOVE_SPACES);
   Str_ChangeFormat (Str_FROM_FORM,Str_TO_MARKDOWN,
                     Txt_MarkdownFormat,Cns_MAX_BYTES_LONG_TEXT,Str_REMOVE_SPACES);	// Store a copy in Markdown format

   /***** Update text of course info in database *****/
   Inf_DB_SetInfoTxt (Txt_HTMLFormat,Txt_MarkdownFormat);

   /***** Change info source to "rich text" in database *****/
   Inf_DB_SetInfoSrc (Txt_HTMLFormat[0] ? Inf_RICH_TEXT :
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
   extern const char *Txt_Upload_file;
   static Act_Action_t Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActRcvPagCrsInf,
      [Inf_TEACH_GUIDE	] = ActRcvPagTchGui,
      [Inf_SYLLABUS_LEC	] = ActRcvPagSyl,
      [Inf_SYLLABUS_PRA	] = ActRcvPagSyl,
      [Inf_BIBLIOGRAPHY	] = ActRcvPagBib,
      [Inf_FAQ		] = ActRcvPagFAQ,
      [Inf_LINKS	] = ActRcvPagCrsLnk,
      [Inf_ASSESSMENT	] = ActRcvPagAss,
     };

   /***** Begin box *****/
   Inf_BeforeTree (Vie_EDIT,Inf_PAGE);

      /***** Begin form *****/
      Frm_BeginForm (Actions[Gbl.Crs.Info.Type]);
	 Inf_PutParInfoType (&Gbl.Crs.Info.Type);

	 /***** Help *****/
	 Ale_ShowAlert (Ale_INFO,Txt_INFO_SRC_HELP[Inf_PAGE]);

	 /***** File *****/
	 HTM_DIV_Begin ("class=\"CM\"");
	    HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	       HTM_TxtColonNBSP (Txt_File);
	       HTM_INPUT_FILE (Fil_NAME_OF_PARAM_FILENAME_ORG,".htm,.html,.pdf,.zip",
			       HTM_NO_ATTR,
			       NULL);
	    HTM_LABEL_End ();
	 HTM_DIV_End ();

	 /***** Send button *****/
	 Btn_PutCreateButton (Txt_Upload_file);

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
   struct Par_Param *Par;
   char SourceFileName[PATH_MAX + 1];
   char PathRelDirHTML[PATH_MAX + 1];
   char PathRelFileHTML[PATH_MAX + 1 + 10 + 1];
   char PathRelFileZIP[PATH_MAX + 1 + NAME_MAX + 1];
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   char StrUnzip[128 + PATH_MAX + 1 + NAME_MAX + 1 + PATH_MAX + 1];
   bool WrongType = false;
   bool FileIsOK = false;

   /***** Set info type *****/
   Inf_AsignInfoType (&Gbl.Crs.Info);

   /***** First of all, store in disk the file received *****/
   Par = Fil_StartReceptionOfFile (Fil_NAME_OF_PARAM_FILENAME_ORG,
                                   SourceFileName,MIMEType);

   /***** Check that MIME type is HTML or ZIP *****/
   if (strcmp (MIMEType,"text/html"))
      if (strcmp (MIMEType,"text/plain"))
         if (strcmp (MIMEType,"application/x-zip-compressed"))
            if (strcmp (MIMEType,"application/zip"))
               if (strcmp (MIMEType,"application/x-download"))
		  if (strcmp (MIMEType,"application/octet-stream"))
	             if (strcmp (MIMEType,"application/octetstream"))
	                if (strcmp (MIMEType,"application/octet"))
                           WrongType = true;
   if (WrongType)
      Ale_ShowAlert (Ale_INFO,Txt_The_file_type_is_X_and_should_be_HTML_or_ZIP,
                     MIMEType);
   else
     {
      /***** Build path of directory containing web page *****/
      Inf_BuildPathPage (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Gbl.Crs.Info.Type,
			 PathRelDirHTML);

      /***** End the reception of the data *****/
      if (Str_FileIs (SourceFileName,"html") ||
          Str_FileIs (SourceFileName,"htm" )) // .html or .htm file
        {
         Fil_RemoveTree (PathRelDirHTML);
         Fil_CreateDirIfNotExists (PathRelDirHTML);
         snprintf (PathRelFileHTML,sizeof (PathRelFileHTML),"%s/index.html",
		   PathRelDirHTML);
         if (Fil_EndReceptionOfFile (PathRelFileHTML,Par))
           {
            Ale_ShowAlert (Ale_SUCCESS,Txt_The_HTML_file_has_been_received_successfully);
            FileIsOK = true;
           }
         else
            Ale_ShowAlert (Ale_ERROR,"Error uploading file.");
        }
      else if (Str_FileIs (SourceFileName,"zip")) // .zip file
        {
         Fil_RemoveTree (PathRelDirHTML);
         Fil_CreateDirIfNotExists (PathRelDirHTML);
         snprintf (PathRelFileZIP,sizeof (PathRelFileZIP),"%s/%s.zip",
                   Gbl.Crs.Path.AbsPriv,
                   Inf_FileNamesForInfoType[Gbl.Crs.Info.Type]);

         if (Fil_EndReceptionOfFile (PathRelFileZIP,Par))
           {
            Ale_ShowAlert (Ale_SUCCESS,Txt_The_ZIP_file_has_been_received_successfully);

            /* Uncompress ZIP */
            snprintf (StrUnzip,sizeof (StrUnzip),"unzip -qq -o %s -d %s",
                      PathRelFileZIP,PathRelDirHTML);
            if (system (StrUnzip) == 0)
              {
               /* Check if uploaded file is index.html or index.htm */
               snprintf (PathRelFileHTML,sizeof (PathRelFileHTML),
        	         "%s/index.html",PathRelDirHTML);
               if (Fil_CheckIfPathExists (PathRelFileHTML))
                 {
                  Ale_ShowAlert (Ale_SUCCESS,Txt_The_ZIP_file_has_been_unzipped_successfully);
                  Ale_ShowAlert (Ale_SUCCESS,Txt_Found_an_index_html_file);
                  FileIsOK = true;
                 }
	       else
	         {
	          snprintf (PathRelFileHTML,sizeof (PathRelFileHTML),
	        	    "%s/index.htm",PathRelDirHTML);
	          if (Fil_CheckIfPathExists (PathRelFileHTML))
                    {
                     Ale_ShowAlert (Ale_SUCCESS,Txt_The_ZIP_file_has_been_unzipped_successfully);
                     Ale_ShowAlert (Ale_SUCCESS,Txt_Found_an_index_html_file);
                     FileIsOK = true;
                    }
	          else
                     Ale_ShowAlert (Ale_WARNING,Txt_No_file_index_html_found_within_the_ZIP_file);
	         }
	      }
            else
               Err_ShowErrorAndExit ("Can not unzip file.");
           }
         else
            Ale_ShowAlert (Ale_ERROR,"Error uploading file.");
        }
      else
         Ale_ShowAlert (Ale_WARNING,Txt_The_file_type_should_be_HTML_or_ZIP);
     }

   if (FileIsOK)
     {
      /***** Change info source to page in database *****/
      Inf_DB_SetInfoSrc (Inf_PAGE);

      /***** Show the updated info *****/
      Inf_ShowInfo ();
     }
   else
     {
      /***** Change info source to none in database *****/
      Inf_DB_SetInfoSrc (Inf_SRC_NONE);

      /***** Show again the form to select and send course info *****/
      Inf_ConfigInfo ();
     }
  }

/*****************************************************************************/
/********* Receive a link to a page of syllabus, bibliography, etc. **********/
/*****************************************************************************/

void Inf_EditURLInfo (void)
  {
   extern const char *Txt_URL;
   extern const char *Txt_Send_URL;
   char PathFile[PATH_MAX + 1];
   FILE *FileURL;
   static Act_Action_t Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActRcvURLCrsInf,
      [Inf_TEACH_GUIDE	] = ActRcvURLTchGui,
      [Inf_SYLLABUS_LEC	] = ActRcvURLSyl,
      [Inf_SYLLABUS_PRA	] = ActRcvURLSyl,
      [Inf_BIBLIOGRAPHY	] = ActRcvURLBib,
      [Inf_FAQ		] = ActRcvURLFAQ,
      [Inf_LINKS	] = ActRcvURLCrsLnk,
      [Inf_ASSESSMENT	] = ActRcvURLAss,
     };

   /***** Begin box *****/
   Inf_BeforeTree (Vie_EDIT,Inf_URL);

      /***** Build path to file containing URL *****/
      Inf_BuildPathURL (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Gbl.Crs.Info.Type,
			PathFile);

      /***** Begin form *****/
      Frm_BeginForm (Actions[Gbl.Crs.Info.Type]);
	 Inf_PutParInfoType (&Gbl.Crs.Info.Type);

	 /***** Link *****/
	 if ((FileURL = fopen (PathFile,"rb")) != NULL)
	   {
	    if (fgets (Gbl.Crs.Info.URL,WWW_MAX_BYTES_WWW,FileURL) == NULL)
	       Gbl.Crs.Info.URL[0] = '\0';
	    /* File is not longer needed. Close it */
	    fclose (FileURL);
	   }
	 else
	    Gbl.Crs.Info.URL[0] = '\0';

	 HTM_DIV_Begin ("class=\"CM\"");
	    HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	       HTM_TxtColonNBSP (Txt_URL);
	       HTM_INPUT_URL ("InfoSrcURL",Gbl.Crs.Info.URL,
			      HTM_NO_ATTR,	// TODO: Required?
			      "size=\"50\" class=\"INPUT_%s\"",The_GetSuffix ());
	    HTM_LABEL_End ();
	 HTM_DIV_End ();

	 /***** Send button *****/
	 Btn_PutCreateButton (Txt_Send_URL);

      /***** End form *****/
      Frm_EndForm ();

   /***** End box *****/
   Inf_AfterTree ();
  }

void Inf_ReceiveURLInfo (void)
  {
   extern const char *Txt_The_URL_X_has_been_updated;
   char PathFile[PATH_MAX + 1];
   FILE *FileURL;
   bool URLIsOK = false;

   /***** Set info type *****/
   Inf_AsignInfoType (&Gbl.Crs.Info);

   /***** Get parameter with URL *****/
   Par_GetParText ("InfoSrcURL",Gbl.Crs.Info.URL,WWW_MAX_BYTES_WWW);

   /***** Build path to file containing URL *****/
   Inf_BuildPathURL (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Gbl.Crs.Info.Type,
		     PathFile);

   /***** Open file with URL *****/
   if ((FileURL = fopen (PathFile,"wb")) != NULL)
     {
      /***** Write URL *****/
      fprintf (FileURL,"%s",Gbl.Crs.Info.URL);

      /***** Close file *****/
      fclose (FileURL);

      /***** Write message *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_URL_X_has_been_updated,
                     Gbl.Crs.Info.URL);
      URLIsOK = true;
     }
   else
      Ale_ShowAlert (Ale_ERROR,"Error when saving URL to file.");

   if (URLIsOK)
     {
      /***** Change info source to URL in database *****/
      Inf_DB_SetInfoSrc (Inf_URL);

      /***** Show the updated info *****/
      Inf_ShowInfo ();
     }
   else
     {
      /***** Change info source to none in database *****/
      Inf_DB_SetInfoSrc (Inf_SRC_NONE);

      /***** Show again the form to select and send course info *****/
      Inf_ConfigInfo ();
     }
  }

/*****************************************************************************/
/******************* Integrated editor for introduction **********************/
/*****************************************************************************/

void Inf_EditorCourseInfo (void)
  {
   extern const char *Txt_The_integrated_editor_is_not_yet_available;

   Ale_ShowAlert (Ale_WARNING,Txt_The_integrated_editor_is_not_yet_available);

   /***** Show again the form to select and send course info *****/
   Inf_ConfigInfo ();
  }

/*****************************************************************************/
/****************** Integrated editor for teaching guide *********************/
/*****************************************************************************/

void Inf_EditorTeachingGuide (void)
  {
   extern const char *Txt_The_integrated_editor_is_not_yet_available;

   Ale_ShowAlert (Ale_WARNING,Txt_The_integrated_editor_is_not_yet_available);

   /***** Show again the form to select and send course info *****/
   Inf_ConfigInfo ();
  }

/*****************************************************************************/
/******************* Integrated editor for bibliography **********************/
/*****************************************************************************/

void Inf_EditorBibliography (void)
  {
   extern const char *Txt_The_integrated_editor_is_not_yet_available;

   Ale_ShowAlert (Ale_WARNING,Txt_The_integrated_editor_is_not_yet_available);

   /***** Show again the form to select and send course info *****/
   Inf_ConfigInfo ();
  }

/*****************************************************************************/
/*********************** Integrated editor for FAQ ***************************/
/*****************************************************************************/

void Inf_EditorFAQ (void)
  {
   extern const char *Txt_The_integrated_editor_is_not_yet_available;

   Ale_ShowAlert (Ale_WARNING,Txt_The_integrated_editor_is_not_yet_available);

   /***** Show again the form to select and send course info *****/
   Inf_ConfigInfo ();
  }

/*****************************************************************************/
/********************* Integrated editor for links ***************************/
/*****************************************************************************/

void Inf_EditorLinks (void)
  {
   extern const char *Txt_The_integrated_editor_is_not_yet_available;

   Ale_ShowAlert (Ale_WARNING,Txt_The_integrated_editor_is_not_yet_available);

   /***** Show again the form to select and send course info *****/
   Inf_ConfigInfo ();
  }

/*****************************************************************************/
/****************** Integrated editor for assessment system ******************/
/*****************************************************************************/

void Inf_EditorAssessment (void)
  {
   extern const char *Txt_The_integrated_editor_is_not_yet_available;

   Ale_ShowAlert (Ale_WARNING,Txt_The_integrated_editor_is_not_yet_available);

   /***** Show again the form to select and send course info *****/
   Inf_ConfigInfo ();
  }
