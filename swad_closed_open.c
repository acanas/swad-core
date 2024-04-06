// swad_closed_open.c: types and constants related to closed/open

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/* Open in database fields */
const char CloOpe_YN[CloOpe_NUM_CLOSED_OPEN] =
  {
   [CloOpe_CLOSED] = 'Y',
   [CloOpe_OPEN  ] = 'N',
  };
/*
const char *CloOpe_DateGreenClass[CloOpe_NUM_CLOSED_OPEN] =
  {
   [CloOpe_CLOSED] = "DATE_GREEN_LIGHT",
   [CloOpe_OPEN  ] = "DATE_GREEN",
  };
const char *CloOpe_DateRedClass[CloOpe_NUM_CLOSED_OPEN] =
  {
   [CloOpe_CLOSED] = "DATE_RED_LIGHT",
   [CloOpe_OPEN  ] = "DATE_RED",
  };
const char *CloOpe_DateBlueClass[CloOpe_NUM_CLOSED_OPEN] =
  {
   [CloOpe_CLOSED ] = "DATE_BLUE_LIGHT",
   [CloOpe_OPEN] = "DATE_BLUE",
  };
const char *CloOpe_TitleClass[CloOpe_NUM_CLOSED_OPEN] =
  {
   [CloOpe_CLOSED] = "ASG_TITLE_LIGHT",
   [CloOpe_OPEN  ] = "ASG_TITLE",
  };
const char *CloOpe_GroupClass[CloOpe_NUM_CLOSED_OPEN] =
  {
   [CloOpe_CLOSED] = "ASG_GRP_LIGHT",
   [CloOpe_OPEN  ] = "ASG_GRP",
  };
const char *CloOpe_LabelClass[CloOpe_NUM_CLOSED_OPEN] =
  {
   [CloOpe_CLOSED] = "ASG_LABEL_LIGHT",
   [CloOpe_OPEN  ] = "ASG_LABEL",
  };
const char *CloOpe_DataClass[CloOpe_NUM_CLOSED_OPEN] =
  {
   [CloOpe_CLOSED] = "DAT_LIGHT",
   [CloOpe_OPEN  ] = "DAT",
  };
const char *CloOpe_MsgClass[CloOpe_NUM_CLOSED_OPEN] =
  {
   [CloOpe_CLOSED] = "MSG_AUT_LIGHT",
   [CloOpe_OPEN  ] = "MSG_AUT",
  };
const char *CloOpe_PrgClass[CloOpe_NUM_CLOSED_OPEN] =
  {
   [CloOpe_CLOSED] = " PRG_HIDDEN",
   [CloOpe_OPEN  ] = "",
  };
const char *CloOpe_ShownStyle[CloOpe_NUM_CLOSED_OPEN] =
  {
   [CloOpe_CLOSED] = " style=\"display:none;\"",
   [CloOpe_OPEN  ] = "",
  };
*/
/*****************************************************************************/
/****************** Get if closed or open from a character *******************/
/*****************************************************************************/

CloOpe_ClosedOrOpen_t CloOpe_GetClosedOrOpen (char Ch)
  {
   return (Ch == 'Y') ? CloOpe_OPEN :
		        CloOpe_CLOSED;
  }

