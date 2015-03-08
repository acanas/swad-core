// swad_parameter.c: CGI parameters

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_config.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_password.h"
#include "swad_preference.h"
#include "swad_tab.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*********************** Private types and constants *************************/
/*****************************************************************************/

typedef enum
  {
   Par_PARAM_SINGLE,
   Par_PARAM_MULTIPLE,
  } tParamType; // Parameter is present only one time / multiple times

/*****************************************************************************/
/****************************** Private variables ****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static unsigned Par_GetParameter (tParamType ParamType,const char *ParamName,
                                  char *ParamValue,size_t MaxBytes);

/*****************************************************************************/
/*** Read all parameters passed to this CGI and store for later processing ***/
/*****************************************************************************/

bool Par_GetQueryString (void)
  {
   char Method[256];
   char ContentType[512];

   strcpy (Method,getenv ("REQUEST_METHOD"));

   if (!strcmp (Method,"GET"))
     {
      Gbl.GetMethod = true;
      Gbl.ContentReceivedByCGI = Act_CONTENT_NORM;
      strcpy (Gbl.QueryString,getenv ("QUERY_STRING"));
     }
   else
     {
      /* If data are received ==> the environment variable CONTENT_TYPE will hold:
      multipart/form-data; boundary=---------------------------7d13ca2e948
      */
      if (getenv ("CONTENT_TYPE") == NULL)
         return false;

      strcpy (ContentType,getenv ("CONTENT_TYPE"));

      if (!strncmp (ContentType,"multipart/form-data",strlen ("multipart/form-data")))
        {
         Gbl.ContentReceivedByCGI = Act_CONTENT_DATA;
         /* If data are received ==> the environment variable CONTENT_TYPE will hold:
         multipart/form-data; boundary=---------------------------7d13ca2e948
         Gbl.DelimiterStringIncludingInitialRet will be set to:
			        "\r\n-----------------------------7d13ca2e948"
         I.e. 0x0D, 0x0A, '-', '-', and boundary.
         */
         sprintf (Gbl.DelimiterString,"--%s",strstr (getenv ("CONTENT_TYPE"),"boundary=") + strlen ("boundary="));
         sprintf (Gbl.DelimiterStringIncludingInitialRet,"%c%c%s",0x0D,0x0A,Gbl.DelimiterString);
         return Fil_ReadStdinIntoTmpFile ();
        }
      else if (!strncmp (ContentType,"text/xml",strlen ("text/xml")))
        {
         Gbl.WebService.IsWebService = true;
        }
      else
        {
         Gbl.ContentReceivedByCGI = Act_CONTENT_NORM;

         /***** Get the string sent by form *****/
         if (fgets (Gbl.QueryString,Cns_MAX_LENGTH_ARGS_SENT_TO_CGI,stdin) == NULL)
            Gbl.QueryString[0] = '\0';
        }
     }
   return true;
  }

/*****************************************************************************/
/****************** Get the parameters sent to this CGI **********************/
/*****************************************************************************/

void Par_GetMainParameters (void)
  {
   extern Act_Action_t Act_FromActCodToAction[1+Act_MAX_ACTION_COD];
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];
   char UnsignedStr[10+1];
   unsigned UnsignedNum;
   char Nickname[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA + 1];
   long OtherUsrCod;
   char LongStr[1+10+1];
   char YearStr[2+1];
   Lay_Layout_t LayoutFromForm;

   /***** Reset codes of country, institution, centre, degree and course *****/
   Gbl.CurrentCty.Cty.CtyCod =
   Gbl.CurrentIns.Ins.InsCod =
   Gbl.CurrentCtr.Ctr.CtrCod =
   Gbl.CurrentDeg.Deg.DegCod =
   Gbl.CurrentCrs.Crs.CrsCod = -1L;
   Gbl.YearOK = false;

   // First of all, get action, and session identifier.
   // So, if other parameters have been stored in the database, there will be no problems to get them.

   /***** Get action to perform *****/
   if (Gbl.WebService.IsWebService)
     {
      Gbl.CurrentAct = ActWebSvc;
      Tab_SetCurrentTab ();
      return;
     }

   /* Check if SWAD is called from an external site */
   Imp_GetImpUsrAndSession ();
   if (Gbl.Imported.ExternalUsrId[0] &&
       Gbl.Imported.ExternalSesId[0])
     {
      Gbl.CurrentAct = ActAutUsrExt;
      Tab_SetCurrentTab ();
      return;
     }
   // SWAD is not called from external site

   Gbl.CurrentAct = ActUnk;
   Par_GetParToText ("act",UnsignedStr,10);
   if (UnsignedStr[0])
     {
      if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
         if (UnsignedNum <= Act_MAX_ACTION_COD)
            Gbl.CurrentAct = Act_FromActCodToAction[UnsignedNum];
     }
   else
     {
      // Try old parameter "ActCod" (allowed for compatibility, to be removed soon)
      Par_GetParToText ("ActCod",UnsignedStr,10);
      if (UnsignedStr[0])
	 if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
	    if (UnsignedNum <= Act_MAX_ACTION_COD)
	       Gbl.CurrentAct = Act_FromActCodToAction[UnsignedNum];
     }

   /***** Get session identifier, if exists *****/
   Par_GetParToText ("ses",Gbl.Session.Id,Ses_LENGTH_SESSION_ID);
   if (Gbl.Session.Id[0])
     {
      /***** Get user's code, password, current degree and current course from stored session *****/
      if (Ses_GetSessionData ())
        {
	 Gbl.Session.IsOpen = true;
         Imp_GetImpSessionData ();
        }
      else
	{
	 Gbl.Session.HasBeenDisconnected = true;
	 Gbl.Session.Id[0] = '\0';
	}
     }
   else
     {
      // Try old parameter "IdSes" (allowed for compatibility, to be removed soon)
      Par_GetParToText ("IdSes",Gbl.Session.Id,Ses_LENGTH_SESSION_ID);
      if (Gbl.Session.Id[0])
	{
	 /***** Get user's code, password, current degree and current course from stored session *****/
	 if (Ses_GetSessionData ())
	   {
	    Gbl.Session.IsOpen = true;
	    Imp_GetImpSessionData ();
	   }
	 else
	   {
	    Gbl.Session.HasBeenDisconnected = true;
	    Gbl.Session.Id[0] = '\0';
	   }
	}
     }

   /***** Get user password and login *****/
   switch (Gbl.CurrentAct)
     {
      case ActAutUsrInt:
         Pwd_GetParamUsrPwdLogin ();
         // no break;
      case ActReqSndNewPwd:
      case ActSndNewPwd:
         Usr_GetParamUsrIdLogin ();
	 break;
     }

   /***** Try to get preferences changed from current IP *****/
   Prf_GetPrefsFromIP ();
   if (Gbl.Prefs.Layout == Lay_LAYOUT_UNKNOWN)
      Gbl.Prefs.Layout = Lay_LAYOUT_DEFAULT;

   if (!Gbl.Session.IsOpen)	// When no session open (no logged user)...
     {
      /***** Try to get preferences changed from current IP *****/
      if (Gbl.Prefs.Theme == The_THEME_UNKNOWN)
         Gbl.Prefs.Theme = The_THEME_DEFAULT;

      /***** Try to get parameter layout (sent via GET method) *****/
      if ((LayoutFromForm = Lay_GetParamLayout ()) != Lay_LAYOUT_UNKNOWN)
        {
         /* Parameter is correct, so set layout */
         Gbl.Prefs.Layout = LayoutFromForm;

         /* Update preferences from current IP */
         Prf_SetPrefsFromIP ();
        }

      /***** Set path of theme *****/
      sprintf (Gbl.Prefs.PathTheme,"%s/%s/%s",
               Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_THEMES,The_ThemeId[Gbl.Prefs.Theme]);
      /***** Set path of theme *****/
      sprintf (Gbl.Prefs.PathIconSet,"%s/%s/%s",
               Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_ICON_SETS,
               Ico_IconSetId[Gbl.Prefs.IconSet]);
     }

   /***** Get country if exists (from menu) *****/
   Par_GetParToText ("cty",LongStr,1+10);
   if (LongStr[0])	// Parameter "cty" available
     {
      Gbl.CurrentCty.Cty.CtyCod = Str_ConvertStrCodToLongCod (LongStr);
      Gbl.YearOK = false;
      Gbl.CurrentIns.Ins.InsCod =
      Gbl.CurrentCtr.Ctr.CtrCod =
      Gbl.CurrentDeg.Deg.DegCod =
      Gbl.CurrentCrs.Crs.CrsCod = -1L;
     }

   /***** Get institution if exists (from menu) *****/
   Par_GetParToText ("ins",LongStr,1+10);
   if (LongStr[0])	// Parameter "ins" available
     {
      Gbl.CurrentIns.Ins.InsCod = Str_ConvertStrCodToLongCod (LongStr);
      Gbl.YearOK = false;
      Gbl.CurrentCtr.Ctr.CtrCod =
      Gbl.CurrentDeg.Deg.DegCod =
      Gbl.CurrentCrs.Crs.CrsCod = -1L;
     }

   /***** Get centre if exists (from menu) *****/
   Par_GetParToText ("ctr",LongStr,1+10);
   if (LongStr[0])	// Parameter "ctr" available
     {
      Gbl.CurrentCtr.Ctr.CtrCod = Str_ConvertStrCodToLongCod (LongStr);
      Gbl.YearOK = false;
      Gbl.CurrentDeg.Deg.DegCod =
      Gbl.CurrentCrs.Crs.CrsCod = -1L;
     }

   /***** Get numerical degree code if exists (from menu) *****/
   Par_GetParToText ("deg",LongStr,1+10);
   if (LongStr[0])	// Parameter "deg" available
     {
      Gbl.CurrentDeg.Deg.DegCod = Str_ConvertStrCodToLongCod (LongStr);
      if (Gbl.CurrentDeg.Deg.DegCod > 0)
        {
         /***** Get possible course year *****/
         Par_GetParToText ("CrsYear",YearStr,2);
         if (YearStr[0])	// Parameter CrsYear available
           {
            Gbl.CurrentCrs.Crs.Year = Deg_ConvStrToYear (YearStr);
            Gbl.YearOK = true;
           }

         Gbl.CurrentCrs.Crs.CrsCod = -1L;	// Reset possible course from session
        }
      else
         Gbl.CurrentCty.Cty.CtyCod =
         Gbl.CurrentIns.Ins.InsCod =
         Gbl.CurrentCtr.Ctr.CtrCod =
         Gbl.CurrentDeg.Deg.DegCod =
         Gbl.CurrentCrs.Crs.CrsCod = -1L;
     }

   /***** Get numerical course code if exists (from menu) *****/
   Par_GetParToText ("crs",LongStr,1+10);
   if (LongStr[0])	// Parameter "crs" available
      Gbl.CurrentCrs.Crs.CrsCod = Str_ConvertStrCodToLongCod (LongStr);	// Overwrite CrsCod from session
   else
     {
      // Try old parameter "CrsCod" (allowed for compatibility with old links, to be removed in 2016)
      Par_GetParToText ("CrsCod",LongStr,1+10);
      if (LongStr[0])	// Parameter "CrsCod" available
	 Gbl.CurrentCrs.Crs.CrsCod = Str_ConvertStrCodToLongCod (LongStr);	// Overwrite CrsCod from session
     }

   /***** Get user's nickname, if exists
          (this nickname is used to go to a user's profile, not to get the logged user) *****/
   Par_GetParToText ("usr",Nickname,Nck_MAX_BYTES_NICKNAME_WITH_ARROBA);
   if ((OtherUsrCod = Nck_GetUsrCodFromNickname (Nickname)) > 0)
     {
      Gbl.Usrs.Other.UsrDat.UsrCod = OtherUsrCod;
      Gbl.CurrentAct = ActSeePubPrf;
     }

   /***** Get tab to activate *****/
   Gbl.CurrentTab = TabUnk;
   if (Gbl.CurrentAct == ActMnu)
     {
      Par_GetParToText ("NxtTab",UnsignedStr,10);
      if (UnsignedStr[0])
	 if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
	    if (UnsignedNum < Tab_NUM_TABS)
	      {
	       Gbl.CurrentTab = (Act_Tab_t) UnsignedNum;
	       Tab_DisableIncompatibleTabs ();
	      }
     }
   else	// Set tab depending on current action
      Tab_SetCurrentTab ();
  }

/*****************************************************************************/
/******* Get the value of a single parameter and change format to text *******/
/*****************************************************************************/
// Return the number of parameters found

unsigned Par_GetParToText (const char *ParamName,char *ParamValue,size_t MaxBytes)
  {
   return Par_GetParAndChangeFormat (ParamName,ParamValue,MaxBytes,
                                     Str_TO_TEXT,true);
  }

/*****************************************************************************/
/*** Get value of a single parameter and change format to not rigorous HTML **/
/*****************************************************************************/
// Return the number of parameters found

unsigned Par_GetParToHTML (const char *ParamName,char *ParamValue,size_t MaxBytes)
  {
   return Par_GetParAndChangeFormat (ParamName,ParamValue,MaxBytes,
                                     Str_TO_HTML,true);
  }

/*****************************************************************************/
/****** Get the value of a multiple parameter and change format to text ******/
/*****************************************************************************/
// Return the number of parameters found

unsigned Par_GetParMultiToText (const char *ParamName,char *ParamValue,size_t MaxBytes)
  {
   unsigned NumTimes = Par_GetParameter (Par_PARAM_MULTIPLE,ParamName,
                                         ParamValue,MaxBytes);
   Str_ChangeFormat (Str_FROM_FORM,Str_TO_TEXT,
                     ParamValue,MaxBytes,true);
   return NumTimes;
  }

/*****************************************************************************/
/*********** Get the value of a single parameter and change format ***********/
/*****************************************************************************/
// Return the number of parameters found

unsigned Par_GetParAndChangeFormat (const char *ParamName,char *ParamValue,size_t MaxBytes,
                                    Str_ChangeTo_t ChangeTo,bool RemoveLeadingAndTrailingSpaces)
  {
   unsigned NumTimes = Par_GetParameter (Par_PARAM_SINGLE,ParamName,
                                         ParamValue,MaxBytes);
   Str_ChangeFormat (Str_FROM_FORM,ChangeTo,
                     ParamValue,MaxBytes,RemoveLeadingAndTrailingSpaces);
   return NumTimes;
  }

/*****************************************************************************/
/************************* Get the value of a parameter **********************/
/*****************************************************************************/
// Return the number of parameters found

#define Par_LENGTH_OF_STR_BEFORE_PARAM	  38	// Length of "CONTENT-DISPOSITION: FORM-DATA; NAME=\""
#define Par_MAX_BYTES_STR_AUX		1024

static unsigned Par_GetParameter (tParamType ParamType,const char *ParamName,
                                  char *ParamValue,size_t MaxBytes)
  {
   static const char *StringBeforeParam = "CONTENT-DISPOSITION: FORM-DATA; NAME=\"";
   size_t BytesToCopy;
   size_t BytesAlreadyCopied = 0;
   int Ch;
   char *PtrSrc;
   char *PtrDst;
   char *PtrStartOfParam;
   int Result;
   unsigned NumTimes = 0;
   bool ParamFound = false;
   bool ContinueSearching = true;
   unsigned ParamNameLength;
   char StrAux[Par_MAX_BYTES_STR_AUX+1];

   ParamValue[0] = '\0'; // By default, the value of the parameter will be an empty string
   ParamNameLength = strlen (ParamName);

   switch (Gbl.ContentReceivedByCGI)
     {
      case Act_CONTENT_NORM:
         if (Gbl.GetMethod)			// Only some selected parameters can be passed by GET method
           {
            if (strcmp (ParamName,"cty") &&	// To enter directly to a country
                strcmp (ParamName,"ins") &&	// To enter directly to an institution
                strcmp (ParamName,"ctr") &&	// To enter directly to a centre
        	strcmp (ParamName,"deg") &&	// To enter directly to a degree
                strcmp (ParamName,"crs") &&	// To enter directly to a course
                strcmp (ParamName,"CrsCod") &&	// To enter directly to a course (allowed for compatibility with old links, to be removed in 2016)
                strcmp (ParamName,"usr") &&	// To enter directly to a user
                strcmp (ParamName,"act") &&	// To execute directly an action (allowed only for fully public actions)
                strcmp (ParamName,"ses") &&	// To use an open session when redirecting from one language to another
                strcmp (ParamName,"key"))	// To verify an email address
                // strcmp (ParamName,"Layout") && // To change the layout of the page (wide or narrow)
	       return 0;	// Return no-parameters-found when method is GET and parameter name is not one of these
           }
         PtrSrc = Gbl.QueryString;
         PtrDst = ParamValue;
         for (NumTimes = 0;
              NumTimes < 1 || ParamType == Par_PARAM_MULTIPLE;
              NumTimes++)
	   {
            ParamFound = false;
            do
              {
               /* If method is GET ==> do case insensitive comparison */
               PtrStartOfParam = strstr (PtrSrc,ParamName);
               if (PtrStartOfParam)
                 {
	          // String ParamName found inside Gbl.QueryString
                  if (*(PtrStartOfParam + ParamNameLength) == '=')
                    {
		     // Just after the name of the parameter, must be found a '=' symbol
                     if (PtrStartOfParam == Gbl.QueryString)	// The parameter is just at start
                        ParamFound = true;
                     else if (*(PtrStartOfParam - 1) == '&')		// The parameter is not at start, but just after an "&" separator
                        ParamFound = true;
                     else						// String has been found at the end of another parameter
                        PtrSrc = PtrStartOfParam + ParamNameLength;
                    }
                  else						// String has been found, but it is not a parameter
                     PtrSrc = PtrStartOfParam + ParamNameLength;
                 }
              }
            while (PtrStartOfParam != NULL && !ParamFound);
            if (!ParamFound)	// Not found ==> PtrStartOfParam == NULL
               break;
            PtrSrc = PtrStartOfParam + ParamNameLength + 1;	// Add 1 due to the '=' symbol
	    if (NumTimes)
	      {
	       if (BytesAlreadyCopied + 1 > MaxBytes)
                 {
	          sprintf (Gbl.Message,"Multiple parameter <strong>%s</strong> too large,"
	                               " it exceed the maximum allowed size (%lu bytes).",
	                   ParamName,(unsigned long) MaxBytes);
	          Lay_ShowErrorAndExit (Gbl.Message);
                 }
	       *PtrDst++ = Par_SEPARATOR_PARAM_MULTIPLE;		// Separator in the destination string
	       BytesAlreadyCopied++;
	      }
	    BytesToCopy = strcspn (PtrSrc,"&");	// The & charecter is the separator of two parameters
	    if (BytesAlreadyCopied + BytesToCopy > MaxBytes)
              {
               sprintf (Gbl.Message,"Parameter <strong>%s</strong> too large,"
                                    " it exceed the maximum allowed size (%lu bytes).",
                        ParamName,(unsigned long) MaxBytes);
	       Lay_ShowErrorAndExit (Gbl.Message);
              }
	    strncpy (PtrDst,PtrSrc,BytesToCopy);
	    BytesAlreadyCopied += BytesToCopy;
	    PtrDst += BytesToCopy;
            PtrSrc += BytesToCopy;
           }
         *PtrDst = '\0'; // strncpy() does not add the final NULL
         break;
      case Act_CONTENT_DATA:
         rewind (Gbl.F.Tmp);
         while (ContinueSearching)
           {
            Result = Str_ReceiveFileUntilDelimitStr (Gbl.F.Tmp,NULL,StrAux,Gbl.DelimiterString,(unsigned long long) Par_MAX_BYTES_STR_AUX);
            switch (Result)
              {
               case -1:	// Delimiter string not found
                  ContinueSearching = false;
                  break;
               case 0:	// Par_MAX_BYTES_STR_AUX exceeded
                  ContinueSearching = false;
                  break;
               case 1:	// Delimiter string found
	          /* Skip carriage returns, spaces, etc. */
	          do
	             Ch = fgetc (Gbl.F.Tmp);
	          while (isspace (Ch) && Ch != EOF);
                  if (Ch == (int) StringBeforeParam[0])
                     if (!strcasecmp (Str_GetNextStrFromFileConvertingToLower (Gbl.F.Tmp,StrAux,Par_LENGTH_OF_STR_BEFORE_PARAM-1),StringBeforeParam+1)) // Start of a parameter
                        if (!strcasecmp (Str_GetNextStrFromFileConvertingToLower (Gbl.F.Tmp,StrAux,ParamNameLength),ParamName)) // Parameter found
                          {
 	                   /* Skip "'" symbol after parameter name */
	                   Ch = fgetc (Gbl.F.Tmp);
	                   if (Ch != (int) '\"')
                             {
                              sprintf (Gbl.Message,"Error reading parameter <strong>%s</strong>."
                                                   " A <strong>&quot;</strong> character was expected, but a <strong>%c</strong> (ASCII code %d) has been found.",
                                       ParamName,(char) Ch,Ch);
	                      Lay_ShowErrorAndExit (Gbl.Message);
                             }

	                   /* Skip carriage returns, spaces, etc. */
	                   do
	                      Ch = fgetc (Gbl.F.Tmp);
	                   while (isspace (Ch) && Ch != EOF);

	                   /* Get the parameter */
	                   ParamValue[0] = Ch;
	                   Result = Str_ReceiveFileUntilDelimitStr (Gbl.F.Tmp,(FILE *) NULL,&ParamValue[1],Gbl.DelimiterStringIncludingInitialRet,(unsigned long long) MaxBytes);

	                   /* Depending on the result... */
                           switch (Result)
                             {
                              case -1:	// Delimiter string not found
	                         sprintf (Gbl.Message,"Parameter <strong>%s</strong> has not been readed properly.",
	                                  ParamName);
	                         Lay_ShowErrorAndExit (Gbl.Message);
                                 break;
                              case 0:	// Parameter's too long
	                         sprintf (Gbl.Message,"Parameter <strong>%s</strong> exceed the maximum allowed size (%lu bytes).",
	                                  ParamName,(unsigned long) MaxBytes);
	                         Lay_ShowErrorAndExit (Gbl.Message);
                                 break;
	             	      case 1:	// Delimiter string and parameter value found
                                 ParamFound = true;
                                 ContinueSearching = false;
                                 NumTimes = 1;
                                 break;
	                     }
                          }
               break;
              }
           }
         break;
     }

   return NumTimes;
  }

/*****************************************************************************/
/***** Search in the string StrSrc the next string until find separator ******/
/*****************************************************************************/
// Modifies *StrSrc
// When StrDst is NULL, nothing is stored
// Return true if characters found

bool Par_GetNextStrUntilSeparParamMult (const char **StrSrc,char *StrDst,size_t LongMax)
  {
   size_t i = 0;
   unsigned char Ch;	// Must be unsigned to work with characters > 127
   bool CharsFound = false;

   do
      if ((Ch = (unsigned char) **StrSrc))
	 (*StrSrc)++;
   while (Ch && Ch < 32); // Skip special characters (the separator will be a special character less than 32)

   while (Ch >= 32)
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
/************ Replace any character between 1 and 31 by a comma **************/
/*****************************************************************************/

void Par_ReplaceSeparatorMultipleByComma (const char *StrSrc,char *StrDst)
  {
   for (;
	*StrSrc;
	StrSrc++, StrDst++)
      *StrDst = ((unsigned char) *StrSrc < 32) ? ',' :
	                                         *StrSrc;
   *StrDst = '\0';
  }

/*****************************************************************************/
/******** Replace each comma by the separator of multiple parameters *********/
/*****************************************************************************/

void Par_ReplaceCommaBySeparatorMultiple (char *Str)
  {
   for (;
	*Str;
	Str++)
      if (*Str == ',')
	 *Str = Par_SEPARATOR_PARAM_MULTIPLE;
  }

/*****************************************************************************/
/********************** Put an unsigned hidden parameter *********************/
/*****************************************************************************/

void Par_PutHiddenParamUnsigned (const char *ParamName,unsigned Value)
  {
   fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"%s\" value=\"%u\" />",
            ParamName,Value);
  }

/*****************************************************************************/
/************************* Put a long hidden parameter ***********************/
/*****************************************************************************/

void Par_PutHiddenParamLong (const char *ParamName,long Value)
  {
   fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"%s\" value=\"%ld\" />",
            ParamName,Value);
  }

/*****************************************************************************/
/************************* Put a char hidden parameter ***********************/
/*****************************************************************************/

void Par_PutHiddenParamChar (const char *ParamName,char Value)
  {
   fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"%s\" value=\"%c\" />",
            ParamName,Value);
  }

/*****************************************************************************/
/************************ Put a string hidden parameter **********************/
/*****************************************************************************/

void Par_PutHiddenParamString (const char *ParamName,const char *Value)
  {
   fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"%s\" value=\"%s\" />",
            ParamName,Value);
  }
