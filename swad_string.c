// swad_string.c: string processing

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <ctype.h>		// For isprint, isspace, etc.
#include <locale.h>		// For setlocale
#include <math.h>		// For log10, floor, ceil, modf, sqrt...
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_alert.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_notification_database.h"
#include "swad_parameter.h"
#include "swad_string.h"

/*****************************************************************************/
/******************** Global variables from other modules ********************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*************************** Private prototypes ******************************/
/*****************************************************************************/

static unsigned Str_FindHTMLEntity (const char *Ptr);

static int Str_ReadCharAndSkipComments (FILE *FileSrc,Str_SkipHTMLComments_t SkipHTMLComments);
static int Str_ReadCharAndSkipCommentsWriting (FILE *FileSrc,FILE *FileTgt,Str_SkipHTMLComments_t SkipHTMLComments);
static int Str_ReadCharAndSkipCommentsBackward (FILE *FileSrc,Str_SkipHTMLComments_t SkipHTMLComments);

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/***** Conversion to Base64URL *****/
// base64url is described in document http://tools.ietf.org/html/rfc4648.
// It uses '-' and '_' because they are safe for URL/parameters (without enconding) and for filenames.
const char Str_BIN_TO_BASE64URL[64 + 1] =
  {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
   'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
   '0','1','2','3','4','5','6','7','8','9','-','_',
   '\0'};	// NULL-terminated string

static char Str_LF[2] = {10,0};
static char Str_CR[2] = {13,0};

/*****************************************************************************/
/*********** Check if a character is in set { a-z, A-Z, 0-9, _ } *************/
/*****************************************************************************/

bool Str_ChIsAlphaNum (char Ch)
  {
   return ((Ch >= 'a' && Ch <= 'z') ||
	   (Ch >= 'A' && Ch <= 'Z') ||
	   (Ch >= '0' && Ch <= '9') ||
	   (Ch == '_'));
  }

/*****************************************************************************/
/** Get next ASCII character from a string converting &#number; to character */
/*****************************************************************************/
// Returns number of char analyzed

unsigned Str_GetNextASCIICharFromStr (const char *Ptr,unsigned char *Ch)
  {
   unsigned NumChars;
   unsigned Num;

   if (*Ptr == '\0')
     {
      *Ch = '\0';
      return 0;
     }
   else if (*Ptr == '&')	// It's a &#num; character?
     {
      Ptr++;
      if (*Ptr == '#')
        {
         Ptr++;
         for (NumChars = 2, Num = 0;
              *Ptr >= '0' && *Ptr <= '9';
              Ptr++, NumChars++)
            if (Num < 100000)	// To avoid overflow
               Num = Num * 10 + (unsigned) (*Ptr - '0');
         if (*Ptr == ';')	// &#num; found
           {
            NumChars++;
            switch (Num)
              {
               case  32: *Ch = ' ';  return NumChars;
               case  33: *Ch = '!';  return NumChars;
               case  34: *Ch = '\"'; return NumChars;
               case  35: *Ch = '#';  return NumChars;
               case  36: *Ch = '$';  return NumChars;
               case  37: *Ch = '%';  return NumChars;
               case  38: *Ch = '&';  return NumChars;
               case  39: *Ch = '\''; return NumChars;
               case  40: *Ch = '(';  return NumChars;
               case  41: *Ch = ')';  return NumChars;
               case  42: *Ch = '*';  return NumChars;
               case  43: *Ch = '+';  return NumChars;
               case  44: *Ch = ',';  return NumChars;
               case  45: *Ch = '-';  return NumChars;
               case  46: *Ch = '.';  return NumChars;
               case  47: *Ch = '/';  return NumChars;
               case  58: *Ch = ':';  return NumChars;
               case  59: *Ch = ';';  return NumChars;
               case  60: *Ch = ' ';  return NumChars;	// '<'
               case  61: *Ch = '=';  return NumChars;
               case  62: *Ch = ' ';  return NumChars;	// '>'
               case  63: *Ch = '?';  return NumChars;
               case  64: *Ch = '@';  return NumChars;
               case  91: *Ch = '[';  return NumChars;
               case  92: *Ch = '\\'; return NumChars;
               case  93: *Ch = ']';  return NumChars;
               case  94: *Ch = '^';  return NumChars;
               case  95: *Ch = '_';  return NumChars;
               case  96: *Ch = '`';  return NumChars;
               case 123: *Ch = '{';  return NumChars;
               case 124: *Ch = '|';  return NumChars;
               case 125: *Ch = '}';  return NumChars;
               case 126: *Ch = '~';  return NumChars;
               default:  *Ch = '?';  return NumChars;	// Unknown character
              }
           }
         else
           {
            *Ch = (unsigned char) *Ptr;
            return NumChars;
           }
        }
      else
        {
         *Ch = (unsigned char) *Ptr;
         return 2;
        }
     }
   else
     {
      *Ch = (unsigned char) *Ptr;
      return 1;
     }
  }

/*****************************************************************************/
/*************** Limit number of chars on screen of a string *****************/
/*****************************************************************************/
// Returns length of resulting string in bytes (a special char counts for several bytes)

size_t Str_LimitLengthHTMLStr (char *Str,size_t MaxCharsOnScreen)
  {
   char *Ptr;
   size_t NumCharsOnScreen;
   size_t Length;
   size_t LengthHTMLEntity;

   if (MaxCharsOnScreen < 3)
      MaxCharsOnScreen = 3;	// Length of "..."

   /***** Calculate length counting "&...;" as one character *****/
   for (Ptr = Str, NumCharsOnScreen = 0, Length = 0;
	*Ptr;
	Ptr++, NumCharsOnScreen++, Length++)
      /* Check if an HTML entity is present */
      if (*Ptr == '&')	// Possible HTML entity
	 if ((LengthHTMLEntity = Str_FindHTMLEntity (Ptr)))
	   {
	    /* if Ptr points to &ntilde; ==> Length = 8 */
	    Ptr    += LengthHTMLEntity - 1;	// Now Ptr point to ';'
	    Length += LengthHTMLEntity - 1;
	   }

   if (NumCharsOnScreen <= MaxCharsOnScreen)	// Don't limit string
      return Length;

   /***** Limit length of string *****/
   for (Ptr = Str, NumCharsOnScreen = 0, Length = 0;
	*Ptr;
	Ptr++, NumCharsOnScreen++, Length++)
     {
      if (NumCharsOnScreen == MaxCharsOnScreen - 3)
	{
         *Ptr++ = '.';
         *Ptr++ = '.';
         *Ptr++ = '.';
	 *Ptr   = '\0';		// ...limit length...
	 Length += 3;
         break;
	}
      /* Check if an HTML entity or directive is present */
      if (*Ptr == '&')	// Possible HTML entity
	{
	 if ((LengthHTMLEntity = Str_FindHTMLEntity (Ptr)))
	   {
	    /* if Ptr points to &ntilde; ==> Length = 8 */
	    Ptr += LengthHTMLEntity - 1;	// Now Ptr point to ';'
	    Length += LengthHTMLEntity - 1;
	   }
	}
      else if (*Ptr == '<')	// HTML directive "<...>"
	 for (Ptr++, Length++;
	      *Ptr && *Ptr != '>';
	      Ptr++, Length++);	// While not end of HTML directive "<...>"
     }

   return Length;
  }

/*****************************************************************************/
/******** Return the length of a possible HTML entity inside a string ********/
/*****************************************************************************/
// For example, if Ptr points to "&ntilde;..." or "&#38754;...", return 8
// If Ptr points to no HTML entity, return 0

static unsigned Str_FindHTMLEntity (const char *Ptr)
  {
   size_t Length = 0;
   char Ch;

   /***** The first character must be '&' *****/
   if (Ptr[Length] != '&')
      return 0;	// No HTML entity found

   /***** The second character can be '#' *****/
   Length++;
   if (Ptr[Length] == '#')
      /* Go to third character */
      Length++;

   /***** Now one or more alphanumeric characters are expected *****/
   /* Check second/third character */
   Ch = Ptr[Length];
   if (!((Ch >= '0' && Ch <= '9') ||
	 (Ch >= 'a' && Ch <= 'z') ||
	 (Ch >= 'A' && Ch <= 'Z')))
      return 0;	// No HTML entity found

   /* Go to first non alphanumeric character */
   do
     {
      Length++;
      Ch = Ptr[Length];
     }
   while ((Ch >= '0' && Ch <= '9') ||
	  (Ch >= 'a' && Ch <= 'z') ||
	  (Ch >= 'A' && Ch <= 'Z'));

   /***** An HTML entity must end by ';' *****/
   return (Ptr[Length] == ';') ? Length + 1 :	// HTML entity found (return Length including the final ';')
	                         0;		// No HTML entity found
  }

/*****************************************************************************/
/*** Create a notification about mention for any nickname in a publication ***/
/*****************************************************************************/
/*
 Example: "The user @rms says..."
                     ^ ^
         PtrStart ___| |___ PtrEnd
                 Length = 3
*/
void Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (long PubCod,const char *Txt)
  {
   const char *Ptr;
   bool IsNickname;
   struct
     {
      const char *PtrStart;
      const char *PtrEnd;
      size_t Length;		// Length of the nickname
     } Nickname;
   struct Usr_Data UsrDat;
   bool CreateNotif;
   bool NotifyByEmail;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Find nicknames and create notifications *****/
   for (Ptr = Txt;
	*Ptr;)
      /* Check if the next char is the start of a nickname */
      if ((int) *Ptr == (int) '@')
	{
	 /* Find nickname end */
	 Ptr++;	// Points to first character after @
         Nickname.PtrStart = Ptr;

	 /* A nick can have digits, letters and '_'  */
	 for (;
	      *Ptr;
	      Ptr++)
	    if (!((*Ptr >= 'a' && *Ptr <= 'z') ||
		  (*Ptr >= 'A' && *Ptr <= 'Z') ||
		  (*Ptr >= '0' && *Ptr <= '9') ||
		  (*Ptr == '_')))
	       break;

	 /* Calculate length of this nickname */
	 Nickname.PtrEnd = Ptr - 1;
         Nickname.Length = (size_t) (Ptr - Nickname.PtrStart);

	 /* A nick (without arroba) must have a number of characters
            Nck_MIN_CHARS_NICK_WITHOUT_ARROBA <= Length <= Nck_MAX_CHARS_NICK_WITHOUT_ARROBA */
	 IsNickname = (Nickname.Length >= Nck_MIN_CHARS_NICK_WITHOUT_ARROBA &&
	               Nickname.Length <= Nck_MAX_CHARS_NICK_WITHOUT_ARROBA);

	 if (IsNickname)
	   {
	    /* Copy nickname */
	    strncpy (UsrDat.Nickname,Nickname.PtrStart,Nickname.Length);
	    UsrDat.Nickname[Nickname.Length] = '\0';

	    if ((UsrDat.UsrCod = Nck_GetUsrCodFromNickname (UsrDat.Nickname)) > 0)
	       if (Usr_ItsMe (UsrDat.UsrCod) == Usr_OTHER)	// Not me
		 {
		  /* Get user's data */
		  Usr_GetAllUsrDataFromUsrCod (&UsrDat,
		                               Usr_DONT_GET_PREFS,
		                               Usr_DONT_GET_ROLE_IN_CRS);

		  /* Create notification for the mentioned user *****/
		  CreateNotif = (UsrDat.NtfEvents.CreateNotif & (1 << Ntf_EVENT_TML_MENTION));
		  if (CreateNotif)
		    {
		     NotifyByEmail = (UsrDat.NtfEvents.SendEmail & (1 << Ntf_EVENT_TML_MENTION));
		     Ntf_DB_StoreNotifyEventToUsr (Ntf_EVENT_TML_MENTION,UsrDat.UsrCod,PubCod,
						   (Ntf_Status_t) (NotifyByEmail ? Ntf_STATUS_BIT_EMAIL :
										   0),
						   Gbl.Hierarchy.Node[Hie_INS].HieCod,
						   Gbl.Hierarchy.Node[Hie_CTR].HieCod,
						   Gbl.Hierarchy.Node[Hie_DEG].HieCod,
						   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
		    }
		 }
	   }
	}
      /* The next char is not the start of a nickname */
      else	// Character != '@'
         Ptr++;

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/***** Convert a string to title: first uppercase and the rest lowercase *****/
/*****************************************************************************/

void Str_ConvertToTitleType (char *Str)
  {
   char Ch;
   char *Ptr;
   char *Ptr2;
   unsigned LengthStr;
   bool FirstLetter = true;

   for (Ptr = Str;
	*Ptr;
	Ptr++)
     {
      Ch = *Ptr;
      if (isspace ((int) Ch) ||
	  Ch == '\xA0' ||	// Unicode translation for &nbsp;
	  Ch ==    '-' ||
	  Ch ==    '(' ||
	  Ch ==    ')' ||
	  Ch ==    ',' ||
       // Ch ==    ';' ||	// Do not start a new word on ';'.
				// Example: akto&#287;an should be converted to Akto&#287;an
	  Ch ==    '.' ||
	  Ch ==    ':' ||
	  Ch ==    'ª' ||
	  Ch ==    'º')
	 FirstLetter = true;
      else
	{
	 if (FirstLetter)
	   {
	    /* Check if it's "de", "del", "la", "y" */	// This should be internationalized!!!!!
	    for (Ptr2 = Ptr, LengthStr = 0;
		 *Ptr2;
		 Ptr2++, LengthStr++)
	       if (isspace ((int) *Ptr2) ||
		   *Ptr2 == '\xA0')	// Unicode translation for &nbsp;
		  break;
	    if (LengthStr == 1)
	      {
	       if (!strncasecmp (Ptr,"y",1))
		  FirstLetter = false;
	      }
	    else if (LengthStr == 2)
	      {
	       if (!strncasecmp (Ptr,"de",2))
		  FirstLetter = false;
	       else if (!strncasecmp (Ptr,"la",2))
		  FirstLetter = false;
	      }
	    else if (LengthStr == 3)
	       if (!strncasecmp (Ptr,"del",3))
		  FirstLetter = false;
	   }
	 if (FirstLetter)
	   {
	    *Ptr = Str_ConvertToUpperLetter (*Ptr);
	    FirstLetter = false;
	   }
	 else
	    *Ptr = Str_ConvertToLowerLetter (*Ptr);
	}
     }
  }

/*****************************************************************************/
/*** Changes a string to made it comparable with others (removing tildes) ****/
/*****************************************************************************/

void Str_ConvertToComparable (char *Str)
  {
   char *Ptr;

   for (Ptr = Str;
	*Ptr;
	Ptr++)
     {
      *Ptr = Str_ConvertToLowerLetter (*Ptr);
      switch (*Ptr)
	{
	 case 'á': *Ptr = 'a'; break;
	 case 'é': *Ptr = 'e'; break;
	 case 'í': *Ptr = 'i'; break;
	 case 'ó': *Ptr = 'o'; break;
	 case 'ú': *Ptr = 'u'; break;
	 case 'ä': *Ptr = 'a'; break;
	 case 'ë': *Ptr = 'e'; break;
	 case 'ï': *Ptr = 'i'; break;
	 case 'ö': *Ptr = 'o'; break;
	 case 'ü': *Ptr = 'u'; break;
	}
     }
  }

/*****************************************************************************/
/********************** Convert a string to uppercase ************************/
/*****************************************************************************/

char *Str_ConvertToUpperText (char *Str)
  {
   char *Ptr;

   for (Ptr = Str;
	*Ptr;
	Ptr++)
      *Ptr = Str_ConvertToUpperLetter (*Ptr);

   return Str;
  }

/*****************************************************************************/
/********************** Convert a string to lowercase ************************/
/*****************************************************************************/

char *Str_ConvertToLowerText (char *Str)
  {
   char *Ptr;

   for (Ptr = Str;
	*Ptr;
	Ptr++)
      *Ptr = Str_ConvertToLowerLetter (*Ptr);

   return Str;
  }

/*****************************************************************************/
/*********************** Convert a character to uppercase ********************/
/*****************************************************************************/

char Str_ConvertToUpperLetter (char Ch)
  {
   switch (Ch)
     {
      case 'á': return 'Á';
      case 'é': return 'É';
      case 'í': return 'Í';
      case 'ó': return 'Ó';
      case 'ú': return 'Ú';
      case 'ñ': return 'Ñ';
      case 'ç': return 'Ç';
      case 'ä': return 'Ä';
      case 'ë': return 'Ë';
      case 'ï': return 'Ï';
      case 'ö': return 'Ö';
      case 'ü': return 'Ü';
      default: return (char) toupper ((int) Ch);
     }
  }

/*****************************************************************************/
/*********************** Convert a character to lowercase ********************/
/*****************************************************************************/

char Str_ConvertToLowerLetter (char Ch)
  {
   switch (Ch)
     {
      case 'Á': return 'á';
      case 'É': return 'é';
      case 'Í': return 'í';
      case 'Ó': return 'ó';
      case 'Ú': return 'ú';
      case 'Ñ': return 'ñ';
      case 'Ç': return 'ç';
      case 'Ä': return 'ä';
      case 'Ë': return 'ë';
      case 'Ï': return 'ï';
      case 'Ö': return 'ö';
      case 'Ü': return 'ü';
      default: return (char) tolower ((int) Ch);
     }
  }

/*****************************************************************************/
/****************** Write a number in floating point *************************/
/*****************************************************************************/
// Str should be freed after calling this function

void Str_DoubleNumToStr (char **Str,double Number)
  {
   if (asprintf (Str,"%.15lg",
		 Number) < 0)
      Err_NotEnoughMemoryExit ();
  }

/*****************************************************************************/
/******* Write a number in floating point with few digits to a string ********/
/*****************************************************************************/
// Str should be freed after calling this function

void Str_DoubleNumToStrFewDigits (char **Str,double Number)
  {
   double IntegerPart;
   double FractionaryPart;
   char *Format;

   FractionaryPart = fabs (modf (Number,&IntegerPart));

   if (FractionaryPart == 0.0)
     {
      if (asprintf (Str,"%.0f",IntegerPart) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else
     {
      if (IntegerPart != 0.0)
         Format = "%.1lf";
      else if (FractionaryPart >= 0.095)
         Format = "%.2lf";
      else if (FractionaryPart >= 0.0095)
         Format = "%.3lf";
      else if (FractionaryPart >= 0.00095)
         Format = "%.4lf";
      else if (FractionaryPart >= 0.000095)
         Format = "%.5lf";
      else if (FractionaryPart >= 0.0000095)
         Format = "%.6lf";
      else
         Format = "%le";
      if (asprintf (Str,Format,Number) < 0)
	 Err_NotEnoughMemoryExit ();
     }
  }

/*****************************************************************************/
/********** Convert a string that holds a number in floating comma ***********/
/********** to another in floating point changing commas to points ***********/
/*****************************************************************************/

void Str_ConvertStrFloatCommaToStrFloatPoint (char *Str)
  {
   for (;
	*Str;
	Str++)
      if (*Str == ',')
        *Str = '.';
  }

/*****************************************************************************/
/************************ Get a double from a string *************************/
/*****************************************************************************/
// This function may change Str on wrong double

double Str_GetDoubleFromStr (const char *Str)
  {
   char *StrPoint;
   double DoubleNum;

   /***** Trivial check *****/
   if (Str == NULL)	// If no string...
      return 0.0;	// ...the number is reset to 0

   /***** Copy source string to temporary string to convert to point *****/
   if (asprintf (&StrPoint,"%s",Str) < 0)
      Err_NotEnoughMemoryExit ();

   /***** The string is "scanned" in floating point
          (it must have a point, not a comma as decimal separator) *****/
   Str_ConvertStrFloatCommaToStrFloatPoint (StrPoint);
   Str_SetDecimalPointToUS ();		// To get the decimal point as a dot
   if (sscanf (StrPoint,"%lf",&DoubleNum) != 1)	// If the string does not hold a valid number...
      DoubleNum = 0.0;				// ...the number is reset to 0
   Str_SetDecimalPointToLocal ();	// Return to local system

   /***** Free temporary string *****/
   free (StrPoint);

   return DoubleNum;
  }

/*****************************************************************************/
/**** Change decimal point to US system in order to get/print it as a dot ****/
/*****************************************************************************/

void Str_SetDecimalPointToUS (void)
  {
   if (!setlocale (LC_NUMERIC,"en_US.utf8"))	// To get/print the floating point as a dot
      if (Gbl.Layout.HTMLStartWritten)
         Ale_ShowAlert (Ale_ERROR,"Can not set locale to en_US.");
  }

/*****************************************************************************/
/****************** Change decimal point to local system *********************/
/*****************************************************************************/

void Str_SetDecimalPointToLocal (void)
  {
   // TODO: this should be internationalized!!!!!!!
   if (!setlocale (LC_NUMERIC,"es_ES.utf8"))	// Return to local system
      if (Gbl.Layout.HTMLStartWritten)
         Ale_ShowAlert (Ale_ERROR,"Can not set locale to es_ES.");
  }

/*****************************************************************************/
/*************** Add a string to a Query, changing ' for &#39; ***************/
/*****************************************************************************/

void Str_AddStrToQuery (char *Query,const char *Str,size_t SizeOfQuery)
  {
   size_t LengthOfQuery = strlen (Query);
   size_t SizeOfQuery_6 = SizeOfQuery - 6;
   size_t SizeOfQuery_2 = SizeOfQuery - 2;

   // Query can not have a ' character
   for (;
	*Str;
	Str++)
      if (*Str == '\'')	// It's a ' ...
        {
         if (LengthOfQuery > SizeOfQuery_6)
            break;
         Query[LengthOfQuery++] = '&';
         Query[LengthOfQuery++] = '#';
         Query[LengthOfQuery++] = '3';
         Query[LengthOfQuery++] = '9';
         Query[LengthOfQuery++] = ';';	// ...add the HTML special character &#39;
        }
      else		// It's another character...
        {
         if (LengthOfQuery > SizeOfQuery_2)
            break;
         Query[LengthOfQuery++] = *Str;	// ...add the character without changes
        }
   Query[LengthOfQuery] = '\0';
  }

/*****************************************************************************/
/******************** Change the format of a string **************************/
/*****************************************************************************/
/*
ChangeFrom can be:
Str_FROM_FORM
Str_FROM_TEXT
Str_FROM_HTML
ChangeTo can be:
Str_DONT_CHANGE
Str_TO_TEXT
Str_TO_MARKDOWN
Str_TO_HTML
Str_TO_RIGOROUS_HTML
For example the string "Nueva++de+San+Ant%F3n"
      is converted to:
      "Nueva &nbsp;de San Antón"		if ChangeTo == Str_TO_RIGOROUS_HTML
      "Nueva  de San Antón"			if ChangeTo == Str_TO_HTML
      "Nueva  de San Antón"			if ChangeTo == Str_TO_TEXT
*/
#define Str_MAX_BYTES_SPECIAL_CHAR (256 - 1)

void Str_ChangeFormat (Str_ChangeFrom_t ChangeFrom,Str_ChangeTo_t ChangeTo,
                       char *Str,size_t MaxLengthStr,
                       Str_RemoveSpaces_t RemoveLeadingAndTrailingSpaces)
  {
   char *StrDst;
   char *PtrSrc;
   char *PtrDst;
   unsigned char Ch;
   unsigned int SpecialChar;
   size_t LengthSpecStrSrc = 0;
   size_t LengthSpecStrDst;
   unsigned long LengthStrDst = 0;
   unsigned NumSpacesTab;
   unsigned i;
   unsigned NumPrintableCharsFromReturn = 0;	// To substitute tabs for spaces
   bool IsSpecialChar = false;
   bool ThereIsSpaceChar = true;	// Indicates if the character before was a space. Set to true to respect the initial spaces.
   char StrSpecialChar[Str_MAX_BYTES_SPECIAL_CHAR + 1];

   if (ChangeTo != Str_DONT_CHANGE)
     {
      /***** Allocate memory for a destination string where to do the changes *****/
      if ((StrDst = malloc (MaxLengthStr + 1)) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Make the change *****/
      for (PtrSrc = Str, PtrDst = StrDst;
	   *PtrSrc;)
        {
         Ch = (unsigned char) *PtrSrc;
         switch (ChangeFrom)
           {
            case Str_FROM_FORM:
               switch (Par_GetContentReceivedByCGI ())
                 {
		  case Act_NORM:
		     // The form contained text and was sent with content type application/x-www-form-urlencoded
		     switch (Ch)
		       {
			case '+':	/* Change every '+' to a space */
			   IsSpecialChar = true;
			   LengthSpecStrSrc = 1;
			   SpecialChar = 0x20;
			   break;
			case '%':	/* Change "%XX" --> "&#decimal_number;" (from 0 to 255) */
				   /* Change "%uXXXX" --> "&#decimal number;  (from 0 to 65535) */
			   IsSpecialChar = true;
			   if (*(PtrSrc + 1) == 'u')
			     {
			      sscanf (PtrSrc + 2,"%4X",&SpecialChar);
			      LengthSpecStrSrc = 6;
			     }
			   else
			     {
			      sscanf (PtrSrc + 1,"%2X",&SpecialChar);
			      LengthSpecStrSrc = 3;
			     }
			   /* Some special characters, like a chinese character,
			      can be received from a form in a format like this:
			      %26%2335753%3B --> %26 %23 3 5 7 5 3 %3B --> &#35753;
						  ^   ^             ^
						  |   |             |
					 SpecialChar SpecialChar SpecialChar
			      Here one chinese character is converted
			      to 2 special chars + 5 normal chars + 1 special char,
			      and finally is stored as the following 8 bytes: &#35753;
			   */
			   break;
			case 0x27:	/* Change single comilla --> "&#39;" to avoid SQL code injection */
			case 0x5C:	/* '\\' */
			   IsSpecialChar = true;
			   LengthSpecStrSrc = 1;
			   SpecialChar = (unsigned int) Ch;
			   break;
			default:
			   IsSpecialChar = false;
			   NumPrintableCharsFromReturn++;
			   ThereIsSpaceChar = false;
			   break;
		       }
                     break;
		  case Act_DATA:
		     // The form contained data and was sent with content type multipart/form-data
		     switch (Ch)
		       {
			case 0x20:	/* Space */
			case 0x22:	/* Change double comilla --> "&#34;" */
			case 0x23:	/* '#' */
			case 0x26:	/* Change '&' --> "&#38;" */
			case 0x27:	/* Change single comilla --> "&#39;" to avoid SQL code injection */
			case 0x2C:	/* ',' */
			case 0x2F:	/* '/' */
			case 0x3A:	/* ':' */
			case 0x3B:	/* ';' */
			case 0x3C:	/* '<' --> "&#60;" */
			case 0x3E:	/* '>' --> "&#62;" */
			case 0x3F:	/* '?' */
			case 0x40:	/* '@' */
			case 0x5C:	/* '\\' */
			   IsSpecialChar = true;
			   LengthSpecStrSrc = 1;
			   SpecialChar = (unsigned int) Ch;
			   break;
			default:
			   if (Ch < 0x20 || Ch > 0x7F)
			     {
			      IsSpecialChar = true;
			      LengthSpecStrSrc = 1;
			      SpecialChar = (unsigned int) Ch;
			     }
			   else
			     {
			      IsSpecialChar = false;
			      NumPrintableCharsFromReturn++;
			      ThereIsSpaceChar = false;
			     }
			   break;
		       }
		     break;
                 }
               break;
            case Str_FROM_HTML:
            case Str_FROM_TEXT:
               switch (Ch)
                 {
                  case 0x20:	/* Space */
                  case 0x27:	/* Change single comilla --> "&#39;" to avoid SQL code injection */
                  case 0x5C:	/* '\\' */
                     IsSpecialChar = true;
                     LengthSpecStrSrc = 1;
                     SpecialChar = (unsigned int) Ch;
                     break;
                  default:
                     if (Ch < 0x20)
                       {
                        IsSpecialChar = true;
                        LengthSpecStrSrc = 1;
                        SpecialChar = (unsigned int) Ch;
                       }
                     else
                       {
                        IsSpecialChar = false;
                        NumPrintableCharsFromReturn++;
                        ThereIsSpaceChar = false;
                       }
                     break;
                 }
               break;
           }

         if (IsSpecialChar)
           {
            switch (SpecialChar)
              {
               case 0x09:  /* Tab */
                  if (ChangeTo == Str_TO_RIGOROUS_HTML)
                    {
                     /* Change tab to spaces (so many spaces as left until the next multiple of 8) */
                     StrSpecialChar[0] = '\0';
                     NumSpacesTab = 8 - NumPrintableCharsFromReturn % 8;
                     // Insert a space to separate former string from &nbsp;
                     // This space is not printed on screen in HTML,
                     // but it is necessary to mark the end of a possible previous URL
                     Str_Concat (StrSpecialChar,
                                 ThereIsSpaceChar ? "&nbsp;" :
                        	                    " ",	// The first space
                                 sizeof (StrSpecialChar) - 1);
                     for (i = 1;
                	  i < NumSpacesTab;
                	  i++)					// Rest of spaces, except the first
                        Str_Concat (StrSpecialChar,"&nbsp;",	// Add a space
                                    sizeof (StrSpecialChar) - 1);
                     NumPrintableCharsFromReturn += NumSpacesTab;
                    }
                  else
                    {
                     StrSpecialChar[0] = '\t';
                     StrSpecialChar[1] = '\0';
                    }
                  ThereIsSpaceChar = true;
                  break;
               case 0x0A:  /* \n */
        	  if (ChangeTo == Str_TO_RIGOROUS_HTML)
                     Str_Copy (StrSpecialChar,"<br>",
                	       sizeof (StrSpecialChar) - 1);
        	  else
                    {
                     StrSpecialChar[0] = Str_LF[0];
                     StrSpecialChar[1] = '\0';
                    }
                  NumPrintableCharsFromReturn = 0;
                  ThereIsSpaceChar = true;
                  break;
               case 0x0D:  /* "%0D" --> "" */
        	  if (ChangeTo == Str_TO_RIGOROUS_HTML)
                     StrSpecialChar[0] = '\0';
        	  else
                    {
                     StrSpecialChar[0] = Str_CR[0];
                     StrSpecialChar[1] = '\0';
                    }
                  NumPrintableCharsFromReturn = 0;
                  ThereIsSpaceChar = true;
                  break;
               case 0x20:  /* Space */
        	  if (ChangeTo == Str_TO_RIGOROUS_HTML && ThereIsSpaceChar)
                     Str_Copy (StrSpecialChar,"&nbsp;",
                               sizeof (StrSpecialChar) - 1);
        	  else
                    {
                     StrSpecialChar[0] = ' ';
                     StrSpecialChar[1] = '\0';
                    }
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = true;
                  break;
               case 0x22:  /* "%22" --> "&#34;" (double comilla) */
        	  if (ChangeTo == Str_TO_MARKDOWN)
        	    {	// Escape sequence for database, two characters
        	     StrSpecialChar[0] = '\\';	// 1. An inverted bar
        	     StrSpecialChar[1] = '\"';	// 2. A double comilla
        	     StrSpecialChar[2] = '\0';	// End of string
        	    }
        	  else
                     Str_Copy (StrSpecialChar,"&#34;",	// Double comilla is stored as HTML code to avoid problems when displaying it
                	       sizeof (StrSpecialChar) - 1);
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x23:  /* "%23" --> "#" */
		  StrSpecialChar[0] = '#';	// '#' must be converted to '#' to allow HTML entities like &#20998;
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x26:  /* "%26" --> "&" */
		  StrSpecialChar[0] = '&';	// '&' must be converted to '&' to allow HTML entities like &#20998;
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x27:  /* "%27" --> "&#39;" (single comilla) */
        	  if (ChangeTo == Str_TO_MARKDOWN)
        	    {	// Escape sequence for database, two characters
        	     StrSpecialChar[0] = '\\';	// 1. An inverted bar
        	     StrSpecialChar[1] = '\'';	// 2. A single comilla
        	     StrSpecialChar[2] = '\0';	// End of string
        	    }
        	  else
                     Str_Copy (StrSpecialChar,"&#39;",	// Single comilla is stored as HTML entity to avoid problem when querying database (SQL code injection)
                	       sizeof (StrSpecialChar) - 1);
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x2C:  /* "%2C" --> "," */
		  StrSpecialChar[0] = ',';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x2F:  /* "%2F" --> "/" */
		  StrSpecialChar[0] = '/';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x3A:  /* "%3A" --> ":" */
		  StrSpecialChar[0] = ':';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x3B:  /* "%3B" --> ";" */
		  StrSpecialChar[0] = ';';	// ';' must be converted to ';' to allow HTML entities like &#20998;
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x3C:  /* "%3C" --> "&#60;" (<) */
        	  if (ChangeTo == Str_TO_MARKDOWN)
        	    {
		     StrSpecialChar[0] = '<';
		     StrSpecialChar[1] = '\0';
        	    }
        	  else
                     Str_Copy (StrSpecialChar,"&#60;", // "<" is stored as HTML code to avoid problems when displaying it
                               sizeof (StrSpecialChar) - 1);
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x3E:  /* "%3E" --> "&#62;" (>) */
        	  if (ChangeTo == Str_TO_MARKDOWN)
        	    {
		     StrSpecialChar[0] = '>';
		     StrSpecialChar[1] = '\0';
        	    }
        	  else
        	     Str_Copy (StrSpecialChar,"&#62;", // ">" is stored as HTML code to avoid problems when displaying it
        	               sizeof (StrSpecialChar) - 1);
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x3F:  /* "%3F" --> "?" */
		  StrSpecialChar[0] = '?';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x40:  /* "%40" --> "@" */
		  StrSpecialChar[0] = '@';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x5C:  /* "%5C" --> "&#92;" (\) */
        	  if (ChangeTo == Str_TO_MARKDOWN)
        	    {	// Escape sequence for database, two characters
        	     StrSpecialChar[0] = '\\';	// 1. An inverted bar
        	     StrSpecialChar[1] = '\\';	// 2. An inverted bar
        	     StrSpecialChar[2] = '\0';	// End of string
        	    }
        	  else
                     Str_Copy (StrSpecialChar,"&#92;", // "\" is stored as HTML code to avoid problems when displaying it
                               sizeof (StrSpecialChar) - 1);
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xC1:  /* "%C1" --> "Á" */
		  StrSpecialChar[0] = 'Á';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xC9:  /* "%C9" --> "É" */
		  StrSpecialChar[0] = 'É';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xCD:  /* "%CD" --> "Í" */
		  StrSpecialChar[0] = 'Í';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xD3:  /* "%D3" --> "Ó" */
		  StrSpecialChar[0] = 'Ó';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xDA:  /* "%DA" --> "Ú" */
		  StrSpecialChar[0] = 'Ú';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xD1:  /* "%D1" --> "Ñ" */
		  StrSpecialChar[0] = 'Ñ';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xC7:  /* "%C7" --> "Ç" */
		  StrSpecialChar[0] = 'Ç';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xE1:  /* "%E1" --> "á" */
		  StrSpecialChar[0] = 'á';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xE9:  /* "%E9" --> "é" */
		  StrSpecialChar[0] = 'é';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xED:  /* "%ED" --> "í" */
		  StrSpecialChar[0] = 'í';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xF3:  /* "%F3" --> "ó" */
		  StrSpecialChar[0] = 'ó';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xFA:  /* "%FA" --> "ú" */
		  StrSpecialChar[0] = 'ú';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xF1:  /* "%F1" --> "ñ" */
		  StrSpecialChar[0] = 'ñ';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xE7:  /* "%E7" --> "ç" */
		  StrSpecialChar[0] = 'ç';
		  StrSpecialChar[1] = '\0';
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               default: /* The rest of special chars are stored as special code */
                  snprintf (StrSpecialChar,sizeof (StrSpecialChar),
                            (SpecialChar < 256 &&
                             (ChangeTo == Str_TO_TEXT ||
                              ChangeTo == Str_TO_MARKDOWN)) ? "%c" :
                        	                              "&#%u;",
                	    SpecialChar);
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
              }
            /* Compute length of destination special char */
            LengthSpecStrDst = strlen (StrSpecialChar);

            /* Check new length of destination string after the copy */
            if (LengthStrDst + LengthSpecStrDst > MaxLengthStr)
               Err_ShowErrorAndExit ("Space allocated to string is full.");

            /* Copy to appropiate place the special character string */
            // strncpy (PtrDst,StrSpecialChar,LengthSpecStrDst);
            strcpy (PtrDst,StrSpecialChar);

            /* Increment pointer to character after ';' */
            PtrSrc += LengthSpecStrSrc;
            PtrDst += LengthSpecStrDst;

            /* Increment length of destination string */
            LengthStrDst += LengthSpecStrDst;
           }
         else	// Not a special char ==> copy char from source to destination
           {
            /* Check new length of destination string after the copy */
            if (LengthStrDst >= MaxLengthStr)
               Err_ShowErrorAndExit ("Space allocated to string is full.");

            /* Copy char from source to destination and increment pointers */
            *PtrDst++ = *PtrSrc++;

            /* Increment length of destination string */
            LengthStrDst++;
           }
        }
      StrDst[LengthStrDst] = '\0';

      /***** Copy destination string with changes to source string *****/
      strncpy (Str,StrDst,LengthStrDst);	// Str <-- StrDst
      Str[LengthStrDst] = '\0';

      /***** Free memory used for the destination string *****/
      free (StrDst);
     }

   if (RemoveLeadingAndTrailingSpaces == Str_REMOVE_SPACES)
     {
      /***** Remove leading spaces *****/
      Str_RemoveLeadingSpacesHTML (Str);

      /***** Remove trailing spaces *****/
      Str_RemoveTrailingSpacesHTML (Str);
     }
  }

/*****************************************************************************/
/****************** Remove the spaces iniciales of a string ******************/
/*****************************************************************************/

void Str_RemoveLeadingSpacesHTML (char *Str)
  {
   char *Ptr;

   /***** Find the first character no space from left to right *****/
   for (Ptr = Str;
	*Ptr;
	Ptr++)
     {
      /* If it's space ==> continue in the loop */
      if (isspace ((int) *Ptr) ||
	  *Ptr == '\xA0')	// Unicode translation for &nbsp;
         continue;
      /* Check forward if it's a <br> or <br /> */
      if (*Ptr == '<')
        {
         Ptr++;
         if (tolower ((int) *Ptr) != 'b')
           {
            Ptr--;
            break;
           }
         if (!*Ptr)
            break;
         Ptr++;
         if (tolower ((int) *Ptr) != 'r')
           {
            Ptr -= 2;
            break;
           }
         if (!*Ptr)
            break;
         Ptr++;
         if (*Ptr == '>')
            continue;	// It's <br>
         if (*Ptr != ' ')
           {
            Ptr -= 3;
            break;
           }
         if (!*Ptr)
            break;
         Ptr++;
         if (*Ptr != '/')
           {
            Ptr -= 4;
            break;
           }
         if (!*Ptr)
            break;
         Ptr++;
         if (*Ptr != '>')
           {
            Ptr -= 5;
            break;
           }
         continue;	// It's <br />
        }
      /* Check forward if it's &nbsp; */
      if (*Ptr == '&')
        {
         Ptr++;
         if (tolower ((int) *Ptr) != 'n')
           {
            Ptr--;
            break;
           }
         if (!*Ptr)
            break;
         Ptr++;
         if (tolower ((int) *Ptr) != 'b')
           {
            Ptr -= 2;
            break;
           }
         if (!*Ptr)
            break;
         Ptr++;
         if (tolower ((int) *Ptr) != 's')
           {
            Ptr -= 3;
            break;
           }
         if (!*Ptr)
            break;
         Ptr++;
         if (tolower ((int) *Ptr) != 'p')
           {
            Ptr -= 4;
            break;
           }
         if (!*Ptr)
            break;
         Ptr++;
         if (*Ptr != ';')
           {
            Ptr -= 5;
            break;
           }
         /* It's &nbsp; */
         continue;
        }
      /* It's not space */
      break;
     }

   /***** Ptr is the first character no space *****/
   if (Ptr != Str)
     {
      /* Copy string pointed by Ptr to Str.
         Do not use strcpy because according to strcpy manual:
         "The source and destination strings should not overlap,
         as the behavior is undefined" */
      while (*Ptr)
         *Str++ = *Ptr++;
      *Str = '\0';
     }
  }

/*****************************************************************************/
/******************* Remove the trailing spaces of a string ******************/
/*****************************************************************************/

void Str_RemoveTrailingSpacesHTML (char *Str)
  {
   char *Ptr;

   /* Find the first not space character starting at the end */
   for (Ptr = Str + strlen (Str) - 1;
	Ptr >= Str;
	Ptr--)
     {
      /* If it's space ==> continue in the loop */
      if (isspace ((int) *Ptr) ||
	  *Ptr == '\xA0')	// Unicode translation for &nbsp;
         continue;
      /* Check backward if it's <br> or <br /> */
      if (*Ptr == '>')
        {
         if (Ptr == Str)
            break;
         Ptr--;
         if (*Ptr == '/')	// Possible <br />
           {
            if (Ptr == Str)
               break;
            Ptr--;
            if (*Ptr != ' ')
              {
               Ptr += 2;
               break;
              }
            if (Ptr == Str)
               break;
            Ptr--;
            if (tolower ((int) *Ptr) != 'r')
              {
               Ptr += 3;
               break;
              }
            if (Ptr == Str)
               break;
            Ptr--;
            if (tolower ((int) *Ptr) != 'b')
              {
               Ptr += 4;
               break;
              }
            if (Ptr == Str)
               break;
            Ptr--;
            if (*Ptr != '<')
              {
               Ptr += 5;
               break;
              }
            /* It's <br /> */
            continue;
           }
         else	// Possible <br>
           {
            if (tolower ((int) *Ptr) != 'r')
              {
               Ptr++;
               break;
              }
            if (Ptr == Str)
               break;
            Ptr--;
            if (tolower ((int) *Ptr) != 'b')
              {
               Ptr += 2;
               break;
              }
            if (Ptr == Str)
               break;
            Ptr--;
            if (*Ptr != '<')
              {
               Ptr += 3;
               break;
              }
            /* It's <br> */
            continue;
           }
        }
      /* Check backward if it's &nbsp; */
      if (*Ptr == ';')
        {
         if (Ptr == Str)
            break;
         Ptr--;
         if (tolower ((int) *Ptr) != 'p')
           {
            Ptr ++;
            break;
           }
         if (Ptr == Str)
            break;
         Ptr--;
         if (tolower ((int) *Ptr) != 's')
           {
            Ptr += 2;
            break;
           }
         if (Ptr == Str)
            break;
         Ptr--;
         if (tolower ((int) *Ptr) != 'b')
           {
            Ptr += 3;
            break;
           }
         if (Ptr == Str)
            break;
         Ptr--;
         if (tolower ((int) *Ptr) != 'n')
           {
            Ptr += 4;
            break;
           }
         if (Ptr == Str)
            break;
         Ptr--;
         if (*Ptr != '&')
           {
            Ptr += 5;
            break;
           }
         /* It's &nbsp; */
         continue;
        }
      /* It's not space */
      break;
     }

   *(Ptr + 1) = '\0';
  }

/*****************************************************************************/
/********* Remove the leading zeros ('0', not '\0') from a string ************/
/*****************************************************************************/

void Str_RemoveLeadingZeros (char *Str)
  {
   size_t NumLeadingZeros;

   if (Str)
      if (Str[0])
	{
	 NumLeadingZeros = strspn (Str,"0");
	 if (NumLeadingZeros)
	    // Do not use strcpy / memcpy because the strings overlap
	    memmove (Str,&Str[NumLeadingZeros],
		     strlen (Str) - NumLeadingZeros + 1);
	}
  }

/*****************************************************************************/
/***************** Delete @'s at the start of a string ***********************/
/*****************************************************************************/

#include "swad_database.h"

void Str_RemoveLeadingArrobas (char *Str)
  {
   size_t NumLeadingArr;

   if (Str)
      if (Str[0])
	{
	 NumLeadingArr = strspn (Str,"@");
	 if (NumLeadingArr)
	    // Do not use strcpy / memcpy because the strings overlap
	    memmove (Str,&Str[NumLeadingArr],
		     strlen (Str) - NumLeadingArr + 1);
	}
  }

/*****************************************************************************/
/*************** Find string in a HTML file skipping comments ****************/
/*****************************************************************************/
/*
Search (case insensitive) the string Str in HTML file FileSrc, skipping comments.
Returns true if found.
Returns false if not found.
*/

bool Str_FindStrInFile (FILE *FileSrc,const char *Str,Str_SkipHTMLComments_t SkipHTMLComments)
  {
   int i;
   int Length = strlen (Str);
   int Ch;
   long CurPos;

   for (;;)
     {
      /* Skip possible comments */
      if ((Ch = Str_ReadCharAndSkipComments (FileSrc,SkipHTMLComments)) == EOF)	// Set pointer to '<' if not comment, or to first character after comment if comment
	 return false;
      CurPos = ftell (FileSrc);

      if (Str_ConvertToLowerLetter ((char) Ch) == Str_ConvertToLowerLetter (Str[0]))  // First char found
	{
	 for (i = 1;
	      i < Length;
	      i++)
	   {
            /* Skip possible comments */
 	    if ((Ch = Str_ReadCharAndSkipComments (FileSrc,SkipHTMLComments)) == EOF)	// Set pointer to '<' if not comment, or to first character after comment if comment
	       return false;
	    if (Str_ConvertToLowerLetter ((char) Ch) != Str_ConvertToLowerLetter (Str[i]))
	       break;
	   }
	 if (i == Length) // Found!
	    return true;
         else	// Not found, continue on next character
            fseek (FileSrc,CurPos,SEEK_SET);
	}
     }

   return false;	// Not reached
  }

/*****************************************************************************/
/******************* Search a string in a file backward **********************/
/*****************************************************************************/

/* The file queda posicionado in:
   If found --> in the character anterior to the first of the string
   If no found --> to the principio of the file */

bool Str_FindStrInFileBack (FILE *FileSrc,const char *Str,Str_SkipHTMLComments_t SkipHTMLComments)
  {
   int i;
   int Length = strlen (Str);
   int Ch;
   int ChFinal;
   long CurPos;

   ChFinal = Str_ConvertToLowerLetter (Str[Length-1]);
   if (fseek (FileSrc,-1L,SEEK_CUR))	// Go to the previous character
      return false;
   for (;;)
     {
      if ((Ch = Str_ReadCharAndSkipCommentsBackward (FileSrc,SkipHTMLComments)) == EOF)	// Set pointer to '>' if not comment, or to character before start of comment if comment
         return false;
      CurPos = ftell (FileSrc);

      if (Str_ConvertToLowerLetter ((char) Ch) == (char) ChFinal)
	{
	 for (i = Length - 2;
	      i >= 0;
	      i--)
	   {
	    if (fseek (FileSrc,-2L,SEEK_CUR))
	       return false;
            if ((Ch = Str_ReadCharAndSkipCommentsBackward (FileSrc,SkipHTMLComments)) == EOF)	// Set pointer to '>' if not comment, or to character before start of comment if comment
               return false;
	    if (Str_ConvertToLowerLetter ((char) Ch) != Str_ConvertToLowerLetter (Str[i]))
	       break;
	   }
	 if (i<0)	// Found!
	   {
	    fseek (FileSrc,-1L,SEEK_CUR);	// Move to start of found string in FileSrc
	    return true;
	   }
         else	// Not found, continue on next character
            fseek (FileSrc,CurPos,SEEK_SET);
	}
      if (fseek (FileSrc,-2L,SEEK_CUR))	// Move to previous character
	 return false;
     }

   return false;	// Not reached
  }

/*****************************************************************************/
/************* Search a string in a file writing in FileTgt ******************/
/*****************************************************************************/
/*
Search in the file FileSrc the string Str without distinguish uppercase from
lowercase. Write in FileTgt what is read from the file FileSrc.
*/

bool Str_WriteUntilStrFoundInFileIncludingStr (FILE *FileTgt,FILE *FileSrc,const char *Str,Str_SkipHTMLComments_t SkipHTMLComments)
  {
   int i;
   int StrLength = strlen (Str);
   int Ch;
   long CurPos1,CurPos2;

   for (;;)
     {
      /* Skip possible comments */
      if ((Ch = Str_ReadCharAndSkipCommentsWriting (FileSrc,FileTgt,SkipHTMLComments)) == EOF)	// Set pointer to '<' if not comment, or to first character after comment if comment
	 return false;
      CurPos1 = ftell (FileSrc);
      fputc (Ch,FileTgt);

      if (Str_ConvertToLowerLetter ((char) Ch) == Str_ConvertToLowerLetter (Str[0]))  // Found first character
	{
	 for (i = 1;
	      i < StrLength;
	      i++)
	   {
            /* Skip possible comments */
            if ((Ch = Str_ReadCharAndSkipComments (FileSrc,SkipHTMLComments)) == EOF)	// Set pointer to '<' if not comment, or to first character after comment if comment
	       return false;
	    if (Str_ConvertToLowerLetter ((char) Ch) != Str_ConvertToLowerLetter (Str[i]))
	       break;
	   }
	 if (i == StrLength) // Found!
           {
            CurPos2 = ftell (FileSrc);
            fseek (FileSrc,CurPos1,SEEK_SET);
	    while (ftell (FileSrc) < CurPos2)
	      {
	       Ch = fgetc (FileSrc);
               fputc (Ch,FileTgt);
	      }
	    return true;
           }
	 else	// Not found, continue on next character
            fseek (FileSrc,CurPos1,SEEK_SET);
	}
     }

   return false;	// Not reached
  }

/*****************************************************************************/
/*********************** Skip HTML comments in FileSrc ***********************/
/*****************************************************************************/
// Set pointer to '<' if not comment, or to first character after comment if comment
// Returns last char read after comments

static int Str_ReadCharAndSkipComments (FILE *FileSrc,Str_SkipHTMLComments_t SkipHTMLComments)
  {
   int Ch;
   char StrAux[1 + 1];  /* To check "!--" string */

   Ch = fgetc (FileSrc);

   if (SkipHTMLComments)
      while (Ch == (int) '<')
        {
         /***** Check if "<!--" *****/
         if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"!"))
           {
            Str_FindStrInFileBack (FileSrc,"<",Str_NO_SKIP_HTML_COMMENTS);	// Not a comment. Return to start of directive
            return fgetc (FileSrc);
           }
         if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"-"))
           {
            Str_FindStrInFileBack (FileSrc,"<",Str_NO_SKIP_HTML_COMMENTS);	// Not a comment. Return to start of directive
            return fgetc (FileSrc);
           }
         if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"-"))
           {
            Str_FindStrInFileBack (FileSrc,"<",Str_NO_SKIP_HTML_COMMENTS);	// Not a comment. Return to start of directive
            return fgetc (FileSrc);
           }

         /***** It's a comment ==> skip comment *****/
         Str_FindStrInFile (FileSrc,"-->",Str_NO_SKIP_HTML_COMMENTS);

         /***** Get next character *****/
         Ch = fgetc (FileSrc);
        }

   return Ch;
  }

/*****************************************************************************/
/************* Skip HTML comments in FileSrc writing into FileTgt ************/
/*****************************************************************************/
// Set pointer to '<' if not comment, or to first character after comment if comment
// Returns last char read after comments

static int Str_ReadCharAndSkipCommentsWriting (FILE *FileSrc,FILE *FileTgt,Str_SkipHTMLComments_t SkipHTMLComments)
  {
   int Ch;
   char StrAux[1 + 1];  /* To check "!--" string */

   Ch = fgetc (FileSrc);

   if (SkipHTMLComments)
      while (Ch == (int) '<')
        {
         /***** Check if "<!--" *****/
         if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"!"))
           {
            Str_FindStrInFileBack (FileSrc,"<",Str_NO_SKIP_HTML_COMMENTS);	// Not a comment. Return to start of directive
            return fgetc (FileSrc);
           }
         if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"-"))
           {
            Str_FindStrInFileBack (FileSrc,"<",Str_NO_SKIP_HTML_COMMENTS);	// Not a comment. Return to start of directive
            return fgetc (FileSrc);
           }
         if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"-"))
           {
            Str_FindStrInFileBack (FileSrc,"<",Str_NO_SKIP_HTML_COMMENTS);	// Not a comment. Return to start of directive
            return fgetc (FileSrc);
           }

         /***** It's a comment ==> skip comment *****/
         Str_FindStrInFileBack (FileSrc,"<",Str_NO_SKIP_HTML_COMMENTS);	// Return to start of comment
         Str_WriteUntilStrFoundInFileIncludingStr (FileTgt,FileSrc,"-->",Str_NO_SKIP_HTML_COMMENTS);	// Skip comment in search, and write it to FileTgt

         /***** Get next character *****/
         Ch = fgetc (FileSrc);
        }

   return Ch;
  }

/*****************************************************************************/
/******************************* Skip a comment ******************************/
/*****************************************************************************/
// Set pointer to '>' if not comment, or to character previous to start of comment if comment
// Returns char preceeding comments, of EOF if start of file reached

static int Str_ReadCharAndSkipCommentsBackward (FILE *FileSrc,Str_SkipHTMLComments_t SkipHTMLComments)
  {
   int Ch;
   char StrAux[3 + 1];  /* To check "--" string */

   Ch = fgetc (FileSrc);

   if (SkipHTMLComments)
      while (Ch == (int) '>')
        {
         /* Check if "-->" */
         // Now: "<!--example of comment-->..."
         //                                ^
         if (fseek (FileSrc,-3L,SEEK_CUR))	// Go to start of possible "-->" */
            return EOF;

         // Now: "<!--example of comment-->..."
         //                             ^
         if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,3),"-->"))	// Not a comment
            return '>';

         /***** It's a comment end *****/
         Str_FindStrInFileBack (FileSrc,"<!--",Str_NO_SKIP_HTML_COMMENTS);	// Skip comment backwards
         if (fseek (FileSrc,-1L,SEEK_CUR))		// Return to character previous to '<'
            return EOF;

         /***** Get character previous to '<' *****/
         Ch = fgetc (FileSrc);
        }

   return Ch;
  }

/*****************************************************************************/
/****** Scan next string in file FileSrc until find </td>              *******/
/****** ( skipping comments <!--...--> and directives <...> )          *******/
/****** and write string into Str (MaxLength characters as much)       *******/
/*****************************************************************************/

char *Str_GetCellFromHTMLTableSkipComments (FILE *FileSrc,char *Str,int MaxLength)
  {
   long CurPos;
   long PosNextTR;
   long PosTD;
   int i = 0;
   int Ch;
   bool EndCellFound = false;
   bool DirectiveFound;
   bool SpaceFound;
   char StrAux[1 + 1];  // To find next "/td>" or "nbsp;"

   Str[0] = '\0';

   /***** Find next <td ...> inside current row (before next <tr) *****/
   CurPos = ftell (FileSrc);

   Str_FindStrInFile (FileSrc,"<tr",Str_NO_SKIP_HTML_COMMENTS);
   PosNextTR = ftell (FileSrc) - 3;

   fseek (FileSrc,CurPos,SEEK_SET);
   Str_FindStrInFile (FileSrc,"<td",Str_NO_SKIP_HTML_COMMENTS);
   PosTD = ftell (FileSrc);

   if (PosTD > PosNextTR)
     {
      // <td not found in this row ==> leave current position unchanged
      fseek (FileSrc,CurPos,SEEK_SET);
      return Str;
     }

   Str_FindStrInFile (FileSrc,">",Str_NO_SKIP_HTML_COMMENTS);

   for (EndCellFound = false;
	!EndCellFound;
	)
     {
      if ((Ch = Str_ReadCharAndSkipComments (FileSrc,Str_SKIP_HTML_COMMENTS)) == EOF)	// Set pointer to '<' if not comment, or to first character after comment if comment
	 break;

      /***** Skip directives except </td> *****/
      DirectiveFound = (Ch == (int) '<');

      if (DirectiveFound)		// Start of directive, not a comment
	{
	 /* Check if it's </td> */
	 if (!strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"/"))		// It's </
	    if (!strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"t"))		// It's </t
	       if (!strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"d"))	// It's </td
		  if (!strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),">"))	// It's </td>
	             EndCellFound = true;	// </td> found

	 if (!EndCellFound)
	   {
	    /* Skip directive */
	    Str_FindStrInFileBack (FileSrc,"<",Str_NO_SKIP_HTML_COMMENTS);
	    Str_FindStrInFile (FileSrc,">",Str_NO_SKIP_HTML_COMMENTS);
	   }
	}

      if (!EndCellFound)
	{
	 if (DirectiveFound)
	    Ch = (int) ' ';	// Replace directive for ' ' (separator)
	 else
	   {
	    /***** Check for space or &nbsp; *****/
	    SpaceFound = false;

	    if (Ch == (int) '&')
	      {
	       /* Check for &nbsp; (case insensitive) */
	       if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"n"))
		 {	// It's not &n
		  Str_FindStrInFileBack (FileSrc,"&",Str_NO_SKIP_HTML_COMMENTS);	// Back until &
		  Str_FindStrInFile (FileSrc,"&",Str_NO_SKIP_HTML_COMMENTS);		// Skip &
		 }
	       else if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"b"))
		 {	// It's not &nb
		  Str_FindStrInFileBack (FileSrc,"&",Str_NO_SKIP_HTML_COMMENTS);	// Back until &
		  Str_FindStrInFile (FileSrc,"&",Str_NO_SKIP_HTML_COMMENTS);		// Skip &
		 }
	       else if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"s"))
		 {	// It's not &nbs
		  Str_FindStrInFileBack (FileSrc,"&",Str_NO_SKIP_HTML_COMMENTS);	// Back until &
		  Str_FindStrInFile (FileSrc,"&",Str_NO_SKIP_HTML_COMMENTS);		// Skip &
		 }
	       else if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"p"))
		 {	// It's not &nbsp
		  Str_FindStrInFileBack (FileSrc,"&",Str_NO_SKIP_HTML_COMMENTS);	// Back until &
		  Str_FindStrInFile (FileSrc,"&",Str_NO_SKIP_HTML_COMMENTS);		// Skip &
		 }
	       else if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),";"))
		 {	// It's not &nbsp;
		  Str_FindStrInFileBack (FileSrc,"&",Str_NO_SKIP_HTML_COMMENTS);	// Back until &
		  Str_FindStrInFile (FileSrc,"&",Str_NO_SKIP_HTML_COMMENTS);		// Skip &
		 }
	       else	// It's &nbsp;
		  SpaceFound = true;
	      }

	    /***** Skip spaces *****/
	    if (isspace (Ch) ||
		Ch == 0xC2   ||	// Used in Unicode &nbsp;	// TODO: Skip '\xA0' or the sequence "\xC2\xA0"
		Ch == 0xA0)	// Used in Unicode &nbsp;
		SpaceFound = true;

	    if (SpaceFound)
	       Ch = (int) ' ';	// Replace any kind of space for ' ' (separator)
	   }

	 if (i < MaxLength)
	    Str[i++] = (char) Ch;
	}
     }
   Str[i] = '\0';
   return Str;
  }

/*****************************************************************************/
/******************* Read the next N characters in a string ******************/
/*****************************************************************************/
/*
Read from the file FileSrc the next N characters converting them
to lowercase and store them in Str. Return a pointer to the string.
*/

char *Str_GetNextStrFromFileConvertingToLower (FILE *FileSrc,char *Str, int N)
  {
   int i,Ch;

   for (i = 0;
	i < N;
	i++)
     {
      if ((Ch = fgetc (FileSrc)) == EOF)
	 break;
      Str[i] = Str_ConvertToLowerLetter ((char) Ch);
     }
   Str[i] = '\0';
   return Str;
  }

/*****************************************************************************/
/********** Get from StrSrc into StrDst the next string until space **********/
/*****************************************************************************/
// Modifies *StrSrc

void Str_GetNextStringUntilSpace (const char **StrSrc,char *StrDst,size_t MaxLength)
  {
   size_t i = 0;
   int Ch;

   /***** Skip leading spaces *****/
   do
     {
      if ((Ch = (int) **StrSrc))
	 (*StrSrc)++;
     }
   while (isspace (Ch) ||
	  Ch == 0xC2   ||	// Used in Unicode &nbsp;	// TODO: Skip '\xA0' or the sequence "\xC2\xA0"
    	  Ch == 0xA0);		// Used in Unicode &nbsp;

   /***** Copy string while non-space characters *****/
   while (Ch &&
	  !(isspace (Ch) ||
	    Ch == 0xC2   ||	// Used in Unicode &nbsp;	// TODO: Skip '\xA0' or the sequence "\xC2\xA0"
    	    Ch == 0xA0))	// Used in Unicode &nbsp;
     {
      if (i < MaxLength)
	 StrDst[i++] = (char) Ch;
      if ((Ch = (int) **StrSrc))
	 (*StrSrc)++;
     }
   StrDst[i] = '\0';
  }

/*****************************************************************************/
/******* Get from StrSrc into StrDst the next string until separator *********/
/*****************************************************************************/
// Modifies *StrSrc

void Str_GetNextStringUntilSeparator (const char **StrSrc,char *StrDst,size_t MaxLength)
  {
   size_t i = 0;
   int Ch;

   /***** Skip separators *****/
   do
     {
      if ((Ch = (int) **StrSrc))
	 (*StrSrc)++;
     }
   while (isspace (Ch)    ||
	  Ch == 0xC2      ||	// Used in Unicode &nbsp;	// TODO: Skip '\xA0' or the sequence "\xC2\xA0"
    	  Ch == 0xA0      ||	// Used in Unicode &nbsp;
	  Ch == (int) ',' ||
	  Ch == (int) ';');

   /***** Copy string while no separator found *****/
   while (Ch &&
	  !(isspace (Ch)    ||
	    Ch == 0xC2      ||	// Used in Unicode &nbsp;	// TODO: Skip '\xA0' or the sequence "\xC2\xA0"
    	    Ch == 0xA0      ||	// Used in Unicode &nbsp;
	    Ch == (int) ',' ||
	    Ch == (int) ';'))
     {
      if (i < MaxLength)
	 StrDst[i++] = (char) Ch;
      if ((Ch = (int) **StrSrc))
	 (*StrSrc)++;
     }
   StrDst[i] = '\0';
  }

/*****************************************************************************/
/********** Get from StrSrc into StrDst the next string until comma **********/
/*****************************************************************************/
// Modifies *StrSrc
// Leading spaces are not copied
// Trailing spaces are removed at end

void Str_GetNextStringUntilComma (const char **StrSrc,char *StrDst,size_t MaxLength)
  {
   int Ch;
   char *Ptr;
   size_t i = 0;

   /***** Skip leading spaces and ',' *****/
   Ch = (int) **StrSrc;
   while (isspace (Ch) ||
	  Ch == 0xC2   ||	// Used in Unicode &nbsp;	// TODO: Skip '\xA0' or the sequence "\xC2\xA0"
    	  Ch == 0xA0   ||	// Used in Unicode &nbsp;
	  Ch == (int) ',')
     {
      (*StrSrc)++;
      Ch = (int) **StrSrc;
     }

   /***** Copy string until ',' or end *****/
   Ptr = StrDst;
   while (Ch && Ch != (int) ',')
     {
      if (i < MaxLength)
        {
	 *Ptr++ = (char) Ch;
         i++;
        }
      (*StrSrc)++;
      Ch = (int) **StrSrc;
     }

   /***** Remove trailing spaces *****/
   for (Ptr--;
	Ptr >= *StrSrc;
	Ptr--)
      if (!(isspace ((int) *Ptr) ||
	    *Ptr == '\xA0'))	// Unicode translation for &nbsp;
         break;
   *(Ptr + 1) = '\0';
  }

/*****************************************************************************/
/***************** Replace several spaces of a string for one ****************/
/*****************************************************************************/

void Str_ReplaceSeveralSpacesForOne (char *Str)
  {
   char *PtrSrc, *PtrDst;
   bool PreviousWasSpace = false;

   /***** Do the replacing *****/
   for (PtrDst = PtrSrc = Str;
	*PtrSrc;)
      if (isspace ((int) *PtrSrc) ||
	  *PtrSrc == '\xA0')	// Unicode translation for &nbsp;
        {
         if (!PreviousWasSpace)
            *PtrDst++ = ' ';
         PreviousWasSpace = true;
         PtrSrc++;
        }
      else
        {
         PreviousWasSpace = false;
         *PtrDst++ = *PtrSrc++;
        }
   *PtrDst = '\0';
  }

/*****************************************************************************/
/************* Copy a string to another changing ' ' to '%20' ****************/
/*****************************************************************************/

void Str_CopyStrChangingSpaces (const char *StringWithSpaces,char *StringWithoutSpaces,unsigned MaxLength)
  {
   const char *PtrSrc;
   char *PtrDst;
   unsigned Length = 0;

   for (PtrSrc = StringWithSpaces, PtrDst = StringWithoutSpaces;
	*PtrSrc && Length <= MaxLength;
        PtrSrc++)
      if (*PtrSrc == ' ')
        {
         Length += 3;
         if (Length <= MaxLength)
           {
            *PtrDst++ = '%';
            *PtrDst++ = '2';
            *PtrDst++ = '0';
           }
        }
      else
        {
         Length++;
         if (Length <= MaxLength)
            *PtrDst++ = *PtrSrc;
        }
   *PtrDst = '\0';

   if (Length > MaxLength)
      Err_ShowErrorAndExit ("Path is too long.");
  }

/*****************************************************************************/
/* Convert string with a code (of group type, group, degree, etc.) to long **/
/*****************************************************************************/
// Return -1L if code not found in Str

long Str_ConvertStrCodToLongCod (const char *Str)
  {
   long Code;

   if (!Str)
      return -1L;

   if (Str[0] == '\0')
      return -1L;

   if (sscanf (Str,"%ld",&Code) != 1)
      return -1L;

   return Code;
  }

/*****************************************************************************/
/******************* Get parameter with index of question ********************/
/*****************************************************************************/

unsigned Str_ConvertStrToUnsigned (const char *UnsignedStr)
  {
   long LongNum = Str_ConvertStrCodToLongCod (UnsignedStr);

   return (LongNum >= 0 &&
	   LongNum <= UINT_MAX) ? (unsigned) LongNum :
	                          0;
  }

/*****************************************************************************/
/**** Compute length of root (all except extension) of the name of a file ****/
/*****************************************************************************/

size_t Str_GetLengthRootFileName (const char *FileName)
  {
   char *PtrToDot = strrchr (FileName,(int) '.');
   size_t LengthFileName = strlen (FileName);

   if (PtrToDot)
      return LengthFileName - strlen (PtrToDot);
   else
      return LengthFileName;
  }

/*****************************************************************************/
/************** Get the name of a file from a complete path ******************/
/*****************************************************************************/
// Split a full path in path (without ending '/' ) and a file name

void Str_SplitFullPathIntoPathAndFileName (const char FullPath[PATH_MAX + 1],
                                           char PathWithoutFileName[PATH_MAX + 1],
                                           char FileName[NAME_MAX + 1])
  {
   const char *PtrFileName;
   size_t LengthUntilFileName;

   /***** Find the start of filename *****/
   if ((PtrFileName = strrchr (FullPath,(int) '/')) != NULL)
      PtrFileName++;
   else if ((PtrFileName = strrchr (FullPath,(int) '\\')) != NULL)
      PtrFileName++;
   else
      PtrFileName = FullPath;

   /***** Get PathWithoutFileName *****/
   LengthUntilFileName = (size_t) (PtrFileName - FullPath);	// Last slash included
   if (LengthUntilFileName > 1)
     {
      Str_Copy (PathWithoutFileName,FullPath,PATH_MAX);
      PathWithoutFileName[LengthUntilFileName - 1] = '\0';	// Do not copy ending slash
     }
   else
      PathWithoutFileName[0] = '\0';

   /***** Get FileName *****/
   Str_Copy (FileName,PtrFileName,NAME_MAX);
  }

/*****************************************************************************/
/************** Check if the extension of a file is .Extension ***************/
/*****************************************************************************/
// Return true if FileName ends by .Extension
// Else return false

bool Str_FileIs (const char *FileName,const char *Extension)
  {
   int i;
   int j;
   size_t LengthExtension = strlen (Extension);

   /***** Check length of extension. Extension valid are, for example "zip", "html", "mhtml" *****/
   if (LengthExtension < Fil_MIN_BYTES_FILE_EXTENSION ||
       LengthExtension > Fil_MAX_BYTES_FILE_EXTENSION)
      return false;

   /***** Check the extension *****/
   for (i = strlen (FileName) - 1, j = LengthExtension - 1;
	i > 0 && j >= 0;
	i--, j--)
      if (Str_ConvertToLowerLetter (FileName[i]) != Str_ConvertToLowerLetter (Extension[j]))
	 return false;

   if (j >= 0)	/* If all the characters of the extension have not been checked
		   due to the name of the file is too short */
      return false;

   /***** Check the dot before the extension *****/
   return (FileName[i] == '.');
  }

/*****************************************************************************/
/**************** Check if the extension of a file is .html ******************/
/*****************************************************************************/
// Return true if FileName ends by .htm or .html
// Else return false

bool Str_FileIsHTML (const char *FileName)
  {
   if (Str_FileIs (FileName,"htm"))
      return true;
   return Str_FileIs (FileName,"html");
  }

/*****************************************************************************/
/********************* Check if Path1 starts by Path2 ************************/
/*****************************************************************************/

bool Str_Path1BeginsByPath2 (const char *Path1,const char *Path2)
  {
   /* The string Path1 must start by the complete string Path2 */
   while (*Path2)
      if (*Path2++ != *Path1++)
         return false;

   /* The string Path1 starts by the complete string Path2 */
   /* Check that the next character of Path1 is '\0' or '/' */
   return *Path1 == '\0' || *Path1 == '/';
  }

/*****************************************************************************/
/** Skip spaces in a file seeking it before of reading the first non-blank ***/
/*****************************************************************************/

void Str_SkipSpacesInFile (FILE *FileSrc)
  {
   int Ch;

   while ((Ch = fgetc (FileSrc)) != EOF)
      if (!(isspace (Ch) ||
	    Ch == 0xC2   ||	// Used in Unicode &nbsp;	// TODO: Skip '\xA0' or the sequence "\xC2\xA0"
    	    Ch == 0xA0))	// Used in Unicode &nbsp;
	{
	 fseek (FileSrc,-1L,SEEK_CUR);
	 break;
	}
  }

/*****************************************************************************/
/***************** Write a string to a file changing *************************/
/***************** <br> or <br /> for return         *************************/
/***************** and &nbsp; for space              *************************/
/*****************************************************************************/

void Str_FilePrintStrChangingBRToRetAndNBSPToSpace (FILE *FileTgt,const char *Str)
  {
   while (*Str)
     {
      /* Is &nbsp;? */
      if (*Str == '&')
        {
         if (*(Str + 1) == 'N' || *(Str + 1) == 'n')
            if (*(Str + 2) == 'B' || *(Str + 2) == 'b')
               if (*(Str + 3) == 'S' || *(Str + 3) == 's')
                  if (*(Str + 4) == 'P' || *(Str + 4) == 'p')
                     if (*(Str + 5) == ';')
                       {
                        fputc ((int) ' ',FileTgt);
                        Str += 6;
                        continue;
                       }
        }
      /* Is <br> or <br />? */
      else if (*Str == '<')
        {
         if (*(Str + 1) == 'B' || *(Str + 1) == 'b')
            if (*(Str + 2) == 'R' || *(Str + 2) == 'r')
              {
               if (*(Str + 3) == '>')
                 {
                  fputc ((int) '\n',FileTgt);
                  Str += 4;
                  continue;
                 }
               else if (*(Str + 3) == ' ')
                 {
                  if (*(Str + 4) == '/')
                     if (*(Str + 5) == '>')
                       {
                        fputc ((int) '\n',FileTgt);
                        Str += 6;
                        continue;
                       }
                 }
              }
        }

      fputc ((int) *Str,FileTgt);
      Str++;
     }
  }

/*****************************************************************************/
/*************** Search a string in a file and/or in a string ****************/
/*****************************************************************************/
/*
Search in the file FileSrc the string StrDelimit.
Write in the file FileTgt and/or StrDst the characters read from FileSrc, not including StrDelimit!.
StrDst can be NULL if you don't want to use them.
If StrDelimit is found, return 1.
If what is read exceed MaxLength, abort and return 0.
If StrDelimit is not found, return -1.
*/

#define Str_MAX_BYTES_BOUNDARY_STR 100

int Str_ReadFileUntilBoundaryStr (FILE *FileSrc,char *StrDst,
                                  const char *BoundaryStr,
                                  unsigned LengthBoundaryStr,
                                  unsigned long long MaxLength)
  {
   unsigned NumBytesIdentical;			// Number of characters identical in each iteration of the loop
   unsigned NumBytesReadButNotDiscarded;	// Number of characters read from the source file...
						// ...and not fully discarded in search
   int Buffer[Str_MAX_BYTES_BOUNDARY_STR + 1];
   unsigned StartIndex;
   unsigned i;
   char *Ptr; // Pointer used to go through StrDst writing characters
   unsigned long long DstLength;

   /***** Checkings on boundary string *****/
   if (!LengthBoundaryStr)
     {
      if (StrDst != NULL)
	 *StrDst = '\0';
      return 1;
     }
   if (LengthBoundaryStr > Str_MAX_BYTES_BOUNDARY_STR)
      Err_ShowErrorAndExit ("Delimiter string too large.");
   Ptr = StrDst;

   StartIndex = 0;
   NumBytesReadButNotDiscarded = 0;
   DstLength = 0;

   for (;;)
     {
      if (!NumBytesReadButNotDiscarded)
	{      // Read next character
	 Buffer[StartIndex] = fgetc (FileSrc);
	 if (feof (FileSrc))
	   {
	    if (StrDst != NULL)
	       *Ptr = '\0';
	    return -1;
	   }
	 NumBytesReadButNotDiscarded++;
	}

      if (Buffer[StartIndex] == (int) BoundaryStr[0]) // First character identical
	{
	 for (NumBytesIdentical = 1, i = (StartIndex + 1) % LengthBoundaryStr;
	      NumBytesIdentical < LengthBoundaryStr;
	      NumBytesIdentical++, i = (i + 1) % LengthBoundaryStr)
	   {
	    if (NumBytesReadButNotDiscarded == NumBytesIdentical) // Last character is identical
	      {
	       Buffer[i] = fgetc (FileSrc);  // Read next character
	       if (feof (FileSrc))
		 {
		  if (StrDst != NULL)
		     *Ptr = '\0';
		  return -1;
		 }
	       NumBytesReadButNotDiscarded++;
	      }
	    if (Buffer[i] != (int) BoundaryStr[NumBytesIdentical])  // Next character is different
	       break;
	   }
	 if (NumBytesIdentical == LengthBoundaryStr) // Boundary found
	   {
	    if (StrDst != NULL)
	       *Ptr = '\0';
	    return 1;
	   }
	}

      if (DstLength == MaxLength)
	{
	 if (StrDst != NULL)
	    *Ptr = '\0';
	 return 0;
	}

      if (StrDst != NULL)
	 *Ptr++ = (char) Buffer[StartIndex];

      StartIndex = (StartIndex + 1) % LengthBoundaryStr;
      NumBytesReadButNotDiscarded--;
      DstLength++;
     }

   return 0;	// Not reached
  }

/*****************************************************************************/
/****** Convert invalid characters in a file name to valid characters ********/
/*****************************************************************************/
// Return true if the name of the file o folder is valid
// If the name is not valid, an alert will contain feedback text
// File names with heading and trailing spaces are allowed

bool Str_ConvertFilFolLnkNameToValid (char *FileName)
  {
   extern const char *Txt_UPLOAD_FILE_X_invalid_name_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_X_invalid_name;
   extern const char *Txt_UPLOAD_FILE_Invalid_name_NO_HTML;
   extern const char *Txt_UPLOAD_FILE_Invalid_name;
   char *Ptr;
   unsigned NumAlfanum = 0;
   unsigned NumSpaces  = 0;
   unsigned NumPoints  = 0;
   bool FileNameIsOK = false;

   Ptr = FileName;
   if (*Ptr)	// FileName is not empty
     {
      for (;
	   *Ptr;
	   Ptr++)
	{
	 if (*Ptr == ' ')
	   {
	    NumPoints = 0;
	    if (++NumSpaces > 1)
	      {
	       *Ptr = '_';
	       NumAlfanum++;
	       NumSpaces = NumPoints = 0;
	      }
	   }
	 else if (*Ptr == '.')
	   {
	    if (++NumPoints > 1)	// Don't allow ".."
	      {
	       *Ptr = '_';
	       NumAlfanum++;
	       NumSpaces = NumPoints = 0;
	      }
	   }
	 else
	   {
	    switch (*Ptr)
	      {
	       case 'á': *Ptr = 'a'; break;
	       case 'é': *Ptr = 'e'; break;
	       case 'í': *Ptr = 'i'; break;
	       case 'ó': *Ptr = 'o'; break;
	       case 'ú': *Ptr = 'u'; break;
	       case 'ñ': *Ptr = 'n'; break;
	       case 'ä': *Ptr = 'a'; break;
	       case 'ë': *Ptr = 'e'; break;
	       case 'ï': *Ptr = 'i'; break;
	       case 'ö': *Ptr = 'o'; break;
	       case 'ü': *Ptr = 'u'; break;
	       case 'ç': *Ptr = 'c'; break;

	       case 'Á': *Ptr = 'A'; break;
	       case 'É': *Ptr = 'E'; break;
	       case 'Í': *Ptr = 'I'; break;
	       case 'Ó': *Ptr = 'O'; break;
	       case 'Ú': *Ptr = 'U'; break;
	       case 'Ñ': *Ptr = 'N'; break;
	       case 'Ä': *Ptr = 'A'; break;
	       case 'Ë': *Ptr = 'E'; break;
	       case 'Ï': *Ptr = 'I'; break;
	       case 'Ö': *Ptr = 'O'; break;
	       case 'Ü': *Ptr = 'U'; break;
	       case 'Ç': *Ptr = 'C'; break;
	      }
	    if ((*Ptr >= 'a' && *Ptr <= 'z') ||
		(*Ptr >= 'A' && *Ptr <= 'Z') ||
		(*Ptr >= '0' && *Ptr <= '9') ||
		 *Ptr == '_' ||
		 *Ptr == '-')
	      {
	       NumAlfanum++;
	       NumSpaces = NumPoints = 0;
	      }
	    else
	      {
	       *Ptr = '_';
	       NumAlfanum++;
	       NumSpaces = NumPoints = 0;
	      }
	   }
	}
      if (NumAlfanum)
         FileNameIsOK = true;
      else
	 Ale_CreateAlert (Ale_WARNING,NULL,
			  Gbl.FileBrowser.UploadingWithDropzone ? Txt_UPLOAD_FILE_X_invalid_name_NO_HTML :
								  Txt_UPLOAD_FILE_X_invalid_name,
			  FileName);
     }
   else	// FileName is empty
      Ale_CreateAlert (Ale_WARNING,NULL,
		       Gbl.FileBrowser.UploadingWithDropzone ? Txt_UPLOAD_FILE_Invalid_name_NO_HTML :
							       Txt_UPLOAD_FILE_Invalid_name);

   return FileNameIsOK;
  }

/*****************************************************************************/
/************ Convert a string to a valid name of file or folder *************/
/*****************************************************************************/

void Str_ConvertToValidFileName (char *Str)
  {
   char *Ptr;

   for (Ptr = Str;
	*Ptr;
	Ptr++)
     {
      if ((*Ptr >= 'a' && *Ptr <= 'z') ||
	  (*Ptr >= 'A' && *Ptr <= 'Z') ||
	  (*Ptr >= '0' && *Ptr <= '9') ||
	  *Ptr == '_' ||
	  *Ptr == '-')
         continue;
      if (isspace ((int) *Ptr) ||
	  *Ptr == '\xA0')
	 *Ptr = '_';
      else
	 switch (*Ptr)
	   {
	    case 'á': case 'à': case 'ä': case 'â': *Ptr = 'a'; break;
	    case 'é': case 'è': case 'ë': case 'ê': *Ptr = 'e'; break;
	    case 'í': case 'ì': case 'ï': case 'î': *Ptr = 'i'; break;
	    case 'ó': case 'ò': case 'ö': case 'ô': *Ptr = 'o'; break;
	    case 'ú': case 'ù': case 'ü': case 'û': *Ptr = 'u'; break;
	    case 'ñ': *Ptr = 'n'; break;
	    case 'ç': *Ptr = 'c'; break;

	    case 'Á': case 'À': case 'Ä': case 'Â': *Ptr = 'A'; break;
	    case 'É': case 'È': case 'Ë': case 'Ê': *Ptr = 'E'; break;
	    case 'Í': case 'Ì': case 'Ï': case 'Î': *Ptr = 'I'; break;
	    case 'Ó': case 'Ò': case 'Ö': case 'Ô': *Ptr = 'O'; break;
	    case 'Ú': case 'Ù': case 'Ü': case 'Û': *Ptr = 'U'; break;
	    case 'Ñ': *Ptr = 'N'; break;
	    case 'Ç': *Ptr = 'C'; break;

	    default: *Ptr = '-'; break;
	   }
     }
  }

/*****************************************************************************/
/******************* Create a random alphanumeric string *********************/
/*****************************************************************************/

#define NUM_ALPHANUM_CHARS (10 + 26 + 26)

void Str_CreateRandomAlphanumStr (char *Str,size_t Length)
  {
   static char CharTable[NUM_ALPHANUM_CHARS] =
     {'0','1','2','3','4','5','6','7','8','9',
      'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
      'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'
     };
   size_t i;

   /***** Set random chars in string *****/
   for (i = 0;
	i <Length;
	i++)
      Str[i] = CharTable[(unsigned) (((double) rand () * (double) (NUM_ALPHANUM_CHARS-1)) /
	                              (double) RAND_MAX + 0.5)];
   Str[Length] = '\0';
  }

/*****************************************************************************/
/****************************** Safe string copy *****************************/
/*****************************************************************************/
// If Dst is not null, it should have at least DstSize + 1 bytes
// DstSize does not include the ending byte '\0'

void Str_Copy (char *Dst,const char *Src,size_t DstSize)
  {
   size_t SrcLength;
   char ErrorTxt[128];

   /***** Trivial check: destination pointer should be not null *****/
   if (Dst == NULL)
      Err_ShowErrorAndExit ("Destination of copy is null.");

   /***** Trivial check: if source is null >= clear destination *****/
   if (Src == NULL)
     {
      Dst[0] = '\0';
      return;
     }

   /***** Trivial check: if source is an empty string ==> clear destination *****/
   if (Src[0] == '\0')
     {
      Dst[0] = '\0';
      return;
     }

   /***** Check if buffer has enough space for source *****/
   if ((SrcLength = strlen (Src)) > DstSize)
     {
      snprintf (ErrorTxt,sizeof (ErrorTxt),
	        "Trying to copy %zu chars into a %zu-chars buffer.",
                SrcLength,DstSize);
      Err_ShowErrorAndExit (ErrorTxt);
     }

   /***** Copy source into destination *****/
   strcpy (Dst,Src);
  }

/*****************************************************************************/
/************************** Safe string concatenation ************************/
/*****************************************************************************/
// DstSize does not include ending byte '\0'

void Str_Concat (char *Dst,const char *Src,size_t DstSize)
  {
   size_t DstLength;
   size_t SrcLength;
   size_t FreeSpace;
   char ErrorTxt[256];

   /***** Check if buffer has already overflowed *****/
   DstLength = strlen (Dst);
   if (DstLength > DstSize)
     {
      snprintf (ErrorTxt,sizeof (ErrorTxt),"%lu-chars buffer has %lu chars!",
                DstSize,DstLength);
      Err_ShowErrorAndExit (ErrorTxt);
     }

   /***** Check if buffer has enough space for source *****/
   // DstSize >= DstLength ==> FreeSpace >= 0
   FreeSpace = DstSize - DstLength;
   SrcLength = strlen (Src);
   if (FreeSpace < SrcLength)
     {
      snprintf (ErrorTxt,sizeof (ErrorTxt),
	        "Trying to concatenate %zu chars to a %zu-chars buffer"
	        " with free space for only %zu chars!",
                SrcLength,DstSize,FreeSpace);
      Err_ShowErrorAndExit (ErrorTxt);
     }

   /***** Concatenate ******/
   strcat (Dst,Src);
  }

/*****************************************************************************/
/********************** Build a "Go to <where>" title ************************/
/*****************************************************************************/
// Where is a hierarchy member (country, institution, center, degree or course
// Str_FreeGoToTitle() must be called after calling this function

static char *Str_Title = NULL;

char *Str_BuildGoToTitle (const char *Where)
  {
   extern const char *Txt_Go_to_X;

   if (Str_Title != NULL)
      Err_ShowErrorAndExit ("Can not build go to title.");

   if (asprintf (&Str_Title,Txt_Go_to_X,Where) < 0)
      Err_NotEnoughMemoryExit ();

   return Str_Title;
  }

void Str_FreeGoToTitle (void)
  {
   free (Str_Title);
   Str_Title = NULL;
  }
