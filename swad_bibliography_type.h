// swad_bibliography_type.h: definition of types for course bibliography

#ifndef _SWAD_BIB_TYP
#define _SWAD_BIB_TYP
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_constant.h"
#include "swad_www.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Bib_MAX_CHARS_AUTHORS	(128 - 1)	// 127
#define Bib_MAX_BYTES_AUTHORS	((Bib_MAX_CHARS_AUTHORS + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Bib_MAX_CHARS_TITLE	(128 - 1)	// 127
#define Bib_MAX_BYTES_TITLE	((Bib_MAX_CHARS_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

struct Bib_BibliographicReference
  {
   char Authors[Bib_MAX_BYTES_AUTHORS + 1];	// List of authors
   char Title[Bib_MAX_BYTES_TITLE + 1];		// Title
   char Source[Bib_MAX_BYTES_TITLE + 1];	// Collection, book, volume, pages
   char Publisher[Bib_MAX_BYTES_TITLE + 1];	// Including place of publication
   char Date[Bib_MAX_BYTES_TITLE + 1];		// Usually year
   char Id[Bib_MAX_BYTES_TITLE + 1];		// Identifier (ISBN, ISSN, DOI...)
   char URL[WWW_MAX_BYTES_WWW + 1];		// URL
  };

#endif
