// swad_parameter.c: CGI parameters

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <ctype.h>		// For isprint, isspace, etc.
#include <stddef.h>		// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_config.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_password.h"
#include "swad_session.h"
#include "swad_setting.h"
#include "swad_tab.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

const char *Par_SEPARATOR_PARAM_MULTIPLE = "\x0a";	// Must be 1 <= character <= 31

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

#define Par_MAX_BYTES_BOUNDARY_WITHOUT_CR_LF	(128 - 1)
#define Par_MAX_BYTES_BOUNDARY_WITH_CR_LF	(2 + Par_MAX_BYTES_BOUNDARY_WITHOUT_CR_LF)

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static struct
  {
   /* Content send by the form and received by the CGI:
      Act_CONTENT_NORM (if CONTENT_TYPE==text/plain)
      Act_CONT_DATA    (if CONTENT_TYPE==multipart/form-data) */
   Act_Content_t ContentReceivedByCGI;
   size_t ContentLength;
   char *QueryString;	// String allocated dynamically with the arguments sent to the CGI
   struct Par_Param *List;	// Linked list of parameters
   Par_Method_t Method;
   bool GetMethod;	// Am I accessing using GET method?
   struct
     {
      char StrWithoutCRLF[Par_MAX_BYTES_BOUNDARY_WITHOUT_CR_LF + 1];
      char StrWithCRLF   [Par_MAX_BYTES_BOUNDARY_WITH_CR_LF    + 1];
      size_t LengthWithoutCRLF;
      size_t LengthWithCRLF;
     } Boundary;
   char IP[Cns_MAX_BYTES_IP + 1];
  } Par_Pars =
  {
   .ContentReceivedByCGI = Act_CONT_NORM,
   .ContentLength = 0,
   .QueryString = NULL,
   .List = NULL,
   .Method = Par_METHOD_POST,
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static inline void Par_SetContentReceivedByCGI (Act_Content_t ContentReceivedByCGI);

static void Par_GetBoundary (void);

static void Par_CreateListOfParsFromQueryString (void);
static void Par_CreateListOfParsFromTmpFile (void);
static int Par_ReadTmpFileUntilQuote (FILE *QueryFile);
static int Par_ReadTmpFileUntilReturn (FILE *QueryFile);

static bool Par_CheckIsParCanBeUsedInGETMethod (const char *ParName);

/*****************************************************************************/
/********************** Type of content received by CGI **********************/
/*****************************************************************************/

static inline void Par_SetContentReceivedByCGI (Act_Content_t ContentReceivedByCGI)
  {
   Par_Pars.ContentReceivedByCGI = ContentReceivedByCGI;
  }

Act_Content_t Par_GetContentReceivedByCGI (void)
  {
   return Par_Pars.ContentReceivedByCGI;
  }

/*****************************************************************************/
/*** Read all parameters passed to this CGI and store for later processing ***/
/*****************************************************************************/

#define Par_MAX_BYTES_METHOD		(128 - 1)
#define Par_MAX_BYTES_CONTENT_TYPE	(128 - 1)

bool Par_GetQueryString (void)
  {
   char Method[Par_MAX_BYTES_METHOD + 1];
   char ContentType[Par_MAX_BYTES_CONTENT_TYPE + 1];
   char UnsignedLongStr[Cns_MAX_DECIMAL_DIGITS_ULONG + 1];
   unsigned long UnsignedLong;

   Str_Copy (Method,getenv ("REQUEST_METHOD"),sizeof (Method) - 1);

   if (!strcmp (Method,"GET"))
     {
      /***** GET method *****/
      Par_Pars.Method = Par_METHOD_GET;
      Par_SetContentReceivedByCGI (Act_CONT_NORM);

      /* Get content length */
      Par_Pars.ContentLength = strlen (getenv ("QUERY_STRING"));

      /* Allocate memory for query string */
      if ((Par_Pars.QueryString = malloc (Par_Pars.ContentLength + 1)) == NULL)
	 return false;

      /* Copy query string from environment variable */
      Str_Copy (Par_Pars.QueryString,getenv ("QUERY_STRING"),
                Par_Pars.ContentLength);
     }
   else
     {
      /***** POST method *****/
      Par_Pars.Method = Par_METHOD_POST;

      /* Get content length */
      if (getenv ("CONTENT_LENGTH"))
	{
         Str_Copy (UnsignedLongStr,getenv ("CONTENT_LENGTH"),
                   sizeof (UnsignedLongStr) - 1);
         if (sscanf (UnsignedLongStr,"%lu",&UnsignedLong) != 1)
            return false;
         Par_Pars.ContentLength = (size_t) UnsignedLong;
	}
      else
         return false;

      /* If data are received ==> the environment variable CONTENT_TYPE will hold:
      multipart/form-data; boundary=---------------------------7d13ca2e948
      */
      if (getenv ("CONTENT_TYPE") == NULL)
         return false;

      Str_Copy (ContentType,getenv ("CONTENT_TYPE"),sizeof (ContentType) - 1);

      if (!strncmp (ContentType,"multipart/form-data",strlen ("multipart/form-data")))
        {
         Par_SetContentReceivedByCGI (Act_CONT_DATA);
         Par_GetBoundary ();
         return Fil_ReadStdinIntoTmpFile ();
        }
      else if (!strncmp (ContentType,"text/xml",strlen ("text/xml")))
        {
         Gbl.WebService.IsWebService = true;
        }
      else
        {
         Par_SetContentReceivedByCGI (Act_CONT_NORM);

	 /* Allocate memory for query string */
	 if ((Par_Pars.QueryString = malloc (Par_Pars.ContentLength + 1)) == NULL)
	    return false;

	 /* Copy query string from stdin */
         if (fread (Par_Pars.QueryString,sizeof (char),
                    Par_Pars.ContentLength,stdin) != Par_Pars.ContentLength)
           {
            Par_Pars.QueryString[0] = '\0';
            return false;
           }
	 Par_Pars.QueryString[Par_Pars.ContentLength] = '\0';
        }
     }

   return true;
  }

Par_Method_t Par_GetMethod (void)
  {
   return Par_Pars.Method;
  }

/*****************************************************************************/
/**************************** Get boundary string ****************************/
/*****************************************************************************/

static void Par_GetBoundary (void)
  {
   const char *PtrToBoundary;

   /*
   If data are received ==> the environment variable CONTENT_TYPE will hold:
   multipart/form-data; boundary=---------------------------7d13ca2e948
   Gbl.Boundary.StrWithCRLF will be set to:
			  "\r\n-----------------------------7d13ca2e948"
   I.e. 0x0D, 0x0A, '-', '-', and boundary.
   */
   /***** Get pointer to boundary string *****/
   PtrToBoundary = strstr (getenv ("CONTENT_TYPE"),"boundary=") + strlen ("boundary=");

   /***** Check that boundary string is not too long *****/
   if (2 + 2 + strlen (PtrToBoundary) > Par_MAX_BYTES_BOUNDARY_WITH_CR_LF)
      Err_ShowErrorAndExit ("Delimiter string too long.");

   /***** Create boundary strings *****/
   snprintf (Par_Pars.Boundary.StrWithoutCRLF,sizeof (Par_Pars.Boundary.StrWithoutCRLF),
	     "--%s",PtrToBoundary);
   snprintf (Par_Pars.Boundary.StrWithCRLF,sizeof (Par_Pars.Boundary.StrWithCRLF),
	     "%c%c%s",0x0D,0x0A,Par_Pars.Boundary.StrWithoutCRLF);

   /***** Compute lengths *****/
   Par_Pars.Boundary.LengthWithoutCRLF = strlen (Par_Pars.Boundary.StrWithoutCRLF);
   Par_Pars.Boundary.LengthWithCRLF    = 2 + Par_Pars.Boundary.LengthWithoutCRLF;
  }

/*****************************************************************************/
/************************ Create list of parameters **************************/
/*****************************************************************************/
/*
         Parameter #1               Parameter #n
         +------------------+       +------------------+
List --> |Name.Start        |    -> |Name.Start        |
         +------------------+   /   +------------------+
         |Name.Length       |  |    |Name.Length       |
         +------------------+  |    +------------------+
         |FileName.Start    |  |    |FileName.Start    |
         +------------------+  |    +------------------+
         |FileName.Lenght   |  |    |FileName.Lenght   |
         +------------------+  .    +------------------+
         |ContentType.Start |  .    |ContentType.Start |
         +------------------+  .    +------------------+
         |ContentType.Lenght|  |    |ContentType.Lenght|
         +------------------+  |    +------------------+
         |Value.Start       |  |    |Value.Start       |
         +------------------+  |    +------------------+
         |Value.Lengh       |  |    |Value.Lengh       |
         +------------------+ /     +------------------+
         |       Next --------      |       NULL       |
         +------------------+       +------------------+
*/

void Par_CreateListOfPars (void)
  {
   static void (*CreateListOfPars[Act_NUM_CONTENTS]) (void) =
     {
      [Act_CONT_NORM] = Par_CreateListOfParsFromQueryString,
      [Act_CONT_DATA] = Par_CreateListOfParsFromTmpFile,
     };

   /***** Initialize empty list of parameters *****/
   Par_Pars.List = NULL;

   /***** Get list *****/
   if (Par_Pars.ContentLength)
      CreateListOfPars[Par_GetContentReceivedByCGI ()] ();
  }

/*****************************************************************************/
/**************** Create list of parameters from query string ****************/
/*****************************************************************************/

static void Par_CreateListOfParsFromQueryString (void)
  {
   unsigned long CurPos;	// Current position in query string
   struct Par_Param *Par = NULL;	// Initialized to avoid warning
   struct Par_Param *NewPar;

   /***** Check if query string is empty *****/
   if (!Par_Pars.QueryString)    return;
   if (!Par_Pars.QueryString[0]) return;

   /***** Go over the query string
          getting start positions and lengths of parameters *****/
   for (CurPos = 0;
	CurPos < Par_Pars.ContentLength;
	)
     {
      /* Allocate space for a new parameter initialized to 0 */
      if ((NewPar = calloc (1,sizeof (*NewPar))) == NULL)
          Err_NotEnoughMemoryExit ();

      /* Link the previous element in list with the current element */
      if (CurPos == 0)
	 Par_Pars.List = NewPar;	// Pointer to first param
      else
	 Par->Next = NewPar;	// Pointer from former param to new param

      /* Make the current element to be the just created */
      Par = NewPar;

      /* Get parameter name */
      Par->Name.Start = CurPos;
      Par->Name.Length = strcspn (&Par_Pars.QueryString[CurPos],"=");
      CurPos += Par->Name.Length;

      /* Get parameter value */
      if (CurPos < Par_Pars.ContentLength)
	 if (Par_Pars.QueryString[CurPos] == '=')
	   {
	    CurPos++;	// Skip '='
	    if (CurPos < Par_Pars.ContentLength)
	      {
	       Par->Value.Start = CurPos;
	       Par->Value.Length = strcspn (&Par_Pars.QueryString[CurPos],"&");
	       CurPos += Par->Value.Length;
	       if (CurPos < Par_Pars.ContentLength)
		  if (Par_Pars.QueryString[CurPos] == '&')
		     CurPos++;	// Skip '&'
	      }
	   }
     }
  }

/*****************************************************************************/
/*************** Create list of parameters from temporary file ***************/
/*****************************************************************************/

#define Par_LENGTH_OF_STR_BEFORE_PARAM	38		// Length of "Content-Disposition: form-data; name=\""
#define Par_LENGTH_OF_STR_FILENAME	12		// Length of "; filename=\""
#define Par_LENGTH_OF_STR_CONTENT_TYPE	14		// Length of "Content-Type: "
#define Par_MAX_BYTES_STR_AUX		(38 + 1)	// Space to read any of the three preceding strings

static void Par_CreateListOfParsFromTmpFile (void)
  {
   static const char *StringBeforePar = "Content-Disposition: form-data; name=\"";
   static const char *StringFilename = "; filename=\"";
   static const char *StringContentType = "Content-Type: ";
   unsigned long CurPos;	// Current position in temporal file
   struct Par_Param *Par = NULL;	// Initialized to avoid warning
   struct Par_Param *NewPar;
   int Ch;
   char StrAux[Par_MAX_BYTES_STR_AUX + 1];
   FILE *QueryFile = Fil_GetQueryFile ();

   /***** Go over the file
          getting start positions and lengths of parameters *****/
   if (Str_ReadFileUntilBoundaryStr (QueryFile,NULL,
                                     Par_Pars.Boundary.StrWithoutCRLF,
                                     Par_Pars.Boundary.LengthWithoutCRLF,
                                     Fil_MAX_FILE_SIZE) == 1)	// Delimiter string found

      for (CurPos = 0;
	   CurPos < Par_Pars.ContentLength;
	  )
	{
	 /***** Skip \r\n after delimiter string *****/
	 if (fgetc (QueryFile) != 0x0D) break;	// '\r'
	 if (fgetc (QueryFile) != 0x0A) break;	// '\n'

	 Str_GetNextStrFromFileConvertingToLower (QueryFile,StrAux,
	                                          Par_LENGTH_OF_STR_BEFORE_PARAM);
	 if (!strcasecmp (StrAux,StringBeforePar)) // Start of a parameter
	   {
	    /* Allocate space for a new parameter initialized to 0 */
	    if ((NewPar = calloc (1,sizeof (*NewPar))) == NULL)
               Err_NotEnoughMemoryExit ();

	    /* Link the previous element in list with the current element */
	    if (CurPos == 0)
	       Par_Pars.List = NewPar;	// Pointer to first param
	    else
	       Par->Next = NewPar;	// Pointer from former param to new param

	    /* Make the current element to be the just created */
	    Par = NewPar;

	    /***** Get parameter name *****/
	    CurPos = (unsigned long) ftell (QueryFile);	// At start of parameter name
	    Par->Name.Start = CurPos;
	    Ch = Par_ReadTmpFileUntilQuote (QueryFile);
	    CurPos = (unsigned long) ftell (QueryFile);	// Just after quote
	    Par->Name.Length = CurPos - 1 - Par->Name.Start;

	    /* Check if last character read after parameter name is a quote */
	    if (Ch != (int) '\"') break;		// '\"'

	    /* Get next char after parameter name */
	    Ch = fgetc (QueryFile);

	    /***** Check if filename is present *****/
	    if (Ch == (int) StringFilename[0])
	      {
	       Str_GetNextStrFromFileConvertingToLower (QueryFile,StrAux,
	                                                Par_LENGTH_OF_STR_FILENAME-1);
	       if (!strcasecmp (StrAux,StringFilename + 1))	// Start of filename
		 {
		  /* Get filename */
		  CurPos = (unsigned long) ftell (QueryFile);	// At start of filename
		  Par->FileName.Start = CurPos;
		  Ch = Par_ReadTmpFileUntilQuote (QueryFile);
		  CurPos = (unsigned long) ftell (QueryFile);	// Just after quote
		  Par->FileName.Length = CurPos - 1 - Par->FileName.Start;

		  /* Check if last character read after filename is a quote */
		  if (Ch != (int) '\"') break;		// '\"'

		  /* Skip \r\n */
		  if (fgetc (QueryFile) != 0x0D) break;	// '\r'
		  if (fgetc (QueryFile) != 0x0A) break;	// '\n'

		  /* Check if Content-Type is present */
		  Str_GetNextStrFromFileConvertingToLower (QueryFile,StrAux,
		                                           Par_LENGTH_OF_STR_CONTENT_TYPE);
		  if (!strcasecmp (StrAux,StringContentType)) // Start of Content-Type
		    {
		     /* Get content type */
		     CurPos = (unsigned long) ftell (QueryFile);	// At start of content type
		     Par->ContentType.Start = CurPos;
		     Ch = Par_ReadTmpFileUntilReturn (QueryFile);
		     CurPos = (unsigned long) ftell (QueryFile);	// Just after return
		     Par->ContentType.Length = CurPos - 1 - Par->ContentType.Start;
		    }
		 }
	      }

	    /***** Now \r\n\r\n is expected just before parameter value or file content *****/
	    /* Check if last character read is '\r' */
	    if (Ch != 0x0D) break;			// '\r'

	    /* Skip \n\r\n */
	    if (fgetc (QueryFile) != 0x0A) break;	// '\n'
	    if (fgetc (QueryFile) != 0x0D) break;	// '\r'
	    if (fgetc (QueryFile) != 0x0A) break;	// '\n'

	    /***** Get parameter value or file content *****/
	    CurPos = (unsigned long) ftell (QueryFile);	// At start of value or file content
	    if (Str_ReadFileUntilBoundaryStr (QueryFile,NULL,
					      Par_Pars.Boundary.StrWithCRLF,
					      Par_Pars.Boundary.LengthWithCRLF,
					      Fil_MAX_FILE_SIZE) != 1) break;	// Boundary string not found

	    // Delimiter string found
	    Par->Value.Start = CurPos;
	    CurPos = (unsigned long) ftell (QueryFile);	// Just after delimiter string
	    Par->Value.Length = CurPos - Par_Pars.Boundary.LengthWithCRLF -
		                  Par->Value.Start;
	   }
        }
  }

/*****************************************************************************/
/******************** Read from file until quote '\"' ************************/
/*****************************************************************************/
// Return last char read

static int Par_ReadTmpFileUntilQuote (FILE *QueryFile)
  {
   int Ch;

   /***** Read until quote if found *****/
   do
      Ch = fgetc (QueryFile);
   while (Ch != EOF && Ch != (int) '\"');

   return Ch;
  }

/*****************************************************************************/
/*************************** Read from file until \r *************************/
/*****************************************************************************/
// Return last char read

static int Par_ReadTmpFileUntilReturn (FILE *QueryFile)
  {
   int Ch;

   /***** Read until \r if found *****/
   do
      Ch = fgetc (QueryFile);
   while (Ch != EOF && Ch != 0x0D);	// '\r'

   return Ch;
  }

/*****************************************************************************/
/***************** Free memory allocated for query string ********************/
/*****************************************************************************/

void Par_FreePars (void)
  {
   struct Par_Param *Param;
   struct Par_Param *NextParam;

   /***** Free list of parameters *****/
   for (Param = Par_Pars.List;
	Param != NULL;
	Param = NextParam)
     {
      NextParam = Param->Next;
      free (Param);
     }

   /***** Free query string *****/
   if (Par_Pars.QueryString)
      free (Par_Pars.QueryString);
  }

/*****************************************************************************/
/************************* Get the value of a parameter **********************/
/*****************************************************************************/
// Return the number of parameters found
// If ParPtr is not NULL, on return it will point to the first ocurrence in list of parameters
// ParValue can be NULL (if so, no value is copied)

unsigned Par_GetPar (Par_ParamType_t ParType,const char *ParName,
                     char *ParValue,size_t MaxBytes,
                     struct Par_Param **ParPtr)	// NULL if not used
  {
   extern const char *Par_SEPARATOR_PARAM_MULTIPLE;
   FILE *QueryFile = Fil_GetQueryFile ();
   size_t BytesAlreadyCopied = 0;
   unsigned i;
   struct Par_Param *Par;
   char *PtrDst;
   unsigned NumTimes;
   bool ParFound = false;
   unsigned ParNameLength;
   bool FindMoreThanOneOcurrence;
   char ErrorTxt[256];

   /***** Default values returned *****/
   if (ParValue)
      ParValue[0] = '\0'; // By default, the value of the parameter will be an empty string

   /***** Only some selected parameters can be passed by GET method *****/
   if (Par_GetMethod () == Par_METHOD_GET)
      if (!Par_CheckIsParCanBeUsedInGETMethod (ParName))
	 return 0;	// Return no-parameters-found

   /***** Initializations *****/
   ParNameLength = strlen (ParName);
   PtrDst = ParValue;
   FindMoreThanOneOcurrence = (ParType == Par_PARAM_MULTIPLE);

   /***** For multiple parameters, loop for any ocurrence of the parameter
          For unique parameter, find only the first ocurrence *****/
   for (Par = Par_Pars.List, NumTimes = 0;
	Par != NULL && (FindMoreThanOneOcurrence || NumTimes == 0);
	)
      /***** Find next ocurrence of parameter in list of parameters *****/
      for (ParFound = false;
	   Par != NULL && !ParFound;
	   Par = Par->Next)
	{
	 if (Par->Name.Length == ParNameLength)
	   {
	    // The current element in the list has the length of the searched parameter
	    // Check if the name of the parameter is the same
	    switch (Par_GetContentReceivedByCGI ())
	      {
	       case Act_CONT_NORM:
		  ParFound = !strncmp (ParName,&Par_Pars.QueryString[Par->Name.Start],
					 Par->Name.Length);
		  break;
	       case Act_CONT_DATA:
		  fseek (QueryFile,Par->Name.Start,SEEK_SET);
		  for (i = 0, ParFound = true;
		       i < Par->Name.Length && ParFound;
		       i++)
		     if (ParName[i] != (char) fgetc (QueryFile))
			ParFound = false;
		  break;
	      }

	    if (ParFound)
	      {
	       NumTimes++;
	       if (NumTimes == 1)	// NumTimes == 1 ==> the first ocurrence of this parameter
		 {
		  /***** Get the first ocurrence of this parameter in list *****/
		  if (ParPtr)
		     *ParPtr = Par;

		  /***** If this parameter is a file ==> do not find more ocurrences ******/
		  if (Par->FileName.Start != 0)	// It's a file
		     FindMoreThanOneOcurrence = false;
		 }
	       else			// NumTimes > 1 ==> not the first ocurrence of this parameter
		 {
	          /***** Add separator when param multiple *****/
		  /* Check if there is space to copy separator */
		  if (BytesAlreadyCopied + 1 > MaxBytes)
		    {
		     snprintf (ErrorTxt,sizeof (ErrorTxt),
	                       "Multiple parameter <strong>%s</strong> too large,"
			       " it exceed the maximum allowed size (%lu bytes).",
			       ParName,(unsigned long) MaxBytes);
		     Err_ShowErrorAndExit (ErrorTxt);
		    }

		  /* Copy separator */
		  if (PtrDst)
		     *PtrDst++ = Par_SEPARATOR_PARAM_MULTIPLE[0];	// Separator in the destination string
		  BytesAlreadyCopied++;
		 }

	       /***** Copy parameter value *****/
	       if (Par->Value.Length)
		 {
		  /* Check if there is space to copy the parameter value */
		  if (BytesAlreadyCopied + Par->Value.Length > MaxBytes)
		    {
		     snprintf (ErrorTxt,sizeof (ErrorTxt),
	                       "Parameter <strong>%s</strong> too large,"
			       " it exceed the maximum allowed size (%lu bytes).",
			       ParName,(unsigned long) MaxBytes);
		     Err_ShowErrorAndExit (ErrorTxt);
		    }

		  /* Copy parameter value */
		  switch (Par_GetContentReceivedByCGI ())
		    {
		     case Act_CONT_NORM:
			if (PtrDst)
			   strncpy (PtrDst,&Par_Pars.QueryString[Par->Value.Start],
				    Par->Value.Length);
			break;
		     case Act_CONT_DATA:
		        if (Par->FileName.Start == 0 &&	// Copy into destination only if it's not a file
		            PtrDst)
		          {
			   fseek (QueryFile,Par->Value.Start,SEEK_SET);
			   if (fread (PtrDst,sizeof (char),Par->Value.Length,QueryFile) !=
			       Par->Value.Length)
			      Err_ShowErrorAndExit ("Error while getting value of parameter.");
		          }
			break;
		    }
		  BytesAlreadyCopied += Par->Value.Length;
		  if (PtrDst)
		     PtrDst += Par->Value.Length;
		 }
	      }
	   }
	}

   if (PtrDst)
      *PtrDst = '\0'; // Add the final NULL

   return NumTimes;
  }

/*****************************************************************************/
/*************** Check if parameter can be used in GET method ****************/
/*****************************************************************************/

static bool Par_CheckIsParCanBeUsedInGETMethod (const char *ParName)
  {
   static const char *ValidParsInGETMethod[] =
     {
      "cty",	// To enter directly to a country
      "ins",	// To enter directly to an institution
      "ctr",	// To enter directly to a center
      "deg",	// To enter directly to a degree
      "crs",	// To enter directly to a course
      "usr",	// To enter directly to a user's profile
      "agd",	// To view user's public agenda
      "act",	// To execute directly an action (allowed only for fully public actions)
      "ses",	// To use an open session when redirecting from one language to another
      "key",	// To verify an email address
     };
#define NUM_VALID_PARAMS (sizeof (ValidParsInGETMethod) / sizeof (ValidParsInGETMethod[0]))
   unsigned i;

   for (i = 0;
	i < NUM_VALID_PARAMS;
	i++)
      if (!strcmp (ParName,ValidParsInGETMethod[i]))
         return true;
   return false;
  }

/*****************************************************************************/
/****************** Get the parameters sent to this CGI **********************/
/*****************************************************************************/

void Par_GetMainPars (void)
  {
   extern const char *Par_CodeStr[];
   long ActCod;
   char Nick[Nck_MAX_BYTES_NICK_WITH_ARROBA + 1];
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   // First of all, get action, and session identifier.
   // So, if other parameters have been stored in the database, there will be no problems to get them.

   /***** Get action to perform *****/
   if (Gbl.WebService.IsWebService)
     {
      Gbl.Action.Act = Gbl.Action.Original = ActWebSvc;
      Tab_SetCurrentTab ();
      return;
     }

   /***** Set default action *****/
   Gbl.Action.Act = Gbl.Action.Original = ActUnk;

   /***** Get another user's nickname, if exists
          (this nickname is used to go to another user's profile,
           not to get the logged user) *****/
   if (Par_GetParText ("usr",Nick,sizeof (Nick) - 1))
     {
      if (Nick[0])
	{
	 /* Set another user's nickname */
	 Str_RemoveLeadingArrobas (Nick);
         Str_Copy (Gbl.Usrs.Other.UsrDat.Nickname,Nick,	// Leading arrobas already removed
                   sizeof (Gbl.Usrs.Other.UsrDat.Nickname) - 1);

	 // This user's code is used to go to public profile
	 // and to refresh old publishings in user's timeline
	 // If user does not exist ==> UsrCod = -1
	 Gbl.Usrs.Other.UsrDat.UsrCod = Nck_GetUsrCodFromNickname (Gbl.Usrs.Other.UsrDat.Nickname);
         Gbl.Action.Act = Gbl.Action.Original = ActSeeOthPubPrf;	// Set default action if no other is specified
	}
     }
   else if (Par_GetParText ("agd",Nick,sizeof (Nick) - 1))
     {
      if (Nick[0])
	{
	 /* Set another user's nickname */
	 Str_RemoveLeadingArrobas (Nick);
         Str_Copy (Gbl.Usrs.Other.UsrDat.Nickname,Nick,	// Leading arrobas already removed
                   sizeof (Gbl.Usrs.Other.UsrDat.Nickname) - 1);

	 // This user's code is used to go to public agenda
	 // If user does not exist ==> UsrCod = -1
	 Gbl.Usrs.Other.UsrDat.UsrCod = Nck_GetUsrCodFromNickname (Gbl.Usrs.Other.UsrDat.Nickname);
	 Gbl.Action.Act = Gbl.Action.Original = ActFrmLogInUsrAgd;	// Set default action if no other is specified
	}
     }

   /***** Get action to perform *****/
   ActCod = Par_GetParLong ("act");
   if (ActCod >= 0 && ActCod <= ActLst_MAX_ACTION_COD)
      Gbl.Action.Act = Gbl.Action.Original = Act_GetActionFromActCod (ActCod);

   /***** Some preliminary adjusts depending on action *****/
   switch (Act_GetBrowserTab (Gbl.Action.Act))
     {
      case Act_AJAX_NORMAL:
	 Gbl.Action.UsesAJAX          = true;
	 Gbl.Action.IsAJAXAutoRefresh = false;
	 break;
      case Act_AJAX_RFRESH:
         Gbl.Action.UsesAJAX          = true;
         Gbl.Action.IsAJAXAutoRefresh = true;
         break;
      default:
	 Gbl.Action.UsesAJAX          = false;
	 Gbl.Action.IsAJAXAutoRefresh = false;
         break;
     }

   /***** Get session identifier, if exists *****/
   Par_GetParText ("ses",Gbl.Session.Id,Cns_BYTES_SESSION_ID);
   if (Gbl.Session.Id[0])
     {
      /***** Get user's code, password, current degree and current course from stored session *****/
      if (Ses_GetSessionData ())
	 Gbl.Session.IsOpen = true;
      else
	{
	 Gbl.Session.HasBeenDisconnected = true;
	 Gbl.Session.Id[0] = '\0';
	}
     }
   else
     {
      // Try old parameter "IdSes" (allowed for compatibility, to be removed soon)
      Par_GetParText ("IdSes",Gbl.Session.Id,Cns_BYTES_SESSION_ID);
      if (Gbl.Session.Id[0])
	{
	 /***** Get user's code, password, current degree and current course from stored session *****/
	 if (Ses_GetSessionData ())
	    Gbl.Session.IsOpen = true;
	 else
	   {
	    Gbl.Session.HasBeenDisconnected = true;
	    Gbl.Session.Id[0] = '\0';
	   }
	}
     }

   /***** Get user password and login *****/
   switch (Gbl.Action.Act)
     {
      case ActLogIn:
      case ActFrmLogInUsrAgd:
      case ActLogInUsrAgd:	// This action is necessary here when log in fails
         Pwd_GetParUsrPwdLogin ();
	 /* falls through */
	 /* no break */
      case ActReqSndNewPwd:
      case ActSndNewPwd:
         Usr_GetParUsrIdLogin ();
	 break;
     }

   /***** Try to get settings changed from current IP *****/
   Set_GetSettingsFromIP ();

   /***** Get country if exists (from menu) *****/
   Par_GetParText (Par_CodeStr[ParCod_Cty],LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
   if (LongStr[0])	// Parameter available
     {
      Gbl.Hierarchy.Node[HieLvl_CTY].HieCod = Str_ConvertStrCodToLongCod (LongStr);
      Gbl.Hierarchy.Node[HieLvl_INS].HieCod =
      Gbl.Hierarchy.Node[HieLvl_CTR].HieCod =
      Gbl.Hierarchy.Node[HieLvl_DEG].HieCod =
      Gbl.Hierarchy.Node[HieLvl_CRS].HieCod = -1L;
     }

   /***** Get institution if exists (from menu) *****/
   Par_GetParText (Par_CodeStr[ParCod_Ins],LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
   if (LongStr[0])	// Parameter available
     {
      Gbl.Hierarchy.Node[HieLvl_INS].HieCod = Str_ConvertStrCodToLongCod (LongStr);
      Gbl.Hierarchy.Node[HieLvl_CTR].HieCod =
      Gbl.Hierarchy.Node[HieLvl_DEG].HieCod =
      Gbl.Hierarchy.Node[HieLvl_CRS].HieCod = -1L;
     }

   /***** Get center if exists (from menu) *****/
   Par_GetParText (Par_CodeStr[ParCod_Ctr],LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
   if (LongStr[0])	// Parameter available
     {
      Gbl.Hierarchy.Node[HieLvl_CTR].HieCod = Str_ConvertStrCodToLongCod (LongStr);
      Gbl.Hierarchy.Node[HieLvl_DEG].HieCod =
      Gbl.Hierarchy.Node[HieLvl_CRS].HieCod = -1L;
     }

   /***** Get numerical degree code if exists (from menu) *****/
   Par_GetParText (Par_CodeStr[ParCod_Deg],LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
   if (LongStr[0])	// Parameter available
     {
      Gbl.Hierarchy.Node[HieLvl_DEG].HieCod = Str_ConvertStrCodToLongCod (LongStr);
      Gbl.Hierarchy.Node[HieLvl_CRS].HieCod = -1L;	// Reset possible course from session
     }

   /***** Get numerical course code if exists (from menu) *****/
   Par_GetParText (Par_CodeStr[ParCod_Crs],LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
   if (LongStr[0])	// Parameter available
      Gbl.Hierarchy.Node[HieLvl_CRS].HieCod = Str_ConvertStrCodToLongCod (LongStr);	// Overwrite CrsCod from session

   /***** Get tab to activate *****/
   Gbl.Action.Tab = TabUnk;
   if (Gbl.Action.Act == ActMnu)
     {
      Gbl.Action.Tab = (Tab_Tab_t)
	               Par_GetParUnsignedLong ("NxtTab",
                                               (unsigned long) TabUnk,
                                               Tab_NUM_TABS - 1,
                                               (unsigned long) TabUnk);
      Tab_DisableIncompatibleTabs ();
     }
   else	// Set tab depending on current action
      Tab_SetCurrentTab ();
  }

/*****************************************************************************/
/******* Get the value of a single parameter and change format to text *******/
/*****************************************************************************/
// Return the number of parameters found

unsigned Par_GetParText (const char *ParName,char *ParValue,size_t MaxBytes)
  {
   return Par_GetParAndChangeFormat (ParName,ParValue,MaxBytes,
                                     Str_TO_TEXT,
                                     Str_REMOVE_SPACES);
  }

/*****************************************************************************/
/****************** Get the unsigned value of a parameter ********************/
/*****************************************************************************/

unsigned long Par_GetParUnsignedLong (const char *ParName,
                                      unsigned long Min,
                                      unsigned long Max,
                                      unsigned long Default)
  {
   char UnsignedLongStr[Cns_MAX_DECIMAL_DIGITS_ULONG + 1];
   unsigned long UnsignedLongNum;

   /***** Get parameter with unsigned number *****/
   Par_GetParText (ParName,UnsignedLongStr,Cns_MAX_DECIMAL_DIGITS_ULONG);
   if (sscanf (UnsignedLongStr,"%lu",&UnsignedLongNum) == 1)
      if (UnsignedLongNum >= Min && UnsignedLongNum <= Max)
         return UnsignedLongNum;

   return Default;
  }

/*****************************************************************************/
/******************** Get the long value of a parameter **********************/
/*****************************************************************************/

long Par_GetParLong (const char *ParName)
  {
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Get parameter with long number *****/
   Par_GetParText (ParName,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/************************ Get a boolean Y/N parameter ************************/
/*****************************************************************************/

bool Par_GetParBool (const char *ParName)
  {
   char YN[1 + 1];

   /***** Get parameter "Y"/"N" and convert to boolean *****/
   Par_GetParText (ParName,YN,1);
   return (Str_ConvertToUpperLetter (YN[0]) == 'Y');
  }

/*****************************************************************************/
/*** Get value of a single parameter and change format to not rigorous HTML **/
/*****************************************************************************/
// Return the number of parameters found

unsigned Par_GetParHTML (const char *ParName,char *ParValue,size_t MaxBytes)
  {
   return Par_GetParAndChangeFormat (ParName,ParValue,MaxBytes,
                                     Str_TO_HTML,Str_REMOVE_SPACES);
  }

/*****************************************************************************/
/****** Get the value of a multiple parameter and change format to text ******/
/*****************************************************************************/
// Return the number of parameters found

unsigned Par_GetParMultiToText (const char *ParName,char *ParValue,size_t MaxBytes)
  {
   unsigned NumTimes = Par_GetPar (Par_PARAM_MULTIPLE,ParName,
                                   ParValue,MaxBytes,NULL);

   Str_ChangeFormat (Str_FROM_FORM,Str_TO_TEXT,
                     ParValue,MaxBytes,Str_REMOVE_SPACES);
   return NumTimes;
  }

/*****************************************************************************/
/*********** Get the value of a single parameter and change format ***********/
/*****************************************************************************/
// Return the number of parameters found

unsigned Par_GetParAndChangeFormat (const char *ParName,char *ParValue,size_t MaxBytes,
                                    Str_ChangeTo_t ChangeTo,
                                    Str_RemoveSpaces_t RemoveLeadingAndTrailingSpaces)
  {
   unsigned NumTimes = Par_GetPar (Par_PARAM_SINGLE,ParName,
                                   ParValue,MaxBytes,NULL);

   Str_ChangeFormat (Str_FROM_FORM,ChangeTo,
                     ParValue,MaxBytes,RemoveLeadingAndTrailingSpaces);
   return NumTimes;
  }

/*****************************************************************************/
/***** Search in the string StrSrc the next string until find separator ******/
/*****************************************************************************/
// Modifies *StrSrc
// When StrDst is NULL, nothing is stored
// Return true if characters found

bool Par_GetNextStrUntilSeparParMult (const char **StrSrc,char *StrDst,size_t LongMax)
  {
   size_t i = 0;
   unsigned char Ch;	// Must be unsigned to work with characters > 127
   bool CharsFound = false;

   do
      if ((Ch = (unsigned char) **StrSrc))
	 (*StrSrc)++;
   while (Ch && Ch < 32);	// Skip special characters
				// (the separator will be a special character
				// less than 32)

   while (Ch >= 32)		// Until special character or end
     {
      CharsFound = true;
      if (i < LongMax)
         if (StrDst)
            StrDst[i++] = (char) Ch;
      if ((Ch = (unsigned char) **StrSrc))
	 (*StrSrc)++;
     }

   if (StrDst)
      StrDst[i] = '\0';

   return CharsFound;
  }

/*****************************************************************************/
/***** Search in the string StrSrc the next string until find separator ******/
/*****************************************************************************/
// Modifies *StrSrc
// When StrDst is NULL, nothing is stored
// Return true if characters found

bool Par_GetNextStrUntilComma (const char **StrSrc,char *StrDst,size_t LongMax)
  {
   size_t i = 0;
   unsigned char Ch;	// Must be unsigned to work with characters > 127
   bool CharsFound = false;

   do
      if ((Ch = (unsigned char) **StrSrc))
	 (*StrSrc)++;
   while (Ch == ',');	// Skip commas

   while (Ch && Ch != ',')		// Until special character or end
     {
      CharsFound = true;
      if (i < LongMax)
         if (StrDst)
            StrDst[i++] = (char) Ch;
      if ((Ch = (unsigned char) **StrSrc))
	 (*StrSrc)++;
     }

   if (StrDst)
      StrDst[i] = '\0';

   return CharsFound;
  }

/*****************************************************************************/
/******** Replace each comma by the separator of multiple parameters *********/
/*****************************************************************************/

void Par_ReplaceSeparatorMultipleByComma (char *Str)
  {
   for (;
	*Str;
	Str++)
      if ((unsigned char) *Str < 32)
	 *Str = ',';
  }

/*****************************************************************************/
/********************** Put an unsigned hidden parameter *********************/
/*****************************************************************************/

void Par_PutParUnsigned (const char *Id,const char *ParName,unsigned ParValue)
  {
   HTM_Txt ("<input type=\"hidden\"");
   if (Id)
      if (Id[0])
         HTM_TxtF (" id=\"%s\"",Id);
   HTM_TxtF (" name=\"%s\" value=\"%u\" />",ParName,ParValue);
  }

void Par_PutParUnsignedDisabled (const char *Id,const char *ParName,unsigned ParValue)
  {
   HTM_Txt ("<input type=\"hidden\"");
   if (Id)
      if (Id[0])
         HTM_TxtF (" id=\"%s\"",Id);
   HTM_TxtF (" name=\"%s\" value=\"%u\" disabled=\"disabled\" />",
	     ParName,ParValue);
  }

/*****************************************************************************/
/******************** Put the value of a code parameter **********************/
/*****************************************************************************/

void ParCod_PutPar (ParCod_Param_t ParCod,long Cod)
  {
   extern const char *Par_CodeStr[];

   if (ParCod != ParCod_None && Cod >= 0)
      // <0 => not specified => don't write parameter
      //  0 => another country, institution, centre...
      // >0 => a given country, institution, centre...
      Par_PutParLong (NULL,Par_CodeStr[ParCod],Cod);
  }

/*****************************************************************************/
/************************* Put a long hidden parameter ***********************/
/*****************************************************************************/

void Par_PutParLong (const char *Id,const char *ParName,long ParValue)
  {
   HTM_Txt ("<input type=\"hidden\"");
   if (Id)
      if (Id[0])
         HTM_TxtF (" id=\"%s\"",Id);
   HTM_TxtF (" name=\"%s\" value=\"%ld\" />",ParName,ParValue);
  }

/*****************************************************************************/
/************************* Put a char hidden parameter ***********************/
/*****************************************************************************/

void Par_PutParChar (const char *ParName,char ParValue)
  {
   HTM_TxtF ("<input type=\"hidden\" name=\"%s\" value=\"%c\" />",
             ParName,ParValue);
  }

/*****************************************************************************/
/************************ Put a string hidden parameter **********************/
/*****************************************************************************/

void Par_PutParString (const char *Id,const char *ParName,const char *ParValue)
  {
   HTM_Txt ("<input type=\"hidden\"");
   if (Id)
      if (Id[0])
         HTM_TxtF (" id=\"%s\"",Id);
   HTM_TxtF (" name=\"%s\" value=\"%s\" />",
             ParName,ParValue ? ParValue :
	                     "");
  }

/*****************************************************************************/
/********* Put a hidden parameter with the type of order in listing **********/
/*****************************************************************************/

void Par_PutParOrder (unsigned SelectedOrder)
  {
   Par_PutParUnsigned (NULL,"Order",SelectedOrder);
  }

/*****************************************************************************/
/****************************** Set/Get current IP ***************************/
/*****************************************************************************/
/*
CGI Environment Variables:
REMOTE_ADDR
The IP address of the remote host making the request.
*/
void Par_SetIP (void)
  {
   if (getenv ("REMOTE_ADDR"))
      Str_Copy (Par_Pars.IP,getenv ("REMOTE_ADDR"),sizeof (Par_Pars.IP) - 1);
   else
      Par_Pars.IP[0] = '\0';
  }

const char *Par_GetIP (void)
  {
   return Par_Pars.IP;
  }
