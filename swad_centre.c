// swad_centre.c: centres

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_bool.h"
#include "swad_centre.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_institution.h"
#include "swad_logo.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_string.h"
#include "swad_text.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

// Centre photo must be stored in Ctr_PHOTO_REAL_WIDTH x Ctr_PHOTO_REAL_HEIGHT
// Aspect ratio: 3:2 (1.5)
#define Ctr_PHOTO_REAL_WIDTH	768
#define Ctr_PHOTO_REAL_HEIGHT	512

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ctr_Configuration (bool PrintView);

static void Ctr_ListCentres (void);
static void Ctr_ListCentresForSeeing (void);
static void Ctr_ListOneCentreForSeeing (struct Centre *Ctr,unsigned NumCtr);
static void Ctr_GetParamCtrOrderType (void);
static void Ctr_GetPhotoAttribution (long CtrCod,char **PhotoAttribution);
static void Ctr_FreePhotoAttribution (char **PhotoAttribution);
static void Ctr_ListCentresForEdition (void);
static bool Ctr_CheckIfICanEdit (struct Centre *Ctr);
static Ctr_StatusTxt_t Ctr_GetStatusTxtFromStatusBits (Ctr_Status_t Status);
static Ctr_Status_t Ctr_GetStatusBitsFromStatusTxt (Ctr_StatusTxt_t StatusTxt);
static void Ctr_PutParamOtherCtrCod (long CtrCod);
static void Ctr_RenameCentre (Cns_ShortOrFullName_t ShortOrFullName);
static bool Ctr_CheckIfCentreNameExistsInCurrentIns (const char *FieldName,const char *Name,long CtrCod);

static void Ctr_PutFormToChangeCtrPhoto (bool PhotoExists);
static void Ctr_PutFormToCreateCentre (void);
static void Ctr_PutHeadCentresForSeeing (bool OrderSelectable);
static void Ctr_PutHeadCentresForEdition (void);
static void Ctr_RecFormRequestOrCreateCtr (unsigned Status);
static void Ctr_CreateCentre (struct Centre *Ctr,unsigned Status);

/*****************************************************************************/
/******************* List centres with pending degrees ***********************/
/*****************************************************************************/

void Ctr_SeeCtrWithPendingDegs (void)
  {
   extern const char *Txt_Centres_with_pending_degrees;
   extern const char *Txt_Centre;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_Go_to_X;
   extern const char *Txt_There_are_no_centres_with_requests_for_degrees_to_be_confirmed;
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtrs;
   unsigned NumCtr;
   struct Centre Ctr;
   const char *BgColor;

   /***** Get centres with pending degrees *****/
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_CTR_ADM:
         sprintf (Query,"SELECT degrees.CtrCod,COUNT(*)"
                        " FROM degrees,ctr_admin,centres"
                        " WHERE (degrees.Status & %u)<>0"
                        " AND degrees.CtrCod=ctr_admin.CtrCod AND ctr_admin.UsrCod='%ld'"
                        " AND degrees.CtrCod=centres.CtrCod"
                        " GROUP BY degrees.CtrCod ORDER BY centres.ShortName",
                  (unsigned) Deg_STATUS_BIT_PENDING,Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Rol_ROLE_SYS_ADM:
         sprintf (Query,"SELECT degrees.CtrCod,COUNT(*)"
                        " FROM degrees,centres"
                        " WHERE (degrees.Status & %u)<>0"
                        " AND degrees.CtrCod=centres.CtrCod"
                        " GROUP BY degrees.CtrCod ORDER BY centres.ShortName",
                  (unsigned) Deg_STATUS_BIT_PENDING);
         break;
      default:	// Forbidden for other users
	 return;
     }

   /***** Get centres *****/
   if ((NumCtrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get centres with pending degrees")))
     {
      /***** Write heading *****/
      Lay_StartRoundFrameTable10 (NULL,2,Txt_Centres_with_pending_degrees);
      fprintf (Gbl.F.Out,"<tr>"
                         "<th></th>"
                         "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                         "%s"
                         "</th>"
                         "<th class=\"TIT_TBL\" style=\"text-align:right;\">"
                         "%s"
                         "</th>"
                         "</tr>",
               Txt_Centre,
               Txt_Degrees_ABBREVIATION);

      /***** List the centres *****/
      for (NumCtr = 0;
	   NumCtr < NumCtrs;
	   NumCtr++)
        {
         /* Get next centre */
         row = mysql_fetch_row (mysql_res);

         /* Get centre code (row[0]) */
         Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[0]);
         BgColor = (Ctr.CtrCod == Gbl.CurrentCtr.Ctr.CtrCod) ? VERY_LIGHT_BLUE :
	                                                       Gbl.ColorRows[Gbl.RowEvenOdd];

         /* Get data of centre */
         Ctr_GetDataOfCentreByCod (&Ctr);

         /* Centre logo */
         fprintf (Gbl.F.Out,"<tr>"
	                    "<td class=\"DAT\" style=\"text-align:center;"
	                    " vertical-align:middle; background-color:%s;\">"
                            "<a href=\"%s\" title=\"%s\" class=\"DAT\" target=\"_blank\">",
                  BgColor,Ctr.WWW,Ctr.FullName);
         Log_DrawLogo (Sco_SCOPE_CTR,Ctr.CtrCod,Ctr.ShortName,
                       16,"vertical-align:top;",true);
         fprintf (Gbl.F.Out,"</a>"
                            "</td>");

         /* Centre full name */
         fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:left;"
	                    " vertical-align:middle; background-color:%s;\">",
                  BgColor);
         Act_FormGoToStart (ActSeeDeg);
         Ctr_PutParamCtrCod (Ctr.CtrCod);
         sprintf (Gbl.Title,Txt_Go_to_X,Ctr.FullName);
         Act_LinkFormSubmit (Gbl.Title,"DAT");
         fprintf (Gbl.F.Out,"%s"
                            "</a>"
                            "</form>"
                            "</td>",
	          Ctr.FullName);

         /* Number of pending degrees (row[1]) */
         fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:right;"
	                    " vertical-align:middle; background-color:%s;\">"
	                    "%s"
	                    "</td>"
	                    "</tr>",
                  BgColor,row[1]);

         Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
        }

      Lay_EndRoundFrameTable10 ();
     }
   else
      Lay_ShowAlert (Lay_INFO,Txt_There_are_no_centres_with_requests_for_degrees_to_be_confirmed);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Show information of the current centre *******************/
/*****************************************************************************/

void Ctr_ShowConfiguration (void)
  {
   Ctr_Configuration (false);
  }

/*****************************************************************************/
/****************** Print information of the current centre ******************/
/*****************************************************************************/

void Ctr_PrintConfiguration (void)
  {
   Ctr_Configuration (true);
  }

/*****************************************************************************/
/******************* Information of the current centre ***********************/
/*****************************************************************************/

static void Ctr_Configuration (bool PrintView)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Degrees;
   extern const char *Txt_Centre;
   extern const char *Txt_Short_Name;
   extern const char *Txt_Shortcut_to_this_centre;
   extern const char *Txt_QR_code;
   extern const char *Txt_Courses;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   char PathPhoto[PATH_MAX+1];
   bool PhotoExists;
   char *PhotoAttribution = NULL;
   bool PutLink = !PrintView && Gbl.CurrentCtr.Ctr.WWW[0];

   if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
     {
      /***** Path to photo *****/
      sprintf (PathPhoto,"%s/%s/%02u/%u/%u.jpg",
               Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CTR,
	       (unsigned) (Gbl.CurrentCtr.Ctr.CtrCod % 100),
	       (unsigned) Gbl.CurrentCtr.Ctr.CtrCod,
	       (unsigned) Gbl.CurrentCtr.Ctr.CtrCod);
      PhotoExists = Fil_CheckIfPathExists (PathPhoto);

      /***** Links to show degrees, to print view
             and to upload photo and logo *****/
      fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");

      /* Link to show degrees */
      Act_FormStart (ActSeeDeg);
      Act_LinkFormSubmit (Txt_Degrees,The_ClassFormul[Gbl.Prefs.Theme]);
      Lay_PutSendIcon ("deg",Txt_Degrees,Txt_Degrees);
      fprintf (Gbl.F.Out,"</form>");

      if (!PrintView)
	{
	  /* Link to print view */
	 Lay_PutLinkToPrintView1 (ActPrnCtrInf);
	 Lay_PutLinkToPrintView2 ();

	 /* Links to upload logo and photo */
	 if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_CTR_ADM)
	   {
	    Log_PutFormToChangeLogo (Sco_SCOPE_CTR);
	    Ctr_PutFormToChangeCtrPhoto (PhotoExists);
	   }
	}

      fprintf (Gbl.F.Out,"</div>");

      /***** Start frame *****/
      Lay_StartRoundFrameTable10 (NULL,2,NULL);

      /***** Title *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td colspan=\"2\" class=\"TITLE_LOCATION\""
	                 " style=\"text-align:center;\">");
      if (PutLink)
	 fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
	                    " class=\"TITLE_LOCATION\" title=\"%s\">",
		  Gbl.CurrentCtr.Ctr.WWW,
		  Gbl.CurrentCtr.Ctr.FullName);
      Log_DrawLogo (Sco_SCOPE_CTR,Gbl.CurrentCtr.Ctr.CtrCod,
                    Gbl.CurrentCtr.Ctr.ShortName,64,NULL,true);
      fprintf (Gbl.F.Out,"<br />%s",Gbl.CurrentCtr.Ctr.FullName);
      if (PutLink)
	 fprintf (Gbl.F.Out,"</a>");
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      /***** Centre photo *****/
      if (PhotoExists)
	{
	 /* Get photo attribution */
	 Ctr_GetPhotoAttribution (Gbl.CurrentCtr.Ctr.CtrCod,&PhotoAttribution);

	 /* Photo image */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td colspan=\"2\" class=\"DAT_SMALL\""
			    " style=\"text-align:center;\">");
	 if (PutLink)
	    fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"DAT_N\">",
		     Gbl.CurrentCtr.Ctr.WWW);
	 fprintf (Gbl.F.Out,"<img src=\"%s/%s/%02u/%u/%u.jpg\" class=\"%s\" />",
		  Cfg_HTTPS_URL_SWAD_PUBLIC,Cfg_FOLDER_CTR,
		  (unsigned) (Gbl.CurrentCtr.Ctr.CtrCod % 100),
		  (unsigned) Gbl.CurrentCtr.Ctr.CtrCod,
		  (unsigned) Gbl.CurrentCtr.Ctr.CtrCod,
		  PrintView ? "CENTRE_PHOTO_PRINT" :
			      "CENTRE_PHOTO_SHOW");
	 if (PutLink)
	    fprintf (Gbl.F.Out,"</a>");
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /* Photo attribution */
	 if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM && !PrintView)
	   {
	    fprintf (Gbl.F.Out,"<tr>"
			       "<td colspan=\"2\""
			       " style=\"text-align:center;\">");
	    Act_FormStart (ActChgCtrPhoAtt);
	    fprintf (Gbl.F.Out,"<textarea name=\"Attribution\" cols=\"50\" rows=\"2\""
			       " onchange=\"javascript:document.getElementById('%s').submit();\">",
		     Gbl.FormId);
            if (PhotoAttribution)
	       fprintf (Gbl.F.Out,"%s",PhotoAttribution);
	    fprintf (Gbl.F.Out,"</textarea>"
		               "</form>"
		               "</td>"
		               "</tr>");
	   }
	 else if (PhotoAttribution)
	    fprintf (Gbl.F.Out,"<tr>"
			       "<td colspan=\"2\" class=\"ATTRIBUTION\""
			       " style=\"text-align:center;\">"
			       "%s"
			       "</td>"
			       "</tr>",
		     PhotoAttribution);

	 /* Free memory used for photo attribution */
	 Ctr_FreePhotoAttribution (&PhotoAttribution);
	}

      /***** Centre full name *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s\""
			 " style=\"text-align:right; vertical-align:middle;\">"
	                 "%s:"
	                 "</td>"
			 "<td class=\"DAT_N\""
			 " style=\"text-align:left; vertical-align:middle;\">",
	       The_ClassFormul[Gbl.Prefs.Theme],
	       Txt_Centre);
      if (PutLink)
	 fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"DAT_N\">",
		  Gbl.CurrentCtr.Ctr.WWW);
      fprintf (Gbl.F.Out,"%s",Gbl.CurrentCtr.Ctr.FullName);
      if (PutLink)
	 fprintf (Gbl.F.Out,"</a>");
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      /***** Centre short name *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s\""
			 " style=\"text-align:right; vertical-align:middle;\">"
			 "%s:"
			 "</td>"
			 "<td class=\"DAT\""
			 " style=\"text-align:left; vertical-align:middle;\">"
			 "%s"
			 "</td>"
			 "</tr>",
	       The_ClassFormul[Gbl.Prefs.Theme],
	       Txt_Short_Name,
	       Gbl.CurrentCtr.Ctr.ShortName);

      /***** Link to the centre *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s\""
			 " style=\"text-align:right; vertical-align:middle;\">"
			 "%s:"
			 "</td>"
			 "<td class=\"DAT\""
			 " style=\"text-align:left; vertical-align:middle;\">"
			 "<a href=\"%s/?CtrCod=%ld\" class=\"DAT\" target=\"_blank\">"
			 "%s/?CtrCod=%ld"
			 "</a>"
			 "</td>"
			 "</tr>",
	       The_ClassFormul[Gbl.Prefs.Theme],
	       Txt_Shortcut_to_this_centre,
	       Cfg_HTTPS_URL_SWAD_CGI,Gbl.CurrentCtr.Ctr.CtrCod,
	       Cfg_HTTPS_URL_SWAD_CGI,Gbl.CurrentCtr.Ctr.CtrCod);

      if (PrintView)
	{
	 /***** QR code with link to the centre *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">",
		  The_ClassFormul[Gbl.Prefs.Theme],
		  Txt_QR_code);
	 QR_LinkToCentre (200);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");
	}
      else
	{
	 /***** Number of degrees *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassFormul[Gbl.Prefs.Theme],
		  Txt_Degrees,
		  Deg_GetNumDegsInCtr (Gbl.CurrentCtr.Ctr.CtrCod));

	 /***** Number of courses *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassFormul[Gbl.Prefs.Theme],
		  Txt_Courses,
		  Crs_GetNumCrssInCtr (Gbl.CurrentCtr.Ctr.CtrCod));

	 /***** Number of teachers *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassFormul[Gbl.Prefs.Theme],
		  Txt_ROLES_PLURAL_Abc[Rol_ROLE_TEACHER][Usr_SEX_UNKNOWN],
		  Usr_GetNumUsrsInCrssOfCtr (Rol_ROLE_TEACHER,Gbl.CurrentCtr.Ctr.CtrCod));

	 /***** Number of students *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s\" style=\"text-align:right;"
			    " vertical-align:middle;\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT\" style=\"text-align:left;"
			    " vertical-align:middle;\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassFormul[Gbl.Prefs.Theme],
		  Txt_ROLES_PLURAL_Abc[Rol_ROLE_STUDENT][Usr_SEX_UNKNOWN],
		  Usr_GetNumUsrsInCrssOfCtr (Rol_ROLE_STUDENT,Gbl.CurrentCtr.Ctr.CtrCod));
	}

      /***** End frame *****/
      Lay_EndRoundFrameTable10 ();
     }
  }

/*****************************************************************************/
/*************** Show the centres of the current institution *****************/
/*****************************************************************************/

void Ctr_ShowCtrsOfCurrentIns (void)
  {
   if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      /***** Get parameter with the type of order in the list of centres *****/
      Ctr_GetParamCtrOrderType ();

      /***** Get list of centres *****/
      Ctr_GetListCentres (Gbl.CurrentIns.Ins.InsCod);

      /***** Write menu to select country and institution *****/
      Deg_WriteMenuAllCourses (ActSeeIns,ActSeeCtr,ActUnk,ActUnk);

      /***** Put link (form) to edit centres in current institution *****/
      if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_GUEST__)
         Lay_PutFormToEdit (ActEdiCtr);

      /***** List centres *****/
      Ctr_ListCentres ();

      /***** Free list of centres *****/
      Ctr_FreeListCentres ();
     }
  }

/*****************************************************************************/
/******************** List centres in this institution ***********************/
/*****************************************************************************/

static void Ctr_ListCentres (void)
  {
   extern const char *Txt_No_centres_have_been_created_in_this_institution;

   if (Gbl.Ctrs.Num)
      Ctr_ListCentresForSeeing ();
   else
      Lay_ShowAlert (Lay_INFO,Txt_No_centres_have_been_created_in_this_institution);
  }

/*****************************************************************************/
/*************** List the centres of the current institution *****************/
/*****************************************************************************/

static void Ctr_ListCentresForSeeing (void)
  {
   extern const char *Txt_Centres_of_INSTITUTION_X;
   unsigned NumCtr;

   /***** Write heading *****/
   sprintf (Gbl.Title,Txt_Centres_of_INSTITUTION_X,
            Gbl.CurrentIns.Ins.FullName);
   Lay_StartRoundFrameTable10 (NULL,2,Gbl.Title);
   Ctr_PutHeadCentresForSeeing (true);	// Order selectable

   /***** Write all the centres and their nuber of teachers *****/
   for (NumCtr = 0;
	NumCtr < Gbl.Ctrs.Num;
	NumCtr++)
      Ctr_ListOneCentreForSeeing (&(Gbl.Ctrs.Lst[NumCtr]),NumCtr + 1);

   /***** Table end *****/
   Lay_EndRoundFrameTable10 ();
  }

/*****************************************************************************/
/************************* List one centre for seeing ************************/
/*****************************************************************************/

static void Ctr_ListOneCentreForSeeing (struct Centre *Ctr,unsigned NumCtr)
  {
   extern const char *Txt_Another_place;
   extern const char *Txt_Go_to_X;
   extern const char *Txt_CENTRE_STATUS[Ctr_NUM_STATUS_TXT];
   struct Place Plc;
   const char *TxtClass;
   const char *BgColor;
   Crs_StatusTxt_t StatusTxt;

   /***** Get data of place of this centre *****/
   Plc.PlcCod = Ctr->PlcCod;
   Plc_GetDataOfPlaceByCod (&Plc);

   TxtClass = (Ctr->Status & Ctr_STATUS_BIT_PENDING) ? "DAT_LIGHT" :
	                                               "DAT";
   BgColor = (Ctr->CtrCod == Gbl.CurrentCtr.Ctr.CtrCod) ? VERY_LIGHT_BLUE :
							  Gbl.ColorRows[Gbl.RowEvenOdd];

   /***** Number of centre in this list *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s\" style=\"text-align:right;"
		      " vertical-align:middle; background-color:%s;\">"
                      "%u"
                      "</td>",
	    TxtClass,BgColor,
            NumCtr);

   /***** Centre logo *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:center;"
		      " vertical-align:middle; background-color:%s;\">"
		      "<a href=\"%s\" title=\"%s\" class=\"DAT\" target=\"_blank\">",
	    TxtClass,BgColor,
	    Ctr->WWW,Ctr->FullName);
   Log_DrawLogo (Sco_SCOPE_CTR,Ctr->CtrCod,Ctr->ShortName,
                 16,"vertical-align:top;",true);
   fprintf (Gbl.F.Out,"</a>"
		      "</td>");

   /***** Place *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
		      " vertical-align:middle; background-color:%s;\">"
		      "%s"
		      "</td>",
	    TxtClass,BgColor,
	    Plc.PlcCod > 0 ? Plc.ShortName :
			     Txt_Another_place);

   /***** Centre name *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
		      " vertical-align:middle; background-color:%s;\">",
	    TxtClass,BgColor);
   Act_FormGoToStart (ActSeeCtrInf);
   Ctr_PutParamCtrCod (Ctr->CtrCod);
   sprintf (Gbl.Title,Txt_Go_to_X,Ctr->FullName);
   Act_LinkFormSubmit (Gbl.Title,TxtClass);
   fprintf (Gbl.F.Out,"%s</a>"
		      "</form>"
		      "</td>",
	    Ctr->FullName);

   /***** Number of teachers *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:right;"
		      " vertical-align:middle; background-color:%s;\">"
		      "%u"
		      "</td>",
	    TxtClass,BgColor,
	    Ctr->NumTchs);

   /***** Number of degrees *****/
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:right;"
		      " vertical-align:middle; background-color:%s;\">"
		      "%u"
		      "</td>",
	    TxtClass,BgColor,
	    Ctr->NumDegs);

   /***** Centre status *****/
   StatusTxt = Ctr_GetStatusTxtFromStatusBits (Ctr->Status);
   fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
		      " vertical-align:middle; background-color:%s;\">"
		      "%s"
		      "</td>"
		      "</tr>",
	    TxtClass,BgColor,
	    Txt_CENTRE_STATUS[StatusTxt]);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of centres **********/
/*****************************************************************************/

static void Ctr_GetParamCtrOrderType (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("Order",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      Gbl.Ctrs.SelectedOrderType = (tCtrsOrderType) UnsignedNum;
   else
      Gbl.Ctrs.SelectedOrderType = Ctr_DEFAULT_ORDER_TYPE;
  }

/*****************************************************************************/
/************************** Put forms to edit centres ************************/
/*****************************************************************************/

void Ctr_EditCentres (void)
  {
   extern const char *Txt_No_centres_have_been_created_in_this_institution;
   extern const char *Txt_There_is_no_list_of_institutions;
   extern const char *Txt_You_must_create_at_least_one_institution_before_creating_centres;

   /***** Put link (form) to view centres *****/
   Lay_PutFormToView (ActSeeCtr);

   /***** Get list of institutions of the current country *****/
   Ins_GetListInstitutions (Gbl.CurrentCty.Cty.CtyCod,Ins_GET_MINIMAL_DATA);
   if (Gbl.Inss.Num)
     {
      /***** Get list of places *****/
      Plc_GetListPlaces ();

      /***** Get list of centres *****/
      Ctr_GetListCentres (Gbl.CurrentIns.Ins.InsCod);

      /***** Help message *****/
      if (!Gbl.Ctrs.Num)
	 Lay_ShowAlert (Lay_INFO,Txt_No_centres_have_been_created_in_this_institution);

      /***** Put a form to create a new centre *****/
      Ctr_PutFormToCreateCentre ();

      /***** List current centres *****/
      if (Gbl.Ctrs.Num)
	 Ctr_ListCentresForEdition ();

      /***** Free list of centres *****/
      Ctr_FreeListCentres ();

      /***** Free list of places *****/
      Plc_FreeListPlaces ();
     }
   else	// No institutions
     {
      Lay_ShowAlert (Lay_WARNING,Txt_There_is_no_list_of_institutions);
      Lay_ShowAlert (Lay_INFO,Txt_You_must_create_at_least_one_institution_before_creating_centres);
     }

   /***** Free list of institutions *****/
   Ins_FreeListInstitutions ();
  }

/*****************************************************************************/
/***** Get a list with all the centres or with those of an institution *******/
/*****************************************************************************/
// If InsCod <= 0 ==> get all the centres, of any institution
// In InsCod >  0 ==> get only the centres of the specified institution

void Ctr_GetListCentres (long InsCod)
  {
   char OrderBySubQuery[256];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumCtr;
   struct Centre *Ctr;

   /***** Get centres from database *****/
   switch (Gbl.Ctrs.SelectedOrderType)
     {
      case Ctr_ORDER_BY_CENTRE:
         sprintf (OrderBySubQuery,"FullName");
         break;
      case Ctr_ORDER_BY_NUM_TCHS:
         sprintf (OrderBySubQuery,"NumTchs DESC,FullName");
         break;
     }
   if (InsCod > 0)	// Only the centres of the specified institution
      sprintf (Query,"(SELECT centres.CtrCod,centres.InsCod,centres.PlcCod,"
                     "centres.Status,centres.RequesterUsrCod,"
		     "centres.ShortName,centres.FullName,centres.WWW,"
		     "COUNT(DISTINCT usr_data.UsrCod) AS NumTchs"
		     " FROM centres,usr_data,crs_usr"
		     " WHERE centres.InsCod='%ld' AND crs_usr.Role='%u'"
		     " AND crs_usr.UsrCod=usr_data.UsrCod AND usr_data.CtrCod=centres.CtrCod"
		     " GROUP BY centres.CtrCod)"
		     " UNION "
		     "(SELECT CtrCod,InsCod,PlcCod,Status,RequesterUsrCod,"
		     "ShortName,FullName,WWW,0 AS NumTchs"
		     " FROM centres"
		     " WHERE centres.InsCod='%ld' AND CtrCod NOT IN"
		     " (SELECT DISTINCT usr_data.CtrCod FROM usr_data,crs_usr"
		     " WHERE centres.InsCod='%ld' AND crs_usr.Role='%u'"
		     " AND crs_usr.UsrCod=usr_data.UsrCod))"
		     " ORDER BY %s",
	       InsCod,
	       (unsigned) Rol_ROLE_TEACHER,
	       InsCod,InsCod,
	       (unsigned) Rol_ROLE_TEACHER,
	       OrderBySubQuery);
   else			// InsCod <= 0 ==> all the centres
      sprintf (Query,"(SELECT centres.CtrCod,centres.InsCod,centres.PlcCod,"
                     "centres.Status,centres.RequesterUsrCod,"
		     "centres.ShortName,centres.FullName,centres.WWW,"
		     "COUNT(DISTINCT usr_data.UsrCod) AS NumTchs"
		     " FROM centres,usr_data,crs_usr"
		     " WHERE crs_usr.Role='%u'"
		     " AND crs_usr.UsrCod=usr_data.UsrCod AND usr_data.CtrCod=centres.CtrCod"
		     " GROUP BY centres.CtrCod)"
		     " UNION "
		     "(SELECT CtrCod,InsCod,PlcCod,Status,RequesterUsrCod,"
		     "ShortName,FullName,WWW,0 AS NumTchs"
		     " FROM centres"
		     " WHERE CtrCod NOT IN"
		     " (SELECT DISTINCT usr_data.CtrCod FROM usr_data,crs_usr"
		     " WHERE crs_usr.Role='%u'"
		     " AND crs_usr.UsrCod=usr_data.UsrCod))"
		     " ORDER BY %s",
	       (unsigned) Rol_ROLE_TEACHER,
	       (unsigned) Rol_ROLE_TEACHER,
	       OrderBySubQuery);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get centres");

   if (NumRows) // Centres found...
     {
      // NumRows should be equal to Deg->NumCourses
      Gbl.Ctrs.Num = (unsigned) NumRows;

      /***** Create list with courses in degree *****/
      if ((Gbl.Ctrs.Lst = (struct Centre *) calloc (NumRows,sizeof (struct Centre))) == NULL)
          Lay_ShowErrorAndExit ("Not enough memory to store centres.");

      /***** Get the centres *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Ctrs.Num;
	   NumCtr++)
        {
         Ctr = &(Gbl.Ctrs.Lst[NumCtr]);

         /* Get next centre */
         row = mysql_fetch_row (mysql_res);

         /* Get centre code (row[0]) */
         if ((Ctr->CtrCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Lay_ShowErrorAndExit ("Wrong code of centre.");

         /* Get institution code (row[1]) */
         Ctr->InsCod = Str_ConvertStrCodToLongCod (row[1]);

         /* Get place code (row[2]) */
         Ctr->PlcCod = Str_ConvertStrCodToLongCod (row[2]);

	 /* Get centre status (row[3]) */
	 if (sscanf (row[3],"%u",&(Ctr->Status)) != 1)
	    Lay_ShowErrorAndExit ("Wrong centre status.");

	 /* Get requester user's code (row[4]) */
	 Ctr->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[4]);

         /* Get the short name of the centre (row[5]) */
         strcpy (Ctr->ShortName,row[5]);

         /* Get the full name of the centre (row[6]) */
         strcpy (Ctr->FullName,row[6]);

         /* Get the URL of the centre (row[7]) */
         strcpy (Ctr->WWW,row[7]);

         /* Get number of teachers in this centre (row[8]) */
         if (sscanf (row[8],"%u",&Ctr->NumTchs) != 1)
            Ctr->NumTchs = 0;

         /* Count number of degrees in this centre */
         Ctr->NumDegs = Deg_CountNumDegsInCtr (Ctr->CtrCod);
        }
     }
   else
      Gbl.Ctrs.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Get data of centre by code *************************/
/*****************************************************************************/

bool Ctr_GetDataOfCentreByCod (struct Centre *Ctr)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool CtrFound = false;

   /***** Clear data *****/
   Ctr->InsCod = -1L;
   Ctr->PlcCod = -1L;
   Ctr->Status = (Ctr_Status_t) 0;
   Ctr->RequesterUsrCod = -1L;
   Ctr->ShortName[0] = '\0';
   Ctr->FullName[0]  = '\0';
   Ctr->WWW[0]       = '\0';
   Ctr->NumDegs = 0;
   Ctr->NumTchs = 0;

   /***** Check if centre code is correct *****/
   if (Ctr->CtrCod > 0)
     {
      /***** Get data of a centre from database *****/
      sprintf (Query,"(SELECT centres.InsCod,centres.PlcCod,centres.Status,centres.RequesterUsrCod,centres.ShortName,centres.FullName,"
                     "centres.WWW,COUNT(DISTINCT usr_data.UsrCod) AS NumTchs"
                     " FROM centres,usr_data,crs_usr"
                     " WHERE centres.CtrCod='%ld'"
                     " AND centres.CtrCod=usr_data.CtrCod AND usr_data.UsrCod=crs_usr.UsrCod AND crs_usr.Role='%u'"
                     " GROUP BY centres.CtrCod)"
                     " UNION "
                     "(SELECT InsCod,PlcCod,Status,RequesterUsrCod,ShortName,FullName,WWW,0 AS NumTchs"
                     " FROM centres"
                     " WHERE CtrCod='%ld' AND CtrCod NOT IN"
                     " (SELECT DISTINCT usr_data.CtrCod FROM usr_data,crs_usr"
                     " WHERE crs_usr.Role='%u'"
                     " AND crs_usr.UsrCod=usr_data.UsrCod))",
               Ctr->CtrCod,
               (unsigned) Rol_ROLE_TEACHER,
               Ctr->CtrCod,
               (unsigned) Rol_ROLE_TEACHER);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get data of a centre")) // Centre found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get the code of the institution (row[0]) */
         Ctr->InsCod = Str_ConvertStrCodToLongCod (row[0]);

         /* Get the code of the place (row[1]) */
         Ctr->PlcCod = Str_ConvertStrCodToLongCod (row[1]);

	 /* Get centre status (row[2]) */
	 if (sscanf (row[2],"%u",&(Ctr->Status)) != 1)
	    Lay_ShowErrorAndExit ("Wrong centre status.");

	 /* Get requester user's code (row[3]) */
	 Ctr->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[3]);

         /* Get the short name of the centre (row[4]) */
         strcpy (Ctr->ShortName,row[4]);

         /* Get the full name of the centre (row[5]) */
         strcpy (Ctr->FullName,row[5]);

         /* Get the URL of the centre (row[6]) */
         strcpy (Ctr->WWW,row[6]);

         /* Get number of teachers in this centre (row[7]) */
         if (sscanf (row[7],"%u",&Ctr->NumTchs) != 1)
            Ctr->NumTchs = 0;

         /* Count number of degrees in this centre */
         Ctr->NumDegs = Deg_CountNumDegsInCtr (Ctr->CtrCod);

         /* Set return value */
         CtrFound = true;
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return CtrFound;
  }

/*****************************************************************************/
/*********** Get the institution code of a centre from its code **************/
/*****************************************************************************/

long Ctr_GetInsCodOfCentreByCod (long CtrCod)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long InsCod = -1L;

   if (CtrCod > 0)
     {
      /***** Get the institution code of a centre from database *****/
      sprintf (Query,"SELECT InsCod FROM centres WHERE CtrCod='%ld'",
               CtrCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get the institution of a centre") == 1)
	{
	 /***** Get the institution code of this centre *****/
	 row = mysql_fetch_row (mysql_res);
	 InsCod = Str_ConvertStrCodToLongCod (row[0]);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return InsCod;
  }

/*****************************************************************************/
/******************* Get photo attribution from database *********************/
/*****************************************************************************/

static void Ctr_GetPhotoAttribution (long CtrCod,char **PhotoAttribution)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Free possible former photo attribution *****/
   Ctr_FreePhotoAttribution (PhotoAttribution);

   /***** Get photo attribution from database *****/
   sprintf (Query,"SELECT PhotoAttribution FROM centres WHERE CtrCod='%ld'",
	    CtrCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get photo attribution"))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get the attribution of the photo of the centre (row[0]) */
      if (row[0])
	 if (row[0][0])
	   {
	    if (((*PhotoAttribution) = (char *) malloc (strlen (row[0])+1)) == NULL)
	       Lay_ShowErrorAndExit ("Error allocating memory for photo attribution.");
	    strcpy (*PhotoAttribution,row[0]);
	   }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Free memory used for photo attribution *******************/
/*****************************************************************************/

static void Ctr_FreePhotoAttribution (char **PhotoAttribution)
  {
   if (*PhotoAttribution)
     {
      free ((void *) *PhotoAttribution);
      *PhotoAttribution = NULL;
     }
  }

/*****************************************************************************/
/**************************** Free list of centres ***************************/
/*****************************************************************************/

void Ctr_FreeListCentres (void)
  {
   if (Gbl.Ctrs.Lst)
     {
      /***** Free memory used by the list of courses in degree *****/
      free ((void *) Gbl.Ctrs.Lst);
      Gbl.Ctrs.Lst = NULL;
      Gbl.Ctrs.Num = 0;
     }
  }

/*****************************************************************************/
/************************** Write selector of centre *************************/
/*****************************************************************************/

void Ctr_WriteSelectorOfCentre (Act_Action_t NextAction)
  {
   extern const char *Txt_Centre;
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtrs;
   unsigned NumCtr;
   long CtrCod;

   /***** Start form *****/
   Act_FormGoToStart (NextAction);
   fprintf (Gbl.F.Out,"<select name=\"CtrCod\" style=\"width:140px;\"");
   if (Gbl.CurrentIns.Ins.InsCod > 0)
      fprintf (Gbl.F.Out," onchange=\"javascript:document.getElementById('%s').submit();\"",
               Gbl.FormId);
   else
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,"><option value=\"\"");
   if (Gbl.CurrentCtr.Ctr.CtrCod < 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out," disabled=\"disabled\">[%s]</option>",
            Txt_Centre);

   if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      /***** Get centres from database *****/
      sprintf (Query,"SELECT DISTINCT CtrCod,ShortName"
	             " FROM centres"
                     " WHERE InsCod='%ld'"
                     " ORDER BY ShortName",
               Gbl.CurrentIns.Ins.InsCod);
      NumCtrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get centres");

      /***** Get centres *****/
      for (NumCtr = 0;
	   NumCtr < NumCtrs;
	   NumCtr++)
        {
         /* Get next centre */
         row = mysql_fetch_row (mysql_res);

         /* Get centre code (row[0]) */
         if ((CtrCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of centre.");

         /* Write option */
         fprintf (Gbl.F.Out,"<option value=\"%ld\"",CtrCod);
         if (Gbl.CurrentCtr.Ctr.CtrCod > 0 &&
             CtrCod == Gbl.CurrentCtr.Ctr.CtrCod)
	    fprintf (Gbl.F.Out," selected=\"selected\"");
         fprintf (Gbl.F.Out,">%s</option>",row[1]);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** End form *****/
   fprintf (Gbl.F.Out,"</select>"
	              "</form>");
  }

/*****************************************************************************/
/*************************** List all the centres ****************************/
/*****************************************************************************/

#define Ctr_MAX_LENGTH_WWW_ON_SCREEN 10

static void Ctr_ListCentresForEdition (void)
  {
   extern const char *Txt_Centres_of_INSTITUTION_X;
   extern const char *Txt_Remove_centre;
   extern const char *Txt_Another_place;
   extern const char *Txt_CENTRE_STATUS[Ctr_NUM_STATUS_TXT];
   unsigned NumCtr;
   struct Centre *Ctr;
   struct Institution Ins;
   unsigned NumIns;
   unsigned NumPlc;
   char WWW[Ctr_MAX_LENGTH_WWW_ON_SCREEN+1];
   struct UsrData UsrDat;
   bool ICanEdit;
   Ctr_StatusTxt_t StatusTxt;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Write heading *****/
   sprintf (Gbl.Title,Txt_Centres_of_INSTITUTION_X,
            Gbl.CurrentIns.Ins.FullName);
   Lay_StartRoundFrameTable10 (NULL,2,Gbl.Title);
   Ctr_PutHeadCentresForEdition ();

   /***** Write all the centres *****/
   for (NumCtr = 0;
	NumCtr < Gbl.Ctrs.Num;
	NumCtr++)
     {
      Ctr = &Gbl.Ctrs.Lst[NumCtr];

      /* Get data of institution of this centre */
      Ins.InsCod = Ctr->InsCod;
      Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);

      ICanEdit = Ctr_CheckIfICanEdit (Ctr);

      /* Put icon to remove centre */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"BM\">");
      if (Ctr->NumDegs || Ctr->NumTchs ||	// Centre has degrees or teachers ==> deletion forbidden
          !ICanEdit)
         fprintf (Gbl.F.Out,"<img src=\"%s/deloff16x16.gif\""
                            " alt=\"\" class=\"ICON16x16\" />",
                  Gbl.Prefs.IconsURL);
      else
        {
         Act_FormStart (ActRemCtr);
         Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
         fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/delon16x16.gif\""
                            " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />"
                            "</form>",
                  Gbl.Prefs.IconsURL,
                  Txt_Remove_centre,
                  Txt_Remove_centre);
        }
      fprintf (Gbl.F.Out,"</td>");

      /* Centre code */
      fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:right;\">"
	                 "%ld&nbsp;"
	                 "</td>",
               Ctr->CtrCod);

      /* Centre logo */
      fprintf (Gbl.F.Out,"<td title=\"%s\""
	                 " style=\"width:20px; text-align:left;\">",
               Ctr->FullName);
      Log_DrawLogo (Sco_SCOPE_CTR,Ctr->CtrCod,Ctr->ShortName,16,NULL,true);
      fprintf (Gbl.F.Out,"</td>");

      /* Institution */
      fprintf (Gbl.F.Out,"<td class=\"DAT\""
	                 " style=\"text-align:left; vertical-align:middle;\">");
      if (Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SYS_ADM)	// I can select institution
	{
	 Act_FormStart (ActChgCtrIns);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<select name=\"OthInsCod\" style=\"width:50px;\""
			    " onchange=\"javascript:document.getElementById('%s').submit();\">",
		  Gbl.FormId);
	 for (NumIns = 0;
	      NumIns < Gbl.Inss.Num;
	      NumIns++)
	    fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
		     Gbl.Inss.Lst[NumIns].InsCod,
		     (Gbl.Inss.Lst[NumIns].InsCod == Ctr->InsCod) ? " selected=\"selected\"" :
			                                            "",
		     Gbl.Inss.Lst[NumIns].ShortName);
	 fprintf (Gbl.F.Out,"</select>"
			    "</form>");
	}
      else
	 fprintf (Gbl.F.Out,"%s",Gbl.CurrentIns.Ins.ShortName);
      fprintf (Gbl.F.Out,"</td>");

      /* Place */
      fprintf (Gbl.F.Out,"<td class=\"DAT\""
	                 " style=\"text-align:left; vertical-align:middle;\">");
      if (ICanEdit)
	{
	 Act_FormStart (ActChgCtrPlc);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<select name=\"PlcCod\" style=\"width:50px;\""
			    " onchange=\"javascript:document.getElementById('%s').submit();\">",
		  Gbl.FormId);
	 fprintf (Gbl.F.Out,"<option value=\"0\"");
	 if (Ctr->PlcCod == 0)
	    fprintf (Gbl.F.Out," selected=\"selected\"");
	 fprintf (Gbl.F.Out,">%s</option>",Txt_Another_place);
	 for (NumPlc = 0;
	      NumPlc < Gbl.Plcs.Num;
	      NumPlc++)
	    fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
		     Gbl.Plcs.Lst[NumPlc].PlcCod,
		     (Gbl.Plcs.Lst[NumPlc].PlcCod == Ctr->PlcCod) ? " selected=\"selected\"" :
			                                            "",
		     Gbl.Plcs.Lst[NumPlc].ShortName);
	 fprintf (Gbl.F.Out,"</select>"
			    "</form>");
	}
      else
	 for (NumPlc = 0;
	      NumPlc < Gbl.Plcs.Num;
	      NumPlc++)
	    if (Gbl.Plcs.Lst[NumPlc].PlcCod == Ctr->PlcCod)
	       fprintf (Gbl.F.Out,"%s",Gbl.Plcs.Lst[NumPlc].ShortName);
      fprintf (Gbl.F.Out,"</td>");

      /* Centre short name */
      fprintf (Gbl.F.Out,"<td class=\"DAT\""
	                 " style=\"text-align:left; vertical-align:middle;\">");
      if (ICanEdit)
	{
	 Act_FormStart (ActRenCtrSho);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\" size=\"10\" maxlength=\"%u\" value=\"%s\""
			    " onchange=\"javascript:document.getElementById('%s').submit();\" />"
			    "</form>"
			    "</td>",
		  Ctr_MAX_LENGTH_CENTRE_SHORT_NAME,Ctr->ShortName,Gbl.FormId);
	}
      else
	 fprintf (Gbl.F.Out,"%s",Ctr->ShortName);
      fprintf (Gbl.F.Out,"</td>");

      /* Centre full name */
      fprintf (Gbl.F.Out,"<td class=\"DAT\""
	                 " style=\"text-align:left; vertical-align:middle;\">");
      if (ICanEdit)
	{
	 Act_FormStart (ActRenCtrFul);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\" size=\"30\" maxlength=\"%u\" value=\"%s\""
			    " onchange=\"javascript:document.getElementById('%s').submit();\" />"
			    "</form>"
			    "</td>",
		  Ctr_MAX_LENGTH_CENTRE_FULL_NAME,Ctr->FullName,Gbl.FormId);
	}
      else
	 fprintf (Gbl.F.Out,"%s",Ctr->FullName);
      fprintf (Gbl.F.Out,"</td>");

      /* Centre WWW */
      fprintf (Gbl.F.Out,"<td class=\"DAT\""
	                 " style=\"text-align:left; vertical-align:middle;\">");
      if (ICanEdit)
	{
	 Act_FormStart (ActChgCtrWWW);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"WWW\" size=\"10\" maxlength=\"%u\" value=\"%s\""
			    " onchange=\"javascript:document.getElementById('%s').submit();\" />"
			    "</form>",
		  Cns_MAX_LENGTH_WWW,Ctr->WWW,Gbl.FormId);
	}
      else
	{
         strncpy (WWW,Ctr->WWW,Ctr_MAX_LENGTH_WWW_ON_SCREEN);
         WWW[Ctr_MAX_LENGTH_WWW_ON_SCREEN] = '\0';
         fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"DAT\" title=\"%s\">%s",
                  Ctr->WWW,Ctr->WWW,WWW);
         if (strlen (Ctr->WWW) > Ctr_MAX_LENGTH_WWW_ON_SCREEN)
            fprintf (Gbl.F.Out,"...");
         fprintf (Gbl.F.Out,"</a>");
	}
      fprintf (Gbl.F.Out,"</td>");

      /* Number of teachers */
      fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:right;\">"
	                 "%u"
	                 "</td>",
               Ctr->NumTchs);

      /* Number of degrees */
      fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:right;\">"
	                 "%u"
	                 "</td>",
               Ctr->NumDegs);

      /* Centre status */
      StatusTxt = Ctr_GetStatusTxtFromStatusBits (Ctr->Status);
      fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:left; vertical-align:middle;\">");
      if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_INS_ADM &&
	  StatusTxt == Ctr_STATUS_PENDING)
	{
	 Act_FormStart (ActChgCtrSta);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<select name=\"Status\" style=\"width:80px;\""
			    " onchange=\"javascript:document.getElementById('%s').submit();\">"
			    "<option value=\"%u\" selected=\"selected\">%s</option>"
			    "<option value=\"%u\">%s</option>"
			    "</select>"
			    "</form>",
		  Gbl.FormId,
		  (unsigned) Ctr_GetStatusBitsFromStatusTxt (Ctr_STATUS_PENDING),
		  Txt_CENTRE_STATUS[Ctr_STATUS_PENDING],
		  (unsigned) Ctr_GetStatusBitsFromStatusTxt (Ctr_STATUS_ACTIVE),
		  Txt_CENTRE_STATUS[Ctr_STATUS_ACTIVE]);
	}
      else
	 fprintf (Gbl.F.Out,"%s",Txt_CENTRE_STATUS[StatusTxt]);
      fprintf (Gbl.F.Out,"</td>");

      /* Centre requester */
      UsrDat.UsrCod = Ctr->RequesterUsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);
      fprintf (Gbl.F.Out,"<td style=\"width:100px;"
	                 " text-align:left; vertical-align:top;\">"
			 "<table class=\"CELLS_PAD_2\" style=\"width:100px;\">"
			 "<tr>");
      Msg_WriteMsgAuthor (&UsrDat,80,10,"DAT",true,NULL);
      fprintf (Gbl.F.Out,"</tr>"
			 "</table>"
			 "</td>"
			 "</tr>");
     }

   /***** End table *****/
   Lay_EndRoundFrameTable10 ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************** Check if I can edit, remove, etc. a centre *******************/
/*****************************************************************************/

static bool Ctr_CheckIfICanEdit (struct Centre *Ctr)
  {
   return (bool) (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_INS_ADM ||		// I am an institution administrator or higher
                  ((Ctr->Status & Ctr_STATUS_BIT_PENDING) != 0 &&		// Centre is not yet activated
                   Gbl.Usrs.Me.UsrDat.UsrCod == Ctr->RequesterUsrCod));		// I am the requester
  }

/*****************************************************************************/
/******************* Set StatusTxt depending on status bits ******************/
/*****************************************************************************/
// Ctr_STATUS_UNKNOWN = 0	// Other
// Ctr_STATUS_ACTIVE  = 1	// 00 (Status == 0)
// Ctr_STATUS_PENDING = 2	// 01 (Status == Ctr_STATUS_BIT_PENDING)
// Ctr_STATUS_REMOVED = 3	// 1- (Status & Ctr_STATUS_BIT_REMOVED)

static Ctr_StatusTxt_t Ctr_GetStatusTxtFromStatusBits (Ctr_Status_t Status)
  {
   if (Status == 0)
      return Ctr_STATUS_ACTIVE;
   if (Status == Ctr_STATUS_BIT_PENDING)
      return Ctr_STATUS_PENDING;
   if (Status & Ctr_STATUS_BIT_REMOVED)
      return Ctr_STATUS_REMOVED;
   return Ctr_STATUS_UNKNOWN;
  }

/*****************************************************************************/
/******************* Set status bits depending on StatusTxt ******************/
/*****************************************************************************/
// Ctr_STATUS_UNKNOWN = 0	// Other
// Ctr_STATUS_ACTIVE  = 1	// 00 (Status == 0)
// Ctr_STATUS_PENDING = 2	// 01 (Status == Ctr_STATUS_BIT_PENDING)
// Ctr_STATUS_REMOVED = 3	// 1- (Status & Ctr_STATUS_BIT_REMOVED)

static Ctr_Status_t Ctr_GetStatusBitsFromStatusTxt (Ctr_StatusTxt_t StatusTxt)
  {
   switch (StatusTxt)
     {
      case Ctr_STATUS_UNKNOWN:
      case Ctr_STATUS_ACTIVE:
	 return (Ctr_Status_t) 0;
      case Ctr_STATUS_PENDING:
	 return Ctr_STATUS_BIT_PENDING;
      case Ctr_STATUS_REMOVED:
	 return Ctr_STATUS_BIT_REMOVED;
     }
   return (Ctr_Status_t) 0;
  }

/*****************************************************************************/
/******************** Write parameter with code of centre ********************/
/*****************************************************************************/

void Ctr_PutParamCtrCod (long CtrCod)
  {
   Par_PutHiddenParamLong ("CtrCod",CtrCod);
  }

/*****************************************************************************/
/***************** Write parameter with code of other centre *****************/
/*****************************************************************************/

static void Ctr_PutParamOtherCtrCod (long CtrCod)
  {
   Par_PutHiddenParamLong ("OthCtrCod",CtrCod);
  }

/*****************************************************************************/
/****************** Get parameter with code of other centre ******************/
/*****************************************************************************/

long Ctr_GetParamOtherCtrCod (void)
  {
   char LongStr[1+10+1];

   /***** Get parameter with code of centre *****/
   Par_GetParToText ("OthCtrCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/******************************* Remove a centre *****************************/
/*****************************************************************************/

void Ctr_RemoveCentre (void)
  {
   extern const char *Txt_To_remove_a_centre_you_must_first_remove_all_degrees_and_teachers_in_the_centre;
   extern const char *Txt_Centre_X_removed;
   char Query[512];
   struct Centre Ctr;
   char PathCtr[PATH_MAX+1];

   /***** Get centre code *****/
   if ((Ctr.CtrCod = Ctr_GetParamOtherCtrCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of centre is missing.");

   /***** Get data of the centre from database *****/
   Ctr_GetDataOfCentreByCod (&Ctr);

   /***** Check if this centre has teachers *****/
   if (Ctr.NumDegs ||
       Ctr.NumTchs)	// Centre has degrees or teachers ==> don't remove
      Lay_ShowAlert (Lay_WARNING,Txt_To_remove_a_centre_you_must_first_remove_all_degrees_and_teachers_in_the_centre);
   else	// Centre has no teachers ==> remove it
     {
      /***** Remove information related to files in centre *****/
      Brw_RemoveCtrFilesFromDB (Ctr.CtrCod);

      /***** Remove directories of the centre *****/
      sprintf (PathCtr,"%s/%s/%02u/%u",
	       Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CTR,
	       (unsigned) (Ctr.CtrCod % 100),
	       (unsigned) Ctr.CtrCod);
      Brw_RemoveTree (PathCtr);

      /***** Remove centre *****/
      sprintf (Query,"DELETE FROM centres WHERE CtrCod='%ld'",
               Ctr.CtrCod);
      DB_QueryDELETE (Query,"can not remove a centre");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Message,Txt_Centre_X_removed,
               Ctr.FullName);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }

   /***** Show the form again *****/
   Ctr_EditCentres ();
  }

/*****************************************************************************/
/********************* Change the institution of a centre ********************/
/*****************************************************************************/

void Ctr_ChangeCentreIns (void)
  {
   extern const char *Txt_The_institution_of_the_centre_has_changed;
   struct Centre *Ctr;
   char Query[512];

   Ctr = &Gbl.Ctrs.EditingCtr;

   /***** Get parameters from form *****/
   /* Get the code of the centre */
   if ((Ctr->CtrCod = Ctr_GetParamOtherCtrCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of centre is missing.");

   /* Get parameter with institution code */
   Ctr->InsCod = Ins_GetParamOtherInsCod ();

   /***** Update institution in table of centres *****/
   sprintf (Query,"UPDATE centres SET InsCod='%ld' WHERE CtrCod='%ld'",
            Ctr->InsCod,Ctr->CtrCod);
   DB_QueryUPDATE (Query,"can not update the institution of a centre");

   /***** Write message to show the change made *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_The_institution_of_the_centre_has_changed);

   /***** Show the form again *****/
   Ctr_EditCentres ();
  }

/*****************************************************************************/
/************************ Change the place of a centre ***********************/
/*****************************************************************************/

void Ctr_ChangeCentrePlace (void)
  {
   extern const char *Txt_The_place_of_the_centre_has_changed;
   struct Centre *Ctr;
   char Query[512];

   Ctr = &Gbl.Ctrs.EditingCtr;

   /***** Get parameters from form *****/
   /* Get centre code */
   if ((Ctr->CtrCod = Ctr_GetParamOtherCtrCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of centre is missing.");

   /* Get parameter with centre code */
   Ctr->PlcCod = Plc_GetParamPlcCod ();

   /***** Update place in table of centres *****/
   sprintf (Query,"UPDATE centres SET PlcCod='%ld' WHERE CtrCod='%ld'",
            Ctr->PlcCod,Ctr->CtrCod);
   DB_QueryUPDATE (Query,"can not update the place of a centre");

   /***** Write message to show the change made *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_The_place_of_the_centre_has_changed);

   /***** Show the form again *****/
   Ctr_EditCentres ();
  }

/*****************************************************************************/
/********************* Change the short name of a centre *********************/
/*****************************************************************************/

void Ctr_RenameCentreShort (void)
  {
   Ctr_RenameCentre (Cns_SHORT_NAME);
  }

/*****************************************************************************/
/********************* Change the full name of a centre **********************/
/*****************************************************************************/

void Ctr_RenameCentreFull (void)
  {
   Ctr_RenameCentre (Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a degree ************************/
/*****************************************************************************/

static void Ctr_RenameCentre (Cns_ShortOrFullName_t ShortOrFullName)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_centre_X_empty;
   extern const char *Txt_The_centre_X_already_exists;
   extern const char *Txt_The_centre_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_centre_X_has_not_changed;
   char Query[512];
   struct Centre *Ctr;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxLength = 0;		// Initialized to avoid warning
   char *CurrentCtrName = NULL;		// Initialized to avoid warning
   char NewCtrName[Ctr_MAX_LENGTH_CENTRE_FULL_NAME+1];

   Ctr = &Gbl.Ctrs.EditingCtr;
   switch (ShortOrFullName)
     {
      case Cns_SHORT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxLength = Ctr_MAX_LENGTH_CENTRE_SHORT_NAME;
         CurrentCtrName = Ctr->ShortName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxLength = Ctr_MAX_LENGTH_CENTRE_FULL_NAME;
         CurrentCtrName = Ctr->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the centre */
   if ((Ctr->CtrCod = Ctr_GetParamOtherCtrCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of centre is missing.");

   /* Get the new name for the centre */
   Par_GetParToText (ParamName,NewCtrName,MaxLength);

   /***** Get from the database the old names of the centre *****/
   Ctr_GetDataOfCentreByCod (Ctr);

   /***** Check if new name is empty *****/
   if (!NewCtrName[0])
     {
      sprintf (Gbl.Message,Txt_You_can_not_leave_the_name_of_the_centre_X_empty,
               CurrentCtrName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
   else
     {
      /***** Check if old and new names are the same (this happens when user press enter with no changes in the form) *****/
      if (strcmp (CurrentCtrName,NewCtrName))	// Different names
        {
         /***** If degree was in database... *****/
         if (Ctr_CheckIfCentreNameExistsInCurrentIns (ParamName,NewCtrName,Ctr->CtrCod))
           {
            sprintf (Gbl.Message,Txt_The_centre_X_already_exists,
                     NewCtrName);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else
           {
            /* Update the table changing old name by new name */
            sprintf (Query,"UPDATE centres SET %s='%s' WHERE CtrCod='%ld'",
                     FieldName,NewCtrName,Ctr->CtrCod);
            DB_QueryUPDATE (Query,"can not update the name of a centre");

            /***** Write message to show the change made *****/
            sprintf (Gbl.Message,Txt_The_centre_X_has_been_renamed_as_Y,
                     CurrentCtrName,NewCtrName);
            Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
           }
        }
      else	// The same name
        {
         sprintf (Gbl.Message,Txt_The_name_of_the_centre_X_has_not_changed,
                  CurrentCtrName);
         Lay_ShowAlert (Lay_INFO,Gbl.Message);
        }
     }

   /***** Show the form again *****/
   strcpy (CurrentCtrName,NewCtrName);
   Ctr_EditCentres ();
  }

/*****************************************************************************/
/********************* Check if the name of centre exists ********************/
/*****************************************************************************/

static bool Ctr_CheckIfCentreNameExistsInCurrentIns (const char *FieldName,const char *Name,long CtrCod)
  {
   char Query[512];

   /***** Get number of centres with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM centres"
	          " WHERE InsCod='%ld' AND %s='%s' AND CtrCod<>'%ld'",
            Gbl.CurrentIns.Ins.InsCod,FieldName,Name,CtrCod);
   return (DB_QueryCOUNT (Query,"can not check if the name of a centre already existed") != 0);
  }

/*****************************************************************************/
/************************* Change the URL of a centre ************************/
/*****************************************************************************/

void Ctr_ChangeCtrWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   extern const char *Txt_You_can_not_leave_the_web_address_empty;
   struct Centre *Ctr;
   char Query[256+Cns_MAX_LENGTH_WWW];
   char NewWWW[Cns_MAX_LENGTH_WWW+1];

   Ctr = &Gbl.Ctrs.EditingCtr;

   /***** Get parameters from form *****/
   /* Get the code of the centre */
   if ((Ctr->CtrCod = Ctr_GetParamOtherCtrCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of centre is missing.");

   /* Get the new WWW for the centre */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_LENGTH_WWW);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /* Update the table changing old WWW by new WWW */
      sprintf (Query,"UPDATE centres SET WWW='%s' WHERE CtrCod='%ld'",
               NewWWW,Ctr->CtrCod);
      DB_QueryUPDATE (Query,"can not update the web of a centre");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Message,Txt_The_new_web_address_is_X,
               NewWWW);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
   else
     {
      sprintf (Gbl.Message,"%s",Txt_You_can_not_leave_the_web_address_empty);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }

   /***** Show the form again *****/
   strcpy (Ctr->WWW,NewWWW);
   Ctr_EditCentres ();
  }

/*****************************************************************************/
/*********************** Change the status of a centre ***********************/
/*****************************************************************************/

void Ctr_ChangeCtrStatus (void)
  {
   extern const char *Txt_The_status_of_the_centre_X_has_changed;
   struct Centre *Ctr;
   char Query[256];
   char UnsignedNum[10+1];
   Ctr_Status_t Status;
   Ctr_StatusTxt_t StatusTxt;

   Ctr = &Gbl.Ctrs.EditingCtr;

   /***** Get parameters from form *****/
   /* Get centre code */
   if ((Ctr->CtrCod = Ctr_GetParamOtherCtrCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of centre is missing.");

   /* Get parameter with status */
   Par_GetParToText ("Status",UnsignedNum,1);
   if (sscanf (UnsignedNum,"%u",&Status) != 1)
      Lay_ShowErrorAndExit ("Wrong status.");
   StatusTxt = Ctr_GetStatusTxtFromStatusBits (Status);
   Status = Ctr_GetStatusBitsFromStatusTxt (StatusTxt);	// New status

   /***** Get data of centre *****/
   Ctr_GetDataOfCentreByCod (Ctr);

   /***** Update status in table of centres *****/
   sprintf (Query,"UPDATE centres SET Status='%u' WHERE CtrCod='%ld'",
            (unsigned) Status,Ctr->CtrCod);
   DB_QueryUPDATE (Query,"can not update the status of a centre");

   Ctr->Status = Status;

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_The_status_of_the_centre_X_has_changed,
            Ctr->ShortName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the form again *****/
   Ctr_EditCentres ();
  }

/*****************************************************************************/
/*********** Show a form for sending a logo of the current centre ************/
/*****************************************************************************/

void Ctr_RequestLogo (void)
  {
   Log_RequestLogo (Sco_SCOPE_CTR);
  }

/*****************************************************************************/
/***************** Receive the logo of the current centre ********************/
/*****************************************************************************/

void Ctr_ReceiveLogo (void)
  {
   Log_ReceiveLogo (Sco_SCOPE_CTR);
  }

/*****************************************************************************/
/****************** Remove the logo of the current centre ********************/
/*****************************************************************************/

void Ctr_RemoveLogo (void)
  {
   Log_RemoveLogo (Sco_SCOPE_CTR);
  }

/*****************************************************************************/
/******** Put a link to the action used to request photo of centre ***********/
/*****************************************************************************/

static void Ctr_PutFormToChangeCtrPhoto (bool PhotoExists)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Change_photo;
   extern const char *Txt_Upload_photo;
   const char *Msg;

   /***** Link for changing / uploading the photo *****/
   Act_FormStart (ActReqCtrPho);
   Msg = PhotoExists ? Txt_Change_photo :
		       Txt_Upload_photo;
   Act_LinkFormSubmit (Msg,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("photo",Msg,Msg);
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/*********** Show a form for sending a photo of the current centre ***********/
/*****************************************************************************/

void Ctr_RequestPhoto (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_You_can_send_a_file_with_an_image_in_jpg_format_and_size_X_Y;
   extern const char *Txt_File_with_the_photo;
   extern const char *Txt_Upload_photo;

   /***** Write help message *****/
   sprintf (Gbl.Message,Txt_You_can_send_a_file_with_an_image_in_jpg_format_and_size_X_Y,
	    Ctr_PHOTO_REAL_WIDTH,
	    Ctr_PHOTO_REAL_HEIGHT);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   /***** Write a form to send photo *****/
   Act_FormStart (ActRecCtrPho);
   fprintf (Gbl.F.Out,"<table style=\"margin:0 auto;\">"
                      "<tr>"
                      "<td class=\"%s\" style=\"text-align:right;\">"
                      "%s:"
                      "</td>"
                      "<td style=\"text-align:left;\">"
                      "<input type=\"file\" name=\"%s\" size=\"40\" maxlength=\"100\" value=\"\" />"
                      "</td>"
                      "</tr>"
                      "<tr>"
                      "<td colspan=\"2\" style=\"text-align:center;\">"
                      "<input type=\"submit\" value=\"%s\" accept=\"image/jpeg\" />"
                      "</td>"
                      "</tr>"
                      "</table>"
                      "</form>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_File_with_the_photo,
            Fil_NAME_OF_PARAM_FILENAME_ORG,
            Txt_Upload_photo);
  }

/*****************************************************************************/
/****************** Receive a photo of the current centre ********************/
/*****************************************************************************/

void Ctr_ReceivePhoto (void)
  {
   extern const char *Txt_The_file_is_not_X;
   char Path[PATH_MAX+1];
   char FileNamePhotoSrc[PATH_MAX+1];
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE+1];
   char FileNamePhoto[PATH_MAX+1];        // Full name (including path and .jpg) of the destination file
   bool WrongType = false;

   /***** Creates directories if not exist *****/
   sprintf (Path,"%s/%s",
	    Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CTR);
   Fil_CreateDirIfNotExists (Path);
   sprintf (Path,"%s/%s/%02u",
	    Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CTR,
	    (unsigned) (Gbl.CurrentCtr.Ctr.CtrCod % 100));
   Fil_CreateDirIfNotExists (Path);
   sprintf (Path,"%s/%s/%02u/%u",
	    Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CTR,
	    (unsigned) (Gbl.CurrentCtr.Ctr.CtrCod % 100),
	    (unsigned) Gbl.CurrentCtr.Ctr.CtrCod);
   Fil_CreateDirIfNotExists (Path);

   /***** Copy in disk the file received from stdin (really from Gbl.F.Tmp) *****/
   Fil_StartReceptionOfFile (FileNamePhotoSrc,MIMEType);

   /* Check if the file type is image/jpeg or image/pjpeg or application/octet-stream */
   if (strcmp (MIMEType,"image/jpeg"))
      if (strcmp (MIMEType,"image/pjpeg"))
         if (strcmp (MIMEType,"application/octet-stream"))
            if (strcmp (MIMEType,"application/octetstream"))
               if (strcmp (MIMEType,"application/octet"))
                  WrongType = true;
   if (WrongType)
     {
      sprintf (Gbl.Message,Txt_The_file_is_not_X,"jpg");
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
      return;
     }

   /* End the reception of photo in a temporary file */
   sprintf (FileNamePhoto,"%s/%s/%02u/%u/%u.jpg",
	    Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CTR,
	    (unsigned) (Gbl.CurrentCtr.Ctr.CtrCod % 100),
	    (unsigned) Gbl.CurrentCtr.Ctr.CtrCod,
	    (unsigned) Gbl.CurrentCtr.Ctr.CtrCod);
   if (!Fil_EndReceptionOfFile (FileNamePhoto))
     {
      Lay_ShowAlert (Lay_WARNING,"Error uploading file.");
      return;
     }

   /***** Show the centre information again *****/
   Ctr_ShowConfiguration ();
  }

/*****************************************************************************/
/**************** Change the attribution of a centre photo *******************/
/*****************************************************************************/

void Ctr_ChangeCtrPhotoAttribution (void)
  {
   char Query[256+Ctr_MAX_LENGTH_PHOTO_ATTRIBUTION];
   char NewPhotoAttribution[Ctr_MAX_LENGTH_PHOTO_ATTRIBUTION+1];

   /***** Get parameters from form *****/
   /* Get the new photo attribution for the centre */
   Par_GetParToText ("Attribution",NewPhotoAttribution,Ctr_MAX_LENGTH_PHOTO_ATTRIBUTION);

   /***** Update the table changing old attribution by new attribution *****/
   sprintf (Query,"UPDATE centres SET PhotoAttribution='%s'"
		  " WHERE CtrCod='%ld'",
	    NewPhotoAttribution,Gbl.CurrentCtr.Ctr.CtrCod);
   DB_QueryUPDATE (Query,"can not update the photo attribution of the current centre");

   /***** Show the centre information again *****/
   Ctr_ShowConfiguration ();
  }

/*****************************************************************************/
/********************* Put a form to create a new centre *********************/
/*****************************************************************************/

static void Ctr_PutFormToCreateCentre (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_New_centre_of_INSTITUTION_X;
   extern const char *Txt_Another_place;
   extern const char *Txt_CENTRE_STATUS[Ctr_NUM_STATUS_TXT];
   extern const char *Txt_Create_centre;
   struct Centre *Ctr;
   unsigned NumPlc;

   Ctr = &Gbl.Ctrs.EditingCtr;

   /***** Start form *****/
   if (Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_INS_ADM)
      Act_FormStart (ActNewCtr);
   else if (Gbl.Usrs.Me.MaxRole >= Rol_ROLE_GUEST__)
      Act_FormStart (ActReqCtr);
   else
      Lay_ShowErrorAndExit ("You can not edit centres.");

   /***** Start of frame *****/
   sprintf (Gbl.Title,Txt_New_centre_of_INSTITUTION_X,
            Gbl.CurrentIns.Ins.ShortName);
   Lay_StartRoundFrameTable10 (NULL,2,Gbl.Title);

   /***** Write heading *****/
   Ctr_PutHeadCentresForEdition ();

   /***** Put disabled icon to remove centre *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"BM\">"
                      "<img src=\"%s/deloff16x16.gif\""
		      " alt=\"\" class=\"ICON16x16\" />"
		      "</td>",
	    Gbl.Prefs.IconsURL);

   /***** Centre code *****/
   fprintf (Gbl.F.Out,"<td></td>");

   /***** Centre logo *****/
   fprintf (Gbl.F.Out,"<td style=\"width:20px; text-align:left;\">");
   Log_DrawLogo (Sco_SCOPE_CTR,-1L,"",16,NULL,true);
   fprintf (Gbl.F.Out,"</td>");

   /***** Institution *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:left; vertical-align:middle;\">"
                      "<select name=\"OthInsCod\" style=\"width:50px;\" disabled=\"disabled\">"
                      "<option value=\"%ld\" selected=\"selected\">"
                      "%s"
                      "</option>"
                      "</select>"
                      "</td>",
            Gbl.CurrentIns.Ins.InsCod,
	    Gbl.CurrentIns.Ins.ShortName);

   /***** Place *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:left; vertical-align:middle;\">"
                      "<select name=\"PlcCod\" style=\"width:50px;\">"
                      "<option value=\"0\"");
   if (Ctr->PlcCod == 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",Txt_Another_place);
   for (NumPlc = 0;
	NumPlc < Gbl.Plcs.Num;
	NumPlc++)
      fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
               Gbl.Plcs.Lst[NumPlc].PlcCod,
               (Gbl.Plcs.Lst[NumPlc].PlcCod == Ctr->PlcCod) ? " selected=\"selected\"" :
        	                                              "",
               Gbl.Plcs.Lst[NumPlc].ShortName);
   fprintf (Gbl.F.Out,"</select>"
	              "</td>");

   /***** Centre short name *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:left; vertical-align:middle;\">"
                      "<input type=\"text\" name=\"ShortName\" size=\"10\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            Ctr_MAX_LENGTH_CENTRE_SHORT_NAME,Ctr->ShortName);

   /***** Centre full name *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:left; vertical-align:middle;\">"
                      "<input type=\"text\" name=\"FullName\" size=\"30\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            Ctr_MAX_LENGTH_CENTRE_FULL_NAME,Ctr->FullName);

   /***** Centre WWW *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:left; vertical-align:middle;\">"
                      "<input type=\"text\" name=\"WWW\" size=\"10\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            Cns_MAX_LENGTH_WWW,Ctr->WWW);

   /***** Number of teachers *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:right;\">"
	              "0"
	              "</td>");

   /***** Number of degrees *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:right;\">"
	              "0"
	              "</td>");

   /***** Centre status *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT\""
	              " style=\"text-align:left; vertical-align:middle;\">"
	              "%s"
	              "</td>",
            Txt_CENTRE_STATUS[Ctr_STATUS_PENDING]);

   /***** Centre requester *****/
   fprintf (Gbl.F.Out,"<td style=\"width:100px;"
	              " text-align:left; vertical-align:top;\">"
		      "<table class=\"CELLS_PAD_2\" style=\"width:100px;\">"
		      "<tr>");
   Msg_WriteMsgAuthor (&Gbl.Usrs.Me.UsrDat,80,10,"DAT",true,NULL);
   fprintf (Gbl.F.Out,"</tr>"
		      "</table>"
		      "</td>"
		      "</tr>");

   /***** Send button *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"12\" style=\"text-align:center;\">"
                      "<input type=\"submit\" value=\"%s\" />"
	              "</td>"
	              "</tr>",
            Txt_Create_centre);

   /***** End of frame *****/
   Lay_EndRoundFrameTable10 ();

   /***** End of form *****/
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/******************** Write header with fields of a degree *******************/
/*****************************************************************************/

static void Ctr_PutHeadCentresForSeeing (bool OrderSelectable)
  {
   extern const char *Txt_Place;
   extern const char *Txt_CENTRES_HELP_ORDER[2];
   extern const char *Txt_CENTRES_ORDER[2];
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_Status;
   tCtrsOrderType Order;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>"
                      "<th class=\"BM\"></th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</th>",
            Txt_Place);
   for (Order = Ctr_ORDER_BY_CENTRE;
	Order <= Ctr_ORDER_BY_NUM_TCHS;
	Order++)
     {
      fprintf (Gbl.F.Out,"<th class=\"TIT_TBL\" style=\"text-align:left;\">");
      if (OrderSelectable)
	{
	 Act_FormStart (ActSeeCtr);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Act_LinkFormSubmit (Txt_CENTRES_HELP_ORDER[Order],"TIT_TBL");
	 if (Order == Gbl.Ctrs.SelectedOrderType)
	    fprintf (Gbl.F.Out,"<u>");
	}
      fprintf (Gbl.F.Out,"%s",Txt_CENTRES_ORDER[Order]);
      if (OrderSelectable)
	{
	 if (Order == Gbl.Ctrs.SelectedOrderType)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>"
			    "</form>");
	}
      fprintf (Gbl.F.Out,"</th>");
     }
   fprintf (Gbl.F.Out,"<th class=\"TIT_TBL\" style=\"text-align:right;\">"
	              "%s"
	              "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</th>"
	              "</tr>",
	    Txt_Degrees_ABBREVIATION,
	    Txt_Status);
  }

/*****************************************************************************/
/******************** Write header with fields of a degree *******************/
/*****************************************************************************/

static void Ctr_PutHeadCentresForEdition (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Institution;
   extern const char *Txt_Place;
   extern const char *Txt_Short_Name;
   extern const char *Txt_Full_Name;
   extern const char *Txt_WWW;
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_Status;
   extern const char *Txt_Requester;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:right;\">"
                      "%s"
                      "</th>"
                      "<th style=\"width:20px;\"></th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:right;\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:right;\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Code,
            Txt_Institution,
            Txt_Place,
            Txt_Short_Name,
            Txt_Full_Name,
            Txt_WWW,
            Txt_Teachers_ABBREVIATION,
            Txt_Degrees_ABBREVIATION,
            Txt_Status,
            Txt_Requester);
  }

/*****************************************************************************/
/****************** Receive form to request a new centre *********************/
/*****************************************************************************/

void Ctr_RecFormReqCtr (void)
  {
   Ctr_RecFormRequestOrCreateCtr ((unsigned) Ctr_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/******************* Receive form to create a new centre *********************/
/*****************************************************************************/

void Ctr_RecFormNewCtr (void)
  {
   Ctr_RecFormRequestOrCreateCtr (0);
  }

/*****************************************************************************/
/************* Receive form to request or create a new centre ****************/
/*****************************************************************************/

static void Ctr_RecFormRequestOrCreateCtr (unsigned Status)
  {
   extern const char *Txt_The_centre_X_already_exists;
   extern const char *Txt_You_must_specify_the_web_address_of_the_new_centre;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_centre;
   struct Centre *Ctr;

   Ctr = &Gbl.Ctrs.EditingCtr;

   /***** Get parameters from form *****/
   /* Set centre institution */
   Ctr->InsCod = Gbl.CurrentIns.Ins.InsCod;

   /* Get place */
   if ((Ctr->PlcCod = Plc_GetParamPlcCod ()) < 0)	// 0 is reserved for "other place"
      Lay_ShowAlert (Lay_ERROR,"Wrong place.");

   /* Get centre short name */
   Par_GetParToText ("ShortName",Ctr->ShortName,Ctr_MAX_LENGTH_CENTRE_SHORT_NAME);

   /* Get centre full name */
   Par_GetParToText ("FullName",Ctr->FullName,Ctr_MAX_LENGTH_CENTRE_FULL_NAME);

   /* Get centre WWW */
   Par_GetParToText ("WWW",Ctr->WWW,Cns_MAX_LENGTH_WWW);

   if (Ctr->ShortName[0] && Ctr->FullName[0])	// If there's a centre name
     {
      if (Ctr->WWW[0])
        {
         /***** If name of centre was in database... *****/
         if (Ctr_CheckIfCentreNameExistsInCurrentIns ("ShortName",Ctr->ShortName,-1L))
           {
            sprintf (Gbl.Message,Txt_The_centre_X_already_exists,
                     Ctr->ShortName);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else if (Ctr_CheckIfCentreNameExistsInCurrentIns ("FullName",Ctr->FullName,-1L))
           {
            sprintf (Gbl.Message,Txt_The_centre_X_already_exists,
                     Ctr->FullName);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else	// Add new centre to database
            Ctr_CreateCentre (Ctr,Status);
        }
      else	// If there is not a web
        {
         sprintf (Gbl.Message,"%s",Txt_You_must_specify_the_web_address_of_the_new_centre);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
     }
   else	// If there is not a centre name
     {
      sprintf (Gbl.Message,"%s",Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_centre);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }

   /***** Show the form again *****/
   Ctr_EditCentres ();
  }

/*****************************************************************************/
/***************************** Create a new centre ***************************/
/*****************************************************************************/

static void Ctr_CreateCentre (struct Centre *Ctr,unsigned Status)
  {
   extern const char *Txt_Created_new_centre_X;
   char Query[1024];

   /***** Create a new centre *****/
   sprintf (Query,"INSERT INTO centres (InsCod,PlcCod,Status,RequesterUsrCod,"
                  "ShortName,FullName,WWW)"
                  " VALUES ('%ld','%ld','%u','%ld',"
                  "'%s','%s','%s')",
            Ctr->InsCod,Ctr->PlcCod,
            Status,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Ctr->ShortName,Ctr->FullName,Ctr->WWW);
   DB_QueryINSERT (Query,"can not create a new centre");

   /***** Write success message *****/
   sprintf (Gbl.Message,Txt_Created_new_centre_X,
            Ctr->FullName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/************************** Get number of centres ****************************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsTotal (void)
  {
   char Query[128];

   /***** Get total number of centres from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM centres");
   return (unsigned) DB_QueryCOUNT (Query,"can not get total number of centres");
  }

/*****************************************************************************/
/******************* Get number of centres in a country **********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsInCty (long CtyCod)
  {
   char Query[256];

   /***** Get number of centres of a country from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM institutions,centres"
	          " WHERE institutions.CtyCod='%ld'"
	          " AND institutions.InsCod=centres.InsCod",
	    CtyCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of centres in a country");
  }

/*****************************************************************************/
/**************** Get number of centres in an institution ********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsInIns (long InsCod)
  {
   char Query[128];

   /***** Get number of centres of an institution from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM centres"
	          " WHERE InsCod='%ld'",
	    InsCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of centres in an institution");
  }

/*****************************************************************************/
/******************** Get number of centres in a place ***********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsInPlc (long PlcCod)
  {
   char Query[256];

   /***** Get number of degrees in a place from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM centres"
	          " WHERE PlcCod='%ld'",
	    PlcCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of centres in a place");
  }

/*****************************************************************************/
/********************* Get number of centres with degrees ********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsWithDegs (const char *SubQuery)
  {
   char Query[512];

   /***** Get number of centres with degrees from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT centres.CtrCod)"
                  " FROM institutions,centres,degrees"
                  " WHERE %sinstitutions.InsCod=centres.InsCod"
                  " AND centres.CtrCod=degrees.CtrCod",
            SubQuery);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of centres with degrees");
  }

/*****************************************************************************/
/********************* Get number of centres with courses ********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsWithCrss (const char *SubQuery)
  {
   char Query[512];

   /***** Get number of centres with courses from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT centres.CtrCod)"
                  " FROM institutions,centres,degrees,courses"
                  " WHERE %sinstitutions.InsCod=centres.InsCod"
                  " AND centres.CtrCod=degrees.CtrCod"
                  " AND degrees.DegCod=courses.DegCod",
            SubQuery);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of centres with courses");
  }

/*****************************************************************************/
/********************* Get number of centres with users **********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsWithUsrs (Rol_Role_t Role,const char *SubQuery)
  {
   char Query[512];

   /***** Get number of centres with users from database *****/
   sprintf (Query,"SELECT COUNT(DISTINCT centres.CtrCod)"
                  " FROM institutions,centres,degrees,courses,crs_usr"
                  " WHERE %sinstitutions.InsCod=centres.InsCod"
                  " AND centres.CtrCod=degrees.CtrCod"
                  " AND degrees.DegCod=courses.DegCod"
                  " AND courses.CrsCod=crs_usr.CrsCod"
                  " AND crs_usr.Role='%u'",
            SubQuery,(unsigned) Role);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of centres with users");
  }

/*****************************************************************************/
/****************************** List centres found ***************************/
/*****************************************************************************/
// Returns number of centres found

unsigned Ctr_ListCtrsFound (const char *Query)
  {
   extern const char *Txt_Centres;
   extern const char *Txt_centre;
   extern const char *Txt_centres;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtrs;
   unsigned NumCtr;
   struct Centre Ctr;

   /***** Query database *****/
   if ((NumCtrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get centres")))
     {
      /***** Write heading *****/
      Lay_StartRoundFrameTable10 (NULL,2,Txt_Centres);

      /* Number of centres found */
      fprintf (Gbl.F.Out,"<tr>"
			 "<td colspan=\"7\" class=\"TIT_TBL\""
			 " style=\"text-align:center;\">");
      if (NumCtrs == 1)
	 fprintf (Gbl.F.Out,"1 %s",Txt_centre);
      else
	 fprintf (Gbl.F.Out,"%u %s",NumCtrs,Txt_centres);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      Ctr_PutHeadCentresForSeeing (false);	// Order not selectable

      /***** List the centres (one row per centre) *****/
      for (NumCtr = 1;
	   NumCtr <= NumCtrs;
	   NumCtr++)
	{
	 /* Get next centre */
	 row = mysql_fetch_row (mysql_res);

	 /* Get centre code (row[0]) */
	 Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get data of centre */
	 Ctr_GetDataOfCentreByCod (&Ctr);

	 /* Write data of this centre */
	 Ctr_ListOneCentreForSeeing (&Ctr,NumCtr);
	}

      /***** Table end *****/
      Lay_EndRoundFrameTable10 ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCtrs;
  }
