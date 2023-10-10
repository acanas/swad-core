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

const char *Nam_ParShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = "ShortName",
   [Nam_FULL_NAME] = "FullName",
  };
const char *Nam_FldShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = "ShortName",
   [Nam_FULL_NAME] = "FullName",
  };
unsigned Nam_MaxCharsShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = Nam_MAX_CHARS_SHRT_NAME,
   [Nam_FULL_NAME] = Nam_MAX_CHARS_FULL_NAME,
  };
unsigned Nam_MaxBytesShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = Nam_MAX_BYTES_SHRT_NAME,
   [Nam_FULL_NAME] = Nam_MAX_BYTES_FULL_NAME,
  };
const char *Nam_ClassShrtOrFullName[Nam_NUM_SHRT_FULL_NAMES] =
  {
   [Nam_SHRT_NAME] = "INPUT_SHORT_NAME",
   [Nam_FULL_NAME] = "INPUT_FULL_NAME",
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
