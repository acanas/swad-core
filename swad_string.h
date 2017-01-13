// swad_string.h: string processing

#ifndef _SWAD_STR
#define _SWAD_STR
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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
/********************************* Headers **********************************/
/*****************************************************************************/

#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For FILE

/*****************************************************************************/
/***************************** Public constants *****************************/
/*****************************************************************************/

#define Str_LENGTH_STR_HEX		3 // Length of the string #XX
#define Str_MAX_LENGTH_SPEC_CHAR_HTML	6 // Length of the string &#DDD;
#define Str_MAX_CHARACTER		6 // max (Str_LENGTH_STR_HEX,Str_MAX_LENGTH_SPEC_CHAR_HTML)

/*****************************************************************************/
/******************************* Public types *******************************/
/*****************************************************************************/

typedef enum
  {
   Str_NO_SKIP_HTML_COMMENTS,
   Str_SKIP_HTML_COMMENTS,
  } Str_SkipHTMLComments_t;
typedef enum
  {
   Str_FROM_FORM,
   Str_FROM_TEXT,
   Str_FROM_HTML,
   } Str_ChangeFrom_t;
typedef enum
  {
   Str_DONT_CHANGE,
   Str_TO_RIGOROUS_HTML,
   Str_TO_HTML,
   Str_TO_TEXT,
   Str_TO_MARKDOWN,
   } Str_ChangeTo_t;

/*****************************************************************************/
/***************************** Public prototypes ****************************/
/*****************************************************************************/

void Str_InsertLinks (char *Txt,unsigned long MaxLength,size_t MaxCharsURLOnScreen);
size_t Str_LimitLengthHTMLStr (char *Str,size_t MaxCharsOnScreen);
// bool Str_URLLooksValid (const char *URL);
void Str_ConvertToTitleType (char *Str);
void Str_ConvertToComparable (char *Str);
char *Str_ConvertToUpperText (char *Str);
char *Str_ConvertToLowerText (char *Str);
char Str_ConvertToUpperLetter (char Ch);
char Str_ConvertToLowerLetter (char Ch);

void Str_WriteFloatNum (FILE *FileDst,float Number);
void Str_ConvertStrFloatCommaToStrFloatPoint (char *Str);
float Str_GetFloatNumFromStr (const char *Str);
void Str_SetDecimalPointToUS (void);
void Str_SetDecimalPointToLocal (void);

void Str_AddStrToQuery (char *Query,const char *Str,size_t SizeOfQuery);
void Str_ChangeFormat (Str_ChangeFrom_t ChangeFrom,Str_ChangeTo_t ChangeTo,
                       char *Str,size_t MaxLengthStr,bool RemoveLeadingAndTrailingSpaces);
void Str_RemoveLeadingSpacesHTML (char *Str);
void Str_RemoveTrailingSpacesHTML (char *Str);
void Str_RemoveLeadingZeros (char *Str);
void Str_RemoveLeadingArrobas (char *Str);
bool Str_FindStrInFile (FILE *FileSrc,const char *Str,Str_SkipHTMLComments_t SkipHTMLComments);
bool Str_FindStrInFileBack (FILE *FileSrc,const char *Str,Str_SkipHTMLComments_t SkipHTMLComments);
bool Str_WriteUntilStrFoundInFileIncludingStr (FILE *FileTgt,FILE *FileSrc,const char *Str,Str_SkipHTMLComments_t SkipHTMLComments);
char *Str_GetCellFromHTMLTableSkipComments (FILE *FileSrc,char *Str,int MaxLength);
char *Str_GetNextStrFromFileConvertingToLower (FILE *FileSrc,char *Str, int N);
void Str_GetNextStringUntilSpace (const char **StrSrc,char *StrDst,size_t MaxLength);
void Str_GetNextStringUntilSeparator (const char **StrSrc,char *StrDst,size_t MaxLength);
void Str_GetNextStringFromFileUntilSeparator (FILE *FileSrc,char *StrDst);
void Str_GetNextStringUntilComma (const char **StrSrc,char *StrDst,size_t MaxLength);
void Str_ReplaceSeveralSpacesForOne (char *Str);
void Str_CopyStrChangingSpaces (const char *StringWithSpaces,char *StringWithoutSpaces,unsigned MaxLength);
long Str_ConvertStrCodToLongCod (const char *Str);
size_t Str_GetLengthRootFileName (const char *FileName);
void Str_SplitFullPathIntoPathAndFileName (const char *FullPath,
                                           char *PathWithoutFileName,char *FileName);
bool Str_FileIs (const char *FileName,const char *Extension);
bool Str_FileIsHTML (const char *FileName);
bool Str_Path1BeginsByPath2 (const char *Path1,const char *Path2);
void Str_SkipSpacesInFile (FILE *FileSrc);
void Str_FilePrintStrChangingBRToRetAndNBSPToSpace (FILE *FileTgt,const char *Str);
int Str_ReadFileUntilBoundaryStr (FILE *FileSrc,char *StrDst,
                                  const char *BoundaryStr,
                                  unsigned LengthBoundaryStr,
                                  unsigned long long MaxLength);
bool Str_ConvertFilFolLnkNameToValid (char *FileName);
void Str_ConvertToValidFileName (char *Str);

void Str_CreateRandomAlphanumStr (char *Str,size_t Length);
void Str_GetMailBox (const char *Email,char *MailBox,size_t MaxLength);

#endif
