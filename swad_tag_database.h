// swad_tag_database.h: tags for questions, operations with database

#ifndef _SWAD_TAG_DB
#define _SWAD_TAG_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include <stdbool.h>		// For boolean type

#include "swad_tag.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

long Tag_DB_CreateNewTag (long CrsCod,const char *TagTxt);
void Tag_DB_AddTagToQst (long QstCod,long TagCod,unsigned TagInd);
void Tag_DB_CreateTmpTableQuestionsWithTag (long TagCod);
void Tag_DB_DropTmpTableQuestionsWithTag (void);
void Tag_DB_ComplexRenameTag (long TagCodOldTxt,long ExistingTagCodNewTxt);
void Tag_DB_SimplexRenameTag (const char OldTagTxt[Tag_MAX_BYTES_TAG + 1],
                              const char NewTagTxt[Tag_MAX_BYTES_TAG + 1]);
void Tag_DB_EnableOrDisableTag (long TagCod,bool TagHidden);

unsigned Tag_DB_GetTagsQst (MYSQL_RES **mysql_res,long QstCod);
unsigned Tag_DB_GetAllTagsFromCurrentCrs (MYSQL_RES **mysql_res);
unsigned Tag_DB_GetEnabledTagsFromCrs (MYSQL_RES **mysql_res,
				       long TagCod,long CrsCod);
Exi_Exist_t Tag_DB_CheckIfTagsExistInCurrentCrs (void);
long Tag_DB_GetTagCodFromTagTxt (const char *TagTxt);
void Tag_DB_GetTagTitle (long TagCod,char *Title,size_t TitleSize);

unsigned Tag_DB_GetRecentTags (MYSQL_RES **mysql_res,
		               long CrsCod,time_t BeginTime);

void Tag_DB_RemTagsInQstsInCrs (long CrsCod);
void Tag_DB_RemTagsInCrs (long CrsCod);
void Tag_DB_RemTagsFromQst (long QstCod);
void Tag_DB_RemoveTag (long TagCod);
void Tag_DB_RemoveUnusedTagsFromCrs (long CrsCod);

#endif
