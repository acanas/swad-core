// swad_degree_config.c: configuration of current degree

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_degree_config.h"
#include "swad_degree_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_config.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_logo.h"
#include "swad_parameter.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void DegCfg_Configuration (Vie_ViewType_t ViewType);
static void DegCfg_PutIconsToPrintAndUpload (__attribute__((unused)) void *Args);
static void DegCfg_Center (Vie_ViewType_t ViewType,Frm_PutForm_t PutForm);
static void DegCfg_NumCrss (void);

/*****************************************************************************/
/****************** Show information of the current degree *******************/
/*****************************************************************************/

void DegCfg_ShowConfiguration (void)
  {
   DegCfg_Configuration (Vie_VIEW);

   /***** Show help to enrol me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/****************** Print information of the current degree ******************/
/*****************************************************************************/

void DegCfg_PrintConfiguration (void)
  {
   DegCfg_Configuration (Vie_PRINT);
  }

/*****************************************************************************/
/******************* Information of the current degree ***********************/
/*****************************************************************************/

static void DegCfg_Configuration (Vie_ViewType_t ViewType)
  {
   extern const char *Hlp_DEGREE_Information;
   Hie_PutLink_t PutLink;
   Frm_PutForm_t PutFormCtr;
   Frm_PutForm_t PutFormName;
   Frm_PutForm_t PutFormWWW;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Node[Hie_DEG].HieCod <= 0)	// No degree selected
      return;

   /***** Initializations *****/
   PutLink     = (ViewType == Vie_VIEW &&
		  Gbl.Hierarchy.Node[Hie_DEG].WWW[0]) ? Hie_PUT_LINK :
							Hie_DONT_PUT_LINK;
   PutFormCtr  = (ViewType == Vie_VIEW &&
		  Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM) ? Frm_PUT_FORM :
							    Frm_DONT_PUT_FORM;
   PutFormName = (ViewType == Vie_VIEW &&
		  Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM) ? Frm_PUT_FORM :
							    Frm_DONT_PUT_FORM;
   PutFormWWW  = (ViewType == Vie_VIEW &&
		  Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM) ? Frm_PUT_FORM :
							    Frm_DONT_PUT_FORM;

   /***** Begin box *****/
   Box_BoxBegin (NULL,
		 ViewType == Vie_VIEW ? DegCfg_PutIconsToPrintAndUpload :
					NULL,NULL,
		 ViewType == Vie_VIEW ? Hlp_DEGREE_Information :
					NULL,Box_NOT_CLOSABLE);

      /***** Title *****/
      HieCfg_Title (PutLink,Hie_DEG);

      /**************************** Left part ***********************************/
      HTM_DIV_Begin ("class=\"HIE_CFG_LEFT\"");

	 /***** Begin table *****/
	 HTM_TABLE_BeginCenterPadding (2);

	    /***** Center *****/
	    DegCfg_Center (ViewType,PutFormCtr);

	    /***** Degree name *****/
	    HieCfg_Name (PutFormName,Hie_DEG,Nam_FULL_NAME);
	    HieCfg_Name (PutFormName,Hie_DEG,Nam_SHRT_NAME);

	    /***** Degree WWW *****/
	    HieCfg_WWW (ViewType,PutFormWWW,ActChgDegWWWCfg,Gbl.Hierarchy.Node[Hie_DEG].WWW);

	    /***** Shortcut to the degree *****/
	    HieCfg_Shortcut (ViewType,ParCod_Deg,Gbl.Hierarchy.Node[Hie_DEG].HieCod);

	    switch (ViewType)
	      {
	       case Vie_VIEW:
		  /***** Number of courses *****/
		  DegCfg_NumCrss ();

		  /***** Number of users *****/
		  HieCfg_NumUsrsInCrss (Hie_DEG,Gbl.Hierarchy.Node[Hie_DEG].HieCod,Rol_TCH);
		  HieCfg_NumUsrsInCrss (Hie_DEG,Gbl.Hierarchy.Node[Hie_DEG].HieCod,Rol_NET);
		  HieCfg_NumUsrsInCrss (Hie_DEG,Gbl.Hierarchy.Node[Hie_DEG].HieCod,Rol_STD);
		  HieCfg_NumUsrsInCrss (Hie_DEG,Gbl.Hierarchy.Node[Hie_DEG].HieCod,Rol_UNK);
		  break;
	       case Vie_PRINT:
		  /***** QR code with link to the degree *****/
		  HieCfg_QR (ParCod_Deg,Gbl.Hierarchy.Node[Hie_DEG].HieCod);
		  break;
	       default:
		  Err_WrongTypeExit ();
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();

      /***** End of left part *****/
      HTM_DIV_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************ Put contextual icons in configuration of a degree **************/
/*****************************************************************************/

static void DegCfg_PutIconsToPrintAndUpload (__attribute__((unused)) void *Args)
  {
   /***** Link to print info about degree *****/
   Ico_PutContextualIconToPrint (ActPrnDegInf,NULL,NULL);

   if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
      // Only degree admins, center admins, institution admins and system admins
      // have permission to upload logo of the degree
      /***** Link to upload logo of degree *****/
      Lgo_PutIconToChangeLogo (Hie_DEG);
  }

/*****************************************************************************/
/******************** Show center in degree configuration ********************/
/*****************************************************************************/

static void DegCfg_Center (Vie_ViewType_t ViewType,Frm_PutForm_t PutForm)
  {
   extern const char *Par_CodeStr[Par_NUM_PAR_COD];
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   unsigned NumCtr;
   const struct Hie_Node *Ctr;
   const char *Id[Frm_NUM_PUT_FORM] =
     {
      [Frm_DONT_PUT_FORM] = NULL,
      [Frm_PUT_FORM     ] = Par_CodeStr[ParCod_OthCtr],
     };

   /***** Center *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",Id[PutForm],Txt_HIERARCHY_SINGUL_Abc[Hie_CTR]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
         switch (PutForm)
           {
            case Frm_DONT_PUT_FORM:	// I can not move degree to another center
	       if (ViewType == Vie_VIEW)
		 {
		  Frm_BeginFormGoTo (ActSeeCtrInf);
		     ParCod_PutPar (ParCod_Ctr,Gbl.Hierarchy.Node[Hie_CTR].HieCod);
		     HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Gbl.Hierarchy.Node[Hie_CTR].ShrtName),NULL,
					      "class=\"LB BT_LINK\"");
		     Str_FreeGoToTitle ();
		 }
	       Lgo_DrawLogo (Hie_CTR,&Gbl.Hierarchy.Node[Hie_CTR],"LM ICO20x20");
	       HTM_NBSP ();
	       HTM_Txt (Gbl.Hierarchy.Node[Hie_CTR].FullName);
	       if (ViewType == Vie_VIEW)
		 {
		     HTM_BUTTON_End ();
		  Frm_EndForm ();
		 }
	       break;
            case Frm_PUT_FORM:
	       /* Get list of centers of the current institution */
	       Ctr_GetBasicListOfCenters (Gbl.Hierarchy.Node[Hie_INS].HieCod);

	       /* Put form to select center */
	       Frm_BeginForm (ActChgDegCtrCfg);
		  HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
				    "id=\"OthCtrCod\" name=\"OthCtrCod\""
				    " class=\"Frm_C2_INPUT INPUT_%s\"",
				    The_GetSuffix ());
		     for (NumCtr = 0;
			  NumCtr < Gbl.Hierarchy.List[Hie_INS].Num;
			  NumCtr++)
		       {
			Ctr = &Gbl.Hierarchy.List[Hie_INS].Lst[NumCtr];
			HTM_OPTION (HTM_Type_LONG,&Ctr->HieCod,
				    (Ctr->HieCod == Gbl.Hierarchy.Node[Hie_CTR].HieCod) ? HTM_SELECTED :
											  HTM_NO_ATTR,
				    "%s",Ctr->ShrtName);
		       }
		  HTM_SELECT_End ();
	       Frm_EndForm ();

	       /* Free list of centers */
	       Hie_FreeList (Hie_INS);
               break;
           }
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************** Show number of courses in degree configuration ***************/
/*****************************************************************************/

static void DegCfg_NumCrss (void)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Courses_of_DEGREE_X;
   char *Title;

   /***** Number of courses *****/
   /* Begin table row */
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_PLURAL_Abc[Hie_CRS]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 Frm_BeginFormGoTo (ActSeeCrs);
	    ParCod_PutPar (ParCod_Deg,Gbl.Hierarchy.Node[Hie_DEG].HieCod);
	    if (asprintf (&Title,Txt_Courses_of_DEGREE_X,
			  Gbl.Hierarchy.Node[Hie_DEG].ShrtName) < 0)
	       Err_NotEnoughMemoryExit ();
	    HTM_BUTTON_Submit_Begin (Title,NULL,"class=\"LB BT_LINK\"");
	    free (Title);
	       HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CRS,	// Number of courses...
						            Hie_DEG,	// ...in degree
						            Gbl.Hierarchy.Node[Hie_DEG].HieCod));
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
      HTM_TD_End ();

   /* End table row */
   HTM_TR_End ();
  }

/*****************************************************************************/
/************************ Change the center of a degree **********************/
/*****************************************************************************/

void DegCfg_ChangeDegCtr (void)
  {
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_degree_X_has_been_moved_to_the_center_Y;
   struct Hie_Node NewCtr;
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Get parameter with center code *****/
   NewCtr.HieCod = ParCod_GetAndCheckPar (ParCod_OthCtr);

   /***** Check if center has changed *****/
   if (NewCtr.HieCod != Gbl.Hierarchy.Node[Hie_DEG].PrtCod)
     {
      /***** Get data of new center *****/
      SuccessOrError = Hie_GetDataByCod[Hie_CTR] (&NewCtr);

      /***** Check if it already existed a degree with the same name in the new center *****/
      Names[Nam_SHRT_NAME] = Gbl.Hierarchy.Node[Hie_DEG].ShrtName;
      Names[Nam_FULL_NAME] = Gbl.Hierarchy.Node[Hie_DEG].FullName;
      if (Nam_CheckIfNameExists (Deg_DB_CheckIfDegNameExistsInCtr,
				 Names,
				 Gbl.Hierarchy.Node[Hie_DEG].HieCod,
				 NewCtr.HieCod,
				 0) == Exi_DOES_NOT_EXIST)	// Unused
	{
	 /***** Update center in table of degrees *****/
	 Deg_DB_UpdateDegCtr (Gbl.Hierarchy.Node[Hie_DEG].HieCod,NewCtr.HieCod);
	 Gbl.Hierarchy.Node[Hie_DEG].PrtCod =
	 Gbl.Hierarchy.Node[Hie_CTR].HieCod = NewCtr.HieCod;

	 /***** Initialize again current course, degree, center... *****/
	 Hie_InitHierarchy ();

	 /***** Create alert to show the change made *****/
         Ale_CreateAlert (Ale_SUCCESS,NULL,
                          Txt_The_degree_X_has_been_moved_to_the_center_Y,
		          Names[Nam_FULL_NAME],
		          Gbl.Hierarchy.Node[Hie_CTR].FullName);
	}
     }
  }

/*****************************************************************************/
/*************** Change the name of a degree in configuration ****************/
/*****************************************************************************/

void DegCfg_RenameDegreeShrt (void)
  {
   Deg_RenameDegree (&Gbl.Hierarchy.Node[Hie_DEG],Nam_SHRT_NAME);
  }

void DegCfg_RenameDegreeFull (void)
  {
   Deg_RenameDegree (&Gbl.Hierarchy.Node[Hie_DEG],Nam_FULL_NAME);
  }

/*****************************************************************************/
/************************* Change the WWW of a degree ************************/
/*****************************************************************************/

void DegCfg_ChangeDegWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[WWW_MAX_BYTES_WWW + 1];

   /***** Get parameters from form *****/
   /* Get the new WWW for the degree */
   Par_GetParText ("WWW",NewWWW,WWW_MAX_BYTES_WWW);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update the table changing old WWW by new WWW *****/
      Deg_DB_UpdateDegWWW (Gbl.Hierarchy.Node[Hie_DEG].HieCod,NewWWW);
      Str_Copy (Gbl.Hierarchy.Node[Hie_DEG].WWW,NewWWW,
	        sizeof (Gbl.Hierarchy.Node[Hie_DEG].WWW) - 1);

      /***** Write message to show the change made *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_new_web_address_is_X,
		     NewWWW);
     }
   else
      Ale_ShowAlertYouCanNotLeaveFieldEmpty ();

   /***** Show the form again *****/
   DegCfg_ShowConfiguration ();
  }

/*****************************************************************************/
/*********** Show a form for sending a logo of the current degree ************/
/*****************************************************************************/

void DegCfg_ReqLogo (void)
  {
   Lgo_RequestLogo (Hie_DEG);
  }

/*****************************************************************************/
/***************** Receive the logo of the current degree ********************/
/*****************************************************************************/

void DegCfg_ReceiveLogo (void)
  {
   Lgo_ReceiveLogo (Hie_DEG);
  }

/*****************************************************************************/
/****************** Remove the logo of the current degree ********************/
/*****************************************************************************/

void DegCfg_RemoveLogo (void)
  {
   Lgo_RemoveLogo (Hie_DEG);
  }

/*****************************************************************************/
/** Show message of success after changing a degree in degree configuration **/
/*****************************************************************************/

void DegCfg_ContEditAfterChgDeg (void)
  {
   /***** Write success / warning message *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   DegCfg_ShowConfiguration ();
  }
