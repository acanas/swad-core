// swad_private_public.c: types and constants related to private/public

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_private_public.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/* Public in database fields */
const char PriPub_Public_YN[PriPub_NUM_PRIVATE_PUBLIC] =
  {
   [PriPub_PRIVATE] = 'N',
   [PriPub_PUBLIC ] = 'Y',
  };

/*****************************************************************************/
/************* Get if private or public from a 'Y'/'N' character *************/
/*****************************************************************************/

PriPub_PrivateOrPublic_t PriPub_GetPublicFromYN (char Ch)
  {
   return (Ch == 'Y') ? PriPub_PUBLIC :
			PriPub_PRIVATE;
  }
