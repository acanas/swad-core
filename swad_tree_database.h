// swad_tree_database.h: course tree, operations with database

#ifndef _SWAD_TRE_DB
#define _SWAD_TRE_DB
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

#include "swad_contracted_expanded.h"
#include "swad_hierarchy_type.h"
#include "swad_program.h"
#include "swad_program_resource.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//----------------------------- Tree nodes ------------------------------------
long Tre_DB_InsertNode (const struct Tre_Node *Node,const char *Txt);
void Tre_DB_UpdateNode (const struct Tre_Node *Node,const char *Txt);
void Tre_DB_HideOrUnhideNode (const struct Tre_Node *Node,
			      HidVis_HiddenOrVisible_t HiddenOrVisible);
void Tre_DB_UpdateIndexRange (Inf_Type_t InfoType,long Diff,long Begin,long End);
void Tre_DB_LockTableNodes (void);
void Tre_DB_MoveDownNodes (Inf_Type_t InfoType,unsigned Index);
void Tre_DB_MoveLeftRightNodeRange (Inf_Type_t InfoType,
				    const struct Tre_NodeRange *ToMove,
                                    Tre_MoveLeftRight_t LeftRight);

unsigned Tre_DB_GetListNodes (Inf_Type_t InfoType,MYSQL_RES **mysql_res);
unsigned Tre_DB_GetNodeDataByCod (const struct Tre_Node *Node,MYSQL_RES **mysql_res);
void Tre_DB_GetNodeTxt (const struct Tre_Node *Node,
			char Txt[Cns_MAX_BYTES_TEXT + 1]);

unsigned Tre_DB_GetNumCoursesWithNodes (Inf_Type_t InfoType,Hie_Level_t Level);
unsigned Tre_DB_GetNumNodes (Inf_Type_t InfoType,Hie_Level_t Level);

void Tre_DB_RemoveNodeRange (Inf_Type_t InfoType,
			     const struct Tre_NodeRange *ToRemove);
void Tre_DB_RemoveCrsNodes (long CrsCod);

//--------------------------- Expanded tree nodes -----------------------------
void Tre_DB_InsertNodeInExpandedNodes (long NodCod);
ConExp_ContractedOrExpanded_t Tre_DB_GetIfContractedOrExpandedNode (long NodCod);
void Tre_DB_RemoveNodeFromExpandedNodes (long NodCod);

#endif
