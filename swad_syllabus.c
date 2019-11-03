// swad_syllabus.c: syllabus

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

#include <unistd.h>		// For SEEK_SET
#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdsoap2.h>		// For SOAP_OK and soap functions
#include <stdlib.h>		// For free ()
#include <string.h>		// For string functions
#include <time.h>		// For time ()

#include "swad_box.h"
#include "swad_changelog.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_string.h"
#include "swad_xml.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Syl_MAX_LEVELS_SYLLABUS		  10

#define Syl_MAX_BYTES_ITEM_COD		(Syl_MAX_LEVELS_SYLLABUS * (10 + 1) - 1)

#define Syl_MAX_CHARS_TEXT_ITEM		(1024 - 1)	// 1023
#define Syl_MAX_BYTES_TEXT_ITEM		((Syl_MAX_CHARS_TEXT_ITEM + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 16383

#define Syl_WIDTH_NUM_SYLLABUS 20

static const char *StyleSyllabus[1 + Syl_MAX_LEVELS_SYLLABUS] =
  {
   "",
   "T1",
   "T2",
   "T3",
   "T3",
   "T3",
   "T3",
   "T3",
   "T3",
   "T3",
   "T3",
  };

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct ItemSyllabus
  {
   int Level;
   int CodItem[1 + Syl_MAX_LEVELS_SYLLABUS];
   bool HasChildren;
   char Text[Syl_MAX_BYTES_TEXT_ITEM + 1];
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

struct
  {
   struct ItemSyllabus *Lst;		// List of items of a syllabus
   unsigned NumItems;			// Number of items in the list
   unsigned NumItemsWithChildren;	// Number of items with children
   int NumLevels;			// Number of levels in the list
  } LstItemsSyllabus;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Syl_SetSyllabusTypeFromAction (void);
static void Syl_ShowSyllabus (bool PutIconToEdit);
static void Syl_ShowRowSyllabus (unsigned NumItem,
                                 int Level,int *CodItem,const char *Text,bool NewItem);
static void Syl_WriteSyllabusIntoHTMLTmpFile (FILE *FileHTMLTmp);
static void Syl_PutFormItemSyllabus (bool NewItem,unsigned NumItem,int Level,int *CodItem,const char *Text);
static void Syl_PutParamNumItem (void);

static void Syl_WriteNumItem (char *StrDst,FILE *FileTgt,int Level,int *CodItem);

/*****************************************************************************/
/************* Get parameter about which syllabus I want to see **************/
/*****************************************************************************/

void Syl_GetParamWhichSyllabus (void)
  {
   /***** Get which syllabus I want to see *****/
   Gbl.Syllabus.WhichSyllabus = (Syl_WhichSyllabus_t)
	                        Par_GetParToUnsignedLong ("WhichSyllabus",
	                                                  0,
	                                                  Syl_NUM_WHICH_SYLLABUS - 1,
	                                                  (unsigned long) Syl_DEFAULT_WHICH_SYLLABUS);
  }

/*****************************************************************************/
/************************ Write form to select syllabus **********************/
/*****************************************************************************/

void Syl_PutFormWhichSyllabus (void)
  {
   extern const char *Txt_SYLLABUS_WHICH_SYLLABUS[Syl_NUM_WHICH_SYLLABUS];
   Syl_WhichSyllabus_t WhichSyllabus;

   /***** Form to select which syllabus I want to see (lectures/practicals) *****/
   Frm_StartForm (ActSeeSyl);
   HTM_DIV_Begin ("class=\"CM\"");
   HTM_UL_Begin ("class=\"LIST_LEFT\"");

   for (WhichSyllabus = (Syl_WhichSyllabus_t) 0;
	WhichSyllabus < For_NUM_FORUM_SETS;
	WhichSyllabus++)
     {
      HTM_LI_Begin ("class=\"DAT LM\"");
      HTM_LABEL_Begin (NULL);
      fprintf (Gbl.F.Out,"<input type=\"radio\" name=\"WhichSyllabus\" value=\"%u\"",
               (unsigned) WhichSyllabus);
      if (WhichSyllabus == Gbl.Syllabus.WhichSyllabus)
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"document.getElementById('%s').submit();\" />"
	                 "%s",
               Gbl.Form.Id,Txt_SYLLABUS_WHICH_SYLLABUS[WhichSyllabus]);
      HTM_LABEL_End ();
      HTM_LI_End ();
     }
   HTM_UL_End ();
   HTM_DIV_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/************ Get parameter item number in edition of syllabus ***************/
/*****************************************************************************/

void Syl_GetParamItemNumber (void)
  {
   Gbl.Syllabus.NumItem = (unsigned)
	                  Par_GetParToUnsignedLong ("NumI",
	                                            0,
	                                            UINT_MAX,
	                                            0);
  }

/*****************************************************************************/
/********************** Check if syllabus is not empty ***********************/
/*****************************************************************************/
// Return true if info available

bool Syl_CheckSyllabus (long CrsCod,Inf_InfoType_t InfoType)
  {
   bool InfoAvailable;

   /***** Set syllabus type *****/
   switch (InfoType)
     {
      case Inf_LECTURES:
         Gbl.Syllabus.WhichSyllabus = Syl_LECTURES;
         break;
      case Inf_PRACTICALS:
         Gbl.Syllabus.WhichSyllabus = Syl_PRACTICALS;
         break;
      default:
	 return false;
     }

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (CrsCod);

   /***** Number of items > 0 ==> info available *****/
   InfoAvailable = (LstItemsSyllabus.NumItems != 0);

   /***** Free memory used to store items *****/
   Syl_FreeListItemsSyllabus ();

   return InfoAvailable;
  }

/*****************************************************************************/
/****************************** Edit a syllabus ******************************/
/*****************************************************************************/
// Return true if info available

bool Syl_CheckAndEditSyllabus (void)
  {
   extern const Act_Action_t Inf_ActionsSeeInfo[Inf_NUM_INFO_TYPES];
   extern const char *Txt_Done;
   bool ICanEdit;
   bool PutIconToEdit;

   /***** Set syllabus type depending on current action *****/
   Syl_SetSyllabusTypeFromAction ();

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (Gbl.Hierarchy.Crs.CrsCod);

   if (Gbl.Action.Act == ActEditorSylLec ||
       Gbl.Action.Act == ActEditorSylPra)
      Gbl.Syllabus.EditionIsActive = true;

   if (Gbl.Syllabus.EditionIsActive || LstItemsSyllabus.NumItems)
     {
      ICanEdit = Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
		 Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM;
      PutIconToEdit = ICanEdit && !Gbl.Syllabus.EditionIsActive;

      /***** Write the current syllabus *****/
      Syl_ShowSyllabus (PutIconToEdit);

      if (Gbl.Syllabus.EditionIsActive)
	{
	 /***** Button to view *****/
         Frm_StartForm (Inf_ActionsSeeInfo[Gbl.Crs.Info.Type]);
	 Btn_PutConfirmButton (Txt_Done);
	 Frm_EndForm ();
	}

      /***** End box *****/
      Box_BoxEnd ();

      return true;
     }

   return false;
  }

/*****************************************************************************/
/****************************** Edit a syllabus ******************************/
/*****************************************************************************/
// Return true if info available

void Syl_EditSyllabus (void)
  {
   (void) Syl_CheckAndEditSyllabus ();
  }

/*****************************************************************************/
/************* Set syllabus type depending on the current action *************/
/*****************************************************************************/

static void Syl_SetSyllabusTypeFromAction (void)
  {
   Gbl.Crs.Info.Type = Inf_LECTURES;

   /***** Set the type of syllabus (lectures or practicals) *****/
   switch (Gbl.Action.Act)
     {
      case ActSeeSyl:
	 Gbl.Crs.Info.Type = (Gbl.Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
	                                                                          Inf_PRACTICALS);
	 break;
      case ActSeeSylLec:
      case ActEdiSylLec:
      case ActDelItmSylLec:
      case ActUp_IteSylLec:
      case ActDwnIteSylLec:
      case ActRgtIteSylLec:
      case ActLftIteSylLec:
      case ActInsIteSylLec:
      case ActModIteSylLec:
      case ActChgFrcReaSylLec:
      case ActChgHavReaSylLec:
      case ActSelInfSrcSylLec:
      case ActRcvURLSylLec:
      case ActRcvPagSylLec:
      case ActEditorSylLec:
      case ActPlaTxtEdiSylLec:
      case ActRchTxtEdiSylLec:
      case ActRcvPlaTxtSylLec:
      case ActRcvRchTxtSylLec:
	 Gbl.Syllabus.WhichSyllabus = Syl_LECTURES;
	 Gbl.Crs.Info.Type = Inf_LECTURES;
	 break;
      case ActSeeSylPra:
      case ActEdiSylPra:
      case ActDelItmSylPra:
      case ActUp_IteSylPra:
      case ActDwnIteSylPra:
      case ActRgtIteSylPra:
      case ActLftIteSylPra:
      case ActInsIteSylPra:
      case ActModIteSylPra:
      case ActChgFrcReaSylPra:
      case ActChgHavReaSylPra:
      case ActSelInfSrcSylPra:
      case ActRcvURLSylPra:
      case ActRcvPagSylPra:
      case ActEditorSylPra:
      case ActPlaTxtEdiSylPra:
      case ActRchTxtEdiSylPra:
      case ActRcvPlaTxtSylPra:
      case ActRcvRchTxtSylPra:
	 Gbl.Syllabus.WhichSyllabus = Syl_PRACTICALS;
	 Gbl.Crs.Info.Type = Inf_PRACTICALS;
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong action.");
	 break;
     }
  }

/*****************************************************************************/
/*** Read from XML and load in memory a syllabus of lectures or practicals ***/
/*****************************************************************************/

void Syl_LoadListItemsSyllabusIntoMemory (long CrsCod)
  {
   char PathFile[PATH_MAX + 1];
   long PostBeginList;
   unsigned NumItem = 0;
   int N;
   int CodItem[1 + Syl_MAX_LEVELS_SYLLABUS];	// To make numeration
   int Result;
   unsigned NumItemsWithChildren = 0;

   /* Path of the private directory for the XML file with the syllabus */
   snprintf (Gbl.Syllabus.PathDir,sizeof (Gbl.Syllabus.PathDir),
	     "%s/%ld/%s",
	     Cfg_PATH_CRS_PRIVATE,CrsCod,
	     Gbl.Syllabus.WhichSyllabus == Syl_LECTURES ? Cfg_SYLLABUS_FOLDER_LECTURES :
		                                          Cfg_SYLLABUS_FOLDER_PRACTICALS);

   /***** Open the file with the syllabus *****/
   Syl_OpenSyllabusFile (Gbl.Syllabus.PathDir,PathFile);

   /***** Go to the start of the list of items *****/
   if (!Str_FindStrInFile (Gbl.F.XML,"<lista>",Str_NO_SKIP_HTML_COMMENTS))
      Lay_ShowErrorAndExit ("Wrong syllabus format.");

   /***** Save the position of the start of the list *****/
   PostBeginList = ftell (Gbl.F.XML);

   /***** Loop to count the number of items *****/
   for (LstItemsSyllabus.NumItems = 0;
	Str_FindStrInFile (Gbl.F.XML,"<item",Str_NO_SKIP_HTML_COMMENTS);
	LstItemsSyllabus.NumItems++);

   /***** Allocate memory for the list of items *****/
   if ((LstItemsSyllabus.Lst = (struct ItemSyllabus *) calloc (LstItemsSyllabus.NumItems + 1,sizeof (struct ItemSyllabus))) == NULL)
      Lay_NotEnoughMemoryExit ();

   /***** Return to the start of the list *****/
   fseek (Gbl.F.XML,PostBeginList,SEEK_SET);

   for (N = 1;
	N <= Syl_MAX_LEVELS_SYLLABUS;
	N++)
      CodItem[N] = 0;
   LstItemsSyllabus.NumLevels = 1;

   /***** If the syllabus is empty ==> initialize an item to be edited *****/
   if (LstItemsSyllabus.NumItems == 0)
     {
      /* Level of the item */
      LstItemsSyllabus.Lst[0].Level = 1;

      /* Code (numeración) of the item */
      CodItem[1] = 1;
      for (N = 1;
	   N <= Syl_MAX_LEVELS_SYLLABUS;
	   N++)
	 LstItemsSyllabus.Lst[0].CodItem[N] = CodItem[N];

      /* Text of the item  */
      LstItemsSyllabus.Lst[0].Text[0] = '\0';
     }
   else
      /***** Loop to read and store all the items of the syllabus *****/
      for (NumItem = 0;
	   NumItem < LstItemsSyllabus.NumItems;
	   NumItem++)
	{
	 /* Go to the start of the item */
	 if (!Str_FindStrInFile (Gbl.F.XML,"<item",Str_NO_SKIP_HTML_COMMENTS))
	    Lay_ShowErrorAndExit ("Wrong syllabus format.");

	 /* Get the level */
	 LstItemsSyllabus.Lst[NumItem].Level = Syl_ReadLevelItemSyllabus ();
	 if (LstItemsSyllabus.Lst[NumItem].Level > LstItemsSyllabus.NumLevels)
	    LstItemsSyllabus.NumLevels = LstItemsSyllabus.Lst[NumItem].Level;

	 /* Set the code (number) of the item */
	 CodItem[LstItemsSyllabus.Lst[NumItem].Level]++;
	 for (N = LstItemsSyllabus.Lst[NumItem].Level + 1;
	      N <= Syl_MAX_LEVELS_SYLLABUS;
	      N++)
	    CodItem[N] = 0;
	 for (N = 1;
	      N <= Syl_MAX_LEVELS_SYLLABUS;
	      N++)
	    LstItemsSyllabus.Lst[NumItem].CodItem[N] = CodItem[N];

	 /* Get the text of the item */
	 Result = Str_ReadFileUntilBoundaryStr (Gbl.F.XML,LstItemsSyllabus.Lst[NumItem].Text,
	                                        "</item>",strlen ("</item>"),
	                                        (unsigned long long) Syl_MAX_BYTES_TEXT_ITEM);
	 if (Result == 0) // Str too long
	   {
	    if (!Str_FindStrInFile (Gbl.F.XML,"</item>",Str_NO_SKIP_HTML_COMMENTS)) // End the search
	       Lay_ShowErrorAndExit ("Wrong syllabus format.");
	   }
	 else if (Result == -1)
	    Lay_ShowErrorAndExit ("Wrong syllabus format.");
	}

   /***** Close the file with the syllabus *****/
   Fil_CloseXMLFile ();

   /***** Initialize other fields in the list *****/
   if (LstItemsSyllabus.NumItems)
     {
      for (NumItem = 0;
	   NumItem < LstItemsSyllabus.NumItems - 1;
	   NumItem++)
	 if (LstItemsSyllabus.Lst[NumItem].Level < LstItemsSyllabus.Lst[NumItem + 1].Level)
	   {
	    LstItemsSyllabus.Lst[NumItem].HasChildren = true;
	    NumItemsWithChildren++;
	   }
	 else
	    LstItemsSyllabus.Lst[NumItem].HasChildren = false;
      LstItemsSyllabus.Lst[LstItemsSyllabus.NumItems - 1].HasChildren = false;
     }
   LstItemsSyllabus.NumItemsWithChildren = NumItemsWithChildren;
  }

/*****************************************************************************/
/*********************** Free list of items of a syllabus ********************/
/*****************************************************************************/

void Syl_FreeListItemsSyllabus (void)
  {
   if (LstItemsSyllabus.Lst)
     {
      free ((void *) LstItemsSyllabus.Lst);
      LstItemsSyllabus.Lst = NULL;
      LstItemsSyllabus.NumItems = 0;
     }
  }

/*****************************************************************************/
/************* Read the level of the current item in a syllabus **************/
/*****************************************************************************/
// XML file with the syllabus must be positioned after <item
// XML with the syllabus becomes positioned after <item nivel="x">

int Syl_ReadLevelItemSyllabus (void)
  {
   int Level;
   char StrlLevel[11 + 1];

   if (!Str_FindStrInFile (Gbl.F.XML,"nivel=\"",Str_NO_SKIP_HTML_COMMENTS))
      Lay_ShowErrorAndExit ("Wrong syllabus format.");
   if (Str_ReadFileUntilBoundaryStr (Gbl.F.XML,StrlLevel,"\"",1,
   	                             (unsigned long long) (11 + 1)) != 1)
      Lay_ShowErrorAndExit ("Wrong syllabus format.");
   if (sscanf (StrlLevel,"%d",&Level) != 1)
      Lay_ShowErrorAndExit ("Wrong syllabus format.");
   Str_FindStrInFile (Gbl.F.XML,">",Str_NO_SKIP_HTML_COMMENTS);
   if (Level < 1)
      Level = 1;
   else if (Level > Syl_MAX_LEVELS_SYLLABUS)
      Level = Syl_MAX_LEVELS_SYLLABUS;
   return Level;
  }

/*****************************************************************************/
/***************** Show a syllabus of lectures or practicals *****************/
/*****************************************************************************/

static void Syl_ShowSyllabus (bool PutIconToEdit)
  {
   extern const char *Txt_INFO_TITLE[Inf_NUM_INFO_TYPES];
   extern const char *Hlp_COURSE_Syllabus_edit;
   extern const char *Hlp_COURSE_Syllabus;
   unsigned NumItem;
   int i;
   int NumButtons = Gbl.Syllabus.EditionIsActive ? 5 :
	                                           0;
   bool ShowRowInsertNewItem = (Gbl.Action.Act == ActInsIteSylLec || Gbl.Action.Act == ActInsIteSylPra ||
                                Gbl.Action.Act == ActModIteSylLec || Gbl.Action.Act == ActModIteSylPra ||
				Gbl.Action.Act == ActRgtIteSylLec || Gbl.Action.Act == ActRgtIteSylPra ||
                                Gbl.Action.Act == ActLftIteSylLec || Gbl.Action.Act == ActLftIteSylPra);

   /***** Begin box and table *****/
   Box_StartBoxTable (NULL,Txt_INFO_TITLE[Gbl.Crs.Info.Type],
		      PutIconToEdit ? Inf_PutIconToEditInfo :
				      NULL,
		      Gbl.Syllabus.EditionIsActive ? Hlp_COURSE_Syllabus_edit :
						     Hlp_COURSE_Syllabus,
		      Box_NOT_CLOSABLE,0);

   /***** Set width of columns of the table *****/
   fprintf (Gbl.F.Out,"<colgroup>");
   for (i = 0;
	i < NumButtons;
	i++)
      fprintf (Gbl.F.Out,"<col width=\"12\" />");
   for (i = 1;
	i <= LstItemsSyllabus.NumLevels;
	i++)
      fprintf (Gbl.F.Out,"<col width=\"%d\" />",
	       i * Syl_WIDTH_NUM_SYLLABUS);
   fprintf (Gbl.F.Out,"<col width=\"*\" />"
                      "</colgroup>");

   if (LstItemsSyllabus.NumItems)
      /***** Loop writing all items of the syllabus *****/
      for (NumItem = 0;
	   NumItem < LstItemsSyllabus.NumItems;
	   NumItem++)
	{
	 Syl_ShowRowSyllabus (NumItem,
			      LstItemsSyllabus.Lst[NumItem].Level,
			      LstItemsSyllabus.Lst[NumItem].CodItem,
			      LstItemsSyllabus.Lst[NumItem].Text,false);
	 if (ShowRowInsertNewItem && NumItem == Gbl.Syllabus.NumItem)
	    // Mostrar a new row where se puede insert a new item
	    Syl_ShowRowSyllabus (NumItem + 1,
				 LstItemsSyllabus.Lst[NumItem].Level,NULL,
				 "",true);
	}
   else if (Gbl.Syllabus.EditionIsActive)
      /***** If the syllabus is empty ==>
             show form to add a iten to the end *****/
      Syl_ShowRowSyllabus (0,1,LstItemsSyllabus.Lst[0].CodItem,"",true);

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******** Write a row (item) of a syllabus of lectures or practicals *********/
/*****************************************************************************/

static void Syl_ShowRowSyllabus (unsigned NumItem,
                                 int Level,int *CodItem,const char *Text,bool NewItem)
  {
   extern const char *Txt_Move_up_X_and_its_subsections;
   extern const char *Txt_Move_up_X;
   extern const char *Txt_Movement_not_allowed;
   extern const char *Txt_Move_down_X_and_its_subsections;
   extern const char *Txt_Move_down_X;
   extern const char *Txt_Increase_level_of_X;
   extern const char *Txt_Decrease_level_of_X;
   static int LastLevel = 0;
   char StrItemCod[Syl_MAX_LEVELS_SYLLABUS * (10 + 1)];
   struct MoveSubtrees Subtree;

   Subtree.ToGetUp.Ini   = Subtree.ToGetUp.End   = 0;
   Subtree.ToGetDown.Ini = Subtree.ToGetDown.End = 0;
   Subtree.MovAllowed = false;

   Gbl.Syllabus.ParamNumItem = NumItem;	// Used as parameter in forms

   if (!NewItem)	// If the item is new (not stored in file), it has no number
      Syl_WriteNumItem (StrItemCod,NULL,Level,CodItem);

   /***** Start the row *****/
   HTM_TR_Begin (NULL);

   if (Gbl.Syllabus.EditionIsActive)
     {
      if (NewItem)
	{
         HTM_TD_Begin ("colspan=\"5\" class=\"COLOR%u\"",Gbl.RowEvenOdd);
         HTM_TD_End ();
	}
      else
	{
	 /***** Icon to remove the row *****/
         HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
	 if (LstItemsSyllabus.Lst[NumItem].HasChildren)
            Ico_PutIconRemovalNotAllowed ();
	 else
	   {
	    Frm_StartForm (Gbl.Crs.Info.Type == Inf_LECTURES ? ActDelItmSylLec :
		                                                      ActDelItmSylPra);
	    Syl_PutParamNumItem ();
            Ico_PutIconRemove ();
            Frm_EndForm ();
	   }
         HTM_TD_End ();

	 /***** Icon to get up an item *****/
	 Syl_CalculateUpSubtreeSyllabus (&Subtree,NumItem);
	 HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
	 if (Subtree.MovAllowed)
	   {
            snprintf (Gbl.Title,sizeof (Gbl.Title),
                      LstItemsSyllabus.Lst[NumItem].HasChildren ? Txt_Move_up_X_and_its_subsections :
                                                                  Txt_Move_up_X,
                      StrItemCod);
	    Lay_PutContextualLinkOnlyIcon (Gbl.Crs.Info.Type == Inf_LECTURES ? ActUp_IteSylLec :
										      ActUp_IteSylPra,
					   NULL,Syl_PutParamNumItem,
					   "arrow-up.svg",
					   Gbl.Title);
	   }
	 else
            Ico_PutIconOff ("arrow-up.svg",Txt_Movement_not_allowed);
         HTM_TD_End ();

	 /***** Icon to get down item *****/
	 Syl_CalculateDownSubtreeSyllabus (&Subtree,NumItem);
	 HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
	 if (Subtree.MovAllowed)
	   {
            snprintf (Gbl.Title,sizeof (Gbl.Title),
                      LstItemsSyllabus.Lst[NumItem].HasChildren ? Txt_Move_down_X_and_its_subsections :
                                                                  Txt_Move_down_X,
                      StrItemCod);
	    Lay_PutContextualLinkOnlyIcon (Gbl.Crs.Info.Type == Inf_LECTURES ? ActDwnIteSylLec :
										      ActDwnIteSylPra,
					   NULL,Syl_PutParamNumItem,
					   "arrow-down.svg",
					   Gbl.Title);
	   }
	 else
            Ico_PutIconOff ("arrow-down.svg",Txt_Movement_not_allowed);
         HTM_TD_End ();

	 /***** Icon to increase the level of an item *****/
	 HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
	 if (Level > 1)
	   {
	    snprintf (Gbl.Title,sizeof (Gbl.Title),
		      Txt_Increase_level_of_X,
		      StrItemCod);
	    Lay_PutContextualLinkOnlyIcon (Gbl.Crs.Info.Type == Inf_LECTURES ? ActRgtIteSylLec :
										      ActRgtIteSylPra,
					   NULL,Syl_PutParamNumItem,
					   "arrow-left.svg",
					   Gbl.Title);
	   }
	 else
            Ico_PutIconOff ("arrow-left.svg",Txt_Movement_not_allowed);
         HTM_TD_End ();

	 /***** Icon to decrease level item *****/
	 HTM_TD_Begin ("class=\"BM%u\"",Gbl.RowEvenOdd);
	 if (Level < LastLevel + 1 &&
	     Level < Syl_MAX_LEVELS_SYLLABUS)
	   {
	    snprintf (Gbl.Title,sizeof (Gbl.Title),
		      Txt_Decrease_level_of_X,
		      StrItemCod);
	    Lay_PutContextualLinkOnlyIcon (Gbl.Crs.Info.Type == Inf_LECTURES ? ActLftIteSylLec :
										      ActLftIteSylPra,
					   NULL,Syl_PutParamNumItem,
					   "arrow-right.svg",
					   Gbl.Title);
	   }
	 else
            Ico_PutIconOff ("arrow-right.svg",Txt_Movement_not_allowed);
         HTM_TD_End ();

	 LastLevel = Level;
	}
     }

   if (Gbl.Syllabus.EditionIsActive)
      Syl_PutFormItemSyllabus (NewItem,NumItem,Level,CodItem,Text);
   else
     {
      /***** Indent depending on the level *****/
      if (Level > 1)
	{
	 HTM_TD_Begin ("colspan=\"%d\" class=\"COLOR%u\"",Level - 1,Gbl.RowEvenOdd);
         HTM_TD_End ();
	}

      /***** Code of the item *****/
      HTM_TD_Begin ("class=\"%s RT COLOR%u\" style=\"width:%dpx;\"",
		    StyleSyllabus[Level],Gbl.RowEvenOdd,
		    Level * Syl_WIDTH_NUM_SYLLABUS);
      if (Level == 1)
	 fprintf (Gbl.F.Out,"&nbsp;");
      fprintf (Gbl.F.Out,"%s&nbsp;",StrItemCod);
      HTM_TD_End ();

      /***** Text of the item *****/
      HTM_TD_Begin ("colspan=\"%d\" class=\"%s LT COLOR%u\"",
		    LstItemsSyllabus.NumLevels - Level + 1,
		    StyleSyllabus[Level],
		    Gbl.RowEvenOdd);
      fprintf (Gbl.F.Out,"%s",Text);
      HTM_TD_End ();
     }

   /***** End of the row *****/
   HTM_TR_End ();

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/************** Write the syllabus into a temporary HTML file ****************/
/*****************************************************************************/
// This function is called only from web service

int Syl_WriteSyllabusIntoHTMLBuffer (char **HTMLBuffer)
  {
   char FileNameHTMLTmp[PATH_MAX + 1];
   FILE *FileHTMLTmp;
   size_t Length;

   /***** Initialize buffer *****/
   *HTMLBuffer = NULL;

   /***** Load syllabus from XML file to list of items in memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (Gbl.Hierarchy.Crs.CrsCod);

   if (LstItemsSyllabus.NumItems)
     {
      /***** Create a unique name for the file *****/
      snprintf (FileNameHTMLTmp,sizeof (FileNameHTMLTmp),
	        "%s/%s_syllabus.html",
	        Cfg_PATH_OUT_PRIVATE,Gbl.UniqueNameEncrypted);

      /***** Create a new temporary file for writing and reading *****/
      if ((FileHTMLTmp = fopen (FileNameHTMLTmp,"w+b")) == NULL)
	{
         Syl_FreeListItemsSyllabus ();
         return soap_receiver_fault (Gbl.soap,
                                     "Syllabus can not be copied into buffer",
                                     "Can not create temporary file");
	}

      /***** Write syllabus in HTML into a temporary file *****/
      Syl_WriteSyllabusIntoHTMLTmpFile (FileHTMLTmp);

      /***** Write syllabus from list of items in memory to text buffer *****/
      /* Compute length of file */
      Length = (size_t) ftell (FileHTMLTmp);

      /* Allocate memory for buffer */
      if ((*HTMLBuffer = (char *) malloc (Length + 1)) == NULL)
	{
	 fclose (FileHTMLTmp);
	 unlink (FileNameHTMLTmp);
         Syl_FreeListItemsSyllabus ();
         return soap_receiver_fault (Gbl.soap,
                                     "Syllabus can not be copied into buffer",
                                     "Not enough memory for buffer");
	}

      /* Copy file content into buffer */
      fseek (FileHTMLTmp,0L,SEEK_SET);
      if (fread ((void *) *HTMLBuffer,sizeof (char),Length,FileHTMLTmp) != Length)
	{
	 fclose (FileHTMLTmp);
	 unlink (FileNameHTMLTmp);
         Syl_FreeListItemsSyllabus ();
         return soap_receiver_fault (Gbl.soap,
                                     "Syllabus can not be copied into buffer",
                                     "Error reading file into buffer");
	}
      (*HTMLBuffer)[Length] = '\0';

      /***** Close and remove temporary file *****/
      fclose (FileHTMLTmp);
      unlink (FileNameHTMLTmp);
     }

   /***** Free list of items *****/
   Syl_FreeListItemsSyllabus ();

   return SOAP_OK;
  }

/*****************************************************************************/
/************** Write the syllabus into a temporary HTML file ****************/
/*****************************************************************************/

static void Syl_WriteSyllabusIntoHTMLTmpFile (FILE *FileHTMLTmp)
  {
   extern const char *Txt_INFO_TITLE[Inf_NUM_INFO_TYPES];
   unsigned NumItem;
   int i;

   /***** Write start of HTML code *****/
   Lay_StartHTMLFile (FileHTMLTmp,Txt_INFO_TITLE[Gbl.Crs.Info.Type]);
   fprintf (FileHTMLTmp,"<body>\n"
                        "<table>\n");

   /***** Set width of columns of the table *****/
   fprintf (FileHTMLTmp,"<colgroup>\n");
   for (i = 1;
	i <= LstItemsSyllabus.NumLevels;
	i++)
      fprintf (FileHTMLTmp,"<col width=\"%d\" />\n",
	       i * Syl_WIDTH_NUM_SYLLABUS);
   fprintf (FileHTMLTmp,"<col width=\"*\" />\n"
			"</colgroup>\n");

   /***** Write all items of the current syllabus into text buffer *****/
   for (NumItem = 0;
	NumItem < LstItemsSyllabus.NumItems;
	NumItem++)
     {
      /***** Start the row *****/
      fprintf (FileHTMLTmp,"<tr>");

      /***** Indent depending on the level *****/
      if (LstItemsSyllabus.Lst[NumItem].Level > 1)
	 fprintf (FileHTMLTmp,"<td colspan=\"%d\"></td>",
		  LstItemsSyllabus.Lst[NumItem].Level - 1);

      /***** Code of the item *****/
      fprintf (FileHTMLTmp,"<td class=\"%s RT\" style=\"width:%dpx;\">",
	       StyleSyllabus[LstItemsSyllabus.Lst[NumItem].Level],
	       LstItemsSyllabus.Lst[NumItem].Level * Syl_WIDTH_NUM_SYLLABUS);
      if (LstItemsSyllabus.Lst[NumItem].Level == 1)
	 fprintf (FileHTMLTmp,"&nbsp;");
      Syl_WriteNumItem (NULL,FileHTMLTmp,
			LstItemsSyllabus.Lst[NumItem].Level,
			LstItemsSyllabus.Lst[NumItem].CodItem);
      fprintf (FileHTMLTmp,"&nbsp;</td>");

      /***** Text of the item *****/
      fprintf (FileHTMLTmp,"<td colspan=\"%d\" class=\"%s LT\">"
			   "%s"
			   "</td>",
	       LstItemsSyllabus.NumLevels - LstItemsSyllabus.Lst[NumItem].Level + 1,
	       StyleSyllabus[LstItemsSyllabus.Lst[NumItem].Level],
	       LstItemsSyllabus.Lst[NumItem].Text);

      /***** End of the row *****/
      fprintf (FileHTMLTmp,"</tr>\n");
     }

   fprintf (FileHTMLTmp,"</table>\n"
			"</html>\n"
			"</body>\n");
  }

/*****************************************************************************/
/*************** Show a form to modify an item of the syllabus ***************/
/*****************************************************************************/

static void Syl_PutFormItemSyllabus (bool NewItem,unsigned NumItem,int Level,int *CodItem,const char *Text)
  {
   extern const char *Txt_Enter_a_new_item_here;

   if (Level < 1)
      Level = 1;

   /***** Indent depending on the level *****/
   if (Level > 1)
     {
      HTM_TD_Begin ("colspan=\"%d\" class=\"COLOR%u\"",Level - 1,Gbl.RowEvenOdd);
      HTM_TD_End ();
     }

   /***** Write the code of the item *****/
   if (NewItem)	// If the item is new (not stored in the file) ==> it has not a number
     {
      HTM_TD_Begin ("class=\"COLOR%u\" style=\"width:%dpx;\"",
	            Gbl.RowEvenOdd,Level * Syl_WIDTH_NUM_SYLLABUS);
      HTM_TD_End ();
     }
   else
     {
      HTM_TD_Begin ("class=\"%s LM COLOR%u\" style=\"width:%dpx;\"",
		    StyleSyllabus[Level],Gbl.RowEvenOdd,
		    Level * Syl_WIDTH_NUM_SYLLABUS);
      if (Level == 1)
	 fprintf (Gbl.F.Out,"&nbsp;");
      Syl_WriteNumItem (NULL,Gbl.F.Out,Level,CodItem);
      fprintf (Gbl.F.Out,"&nbsp;");
      HTM_TD_End ();
     }

   /***** Text of the item *****/
   HTM_TD_Begin ("colspan=\"%d\" class=\"LM COLOR%u\"",
		 LstItemsSyllabus.NumLevels - Level + 1,Gbl.RowEvenOdd);
   Frm_StartForm (NewItem ? (Gbl.Crs.Info.Type == Inf_LECTURES ? ActInsIteSylLec :
	                                                                ActInsIteSylPra) :
                            (Gbl.Crs.Info.Type == Inf_LECTURES ? ActModIteSylLec :
                        	                                        ActModIteSylPra));
   Gbl.Syllabus.ParamNumItem = NumItem;
   Syl_PutParamNumItem ();
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Txt\""
	              " size=\"60\" maxlength=\"%u\" value=\"%s\""
                      " placeholder=\"%s\"",
	    Syl_MAX_CHARS_TEXT_ITEM,Text,
	    Txt_Enter_a_new_item_here);
   if (NewItem)
      fprintf (Gbl.F.Out," autofocus=\"autofocus\"");
   fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\" />",
	    Gbl.Form.Id);
   Frm_EndForm ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/***** Write parameter with the number of an item in a syllabus form *********/
/*****************************************************************************/

static void Syl_PutParamNumItem (void)
  {
   Par_PutHiddenParamUnsigned ("NumI",Gbl.Syllabus.ParamNumItem);
  }

/*****************************************************************************/
/******************** Write number of item in legal style ********************/
/*****************************************************************************/

static void Syl_WriteNumItem (char *StrDst,FILE *FileTgt,int Level,int *CodItem)
  {
   int N;
   char InStr[1 + 10 + 1];

   if (StrDst)
      StrDst[0] = '\0';

   for (N = 1;
	N <= Level;
	N++)
     {
      if (N > 1)
	{
	 if (StrDst)
	    Str_Concat (StrDst,".",
	                Syl_MAX_BYTES_ITEM_COD);
	 if (FileTgt)
	    fprintf (FileTgt,".");
	}
      snprintf (InStr,sizeof (InStr),
	        "%d",
		CodItem[N]);
      if (StrDst)
	 Str_Concat (StrDst,InStr,
	             Syl_MAX_BYTES_ITEM_COD);
      if (FileTgt)
	 fprintf (FileTgt,"%s",InStr);
     }
  }

/*****************************************************************************/
/********************** Remove an item from syllabus *************************/
/*****************************************************************************/

void Syl_RemoveItemSyllabus (void)
  {
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;
   unsigned NumItem;

   /***** Set syllabus type depending on current action *****/
   Syl_SetSyllabusTypeFromAction ();

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (Gbl.Hierarchy.Crs.CrsCod);

   Gbl.Syllabus.EditionIsActive = true;

   /***** Get item number *****/
   Syl_GetParamItemNumber ();

   /***** Create a new file to make the update *****/
   Syl_BuildPathFileSyllabus (PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   /***** Create the new XML file *****/
   Syl_WriteStartFileSyllabus (NewFile);
   for (NumItem = 0;
	NumItem < LstItemsSyllabus.NumItems;
	NumItem++)
      if (NumItem != Gbl.Syllabus.NumItem)
	 Syl_WriteItemFileSyllabus (NewFile,LstItemsSyllabus.Lst[NumItem].Level,LstItemsSyllabus.Lst[NumItem].Text);
   Syl_WriteEndFileSyllabus (NewFile);

   /***** Close the files *****/
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

   /***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****/
   Inf_SetInfoSrcIntoDB (LstItemsSyllabus.NumItems ? Inf_INFO_SRC_EDITOR :
   	                                             Inf_INFO_SRC_NONE);

   /***** Show the updated syllabus to continue editing it *****/
   Syl_FreeListItemsSyllabus ();
   Syl_EditSyllabus ();
  }

/*****************************************************************************/
/*** Get up position of an item of the syllabus of lectures or prácticals ****/
/*****************************************************************************/

void Syl_UpItemSyllabus (void)
  {
   Syl_ChangePlaceItemSyllabus (Syl_GET_UP);
  }

/*****************************************************************************/
/** Get down position of an item of the syllabus of lectures or practicals ***/
/*****************************************************************************/

void Syl_DownItemSyllabus (void)
  {
   Syl_ChangePlaceItemSyllabus (Syl_GET_DOWN);
  }

/*****************************************************************************/
/*************** Get up or get down a subtree of a syllabus ******************/
/*****************************************************************************/

void Syl_ChangePlaceItemSyllabus (Syl_ChangePosItem_t UpOrDownPos)
  {
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;
   unsigned NumItem;
   struct MoveSubtrees Subtree;

   /***** Set syllabus type depending on current action *****/
   Syl_SetSyllabusTypeFromAction ();

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (Gbl.Hierarchy.Crs.CrsCod);

   Gbl.Syllabus.EditionIsActive = true;

   /***** Get item number *****/
   Syl_GetParamItemNumber ();

   Subtree.ToGetUp.Ini   = Subtree.ToGetUp.End   = 0;
   Subtree.ToGetDown.Ini = Subtree.ToGetDown.End = 0;
   Subtree.MovAllowed = false;

   if (Gbl.Syllabus.NumItem < LstItemsSyllabus.NumItems)
     {
      /***** Create a new file where make the update *****/
      Syl_BuildPathFileSyllabus (PathFile);
      Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

      /***** Get up or get down position *****/
      switch (UpOrDownPos)
	{
	 case Syl_GET_UP:
	    Syl_CalculateUpSubtreeSyllabus (&Subtree,Gbl.Syllabus.NumItem);
	    break;
	 case Syl_GET_DOWN:
	    Syl_CalculateDownSubtreeSyllabus (&Subtree,Gbl.Syllabus.NumItem);
	    break;
	}

      /***** Create the new XML file *****/
      Syl_WriteStartFileSyllabus (NewFile);
      if (Subtree.MovAllowed)
	{
	 for (NumItem = 0;
	      NumItem < Subtree.ToGetDown.Ini;
	      NumItem++)
	    Syl_WriteItemFileSyllabus (NewFile,LstItemsSyllabus.Lst[NumItem].Level,LstItemsSyllabus.Lst[NumItem].Text);
	 for (NumItem = Subtree.ToGetUp.Ini;
	      NumItem <= Subtree.ToGetUp.End;
	      NumItem++)
	    Syl_WriteItemFileSyllabus (NewFile,LstItemsSyllabus.Lst[NumItem].Level,LstItemsSyllabus.Lst[NumItem].Text);
	 for (NumItem = Subtree.ToGetDown.Ini;
	      NumItem <= Subtree.ToGetDown.End;
	      NumItem++)
	    Syl_WriteItemFileSyllabus (NewFile,LstItemsSyllabus.Lst[NumItem].Level,LstItemsSyllabus.Lst[NumItem].Text);
	 for (NumItem = Subtree.ToGetUp.End + 1;
	      NumItem < LstItemsSyllabus.NumItems;
	      NumItem++)
	    Syl_WriteItemFileSyllabus (NewFile,LstItemsSyllabus.Lst[NumItem].Level,LstItemsSyllabus.Lst[NumItem].Text);
	}
      else
	 Syl_WriteAllItemsFileSyllabus (NewFile);
      Syl_WriteEndFileSyllabus (NewFile);

      /***** Close the files *****/
      Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);
     }

   /***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****/
   Inf_SetInfoSrcIntoDB (LstItemsSyllabus.NumItems ? Inf_INFO_SRC_EDITOR :
						     Inf_INFO_SRC_NONE);

   /***** Show the updated syllabus to continue editing it *****/
   Syl_FreeListItemsSyllabus ();
   Syl_EditSyllabus ();
  }

/*****************************************************************************/
/********** Compute the limits for get up a subtree of a syllabus ************/
/*****************************************************************************/
// If return Subtree->MovAllowed = false, the limits become undefined

void Syl_CalculateUpSubtreeSyllabus (struct MoveSubtrees *Subtree,unsigned NumItem)
  {
   int Level = LstItemsSyllabus.Lst[NumItem].Level;

   if (NumItem == 0)
      Subtree->MovAllowed = false;
   else	// NumItem > 0
     {
      /***** Compute limits of the subtree to get up *****/
      Subtree->ToGetUp.Ini = NumItem;
      /* Search down the end of the full subtree to get up */
      for (Subtree->ToGetUp.End = NumItem + 1;
	   Subtree->ToGetUp.End < LstItemsSyllabus.NumItems;
	   Subtree->ToGetUp.End++)
	 if (LstItemsSyllabus.Lst[Subtree->ToGetUp.End].Level <= Level)
	   {
	    Subtree->ToGetUp.End--;
	    break;
	   }
      if (Subtree->ToGetUp.End == LstItemsSyllabus.NumItems)
	 Subtree->ToGetUp.End = LstItemsSyllabus.NumItems - 1;

      /***** Compute limits of the subtree to get down *****/
      Subtree->ToGetDown.End = NumItem - 1;
      if (LstItemsSyllabus.Lst[Subtree->ToGetDown.End].Level < Level)
	 Subtree->MovAllowed = false;
      else
	{
	 Subtree->MovAllowed = true;
	 /* Find backwards the start of the subtree to get down */
	 for (Subtree->ToGetDown.Ini = Subtree->ToGetDown.End;
	      Subtree->ToGetDown.Ini > 0;
	      Subtree->ToGetDown.Ini--)
	    if (LstItemsSyllabus.Lst[Subtree->ToGetDown.Ini].Level <= Level)
	       break;
	}
     }
  }

/*****************************************************************************/
/****** Compute the limits for the get down of a subtree of a syllabus *******/
/*****************************************************************************/
// When return Subtree->MovAllowed equal to false, the limits become undefined

void Syl_CalculateDownSubtreeSyllabus (struct MoveSubtrees *Subtree,unsigned NumItem)
  {
   int Level = LstItemsSyllabus.Lst[NumItem].Level;

   /***** Compute limits of the subtree to get down *****/
   Subtree->ToGetDown.Ini = NumItem;
   /* Search down the end of the full subtree to get down */
   for (Subtree->ToGetDown.End = NumItem + 1;
	Subtree->ToGetDown.End < LstItemsSyllabus.NumItems;
	Subtree->ToGetDown.End++)
      if (LstItemsSyllabus.Lst[Subtree->ToGetDown.End].Level <= Level)
	{
	 Subtree->ToGetDown.End--;
	 break;
	}
   if (Subtree->ToGetDown.End >= LstItemsSyllabus.NumItems - 1)
      Subtree->MovAllowed = false;
   else
     {
      /***** Compute limits of the subtree to get up *****/
      Subtree->ToGetUp.Ini = Subtree->ToGetDown.End + 1;
      if (LstItemsSyllabus.Lst[Subtree->ToGetUp.Ini].Level < Level)
	 Subtree->MovAllowed = false;
      else
	{
	 Subtree->MovAllowed = true;
	 /* Find downwards the end of the subtree to get up */
	 for (Subtree->ToGetUp.End = Subtree->ToGetUp.Ini + 1;
	      Subtree->ToGetUp.End < LstItemsSyllabus.NumItems;
	      Subtree->ToGetUp.End++)
	    if (LstItemsSyllabus.Lst[Subtree->ToGetUp.End].Level <= Level)
	      {
	       Subtree->ToGetUp.End--;
	       break;
	      }
	 if (Subtree->ToGetUp.End == LstItemsSyllabus.NumItems)
	    Subtree->ToGetUp.End = LstItemsSyllabus.NumItems - 1;
	}
     }
  }

/*****************************************************************************/
/** Increase the level of an item of the syllabus of lectures or practicals **/
/*****************************************************************************/

void Syl_RightItemSyllabus (void)
  {
   Syl_ChangeLevelItemSyllabus (Syl_INCREASE_LEVEL);
  }

/*****************************************************************************/
/** Decrease the level of an item of the syllabus of lectures or practicals **/
/*****************************************************************************/

void Syl_LeftItemSyllabus (void)
  {
   Syl_ChangeLevelItemSyllabus (Syl_DECREASE_LEVEL);
  }

/*****************************************************************************/
/********* Increase or decrease the level of an item of a syllabus ***********/
/*****************************************************************************/

void Syl_ChangeLevelItemSyllabus (Syl_ChangeLevelItem_t IncreaseOrDecreaseLevel)
  {
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;

   /***** Set syllabus type depending on current action *****/
   Syl_SetSyllabusTypeFromAction ();

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (Gbl.Hierarchy.Crs.CrsCod);

   Gbl.Syllabus.EditionIsActive = true;

   /***** Get item number *****/
   Syl_GetParamItemNumber ();

   /***** Create a new file to do the update *****/
   Syl_BuildPathFileSyllabus (PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   /***** Increase or decrease level *****/
   switch (IncreaseOrDecreaseLevel)
     {
      case Syl_INCREASE_LEVEL:
	 if (LstItemsSyllabus.Lst[Gbl.Syllabus.NumItem].Level > 1)
	    LstItemsSyllabus.Lst[Gbl.Syllabus.NumItem].Level--;
	 break;
      case Syl_DECREASE_LEVEL:
	 if (LstItemsSyllabus.Lst[Gbl.Syllabus.NumItem].Level < Syl_MAX_LEVELS_SYLLABUS)
	    LstItemsSyllabus.Lst[Gbl.Syllabus.NumItem].Level++;
	 break;
     }

   /***** Create the new XML file *****/
   Syl_WriteStartFileSyllabus (NewFile);
   Syl_WriteAllItemsFileSyllabus (NewFile);
   Syl_WriteEndFileSyllabus (NewFile);

   /***** Close the files *****/
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

   /***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****/
   Inf_SetInfoSrcIntoDB (LstItemsSyllabus.NumItems ? Inf_INFO_SRC_EDITOR :
   	                                             Inf_INFO_SRC_NONE);

   /***** Show the updated syllabus to continue editing it *****/
   Syl_FreeListItemsSyllabus ();
   Syl_EditSyllabus ();
  }

/*****************************************************************************/
/************************ Insert an item in a syllabus ***********************/
/*****************************************************************************/

void Syl_InsertItemSyllabus (void)
  {
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;
   unsigned NumItem;
   char Txt[Syl_MAX_BYTES_TEXT_ITEM + 1];

   /***** Set syllabus type depending on current action *****/
   Syl_SetSyllabusTypeFromAction ();

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (Gbl.Hierarchy.Crs.CrsCod);

   Gbl.Syllabus.EditionIsActive = true;

   /***** Get item number *****/
   Syl_GetParamItemNumber ();

   /***** Get item body *****/
   Par_GetParToHTML ("Txt",Txt,Syl_MAX_BYTES_TEXT_ITEM);

   /***** Create a new file to do the update *****/
   Syl_BuildPathFileSyllabus (PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   /***** Create the new XML file *****/
   Syl_WriteStartFileSyllabus (NewFile);

   /* Write items before the one to be inserted */
   for (NumItem = 0;
	NumItem < Gbl.Syllabus.NumItem;
	NumItem++)
      Syl_WriteItemFileSyllabus (NewFile,LstItemsSyllabus.Lst[NumItem].Level,LstItemsSyllabus.Lst[NumItem].Text);

   /* Write the item that will be inserted */
   Syl_WriteItemFileSyllabus (NewFile,
                              NumItem ? LstItemsSyllabus.Lst[NumItem - 1].Level :
                        	        1,
                              Txt);

   /* Write items after the one just inserted */
   for (;
	NumItem < LstItemsSyllabus.NumItems;
	NumItem++)
      Syl_WriteItemFileSyllabus (NewFile,LstItemsSyllabus.Lst[NumItem].Level,LstItemsSyllabus.Lst[NumItem].Text);

   Syl_WriteEndFileSyllabus (NewFile);

   /***** Close the files *****/
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

   /***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****/
   Inf_SetInfoSrcIntoDB (LstItemsSyllabus.NumItems ? Inf_INFO_SRC_EDITOR :
   	                                             Inf_INFO_SRC_NONE);

   /***** Show the updated syllabus to continue editing it *****/
   Syl_FreeListItemsSyllabus ();
   Syl_EditSyllabus ();
  }

/*****************************************************************************/
/**************** Modify and existing item of the syllabus *******************/
/*****************************************************************************/

void Syl_ModifyItemSyllabus (void)
  {
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;

   /***** Set syllabus type depending on current action *****/
   Syl_SetSyllabusTypeFromAction ();

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (Gbl.Hierarchy.Crs.CrsCod);

   Gbl.Syllabus.EditionIsActive = true;

   /***** Get item number *****/
   Syl_GetParamItemNumber ();

   /***** Get item body *****/
   Par_GetParToHTML ("Txt",LstItemsSyllabus.Lst[Gbl.Syllabus.NumItem].Text,
                     Syl_MAX_BYTES_TEXT_ITEM);

   /***** Create a new file where make the update *****/
   Syl_BuildPathFileSyllabus (PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   /***** Create the new XML file *****/
   Syl_WriteStartFileSyllabus (NewFile);
   Syl_WriteAllItemsFileSyllabus (NewFile);
   Syl_WriteEndFileSyllabus (NewFile);

   /***** Close the files *****/
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

   /***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****/
   Inf_SetInfoSrcIntoDB (LstItemsSyllabus.NumItems ? Inf_INFO_SRC_EDITOR :
   	                                             Inf_INFO_SRC_NONE);

   /***** Show the updated syllabus to continue editing it *****/
   Syl_FreeListItemsSyllabus ();
   Syl_EditSyllabus ();
  }

/*****************************************************************************/
/************ Build the path of the file XML with the syllabus ***************/
/*****************************************************************************/

void Syl_BuildPathFileSyllabus (char *PathFile)
  {
   char Path[PATH_MAX + 1 + NAME_MAX + 1];

   snprintf (Path,sizeof (Path),
	     "%s/%s",
	     Gbl.Syllabus.PathDir,Cfg_SYLLABUS_FILENAME);
   Str_Copy (PathFile,Path,
	     PATH_MAX);
  }

/*****************************************************************************/
/*********************** Open file with the syllabus *************************/
/*****************************************************************************/

void Syl_OpenSyllabusFile (const char *PathDir,char *PathFile)
  {
   if (Gbl.F.XML == NULL) // If it's not open in this moment...
     {
      /* If the directory does not exist, create it */
      Fil_CreateDirIfNotExists (PathDir);

      /* Open the file for reading */
      Syl_BuildPathFileSyllabus (PathFile);
      if ((Gbl.F.XML = fopen (PathFile,"rb")) == NULL)
	{
	 /* Can't open the file */
	 if (!Fil_CheckIfPathExists (PathDir)) // Strange error, since it is just created
	    Lay_ShowErrorAndExit ("Can not open syllabus file.");
	 else
	   {
	    /* Create a new empty syllabus */
	    if ((Gbl.F.XML = fopen (PathFile,"wb")) == NULL)
	       Lay_ShowErrorAndExit ("Can not create syllabus file.");
	    Syl_WriteStartFileSyllabus (Gbl.F.XML);
	    Syl_WriteEndFileSyllabus (Gbl.F.XML);
	    Fil_CloseXMLFile ();
	    /* Open of new the file for reading */
	    if ((Gbl.F.XML = fopen (PathFile,"rb")) == NULL)
	       Lay_ShowErrorAndExit ("Can not open syllabus file.");
	   }
	}
     }
   else  // Go to the start of the file
      rewind (Gbl.F.XML);
  }

/*****************************************************************************/
/*********************** Write a file with a syllabus ************************/
/*****************************************************************************/

void Syl_WriteStartFileSyllabus (FILE *FileSyllabus)
  {
   extern const char *Txt_NEW_LINE;

   XML_WriteStartFile (FileSyllabus,"temario",true);
   fprintf (FileSyllabus,"<lista>%s",Txt_NEW_LINE);
  }

void Syl_WriteAllItemsFileSyllabus (FILE *FileSyllabus)
  {
   unsigned NumItem;

   for (NumItem = 0;
	NumItem < LstItemsSyllabus.NumItems;
	NumItem++)
      Syl_WriteItemFileSyllabus (FileSyllabus,LstItemsSyllabus.Lst[NumItem].Level,LstItemsSyllabus.Lst[NumItem].Text);
  }

void Syl_WriteItemFileSyllabus (FILE *FileSyllabus,int Level,const char *Text)
  {
   extern const char *Txt_NEW_LINE;

   fprintf (FileSyllabus,"<item nivel=\"%d\">%s</item>%s",Level,Text,Txt_NEW_LINE);
  }

void Syl_WriteEndFileSyllabus (FILE *FileSyllabus)
  {
   extern const char *Txt_NEW_LINE;

   fprintf (FileSyllabus,"</lista>%s",Txt_NEW_LINE);
   XML_WriteEndFile (FileSyllabus,"temario");
  }
