// swad_degree_type.c: degree types

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

#include <ctype.h>		// For isprint, isspace, etc.
#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdlib.h>		// For exit, system, calloc, free, etc.
#include <string.h>		// For string functions
#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_degree.h"
#include "swad_degree_database.h"
#include "swad_degree_type.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Private variables ******************************/
/*****************************************************************************/

static struct DegTyp_DegType *DegTyp_EditingDegTyp = NULL;	// Static variable to keep the degree type being edited

/*****************************************************************************/
/*************************** Private prototypes ******************************/
/*****************************************************************************/

static void DegTyp_SeeDegTypes (Act_Action_t NextAction,Hie_Level_t HieLvl,
                                DegTyp_Order_t DefaultOrder);
static DegTyp_Order_t DegTyp_GetParDegTypOrder (DegTyp_Order_t DefaultOrder);

static void DegTyp_ListDegTypes (const struct DegTyp_DegTypes *DegTypes,
                                 Act_Action_t NextAction,
                                 Hie_Level_t HieLvl,
                                 DegTyp_Order_t SelectedOrder);

static void DegTyp_EditDegTypesInternal (const struct DegTyp_DegTypes *DegTypes);
static void DegTyp_PutIconsEditingDegTypes (__attribute__((unused)) void *Args);

static void DegTyp_ListDegTypesForSeeing (const struct DegTyp_DegTypes *DegTypes);
static void DegTyp_PutIconsListingDegTypes (__attribute__((unused)) void *Args);
static void DegTyp_PutIconToEditDegTypes (__attribute__((unused)) void *Args);
static void DegTyp_ListDegTypesForEdition (const struct DegTyp_DegTypes *DegTypes);

static void DegTyp_PutFormToCreateDegreeType (void);

static void DegTyp_PutHeadDegTypesForSeeing (Act_Action_t NextAction,
                                             Hie_Level_t HieLvl,
                                             DegTyp_Order_t SelectedOrder);
static void DegTyp_PutHeadDegTypesForEdition (void);

static void DegTyp_PutParOtherDegTypCod (void *DegTypCod);

static void DegTyp_RemoveDegTypeCompletely (long DegTypCod);

static void DegTyp_EditingDegTypeConstructor (void);
static void DegTyp_EditingDegTypeDestructor (void);

/*****************************************************************************/
/************** Show selector of degree types for statistics *****************/
/*****************************************************************************/

void DegTyp_WriteSelectorDegTypes (long SelectedDegTypCod)
  {
   extern const char *Txt_Any_type_of_degree;
   struct DegTyp_DegTypes DegTypes;
   unsigned NumDegTyp;
   const struct DegTyp_DegType *DegTyp;

   /***** Form to select degree types *****/
   /* Get list of degree types */
   DegTyp_GetListDegTypes (&DegTypes,Hie_SYS,DegTyp_ORDER_BY_DEG_TYPE);

   /* List degree types */
   HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
		     "id=\"OthDegTypCod\" name=\"OthDegTypCod\""
		     " class=\"Frm_C2_INPUT INPUT_%s\"",The_GetSuffix ());
      HTM_OPTION (HTM_Type_STRING,"-1",
		  (SelectedDegTypCod <= 0) ? HTM_SELECTED :
					     HTM_NO_ATTR,
		  "%s",Txt_Any_type_of_degree);
      for (NumDegTyp = 0;
	   NumDegTyp < DegTypes.Num;
	   NumDegTyp++)
	{
	 DegTyp = &DegTypes.Lst[NumDegTyp];
	 HTM_OPTION (HTM_Type_LONG,&DegTyp->DegTypCod,
		     (DegTyp->DegTypCod == SelectedDegTypCod) ? HTM_SELECTED :
							        HTM_NO_ATTR,
		     "%s",DegTyp->DegTypName);
	}
   HTM_SELECT_End ();

   /***** Free list of degree types *****/
   DegTyp_FreeListDegTypes (&DegTypes);
  }

/*****************************************************************************/
/***************************** Show degree types *****************************/
/*****************************************************************************/

void DegTyp_SeeDegTypesInDegTab (void)
  {
   DegTyp_SeeDegTypes (ActSeeDegTyp,Hie_SYS,
		       DegTyp_ORDER_BY_DEG_TYPE);	// Default order if not specified
  }

void DegTyp_GetAndShowDegTypesStats (Hie_Level_t HieLvl)
  {
   DegTyp_SeeDegTypes (ActSeeUseGbl,HieLvl,
		       DegTyp_ORDER_BY_NUM_DEGS);	// Default order if not specified
  }

static void DegTyp_SeeDegTypes (Act_Action_t NextAction,Hie_Level_t HieLvl,
                                DegTyp_Order_t DefaultOrder)
  {
   DegTyp_Order_t SelectedOrder;
   struct DegTyp_DegTypes DegTypes;

   /***** Get parameter with the type of order in the list of degree types *****/
   SelectedOrder = DegTyp_GetParDegTypOrder (DefaultOrder);

   /***** Get list of degree types *****/
   DegTyp_GetListDegTypes (&DegTypes,HieLvl,SelectedOrder);

      /***** List degree types *****/
      DegTyp_ListDegTypes (&DegTypes,NextAction,HieLvl,SelectedOrder);

   /***** Free list of degree types *****/
   DegTyp_FreeListDegTypes (&DegTypes);
  }

/*****************************************************************************/
/******* Get parameter with the type or order in list of degree types ********/
/*****************************************************************************/

static DegTyp_Order_t DegTyp_GetParDegTypOrder (DegTyp_Order_t DefaultOrder)
  {
   return (DegTyp_Order_t) Par_GetParUnsignedLong ("Order",
						   0,
						   DegTyp_NUM_ORDERS - 1,
						   (unsigned long) DefaultOrder);
  }

/*****************************************************************************/
/***************************** List degree types *****************************/
/*****************************************************************************/
// This function can be called from:
// - center tab		=> NextAction = ActSeeDegTyp
// - statistic tab	=> NextAction = ActSeeUseGbl

static void DegTyp_ListDegTypes (const struct DegTyp_DegTypes *DegTypes,
                                 Act_Action_t NextAction,
                                 Hie_Level_t HieLvl,
                                 DegTyp_Order_t SelectedOrder)
  {
   extern const char *Hlp_CENTER_DegreeTypes;
   extern const char *Hlp_ANALYTICS_Figures_types_of_degree;
   extern const char *Txt_Types_of_degree;
   extern const char *Txt_No_types_of_degree;

   /***** Begin box *****/
   switch (NextAction)
     {
      case ActSeeDegTyp:
	 Box_BoxBegin (Txt_Types_of_degree,
	               DegTyp_PutIconsListingDegTypes,NULL,
		       Hlp_CENTER_DegreeTypes,Box_NOT_CLOSABLE);
	 break;
      case ActSeeUseGbl:
	 Box_BoxBegin (Txt_Types_of_degree,
	               DegTyp_PutIconToEditDegTypes,NULL,
		       Hlp_ANALYTICS_Figures_types_of_degree,Box_NOT_CLOSABLE);
	 break;
      default:	// Bad call
	 return;
     }

   if (DegTypes->Num)
     {
      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (2);

         /***** Write heading *****/
	 DegTyp_PutHeadDegTypesForSeeing (NextAction,HieLvl,SelectedOrder);

	 /***** List current degree types for seeing *****/
	 DegTyp_ListDegTypesForSeeing (DegTypes);

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No degree types created
      Ale_ShowAlert (Ale_INFO,Txt_No_types_of_degree);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************************ Put forms to edit degree types *********************/
/*****************************************************************************/

void DegTyp_GetAndEditDegTypes (void)
  {
   struct DegTyp_DegTypes DegTypes;

   DegTyp_GetListDegTypes (&DegTypes,Hie_SYS,DegTyp_ORDER_BY_DEG_TYPE);
   DegTyp_EditDegTypes (&DegTypes);
   DegTyp_FreeListDegTypes (&DegTypes);
  }

void DegTyp_EditDegTypes (const struct DegTyp_DegTypes *DegTypes)
  {
   DegTyp_EditingDegTypeConstructor ();
   DegTyp_EditDegTypesInternal (DegTypes);
   DegTyp_EditingDegTypeDestructor ();
  }

static void DegTyp_EditDegTypesInternal (const struct DegTyp_DegTypes *DegTypes)
  {
   extern const char *Hlp_CENTER_DegreeTypes_edit;
   extern const char *Txt_Types_of_degree;

   /***** Begin box *****/
   Box_BoxBegin (Txt_Types_of_degree,DegTyp_PutIconsEditingDegTypes,NULL,
                 Hlp_CENTER_DegreeTypes_edit,Box_NOT_CLOSABLE);

      /***** Put a form to create a new degree type *****/
      DegTyp_PutFormToCreateDegreeType ();

      /***** Forms to edit current degree types *****/
      if (DegTypes->Num)
	 DegTyp_ListDegTypesForEdition (DegTypes);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************ Put contextual icons when editing degree types *****************/
/*****************************************************************************/

static void DegTyp_PutIconsEditingDegTypes (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view degree types *****/
   Ico_PutContextualIconToView (ActSeeDegTyp,NULL,NULL,NULL);

   /***** Put icon to view degrees *****/
   Deg_PutIconToViewDegrees ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_DEGREE_TYPES);
  }

/*****************************************************************************/
/******************* Put link (form) to view degree types ********************/
/*****************************************************************************/

void DegTyp_PutIconToViewDegTypes (void)
  {
   Lay_PutContextualLinkOnlyIcon (ActSeeDegTyp,NULL,NULL,NULL,
				  "sitemap.svg",Ico_BLACK);
  }

/*****************************************************************************/
/******************* List current degree types for seeing ********************/
/*****************************************************************************/

static void DegTyp_ListDegTypesForSeeing (const struct DegTyp_DegTypes *DegTypes)
  {
   unsigned NumDegTyp;
   const char *BgColor;

   /***** List degree types with forms for edition *****/
   for (NumDegTyp = 0, The_ResetRowColor ();
	NumDegTyp < DegTypes->Num;
	NumDegTyp++, The_ChangeRowColor ())
     {
      BgColor = (DegTypes->Lst[NumDegTyp].DegTypCod ==
	         Gbl.Hierarchy.Node[Hie_DEG].Specific.TypCod) ? "BG_HIGHLIGHT" :
								The_GetColorRows ();

      /* Begin table row */
      HTM_TR_Begin (NULL);

	 /* Number of degree type in this list */
	 HTM_TD_Begin ("class=\"RM DAT_STRONG_%s %s\"",The_GetSuffix (),BgColor);
	    HTM_Unsigned (NumDegTyp + 1);
	 HTM_TD_End ();

	 /* Name of degree type */
	 HTM_TD_Begin ("class=\"LM DAT_STRONG_%s %s\"",The_GetSuffix (),BgColor);
	    HTM_Txt (DegTypes->Lst[NumDegTyp].DegTypName);
	 HTM_TD_End ();

	 /* Number of degrees of this type */
	 HTM_TD_Begin ("class=\"RM DAT_STRONG_%s %s\"",The_GetSuffix (),BgColor);
	    HTM_Unsigned (DegTypes->Lst[NumDegTyp].NumDegs);
	 HTM_TD_End ();

      /* End table row */
      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/************** Put contextual icons in list of degree types *****************/
/*****************************************************************************/

static void DegTyp_PutIconsListingDegTypes (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit degree types *****/
   DegTyp_PutIconToEditDegTypes (NULL);

   /***** Put icon to view degrees *****/
   Deg_PutIconToViewDegrees ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_DEGREE_TYPES);
  }

/*****************************************************************************/
/******************* Put link (form) to edit degree types ********************/
/*****************************************************************************/

static void DegTyp_PutIconToEditDegTypes (__attribute__((unused)) void *Args)
  {
   if (Gbl.Hierarchy.HieLvl == Hie_CTR &&	// Only editable if center tab is visible
       DegTyp_CheckIfICanCreateDegTypes () == Usr_CAN)
      Ico_PutContextualIconToEdit (ActEdiDegTyp,NULL,NULL,NULL);
  }

/*****************************************************************************/
/******************* List current degree types for edition *******************/
/*****************************************************************************/

static void DegTyp_ListDegTypesForEdition (const struct DegTyp_DegTypes *DegTypes)
  {
   unsigned NumDegTyp;

   /***** Begin table *****/
   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Write heading *****/
      DegTyp_PutHeadDegTypesForEdition ();

      /***** List degree types with forms for edition *****/
      for (NumDegTyp = 0;
	   NumDegTyp < DegTypes->Num;
	   NumDegTyp++)
	{
	 /* Begin table row */
	 HTM_TR_Begin (NULL);

	    /* Put icon to remove degree type */
	    HTM_TD_Begin ("class=\"BM\"");
	       if (DegTypes->Lst[NumDegTyp].NumDegs)	// Degree type has degrees => deletion forbidden
		  Ico_PutIconRemovalNotAllowed ();
	       else
		  Ico_PutContextualIconToRemove (ActRemDegTyp,NULL,
						 DegTyp_PutParOtherDegTypCod,
						 &DegTypes->Lst[NumDegTyp].DegTypCod);
	    HTM_TD_End ();

	    /* Degree type code */
	    HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
	       HTM_Long (DegTypes->Lst[NumDegTyp].DegTypCod);
	    HTM_TD_End ();

	    /* Name of degree type */
	    HTM_TD_Begin ("class=\"LM\"");
	       Frm_BeginForm (ActRenDegTyp);
		  DegTyp_PutParOtherDegTypCod (&DegTypes->Lst[NumDegTyp].DegTypCod);
		  HTM_INPUT_TEXT ("DegTypName",DegTyp_MAX_CHARS_DEGREE_TYPE_NAME,
				  DegTypes->Lst[NumDegTyp].DegTypName,
				  HTM_REQUIRED | HTM_SUBMIT_ON_CHANGE,
				  "size=\"25\" class=\"INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Number of degrees of this type */
	    HTM_TD_Unsigned (DegTypes->Lst[NumDegTyp].NumDegs);

	 /* End table row */
	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** Check if I can create degree types *********************/
/*****************************************************************************/

Usr_Can_t DegTyp_CheckIfICanCreateDegTypes (void)
  {
   return (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM) ? Usr_CAN :
						     Usr_CAN_NOT;
  }

/*****************************************************************************/
/******************** Put a form to create a new degree type *****************/
/*****************************************************************************/

static void DegTyp_PutFormToCreateDegreeType (void)
  {
   /***** Begin form to create *****/
   Frm_BeginFormTable (ActNewDegTyp,NULL,NULL,NULL,"TBL_SCROLL");

      /***** Write heading *****/
      DegTyp_PutHeadDegTypesForEdition ();

      /***** Begin table row *****/
      HTM_TR_Begin (NULL);

	 /***** Column to remove degree type, disabled here *****/
	 HTM_TD_Begin ("class=\"BM\"");
	 HTM_TD_End ();

	 /***** Degree type code *****/
	 HTM_TD_Begin ("class=\"CODE\"");
	 HTM_TD_End ();

	 /***** Degree type name *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("DegTypName",DegTyp_MAX_CHARS_DEGREE_TYPE_NAME,DegTyp_EditingDegTyp->DegTypName,
			    HTM_REQUIRED,
			    "size=\"25\" class=\"INPUT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Number of degrees of this degree type ****/
	 HTM_TD_Unsigned (0);

      /***** End table row *****/
      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE);
  }

/*****************************************************************************/
/***************** Write header with fields of a degree type *****************/
/*****************************************************************************/

static void DegTyp_PutHeadDegTypesForSeeing (Act_Action_t NextAction,
                                             Hie_Level_t HieLvl,
                                             DegTyp_Order_t SelectedOrder)
  {
   extern const char *Txt_DEGREE_TYPES_HELP_ORDER[DegTyp_NUM_ORDERS];
   extern const char *Txt_DEGREE_TYPES_ORDER[DegTyp_NUM_ORDERS];
   DegTyp_Order_t Order;
   static HTM_HeadAlign Align[DegTyp_NUM_ORDERS] =
     {
      [DegTyp_ORDER_BY_DEG_TYPE] = HTM_HEAD_LEFT,
      [DegTyp_ORDER_BY_NUM_DEGS] = HTM_HEAD_RIGHT
     };
   struct Fig_Figures Figures;

   HTM_TR_Begin (NULL);

      HTM_TH_Empty (1);

      for (Order  = (DegTyp_Order_t) 0;
	   Order <= (DegTyp_Order_t) (DegTyp_NUM_ORDERS - 1);
	   Order++)
	{
         HTM_TH_Begin (Align[Order]);

	    /* Begin form to change order */
	    Frm_BeginForm (NextAction);
	       if (NextAction == ActSeeUseGbl)
		 {
		  Figures.HieLvl      = HieLvl;
		  Figures.FigureType = Fig_DEGREE_TYPES;
		  Fig_PutParsFigures (&Figures);
		 }
	       Par_PutParUnsigned (NULL,"Order",(unsigned) Order);

	       /* Link with the head of this column */
	       HTM_BUTTON_Submit_Begin (Txt_DEGREE_TYPES_HELP_ORDER[Order],NULL,
	                                "class=\"BT_LINK\"");
		  if (Order == SelectedOrder)
		     HTM_U_Begin ();
		  HTM_Txt (Txt_DEGREE_TYPES_ORDER[Order]);
		  if (Order == SelectedOrder)
		     HTM_U_End ();
	       HTM_BUTTON_End ();

	    /* End form */
	    Frm_EndForm ();

	 HTM_TH_End ();
	}

   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Write header with fields of a degree type *****************/
/*****************************************************************************/

static void DegTyp_PutHeadDegTypesForEdition (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Type_of_degree;
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];

   HTM_TR_Begin (NULL);

      HTM_TH_Span (NULL			       ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH (Txt_Code			       ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Type_of_degree	       ,HTM_HEAD_LEFT);
      HTM_TH (Txt_HIERARCHY_PLURAL_Abc[Hie_DEG],HTM_HEAD_RIGHT );

   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Create a list with all degree types **********************/
/*****************************************************************************/

void DegTyp_GetListDegTypes (struct DegTyp_DegTypes *DegTypes,
                             Hie_Level_t HieLvl,DegTyp_Order_t Order)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumTyp;

   /***** Get types of degree from database *****/
   DegTypes->Num = Deg_DB_GetDegreeTypes (&mysql_res,HieLvl,Order);
   DegTypes->Lst = NULL;

   /***** Get degree types *****/
   if (DegTypes->Num)
     {
      /***** Create a list of degree types *****/
      if ((DegTypes->Lst = calloc ((size_t) DegTypes->Num,
			           sizeof (struct DegTyp_DegType))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get degree types *****/
      for (NumTyp = 0;
	   NumTyp < DegTypes->Num;
	   NumTyp++)
        {
         /* Get next degree type */
         row = mysql_fetch_row (mysql_res);

         /* Get degree type code (row[0]) */
         if ((DegTypes->Lst[NumTyp].DegTypCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Err_WrongDegTypExit ();

         /* Get degree type name (row[1]) */
         Str_Copy (DegTypes->Lst[NumTyp].DegTypName,row[1],
                   sizeof (DegTypes->Lst[NumTyp].DegTypName) - 1);

         /* Number of degrees of this type (row[2]) */
         if (sscanf (row[2],"%u",&DegTypes->Lst[NumTyp].NumDegs) != 1)
            Err_ShowErrorAndExit ("Error when getting number of degrees of a type");
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********* Free list of degree types and list of degrees of each type ********/
/*****************************************************************************/

void DegTyp_FreeListDegTypes (struct DegTyp_DegTypes *DegTypes)
  {
   /***** Free memory used by the list of degree types *****/
   if (DegTypes->Num && DegTypes->Lst)
     {
      free (DegTypes->Lst);
      DegTypes->Lst = NULL;
      DegTypes->Num = 0;
     }
  }

/*****************************************************************************/
/***************** Receive form to create a new degree type ******************/
/*****************************************************************************/

void DegTyp_ReceiveNewDegTyp (void)
  {
   extern const char *Txt_The_type_of_degree_X_already_exists;
   extern const char *Txt_Created_new_type_of_degree_X;
   extern const char *Txt_You_must_specify_the_name;

   /***** Degree type constructor *****/
   DegTyp_EditingDegTypeConstructor ();

   /***** Get parameters from form *****/
   /* Get the name of degree type */
   Par_GetParText ("DegTypName",DegTyp_EditingDegTyp->DegTypName,DegTyp_MAX_BYTES_DEGREE_TYPE_NAME);

   if (DegTyp_EditingDegTyp->DegTypName[0])	// If there's a degree type name
      /***** If name of degree type was in database... *****/
      switch (Deg_DB_CheckIfDegreeTypeNameExists (DegTyp_EditingDegTyp->DegTypName,-1L))
        {
         case Exi_EXISTS:
	    Ale_CreateAlert (Ale_WARNING,NULL,
			     Txt_The_type_of_degree_X_already_exists,
			     DegTyp_EditingDegTyp->DegTypName);
            break;
         case Exi_DOES_NOT_EXIST:
         default:
            /* Add new degree type to database */
	    Deg_DB_CreateDegreeType (DegTyp_EditingDegTyp->DegTypName);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
			     Txt_Created_new_type_of_degree_X,
			     DegTyp_EditingDegTyp->DegTypName);
            break;
        }
   else	// If there is not a degree type name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_name);
  }

/*****************************************************************************/
/**************************** Remove a degree type ***************************/
/*****************************************************************************/

void DegTyp_RemoveDegTyp (void)
  {
   extern const char *Txt_To_remove_a_type_of_degree_you_must_first_remove_all_degrees_of_that_type;
   extern const char *Txt_Type_of_degree_X_removed;

   /***** Degree type constructor *****/
   DegTyp_EditingDegTypeConstructor ();

   /***** Get the code of the degree type *****/
   DegTyp_EditingDegTyp->DegTypCod = ParCod_GetAndCheckPar (ParCod_OthDegTyp);

   /***** Get data of the degree type from database *****/
   if (!DegTyp_GetDegTypeDataByCod (DegTyp_EditingDegTyp))
      Err_WrongDegTypExit ();

   /***** Check if this degree type has degrees *****/
   if (DegTyp_EditingDegTyp->NumDegs)	// Degree type has degrees => don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_a_type_of_degree_you_must_first_remove_all_degrees_of_that_type);
   else	// Degree type has no degrees => remove it
     {
      /***** Remove degree type *****/
      DegTyp_RemoveDegTypeCompletely (DegTyp_EditingDegTyp->DegTypCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Type_of_degree_X_removed,
                       DegTyp_EditingDegTyp->DegTypName);
     }
  }

/*****************************************************************************/
/***************** Write parameter with code of degree type ******************/
/*****************************************************************************/

static void DegTyp_PutParOtherDegTypCod (void *DegTypCod)
  {
   if (DegTypCod)
      ParCod_PutPar (ParCod_OthDegTyp,*((long *) DegTypCod));
  }

/*****************************************************************************/
/****************** Get data of a degree type from its code ******************/
/*****************************************************************************/

bool DegTyp_GetDegTypeDataByCod (struct DegTyp_DegType *DegTyp)
  {
   /***** Trivial check: code of degree type should be >= 0 *****/
   if (DegTyp->DegTypCod <= 0)
     {
      DegTyp->DegTypName[0] = '\0';
      DegTyp->NumDegs = 0;
      return false;
     }

   /***** Get the name of a type of degree from database *****/
   Deg_DB_GetDegTypeNameByCod (DegTyp);
   if (DegTyp->DegTypName[0])
     {
      /* Count number of degrees of this type */
      DegTyp->NumDegs = Deg_DB_GetNumDegsOfType (DegTyp->DegTypCod);
      return true;
     }

   DegTyp->DegTypName[0] = '\0';
   DegTyp->NumDegs = 0;
   return false;
  }

/*****************************************************************************/
/******************** Remove a degree type and its degrees *******************/
/*****************************************************************************/

static void DegTyp_RemoveDegTypeCompletely (long DegTypCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumDegs;
   unsigned NumDeg;
   long DegCod;

   /***** Get degrees of a type from database *****/
   NumDegs = Deg_DB_GetDegsOfType (&mysql_res,DegTypCod);

   /***** Remove degrees ******/
   for (NumDeg = 0;
	NumDeg < NumDegs;
	NumDeg++)
     {
      /* Get next degree */
      if ((DegCod = DB_GetNextCode (mysql_res)) < 0)
         Err_WrongDegreeExit ();

      /* Remove degree */
      Deg_RemoveDegreeCompletely (DegCod);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Remove the degree type *****/
   Deg_DB_RemoveDegTyp (DegTypCod);
  }

/*****************************************************************************/
/**************************** Rename a degree type ***************************/
/*****************************************************************************/

void DegTyp_RenameDegTyp (void)
  {
   extern const char *Txt_The_type_of_degree_X_already_exists;
   extern const char *Txt_The_type_of_degree_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   char NewNameDegTyp[DegTyp_MAX_BYTES_DEGREE_TYPE_NAME + 1];

   /***** Degree type constructor *****/
   DegTyp_EditingDegTypeConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the degree type */
   DegTyp_EditingDegTyp->DegTypCod = ParCod_GetAndCheckPar (ParCod_OthDegTyp);

   /* Get the new name for the degree type */
   Par_GetParText ("DegTypName",NewNameDegTyp,DegTyp_MAX_BYTES_DEGREE_TYPE_NAME);

   /***** Get from the database the old name of the degree type *****/
   if (!DegTyp_GetDegTypeDataByCod (DegTyp_EditingDegTyp))
      Err_WrongDegTypExit ();

   /***** Check if new name is empty *****/
   if (NewNameDegTyp[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (DegTyp_EditingDegTyp->DegTypName,NewNameDegTyp))	// Different names
         /***** If degree type was in database... *****/
         switch (Deg_DB_CheckIfDegreeTypeNameExists (NewNameDegTyp,DegTyp_EditingDegTyp->DegTypCod))
	   {
	    case Exi_EXISTS:
	       Ale_CreateAlert (Ale_WARNING,NULL,
				Txt_The_type_of_degree_X_already_exists,
				NewNameDegTyp);
	       break;
	    case Exi_DOES_NOT_EXIST:
	    default:
	       /* Update the table changing old name by new name */
	       Deg_DB_UpdateDegTypName (DegTyp_EditingDegTyp->DegTypCod,NewNameDegTyp);

	       /* Write message to show the change made */
	       Ale_CreateAlert (Ale_SUCCESS,NULL,
				Txt_The_type_of_degree_X_has_been_renamed_as_Y,
				DegTyp_EditingDegTyp->DegTypName,NewNameDegTyp);
	       break;
	   }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_X_has_not_changed,NewNameDegTyp);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Set degree type name *****/
   Str_Copy (DegTyp_EditingDegTyp->DegTypName,NewNameDegTyp,
	     sizeof (DegTyp_EditingDegTyp->DegTypName) - 1);
  }

/*****************************************************************************/
/********** Show message of success after changing a degree type *************/
/*****************************************************************************/

void DegTyp_ContEditAfterChgDegTyp (void)
  {
   struct DegTyp_DegTypes DegTypes;

   /***** Show possible delayed alerts *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   DegTyp_GetListDegTypes (&DegTypes,Hie_SYS,DegTyp_ORDER_BY_DEG_TYPE);
   DegTyp_EditDegTypesInternal (&DegTypes);
   DegTyp_FreeListDegTypes (&DegTypes);

   /***** Degree type destructor *****/
   DegTyp_EditingDegTypeDestructor ();
  }

/*****************************************************************************/
/********************* Degree type constructor/destructor ********************/
/*****************************************************************************/

static void DegTyp_EditingDegTypeConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (DegTyp_EditingDegTyp != NULL)
      Err_WrongDegTypExit ();

   /***** Allocate memory for degree type *****/
   if ((DegTyp_EditingDegTyp = malloc (sizeof (*DegTyp_EditingDegTyp))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Reset degree type *****/
   DegTyp_EditingDegTyp->DegTypCod     = -1L;
   DegTyp_EditingDegTyp->DegTypName[0] = '\0';
   DegTyp_EditingDegTyp->NumDegs       = 0;
  }

static void DegTyp_EditingDegTypeDestructor (void)
  {
   /***** Free memory used for degree type *****/
   if (DegTyp_EditingDegTyp != NULL)
     {
      free (DegTyp_EditingDegTyp);
      DegTyp_EditingDegTyp = NULL;
     }
  }
