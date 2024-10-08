// swad_tag.h: tags for questions

#ifndef _SWAD_TAG
#define _SWAD_TAG
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_string.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Tag_MAX_TAGS_PER_QUESTION	5

#define Tag_MAX_CHARS_TAG		(128 - 1)	// 127
#define Tag_MAX_BYTES_TAG		((Tag_MAX_CHARS_TAG + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Tag_MAX_BYTES_TAGS_LIST		(16 * 1024)

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct Tag_Tags
  {
   long PreselectedTagCod;	// If > 0 ==> only one tag is preselected, students can not select
   unsigned Num;
   unsigned NumSelected;
   bool All;
   long *ListSelectedTagCods;
   char Txt[Tag_MAX_TAGS_PER_QUESTION][Tag_MAX_BYTES_TAG + 1];
  };

typedef enum
  {
   Tag_SHOW_ALL_TAGS,
   Tag_SHOW_ONLY_VISIBLE_TAGS,
  } Tag_ShowAllOrVisibleTags_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Tag_ResetTags (struct Tag_Tags *Tags);
void Tag_FreeTagsList (struct Tag_Tags *Tags);

void Tag_PutIconToEditTags (void);

void Tag_EnableTag (void);
void Tag_DisableTag (void);
void Tag_RenameTag (void);

void Tag_InsertTagsIntoDB (long QstCod,const struct Tag_Tags *Tags);

void Tag_ShowFormSelTags (const struct Tag_Tags *Tags,MYSQL_RES *mysql_res,
                          Tag_ShowAllOrVisibleTags_t ShowAllOrVisibleTags);
void Tag_ShowFormEditTags (void);
void Tag_PutPars (void *TagCod);

void Tag_GetAndWriteTagsQst (long QstCod);
void Tag_ShowTagList (unsigned NumTags,MYSQL_RES *mysql_res);

#endif
