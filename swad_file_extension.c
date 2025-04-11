// swad_file_extension.c: file extensions allowed in file browsers

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_file_extension.h"
#include "swad_string.h"

/*****************************************************************************/
/*************************** Private constants *******************************/
/*****************************************************************************/

/* Extensions allowed for uploaded files */
const char *Ext_FileExtAllowed[Ext_NUM_FILE_EXT_ALLOWED] =
  {
   "3gp"  ,	// Video Android mobile
   "7z"   ,
   "asm"  ,
   "avi"  ,
   "bas"  ,
   "bat"  ,
   "bbl"  ,
   "bib"  ,
   "bin"  ,
   "bmp"  ,
   "bz2"  ,
   "c"    ,
   "cc"   ,
   "cct"  ,	// LogicWorks
   "cdr"  ,
   "clf"  ,	// LogicWorks
   "cpp"  ,
   "css"  ,
   "csv"  ,
   "dmg"  ,
   "doc"  ,
   "docx" ,
   "dotm" ,
   "dwd"  ,
   "eps"  ,
   "fdf"  ,
   "flv"  ,
   "gdb"  ,	// GNU Debugger Script
   "ggb"  ,
   "ggt"  ,
   "gif"  ,
   "gz"   ,
   "h"    ,
   "hex"  ,
   "htm"  ,
   "html" ,
   "img"  ,
   "ipynb",	// Jupyter Notebook
   "java" ,
   "jpg"  ,
   "jpeg" ,
   "latex",
   "m"    ,
   "m4a"  ,	// MPEG-4 de audio
   "mdb"  ,
   "mht"  ,
   "mhtml",
   "mid"  ,
   "mov"  ,
   "mp3"  ,
   "mp4"  ,
   "mpeg" ,
   "mpg"  ,
   "mpp"  ,
   "mus"  ,
   "nb"   ,
   "odb"  ,
   "odc"  ,
   "odf"  ,
   "odg"  ,
   "odi"  ,
   "odm"  ,
   "odp"  ,
   "ods"  ,
   "odt"  ,
   "otg"  ,
   "otp"  ,
   "ots"  ,
   "ott"  ,
   "pas"  ,
   "pl"   ,
   "pdf"  ,
   "png"  ,
   "pps"  ,
   "ppsx" ,
   "ppt"  ,
   "pptx" ,
   "ps"   ,
   "pss"  ,
   "qt"   ,
   "r"    ,
   "ram"  ,
   "rar"  ,
   "raw"  ,
   "rdata",
   "rm"   ,
   "rp"   ,	// Axure, http://www.axure.com/
   "rtf"  ,
   "s"    ,
   "sav"  ,	// SPSS Data File
   "sbs"  ,
   "sf3"  ,
   "sgp"  ,
   "spp"  ,
   "spo"  ,
   "sps"  ,
   "swf"  ,
   "sws"  ,
   "tar"  ,
   "tex"  ,
   "tgz"  ,
   "tif"  ,
   "tim"  ,	// LogicWorks
   "txt"  ,
   "voc"  ,
   "vp"   ,	// Justinmind, http://www.justinmind.com/
   "wav"  ,
   "wma"  ,
   "wmv"  ,
   "wxm"  ,
   "wxmx" ,
   "xls"  ,
   "xlsx" ,
   "zip"  ,
  };

/*****************************************************************************/
/******************* Check if file extension is allowed **********************/
/*****************************************************************************/

bool Ext_CheckIfFileExtensionIsAllowed (const char FilFolLnkName[NAME_MAX + 1])
  {
   unsigned Type;

   for (Type = 0;
	Type < Ext_NUM_FILE_EXT_ALLOWED;
	Type++)
      if (Str_FileIs (FilFolLnkName,Ext_FileExtAllowed[Type]))
	 return true;

   return false;
  }
