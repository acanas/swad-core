// swad_closed_open.c: types and constants related to closed/open

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

#include "swad_closed_open.h"
#include "swad_hidden_visible.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *CloOpe_Txt[CloOpe_NUM_CLOSED_OPEN] =
  {
   [CloOpe_CLOSED] = "closed",
   [CloOpe_OPEN  ] = "open",
  };

const char *CloOpe_Class[CloOpe_NUM_CLOSED_OPEN][HidVis_NUM_HIDDEN_VISIBLE] =
  {
   [CloOpe_CLOSED][HidVis_HIDDEN ] = "DATE_RED_LIGHT",
   [CloOpe_CLOSED][HidVis_VISIBLE] = "DATE_RED",
   [CloOpe_OPEN  ][HidVis_HIDDEN ] = "DATE_GREEN_LIGHT",
   [CloOpe_OPEN  ][HidVis_VISIBLE] = "DATE_GREEN",
  };

/*****************************************************************************/
/************** Get if closed or open from a '0'/'1' character ***************/
/*****************************************************************************/

CloOpe_ClosedOrOpen_t CloOpe_GetClosedOrOpenFrom01 (char Ch)
  {
   return (Ch == '1') ? CloOpe_OPEN :
		        CloOpe_CLOSED;
  }

/*****************************************************************************/
/************** Get if closed or open from a 'Y'/'N' character ***************/
/*****************************************************************************/

CloOpe_ClosedOrOpen_t CloOpe_GetClosedOrOpenFromYN (char Ch)
  {
   return (Ch == 'Y') ? CloOpe_OPEN :
		        CloOpe_CLOSED;
  }
