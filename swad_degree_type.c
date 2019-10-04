// swad_degree_type.c: degree types

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#include <linux/stddef.h>	// For NULL
#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For fprintf, etc.
#include <stdlib.h>		// For exit, system, calloc, free, etc.
#include <string.h>		// For string functions
#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_degree.h"
#include "swad_degree_type.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_table.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Public constants *******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Private types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private variables ******************************/
/*****************************************************************************/

static struct DegreeType *DT_EditingDegTyp = NULL;	// Static variable to keep the degree type being edited

/*****************************************************************************/
/*************************** Private prototypes ******************************/
/*****************************************************************************/

static void DT_SeeDegreeTypes (Act_Action_t NextAction,Hie_Level_t Scope,
                               DT_Order_t DefaultOrder);
static DT_Order_t DT_GetParamDegTypOrder (DT_Order_t DefaultOrder);

static void DT_ListDegreeTypes (Act_Action_t NextAction,DT_Order_t SelectedOrder);

static void DT_EditDegreeTypesInternal (void);
static void DT_PutIconsEditingDegreeTypes (void);

static void DT_ListDegreeTypesForSeeing (void);
static void DT_PutIconsListingDegTypes (void);
static void DT_PutIconToEditDegTypes (void);
static void DT_ListDegreeTypesForEdition (void);

static void DT_PutFormToCreateDegreeType (void);

static void DT_PutHeadDegreeTypesForSeeing (Act_Action_t NextAction,DT_Order_t SelectedOrder);
static void DT_PutHeadDegreeTypesForEdition (void);
static void DT_CreateDegreeType (struct DegreeType *DegTyp);

static void DT_PutParamOtherDegTypCod (long DegTypCod);

static unsigned DT_CountNumDegsOfType (long DegTypCod);
static void DT_RemoveDegreeTypeCompletely (long DegTypCod);
static bool DT_CheckIfDegreeTypeNameExists (const char *DegTypName,long DegTypCod);

static void DT_EditingDegreeTypeConstructor (void);
static void DT_EditingDegreeTypeDestructor (void);

/*****************************************************************************/
/************** Show selector of degree types for statistics *****************/
/*****************************************************************************/

void DT_WriteSelectorDegreeTypes (void)
  {
   extern const char *Txt_Any_type_of_degree;
   unsigned NumDegTyp;

   /***** Form to select degree types *****/
   /* Get list of degree types */
   DT_GetListDegreeTypes (Hie_SYS,DT_ORDER_BY_DEGREE_TYPE);

   /* List degree types */
   fprintf (Gbl.F.Out,"<select id=\"OthDegTypCod\" name=\"OthDegTypCod\""
	              " onchange=\"document.getElementById('%s').submit();\">",
            Gbl.Form.Id);

   fprintf (Gbl.F.Out,"<option value=\"-1\"");
   if (Gbl.Stat.DegTypCod == -1L)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",Txt_Any_type_of_degree);

   for (NumDegTyp = 0;
	NumDegTyp < Gbl.DegTypes.Num;
	NumDegTyp++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%ld\"",Gbl.DegTypes.Lst[NumDegTyp].DegTypCod );
      if (Gbl.DegTypes.Lst[NumDegTyp].DegTypCod  == Gbl.Stat.DegTypCod)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",Gbl.DegTypes.Lst[NumDegTyp].DegTypName);
     }

   fprintf (Gbl.F.Out,"</select>");

   /***** Free list of degree types *****/
   DT_FreeListDegreeTypes ();
  }

/*****************************************************************************/
/***************************** Show degree types *****************************/
/*****************************************************************************/

void DT_SeeDegreeTypesInDegTab (void)
  {
   DT_SeeDegreeTypes (ActSeeDegTyp,Hie_SYS,
                      DT_ORDER_BY_DEGREE_TYPE);	// Default order if not specified
  }

void DT_SeeDegreeTypesInStaTab (void)
  {
   DT_SeeDegreeTypes (ActSeeUseGbl,Gbl.Scope.Current,
                      DT_ORDER_BY_NUM_DEGREES);	// Default order if not specified
  }

static void DT_SeeDegreeTypes (Act_Action_t NextAction,Hie_Level_t Scope,
                               DT_Order_t DefaultOrder)
  {
   DT_Order_t SelectedOrder;

   /***** Get parameter with the type of order in the list of degree types *****/
   SelectedOrder = DT_GetParamDegTypOrder (DefaultOrder);

   /***** Get list of degree types *****/
   DT_GetListDegreeTypes (Scope,SelectedOrder);

   /***** List degree types *****/
   DT_ListDegreeTypes (NextAction,SelectedOrder);

   /***** Free list of degree types *****/
   DT_FreeListDegreeTypes ();
  }

/*****************************************************************************/
/******* Get parameter with the type or order in list of degree types ********/
/*****************************************************************************/

static DT_Order_t DT_GetParamDegTypOrder (DT_Order_t DefaultOrder)
  {
   return (DT_Order_t) Par_GetParToUnsignedLong ("Order",
						 0,
						 DT_NUM_ORDERS - 1,
						 (unsigned long) DefaultOrder);
  }

/*****************************************************************************/
/***************************** List degree types *****************************/
/*****************************************************************************/
// This function can be called from:
// - centre tab		=> NextAction = ActSeeDegTyp
// - statistic tab	=> NextAction = ActSeeUseGbl

static void DT_ListDegreeTypes (Act_Action_t NextAction,DT_Order_t SelectedOrder)
  {
   extern const char *Hlp_CENTRE_DegreeTypes;
   extern const char *Hlp_ANALYTICS_Figures_types_of_degree;
   extern const char *Txt_Types_of_degree;
   extern const char *Txt_No_types_of_degree;
   extern const char *Txt_Create_another_type_of_degree;
   extern const char *Txt_Create_type_of_degree;

   /***** Start box *****/
   switch (NextAction)
     {
      case ActSeeDegTyp:
	 Box_StartBox (NULL,Txt_Types_of_degree,DT_PutIconsListingDegTypes,
		       Hlp_CENTRE_DegreeTypes,Box_NOT_CLOSABLE);
	 break;
      case ActSeeUseGbl:
	 Box_StartBox (NULL,Txt_Types_of_degree,DT_PutIconToEditDegTypes,
		       Hlp_ANALYTICS_Figures_types_of_degree,Box_NOT_CLOSABLE);
	 break;
      default:	// Bad call
	 return;
     }

   if (Gbl.DegTypes.Num)
     {
      /***** Write heading *****/
      Tbl_StartTableWideMarginPadding (2);
      DT_PutHeadDegreeTypesForSeeing (NextAction,SelectedOrder);

      /***** List current degree types for seeing *****/
      DT_ListDegreeTypesForSeeing ();

      /***** End table *****/
      Tbl_EndTable ();
     }
   else	// No degree types created
      Ale_ShowAlert (Ale_INFO,Txt_No_types_of_degree);

   /***** Button to create degree type  *****/
   if (DT_CheckIfICanCreateDegreeTypes ())
     {
      Frm_StartForm (ActEdiDegTyp);
      Btn_PutConfirmButton (Gbl.DegTypes.Num ? Txt_Create_another_type_of_degree :
	                                            Txt_Create_type_of_degree);
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/************************ Put forms to edit degree types *********************/
/*****************************************************************************/

void DT_EditDegreeTypes (void)
  {
   /***** Degree type constructor *****/
   DT_EditingDegreeTypeConstructor ();

   /***** Edit degree types *****/
   DT_EditDegreeTypesInternal ();

   /***** Degree type destructor *****/
   DT_EditingDegreeTypeDestructor ();
  }

static void DT_EditDegreeTypesInternal (void)
  {
   extern const char *Hlp_CENTRE_DegreeTypes_edit;
   extern const char *Txt_Types_of_degree;

   /***** Get list of degree types *****/
   DT_GetListDegreeTypes (Hie_SYS,DT_ORDER_BY_DEGREE_TYPE);

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Types_of_degree,DT_PutIconsEditingDegreeTypes,
                 Hlp_CENTRE_DegreeTypes_edit,Box_NOT_CLOSABLE);

   /***** Put a form to create a new degree type *****/
   DT_PutFormToCreateDegreeType ();

   /***** Forms to edit current degree types *****/
   if (Gbl.DegTypes.Num)
      DT_ListDegreeTypesForEdition ();

   /***** End box *****/
   Box_EndBox ();

   /***** Free list of degree types *****/
   DT_FreeListDegreeTypes ();
  }

/*****************************************************************************/
/************ Put contextual icons when editing degree types *****************/
/*****************************************************************************/

static void DT_PutIconsEditingDegreeTypes (void)
  {
   /***** Put icon to viee degree types *****/
   DT_PutIconToViewDegreeTypes ();

   /***** Put icon to view degrees *****/
   Deg_PutIconToViewDegrees ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_DEGREE_TYPES;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/******************* Put link (form) to view degree types ********************/
/*****************************************************************************/

void DT_PutIconToViewDegreeTypes (void)
  {
   extern const char *Txt_Types_of_degree;

   Lay_PutContextualLinkOnlyIcon (ActSeeDegTyp,NULL,NULL,
				  "sitemap.svg",
				  Txt_Types_of_degree);
  }

/*****************************************************************************/
/******************* List current degree types for seeing ********************/
/*****************************************************************************/

static void DT_ListDegreeTypesForSeeing (void)
  {
   unsigned NumDegTyp;
   const char *BgColor;

   /***** List degree types with forms for edition *****/
   for (NumDegTyp = 0;
	NumDegTyp < Gbl.DegTypes.Num;
	NumDegTyp++)
     {
      BgColor = (Gbl.DegTypes.Lst[NumDegTyp].DegTypCod ==
	         Gbl.Hierarchy.Deg.DegTypCod) ? "LIGHT_BLUE" :
                                                Gbl.ColorRows[Gbl.RowEvenOdd];

      /* Number of degree type in this list */
      Tbl_StartRow ();
      fprintf (Gbl.F.Out,"<td class=\"DAT_N RIGHT_MIDDLE %s\">"
			 "%u"
			 "</td>",
	       BgColor,NumDegTyp + 1);

      /* Name of degree type */
      fprintf (Gbl.F.Out,"<td class=\"DAT_N LEFT_MIDDLE %s\">"
	                 "%s"
	                 "</td>",
               BgColor,Gbl.DegTypes.Lst[NumDegTyp].DegTypName);

      /* Number of degrees of this type */
      fprintf (Gbl.F.Out,"<td class=\"DAT_N RIGHT_MIDDLE %s\">"
	                 "%u"
	                 "</td>",
               BgColor,Gbl.DegTypes.Lst[NumDegTyp].NumDegs);
      Tbl_EndRow ();

      Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
     }
  }

/*****************************************************************************/
/************** Put contextual icons in list of degree types *****************/
/*****************************************************************************/

static void DT_PutIconsListingDegTypes (void)
  {
   /***** Put icon to edit degree types *****/
   DT_PutIconToEditDegTypes ();

   /***** Put icon to view degrees *****/
   Deg_PutIconToViewDegrees ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_DEGREE_TYPES;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/******************* Put link (form) to edit degree types ********************/
/*****************************************************************************/

static void DT_PutIconToEditDegTypes (void)
  {
   if (Gbl.Hierarchy.Level == Hie_CTR &&	// Only editable if centre tab is visible
       DT_CheckIfICanCreateDegreeTypes ())
      Ico_PutContextualIconToEdit (ActEdiDegTyp,NULL);
  }

/*****************************************************************************/
/******************* List current degree types for edition *******************/
/*****************************************************************************/

static void DT_ListDegreeTypesForEdition (void)
  {
   unsigned NumDegTyp;

   /***** Write heading *****/
   Tbl_StartTableWidePadding (2);
   DT_PutHeadDegreeTypesForEdition ();

   /***** List degree types with forms for edition *****/
   for (NumDegTyp = 0;
	NumDegTyp < Gbl.DegTypes.Num;
	NumDegTyp++)
     {
      /* Put icon to remove degree type */
      Tbl_StartRow ();
      fprintf (Gbl.F.Out,"<td class=\"BM\">");
      if (Gbl.DegTypes.Lst[NumDegTyp].NumDegs)	// Degree type has degrees => deletion forbidden
         Ico_PutIconRemovalNotAllowed ();
      else
        {
         Frm_StartForm (ActRemDegTyp);
         DT_PutParamOtherDegTypCod (Gbl.DegTypes.Lst[NumDegTyp].DegTypCod);
         Ico_PutIconRemove ();
         Frm_EndForm ();
        }

      /* Degree type code */
      fprintf (Gbl.F.Out,"</td>"
	                 "<td class=\"DAT CODE\">"
	                 "%ld"
	                 "</td>",
               Gbl.DegTypes.Lst[NumDegTyp].DegTypCod);

      /* Name of degree type */
      fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">");
      Frm_StartForm (ActRenDegTyp);
      DT_PutParamOtherDegTypCod (Gbl.DegTypes.Lst[NumDegTyp].DegTypCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"DegTypName\""
	                 " size=\"25\" maxlength=\"%u\" value=\"%s\""
	                 " required=\"required\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Deg_MAX_CHARS_DEGREE_TYPE_NAME,
               Gbl.DegTypes.Lst[NumDegTyp].DegTypName,
               Gbl.Form.Id);
      Frm_EndForm ();
      fprintf (Gbl.F.Out,"</td>");

      /* Number of degrees of this type */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%u"
	                 "</td>",
               Gbl.DegTypes.Lst[NumDegTyp].NumDegs);
      Tbl_EndRow ();
     }

   /***** End table *****/
   Tbl_EndTable ();
  }

/*****************************************************************************/
/******************** Check if I can create degree types *********************/
/*****************************************************************************/

bool DT_CheckIfICanCreateDegreeTypes (void)
  {
   return (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
  }

/*****************************************************************************/
/******************** Put a form to create a new degree type *****************/
/*****************************************************************************/

static void DT_PutFormToCreateDegreeType (void)
  {
   extern const char *Txt_New_type_of_degree;
   extern const char *Txt_Create_type_of_degree;

   /***** Start form *****/
   Frm_StartForm (ActNewDegTyp);

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_New_type_of_degree,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   DT_PutHeadDegreeTypesForEdition ();

   /***** Column to remove degree type, disabled here *****/
   Tbl_StartRow ();
   fprintf (Gbl.F.Out,"<td class=\"BM\"></td>");

   /***** Degree type code *****/
   fprintf (Gbl.F.Out,"<td class=\"CODE\"></td>");

   /***** Degree type name *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" name=\"DegTypName\""
                      " size=\"25\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                     "</td>",
            Deg_MAX_CHARS_DEGREE_TYPE_NAME,DT_EditingDegTyp->DegTypName);

   /***** Number of degrees of this degree type ****/
   fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	              "0"
	              "</td>");
   Tbl_EndRow ();

   /***** End table, send button and end box *****/
   Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_type_of_degree);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/***************** Write header with fields of a degree type *****************/
/*****************************************************************************/

static void DT_PutHeadDegreeTypesForSeeing (Act_Action_t NextAction,DT_Order_t SelectedOrder)
  {
   extern const char *Txt_DEGREE_TYPES_HELP_ORDER[DT_NUM_ORDERS];
   extern const char *Txt_DEGREE_TYPES_ORDER[DT_NUM_ORDERS];
   DT_Order_t Order;

   Tbl_StartRow ();
   fprintf (Gbl.F.Out,"<th></th>");
   for (Order = DT_ORDER_BY_DEGREE_TYPE;
	Order <= DT_ORDER_BY_NUM_DEGREES;
	Order++)
     {
      fprintf (Gbl.F.Out,"<th class=\"%s\">",
               Order == DT_ORDER_BY_DEGREE_TYPE ? "LEFT_MIDDLE" :
        	                                  "RIGHT_MIDDLE");

      /* Start form to change order */
      Frm_StartForm (NextAction);
      if (NextAction == ActSeeUseGbl)
         Fig_PutHiddenParamFigures ();
      Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);

      /* Link with the head of this column */
      Frm_LinkFormSubmit (Txt_DEGREE_TYPES_HELP_ORDER[Order],"TIT_TBL",NULL);
      if (Order == SelectedOrder)
	 fprintf (Gbl.F.Out,"<u>");
      fprintf (Gbl.F.Out,"%s",Txt_DEGREE_TYPES_ORDER[Order]);
      if (Order == SelectedOrder)
	 fprintf (Gbl.F.Out,"</u>");
      fprintf (Gbl.F.Out,"</a>");

      /* End form */
      Frm_EndForm ();

      fprintf (Gbl.F.Out,"</th>");
     }
  }

/*****************************************************************************/
/***************** Write header with fields of a degree type *****************/
/*****************************************************************************/

static void DT_PutHeadDegreeTypesForEdition (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Type_of_degree;
   extern const char *Txt_Degrees;

   Tbl_StartRow ();
   fprintf (Gbl.F.Out,"<th class=\"BM\"></th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>",
            Txt_Code,
            Txt_Type_of_degree,
            Txt_Degrees);
   Tbl_EndRow ();
  }

/*****************************************************************************/
/************************** Create a new degree type *************************/
/*****************************************************************************/

static void DT_CreateDegreeType (struct DegreeType *DegTyp)
  {
   /***** Create a new degree type *****/
   DB_QueryINSERT ("can not create a new type of degree",
		   "INSERT INTO deg_types SET DegTypName='%s'",
                   DegTyp->DegTypName);
  }

/*****************************************************************************/
/**************** Create a list with all the degree types ********************/
/*****************************************************************************/

void DT_GetListDegreeTypes (Hie_Level_t Scope,DT_Order_t Order)
  {
   static const char *OrderBySubQuery[DT_NUM_ORDERS] =
     {
      "DegTypName",			// DT_ORDER_BY_DEGREE_TYPE
      "NumDegs DESC,DegTypName",	// DT_ORDER_BY_NUM_DEGREES
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;

   /***** Get types of degree from database *****/
   switch (Scope)
     {
      case Hie_SYS:
	 /* Get
	    all degree types with degrees
	    union with
	    all degree types without any degree */
	 Gbl.DegTypes.Num = (unsigned) DB_QuerySELECT (&mysql_res,"can not get types of degree",
							    "(SELECT deg_types.DegTypCod,deg_types.DegTypName,"
							    "COUNT(degrees.DegCod) AS NumDegs"
							    " FROM degrees,deg_types"
							    " WHERE degrees.DegTypCod=deg_types.DegTypCod"
							    " GROUP BY degrees.DegTypCod)"
							    " UNION "
							    "(SELECT DegTypCod,DegTypName,0 AS NumDegs"	// Do not use '0' because NumDegs will be casted to string and order will be wrong
							    " FROM deg_types"
							    " WHERE DegTypCod NOT IN"
							    " (SELECT DegTypCod FROM degrees))"
							    " ORDER BY %s",
							    OrderBySubQuery[Order]);
         break;
      case Hie_CTY:
	 /* Get only degree types with degrees in the current country */
	 Gbl.DegTypes.Num = (unsigned) DB_QuerySELECT (&mysql_res,"can not get types of degree",
							    "SELECT deg_types.DegTypCod,deg_types.DegTypName,"
							    "COUNT(degrees.DegCod) AS NumDegs"
							    " FROM institutions,centres,degrees,deg_types"
							    " WHERE institutions.CtyCod=%ld"
							    " AND institutions.InsCod=centres.InsCod"
							    " AND centres.CtrCod=degrees.CtrCod"
							    " AND degrees.DegTypCod=deg_types.DegTypCod"
							    " GROUP BY degrees.DegTypCod"
							    " ORDER BY %s",
							    Gbl.Hierarchy.Cty.CtyCod,
							    OrderBySubQuery[Order]);
         break;
      case Hie_INS:
	 /* Get only degree types with degrees in the current institution */
	 Gbl.DegTypes.Num = (unsigned) DB_QuerySELECT (&mysql_res,"can not get types of degree",
							    "SELECT deg_types.DegTypCod,deg_types.DegTypName,"
							    "COUNT(degrees.DegCod) AS NumDegs"
							    " FROM centres,degrees,deg_types"
							    " WHERE centres.InsCod=%ld"
							    " AND centres.CtrCod=degrees.CtrCod"
							    " AND degrees.DegTypCod=deg_types.DegTypCod"
							    " GROUP BY degrees.DegTypCod"
							    " ORDER BY %s",
							    Gbl.Hierarchy.Ins.InsCod,
							    OrderBySubQuery[Order]);
	 break;
      case Hie_CTR:
	 /* Get only degree types with degrees in the current centre */
	 Gbl.DegTypes.Num = (unsigned) DB_QuerySELECT (&mysql_res,"can not get types of degree",
							    "SELECT deg_types.DegTypCod,deg_types.DegTypName,"
							    "COUNT(degrees.DegCod) AS NumDegs"
							    " FROM degrees,deg_types"
							    " WHERE degrees.CtrCod=%ld"
							    " AND degrees.DegTypCod=deg_types.DegTypCod"
							    " GROUP BY degrees.DegTypCod"
							    " ORDER BY %s",
							    Gbl.Hierarchy.Ctr.CtrCod,
							    OrderBySubQuery[Order]);
	 break;
      case Hie_DEG:
      case Hie_CRS:
	 /* Get only degree types with degrees in the current degree */
	 Gbl.DegTypes.Num = (unsigned) DB_QuerySELECT (&mysql_res,"can not get types of degree",
							    "SELECT deg_types.DegTypCod,deg_types.DegTypName,"
							    "COUNT(degrees.DegCod) AS NumDegs"
							    " FROM degrees,deg_types"
							    " WHERE degrees.DegCod=%ld"
							    " AND degrees.DegTypCod=deg_types.DegTypCod"
							    " GROUP BY degrees.DegTypCod"
							    " ORDER BY %s",
							    Gbl.Hierarchy.Deg.DegCod,
							    OrderBySubQuery[Order]);
	 break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get degree types *****/
   if (Gbl.DegTypes.Num)
     {
      /***** Create a list of degree types *****/
      if ((Gbl.DegTypes.Lst = (struct DegreeType *)
				   calloc (Gbl.DegTypes.Num,
					   sizeof (struct DegreeType))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get degree types *****/
      for (NumRow = 0;
	   NumRow < Gbl.DegTypes.Num;
	   NumRow++)
        {
         /* Get next degree type */
         row = mysql_fetch_row (mysql_res);

         /* Get degree type code (row[0]) */
         if ((Gbl.DegTypes.Lst[NumRow].DegTypCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of type of degree.");

         /* Get degree type name (row[1]) */
         Str_Copy (Gbl.DegTypes.Lst[NumRow].DegTypName,row[1],
                   Deg_MAX_BYTES_DEGREE_TYPE_NAME);

         /* Number of degrees of this type (row[2]) */
         if (sscanf (row[2],"%u",&Gbl.DegTypes.Lst[NumRow].NumDegs) != 1)
            Lay_ShowErrorAndExit ("Error when getting number of degrees of a type");
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********* Free list of degree types and list of degrees of each type ********/
/*****************************************************************************/

void DT_FreeListDegreeTypes (void)
  {
   /***** Free memory used by the list of degree types *****/
   if (Gbl.DegTypes.Lst)
     {
      free ((void *) Gbl.DegTypes.Lst);
      Gbl.DegTypes.Lst = NULL;
      Gbl.DegTypes.Num = 0;
     }
  }

/*****************************************************************************/
/***************** Receive form to create a new degree type ******************/
/*****************************************************************************/

void DT_RecFormNewDegreeType (void)
  {
   extern const char *Txt_The_type_of_degree_X_already_exists;
   extern const char *Txt_Created_new_type_of_degree_X;
   extern const char *Txt_You_must_specify_the_name_of_the_new_type_of_degree;

   /***** Degree type constructor *****/
   DT_EditingDegreeTypeConstructor ();

   /***** Get parameters from form *****/
   /* Get the name of degree type */
   Par_GetParToText ("DegTypName",DT_EditingDegTyp->DegTypName,Deg_MAX_BYTES_DEGREE_TYPE_NAME);

   if (DT_EditingDegTyp->DegTypName[0])	// If there's a degree type name
     {
      /***** If name of degree type was in database... *****/
      if (DT_CheckIfDegreeTypeNameExists (DT_EditingDegTyp->DegTypName,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_type_of_degree_X_already_exists,
                          DT_EditingDegTyp->DegTypName);
      else	// Add new degree type to database
        {
         DT_CreateDegreeType (DT_EditingDegTyp);
      	 Ale_CreateAlert (Ale_SUCCESS,NULL,
      	                  Txt_Created_new_type_of_degree_X,
			  DT_EditingDegTyp->DegTypName);
        }
     }
   else	// If there is not a degree type name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_name_of_the_new_type_of_degree);
  }

/*****************************************************************************/
/**************************** Remove a degree type ***************************/
/*****************************************************************************/

void DT_RemoveDegreeType (void)
  {
   extern const char *Txt_To_remove_a_type_of_degree_you_must_first_remove_all_degrees_of_that_type;
   extern const char *Txt_Type_of_degree_X_removed;

   /***** Degree type constructor *****/
   DT_EditingDegreeTypeConstructor ();

   /***** Get the code of the degree type *****/
   DT_EditingDegTyp->DegTypCod = DT_GetAndCheckParamOtherDegTypCod (1);

   /***** Get data of the degree type from database *****/
   if (!DT_GetDataOfDegreeTypeByCod (DT_EditingDegTyp))
      Lay_ShowErrorAndExit ("Code of type of degree not found.");

   /***** Check if this degree type has degrees *****/
   if (DT_EditingDegTyp->NumDegs)	// Degree type has degrees => don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_a_type_of_degree_you_must_first_remove_all_degrees_of_that_type);
   else	// Degree type has no degrees => remove it
     {
      /***** Remove degree type *****/
      DT_RemoveDegreeTypeCompletely (DT_EditingDegTyp->DegTypCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Type_of_degree_X_removed,
                       DT_EditingDegTyp->DegTypName);
     }
  }

/*****************************************************************************/
/***************** Write parameter with code of degree type ******************/
/*****************************************************************************/

static void DT_PutParamOtherDegTypCod (long DegTypCod)
  {
   Par_PutHiddenParamLong ("OthDegTypCod",DegTypCod);
  }

/*****************************************************************************/
/******************* Get parameter with code of degree type ******************/
/*****************************************************************************/

long DT_GetAndCheckParamOtherDegTypCod (long MinCodAllowed)
  {
   long DegTypCod;

   /***** Get and check parameter with code of degree type *****/
   if ((DegTypCod = Par_GetParToLong ("OthDegTypCod")) < MinCodAllowed)
      Lay_ShowErrorAndExit ("Code of degree type is missing or invalid.");

   return DegTypCod;
  }

/*****************************************************************************/
/**************** Count number of degrees in a degree type ******************/
/*****************************************************************************/

static unsigned DT_CountNumDegsOfType (long DegTypCod)
  {
   /***** Get number of degrees of a type from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of degrees of a type",
			     "SELECT COUNT(*) FROM degrees"
			     " WHERE DegTypCod=%ld",
			     DegTypCod);
  }

/*****************************************************************************/
/****************** Get data of a degree type from its code ******************/
/*****************************************************************************/

bool DT_GetDataOfDegreeTypeByCod (struct DegreeType *DegTyp)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   bool DegTypFound = false;

   if (DegTyp->DegTypCod <= 0)
     {
      DegTyp->DegTypCod = -1L;
      DegTyp->DegTypName[0] = '\0';
      DegTyp->NumDegs = 0;
      return false;
     }

   /***** Get the name of a type of degree from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get the name of a type of degree",
			     "SELECT DegTypName FROM deg_types WHERE DegTypCod=%ld",
			     DegTyp->DegTypCod);
   if (NumRows == 1)
     {
      /***** Get data of degree type *****/
      row = mysql_fetch_row (mysql_res);

      /* Get the name of the degree type (row[0]) */
      Str_Copy (DegTyp->DegTypName,row[0],
                Deg_MAX_BYTES_DEGREE_TYPE_NAME);

      /* Count number of degrees of this type */
      DegTyp->NumDegs = DT_CountNumDegsOfType (DegTyp->DegTypCod);

      /* Set return value */
      DegTypFound = true;
     }
   else if (NumRows == 0)
     {
      DegTyp->DegTypCod = -1L;
      DegTyp->DegTypName[0] = '\0';
      DegTyp->NumDegs = 0;
      return false;
     }
   else // NumRows > 1
      Lay_ShowErrorAndExit ("Type of degree repeated in database.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return DegTypFound;
  }

/*****************************************************************************/
/******************** Remove a degree type and its degrees *******************/
/*****************************************************************************/

static void DT_RemoveDegreeTypeCompletely (long DegTypCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;
   long DegCod;

   /***** Get degrees of a type from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get degrees of a type",
			     "SELECT DegCod FROM degrees WHERE DegTypCod=%ld",
			     DegTypCod);

   /* Get degrees of this type */
   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      /* Get next degree */
      row = mysql_fetch_row (mysql_res);

      /* Get degree code (row[0]) */
      if ((DegCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
         Lay_ShowErrorAndExit ("Wrong code of degree.");

      /* Remove degree */
      Deg_RemoveDegreeCompletely (DegCod);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Remove the degree type *****/
   DB_QueryDELETE ("can not remove a type of degree",
		   "DELETE FROM deg_types WHERE DegTypCod=%ld",
		   DegTypCod);
  }

/*****************************************************************************/
/**************************** Rename a degree type ***************************/
/*****************************************************************************/

void DT_RenameDegreeType (void)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_type_of_degree_X_empty;
   extern const char *Txt_The_type_of_degree_X_already_exists;
   extern const char *Txt_The_type_of_degree_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_type_of_degree_X_has_not_changed;
   char NewNameDegTyp[Deg_MAX_BYTES_DEGREE_TYPE_NAME + 1];

   /***** Degree type constructor *****/
   DT_EditingDegreeTypeConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the degree type */
   DT_EditingDegTyp->DegTypCod = DT_GetAndCheckParamOtherDegTypCod (1);

   /* Get the new name for the degree type */
   Par_GetParToText ("DegTypName",NewNameDegTyp,Deg_MAX_BYTES_DEGREE_TYPE_NAME);

   /***** Get from the database the old name of the degree type *****/
   if (!DT_GetDataOfDegreeTypeByCod (DT_EditingDegTyp))
      Lay_ShowErrorAndExit ("Code of type of degree not found.");

   /***** Check if new name is empty *****/
   if (!NewNameDegTyp[0])
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_leave_the_name_of_the_type_of_degree_X_empty,
                       DT_EditingDegTyp->DegTypName);
   else
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (DT_EditingDegTyp->DegTypName,NewNameDegTyp))	// Different names
        {
         /***** If degree type was in database... *****/
         if (DT_CheckIfDegreeTypeNameExists (NewNameDegTyp,DT_EditingDegTyp->DegTypCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_type_of_degree_X_already_exists,
                             NewNameDegTyp);
         else
           {
            /* Update the table changing old name by new name */
            DB_QueryUPDATE ("can not update the type of a degree",
        		    "UPDATE deg_types SET DegTypName='%s'"
			    " WHERE DegTypCod=%ld",
                            NewNameDegTyp,DT_EditingDegTyp->DegTypCod);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_type_of_degree_X_has_been_renamed_as_Y,
                             DT_EditingDegTyp->DegTypName,NewNameDegTyp);
           }


        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_of_the_type_of_degree_X_has_not_changed,
                          NewNameDegTyp);
     }

   /***** Set degree type name *****/
   Str_Copy (DT_EditingDegTyp->DegTypName,NewNameDegTyp,
	     Deg_MAX_BYTES_DEGREE_TYPE_NAME);
  }

/*****************************************************************************/
/****************** Check if name of degree type exists **********************/
/*****************************************************************************/

static bool DT_CheckIfDegreeTypeNameExists (const char *DegTypName,long DegTypCod)
  {
   /***** Get number of degree types with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a type of degree"
			  " already existed",
			  "SELECT COUNT(*) FROM deg_types"
			  " WHERE DegTypName='%s' AND DegTypCod<>%ld",
			  DegTypName,DegTypCod) != 0);
  }

/*****************************************************************************/
/********** Show message of success after changing a degree type *************/
/*****************************************************************************/

void DT_ContEditAfterChgDegTyp (void)
  {
   /***** Show possible delayed alerts *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   DT_EditDegreeTypesInternal ();

   /***** Degree type destructor *****/
   DT_EditingDegreeTypeDestructor ();
  }

/*****************************************************************************/
/********************* Degree type constructor/destructor ********************/
/*****************************************************************************/

static void DT_EditingDegreeTypeConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (DT_EditingDegTyp != NULL)
      Lay_ShowErrorAndExit ("Error initializing degree type.");

   /***** Allocate memory for degree type *****/
   if ((DT_EditingDegTyp = (struct DegreeType *) malloc (sizeof (struct DegreeType))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for degree type.");

   /***** Reset degree type *****/
   DT_EditingDegTyp->DegTypCod     = -1L;
   DT_EditingDegTyp->DegTypName[0] = '\0';
   DT_EditingDegTyp->NumDegs       = 0;
  }

static void DT_EditingDegreeTypeDestructor (void)
  {
   /***** Free memory used for degree type *****/
   if (DT_EditingDegTyp != NULL)
     {
      free ((void *) DT_EditingDegTyp);
      DT_EditingDegTyp = NULL;
     }
  }
