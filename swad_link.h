// swad_link.h: institutional links

#ifndef _SWAD_LNK
#define _SWAD_LNK
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Lnk_MAX_CHARS_LINK_SHRT_NAME	32
#define Lnk_MAX_BYTES_LINK_SHRT_NAME	(Lnk_MAX_CHARS_LINK_SHRT_NAME * Str_MAX_BYTES_PER_CHAR)

#define Lnk_MAX_CHARS_LINK_FULL_NAME	(128 - 1)
#define Lnk_MAX_BYTES_LINK_FULL_NAME	(Lnk_MAX_CHARS_LINK_FULL_NAME * Str_MAX_BYTES_PER_CHAR)

struct Link
  {
   long LnkCod;
   char ShrtName[Lnk_MAX_BYTES_LINK_SHRT_NAME + 1];
   char FullName[Lnk_MAX_BYTES_LINK_FULL_NAME + 1];
   char WWW[Cns_MAX_BYTES_WWW + 1];
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Lnk_SeeLinks (void);
void Lnk_WriteMenuWithInstitutionalLinks (void);

void Lnk_EditLinks (void);
void Lnk_GetListLinks (void);
void Lnk_FreeListLinks (void);
void Lnk_GetDataOfLinkByCod (struct Link *Lnk);
long Lnk_GetParamLnkCod (void);
void Lnk_RemoveLink (void);
void Lnk_RenameLinkShort (void);
void Lnk_RenameLinkFull (void);
void Lnk_ChangeLinkWWW (void);
void Lnk_RecFormNewLink (void);

#endif
