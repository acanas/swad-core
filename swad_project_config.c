// swad_project_config.c: projects configuration
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_project.h"
#include "swad_project_config.h"
#include "swad_project_database.h"
#include "swad_rubric.h"
#include "swad_rubric_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void PrjCfg_ShowFormNETCanCreate (const struct PrjCfg_Config *Config);
static void PrjCfg_ShowFormsRubrics (void);
static void PrjCfg_ShowFormsRubricsOfType (const struct Rub_Rubrics *Rubrics,
                                           PrjCfg_RubricType_t RubricType);

static void PrjCfg_GetConfigDataFromRow (MYSQL_RES *mysql_res,
				         struct PrjCfg_Config *Config);
static void PrjCfg_GetListRubCods (const struct Rub_Rubrics *Rubrics,
                                   struct PrgCfg_ListRubCods *ListRubCods);
static bool PrjCfg_GetIfNETCanCreateFromForm (void);

/*****************************************************************************/
/************** Get configuration of projects for current course *************/
/*****************************************************************************/

void PrjCfg_GetConfig (struct PrjCfg_Config *Config)
  {
   MYSQL_RES *mysql_res;

   Config->NETCanCreate = PrjCfg_NET_CAN_CREATE_DEFAULT;
   if (Prj_DB_GetConfig (&mysql_res))
      PrjCfg_GetConfigDataFromRow (mysql_res,Config);
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Can I configure all projects? **********************/
/*****************************************************************************/

bool PrjCfg_CheckIfICanConfig (void)
  {
   static const bool ICanConfigAllProjects[Rol_NUM_ROLES] =
     {
      [Rol_TCH	  ] = true,
      [Rol_SYS_ADM] = true,
     };

   return ICanConfigAllProjects[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/********************** Configuration of all projects ************************/
/*****************************************************************************/

void PrjCfg_ShowFormConfig (void)
  {
   extern const char *Hlp_ASSESSMENT_Projects;
   extern const char *Txt_Configure_projects;
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];
   extern const char *Txt_Rubrics;
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Configure_projects,Prj_PutIconsListProjects,&Projects,
                 Hlp_ASSESSMENT_Projects,Box_NOT_CLOSABLE);

      /***** Projects are editable by non-editing teachers? *****/
      Frm_BeginForm (ActChgNETCanCrePrj);
	 HTM_FIELDSET_Begin (NULL);
	    HTM_LEGEND (Txt_Actions[ActChgPrj]);
	    PrjCfg_ShowFormNETCanCreate (&Projects.Config);
	 HTM_FIELDSET_End ();
      Frm_EndForm ();

      /***** Rubrics *****/
      if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)	// TODO: Remove this rectriction when rubrics are finished
	{
	 HTM_FIELDSET_Begin (NULL);
	    HTM_LEGEND (Txt_Rubrics);
	    PrjCfg_ShowFormsRubrics ();
	 HTM_FIELDSET_End ();
	}

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************ Show forms to edit rubrics associated to projects **************/
/*****************************************************************************/

static void PrjCfg_ShowFormsRubrics (void)
  {
   struct Rub_Rubrics Rubrics;
   PrjCfg_RubricType_t RubricType;

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);

   /***** Get list of rubrics *****/
   Rub_GetListRubrics (&Rubrics);

   /***** Rubric selectors *****/
   HTM_TABLE_BeginCenterPadding (2);
      for (RubricType  = (PrjCfg_RubricType_t) 1;
	   RubricType <= (PrjCfg_RubricType_t) (PrjCfg_NUM_RUBRIC_TYPES - 1);
	   RubricType++)
	 PrjCfg_ShowFormsRubricsOfType (&Rubrics,RubricType);
   HTM_TABLE_End ();

   /***** Free list of rubrics *****/
   Rub_FreeListRubrics (&Rubrics);
  }

/*****************************************************************************/
/*** Show form to choose the rubrics of a given type associated to projects **/
/*****************************************************************************/

static void PrjCfg_ShowFormsRubricsOfType (const struct Rub_Rubrics *Rubrics,
                                           PrjCfg_RubricType_t RubricType)
  {
   extern const char *Par_CodeStr[];
   extern const char *Prj_DB_RubricType[PrjCfg_NUM_RUBRIC_TYPES];
   extern const char *Txt_PROJECT_RUBRIC[PrjCfg_NUM_RUBRIC_TYPES];
   extern const char *Txt_no_rubric;
   extern const char *Txt_add_rubric;
   MYSQL_RES *mysql_res;
   unsigned NumRubricsThisType;
   unsigned NumRubThisType;
   unsigned NumRubThisCrs;
   long RubCodThisType;
   char Title[Rub_MAX_BYTES_TITLE + 1];

   /***** Get project rubrics for current course from database *****/
   NumRubricsThisType = Prj_DB_GetRubricsOfType (&mysql_res,RubricType);

   /***** Row with form for rubric *****/
   HTM_TR_Begin (NULL);

      /* Label */
      HTM_TD_TxtColon (Txt_PROJECT_RUBRIC[RubricType]);

      /* Selectors of rubrics */
      HTM_TD_Begin ("class=\"LT\"");

	 Frm_BeginForm (ActChgRubPrj);
	    Par_PutParString (NULL,"RubricType",Prj_DB_RubricType[RubricType]);

	    /* One selector for each rubric */
	    for (NumRubThisType = 0;
		 NumRubThisType < NumRubricsThisType;
		 NumRubThisType++)
	      {
	       RubCodThisType = DB_GetNextCode (mysql_res);

	       HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
				 "name=\"%s\" class=\"PrjCfg_RUBRIC_SEL\"",
				 Par_CodeStr[ParCod_Rub]);
		  /* First option to indicate that no rubric is selected */
		  HTM_OPTION (HTM_Type_STRING,"-1",
			      HTM_OPTION_UNSELECTED,
			      HTM_OPTION_ENABLED,
			      "[%s]",Txt_no_rubric);

		  /* One option for each rubric in this course */
		  for (NumRubThisCrs = 0;
		       NumRubThisCrs < Rubrics->Num;
		       NumRubThisCrs++)
		    {
		     Rub_DB_GetRubricTitle (Rubrics->Lst[NumRubThisCrs],Title,Rub_MAX_BYTES_TITLE);
		     HTM_OPTION (HTM_Type_LONG,&Rubrics->Lst[NumRubThisCrs],
				 Rubrics->Lst[NumRubThisCrs] == RubCodThisType ? HTM_OPTION_SELECTED :
										 HTM_OPTION_UNSELECTED,
				 HTM_OPTION_ENABLED,
				 "%s",Title);
		    }

	       HTM_SELECT_End ();
	       HTM_BR ();
	      }

	    /* One extra selector to add a new rubric,
	       only if not all available rubrics are selected */
	    if (NumRubricsThisType < Rubrics->Num)
	      {
	       HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
				 "name=\"%s\" class=\"PrjCfg_RUBRIC_SEL\"",
				 Par_CodeStr[ParCod_Rub]);

		  /* First option to indicate that no rubric is selected */
		  HTM_OPTION (HTM_Type_STRING,"-1",
			      HTM_OPTION_SELECTED,
			      HTM_OPTION_DISABLED,
			      "[%s]",Txt_add_rubric);

		  /* One option for each rubric in this course */
		  for (NumRubThisCrs = 0;
		       NumRubThisCrs < Rubrics->Num;
		       NumRubThisCrs++)
		    {
		     Rub_DB_GetRubricTitle (Rubrics->Lst[NumRubThisCrs],Title,Rub_MAX_BYTES_TITLE);
		     HTM_OPTION (HTM_Type_LONG,&Rubrics->Lst[NumRubThisCrs],
				 HTM_OPTION_UNSELECTED,
				 HTM_OPTION_ENABLED,
				 "%s",Title);
		    }

	       HTM_SELECT_End ();
	      }

	 Frm_EndForm ();
      HTM_TD_End ();
   HTM_TR_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Get configuration values from a database table row *************/
/*****************************************************************************/

static void PrjCfg_GetConfigDataFromRow (MYSQL_RES *mysql_res,
				         struct PrjCfg_Config *Config)
  {
   MYSQL_ROW row;

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]	NETCanCreate
   */
   /***** Get whether non-editing teachers can create new projects or not (row[0]) *****/
   Config->NETCanCreate = (row[0][0] == 'Y');
  }

/*****************************************************************************/
/********************** Convert from string to type **************************/
/*****************************************************************************/

PrjCfg_RubricType_t PrjCfg_GetRubricFromString (const char *Str)
  {
   extern const char *Prj_DB_RubricType[PrjCfg_NUM_RUBRIC_TYPES];
   PrjCfg_RubricType_t RubricType;

   /***** Compare string with all string types *****/
   for (RubricType  = (PrjCfg_RubricType_t) 1;
	RubricType <= (PrjCfg_RubricType_t) (PrjCfg_NUM_RUBRIC_TYPES - 1);
	RubricType++)
      if (!strcmp (Prj_DB_RubricType[RubricType],Str))
	 return RubricType;

   return PrjCfg_RUBRIC_ERR;
  }

/*****************************************************************************/
/****** Show form to edit if non-editing teachers create new projects ********/
/*****************************************************************************/

static void PrjCfg_ShowFormNETCanCreate (const struct PrjCfg_Config *Config)
  {
   extern const char *Txt_Non_editing_teachers_can_create_new_projects;

   HTM_LABEL_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
      HTM_INPUT_CHECKBOX ("NETCanCreate",HTM_SUBMIT_ON_CHANGE,
			  "id=\"NETCanCreate\" value=\"Y\"%s",
			  Config->NETCanCreate ? " checked=\"checked\"" :
						 "");
      HTM_Txt (Txt_Non_editing_teachers_can_create_new_projects);
   HTM_LABEL_End ();
  }

/*****************************************************************************/
/********* Change whether non-editing teachers can create projects ***********/
/*****************************************************************************/

void PrjCfg_ChangeNETCanCreate (void)
  {
   extern const char *Txt_The_configuration_of_the_projects_has_been_updated;
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Get non-editing teachers can create new projects or not *****/
   Projects.Config.NETCanCreate = PrjCfg_GetIfNETCanCreateFromForm ();

   /***** Update database *****/
   Prj_DB_UpdateNETCanCreate (&Projects);

   /***** Show confirmation message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_configuration_of_the_projects_has_been_updated);

   /***** Show again the form to configure projects *****/
   PrjCfg_ShowFormConfig ();
  }

/*****************************************************************************/
/************************ Change rubrics of a type ***************************/
/*****************************************************************************/

void PrjCfg_ChangeRubricsOfType (void)
  {
   extern const char *Txt_The_configuration_of_the_projects_has_been_updated;
   struct Rub_Rubrics Rubrics;
   PrjCfg_RubricType_t RubricType;
   struct PrgCfg_ListRubCods ListRubCods;
   char StrType[32];

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);

   /***** Get list of rubrics *****/
   Rub_GetListRubrics (&Rubrics);

   /***** Get parameter with the type of rubric *****/
   Par_GetParText ("RubricType",StrType,sizeof (StrType) - 1);
   RubricType = PrjCfg_GetRubricFromString (StrType);

   /***** Get multiple parameter with rubric codes of this type *****/
   PrjCfg_GetListRubCods (&Rubrics,&ListRubCods);

   /***** Update database *****/
   Prj_DB_UpdateRubrics (RubricType,&ListRubCods);

   /***** Show confirmation message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_configuration_of_the_projects_has_been_updated);

   /***** Show again the form to configure projects *****/
   PrjCfg_ShowFormConfig ();
  }

/*****************************************************************************/
/******************* Get list of rubric codes of a type **********************/
/*****************************************************************************/

static void PrjCfg_GetListRubCods (const struct Rub_Rubrics *Rubrics,
                                   struct PrgCfg_ListRubCods *ListRubCods)
  {
   extern const char *Par_CodeStr[];
   char *ParLstRubCods;
   const char *Ptr;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   unsigned NumRub;
   unsigned MaxSizeListRubCods;

   /***** Reset number of rubrics specified *****/
   ListRubCods->NumRubrics = 0;
   if (Rubrics->Num)	// If course has rubrics
     {
      /***** Allocate memory for the list of rubric codes specified *****/
      MaxSizeListRubCods = ((Cns_MAX_DECIMAL_DIGITS_LONG + 1) * Rubrics->Num) - 1;
      if ((ParLstRubCods = malloc (MaxSizeListRubCods + 1)) == NULL)
	 Err_NotEnoughMemoryExit ();

      /***** Get parameter with list of groups to list *****/
      Par_GetParMultiToText (Par_CodeStr[ParCod_Rub],ParLstRubCods,MaxSizeListRubCods);
      if (ParLstRubCods[0])
	{
	 /***** Count number of rubric codes from ParLstRubCods *****/
	 for (Ptr = ParLstRubCods, NumRub = 0;
	      *Ptr;
	      NumRub++)
	    Par_GetNextStrUntilSeparParMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
	 ListRubCods->NumRubrics = NumRub;

	 if (ListRubCods->NumRubrics)	// If rubrics specified...
	   {
	    /***** Create a list of rubric codes from ParLstRubCods *****/
	    if ((ListRubCods->RubCods = calloc (ListRubCods->NumRubrics,
	                                        sizeof (*ListRubCods->RubCods))) == NULL)
	       Err_NotEnoughMemoryExit ();
	    for (Ptr = ParLstRubCods, NumRub = 0;
		 *Ptr;
		 NumRub++)
	      {
	       Par_GetNextStrUntilSeparParMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
	       ListRubCods->RubCods[NumRub] = Str_ConvertStrCodToLongCod (LongStr);
	      }
	   }
	}

      /***** Free memory used for the list of groups to show *****/
      free (ParLstRubCods);
     }
  }

/*****************************************************************************/
/****** Get if projects are creatable by non-editing teachers from form *******/
/*****************************************************************************/

static bool PrjCfg_GetIfNETCanCreateFromForm (void)
  {
   return Par_GetParBool ("NETCanCreate");
  }
