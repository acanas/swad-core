// swad_centre.c: centres

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
#include <stdbool.h>		// For boolean type
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For unlink

#include "swad_centre.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
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

// Centre photo will be saved with:
// - maximum width of Ctr_PHOTO_SAVED_MAX_HEIGHT
// - maximum height of Ctr_PHOTO_SAVED_MAX_HEIGHT
// - maintaining the original aspect ratio (aspect ratio recommended: 3:2)
#define Ctr_RECOMMENDED_ASPECT_RATIO	"3:2"
#define Ctr_PHOTO_SAVED_MAX_WIDTH	768
#define Ctr_PHOTO_SAVED_MAX_HEIGHT	512
#define Ctr_PHOTO_SAVED_QUALITY		 75	// 1 to 100

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
static void Ctr_PutIconsToPrintAndUpload (void);
static void Ctr_PutIconToChangePhoto (void);

static void Ctr_ListCentres (void);
static bool Ctr_CheckIfICanCreateCentres (void);
static void Ctr_PutIconsListCentres (void);
static void Ctr_PutIconToEditCentres (void);
static void Ctr_ListOneCentreForSeeing (struct Centre *Ctr,unsigned NumCtr);
static void Ctr_GetParamCtrOrderType (void);
static void Ctr_GetPhotoAttribution (long CtrCod,char **PhotoAttribution);
static void Ctr_FreePhotoAttribution (char **PhotoAttribution);
static void Ctr_ListCentresForEdition (void);
static bool Ctr_CheckIfICanEditACentre (struct Centre *Ctr);
static Ctr_StatusTxt_t Ctr_GetStatusTxtFromStatusBits (Ctr_Status_t Status);
static Ctr_Status_t Ctr_GetStatusBitsFromStatusTxt (Ctr_StatusTxt_t StatusTxt);

static void Ctr_PutParamOtherCtrCod (long CtrCod);
static long Ctr_GetParamOtherCtrCod (void);

static void Ctr_UpdateCtrInsDB (long CtrCod,long InsCod);
static void Ctr_RenameCentre (struct Centre *Ctr,Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Ctr_CheckIfCtrNameExistsInIns (const char *FieldName,const char *Name,long CtrCod,long InsCod);
static void Ctr_UpdateCtrWWWDB (long CtrCod,const char NewWWW[Cns_MAX_LENGTH_WWW+1]);
static void Ctr_PutButtonToGoToCtr (struct Centre *Ctr);

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
   extern const char *Hlp_SYSTEM_Pending;
   extern const char *Txt_Centres_with_pending_degrees;
   extern const char *Txt_Centre;
   extern const char *Txt_Degrees_ABBREVIATION;
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
      case Rol_CTR_ADM:
         sprintf (Query,"SELECT degrees.CtrCod,COUNT(*)"
                        " FROM degrees,ctr_admin,centres"
                        " WHERE (degrees.Status & %u)<>0"
                        " AND degrees.CtrCod=ctr_admin.CtrCod AND ctr_admin.UsrCod='%ld'"
                        " AND degrees.CtrCod=centres.CtrCod"
                        " GROUP BY degrees.CtrCod ORDER BY centres.ShortName",
                  (unsigned) Deg_STATUS_BIT_PENDING,Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Rol_SYS_ADM:
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
      Lay_StartRoundFrameTable (NULL,Txt_Centres_with_pending_degrees,
                                NULL,Hlp_SYSTEM_Pending,2);
      fprintf (Gbl.F.Out,"<tr>"
                         "<th class=\"LEFT_MIDDLE\">"
                         "%s"
                         "</th>"
                         "<th class=\"RIGHT_MIDDLE\">"
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
         BgColor = (Ctr.CtrCod == Gbl.CurrentCtr.Ctr.CtrCod) ? "LIGHT_BLUE" :
                                                               Gbl.ColorRows[Gbl.RowEvenOdd];

         /* Get data of centre */
         Ctr_GetDataOfCentreByCod (&Ctr);

         /* Centre logo and full name */
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"LEFT_MIDDLE %s\">",
                  BgColor);
         Ctr_DrawCentreLogoAndNameWithLink (&Ctr,ActSeeDeg,
                                            "DAT_NOBR","CENTER_MIDDLE");
         fprintf (Gbl.F.Out,"</td>");

         /* Number of pending degrees (row[1]) */
         fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE %s\">"
	                    "%s"
	                    "</td>"
	                    "</tr>",
                  BgColor,row[1]);

         Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
        }

      Lay_EndRoundFrameTable ();
     }
   else
      Lay_ShowAlert (Lay_INFO,Txt_There_are_no_centres_with_requests_for_degrees_to_be_confirmed);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Draw centre logo and name with link ********************/
/*****************************************************************************/

void Ctr_DrawCentreLogoAndNameWithLink (struct Centre *Ctr,Act_Action_t Action,
                                        const char *ClassLink,const char *ClassLogo)
  {
   extern const char *Txt_Go_to_X;

   /***** Start form *****/
   Act_FormGoToStart (Action);
   Ctr_PutParamCtrCod (Ctr->CtrCod);

   /***** Link to action *****/
   sprintf (Gbl.Title,Txt_Go_to_X,Ctr->FullName);
   Act_LinkFormSubmit (Gbl.Title,ClassLink,NULL);

   /***** Draw centre logo *****/
   Log_DrawLogo (Sco_SCOPE_CTR,Ctr->CtrCod,Ctr->ShrtName,20,ClassLogo,true);

   /***** End link *****/
   fprintf (Gbl.F.Out,"&nbsp;%s</a>",Ctr->FullName);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/****************** Show information of the current centre *******************/
/*****************************************************************************/

void Ctr_ShowConfiguration (void)
  {
   Ctr_Configuration (false);

   /***** Show help to enroll me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
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
   extern const char *Hlp_CENTRE_Information;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Short_name;
   extern const char *Txt_Web;
   extern const char *Txt_Shortcut;
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];
   extern const char *Txt_QR_code;
   extern const char *Txt_Users_of_the_centre;
   extern const char *Txt_Place;
   extern const char *Txt_Degrees;
   extern const char *Txt_Degrees_of_CENTRE_X;
   extern const char *Txt_Courses;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   unsigned NumIns;
   struct Place Plc;
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

      /***** Start frame *****/
      Lay_StartRoundFrame (NULL,NULL,
                           PrintView ? NULL :
	                               Ctr_PutIconsToPrintAndUpload,
	                   PrintView ? NULL :
	                               Hlp_CENTRE_Information);

      /***** Title *****/
      fprintf (Gbl.F.Out,"<div class=\"FRAME_TITLE FRAME_TITLE_BIG\">");
      if (PutLink)
	 fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
	                    " class=\"FRAME_TITLE_BIG\" title=\"%s\">",
		  Gbl.CurrentCtr.Ctr.WWW,
		  Gbl.CurrentCtr.Ctr.FullName);
      Log_DrawLogo (Sco_SCOPE_CTR,Gbl.CurrentCtr.Ctr.CtrCod,
                    Gbl.CurrentCtr.Ctr.ShrtName,64,NULL,true);
      fprintf (Gbl.F.Out,"<br />%s",Gbl.CurrentCtr.Ctr.FullName);
      if (PutLink)
	 fprintf (Gbl.F.Out,"</a>");
      fprintf (Gbl.F.Out,"</div>");

      /***** Centre photo *****/
      if (PhotoExists)
	{
	 /* Get photo attribution */
	 Ctr_GetPhotoAttribution (Gbl.CurrentCtr.Ctr.CtrCod,&PhotoAttribution);

	 /* Photo image */
	 fprintf (Gbl.F.Out,"<div class=\"DAT_SMALL CENTER_MIDDLE\">");
	 if (PutLink)
	    fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"DAT_N\">",
		     Gbl.CurrentCtr.Ctr.WWW);
	 fprintf (Gbl.F.Out,"<img src=\"%s/%s/%02u/%u/%u.jpg\""
	                    " alt=\"%s\" title=\"%s\""
	                    " class=\"%s\" />",
		  Cfg_URL_SWAD_PUBLIC,Cfg_FOLDER_CTR,
		  (unsigned) (Gbl.CurrentCtr.Ctr.CtrCod % 100),
		  (unsigned) Gbl.CurrentCtr.Ctr.CtrCod,
		  (unsigned) Gbl.CurrentCtr.Ctr.CtrCod,
		  Gbl.CurrentCtr.Ctr.ShrtName,
		  Gbl.CurrentCtr.Ctr.FullName,
		  PrintView ? "CENTRE_PHOTO_PRINT" :
			      "CENTRE_PHOTO_SHOW");
	 if (PutLink)
	    fprintf (Gbl.F.Out,"</a>");
	 fprintf (Gbl.F.Out,"</div>");

	 /* Photo attribution */
	 if (!PrintView &&
	     Gbl.Usrs.Me.LoggedRole >= Rol_CTR_ADM)
	    // Only centre admins, institution admins and centre admins
	    // have permission to edit photo attribution
	   {
	    fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
	    Act_FormStart (ActChgCtrPhoAtt);
	    fprintf (Gbl.F.Out,"<textarea id=\"AttributionArea\""
		               " name=\"Attribution\" rows=\"2\""
			       " onchange=\"document.getElementById('%s').submit();\">",
		     Gbl.Form.Id);
            if (PhotoAttribution)
	       fprintf (Gbl.F.Out,"%s",PhotoAttribution);
	    fprintf (Gbl.F.Out,"</textarea>");
	    Act_FormEnd ();
	    fprintf (Gbl.F.Out,"</div>");
	   }
	 else if (PhotoAttribution)
	    fprintf (Gbl.F.Out,"<div class=\"ATTRIBUTION\">"
			       "%s"
			       "</div>",
		     PhotoAttribution);

	 /* Free memory used for photo attribution */
	 Ctr_FreePhotoAttribution (&PhotoAttribution);
	}

      /***** Start table *****/
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL CELLS_PAD_2\">");

      /***** Institution *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
			 "%s:"
			 "</td>"
			 "<td class=\"DAT_N LEFT_MIDDLE\">",
	       The_ClassForm[Gbl.Prefs.Theme],
	       Txt_Institution);

      if (!PrintView &&
	  Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
	 // Only system admins can move a centre to another institution
	{
	 /* Get list of institutions of the current country */
         Ins_GetListInstitutions (Gbl.CurrentCty.Cty.CtyCod,Ins_GET_BASIC_DATA);

	 /* Put form to select institution */
	 Act_FormStart (ActChgCtrInsCfg);
	 fprintf (Gbl.F.Out,"<select name=\"OthInsCod\""
			    " class=\"INPUT_SHORT_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\">",
		  Gbl.Form.Id);
	 for (NumIns = 0;
	      NumIns < Gbl.Inss.Num;
	      NumIns++)
	    fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
		     Gbl.Inss.Lst[NumIns].InsCod,
		     Gbl.Inss.Lst[NumIns].InsCod == Gbl.CurrentIns.Ins.InsCod ? " selected=\"selected\"" :
										"",
		     Gbl.Inss.Lst[NumIns].ShrtName);
	 fprintf (Gbl.F.Out,"</select>");
	 Act_FormEnd ();

	 /* Free list of institutions */
	 Ins_FreeListInstitutions ();
	}
      else	// I can not move centre to another institution
	 fprintf (Gbl.F.Out,"%s",Gbl.CurrentIns.Ins.FullName);

      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      /***** Centre full name *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
	                 "%s:"
	                 "</td>"
			 "<td class=\"DAT_N LEFT_MIDDLE\">",
	       The_ClassForm[Gbl.Prefs.Theme],
	       Txt_Centre);
      if (!PrintView &&
	  Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM)
	 // Only institution admins and system admins can edit centre full name
	{
	 /* Form to change centre full name */
	 Act_FormStart (ActRenCtrFulCfg);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_FULL_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Ctr_MAX_LENGTH_CENTRE_FULL_NAME,
		  Gbl.CurrentCtr.Ctr.FullName,
		  Gbl.Form.Id);
	 Act_FormEnd ();
	}
      else	// I can not edit centre full name
	 fprintf (Gbl.F.Out,"%s",Gbl.CurrentCtr.Ctr.FullName);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      /***** Centre short name *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
	                 "%s:"
	                 "</td>"
			 "<td class=\"DAT_N LEFT_MIDDLE\">",
	       The_ClassForm[Gbl.Prefs.Theme],
	       Txt_Short_name);
      if (!PrintView &&
	  Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM)
	 // Only institution admins and system admins can edit centre short name
	{
	 /* Form to change centre short name */
	 Act_FormStart (ActRenCtrShoCfg);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_SHORT_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Ctr_MAX_LENGTH_CENTRE_SHRT_NAME,
		  Gbl.CurrentCtr.Ctr.ShrtName,
		  Gbl.Form.Id);
	 Act_FormEnd ();
	}
      else	// I can not edit centre short name
	 fprintf (Gbl.F.Out,"%s",Gbl.CurrentCtr.Ctr.ShrtName);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      /***** Centre WWW *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
			 "%s:"
			 "</td>"
			 "<td class=\"DAT LEFT_MIDDLE\">",
	       The_ClassForm[Gbl.Prefs.Theme],
	       Txt_Web);
      if (!PrintView &&
	  Gbl.Usrs.Me.LoggedRole >= Rol_CTR_ADM)
	 // Only centre admins, institution admins and system admins
	 // can change centre WWW
	{
	 /* Form to change centre WWW */
	 Act_FormStart (ActChgCtrWWWCfg);
	 fprintf (Gbl.F.Out,"<input type=\"url\" name=\"WWW\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_WWW\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Cns_MAX_LENGTH_WWW,
		  Gbl.CurrentCtr.Ctr.WWW,
		  Gbl.Form.Id);
	 Act_FormEnd ();
	}
      else	// I can not change centre WWW
	 fprintf (Gbl.F.Out,"<div class=\"EXTERNAL_WWW\">"
			    "<a href=\"%s\" target=\"_blank\" class=\"DAT\">"
	                    "%s"
			    "</a>"
			    "</div>",
		  Gbl.CurrentCtr.Ctr.WWW,
		  Gbl.CurrentCtr.Ctr.WWW);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");

      /***** Shortcut to the centre *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"%s RIGHT_MIDDLE\">"
			 "%s:"
			 "</td>"
			 "<td class=\"DAT LEFT_MIDDLE\">"
			 "<a href=\"%s/%s?ctr=%ld\" class=\"DAT\" target=\"_blank\">"
			 "%s/%s?ctr=%ld"
			 "</a>"
			 "</td>"
			 "</tr>",
	       The_ClassForm[Gbl.Prefs.Theme],
	       Txt_Shortcut,
	       Cfg_URL_SWAD_CGI,
	       Txt_STR_LANG_ID[Gbl.Prefs.Language],
	       Gbl.CurrentCtr.Ctr.CtrCod,
	       Cfg_URL_SWAD_CGI,
	       Txt_STR_LANG_ID[Gbl.Prefs.Language],
	       Gbl.CurrentCtr.Ctr.CtrCod);

      if (PrintView)
	{
	 /***** QR code with link to the centre *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_QR_code);
	 QR_LinkTo (250,"ctr",Gbl.CurrentCtr.Ctr.CtrCod);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");
	}
      else
	{
	 /***** Place *****/
	 Plc.PlcCod = Gbl.CurrentCtr.Ctr.PlcCod;
         Plc_GetDataOfPlaceByCod (&Plc);
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "%s"
			    "</td>"
			    "</tr>",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_Place,
		  Plc.FullName);

	 /***** Number of users who claim to belong to this centre *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_Users_of_the_centre,
		  Usr_GetNumUsrsWhoClaimToBelongToCtr (Gbl.CurrentCtr.Ctr.CtrCod));

	 /***** Number of degrees *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
	                    "%s:"
	                    "</td>"
			    "<td class=\"LEFT_MIDDLE\">",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_Degrees);

	 /* Form to go to see degrees of this centre */
	 Act_FormGoToStart (ActSeeDeg);
	 Ctr_PutParamCtrCod (Gbl.CurrentCtr.Ctr.CtrCod);
	 sprintf (Gbl.Title,Txt_Degrees_of_CENTRE_X,
	          Gbl.CurrentCtr.Ctr.ShrtName);
	 Act_LinkFormSubmit (Gbl.Title,"DAT",NULL);
	 fprintf (Gbl.F.Out,"%u</a>",
		  Deg_GetNumDegsInCtr (Gbl.CurrentCtr.Ctr.CtrCod));
	 Act_FormEnd ();

	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 /***** Number of courses *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_Courses,
		  Crs_GetNumCrssInCtr (Gbl.CurrentCtr.Ctr.CtrCod));

	 /***** Number of teachers in courses of this centre *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_ROLES_PLURAL_Abc[Rol_TEACHER][Usr_SEX_UNKNOWN],
		  Usr_GetNumUsrsInCrssOfCtr (Rol_TEACHER,Gbl.CurrentCtr.Ctr.CtrCod));

	 /***** Number of students in courses of this centre *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_ROLES_PLURAL_Abc[Rol_STUDENT][Usr_SEX_UNKNOWN],
		  Usr_GetNumUsrsInCrssOfCtr (Rol_STUDENT,Gbl.CurrentCtr.Ctr.CtrCod));

	 /***** Number of users in courses of this centre *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s + %s:"
			    "</td>"
			    "<td class=\"DAT LEFT_MIDDLE\">"
			    "%u"
			    "</td>"
			    "</tr>",
		  The_ClassForm[Gbl.Prefs.Theme],
		  Txt_ROLES_PLURAL_Abc[Rol_TEACHER][Usr_SEX_UNKNOWN],
		  Txt_ROLES_PLURAL_Abc[Rol_STUDENT][Usr_SEX_UNKNOWN],
		  Usr_GetNumUsrsInCrssOfCtr (Rol_UNKNOWN,Gbl.CurrentCtr.Ctr.CtrCod));
	}

      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>");

      /***** End frame *****/
      Lay_EndRoundFrame ();
     }
  }

/*****************************************************************************/
/************ Put contextual icons in configuration of a centre **************/
/*****************************************************************************/

static void Ctr_PutIconsToPrintAndUpload (void)
  {
   extern const char *Txt_Print;

   /***** Link to print info about centre *****/
   Lay_PutContextualLink (ActPrnCtrInf,NULL,
                          "print64x64.png",
                          Txt_Print,NULL,
                          NULL);

   if (Gbl.Usrs.Me.LoggedRole >= Rol_CTR_ADM)
      // Only centre admins, institution admins and system admins
      // have permission to upload logo and photo of the centre
     {
      /***** Link to upload logo of centre *****/
      Log_PutIconToChangeLogo (Sco_SCOPE_CTR);

      /***** Link to upload photo of centre *****/
      Ctr_PutIconToChangePhoto ();
     }
  }

/*****************************************************************************/
/************* Put contextual icons to upload photo of centre ****************/
/*****************************************************************************/

static void Ctr_PutIconToChangePhoto (void)
  {
   extern const char *Txt_Change_photo;
   extern const char *Txt_Upload_photo;
   char PathPhoto[PATH_MAX+1];
   bool PhotoExists;

   /***** Link to upload photo of centre *****/
   sprintf (PathPhoto,"%s/%s/%02u/%u/%u.jpg",
	    Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CTR,
	    (unsigned) (Gbl.CurrentCtr.Ctr.CtrCod % 100),
	    (unsigned)  Gbl.CurrentCtr.Ctr.CtrCod,
	    (unsigned)  Gbl.CurrentCtr.Ctr.CtrCod);
   PhotoExists = Fil_CheckIfPathExists (PathPhoto);
   Lay_PutContextualLink (ActReqCtrPho,NULL,
			  "photo64x64.gif",
			  PhotoExists ? Txt_Change_photo :
				        Txt_Upload_photo,NULL,
                          NULL);
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
      Hie_WriteMenuHierarchy ();

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
   extern const char *Hlp_INSTITUTION_Centres;
   extern const char *Txt_Centres_of_INSTITUTION_X;
   extern const char *Txt_No_centres;
   extern const char *Txt_Create_another_centre;
   extern const char *Txt_Create_centre;
   unsigned NumCtr;

   /***** Start frame *****/
   sprintf (Gbl.Title,Txt_Centres_of_INSTITUTION_X,Gbl.CurrentIns.Ins.FullName);
   Lay_StartRoundFrame (NULL,Gbl.Title,
                        Ctr_PutIconsListCentres,Hlp_INSTITUTION_Centres);

   if (Gbl.Ctrs.Num)	// There are centres in the current institution
     {
      /***** Start table *****/
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_MARGIN CELLS_PAD_2\">");
      Ctr_PutHeadCentresForSeeing (true);	// Order selectable

      /***** Write all the centres and their nuber of teachers *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Ctrs.Num;
	   NumCtr++)
	 Ctr_ListOneCentreForSeeing (&(Gbl.Ctrs.Lst[NumCtr]),NumCtr + 1);

      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>");
     }
   else	// No centres created in the current institution
      Lay_ShowAlert (Lay_INFO,Txt_No_centres);

   /***** Button to create centre *****/
   if (Ctr_CheckIfICanCreateCentres ())
     {
      Act_FormStart (ActEdiCtr);
      Lay_PutConfirmButton (Gbl.Ctrs.Num ? Txt_Create_another_centre :
	                                   Txt_Create_centre);
      Act_FormEnd ();
     }

   /***** End frame *****/
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/********************** Check if I can create centres ************************/
/*****************************************************************************/

static bool Ctr_CheckIfICanCreateCentres (void)
  {
   return (bool) (Gbl.Usrs.Me.LoggedRole >= Rol__GUEST_);
  }

/*****************************************************************************/
/***************** Put contextual icons in list of centres *******************/
/*****************************************************************************/

static void Ctr_PutIconsListCentres (void)
  {
   /***** Put icon to edit centres *****/
   if (Ctr_CheckIfICanCreateCentres ())
      Ctr_PutIconToEditCentres ();

   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_HIERARCHY;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/********************** Put link (form) to edit centres **********************/
/*****************************************************************************/

static void Ctr_PutIconToEditCentres (void)
  {
   extern const char *Txt_Edit;

   Lay_PutContextualLink (ActEdiCtr,NULL,
                          "edit64x64.png",
                          Txt_Edit,NULL,
                          NULL);
  }

/*****************************************************************************/
/************************* List one centre for seeing ************************/
/*****************************************************************************/

static void Ctr_ListOneCentreForSeeing (struct Centre *Ctr,unsigned NumCtr)
  {
   extern const char *Txt_CENTRE_STATUS[Ctr_NUM_STATUS_TXT];
   struct Place Plc;
   const char *TxtClassNormal;
   const char *TxtClassStrong;
   const char *BgColor;
   Crs_StatusTxt_t StatusTxt;

   /***** Get data of place of this centre *****/
   Plc.PlcCod = Ctr->PlcCod;
   Plc_GetDataOfPlaceByCod (&Plc);

   if (Ctr->Status & Ctr_STATUS_BIT_PENDING)
     {
      TxtClassNormal = "DAT_LIGHT";
      TxtClassStrong = "DAT_LIGHT";
     }
   else
     {
      TxtClassNormal = "DAT";
      TxtClassStrong = "DAT_N";
     }
   BgColor = (Ctr->CtrCod == Gbl.CurrentCtr.Ctr.CtrCod) ? "LIGHT_BLUE" :
                                                          Gbl.ColorRows[Gbl.RowEvenOdd];

   /***** Number of centre in this list *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s RIGHT_MIDDLE %s\">"
                      "%u"
                      "</td>",
	    TxtClassNormal,BgColor,
            NumCtr);

   /***** Centre logo and name *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE %s\">",BgColor);
   Ctr_DrawCentreLogoAndNameWithLink (Ctr,ActSeeDeg,
                                      TxtClassStrong,"CENTER_MIDDLE");
   fprintf (Gbl.F.Out,"</td>");

   /***** Number of users who claim to belong to this centre *****/
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
		      "%u"
		      "</td>",
	    TxtClassNormal,BgColor,
	    Ctr->NumUsrsWhoClaimToBelongToCtr);

   /***** Place *****/
   fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE %s\">"
		      "%s"
		      "</td>",
	    TxtClassNormal,BgColor,
	    Plc.ShrtName);

   /***** Number of degrees *****/
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
		      "%u"
		      "</td>",
	    TxtClassNormal,BgColor,
	    Ctr->Degs.Num);

   /***** Number of courses *****/
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
		      "%u"
		      "</td>",
	    TxtClassNormal,BgColor,
	    Ctr->NumCrss);

   /***** Number of users in courses of this centre *****/
   fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_MIDDLE %s\">"
		      "%u"
		      "</td>",
	    TxtClassNormal,BgColor,
	    Ctr->NumUsrs);

   /***** Centre status *****/
   StatusTxt = Ctr_GetStatusTxtFromStatusBits (Ctr->Status);
   fprintf (Gbl.F.Out,"<td class=\"%s LEFT_MIDDLE %s\">"
		      "%s"
		      "</td>"
		      "</tr>",
	    TxtClassNormal,BgColor,
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
   /***** Get list of places *****/
   Plc_GetListPlaces ();

   /***** Get list of centres *****/
   Ctr_GetListCentres (Gbl.CurrentIns.Ins.InsCod);

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
         sprintf (OrderBySubQuery,"NumUsrs DESC,FullName");
         break;
     }
   sprintf (Query,"(SELECT centres.CtrCod,centres.InsCod,centres.PlcCod,"
		  "centres.Status,centres.RequesterUsrCod,"
		  "centres.ShortName,centres.FullName,centres.WWW,"
		  "COUNT(DISTINCT usr_data.UsrCod) AS NumUsrs"
		  " FROM centres,usr_data"
		  " WHERE centres.InsCod='%ld'"
		  " AND centres.CtrCod=usr_data.CtrCod"
		  " GROUP BY centres.CtrCod)"
		  " UNION "
		  "(SELECT CtrCod,InsCod,PlcCod,Status,RequesterUsrCod,"
		  "ShortName,FullName,WWW,0 AS NumUsrs"
		  " FROM centres"
		  " WHERE centres.InsCod='%ld'"
		  " AND CtrCod NOT IN"
		  " (SELECT DISTINCT CtrCod FROM usr_data))"
		  " ORDER BY %s",
	    InsCod,
	    InsCod,
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
         strcpy (Ctr->ShrtName,row[5]);

         /* Get the full name of the centre (row[6]) */
         strcpy (Ctr->FullName,row[6]);

         /* Get the URL of the centre (row[7]) */
         strcpy (Ctr->WWW,row[7]);

         /* Get number of users who claim to belong to this centre (row[8]) */
         if (sscanf (row[8],"%u",&Ctr->NumUsrsWhoClaimToBelongToCtr) != 1)
            Ctr->NumUsrsWhoClaimToBelongToCtr = 0;

         /* Get number of degrees in this centre */
         Ctr->Degs.Num = Deg_GetNumDegsInCtr (Ctr->CtrCod);

         /* Get number of courses in this centre */
         Ctr->NumCrss = Crs_GetNumCrssInCtr (Ctr->CtrCod);

	 /* Get number of users in courses of this centre */
	 Ctr->NumUsrs = Usr_GetNumUsrsInCrssOfCtr (Rol_UNKNOWN,Ctr->CtrCod);	// Here Rol_UNKNOWN means "all users"
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
   Ctr->ShrtName[0] = '\0';
   Ctr->FullName[0]  = '\0';
   Ctr->WWW[0]       = '\0';
   Ctr->NumUsrsWhoClaimToBelongToCtr = 0;
   Ctr->Degs.Num =
   Ctr->NumCrss  = 0;
   Ctr->NumUsrs = 0;

   /***** Check if centre code is correct *****/
   if (Ctr->CtrCod > 0)
     {
      /***** Get data of a centre from database *****/
      sprintf (Query,"(SELECT centres.InsCod,centres.PlcCod,"
	             "centres.Status,centres.RequesterUsrCod,"
	             "centres.ShortName,centres.FullName,centres.WWW,"
	             "COUNT(DISTINCT usr_data.UsrCod) AS NumUsrs"
                     " FROM centres,usr_data"
                     " WHERE centres.CtrCod='%ld'"
                     " AND centres.CtrCod=usr_data.CtrCod"
                     " GROUP BY centres.CtrCod)"
                     " UNION "
                     "(SELECT InsCod,PlcCod,"
                     "Status,RequesterUsrCod,"
                     "ShortName,FullName,WWW,"
                     "0 AS NumUsrs"
                     " FROM centres"
                     " WHERE CtrCod='%ld'"
                     " AND CtrCod NOT IN"
                     " (SELECT DISTINCT CtrCod FROM usr_data))",
               Ctr->CtrCod,
               Ctr->CtrCod);
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
         strcpy (Ctr->ShrtName,row[4]);

         /* Get the full name of the centre (row[5]) */
         strcpy (Ctr->FullName,row[5]);

         /* Get the URL of the centre (row[6]) */
         strcpy (Ctr->WWW,row[6]);

         /* Get number of users who claim to belong to this centre (row[7]) */
         if (sscanf (row[7],"%u",&Ctr->NumUsrsWhoClaimToBelongToCtr) != 1)
            Ctr->NumUsrsWhoClaimToBelongToCtr = 0;

         /* Get number of degrees in this centre */
         Ctr->Degs.Num = Deg_GetNumDegsInCtr (Ctr->CtrCod);

         /* Get number of courses in this centre */
         Ctr->NumCrss = Crs_GetNumCrssInCtr (Ctr->CtrCod);

	 /* Get number of users in courses of this centre */
	 Ctr->NumUsrs = Usr_GetNumUsrsInCrssOfCtr (Rol_UNKNOWN,Ctr->CtrCod);	// Here Rol_UNKNOWN means "all users"

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
/*************** Get the short name of a centre from its code ****************/
/*****************************************************************************/

void Ctr_GetShortNameOfCentreByCod (struct Centre *Ctr)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   Ctr->ShrtName[0] = '\0';
   if (Ctr->CtrCod > 0)
     {
      /***** Get the short name of a centre from database *****/
      sprintf (Query,"SELECT ShortName FROM centres"
		     " WHERE CtrCod ='%ld'",
	       Ctr->CtrCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get the short name of a centre") == 1)
	{
	 /***** Get the short name of this centre *****/
	 row = mysql_fetch_row (mysql_res);
	 strcpy (Ctr->ShrtName,row[0]);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
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

void Ctr_WriteSelectorOfCentre (void)
  {
   extern const char *Txt_Centre;
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtrs;
   unsigned NumCtr;
   long CtrCod;

   /***** Start form *****/
   Act_FormGoToStart (ActSeeDeg);
   fprintf (Gbl.F.Out,"<select name=\"ctr\" style=\"width:175px;\"");
   if (Gbl.CurrentIns.Ins.InsCod > 0)
      fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
               Gbl.Form.Id);
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
   fprintf (Gbl.F.Out,"</select>");
   Act_FormEnd ();
  }

/*****************************************************************************/
/*************************** List all the centres ****************************/
/*****************************************************************************/

#define Ctr_MAX_LENGTH_WWW_ON_SCREEN 10

static void Ctr_ListCentresForEdition (void)
  {
   extern const char *Hlp_INSTITUTION_Centres;
   extern const char *Txt_Centres_of_INSTITUTION_X;
   extern const char *Txt_Another_place;
   extern const char *Txt_CENTRE_STATUS[Ctr_NUM_STATUS_TXT];
   unsigned NumCtr;
   struct Centre *Ctr;
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
   Lay_StartRoundFrameTable (NULL,Gbl.Title,NULL,Hlp_INSTITUTION_Centres,2);
   Ctr_PutHeadCentresForEdition ();

   /***** Write all the centres *****/
   for (NumCtr = 0;
	NumCtr < Gbl.Ctrs.Num;
	NumCtr++)
     {
      Ctr = &Gbl.Ctrs.Lst[NumCtr];

      ICanEdit = Ctr_CheckIfICanEditACentre (Ctr);

      /* Put icon to remove centre */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"BM\">");
      if (Ctr->Degs.Num ||
	  Ctr->NumUsrsWhoClaimToBelongToCtr ||
	  Ctr->NumUsrs ||	// Centre has degrees or users ==> deletion forbidden
          !ICanEdit)
	 Lay_PutIconRemovalNotAllowed ();
      else
        {
         Act_FormStart (ActRemCtr);
         Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
         Lay_PutIconRemove ();
         Act_FormEnd ();
        }
      fprintf (Gbl.F.Out,"</td>");

      /* Centre code */
      fprintf (Gbl.F.Out,"<td class=\"DAT CODE\">"
	                 "%ld"
	                 "</td>",
               Ctr->CtrCod);

      /* Centre logo */
      fprintf (Gbl.F.Out,"<td title=\"%s\" class=\"LEFT_MIDDLE\""
	                 " style=\"width:25px;\">",
               Ctr->FullName);
      Log_DrawLogo (Sco_SCOPE_CTR,Ctr->CtrCod,Ctr->ShrtName,20,NULL,true);
      fprintf (Gbl.F.Out,"</td>");

      /* Place */
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (ICanEdit)
	{
	 Act_FormStart (ActChgCtrPlc);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<select name=\"PlcCod\" style=\"width:62px;\""
			    " onchange=\"document.getElementById('%s').submit();\">",
		  Gbl.Form.Id);
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
		     Gbl.Plcs.Lst[NumPlc].ShrtName);
	 fprintf (Gbl.F.Out,"</select>");
	 Act_FormEnd ();
	}
      else
	 for (NumPlc = 0;
	      NumPlc < Gbl.Plcs.Num;
	      NumPlc++)
	    if (Gbl.Plcs.Lst[NumPlc].PlcCod == Ctr->PlcCod)
	       fprintf (Gbl.F.Out,"%s",Gbl.Plcs.Lst[NumPlc].ShrtName);
      fprintf (Gbl.F.Out,"</td>");

      /* Centre short name */
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (ICanEdit)
	{
	 Act_FormStart (ActRenCtrSho);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_SHORT_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Ctr_MAX_LENGTH_CENTRE_SHRT_NAME,Ctr->ShrtName,Gbl.Form.Id);
	 Act_FormEnd ();
	 fprintf (Gbl.F.Out,"</td>");
	}
      else
	 fprintf (Gbl.F.Out,"%s",Ctr->ShrtName);
      fprintf (Gbl.F.Out,"</td>");

      /* Centre full name */
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (ICanEdit)
	{
	 Act_FormStart (ActRenCtrFul);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_FULL_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Ctr_MAX_LENGTH_CENTRE_FULL_NAME,Ctr->FullName,Gbl.Form.Id);
	 Act_FormEnd ();
	 fprintf (Gbl.F.Out,"</td>");
	}
      else
	 fprintf (Gbl.F.Out,"%s",Ctr->FullName);
      fprintf (Gbl.F.Out,"</td>");

      /* Centre WWW */
      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">");
      if (ICanEdit)
	{
	 Act_FormStart (ActChgCtrWWW);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<input type=\"url\" name=\"WWW\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_WWW\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Cns_MAX_LENGTH_WWW,Ctr->WWW,Gbl.Form.Id);
	 Act_FormEnd ();
	}
      else
	{
         strncpy (WWW,Ctr->WWW,Ctr_MAX_LENGTH_WWW_ON_SCREEN);
         WWW[Ctr_MAX_LENGTH_WWW_ON_SCREEN] = '\0';
         fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\" class=\"DAT\" title=\"%s\">%s",
                  Ctr->WWW,Ctr->WWW,WWW);
         if (strlen (Ctr->WWW) > Ctr_MAX_LENGTH_WWW_ON_SCREEN)
            fprintf (Gbl.F.Out,"&hellip;");
         fprintf (Gbl.F.Out,"</a>");
	}
      fprintf (Gbl.F.Out,"</td>");

      /* Number of users who claim to belong to this centre */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
			 "%u"
			 "</td>",
	       Ctr->NumUsrsWhoClaimToBelongToCtr);

      /* Number of degrees */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%u"
	                 "</td>",
               Ctr->Degs.Num);

      /* Number of users in courses of this centre */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%u"
	                 "</td>",
               Ctr->NumUsrs);

      /* Centre status */
      StatusTxt = Ctr_GetStatusTxtFromStatusBits (Ctr->Status);
      fprintf (Gbl.F.Out,"<td class=\"DAT STATUS\">");
      if (Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM &&
	  StatusTxt == Ctr_STATUS_PENDING)
	{
	 Act_FormStart (ActChgCtrSta);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<select name=\"Status\" class=\"INPUT_STATUS\""
			    " onchange=\"document.getElementById('%s').submit();\">"
			    "<option value=\"%u\" selected=\"selected\">%s</option>"
			    "<option value=\"%u\">%s</option>"
			    "</select>",
		  Gbl.Form.Id,
		  (unsigned) Ctr_GetStatusBitsFromStatusTxt (Ctr_STATUS_PENDING),
		  Txt_CENTRE_STATUS[Ctr_STATUS_PENDING],
		  (unsigned) Ctr_GetStatusBitsFromStatusTxt (Ctr_STATUS_ACTIVE),
		  Txt_CENTRE_STATUS[Ctr_STATUS_ACTIVE]);
	 Act_FormEnd ();
	}
      else
	 fprintf (Gbl.F.Out,"%s",Txt_CENTRE_STATUS[StatusTxt]);
      fprintf (Gbl.F.Out,"</td>");

      /* Centre requester */
      UsrDat.UsrCod = Ctr->RequesterUsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);
      fprintf (Gbl.F.Out,"<td class=\"INPUT_REQUESTER LEFT_TOP\">"
			 "<table class=\"INPUT_REQUESTER CELLS_PAD_2\">"
			 "<tr>");
      Msg_WriteMsgAuthor (&UsrDat,100,6,"DAT",true,NULL);
      fprintf (Gbl.F.Out,"</tr>"
			 "</table>"
			 "</td>"
			 "</tr>");
     }

   /***** End table *****/
   Lay_EndRoundFrameTable ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************** Check if I can edit, remove, etc. a centre *******************/
/*****************************************************************************/

static bool Ctr_CheckIfICanEditACentre (struct Centre *Ctr)
  {
   return (bool) (Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM ||			// I am an institution administrator or higher
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
   Par_PutHiddenParamLong ("ctr",CtrCod);
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

long Ctr_GetAndCheckParamOtherCtrCod (void)
  {
   long CtrCod;

   /***** Get and check parameter with code of centre *****/
   if ((CtrCod = Ctr_GetParamOtherCtrCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of centre is missing.");

   return CtrCod;
  }

static long Ctr_GetParamOtherCtrCod (void)
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
   Ctr.CtrCod = Ctr_GetAndCheckParamOtherCtrCod ();

   /***** Get data of the centre from database *****/
   Ctr_GetDataOfCentreByCod (&Ctr);

   /***** Check if this centre has teachers *****/
   if (Ctr.Degs.Num ||
       Ctr.NumUsrsWhoClaimToBelongToCtr ||
       Ctr.NumUsrs)	// Centre has degrees or users ==> don't remove
      Lay_ShowAlert (Lay_WARNING,Txt_To_remove_a_centre_you_must_first_remove_all_degrees_and_teachers_in_the_centre);
   else	// Centre has no teachers ==> remove it
     {
      /***** Remove all the threads and posts in forums of the centre *****/
      For_RemoveForums (Sco_SCOPE_CTR,Ctr.CtrCod);

      /***** Remove surveys of the centre *****/
      Svy_RemoveSurveys (Sco_SCOPE_CTR,Ctr.CtrCod);

      /***** Remove information related to files in centre *****/
      Brw_RemoveCtrFilesFromDB (Ctr.CtrCod);

      /***** Remove directories of the centre *****/
      sprintf (PathCtr,"%s/%s/%02u/%u",
	       Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CTR,
	       (unsigned) (Ctr.CtrCod % 100),
	       (unsigned) Ctr.CtrCod);
      Fil_RemoveTree (PathCtr);

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

void Ctr_ChangeCtrInsInConfig (void)
  {
   extern const char *Txt_The_centre_X_already_exists;
   extern const char *Txt_The_centre_X_has_been_moved_to_the_institution_Y;
   struct Instit NewIns;

   /***** Get parameter with institution code *****/
   NewIns.InsCod = Ins_GetAndCheckParamOtherInsCod ();

   /***** Check if institution has changed *****/
   if (NewIns.InsCod != Gbl.CurrentCtr.Ctr.InsCod)
     {
      /***** Get data of new institution *****/
      Ins_GetDataOfInstitutionByCod (&NewIns,Ins_GET_BASIC_DATA);

      /***** Check if it already exists a centre with the same name in the new institution *****/
      if (Ctr_CheckIfCtrNameExistsInIns ("ShortName",Gbl.CurrentCtr.Ctr.ShrtName,Gbl.CurrentCtr.Ctr.CtrCod,NewIns.InsCod))
	{
	 Gbl.Error = true;
	 sprintf (Gbl.Message,Txt_The_centre_X_already_exists,
		  Gbl.CurrentCtr.Ctr.ShrtName);
	}
      else if (Ctr_CheckIfCtrNameExistsInIns ("FullName",Gbl.CurrentCtr.Ctr.FullName,Gbl.CurrentCtr.Ctr.CtrCod,NewIns.InsCod))
	{
	 Gbl.Error = true;
	 sprintf (Gbl.Message,Txt_The_centre_X_already_exists,
		  Gbl.CurrentCtr.Ctr.FullName);
	}
      else
	{
	 /***** Update institution in table of centres *****/
	 Ctr_UpdateCtrInsDB (Gbl.CurrentCtr.Ctr.CtrCod,NewIns.InsCod);
	 Gbl.CurrentCtr.Ctr.InsCod =
	 Gbl.CurrentIns.Ins.InsCod = NewIns.InsCod;

	 /***** Initialize again current course, degree, centre... *****/
	 Hie_InitHierarchy ();

	 /***** Write message to show the change made *****/
	 sprintf (Gbl.Message,Txt_The_centre_X_has_been_moved_to_the_institution_Y,
		  Gbl.CurrentCtr.Ctr.FullName,NewIns.FullName);
	}
     }
  }

/*****************************************************************************/
/** Show message of success after changing a centre in centre configuration **/
/*****************************************************************************/

void Ctr_ContEditAfterChgCtrInConfig (void)
  {
   /***** Write error/success message *****/
   Lay_ShowAlert (Gbl.Error ? Lay_WARNING :
			      Lay_SUCCESS,
		  Gbl.Message);

   /***** Show the form again *****/
   Ctr_ShowConfiguration ();
  }

/*****************************************************************************/
/******************* Update institution in table of centres ******************/
/*****************************************************************************/

static void Ctr_UpdateCtrInsDB (long CtrCod,long InsCod)
  {
   char Query[128];

   /***** Update institution in table of centres *****/
   sprintf (Query,"UPDATE centres SET InsCod='%ld' WHERE CtrCod='%ld'",
            InsCod,CtrCod);
   DB_QueryUPDATE (Query,"can not update the institution of a centre");
  }

/*****************************************************************************/
/************************ Change the place of a centre ***********************/
/*****************************************************************************/

void Ctr_ChangeCentrePlace (void)
  {
   extern const char *Txt_The_place_of_the_centre_has_changed;
   struct Centre *Ctr;
   long NewPlcCod;
   char Query[512];

   Ctr = &Gbl.Ctrs.EditingCtr;

   /***** Get parameters from form *****/
   /* Get centre code */
   Ctr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod ();

   /* Get parameter with centre code */
   NewPlcCod = Plc_GetParamPlcCod ();

   /***** Get data of centre from database *****/
   Ctr_GetDataOfCentreByCod (Ctr);

   /***** Update place in table of centres *****/
   sprintf (Query,"UPDATE centres SET PlcCod='%ld' WHERE CtrCod='%ld'",
            NewPlcCod,Ctr->CtrCod);
   DB_QueryUPDATE (Query,"can not update the place of a centre");
   Ctr->PlcCod = NewPlcCod;

   /***** Write message to show the change made *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_The_place_of_the_centre_has_changed);

   /***** Put button to go to centre changed *****/
   Ctr_PutButtonToGoToCtr (Ctr);

   /***** Show the form again *****/
   Ctr_EditCentres ();
  }

/*****************************************************************************/
/********************* Change the short name of a centre *********************/
/*****************************************************************************/

void Ctr_RenameCentreShort (void)
  {
   Gbl.Ctrs.EditingCtr.CtrCod = Ctr_GetAndCheckParamOtherCtrCod ();
   Ctr_RenameCentre (&Gbl.Ctrs.EditingCtr,Cns_SHRT_NAME);
  }

void Ctr_RenameCentreShortInConfig (void)
  {
   Ctr_RenameCentre (&Gbl.CurrentCtr.Ctr,Cns_SHRT_NAME);
  }

/*****************************************************************************/
/********************* Change the full name of a centre **********************/
/*****************************************************************************/

void Ctr_RenameCentreFull (void)
  {
   Gbl.Ctrs.EditingCtr.CtrCod = Ctr_GetAndCheckParamOtherCtrCod ();
   Ctr_RenameCentre (&Gbl.Ctrs.EditingCtr,Cns_FULL_NAME);
  }

void Ctr_RenameCentreFullInConfig (void)
  {
   Ctr_RenameCentre (&Gbl.CurrentCtr.Ctr,Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a centre ************************/
/*****************************************************************************/

static void Ctr_RenameCentre (struct Centre *Ctr,Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_centre_X_empty;
   extern const char *Txt_The_centre_X_already_exists;
   extern const char *Txt_The_centre_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_centre_X_has_not_changed;
   char Query[128+Ctr_MAX_LENGTH_CENTRE_FULL_NAME];
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxLength = 0;		// Initialized to avoid warning
   char *CurrentCtrName = NULL;		// Initialized to avoid warning
   char NewCtrName[Ctr_MAX_LENGTH_CENTRE_FULL_NAME+1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxLength = Ctr_MAX_LENGTH_CENTRE_SHRT_NAME;
         CurrentCtrName = Ctr->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxLength = Ctr_MAX_LENGTH_CENTRE_FULL_NAME;
         CurrentCtrName = Ctr->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the new name for the centre */
   Par_GetParToText (ParamName,NewCtrName,MaxLength);

   /***** Get from the database the old names of the centre *****/
   Ctr_GetDataOfCentreByCod (Ctr);

   /***** Check if new name is empty *****/
   if (!NewCtrName[0])
     {
      Gbl.Error = true;
      sprintf (Gbl.Message,Txt_You_can_not_leave_the_name_of_the_centre_X_empty,
               CurrentCtrName);
     }
   else
     {
      /***** Check if old and new names are the same (this happens when user press enter with no changes in the form) *****/
      if (strcmp (CurrentCtrName,NewCtrName))	// Different names
        {
         /***** If degree was in database... *****/
         if (Ctr_CheckIfCtrNameExistsInIns (ParamName,NewCtrName,Ctr->CtrCod,Gbl.CurrentIns.Ins.InsCod))
           {
            Gbl.Error = true;
            sprintf (Gbl.Message,Txt_The_centre_X_already_exists,
                     NewCtrName);
           }
         else
           {
            /* Update the table changing old name by new name */
            sprintf (Query,"UPDATE centres SET %s='%s' WHERE CtrCod='%ld'",
                     FieldName,NewCtrName,Ctr->CtrCod);
            DB_QueryUPDATE (Query,"can not update the name of a centre");

            /* Write message to show the change made */
            sprintf (Gbl.Message,Txt_The_centre_X_has_been_renamed_as_Y,
                     CurrentCtrName,NewCtrName);

	    /* Change current centre name in order to display it properly */
	    strncpy (CurrentCtrName,NewCtrName,MaxLength);
	    CurrentCtrName[MaxLength] = '\0';
           }
        }
      else	// The same name
         sprintf (Gbl.Message,Txt_The_name_of_the_centre_X_has_not_changed,
                  CurrentCtrName);
     }
  }

/*****************************************************************************/
/********************* Check if the name of centre exists ********************/
/*****************************************************************************/

static bool Ctr_CheckIfCtrNameExistsInIns (const char *FieldName,const char *Name,long CtrCod,long InsCod)
  {
   char Query[256+Ctr_MAX_LENGTH_CENTRE_FULL_NAME];

   /***** Get number of centres with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM centres"
	          " WHERE InsCod='%ld' AND %s='%s' AND CtrCod<>'%ld'",
            InsCod,FieldName,Name,CtrCod);
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
   char NewWWW[Cns_MAX_LENGTH_WWW+1];

   Ctr = &Gbl.Ctrs.EditingCtr;

   /***** Get parameters from form *****/
   /* Get the code of the centre */
   Ctr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod ();

   /* Get the new WWW for the centre */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_LENGTH_WWW);

   /***** Get data of centre *****/
   Ctr_GetDataOfCentreByCod (Ctr);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ctr_UpdateCtrWWWDB (Ctr->CtrCod,NewWWW);
      strcpy (Ctr->WWW,NewWWW);

      /***** Write message to show the change made *****/
      sprintf (Gbl.Message,Txt_The_new_web_address_is_X,NewWWW);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

      /***** Put button to go to centre changed *****/
      Ctr_PutButtonToGoToCtr (Ctr);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_leave_the_web_address_empty);

   /***** Show the form again *****/
   Ctr_EditCentres ();
  }

void Ctr_ChangeCtrWWWInConfig (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   extern const char *Txt_You_can_not_leave_the_web_address_empty;
   char NewWWW[Cns_MAX_LENGTH_WWW+1];

   /***** Get parameters from form *****/
   /* Get the new WWW for the centre */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_LENGTH_WWW);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ctr_UpdateCtrWWWDB (Gbl.CurrentCtr.Ctr.CtrCod,NewWWW);
      strcpy (Gbl.CurrentCtr.Ctr.WWW,NewWWW);

      /***** Write message to show the change made *****/
      sprintf (Gbl.Message,Txt_The_new_web_address_is_X,NewWWW);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_leave_the_web_address_empty);

   /***** Show the form again *****/
   Ctr_ShowConfiguration ();
  }

/*****************************************************************************/
/**************** Update database changing old WWW by new WWW ****************/
/*****************************************************************************/

static void Ctr_UpdateCtrWWWDB (long CtrCod,const char NewWWW[Cns_MAX_LENGTH_WWW+1])
  {
   char Query[256+Cns_MAX_LENGTH_WWW];

   /***** Update database changing old WWW by new WWW *****/
   sprintf (Query,"UPDATE centres SET WWW='%s' WHERE CtrCod='%ld'",
	    NewWWW,CtrCod);
   DB_QueryUPDATE (Query,"can not update the web of a centre");
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
   Ctr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod ();

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
            Ctr->ShrtName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Put button to go to centre changed *****/
   Ctr_PutButtonToGoToCtr (Ctr);

   /***** Show the form again *****/
   Ctr_EditCentres ();
  }

/*****************************************************************************/
/************* Show message of success after changing a centre ***************/
/*****************************************************************************/

void Ctr_ContEditAfterChgCtr (void)
  {
   if (Gbl.Error)
      /***** Write error message *****/
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
   else
     {
      /***** Write success message showing the change made *****/
      Lay_ShowAlert (Lay_INFO,Gbl.Message);

      /***** Put button to go to centre changed *****/
      Ctr_PutButtonToGoToCtr (&Gbl.Ctrs.EditingCtr);
     }

   /***** Show the form again *****/
   Ctr_EditCentres ();
  }

/*****************************************************************************/
/************************ Put button to go to centre *************************/
/*****************************************************************************/

static void Ctr_PutButtonToGoToCtr (struct Centre *Ctr)
  {
   extern const char *Txt_Go_to_X;

   // If the centre is different to the current one...
   if (Ctr->CtrCod != Gbl.CurrentCtr.Ctr.CtrCod)
     {
      fprintf (Gbl.F.Out,"<div class=\"BUTTONS_AFTER_ALERT\">");
      Act_FormStart (ActSeeDeg);
      Ctr_PutParamCtrCod (Ctr->CtrCod);
      sprintf (Gbl.Title,Txt_Go_to_X,Ctr->ShrtName);
      Lay_PutConfirmButtonInline (Gbl.Title);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");
     }
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
/*********** Show a form for sending a photo of the current centre ***********/
/*****************************************************************************/

void Ctr_RequestPhoto (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Photo;
   extern const char *Txt_Recommended_aspect_ratio;
   extern const char *Txt_Recommended_resolution;
   extern const char *Txt_XxY_pixels_or_higher;
   extern const char *Txt_File_with_the_photo;

   /***** Start form to upload photo *****/
   Act_FormStart (ActRecCtrPho);

   /***** Start frame *****/
   Lay_StartRoundFrame (NULL,Txt_Photo,NULL,NULL);

   /***** Write help message *****/
   sprintf (Gbl.Message,"%s: %s<br />"
                        "%s: %u&times;%u %s",
            Txt_Recommended_aspect_ratio,
            Ctr_RECOMMENDED_ASPECT_RATIO,
            Txt_Recommended_resolution,
            Ctr_PHOTO_SAVED_MAX_WIDTH,
            Ctr_PHOTO_SAVED_MAX_HEIGHT,
            Txt_XxY_pixels_or_higher);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);

   /***** Upload photo *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">"
	              "%s: "
	              "</label>"
                      "<input type=\"file\" name=\"%s\" accept=\"image/*\""
                      " onchange=\"document.getElementById('%s').submit();\" />",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_File_with_the_photo,
            Fil_NAME_OF_PARAM_FILENAME_ORG,
            Gbl.Form.Id);

   /***** End frame *****/
   Lay_EndRoundFrame ();

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/****************** Receive a photo of the current centre ********************/
/*****************************************************************************/

void Ctr_ReceivePhoto (void)
  {
   extern const char *Txt_Wrong_file_type;
   char Path[PATH_MAX+1];
   struct Param *Param;
   char FileNameImgSrc[PATH_MAX+1];
   char *PtrExtension;
   size_t LengthExtension;
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE+1];
   char PathImgPriv[PATH_MAX+1];
   char FileNameImgTmp[PATH_MAX+1];	// Full name (including path and .jpg) of the destination temporary file
   char FileNameImg[PATH_MAX+1];	// Full name (including path and .jpg) of the destination file
   bool WrongType = false;
   char Command[1024+PATH_MAX*2];
   int ReturnCode;

   /***** Copy in disk the file received *****/
   Param = Fil_StartReceptionOfFile (Fil_NAME_OF_PARAM_FILENAME_ORG,
                                     FileNameImgSrc,MIMEType);

   /* Check if the file type is image/ or application/octet-stream */
   if (strncmp (MIMEType,"image/",strlen ("image/")))
      if (strcmp (MIMEType,"application/octet-stream"))
	 if (strcmp (MIMEType,"application/octetstream"))
	    if (strcmp (MIMEType,"application/octet"))
	       WrongType = true;
   if (WrongType)
     {
      Lay_ShowAlert (Lay_WARNING,Txt_Wrong_file_type);
      return;
     }

   /***** Create private directories if not exist *****/
   /* Create private directory for images if it does not exist */
   sprintf (PathImgPriv,"%s/%s",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG);
   Fil_CreateDirIfNotExists (PathImgPriv);

   /* Create temporary private directory for images if it does not exist */
   sprintf (PathImgPriv,"%s/%s/%s",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP);
   Fil_CreateDirIfNotExists (PathImgPriv);

   /* Get filename extension */
   if ((PtrExtension = strrchr (FileNameImgSrc,(int) '.')) == NULL)
     {
      Lay_ShowAlert (Lay_WARNING,Txt_Wrong_file_type);
      return;
     }
   LengthExtension = strlen (PtrExtension);
   if (LengthExtension < Fil_MIN_LENGTH_FILE_EXTENSION ||
       LengthExtension > Fil_MAX_LENGTH_FILE_EXTENSION)
     {
      Lay_ShowAlert (Lay_WARNING,Txt_Wrong_file_type);
      return;
     }

   /* End the reception of image in a temporary file */
   sprintf (FileNameImgTmp,"%s/%s/%s/%s.%s",
            Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP,
            Gbl.UniqueNameEncrypted,PtrExtension);
   if (!Fil_EndReceptionOfFile (FileNameImgTmp,Param))
     {
      Lay_ShowAlert (Lay_WARNING,"Error copying file.");
      return;
     }

   /***** Creates public directories if not exist *****/
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

   /***** Convert temporary file to public JPEG file *****/
   sprintf (FileNameImg,"%s/%s/%02u/%u/%u.jpg",
	    Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_CTR,
	    (unsigned) (Gbl.CurrentCtr.Ctr.CtrCod % 100),
	    (unsigned) Gbl.CurrentCtr.Ctr.CtrCod,
	    (unsigned) Gbl.CurrentCtr.Ctr.CtrCod);

   /* Call to program that makes the conversion */
   sprintf (Command,"convert %s -resize '%ux%u>' -quality %u %s",
            FileNameImgTmp,
            Ctr_PHOTO_SAVED_MAX_WIDTH,
            Ctr_PHOTO_SAVED_MAX_HEIGHT,
            Ctr_PHOTO_SAVED_QUALITY,
            FileNameImg);
   ReturnCode = system (Command);
   if (ReturnCode == -1)
      Lay_ShowErrorAndExit ("Error when running command to process image.");

   /***** Write message depending on return code *****/
   ReturnCode = WEXITSTATUS(ReturnCode);
   if (ReturnCode != 0)
     {
      sprintf (Gbl.Message,"Image could not be processed successfully.<br />"
			   "Error code returned by the program of processing: %d",
	       ReturnCode);
      Lay_ShowErrorAndExit (Gbl.Message);
     }

   /***** Remove temporary file *****/
   unlink (FileNameImgTmp);

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
   extern const char *Hlp_INSTITUTION_Centres;
   extern const char *Txt_New_centre_of_INSTITUTION_X;
   extern const char *Txt_Another_place;
   extern const char *Txt_CENTRE_STATUS[Ctr_NUM_STATUS_TXT];
   extern const char *Txt_Create_centre;
   struct Centre *Ctr;
   unsigned NumPlc;

   Ctr = &Gbl.Ctrs.EditingCtr;

   /***** Start form *****/
   if (Gbl.Usrs.Me.LoggedRole >= Rol_INS_ADM)
      Act_FormStart (ActNewCtr);
   else if (Gbl.Usrs.Me.MaxRole >= Rol__GUEST_)
      Act_FormStart (ActReqCtr);
   else
      Lay_ShowErrorAndExit ("You can not edit centres.");

   /***** Start of frame *****/
   sprintf (Gbl.Title,Txt_New_centre_of_INSTITUTION_X,
            Gbl.CurrentIns.Ins.ShrtName);
   Lay_StartRoundFrameTable (NULL,Gbl.Title,NULL,Hlp_INSTITUTION_Centres,2);

   /***** Write heading *****/
   Ctr_PutHeadCentresForEdition ();

   /***** Put disabled icon to remove centre *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"BM\">");
   Lay_PutIconRemovalNotAllowed ();
   fprintf (Gbl.F.Out,"</td>");

   /***** Centre code *****/
   fprintf (Gbl.F.Out,"<td class=\"CODE\"></td>");

   /***** Centre logo *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\" style=\"width:25px;\">");
   Log_DrawLogo (Sco_SCOPE_CTR,-1L,"",20,NULL,true);
   fprintf (Gbl.F.Out,"</td>");

   /***** Place *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<select name=\"PlcCod\" style=\"width:62px;\">"
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
               Gbl.Plcs.Lst[NumPlc].ShrtName);
   fprintf (Gbl.F.Out,"</select>"
	              "</td>");

   /***** Centre short name *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" name=\"ShortName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_SHORT_NAME\""
                      " required=\"required\" />"
                      "</td>",
            Ctr_MAX_LENGTH_CENTRE_SHRT_NAME,Ctr->ShrtName);

   /***** Centre full name *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" name=\"FullName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_FULL_NAME\""
                      " required=\"required\" />"
                      "</td>",
            Ctr_MAX_LENGTH_CENTRE_FULL_NAME,Ctr->FullName);

   /***** Centre WWW *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"url\" name=\"WWW\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_WWW\""
                      " required=\"required\" />"
                      "</td>",
            Cns_MAX_LENGTH_WWW,Ctr->WWW);

   /***** Number of users who claim to belong to this centre *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	              "0"
	              "</td>");

   /***** Number of degrees *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	              "0"
	              "</td>");

   /***** Number of users in courses of this centre *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	              "0"
	              "</td>");

   /***** Centre status *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT STATUS\">"
	              "%s"
	              "</td>",
            Txt_CENTRE_STATUS[Ctr_STATUS_PENDING]);

   /***** Centre requester *****/
   fprintf (Gbl.F.Out,"<td class=\"INPUT_REQUESTER LEFT_TOP\">"
		      "<table class=\"INPUT_REQUESTER CELLS_PAD_2\">"
		      "<tr>");
   Msg_WriteMsgAuthor (&Gbl.Usrs.Me.UsrDat,100,6,"DAT",true,NULL);
   fprintf (Gbl.F.Out,"</tr>"
		      "</table>"
		      "</td>"
		      "</tr>");

   /***** Send button and end frame *****/
   Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_centre);

   /***** End of form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************** Write header with fields of a degree *******************/
/*****************************************************************************/

static void Ctr_PutHeadCentresForSeeing (bool OrderSelectable)
  {
   extern const char *Txt_CENTRES_HELP_ORDER[2];
   extern const char *Txt_CENTRES_ORDER[2];
   extern const char *Txt_Place;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_Courses_ABBREVIATION;
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Students_ABBREVIATION;
   extern const char *Txt_Status;
   tCtrsOrderType Order;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>");
   for (Order = Ctr_ORDER_BY_CENTRE;
	Order <= Ctr_ORDER_BY_NUM_TCHS;
	Order++)
     {
      fprintf (Gbl.F.Out,"<th class=\"%s\">",
               Order == Ctr_ORDER_BY_CENTRE ? "LEFT_MIDDLE" :
        	                              "RIGHT_MIDDLE");
      if (OrderSelectable)
	{
	 Act_FormStart (ActSeeCtr);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Act_LinkFormSubmit (Txt_CENTRES_HELP_ORDER[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Ctrs.SelectedOrderType)
	    fprintf (Gbl.F.Out,"<u>");
	}
      fprintf (Gbl.F.Out,"%s",Txt_CENTRES_ORDER[Order]);
      if (OrderSelectable)
	{
	 if (Order == Gbl.Ctrs.SelectedOrderType)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Act_FormEnd ();
	}
      fprintf (Gbl.F.Out,"</th>");
     }
   fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
	              "%s"
	              "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s+<br />%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Place,
	    Txt_Degrees_ABBREVIATION,
	    Txt_Courses_ABBREVIATION,
            Txt_Teachers_ABBREVIATION,Txt_Students_ABBREVIATION,
	    Txt_Status);
  }

/*****************************************************************************/
/******************** Write header with fields of a degree *******************/
/*****************************************************************************/

static void Ctr_PutHeadCentresForEdition (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Place;
   extern const char *Txt_Short_name_of_the_centre;
   extern const char *Txt_Full_name_of_the_centre;
   extern const char *Txt_WWW;
   extern const char *Txt_Users;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_Teachers_ABBREVIATION;
   extern const char *Txt_Students_ABBREVIATION;
   extern const char *Txt_Status;
   extern const char *Txt_Requester;

   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th></th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s+<br />%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Code,
            Txt_Place,
            Txt_Short_name_of_the_centre,
            Txt_Full_name_of_the_centre,
            Txt_WWW,
            Txt_Users,
            Txt_Degrees_ABBREVIATION,
            Txt_Teachers_ABBREVIATION,Txt_Students_ABBREVIATION,
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
   Par_GetParToText ("ShortName",Ctr->ShrtName,Ctr_MAX_LENGTH_CENTRE_SHRT_NAME);

   /* Get centre full name */
   Par_GetParToText ("FullName",Ctr->FullName,Ctr_MAX_LENGTH_CENTRE_FULL_NAME);

   /* Get centre WWW */
   Par_GetParToText ("WWW",Ctr->WWW,Cns_MAX_LENGTH_WWW);

   if (Ctr->ShrtName[0] && Ctr->FullName[0])	// If there's a centre name
     {
      if (Ctr->WWW[0])
        {
         /***** If name of centre was in database... *****/
         if (Ctr_CheckIfCtrNameExistsInIns ("ShortName",Ctr->ShrtName,-1L,Gbl.CurrentIns.Ins.InsCod))
           {
            sprintf (Gbl.Message,Txt_The_centre_X_already_exists,
                     Ctr->ShrtName);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else if (Ctr_CheckIfCtrNameExistsInIns ("FullName",Ctr->FullName,-1L,Gbl.CurrentIns.Ins.InsCod))
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
   char Query[512+
              Ctr_MAX_LENGTH_CENTRE_SHRT_NAME+
              Ctr_MAX_LENGTH_CENTRE_FULL_NAME+
              Cns_MAX_LENGTH_WWW];

   /***** Create a new centre *****/
   sprintf (Query,"INSERT INTO centres (InsCod,PlcCod,Status,RequesterUsrCod,"
                  "ShortName,FullName,WWW,PhotoAttribution)"
                  " VALUES ('%ld','%ld','%u','%ld',"
                  "'%s','%s','%s','')",
            Ctr->InsCod,Ctr->PlcCod,
            Status,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Ctr->ShrtName,Ctr->FullName,Ctr->WWW);
   Ctr->CtrCod = DB_QueryINSERTandReturnCode (Query,"can not create a new centre");

   /***** Write success message *****/
   sprintf (Gbl.Message,Txt_Created_new_centre_X,
            Ctr->FullName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Put button to go to centre created *****/
   Ctr_PutButtonToGoToCtr (Ctr);
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
/******* Get number of centres (of the current institution) in a place *******/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsInPlc (long PlcCod)
  {
   char Query[256];

   /***** Get number of centres (of the current institution) in a place *****/
   sprintf (Query,"SELECT COUNT(*) FROM centres"
	          " WHERE InsCod='%ld' AND PlcCod='%ld'",
	    Gbl.CurrentIns.Ins.InsCod,PlcCod);
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
      /* Number of centres found */
      sprintf (Gbl.Title,"%u %s",
               NumCtrs,(NumCtrs == 1) ? Txt_centre :
	                                Txt_centres);
      Lay_StartRoundFrameTable (NULL,Gbl.Title,NULL,NULL,2);
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

      /***** End table *****/
      Lay_EndRoundFrameTable ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCtrs;
  }
