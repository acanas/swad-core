// swad_program_database.h: course program, operations with database

#ifndef _SWAD_PRG_DB
#define _SWAD_PRG_DB
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include "swad_hierarchy_level.h"
#include "swad_program.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

long Prg_DB_InsertItem (const struct Prg_Item *Item,const char *Txt);
void Prg_DB_UpdateItem (const struct Prg_Item *Item,const char *Txt);
void Prg_DB_HideOrUnhideItem (long ItmCod,bool Hide);
void Prg_DB_UpdateIndexRange (long Diff,long Begin,long End);
void Prg_DB_LockTable (void);
void Prg_DB_UnlockTable (void);
void Prg_DB_MoveDownItems (unsigned Index);
void Prg_DB_MoveLeftRightItemRange (const struct Prg_ItemRange *ToMove,
                                    Prg_MoveLeftRight_t LeftRight);

unsigned Prg_DB_GetListItems (MYSQL_RES **mysql_res);
unsigned Prg_DB_GetDataOfItemByCod (MYSQL_RES **mysql_res,long ItmCod);
void Prg_DB_GetItemTxt (long ItmCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);

unsigned Prg_DB_GetListResources (MYSQL_RES **mysql_res,long ItmCod);
unsigned Prg_DB_GetDataOfResourceByCod (MYSQL_RES **mysql_res,
                                        long ItmCod,long RscCod);

unsigned Prg_DB_GetNumCoursesWithItems (HieLvl_Level_t Scope);
unsigned Prg_DB_GetNumItems (HieLvl_Level_t Scope);

void Prg_DB_RemoveItemRange (const struct Prg_ItemRange *ToRemove);
void Prg_DB_RemoveCrsItems (long CrsCod);

#endif
