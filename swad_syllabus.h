// swad_syllabus.h: syllabus

#ifndef _SWAD_SYL
#define _SWAD_SYL
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

#include "swad_view_edit.h"

/*****************************************************************************/
/************************ Public constants and types *************************/
/*****************************************************************************/

#define Syl_NUM_WHICH_SYLLABUS 2
typedef enum
  {
   Syl_LECTURES		= 0,
   Syl_PRACTICALS	= 1,
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

struct LstItemsSyllabus
  {
   struct ItemSyllabus *Lst;		// List of items of a syllabus
   unsigned NumItems;			// Number of items in the list
   unsigned NumItemsWithChildren;	// Number of items with children
   int NumLevels;			// Number of levels in the list
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

struct Syl_Syllabus
  {
   char PathDir[PATH_MAX + 1];
   unsigned NumItem;	// Item being edited
   unsigned ParNumItem;	// Used as parameter in forms
   Vie_ViewType_t ViewType;
   Syl_WhichSyllabus_t WhichSyllabus;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Syl_ResetSyllabus (struct Syl_Syllabus *Syllabus);

Syl_WhichSyllabus_t Syl_GetParWhichSyllabus (void);

bool Syl_CheckSyllabus (struct Syl_Syllabus *Syllabus,long CrsCod);
bool Syl_CheckAndEditSyllabus (struct Syl_Syllabus *Syllabus);
void Syl_EditSyllabus (void);

void Syl_LoadListItemsSyllabusIntoMemory (struct Syl_Syllabus *Syllabus,
                                          long CrsCod);
void Syl_FreeListItemsSyllabus (void);

int Syl_ReadLevelItemSyllabus (FILE *XML);

void Syl_WriteSyllabusIntoHTMLTmpFile (FILE *FileHTMLTmp);

void Syl_RemoveItemSyllabus (void);
void Syl_UpItemSyllabus (void);
void Syl_DownItemSyllabus (void);

void Syl_CalculateUpSubtreeSyllabus (struct MoveSubtrees *Subtree,unsigned NumItem);
void Syl_CalculateDownSubtreeSyllabus (struct MoveSubtrees *Subtree,unsigned NumItem);
void Syl_RightItemSyllabus (void);
void Syl_LeftItemSyllabus (void);

void Syl_InsertItemSyllabus (void);
void Syl_ModifyItemSyllabus (void);
void Syl_BuildPathFileSyllabus (const struct Syl_Syllabus *Syllabus,
                                char PathFile[PATH_MAX + 1]);
void Syl_WriteStartFileSyllabus (FILE *FileSyllabus);
void Syl_WriteAllItemsFileSyllabus (FILE *FileSyllabus);
void Syl_WriteItemFileSyllabus (FILE *FileSyllabus,int Level,const char *Text);
void Syl_WriteEndFileSyllabus (FILE *FileSyllabus);

#endif
