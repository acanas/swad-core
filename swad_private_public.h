// swad_private_public.h: types and constants related to private/public

#ifndef _SWAD_PRI_PUB
#define _SWAD_PRI_PUB
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
/***************************** Public constants ******************************/
/*****************************************************************************/

#define PriPub_NUM_PRIVATE_PUBLIC 2

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   PriPub_PRIVATE,
   PriPub_PUBLIC,
  } PriPub_PrivateOrPublic_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

PriPub_PrivateOrPublic_t PriPub_GetPublicFromYN (char Ch);
PriPub_PrivateOrPublic_t PriPub_GetParPublic (void);

#endif
