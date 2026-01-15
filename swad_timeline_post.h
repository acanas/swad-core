// swad_timeline_post.h: social timeline posts

#ifndef _SWAD_TML_PST
#define _SWAD_TML_PST
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
/******************************** Public types *******************************/
/*****************************************************************************/

struct TmlPst_Content
  {
   char Txt[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Med_Media Media;
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TmlPst_GetAndWritePost (long PstCod);
void TmlPst_PutPhotoAndFormToWriteNewPost (struct Tml_Timeline *Timeline);

void TmlPst_PutTextarea (const char *Placeholder,const char *ClassTextArea);

void TmlPst_ReceivePostUsr (void);
void TmlPst_ReceivePostGbl (void);

#endif
