// swad_timeline_share.h: social timeline shared

#ifndef _SWAD_TL_SHA
#define _SWAD_TL_SHA
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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
/********************************** Headers **********************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TL_Sha_ShowAllSharersNoteUsr (void);
void TL_Sha_ShowAllSharersNoteGbl (void);
void TL_Sha_ShaNoteUsr (void);
void TL_Sha_ShaNoteGbl (void);
void TL_Sha_UnsNoteUsr (void);
void TL_Sha_UnsNoteGbl (void);
void TL_Sha_PutFormToShaUnsNote (const struct TL_Note *SocNot,
                                 TL_HowManyUsrs_t HowManyUsrs);

void TL_Sha_UpdateNumTimesANoteHasBeenShared (struct TL_Note *SocNot);

#endif
