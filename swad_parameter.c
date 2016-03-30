// swad_parameter.c: CGI parameters

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <ctype.h>		// For isprint, isspace, etc.
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_config.h"

#include "swad_database.h"	// TODO: remove when debug ends

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

/*****************************************************************************/
/****************************** Private variables ****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Par_GetParameters (void);

static void Par_ShowErrorReadingParam (const char *ParamName,const char *ExpectedChar,int Ch);

/*****************************************************************************/
/*** Read all parameters passed to this CGI and store for later processing ***/
/*****************************************************************************/

bool Par_GetQueryString (void)
  {
   char Method[256];
   char ContentType[512];
   char UnsignedLongStr[10+1];

   strcpy (Method,getenv ("REQUEST_METHOD"));

   if (!strcmp (Method,"GET"))
     {
      /***** GET method *****/
      Gbl.Params.GetMethod = true;
      Gbl.ContentReceivedByCGI = Act_CONTENT_NORM;

      /* Get content length */
      Gbl.Params.ContentLength = strlen (getenv ("QUERY_STRING"));

      /* Allocate memory for query string */
      if ((Gbl.Params.QueryString = (char *) malloc (Gbl.Params.ContentLength + 1)) == NULL)
	 return false;

      /* Copy query string from environment variable */
      strcpy (Gbl.Params.QueryString,getenv ("QUERY_STRING"));
     }
   else
     {
      /***** PUSH method *****/
      /* Get content length */
      if (getenv ("CONTENT_LENGTH"))
	{
         strcpy (UnsignedLongStr,getenv ("CONTENT_LENGTH"));
         if (sscanf (UnsignedLongStr,"%lu",&Gbl.Params.ContentLength) != 1)
            return false;
	}
      else
         return false;

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

	 /* Allocate memory for query string */
	 if ((Gbl.Params.QueryString = (char *) malloc (Gbl.Params.ContentLength + 1)) == NULL)
	    return false;

	 /* Copy query string from stdin */
         if (fread ((void *) Gbl.Params.QueryString,sizeof (char),Gbl.Params.ContentLength,stdin) != Gbl.Params.ContentLength)
           {
            Gbl.Params.QueryString[0] = '\0';
            return false;
           }
	 Gbl.Params.QueryString[Gbl.Params.ContentLength] = '\0';
        }
     }

   if (Gbl.ContentReceivedByCGI == Act_CONTENT_NORM)
      Par_GetParameters ();

   return true;
  }

/*****************************************************************************/
/***************** Free memory allocated for query string ********************/
/*****************************************************************************/

void Par_FreeQueryString (void)
  {
   if (Gbl.Params.QueryString)
      free ((void *) Gbl.Params.QueryString);
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
   char Nickname[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA+1];
   char LongStr[1+10+1];
   char YearStr[2+1];

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
      Gbl.Action.Act = ActWebSvc;
      Tab_SetCurrentTab ();
      return;
     }

   /* Check if SWAD is called from an external site */
   Imp_GetImpUsrAndSession ();
   if (Gbl.Imported.ExternalUsrId[0] &&
       Gbl.Imported.ExternalSesId[0])
     {
      Gbl.Action.Act = ActAutUsrExt;
      Tab_SetCurrentTab ();
      return;
     }
   // SWAD is not called from external site

   /***** Set dfault action *****/
   Gbl.Action.Act = ActUnk;

   /***** Get another user's nickname, if exists
          (this nickname is used to go to another user's profile,
           not to get the logged user) *****/
   if (Par_GetParToText ("usr",Nickname,Nck_MAX_BYTES_NICKNAME_WITH_ARROBA))
      if (Nickname[0])
	{
	 // This user's code is used to go to public profile
	 // and to refresh old publishings in user's timeline
	 // If user does not exist ==> UsrCod = -1
	 Gbl.Usrs.Other.UsrDat.UsrCod = Nck_GetUsrCodFromNickname (Nickname);
	 Gbl.Action.Act = ActSeePubPrf;	// Set default action if no other is specified
	}

   /***** Get action to perform *****/
   Par_GetParToText ("act",UnsignedStr,10);
   if (UnsignedStr[0])
     {
      if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
         if (UnsignedNum <= Act_MAX_ACTION_COD)
            Gbl.Action.Act = Act_FromActCodToAction[UnsignedNum];
     }
   else
     {
      // Try old parameter "ActCod" (allowed for compatibility, to be removed soon)
      Par_GetParToText ("ActCod",UnsignedStr,10);
      if (UnsignedStr[0])
	 if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
	    if (UnsignedNum <= Act_MAX_ACTION_COD)
	       Gbl.Action.Act = Act_FromActCodToAction[UnsignedNum];
     }
   if (Gbl.Action.Act == ActRefCon          ||
       Gbl.Action.Act == ActRefLstClk       ||
       Gbl.Action.Act == ActRefNewSocPubGbl ||
       Gbl.Action.Act == ActRefOldSocPubGbl ||
       Gbl.Action.Act == ActRefOldSocPubUsr)
      Gbl.Action.UsesAJAX = true;

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
   switch (Gbl.Action.Act)
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
   Pre_GetPrefsFromIP ();

   if (!Gbl.Session.IsOpen)	// When no session open (no logged user)...
     {
      /***** Try to get preferences changed from current IP *****/
      if (Gbl.Prefs.Theme == The_THEME_UNKNOWN)
         Gbl.Prefs.Theme = The_THEME_DEFAULT;

      /***** Set path of theme *****/
      sprintf (Gbl.Prefs.PathTheme,"%s/%s/%s",
               Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_THEMES,The_ThemeId[Gbl.Prefs.Theme]);

      /***** Set path of icon set *****/
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

   /***** Get tab to activate *****/
   Gbl.Action.Tab = TabUnk;
   if (Gbl.Action.Act == ActMnu)
     {
      Par_GetParToText ("NxtTab",UnsignedStr,10);
      if (UnsignedStr[0])
	 if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
	    if (UnsignedNum < Tab_NUM_TABS)
	      {
	       Gbl.Action.Tab = (Act_Tab_t) UnsignedNum;
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

static void Par_GetParameters (void)
  {
   unsigned long i;
   struct Param *Param;
   struct Param *NewParam;

   switch (Gbl.ContentReceivedByCGI)
     {
      case Act_CONTENT_NORM:
	 if (Gbl.Params.QueryString == NULL)
	   {
	    Gbl.Params.List = NULL;
	    return;
	   }
	 if (!Gbl.Params.QueryString[0])
	   {
	    Gbl.Params.List = NULL;
	    return;
	   }

         for (i = 0;
              i < Gbl.Params.ContentLength;
              )
           {
            /* Allocate space for parameter */
            if ((NewParam = (struct Param *) malloc (sizeof (struct Param))) == NULL)
               Lay_ShowErrorAndExit ("Error allocating memory for parameter");

            /* Point last element in list to this */
            if (i == 0)
               Gbl.Params.List = NewParam;
            else
               Param->Next = NewParam;

            /* Point current element to the new just created */
            Param = NewParam;
            Param->Next = NULL;

            /* Get parameter name */
            Param->Name.Start = i;
            Param->Name.Length = strcspn (&Gbl.Params.QueryString[i],"=");

            /* Get parameter value */
            i += Param->Name.Length;
            if (i < Gbl.Params.ContentLength)
              {
               if (Gbl.Params.QueryString[i] == '=')
		 {
		  i++;	// Skip '='
		  Param->Value.Start = i;
		  if (i < Gbl.Params.ContentLength)
		    {
                     Param->Value.Length = strcspn (&Gbl.Params.QueryString[i],"&");
                     i += Param->Value.Length;
                     if (i < Gbl.Params.ContentLength)
			if (Gbl.Params.QueryString[i] == '&')
			   i++;	// Skip '&'
		    }
		  else
                     Param->Value.Length = 0;
		 }
               else
        	 {
		  Param->Value.Start = i;
                  Param->Value.Length = 0;
        	 }
              }
            else
	      {
	       Param->Value.Start = i;
	       Param->Value.Length = 0;
	      }
           }
         break;
      case Act_CONTENT_DATA:
	 // TODO: Implement
         break;
     }
  }

/*****************************************************************************/
/************************* Get the value of a parameter **********************/
/*****************************************************************************/
// Return the number of parameters found

#define Par_LENGTH_OF_STR_BEFORE_PARAM	  38	// Length of "CONTENT-DISPOSITION: FORM-DATA; NAME=\""
#define Par_MAX_BYTES_STR_AUX		1024

unsigned Par_GetParameter (tParamType ParamType,const char *ParamName,
                           char *ParamValue,size_t MaxBytes)
  {
   static const char *StringBeforeParam = "Content-Disposition: form-data; name=\"";
   size_t BytesToCopy;
   size_t BytesAlreadyCopied = 0;
   int Ch;
   unsigned i;
   struct Param *Param;
   char *PtrDst;
   const char *PtrSrc = NULL;
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
         if (Gbl.Params.GetMethod)			// Only some selected parameters can be passed by GET method
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
	       return 0;	// Return no-parameters-found when method is GET and parameter name is not one of these
           }

         PtrDst = ParamValue;
         Param = Gbl.Params.List;
         for (NumTimes = 0;
              NumTimes < 1 || ParamType == Par_PARAM_MULTIPLE;
              NumTimes++)
	   {
            for (ParamFound = false;
        	 Param != NULL && !ParamFound;
        	 Param = Param->Next)
               if (ParamNameLength == Param->Name.Length)
                  if (!strncmp (ParamName,&Gbl.Params.QueryString[Param->Name.Start],
                                Param->Name.Length))
                    {
        	     ParamFound = true;
                     if ((BytesToCopy = Param->Value.Length))
                        PtrSrc = &Gbl.Params.QueryString[Param->Value.Start];
                    }

            if (!ParamFound)
               break;

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

	    if (BytesAlreadyCopied + BytesToCopy > MaxBytes)
              {
               sprintf (Gbl.Message,"Parameter <strong>%s</strong> too large,"
                                    " it exceed the maximum allowed size (%lu bytes).",
                        ParamName,(unsigned long) MaxBytes);
	       Lay_ShowErrorAndExit (Gbl.Message);
              }
	    if (BytesToCopy)
	      {
	       strncpy (PtrDst,PtrSrc,BytesToCopy);
	       BytesAlreadyCopied += BytesToCopy;
	       PtrDst += BytesToCopy;
	      }
           }
         *PtrDst = '\0'; // strncpy() does not add the final NULL
         break;
      case Act_CONTENT_DATA:
         rewind (Gbl.F.Tmp);

         while (ContinueSearching)
           {
            Result = Str_ReceiveFileUntilDelimitStr (Gbl.F.Tmp,
                                                     (FILE *) NULL,(char *) NULL,
                                                     Gbl.DelimiterString,
                                                     Fil_MAX_FILE_SIZE);
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
                    {
                     Str_GetNextStrFromFileConvertingToLower (Gbl.F.Tmp,StrAux,Par_LENGTH_OF_STR_BEFORE_PARAM-1);
                     if (!strcasecmp (StrAux,StringBeforeParam+1)) // Start of a parameter
                       {
                	/* Check if parameter is the parameter that we are looking for */
                	Str_GetNextStrFromFileConvertingToLower (Gbl.F.Tmp,StrAux,ParamNameLength);
                        if (!strcasecmp (StrAux,ParamName)) // Parameter found
                          {
 	                   /* Skip quote after parameter name */
	                   Ch = fgetc (Gbl.F.Tmp);
	                   if (Ch != (int) '\"')
	                      Par_ShowErrorReadingParam (ParamName,"&quot;",Ch);

	                   /* Skip two CR-LF (0x0D 0x0A) */
	                   for (i = 0;
	                	i < 2;
	                	i++)
	                     {
			      Ch = fgetc (Gbl.F.Tmp);
			      if (Ch != 0x0D)	// '\r'
				 Par_ShowErrorReadingParam (ParamName,"0x0D",Ch);
			      Ch = fgetc (Gbl.F.Tmp);
			      if (Ch != 0x0A)	// '\n'
				 Par_ShowErrorReadingParam (ParamName,"0x0A",Ch);
	                     }

	                   /* Get the parameter */
	                   Result = Str_ReceiveFileUntilDelimitStr (Gbl.F.Tmp,
	                                                            (FILE *) NULL,ParamValue,
	                                                            Gbl.DelimiterStringIncludingInitialRet,
	                                                            (unsigned long long) MaxBytes);

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
/********************** Write error reading parameter ************************/
/*****************************************************************************/

static void Par_ShowErrorReadingParam (const char *ParamName,const char *ExpectedChar,int Ch)
  {
   sprintf (Gbl.Message,"Error reading parameter <strong>%s</strong>."
			" A <strong>%s</strong> character was expected,"
			" but a character with code %X has been found.",
	    ParamName,ExpectedChar,Ch);
   Lay_ShowErrorAndExit (Gbl.Message);
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
