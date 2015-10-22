// swad_syllabus.c: syllabus

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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
#include <string.h>		// For strcat (), etc.
#include <time.h>		// For time ()

#include "swad_changelog.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_global.h"
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
#define Syl_MAX_LENGTH_TEXT_ITEM	1024
#define Syl_MAX_BYTES_TEXT_ITEM		(Syl_MAX_LENGTH_TEXT_ITEM*Str_MAX_CHARACTER)

#define Syl_WIDTH_NUM_SYLLABUS 20

static const char *StyleSyllabus[1+Syl_MAX_LEVELS_SYLLABUS] =
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
   int CodItem[1+Syl_MAX_LEVELS_SYLLABUS];
   bool HasChildren;
   char Text[Syl_MAX_BYTES_TEXT_ITEM+1];
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

static Inf_InfoType_t Syl_SetSyllabusTypeAndLoadToMemory (void);
static void Syl_LoadToMemory (Inf_InfoType_t InfoType);
static void Syl_ShowSyllabus (Inf_InfoType_t InfoType);
static void Syl_ShowRowSyllabus (Inf_InfoType_t InfoType,unsigned NumItem,
                                 int Level,int *CodItem,const char *Text,bool NewItem);
static void Syl_WriteSyllabusIntoHTMLTmpFile (Inf_InfoType_t InfoType,FILE *FileHTMLTmp);
static void Syl_PutFormItemSyllabus (Inf_InfoType_t InfoType,bool NewItem,unsigned NumItem,int Level,int *CodItem,const char *Text);

/*****************************************************************************/
/******************** Get parameter to select a syllabus *********************/
/*****************************************************************************/

void Syl_GetParamWhichSyllabus (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   /***** Get which syllabus I want to see *****/
   Par_GetParToText ("WhichSyllabus",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      Gbl.CurrentCrs.Syllabus.WhichSyllabus = (Syl_WhichSyllabus_t) UnsignedNum;
   else
      Gbl.CurrentCrs.Syllabus.WhichSyllabus = Syl_DEFAULT_WHICH_SYLLABUS;
  }

/*****************************************************************************/
/************************ Write form to select syllabus **********************/
/*****************************************************************************/

void Syl_PutFormWhichSyllabus (void)
  {
   extern const char *Txt_SYLLABUS_WHICH_SYLLABUS[Syl_NUM_WHICH_SYLLABUS];
   Syl_WhichSyllabus_t WhichSyllabus;

   /***** Form to select which forums I want to see
          (all my forums or only the forums of current institution/degree/course) *****/
   Act_FormStart (ActSeeSyl);
   fprintf (Gbl.F.Out,"<div style=\"margin:12px 0;\">"
                      "<ul class=\"LIST_CENTER\">");

   for (WhichSyllabus = (Syl_WhichSyllabus_t) 0;
	WhichSyllabus < For_NUM_WHICH_FORUMS;
	WhichSyllabus++)
     {
      fprintf (Gbl.F.Out,"<li class=\"DAT LEFT_MIDDLE\""
	                 " style=\"display:inline;\">"
                         "<input type=\"radio\" name=\"WhichSyllabus\" value=\"%u\"",
               (unsigned) WhichSyllabus);
      if (WhichSyllabus == Gbl.CurrentCrs.Syllabus.WhichSyllabus)
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"document.getElementById('%s').submit();\" />"
	                 "%s"
                         "</li>",
               Gbl.FormId,Txt_SYLLABUS_WHICH_SYLLABUS[WhichSyllabus]);
     }
   fprintf (Gbl.F.Out,"</ul>"
	              "</div>");
   Act_FormEnd ();
  }

/*****************************************************************************/
/************** Get parameter item number in program edition *****************/
/*****************************************************************************/

void Syl_GetParamItemNumber (void)
  {
   char UnsignedStr[10+1];

   Par_GetParToText ("NumI",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&Gbl.CurrentCrs.Syllabus.NumItem) != 1)
      Lay_ShowErrorAndExit ("Item is missing.");
  }

/*****************************************************************************/
/****************************** Edit a syllabus ******************************/
/*****************************************************************************/

void Syl_EditSyllabus (void)
  {
   extern const Act_Action_t Inf_ActionsSeeInfo[Inf_NUM_INFO_TYPES];
   extern const char *Txt_View;
   extern const char *Txt_INFO_TITLE[Inf_NUM_INFO_TYPES];
   extern const char *Txt_This_syllabus_has_been_edited_by_teachers_of_the_course_;
   extern const char *Txt_The_syllabus_lectures_of_the_course_X_is_not_available;
   extern const char *Txt_The_syllabus_practicals_of_the_course_X_is_not_available;
   Inf_InfoType_t InfoType;

   /***** Set syllabus type and load syllabus from XML file to memory *****/
   InfoType = Syl_SetSyllabusTypeAndLoadToMemory ();

   if (Gbl.CurrentAct == ActEditorSylLec ||
       Gbl.CurrentAct == ActEditorSylPra)
      Gbl.CurrentCrs.Syllabus.EditionIsActive = true;

   if (Gbl.CurrentCrs.Syllabus.EditionIsActive || LstItemsSyllabus.NumItems)
     {
      if (Gbl.CurrentCrs.Syllabus.EditionIsActive)
	{
	 /***** Put link to view *****/
	 fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
	 Act_PutContextualLink (Inf_ActionsSeeInfo[InfoType],NULL,
				"visible_on",Txt_View);
	 fprintf (Gbl.F.Out,"</div>");
	}

      /***** Start of table *****/
      Lay_StartRoundFrameTable (NULL,1,Txt_INFO_TITLE[InfoType]);

      /***** Write the current syllabus *****/
      Syl_ShowSyllabus (InfoType);

      /***** If the syllabus is empty ==> show form to add a iten to the end *****/
      if (Gbl.CurrentCrs.Syllabus.EditionIsActive && LstItemsSyllabus.NumItems == 0)
         Syl_ShowRowSyllabus (InfoType,0,1,LstItemsSyllabus.Lst[0].CodItem,"",true);

      /***** End of table *****/
      Lay_EndRoundFrameTable ();

      if (!Gbl.CurrentCrs.Syllabus.EditionIsActive)
         fprintf (Gbl.F.Out,"<div class=\"DAT_SMALL CENTER_MIDDLE\">"
                            "<br />%s</div>",
                  Txt_This_syllabus_has_been_edited_by_teachers_of_the_course_);
     }
   else
     {
      sprintf (Gbl.Message,
               InfoType == Inf_LECTURES ? Txt_The_syllabus_lectures_of_the_course_X_is_not_available :
	                                  Txt_The_syllabus_practicals_of_the_course_X_is_not_available,
	       Gbl.CurrentCrs.Crs.FullName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
  }

/*****************************************************************************/
/************* Set syllabus type depending on the current action *************/
/*****************************************************************************/

static Inf_InfoType_t Syl_SetSyllabusTypeAndLoadToMemory (void)
  {
   Inf_InfoType_t InfoType = Inf_LECTURES;	// Initialized to avoid warning

   /***** Set the type of syllabus (lectures or practicals) *****/
   switch (Gbl.CurrentAct)
     {
      case ActSeeSyl:
	 InfoType = (Gbl.CurrentCrs.Syllabus.WhichSyllabus == Syl_LECTURES ? Inf_LECTURES :
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
	 Gbl.CurrentCrs.Syllabus.WhichSyllabus = Syl_LECTURES;
	 InfoType = Inf_LECTURES;
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
	 Gbl.CurrentCrs.Syllabus.WhichSyllabus = Syl_PRACTICALS;
	 InfoType = Inf_PRACTICALS;
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong action.");
	 break;
     }

   /***** We are editing a syllabus with the internal editor,
          so change info source to internal editor in database *****/
   Inf_SetInfoSrcIntoDB (Gbl.CurrentCrs.Crs.CrsCod,
	                 InfoType,Inf_INFO_SRC_EDITOR);

   /***** Load syllabus from XML file to memory *****/
   Syl_LoadToMemory (InfoType);

   return InfoType;
  }

/*****************************************************************************/
/*** Read from XML and load in memory a syllabus of lectures or practicals ***/
/*****************************************************************************/

static void Syl_LoadToMemory (Inf_InfoType_t InfoType)
  {
   char PathFile[PATH_MAX+1];
   long PostBeginList;
   unsigned NumItem = 0;
   int N;
   int CodItem[1+Syl_MAX_LEVELS_SYLLABUS];	// To make numeration
   int Result;
   unsigned NumItemsWithChildren = 0;

   /* Path of the private directory for the XML file with the syllabus */
   sprintf (Gbl.CurrentCrs.Syllabus.PathDir,"%s/%s",
	    Gbl.CurrentCrs.PathPriv,
	    InfoType == Inf_LECTURES ? Cfg_SYLLABUS_FOLDER_LECTURES :
		                       Cfg_SYLLABUS_FOLDER_PRACTICALS);

   /***** Open the file with the syllabus *****/
   Syl_OpenSyllabusFile (Gbl.CurrentCrs.Syllabus.PathDir,PathFile);

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
      Lay_ShowErrorAndExit ("Not enough memory to store syllabus.");

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
	 Result = Str_ReceiveFileUntilDelimitStr (Gbl.F.XML, NULL, LstItemsSyllabus.Lst[NumItem].Text,"</item>",
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
	{
	 if (LstItemsSyllabus.Lst[NumItem].Level < LstItemsSyllabus.Lst[NumItem + 1].Level)
	   {
	    LstItemsSyllabus.Lst[NumItem].HasChildren = true;
	    NumItemsWithChildren++;
	   }
	 else
	    LstItemsSyllabus.Lst[NumItem].HasChildren = false;
	}
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
   char StrlLevel[11+1];

   if (!Str_FindStrInFile (Gbl.F.XML,"nivel=\"",Str_NO_SKIP_HTML_COMMENTS))
      Lay_ShowErrorAndExit ("Wrong syllabus format.");
   if (Str_ReceiveFileUntilDelimitStr (Gbl.F.XML,NULL,StrlLevel,"\"",
   	                               (unsigned long long) (11+1)) != 1)
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

static void Syl_ShowSyllabus (Inf_InfoType_t InfoType)
  {
   unsigned NumItem;
   int i;
   int NumButtons = Gbl.CurrentCrs.Syllabus.EditionIsActive ? 5 :
	                                                      0;
   bool ShowRowInsertNewItem = (Gbl.CurrentAct == ActInsIteSylLec || Gbl.CurrentAct == ActInsIteSylPra ||
                                Gbl.CurrentAct == ActModIteSylLec || Gbl.CurrentAct == ActModIteSylPra ||
				Gbl.CurrentAct == ActRgtIteSylLec || Gbl.CurrentAct == ActRgtIteSylPra ||
                                Gbl.CurrentAct == ActLftIteSylLec || Gbl.CurrentAct == ActLftIteSylPra);

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

   /***** Loop for writing all items of the syllabus *****/
   for (NumItem = 0;
	NumItem < LstItemsSyllabus.NumItems;
	NumItem++)
     {
      Syl_ShowRowSyllabus (InfoType,NumItem,
                           LstItemsSyllabus.Lst[NumItem].Level,
                           LstItemsSyllabus.Lst[NumItem].CodItem,
                           LstItemsSyllabus.Lst[NumItem].Text,false);
      if (ShowRowInsertNewItem && NumItem == Gbl.CurrentCrs.Syllabus.NumItem)
	 // Mostrar a new row where se puede insert a new item
	 Syl_ShowRowSyllabus (InfoType,NumItem + 1,
	                      LstItemsSyllabus.Lst[NumItem].Level,NULL,
	                      "",true);
     }
  }

/*****************************************************************************/
/******** Write a row (item) of a syllabus of lectures or practicals *********/
/*****************************************************************************/

static void Syl_ShowRowSyllabus (Inf_InfoType_t InfoType,unsigned NumItem,
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
   char StrItemCod[Syl_MAX_LEVELS_SYLLABUS*(10+1)];
   struct MoveSubtrees Subtree;

   Subtree.ToGetUp.Ini   = Subtree.ToGetUp.End   = 0;
   Subtree.ToGetDown.Ini = Subtree.ToGetDown.End = 0;
   Subtree.MovAllowed = false;

   if (!NewItem)	// If the item is new (not stored in file), it has no number
      Syl_WriteNumItem (StrItemCod,NULL,Level,CodItem);

   /***** Start the row *****/
   fprintf (Gbl.F.Out,"<tr>");

   if (Gbl.CurrentCrs.Syllabus.EditionIsActive)
     {
      if (NewItem)
         fprintf (Gbl.F.Out,"<td colspan=\"5\" class=\"COLOR%u\">"
                            "</td>",
	          Gbl.RowEvenOdd);
      else
	{
	 /***** Icon to remove the row *****/
         fprintf (Gbl.F.Out,"<td class=\"BM%u\">",Gbl.RowEvenOdd);
	 if (LstItemsSyllabus.Lst[NumItem].HasChildren)
            Lay_PutIconRemovalNotAllowed ();
	 else
	   {
	    Act_FormStart (InfoType == Inf_LECTURES ? ActDelItmSylLec :
		                                      ActDelItmSylPra);
	    Syl_PutParamNumItem (NumItem);
            Lay_PutIconRemove ();
            Act_FormEnd ();
	   }
         fprintf (Gbl.F.Out,"</td>");

	 /***** Icon to get up an item *****/
	 Syl_CalculateUpSubtreeSyllabus (&Subtree,NumItem);
	 fprintf (Gbl.F.Out,"<td class=\"BM%u\">",Gbl.RowEvenOdd);
	 if (Subtree.MovAllowed)
	   {
	    Act_FormStart (InfoType == Inf_LECTURES ? ActUp_IteSylLec :
	                                              ActUp_IteSylPra);
	    Syl_PutParamNumItem (NumItem);
            sprintf (Gbl.Title,
                     LstItemsSyllabus.Lst[NumItem].HasChildren ? Txt_Move_up_X_and_its_subsections :
                                                                 Txt_Move_up_X,
                     StrItemCod);
	    fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/up_on16x16.gif\""
		               " alt=\"%s\" title=\"%s\""
		               " class=\"ICON16x16\" />",
                     Gbl.Prefs.IconsURL,
                     Gbl.Title,
                     Gbl.Title);
	    Act_FormEnd ();
	   }
	 else
	    fprintf (Gbl.F.Out,"<img src=\"%s/up_off16x16.gif\""
		               " alt=\"%s\" title=\"%s\""
		               " class=\"ICON16x16\" />",
                     Gbl.Prefs.IconsURL,
                     Txt_Movement_not_allowed,
                     Txt_Movement_not_allowed);
         fprintf (Gbl.F.Out,"</td>");

	 /***** Icon to get down item *****/
	 Syl_CalculateDownSubtreeSyllabus (&Subtree,NumItem);
	 fprintf (Gbl.F.Out,"<td class=\"BM%u\">",Gbl.RowEvenOdd);
	 if (Subtree.MovAllowed)
	   {
	    Act_FormStart (InfoType == Inf_LECTURES ? ActDwnIteSylLec :
		                                      ActDwnIteSylPra);
	    Syl_PutParamNumItem (NumItem);
            sprintf (Gbl.Title,
                     LstItemsSyllabus.Lst[NumItem].HasChildren ? Txt_Move_down_X_and_its_subsections :
                                                                 Txt_Move_down_X,
                     StrItemCod);
	    fprintf (Gbl.F.Out,"<input type=\"image\""
		               " src=\"%s/down_on16x16.gif\""
		               " alt=\"%s\" title=\"%s\""
		               " class=\"ICON16x16\" />",
                     Gbl.Prefs.IconsURL,
                     Gbl.Title,
                     Gbl.Title);
	    Act_FormEnd ();
	   }
	 else
	    fprintf (Gbl.F.Out,"<img src=\"%s/down_off16x16.gif\""
		               " alt=\"%s\" title=\"%s\""
		               " class=\"ICON16x16\" />",
                     Gbl.Prefs.IconsURL,
                     Txt_Movement_not_allowed,
                     Txt_Movement_not_allowed);
         fprintf (Gbl.F.Out,"</td>");

	 /***** Icon to increase the level of an item *****/
	 fprintf (Gbl.F.Out,"<td class=\"BM%u\">",Gbl.RowEvenOdd);
	 if (Level > 1)
	   {
	    Act_FormStart (InfoType == Inf_LECTURES ? ActRgtIteSylLec :
                                                      ActRgtIteSylPra);
	    Syl_PutParamNumItem (NumItem);
	    sprintf (Gbl.Title,Txt_Increase_level_of_X,
                     StrItemCod);
	    fprintf (Gbl.F.Out,"<input type=\"image\""
		               " src=\"%s/left_on16x16.gif\""
		               " alt=\"%s\" title=\"%s\""
		               " class=\"ICON16x16\" />",
                     Gbl.Prefs.IconsURL,
                     Gbl.Title,
                     Gbl.Title);
	    Act_FormEnd ();
	   }
	 else
	    fprintf (Gbl.F.Out,"<img src=\"%s/left_off16x16.gif\""
		               " alt=\"%s\" title=\"%s\""
		               " class=\"ICON16x16\" />",
                     Gbl.Prefs.IconsURL,
                     Txt_Movement_not_allowed,
                     Txt_Movement_not_allowed);
         fprintf (Gbl.F.Out,"</td>");

	 /***** Icon to decrease level item *****/
	 fprintf (Gbl.F.Out,"<td class=\"BM%u\">",Gbl.RowEvenOdd);
	 if (Level < LastLevel + 1 &&
	     Level < Syl_MAX_LEVELS_SYLLABUS)
	   {
	    Act_FormStart (InfoType == Inf_LECTURES ? ActLftIteSylLec :
                                                      ActLftIteSylPra);
	    Syl_PutParamNumItem (NumItem);
	    sprintf (Gbl.Title,Txt_Decrease_level_of_X,
                     StrItemCod);
	    fprintf (Gbl.F.Out,"<input type=\"image\""
		               " src=\"%s/right_on16x16.gif\""
		               " alt=\"%s\" title=\"%s\""
		               " class=\"ICON16x16\" />",
                     Gbl.Prefs.IconsURL,
                     Gbl.Title,
                     Gbl.Title);
	    Act_FormEnd ();
	   }
	 else
	    fprintf (Gbl.F.Out,"<img src=\"%s/right_off16x16.gif\""
		               " alt=\"%s\" title=\"%s\""
		               " class=\"ICON16x16\" />",
                     Gbl.Prefs.IconsURL,
                     Txt_Movement_not_allowed,
                     Txt_Movement_not_allowed);
         fprintf (Gbl.F.Out,"</td>");

	 LastLevel = Level;
	}
     }

   if (Gbl.CurrentCrs.Syllabus.EditionIsActive)
      Syl_PutFormItemSyllabus (InfoType,NewItem,NumItem,Level,CodItem,Text);
   else
     {
      /***** Indent depending on the level *****/
      if (Level > 1)
	 fprintf (Gbl.F.Out,"<td colspan=\"%d\" class=\"COLOR%u\">"
	                    "</td>",
                  Level - 1,Gbl.RowEvenOdd);

      /***** Code of the item *****/
      fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_TOP COLOR%u\""
	                 " style=\"width:%dpx;\">",
               StyleSyllabus[Level],Gbl.RowEvenOdd,
               Level * Syl_WIDTH_NUM_SYLLABUS);
      if (Level == 1)
	 fprintf (Gbl.F.Out,"&nbsp;");
      fprintf (Gbl.F.Out,"%s&nbsp;</td>",StrItemCod);

      /***** Text of the item *****/
      fprintf (Gbl.F.Out,"<td colspan=\"%d\" class=\"%s LEFT_TOP COLOR%u\">"
                         "%s"
                         "</td>",
               LstItemsSyllabus.NumLevels - Level + 1,
               StyleSyllabus[Level],
               Gbl.RowEvenOdd,
               Text);
     }

   /***** End of the row *****/
   fprintf (Gbl.F.Out,"</tr>");

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/************** Write the syllabus into a temporary HTML file ****************/
/*****************************************************************************/
// This function is called only from web service

int Syl_WriteSyllabusIntoHTMLBuffer (Inf_InfoType_t InfoType,char **HTMLBuffer)
  {
   char FileNameHTMLTmp[PATH_MAX+1];
   FILE *FileHTMLTmp;
   size_t Length;

   /***** Initialize buffer *****/
   *HTMLBuffer = NULL;

   /***** Load syllabus from XML file to list of items in memory *****/
   Syl_LoadToMemory (InfoType);

   if (LstItemsSyllabus.NumItems)
     {
      /***** Create a unique name for the file *****/
      sprintf (FileNameHTMLTmp,"%s/%s/%s_syllabus.html",
	       Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_OUT,Gbl.UniqueNameEncrypted);

      /***** Create a new temporary file for writing and reading *****/
      if ((FileHTMLTmp = fopen (FileNameHTMLTmp,"w+b")) == NULL)
	{
         Syl_FreeListItemsSyllabus ();
         return soap_receiver_fault (Gbl.soap,
                                     "Syllabus can not be copied into buffer",
                                     "Can not create temporary file");
	}

      /***** Write syllabus in HTML into a temporary file *****/
      Syl_WriteSyllabusIntoHTMLTmpFile (InfoType,FileHTMLTmp);

      /***** Write syllabus from list of items in memory to text buffer *****/
      /* Compute length of file */
      Length = (size_t) ftell (FileHTMLTmp);

      /* Allocate memory for buffer */
      if ((*HTMLBuffer = (char *) malloc (Length+1)) == NULL)
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

static void Syl_WriteSyllabusIntoHTMLTmpFile (Inf_InfoType_t InfoType,FILE *FileHTMLTmp)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   extern const char *Txt_INFO_TITLE[Inf_NUM_INFO_TYPES];
   unsigned NumItem;
   int i;

   /***** Write start of HTML code *****/
   fprintf (FileHTMLTmp,"<!DOCTYPE html>\n"
                         "<html lang=\"%s\">\n"
                         "<head>\n"
                         "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=windows-1252\" />\n"
                         "<title>%s</title>\n"
                         "</head>\n"
                         "<body>\n"
			 "<table>\n",
            Txt_STR_LANG_ID[Gbl.Prefs.Language],	// Language
            Txt_INFO_TITLE[InfoType]);			// Page title

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
      fprintf (FileHTMLTmp,"<td class=\"%s RIGHT_TOP\" style=\"width:%dpx;\">",
	       StyleSyllabus[LstItemsSyllabus.Lst[NumItem].Level],
	       LstItemsSyllabus.Lst[NumItem].Level * Syl_WIDTH_NUM_SYLLABUS);
      if (LstItemsSyllabus.Lst[NumItem].Level == 1)
	 fprintf (FileHTMLTmp,"&nbsp;");
      Syl_WriteNumItem (NULL,FileHTMLTmp,
			LstItemsSyllabus.Lst[NumItem].Level,
			LstItemsSyllabus.Lst[NumItem].CodItem);
      fprintf (FileHTMLTmp,"&nbsp;</td>");

      /***** Text of the item *****/
      fprintf (FileHTMLTmp,"<td colspan=\"%d\" class=\"%s LEFT_TOP\">"
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

static void Syl_PutFormItemSyllabus (Inf_InfoType_t InfoType,bool NewItem,unsigned NumItem,int Level,int *CodItem,const char *Text)
  {
   extern const char *Txt_Enter_a_new_item_here;

   if (Level < 1)
      Level = 1;

   /***** Indent depending on the level *****/
   if (Level > 1)
      fprintf (Gbl.F.Out,"<td colspan=\"%d\" class=\"COLOR%u\">"
	                 "</td>",
               Level - 1,Gbl.RowEvenOdd);

   /***** Write the code of the item *****/
   if (NewItem)	// If the item is new (not stored in the file) ==> it has not a number
      fprintf (Gbl.F.Out,"<td class=\"COLOR%u\" style=\"width:%dpx;\">"
	                 "</td>",
               Gbl.RowEvenOdd,Level * Syl_WIDTH_NUM_SYLLABUS);
   else
     {
      fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE COLOR%u\""
	                 " style=\"width:%dpx;\">",
               StyleSyllabus[Level],Gbl.RowEvenOdd,
               Level * Syl_WIDTH_NUM_SYLLABUS);
      if (Level == 1)
	 fprintf (Gbl.F.Out,"&nbsp;");
      Syl_WriteNumItem (NULL,Gbl.F.Out,Level,CodItem);
      fprintf (Gbl.F.Out,"&nbsp;</td>");
     }

   /***** Text of the item *****/
   fprintf (Gbl.F.Out,"<td colspan=\"%d\" class=\"LEFT_MIDDLE COLOR%u\">",
            LstItemsSyllabus.NumLevels - Level + 1,Gbl.RowEvenOdd);
   Act_FormStart (NewItem ? (InfoType == Inf_LECTURES ? ActInsIteSylLec :
	                                                ActInsIteSylPra) :
                            (InfoType == Inf_LECTURES ? ActModIteSylLec :
                        	                        ActModIteSylPra));
   Syl_PutParamNumItem (NumItem);
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Txt\""
	              " size=\"80\" maxlength=\"%u\" value=\"%s\""
                      " placeholder=\"%s\""
                      " onchange=\"document.getElementById('%s').submit();\" />",
	    Syl_MAX_LENGTH_TEXT_ITEM,Text,
	    Txt_Enter_a_new_item_here,
	    Gbl.FormId);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/***** Write parameter with the number of an item in a syllabus form *********/
/*****************************************************************************/

void Syl_PutParamNumItem (unsigned NumItem)
  {
   Par_PutHiddenParamUnsigned ("NumI",NumItem);
  }

/*****************************************************************************/
/******************** Write number of item in legal style ********************/
/*****************************************************************************/

void Syl_WriteNumItem (char *StrDst,FILE *FileTgt,int Level,int *CodItem)
  {
   int N;
   char InStr[1+10+1];

   if (StrDst)
      StrDst[0] = '\0';

   for (N = 1;
	N <= Level;
	N++)
     {
      if (N > 1)
	{
	 if (StrDst)
	    strcat (StrDst,".");
	 if (FileTgt)
	    fprintf (FileTgt,".");
	}
      sprintf (InStr,"%d",CodItem[N]);
      if (StrDst)
	 strcat (StrDst,InStr);
      if (FileTgt)
	 fprintf (FileTgt,"%s",InStr);
     }
  }

/*****************************************************************************/
/********************** Remove an item from syllabus *************************/
/*****************************************************************************/

void Syl_RemoveItemSyllabus (void)
  {
   char PathFile[PATH_MAX+1],PathOldFile[PATH_MAX+1],PathNewFile[PATH_MAX+1];
   FILE *NewFile;
   unsigned NumItem;

   /***** Load syllabus from XML file to memory *****/
   Syl_SetSyllabusTypeAndLoadToMemory ();

   Gbl.CurrentCrs.Syllabus.EditionIsActive = true;

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
      if (NumItem != Gbl.CurrentCrs.Syllabus.NumItem)
	 Syl_WriteItemFileSyllabus (NewFile,LstItemsSyllabus.Lst[NumItem].Level,LstItemsSyllabus.Lst[NumItem].Text);
   Syl_WriteEndFileSyllabus (NewFile);

   /***** Close the files *****/
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

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
   char PathFile[PATH_MAX+1],PathOldFile[PATH_MAX+1],PathNewFile[PATH_MAX+1];
   FILE *NewFile;
   unsigned NumItem;
   struct MoveSubtrees Subtree;

   /***** Load syllabus from XML file to memory *****/
   Syl_SetSyllabusTypeAndLoadToMemory ();

   Gbl.CurrentCrs.Syllabus.EditionIsActive = true;

   /***** Get item number *****/
   Syl_GetParamItemNumber ();

   Subtree.ToGetUp.Ini   = Subtree.ToGetUp.End   = 0;
   Subtree.ToGetDown.Ini = Subtree.ToGetDown.End = 0;
   Subtree.MovAllowed = false;

   if (Gbl.CurrentCrs.Syllabus.NumItem < LstItemsSyllabus.NumItems)
     {
      /***** Create a new file where make the update *****/
      Syl_BuildPathFileSyllabus (PathFile);
      Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

      /***** Get up or get down position *****/
      switch (UpOrDownPos)
	{
	 case Syl_GET_UP:
	    Syl_CalculateUpSubtreeSyllabus (&Subtree,Gbl.CurrentCrs.Syllabus.NumItem);
	    break;
	 case Syl_GET_DOWN:
	    Syl_CalculateDownSubtreeSyllabus (&Subtree,Gbl.CurrentCrs.Syllabus.NumItem);
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
   char PathFile[PATH_MAX+1],PathOldFile[PATH_MAX+1],PathNewFile[PATH_MAX+1];
   FILE *NewFile;

   /***** Load syllabus from XML file to memory *****/
   Syl_SetSyllabusTypeAndLoadToMemory ();

   Gbl.CurrentCrs.Syllabus.EditionIsActive = true;

   /***** Get item number *****/
   Syl_GetParamItemNumber ();

   /***** Create a new file to do the update *****/
   Syl_BuildPathFileSyllabus (PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   /***** Increase or decrease level *****/
   switch (IncreaseOrDecreaseLevel)
     {
      case Syl_INCREASE_LEVEL:
	 if (LstItemsSyllabus.Lst[Gbl.CurrentCrs.Syllabus.NumItem].Level > 1)
	    LstItemsSyllabus.Lst[Gbl.CurrentCrs.Syllabus.NumItem].Level--;
	 break;
      case Syl_DECREASE_LEVEL:
	 if (LstItemsSyllabus.Lst[Gbl.CurrentCrs.Syllabus.NumItem].Level < Syl_MAX_LEVELS_SYLLABUS)
	    LstItemsSyllabus.Lst[Gbl.CurrentCrs.Syllabus.NumItem].Level++;
	 break;
     }

   /***** Create the new XML file *****/
   Syl_WriteStartFileSyllabus (NewFile);
   Syl_WriteAllItemsFileSyllabus (NewFile);
   Syl_WriteEndFileSyllabus (NewFile);

   /***** Close the files *****/
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

   /***** Show the updated syllabus to continue editing it *****/
   Syl_FreeListItemsSyllabus ();
   Syl_EditSyllabus ();
  }

/*****************************************************************************/
/************************ Insert an item in a syllabus ***********************/
/*****************************************************************************/

void Syl_InsertItemSyllabus (void)
  {
   char PathFile[PATH_MAX+1],PathOldFile[PATH_MAX+1],PathNewFile[PATH_MAX+1];
   FILE *NewFile;
   unsigned NumItem;
   char Txt[Syl_MAX_BYTES_TEXT_ITEM+1];

   /***** Load syllabus from XML file to memory *****/
   Syl_SetSyllabusTypeAndLoadToMemory ();

   Gbl.CurrentCrs.Syllabus.EditionIsActive = true;

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
	NumItem < Gbl.CurrentCrs.Syllabus.NumItem;
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

   /***** Show the updated syllabus to continue editing it *****/
   Syl_FreeListItemsSyllabus ();
   Syl_EditSyllabus ();
  }

/*****************************************************************************/
/**************** Modify and existing item of the syllabus *******************/
/*****************************************************************************/

void Syl_ModifyItemSyllabus (void)
  {
   char PathFile[PATH_MAX+1],PathOldFile[PATH_MAX+1],PathNewFile[PATH_MAX+1];
   FILE *NewFile;

   /***** Load syllabus from XML file to memory *****/
   Syl_SetSyllabusTypeAndLoadToMemory ();

   Gbl.CurrentCrs.Syllabus.EditionIsActive = true;

   /***** Get item number *****/
   Syl_GetParamItemNumber ();

   /***** Get item body *****/
   Par_GetParToHTML ("Txt",LstItemsSyllabus.Lst[Gbl.CurrentCrs.Syllabus.NumItem].Text,Syl_MAX_BYTES_TEXT_ITEM);

   /***** Create a new file where make the update *****/
   Syl_BuildPathFileSyllabus (PathFile);
   Fil_CreateUpdateFile (PathFile,".old",PathOldFile,PathNewFile,&NewFile);

   /***** Create the new XML file *****/
   Syl_WriteStartFileSyllabus (NewFile);
   Syl_WriteAllItemsFileSyllabus (NewFile);
   Syl_WriteEndFileSyllabus (NewFile);

   /***** Close the files *****/
   Fil_CloseUpdateFile (PathFile,PathOldFile,PathNewFile,NewFile);

   /***** Show the updated syllabus to continue editing it *****/
   Syl_FreeListItemsSyllabus ();
   Syl_EditSyllabus ();
  }

/*****************************************************************************/
/************ Build the path of the file XML with the syllabus ***************/
/*****************************************************************************/

void Syl_BuildPathFileSyllabus (char *PathFile)
  {
   sprintf (PathFile,"%s/%s",Gbl.CurrentCrs.Syllabus.PathDir,Cfg_SYLLABUS_FILENAME);
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
