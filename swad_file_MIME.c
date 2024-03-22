// swad_file_MIME.c: allowed MIME types in files in file browsers

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <string.h>	// For strcmp

#include "swad_file_MIME.h"

/*****************************************************************************/
/*************************** Private constants *******************************/
/*****************************************************************************/

/* MIME types allowed for uploades files */
#define MIM_NUM_MIME_TYPES_ALLOWED 200
const char *MIM_MIMETypesAllowed[MIM_NUM_MIME_TYPES_ALLOWED] =
  {
   "application/",			//
   "application/acrobat",		// PDF
   "application/arj",			// compressed archive arj
   "application/binary",		//
   "application/bzip2",			// Bzip 2 UNIX Compressed File
   "application/cdr",			// Corel Draw (CDR)
   "application/coreldraw",		// Corel Draw (CDR)
   "application/css-stylesheet",	// Hypertext Cascading Style Sheet
   "application/csv",			// CSV, Comma Separated Values
   "application/data",			//
   "application/download",		// zip files in Firefox caused by an error?
   "application/excel",			// Microsoft Excel xls
   "application/finale",		// Finale .mus
   "application/force",			// PDF uploaded from Firefox
   "application/force-download",	// RAR uploaded from Firefox
   "application/futuresplash",		// Flash
   "application/gzip",			// GNU ZIP gz, gzip
   "application/gzip-compressed",	// GNU ZIP gz, gzip
   "application/gzipped",		// GNU ZIP gz, gzip
   "application/msaccess",		// Microsoft Access mdb
   "application/msexcel",		// Microsoft Excel xla, xls, xlt, xlw
   "application/mspowerpoint",		// Microsoft PowerPoint pot, pps, ppt
   "application/mathematica",		// Mathematica
   "application/matlab",		// Matlab
   "application/mfile",			// Matlab
   "application/mpp",			// Microsoft Project mpp
   "application/msproj",		// Microsoft Project mpp
   "application/msproject",		// Microsoft Project mpp
   "application/msword",		// Microsoft Word doc, word, w6w
   "application/mswrite",		// Microsoft Write wri
   "application/octet",			// uninterpreted binary bin
   "application/octet-binary",
   "application/octetstream",		// uninterpreted binary bin
   "application/octet-stream",		// uninterpreted binary bin
   "application/pdf",			// Adobe Acrobat pdf
   "application/postscript",		// PostScript ai, eps, ps
   "application/powerpoint",		// Microsoft PowerPoint pot, pps, ppt
   "application/rar",			// RAR
   "application/rtf",			// RTF
   "application/self-extracting",	// Compressed file, self-extracting
   "application/stream",		// PDF in Mac?
   "application/unknown",
   "application/vnd.fdf",		// Forms Data Format
   "application/vnd.geogebra.file",	// GeoGebra .ggb
   "application/vnd.geogebra.tool",	// GeoGebra .ggt
   "application/vnd.jupyter",		// Jupyter and IPython
   "application/vnd.jupyter.cells",	// Jupyter and IPython
   "application/vnd.jupyter.dragindex",	// Jupyter and IPython
   "application/vnd.msexcel",		// Microsoft Excel .xls
   "application/vnd.ms-excel",		// Microsoft Excel .xls
   "application/vnd.ms-powerpoint",	// Microsoft PowerPoint .ppt or .pps
   "application/vnd.ms-project",	// Microsoft Project .mpp
   "application/vnd.ms-word",		// Microsoft Word .doc
   "application/vnd.ms-word.template.macroenabled.12",		// Microsoft Word template .dotm
   "application/vnd.oasis.opendocument.text",			// OpenOffice Text 			.odt
   "application/vnd.oasis.opendocument.spreadsheet",		// OpenOffice Hoja of cálculo 		.ods
   "application/vnd.oasis.opendocument.presentation",		// OpenOffice Presentación 		.odp
   "application/vnd.oasis.opendocument.graphics",		// OpenOffice Dibujo 			.odg
   "application/vnd.oasis.opendocument.chart",			// OpenOffice Gráfica 			.odc
   "application/vnd.oasis.opendocument.formula",		// OpenOffice Fórmula matemática 	.odf
   "application/vnd.oasis.opendocument.database",		// OpenOffice database 			.odb
   "application/vnd.oasis.opendocument.image",			// OpenOffice Imagen 			.odi
   "application/vnd.oasis.opendocument.text-master",		// OpenOffice Documento maestro 	.odm
   "application/vnd.oasis.opendocument.text-template",		// OpenOffice Text 			.ott
   "application/vnd.oasis.opendocument.spreadsheet-template",	// OpenOffice Hoja of cálculo 		.ots
   "application/vnd.oasis.opendocument.presentation-template",	// OpenOffice Presentación 		.otp
   "application/vnd.oasis.opendocument.graphics-template",	// OpenOffice Dibujo 			.otg
   "application/vnd.openxmlformats-officedocument.presentationml.presentation",	// Power Point Microsoft Office Open XML Format Presentation Slide Show .pptx
   "application/vnd.openxmlformats-officedocument.presentationml.slideshow",	// Power Point Microsoft Office Open XML Format Presentation Slide Show .ppsx
   "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",		// Excel Microsoft Office Open XML Format Spreadsheet .xlsx
   "application/vnd.openxmlformats-officedocument.wordprocessingml.document",	// Word Microsoft Office Open XML Format Document .docx
   "application/vnd.pdf",		// PDF
   "application/vnd.rar",		// RAR
   "application/vnd.wolfram.mathematica.package",		// Octave .m file
   "application/x-7z-compressed",	// 7 zip compressed file
   "application/x-bz2",			// Bzip 2 UNIX Compressed File
   "application/x-bzip",		// Bzip 2 UNIX Compressed File
   "application/x-cdr",			// Corel Draw (CDR)
   "application/x-compress",		// GNU ZIP gz, gzip
   "application/x-compressed",		// GNU ZIP gz, gzip, Bzip 2 UNIX Compressed File
   "application/x-compressed-tar",	// TGZ
   "application/x-coreldraw",		// Corel Draw (CDR)
   "application/x-dos_ms_project",	// Microsoft Project mpp
   "application/x-download",		// RAR
   "application/x-file-download",	// PDF
   "application/x-forcedownload",	// PDF
   "application/x-gtar",		// GNU tar gtar
   "application/x-gunzip",		// GNU ZIP gz, gzip
   "application/x-gzip",		// GNU ZIP gz, gzip
   "application/x-ipynb+json",		// Jupyter and IPython
   "application/x-latex",	   	// LateX latex (LateX)
   "application/x-midi",		// MIDI mid
   "application/x-msdos-program",	// MSDOS program
   "application/x-msdownload",		// dll, exe
   "application/x-mspowerpoint",	// Microsoft PowerPoint pot, pps, ppt
   "application/x-msproject",		// Microsoft Project mpp
   "application/x-ms-project",		// Microsoft Project mpp
   "application/x-msword",		// PDF?
   "application/x-mswrite",		// PDF
   "application/x-octet-stream",	// PDF
   "application/x-rar",			// .rar
   "application/x-shockwave-flash",		// Flash
   "application/x-shockwave-flash2-preview",	// Flash
   "application/x-unknown",		//  Unknown file type
   "application/x-vnd.oasis.opendocument.chart",		// OpenOffice Gráfica 			.odc
   "application/x-vnd.oasis.opendocument.database",		// OpenOffice database 			.odb
   "application/x-vnd.oasis.opendocument.formula",		// OpenOffice Fórmula matemática 	.odf
   "application/x-vnd.oasis.opendocument.graphics",		// OpenOffice Dibujo 			.odg
   "application/x-vnd.oasis.opendocument.graphics-template",	// OpenOffice Dibujo 			.otg
   "application/x-vnd.oasis.opendocument.image",		// OpenOffice Imagen 			.odi
   "application/x-vnd.oasis.opendocument.presentation",		// OpenOffice Presentación 		.odp
   "application/x-vnd.oasis.opendocument.presentation-template",// OpenOffice Presentación 		.otp
   "application/x-vnd.oasis.opendocument.spreadsheet",		// OpenOffice Hoja of cálculo 		.ods
   "application/x-vnd.oasis.opendocument.spreadsheet-template",	// OpenOffice Hoja of cálculo 		.ots
   "application/x-vnd.oasis.opendocument.text",			// OpenOffice Text 			.odt
   "application/x-vnd.oasis.opendocument.text-master",		// OpenOffice Documento maestro 	.odm
   "application/x-vnd.oasis.opendocument.text-template",	// OpenOffice Text			.ott
   "application/x-pdf",			// PDF
   "application/x-shockwave-flash",	// Macromedia Shockwave swf
   "application/x-spss",		// SPSS File sav spp sbs sps spo
   "application/x-rar-compressed",	// RAR archive rar
   "application/x-tar",			// 4.3BSD tar format tar
   "application/x-tex",			// TeX tex (LateX)
   "application/x-tgz",			// TGZ
   "application/x-troff",		// .s assembler source file
   "application/x-zip",			// ZIP archive zip
   "application/x-zip-compressed",	// ZIP archive zip
   "application/zip",			// ZIP archive zip
   "audio/basic",			// BASIC audio (u-law) au, snd
   "audio/mp4",				// MPEG-4
   "audio/mpeg",			// MP3
   "audio/midi",			// MIDI mid, midi
   "audio/x-aiff",			// AIFF audio aif, aifc, aiff
   "audio/x-mpeg",			// MPEG audio mp3
   "audio/x-ms-wma",			// WMA (Windows Media Audio File)
   "audio/x-pn-realaudio",		// RealAudio ra, ram
   "audio/x-pn-realaudio-plugin",	// RealAudio plug-in rpm
   "audio/x-voice",			// Voice voc
   "audio/x-wav",			// Microsoft Windows WAVE audio wav
   "binary/octet-stream",		// uninterpreted binary bin
   "document/unknown",			// Some bowsers send this (?)
   "file/unknown",			// Some bowsers send this (?)
   "gzip/document",			// GNU ZIP gz, gzip
   "image/bmp",				// Bitmap bmp
   "image/cdr",				// Corel Draw (CDR)
   "image/gif",				// GIF image gif
   "image/jpeg",			// JPEG image jpe, jpeg, jpg
   "image/pdf",				// PDF
   "image/pjpeg",			// JPEG image jpe, jpeg, jpg
   "image/pict",			// Macintosh PICT pict
   "image/png",				// Portable Network Graphic png
   "image/tiff",			// TIFF image tif, tiff
   "image/vnd.rn-realflash",		// Flash
   "image/x-cdr",			// Corel Draw (CDR)
   "image/x-cmu-raster",		// CMU raster ras
   "image/x-eps",			// Imagen postcript
   "image/x-png",			// Portable Network Graphic png
   "image/x-portable-anymap",		// PBM Anymap format pnm
   "image/x-portable-bitmap",		// PBM Bitmap format pbm
   "image/x-portable-graymap",		// PBM Graymap format pgm
   "image/x-portable-pixmap",		// PBM Pixmap format ppm
   "image/x-rgb",			// RGB image rgb
   "image/x-xbitmap",			// X Bitmap xbm
   "image/x-xpixmap",			// X Pixmap xpm
   "image/x-xwindowdump",		// X Window System dump xwd
   "message/rfc822",			// Files .mht and .mhtml
   "mime/pdf",				// Adobe Acrobat pdf
   "multipart/x-gzip",			// GNU ZIP archive gzip
   "multipart/x-zip",			// PKZIP archive zip
   "octet/pdf",				// PDF
   "text/anytext",			// CSV, Comma Separated Values?
   "text/comma-separated-values",	// CSV, Comma Separated Values
   "text/css",				// Hypertext Cascading Style Sheet
   "text/csv",				// CSV, Comma Separated Values
   "text/html",				// HTML htm, html, php
   "text/pdf",				// PDF
   "text/plain",			// plain text C, cc, h, txt. BAS
   "text/richtext",			// RTF
   "text/xml",				//
   "text/x-c",				// Source code in C
   "text/x-chdr",			// Source code in C
   "text/x-csrc",			// Source code in C
   "text/x-c++src",			// Source code in C++
   "text/x-latex",			// LateX
   "text/x-objcsrc",			// Source code
   "text/x-pdf",			// PDF
   "video/3gpp",			// Video Android mobile
   "video/avi",				// AVI
   "video/mp4",				// MPEG-4
   "video/mpeg",			// MPEG video mpe, mpeg, mpg
   "video/msvideo",			// Microsoft Windows video avi
   "video/quicktime",			// QuickTime video mov, qt
   "video/unknown",			// ?
   "video/x-ms-asf",			// WMA (Windows Media Audio File)
   "video/x-ms-wmv",			// WMV (Windows Media File)
   "video/x-msvideo",			// AVI
   "x-world/x-vrml",			// VRML Worlds wrl
   "x-java",				// Source code in Java
   "zz-application/zz-winassoc-cdr",	// Corel Draw (CDR)
   "zz-application/zz-winassoc-mpp"	// Microsoft Project mpp
  };

/*****************************************************************************/
/*********************** Check if MIME type is allowed ***********************/
/*****************************************************************************/

bool MIM_CheckIfMIMETypeIsAllowed (const char *MIMEType)
  {
   unsigned Type;

   /* Check type MIME */
   for (Type = 0;
	Type < MIM_NUM_MIME_TYPES_ALLOWED;
	Type++)
      if (!strcmp (MIMEType,MIM_MIMETypesAllowed[Type]))
	 return true;

   return false;
  }
