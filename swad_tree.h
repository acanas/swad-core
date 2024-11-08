// swad_tree.h: course tree

#ifndef _SWAD_TRE
#define _SWAD_TRE
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

#include <stdbool.h>		// For boolean type
#include <time.h>		// For time

#include "swad_database.h"
#include "swad_date.h"
#include "swad_resource.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Tre_NUM_TYPES 9
typedef enum
  {
   Tre_UNKNOWN,		// Not assigned, unknown
   Tre_PROGRAM,		// Program
   Tre_GUIDE,		// Teaching guide
   Tre_SYLLABUS_LEC,	// Syllabus (lectures)
   Tre_SYLLABUS_PRA,	// Syllabus (practicals)
   Tre_BIBLIOGRAPHY,	// Bibliography
   Tre_FAQ,		// FAQ
   Tre_LINKS,		// Links
   Tre_ASSESSMENT,	// Assessment system
  } Tre_TreeType_t;

#define Tre_MAX_CHARS_NODE_TITLE	(128 - 1)	// 127
#define Tre_MAX_BYTES_NODE_TITLE	((Tre_MAX_CHARS_NODE_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

struct Tre_NodeHierarchy
  {
   long NodCod;
   unsigned NodInd;	// 1, 2, 3...
   unsigned Level;
   HidVis_HiddenOrVisible_t HiddenOrVisible;
  };

struct Tre_Node
  {
   Tre_TreeType_t TreeType;
   struct Tre_NodeHierarchy Hierarchy;
   long UsrCod;
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   CloOpe_ClosedOrOpen_t ClosedOrOpen;
   char Title[Tre_MAX_BYTES_NODE_TITLE + 1];
   struct Rsc_Resource Resource;
  };

struct Tre_NodeRange
  {
   unsigned Begin;	// Index of the first node in the subtree
   unsigned End;	// Index of the last node in the subtree
  };

#define Tre_NUM_MOVEMENTS_LEFT_RIGHT 2
typedef enum
  {
   Tre_MOVE_LEFT,
   Tre_MOVE_RIGHT,
  } Tre_MoveLeftRight_t;

#define Tre_NUM_MOVEMENTS_UP_DOWN 2
typedef enum
  {
   Tre_MOVE_UP,
   Tre_MOVE_DOWN,
  } Tre_MoveUpDown_t;

#define Tre_NUM_MOVEMENTS_EXPAND_CONTRACT 2
typedef enum
  {
   Tre_EXPAND,
   Tre_CONTRACT,
  } Tre_ExpandContract_t;

#define Tre_NUM_LISTING_TYPES 12
typedef enum
  {
   Tre_PRINT,			// List nodes ready to be printed
   Tre_VIEW,			// List nodes without any edition

   Tre_EDIT_NODES,		// Buttons to edit list of nodes
   Tre_FORM_NEW_END_NODE,	// Form to create a new first level node at the end
   Tre_FORM_NEW_CHILD_NODE,	// Form to create a new child node
   Tre_FORM_EDIT_NODE,		// Form to edit a selected node
   Tre_END_EDIT_NODE,		// List node after edition
   Tre_RECEIVE_NODE,		// Receive node data after create/edit

   Tre_EDIT_PRG_RESOURCES,	// List resources of a selected node for edition
   Tre_EDIT_PRG_RESOURCE_LINK,	// Show clipboard in a resource to select a link
   Tre_CHG_PRG_RESOURCE_LINK,	// Change resource link
   Tre_END_EDIT_PRG_RESOURCES,	// List resources of a selected node after edition
  } Tre_ListingType_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

unsigned Tre_ShowTree (void);
void Tre_EditTree (void);
void Tre_ShowAllNodes (Tre_TreeType_t TreeType,
		       Tre_ListingType_t ListingType,
                       long SelectedItmCod,long SelectedRscCod);

Usr_Can_t Tre_CheckIfICanEditTree (void);
void Tre_PutIconToEditTree (struct Tre_Node *Node);
void Tre_PutIconToViewTree (struct Tre_Node *Node);
void Tre_PutIconToCreateNewNode (struct Tre_Node *Node);

void Tre_PutPars (void *Node);
void Tre_GetPars (struct Tre_Node *Node);

void Tre_GetListNodes (Tre_TreeType_t TreeType);
void Tre_FreeListNodes (void);

void Tre_ResetNode (struct Tre_Node *Node);

unsigned Tre_GetNumNodeFromNodCod (long NodCod);
long Tre_GetNodCodFromNumNode (unsigned NumNode);
unsigned Tre_GetNodIndFromNumNode (unsigned NumNode);
unsigned Tre_GetLevelFromNumNode (unsigned NumNode);

void Tre_ViewNodeAfterEdit (void);
void Tre_ReqChangeNode (void);
void Tre_ReqCreateNode (void);
void Tre_ReceiveChgNode (void);
void Tre_ReceiveNewNode (void);
void Tre_ReqRemNode (void);
void Tre_RemoveNode (void);
void Tre_HideOrUnhideNode (HidVis_HiddenOrVisible_t HiddenOrVisible);
void Tre_MoveUpDownNode (Tre_MoveUpDown_t UpDown);
void Tre_MoveLeftRightNode (Tre_MoveLeftRight_t LeftRight);
void Tre_ExpandContractNode (Tre_ExpandContract_t ExpandContract,
			     Tre_ListingType_t ListingType);

#endif
