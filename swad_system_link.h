// swad_system_link.h: system links

#ifndef _SWAD_SYS_LNK
#define _SWAD_SYS_LNK
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
/************************** Public types and constants ***********************/
/*****************************************************************************/

struct SysLnk_Link
  {
   long LnkCod;
   char ShrtName[Nam_MAX_BYTES_SHRT_NAME + 1];
   char FullName[Nam_MAX_BYTES_FULL_NAME + 1];
   char WWW[WWW_MAX_BYTES_WWW + 1];
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void SysLnk_SeeLinks (void);
void SysLnk_WriteMenuWithSystemLinks (void);

void SysLnk_EditLinks (void);

void SysLnk_PutIconToViewLinks (void);

void SysLnk_GetLinkDataByCod (struct SysLnk_Link *Lnk);
void SysLnk_RemoveLink (void);
void SysLnk_RenameLinkShort (void);
void SysLnk_RenameLinkFull (void);
void SysLnk_ChangeLinkWWW (void);
void SysLnk_ContEditAfterChgLnk (void);
void SysLnk_ReceiveNewLink (void);

#endif
