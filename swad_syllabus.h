// swad_syllabus.h: syllabus

#ifndef _SWAD_SYL
#define _SWAD_SYL
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

#include "swad_tree.h"
#include "swad_view.h"

/*****************************************************************************/
/************************ Public constants and types *************************/
/*****************************************************************************/

#define Syl_NUM_WHICH_SYLLABUS 3
typedef enum
  {
   Syl_NONE		= 0,
   Syl_LECTURES		= 1,
   Syl_PRACTICALS	= 2,
  } Syl_WhichSyllabus_t;	// Which syllabus I want to see
#define Syl_DEFAULT_WHICH_SYLLABUS Syl_LECTURES

#define Syl_NUM_CHANGE_POS_ITEM 2
typedef enum
  {
   Syl_GET_UP,
   Syl_GET_DOWN,
  } Syl_ChangePosItem_t;

typedef enum
  {
   Syl_INCREASE_LEVEL,
   Syl_DECREASE_LEVEL,
  } Syl_ChangeLevelItem_t;

#define Syl_MAX_LEVELS_SYLLABUS		  10

#define Syl_MAX_CHARS_TEXT_ITEM		(1024 - 1)	// 1023
#define Syl_MAX_BYTES_TEXT_ITEM		((Syl_MAX_CHARS_TEXT_ITEM + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 16383

struct ItemSyllabus
  {
   int Level;
   int CodItem[1 + Syl_MAX_LEVELS_SYLLABUS];
   bool HasChildren;
   char Text[Syl_MAX_BYTES_TEXT_ITEM + 1];
  };

struct LstItemsSyllabus
  {
   struct ItemSyllabus *Lst;		// List of items of a syllabus
   unsigned NumItems;			// Number of items in the list
   unsigned NumItemsWithChildren;	// Number of items with children
   int NumLevels;			// Number of levels in the list
  };

struct Syl_Syllabus
  {
   struct LstItemsSyllabus LstItems;
   char PathDir[PATH_MAX + 1];
   unsigned NumItem;	// Item being edited
   unsigned ParNumItem;	// Used as parameter in forms
   Vie_ViewType_t ViewType;
   Syl_WhichSyllabus_t WhichSyllabus;
  };

// Structure used to get the limits (number of items) of the subtrees to exchange in a syllabus
struct MoveSubtrees
  {
   struct
     {
      unsigned Ini,End;
     } ToGetUp,ToGetDown; // Number of initial and ending items of the two subtrees to get up and get down
   bool MovAllowed;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Syl_ResetSyllabus (struct Syl_Syllabus *Syllabus);

Syl_WhichSyllabus_t Syl_GetParWhichSyllabus (void);
void Syl_PutParWhichSyllabus (void *SyllabusSelected);
void Syl_PutFormWhichSyllabus (Syl_WhichSyllabus_t WhichSyllabus);

bool Syl_CheckSyllabus (Tre_TreeType_t TreeType);

void Syl_LoadListItemsSyllabusIntoMemory (struct Syl_Syllabus *Syllabus,
                                          long CrsCod);
void Syl_FreeListItemsSyllabus (struct Syl_Syllabus *Syllabus);

int Syl_ReadLevelItemSyllabus (FILE *XML);

void Syl_WriteSyllabusIntoHTMLTmpFile (struct Syl_Syllabus *Syllabus,
				       FILE *FileHTMLTmp);

void Syl_BuildPathFileSyllabus (const struct Syl_Syllabus *Syllabus,
                                char PathFile[PATH_MAX + 1]);
void Syl_WriteStartFileSyllabus (FILE *FileSyllabus);
void Syl_WriteEndFileSyllabus (FILE *FileSyllabus);

//--------------------- Convert syllabus from file to tree --------------------
void Syl_PutLinkToConvertSyllabus (void);
void Syl_ConvertAllSyllabus (void);

#endif
