// swad_hierarchy_level.h: hierarchy levels: system, institution, center, degree, course

#ifndef _SWAD_HIE_LVL
#define _SWAD_HIE_LVL
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

#include "swad_constant.h"
#include "swad_string.h"

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

// Levels in the hierarchy
#define HieLvl_NUM_LEVELS	7
typedef enum
  {
   HieLvl_UNK = 0,	// Unknown
   HieLvl_SYS = 1,	// System
   HieLvl_CTY = 2,	// Country
   HieLvl_INS = 3,	// Institution
   HieLvl_CTR = 4,	// Center
   HieLvl_DEG = 5,	// Degree
   HieLvl_CRS = 6,	// Course
  } HieLvl_Level_t;

typedef enum
  {
   Hie_STATUS_BIT_PENDING = (1 << 0),	// Requested, but not yet activated
   Hie_STATUS_BIT_REMOVED = (1 << 1),	// Removed
  } Hie_Status_Bits_t;

typedef unsigned Hie_Status_t;
#define Hie_MAX_STATUS		((Hie_Status_t) 3)
#define Hie_WRONG_STATUS	((Hie_Status_t) (Hie_MAX_STATUS + 1))

#define Hie_NUM_STATUS_TXT	4
typedef enum
  {
   Hie_STATUS_UNKNOWN = 0,	// Other
   Hie_STATUS_ACTIVE  = 1,	// 00 (Status == 0)
   Hie_STATUS_PENDING = 2,	// 01 (Status == Hie_STATUS_BIT_PENDING)
   Hie_STATUS_REMOVED = 3,	// 1- (Status & Hie_STATUS_BIT_REMOVED)
  } Hie_StatusTxt_t;

#define Hie_MAX_CHARS_INSTITUTIONAL_COD  (16 - 1)	// 15
#define Hie_MAX_BYTES_INSTITUTIONAL_COD  ((Hie_MAX_CHARS_INSTITUTIONAL_COD + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 255

struct Hie_Node
  {
   long Cod;			// Course/degree/center/institution/country code
   long PrtCod;			// Parent code
   struct
     {
      long PlcCod;		// Center place code
      long TypCod;		// Degree type code
      unsigned Year;		// Course year: 0 (optatives), 1, 2, 3...
     } Specific;
   Hie_Status_t Status;		// Node status
   long RequesterUsrCod;	// User code of the person who requested the creation of this node
   char InstitutionalCod[Hie_MAX_BYTES_INSTITUTIONAL_COD + 1];	// Institutional code of the node
   char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1];	// Short name of the node
   char FullName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1];	// Full name of the node
   char WWW[Cns_MAX_BYTES_WWW + 1];
   struct
     {
      bool Valid;
      unsigned NumUsrs;
     } NumUsrsWhoClaimToBelong;
  };

#define Hie_NUM_ORDERS 2
typedef enum
  {
   Hie_ORDER_BY_NAME     = 0,
   Hie_ORDER_BY_NUM_USRS = 1,
  } Hie_Order_t;
#define Hie_ORDER_DEFAULT Hie_ORDER_BY_NUM_USRS

struct Hie_List
  {
   unsigned Num;		// Number of courses
   struct Hie_Node *Lst;	// List of courses
   Hie_Order_t SelectedOrder;	// Order of centers
  };

#endif
