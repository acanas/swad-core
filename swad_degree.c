// swad_degree.c: degrees

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
#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_admin_database.h"
#include "swad_box.h"
#include "swad_browser_database.h"
#include "swad_course_database.h"
#include "swad_database.h"
#include "swad_degree.h"
#include "swad_degree_config.h"
#include "swad_degree_database.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_forum_database.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_HTML.h"
#include "swad_logo.h"
#include "swad_message.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_photo_database.h"
#include "swad_survey.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private types *********************************/
/*****************************************************************************/

typedef enum
  {
   Deg_FIRST_YEAR,
   Deg_LAST_YEAR,
  } Deg_FirstOrLastYear_t;

/*****************************************************************************/
/**************************** Private variables ******************************/
/*****************************************************************************/

static struct Hie_Node *Deg_EditingDeg = NULL;	// Static variable to keep the degree being edited

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Deg_ListDegreesForEdition (const struct DegTyp_DegTypes *DegTypes);
static Usr_Can_t Deg_CheckIfICanEditADegree (struct Hie_Node *Deg);
static void Deg_PutFormToCreateDegree (const struct DegTyp_DegTypes *DegTypes);
static void Deg_PutHeadDegreesForSeeing (void);
static void Deg_PutHeadDegreesForEdition (void);

static void Deg_ListDegrees (void);
static void Deg_PutIconsListingDegrees (__attribute__((unused)) void *Args);
static void Deg_PutIconToEditDegrees (void);
static void Deg_ListOneDegreeForSeeing (struct Hie_Node *Deg,unsigned NumDeg);

static void Deg_EditDegreesInternal (void);
static void Deg_PutIconsEditingDegrees (__attribute__((unused)) void *Args);

static void Deg_ReceiveRequestOrCreateDeg (Hie_Status_t Status);

static void Deg_GetDegreeDataFromRow (MYSQL_RES *mysql_res,
                                      struct Hie_Node *Deg);

static void Deg_ShowAlertAndButtonToGoToDeg (void);

static void Deg_EditingDegreeConstructor (void);
static void Deg_EditingDegreeDestructor (void);

static void Deg_PutParDegCod (void *DegCod);

/*****************************************************************************/
/******************* List degrees with pending courses ***********************/
/*****************************************************************************/

void Deg_SeeDegWithPendingCrss (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Hlp_SYSTEM_Pending;
   extern const char *Txt_Degrees_with_pending_courses;
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Courses_ABBREVIATION;
   extern const char *Txt_There_are_no_degrees_with_requests_for_courses_to_be_confirmed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumDegs;
   unsigned NumDeg;
   struct Hie_Node Deg;
   const char *BgColor;

   /***** Get degrees with pending courses *****/
   if ((NumDegs = Deg_DB_GetDegsWithPendingCrss (&mysql_res)))
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin (Txt_Degrees_with_pending_courses,NULL,NULL,
                         Hlp_SYSTEM_Pending,Box_NOT_CLOSABLE,2);

	 /***** Write heading *****/
	 HTM_TR_Begin (NULL);
            HTM_TH (Txt_HIERARCHY_SINGUL_Abc[Hie_DEG],HTM_HEAD_LEFT );
            HTM_TH (Txt_Courses_ABBREVIATION  ,HTM_HEAD_RIGHT);
	 HTM_TR_End ();

	 /***** List the degrees *****/
	 for (NumDeg = 0, The_ResetRowColor ();
	      NumDeg < NumDegs;
	      NumDeg++, The_ChangeRowColor ())
	   {
	    /* Get next degree */
	    row = mysql_fetch_row (mysql_res);

	    /* Get degree code (row[0]) */
	    Deg.HieCod = Str_ConvertStrCodToLongCod (row[0]);
	    BgColor = (Deg.HieCod == Gbl.Hierarchy.Node[Hie_DEG].HieCod) ? "BG_HIGHLIGHT" :
								        The_GetColorRows ();

	    /* Get data of degree */
	    Hie_GetDataByCod[Hie_DEG] (&Deg);

	    /* Begin table row */
	    HTM_TR_Begin (NULL);

	       /* Degree logo and full name */
	       HTM_TD_Begin ("class=\"LM DAT_%s NOWRAP %s\"",
	                     The_GetSuffix (),BgColor);
		  Deg_DrawDegreeLogoAndNameWithLink (&Deg,ActSeeCrs,"CM ICO20x20");
	       HTM_TD_End ();

	       /* Number of pending courses (row[1]) */
	       HTM_TD_Begin ("class=\"RM DAT_%s %s\"",
	                     The_GetSuffix (),BgColor);
		  HTM_Txt (row[1]);
	       HTM_TD_End ();

	    /* End table row */
	    HTM_TR_End ();
	   }

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }
   else
      Ale_ShowAlert (Ale_INFO,Txt_There_are_no_degrees_with_requests_for_courses_to_be_confirmed);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Draw degree logo and name with link ********************/
/*****************************************************************************/

void Deg_DrawDegreeLogoAndNameWithLink (struct Hie_Node *Deg,Act_Action_t Action,
                                        const char *IconClass)
  {
   /***** Begin form *****/
   Frm_BeginFormGoTo (Action);
      ParCod_PutPar (ParCod_Deg,Deg->HieCod);

      /***** Link to action *****/
      HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Deg->FullName),
                               "class=\"BT_LINK LT\"");
      Str_FreeGoToTitle ();

	 /***** Degree logo and name *****/
	 Lgo_DrawLogo (Hie_DEG,Deg,IconClass);
	 HTM_NBSPTxt (Deg->FullName);

      /***** End link *****/
      HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************************** Write selector of degree ************************/
/*****************************************************************************/

void Deg_WriteSelectorOfDegree (void)
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumDegs;
   unsigned NumDeg;
   long DegCod;

   /***** Begin form *****/
   Frm_BeginFormGoTo (ActSeeCrs);

      /***** Begin selector of degree *****/
      HTM_SELECT_Begin ((Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0) ? HTM_SUBMIT_ON_CHANGE :
								   HTM_DISABLED,
			NULL,
			"id=\"deg\" name=\"deg\" class=\"HIE_SEL INPUT_%s\"",
			The_GetSuffix ());

	 /***** Initial disabled option *****/
	 HTM_OPTION (HTM_Type_STRING,"",
		     ((Gbl.Hierarchy.Node[Hie_DEG].HieCod <= 0) ? HTM_SELECTED :
							         HTM_NO_ATTR) | HTM_DISABLED,
		     "[%s]",Txt_HIERARCHY_SINGUL_Abc[Hie_DEG]);

	 if (Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0)
	   {
	    /***** Get degrees belonging to the current center from database *****/
	    NumDegs = Deg_DB_GetDegsOfCurrentCtrBasic (&mysql_res);

	    /***** Get degrees of this center *****/
	    for (NumDeg = 0;
		 NumDeg < NumDegs;
		 NumDeg++)
	      {
	       /* Get next degree */
	       row = mysql_fetch_row (mysql_res);

	       /* Get degree code (row[0]) */
	       if ((DegCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
		  Err_WrongDegreeExit ();

	       /* Write option */
	       HTM_OPTION (HTM_Type_LONG,&DegCod,
			   Gbl.Hierarchy.Node[Hie_DEG].HieCod > 0 &&
			   (DegCod == Gbl.Hierarchy.Node[Hie_DEG].HieCod) ? HTM_SELECTED :
									    HTM_NO_ATTR,
			   "%s",row[1]);
	      }

	    /***** Free structure that stores the query result *****/
	    DB_FreeMySQLResult (&mysql_res);
	   }

      /***** End selector of degree *****/
      HTM_SELECT_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/************* Show the degrees belonging to the current center **************/
/*****************************************************************************/

void Deg_ShowDegsOfCurrentCtr (void)
  {
   /***** Trivial check *****/
   if (Gbl.Hierarchy.Node[Hie_CTR].HieCod <= 0)	// No center selected
      return;

   /***** Get list of centers and degrees *****/
   Ctr_GetBasicListOfCenters (Gbl.Hierarchy.Node[Hie_INS].HieCod);
   Deg_GetListDegsInCurrentCtr ();

   /***** Write menu to select country, institution and center *****/
   Hie_WriteMenuHierarchy ();

   /***** Show list of degrees *****/
   Deg_ListDegrees ();

   /***** Free list of degrees and centers *****/
   Hie_FreeList (Hie_CTR);
   Hie_FreeList (Hie_INS);
  }

/*****************************************************************************/
/********************* List current degrees for edition **********************/
/*****************************************************************************/

static void Deg_ListDegreesForEdition (const struct DegTyp_DegTypes *DegTypes)
  {
   extern const char *Txt_DEGREE_STATUS[Hie_NUM_STATUS_TXT];
   static Act_Action_t ActionRename[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = ActRenDegSho,
      [Nam_FULL_NAME] = ActRenDegFul,
     };
   unsigned NumDeg;
   struct Hie_Node *Deg;
   unsigned NumDegTyp;
   struct DegTyp_DegType *DegTyp;
   char WWW[WWW_MAX_BYTES_WWW + 1];
   struct Usr_Data UsrDat;
   Usr_Can_t ICanEdit;
   unsigned NumCrss;
   unsigned NumUsrsInCrssOfDeg;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Begin table of degrees *****/
   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Write heading *****/
      Deg_PutHeadDegreesForEdition ();

      /***** List the degrees *****/
      for (NumDeg = 0;
	   NumDeg < Gbl.Hierarchy.List[Hie_CTR].Num;
	   NumDeg++)
	{
	 Deg = &(Gbl.Hierarchy.List[Hie_CTR].Lst[NumDeg]);

	 ICanEdit = Deg_CheckIfICanEditADegree (Deg);
	 NumCrss = Hie_GetNumNodesInHieLvl (Hie_CRS,	// Number of courses...
					    Hie_DEG,	// ...in degree
					    Deg->HieCod);
	 NumUsrsInCrssOfDeg = Enr_GetNumUsrsInCrss (Hie_DEG,Deg->HieCod,
						    1 << Rol_STD |
						    1 << Rol_NET |
						    1 << Rol_TCH);	// Any user

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove degree */
	    HTM_TD_Begin ("class=\"BT\"");
	       if (ICanEdit == Usr_CAN_NOT ||
		   NumCrss ||	// Degree has courses ==> deletion forbidden
		   NumUsrsInCrssOfDeg)
		  Ico_PutIconRemovalNotAllowed ();
	       else
		  Ico_PutContextualIconToRemove (ActRemDeg,NULL,
						 Hie_PutParOtherHieCod,&Deg->HieCod);
	    HTM_TD_End ();

	    /* Degree code */
	    HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
	       HTM_Long (Deg->HieCod);
	    HTM_TD_End ();

	    /* Degree logo */
	    HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Deg->FullName);
	       Lgo_DrawLogo (Hie_DEG,Deg,"ICO20x20");
	    HTM_TD_End ();

	    /* Degree short name and full name */
	    Names[Nam_SHRT_NAME] = Deg->ShrtName;
	    Names[Nam_FULL_NAME] = Deg->FullName;
	    Nam_ExistingShortAndFullNames (ActionRename,
				           ParCod_OthHie,Deg->HieCod,
				           Names,
				           ICanEdit == Usr_CAN ? Frm_PUT_FORM :
				        			   Frm_DONT_PUT_FORM);

	    /* Degree type */
	    HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
	       switch (ICanEdit)
		 {
		  case Usr_CAN:
		     Frm_BeginForm (ActChgDegTyp);
			ParCod_PutPar (ParCod_OthHie,Deg->HieCod);
			HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
					  "name=\"OthDegTypCod\""
					  " class=\"HIE_SEL_NARROW INPUT_%s\"",
					  The_GetSuffix ());
			   for (NumDegTyp = 0;
				NumDegTyp < DegTypes->Num;
				NumDegTyp++)
			     {
			      DegTyp = &DegTypes->Lst[NumDegTyp];
			      HTM_OPTION (HTM_Type_LONG,&DegTyp->DegTypCod,
					  (DegTyp->DegTypCod == Deg->Specific.TypCod) ? HTM_SELECTED :
										        HTM_NO_ATTR,
					  "%s",DegTyp->DegTypName);
			     }
			HTM_SELECT_End ();
		     Frm_EndForm ();
		     break;
		  case Usr_CAN_NOT:
		  default:
		     for (NumDegTyp = 0;
			  NumDegTyp < DegTypes->Num;
			  NumDegTyp++)
		       {
			DegTyp = &DegTypes->Lst[NumDegTyp];
			if (DegTyp->DegTypCod == Deg->Specific.TypCod)
			   HTM_Txt (DegTyp->DegTypName);
		       }
		     break;
		 }
	    HTM_TD_End ();

	    /* Degree WWW */
	    HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
	       switch (ICanEdit)
		 {
		  case Usr_CAN:
		     Frm_BeginForm (ActChgDegWWW);
			ParCod_PutPar (ParCod_OthHie,Deg->HieCod);
			HTM_INPUT_URL ("WWW",Deg->WWW,
				       HTM_REQUIRED | HTM_SUBMIT_ON_CHANGE,
				       "class=\"INPUT_WWW INPUT_%s\"",
				       The_GetSuffix ());
		     Frm_EndForm ();
		     break;
		  case Usr_CAN_NOT:
		  default:
		     Str_Copy (WWW,Deg->WWW,sizeof (WWW) - 1);
		     HTM_DIV_Begin ("class=\"EXTERNAL_WWW_SHRT\"");
			HTM_A_Begin ("href=\"%s\" target=\"_blank\" title=\"%s\""
				     " class=\"DAT_%s\"",
				     Deg->WWW,
				     Deg->WWW,
				     The_GetSuffix ());
			   HTM_Txt (WWW);
			HTM_A_End ();
		     HTM_DIV_End ();
		     break;
		 }
	    HTM_TD_End ();

	    /* Number of courses in this degree */
	    HTM_TD_Unsigned (NumCrss);

	    /* Number of users in courses of this degree */
	    HTM_TD_Unsigned (NumUsrsInCrssOfDeg);

	    /* Degree requester */
	    UsrDat.UsrCod = Deg->RequesterUsrCod;
	    Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
						     Usr_DONT_GET_PREFS,
						     Usr_DONT_GET_ROLE_IN_CRS);
	    HTM_TD_Begin ("class=\"LT DAT_%s INPUT_REQUESTER\"",
	                  The_GetSuffix ());
	       Usr_WriteAuthor (&UsrDat,For_ENABLED);
	    HTM_TD_End ();

	    /* Degree status */
	    Hie_WriteStatusCellEditable (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM ? Usr_CAN :
										  Usr_CAN_NOT,
	                                 Deg->Status,ActChgDegSta,Deg->HieCod,
	                                 Txt_DEGREE_STATUS);

	 HTM_TR_End ();
	}

   /***** End table of degrees *****/
   HTM_TABLE_End ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************** Check if I can edit, remove, etc. a degree *******************/
/*****************************************************************************/

static Usr_Can_t Deg_CheckIfICanEditADegree (struct Hie_Node *Deg)
  {
   return (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM ||		// I am a center administrator or higher
           ((Deg->Status & Hie_STATUS_BIT_PENDING) &&	// Degree is not yet activated
           Gbl.Usrs.Me.UsrDat.UsrCod == Deg->RequesterUsrCod)) ? Usr_CAN :	// I am the requester
        							 Usr_CAN_NOT;
  }

/*****************************************************************************/
/*********************** Put a form to create a new degree *******************/
/*****************************************************************************/

static void Deg_PutFormToCreateDegree (const struct DegTyp_DegTypes *DegTypes)
  {
   Act_Action_t NextAction = ActUnk;
   unsigned NumDegTyp;
   struct DegTyp_DegType *DegTyp;
   struct Hie_Node Deg;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Set action depending on role *****/
   if (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
      NextAction = ActNewDeg;
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      NextAction = ActReqDeg;
   else
      Err_NoPermissionExit ();

   /***** Begin form to create *****/
   Frm_BeginFormTable (NextAction,NULL,NULL,NULL,"TBL_SCROLL");

      /***** Write heading *****/
      Deg_PutHeadDegreesForEdition ();

      HTM_TR_Begin (NULL);

	 /***** Column to remove degree, disabled here *****/
	 HTM_TD_Begin ("class=\"BM\"");
	 HTM_TD_End ();

	 /***** Degree code *****/
	 HTM_TD_Begin ("class=\"CODE\"");
	 HTM_TD_End ();

	 /***** Degree logo *****/
	 HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Deg_EditingDeg->FullName);
	    Deg.HieCod = -1L;
	    Deg.ShrtName[0] = '\0';
	    Lgo_DrawLogo (Hie_DEG,&Deg,"ICO20x20");
	 HTM_TD_End ();

	 /***** Degree short name and full name *****/
	 Names[Nam_SHRT_NAME] = Deg_EditingDeg->ShrtName;
	 Names[Nam_FULL_NAME] = Deg_EditingDeg->FullName;
	 Nam_NewShortAndFullNames (Names);

	 /***** Degree type *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
			      "name=\"OthDegTypCod\""
			      " class=\"HIE_SEL_NARROW INPUT_%s\"",
			      The_GetSuffix ());
	       for (NumDegTyp = 0;
		    NumDegTyp < DegTypes->Num;
		    NumDegTyp++)
		 {
		  DegTyp = &DegTypes->Lst[NumDegTyp];
		  HTM_OPTION (HTM_Type_LONG,&DegTyp->DegTypCod,
			      (DegTyp->DegTypCod == Deg_EditingDeg->Specific.TypCod) ? HTM_SELECTED :
										       HTM_NO_ATTR,
			      "%s",DegTyp->DegTypName);
		 }
	    HTM_SELECT_End ();
	 HTM_TD_End ();

	 /***** Degree WWW *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_URL ("WWW",Deg_EditingDeg->WWW,
			   HTM_REQUIRED,
			   "class=\"INPUT_WWW INPUT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Number of courses in this degree *****/
	 HTM_TD_Unsigned (0);

	 /***** Number of users in courses of this degree *****/
	 HTM_TD_Unsigned (0);

	 /***** Degree requester *****/
	 HTM_TD_Begin ("class=\"LT DAT_%s INPUT_REQUESTER\"",The_GetSuffix ());
	    Usr_WriteAuthor (&Gbl.Usrs.Me.UsrDat,For_ENABLED);
	 HTM_TD_End ();

	 /***** Degree status *****/
	 HTM_TD_Empty (1);

      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE);
  }

/*****************************************************************************/
/******************** Write header with fields of a degree *******************/
/*****************************************************************************/

static void Deg_PutHeadDegreesForSeeing (void)
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Type;
   extern const char *Txt_Courses_ABBREVIATION;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];

   HTM_TR_Begin (NULL);

      HTM_TH_Span (NULL                 ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH_Empty (1);
      HTM_TH (Txt_HIERARCHY_SINGUL_Abc[Hie_DEG],HTM_HEAD_LEFT  );
      HTM_TH (Txt_Type                  ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Courses_ABBREVIATION  ,HTM_HEAD_RIGHT );
      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 HTM_TxtF ("%s+",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
	 HTM_BR ();
	 HTM_Txt (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
      HTM_TH_End ();
      HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Write header with fields of a degree *******************/
/*****************************************************************************/

static void Deg_PutHeadDegreesForEdition (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Short_name_of_the_degree;
   extern const char *Txt_Full_name_of_the_degree;
   extern const char *Txt_Type;
   extern const char *Txt_WWW;
   extern const char *Txt_Courses_ABBREVIATION;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Requester;

   HTM_TR_Begin (NULL);

      HTM_TH_Span (NULL                   ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH (Txt_Code                    ,HTM_HEAD_RIGHT);
      HTM_TH_Empty (1);
      HTM_TH (Txt_Short_name_of_the_degree,HTM_HEAD_LEFT );
      HTM_TH (Txt_Full_name_of_the_degree ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Type                    ,HTM_HEAD_LEFT );
      HTM_TH (Txt_WWW                     ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Courses_ABBREVIATION    ,HTM_HEAD_RIGHT);
      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 HTM_TxtF ("%s+",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
	 HTM_BR ();
	 HTM_Txt (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
      HTM_TH_End ();
      HTM_TH (Txt_Requester               ,HTM_HEAD_LEFT );
      HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Convert string to year of a degree *********************/
/*****************************************************************************/

unsigned Deg_ConvStrToYear (const char *StrYear)
  {
   int Year;

   if (sscanf (StrYear,"%d",&Year) != 1)
      return 0;
   if (Year < 0)
      return 0;
   if (Year > Deg_MAX_YEARS_PER_DEGREE)
      return Deg_MAX_YEARS_PER_DEGREE;
   return (unsigned) Year;
  }

/*****************************************************************************/
/************** List degrees belonging to the current center *****************/
/*****************************************************************************/

static void Deg_ListDegrees (void)
  {
   extern const char *Hlp_CENTER_Degrees;
   extern const char *Txt_Degrees_of_CENTER_X;
   extern const char *Txt_No_degrees;
   char *Title;
   unsigned NumDeg;

   /***** Begin box *****/
   if (asprintf (&Title,Txt_Degrees_of_CENTER_X,
		 Gbl.Hierarchy.Node[Hie_CTR].ShrtName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (Title,Deg_PutIconsListingDegrees,NULL,
                 Hlp_CENTER_Degrees,Box_NOT_CLOSABLE);
   free (Title);

      if (Gbl.Hierarchy.List[Hie_CTR].Num)	// There are degrees in the current center
	{
	 /***** Begin table *****/
	 HTM_TABLE_Begin ("TBL_SCROLL");

	    /***** Heading *****/
	    Deg_PutHeadDegreesForSeeing ();

	    /***** List the degrees *****/
	    for (NumDeg = 0;
		 NumDeg < Gbl.Hierarchy.List[Hie_CTR].Num;
		 NumDeg++)
	       Deg_ListOneDegreeForSeeing (&(Gbl.Hierarchy.List[Hie_CTR].Lst[NumDeg]),
					   NumDeg + 1);

	 /***** End table *****/
	 HTM_TABLE_End ();
	}
      else	// No degrees created in the current center
	 Ale_ShowAlert (Ale_INFO,Txt_No_degrees);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/***************** Put contextual icons in list of degrees *******************/
/*****************************************************************************/

static void Deg_PutIconsListingDegrees (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit degrees *****/
   if (Hie_CheckIfICanEdit () == Usr_CAN)
      Deg_PutIconToEditDegrees ();

   /***** Put icon to view degree types *****/
   DegTyp_PutIconToViewDegTypes ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/********************** Put link (form) to edit degrees **********************/
/*****************************************************************************/

static void Deg_PutIconToEditDegrees (void)
  {
   Ico_PutContextualIconToEdit (ActEdiDeg,NULL,NULL,NULL);
  }

/*****************************************************************************/
/************************ List one degree for seeing *************************/
/*****************************************************************************/

static void Deg_ListOneDegreeForSeeing (struct Hie_Node *Deg,unsigned NumDeg)
  {
   extern const char *Txt_DEGREE_With_courses;
   extern const char *Txt_DEGREE_Without_courses;
   extern const char *Txt_DEGREE_STATUS[Hie_NUM_STATUS_TXT];
   struct DegTyp_DegType DegTyp;
   const char *TxtClassNormal;
   const char *TxtClassStrong;
   const char *BgColor;
   unsigned NumCrss = Hie_GetCachedNumNodesInHieLvl (Hie_CRS,	// Number of courses...
					             Hie_DEG,	// ...in degree
					             Deg->HieCod);

   /***** Get data of type of degree of this degree *****/
   DegTyp.DegTypCod = Deg->Specific.TypCod;
   if (!DegTyp_GetDegTypeDataByCod (&DegTyp))
      Err_WrongDegTypExit ();

   if (Deg->Status & Hie_STATUS_BIT_PENDING)
     {
      TxtClassNormal =
      TxtClassStrong = "DAT_LIGHT";
     }
   else
     {
      TxtClassNormal = "DAT";
      TxtClassStrong = "DAT_STRONG";
     }
   BgColor = (Deg->HieCod == Gbl.Hierarchy.Node[Hie_DEG].HieCod) ? "BG_HIGHLIGHT" :
							        The_GetColorRows ();

   /***** Begin table row *****/
   HTM_TR_Begin (NULL);

      /***** Put tip if degree has courses *****/
      HTM_TD_Begin ("class=\"CT %s_%s %s\" title=\"%s\"",
		    TxtClassNormal,The_GetSuffix (),BgColor,
		    NumCrss ? Txt_DEGREE_With_courses :
			      Txt_DEGREE_Without_courses);
	 if (NumCrss)
	    HTM_Txt ("&check;");
	 else
	    HTM_NBSP ();
      HTM_TD_End ();

      /***** Number of degree in this list *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (NumDeg);
      HTM_TD_End ();

      /***** Degree logo and name *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",
                    TxtClassStrong,The_GetSuffix (),BgColor);
	 Deg_DrawDegreeLogoAndNameWithLink (Deg,ActSeeCrs,"CM ICO20x20");
      HTM_TD_End ();

      /***** Type of degree *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Txt (DegTyp.DegTypName);
      HTM_TD_End ();

      /***** Current number of courses in this degree *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (NumCrss);
      HTM_TD_End ();

      /***** Number of users in courses of this degree *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Enr_GetCachedNumUsrsInCrss (Hie_DEG,Deg->HieCod,
						   1 << Rol_STD |
						   1 << Rol_NET |
						   1 << Rol_TCH));	// Any user
      HTM_TD_End ();

      /***** Degree status *****/
      Hie_WriteStatusCell (Deg->Status,TxtClassNormal,BgColor,Txt_DEGREE_STATUS);

   /***** End table row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*************************** Put forms to edit degrees ***********************/
/*****************************************************************************/

void Deg_EditDegrees (void)
  {
   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Edit degrees *****/
   Deg_EditDegreesInternal ();

   /***** Degree destructor *****/
   Deg_EditingDegreeDestructor ();
  }

static void Deg_EditDegreesInternal (void)
  {
   extern const char *Hlp_CENTER_Degrees;
   extern const char *Txt_Degrees_of_CENTER_X;
   extern const char *Txt_No_types_of_degree;
   struct DegTyp_DegTypes DegTypes;
   char *Title;

   /***** Get list of degree types *****/
   DegTyp_GetListDegTypes (&DegTypes,Hie_SYS,DegTyp_ORDER_BY_DEG_TYPE);

   /***** Get list of degrees in the current center *****/
   Deg_GetListDegsInCurrentCtr ();

   /***** Write menu to select country, institution and center *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   if (asprintf (&Title,Txt_Degrees_of_CENTER_X,
		 Gbl.Hierarchy.Node[Hie_CTR].ShrtName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (Title,Deg_PutIconsEditingDegrees,NULL,
                 Hlp_CENTER_Degrees,Box_NOT_CLOSABLE);
   free (Title);

      if (Deg_DB_GetNumDegreeTypes (Hie_SYS))
	{
	 /***** Put a form to create a new degree *****/
	 Deg_PutFormToCreateDegree (&DegTypes);

	 /***** Forms to edit current degrees *****/
	 if (Gbl.Hierarchy.List[Hie_CTR].Num)
	    Deg_ListDegreesForEdition (&DegTypes);
	}
      else	// No degree types
	{
	 /***** Warning message *****/
	 Ale_ShowAlert (Ale_WARNING,Txt_No_types_of_degree);

	 /***** Form to create the first degree type *****/
	 if (DegTyp_CheckIfICanCreateDegTypes () == Usr_CAN)
	    DegTyp_EditDegTypes (&DegTypes);
	}

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of degree types *****/
   DegTyp_FreeListDegTypes (&DegTypes);
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of degrees *****************/
/*****************************************************************************/

static void Deg_PutIconsEditingDegrees (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view degrees *****/
   Ico_PutContextualIconToView (ActSeeDeg,NULL,NULL,NULL);

   /***** Put icon to view types of degree *****/
   DegTyp_PutIconToViewDegTypes ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/*********************** Put icon to view degrees ****************************/
/*****************************************************************************/

void Deg_PutIconToViewDegrees (void)
  {
   Lay_PutContextualLinkOnlyIcon (ActSeeDeg,NULL,NULL,NULL,
				  "graduation-cap.svg",Ico_BLACK);
  }

/*****************************************************************************/
/************ Create a list with all degrees that have students **************/
/*****************************************************************************/

void Deg_GetListAllDegsWithStds (struct Hie_List *Degs)
  {
   MYSQL_RES *mysql_res;
   unsigned NumDeg;

   /***** Get degrees admin by me from database *****/
   if ((Degs->Num = Deg_DB_GetDegsWithStds (&mysql_res))) // Degrees found...
     {
      /***** Create list with degrees *****/
      if ((Degs->Lst = calloc (Degs->Num,sizeof (*Degs->Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the degrees *****/
      for (NumDeg = 0;
	   NumDeg < Degs->Num;
	   NumDeg++)
         Deg_GetDegreeDataFromRow (mysql_res,&Degs->Lst[NumDeg]);
     }
   else
      Degs->Lst = NULL;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Get a list with the degrees of the current center **************/
/*****************************************************************************/

void Deg_GetListDegsInCurrentCtr (void)
  {
   MYSQL_RES *mysql_res;
   unsigned NumDeg;

   /***** Get degrees of the current center from database *****/
   Gbl.Hierarchy.List[Hie_CTR].Num = Deg_DB_GetDegsOfCurrentCtrFull (&mysql_res);

   /***** Count number of rows in result *****/
   if (Gbl.Hierarchy.List[Hie_CTR].Num) // Degrees found...
     {
      /***** Create list with degrees of this center *****/
      if ((Gbl.Hierarchy.List[Hie_CTR].Lst = calloc ((size_t) Gbl.Hierarchy.List[Hie_CTR].Num,
                                                        sizeof (*Gbl.Hierarchy.List[Hie_CTR].Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the degrees of this center *****/
      for (NumDeg = 0;
	   NumDeg < Gbl.Hierarchy.List[Hie_CTR].Num;
	   NumDeg++)
         Deg_GetDegreeDataFromRow (mysql_res,&Gbl.Hierarchy.List[Hie_CTR].Lst[NumDeg]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************************** Free list of degrees ****************************/
/*****************************************************************************/

void Deg_FreeListAllDegsWithStds (struct Hie_List *Degs)
  {
   if (Degs->Lst)
     {
      free (Degs->Lst);
      Degs->Lst = NULL;
      Degs->Num = 0;
     }
  }

/*****************************************************************************/
/****************** Receive form to request a new degree *********************/
/*****************************************************************************/

void Deg_ReceiveReqDeg (void)
  {
   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Receive form to request a new degree *****/
   Deg_ReceiveRequestOrCreateDeg ((Hie_Status_t) Hie_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/******************* Receive form to create a new degree *********************/
/*****************************************************************************/

void Deg_ReceiveNewDeg (void)
  {
   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Receive form to create a new degree *****/
   Deg_ReceiveRequestOrCreateDeg ((Hie_Status_t) 0);
  }

/*****************************************************************************/
/******************* Receive form to create a new degree *********************/
/*****************************************************************************/

static void Deg_ReceiveRequestOrCreateDeg (Hie_Status_t Status)
  {
   extern const char *Txt_Created_new_degree_X;
   char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Get parameters from form *****/
   /* Set degree center */
   Deg_EditingDeg->PrtCod = Gbl.Hierarchy.Node[Hie_CTR].HieCod;

   /* Get degree short name and full name */
   Names[Nam_SHRT_NAME] = Deg_EditingDeg->ShrtName;
   Names[Nam_FULL_NAME] = Deg_EditingDeg->FullName;
   Nam_GetParsShrtAndFullName (Names);

   /* Get degree type */
   Deg_EditingDeg->Specific.TypCod = ParCod_GetAndCheckPar (ParCod_OthDegTyp);

   /* Get degree WWW */
   Par_GetParText ("WWW",Deg_EditingDeg->WWW,WWW_MAX_BYTES_WWW);

   if (Deg_EditingDeg->ShrtName[0] &&
       Deg_EditingDeg->FullName[0])	// If there's a degree name
     {
      if (Deg_EditingDeg->WWW[0])
	{
	 /***** If name of degree was not in database... *****/
	 if (!Nam_CheckIfNameExists (Deg_DB_CheckIfDegNameExistsInCtr,
				     (const char **) Names,
				     -1L,
				     Deg_EditingDeg->PrtCod,
				     0))	// Unused
	   {
	    Deg_DB_CreateDegree (Deg_EditingDeg,Status);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_Created_new_degree_X,
			     Names[Nam_FULL_NAME]);
	   }
	}
      else	// If there is not a degree logo or web
         Ale_CreateAlertYouMustSpecifyTheWebAddress ();
     }
   else	// If there is not a degree name
      Ale_CreateAlertYouMustSpecifyShrtNameAndFullName ();
  }

/*****************************************************************************/
/************************ Request removing of a degree ***********************/
/*****************************************************************************/

void Deg_RemoveDeg (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_To_remove_a_degree_you_must_first_remove_all_courses_in_the_degree;
   extern const char *Txt_Degree_X_removed;

   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Get degree code *****/
   Deg_EditingDeg->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /***** Get data of degree *****/
   Hie_GetDataByCod[Hie_DEG] (Deg_EditingDeg);

   /***** Check if this degree has courses *****/
   if (Hie_GetNumNodesInHieLvl (Hie_CRS,	// Number of courses...
				Hie_DEG,	// ...in degree
				Deg_EditingDeg->HieCod))	// Degree has courses ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_a_degree_you_must_first_remove_all_courses_in_the_degree);
   else	// Degree has no courses ==> remove it
     {
      /***** Remove degree *****/
      Deg_RemoveDegreeCompletely (Deg_EditingDeg->HieCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
		       Txt_Degree_X_removed,
                       Deg_EditingDeg->FullName);

      Deg_EditingDeg->HieCod = -1L;	// To not showing button to go to degree
     }
  }

/*****************************************************************************/
/********************* Get data of a degree from its code ********************/
/*****************************************************************************/
// Returns true if degree found

bool Deg_GetDegreeDataByCod (struct Hie_Node *Node)
  {
   MYSQL_RES *mysql_res;
   bool Found = false;

   /***** Clear data *****/
   Node->PrtCod          = -1L;
   Node->Specific.TypCod = -1L;
   Node->Status          = (Hie_Status_t) 0;
   Node->RequesterUsrCod = -1L;
   Node->ShrtName[0]     = '\0';
   Node->FullName[0]     = '\0';
   Node->WWW[0]          = '\0';

   /***** Check if degree code is correct *****/
   if (Node->HieCod > 0)
     {
      /***** Get data of a degree from database *****/
      if (Deg_DB_GetDegreeDataByCod (&mysql_res,Node->HieCod)) // Degree found...
	{
	 /***** Get data of degree *****/
	 Deg_GetDegreeDataFromRow (mysql_res,Node);

         /* Set return value */
	 Found = true;
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return Found;
  }

/*****************************************************************************/
/********** Get data of a degree from a row resulting of a query *************/
/*****************************************************************************/

static void Deg_GetDegreeDataFromRow (MYSQL_RES *mysql_res,
                                      struct Hie_Node *Deg)
  {
   MYSQL_ROW row;

   /**** Get next row from result ****/
   row = mysql_fetch_row (mysql_res);

   /***** Get degree code (row[0]) *****/
   if ((Deg->HieCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongDegreeExit ();

   /***** Get center code (row[1]) and code of the degree type (row[2]) *****/
   Deg->PrtCod          = Str_ConvertStrCodToLongCod (row[1]);
   Deg->Specific.TypCod = Str_ConvertStrCodToLongCod (row[2]);

   /* Get course status (row[3]) */
   if (sscanf (row[3],"%u",&(Deg->Status)) != 1)
      Err_WrongStatusExit ();

   /* Get requester user's code (row[4]) */
   Deg->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[4]);

   /***** Get degree short name (row[5]), full name (row[6]) and WWW (row[7]) *****/
   Str_Copy (Deg->ShrtName,row[5],sizeof (Deg->ShrtName) - 1);
   Str_Copy (Deg->FullName,row[6],sizeof (Deg->FullName) - 1);
   Str_Copy (Deg->WWW     ,row[7],sizeof (Deg->WWW     ) - 1);
  }

/*****************************************************************************/
/***************************** Remove a degree *******************************/
/*****************************************************************************/

void Deg_RemoveDegreeCompletely (long DegCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumCrss;
   unsigned NumCrs;
   long CrsCod;
   char PathDeg[PATH_MAX + 1];

   /***** Get courses of a degree and remove them *****/
   NumCrss = Crs_DB_GetCrssInDeg (&mysql_res,DegCod);
   for (NumCrs = 0;
	NumCrs < NumCrss;
	NumCrs++)
     {
      /* Get next course */
      if ((CrsCod = DB_GetNextCode (mysql_res)) < 0)
         Err_WrongCourseExit ();

      /* Remove course */
      Crs_RemoveCourseCompletely (CrsCod);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Remove all threads and posts in forums of the degree *****/
   For_DB_RemoveForums (Hie_DEG,DegCod);

   /***** Remove surveys of the degree *****/
   Svy_RemoveSurveys (Hie_DEG,DegCod);

   /***** Remove information related to files in degree *****/
   Brw_DB_RemoveDegFiles (DegCod);

   /***** Remove directories of the degree *****/
   snprintf (PathDeg,sizeof (PathDeg),"%s/%02u/%u",
	     Cfg_PATH_DEG_PUBLIC,
	     (unsigned) (DegCod % 100),
	     (unsigned)  DegCod);
   Fil_RemoveTree (PathDeg);

   /***** Remove administrators of this degree *****/
   Adm_DB_RemAdmins (Hie_DEG,DegCod);

   /***** Remove the degree *****/
   Deg_DB_RemoveDeg (DegCod);

   /***** Flush caches *****/
   Hie_FlushCachedNumNodesInHieLvl (Hie_CRS,Hie_DEG);	// Number of courses in degree

   /***** Delete all degrees in stats table not present in degrees table *****/
   Pho_DB_RemoveObsoleteStatDegrees ();
  }

/*****************************************************************************/
/************************ Change the name of a degree ************************/
/*****************************************************************************/

void Deg_RenameDegShrt (void)
  {
   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Rename degree *****/
   Deg_EditingDeg->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);
   Deg_RenameDegree (Deg_EditingDeg,Nam_SHRT_NAME);
  }

void Deg_RenameDegFull (void)
  {
   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Rename degree *****/
   Deg_EditingDeg->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);
   Deg_RenameDegree (Deg_EditingDeg,Nam_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a degree ************************/
/*****************************************************************************/

void Deg_RenameDegree (struct Hie_Node *Deg,Nam_ShrtOrFullName_t ShrtOrFull)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Nam_Fields[Nam_NUM_SHRT_FULL_NAMES];
   extern unsigned Nam_MaxBytes[Nam_NUM_SHRT_FULL_NAMES];
   extern const char *Txt_X_already_exists;
   extern const char *Txt_The_degree_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   char *CurrentName[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = Deg->ShrtName,
      [Nam_FULL_NAME] = Deg->FullName,
     };
   char NewName[Nam_MAX_BYTES_FULL_NAME + 1];

   /***** Get parameters from form *****/
   /* Get the new name for the degree */
   Nam_GetParShrtOrFullName (ShrtOrFull,NewName);

   /***** Get data of degree *****/
   Hie_GetDataByCod[Hie_DEG] (Deg);

   /***** Check if new name is empty *****/
   if (NewName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentName[ShrtOrFull],NewName))	// Different names
        {
         /***** If degree was in database... *****/
         if (Deg_DB_CheckIfDegNameExistsInCtr (Nam_Fields[ShrtOrFull],
					       NewName,Deg->HieCod,Deg->PrtCod,
					       0))	// Unused
            Ale_CreateAlert (Ale_WARNING,NULL,Txt_X_already_exists,NewName);
         else
           {
            /* Update the table changing old name by new name */
            Deg_DB_UpdateDegNameDB (Deg->HieCod,
        			    Nam_Fields[ShrtOrFull],NewName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_degree_X_has_been_renamed_as_Y,
                             CurrentName[ShrtOrFull],NewName);

	    /* Change current degree name in order to display it properly */
	    Str_Copy (CurrentName[ShrtOrFull],NewName,Nam_MaxBytes[ShrtOrFull]);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_X_has_not_changed,
                          CurrentName[ShrtOrFull]);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
  }

/*****************************************************************************/
/************************ Change the type of a degree ************************/
/*****************************************************************************/

void Deg_ChangeDegTyp (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_type_of_degree_of_the_degree_X_has_changed;
   long NewDegTypCod;

   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Get parameters from form *****/
   /* Get degree code */
   Deg_EditingDeg->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /* Get the new degree type */
   NewDegTypCod = ParCod_GetAndCheckPar (ParCod_OthDegTyp);

   /***** Get data of degree *****/
   Hie_GetDataByCod[Hie_DEG] (Deg_EditingDeg);

   /***** Update the table of degrees changing old type by new type *****/
   Deg_DB_UpdateDegTyp (Deg_EditingDeg->HieCod,NewDegTypCod);
   Deg_EditingDeg->Specific.TypCod = NewDegTypCod;

   /***** Create alert to show the change made
          and put button to go to degree changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_type_of_degree_of_the_degree_X_has_changed,
	            Deg_EditingDeg->FullName);
  }

/*****************************************************************************/
/************************* Change the WWW of a degree ************************/
/*****************************************************************************/

void Deg_ChangeDegWWW (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[WWW_MAX_BYTES_WWW + 1];

   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the degree */
   Deg_EditingDeg->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /* Get the new WWW for the degree */
   Par_GetParText ("WWW",NewWWW,WWW_MAX_BYTES_WWW);

   /***** Get data of degree *****/
   Hie_GetDataByCod[Hie_DEG] (Deg_EditingDeg);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update the table changing old WWW by new WWW *****/
      Deg_DB_UpdateDegWWW (Deg_EditingDeg->HieCod,NewWWW);
      Str_Copy (Deg_EditingDeg->WWW,NewWWW,sizeof (Deg_EditingDeg->WWW) - 1);

      /***** Write alert to show the change made
	     and put button to go to degree changed *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_web_address_is_X,
		       NewWWW);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
  }

/*****************************************************************************/
/*********************** Change the status of a degree ***********************/
/*****************************************************************************/

void Deg_ChangeDegStatus (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_status_of_the_degree_X_has_changed;
   Hie_Status_t Status;

   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Get parameters from form *****/
   /* Get degree code */
   Deg_EditingDeg->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /* Get parameter with status */
   Status = Hie_GetParStatus ();	// New status

   /***** Get data of degree *****/
   Hie_GetDataByCod[Hie_DEG] (Deg_EditingDeg);

   /***** Update status *****/
   Deg_DB_UpdateDegStatus (Deg_EditingDeg->HieCod,Status);
   Deg_EditingDeg->Status = Status;

   /***** Write alert to show the change made
	  and put button to go to degree changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_status_of_the_degree_X_has_changed,
                    Deg_EditingDeg->ShrtName);
  }

/*****************************************************************************/
/********* Show alerts after changing a degree and continue editing **********/
/*****************************************************************************/

void Deg_ContEditAfterChgDeg (void)
  {
   /***** Write message to show the change made
	  and put button to go to degree changed *****/
   Deg_ShowAlertAndButtonToGoToDeg ();

   /***** Show the form again *****/
   Deg_EditDegreesInternal ();

   /***** Degree destructor *****/
   Deg_EditingDegreeDestructor ();
  }

/*****************************************************************************/
/***************** Write message to show the change made  ********************/
/***************** and put button to go to degree changed ********************/
/*****************************************************************************/

static void Deg_ShowAlertAndButtonToGoToDeg (void)
  {
   // If the degree being edited is different to the current one...
   if (Deg_EditingDeg->HieCod != Gbl.Hierarchy.Node[Hie_DEG].HieCod)
      /***** Alert with button to go to degree *****/
      Ale_ShowLastAlertAndButtonGo (ActSeeCrs,
                                    Deg_PutParDegCod,&Deg_EditingDeg->HieCod,
				    Deg_EditingDeg->ShrtName);
   else
      /***** Alert *****/
      Ale_ShowAlerts (NULL);
  }

/*****************************************************************************/
/********************* Get number of degrees with users **********************/
/*****************************************************************************/

unsigned Deg_GetCachedNumDegsWithUsrs (Hie_Level_t HieLvl,Rol_Role_t Role)
  {
   static FigCch_FigureCached_t FigureDegs[Rol_NUM_ROLES] =
     {
      [Rol_STD] = FigCch_NUM_DEGS_WITH_STDS,	// Students
      [Rol_NET] = FigCch_NUM_DEGS_WITH_NETS,	// Non-editing teachers
      [Rol_TCH] = FigCch_NUM_DEGS_WITH_TCHS,	// Teachers
     };
   unsigned NumNodesWithUsrs;
   long HieCod = Hie_GetHieCod (HieLvl);

   /***** Get number of degrees with users from cache *****/
   if (!FigCch_GetFigureFromCache (FigureDegs[Role],HieLvl,HieCod,
				   FigCch_UNSIGNED,&NumNodesWithUsrs))
     {
      /***** Get current number of degrees with users from database and update cache *****/
      NumNodesWithUsrs = Deg_DB_GetNumDegsWithUsrs (HieLvl,HieCod,Role);
      FigCch_UpdateFigureIntoCache (FigureDegs[Role],HieLvl,HieCod,
				    FigCch_UNSIGNED,&NumNodesWithUsrs);
     }

   return NumNodesWithUsrs;
  }

/*****************************************************************************/
/**************************** List degrees found *****************************/
/*****************************************************************************/

void Deg_ListDegsFound (MYSQL_RES **mysql_res,unsigned NumDegs)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_HIERARCHY_SINGUL_abc[Hie_NUM_LEVELS];
   extern const char *Txt_HIERARCHY_PLURAL_abc[Hie_NUM_LEVELS];
   char *Title;
   unsigned NumDeg;
   struct Hie_Node Deg;

   /***** Query database *****/
   if (NumDegs)
     {
      /***** Begin box and table *****/
      /* Number of degrees found */
      if (asprintf (&Title,"%u %s",NumDegs,
				   NumDegs == 1 ? Txt_HIERARCHY_SINGUL_abc[Hie_DEG] :
						  Txt_HIERARCHY_PLURAL_abc[Hie_DEG]) < 0)
	 Err_NotEnoughMemoryExit ();
      Box_BoxTableBegin (Title,NULL,NULL,NULL,Box_NOT_CLOSABLE,2);
      free (Title);

	 /***** Write heading *****/
	 Deg_PutHeadDegreesForSeeing ();

	 /***** List the degrees (one row per degree) *****/
	 for (NumDeg  = 1, The_ResetRowColor ();
	      NumDeg <= NumDegs;
	      NumDeg++, The_ChangeRowColor ())
	   {
	    /* Get next degree */
	    Deg.HieCod = DB_GetNextCode (*mysql_res);

	    /* Get data of degree */
	    Hie_GetDataByCod[Hie_DEG] (&Deg);

	    /* Write data of this degree */
	    Deg_ListOneDegreeForSeeing (&Deg,NumDeg);
	   }

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************************ Degree constructor/destructor **********************/
/*****************************************************************************/

static void Deg_EditingDegreeConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Deg_EditingDeg != NULL)
      Err_WrongDegreeExit ();

   /***** Allocate memory for degree *****/
   if ((Deg_EditingDeg = malloc (sizeof (*Deg_EditingDeg))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Reset degree *****/
   Deg_EditingDeg->HieCod             = -1L;
   Deg_EditingDeg->PrtCod          = -1L;
   Deg_EditingDeg->Specific.TypCod = -1L;
   Deg_EditingDeg->Status          = (Hie_Status_t) 0;
   Deg_EditingDeg->RequesterUsrCod = -1L;
   Deg_EditingDeg->ShrtName[0]     = '\0';
   Deg_EditingDeg->FullName[0]     = '\0';
   Deg_EditingDeg->WWW[0]          = '\0';
  }

static void Deg_EditingDegreeDestructor (void)
  {
   /***** Free memory used for degree *****/
   if (Deg_EditingDeg != NULL)
     {
      free (Deg_EditingDeg);
      Deg_EditingDeg = NULL;
     }
  }

/*****************************************************************************/
/********* Get the degree in which a user is enroled in more courses *********/
/*****************************************************************************/

void Deg_GetUsrMainDeg (long UsrCod,
		        char ShrtName[Nam_MAX_BYTES_SHRT_NAME + 1],
		        Rol_Role_t *MaxRole)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get the degree in which a user is enroled in more courses *****/
   if (Deg_DB_GetUsrMainDeg (&mysql_res,UsrCod))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get degree name (row[0]) */
      Str_Copy (ShrtName,row[0],Nam_MAX_BYTES_SHRT_NAME);

      /* Get maximum role (row[1]) */
      *MaxRole = Rol_ConvertUnsignedStrToRole (row[1]);
     }
   else	// User is not enroled in any course
     {
      ShrtName[0] = '\0';
      *MaxRole = Rol_UNK;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Write parameter with code of degree ********************/
/*****************************************************************************/

static void Deg_PutParDegCod (void *DegCod)
  {
   if (DegCod)
      ParCod_PutPar (ParCod_Deg,*((long *) DegCod));
  }
