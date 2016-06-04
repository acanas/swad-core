// swad_string.c: string processing

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <ctype.h>		// For isprint, isspace, etc.
#include <locale.h>		// For setlocale
#include <math.h>		// For log10, floor, ceil, modf, sqrt...
#include <stdlib.h>		// For malloc and free
#include <string.h>		// For string functions

#include "swad_global.h"
#include "swad_ID.h"
#include "swad_parameter.h"
#include "swad_string.h"

/*****************************************************************************/
/******************** Global variables from other modules ********************/
/*****************************************************************************/

extern struct Globals Gbl;		// Declaration in swad.c

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/*************************** Internal prototypes *****************************/
/*****************************************************************************/

static unsigned Str_GetNextASCIICharFromStr (const char *Ptr,unsigned char *Ch);
static int Str_ReadCharAndSkipComments (FILE *FileSrc,Str_SkipHTMLComments_t SkipHTMLComments);
static int Str_ReadCharAndSkipCommentsWriting (FILE *FileSrc,FILE *FileTgt,Str_SkipHTMLComments_t SkipHTMLComments);
static int Str_ReadCharAndSkipCommentsBackward (FILE *FileSrc,Str_SkipHTMLComments_t SkipHTMLComments);

/*****************************************************************************/
/**************************** Internal constants *****************************/
/*****************************************************************************/

/***** Conversion to Base64URL *****/
// base64url is described in document http://tools.ietf.org/html/rfc4648.
// It uses '-' and '_' because they are safe for URL/parameters (without enconding) and for filenames.
const char Str_BIN_TO_BASE64URL[64] =
  {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
   'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
   '0','1','2','3','4','5','6','7','8','9','-','_'};

static const char Str_LF[2] = {10,0};
static const char Str_CR[2] = {13,0};

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************** Insert a link in every URL or nickname *******************/
/*****************************************************************************/
/*
Insertion example:
The web site of @rms is https://stallman.org/
The web site of <a href="https://openswad.org/?usr=@rms">@rms</a> is <a href="https://stallman.org/" target="_blank">https://stallman.org/</a>
*/

/*

<form id="form_z7FY4oFr-yot9R9xRVwDhIntnod3geLj36nyQ6jlJJs_53"
action="https://localhost/swad/es" method="post">
<input type="hidden" name="ses" value="2jb9CGhIJ81_qhDyeQ6MWDFKQ5ZaA_F68tq22ZAjYww">
<input type="hidden" name="usr" value="@acanas">
<a href="" onclick="document.getElementById('form_z7FY4oFr-yot9R9xRVwDhIntnod3geLj36nyQ6jlJJs_53').submit();return false;">
@acanas
</a>
</form>

*/

#define ANCHOR_1_URL	"<a href=\""
#define ANCHOR_2_URL	"\" target=\"_blank\">"
#define ANCHOR_3_URL	"</a>"
#define ANCHOR_3_NICK	"</a></form>"

#define MAX_LINKS 1000

#define MAX_BYTES_LIMITED_URL 1024	// Max. number of bytes of the URL shown on screen

void Str_InsertLinks (char *Txt,unsigned long MaxLength,size_t MaxCharsURLOnScreen)
  {
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];
   char ParamsStr[256+256+Ses_LENGTH_SESSION_ID+256];
   char Anchor1Nick[256+256+256+Ses_LENGTH_SESSION_ID+256+256];
   char Anchor2Nick[256+Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64];
   size_t TxtLength;
   size_t TxtLengthWithInsertedAnchors;

   size_t Anchor1URLLength;
   size_t Anchor2URLLength;
   size_t Anchor3URLLength;
   size_t AnchorURLTotalLength;

   size_t Anchor1NickLength = 0;	// Initialized only to avoid warning
   size_t Anchor2NickLength = 0;	// Initialized only to avoid warning
   size_t Anchor3NickLength;
   size_t AnchorNickTotalLength = 0;	// Initialized only to avoid warning

   char *PtrSrc;
   char *PtrDst;
   bool URLStartFound;
   bool IsNickname;
   int NumLinks = 0;
   int NumLink;
   struct
     {
      char *PtrStart;			// Pointer to the first char of URL/nickname in original text
      char *PtrEnd;			// Pointer to the last  char of URL/nickname in original text
      size_t NumActualBytes;		// Actual length of the URL/nickname
      char *Anchor1Nick;
      char *Anchor2Nick;
      size_t Anchor1NickLength;
      size_t Anchor2NickLength;
      size_t AddedLengthUntilHere;	// Total length of extra HTML code added until this link (included)
     } Links[MAX_LINKS];
   size_t LengthVisibleLink;
   size_t Length;
   size_t i;
   size_t NumChars1;
   size_t NumChars2;
   size_t NumBytesToCopy;
   size_t NumBytesToShow;		// Length of the link displayed on screen (may be shorter than actual length)
   char LimitedURL[MAX_BYTES_LIMITED_URL+1];
   unsigned char Ch;

   /****** Initialize constant anchors and their lengths *****/
   TxtLength = strlen (Txt);

   // For URLs the length of anchor is fixed
   // so it can be calculated once
   Anchor1URLLength = strlen (ANCHOR_1_URL);
   Anchor2URLLength = strlen (ANCHOR_2_URL);
   Anchor3URLLength = strlen (ANCHOR_3_URL);
   AnchorURLTotalLength  = Anchor1URLLength +
	                   Anchor2URLLength +
	                   Anchor3URLLength;

   // For nicknames the length of anchor is variable
   // so it can be calculated for each link,
   // except the third part that is fixed
   Anchor3NickLength = strlen (ANCHOR_3_NICK);

   /**************************************************************/
   /***** Find starts and ends of links (URLs and nicknames) *****/
   /**************************************************************/
   for (PtrSrc = Txt;
	*PtrSrc;)
      /* Check if the next char is the start of a URL */
      if (tolower ((int) *PtrSrc) == (int) 'h')
        {
         URLStartFound = false;
         Links[NumLinks].PtrStart = PtrSrc;
         if (tolower ((int) *++PtrSrc) == (int) 't') // ht...
           {
            if (tolower ((int) *++PtrSrc) == (int) 't') // htt...
              {
               if (tolower ((int) *++PtrSrc) == (int) 'p') // http...
                 {
                  PtrSrc++;
                  if (*PtrSrc == ':') // http:...
                    {
                     if (*++PtrSrc ==  '/') // http:/...
                        if (*++PtrSrc == '/') // http://...
                           URLStartFound = true;
                    }
                  else if (tolower ((int) *PtrSrc) == (int) 's') // https...
                    {
                     if (*++PtrSrc == ':') // https:...
                       {
                        if (*++PtrSrc == '/') // https:/...
                           if (*++PtrSrc == '/') // https://...
                              URLStartFound = true;
                       }
                    }
                 }
              }
           }
         if (URLStartFound)
           {
            /* Find URL end */
            PtrSrc++;	// Points to first character after http:// or https://
            for (;;)
              {
               NumChars1 = Str_GetNextASCIICharFromStr (PtrSrc,&Ch);
               PtrSrc += NumChars1;
               if (Ch <= 32 || Ch == '<'  || Ch == '"')
                 {
                  Links[NumLinks].PtrEnd = PtrSrc - NumChars1 - 1;
                  break;
                 }
               else if (Ch == ',' || Ch == '.' || Ch == ';' || Ch == ':' || Ch == ')' || Ch == ']' || Ch == '}')
                 {
                  NumChars2 = Str_GetNextASCIICharFromStr (PtrSrc,&Ch);
                  PtrSrc += NumChars2;
                  if (Ch <= 32 || Ch == '<' || Ch == '"')
                    {
                     Links[NumLinks].PtrEnd = PtrSrc - NumChars2 - NumChars1 - 1;
                     break;
                    }
                 }
              }

            /* Initialize anchors for this link */
            Links[NumLinks].Anchor1Nick = NULL;
            Links[NumLinks].Anchor2Nick = NULL;

            /* Calculate length of this URL */
            Links[NumLinks].NumActualBytes = (size_t) (Links[NumLinks].PtrEnd + 1 - Links[NumLinks].PtrStart);
            if (Links[NumLinks].NumActualBytes <= MaxCharsURLOnScreen)
               LengthVisibleLink = Links[NumLinks].NumActualBytes;
            else	// If URL is too long to be displayed ==> short it
              {
               /* Make a copy of this URL */
               NumBytesToCopy = (Links[NumLinks].NumActualBytes < MAX_BYTES_LIMITED_URL) ? Links[NumLinks].NumActualBytes :
        	                                                                           MAX_BYTES_LIMITED_URL;
               strncpy (LimitedURL,Links[NumLinks].PtrStart,NumBytesToCopy);
               LimitedURL[NumBytesToCopy] = '\0';

               /* Limit the number of characters on screen of the copy, and calculate its length in bytes */
               LengthVisibleLink = Str_LimitLengthHTMLStr (LimitedURL,MaxCharsURLOnScreen);
              }
            if (NumLinks == 0)
               Links[NumLinks].AddedLengthUntilHere = AnchorURLTotalLength + LengthVisibleLink;
            else
               Links[NumLinks].AddedLengthUntilHere = Links[NumLinks - 1].AddedLengthUntilHere +
                                                      AnchorURLTotalLength + LengthVisibleLink;

	    /* Increment number of found links */
	    NumLinks++;
	    if (NumLinks == MAX_LINKS)
               break;
           }
        }
      /* Check if the next char is the start of a nickname */
      else if ((int) *PtrSrc == (int) '@')
	{
         Links[NumLinks].PtrStart = PtrSrc;

	 /* Find nickname end */
	 PtrSrc++;	// Points to first character after @

	 /* A nick can have digits, letters and '_'  */
	 for (;
	      *PtrSrc;
	      PtrSrc++)
	    if (!((*PtrSrc >= 'a' && *PtrSrc <= 'z') ||
		  (*PtrSrc >= 'A' && *PtrSrc <= 'Z') ||
		  (*PtrSrc >= '0' && *PtrSrc <= '9') ||
		  (*PtrSrc == '_')))
	       break;

	 /* Calculate length of this nickname */
	 Links[NumLinks].PtrEnd = PtrSrc - 1;
         Links[NumLinks].NumActualBytes = (size_t) (PtrSrc - Links[NumLinks].PtrStart);

	 /* A nick (without arroba) must have a number of characters
            Nck_MIN_LENGTH_NICKNAME_WITHOUT_ARROBA <= Length <= Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA */
	 Length = Links[NumLinks].NumActualBytes - 1;	// Do not count the initial @
	 IsNickname = (Length >= Nck_MIN_LENGTH_NICKNAME_WITHOUT_ARROBA &&
	               Length <= Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA);

	 if (IsNickname)
	   {
            /* Initialize anchors for this link */
            Links[NumLinks].Anchor1Nick = NULL;
            Links[NumLinks].Anchor2Nick = NULL;

	    /* Create id for this form */
	    Gbl.Form.Num++;
	    if (Gbl.Usrs.Me.Logged)
	       sprintf (Gbl.Form.UniqueId,"form_%s_%d",
			Gbl.UniqueNameEncrypted,Gbl.Form.Num);
	    else
	       sprintf (Gbl.Form.Id,"form_%d",Gbl.Form.Num);

	    /* Store first part of anchor */
	    Act_SetParamsForm (ParamsStr,ActSeePubPrf,true);
	    sprintf (Anchor1Nick,"<form method=\"post\" action=\"%s/%s\" id=\"%s\">"
				 "%s"
				 "<input type=\"hidden\" name=\"usr\" value=\"",
		     Cfg_HTTPS_URL_SWAD_CGI,
		     Txt_STR_LANG_ID[Gbl.Prefs.Language],
		     Gbl.Usrs.Me.Logged ? Gbl.Form.UniqueId :
			                  Gbl.Form.Id,
		     ParamsStr);
	    Anchor1NickLength = strlen (Anchor1Nick);
	    if ((Links[NumLinks].Anchor1Nick = (char *) malloc (Anchor1NickLength+1)) == NULL)
	       Lay_ShowErrorAndExit ("Not enough memory to insert link.");
	    strcpy (Links[NumLinks].Anchor1Nick,Anchor1Nick);
	    Links[NumLinks].Anchor1NickLength = Anchor1NickLength;

	    /* Store second part of anchor */
	    sprintf (Anchor2Nick,"\">"
				 "<a href=\"\""
				 " onclick=\"document.getElementById('%s').submit();"
				 "return false;\">",
		     Gbl.Usrs.Me.Logged ? Gbl.Form.UniqueId :
			                  Gbl.Form.Id);
	    Anchor2NickLength = strlen (Anchor2Nick);
	    if ((Links[NumLinks].Anchor2Nick = (char *) malloc (Anchor2NickLength+1)) == NULL)
	       Lay_ShowErrorAndExit ("Not enough memory to insert link.");
	    strcpy (Links[NumLinks].Anchor2Nick,Anchor2Nick);
	    Links[NumLinks].Anchor2NickLength = Anchor2NickLength;

	    AnchorNickTotalLength = Anchor1NickLength + Anchor2NickLength + Anchor3NickLength;

	    LengthVisibleLink = Links[NumLinks].NumActualBytes;
            if (NumLinks == 0)
               Links[NumLinks].AddedLengthUntilHere = AnchorNickTotalLength + LengthVisibleLink;
            else
               Links[NumLinks].AddedLengthUntilHere = Links[NumLinks - 1].AddedLengthUntilHere +
                                                      AnchorNickTotalLength + LengthVisibleLink;

	    /* Increment number of found links */
	    NumLinks++;
	    if (NumLinks == MAX_LINKS)
	       break;
	   }
	}
      /* The next char is not the start of URL or nickname */
      else	// Character distinct to 'h' or '@'
         PtrSrc++;

   /**********************************************************************/
   /***** If there are one or more links (URLs or nicknames) in text *****/
   /**********************************************************************/
   if (NumLinks)
     {
      /***** Insert links from end to start of text,
             only if there is enough space available in text *****/
      TxtLengthWithInsertedAnchors = TxtLength + Links[NumLinks - 1].AddedLengthUntilHere;
      if (TxtLengthWithInsertedAnchors <= MaxLength)
         for (NumLink = NumLinks - 1;
              NumLink >= 0;
              NumLink--)
           {
            IsNickname = (*(Links[NumLink].PtrStart) == '@');

            /***** Step 1: Move forward the text after the link (URL or nickname)
                           (it's mandatory to do the copy in reverse order
                            to avoid overwriting source) *****/
            for (i = 0,
                 PtrSrc = (NumLink == NumLinks - 1) ? Txt + TxtLength :
                                                      Links[NumLink + 1].PtrStart - 1,
                 PtrDst = PtrSrc + Links[NumLink].AddedLengthUntilHere,
                 Length = PtrSrc - Links[NumLink].PtrEnd;
                 i < Length;
                 i++)
               *PtrDst-- = *PtrSrc--;

            /***** Step 2: Insert ANCHOR_3_NICK or ANCHOR_3_URL *****/
            if (IsNickname)
              {
	       Length = Anchor3NickLength;
	       PtrSrc = ANCHOR_3_NICK + Length - 1;
              }
            else
              {
	       Length = Anchor3URLLength;
	       PtrSrc = ANCHOR_3_URL + Length - 1;
              }
	    for (i = 0;
		 i < Length;
		 i++)
               *PtrDst-- = *PtrSrc--;

            /***** Step 3: Move forward the link (URL or nickname)
                           to be shown on screen *****/
            if (IsNickname ||
        	Links[NumLink].NumActualBytes <= MaxCharsURLOnScreen)
              {
               NumBytesToShow = Links[NumLink].NumActualBytes;
               PtrSrc = Links[NumLink].PtrEnd;			// PtrSrc must point to end of complete nickname
              }
            else	// If URL is too long to be displayed ==> short it
              {
               /* Make a copy of this URL */
               NumBytesToCopy = (Links[NumLink].NumActualBytes < MAX_BYTES_LIMITED_URL) ? Links[NumLink].NumActualBytes :
        	                                                                          MAX_BYTES_LIMITED_URL;
               strncpy (LimitedURL,Links[NumLink].PtrStart,NumBytesToCopy);
               LimitedURL[NumBytesToCopy] = '\0';

               /* Limit the length of the copy */
               NumBytesToShow = Str_LimitLengthHTMLStr (LimitedURL,MaxCharsURLOnScreen);

               PtrSrc = LimitedURL + NumBytesToShow - 1;	// PtrSrc must point to end of limited URL
              }
            for (i = 0;
        	 i < NumBytesToShow;
        	 i++)
               *PtrDst-- = *PtrSrc--;

            /***** Step 4: Insert Anchor2Nick or ANCHOR_2_URL *****/
            if (IsNickname)
              {
	       Length = Links[NumLink].Anchor2NickLength;
	       PtrSrc = Links[NumLink].Anchor2Nick + Length - 1;
              }
            else
              {
	       Length = Anchor2URLLength;
	       PtrSrc = ANCHOR_2_URL + Length - 1;
              }
	    for (i = 0;
		 i < Length;
		 i++)
	       *PtrDst-- = *PtrSrc--;

            /***** Step 5: Insert link into directive A
                           (it's mandatory to do the copy in reverse order
                           to avoid overwriting source URL or nickname) *****/
            for (i = 0, PtrSrc = Links[NumLink].PtrEnd;
        	 i < Links[NumLink].NumActualBytes;
        	 i++)
               *PtrDst-- = *PtrSrc--;

            /***** Step 6: Insert Anchor1Nick or ANCHOR_1_URL *****/
            if (IsNickname)
              {
	       Length = Links[NumLink].Anchor1NickLength;
	       PtrSrc = Links[NumLink].Anchor1Nick + Length - 1;
              }
            else
              {
	       Length = Anchor1URLLength;
	       PtrSrc = ANCHOR_1_URL + Length - 1;
              }
	    for (i = 0;
		 i < Length;
		 i++)
	       *PtrDst-- = *PtrSrc--;
           }
     }

   /***********************************/
   /***** Free memory for anchors *****/
   /***********************************/
   for (NumLink = 0;
	NumLink < NumLinks;
	NumLink++)
     {
      if (Links[NumLink].Anchor1Nick)
	 free ((void *) Links[NumLink].Anchor1Nick);
      if (Links[NumLink].Anchor2Nick)
	 free ((void *) Links[NumLink].Anchor2Nick);
     }
  }

/*****************************************************************************/
/** Get next ASCII character from a string converting &#number; to character */
/*****************************************************************************/
// Returns number of char analyzed

static unsigned Str_GetNextASCIICharFromStr (const char *Ptr,unsigned char *Ch)
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
            if (Num < 256)	// To avoid overflow
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
               default:  *Ch = ' ';  return NumChars;	// Unknown character
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

   if (MaxCharsOnScreen < 3)
      MaxCharsOnScreen = 3;	// Length of "..."

   /***** Calculate length counting "&...;" as one character *****/
   for (Ptr = Str, NumCharsOnScreen = 0, Length = 0;
	*Ptr;
	Ptr++, NumCharsOnScreen++, Length++)
      if (*Ptr == '&')	// Special character
	 for (Ptr++, Length++;
	      *Ptr && *Ptr != ';';
	      Ptr++, Length++);  // While not end of special character

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
      if (*Ptr == '&')		// Special character
	 for (Ptr++, Length++;
	      *Ptr && *Ptr != ';';
	      Ptr++, Length++);	// While not end of special character
      else if (*Ptr == '<')	// HTML entity
	 for (Ptr++, Length++;
	      *Ptr && *Ptr != '>';
	      Ptr++, Length++);	// While not end of HTML entity
     }

   return Length;
  }

/*****************************************************************************/
/**************** Check if a URL adreess looks as** valid ********************/
/*****************************************************************************/
/*
bool Str_URLLooksValid (const char *URL)
  {
   ***** If it's a NULL pointer *****
   if (!URL)
      return false;

   ***** If it's the empty string *****
   if (!URL[0])
      return false;

   ***** Check if start by http:// or https:// *****
   if (!strncasecmp (URL,"http://",7) || !strncasecmp (URL,"https://",8))
      return (bool) (strchr (URL,(int) '.') != NULL);	// There is any . in the URL
   else	// There's no http:// nor https://
      return false;
  }
*/
/*****************************************************************************/
/***** Convert a string to title: first uppercase and the rest lowercase *****/
/*****************************************************************************/

void Str_ConvertToTitleType (char *Str)
  {
   char Ch,*Ptr,*Ptr2;
   int LengthStr;
   bool FirstLetter = true;

   for (Ptr = Str;
	*Ptr;
	Ptr++)
     {
      Ch = *Ptr;
      if (isspace ((int) Ch) ||
	  Ch == '\xA0' ||	// Unicode translation for &nbsp;
	  Ch == '-' ||
	  Ch == '(' ||
	  Ch == ')' ||
	  Ch == ',' ||
	  Ch == ';' ||
	  Ch == '.' ||
	  Ch == ':' ||
	  Ch == 'ª' ||
	  Ch == 'º')
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
/******** Write a number in floating point with the correct accuracy *********/
/*****************************************************************************/

void Str_WriteFloatNum (float Number)
  {
   double IntegerPart;
   double FractionaryPart;
   char *Format;

   FractionaryPart = modf ((double) Number,&IntegerPart);

   if (FractionaryPart == 0.0)
      fprintf (Gbl.F.Out,"%.0f",IntegerPart);
   else
     {
      if (IntegerPart != 0.0 || FractionaryPart >= 0.1)
         Format = "%.2f";
      else if (FractionaryPart >= 0.01)
         Format = "%.3f";
      else if (FractionaryPart >= 0.001)
         Format = "%.4f";
      else if (FractionaryPart >= 0.0001)
         Format = "%.5f";
      else if (FractionaryPart >= 0.00001)
         Format = "%.6f";
      else if (FractionaryPart >= 0.000001)
         Format = "%.7f";
      else
         Format = "%e";
      fprintf (Gbl.F.Out,Format,Number);
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
/************** Read a number in floating point from a string ****************/
/*****************************************************************************/

float Str_GetFloatNumFromStr (const char *Str)
  {
   float Num;

   if (Str)
     {
      if (!setlocale (LC_NUMERIC,"en_US.utf8"))	// To get the decimal point
	 Lay_ShowAlert (Lay_ERROR,"Can not set locale to en_US.");
      if (sscanf (Str,"%f",&Num) != 1)
         Lay_ShowErrorAndExit ("Bad floating point format.");
      if (!setlocale (LC_NUMERIC,"es_ES.utf8"))
	 Lay_ShowAlert (Lay_ERROR,"Can not set locale to es_ES.");
     }
   else // Str == NULL
      Num = 0.0;

   return Num;
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
void Str_ChangeFormat (Str_ChangeFrom_t ChangeFrom,Str_ChangeTo_t ChangeTo,
                       char *Str,size_t MaxLengthStr,bool RemoveLeadingAndTrailingSpaces)
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
   char StrSpecialChar[256];

/*
  if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)
    {
     sprintf (Gbl.Message,"Str_ChangeFormat (&quot;%s&quot;)",Str);
     Lay_ShowAlert (Lay_INFO,Gbl.Message);
    }
*/

   if (ChangeTo != Str_DONT_CHANGE)
     {
      /***** Allocate memory for a destination string where to do the changes *****/
      if ((StrDst = (char *) malloc (MaxLengthStr)) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to change the format of a string.");

      /***** Make the change *****/
      for (PtrSrc = Str, PtrDst = StrDst;
	   *PtrSrc;)
        {
         Ch = (unsigned char) *PtrSrc;
         switch (ChangeFrom)
           {
            case Str_FROM_FORM:
               if (Gbl.ContentReceivedByCGI == Act_CONTENT_DATA)
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
               else // Gbl.ContentReceivedByCGI == Act_CONTENT_NORM
        	  // The form contained text and was sent with content type application/x-www-form-urlencoded
		  switch (Ch)
		    {
		     case '+':	/* Change every '+' to a space */
			IsSpecialChar = true;
			LengthSpecStrSrc = 1;
			SpecialChar = 0x20;
			break;
		     case '%':	/* Change "%XX" --> "&#decimal_number;" */
			IsSpecialChar = true;
			sscanf (PtrSrc+1,"%2X",&SpecialChar);
			LengthSpecStrSrc = 3;
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
                     strcat (StrSpecialChar,
                             ThereIsSpaceChar ? "&nbsp;" :
                        	                " ");	// The first space
                     for (i = 1;
                	  i < NumSpacesTab;
                	  i++)					// Rest of spaces, except the first
                        strcat (StrSpecialChar,"&nbsp;"); // Add a space
                     NumPrintableCharsFromReturn += NumSpacesTab;
                    }
                  else
                     strcpy (StrSpecialChar,"\t");
                  ThereIsSpaceChar = true;
                  break;
               case 0x0A:  /* \n */
                  strcpy (StrSpecialChar,
                          ChangeTo == Str_TO_RIGOROUS_HTML ? "<br />" :
                        	                             Str_LF);
                  NumPrintableCharsFromReturn = 0;
                  ThereIsSpaceChar = true;
                  break;
               case 0x0D:  /* "%0D" --> "" */
                  strcpy (StrSpecialChar,
                          ChangeTo == Str_TO_RIGOROUS_HTML ? "" :
                        	                             Str_CR);
                  NumPrintableCharsFromReturn = 0;
                  ThereIsSpaceChar = true;
                  break;
               case 0x20:  /* Space */
                  strcpy (StrSpecialChar,
                          (ChangeTo == Str_TO_RIGOROUS_HTML && ThereIsSpaceChar) ? "&nbsp;" :
                        	                                                   " ");
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
                     sprintf (StrSpecialChar,"&#34;");	// Double comilla is stored as HTML code to avoid problems when displaying it
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x23:  /* "%23" --> "#" */
                  strcpy (StrSpecialChar,"#");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x26:  /* "%26" --> "&#38;" (&) */
                  strcpy (StrSpecialChar,"&");
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
        	     sprintf (StrSpecialChar,"&#39;");	// Single comilla is stored as HTML code to avoid problem when querying database (SQL code injection)
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x2C:  /* "%2C" --> "," */
                  strcpy (StrSpecialChar,",");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x2F:  /* "%2F" --> "/" */
                  strcpy (StrSpecialChar,"/");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x3A:  /* "%3A" --> ":" */
                  strcpy (StrSpecialChar,":");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x3B:  /* "%3B" --> ";" */
                  strcpy (StrSpecialChar,";");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x3C:  /* "%3C" --> "&#60;" (<) */
        	  if (ChangeTo == Str_TO_MARKDOWN)
                     strcpy (StrSpecialChar,"<");
        	  else
                     strcpy (StrSpecialChar,"&#60;"); // "<" is stored as HTML code to avoid problems when displaying it
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x3E:  /* "%3E" --> "&#62;" (>) */
        	  if (ChangeTo == Str_TO_MARKDOWN)
                     strcpy (StrSpecialChar,">");
        	  else
        	     strcpy (StrSpecialChar,"&#62;"); // ">" is stored as HTML code to avoid problems when displaying it
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x3F:  /* "%3F" --> "?" */
                  strcpy (StrSpecialChar,"?");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0x40:  /* "%40" --> "@" */
                  strcpy (StrSpecialChar,"@");
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
                     strcpy (StrSpecialChar,"&#92;"); // "\" is stored as HTML code to avoid problems when displaying it
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xC1:  /* "%C1" --> "Á" */
                  strcpy (StrSpecialChar,"Á");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xC9:  /* "%C9" --> "É" */
                  strcpy (StrSpecialChar,"É");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xCD:  /* "%CD" --> "Í" */
                  strcpy (StrSpecialChar,"Í");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xD3:  /* "%D3" --> "Ó" */
                  strcpy (StrSpecialChar,"Ó");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xDA:  /* "%DA" --> "Ú" */
                  strcpy (StrSpecialChar,"Ú");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xD1:  /* "%D1" --> "Ñ" */
                  strcpy (StrSpecialChar,"Ñ");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xC7:  /* "%C7" --> "Ç" */
                  strcpy (StrSpecialChar,"Ç");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xE1:  /* "%E1" --> "á" */
                  strcpy (StrSpecialChar,"á");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xE9:  /* "%E9" --> "é" */
                  strcpy (StrSpecialChar,"é");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xED:  /* "%ED" --> "í" */
                  strcpy (StrSpecialChar,"í");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xF3:  /* "%F3" --> "ó" */
                  strcpy (StrSpecialChar,"ó");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xFA:  /* "%FA" --> "ú" */
                  strcpy (StrSpecialChar,"ú");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xF1:  /* "%F1" --> "ñ" */
                  strcpy (StrSpecialChar,"ñ");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               case 0xE7:  /* "%E7" --> "ç" */
                  strcpy (StrSpecialChar,"ç");
                  NumPrintableCharsFromReturn++;
                  ThereIsSpaceChar = false;
                  break;
               default: /* The rest of special chars are stored as special code */
                  sprintf (StrSpecialChar,
                           (ChangeTo == Str_TO_TEXT ||
                            ChangeTo == Str_TO_MARKDOWN) ? "%c" :
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
               Lay_ShowErrorAndExit ("Space allocated to string is full.");

            /* Copy to appropiate place the special character string */
            strncpy (PtrDst,StrSpecialChar,LengthSpecStrDst);

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
               Lay_ShowErrorAndExit ("Space allocated to string is full.");

            /* Copy char from source to destination and increment pointers */
            *PtrDst++ = *PtrSrc++;

            /* Increment length of destination string */
            LengthStrDst++;
           }
        }

      /***** Copy destination string with changes to source string *****/
      strncpy (Str,StrDst,LengthStrDst);	// Str <-- StrDst
      Str[LengthStrDst] = '\0';

      /***** Free memory used for the destination string *****/
      free ((void *) StrDst);
     }

   if (RemoveLeadingAndTrailingSpaces)
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

   *(Ptr+1) = '\0';
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
   size_t NumLeadingArrobas;

   if (Str)
      if (Str[0])
	{
	 NumLeadingArrobas = strspn (Str,"@");
	 if (NumLeadingArrobas)
	    // Do not use strcpy / memcpy because the strings overlap
	    memmove (Str,&Str[NumLeadingArrobas],
		     strlen (Str) - NumLeadingArrobas + 1);
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
   char StrAux[1+1];  /* To check "!--" string */

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
   char StrAux[1+1];  /* To check "!--" string */

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
   char StrAux[3+1];  /* To check "--" string */

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
   bool SpaceFound;
   char StrAux[1+1];  // To find next "/td>" or "nbsp;"

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

   for (;;)
     {
      if ((Ch = Str_ReadCharAndSkipComments (FileSrc,Str_SKIP_HTML_COMMENTS)) == EOF)	// Set pointer to '<' if not comment, or to first character after comment if comment
	 break;

      /***** Skip directives except </td> *****/
      if (Ch == (int) '<')		// Start of directive, not a comment
	{
	 /* Check if it's </td> */
	 if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"/"))
	   {	// It's not </
	    Str_FindStrInFileBack (FileSrc,"<",Str_NO_SKIP_HTML_COMMENTS);	// Skip directive backward
	    Str_FindStrInFile (FileSrc,">",Str_NO_SKIP_HTML_COMMENTS);
	    continue;
	   }
	 if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"t"))
	   {	// It's not </t
	    Str_FindStrInFileBack (FileSrc,"<",Str_NO_SKIP_HTML_COMMENTS);	// Skip directive backward
	    Str_FindStrInFile (FileSrc,">",Str_NO_SKIP_HTML_COMMENTS);
	    continue;
	   }
	 if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),"d"))
	   {	// It's not </td
	    Str_FindStrInFileBack (FileSrc,"<",Str_NO_SKIP_HTML_COMMENTS);	// Skip directive backward
	    Str_FindStrInFile (FileSrc,">",Str_NO_SKIP_HTML_COMMENTS);
	    continue;
	   }
	 if (strcasecmp (Str_GetNextStrFromFileConvertingToLower (FileSrc,StrAux,1),">"))
	   {	// It's not </td>
	    Str_FindStrInFileBack (FileSrc,"<",Str_NO_SKIP_HTML_COMMENTS);	// Skip directive backward
	    Str_FindStrInFile (FileSrc,">",Str_NO_SKIP_HTML_COMMENTS);
	    continue;
	   }
	 break; // If it's </td>
	}

      SpaceFound = false;

      /***** Skip &nbsp; *****/
      if (Ch == (int) '&')
	{
	 /* Ver if no it's &nbsp; */
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
	  Ch == 0xA0)	// Unicode translation for &nbsp;
	  SpaceFound = true;

      if (SpaceFound)
	 Ch = (int) ' ';

      if (i < MaxLength)
	 Str[i++] = (char) Ch;
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
      if ((Ch = (int) **StrSrc) != 0)
	 (*StrSrc)++;
     }
   while (isspace (Ch) ||
	  Ch == 0xA0);	// Unicode translation for &nbsp;

   /***** Copy string while non-space characters *****/
   while (Ch &&
	  !(isspace (Ch) ||
	    Ch == 0xA0))	// Unicode translation for &nbsp;
     {
      if (i < MaxLength)
	 StrDst[i++] = (char) Ch;
      if ((Ch = (int) **StrSrc) != 0)
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
      if ((Ch = (int) **StrSrc) != 0)
	 (*StrSrc)++;
     }
   while (isspace (Ch)    ||
	  Ch == 0xA0      ||	// Unicode translation for &nbsp;
	  Ch == (int) ',' ||
	  Ch == (int) ';');

   /***** Copy string while no separator found *****/
   while (Ch &&
	  !(isspace (Ch)    ||
	    Ch == 0xA0      ||	// Unicode translation for &nbsp;
	    Ch == (int) ',' ||
	    Ch == (int) ';'))
     {
      if (i < MaxLength)
	 StrDst[i++] = (char) Ch;
      if ((Ch = (int) **StrSrc) != 0)
	 (*StrSrc)++;
     }
   StrDst[i] = '\0';
  }

/*****************************************************************************/
/********** Get from file FileSrc the next string until separator ************/
/*****************************************************************************/

void Str_GetNextStringFromFileUntilSeparator (FILE *FileSrc,char *StrDst)
  {
   int i = 0;
   int Ch;

   /***** Skip separators *****/
   do
      Ch = fgetc (FileSrc);
   while (Ch != EOF &&
	  (isspace (Ch)    ||
	   Ch == 0xA0      ||	// Unicode translation for &nbsp;
	   Ch == (int) ',' ||
	   Ch == (int) ';')); // Skip spaces, puntuación, etc.

   /***** Copy string while no separator found *****/
   while (Ch != EOF &&
	  !(isspace (Ch)    ||
	    Ch == 0xA0      ||	// Unicode translation for &nbsp;
	    Ch == (int) ',' ||
	    Ch == (int) ';'))
     {
      if (i < ID_MAX_LENGTH_USR_ID)
	 StrDst[i++] = (char) Ch;
      Ch = fgetc (FileSrc);
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
	  Ch == 0xA0   ||	// Unicode translation for &nbsp;
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
   *(Ptr+1) = '\0';
  }

/*****************************************************************************/
/********** Replace special characters in a string for HTML codes ************/
/*****************************************************************************/

void Str_ReplaceSpecialCharByCodes (char *Str,unsigned long MaxLengthStr)
  {
   char *Ptr, *PtrSrc, *PtrDst, *PtrEndStr;
   int LengthSpecStrDst;
   unsigned long LengthStrDst = 0;
   char StrSpecialChar[256];

   /***** Make the conversion *****/
   for (Ptr = Str;
	*Ptr;)
     {
      if (*Ptr >= 32 && *Ptr <= 126)	// If character is printable in english ==> is OK; else ==> convert to code
        {
         LengthStrDst++;
         Ptr++;
        }
      else
        {
         sprintf (StrSpecialChar,"&#%u;",(unsigned char) *Ptr);
         PtrEndStr = Str + strlen (Str);
         LengthSpecStrDst = strlen (StrSpecialChar);
         LengthStrDst += LengthSpecStrDst;
         if (LengthStrDst > MaxLengthStr)
            Lay_ShowErrorAndExit ("Not enough memory to convert the format of a string.");
         for (PtrSrc = PtrEndStr,
	      PtrDst = PtrSrc + LengthSpecStrDst - 1;
	      PtrSrc >= Ptr + 1;
              *PtrDst-- = *PtrSrc--);	/* Copy backward from '\0' (included) */
         /* Copy to appropiate place the special character string */
         strncpy (Ptr,StrSpecialChar,LengthSpecStrDst);
	 /* Increment pointer to character after ';' */
 	 Ptr += LengthSpecStrDst;
        }
     }
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
      Lay_ShowErrorAndExit ("Path is too long.");
  }

/*****************************************************************************/
/* Convert string with a code (of group type, group, degree, etc.) to long **/
/*****************************************************************************/
// Return -1L if code not found in Str

long Str_ConvertStrCodToLongCod (const char *Str)
  {
   long Code;

   if (Str[0] == '\0')
      return -1L;

   if (sscanf (Str,"%ld",&Code) != 1)
      return -1L;

   return Code;
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
// PathWithoutFileName must have space for at least PATH_MAX+1 bytes
// FileName must have space for at least NAME_MAX+1 bytes

void Str_SplitFullPathIntoPathAndFileName (const char *FullPath,
                                           char *PathWithoutFileName,
                                           char *FileName)
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
   LengthUntilFileName = (size_t) (PtrFileName - FullPath); // Last slash included
   if (LengthUntilFileName > 1)
     {
      if (LengthUntilFileName > PATH_MAX)
	{
	 strncpy (PathWithoutFileName,FullPath,PATH_MAX);
	 PathWithoutFileName[PATH_MAX] = '\0';
	}
      else
	{
         strncpy (PathWithoutFileName,FullPath,LengthUntilFileName - 1);	// Do not copy ending slash
         PathWithoutFileName[LengthUntilFileName - 1] = '\0';
	}
     }
   else
      PathWithoutFileName[0] = '\0';

   /***** Get FileName *****/
   strncpy (FileName,PtrFileName,NAME_MAX);
   FileName[NAME_MAX] = '\0';
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
   if (LengthExtension < Fil_MIN_LENGTH_FILE_EXTENSION ||
       LengthExtension > Fil_MAX_LENGTH_FILE_EXTENSION)
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
   return (bool) (*Path1 == '\0' || *Path1 == '/');
  }

/*****************************************************************************/
/** Skip spaces in a file seeking it before of reading the first non-blank ***/
/*****************************************************************************/

void Str_SkipSpacesInFile (FILE *FileSrc)
  {
   int Ch;

   while ((Ch = fgetc (FileSrc)) != EOF)
      if (!(isspace (Ch) ||
	    Ch == 0xA0))	// Unicode translation for &nbsp;
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
         if (*(Str+1) == 'N' || *(Str+1) == 'n')
            if (*(Str+2) == 'B' || *(Str+2) == 'b')
               if (*(Str+3) == 'S' || *(Str+3) == 's')
                  if (*(Str+4) == 'P' || *(Str+4) == 'p')
                     if (*(Str+5) == ';')
                       {
                        fputc ((int) ' ',FileTgt);
                        Str += 6;
                        continue;
                       }
        }
      /* Is <br> or <br />? */
      else if (*Str == '<')
        {
         if (*(Str+1) == 'B' || *(Str+1) == 'b')
            if (*(Str+2) == 'R' || *(Str+2) == 'r')
              {
               if (*(Str+3) == '>')
                 {
                  fputc ((int) '\n',FileTgt);
                  Str += 4;
                  continue;
                 }
               else if (*(Str+3) == ' ')
                 {
                  if (*(Str+4) == '/')
                     if (*(Str+5) == '>')
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

#define MAX_LENGTH_BOUNDARY_STR 100

int Str_ReadFileUntilBoundaryStr (FILE *FileSrc,char *StrDst,
                                  const char *BoundaryStr,
                                  unsigned LengthBoundaryStr,
                                  unsigned long long MaxLength)
  {
   unsigned NumBytesIdentical;			// Number of characters identical in each iteration of the loop
   unsigned NumBytesReadButNotDiscarded;	// Number of characters read from the source file...
						// ...and not fully discarded in search
   int Buffer[MAX_LENGTH_BOUNDARY_STR+1];
   unsigned StartIndex;
   unsigned i;
   char *Ptr; // Pointer used to go through StrDst writing characters
   unsigned long long LengthDst;

   /***** Checkings on boundary string *****/
   if (!LengthBoundaryStr)
     {
      if (StrDst != NULL)
	 *StrDst = '\0';
      return 1;
     }
   if (LengthBoundaryStr > MAX_LENGTH_BOUNDARY_STR)
      Lay_ShowErrorAndExit ("Delimiter string too large.");
   Ptr = StrDst;

   StartIndex = 0;
   NumBytesReadButNotDiscarded = 0;
   LengthDst = 0;

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

      if (LengthDst == MaxLength)
	{
	 if (StrDst != NULL)
	    *Ptr = '\0';
	 return 0;
	}

      if (StrDst != NULL)
	 *Ptr++ = (char) Buffer[StartIndex];

      StartIndex = (StartIndex + 1) % LengthBoundaryStr;
      NumBytesReadButNotDiscarded--;
      LengthDst++;
     }

   return 0;	// Not reached
  }

/*****************************************************************************/
/****** Convert invalid characters in a file name to valid characters ********/
/*****************************************************************************/
// Return true if the name of the file o folder is valid
// If the name is not valid, Gbl.Message will contain feedback text
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
         sprintf (Gbl.Message,
                  Gbl.FileBrowser.UploadingWithDropzone ? Txt_UPLOAD_FILE_X_invalid_name_NO_HTML :
                	                                  Txt_UPLOAD_FILE_X_invalid_name,
		  FileName);
     }
   else	// FileName is empty
      strcpy (Gbl.Message,
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
/********** Write a quantity of bytes as bytes, KiB, MiB, GiB or TiB *********/
/*****************************************************************************/

#define Ki 1024.0
#define Mi 1048576.0
#define Gi 1073741824.0
#define Ti 1099511627776.0

void Str_WriteSizeInBytesBrief (double SizeInBytes)
  {
   if (SizeInBytes < Ki)
      fprintf (Gbl.F.Out,"%.0f&nbsp;B",
               SizeInBytes);
   else if (SizeInBytes < Mi)
      fprintf (Gbl.F.Out,"%.0f&nbsp;KiB",
               SizeInBytes / Ki);
   else if (SizeInBytes < Gi)
      fprintf (Gbl.F.Out,"%.0f&nbsp;MiB",
               SizeInBytes / Mi);
   else if (SizeInBytes < Ti)
      fprintf (Gbl.F.Out,"%.0f&nbsp;GiB",
               SizeInBytes / Gi);
   else
      fprintf (Gbl.F.Out,"%.0f&nbsp;TiB",
               SizeInBytes / Ti);
  }

void Str_WriteSizeInBytesFull (double SizeInBytes)
  {
   if (SizeInBytes < Ki)
      fprintf (Gbl.F.Out,"%.0f&nbsp;B",
               SizeInBytes);
   else if (SizeInBytes < Mi)
      fprintf (Gbl.F.Out,"%.1f&nbsp;KiB",
               SizeInBytes / Ki);
   else if (SizeInBytes < Gi)
      fprintf (Gbl.F.Out,"%.1f&nbsp;MiB",
               SizeInBytes / Mi);
   else if (SizeInBytes < Ti)
      fprintf (Gbl.F.Out,"%.1f&nbsp;GiB",
               SizeInBytes / Gi);
   else
      fprintf (Gbl.F.Out,"%.1f&nbsp;TiB",
               SizeInBytes / Ti);
  }

/*****************************************************************************/
/******************* Create a random alphanumeric string *********************/
/*****************************************************************************/

#define NUM_ALPHANUM_CHARS (10+26+26)

void Str_CreateRandomAlphanumStr (char *Str,size_t Length)
  {
   static const char CharTable[NUM_ALPHANUM_CHARS] =
     {'0','1','2','3','4','5','6','7','8','9',
      'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
      'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'
     };
   size_t i;

   /***** Set random chars in string *****/
   for (i = 0;
	i <Length;
	i++)
      Str[i] = CharTable[(unsigned) (((float) rand () * (float) (NUM_ALPHANUM_CHARS-1)) / (float) RAND_MAX + 0.5)];
   Str[Length] = '\0';
  }

/*****************************************************************************/
/********************** Get mailbox from e-mail address **********************/
/*****************************************************************************/

void Str_GetMailBox (const char *Email,char *MailBox,size_t MaxLength)
  {
   const char *Ptr;

   MailBox[0] = '\0';	// Return empty mailbox on error

   if ((Ptr = strchr (Email,(int) '@')))	// Find first '@' in address
      if (Ptr != Email)				// '@' is not the first character in Email
        {
         Ptr++;					// Skip '@'
         if (strchr (Ptr,(int) '@') == NULL)	// No more '@' found
           {
            strncpy (MailBox,Ptr,MaxLength);
            MailBox[MaxLength] = '\0';
           }
        }
  }
