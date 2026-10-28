// swad_tag.h: tags for questions

#ifndef _SWAD_TAG
#define _SWAD_TAG
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_question_type.h"
#include "swad_string.h"
#include "swad_tag_type.h"

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
void Tag_PutPars (const void *TagCod);

void Tag_GetAndWriteTagsQst (long QstCod);
void Tag_ShowTagList (unsigned NumTags,MYSQL_RES *mysql_res);

void Tag_GetQstTagsByCod (struct Qst_Question *Qst);

#endif
