// swad_centre.c: centres

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#include <linux/stddef.h>	// For NULL
#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For unlink

#include "swad_box.h"
#include "swad_centre.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
#include "swad_HTML.h"
#include "swad_institution.h"
#include "swad_language.h"
#include "swad_logo.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_string.h"

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

static struct Centre *Ctr_EditingCtr = NULL;	// Static variable to keep the centre being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ctr_Configuration (bool PrintView);
static void Ctr_PutIconsCtrConfig (void);
static void Ctr_PutIconToChangePhoto (void);
static void Ctr_ShowNumUsrsInCrssOfCtr (Rol_Role_t Role);

static void Ctr_ListCentres (void);
static bool Ctr_CheckIfICanCreateCentres (void);
static void Ctr_PutIconsListingCentres (void);
static void Ctr_PutIconToEditCentres (void);
static void Ctr_ListOneCentreForSeeing (struct Centre *Ctr,unsigned NumCtr);
static void Ctr_GetParamCtrOrder (void);

static void Ctr_EditCentresInternal (void);
static void Ctr_PutIconsEditingCentres (void);

static void Ctr_GetPhotoAttribution (long CtrCod,char **PhotoAttribution);
static void Ctr_FreePhotoAttribution (char **PhotoAttribution);
static void Ctr_ListCentresForEdition (void);
static bool Ctr_CheckIfICanEditACentre (struct Centre *Ctr);
static Ctr_StatusTxt_t Ctr_GetStatusTxtFromStatusBits (Ctr_Status_t Status);
static Ctr_Status_t Ctr_GetStatusBitsFromStatusTxt (Ctr_StatusTxt_t StatusTxt);

static void Ctr_PutParamOtherCtrCod (long CtrCod);

static void Ctr_UpdateCtrInsDB (long CtrCod,long InsCod);
static void Ctr_UpdateCtrPlcDB (long CtrCod,long NewPlcCod);

static void Ctr_RenameCentre (struct Centre *Ctr,Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Ctr_CheckIfCtrNameExistsInIns (const char *FieldName,const char *Name,long CtrCod,long InsCod);
static void Ctr_UpdateInsNameDB (long CtrCod,const char *FieldName,const char *NewCtrName);

static void Ctr_UpdateCtrWWWDB (long CtrCod,
                                const char NewWWW[Cns_MAX_BYTES_WWW + 1]);
static void Ctr_ShowAlertAndButtonToGoToCtr (void);
static void Ctr_PutParamGoToCtr (void);

static void Ctr_PutFormToCreateCentre (void);
static void Ctr_PutHeadCentresForSeeing (bool OrderSelectable);
static void Ctr_PutHeadCentresForEdition (void);
static void Ctr_RecFormRequestOrCreateCtr (unsigned Status);
static void Ctr_CreateCentre (unsigned Status);

static void Ctr_EditingCentreConstructor (void);
static void Ctr_EditingCentreDestructor (void);

/*****************************************************************************/
/******************* List centres with pending degrees ***********************/
/*****************************************************************************/

void Ctr_SeeCtrWithPendingDegs (void)
  {
   extern const char *Hlp_SYSTEM_Hierarchy_pending;
   extern const char *Txt_Centres_with_pending_degrees;
   extern const char *Txt_Centre;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_There_are_no_centres_with_requests_for_degrees_to_be_confirmed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtrs;
   unsigned NumCtr;
   struct Centre Ctr;
   const char *BgColor;

   /***** Get centres with pending degrees *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_CTR_ADM:
         NumCtrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get centres"
							 " with pending degrees",
					      "SELECT degrees.CtrCod,COUNT(*)"
					      " FROM degrees,ctr_admin,centres"
					      " WHERE (degrees.Status & %u)<>0"
					      " AND degrees.CtrCod=ctr_admin.CtrCod"
					      " AND ctr_admin.UsrCod=%ld"
					      " AND degrees.CtrCod=centres.CtrCod"
					      " GROUP BY degrees.CtrCod ORDER BY centres.ShortName",
					      (unsigned) Deg_STATUS_BIT_PENDING,
					      Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Rol_SYS_ADM:
         NumCtrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get centres"
							 " with pending degrees",
					      "SELECT degrees.CtrCod,COUNT(*)"
					      " FROM degrees,centres"
					      " WHERE (degrees.Status & %u)<>0"
					      " AND degrees.CtrCod=centres.CtrCod"
					      " GROUP BY degrees.CtrCod ORDER BY centres.ShortName",
					      (unsigned) Deg_STATUS_BIT_PENDING);
         break;
      default:	// Forbidden for other users
	 return;
     }
   if (NumCtrs)
     {
      /***** Begin box and table *****/
      Box_StartBoxTable (NULL,Txt_Centres_with_pending_degrees,NULL,
                         Hlp_SYSTEM_Hierarchy_pending,Box_NOT_CLOSABLE,2);

      /***** Wrtie heading *****/
      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"LM",Txt_Centre);
      HTM_TH (1,1,"RM",Txt_Degrees_ABBREVIATION);

      HTM_TR_End ();

      /***** List the centres *****/
      for (NumCtr = 0;
	   NumCtr < NumCtrs;
	   NumCtr++)
        {
         /* Get next centre */
         row = mysql_fetch_row (mysql_res);

         /* Get centre code (row[0]) */
         Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[0]);
         BgColor = (Ctr.CtrCod == Gbl.Hierarchy.Ctr.CtrCod) ? "LIGHT_BLUE" :
                                                              Gbl.ColorRows[Gbl.RowEvenOdd];

         /* Get data of centre */
         Ctr_GetDataOfCentreByCod (&Ctr);

         /* Centre logo and full name */
         HTM_TR_Begin (NULL);

         HTM_TD_Begin ("class=\"LM %s\"",BgColor);
         Ctr_DrawCentreLogoAndNameWithLink (&Ctr,ActSeeDeg,
                                            "DAT_NOBR","CM");
         HTM_TD_End ();

         /* Number of pending degrees (row[1]) */
         HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
	 fprintf (Gbl.F.Out,"%s",row[1]);
         HTM_TD_End ();

         Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
         HTM_TR_End ();
        }

      /***** End table and box *****/
      Box_EndBoxTable ();
     }
   else
      Ale_ShowAlert (Ale_INFO,Txt_There_are_no_centres_with_requests_for_degrees_to_be_confirmed);

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

   /***** Begin form *****/
   Frm_StartFormGoTo (Action);
   Ctr_PutParamCtrCod (Ctr->CtrCod);

   /***** Link to action *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Go_to_X,
	     Ctr->FullName);
   Frm_LinkFormSubmit (Gbl.Title,ClassLink,NULL);

   /***** Centre logo and name *****/
   Log_DrawLogo (Hie_CTR,Ctr->CtrCod,Ctr->ShrtName,16,ClassLogo,true);
   fprintf (Gbl.F.Out,"&nbsp;%s",Ctr->FullName);

   /***** End link *****/
   Frm_LinkFormEnd ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/****************** Show information of the current centre *******************/
/*****************************************************************************/

void Ctr_ShowConfiguration (void)
  {
   Ctr_Configuration (false);

   /***** Show help to enrol me *****/
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
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Short_name;
   extern const char *Txt_Another_place;
   extern const char *Txt_Web;
   extern const char *Txt_Shortcut;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_QR_code;
   extern const char *Txt_Users_of_the_centre;
   extern const char *Txt_Place;
   extern const char *Txt_Degrees;
   extern const char *Txt_Degrees_of_CENTRE_X;
   extern const char *Txt_Courses;
   unsigned NumIns;
   unsigned NumPlc;
   struct Place Plc;
   char PathPhoto[PATH_MAX + 1];
   bool PhotoExists;
   char *PhotoAttribution = NULL;
   bool PutLink;
   char *URL;
   char *Icon;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Ctr.CtrCod <= 0)		// No centre selected
      return;

   /***** Path to photo *****/
   snprintf (PathPhoto,sizeof (PathPhoto),
	     "%s/%02u/%u/%u.jpg",
	     Cfg_PATH_CTR_PUBLIC,
	     (unsigned) (Gbl.Hierarchy.Ctr.CtrCod % 100),
	     (unsigned) Gbl.Hierarchy.Ctr.CtrCod,
	     (unsigned) Gbl.Hierarchy.Ctr.CtrCod);
   PhotoExists = Fil_CheckIfPathExists (PathPhoto);

   /***** Begin box *****/
   if (PrintView)
      Box_BoxBegin (NULL,NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,NULL,Ctr_PutIconsCtrConfig,
		    Hlp_CENTRE_Information,Box_NOT_CLOSABLE);

   /***** Title *****/
   PutLink = !PrintView && Gbl.Hierarchy.Ctr.WWW[0];
   HTM_DIV_Begin ("class=\"FRAME_TITLE FRAME_TITLE_BIG\"");
   if (PutLink)
      HTM_A_Begin ("href=\"%s\" target=\"_blank\""
	           " class=\"FRAME_TITLE_BIG\" title=\"%s\"",
	           Gbl.Hierarchy.Ctr.WWW,
	           Gbl.Hierarchy.Ctr.FullName);
   Log_DrawLogo (Hie_CTR,Gbl.Hierarchy.Ctr.CtrCod,
		 Gbl.Hierarchy.Ctr.ShrtName,64,NULL,true);
   fprintf (Gbl.F.Out,"<br />%s",Gbl.Hierarchy.Ctr.FullName);
   if (PutLink)
      HTM_A_End ();
   HTM_DIV_End ();

   /***** Centre photo *****/
   if (PhotoExists)
     {
      /* Get photo attribution */
      Ctr_GetPhotoAttribution (Gbl.Hierarchy.Ctr.CtrCod,&PhotoAttribution);

      /* Photo image */
      HTM_DIV_Begin ("class=\"DAT_SMALL CM\"");
      if (PutLink)
	 HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"DAT_N\"",
		      Gbl.Hierarchy.Ctr.WWW);
      if (asprintf (&URL,"%s/%02u/%u",
		    Cfg_URL_CTR_PUBLIC,
	            (unsigned) (Gbl.Hierarchy.Ctr.CtrCod % 100),
	            (unsigned) Gbl.Hierarchy.Ctr.CtrCod) < 0)
	 Lay_NotEnoughMemoryExit ();
      if (asprintf (&Icon,"%u.jpg",
		    (unsigned) Gbl.Hierarchy.Ctr.CtrCod) < 0)
	 Lay_NotEnoughMemoryExit ();
      HTM_IMG (URL,Icon,Gbl.Hierarchy.Ctr.FullName,
	       "class=\"%s\"",PrintView ? "CENTRE_PHOTO_PRINT" :
			                  "CENTRE_PHOTO_SHOW");
      free ((void *) Icon);
      free ((void *) URL);
      if (PutLink)
	 HTM_A_End ();
      HTM_DIV_End ();

      /* Photo attribution */
      if (!PrintView &&
	  Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
	 // Only centre admins, institution admins and centre admins
	 // have permission to edit photo attribution
	{
	 HTM_DIV_Begin ("class=\"CM\"");
	 Frm_StartForm (ActChgCtrPhoAtt);
	 HTM_TEXTAREA_Begin ("id=\"AttributionArea\" name=\"Attribution\" rows=\"2\""
			     " onchange=\"document.getElementById('%s').submit();\"",
		             Gbl.Form.Id);
	 if (PhotoAttribution)
	    fprintf (Gbl.F.Out,"%s",PhotoAttribution);
	 HTM_TEXTAREA_End ();
	 Frm_EndForm ();
	 HTM_DIV_End ();
	}
      else if (PhotoAttribution)
	{
	 HTM_DIV_Begin ("class=\"ATTRIBUTION\"");
	 fprintf (Gbl.F.Out,"%s",PhotoAttribution);
	 HTM_DIV_End ();
	}

      /* Free memory used for photo attribution */
      Ctr_FreePhotoAttribution (&PhotoAttribution);
     }

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

   /***** Institution *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   fprintf (Gbl.F.Out,"<label for=\"OthInsCod\" class=\"%s\">%s:",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_Institution);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT_N LM\"");
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      // Only system admins can move a centre to another institution
     {
      /* Get list of institutions of the current country */
      Ins_GetListInstitutions (Gbl.Hierarchy.Cty.CtyCod,Ins_GET_BASIC_DATA);

      /* Put form to select institution */
      Frm_StartForm (ActChgCtrInsCfg);
      fprintf (Gbl.F.Out,"<select id=\"OthInsCod\" name=\"OthInsCod\""
			 " class=\"INPUT_SHORT_NAME\""
			 " onchange=\"document.getElementById('%s').submit();\">",
	       Gbl.Form.Id);
      for (NumIns = 0;
	   NumIns < Gbl.Hierarchy.Cty.Inss.Num;
	   NumIns++)
	 fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
		  Gbl.Hierarchy.Cty.Inss.Lst[NumIns].InsCod,
		  Gbl.Hierarchy.Cty.Inss.Lst[NumIns].InsCod == Gbl.Hierarchy.Ins.InsCod ? " selected=\"selected\"" :
									     "",
		  Gbl.Hierarchy.Cty.Inss.Lst[NumIns].ShrtName);
      fprintf (Gbl.F.Out,"</select>");
      Frm_EndForm ();

      /* Free list of institutions */
      Ins_FreeListInstitutions ();
     }
   else	// I can not move centre to another institution
      fprintf (Gbl.F.Out,"%s",Gbl.Hierarchy.Ins.FullName);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Centre full name *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   fprintf (Gbl.F.Out,"<label for=\"FullName\" class=\"%s\">%s:",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_Centre);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT_N LM\"");
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      // Only institution admins and system admins can edit centre full name
     {
      /* Form to change centre full name */
      Frm_StartForm (ActRenCtrFulCfg);
      fprintf (Gbl.F.Out,"<input type=\"text\""
			 " id=\"FullName\" name=\"FullName\""
			 " maxlength=\"%u\" value=\"%s\""
			 " class=\"INPUT_FULL_NAME\""
			 " onchange=\"document.getElementById('%s').submit();\" />",
	       Hie_MAX_CHARS_FULL_NAME,
	       Gbl.Hierarchy.Ctr.FullName,
	       Gbl.Form.Id);
      Frm_EndForm ();
     }
   else	// I can not edit centre full name
      fprintf (Gbl.F.Out,"%s",Gbl.Hierarchy.Ctr.FullName);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Centre short name *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   fprintf (Gbl.F.Out,"<label for=\"ShortName\" class=\"%s\">%s:",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_Short_name);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT_N LM\"");
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      // Only institution admins and system admins can edit centre short name
     {
      /* Form to change centre short name */
      Frm_StartForm (ActRenCtrShoCfg);
      fprintf (Gbl.F.Out,"<input type=\"text\""
			 " id=\"ShortName\" name=\"ShortName\""
			 " maxlength=\"%u\" value=\"%s\""
			 " class=\"INPUT_SHORT_NAME\""
			 " onchange=\"document.getElementById('%s').submit();\" />",
	       Hie_MAX_CHARS_SHRT_NAME,
	       Gbl.Hierarchy.Ctr.ShrtName,
	       Gbl.Form.Id);
      Frm_EndForm ();
     }
   else	// I can not edit centre short name
      fprintf (Gbl.F.Out,"%s",Gbl.Hierarchy.Ctr.ShrtName);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Place *****/
   Plc.PlcCod = Gbl.Hierarchy.Ctr.PlcCod;
   Plc_GetDataOfPlaceByCod (&Plc);
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s:",Txt_Place);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LM\"");
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
      // Only centre admins, institution admins and system admins
      // can change centre place
     {
      /* Get list of places of the current institution */
      Gbl.Plcs.SelectedOrder = Plc_ORDER_BY_PLACE;
      Plc_GetListPlaces ();

      /* Put form to select place */
      Frm_StartForm (ActChgCtrPlcCfg);
      fprintf (Gbl.F.Out,"<select name=\"PlcCod\" class=\"INPUT_SHORT_NAME\""
			 " onchange=\"document.getElementById('%s').submit();\">",
	       Gbl.Form.Id);
      fprintf (Gbl.F.Out,"<option value=\"0\"");
      if (Gbl.Hierarchy.Ctr.PlcCod == 0)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",Txt_Another_place);
      for (NumPlc = 0;
	   NumPlc < Gbl.Plcs.Num;
	   NumPlc++)
	 fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
		  Gbl.Plcs.Lst[NumPlc].PlcCod,
		  (Gbl.Plcs.Lst[NumPlc].PlcCod == Gbl.Hierarchy.Ctr.PlcCod) ? " selected=\"selected\"" :
									       "",
		  Gbl.Plcs.Lst[NumPlc].ShrtName);
      fprintf (Gbl.F.Out,"</select>");
      Frm_EndForm ();

      /* Free list of places */
      Plc_FreeListPlaces ();
     }
   else	// I can not change centre place
      fprintf (Gbl.F.Out,"%s",Plc.FullName);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Centre WWW *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   fprintf (Gbl.F.Out,"<label for=\"WWW\" class=\"%s\">%s:",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_Web);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LM\"");
   if (!PrintView &&
       Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
      // Only centre admins, institution admins and system admins
      // can change centre WWW
     {
      /* Form to change centre WWW */
      Frm_StartForm (ActChgCtrWWWCfg);
      fprintf (Gbl.F.Out,"<input type=\"url\" id=\"WWW\" name=\"WWW\""
			 " maxlength=\"%u\" value=\"%s\""
			 " class=\"INPUT_WWW\""
			 " onchange=\"document.getElementById('%s').submit();\" />",
	       Cns_MAX_CHARS_WWW,
	       Gbl.Hierarchy.Ctr.WWW,
	       Gbl.Form.Id);
      Frm_EndForm ();
     }
   else	// I can not change centre WWW
     {
      HTM_DIV_Begin ("class=\"EXTERNAL_WWW_LONG\"");
      HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"DAT\"",
	           Gbl.Hierarchy.Ctr.WWW);
      fprintf (Gbl.F.Out,"%s",Gbl.Hierarchy.Ctr.WWW);
      HTM_A_End ();
      HTM_DIV_End ();
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Shortcut to the centre *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s:",Txt_Shortcut);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_A_Begin ("href=\"%s/%s?ctr=%ld\" class=\"DAT\" target=\"_blank\"",
	        Cfg_URL_SWAD_CGI,
	        Lan_STR_LANG_ID[Gbl.Prefs.Language],
	        Gbl.Hierarchy.Ctr.CtrCod);
   fprintf (Gbl.F.Out,"%s/%s?ctr=%ld",
	    Cfg_URL_SWAD_CGI,
	    Lan_STR_LANG_ID[Gbl.Prefs.Language],
	    Gbl.Hierarchy.Ctr.CtrCod);
   HTM_A_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   if (PrintView)
     {
      /***** QR code with link to the centre *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"%s:",Txt_QR_code);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LM\"");
      QR_LinkTo (250,"ctr",Gbl.Hierarchy.Ctr.CtrCod);
      HTM_TD_End ();

      HTM_TR_End ();
     }
   else
     {
      /***** Number of users who claim to belong to this centre *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"%s:",Txt_Users_of_the_centre);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LM\"");
      fprintf (Gbl.F.Out,"%u",Usr_GetNumUsrsWhoClaimToBelongToCtr (Gbl.Hierarchy.Ctr.CtrCod));
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Number of degrees *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"%s:",Txt_Degrees);
      HTM_TD_End ();

      /* Form to go to see degrees of this centre */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartFormGoTo (ActSeeDeg);
      Ctr_PutParamCtrCod (Gbl.Hierarchy.Ctr.CtrCod);
      snprintf (Gbl.Title,sizeof (Gbl.Title),
		Txt_Degrees_of_CENTRE_X,
		Gbl.Hierarchy.Ctr.ShrtName);
      Frm_LinkFormSubmit (Gbl.Title,"DAT",NULL);
      fprintf (Gbl.F.Out,"%u",Deg_GetNumDegsInCtr (Gbl.Hierarchy.Ctr.CtrCod));
      Frm_LinkFormEnd ();
      Frm_EndForm ();
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Number of courses *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      fprintf (Gbl.F.Out,"%s:",Txt_Courses);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LM\"");
      fprintf (Gbl.F.Out,"%u",Crs_GetNumCrssInCtr (Gbl.Hierarchy.Ctr.CtrCod));
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Number of users in courses of this centre *****/
      Ctr_ShowNumUsrsInCrssOfCtr (Rol_TCH);
      Ctr_ShowNumUsrsInCrssOfCtr (Rol_NET);
      Ctr_ShowNumUsrsInCrssOfCtr (Rol_STD);
      Ctr_ShowNumUsrsInCrssOfCtr (Rol_UNK);
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************ Put contextual icons in configuration of a centre **************/
/*****************************************************************************/

static void Ctr_PutIconsCtrConfig (void)
  {
   /***** Put icon to print info about centre *****/
   Ico_PutContextualIconToPrint (ActPrnCtrInf,NULL);

   /***** Put icon to view places *****/
   Plc_PutIconToViewPlaces ();

   if (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
      // Only centre admins, institution admins and system admins
      // have permission to upload logo and photo of the centre
     {
      /***** Put icon to upload logo of centre *****/
      Log_PutIconToChangeLogo (Hie_CTR);

      /***** Put icon to upload photo of centre *****/
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
   char PathPhoto[PATH_MAX + 1];
   bool PhotoExists;

   /***** Link to upload photo of centre *****/
   snprintf (PathPhoto,sizeof (PathPhoto),
	     "%s/%02u/%u/%u.jpg",
	     Cfg_PATH_CTR_PUBLIC,
	     (unsigned) (Gbl.Hierarchy.Ctr.CtrCod % 100),
	     (unsigned)  Gbl.Hierarchy.Ctr.CtrCod,
	     (unsigned)  Gbl.Hierarchy.Ctr.CtrCod);
   PhotoExists = Fil_CheckIfPathExists (PathPhoto);
   Lay_PutContextualLinkOnlyIcon (ActReqCtrPho,NULL,NULL,
			          "camera.svg",
			          PhotoExists ? Txt_Change_photo :
				                Txt_Upload_photo);
  }

/*****************************************************************************/
/**************** Number of users in courses of this centre ******************/
/*****************************************************************************/

static void Ctr_ShowNumUsrsInCrssOfCtr (Rol_Role_t Role)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Users_in_courses;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s:",
	    (Role == Rol_UNK) ? Txt_Users_in_courses :
		                Txt_ROLES_PLURAL_Abc[Role][Usr_SEX_UNKNOWN]);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LM\"");
   fprintf (Gbl.F.Out,"%u",Usr_GetNumUsrsInCrssOfCtr (Role,Gbl.Hierarchy.Ctr.CtrCod));
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/*************** Show the centres of the current institution *****************/
/*****************************************************************************/

void Ctr_ShowCtrsOfCurrentIns (void)
  {
   /***** Trivial check *****/
   if (Gbl.Hierarchy.Ins.InsCod <= 0)		// No institution selected
      return;

   /***** Get parameter with the type of order in the list of centres *****/
   Ctr_GetParamCtrOrder ();

   /***** Get list of centres *****/
   Ctr_GetListCentres (Gbl.Hierarchy.Ins.InsCod);

   /***** Write menu to select country and institution *****/
   Hie_WriteMenuHierarchy ();

   /***** List centres *****/
   Ctr_ListCentres ();

   /***** Free list of centres *****/
   Ctr_FreeListCentres ();
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

   /***** Begin box *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Centres_of_INSTITUTION_X,
	     Gbl.Hierarchy.Ins.FullName);
   Box_BoxBegin (NULL,Gbl.Title,Ctr_PutIconsListingCentres,
                 Hlp_INSTITUTION_Centres,Box_NOT_CLOSABLE);

   if (Gbl.Hierarchy.Ins.Ctrs.Num)	// There are centres in the current institution
     {
      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (2);
      Ctr_PutHeadCentresForSeeing (true);	// Order selectable

      /***** Write all the centres and their nuber of teachers *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Hierarchy.Ins.Ctrs.Num;
	   NumCtr++)
	 Ctr_ListOneCentreForSeeing (&(Gbl.Hierarchy.Ins.Ctrs.Lst[NumCtr]),NumCtr + 1);

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No centres created in the current institution
      Ale_ShowAlert (Ale_INFO,Txt_No_centres);

   /***** Button to create centre *****/
   if (Ctr_CheckIfICanCreateCentres ())
     {
      Frm_StartForm (ActEdiCtr);
      Btn_PutConfirmButton (Gbl.Hierarchy.Ins.Ctrs.Num ? Txt_Create_another_centre :
	                                   Txt_Create_centre);
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************** Check if I can create centres ************************/
/*****************************************************************************/

static bool Ctr_CheckIfICanCreateCentres (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_GST);
  }

/*****************************************************************************/
/***************** Put contextual icons in list of centres *******************/
/*****************************************************************************/

static void Ctr_PutIconsListingCentres (void)
  {
   /***** Put icon to edit centres *****/
   if (Ctr_CheckIfICanCreateCentres ())
      Ctr_PutIconToEditCentres ();

   /***** Put icon to view places *****/
   Plc_PutIconToViewPlaces ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_HIERARCHY;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/********************** Put link (form) to edit centres **********************/
/*****************************************************************************/

static void Ctr_PutIconToEditCentres (void)
  {
   Ico_PutContextualIconToEdit (ActEdiCtr,NULL);
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
   Ctr_StatusTxt_t StatusTxt;

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
   BgColor = (Ctr->CtrCod == Gbl.Hierarchy.Ctr.CtrCod) ? "LIGHT_BLUE" :
                                                         Gbl.ColorRows[Gbl.RowEvenOdd];

   HTM_TR_Begin (NULL);

   /***** Number of centre in this list *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   fprintf (Gbl.F.Out,"%u",NumCtr);
   HTM_TD_End ();

   /***** Centre logo and name *****/
   HTM_TD_Begin ("class=\"LM %s\"",BgColor);
   Ctr_DrawCentreLogoAndNameWithLink (Ctr,ActSeeDeg,
                                      TxtClassStrong,"CM");
   HTM_TD_End ();

   /***** Number of users who claim to belong to this centre *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   fprintf (Gbl.F.Out,"%u",Ctr->NumUsrsWhoClaimToBelongToCtr);
   HTM_TD_End ();

   /***** Place *****/
   HTM_TD_Begin ("class=\"%s LM %s\"",TxtClassNormal,BgColor);
   fprintf (Gbl.F.Out,"%s",Plc.ShrtName);
   HTM_TD_End ();

   /***** Number of degrees *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   fprintf (Gbl.F.Out,"%u",Ctr->Degs.Num);
   HTM_TD_End ();

   /***** Number of courses *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   fprintf (Gbl.F.Out,"%u",Ctr->NumCrss);
   HTM_TD_End ();

   /***** Number of users in courses of this centre *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   fprintf (Gbl.F.Out,"%u",Ctr->NumUsrs);
   HTM_TD_End ();

   /***** Centre status *****/
   StatusTxt = Ctr_GetStatusTxtFromStatusBits (Ctr->Status);
   HTM_TD_Begin ("class=\"%s LM %s\"",TxtClassNormal,BgColor);
   if (StatusTxt != Ctr_STATUS_ACTIVE) // If active ==> do not show anything
      fprintf (Gbl.F.Out,"%s",Txt_CENTRE_STATUS[StatusTxt]);
   HTM_TD_End ();

   HTM_TR_End ();

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of centres **********/
/*****************************************************************************/

static void Ctr_GetParamCtrOrder (void)
  {
   Gbl.Hierarchy.Ins.Ctrs.SelectedOrder = (Ctr_Order_t)
	                    Par_GetParToUnsignedLong ("Order",
                                                      0,
                                                      Ctr_NUM_ORDERS - 1,
                                                      (unsigned long) Ctr_ORDER_DEFAULT);
  }

/*****************************************************************************/
/************************** Put forms to edit centres ************************/
/*****************************************************************************/

void Ctr_EditCentres (void)
  {
   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Edit centres *****/
   Ctr_EditCentresInternal ();

   /***** Centre destructor *****/
   Ctr_EditingCentreDestructor ();
  }

static void Ctr_EditCentresInternal (void)
  {
   extern const char *Hlp_INSTITUTION_Centres;
   extern const char *Txt_Centres_of_INSTITUTION_X;

   /***** Get list of places *****/
   Gbl.Plcs.SelectedOrder = Plc_ORDER_BY_PLACE;
   Plc_GetListPlaces ();

   /***** Get list of centres *****/
   Gbl.Hierarchy.Ins.Ctrs.SelectedOrder = Ctr_ORDER_BY_CENTRE;
   Ctr_GetListCentres (Gbl.Hierarchy.Ins.InsCod);

   /***** Write menu to select country and institution *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Centres_of_INSTITUTION_X,
             Gbl.Hierarchy.Ins.FullName);
   Box_BoxBegin (NULL,Gbl.Title,Ctr_PutIconsEditingCentres,
                 Hlp_INSTITUTION_Centres,Box_NOT_CLOSABLE);

   /***** Put a form to create a new centre *****/
   Ctr_PutFormToCreateCentre ();

   /***** List current centres *****/
   if (Gbl.Hierarchy.Ins.Ctrs.Num)
      Ctr_ListCentresForEdition ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of centres *****/
   Ctr_FreeListCentres ();

   /***** Free list of places *****/
   Plc_FreeListPlaces ();
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of centres *****************/
/*****************************************************************************/

static void Ctr_PutIconsEditingCentres (void)
  {
   /***** Put icon to view centres *****/
   Ctr_PutIconToViewCentres ();

   /***** Put icon to view places *****/
   Plc_PutIconToViewPlaces ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_HIERARCHY;
   Fig_PutIconToShowFigure ();
  }

void Ctr_PutIconToViewCentres (void)
  {
   extern const char *Txt_Centres;

   Lay_PutContextualLinkOnlyIcon (ActSeeCtr,NULL,NULL,
                                  "building.svg",
                                  Txt_Centres);
  }

/*****************************************************************************/
/***** Get a list with all the centres or with those of an institution *******/
/*****************************************************************************/
// If InsCod <= 0 ==> get all the centres, of any institution
// In InsCod >  0 ==> get only the centres of the specified institution

void Ctr_GetListCentres (long InsCod)
  {
   static const char *OrderBySubQuery[Ctr_NUM_ORDERS] =
     {
      "FullName",		// Ctr_ORDER_BY_CENTRE
      "NumUsrs DESC,FullName",	// Ctr_ORDER_BY_NUM_TCHS
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumCtr;
   struct Centre *Ctr;

   /***** Get centres from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get centres",
			     "(SELECT centres.CtrCod,centres.InsCod,centres.PlcCod,"
			     "centres.Status,centres.RequesterUsrCod,"
			     "centres.ShortName,centres.FullName,centres.WWW,"
			     "COUNT(DISTINCT usr_data.UsrCod) AS NumUsrs"
			     " FROM centres,usr_data"
			     " WHERE centres.InsCod=%ld"
			     " AND centres.CtrCod=usr_data.CtrCod"
			     " GROUP BY centres.CtrCod)"
			     " UNION "
			     "(SELECT CtrCod,InsCod,PlcCod,Status,RequesterUsrCod,"
			     "ShortName,FullName,WWW,0 AS NumUsrs"
			     " FROM centres"
			     " WHERE centres.InsCod=%ld"
			     " AND CtrCod NOT IN"
			     " (SELECT DISTINCT CtrCod FROM usr_data))"
			     " ORDER BY %s",
			     InsCod,
			     InsCod,
			     OrderBySubQuery[Gbl.Hierarchy.Ins.Ctrs.SelectedOrder]);

   if (NumRows) // Centres found...
     {
      // NumRows should be equal to Deg->NumCourses
      Gbl.Hierarchy.Ins.Ctrs.Num = (unsigned) NumRows;

      /***** Create list with courses in degree *****/
      if ((Gbl.Hierarchy.Ins.Ctrs.Lst = (struct Centre *) calloc (NumRows,sizeof (struct Centre))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the centres *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Hierarchy.Ins.Ctrs.Num;
	   NumCtr++)
        {
         Ctr = &(Gbl.Hierarchy.Ins.Ctrs.Lst[NumCtr]);

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
         Str_Copy (Ctr->ShrtName,row[5],
                   Hie_MAX_BYTES_SHRT_NAME);

         /* Get the full name of the centre (row[6]) */
         Str_Copy (Ctr->FullName,row[6],
                   Hie_MAX_BYTES_FULL_NAME);

         /* Get the URL of the centre (row[7]) */
         Str_Copy (Ctr->WWW,row[7],
                   Cns_MAX_BYTES_WWW);

         /* Get number of users who claim to belong to this centre (row[8]) */
         if (sscanf (row[8],"%u",&Ctr->NumUsrsWhoClaimToBelongToCtr) != 1)
            Ctr->NumUsrsWhoClaimToBelongToCtr = 0;

         /* Get number of degrees in this centre */
         Ctr->Degs.Num = Deg_GetNumDegsInCtr (Ctr->CtrCod);

         /* Get number of courses in this centre */
         Ctr->NumCrss = Crs_GetNumCrssInCtr (Ctr->CtrCod);

	 /* Get number of users in courses of this centre */
	 Ctr->NumUsrs = Usr_GetNumUsrsInCrssOfCtr (Rol_UNK,Ctr->CtrCod);	// Here Rol_UNK means "all users"
        }
     }
   else
      Gbl.Hierarchy.Ins.Ctrs.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Get data of centre by code *************************/
/*****************************************************************************/

bool Ctr_GetDataOfCentreByCod (struct Centre *Ctr)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   bool CtrFound = false;

   /***** Clear data *****/
   Ctr->InsCod = -1L;
   Ctr->PlcCod = -1L;
   Ctr->Status = (Ctr_Status_t) 0;
   Ctr->RequesterUsrCod = -1L;
   Ctr->ShrtName[0] = '\0';
   Ctr->FullName[0] = '\0';
   Ctr->WWW[0]      = '\0';
   Ctr->NumUsrsWhoClaimToBelongToCtr = 0;
   Ctr->Degs.Num =
   Ctr->NumCrss  = 0;
   Ctr->NumUsrs  = 0;

   /***** Check if centre code is correct *****/
   if (Ctr->CtrCod > 0)
     {
      /***** Get data of a centre from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a centre",
				"(SELECT centres.InsCod,centres.PlcCod,"
				"centres.Status,centres.RequesterUsrCod,"
				"centres.ShortName,centres.FullName,centres.WWW,"
				"COUNT(DISTINCT usr_data.UsrCod) AS NumUsrs"
				" FROM centres,usr_data"
				" WHERE centres.CtrCod=%ld"
				" AND centres.CtrCod=usr_data.CtrCod"
				" GROUP BY centres.CtrCod)"
				" UNION "
				"(SELECT InsCod,PlcCod,"
				"Status,RequesterUsrCod,"
				"ShortName,FullName,WWW,"
				"0 AS NumUsrs"
				" FROM centres"
				" WHERE CtrCod=%ld"
				" AND CtrCod NOT IN"
				" (SELECT DISTINCT CtrCod FROM usr_data))",
				Ctr->CtrCod,
				Ctr->CtrCod);
      if (NumRows) // Centre found...
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
         Str_Copy (Ctr->ShrtName,row[4],
                   Hie_MAX_BYTES_SHRT_NAME);

         /* Get the full name of the centre (row[5]) */
         Str_Copy (Ctr->FullName,row[5],
                   Hie_MAX_BYTES_FULL_NAME);

         /* Get the URL of the centre (row[6]) */
         Str_Copy (Ctr->WWW,row[6],
                   Cns_MAX_BYTES_WWW);

         /* Get number of users who claim to belong to this centre (row[7]) */
         if (sscanf (row[7],"%u",&Ctr->NumUsrsWhoClaimToBelongToCtr) != 1)
            Ctr->NumUsrsWhoClaimToBelongToCtr = 0;

         /* Get number of degrees in this centre */
         Ctr->Degs.Num = Deg_GetNumDegsInCtr (Ctr->CtrCod);

         /* Get number of courses in this centre */
         Ctr->NumCrss = Crs_GetNumCrssInCtr (Ctr->CtrCod);

	 /* Get number of users in courses of this centre */
	 Ctr->NumUsrs = Usr_GetNumUsrsInCrssOfCtr (Rol_UNK,Ctr->CtrCod);	// Here Rol_UNK means "all users"

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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long InsCod = -1L;

   if (CtrCod > 0)
     {
      /***** Get the institution code of a centre from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get the institution of a centre",
			  "SELECT InsCod FROM centres WHERE CtrCod=%ld",
			  CtrCod) == 1)
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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   Ctr->ShrtName[0] = '\0';
   if (Ctr->CtrCod > 0)
     {
      /***** Get the short name of a centre from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get the short name of a centre",
			  "SELECT ShortName FROM centres"
			  " WHERE CtrCod=%ld",
			  Ctr->CtrCod) == 1)
	{
	 /***** Get the short name of this centre *****/
	 row = mysql_fetch_row (mysql_res);

	 Str_Copy (Ctr->ShrtName,row[0],
	           Hie_MAX_BYTES_SHRT_NAME);
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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   /***** Free possible former photo attribution *****/
   Ctr_FreePhotoAttribution (PhotoAttribution);

   /***** Get photo attribution from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get photo attribution",
		       "SELECT PhotoAttribution"
		       " FROM centres WHERE CtrCod=%ld",
		       CtrCod))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get the attribution of the photo of the centre (row[0]) */
      if (row[0])
	 if (row[0][0])
	   {
	    Length = strlen (row[0]);
	    if (((*PhotoAttribution) = (char *) malloc (Length + 1)) == NULL)
	       Lay_ShowErrorAndExit ("Error allocating memory for photo attribution.");
	    Str_Copy (*PhotoAttribution,row[0],
	              Length);
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
   if (Gbl.Hierarchy.Ins.Ctrs.Lst)
     {
      /***** Free memory used by the list of courses in degree *****/
      free ((void *) Gbl.Hierarchy.Ins.Ctrs.Lst);
      Gbl.Hierarchy.Ins.Ctrs.Lst = NULL;
      Gbl.Hierarchy.Ins.Ctrs.Num = 0;
     }
  }

/*****************************************************************************/
/************************** Write selector of centre *************************/
/*****************************************************************************/

void Ctr_WriteSelectorOfCentre (void)
  {
   extern const char *Txt_Centre;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtrs;
   unsigned NumCtr;
   long CtrCod;

   /***** Begin form *****/
   Frm_StartFormGoTo (ActSeeDeg);
   fprintf (Gbl.F.Out,"<select id=\"ctr\" name=\"ctr\" class=\"HIE_SEL\"");
   if (Gbl.Hierarchy.Ins.InsCod > 0)
      fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
               Gbl.Form.Id);
   else
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,"><option value=\"\"");
   if (Gbl.Hierarchy.Ctr.CtrCod < 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out," disabled=\"disabled\">[%s]</option>",
            Txt_Centre);

   if (Gbl.Hierarchy.Ins.InsCod > 0)
     {
      /***** Get centres from database *****/
      NumCtrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get centres",
					   "SELECT DISTINCT CtrCod,ShortName"
					   " FROM centres"
					   " WHERE InsCod=%ld"
					   " ORDER BY ShortName",
					   Gbl.Hierarchy.Ins.InsCod);

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
         if (Gbl.Hierarchy.Ctr.CtrCod > 0 &&
             CtrCod == Gbl.Hierarchy.Ctr.CtrCod)
	    fprintf (Gbl.F.Out," selected=\"selected\"");
         fprintf (Gbl.F.Out,">%s</option>",row[1]);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** End form *****/
   fprintf (Gbl.F.Out,"</select>");
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************************** List all the centres ****************************/
/*****************************************************************************/

static void Ctr_ListCentresForEdition (void)
  {
   extern const char *Txt_Another_place;
   extern const char *Txt_CENTRE_STATUS[Ctr_NUM_STATUS_TXT];
   unsigned NumCtr;
   struct Centre *Ctr;
   unsigned NumPlc;
   char WWW[Cns_MAX_BYTES_WWW + 1];
   struct UsrData UsrDat;
   bool ICanEdit;
   Ctr_StatusTxt_t StatusTxt;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Write heading *****/
   HTM_TABLE_BeginWidePadding (2);
   Ctr_PutHeadCentresForEdition ();

   /***** Write all the centres *****/
   for (NumCtr = 0;
	NumCtr < Gbl.Hierarchy.Ins.Ctrs.Num;
	NumCtr++)
     {
      Ctr = &Gbl.Hierarchy.Ins.Ctrs.Lst[NumCtr];

      ICanEdit = Ctr_CheckIfICanEditACentre (Ctr);

      /* Put icon to remove centre */
      HTM_TR_Begin (NULL);
      HTM_TD_Begin ("class=\"BM\"");
      if (Ctr->Degs.Num ||
	  Ctr->NumUsrsWhoClaimToBelongToCtr ||
	  Ctr->NumUsrs ||	// Centre has degrees or users ==> deletion forbidden
          !ICanEdit)
	 Ico_PutIconRemovalNotAllowed ();
      else
        {
         Frm_StartForm (ActRemCtr);
         Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
         Ico_PutIconRemove ();
         Frm_EndForm ();
        }
      HTM_TD_End ();

      /* Centre code */
      HTM_TD_Begin ("class=\"DAT CODE\"");
      fprintf (Gbl.F.Out,"%ld",Ctr->CtrCod);
      HTM_TD_End ();

      /* Centre logo */
      HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Ctr->FullName);
      Log_DrawLogo (Hie_CTR,Ctr->CtrCod,Ctr->ShrtName,20,NULL,true);
      HTM_TD_End ();

      /* Place */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_StartForm (ActChgCtrPlc);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<select name=\"PlcCod\" class=\"PLC_SEL\""
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
	 Frm_EndForm ();
	}
      else
	 for (NumPlc = 0;
	      NumPlc < Gbl.Plcs.Num;
	      NumPlc++)
	    if (Gbl.Plcs.Lst[NumPlc].PlcCod == Ctr->PlcCod)
	       fprintf (Gbl.F.Out,"%s",Gbl.Plcs.Lst[NumPlc].ShrtName);
      HTM_TD_End ();

      /* Centre short name */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_StartForm (ActRenCtrSho);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_SHORT_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Hie_MAX_CHARS_SHRT_NAME,Ctr->ShrtName,Gbl.Form.Id);
	 Frm_EndForm ();
	}
      else
	 fprintf (Gbl.F.Out,"%s",Ctr->ShrtName);
      HTM_TD_End ();

      /* Centre full name */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_StartForm (ActRenCtrFul);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_FULL_NAME\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Hie_MAX_CHARS_FULL_NAME,Ctr->FullName,Gbl.Form.Id);
	 Frm_EndForm ();
	}
      else
	 fprintf (Gbl.F.Out,"%s",Ctr->FullName);
      HTM_TD_End ();

      /* Centre WWW */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_StartForm (ActChgCtrWWW);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 fprintf (Gbl.F.Out,"<input type=\"url\" name=\"WWW\""
	                    " maxlength=\"%u\" value=\"%s\""
                            " class=\"INPUT_WWW\""
			    " onchange=\"document.getElementById('%s').submit();\" />",
		  Cns_MAX_CHARS_WWW,Ctr->WWW,Gbl.Form.Id);
	 Frm_EndForm ();
	}
      else
	{
         Str_Copy (WWW,Ctr->WWW,
                   Cns_MAX_BYTES_WWW);
         HTM_DIV_Begin ("class=\"EXTERNAL_WWW_SHORT\"");
         HTM_A_Begin ("href=\"%s\" target=\"_blank\""
                      " class=\"DAT\" title=\"%s\"",Ctr->WWW,Ctr->WWW);
         fprintf (Gbl.F.Out,"%s",WWW);
         HTM_A_End ();
         HTM_DIV_End ();
	}
      HTM_TD_End ();

      /* Number of users who claim to belong to this centre */
      HTM_TD_Begin ("class=\"DAT RM\"");
      fprintf (Gbl.F.Out,"%u",Ctr->NumUsrsWhoClaimToBelongToCtr);
      HTM_TD_End ();

      /* Number of degrees */
      HTM_TD_Begin ("class=\"DAT RM\"");
      fprintf (Gbl.F.Out,"%u",Ctr->Degs.Num);
      HTM_TD_End ();

      /* Number of users in courses of this centre */
      HTM_TD_Begin ("class=\"DAT RM\"");
      fprintf (Gbl.F.Out,"%u",Ctr->NumUsrs);
      HTM_TD_End ();

      /* Centre requester */
      UsrDat.UsrCod = Ctr->RequesterUsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);
      HTM_TD_Begin ("class=\"DAT INPUT_REQUESTER LT\"");
      Msg_WriteMsgAuthor (&UsrDat,true,NULL);
      HTM_TD_End ();

      /* Centre status */
      StatusTxt = Ctr_GetStatusTxtFromStatusBits (Ctr->Status);
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM &&
	  StatusTxt == Ctr_STATUS_PENDING)
	{
	 Frm_StartForm (ActChgCtrSta);
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
	 Frm_EndForm ();
	}
      else if (StatusTxt != Ctr_STATUS_ACTIVE)	// If active ==> do not show anything
	 fprintf (Gbl.F.Out,"%s",Txt_CENTRE_STATUS[StatusTxt]);
      HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************** Check if I can edit, remove, etc. a centre *******************/
/*****************************************************************************/

static bool Ctr_CheckIfICanEditACentre (struct Centre *Ctr)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM ||		// I am an institution administrator or higher
                  ((Ctr->Status & Ctr_STATUS_BIT_PENDING) != 0 &&	// Centre is not yet activated
                   Gbl.Usrs.Me.UsrDat.UsrCod == Ctr->RequesterUsrCod));	// I am the requester
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

long Ctr_GetAndCheckParamOtherCtrCod (long MinCodAllowed)
  {
   long CtrCod;

   /***** Get and check parameter with code of centre *****/
   if ((CtrCod = Par_GetParToLong ("OthCtrCod")) < MinCodAllowed)
      Lay_ShowErrorAndExit ("Code of centre is missing or invalid.");

   return CtrCod;
  }

/*****************************************************************************/
/******************************* Remove a centre *****************************/
/*****************************************************************************/

void Ctr_RemoveCentre (void)
  {
   extern const char *Txt_To_remove_a_centre_you_must_first_remove_all_degrees_and_teachers_in_the_centre;
   extern const char *Txt_Centre_X_removed;
   char PathCtr[PATH_MAX + 1];

   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Get centre code *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);

   /***** Get data of the centre from database *****/
   Ctr_GetDataOfCentreByCod (Ctr_EditingCtr);

   /***** Check if this centre has teachers *****/
   if (Ctr_EditingCtr->Degs.Num ||
       Ctr_EditingCtr->NumUsrsWhoClaimToBelongToCtr ||
       Ctr_EditingCtr->NumUsrs)	// Centre has degrees or users ==> don't remove
      Ale_ShowAlert (Ale_WARNING,Txt_To_remove_a_centre_you_must_first_remove_all_degrees_and_teachers_in_the_centre);
   else	// Centre has no teachers ==> remove it
     {
      /***** Remove all the threads and posts in forums of the centre *****/
      For_RemoveForums (Hie_CTR,Ctr_EditingCtr->CtrCod);

      /***** Remove surveys of the centre *****/
      Svy_RemoveSurveys (Hie_CTR,Ctr_EditingCtr->CtrCod);

      /***** Remove information related to files in centre *****/
      Brw_RemoveCtrFilesFromDB (Ctr_EditingCtr->CtrCod);

      /***** Remove all classrooms in centre *****/
      Cla_RemoveAllClassroomsInCtr (Ctr_EditingCtr->CtrCod);

      /***** Remove directories of the centre *****/
      snprintf (PathCtr,sizeof (PathCtr),
	        "%s/%02u/%u",
	        Cfg_PATH_CTR_PUBLIC,
	        (unsigned) (Ctr_EditingCtr->CtrCod % 100),
	        (unsigned) Ctr_EditingCtr->CtrCod);
      Fil_RemoveTree (PathCtr);

      /***** Remove centre *****/
      DB_QueryDELETE ("can not remove a centre",
		      "DELETE FROM centres WHERE CtrCod=%ld",
		      Ctr_EditingCtr->CtrCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Centre_X_removed,
	               Ctr_EditingCtr->FullName);

      Ctr_EditingCtr->CtrCod = -1L;	// To not showing button to go to centre
     }
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
   NewIns.InsCod = Ins_GetAndCheckParamOtherInsCod (1);

   /***** Check if institution has changed *****/
   if (NewIns.InsCod != Gbl.Hierarchy.Ctr.InsCod)
     {
      /***** Get data of new institution *****/
      Ins_GetDataOfInstitutionByCod (&NewIns,Ins_GET_BASIC_DATA);

      /***** Check if it already exists a centre with the same name in the new institution *****/
      if (Ctr_CheckIfCtrNameExistsInIns ("ShortName",
                                         Gbl.Hierarchy.Ctr.ShrtName,
                                         Gbl.Hierarchy.Ctr.CtrCod,
                                         NewIns.InsCod))
	 /***** Create warning message *****/
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_The_centre_X_already_exists,
		          Gbl.Hierarchy.Ctr.ShrtName);
      else if (Ctr_CheckIfCtrNameExistsInIns ("FullName",
                                              Gbl.Hierarchy.Ctr.FullName,
                                              Gbl.Hierarchy.Ctr.CtrCod,
                                              NewIns.InsCod))
	 /***** Create warning message *****/
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_The_centre_X_already_exists,
		          Gbl.Hierarchy.Ctr.FullName);
      else
	{
	 /***** Update institution in table of centres *****/
	 Ctr_UpdateCtrInsDB (Gbl.Hierarchy.Ctr.CtrCod,NewIns.InsCod);
	 Gbl.Hierarchy.Ctr.InsCod =
	 Gbl.Hierarchy.Ins.InsCod = NewIns.InsCod;

	 /***** Initialize again current course, degree, centre... *****/
	 Hie_InitHierarchy ();

	 /***** Create message to show the change made *****/
         Ale_CreateAlert (Ale_SUCCESS,NULL,
                          Txt_The_centre_X_has_been_moved_to_the_institution_Y,
		          Gbl.Hierarchy.Ctr.FullName,NewIns.FullName);
	}
     }
  }

/*****************************************************************************/
/** Show message of success after changing a centre in centre configuration **/
/*****************************************************************************/

void Ctr_ContEditAfterChgCtrInConfig (void)
  {
   /***** Write error/success message *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Ctr_ShowConfiguration ();
  }

/*****************************************************************************/
/******************* Update institution in table of centres ******************/
/*****************************************************************************/

static void Ctr_UpdateCtrInsDB (long CtrCod,long InsCod)
  {
   /***** Update institution in table of centres *****/
   DB_QueryUPDATE ("can not update the institution of a centre",
		   "UPDATE centres SET InsCod=%ld WHERE CtrCod=%ld",
                   InsCod,CtrCod);
  }

/*****************************************************************************/
/************************ Change the place of a centre ***********************/
/*****************************************************************************/

void Ctr_ChangeCtrPlc (void)
  {
   extern const char *Txt_The_place_of_the_centre_has_changed;
   long NewPlcCod;

   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Get centre code *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);

   /***** Get parameter with place code *****/
   NewPlcCod = Plc_GetParamPlcCod ();

   /***** Get data of centre from database *****/
   Ctr_GetDataOfCentreByCod (Ctr_EditingCtr);

   /***** Update place in table of centres *****/
   Ctr_UpdateCtrPlcDB (Ctr_EditingCtr->CtrCod,NewPlcCod);
   Ctr_EditingCtr->PlcCod = NewPlcCod;

   /***** Create alert to show the change made
	  and put button to go to centre changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_place_of_the_centre_has_changed);
  }

void Ctr_ChangeCtrPlcInConfig (void)
  {
   extern const char *Txt_The_place_of_the_centre_has_changed;
   long NewPlcCod;

   /***** Get parameter with place code *****/
   NewPlcCod = Plc_GetParamPlcCod ();

   /***** Update place in table of centres *****/
   Ctr_UpdateCtrPlcDB (Gbl.Hierarchy.Ctr.CtrCod,NewPlcCod);
   Gbl.Hierarchy.Ctr.PlcCod = NewPlcCod;

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_place_of_the_centre_has_changed);

   /***** Show the form again *****/
   Ctr_ShowConfiguration ();
  }

/*****************************************************************************/
/************** Update database changing old place by new place **************/
/*****************************************************************************/

static void Ctr_UpdateCtrPlcDB (long CtrCod,long NewPlcCod)
  {
   DB_QueryUPDATE ("can not update the place of a centre",
		   "UPDATE centres SET PlcCod=%ld WHERE CtrCod=%ld",
	           NewPlcCod,CtrCod);
  }

/*****************************************************************************/
/************************ Change the name of a centre ************************/
/*****************************************************************************/

void Ctr_RenameCentreShort (void)
  {
   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Rename centre *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);
   Ctr_RenameCentre (Ctr_EditingCtr,Cns_SHRT_NAME);
  }

void Ctr_RenameCentreFull (void)
  {
   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Rename centre *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);
   Ctr_RenameCentre (Ctr_EditingCtr,Cns_FULL_NAME);
  }

/*****************************************************************************/
/*************** Change the name of a centre in configuration ****************/
/*****************************************************************************/

void Ctr_RenameCentreShortInConfig (void)
  {
   Ctr_RenameCentre (&Gbl.Hierarchy.Ctr,Cns_SHRT_NAME);
  }

void Ctr_RenameCentreFullInConfig (void)
  {
   Ctr_RenameCentre (&Gbl.Hierarchy.Ctr,Cns_FULL_NAME);
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
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentCtrName = NULL;		// Initialized to avoid warning
   char NewCtrName[Hie_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Hie_MAX_BYTES_SHRT_NAME;
         CurrentCtrName = Ctr->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Hie_MAX_BYTES_FULL_NAME;
         CurrentCtrName = Ctr->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the new name for the centre */
   Par_GetParToText (ParamName,NewCtrName,MaxBytes);

   /***** Get from the database the old names of the centre *****/
   Ctr_GetDataOfCentreByCod (Ctr);

   /***** Check if new name is empty *****/
   if (!NewCtrName[0])
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_leave_the_name_of_the_centre_X_empty,
                       CurrentCtrName);
   else
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentCtrName,NewCtrName))	// Different names
        {
         /***** If degree was in database... *****/
         if (Ctr_CheckIfCtrNameExistsInIns (ParamName,NewCtrName,Ctr->CtrCod,Gbl.Hierarchy.Ins.InsCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_centre_X_already_exists,
			     NewCtrName);
         else
           {
            /* Update the table changing old name by new name */
            Ctr_UpdateInsNameDB (Ctr->CtrCod,FieldName,NewCtrName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_centre_X_has_been_renamed_as_Y,
                             CurrentCtrName,NewCtrName);

	    /* Change current centre name in order to display it properly */
	    Str_Copy (CurrentCtrName,NewCtrName,
	              MaxBytes);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_of_the_centre_X_has_not_changed,
                          CurrentCtrName);
     }
  }

/*****************************************************************************/
/********************* Check if the name of centre exists ********************/
/*****************************************************************************/

static bool Ctr_CheckIfCtrNameExistsInIns (const char *FieldName,const char *Name,long CtrCod,long InsCod)
  {
   /***** Get number of centres with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a centre"
			  " already existed",
			  "SELECT COUNT(*) FROM centres"
			  " WHERE InsCod=%ld AND %s='%s' AND CtrCod<>%ld",
			  InsCod,FieldName,Name,CtrCod) != 0);
  }

/*****************************************************************************/
/****************** Update centre name in table of centres *******************/
/*****************************************************************************/

static void Ctr_UpdateInsNameDB (long CtrCod,const char *FieldName,const char *NewCtrName)
  {
   /***** Update centre changing old name by new name */
   DB_QueryUPDATE ("can not update the name of a centre",
		   "UPDATE centres SET %s='%s' WHERE CtrCod=%ld",
	           FieldName,NewCtrName,CtrCod);
  }

/*****************************************************************************/
/************************* Change the URL of a centre ************************/
/*****************************************************************************/

void Ctr_ChangeCtrWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   extern const char *Txt_You_can_not_leave_the_web_address_empty;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Get the code of the centre *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);

   /***** Get the new WWW for the centre *****/
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get data of centre *****/
   Ctr_GetDataOfCentreByCod (Ctr_EditingCtr);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ctr_UpdateCtrWWWDB (Ctr_EditingCtr->CtrCod,NewWWW);
      Str_Copy (Ctr_EditingCtr->WWW,NewWWW,
		Cns_MAX_BYTES_WWW);

      /***** Write message to show the change made
	     and put button to go to centre changed *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_web_address_is_X,
		       NewWWW);
     }
   else
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_leave_the_web_address_empty);
  }

void Ctr_ChangeCtrWWWInConfig (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   extern const char *Txt_You_can_not_leave_the_web_address_empty;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Get parameters from form *****/
   /* Get the new WWW for the centre */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ctr_UpdateCtrWWWDB (Gbl.Hierarchy.Ctr.CtrCod,NewWWW);
      Str_Copy (Gbl.Hierarchy.Ctr.WWW,NewWWW,
                Cns_MAX_BYTES_WWW);

      /***** Write message to show the change made *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_new_web_address_is_X,
		     NewWWW);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_the_web_address_empty);

   /***** Show the form again *****/
   Ctr_ShowConfiguration ();
  }

/*****************************************************************************/
/**************** Update database changing old WWW by new WWW ****************/
/*****************************************************************************/

static void Ctr_UpdateCtrWWWDB (long CtrCod,
                                const char NewWWW[Cns_MAX_BYTES_WWW + 1])
  {
   /***** Update database changing old WWW by new WWW *****/
   DB_QueryUPDATE ("can not update the web of a centre",
		   "UPDATE centres SET WWW='%s' WHERE CtrCod=%ld",
	           NewWWW,CtrCod);
  }

/*****************************************************************************/
/*********************** Change the status of a centre ***********************/
/*****************************************************************************/

void Ctr_ChangeCtrStatus (void)
  {
   extern const char *Txt_The_status_of_the_centre_X_has_changed;
   Ctr_Status_t Status;
   Ctr_StatusTxt_t StatusTxt;

   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Get centre code *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);

   /***** Get parameter with status *****/
   Status = (Ctr_Status_t)
	    Par_GetParToUnsignedLong ("Status",
				      0,
				      (unsigned long) Ctr_MAX_STATUS,
				      (unsigned long) Ctr_WRONG_STATUS);
   if (Status == Ctr_WRONG_STATUS)
      Lay_ShowErrorAndExit ("Wrong status.");
   StatusTxt = Ctr_GetStatusTxtFromStatusBits (Status);
   Status = Ctr_GetStatusBitsFromStatusTxt (StatusTxt);	// New status

   /***** Get data of centre *****/
   Ctr_GetDataOfCentreByCod (Ctr_EditingCtr);

   /***** Update status in table of centres *****/
   DB_QueryUPDATE ("can not update the status of a centre",
		   "UPDATE centres SET Status=%u WHERE CtrCod=%ld",
	           (unsigned) Status,Ctr_EditingCtr->CtrCod);
   Ctr_EditingCtr->Status = Status;

   /***** Write message to show the change made
	  and put button to go to centre changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_status_of_the_centre_X_has_changed,
	            Ctr_EditingCtr->ShrtName);
  }

/*****************************************************************************/
/********* Show alerts after changing a centre and continue editing **********/
/*****************************************************************************/

void Ctr_ContEditAfterChgCtr (void)
  {
   /***** Write message to show the change made
	  and put button to go to centre changed *****/
   Ctr_ShowAlertAndButtonToGoToCtr ();

   /***** Show the form again *****/
   Ctr_EditCentresInternal ();

   /***** Centre destructor *****/
   Ctr_EditingCentreDestructor ();
  }

/*****************************************************************************/
/***************** Write message to show the change made  ********************/
/***************** and put button to go to centre changed ********************/
/*****************************************************************************/

static void Ctr_ShowAlertAndButtonToGoToCtr (void)
  {
   extern const char *Txt_Go_to_X;

   // If the centre being edited is different to the current one...
   if (Ctr_EditingCtr->CtrCod != Gbl.Hierarchy.Ctr.CtrCod)
     {
      /***** Alert with button to go to centre *****/
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        Txt_Go_to_X,
		Ctr_EditingCtr->ShrtName);
      Ale_ShowLastAlertAndButton (ActSeeDeg,NULL,NULL,Ctr_PutParamGoToCtr,
                                  Btn_CONFIRM_BUTTON,Gbl.Title);
     }
   else
      /***** Alert *****/
      Ale_ShowAlerts (NULL);
  }

static void Ctr_PutParamGoToCtr (void)
  {
   Ctr_PutParamCtrCod (Ctr_EditingCtr->CtrCod);
  }

/*****************************************************************************/
/*********** Show a form for sending a logo of the current centre ************/
/*****************************************************************************/

void Ctr_RequestLogo (void)
  {
   Log_RequestLogo (Hie_CTR);
  }

/*****************************************************************************/
/***************** Receive the logo of the current centre ********************/
/*****************************************************************************/

void Ctr_ReceiveLogo (void)
  {
   Log_ReceiveLogo (Hie_CTR);
  }

/*****************************************************************************/
/****************** Remove the logo of the current centre ********************/
/*****************************************************************************/

void Ctr_RemoveLogo (void)
  {
   Log_RemoveLogo (Hie_CTR);
  }

/*****************************************************************************/
/*********** Show a form for sending a photo of the current centre ***********/
/*****************************************************************************/

void Ctr_RequestPhoto (void)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Photo;
   extern const char *Txt_Recommended_aspect_ratio;
   extern const char *Txt_Recommended_resolution;
   extern const char *Txt_XxY_pixels_or_higher;
   extern const char *Txt_File_with_the_photo;

   /***** Begin form to upload photo *****/
   Frm_StartForm (ActRecCtrPho);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Photo,NULL,
                 NULL,Box_NOT_CLOSABLE);

   /***** Write help message *****/
   Ale_ShowAlert (Ale_INFO,"%s: %s<br />"
		           "%s: %u&times;%u %s",
		  Txt_Recommended_aspect_ratio,
		  Ctr_RECOMMENDED_ASPECT_RATIO,
		  Txt_Recommended_resolution,
		  Ctr_PHOTO_SAVED_MAX_WIDTH,
		  Ctr_PHOTO_SAVED_MAX_HEIGHT,
		  Txt_XxY_pixels_or_higher);

   /***** Upload photo *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">"
	              "%s:&nbsp;"
                      "<input type=\"file\" name=\"%s\" accept=\"image/*\""
                      " onchange=\"document.getElementById('%s').submit();\" />",
            The_ClassFormInBox[Gbl.Prefs.Theme],
            Txt_File_with_the_photo,
            Fil_NAME_OF_PARAM_FILENAME_ORG,
            Gbl.Form.Id);
   HTM_LABEL_End ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/****************** Receive a photo of the current centre ********************/
/*****************************************************************************/

void Ctr_ReceivePhoto (void)
  {
   extern const char *Txt_Wrong_file_type;
   char Path[PATH_MAX + 1];
   struct Param *Param;
   char FileNameImgSrc[PATH_MAX + 1];
   char *PtrExtension;
   size_t LengthExtension;
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   char PathFileImgTmp[PATH_MAX + 1];	// Full name (including path and .jpg) of the destination temporary file
   char PathFileImg[PATH_MAX + 1];	// Full name (including path and .jpg) of the destination file
   bool WrongType = false;
   char Command[1024 + PATH_MAX * 2];
   int ReturnCode;
   char ErrorMsg[256];

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
      Ale_ShowAlert (Ale_WARNING,Txt_Wrong_file_type);
      return;
     }

   /***** Create private directories if not exist *****/
   /* Create private directory for images if it does not exist */
   Fil_CreateDirIfNotExists (Cfg_PATH_MEDIA_PRIVATE);

   /* Create temporary private directory for images if it does not exist */
   Fil_CreateDirIfNotExists (Cfg_PATH_MEDIA_TMP_PRIVATE);

   /* Get filename extension */
   if ((PtrExtension = strrchr (FileNameImgSrc,(int) '.')) == NULL)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_Wrong_file_type);
      return;
     }
   LengthExtension = strlen (PtrExtension);
   if (LengthExtension < Fil_MIN_BYTES_FILE_EXTENSION ||
       LengthExtension > Fil_MAX_BYTES_FILE_EXTENSION)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_Wrong_file_type);
      return;
     }

   /* End the reception of image in a temporary file */
   snprintf (PathFileImgTmp,sizeof (PathFileImgTmp),
	     "%s/%s.%s",
             Cfg_PATH_MEDIA_TMP_PRIVATE,Gbl.UniqueNameEncrypted,PtrExtension);
   if (!Fil_EndReceptionOfFile (PathFileImgTmp,Param))
     {
      Ale_ShowAlert (Ale_WARNING,"Error copying file.");
      return;
     }

   /***** Creates public directories if not exist *****/
   Fil_CreateDirIfNotExists (Cfg_PATH_CTR_PUBLIC);
   snprintf (Path,sizeof (Path),
	     "%s/%02u",
	     Cfg_PATH_CTR_PUBLIC,
	     (unsigned) (Gbl.Hierarchy.Ctr.CtrCod % 100));
   Fil_CreateDirIfNotExists (Path);
   snprintf (Path,sizeof (Path),
	     "%s/%02u/%u",
	     Cfg_PATH_CTR_PUBLIC,
	     (unsigned) (Gbl.Hierarchy.Ctr.CtrCod % 100),
	     (unsigned) Gbl.Hierarchy.Ctr.CtrCod);
   Fil_CreateDirIfNotExists (Path);

   /***** Convert temporary file to public JPEG file *****/
   snprintf (PathFileImg,sizeof (PathFileImg),
	     "%s/%02u/%u/%u.jpg",
	     Cfg_PATH_CTR_PUBLIC,
	     (unsigned) (Gbl.Hierarchy.Ctr.CtrCod % 100),
	     (unsigned) Gbl.Hierarchy.Ctr.CtrCod,
	     (unsigned) Gbl.Hierarchy.Ctr.CtrCod);

   /* Call to program that makes the conversion */
   snprintf (Command,sizeof (Command),
	     "convert %s -resize '%ux%u>' -quality %u %s",
             PathFileImgTmp,
             Ctr_PHOTO_SAVED_MAX_WIDTH,
             Ctr_PHOTO_SAVED_MAX_HEIGHT,
             Ctr_PHOTO_SAVED_QUALITY,
             PathFileImg);
   ReturnCode = system (Command);
   if (ReturnCode == -1)
      Lay_ShowErrorAndExit ("Error when running command to process image.");

   /***** Write message depending on return code *****/
   ReturnCode = WEXITSTATUS(ReturnCode);
   if (ReturnCode != 0)
     {
      snprintf (ErrorMsg,sizeof (ErrorMsg),
	        "Image could not be processed successfully.<br />"
		"Error code returned by the program of processing: %d",
	        ReturnCode);
      Lay_ShowErrorAndExit (ErrorMsg);
     }

   /***** Remove temporary file *****/
   unlink (PathFileImgTmp);

   /***** Show the centre information again *****/
   Ctr_ShowConfiguration ();
  }

/*****************************************************************************/
/**************** Change the attribution of a centre photo *******************/
/*****************************************************************************/

void Ctr_ChangeCtrPhotoAttribution (void)
  {
   char NewPhotoAttribution[Med_MAX_BYTES_ATTRIBUTION + 1];

   /***** Get parameters from form *****/
   /* Get the new photo attribution for the centre */
   Par_GetParToText ("Attribution",NewPhotoAttribution,Med_MAX_BYTES_ATTRIBUTION);

   /***** Update the table changing old attribution by new attribution *****/
   DB_QueryUPDATE ("can not update the photo attribution"
		   " of the current centre",
		   "UPDATE centres SET PhotoAttribution='%s'"
		   " WHERE CtrCod=%ld",
	           NewPhotoAttribution,Gbl.Hierarchy.Ctr.CtrCod);

   /***** Show the centre information again *****/
   Ctr_ShowConfiguration ();
  }

/*****************************************************************************/
/********************* Put a form to create a new centre *********************/
/*****************************************************************************/

static void Ctr_PutFormToCreateCentre (void)
  {
   extern const char *Txt_New_centre;
   extern const char *Txt_Another_place;
   extern const char *Txt_Create_centre;
   unsigned NumPlc;

   /***** Begin form *****/
   if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      Frm_StartForm (ActNewCtr);
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      Frm_StartForm (ActReqCtr);
   else
      Lay_NoPermissionExit ();

   /***** Begin box and table *****/
   Box_StartBoxTable (NULL,Txt_New_centre,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Ctr_PutHeadCentresForEdition ();

   HTM_TR_Begin (NULL);

   /***** Column to remove centre, disabled here *****/
   HTM_TD_Begin ("class=\"BM\"");
   HTM_TD_End ();

   /***** Centre code *****/
   HTM_TD_Begin ("class=\"CODE\"");
   HTM_TD_End ();

   /***** Centre logo *****/
   HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Ctr_EditingCtr->FullName);
   Log_DrawLogo (Hie_CTR,-1L,"",20,NULL,true);
   HTM_TD_End ();

   /***** Place *****/
   HTM_TD_Begin ("class=\"LM\"");
   fprintf (Gbl.F.Out,"<select name=\"PlcCod\" class=\"PLC_SEL\">"
                      "<option value=\"0\"");
   if (Ctr_EditingCtr->PlcCod == 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",Txt_Another_place);
   for (NumPlc = 0;
	NumPlc < Gbl.Plcs.Num;
	NumPlc++)
      fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s</option>",
               Gbl.Plcs.Lst[NumPlc].PlcCod,
               (Gbl.Plcs.Lst[NumPlc].PlcCod == Ctr_EditingCtr->PlcCod) ? " selected=\"selected\"" :
        	                                                         "",
               Gbl.Plcs.Lst[NumPlc].ShrtName);
   fprintf (Gbl.F.Out,"</select>");
   HTM_TD_End ();

   /***** Centre short name *****/
   HTM_TD_Begin ("class=\"LM\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_SHORT_NAME\""
                      " required=\"required\" />",
            Hie_MAX_CHARS_SHRT_NAME,Ctr_EditingCtr->ShrtName);
   HTM_TD_End ();

   /***** Centre full name *****/
   HTM_TD_Begin ("class=\"LM\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_FULL_NAME\""
                      " required=\"required\" />",
            Hie_MAX_CHARS_FULL_NAME,Ctr_EditingCtr->FullName);
   HTM_TD_End ();

   /***** Centre WWW *****/
   HTM_TD_Begin ("class=\"LM\"");
   fprintf (Gbl.F.Out,"<input type=\"url\" name=\"WWW\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_WWW\""
                      " required=\"required\" />",
            Cns_MAX_CHARS_WWW,Ctr_EditingCtr->WWW);
   HTM_TD_End ();

   /***** Number of users who claim to belong to this centre *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   fprintf (Gbl.F.Out,"0");
   HTM_TD_End ();

   /***** Number of degrees *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   fprintf (Gbl.F.Out,"0");
   HTM_TD_End ();

   /***** Number of users in courses of this centre *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   fprintf (Gbl.F.Out,"0");
   HTM_TD_End ();

   /***** Centre requester *****/
   HTM_TD_Begin ("class=\"DAT INPUT_REQUESTER LT\"");
   Msg_WriteMsgAuthor (&Gbl.Usrs.Me.UsrDat,true,NULL);
   HTM_TD_End ();

   /***** Centre status *****/
   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_centre);

   /***** End form *****/
   Frm_EndForm ();
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
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   Ctr_Order_t Order;

   HTM_TR_Begin (NULL);

   HTM_TH_Empty (1);

   for (Order = Ctr_ORDER_BY_CENTRE;
	Order <= Ctr_ORDER_BY_NUM_TCHS;
	Order++)
     {
      HTM_TH_Begin (1,1,Order == Ctr_ORDER_BY_CENTRE ? "LM" :
						       "RM");
      if (OrderSelectable)
	{
	 Frm_StartForm (ActSeeCtr);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Frm_LinkFormSubmit (Txt_CENTRES_HELP_ORDER[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Hierarchy.Ins.Ctrs.SelectedOrder)
	    fprintf (Gbl.F.Out,"<u>");
	}
      fprintf (Gbl.F.Out,"%s",Txt_CENTRES_ORDER[Order]);
      if (OrderSelectable)
	{
	 if (Order == Gbl.Hierarchy.Ins.Ctrs.SelectedOrder)
	    fprintf (Gbl.F.Out,"</u>");
	 Frm_LinkFormEnd ();
	 Frm_EndForm ();
	}
      HTM_TH_End ();
     }

   HTM_TH (1,1,"LM",Txt_Place);
   HTM_TH (1,1,"RM",Txt_Degrees_ABBREVIATION);
   HTM_TH (1,1,"RM",Txt_Courses_ABBREVIATION);
   HTM_TH_Begin (1,1,"RM");
   fprintf (Gbl.F.Out,"%s+<br />%s",
            Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH],
            Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
   HTM_TH_End ();
   HTM_TH_Empty (1);

   HTM_TR_End ();
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
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Requester;

   HTM_TR_Begin (NULL);

   HTM_TH_Empty (1);
   HTM_TH (1,1,"RM",Txt_Code);
   HTM_TH_Empty (1);
   HTM_TH (1,1,"LM",Txt_Place);
   HTM_TH (1,1,"LM",Txt_Short_name_of_the_centre);
   HTM_TH (1,1,"LM",Txt_Full_name_of_the_centre);
   HTM_TH (1,1,"LM",Txt_WWW);
   HTM_TH (1,1,"RM",Txt_Users);
   HTM_TH (1,1,"RM",Txt_Degrees_ABBREVIATION);
   HTM_TH_Begin (1,1,"RM");
   fprintf (Gbl.F.Out,"%s+<br />%s",
	    Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH],
            Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
   HTM_TH_End ();
   HTM_TH (1,1,"LM",Txt_Requester);
   HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Receive form to request a new centre *********************/
/*****************************************************************************/

void Ctr_RecFormReqCtr (void)
  {
   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Receive form to request a new centre *****/
   Ctr_RecFormRequestOrCreateCtr ((unsigned) Ctr_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/******************* Receive form to create a new centre *********************/
/*****************************************************************************/

void Ctr_RecFormNewCtr (void)
  {
   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Receive form to create a new centre *****/
   Ctr_RecFormRequestOrCreateCtr (0);
  }

/*****************************************************************************/
/************* Receive form to request or create a new centre ****************/
/*****************************************************************************/

static void Ctr_RecFormRequestOrCreateCtr (unsigned Status)
  {
   extern const char *Txt_The_centre_X_already_exists;
   extern const char *Txt_Created_new_centre_X;
   extern const char *Txt_You_must_specify_the_web_address_of_the_new_centre;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_centre;

   /***** Get parameters from form *****/
   /* Set centre institution */
   Ctr_EditingCtr->InsCod = Gbl.Hierarchy.Ins.InsCod;

   /* Get place */
   if ((Ctr_EditingCtr->PlcCod = Plc_GetParamPlcCod ()) < 0)	// 0 is reserved for "other place"
      Ale_ShowAlert (Ale_ERROR,"Wrong place.");

   /* Get centre short name */
   Par_GetParToText ("ShortName",Ctr_EditingCtr->ShrtName,Hie_MAX_BYTES_SHRT_NAME);

   /* Get centre full name */
   Par_GetParToText ("FullName",Ctr_EditingCtr->FullName,Hie_MAX_BYTES_FULL_NAME);

   /* Get centre WWW */
   Par_GetParToText ("WWW",Ctr_EditingCtr->WWW,Cns_MAX_BYTES_WWW);

   if (Ctr_EditingCtr->ShrtName[0] &&
       Ctr_EditingCtr->FullName[0])	// If there's a centre name
     {
      if (Ctr_EditingCtr->WWW[0])
        {
         /***** If name of centre was in database... *****/
         if (Ctr_CheckIfCtrNameExistsInIns ("ShortName",Ctr_EditingCtr->ShrtName,-1L,Gbl.Hierarchy.Ins.InsCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_centre_X_already_exists,
                             Ctr_EditingCtr->ShrtName);
         else if (Ctr_CheckIfCtrNameExistsInIns ("FullName",Ctr_EditingCtr->FullName,-1L,Gbl.Hierarchy.Ins.InsCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        		     Txt_The_centre_X_already_exists,
                             Ctr_EditingCtr->FullName);
         else	// Add new centre to database
           {
            Ctr_CreateCentre (Status);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
			     Txt_Created_new_centre_X,
			     Ctr_EditingCtr->FullName);
           }
        }
      else	// If there is not a web
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_You_must_specify_the_web_address_of_the_new_centre);
     }
   else	// If there is not a centre name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_centre);
  }

/*****************************************************************************/
/***************************** Create a new centre ***************************/
/*****************************************************************************/

static void Ctr_CreateCentre (unsigned Status)
  {
   /***** Create a new centre *****/
   Ctr_EditingCtr->CtrCod =
   DB_QueryINSERTandReturnCode ("can not create a new centre",
				"INSERT INTO centres"
				" (InsCod,PlcCod,Status,RequesterUsrCod,"
				"ShortName,FullName,WWW,PhotoAttribution)"
				" VALUES"
				" (%ld,%ld,%u,%ld,"
				"'%s','%s','%s','')",
				Ctr_EditingCtr->InsCod,
				Ctr_EditingCtr->PlcCod,
				Status,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Ctr_EditingCtr->ShrtName,
				Ctr_EditingCtr->FullName,
				Ctr_EditingCtr->WWW);
  }

/*****************************************************************************/
/************************** Get number of centres ****************************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsTotal (void)
  {
   /***** Get total number of centres from database *****/
   return (unsigned) DB_GetNumRowsTable ("centres");
  }

/*****************************************************************************/
/******************* Get number of centres in a country **********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsInCty (long CtyCod)
  {
   /***** Get number of centres of a country from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of centres in a country",
			     "SELECT COUNT(*) FROM institutions,centres"
			     " WHERE institutions.CtyCod=%ld"
			     " AND institutions.InsCod=centres.InsCod",
			     CtyCod);
  }

/*****************************************************************************/
/**************** Get number of centres in an institution ********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsInIns (long InsCod)
  {
   /***** Get number of centres of an institution from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of centres in an institution",
			     "SELECT COUNT(*) FROM centres"
			     " WHERE InsCod=%ld",
			     InsCod);
  }

/*****************************************************************************/
/******* Get number of centres (of the current institution) in a place *******/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsInPlc (long PlcCod)
  {
   /***** Get number of centres (of the current institution) in a place *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get the number of centres in a place",
			     "SELECT COUNT(*) FROM centres"
			     " WHERE InsCod=%ld AND PlcCod=%ld",
			     Gbl.Hierarchy.Ins.InsCod,PlcCod);
  }

/*****************************************************************************/
/********************* Get number of centres with degrees ********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsWithDegs (const char *SubQuery)
  {
   /***** Get number of centres with degrees from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of centres with degrees",
			     "SELECT COUNT(DISTINCT centres.CtrCod)"
			     " FROM institutions,centres,degrees"
			     " WHERE %sinstitutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod",
			     SubQuery);
  }

/*****************************************************************************/
/********************* Get number of centres with courses ********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsWithCrss (const char *SubQuery)
  {
   /***** Get number of centres with courses from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of centres with courses",
			     "SELECT COUNT(DISTINCT centres.CtrCod)"
			     " FROM institutions,centres,degrees,courses"
			     " WHERE %sinstitutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod"
			     " AND degrees.DegCod=courses.DegCod",
			     SubQuery);
  }

/*****************************************************************************/
/********************* Get number of centres with users **********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsWithUsrs (Rol_Role_t Role,const char *SubQuery)
  {
   /***** Get number of centres with users from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of centres with users",
			     "SELECT COUNT(DISTINCT centres.CtrCod)"
			     " FROM institutions,centres,degrees,courses,crs_usr"
			     " WHERE %sinstitutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod"
			     " AND degrees.DegCod=courses.DegCod"
			     " AND courses.CrsCod=crs_usr.CrsCod"
			     " AND crs_usr.Role=%u",
			     SubQuery,(unsigned) Role);
  }

/*****************************************************************************/
/****************************** List centres found ***************************/
/*****************************************************************************/

void Ctr_ListCtrsFound (MYSQL_RES **mysql_res,unsigned NumCtrs)
  {
   extern const char *Txt_centre;
   extern const char *Txt_centres;
   MYSQL_ROW row;
   unsigned NumCtr;
   struct Centre Ctr;

   /***** Query database *****/
   if (NumCtrs)
     {
      /***** Begin box and table *****/
      /* Number of centres found */
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        "%u %s",
                NumCtrs,(NumCtrs == 1) ? Txt_centre :
	                                 Txt_centres);
      Box_StartBoxTable (NULL,Gbl.Title,NULL,
                         NULL,Box_NOT_CLOSABLE,2);

      /***** Write heading *****/
      Ctr_PutHeadCentresForSeeing (false);	// Order not selectable

      /***** List the centres (one row per centre) *****/
      for (NumCtr = 1;
	   NumCtr <= NumCtrs;
	   NumCtr++)
	{
	 /* Get next centre */
	 row = mysql_fetch_row (*mysql_res);

	 /* Get centre code (row[0]) */
	 Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get data of centre */
	 Ctr_GetDataOfCentreByCod (&Ctr);

	 /* Write data of this centre */
	 Ctr_ListOneCentreForSeeing (&Ctr,NumCtr);
	}

      /***** End table and box *****/
      Box_EndBoxTable ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************************ Centre constructor/destructor **********************/
/*****************************************************************************/

static void Ctr_EditingCentreConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Ctr_EditingCtr != NULL)
      Lay_ShowErrorAndExit ("Error initializing centre.");

   /***** Allocate memory for centre *****/
   if ((Ctr_EditingCtr = (struct Centre *) malloc (sizeof (struct Centre))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for centre.");

   /***** Reset centre *****/
   Ctr_EditingCtr->CtrCod          = -1L;
   Ctr_EditingCtr->InsCod          = -1L;
   Ctr_EditingCtr->PlcCod          = -1L;
   Ctr_EditingCtr->RequesterUsrCod = -1L;
   Ctr_EditingCtr->ShrtName[0]     = '\0';
   Ctr_EditingCtr->FullName[0]     = '\0';
   Ctr_EditingCtr->WWW[0]          = '\0';
   Ctr_EditingCtr->Degs.Num        = 0;
   Ctr_EditingCtr->Degs.Lst        = NULL;
   Ctr_EditingCtr->NumCrss         = 0;
   Ctr_EditingCtr->NumUsrs         = 0;
   Ctr_EditingCtr->NumUsrsWhoClaimToBelongToCtr = 0;
  }

static void Ctr_EditingCentreDestructor (void)
  {
   /***** Free memory used for centre *****/
   if (Ctr_EditingCtr != NULL)
     {
      free ((void *) Ctr_EditingCtr);
      Ctr_EditingCtr = NULL;
     }
  }
