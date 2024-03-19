// swad_syllabus.c: syllabus

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free ()
#include <stdsoap2.h>		// For SOAP_OK and soap functions
#include <string.h>		// For string functions
#include <time.h>		// For time ()
#include <unistd.h>		// For SEEK_SET

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_changelog.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_info_database.h"
#include "swad_parameter.h"
#include "swad_string.h"
#include "swad_xml.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

Syl_WhichSyllabus_t Syl_WhichSyllabus[Syl_NUM_WHICH_SYLLABUS] =
  {
   [Syl_NONE      ] = Syl_NONE,
   [Syl_LECTURES  ] = Syl_LECTURES,
   [Syl_PRACTICALS] = Syl_PRACTICALS
  };

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

static const char *ClassSyllabus[1 + Syl_MAX_LEVELS_SYLLABUS] =
  {
   [ 0] = "",
   [ 1] = "SYL1",
   [ 2] = "SYL2",
   [ 3] = "SYL3",
   [ 4] = "SYL3",
   [ 5] = "SYL3",
   [ 6] = "SYL3",
   [ 7] = "SYL3",
   [ 8] = "SYL3",
   [ 9] = "SYL3",
   [10] = "SYL3",
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

struct LstItemsSyllabus Syl_LstItemsSyllabus;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static unsigned Syl_GetParItemNumber (void);

static void Syl_ShowSyllabus (struct Syl_Syllabus *Syllabus);
static void Syl_ShowRowSyllabus (struct Syl_Syllabus *Syllabus,unsigned NumItem,
                                 int Level,int *CodItem,const char *Text,bool NewItem);
static void Syl_PutFormItemSyllabus (struct Syl_Syllabus *Syllabus,
                                     bool NewItem,unsigned NumItem,int Level,int *CodItem,const char *Text);
static void Syl_PutParsSyllabus (void *Syllabus);

static void Syl_WriteNumItem (char *StrDst,FILE *FileTgt,int Level,int *CodItem);

static void Syl_ChangePlaceItemSyllabus (Syl_ChangePosItem_t UpOrDownPos);
static void Syl_ChangeLevelItemSyllabus (Syl_ChangeLevelItem_t IncreaseOrDecreaseLevel);

static void Syl_OpenSyllabusFile (const struct Syl_Syllabus *Syllabus,
                                  char PathFile[PATH_MAX + 1],
                                  FILE **XML);
static void Syl_CloseXMLFile (FILE **XML);

/*****************************************************************************/
/************************** Reset syllabus context ***************************/
/*****************************************************************************/

void Syl_ResetSyllabus (struct Syl_Syllabus *Syllabus)
  {
   Syllabus->PathDir[0] = '\0';
   Syllabus->NumItem = 0;
   Syllabus->ViewType = Vie_VIEW;
   Syllabus->WhichSyllabus = Syl_DEFAULT_WHICH_SYLLABUS;
  }

/*****************************************************************************/
/************* Get parameter about which syllabus I want to see **************/
/*****************************************************************************/

Syl_WhichSyllabus_t Syl_GetParWhichSyllabus (void)
  {
   /***** Get which syllabus I want to see *****/
   return (Syl_WhichSyllabus_t)
	  Par_GetParUnsignedLong ("WhichSyllabus",
				  0,
				  Syl_NUM_WHICH_SYLLABUS - 1,
				  (unsigned long) Syl_DEFAULT_WHICH_SYLLABUS);
  }

/*****************************************************************************/
/****************** Put parameter with type of syllabus **********************/
/*****************************************************************************/

void Syl_PutParWhichSyllabus (void *SyllabusSelected)
  {
   if (SyllabusSelected)
      if (*((Syl_WhichSyllabus_t *) SyllabusSelected) != Syl_NONE)
         Par_PutParUnsigned (NULL,"WhichSyllabus",
                             (unsigned) *((Syl_WhichSyllabus_t *) SyllabusSelected));
  }

/*****************************************************************************/
/************************ Write form to select syllabus **********************/
/*****************************************************************************/

void Syl_PutFormWhichSyllabus (Syl_WhichSyllabus_t WhichSyllabus)
  {
   extern const char *Txt_SYLLABUS_WHICH_SYLLABUS[Syl_NUM_WHICH_SYLLABUS];
   Syl_WhichSyllabus_t WhichSyl;

   /***** If no syllabus ==> nothing to do *****/
   switch (Gbl.Crs.Info.Type)
     {
      case Inf_LECTURES:
      case Inf_PRACTICALS:
	 break;
      default:	// Nothing to do
	 return;
     }

   /***** Form to select which syllabus I want to see (lectures/practicals) *****/
   Frm_BeginForm (ActSeeSyl);
      HTM_DIV_Begin ("class=\"SEL_BELOW_TITLE DAT_%s\"",The_GetSuffix ());
	 HTM_UL_Begin (NULL);

	 for (WhichSyl  = (Syl_WhichSyllabus_t) 1;
	      WhichSyl <= (Syl_WhichSyllabus_t) (Syl_NUM_WHICH_SYLLABUS - 1);
	      WhichSyl++)
	   {
	    HTM_LI_Begin (NULL);
	       HTM_LABEL_Begin (NULL);
		  HTM_INPUT_RADIO ("WhichSyllabus",HTM_SUBMIT_ON_CLICK,
				   "value=\"%u\"%s",
				   (unsigned) WhichSyl,
				   WhichSyl == WhichSyllabus ? " checked=\"checked\"" :
							       "");
		  HTM_Txt (Txt_SYLLABUS_WHICH_SYLLABUS[WhichSyl]);
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

static unsigned Syl_GetParItemNumber (void)
  {
   return (unsigned) Par_GetParUnsignedLong ("NumI",
					     0,
					     UINT_MAX,
					     0);
  }

/*****************************************************************************/
/********************** Check if syllabus is not empty ***********************/
/*****************************************************************************/
// Return true if info available

bool Syl_CheckSyllabus (struct Syl_Syllabus *Syllabus,long CrsCod)
  {
   bool InfoAvailable;

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (Syllabus,CrsCod);

   /***** Number of items > 0 ==> info available *****/
   InfoAvailable = (Syl_LstItemsSyllabus.NumItems != 0);

   /***** Free memory used to store items *****/
   Syl_FreeListItemsSyllabus ();

   return InfoAvailable;
  }

/*****************************************************************************/
/************** Load syllabus from file to memoruy and edit it ***************/
/*****************************************************************************/
// Return true if info available

bool Syl_CheckAndShowSyllabus (struct Syl_Syllabus *Syllabus)
  {
   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (Syllabus,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   switch (Gbl.Action.Act)
     {
      case ActEditorSylLec:	case ActEditorSylPra:
      case ActDelItmSylLec:	case ActDelItmSylPra:
      case ActUp_IteSylLec:	case ActUp_IteSylPra:
      case ActDwnIteSylLec:	case ActDwnIteSylPra:
      case ActRgtIteSylLec:	case ActRgtIteSylPra:
      case ActLftIteSylLec:	case ActLftIteSylPra:
      case ActInsIteSylLec:	case ActInsIteSylPra:
      case ActModIteSylLec:	case ActModIteSylPra:
         Syllabus->ViewType = Vie_EDIT;
         break;
      default:
         Syllabus->ViewType = Vie_VIEW;
         break;
     }

   if (Syllabus->ViewType == Vie_EDIT ||
       Syl_LstItemsSyllabus.NumItems)
     {
      /***** Write the current syllabus *****/
      Syl_ShowSyllabus (Syllabus);
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
   extern const char *Hlp_COURSE_Syllabus_edit;
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   struct Syl_Syllabus Syllabus;

   /***** Reset syllabus context *****/
   Syl_ResetSyllabus (&Syllabus);

   /***** Get syllabus type *****/
   Syllabus.WhichSyllabus = Syl_GetParWhichSyllabus ();
   Gbl.Crs.Info.Type = (Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
							         Inf_PRACTICALS);

   /***** Begin box *****/
   Box_BoxBegin (Txt_INFO_TITLE[Gbl.Crs.Info.Type],
		 NULL,NULL,
		 Hlp_COURSE_Syllabus_edit,Box_NOT_CLOSABLE);

      /***** Edit syllabus *****/
      Syl_CheckAndShowSyllabus (&Syllabus);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*** Read from XML and load in memory a syllabus of lectures or practicals ***/
/*****************************************************************************/

void Syl_LoadListItemsSyllabusIntoMemory (struct Syl_Syllabus *Syllabus,
                                          long CrsCod)
  {
   char PathFile[PATH_MAX + 1];
   FILE *XML = NULL;	// XML file for syllabus
   long PostBeginList;
   unsigned NumItem = 0;
   int N;
   int CodItem[1 + Syl_MAX_LEVELS_SYLLABUS];	// To make numeration
   int Result;
   unsigned NumItemsWithChildren = 0;

   /* Path of the private directory for the XML file with the syllabus */
   snprintf (Syllabus->PathDir,sizeof (Syllabus->PathDir),"%s/%ld/%s",
	     Cfg_PATH_CRS_PRIVATE,CrsCod,
	     Syllabus->WhichSyllabus == Syl_LECTURES ? Cfg_SYLLABUS_FOLDER_LECTURES :
		                                       Cfg_SYLLABUS_FOLDER_PRACTICALS);

   /***** Open the file with the syllabus *****/
   Syl_OpenSyllabusFile (Syllabus,PathFile,&XML);

   /***** Go to the start of the list of items *****/
   if (!Str_FindStrInFile (XML,"<lista>",Str_NO_SKIP_HTML_COMMENTS))
      Err_WrongSyllabusFormatExit ();

   /***** Save the position of the start of the list *****/
   PostBeginList = ftell (XML);

   /***** Loop to count the number of items *****/
   for (Syl_LstItemsSyllabus.NumItems = 0;
	Str_FindStrInFile (XML,"<item",Str_NO_SKIP_HTML_COMMENTS);
	Syl_LstItemsSyllabus.NumItems++);

   /***** Allocate memory for the list of items *****/
   if ((Syl_LstItemsSyllabus.Lst = calloc (Syl_LstItemsSyllabus.NumItems + 1,
                                           sizeof (*Syl_LstItemsSyllabus.Lst))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Return to the start of the list *****/
   fseek (XML,PostBeginList,SEEK_SET);

   for (N  = 1;
	N <= Syl_MAX_LEVELS_SYLLABUS;
	N++)
      CodItem[N] = 0;
   Syl_LstItemsSyllabus.NumLevels = 1;

   /***** If the syllabus is empty ==> initialize an item to be edited *****/
   if (Syl_LstItemsSyllabus.NumItems == 0)
     {
      /* Level of the item */
      Syl_LstItemsSyllabus.Lst[0].Level = 1;

      /* Code (numeration) of the item */
      CodItem[1] = 1;
      for (N = 1;
	   N <= Syl_MAX_LEVELS_SYLLABUS;
	   N++)
	 Syl_LstItemsSyllabus.Lst[0].CodItem[N] = CodItem[N];

      /* Text of the item  */
      Syl_LstItemsSyllabus.Lst[0].Text[0] = '\0';
     }
   else
      /***** Loop to read and store all items of the syllabus *****/
      for (NumItem = 0;
	   NumItem < Syl_LstItemsSyllabus.NumItems;
	   NumItem++)
	{
	 /* Go to the start of the item */
	 if (!Str_FindStrInFile (XML,"<item",Str_NO_SKIP_HTML_COMMENTS))
	    Err_WrongSyllabusFormatExit ();

	 /* Get the level */
	 Syl_LstItemsSyllabus.Lst[NumItem].Level = Syl_ReadLevelItemSyllabus (XML);
	 if (Syl_LstItemsSyllabus.Lst[NumItem].Level > Syl_LstItemsSyllabus.NumLevels)
	    Syl_LstItemsSyllabus.NumLevels = Syl_LstItemsSyllabus.Lst[NumItem].Level;

	 /* Set the code (number) of the item */
	 CodItem[Syl_LstItemsSyllabus.Lst[NumItem].Level]++;
	 for (N  = Syl_LstItemsSyllabus.Lst[NumItem].Level + 1;
	      N <= Syl_MAX_LEVELS_SYLLABUS;
	      N++)
	    CodItem[N] = 0;
	 for (N  = 1;
	      N <= Syl_MAX_LEVELS_SYLLABUS;
	      N++)
	    Syl_LstItemsSyllabus.Lst[NumItem].CodItem[N] = CodItem[N];

	 /* Get the text of the item */
	 Result = Str_ReadFileUntilBoundaryStr (XML,Syl_LstItemsSyllabus.Lst[NumItem].Text,
	                                        "</item>",strlen ("</item>"),
	                                        (unsigned long long) Syl_MAX_BYTES_TEXT_ITEM);
	 if (Result == 0) // Str too long
	   {
	    if (!Str_FindStrInFile (XML,"</item>",Str_NO_SKIP_HTML_COMMENTS)) // End the search
	       Err_WrongSyllabusFormatExit ();
	   }
	 else if (Result == -1)
	    Err_WrongSyllabusFormatExit ();
	}

   /***** Close the file with the syllabus *****/
   Syl_CloseXMLFile (&XML);

   /***** Initialize other fields in the list *****/
   if (Syl_LstItemsSyllabus.NumItems)
     {
      for (NumItem = 0;
	   NumItem < Syl_LstItemsSyllabus.NumItems - 1;
	   NumItem++)
	 if (Syl_LstItemsSyllabus.Lst[NumItem].Level < Syl_LstItemsSyllabus.Lst[NumItem + 1].Level)
	   {
	    Syl_LstItemsSyllabus.Lst[NumItem].HasChildren = true;
	    NumItemsWithChildren++;
	   }
	 else
	    Syl_LstItemsSyllabus.Lst[NumItem].HasChildren = false;
      Syl_LstItemsSyllabus.Lst[Syl_LstItemsSyllabus.NumItems - 1].HasChildren = false;
     }
   Syl_LstItemsSyllabus.NumItemsWithChildren = NumItemsWithChildren;
  }

/*****************************************************************************/
/*********************** Free list of items of a syllabus ********************/
/*****************************************************************************/

void Syl_FreeListItemsSyllabus (void)
  {
   if (Syl_LstItemsSyllabus.Lst)
     {
      free (Syl_LstItemsSyllabus.Lst);
      Syl_LstItemsSyllabus.Lst = NULL;
      Syl_LstItemsSyllabus.NumItems = 0;
     }
  }

/*****************************************************************************/
/************* Read the level of the current item in a syllabus **************/
/*****************************************************************************/
// XML file with the syllabus must be positioned after <item
// XML with the syllabus becomes positioned after <item nivel="x">

int Syl_ReadLevelItemSyllabus (FILE *XML)
  {
   int Level;
   char StrlLevel[11 + 1];

   if (!Str_FindStrInFile (XML,"nivel=\"",Str_NO_SKIP_HTML_COMMENTS))
      Err_WrongSyllabusFormatExit ();
   if (Str_ReadFileUntilBoundaryStr (XML,StrlLevel,"\"",1,
   	                             (unsigned long long) (11 + 1)) != 1)
      Err_WrongSyllabusFormatExit ();
   if (sscanf (StrlLevel,"%d",&Level) != 1)
      Err_WrongSyllabusFormatExit ();
   Str_FindStrInFile (XML,">",Str_NO_SKIP_HTML_COMMENTS);
   if (Level < 1)
      Level = 1;
   else if (Level > Syl_MAX_LEVELS_SYLLABUS)
      Level = Syl_MAX_LEVELS_SYLLABUS;
   return Level;
  }

/*****************************************************************************/
/***************** Show a syllabus of lectures or practicals *****************/
/*****************************************************************************/

static void Syl_ShowSyllabus (struct Syl_Syllabus *Syllabus)
  {
   extern const char *Hlp_COURSE_Syllabus_edit;
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   extern const char *Txt_Done;
   unsigned NumItem;
   int Col;
   static int NumButtons[Vie_NUM_VIEW_TYPES] =
     {
      [Vie_VIEW] = 0,
      [Vie_EDIT] = 5,
     };
   static const Act_Action_t Inf_Actions[Inf_NUM_TYPES] =
     {
      [Inf_INTRODUCTION  ] = ActSeeCrsInf,
      [Inf_TEACHING_GUIDE] = ActSeeTchGui,
      [Inf_LECTURES      ] = ActSeeSylLec,
      [Inf_PRACTICALS    ] = ActSeeSylPra,
      [Inf_BIBLIOGRAPHY  ] = ActSeeBib,
      [Inf_FAQ           ] = ActSeeFAQ,
      [Inf_LINKS         ] = ActSeeCrsLnk,
      [Inf_ASSESSMENT    ] = ActSeeAss,
     };
   bool ShowRowInsertNewItem = (Gbl.Action.Act == ActInsIteSylLec || Gbl.Action.Act == ActInsIteSylPra ||
                                Gbl.Action.Act == ActModIteSylLec || Gbl.Action.Act == ActModIteSylPra ||
				Gbl.Action.Act == ActRgtIteSylLec || Gbl.Action.Act == ActRgtIteSylPra ||
                                Gbl.Action.Act == ActLftIteSylLec || Gbl.Action.Act == ActLftIteSylPra);

   /***** Begin table *****/
   HTM_TABLE_BeginWide ();

      /***** Set width of columns of the table *****/
      HTM_Txt ("<colgroup>");
	 for (Col = 0;
	      Col < NumButtons[Syllabus->ViewType];
	      Col++)
	    HTM_Txt ("<col width=\"12\" />");
	 for (Col  = 1;
	      Col <= Syl_LstItemsSyllabus.NumLevels;
	      Col++)
	    HTM_TxtF ("<col width=\"%d\" />",Col * Syl_WIDTH_NUM_SYLLABUS);
	 HTM_Txt ("<col width=\"*\" />");
      HTM_Txt ("</colgroup>");

      if (Syl_LstItemsSyllabus.NumItems)
	 /***** Loop writing all items of the syllabus *****/
	 for (NumItem = 0;
	      NumItem < Syl_LstItemsSyllabus.NumItems;
	      NumItem++)
	   {
	    Syl_ShowRowSyllabus (Syllabus,NumItem,
				 Syl_LstItemsSyllabus.Lst[NumItem].Level,
				 Syl_LstItemsSyllabus.Lst[NumItem].CodItem,
				 Syl_LstItemsSyllabus.Lst[NumItem].Text,false);
	    if (ShowRowInsertNewItem && NumItem == Syllabus->NumItem)
	       // Mostrar a new row where se puede insert a new item
	       Syl_ShowRowSyllabus (Syllabus,NumItem + 1,
				    Syl_LstItemsSyllabus.Lst[NumItem].Level,NULL,
				    "",true);
	   }
      else if (Syllabus->ViewType == Vie_EDIT)
	 /***** If the syllabus is empty ==>
		show form to add a iten to the end *****/
	 Syl_ShowRowSyllabus (Syllabus,0,
			      1,Syl_LstItemsSyllabus.Lst[0].CodItem,"",true);

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Button to view *****/
   if (Syllabus->ViewType == Vie_EDIT)
     {
      Frm_BeginForm (Inf_Actions[Gbl.Crs.Info.Type]);
	 Syl_PutParWhichSyllabus (&Syllabus->WhichSyllabus);
	 Btn_PutConfirmButton (Txt_Done);
      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/******** Write a row (item) of a syllabus of lectures or practicals *********/
/*****************************************************************************/

static void Syl_ShowRowSyllabus (struct Syl_Syllabus *Syllabus,unsigned NumItem,
                                 int Level,int *CodItem,const char *Text,bool NewItem)
  {
   extern const char *Txt_Movement_not_allowed;
   static int LastLevel = 0;
   char StrItemCod[Syl_MAX_LEVELS_SYLLABUS * (10 + 1)];
   struct MoveSubtrees Subtree;

   Subtree.ToGetUp.Ini   = Subtree.ToGetUp.End   = 0;
   Subtree.ToGetDown.Ini = Subtree.ToGetDown.End = 0;
   Subtree.MovAllowed = false;

   Syllabus->ParNumItem = NumItem;	// Used as parameter in forms

   if (!NewItem)	// If the item is new (not stored in file), it has no number
      Syl_WriteNumItem (StrItemCod,NULL,Level,CodItem);

   /***** Begin the row *****/
   HTM_TR_Begin (NULL);

      switch (Syllabus->ViewType)
        {
         case Vie_VIEW:
	    /***** Indent depending on the level *****/
	    if (Level > 1)
	      {
	       HTM_TD_Begin ("colspan=\"%d\" class=\"%s\"",
			     Level - 1,The_GetColorRows ());
	       HTM_TD_End ();
	      }

	    /***** Code of the item *****/
	    HTM_TD_Begin ("class=\"RT %s_%s %s\" style=\"width:%dpx;\"",
			  ClassSyllabus[Level],The_GetSuffix (),
			  The_GetColorRows (),
			  Level * Syl_WIDTH_NUM_SYLLABUS);
	       if (Level == 1)
		  HTM_NBSP ();
	       HTM_TxtF ("%s&nbsp;",StrItemCod);
	    HTM_TD_End ();

	    /***** Text of the item *****/
	    HTM_TD_Begin ("colspan=\"%d\" class=\"LT %s_%s %s\"",
			  Syl_LstItemsSyllabus.NumLevels - Level + 1,
			  ClassSyllabus[Level],The_GetSuffix (),
			  The_GetColorRows ());
	       HTM_Txt (Text);
	    HTM_TD_End ();
            break;
         case Vie_EDIT:
	    if (NewItem)
	      {
	       HTM_TD_Begin ("colspan=\"5\" class=\"%s\"",
			     The_GetColorRows ());
	       HTM_TD_End ();
	      }
	    else
	      {
	       /***** Icon to remove the row *****/
	       HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
		  if (Syl_LstItemsSyllabus.Lst[NumItem].HasChildren)
		     Ico_PutIconRemovalNotAllowed ();
		  else
		     Ico_PutContextualIconToRemove (Gbl.Crs.Info.Type == Inf_LECTURES ? ActDelItmSylLec :
											ActDelItmSylPra,NULL,
						    Syl_PutParsSyllabus,Syllabus);
	       HTM_TD_End ();

	       /***** Icon to get up an item *****/
	       Syl_CalculateUpSubtreeSyllabus (&Subtree,NumItem);
	       HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
		  if (Subtree.MovAllowed)
		     Lay_PutContextualLinkOnlyIcon (Gbl.Crs.Info.Type == Inf_LECTURES ? ActUp_IteSylLec :
											ActUp_IteSylPra,
						    NULL,
						    Syl_PutParsSyllabus,Syllabus,
						    "arrow-up.svg",Ico_BLACK);
		  else
		     Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,
				     Txt_Movement_not_allowed);
	       HTM_TD_End ();

	       /***** Icon to get down item *****/
	       Syl_CalculateDownSubtreeSyllabus (&Subtree,NumItem);
	       HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
		  if (Subtree.MovAllowed)
		     Lay_PutContextualLinkOnlyIcon (Gbl.Crs.Info.Type == Inf_LECTURES ? ActDwnIteSylLec :
											ActDwnIteSylPra,
						    NULL,
						    Syl_PutParsSyllabus,Syllabus,
						    "arrow-down.svg",Ico_BLACK);
		  else
		     Ico_PutIconOff ("arrow-down.svg",Ico_BLACK,
				     Txt_Movement_not_allowed);
	       HTM_TD_End ();

	       /***** Icon to increase the level of an item *****/
	       HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
		  if (Level > 1)
		     Lay_PutContextualLinkOnlyIcon (Gbl.Crs.Info.Type == Inf_LECTURES ? ActRgtIteSylLec :
											ActRgtIteSylPra,
						    NULL,
						    Syl_PutParsSyllabus,Syllabus,
						    "arrow-left.svg",Ico_BLACK);
		  else
		     Ico_PutIconOff ("arrow-left.svg",Ico_BLACK,
				     Txt_Movement_not_allowed);
	       HTM_TD_End ();

	       /***** Icon to decrease level item *****/
	       HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
		  if (Level < LastLevel + 1 &&
		      Level < Syl_MAX_LEVELS_SYLLABUS)
		     Lay_PutContextualLinkOnlyIcon (Gbl.Crs.Info.Type == Inf_LECTURES ? ActLftIteSylLec :
											ActLftIteSylPra,
						    NULL,
						    Syl_PutParsSyllabus,Syllabus,
						    "arrow-right.svg",Ico_BLACK);
		  else
		     Ico_PutIconOff ("arrow-right.svg",Ico_BLACK,
				     Txt_Movement_not_allowed);
	       HTM_TD_End ();

	       LastLevel = Level;
	      }

	    Syl_PutFormItemSyllabus (Syllabus,NewItem,NumItem,Level,CodItem,Text);
            break;
	 default:
	    Err_WrongTypeExit ();
	    break;
        }

   /***** End of the row *****/
   HTM_TR_End ();

   The_ChangeRowColor ();
  }

/*****************************************************************************/
/************** Write the syllabus into a temporary HTML file ****************/
/*****************************************************************************/

void Syl_WriteSyllabusIntoHTMLTmpFile (FILE *FileHTMLTmp)
  {
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   unsigned NumItem;
   int i;

   /***** Write start of HTML code *****/
   Lay_BeginHTMLFile (FileHTMLTmp,Txt_INFO_TITLE[Gbl.Crs.Info.Type]);
   fprintf (FileHTMLTmp,"<body>\n"
                        "<table>\n");

   /***** Set width of columns of the table *****/
   fprintf (FileHTMLTmp,"<colgroup>\n");
   for (i  = 1;
	i <= Syl_LstItemsSyllabus.NumLevels;
	i++)
      fprintf (FileHTMLTmp,"<col width=\"%d\" />\n",
	       i * Syl_WIDTH_NUM_SYLLABUS);
   fprintf (FileHTMLTmp,"<col width=\"*\" />\n"
			"</colgroup>\n");

   /***** Write all items of the current syllabus into text buffer *****/
   for (NumItem = 0;
	NumItem < Syl_LstItemsSyllabus.NumItems;
	NumItem++)
     {
      /***** Begin the row *****/
      fprintf (FileHTMLTmp,"<tr>");

      /***** Indent depending on the level *****/
      if (Syl_LstItemsSyllabus.Lst[NumItem].Level > 1)
	 fprintf (FileHTMLTmp,"<td colspan=\"%d\">"
		              "</td>",
		  Syl_LstItemsSyllabus.Lst[NumItem].Level - 1);

      /***** Code of the item *****/
      fprintf (FileHTMLTmp,"<td class=\"RT\" style=\"width:%dpx;\">",
	       Syl_LstItemsSyllabus.Lst[NumItem].Level * Syl_WIDTH_NUM_SYLLABUS);
      if (Syl_LstItemsSyllabus.Lst[NumItem].Level == 1)
	 fprintf (FileHTMLTmp,"&nbsp;");
      Syl_WriteNumItem (NULL,FileHTMLTmp,
			Syl_LstItemsSyllabus.Lst[NumItem].Level,
			Syl_LstItemsSyllabus.Lst[NumItem].CodItem);
      fprintf (FileHTMLTmp,"&nbsp;"
	                   "</td>");

      /***** Text of the item *****/
      fprintf (FileHTMLTmp,"<td colspan=\"%d\" class=\"LT\">"
			   "%s"
			   "</td>",
	       Syl_LstItemsSyllabus.NumLevels - Syl_LstItemsSyllabus.Lst[NumItem].Level + 1,
	       Syl_LstItemsSyllabus.Lst[NumItem].Text);

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

static void Syl_PutFormItemSyllabus (struct Syl_Syllabus *Syllabus,
                                     bool NewItem,unsigned NumItem,int Level,int *CodItem,const char *Text)
  {
   extern const char *Txt_Enter_a_new_item_here;

   if (Level < 1)
      Level = 1;

   /***** Indent depending on the level *****/
   if (Level > 1)
     {
      HTM_TD_Begin ("colspan=\"%d\" class=\"%s\"",
                    Level - 1,The_GetColorRows ());
      HTM_TD_End ();
     }

   /***** Write the code of the item *****/
   if (NewItem)	// If the item is new (not stored in the file) ==> it has not a number
     {
      HTM_TD_Begin ("class=\"%s\" style=\"width:%dpx;\"",
	            The_GetColorRows (),
	            Level * Syl_WIDTH_NUM_SYLLABUS);
      HTM_TD_End ();
     }
   else
     {
      HTM_TD_Begin ("class=\"LM %s_%s %s\" style=\"width:%dpx;\"",
		    ClassSyllabus[Level],The_GetSuffix (),The_GetColorRows (),
		    Level * Syl_WIDTH_NUM_SYLLABUS);
	 if (Level == 1)
	    HTM_NBSP ();
	 Syl_WriteNumItem (NULL,Fil_GetOutputFile (),Level,CodItem);
	 HTM_NBSP ();
      HTM_TD_End ();
     }

   /***** Text of the item *****/
   HTM_TD_Begin ("colspan=\"%d\" class=\"LM %s\"",
		 Syl_LstItemsSyllabus.NumLevels - Level + 1,
		 The_GetColorRows ());
      Frm_BeginForm (NewItem ? (Gbl.Crs.Info.Type == Inf_LECTURES ? ActInsIteSylLec :
								    ActInsIteSylPra) :
			       (Gbl.Crs.Info.Type == Inf_LECTURES ? ActModIteSylLec :
								    ActModIteSylPra));
	 Syllabus->ParNumItem = NumItem;
	 Syl_PutParsSyllabus (Syllabus);
	 HTM_INPUT_TEXT ("Txt",Syl_MAX_CHARS_TEXT_ITEM,Text,
			 HTM_SUBMIT_ON_CHANGE,
			 "size=\"60\" class=\"INPUT_%s\" placeholder=\"%s\"%s",
			 The_GetSuffix (),
			 Txt_Enter_a_new_item_here,
			 NewItem ? " autofocus=\"autofocus\"" :
				   "");
      Frm_EndForm ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/******************* Write parameters related to syllabus ********************/
/*****************************************************************************/

static void Syl_PutParsSyllabus (void *Syllabus)
  {
   if (Syllabus)
     {
      if (((struct Syl_Syllabus *) Syllabus)->WhichSyllabus != Syl_NONE)
         Par_PutParUnsigned (NULL,"WhichSyllabus",
                             (unsigned) ((struct Syl_Syllabus *) Syllabus)->WhichSyllabus);
      Par_PutParUnsigned (NULL,"NumI",((struct Syl_Syllabus *) Syllabus)->ParNumItem);
     }
  }

/*****************************************************************************/
/******************** Write number of item in legal style ********************/
/*****************************************************************************/

static void Syl_WriteNumItem (char *StrDst,FILE *FileTgt,int Level,int *CodItem)
  {
   int N;
   char InStr[Cns_MAX_DECIMAL_DIGITS_INT + 1];

   if (StrDst)
      StrDst[0] = '\0';

   for (N  = 1;
	N <= Level;
	N++)
     {
      if (N > 1)
	{
	 if (StrDst)
	    Str_Concat (StrDst,".",Syl_MAX_BYTES_ITEM_COD);
	 if (FileTgt)
	    fprintf (FileTgt,".");
	}
      snprintf (InStr,sizeof (InStr),"%d",CodItem[N]);
      if (StrDst)
	 Str_Concat (StrDst,InStr,Syl_MAX_BYTES_ITEM_COD);
      if (FileTgt)
	 fprintf (FileTgt,"%s",InStr);
     }
  }

/*****************************************************************************/
/********************** Remove an item from syllabus *************************/
/*****************************************************************************/

void Syl_RemoveItemSyllabus (void)
  {
   struct Syl_Syllabus Syllabus;
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;
   unsigned NumItem;

   /***** Reset syllabus context *****/
   Syl_ResetSyllabus (&Syllabus);

   /***** Get syllabus type *****/
   Syllabus.WhichSyllabus = Syl_GetParWhichSyllabus ();
   Gbl.Crs.Info.Type = (Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
							         Inf_PRACTICALS);

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (&Syllabus,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   Syllabus.ViewType = Vie_EDIT;

   /***** Get item number *****/
   Syllabus.NumItem = Syl_GetParItemNumber ();

   /***** Create a new file to make the update *****/
   Syl_BuildPathFileSyllabus (&Syllabus,PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   /***** Create the new XML file *****/
   Syl_WriteStartFileSyllabus (NewFile);
   for (NumItem = 0;
	NumItem < Syl_LstItemsSyllabus.NumItems;
	NumItem++)
      if (NumItem != Syllabus.NumItem)
	 Syl_WriteItemFileSyllabus (NewFile,Syl_LstItemsSyllabus.Lst[NumItem].Level,Syl_LstItemsSyllabus.Lst[NumItem].Text);
   Syl_WriteEndFileSyllabus (NewFile);

   /***** Close the files *****/
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

   /***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****/
   Inf_DB_SetInfoSrc (Syl_LstItemsSyllabus.NumItems ? Inf_EDITOR :
   	                                                 Inf_NONE);

   /***** Show the updated syllabus to continue editing it *****/
   Syl_FreeListItemsSyllabus ();
   (void) Syl_CheckAndShowSyllabus (&Syllabus);
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

static void Syl_ChangePlaceItemSyllabus (Syl_ChangePosItem_t UpOrDownPos)
  {
   struct Syl_Syllabus Syllabus;
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;
   unsigned NumItem;
   struct MoveSubtrees Subtree;
   static void (*CalculateSubtreeSyllabus[Syl_NUM_CHANGE_POS_ITEM]) (struct MoveSubtrees *Subtree,unsigned NumItem) =
     {
      [Syl_GET_UP  ] = Syl_CalculateUpSubtreeSyllabus,
      [Syl_GET_DOWN] = Syl_CalculateDownSubtreeSyllabus,
     };

   /***** Reset syllabus context *****/
   Syl_ResetSyllabus (&Syllabus);

   /***** Get syllabus type *****/
   Syllabus.WhichSyllabus = Syl_GetParWhichSyllabus ();
   Gbl.Crs.Info.Type = (Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
							         Inf_PRACTICALS);

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (&Syllabus,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   Syllabus.ViewType = Vie_EDIT;

   /***** Get item number *****/
   Syllabus.NumItem = Syl_GetParItemNumber ();

   Subtree.ToGetUp.Ini   = Subtree.ToGetUp.End   = 0;
   Subtree.ToGetDown.Ini = Subtree.ToGetDown.End = 0;
   Subtree.MovAllowed = false;

   if (Syllabus.NumItem < Syl_LstItemsSyllabus.NumItems)
     {
      /***** Create a new file where make the update *****/
      Syl_BuildPathFileSyllabus (&Syllabus,PathFile);
      Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

      /***** Get up or get down position *****/
      CalculateSubtreeSyllabus[UpOrDownPos] (&Subtree,Syllabus.NumItem);

      /***** Create the new XML file *****/
      Syl_WriteStartFileSyllabus (NewFile);
      if (Subtree.MovAllowed)
	{
	 for (NumItem = 0;
	      NumItem < Subtree.ToGetDown.Ini;
	      NumItem++)
	    Syl_WriteItemFileSyllabus (NewFile,Syl_LstItemsSyllabus.Lst[NumItem].Level,Syl_LstItemsSyllabus.Lst[NumItem].Text);
	 for (NumItem  = Subtree.ToGetUp.Ini;
	      NumItem <= Subtree.ToGetUp.End;
	      NumItem++)
	    Syl_WriteItemFileSyllabus (NewFile,Syl_LstItemsSyllabus.Lst[NumItem].Level,Syl_LstItemsSyllabus.Lst[NumItem].Text);
	 for (NumItem  = Subtree.ToGetDown.Ini;
	      NumItem <= Subtree.ToGetDown.End;
	      NumItem++)
	    Syl_WriteItemFileSyllabus (NewFile,Syl_LstItemsSyllabus.Lst[NumItem].Level,Syl_LstItemsSyllabus.Lst[NumItem].Text);
	 for (NumItem = Subtree.ToGetUp.End + 1;
	      NumItem < Syl_LstItemsSyllabus.NumItems;
	      NumItem++)
	    Syl_WriteItemFileSyllabus (NewFile,Syl_LstItemsSyllabus.Lst[NumItem].Level,Syl_LstItemsSyllabus.Lst[NumItem].Text);
	}
      else
	 Syl_WriteAllItemsFileSyllabus (NewFile);
      Syl_WriteEndFileSyllabus (NewFile);

      /***** Close the files *****/
      Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);
     }

   /***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****/
   Inf_DB_SetInfoSrc (Syl_LstItemsSyllabus.NumItems ? Inf_EDITOR :
						         Inf_NONE);

   /***** Show the updated syllabus to continue editing it *****/
   Syl_FreeListItemsSyllabus ();
   (void) Syl_CheckAndShowSyllabus (&Syllabus);
  }

/*****************************************************************************/
/********** Compute the limits for get up a subtree of a syllabus ************/
/*****************************************************************************/
// If return Subtree->MovAllowed = false, the limits become undefined

void Syl_CalculateUpSubtreeSyllabus (struct MoveSubtrees *Subtree,unsigned NumItem)
  {
   int Level = Syl_LstItemsSyllabus.Lst[NumItem].Level;

   if (NumItem == 0)
      Subtree->MovAllowed = false;
   else	// NumItem > 0
     {
      /***** Compute limits of the subtree to get up *****/
      Subtree->ToGetUp.Ini = NumItem;
      /* Search down the end of the full subtree to get up */
      for (Subtree->ToGetUp.End = NumItem + 1;
	   Subtree->ToGetUp.End < Syl_LstItemsSyllabus.NumItems;
	   Subtree->ToGetUp.End++)
	 if (Syl_LstItemsSyllabus.Lst[Subtree->ToGetUp.End].Level <= Level)
	   {
	    Subtree->ToGetUp.End--;
	    break;
	   }
      if (Subtree->ToGetUp.End == Syl_LstItemsSyllabus.NumItems)
	 Subtree->ToGetUp.End = Syl_LstItemsSyllabus.NumItems - 1;

      /***** Compute limits of the subtree to get down *****/
      Subtree->ToGetDown.End = NumItem - 1;
      if (Syl_LstItemsSyllabus.Lst[Subtree->ToGetDown.End].Level < Level)
	 Subtree->MovAllowed = false;
      else
	{
	 Subtree->MovAllowed = true;
	 /* Find backwards the start of the subtree to get down */
	 for (Subtree->ToGetDown.Ini = Subtree->ToGetDown.End;
	      Subtree->ToGetDown.Ini > 0;
	      Subtree->ToGetDown.Ini--)
	    if (Syl_LstItemsSyllabus.Lst[Subtree->ToGetDown.Ini].Level <= Level)
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
   int Level = Syl_LstItemsSyllabus.Lst[NumItem].Level;

   /***** Compute limits of the subtree to get down *****/
   Subtree->ToGetDown.Ini = NumItem;
   /* Search down the end of the full subtree to get down */
   for (Subtree->ToGetDown.End = NumItem + 1;
	Subtree->ToGetDown.End < Syl_LstItemsSyllabus.NumItems;
	Subtree->ToGetDown.End++)
      if (Syl_LstItemsSyllabus.Lst[Subtree->ToGetDown.End].Level <= Level)
	{
	 Subtree->ToGetDown.End--;
	 break;
	}
   if (Subtree->ToGetDown.End >= Syl_LstItemsSyllabus.NumItems - 1)
      Subtree->MovAllowed = false;
   else
     {
      /***** Compute limits of the subtree to get up *****/
      Subtree->ToGetUp.Ini = Subtree->ToGetDown.End + 1;
      if (Syl_LstItemsSyllabus.Lst[Subtree->ToGetUp.Ini].Level < Level)
	 Subtree->MovAllowed = false;
      else
	{
	 Subtree->MovAllowed = true;
	 /* Find downwards the end of the subtree to get up */
	 for (Subtree->ToGetUp.End = Subtree->ToGetUp.Ini + 1;
	      Subtree->ToGetUp.End < Syl_LstItemsSyllabus.NumItems;
	      Subtree->ToGetUp.End++)
	    if (Syl_LstItemsSyllabus.Lst[Subtree->ToGetUp.End].Level <= Level)
	      {
	       Subtree->ToGetUp.End--;
	       break;
	      }
	 if (Subtree->ToGetUp.End == Syl_LstItemsSyllabus.NumItems)
	    Subtree->ToGetUp.End = Syl_LstItemsSyllabus.NumItems - 1;
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

static void Syl_ChangeLevelItemSyllabus (Syl_ChangeLevelItem_t IncreaseOrDecreaseLevel)
  {
   struct Syl_Syllabus Syllabus;
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;

   /***** Reset syllabus context *****/
   Syl_ResetSyllabus (&Syllabus);

   /***** Get syllabus type *****/
   Syllabus.WhichSyllabus = Syl_GetParWhichSyllabus ();
   Gbl.Crs.Info.Type = (Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
							         Inf_PRACTICALS);

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (&Syllabus,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   Syllabus.ViewType = Vie_EDIT;

   /***** Get item number *****/
   Syllabus.NumItem = Syl_GetParItemNumber ();

   /***** Create a new file to do the update *****/
   Syl_BuildPathFileSyllabus (&Syllabus,PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   /***** Increase or decrease level *****/
   switch (IncreaseOrDecreaseLevel)
     {
      case Syl_INCREASE_LEVEL:
	 if (Syl_LstItemsSyllabus.Lst[Syllabus.NumItem].Level > 1)
	    Syl_LstItemsSyllabus.Lst[Syllabus.NumItem].Level--;
	 break;
      case Syl_DECREASE_LEVEL:
	 if (Syl_LstItemsSyllabus.Lst[Syllabus.NumItem].Level < Syl_MAX_LEVELS_SYLLABUS)
	    Syl_LstItemsSyllabus.Lst[Syllabus.NumItem].Level++;
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
   Inf_DB_SetInfoSrc (Syl_LstItemsSyllabus.NumItems ? Inf_EDITOR :
   	                                                 Inf_NONE);

   /***** Show the updated syllabus to continue editing it *****/
   Syl_FreeListItemsSyllabus ();
   (void) Syl_CheckAndShowSyllabus (&Syllabus);
  }

/*****************************************************************************/
/************************ Insert an item in a syllabus ***********************/
/*****************************************************************************/

void Syl_InsertItemSyllabus (void)
  {
   struct Syl_Syllabus Syllabus;
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;
   unsigned NumItem;
   char Txt[Syl_MAX_BYTES_TEXT_ITEM + 1];

   /***** Reset syllabus context *****/
   Syl_ResetSyllabus (&Syllabus);

   /***** Get syllabus type *****/
   Syllabus.WhichSyllabus = Syl_GetParWhichSyllabus ();
   Gbl.Crs.Info.Type = (Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
							         Inf_PRACTICALS);

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (&Syllabus,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   Syllabus.ViewType = Vie_EDIT;

   /***** Get item number *****/
   Syllabus.NumItem = Syl_GetParItemNumber ();

   /***** Get item body *****/
   Par_GetParHTML ("Txt",Txt,Syl_MAX_BYTES_TEXT_ITEM);

   /***** Create a new file to do the update *****/
   Syl_BuildPathFileSyllabus (&Syllabus,PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   /***** Create the new XML file *****/
   Syl_WriteStartFileSyllabus (NewFile);

   /* Write items before the one to be inserted */
   for (NumItem = 0;
	NumItem < Syllabus.NumItem;
	NumItem++)
      Syl_WriteItemFileSyllabus (NewFile,Syl_LstItemsSyllabus.Lst[NumItem].Level,Syl_LstItemsSyllabus.Lst[NumItem].Text);

   /* Write the item that will be inserted */
   Syl_WriteItemFileSyllabus (NewFile,
                              NumItem ? Syl_LstItemsSyllabus.Lst[NumItem - 1].Level :
                        	        1,
                              Txt);

   /* Write items after the one just inserted */
   for (;
	NumItem < Syl_LstItemsSyllabus.NumItems;
	NumItem++)
      Syl_WriteItemFileSyllabus (NewFile,Syl_LstItemsSyllabus.Lst[NumItem].Level,Syl_LstItemsSyllabus.Lst[NumItem].Text);

   Syl_WriteEndFileSyllabus (NewFile);

   /***** Close the files *****/
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

   /***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****/
   Inf_DB_SetInfoSrc (Syl_LstItemsSyllabus.NumItems ? Inf_EDITOR :
   	                                              Inf_NONE);

   /***** Show the updated syllabus to continue editing it *****/
   Syl_FreeListItemsSyllabus ();
   (void) Syl_CheckAndShowSyllabus (&Syllabus);
  }

/*****************************************************************************/
/**************** Modify and existing item of the syllabus *******************/
/*****************************************************************************/

void Syl_ModifyItemSyllabus (void)
  {
   struct Syl_Syllabus Syllabus;
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;

   /***** Reset syllabus context *****/
   Syl_ResetSyllabus (&Syllabus);

   /***** Get syllabus type *****/
   Syllabus.WhichSyllabus = Syl_GetParWhichSyllabus ();
   Gbl.Crs.Info.Type = (Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
							         Inf_PRACTICALS);

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadListItemsSyllabusIntoMemory (&Syllabus,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   Syllabus.ViewType = Vie_EDIT;

   /***** Get item number *****/
   Syllabus.NumItem = Syl_GetParItemNumber ();

   /***** Get item body *****/
   Par_GetParHTML ("Txt",Syl_LstItemsSyllabus.Lst[Syllabus.NumItem].Text,
                   Syl_MAX_BYTES_TEXT_ITEM);

   /***** Create a new file where make the update *****/
   Syl_BuildPathFileSyllabus (&Syllabus,PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   /***** Create the new XML file *****/
   Syl_WriteStartFileSyllabus (NewFile);
   Syl_WriteAllItemsFileSyllabus (NewFile);
   Syl_WriteEndFileSyllabus (NewFile);

   /***** Close the files *****/
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

   /***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****/
   Inf_DB_SetInfoSrc (Syl_LstItemsSyllabus.NumItems ? Inf_EDITOR :
   	                                              Inf_NONE);

   /***** Show the updated syllabus to continue editing it *****/
   Syl_FreeListItemsSyllabus ();
   (void) Syl_CheckAndShowSyllabus (&Syllabus);
  }

/*****************************************************************************/
/************ Build the path of the file XML with the syllabus ***************/
/*****************************************************************************/

void Syl_BuildPathFileSyllabus (const struct Syl_Syllabus *Syllabus,
                                char PathFile[PATH_MAX + 1])
  {
   char Path[PATH_MAX + 1 + NAME_MAX + 1];

   snprintf (Path,sizeof (Path),"%s/%s",Syllabus->PathDir,Cfg_SYLLABUS_FILENAME);
   Str_Copy (PathFile,Path,PATH_MAX);
  }

/*****************************************************************************/
/*********************** Open file with the syllabus *************************/
/*****************************************************************************/

static void Syl_OpenSyllabusFile (const struct Syl_Syllabus *Syllabus,
                                  char PathFile[PATH_MAX + 1],
                                  FILE **XML)
  {
   if (*XML == NULL) // If it's not open in this moment...
     {
      /* If the directory does not exist, create it */
      Fil_CreateDirIfNotExists (Syllabus->PathDir);

      /* Open the file for reading */
      Syl_BuildPathFileSyllabus (Syllabus,PathFile);
      if ((*XML = fopen (PathFile,"rb")) == NULL)
	{
	 /* Can't open the file */
	 if (!Fil_CheckIfPathExists (Syllabus->PathDir)) // Strange error, since it is just created
	    Err_ShowErrorAndExit ("Can not open syllabus file.");
	 else
	   {
	    /* Create a new empty syllabus */
	    if ((*XML = fopen (PathFile,"wb")) == NULL)
	       Err_ShowErrorAndExit ("Can not create syllabus file.");
	    Syl_WriteStartFileSyllabus (*XML);
	    Syl_WriteEndFileSyllabus (*XML);
            Syl_CloseXMLFile (XML);

	    /* Open of new the file for reading */
	    if ((*XML = fopen (PathFile,"rb")) == NULL)
	       Err_ShowErrorAndExit ("Can not open syllabus file.");
	   }
	}
     }
   else  // Go to the start of the file
      rewind (*XML);
  }

/*****************************************************************************/
/**************************** Close XML file *********************************/
/*****************************************************************************/

static void Syl_CloseXMLFile (FILE **XML)
  {
   if (*XML)
     {
      fclose (*XML);
      *XML = NULL;	// To indicate that it is not open
     }
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
	NumItem < Syl_LstItemsSyllabus.NumItems;
	NumItem++)
      Syl_WriteItemFileSyllabus (FileSyllabus,Syl_LstItemsSyllabus.Lst[NumItem].Level,Syl_LstItemsSyllabus.Lst[NumItem].Text);
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
