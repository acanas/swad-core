// swad_file_extension.h: file extensions allowed in file browsers

#ifndef _SWAD_EXT
#define _SWAD_EXT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Ca�as Vargas

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <linux/limits.h>	// For NAME_MAX

#include "swad_error.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Ext_NUM_FILE_EXT_ALLOWED 114

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

Err_SuccessOrError_t Ext_CheckIfFileExtensionIsAllowed (const char FilFolLnkName[NAME_MAX + 1]);

#endif
