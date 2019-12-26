// swad_hierarchy.h: hierarchy (system, institution, centre, degree, course)

#ifndef _SWAD_HIE
#define _SWAD_HIE
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
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Hie_MAX_CHARS_SHRT_NAME	(32 - 1)	// 31
#define Hie_MAX_BYTES_SHRT_NAME	((Hie_MAX_CHARS_SHRT_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 511

#define Hie_MAX_CHARS_FULL_NAME	(128 - 1)	// 127
#define Hie_MAX_BYTES_FULL_NAME	((Hie_MAX_CHARS_FULL_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

// Levels in the hierarchy
#define Hie_NUM_LEVELS	7
typedef enum
  {
   Hie_UNK = 0,	// Unknown
   Hie_SYS = 1,	// System
   Hie_CTY = 2,	// Country
   Hie_INS = 3,	// Institution
   Hie_CTR = 4,	// Centre
   Hie_DEG = 5,	// Degree
   Hie_CRS = 6,	// Course
  } Hie_Level_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Hie_ConfigTitle (bool PutLink,
		      Hie_Level_t LogoScope,
		      long LogoCod,
                      char LogoShrtName[Hie_MAX_BYTES_SHRT_NAME + 1],
		      char LogoFullName[Hie_MAX_BYTES_FULL_NAME + 1],
		      char LogoWWW[Cns_MAX_BYTES_WWW + 1],
		      char TextFullName[Hie_MAX_BYTES_FULL_NAME + 1]);
void Hie_ConfigLabel (const char *Id,const char *Label);

void Hie_SeePending (void);

void Hie_WriteMenuHierarchy (void);
void Hie_WriteHierarchyInBreadcrumb (void);
void Hie_WriteBigNameCtyInsCtrDegCrs (void);

void Hie_SetHierarchyFromUsrLastHierarchy (void);
void Hie_InitHierarchy (void);
void Hie_ResetHierarchy (void);

void Hie_GetAndWriteInsCtrDegAdminBy (long UsrCod,unsigned ColSpan);

#endif
