// swad_syllabus.h: syllabus

#ifndef _SWAD_SYL
#define _SWAD_SYL
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

void Syl_GetParamWhichSyllabus (void);
void Syl_PutFormWhichSyllabus (void);

void Syl_GetParamItemNumber (void);

bool Syl_CheckSyllabus (long CrsCod,Inf_InfoType_t InfoType);
bool Syl_CheckAndEditSyllabus (void);
void Syl_EditSyllabus (void);

void Syl_LoadListItemsSyllabusIntoMemory (long CrsCod);
void Syl_FreeListItemsSyllabus (void);

int Syl_ReadLevelItemSyllabus (void);
int Syl_WriteSyllabusIntoHTMLBuffer (char **HTMLBuffer);

void Syl_RemoveItemSyllabus (void);
void Syl_UpItemSyllabus (void);
void Syl_DownItemSyllabus (void);
void Syl_ChangePlaceItemSyllabus (Syl_ChangePosItem_t UpOrDownPos);
void Syl_CalculateUpSubtreeSyllabus (struct MoveSubtrees *Subtree,unsigned NumItem);
void Syl_CalculateDownSubtreeSyllabus (struct MoveSubtrees *Subtree,unsigned NumItem);
void Syl_RightItemSyllabus (void);
void Syl_LeftItemSyllabus (void);
void Syl_ChangeLevelItemSyllabus (Syl_ChangeLevelItem_t IncreaseOrDecreaseLevel);
void Syl_InsertItemSyllabus (void);
void Syl_ModifyItemSyllabus (void);
void Syl_BuildPathFileSyllabus (char *PathFile);
void Syl_OpenSyllabusFile (const char *PathDir,char *PathFile);
void Syl_WriteStartFileSyllabus (FILE *FileSyllabus);
void Syl_WriteAllItemsFileSyllabus (FILE *FileSyllabus);
void Syl_WriteItemFileSyllabus (FILE *FileSyllabus,int Level,const char *Text);
void Syl_WriteEndFileSyllabus (FILE *FileSyllabus);

#endif
