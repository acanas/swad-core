// swad_constant.c: common constants used in several modules

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_constant.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

const char *Cns_ParShrtOrFullName[Cns_NUM_SHRT_FULL_NAMES] =
  {
   [Cns_SHRT_NAME] = "ShortName",
   [Cns_FULL_NAME] = "FullName",
  };
const char *Cns_FldShrtOrFullName[Cns_NUM_SHRT_FULL_NAMES] =
  {
   [Cns_SHRT_NAME] = "ShortName",
   [Cns_FULL_NAME] = "FullName",
  };
unsigned Cns_MaxCharsShrtOrFullName[Cns_NUM_SHRT_FULL_NAMES] =
  {
   [Cns_SHRT_NAME] = Cns_MAX_CHARS_SHRT_NAME,
   [Cns_FULL_NAME] = Cns_MAX_CHARS_FULL_NAME,
  };
unsigned Cns_MaxBytesShrtOrFullName[Cns_NUM_SHRT_FULL_NAMES] =
  {
   [Cns_SHRT_NAME] = Cns_MAX_BYTES_SHRT_NAME,
   [Cns_FULL_NAME] = Cns_MAX_BYTES_FULL_NAME,
  };
const char *Cns_ClassShrtOrFullName[Cns_NUM_SHRT_FULL_NAMES] =
  {
   [Cns_SHRT_NAME] = "INPUT_SHORT_NAME",
   [Cns_FULL_NAME] = "INPUT_FULL_NAME",
  };

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

/*****************************************************************************/
/************************** Public global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/
