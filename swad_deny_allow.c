// swad_deny_allow.c: types and constants related to deny/allow

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

#include "swad_deny_allow.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/* Correct in database fields */
const char DenAll_Allow_YN[DenAll_NUM_DENY_ALLOW] =
  {
   [DenAll_DENY ] = 'N',
   [DenAll_ALLOW] = 'Y',
  };

/*****************************************************************************/
/************** Get if deny or allow from a 'Y'/'N' character ****************/
/*****************************************************************************/

DenAll_DenyOrAllow_t DenAll_GetAllowFromYN (char Ch)
  {
   return (Ch == 'Y') ? DenAll_ALLOW :
			DenAll_DENY;
  }
