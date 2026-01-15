// swad_autolink.c: inserting automatic links in text

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

#define _GNU_SOURCE 		// For asprintf
#include <ctype.h>		// For tolower
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For malloc and free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_autolink.h"
#include "swad_config.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_nickname.h"
#include "swad_nickname_database.h"
#include "swad_photo.h"
#include "swad_string.h"
#include "swad_user.h"
#include "swad_www.h"

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

typedef enum
  {
   ALn_LINK_UNKNOWN = 0,
   ALn_LINK_URL     = 1,
   ALn_LINK_NICK    = 2,
  } ALn_LinkType_t;

struct ALn_Substring
  {
   char *Str;	// Pointer to the first char of substring
   size_t Len;	// Length of the substring
  };

struct ALn_Link
  {
   ALn_LinkType_t Type;			// URL or nickname?
   struct ALn_Substring URLorNick;	// Link text
   struct ALn_Substring NickAnchor[3];	// Pointer to anchors if nick
   size_t LengthAddedUpToHere;	// Total length of extra HTML code
				// added up to this link (included)
   struct ALn_Link *Prev;	// Pointer to previous link
   struct ALn_Link *Next;	// Pointer to next link
  };

/*****************************************************************************/
/******************** Global variables from other modules ********************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*************************** Private prototypes ******************************/
/*****************************************************************************/

static void ALn_CreateFirstLink (struct ALn_Link **Link,
                                 struct ALn_Link **LastLink);
static void ALn_CreateNextLink (struct ALn_Link **Link,
                                struct ALn_Link **LastLink);
static void ALn_FreeLinks (struct ALn_Link *LastLink);
static ALn_LinkType_t ALn_CheckURL (char **PtrSrc,char PrevCh,
				    struct ALn_Link **Link,
				    struct ALn_Link **LastLink,
				    size_t MaxCharsURLOnScreen);
static ALn_LinkType_t ALn_CheckNickname (char **PtrSrc,char PrevCh,
                                         struct ALn_Link **Link,
                                         struct ALn_Link **LastLink);
static void ALn_CopySubstring (const struct ALn_Substring *PtrSrc,char **PtrDst);

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

// For URLs the length of anchor is fixed, so it can be calculated once
#define ALn_URL_ANCHOR_0		"<a href=\""
#define ALn_URL_ANCHOR_1		"\" target=\"_blank\">"
#define ALn_URL_ANCHOR_2		"</a>"

#define ALn_URL_ANCHOR_0_LENGTH		(sizeof (ALn_URL_ANCHOR_0) - 1)
#define ALn_URL_ANCHOR_1_LENGTH		(sizeof (ALn_URL_ANCHOR_1) - 1)
#define ALn_URL_ANCHOR_2_LENGTH		(sizeof (ALn_URL_ANCHOR_2) - 1)

#define ALn_URL_ANCHOR_TOTAL_LENGTH	(ALn_URL_ANCHOR_0_LENGTH + ALn_URL_ANCHOR_1_LENGTH + ALn_URL_ANCHOR_2_LENGTH)

static struct ALn_Substring URLAnchor[3] =
  {
   [0] = {
	  .Str = ALn_URL_ANCHOR_0,
	  .Len = ALn_URL_ANCHOR_0_LENGTH,
	  },
   [1] = {
	  .Str = ALn_URL_ANCHOR_1,
	  .Len = ALn_URL_ANCHOR_1_LENGTH,
	  },
   [2] = {
	  .Str = ALn_URL_ANCHOR_2,
	  .Len = ALn_URL_ANCHOR_2_LENGTH,
	  },
  };

/*****************************************************************************/
/************** Insert automatic links in every URL or nickname **************/
/*****************************************************************************/
/*
Insertion example:
The web site of @rms is https://stallman.org/
The web site of <a href="https://openswad.org/?usr=@rms">@rms</a> is <a href="https://stallman.org/" target="_blank">https://stallman.org/</a>
*/

/*

<form action="https://localhost/swad/es" method="post">
<input type="hidden" name="ses" value="2jb9CGhIJ81_qhDyeQ6MWDFKQ5ZaA_F68tq22ZAjYww">
<input type="hidden" name="usr" value="@acanas">
<button type="submit" class="NICK_WHITE">
@acanas
</button>
</form>

*/
/*
    ______          ______          ______          ______
   |______|<--  -->|______|<--  -->|______|<--  -->|______|<--- LastLink
   |______|   \/   |______|   \/   |______|   \/   |______|
   |______|   /\   |______|   /\   |______|   /\   |______|
   |_NULL_|  /  ---|_Prev_|  /  ---|_Prev_|  /  ---|_Prev_|
   |_Next_|--      |_Next_|--      |_Next_|--      |_NULL_|

1 Move forward the text after the link
2 Copy the 3rd part of the anchor
3 Move forward the link
4 Copy the 2nd part of the anchor
5 Copy the link into the anchor
6 Copy the 1st part of the anchor

Hi @admin, can I use https://openswad.org for free?
   _______________________________
  |H|i|_|@|a|d|m|i|n|,|_|c|a|n|...|
   | | | | | | | | | | | | | | |
   | | | | | | | | | \_\_\_\_\_\______________________________________1____
   | | | \ \ \ \ \ \                                             \ \ \ \ \ \
   | | |  \_\_\_\_\_\_________________________3____              | | | | | |
   | | |   \ \ \ \ \ \                   \ \ \ \ \ \             | | | | | |
   | | |    \_\_\_\_\_\_5____            | | | | | |             | | | | | |
   | | |           \ \ \ \ \ \           | | | | | |             | | | | | |
   | | |      6    | | | | | |     4     | | | | | |      2      | | | | | |
   v v v  anchor#1 v v v v v v anchor#2  v v v v v v  anchor#3   v v v v v v
   ___________________________________________________________________________
  |H|i|_|<|_|_|_|_|@|a|d|m|i|n|_|_|_|_|>|@|a|d|m|i|n|<|_|_|_|_|>|,|_|c|a|n|...|
*/
void ALn_InsertLinks (char *Txt,unsigned long MaxLength,size_t MaxCharsURLOnScreen)
  {
   size_t TxtLength;
   char PrevCh = '\0';
   char *PtrSrc;
   char *PtrDst;
   struct ALn_Link *Link;
   struct ALn_Link *LastLink;
   size_t Length;
   size_t i;
   struct ALn_Substring Limited;	// URL displayed on screen (may be shorter than actual length)
   const struct ALn_Substring *Anchor[3];

   /**************************************************************/
   /***** Find starts and ends of links (URLs and nicknames) *****/
   /**************************************************************/
   ALn_CreateFirstLink (&Link,&LastLink);

   for (PtrSrc = Txt;
	*PtrSrc;)
      /* Check if the next char is the start of a URL */
      if ((Link->Type = ALn_CheckURL (&PtrSrc,PrevCh,
                                      &Link,&LastLink,
                                      MaxCharsURLOnScreen)) == ALn_LINK_UNKNOWN)
	 /* Check if the next char is the start of a nickname */
	 if ((Link->Type = ALn_CheckNickname (&PtrSrc,PrevCh,
	                                      &Link,&LastLink)) == ALn_LINK_UNKNOWN)
	    /* The next char is not the start of a URL or a nickname */
	    if (*PtrSrc)	// If the end has not been reached
	      {
	       PrevCh = *PtrSrc;
	       PtrSrc++;
	      }

   /**********************************************************************/
   /***** If there are one or more links (URLs or nicknames) in text *****/
   /**********************************************************************/
   if (LastLink)	// Not null ==> one or more links found
     {
      /***** Insert links from end to start of text,
             only if there is enough space available in text *****/
      TxtLength = strlen (Txt);
      if (TxtLength + LastLink->LengthAddedUpToHere <= MaxLength)
	{
         for (Link = LastLink;
              Link;
              Link = Link->Prev)
           {
            /***** Set anchors *****/
            switch (Link->Type)
              {
               case ALn_LINK_URL:
                  Anchor[0] = &URLAnchor[0];
                  Anchor[1] = &URLAnchor[1];
                  Anchor[2] = &URLAnchor[2];
		  break;
               case ALn_LINK_NICK:
                  Anchor[0] = &Link->NickAnchor[0];
                  Anchor[1] = &Link->NickAnchor[1];
                  Anchor[2] = &Link->NickAnchor[2];
		  break;
               default:
        	  continue;
              }

            /***** Step 1: Move forward the text after the link (URL or nickname)
                           (it's mandatory to do the copy in reverse order
                            to avoid overwriting source) *****/

	    PtrSrc = Link == LastLink ? Txt + TxtLength :
					Link->Next->URLorNick.Str - 1;
	    PtrDst = PtrSrc + Link->LengthAddedUpToHere;
	    Length = PtrSrc - (Link->URLorNick.Str + Link->URLorNick.Len - 1);
            for (i = 0;
                 i < Length;
                 i++)
               *PtrDst-- = *PtrSrc--;

            /***** Step 2: Copy the third part of the anchor *****/
            ALn_CopySubstring (Anchor[2],&PtrDst);

            /***** Step 3: Move forward the link (URL or nickname)
                           to be shown on screen *****/
            switch (Link->Type)
              {
               case ALn_LINK_URL:
            	  if (Link->URLorNick.Len <= MaxCharsURLOnScreen)
	    	     ALn_CopySubstring (&Link->URLorNick,&PtrDst);
            	  else
              	    {
	    	     /* Limit the length of URL */
	    	     if ((Limited.Str = malloc (Link->URLorNick.Len + 1)) == NULL)
	     		Err_NotEnoughMemoryExit ();
	     	     strncpy (Limited.Str,Link->URLorNick.Str,Link->URLorNick.Len);
	     	     Limited.Str[Link->URLorNick.Len] = '\0';
	    	     Limited.Len = Str_LimitLengthHTMLStr (Limited.Str,MaxCharsURLOnScreen);
	     	     ALn_CopySubstring (&Limited,&PtrDst);
	    	     free (Limited.Str);
            	    }
	    	  break;
               case ALn_LINK_NICK:
                  ALn_CopySubstring (&Link->URLorNick,&PtrDst);
	    	  break;
               default:
            	  break;
              }

            /***** Step 4: Copy the second part of the anchor *****/
            ALn_CopySubstring (Anchor[1],&PtrDst);

            /***** Step 5: Copy the link into the anchor
                           (it's mandatory to do the copy in reverse order
                           to avoid overwriting source URL or nickname) *****/
            ALn_CopySubstring (&Link->URLorNick,&PtrDst);

            /***** Step 6: Copy the first part of the anchor *****/
            ALn_CopySubstring (Anchor[0],&PtrDst);
           }
	}
     }

   /***********************************/
   /***** Free memory for anchors *****/
   /***********************************/
   ALn_FreeLinks (LastLink);
  }

/***************************** Create first link ******************************/

static void ALn_CreateFirstLink (struct ALn_Link **Link,
                                 struct ALn_Link **LastLink)
  {
   /***** Reset last link pointer *****/
   (*LastLink) = NULL;

   /***** Allocate current link *****/
   if (((*Link) = malloc (sizeof (struct ALn_Link))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Initialize current link *****/
   (*Link)->Prev = NULL;
   (*Link)->Next = NULL;
  }

/***************************** Create next link ******************************/

static void ALn_CreateNextLink (struct ALn_Link **Link,
                                struct ALn_Link **LastLink)
  {
   /***** Current link now is pointing to a correct link,
          so set last link pointer to current link *****/
   (*LastLink) = (*Link);

   /***** Allocate next link *****/
   if (((*Link)->Next = malloc (sizeof (struct ALn_Link))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Initialize next link *****/
   (*Link)->Next->Prev = *Link;
   (*Link)->Next->Next = NULL;

   /***** Change current link to just allocated link *****/
   (*Link) = (*Link)->Next;
  }

/***************************** Free found links ******************************/

static void ALn_FreeLinks (struct ALn_Link *LastLink)
  {
   struct ALn_Link *Link;
   struct ALn_Link *PrevLink;

   for (Link = LastLink;
	Link;
	Link = PrevLink)
     {
      PrevLink = Link->Prev;
      if (Link->Type == ALn_LINK_NICK)
	{
	 if (Link->NickAnchor[2].Str) free (Link->NickAnchor[2].Str);
	 if (Link->NickAnchor[1].Str) free (Link->NickAnchor[1].Str);
	 if (Link->NickAnchor[0].Str) free (Link->NickAnchor[0].Str);
	}
      free (Link);
     }
  }

/**************************** Check if a URL found ***************************/

static ALn_LinkType_t ALn_CheckURL (char **PtrSrc,char PrevCh,
				    struct ALn_Link **Link,
				    struct ALn_Link **LastLink,
				    size_t MaxCharsURLOnScreen)
  {
   unsigned char Ch;
   size_t NumChars1;
   size_t NumChars2;
   char *PtrEnd;	// Pointer to the last char of URL/nickname in original text
   char *Limited;	// URL displayed on screen (may be shorter than actual length)
   ALn_LinkType_t Type = ALn_LINK_UNKNOWN;

   /***** Check if the next char is the start of a URL *****/
   if (tolower ((int) *(*PtrSrc)) == (int) 'h')
      if (!Str_ChIsAlphaNum (PrevCh))
	{
	 (*Link)->URLorNick.Str = (*PtrSrc);
	 if (tolower ((int) *++(*PtrSrc)) == (int) 't') // ht...
	   {
	    if (tolower ((int) *++(*PtrSrc)) == (int) 't') // htt...
	      {
	       if (tolower ((int) *++(*PtrSrc)) == (int) 'p') // http...
		 {
		  (*PtrSrc)++;
		  if (*(*PtrSrc) == ':') // http:...
		    {
		     if (*++(*PtrSrc) ==  '/') // http:/...
			if (*++(*PtrSrc) == '/') // http://...
			   Type = ALn_LINK_URL;
		    }
		  else if (tolower ((int) *(*PtrSrc)) == (int) 's') // https...
		    {
		     if (*++(*PtrSrc) == ':') // https:...
		       {
			if (*++(*PtrSrc) == '/') // https:/...
			   if (*++(*PtrSrc) == '/') // https://...
			      Type = ALn_LINK_URL;
		       }
		    }
		 }
	      }
	   }
	 if (Type == ALn_LINK_URL)
	   {
	    /***** Find URL end *****/
	    (*PtrSrc)++;	// Points to first character after http:// or https://
	    for (;;)
	      {
	       NumChars1 = Str_GetNextASCIICharFromStr ((*PtrSrc),&Ch);
	       (*PtrSrc) += NumChars1;
	       if (Ch <= 32 || Ch == '<'  || Ch == '"')
		 {
		  PtrEnd = (*PtrSrc) - NumChars1 - 1;
		  break;
		 }
	       else if (Ch == ',' || Ch == '.' || Ch == ';' || Ch == ':' ||
			Ch == ')' || Ch == ']' || Ch == '}')
		 {
		  NumChars2 = Str_GetNextASCIICharFromStr ((*PtrSrc),&Ch);
		  (*PtrSrc) += NumChars2;
		  if (Ch <= 32 || Ch == '<' || Ch == '"')
		    {
		     PtrEnd = (*PtrSrc) - NumChars2 - NumChars1 - 1;
		     break;
		    }
		 }
	      }

	    /***** Compute number of bytes added until here *****/
	    (*Link)->LengthAddedUpToHere  = (*Link)->Prev ? (*Link)->Prev->LengthAddedUpToHere  :
							    0;
	    (*Link)->URLorNick.Len = (size_t) (PtrEnd + 1 - (*Link)->URLorNick.Str);
	    if ((*Link)->URLorNick.Len <= MaxCharsURLOnScreen)
	       (*Link)->LengthAddedUpToHere  += ALn_URL_ANCHOR_TOTAL_LENGTH +
						(*Link)->URLorNick.Len;
	    else	// If URL is too long to be displayed ==> short it
	      {
	       if ((Limited = malloc ((*Link)->URLorNick.Len + 1)) == NULL)
		  Err_NotEnoughMemoryExit ();
	       strncpy (Limited,(*Link)->URLorNick.Str,(*Link)->URLorNick.Len);
	       Limited[(*Link)->URLorNick.Len] = '\0';
	       (*Link)->LengthAddedUpToHere  += ALn_URL_ANCHOR_TOTAL_LENGTH +
						Str_LimitLengthHTMLStr (Limited,MaxCharsURLOnScreen);
	       free (Limited);
	      }

	    /***** Create next link *****/
	    ALn_CreateNextLink (Link,LastLink);
	   }
	}

   return Type;
  }

/************************* Check if a nickname found *************************/

static ALn_LinkType_t ALn_CheckNickname (char **PtrSrc,char PrevCh,
                                         struct ALn_Link **Link,
                                         struct ALn_Link **LastLink)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC12x16",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE12x16",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO12x16",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR12x16",
     };
   char Ch;
   size_t Length;
   char ParsStr[Frm_MAX_BYTES_PARAMS_STR + 1];
   struct Usr_Data UsrDat;
   Pho_ShowPhotos_t ShowPhotos = Pho_PHOTOS_DONT_SHOW;
   char PhotoURL[WWW_MAX_BYTES_WWW + 1];
   char *CaptionStr;
   char *ImgStr;
   char NickWithoutArr[Nck_MAX_BYTES_NICK_WITHOUT_ARROBA + 1];
   ALn_LinkType_t Type = ALn_LINK_UNKNOWN;

   /***** Check if the next char is the start of a nickname *****/
   Ch = *(*PtrSrc);
   if (Ch == '@')					// Current is @
      if (!Str_ChIsAlphaNum (PrevCh))			// Previous is not alphanumeric
         if (Str_ChIsAlphaNum (*((*PtrSrc) + 1)))	// Next is alphanumeric
	   {
	    (*Link)->URLorNick.Str = (*PtrSrc);

	    /***** Find nickname end *****/
	    (*PtrSrc)++;	// Points to first character after @

	    /***** A nick can have digits, letters and '_'  *****/
	    for (;
		 *(*PtrSrc);
		 (*PtrSrc)++)
	      {
	       Ch = *(*PtrSrc);
	       if (!Str_ChIsAlphaNum (Ch))
		  break;
	      }

	    /***** Calculate length of this nickname *****/
	    (*Link)->URLorNick.Len = (size_t) ((*PtrSrc) - (*Link)->URLorNick.Str);

	    /***** A nick (without arroba) must have a number of characters
		   Nck_MIN_CHARS_NICK_WITHOUT_ARROBA <= Length <= Nck_MAX_CHARS_NICK_WITHOUT_ARROBA *****/
	    Length = (*Link)->URLorNick.Len - 1;	// Do not count the initial @
	    if (Length >= Nck_MIN_CHARS_NICK_WITHOUT_ARROBA &&
		Length <= Nck_MAX_CHARS_NICK_WITHOUT_ARROBA)	// Nick seems valid
	      {
	       /***** Create data for a user *****/
	       Usr_UsrDataConstructor (&UsrDat);

	          /***** Get user's code using nickname *****/
	          strncpy (NickWithoutArr,(*Link)->URLorNick.Str + 1,Length);
		  NickWithoutArr[Length] = '\0';
		  if ((UsrDat.UsrCod = Nck_DB_GetUsrCodFromNickname (NickWithoutArr)) > 0)
		    {
		     Type = ALn_LINK_NICK;
		     Usr_GetUsrDataFromUsrCod (&UsrDat,
					       Usr_DONT_GET_PREFS,
					       Usr_DONT_GET_ROLE_IN_CRS);
		    }

		  if (Type == ALn_LINK_NICK)
		    {
		     /***** Reset anchors (checked on freeing) *****/
		     (*Link)->NickAnchor[0].Str =
		     (*Link)->NickAnchor[1].Str =
		     (*Link)->NickAnchor[2].Str = NULL;

		     /***** Store first part of anchor *****/
		     Frm_SetParsForm (ParsStr,ActSeeOthPubPrf,
				      Frm_PUT_PAR_LOCATION_IF_NO_SESSION);
		     if (asprintf (&(*Link)->NickAnchor[0].Str,
				   "<form method=\"post\" action=\"%s/%s\">"
				   "%s"	// Parameters
				   "<input type=\"hidden\" name=\"usr\" value=\"",
				   Cfg_URL_SWAD_CGI,
				   Lan_STR_LANG_ID[Gbl.Prefs.Language],
				   ParsStr) < 0)
			Err_NotEnoughMemoryExit ();
		     (*Link)->NickAnchor[0].Len = strlen ((*Link)->NickAnchor[0].Str);

		     /***** Store second part of anchor *****/
		     if (asprintf (&(*Link)->NickAnchor[1].Str,
				   "\">"
		                   "<button type=\"submit\" class=\"NICK_%s\">",
		                   The_GetSuffix ()) < 0)
			Err_NotEnoughMemoryExit ();
		     (*Link)->NickAnchor[1].Len = strlen ((*Link)->NickAnchor[1].Str);

		     /***** Store third part of anchor *****/
		     ShowPhotos = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);
		     Pho_BuildHTMLUsrPhoto (&UsrDat,
					    ShowPhotos == Pho_PHOTOS_SHOW ? PhotoURL :
									    NULL,
					    ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM,
					    &CaptionStr,
					    &ImgStr);
		     if (asprintf (&(*Link)->NickAnchor[2].Str,
				   "</button>"
				   "</form>"
				   "%s"
				   "%s",
				   CaptionStr,
				   ImgStr) < 0)
			Err_NotEnoughMemoryExit ();
		     free (ImgStr);
		     free (CaptionStr);
		     (*Link)->NickAnchor[2].Len = strlen ((*Link)->NickAnchor[2].Str);

		     /***** Compute number of bytes added until here *****/
		     (*Link)->LengthAddedUpToHere  = (*Link)->Prev ? (*Link)->Prev->LengthAddedUpToHere  :
								     0;
		     (*Link)->LengthAddedUpToHere  += (*Link)->NickAnchor[0].Len +
						      (*Link)->URLorNick.Len +
						      (*Link)->NickAnchor[1].Len +
						      (*Link)->NickAnchor[2].Len;

		     /***** Create next link *****/
		     ALn_CreateNextLink (Link,LastLink);
		    }

	       /***** Free memory used for user's data *****/
	       Usr_UsrDataDestructor (&UsrDat);
	      }
	   }

   return Type;
  }

/************** Copy source substring to destination, backwards **************/

static void ALn_CopySubstring (const struct ALn_Substring *Src,char **Dst)
  {
   char *PtrSrc;	// Local pointer optimizes access to memory
   char *PtrDst;	// Local pointer optimizes access to memory
   size_t Len = Src->Len;
   /*
   Example: Src->Str = "<a href=\""
            Src->Len = 9
   Src->Str
      |
     _v_________________
     |<|a|_|h|r|e|f|=|"|
      | | | | | | | | |
       \ \ \ \ \ \ \ \ \
        \ \ \ \ \ \ \ \ \
         \ \ \ \ \ \ \ \ \
          \ \ \ \ \ \ \ \ \
           \ \ \ \ \ \ \ \ \
            | | | | | | | | |
     _______v_v_v_v_v_v_v_v_v_____________
     |_|_|_|<|a|_|h|r|e|f|=|"|_|_|_|_|_|_|
          ^                 ^
          |                 |
       PtrDst            (*Dst)
   The copy has to be done backwards to avoid overwriting the original string
   */
   if (Len)
     {
      PtrSrc = Src->Str + Len - 1;
      PtrDst = *Dst;	// Make a local copy of destination pointer
      for (;
	   Len;
	   Len--)
	 *PtrDst-- = *PtrSrc--;
      *Dst = PtrDst;	// Update destination pointer
     }
  }
