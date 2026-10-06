// swad_www.c: URLs

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include <stdarg.h>		// For va_start, va_end
#include <stdio.h>		// For FILE,fprintf
#include <stdlib.h>		// For exit, system, free, etc.
#include <string.h>		// For string functions

#include "swad_error.h"
#include "swad_www.h"

/*****************************************************************************/
/*************************** Build path using format *************************/
/*****************************************************************************/

void WWW_BuildURL (char WWW[WWW_MAX_BYTES_WWW + 1],const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Ptr;

   WWW[0] = '\0';
   if (fmt)
      if (fmt[0])
	{
	 va_start (ap,fmt);
	 NumBytesPrinted = vasprintf (&Ptr,fmt,ap);	// Number of bytes printed (excluding the null byte)
	 va_end (ap);
	 if (NumBytesPrinted < 0)	// -1 if no memory or any other error
	    Err_NotEnoughMemoryExit ();

	 /***** Print attributes *****/
	 if (NumBytesPrinted > WWW_MAX_BYTES_WWW)
            Err_URLTooLongExit ();

	 strcpy (WWW,Ptr);

	 free (Ptr);
	}
  }
