// swad_hidden_visible.c: types and constants related to hidden/visible

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

#include "swad_hidden_visible.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/* Hidden in database fields */
const char HidVis_YN[HidVis_NUM_HIDDEN_VISIBLE] =
  {
   [HidVis_HIDDEN ] = 'Y',
   [HidVis_VISIBLE] = 'N',
  };

const char *HidVis_DateBlueClass[HidVis_NUM_HIDDEN_VISIBLE] =
  {
   [HidVis_HIDDEN ] = "DATE_BLUE_LIGHT",
   [HidVis_VISIBLE] = "DATE_BLUE",
  };
const char *HidVis_TitleClass[HidVis_NUM_HIDDEN_VISIBLE] =
  {
   [HidVis_HIDDEN ] = "ASG_TITLE_LIGHT",
   [HidVis_VISIBLE] = "ASG_TITLE",
  };
const char *HidVis_GroupClass[HidVis_NUM_HIDDEN_VISIBLE] =
  {
   [HidVis_HIDDEN ] = "ASG_GRP_LIGHT",
   [HidVis_VISIBLE] = "ASG_GRP",
  };
const char *HidVis_LabelClass[HidVis_NUM_HIDDEN_VISIBLE] =
  {
   [HidVis_HIDDEN ] = "ASG_LABEL_LIGHT",
   [HidVis_VISIBLE] = "ASG_LABEL",
  };
const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE] =
  {
   [HidVis_HIDDEN ] = "DAT_LIGHT",
   [HidVis_VISIBLE] = "DAT",
  };
const char *HidVis_MsgClass[HidVis_NUM_HIDDEN_VISIBLE] =
  {
   [HidVis_HIDDEN ] = "MSG_AUT_LIGHT",
   [HidVis_VISIBLE] = "MSG_AUT",
  };
const char *HidVis_PrgClass[HidVis_NUM_HIDDEN_VISIBLE] =
  {
   [HidVis_HIDDEN ] = " PRG_HIDDEN",
   [HidVis_VISIBLE] = "",
  };
const char *HidVis_ShownStyle[HidVis_NUM_HIDDEN_VISIBLE] =
  {
   [HidVis_HIDDEN ] = " style=\"display:none;\"",
   [HidVis_VISIBLE] = "",
  };

/*****************************************************************************/
/**************** Get if hidden or visible from a character ******************/
/*****************************************************************************/

HidVis_HiddenOrVisible_t HidVid_GetHiddenOrVisible (char Ch)
  {
   return (Ch == 'Y') ? HidVis_HIDDEN :
		        HidVis_VISIBLE;
  }
