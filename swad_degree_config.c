// swad_degree_config.c: configuration of current degree

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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
#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For asprintf

#include "swad_database.h"
#include "swad_degree_config.h"
#include "swad_degree_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_config.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_logo.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void DegCfg_Configuration (bool PrintView);
static void DegCfg_PutIconsToPrintAndUpload (__attribute__((unused)) void *Args);
static void DegCfg_Title (bool PutLink);
static void DegCfg_Center (bool PrintView,bool PutForm);
static void DegCfg_FullName (bool PutForm);
static void DegCfg_ShrtName (bool PutForm);
static void DegCfg_WWW (bool PrintView,bool PutForm);
static void DegCfg_Shortcut (bool PrintView);
static void DegCfg_QR (void);
static void DegCfg_NumCrss (void);

/*****************************************************************************/
/****************** Show information of the current degree *******************/
/*****************************************************************************/

void DegCfg_ShowConfiguration (void)
  {
   DegCfg_Configuration (false);

   /***** Show help to enrol me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/****************** Print information of the current degree ******************/
/*****************************************************************************/

void DegCfg_PrintConfiguration (void)
  {
   DegCfg_Configuration (true);
  }

/*****************************************************************************/
/******************* Information of the current degree ***********************/
/*****************************************************************************/

static void DegCfg_Configuration (bool PrintView)
  {
   extern const char *Hlp_DEGREE_Information;
   bool PutLink;
   bool PutFormCtr;
   bool PutFormName;
   bool PutFormWWW;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Deg.DegCod <= 0)	// No degree selected
      return;

   /***** Initializations *****/
   PutLink     = !PrintView && Gbl.Hierarchy.Deg.WWW[0];
   PutFormCtr  = !PrintView && Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM;
   PutFormName = !PrintView && Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM;
   PutFormWWW  = !PrintView && Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM;

   /***** Begin box *****/
   if (PrintView)
      Box_BoxBegin (NULL,NULL,
                    NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,NULL,
                    DegCfg_PutIconsToPrintAndUpload,NULL,
		    Hlp_DEGREE_Information,Box_NOT_CLOSABLE);

   /***** Title *****/
   DegCfg_Title (PutLink);

      /**************************** Left part ***********************************/
      HTM_DIV_Begin ("class=\"HIE_CFG_LEFT HIE_CFG_WIDTH\"");

	 /***** Begin table *****/
	 HTM_TABLE_BeginWidePadding (2);

	    /***** Center *****/
	    DegCfg_Center (PrintView,PutFormCtr);

	    /***** Degree name *****/
	    DegCfg_FullName (PutFormName);
	    DegCfg_ShrtName (PutFormName);

	    /***** Degree WWW *****/
	    DegCfg_WWW (PrintView,PutFormWWW);

	    /***** Shortcut to the degree *****/
	    DegCfg_Shortcut (PrintView);

	    if (PrintView)
	       /***** QR code with link to the degree *****/
	       DegCfg_QR ();
	    else
	      {
	       /***** Number of courses *****/
	       DegCfg_NumCrss ();

	       /***** Number of users *****/
	       HieCfg_NumUsrsInCrss (HieLvl_DEG,Gbl.Hierarchy.Deg.DegCod,Rol_TCH);
	       HieCfg_NumUsrsInCrss (HieLvl_DEG,Gbl.Hierarchy.Deg.DegCod,Rol_NET);
	       HieCfg_NumUsrsInCrss (HieLvl_DEG,Gbl.Hierarchy.Deg.DegCod,Rol_STD);
	       HieCfg_NumUsrsInCrss (HieLvl_DEG,Gbl.Hierarchy.Deg.DegCod,Rol_UNK);
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
   Ico_PutContextualIconToPrint (ActPrnDegInf,
				 NULL,NULL);

   if (Gbl.Usrs.Me.Role.Logged >= Rol_DEG_ADM)
      // Only degree admins, center admins, institution admins and system admins
      // have permission to upload logo of the degree
      /***** Link to upload logo of degree *****/
      Lgo_PutIconToChangeLogo (HieLvl_DEG);
  }

/*****************************************************************************/
/******************** Show title in degree configuration *********************/
/*****************************************************************************/

static void DegCfg_Title (bool PutLink)
  {
   HieCfg_Title (PutLink,
		    HieLvl_DEG,				// Logo scope
		    Gbl.Hierarchy.Deg.DegCod,		// Logo code
                    Gbl.Hierarchy.Deg.ShrtName,		// Logo short name
		    Gbl.Hierarchy.Deg.FullName,		// Logo full name
		    Gbl.Hierarchy.Deg.WWW,		// Logo www
		    Gbl.Hierarchy.Deg.FullName);	// Text full name
  }

/*****************************************************************************/
/******************** Show center in degree configuration ********************/
/*****************************************************************************/

static void DegCfg_Center (bool PrintView,bool PutForm)
  {
   extern const char *The_Colors[The_NUM_THEMES];
   extern const char *The_ClassInput[The_NUM_THEMES];
   extern const char *Txt_Center;
   unsigned NumCtr;

   /***** Center *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",PutForm ? "OthCtrCod" :
				      NULL,
		       Txt_Center);

      /* Data */
      HTM_TD_Begin ("class=\"LB DAT_%s\"",The_Colors[Gbl.Prefs.Theme]);
	 if (PutForm)
	   {
	    /* Get list of centers of the current institution */
	    Ctr_GetBasicListOfCenters (Gbl.Hierarchy.Ins.InsCod);

	    /* Put form to select center */
	    Frm_BeginForm (ActChgDegCtrCfg);
	       HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
				 "id=\"OthCtrCod\" name=\"OthCtrCod\""
				 " class=\"INPUT_SHORT_NAME %s\"",
				 The_ClassInput[Gbl.Prefs.Theme]);
		  for (NumCtr = 0;
		       NumCtr < Gbl.Hierarchy.Ctrs.Num;
		       NumCtr++)
		     HTM_OPTION (HTM_Type_LONG,&Gbl.Hierarchy.Ctrs.Lst[NumCtr].CtrCod,
				 Gbl.Hierarchy.Ctrs.Lst[NumCtr].CtrCod == Gbl.Hierarchy.Ctr.CtrCod,false,
				 "%s",Gbl.Hierarchy.Ctrs.Lst[NumCtr].ShrtName);
	       HTM_SELECT_End ();
	    Frm_EndForm ();

	    /* Free list of centers */
	    Ctr_FreeListCenters ();
	   }
	 else	// I can not move degree to another center
	   {
	    if (!PrintView)
	      {
	       Frm_BeginFormGoTo (ActSeeCtrInf);
		  Ctr_PutParamCtrCod (Gbl.Hierarchy.Ctr.CtrCod);
		  HTM_BUTTON_OnSubmit_Begin (Str_BuildGoToTitle (Gbl.Hierarchy.Ctr.ShrtName),
					     "BT_LINK",NULL);
		  Str_FreeGoToTitle ();
	      }
	    Lgo_DrawLogo (HieLvl_CTR,Gbl.Hierarchy.Ctr.CtrCod,Gbl.Hierarchy.Ctr.ShrtName,
			  20,"LM",true);
	    HTM_NBSP ();
	    HTM_Txt (Gbl.Hierarchy.Ctr.FullName);
	    if (!PrintView)
	      {
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	      }
	   }
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************** Show degree full name in degree configuration ****************/
/*****************************************************************************/

static void DegCfg_FullName (bool PutForm)
  {
   extern const char *Txt_Degree;

   HieCfg_FullName (PutForm,Txt_Degree,ActRenDegFulCfg,
		    Gbl.Hierarchy.Deg.FullName);
  }

/*****************************************************************************/
/************** Show degree short name in degree configuration ***************/
/*****************************************************************************/

static void DegCfg_ShrtName (bool PutForm)
  {
   HieCfg_ShrtName (PutForm,ActRenDegShoCfg,Gbl.Hierarchy.Deg.ShrtName);
  }

/*****************************************************************************/
/***************** Show degree WWW in degree configuration *******************/
/*****************************************************************************/

static void DegCfg_WWW (bool PrintView,bool PutForm)
  {
   HieCfg_WWW (PrintView,PutForm,ActChgDegWWWCfg,Gbl.Hierarchy.Deg.WWW);
  }

/*****************************************************************************/
/*************** Show degree shortcut in degree configuration ****************/
/*****************************************************************************/

static void DegCfg_Shortcut (bool PrintView)
  {
   HieCfg_Shortcut (PrintView,"deg",Gbl.Hierarchy.Deg.DegCod);
  }

/*****************************************************************************/
/****************** Show degree QR in degree configuration *******************/
/*****************************************************************************/

static void DegCfg_QR (void)
  {
   HieCfg_QR ("deg",Gbl.Hierarchy.Deg.DegCod);
  }

/*****************************************************************************/
/************** Show number of courses in degree configuration ***************/
/*****************************************************************************/

static void DegCfg_NumCrss (void)
  {
   extern const char *The_Colors[The_NUM_THEMES];
   extern const char *Txt_Courses;
   extern const char *Txt_Courses_of_DEGREE_X;
   char *Title;

   /***** Number of courses *****/
   /* Begin table row */
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",NULL,Txt_Courses);

      /* Data */
      HTM_TD_Begin ("class=\"LB DAT_%s\"",The_Colors[Gbl.Prefs.Theme]);
	 Frm_BeginFormGoTo (ActSeeCrs);
	    Deg_PutParamDegCod (Gbl.Hierarchy.Deg.DegCod);
	    if (asprintf (&Title,Txt_Courses_of_DEGREE_X,Gbl.Hierarchy.Deg.ShrtName) < 0)
	       Err_NotEnoughMemoryExit ();
	    HTM_BUTTON_OnSubmit_Begin (Title,"BT_LINK",NULL);
	    free (Title);
	       HTM_Unsigned (Crs_GetCachedNumCrssInDeg (Gbl.Hierarchy.Deg.DegCod));
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
   extern const char *Txt_The_degree_X_already_exists;
   extern const char *Txt_The_degree_X_has_been_moved_to_the_center_Y;
   struct Ctr_Center NewCtr;

   /***** Get parameter with center code *****/
   NewCtr.CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);

   /***** Check if center has changed *****/
   if (NewCtr.CtrCod != Gbl.Hierarchy.Deg.CtrCod)
     {
      /***** Get data of new center *****/
      Ctr_GetDataOfCenterByCod (&NewCtr);

      /***** Check if it already exists a degree with the same name in the new center *****/
      if (Deg_DB_CheckIfDegNameExistsInCtr ("ShortName",Gbl.Hierarchy.Deg.ShrtName,Gbl.Hierarchy.Deg.DegCod,NewCtr.CtrCod))
         Ale_CreateAlert (Ale_WARNING,
                          Txt_The_degree_X_already_exists,
		          Gbl.Hierarchy.Deg.ShrtName);
      else if (Deg_DB_CheckIfDegNameExistsInCtr ("FullName",Gbl.Hierarchy.Deg.FullName,Gbl.Hierarchy.Deg.DegCod,NewCtr.CtrCod))
         Ale_CreateAlert (Ale_WARNING,
                          Txt_The_degree_X_already_exists,
		          Gbl.Hierarchy.Deg.FullName);
      else
	{
	 /***** Update center in table of degrees *****/
	 Deg_DB_UpdateDegCtr (Gbl.Hierarchy.Deg.DegCod,NewCtr.CtrCod);
	 Gbl.Hierarchy.Deg.CtrCod =
	 Gbl.Hierarchy.Ctr.CtrCod = NewCtr.CtrCod;

	 /***** Initialize again current course, degree, center... *****/
	 Hie_InitHierarchy ();

	 /***** Create alert to show the change made *****/
         Ale_CreateAlert (Ale_SUCCESS,NULL,
                          Txt_The_degree_X_has_been_moved_to_the_center_Y,
		          Gbl.Hierarchy.Deg.FullName,
		          Gbl.Hierarchy.Ctr.FullName);
	}
     }
  }

/*****************************************************************************/
/*************** Change the name of a degree in configuration ****************/
/*****************************************************************************/

void DegCfg_RenameDegreeShort (void)
  {
   Deg_RenameDegree (&Gbl.Hierarchy.Deg,Cns_SHRT_NAME);
  }

void DegCfg_RenameDegreeFull (void)
  {
   Deg_RenameDegree (&Gbl.Hierarchy.Deg,Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************* Change the WWW of a degree ************************/
/*****************************************************************************/

void DegCfg_ChangeDegWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Get parameters from form *****/
   /* Get the new WWW for the degree */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update the table changing old WWW by new WWW *****/
      Deg_DB_UpdateDegWWW (Gbl.Hierarchy.Deg.DegCod,NewWWW);
      Str_Copy (Gbl.Hierarchy.Deg.WWW,NewWWW,sizeof (Gbl.Hierarchy.Deg.WWW) - 1);

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
/** Show message of success after changing a degree in degree configuration **/
/*****************************************************************************/

void DegCfg_ContEditAfterChgDeg (void)
  {
   /***** Write success / warning message *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   DegCfg_ShowConfiguration ();
  }
