// swad_hidden_visible.c: types and constants related to hidden/visible

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

#include "swad_parameter.h"
#include "swad_show.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/* Show in database fields */
const char Sho_Show_YN[Sho_NUM_SHOW] =
  {
   [Sho_DONT_SHOW ] = 'N',
   [Sho_SHOW      ] = 'Y',
  };

/*****************************************************************************/
/******************* Get if show from a 'Y'/'N' character ********************/
/*****************************************************************************/

Sho_Show_t Sho_GetShowFromYN (char Ch)
  {
   return (Ch == 'Y') ? Sho_SHOW :
		        Sho_DONT_SHOW;
  }

/*****************************************************************************/
/************* Get parameter don't show/show from a Y/N parameter ************/
/*****************************************************************************/

Sho_Show_t Sho_GetParShow (const char *ParName)
  {
   return Par_GetParBool (ParName) ? Sho_SHOW :
				     Sho_DONT_SHOW;
  }
