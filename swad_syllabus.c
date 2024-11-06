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
#include "swad_course_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_info_database.h"
#include "swad_parameter.h"
#include "swad_string.h"
#include "swad_tree.h"
#include "swad_xml.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

// Necessary to make pointer as argument of functions
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

#define Syl_MAX_BYTES_ITEM_COD		(Syl_MAX_LEVELS_SYLLABUS * (10 + 1) - 1)

#define Syl_WIDTH_NUM_SYLLABUS 20
/*
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
*/
/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

// static unsigned Syl_GetParItemNumber (void);

// static void Syl_ShowSyllabus (struct Syl_Syllabus *Syllabus);
// static void Syl_ShowRowSyllabus (struct Syl_Syllabus *Syllabus,unsigned NumItem,
//                                  int Level,int *CodItem,const char *Text,bool NewItem);
// static void Syl_PutFormItemSyllabus (struct Syl_Syllabus *Syllabus,
//                                      bool NewItem,unsigned NumItem,int Level,int *CodItem,const char *Text);
// static void Syl_PutParsSyllabus (void *Syllabus);

static void Syl_WriteNumItem (char *StrDst,FILE *FileTgt,int Level,int *CodItem);

// static void Syl_ChangePlaceItemSyllabus (Syl_ChangePosItem_t UpOrDownPos);
// static void Syl_ChangeLevelItemSyllabus (Syl_ChangeLevelItem_t IncreaseOrDecreaseLevel);

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
   static Syl_WhichSyllabus_t WhichSyllabusCached = Syl_NONE;

   /***** If already got ==> don't search parameter again *****/
   if (WhichSyllabusCached != Syl_NONE)
      return WhichSyllabusCached;

   /***** If not yet got ==> search parameter *****/
   return WhichSyllabusCached = (Syl_WhichSyllabus_t)
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
		  HTM_INPUT_RADIO ("WhichSyllabus",
				   ((WhichSyl == WhichSyllabus) ? HTM_CHECKED :
								  HTM_NO_ATTR) | HTM_SUBMIT_ON_CLICK,
				   "value=\"%u\"",(unsigned) WhichSyl);
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
/*
static unsigned Syl_GetParItemNumber (void)
  {
   return (unsigned) Par_GetParUnsignedLong ("NumI",
					     0,
					     UINT_MAX,
					     0);
  }
*/
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
   InfoAvailable = (Syllabus->LstItems.NumItems != 0);

   /***** Free memory used to store items *****/
   Syl_FreeListItemsSyllabus (Syllabus);

   return InfoAvailable;
  }

/*****************************************************************************/
/************** Load syllabus from file to memoruy and edit it ***************/
/*****************************************************************************/
// Return true if info available
/*
bool Syl_CheckAndShowSyllabus (struct Syl_Syllabus *Syllabus)
  {
   ***** Load syllabus from XML file to memory *****
   Syl_LoadListItemsSyllabusIntoMemory (Syllabus,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   switch (Gbl.Action.Act)
     {
      // case ActEditorSyl:
      // case ActDelItmSyl:
      // case ActUp_IteSyl:
      // case ActDwnIteSyl:
      // case ActRgtIteSyl:
      // case ActLftIteSyl:
      // case ActInsIteSyl:
      // case ActModIteSyl:
         Syllabus->ViewType = Vie_EDIT;
         break;
      default:
         Syllabus->ViewType = Vie_VIEW;
         break;
     }

   if (Syllabus->ViewType == Vie_EDIT ||
       Syllabus->LstItems.NumItems)
     {
      ***** Write the current syllabus *****
      Syl_ShowSyllabus (Syllabus);
      return true;
     }

   return false;
  }
*/
/*****************************************************************************/
/****************************** Edit a syllabus ******************************/
/*****************************************************************************/
// Return true if info available
/*
void Syl_EditSyllabus (void)
  {
   extern const char *Hlp_COURSE_Syllabus_edit;
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   struct Syl_Syllabus Syllabus;

   ***** Reset syllabus context *****
   Syl_ResetSyllabus (&Syllabus);

   ***** Get syllabus type *****
   Syllabus.WhichSyllabus = Syl_GetParWhichSyllabus ();
   Gbl.Crs.Info.Type = (Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
							         Inf_PRACTICALS);

   ***** Begin box *****
   Box_BoxBegin (Txt_INFO_TITLE[Gbl.Crs.Info.Type],
		 NULL,NULL,
		 Hlp_COURSE_Syllabus_edit,Box_NOT_CLOSABLE);

      ***** Edit syllabus *****
      Syl_CheckAndShowSyllabus (&Syllabus);

   ***** End box *****
   Box_BoxEnd ();
  }
*/
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
      Err_WrongSyllabusExit ();

   /***** Save the position of the start of the list *****/
   PostBeginList = ftell (XML);

   /***** Loop to count the number of items *****/
   for (Syllabus->LstItems.NumItems = 0;
	Str_FindStrInFile (XML,"<item",Str_NO_SKIP_HTML_COMMENTS);
	Syllabus->LstItems.NumItems++);

   /***** Allocate memory for the list of items *****/
   if ((Syllabus->LstItems.Lst = calloc (Syllabus->LstItems.NumItems + 1,
                                         sizeof (*Syllabus->LstItems.Lst))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Return to the start of the list *****/
   fseek (XML,PostBeginList,SEEK_SET);

   for (N  = 1;
	N <= Syl_MAX_LEVELS_SYLLABUS;
	N++)
      CodItem[N] = 0;
   Syllabus->LstItems.NumLevels = 1;

   /***** If the syllabus is empty ==> initialize an item to be edited *****/
   if (Syllabus->LstItems.NumItems == 0)
     {
      /* Level of the item */
      Syllabus->LstItems.Lst[0].Level = 1;

      /* Code (numeration) of the item */
      CodItem[1] = 1;
      for (N = 1;
	   N <= Syl_MAX_LEVELS_SYLLABUS;
	   N++)
	 Syllabus->LstItems.Lst[0].CodItem[N] = CodItem[N];

      /* Text of the item  */
      Syllabus->LstItems.Lst[0].Text[0] = '\0';
     }
   else
      /***** Loop to read and store all items of the syllabus *****/
      for (NumItem = 0;
	   NumItem < Syllabus->LstItems.NumItems;
	   NumItem++)
	{
	 /* Go to the start of the item */
	 if (!Str_FindStrInFile (XML,"<item",Str_NO_SKIP_HTML_COMMENTS))
	    Err_WrongSyllabusExit ();

	 /* Get the level */
	 Syllabus->LstItems.Lst[NumItem].Level = Syl_ReadLevelItemSyllabus (XML);
	 if (Syllabus->LstItems.Lst[NumItem].Level > Syllabus->LstItems.NumLevels)
	    Syllabus->LstItems.NumLevels = Syllabus->LstItems.Lst[NumItem].Level;

	 /* Set the code (number) of the item */
	 CodItem[Syllabus->LstItems.Lst[NumItem].Level]++;
	 for (N  = Syllabus->LstItems.Lst[NumItem].Level + 1;
	      N <= Syl_MAX_LEVELS_SYLLABUS;
	      N++)
	    CodItem[N] = 0;
	 for (N  = 1;
	      N <= Syl_MAX_LEVELS_SYLLABUS;
	      N++)
	    Syllabus->LstItems.Lst[NumItem].CodItem[N] = CodItem[N];

	 /* Get the text of the item */
	 Result = Str_ReadFileUntilBoundaryStr (XML,Syllabus->LstItems.Lst[NumItem].Text,
	                                        "</item>",strlen ("</item>"),
	                                        (unsigned long long) Syl_MAX_BYTES_TEXT_ITEM);
	 if (Result == 0) // Str too long
	   {
	    if (!Str_FindStrInFile (XML,"</item>",Str_NO_SKIP_HTML_COMMENTS)) // End the search
	       Err_WrongSyllabusExit ();
	   }
	 else if (Result == -1)
	    Err_WrongSyllabusExit ();
	}

   /***** Close the file with the syllabus *****/
   Syl_CloseXMLFile (&XML);

   /***** Initialize other fields in the list *****/
   if (Syllabus->LstItems.NumItems)
     {
      for (NumItem = 0;
	   NumItem < Syllabus->LstItems.NumItems - 1;
	   NumItem++)
	 if (Syllabus->LstItems.Lst[NumItem].Level < Syllabus->LstItems.Lst[NumItem + 1].Level)
	   {
	    Syllabus->LstItems.Lst[NumItem].HasChildren = true;
	    NumItemsWithChildren++;
	   }
	 else
	    Syllabus->LstItems.Lst[NumItem].HasChildren = false;
      Syllabus->LstItems.Lst[Syllabus->LstItems.NumItems - 1].HasChildren = false;
     }
   Syllabus->LstItems.NumItemsWithChildren = NumItemsWithChildren;
  }

/*****************************************************************************/
/*********************** Free list of items of a syllabus ********************/
/*****************************************************************************/

void Syl_FreeListItemsSyllabus (struct Syl_Syllabus *Syllabus)
  {
   if (Syllabus->LstItems.Lst)
     {
      free (Syllabus->LstItems.Lst);
      Syllabus->LstItems.Lst = NULL;
      Syllabus->LstItems.NumItems = 0;
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
      Err_WrongSyllabusExit ();
   if (Str_ReadFileUntilBoundaryStr (XML,StrlLevel,"\"",1,
   	                             (unsigned long long) (11 + 1)) != 1)
      Err_WrongSyllabusExit ();
   if (sscanf (StrlLevel,"%d",&Level) != 1)
      Err_WrongSyllabusExit ();
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
/*
static void Syl_ShowSyllabus (struct Syl_Syllabus *Syllabus)
  {
   extern const char *Txt_Done;
   unsigned NumItem;
   int Col;
   static int NumButtons[Vie_NUM_VIEW_TYPES] =
     {
      [Vie_VIEW] = 0,
      [Vie_EDIT] = 5,
     };
   static Act_Action_t Inf_Actions[Inf_NUM_TYPES] =
     {
      [Inf_INFORMATION	] = ActSeeCrsInf,
      [Inf_TEACH_GUIDE	] = ActSeeTchGui,
      [Inf_LECTURES	] = ActSeeSyl,
      [Inf_PRACTICALS	] = ActSeeSyl,
      [Inf_BIBLIOGRAPHY	] = ActSeeBib,
      [Inf_FAQ		] = ActSeeFAQ,
      [Inf_LINKS	] = ActSeeCrsLnk,
      [Inf_ASSESSMENT	] = ActSeeAss,
     };
   bool ShowRowInsertNewItem = (Gbl.Action.Act == ActInsIteSyl ||
                                Gbl.Action.Act == ActModIteSyl ||
				Gbl.Action.Act == ActRgtIteSyl ||
                                Gbl.Action.Act == ActLftIteSyl);

   ***** Begin table *****
   HTM_TABLE_BeginWide ();

      ***** Set width of columns of the table *****
      HTM_Txt ("<colgroup>");
	 for (Col = 0;
	      Col < NumButtons[Syllabus->ViewType];
	      Col++)
	    HTM_Txt ("<col width=\"12\" />");
	 for (Col  = 1;
	      Col <= Syllabus->LstItems.NumLevels;
	      Col++)
	    HTM_TxtF ("<col width=\"%d\" />",Col * Syl_WIDTH_NUM_SYLLABUS);
	 HTM_Txt ("<col width=\"*\" />");
      HTM_Txt ("</colgroup>");

      if (Syllabus->LstItems.NumItems)
	 ***** Loop writing all items of the syllabus *****
	 for (NumItem = 0;
	      NumItem < Syllabus->LstItems.NumItems;
	      NumItem++)
	   {
	    Syl_ShowRowSyllabus (Syllabus,NumItem,
				 Syllabus->LstItems.Lst[NumItem].Level,
				 Syllabus->LstItems.Lst[NumItem].CodItem,
				 Syllabus->LstItems.Lst[NumItem].Text,false);
	    if (ShowRowInsertNewItem && NumItem == Syllabus->NumItem)
	       // Show a new row where insert a new item
	       Syl_ShowRowSyllabus (Syllabus,NumItem + 1,
				    Syllabus->LstItems.Lst[NumItem].Level,NULL,
				    "",true);
	   }
      else if (Syllabus->ViewType == Vie_EDIT)
	 ***** If the syllabus is empty ==>
		show form to add a iten to the end *****
	 Syl_ShowRowSyllabus (Syllabus,0,
			      1,Syllabus->LstItems.Lst[0].CodItem,"",true);

   ***** End table *****
   HTM_TABLE_End ();

   ***** Button to view *****
   if (Syllabus->ViewType == Vie_EDIT)
     {
      Frm_BeginForm (Inf_Actions[Gbl.Crs.Info.Type]);
	 Syl_PutParWhichSyllabus (&Syllabus->WhichSyllabus);
	 Btn_PutConfirmButton (Txt_Done);
      Frm_EndForm ();
     }
  }
*/
/*****************************************************************************/
/******** Write a row (item) of a syllabus of lectures or practicals *********/
/*****************************************************************************/
/*
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

   ***** Begin the row *****
   HTM_TR_Begin (NULL);

      switch (Syllabus->ViewType)
        {
         case Vie_VIEW:
	    ***** Indent depending on the level *****
	    if (Level > 1)
	      {
	       HTM_TD_Begin ("colspan=\"%d\" class=\"%s\"",
			     Level - 1,The_GetColorRows ());
	       HTM_TD_End ();
	      }

	    ***** Code of the item *****
	    HTM_TD_Begin ("class=\"RT %s_%s %s\" style=\"width:%dpx;\"",
			  ClassSyllabus[Level],The_GetSuffix (),
			  The_GetColorRows (),Level * Syl_WIDTH_NUM_SYLLABUS);
	       if (Level == 1)
		  HTM_NBSP ();
	       HTM_TxtF ("%s&nbsp;",StrItemCod);
	    HTM_TD_End ();

	    ***** Text of the item *****
	    HTM_TD_Begin ("colspan=\"%d\" class=\"LT %s_%s %s\"",
			  Syllabus->LstItems.NumLevels - Level + 1,
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
	       ***** Icon to remove the row *****
	       HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
		  if (Syllabus->LstItems.Lst[NumItem].HasChildren)
		     Ico_PutIconRemovalNotAllowed ();
		  else
		     Ico_PutContextualIconToRemove (ActDelItmSyl,NULL,
						    Syl_PutParsSyllabus,Syllabus);
	       HTM_TD_End ();

	       ***** Icon to get up an item *****
	       Syl_CalculateUpSubtreeSyllabus (Syllabus,&Subtree,NumItem);
	       HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
		  if (Subtree.MovAllowed)
		     Lay_PutContextualLinkOnlyIcon (ActUp_IteSyl,NULL,
						    Syl_PutParsSyllabus,Syllabus,
						    "arrow-up.svg",Ico_BLACK);
		  else
		     Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,
				     Txt_Movement_not_allowed);
	       HTM_TD_End ();

	       ***** Icon to get down item *****
	       Syl_CalculateDownSubtreeSyllabus (Syllabus,&Subtree,NumItem);
	       HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
		  if (Subtree.MovAllowed)
		     Lay_PutContextualLinkOnlyIcon (ActDwnIteSyl,NULL,
						    Syl_PutParsSyllabus,Syllabus,
						    "arrow-down.svg",Ico_BLACK);
		  else
		     Ico_PutIconOff ("arrow-down.svg",Ico_BLACK,
				     Txt_Movement_not_allowed);
	       HTM_TD_End ();

	       ***** Icon to increase the level of an item *****
	       HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
		  if (Level > 1)
		     Lay_PutContextualLinkOnlyIcon (ActRgtIteSyl,NULL,
						    Syl_PutParsSyllabus,Syllabus,
						    "arrow-left.svg",Ico_BLACK);
		  else
		     Ico_PutIconOff ("arrow-left.svg",Ico_BLACK,
				     Txt_Movement_not_allowed);
	       HTM_TD_End ();

	       ***** Icon to decrease level item *****
	       HTM_TD_Begin ("class=\"BM %s\"",The_GetColorRows ());
		  if (Level < LastLevel + 1 &&
		      Level < Syl_MAX_LEVELS_SYLLABUS)
		     Lay_PutContextualLinkOnlyIcon (ActLftIteSyl,NULL,
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

   ***** End of the row *****
   HTM_TR_End ();

   The_ChangeRowColor ();
  }
*/
/*****************************************************************************/
/************** Write the syllabus into a temporary HTML file ****************/
/*****************************************************************************/

void Syl_WriteSyllabusIntoHTMLTmpFile (struct Syl_Syllabus *Syllabus,
				       FILE *FileHTMLTmp)
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
	i <= Syllabus->LstItems.NumLevels;
	i++)
      fprintf (FileHTMLTmp,"<col width=\"%d\" />\n",
	       i * Syl_WIDTH_NUM_SYLLABUS);
   fprintf (FileHTMLTmp,"<col width=\"*\" />\n"
			"</colgroup>\n");

   /***** Write all items of the current syllabus into text buffer *****/
   for (NumItem = 0;
	NumItem < Syllabus->LstItems.NumItems;
	NumItem++)
     {
      /***** Begin the row *****/
      fprintf (FileHTMLTmp,"<tr>");

      /***** Indent depending on the level *****/
      if (Syllabus->LstItems.Lst[NumItem].Level > 1)
	 fprintf (FileHTMLTmp,"<td colspan=\"%d\">"
		              "</td>",
		  Syllabus->LstItems.Lst[NumItem].Level - 1);

      /***** Code of the item *****/
      fprintf (FileHTMLTmp,"<td class=\"RT\" style=\"width:%dpx;\">",
	       Syllabus->LstItems.Lst[NumItem].Level * Syl_WIDTH_NUM_SYLLABUS);
      if (Syllabus->LstItems.Lst[NumItem].Level == 1)
	 fprintf (FileHTMLTmp,"&nbsp;");
      Syl_WriteNumItem (NULL,FileHTMLTmp,
			Syllabus->LstItems.Lst[NumItem].Level,
			Syllabus->LstItems.Lst[NumItem].CodItem);
      fprintf (FileHTMLTmp,"&nbsp;"
	                   "</td>");

      /***** Text of the item *****/
      fprintf (FileHTMLTmp,"<td colspan=\"%d\" class=\"LT\">"
			   "%s"
			   "</td>",
	       Syllabus->LstItems.NumLevels - Syllabus->LstItems.Lst[NumItem].Level + 1,
	       Syllabus->LstItems.Lst[NumItem].Text);

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
/*
static void Syl_PutFormItemSyllabus (struct Syl_Syllabus *Syllabus,
                                     bool NewItem,unsigned NumItem,int Level,int *CodItem,const char *Text)
  {
   extern const char *Txt_Enter_a_new_item_here;

   if (Level < 1)
      Level = 1;

   ***** Indent depending on the level *****
   if (Level > 1)
     {
      HTM_TD_Begin ("colspan=\"%d\" class=\"%s\"",
                    Level - 1,The_GetColorRows ());
      HTM_TD_End ();
     }

   ***** Write the code of the item *****
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

   ***** Text of the item *****
   HTM_TD_Begin ("colspan=\"%d\" class=\"LM %s\"",
		 Syllabus->LstItems.NumLevels - Level + 1,
		 The_GetColorRows ());
      Frm_BeginForm (NewItem ? ActInsIteSyl :
			       ActModIteSyl);
	 Syllabus->ParNumItem = NumItem;
	 Syl_PutParsSyllabus (Syllabus);
	 HTM_INPUT_TEXT ("Txt",Syl_MAX_CHARS_TEXT_ITEM,Text,
			 (NewItem ? HTM_AUTOFOCUS :
				    HTM_NO_ATTR) | HTM_SUBMIT_ON_CHANGE,
			 "size=\"60\" class=\"INPUT_%s\" placeholder=\"%s\"",
			 The_GetSuffix (),Txt_Enter_a_new_item_here);
      Frm_EndForm ();
   HTM_TD_End ();
  }
*/
/*****************************************************************************/
/******************* Write parameters related to syllabus ********************/
/*****************************************************************************/
/*
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
*/
/*****************************************************************************/
/******************** Write number of item in legal style ********************/
/*****************************************************************************/

static void Syl_WriteNumItem (char *StrDst,FILE *FileTgt,int Level,int *CodItem)
  {
   int N;
   char InStr[Cns_MAX_DIGITS_INT + 1];

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
/*
void Syl_RemoveItemSyllabus (void)
  {
   struct Syl_Syllabus Syllabus;
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;
   unsigned NumItem;

   ***** Reset syllabus context *****
   Syl_ResetSyllabus (&Syllabus);

   ***** Get syllabus type *****
   Syllabus.WhichSyllabus = Syl_GetParWhichSyllabus ();
   Gbl.Crs.Info.Type = (Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
							         Inf_PRACTICALS);

   ***** Load syllabus from XML file to memory *****
   Syl_LoadListItemsSyllabusIntoMemory (&Syllabus,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   Syllabus.ViewType = Vie_EDIT;

   ***** Get item number *****
   Syllabus.NumItem = Syl_GetParItemNumber ();

   ***** Create a new file to make the update *****
   Syl_BuildPathFileSyllabus (&Syllabus,PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   ***** Create the new XML file *****
   Syl_WriteStartFileSyllabus (NewFile);
   for (NumItem = 0;
	NumItem < Syllabus.LstItems.NumItems;
	NumItem++)
      if (NumItem != Syllabus.NumItem)
	 Syl_WriteItemFileSyllabus (NewFile,
				    Syllabus.LstItems.Lst[NumItem].Level,
				    Syllabus.LstItems.Lst[NumItem].Text);
   Syl_WriteEndFileSyllabus (NewFile);

   ***** Close the files *****
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

   ***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****
   Inf_DB_SetInfoSrc (Syllabus.LstItems.NumItems ? Inf_EDITOR :
   	                                           Inf_NONE);

   ***** Show the updated syllabus to continue editing it *****
   Syl_FreeListItemsSyllabus (&Syllabus);
   (void) Syl_CheckAndShowSyllabus (&Syllabus);
  }
*/
/*****************************************************************************/
/*** Get up position of an item of the syllabus of lectures or prácticals ****/
/*****************************************************************************/
/*
void Syl_UpItemSyllabus (void)
  {
   Syl_ChangePlaceItemSyllabus (Syl_GET_UP);
  }
*/
/*****************************************************************************/
/** Get down position of an item of the syllabus of lectures or practicals ***/
/*****************************************************************************/
/*
void Syl_DownItemSyllabus (void)
  {
   Syl_ChangePlaceItemSyllabus (Syl_GET_DOWN);
  }
*/
/*****************************************************************************/
/*************** Get up or get down a subtree of a syllabus ******************/
/*****************************************************************************/
/*
static void Syl_ChangePlaceItemSyllabus (Syl_ChangePosItem_t UpOrDownPos)
  {
   struct Syl_Syllabus Syllabus;
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;
   unsigned NumItem;
   struct MoveSubtrees Subtree;
   static void (*CalculateSubtreeSyllabus[Syl_NUM_CHANGE_POS_ITEM]) (const struct Syl_Syllabus *Syllabus,
								     struct MoveSubtrees *Subtree,
								     unsigned NumItem) =
     {
      [Syl_GET_UP  ] = Syl_CalculateUpSubtreeSyllabus,
      [Syl_GET_DOWN] = Syl_CalculateDownSubtreeSyllabus,
     };

   ***** Reset syllabus context *****
   Syl_ResetSyllabus (&Syllabus);

   ***** Get syllabus type *****
   Syllabus.WhichSyllabus = Syl_GetParWhichSyllabus ();
   Gbl.Crs.Info.Type = (Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
							         Inf_PRACTICALS);

   ***** Load syllabus from XML file to memory *****
   Syl_LoadListItemsSyllabusIntoMemory (&Syllabus,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   Syllabus.ViewType = Vie_EDIT;

   ***** Get item number *****
   Syllabus.NumItem = Syl_GetParItemNumber ();

   Subtree.ToGetUp.Ini   = Subtree.ToGetUp.End   = 0;
   Subtree.ToGetDown.Ini = Subtree.ToGetDown.End = 0;
   Subtree.MovAllowed = false;

   if (Syllabus.NumItem < Syllabus.LstItems.NumItems)
     {
      ***** Create a new file where make the update *****
      Syl_BuildPathFileSyllabus (&Syllabus,PathFile);
      Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

      ***** Get up or get down position *****
      CalculateSubtreeSyllabus[UpOrDownPos] (&Syllabus,&Subtree,Syllabus.NumItem);

      ***** Create the new XML file *****
      Syl_WriteStartFileSyllabus (NewFile);
      if (Subtree.MovAllowed)
	{
	 for (NumItem = 0;
	      NumItem < Subtree.ToGetDown.Ini;
	      NumItem++)
	    Syl_WriteItemFileSyllabus (NewFile,
				       Syllabus.LstItems.Lst[NumItem].Level,
				       Syllabus.LstItems.Lst[NumItem].Text);
	 for (NumItem  = Subtree.ToGetUp.Ini;
	      NumItem <= Subtree.ToGetUp.End;
	      NumItem++)
	    Syl_WriteItemFileSyllabus (NewFile,
				       Syllabus.LstItems.Lst[NumItem].Level,
				       Syllabus.LstItems.Lst[NumItem].Text);
	 for (NumItem  = Subtree.ToGetDown.Ini;
	      NumItem <= Subtree.ToGetDown.End;
	      NumItem++)
	    Syl_WriteItemFileSyllabus (NewFile,
				       Syllabus.LstItems.Lst[NumItem].Level,
				       Syllabus.LstItems.Lst[NumItem].Text);
	 for (NumItem = Subtree.ToGetUp.End + 1;
	      NumItem < Syllabus.LstItems.NumItems;
	      NumItem++)
	    Syl_WriteItemFileSyllabus (NewFile,
				       Syllabus.LstItems.Lst[NumItem].Level,
				       Syllabus.LstItems.Lst[NumItem].Text);
	}
      else
	 Syl_WriteAllItemsFileSyllabus (&Syllabus,NewFile);
      Syl_WriteEndFileSyllabus (NewFile);

      ***** Close the files *****
      Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);
     }

   ***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****
   Inf_DB_SetInfoSrc (Syllabus.LstItems.NumItems ? Inf_EDITOR :
						   Inf_NONE);

   ***** Show the updated syllabus to continue editing it *****
   Syl_FreeListItemsSyllabus (&Syllabus);
   (void) Syl_CheckAndShowSyllabus (&Syllabus);
  }
*/
/*****************************************************************************/
/********** Compute the limits for get up a subtree of a syllabus ************/
/*****************************************************************************/
// If return Subtree->MovAllowed = false, the limits become undefined
/*
void Syl_CalculateUpSubtreeSyllabus (const struct Syl_Syllabus *Syllabus,
				     struct MoveSubtrees *Subtree,
				     unsigned NumItem)
  {
   int Level = Syllabus->LstItems.Lst[NumItem].Level;

   if (NumItem == 0)
      Subtree->MovAllowed = false;
   else	// NumItem > 0
     {
      ***** Compute limits of the subtree to get up *****
      Subtree->ToGetUp.Ini = NumItem;
      * Search down the end of the full subtree to get up *
      for (Subtree->ToGetUp.End = NumItem + 1;
	   Subtree->ToGetUp.End < Syllabus->LstItems.NumItems;
	   Subtree->ToGetUp.End++)
	 if (Syllabus->LstItems.Lst[Subtree->ToGetUp.End].Level <= Level)
	   {
	    Subtree->ToGetUp.End--;
	    break;
	   }
      if (Subtree->ToGetUp.End == Syllabus->LstItems.NumItems)
	 Subtree->ToGetUp.End = Syllabus->LstItems.NumItems - 1;

      ***** Compute limits of the subtree to get down *****
      Subtree->ToGetDown.End = NumItem - 1;
      if (Syllabus->LstItems.Lst[Subtree->ToGetDown.End].Level < Level)
	 Subtree->MovAllowed = false;
      else
	{
	 Subtree->MovAllowed = true;
	 * Find backwards the start of the subtree to get down *
	 for (Subtree->ToGetDown.Ini = Subtree->ToGetDown.End;
	      Subtree->ToGetDown.Ini > 0;
	      Subtree->ToGetDown.Ini--)
	    if (Syllabus->LstItems.Lst[Subtree->ToGetDown.Ini].Level <= Level)
	       break;
	}
     }
  }
*/
/*****************************************************************************/
/****** Compute the limits for the get down of a subtree of a syllabus *******/
/*****************************************************************************/
// When return Subtree->MovAllowed equal to false, the limits become undefined
/*
void Syl_CalculateDownSubtreeSyllabus (const struct Syl_Syllabus *Syllabus,
				       struct MoveSubtrees *Subtree,
				       unsigned NumItem)
  {
   int Level = Syllabus->LstItems.Lst[NumItem].Level;

   ***** Compute limits of the subtree to get down *****
   Subtree->ToGetDown.Ini = NumItem;
   * Search down the end of the full subtree to get down *
   for (Subtree->ToGetDown.End = NumItem + 1;
	Subtree->ToGetDown.End < Syllabus->LstItems.NumItems;
	Subtree->ToGetDown.End++)
      if (Syllabus->LstItems.Lst[Subtree->ToGetDown.End].Level <= Level)
	{
	 Subtree->ToGetDown.End--;
	 break;
	}
   if (Subtree->ToGetDown.End >= Syllabus->LstItems.NumItems - 1)
      Subtree->MovAllowed = false;
   else
     {
      ***** Compute limits of the subtree to get up *****
      Subtree->ToGetUp.Ini = Subtree->ToGetDown.End + 1;
      if (Syllabus->LstItems.Lst[Subtree->ToGetUp.Ini].Level < Level)
	 Subtree->MovAllowed = false;
      else
	{
	 Subtree->MovAllowed = true;
	 * Find downwards the end of the subtree to get up *
	 for (Subtree->ToGetUp.End = Subtree->ToGetUp.Ini + 1;
	      Subtree->ToGetUp.End < Syllabus->LstItems.NumItems;
	      Subtree->ToGetUp.End++)
	    if (Syllabus->LstItems.Lst[Subtree->ToGetUp.End].Level <= Level)
	      {
	       Subtree->ToGetUp.End--;
	       break;
	      }
	 if (Subtree->ToGetUp.End == Syllabus->LstItems.NumItems)
	    Subtree->ToGetUp.End = Syllabus->LstItems.NumItems - 1;
	}
     }
  }
*/
/*****************************************************************************/
/** Increase the level of an item of the syllabus of lectures or practicals **/
/*****************************************************************************/
/*
void Syl_RightItemSyllabus (void)
  {
   Syl_ChangeLevelItemSyllabus (Syl_INCREASE_LEVEL);
  }
*/
/*****************************************************************************/
/** Decrease the level of an item of the syllabus of lectures or practicals **/
/*****************************************************************************/
/*
void Syl_LeftItemSyllabus (void)
  {
   Syl_ChangeLevelItemSyllabus (Syl_DECREASE_LEVEL);
  }
*/
/*****************************************************************************/
/********* Increase or decrease the level of an item of a syllabus ***********/
/*****************************************************************************/
/*
static void Syl_ChangeLevelItemSyllabus (Syl_ChangeLevelItem_t IncreaseOrDecreaseLevel)
  {
   struct Syl_Syllabus Syllabus;
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;

   ***** Reset syllabus context *****
   Syl_ResetSyllabus (&Syllabus);

   ***** Get syllabus type *****
   Syllabus.WhichSyllabus = Syl_GetParWhichSyllabus ();
   Gbl.Crs.Info.Type = (Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
							         Inf_PRACTICALS);

   ***** Load syllabus from XML file to memory *****
   Syl_LoadListItemsSyllabusIntoMemory (&Syllabus,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   Syllabus.ViewType = Vie_EDIT;

   ***** Get item number *****
   Syllabus.NumItem = Syl_GetParItemNumber ();

   ***** Create a new file to do the update *****
   Syl_BuildPathFileSyllabus (&Syllabus,PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   ***** Increase or decrease level *****
   switch (IncreaseOrDecreaseLevel)
     {
      case Syl_INCREASE_LEVEL:
	 if (Syllabus.LstItems.Lst[Syllabus.NumItem].Level > 1)
	    Syllabus.LstItems.Lst[Syllabus.NumItem].Level--;
	 break;
      case Syl_DECREASE_LEVEL:
	 if (Syllabus.LstItems.Lst[Syllabus.NumItem].Level < Syl_MAX_LEVELS_SYLLABUS)
	    Syllabus.LstItems.Lst[Syllabus.NumItem].Level++;
	 break;
     }

   ***** Create the new XML file *****
   Syl_WriteStartFileSyllabus (NewFile);
   Syl_WriteAllItemsFileSyllabus (&Syllabus,NewFile);
   Syl_WriteEndFileSyllabus (NewFile);

   ***** Close the files *****
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

   ***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****
   Inf_DB_SetInfoSrc (Syllabus.LstItems.NumItems ? Inf_EDITOR :
   	                                           Inf_NONE);

   ***** Show the updated syllabus to continue editing it *****
   Syl_FreeListItemsSyllabus (&Syllabus);
   (void) Syl_CheckAndShowSyllabus (&Syllabus);
  }
*/
/*****************************************************************************/
/************************ Insert an item in a syllabus ***********************/
/*****************************************************************************/
/*
void Syl_InsertItemSyllabus (void)
  {
   struct Syl_Syllabus Syllabus;
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;
   unsigned NumItem;
   char Txt[Syl_MAX_BYTES_TEXT_ITEM + 1];

   ***** Reset syllabus context *****
   Syl_ResetSyllabus (&Syllabus);

   ***** Get syllabus type *****
   Syllabus.WhichSyllabus = Syl_GetParWhichSyllabus ();
   Gbl.Crs.Info.Type = (Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
							         Inf_PRACTICALS);

   ***** Load syllabus from XML file to memory *****
   Syl_LoadListItemsSyllabusIntoMemory (&Syllabus,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   Syllabus.ViewType = Vie_EDIT;

   ***** Get item number *****
   Syllabus.NumItem = Syl_GetParItemNumber ();

   ***** Get item body *****
   Par_GetParHTML ("Txt",Txt,Syl_MAX_BYTES_TEXT_ITEM);

   ***** Create a new file to do the update *****
   Syl_BuildPathFileSyllabus (&Syllabus,PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   ***** Create the new XML file *****
   Syl_WriteStartFileSyllabus (NewFile);

   * Write items before the one to be inserted *
   for (NumItem = 0;
	NumItem < Syllabus.NumItem;
	NumItem++)
      Syl_WriteItemFileSyllabus (NewFile,
				 Syllabus.LstItems.Lst[NumItem].Level,
				 Syllabus.LstItems.Lst[NumItem].Text);

   * Write the item that will be inserted *
   Syl_WriteItemFileSyllabus (NewFile,
                              NumItem ? Syllabus.LstItems.Lst[NumItem - 1].Level :
                        	        1,
                              Txt);

   * Write items after the one just inserted *
   for (;
	NumItem < Syllabus.LstItems.NumItems;
	NumItem++)
      Syl_WriteItemFileSyllabus (NewFile,
				 Syllabus.LstItems.Lst[NumItem].Level,
				 Syllabus.LstItems.Lst[NumItem].Text);

   Syl_WriteEndFileSyllabus (NewFile);

   ***** Close the files *****
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

   ***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****
   Inf_DB_SetInfoSrc (Syllabus.LstItems.NumItems ? Inf_EDITOR :
   	                                           Inf_NONE);

   ***** Show the updated syllabus to continue editing it *****
   Syl_FreeListItemsSyllabus (&Syllabus);
   (void) Syl_CheckAndShowSyllabus (&Syllabus);
  }
*/
/*****************************************************************************/
/**************** Modify and existing item of the syllabus *******************/
/*****************************************************************************/
/*
void Syl_ModifyItemSyllabus (void)
  {
   struct Syl_Syllabus Syllabus;
   char PathFile[PATH_MAX + 1];
   char PathOldFile[PATH_MAX + 1];
   char PathNewFile[PATH_MAX + 1];
   FILE *NewFile;

   ***** Reset syllabus context *****
   Syl_ResetSyllabus (&Syllabus);

   ***** Get syllabus type *****
   Syllabus.WhichSyllabus = Syl_GetParWhichSyllabus ();
   Gbl.Crs.Info.Type = (Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
							         Inf_PRACTICALS);

   ***** Load syllabus from XML file to memory *****
   Syl_LoadListItemsSyllabusIntoMemory (&Syllabus,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   Syllabus.ViewType = Vie_EDIT;

   ***** Get item number *****
   Syllabus.NumItem = Syl_GetParItemNumber ();

   ***** Get item body *****
   Par_GetParHTML ("Txt",Syllabus.LstItems.Lst[Syllabus.NumItem].Text,
                   Syl_MAX_BYTES_TEXT_ITEM);

   ***** Create a new file where make the update *****
   Syl_BuildPathFileSyllabus (&Syllabus,PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   ***** Create the new XML file *****
   Syl_WriteStartFileSyllabus (NewFile);
   Syl_WriteAllItemsFileSyllabus (&Syllabus,NewFile);
   Syl_WriteEndFileSyllabus (NewFile);

   ***** Close the files *****
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

   ***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****
   Inf_DB_SetInfoSrc (Syllabus.LstItems.NumItems ? Inf_EDITOR :
   	                                           Inf_NONE);

   ***** Show the updated syllabus to continue editing it *****
   Syl_FreeListItemsSyllabus (&Syllabus);
   (void) Syl_CheckAndShowSyllabus (&Syllabus);
  }
*/
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

void Syl_WriteAllItemsFileSyllabus (const struct Syl_Syllabus *Syllabus,
				    FILE *FileSyllabus)
  {
   unsigned NumItem;

   for (NumItem = 0;
	NumItem < Syllabus->LstItems.NumItems;
	NumItem++)
      Syl_WriteItemFileSyllabus (FileSyllabus,Syllabus->LstItems.Lst[NumItem].Level,Syllabus->LstItems.Lst[NumItem].Text);
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

/*****************************************************************************/
/************************ Put link to convert syllabus ***********************/
/*****************************************************************************/

void Syl_PutLinkToConvertSyllabus (void)
  {
   extern const char *Txt_Convert_syllabus;

   /***** Put form to set up platform *****/
   Lay_PutContextualLinkIconText (ActCvtSyl,NULL,
                                  NULL,NULL,
				  "file-export.svg",Ico_BLACK,
				  Txt_Convert_syllabus,NULL);
  }

/*****************************************************************************/
/**************** Convert all syllabus from files to database ****************/
/*****************************************************************************/

void Syl_ConvertAllSyllabus (void)
  {
   extern const char *Tre_DB_Types[Tre_NUM_TYPES];
   Tre_TreeType_t TreeType;
   struct Syl_Syllabus Syllabus;
   MYSQL_RES *mysql_res_crs;
   MYSQL_ROW row;
   unsigned NumCrss;
   unsigned NumCrs;
   long CrsCod;
   unsigned NumItem;
   char StrItemCod[Syl_MAX_LEVELS_SYLLABUS * (10 + 1)];
   int Level;
   struct Tre_Node Node;
   static Tre_TreeType_t TreeTypes[Syl_NUM_WHICH_SYLLABUS] =
     {
      [Syl_NONE		] = Tre_UNKNOWN,
      [Syl_LECTURES	] = Tre_LECTURES,
      [Syl_PRACTICALS	] = Tre_PRACTICALS,
     };

   /***** Remove all syllabus from database *****/
   for (TreeType  = Tre_LECTURES;
	TreeType <= Tre_PRACTICALS;
	TreeType++)
      DB_QueryDELETE ("can not remove tree nodes",
		      "DELETE FROM tre_nodes"
		      " WHERE Type='%s'",
		      Tre_DB_Types[TreeType]);

   /***** Get all courses from database *****/
   NumCrss = Crs_DB_GetAllCrss (&mysql_res_crs);
   HTM_OL_Begin ();
   for (NumCrs = 0;
	NumCrs < NumCrss;
	NumCrs++)
     {
      /* Get next course */
      row = mysql_fetch_row (mysql_res_crs);

      /* Get course code (row[0]) */
      if ((CrsCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
	{
	 for (Syllabus.WhichSyllabus  = Syl_LECTURES;
	      Syllabus.WhichSyllabus <= Syl_PRACTICALS;
	      Syllabus.WhichSyllabus++)
	   {
	    /***** Load syllabus from XML file to memory *****/
	    Syl_LoadListItemsSyllabusIntoMemory (&Syllabus,CrsCod);

	    if (Syllabus.LstItems.NumItems)
	      {
	       /***** Write number of items *****/
	       HTM_LI_Begin (NULL);
		  HTM_TxtF ("CrsCod: %ld; NumItems: %u",
			    CrsCod,Syllabus.LstItems.NumItems);

		  /***** Loop writing all items of the syllabus *****/
		  HTM_UL_Begin (NULL);

		  for (NumItem = 0;
		       NumItem < Syllabus.LstItems.NumItems;
		       NumItem++)
		    {
		     HTM_LI_Begin (NULL);

			Level = Syllabus.LstItems.Lst[NumItem].Level;
			Syl_WriteNumItem (StrItemCod,NULL,Level,Syllabus.LstItems.Lst[NumItem].CodItem);
			HTM_TxtColonNBSP (StrItemCod);
			HTM_Txt (Syllabus.LstItems.Lst[NumItem].Text);

		     HTM_LI_End ();

		     /***** Insert new tree node *****/
		     Node.TreeType = TreeTypes[Syllabus.WhichSyllabus];
		     Tre_ResetNode (&Node);
		     Node.Hierarchy.NodInd = NumItem + 1;	// 1, 2, 3...
		     Node.Hierarchy.Level = (unsigned) Syllabus.LstItems.Lst[NumItem].Level;
		     Str_Copy (Node.Title,Syllabus.LstItems.Lst[NumItem].Text,Tre_MAX_BYTES_NODE_TITLE);
		     if (strlen (Syllabus.LstItems.Lst[NumItem].Text) <= Tre_MAX_BYTES_NODE_TITLE)
		        Syllabus.LstItems.Lst[NumItem].Text[0] = '\0';	// If text can be stored in title, don't store text
		     /*
		     Ale_ShowAlert (Ale_INFO,
				    "INSERT INTO tre_nodes"
					       " (CrsCod,Type,NodInd,Level,UsrCod,"
						 "StartTime,EndTime,"
						 "Title,Txt)"
				        " VALUES"
					       " (%ld,'%s',%u,%u,%ld,"
						 "FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
						 "'%s','%s')",
				     CrsCod,
				     Tre_DB_Types[Node.TreeType],
				     Node.Hierarchy.NodInd,
				     Node.Hierarchy.Level,
				     -1L,
				     Node.TimeUTC[Dat_STR_TIME],
				     Node.TimeUTC[Dat_END_TIME],
				     Node.Title,
				     Syllabus.LstItems.Lst[NumItem].Text);
		     */
		     Node.Hierarchy.NodCod =
		     DB_QueryINSERTandReturnCode ("can not create new tree node",
						  "INSERT INTO tre_nodes"
							     " (CrsCod,Type,NodInd,Level,UsrCod,"
							       "StartTime,EndTime,"
							       "Title,Txt)"
						      " VALUES (%ld,'%s',%u,%u,%ld,"
							       "FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
							       "'%s','%s')",
						   CrsCod,
						   Tre_DB_Types[Node.TreeType],
						   Node.Hierarchy.NodInd,
						   Node.Hierarchy.Level,
						   -1L,
						   Node.TimeUTC[Dat_STR_TIME],
						   Node.TimeUTC[Dat_END_TIME],
						   Node.Title,
						   Syllabus.LstItems.Lst[NumItem].Text);
		    }

		  HTM_UL_End ();

	       HTM_LI_End ();
	      }

	    /***** Free memory used to store items *****/
	    Syl_FreeListItemsSyllabus (&Syllabus);
	   }
	}
     }
   HTM_OL_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res_crs);
  }
