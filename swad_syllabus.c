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
#include "swad_syllabus.h"
#include "swad_tree.h"
#include "swad_tree_database.h"
#include "swad_xml.h"

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Syl_MAX_BYTES_ITEM_COD		(Syl_MAX_LEVELS_SYLLABUS * (10 + 1) - 1)

#define Syl_WIDTH_NUM_SYLLABUS 20

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Syl_WriteNumItem (char *StrDst,FILE *FileTgt,int Level,int *CodItem);

static void Syl_OpenSyllabusFile (const struct Syl_Syllabus *Syllabus,
                                  char PathFile[PATH_MAX + 1],
                                  FILE **XML);
static void Syl_CloseXMLFile (FILE **XML);

/*****************************************************************************/
/************************** Reset syllabus context ***************************/
/*****************************************************************************/

void Syl_ResetSyllabus (struct Syl_Syllabus *Syllabus)
  {
   Syllabus->LstItems.Lst = NULL;
   Syllabus->LstItems.NumItems = 0;
   Syllabus->LstItems.NumItemsWithChildren = 0;
   Syllabus->LstItems.NumLevels = 0;
   Syllabus->PathDir[0] = '\0';
   Syllabus->NumItem = 0;
   Syllabus->ParNumItem = 0;
  }

/*****************************************************************************/
/************************ Write form to select syllabus **********************/
/*****************************************************************************/

void Syl_PutFormWhichSyllabus (Inf_Type_t InfoType,Vie_ViewType_t ViewType)
  {
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   static Act_Action_t Actions[Vie_NUM_VIEW_TYPES] =
     {
      [Vie_VIEW		] = ActSeeSyl,
      [Vie_EDIT		] = ActEdiTreSyl,
      [Vie_CONFIG	] = ActCfgSyl,
      [Vie_PRINT	] = ActUnk,
     };
   Inf_Type_t Type;

   /***** If no syllabus ==> nothing to do *****/
   switch (InfoType)
     {
      case Inf_SYLLABUS_LEC:
      case Inf_SYLLABUS_PRA:
	 break;
      default:	// Nothing to do
	 return;
     }

   /***** Form to select which syllabus I want to see (lectures/practicals) *****/
   Frm_BeginForm (Actions[ViewType]);
      HTM_DIV_Begin ("class=\"SEL_BELOW_TITLE DAT_%s\"",The_GetSuffix ());
	 HTM_UL_Begin (NULL);

	    for (Type  = Inf_SYLLABUS_LEC;
		 Type <= Inf_SYLLABUS_PRA;
		 Type++)
	      {
	       HTM_LI_Begin (NULL);
		  HTM_LABEL_Begin (NULL);
		     HTM_INPUT_RADIO ("WhichSyllabus",
				      ((Type == InfoType) ? HTM_CHECKED :
							    HTM_NO_ATTR) | HTM_SUBMIT_ON_CLICK,
				      "value=\"%u\"",(unsigned) Type);
		     HTM_Txt (Txt_INFO_TITLE[Type]);
		  HTM_LABEL_End ();
	       HTM_LI_End ();
	      }

	 HTM_UL_End ();
      HTM_DIV_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/*** Read from XML and load in memory a syllabus of lectures or practicals ***/
/*****************************************************************************/

void Syl_LoadListItemsSyllabusIntoMemory (struct Syl_Syllabus *Syllabus,
					  Inf_Type_t InfoType,
                                          long CrsCod)
  {
   char PathFile[PATH_MAX + 1];
   const char *Folder;
   FILE *XML = NULL;	// XML file for syllabus
   long PostBeginList;
   unsigned NumItem = 0;
   int N;
   int CodItem[1 + Syl_MAX_LEVELS_SYLLABUS];	// To make numeration
   int Result;
   unsigned NumItemsWithChildren = 0;

   /***** Trivial check: syllabus should be lectures or practicals *****/
   switch (InfoType)
     {
      case Inf_SYLLABUS_LEC:
	 Folder = Cfg_SYLLABUS_FOLDER_LECTURES;
	 break;
      case Inf_SYLLABUS_PRA:
	 Folder = Cfg_SYLLABUS_FOLDER_PRACTICALS;
	 break;
      default:
	 Syl_ResetSyllabus (Syllabus);
	 return;
     }

   /***** If the course directory does not exist, create it *****/
   snprintf (Syllabus->PathDir,sizeof (Syllabus->PathDir),"%s/%ld",
	     Cfg_PATH_CRS_PRIVATE,CrsCod);
   Fil_CreateDirIfNotExists (Syllabus->PathDir);

   /***** Path of the private directory for the XML file with the syllabus *****/
   snprintf (Syllabus->PathDir,sizeof (Syllabus->PathDir),"%s/%ld/%s",
	     Cfg_PATH_CRS_PRIVATE,CrsCod,Folder);

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
      Syllabus->LstItems.NumItemsWithChildren = 0;
      Syllabus->LstItems.NumLevels = 0;
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
   extern const char *Tre_DB_Types[Inf_NUM_TYPES];
   Inf_Type_t InfoType;
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

   /***** Remove all syllabus from database *****/
   for (InfoType  = Inf_SYLLABUS_LEC;
	InfoType <= Inf_SYLLABUS_PRA;
	InfoType++)
      DB_QueryDELETE ("can not remove tree nodes",
		      "DELETE FROM tre_nodes"
		      " WHERE Type='%s'",
		      Tre_DB_Types[InfoType]);

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
	 for (InfoType  = Inf_SYLLABUS_LEC;
	      InfoType <= Inf_SYLLABUS_PRA;
	      InfoType++)
	   {
	    /***** Load syllabus from XML file to memory *****/
	    Syl_LoadListItemsSyllabusIntoMemory (&Syllabus,InfoType,CrsCod);

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
		     Node.InfoType = InfoType;
		     Tre_ResetNode (&Node);
		     Node.Hierarchy.NodInd = NumItem + 1;	// 1, 2, 3...
		     Node.Hierarchy.Level = (unsigned) Syllabus.LstItems.Lst[NumItem].Level;
		     Str_Copy (Node.Title,Syllabus.LstItems.Lst[NumItem].Text,Tre_MAX_BYTES_NODE_TITLE);
		     if (strlen (Syllabus.LstItems.Lst[NumItem].Text) <= Tre_MAX_BYTES_NODE_TITLE)
		        Syllabus.LstItems.Lst[NumItem].Text[0] = '\0';	// If text can be stored in title, don't store text

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
						   Tre_DB_Types[Node.InfoType],
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
